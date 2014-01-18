{
	"targets": [{
      	"target_name": "ph7",
      	"sources": [ 
      		"node_ph7.cc", 
      		"ph7_wrapper.cpp",
      		"ph7/ph7.c"
      	],
      	"defines": [ "NDEBUG" ],
      	"include_dirs": [ "ph7" ]
	}]
}
