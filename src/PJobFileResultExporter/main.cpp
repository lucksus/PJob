#include <QtCore/QCoreApplication>
#include <boost/program_options.hpp>
#include <QFile>
#include <PJobFileXMLFunctions.h>
#include <PJobFileParameterDefinition.h>
#include <PJobResultFile.h>
#include <QHash>
#include <QStringList>

namespace po = boost::program_options;

struct chunk_header{
    std::string file_path;
    long modification_date;
    int size;
};

struct run{
    QList<PJobFileParameter> parameter_combination;
    QHash<QString, QByteArray*> result_files;
};


bool check_header(QFile& file){
    QByteArray header = file.read(21);
    return header.startsWith("PJobFile\n");
}

chunk_header read_chunk_header(QFile& file){
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

QByteArray* read_file(const chunk_header& header, QFile& file){
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

bool is_run_complete(const QList<PJobResultFile>& result_definitions, const run& r){
    if(r.parameter_combination.isEmpty()) return false;
    foreach(PJobResultFile result_file, result_definitions){
        if(! r.result_files.keys().contains(result_file.filename())) return false;
    }
    return true;
}

bool operator<(const PJobFileParameter& p1, const PJobFileParameter& p2){
    return p1.name() < p2.name();
}

bool operator<(const PJobFileParameterDefinition& p1, const PJobFileParameterDefinition& p2){
    return p1.name() < p2.name();
}

bool operator<(const PJobResultFile& r1, const PJobResultFile& r2){
    return r1.filename() < r2.filename();
}

bool operator<(const PJobResult& r1, const PJobResult& r2){
    return r1.name() < r2.name();
}

void write_csv_header(QList<PJobFileParameterDefinition> parameter_definitions, QList<PJobResultFile> result_defintions, QFile& output_file, const QString& file_name){
    std::stringstream out;
    out << "%" << file_name.toStdString() << "\t";
    qSort(parameter_definitions.begin(), parameter_definitions.end());
    foreach(PJobFileParameterDefinition param, parameter_definitions){
        out << param.name().toStdString() << "\t";
    }
    out << "\t";
    qSort(result_defintions.begin(), result_defintions.end());
    foreach(PJobResultFile result_file, result_defintions){
        qSort(result_file.results().begin(), result_file.results().end());
        foreach(PJobResult result, result_file.results()){
            out << result.name().toStdString() << "\t";
        }
    }

    out << std::endl;

    out << "%PJobFile\t";
    foreach(PJobFileParameterDefinition param, parameter_definitions){
        out << param.name().toStdString() << "\t";
    }
    foreach(PJobResultFile result_file, result_defintions){
        foreach(PJobResult result, result_file.results()){
            out << result.name().toStdString() << "\t";
        }
    }
    out << std::endl;

    output_file.write(out.str().c_str());
}


void write_run_line(run r, const QList<PJobResultFile>& result_definitions, QFile& file, const QString& file_name){
    std::stringstream out;
    out << file_name.toStdString() << "\t";
    qSort(r.parameter_combination.begin(), r.parameter_combination.end());
    foreach(PJobFileParameter param, r.parameter_combination){
        out << param.value() << "\t";
    }
    foreach(PJobResultFile result_file, result_definitions){
        foreach(PJobResult result, result_file.results()){
            out << QString(*r.result_files[result_file.filename()]).trimmed().toStdString() << "\t";
        }
    }
    out << std::endl;
    file.write(out.str().c_str());
}

int main(int argc, char *argv[])
{
    std::vector<std::string> results;
    std::string filePath, out;
    bool verbose=false;

    //zulässige Optionen deklarieren
    po::options_description desc("This program provides commandline support for easy pjob-file results access.\nAllowed options are");
    desc.add_options()
        ("help,h", "* \tproduces this help message")

        ("file,f", po::value<std::string>(&filePath),"* \tspecifies the path of the .pjob-file.")
        ("result,p", po::value<std::vector<std::string> >(&results)->multitoken(),"* \tspecifies which results are to be exported. If none is given, all are exported." )
        ("verbose,v", "* \tdisplays detailed information about the tasks being made, e.g. list of extracted files")
        //("force","* \tsets overwrite-flag for all commands")
        ("out,o", po::value<std::string>(&out),"* \tspecifies the output path\n")
    ;

    //Kommandozeile parsen
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("verbose"))
    {
        verbose=true;
    }

    if(vm.count("file")!=1)
    {
        std::cout << std::endl << "You must provide the path to a pjob file!" << std::endl << "Example: pjob_export -f my_file.pjob -o results.csv"
                     << std::endl << desc << std::endl;
        exit(1);
    }

    if(vm.count("out")!=1)
    {
        std::cout << std::endl << "You must provide the path to a output file!" << std::endl << "Example: pjob_export -f my_file.pjob -o results.csv"
                     << std::endl << desc << std::endl;
        exit(1);
    }

    if (vm.count("help"))
    {
        std::cout << "\n" << desc << "\n";
        exit(1);
    }

    QFile input_file(filePath.c_str());
    if(!input_file.open(QIODevice::ReadOnly)){
        std::cout << "Could not open \"" << filePath << "\" for reading!" << std::endl;
        exit(1);
    }

    QFile output_file(out.c_str());
    if(!output_file.open(QIODevice::ReadWrite)){
        std::cout << "Could not open \"" << out << "\" for writing!" << std::endl;
        exit(1);
    }

    if(!check_header(input_file)){
        std::cout << "Specified file is no PJob file! Exiting.." << std::endl;
        exit(1);
    }


    QList<PJobResultFile> result_defintions;
    QList<PJobFileParameterDefinition> parameter_definitions;
    QHash<QString, run> runs;
    bool header_written = false;


    while(!input_file.atEnd()){
        std::cout << "\r" << input_file.pos()*100/input_file.size() << "% ";
        chunk_header header = read_chunk_header(input_file);

        if(header.file_path == "parameterdefinitions.xml"){
            QByteArray* content = read_file(header,input_file);
            parameter_definitions = PJobFileXMLFunctions::readParameterDefinitions(*content);
            delete content;
            continue;
        }

        if(header.file_path == "resultdefinitions.xml"){
            QByteArray* content = read_file(header,input_file);
            result_defintions = PJobFileXMLFunctions::readResultDefinitions(*content);
            delete content;
            continue;
        }

        QString file_path = QString(header.file_path.c_str());

        if(file_path.startsWith("Runs/")){
            QStringList items = file_path.split("/");
            assert(items.size() > 2);
            QString run_name = items.at(1);
            QString file_name = items.at(2);

            if(file_name == "parametercombination.xml"){
                QByteArray* content = read_file(header,input_file);
                runs[run_name].parameter_combination = PJobFileXMLFunctions::readParameterCombination(*content);
                delete content;
                continue;
            }else{
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
                }


            }
        }

        input_file.seek(input_file.pos() + header.size);

    }
    std::cout << "\r100%" << std::endl;
    return 0;
}
