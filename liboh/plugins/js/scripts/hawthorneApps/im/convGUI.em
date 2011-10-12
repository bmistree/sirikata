
system.require('hawthorneApps/im/imUtil.em');

(function()
 {
     var WRITE_ME_EVENT     =      'WRITE_ME_EVENT';
     var WRITE_FRIEND_EVENT =  'WRITE_FRIEND_EVENT';
     var WARN_EVENT         =          'WARN_EVENT';

     function guiName(friendName)
     {
         return 'Messaging with '+ friendName;
     }
     
     
     /**
      @param {string} name
      @param {Friend} friend
      */
     ConvGUI = function(name,friend)
     {
         IMUtil.dPrint('\nGot into conv gui constructor.\n');
         this.friendName = name;
         this.friend = friend;
         this.uiID = IMUtil.getUniqueInt();
         this.guiInitialized = false;
         this.pendingEvents = [];
         this.guiMod = simulator._simulator.addGUITextModule(
             guiName(name),
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
         internalWarn(this,warnMsg);
     };

     /**
      Called when user enters text into tab that needs to be displayed.
      */
     ConvGUI.prototype.writeMe = function(toWrite)
     {
         if (!this.guiInitialized)
         {
             this.pendingEvents.push([WRITE_ME_EVENT,toWrite]);
             return;
         }
         internalWriteMe(this,toWrite);
     };

     /**
      Called when friend enters text into tab that needs to be displayed.
      */
     ConvGUI.prototype.writeFriend = function(toWrite)
     {
         if (!this.guiInitialized)
         {
             this.pendingEvents.push([WRITE_FRIEND_EVENT,toWrite]);
             return;
         }
         internalWriteFriend(this,toWrite);
     };

     
     /**
      Gets called by js display code when user enters data.  Already
      bound in guiInitFunc to convGUI.
      */
     function userInput(whatWrote)
     {
         IMUtil.dPrint('\n\nThis is what was written by user: ' +
                       whatWrote+ '\n\n');
         
         this.friend.msgToFriend(whatWrote);
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
                 internalWriteMe(this,this.pendingEvents[s][1]);
             else if (this.pendingEvents[s][0] == WRITE_FRIEND_EVENT)
                 internalWriteFriend(this,this.pendingEvents[s][1]);
             else
                 internalWarn(this,this.pendingEvents[s][1]);
         }
         this.pendingEvents = [];
     }

     /**
      @param {string-untainted} toWarnWith
      */
     function internalWarn(convGUI,toWarnWith)
     {
         convGUI.guiMod.call('warn',toWarnWith);
     }

     /**
      @param {string-untainted} message
      */
     function internalWriteFriend(convGUI,message)
     {
         convGUI.guiMod.call('writeFriend',message);
     }

     /**
      @param {string-untainted} message
      */
     function internalWriteMe(convGUI,message)
     {
         IMUtil.dPrint('\n\nGot into internalWriteMe with message: ' +
                       message +'\n\n');
         convGUI.guiMod.call('writeMe',message);
     }


     function getGUIText(friendName)
     {
         var returner = "sirikata.ui('" + guiName(friendName) + "',";
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
         
         
        $('<div>' + 
              '<div id="history" style="height:120px;width:250px;font:16px/26px Georgia, Garamond, Serif;overflow:scroll;">' +
              '</div>' + //end history
          
              '<input value="" id="melvilletarea" style="width:250px;">' +
              '</input>' +

              '<button id="melvilleChatButton">Enter</button>' +
          
          '</div>' //end div at top.
         ).attr({id:'melville-dialog',title:'melville'}).appendTo('body');

         var melvilleWindow = new sirikata.ui.window(
            '#melville-dialog',
            {
	        autoOpen: false,
	        height: 'auto',
	        width: 300,
                height: 400,
                position: 'right'
            }
        );

         sirikata.ui.button('#melvilleChatButton').click(
             function()
             {
                 sirikata.event('userInput',$('#melvilletarea').val());                 
             }
         );  

         melvilleWindow.show();
         @;
         
         
         //closes function passed to sirikata.ui and sirikata.ui.
         returner += '});' ;

         return returner;
     }
     
 })();



