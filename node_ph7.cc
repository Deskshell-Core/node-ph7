#include <node.h>
#include "ph7_wrapper.h"

using namespace v8;

void InitAll(Handle<Object> exports) {
	// init the TestObject...
  	ph7_wrapper::init(exports);
}

NODE_MODULE(ph7, InitAll)
