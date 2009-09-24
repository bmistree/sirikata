#ifndef _CBR_SST_NETWORK_IMPL_H_
#define _CBR_SST_NETWORK_IMPL_H_

#include "Network.hpp"
#include "Statistics.hpp"
#include "stream.h"
#include "host.h"
#include <QApplication>
#include <map>
#include <queue>

using namespace SST;

namespace CBR {

class SSTStatsListener : public SST::StreamStatListener {
public:
    SSTStatsListener(Trace* trace, const QTime& start, const Address4& remote);
    virtual ~SSTStatsListener() {}

    void packetSent(qint32 size);
    void packetReceived(qint32 size);
private:
    Trace* mTrace;
    const QTime& mStartTime;
    Address4 mRemote;
};

class CBRSST : public QObject {
    Q_OBJECT
public:
    CBRSST(Trace* trace);
    ~CBRSST();
    void listen(uint32 port);

    // Checks if this chunk, when passed to send, would be successfully pushed.
    virtual bool canSend(const Address4&,uint32 size, bool reliable, bool ordered, int priority);
    bool send(const Address4& addy, const Network::Chunk& data, bool reliable, bool ordered, int priority);
    Network::Chunk* front(const Address4& from, uint32 max_size);
    Network::Chunk* receiveOne(const Address4& from, uint32 max_size);
    void service();
    void init(void* (*x)(void*));
    void start();

    void reportQueueInfo(const Time& t);
private slots:
    void handleConnection();

    void handleReadyReadMessage();
    void handleReadyReadDatagram();

    void handleNewSubstream();

    void handleReset();
    void handleInit();
private:
    struct NetworkChunk {
        Address4 addr;
        Network::Chunk data;
        uint32 bytes_sent;
    };

    struct StreamInfo {
        SST::Stream* stream;
        SSTStatsListener* stats;
        NetworkChunk* currentSendChunk;
        Network::Chunk* peek;
    };
    typedef std::map<Address4, StreamInfo> StreamMap;

    StreamInfo* lookupOrConnectSend(const Address4& addy);
    StreamInfo* lookupReceive(const Address4& addy);

    void trySendCurrentChunk(StreamInfo* si);

    Trace* mTrace;

    void *(*mMainCallback)(void*);
    QApplication* mApp;
    QTime mStartTime;
    SST::Host* mHost;
    SST::StreamServer* mAcceptor;

    StreamMap mSendConnections;
    StreamMap mReceiveConnections;

    Time mLastSampleTime;
    Duration mSampleRate;
}; // class CBRSST

} // namespace CBR

#endif //_CBR_SST_NETWORK_IMPL_H_
