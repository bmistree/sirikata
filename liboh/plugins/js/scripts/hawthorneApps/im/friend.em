
system.require('hawthorneApps/im/imUtil.em');
system.require('hawthorneApps/im/convGUI.em');


(function()
{
    //constants
    var REGISTRATION               =  0;
    var CONNECTED_CONV             =  1;
    var OTHER_SIDE_NON_RESPONSIVE  =  2;
    
    var REGISTRATION_TIMEOUT       = 20;
    var MESSAGE_TIMEOUT            =  5;


    /**
     @param{String-tainted} name - Self-reported name of the friend
     that will appear in friend's list.
 
     @param{Visible} vis - Visible object corresponding to presence of
     friend.

     @param{ApplicationGUI} appGui - Gui object for application.  @see
     appGui.em

     @param{unique int} imID - Each friend has a unique id associated
     with it.
     */
    Friend = function(name, vis, appGui, imID)
    {
        //how the friend name will be displayed on the friend's list.
        this.name       = IMUtil.htmlEscape(name);
        this.vis        = vis;
        this.appGui     = appGui;
        //starts out not in a conversation with friend
        this.convGUI    = null;

        //friendID: note maybe could just use the visible id.
        this.imID       = imID;
        
        this.statusToFriend = this.appGui.getStatusPresenting(this.imID);
        this.statusFromFriend  = "Registering";

        this.profileToFriend = this.appGui.getProfilePresenting(this.imID);
        this.profileFromFriend = "Registering";
        
        this.topicsDiscussed = [];

        this.appGui.display(this.imID);

        //connection status
        this.connStatus = REGISTRATION;
        this.beginRegistration();
        this.setupMessageListeners();
    };



    /**
     @param {message object} msg from sender corresponding to this.vis.
     Replies with a message that should satisfy regResponseMsg.
     */
    Friend.prototype.processRegReqMsg = function (msg)
    {
        var replyPart = {
            'status':  this.statusToFriend,
            'profile': this.profileToFriend
        };
        msg.makeReply(replyPart) >> [];
    };


    /**
     Called when do not receive a response for a registration message
     from within @see Friend.prototype.beginRegistration.
     */
    function noRegResponse()
    {
        this.connStatus = OTHER_SIDE_NON_RESPONSIVE;
        this.appGui.display(this.imID);
    }


    /**
     Called when receive a response for a registration message from
     within @see Friend.prototype.beginRegistration.
     */
    function regResponse(msg,sender)
    {
        if (regResponseMsg(msg))
        {
            //registration response message was correctly formatted
            //and accepted.
            this.connStatus = CONNECTED_CONV;
            this.statusFromFriend = IMUtil.htmlEscape(msg.status);
            this.profileFromFriend = IMUtil.htmlEscape(msg.profile);
            this.appGui.display(this.imID);
        }
        else
        {
            //if cannot parse message as a registration response, or
            //if registration was declined, then
            //just treat as if we got no registration response.
            noRegResponse();                
        }
    }

    
    /**
     Helper function that returns true if the msg received is
     correctly formatted with status and profile fields that are
     strings.  Otherwise, returns false.  Called from @see
     Friend.prototype.beginRegistration.
     */
    function regResponseMsg(msg)
    {
        return ((typeof(msg.status) == 'string') &&
                (typeof(msg.profile) == 'string'));
    }

    
    /**
     Sends registration request to other side.  If do not receive a
     response from the other side within time period, connStatus
     transitions from REGISTRATION to OTHER_SIDE_NON_RESPONSIVE.

     If receive a reg accepted response, then update status, etc. of
     other side with info from that message.
     */
    Friend.prototype.beginRegistration = function()
    {

        var wrappedNoRegResponse = std.core.bind(
            noRegResponse,this);


        var wrappedRegResponse = std.core.bind(
            regResponse,this);
        
        { 'imRegRequest': 1}  >> this.vis >>
            [ wrappedRegResponse, REGISTRATION_TIMEOUT, wrappedNoRegResponse];

    };

    
    /**
     Called when we send a chat message to a friend, and the friend
     does not acknowledge it.  Called from @see
     Friend.prototype.msgToFriend.
     */
    function msgUnacked(msgToSend)
    {
        if (this.convGUI !== null)
        {
            this.convGUI.warn('Message: "' +
                              IMUtil.htmlEscape(msgToSend) +
                              '" went undelivered.  Friend'+
                              ' is no longer available.'
                             );
        }
        else
        {
            this.appGUI.warn('Some messages to ' + this.name +
                             ' may not have been delivered.');
        }
        this.status = OTHER_SIDE_NON_RESPONSIVE;
        this.appGui.display(this.imID);
    }

    

    /**
     @param {String} msgToSend: tainted.  Need to de-taint before
     sending.

     Regardless of the state of the connection between self and
     friend, tries to set up
     */
    Friend.prototype.msgToFriend = function(msgToSend)
    {
        if ((this.connStatus == OTHER_SIDE_NON_RESPONSIVE) ||
            (this.connStatus == REGISTRATION))
        {
            var warnMsg = 'Cannot message ' + this.name;
            warnMsg += (this.connStatus == REGISTRATION) ?
                ' until your connection is registered.' :
                ' when he/she is offline.';
        
            this.appGui.warn(warnMsg);
            return;
        }


        //if we do not already have a pre-existing conversation, then
        //create a conversation gui
        if (this.convGUI === null)
            this.convGUI = new ConvGUI(this.name,this);                


        //output to the conversation gui.
        this.convGUI.writeMe(IMUtil.htmlEscape(msgToSend));

        var wrappedMsgUnacked = std.core.bind(
            msgUnacked,this,msgToSend);
        
        //send the message to the other side & output it to
        //conversation gui
        {'imMsg': msgToSend} >> this.vis >>
            [ function(){},MESSAGE_TIMEOUT,wrappedMsgUnacked];
    };

    /**
     */
    Friend.prototype.kill = function()
    {
        this.appGui.remove(this.imID);
    
        if (this.msgHandler !== null )
        {
            this.msgHandler.clear();
            this.msgHandler = null;
        }

        if (this.statusUpdateHandler !== null)
        {
            this.statusUpdateHandler.clear();
            this.statusUpdateHandler = null;
        }

        if (this.profUpdateHandler !== null)
        {
            this.profUpdateHandler.clear();
            this.profUpdateHandler = null;
        }
    };



    /**
     Called when receive a message to display from friend from @see
     Friend.prototype.setupMessageListeners.
     */
    function handleMessage(msg,sender)
    {
        if (typeof(msg.imMsg) != 'string')
            return;
            
        //if we've put this friend into an invisible group, do nothing.
        if (!this.appGui.getIsVisibleTo(this.imID))
            return;
            
        //If we don't already have a conversation gui, create one.
        if (this.convGUI === null)
            this.convGUI = new ConvGUI(this.name,this);
            
        //output to the conversation gui.
        this.convGUI.writeFriend(IMUtil.htmlEscape(msg.imMsg));

        //send ack back to other side.
        msg.makeReply({'imMsgAck': 1}) >> [];
    }

    /**
     What to do when receive a status message from friend; called
     from @see Friend.prototype.setupMessageListeners.
     */
    function handleStatusMessage(msg,sender)
    {
        if (typeof(msg.imStatus) != 'string')
            return;

        this.statusFromFriend = msg.imStatus;
        this.appGui.display(this.imID);
    }


    /**
     Called when receive a profile update from friend; called
     from @see Friend.prototype.setupMessageListeners
     */
    function handleProfMessage(msg,sender)
    {
        if (typeof(msg.imProf) != 'string')
            return;

        this.profileFromFriend = msg.imProf;
        this.appGui.display(this.imID);
    }


    
    /**
     Sets up listeners for profile changes, status changes, and
     regular conversation messages.
     */
    Friend.prototype.setupMessageListeners = function()
    {
        //handler for receiving conversation messages
        var wrappedHandleMessage = std.core.bind(
            handleMessage,this);

        this.msgHandler = wrappedHandleMessage <<{'imMsg'::} << this.vis;


        //handler for status updates
        var wrappedHandleStatusMessage = std.core.bind(
            handleStatusMessage,this);
    
        this.statusUpdateHandler = wrappedHandleStatusMessage <<
            {'imStatus'::} << this.vis;


        //handler for profile updates
        var wrappedHandleProfMessage = std.core.bind(
            handleProfMessage,this);
        
        
        this.profUpdateHandler = handleProfMessage <<{'imProf'::} << this.vis;
    };

    
    
})();


