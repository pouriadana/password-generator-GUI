#ifndef CRYPTOHELPER_H
#define CRYPTOHELPER_H

#include <QString>
#include <QByteArray>

class CryptoHelper {
public:
    static QByteArray generateRandomKey(int length);
    static QByteArray deriveKeyFromPassword(const QString &password, const QByteArray &salt, int keyLength);
    static QByteArray encryptAES(const QByteArray &data, const QByteArray &key, const QByteArray &iv);
    static QByteArray decryptAES(const QByteArray &data, const QByteArray &key, const QByteArray &iv);
};

#endif // CRYPTOHELPER_H
