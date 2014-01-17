#include <node.h>
#include "ph7_wrapper.h"

using namespace v8;

void InitAll(Handle<Object> exports) {
	// init the TestObject...
  	ph7_wrapper::init(exports);
}

// Bit of logic to make this easier.
#ifdef GO_NATIVE
NODE_MODULE(node_ph7, InitAll)
#else
NODE_MODULE(ph7, InitAll)
#endif
