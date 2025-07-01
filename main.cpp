#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "encryptors/Encryptor.hpp"
#include "encryptors/AES_Encryptor.hpp"
#include "encryptors/ChaCha20_Encryptor.hpp"
#include "encryptors/Kyber_Encryptor.hpp"
#include "utils/CLIParser.hpp"
#include "utils/fileio.hpp"
#include "utils\ConsoleManager.hpp"

void print_art() {
    std::cout << R"(

 ____                  _   _ _     _      
|  _ \  ___  ___ _ __ | | | (_) __| | ___ 
| | | |/ _ \/ _ \ '_ \| |_| | |/ _` |/ _ \
| |_| |  __/  __/ |_) |  _  | | (_| |  __/
|____/ \___|\___| .__/|_| |_|_|\__,_|\___|
                |_|                       

)" << "\n";
}

int main(int argc, char* argv[]) {
    ConsoleManager::set_font(L"Consolas", 14);

    ConsoleManager::set_green_text();
    print_art();
    ConsoleManager::reset_color();

    CLIParser parser;
    if (!parser.parse(argc, argv)) {
        parser.print_help();
        return 1;
    }

    std::vector<uint8_t> data;
    if (!read_file(parser.input_file, data)) {
        std::cerr << "[!] Failed to read input file: " << parser.input_file << std::endl;
        return 1;
    }

    std::vector<std::unique_ptr<Encryptor>> pipeline;
    for (const auto& name : parser.algorithms) {
        if (name == "aes") pipeline.push_back(std::make_unique<AES_Encryptor>());
        else if (name == "chacha20") pipeline.push_back(std::make_unique<ChaCha20_Encryptor>());
        else if (name == "kyber") pipeline.push_back(std::make_unique<Kyber_Encryptor>());
        else {
            std::cerr << "[!] Unknown algorithm: " << name << std::endl;
            return 1;
        }
    }

    for (auto& encryptor : pipeline) {
        encryptor->get_data();
    }

    if (parser.mode == "encrypt") {
        for (auto& encryptor : pipeline) {
            encryptor->invoke(data);
        }
    }
    else if (parser.mode == "decrypt") {
        for (auto it = pipeline.rbegin(); it != pipeline.rend(); ++it) {
            (*it)->invoke_decrypt(data);
        }
    }
    else {
        std::cerr << "[!] Unknown mode: " << parser.mode << std::endl;
        return 1;
    }

    if (!write_file(parser.output_file, data)) {
        std::cerr << "[!] Failed to write output file: " << parser.output_file << std::endl;
        return 1;
    }

    std::cout << "[*] Operation completed successfully: " << parser.output_file << std::endl;
    return 0;
}