#pragma once

enum {
	BeginTag_MSG_C2S = 0X1000,
	MSG_C2S_ReportUID = 0X1001, // 上报角色的uid : RDStringInfo (wait : MSG_S2C_ReportUIDRe)
	EndTag_MSG_C2S = 0X3000,
}

enum {
	BeginTag_MSG_C2W = 0X4000,
	EndTag_MSG_C2W = 0X6000,
}

enum {
	BeginTag_MSG_S2C = 0X7000,
	EndTag_MSG_S2C = 0XB000,
}

