#include <boost/program_options.hpp>
#include "PJobOutput.h"
#include <iterator>
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
	//Hilfsvariablen
	int count=0;
	PJobFileFormat *file;
	PJobOutput *output;
	std::vector<std::string> addParams, peekParams;
	std::string filePath, detail, out, create;
	bool overwrite = false, verbose=false;

	//zulässige Optionen deklarieren
	po::options_description desc("This file provides commandline support for easy pjob-file access.\nAllowed options are");
    desc.add_options()
        ("help,h", "* \tproduces this help message")
		("detail,d",po::value<std::string>(&detail),"* \tshows a detailed description of the specified command with examples if provided")
		("about,?","* \tshows the about-dialog")
		
		("file,f", po::value<std::string>(&filePath),"* \tspecifies the path of the .pjob-file. If it doesn't exist, an empty file will be created automatically")
		("verbose,v", "* \tdisplays detailed information about the tasks being made, e.g. list of extracted files")
		("force","* \tsets overwrite-flag for all commands")
		("out,o", po::value<std::string>(&out),"* \tsets the target directory for --extract and --peek\n")
		
		("extract,x", "* \textracts all files")
		("peek,p", po::value< std::vector<std::string> >(&peekParams)->multitoken(), "* \textracts the specified source files to --out path")
		("create,c", po::value<std::string>(&create),"* \tcreates a new .pjob-file which ONLY contains the specified folder. Only takes effect if the file is empty, doesn't exist or --force is used")
		("add,a" , po::value< std::vector<std::string> >(&addParams)->multitoken(), "* \tadds a folder or directory to the specified file and flush() is called")
    ;

	//Kommandozeile parsen
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

	if (vm.count("verbose"))
	{
		verbose=true;
	}

	if (vm.count("create"))
	{
		QFile temp(QString::fromStdString(filePath));
		if((temp.exists())&&vm.count("force"))
			temp.remove();
	}

	if (vm.count("file"))
	{
		//Öffnen der .pjob-Datei und Fehler abfangen
		try
		{
			file = new PJobFileFormat(QString::fromStdString(filePath));
			if(file == NULL)
				throw QString("Not enough memory.");
		}
		catch(QString s)
		{
			std::cerr << "\nError: " << s.toStdString() << "\n";
			return 1;
		}

		//Ausgabeklasse verknüpfen und Ausgabe
		output = new PJobOutput(file);
		if(verbose)
		{
			output->setVerbose(true);
			std::cout << "\nOpening " << filePath << " was succesful.\n";
		}
	}

	if (vm.count("force"))
	{
		overwrite=true;
	}

    if (vm.count("help")) 
	{
        std::cout << "\n" << desc << "\n";
    }

	if (vm.count("detail"))
	{
		output->printDetail(detail);
	}

	if (vm.count("about"))
	{
		output->printAbout();
	}

	//Bei Aufruf von Hilfe-Funktionen alle weiteren Eingaben blockieren, um den Nutzer nicht zu verwirren (z.B. no-file-Dialog)
	if ((vm.count("help"))||(vm.count("detail"))||(vm.count("about")))
	{
		return 0;
	}

	if(!vm.count("file"))
	{
		//Abbruch, da ohne pjob-Datei nichts mehr gemacht werden kann
		std::cerr << "\nNo file has been specifed.\n";
		return 1;
	}

	//Anzahl der Befehle zählen und ggf. Fehler ausgeben
	if(vm.count("extract")) count++;
	if(vm.count("peek")) count++;
	if(vm.count("create")) count++;
	if (vm.count("add")) count++;
	if (count > 1)
	{
		std::cerr << "\nOnly 1 command (i.e. -x -p -c -a) allowed at the same time.\n";
		return 1;
	}

	if(vm.count("extract"))
	{
		if(verbose)
			std::cout << "\nExtracting files:\n";
		file->extract(QString::fromStdString(out),NULL,overwrite);
	}

	if(vm.count("peek"))
	{
		if(verbose)
			std::cout << "\nExtracting files:\n";
		
		std::vector<std::string>::iterator i;

		//über Parameter iterieren und entpacken
		for(i = peekParams.begin(); i != peekParams.end(); i++)
			file->extract(QString::fromStdString(out),QString::fromStdString(*i),overwrite);
	}

	if(vm.count("create"))
	{
		//Schreiben
		if(file->content().size()==0)
		{
			if(verbose)
				std::cout << "\nCreating file " << filePath << " containing:\n";
            QString s=QString::fromStdString(create);
            if(!file->appendFolder(s))
            {
				std::cerr << "\nCouldn't append data to file.\n";
				return 1;
			}
		}
		else
		{
			std::cerr << "\nFile wasn't created because it already exists and is not empty. Use --force to overwrite.\n";
			return 1;
		}
		file->flush();
	}

	if (vm.count("add"))
	{
		if(verbose)
			std::cout << "\nAppending files to " << filePath << ":\n";
		
		std::vector<std::string>::iterator i;
		
		// in 2er-Schritten über Parameter iterieren
		for(i = addParams.begin();(i != addParams.end()); i++)
		{
			//Wenn nur 1 Argument am Ende übrig ist kann dies nicht innerhalb der Schleife verarbeitet werden
			if(i+1 == addParams.end())
				break;

			//Speichern des 1. Arguments
			std::string temp = *i;
			i++;

			//falls das zweite Argument leer ist, muss der String angepasst werden
			QString s2 = NULL;
			if((*i!="NULL")&&(*i!="."))
				s2 = QString::fromStdString(*i);

			//Datei(en) hinzufügen
			file->appendFolder(QString::fromStdString(temp),s2,overwrite);
		}

		//Wenn noch ein Argument am Ende einzeln übergeben wurde
		if(i!=addParams.end())
			file->appendFolder(QString::fromStdString(*i),NULL,overwrite);

		std::cout << "\n";
		
		//Änderungen speichern
                file->flush();
	}

	//Allokierten Speicher freigeben
	if(vm.count("file"))
	{
		delete output;
		delete file;
	}

    return 0;
}
