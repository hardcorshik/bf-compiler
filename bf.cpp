#include <iostream>
#include <fstream>
#include <errno.h>
#include <string>
#include <cstring>

#define MEMORY 256

void usage();

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "usage: bf <source> <output>" << std::endl;
        return -1;
    }

    char cfilepath[50];
    char outfile[50];

    snprintf(cfilepath, sizeof(cfilepath), "%s.c", argv[1]);

    std::ifstream source(argv[1]);
    std::ofstream cfile(cfilepath);

    if (!source.good()) {
        std::cout << "error: failed to open source file: " << strerror(errno) << std::endl;
        return -1;
    }
    if (!cfile.good()) {
        std::cout << "error: failed to open temporary C file file: " << strerror(errno) << std::endl;
        return -1;
    }

    // setup C file
    cfile << "#include <stdio.h>" << std::endl
        << "int main(){char mem[" << MEMORY << "]={0};int ptr;";

    // The only compilation error that we need to account for
    int unbalancedLoops;

    char command;
    while (source.read(&command, 1)) {
        // Byte increment/decrement
        if (command == '+') cfile << "mem[ptr]++;";
        if (command == '-') cfile << "mem[ptr]--;";
        // Move pointer
        if (command == '>') cfile << "ptr++;";
        if (command == '<') cfile << "ptr--;";
        // Loops
        if (command == '[') {
            unbalancedLoops++;
            cfile << "while(mem[ptr]!=0){";
        }
        if (command == ']') {
            unbalancedLoops--;
            cfile << "}";
        }
        // IO
        if (command == '.') cfile << "putchar(mem[ptr]);";
        if (command == ',') cfile << "mem[ptr]=getchar();";
    }

    // end C file
    cfile << "printf(\"\\n\");return 0;}";
    cfile.close();

    if (unbalancedLoops) {
        remove(cfilepath);
        std::cout << "error: brackets are unbalanced" << std::endl;
        return -1;
    }

    char compileCommand[100];
    snprintf(compileCommand, sizeof(compileCommand), "gcc %s -o %s", cfilepath, argv[2]);
    system(compileCommand);
    remove(cfilepath);

    return 0;
}

void usage() {
    std::cout << "usage: bf source [args]" << std::endl << "args:"
        << "\n\t-h --help: print this"
        << "\n\t-o --out [file]: set output file"
        << "\n\t-c: don't remove compiled C file";
}