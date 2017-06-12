/*

 11             11111    11    11  1111111 111       1     111       1    11111
111       /\   11  1    111   111 111111111 111     11  /\  111     11   11  1
 11       \/  1          11    11     11     1111   11  \/   1111   11  1
 11          11          11    11     11     11111  11       11111  11 11
 11      111 11     11  111111111     11     11 111 11 111   11 111 11 11     11
 11       11 11    111   11111111     11     11  11111  11   11  11111 11    111
 11       11 111    11   11    11     11     11   1111  11   11   1111 111    11
 11       11  11    1    11    11     11     11    111  11   11    111  11    1
 1111111  11  111   1    11    11     11     11     11  11   11     11  111   1
    111   1    11111     1     1      1      1       1  1    1       1   11111

                .ONE file handling class written by Lightning.
      For use with Sonic Heroes' and any other applicable games' archives.

               Many thanks go to Nemesis; without him, the .PRS
             aspects of this program wouldn't have been possible.
*/

// 11/12/2005 - Released alongside the buggy PRS class
// 10/15/2005 - Added delete_this_buffer() functions for garbage collection
// 10/14/2005 - Added a call to int mkdir(char*) in bool extract_archive(char*)
// 10/12/2005 - Fixed a minor design flaw involving buffer sizes
// 10/03/2005 - First private release

#include "one.h"
#include <cstring>

one::one(void)
{
}
one::one(const char *filename)
{
    open_file(filename);
}
one::~one(void)
{
	if(one_file.is_open())
		one_file.close();
}
void one::delete_file_buffer(void)
{
	if(!file_buffer)
		delete[](file_buffer);
}
void one::delete_string_table(void)
{
	if(!string_table)
		delete[](string_table);
}
void one::delete_one_filename(void)
{
	if(!one_filename)
		delete[](one_filename);
}

void one::read_entry(void)
{
	one_file.read((char *)&entry, sizeof(entry));
}
void one::write_entry(void)
{
	one_file.write((char *)&entry, sizeof(entry));
}

bool one::open_file(const char *filename)
{
	close_file();

	char *temp = new char[strlen(filename) + 2];
	strcpy(temp + 2, filename);
	if(!(temp[2] == '.' || temp[3] == ':'))
		memcpy(temp, "."_DIR_SLASH_CHAR, 2);
	else
		temp += 2;

	one_file.open(temp, std::ios::in | std::ios::out | std::ios::binary);
	if(!one_file.good())
		return(false);

	read_entry();
	if(!entry.number)
		size = entry.size + sizeof(one_entry);
	else
	{
		one_file.seekg(0, std::ios::end);
		size = one_file.tellg();
		one_file.seekg(0, std::ios::beg);
	}

	version = entry.version;
	read_entry();

	delete_string_table();
	string_table = new char[entry.size];
	string_entries = entry.size >> 6;
	one_file.read(string_table, entry.size);

	one_filename = temp;

	return(true);
}
void one::close_file(void)
{
	if(one_file.is_open())
		one_file.close();
}

dword one::get_index(const char *entryname)
{
	for(register dword i = 0; i < string_entries; i++)
		if(!strcmp(string_table + (i << 6), entryname))
			return(i);

	return(0);
}
char *one::get_entryname(dword index)
{
	char *temp = new char[0x40];

	strcpy(temp, string_table + (index << 6));

	if(!*temp)
		if(index == 0)
			temp = "one_file.dat";
		else if(index == 1)
			temp = "string_table.dat";

	return(temp);
}
char *one::get_entrynames(void)
{
	char *temp = new char[string_entries << 6];
	*temp = '\0';

	for(register dword i = 2; *get_entryname(i); i++)
	{
		strcat(temp, get_entryname(i));
		strcat(temp, "\n");
	}

	return(temp);
}
dword one::get_offset(dword index)
{
	one_file.seekg(sizeof(one_entry), std::ios::beg);

	while((dword)one_file.tellg() < size)
	{
		read_entry();
		if(entry.number == index)
			return(one_file.tellg());

		one_file.seekg(entry.size, std::ios::cur);
	}

	one_file.seekg(0, std::ios::beg);
	read_entry();

	return(one_file.tellg());
}
dword one::get_offset(const char *entryname)
{
	return(get_offset(get_index(entryname)));
}
dword one::get_size(dword index)
{
	get_offset(index);

	delete_file_buffer();
	file_buffer = new byte[entry.size];

	one_file.read((char *)file_buffer, entry.size);
	prs_data.read_cmp_buffer(file_buffer, entry.size);

	return(prs_data.get_dec_size());
}
dword one::get_size(const char *entryname)
{
	return(get_size(get_index(entryname)));
}
char *one::get_filename(void)
{
	return(one_filename);
}

bool one::extract_file(dword index, const char *filename)
{
	get_offset(index);

	delete_file_buffer();
	file_buffer = new byte[entry.size];

	one_file.read((char *)file_buffer, entry.size);
	prs_data.read_cmp_buffer(file_buffer, entry.size);

	return(prs_data.write_dec_file(filename));
}
bool one::extract_file(const char *entryname, const char *filename)
{
	return(extract_file(get_index(entryname), filename));
}
void one::extract_buffer(dword index, byte *buffer)
{
	get_offset(index);

	delete_file_buffer();
	file_buffer = new byte[entry.size];

	one_file.read((char *)file_buffer, entry.size);
	prs_data.read_cmp_buffer(file_buffer, entry.size);

	prs_data.write_dec_buffer(buffer);
}
void one::extract_buffer(const char *entryname, byte *buffer)
{
	extract_buffer(get_index(entryname), buffer);
}
bool one::extract_archive(void)
{
	for(register dword i = 2; *get_entryname(i); i++)
		if(!extract_file(i, get_entryname(i)))
			return(false);

	return(true);
}
bool one::extract_archive(const char *directory)
{
	mkdir(directory, 775);

	char *temp = new char[strlen(directory) + 0x48];
	char *end;

	strcpy(temp, directory);
	if(strrchr(temp, *_DIR_SLASH_CHAR) - temp != strlen(temp) - 1)
		strcat(temp, _DIR_SLASH_CHAR);

	end = strrchr(temp, *_DIR_SLASH_CHAR) + 1;
	for(register dword i = 2; *get_entryname(i); i++)
	{
		memset(end + 1, '\0', 0x46);
		strcpy(end, get_entryname(i));

		if(!extract_file(i, temp))
			return(false);
	}
	delete(temp);

	return(true);
}

bool one::create_archive(const char *onename, const char *listname)
{
	std::fstream list_file;
	dword list_size;
	dword entries = 2;
	register dword i;
	char list_strings[0x100][0x400];
	char *temp;

	close_file();
	one_file.open(onename, std::ios::in | std::ios::out | std::ios::binary);
	list_file.open(listname, std::ios::in);
	list_file.seekg(0, std::ios::end);
	list_size = list_file.tellg();
	list_file.seekg(0, std::ios::beg);

	entry.version = version = RW_VERSION;
	entry.number = 0;
	entry.size = 0x7FFFFFFF;
	write_entry();

	entry.number++;
	entry.size = 0x4000;
	write_entry();

	for(i = 0; i < 0x100; i++)
		memset(list_strings[i], '\0', 0x400);

	while(list_file.tellg() < list_size - 1)
		list_file.getline(list_strings[entries++], 0x400);

	list_file.close();

	delete_string_table();
	string_table = new char[0x4000];
	memset(string_table, '\0', 0x4000);

	for(i = 2; i < 0x100; i++)
		if(temp = strrchr(list_strings[i], '\\'))
			strcpy(string_table + (i << 6), ++temp);
		else
			strcpy(string_table + (i << 6), list_strings[i]);
	one_file.write(string_table, 0x4000);

	for(entry.number = 2; entry.number < entries; entry.number++)
	{
		if(!prs_data.read_dec_file(list_strings[entry.number]))
			return(false);
		if(!prs_data.compress())
			return(false);

		entry.size = prs_data.get_cmp_size();
		write_entry();

		delete_file_buffer();
		file_buffer = new byte[entry.size];

		prs_data.write_cmp_buffer(file_buffer);
		one_file.write((char *)file_buffer, entry.size);
	}

	entry.number = 0;
	entry.size = size = (dword)one_file.tellp() - sizeof(one_entry);
	one_file.seekp(0, std::ios::beg);
	write_entry();

	delete_one_filename();
	one_filename = new char[strlen(onename)];
	strcpy(one_filename, onename);
	return(true);
}
