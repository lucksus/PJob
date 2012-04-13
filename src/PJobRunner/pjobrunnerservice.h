#ifndef PJOBRUNNERSERVICE_H
#define PJOBRUNNERSERVICE_H

#include "qtservice.h"

class PJobRunnerService : public QtService<QCoreApplication>
{
public:
    PJobRunnerService(int argc, char** argv);

protected:
    void start();
    void stop();
    void pause();
    void resume();
    void processCommand(int code);
};

#endif // PJOBRUNNERSERVICE_H
