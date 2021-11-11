//////////////////////////////////////////
/*
* Copyright (c) 2020-2021 Perchik71 <email:perchik71@outlook.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
//////////////////////////////////////////

#pragma once

#include "TESFile.h"
#include "TESForm.h"

#include "BGLayer.h"
#include "TESCell.h"
#include "TESObjectREFR.h"

#include <vector>

namespace api {

	struct ModList {
		BSSimpleList<TESFile> modInfoList;		// 00
		BSTArray<TESFile*> loadedMods;			// 10
	};

	class TESDataFileHandler {
	public:
		using UnkArray = BSTArray<LPVOID>;
		using UnkFormArray = BSTArray<TESForm*>;
		using TESFileList = BSSimpleList<TESFile>;
		using TESFileArray = BSTArray<TESFile*>;
	private:
		INT64 unk00;							// 00
		UnkArray unk08;							// 08
		UnkArray unk20;							// 20
		UnkArray unk38;							// 38
		UnkArray unk50;							// 50
	public:
		UnkFormArray arrNONE;					// 68 Form Type 0
		UnkFormArray arrTES4;					// 80 Form Type 1
		UnkFormArray arrGRUP;					// 98 Form Type 2
		UnkFormArray arrGMST;					// B0 Form Type 3
		UnkFormArray arrKYWD;					// C8 Form Type 4
		UnkFormArray arrLCRT;					// E0 Form Type 5
		UnkFormArray arrAACT;					// F8 Form Type 6
		UnkFormArray arrTRNS;					// 110 Form Type 7
		UnkFormArray arrCMPO;					// 128 Form Type 8
		UnkFormArray arrTXST;					// 140 Form Type 9
		UnkFormArray arrMICN;					// 158 Form Type 10
		UnkFormArray arrGLOB;					// 170 Form Type 11
		UnkFormArray arrDMGT;					// 188 Form Type 12
		UnkFormArray arrCLAS;					// 1A0 Form Type 13
		UnkFormArray arrFACT;					// 1B8 Form Type 14
		UnkFormArray arrHDPT;					// 1D0 Form Type 15
		UnkFormArray arrEYES;					// 1E8 Form Type 16
		UnkFormArray arrRACE;					// 200 Form Type 17
		UnkFormArray arrSOUN;					// 218 Form Type 18
		UnkFormArray arrASPC;					// 230 Form Type 19
		UnkFormArray arrSKIL;					// 248 Form Type 20
		UnkFormArray arrMGEF;					// 260 Form Type 21
		UnkFormArray arrSCPT;					// 278 Form Type 22
		UnkFormArray arrLTEX;					// 290 Form Type 23
		UnkFormArray arrENCH;					// 2A8 Form Type 24
		UnkFormArray arrSPEL;					// 2C0 Form Type 25
		UnkFormArray arrSCRL;					// 2D8 Form Type 26
		UnkFormArray arrACTI;					// 2F0 Form Type 27
		UnkFormArray arrTACT;					// 308 Form Type 28
		UnkFormArray arrARMO;					// 320 Form Type 29
		UnkFormArray arrBOOK;					// 338 Form Type 30
		UnkFormArray arrCONT;					// 350 Form Type 31
		UnkFormArray arrDOOR;					// 368 Form Type 32
		UnkFormArray arrINGR;					// 380 Form Type 33
		UnkFormArray arrLIGH;					// 398 Form Type 34
		UnkFormArray arrMISC;					// 3B0 Form Type 35
		UnkFormArray arrSTAT;					// 3C8 Form Type 36
		UnkFormArray arrSCOL;					// 3E0 Form Type 37
		UnkFormArray arrMSTT;					// 3F8 Form Type 38
		UnkFormArray arrGRAS;					// 410 Form Type 39
		UnkFormArray arrTREE;					// 428 Form Type 40
		UnkFormArray arrFLOR;					// 440 Form Type 41
		UnkFormArray arrFURN;					// 458 Form Type 42
		UnkFormArray arrWEAP;					// 470 Form Type 43
		UnkFormArray arrAMMO;					// 488 Form Type 44
		UnkFormArray arrNPC_;					// 4A0 Form Type 45
		UnkFormArray arrLVLN;					// 4B8 Form Type 46
		UnkFormArray arrKEYM;					// 4D0 Form Type 47
		UnkFormArray arrALCH;					// 4E8 Form Type 48
		UnkFormArray arrIDLM;					// 500 Form Type 49
		UnkFormArray arrNOTE;					// 518 Form Type 50
		UnkFormArray arrPROJ;					// 530 Form Type 51
		UnkFormArray arrHAZD;					// 548 Form Type 52
		UnkFormArray arrBNDS;					// 560 Form Type 53
		UnkFormArray arrSLGM;					// 578 Form Type 54
		UnkFormArray arrTERM;					// 590 Form Type 55
		UnkFormArray arrLVLI;					// 5A8 Form Type 56
		UnkFormArray arrWTHR;					// 5C0 Form Type 57
		UnkFormArray arrCLMT;					// 5D8 Form Type 58
		UnkFormArray arrSPGD;					// 5F0 Form Type 59
		UnkFormArray arrRFCT;					// 608 Form Type 60
		UnkFormArray arrREGN;					// 620 Form Type 61
		UnkFormArray arrNAVI;					// 638 Form Type 62
		BSTArray<TESCell*> arrCELL;				// 650 Form Type 63
		BSTArray<TESObjectREFR*> arrREFR;		// 668 Form Type 64
		UnkFormArray arrACHR;					// 680 Form Type 65
		UnkFormArray arrPMIS;					// 698 Form Type 66
		UnkFormArray arrPARW;					// 6B0 Form Type 67
		UnkFormArray arrPGRE;					// 6C8 Form Type 68
		UnkFormArray arrPBEA;					// 6E0 Form Type 69
		UnkFormArray arrPFLA;					// 6F8 Form Type 70
		UnkFormArray arrPCOM;					// 710 Form Type 71
		UnkFormArray arrPBAR;					// 728 Form Type 72
		UnkFormArray arrPHZD;					// 740 Form Type 73
		UnkFormArray arrWRLD;					// 758 Form Type 74
		UnkFormArray arrLAND;					// 770 Form Type 75
		UnkFormArray arrNAVM;					// 788 Form Type 76
		UnkFormArray arrTLOD;					// 7A0 Form Type 77
		UnkFormArray arrDIAL;					// 7B8 Form Type 78
		UnkFormArray arrINFO;					// 7D0 Form Type 79
		UnkFormArray arrQUST;					// 7E8 Form Type 80
		UnkFormArray arrIDLE;					// 800 Form Type 81
		UnkFormArray arrPACK;					// 818 Form Type 82
		UnkFormArray arrCSTY;					// 830 Form Type 83
		UnkFormArray arrLSCR;					// 848 Form Type 84
		UnkFormArray arrLVSP;					// 860 Form Type 85
		UnkFormArray arrANIO;					// 878 Form Type 86
		UnkFormArray arrWATR;					// 890 Form Type 87
		UnkFormArray arrEFSH;					// 8A8 Form Type 88
		UnkFormArray arrTOFT;					// 8C0 Form Type 89
		UnkFormArray arrEXPL;					// 8D8 Form Type 90
		UnkFormArray arrDEBR;					// 8F0 Form Type 91
		UnkFormArray arrIMGS;					// 908 Form Type 92
		UnkFormArray arrIMAD;					// 920 Form Type 93
		UnkFormArray arrFLST;					// 938 Form Type 94
		UnkFormArray arrPERK;					// 950 Form Type 95
		UnkFormArray arrBPTD;					// 968 Form Type 96
		UnkFormArray arrADDN;					// 980 Form Type 97
		UnkFormArray arrAVIF;					// 998 Form Type 98
		UnkFormArray arrCAMS;					// 9B0 Form Type 99
		UnkFormArray arrCPTH;					// 9C8 Form Type 100
		UnkFormArray arrVTYP;					// 9E0 Form Type 101
		UnkFormArray arrMATT;					// 9F8 Form Type 102
		UnkFormArray arrIPCT;					// A10 Form Type 103
		UnkFormArray arrIPDS;					// A28 Form Type 104
		UnkFormArray arrARMA;					// A40 Form Type 105
		UnkFormArray arrECZN;					// A58 Form Type 106
		UnkFormArray arrLCTN;					// A70 Form Type 107
		UnkFormArray arrMESG;					// A88 Form Type 108
		UnkFormArray arrRGDL;					// AA0 Form Type 109
		UnkFormArray arrDOBJ;					// AB8 Form Type 110
		UnkFormArray arrDFOB;					// AD0 Form Type 111
		UnkFormArray arrLGTM;					// AE8 Form Type 112
		UnkFormArray arrMUSC;					// B00 Form Type 113
		UnkFormArray arrFSTP;					// B18 Form Type 114
		UnkFormArray arrFSTS;					// B30 Form Type 115
		UnkFormArray arrSMBN;					// B48 Form Type 116
		UnkFormArray arrSMQN;					// B60 Form Type 117
		UnkFormArray arrSMEN;					// B78 Form Type 118
		UnkFormArray arrDLBR;					// B90 Form Type 119
		UnkFormArray arrMUST;					// BA8 Form Type 120
		UnkFormArray arrDLVW;					// BC0 Form Type 121
		UnkFormArray arrWOOP;					// BD8 Form Type 122
		UnkFormArray arrSHOU;					// BF0 Form Type 123
		UnkFormArray arrEQUP;					// C08 Form Type 124
		UnkFormArray arrRELA;					// C20 Form Type 125
		UnkFormArray arrSCEN;					// C38 Form Type 126
		UnkFormArray arrASTP;					// C50 Form Type 127
		UnkFormArray arrOTFT;					// C68 Form Type 128
		UnkFormArray arrARTO;					// C80 Form Type 129
		UnkFormArray arrMATO;					// C98 Form Type 130
		UnkFormArray arrMOVT;					// CB0 Form Type 131
		UnkFormArray arrSNDR;					// CC8 Form Type 132
		UnkFormArray arrDUAL;					// CE0 Form Type 133
		UnkFormArray arrSNCT;					// CF8 Form Type 134
		UnkFormArray arrSOPM;					// D10 Form Type 135
		UnkFormArray arrCOLL;					// D28 Form Type 136
		UnkFormArray arrCLFM;					// D40 Form Type 137
		UnkFormArray arrREVB;					// D58 Form Type 138
		UnkFormArray arrPKIN;					// D70 Form Type 139
		UnkFormArray arrRFGP;					// D88 Form Type 140
		UnkFormArray arrAMDL;					// DA0 Form Type 141
		BSTArray<BGLayer*> arrLAYR;				// DB8 Form Type 142
		UnkFormArray arrCOBJ;					// DD0 Form Type 143
		UnkFormArray arrOMOD;					// DE8 Form Type 144
		UnkFormArray arrMSWP;					// E00 Form Type 145
		UnkFormArray arrZOOM;					// E18 Form Type 146
		UnkFormArray arrINNR;					// E30 Form Type 147
		UnkFormArray arrKSSM;					// E48 Form Type 148
		UnkFormArray arrAECH;					// E60 Form Type 149
		UnkFormArray arrSCCO;					// E78 Form Type 150
		UnkFormArray arrAORU;					// E90 Form Type 151
		UnkFormArray arrSCSN;					// EA8 Form Type 152
		UnkFormArray arrSTAG;					// EC0 Form Type 153
		UnkFormArray arrNOCM;					// ED8 Form Type 154
		UnkFormArray arrLENS;					// EF0 Form Type 155
		UnkFormArray arrLSPR;					// F08 Form Type 156
		UnkFormArray arrGDRY;					// F20 Form Type 157
		UnkFormArray arrOVIS;					// F38 Form Type 158
	private:
		LPVOID UnkPtrF50;						// F50
	public:
		NiTArray<TESCell*> cellList;			// F58
		NiTArray<TESForm*> addonNodes;			// F70			
	private:
		DWORD64 unkF88;							// F88
		DWORD64 unkF90;							// F90
		DWORD64 unkF98;							// F98
		DWORD unkFA0;							// FA0 - FormID?
		DWORD unkFA4;							// FA4
		TESFile* activePlugin;					// FA8
	public:
		ModList modList;						// FB0
	public:
		bool Load(int Unknown);
		bool InitUnknownDataSetTextStatusBar(void);
	public:
		inline const TESFileList* GetMods(VOID) const { return &modList.modInfoList; }
		inline const TESFileArray* GetLoadedMods(VOID) const { return &modList.loadedMods; }
		inline TESFile* GetActiveMod(VOID) const { return activePlugin; }
		inline BOOL IsActiveMod(VOID) const { return activePlugin != NULL; }
		bool IsLoaded(void) const;
	public:
		static VOID Initialize(VOID);
	public:
		READ_PROPERTY(GetMods) TESFileList* Mods;
		READ_PROPERTY(GetLoadedMods) const TESFileArray* LoadedMods;
		READ_PROPERTY(GetActiveMod) TESFile* ActiveMod;
	};

	extern TESDataFileHandler* FileHandler;
}