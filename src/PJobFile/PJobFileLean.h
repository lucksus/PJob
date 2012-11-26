#ifndef PJOBFILELEAN_H
#define PJOBFILELEAN_H
#include <string>
#include <QList>
#include <QHash>
#include <QFile>
#include <QStringList>
#include "PJobFileParameter.h"
#include "PJobResultFile.h"
#include "PJobFileParameterDefinition.h"

class PJobFileLean
{
public:
    typedef QList<PJobFileParameter> ParameterCombination;
    PJobFileLean(QString file_path);

    QList<ParameterCombination> parameter_combinations();
    QStringList files_in_run(ParameterCombination parameter_combination);
    QByteArray* read_result_file(ParameterCombination parameter_combination, QString file);
    QList<PJobResultFile> result_definitions();

protected:
    QString m_file_path;
    QStringList m_files;
    QHash<QString, qint64> m_file_positions;
    QHash<ParameterCombination, QString> m_run_directories;
    QList<PJobResultFile> m_result_definitions;
    QList<PJobFileParameterDefinition> m_parameter_definitions;

    void scan_file();

    struct chunk_header{
        std::string file_path;
        long modification_date;
        int size;
    };

    struct run{
        QList<PJobFileParameter> parameter_combination;
        QHash<QString, QByteArray*> result_files;
    };


    bool check_header(QFile& file);
    chunk_header read_chunk_header(QFile& file);
    QByteArray* read_file(const chunk_header& header, QFile& file);
    QByteArray* read_file(qint64, QFile& file);
};

#endif // PJOBFILELEAN_H
