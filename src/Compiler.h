#ifndef INCLUDECOMPILER
#define INCLUDECOMPILER

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <regex>

typedef unsigned char dtype;

class Compiler
{
	public:
		//Fields
	
		//Methods
		Compiler(std::stringstream&);
				
		void compile()&;
		
	private:
		//Fields
		
		const size_t MAX_REGISTERS = 12;
		std::vector<std::string> splited_by_lines;
		std::map<const std::string, dtype> labels;
		std::map<const std::string, dtype> constants;
		std::vector<dtype> bytes;
		size_t line_count = 0;
		
		
		//Methods
		
		void write_to_file()&;
		
		void get_labels_and_constants()&;
		void compile_lines()&;
		void compile_mov(std::stringstream&)&;
		void compile_computation(std::stringstream&)&;
		
		
		void compile_cmp(std::stringstream&)&;

};


#endif