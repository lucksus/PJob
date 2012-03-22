//-----------------------------------------------------------
// Get PHO file from user:
var phoFileParams = new Object();
phoFileParams["PHO file"] = new Object();
phoFileParams["PHO file"].type = "file";
phoFileParams["PHO file"].filter = "PHO files (*.pho)";
phoFileParams["PHO file"].caption = "Select PHO file";
var result1 = userInput(phoFileParams);
var phoFile = result1["PHO file"];
if(!isExistingFile(phoFile)) throw "no such file '"+phoFile+"'!";
//-----------------------------------------------------------

//Read global variables from PHO file:
var phoParameters = readGlobalVariablesFromPHOFile(phoFile);

//-----------------------------------------------------------
// Get remaining parameters from user:
var params = new Object();
params["Result"] = new Object();
params["Result"].type = "string";
params["Search"] = new Object();
params["Search"].type = "boolean";
params["Search"].falseName = "maximum";
params["Search"].trueName = "minimum";
for(var p in phoParameters){
	params[p+" min for initial region"] = new Object();
	params[p+" min for initial region"].type = "double";
	params[p+" min for initial region"].defaultValue = phoParameters[p][0];
	params[p+" max for initial region"] = new Object();
	params[p+" max for initial region"].type = "double";
	params[p+" max for initial region"].defaultValue = phoParameters[p][phoParameters[p].length - 1];
}
for(var p in phoParameters){
	params["Target size for "+p] = new Object();
	params["Target size for "+p].type = "double";
}
params["FAC"] = new Object();
params["FAC"].type = "double";
params["FAC"].defaultValue = 0.8;
params["Sample count"] = new Object();
params["Sample count"].type = "integer";
params["Sample count"].defaultValue = 4;
var results2 = userInput(params);
//-----------------------------------------------------------

//-----------------------------------------------------------
// Process parameters:
var FAC = results2.FAC;
var sampleCount = results2["Sample count"];
var searchMinimum = results2.Search;
var result = results2.Result;
var region = new Object();
for(var p in phoParameters){
	region[p] = new Object();
	region[p]["min"] = results2[p+" min for initial region"];
	region[p]["max"] = results2[p+" max for initial region"];
}
var sizes = new Object();
for(var p in phoParameters){
	sizes[p] = results2["Target size for "+p];
}
//-----------------------------------------------------------


function isBigger(region, sizes){
	var b = false;
	for(var p in region){
		var s = region[p].max - region[p].min;
		if(s > sizes[p]) b = true;
	}
	return b;
}

function updateRegionAround(pos, region){
	var distances = new Object();
	for(var p in region){
		distances[p] = region[p].max - region[p].min;
	}
	
	var result = new Object();
	for(var p in distances){
		distances[p] = distances[p] * FAC;
		result[p] = new Object();
		result[p]["max"] = pos[p] + distances[p]/2;
		result[p]["min"] = pos[p] - distances[p]/2;
	}
	
	return result;
}

function printRegion(region){
	for(var param in region){
		print(param+": "+region[param].min+" - "+region[param].max+"\n");
	}
}

function printPoint(point){
	for(var p in point){
		print(p+": "+point[p]+"\n");
	}
}



//-----------------------------------------------------------
//-----------------------------------------------------------
// MAIN ALGORITHM:
print("Starting optimization!\n");
PQueue.start(sampleCount);
do{
	var newPoints = createLHDPoints(region,sampleCount);
	var newJobs = new Array();
	
	for(var i in newPoints){
		var job = new PhotossJob(phoFile,newPoints[i]);
		PQueue.addJob(job);
		newJobs.push(job);
	}
	
	print("Created "+sampleCount+" jobs.\n");
	print("Waiting for results...");
	
	for(var i in newJobs){
		newJobs[i].waitUntilFinished();
	}
	
	print("Done!\n");
	
	print("Creating interpolant...");
	var interpolant = new Interpolant(phoFile, result);
	print("Done!\n");
	print("Searching optimum...");
	var optimumPosition = new Object();
	if(searchMinimum)	optimumPosition = interpolant.findMinimum();
	else				optimumPosition = interpolant.findMaximum();
	print("Done!\n");
	
	print("Found optimum at:\n");
	printPoint(optimumPosition);
	
	region = updateRegionAround(optimumPosition,region);
	print("Updated region is:\n");
	printRegion(region);
	print("-------------");
}while(isBigger(region,sizes));
PQueue.stop();
//-----------------------------------------------------------
//-----------------------------------------------------------

//-----------------------------------------------------------
// Output:
print("Result:\n");
printRegion(region);
//-----------------------------------------------------------








