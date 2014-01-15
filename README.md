# node-ph7 - Run PHP inside NodeJS!

Just imagine. You have a web server already running and can serve static HTML pages....but now you figure, you want to do some php scripting? Well ouch, now you would have to install PHP itself.

Not anymore. :)

node-ph7 brings an in-process PHP engine into your nodejs process and lets you execute stuff from it too. Currently, it is not async, but I want to make it async. That way, you amy even apply a custom callback to output all the data asynchronously.


## Key features

- Run PHP scripts inside a nodejs context
- Modify and customize the script's environment ($_SERVER, $_ENV, $_SESSION, $_COOKIE, ...)
- Run scripts with, or without `<?php ... ?>`
- You also can run a script from a string, instead from a file.
- Retrive exit codes, buffer the entire output or errors.


## Todo

- Install custom functions into the environment (TODO)


# This is currently not working!

I am still working on this, but I wanted to share my current codebase. :)
