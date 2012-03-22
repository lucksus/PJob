#ifndef TOKNIZER_H
#define TOKNIZER_H

#include <string>
#include <vector>
#include <sstream>
#undef min

class token
{
public:
	token(const std::string& token,int signLength);

	void setName(const std::string& name);
	const std::string& getName() const;
	std::string& getName();

	void setValue(const std::string& value);
	const std::string& getValue() const;
	std::string& getValue();

private:
	std::string name;
	std::string value;
};

void FindTokens(const std::string& beginSign, const std::string& endSign, const std::string& tokenString, std::vector<token>& toks);
void FindString(const std::string& beginSign, const std::string& endSign, std::istream& istr, std::string& result);

template<typename T>
inline std::string to_string(const T& data)
{
	std::ostringstream tmp; tmp << data; return tmp.str();
}

template<typename T>
inline bool from_string(const std::string& s, T& data)
{
	std::istringstream stream(s); stream >> data;
	return !stream.fail() && !stream.bad();
}

template<>
inline bool from_string(const std::string& s, std::string& data)
{
	data = s;
	return true;
}

template<typename T> inline
bool ReadToken(std::vector< token >& tokens, const std::string& name, T& data)
{
	// somit hat der Parameter einen definierten Wert
	data = T();
	for (unsigned int i=0; i<tokens.size(); i++) {
		if (tokens[i].getName().compare(name) == 0) {
			from_string(tokens[i].getValue(), data);
			return true;
		}
	}
	return false;
}

//Lesen der Versions-Nummer
inline
bool ReadToken(std::vector< token >& tokens, const std::string& name, std::pair<unsigned int, unsigned int>& data)
{
	// somit hat der Parameter einen definierten Wert
	data = std::pair<unsigned int, unsigned int>(0,0);
	for (unsigned int i=0; i<tokens.size(); i++) {
		if (tokens[i].getName().compare(name) == 0) 
		{
			//Erste Zahl auslesen:
                       std::string::size_type pos = tokens[i].getValue().find('.');
			if(pos == std::string::npos) //Kein "." gefunden :-(
				return false;
			std::string sub_str = tokens[i].getValue().substr(0,pos);
			from_string(tokens[i].getValue(), data.first);
			sub_str = tokens[i].getValue().substr(pos+1,tokens[i].getValue().size()-1);
			//GPC::from_string(tokens[i].getValue(), data.second);
			from_string(sub_str, data.second);
			return true;
		}
	}
	return false;
}

template<typename T> inline
std::string WriteToken(const std::string& token_name, const T& token_value)
{
	return std::string("\t(#") + token_name + "," + to_string<T>(token_value) + "#)\n";
}

//Zum schreiben der Versions-Nummer
inline
std::string WriteToken(const std::string& token_name, const std::pair<unsigned int, unsigned int>& token_value)
{
	return std::string("\t(#") + token_name + "," + to_string<unsigned int>(token_value.first) + "." + to_string<unsigned int>(token_value.second) + "#)\n";
}

#endif
