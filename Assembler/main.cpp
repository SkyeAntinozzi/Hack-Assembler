/* 
 * Author: Skye Antinozzi
 * Created for The Elements of Computing Systems by Noam Nissan and Shimon Schocken
 * Translates the 16-bit HACK assembly language into 16-bit HACK machine code.
 */

#include<fstream>
#include<string>
#include<sstream>
#include<iostream>
#include<algorithm>
#include<unordered_map>
#include<bitset>

#include "Assembler.h"

using namespace std;

int main(int argc, char* argv[]){

	if(argc <= 1){
		cerr << "Please provide an assembly file name (MyFile.asm)!\n";
		exit(0);
	}

	Assembler::start(argv[1]);
}