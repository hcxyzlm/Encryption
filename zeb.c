
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

	// ��������
	int i = 0;
	for (i = 0; i < bufSize; i++) {
		byte b = buffer[i];
		byte index = (byte) (indexKey - (i % indexKey));
		byte c = (keySize > 0) ? keys[i % keySize] : 0;
		c += index;
		// ����ƫ��
		b = (byte) ((b + (offsetKey + keySize + c) + 256) % 256);
		// ����ת��
		b = (byte) (b ^ c);
		// ��������
		out[i + offset] = b;
	};
	return bufSize + offset;
	//return bufSize;
}

int decrypt0(byte* buffer, int bufSize, byte* keys, int keySize, byte* out, int offset)
{
	// ��������
	int i = 0;
	for (i = 0; i < bufSize; i++) {
		byte b = buffer[i];
		byte index = (byte) (indexKey - (i % indexKey));
		byte c = (keySize > 0) ? keys[i % keySize] : 0;
		c += index;
		// ����ת��
		b = (byte) (b ^ c);
		// ����ƫ��
		b = (byte) ((b - (offsetKey + keySize + c) + 256) % 256);
		// ��������
		out[i + offset] = b;
	}

	return bufSize + offset;
	//return 0;
}










static int encrypt(lua_State *L)
{
	//��ȡ��һ������
	size_t bufferSize = 0;
	const char* buffer = lua_tolstring(L, 1, &bufferSize);

	size_t keySize = 0;
	const char* keys = lua_tolstring(L, 2, &keySize);

	//����
	byte* out = (byte *)malloc(bufferSize+1);
	int outSize = encrypt0((byte*)buffer, (int)bufferSize, (byte*)keys, (int)keySize, out, 0);
	out[outSize] = '\0';	//д�������


	//��������
	lua_pushfstring(L, (const char*)out);

	//�ͷ��ڴ�
	free(out);
    return 1;
}

static int decrypt(lua_State *L)
{
	//��ȡ��һ������
	size_t bufferSize = 0;
	const char* buffer = lua_tolstring(L, 1, &bufferSize);

	size_t keySize = 0;
	const char* keys = lua_tolstring(L, 2, &keySize);

	//����
	byte* out = (byte *)malloc(bufferSize+1);
	int outSize = decrypt0((byte*)buffer, (int)bufferSize, (byte*)keys, (int)keySize, out, 0);
	out[outSize] = '\0';	//д�������


	//��������
	//lua_pushfstring(L, (const char*)out);
	lua_pushlstring(L, (const char*)out, bufferSize);

	//�ͷ��ڴ�
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