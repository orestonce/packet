#include <iostream>
#include "tinyxml2.h"
#include <assert.h>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

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
public :
    void PushItem(const SItemDefine& iDefine)
    {
        for(std::vector<SItemDefine>::iterator it = items.begin(); it != items.end(); ++it)
        {
            assert(it->variable != iDefine.variable);
        }

        items.push_back(iDefine);
    }
};

struct SRpcdataDefine
{
    std::vector<SProtocolDefine> protocols;
    std::vector<std::string> basicTypes;
    std::vector<std::string> composeTypes;
public :
    SRpcdataDefine()
    {
        basicTypes = {"long", "int", "short", "byte", "string"};
        composeTypes = { "vector" };
    }

    void LoadFile(const char* fileName)
    {
        tinyxml2::XMLDocument doc;

        tinyxml2::XMLError err = doc.LoadFile(fileName);
        if( err != tinyxml2::XML_SUCCESS)
            assert(false);

        for(tinyxml2::XMLElement* rpcdata = doc.FirstChildElement();
            rpcdata != NULL; rpcdata = rpcdata->NextSiblingElement() )
        {
            for(tinyxml2::XMLElement* protocol = rpcdata->FirstChildElement();
                protocol != NULL; protocol = protocol->NextSiblingElement() )
            {
                LoadProtocol(protocol);
            }
        }
    }

    void GenDotH(const char* fileName, const char* includeFileName)
    {
        FILE* fp = fopen(fileName, "w");
        assert( fp );

        fprintf(fp, "#pragma once\n");
        fprintf(fp, "#include \"%s\"\n\n", includeFileName);
        fprintf(fp, "#include <string>\n");
        fprintf(fp, "#include <vector>\n");
        fprintf(fp, "class CPacket;\n\n");
        for(auto it = protocols.begin(); it != protocols.end(); ++it)
        {
            SProtocolDefine& pDefine = *it;
            fprintf(fp, "class %s\n", pDefine.name.c_str() );
            fprintf(fp, "{\n");
            fprintf(fp, "public :\n");
            fprintf(fp, "\t%s();\n", pDefine.name.c_str());
            fprintf(fp, "\tvoid Clear();\n");
            fprintf(fp, "\tconst char* GetName() const;\n");
            fprintf(fp, "\tvoid Encode(CPacket& p) const;\n");
            fprintf(fp, "\tvoid Decode(CPacket& p);\n");
            fprintf(fp, "public :\n");
            for(auto it = pDefine.items.begin(); it != pDefine.items.end(); ++it)
            {
                SItemDefine& iDefine = *it;

                const char* type = iDefine.type.c_str();
                const char* subtype = iDefine.subtype.c_str();
                const char* mark = iDefine.mark.c_str();
                const char* variable = iDefine.variable.c_str();

                if ( iDefine.type == "vector" )
                {
                    fprintf(fp, "\tstd::vector<%s> %s; // %s\n", subtype, variable, mark);
                }
                else if ( iDefine.type == "string" )
                {
                    fprintf(fp, "\tstd::string %s; // %s\n", variable, mark );
                }
                else if ( iDefine.type == "byte" )
                {
                    fprintf(fp, "\tchar %s; // %s\n", variable, mark);
                }
                else // long, int, short, protocols
                {
                    fprintf(fp, "\t%s %s; // %s\n",
                        type, iDefine.variable.c_str(), iDefine.mark.c_str());
                }
            }
            fprintf(fp, "};\n\n");
        }

        fclose(fp);
    }

    void GenDotCpp(const char* fileName, const char* if1, const char* if2)
    {
        FILE* fp = fopen(fileName, "w");
        assert(fp);

        GenInclude(fp, if1);
        GenInclude(fp, if2);
        fprintf(fp, "\n");
        fprintf(fp, "#include <stdexcept>\n\n");

        for(auto it = protocols.begin(); it != protocols.end(); ++it)
        {
            const SProtocolDefine& pDefine = *it;
            GenConstructor(fp, pDefine);
            GenFuncClear(fp, pDefine);
            GenFuncGetName(fp, pDefine);
            GenFuncEncode(fp, pDefine);
            GenFuncDecode(fp, pDefine);
        }

        fclose(fp);
    }

private:
    void GenInclude(FILE* fp, const char* includeFileName)
    {
        fprintf(fp, "#include \"%s\"\n", includeFileName);
    }

    void GenConstructor(FILE* fp, const SProtocolDefine& pDefine)
    {
        fprintf(fp, "%s::%s()\n", pDefine.name.c_str(), pDefine.name.c_str());
        fprintf(fp, "{\n");
        fprintf(fp, "\tClear();\n");
        fprintf(fp, "}\n\n");
    }

    void GenFuncClear(FILE* fp, const SProtocolDefine& pDefine)
    {
        fprintf(fp, "void %s::Clear()\n", pDefine.name.c_str());
        fprintf(fp, "{\n");
        for(auto it = pDefine.items.begin(); it != pDefine.items.end(); ++it)
        {
            const SItemDefine& iDefine = *it;
            const char* variable = iDefine.variable.c_str();

            if ( IsBasicType(iDefine.type) )
            {
                if ( iDefine.type != "string" )
                {
                    fprintf(fp, "\t%s = 0; \n", variable);
                }
                else
                {
                    fprintf(fp, "\t%s.clear(); \n", variable);
                }
            }
            else if (IsProtocolType(iDefine.type))
            {
                fprintf(fp, "\t%s.Clear(); \n", variable);
            }
            else if (IsComposeType(iDefine.type))
            {
                fprintf(fp, "\t%s.clear(); \n", variable);
            }
            else
            {
                assert(false);
            }
        }
        fprintf(fp, "}\n\n");
    }

    void GenFuncGetName(FILE* fp, const SProtocolDefine& pDefine)
    {
        fprintf(fp, "const char* %s::GetName() const\n", pDefine.name.c_str());
        fprintf(fp, "{\n");
        fprintf(fp, "\treturn \"%s\";\n", pDefine.name.c_str());
        fprintf(fp, "}\n\n");
    }

    void GenFuncEncode(FILE* fp, const SProtocolDefine& pDefine)
    {
        fprintf(fp, "void %s::Encode(CPacket& p) const\n", pDefine.name.c_str());
        fprintf(fp, "{\n");

        for(auto it = pDefine.items.begin(); it != pDefine.items.end(); ++it)
        {
            const SItemDefine& iDefine = *it;
            const char* variable = iDefine.variable.c_str();

            if ( IsProtocolType(iDefine.type) )
            {
                fprintf(fp, "\t%s.Encode(p);\n",variable );
            }
            else if ( IsBasicType(iDefine.type) )
            {
                GenBasicEncode(fp, iDefine.variable.c_str(), iDefine.type, "");
            }
            else if ( IsComposeType(iDefine.type) )
            {
                if ( iDefine.type == "vector" )
                {
                    const char* subtype = iDefine.subtype.c_str();

                    fprintf(fp, "\t{\n");
                    fprintf(fp, "\t\tp.WriteInt(%s.size());\n", variable);
                    fprintf(fp, "\t\tfor(auto it = %s.begin(); it != %s.end(); ++it)\n", variable, variable);
                    fprintf(fp, "\t\t{\n");
                    fprintf(fp, "\t\t\tconst %s& temp = *it;\n", subtype);
                    if ( IsBasicType(iDefine.subtype) )
                    {
                        GenBasicEncode(fp, "temp", iDefine.subtype, "\t\t");
                    }
                    else if ( IsProtocolType(iDefine.subtype) )
                    {
                        fprintf(fp, "\t\t\ttemp.Encode(p);\n");
                    }
                    else
                    {
                        assert(false);
                    }
                    fprintf(fp, "\t\t}\n");
                    fprintf(fp, "\t}\n");
                }
                else
                {
                    assert(false);
                }
            }
        }
        fprintf(fp, "\tp.Flush(); // 在包头部写入包大小信息\n");
        fprintf(fp, "}\n\n");
    }

    void GenBasicEncode(FILE* fp, const char*variable, const std::string& pname, const char* prefix)
    {
        if ( pname == "long")
        {
            fprintf(fp, "%s\tp.WriteLong(%s); \n", prefix, variable);
        }
        else if ( pname == "int" )
        {
            fprintf(fp, "%s\tp.WriteInt(%s); \n", prefix, variable);
        }
        else if ( pname == "short" )
        {
            fprintf(fp, "%s\tp.WriteShort(%s); \n",prefix, variable);
        }
        else if ( pname == "byte" )
        {
            fprintf(fp, "%s\tp.WriteByte(%s); \n",prefix, variable);
        }
        else if ( pname == "string" )
        {
            fprintf(fp, "%s\t{\n", prefix);
            fprintf(fp, "%s\t\tint len = (int) %s.size();\n", prefix, variable);
            fprintf(fp, "%s\t\tp.WriteInt(len); \n", prefix);
            fprintf(fp, "%s\t\tif (len > 0)\n", prefix);
            fprintf(fp, "%s\t\t{\n", prefix);
            fprintf(fp, "%s\t\t\tp.WriteByteArray(%s.c_str(), len); \n",prefix, variable);
            fprintf(fp, "%s\t\t}\n", prefix);
            fprintf(fp, "%s\t}\n", prefix);
        }
        else
        {
            assert(false);
        }
    }

    void GenBasicDecode(FILE* fp, const char*variable, const std::string& pname, const char* prefix)
    {
        if ( pname == "long" )
        {
            fprintf(fp, "%s\t%s = p.ReadLong(); \n", prefix, variable);
        }
        else if ( pname == "int" )
        {
            fprintf(fp, "%s\t%s = p.ReadInt(); \n", prefix, variable);
        }
        else if ( pname == "short" )
        {
            fprintf(fp, "%s\t%s = p.ReadShort(); \n",prefix, variable);
        }
        else if ( pname == "byte" )
        {
            fprintf(fp, "%s\t%s = p.ReadByte(); \n",prefix, variable);
        }
        else if ( pname == "string" )
        {
            fprintf(fp, "%s\t{\n", prefix);
            fprintf(fp, "%s\t\tint len = 0;\n", prefix);
            fprintf(fp, "%s\t\tlen = p.ReadInt(); \n", prefix);
            fprintf(fp, "%s\t\tif ( len < 0 || len > CPacket::PACKET_MAX_SIZE ) \n", prefix);
            fprintf(fp, "%s\t\t\tthrow std::runtime_error(\"length of %s invalid !\"); \n", prefix, variable);
            fprintf(fp, "%s\t\tif ( len > 0 )\n",prefix);
            fprintf(fp, "%s\t\t{\n",prefix);
            fprintf(fp, "%s\t\t\t%s.resize(len); \n", prefix, variable);
            fprintf(fp, "%s\t\t\tp.ReadByteArray( &(%s[0]), len); \n",prefix, variable);
            fprintf(fp, "%s\t\t}\n", prefix);
            fprintf(fp, "%s\t}\n", prefix);
        }
        else
        {
            assert(false);
        }
    }

    void GenFuncDecode(FILE* fp, const SProtocolDefine& pDefine)
    {
        fprintf(fp, "void %s::Decode(CPacket& p)\n", pDefine.name.c_str());
        fprintf(fp, "{\n");
        for(auto it = pDefine.items.begin(); it != pDefine.items.end(); ++it)
        {
            const SItemDefine& iDefine = *it;
            const char* variable = iDefine.variable.c_str();

            if( IsProtocolType(iDefine.type) )
            {
                fprintf(fp, "\t%s.Decode(p);\n",variable );
            }
            else if ( IsBasicType(iDefine.type) )
            {
                GenBasicDecode(fp, iDefine.variable.c_str(), iDefine.type, "");
            }
            else if ( IsComposeType(iDefine.type) )
            {
                if ( iDefine.type == "vector" )
                {
                    const char* subtype = iDefine.subtype.c_str();

                    fprintf(fp, "\t{\n");
                    fprintf(fp, "\t\tint sz = p.ReadInt();\n");
                    fprintf(fp, "\t\tfor(int i = 0; i < sz; ++i)\n");
                    fprintf(fp, "\t\t{\n");
                    fprintf(fp, "\t\t\t%s temp;\n", subtype);
                    if ( IsBasicType(iDefine.subtype) )
                    {
                        GenBasicDecode(fp, "temp", iDefine.subtype, "\t\t");
                    }
                    else if ( IsProtocolType(iDefine.subtype) )
                    {
                        fprintf(fp, "\t\t\ttemp.Decode(p);\n");
                    }
                    else
                    {
                        assert(false);
                    }
		fprintf(fp, "\t\t\t%s.push_back(temp);\n", variable);
                    fprintf(fp, "\t\t}\n");
                    fprintf(fp, "\t}\n");
                }
                else
                {
                    assert(false);
                }
            }
            else
            {
                assert(false);
            }
        }
        fprintf(fp, "}\n\n");
    }

    void LoadProtocol(tinyxml2::XMLElement* protocol)
    {
        std::string pname = protocol->Attribute("name");
        check_pname_not_exists( pname );

        SProtocolDefine pDefine;

        pDefine.name = pname;

        for(tinyxml2::XMLElement* item = protocol->FirstChildElement();
            item != NULL ; item = item->NextSiblingElement() )
        {
            SItemDefine iDefine;

            iDefine.type = item->Attribute("type");
            iDefine.mark = item->Attribute("mark");
            if ( iDefine.type == "vector" )
            {
                iDefine.subtype = item->Attribute("subtype");
            }

            iDefine.variable = item->Attribute("variable");

            check_pname_exists(iDefine.type, iDefine.subtype);

            pDefine.PushItem( iDefine );
        }

        protocols.push_back(pDefine);
    }

    void check_pname_not_exists(const std::string& pname)
    {
        if ( IsBasicType(pname) || IsComposeType(pname) || IsProtocolType(pname))
            assert(false);
    }

    void check_pname_exists(const std::string &pname, const std::string& subtype)
    {
        if( IsBasicType(pname) || IsProtocolType(pname) )
            return ;

        if (IsComposeType(pname) && !subtype.empty() )
        {
            check_pname_exists(subtype, "");
        }
        else
        {
            assert(false);
        }
    }

    bool IsComposeType(const std::string& pname)
    {
        auto it = std::find(composeTypes.begin(), composeTypes.end(), pname);
        return it != composeTypes.end();
    }

    bool IsProtocolType(const std::string& pname)
    {
        for(auto it = protocols.begin(); it != protocols.end(); ++it)
        {
            const SProtocolDefine& pDefine = *it;

            if ( pDefine.name == pname)
                return true;
        }

        return false;
    }

    bool IsBasicType(const std::string& pname)
    {
        auto it = std::find(basicTypes.begin(), basicTypes.end(), pname);
        return it != basicTypes.end();
    }

    bool HasComposeType(const SProtocolDefine& pDefine)
    {
        for(auto it = pDefine.items.begin(); it != pDefine.items.end(); ++it)
        {
            if ( IsComposeType(it->type) )
                return true;
        }

        return false;
    }
};

int main()
{
    SRpcdataDefine rpcdata;

    rpcdata.LoadFile("./rpcdata.xml");
    rpcdata.GenDotH("../rpcdata.h", "localmacro.h");
    rpcdata.GenDotCpp("../rpcdata.cpp", "rpcdata.h", "packet.h");

    return 0;
}


