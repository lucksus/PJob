#pragma once
#include <QtCore/QString>
#include <QtCore/QDir>
#include "PJobFileFormat.h"
#include "PJobFileParameterDefinition.h"
#include "PJobFileParameter.h"
#include "PJobResultFile.h"
#include "hash.h"
#include "PJobFileFormat.h"
#include "PJobFileApplication.h"
#include <iostream>
#include <QtCore/QSet>
#include <QMutex>

using namespace std;

class PJobFile : public QObject{
Q_OBJECT
public:
    PJobFile(QString pjobFile);
    explicit PJobFile(const QByteArray& data);
    ~PJobFile();
    void addResource(QString path);
    void addResource(const QByteArray& content, const QString& internal_file_name);
    QStringList runDirectoryEntries() const;
    QString latestRunDirectory() const;
    QString pjobFile() const;
    QString mainPscript() const;
    void setMainPscript(QString script);
    PJobFileFormat* getPJobFile();

    QList<PJobFileParameterDefinition> parameterDefinitions() const;
    void addParameterDefinition(const PJobFileParameterDefinition&);
    void removeParameterDefinition(QString parameterName);
    void writeParameterDefinitions(QList<PJobFileParameterDefinition>);

    QList<PJobFileParameter> readParameterCombinationForRun(QString run);
    void writeParameterCombinationForRun(QList<PJobFileParameter>, QString run);

    QList<PJobResultFile> readResultDefinitions();
    void writeResultDefinitions(QList<PJobResultFile>);

    QList<PJobFileApplication> applications() const;
    PJobFileApplication applicationByName(QString name) const;
    void addApplication(const PJobFileApplication&);
    void removeApplication(const QString& name);
    void writeApplications(const QList<PJobFileApplication>&);
    void renameApplication(const QString& old_name, const QString& new_name);
    QString defaultApplication();
    void setDefaultApplication(const QString& name);

    void save();
    void setSaveAutomatically(bool);

    void copyWithoutRuns(QString path);
    void mergeRunsFrom(const PJobFile&);
    QHash< QHash<QString,double>, QHash<QString,double> > getResultsForRun(QString run);

    void checkIfRunIsProperlyFinished(QString run);

    void remove_all_runs();

    void export_application(QString application_name, QString path);
    void export_resources(QString path);
    void import_run_directory(QString path, const QList<PJobFileParameter>&);

    //! Reads all result files (all files in all run directories) with header from underlying file format and concatenates them into one QByteArray
    QByteArray* get_result_files_raw();
    QByteArray* raw_without_results();

    void add_raw_files(const QByteArray&);
    static QString name_of_first_run_in_raw_bytes(const QByteArray&);


public slots:
    void abort_progress(const QString &);

signals:
    void changed();
    void progress(QString message, unsigned int percent);

protected:
    PJobFileFormat* m_data;
    QString m_pjobFile;
    bool m_saveAutomatically;

    QList<PJobFileParameter> makeParameterCombinationValid(const QList<PJobFileParameter>&) const;

private:
    void create();
    mutable QMutex m_mutex;

    //funktionieren z.Z. nicht!
    QList< QHash< QString, double > > readResultFilePHOTOSS_CSV(QString resultFile);
    QHash< QHash<QString,double>, QHash<QString,double> >  readResultsPHOTOSS_CSV(QString run);
    static QList< QHash< QString, double > > resultsFromFile(istream& input);
    static QStringList readHeader(istream& file);
    static QHash<QString,double> readValues(istream& input, QStringList parameters);
    static pair< QStringList, QStringList > tellParametersAndResultsApart(QStringList header, QStringList knownParameters);
    QSet<QString> m_progresses_to_abort;
};
