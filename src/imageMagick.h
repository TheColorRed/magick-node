#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <Magick++.h>
#include <node.h>
#include <node_object_wrap.h>
#include <uv.h>

namespace NodeImageMagick {

  using Magick::Blob;
  using Magick::Image;
  using v8::AccessorGetterCallback;
  using v8::Context;
  using v8::CopyablePersistentTraits;
  using v8::Function;
  using v8::FunctionCallbackInfo;
  using v8::Isolate;
  using v8::Local;
  using v8::MaybeLocal;
  using v8::Object;
  using v8::Persistent;
  using v8::Promise;
  using v8::PropertyCallbackInfo;
  using v8::String;
  using v8::Value;
  using namespace std;

  struct CallbackInfo;

  class MagickNode : public node::ObjectWrap {
  public:
    static void Init(Local<Object> exports, Local<Object> module);

  private:
    explicit MagickNode(const std::string &path);
    explicit MagickNode(const Blob &blob);
    ~MagickNode();

    static void New(const FunctionCallbackInfo<Value> &args);
    static CallbackInfo Info(const FunctionCallbackInfo<Value> &args);
    static CallbackInfo Info(const PropertyCallbackInfo<Value> &args);
    // Data method
    static void Width(Local<String> property, const PropertyCallbackInfo<Value> &args);
    static void Height(Local<String> property, const PropertyCallbackInfo<Value> &args);
    static void BaseWidth(Local<String> property, const PropertyCallbackInfo<Value> &args);
    static void BaseHeight(Local<String> property, const PropertyCallbackInfo<Value> &args);
    // Manipulation methods
    static void Scale(const FunctionCallbackInfo<Value> &args);
    static void _Scale(double width, double height, CallbackInfo);
    static void ScaleWidth(const FunctionCallbackInfo<Value> &args);
    static void ScaleHeight(const FunctionCallbackInfo<Value> &args);
    static void Crop(const FunctionCallbackInfo<Value> &args);
    static void Rotate(const FunctionCallbackInfo<Value> &args);
    static void Flip(const FunctionCallbackInfo<Value> &args);
    // Saving methods
    static void Save(const FunctionCallbackInfo<Value> &args);
    static void GetBuffer(const FunctionCallbackInfo<Value> &args);
    // Workers Complete
    static void WorkBlobComplete(uv_work_t *req, int status);
    static void WorkVoidComplete(uv_work_t *req, int status);
    static Persistent<Function> constructor;
    Magick::Image image;
  };

  void Log(char *message);
  void Log(string message);

  struct CallbackInfo {
    Isolate *isolate;
    MagickNode *obj;
    Local<Context> context;
    Local<Value> args[1];
  };

  struct Work {
    uv_work_t requst;
    CallbackInfo info;
    Local<Value> args[10];
    Persistent<Promise::Resolver, CopyablePersistentTraits<Promise::Resolver>> resolver;
    Blob result;
  };

} // namespace NodeImageMagick

#endif