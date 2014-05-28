#ifndef CBMROUTINES_H
#define CBMROUTINES_H

#include <QStringList>

class CBMroutines
{
public:
    CBMroutines();

    static QByteArray list_dir(QString filename);
    static int copyFromD64(QString d64, QByteArray filename, QString path);
    static QString stringToPETSCII(QString pS, bool cbmctrlhasraw);
    static QString stringToPETSCII(QByteArray pS, bool keepSpecialChars, bool cbmctrlhasraw);
    static QString PETSCIItoString(QString text);
    static QString randomString(const int len);
    static QString formatFileSize(qint64 size);
    static char cbm_petscii2ascii_c(char c);
    static char * cbm_petscii2ascii(char *Str);
    static QString ptoa(unsigned char *s);
    static void atop(unsigned char *s);

private:

};

#endif // CBMROUTINES_H
