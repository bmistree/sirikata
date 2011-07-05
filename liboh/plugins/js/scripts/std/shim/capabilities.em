

if (typeof (std) ===  'undefined')
    util = {  };


util.Capabilities = function()
{
    this.capPerms = 1;

    for (var s =0; s < arguments.length; ++s)
        this.capPerms*=arguments[s];            
};



util.Capabilities.SEND_MESSAGE     = 2;
util.Capabilities.RECEIVE_MESSAGE  = 3;
util.Capabilities.IMPORT           = 5;
util.Capabilities.CREATE_PRESENCE  = 7;
util.Capabilities.CREATE_ENTITY    = 11;
uill.Capabilities.EVAL             = 13;
util.Capabilities.PROX_CALLBACKS   = 17;
util.Capabilities.PROX_QUERIES     = 19;
util.Capabilities.CREATE_SANDBOX   = 23;
util.Capabilities.GUI              = 29;
util.Capabilities.HTTP             = 31;




util.Capabilities.prototype.__getType = function()
{
    return "capabilities";
};

util.Capabilities.prototype.createSandbox = function(presence,visible)
{
    var permProduct=capObj.capPerms;

    //initialize all permission values
    var canSendMsg       = (permProduct % util.Capabilities.SEND_MESSAGE) == 0;
    var canRecvMsg       = (permProduct % util.Capabilities.RECEIVE_MESSAGE) == 0;
    var canImport        = (permProduct % util.Capabilities.IMPORT) == 0;
    var canCreatePres    = (permProduct % util.Capabilities.CREATE_PRESENCE) == 0;
    var canCreateEnt     = (permProduct % util.Capabilities.CREATE_ENTITY) == 0;
    var canEval          = (permProduct % util.Capabilities.EVAL) == 0;
    var canProxCallback  = (permProduct % util.Capabilities.PROX_CALLBACKS) == 0;
    var canProxQuery     = (permProduct % util.Capabilities.PROX_CALLBACKS) == 0;
    var canCreateSbox    = (permProduct % util.Capabilities.CREATE_SANDBOX) == 0;
    var canGui           = (permProduct % util.Capabilities.GUI) == 0;
    var canHttp          = (permProduct % util.Capabilities.HTTP) == 0;

    return system.__createSandbox(presence,
                                  visible,
                                  permProduct);

};

