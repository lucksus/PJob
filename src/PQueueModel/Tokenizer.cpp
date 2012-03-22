#include "Tokenizer.h"
#include <assert.h>
using namespace std;

token::token(const string& token,int signLength) {
	// Token: (Name,Value)
	int kommaPos = token.find(",");
	name = token.substr(signLength,kommaPos-signLength);
	value = token.substr(kommaPos+1,token.size()-kommaPos-1-signLength);
}

void token::setName(const string& name) {
	this->name = name;
}

string& token:: getName() {
	return name;
}

const string& token:: getName() const {
	return name;
}

void token::setValue(const string& value) {
	this->value = value;
}

const string& token::getValue() const {
	return value;
}

string& token::getValue() {
	return value;
}

void FindTokens(const string& beginSign, const string& endSign, const string& tokenString, vector<token>& toks)
{
	if (tokenString.empty())
		return;

	if (beginSign.size() != endSign.size())
                throw "Tokenizer assignment missmatch\n void FindTokens(const string& beginSign, const string& endSign, const string& OrgTokenString, vector<token>& toks)";

        std::string::size_type beginPos;
        std::string::size_type endPos;
	int itr_pos = 0;

	while ((beginPos = tokenString.find(beginSign,itr_pos)) != string::npos) {
		// beginSign gefunden, jetzt nach endSign suchen (Muss vorhanden sein! Tom: Und wenn nicht? Dann lief das hier in eine Endlosschleife...)
		int signCounter = 1;
		int startSearchPos = beginPos;
                std::string::size_type nextBeginPos;
                std::string::size_type nextEndPos;

		do {
			nextBeginPos = tokenString.find(beginSign,startSearchPos+beginSign.size());
			nextEndPos   = tokenString.find(endSign,  startSearchPos+beginSign.size());
			if (nextEndPos == string::npos) {
				//Wir haben zu dem beginSign kein endSign gefunden. Das ist schlecht
				std::stringstream msg;
				msg << "Tokenizer could not find end sign (\"" << endSign << "\") for begin sign \"" << beginSign << "\". Does one of the global parameters or component parameters contain \"" << beginSign << "\" or \"" << endSign << "\" or other PHOTOSS-reserved tokens?";
				throw msg.str();
			}
			if ((nextBeginPos == string::npos) || (nextEndPos < nextBeginPos)) {
				signCounter--;
				startSearchPos = nextEndPos;
				continue;
			}

			signCounter++;
			startSearchPos = nextBeginPos;
		} while (signCounter > 0);

		endPos = nextEndPos;

		// Neues Token anlegen
		// end und begin sign werden mit gespeichert im Token
		toks.push_back(
			token(tokenString.substr( beginPos,endPos-beginPos+endSign.size() ),beginSign.size())
						);
		itr_pos = endPos+endSign.size();
	}
}

void FindString(const std::string& beginSign, const std::string& endSign, istream& istr, string& result)
{
	string test;

	if ((istr.bad()!=0) || (istr.eof()!=0)) return;

	// istream muss ignoriert werden bis 'VectorParameter {' gelesen wird
	// Zaehler ist auf 1 gesetzt
	do istr >> result;
	
//	if ((istr.bad()!=0) || (istr.eof()!=0)) return;

	while ((result.compare(beginSign)!=0) && (istr.bad()==0) && (istr.eof()==0));

	if ((istr.bad()!=0) || (istr.eof()!=0)) return;

	int Counter = 1;
	// hier koennte man noch den buffer entleren muesste aber auch so gehen
//	buffer = "";

	// solange in den string einlesen bis korrespondierendes } gefunden ist
	// { gefunden Zaehler incrementieren
	// } gefunden Zaehler dekrementieren
	// wenn Zaehler == 0 ist Feierabend
	do
	{
		// white spaces werden ueberlesen,
		// daher ACHTUNG an Keno kommt der Tokenizer damit klar?
		//istr >> test;
		// besser ganze Zeile lesen
		getline(istr, test);
		// Testen
//		if (test.compare("{") == 0)
		if (test.find(beginSign) != string::npos)
			Counter++;
//		if (test.compare("}") == 0)
		if (test.find(endSign) != string::npos)
			Counter--;
		result += test + "\n";	// MW: Das hier habe ich eingefügt. Der Tokenizer entfernte nämlich früher alle Umbrüche aus dem String
//		buffer += "\n";
	}
	while ( (Counter>0) && (istr.bad()==0) && (istr.eof()==0) );
	assert(Counter==0); //Sonst haben wir einen Syntax Fehler in der .pho Datei
}
