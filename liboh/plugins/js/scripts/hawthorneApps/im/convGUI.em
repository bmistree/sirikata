
system.require('hawthorneApps/im/imUtil.em');

(function()
 {
     var WRITE_ME_EVENT     =      'WRITE_ME_EVENT';
     var WRITE_FRIEND_EVENT =  'WRITE_FRIEND_EVENT';
     var WARN_EVENT         =          'WARN_EVENT';
     
     
     /**
      @param {string} name
      @param {Friend} friend
      */
     ConvGUI = function(name,friend)
     {
         this.friendName = name;
         this.friend = friend;
         this.uiID = IMUtil.getUniqueInt();
         this.guiInitialized = false;
         this.pendingEvents = [];
         this.guiMod = simulator.addGUITextModule(
             'Messaging with ' + name,
             getGUIText(name),
             std.core.bind(guiInitFunc,this)
         );
     };

         
     /**
      @param {string -- untainted} warnMsg
      */
     ConvGUI.prototype.warn = function(warnMsg)
     {
         if (!this.guiInitialized)
         {
             this.pendingEvents.push([WARN_EVENT,warnMsg]);
             return;
         }
         internalWarn(warnMsg);
     };

     /**
      Called when user 
      */
     ConvGUI.prototype.writeMe = function(toWrite)
     {
         if (!this.guiInitialized)
         {
             this.pendingEvents.push([WRITE_ME_EVENT,toWrite]);
             return;
         }
         internalWriteMe(toWrite);
     };


     /**
      Gets called by js display code when 
      */
     function userInput(whatWrote)
     {
         this.friend.msgToFriend(whatWrote);
         this.writeMe(whatWrote);
     }

     /**
      Bound in @see ConvGui function, and called by gui when it's been
      initialized.
      */
     function guiInitFunc()
     {
         //gets called when user enters text
         this.guiMod.bind('userInput',std.core.bind(userInput,this));

         //clear all pending events that didn't occur until gui was created.
         this.guiInitialized = true;
         for (var s in this.pendingEvents)
         {
             if(this.pendingEvents[s][0] == WRITE_ME_EVENT)
                 internalWriteMe(this.pendingEvents[s][1]);
             else if (this.pendingEvents[s][0] == WRITE_FRIEND_EVENT)
                 internalWriteFriend(this.pendingEvents[s][1]);
             else
                 internalWarn(this.pendingEvents[s][1]);
         }
         this.pendingEvents = [];
     }

     /**
      @param {string-untainted} toWarnWith
      */
     function internalWarn(toWarnWith)
     {
         this.gui.call('warn',toWarnWith);
     }

     /**
      @param {string-untainted} message
      */
     function internalWriteFriend(message)
     {
         this.gui.call('writeFriend',message);
     }

     /**
      @param {string-untainted} message
      */
     function internalWriteMe(message)
     {
         this.gui.call('writeMe',message);
     }


     function getGUIText(friendName)
     {
         var returner = "sirikata.ui('" + friendName + "',";
         returner += 'function(){ ';


         returner += 'var FRIEND_NAME  = "'+friendName+'";';
         
         //fill in guts of function to execute for gui module
         returner += @
         var ME_NAME      = 'me';
         var SYS_NAME     = 'system';

         var ME_COLOR     = 'red';
         var FRIEND_COLOR = 'blue';
         var SYS_COLOR    = 'green';

         
         
         //internal to gui display
         writeMe = function(msg)
         {
             var formattedMsg = "<font color=ME_COLOR> "
                 + ME_NAME + "</font>: " + msg;
             
             writeToLog(formattedMsg);
         };

         //internal to gui display
         writeFriend = function(msg)
         {
             var formattedMsg = "<font color=FRIEND_COLOR> "
                 + FRIEND_NAME + "</font>: " + msg;
             
             writeToLog(formattedMsg);
         };

         //internal to gui display
         warn = function(msg)
         {
             var formattedMsg = "<font color=SYS_COLOR> "
                 + SYS_NAME + "</font>: " + msg;
             
             writeToLog(formattedMsg);
         };


         //appends the string to the end of the scrolling chat log.
         function writeToLog(msgToWrite)
         {
             $('#history').append(msgToWrite + '<br />');
         }
         
         
        $('<div id="history" style="height:120px;width:250px;font:16px/26px Georgia, Garamond, Serif;overflow:scroll;">' +
          '</div>' +
          
          '<textarea id="tarea" style="width:250px;">' +
          '</textarea>'
         ).appendTo('body');

         $('#history').append('I got into here');
         @;
         
         
         //closes function passed to sirikata.ui and sirikata.ui.
         returner += '});' ;

         return returner;
     }
     
 })();



