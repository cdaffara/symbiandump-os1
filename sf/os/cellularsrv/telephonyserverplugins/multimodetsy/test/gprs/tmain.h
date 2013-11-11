// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header for the mainframe of the gprs test programs
// 
//

/**
 @file
*/


#if !defined(__TMAIN_H__)
#define __TMAIN_H__

#include <e32test.h>
#include <c32comm.h>
#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include <smsuaddr.h>
#include <msvstd.h>
//#include <agentclient.h>
#include "tcfgfile.h"
#include "etelpckt.h"

// LITs used for ini file.
_LIT(KTGPRSConfigFile,"t_gprs.ini"); 
_LIT(KTestCfgHTTPServerIpAddress,"TestCfgHTTPServerIpAddress");
_LIT(KTestCfgHTTPPageAddress,"TestCfgHTTPPageAddress");
_LIT(KTestCfgPhoneMake, "TestCfgPhoneMake");
_LIT(KTestCfgPhoneMotorola, "Motorola");
_LIT(KTestCfgPhoneEricsson, "Ericsson");

const TInt KOneSecond = 1000000;
const TInt KFiveSeconds = KOneSecond * 5;
_LIT(KtsyName, "MM");
_LIT(KPhoneName,  "GsmPhone1");


GLREF_D     RTest           gTest;
GLREF_D     RPhone          gPhone;
GLREF_D     RTelServer      gServer;
GLREF_D		RPacketService	gPacketService;
GLREF_D		RPacketContext	gPacketContext[20];
// Used for webpage download
static RConnection		conn;
//GLREF_D     RGenericAgent	gGenericAgent;
GLREF_D     RSocketServ		gSocketServer;
GLREF_D		CConfigParams* testConfigParams;


enum TPhoneBrand
	{
	EMotorola,
	EEricsson
	};

GLREF_D TPhoneBrand GPhoneBrand;
GLDEF_C void AutoTestExecute();


#define LEAVE_IF_ERROR(a)                                         \
if (a!=KErrNone)                                                  \
	{                                                             \
    TText8* fileChars = (TText8*)__FILE__;                        \
    TPtrC8 fileName8(fileChars);                                  \
	TBuf<128> fileName;                                           \
	fileName.Copy(fileName8);                                     \
    gTest.Printf(_L("%S ret=%d line=%d\n"),&fileName,a,__LINE__); \
	}                                                             \
User::LeaveIfError(a);

#endif  //__TMAIN_H__


