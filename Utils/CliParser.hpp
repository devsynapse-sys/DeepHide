#ifndef CLIPARSER_HPP
#define CLIPARSER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

class CLIParser {
public:
    std::string input_file;
    std::string output_file;
    std::string mode;
    std::vector<std::string> algorithms;

    void print_help() {
        std::cout << "Usage:\n"
            << "  program -i inputfile -o outputfile -m [encrypt|decrypt] -a [aes,chacha20,kyber,...]\n"
            << "Example:\n"
            << "  program -i plain.txt -o encrypted.bin -m encrypt -a aes,chacha20,kyber\n";
    }

    bool parse(int argc, char* argv[]) {
        if (argc < 9) return false;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-i" && i + 1 < argc) {
                input_file = argv[++i];
            }
            else if (arg == "-o" && i + 1 < argc) {
                output_file = argv[++i];
            }
            else if (arg == "-m" && i + 1 < argc) {
                mode = argv[++i];
                std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);
            }
            else if (arg == "-a" && i + 1 < argc) {
                std::string algs = argv[++i];
                split(algs, ',', algorithms);
                for (auto& s : algorithms) std::transform(s.begin(), s.end(), s.begin(), ::tolower);
            }
            else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                return false;
            }
        }
        if (input_file.empty() || output_file.empty() || mode.empty() || algorithms.empty()) {
            return false;
        }
        return true;
    }

private:
    void split(const std::string& s, char delim, std::vector<std::string>& elems) {
        size_t start = 0, end = 0;
        while ((end = s.find(delim, start)) != std::string::npos) {
            elems.push_back(s.substr(start, end - start));
            start = end + 1;
        }
        elems.push_back(s.substr(start));
    }
};

#endif