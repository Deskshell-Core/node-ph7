var ph7 = require("ph7");

var pVM = ph7.create({
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

pVM.compileFile("./script.php");
// or: pVM.compile("<?php echo 'ovo'; ?>");

// Run it as a function to execute it - how nice! :)
var exitCode = pVM();

pVM.config({ outputBuffer: true });
var exitCode2 = pVM(),
    output = pVM.output;
