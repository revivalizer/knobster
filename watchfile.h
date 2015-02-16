#pragma once

class ZSource
{
public:
	ZSource()
		: data(nullptr)
		, updateTime(-1) // when was the file last read?
		, checkTime(-1)
	{

	}
	virtual void Update(void) {};

	char* data;

	int32_t checkTime;
	int32_t updateTime;
};

class ZStaticString : public ZSource
{
public:
	ZStaticString(char* str);
	virtual void Update(void);
};

class ZWatchFile : public ZSource
{
public:
	ZWatchFile(const char* path);
	~ZWatchFile(void);

	virtual void Update(void);

	#ifndef SWIG
		HANDLE hFile;
		FILETIME ftLastRead;
	#endif

	private:
		const char* path;
};