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
// Implementation of CStrtSecurityCheckTask class
//




/**
 @file
 @internalComponent
 @released
*/

#include "ssmsecuritychecknotifier.h"
#include "cmdsimsecuritycheck.h"
#include "ssmrefcustomcmdcommon.h"
#include "ssmdebug.h"

#include <e32def.h>
#include <ssm/startupdomainpskeys.h>
#include <ssm/ssmstateawaresession.h>
#include <ssm/ssmpatchableconstants.h>
#include <centralrepository.h>
#include <e32svr.h>
#include <u32hal.h>

//Ordinal postion of the security event observer NewL in the plugin dll
const TInt KSecurityEventObserverNewLOridinal = 1;

//Utility plugin name used for starting security note event observer
_LIT(KSusUtilityPlugin, "ssmclayersup.dll");

/*
 * Function to check whether Invalid Sim feature is enabled or not
 */
TUint32 IsSsmInvalidSimOn()
    {
    TUint32 retVal = 0;

#ifdef __WINS__  
        // KSsmInvalidSim is a ROM patchable constant, so need an emulator equivalent
        // if WINS then read value from epoc.ini, requires licencees to set property in epoc.ini. This value is 0xFFFFFFFF if feature is enabled.
        // Usage: In epoc.ini patchdata_KSsmInvalidSim 0xFFFFFFFF (Can be any non-zero value)
        TUint32 readValueOfInvalidSim = 0;
        if( UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty, (TAny*)"patchdata_KSsmInvalidSim", &readValueOfInvalidSim) == KErrNone )
            {
            retVal = readValueOfInvalidSim;
            }
#else   
        retVal = KSsmInvalidSim;        
        
#endif // __WINS__
    return retVal;
    }

CCustomCmdSimSecurityCheck* CCustomCmdSimSecurityCheck::NewL()
	{
	CCustomCmdSimSecurityCheck* self = new (ELeave) CCustomCmdSimSecurityCheck();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CCustomCmdSimSecurityCheck::ConstructL()
	{
	User::LeaveIfError(iSsmMiscAdaptation.Connect());
	User::LeaveIfError(iSsmStateManager.Connect());
	User::LeaveIfError(iSsmSusCli.Connect());

	iStartupPSUid = CSsmUiSpecific::StartupPSUid();
    __ASSERT_ALWAYS(0 != iStartupPSUid.iUid, PanicNow(KPanicCustomCmdSimSecurityCheck, EInvalidUid));
	iSecurityStateInfoResult = new (ELeave) TPckg<TSecurityStateInfo>(iSecurityStateInfo);
	iSsmUiSpecific = CSsmUiSpecific::InstanceL();

	iSimStatusPSObserver = CSsmSimStatusPSObserver::NewL(iStartupPSUid, iSsmUiSpecific->SimStatusPropertyKey());
	//Add active object to active scheduler
	CActiveScheduler::Add(this);
	}

CCustomCmdSimSecurityCheck::CCustomCmdSimSecurityCheck()
						: CActive( EPriorityStandard)
	{
	}

CCustomCmdSimSecurityCheck::~CCustomCmdSimSecurityCheck()
	{
	ExecuteCancel();
	iSsmMiscAdaptation.Close();		
	iSsmStateManager.Close();
	iSsmSusCli.Close();

	if(iSsmUiSpecific)
		{
		//Will be deleted after last dereference
		iSsmUiSpecific->Release();
		}
	
	delete iSsmSecurityCheckNotifier;
	delete iSecurityNoteObserver;
	delete iSecurityStateInfoResult;
	delete iSimStatusPSObserver;
	}

TInt CCustomCmdSimSecurityCheck::Initialize(CSsmCustomCommandEnv* /*aCmdEnv*/)
	{
	return KErrNone;
	}

void CCustomCmdSimSecurityCheck::Close()
	{
	}

void CCustomCmdSimSecurityCheck::Release()
	{
	delete this;
	}

void CCustomCmdSimSecurityCheck::Execute(const TDesC8& /*aParams*/, TRequestStatus& aStatus)
	{
	//Load the security event observer
	TSsmSupInfo observerPlugin(KSusUtilityPlugin, KSecurityEventObserverNewLOridinal);
	TInt retLoadSecEventObserver = iSsmSusCli.RequestLoadSup(observerPlugin);

	if(KErrNone != retLoadSecEventObserver)
		{
		DEBUGPRINT2A("Load security event observer SUP failed with error code : %d", retLoadSecEventObserver);
		}
	
	//Set the user request to pending
	aStatus = KRequestPending;
	iExecuteRequest = &aStatus;

	//Start the state change
	StartStateChange(StartupAdaptation::ESIMPresent);
	}

/**
@panic EInternalStateError if the active object is running or the state change is pending
*/
void CCustomCmdSimSecurityCheck::StartStateChange(const TSecurityState aState, const TStrtSecurityNoteType aNoteType)
	{
	__ASSERT_ALWAYS(!IsActive(), PanicNow(KPanicCustomCmdSimSecurityCheck, EInternalStateError));
	__ASSERT_ALWAYS(!iStateChangePending, PanicNow(KPanicCustomCmdSimSecurityCheck, EInternalStateError));

	DEBUGPRINT2A("CCustomCmdSimSecurityCheck StartStateChange with State - %d ", aState);

	iSubState = ESecuritySubStateNone;
	iStateChangePending = ETrue;
	iNoteType = aNoteType;
	iState = aState;

	//Request for security state change
	iSsmMiscAdaptation.SecurityStateChange(iState, *iSecurityStateInfoResult, iStatus);
	SetActive();
	}

void CCustomCmdSimSecurityCheck::RunL()
	{
	if(iSubState != ESecuritySubStateNone)
		{
		//Execute sim operations 
		DoExtendedSimOperations();
		return;
		}

	if (iStateChangePending)
		{
		//Start changing the state
		StateChangeL(iStatus.Int());
		}
	else
		{
		if (iStatus == KErrNone)
			{
			//Do further security check
			DoSecurityCheck();
			}
		else
			{
			//Complete client request with result code
			CompleteClientRequest(iStatus.Int());
			}
		}
	}

void CCustomCmdSimSecurityCheck::DoExtendedSimOperations()
	{
    if (KErrNone == iStatus.Int())
        {
        switch (iSubState)
            {
            case ESecuritySubStateSimRemoved:
                DoSimRemoved();
                break;
            case ESecuritySubStateSimNotSupported:
                DoSimNotSupported();
                break;
            case ESecuritySubStateSimReadable:
                DoSimReadable();
                break;
            case ESecuritySubStateSimUsable:
                DoSimUsable();
                break;
            case ESecuritySubStateSimStatusPSObserver:
                DoObserveSimStatusPS();
                break;
            default:
                __ASSERT_DEBUG(ESecuritySubStateNone == iSubState, PanicNow(KPanicCustomCmdSimSecurityCheck, EInvalidSecuritySubState));
                break;
            }
        }
    else
        {
        //Complete client request with result code
        CompleteClientRequest(iStatus.Int());
        }
	}

void CCustomCmdSimSecurityCheck::StateChangeL(const TInt aResultCode)
	{
	__ASSERT_ALWAYS(iStateChangePending, PanicNow(KPanicCustomCmdSimSecurityCheck, EInternalStateError));
	DEBUGPRINT2A("Sim security state change completed with error %d", aResultCode);

	iStateChangePending = EFalse;
	if (aResultCode == KErrNone)
		{
		//Proceed with state change in case previous state completed with out any error
		SecurityCheckL();
		}
	else
		{
		//Complete client request with result code
		CompleteClientRequest(aResultCode);
		}
	}

void CCustomCmdSimSecurityCheck::CompleteClientRequest(TInt aReason)
	{
	//Complete client request with reason code
	if (iExecuteRequest)
        {
        User::RequestComplete(iExecuteRequest, aReason);
        }
	}

void CCustomCmdSimSecurityCheck::SecurityCheckL()
    {
    //Delete the security notifier
	delete iSsmSecurityCheckNotifier;
	iSsmSecurityCheckNotifier = NULL;

	TInt errorCode = KErrNone;
    //Handle the state transition
	
	DEBUGPRINT2A("CCustomCmdSimSecurityCheck SecurityCheckL with State - %d ", iState);
    switch (iState)
        {
        case StartupAdaptation::ESIMPresent:
			HandleSIMPresent();
            break;
		case StartupAdaptation::ESIMReadable:
            SimReadable();
            break;
		case StartupAdaptation::ESIMInvalid:
		    HandleSIMInvalid();
		    break;
        case StartupAdaptation::ESIMRejected:
        	HandleSIMRejected();
        	break;
        case StartupAdaptation::ESIMBlocked:
        	HandleSIMBlocked();
        	break;
        case StartupAdaptation::EPINRequired:
        	HandlePINRequired();
        	break;
        case StartupAdaptation::ESIMLessOfflineSupported:
            HandleSIMLessOfflineSupported();
            break;
		case StartupAdaptation::ESIMCodesOK:
			StartStateChange( StartupAdaptation::ESIMLock );
            break;
        case StartupAdaptation::ESecurityCheckOK:
        	errorCode = RProperty::Set(CSsmUiSpecific::StarterPSUid(), KStarterSecurityPhase, EStarterSecurityPhaseSimOk );
        	if(KErrNone != errorCode)
        		{
        		DEBUGPRINT2A("Failed to set value of property CSsmUiSpecific::StarterPSUid() because of error %d", errorCode);
        		}
        	CompleteClientRequest(KErrNone);
        	break;
        case StartupAdaptation::ESecurityCheckFailed:
        	errorCode = RProperty::Set(CSsmUiSpecific::StarterPSUid(), KStarterSecurityPhase, EStarterSecurityPhaseSimNok);
        	if(KErrNone != errorCode)
        		{
        		DEBUGPRINT2A("Failed to set value of property CSsmUiSpecific::StarterPSUid() because of error %d", errorCode);
        		}
        	CompleteClientRequest(KErrNone);
        	break;
        case StartupAdaptation::ESIMLock:
            if (iSecurityStateInfo == StartupAdaptation::ESimLockRestrictionOn)
                {
                StartSecurityNoteObserverL();
                iSsmSecurityCheckNotifier = CSsmSecurityCheckNotifier::NewL(ESecInfoSimLockRestrOn);
                iSsmSecurityCheckNotifier->ShowNoteL(iStatus);
                SetActive();
                }
            else
                {
				DoSecurityCheck();
                }
            break;
        case StartupAdaptation::EAskPIN:
        case StartupAdaptation::EAskPUK:
            StartSecurityNoteObserverL();
			iSsmSecurityCheckNotifier = CSsmSecurityCheckNotifier::NewL(iNoteType);
            iSsmSecurityCheckNotifier->ShowNoteL(iStatus);
            SetActive();
			break;
        default:
            CompleteClientRequest(KErrNotSupported);
            break;
        };
    }

void CCustomCmdSimSecurityCheck::DoSecurityCheck()
    {
	DEBUGPRINT2A("CCustomCmdSimSecurityCheck DoSecurityCheck with State - %d ", iState);
    //Handle pending state transition
    switch (iState)
        {
        case StartupAdaptation::ESIMLock:
            HandleSIMLock();
            break;
        case StartupAdaptation::EAskPIN:
            HandlePINAsked();
            break;
        case StartupAdaptation::EAskPUK:
            HandlePUKAsked();
            break;
        default:
            CompleteClientRequest(KErrNotSupported);
            break;
        };
	}

void CCustomCmdSimSecurityCheck::HandleSIMPresent()
    {
    switch (iSecurityStateInfo)
    	{
    	case StartupAdaptation::EYes:
    		//Start sim readable state transition
    		StartStateChange(StartupAdaptation::ESIMReadable);
    		break;
    	case StartupAdaptation::ENo:
    		if (IsSimSupported())
    			{
    			SimRemoved();
    			}
    		else
    			{
    			SimNotSupported();
    			}
    		break;
    	default:
    		DEBUGPRINT2A("Illegal security result in ESIMPresent state: %d", iSecurityStateInfo);
    		CompleteClientRequest(KErrGeneral);
    		break;
    	};
    }

void CCustomCmdSimSecurityCheck::DoSimRemoved()
	{
	//Start sim less offline support state transition
	StartStateChange(StartupAdaptation::ESIMLessOfflineSupported);
	}

void CCustomCmdSimSecurityCheck::DoSimNotSupported()
	{
	//Start sim less offline support state transition
	StartStateChange(StartupAdaptation::ESIMLessOfflineSupported);
	}

void CCustomCmdSimSecurityCheck::DoSimReadable()
	{
	if(IsSsmInvalidSimOn())
	    {
	    //Start sim invalid state transition
	    StartStateChange(StartupAdaptation::ESIMInvalid);
	    }
	else
	    {
	    //Start sim rejected state transition
        StartStateChange(StartupAdaptation::ESIMRejected);
	    }
	}

void CCustomCmdSimSecurityCheck::DoSimUsable()
	{
	TInt simStatus = ESimStatusUninitialized;
	TInt errVal = (iSimStatusPSObserver->GetValue(simStatus));
	
	if (KErrNone == errVal)
	    {
	    //Do not observe if SIM status has already changed to ESimUsable
	     if(simStatus == ESimUsable)
	         {
	         //Start security check ok state transition
	         StartStateChange(StartupAdaptation::ESecurityCheckOK);
	         }
	     else
	         {
	         //Wait till the SIM status changes to ESimUsable. 
	         //Start observing for the P&S key change
	         iSimStatusPSObserver->StartObserving(iStatus);
	         iSubState = ESecuritySubStateSimStatusPSObserver;
	         SetActive();
	         }
	    }
	else
	    {
	    DEBUGPRINT2A("Error when getting SIM status value: %d", errVal);
	    CompleteClientRequest(errVal);
	    }
	}

void CCustomCmdSimSecurityCheck::DoObserveSimStatusPS()
    {
    //Start security check ok state transition
    StartStateChange(StartupAdaptation::ESecurityCheckOK);
    }

void CCustomCmdSimSecurityCheck::SimRemoved()
    {
    TUint simStatusPropertyKey = iSsmUiSpecific->SimStatusPropertyKey();
    TSsmSwp swp(simStatusPropertyKey, ESimNotPresent);
    iSsmStateManager.RequestSwpChange(swp, iStatus);
	iSubState = ESecuritySubStateSimRemoved;
	SetActive();
    }

void CCustomCmdSimSecurityCheck::SimNotSupported()
	{
    TUint simStatusPropertyKey = iSsmUiSpecific->SimStatusPropertyKey();
    TSsmSwp swp(simStatusPropertyKey, ESimNotSupported);
	iSsmStateManager.RequestSwpChange(swp, iStatus);
	iSubState = ESecuritySubStateSimNotSupported;
	SetActive();
	}

void CCustomCmdSimSecurityCheck::SimReadable()
	{
    TUint simStatusPropertyKey = iSsmUiSpecific->SimStatusPropertyKey();
    TSsmSwp swp(simStatusPropertyKey, ESimReadable);
	iSsmStateManager.RequestSwpChange(swp, iStatus);
	iSubState = ESecuritySubStateSimReadable;
	SetActive();
	}

void CCustomCmdSimSecurityCheck::SimUsable()
	{
	TUint simStatusPropertyKey = iSsmUiSpecific->SimStatusPropertyKey();
    TSsmSwp swp(simStatusPropertyKey, ESimUsable);
    iSsmStateManager.RequestSwpChange(swp, iStatus);
	iSubState = ESecuritySubStateSimUsable;
	SetActive();	
 	}

void CCustomCmdSimSecurityCheck::HandleSIMInvalid()
    {
    switch (iSecurityStateInfo)
        {
        case StartupAdaptation::EYes:

            //Publish the Sim Security Status as ESimInvalid
            RProperty::Set(iStartupPSUid, KStartupSimSecurityStatus, ESimInvalid);
            
            //Set the security state as failed
            SetSecurityStatus(EStrtSecurityCheckFailed);

            //Start sim less offline support state transition
            StartStateChange(StartupAdaptation::ESecurityCheckFailed);
            break;
        case StartupAdaptation::ENo:
            //Start sim rejected state transition
            StartStateChange(StartupAdaptation::ESIMRejected);
            break;
        default:
            DEBUGPRINT2A("Illegal security result in ESIMInvalid state: %d", iSecurityStateInfo);
            //Complete user request with KErrGeneral
            CompleteClientRequest(KErrGeneral);
            break;
        };
    }


void CCustomCmdSimSecurityCheck::HandleSIMRejected()
    {
    switch (iSecurityStateInfo)
    	{
    	case StartupAdaptation::EYes:
    		RProperty::Set(iStartupPSUid, KStartupSimSecurityStatus, ESimRejected );

    		//Set the security state as failed
			SetSecurityStatus(EStrtSecurityCheckFailed);

			//Start sim less offline support state transition
			StartStateChange(StartupAdaptation::ESecurityCheckFailed);
			break;
    	case StartupAdaptation::ENo:
			//Start sim blocked state transition
			StartStateChange(StartupAdaptation::ESIMBlocked);
			break;
    	default:
    		DEBUGPRINT2A("Illegal security result in ESIMRejected state: %d", iSecurityStateInfo);

    		//Complete user request with KErrGeneral
    		CompleteClientRequest(KErrGeneral);
    		break;
    	};
    }

void CCustomCmdSimSecurityCheck::HandleSIMBlocked()
    {
    switch (iSecurityStateInfo)
    	{
    	case StartupAdaptation::EPUK1Required:
    		//Start PUK required state transition
    		StartStateChange(StartupAdaptation::EAskPUK, ESecCodePUK1);
    		break;
    	case StartupAdaptation::EUPUKRequired:
    		//Start UPUK required state transition
    		StartStateChange(StartupAdaptation::EAskPUK, ESecCodeUPUK);
    		break;
    	case StartupAdaptation::ENo:
    		//Start PIN required state transition
    		StartStateChange(StartupAdaptation::EPINRequired);
    		break;
    	default:
    		DEBUGPRINT2A("Illegal security result in ESIMBlocked state: %d", iSecurityStateInfo);

    		//Complete user request with KErrGeneral
    		CompleteClientRequest(KErrGeneral);
    		break;
    	};
    }

void CCustomCmdSimSecurityCheck::HandlePINRequired()
    {
    switch (iSecurityStateInfo)
    	{
    	case StartupAdaptation::EPIN1Required:
    		//Start PIN required state transition
    		StartStateChange(StartupAdaptation::EAskPIN, ESecCodePIN1);
    		break;
    	case StartupAdaptation::EUPINRequired:
    		//Start UPIN required state transition
    		StartStateChange(StartupAdaptation::EAskPIN, ESecCodeUPIN);
    		break;
    	case StartupAdaptation::ENo:
    		//Start SIM Codes OK state transition
    		StartStateChange(StartupAdaptation::ESIMCodesOK);
    		break;
    	default:
    		DEBUGPRINT2A( "Illegal security result in EPINRequired state: %d", iSecurityStateInfo );

    		//Complete user request with KErrGeneral
    		CompleteClientRequest(KErrGeneral);
    		break;
    	};
    }

void CCustomCmdSimSecurityCheck::HandleSIMLessOfflineSupported()
    {
    if (IsSimSupported())
        {
        if (iSsmUiSpecific->IsSimlessOfflineSupported())
            {
            //SIMless offline is supported

            //Set the security state as Sim less offline
            SetSecurityStatus(EStrtSimlessOffline);
            StartStateChange(StartupAdaptation::ESIMLock);
            }
        else
            {
            //SIMless offline is not supported

            //Set the security state as failed
            SetSecurityStatus(EStrtSecurityCheckFailed);
            StartStateChange(StartupAdaptation::ESecurityCheckFailed);
            }
        }
    else
        {
        //SIM not supported
        SetSecurityStatus(EStrtSimlessOffline);
        StartStateChange(StartupAdaptation::ESIMLock);
        }
    }

TBool CCustomCmdSimSecurityCheck::IsSimSupported()
	{
	return iSsmUiSpecific->IsSimSupported();
	}

void CCustomCmdSimSecurityCheck::HandleSIMLock()
    {
    switch(iSecurityStateInfo)
    	{
    	case StartupAdaptation::ESimLockRestricted:
    		//Set the sim lock status property to "Sim lock active"
		    RProperty::Set(iStartupPSUid, KStartupSimLockStatus, ESimLockActive );

		    //Set the sim security status property to "Sim unaccepted"
		    RProperty::Set(iStartupPSUid, KStartupSimSecurityStatus, ESimUnaccepted );

	        //Set the security state as failed
	        SetSecurityStatus(EStrtSecurityCheckFailed);

	        //Start security check failed state transition
	        StartStateChange(StartupAdaptation::ESecurityCheckFailed);
			break;
		case StartupAdaptation::ESimLockOk:
	       //Set the sim lock status property to "Sim lock ok"
	        RProperty::Set(iStartupPSUid, KStartupSimLockStatus, ESimLockOk );
	        SetSecurityCheckOK();
			break;
		case StartupAdaptation::ESimLockRestrictionOn:
			//Set the sim lock status property to "Sim lock restriction on"
	        RProperty::Set(iStartupPSUid, KStartupSimLockStatus, ESimLockRestrictionOn);
	        SetSecurityCheckOK();
			break;
		case StartupAdaptation::ESimLockRestrictionPending:
	        //Set the sim lock status property to "Sim lock restriction pending"
	        RProperty::Set(iStartupPSUid, KStartupSimLockStatus, ESimLockRestrictionPending);

	        //Set the sim security status property to "Sim unaccepted"
	        RProperty::Set(iStartupPSUid, KStartupSimSecurityStatus, ESimUnaccepted);

	        //Set the security state as failed
	        SetSecurityStatus( EStrtSecurityCheckFailed);

	        //Start security check failed state transition
	        StartStateChange(StartupAdaptation::ESecurityCheckFailed);
			break;
		default:
			DEBUGPRINT2A("Illegal security result in ESIMLock state: %d", iSecurityStateInfo);
			//Complete user request with KErrGeneral
			CompleteClientRequest(KErrGeneral);
			break;
		}

    }

void CCustomCmdSimSecurityCheck::SetSecurityCheckOK()
    {
    if (SecurityStatus() != EStrtSimlessOffline)
        {
        //Set the security state as passed
        SetSecurityStatus( EStrtSecurityCheckPassed);
        SimUsable();
        }
	else
		{
		//Start security check ok state transition
    	StartStateChange(StartupAdaptation::ESecurityCheckOK);
		}
    }


void CCustomCmdSimSecurityCheck::HandlePINAsked()
    {
	__ASSERT_DEBUG(NULL != iSsmSecurityCheckNotifier, PanicNow(KPanicCustomCmdSimSecurityCheck, ECmdNullPtr));

    const TBool isCodeAccepted = iSsmSecurityCheckNotifier->IsCodeAccepted();

    //Try again until PIN is either accepted or blocked.
    if (isCodeAccepted)
        {
        //Start SIM codes ok state transition
        StartStateChange(StartupAdaptation::ESIMCodesOK);
        }
    else
        {
        switch(iNoteType)
      		{
      		case ESecCodePIN1:
      			//Start security code PUK1 state transition
				StartStateChange(StartupAdaptation::EAskPUK, ESecCodePUK1);
				break;
      		case ESecCodeUPIN:
      			//Start security code UPUK state transition
      			StartStateChange(StartupAdaptation::EAskPUK, ESecCodeUPUK);
      			break;
      		default:
                DEBUGPRINT2A("Illegal code type in EAskPIN state: %d", iNoteType);
                //Complete user request with KErrGeneral
                CompleteClientRequest(KErrGeneral);
      			break;
      		}
        }
    }

void CCustomCmdSimSecurityCheck::HandlePUKAsked()
    {
	__ASSERT_DEBUG(NULL != iSsmSecurityCheckNotifier, PanicNow(KPanicCustomCmdSimSecurityCheck, ECmdNullPtr));
    
	//Is the entered code correct
    const TBool isCodeAccepted = iSsmSecurityCheckNotifier->IsCodeAccepted();

    if (isCodeAccepted)
        {
        //Start SIM code ok state transition
        StartStateChange(StartupAdaptation::ESIMCodesOK);
        }
    else
        {
        //Set the Sim security status property to "Sim rejected"
        RProperty::Set(iStartupPSUid, KStartupSimSecurityStatus, ESimRejected);

        //Set the security state as failed
        SetSecurityStatus(EStrtSecurityCheckFailed);

        //Start security check failed state transition
        StartStateChange(StartupAdaptation::ESecurityCheckFailed);
        }
    }

void CCustomCmdSimSecurityCheck::SetSecurityStatus(const TStrtSecurityStatus& aSecurityStatus)
    {
    iSsmUiSpecific->SetSecurityStatus(aSecurityStatus);
    }

TStrtSecurityStatus CCustomCmdSimSecurityCheck::SecurityStatus() const
    {
    return iSsmUiSpecific->SecurityStatus();
    }

void CCustomCmdSimSecurityCheck::StartSecurityNoteObserverL()
	{
	if(!iSecurityNoteObserver)
		{
		iSecurityNoteObserver = CSecurityNoteObserver::NewL();
		}
	}

TInt CCustomCmdSimSecurityCheck::RunError(TInt aError)
	{
	//Do the final cleanup incase of error
	Cleanup(aError);
	return KErrNone;
	}

void CCustomCmdSimSecurityCheck::DoCancel()
	{
	// Cancel all possible requests that could be outstanding on iStatus
	if(iSsmSecurityCheckNotifier)
		{
		iSsmSecurityCheckNotifier->Cancel();
		}
	if(iSecurityNoteObserver)
		{
		iSecurityNoteObserver->Cancel();
		}
	iSsmMiscAdaptation.Cancel();
	iSsmStateManager.RequestSwpChangeCancel();
	iSimStatusPSObserver->Cancel();
	}

void CCustomCmdSimSecurityCheck::ExecuteCancel()
	{
	//Call cancel method of CActive	
	Cancel();

	//Do the final cleanup before cancelling
	Cleanup(KErrCancel);
	}

void CCustomCmdSimSecurityCheck::Cleanup(TInt aError)
	{
	if (iExecuteRequest)
		{
		//Complete the user request with the given error code
		User::RequestComplete(iExecuteRequest, aError);
		}
	}

// CSsmSimStatusPSObserver 
// Active Object Class that observes for SIM status P&S key change
CSsmSimStatusPSObserver* CSsmSimStatusPSObserver::NewL(TUid aCategory, TUint aKey)
	{
	CSsmSimStatusPSObserver* self = new (ELeave) CSsmSimStatusPSObserver(aCategory, aKey);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CSsmSimStatusPSObserver::ConstructL()
	{
	User::LeaveIfError(iProperty.Attach(iCategory, iKey));
	}

CSsmSimStatusPSObserver::CSsmSimStatusPSObserver(TUid aCategory, TUint aKey)  
                                    : CActive(EPriorityStandard), iCategory(aCategory), iKey(aKey)
    {
    CActiveScheduler::Add(this);
    }

CSsmSimStatusPSObserver::~CSsmSimStatusPSObserver()
    {
    Cancel();
    iProperty.Close();
    }

void CSsmSimStatusPSObserver::StartObserving(TRequestStatus& aStatus)
    {
    __ASSERT_DEBUG(!(IsActive()), PanicNow(KPanicCustomCmdSimSecurityCheck, EInternalStateError));
    aStatus = KRequestPending;
    iClientStatus = &aStatus;
    iProperty.Subscribe(iStatus);
    SetActive();
    }

TInt CSsmSimStatusPSObserver::GetValue(TInt& aValue)
    {
    TInt errVal = (iProperty.Get(aValue));
    return errVal;
    }

void CSsmSimStatusPSObserver::DoCancel()
    {
    iProperty.Cancel();
    Complete(KErrCancel);
    }

void CSsmSimStatusPSObserver::RunL()
    {
    Complete(iStatus.Int());
    }

void CSsmSimStatusPSObserver::Complete(const TInt aErrorCode)
    {
    if (iClientStatus)
        {
        User::RequestComplete(iClientStatus, aErrorCode);
        iClientStatus = NULL;
        }
    }
