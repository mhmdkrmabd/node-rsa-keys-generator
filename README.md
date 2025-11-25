# Node RSA Keys Generator

A cross-platform Node.js native module for generating RSA key pairs and securely storing them in the OS keychain. Built with C++ and N-API for high performance and seamless JavaScript/TypeScript integration.

## Overview

The Node RSA Keys Generator provides secure RSA key generation and management for credential encryption. Keys are automatically stored in the operating system's native keychain (Windows Credential Manager on Windows, GNOME Keyring on Linux, macOS Keychain on macOS) for secure, persistent storage.

## Installation

```bash
npm install @mhmdkrmabd/node-rsa-keys-generator
```

The native module will be automatically compiled during installation using `node-gyp`.

### System Requirements

**All Platforms:**

- Node.js >= 16.0.0
- C++17 compatible compiler
- OpenSSL development libraries

**Ubuntu/Debian:**

```bash
sudo apt-get update
sudo apt-get install build-essential libssl-dev libsecret-1-dev pkg-config
```

**CentOS/RHEL:**

```bash
sudo yum install gcc gcc-c++ openssl-devel libsecret-devel pkgconfig
```

**Fedora:**

```bash
sudo dnf install gcc gcc-c++ openssl-devel libsecret-devel pkgconfig
```

**Windows:**

- Visual Studio 2019 or later with C++ tools
- OpenSSL is bundled with the package (no separate installation required)

**macOS:**

```bash
brew install openssl
```

## Quick Start

### JavaScript

```javascript
const keysGenerator = require('node-rsa-keys-generator');

// serviceName is REQUIRED and used as prefix for keychain storage
// Keys stored as: {serviceName}PublicKey and {serviceName}PrivateKey

// Generate or retrieve RSA keys for your application
const publicKey = keysGenerator.generateKeys('MyApp');
console.log('Public Key:', publicKey);

// Check if keychain is available
if (keysGenerator.isKeychainAvailable()) {
    console.log('Keychain is available for secure storage');
}

// Get current platform
console.log('Platform:', keysGenerator.getPlatform());
```

### TypeScript

```typescript
import * as keysGenerator from 'node-rsa-keys-generator';

// serviceName is required as first parameter
const publicKey: string | null = keysGenerator.generateKeys('MyApp', 2048);

if (publicKey) {
    console.log('Successfully generated keys');
}
```

## API Reference

### `generateKeys(serviceName, keyLength?)`

Generates a new RSA key pair or retrieves existing keys from the keychain.

**Parameters:**

- `serviceName` (string, **required**): Service name prefix for keychain storage. Keys will be stored as `{serviceName}PublicKey` and `{serviceName}PrivateKey`.
- `keyLength` (number, optional): RSA key length in bits. Defaults to value from `RSA_KEY_LENGTH` environment variable or 2048.

**Returns:** `string | null` - The public key in PEM format, or null if generation fails.

**Example:**

```javascript
// Basic usage with required service name
const publicKey = keysGenerator.generateKeys('MyApp');

// With custom key length
const publicKey2048 = keysGenerator.generateKeys('MyApp', 2048);

// Different service names for different applications
const app1Key = keysGenerator.generateKeys('Application1');
const app2Key = keysGenerator.generateKeys('Application2', 4096);
```

---

### `getPublicKey(serviceName)`

Retrieves the stored public key from the keychain without generating new keys.

**Parameters:**

- `serviceName` (string, **required**): Service name prefix for keychain storage.

**Returns:** `string | null` - The stored public key in PEM format, or null if not found.

---

### `getPrivateKey(serviceName)`

Retrieves the stored private key from the keychain.

**Parameters:**

- `serviceName` (string, **required**): Service name prefix for keychain storage.

**Returns:** `string | null` - The stored private key in PEM format, or null if not found.

---

### `regenerateKeys(serviceName, keyLength?)`

Forces generation of new RSA keys, replacing any existing keys in the keychain.

**Parameters:**

- `serviceName` (string, **required**): Service name prefix for keychain storage.
- `keyLength` (number, optional): RSA key length in bits. Defaults to 2048.

**Returns:** `string | null` - The new public key in PEM format, or null if generation fails.

---

### `isKeychainAvailable()`

Checks if the system keychain is available for secure storage.

**Returns:** `boolean` - True if keychain is available, false otherwise.

---

### `getPlatform()`

Returns the current operating system platform.

**Returns:** `"Windows" | "Linux" | "macOS" | "Unknown"` - Platform identifier string.
