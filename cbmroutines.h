#ifndef CBMROUTINES_H
#define CBMROUTINES_H

#include <QStringList>

class CBMroutines
{
public:
    CBMroutines();

    static QStringList list_dir(QString filename);
    static int copyFromD64(QString d64, QString filename, QString path);
    static QString stringToPETSCII(QString pS, bool usec64font, bool cbmctrlhasraw);
    static QString stringToPETSCII(QByteArray pS, bool keepSpecialChars, bool usec64font, bool cbmctrlhasraw);
    static QString randomString(const int len);
    static QString formatFileSize(qint64 size);
    static char cbm_petscii2ascii_c(char c);

private:
    static QString ptoa(unsigned char *s);
    static void atop(unsigned char *s);
};

#endif // CBMROUTINES_H
