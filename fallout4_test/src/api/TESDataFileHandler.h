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
#include "BSTArray.h"
#include "BSTList.h"

#include <vector>

namespace api {
	class TESDataFileHandler
	{
	public:
		using UnkArray = BSTArray<void*>;
		using UnkFormArray = BSTArray<TESForm*>;
		using TESFileList = BSSimpleList<TESFile*>;
		using TESFileListPtr = TESFileList*;
		using TESFileArray = std::vector<TESFile*>;
	private:
		INT64 unk00;			// 00
		UnkArray unk08;			// 08
		UnkArray unk20;			// 20
		UnkArray unk38;			// 38
		UnkArray unk50;			// 50
		UnkFormArray arrNONE;	// Form Type 0
		UnkFormArray arrTES4;	// Form Type 1
		UnkFormArray arrGRUP;	// Form Type 2
		UnkFormArray arrGMST;	// Form Type 3
		UnkFormArray arrKYWD;	// Form Type 4
		UnkFormArray arrLCRT;	// Form Type 5
		UnkFormArray arrAACT;	// Form Type 6
		UnkFormArray arrTRNS;	// Form Type 7
		UnkFormArray arrCMPO;	// Form Type 8
		UnkFormArray arrTXST;	// Form Type 9
		UnkFormArray arrMICN;	// Form Type 10
		UnkFormArray arrGLOB;	// Form Type 11
		UnkFormArray arrDMGT;	// Form Type 12
		UnkFormArray arrCLAS;	// Form Type 13
		UnkFormArray arrFACT;	// Form Type 14
		UnkFormArray arrHDPT;	// Form Type 15
		UnkFormArray arrEYES;	// Form Type 16
		UnkFormArray arrRACE;	// Form Type 17
		UnkFormArray arrSOUN;	// Form Type 18
		UnkFormArray arrASPC;	// Form Type 19
		UnkFormArray arrSKIL;	// Form Type 20
		UnkFormArray arrMGEF;	// Form Type 21
		UnkFormArray arrSCPT;	// Form Type 22
		UnkFormArray arrLTEX;	// Form Type 23
		UnkFormArray arrENCH;	// Form Type 24
		UnkFormArray arrSPEL;	// Form Type 25
		UnkFormArray arrSCRL;	// Form Type 26
		UnkFormArray arrACTI;	// Form Type 27
		UnkFormArray arrTACT;	// Form Type 28
		UnkFormArray arrARMO;	// Form Type 29
		UnkFormArray arrBOOK;	// Form Type 30
		UnkFormArray arrCONT;	// Form Type 31
		UnkFormArray arrDOOR;	// Form Type 32
		UnkFormArray arrINGR;	// Form Type 33
		UnkFormArray arrLIGH;	// Form Type 34
		UnkFormArray arrMISC;	// Form Type 35
		UnkFormArray arrSTAT;	// Form Type 36
		UnkFormArray arrSCOL;	// Form Type 37
		UnkFormArray arrMSTT;	// Form Type 38
		UnkFormArray arrGRAS;	// Form Type 39
		UnkFormArray arrTREE;	// Form Type 40
		UnkFormArray arrFLOR;	// Form Type 41
		UnkFormArray arrFURN;	// Form Type 42
		UnkFormArray arrWEAP;	// Form Type 43
		UnkFormArray arrAMMO;	// Form Type 44
		UnkFormArray arrNPC_;	// Form Type 45
		UnkFormArray arrLVLN;	// Form Type 46
		UnkFormArray arrKEYM;	// Form Type 47
		UnkFormArray arrALCH;	// Form Type 48
		UnkFormArray arrIDLM;	// Form Type 49
		UnkFormArray arrNOTE;	// Form Type 50
		UnkFormArray arrPROJ;	// Form Type 51
		UnkFormArray arrHAZD;	// Form Type 52
		UnkFormArray arrBNDS;	// Form Type 53
		UnkFormArray arrSLGM;	// Form Type 54
		UnkFormArray arrTERM;	// Form Type 55
		UnkFormArray arrLVLI;	// Form Type 56
		UnkFormArray arrWTHR;	// Form Type 57
		UnkFormArray arrCLMT;	// Form Type 58
		UnkFormArray arrSPGD;	// Form Type 59
		UnkFormArray arrRFCT;	// Form Type 60
		UnkFormArray arrREGN;	// Form Type 61
		UnkFormArray arrNAVI;	// Form Type 62
		UnkFormArray arrCELL;	// Form Type 63
		UnkFormArray arrREFR;	// Form Type 64
		UnkFormArray arrACHR;	// Form Type 65
		UnkFormArray arrPMIS;	// Form Type 66
		UnkFormArray arrPARW;	// Form Type 67
		UnkFormArray arrPGRE;	// Form Type 68
		UnkFormArray arrPBEA;	// Form Type 69
		UnkFormArray arrPFLA;	// Form Type 70
		UnkFormArray arrPCOM;	// Form Type 71
		UnkFormArray arrPBAR;	// Form Type 72
		UnkFormArray arrPHZD;	// Form Type 73
		UnkFormArray arrWRLD;	// Form Type 74
		UnkFormArray arrLAND;	// Form Type 75
		UnkFormArray arrNAVM;	// Form Type 76
		UnkFormArray arrTLOD;	// Form Type 77
		UnkFormArray arrDIAL;	// Form Type 78
		UnkFormArray arrINFO;	// Form Type 79
		UnkFormArray arrQUST;	// Form Type 80
		UnkFormArray arrIDLE;	// Form Type 81
		UnkFormArray arrPACK;	// Form Type 82
		UnkFormArray arrCSTY;	// Form Type 83
		UnkFormArray arrLSCR;	// Form Type 84
		UnkFormArray arrLVSP;	// Form Type 85
		UnkFormArray arrANIO;	// Form Type 86
		UnkFormArray arrWATR;	// Form Type 87
		UnkFormArray arrEFSH;	// Form Type 88
		UnkFormArray arrTOFT;	// Form Type 89
		UnkFormArray arrEXPL;	// Form Type 90
		UnkFormArray arrDEBR;	// Form Type 91
		UnkFormArray arrIMGS;	// Form Type 92
		UnkFormArray arrIMAD;	// Form Type 93
		UnkFormArray arrFLST;	// Form Type 94
		UnkFormArray arrPERK;	// Form Type 95
		UnkFormArray arrBPTD;	// Form Type 96
		UnkFormArray arrADDN;	// Form Type 97
		UnkFormArray arrAVIF;	// Form Type 98
		UnkFormArray arrCAMS;	// Form Type 99
		UnkFormArray arrCPTH;	// Form Type 100
		UnkFormArray arrVTYP;	// Form Type 101
		UnkFormArray arrMATT;	// Form Type 102
		UnkFormArray arrIPCT;	// Form Type 103
		UnkFormArray arrIPDS;	// Form Type 104
		UnkFormArray arrARMA;	// Form Type 105
		UnkFormArray arrECZN;	// Form Type 106
		UnkFormArray arrLCTN;	// Form Type 107
		UnkFormArray arrMESG;	// Form Type 108
		UnkFormArray arrRGDL;	// Form Type 109
		UnkFormArray arrDOBJ;	// Form Type 110
		UnkFormArray arrDFOB;	// Form Type 111
		UnkFormArray arrLGTM;	// Form Type 112
		UnkFormArray arrMUSC;	// Form Type 113
		UnkFormArray arrFSTP;	// Form Type 114
		UnkFormArray arrFSTS;	// Form Type 115
		UnkFormArray arrSMBN;	// Form Type 116
		UnkFormArray arrSMQN;	// Form Type 117
		UnkFormArray arrSMEN;	// Form Type 118
		UnkFormArray arrDLBR;	// Form Type 119
		UnkFormArray arrMUST;	// Form Type 120
		UnkFormArray arrDLVW;	// Form Type 121
		UnkFormArray arrWOOP;	// Form Type 122
		UnkFormArray arrSHOU;	// Form Type 123
		UnkFormArray arrEQUP;	// Form Type 124
		UnkFormArray arrRELA;	// Form Type 125
		UnkFormArray arrSCEN;	// Form Type 126
		UnkFormArray arrASTP;	// Form Type 127
		UnkFormArray arrOTFT;	// Form Type 128
		UnkFormArray arrARTO;	// Form Type 129
		UnkFormArray arrMATO;	// Form Type 130
		UnkFormArray arrMOVT;	// Form Type 131
		UnkFormArray arrSNDR;	// Form Type 132
		UnkFormArray arrDUAL;	// Form Type 133
		UnkFormArray arrSNCT;	// Form Type 134
		UnkFormArray arrSOPM;	// Form Type 135
		UnkFormArray arrCOLL;	// Form Type 136
		UnkFormArray arrCLFM;	// Form Type 137
		UnkFormArray arrREVB;	// Form Type 138
		UnkFormArray arrPKIN;	// Form Type 139
		UnkFormArray arrRFGP;	// Form Type 140
		UnkFormArray arrAMDL;	// Form Type 141
		UnkFormArray arrLAYR;	// Form Type 142
		UnkFormArray arrCOBJ;	// Form Type 143
		UnkFormArray arrOMOD;	// Form Type 144
		UnkFormArray arrMSWP;	// Form Type 145
		UnkFormArray arrZOOM;	// Form Type 146
		UnkFormArray arrINNR;	// Form Type 147
		UnkFormArray arrKSSM;	// Form Type 148
		UnkFormArray arrAECH;	// Form Type 149
		UnkFormArray arrSCCO;	// Form Type 150
		UnkFormArray arrAORU;	// Form Type 151
		UnkFormArray arrSCSN;	// Form Type 152
		UnkFormArray arrSTAG;	// Form Type 153
		UnkFormArray arrNOCM;	// Form Type 154
		UnkFormArray arrLENS;	// Form Type 155
		UnkFormArray arrLSPR;	// Form Type 156
		UnkFormArray arrGDRY;	// Form Type 157
		UnkFormArray arrOVIS;	// Form Type 158

	public:
		bool Load(int Unknown);
		bool InitUnknownDataSetTextStatusBar(void);
		static void DetectSelectFile(TESFile* File);
		static void EndLoadEvent_SendDone(int32_t index, LPCSTR message);
		static void AttachBA2File(LPCSTR _filename, LPCSTR _folder = NULL);
	public:
		static TESFileListPtr GetArchiveFiles(void);
		static TESFileArray* GetSelectedFiles(void);
		TESFile* GetActiveFile(void) const;
		bool IsActiveFile(void) const;
		bool IsLoaded(void) const;
	public:
		static void Initialize(void);
	public:
		READ_PROPERTY(GetArchiveFiles) TESFileListPtr ArchiveFiles;
		READ_PROPERTY(GetSelectedFiles) TESFileArray* SelectedFiles;
		READ_PROPERTY(GetActiveFile) TESFile* ActiveFile;
	};

	extern TESDataFileHandler* FileHandler;
}