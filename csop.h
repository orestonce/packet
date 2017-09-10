#pragma once

enum {
	BeginTag_MSG_C2S = 0X1000,
	MSG_C2S_ReportUID2 = 0X1031, // 上报角色的uid2 : RDStringInfo2 (wait : MSG_S2C_ReportUID2Re)
	MSG_C2S_ReportUID3 = 0X1061, // 上报角色的uid3 : RDStringInfo3 (wait : MSG_S2C_ReportUID3Re)
	EndTag_MSG_C2S = 0X3000,
}

enum {
	BeginTag_MSG_C2W = 0X4000,
	EndTag_MSG_C2W = 0X6000,
}

enum {
	BeginTag_MSG_S2C = 0X7000,
	MSG_S2C_ReportUID3Re = 0X7061, // 上报角色的uid3 : RDStringInfo4
	EndTag_MSG_S2C = 0XB000,
}

