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
// Description: Stores information for a X3P test sequence
// 
//

#include "clbstestx3psessionsequence.h"

/** 2nd Phase constructor
 */
CTestX3PPositionListener* CTestX3PPositionListener::NewL(MTestX3PPositionListener& aObserver, TInt aCallBackId)
	{
	CTestX3PPositionListener* self = new (ELeave) CTestX3PPositionListener(aObserver, aCallBackId);
	return self;
	}

/** Destructor
 */
CTestX3PPositionListener::~CTestX3PPositionListener()
	{
	Cancel();
	}

/** Standard C Constructor
 */
CTestX3PPositionListener::CTestX3PPositionListener(MTestX3PPositionListener& aObserver, TInt aCallBackId)
: CActive(CActive::EPriorityStandard),
iCallBackId(aCallBackId),
iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

/** RunL, calls the X3P test session sequence notifying it of the received event
 */
void CTestX3PPositionListener::RunL()
	{
	iObserver.NotifyX3PEvent(iCallBackId);
	}

void CTestX3PPositionListener::DoCancel()
	{
	}

TInt CTestX3PPositionListener::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

TRequestStatus& CTestX3PPositionListener::GetStatus()
	{
	SetActive();
	return iStatus;
	}

//------------------------------------------------------------------------------------------------


/** 2nd Phase constructor
 */
CTestX3PSessionSequence* CTestX3PSessionSequence::NewL(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback,
		TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection)
	{
	CTestX3PSessionSequence* self = new (ELeave) CTestX3PSessionSequence(aNgProxy, aTestCallback);
	CleanupStack::PushL(self);
	self->ConstructL(aSequenceNum, aConfigFileName, aConfigSection);
	CleanupStack::Pop(self);
	return self;
	}

/** Destructor
 */
CTestX3PSessionSequence::~CTestX3PSessionSequence()
	{
	iTransmitter.Close();
	iServer.Close();
	delete iRefPosListener;
	delete iPosListener;
	}
	
/** Standard C Constructor
 */
CTestX3PSessionSequence::CTestX3PSessionSequence(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback) 
: CTestSessionSequence(aNgProxy, aTestCallback),
iSetup(EFalse)
	{
	}
	
/** Second phase construction
 */
void CTestX3PSessionSequence::ConstructL(TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection)
	{
	CTestSessionSequence::ConstructL(aSequenceNum, aConfigFileName, aConfigSection);
	iRefPosListener = CTestX3PPositionListener::NewL(*this, EX3PRefPositionUpdate);
	iPosListener = CTestX3PPositionListener::NewL(*this, EX3PPositionUpdate);
	
	iTestCallback.LogTestStatement(_L("Type = X3P"));
	iTestCallback.LogTestStatement(_L("-----------------------------------------------------------------------"));
	}

/** Sets up the X3P session
 */
void CTestX3PSessionSequence::SetupTransmit()
	{
	User::LeaveIfError(iServer.Connect());
	TInt err = (iTransmitter.Open(iServer));
	iSetup = ETrue;
	}

/** Makes a Transmit Position (X3P) request
 */
void CTestX3PSessionSequence::TransmitPosition()
	{
	if(!iSetup)
		{
		SetupTransmit();
		}
	
	_LIT(KThirdParty,"+4407463842101"); 
	const TInt KPriority= 1;
	TLbsTransmitPositionOptions options(TTimeIntervalMicroSeconds(50*1000*1000));
	TRequestStatus& refPosStatus = iRefPosListener->GetStatus();
	TRequestStatus& transPosStatus = iPosListener->GetStatus();
	
	iTransmitter.SetTransmitOptions(options);
	iTransmitter.TransmitPosition(KThirdParty, KPriority, refPosStatus, iRefPosInfo, transPosStatus, iTransPosInfo);
	}

/* << ProcessStatusUpdate()
 */
void CTestX3PSessionSequence::SendProcessStatusUpdate()
	{
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceTransmitThirdParty;
	iNgProxy.ProtocolProxy()->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);	
	}

/* << ProcessLocationRequest()
 */
void CTestX3PSessionSequence::SendProcessLocationRequest()
	{
	TBool emergency = EFalse;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceTransmitThirdParty;
	TLbsNetPosRequestQuality quality = ArgUtils::QualityAlpha2(); 
	TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
	iNgProxy.ProtocolProxy()->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	}

/* >> RequestTransmitLocation
 */
void CTestX3PSessionSequence::CheckRequestTransmitLocation(TLbsNetSessionId* aSessionId, TInt aPriority, const TDesC16& aThirdParty)
	{
	//Extract the Session ID and assign it to this sequence
	iSessionId.SetSessionNum(aSessionId->SessionNum());
	iSessionId.SetSessionOwner(aSessionId->SessionOwner());
	
	//Check the priority is correct
	TInt expectedPriority = 1;
	if(aPriority != expectedPriority)
		{
		iTestCallback.LogTestStatement(_L("Error - Incorrect Priority in RequestTransmitLocation."));
		}
	
	//Check the Third Party Request ID
	_LIT(KThirdParty,"+4407463842101");
	
	TBufC16<14> thirdParty(KThirdParty);
	TPtr16 ptr = thirdParty.Des();
	
	if(ptr.Compare(aThirdParty) != KErrNone)
		{
		iTestCallback.LogTestStatement(_L("Error - Incorrect ThirdParty ID in RequestTransmitLocation."));
		}
	
	}

/** Handles receiving an X3P event
 */
void CTestX3PSessionSequence::NotifyX3PEvent(TInt aId)
	{
	if(aId == EX3PRefPositionUpdate)
		{
		LogTestStatement(_L("-> X3PReferencePositionUpdate [7000] Sq: %d"), 1, iSequenceId);
		
		//Check to see whether the event was expected
		if(iStateSequence[iCurrentPosition] == EX3PRefPositionUpdate)
			{
			SignalCallbackIdleStart();
			}
		}
	else if (aId == EX3PPositionUpdate)
		{
		LogTestStatement(_L("-> X3PPositionUpdate [7001] Sq: %d"), 1, iSequenceId);
		
		//Check to see whether the event was expected
		if(iStateSequence[iCurrentPosition] == EX3PPositionUpdate)
			{
			SignalCallbackIdleStart();
			}
		}
	else
		{
		LogTestStatement(_L("-> Invalid X3P Message Received Sq: %d"), 1, iSequenceId);
		}
	}

