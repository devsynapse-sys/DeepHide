#ifndef AES_ENCRYPTOR_HPP
#define AES_ENCRYPTOR_HPP

#include "Encryptor.hpp"
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <iostream>
#include <vector>
#include <string>

using namespace CryptoPP;

class AES_Encryptor : public Encryptor {
    SecByteBlock key;
    byte iv[AES::BLOCKSIZE];
public:
    void get_data() override {
        std::string password;
        std::cout << "[AES] Enter password (used as key): ";
        std::getline(std::cin, password);
        if (password.size() < AES::DEFAULT_KEYLENGTH) {
            password.append(AES::DEFAULT_KEYLENGTH - password.size(), '0');
        }
        key = SecByteBlock((const byte*)password.data(), AES::DEFAULT_KEYLENGTH);

        AutoSeededRandomPool prng;
        prng.GenerateBlock(iv, sizeof(iv));
    }

    void invoke(std::vector<uint8_t>& data) override {
        CBC_Mode<AES>::Encryption encryptor;
        encryptor.SetKeyWithIV(key, key.size(), iv);

        std::string cipher;
        StringSource ss(data.data(), data.size(), true,
            new StreamTransformationFilter(encryptor,
                new StringSink(cipher)
            )
        );

        data.assign(cipher.begin(), cipher.end());

        // prepend IV for decryption
        data.insert(data.begin(), iv, iv + sizeof(iv));
    }

    void invoke_decrypt(std::vector<uint8_t>& data) override {
        if (data.size() < AES::BLOCKSIZE) {
            std::cerr << "[AES] Error: data too small to contain IV" << std::endl;
            return;
        }
        // extract IV
        std::copy(data.begin(), data.begin() + AES::BLOCKSIZE, iv);
        data.erase(data.begin(), data.begin() + AES::BLOCKSIZE);

        CBC_Mode<AES>::Decryption decryptor;
        decryptor.SetKeyWithIV(key, key.size(), iv);

        std::string recovered;
        try {
            StringSource ss(data.data(), data.size(), true,
                new StreamTransformationFilter(decryptor,
                    new StringSink(recovered)
                )
            );
        }
        catch (const CryptoPP::Exception& e) {
            std::cerr << "[AES] Decryption failed: " << e.what() << std::endl;
            return;
        }

        data.assign(recovered.begin(), recovered.end());
    }

    std::string name() const override {
        return "AES";
    }
};

#endif