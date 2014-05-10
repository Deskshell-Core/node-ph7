{
	"targets": [{
    	"target_name": "ph7",
    	"sources": [ 
    		"src/node_ph7.cc", 
    		"src/ph7_wrapper.cpp",
    		"src/ph7/ph7.c"
    	],
    	"defines": [ "NDEBUG" ],
    	"include_dirs": [ "src/ph7" ],
      "conditions": [
        ['OS=="win"', {
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': 1, # /EHsc
              'WarnAsError': 'true',
            },
          },
          'msvs_disabled_warnings': [
            4018,  # signed/unsigned mismatch
            4244,  # conversion from 'type1' to 'type2', possible loss of data
            4267,  # conversion from 'size_t' to 'type', possible loss of data
            4530,  # C++ exception handler used, but unwind semantics are not enabled
            4506,  # no definition for inline function
            4996,  # function was declared deprecated
          ],
          'defines': [
            '_WIN32_WINNT=0x0600',
          ],
        }]
      ]
	}]
}
