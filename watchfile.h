#pragma once

#ifndef STANDALONE // DEVELOPMENT
	class ZWatchFile : public align16
	{
	public:
		ZWatchFile(const char* path);
		~ZWatchFile(void);

		void Update(void);

		char* data;

		int32_t checkTime;
		int32_t readTime;

		#ifndef SWIG
			HANDLE hFile;
			FILETIME ftLastRead;
		#endif

		private:
			const char* path;
	};
#else // STANDALONE
	class ZWatchFile : public align16
	{
	public:
		ZWatchFile(const char* path);

		void Update(void);

		char* data;

		int32_t checkTime;
		int32_t readTime;
	};
#endif

