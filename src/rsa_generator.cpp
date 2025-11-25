#include "rsa_generator.h"
#include "keyring.h"
#include "platform_utils.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <memory>

namespace KeysGen {

std::optional<KeyPair> RSAGenerator::getOrGenerateKeys(const std::string& serviceName, int keyLength) {
    // First try to retrieve existing keys from keyring
    auto existingKeys = retrieveKeysFromKeyring(serviceName);
    if (existingKeys.has_value()) {
        return existingKeys;
    }

    // If no existing keys, generate new ones
    auto newKeys = generateKeys(keyLength);
    if (newKeys.has_value()) {
        // Store in keyring
        storeKeysInKeyring(newKeys.value(), serviceName);
        return newKeys;
    }

    return std::nullopt;
}

std::optional<KeyPair> RSAGenerator::generateKeys(int keyLength) {
    // Create RSA key pair
    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
        EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr), EVP_PKEY_CTX_free);

    if (!ctx) {
        return std::nullopt;
    }

    if (EVP_PKEY_keygen_init(ctx.get()) <= 0) {
        return std::nullopt;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx.get(), keyLength) <= 0) {
        return std::nullopt;
    }

    EVP_PKEY* pkey = nullptr;
    if (EVP_PKEY_keygen(ctx.get(), &pkey) <= 0) {
        return std::nullopt;
    }

    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> keyPtr(pkey, EVP_PKEY_free);

    // Extract public key PEM
    std::unique_ptr<BIO, decltype(&BIO_free)> pubBio(BIO_new(BIO_s_mem()), BIO_free);
    if (!pubBio || PEM_write_bio_PUBKEY(pubBio.get(), keyPtr.get()) != 1) {
        return std::nullopt;
    }

    // Extract private key PEM
    std::unique_ptr<BIO, decltype(&BIO_free)> privBio(BIO_new(BIO_s_mem()), BIO_free);
    if (!privBio || PEM_write_bio_PrivateKey(privBio.get(), keyPtr.get(), nullptr, nullptr, 0, nullptr, nullptr) != 1) {
        return std::nullopt;
    }

    // Get the PEM strings
    char* pubData = nullptr;
    char* privData = nullptr;
    long pubLen = BIO_get_mem_data(pubBio.get(), &pubData);
    long privLen = BIO_get_mem_data(privBio.get(), &privData);

    if (pubLen <= 0 || privLen <= 0 || !pubData || !privData) {
        return std::nullopt;
    }

    KeyPair keys;
    keys.publicKey = std::string(pubData, pubLen);
    keys.privateKey = std::string(privData, privLen);

    return keys;
}

std::optional<KeyPair> RSAGenerator::retrieveKeysFromKeyring(const std::string& serviceName) {
    if (!Keyring::isAvailable()) {
        return std::nullopt;
    }

    std::string publicKeyService = serviceName + "PublicKey";
    std::string privateKeyService = serviceName + "PrivateKey";

    auto publicKey = Keyring::getPassword(publicKeyService, "key");
    auto privateKey = Keyring::getPassword(privateKeyService, "key");

    if (!publicKey.has_value() || !privateKey.has_value()) {
        return std::nullopt;
    }

    KeyPair keys;
    keys.publicKey = publicKey.value();
    keys.privateKey = privateKey.value();

    return keys;
}

bool RSAGenerator::storeKeysInKeyring(const KeyPair& keys, const std::string& serviceName) {
    if (!Keyring::isAvailable()) {
        return false;
    }

    std::string publicKeyService = serviceName + "PublicKey";
    std::string privateKeyService = serviceName + "PrivateKey";

    bool pubSuccess = Keyring::setPassword(publicKeyService, "key", keys.publicKey);
    bool privSuccess = Keyring::setPassword(privateKeyService, "key", keys.privateKey);

    return pubSuccess && privSuccess;
}

} // namespace KeysGen