#include "single_pass_search.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <string>

int main(int argc, char* argv[])
{
	std::ifstream infile;

    //argv[1]: file path, argv[2]: pattern string
	if (argc != 3)
	{
	    std::cout << "Usage: <executable> <file-path> <pattern-string>" << std::endl;
	    return -1;
	}

	if (*argv[1] != 0)
        infile.open(argv[1], std::ios::in);

	if ( infile.is_open() )
	{
		typedef std::istreambuf_iterator<char> stream_it_t;
		std::string pattern(argv[2]);

		stream_it_t it, end_it;
		long count = -1;

		do //now count the pattern occurrences (see also: Syntax description)
		{
			it = boost::single_pass_search(stream_it_t(infile), end_it, pattern.begin(), pattern.end());

			++count;

		} while (it != end_it);

		infile.close();

		std::cout << "Found " << count << " occurrences of \"" << pattern << "\"" << std::endl;
	}
	else
		std::cout << "Failed to open input file" << std::endl;

	getchar();

	return 0;
}
