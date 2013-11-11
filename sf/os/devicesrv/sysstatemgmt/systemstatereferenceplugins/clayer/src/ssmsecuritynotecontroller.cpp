// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of CStrtSecurityNoteController class
//



#include "strtsecnoterequestqueue.h"
#include "ssmsecuritynotecontroller.h"
#include "ssmsecuritychecknotifier.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
#include <ssm/ssmstatemanager.h>
#include <ssm/startupreason.h>

CStrtSecurityNoteController* CStrtSecurityNoteController::NewL()
    {
    CStrtSecurityNoteController* self = new( ELeave ) CStrtSecurityNoteController;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CStrtSecurityNoteController::~CStrtSecurityNoteController()
    {
    Cancel();
    delete iSecurityNote;
    delete iQueue;
    }

TInt CStrtSecurityNoteController::SecurityNoteRequested(
    const TStrtSecurityNoteType aNoteType )
    {
    TInt errorCode = iQueue->Add(aNoteType);
    if(KErrNone != errorCode)
    	{
    	DEBUGPRINT2A("Failed to add security note request to the queue "
    			"with error %d", errorCode);
    	}

    if ((errorCode == KErrNone) && !IsActive())
        {
        CompleteSelf(); // Jump to RunL
        }
    return errorCode;
    }

void CStrtSecurityNoteController::SecurityCodeVerified(
    const TStrtSecurityNoteType aNoteType )
    {
    iQueue->Remove( aNoteType );
    // No need to care about the security code which is currently being
    // requested, SecurityNotifier completes that request.
    }

void CStrtSecurityNoteController::DoCancel()
    {
    iSecurityNote->Cancel();
    }

void CStrtSecurityNoteController::RunL()
    {
    TStrtSecurityNoteType code = iQueue->GetFirst();
    if ( code != ESecNoteNone )
        {
        iSecurityNote->ShowNoteL(code, iStatus );
        SetActive();
        }
    }
#ifdef _DEBUG
TInt CStrtSecurityNoteController::RunError(TInt aError)
#else
TInt CStrtSecurityNoteController::RunError(TInt)
#endif // _DEBUG
    {
    DEBUGPRINT2A("CStrtSecurityNoteController RunL completed with error %d", aError);
#ifdef _DEBUG
	PanicNow(KPanicSecurityNoteController, aError);
#else
    RSsmStateManager session;
    TInt errorCode = session.Connect();
    if ( KErrNone == errorCode )
        {
        TSsmStateTransition stateinfo( ESsmShutdown, KSsmAnySubState, EUnknownReset );
        errorCode = session.RequestStateTransition( stateinfo );
        if( KErrNone != errorCode )
            {
            PanicNow(KPanicSecurityNoteController, errorCode);
            }
        session.Close();
        }
	else
		{
        PanicNow(KPanicSecurityNoteController, errorCode);	
		}
#endif //_DEBUG

    return KErrNone;
    }

CStrtSecurityNoteController::CStrtSecurityNoteController()
  : CActive( EPriorityStandard ),    
    iQueue( NULL ),
    iSecurityNote( NULL )
    {
    CActiveScheduler::Add( this );
    }

void CStrtSecurityNoteController::ConstructL()
    {
    iQueue = CStrtSecNoteRequestQueue::NewL();
    iSecurityNote = CSsmSecurityCheckNotifier::NewL();
    }

void CStrtSecurityNoteController::CompleteSelf()
    {
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }
