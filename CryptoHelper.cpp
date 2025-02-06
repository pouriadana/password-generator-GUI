#include "CryptoHelper.h"
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <QCryptographicHash>
#include <QDebug>

// Generate a random key of specified length
QByteArray CryptoHelper::generateRandomKey(int length) {
    QByteArray key(length, 0);
    RAND_bytes(reinterpret_cast<unsigned char *>(key.data()), length);
    return key;
}

// Derive a key from password using PBKDF2 with SHA-256
QByteArray CryptoHelper::deriveKeyFromPassword(const QString &password, const QByteArray &salt, int keyLength) {
    QByteArray derivedKey(keyLength, 0);
    PKCS5_PBKDF2_HMAC(password.toUtf8().constData(), password.size(),
                      reinterpret_cast<const unsigned char *>(salt.data()), salt.size(),
                      100000, EVP_sha256(),
                      keyLength, reinterpret_cast<unsigned char *>(derivedKey.data()));
    return derivedKey;
}

// Encrypt data using AES-256-CBC
QByteArray CryptoHelper::encryptAES(const QByteArray &data, const QByteArray &key, const QByteArray &iv) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    QByteArray encryptedData(data.size() + EVP_MAX_BLOCK_LENGTH, 0);
    int len = 0, totalLen = 0;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       reinterpret_cast<const unsigned char *>(key.data()),
                       reinterpret_cast<const unsigned char *>(iv.data()));

    EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(encryptedData.data()), &len,
                      reinterpret_cast<const unsigned char *>(data.data()), data.size());
    totalLen += len;

    EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(encryptedData.data()) + totalLen, &len);
    totalLen += len;

    EVP_CIPHER_CTX_free(ctx);
    encryptedData.resize(totalLen);
    return encryptedData;
}

// Decrypt data using AES-256-CBC
QByteArray CryptoHelper::decryptAES(const QByteArray &data, const QByteArray &key, const QByteArray &iv) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    QByteArray decryptedData(data.size(), 0);
    int len = 0, totalLen = 0;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       reinterpret_cast<const unsigned char *>(key.data()),
                       reinterpret_cast<const unsigned char *>(iv.data()));

    EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(decryptedData.data()), &len,
                      reinterpret_cast<const unsigned char *>(data.data()), data.size());
    totalLen += len;

    EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(decryptedData.data()) + totalLen, &len);
    totalLen += len;

    EVP_CIPHER_CTX_free(ctx);
    decryptedData.resize(totalLen);
    return decryptedData;
}
