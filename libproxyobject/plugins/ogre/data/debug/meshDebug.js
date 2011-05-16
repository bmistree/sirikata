$(document).ready(
    function()
    {
        $('<div id="debug-mesh-ui" title="CheckMeshes">' +
          '<img id="currMeshImg" src="http://open3dhub.com/download/988525fdc6d8a8baa1e18a9fd796bfe3325c6775540d8a26750f73c5109311a3"></img>' +
          '<button id="nextMeshButton">Next</button>' +
          '<div id="meshDescriptionText"> default image</div>' + 
          '</div>').appendTo('body');

        $( "#debug-mesh-ui" ).dialog({            
            width: 300,
            height: 'auto',
            modal: false
        });

        $('#nextMeshButton').click(loadNext);

        allMeshes = [];
        currentMesh = null;
        

        $.ajax({
                   url: 'http://open3dhub.com/api/browse/1304983088276662',
                   type: 'GET',
                   dataType:'json',
                   success: allMeshesFunc,
                   //success: changeFrame2,
                   crossDomain: true
               });
        
        
    }
);



function allMeshesFunc(dataGotten,status, xhr)
{
    console.log('In allMeshes');
    for (var s in dataGotten.content_items)
    {
        if (typeof(dataGotten.content_items[s].metadata.types.original.thumbnail) != 'undefined')
        {
            allMeshes.push([dataGotten.content_items[s].metadata.types.original.thumbnail, dataGotten.content_items[s].full_path]);
            console.log(dataGotten.content_items[s].metadata.types.original.thumbnail);
        }
    }

    loadNext();
    console.log('Exiting allMeshes');
}

//Takes a string like this:
// "d3308661de134382afafce26376c84c9d29a6bd319d9c3f3ac501d17f75f0e60"
//turns it into this:
//  http://open3dhub.com/download/d3308661de134382afafce26376c84c9d29a6bd319d9c3f3ac501d17f75f0e60
function formSource(str)
{
    return 'http://open3dhub.com/download/'+str;
}

//Takes a string like this:
// /bmistree/models/model.dae/0
//and turns it into:
//  Can use this model if set mesh to: meerkat:///bmistree/models/model.dae/original/0/model.dae
function formDescriptionText(str)
{
    return "Can use this model if set mesh to: meerkat://"+str;
}



function loadNext()
{
    if (currentMesh == null)
        currentMesh =0;
    else
        currentMesh++;

    if (currentMesh >= allMeshes.length)
        currentMesh =0;

    
    $('#currMeshImg').attr("src",  formSource(allMeshes[currentMesh] [0])  );
    $('#meshDescriptionText').text(  formDescriptionText(allMeshes[currentMesh][1])  );
}