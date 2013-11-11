// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "susadaptionserver.h"
#include "susadaptionsession.h"
#include "susadaptionclisrv.h"
#include "suspanic.h"
#include "ssmdebug.h"


// ------------------- Policy Server Security Setup ----------------------

const TUint  KRangeCount = 8; 
const TInt   KRanges[KRangeCount] = // add the required ranges here
	{ 
	0,
	ERequestCoopSysStateChange,     //State Adaptation Range
	EGetSimOwned,					//Sim Adaptation Range
	EValidateRtc,					//Rtc Adaptation Range
	ESecurityStateChange,			//Misc Adaptation Range
	EActivateRfForEmergencyCall,	//Emergency Call Adaptation Range
	ESetAsPriorityClient,           //Setting Priority Client 
	EEndOfSusAdaptionOpCodes		//Dummy
	};
	
/**
 Maps to index in the array KPolicyElements
 */
const TInt KCapabilitiesForRequest1 = 0;
const TInt KCapabilitiesForRequest2 = 1;
const TInt KSecurityCheckForRequest3 = 2;

/**
 Specifies the appropriate action to take for each range in KRanges. 
 The n:th element of KElementsIndex specifies the appropriate action to take for the n:th range in KRanges.
 */	
const TUint8 KElementsIndex[KRangeCount] = 
	{
	CPolicyServer::ENotSupported,
	CPolicyServer::ECustomCheck, 
	KCapabilitiesForRequest1,
	CPolicyServer::ECustomCheck, 
	CPolicyServer::ECustomCheck, 
	KCapabilitiesForRequest2,
	KSecurityCheckForRequest3,
	CPolicyServer::ENotSupported
	};

//ESetAsPriorityClient request is allowed only for priotity clients(Telephony). Inorder to prevent the access
//client request will be policed using C32exe.exe SID
#define KC32SecureId 0x101F7989

/**
 Array containing the different security checks performed by this server 
 */
const CPolicyServer::TPolicyElement KPolicyElements[] = 
	{ 
	{_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), CPolicyServer::EFailClient}, //lint !e778 suppress Constant expression evaluates to 0 in operation '+'
	{_INIT_SECURITY_POLICY_C2(ECapabilityPowerMgmt, ECapabilityWriteDeviceData), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_S0(KC32SecureId), CPolicyServer::EFailClient}
	};

/**
 Setup a security policy.
 */
const CPolicyServer::TPolicy KSusAdaptionServerPolicy =
	{
	CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
	KRangeCount,
	KRanges,
	KElementsIndex, 	
	KPolicyElements 	
	};

// ------------------- AdaptionServer Implementation ----------------------

/**
 */
 
#define KSsmSecureId 0x2000D75B
#define KAlarmServerSecureId 0x101F5027
#define KSusSecureId 0x2000D766


CPolicyServer::TCustomResult CSsmAdaptationServer::CustomSecurityCheckL(const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/)
	{
	CPolicyServer::TCustomResult customResult;
	switch(aMsg.Function())
		{
		case ERequestCoopSysStateChange:/*State Adaptation OpCodes*/
		case ERequestCoopSysSelfTest:
		case ERequestCoopSysPerformRestartActions:
		case ERequestCoopSysPerformShutdownActions:
		case ERequestCoopSysPerformRfsActions:
		case ERequestCoopSysCancel:
		case ESecurityStateChange:/*Misc Adaptation OpCodes*/
		case EGetGlobalStartupMode:
		case EPrepareSimLanguages:
		case EMiscCancel:
		case EGetHiddenReset:
		case EGetSimLanguagesArray:
			{
			if(aMsg.SecureId() == KSsmSecureId)
				{
				customResult = EPass;					
				}
			else
				{
				customResult = EFail;				
				}
			break;
			}
		case EGetLastCoopSysEvent:
		case ENotifyCoopSysEvent:
		case ENotifyCoopSysCancel:
			{
			if(aMsg.SecureId() == KSusSecureId)
				{
				customResult = EPass;					
				}
			else
				{
				customResult = EFail;					
				}
			break;
			}			
		case EValidateRtc:/*Rtc Adaptation OpCodes*/
		case ERtcCancel:
			{
			// On licensee devices processes other than the alarm server need to check the validity of the RTC,
			// so they should be allowed to validate the Rtc and to cancel the validate rtc request.
			customResult = EPass;
			break;
			}
		case ESetWakeupAlarm:
		case EUnsetWakeupAlarm:
			{
			if(aMsg.SecureId() == KAlarmServerSecureId)
				{
				customResult = EPass;					
				}
			else
				{
				customResult = EFail;				
				}
			break;
			}
		default:
			{
			customResult = EFail;
			break;
			}
		}
		
	return customResult;
	}
 
 
CSsmAdaptationServer::CSsmAdaptationServer(): CPolicyServer(EPriorityStandard, KSusAdaptionServerPolicy)
	{
	}

/**
 */
CSsmAdaptationServer::~CSsmAdaptationServer( )
	{
	const TInt ignore = User::SetCritical (User::ENotCritical );

	// Observers must be deleted before their corresponding adaptation
	delete iStateAdaptationObservers;
	iStateAdaptationObservers = NULL;
	delete iStateAdaptation;
	iStateAdaptation = NULL;
	
	// Observers must be deleted before their corresponding adaptation
	delete iSimAdaptationObservers;
	iSimAdaptationObservers = NULL;
	delete iSimAdaptation;
	iSimAdaptation = NULL;
	
	delete iEmergencyCallRfAdaptation;
	iEmergencyCallRfAdaptation = NULL;
	delete iMiscAdaptation;
	iMiscAdaptation = NULL;
	delete iRtcAdaptation;
	iRtcAdaptation = NULL;
	
	//Delete iSusAdaptationPluginLoader after deleting all the adaptations as it will close
	//all the loaded libraries.
	delete iSusAdaptationPluginLoader;
	} //lint !e529 suppress Symbol 'ignore' not subsequently referenced

/**
 */
CSsmAdaptationServer* CSsmAdaptationServer::NewLC( )
	{
	return CSsmAdaptationServer::NewLC(KSusAdaptionServerName);
	}

/**
 * Overloaded NewLC, which starts the server with the provided name.
 @param aServerName - Name of the Server.
 */
CSsmAdaptationServer* CSsmAdaptationServer::NewLC(const TDesC& aServerName)
    {
    CSsmAdaptationServer* server = new(ELeave) CSsmAdaptationServer();
    CleanupStack::PushL(server);
    server->ConstructL(aServerName);
    return server;
    }


/**
 * ConstructL()
 */
void CSsmAdaptationServer::ConstructL(const TDesC& aServerName)
	{

	__ASSERT_ALWAYS( KErrNone == User::SetCritical(User::ESystemCritical),
			User::Panic(KPanicSsmSus, EAdaptionServerError1));

	__ASSERT_ALWAYS( KErrNone == User::RenameThread(KSusAdaptionServerName),
			User::Panic(KPanicSsmSus, EAdaptionServerError2));

	iSusAdaptationPluginLoader = CSusAdaptationPluginLoader::NewL();
	TInt err = Start(aServerName);
#ifdef  TESTSUSADAPTATIONPLUGINLOAD_OOM
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
#else
	User::LeaveIfError(err);
#endif
	}

/**
 */
CSession2* CSsmAdaptationServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{
	// Check we're the right version
	TVersion v(KSsmAdaptationMajorVersionNumber, KSsmAdaptationMinorVersionNumber,
			KSsmAdaptationBuildVersionNumber);
	if ( !User::QueryVersionSupported(v,aVersion) )
		{
		User::Leave (KErrNotSupported );
		}

	// version supported, go ahead
	return CSsmAdaptationSession::NewL ( );
	}
void CSsmAdaptationServer::DoRequestCoopSysStateChangeL(const RMessage2& aMessage)
	{
	if( iStateAdaptation == NULL )
	    {
	    LoadStateAdaptationL();
	    }
	iStateAdaptation->DoRequestCoopSysStateChangeL(aMessage);
	}
void CSsmAdaptationServer::DoRequestCoopSysSelfTestL(const RMessage2& aMessage)
	{
    if( iStateAdaptation == NULL )
        {
        LoadStateAdaptationL();
        }

	iStateAdaptation->DoRequestCoopSysSelfTestL(aMessage);
	}

void CSsmAdaptationServer::DoRequestCoopSysPerformRestartActionsL(const RMessage2& aMessage)
	{
    if( iStateAdaptation == NULL )
        {
        LoadStateAdaptationL();
        }

	iStateAdaptation->DoRequestCoopSysPerformRestartActionsL(aMessage);
	}

void CSsmAdaptationServer::DoRequestCoopSysPerformShutdownActionsL(const RMessage2& aMessage)
	{
    if( iStateAdaptation == NULL )
        {
        LoadStateAdaptationL();
        }

	iStateAdaptation->DoRequestCoopSysPerformShutdownActionsL(aMessage);
	}

void CSsmAdaptationServer::DoRequestCoopSysPerformRfsActionsL(const RMessage2& aMessage)
	{
    if( iStateAdaptation == NULL )
        {
        LoadStateAdaptationL();
        }

	iStateAdaptation->DoRequestCoopSysPerformRfsActionsL(aMessage);
	}

void CSsmAdaptationServer::DoRequestCoopSysCancelL(const RMessage2& aMessage)
	{
    if( iStateAdaptation == NULL )
        {
        LoadStateAdaptationL();
        }

	iStateAdaptation->DoRequestCancel(aMessage);
	}
void CSsmAdaptationServer::DoNotifyCoopSysEventL(const RMessage2& aMessage)
	{
	if( iStateAdaptationObservers == NULL )
	    {
	    LoadStateAdaptationObserversL();
	    }
	iStateAdaptationObservers->DoNotifyCoopSysEventL(aMessage) ;
	}
void CSsmAdaptationServer::DoNotifyCoopSysEventCancelL(const RMessage2& aMessage)
	{
    if( iStateAdaptationObservers == NULL )
        {
        LoadStateAdaptationObserversL();
        }
	iStateAdaptationObservers->DoNotifyCoopSysEventCancelL(aMessage);
	}
void CSsmAdaptationServer::DoGetLastCoopSysEventL(const RMessage2& aMessage)
	{
    if( iStateAdaptationObservers == NULL )
        {
        LoadStateAdaptationObserversL();
        }
	iStateAdaptationObservers->DoGetLastCoopSysEvent(aMessage);
	}

//SIM ADAPTATION RELATED

void CSsmAdaptationServer::DoGetSimOwnedL(const RMessage2& aMessage)
	{
	if( iSimAdaptation == NULL )
	    {
	    LoadSimAdaptationL();	    
	    }

	iSimAdaptation->DoGetSimOwnedL(aMessage);
	}
void CSsmAdaptationServer::DoGetSimCancelL(const RMessage2& aMessage)
	{
    if( iSimAdaptation == NULL )
        {
        LoadSimAdaptationL();       
        }

	iSimAdaptation->DoGetSimCancel(aMessage);
	}
void CSsmAdaptationServer::DoNotifySimEventL(const RMessage2& aMessage)
	{
	if( iSimAdaptationObservers == NULL )
	    {
	    LoadSimAdaptationObserversL();	    
	    }

	iSimAdaptationObservers->DoNotifySimEventL(aMessage);
	}
void CSsmAdaptationServer::DoNotifySimCancelL(const RMessage2& aMessage)
	{
    if( iSimAdaptationObservers == NULL )
        {
        LoadSimAdaptationObserversL();      
        }

	iSimAdaptationObservers->DoNotifySimEventCancelL(aMessage);
	}
void CSsmAdaptationServer::DoGetLastSimEventL(const RMessage2& aMessage)
	{
    if( iSimAdaptationObservers == NULL )
        {
        LoadSimAdaptationObserversL();      
        }

	iSimAdaptationObservers->DoGetLastSimEvent(aMessage);
	}
// RTC ADAPTATION RELATED

void CSsmAdaptationServer::DoValidateRtcL(const RMessage2& aMessage)
	{
   if( iRtcAdaptation == NULL )
       {
       LoadRtcAdaptationL();       
       }

	iRtcAdaptation->DoValidateRtcL(aMessage);
	}
void CSsmAdaptationServer::DoSetWakeupAlarmL(const RMessage2& aMessage)
	{
	if( iRtcAdaptation == NULL )
	    {
	    LoadRtcAdaptationL();	    
	    }

	iRtcAdaptation->DoSetWakeupAlarmL(aMessage);
	}
void CSsmAdaptationServer::DoUnsetWakeupAlarmL(const RMessage2& aMessage)
	{
    if( iRtcAdaptation == NULL )
        {
        LoadRtcAdaptationL();
        }
	iRtcAdaptation->DoUnsetWakeupAlarmL(aMessage);
	}
void CSsmAdaptationServer::DoRtcCancelL(const RMessage2& aMessage)
	{
    if( iRtcAdaptation == NULL )
        {
        LoadRtcAdaptationL();
        }
	iRtcAdaptation->DoRtcCancelL(aMessage);
	}

//MISC ADAPTATION RELATED
void CSsmAdaptationServer::DoSecurityStateChangeL(const RMessage2& aMessage)
	{
	if( iMiscAdaptation == NULL )
	    {
	    LoadMiscAdaptationL();    
	    }
	
	iMiscAdaptation->DoSecurityStateChangeL(aMessage);
	}
void CSsmAdaptationServer::DoGetGlobalStartupModeL(const RMessage2& aMessage)
	{
    if( iMiscAdaptation == NULL )
        {
        LoadMiscAdaptationL();    
        }

	iMiscAdaptation->DoGetGlobalStartupModeL(aMessage);
	}
void CSsmAdaptationServer::DoPrepareSimLanguagesL(const RMessage2& aMessage)
	{
    if( iMiscAdaptation == NULL )
        {
        LoadMiscAdaptationL();    
        }

	iMiscAdaptation->DoPrepareSimLanguagesL(aMessage);
	}

void CSsmAdaptationServer::DoGetSimLanguagesArrayL(const RMessage2& aMessage)
	{
	if( iMiscAdaptation == NULL )
        {
		LoadMiscAdaptationL();
		}
	iMiscAdaptation->DoGetSimLanguagesArrayL(aMessage);

	}

void CSsmAdaptationServer::DoMiscAdaptationCancelL(const RMessage2& aMessage)
	{
    if( iMiscAdaptation == NULL )
        {
        LoadMiscAdaptationL();    
        }

	iMiscAdaptation->DoMiscAdaptationCancelL(aMessage);
	}

void CSsmAdaptationServer::DoMiscAdaptationHiddenResetL(const RMessage2& aMessage)
	{
    if( iMiscAdaptation == NULL )
        {
        LoadMiscAdaptationL();    
        }

	iMiscAdaptation->DoMiscAdaptationHiddenResetL(aMessage);
	}

//EMERGENCY CALL RF ADAPTATION RELATED 
void CSsmAdaptationServer::DoActivateRfForEmergencyCallL(const RMessage2& aMessage)
	{
	if( iEmergencyCallRfAdaptation == NULL )
	    {
	    LoadEmergencyCallRfAdaptationL();    
	    }
	
	iEmergencyCallRfAdaptation->DoActivateRfForEmergencyCallL(aMessage);
	}
void CSsmAdaptationServer::DoDeactivateRfForEmergencyCallL(const RMessage2& aMessage)
	{
    if( iEmergencyCallRfAdaptation == NULL )
        {
        LoadEmergencyCallRfAdaptationL();    
        }

	iEmergencyCallRfAdaptation->DoDeactivateRfForEmergencyCallL(aMessage);
	}
void CSsmAdaptationServer::DoEmergencyCallRfCancelL(const RMessage2& aMessage)
	{
    if( iEmergencyCallRfAdaptation == NULL )
        {
        LoadEmergencyCallRfAdaptationL();    
        }
	iEmergencyCallRfAdaptation->DoEmergencyCallRfAdaptationCancelL(aMessage);
	}
	
void CSsmAdaptationServer::LoadStateAdaptationL()
	{
	MStateAdaptation *stateAdaptationPlugin = iSusAdaptationPluginLoader->CreateStateAdaptationL();
    CleanupReleasePushL(*stateAdaptationPlugin);
    //From here stateAdaptationPlugin object will be owned by iStateAdaptation
    iStateAdaptation = CStateAdaptationRequests::NewL(*stateAdaptationPlugin);
    CleanupStack::Pop(stateAdaptationPlugin);
	}

void CSsmAdaptationServer::LoadStateAdaptationObserversL()
	{
	if( iStateAdaptation == NULL )
	    {
	    // Load the State adaptation to use it's adaptation plugin
	    LoadStateAdaptationL();
	    }
	iStateAdaptationObservers = CStateAdaptationObservers::NewL(iStateAdaptation->Adaptation());
	}

void CSsmAdaptationServer::LoadSimAdaptationL()
	{
	MSimAdaptation *simAdaptation = iSusAdaptationPluginLoader->CreateSimAdaptationL();
    CleanupReleasePushL(*simAdaptation);
    //From here simAdaptation plugin object will be owned by iSimAdaptation
    iSimAdaptation = CSimAdaptationRequests::NewL(*simAdaptation);
    CleanupStack::Pop(simAdaptation);
	}
void CSsmAdaptationServer::LoadSimAdaptationObserversL()
	{
    if( iSimAdaptation == NULL )
        {
        // Load the SIM adaptation to use it's adaptation plugin
        LoadSimAdaptationL();
        }
    iSimAdaptationObservers = CSimAdaptationObservers::NewL(iSimAdaptation->Adaptation());
	}
void CSsmAdaptationServer::LoadRtcAdaptationL()
	{
	MRtcAdaptation *rtcAdaptationPlugin = iSusAdaptationPluginLoader->CreateRtcAdaptationL();
    CleanupReleasePushL(*rtcAdaptationPlugin);
    //From here rtcAdaptationPlugin object object will be owned by iRtcAdaptation
    iRtcAdaptation = CRtcAdaptation::NewL(*rtcAdaptationPlugin);
    CleanupStack::Pop(rtcAdaptationPlugin);
	}
void CSsmAdaptationServer::LoadMiscAdaptationL()
	{
	MMiscAdaptation *miscAdaptationPlugin = iSusAdaptationPluginLoader->CreateMiscAdaptationL();
    CleanupReleasePushL(*miscAdaptationPlugin);
    //From here miscAdaptationPlugin object will be owned by iMiscAdaptation
    iMiscAdaptation = CMiscAdaptation::NewL(*miscAdaptationPlugin);
    CleanupStack::Pop(miscAdaptationPlugin);
	}
void CSsmAdaptationServer::LoadEmergencyCallRfAdaptationL()
	{
	MEmergencyCallRfAdaptation *emergencyCallRfAdaptationPlugin = iSusAdaptationPluginLoader->CreateEmergencyCallRfAdaptationL();
    CleanupReleasePushL(*emergencyCallRfAdaptationPlugin);
	//From here emergencyCallRfAdaptationPlugin object will be owned by iEmergencyCallRfAdaptation
    iEmergencyCallRfAdaptation = CEmergencyCallRfAdaptation::NewL(emergencyCallRfAdaptationPlugin);    
    CleanupStack::Pop(emergencyCallRfAdaptationPlugin);
	}

#ifdef _DEBUG
void CSsmAdaptationServer::UnloadAdaptationsAndObservers()
{
	delete iStateAdaptation;
	iStateAdaptation = NULL;
	delete iStateAdaptationObservers;
	iStateAdaptationObservers = NULL;
	delete iSimAdaptation;
	iSimAdaptation = NULL;
	delete iSimAdaptationObservers;
	iSimAdaptationObservers = NULL;
	delete iEmergencyCallRfAdaptation;
	iEmergencyCallRfAdaptation = NULL;
	delete iMiscAdaptation;
	iMiscAdaptation = NULL;
	delete iRtcAdaptation;
	iRtcAdaptation = NULL;
}
#endif

/**
 * Sets the calling client as a priority client. This also loads the emergency client dll,
 * if it is not already loaded.
 */
TInt CSsmAdaptationServer::SetAsPriorityClientL(CSsmAdaptationSession* aSession)
    {
    if(iPriorityClientSession == NULL )
        {
        if(iEmergencyCallRfAdaptation == NULL)
            {
            LoadEmergencyCallRfAdaptationL();
            }
        iPriorityClientSession = aSession;
        iEmergencyCallRfAdaptation->SetPriorityClientSession(iPriorityClientSession);
        return KErrNone;
        }
    return KErrAlreadyExists;
    }

/*
 * Remove priority Client
 */
void CSsmAdaptationServer::RemovePriorityClient(CSsmAdaptationSession* aSession)
    {
    if(aSession == iPriorityClientSession)
        {
        iPriorityClientSession = NULL; 
        iEmergencyCallRfAdaptation->RemovePriorityClientSession();
        } 
    }

//RMessage2 related functionality
//C Class

void CAdaptationMessage::ReadL(TInt aParamNumber,TPtr8 aParam)
	{
	iMessage.ReadL(aParamNumber,aParam);			
	}

TInt CAdaptationMessage::GetDesLength(TInt aParam)
	{
	return iMessage.GetDesLength(aParam);
	}

TInt CAdaptationMessage::Function() const
	{
	return iMessage.Function();	
	}

CSession2 *CAdaptationMessage::Session() const
	{
	return iMessage.Session();
	}

TInt CAdaptationMessage::Int0() const
	{
	return iMessage.Int0();
	}

void CAdaptationMessage::WriteL(TInt aParam, const TDesC8 &aDes)
	{
	iMessage.WriteL(aParam,aDes);
	}

CAdaptationMessage::CAdaptationMessage(const RMessage2 &aMessage): CBase(),iMessage(aMessage)
	{
	}
 
void CAdaptationMessage::Complete(TInt aReason)
	{
    if (!(iMessage.IsNull()))
        {
        iMessage.Complete(aReason);  	
        }
	}

void CAdaptationMessage::SetMessage(const RMessage2& aMessage)
    {
    iMessage = const_cast<RMessage2&> (aMessage);
    }

void RSsmAdaptationRequestQueue::NotifyAndRemoveAll()
	{
	TInt index,count = iQueue.Count();
	
	for(index =0;index < count ;index++)
		{
		iQueue[index]->Complete(KErrServerTerminated);
		delete iQueue[index];
		iQueue[index] = NULL;
		}
	iQueue.Reset();		
	}

void RSsmAdaptationRequestQueue::Close()
	{
	iQueue.Close();
	}
TBool RSsmAdaptationRequestQueue::IsEmpty()
	{
	if(iQueue.Count() == 0)
		{
		return ETrue;	
		}
	else
		{
		return EFalse;	
		}
	}
void RSsmAdaptationRequestQueue::RemoveFromQueueAndComplete(const RMessage2 &aMessage)
	{

	CAdaptationMessage *messageToBeDeleted;
	
	// iQueue.Count() is recalculated each time as the object is removed from the 
	// array(which reduces the number of object pointers in the array
	for(TInt index = 0; index < iQueue.Count(); index++ )
		{
		if(aMessage.Session() == iQueue[index]->Session())
			{
			messageToBeDeleted = iQueue[index];
			DEBUGPRINT2A("RSsmAdaptationRequestQueue cancelling request with function id: %d", messageToBeDeleted->Function());
			iQueue.Remove(index);
			messageToBeDeleted->Complete(KErrCancel);
			delete messageToBeDeleted;
			index--;				
			}
		}
	
	}

void RSsmAdaptationRequestQueue::Dequeue(CAdaptationMessage *&aCurrentMessage)
	{
	aCurrentMessage = iQueue[0];
	iQueue.Remove(0);
	}
TInt RSsmAdaptationRequestQueue::Queue(CAdaptationMessage *aPendingRequest)
	{
	return iQueue.Append(aPendingRequest);
	} 

/**
 * Method to get number of object pointers in the Queue
 */
TInt RSsmAdaptationRequestQueue::Count()
    {
    return iQueue.Count();
    } 

