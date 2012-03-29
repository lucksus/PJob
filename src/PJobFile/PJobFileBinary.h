#ifndef PJOBFILEBINARY_H
#define PJOBFILEBINARY_H
#include <QtCore/QString>

class PJobFileBinary
{
public:
    enum Platform{
        Win32, Win64, MacOSX, Linux
    };
    QString name;
    QString program_name;
    QString program_version;
    Platform platform;
    QString executable;
    QString parameter_pattern;


};

#endif // PJOBFILEBINARY_H
