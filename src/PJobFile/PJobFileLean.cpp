#include "PJobFileLean.h"
#include <cmath>
#include "PJobFileXMLFunctions.h"
#include <assert.h>
#include "hash.h"

PJobFileLean::PJobFileLean(QString file_path)
    : m_file_path(file_path)
{
    scan_file();
}

QList<PJobFileLean::ParameterCombination> PJobFileLean::parameter_combinations(){
    return m_run_directories.keys();
}

QByteArray* PJobFileLean::read_result_file(ParameterCombination parameter_combination, QString file){
    QString run_name = *(m_run_directories.find(parameter_combination));
    QString path = QString("Runs/%1/%2").arg(run_name).arg(file);
    qint64 pos = *(m_file_positions.find(path));
    QFile input_file(m_file_path);
    if(!input_file.open(QIODevice::ReadOnly))
        throw QString("Could not open \"%1\" for reading!").arg(m_file_path);
    return read_file(pos, input_file);
}

QList<PJobResultFile> PJobFileLean::result_definitions(){
    return m_result_definitions;
}



bool PJobFileLean::check_header(QFile& file){
    QByteArray header = file.read(21);
    return header.startsWith("PJobFile\n");
}

PJobFileLean::chunk_header PJobFileLean::read_chunk_header(QFile& file){
    chunk_header header;
    QByteArray file_name = file.readLine();
    file_name.chop(1);

    header.file_path = QString(file_name).toStdString();
    file.read(reinterpret_cast<char*>(&header.modification_date), 8);
    file.read(4);

    QByteArray size = file.read(4);
    quint32 j=0;
    quint32 pos=0;
    for(int i=0;i<4;i++)
        j+=static_cast<unsigned char>(size.at(pos++))*pow(256,i);
    header.size = j;

    return header;
}

QByteArray* PJobFileLean::read_file(const chunk_header& header, QFile& file){
    QByteArray compressed = file.read(header.size);

    int len = header.size;
    //4 Byte vorne anhängen und Dekomprimieren
    for(int i=0;i<4;i++)
    {
        compressed.prepend(len);
        len/=256;
    }
    return new QByteArray(qUncompress(compressed));
}

QByteArray* PJobFileLean::read_file(qint64 pos, QFile& file){
    file.seek(pos);
    return read_file(read_chunk_header(file),file);
}


void PJobFileLean::scan_file(){
    QFile input_file(m_file_path);
    if(!input_file.open(QIODevice::ReadOnly))
        throw QString("Could not open \"%1\" for reading!").arg(m_file_path);


    if(!check_header(input_file))
        throw QString("Specified file is no PJob file! Exiting..");



    while(!input_file.atEnd()){
        qint64 pos = input_file.pos();
        chunk_header header = read_chunk_header(input_file);

        if(header.file_path == "parameterdefinitions.xml"){
            QByteArray* content = read_file(header,input_file);
            m_parameter_definitions = PJobFileXMLFunctions::readParameterDefinitions(*content);
            delete content;
            continue;
        }

        if(header.file_path == "resultdefinitions.xml"){
            QByteArray* content = read_file(header,input_file);
            m_result_definitions = PJobFileXMLFunctions::readResultDefinitions(*content);
            delete content;
            continue;
        }

        QString file_path = QString(header.file_path.c_str());
        m_file_positions[file_path] = pos;
        m_files << file_path;

        if(file_path.startsWith("Runs/")){
            QStringList items = file_path.split("/");
            assert(items.size() > 2);
            QString run_name = items.at(1);
            QString file_name = items.at(2);

            if(file_name == "parametercombination.xml"){
                QByteArray* content = read_file(header,input_file);
                ParameterCombination parameter_combination = PJobFileXMLFunctions::readParameterCombination(*content);
                m_run_directories[parameter_combination] = run_name;
                delete content;
                continue;
            }
            /*else{
                if(!result_defintions.empty()){
                    bool is_result = false;
                    foreach(PJobResultFile result, result_defintions){
                        if(result.filename() == file_name) is_result = true;
                    }
                    if(!is_result){
                        input_file.seek(input_file.pos() + header.size);
                        continue;
                    }
                }
                QByteArray* content = read_file(header,input_file);
                runs[run_name].result_files[file_name] = content;

                if(!result_defintions.empty() && !parameter_definitions.empty()){
                    if(!header_written){
                        write_csv_header(parameter_definitions, result_defintions, output_file, filePath.c_str());
                        header_written = true;
                    }
                    foreach(QString run_name, runs.keys()){
                        run r = runs[run_name];
                        if(is_run_complete(result_defintions,r)){
                            write_run_line(r, result_defintions, output_file, filePath.c_str());
                            foreach(QByteArray* content, r.result_files.values()){
                                delete content;
                            }
                            runs.remove(run_name);
                        }
                    }
                }*/


        }
        input_file.seek(input_file.pos() + header.size);
    }
}
