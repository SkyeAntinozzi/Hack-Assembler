AssembleAll.bat - Assembles all .asm files in this directory.
CompareAll.bat - Compares all xxx_Compare.hack files with assembled xxx.hack files.
DeleteAllHackOutput.bat - Deletes all assembled xxx.hack files in this directory.
GitSample.bat - Assembles the Add.hack file, shows the contents of original source file and the assembled
file, and deletes the generated assembled file.

How to Run External Unit Tests
------------------------------
1. Run AssembleAll.bat to assemble all .asm files
2. Run CompareAll.bat to compare assembled files to comparison files
	2a. If there are no differences found, the tests were succesful
3. Run DeleteAllHackOutput.bat to delete all assembled files