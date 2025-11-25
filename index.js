// Ensure OpenSSL DLLs are in place on Windows before loading native module
if (process.platform === 'win32') {
    const fs = require('fs');
    const path = require('path');

    const dllSource = path.join(__dirname, 'deps', 'openssl', 'bin');
    const dllTarget = path.join(__dirname, 'build', 'Release');
    const dlls = ['libssl-3-x64.dll', 'libcrypto-3-x64.dll'];

    if (fs.existsSync(dllSource) && fs.existsSync(dllTarget)) {
        dlls.forEach(dll => {
            const src = path.join(dllSource, dll);
            const dest = path.join(dllTarget, dll);

            if (fs.existsSync(src) && !fs.existsSync(dest)) {
                try {
                    fs.copyFileSync(src, dest);
                } catch (err) {
                    // Silent fail - DLL might already be there or in use
                }
            }
        });
    }
}

const keysGenerator = require('./build/Release/keys_generator.node');

/**
 * Generate or retrieve RSA keys for credential encryption.
 * The serviceName is used as a prefix for keychain storage: {serviceName}PublicKey and {serviceName}PrivateKey.
 *
 * @param {string} serviceName - Service name prefix for keychain storage (required)
 * @param {number} [keyLength] - RSA key length in bits (default: from RSA_KEY_LENGTH env var or 2048)
 * @returns {string|null} - The public key in PEM format, or null if generation fails
 */
function generateKeys(serviceName, keyLength) {
    return keysGenerator.generateKeys(serviceName, keyLength);
}

/**
 * Get the stored public key from the system keychain without generating new keys.
 * The serviceName is used to construct the keychain service name: {serviceName}PublicKey.
 *
 * @param {string} serviceName - Service name prefix for keychain storage (required)
 * @returns {string|null} - The stored public key in PEM format, or null if not found
 */
function getPublicKey(serviceName) {
    return keysGenerator.getPublicKey(serviceName);
}

/**
 * Get the stored private key from the system keychain.
 * The serviceName is used to construct the keychain service name: {serviceName}PrivateKey.
 *
 * @param {string} serviceName - Service name prefix for keychain storage (required)
 * @returns {string|null} - The stored private key in PEM format, or null if not found
 */
function getPrivateKey(serviceName) {
    return keysGenerator.getPrivateKey(serviceName);
}

/**
 * Check if the system keychain is available.
 *
 * @returns {boolean} - True if keychain is available, false otherwise
 */
function isKeychainAvailable() {
    return keysGenerator.isKeychainAvailable();
}

/**
 * Get the current platform name.
 *
 * @returns {string} - Platform name ("Windows", "Linux", or "Unknown")
 */
function getPlatform() {
    return keysGenerator.getPlatform();
}

/**
 * Force regeneration of keys with the specified length.
 * This will generate new keys and store them in the keychain.
 * The serviceName is used as a prefix for keychain storage: {serviceName}PublicKey and {serviceName}PrivateKey.
 *
 * @param {string} serviceName - Service name prefix for keychain storage (required)
 * @param {number} [keyLength] - RSA key length in bits (default: 2048)
 * @returns {string|null} - The new public key in PEM format, or null if generation fails
 */
function regenerateKeys(serviceName, keyLength) {
    return keysGenerator.regenerateKeys(serviceName, keyLength);
}

/**
 * Clear stored keys from the system keychain.
 * Note: This functionality is not implemented in the current version.
 *
 * @returns {boolean} - False (not implemented)
 */
function clearKeys() {
    return keysGenerator.clearKeys();
}

module.exports = {
    generateKeys,
    getPublicKey,
    getPrivateKey,
    isKeychainAvailable,
    getPlatform,
    regenerateKeys,
    clearKeys
};