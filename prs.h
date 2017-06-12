#ifndef __PRS__
#define __PRS__

#include <fstream>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

#define PRS_MODE			byte
#define PRS_MODE_STORAGE	0x1
#define PRS_MODE_OFFSETCPY	0x2
#define PRS_MODE_EXTENDCPY	0x4
#define PRS_MODE_ALIGNED	0x8
#define PRS_MODE_DEFAULT	0x1 // replace with 0xF when compression works

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

class prs
{
private:
	byte *cmp_buffer;
	byte *dec_buffer;
	dword cmp_size, cmp_index;
	dword dec_size, dec_index;

	bool tag_reposition;
	byte tag_byte;
	byte tag_bits;
	dword tag_index;

	byte min_length;
	dword offset_count;
	word offset;

	bool compressed;
	bool decompressed;
	byte compression;
	byte decompression;
public:
	prs(void);
	~prs(void);
	void reset(void);
	void delete_cmp_buffer(void);
	void delete_dec_buffer(void);
private:
	byte cmp_get_byte(void);
	byte dec_get_byte(void);
	void cmp_put_byte(byte value);
	void dec_put_byte(byte value);
public:
	bool decompress(PRS_MODE mode);
	bool decompress(void);
private:
	bool dec_get_bit(void);
	bool dec_extended_copy(void);
	bool dec_offset_copy(void);
	bool dec_copy(void);
public:
	bool compress(PRS_MODE mode);
	bool compress(void);
private:
	void cmp_put_bit(bool bit);
	PRS_MODE cmp_find_best(PRS_MODE mode);
public:
	bool read_cmp_file(const char *filename);
	bool read_dec_file(const char *filename);
	void read_cmp_buffer(const byte *new_buffer, const dword new_size);
	void read_dec_buffer(const byte *new_buffer, const dword new_size);
	bool write_cmp_file(const char *filename);
	bool write_dec_file(const char *filename);
	void write_cmp_buffer(byte *new_buffer);
	void write_dec_buffer(byte *new_buffer);
	dword get_cmp_size(void);
	dword get_dec_size(void);
};

#endif // __PRS__
