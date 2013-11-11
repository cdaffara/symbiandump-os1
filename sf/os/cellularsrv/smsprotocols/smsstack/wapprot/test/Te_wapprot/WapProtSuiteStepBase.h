/**
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/**
    @file
    @test
*/

#ifndef WAPPROTSUITESTEPBASE_H
#define WAPPROTSUITESTEPBASE_H

#include "smsstackbaseteststeps.h"

#include <es_wsms.h>
#include <etelmm.h>

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define PDD_NAME2 _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

class CWapProtSuiteStepBase : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

    void SetupWapSocketL();
    void SetupWapSocketL(RSocket& aSocket, TWapAddr& aWapAddr, const TDesC& aPort, TBool aNewStyleClient = ETrue);
    //Socket for receiving status reports
    void SetupStatusReportSocketL();
	
	void ReadWapPortSettingsL(TWapAddr& aWapAddr, const TDesC& aPort);
	void SetCodingSchemeL();
	void SetMessageTypeL();

protected:
	void InternaliseSmsDataAndCheckL(TDes8& aBuffer , TPtrC8& aScnumber);
	TInt GetMessageParameterLengthL(RSocket& aSock);
	void GetMessageParameterL(RSocket& aSock, TPtr8& aParameterStorePtr);
	
	void OpenSocketLC(RSocketServ& aSocketServer, RSocket& aSocket, TUint aAddrFamily = KWAPSMSAddrFamily, TUint aProtocol =KWAPSMSDatagramProtocol);          
	void OpenSocketL(RSocketServ& aSocketServer, RSocket& aSocket, TUint aAddrFamily = KWAPSMSAddrFamily, TUint aProtocol =KWAPSMSDatagramProtocol);
    
	void ReceiveWapMessageFromSocket(TInt aLength, TPtrC& aData);
	void ReceiveWapMessage(TPtrC& aData);
	void SendWapMessage(TPtrC& aData);
	
private:
    void CheckSmsMessageL(CSmsMessage& aSmsmessagebuf, TPtrC8& aScnumber);

protected:
	RSocket iSocket;
	RSocket iStatusReportSocket;
	TWapAddr iWapAddr;
	TSmsAddr iSmsAddr;
	};

#endif // WAPPROTSUITESTEPBASE_H
