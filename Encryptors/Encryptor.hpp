#ifndef ENCRYPTOR_HPP
#define ENCRYPTOR_HPP

#include <vector>
#include <string>

class Encryptor {
public:
    virtual void get_data() = 0;
    virtual void invoke(std::vector<uint8_t>& data) = 0;
    virtual void invoke_decrypt(std::vector<uint8_t>& data) = 0;
    virtual std::string name() const = 0;
    virtual ~Encryptor() = default;
};

#endif