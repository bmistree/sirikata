
(function()
 {
     RoomGui = function(rmID,room)
     {
         this.rmID = rmID;
         this.guiInitialized = false;
         this.room = room;
         this.guiMod = simulator._simulator.addGUITextModule(
             guiName(this),
             getRoomGuiText(this),
             std.core.bind(guiInitFunc,undefined,this)
         );


         system.__debugFileWrite(getRoomGuiText(this),'testRoomGui.em');
     };


     /**
      Creates a list of friends that are in the room and a list that
      are outside it.  Returns each of these lists to gui part of the
      application.  Each list has elements with the following format:
      elem 1: [friendName_1, friendID_1]
      elem 2: [friendName_2, friendID_2]
      ...
      */
     function requestMembershipDialogEmerson(roomGui)
     {
         IMUtil.dPrint('\n\nGot into requestMembershipDialogEmerson function\n\n');
         
         //find list of friends that are already in the room:
         var inRoom = {};
         for (var s in roomGui.room.friendMap)
         {
             var friend = friendMap[s];
             inRoom[friend.imID] = [friend.name,friend.imID];
         }

         //find list of friends that are not already in room:
         var notInRoom ={};
         for (var s in roomGui.room.appGui.imIDToFriendMap)
         {
             IMUtil.dPrint('In requestMembershipDialogEmerson of roomGui.em: ' +
                           'this check will not actually work: friends ' +
                           'have different ids since they are room ' +
                           'friends or individual friends.'
                          );

             if (s in inRoom)
                 continue;

             var friend = roomGui.room.appGui.imIDToFriendMap[s];
             notInRoom[friend.imID] = [friend.name, friend.imID];
         }

         //actually pop up a new requestMembershipDialog window in gui.
         roomGui.guiMod.call('requestMembershipDialog',inRoom,notInRoom);
     }
     

     /**
      @param {map<int,int>} added - Keys are the ids of friends that
      have been added to room.  Values do not matter.
      
      @param {map<int,int>} added - Keys are the ids of friends that
      have been removed from room.  Values do not matter.

      Note: added and removed cannot have same members.
      */
     function membershipChange(roomGui,added,removed)
     {
         //add new friends
         for(var s in added)
         {
             if (!(s in roomGui.room.appGui.imIDToFriendMap))
             {
                 IMUtil.dPrint('\nFriend id no longer ' +
                               'available in roomGui.em.');
                 continue;
             }
             
             var friendToAdd = roomGui.room.appGui.imIDToFriendMap[s];
             roomGui.room.addFriend(friendToAdd);
         }

         //remove
         for (var s in removed)
             roomGui.room.removeFriend(s);
     }
     
     function guiInitFunc(roomGui)
     {
         IMUtil.dPrint('\n\n\nGot into gui init func\n\n');
         
         //called when user clicks membership dialog
         var wrappedRequestMembDialogEmerson =  std.core.bind(
             requestMembershipDialogEmerson,undefined,roomGui);
         
         roomGui.guiMod.bind('requestMembershipDialogEmerson',
                             wrappedRequestMembDialogEmerson);


         //called when user finishes changing membership of room.
         var wrappedMembershipChange = std.core.bind(
             requestMembershipChange,undefined,roomGui
         );
         roomGui.guiMod.bind('membershipChange',
                             wrappedMembershipChange);
         
     }

     function guiName(roomGui)
     {
         return 'Room_controller_for_room ' + roomGui.rmID.toString();
     }


     
     function getRoomGuiText(roomGui)
     {
         var name = 'melvilleRoomManagement__' + roomGui.rmID.toString();
         var returner = "sirikata.ui('" + guiName(roomGui)  + "',";
         returner += 'function(){ ';

         returner += 'var roomCtrlDivName = "' + name + '";';
         returner += 'var roomMembershipDivName ="' + name + 'Member";';
         returner += @

         //gui for displaying room controls.
         $('<div>' +

           '<button id=' + getMelvilleRoomGuiMembershipID()+'> Change group membership</button>' +
           
           '</div>' //end div at top.
          ).attr({id:roomCtrlDivName,title:'melvilleRoom'}).appendTo('body');


         function getMelvilleRoomGuiMembershipID()
         {
             return roomCtrlDivName+'__changeGroupMembershipID';
         }
         
         var roomWindow = new sirikata.ui.window(
            '#' + roomCtrlDivName,
            {
	        autoOpen: false,
	        height: 'auto',
	        width: 300,
                height: 400,
                position: 'right'
            }
         );
         roomWindow.show();

         //executed when change membership button clicked: sends message down
         //to emerson roomGui code 
         sirikata.ui.button('#' + getMelvilleRoomGuiMembershipID()).click(
             function()
             {
                 sirikata.event('requestMembershipDialogEmerson');             
             }
         );
         
         
         //gui for displaying membership controls
         $('<div>' +
           '</div>' //end div at top.
          ).attr({id:roomMembershipDivName,title:'melvilleRoom'}).appendTo('body');

         var membershipWindow = new sirikata.ui.window(
            '#' + roomMembershipDivName,
            {
	        autoOpen: false,
	        height: 'auto',
	        width: 300,
                height: 400,
                position: 'right'
            }
         );
         membershipWindow.hide();


         /**
          \param entry - \see generateInRoomDiv
          */
         function getDivIDMemDialog(entry)
         {
             return 'melville_room_management_div__' +
                 roomCtrlDivName + 
                 entry[1].toString();
         }
         
         /**
          \param {array} entry - first element of entry array is the
          name of the friend, the second is the imID of the friend.

          \param {bool} inRoom - True if the entry is currently in the
          room.  False otherwise.
          
          \return {string} - html for div enclosing this element.
          */
         function generateMembershipDiv(entry,inRoom)
         {
             var membDivID = getDivIDMemDialog(inRoom[s]);
             var newHtml = '';
             newHtml += '<div id="' + membDivID + '"';
             if (inRoom)
                 newHtml += ' onclick="melvilleRmMembFuncRemove( ';
             else
                 newHtml += ' onclick="melvilleRmMembFuncAdd( ';
                 
             newHtml += '\\'' + inRoom[s][0] + '\\',\\'' + inRoom[s][1] + '\\'")>';
             
             newHtml += inRoom[s][0];
             newHtml += '</div>';             

             return newHtml;
         }

         function generateInRoomTableCellDivID()
         {
             return roomCtrlDivName + '__inRoomTableCellID';
         }

         function generateNotInRoomTableCellDivID()
         {
             return roomCtrlDivName + '__notInRoomTableCellID';
         }
         
         
         //this gets called
         requestMembershipDialog = function(inRoom,notInRoom)
         {
             var newHtml = '<table><tr><td>In room</td>' +
                 '<td>Not in room</td></tr>';

             newHtml += '<tr><td id="' + generateInRoomTableCellDivID() +
                 '">';
             
             for (var s in inRoom)
                 newHtml += generateMembershipDiv(inRoom[s],true);

             newHtml+= '</td><td id="' + generateNotInRoomTableCellDivID() +
                 '">';

             for (var s in notInRoom)
                 newHtml += generateMembershipDiv(notInRoom[s],false);

             newHtml += '</td></tr></table>';
             
             var jqueryMembershipID = '#' + roomMembershipDivName;
             $(jqueryMembershipID).html(newHtml);
             membershipWindow.show();
         };
         @;

         //close the onready function and the sirikata.ui 
         returner += '});'
         return returner;
     }
     
 })();