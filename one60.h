#ifndef __ONE60__
#define __ONE60__

#include <fstream>

#ifndef WIN32

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _DIR_SLASH_CHAR
#define _DIR_SLASH_CHAR "/"
#endif

#else

#include <direct.h>

#ifndef _DIR_SLASH_CHAR
#define _DIR_SLASH_CHAR "\\"
#endif

#endif

#include "prs.h"

#define ONE60_FILENAME_LENGTH 0x2C

class one60
{
private:
	struct one_header
	{
		dword number;
		dword size;
		dword version;
		char signature[0x0C];
		dword unknown;
		dword count;
		char garbage[0x90];
	} header;
	struct one_entry
	{
		char filename[ONE60_FILENAME_LENGTH];
		dword unknown0;
		dword offset;
		dword unknown1; // flag?
	} *entries;

	std::fstream one_file;
	char *one_filename;
	prs prs_data;

	dword size;
	dword version;

	byte *file_buffer;
public:
	one60(void);
	one60(const char *filename);
	~one60(void);
	void delete_file_buffer(void);
	void delete_one_filename(void);
private:
	void read_entries(void);
	void write_entries(void);
public:
	bool open_file(const char *filename);
	void close_file(void);

	dword get_index(const char *entryname);
	char *get_entryname(dword index);
	char *get_entrynames(void);
	dword get_offset(dword index);
	dword get_offset(const char *entryname);
	dword get_size(dword index);
	dword get_size(const char *entryname);
	dword get_size_60(dword index);
	dword get_size_60(const char *entryname);
	char *get_filename(void);

	bool extract_file(dword index, const char *filename);
	bool extract_file(const char *entryname, const char *filename);
	void extract_buffer(dword index, byte *buffer);
	void extract_buffer(const char *entryname, byte *buffer);
	bool extract_archive(void);
	bool extract_archive(const char *directory);

	bool create_archive(const char *onename, const char *listname);
};

#endif // __ONE60__
