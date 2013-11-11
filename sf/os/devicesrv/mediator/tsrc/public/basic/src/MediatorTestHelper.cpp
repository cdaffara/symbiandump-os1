/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#include "MediatorTestHelper.h"
#include <e32debug.h>

CTestCasePropertyWatch* CTestCasePropertyWatch::NewL()
	{
    CTestCasePropertyWatch* me=new(ELeave) CTestCasePropertyWatch;
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop(me);
    return me;
	}

CTestCasePropertyWatch::CTestCasePropertyWatch()
	{
	
	}

void CTestCasePropertyWatch::ConstructL()
	{
	iNotificationReceived = EClear;
	}

CTestCasePropertyWatch::~CTestCasePropertyWatch()
	{

	}

void CTestCasePropertyWatch::ClearAllNotifyData( )
    {
    iNotificationReceived = EClear;
    iDomain = TUid::Uid(0);
    iCategory = TUid::Uid(0); 
    iEventId = 0;  		
    iCommandId = 0;
    iEvents.Reset();
    iCommands.Reset();		
    }    

void CTestCasePropertyWatch::MediatorEventL( TUid aDomain,
                                             TUid aCategory, 
                                             TInt aEventId, 
                                             const TDesC8& aData )
    {
    iNotificationReceived = EMediatorEvent;
    iDomain = aDomain;
    iCategory = aCategory;
    iEventId = aEventId;
    // data?
    }    

void CTestCasePropertyWatch::MediatorCommandL( TUid aDomain,
                                               TUid aCategory, 
                                               TInt aCommandId,
                                               TVersion aVersion, 
                                               const TDesC8& aData )
    {
    iNotificationReceived = EMediatorCommand;
    iDomain = aDomain;
    iCategory = aCategory; 
    iCommandId = aCommandId;			

    //_LIT8(KTest, "Command response here");
    //HBufC8* data = KTest().AllocLC();
    //TInt status = iCommandResponder->IssueResponse( aDomain, 
    //                                  aCategory, 
    //                                  aCommandId, 
    //                                  KErrNone,
    //                                  *data );                                      
    //CleanupStack::PopAndDestroy( data );        

    }    

void CTestCasePropertyWatch::CommandResponseL( TUid aDomain,
                                               TUid aCategory, 
                                               TInt aCommandId,
                                               TInt aStatus, 
                                               const TDesC8& aData )
    {
    iNotificationReceived = ECommandResponse;		
    }    

void CTestCasePropertyWatch::CancelMediatorCommand( TUid aDomain,
                                                    TUid aCategory, 
                                                    TInt aCommandId )
    {
    iNotificationReceived = ECancelMediatorCommand;	
    }        

void CTestCasePropertyWatch::MediatorEventsAddedL( TUid aDomain, 
                                                   TUid aCategory, 
                                                   const REventList& aEvents )
    {
    iNotificationReceived = ECancelMediatorCommand;
    }    
     
void CTestCasePropertyWatch::MediatorCommandsAddedL( TUid aDomain, 
                                                     TUid aCategory,
                                                     const RCommandList& aCommands )
    {
    iNotificationReceived = EMediatorCommandsAdded;	
    }    
                                             
void CTestCasePropertyWatch::MediatorCategoryRemovedL( TUid aDomain, TUid aCategory )
    {
    iNotificationReceived = EMediatorCommandsAdded;
    }    
                                             
void CTestCasePropertyWatch::MediatorEventsRemovedL( TUid aDomain, 
                             						 TUid aCategory,
                             						 const REventList& aEvents )
    {
    iNotificationReceived = EMediatorEventsRemoved;	
    }    
                                             
void CTestCasePropertyWatch::MediatorCommandsRemovedL( TUid aDomain, 
     												   TUid aCategory,
                               						   const RCommandList& aCommands )
    {
    iNotificationReceived = EMediatorCommandsRemoved;	
    }              

// -----------------------------------------------------------------------------
// CTimeOutTimer
//
// -----------------------------------------------------------------------------
//

CTimeOutTimer* CTimeOutTimer::NewL() 
    {
	CTimeOutTimer* self=new(ELeave) CTimeOutTimer();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
    }

CTimeOutTimer::CTimeOutTimer()
: CActive(CActive::EPriorityUserInput)
    {
	
    }
	
void CTimeOutTimer::ConstructL()
    {
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
    }

void CTimeOutTimer::RequestTimeOut(TInt aSeconds)
    {
	iTimeOutOccured = EFalse;
	iTimer.After(iStatus, aSeconds * 1000000);
	SetActive();	
    }

void CTimeOutTimer::RunL()
    {
	//CEikonEnv::Static()->InfoMsg(_L("TimeOut"));
	
	iTimeOutOccured = ETrue;
    }

void CTimeOutTimer::DoCancel()
    {
	iTimer.Cancel();
    }
