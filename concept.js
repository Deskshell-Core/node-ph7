var ph7 = require("ph7");

var pVM = ph7.createVM({
	// global config
	outputBuffer: false,
	errorBuffer: false,
	useTags: true,
	includePath: [ __dirname ],
	reportAll: false
	server: {
		// $_SERVER config
		"nodejs": true
	},
	env: {
		// $_ENV
		"_CWD": __dirname
	}
});
// Add a super global
pVM.addSuper("NODEJS_VERSION","0.10.11");
// Add a normal var
pVM.addVar("nonGlobal","This is absolutely not global.");
// Add to $_SESSION, $_POST, $_GET...
pVM.$_SESSION["nonGlobal"] 		= "This is absolutely not global.";
pVM.$_GET["nonGlobal"] 		= "This is absolutely not global.";
pVM.$_POST["nonGlobal"] 	= "This is absolutely not global.";
pVM.$_COOKIE["nonGlobal"] 	= "This is absolutely not global.";
pVM.$_HEADER["nonGlobal"] 	= "This is absolutely not global.";
pVM.$_SERVER["..."]			= "foobar";
pVM.$_ENV["..."]			= "foobar";
pVM.$argv[] 				= "beep";

// run
var exitcode1 = pVM.runScript("<?php echo 'OvO'; ?>");
var exitcode2 = pVM.runFile(__dirname+"/script.php");

pVM.config({ outputBuffer: true });
var exitcode3 = pVM.runScript("<?php echo 'OvO'; ?>");
var output = pVM.output, error = pVM.error;
