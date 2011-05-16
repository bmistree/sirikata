var meshes = [];
meshes.push("meerkat:///bmistree/models/warehouse_model.dae/original/1/warehouse_model.dae");
meshes.push("meerkat:///bmistree/models/model.dae/original/2/model.dae");
meshes.push("meerkat:///bmistree/models/model.dae/original/3/model.dae");
meshes.push("meerkat:///bmistree/models/model.dae/original/4/model.dae");



function createIt(meshToChangeTo, newPos)
{
    //First thing that the new entity will do after its presence connects
    //to space is import the following file.
    var newEntExec = function()
    {
        system.require('std/default.em');
    };


    system.createEntityScript(newPos,
                              newEntExec,
                              null,
                              3,  //what is the solid angle query that
                              //the entity's initial presence
                              //queries with.
                              meshToChangeTo
                             );
}





var posToCreateIn = system.self.getPosition();
for (var s in meshes)
{
    posToCreateIn = posToCreateIn + <6,0,0>;
    createIt(meshes[s], posToCreateIn);
}





    
