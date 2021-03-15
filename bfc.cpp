#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>

#define DEFAULT_MEMORY 256
#define CC "gcc"

void usage();

int main(int argc, char** argv) {
   if (argc < 2) {
       std::cout << "usage: bfc [source] <args>" << std::endl;
       return -1;
   }

    int memory = DEFAULT_MEMORY;
    int removeCode = 1;
    std::string cfilepath = argv[1];
    cfilepath.append(".c");

    std::string output = "a.out";

    char opt;
    while ((opt = getopt(argc, argv, "hco:m:")) != -1) {
        switch (opt) {
            case 'h':
                usage();
                break;
            case 'c':
                removeCode = 0;
                break;
            case 'o':
                output = optarg;
                break;
            case 'm':
                memory = atoi(optarg);
                break;
            case '?':
                break;
        }
    }

    std::ifstream source(argv[optind]);
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
        << "int main(){char mem[" << memory << "]={0};int ptr;";

    // The only compilation error that we need to account for
    int unbalancedLoops = 0;

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
        if (removeCode) remove(cfilepath.c_str());
        std::cout << "error: brackets are unbalanced" << std::endl;
        return -1;
    }

    std::string compileCommand = CC;
    compileCommand += " " + cfilepath + " -o " + output;

    system(compileCommand.c_str());
    if (removeCode) remove(cfilepath.c_str());

    return 0;
}

// Not used
void usage() {
    std::cout << "usage: bfc [source] <args>" << std::endl << "args:"
        << "\n\t-h: print this"
        << "\n\t-c: don't remove compiled C file"
        << "\n\t-o [file]: set output file"
        << "\n\t-m [memory]: sets memory for the brainfuck program"
        << std::endl;
    exit(0);
}