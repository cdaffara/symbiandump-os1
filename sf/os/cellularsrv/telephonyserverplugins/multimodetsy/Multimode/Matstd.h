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
// AT Command TSY Standard Header 
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __MATSTD_H__
#define __MATSTD_H__

#include "ATSTD.H"

_LIT8(KAsterisk,"*");

_LIT8(KCMTMatchString,"+CMT:*");
_LIT8(KCDSMatchString,"+CDS:*");

_LIT8(KCGREGMatchString,"+CGREG:*");

_LIT8(KCMTIMatchString,"+CMTI:*");

_LIT8(KSmsGetModeCommand,"AT+CMGF=?");
_LIT8(KSmsSetModeCommand,"AT+CMGF=");
_LIT8(KSetSmsModeSupportCommand,"AT+CMGF=0");

_LIT8(KSmsEnterBlockModeCommand,"AT+CESP");

_LIT8(KSmsSetSCCommand,"AT+CSCA=");
_LIT8(KServiceCentreQueryCommand,"AT+CSCA?\r");
_LIT8(KServiceCentreQueryCommandWithCRES,"AT+CRES=1;+CSCA?\r");
_LIT8(KSmsFormatCommand,"AT+CMGF=%d\r");
_LIT8(KSmsSendPduLengthCommand,"AT+CMGS=%d\r");

_LIT8(KSmsSetTextModeCommand,"AT+CSMP=");

_LIT8(KSmsSendCommand,"AT+CMGS=");

_LIT8(KSmsSetTABufferMode,"AT+CNMI=,0\r");	
_LIT8(KSmsSetCMTMode,"AT+CNMI=%d,2\r");	
_LIT8(KSmsSetCMTIMode,"AT+CNMI=%d,1\r");
_LIT8(KSmsGetNMICommand,"AT+CNMI?\r");
_LIT8(KGetCNMIConfigCommand,"AT+CNMI=?\r");

_LIT8(KSmsSetSMSCommand,"AT+CSMS=");
_LIT8(KSmsGetSMSCommand,"AT+CSMS?");
_LIT8(KGetCSMSConfigCommand,"AT+CSMS=?");

_LIT8(KDataCallBearerServiceType,"AT+CBST="); 
_LIT8(KGetCBSTConfigCommand,"AT+CBST=?\r");  

_LIT8(KDeleteContextCommand,"AT+CGDCONT=%d\r");

_LIT8(KGetCGREGCommand,"AT+CGREG?\r");


// AT commands used to interrogate the phone's phonebook support.
_LIT8(KPhoneBookStorageSet,"AT+CPBS=\"%S\"");
_LIT8(KPhoneBookStorageRead,"AT+CPBS?");
_LIT8(KPhoneBookStorageTest,"AT+CPBS=?");


// CNMI Capability Flags
const TInt KDefaultCnmiModeParam=1;
const TInt KBufferedCnmiModeParam=2;
const TInt KInBandCnmiModeParam=3;
const TUint KCnmiModeFlag0=1;
const TUint KCnmiModeFlag1=2;
const TUint KCnmiModeFlag2=4;
const TUint KCnmiModeFlag3=8;
const TUint KCmtiParam=1;
const TUint KCmtParam=2;
const TUint KCmtModeOff=1;
const TUint KCmtModeIndicator=2;
const TUint KCmtModeDirect=4;
const TUint KSrNoneParam=0;
const TUint KSrOnParam=1;
const TUint KSrCapsNotSupported=1;
const TUint KSrCapsSupported=2;
const TUint KGprsCommandTimeOut = 10;		// seconds
const TUint KGprsLongCommandTimeOut = 30;

_LIT8(KSetCNMIConfigCommand,"AT+CNMI=%d");
_LIT8(KNextCNMIParam,",%d");

_LIT8(KSetPrefMemCommandWithString,"AT+CPMS=\"%S\",\"%S\"\r"); // Changed to set the write store as well(mem1 = read,mem2 = write)
_LIT8(KGetPrefMemTestCommand,"AT+CPMS?\r");
_LIT8(KGetPrefMemCommand,"AT+CPMS=?\r");

_LIT8(KSmsReadCommand,"AT+CMGR=%d\r");

_LIT8(KGetBatteryChargeCommand,"AT+CBC\r");

_LIT8(KGetSignalQualityCommand,"AT+CSQ\r");

_LIT8(KGetManufacturerIDCommand,"AT+CGMI\r");

_LIT8(KGetModelIDCommand,"AT+CGMM\r");
	
_LIT8(KGetRevisionIDCommand,"AT+CGMR\r");

_LIT8(KGetSerialNumberIDCommand,"AT+CGSN\r");

_LIT8(KGetSubscrieberIDCommand,"AT+CIMI\r");

_LIT8(KCGDCONTCommand,"AT+CGDCONT=?\r");
_LIT8(KCurrentCGCLASSCommand,"AT+CGCLASS?\r");
_LIT8(KMaxCGCLASSCommand,"AT+CGCLASS=?\r");

_LIT8(KCGQREQCommand,"AT+CGQREQ=?\r");
_LIT8(KCGATTCommand,"AT+CGATT?\r");


//
// Expected modem responses to GSM commands
//
_LIT8(KCBSTResponseString,"+CBST:");  
_LIT8(KCMTResponseString,"+CMT:");
_LIT8(KCDSResponseString,"+CDS:");
_LIT8(KCNMIResponseString,"+CNMI:");
_LIT8(KCMGFResponseString,"+CMGF:");
_LIT8(KCMTIResponseString,"+CMTI:");
_LIT8(KCMGSResponseString,"+CMGS:");
const TInt KCMGSResponseStringLength=6;
_LIT8(KCMGRResponseString,"+CMGR:");
_LIT8(KCMGWResponseString,"+CMGW:");
_LIT8(KCPMSResponseString,"+CPMS:");
_LIT8(KCSCAResponseString,"+CSCA:");
_LIT8(KCBCResponseString,"+CBC:");
_LIT8(KCSQResponseString,"+CSQ:");
_LIT8(KCGMIResponseString,"+CGMI:");
_LIT8(KCGMMResponseString,"+CGMM:");
_LIT8(KCGMRResponseString,"+CGMR:");
_LIT8(KCGSNResponseString,"+CGSN:");
_LIT8(KCGDCONTResponseString,"+CGDCONT:");
_LIT8(KCGREGResponseString,"+CGREG:");

_LIT8(KEndSmsModeResponseString,"\x10\x02\x3F\x10\x03\x00\x3F");

_LIT8(KERRORResponseString,"ERROR");
const TInt KERRORResponseStringLength=5;

_LIT8(KCGCLASSResponseString,"+CGCLASS:");
_LIT8(KCGQREQResponseString,"+CGQREQ:");
_LIT8(KCGATTResponseString,"+CGATT:");
_LIT8(KCGQMINResponseString,"+CGQMIN:");
_LIT8(KPhoneBookStorageResponse,"+CPBS");


//
// GSM storage types
//

_LIT8(KFDStorage,"FD");
_LIT8(KLDStorage,"LD");
_LIT8(KMEStorage,"ME");
_LIT8(KMTStorage,"MT");
_LIT8(KSMStorage,"SM");
_LIT8(KTAStorage,"TA");
_LIT8(KBMStorage,"BM");
_LIT8(KDCStorage,"DC");
_LIT8(KENStorage,"EN");
_LIT8(KMCStorage,"MC");
_LIT8(KONStorage,"ON");
_LIT8(KRCStorage,"RC");
_LIT8(KSNStorage,"SN");

#endif
