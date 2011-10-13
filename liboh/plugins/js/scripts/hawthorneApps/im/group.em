/**
 @param {String} groupName Human readable name of the group.
      
 @param {unique int} groupID unique id for the group.
      
 @param {string} status status to be displayed to all memebers of the
 group.

 @param {string} profile Information about me to be displayed to all
 members of the group.

 @param {bool} visible When a group is invisible (ie, this boolean is
 false), a Friend object in that group won't acknowledge any message
 sent to it.

 @param {AppGUI} appGui Used to display information back to the user
 when errors may occur.  (Eg. try to change status when you're
 invisible.)
      
 Friends can be in different groups.  Each group keeps track of a
 separate status and profile.  These are just strings that get
 displayed on friends' guis when they request additional information.
 */
function Group(groupName,groupID,status,profile,visible,appGui)
{
    this.groupName = groupName;
    this.groupID = groupID;
    this.status  = status;
    this.profile = profile;
    this.visible = visible;

    this.appGui = appGui;
         
    //map from friend id to friends
    this.friendsInGroup = {};
}

Group.prototype.changeStatus = function(newStatus)
{
    this.status  = newStatus;
    
    if (!this.visible)
    {
        this.appGui.warn('You are currently invisible to this group.  '+
                         'Members of the group will not see this ' +
                         'status change until you become visible to '+
                         'them again.');
        return;
    }

         
    for (var s in this.friendsInGroup)
        this.friendsInGroup[s].updateStatusToFriend(newStatus);

};

Group.prototype.changeProfile = function(newProfile)
{
    this.profile  = newProfile;
    
    if (!this.visible)
    {
        this.appGui.warn('You are currently invisible to this group.  '+
                         'Members of the group will not see this ' +
                         'profile change until you become visible to '+
                         'them again.');
        return;
    }

         
    for (var s in this.friendsInGroup)
        this.friendsInGroup[s].updateProfileToFriend(newProfile);
};

Group.prototype.changeVisible = function(newVisible)
{
    var prevVisible = this.visible;
    this.visible = newVisible;

    //if we've gone from invisible to visible, automatically send
    //out profile and status: this way, any changes to prof and
    //status that occurred while invisible get propagated out.
    if ((!prevVisible) && newVisible)
    {
        this.changeProfile(this.profile);
        this.changeStatus(this.status);
    }
};
     
     
Group.prototype.addMember = function(friendToAdd)
{
    this.friendsInGroup[friendToAdd.imID] = friendToAdd;
    friendToAdd.updateProfileAndStatusToFriend(this.profile,this.status);
};

Group.prototype.removeMember = function(friendToRemove)
{
    if (friendToRemove.imID in this.friendsInGroup)
        delete this.friendsInGroup[friendToRemove.imID];
    else
    {
        IMUtil.dPrint('\nWarning: trying to remove a friend ' +
                      'from a group he/she does not belong to.\n');
    }
};
