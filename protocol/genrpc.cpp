#include "genrpc.h"

SProtocolDefine::SProtocolDefine()
{
    forClient = false;
}

void SProtocolDefine::PushItem(const SItemDefine &iDefine)
{
    for(std::vector<SItemDefine>::iterator it = items.begin(); it != items.end(); ++it)
    {
        assert(it->variable != iDefine.variable);
    }

    items.push_back(iDefine);
}

SRpcdataDefine::SRpcdataDefine()
{
    basicTypes = {"long", "int", "short", "byte", "string"};
    composeTypes = { "vector" };
}

void SRpcdataDefine::LoadFile(const char *fileName)
{
    tinyxml2::XMLDocument doc;

    tinyxml2::XMLError err = doc.LoadFile(fileName);
    if( err != tinyxml2::XML_SUCCESS)
        assert(false);

    for(tinyxml2::XMLElement* rpcdata = doc.FirstChildElement();
        rpcdata != NULL; rpcdata = rpcdata->NextSiblingElement() )
    {
        std::string sectionpath = rpcdata->Attribute("sectionpath");

        for(tinyxml2::XMLElement* sectiondef = rpcdata->FirstChildElement();
            sectiondef != NULL; sectiondef = sectiondef->NextSiblingElement() )
        {
            SProtocolSection section;

            const char* szNoop = sectiondef->Attribute("noop");
            if ( szNoop && strncmp(szNoop, "true", strlen("true"))== 0 )
            {
                section.noop = true;
            }

            sections.push_back(section);

            LoadSection(sectionpath + sectiondef->Attribute("file"), sections.back());

        }
    }
}

void SRpcdataDefine::GenCSOPDotH(const char *fileName)
{
    FILE* fp = fopen(fileName, "w");
    assert(fp);

    fprintf(fp, "#pragma once\n\n");
    // op defines :
    //MSG_C2S_xxx : 0x1000 ~ 0x3000 : 8192个协议
    //MSG_C2W_xxx : 0x4000 ~ 0x6000 : 8192个协议
    //MSG_S2C_xxx : 0x7000 ~ 0xB000 : 8192 * 2 个协议
    //服务器内部协议: 0xC000 ~ 0xE000 : 8192个协议

    const char* szC2SPrefix = "MSG_C2S";
    const char* szC2WPrefix = "MSG_C2W";
    const char* szS2CPrefix = "MSG_S2C";

    CSOPAllocator c2s(szC2SPrefix, 0x1000, 0x3000, 0x0030);
    CSOPAllocator c2w(szC2WPrefix, 0x4000, 0x6000, 0x0030);
    CSOPAllocator s2c(szS2CPrefix, 0x7000, 0xB000, 0x0030);

    for(auto it=sections.begin(); it != sections.end(); ++it)
    {
        SProtocolSection& section = *it;
        if ( section.noop )
            continue;

        c2s.NextSection();
        c2w.NextSection();
        s2c.NextSection();

        for(auto it2 = section.protocols.begin(); it2 != section.protocols.end(); ++it2)
        {
            SProtocolDefine& pDefine = *it2;

            if ( pDefine.forClient != true)
                continue;

            if ( pDefine.csop.empty() )
                continue;

            const char* csop = pDefine.csop.c_str();

            if ( 0 == strncmp(szC2SPrefix, csop, strlen(szC2SPrefix)) )
            {
                c2s.PushOp(pDefine.csop,pDefine.name, pDefine.waitmsg, pDefine.mark);
            }
            else if ( 0 == strncmp(szC2WPrefix, csop, strlen(szC2WPrefix)) )
            {
                c2w.PushOp(pDefine.csop,pDefine.name, pDefine.waitmsg, pDefine.mark);
            }
            else if ( 0 == strncmp(szS2CPrefix, csop, strlen(szS2CPrefix)) )
            {
                s2c.PushOp(pDefine.csop,pDefine.name, pDefine.waitmsg, pDefine.mark);
            }
            else
            {
                assert(false);
            }
        }
    }

    c2s.GenEnumToFile(fp);
    c2w.GenEnumToFile(fp);
    s2c.GenEnumToFile(fp);

    fclose(fp);
}

void SRpcdataDefine::GenDotH(const char *fileName, bool forClient)
{
    FILE* fp = fopen(fileName, "w");
    assert( fp );

    fprintf(fp, "#pragma once\n");
    fprintf(fp, "#include <string>\n");
    fprintf(fp, "#include <vector>\n");
    fprintf(fp, "class CPacket;\n\n");
    for(auto it = sections.begin(); it != sections.end(); ++it)
    {
        for(auto it2=it->protocols.begin(); it2 != it->protocols.end(); ++it2)
        {
            SProtocolDefine& pDefine = *it2;

            if ( forClient && pDefine.forClient != true)
                continue;

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
    }

    fclose(fp);
}

void SRpcdataDefine::GenDotCpp(const char *fileName, const char *if1, const char *if2, bool forClient)
{
    FILE* fp = fopen(fileName, "w");
    assert(fp);

    GenInclude(fp, if1);
    GenInclude(fp, if2);
    fprintf(fp, "\n");
    fprintf(fp, "#include <stdexcept>\n\n");

    for(auto it = sections.begin(); it != sections.end(); ++it)
    {
        for(auto it2 = it->protocols.begin(); it2 != it->protocols.end(); ++it2)
        {
            const SProtocolDefine& pDefine = *it2;
            if ( forClient && pDefine.forClient != true )
                continue;

            GenConstructor(fp, pDefine);
            GenFuncClear(fp, pDefine);
            GenFuncGetName(fp, pDefine);
            GenFuncEncode(fp, pDefine);
            GenFuncDecode(fp, pDefine);
        }
    }

    fclose(fp);
}

void SRpcdataDefine::GenInclude(FILE *fp, const char *includeFileName)
{
    fprintf(fp, "#include \"%s\"\n", includeFileName);
}

void SRpcdataDefine::GenConstructor(FILE *fp, const SProtocolDefine &pDefine)
{
    fprintf(fp, "%s::%s()\n", pDefine.name.c_str(), pDefine.name.c_str());
    fprintf(fp, "{\n");
    fprintf(fp, "\tClear();\n");
    fprintf(fp, "}\n\n");
}

void SRpcdataDefine::GenFuncClear(FILE *fp, const SProtocolDefine &pDefine)
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

void SRpcdataDefine::GenFuncGetName(FILE *fp, const SProtocolDefine &pDefine)
{
    fprintf(fp, "const char* %s::GetName() const\n", pDefine.name.c_str());
    fprintf(fp, "{\n");
    fprintf(fp, "\treturn \"%s\";\n", pDefine.name.c_str());
    fprintf(fp, "}\n\n");
}

void SRpcdataDefine::GenFuncEncode(FILE *fp, const SProtocolDefine &pDefine)
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

void SRpcdataDefine::GenBasicEncode(FILE *fp, const char *variable, const std::string &pname, const char *prefix)
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

void SRpcdataDefine::GenBasicDecode(FILE *fp, const char *variable, const std::string &pname, const char *prefix)
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

void SRpcdataDefine::GenFuncDecode(FILE *fp, const SProtocolDefine &pDefine)
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

void SRpcdataDefine::LoadSection(const std::string &fileName, SProtocolSection &section)
{
    tinyxml2::XMLDocument doc;

    tinyxml2::XMLError err = doc.LoadFile(fileName.c_str());
    if( err != tinyxml2::XML_SUCCESS)
        assert(false);

    for(tinyxml2::XMLElement* rpcdata = doc.FirstChildElement();
        rpcdata != NULL; rpcdata = rpcdata->NextSiblingElement() )
    {
        for(tinyxml2::XMLElement* protocol = rpcdata->FirstChildElement();
            protocol != NULL; protocol = protocol->NextSiblingElement() )
        {
            LoadProtocol(protocol, section);
        }
    }
}

void SRpcdataDefine::LoadProtocol(tinyxml2::XMLElement *protocol, SProtocolSection& section)
{
    std::string pname = protocol->Attribute("name");
    check_pname_not_exists( pname );

    SProtocolDefine pDefine;

    pDefine.name = pname;

    do {
        const char* szMark = protocol->Attribute("mark");
        if ( szMark )
        {
            pDefine.mark = szMark;
        }

        const char* szForClient = protocol->Attribute("forclient");
        if ( !szForClient || 0 != strcasecmp("true", szForClient) )
            break;

        pDefine.forClient = true;
        const char* szCsop = protocol->Attribute("csop");
        if ( szCsop )
        {
            pDefine.csop = szCsop;
        }

        const char* szWaitMsg = protocol->Attribute("waitmsg");
        if ( szWaitMsg )
        {
            pDefine.waitmsg = szWaitMsg;
        }

    } while ( false );

    const char* szMark = protocol->Attribute("mark");
    if ( szMark )
    {
        pDefine.mark = szMark;
    }

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

    section.protocols.push_back(pDefine);
}

void SRpcdataDefine::check_pname_not_exists(const std::string &pname)
{
    if ( IsBasicType(pname) || IsComposeType(pname) || IsProtocolType(pname))
        assert(false);
}

void SRpcdataDefine::check_pname_exists(const std::string &pname, const std::string &subtype)
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

bool SRpcdataDefine::IsComposeType(const std::string &pname)
{
    auto it = std::find(composeTypes.begin(), composeTypes.end(), pname);
    return it != composeTypes.end();
}

bool SRpcdataDefine::IsProtocolType(const std::string &pname)
{
    for(auto it = sections.begin(); it != sections.end(); ++it)
    {
        for(auto it2 = it->protocols.begin(); it2 != it->protocols.end(); ++it2)
        {
            const SProtocolDefine& pDefine = *it2;

            if ( pDefine.name == pname)
                return true;
        }
    }

    return false;
}

bool SRpcdataDefine::IsBasicType(const std::string &pname)
{
    auto it = std::find(basicTypes.begin(), basicTypes.end(), pname);
    return it != basicTypes.end();
}

bool SRpcdataDefine::HasComposeType(const SProtocolDefine &pDefine)
{
    for(auto it = pDefine.items.begin(); it != pDefine.items.end(); ++it)
    {
        if ( IsComposeType(it->type) )
            return true;
    }

    return false;
}

CSOPAllocator::CSOPAllocator(const std::string &tag, int opbegin, int opend, int sectionSize)
{
    this->tag = tag;
    this->opbegin = opbegin;
    this->opend = opend;
    alloc = opbegin;
    this->sectionSize = sectionSize;
    curBegin = opbegin;

    assert(sectionSize > 0 );

    assert( opbegin < opend);
}

void CSOPAllocator::PushOp(const std::string &opname, const std::string &param, const std::string waitop, const std::string &mark)
{
    assert( conflicts.find(opname) == conflicts.end());
    conflicts.insert( opname);

    ++alloc;
    assert(curBegin < alloc && alloc < curBegin + sectionSize);

    SCSOPItem item;

    item.opname = opname;
    item.param = param;
    item.waitop = waitop;
    item.mark = mark;
    item.op = alloc;

    items.push_back(item);
}

void CSOPAllocator::NextSection()
{
    curBegin += sectionSize;
    alloc = curBegin;
    assert(opbegin < alloc && alloc < opend);
}

void CSOPAllocator::GenEnumToFile(FILE* fp)
{
    assert(fp);

    fprintf(fp, "enum {\n");
    fprintf(fp, "\tBeginTag_%s = %#04X,\n", tag.c_str(), opbegin);
    for(auto it = items.begin(); it != items.end(); ++it)
    {
        SCSOPItem& iDefine = *it;
        if ( iDefine.waitop.empty() )
            fprintf(fp, "\t%s = %#04X, // %s : %s\n", iDefine.opname.c_str(), iDefine.op, iDefine.mark.c_str(), iDefine.param.c_str());
        else
            fprintf(fp, "\t%s = %#04X, // %s : %s (wait : %s)\n",
                    iDefine.opname.c_str(), iDefine.op, iDefine.mark.c_str(), iDefine.param.c_str(), iDefine.waitop.c_str() );
    }
    fprintf(fp, "\tEndTag_%s = %#04X,\n", tag.c_str(), opend);
    fprintf(fp, "}\n\n");
}

SProtocolSection::SProtocolSection()
{
    noop = false;
}

void CErrorData::LoadFile(const char *fileName)
{
    tinyxml2::XMLDocument doc;

    tinyxml2::XMLError err = doc.LoadFile(fileName);
    if( err != tinyxml2::XML_SUCCESS)
        assert(false);

    for(tinyxml2::XMLElement* errordata = doc.FirstChildElement();
        errordata != NULL; errordata = errordata->NextSiblingElement() )
    {
        SErrorSection section;
        int cnt = 0;

        for(tinyxml2::XMLElement* item = errordata->FirstChildElement();
            item != NULL; item = item->NextSiblingElement())
        {
            if ( item->Name() == std::string("section") )
            {
                if ( cnt > 0 )
                {
                    sections.push_back(section);
                }

                section.Clear();

                section.mark = item->Attribute("mark");
                ++cnt;
            }
            else if ( item->Name() == std::string("item") )
            {
                SErrorItem eitem;

                eitem.name = item->Attribute("name");
                eitem.mark = item->Attribute("mark");
                assert( conflicts.find(eitem.name) == conflicts.end() );

                conflicts.insert(eitem.name);
                section.items.push_back(eitem);
            }
            else
            {
                assert(false);
            }
        }

        if ( cnt > 0 )
        {
            sections.push_back(section);
        }
    }
}

void CErrorData::GenDotH(const char *fileName)
{
    FILE* fp = fopen(fileName, "w");

    fprintf(fp, "#pragma once\n\n");
    fprintf(fp, "enum {\n");

    int id = 1;

    for(auto it = sections.begin(); it != sections.end(); ++it)
    {
        SErrorSection& s = *it;

        int end = id + 300;

        fprintf(fp, "\n\t//-------%s-----\n", s.mark.c_str());

        for(auto it2 = s.items.begin(); it2 != s.items.end(); ++it2)
        {
            SErrorItem& item = *it2;

            fprintf(fp, "\t%s = %d; // %s \n", item.name.c_str(), id, item.mark.c_str() );
            id ++;
            assert(id < end);
        }
        id = end;
    }

    fprintf(fp, "};\n");
    fclose(fp);
}

void SErrorSection::Clear()
{
    this->items.clear();
    this->mark.clear();
}
