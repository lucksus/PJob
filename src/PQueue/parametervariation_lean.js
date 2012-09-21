var get_pjobfile_from_user = function() {
	var pjobFileParams = new Object();
	pjobFileParams["PJOB file"] = new Object();
	pjobFileParams["PJOB file"].type = "file";
	pjobFileParams["PJOB file"].filter = "PJOB files (*.pjob)";
	pjobFileParams["PJOB file"].caption = "Select PJOB file";
	var result1 = userInput(pjobFileParams);
	return result1["PJOB file"];
}

var parameters_that_already_have_variation_values = function(){
	if(!proxy.params) return new Array;
	var r_value = new Array;
	for(parameter in proxy.params)
		if(proxy.params[parameter].variation_values) r_value[r_value.length] = parameter;
	return r_value;
}

var array_contains = function(array, content){
	for(i in array)
		if(array[i] == content) return true;
	return false;
}

var array_minus = function(a,b){
	var r_value = new Array;
	for(i in a){
		if(!array_contains(b,a[i]))
			r_value.push(a[i]);
	}
	return r_value;
}

var get_variation_values_from_user = function(parameters_to_ask_for, parameters_info){
	var params = new Object();
	for(var i in parameters_to_ask_for){
		var p = parameters_to_ask_for[i];
		params[p+" min"] = new Object();
		params[p+" min"].type = "double";
		if(proxy.params && proxy.params[p] && proxy.params[p].min)
			params[p+" min"].defaultValue = proxy.params[p].min;
		else
			params[p+" min"].defaultValue = parameters_info[p].defaultValue;
		params[p+" max"] = new Object();
		params[p+" max"].type = "double";
		if(proxy.params && proxy.params[p] && proxy.params[p].max)
			params[p+" max"].defaultValue = proxy.params[p].max;
		else
			params[p+" max"].defaultValue = parameters_info[p].defaultValue;
		params[p+" step"] = new Object();
		params[p+" step"].type = "double";
		if(proxy.params && proxy.params[p] && proxy.params[p].step)
			params[p+" step"].defaultValue = proxy.params[p].step;
		else
			params[p+" step"].defaultValue = 1;
	}
	var variation_limits;
	var size = 0;
	for(i in params) size++;
	if(size > 0)
		variation_limits = userInput(params);
	else
		return new Object;
		
		
		
	var parameters = new Object();
	for(var i in parameters_to_ask_for){
		var p = parameters_to_ask_for[i];
		parameters[p] = new Object;
		parameters[p].min = variation_limits[p+" min"];
		parameters[p].max = variation_limits[p+" max"];
		parameters[p].step = variation_limits[p+" step"];
		parameters[p].variation_values = new Array();
		for(var v=parameters[p].min;v<=parameters[p].max;v+=parameters[p].step){
			parameters[p].variation_values.push(v);
		}
	}
	return parameters;
}

function merge_objects(obj1,obj2){
    var obj3 = {};
    for (var attrname in obj1) { obj3[attrname] = obj1[attrname]; }
    for (var attrname in obj2) { obj3[attrname] = obj2[attrname]; }
    return obj3;
}

//1. Read parameters from PJOB file:
var parameters_info = getPJobFileParameters();
var parameters_array = new Array;
for(x in parameters_info) parameters_array[parameters_array.length] = x;

print("Parameters found in PJOB file: " + parameters_array + "\n");
var parameters_already_set = parameters_that_already_have_variation_values();
if(parameters_already_set.length > 0) print("Parameters set via command line parameters: " + parameters_already_set + "\n");

//2. Get variation limits from user
var remaining_parameters = array_minus(parameters_array, parameters_already_set);
print("Remaining parameters to set limits for: " + remaining_parameters + "\n");
var parameters = get_variation_values_from_user(remaining_parameters, parameters_info);
if(proxy.params) parameters = merge_objects(parameters,proxy.params);

//3. Create parameter combinations as array of hashes
//combinations = [{length:100, power:3}, {length:200,power:4},...]
print("Creating parameter combinations...");
var combinations = new Array();
var parameter_names = new Array;
for(name in parameters) parameter_names.push(name);
var index = new Array();
var done = false;
for(var p in parameters) index.push(0);

function next_combination(){
	if(done) return 0;

	var combination = new Object();
	for(var i in index){
		combination[parameter_names[i]] = parameters[parameter_names[i]].variation_values[index[i]];
	}

	index[0]++;
	var i=0;
	while(index[i] >= parameters[parameter_names[i]].variation_values.length){
		index[i]=0;
		i++;
		if(i>=index.length){
			done = true;
			break;
		}
		index[i]++;
	}
	return combination;
}

function combination_count(){
	var count=1;
	for(var i in index){
		//print(parameter_names[i] +": "+ parameters[parameter_names[i]].variation_values.length+"\n");
		count = count * parameters[parameter_names[i]].variation_values.length;
	}
	return count;
}

function sleep(milliseconds) {
  var start = new Date().getTime();
  for (var i = 0; i < 1e7; i++) {
    if ((new Date().getTime() - start) > milliseconds){
      break;
    }
  }
}

//4. Create job instances
print("Doing parametervariation with " + combination_count() + " combinations...");
PQueue.clearFinishedJobs();
var job_count = 0;
PQueue.start();
while(job_count < combination_count() || !done){
	Script.progress = job_count / combination_count() * 100;
	job_count += PQueue.finishedJobs().length;
	print("job_count: "+job_count+"\n");
	PQueue.clearFinishedJobs();
	var threads = Pool.max_thread_count() - Pool.thread_count();
	print("Pool.max_thread_count: "+Pool.max_thread_count()+"\n");
	print("Pool.thread_count(): "+Pool.thread_count()+"\n");
	if(threads - PQueue.submittedJobs().length - PQueue.queuedJobs().length <= 0){
		sleep(1000);
		continue;
	}
	var failed_jobs = PQueue.failedJobs();
	var jobs = new Array;
	
	for(var i=0;i<threads;i++){
		var job;
		if(failed_jobs.length>0){
			job = failed_jobs.pop();
			job.requeue();
		}else{
			if(done)break;
			job = new Job(next_combination());
			PQueue.addJob(job);
		}
		jobs.push(job);
	}
	//for(var i in jobs){
	//	try{
	//		jobs[i].waitUntilRunning();
	//	}catch(e){};
	//}
}
PQueue.stop();
Script.progress = 100;
print("done!\n");
