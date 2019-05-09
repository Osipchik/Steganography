#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <QString>
#include <QByteArray>
#include <QBitArray>
#include <QVector>
#include <QImage>

class Steganography
{
public:
    QVector<QByteArray> textToByteArr(const QString message);
    QByteArray ToByteArr(const QBitArray &bits);

    QBitArray ToBitArray(const QByteArray &bytes);
    QBitArray ToBitArray(const int num);
    int BitToInt(const QBitArray &bits);

    void SetSize(int size, QImage &image);
    QString GetSize(QImage &image);
    void encode(QString message, QImage &image);
    QString decode(QImage &image, int mSize);
};

#endif // STEGANOGRAPHY_H
