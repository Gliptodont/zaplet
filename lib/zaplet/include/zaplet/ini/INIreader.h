#ifndef INIreader_h
#define INIreader_h

#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstdio>

extern "C" {
    int ini_parse(const char* filename,
                  int (*handler)(void* user, const char* section,
                                 const char* name, const char* value),
                  void* user);

    int ini_parse_file(FILE* file,
                       int (*handler)(void* user, const char* section,
                                      const char* name, const char* value),
                       void* user);
}

class INIReader
{
public:
    INIReader(std::string filename);
    ~INIReader();

    int ParseError();

    std::string Get(std::string section, std::string name, std::string default_value);
    long GetInteger(std::string section, std::string name, long default_value);
    double GetReal(std::string section, std::string name, double default_value);
    bool GetBoolean(std::string section, std::string name, bool default_value);

    std::set<std::string> GetSections() const;
    std::set<std::string> GetFields(std::string section) const;

private:
    int _error;
    std::map<std::string, std::string> _values;
    std::set<std::string> _sections;
    std::map<std::string, std::set<std::string>*> _fields;

    static std::string MakeKey(std::string section, std::string name);
    static int ValueHandler(void* user, const char* section, const char* name, const char* value);
};

#endif // INIreader_h
