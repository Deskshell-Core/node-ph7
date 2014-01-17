# node-ph7 - in-process PHP for NodeJS.

This cool extension allows you to run PHP code based off the 5.3.3 standart from inside nodejs. It has a very clean API and aims to be simple, yet useful. It still lacks some configuration, but it can do what is is supposed to already.


## Features

- Run PHP script from string.
- Run PHP script from file.
- Configure $_SERVER, $_ENV, $_HEADER, $_COOKIES, $_GET, $_POST, $_SESSION, $GLOBALS and $argv
- Bring in new super globals too!


# Yet missing but to come very very soon:

- Constant declaration


# On my ToDo list:

- Configure the engine to:
	* Buffer standart output (then accessible as object property)
	* Buffer error output (same as above)
	* Report all errors
	* Run without <?php ?> - plain php scripts!


# How to use it

You should look at test.js and test.php - but here is the API for you :)

```js
var ph7 = require("ph7"),
	// Create a new virtual maschine for PHP
    pVM = ph7.create();

// Concigure the variables you want to...
pVM.$_ENV['my_awesome_app_name']="Meep Meep!";
// This also works for the other super globals too:
pVM.$_GET['ajax']=true;
pVM.$_POST['userName']="fubar";
pVM.$_SERVER['NODE_VERSION']=NODEJS_VERSION;
pVM.$_SESSION['randomNumber']=42;
pVM.$_HEADER['Upgrade:']="WebSocket";
pVM.$_COOKIE['is']="very tasty!";

// But that's not all... :)
// To create a new, normal, variable as you would know it in PHP, use $GLOBALS.
pVM.$GLOBALS['appName']="meepify";
// To create a SUPER global (like $_GET), use the special $SGLOBALS array:
pVM.$SGLOBALS['_MODULES']=["fs","os"];
// And if you want, you can even use $argv.
pVM.$argv[] = "--help";

// Now, that is how you set THAT up. But how about we become a bit more explicit and communicate directly with ph7_vm_config?
pVM.config(ph7.PH7_VM_CONFIG_RECURSION_DEPTH, 100); // See: [ph7_vm_config(pVM,int,...)](http://ph7.symisc.net/c_api_func.html#ph7_vm_config)

// How about a simple script to demonstrate the power?
var script = "<?php echo 'Hello, nodeWorld!\n';";
// Compile the script; that means, that the VM will be made ready with this script.
// Behind the scenes: The engine creates bytecode of the script and prepairs execution of that.
pVM.compile(script);
// Alternatively, you could do that too:
// pVM.compileFile(__dirname+"/myScript.php");

// Prepair the execution. This will save all the custom variables and the like into the virtual maschine - for real.
pVM.prepair();

// Run it!
var exitCode = pVM();
```

As you just saw, if you call the virtual maschine as a function, it will execute and return the script's exit code. The content currently is streamed right to stdout. In a future update, you may turn that off by passing an object into the create() call. After that, you may use pVM.output to obtain the output that the VM delivers.


# Some last words.

This project is based off [ph7](http://ph7.symisc.net/), and therefore their license is included in the ph7 subfolder. The version I am using is from their website.

Have fun with this! :)
