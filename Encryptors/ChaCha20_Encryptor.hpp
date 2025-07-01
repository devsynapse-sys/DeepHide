#ifndef CHACHA20_ENCRYPTOR_HPP
#define CHACHA20_ENCRYPTOR_HPP

#include <cryptopp/chachapoly.h>
#include <cryptopp/osrng.h>
#include <cryptopp/filters.h>
#include <iostream>
#include <vector>
#include <string>

using namespace CryptoPP;

class ChaCha20_Encryptor : public Encryptor {
    SecByteBlock key;
    byte nonce[12]; // 96 бит Ч как требует ChaChaPoly1305

public:
    void get_data() override {
        std::string password;
        std::cout << "[ChaChaPoly1305] Enter password (used as key): ";
        std::getline(std::cin, password);

        if (password.size() < 32)
            password.append(32 - password.size(), '0');

        key = SecByteBlock((const byte*)password.data(), 32);

        AutoSeededRandomPool prng;
        prng.GenerateBlock(nonce, sizeof(nonce));
    }

    void invoke(std::vector<uint8_t>& data) override {
        ChaCha20Poly1305::Encryption enc;
        enc.SetKeyWithIV(key, key.size(), nonce, sizeof(nonce));

        std::string cipher;
        try {
            StringSource ss(data.data(), data.size(), true,
                new AuthenticatedEncryptionFilter(enc,
                    new StringSink(cipher)
                )
            );
        }
        catch (const Exception& e) {
            std::cerr << "[ChaChaPoly1305] Encryption failed: " << e.what() << std::endl;
            return;
        }

        // prepend nonce
        std::vector<uint8_t> result(nonce, nonce + sizeof(nonce));
        result.insert(result.end(), cipher.begin(), cipher.end());
        data.swap(result);
    }

    void invoke_decrypt(std::vector<uint8_t>& data) override {
        if (data.size() < sizeof(nonce)) {
            std::cerr << "[ChaChaPoly1305] Error: data too small to contain nonce" << std::endl;
            return;
        }

        std::memcpy(nonce, data.data(), sizeof(nonce));
        data.erase(data.begin(), data.begin() + sizeof(nonce));

        ChaCha20Poly1305::Decryption dec;
        dec.SetKeyWithIV(key, key.size(), nonce, sizeof(nonce));

        std::string recovered;
        try {
            StringSource ss(data.data(), data.size(), true,
                new AuthenticatedDecryptionFilter(dec,
                    new StringSink(recovered)
                )
            );
        }
        catch (const Exception& e) {
            std::cerr << "[ChaChaPoly1305] Decryption failed: " << e.what() << std::endl;
            return;
        }

        data.assign(recovered.begin(), recovered.end());
    }

    std::string name() const override {
        return "ChaChaPoly1305";
    }
};


#endif // CHACHA20_ENCRYPTOR_HPP
