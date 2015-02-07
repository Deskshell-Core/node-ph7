#include <iostream>
#include <string.h>
#include <node.h>
#include "ph7_wrapper.h"
extern "C" {
	// Including C header here.
	#include "ph7.h"
}

#define NJS_METHOD(obj, name, target) obj->Set(String::NewSymbol(name), target)
#define NJS_UNWRAP(var, cname, args) cname * var = node::ObjectWrap::Unwrap<cname>(args.Holder())
#define NJS_UNWRAP_ACC(var, cname, args, c) cname * var = (cname *) args.This()->GetPointerFromInternalField( c )
#define NODE_SET_PROPERTY(obj, name, val) obj->Set(String::NewSymbol(name), val)

using namespace std;
using namespace v8;

// Deh structorz.
ph7_wrapper::ph7_wrapper() : $_SERVER(Array::New(10)) {
	ph7_init(&this->engine);
	this->$SGLOBALS 		= Persistent<Array>::New( Array::New() );
	this->include_path 		= Persistent<Array>::New( Array::New() );
	this->$_SERVER 			= Persistent<Array>::New( Array::New() );
	this->$_ENV 			= Persistent<Array>::New( Array::New() );
	this->$_HEADER	 		= Persistent<Array>::New( Array::New() );
	this->$_GET 			= Persistent<Array>::New( Array::New() );
	this->$_POST 			= Persistent<Array>::New( Array::New() );
	this->$_COOKIE 			= Persistent<Array>::New( Array::New() );
	this->$_SESSION 		= Persistent<Array>::New( Array::New() );
	this->$argv 			= Persistent<Array>::New( Array::New() );
	this->$GLOBALS 			= Persistent<Array>::New( Array::New() );
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
  	
  	// Mapping the PH7_VM_* constants...
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_OUTPUT", Number::New(PH7_VM_CONFIG_OUTPUT));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_IMPORT_PATH", Number::New(PH7_VM_CONFIG_IMPORT_PATH));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_ERR_REPORT", Number::New(PH7_VM_CONFIG_ERR_REPORT));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_RECURSION_DEPTH", Number::New(PH7_VM_CONFIG_RECURSION_DEPTH));
  	NODE_SET_PROPERTY(exports, "PH7_VM_OUTPUT_LENGTH", Number::New(PH7_VM_OUTPUT_LENGTH));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_CREATE_SUPER", Number::New(PH7_VM_CONFIG_CREATE_SUPER));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_CREATE_VAR", Number::New(PH7_VM_CONFIG_CREATE_VAR));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_HTTP_REQUEST", Number::New(PH7_VM_CONFIG_HTTP_REQUEST));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_SERVER_ATTR", Number::New(PH7_VM_CONFIG_SERVER_ATTR));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_ENV_ATTR", Number::New(PH7_VM_CONFIG_ENV_ATTR));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_SESSION_ATTR", Number::New(PH7_VM_CONFIG_SESSION_ATTR));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_POST_ATTR", Number::New(PH7_VM_CONFIG_POST_ATTR));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_GET_ATTR", Number::New(PH7_VM_CONFIG_GET_ATTR));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_COOKIE_ATTR", Number::New(PH7_VM_CONFIG_COOKIE_ATTR));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_HEADER_ATTR", Number::New(PH7_VM_CONFIG_HEADER_ATTR));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_EXEC_VALUE", Number::New(PH7_VM_CONFIG_EXEC_VALUE));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_IO_STREAM", Number::New(PH7_VM_CONFIG_IO_STREAM));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_ARGV_ENTRY", Number::New(PH7_VM_CONFIG_ARGV_ENTRY));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_EXTRACT_OUTPUT", Number::New(PH7_VM_CONFIG_EXTRACT_OUTPUT));
  	NODE_SET_PROPERTY(exports, "PH7_VM_CONFIG_ERR_LOG_HANDLER", Number::New(PH7_VM_CONFIG_ERR_LOG_HANDLER));
  	
  	// Mapping the other flags
  	NODE_SET_PROPERTY(exports, "PH7_CONFIG_ERR_OUTPUT", Number::New(PH7_CONFIG_ERR_OUTPUT));
  	NODE_SET_PROPERTY(exports, "PH7_CONFIG_ERR_ABORT", Number::New(PH7_CONFIG_ERR_ABORT));
  	NODE_SET_PROPERTY(exports, "PH7_CONFIG_ERR_LOG", Number::New(PH7_CONFIG_ERR_LOG));

  	NODE_SET_PROPERTY(exports, "PH7_PHP_ONLY", Number::New(PH7_PHP_ONLY));
  	NODE_SET_PROPERTY(exports, "PH7_PHP_EXPR", Number::New(PH7_PHP_EXPR));

  	NODE_SET_PROPERTY(exports, "PH7_OK", Number::New(PH7_OK));
  	NODE_SET_PROPERTY(exports, "PH7_ABORT", Number::New(PH7_ABORT));
  	
  	NODE_SET_PROPERTY(exports, "PH7_VERSION", String::New(PH7_VERSION));
  	NODE_SET_PROPERTY(exports, "PH7_COPYRIGHT", String::New(PH7_COPYRIGHT));
}


// The actual ph7 interaction method. This spills out a Ph7VM object.
Handle<Value> ph7_wrapper::create(const Arguments& args) {
	HandleScope scope;
	
	// Create template
	Local<ObjectTemplate> tpl = ObjectTemplate::New();
	tpl->SetInternalFieldCount(1);
		
	// Let's not forget the accessors.
	tpl->SetAccessor(String::New("$_SERVER"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_ENV"), 		 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_GET"), 		 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_POST"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_COOKIE"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_SESSION"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$_HEADER"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$SGLOBALS"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$GLOBALS"), 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("$argv"), 	 	 ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);
	tpl->SetAccessor(String::New("include_path"),ph7_wrapper::vm_getter, ph7_wrapper::vm_setter);

	// Now, lets assign methods!
	NODE_SET_METHOD(tpl, "config", 			ph7_wrapper::vm_config);
	NODE_SET_METHOD(tpl, "prepair", 		ph7_wrapper::vm_prepair);
	NODE_SET_METHOD(tpl, "compile", 		ph7_wrapper::vm_compile);
	NODE_SET_METHOD(tpl, "compileFile", 	ph7_wrapper::vm_compileFile);
	
	// Now the uber convinience :3
	tpl->SetCallAsFunctionHandler(ph7_wrapper::vm_run);

	// Wrap the nativeness.
	Local<Object> obj = tpl->NewInstance();
	ph7_wrapper *$self = new ph7_wrapper();
	// Cast to void, store in object.
	obj->SetPointerInInternalField(0, (void*)$self);

	return scope.Close(obj);
}


// Now the interesting part: the VM functions.
Handle<Value> ph7_wrapper::vm_config(const Arguments& args) {
	// Get ourself
	NJS_UNWRAP($self, ph7_wrapper, args);
	int			 rc;
	int			 flag;
	HandleScope  scope;

	// Type checking
	if(!args[0]->IsNumber()) {
		return v8::ThrowException(v8::String::New("Argument 1 must be a ph7 constant."));
	} else {
		flag = args[0]->ToInteger()->Value();
	}
	
	switch(flag) {
		// The working ones
		case PH7_VM_CONFIG_IMPORT_PATH:
			if(args[1]->IsString()) {
				const char* path = **(new String::AsciiValue( args[1]->ToString() ));
				rc = ph7_vm_config($self->pVM, flag, path);
			} else {
				return v8::ThrowException(v8::String::New("Argument 2 must be a string."));
			}
		break;
		case PH7_VM_CONFIG_ERR_REPORT:
			rc = ph7_vm_config($self->pVM, flag);
		break;
		case PH7_VM_CONFIG_RECURSION_DEPTH:
			if(args[1]->IsInt32()) {
				int arg = (int)args[1]->ToInteger()->Value();
				rc = ph7_vm_config($self->pVM, flag, arg);
			} else {
				return v8::ThrowException(v8::String::New("Argument 2 must be a number."));
			}
		break;
		case PH7_VM_CONFIG_CREATE_SUPER:
		case PH7_VM_CONFIG_CREATE_VAR:
			// These two share the exact same logic. :)
			if(args[1]->IsString()) {
				const char* name = **(new String::AsciiValue( args[1]->ToString() ));
				ph7_value *pv = ph7_new_scalar($self->pVM);
				Local<Value> arg = args[1];
				// Convert to ph7_value
				if(arg->IsInt32()) { ph7_value_int(pv, arg->ToInteger()->Value()); }
				else if(arg->IsNumber()) { ph7_value_double(pv, arg->ToNumber()->Value()); }
				else if(arg->IsBoolean()) { ph7_value_bool(pv, (arg->ToBoolean()->Value() == true ? 1:0)); }
				else if(arg->IsNull()) { ph7_value_null(pv); }
				else if(arg->IsString()) { ph7_value_string(pv, **(new String::AsciiValue(arg->ToString()) ), arg->ToString()->Length()); }

				rc = ph7_vm_config($self->pVM, flag, name, pv);
			} else {
				return v8::ThrowException(v8::String::New("Argument 2 must be a string."));
			}
		break;
		case PH7_VM_CONFIG_SERVER_ATTR:
		case PH7_VM_CONFIG_ENV_ATTR:
		case PH7_VM_CONFIG_SESSION_ATTR:
		case PH7_VM_CONFIG_POST_ATTR:
		case PH7_VM_CONFIG_GET_ATTR:
		case PH7_VM_CONFIG_COOKIE_ATTR:
		case PH7_VM_CONFIG_HEADER_ATTR:
			// These also share the same logic!
			if(args[1]->IsString() && args[2]->IsString()) {
				const char* name = **( new String::AsciiValue(args[1]->ToString()) );
				const char* value = **( new String::AsciiValue(args[2]->ToString()) );
				int len = args[2]->ToString()->Length();
				rc = ph7_vm_config($self->pVM, flag, name, value, len);
			} else {
				return v8::ThrowException(v8::String::New("Argument 2 and 3 must be strings."));
			}
		break;
	
		// Not implemented yet... So they're at the bottom, sharing the same message.
		case PH7_VM_CONFIG_OUTPUT:
		case PH7_VM_OUTPUT_LENGTH:
		case PH7_VM_CONFIG_HTTP_REQUEST:
		case PH7_VM_CONFIG_EXEC_VALUE: // Returns the scripts exit code - IMPORTANT.
			return v8::ThrowException(v8::String::New("The selected flag has not been implemented yet."));
		default:
			return v8::ThrowException(v8::String::New("Unknown flag."));
	}
		
	if(rc == PH7_OK) {
		return scope.Close(True());
	} else {
		return scope.Close(False());
	}
}
Handle<Value> ph7_wrapper::vm_compileFile(const Arguments& args) {
	// ReadFile
	NJS_UNWRAP_ACC($self, ph7_wrapper, args, 0);
	HandleScope scope;
	int rc;
	FILE *f;

	if(args[0]->IsString()) {
		const char* fileName = **(new String::AsciiValue(args[0]->ToString()));
		f = fopen(fileName,"rb");
		if(f) {
			// File exists.
			fclose(f);
			rc = ph7_compile_file($self->engine, fileName, &$self->pVM, 0);
			if(rc == PH7_OK) {
				return scope.Close(True());
			} else {
				return scope.Close(Integer::New(rc));
			}
		} else {
			return scope.Close(v8::ThrowException(String::New("Specified file was not found!")));
		}
	} else {
		return scope.Close(v8::ThrowException(String::New("Argument 1 must be a string.")));
	}
}
Handle<Value> ph7_wrapper::vm_compile(const Arguments& args) {
	NJS_UNWRAP_ACC($self, ph7_wrapper, args, 0);
	HandleScope scope;
	int rc;

	if(args[0]->IsString()) {
		const char* code = **(new String::AsciiValue(args[0]->ToString()));
		rc = ph7_compile_v2($self->engine, code, args[0]->ToString()->Length(), &$self->pVM, 0);
		if(rc == PH7_OK) {
			return scope.Close(True());
		} else {
			return scope.Close(Integer::New(rc));
		}
	} else {
		return scope.Close(v8::ThrowException(String::New("Argument 1 must be a string.")));
	}
}
Handle<Value> ph7_wrapper::vm_prepair(const Arguments& args) {
	// We are going to save everything down tot he dust. And in fact, are skipping the above ->config method - because we'Re shortcutting here.
	NJS_UNWRAP_ACC($self, ph7_wrapper, args, 0);
	HandleScope scope;
	Persistent<Array> obj;
	Local<Array> props;
	uint32_t length;
	const char* key;
	const char* value;
	
	// $argv goes first
	obj = $self->$argv;
	props = obj->GetOwnPropertyNames();
	length = props->Length();
	for(uint32_t i=0; i<length; ++i) {
		value = **(new String::AsciiValue( 
			obj->Get( props->Get(i) )->ToString() 
		));
		ph7_vm_config($self->pVM, PH7_VM_CONFIG_ARGV_ENTRY, value);
	}

	// inclue path...
	obj = $self->include_path;
	props = obj->GetOwnPropertyNames();
	length = props->Length();
	for(uint32_t i=0; i<length; ++i) {
		value = **(new String::AsciiValue( 
			obj->Get( props->Get(i) )->ToString() 
		));
		ph7_vm_config($self->pVM, PH7_VM_CONFIG_IMPORT_PATH, value);
	}

	// $_SERVER
	obj = $self->$_SERVER;
	props = obj->GetOwnPropertyNames();
	length = props->Length();
	for(uint32_t i=0; i<length; ++i) {
		key = **(new String::AsciiValue(  props->Get(i)->ToString() ));
		value = **(new String::AsciiValue( 
			obj->Get( props->Get(i) )->ToString() 
		));
		ph7_vm_config($self->pVM, PH7_VM_CONFIG_SERVER_ATTR, key, value, obj->Get( props->Get(i) )->ToString()->Length());
	}
	
	// $_ENV
	obj = $self->$_ENV;
	props = obj->GetOwnPropertyNames();
	length = props->Length();
	for(uint32_t i=0; i<length; ++i) {
		key = **(new String::AsciiValue(  props->Get(i)->ToString() ));
		value = **(new String::AsciiValue( 
			obj->Get( props->Get(i) )->ToString() 
		));
		ph7_vm_config($self->pVM, PH7_VM_CONFIG_ENV_ATTR, key, value, obj->Get( props->Get(i) )->ToString()->Length());
	}
	
	// $_SESSION
	obj = $self->$_SESSION;
	props = obj->GetOwnPropertyNames();
	length = props->Length();
	for(uint32_t i=0; i<length; ++i) {
		key = **(new String::AsciiValue(  props->Get(i)->ToString() ));
		value = **(new String::AsciiValue( 
			obj->Get( props->Get(i) )->ToString() 
		));
		ph7_vm_config($self->pVM, PH7_VM_CONFIG_SESSION_ATTR, key, value, obj->Get( props->Get(i) )->ToString()->Length());
	}
	
	// $_POST
	obj = $self->$_POST;
	props = obj->GetOwnPropertyNames();
	length = props->Length();
	for(uint32_t i=0; i<length; ++i) {
		key = **(new String::AsciiValue(  props->Get(i)->ToString() ));
		value = **(new String::AsciiValue( 
			obj->Get( props->Get(i) )->ToString() 
		));
		ph7_vm_config($self->pVM, PH7_VM_CONFIG_POST_ATTR, key, value, obj->Get( props->Get(i) )->ToString()->Length());
	}

	// $_GET
	obj = $self->$_GET;
	props = obj->GetOwnPropertyNames();
	length = props->Length();
	for(uint32_t i=0; i<length; ++i) {
		key = **(new String::AsciiValue(  props->Get(i)->ToString() ));
		value = **(new String::AsciiValue( 
			obj->Get( props->Get(i) )->ToString() 
		));
		ph7_vm_config($self->pVM, PH7_VM_CONFIG_GET_ATTR, key, value, obj->Get( props->Get(i) )->ToString()->Length());
	}
	
	// $_COOKIE
	obj = $self->$_COOKIE;
	props = obj->GetOwnPropertyNames();
	length = props->Length();
	for(uint32_t i=0; i<length; ++i) {
		key = **(new String::AsciiValue(  props->Get(i)->ToString() ));
		value = **(new String::AsciiValue( 
			obj->Get( props->Get(i) )->ToString() 
		));
		ph7_vm_config($self->pVM, PH7_VM_CONFIG_COOKIE_ATTR, key, value, obj->Get( props->Get(i) )->ToString()->Length());
	}
	
	// $_HEADER
	obj = $self->$_HEADER;
	props = obj->GetOwnPropertyNames();
	length = props->Length();
	for(uint32_t i=0; i<length; ++i) {
		key = **(new String::AsciiValue(  props->Get(i)->ToString() ));
		value = **(new String::AsciiValue( 
			obj->Get( props->Get(i) )->ToString() 
		));
		ph7_vm_config($self->pVM, PH7_VM_CONFIG_HEADER_ATTR, key, value, obj->Get( props->Get(i) )->ToString()->Length());
	}
	
	// SUPER GLOBALZ!
	obj = $self->$SGLOBALS;
	props = obj->GetOwnPropertyNames();
	length = props->Length();
	for(uint32_t i=0; i<length; ++i) {
		key = **(new String::AsciiValue(  props->Get(i)->ToString() ));
		Local<Value> jsVal = obj->Get( props->Get(i) );
		// This is a biiiiit more complicated...
		ph7_value *pv;
		if(jsVal->IsArray() || jsVal->IsObject()) {
			// What we're doing here is awful weird o-o
			// Create a PHP array, and iterate over the js array, convert types and insert.
			// Untill I can do recrusive functions, one level needs to do it for now.
			// For now: pVM.$_SERVER[key]=jsVal:{jsakey:jsaval, key:val}
			pv = ph7_new_array($self->pVM);
			Local<Array> jsProps = jsVal->ToObject()->GetOwnPropertyNames();
			uint32_t len = jsProps->Length();
			for(uint32_t c=0; c<len; ++c) {
				Local<Value> jsaKey = jsProps->Get(c);
				Local<Value> jsaVal = jsVal->ToObject()->Get(jsaKey);
				ph7_value *a_pv = ph7_new_scalar($self->pVM);
				const char* jsaKey_c = **(new String::AsciiValue( jsaKey->ToString() ));
				
				// Convert js->php
				if(jsaVal->IsInt32()) { ph7_value_int(a_pv, jsaVal->ToInteger()->Value()); }
				else if(jsaVal->IsNumber()) { ph7_value_double(a_pv, jsaVal->ToNumber()->Value()); }
				else if(jsaVal->IsBoolean()) { ph7_value_bool(a_pv, (jsaVal->ToBoolean()->Value() == true ? 1:0)); }
				else if(jsaVal->IsNull()) { ph7_value_null(a_pv); }
				else if(jsaVal->IsString()) { ph7_value_string(a_pv, **(new String::AsciiValue(jsaVal->ToString()) ), jsaVal->ToString()->Length()); }
					
				ph7_array_add_strkey_elem(pv, jsaKey_c, a_pv);
			}
		} else /*if number, string, double, ...*/ {
			pv = ph7_new_scalar($self->pVM);
			// Type conversion. Isnt there an easier way, like an auto-caster? @_@
			if(jsVal->IsInt32()) { ph7_value_int(pv, jsVal->ToInteger()->Value()); }
			else if(jsVal->IsNumber()) { ph7_value_double(pv, jsVal->ToNumber()->Value()); }
			else if(jsVal->IsBoolean()) { ph7_value_bool(pv, (jsVal->ToBoolean()->Value() == true ? 1:0)); }
			else if(jsVal->IsNull()) { ph7_value_null(pv); }
			else if(jsVal->IsString()) { ph7_value_string(pv, **(new String::AsciiValue(jsVal->ToString()) ), jsVal->ToString()->Length()); }
		}

		ph7_vm_config(
			$self->pVM, PH7_VM_CONFIG_CREATE_SUPER, 
			key, pv
		);
	}
	
	// ROUND TEWO...normal globals
	obj = $self->$GLOBALS;
	props = obj->GetOwnPropertyNames();
	length = props->Length();
	for(uint32_t i=0; i<length; ++i) {
		key = **(new String::AsciiValue(  props->Get(i)->ToString() ));
		Local<Value> jsVal = obj->Get( props->Get(i) );
		// This is a biiiiit more complicated...
		ph7_value *pv;
		if(jsVal->IsArray() || jsVal->IsObject()) {
			// What we're doing here is awful weird o-o
			// Create a PHP array, and iterate over the js array, convert types and insert.
			// Untill I can do recrusive functions, one level needs to do it for now.
			// For now: pVM.$_SERVER[key]=jsVal:{jsakey:jsaval, key:val}
			pv = ph7_new_array($self->pVM);
			Local<Array> jsProps = jsVal->ToObject()->GetOwnPropertyNames();
			uint32_t len = jsProps->Length();
			for(uint32_t c=0; c<len; ++c) {
				Local<Value> jsaKey = jsProps->Get(c);
				Local<Value> jsaVal = jsVal->ToObject()->Get(jsaKey);
				ph7_value *a_pv = ph7_new_scalar($self->pVM);
				const char* jsaKey_c = **(new String::AsciiValue( jsaKey->ToString() ));
				
				// Convert js->php
				if(jsaVal->IsInt32()) { ph7_value_int(a_pv, jsaVal->ToInteger()->Value()); }
				else if(jsaVal->IsNumber()) { ph7_value_double(a_pv, jsaVal->ToNumber()->Value()); }
				else if(jsaVal->IsBoolean()) { ph7_value_bool(a_pv, (jsaVal->ToBoolean()->Value() == true ? 1:0)); }
				else if(jsaVal->IsNull()) { ph7_value_null(a_pv); }
				else if(jsaVal->IsString()) { ph7_value_string(a_pv, **(new String::AsciiValue(jsaVal->ToString()) ), jsaVal->ToString()->Length()); }
					
				ph7_array_add_strkey_elem(pv, jsaKey_c, a_pv);
			}
		} else /*if number, string, double, ...*/ {
			pv = ph7_new_scalar($self->pVM);
			// Type conversion. Isnt there an easier way, like an auto-caster? @_@
			if(jsVal->IsInt32()) { ph7_value_int(pv, jsVal->ToInteger()->Value()); }
			else if(jsVal->IsNumber()) { ph7_value_double(pv, jsVal->ToNumber()->Value()); }
			else if(jsVal->IsBoolean()) { ph7_value_bool(pv, (jsVal->ToBoolean()->Value() == true ? 1:0)); }
			else if(jsVal->IsNull()) { ph7_value_null(pv); }
			else if(jsVal->IsString()) { ph7_value_string(pv, **(new String::AsciiValue(jsVal->ToString()) ), jsVal->ToString()->Length()); }
		}

		ph7_vm_config(
			$self->pVM, PH7_VM_CONFIG_CREATE_SUPER, 
			key, pv
		);
	}
	
	return scope.Close(Null());
}
Handle<Value> ph7_wrapper::vm_run(const Arguments& args) {
	NJS_UNWRAP_ACC($self, ph7_wrapper, args, 0);
	HandleScope scope;
	int rc;
	int exitStatus;
	
	// PH7_VM_CONFIG_EXEC_VALUE can get the whole...but were not gonna do crazy typing from php->js, yet.
	
	// Tell the engine to use ph7_wrapper::print.
	ph7_vm_config($self->pVM, PH7_VM_CONFIG_OUTPUT, ph7_wrapper::print);
	rc = ph7_vm_exec($self->pVM, &exitStatus);
	
	if(rc == PH7_OK){
		return scope.Close(Integer::New(exitStatus));
	} else {
		return scope.Close(False());
	}
}

// The getter and setters
Handle<Value> ph7_wrapper::vm_getter(Local<String> property, const AccessorInfo& info) {
	NJS_UNWRAP_ACC($self, ph7_wrapper, info, 0);
	HandleScope scope;

	const char* cprop = **(new String::AsciiValue(property));
	// cout << "Getting: " << cprop << endl;

	     if (strcmp(cprop, "include_path") == 0) { return scope.Close($self->include_path); }
	else if (strcmp(cprop, "$_SERVER") == 0) { return scope.Close($self->$_SERVER); }
	else if (strcmp(cprop, "$_ENV") == 0) { return scope.Close($self->$_ENV); }
	else if (strcmp(cprop, "$_HEADER") == 0) { return scope.Close($self->$_HEADER); }
	else if (strcmp(cprop, "$_GET") == 0) { return scope.Close($self->$_GET); }
	else if (strcmp(cprop, "$_POST") == 0) { return scope.Close($self->$_POST); }
	else if (strcmp(cprop, "$_COOKIE") == 0) { return scope.Close($self->$_COOKIE); }
	else if (strcmp(cprop, "$_SESSION") == 0) { return scope.Close($self->$_SESSION); }
	else if (strcmp(cprop, "$argv") == 0) { return scope.Close($self->$argv); }
	else if (strcmp(cprop, "$GLOBALS") == 0) { return scope.Close($self->$GLOBALS); }
	else if (strcmp(cprop, "$SGLOBALS") == 0) { return scope.Close($self->$SGLOBALS); }
	else    return scope.Close(Undefined());
}
void ph7_wrapper::vm_setter(Local<String> property, Local<Value> value, const AccessorInfo& info) {
	const char* cprop = **(new String::AsciiValue(property));
	cout << "--> Setting: " << cprop << endl;
}

int ph7_wrapper::print(const void *msg, unsigned int len, void *userData) {
	ssize_t rt;
   	rt = write(STDOUT_FILENO, msg, len);
   	if( rt < 0 ){
    	/* Abort processing */
     	return PH7_ABORT;
    }
    /* All done,VM output was redirected to STDOUT */
    return PH7_OK;
}

// Other functions.
// Taken from v8_shell.cc
/*Handle<String> ReadFile(const char* name);
Handle<String> ReadFile(const char* name) {
  	FILE* file = fopen(name, "rb");
  	if (file == NULL) return Handle<String>();
	
	// Determine filesize
  	fseek(file, 0, SEEK_END);
  	int size = ftell(file);
  	rewind(file);
	
	// Store file in char pointer, of the size of the file. Also append a null-byte
  	char* chars = new char[size + 1];
  	chars[size] = '\0';
  	for (int i = 0; i < size;) {
    	int read = fread(&chars[i], 1, size - i, file);
    	i += read;
  	}
  	fclose(file);
  	v8::Handle<v8::String> result = v8::String::New(chars, size);
  	delete[] chars;
	return result;
}*/
