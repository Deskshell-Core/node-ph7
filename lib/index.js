var os=require("os").platform();

// client should have a binary distro, like ph7-darwin, ph7-win32, etc...
module.exports=require("ph7-"+os);
