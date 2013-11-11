// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#ifndef _TE_WAPSMSBASE_H_
#define _TE_WAPSMSBASE_H_

#include <es_sock.h>
#include <e32std.h>
#include "es_wsms.h"
#include "commdb.h"
#include <etel.h>
#include "gsmubuf.h"
#include "gsmumsg.h"
#include "gsmuset.h"
#include "smsuaddr.h"
#include "smsustrm.h"
#include <testconfigfileparser.h>

#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

#define DSTDNC_NEWCALL_FULLNAME				_L("DummyNc::PotsNc::")

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

_LIT8(KWapTestMsg,"BEGIN:VCARD\r\nVERSION:2.1\r\nFN:Jal\r\nN:Jal\r\nORG:PanSoftware\r\nTITLE:Director\r\nLABEL:Islington\r\nEND:VCARD\r\n");
_LIT(KReassemblyStoreName,"C:\\Private\\101F7989\\sms\\smsreast.dat");
_LIT(KSegmentationStoreName,"C:\\Private\\101F7989\\sms\\smssegst.dat");

_LIT(KConfigFileDir,"sms");
_LIT(KConfigFilename,"te_wapsms.txt");
// section names
_LIT8(KSendRecvWapSms,           "SendReceiveWapSms");

// item names
_LIT8(KPort,                        "Port");
_LIT8(KDCS,                         "DCS");
_LIT8(KFile,                        "File");

_LIT(KGmsSmsConfigFileName,"te_setupgsmsms.txt");
// section names
_LIT8(KSetupTelNumbers,           "Defaults");
// item names
_LIT8(KServiceCenter,               "ServiceCenter");
_LIT8(KTelefoneNumber,              "TelephoneNumber");


class CWapSmsTestStep : public CTestStep
{

public:
	// definition of helper functions
	void Print8BitDes(const TDesC8& aDes8);
	void WaitForInitializeL();
	void ParseSettingsFromFileL();
	void Connect2EsockLC(RSocketServ& aSocketServer);
	CSmsMessage* CreateSmsMessageL(const TDesC& aDes,
					TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
					CSmsPDU::TSmsPDUType aType = CSmsPDU::ESmsSubmit);
	void SendSmsL(const CSmsMessage* aSms, RSocket& aSocket);
	CSmsMessage* RecvSmsL(RSocket& aSocket, TInt aIoctl = KIoctlReadMessageSucceeded);
	void WaitForRecvL(RSocket& aSocket);
	void ReadFileL( TDesC8& aFileName, TDes8& aDes );
	void DoTestTxRxWapL(TDesC8& aDes, TWapPortNumber aWpn,
					TWapSmsDataCodingScheme aDcs);
	TInt CommInit();
	void InitGlobalsL();
	void DestroyGlobals();

public:
	RFs  iFs;
	TSmsServiceCenterAddress iTelephoneNumber; // Test SIM
	TSmsServiceCenterAddress iServiceCenterNumber;

public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

};


#endif
