// v8engine.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <v8engine.hpp>

#ifdef _V8_LATEST

using namespace v8;

// Reads a file into a v8 string.
v8::Local<v8::String> _ReadFile(Isolate* isolate, const char* name)
{
#pragma warning(disable : 4996)
    FILE* file = fopen(name, "rb");
    if (file == NULL) return v8::Handle<v8::String>();
    
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    
    char* chars = new char[size + 1];
    chars[size] = '\0';
    for (int i = 0; i < size;)
    {
        int read = fread(&chars[i], 1, size - i, file);
        i += read;
    }
    fclose(file);
    v8::Local<v8::String> result = v8::String::NewFromUtf8(isolate, chars, NewStringType::kNormal).ToLocalChecked();
    delete[] chars;
    return result;
    
}

/*class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
public:
    virtual void *Allocate(size_t length) {
        void *data = AllocateUninitialized(length);
        return data == NULL ? data : memset(data, 0, length);
    }
    virtual void *AllocateUninitialized(size_t length) { return malloc(length); }
    virtual void Free(void *data, size_t) { free(data); }
};*/

v8Engine::v8Engine(std::function<void (std::string)> logFunc /*= nullptr*/){
    _logFunc = logFunc;
}

v8Engine::~v8Engine(){
}


void v8Engine::run(std::function<void (v8Engine::v8Context&)> onReady)
{
    // Create a new Isolate and make it the current one.
    //ArrayBufferAllocator allocator;
    Isolate::CreateParams create_params;
    //create_params.array_buffer_allocator = &allocator;
    create_params.array_buffer_allocator =
    v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    Isolate* isolate = Isolate::New(create_params);
    {
        log("v8 Engine Starting ..");
        
        Isolate::Scope isolate_scope(isolate);
        // Create a stack-allocated handle scope.
        HandleScope handle_scope(isolate);
        // Create a new context.
        v8::Local<v8::Context> context = Context::New(isolate);
        // Enter the context for compiling and running the hello world script.
        Context::Scope context_scope(context);
        // Create a string containing the JavaScript source code.
        Local<String> source =
        String::NewFromUtf8(isolate,
                            "function matcher() { var elem = arguments[0]; \
                            var args = JSON.parse(arguments[1]); \
                            var match = 0;if(args[0] == args[1]) { match = 1; } return match; }",
                            NewStringType::kNormal).ToLocalChecked();
        // Compile the source code.
        Local<Script> script = Script::Compile(context, source).ToLocalChecked();
        // Run the script to get the result.
        Local<Value> result = script->Run(context).ToLocalChecked();
        
        v8Context v8Ctx(isolate, context);
        onReady(v8Ctx);
        // Convert the result to an UTF8 string and print it.
        //String::Utf8Value utf8(isolate, result);
        //printf("%s\n", *utf8);
    }
    // Dispose the isolate and tear down V8.
    isolate->Dispose();
   
    delete create_params.array_buffer_allocator;
    
    
}

std::string v8Engine::CallJSFunction(v8Engine::v8Context& v8Ctx, std::string funcName, v8::Local<v8::Value> argList[], unsigned int argCount) {
    
    
    Local<String> fncName = String::NewFromUtf8(v8Ctx.isolate, funcName.c_str(), NewStringType::kNormal).ToLocalChecked();
    
    Local<Value> func_val;
    // If there is no Process function, or if it is not a function,
    // bail out
    if (!v8Ctx.context->Global()->Get(v8Ctx.context, fncName).ToLocal(&func_val) ||
        !func_val->IsFunction()) {
        return "";
    }
    
    // It is a function; cast it to a Function
    Local<Function> func = Local<Function>::Cast(func_val);
    
    Local<Value> result;
    if (!func->Call(v8Ctx.context, v8Ctx.context->Global(), argCount, argList).ToLocal(&result)) {
        //String::Utf8Value error(try_catch.Exception());
        //return error;
        return "";
    }
    String::Utf8Value utf8(v8Ctx.isolate, result);
    
    return *utf8;
}

std::string v8Engine::invoke(v8Engine::v8Context& v8Ctx, std::string funcName, std::string element,
                             std::string params){
    v8::Local<v8::Value>  args[2];
    
    args[0] =  String::NewFromUtf8(v8Ctx.isolate, element.c_str(),
                                   NewStringType::kNormal).ToLocalChecked();
    args[1] =  String::NewFromUtf8(v8Ctx.isolate, params.c_str(),
                                   NewStringType::kNormal).ToLocalChecked();
    
    return CallJSFunction(v8Ctx, funcName, args, 2);
    
}

#else

v8Engine::v8Engine(v8::Persistent<v8::Context>& ctx){
    
    
    //context->AllowCodeGenerationFromStrings(true);
    
    // Enter the created context for compiling and
    // running the hello world script.
    /*v8::Handle<v8::String> source;
    v8::Handle<v8::Script> script;
    v8::Handle<v8::Value> result;
    
    
    // Create a string containing the JavaScript source code.
    source = v8::String::New("function matcher() { var elem = arguments[0]; \
                             var args = JSON.parse(arguments[1]); \
                             var match = 0;if(args[0] == args[1]) { match = 1; } return match; }");
    
    // Compile the source code.
    script = v8::Script::Compile(source);
    
    // Run the script to get the result.
    result = script->Run();
    
    // Dispose the persistent context.
    ctx.Dispose();
    
    global = ctx->Global();*/
    
    
}

v8Engine::~v8Engine(){
    
}

std::string v8Engine::invoke(const char* fnc, const char* element, const char* arguments){
    
    v8::Handle<v8::Value> value = global->Get(v8::String::New(fnc));
    v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
    
    v8::Handle<v8::Value> args[2];
    args[0] = v8::String::New(element);
    args[1] = v8::String::New(arguments);
    
    v8::Handle<v8::Value> js_result = func->Call(global, 2, args);
    v8::String::AsciiValue ascii(js_result);
    
    std::string final_result = *ascii;
    
    return final_result;
}

#endif