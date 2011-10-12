system.require('hawthorneApps/im/friend.em');
system.require('hawthorneApps/im/imUtil.em');

(function()
 {
     var IM_APP_NAME = 'Melville';
     var visIDToFriendMap  = {};
     var imIDToFriendMap   = {};

     
     var proxHandler = null;
     var requestHandler = null;


     /**
      @param {visible} potentialFriend

      don't add them as friend unless they aren't already your friend,
      they aren't you, and the user gives permission to add them.
      (For now, skipping user-asking part.)
      */
     function tryAddFriend(potentialFriend)
     {
         system.__debugPrint('\n\nAttempting to add friend in tryAddFriend of appGui.em\n\n');
         
         if ((potentialFriend.toString() in visIDToFriendMap) ||
             (potentialFriend.toString() == system.self.toString()))
             return null;

         //ask user if he/she wants to add the friend.

         //for now, automatically add as friend.  Automatically name
         //friend as well fname
         var friendToAdd =
             new Friend('fnamer', potentialFriend, this, IMUtil.getUniqueInt());
         
         visIDToFriendMap[potentialFriend.toString()] = friendToAdd;
         imIDToFriendMap [friendToAdd.imID]           = friendToAdd;

         return friendToAdd;
     }

     
     
     AppGui = function()
     {
         var wrappedTryAddFriend = std.core.bind(tryAddFriend,this);
         
         //policy is to try to become friends with everyone that I can
         //see.
         function proxAddedCallback(visAdded)
         {
             var newFriend = wrappedTryAddFriend(visAdded);
             if (newFriend !== null)
                 system.__debugPrint('Trying to add new friend through prox.');
                 
         }

         //additional true field indicates to also call handling
         //function for all visibles that are *currently* within query
         //range, not just those that get added to result set.
         proxHandler = system.self.onProxAdded(
             std.core.bind(proxAddedCallback,this),true);


         //If we receive a registration request, then we check if
         //we have any friends corresponding to the sender of the
         //friend request.  If we do, then we ask the friend to
         //process the request.
         //If we do not, then, we check if we should add the other
         //side as a friend.  If we should, we construct a new friend
         //object, and ask it to handle the registration request.
         function handleRegRequest(msg, sender)
         {
             var friendToProcMsg = null;
             if (sender.toString() in visIDToFriendMap)
                 friendToProcMsg = visIDToFriendMap[sender.toString()];
             else
                 friendToProcMsg = wrappedTryAddFriend(sender);
             
             if (friendToProcMsg !== null)
                 friendToProcMsg.processRegReqMsg(msg);
             
         }

         
         //actually set the handler for registration requests.
         requestHandler = std.core.bind(handleRegRequest,this) <<
             {'imRegRequest'::};
         
     };

     AppGui.prototype.kill = function ()
     {
         if (proxHandler !== null)
         {
             proxHandler.clear();
             proxHandler = null;
         }
         if (requestHandler !== null)
         {
             requestHandler.clear();
             requestHandler = null;
         }


         for (var s in imIDToFriendMap)
             imIDToFriendMap[s].kill();

         imIDToFriendMap  = {};
         visIDToFriendMap = {};

     };

     /**
      Will remove this function sooner or later. Right now, I'm just using it for testing.
      */
     AppGui.prototype.debugBroadcast = function(toBroadcast)
     {
         for (var s in imIDToFriendMap)
             imIDToFriendMap[s].msgToFriend(toBroadcast);
     };
     
     AppGui.prototype.getStatusPresenting = function()
     {
         return 'Status Presenting';
     };

     AppGui.prototype.getProfilePresenting = function()
     {
         return 'Profile Presenting';
     };

     AppGui.prototype.display = function()
     {
         system.__debugPrint('Asked to display in app gui.');
     };

     AppGui.prototype.warn = function(warningMsg)
     {
         system.__debugPrint('\n\nAsked to warn in app gui with message:');
         system.__debugPrint(warningMsg + '\n\n');
         
     };

     AppGui.prototype.remove = function()
     {
         system.__debugPrint('Asked to remove in app gui.');
     };

     /**
      @param {unique int} imID identifier for existing Friend.

      Returns true if Friend represented by imID should be able to
      send messages to me.
      For now, just returns true.
      */
     AppGui.prototype.getIsVisibleTo = function(imID)
     {
         system.__debugPrint('Asked to getIsVisibleTo in app gui.');
         return true;
     };
     
 })();