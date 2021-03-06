/*  Sirikata
 *  MasterPintoServerQuerier.cpp
 *
 *  Copyright (c) 2010, Ewen Cheslack-Postava
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Sirikata nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "MasterPintoServerQuerier.hpp"

#include <sirikata/core/network/StreamFactory.hpp>
#include <sirikata/core/options/CommonOptions.hpp>
#include <sirikata/core/options/Options.hpp>

#include "Protocol_MasterPinto.pbj.hpp"

using namespace Sirikata::Network;

#define MP_LOG(lvl,msg) SILOG(masterpinto,lvl, msg);

namespace Sirikata {

MasterPintoServerQuerier::MasterPintoServerQuerier(SpaceContext* ctx, const String& params)
 : mContext(ctx),
   mIOStrand(ctx->ioService->createStrand("MasterPintoServerQuerier")),
   mConnecting(false),
   mConnected(false),
   mGaveID(false),
   mRegion(),
   mRegionDirty(true),
   mMaxRadius(0),
   mMaxRadiusDirty(true),
   mAggregateQuery(SolidAngle::Max),
   mAggregateQueryMaxResults(0),
   mAggregateQueryDirty(true)
{
    OptionSet* optionsSet = OptionSet::getOptions("space_master_pinto",NULL);
    optionsSet->parse(params);

    mHost = optionsSet->referenceOption(OPT_MASTER_PINTO_HOST)->as<String>();
    mPort = optionsSet->referenceOption(OPT_MASTER_PINTO_PORT)->as<String>();

    // Register us as a service for startup/shutdown
    mContext->add(this);
}

MasterPintoServerQuerier::~MasterPintoServerQuerier() {
    delete mServerStream;
    delete mIOStrand;
}

void MasterPintoServerQuerier::start() {
    connect();
}

void MasterPintoServerQuerier::connect() {
    if (mConnecting) return;

    mConnecting = true;

    OptionSet* optionsSet = OptionSet::getOptions("space_master_pinto",NULL);
    String server_protocol = optionsSet->referenceOption(OPT_MASTER_PINTO_PROTOCOL)->as<String>();
    String server_protocol_options = optionsSet->referenceOption(OPT_MASTER_PINTO_PROTOCOL_OPTIONS)->as<String>();
    OptionSet* server_protocol_optionset = StreamFactory::getSingleton().getOptionParser(server_protocol)(server_protocol_options);
    mServerStream = StreamFactory::getSingleton().getConstructor(server_protocol)(mIOStrand, server_protocol_optionset);

    using std::tr1::placeholders::_1;
    using std::tr1::placeholders::_2;

    Address serverAddress(mHost, mPort);
    MP_LOG(debug, "Trying to connect to " << serverAddress.toString());
    mServerStream->connect(
        serverAddress,
        &Network::Stream::ignoreSubstreamCallback,
        std::tr1::bind(&MasterPintoServerQuerier::handleServerConnection, this, _1, _2),
        std::tr1::bind(&MasterPintoServerQuerier::handleServerReceived, this, _1, _2),
        std::tr1::bind(&MasterPintoServerQuerier::handleServerReadySend, this)
    );
}

void MasterPintoServerQuerier::stop() {
    mServerStream->close();
    mConnected = false;
}

void MasterPintoServerQuerier::updateRegion(const BoundingBox3f& region) {
    MP_LOG(debug, "Updating region " << region);
    mRegion = region;
    mRegionDirty = true;
    mIOStrand->post(
        std::tr1::bind(&MasterPintoServerQuerier::tryServerUpdate, this),
        "MasterPintoServerQuerier::tryServerUpdate"
    );
}

void MasterPintoServerQuerier::updateLargestObject(float max_radius) {
    MP_LOG(debug, "Updating largest object " << max_radius);
    mMaxRadius = max_radius;
    mMaxRadiusDirty = true;
    mIOStrand->post(
        std::tr1::bind(&MasterPintoServerQuerier::tryServerUpdate, this),
        "MasterPintoServerQuerier::tryServerUpdate"
    );
}

void MasterPintoServerQuerier::updateQuery(const SolidAngle& min_angle, uint32 max_results) {
    MP_LOG(debug, "Updating aggregate query angle " << min_angle << " and " << max_results << " maximum results.");
    mAggregateQuery = min_angle;
    mAggregateQueryMaxResults = max_results;
    mAggregateQueryDirty = true;
    mIOStrand->post(
        std::tr1::bind(&MasterPintoServerQuerier::tryServerUpdate, this),
        "MasterPintoServerQuerier::tryServerUpdate"
    );
}

void MasterPintoServerQuerier::tryServerUpdate() {
    if (!mRegionDirty && !mMaxRadiusDirty && !mAggregateQueryDirty)
        return;

    if (!mConnected) {
        connect();
        return;
    }

    Sirikata::Protocol::MasterPinto::PintoMessage msg;

    if (!mGaveID) {
        mGaveID = true;
        Sirikata::Protocol::MasterPinto::IRegionIDUpdate update = msg.mutable_server();
        update.set_server(mContext->id());
    }

    if (mRegionDirty) {
        mRegionDirty = false;
        Sirikata::Protocol::MasterPinto::IRegionUpdate update = msg.mutable_region();
        update.set_bounds(mRegion.toBoundingSphere());
    }

    if (mMaxRadiusDirty) {
        mMaxRadiusDirty = false;
        Sirikata::Protocol::MasterPinto::ILargestObjectUpdate update = msg.mutable_largest();
        update.set_radius(mMaxRadius);
    }

    if (mAggregateQueryDirty) {
        mAggregateQueryDirty = false;
        Sirikata::Protocol::MasterPinto::IQueryUpdate update = msg.mutable_query();
        update.set_min_angle(mAggregateQuery.asFloat());
        update.set_max_count(mAggregateQueryMaxResults);
    }

    String serialized = serializePBJMessage(msg);
    mServerStream->send( MemoryReference(serialized), ReliableOrdered );
}

void MasterPintoServerQuerier::handleServerConnection(Network::Stream::ConnectionStatus status, const std::string &reason) {
    mConnecting = false;

    if (status == Network::Stream::Connected) {
        MP_LOG(debug, "Connected to master pinto server.");
        mConnected = true;
        tryServerUpdate();
    }
    else if (status == Network::Stream::ConnectionFailed) {
        MP_LOG(debug, "Connection to master pinto server failed.");
    }
    else if (status == Network::Stream::Disconnected) {
        MP_LOG(debug, "Disconnected from pinto server.");
        mConnected = false;
        mGaveID = false;
    }
}

void MasterPintoServerQuerier::handleServerReceived(Chunk& data, const Network::Stream::PauseReceiveCallback& pause) {
    Sirikata::Protocol::MasterPinto::PintoResponse msg;
    bool parsed = parsePBJMessage(&msg, data);

    if (!parsed) {
        MP_LOG(error, "Couldn't parse response from server.");
        return;
    }

    for(int32 idx = 0; idx < msg.update_size(); idx++) {
        Sirikata::Protocol::MasterPinto::PintoUpdate update = msg.update(idx);
        for(int32 i = 0; i < update.change_size(); i++) {
            Sirikata::Protocol::MasterPinto::PintoResult result = update.change(i);
            MP_LOG(debug, "Event received from master pinto: " << result.server() << (result.addition() ? " added" : " removed"));
            if (result.addition())
                notify(&PintoServerQuerierListener::addRelevantServer, result.server());
            else
                notify(&PintoServerQuerierListener::removeRelevantServer, result.server());
        }
    }
}

void MasterPintoServerQuerier::handleServerReadySend() {
}

} // namespace Sirikata
