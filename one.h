#ifndef __ONE__
#define __ONE__

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

#define RW_VERSION 0x1400FFFF

class one
{
private:
	struct one_entry
	{
		dword number;
		dword size;
		dword version;
	} entry;

	std::fstream one_file;
	char *one_filename;
	prs prs_data;

	dword size;
	dword version;

	byte *file_buffer;
	char *string_table;
	dword string_entries;
public:
	one(void);
	one(const char *filename);
	~one(void);
	void delete_file_buffer(void);
	void delete_string_table(void);
	void delete_one_filename(void);
private:
	void read_entry(void);
	void write_entry(void);
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
	char *get_filename(void);

	bool extract_file(dword index, const char *filename);
	bool extract_file(const char *entryname, const char *filename);
	void extract_buffer(dword index, byte *buffer);
	void extract_buffer(const char *entryname, byte *buffer);
	bool extract_archive(void);
	bool extract_archive(const char *directory);

	bool create_archive(const char *onename, const char *listname);
};

#endif // __ONE__
