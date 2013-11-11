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
// Description: Stores information for a MOLR test sequence
// 
//

#include "clbstestmolrsessionsequence.h"

/** 2nd Phase constructor
 */
CTestMolrSessionSequence* CTestMolrSessionSequence::NewL(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback,
		TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection)
	{
	CTestMolrSessionSequence* self = new (ELeave) CTestMolrSessionSequence(aNgProxy, aTestCallback);
	CleanupStack::PushL(self);
	self->ConstructL(aSequenceNum, aConfigFileName, aConfigSection);
	CleanupStack::Pop(self);
	return self;
	}

/** Destructor
 */
CTestMolrSessionSequence::~CTestMolrSessionSequence()
	{
	if(iSetupPsy)
		{
		iPos.Close();
		iPosServer.Close();
		delete iPosWatcher;
		}
	}
	
/** Standard C Constructor
 */
CTestMolrSessionSequence::CTestMolrSessionSequence(CLbsTestNgMessageHandler& aNgProxy, MHybridMultipleTestCallBack& aTestCallback) 
: CTestSessionSequence(aNgProxy, aTestCallback),
iSetupPsy(EFalse)
	{
	}
	
/** Second phase construction
 */
void CTestMolrSessionSequence::ConstructL(TInt aSequenceNum, const TDesC& aConfigFileName, const TDesC& aConfigSection)
	{
    CTestSessionSequence::ConstructL(aSequenceNum, aConfigFileName, aConfigSection);
    iTestCallback.LogTestStatement(_L("Type = MOLR"));

    // Default to NULL
    iPsyUid = TUid::Null();        
    
    const TInt KMaxHexStringSize = 10;// enough for 0x12345678
    TBuf<KMaxHexStringSize> hexBuffer;
    TPtrC ptr;
    ptr.Set(hexBuffer);
    
    // Find out what PSY to use
    TBuf<10> psyUidName;
    psyUidName.Format(_L("PsyUid%d"), iSequenceId);
    if (iIniFile->FindVar(aConfigSection, psyUidName, ptr))
        {
        // strip any leading "0x" or "0X"
        if(ptr[0] == TChar('0') && (ptr[1] == TChar('X') || ptr[1] == TChar('x')))
            {
            // leading "0x" encoded. So strip
            hexBuffer = ptr.Right(ptr.Length()-2);
            }
        else
            {
            hexBuffer = ptr;
            }
        TLex lex(hexBuffer);
        TUint32 uid;
        if (lex.Val(uid, EHex) == KErrNone)
            {
            iPsyUid = TUid::Uid(uid);
            }
        else
            {
            LogTestStatement(_L("Invalid PsyUid setting: %s"),1,&ptr);
            }
        }
    
    if (iPsyUid == TUid::Null())
        {
        iTestCallback.LogTestStatement(_L("Using default PSY"));
        }
    else
        {
        LogTestStatement(_L("Using PSY %x"), 1, iPsyUid.iUid);        
        }
    
    iTestCallback.LogTestStatement(_L("-----------------------------------------------------------------------"));
	}

/** Sets up the PSY (RPositioner)
 */
void CTestMolrSessionSequence::SetupPsy()
	{
	TInt err = iPosServer.Connect();

	if (iPsyUid == TUid::Null())
	    {
	    err = iPos.Open(iPosServer);
	    }
	else 
	    {
	    err = iPos.Open(iPosServer, TPositionModuleId(iPsyUid));
	    }
			
	TPositionUpdateOptions posOpts(TTimeIntervalMicroSeconds(0), TTimeIntervalMicroSeconds(240 * 1000 * 1000));
	iPos.SetUpdateOptions(posOpts);
	
	iPosWatcher = CPosServerWatcher::NewLC(iPos, *this);
	CleanupStack::Pop(iPosWatcher);
	
	iSetupPsy = ETrue;
	}

/* << Request Location Update
 */
void CTestMolrSessionSequence::RequestLocationUpdate()
	{
	if(!iSetupPsy)
		{
		SetupPsy();
		}
		
	iPosWatcher->IssueNotifyPositionUpdate();
	}

/* << ProcessLocationRequest()
 */
void CTestMolrSessionSequence::SendProcessLocationRequest()
	{
	TBool emergency = EFalse;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	TLbsNetPosRequestQuality quality = ArgUtils::Quality();
	TLbsNetPosRequestMethod method   = ArgUtils::RequestHybridMethod();
	iNgProxy.ProtocolProxy()->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	}

/* << ProcessStatusUpdate()
 */
void CTestMolrSessionSequence::SendProcessStatusUpdate()
	{
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceSelfLocation;
	iNgProxy.ProtocolProxy()->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);
	}

/* >> SelfLocationRequest()
 */
void CTestMolrSessionSequence::CheckSelfLocationRequest(TLbsNetSessionId* aSessionId)
	{
	//Extract the Session ID and assign it to this sequence
	iSessionId.SetSessionNum(aSessionId->SessionNum());
	iSessionId.SetSessionOwner(aSessionId->SessionOwner());
	}


/* >> NetworkLocationRequest()
 */
void CTestMolrSessionSequence::CheckNetworkLocationRequest(TLbsNetSessionId* aSessionId)
    {
    //Extract the Session ID and assign it to this sequence
    iSessionId.SetSessionNum(aSessionId->SessionNum());
    iSessionId.SetSessionOwner(aSessionId->SessionOwner());
    }


/** MPosServerObserver callback when LastKnownPosition is called 
 */
void CTestMolrSessionSequence::OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& /*aPosInfo*/)
	{
	LogTestStatement(_L("-> LastKnownPosition(%d) [5001] Sq: %d"), 2, aErr, iSequenceId);
		
	//Check to see whether this was required in the current sequence
	if(iStateSequence[iCurrentPosition] == EGetLastKnownLocation)
		{
		SignalCallbackIdleStart();
		}
	}

/** MPosServerObserver callback when NotifyPositionUpdate is called
 */
void CTestMolrSessionSequence::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& /*aPosInfo*/)
	{
	LogTestStatement(_L("-> NotifyPositionUpdate(%d) [5000] Sq: %d"), 2, aErr, iSequenceId);
	
	//Check to see whether this was required in the current sequence
	if(iStateSequence[iCurrentPosition] == ENotifyPositionUpdate)
		{
		SignalCallbackIdleStart();
		}
	}
