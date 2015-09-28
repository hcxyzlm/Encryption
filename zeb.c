
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdint.h>
#include <string.h>

#define byte unsigned char
#define offsetKey 18
#define indexKey 121


int encrypt0(byte* buffer, int bufSize, byte* keys, int keySize, byte* out, int offset)
{
	//memcpy(out, buffer, bufSize);

	// 遍历处理
	int i = 0;
	for (i = 0; i < bufSize; i++) {
		byte b = buffer[i];
		byte index = (byte) (indexKey - (i % indexKey));
		byte c = (keySize > 0) ? keys[i % keySize] : 0;
		c += index;
		// 数据偏移
		b = (byte) ((b + (offsetKey + keySize + c) + 256) % 256);
		// 数据转化
		b = (byte) (b ^ c);
		// 设置数据
		out[i + offset] = b;
	};
	return bufSize + offset;
	//return bufSize;
}

int decrypt0(byte* buffer, int bufSize, byte* keys, int keySize, byte* out, int offset)
{
	// 遍历处理
	int i = 0;
	for (i = 0; i < bufSize; i++) {
		byte b = buffer[i];
		byte index = (byte) (indexKey - (i % indexKey));
		byte c = (keySize > 0) ? keys[i % keySize] : 0;
		c += index;
		// 数据转化
		b = (byte) (b ^ c);
		// 数据偏移
		b = (byte) ((b - (offsetKey + keySize + c) + 256) % 256);
		// 设置数据
		out[i + offset] = b;
	}

	return bufSize + offset;
	//return 0;
}










static int encrypt(lua_State *L)
{
	//读取第一个参数
	size_t bufferSize = 0;
	const char* buffer = lua_tolstring(L, 1, &bufferSize);

	size_t keySize = 0;
	const char* keys = lua_tolstring(L, 2, &keySize);

	//加密
	byte* out = (byte *)malloc(bufferSize+1);
	int outSize = encrypt0((byte*)buffer, (int)bufferSize, (byte*)keys, (int)keySize, out, 0);
	out[outSize] = '\0';	//写入结束符


	//返回数据
	lua_pushfstring(L, (const char*)out);

	//释放内存
	free(out);
    return 1;
}

static int decrypt(lua_State *L)
{
	//读取第一个参数
	size_t bufferSize = 0;
	const char* buffer = lua_tolstring(L, 1, &bufferSize);

	size_t keySize = 0;
	const char* keys = lua_tolstring(L, 2, &keySize);

	//解码
	byte* out = (byte *)malloc(bufferSize+1);
	int outSize = decrypt0((byte*)buffer, (int)bufferSize, (byte*)keys, (int)keySize, out, 0);
	out[outSize] = '\0';	//写入结束符


	//返回数据
	//lua_pushfstring(L, (const char*)out);
	lua_pushlstring(L, (const char*)out, bufferSize);

	//释放内存
	free(out);
    return 1;
}




static const struct luaL_reg zedlib[] = {
	{"encrypt", encrypt},
	{"decrypt", decrypt},
	{NULL, NULL}
};

LUALIB_API int luaopen_zed (lua_State *L) {
  luaL_register(L, "ZED", zedlib);
  return 1;
}