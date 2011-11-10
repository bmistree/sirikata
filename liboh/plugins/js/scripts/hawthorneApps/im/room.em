
system.require('hawthorneApps/im/imUtil.em');
system.require('hawthorneApps/im/friend.em');

(function()
 {

     /**
      For each chat room, one presence controls chat room and
      coordinates it.  To all other presences, it looks just like a
      friend.  
      */
     

     /**
      @param {String-tainted} name - Self-reported name of room to
      create.

      @param {array} friendArray - An array of friend objects that we
      want to add to this group.  For each, we try to create a new
      connection, requesting them to join our group.

      @param {AppGui} appGui

      @param {unique int} rmID - Room ID.  
      */
     Room = function(name,friendArray,appGui,rmID)
     {
         this.name = name;

         this.friendArray = [];
         for (var s in friendArray)
         {
             var newFriend = new Friend(
                 friendArray[s].name,friendArray[s].vis,
                 this,IMUtil.getUniqueInt(),this,
                 Friend.RoomType.RoomCoordinator,null);
             
             this.friendArray.push(newFriend);

             //note: may need to change

             appGui.addRoomFriend(newFriend);

         }
         
         this.appGui = appGui;
         this.rmID   = rmID;
     };


     
     //appGui functionality used by friend

     /**
      called when a friend has changed status, initially created
      itself,changed its connection status (either open or closed),
      will need to change this so that sends messages to friends to
      filter friends that are no longer participating.
      */
     Room.prototype.display = function(imID)
     {
         IMUtil.dPrint('\nGot a request to display in Room.em\n');
     };

     /**
      @param {String - tainted} message to warn.  Called by friend
      whenever need to display a warning message.  Generally, we can
      ignore these in Room.
      */
     Room.prototype.warn = function(warnMsg)
     {
         IMUtil.dPrint('\nIgnoring warning messages in Room  ');
         IMUtil.dPrint(warnMsg);
         IMUtil.dPrint('\n\n');
     };

     Room.prototype.remove = function(imID)
     {
         IMUtil.dPrint('\n\nFIXME: allow users to be removed from group.\n\n');
     };

     Room.prototype.getIsVisibleTo = function(imID )
     {
         IMUtil.dPrint('\n\nCalled getIsVisibleTo in room.\n\n');
         return true;
     };
     

     //convGUI functionality used by friend

     /**
      Called when user sends a message to a friend.  
      */
     Room.prototype.writeMe = function(toWrite)
     {
         IMUtil.dPrint('\n\nIgnoring writeMe call.  Will need to ' +
                       'display message that I entered separately\n\n');
     };

     
     /**
      Called when receive message from friend.  Run through list of
      all friends.  For all friends that are connected, forward the
      message.
      */
     Room.prototype.writeFriend = function(toWrite)
     {
         IMUtil.dPrint('\n\nIn room, trying to write message: ');
         IMUtil.dPrint(toWrite);
         IMUtil.dPrint('\n\n');
         for (var s in this.friendArray)
         {
             IMUtil.dPrint('\n\nTrying to write once\n');
             if (this.friendArray[s].canSend(toWrite))
             {
                 this.friendArray[s].msgToFriend(toWrite);
                 IMUtil.dPrint('\nWritten\n');
             }
         }
     };


     /**
      Should have no effect to start with for our gui.
      */
     Room.prototype.changeFriendName = function(newFriendName)
     {
         IMUtil.dPrint('\n\nChanging friend name\n\n');
     };
     
     
 })();