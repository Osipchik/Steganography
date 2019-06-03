#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <QString>
#include <QImage>

class Steganography
{
    int bit16_to_int(QString BIN);

    void SetSize(int size);

    QImage *image;

public:
    Steganography(QImage &image);

    QString GetSize();
    int GetMaxSize();

    void encode(QString message);
    QString decode();
};


#endif // STEGANOGRAPHY_H
