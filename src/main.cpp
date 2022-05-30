#include <fstream>
#include <string>


#include "Compiler.h"

void ui_handler(int argc, char* argv[])
{
	if (argc < 2)	
	{
		throw std::string("Error: less arguments. Use this pattern:\n ./AssemblyCompiler input_file\n");
	}
	
	if (argc > 2)
	{
		throw std::string("Error: too many arguments. Use this pattern:\n ./AssemblyCompiler input_file\n");
	}
	
	std::ifstream file;
	file.open(argv[1]);
	
	if (file.fail())
	{
		throw std::string("Error: incorrect input file name or directory.\n");
	}

	std::stringstream sstream;
	sstream << file.rdbuf();	
	file.close();	
	
	Compiler compiler(sstream);
	
	compiler.compile();
}


int main(int argc, char *argv[])
{
		try
		{
			ui_handler(argc, argv);
		}
		catch(const std::string& report)
		{
			std::cerr << "\033[31m" << report << "\033[0m\n";
			return 1;
		}
		
		return 0;
}