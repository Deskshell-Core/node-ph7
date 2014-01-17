var fs=require("fs");
var os=require("os").platform();
if(fs.existsSync(__dirname+"/build/Release/ph7.node")) {
	module.exports=require(__dirname+"/build/Release/ph7.node");
} else if(fs.existsSync(__dirname+"/bin/"+os+"/ph7")) {
	module.exports=require(__dirname+"/bin/"+os+"/ph7");
} else {
	console.error("!! Unable to load native addon. Not found.");	
	console.error("!! Not found in: "+__dirname+"/bin/"+os+"/ph7");
	console.error("!! Not found in: "+__dirname+"/build/Release/ph7.node");
	module.exports={};
}
