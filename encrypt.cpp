#include <iostream>
#include <fstream>
#include <string>
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>
#include <cryptopp/osrng.h>
#include <cryptopp/gcm.h>
#include "encrypt.h"

using namespace CryptoPP;

// define a suitable IV size
// 12 bytes is common for GCM to avoid the need for counter wrapping considerations
const size_t IV_SIZE = 12;  

std::string EncryptString(const std::string& plaintext, const  std::string& password) {
    std::string salt, ciphertext, encoded;

    // generate a random salt
    AutoSeededRandomPool prng;
    CryptoPP::byte tempSalt[8];
    prng.GenerateBlock(tempSalt, sizeof(tempSalt));

    // derive key from password and salt
    SecByteBlock key(AES::DEFAULT_KEYLENGTH);
    PKCS5_PBKDF2_HMAC<SHA256> pbkdf;
    pbkdf.DeriveKey(key.data(), key.size(), 0x00, reinterpret_cast<const CryptoPP::byte*>(password.data()), password.size(), tempSalt, sizeof(tempSalt), 10000);

    // encrypt with aes-gcm
    CryptoPP::byte iv[IV_SIZE];
    prng.GenerateBlock(iv, sizeof(iv));

    GCM<AES>::Encryption enc;
    enc.SetKeyWithIV(key, key.size(), iv);

    StringSource ss(plaintext, true,
        new AuthenticatedEncryptionFilter(enc,
            new StringSink(ciphertext)));

    // encode salt, IV, and ciphertext for transport
    HexEncoder encoder;
    encoder.Attach(new StringSink(encoded));
    encoder.Put(tempSalt, sizeof(tempSalt));
    encoder.Put(iv, sizeof(iv));
    encoder.Put(reinterpret_cast<const CryptoPP::byte*>(ciphertext.data()), ciphertext.size());
    encoder.MessageEnd();

    return encoded;
}

std::string DecryptString(const std::string& encodedCiphertext, const std::string& password) {
    std::string decoded;

    // decode from Hex
    StringSource ss(encodedCiphertext, true, new HexDecoder(new StringSink(decoded)));

    // extract salt and IV
    CryptoPP::byte tempSalt[8], iv[IV_SIZE];
    memcpy(tempSalt, decoded.data(), sizeof(tempSalt));
    memcpy(iv, decoded.data() + sizeof(tempSalt), sizeof(iv));

    // derive key
    SecByteBlock key(AES::DEFAULT_KEYLENGTH);
    PKCS5_PBKDF2_HMAC<SHA256> pbkdf;
    pbkdf.DeriveKey(key.data(), key.size(), 0x00, reinterpret_cast<const CryptoPP::byte*>(password.data()), password.size(), tempSalt, sizeof(tempSalt), 10000);

    // decrypt with aes-gcm
    GCM<AES>::Decryption dec;
    dec.SetKeyWithIV(key, key.size(), iv);

    const CryptoPP::byte* ciphertext = reinterpret_cast<const CryptoPP::byte*>(decoded.data() + sizeof(tempSalt) + sizeof(iv));
    size_t ciphertextLen = decoded.size() - sizeof(tempSalt) - sizeof(iv);
    std::string decryptedtext;

    try {
        AuthenticatedDecryptionFilter df(dec, new StringSink(decryptedtext));
        df.Put(ciphertext, ciphertextLen);
        df.MessageEnd();
    } catch (const CryptoPP::Exception& e) {
        std::cerr << "Decryption failed: " << e.what() << std::endl;
        return "";
    }

    return decryptedtext;
}