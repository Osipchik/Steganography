#include "steganography.h"

#include <QtMath>



QString Steganography::int_to_bit(const int num, int size)
{
    QString bin = QString("%1").arg(num, size, 2, QChar('0'));
    QString bit(size);
    int i = 0;
    foreach(QChar ch, bin)
    {
        bit[i++] = (ch == '1') ? '1' : '0';
    }
    return bit;
}

int Steganography::bit16_to_int(QString BIN)
{
    QString byte_high = BIN.mid(0, 8);
    QString byte_low = BIN.mid(8, 8);

    int X1 = byte_high.toInt(nullptr, 2);
    int count = 7 - BIN.indexOf('1');
    if(X1) X1 += 256 * pow(2, count) - pow(2, count);

    return X1 + byte_low.toInt(nullptr, 2);
}

void Steganography::SetSize(int size)
{
    QString mSize = QString("%1").arg(size, 0, 36);
    int height = image->height();

    QString bit_size;
    QColor pixel;
    QString red;
    QString green;
    QString blue;
    for(int y = 0; y < height; y++)
    {
        pixel = image->pixel(0, y);
        if(y < mSize.length())
        {
            bit_size = QString("%1").arg(mSize[y].unicode(), 8, 2, QChar('0'));
            red = int_to_bit(pixel.red());
            green = int_to_bit(pixel.green());
            blue = int_to_bit(pixel.blue());

            red[6] = bit_size[0];
            red[7] = bit_size[1];

            green[5] = bit_size[2];
            green[6] = bit_size[3];
            green[7] = bit_size[4];

            blue[5] = bit_size[5];
            blue[6] = bit_size[6];
            blue[7] = bit_size[7];

            image->setPixel(0, y, qRgb(red.toInt(nullptr, 2), green.toInt(nullptr, 2), blue.toInt(nullptr, 2)));
        }
        else
        {
            red = int_to_bit(pixel.red());
            green = int_to_bit(pixel.green());
            blue = int_to_bit(pixel.blue());

            red[6] = '0';
            red[7] = '0';

            green[5] = '0';
            green[6] = '0';
            green[7] = '0';

            blue[5] = '0';
            blue[6] = '0';
            blue[7] = '0';

            image->setPixel(0, y, qRgb(red.toInt(nullptr, 2), green.toInt(nullptr, 2), blue.toInt(nullptr, 2)));
            break;
        }
    }
}

Steganography::Steganography(QImage &image)
{
    this->image = &image;
}

QString Steganography::GetSize()
{
    int height = image->height();
    QString size = "", zero_str = "00000000";

    QString bit_size;
    QColor pixel;
    QString red;
    QString green;
    QString blue;
    int count = 0;
    for(int y = 0; y < height; y++)
    {
        pixel = image->pixel(0, y);
        red = int_to_bit(pixel.red(), 8);
        green = int_to_bit(pixel.green(), 8);
        blue = int_to_bit(pixel.blue(), 8);

        bit_size[0] = red[6];
        bit_size[1] = red[7];

        bit_size[2] = green[5];
        bit_size[3] = green[6];
        bit_size[4] = green[7];

        bit_size[5] = blue[5];
        bit_size[6] = blue[6];
        bit_size[7] = blue[7];

        count = y;
        if(bit_size == zero_str) break;
        size += QChar(bit_size.toInt(nullptr, 2));
    }
    return QString::number(size.toInt(nullptr, 36));
}

int Steganography::GetMaxSize()
{
    int width = image->width()-1, height = image->height();
    while(width%2 != 0) { width--; }
    return ((width * height) - height) / 2;
}

void Steganography::encode(QString message)
{
    int height = image->height(), width = image->width()-1, mSize = message.length();
    while(width%2 != 0) { width--; }
    SetSize(mSize);

    QColor pixel;
    QString bit_mess(16);
    QString red;
    QString green;
    QString blue;
    for(int y = 0, count = 0; y < height; y++)
    {
        if(count >= mSize) break;
        for(int x = 1; x < width; x++)
        {
            if(count >= mSize) break;

            bit_mess = QString("%1").arg(message[count++].unicode(), 16, 2, QChar('0'));

            pixel = image->pixel(x, y);
            red = int_to_bit(pixel.red());
            green = int_to_bit(pixel.green());
            blue = int_to_bit(pixel.blue());

            red[6] = bit_mess[0];
            red[7] = bit_mess[1];

            green[5] = bit_mess[2];
            green[6] = bit_mess[3];
            green[7] = bit_mess[4];

            blue[5] = bit_mess[5];
            blue[6] = bit_mess[6];
            blue[7] = bit_mess[7];

            image->setPixel(x, y, qRgb(red.toInt(nullptr, 2), green.toInt(nullptr, 2), blue.toInt(nullptr, 2)));

            pixel = image->pixel(++x, y);
            red = int_to_bit(pixel.red());
            green = int_to_bit(pixel.green());
            blue = int_to_bit(pixel.blue());

            red[6] = bit_mess[8];
            red[7] = bit_mess[9];

            green[5] = bit_mess[10];
            green[6] = bit_mess[11];
            green[7] = bit_mess[12];

            blue[5] = bit_mess[13];
            blue[6] = bit_mess[14];
            blue[7] = bit_mess[15];

            image->setPixel(x, y, qRgb(red.toInt(nullptr, 2), green.toInt(nullptr, 2), blue.toInt(nullptr, 2)));
        }
    }
}

QString Steganography::decode()
{
    int height = image->height(), width = image->width()-1, mSize = GetSize().toInt();
    while(width%2 != 0) { width--; }

    QColor pixel;
    QString bit_mess(16), message = "";
    QString red;
    QString green;
    QString blue;
    for(int y = 0, count = 0; y < height; y++)
    {
        if(count == mSize) break;
        for(int x = 1; x < width; x++)
        {
            if(count == mSize) break;
            count++;
            pixel = image->pixel(x, y);
            red = int_to_bit(pixel.red());
            green = int_to_bit(pixel.green());
            blue = int_to_bit(pixel.blue());

            bit_mess[0] = red[6];
            bit_mess[1] = red[7];

            bit_mess[2] = green[5];
            bit_mess[3] = green[6];
            bit_mess[4] = green[7];

            bit_mess[5] = blue[5];
            bit_mess[6] = blue[6];
            bit_mess[7] = blue[7];

            pixel = image->pixel(++x, y);
            red = int_to_bit(pixel.red());
            green = int_to_bit(pixel.green());
            blue = int_to_bit(pixel.blue());

            bit_mess[8] = red[6];
            bit_mess[9] = red[7];

            bit_mess[10] = green[5];
            bit_mess[11] = green[6];
            bit_mess[12] = green[7];

            bit_mess[13] = blue[5];
            bit_mess[14] = blue[6];
            bit_mess[15] = blue[7];

            message += QChar(bit16_to_int(bit_mess));
        }
    }
    return message;
}
