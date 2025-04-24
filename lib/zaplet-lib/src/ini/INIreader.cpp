/*
* Code written by Максим Пискарёв "gliptodont"
 */

#include "zaplet/ini/INIreader.h"
#include <cstring>

#define MAX_SECTION 50
#define MAX_NAME 50
#define INI_MAX_LINE 200

static char* rstrip(char* s)
{
    char* p = s + strlen(s);
    while (p > s && isspace((unsigned char)(*--p)))
        *p = '\0';
    return s;
}

static char* lskip(const char* s)
{
    while (*s && isspace((unsigned char)(*s)))
        s++;
    return (char*)s;
}

static char* find_char_or_comment(const char* s, char c)
{
    int was_whitespace = 0;
    while (*s && *s != c && !(was_whitespace && *s == ';')) {
        was_whitespace = isspace((unsigned char)(*s));
        s++;
    }
    return (char*)s;
}

static char* strncpy0(char* dest, const char* src, size_t size)
{
    strncpy(dest, src, size);
    dest[size - 1] = '\0';
    return dest;
}

int ini_parse_file(FILE* file,
                   int (*handler)(void*, const char*, const char*,const char*),
                   void* user)
{
    char line[INI_MAX_LINE];
    char section[MAX_SECTION] = "";
    char prev_name[MAX_NAME] = "";

    char* start;
    char* end;
    char* name;
    char* value;
    int lineno = 0;
    int error = 0;

    while (fgets(line, INI_MAX_LINE, file) != NULL) {
        lineno++;

        start = line;
        if (lineno == 1 && (unsigned char)start[0] == 0xEF &&
                           (unsigned char)start[1] == 0xBB &&
                           (unsigned char)start[2] == 0xBF) {
            start += 3;
        }

        start = lskip(rstrip(start));

        if (*start == ';' || *start == '#') {
            // comment
        }
        else if (*prev_name && *start && start > line) {
            if (!handler(user, section, prev_name, start) && !error)
                error = lineno;
        }
        else if (*start == '[') {
            end = find_char_or_comment(start + 1, ']');
            if (*end == ']') {
                *end = '\0';
                strncpy0(section, start + 1, sizeof(section));
                *prev_name = '\0';
            }
            else if (!error) {
                error = lineno;
            }
        }
        else if (*start && *start != ';') {
            end = find_char_or_comment(start, '=');
            if (*end != '=')
                end = find_char_or_comment(start, ':');

            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = rstrip(start);
                value = lskip(end + 1);
                end = find_char_or_comment(value, '\0');
                if (*end == ';')
                    *end = '\0';
                rstrip(value);
                strncpy0(prev_name, name, sizeof(prev_name));

                if (!handler(user, section, name, value) && !error)
                    error = lineno;
            }
            else if (!error) {
                error = lineno;
            }
        }

        if (error)
            break;
    }

    return error;
}

int ini_parse(const char* filename,
              int (*handler)(void*, const char*, const char*, const char*),
              void* user)
{
    FILE* file = fopen(filename, "r");
    if (!file)
        return -1;
    int error = ini_parse_file(file, handler, user);
    fclose(file);
    return error;
}

// --- INIReader implementation ---

INIReader::INIReader(std::string filename)
{
    _error = ini_parse(filename.c_str(), ValueHandler, this);
}

INIReader::~INIReader()
{
    for (auto& pair : _fields)
        delete pair.second;
}

int INIReader::ParseError()
{
    return _error;
}

std::string INIReader::Get(std::string section, std::string name, std::string default_value)
{
    std::string key = MakeKey(section, name);
    return _values.count(key) ? _values[key] : default_value;
}

long INIReader::GetInteger(std::string section, std::string name, long default_value)
{
    std::string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    long n = strtol(value, &end, 0);
    return end > value ? n : default_value;
}

double INIReader::GetReal(std::string section, std::string name, double default_value)
{
    std::string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    double n = strtod(value, &end);
    return end > value ? n : default_value;
}

bool INIReader::GetBoolean(std::string section, std::string name, bool default_value)
{
    std::string valstr = Get(section, name, "");
    std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
    if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
        return true;
    if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
        return false;
    return default_value;
}

std::set<std::string> INIReader::GetSections() const
{
    return _sections;
}

std::set<std::string> INIReader::GetFields(std::string section) const
{
    std::string sectionKey = section;
    std::transform(sectionKey.begin(), sectionKey.end(), sectionKey.begin(), ::tolower);
    auto it = _fields.find(sectionKey);
    if (it == _fields.end())
        return {};
    return *it->second;
}

std::string INIReader::MakeKey(std::string section, std::string name)
{
    std::string key = section + "=" + name;
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

int INIReader::ValueHandler(void* user, const char* section, const char* name, const char* value)
{
    INIReader* reader = (INIReader*)user;
    std::string key = MakeKey(section, name);

    if (reader->_values[key].size() > 0)
        reader->_values[key] += "\n";
    reader->_values[key] += value;

    reader->_sections.insert(section);

    std::string sectionKey = section;
    std::transform(sectionKey.begin(), sectionKey.end(), sectionKey.begin(), ::tolower);

    auto& fieldMap = reader->_fields;
    std::set<std::string>* fieldsSet;
    auto it = fieldMap.find(sectionKey);
    if (it == fieldMap.end())
    {
        fieldsSet = new std::set<std::string>();
        fieldMap[sectionKey] = fieldsSet;
    }
    else
    {
        fieldsSet = it->second;
    }

    fieldsSet->insert(name);
    return 1;
}
