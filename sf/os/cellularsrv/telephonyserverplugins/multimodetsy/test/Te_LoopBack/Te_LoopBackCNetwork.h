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


#ifndef __CNETWORK_H__
#define __CNETWORK_H__

#include <mmretrieve.h>
#include <mmlist.h>
#include "Te_LoopbackTestStepBase.h"
#include "Te_LoopBackScriptEng.h"

//
// First a Script where everything is in order
//
const struct TScript KNetworkRegistrationScript[]= {
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

// The registration response below differs from all the other initialization sequences.
// This enables the Network and Registration tests to run. Otherwise cached information
// (obtained during initialisation) is used.
	{	ERxString, (TText8*)"AT+CREG?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CREG: 1,1\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"AT+CRC=1",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBC",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CSQ: 15,99\r",0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

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

//
//  Get the Network Operator Info in numeric, short alphanumeric, and
//  long alphanumeric formats.
//
// testNetworkInfo()
	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Numeric format
	{	ERxString, (TText8*)"AT+COPS=3,2;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,2,\"23415\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Short Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,1;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,1,\"UK VODA\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Long Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,0;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


//	Detection of all Networks
	{	ERxString, (TText8*)"AT+COPS=?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: (2,\"UK VODAFONE\",\"UK VODA\",\"23415\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"+COPS: (3,\"UK CELLNET\",\"UK CLNET\",\"23410\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"+COPS: (3,\"UK Orange PCS Ltd.\",\"Orange\",\"23433\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxString, (TText8*)"+COPS: (3,\"UK MERCURY\",\"one2one\",\"23430\")\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Simulate loss of the Network. Wait 8 seconds then send registration status of 
//  "Not Registered". 
	{	EWait,		NULL,					18,	ETimeOutEvent},
	{	ETxString, (TText8*)"+CREG: 0\r",	0,	ETxEvent|EFailIfTimeOut	},

//	Because the network registration status has changed from 1 to 0 ("Registered, home
//	network" to "Not Registered"), the Network Operator information would be updated.
//	Note the incomplete response from the iPhone
	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Numeric format
	{	ERxString, (TText8*)"AT+COPS=3,2;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Short Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,1;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Long Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,0;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	//	Operator information in Numeric format
	{	ERxString, (TText8*)"AT+COPS=3,2;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Short Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,1;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Long Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,0;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},


//	Wait 8 seconds then send registration status of "Registered, Home Network", to 
//	simulate network recovery. 
	{	EWait,		NULL,					15,	ETimeOutEvent},
	{	ETxString, (TText8*)"+CREG: 1\r",	0,	ETxEvent|EFailIfTimeOut	},

//  Update the Network Operator Information. Used in the Notifications tests
//
	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Numeric format
	{	ERxString, (TText8*)"AT+COPS=3,2;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,2,\"23415\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Short Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,1;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,1,\"UK VODA\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Long Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,0;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Numeric format
	{	ERxString, (TText8*)"AT+COPS=3,2;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,2,\"23415\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Short Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,1;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,1,\"UK VODA\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Long Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,0;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	testMoreNotifications() function.
//  Network Operator Info is retrieved from the cache. 

//	Simulate loss of the Network. Wait 8 seconds then send registration status of "Not
//	Registered". 
	{	EWait,		NULL,					15,	ETimeOutEvent},
	{	ETxString, (TText8*)"+CREG: 0\r",	0,	ETxEvent|EFailIfTimeOut	},

//	Because the network registration status has changed from 1 to 0 ("Registered, home
//	network" to "Not Registered"), the Network Operator information would be updated auto-
//	matically. Note the incomplete response from the iPhone
	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Numeric format
	{	ERxString, (TText8*)"AT+COPS=3,2;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Short Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,1;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Long Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,0;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Wait 3 seconds then send registration status of "Registered, Home Network", to 
//	simulate network recovery. 
	{	EWait,		NULL,					15,	ETimeOutEvent},
	{	ETxString, (TText8*)"+CREG: 1\r",	0,	ETxEvent|EFailIfTimeOut	},

//  Update the Network Operator Information. 
//
	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Numeric format
	{	ERxString, (TText8*)"AT+COPS=3,2;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,2,\"23415\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Short Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,1;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,1,\"UK VODA\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Long Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,0;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

// testMoreNotifications() function (new iPhone)
// Re-initialise as we're simulating a new iPhone with different behaviour
	{	ERxString,	(TText8*)"AT\r",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT\r",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString,	(TText8*)"AT\r",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

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
	{	ETxString, (TText8*)"SIEMENS\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMM",	0, ERxEvent	},
	{	ETxString, (TText8*)"S25\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGMR",	0, ERxEvent	},
	{	ETxString, (TText8*)"10\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CGSN",	0, ERxEvent	},
	{	ETxString, (TText8*)"449102523159179\r",	0, ETxEvent|EFailIfTimeOut	},
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

// The registration response below differs from all the other initialization sequences.
// This enables the Network and Registration tests to run. Otherwise cached information
// (obtained during initialisation) is used.
	{	ERxString, (TText8*)"AT+CREG?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CREG: 1,1\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},
	
	{	ERxString, (TText8*)"AT+CRC=1",		0, ERxEvent	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CBC",	0, ERxEvent	},
	{	ETxString, (TText8*)"ERROR\r",		0, ETxEvent|EFailIfTimeOut	},

	{	ERxString, (TText8*)"AT+CSQ",	0, ERxEvent	},
	{	ETxString, (TText8*)"+CSQ: 15,99\r",0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

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

  // testNetworkInfo()
	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Numeric format
	{	ERxString, (TText8*)"AT+COPS=3,2;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,2,\"23415\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Short Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,1;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,1,\"UK VODA\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Long Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,0;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Simulate loss of the Network. Wait 8 seconds then send registration status of "Not
//	Registered". 
	{	EWait,		NULL,					15,	ETimeOutEvent},
	{	ETxString, (TText8*)"+CREG: 0\r",	0,	ETxEvent|EFailIfTimeOut	},

//	Because the network registration status has changed from 1 to 0 ("Registered, home
//	network" to "Not Registered"), the Network Operator information would be updated auto-
//	matically. Note the incomplete response from the iPhone
	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Numeric format
	{	ERxString, (TText8*)"AT+COPS=3,2;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Short Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,1;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Long Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,0;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Wait 3 seconds then send registration status of "Registered, Home Network", to 
//	simulate network recovery. 
	{	EWait,		NULL,					15,	ETimeOutEvent},
	{	ETxString, (TText8*)"+CREG: 1\r",	0,	ETxEvent|EFailIfTimeOut	},

//  Update the Network Operator Information. 
//
	{	ERxString, (TText8*)"AT+COPS?",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Numeric format
	{	ERxString, (TText8*)"AT+COPS=3,2;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,2,\"23415\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Short Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,1;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,1,\"UK VODA\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

//	Operator information in Long Alphanumeric format
	{	ERxString, (TText8*)"AT+COPS=3,0;+COPS",	0, ERxEvent	},
	{	ETxString, (TText8*)"+COPS: 0,0,\"UK VODAFONE\"\r",	0, ETxEvent|EFailIfTimeOut	},
	{	ETxStringOK, NULL,					0, ETxEvent|EFailIfTimeOut	},

	{	EWait,		NULL,					5,	ETimeOutEvent},

	{	EComplete,		NULL,				0, 0	} 
	};


class CTestDriveNetworkRegistration : public CTestBase
		{
	public:
		static CTestDriveNetworkRegistration* NewL(const TScriptList aScriptListEntry);
		virtual TInt RunTestL();

	protected:
		virtual TInt DriveETelApiL();

	private:
		CTestDriveNetworkRegistration(const TScriptList aScriptListEntry);

		void testServerInfoL();
		void testNetworkInfoL();
		void testDetectedNetwork();
		void DotestDetectedNetworkL();
		void testSynchronousDetect();
		TInt testDisconnectedNetworkL();
		void testNotificationsL();
		void testMoreNotificationsL();
		TInt TestAPI_GetCurrentNetworkL();
		TInt TestAPI_GetNetworkRegistrationStatusL();
		void DisplayNetworkRegistrationStatus(RMobilePhone::TMobilePhoneRegistrationStatus aRegStatus);
		void DisplayCurrentNetworkInfo(RMobilePhone::TMobilePhoneNetworkInfoV1& aPhoneNetworkInfo,
									   RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea);

		TScriptList iScriptListEntry;
		RMobilePhone iTestPhone;
		};


class CTestNetworkRegistration : public CATScriptEng
		{
	public:
		static CTestNetworkRegistration* NewL(const TScript* aScript);

		virtual void SpecificAlgorithmL(TInt aParam);
		virtual void Complete(TInt aError);
		virtual TInt Start();

	private:
		CTestNetworkRegistration(const TScript* aScript);
		void ConstructL();

		const TScript* iScript;
		};


#endif
