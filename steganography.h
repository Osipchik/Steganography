#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <QString>
#include <QImage>

class Steganography
{
    QString int_to_bit(const int num, int size = 8);
    int bit16_to_int(QString BIN);

    void SetSize(QImage &image, int size);

public:
    QString GetSize(QImage &image);
    int GetMaxSize(QImage &image);

    void encode(QString message, QImage &image);
    QString decode(QImage &image);
};


#endif // STEGANOGRAPHY_H
