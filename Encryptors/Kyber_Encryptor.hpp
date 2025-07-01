#ifndef KYBER_ENCRYPTOR_HPP
#define KYBER_ENCRYPTOR_HPP

#include "Encryptor.hpp"
#include <oqs/oqs.h>
#include <iostream>
#include <vector>
#include <cstring>

class Kyber_Encryptor : public Encryptor {
    std::vector<uint8_t> pubkey;
    std::vector<uint8_t> privkey;
    std::vector<uint8_t> ciphertext;
    std::vector<uint8_t> shared_secret_enc;
    std::vector<uint8_t> shared_secret_dec;

public:
    void get_data() override {
        OQS_KEM* kem = OQS_KEM_new("Kyber512");
        if (!kem) {
            std::cerr << "[Kyber] Failed to initialize KEM" << std::endl;
            exit(1);
        }

        pubkey.resize(kem->length_public_key);
        privkey.resize(kem->length_secret_key);

        if (OQS_KEM_keypair(kem, pubkey.data(), privkey.data()) != OQS_SUCCESS) {
            std::cerr << "[Kyber] Keypair generation failed" << std::endl;
            OQS_KEM_free(kem);
            exit(1);
        }
        OQS_KEM_free(kem);
    }

    void invoke(std::vector<uint8_t>& data) override {
        OQS_KEM* kem = OQS_KEM_new("Kyber512");
        if (!kem) {
            std::cerr << "[Kyber] Failed to initialize KEM" << std::endl;
            exit(1);
        }

        ciphertext.resize(kem->length_ciphertext);
        shared_secret_enc.resize(kem->length_shared_secret);

        if (OQS_KEM_encaps(kem, ciphertext.data(), shared_secret_enc.data(), pubkey.data()) != OQS_SUCCESS) {
            std::cerr << "[Kyber] Encapsulation failed" << std::endl;
            OQS_KEM_free(kem);
            exit(1);
        }

        // XOR data с shared_secret_enc
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] ^= shared_secret_enc[i % shared_secret_enc.size()];
        }

        // добавим ciphertext в начало данных, чтобы использовать при дешифровании
        data.insert(data.begin(), ciphertext.begin(), ciphertext.end());

        OQS_KEM_free(kem);
    }

    void invoke_decrypt(std::vector<uint8_t>& data) override {
        OQS_KEM* kem = OQS_KEM_new("Kyber512");
        if (!kem) {
            std::cerr << "[Kyber] Failed to initialize KEM" << std::endl;
            exit(1);
        }

        if (data.size() < kem->length_ciphertext) {
            std::cerr << "[Kyber] Ciphertext length too small" << std::endl;
            OQS_KEM_free(kem);
            return;
        }

        // выделяем ciphertext из начала данных
        ciphertext.assign(data.begin(), data.begin() + kem->length_ciphertext);
        data.erase(data.begin(), data.begin() + kem->length_ciphertext);

        shared_secret_dec.resize(kem->length_shared_secret);

        if (OQS_KEM_decaps(kem, shared_secret_dec.data(), ciphertext.data(), privkey.data()) != OQS_SUCCESS) {
            std::cerr << "[Kyber] Decapsulation failed" << std::endl;
            OQS_KEM_free(kem);
            return;
        }

        // XOR обратно
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] ^= shared_secret_dec[i % shared_secret_dec.size()];
        }

        OQS_KEM_free(kem);
    }

    std::string name() const override {
        return "Kyber512";
    }
};

#endif