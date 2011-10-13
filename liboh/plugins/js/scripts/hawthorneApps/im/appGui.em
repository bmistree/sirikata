system.require('hawthorneApps/im/friend.em');
system.require('hawthorneApps/im/imUtil.em');
system.require('hawthorneApps/im/group.em');

(function()
 {
     var IM_APP_NAME = 'MelvilleIM';
     var visIDToFriendMap  = {};
     var imIDToFriendMap   = {};

     var groupIDToGroupMap = {};

     
     var proxHandler    = null;
     var requestHandler = null;

     var WARN_EVENT     = 'WARN_EVENT';
     var DISPLAY_EVENT  = 'DISPLAY_EVENT';

     /**
      @param {visible} potentialFriend

      don't add them as friend unless they aren't already your friend,
      they aren't you, and the user gives permission to add them.
      (For now, skipping user-asking part.)
      */
     function tryAddFriend(potentialFriend)
     {
         if ((potentialFriend.toString() in visIDToFriendMap) ||
             (potentialFriend.toString() == system.self.toString()))
             return null;

         //ask user if he/she wants to add the friend.

         //for now, automatically add as friend.  Automatically name
         //friend as well fname
         var friendToAdd =
             new Friend('fnamer', potentialFriend, this, IMUtil.getUniqueInt());

         //when asking user whether wanted to add friend, presumably
         //got group id to place friend in.  For now, just assuming
         //using first group.
         var groupIDIndex =null;
         for (groupIDIndex in groupIDToGroupMap)
             break;

         if (groupIDIndex !== null)
             groupIDToGroupMap[groupIDIndex].addMember(friendToAdd);
         else
             IMUtil.dPrint('\n\nWarning: no group to add new friend to\n');
             
         
         visIDToFriendMap[potentialFriend.toString()] = friendToAdd;
         imIDToFriendMap [friendToAdd.imID]           = friendToAdd;

         //re-display entire gui when add friend.
         this.display();
         
         return friendToAdd;
     }


     //lkjs;
     //"this" is automatically bound to AppGui object in @see AppGui
     //constructor.
     function appGuiInitFunc()
     {
         //still must clear pendingEvents.

         //only want to execute last display event.
         var displayEvent = false;
         this.guiInitialized = true;
         for (var s in this.pendingEvents)
         {
             if (this.pendingEvents[s][0] == WARN_EVENT)
                 internalWarn(this, this.pendingEvents[s][1]);
             else if (this.pendingEvents[s][0] == DISPLAY_EVENT)
                 displayEvent = true;
         }

         if (displayEvent)
             internalDisplay(this);
         
         this.pendingEvents = [];
     }

     /**
      @param {string-untainted} toWarnWith
      */
     function internalWarn(appGui,toWarnWith)
     {
         appGui.guiMod.call('warnAppGui',toWarnWith);
     }

     function internalDisplay(appGui)
     {
         IMUtil.dPrint('\n\nGot into internalDisplay\n\n');
         appGui.guiMod.call('appGuiDisplay',appGui.getDisplayText());
     }
     
     
     AppGui = function()
     {
         this.guiMod = simulator._simulator.addGUITextModule(
             IM_APP_NAME,
             getAppGUIText(),
             std.core.bind(appGuiInitFunc,this)
         );

         this.pendingEvents = [];
         this.guiInitialized = false;

         //create a default group to friends in
         var defaultGroup = new Group('default',IMUtil.getUniqueInt(),
                                      'def status','def prof',true,this);
         groupIDToGroupMap[defaultGroup.groupID] = defaultGroup;

         
         var wrappedTryAddFriend = std.core.bind(tryAddFriend,this);
         
         //policy is to try to become friends with everyone that I can
         //see.
         function proxAddedCallback(visAdded)
         {
             var newFriend = wrappedTryAddFriend(visAdded);
             if (newFriend !== null)
                 IMUtil.dPrint('\n\nTrying to add new friend through prox.\n\n');
                 
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

         imIDToFriendMap   = {};
         visIDToFriendMap  = {};
         groupIDToGroupMap = {};

     };


     /**
      @return {object} laid out according to the @see appGuiDisplay
      function in @getAppGUIText function.
      */
     AppGui.prototype.getDisplayText = function()
     {
         var returner = {};

         for (var s in groupIDToGroupMap )
         {

             var groupName    =  groupIDToGroupMap[s].groupName;
             var groupID      =  s;
             var groupStatus  =  groupIDToGroupMap[s].status;
             var groupProfile =  groupIDToGroupMap[s].profile;
             var groupVisible =  groupIDToGroupMap[s].visible;
             var groupFriends =  groupIDToGroupMap[s].getFriends();
             
             var singleItem   =  [groupID,groupStatus,groupProfile,
                                  groupVisible,groupFriends];
             
             returner[groupName] = singleItem;
         }
         return returner;
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
         if (! this.guiInitialized)
         {
             this.pendingEvents.push([DISPLAY_EVENT]);
             return;
         }
         internalDisplay(this);
     };

     AppGui.prototype.warn = function(warnMsg)
     {
         if (! this.guiInitialized)
         {
             this.pendingEvents.push([WARN_EVENT, warnMsg]);
             return;
         }

         internalWarn(this,warnMsg);
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

     function getAppGUIText()
     {
         var returner = "sirikata.ui('" + IM_APP_NAME + "',";
         returner += 'function(){ ';

         returner += @
         //gui for displaying friends list.
         $('<div>' +
           '</div>' //end div at top.
          ).attr({id:'melville-chat-gui',title:'melvilleIM'}).appendTo('body');

         
         var melvilleWindow = new sirikata.ui.window(
            '#melville-chat-gui',
            {
	        autoOpen: false,
	        height: 'auto',
	        width: 300,
                height: 400,
                position: 'right'
            }
         );
         melvilleWindow.show();


         /**
          param {object <string, [int, string, string, bool, array]>}
          fullGroups Indices of object are group names.  The values of
          fullGroups are 5 elelement long arrays.

          The first element of the array is a uniue id for the group.

          The second element is a string representing the status that
          will be displayed to every member of the group.

          The third element is a string representing the profile that
          will be displayed to every member of the group.

          The fourth element is a bool indicating whether you are
          visible to members of the group.

          The fifth element is another array, this time containing
          information about the members of the group.  In particular,
          every element of the array has the following form:
            -The first element is an int id for a friend
            -The second element is a string for the friend's name
            -The third element is a string for the friend's status
            -The fourth element is a string for the friend's profile


          This function walks through all the data in fullGroups, and
          displays it in an attemptedly-nice form.
          */
         appGuiDisplay = function(fullGroups)
         {
             //let's just try to display the groups correctly.
             
             var htmlToDisplay = '';
             for(var s in fullGroups)
             {
                 htmlToDisplay += '<div id="melvilleAppGui_' +s +'">';
                 htmlToDisplay += '<b>' + s +'</b>';
                 htmlToDisplay += '</br/>';


                 //run through all the friends that are in this group,
                 //displaying each separately.
                 var friendList = fullGroups[s][4];
                 for (var t in friendList)
                 {
                     var friendName = friendList[t][1];
                     htmlToDisplay += '<i>' + friendName + '</i> <br/>' ;
                 }

                 
                 //closes div at top of forl loop: (one with id =
                 //"melvilleAppGui_" + s
                 htmlToDisplay += '</div>';
             }

             $('#melville-chat-gui').html(htmlToDisplay);
         };


         //gui for displaying warnings.
         $('<div>'   +
          '</div>' //end div at top.
         ).attr({id:'melville-chat-warn-gui',title:'melvilleIMWarning'}).appendTo('body');
         

         //keep hidden the warning window
         var melvilleWarnWindow = new sirikata.ui.window(
            '#melville-chat-warn-gui',
            {
	        autoOpen: false,
	        height: 'auto',
	        width: 300,
                height: 300,
                position: 'right'
            }
         );
         melvilleWarnWindow.hide();


         //displays warning messages in new gui.
         warnAppGui = function(toWarnWith)
         {
             $('#melvilee-chat-warn-gui').append('<br/>' +toWarnWith);
             melvilleWarnWindow.show();
         };
         @;

         
         
         returner += '});';
         return returner;
     }
     
 })();