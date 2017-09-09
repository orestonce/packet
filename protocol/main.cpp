#include <iostream>
#include "genrpc.h"

int main()
{
    SRpcdataDefine rpcdata;

    rpcdata.LoadFile("rpcdata.xml");
    rpcdata.GenDotH("../rpcdata-client.h", true);
    rpcdata.GenDotCpp("../rpcdata-client.cpp", "rpcdata-client.h", "packet.h", true);
    rpcdata.GenDotH("../rpcdata-server.h", false);
    rpcdata.GenDotCpp("../rpcdata-server.cpp", "rpcdata-server.h", "packet.h", false);
    rpcdata.GenCSOPDotH("../csop.h");

    return 0;
}

