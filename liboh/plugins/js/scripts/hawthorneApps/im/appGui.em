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


     /**
      "this" is automatically bound to an AppGui object in @see
      appGuiInitFunc.  Should only be called through event in html gui
      when a user clicks on a friend's name.

      Behavior is to interpret the click as a request for
      conversation, and to tell associated friend to begin
      conversation with other side.
      */
     function melvilleFriendClicked(friendID)
     {
         if (!friendID in imIDToFriendMap)
         {
             IMUtil.dPrint('\n\nClicked on a friendID that '+
                           'does not exist in imIDToFriendMap\n\n');
             return;
         }

         imIDToFriendMap[friendID].beginConversation();
     }


     /**
      "this" is automatically bound to an AppGui object in @see
      appGuiInitFunc.  Should only be called through event in html gui
      when a user potentially changes a group's data (for instance, its
      name, profile, status, etc.).

      Requests that associated group update its fields, send updates
      to group members if necessary, and re-displays the emerson app
      gui.
      */
     function melvilleGroupDataChange(groupID,newGroupName,
                                      newGroupStatus,newGroupProfile)
     {
         IMUtil.dPrint('\n\nGot into melville group data change\n\n');
         IMUtil.dPrint('New name:  ' + newGroupName + '\n\n');
         
         if (!groupID in groupIDToGroupMap)
         {
             IMUtil.dPrint('\n\nError in melvilleGroupDataChange.  ' +
                           'Do not have group with associated id.\n\n');
             return;
         }

         groupIDToGroupMap[groupID].changeName(newGroupName);
         groupIDToGroupMap[groupID].changeStatus(newGroupStatus);
         groupIDToGroupMap[groupID].changeProfile(newGroupProfile);
         //re-paint the group.
         this.display();
     }
     
     

     //"this" is automatically bound to AppGui object in @see AppGui
     //constructor. Should only be called through event in html gui.
     function appGuiInitFunc()
     {
         this.guiMod.bind('melvilleFriendClicked',std.core.bind(melvilleFriendClicked,this));
         this.guiMod.bind('melvilleGroupDataChange',std.core.bind(melvilleGroupDataChange,this));
         
         
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
             var groupID      =  groupIDToGroupMap[s].groupID;
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


         function genGroupDivIDFromGroupID(groupID)
         {
             return 'melvilleAppGui_group_div_id_' +
                 groupID.toString();
         }
         function genGroupNameTextAreaIDFromGroupID(groupID)
         {
             return 'melvilleAppGui_group_name_textarea_id_' +
                 groupID.toString();
         }
         
         function genGroupStatusTextAreaIDFromGroupID(groupID)
         {
             return 'melvilleAppGui_group_status_textarea_id_' +
                 groupID.toString();             
         }
         
         function genGroupProfileTextAreaIDFromGroupID(groupID)
         {
             return 'melvilleAppGui_group_profile_textarea_id_' +
                 groupID.toString();                          
         }

         
         
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
                 var groupName    = s;
                 var groupID      = fullGroups[s][0];
                 var groupStatus  = fullGroups[s][1];
                 var groupProfile = fullGroups[s][2];


                 htmlToDisplay += '<div onclick="' +
                     'melvilleAppGuiGroupClicked(' +
                     groupID.toString() + ')">';
                 htmlToDisplay += '<b>' + groupName +'</b>';
                 htmlToDisplay += '</div>'; //closes on group clicked div

                 
                 htmlToDisplay += '<div id="' +
                     genGroupDivIDFromGroupID(groupID) +
                     '"' + 'style="display: none"' + 
                     '>';

                 //put group name into modifiable textarea.
                 htmlToDisplay += 'group name:   <textarea id="'+
                     genGroupNameTextAreaIDFromGroupID(groupID) +'">' +
                     groupName +
                     '</textarea> <br/>';

                 //put group status into modifiable textarea
                 htmlToDisplay += 'group status: <textarea id="'+
                     genGroupStatusTextAreaIDFromGroupID(groupID) +'">' +
                     groupStatus +
                     '</textarea> <br/>';

                 //// put group profile into modifiable textarea
                 htmlToDisplay += 'group profile: <textarea id="'+
                     genGroupProfileTextAreaIDFromGroupID(groupID) +'">' +
                     groupProfile +
                     '</textarea> <br/>';


                 //closes div associated with genGroupDivIDFromGroupName above
                 htmlToDisplay += '</div>';
                 
                 htmlToDisplay += '</br/>';


                 //run through all the friends that are in this group,
                 //displaying each separately.
                 var friendList = fullGroups[s][4];
                 for (var t in friendList)
                 {
                     var friendID     = friendList[t][0];
                     var friendName   = friendList[t][1];
                     var friendStatus = friendList[t][2];
                     
                     //whenever user clicks on this div, will call
                     //melvilleAppGuiFriendClicked, which sends message
                     //to appgui to open a convgui to friend.
                     htmlToDisplay += '<div onclick="' +
                         'melvilleAppGuiFriendClicked(' +
                         friendID.toString() + ')"';

                     htmlToDisplay += '<i>' + friendName + '</i>:  <br/>';
                     htmlToDisplay += friendStatus;
                     htmlToDisplay += '</div>';//closes onclick div
                     htmlToDisplay += '<br/>';
                 }

                 

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


         /**
          param {unique int} friendID Integer representing the id of
          the friend user clicked on.
          */
         melvilleAppGuiFriendClicked  = function (friendID)
         {
             sirikata.event('melvilleFriendClicked',friendID);
         };


         melvilleAppGuiGroupClicked = function(groupID)
         {
             var groupDivID = genGroupDivIDFromGroupID(groupID);
             var itemToToggle= document.getElementById(groupDivID);
             if (itemToToggle === null)
             {
                 sirikata.log('warn', '\\nWarning on Melville group ' +
                              'clicked: do not have associated groupID\\n');
                 return; 
             }


             if (itemToToggle.style.display==='none')
             {
                 //makes text visible.
                 itemToToggle.style.display = 'block';
             }
             else
             {
                 itemToToggle.style.display = 'none';
                 var groupNameTAreaID =
                     genGroupNameTextAreaIDFromGroupID(groupID);

                 var groupStatusTAreaID =
                     genGroupStatusTextAreaIDFromGroupID(groupID);

                 var groupProfileTAreaID =
                     genGroupProfileTextAreaIDFromGroupID(groupID);
                 
                 
                 var newGroupName    = $('#'+groupNameTAreaID).val();
                 var newGroupStatus  = $('#'+groupStatusTAreaID).val();
                 var newGroupProfile = $('#'+groupProfileTAreaID).val();
                 sirikata.event('melvilleGroupDataChange', groupID,newGroupName,
                                newGroupStatus,newGroupProfile);
             }
         };

         @;

         
         
         returner += '});';
         return returner;
     }
     
 })();