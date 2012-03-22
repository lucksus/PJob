#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#define MAX_CONSOLE_LINES 800

void repairStdout(){
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE                       *fp;

	// allocate a console for this app
	AllocConsole();

	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), 
		&coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), 
		coninfo.dwSize);

	// redirect unbuffered STDOUT to the console
	long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	long hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog 
	// point to console as well
	std::ios::sync_with_stdio();
}
#endif





#include "Condor.h"
#include "Job.h"
#include <time.h>


using namespace CondorAdapter;

int main(int argc, char *argv[])
{
#ifdef _WIN32
	repairStdout();
#endif
	Job job("d:/users/nico/condor_adapter_test","d:/users/nico/photoss_release/photoss.exe");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/ApplicationBasics_dll.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/DefaultModelTree.txt");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/DefaultUserTree.txt");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/condor_exec.exe.manifest");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/ToolkitPro1113vc80.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/component_dlls/dev_kit.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/matlabR12_dll.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/matlabR13_dll.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/matlabR14_dll.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/matlabR2006a_dll.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/msvcp80.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/msvcr80.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/photoss_dll.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/visualizer_dll.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/QtCore4.dll");
	job.addFileToBeTransfered("D:/users/Nico/photoss_release/QtGui4.dll");
	job.addFileToBeTransfered("D:/users/Nico/bla.pho");

	job.addArgument("-m none -r bla.pho");

	Condor::getInstance().submit(&job);

	bool stop=false;
	while(!stop){
		cout<<endl;
		switch(job.getState()){
			case _notsubmited:
				cout<<"not submited";break;
			case _submited:
				cout<<"submited";break;
			case _running:
				cout<<"running";break;
			case _suspended:
				cout<<"suspended";break;
			case _finished:
				cout<<"finished! ...bye";stop=true;break;
		}
		Sleep(2000);
	}



}