system.require('hawthorneApps/im/friend.em');
system.require('hawthorneApps/im/imUtil.em');
system.require('hawthorneApps/im/group.em');



(function()
 {
     var IM_APP_NAME = 'MelvilleIM';
     var visIDToFriendMap  = {};
     var imIDToFriendMap   = {};

     var groupIDToGroupMap = {};

     //a list of outstanding questions that have been asked of user.
     //Indices are unique integers.  Values are arrays.  First element
     //of array always lists request type.  Other elements of array are
     //just saved data used to process that type.
     var outstandingUserRequestMap = { };

     var OUTSTANDING_REQ_FRIENDSHIP_EVENT =
         'OUTSTANDING_REQ_FRIENDSHIP_EVENT';

     //create a utility class to manage outstanding friendship requests.
     var OutFriendshipReqUtil =
         {
             createOutstandingFriendRequest: function(cbackFunc,appgui,potFriendVis,regMsg)
             {
                 return [OUTSTANDING_REQ_FRIENDSHIP_EVENT,
                         std.core.bind(cbackFunc,appgui,potFriendVis,regMsg),
                         potFriendVis.toString()];
             },

             getCbackToExecOnAdd: function(entry)
             {
                 return entry[1];
             },
             getWhoFriendingID: function(entry)
             {
                 return entry[2];
             },
             
             isOutstandingFriendshipReq : function(entry)
             {
                 return (entry[0] === OUTSTANDING_REQ_FRIENDSHIP_EVENT);
             }
         };
     

     var proxHandler    = null;
     var requestHandler = null;

     var WARN_EVENT     = 'WARN_EVENT';
     var DISPLAY_EVENT  = 'DISPLAY_EVENT';
     var TRY_ADD_EVENT  = 'TRY_ADD_EVENT';
     


     /**
      @param {visible} potentialFriend

      @param {msg object} reqMsg is non-null when we are trying to add
      a friend after having received a request message from that
      friend.  Is null if we're trying to add a friend from a
      proximity message.
      
      don't add them as friend unless they aren't already your friend,
      they aren't you, and the user gives permission to add them.
      (For now, skipping user-asking part.)
      
      */
     function tryAddFriend(potentialFriend,reqMsg)
     {
         
         if ((potentialFriend.toString() in visIDToFriendMap) ||
             (potentialFriend.toString() == system.self.toString()))
             return;


         //if the gui has not yet been initalized, hold on to this
         //event until it is.  This way, we only make internal calls
         //into gui after it's initialized.
         if (!this.guiInitialized)
         {
             var newPendingEvent = [TRY_ADD_EVENT,potentialFriend,reqMsg];
             this.pendingEvents.push(newPendingEvent);
             return;
         }

         //check if we are already waiting on the user to validate
         //this friend.
         for (var s in outstandingUserRequestMap)
         {
             var mapEntry = outstandingUserRequestMap[s];

             var isFriendshipReq =
                 OutFriendshipReqUtil.isOutstandingFriendshipReq(
                     mapEntry);

             if (isFriendshipReq)
             {
                 var whoFriendingID =
                     OutFriendshipReqUtil.getWhoFriendingID(mapEntry);
                 if (whoFriendingID == potentialFriend.toString())
                 {
                     //means that we've already sent a
                     //friendship request to the user about
                     //this potential friend.

                     //because this request has a message, overwrite
                     //previous entry so that we'll reply to the most
                     //recent friendship request.
                     if (reqMsg !== null)
                     {
                         var newEntry =
                             OutFriendshipReqUtil.createOutstandingFriendRequest(
                                 completeAddFriend, this, potentialFriend, reqMsg);
                     
                         outstandingUserRequestMap[s] = newEntry;                             
                     }

                     //already waiting on user can return.
                     return;
                 }
             }
         }
         
         var outUserReqID = IMUtil.getUniqueInt();
         var reqMapEntry  =
             OutFriendshipReqUtil.createOutstandingFriendRequest(
                 completeAddFriend, this, potentialFriend, reqMsg);

         outstandingUserRequestMap[outUserReqID] = reqMapEntry;
         this.guiMod.call(
             'checkAddFriend',potentialFriend.toString(),outUserReqID);
     }

     /**
      @param {unique int} requestID We passed an identifier on to the
      gui.  The identifier allows us to index back into
      outstandingUserRequestMap to find the associated addFriend
      request.

      @param {String - tainted} newFriendName What the user wants to
      name the new friend.

      @param {unique int} newFriendGroupID the id of the group to
      enter this friend into.  Note: the group may not still be
      available. 
      */
     function userRespAddFriend(requestID, newFriendName,newFriendGroupID)
     {
         if (! requestID in outstandingUserRequestMap)
         {
             IMUtil.dPrint('\n\nError in userRespAddFriend.  ' +
                           'Cannot find associated requestID.\n\n');
             return;
         }

         //add friend.  Check to ensure that the requestID is
         //associated with a friendship request and execute the
         //friendship request's callback.
         var outReqEntry = outstandingUserRequestMap[requestID];
         if (!OutFriendshipReqUtil.isOutstandingFriendshipReq(
                 outReqEntry))
         {
             IMUtil.dPrint('\n\nError in userRespAddFriend.  ' +
                           'Do not have a friendship request stored ' +
                           'for this id.\n\n');
             return;
         }

         //callback to execute.  @see OutFriendshipReqUtil for type
         //signature.
         var cback = OutFriendshipReqUtil.getCbackToExecOnAdd(outReqEntry);
         cback(newFriendName,newFriendGroupID);
         
         //remove request from pending requests.
         delete outstandingUserRequestMap[requestID];
     }
     
     /**
      @param {visible} newFriendVis
      @param {msg object or null} If not null, then means that this is
      a friendship request message from some other presence in the
      world, and, if we're adding other presence as friend, we need to
      send a reply back saying so.

      @param {String -tainted} newFriendName @see userRespAddFriend

      @param {unique int} newFriendGroupID @see userRespAddFriend
      
      Bound in @see tryAddFriend to an outstanding friendship request
      event.  When user clicks gui to add friend, @see
      userRespAddFriend calls this function to complete the friendship
      addition.
      */
     function completeAddFriend(newFriendVis,reqMsg,
                                newFriendName,newFriendGroupID)
     {
         //already added friend ...can occur if other side sends you
         //multiple requests, and you only confirm the first.
         if (newFriendVis.toString() in visIDToFriendMap)
             return;

         //groupID may have vanished while you were adding friend.
         //Unlikely, but possible.
         if (! newFriendGroupID in groupIDToGroupMap)
         {
             //if wanted to, could instead go through process of
             //adding friend again.
             IMUtil.dPrint('\n\nWarning, no group to add new friend to.  '+
                           'Aborting friend add.\n\n');
             return;
         }
         
         //create new friend object
         var friendToAdd =
             new Friend(IMUtil.htmlEscape(newFriendName), newFriendVis,
                        this, IMUtil.getUniqueInt());

         groupIDToGroupMap[newFriendGroupID].addMember(friendToAdd);
         visIDToFriendMap[newFriendVis.toString()] = friendToAdd;

         imIDToFriendMap [friendToAdd.imID] = friendToAdd;

         //Takes care of the case where the other side had initiated
         //friendship first: we reply saying that we'd be happy to be
         //friends.
         if (reqMsg !== null)
             friendToAdd.processRegReqMsg (reqMsg);
         
         //re-display entire gui when add friend.
         this.display();
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
         if (!groupID in groupIDToGroupMap)
         {
             IMUtil.dPrint('\n\nError in melvilleGroupDataChange.  ' +
                           'Do not have group with associated id.\n\n');
             return;
         }
         
         groupIDToGroupMap[groupID].changeName(
             IMUtil.htmlEscape(newGroupName));
         
         groupIDToGroupMap[groupID].changeStatus(
             IMUtil.htmlEscape(newGroupStatus));
         
         groupIDToGroupMap[groupID].changeProfile(
             IMUtil.htmlEscape(newGroupProfile));
         
         //re-paint the group.
         this.display();
     }

     /**
      "this" is automatically bound to an AppGui object in @see
      appGuiInitFunc.  This function should only be called through
      event in html gui when a user potentially changes a friend's
      name.
      */
     function melvilleFriendNameGroupChange(friendID,
                                            newFriendName,
                                            newFriendGroupID,prevFriendGroupID)
     {
         if (! friendID in imIDToFriendMap)
         {
             IMUtil.dPrint('\n\nError in melvilleFriendNameChange.  ' +
                           'Do not have friend with associated id.\n\n');
             return;                 
         }


         imIDToFriendMap[friendID].changeName(
             IMUtil.htmlEscape(newFriendName));


         //friend transitioned from one group to another
         if (newFriendGroupID != prevFriendGroupID)
         {
             if (newFriendGroupID in groupIDToGroupMap)
             {
                 if (prevFriendGroupID in groupIDToGroupMap)
                 {
                     groupIDToGroupMap[prevFriendGroupID].removeMember(
                         imIDToFriendMap[friendID]);
                 }
                 groupIDToGroupMap[newFriendGroupID].addMember(
                     imIDToFriendMap[friendID]);
             }
             else
             {
                 IMUtil.dPrint('\n\nError in melvilleFriendGroupChange.  ' +
                               'Do not have group to change this friend to.');
             }
         }
         
         //re-paint display to reflect changes.
         this.display();
     }
     

     /**
      "this" is automatically bound to an AppGui object in @see
      appGuiInitFunc.  This function should only be called through
      event in html gui when a user asks to create a new group.  
      */
     function melvilleAddGroup(groupName, groupStatus,groupProfile)
     {
         var defaultGroup = new Group(
             IMUtil.htmlEscape(groupName),IMUtil.getUniqueInt(),
             IMUtil.htmlEscape(groupStatus),IMUtil.htmlEscape(groupProfile),
             true,this);

         groupIDToGroupMap[defaultGroup.groupID] = defaultGroup;
         //re-paint display to reflect changes.
         this.display();
     }


     /**
      @param {unique int} requestRecID The unique id of a friend
      request.  Remove from outstanding requests: we don't want to be
      friends with this person.
      */
     function friendRequestReject(requestRecID)
     {
         if (!requestRecID in outstandingUserRequestMap)
         {
             IMUtil.dPrint('\nError in friendRequestReject.  ' +
                           'Do not have record for this requestID.\n');
             return;
         }
         
         delete outstandingUserRequestMap[requestRecID];
     }


     /**
      @param {unique int} requestRecID The unique id of a friend
      request.  Pop up a gui asking for what to name user and what group to put user in.
      */
     function friendRequestAccept(requestRecID)
     {
         var groupIDToGroupNames= null;
         for (var s in groupIDToGroupMap)
         {
             if (groupIDToGroupNames === null)
                 groupIDToGroupNames = { };
             groupIDToGroupNames [s] = groupIDToGroupMap[s].groupName;        
         }

         //have no groups.  create one: default
         if (groupIDToGroupNames === null)
         {
             var newGroup = new Group(
                 'defualt', IMUtil.getUniqueInt(), '','',true,this);

             groupIDToGroupNames = {};

             groupIDToGroupMap[newGroup.groupID]   = newGroup;
             groupIDToGroupNames[newGroup.groupID] = newGroup.groupName;
             this.display();
         }
         
         //the internal js will pop up asking us what we want to name
         //the new friend, and what group we want to put the new
         //friend in.
         this.guiMod.call('melvilleAppGuiNewFriendGroupID',
                          requestRecID, groupIDToGroupNames);
     }
     
     
     //"this" is automatically bound to AppGui object in @see AppGui
     //constructor. Should only be called through event in html gui.
     function appGuiInitFunc()
     {
         this.guiMod.bind('melvilleFriendClicked',
                          std.core.bind(melvilleFriendClicked,this));
         this.guiMod.bind('melvilleGroupDataChange',
                          std.core.bind(melvilleGroupDataChange,this));
         this.guiMod.bind('melvilleFriendNameGroupChange',
                          std.core.bind(melvilleFriendNameGroupChange,this));
         this.guiMod.bind('melvilleAddGroup',
                          std.core.bind(melvilleAddGroup,this));

         this.guiMod.bind('friendRequestReject',
                         std.core.bind(friendRequestReject,this));

         this.guiMod.bind('friendRequestAccept',
                          std.core.bind(friendRequestAccept,this));
         
         this.guiMod.bind('userRespAddFriend',
                          std.core.bind(userRespAddFriend,this));
         
         
         //still must clear pendingEvents.
         //only want to execute last display event.
         this.guiInitialized = true;
         for (var s in this.pendingEvents)
         {
             if (this.pendingEvents[s][0] == WARN_EVENT)
                 internalWarn(this, this.pendingEvents[s][1]);
             else if (this.pendingEvents[s][0] == TRY_ADD_EVENT)
             {
                 //@see the structure for TRY_ADD_EVENTS defined
                 //in @tryAddFriend
                 var wrappedTryAddFriend =
                     std.core.bind(tryAddFriend,this,
                                   this.pendingEvents[s][1],
                                   this.pendingEvents[s][2]);
                 wrappedTryAddFriend();
             }
             else if (this.pendingEvents[s][0] == DISPLAY_EVENT)
             {
                 //we're already going to display at the end of this
                 //function.  Therefore, we can effectively ignore
                 //this event.
             }
         }

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
             if (visAdded.toString() == system.self.toString())
                 return;
             
             IMUtil.dPrint('\n\nTrying to add new friend through prox.\n\n');
             wrappedTryAddFriend(visAdded,null);
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
         //If we do not, then, call tryAddFriend, which checks if we're
         //already processing a request to add the friend, and whether the
         //user wants to add as friend.
         function handleRegRequest(msg, sender)
         {

             //do we already have a friend with this id
             if (sender.toString() in visIDToFriendMap)
             {
                 var friendToProcMsg = visIDToFriendMap[sender.toString()];
                 friendToProcMsg.processRegReqMsg (msg);
             }
             else
                 wrappedTryAddFriend(sender,msg);
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

         
         function genFriendChangeNameGroupDivIDFromFriendID(friendID)
         {
             return 'melvilleAppGui_friend_id_div_' +
                 friendID.toString();
         }

         function genFriendChangeNameTextAreaIDFromFriendID(friendID)
         {
             return 'melvilleAppGui_friend_id_changeName_tarea_' +
                 friendID.toString();
         }

         function genCreateGroupDivID()
         {
             return 'melvilleAppGui_createGroup_div_id';
         }
         function genNewGroupNameTAreaID()
         {
             return 'melvilleAppGui_createGroup_groupNameTArea';
         }
         function genNewGroupStatusTAreaID()
         {
             return 'melvilleAppGui_createGroup_groupStatusTArea';
         }
         function genNewGroupProfileTAreaID()
         {
             return 'melvilleAppGui_createGroup_groupProfileTArea';
         }

         function genFriendGroupChangeSelectID(friendID)
         {
             return 'melvilleAppGui_friend_group_change_selectID' +
                 friendID.toString();
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

             //header controls to create new groups
             htmlToDisplay += '<div onclick="' +
                 'melvilleAppGuiCreateGroupClicked()">';
             htmlToDisplay += '<b> New group</b>';
             htmlToDisplay += '</div>';

             htmlToDisplay += '<div id="' +
                 genCreateGroupDivID() +
                 '"' + 'style="display: none"' + 
                 '>';

             //put group name into modifiable textarea.
             htmlToDisplay += 'group name:   <textarea id="'+
                 genNewGroupNameTAreaID() +'">' +
                 groupName +
                 '</textarea> <br/>';
             
             //put group status into modifiable textarea
             htmlToDisplay += 'group status: <textarea id="'+
                 genNewGroupStatusTAreaID() +'">' +
                 groupStatus +
                 '</textarea> <br/>';

             //// put group profile into modifiable textarea
             htmlToDisplay += 'group profile: <textarea id="'+
                 genNewGroupProfileTAreaID() +'">' +
                 groupProfile +
                 '</textarea> <br/>';

             //closes hidden div with group name fields
             htmlToDisplay += '</div>';


             //actually print out all friends in groups
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
                         friendID.toString() + ')">';

                     htmlToDisplay += '<i>' + friendName + '</i>.';
                     htmlToDisplay += '</div>';//closes onclick div

                     
                     htmlToDisplay += '<div onclick="' +
                         'melvilleAppGuiFriendNameGroupChangeClicked(' +
                         friendID.toString() + ',' + groupID.toString() +
                         ')">';
                     htmlToDisplay += 'change name/group';
                     htmlToDisplay += '</div>';


                     htmlToDisplay += '<div id="' +
                         genFriendChangeNameGroupDivIDFromFriendID(friendID) +
                         '"' + 'style="display: none"' + 
                         '>';
                     
                     //// put friend name into modifiable textarea
                     htmlToDisplay += 'friend name: <textarea id="'+
                         genFriendChangeNameTextAreaIDFromFriendID(friendID) +'">' +
                         friendName +
                         '</textarea> <br/>';

                     
                     //for each friend, create a pull-down menu of
                     //groups that the friend can change into.
                     htmlToDisplay += '<select id="' +
                         genFriendGroupChangeSelectID(friendID) + '">';

                     //display current group on top
                     htmlToDisplay += '<option value=' + groupID.toString() +
                         ' selected="selected">' + groupName;
                     htmlToDisplay += '</option>';

                     for (var fullGroupsIter in fullGroups)
                     {
                         var gName = fullGroupsIter;
                         var gID   = fullGroups[gName][0];
                         
                         if (gID != groupID)
                         {
                             htmlToDisplay += '<option value=' +
                                 gID.toString() +'>';
                             htmlToDisplay += gName;
                             htmlToDisplay += '</option>';
                             
                         }
                     }

                     //closes selection list of different groups can
                     //add friend to.
                     htmlToDisplay += '</select>'; 
                     
                     htmlToDisplay += '</div>';  //closes friend change name div
                     
                     htmlToDisplay += friendStatus;

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



         
         function genAddFriendRequestID(userReqID)
         {
             return '__friend_requestID__' + userReqID.toString();
         }
         function genAddFriendRequestDivYesID(userReqID)
         {
             return '_friend_req_div_yes_id__' + userReqID.toString();
         }

         function genAddFriendRequestDivNoID(userReqID)
         {
             return '_friend_req_div_no_id_' + userReqID.toString();
         }
         
         /**
          \param {string} visid Presence id of the visible that we
          want to add as friend
          
          \param {unique int} userReqID unique identifier used to
          ensure that the user's response is associated with the
          correct request when function returns.

          This function gets called through Emerson, it opens the warn
          gui window asking the user if he/she wants to become friends
          with a visible that he/she currently is not friends with.
          */
         checkAddFriend = function(visID, userReqID)
         {
             //add text to the warn window
             var moreFriendsText = '';

             moreFriendsText += '<div id="' +
                 genAddFriendRequestID(userReqID) +
                 '">';

             moreFriendsText += 'Do you want to become friends with: ' +
                 visID + '?';

             //div for yes and no
             moreFriendsText += '<div id="' +
                 genAddFriendRequestDivYesID(userReqID) + '" ' +
                 'onclick="melvilleAppGuiAddFriendYesClicked(' +
                 userReqID.toString() + ')"' +  // closes onclick
                 '>'  +  // closes open div
                 '<b>Yes</b></div>';

             moreFriendsText += '<div id="' +
                 genAddFriendRequestDivNoID(userReqID) + '" ' +
                 'onclick="melvilleAppGuiAddFriendNoClicked(' +
                 userReqID.toString() + ')"' + // closes onclick
                 '>'  +  // closes open div
                 '<b>No</b></div>';

             //close the div asking whether to add this friend.
             moreFriendsText += '</div>';

             //actually add the request message to the warn gui.
             $('#melville-chat-warn-gui').append(moreFriendsText);
             
             //expose the warn window
             melvilleWarnWindow.show();
         };



         /**
          \param {unique int} userReqID A unique id for tracking this
          friend request.

          We don't want to add the friend.  Tell emerson code to clear
          request from outstanding.  Also, remove the request from
          those outstanding.
          */
         melvilleAppGuiAddFriendNoClicked = function(userReqID)
         {
             sirikata.event('friendRequestReject', userReqID);
             $('#' + genAddFriendRequestID(userReqID)).remove();
         };

         /**
          \param {unique int} userReqID A unique id for tracking this
          friend request.

          We want want to add the friend.  Message down to base
          Emerson code.  Emerson code generates updated set of group
          ids, and asks you which group you want to put the user into.
          */
         melvilleAppGuiAddFriendYesClicked = function (userReqID)
         {
             sirikata.event('friendRequestAccept', userReqID);
             $('#' + genAddFriendRequestID(userReqID)).remove();
         };
         

         /**
          \param {unique int} friendID Integer representing the id of
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

         /**
          Gets called when user clicks on the change name field next
          to friend name.  If friend name div was hidden, then expose
          it.  If friend name div was exposed, then read values from
          its text areas and notify appgui that a friend's name has
          changed.
          */
         melvilleAppGuiFriendNameGroupChangeClicked = function (friendID,prevGroupID)
         {
             var friendNameGroupDivID =
                 genFriendChangeNameGroupDivIDFromFriendID(friendID);
             
             var itemToToggle
                 = document.getElementById(friendNameGroupDivID);

             if (itemToToggle === null)
             {
                 sirikata.log('warn', '\\nWarning on Melville friend ' +
                              'change name/group clicked: do not have ' +
                              'associated friendID\\n');
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

                 //to change friend's name
                 var friendNameTAreaID =
                     genFriendChangeNameTextAreaIDFromFriendID(friendID);
                 
                 var newFriendName     = $('#'+friendNameTAreaID).val();


                 //to change friend's membership in group
                 var friendGroupTAreaID =
                     genFriendGroupChangeSelectID(friendID);

                 var newGroupID = $('#' + friendGroupTAreaID).val();

                 
                 sirikata.event('melvilleFriendNameGroupChange',
                                friendID,newFriendName,newGroupID,prevGroupID);

             }
         };


         melvilleAppGuiCreateGroupClicked = function()
         {
             var newGroupDivID = genCreateGroupDivID();

             var itemToToggle = document.getElementById(newGroupDivID);

             if (itemToToggle === null)
             {
                 sirikata.log('warn', '\\nWarning on Melville group ' +
                              'create clicked: do not have ' +
                              'associated item\\n');
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
                 var newGroupName    = $('#' + genNewGroupNameTAreaID()).val();
                 var newGroupStatus  = $('#' + genNewGroupStatusTAreaID()).val();
                 var newGroupProfile = $('#' + genNewGroupProfileTAreaID()).val();
                 
                 sirikata.event('melvilleAddGroup', newGroupName,
                                newGroupStatus, newGroupProfile);
             }
         };




         function generateNewFriendGroupDivID (requestRecID)
         {
             return requestRecID.toString() + '___newFriendID';
         }


         function genFriendChangeGroupSelectIDFromRequestID(requestRecID)
         {
             return requestRecID.toString() + '__GroupSelectID__';
         }

         function genFriendChangeNameTextAreaIDFromRequestID(requestRecID)
         {
             return requestRecID.toString() + '_textAreaIDFromRequestID__';
         }

         function genNewFriendSubmitDataID(requestRecID)
         {
             return requestRecID.toString() + '_submitDataID___';
         }

         
         /**
          \param {unique int} requestRecID the id of the request for
          friendship.  Should be passed back to Emerson layer to
          associate the user's repsonse to this request with the
          request itself.

          \param {array <unique int, string>} groupIDToGroupNames
          Lists all available groups to add friend to.
          */
         melvilleAppGuiNewFriendGroupID = function(
             requestRecID,groupIDToGroupNames)
         {
             var windowID = generateNewFriendGroupDivID(requestRecID);

             $('<div>'   +
               '</div>' //end div at top.
              ).attr({id:windowID,title:'melvilleFriendRequest'}).appendTo('body');
                          
             var melvilleFriendCompleteWindow = new sirikata.ui.window(
                 '#' + windowID,
                 {
	             autoOpen: false,
	             height: 'auto',
	             width: 300,
                     height: 300,
                     position: 'right'
                 }
             );
             
             var htmlToDisplay = 'What do you want to name your ' +
                 'new friend?  What group do you want to put ' +
                 'your new friend in? <br/><br/>';
             
             htmlToDisplay += 'friend name: <textarea id="'+
                 genFriendChangeNameTextAreaIDFromRequestID(requestRecID) +
                 '">' + 'some name' +
                 '</textarea> <br/>';

             htmlToDisplay += 'group: <select id="'+
                 genFriendChangeGroupSelectIDFromRequestID(requestRecID) +
                 '">';


             var firstItem = true;
             for (var s in groupIDToGroupNames)
             {
                 htmlToDisplay += '<option value=' + s.toString() + ' ';
                 if (firstItem)
                 {
                     htmlToDisplay += 'selected="selected"';
                     firstItem = false;
                 }
                 htmlToDisplay += '>' + groupIDToGroupNames[s];
                 htmlToDisplay += '</option>';
             }
             htmlToDisplay += '</select>';


             var submitFriendDataID = genNewFriendSubmitDataID(requestRecID);
             htmlToDisplay += '<div id="' + submitFriendDataID + '"  ' +
                 'onclick="newFriendDataSubmit(' + requestRecID.toString() +
                 ')">';
             
             htmlToDisplay += '<b> Submit</b>';
             htmlToDisplay += '</div>';

             $('#' + windowID).append(htmlToDisplay);
             melvilleFriendCompleteWindow.show();
         };

         /**
          Called when user hits submit when assigning new friend to
          group and name.
          */
         newFriendDataSubmit = function(requestRecID)
         {
             var windowID =  generateNewFriendGroupDivID(requestRecID);

             var groupSelectID = 
                 genFriendChangeGroupSelectIDFromRequestID(requestRecID);

             var nameTextAreaID =
                 genFriendChangeNameTextAreaIDFromRequestID(requestRecID);


             var newName = $('#' + nameTextAreaID).val();
             var newGroup = $('#' + groupSelectID).val();

             
             $('#' +windowID).remove();
             sirikata.event('userRespAddFriend',requestRecID,newName,newGroup);
         };
         
         @;
         
         
         returner += '});';
         return returner;
     }
     
 })();