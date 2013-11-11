// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Implementation of LBT Server core system state listner
//

#include <ssm/startupdomainpskeys.h>
#include <e32property.h>
#include <centralrepository.h>

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	#include <ssm/ssmdomaindefs.h>
#endif

#include "lbssystemstatelistener.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CLbtSystemStateListener::NewL
// Symbian Two - phase constructor
// ---------------------------------------------------------------------------
//
CLbsSystemStateListener* CLbsSystemStateListener::NewL(MLbsStartupObserver& aObserver)
    {
    CLbsSystemStateListener* self = new ( ELeave ) CLbsSystemStateListener(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CLbtSystemStateListener::~CLbtSystemStateListener
// Destructor
// ---------------------------------------------------------------------------
//
CLbsSystemStateListener::~CLbsSystemStateListener()
	{
	Cancel();	
	
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	iStateAwareSession.Close();
#else
	iStartupObserver.Close();
#endif
	}

// ---------------------------------------------------------------------------
// CLbtSystemStateListener::CLbtSystemStateListener
// Default C++ Constructor
// ---------------------------------------------------------------------------
//
CLbsSystemStateListener::CLbsSystemStateListener(MLbsStartupObserver& aObserver)
	: CActive( EPriorityStandard ), iObserver(aObserver)
	{
	CActiveScheduler::Add( this );
	}

// ---------------------------------------------------------------------------
// CLbtSystemStateListener::ConstructL()
// 2nd phase constuctor for instantiating member variables
// ---------------------------------------------------------------------------
//
void CLbsSystemStateListener::ConstructL()
	{
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	TInt err = iStateAwareSession.Connect( KSM2GenMiddlewareDomain3 );
	if (err != KErrNone)
        {
        iSystemStateManagerAvailable = EFalse;
        }
    else
        {
        iSystemStateManagerAvailable = ETrue;
        }
#else
	User::LeaveIfError( iStartupObserver.Attach( KPSUidStartup, 
	                                                 KPSGlobalSystemState, EOwnerThread ) );    
#endif
	}

// ---------------------------------------------------------------------------
// CLbtSystemStateListener::DoCancel()
// Cancels any outstanding request
// ---------------------------------------------------------------------------
//
void CLbsSystemStateListener::DoCancel()
	{
#ifndef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	iStartupObserver.Cancel();
#endif
	}

// ---------------------------------------------------------------------------
// CLbtSystemStateListener::StartToListenL()
// Starts to Listen to System State events
// ---------------------------------------------------------------------------
//
void CLbsSystemStateListener::StartToListenL()
	{
// Listen to global system state changes
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT // From 9.2 onwards
	if (!iSystemStateManagerAvailable)
        {
        iObserver.HandleStartupCompletionL();
        }
	else
		{
		TSsmState ssmState = iStateAwareSession.State();
		if( ssmState.MainState() != ESsmNormal )
			{
			iStateAwareSession.RequestStateNotification( iStatus );
			SetActive();
			}
		else
			{
			iObserver.HandleStartupCompletionL();
			}
		}
#else // Prior to 9.2
    TInt systemStatus = 0;
        User::LeaveIfError( iStartupObserver.Get( systemStatus ) );
            
        // If system state is not already normal then subsribe 
        // to system state event
    #ifndef RD_STARTUP_CHANGE // This flag is now defined.
            if( systemStatus != ESWStateNormal )
    #else
                if( (systemStatus != ESwStateNormalRfOn )  &&
                    (systemStatus != ESwStateNormalRfOff ) &&
                    (systemStatus != ESwStateNormalBTSap ) )
    #endif  //RD_STARTUP_CHANGE
            {
            iStartupObserver.Subscribe( iStatus );
            SetActive();
            }
        else
            {
            iObserver.HandleStartupCompletionL();
            }
#endif // SYMBIAN_SYSTEM_STATE_MANAGEMENT
	}

// ---------------------------------------------------------------------------
// CLbtSystemStateListener::RunL()
// Request completion event handler
// ---------------------------------------------------------------------------
//
void CLbsSystemStateListener::RunL()
	{
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
    // Retrieve state info and check if normal. If not true acknowledge and request
    // notification. If true inform CLbtServerCore.
    TSsmState ssmState = iStateAwareSession.State();
    if( ssmState.MainState() != ESsmNormal )
        {
        iStateAwareSession.AcknowledgeAndRequestStateNotification( iStatus.Int(), iStatus );
        SetActive();
        }
    else{
        iObserver.HandleStartupCompletionL();
        }
#else
    TInt systemStatus = 0;
	User::LeaveIfError( iStartupObserver.Get( systemStatus ) );
     switch( systemStatus )
		{
		#ifndef RD_STARTUP_CHANGE
		case ESWStateNormal:
		#else
		case ESwStateNormalRfOn:
        case ESwStateNormalRfOff:
        case ESwStateNormalBTSap:
		#endif // RD_STARTUP_CHANGE
			{
			iObserver.HandleStartupCompletionL();
			break;
			}
        default:
            {
            iStartupObserver.Subscribe( iStatus );
            SetActive();                
            break;
            }
        }
#endif
	}
	

TInt CLbsSystemStateListener::RunError(TInt aError)
    {
    return aError;
    }

//end of file
