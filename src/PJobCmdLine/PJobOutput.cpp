#include "PJobOutput.h"

PJobOutput::PJobOutput(PJobFileFormat *file)
{
	m_pjobFile = file;
}

PJobOutput::~PJobOutput()
{
}

void PJobOutput::setVerbose(bool mode)
{
	if(mode)
		connect(m_pjobFile,SIGNAL(output(QString)),this,SLOT(print(QString)));
	else
		disconnect(m_pjobFile,SIGNAL(output(QString)),this,SLOT(PJobOutput::printFile(QString)));
}

void PJobOutput::printAbout()
{
	std::cout << "\nPJobFileFormat Commandline, Version " << PJobFileFormat::version();
	std::cout << "\n@Author:  Daniel Merget\n";
	std::cout << "@Date:    30.07.2010\n";
	std::cout << "Copyright by Jens Lenge. All rights reserved.\n";
}

void PJobOutput::printDetail(std::string command)
{
	if((command=="add")||(command=="a"))
		std::cout << "\n###### DETAIL ######\n\n --add SOURCE1 TARGET1 SOURCE2 TARGET2 [...]\n\nSOURCE is obligatory and may not be nil. TARGET is optional. For a nil-Value enter \"NULL\" or \".\"\n\nSOURCE specifies the file or folder to append. If no source is specified, an error is thrown.\n\nTARGET specifies the name that shall be given to the file/folder within the .pjob-file. If no target is specified, the name will be untouched.\n\nExample:\nPJobCmdLine.exe --file C:/my.pjob --add C:/myFile.cpp . C:/myDir/ Resources\nPJobCmdLine.exe --f max.pjob D:/myFolder/this_shall_be_named_MAX.EXE MAX.EXE\nPJobCmdLine.exe --file C:/myFile.pjob --add C:/does_already_exist --force\nPJobCmdLine.exe -f other.pjob -a this.is . allowed.too\n\n####################\n";
	else if((command=="peek")||(command=="p"))
		std::cout << "\n###### DETAIL ######\n\n --peek SOURCE1 SOURCE2 SOURCE3 [...]\n\nAt least one SOURCE is obligartory, otherwise use --extract.\n\nSOURCE specifies which files/folders within the .pjob-file shall be extracted.\nIf no target is specified via --out, all files will be extracted to the .pjob-filepath.\n\nExample:\nPJobCmdLine.exe --file C:/my.pjob --peek Resources/ Runs/ and.some other.files\nPJobCmdLine.exe --file C:/my.pjob --peek I_need/this_file.only\nPJobCmdLine.exe -f other.pjob -p file_aready_exists_in_out_path -o C:/ --force\n\n####################\n";
	else if((command=="create")||(command=="c"))			
		std::cout << "\n###### DETAIL ######\n\n --create SOURCE\n\nOnly one SOURCE is allowed. SOURCE specifies which file/folder shall be added to the newly created or empty file.\n\nExample:\nPJobFileFormat.exe --file C:/my.pjob --create C:/my_pjob_file/shall_contain_this_folder_only/\nPJobFileFormat.exe -f other.pjob -c just/one.file\n\n####################\n";
	else if((command=="extract")||(command=="x"))
		std::cout << "\n###### DETAIL ######\n\n --extract\n\nNo arguments allowed.\n\nAll files will be extracted to the specified --out path.\n\nExample:\nPJobCmdLine.exe --file C:/my.pjob --extract\nPJobCmdLine.exe -f other.pjob -x -o C:/\n\n####################\n";
	else if((command=="force")||(command=="f"))
		std::cout << "\n###### DETAIL ######\n\n --force\n\nThere is always the possibility of data loss, thus it is recommended to use --force with consideration.\n\nWhen extracting or appending, --force will overwrite all files that already exist.\nWhen creating a new file via --create, --force will delete any prior file with the same path and replace it with a new one.\n\n####################\n";
	else
		std::cout << "\n###### DETAIL ######\n\nNo detailed description available for " << command << ".\n\n####################\n";
}

void PJobOutput::print(QString text)
{
	std::cout << text.toStdString() << "\n";
}