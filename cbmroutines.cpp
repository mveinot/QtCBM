#include <QDataStream>
#include <QFile>
#include <QMessageBox>
#include <QtDebug>

#include "cbmroutines.h"
#include <diskimage.h>

CBMroutines::CBMroutines()
{

}

QString CBMroutines::ptoa(unsigned char *s) {
    unsigned char c;
    QString str;

    while ((c = *s)) {
        c &= 0x7f;
        if (c >= 'A' && c <= 'Z') {
            c += 32;
        } else if (c >= 'a' && c <= 'z') {
            c -= 32;
        } else if (c == 0x7f) {
            c = 0x3f;
        }
        *s++ = c;
        str.append(c);
    }

    return str;
}

void CBMroutines::atop(unsigned char *s) {
    unsigned char c;

    while ((c = *s)) {
        c &= 0x7f;
        if (c >= 'A' && c <= 'Z') {
            c += 32;
        } else if (c >= 'a' && c <= 'z') {
            c -= 32;
        }
        *s++ = c;
    }
}

char CBMroutines::cbm_petscii2ascii_c(char c)
{
    switch (c & 0xff) {
      case 0x0a:
      case 0x0d:
          return '\n';
      case 0x40:
      case 0x60:
        return c;
      case 0xa0:     // cbm shifted space
      case 0xe0:
        return ' ';
      default:
        switch (c & 0xe0) {
          case 0x40: // 41 - 7E
          case 0x60:
            return (c ^ 0x20);

          case 0xc0: // C0 - DF
            return (c ^ 0x80);

      }
    }

    return ((isprint(c) ? c : '.'));
}

QString CBMroutines::stringToPETSCII(QString pS, bool cbmctrlhasraw = false)
{
    QString output;
    if (cbmctrlhasraw)
    {
        for (int i = 0; i < pS.length(); i++)
        {
            if (pS.at(i).unicode() < 256)
            {
                //qDebug() << "<256: " << pS.at(i).unicode();
                output.append(pS.at(i).unicode()+57344);
            }
            else
            {
                //qDebug() << ">256: " << pS.at(i).unicode();
                output.append(pS.at(i).unicode());
            }
        }
        return output;
    } else
        return pS;
}

QString CBMroutines::PETSCIItoString(QString text)
{
    qDebug() << text;
    QString output;
    for (int i = 0; i < text.length(); i++)
    {
        if (text.at(i).unicode() > 57344)
        {
            output.append(text.at(i).unicode()-57344);
        }
    }
    qDebug() << output;
    return output;
}

QString CBMroutines::stringToPETSCII(QByteArray pS, bool keepSpecialChars = true, bool cbmctrlhasraw = false)
{
    QString output = "";

    if (cbmctrlhasraw)
    {
        for (int i = 0; i < pS.length(); i++)
        {
            int chr = (unsigned char)pS.at(i);

            if (keepSpecialChars)
            {
                if (chr != 32 && chr != 34 && (chr > 127 || (chr >= 33 && chr < 48) || (chr > 57 && chr <= 91)))
                {
                    output.append(QChar(chr+57344));
                } else
                {
                    output.append(QChar(chr));
                }
            } else
            {
                if (chr >= 32 && chr <= 91)
                {
                    output.append(QChar(chr+57344));
                } else if (chr >= 97 && chr <= 122)
                {
                    output.append(QChar(chr-32+57344));
                } else
                {
                    output.append(QChar(chr));
                }
            }
        }
    } else
    {
        return QString(pS);
    }
    return output;
}

QString CBMroutines::randomString(const int len)
{
    QString output = "";

    static const char alpha[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < len; i++)
    {
        output.append(alpha[rand() % (sizeof(alpha) -1)]);
    }

    return output;
}

QString CBMroutines::formatFileSize(qint64 size)
{
    float outputsize = 0;

    if (size > (1024*1024*1024))
    {
        outputsize = size / (1024*1024*1024);
        return QString::number(outputsize).append(" GB");
    }
    else if (size > (1024*1024))
    {
        outputsize = size / (1024*1024);
        return QString::number(outputsize).append(" MB");
    }
    else if (size > (1024))
    {
        outputsize = size / (1024);
        return QString::number(outputsize).append(" KB");
    }
    else
    {
        outputsize = size;
        return QString::number(outputsize).append(" B");
    }
}


QByteArray CBMroutines::list_dir(QString filename)
{
    static QStringList fileTypes = (QStringList() << "del" << "seq" << "prg" << "usr" << "rel" << "cbm" << "dir" << "???");

    DiskImage *di;
    unsigned char buffer[254];
    ImageFile *dh;
    int offset;
    char quotename[19];
    char name[17];
    char id[6];
    int type;
    //int closed;
    //int locked;
    int size;

    if ((di = di_load_image(filename.toLocal8Bit().data())) == NULL)
    {
        qDebug() << "Load image failed";
        return QByteArray();
    }

    if ((dh = di_open(di, (unsigned char *)"$", T_PRG, (char *)"rb")) == NULL)
    {
        qDebug() << "Couldn't open directory";
        return QByteArray();
    }

    di_name_from_rawname(name, di_title(di));

    memcpy(id, di_title(di) + 18, 5);
    id[5] = 0;

    QString label = QString("0 \"%1\" %2").arg(ptoa((unsigned char *)name), -16).arg(ptoa((unsigned char *)id));
    //QStringList output = QStringList() << label;
    QByteArray output;
    output.append("label|").append(label).append('\n');

    if (di_read(dh, buffer, 254) != 254)
    {
        qDebug() << "BAM read failed";
        di_close(dh);
        di_free_image(di);
        return output;
    }

    QByteArray dirEntry;

    /* Read directory blocks */
    while (di_read(dh, buffer, 254) == 254) {
        dirEntry.clear();
        for (offset = -2; offset < 254; offset += 32) {

            /* If file type != 0 */
            if (buffer[offset+2]) {

                di_name_from_rawname(name, buffer + offset + 5);
                type = buffer[offset + 2] & 7;
                //closed = buffer[offset + 2] & 0x80;
                //locked = buffer[offset + 2] & 0x40;
                size = buffer[offset + 31]<<8 | buffer[offset + 30];

                //qDebug() << name;

                /* Convert to ascii and add quotes */
                //ptoa((unsigned char *)name);
                //sprintf(quotename, "\"%s\"", name);

                output.append("entry|").append(QString::number(size)).append('|').append(name).append('|').append(fileTypes.at(type)).append('\n');

                /* Print directory entry */
                //dirEntry = QString("%1  %2%3").arg(size, -4).arg("\""+stringToPETSCII(QByteArray(name), false, true, true)+"\"", -18).arg(fileTypes.at(type));
                //output << dirEntry;
            }
        }
    }

    //output << QString("%1 blocks free").arg(di->blocksfree);
    output.append("bfree|").append(QString::number(di->blocksfree)).append(" blocks free");

    di_close(dh);
    di_free_image(di);

    return output;
}

int CBMroutines::copyFromD64(QString d64, QByteArray filename, QString path)
{
    DiskImage *di;
    unsigned char buffer[4096];
    ImageFile *infile;
    int len;
    unsigned char rawname[16];
    char name[17];
    int size = 0;
    QFile outfile(path+"/"+filename+".prg");

    if (outfile.exists())
    {
        if (QMessageBox::question(0, "QtCBM", "The file \""+filename+"\" already exists. Overwrite?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
            return -1;
    }

    if ((di = di_load_image(d64.toLocal8Bit().data())) == NULL)
    {
        qDebug() << "Load image failed";
        return -1;
    }

    //qDebug() << "filename: " << filename.data();

    strncpy(name, filename.data(), 16);
    name[16] = 0;

    //qDebug() << "name: " << name;

    //atop((unsigned char *)name);

    //qDebug() << "atop: " << name;
    di_rawname_from_name(rawname, name);
    //qDebug() << "rawname: " << rawname;
    if ((infile = di_open(di, rawname, T_PRG, (char *)"rb")) == NULL) {
        qDebug() << "Couldn't open file for reading";
        di_free_image(di);
        return -1;
    }

    if (!outfile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Couldn't open file for writing";
        di_close(infile);
        di_free_image(di);
        return -1;
    }
    QDataStream out(&outfile);

    while ((len = di_read(infile, buffer, 4096)) > 0)
    {
        if (out.writeRawData((char *)buffer, len) != len)
        {
            qDebug() << "Write error";
            outfile.close();
            di_close(infile);
            di_free_image(di);
            return -1;
        }
        size += len;
    }

    return size;
}
