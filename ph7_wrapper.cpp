#include <iostream>
#include <node.h>
#include "ph7_wrapper.h"
extern "C" {
	// Including C header here.
	#include "ph7.h"
}

#define NJS_METHOD(obj, name, target) obj->Set(String::NewSymbol(name), target)

using namespace std;
using namespace v8;

// Deh structorz.
ph7_wrapper::ph7_wrapper() {
	// Save the REAL object we want.
}
ph7_wrapper::~ph7_wrapper() {
}
// Module structorz.
Handle<Value> ph7_wrapper::New(const Arguments& args) {
	HandleScope scope;
	
	ph7_wrapper *to = new ph7_wrapper;
	to->Wrap(args.This());
	
	return args.This();
}
void ph7_wrapper::init(Handle<Object> exports) {
	// Prepare constructor template
  	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  	tpl->SetClassName(String::NewSymbol("__construct"));
  	tpl->InstanceTemplate()->SetInternalFieldCount(1);

  	// Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "runString", ph7_wrapper::runString);
    NODE_SET_PROTOTYPE_METHOD(tpl, "runScript", ph7_wrapper::runScript);

  	Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
  	NJS_METHOD(exports, "ph7", constructor);
}

// The actual ph7 interaction methods.
Handle<Value> ph7_wrapper::runString(const Arguments& args) {
	HandleScope scope;
	
	// Typical ph7 procedure: alloc, config, exec, output, prepair, release & return.
	ph7 *engine; ph7_init(engine);
	ph7_vm *pVM; ph7_vm_config( pVM, PH7_VM_CONFIG_OUTPUT, ph7_wrapper::print );
		
	
	ph7_config(engine, PH7_CONFIG_ERR_OUTPUT, ph7_wrapper::print_error, NULL);
	ph7_compile_v2(
		engine,
		// args[0] as a string
		ph7_wrapper::print,
		-1, // Let the engine read untill the end of the string.
		pVM
	);
	
	return scope.Close(Boolean::New(true));	
}
Handle<Value> ph7_wrapper::runScript(const Arguments& args) {
	HandleScope scope;
	return scope.Close(Boolean::New(true));	
}


