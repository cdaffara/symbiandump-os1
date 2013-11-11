/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include <e32base.h>
#include <s32mem.h>
#ifdef SYMBIAN_FEATURE_MANAGER
	#include <featdiscovery.h>
	#include <featureuids.h>
#endif
#include <lbs.h>
#include <lbs/lbsadmin.h>
#include "lbslocservermessageenums.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_mposmodulesobserver.h>
#include "epos_proxypositionerconstructparams.h"
#include "lbsdevloggermacros.h"

#include "epos_cposmodulessettings.h"
#include "EPos_Global.h"
#include "EPos_ServerPanic.h"

#include "testEPos_CPositionRequest.h"
#include "testEPos_CPosSubSession.h"


// TODO Verify
#include "testEPos_CPosLocMonitorReqHandlerHub.h"

// CONSTANTS
#ifdef _DEBUG
_LIT(KTraceFileName, "testEPos_CPosSubSession.cpp");
#endif

const TInt KParamRequestorType = 0;
const TInt KParamRequestorFormat = 1;
const TInt KParamRequestorData = 2;
const TInt KParamRequestorStack = 0;
const TInt KParamUpdateOptions = 0;


// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// ResetAndDestroy Used for cleanup.
// This function is passed to a TCleanupItem
// ---------------------------------------------------------
//
void ResetAndDestroyRRequestorStack(TAny* aPtr)
    {
    (reinterpret_cast<RRequestorStack*> (aPtr))->ResetAndDestroy();
    }

// ================= MEMBER FUNCTIONS =======================

/**
 *  C++ default constructor
 */
CPosSubSession::CPosSubSession() : iRequestors()
    {
    
    }

/**
 *  Symbian default constructor
 */
void CPosSubSession::ConstructL(
        CPosModuleSettings& aModuleSettings,
        CPosLocMonitorReqHandlerHub& aLocMonitorReqHandlerHub,
        TPositionModuleId aImplementationUid,
        MPosStatusObserver* aStatusObserver,
        MPosModuleStatusManager* aModuleStatusManager,
        TBool aIsProxy)
    {
    TProxyPositionerConstructParams positionerParams;
    positionerParams.iImplementationUid = aImplementationUid;
    positionerParams.iStatusObserver = aStatusObserver;
    positionerParams.iParamObserver = this;
    positionerParams.iModuleSettingsManager = &aModuleSettings;
    positionerParams.iModuleStatusManager = aModuleStatusManager; 

	iPositionRequest = CPositionRequest::NewL(
        aModuleSettings,
        aLocMonitorReqHandlerHub,
        positionerParams, 
        aIsProxy);
	



	//Need to call it to propogate iPsyDefaultUpdateTimeOut in case  
	//SetPsyDefaultUpdateTimeOut was called in the CPositionRequest::NewL
	//See SetPsyDefaultUpdateTimeOut implementation
	SetPsyDefaultUpdateTimeOut(iPsyDefaultUpdateTimeOut);

	iLocMonitorReqHandlerHub = &aLocMonitorReqHandlerHub;
    
#ifdef SYMBIAN_FEATURE_MANAGER
	iLocationManagementSupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KLocationManagement);
#else
	__ASSERT_ALWAYS(EFalse, User::Invariant());	// Would happen on older versions of symbian OS if this code ever backported
#endif    
	
	if(iLocationManagementSupported)
		{
		iAdmin = CLbsAdmin::NewL();
		// connect to network registration status interface
		iNetRegStatus.OpenL();
		}
    }

/**
 * Two-phased constructor.
 *
 * @param aLastPositionHandler Last position handler.
 * @param aModulesDb the position log database.
 * @param aImplementationUid the positioner implementation uid.
 * @param aIsProxy ETrue if aImplementationUid represents a proxy PSY,
 *                 EFalse otherwise.
 * @param aStatusObserver status observer to be used by positioner.
 * @param aLocMonSession Location Monitor session handle shared by all subsessions
 */
CPosSubSession* CPosSubSession::NewLC(
    CPosModuleSettings& aModuleSettings,
    CPosLocMonitorReqHandlerHub& aLocMonitorReqHandlerHub,
    TUid aImplementationUid,
    TBool aIsProxy,
    MPosStatusObserver* aStatusObserver,
    MPosModuleStatusManager* aModuleStatusManager)

    {
    CPosSubSession* self = new (ELeave) CPosSubSession();
    CleanupClosePushL(*self);
    
    self->ConstructL(
        aModuleSettings, 
        aLocMonitorReqHandlerHub, 
        aImplementationUid, 
        aStatusObserver, 
        aModuleStatusManager,
        aIsProxy);
        
    return self;
    }

/**
 * Destructor.
 */
CPosSubSession::~CPosSubSession()
    {
    if(iLocationManagementSupported)
    	{
    	delete iAdmin;
    	iNetRegStatus.Close();
    	}
    iRequestors.ResetAndDestroy();    
    delete iPositionRequest;

    }


/**
 * This function is responsible for handling the
 * servicing of client requests to the server.
 * It uses the message passed as an argument to
 * obtain the request opcode and to access client
 * addresses for reading and writing.
 *
 * @param aMessage The current message
 */
void CPosSubSession::ServiceL(const RMessage2& aMessage)
    {
    switch (aMessage.Function())
        {
	    case ELbsSetSingleRequestor:
		    HandleSetRequestorL(aMessage);
		    break;
	    case ELbsSetMultipleRequestors:
		    HandleSetRequestorStackL(aMessage);
		    break;
	    case ELbsSetUpdateOptions:
		    HandleSetUpdateOptionsL(aMessage);
		    break;
	    case ELbsGetUpdateOptions:
		    HandleGetUpdateOptionsL(aMessage);
		    break;
	    case ELbsGetLastKnownPosition:
			LBS_RDEBUG("Client", "LBS", "GetLastKnownPosition");
		    HandleGetLastKnownPositionL(aMessage);
		    break;
	    case ELbsGetLastKnownPositionArea:
	    	HandleGetLastKnownPositionAreaL(aMessage); // TODO Verify
	    	break;
	    case ELbsPosNotifyPositionUpdate:
			LBS_RDEBUG("Client", "LBS", "NotifyPositionUpdate");
		    HandleNotifyPositionUpdateL(aMessage);
		    break;
	    case ELbsPositionerCancelAsyncRequest:
			LBS_RDEBUG("Client", "LBS", "CancelAsyncRequest");
            HandleCancelAsyncRequestL(aMessage);
            break;
        default:
        	DEBUG_TRACE("default switch case subsession ", __LINE__)
            User::Leave(KErrNotSupported);
		    break;
        }
    }

/**
 * From MPosParameterObserver.
 *
 * Get the position quality requested by the client.
 *
 * @param aRequestedPosQuality The requested position quality.
 */
TInt CPosSubSession::GetRequiredPositionQuality(TPositionQuality& /*aRequestedPosQuality*/) const
    {
    DEBUG_TRACE("GetRequiredPositionQuality()", __LINE__)

    // Not implemented yet.
    return KErrNotFound;
    }

/**
 * From MPosParameterObserver.
 *
 * Get the earliest allowed time of an old position fix, based on the
 * current max age set by the client.
 *
 * The PSY may save the result from the latest position request and
 * return the same result in the next position request if the client
 * says it's ok. Max age tells the PSY how old the stored position is
 * allowed to be. If the stored position is too old or the PSY does not
 * support max age, a normal positioning should be performed.
 *
 * @param aMaxAge On return, will contain the earliest allowed time of 
 *   an old position fix. If no max age is defined aMaxAge will contain 
 *   a time set to zero.
 */
void CPosSubSession::GetMaxAge(TTime& aMaxAge) const
    {
    DEBUG_TRACE("GetMaxAge()", __LINE__)

    if (iMaxUpdateAge != TTimeIntervalMicroSeconds(0))
        {
        aMaxAge.UniversalTime();
        aMaxAge -= iMaxUpdateAge;
        }
    else
        {
        aMaxAge = TTime(0);
        }
    }

/**
 * From MPosParameterObserver.
 *
 * Checks if the client allows a partial position update.
 *
 * A partial update result does not need to contain all parameters. The
 * only mandatory parameters are latitude, longitude and time of fix.
 * Everything else is optional.
 *
 * If a partial update is returned to the client in a
 * NotifyPositionUpdate() call, the completion code must be set to
 * KPositionPartialUpdate.
 * 
 * @return ETrue if partial position updates are allowed, otherwise
 *   EFalse.
 */
TBool CPosSubSession::IsPartialUpdateAllowed() const
    {
    DEBUG_TRACE("IsPartialUpdateAllowed()", __LINE__)
    return iAcceptPartialUpdates;
    }

/**
 * Called when a change has occurred in location settings db.
 * @param aEvent Event information
 */
void CPosSubSession::HandleSettingsChangeL(TPosModulesEvent aEvent)
    {
    __ASSERT_DEBUG(iPositionRequest, DebugPanic(EPosServerPanicGeneralInconsistency));
    iPositionRequest->HandleSettingsChangeL(aEvent);
    }

/**
 * Called when the server class is shutting down.
 */
void CPosSubSession::NotifyServerShutdown()
    {
    DEBUG_TRACE("NotifyServerShutdown", __LINE__)
    
    iPositionRequest->NotifyServerShutdown();
    }

void CPosSubSession::HandleSetRequestorL(const RMessage2& aMessage)
	{
    DEBUG_TRACE("EPositionerSetSingleRequestor", __LINE__)

    // Destroy previously set requestors before any leaving code is called.
	// If this method leaves, the requestor array must be empty so any
	// subsequent call to NotifyPositionUpdate returns KErrAccessDenied.
	iRequestors.ResetAndDestroy();

	TPckgBuf<CRequestor::TRequestorType> requestorType;
    User::LeaveIfError(Global::Read(aMessage, KParamRequestorType, requestorType));

    // the requestor must be a service
    if (requestorType() != CRequestor::ERequestorService)
        {
        User::Leave(KErrArgument);
        }

	TPckgBuf<CRequestor::TRequestorFormat> requestorFormat;
    User::LeaveIfError(Global::Read(aMessage, KParamRequestorFormat, requestorFormat));

    HBufC* data = Global::CopyClientBufferLC(aMessage, KParamRequestorData);

    CRequestor* requestor = CRequestor::NewLC(requestorType(), requestorFormat(), *data);
    ValidateRequestorL(requestor);
    iRequestors.Append(requestor); // takes ownership
    CleanupStack::Pop(requestor);

    iPositionRequest->NewTrackingSessionIfTracking();
    RequestComplete(aMessage, KErrNone);

    CleanupStack::PopAndDestroy(data);
	}

void CPosSubSession::HandleSetRequestorStackL(const RMessage2& aMessage)
	{
    DEBUG_TRACE("EPositionerSetMultipleRequestors", __LINE__)

    // Destroy previously set requestors before any leaving code is called.
	iRequestors.ResetAndDestroy();

    HBufC8* buf = Global::CopyClientBuffer8LC(aMessage, KParamRequestorStack);
	RDesReadStream stream(*buf);

	iRequestors.InternalizeL(stream);

	CleanupStack::PopAndDestroy(buf);

	// If this method leaves, the requestor array must be empty so any
	// subsequent call to NotifyPositionUpdate returns KErrAccessDenied.
    TCleanupItem cleanup(ResetAndDestroyRRequestorStack, &iRequestors);
    CleanupStack::PushL(cleanup);

    // Last requestor must be a service
    if (iRequestors.Count() > 0)
        {
        CRequestor* last = iRequestors[iRequestors.Count() - 1];
        if (last->RequestorType() != CRequestor::ERequestorService)
            {
            User::Leave(KErrArgument);
            }
        }
    else
        {
        User::Leave(KErrArgument);
        }

    // validate them all
    for (TInt i = 0; i < iRequestors.Count(); i++)
        {
        ValidateRequestorL(iRequestors[i]);
        }

    CleanupStack::Pop(&iRequestors);

    iPositionRequest->NewTrackingSessionIfTracking();
	RequestComplete(aMessage, KErrNone);
	}

void CPosSubSession::HandleSetUpdateOptionsL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionerSetUpdateOptions", __LINE__)

    HBufC8* optionsBuf = Global::CopyClientBuffer8LC(aMessage, KParamUpdateOptions);
    TUint bufferSize = optionsBuf->Size();

    TPositionUpdateOptionsBase& optionsBase = reinterpret_cast
        <TPositionUpdateOptionsBase&>(const_cast<TUint8&>(*optionsBuf->Ptr()));
    
    // Check update options. Leaves if not accepted values.

    Global::ValidatePositionClassBufferL(optionsBase, optionsBuf->Des());
    Global::ValidatePositionClassTypeL(optionsBase, EPositionUpdateOptionsClass);

    TPositionUpdateOptions& options = 
        static_cast<TPositionUpdateOptions&>(optionsBase);

    if ((optionsBase.UpdateTimeOut().Int64() > 0) &&
        (optionsBase.UpdateInterval() >= optionsBase.UpdateTimeOut()))
        {
        User::Leave(KErrArgument);
        }

    // Max age must be less than the update interval if both are set.
	if (optionsBase.MaxUpdateAge().Int64() != 0 &&
        optionsBase.UpdateInterval().Int64() != 0 &&
        optionsBase.MaxUpdateAge() >= optionsBase.UpdateInterval())
        {
        User::Leave(KErrArgument);
        }

    iTracking = (optionsBase.UpdateInterval().Int64() != 0);
    iMaxUpdateAge = optionsBase.MaxUpdateAge();
    iAcceptPartialUpdates = options.AcceptPartialUpdates();
    iUpdateTimeOut = optionsBase.UpdateTimeOut();
    
    if(iUpdateTimeOut==0 && iPsyDefaultUpdateTimeOut>0)
    	{
    	optionsBase.SetUpdateTimeOut(iPsyDefaultUpdateTimeOut);
    	}
    
    iPositionRequest->SetUpdateOptions(optionsBase);

    CleanupStack::PopAndDestroy(optionsBuf);
    RequestComplete(aMessage, KErrNone);
    }
  
void CPosSubSession::HandleGetUpdateOptionsL(const RMessage2& aMessage)
    {    
    DEBUG_TRACE("EPositionerGetUpdateOptions", __LINE__)

    HBufC8* optionsBuf = Global::CopyClientBuffer8LC(aMessage, KParamUpdateOptions);
    TUint bufferSize = optionsBuf->Size();

    TPositionUpdateOptionsBase& optionsBase = reinterpret_cast
        <TPositionUpdateOptionsBase&>(const_cast<TUint8&>(*optionsBuf->Ptr()));
    
    Global::ValidatePositionClassBufferL(optionsBase, optionsBuf->Des());
    Global::ValidatePositionClassTypeL(optionsBase, EPositionUpdateOptionsClass);

    TPositionUpdateOptions& options = static_cast<TPositionUpdateOptions&>(optionsBase);

    iPositionRequest->GetUpdateOptions(optionsBase);
    optionsBase.SetMaxUpdateAge(iMaxUpdateAge);
    options.SetAcceptPartialUpdates(iAcceptPartialUpdates);
    optionsBase.SetUpdateTimeOut(iUpdateTimeOut);
    
    User::LeaveIfError(Global::Write(aMessage, KParamUpdateOptions, *optionsBuf));
    CleanupStack::PopAndDestroy(optionsBuf);
    RequestComplete(aMessage, KErrNone);
    }

void CPosSubSession::HandleGetLastKnownPositionL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionerGetLastKnownPosition", __LINE__)

    if (aMessage.Int1() != EPositionInfoClass)
    	{
    	User::Leave(KErrArgument);
    	}
   
    if ( aMessage.Int2() < sizeof(TPositionInfo) )
    	{
    	User::Leave(KErrArgument);
    	}

    // Request the last known postion area from the CPosLocMonitorReqHandlerHub
    iLocMonitorReqHandlerHub->GetLastKnownPosReqL(aMessage);
    }

/* Check that a Self-locate is allowed, given the current network roaming status
*/
TBool CPosSubSession::IsSelfLocateEnabled()
	{
	DEBUG_TRACE("CPositionerSubSession::IsSelfLocateEnabled", __LINE__);
	__ASSERT_DEBUG((iAdmin != NULL) && (iLocationManagementSupported != EFalse), DebugPanic(EPosServerPanicGeneralInconsistency));
	
	TBool selfLocateAllowed(EFalse);
	CLbsAdmin::TSelfLocateService serviceStatus(CLbsAdmin::ESelfLocateOff);	
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus netRegStatus(RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown);
	
	// Read the current network registration	 status
	TInt err = iNetRegStatus.GetNetworkRegistrationStatus(netRegStatus);
	if (err == KErrNone)
		{
		// Read the appropriate admin setting
		switch (netRegStatus)
			{
			case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
				{
				err = iAdmin->Get(KLbsSettingHomeSelfLocate, serviceStatus);
				break;
				}
			case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
				{
				err = iAdmin->Get(KLbsSettingRoamingSelfLocate, serviceStatus);
				break;
				}		
			case RLbsNetworkRegistrationStatus::ENotRegistered:
				{
				err = iAdmin->Get(KLbsSettingRoamingSelfLocate, serviceStatus);
				if(!err && serviceStatus == CLbsAdmin::ESelfLocateOff)
					{
					// in the case of notregistered we allow selflocate if *either* home or roaming setting is on:
					err = iAdmin->Get(KLbsSettingHomeSelfLocate, serviceStatus);
					}
				}
				break;
			case RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown:
			default:
				{
				serviceStatus = CLbsAdmin::ESelfLocateOff;
				break;
				}
			}
		}
	
	// Check to see if Self Locate is allowed
	if (err == KErrNone
	    && serviceStatus == CLbsAdmin::ESelfLocateOn)
		{
		selfLocateAllowed = ETrue;
		}
	
	return selfLocateAllowed;
	}

void CPosSubSession::HandleNotifyPositionUpdateL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionerNotifyPositionUpdate", __LINE__)
    
    // Request cannot be made if one is already in progress
    //
    if (iPositionRequest->IsActive())
        {
        aMessage.Panic(KPosClientFault, EPositionDuplicateRequest);
        iPositionRequest->Cancel();
        return;
        }
    else if (iLocationManagementSupported && !IsSelfLocateEnabled())
    	{
		LBS_RDEBUG_ARGTEXT("LBS", "Client", "RunL", "KErrAccessDenied");
		aMessage.Complete(KErrAccessDenied); 
  		return;
    	}   
    
    // Initiate the request
    iPositionRequest->MakeRequestL(aMessage);
    }

void CPosSubSession::HandleGetLastKnownPositionAreaL(const RMessage2& aMessage)
	{
	DEBUG_TRACE("ELbsGetLastKnownPositionArea", __LINE__)
	
	// Request the last known postion area from the CPosLocMonitorReqHandlerHub
	iLocMonitorReqHandlerHub->GetLastKnownPosAreaReqL(aMessage);

	}

void CPosSubSession::HandleGetLastKnownPositionAreaCancelL(const RMessage2& aMessage)
	{
	//TODO - Implementation - verify the string in debug_trace below
	DEBUG_TRACE("ELbsGetLastKnownPositionAreaCancelL ", __LINE__)
	
	iLocMonitorReqHandlerHub->CancelGetLastKnownPosAreaReqL(aMessage);
	
	}

void CPosSubSession::HandleNotifyPositionUpdateCancelL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionerNotifyPositionUpdateCancel", __LINE__)

    if (!iPositionRequest->IsActive())
        {
        User::Leave(KErrNotFound);
        }

    iPositionRequest->Cancel();
    RequestComplete(aMessage, KErrNone);
    }

void CPosSubSession::HandleGetLastKnownPositionCancelL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionerGetLastKnownPositionCancel", __LINE__)
    iLocMonitorReqHandlerHub->CancelGetLastKnownPosReqL(aMessage);
    
    }


void CPosSubSession::HandleCancelAsyncRequestL(const RMessage2& aMessage)
    {
    switch (aMessage.Int0())
        {
        case KGetLastKnownPositionSymbian:
        case KGetLastKnownPositionVariant:
        case ELbsGetLastKnownPosition:						//TODO Verify
	        HandleGetLastKnownPositionCancelL(aMessage);
            break;
        case KNotifyPositionUpdateSymbian:
        case KNotifyPositionUpdateVariant:
        case ELbsPosNotifyPositionUpdate:					//TODO Verify
	        HandleNotifyPositionUpdateCancelL(aMessage);
            break;
        case ELbsGetLastKnownPositionArea:					//TODO Verify
        	HandleGetLastKnownPositionAreaCancelL(aMessage);
        	break;
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    }

void CPosSubSession::ValidateRequestorL(CRequestor* aRequestor)
    {
    // service is identified by application name
    switch (aRequestor->RequestorType())
        {
        case CRequestor::ERequestorService:
            break;
        case CRequestor::ERequestorContact:
            if (aRequestor->RequestorFormat() == CRequestor::EFormatApplication)
                {
                User::Leave(KErrArgument);
                }
            break;
        default:
            User::Leave(KErrArgument); // service or contact is required
            break;
        }

    switch (aRequestor->RequestorFormat())
        {
        case CRequestor::EFormatApplication:
        case CRequestor::EFormatTelephone:
        case CRequestor::EFormatUrl:
        case CRequestor::EFormatMail:
            break;
        default:
            User::Leave(KErrArgument); // Unknown or other
            break;
        }
    }

void CPosSubSession::RequestComplete(const RMessage2& aMessage, TInt aCompleteCode)
    {
    if (!aMessage.IsNull())
        {
        aMessage.Complete(aCompleteCode);
        }
    }

void CPosSubSession::GetUpdateTimeOut(TTimeIntervalMicroSeconds& aUpdateTimeOut) const
	{
	if(iUpdateTimeOut==0)
		{
		aUpdateTimeOut = iPsyDefaultUpdateTimeOut;
		}
	else
		{
		aUpdateTimeOut = iUpdateTimeOut;
		}
	}

void CPosSubSession::SetPsyDefaultUpdateTimeOut(const TTimeIntervalMicroSeconds& aUpdateTimeOut)
	{
	iPsyDefaultUpdateTimeOut = aUpdateTimeOut;
	
	if(iPositionRequest) //It can be zero if this method is called back from CPosSubSession::ConctructL
		{
		TPositionUpdateOptions updateOptions;
		iPositionRequest->GetUpdateOptions(updateOptions);
		if(iUpdateTimeOut==0)
			{
			updateOptions.SetUpdateTimeOut(iPsyDefaultUpdateTimeOut);
			iPositionRequest->SetUpdateOptions(updateOptions);
			}
		}
	}

void CPosSubSession::ExtendUpdateTimeOut(const TTimeIntervalMicroSeconds& aAdditionalTime)
	{
	iPositionRequest->ExtendUpdateTimeOut(aAdditionalTime);
	}



