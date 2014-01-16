var ph7 = require("./build/Release/ph7");
console.log(ph7);
var pVM = ph7.create();
console.log(pVM);
var server = pVM.$_SERVER;
