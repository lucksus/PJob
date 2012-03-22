#pragma once
#include <QtCore/QHash>
inline uint qHash(const QHash<QString,double> hash)
{
	uint h=0;
	QString string;
	foreach(string, hash.keys())
		h += qHash(string) + hash[string];
	return h;
}