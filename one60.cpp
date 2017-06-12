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

              .ONE v0.60 file handling class written by Lightning.
   For use with Shadow the Hedgehog's and any other applicable games' archives.

               Many thanks go to Nemesis; without him, the .PRS
             aspects of this program wouldn't have been possible.
*/

// 01/04/2006 - Re-implemented stuff for new Shadow format

#include "one60.h"
#include <cstring>

one60::one60(void)
{
}
one60::one60(const char *filename)
{
    open_file(filename);
}
one60::~one60(void)
{
	if(one_file.is_open())
		one_file.close();
}
void one60::delete_file_buffer(void)
{
	if(!file_buffer)
		delete(file_buffer);
}
void one60::delete_one_filename(void)
{
	if(!one_filename)
		delete(one_filename);
}

void one60::read_entries(void)
{
	entries = new one_entry[header.count];
	one_file.read((char *)entries, sizeof(entries[0]) * header.count);
}
void one60::write_entries(void)
{
	one_file.write((char *)entries, sizeof(entries[0]) * header.count);
}

bool one60::open_file(const char *filename)
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
	one_file.read((char *)&header, sizeof(header));

	read_entries();

	one_file.seekg(0, std::ios::end);
	size = one_file.tellg();
	one_file.seekg(0, std::ios::beg);

	one_filename = temp;

	return(true);
}
void one60::close_file(void)
{
	if(one_file.is_open())
		one_file.close();
}

dword one60::get_index(const char *entryname)
{
	for(register dword i = 0; i < header.count; i++)
		if(!strcmp(entries[i].filename, entryname))
			return(i);

	return(0);
}
char *one60::get_entryname(dword index)
{
	char *temp = new char[ONE60_FILENAME_LENGTH];
	strcpy(temp, entries[index].filename);

	return(temp);
}
char *one60::get_entrynames(void)
{
	char *temp = new char[header.count * (ONE60_FILENAME_LENGTH + 2)];
	*temp = '\0';

	for(register dword i = 0; i < header.count; i++)
	{
		strcat(temp, entries[i].filename);
		strcat(temp, "\n");
	}

	return(temp);
}
dword one60::get_offset(dword index)
{
	one_file.seekg(entries[index].offset);
	return(one_file.tellg());
}
dword one60::get_offset(const char *entryname)
{
	return(get_offset(get_index(entryname)));
}
dword one60::get_size(dword index)
{
	return(get_size_60(index));
	/*get_offset(index);
	dword temp = get_size_60(index);

	delete_file_buffer();
	file_buffer = new byte[temp];

	one_file.read((char *)file_buffer, temp);
	prs_data.read_cmp_buffer(file_buffer, temp);

	return(prs_data.get_dec_size());*/
}
dword one60::get_size(const char *entryname)
{
	return(get_size(get_index(entryname)));
}

dword one60::get_size_60(dword index)
{
	dword end_of_file = (index + 1 >= header.count) ?
		(header.size + 12) :
		(entries[index + 1].offset);

	return(end_of_file - entries[index].offset);
}
dword one60::get_size_60(const char *entryname)
{
	return(get_size_60(get_index(entryname)));
}

char *one60::get_filename(void)
{
	return(one_filename);
}

bool one60::extract_file(dword index, const char *filename)
{
	get_offset(index);
	dword temp = get_size_60(index);

	delete_file_buffer();
	file_buffer = new byte[temp];

	one_file.read((char *)file_buffer, temp);

	std::fstream output_file;
	output_file.open(filename, std::ios::out | std::ios::binary);
	output_file.write((char *)file_buffer, temp);
	output_file.close();

	//prs_data.read_cmp_buffer(file_buffer, temp);
	//return(prs_data.write_dec_file(filename));
}
bool one60::extract_file(const char *entryname, const char *filename)
{
	return(extract_file(get_index(entryname), filename));
}
void one60::extract_buffer(dword index, byte *buffer)
{
	get_offset(index);
	dword temp = get_size_60(index);

	delete_file_buffer();
	file_buffer = new byte[temp];

	one_file.read((char *)file_buffer, temp);
	memcpy(buffer, file_buffer, temp);

	//prs_data.read_cmp_buffer(file_buffer, temp);
	//prs_data.write_dec_buffer(buffer);
}
void one60::extract_buffer(const char *entryname, byte *buffer)
{
	extract_buffer(get_index(entryname), buffer);
}
bool one60::extract_archive(void)
{
	for(register dword i = 0; i < header.count; i++)
		if(!extract_file(i, get_entryname(i)))
			return(false);

	return(true);
}
bool one60::extract_archive(const char *directory)
{
	mkdir(directory, 775);

	char *temp = new char[strlen(directory) + ONE60_FILENAME_LENGTH + 8];
	char *end;

	strcpy(temp, directory);
	if(strrchr(temp, *_DIR_SLASH_CHAR) - temp != strlen(temp) - 1)
		strcat(temp, _DIR_SLASH_CHAR);

	end = strrchr(temp, *_DIR_SLASH_CHAR) + 1;
	for(register dword i = 0; i < header.count; i++)
	{
		memset(end + 1, '\0', ONE60_FILENAME_LENGTH + 6);
		strcpy(end, get_entryname(i));

		if(!extract_file(i, temp))
			return(false);
	}
	delete(temp);

	return(true);
}

bool one60::create_archive(const char *onename, const char *listname)
{
/*	std::fstream list_file;
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
*/	return(false);
}
