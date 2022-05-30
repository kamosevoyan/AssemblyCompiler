#include "Compiler.h"

Compiler::Compiler(std::stringstream& sstream)
{
	std::string temp;
	
	while (std::getline(sstream, temp))
	{
		if (temp.length() == 0)
			continue;
		
		this->splited_by_lines.push_back(temp);
	}	
}

void Compiler::compile()&
{
	this->get_labels_and_constants();
	this->compile_lines();
	this->bytes.push_back(0xFF);
	
	this->write_to_file();
}

void Compiler::write_to_file()&
{
	std::ofstream file;
	file.open("output.bin", std::ios::binary);
	
	for (auto byte:this->bytes)
	{
		file << byte;
	}
	
	file.close();
}

void Compiler::get_labels_and_constants()&
{
	std::smatch smatch;
	std::regex label_regex(R"(\s*([a-zA-Z]+):\s*)");
	std::regex constant_regex(R"(\s*const\s+([a-zA-Z]+)\s+(\d+))");
	
	int label_const_count = 0;
		
	for (int i = 0; i < this->splited_by_lines.size(); ++i)
	{
		if (std::regex_match(this->splited_by_lines[i], smatch, label_regex))
		{
			this->labels[smatch[1].str()] = (i-label_const_count)*4;//TODO: check it later
			++label_const_count;
		}
		else
		if (std::regex_match(this->splited_by_lines[i], smatch, constant_regex))
		{			
			this->constants[smatch[1].str()] = std::stoi(smatch[2]);
			++label_const_count;
		}	
	}
	
}

void Compiler::compile_lines()&
{
	for (auto line: this->splited_by_lines)
	{
		
		std::stringstream sstream(line);
		std::string token;
		
		sstream >> token;
				
		if (token == "mov")
		{
			this->compile_mov(sstream);
		}
		else
		if ((token == "add") | (token == "sub") | (token == "and") | (token == "or") | (token == "xor") |
			(token == "mul") | (token == "div")	| (token == "shr") | (token == "shl"))
		{
			this->compile_computation(sstream);
		}
		if (token == "cmp")
		{
			this->compile_cmp(sstream);
		}

	++this->line_count;
	}
}

void Compiler::compile_computation(std::stringstream& sstream)&
{
	std::string token;
	std::regex 			type_regex(R"((add)|(sub)|(and)|(or)|(xor)|(mul)|(div)|(shr)|(shl))");
	std::regex 	operand1_regex(R"(r(\d+)|([a-zA-Z]+)|(\d+))");
	std::regex 	operand2_regex(R"(r(\d+)|([a-zA-Z]+)|(\d+))");
	std::regex destination_regex(R"(r(\d+))");
	std::smatch smatch;
	
	
	sstream.clear();
	sstream.seekg(0);
	sstream >> token;
	
	this->bytes.push_back(0);
	int line_number = this->bytes.size() - 1;
	
	if (std::regex_match(token, smatch, type_regex))
	{
		if (smatch[1].str().length() != 0)
		{
			this->bytes[line_number] += 0;
		}
		else
		if (smatch[2].str().length() != 0)
		{
			this->bytes[line_number] += 1;
		}	
		else
		if (smatch[3].str().length() != 0)
		{
			this->bytes[line_number] += 2;
		}
		else
		if (smatch[4].str().length() != 0)
		{
			this->bytes[line_number] += 3;
		}
		else
		if (smatch[5].str().length() != 0)
		{
			this->bytes[line_number] += 4;
		}
		else
		if (smatch[6].str().length() != 0)
		{
			this->bytes[line_number] += 5;
		}
		else
		if (smatch[7].str().length() != 0)
		{
			this->bytes[line_number] += 6;
		}
		else
		if (smatch[8].str().length() != 0)
		{
			this->bytes[line_number] += 7;
		}
		else
		if (smatch[9].str().length() != 0)
		{
			this->bytes[line_number] += 8;
		}
		else
		{
			throw std::string("Error: unknown comparing argument at line ") + std::to_string(this->line_count);
		}
	}
	
	
	
	
	if (sstream.tellg() == std::string::npos)
	{
		throw std::string("Error: less arguments at line ") + std::to_string(this->line_count);
	}
	
	sstream >> token;
	
	
	if (std::regex_match(token, smatch, operand1_regex))
	{
		if (smatch[1].str().length() != 0)
		{
			
			if (std::stoi(smatch[1]) > this->MAX_REGISTERS)
			{
				throw std::string("Error: unknown register ") + smatch[1].str();
			}
			
			//this->bytes[line_number] = 0;
			this->bytes.push_back(std::stoi(smatch[1]));			
		}
		else
		if (smatch[2].str().length() != 0)
		{
			
			if (this->constants.find(smatch[2].str()) == this->constants.end())
			{
				throw std::string("Error: unknown constant ") + smatch[2].str() + std::string(" at line ") +  std::to_string(this->line_count);
			}
			
			this->bytes[line_number] += 128;
			this->bytes.push_back(this->constants[smatch[2]]);
		}
		else
		if (smatch[3].str().length() != 0)
		{
			this->bytes[line_number] += 128;
			this->bytes.push_back(std::stoi(smatch[3].str())); // TODO: check integer range.
		}
		else
		{
			throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
		}
	}
	else
	{
		throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
	}

	if (sstream.tellg() == std::string::npos)
	{
		throw std::string("Error: less arguments at line ") + std::to_string(this->line_count);
	}
	
	sstream >> token;

	if (std::regex_match(token, smatch, operand2_regex))
	{
		if (smatch[1].str().length() != 0)
		{
			
			if (std::stoi(smatch[1]) > this->MAX_REGISTERS)
			{
				throw std::string("Error: unknown register ") + smatch[1].str();
			}
			
			this->bytes.push_back(std::stoi(smatch[1]));			
		}
		else
		if (smatch[2].str().length() != 0)
		{
			
			if (this->constants.find(smatch[2].str()) == this->constants.end())
			{
				throw std::string("Error: unknown constant ") + smatch[2].str() + std::string(" at line ") +  std::to_string(this->line_count);
			}
			
			this->bytes[line_number] += 64;
			this->bytes.push_back(this->constants[smatch[2]]);
		}
		else
		if (smatch[3].str().length() != 0)
		{
			this->bytes[line_number] += 64;
			this->bytes.push_back(std::stoi(smatch[3].str())); // TODO: check integer range.
		}
		else
		{
			throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
		}
	}
	else
	{
		throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
	}

	if (sstream.tellg() == std::string::npos)
	{
		throw std::string("Error: less arguments at line ") + std::to_string(this->line_count);
	}
	
	sstream >> token;
	
	if (std::regex_match(token, smatch, destination_regex))
	{
		if (std::stoi(smatch[1]) > this->MAX_REGISTERS)
		{
			throw std::string("Error: unknown register ") + smatch[1].str();
		}
		
		this->bytes.push_back(std::stoi(smatch[1]));			
	}
	else
	{
		throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
	}
	
	if (sstream.tellg() != std::string::npos)
	{
		throw std::string("Error: too many arguments at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
	}
	
	
}

void Compiler::compile_mov(std::stringstream& sstream)&
{
	std::string token;
	std::regex operand1_regex(R"(r(\d+))");
	std::regex operand2_regex(R"(r(\d+)|([a-zA-Z]+)|(\d+))");
	std::smatch smatch;
	
	dtype arg1;
	
	if (sstream.tellg() == std::string::npos)
	{
		throw std::string("Error: less arguments at line ") + std::to_string(this->line_count);
	}
	
	sstream >> token;
		
	if (std::regex_match(token, smatch, operand1_regex))
	{
		if (std::stoi(smatch[1]) > this->MAX_REGISTERS)
		{
			throw std::string("Error: unknown register ") + smatch[1].str();
		}
		
		arg1 = std::stoi(smatch[1]);
	}
	else
	{
		throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
	}


	if (sstream.tellg() == std::string::npos)
	{
		throw std::string("Error: less arguments at line ") + std::to_string(this->line_count);
	}
	
	sstream >> token;
	
	if (std::regex_match(token, smatch, operand2_regex))
	{
		if (smatch[1].str().length() != 0)
		{
			if (std::stoi(smatch[1]) > this->MAX_REGISTERS)
			{
				throw std::string("Error: unknown register ") + smatch[1].str();
			}
			
			this->bytes.push_back(64);
			this->bytes.push_back(std::stoi(smatch[1]));
			this->bytes.push_back(0);
			this->bytes.push_back(arg1);
			
		}
		else
		if (smatch[2].str().length() != 0)
		{
			
			if (this->constants.find(smatch[2].str()) == this->constants.end())
			{
				throw std::string("Error: unknown constant ") + smatch[2].str() + std::string(" at line ") +  std::to_string(this->line_count);
			}
			
			this->bytes.push_back(128 + 64);
			this->bytes.push_back(this->constants[smatch[2]]);
			this->bytes.push_back(0);
			this->bytes.push_back(arg1);
		}
		else
		if (smatch[3].str().length() != 0)
		{
			this->bytes.push_back(128 + 64);
			this->bytes.push_back(std::stoi(smatch[3].str())); // TODO: check integer range.
			this->bytes.push_back(0);
			this->bytes.push_back(arg1);
		}
		else
		{
			throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
		}
	}
	else
	{
		throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
	}

	if (sstream.tellg() != std::string::npos)
	{
		throw std::string("Error: too many arguments at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
	}
	
	
}

void Compiler::compile_cmp(std::stringstream& sstream)&
{
	std::string token;
	std::regex 	type_regex(R"((eq)|(neq)|(le)|(leq)|(ge)|(geq))");
	std::regex 	operand1_regex(R"(r(\d+)|([a-zA-Z]+)|(\d+))");
	std::regex 	operand2_regex(R"(r(\d+)|([a-zA-Z]+)|(\d+))");
	std::regex destination_regex(R"(r(\d+)|([a-zA-Z]+))");
	std::smatch smatch;
	
	if (sstream.tellg() == std::string::npos)
	{
		throw std::string("Error: less arguments at line ") + std::to_string(this->line_count);
	}
	
	sstream >> token;
	
	this->bytes.push_back(32);
	int line_number = this->bytes.size() - 1;
	
	if (std::regex_match(token, smatch, type_regex))
	{
		if (smatch[1].str().length() != 0)
		{
			this->bytes[line_number] += 0;
		}
		else
		if (smatch[2].str().length() != 0)
		{
			this->bytes[line_number] += 1;
		}	
		else
		if (smatch[3].str().length() != 0)
		{
			this->bytes[line_number] += 2;
		}
		else
		if (smatch[4].str().length() != 0)
		{
			this->bytes[line_number] += 3;
		}
		else
		if (smatch[5].str().length() != 0)
		{
			this->bytes[line_number] += 4;
		}
		else
		if (smatch[6].str().length() != 0)
		{
			this->bytes[line_number] += 5;
		}
		else
		{
			throw std::string("Error: unknown comparing argument at line ") + std::to_string(this->line_count);
		}
	}
	else
	{
		throw std::string("Error: unknown comparing argument at line ") + std::to_string(this->line_count);
	}
	
	if (sstream.tellg() == std::string::npos)
	{
		throw std::string("Error: less arguments at line ") + std::to_string(this->line_count);
	}
	
	sstream >> token;
	
		
	if (std::regex_match(token, smatch, operand1_regex))
	{
		if (smatch[1].str().length() != 0)
		{
			
			if (std::stoi(smatch[1]) > this->MAX_REGISTERS)
			{
				throw std::string("Error: unknown register ") + smatch[1].str();
			}
			
			this->bytes.push_back(std::stoi(smatch[1]));			
		}
		else
		if (smatch[2].str().length() != 0)
		{
			
			if (this->constants.find(smatch[2].str()) == this->constants.end())
			{
				throw std::string("Error: unknown constant ") + smatch[2].str() + std::string(" at line ") +  std::to_string(this->line_count);
			}
			
			this->bytes[line_number] += 128;
			this->bytes.push_back(this->constants[smatch[2]]);
		}
		else
		if (smatch[3].str().length() != 0)
		{
			this->bytes[line_number] += 128;
			this->bytes.push_back(std::stoi(smatch[3].str())); // TODO: check integer range.
		}
		else
		{
			throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
		}
	}
	else
	{
		throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
	}
	
	if (sstream.tellg() == std::string::npos)
	{
		throw std::string("Error: less arguments at line ") + std::to_string(this->line_count);
	}
	
	sstream >> token;
	
	if (std::regex_match(token, smatch, operand2_regex))
	{
		if (smatch[1].str().length() != 0)
		{
			
			if (std::stoi(smatch[1]) > this->MAX_REGISTERS)
			{
				throw std::string("Error: unknown register ") + smatch[1].str();
			}
			
			this->bytes.push_back(std::stoi(smatch[1]));			
		}
		else
		if (smatch[2].str().length() != 0)
		{
			
			if (this->constants.find(smatch[2].str()) == this->constants.end())
			{
				throw std::string("Error: unknown constant ") + smatch[2].str() + std::string(" at line ") +  std::to_string(this->line_count);
			}
			
			this->bytes[line_number] += 64;
			this->bytes.push_back(this->constants[smatch[2]]);
		}
		else
		if (smatch[3].str().length() != 0)
		{
			this->bytes[line_number] += 64;
			this->bytes.push_back(std::stoi(smatch[3].str())); // TODO: check integer range.
		}
		else
		{
			throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
		}
	}
	else
	{
		throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
	}

	if (sstream.tellg() == std::string::npos)
	{
		throw std::string("Error: less arguments at line ") + std::to_string(this->line_count);
	}
	
	sstream >> token;
	
	if (std::regex_match(token, smatch, destination_regex))
	{
		if (smatch[1].str().length() != 0)
		{
			if (std::stoi(smatch[1]) > this->MAX_REGISTERS)
			{
				throw std::string("Error: unknown register ") + smatch[1].str();
			}
		
			this->bytes.push_back(std::stoi(smatch[1]));			
		}
		else
		if (smatch[2].str().length() != 0)
		{
			if (this->labels.find(smatch[2].str()) == this->labels.end())
			{
				throw std::string("Error: unknown label ") + smatch[2].str() + std::string(" at line ") +  std::to_string(this->line_count);
			}
			this->bytes.push_back(this->labels[smatch[2]]);
		}
	}
	else
	{
		throw std::string("Error: incorrect sintax at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
	}
	
	if (sstream.tellg() != std::string::npos)
	{
		throw std::string("Error: too many arguments at line ") + std::to_string(this->line_count) + std::string(": ") + sstream.str();
	}

	
}
