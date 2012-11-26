#pragma once
#include <QtCore/QHash>
#include "PJobFileParameter.h"

inline uint qHash(const QHash<QString,double> hash)
{
	uint h=0;
	QString string;
	foreach(string, hash.keys())
		h += qHash(string) + hash[string];
	return h;
}

inline uint qHash(const QList<PJobFileParameter>& hash)
{
    uint h=0;
    PJobFileParameter p;
    foreach(p, hash)
        h += qHash(p.name()) + qHash(static_cast<qint64>(p.value()));
    return h;
}
