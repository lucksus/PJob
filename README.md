PJob
====

PJob is a framework of apps and libs to help distribute your (scientific) calculation job over
a network of workers.
It was formerly meant to be used with [PHOTOSS](http://photoss.de) (hence its name P(hotoss)Job)
but can now be used with every application or simulation runner that accepts parameter values
as command line arguments.
If your simulation/calculation can be described as a function `f` which maps an arbitrary number of arguments
to one (or many) scalar result value(s) and every instance of your app can be configured to calculate 
exactly one result and save it to a file,
you can use PJob to automatically distribute your app, calculate all specified parameter combinations
and collect the results within one file on the machine you submitted the calculation from.
PJob is written in Qt based C++ and is compatible with every platform Qt compiles on.

Requirements
------------
* gcc or clang
* [boost](http://www.boost.org/) (>= 1.40)
* [Qt](http://qt-project.org/) (>=4.8)
* LaTeX for the documentation

Overview
--------
PJob can be subdived into three parts.
1. PJob file format and corresponding tools
2. PJobRunner deamon which has to be installed on the worker machines
3. PQueue which runs on the submitting desktop machine, talks to all PJobRunner instances within in the network
   and collects the results

PJob files are archives like zip or tar files but with a special internal directory structure.
The fist step is to create a PJob file containing your app and some xml files whith additional information
about your job: which parameters does it take? which results does it create?
PJobFileCmd is a tar like command line tool to open, create and modifiy PJob file archives.
PJobFileEditor is a graphical tool for editing PJob files.

PJobRunner must be running on every machine you want to use as calculation server.
On Windows PJobRunner can be installed as deamon by invoking it whith the command line switch "-i".

PQueue takes a PJob file and sees it as function which maps parameters to result values.
PQueue can do a network scan to find running PJobRunners.

**At the moment, the provided documentation is outdated.**
It describes PJob as it was designed to work with PHOTOSS only.
However, it is well suited to provide an overview.
The chapters about the PJob file format is still valid.

Build
-----
In order to build the whole project, change into the cloned directory and say
  qmake
  make
or open the .pro file with [Qt-Creator](http://qt-project.org/wiki/Category:Tools::QtCreator).
