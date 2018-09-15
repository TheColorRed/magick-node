#include "imageMagick.h"
#include <node.h>

namespace NodeImageMagick {

  using v8::Local;
  using v8::Object;

  void InitImageMagick(Local<Object> exports, Local<Object> module) {
    MagickNode::Init(exports, module);
  }

  NODE_MODULE(NODE_GYP_MODULE_NAME, InitImageMagick)

} // namespace NodeImageMagick