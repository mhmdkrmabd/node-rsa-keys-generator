#pragma once

#include <string>
#include <optional>

namespace KeysGen {

struct KeyPair {
    std::string publicKey;
    std::string privateKey;
};

class RSAGenerator {
public:
    static std::optional<KeyPair> generateKeys(int keyLength);
    static std::optional<KeyPair> getOrGenerateKeys(const std::string& serviceName, int keyLength);

private:
    static std::optional<KeyPair> retrieveKeysFromKeyring(const std::string& serviceName);
    static bool storeKeysInKeyring(const KeyPair& keys, const std::string& serviceName);
    static std::optional<std::string> rsaKeyToPem(void* key, bool isPrivate);
};

} // namespace KeysGen