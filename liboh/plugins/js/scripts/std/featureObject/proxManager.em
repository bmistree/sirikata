/**
 Should only be instantiated from featureObject.em

 Registers self as proximity manager.
 */


(function()
 {
     function ProxMessageManager()
     {
         
     }

     ProxMessageManager.prototype.onProxAdded = function(cb,runOnAll)
     {
         lkjs;
     };

     ProxMessageManager.prototype.onProxRemoved = function (cb)
     {
         lkjs;
     };
     
     var proxMsgMgr = new ProxMessageManager();
     system.__setProxManager(proxMsgMgr);     
 })();

