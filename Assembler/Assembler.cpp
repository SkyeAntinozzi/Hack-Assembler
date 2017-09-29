/* 
 * Author: Skye Antinozzi
 * Created for The Elements of Computing Systems by Noam Nissan and Shimon Schocken
 * Translates 16-bit HACK assembly language into 16-bit HACK machine code.
 */
#include "Assembler.h"

string Assembler::ASM_FILE_NAME;

// Pre-defined and user-defined symbols representing physical addresses
Assembler::CodeMap Assembler::symbolMap({
		{"SP","0000000000000000"},     {"LCL","0000000000000001"},  {"ARG","0000000000000010"},
		{"THIS","0000000000000011"},   {"THAT","0000000000000100"}, {"R0","0000000000000000"},
		{"R1","0000000000000001"},     {"R2","0000000000000010"},   {"R3","0000000000000011"},
		{"R4","0000000000000100"},     {"R5","0000000000000101"},   {"R6","0000000000000110"},
		{"R7","0000000000000111"},     {"R8","0000000000001000"},   {"R9","0000000000001001"},
		{"R10","0000000000001010"},    {"R11","0000000000001011"},  {"R12","00000000001100"},
		{"R13","0000000000001101"},    {"R14","0000000000001110"},  {"R15","0000000000001111"},
		{"SCREEN","0100000000000000"}, {"KBD","0110000000000000"}
});

void Assembler::start(const string & asmFileName){

	// Global reference
	ASM_FILE_NAME = asmFileName;

	std::ifstream fileIn(asmFileName, std::ios::binary);
	if(!fileIn){
		std::cerr << "Failed to open \"" << asmFileName << "\"" << std::endl;
		exit(0);
	}
	std::ofstream fileOut(removeExtension(asmFileName) + ".hack", std::ios::binary);
	string content;

	// Get size of file
	fileIn.seekg(0, std::ios::end);
	long size = fileIn.tellg();
	content.resize(size);
	fileIn.seekg(0, std::ios::beg);

	// Read file
	fileIn.read(&content[0], size);
	fileIn.close();

	// Prune the assembly file and generate symbol table
	string partialObjCode = firstPass(content, asmFileName);
	string finishedObjCode = secondPass(partialObjCode);

	// Write file
	fileOut.write(&finishedObjCode[0], finishedObjCode.size());
	fileOut.close();

	std::cout << "\"" << asmFileName << "\" successfully assembled into \"" 
		     << removeExtension(asmFileName) + ".hack\".\n";
}

string Assembler::firstPass(const string & asmCode, const string & asmFileName){

	string line, partialObjCode;
	unsigned int asmLineNum = 0, romInstAddr = 0;
	std::istringstream ss(asmCode);

	while(getline(ss, line)){

		++asmLineNum;
		size_t commentLoc;

		// If there is a comment
		if( (commentLoc = line.find("//")) != string::npos)
			// If line is a single comment, skip the line
			if(commentLoc == 0)
				continue;
			// If comment is embedded in line, remove it
			else
				line.resize(commentLoc);
		
		// Remove whitespace
		line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

		const size_t EQUALS_INDEX = line.find('='),
			        SEMICOLON_INDEX = line.find(';');
		
		// If line is C-Instruction (dest=comp;jump, dest=comp, comp;jump)
		if( (EQUALS_INDEX != string::npos) || (SEMICOLON_INDEX != string::npos) ){
			processCInstruction(line, partialObjCode, EQUALS_INDEX, SEMICOLON_INDEX, asmLineNum);
			++romInstAddr;
		}
		//@Xxx
		else if(line[0] == '@'){
			processAInstruction(line, partialObjCode, asmLineNum);
			++romInstAddr;
		}
		//(Xxx)
		else if(line[0] == '(')
			processLabel(line, romInstAddr);
		
		// Blank-Line
		else if(line.empty()){ }

		// Unsupported
		else{
			reportInvalidCommand(line, asmLineNum);
		}
	}

	return partialObjCode;
}

string Assembler::secondPass(const string & partialObjCode){
	string line, objCode;
	istringstream ss(partialObjCode);

	int ramAddress = 0x10;

	while(getline(ss, line)){
	
		// Already translated
		if( line[0] == '1' || line[0] == '0' ){
			objCode += line;
		}
		// Symbol to be translated
		else{			
			unordered_map<string, string>::const_iterator hash = symbolMap.find(line);

			// If symbol was found
			if(hash != symbolMap.end()){
				objCode += hash->second;
			} else{
				symbolMap.insert({line, bitset<16>(ramAddress++).to_string()});
				objCode += symbolMap.at(line);
			}
		}

		objCode += '\n';
	}

	// Delete last newline
	objCode.erase(objCode.length() - 1, 1);

	return objCode;
}

void Assembler::reportMalformedCInstruction(const string & intendedField, const string & attemptedField, int lineNumber){
	std::cerr << "\nMalformed C-Instruction" << " : Line " << lineNumber << " : " << ASM_FILE_NAME << "\n->\t\""
		<< attemptedField << "\" is not a recognized " << intendedField
		<< " field." << std::endl;

	exit(0);
}

void Assembler::reportInvalidSymbol(const string & symbol, int invalidCharIndex){
	string pointer("\t");
	for(int i = 0; i < invalidCharIndex; ++i)
		pointer += " ";

	pointer += "^";

	std::cerr << "Invalid Symbol Declaration : Index " << invalidCharIndex << " : " 
		<< ASM_FILE_NAME << "\n->\t" << symbol << '\n' << pointer << std::endl;

	exit(0);
}

void Assembler::reportInvalidCommand(const string & line, int lineNumber){
	std::cerr << "Invalid Command : Line " << lineNumber << " : " << ASM_FILE_NAME << "\n->\t\""
		     << line << "\"" << " is an unrecognized command.\n";
}

string Assembler::removeExtension(const string & fileName){

	string noExt(fileName);
	return noExt.erase(fileName.find_last_of('.'), string::npos);
}

void Assembler::verifySymbol(const string & sym){

	for(int i = 0, size = sym.size(); i < size; ++i)
		if( !isalnum(sym[i]) && (sym[i] != '_') && (sym[i] != '.') && (sym[i] != '$') && (sym[i] != ':') )
			reportInvalidSymbol(sym, i);
}

void Assembler::verifyLabel(const string & label){
	if(isdigit(label[0]))
		reportInvalidSymbol(label, 0);

	verifySymbol(label);
}

inline
void Assembler::processCInstruction(const string & inst, string & partialObjCode, 
							 const int equalsIndex, const int semicolonIndex,
	                               const int asmLineNum){

	string comp, dest, jump;

	// dest=comp;jump OR dest=comp
	if(equalsIndex != string::npos){

		// dest=comp;jump
		if(semicolonIndex != string::npos){
			comp = inst.substr(equalsIndex + 1, semicolonIndex - equalsIndex - 1);
			dest = inst.substr(0, equalsIndex);
			jump = inst.substr(semicolonIndex + 1);
		}
		// dest=comp
		else {
			comp = inst.substr(equalsIndex + 1);
			dest = inst.substr(0, equalsIndex);
			jump = "null";
		}
	}
	// comp;jump
	else{
		comp = inst.substr(0, semicolonIndex);
		dest = "null";
		jump = inst.substr(semicolonIndex + 1);
	}

	// Translate fields to binary
	try{ partialObjCode += COMP.at(comp); } 
	catch(const out_of_range & ex){ reportMalformedCInstruction("COMP", comp, asmLineNum); }

	try{ partialObjCode += DEST.at(dest); } 
	catch(const out_of_range & ex){ reportMalformedCInstruction("DEST", dest, asmLineNum); }

	try{ partialObjCode += JUMP.at(jump); } 
	catch(const out_of_range & ex){ reportMalformedCInstruction("JUMP", jump, asmLineNum); }

	partialObjCode += '\n';
}

inline
void Assembler::processAInstruction(const string & inst, string & partialObjCode, const int asmLineNum){

	string symbol = inst.substr(1);

	// If first char is a digit, may be a constant
	if(isdigit(symbol[0])){
		// Check that each char is a digit
		for(int i = 1, size = symbol.size(); i < size; ++i)
			if(!isdigit(symbol[i]))
				reportInvalidSymbol(symbol, i);

		partialObjCode += bitset<16>(stoi(symbol)).to_string();
	} 
	// Char is not a digit, verify and translate later
	else{
		verifySymbol(symbol);
		partialObjCode += symbol;
	}
			
	partialObjCode += '\n';
}

inline
void Assembler::processLabel(const string & label, const int romInstAddr){
	size_t rightParenIndex = label.find(')');

	// Label has a ')'
	if(rightParenIndex != string::npos){
		string symbol = label.substr(1, rightParenIndex - 1);

		// Add symbol if not in map
		if(symbolMap.find(symbol) == symbolMap.end()){
			verifyLabel(symbol);
			symbolMap.insert({symbol, bitset<16>(romInstAddr).to_string()});
		}
	}
}
