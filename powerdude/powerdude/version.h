#pragma once
#include <qobject.h>

class Version :public QObject
{
	Q_OBJECT
public:
	explicit Version() {};
	~Version() {};

	char *getVersion() {
		return powerdude_version;
	};
private:
	char *powerdude_version = "3.0.0";
};