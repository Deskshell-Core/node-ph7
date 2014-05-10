{
	"targets": [{
    	"target_name": "ph7",
    	"sources": [ 
    		"src/node_ph7.cc", 
    		"src/ph7_wrapper.cpp",
    		"src/ph7/ph7.c"
    	],
    	"defines": [ "NDEBUG" ],
    	"include_dirs": [ "src/ph7" ]
	}]
}
