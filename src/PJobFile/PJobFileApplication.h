#ifndef PJOBFILEBINARY_H
#define PJOBFILEBINARY_H
#include <QtCore/QString>



class PJobFileApplication
{
public:
    enum Platform{
        Win32, Win64, MacOSX, Linux
    };

    PJobFileApplication()
    {
        platform = Linux;
#ifdef Q_WS_WIN
        platform = Win32;
#endif
#ifdef Q_WS_MACX
        platform = MacOSX;
#endif
    }


    QString name;
    QString program_name;
    QString program_version;
    Platform platform;
    QString executable;
    QString arguments;
    QString parameter_pattern;


};

#endif // PJOBFILEBINARY_H
