/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_manSuiteServer.h
*/

#if (!defined __TE_MAN_SERVER_H__)
#define __TE_MAN_SERVER_H__
#include <test/testexecuteserverbase.h>
#include "lbscommoninternaldatatypes.h"
#include "lbsnetinternalapi.h"
#include "testmessageinterface.h"

//-----------------------------------------------------------------------------------------
class NGDataBufferClass : public MLbsNetChannelObserver
	{
public:
	NGDataBufferClass();
	// From MLbsNetChannelObserver
	virtual void ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage);
	TInt SendNgMsg(const TLbsNetInternalMsgBase& aMsg);
	
	RThread 			   			 iThread;
	RLbsNetChannel 		   			 iNGInt;
	TLbsNetInternalMsgBase&			 iReceivedMsg;
	TUint8							 iReceivedMsgBuffer[KMaximumMessageSizeBytes];
	RLbsNetChannel::TLbsNetChannelId iReceivedChannelId;
	};


class CTe_manSuite : public CTestServer,
					public MNGTestInterface
	{
public:
	static CTe_manSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	virtual void ConstructL(const TDesC& aName);
	
private:
	//NG Functions
	void WaitAndValidateNgMsg(TLbsNetInternalMsgBase::TLbsNetInternalMsgType aMsgType, TBool aFlag = EFalse);
	void SendNgMsg(const TLbsNetInternalMsgBase& aMsg);
	void SetupNGInterface();
	
	void WaitForNgMsg();
	TPtrC* NgMsgTypeToName(TLbsNetInternalMsgBase::TLbsNetInternalMsgType aMsgType);

private:
	TLbsNetSessionIdInt	iSessionId;
	CTestStep* iCurrentStep;
	};

#endif
