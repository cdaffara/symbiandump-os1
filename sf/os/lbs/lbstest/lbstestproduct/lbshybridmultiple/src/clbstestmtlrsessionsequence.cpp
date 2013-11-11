// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Stores information for a MTLR test sequence
// 
//

#include "clbstestmtlrsessionsequence.h"

/** 2nd Phase constructor
 */
CTestMtlrSessionSequence* CTestMtlrSessionSequence::NewL(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback,
		TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection)
	{
	CTestMtlrSessionSequence* self = new (ELeave) CTestMtlrSessionSequence(aNgProxy, aTestCallback);
	CleanupStack::PushL(self);
	self->ConstructL(aSequenceNum, aConfigFileName, aConfigSection);
	CleanupStack::Pop(self);
	return self;
	}

/** Destructor
 */
CTestMtlrSessionSequence::~CTestMtlrSessionSequence()
	{
	}
	
/** Standard C Constructor
 */
CTestMtlrSessionSequence::CTestMtlrSessionSequence(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback)
:CTestSessionSequence(aNgProxy, aTestCallback),
iEmergency(EFalse)
	{
	}
	
/** Second phase construction
 */
void CTestMtlrSessionSequence::ConstructL(TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection)
	{
	CTestSessionSequence::ConstructL(aSequenceNum, aConfigFileName, aConfigSection);
	
	//Find out what type of MTLR this session is (standard MTLR or NI MTLR)
	TBuf<6> typeName;
	typeName.Format(_L("Type%d"), iSequenceId);
	TPtrC sequenceType;
	iIniFile->FindVar(aConfigSection, typeName, sequenceType);
	
	if(sequenceType.Find(_L("NIMTLR")) != KErrNotFound)
		{
		iService = MLbsNetworkProtocolObserver::EServiceNetworkInduced;
		iTestCallback.LogTestStatement(_L("Type = Network Induced MTLR"));
		}
	else
		{
		iService = MLbsNetworkProtocolObserver::EServiceNetworkInduced;
		iTestCallback.LogTestStatement(_L("Type = MTLR"));
		}

	//Read the Emergency field
	TBuf<11> emergencyName;
	emergencyName.Format(_L("Emergency%d"), iSequenceId);
	iIniFile->FindVar(aConfigSection, emergencyName, iEmergency);
	LogTestStatement(_L("Emergency = %d"), 1, iEmergency);
	
	iTestCallback.LogTestStatement(_L("-----------------------------------------------------------------------"));
	}

/* << ProcessPrivacyRequest()
 */
void CTestMtlrSessionSequence::SendProcessPrivacyRequest()
	{
	TLbsNetPosRequestPrivacy privacy    = ArgUtils::Privacy();
	TLbsExternalRequestInfo requestInfo = ArgUtils::RequestInfo();
	
	iNgProxy.ProtocolProxy()->CallL(ENetMsgProcessPrivacyRequest, &iSessionId, &iEmergency, &privacy, &requestInfo);
	}

/* << ProcessLocationRequest()
 */
void CTestMtlrSessionSequence::SendProcessLocationRequest()
	{
	TLbsNetPosRequestQuality quality = ArgUtils::QualityAlpha2(); 
	TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
	iNgProxy.ProtocolProxy()->CallL(ENetMsgProcessLocationRequest, &iSessionId, &iEmergency, &iService, &quality, &method);
	}

/* << ProcessStatusUpdate()
 */
void CTestMtlrSessionSequence::SendProcessStatusUpdate()
	{
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	iNgProxy.ProtocolProxy()->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);
	}

/** Checks RespondPrivacyRequest()
 */
void CTestMtlrSessionSequence::CheckRespondPrivacyRequest(CLbsNetworkProtocolBase::TLbsPrivacyResponse aPrivacyResponse)
	{
	//Check to ensure the privacy request is accepted
	if(aPrivacyResponse != CLbsNetworkProtocolBase::EPrivacyResponseAccepted)
		{
		iTestCallback.StopTest(KErrArgument);
		}
	}

