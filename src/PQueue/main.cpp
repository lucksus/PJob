#include "Workspace.h"
#include <QtGui/QApplication>
#include "MainWindow.h"
//#include <Windows.h>
#include <QtCore/QFileInfo>
#include <QSplashScreen>
#include <boost/program_options.hpp>
#include <vector>
#include <boost/foreach.hpp>
#include "Scripter.h"
namespace po = boost::program_options;
using namespace po;
using namespace std;

QSplashScreen* global_splash_screen;

int main(int argc, char** argv){

	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("import-csv", po::value<std::string>(), "import results from given file into result space")
		("view-result", po::value<std::string>(), "import results from given file into result space")
		("view-file", po::value<std::string>(), "import results from given file into result space")
		("do-variation", po::value<std::string>(), "run parameter variation script on given pjob file")
		("param-min", value< vector<string> >(), "set minimum value for parameter variation (only effective with \"do-variation\")")
		("param-max", value< vector<string> >(), "set maximum value for parameter variation (only effective with \"do-variation\")")
		("param-step", value< vector<string> >(), "set step value for parameter variation (only effective with \"do-variation\")")
		("param-variation-vector", value< vector<string> >(), "set variation vector of varied parameter (only effective with \"do-variation\")")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);    

	if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}

	QApplication app(argc,argv);
	global_splash_screen = new QSplashScreen(QPixmap(":/images/splash"));
	global_splash_screen->setWindowFlags(global_splash_screen->windowFlags() | Qt::WindowStaysOnTopHint);
	global_splash_screen->show();

	MainWindow w;

	if (vm.count("import-csv")) {
		std::string filename = vm["import-csv"].as<std::string>();
                Workspace::getInstace().getResults().importFromCSV(filename.c_str());
	}

	

	if (vm.count("view-result") && vm.count("view-file")) {
		std::string result = vm["view-result"].as<std::string>();
		std::string file = vm["view-file"].as<std::string>();
		w.viewResult(file.c_str(),result.c_str());
	}

	if (vm.count("do-variation")) {
		std::string filename = vm["do-variation"].as<std::string>();
		QString init_script = QString("proxy.params = new Object; proxy.filename=\"%1\";").arg(filename.c_str());

		QStringList suffices; suffices << "min" << "max" << "step";
		QSet<QString> defined_parameters;

		foreach(QString suffix, suffices){
			vector<string> param_strings;
			try{
				param_strings = vm[QString("param-%1").arg(suffix).toStdString()].as<vector<string> >();
			}catch(boost::bad_any_cast){
				continue;
			}
			BOOST_FOREACH(string param_string, param_strings){
				QString str(param_string.c_str());
				QStringList list = str.split("=");
				if(list.size() != 2) continue;
				QString param_name = list.front();
				QString value = list.back();
				if(!defined_parameters.contains(param_name)){
					init_script.append(QString("proxy.params.%1 = new Object;").arg(param_name));
					defined_parameters.insert(param_name);
				}
				
				init_script.append(QString("proxy.params.%1.%3 = %2;").arg(param_name).arg(value).arg(suffix));
			}
		}

		vector<string> variation_vectors;
		try{
			variation_vectors = vm["param-variation-vector"].as<vector<string> >();
			BOOST_FOREACH(string variation_vector, variation_vectors){
				QString str(variation_vector.c_str());
				QStringList list = str.split("=");
				if(list.size() != 2) continue;
				QString param_name = list.front();
				QString value_list = list.back();
				QStringList values = value_list.split(",");
				if(values.size() < 2) continue;
				if(!defined_parameters.contains(param_name)){
					init_script.append(QString("proxy.params.%1 = new Object;").arg(param_name));
					defined_parameters.insert(param_name);
				}
				init_script.append(QString("proxy.params.%1.variation_values = [%2];").arg(param_name).arg(value_list));
			}
		}catch(boost::bad_any_cast){
			;
		}

		Scripter::getInstance().runLoadedScript("parametervariation.js", init_script);
	}

	w.show();
	app.exec();
}
