#include "steganography.h"
#include <QtMath>

// в качестве контейнера для байт используется QString
// т.к нет смысла подключать библиотеку QByteArray для аналогичных действий

int Steganography::bit16_to_int(QString BIN)
{
    QString byte_high = BIN.mid(0, 8);
    QString byte_low = BIN.mid(8, 8);

    int X1 = byte_high.toInt(Q_NULLPTR, 2);
    int count = 7 - BIN.indexOf('1');
    if (X1) X1 += 255 * pow(2, count);
    return X1 + byte_low.toInt(Q_NULLPTR, 2);
}

void Steganography::SetSize(int size)
{
    // перевод числа в 36-ричную сс,чтобы занимать меньше пикселей
    QString mSize = QString("%1").arg(size, 0, 36);
    int height = image->height();

    QString bit_size;
    QColor pixel;
    QString red, green, blue;
    for (int y = 0; y < height; y++)
    {
        pixel = image->pixel(0, y);
        red = QString("%1").arg(pixel.red(), 8, 2, QChar('0'));
        green = QString("%1").arg(pixel.green(), 8, 2, QChar('0'));
        blue = QString("%1").arg(pixel.blue(), 8, 2, QChar('0'));

        if (y < mSize.length())
        {
            bit_size = QString("%1").arg(mSize[y].unicode(), 8, 2, QChar('0'));
            red[6] = bit_size[0];
            red[7] = bit_size[1];

            green[5] = bit_size[2];
            green[6] = bit_size[3];
            green[7] = bit_size[4];

            blue[5] = bit_size[5];
            blue[6] = bit_size[6];
            blue[7] = bit_size[7];

            image->setPixel(0, y, qRgb(red.toInt(Q_NULLPTR, 2), green.toInt(Q_NULLPTR, 2), blue.toInt(Q_NULLPTR, 2)));
        }
        else
        {
            // создаем нулевой байт для указания конца строки
            red[6] = '0';
            red[7] = '0';

            green[5] = '0';
            green[6] = '0';
            green[7] = '0';

            blue[5] = '0';
            blue[6] = '0';
            blue[7] = '0';

            image->setPixel(0, y, qRgb(red.toInt(Q_NULLPTR, 2), green.toInt(Q_NULLPTR, 2), blue.toInt(Q_NULLPTR, 2)));
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
    const QString zero_str = "00000000";
    QString size = "";

    int height = image->height();

    QString bit_size;
    QColor pixel;
    QString red, green, blue;
    for (int y = 0; y < height; y++)
    {
        pixel = image->pixel(0, y);
        red = QString("%1").arg(pixel.red(), 8, 2, QChar('0'));
        green = QString("%1").arg(pixel.green(), 8, 2, QChar('0'));
        blue = QString("%1").arg(pixel.blue(), 8, 2, QChar('0'));

        bit_size[0] = red[6];
        bit_size[1] = red[7];

        bit_size[2] = green[5];
        bit_size[3] = green[6];
        bit_size[4] = green[7];

        bit_size[5] = blue[5];
        bit_size[6] = blue[6];
        bit_size[7] = blue[7];

        // нулевой байт означает конец действительного номера
        if (bit_size == zero_str) break;
        size += QChar(bit_size.toInt(Q_NULLPTR, 2));
    }
    return QString::number(size.toInt(Q_NULLPTR, 36));
}

int Steganography::GetMaxSize()
{
    int width = image->width()-1, height = image->height();
    // ширина изображения должны быть четной, для корректного шифрования
    // т.к на один символ используется 4 пикселя
    while (width%4 != 0) { width--; }
    return ((width * height) - height) / 4;
}

void Steganography::encode(QString message)
{
    int height = image->height(), width = image->width()-1, mSize = message.length();
    // ширина изображения должны быть четной, для корректного шифрования
    // т.к на один символ используется 4 пикселя
    while (width%4 != 0) { width--; }
    SetSize(mSize);

    QColor pixel;
    QString bit_mess(16);
    QString red, green, blue;
    for (int y = 0, count = 0, index = 0; y < height; y++)
    {
        if (count >= mSize) break;
        for (int x = 1; x < width; x++)
        {
            if (count >= mSize) break;
            index = 0;

            bit_mess = QString("%1").arg(message[count++].unicode(), 16, 2, QChar('0'));
            while(index < 16)
            {
                pixel = image->pixel(x, y);
                red = QString("%1").arg(pixel.red(), 8, 2, QChar('0'));
                green = QString("%1").arg(pixel.green(), 8, 2, QChar('0'));
                blue = QString("%1").arg(pixel.blue(), 8, 2, QChar('0'));

                red[6] = bit_mess[index++];
                red[7] = bit_mess[index++];
                green[7] = bit_mess[index++];
                blue[7] = bit_mess[index++];

                image->setPixel(x++, y, qRgb(red.toInt(Q_NULLPTR, 2), green.toInt(Q_NULLPTR, 2), blue.toInt(Q_NULLPTR, 2)));
            }
            // инкримент, т.к в блоке while будет один лишний переход
            x--;
        }
    }
}

QString Steganography::decode()
{
    int height = image->height(), width = image->width()-1, mSize = GetSize().toInt();
    // ширина изображения должны быть четной, для корректного шифрования
    // т.к на один символ используется 4 пикселя
    while (width%4 != 0) { width--; }

    QColor pixel;
    QString bit_mess(16), message = "";
    QString red, green, blue;
    for (int y = 0, count = 0, index = 0; y < height; y++)
    {
        if (count == mSize) break;
        for (int x = 1; x < width; x++)
        {
            if (count == mSize) break;
            count++;
            index = 0;

            while(index < 16)
            {
                pixel = image->pixel(x++, y);
                red = QString("%1").arg(pixel.red(), 8, 2, QChar('0'));
                green = QString("%1").arg(pixel.green(), 8, 2, QChar('0'));
                blue = QString("%1").arg(pixel.blue(), 8, 2, QChar('0'));

                bit_mess[index++] = red[6];
                bit_mess[index++] = red[7];
                bit_mess[index++] = green[7];
                bit_mess[index++] = blue[7];
            }
            // инкримент, т.к в блоке while будет один лишний переход
            x--;
            message += QChar(bit16_to_int(bit_mess));
        }
    }
    return message;
}
