#include <iostream>
#include <node.h>
#include "ph7_wrapper.h"
extern "C" {
	// Including C header here.
	#include "ph7.h"
}

#define NJS_METHOD(obj, name, target) obj->Set(String::NewSymbol(name), target)
#define NJS_UNWRAP(var, cname, args) cname * var = node::ObjectWrap::Unwrap<cname>(args.This())
#define NJS_UNWRAP2(var, cname, args) cname * var = node::ObjectWrap::Unwrap<cname>(args.Holder())

using namespace std;
using namespace v8;

// Deh structorz.
ph7_wrapper::ph7_wrapper() {
	ph7_init(&this->engine);
	this->include_path 		= Array::New();
	this->$_SERVER 			= Array::New();
	this->$_ENV 			= Array::New();
	this->$_HEADERS 		= Array::New();
	this->$_GET 			= Array::New();
	this->$_POST 			= Array::New();
	this->$_COOKIE 			= Array::New();
	this->$_SESSION 		= Array::New();
	this->$argv 			= Array::New();
	this->$_GLOBALS 		= Array::New();
	this->$_SUPER_GLOBALS 	= Array::New();
}
ph7_wrapper::~ph7_wrapper() {
	ph7_vm_release(this->pVM);
	ph7_release(this->engine);
/*	delete this->include_path;
	delete this->$_SERVER;
	delete this->$_ENV;
	delete this->$_HEADERS;
	delete this->$_GET;
	delete this->$_POST;
	delete this->$_COOKIE;
	delete this->$_SESSION;
	delete this->$argv;
	delete this->$_GLOBALS;
	delete this->$_SUPER_GLOBALS;
*/}
// Module structorz.
void ph7_wrapper::init(Handle<Object> exports) {
	// Prepare constructor template  	
  	NODE_SET_METHOD(exports, "create", ph7_wrapper::create);
}


// The actual ph7 interaction method. This spills out a Ph7VM object.
Handle<Value> ph7_wrapper::create(const Arguments& args) {
	HandleScope scope;

	// Impersonate a new...
	ph7_wrapper *$self = new ph7_wrapper;
	$self->Wrap(args.This());
	
	// Create template
	Local<ObjectTemplate> tpl = ObjectTemplate::New();
	tpl->SetInternalFieldCount(14);
	
	// Now, lets assign methods!
	NODE_SET_METHOD(tpl, "addSuper", 	ph7_wrapper::vm_addSuper);
	NODE_SET_METHOD(tpl, "addVar", 		ph7_wrapper::vm_addVar);
	NODE_SET_METHOD(tpl, "config", 		ph7_wrapper::vm_config);
	NODE_SET_METHOD(tpl, "compile", 		ph7_wrapper::vm_compile);
	NODE_SET_METHOD(tpl, "compileFile", 	ph7_wrapper::vm_compileFile);

	// Let's not forget the accessors.
	tpl->SetAccessor(String::New("$_SERVER"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_ENV"), 		 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_GET"), 		 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_POST"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_COOKIE"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_SESSION"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_HEADER"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$argv"), 		 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("include_path"),ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	
	// Now the uber convinience :3
	tpl->SetCallAsFunctionHandler(ph7_wrapper::vm_run);

	$self->Wrap(args.This());	
	return scope.Close(tpl->NewInstance());
}


// Now the interesting part: the VM functions.
Handle<Value> ph7_wrapper::vm_addSuper(const Arguments& args) {
	// Type checking
	if(!args[0]->IsString()) {
		return v8::ThrowException(v8::String::New("Wrong argument supplied: Argument 1 must be a string."));
	}
	if(!args[1]->IsString()) {
		return v8::ThrowException(v8::String::New("Wrong argument supplied: Argument 2 must be a string."));
	}
	
	// Get ourself
	NJS_UNWRAP($self, ph7_wrapper, args);
	
	// Prepair values - includes conversion.
	ph7_value          * pValue = ph7_new_scalar($self->pVM);
	String::AsciiValue * jsKey_ascii = new String::AsciiValue( args[0]->ToString() );
	String::AsciiValue * jsVal_ascii = new String::AsciiValue( args[1]->ToString() );
	const char         * jsKey = **jsKey_ascii;
	const char         * jsValue = **jsVal_ascii;
	int					 rc;
	HandleScope 		 scope;
	
	ph7_value_string(pValue, jsValue, -1);

	rc = ph7_vm_config(
		$self->pVM, PH7_VM_CONFIG_CREATE_SUPER, 
		jsKey, pValue
	);
	
	if(rc == PH7_OK) {
		return scope.Close(Boolean::New(true));
	} else {
		return scope.Close(Boolean::New(false));
	}
}
Handle<Value> ph7_wrapper::vm_addVar(const Arguments& args) {}
Handle<Value> ph7_wrapper::vm_config(const Arguments& args) {}
Handle<Value> ph7_wrapper::vm_compile(const Arguments& args) {}
Handle<Value> ph7_wrapper::vm_compileFile(const Arguments& args) {}
Handle<Value> ph7_wrapper::vm_run(const Arguments& args) {}

// The getter and setters
Handle<Value> ph7_wrapper::vm_getter(Local<String> property, const AccessorInfo& info) {
	//NJS_UNWRAP2($self, ph7_wrapper, info);
	ph7_wrapper* $self = node::ObjectWrap::Unwrap<ph7_wrapper>(info.Holder());
	HandleScope scope;
	Local<Value> undef;

	const char* cprop = **(new String::AsciiValue(property));
	cout << "--> " << cprop << endl;

	     if (strcmp(cprop, "include_path") == 0) { return scope.Close($self->include_path); }
	else if (strcmp(cprop, "$_SERVER") == 0) { return scope.Close($self->$_SERVER); }
	else if (strcmp(cprop, "$_ENV") == 0) { return scope.Close($self->$_ENV); }
	else if (strcmp(cprop, "$_HEADERS") == 0) { return scope.Close($self->$_HEADERS); }
	else if (strcmp(cprop, "$_GET") == 0) { return scope.Close($self->$_GET); }
	else if (strcmp(cprop, "$_POST") == 0) { return scope.Close($self->$_POST); }
	else if (strcmp(cprop, "$_COOKIE") == 0) { return scope.Close($self->$_COOKIE); }
	else if (strcmp(cprop, "$_SESSION") == 0) { return scope.Close($self->$_SESSION); }
	else if (strcmp(cprop, "$argv") == 0) { return scope.Close($self->$argv); }
	else if (strcmp(cprop, "$_GLOBALS") == 0) { return scope.Close($self->$_GLOBALS); }
	else if (strcmp(cprop, "$_SUPER_GLOBALS") == 0) { return scope.Close($self->$_SUPER_GLOBALS); }
	else return scope.Close(undef);
}
void ph7_wrapper::vm_setter(Local<String> property, Local<Value> value, const AccessorInfo& info) {}
