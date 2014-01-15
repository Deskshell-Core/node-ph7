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
	ph7_init(&this->engine);
}
ph7_wrapper::~ph7_wrapper() {
	ph7_vm_release(this->pVM);
	ph7_release(this->engine);
}
// Module structorz.
void ph7_wrapper::init(Handle<Object> exports) {
	// Prepare constructor template  	
  	NODE_SET_METHOD(exports, "create", ph7_wrapper::create);
}

// The actual ph7 interaction method. This spills out a Ph7VM object.
Handle<Value> ph7_wrapper::create(const Arguments& args) {
	HandleScope scope;

	// Impersonate a new...
	ph7_wrapper *to = new ph7_wrapper;
	to->Wrap(args.This());
	
	// Create template
	Local<ObjectTemplate> tpl = ObjectTemplate::New();
	tpl->SetInternalFieldCount(1);
	
	// Now, lets assign methods!
	NODE_SET_METHOD(tpl, "addSuper", 	ph7_wrapper::vm_addSuper);
	NODE_SET_METHOD(tpl, "addVar", 		ph7_wrapper::vm_addVar);
	NODE_SET_METHOD(tpl, "config", 		ph7_wrapper::vm_config);
	NODE_SET_METHOD(tpl, "compile", 		ph7_wrapper::vm_compile);
	NODE_SET_METHOD(tpl, "compileFile", 	ph7_wrapper::vm_compileFile);

	// Let's not forget the accessors.
	tpl->SetAccessor(String::New("$_SERVER"), 	ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_ENV"), 		ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_GET"), 		ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_POST"), 	ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_COOKIE"), 	ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_SESSION"), 	ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_HEADER"), 	ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$argv"), 		ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	
	// Now the uber convinience :3
	tpl->SetCallAsFunctionHandler(ph7_wrapper::vm_run);

	to->Wrap(args.This());	
	return scope.Close(tpl->NewInstance());
}
