system.require('hawthorneApps/im/friend.em');


(function()
 {
     var IM_APP_NAME = 'Melville';
     var visIDToFriendMap  = {};
     var imIDToFriendMap   = {};

     
     var proxHandler = null;
     
     
     AppGui = function()
     {
         //policy is to try to become friends with everyone around me.
         function proxAddedCallback(visAdded)
         {
             if (visAdded.toString() in allFriends)
                 return;

             //ask user if he/she wants to add the friend.

             //for now, automatically add as friend.  Automatically name friend as well,

             var friendToAdd = new Friend('friend name', visAdded, this,);             
             allFriends[visAdded.toString()] = new Friend
                          
             lkjs;
         }

         proxHandler = system.onProxAdded(std.core.bind(proxAddedCallback,this));


                  
     };

     AppGui.prototype.kill = function ()
     {
         if (proxHandler != null)
         {
             proxHandler.clear();
             proxHandler = null;
         }

         for (var s in allFriends)
             allFriends[s].kill();
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

     AppGui.prototype.warn = function()
     {
         system.__debugPrint('Asked to warn in app gui.');
     };

     AppGui.prototype.remove = function()
     {
         system.__debugPrint('Asked to remove in app gui.');
     };

     AppGui.prototype.getIsVisibleTo = function()
     {
         system.__debugPrint('Asked to getIsVisibleTo in app gui.');  
     };
     
 })();