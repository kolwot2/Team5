#include "ByteConverter.h"

std::string toBytes(int x)
{
	std::string bytes(reinterpret_cast<char*>(&x));
	return bytes;
}

std::string toBytes4(size_t x)
{
	int ix = x;
	std::string bytes(reinterpret_cast<char*>(&ix));
	return bytes;
}

std::string toBytes(size_t x)
{
	std::string bytes(reinterpret_cast<char*>(&x));
	return bytes;
}
