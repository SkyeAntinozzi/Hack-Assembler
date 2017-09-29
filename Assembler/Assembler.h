/* 
 * Author: Skye Antinozzi
 * Created for The Elements of Computing Systems by Noam Nissan and Shimon Schocken
 * Translates 16-bit HACK assembly language into 16-bit HACK machine code.
 */
#pragma once

#include<iostream>
#include<string>
#include<fstream>
#include<unordered_map>
#include<sstream>
#include<algorithm>
#include<bitset>

using std::out_of_range;
using std::istringstream;
using std::string;
using std::bitset;
using std::unordered_map;

namespace Assembler{

	typedef std::unordered_map<string, string> CodeMap;

	extern string ASM_FILE_NAME;
	extern CodeMap symbolMap;

	// COMP codes
	const CodeMap COMP ({
		{"0",   "1110101010"}, {"1",   "1110111111"}, {"-1",  "1110111010"},
		{"D",   "1110001100"}, {"A",   "1110110000"}, {"M",   "1111110000"},
		{"!D",  "1110001101"}, {"!A",  "1110110001"}, {"!M",  "1111110001"}, 
		{"-D",  "1110001111"}, {"-A",  "1110110011"}, {"-M",  "1111110011"}, 
		{"D+1", "1110011111"}, {"A+1", "1110110111"}, {"M+1", "1111110111"},
		{"D-1", "1110001110"}, {"A-1", "1110110010"}, {"M-1", "1111110010"},
		{"D+A", "1110000010"}, {"D+M", "1111000010"},
		{"D-A", "1110010011"}, {"D-M", "1111010011"},
		{"A-D", "1110000111"}, {"M-D", "1111000111"}, 
		{"D&A", "1110000000"}, {"D&M", "1111000000"},
		{"D|A", "1110010101"}, {"D|M", "1111010101"} 
	});

	// DEST codes
	const CodeMap DEST ({
		{"null",        "000"}, {"M",   "001"}, {"D", "010"},
		{"MD",          "011"}, {"A",   "100"}, {"AM", "101"},
		{"AD",          "110"}, {"AMD", "111"}
	});

	// JUMP codes
	const CodeMap JUMP ({
		{"null",        "000"},  {"JGT", "001"}, {"JEQ", "010"},
		{"JGE",         "011"},  {"JLT", "100"}, {"JNE", "101"},
		{"JLE",         "110"},  {"JMP", "111"}
	});
	
	/*
	 * Starts the parsing of the assembly file denoted by the passed asmFileName.
      */
	void start(const string & asmFileName);

	/*
	 * First pass parses all instructions and builds the symbol table with user-defined symbols.
      */
	string firstPass(const string & asmCode, const string & asmFileName);

	/*
	 * Second pass parses all symbol instructions that were not translated during the first pass.
      */
	string secondPass(const string & partialObjCode);

	/*
	 * Prints detailed information about an incorrect C-Instruction to the standard error stream and terminates the assembler.
      */
	void reportMalformedCInstruction(const string & intendedField, const string & attemptedField, int lineNumber);

	/*
	 * Prints detailed information about an incorrect symbol declaration to the standard error stream and terminates the assembler.
      */
	void reportInvalidSymbol(const string & symbol, int invalidCharIndex);

	/*
	 * Prints information about an incorrect command to the standard error stream and terminates the assembler.
      */
	void reportInvalidCommand(const string & line, int lineNumber);

	/*
	 * Removes an extension (.txt, .asm, .vm, etc.) from the passed file name.
      */
	inline
	string removeExtension(const string & fileName);

	/*
	 * Verifies that the passed symbol abides by naming conventions defined in the book.
      */
	inline
	void verifySymbol(const string & sym);

	/*
	 * Verifies that the passed label abides by naming conventions defined in the book.
      */
	inline
	void verifyLabel(const string & label);

	/*
	 * Processes the provided C-Instruction and appends its binary translation to the assembly output string.
      */
	inline
	void processCInstruction(const string & inst, string & partialObjCode, 
					     const int equalsIndex, const int semicolonIndex,
						const int asmLineNum);

	/*
	 * Processes the provided A-Instruction and appends its binary translation to the assembly output string.
      */
	inline
	void processAInstruction(const string & inst, string & partialObjCode, const int asmLineNum);

	/*
	 * Processes the provided label by generating a physical address for it and adding it to the symbol table.
      */
	inline
	void processLabel(const string & label, const int romInstAddr);
}