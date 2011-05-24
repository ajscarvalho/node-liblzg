/**
 * Fast string compression and decompression (and small code for decompression)
 *
 * @package liblzg
 * @link http://alexandre.amaraldecarvalho.eu/
 * @autor Alexandre Carvalho <ajscarvalho at gmail>
 * @version 1.0
 */
 
#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include <node_version.h>

#include <stdlib.h>

#include "src/internal.h"



using namespace v8;
using namespace node;

// node v0.2.x compatibility
#if NODE_VERSION_AT_LEAST(0,3,0)
	#define Buffer_Data Buffer::Data
	#define Buffer_Length Buffer::Length
	#define Buffer_New Buffer::New
#else
	inline char* Buffer_Data(Handle<Object> obj) {
		return (ObjectWrap::Unwrap<Buffer>(obj))->data();
	}
	inline size_t Buffer_Length(Handle<Object> obj) {
		return (ObjectWrap::Unwrap<Buffer>(obj))->length();
	}
	inline Buffer* Buffer_New(char* data, size_t length) {
		Buffer* buffer = Buffer::New(length);
		memcpy(buffer->data(), data, length);
		return buffer;
	}
#endif


/**
 * arguments:
 *		data: UTF8 String data to be compressed,
 *		fast: int fast mode - {1,0}
 *		compresison level: [1-9]
 */
Handle<Value> compress(const Arguments& args)
{
	HandleScope scope;
	String::Utf8Value data(args[0]->ToString());
	int fast(args[1]->Int32Value());
	int level(args[2]->Int32Value());

	if (level < 1 || level > 9) return scope.Close(ThrowException(Exception::Error(String::New("Invalid Level [1-9]"))));
	if (fast < 0 || fast > 1) return scope.Close(ThrowException(Exception::Error(String::New("Invalid Fast Mode {0,1}"))));

	unsigned char* decBuf = (unsigned char*) *data;
	lzg_uint32_t decSize = data.length();
	lzg_uint32_t maxEncSize = LZG_MaxEncodedSize(decSize);
	unsigned char *encBuf = (unsigned char*) malloc(maxEncSize);
	if (!encBuf) return scope.Close(ThrowException(Exception::Error(String::New("Memory Exhausted"))));

	lzg_encoder_config_t config;
	LZG_InitEncoderConfig(&config);
	config.fast = fast;
	config.level = level;

	lzg_uint32_t encSize = LZG_Encode(decBuf, decSize, encBuf, maxEncSize, &config);
	if (!encSize) return scope.Close(ThrowException(Exception::Error(String::New("Compression failed!\n"))));

	Buffer* output = Buffer_New((char *) encBuf, encSize);
	free(encBuf);
	return scope.Close(Local<Value>::New(output->handle_));
}

Handle<Value> decompress(const Arguments& args)
{
	HandleScope scope;

	if (args.Length() < 1 || !Buffer::HasInstance(args[0]))
		return scope.Close(ThrowException(Exception::Error(String::New("Expected Buffer as first argument"))));

	Local<Object> input = args[0]->ToObject();
	unsigned char * encBuf = (unsigned char *) Buffer_Data(input);
	lzg_uint32_t encSize = Buffer_Length(input);

	lzg_uint32_t decSize = LZG_DecodedSize(encBuf, encSize);
	if (!decSize) return scope.Close(ThrowException(Exception::Error(String::New("Bad input data!"))));

	unsigned char* decBuf = (unsigned char*) malloc(decSize);
	if (!decBuf) return scope.Close(ThrowException(Exception::Error(String::New("Memory Exhausted!"))));

	decSize = LZG_Decode(encBuf, encSize, decBuf, decSize);
	if (!decSize)return scope.Close(ThrowException(Exception::Error(String::New("Decompression failed!"))));

	return scope.Close(String::New((char*)decBuf, decSize));
}

extern "C" void init (Handle<Object> target)
{
	HandleScope scope;
	target->Set(String::New("compress"), FunctionTemplate::New(compress)->GetFunction());
	target->Set(String::New("decompress"), FunctionTemplate::New(decompress)->GetFunction());
}
