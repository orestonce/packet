#pragma once
#include "tinyxml2.h"
#include <assert.h>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

struct SItemDefine
{
    std::string type;
    std::string subtype;
    std::string variable;
    std::string mark;
};

struct SProtocolDefine
{
    std::vector<SItemDefine> items;
    std::string name;
    bool forClient;
    std::string csop;
    std::string mark;
    std::string waitmsg;
public :
    SProtocolDefine();
    void PushItem(const SItemDefine& iDefine);
};

struct SProtocolSection
{
    std::vector<SProtocolDefine> protocols;
    bool noop;
public :
    SProtocolSection();
};

struct SRpcdataDefine
{
    std::vector<SProtocolSection> sections;
    std::vector<std::string> basicTypes;
    std::vector<std::string> composeTypes;
public :
    SRpcdataDefine();
    void LoadFile(const char* fileName);

    void GenCSOPDotH(const char* fileName);
    void GenDotH(const char* fileName, bool forClient);
    void GenDotCpp(const char* fileName, const char* if1, const char* if2, bool forClient);

private:
    void GenInclude(FILE* fp, const char* includeFileName);

    void GenConstructor(FILE* fp, const SProtocolDefine& pDefine);

    void GenFuncClear(FILE* fp, const SProtocolDefine& pDefine);

    void GenFuncGetName(FILE* fp, const SProtocolDefine& pDefine);

    void GenFuncEncode(FILE* fp, const SProtocolDefine& pDefine);

    void GenBasicEncode(FILE* fp, const char*variable, const std::string& pname, const char* prefix);

    void GenBasicDecode(FILE* fp, const char*variable, const std::string& pname, const char* prefix);

    void GenFuncDecode(FILE* fp, const SProtocolDefine& pDefine);

    void LoadSection(const std::string& fileName, SProtocolSection& section);

    void LoadProtocol(tinyxml2::XMLElement* protocol, SProtocolSection &section);

    void check_pname_not_exists(const std::string& pname);

    void check_pname_exists(const std::string &pname, const std::string& subtype);

    bool IsComposeType(const std::string& pname);

    bool IsProtocolType(const std::string& pname);

    bool IsBasicType(const std::string& pname);

    bool HasComposeType(const SProtocolDefine& pDefine);
};

struct SCSOPItem
{
    std::string opname;
    std::string param;
    int op;
    std::string waitop;
    std::string mark;
};

class CSOPAllocator
{
public :
    CSOPAllocator(const std::string& tag, int opbegin, int opend, int sectionSize);
    void PushOp(const std::string& opname, const std::string& param, const std::string waitop, const std::string& mark);

    void NextSection();
    void GenEnumToFile(FILE *fp);
private:
    std::string tag;
    int opbegin;
    int opend;

    int curBegin;
    int sectionSize;

    int alloc;

    std::set<std::string> conflicts;
    std::vector<SCSOPItem> items;
};

struct SErrorItem
{
    std::string name;
    std::string mark;
};

struct SErrorSection
{
    std::vector<SErrorItem> items;
    std::string mark;
public :
    void Clear();
};

class CErrorData
{
public :

    void LoadFile(const char* fileName);
    void GenDotH(const char* fileName);
private:
    std::vector<SErrorSection> sections;
    std::set<std::string> conflicts;
};
