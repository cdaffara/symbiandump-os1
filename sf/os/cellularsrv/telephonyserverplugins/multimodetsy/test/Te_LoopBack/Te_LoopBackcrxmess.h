// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalComponent 
*/

#ifndef __CRXMESS_H__
#define __CRXMESS_H__

#include "Te_LoopbackTestStepBase.h"
#include "Te_LoopBackScriptEng.h"

const TUint8 KTestPduA[]= {
						   0x04, 0x0c, 0x91, 0x44, 0x77, 0x74, 0x60, 0x45, 0x61, 0x00, 0x00, 0x00, 0x30,
						   0x71, 0x61, 0x03, 0x52, 0x00, 0x0a, 0xd4, 0xf2, 0x9c, 0x9e, 0x76, 0x9f, 0x5d,
						   0x2e, 0x17 };


const TUint8 KTestPduAWithSca[]= {
						   0x0E, 0x91, 0x44, 0x77, 0x58, 0x10, 0x24, 0x80, 0x24,
						   0x04, 0x0c, 0x91, 0x44, 0x77, 0x74, 0x60, 0x45, 0x61, 0x00, 0x00, 0x00, 0x30,
						   0x71, 0x61, 0x03, 0x52, 0x00, 0x0a, 0xd4, 0xf2, 0x9c, 0x9e, 0x76, 0x9f, 0x5d,
						   0x2e, 0x17 };

const TUint8 KTestPduB[]= { 0x07, 0x91, 0x44, 0x87, 0x20, 0x90, 0x02, 0x53, 0x04, 0x10, 0xd0, 0x42, 0xea,
							0xb0, 0xcc, 0x66, 0xbb, 0xcb, 0x74, 0x00, 0x00, 0x00, 0x30, 0x80, 0x41, 0x03,
							0x23, 0x00, 0x88, 0xc7, 0xb2, 0x3b, 0x5d, 0x06, 0x25, 0xdd, 0xf4, 0xb2, 0xdc,
							0x5d, 0xa6, 0x83, 0xe0, 0xf2, 0x77, 0xfb, 0x4d, 0x4f, 0xbf, 0xdd, 0x3a, 0x90,
							0x51, 0x5e, 0x2e, 0x83, 0xae, 0x41, 0x28, 0x28, 0xe9, 0xa6, 0x97, 0xe5, 0xee,
							0x32, 0x1d, 0x04, 0x47, 0xBF, 0xDD, 0x65, 0x90, 0xB8, 0x9C, 0x76, 0x9F, 0x41,
							0xE7, 0xB4, 0xBD, 0xEC, 0x06, 0x85, 0xEF, 0xE1, 0x3C, 0xA8, 0x6C, 0x2F, 0xCB,
							0xF3, 0x20, 0x72, 0x38, 0x0F, 0x4A, 0xBB, 0x41, 0xCD, 0xB0, 0x7C, 0x8C, 0x76,
							0x81, 0xAC, 0xE9, 0x79, 0x9A, 0x0E, 0x02, 0xA1, 0xE9, 0x74, 0xB8, 0xEE, 0xF5,
							0x7A, 0x9B, 0xCD, 0x65, 0xF9, 0xDC, 0x75, 0x2E, 0xBB, 0xD3, 0x65, 0xD7, 0xF8,
							0xED, 0xAA, 0xAF, 0x5F, 0xF7, 0x30, 0x1C, 0x64, 0x7E, 0xCB, 0x41, 0xE4, 0x32,
							0x3D, 0x9C, 0x66, 0xCF, 0x5D };

const TUint8 KTestPduBWithSca[]= {
							0x0E, 0x91, 0x44, 0x77, 0x58, 0x10, 0x24, 0x80, 0x24,
							0x07, 0x91, 0x44, 0x87, 0x20, 0x90, 0x02, 0x53, 0x04, 0x10, 0xd0, 0x42, 0xea,
							0xb0, 0xcc, 0x66, 0xbb, 0xcb, 0x74, 0x00, 0x00, 0x00, 0x30, 0x80, 0x41, 0x03,
							0x23, 0x00, 0x88, 0xc7, 0xb2, 0x3b, 0x5d, 0x06, 0x25, 0xdd, 0xf4, 0xb2, 0xdc,
							0x5d, 0xa6, 0x83, 0xe0, 0xf2, 0x77, 0xfb, 0x4d, 0x4f, 0xbf, 0xdd, 0x3a, 0x90,
							0x51, 0x5e, 0x2e, 0x83, 0xae, 0x41, 0x28, 0x28, 0xe9, 0xa6, 0x97, 0xe5, 0xee,
							0x32, 0x1d, 0x04, 0x47, 0xBF, 0xDD, 0x65, 0x90, 0xB8, 0x9C, 0x76, 0x9F, 0x41,
							0xE7, 0xB4, 0xBD, 0xEC, 0x06, 0x85, 0xEF, 0xE1, 0x3C, 0xA8, 0x6C, 0x2F, 0xCB,
							0xF3, 0x20, 0x72, 0x38, 0x0F, 0x4A, 0xBB, 0x41, 0xCD, 0xB0, 0x7C, 0x8C, 0x76,
							0x81, 0xAC, 0xE9, 0x79, 0x9A, 0x0E, 0x02, 0xA1, 0xE9, 0x74, 0xB8, 0xEE, 0xF5,
							0x7A, 0x9B, 0xCD, 0x65, 0xF9, 0xDC, 0x75, 0x2E, 0xBB, 0xD3, 0x65, 0xD7, 0xF8,
							0xED, 0xAA, 0xAF, 0x5F, 0xF7, 0x30, 0x1C, 0x64, 0x7E, 0xCB, 0x41, 0xE4, 0x32,
							0x3D, 0x9C, 0x66, 0xCF, 0x5D };


const TUint8 KTestPduC[]= {
							0x24, 0x0C, 0x91, 0x44, 0x77, 0x74, 0x60, 0x45, 0x61, 0x00,
							0x00, 0x00, 0x50, 0x80, 0x81, 0x03, 0x03, 0x40, 0x52, 0x2E,
							0xD0, 0x0A, 0x74, 0x02, 0xA9, 0x40, 0x27, 0x90, 0x0B, 0xB4,
							0x02, 0xB9, 0x40, 0x27, 0x90, 0x0A, 0x74, 0x02, 0xAD, 0x5C,
							0x20, 0x15, 0xE8, 0x04, 0x5A, 0x81, 0x5C, 0xC8, 0x30, 0x1C,
							0x9E, 0x77, 0x81, 0x56, 0xA0, 0x13, 0x48, 0x05, 0x72, 0x81,
							0x44, 0x20, 0x95, 0xB3, 0x7C, 0x07, 0x65, 0xCB, 0x61, 0xB9,
							0x0A, 0x24, 0x02, 0xA9, 0x40, 0x22, 0xD0, 0x0A, 0xE4, 0x02,
							0xAD, 0x40, 0x2A, 0xD0, 0x0A, 0xE4, 0x02, 0xAD, 0x40, 0x22,
							0x15 };

const TUint8 KTestPduDWithSca[]= {
							0x0B, 0x91, 0x44, 0x83, 0x05, 0x41, 0x03, 0xF6,
							0x24, 0x0C, 0x91, 0x44, 0x77, 0x74, 0x60, 0x45, 0x61, 0x00,
							0x00, 0x00, 0x50, 0x80, 0x81, 0x92, 0x02, 0x40, 0xA0, 0xD3,
							0x37, 0xE8, 0x5E, 0x3E, 0xB1, 0xD9, 0xA0, 0xF3, 0x1B, 0xE4,
							0x7E, 0x83, 0xDA, 0x6F, 0x79, 0x19, 0x14, 0x96, 0xBF, 0xED,
							0x69, 0xF7, 0x99, 0x05, 0x9A, 0xBF, 0x41, 0xEC, 0x30, 0xBD,
							0x0C, 0x4A, 0xBB, 0xE9, 0x6F, 0x10, 0x1D, 0x5D, 0x06, 0xB9,
							0xD3, 0x67, 0x34, 0x9D, 0x05, 0xA2, 0xA3, 0xDF, 0xF5, 0x33,
							0x1A, 0x44, 0x47, 0x97, 0x41, 0xE8, 0x72, 0x58, 0x4E, 0x07,
							0x89, 0xCB, 0xA0, 0x39, 0x3D, 0xCD, 0x66, 0x83, 0xC2, 0x73,
							0x10, 0xFB, 0x6D, 0x4F, 0xBB, 0xCF, 0xA0, 0xB0, 0x9B, 0x0C,
							0xA2, 0xA3, 0xCB, 0xA0, 0xF6, 0xFB, 0xED, 0x06, 0x89, 0xCB,
							0xA0, 0x39, 0x3D, 0xCD, 0x66, 0x83, 0xC2, 0x73, 0x90, 0x58,
							0x9E, 0x3E, 0xA3, 0xE9, 0x2E, 0x90, 0xF1, 0x2D, 0x07, 0xD1,
							0xD1, 0x65, 0xD0, 0xFC, 0xFE, 0x96, 0x93, 0x41, 0xEF, 0x3A,
							0xFD, 0x5E, 0x0E, 0xCB, 0xE7, 0xA0, 0x34, 0x7D, 0x0E, 0x9A,
							0xA3, 0xCB, 0x61, 0x3A, 0x9A, 0x05, 0x0A, 0xBB, 0xC9 };

const TUint8 KTestPduE[]= { 0x04, 0x0c, 0x91, 0x44, 0x77, 0x74, 0x60, 0x45, 0x61, 0x00, 0x00, 0x00, 0x30,
						   0x71, 0x61, 0x03, 0x52, 0x00, 0x0a, 0xd4, 0xf2, 0x9c, 0x9e, 0x76, 0x9f, 0x5d,
						   0x2e, 0x17 };

const TUint KTestMsgRefE=27;

const struct TScript KRxMessScript[]= {
	{	ERxString,	(TText8*)"AT\r",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EIfRxStringJmp, (TText8*)"AT\r",	1, 0	},

	{	ERxString, (TText8*)"AT+FCLASS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"0,1,2\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//
// The initialisation string - we don't care what it is
//
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=,0",   0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS7?",		0, ERxEvent	},
	{	ETxString, (TText8*)"050\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS0=0",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERICSSON\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMM",	0, ERxEvent	},
	{	ETxString, (TText8*)"1100801\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMR",	0, ERxEvent	},
	{	ETxString, (TText8*)"9808261341\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGSN",	0, ERxEvent	},
	{	ETxString, (TText8*)"520002512954000\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CIMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CMGF: (0)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: (\"ME\",\"SM\"),(\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: \"SM\",0,15,\"SM\",0,15\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPBS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=2",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=1",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG?",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"AT+CRC=1",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBC",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CSQ: 31,99\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGQREQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGATT",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CNMI: (0-2),(0,1,3),(0),(0),(0,1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBST=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CBST: (0-7,12,14-16,34,36,38,39,43,47-51,65,66,68,70,71,75,79-81),(0,2),(1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSCA?",		0, ERxEvent	},
	{	ETxString, (TText8*)"+CSCA: \"441632960000\",145",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGREG=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT*ECAM=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

// Init Complete

	{	ERxString, (TText8*)"AT+CNMI=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CNMI: (0-2),(0,1,3),(0),(0),(0,1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=2,1",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,							1,	ETimeOutEvent	},
	{	ETxString, (TText8*)"\r\n+CMTI: \"ME\",1",	0, ETxEvent|EFailIfTimeOut	},

//
// Expect a request to set the Prefered SMS Storage area....
//
	{	ERxString, (TText8*)"AT+CPMS=\"ME\"",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: 1,15,0,15",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,						0, ETxEvent|EFailIfTimeOut	},
//
// ...and a request to read the new SMS
//
	{	ERxString, (TText8*)"AT+CMGR=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CMGR: 0,28",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"040C914477746045610000003071610352000AD4F29C9E769F5D2E17\r",
								0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,							1,	ETimeOutEvent	},
	{	EComplete,	 NULL,							0, 0	}
	};

const struct TScript KRxMessCmtScript[]= {
	{	ERxString,	(TText8*)"AT\r",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EIfRxStringJmp, (TText8*)"AT\r",	1, 0	},

	{	ERxString, (TText8*)"AT+FCLASS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"0,1,2\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//
// The initialisation string - we don't care what it is
//
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=,0",   0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS7?",		0, ERxEvent	},
	{	ETxString, (TText8*)"050\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS0=0",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"Nokia\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMM",	0, ERxEvent	},
	{	ETxString, (TText8*)"1100801\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMR",	0, ERxEvent	},
	{	ETxString, (TText8*)"9808261341\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGSN",	0, ERxEvent	},
	{	ETxString, (TText8*)"520002512954000\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CIMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CMGF: (0)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: (\"ME\",\"SM\"),(\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: \"SM\",0,15,\"SM\",0,15\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPBS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=2",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=1",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG?",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"AT+CRC=1",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBC",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CSQ: 31,99\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGQREQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGATT",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

// Init Done
 
  	{	ERxString, (TText8*)"AT+CNMI=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CNMI: (0-2),(0,1,3),(0),(0),(0,1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBST=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CBST: (0-7,12,14-16,34,36,38,39,43,47-51,65,66,68,70,71,75,79-81),(0,2),(1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSCA?",		0, ERxEvent	},
	{	ETxString, (TText8*)"+CSCA: \"441632960000\",145",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGREG=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CNMI: (0-2),(0,1,3),(0),(0),(0,1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=2,1",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,							1,	ETimeOutEvent	},
	{	ETxString, (TText8*)"\r\n+CMTI: \"ME\",1",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS=\"ME\"",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: 1,15,0,15",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,						0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGR=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CMGR: 0,28",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"040C914477746045610000003071610352000AD4F29C9E769F5D2E17\r",
								0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
  	
 
	{	EWait,		NULL,							1,	ETimeOutEvent	},
	{	EComplete,	NULL,							0, 0	}
	};

const struct TScript KRxMessEricssonT28Script[]= {
	{	ERxString,	(TText8*)"AT\r",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EIfRxStringJmp, (TText8*)"AT\r",	1, 0	},

	{	ERxString, (TText8*)"AT+FCLASS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"0,1,2\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//
// The initialisation string - we don't care what it is
//
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=,0",   0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS7?",		0, ERxEvent	},
	{	ETxString, (TText8*)"050\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS0=0",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERICSSON\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMM",	0, ERxEvent	},
	{	ETxString, (TText8*)"1101101-BVT28s\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMR",	0, ERxEvent	},
	{	ETxString, (TText8*)"991022 1210 CXC125115\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGSN",	0, ERxEvent	},
	{	ETxString, (TText8*)"520034611260738\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CIMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CMGF: (0)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: (\"ME\",\"SM\"),(\"ME\",\"SM\"),(\"ME\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: \"SM\",0,15,\"SM\",0,15\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPBS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=2",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=1",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG?",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"AT+CRC=1",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBC",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CSQ: 31,99\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGQREQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGATT",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},


	{	ERxString, (TText8*)"AT+CNMI=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CNMI: (0-2),(0,1,3),(0),(0),(0,1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBST=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CBST: (0-7,12,14-16,34,36,38,39,43,47-51,65,66,68,70,71,75,79-81),(0,2),(1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSCA?",		0, ERxEvent	},
	{	ETxString, (TText8*)"+CSCA: \"441632960000\",145",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGREG=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT*ECAM=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

// Init Complete

	{	ERxString, (TText8*)"AT+CNMI=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CNMI: (0-2),(0,1,3),(0),(0),(0,1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=2,1",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//  Now, finally, wait for 1 second and simulate the incoming SMS
//
	{	EWait,		NULL,							1,	ETimeOutEvent	},
	{	ETxString, (TText8*)"\r\n+CMTI: \"ME\",1",	0, ETxEvent|EFailIfTimeOut	},

//
// Expect a request to set the Prefered SMS Storage area....
//
	{	ERxString, (TText8*)"AT+CPMS=\"ME\"",			0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: 1,15,0,15,1,15",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,								0, ETxEvent|EFailIfTimeOut	},
//
// ...and a request to read the new SMS
//
	{	ERxString, (TText8*)"AT+CMGR=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CMGR: 0,,28",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"089144775810248024040C914477746045610000003071610352000AD4F29C9E769F5D2E17\r",
								0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,							1,	ETimeOutEvent	},
	{	EComplete,	 NULL,								0, 0	}
	};


const struct TScript KRxMessCmtAndCmtiScript[]= {
	{	ERxString,	(TText8*)"AT\r",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EIfRxStringJmp, (TText8*)"AT\r",	1, 0	},

	{	ERxString, (TText8*)"AT+FCLASS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"0,1,2\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//
// The initialisation string - we don't care what it is
//
	{	ERxString, (TText8*)"AT",			0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=,0",   0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS7?",		0, ERxEvent	},
	{	ETxString, (TText8*)"050\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"ATS0=0",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"Nokia\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMM",	0, ERxEvent	},
	{	ETxString, (TText8*)"1100801\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMR",	0, ERxEvent	},
	{	ETxString, (TText8*)"9808261341\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGSN",	0, ERxEvent	},
	{	ETxString, (TText8*)"520002512954000\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CIMI",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CMGF: (0)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGF=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: (\"ME\",\"SM\"),(\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: \"SM\",0,15,\"SM\",0,15\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPBS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPBS: (\"ME\",\"SM\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=0",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=2",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG=1",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CREG?",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",	0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"AT+CRC=1",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBC",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CSQ: 31,99\r",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,				0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGCLASS",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGQREQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGATT",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

// Init Done
 
  	{	ERxString, (TText8*)"AT+CNMI=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CNMI: (0-2),(0,1,3),(0),(0),(0,1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBST=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CBST: (0-7,12,14-16,34,36,38,39,43,47-51,65,66,68,70,71,75,79-81),(0,2),(1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSCA?",		0, ERxEvent	},
	{	ETxString, (TText8*)"+CSCA: \"441632960000\",145",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGREG=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CNMI: (0-2),(0-2),(0),(0),(0,1)\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CNMI=2,2",	0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,							1,	ETimeOutEvent	},
	{	ETxString, (TText8*)"\r\n+CMTI: \"ME\",1",	0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CPMS=\"ME\"",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CPMS: 1,15,0,15",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,						0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CMGR=1",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CMGR: 0,28",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"040C914477746045610000003071610352000AD4F29C9E769F5D2E17\r",
								0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// cmt message
	{	EWait,		NULL,							1,	ETimeOutEvent	},
	{	ETxString, (TText8*)"\r\n+CMT: ,91",		0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"240C91447774604561000000508081030340522ED00A7402A94027900BB402B94027900A7402AD5C2015E8045A815CC8301C9E778156A01348057281442095B37C0765CB61B90A2402A94022D00AE402AD402AD00AE402AD402215\r",
			0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
// cmt end  
 
	{	EWait,		NULL,							1,	ETimeOutEvent	},
	{	EComplete,	NULL,							0, 0	}
	};



//
// CMTI Rx SMS messages
//
class CTestDriveRxMess : public CTestBase
	{
	friend class CTestDriveNotMess;
	friend class CTestDriveRxMessCmt;

public:
	static CTestDriveRxMess* NewL(const TScriptList aScriptListEntry);
	virtual TInt RunTestL();


protected:
	virtual TInt DriveETelApiL();

private:
	CTestDriveRxMess(const TScriptList aScriptListEntry);
	void DispSmsAttributes(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1&);
	void DispCaps(RMobileSmsMessaging::TMobileSmsCapsV1& aSmsCaps);

	TScriptList iScriptListEntry;
	};

//
// CMTI Notification SMS messages
//
class CTestDriveRxMess;
class CTestDriveNotMess : public CTestBase
	{
public:
	static CTestDriveNotMess* NewL(const TScriptList aScriptListEntry);
	virtual TInt RunTestL();

protected:
	virtual TInt DriveETelApiL();

private:
	void DispSmsAttributes(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1&);
	CTestDriveNotMess(const TScriptList aScriptListEntry);
	CTestDriveRxMess*	aTestDriveRxMess;
	TScriptList iScriptListEntry;
	};

//
// CMT Rx SMS messages
//
class CTestDriveRxMess;
class CTestDriveRxMessCmt : public CTestBase
	{
public:
	static CTestDriveRxMessCmt* NewL(const TScriptList aScriptListEntry);
	virtual TInt RunTestL();

protected:
	virtual TInt DriveETelApiL();

private:
	void DispSmsAttributes(RMobileSmsMessaging::TMobileSmsReceiveAttributesV1&);
	CTestDriveRxMessCmt(const TScriptList aScriptListEntry);
	CTestDriveRxMess*	aTestDriveRxMess;
	TScriptList iScriptListEntry;
	};

//
// CMT Notification SMS messages
//
class CTestDriveNotMessCmt : public CTestBase
	{
public:
	static CTestDriveNotMessCmt* NewL(const TScriptList aScriptListEntry);
	virtual TInt RunTestL();

protected:
	virtual TInt DriveETelApiL();

private:
	CTestDriveNotMessCmt(const TScriptList aScriptListEntry);

	TScriptList iScriptListEntry;
	};

class CTestRxMess : public CATScriptEng
	{
public:
	static CTestRxMess* NewL(const TScript* aScript);
	virtual void SpecificAlgorithmL(TInt aParam);
	virtual void Complete(TInt aError);
	virtual TInt Start();

private:
	CTestRxMess(const TScript* aScript);
	void ConstructL();

	const TScript* iScript;
	};



#endif
