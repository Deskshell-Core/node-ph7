var npm = require("npm").load({}, function(err, npm){
	var npmlog = require("npmlog"),
        prefix = "",
        os = process.platform;
    
    npmlog.heading="ph7/configure";
    npmlog.info(prefix, "Installing the ph7 binary...");
    
    npm.commands.install(["ph7-"+os], function(err){
    	if(err) {
    		npmlog.error(prefix, "ph7 could not fetch a binary for you. Check that a binary distribution for your platform exists.");
    		npmlog.error(prefix, "For bugs and issues, create an issue at: http://github.com/Deskshell-Core/node-ph7");
    		process.exit(1);
    	}
    });
});
