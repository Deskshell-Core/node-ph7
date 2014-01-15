#ifndef HAVE_PH7W_H
#define HAVE_PH7W_H

#include <node.h>
extern "C" {
	#include "ph7.h"
}

class ph7_wrapper : public node::ObjectWrap {
	public:
		// *structorz...
		ph7_wrapper();
		~ph7_wrapper();
		// NodeJS specific
		static void init(v8::Handle<v8::Object> exports);
		static v8::Handle<v8::Value> New(const v8::Arguments& args);	
		
		// One of the many proxy methods. They're ment to convert return values and act as forwarders.
		static v8::Handle<v8::Value> runString(const v8::Arguments& args);
		static v8::Handle<v8::Value> runScript(const v8::Arguments& args);
		
		// Helper
		static void print(const void *msg, unsigned int len, void *userData);
};

#endif
