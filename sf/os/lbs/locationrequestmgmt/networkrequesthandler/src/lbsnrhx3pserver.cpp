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
// Description:
// server side implementation of the LBS X3P request
// 
//

#include "lbsdevloggermacros.h"
#include "lbsnrhx3pserver.h"
#include "nrhpanic.h"
#include "opensessionparams.h"
#include <lbs/lbslocerrors.h>
#include "lbsnrhmessageenums.h"
#include <lbs/lbstransmitlocationlogevent.h>
#include "lbsqualityprofile.h"
#include "LbsInternalInterface.h"
#include "lbsdevloggermacros.h"
#include "lbssatellite.h"
#include <lbs/lbsgpsmeasurement.h>

CNrhX3pServerSubsession::CNrhX3pServerSubsession(RLbsLogger& aLogger) :
	iTransmitOptions(TTimeIntervalMicroSeconds(10000000)),
	iSessionId(),
	iRefLocErrorCode(KErrNone),
	iLogger(aLogger)
	{
	}

CNrhX3pServerSubsession::~CNrhX3pServerSubsession()
	{
	if (iServer->X3pObserver() != NULL)
		{		
		iServer->X3pObserver()->RemoveObserver(this);
		}

	// cancel ANY outstanding requests with KErrServerTerminated
	// or some other error code to indicate that the session has been closed
	// early	
	if (!iTransmitPositionMessage.IsNull())
		{
#ifdef ENABLE_LBS_DEV_LOGGER
		TFileName* processName = new TFileName;
		if (processName != NULL)
			{
			LBSLOG_GETPROCESSNAME(iTransmitPositionMessage, *processName);
			if (processName->Length() > 0)
				{
				LBSLOG2(ELogP9, "->A RLbsTransmitPosition::RunL(TransmitPosition) %S\n", processName);
				LBSLOG(ELogP9, "  Return  = KErrServerTerminated\n");
				}
			delete processName;
			}
#endif
		iTransmitPositionMessage.Complete(KErrServerTerminated);
		}

	if (!iReferencePositionMessage.IsNull())
		{
#ifdef ENABLE_LBS_DEV_LOGGER
		TFileName* processName = new TFileName;
		if (processName != NULL)
			{
			LBSLOG_GETPROCESSNAME(iReferencePositionMessage, *processName);
			if (processName->Length() > 0)
				{
				LBSLOG2(ELogP9, "->A RLbsTransmitPosition::RunL(ReportReferenceLocation) %S\n", processName);
				LBSLOG(ELogP9, "  Return  = KErrServerTerminated\n");
				}
			delete processName;
			}
#endif
		iReferencePositionMessage.Complete(KErrServerTerminated);
		}
	}

CNrhX3pServerSubsession* CNrhX3pServerSubsession::NewL(RLbsLogger& aLogger)
	{
	CNrhX3pServerSubsession* self = new (ELeave) CNrhX3pServerSubsession(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CNrhX3pServerSubsession::ConstructL()
	{
	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);		
	// Set the default X3P timeout to one in the current profile:
	
	// Retrieve the Id of the quality profile to use
    TLbsQualityProfileId profileId(KLbsNullQualityProfileId);
    admin->Get(KLbsSettingQualityProfileTransmitLocate, profileId);
	
    // Retrieve the data for the quality profile
	TQualityProfile qualityProfile;
	TInt err = LbsQualityProfile::GetQualityProfileById(profileId, qualityProfile);
	if (err == KErrNone)
		{
		// Use the maxfix time from the quality profile
	    iTransmitOptions.SetTimeOut(qualityProfile.MaxFixTime());
		}
	CleanupStack::PopAndDestroy(admin);	
	}
	
// from MSubSessionImpl
void CNrhX3pServerSubsession::DispatchL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
		case EX3pTransmitPosition:
			TransmitPosition(aMessage);
			break;
			
		case EX3pCancelTransmitPosition:
			CancelTransmitPosition(aMessage);
			break;
			
		case EX3pSetTransmitOptions:
			SetTransmitOptions(aMessage);
			break;
			
		case EX3pGetTransmitOptions:
			GetTransmitOptions(aMessage);
			break;
			
		case EX3pReportReferenceLocation:
			ReportReferenceLocation(aMessage);
			break;
		}
	
	}
void CNrhX3pServerSubsession::DispatchError(const RMessage2& /*aMessage*/, 
														TInt /*aError*/)
	{
	
	}
	
void CNrhX3pServerSubsession::CreateSubSessionL(const RMessage2& /*aMessage*/,
												const CSecureServerBase* aServer)
	{
	__ASSERT_DEBUG(aServer != NULL, Panic(ENrhPanicInvalidServerPointer));
	
	// Get hold of something for this subsession 
	// to pass on its messages to	
	iServer = static_cast<const CNrhServer*>(aServer);
	
	// Register as an observer of the X3P Handler
	if (iServer->X3pObserver() != NULL)
		{
		iServer->X3pObserver()->AddObserverL(this);
		}
	}
	
void CNrhX3pServerSubsession::CloseSubSession()
	{
	}
	
// via MSubSessionImpl::MRelease	
void CNrhX3pServerSubsession::VirtualRelease()
	{			
	//delete ourselves - can be called on a via CS on a leave
	delete this; // we are allocated via GetImpl(), and a handle to "us" is returned	
	}
	
void CNrhX3pServerSubsession::TransmitPosition(const RMessage2& aMessage)
	{
	// If a preceding request for reference location failed
	// make this request fail with the same error code.
	// 
	if (iRefLocErrorCode != KErrNone)
		{
#ifdef ENABLE_LBS_DEV_LOGGER
		TFileName* processName = new TFileName;
		if (processName != NULL)
			{
			LBSLOG_GETPROCESSNAME(aMessage, *processName);
			if (processName->Length() > 0)
				{
				LBSLOG2(ELogP9, "->A RLbsTransmitPosition::RunL(TransmitPosition) %S\n", processName);
				LBSLOG2(ELogP9, "  Return  = %d\n", iRefLocErrorCode);
				}
			delete processName;
			}
#endif
		aMessage.Complete(iRefLocErrorCode);
		iRefLocErrorCode = KErrNone;
		return;
		}
	
	// Check that this sub-session doesn't already 
	// have an outstanding request
	if (!iTransmitPositionMessage.IsNull())
		{
#ifdef ENABLE_LBS_DEV_LOGGER
		TFileName* processName = new TFileName;
		if (processName != NULL)
			{
			LBSLOG_GETPROCESSNAME(aMessage, *processName);
			if (processName->Length() > 0)
				{
				LBSLOG2(ELogP9, "->A RLbsTransmitPosition::Panic(TransmitPosition) %S\n", processName);
				LBSLOG2(ELogP9, "  Return  = %d\n", EPositionDuplicateRequest);
				}
			delete processName;
			}
#endif
		aMessage.Panic(KPosClientFault, EPositionDuplicateRequest);
		return;
		}
	
	// Check that Transmit Position is allowed by
	// the LbsAdmin setting
	if (!CheckTransmitPositionAdminSettings())
		{
		// Transmit Position not allowed; complete the client
		// requests with an error
		
		if (!iReferencePositionMessage.IsNull())
			{
#ifdef ENABLE_LBS_DEV_LOGGER
			TFileName* processName = new TFileName;
			if (processName != NULL)
				{
				LBSLOG_GETPROCESSNAME(iReferencePositionMessage, *processName);
				if (processName->Length()> 0)
					{
					LBSLOG2(ELogP9, "->A RLbsTransmitPosition::RunL(ReportReferenceLocation) %S\n", processName);
					LBSLOG(ELogP9, "  Return  = KErrAccessDenied\n");
					}
				delete processName;
				}
#endif
			iReferencePositionMessage.Complete(KErrAccessDenied);
			}
#ifdef ENABLE_LBS_DEV_LOGGER		
		TFileName* processName = new TFileName;
		if (processName != NULL)
			{
			LBSLOG_GETPROCESSNAME(aMessage, *processName);
			if (processName->Length()> 0)
				{
				LBSLOG2(ELogP9, "->A RLbsTransmitPosition::RunL(TransmitPosition) %S\n", processName);
				LBSLOG(ELogP9, "  Return  = KErrAccessDenied\n");
				}
			delete processName;
			}
#endif
		aMessage.Complete(KErrAccessDenied);
					
		return;
		}
	
	// Read in the request parameters
	TPtr8 ptr(reinterpret_cast<TUint8*>(&iTransmitParams),
			  sizeof(iTransmitParams),
			  sizeof(iTransmitParams));
	TInt err = MessageUtils::Read(aMessage, 0, ptr); // this panics client if there's an error
	if (err == KErrNone)
		{
		// Request a new X3P from the X3P handler.
		if(iServer->X3pObserver() != NULL)
			{
			err = iServer->X3pObserver()->OnTransmitPosition(iTransmitParams.iDestinationId,
											   iTransmitParams.iPriority,
											   iTransmitOptions,
											   iSessionId);
			
			}
		else
			{
			err = KErrNotFound;
			}
		if (err != KErrNone)
			{
			// Error starting the X3P request, so complete the
			// client request with the error.
#ifdef ENABLE_LBS_DEV_LOGGER
			TFileName * processName = new TFileName;
			if (processName != NULL)
				{
				LBSLOG_GETPROCESSNAME(aMessage, *processName);
				if (processName->Length()> 0)
					{
					LBSLOG2(ELogP9, "->A RLbsTransmitPosition::RunL(TransmitPosition) %S\n", processName);
					LBSLOG2(ELogP9, "  Return  = %d\n", err);
					}
				delete processName;
				}
#endif
			aMessage.Complete(err);
			}
		else
			{
			// Store the message to be completed asynchronously.
			iTransmitPositionMessage = aMessage;

			// Store the position info size - this is
			// used when writing the sent position back to the client.

			iClientPosInfoClassSize = iTransmitParams.iPositionInfoClassSize;
			
			// Need the secure id of the client for logging
			RThread client;
			aMessage.Client(client);
			iClientUid = client.SecureId();
			client.Close();
			}	
		}
	}

void CNrhX3pServerSubsession::CancelTransmitPosition(const RMessage2& aMessage)
	{
	if(iServer->X3pObserver() != NULL)
		{
		iServer->X3pObserver()->OnCancelTransmitPosition(iSessionId);
		}
	aMessage.Complete(KErrNone);
	}

void CNrhX3pServerSubsession::SetTransmitOptions(const RMessage2& aMessage)
	{
	TPckg<TLbsTransmitPositionOptions> optionsPckg(iTransmitOptions);
	TInt err = MessageUtils::Read(aMessage, 0, optionsPckg);
	aMessage.Complete(err);
	}

void CNrhX3pServerSubsession::GetTransmitOptions(const RMessage2& aMessage)
	{
	TPckg<TLbsTransmitPositionOptions> optionsPckg(iTransmitOptions);
	TInt err = MessageUtils::Write(aMessage, 0, optionsPckg);
	aMessage.Complete(err);
	}

void CNrhX3pServerSubsession::ReportReferenceLocation(const RMessage2& aMessage)
	{
	// Check that this sub-session doesn't already 
	// have an outstanding request
	if ((!iTransmitPositionMessage.IsNull()) || (!iReferencePositionMessage.IsNull()))
		{
#ifdef ENABLE_LBS_DEV_LOGGER
		TFileName* processName = new TFileName;
		if (processName != NULL)
			{
			LBSLOG_GETPROCESSNAME(aMessage, *processName);
			if (processName->Length() > 0)
				{
				LBSLOG2(ELogP9, "->A RLbsTransmitPosition::Panic(ReportReferenceLocation) %S\n", processName);
				LBSLOG2(ELogP9, "  Return  = %d\n", EPositionDuplicateRequest);
				}
			delete processName;
			}
#endif
		aMessage.Panic(KPosClientFault, EPositionDuplicateRequest);
		}
	
	// Check that Transmit Position is allowed by
	// the LbsAdmin setting
	if (!CheckTransmitPositionAdminSettings())
		{
		// Transmit Position not allowed; complete the client
		// request with an error
#ifdef ENABLE_LBS_DEV_LOGGER
		TFileName* processName = new TFileName;
		if (processName != NULL)
			{
			LBSLOG_GETPROCESSNAME(aMessage, *processName);
			if (processName->Length() > 0)
				{
				LBSLOG2(ELogP9, "->A RLbsTransmitPosition::RunL(ReportReferenceLocation) %S\n", processName);
				LBSLOG(ELogP9, "  Return  = KErrAccessDenied\n");
				}
			delete processName;
			}
#endif
		aMessage.Complete(KErrAccessDenied);
		
		// Make sure the Transmit Location request following this
		// failed ReportReferenceLocation won't be serviced either
		iRefLocErrorCode = KErrAccessDenied;
		return;
		}
	else
		{
		// Read in the request parameters
    	TTransmitPositionParams params;
	    TPtr8 ptr(reinterpret_cast<TUint8*>(&params),
			  sizeof(params),
			  sizeof(params));
		TInt err = MessageUtils::Read(aMessage, 0, ptr); // this panics client if there's an error
		if (err == KErrNone)
			{
			// Store the message to be completed asynchronously.
			iReferencePositionMessage = aMessage;
			
			// Store the position info type and size - these are
			// used when writing the sent position back to the client.
			iClientRefPosInfoClassSize = params.iPositionInfoClassSize;	
			}
		}
	}

void CNrhX3pServerSubsession::OnTransmitLocationComplete(const TLbsNetSessionIdInt& aSessionId,
														 const TPositionInfoBase& aPositionInfo,
														 TInt aReason)
	{
	// Check to see if the completed request is the 
	// one started by this subsession.
	if (aSessionId == iSessionId)
		{
		// There may be an outstanding request for 
		// reference location when the Transmit Location
		// is cancelled or times out.
		if (!iReferencePositionMessage.IsNull())
			{
			// There's no reference position at this point.
#ifdef ENABLE_LBS_DEV_LOGGER
			TFileName* processName = new TFileName;
			if (processName != NULL)
				{
				LBSLOG_GETPROCESSNAME(iReferencePositionMessage, *processName);
				if (processName->Length() > 0)
					{
					LBSLOG2(ELogP9, "->A RLbsTransmitPosition::RunL(ReportReferenceLocation) %S\n", processName);
					LBSLOG(ELogP9, "  < TPositionInfoBase aPositionInfo  = \n");
					LBSLOG_TPOSITIONINFOBASE(aPositionInfo);
					LBSLOG2(ELogP9, "  Return  = %d\n", aReason);
					}
				delete processName;
				}
#endif
			iReferencePositionMessage.Complete(aReason);
			}

		// Only write the position back if there was no error.
		TInt err(KErrNone);
		if (aReason == KErrNone || aReason == KPositionQualityLoss)
			{
			// Need to do this in two steps:
			// 1) copy aPositionInfo into a local position info type 
			// that is the same as the client(?!?)
			// 2) write the local position info into the client memory
			//
			// Edge cases:
			// 1) aPositionInfo is a smaller type than the client is expecting;
			//    do we copy just the data in aPositionInfo, and change the 
			//    'type' data of the client pos info, or copy over the data
			//    in aPositionInfo and leave the missing bits as junk?
			// 2) The client pos info is smaller than aPositionInfo - just
			//    copy over the data that will fit in?

			// Current strategy: copy over data equal to the size of
			// the client position info, minus the class type at the 
			// start. This will leave the client position info class
			// type as it was, but copy over the data it covers and no
			// more. if aPositionInfo is a bigger type, the extra data
			// is ignored; if it is smaller, then the 'missing' extra
			// data will be junk when written into the client.	
			const TUint8* start = reinterpret_cast<const TUint8*>(&aPositionInfo) + sizeof(TPositionClassTypeBase);
			TInt length = (iClientPosInfoClassSize - sizeof(TPositionClassTypeBase));
			TPtr8 ptr(const_cast<TUint8*>(start), length, length);
			err = iTransmitPositionMessage.Write(1, ptr, sizeof(TPositionClassTypeBase));
			if(err)
				{
#ifdef ENABLE_LBS_DEV_LOGGER
				TFileName* processName = new TFileName;
				if (processName != NULL)
					{
					LBSLOG_GETPROCESSNAME(iTransmitPositionMessage, *processName);
					if (processName->Length() > 0)
						{
						LBSLOG2(ELogP9, "->A RLbsTransmitPosition::Panic(TransmitPosition) %S\n", processName);
						LBSLOG2(ELogP9, "  Return  = %d\n", err);
						}
					delete processName;
					}
#endif
				_LIT(KServerMessageUtilsWrite8, "RMessageWrite8");
				iTransmitPositionMessage.Panic(KServerMessageUtilsWrite8, err);
				}			
			}
			
		if (err == KErrNone)
			{
#ifdef ENABLE_LBS_DEV_LOGGER
			TFileName* processName = new TFileName;
			if (processName != NULL)
				{
				LBSLOG_GETPROCESSNAME(iTransmitPositionMessage, *processName);
				if (processName->Length() > 0)
					{
					LBSLOG2(ELogP9, "->A RLbsTransmitPosition::RunL(TransmitPosition) %S\n", processName);
					LBSLOG(ELogP9, "  < TPositionInfoBase aPositionInfo  = \n");
					LBSLOG_TPOSITIONINFOBASE(aPositionInfo);
					LBSLOG2(ELogP9, "  Return  = %d\n", aReason);
					}
				delete processName;
				}
#endif
			iTransmitPositionMessage.Complete(aReason);
			}
			
		/** LBSLOGGER - Start Logging */
		// -------------------------------------------------------------------------
		CLbsTransmitLocationLogEvent* event = NULL;
		TPositionInfoBase& position = const_cast<TPositionInfoBase&>(aPositionInfo);
		TInt err2(KErrNone);
		if(position.PositionClassType() != EPositionGpsMeasurementInfoClass)
			{
			TRAP(err2, event = CLbsTransmitLocationLogEvent::NewL(iClientUid, &position, iTransmitParams.iDestinationId));	
			}
		else
			{
			TRAP(err2, event = CLbsTransmitLocationLogEvent::NewL(iClientUid, NULL, iTransmitParams.iDestinationId));
			}
		if (err2 == KErrNone)
			{
			event->SetX3PApp(iClientUid);
			switch (aReason)
				{
				case KErrNone:
					{
					event->SetRequestOutcome(ELbsRequestOutcomeSuccess);
					break;
					}
				case KErrCancel:
					{
					event->SetRequestOutcome(ELbsRequestOutcomeCancel);
					break;
					}
				default:
					{
					event->SetRequestOutcome(ELbsRequestOutcomeFail);
					break;
					}
				}
			
			iLogger.AddEvent(*event);
			
			delete event;
			}
		// -------------------------------------------------------------------------
		/** LBSLOGGER - End Logging */
		}
	}

void CNrhX3pServerSubsession::OnReferenceLocationAvailable(const TLbsNetSessionIdInt& aSessionId,
														 const TPositionInfoBase& aPositionInfo)
	{
	// Do nothing if the client doesn't want to know about Reference Location
	if (iReferencePositionMessage.IsNull())
		{
		return;
		}
		
	// Check if the completed request is the 
	// one started by this subsession.
	if (aSessionId == iSessionId)
		{
		
		TInt err(KErrNone);
		const TUint8* start = reinterpret_cast<const TUint8*>(&aPositionInfo) + sizeof(TPositionClassTypeBase);
		TInt length = (iClientRefPosInfoClassSize - sizeof(TPositionClassTypeBase));
		TPtr8 ptr(const_cast<TUint8*>(start), length, length);
		err = iReferencePositionMessage.Write(1, ptr, sizeof(TPositionClassTypeBase));

		if (err == KErrNone)
			{
#ifdef ENABLE_LBS_DEV_LOGGER
			TFileName* processName = new TFileName;
			if (processName != NULL)
				{
				LBSLOG_GETPROCESSNAME(iReferencePositionMessage, *processName);
				if (processName->Length() > 0)
					{
					LBSLOG2(ELogP9, "->A RLbsTransmitPosition::RunL(ReportReferenceLocation) %S\n", processName);
					LBSLOG(ELogP9, "  < TPositionInfoBase aPositionInfo  = \n");
					LBSLOG_TPOSITIONINFOBASE(aPositionInfo);
					LBSLOG(ELogP9, "  Return  = KErrNone\n");
					}
				delete processName;
				}
#endif
			iReferencePositionMessage.Complete(KErrNone);
			}
		else
			{
#ifdef ENABLE_LBS_DEV_LOGGER
			TFileName* processName = new TFileName;
			if (processName != NULL)
				{
				LBSLOG_GETPROCESSNAME(iReferencePositionMessage, *processName);
				if (processName->Length() > 0)
					{
					LBSLOG2(ELogP9, "->A RLbsTransmitPosition::Panic(ReportReferenceLocation) %S\n", processName);
					LBSLOG2(ELogP9, "  Return  = %d\n", err);
					}
				delete processName;
				}
#endif
			_LIT(KServerMessageUtilsWrite8, "RMessageWrite8");
			iReferencePositionMessage.Panic(KServerMessageUtilsWrite8, err);
			}
		}
	}

/* Check that an X3P is allowed, given the current network roaming status
*/
TBool CNrhX3pServerSubsession::CheckTransmitPositionAdminSettings()
	{
	TBool x3pAllowed(EFalse);
	CLbsAdmin::TTransmitLocateService serviceStatus(CLbsAdmin::ETransmitLocateOff);
	RLbsNetworkRegistrationStatus::TLbsNetworkRegistrationStatus netRegStatus(RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown);
	
	CNrhServer* nrhServer = const_cast<CNrhServer*>(iServer);
	TInt err = nrhServer->NetworkRegistrationStatus().GetNetworkRegistrationStatus(netRegStatus);
	if (err == KErrNone)
		{
		switch (netRegStatus)
			{
			case RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork:
				{
				err = iServer->Admin()->Get(KLbsSettingHomeTransmitLocate, serviceStatus);
				break;
				}
			case RLbsNetworkRegistrationStatus::ERegisteredRoamingNetwork:
			case RLbsNetworkRegistrationStatus::ENotRegistered:
				{
				err = iServer->Admin()->Get(KLbsSettingRoamingTransmitLocate, serviceStatus);
				break;
				}		
			case RLbsNetworkRegistrationStatus::ENetworkRegistrationUnknown:
			default:
				{
				LBSLOG_WARN2(ELogP4, 
							 "Unrecognised TLbsNetworkRegistrationStatus (%d), defaulting to not allowing new X3P", 
							 netRegStatus);
				serviceStatus = CLbsAdmin::ETransmitLocateOff;
				break;
				}
			}
		
		if (serviceStatus == CLbsAdmin::ETransmitLocateOn)
			{
			x3pAllowed = ETrue;
			}
		}
	
	return x3pAllowed;
	}
