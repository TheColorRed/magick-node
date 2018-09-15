#include "imageMagick.h"
#include <node_buffer.h>
#include <node_object_wrap.h>
#include <string>
#include <sys/stat.h>

namespace NodeImageMagick {

  using Magick::Blob;
  using Magick::Geometry;
  using node::ObjectWrap;
  using v8::AccessorGetterCallback;
  using v8::Context;
  using v8::CopyablePersistentTraits;
  using v8::Exception;
  using v8::Function;
  using v8::FunctionCallbackInfo;
  using v8::FunctionTemplate;
  using v8::HandleScope;
  using v8::Integer;
  using v8::Isolate;
  using v8::Local;
  using v8::MaybeLocal;
  using v8::Name;
  using v8::Number;
  using v8::Object;
  using v8::ObjectTemplate;
  using v8::Persistent;
  using v8::Promise;
  using v8::PropertyCallbackInfo;
  using v8::String;
  using v8::Template;
  using v8::Undefined;
  using v8::Value;
  using namespace std;

  Persistent<Function> MagickNode::constructor;

  MagickNode::MagickNode(const string &path) {
    // TODO: Replace the empty string with the path to the library
    Magick::InitializeMagick("");
    image.read(path);
  }

  MagickNode::MagickNode(const Blob &blob) {
    // TODO: Replace the empty string with the path to the library
    Magick::InitializeMagick("");
    image.read(blob);
  }

  MagickNode::~MagickNode() {}

  void MagickNode::Init(Local<Object> exports, Local<Object> module) {
    Isolate *isolate = exports->GetIsolate();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "MagickNode"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // getters
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "width"), Width);
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "height"), Height);
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "baseWidth"), BaseWidth);
    tpl->InstanceTemplate()->SetAccessor(String::NewFromUtf8(isolate, "baseHeight"), BaseHeight);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "scale", Scale);
    NODE_SET_PROTOTYPE_METHOD(tpl, "scaleByWidth", ScaleWidth);
    NODE_SET_PROTOTYPE_METHOD(tpl, "scaleByHeight", ScaleHeight);
    NODE_SET_PROTOTYPE_METHOD(tpl, "crop", Crop);
    NODE_SET_PROTOTYPE_METHOD(tpl, "rotate", Rotate);
    NODE_SET_PROTOTYPE_METHOD(tpl, "flip", Flip);

    // Methods for saving
    NODE_SET_PROTOTYPE_METHOD(tpl, "save", Save);
    NODE_SET_PROTOTYPE_METHOD(tpl, "toBuffer", GetBuffer);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "MagickNode"), tpl->GetFunction());
  }

  void MagickNode::New(const FunctionCallbackInfo<Value> &args) {
    Isolate *isolate       = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    if (args.IsConstructCall()) {
      // Invoked as constructor: `new MagickNode(...)`
      String::Utf8Value param1(args[0]->ToString());
      const char *str = string(*param1).c_str();

      // Test if the file exists
      struct stat buffer;
      bool exists = (stat(str, &buffer) == 0);
      if (!exists) {
        const char *data = ("File does not exist [" + string(str) + "].").c_str();
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, data)));
        return;
      }

      // Create new instance of the object
      MagickNode *obj = new MagickNode(string(*param1));
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    }
  }

  void MagickNode::Width(Local<String> property, const PropertyCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);
    double width   = static_cast<double>(i.obj->image.columns());
    args.GetReturnValue().Set(Number::New(i.isolate, width));
  }

  void MagickNode::Height(Local<String> property, const PropertyCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);
    double height  = static_cast<double>(i.obj->image.rows());
    args.GetReturnValue().Set(Number::New(i.isolate, height));
  }

  void MagickNode::BaseWidth(Local<String> property, const PropertyCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);
    double width   = static_cast<double>(i.obj->image.baseColumns());
    args.GetReturnValue().Set(Number::New(i.isolate, width));
  }

  void MagickNode::BaseHeight(Local<String> property, const PropertyCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);
    double height  = static_cast<double>(i.obj->image.baseRows());
    args.GetReturnValue().Set(Number::New(i.isolate, height));
  }

  void MagickNode::ScaleWidth(const FunctionCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);
    if (args.Length() > 2) {
      i.isolate->ThrowException(Exception::Error(String::NewFromUtf8(i.isolate, "scaleWidth requires two arguments.")));
      return;
    }
    Work *work        = new Work();
    work->requst.data = work;
    work->info        = i;
    work->args[0]     = Number::New(i.isolate, args[0]->NumberValue(i.context).FromMaybe(0));
    // work->callback.Reset(i.isolate, Local<Function>::Cast(args[1]));
    uv_queue_work(uv_default_loop(), &work->requst, [](uv_work_t *req) {
      Work *work   = static_cast<Work *>(req->data);
      double width = work->args[0]->NumberValue(work->info.context).FromMaybe(0);
      MagickNode::_Scale(width, 0, work->info);
    },
        WorkVoidComplete);
    args.GetReturnValue().Set(args.This());
  }

  void MagickNode::ScaleHeight(const FunctionCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);
    if (args.Length() != 1) {
      i.isolate->ThrowException(Exception::Error(String::NewFromUtf8(i.isolate, "scaleHeight requires one argument.")));
      return;
    }
    Work *work        = new Work();
    work->requst.data = work;
    work->info        = i;
    work->args[0]     = Number::New(i.isolate, args[0]->NumberValue(i.context).FromMaybe(0));
    // work->callback.Reset(i.isolate, Local<Function>::Cast(args[1]));
    uv_queue_work(uv_default_loop(), &work->requst, [](uv_work_t *req) {
      Work *work    = static_cast<Work *>(req->data);
      double height = work->args[0]->NumberValue(work->info.context).FromMaybe(0);
      MagickNode::_Scale(0, height, work->info);
    },
        WorkVoidComplete);
    // double height = args[0]->NumberValue(i.context).FromMaybe(0);
    // MagickNode::_Scale(0, height, i);
    args.GetReturnValue().Set(args.This());
  }

  void MagickNode::Scale(const FunctionCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);

    // Require one or two arguments
    if (args.Length() != 2) {
      i.isolate->ThrowException(Exception::Error(String::NewFromUtf8(i.isolate, "scale requires two arguments.")));
      return;
    }

    double width  = args[0]->NumberValue(i.context).FromMaybe(0);
    double height = args[1]->NumberValue(i.context).FromMaybe(0);

    MagickNode::_Scale(width, height, i);

    args.GetReturnValue().Set(args.This());
  }

  void MagickNode::_Scale(double width, double height, CallbackInfo i) {
    if (width > 0 && height > 0) {
      i.obj->image.resize(Geometry(to_string(width) + "x" + to_string(height) + "!"));
    }
    // Resize image with proportions
    else {
      i.obj->image.resize(Geometry(static_cast<size_t>(width), static_cast<size_t>(height)));
    }
  }

  typedef Persistent<Promise::Resolver, CopyablePersistentTraits<Promise::Resolver>> PersistentResolver;

  void MagickNode::Crop(const FunctionCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);

    // Require between two and four arguments
    if (args.Length() < 2 || args.Length() > 4) {
      i.isolate->ThrowException(Exception::Error(String::NewFromUtf8(i.isolate, "crop requires between two and four parameters.")));
      return;
    }

    Local<Promise::Resolver> resolver = Promise::Resolver::New(i.isolate);
    auto presolver                    = PersistentResolver(i.isolate, resolver);
    // Persistent<Promise::Resolver> presolver = Persistent<Promise::Resolver, v8::CopyablePersistentTraits<Promise::Resolver>>::New(i.isolate, resolver);
    // presolver.Reset(i.isolate, resolver);
    // Persistent<Local<Promise::Resolver>> p;

    // ::Cast(Promise::Resolver::New(i.isolate));
    args.GetReturnValue().Set(resolver->GetPromise());

    // Set the default offsets
    Local<Value> x = Integer::New(i.isolate, 0);
    Local<Value> y = Integer::New(i.isolate, 0);

    // Set the size of the image
    Local<Value> width  = Integer::New(i.isolate, args[0]->NumberValue(i.context).FromMaybe(0));
    Local<Value> height = Integer::New(i.isolate, args[1]->NumberValue(i.context).FromMaybe(0));

    // Update the offsets if needed
    if (args.Length() >= 3) x = Integer::New(i.isolate, args[2]->NumberValue(i.context).FromMaybe(0));
    if (args.Length() >= 4) y = Integer::New(i.isolate, args[3]->NumberValue(i.context).FromMaybe(0));

    // Create the worker
    Work *work        = new Work();
    work->requst.data = work;
    work->info        = i;
    work->resolver    = presolver;
    work->args[0]     = width;
    work->args[1]     = height;
    work->args[2]     = x;
    work->args[3]     = y;

    // Start the worker
    uv_queue_work(uv_default_loop(), &work->requst, [](uv_work_t *req) {
      Work *work    = static_cast<Work *>(req->data);
      size_t width  = static_cast<size_t>(work->args[0]->NumberValue(work->info.context).FromMaybe(0));
      size_t height = static_cast<size_t>(work->args[1]->NumberValue(work->info.context).FromMaybe(0));
      size_t x      = static_cast<size_t>(work->args[2]->NumberValue(work->info.context).FromMaybe(0));
      size_t y      = static_cast<size_t>(work->args[3]->NumberValue(work->info.context).FromMaybe(0));
      work->info.obj->image.crop(Geometry(width, height, x, y));
    },
        WorkVoidComplete);
  }

  void MagickNode::Rotate(const FunctionCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);

    // Require one argument
    if (args.Length() < 1) {
      i.isolate->ThrowException(Exception::Error(String::NewFromUtf8(i.isolate, "rotate requires one argument.")));
      return;
    }

    i.obj->image.rotate(args[0]->NumberValue(i.context).FromMaybe(0));

    args.GetReturnValue().Set(args.This());
  }

  void MagickNode::Flip(const FunctionCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);

    // Require one argument
    if (args.Length() < 1) {
      i.isolate->ThrowException(Exception::Error(String::NewFromUtf8(i.isolate, "flip requires one argument.")));
      return;
    }
    String::Utf8Value dir(args[0]->ToString());
    string direction = string(*dir);

    if (direction != string("vertical") && direction != string("horizontal")) {
      i.isolate->ThrowException(Exception::Error(String::NewFromUtf8(i.isolate, "flip Argument must be of 'horizontal' or 'vertical'.")));
      return;
    }

    // Flip the image horizontally
    i.obj->image.flip();
    if (direction == string("vertical")) {
      i.obj->image.rotate(180);
    }

    args.GetReturnValue().Set(args.This());
  }

  void MagickNode::GetBuffer(const FunctionCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);
    string format  = "png";

    // If their is one or more arguments set the new format
    if (args.Length() >= 1 && args[0]->IsString()) {
      String::Utf8Value fmt(args[0]->ToString());
      format = string(*fmt);
    }

    // If there is at least two arguments set the quality
    if (args.Length() == 2 && args[1]->IsNumber()) {
      double quality = args[1]->NumberValue(i.context).FromMaybe(0);
      i.obj->image.quality(static_cast<size_t>(quality));
    }

    Local<Function> cb;
    const unsigned argc = 1;

    // if (args.Length() == 1 && args[0]->IsFunction()) {
    //   cb = Local<Function>::Cast(args[0]);
    // } else if (args.Length() == 2 && args[1]->IsFunction()) {
    //   cb = Local<Function>::Cast(args[1]);
    // } else if (args.Length() == 3 && args[2]->IsFunction()) {
    //   cb = Local<Function>::Cast(args[2]);
    // }

    // Set the image output format
    i.obj->image.magick(format);
    // Write to the format

    Work *work        = new Work();
    work->requst.data = work;
    work->info        = i;
    // work->callback.Reset(i.isolate, cb);
    uv_queue_work(uv_default_loop(), &work->requst, [](uv_work_t *req) {
      Work *work = static_cast<Work *>(req->data);

      Blob blob;
      work->info.obj->image.write(&blob);
      work->result = blob;
    },
        WorkBlobComplete);

    args.GetReturnValue().Set(Undefined(i.isolate));
  }

  void MagickNode::Save(const FunctionCallbackInfo<Value> &args) {
    CallbackInfo i = MagickNode::Info(args);

    // We need a save location, make sure one parameter is set
    if (args.Length() < 1) {
      i.isolate->ThrowException(Exception::Error(String::NewFromUtf8(i.isolate, "Save requires at least one parameter.")));
      return;
    }

    // Get the save location
    String::Utf8Value location(args[0]->ToString());

    Local<Function> cb;
    if (args.Length() == 2) {
      cb = Local<Function>::Cast(args[1]);
    } else if (args.Length() == 3) {
      double quality = args[1]->NumberValue(i.context).FromMaybe(0);
      i.obj->image.quality(static_cast<size_t>(quality));
      cb = Local<Function>::Cast(args[2]);
    }

    Work *work        = new Work();
    work->requst.data = work;
    work->info        = i;
    work->args[0]     = String::NewFromUtf8(i.isolate, *location);
    // work->callback.Reset(i.isolate, cb);
    // Write out the file to the location
    uv_queue_work(uv_default_loop(), &work->requst, [](uv_work_t *req) {
      Work *work = static_cast<Work *>(req->data);
      work->info.obj->image.write(*String::Utf8Value(work->args[0]->ToString()));
    },
        WorkVoidComplete);

    // args.GetReturnValue().Set(args.This());
  }

  CallbackInfo MagickNode::Info(const FunctionCallbackInfo<Value> &args) {
    CallbackInfo item;
    item.isolate = args.GetIsolate();
    item.context = item.isolate->GetCurrentContext();
    item.obj     = ObjectWrap::Unwrap<MagickNode>(args.Holder());
    // item.args[args.Length()]
    item.args[args.Length()];
    for (int i = 0; i < args.Length(); i++) {
      item.args[i] = args[i];
    }
    // item.args    = args;
    return item;
  }

  CallbackInfo MagickNode::Info(const PropertyCallbackInfo<Value> &args) {
    CallbackInfo item;
    item.isolate = args.GetIsolate();
    item.context = item.isolate->GetCurrentContext();
    item.obj     = ObjectWrap::Unwrap<MagickNode>(args.Holder());
    // item.args    = args;
    return item;
  }

  void MagickNode::WorkBlobComplete(uv_work_t *req, int status) {
    Isolate *isolate = Isolate::GetCurrent();
    HandleScope handleScope(isolate);
    Work *work = static_cast<Work *>(req->data);

    MaybeLocal<Object> buffer = node::Buffer::Copy(isolate, (char *)work->result.data(), work->result.length());

    Local<Value> out;
    buffer.ToLocal(&out);
    // work->resolver->Resolve(out);

    // Local<Value> argv[1] = {out};
    // Local<Function>::New(isolate, work->callback)->Call(isolate->GetCurrentContext()->Global(), 1, argv);

    // work->callback.Reset();
    delete work;
  }

  void MagickNode::WorkVoidComplete(uv_work_t *req, int status) {
    Work *work = static_cast<Work *>(req->data);
    HandleScope handleScope(work->info.isolate);
    Log("Work Done!");

    work->resolver.Get(work->info.isolate)->Resolve(Undefined(work->info.isolate));

    // work->resolver->Resolve(Undefined(work->info.isolate));

    // auto persistant = static_cast<Persistent<Promise::Resolver> *>(work->resolver);

    // auto resolver = Promise::Resolver::New(work->info.isolate);
    // resolver->Resolve(work->info.context, Undefined(work->info.isolate));
    // resolver

    // work->resolver->Get(work->info.isolate)->Resolve(work->info.context, Undefined(work->info.isolate));
    // MagickNode::resolver.Get(work->info.isolate)->Resolve(work->info.context, Undefined(work->info.isolate));

    // Local<Promise::Resolver> resolver = static_cast<Local<Promise::Resolver>>(work->resolver.Get(work->info.isolate));
    // resolver->Resolve(work->info.context, Undefined(work->info.isolate));

    // work->resolver->Resolve(work->info.context, Undefined(work->info.isolate));

    // Local<Value> argv[1] = {Undefined(isolate)};
    // Local<Function>::New(isolate, work->callback)->Call(isolate->GetCurrentContext()->Global(), 0, argv);

    // work->callback.Reset();
    delete work;
  }

  void Log(string message) {
    printf((message + "\n").c_str());
  }

  void Log(char *message) {
    printf((string(message) + "\n").c_str());
  }

}; // namespace NodeImageMagick