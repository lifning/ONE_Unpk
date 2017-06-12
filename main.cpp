#include <cstdlib>
#include <iostream>

#include "one.h"
#include "one60.h"

int main(int argc, char *argv[])
{
	if(argc < 2)
		return(0);
	one60 one_file;
    return(!(one_file.open_file(argv[1]) && one_file.extract_archive()));
}
