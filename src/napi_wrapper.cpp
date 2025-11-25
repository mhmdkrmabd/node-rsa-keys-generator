#include <napi.h>
#include "platform_utils.h"
#include "keyring.h"
#include "rsa_generator.h"

using namespace KeysGen;

// Generate RSA keys and return the public key
Napi::Value GenerateKeys(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        // serviceName is required (first parameter)
        if (info.Length() < 1 || !info[0].IsString()) {
            Napi::TypeError::New(env, "serviceName (string) is required as first parameter")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        std::string serviceName = info[0].As<Napi::String>().Utf8Value();

        // keyLength is optional (second parameter)
        int keyLength = 2048; // default
        if (info.Length() > 1 && info[1].IsNumber()) {
            keyLength = info[1].As<Napi::Number>().Int32Value();
        } else {
            keyLength = PlatformUtils::getRSAKeyLength();
        }

        // Replicate the exact Python logic
        std::optional<KeyPair> keys;

        if (PlatformUtils::getPlatform() == Platform::Windows) {
            // Windows always uses 1024 due to issue #105
            keys = RSAGenerator::getOrGenerateKeys(serviceName, 1024);
        } else {
            try {
                keys = RSAGenerator::getOrGenerateKeys(serviceName, keyLength);
            } catch (...) {
                // Fall back to 1024 if initial attempt fails
                try {
                    keys = RSAGenerator::getOrGenerateKeys(serviceName, 1024);
                } catch (...) {
                    // Silent failure like Python version
                    return env.Null();
                }
            }
        }

        if (keys.has_value()) {
            return Napi::String::New(env, keys->publicKey);
        }

    } catch (...) {
        // Silent failure like Python version
    }

    return env.Null();
}

// Get the stored public key without generating new ones
Napi::Value GetPublicKey(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        // serviceName is required (first parameter)
        if (info.Length() < 1 || !info[0].IsString()) {
            Napi::TypeError::New(env, "serviceName (string) is required as first parameter")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!Keyring::isAvailable()) {
            return env.Null();
        }

        std::string serviceName = info[0].As<Napi::String>().Utf8Value();
        std::string publicKeyService = serviceName + "PublicKey";
        auto publicKey = Keyring::getPassword(publicKeyService, "key");
        if (publicKey.has_value()) {
            return Napi::String::New(env, publicKey.value());
        }
    } catch (...) {
        // Silent failure
    }

    return env.Null();
}

// Get the stored private key
Napi::Value GetPrivateKey(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        // serviceName is required (first parameter)
        if (info.Length() < 1 || !info[0].IsString()) {
            Napi::TypeError::New(env, "serviceName (string) is required as first parameter")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!Keyring::isAvailable()) {
            return env.Null();
        }

        std::string serviceName = info[0].As<Napi::String>().Utf8Value();
        std::string privateKeyService = serviceName + "PrivateKey";
        auto privateKey = Keyring::getPassword(privateKeyService, "key");
        if (privateKey.has_value()) {
            return Napi::String::New(env, privateKey.value());
        }
    } catch (...) {
        // Silent failure
    }

    return env.Null();
}

// Check if keyring is available
Napi::Value IsKeychainAvailable(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, Keyring::isAvailable());
}

// Get platform information
Napi::Value GetPlatform(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, PlatformUtils::getPlatformString());
}

// Clear stored keys from keychain
Napi::Value ClearKeys(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        if (!Keyring::isAvailable()) {
            return Napi::Boolean::New(env, false);
        }

        // For clearing, we'd need to implement deletion methods in Keyring class
        // For now, return false as this feature wasn't in the original Python version
        return Napi::Boolean::New(env, false);
    } catch (...) {
        return Napi::Boolean::New(env, false);
    }
}

// Force regenerate keys with specific length
Napi::Value RegenerateKeys(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    try {
        // serviceName is required (first parameter)
        if (info.Length() < 1 || !info[0].IsString()) {
            Napi::TypeError::New(env, "serviceName (string) is required as first parameter")
                .ThrowAsJavaScriptException();
            return env.Null();
        }

        std::string serviceName = info[0].As<Napi::String>().Utf8Value();

        // keyLength is optional (second parameter)
        int keyLength = 2048; // default
        if (info.Length() > 1 && info[1].IsNumber()) {
            keyLength = info[1].As<Napi::Number>().Int32Value();
        }

        // Generate new keys (not retrieve existing)
        auto keys = RSAGenerator::generateKeys(keyLength);
        if (keys.has_value()) {
            // Store in keyring
            if (Keyring::isAvailable()) {
                std::string publicKeyService = serviceName + "PublicKey";
                std::string privateKeyService = serviceName + "PrivateKey";
                Keyring::setPassword(publicKeyService, "key", keys->publicKey);
                Keyring::setPassword(privateKeyService, "key", keys->privateKey);
            }
            return Napi::String::New(env, keys->publicKey);
        }
    } catch (...) {
        // Silent failure
    }

    return env.Null();
}

// Initialize the module
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "generateKeys"),
                Napi::Function::New(env, GenerateKeys));
    exports.Set(Napi::String::New(env, "getPublicKey"),
                Napi::Function::New(env, GetPublicKey));
    exports.Set(Napi::String::New(env, "getPrivateKey"),
                Napi::Function::New(env, GetPrivateKey));
    exports.Set(Napi::String::New(env, "isKeychainAvailable"),
                Napi::Function::New(env, IsKeychainAvailable));
    exports.Set(Napi::String::New(env, "getPlatform"),
                Napi::Function::New(env, GetPlatform));
    exports.Set(Napi::String::New(env, "clearKeys"),
                Napi::Function::New(env, ClearKeys));
    exports.Set(Napi::String::New(env, "regenerateKeys"),
                Napi::Function::New(env, RegenerateKeys));

    return exports;
}

NODE_API_MODULE(keys_generator, Init)