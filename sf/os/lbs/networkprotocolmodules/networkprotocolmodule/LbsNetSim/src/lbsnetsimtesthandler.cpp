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

#include "lbsnetsimtesthandler.h"
#include "lbsnetsimassistancedataprovider.h"
#include "lbsnetsimgatewayhandler.h"
#include <lbspositioninfo.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/test/lbsnetsimstatus.h>

#include <s32mem.h>
#include <e32property.h>

/**
*/
CLbsNetSimTestHandler* CLbsNetSimTestHandler::NewL(CLbsNetSimAssistanceDataProvider& aAssistanceDataProvider)
	{
	CLbsNetSimTestHandler* self = new(ELeave) CLbsNetSimTestHandler(aAssistanceDataProvider);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
	}


/**
*/
void CLbsNetSimTestHandler::SetGatewayHandler(CLbsNetSimGatewayHandler* aGatewayHandler)
	{
	iGatewayHandler = aGatewayHandler;
	}

		
/**
*/
void CLbsNetSimTestHandler::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
		case ECallbacks:
			{
			HandleCallbacksL(aMessage);
			break;
			}
		case ETSetReferenceLocation:
			{
			HandleSetReferenceLocationL(aMessage);
			break;
			}
		case ETSetReferenceLocationData:
			{
			HandleSetReferenceLocationDataL(aMessage);
			break;
			}
		case ETSetResponseTime:
			{
			HandleSetResponseTime(aMessage);
			break;
			}
		case ETSetResponseError:
			{
			HandleSetResponseError(aMessage);
			break;
			}
		case ETSetDefaultAssitanceDataFilter:
			{
			HandleSetDefaultAssitanceDataFilter(aMessage);
			break;
			}
		case ETSetMoreDefaultAssitanceDataFilter:
			{
			HandleSetMoreDefaultAssitanceDataFilter(aMessage);
			break;
			}
		case ETClearAssitanceDataFilters:
			{
			HandleClearAssitanceDataFilters(aMessage);
			break;
			}
		case ETSetAssistanceDataProvider:
			{
			HandleSetAssistanceDataProvider(aMessage);
			break;
			}
		case ETSetEmergenyStatus:
			{
			HandleSetEmergenyStatus(aMessage);
			break;
			}
		case ETSetRoamingStatus:		
			{
			HandleSetRoamingStatus(aMessage);
			break;
			}
		case ETSetRequestQuality:		
			{
			HandleSetQuality(aMessage);
			break;
			}
		case ETSetStepMode:
			{
			HandleSetStepMode(aMessage);
			break;
			}
		case ETReleaseLcsLocationNotification:
			{
			HandleReleaseLcsLocationNotificationL(aMessage);
			break;
			}
		case ETReleaseLcsLocationNotificationCancel:
			{
			if (!iMessageReleaseLcsLocationNotification.IsNull())
				{
				iMessageReleaseLcsLocationNotification.Complete(KErrCancel);
				}
			break;
			}
		case ETStartNetworkLocationRequest:
			{
			HandleStartNetworkLocationRequestL(aMessage);
			break;
			}
		case ETStartNetworkLocationRequestCancel:
			{
			if (!iMessageStartNetworkLocationRequest.IsNull())
				{
				iMessageStartNetworkLocationRequest.Complete(KErrCancel);
				}
			break;
			}
		case ETSendResetAssistanceData:
			{
			HandleSendResetAssistanceData(aMessage);
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	}
	
	
/**
*/
void CLbsNetSimTestHandler::Connected()
	{
	iFireConnectedNotification = ETrue;
	
	// Attach to step mode property
	iStepModeProperty.Attach(TUid::Uid(KLbsNetSimClient), KLbsStepModeKey);
	}


/**
*/
void CLbsNetSimTestHandler::Disconnected()
	{
	// Clean up some values
	RProperty::Set(TUid::Uid(KLbsNetSimStatus), KLbsRoamingStatus, EFalse);
	RProperty::Set(TUid::Uid(KLbsNetSimStatus), KLbsEmergencyStatus, EFalse);
	
	// Detach from step mode property
	iStepModeProperty.Close();
	
	// Complete any outstanding RMessage2's
	if (!iMessageConnect.IsNull())
		{
		iMessageConnect.Complete(KErrDisconnected);
		}

	if (!iMessageDisconnect.IsNull())
		{
		iMessageDisconnect.Complete(KErrDisconnected);
		}

	if (!iMessageRegisterLcsMoLr.IsNull())
		{
		iMessageRegisterLcsMoLr.Complete(KErrDisconnected);
		}

	if (!iMessageMeasurementControlLocation.IsNull())
		{
		iMessageMeasurementControlLocation.Complete(KErrDisconnected);
		}

	if (!iMessageReleaseLcsMoLr.IsNull())
		{
		iMessageReleaseLcsMoLr.Complete(KErrDisconnected);
		}

	if (!iMessageReleaseLcsLocationNotification.IsNull())
		{
		iMessageReleaseLcsLocationNotification.Complete(KErrDisconnected);
		}

	if (!iMessageMeasurementReportLocation.IsNull())
		{
		iMessageMeasurementReportLocation.Complete(KErrDisconnected);
		}

	if (!iMessageMeasurementReportRequestMoreAssitanceData.IsNull())
		{
		iMessageMeasurementReportRequestMoreAssitanceData.Complete(KErrDisconnected);
		}

	if (!iMessageMeasurementReportControlFailure.IsNull())
		{
		iMessageMeasurementReportControlFailure.Complete(KErrDisconnected);
		}

	if (!iMessageFacilityLcsMoLrResult.IsNull())
		{
		iMessageFacilityLcsMoLrResult.Complete(KErrDisconnected);
		}

	if (!iMessageStartNetworkLocationRequest.IsNull())
		{
		iMessageStartNetworkLocationRequest.Complete(KErrDisconnected);
		}
	}
	
	
/**
*/
TInt CLbsNetSimTestHandler::GatewayConnected()
	{
	if (!iMessageConnect.IsNull())
		{
		iMessageConnect.Complete(KErrNone);
		iFireConnectedNotification = EFalse;
		}
	else
		{
		iFireConnectedNotification = ETrue;
		}
		
	iGatewayConnected = ETrue;
	
	return KErrNone;
	}


/**
*/
TInt CLbsNetSimTestHandler::GatewayDisconnected()
	{
	if (!iMessageDisconnect.IsNull())
		{
		iMessageDisconnect.Complete(KErrNone);
		}

	iGatewayConnected = EFalse;
	
	return KErrNone;
	}
	
	
/**
*/
TInt CLbsNetSimTestHandler::GatewayRegisterLcsMoLr(const TDesC& aData)
	{
	TInt ret = KErrNone;
	
	if (!iMessageRegisterLcsMoLr.IsNull())
		{
		iMessageRegisterLcsMoLr.Write(1, aData, 0);
		iMessageRegisterLcsMoLr.Complete(KErrNone);

		ret = StepModeWait();
		}
		
	return ret;
	}
	
TInt CLbsNetSimTestHandler::GatewayProcessAssistanceDataL(RLbsAssistanceDataBuilderSet& aAssistanceData, 
														  TPositionInfo& aPosition,  
														  TLbsNetPosRequestQuality& aQuality)
	{
	TInt ret = KErrNone;
	
	if (!iMessageMeasurementControlLocation.IsNull())
		{
		// Write position to message
		TPckg<TPositionInfo> positionPkg(aPosition);
		iMessageMeasurementControlLocation.WriteL(1, positionPkg, 0);
		
		// Write quality to message
		TPckg<TLbsNetPosRequestQuality> qualityPkg(aQuality);
		iMessageMeasurementControlLocation.WriteL(2, qualityPkg, 0);
			
		// Write assistance data to the message
		HBufC8* buffer = HBufC8::NewLC(RLbsAssistanceDataBuilderSet::MaxExternalizedBufferSize());
		TPtr8 des(buffer->Des());
		RDesWriteStream stream(des);
		CleanupClosePushL(stream);
		aAssistanceData.ExternalizeL(stream);
		stream.CommitL();
		iMessageMeasurementControlLocation.WriteL(3, des, 0);
		CleanupStack::PopAndDestroy(2, buffer);
		
		iMessageMeasurementControlLocation.Complete(KErrNone);

		ret = StepModeWait();
		} // if
		
	return ret;
	}
	
/**
*/	
TInt CLbsNetSimTestHandler::GatewayMeasurementReportLocationL(const TPositionInfo& aPosition)
	{
	TInt ret = KErrNone;
	
	if (!iMessageMeasurementReportLocation.IsNull())
		{
		// Write args
		TPckg<TPositionInfo> positionPkg(aPosition);
		iMessageMeasurementReportLocation.WriteL(1, positionPkg, 0);
		
		iMessageMeasurementReportLocation.Complete(KErrNone);
		
		ret = StepModeWait();
		} // if
		
	return ret;
	}


/**
*/
TInt CLbsNetSimTestHandler::GatewayFacilityLcsMoLrResultL(const TInt aReason, const TPositionInfo& aPosition)
	{
	TInt ret = KErrNone;
	
	if (!iMessageFacilityLcsMoLrResult.IsNull())
		{
		// Write args
		TPckg<TInt> reasonPkg(aReason);
		TPckg<TPositionInfo> positionPkg(aPosition);
		iMessageFacilityLcsMoLrResult.WriteL(1, reasonPkg, 0);
		iMessageFacilityLcsMoLrResult.WriteL(2, positionPkg, 0);
		
		iMessageFacilityLcsMoLrResult.Complete(KErrNone);
		
		ret = StepModeWait();
		}
		
	return ret;
	}
	
	
TInt CLbsNetSimTestHandler::GatewayReleaseLcsMoLrL(const TInt aReason)
	{
	TInt ret = KErrNone;
	
	if (!iMessageReleaseLcsMoLr.IsNull())
		{
		// Write args
		TPckg<TInt> reasonPkg(aReason);
		iMessageReleaseLcsMoLr.WriteL(1, reasonPkg, 0);
		
		iMessageReleaseLcsMoLr.Complete(KErrNone);
		
		ret = StepModeWait();
		}
		
	return ret;
	}
	
	
/**
*/
TInt CLbsNetSimTestHandler::GatewayMeasurementControlFailureL(const TInt aReason)
	{
	TInt ret = KErrNone;
	
	if (!iMessageMeasurementReportControlFailure.IsNull())
		{
		// Write args
		TPckg<TInt> reasonPkg(aReason);
		iMessageMeasurementReportControlFailure.WriteL(1, reasonPkg, 0);
		
		iMessageMeasurementReportControlFailure.Complete(KErrNone);
		
		ret = StepModeWait();
		}
		
	return ret;
	}


/**
*/
TInt CLbsNetSimTestHandler::GatewayRequestMoreAssistanceDataL(const TLbsAssistanceDataGroup& aFilter)
	{
	TInt ret = KErrNone;
	
	if (!iMessageMeasurementReportRequestMoreAssitanceData.IsNull())
		{
		// Write args
		TPckg<TLbsAssistanceDataGroup> filterPkg(aFilter);
		iMessageMeasurementReportRequestMoreAssitanceData.WriteL(1, filterPkg, 0);
		
		iMessageMeasurementReportRequestMoreAssitanceData.Complete(KErrNone);
		
		ret = StepModeWait();
		}
		
	return ret;
	}
	
	

/**
*/
TInt CLbsNetSimTestHandler::GatewayReleaseLcsLocationNotificationL(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse)
	{
	TInt ret = KErrNone;
	
	if (!iMessageReleaseLcsLocationNotification.IsNull())
		{
		TPckg<CLbsNetworkProtocolBase::TLbsPrivacyResponse> responsePkg(aResponse);
		iMessageReleaseLcsLocationNotification.WriteL(2, responsePkg, 0);
		
		iMessageReleaseLcsLocationNotification.Complete(KErrNone);
		
		ret = StepModeWait();
		}

	return ret;	
	}
		

//
// Private


/**
*/
CLbsNetSimTestHandler::CLbsNetSimTestHandler(CLbsNetSimAssistanceDataProvider& aAssitanceDataProvider) :
	iAssistanceDataProvider(aAssitanceDataProvider), iGatewayConnected(EFalse), 
	iFireConnectedNotification(EFalse), iStepMode(EFalse)
	{
	}
	
	
/**
*/
void CLbsNetSimTestHandler::ConstructL()
	{
	// Create publish & subscribe flags
	TUid statusUid = TUid::Uid(KLbsNetSimStatus);
	TInt err = RProperty::Define(KLbsEmergencyStatus, RProperty::EInt, ECapability_None, ECapabilityWriteDeviceData, EFalse);
	if ((err != KErrNone) && (err != KErrAlreadyExists))
		{
		User::Leave(err);
		}
	User::LeaveIfError(RProperty::Set(statusUid, KLbsEmergencyStatus, EFalse));

	err = RProperty::Define(statusUid, KLbsRoamingStatus, RProperty::EInt, ECapability_None, ECapabilityWriteDeviceData);
	if ((err != KErrNone) && (err != KErrAlreadyExists))
		{
		User::Leave(err);
		}
	User::LeaveIfError(RProperty::Set(statusUid, KLbsRoamingStatus, EFalse));
	}
	
	
/**
*/
void CLbsNetSimTestHandler::HandleCallbacksL(const RMessage2& aMessage)
	{
	switch (aMessage.Int0())
		{
		case ENotificationConnect:
			{
			iMessageConnect = aMessage;
			if (iGatewayConnected && iFireConnectedNotification)
				{
				iMessageConnect.Complete(KErrNone);
				iFireConnectedNotification = EFalse;
				}
			break;
			}
		case ENotificationConnectCancel:
			{
			if (!iMessageConnect.IsNull())
				{
				iMessageConnect.Complete(KErrCancel);
				}
			aMessage.Complete(KErrNone);
			break;
			}
		case ENotificationDisconnect:
			{
			iMessageDisconnect = aMessage;
			break;
			}
		case ENotificationDisconnectCancel:
			{
			if (!iMessageDisconnect.IsNull())
				{
				iMessageDisconnect.Complete(KErrCancel);
				}
			aMessage.Complete(KErrNone);
			break;
			}
		case ENotificationRegisterLcsMoLr:
			{
			iMessageRegisterLcsMoLr = aMessage;
			break;
			}
		case ENotificationRegisterLcsMoLrCancel:
			{
			if (!iMessageRegisterLcsMoLr.IsNull())
				{
				iMessageRegisterLcsMoLr.Complete(KErrCancel);
				}
			aMessage.Complete(KErrNone);
			break;
			}
		case ENotificationMeasurementControlLocation:
			{
			iMessageMeasurementControlLocation = aMessage;
			break;
			}
		case ENotificationMeasurementControlLocationCancel:
			{
			if (!iMessageMeasurementControlLocation.IsNull())
				{
				iMessageMeasurementControlLocation.Complete(KErrCancel);
				}
			aMessage.Complete(KErrNone);
			break;
			}
		case ENotificationReleaseLcsMoLr:
			{
			iMessageReleaseLcsMoLr = aMessage;
			break;
			}
		case ENotificationReleaseLcsMoLrCancel:
			{
			if (!iMessageReleaseLcsMoLr.IsNull())
				{
				iMessageReleaseLcsMoLr.Complete(KErrCancel);
				}
			aMessage.Complete(KErrNone);
			break;
			}
		case ENotificationMeasurementReport:
			{
			iMessageMeasurementReportLocation = aMessage;
			break;
			}
		case ENotificationMeasurementReportCancel:
			{
			if (!iMessageMeasurementReportLocation.IsNull())
				{
				iMessageMeasurementReportLocation.Complete(KErrCancel);
				}
			aMessage.Complete(KErrNone);
			break;
			}
		case ENotificationMeasurementReportRequestMoreAssitanceData:
			{
			iMessageMeasurementReportRequestMoreAssitanceData = aMessage;
			break;
			}
		case ENotificationMeasurementReportRequestMoreAssitanceDataCancel:
			{
			if (!iMessageMeasurementReportRequestMoreAssitanceData.IsNull())
				{
				iMessageMeasurementReportRequestMoreAssitanceData.Complete(KErrCancel);
				}
			aMessage.Complete(KErrNone);
			break;
			}
		case ENotificationMeasurementReportControlFailure:
			{
			iMessageMeasurementReportControlFailure = aMessage;
			break;
			}
		case ENotificationMeasurementReportControlFailureCancel:	
			{
			if (!iMessageMeasurementReportControlFailure.IsNull())
				{
				iMessageMeasurementReportControlFailure.Complete(KErrCancel);
				}
			aMessage.Complete(KErrNone);
			break;
			}
		case ENotificationFacilityLcsMoLrResult:
			{
			iMessageFacilityLcsMoLrResult = aMessage;
			break;
			}
		case ENotificationFacilityLcsMoLrResultCancel:
			{
			if (!iMessageFacilityLcsMoLrResult.IsNull())
				{
				iMessageFacilityLcsMoLrResult.Complete(KErrCancel);
				}
			aMessage.Complete(KErrNone);
			break;
			}
			
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	}
	
/**
*/
void CLbsNetSimTestHandler::HandleSetReferenceLocationL(const RMessage2& aMessage)
	{
	TPosition position;
	TPckg<TPosition> positionPkg(position);
	aMessage.ReadL(0, positionPkg, 0);
	
	iAssistanceDataProvider.SetReferenceLocation(position);
	
	aMessage.Complete(KErrNone);
	}


/**
*/
void CLbsNetSimTestHandler::HandleSetReferenceLocationDataL(const RMessage2& aMessage)
	{
	HBufC8* buffer = HBufC8::NewLC(aMessage.GetDesLength(0));
	TPtr8 ptr(buffer->Des());
	aMessage.ReadL(0, ptr, 0);
	
	iAssistanceDataProvider.SetReferenceLocationL(ptr);
	
	CleanupStack::Pop(buffer);

	aMessage.Complete(KErrNone);
	}


/**
*/
void CLbsNetSimTestHandler::HandleSetResponseTime(const RMessage2& aMessage)
	{
	iGatewayHandler->SetResponseTime(aMessage.Int0(), aMessage.Int1());
	
	aMessage.Complete(KErrNone);
	}
	
void CLbsNetSimTestHandler::HandleSetResponseError(const RMessage2& aMessage)
	{
	iGatewayHandler->SetResponseError(aMessage.Int0(), aMessage.Int1());
	
	aMessage.Complete(KErrNone);
	}

/**
*/
void CLbsNetSimTestHandler::HandleSetDefaultAssitanceDataFilter(const RMessage2& aMessage)
	{
	TLbsAssistanceDataGroup filter;
	TPckg<TLbsAssistanceDataGroup> filterPkg(filter);
	aMessage.Read(0, filterPkg, 0);
	
	iGatewayHandler->SetAssistanceDataMask(filter);
	
	aMessage.Complete(KErrNone);
	}


/**
*/
void CLbsNetSimTestHandler::HandleSetMoreDefaultAssitanceDataFilter(const RMessage2& aMessage)
	{
	TLbsAssistanceDataGroup filter;
	TPckg<TLbsAssistanceDataGroup> filterPkg(filter);
	aMessage.Read(0, filterPkg, 0);
	
	iGatewayHandler->SetMoreAssistanceDataMask(filter);

	aMessage.Complete(KErrNone);
	}


/**
*/
void CLbsNetSimTestHandler::HandleClearAssitanceDataFilters(const RMessage2& aMessage)
	{
	iGatewayHandler->ClearAssistanceDataMasks();
	
	aMessage.Complete(KErrNone);
	}


/**
*/
void CLbsNetSimTestHandler::HandleSetAssistanceDataProvider(const RMessage2& aMessage)
	{
	TUid uid;
	uid.iUid = aMessage.Int0();
	
	iAssistanceDataProvider.SetAssistanceDataProvider(uid);
	
	aMessage.Complete(KErrNone);
	}


/**
*/
void CLbsNetSimTestHandler::HandleSetEmergenyStatus(const RMessage2& aMessage)
	{
	aMessage.Complete(RProperty::Set(TUid::Uid(KLbsNetSimStatus), KLbsEmergencyStatus, aMessage.Int0()));
	}


/**
*/
void CLbsNetSimTestHandler::HandleSetRoamingStatus(const RMessage2& aMessage)
	{
	aMessage.Complete(RProperty::Set(TUid::Uid(KLbsNetSimStatus), KLbsRoamingStatus, aMessage.Int0()));
	}

	
/**
*/
void CLbsNetSimTestHandler::HandleSetQuality(const RMessage2& aMessage)
	{
	TLbsNetPosRequestQuality quality;
	TPckg<TLbsNetPosRequestQuality> qualityPkg(quality);
	aMessage.Read(0, qualityPkg, 0);
	
	iAssistanceDataProvider.SetQuality(quality);
	
	aMessage.Complete(KErrNone);
	}

/**
*/
void CLbsNetSimTestHandler::HandleSetStepMode(const RMessage2& aMessage)
	{
	iStepMode = aMessage.Int0();
	
	aMessage.Complete(KErrNone);
	}
	

/**
*/
void CLbsNetSimTestHandler::HandleReleaseLcsLocationNotificationL(const RMessage2& aMessage)
	{
	iMessageReleaseLcsLocationNotification = aMessage;
	
	// Read arguments
	TLbsNetPosRequestPrivacy 		type;
	TPckg<TLbsNetPosRequestPrivacy> typePkg(type);
	aMessage.ReadL(0, typePkg, 0);

	TLbsExternalRequestInfo 		requestInfo;
	TPckg<TLbsExternalRequestInfo> 	requestInfoPkg(requestInfo);
	aMessage.ReadL(1, requestInfoPkg, 0);
	
	TRAPD(err, iGatewayHandler->StartNetworkPrivacyRequestL(type, requestInfo));
	if (err != KErrNone)
		{
		aMessage.Complete(err);
		}
	}


/**
*/
void CLbsNetSimTestHandler::HandleReleaseLcsLocationNotificationCancelL()
	{
	iGatewayHandler->CancelNetworkPrivacyRequestL();
	}


/**
*/
void CLbsNetSimTestHandler::HandleStartNetworkLocationRequestL(const RMessage2& aMessage)
	{
	iMessageStartNetworkLocationRequest = aMessage;
	
	TRAPD(err, iGatewayHandler->StartNetworkLocationRequestL());
	iMessageStartNetworkLocationRequest.Complete(err);
	}


/**
*/
void CLbsNetSimTestHandler::HandleStartNetworkLocationRequestCancelL()
	{
	iGatewayHandler->CancelNetworkLocationRequestL();
	}

void CLbsNetSimTestHandler::HandleSendResetAssistanceData(const RMessage2& aMessage)
	{
	iGatewayHandler->SendResetAssistanceData(aMessage.Int0());
	
	aMessage.Complete(KErrNone);
	}


/**
*/
TInt CLbsNetSimTestHandler::StepModeWait()
	{
	TInt ret = KErrNone;
	
	if (iStepMode)
		{
		// Subscribe and wait for the step mode property
		TRequestStatus status;
		iStepModeProperty.Subscribe(status);
		User::WaitForRequest(status);
		
		// Get the new property
		iStepModeProperty.Get(ret);
		}
		
	return ret;
	}
