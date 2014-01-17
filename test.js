var ph7 = require("./build/Release/ph7");
var pVM = ph7.create();

pVM.$SGLOBALS['Deskshell']="A soon to be sdk :)";

var pfile = __dirname+"/test.php";
console.log("--> Going to run php file: "+pfile);

pVM.compileFile(pfile);
pVM.prepair();
pVM();
