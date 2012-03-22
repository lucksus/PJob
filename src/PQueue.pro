TEMPLATE = subdirs
CONFIG += debug_and_release
SUBDIRS = CondorC++Adapter PQueueModel Tests/PQueue PQueue PJobFile

PQueueModel.subdir = PQueueModel
PQueueModel.depends = PJobFile CondorC++Adapter

Tests.subdir = Tests/PQueue
Tests.depends = PQueueModel

PQueue.subdir = PQueue
PQueue.depends = PQueueModel
