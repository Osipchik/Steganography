#include "steganography.h"

#include <QDebug>

QVector<QByteArray> Steganography::textToByteArr(const QString message)
{
    QVector<QByteArray> byte_vec;
    foreach(QString ch, message)
    {
        byte_vec.push_back(ch.toUtf8());
    }
    return byte_vec;
}

QByteArray Steganography::ToByteArr(const QBitArray &bits)
{
    QByteArray bytes;
    bytes.resize(bits.count()/8);
    bytes.fill(0);

    for(int b=0; b < bits.count(); ++b)
        bytes[b/8] = (bytes.at(b/8) | ((bits[b] ? 1 : 0)<<(b%8)));

    return bytes;
}

QBitArray Steganography::ToBitArray(const QByteArray &bytes)
{
    QBitArray bits;
    int size = bytes.size() * 8;
    bits.resize(size);

    int gpos = 0;
    foreach(char ch, bytes)
    {
        unsigned char uc = ch;
        for (int i = 0; i < 8; ++i)
        {
            bits[gpos++] = (uc & 0x01);
            uc = uc >> 1;
        }
    }
    return bits;
}

QBitArray Steganography::ToBitArray(const int num)
{
    QString BIN = QString("%1").arg(num, 8, 2, QChar('0'));
    QBitArray bit(8);

    for(int i = 0; i < 8; i++)
    {
        if(BIN[i] == '1') bit[i] = 1;
        else bit[i] = 0;
    }
    return bit;
}

int Steganography::BitToInt(const QBitArray &bits)
{
    int n = 0, total = bits.size();
    for(int i = 0; i < total; i++) { n = n * 10 + bits[i]; }
    return QString::number(n).toInt(nullptr, 2);
}

void Steganography::SetSize(int size, QImage &image)
{
    QString mSize = QString("%1").arg(size, 0, 36);

    int height = image.height();
    QColor pixel;
    QVector<QByteArray> byte_size = textToByteArr(mSize);
    for(int y = 0, mLength = mSize.length(); y < height; y++)
    {
        pixel = image.pixel(0, y);
        if(y < mLength)
        {
            QBitArray bit_size = ToBitArray(byte_size[y]);

            QBitArray red = ToBitArray(pixel.red());
            QBitArray green = ToBitArray(pixel.green());
            QBitArray blue = ToBitArray(pixel.blue());

            red[6] = bit_size[0];
            red[7] = bit_size[1];

            green[5] = bit_size[2];
            green[6] = bit_size[3];
            green[7] = bit_size[4];

            blue[5] = bit_size[5];
            blue[6] = bit_size[6];
            blue[7] = bit_size[7];

            image.setPixel(0, y, qRgb(BitToInt(red), BitToInt(green), BitToInt(blue)));
            //image.setPixel(0, y, qRgb(255, 255, 255));
        }
        else
        {
            QBitArray red = ToBitArray(pixel.red());
            QBitArray green = ToBitArray(pixel.green());
            QBitArray blue = ToBitArray(pixel.blue());

            red[7] = 0;
            green[7] = 0;
            blue[7] = 0;

            image.setPixel(0, y, qRgb(BitToInt(red), BitToInt(green), BitToInt(blue)));
            //image.setPixel(0, y, qRgb(0, 0, 0));
        }
    }
}

QString Steganography::GetSize(QImage &image)
{
    int height = image.height();
    QColor pixel;
    QString size = "";

    for(int y = 0; y < height; y++)
    {
        pixel = image.pixel(0, y);
        QBitArray red = ToBitArray(pixel.red());
        QBitArray green = ToBitArray(pixel.green());
        QBitArray blue = ToBitArray(pixel.blue());

        if(red[7] == 0 && green[7] == 0 && blue[7] == 0) break;

        QBitArray bit_size(8);
        bit_size[0] = red[6];
        bit_size[1] = red[7];

        bit_size[2] = green[5];
        bit_size[3] = green[6];
        bit_size[4] = green[7];

        bit_size[5] = blue[5];
        bit_size[6] = blue[6];
        bit_size[7] = blue[7];

        size += ToByteArr(bit_size);
    }
    return QString::number(size.toInt(nullptr, 36), 10);
}

void Steganography::encode(QString message, QImage &image)
{
    int height = image.height(), width = image.width(), mSize = message.length();
    QColor pixel;
    QVector<QByteArray> byte_mess = textToByteArr(message);
    for(int y = 0, count = 0; y < height; y++)
    {
        if(count == mSize) { break; }
        for(int x = 1; x < width; x++)
        {
            if(count == mSize) { break; }

            pixel = image.pixel(x, y);
            QBitArray bit_mess = ToBitArray(byte_mess[count++]);

            QBitArray red = ToBitArray(pixel.red());
            QBitArray green = ToBitArray(pixel.green());
            QBitArray blue = ToBitArray(pixel.blue());

            red[6] = bit_mess[0];
            red[7] = bit_mess[1];

            green[5] = bit_mess[2];
            green[6] = bit_mess[3];
            green[7] = bit_mess[4];

            blue[5] = bit_mess[5];
            blue[6] = bit_mess[6];
            blue[7] = bit_mess[7];

            image.setPixel(x, y, qRgb(BitToInt(red), BitToInt(green), BitToInt(blue)));
            //image.setPixel(x, y, qRgb(255, 255, 255));
         }
    }
    SetSize(mSize, image);
}

QString Steganography::decode(QImage &image, int mSize)
{
    int height = image.height(), width = image.width();
    QColor pixel;
    QString message = "";
    for(int y = 0; y < height; y++)
    {
        for(int x = 1; x < width; x++, mSize--)
        {
            if(mSize == 0) return message;

            pixel = image.pixel(x, y);
            QBitArray bit_mess(8);

            QBitArray red = ToBitArray(pixel.red());
            QBitArray green = ToBitArray(pixel.green());
            QBitArray blue = ToBitArray(pixel.blue());

            bit_mess[0] = red[6];
            bit_mess[1] = red[7];

            bit_mess[2] = green[5];
            bit_mess[3] = green[6];
            bit_mess[4] = green[7];

            bit_mess[5] = blue[5];
            bit_mess[6] = blue[6];
            bit_mess[7] = blue[7];

            message += ToByteArr(bit_mess);
        }
    }

    return QString();
}


