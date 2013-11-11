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
//

#include "lbsdevloggermacros.h"
#include "lbsnetsimgatewayhandler.h"
#include "lbsnetsimtesthandler.h"
#include "lbsnetsimassistancedataprovider.h"
#include <lbspositioninfo.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/test/lbsnetsimstatus.h>
#include <lbs/test/lbsnetsimtest.h>
#include <e32property.h>

#include <s32mem.h>

TLbsAssistanceDataGroup CLbsNetSimGatewayHandler::iDefaultAssistanceDataMask = EAssistanceDataReferenceTime | EAssistanceDataReferenceLocation | EAssistanceDataNavigationModel;

/**
*/
CLbsNetSimGatewayHandler* CLbsNetSimGatewayHandler::NewL(CLbsNetSimAssistanceDataProvider& aAssistanceDataProvider)
	{
	CLbsNetSimGatewayHandler* self = new(ELeave) CLbsNetSimGatewayHandler(aAssistanceDataProvider);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);


	return self;
	}

/**
*/
CLbsNetSimGatewayHandler::~CLbsNetSimGatewayHandler()
	{
	iMessages.Reset();
	iMeasurementControlLocationAssistanceData.Close();
	}

/**
*/
void CLbsNetSimGatewayHandler::SetTestHandler(CLbsNetSimTestHandler* aTestHandler)
	{
	iTestHandler = aTestHandler;
	}

/**
*/
void CLbsNetSimGatewayHandler::ServiceL(const RMessage2& aMessage)
	{
	TInt function = aMessage.Function();

	if ((iError == KErrNone) ||
	    (function == EGWNotificationMeasurementControlLocation) ||
	    (function == EGWNotificationMeasurementControlLocationCancel) ||
	    (function == EGWNotificationRegisterLcsLocation) ||
	    (function == EGWNotificationRegisterLcsLocationCancel) ||
	    (function == EGWNotificationCancelPrivacy) ||
	    (function == EGWNotificationCancelPrivacyCancel) ||
	    (function == EGWNotificationNetworkGone) ||
	    (function == EGWNotificationNetworkGoneCancel) ||
	    (function == EGWNotificationResetAssistanceData) ||
		(function == EGWNotificationResetAssistanceDataCancel))
		{
		switch (aMessage.Function())
			{
			case EGWRegisterLcsMoLr:
				{
				HandleRegisterLcsMoLrL(aMessage);
				break;
				}
			case EGWMeasurementReportLocation:
				{
				HandleMeasurementReportLocationL(aMessage);
				break;
				}
			case EGWMeasurementReportLocationCancel:
				{
				if (!iMessageMeasurementReportLocation.IsNull())
					{
					iMessageMeasurementReportLocation.Complete(KErrCancel);
					}

				aMessage.Complete(KErrNone);
				break;
				}
			case EGWReleaseLcsMoLr:
				{
				HandleReleaseLcsMoLrL(aMessage);
				break;
				}
			case EGWMeasurementControlFailure:
				{
				HandleMeasurementControlFailureL(aMessage);
				break;
				}
			case EGWRequestMoreAssistanceData:
				{
				HandleRequestMoreAssistanceDataL(aMessage);
				break;
				}
			case EGWNotificationMeasurementControlLocation:
				{
				iNotificationMeasurementControlLocation = aMessage;
				break;
				}
			case EGWNotificationMeasurementControlLocationCancel:
				{
				if (!iNotificationMeasurementControlLocation.IsNull())
					{
					iNotificationMeasurementControlLocation.Complete(KErrCancel);
					}
				break;
				}
			case EGWNotificationRegisterLcsLocation:
				{
				iNotificationRegisterLcsLocation = aMessage;
				break;
				}
			case EGWNotificationRegisterLcsLocationCancel:
				{
				if (!iNotificationRegisterLcsLocation.IsNull())
					{
					iNotificationRegisterLcsLocation.Complete(KErrCancel);
					}
				break;
				}
			case EGWReleaseLcsLocationNotification:
				{
				HandleReleaseLcsLocationNotifcationL(aMessage);
				break;
				}
			case EGWNotificationCancelPrivacy:
				{
				iNotificationCancelPrivacyRequest = aMessage;
				break;
				}
			case EGWNotificationCancelPrivacyCancel:
				{
				if (!iNotificationCancelPrivacyRequest.IsNull())
					{
					iNotificationCancelPrivacyRequest.Complete(KErrCancel);
					}
				break;
				}
			case EGWNotificationNetworkGone:
				{
				iNotificationNetworkGone = aMessage;
				break;
				}
			case EGWNotificationNetworkGoneCancel:
				{
				if (!iNotificationNetworkGone.IsNull())
					{
					iNotificationNetworkGone.Complete(KErrCancel);
					}
				break;
				}
			case EGWNotificationResetAssistanceData:
				{
				iNotificationResetAssistanceData = aMessage;
				break;
				}
			case EGWNotificationResetAssistanceDataCancel:
				{
				if (!iNotificationResetAssistanceData.IsNull())
					{
					iNotificationResetAssistanceData.Complete(KErrCancel);
					}
				break;
				}
			default:
				{
				ASSERT(EFalse);
				User::Leave(KErrNotSupported);
				}
			} // switch
		} // if
	else
		{
		if (!iNotificationMeasurementControlLocation.IsNull() &&
			((function == EGWRegisterLcsMoLr) ||
			(function == EGWRequestMoreAssistanceData)))
			{
			iNotificationMeasurementControlLocation.Complete(iError);
			FinishL();
			}
		else
			{
			aMessage.Complete(iError);
			FinishL();
			}

			if (!iErrorSticky)
				{
				iError = KErrNone;
				} // if
		}
	}


/**
*/
void CLbsNetSimGatewayHandler::Connected()
	{
	LBSLOG(ELogP1, "CLbsNetSimGatewayHandler::Connected()");
	iTestHandler->GatewayConnected();
	}


/**
*/
void CLbsNetSimGatewayHandler::Disconnected()
	{
	LBSLOG(ELogP1, "CLbsNetSimGatewayHandler::Disconnected()");

	// As the server and handlers could possibly be used for a
	// second session reset some internals here
	ResetInternals();

	// Empty Messages
	CTimer::Cancel();
	while (iMessages.Count() > 0)
		{
		TRAP_IGNORE(RunL());
		}

	iTestHandler->GatewayDisconnected();

	// Complete any outstanding messages
	if (!iAssistanceDataToFill.IsNull())
		{
		iAssistanceDataToFill.Complete(KErrDisconnected);
		}

	if (!iMessageReleaseLcsMoLr.IsNull())
		{
		iMessageReleaseLcsMoLr.Complete(KErrDisconnected);
		}

	if (!iMessageMeasurementReportLocation.IsNull())
		{
		iMessageMeasurementReportLocation.Complete(KErrDisconnected);
		}

	if (!iMessageMeasurementControlFailure.IsNull())
		{
		iMessageMeasurementControlFailure.Complete(KErrDisconnected);
		}

	if (!iNotificationMeasurementControlLocation.IsNull())
		{
		iNotificationMeasurementControlLocation.Complete(KErrDisconnected);
		}

	if (!iNotificationRegisterLcsLocation.IsNull())
		{
		iNotificationRegisterLcsLocation.Complete(KErrDisconnected);
		}

	if (!iNotificationCancelPrivacyRequest.IsNull())
		{
		iNotificationCancelPrivacyRequest.Complete(KErrDisconnected);
		}

	if (!iNotificationNetworkGone.IsNull())
		{
		iNotificationNetworkGone.Complete(KErrDisconnected);
		}

	if (!iNotificationResetAssistanceData.IsNull())
		{
		iNotificationResetAssistanceData.Complete(KErrDisconnected);
		}
	}

/**
*/
void CLbsNetSimGatewayHandler::ProcessAssistanceDataL(TInt aError,
													  RLbsAssistanceDataBuilderSet& aAssistanceData,
								   					  TPositionInfo& aPosition,
								   					  TLbsNetPosRequestQuality& aQuality)
	{
	LBSLOG(ELogP1, "CLbsNetSimGatewayHandler::ProcessAssistanceDataL()");
	LBSLOG2(ELogP2, "AssistanceDataError = %d", aError);

	iAssistanceDataError = aError;

	// Write position to message
	TPckg<TPositionInfo> positionPkg(aPosition);
	iNotificationMeasurementControlLocation.WriteL(0, positionPkg, 0);

	// Write quality to message
	TPckg<TLbsNetPosRequestQuality> qualityPkg(aQuality);
	iNotificationMeasurementControlLocation.WriteL(1, qualityPkg, 0);

	// Write assistance data to the message
	HBufC8* buffer = HBufC8::NewLC(RLbsAssistanceDataBuilderSet::MaxExternalizedBufferSize());
	TPtr8 des(buffer->Des());
	RDesWriteStream stream(des);
	CleanupClosePushL(stream);
	aAssistanceData.ExternalizeL(stream);
	stream.CommitL();
	iNotificationMeasurementControlLocation.WriteL(2, des, 0);
	CleanupStack::PopAndDestroy(2, buffer);

	// Fire test notification
	iMeasurementControlLocationAssistanceData.MergeL(aAssistanceData);
	iMeasurementControlLocationPosition = aPosition;
	iMeasurementControlLocationQuality = aQuality;

	// Complete the message
	iGotAssistanceData = ETrue;
	if (!IsActive() &&
		(iMessages.Count() > 0) &&
	    ((iMessages.At(0) == EContinueRegisterLcsMoLr2) ||
		 (iMessages.At(0) == EContinueRequestMoreAssistanceData) ||
		 (iMessages.At(0) == EContinueMtLr)))
		{
		After(0);
		}
	}


/**
*/
void CLbsNetSimGatewayHandler::SetAssistanceDataMask(TLbsAssistanceDataGroup& aMask)
	{
	iAssistanceDataMask = aMask;
	}


/**
*/
void CLbsNetSimGatewayHandler::SetMoreAssistanceDataMask(TLbsAssistanceDataGroup& aMask)
	{
	iMoreAssistanceDataMask = aMask;
	}



/**
*/
void CLbsNetSimGatewayHandler::ClearAssistanceDataMasks()
	{
	iAssistanceDataMask = iDefaultAssistanceDataMask;
	}


/**
*/
void CLbsNetSimGatewayHandler::StartNetworkPrivacyRequestL(TLbsNetPosRequestPrivacy& aType, TLbsExternalRequestInfo& aRequestInfo)
	{
	// Get emergency state
	TBool emergency = EFalse;
	RProperty::Get(TUid::Uid(KLbsNetSimStatus), KLbsEmergencyStatus, emergency);

	if (iState == ENothing)
		{
		// Write args
		TPckg<TLbsExternalRequestInfo>	requestInfoPkg(aRequestInfo);
		iNotificationRegisterLcsLocation.WriteL(0, requestInfoPkg);
		TPckg<TLbsNetPosRequestPrivacy>	requestPrivacyPkg(aType);
		iNotificationRegisterLcsLocation.WriteL(1, requestPrivacyPkg);

		iNotificationRegisterLcsLocation.Complete(KErrNone);

		iState = (emergency) ? EPrivacy_E : EPrivacy;
		}
	else
		{
		if ((iMtLrOverrides) ||
		    (emergency && ((iState != EPrivacy_E) && (iState != EMtLr_E))))
			{
			// Cancel what we are doing and start the MtLr
			iPrivacyType = aType;
			iPrivacyRequest = aRequestInfo;
			Cancel((emergency) ? EPrivacy_E : EPrivacy);
			}
		else
			{
			User::Leave(KErrInUse);
			}
		}
	}


/**
*/
void CLbsNetSimGatewayHandler::CancelNetworkPrivacyRequestL()
	{
	if (iState != ENothing)
		{
	   	if (!iNotificationRegisterLcsLocation.IsNull())
			{
			iNotificationRegisterLcsLocation.Complete(KErrCancel);
			}

		FinishL();
		}
	}


/**
*/
void CLbsNetSimGatewayHandler::StartNetworkLocationRequestL()
	{
	// Get emergency state
	TBool emergency = EFalse;
	RProperty::Get(TUid::Uid(KLbsNetSimStatus), KLbsEmergencyStatus, emergency);

	if (iState == ENothing)
		{
		iGotAssistanceData = EFalse;
		iAssistanceDataProvider.RequestAssistanceDataL(iDefaultAssistanceDataMask);
		AddMessageL(EContinueMtLr);

		iState = (emergency) ? EMtLr_E : EMtLr;
		}
	else
		{
		if ((iMtLrOverrides) ||
		    (emergency && ((iState != EPrivacy_E) && (iState != EMtLr_E))))
			{
			// Cancel what we are doing and start the MtLr
			Cancel((emergency) ? EMtLr_E : EMtLr);
			}
		else
			{
			User::Leave(KErrInUse);
			}
		}
	}


/**
*/
void CLbsNetSimGatewayHandler::CancelNetworkLocationRequestL()
	{
	if (iState != ENothing)
		{
		if (!iNotificationMeasurementControlLocation.IsNull())
			{
			iNotificationMeasurementControlLocation.Complete(KErrCancel);
			}

		FinishL();
		}
	}

void CLbsNetSimGatewayHandler::SendResetAssistanceData(TLbsAssistanceDataGroup aMask)
	{
	TPckg<TLbsAssistanceDataGroup> maskPkg(aMask);
	TInt err = iNotificationResetAssistanceData.Write(0, maskPkg);
	iNotificationResetAssistanceData.Complete(err);
	}

/**
*/
void CLbsNetSimGatewayHandler::RunL()
	{
	if (!iWaitTimeSticky)
		{
		iWaitTime = 0;
		}

	switch (iMessages.At(0))
		{
		case EContinueRegisterLcsMoLr1:
			{
			// Check to see we have a valid destination
			if (CheckDestinationValidity(iRegisterLcsMoLrData->Des()))
				{
				// Notify test handler
				TInt err = iTestHandler->GatewayRegisterLcsMoLr(iRegisterLcsMoLrData->Des());
				if (err == KErrNone)
					{
					// So MoLr or X3P
					if (iRegisterLcsMoLrData->Length() == 0)
						{
						iState = EMoLr;
						}
					else
						{
						iState = EX3P;
						}
					iSubState = EPreMrl;
					iRequestCancelled = EFalse;

					// Regardless of type we do same thing here => kick of request for assitance data
					iGotAssistanceData = EFalse;
					iAssistanceDataProvider.RequestAssistanceDataL(iAssistanceDataMask);
					AddMessageL(EContinueRegisterLcsMoLr2);
					}
				else
					{
					iNotificationMeasurementControlLocation.Complete(err);
					FinishL();
					}
				}
			else
				{
				iNotificationMeasurementControlLocation.Complete(KErrArgument);
				FinishL();
				}

			delete iRegisterLcsMoLrData;
			iRegisterLcsMoLrData = NULL;

			iMessages.Delete(0);
			break;
			}
		case EContinueRegisterLcsMoLr2:
		case EContinueRequestMoreAssistanceData:
		case EContinueMtLr:
			{
			// Has the assitance data arrived?
			if (iGotAssistanceData)
				{
				if (!iNotificationMeasurementControlLocation.IsNull() && !iRequestCancelled && (iState != ENothing)) // Could have been cancelled in the delay
					{
					TInt err = iTestHandler->GatewayProcessAssistanceDataL(iMeasurementControlLocationAssistanceData,
																iMeasurementControlLocationPosition,
																iMeasurementControlLocationQuality);
					if (err == KErrNone)
						{
						iNotificationMeasurementControlLocation.Complete(iAssistanceDataError);
						}
					else
						{
						iNotificationMeasurementControlLocation.Complete(err);
						FinishL();
						}
					}

				iRequestCancelled = EFalse;

				iMessages.Delete(0);
				}
			break;
			}
		case EContinueMeasurementReportLocation:
			{
			// Complete the message
			if (!iMessageMeasurementReportLocation.IsNull())
				{
				TInt err = KErrNone;
				if ((iState == EMoLr) || (iState == EX3P))// Only inform observers if MoLr or X3P
					{
					err = iTestHandler->GatewayFacilityLcsMoLrResultL(iMeasurementReportLocationStatus, iMeasurementReportLocationPosition);
					}
				iMessageMeasurementReportLocation.Complete(err); // We will complete but fireNotification should be false
				}

			// Reset state?
			if ((iState == EMtLr) || (iState == EMtLr_E))
				{
				FinishL();
				// Dont delete top message FinishL clears up for us.
				}
			else
				{
				iMessages.Delete(0);
				}

			iRequestCancelled = EFalse;
			break;
			}
		case EContinueReleaseLcsMoLr:
			{
			iTestHandler->GatewayReleaseLcsMoLrL(iReleaseLcsMoLrReason);
			FinishL();

			// Do not delete the top messages as FinishL will have cleared up the messages.
			break;
			}
		case EContinueMeasurementControlFailure:
			{
			iTestHandler->GatewayMeasurementControlFailureL(iMeasurementControlFailureReason);

			if ((iState == EMtLr) ||
				(iState == EMtLr_E))
				{
				FinishL();
				}
			else
				{
				iMessages.Delete(0);
				}
			break;
			}
		default:
			{
			ASSERT(EFalse);
			_LIT(KInternalError, "Internal Error");
			User::Panic(KInternalError, KErrAbort);
			}
		}

	if (iMessages.Count() > 0)
		{
		if (!IsActive())
			{
			if ((iMessages[0] == EContinueRegisterLcsMoLr2) ||
				(iMessages[0] == EContinueRequestMoreAssistanceData) ||
				(iMessages[0] == EContinueMtLr))
				{
				if (iGotAssistanceData)
					{
					After(0);
					}
				}
			else
				{
				After(iWaitTime);
				}
			}
		}
	}




//
// Private


/**
*/
CLbsNetSimGatewayHandler::CLbsNetSimGatewayHandler(CLbsNetSimAssistanceDataProvider& aAssistanceDataProvider) :
	CTimer(EPriorityMuchLess), iMessages(2), iState(ENothing), iAssistanceDataProvider(aAssistanceDataProvider),
	iWaitTime(0), iWaitTimeSticky(EFalse), iError(KErrNone), iErrorSticky(EFalse),
	iMtLrOverrides(EFalse), iRequestCancelled(EFalse), iStartOnComplete(ENothing)
	{
	CActiveScheduler::Add(this);

	iAssistanceDataMask = iDefaultAssistanceDataMask;
	iAssistanceDataProvider.SetObserver(this);
	}


/**
*/
void CLbsNetSimGatewayHandler::ConstructL()
	{
	CTimer::ConstructL();
	iMeasurementControlLocationAssistanceData.OpenL();
	}


/**
*/
void CLbsNetSimGatewayHandler::HandleRegisterLcsMoLrL(const RMessage2& aMessage)
	{
	ASSERT(iRegisterLcsMoLrData == NULL);

	// Read args
	TInt size = aMessage.GetDesLengthL(0);
	iRegisterLcsMoLrData = HBufC::NewL(size);
	TPtr ptr(iRegisterLcsMoLrData->Des());
	aMessage.ReadL(0, ptr, 0);

	AddMessageL(EContinueRegisterLcsMoLr1);
	}


/**
*/
void CLbsNetSimGatewayHandler::HandleMeasurementReportLocationL(const RMessage2& aMessage)
	{
	if (iState != ENothing)
		{
		TInt status = KErrNone;
		iMessageMeasurementReportLocation = aMessage;

		// Read args
		TPositionInfo position;
		TPckg<TPositionInfo> positionPkg(position);
		aMessage.ReadL(1, positionPkg, 0);

		// Notifications
		TInt err = iTestHandler->GatewayMeasurementReportLocationL(position);
		if (err == KErrNone)
			{
			TBool fireNotification = EFalse;
			// Write args
			if ((iState == EMoLr)||(iState == EX3P))
			{
				fireNotification = ETrue;
			}

			TPckg<TInt> statusPkg(status);
			TPckg<TBool> fireNotificationPkg(fireNotification);
			aMessage.WriteL(0, statusPkg, 0);
			aMessage.WriteL(2, fireNotificationPkg, 0);

			// Fire the notification
			iMeasurementReportLocationStatus = status;
			iMeasurementReportLocationPosition = position;
			//iTestHandler->GatewayFacilityLcsMoLrResultL(status, position);

			iSubState = EPostMrl;
			AddMessageL(EContinueMeasurementReportLocation);
			}
		else
			{
			iMessageMeasurementReportLocation.Complete(err);
			FinishL();
			}
		}
	else
		{
		aMessage.Complete(KErrNotReady);
		FinishL();
		}
	}


/**
*/
void CLbsNetSimGatewayHandler::HandleReleaseLcsMoLrL(const RMessage2& aMessage)
	{
	// A release may come in even though we have not managed to transit into a
	// state. So dont check state here.
	iMessageReleaseLcsMoLr = aMessage;

	// Read args
	TPckg<TInt> reasonPkg(iReleaseLcsMoLrReason);
	aMessage.ReadL(0, reasonPkg, 0);

	iRequestCancelled = ETrue;
	AddMessageL(EContinueReleaseLcsMoLr);
	}


/**
*/
void CLbsNetSimGatewayHandler::HandleMeasurementControlFailureL(const RMessage2& aMessage)
	{
	if (iState != ENothing)
		{
		iMeasurementControlFailureReason = aMessage.Int0();
		AddMessageL(EContinueMeasurementControlFailure);
		}
	}


/**
*/
void CLbsNetSimGatewayHandler::HandleRequestMoreAssistanceDataL(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1, "CLbsNetSimGatewayHandler::HandleRequestMoreAssistanceDataL()");

	if (iState != ENothing)
		{
		// Read filter
		TLbsAssistanceDataGroup filter;
		TPckg<TLbsAssistanceDataGroup> filterPkg(filter);
		aMessage.ReadL(0, filterPkg, 0);

		// Fire notification
		TInt err = iTestHandler->GatewayRequestMoreAssistanceDataL(filter);
		if (err == KErrNone)
			{
			// Request assistance data
			iGotAssistanceData = EFalse;
			iAssistanceDataProvider.RequestAssistanceDataL(filter);
			AddMessageL(EContinueRequestMoreAssistanceData);
			}
		else
			{
			iNotificationMeasurementControlLocation.Complete(err);
			FinishL();
			}
		}
	}


/**
*/
void CLbsNetSimGatewayHandler::HandleReleaseLcsLocationNotifcationL(const RMessage2& aMessage)
	{
	// Read the response
	CLbsNetworkProtocolBase::TLbsPrivacyResponse response;
	TPckg<CLbsNetworkProtocolBase::TLbsPrivacyResponse> responsePkg(response);
	aMessage.ReadL(0, responsePkg, 0);

	iTestHandler->GatewayReleaseLcsLocationNotificationL(response);

	FinishL();
	}


/**
*/
void CLbsNetSimGatewayHandler::SetResponseTime(TInt32 aTime, TBool aSticky)
	{
	iWaitTime = aTime * 1000; // We need millisecond
	iWaitTimeSticky = aSticky;
	}


/**
*/
void CLbsNetSimGatewayHandler::SetResponseError(TInt32 aError, TBool aSticky)
	{
	iError = aError;
	iErrorSticky = aSticky;

	LBSLOG3(ELogP2, "SetResponseError %d (sticky = %d)", aError, aSticky);

	if (iError == RLbsNetSimTest::KNetSimNetworkNotAvailable)
		{
		if (!iNotificationNetworkGone.IsNull())
			{
			iNotificationNetworkGone.Complete(iError);
			if (iState != ENothing)
				{
				FinishL();
				}
			}
		}
	}


/**
*/
void CLbsNetSimGatewayHandler::AddMessageL(NextMessage aMessage)
	{
	LBSLOG2(ELogP2, "Appending message to NetSim queue: %d", aMessage);
	iMessages.AppendL(aMessage);

	// Is the timer already running?
	if (!IsActive())
		{
		if ((iMessages[0] == EContinueRegisterLcsMoLr2) ||
			(iMessages[0] == EContinueRequestMoreAssistanceData) ||
			(iMessages[0] == EContinueMtLr))
			{
			if (iGotAssistanceData)
				{
				After(0);
				}
			}
		else
			{
			After(iWaitTime);
			}
		}
	}


/**
*/
void CLbsNetSimGatewayHandler::Cancel(MessageState aToStart)
	{
	// Store what we need to start
	iStartOnComplete = aToStart;

	// Cancel what we are doing
	switch (iState)
		{
		case EMoLr:
		case EX3P:
			{
			if (iSubState == EPreMrl)
				{
				iNotificationMeasurementControlLocation.Complete(KErrCancel);
				iRequestCancelled = ETrue;
				}
			else
				{
				if (iState == EMoLr) // Cant cancel an X3P here
					{
					iMessageMeasurementReportLocation.Complete(KErrCancel);
					iRequestCancelled = ETrue;
					}
				}
			break;
			}
		case EPrivacy:
			{
			TPckg<TInt> reasonPkg(KErrCancel);
			iNotificationCancelPrivacyRequest.Write(0, reasonPkg, 0);
			iNotificationCancelPrivacyRequest.Complete(KErrNone);
			break;
			}
		case EMtLr:
			{
			iNotificationMeasurementControlLocation.Complete(KErrCancel);
			break;
			}
		default:
			{
			ASSERT(EFalse);
			}
		}
	}

void CLbsNetSimGatewayHandler::FinishL()
	{
	iState = ENothing;

	switch (iStartOnComplete)
		{
		case ENothing:
			{
			break;
			}
		case EPrivacy:
		case EPrivacy_E:
			{
			StartNetworkPrivacyRequestL(iPrivacyType, iPrivacyRequest);
			break;
			}
		case EMtLr:
		case EMtLr_E:
			{
			StartNetworkLocationRequestL();
			break;
			}
		default:
			{
			ASSERT(EFalse);
			}
		}

	iStartOnComplete = ENothing;
	ResetInternals();
	}

TBool CLbsNetSimGatewayHandler::CheckDestinationValidity(TPtrC aDestination)
	{
	TInt size = aDestination.Length();
	TBool valid = ETrue;
	for (TInt x = 0; valid && (x < size); x++)
		{
		TChar chr(aDestination[x]);
		valid = chr.IsDigit();
		}

	return valid;
	}

void CLbsNetSimGatewayHandler::ResetInternals()
	{
	iWaitTime = 0;
	iWaitTimeSticky = EFalse;
	iError = KErrNone;
	iErrorSticky = EFalse;
	iRequestCancelled = EFalse;
	// Cancel the timer
	CTimer::Cancel();
	// Reset the messages. Only delete messages up to a LBSRegisterLcsMoLr.
	while (iMessages.Count() > 0)
		{
		if (iMessages[0] != EContinueRegisterLcsMoLr1)
			{
			iMessages.Delete(0);
			}
		else
			{
			break;
			}
		}
	}
