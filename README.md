# node-ph7 - in-process PHP for NodeJS.

This cool extension allows you to run PHP code based off the 5.3.3 standart from inside nodejs. It has a very clean API and aims to be simple, yet useful. It still lacks some configuration, but it can do what is is supposed to already.


# Install

As with each npm module:

    npm install ph7

But if you use github:

    git clone http://github.com/Deskshell-Core/node-ph7 ph7
    npm install ./node-ph7


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

As you just saw, if you call the virtual maschine as a function, it will execute and return the script's exit code. The content currently is streamed right to stdout. In a future update, you may turn that off by passing an object into the create() call. After that, you may use pVM.output to obtain the output that the VM delivers.


# Binary distros...

To manage binary distirbutions, I simply createed a new package called 'ph7-darwin', which will work on OS X. So if you want ph7 to work on a new platform, you can build the native addon yourself. For that, do this:

    npm -g install node-gyp
    cd src
    node-gyp rebuild

Note, that for this, you need python, make and a C/C++ compiler (GCC, Clang, ...).
Once you finished building, you get a file at `build/Release/ph7.node`. Now, copy that .node file somewhere else, and create a new npm module. If you want a base to start on, use `ph7-darwin` - its just a js file that redirects the require call towards the native addon. Keep in mind that the `configure.js` script installs the binaries by using `process.platform`. Hence, you need to name your build something like **ph7-win32**.
Then, just publish the new build, and you've successfuly contributed something. :)
You may also let me know of your build so I can mention it later in this readme. 


# Some last words.

This project is based off [ph7](http://ph7.symisc.net/), and therefore their license is included in the ph7 subfolder. The version I am using is from their website.

Have fun with this! :)
