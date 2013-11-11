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
// lbsnrhserver.cpp
// server side exmaple of how to use the abstract server framework
// 
//

#include <e32debug.h>
#include <lbs/lbslocclasstypes.h>
#include "lbsnrhprivacycontrollerserver.h"
#include "opensessionparams.h"
#include "lbsnrhmessageenums.h"
#include "lbsnrhserverdata.h"
#include "nrhpanic.h"

TPositionInfoClassTypeFixer::TPositionInfoClassTypeFixer(const TPositionInfo& aPositionInfo) :
TPositionInfo(aPositionInfo)
  	{
  	iPosClassType = EPositionInfoClass;
  	iPosClassSize = sizeof(TPositionInfo);
  	}

CNrhPrivacyServerSubsession::CNrhPrivacyServerSubsession() :
	iPendingPrivacyRequests(1)
	{
	}

CNrhPrivacyServerSubsession::~CNrhPrivacyServerSubsession()
	{
	// cancel ANY outstanding requests with KErrServerTerminated
	// or some other error code to indicate that the session has been closed
	// early
	if(!iPrivacyControllerMessage.IsNull())
		{
		iPrivacyControllerMessage.Complete(KErrServerTerminated); 	
		}

	iPendingMessage.Reset();
	iPendingPrivacyRequests.Reset();
	iPendingLocationUpdates.Reset();
	iPendingSessionCompletes.Reset();
	CloseSubSession();
	}

CNrhPrivacyServerSubsession* CNrhPrivacyServerSubsession::NewL()
	{
	CNrhPrivacyServerSubsession* self = new (ELeave) CNrhPrivacyServerSubsession;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CNrhPrivacyServerSubsession::ConstructL()
	{
	}

void CNrhPrivacyServerSubsession::DispatchL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
		case ENrhPrivacyControllerRegister:
			SetClientReady(aMessage);
			break;
			
		case ENrhPrivacyControllerCancelRegister:
			CancelClientReady(aMessage);
			break;
			
		case ENrhPrivacyControllerResponse:
			HandlePrivacyResponse(aMessage);
			break;
			
		case ENrhPrivacyControllerCancel:
			HandlePrivacyCancel(aMessage);
			break;
		}
	}
	
void CNrhPrivacyServerSubsession::DispatchError(const RMessage2& /*aMessage*/, TInt /*aError*/)
	{
	// errors from DispatchL end up in here
	}
	
void CNrhPrivacyServerSubsession::CreateSubSessionL(const RMessage2& aMessage, 
									const CSecureServerBase* aServer)
	{	
	// Get hold of something for this subsession 
	// to pass on its messages (respond/cancel privacy request) to	
	iNrhServer = reinterpret_cast<const CNrhServer*>(aServer);

	// Check the version of the MLbsPrivacyObserver is supported
	User::LeaveIfError(CheckPrivacyObserverVersion(aMessage));

	// Let the privacyhandler know where it should be passing its messages
	// (privacy request, location update, session complete) to.
	iNrhServer->PrivacyObserver()->SetServerObserver(this);
	}
	
void CNrhPrivacyServerSubsession::CloseSubSession()
	{
	if (iNrhServer->PrivacyObserver() != NULL)
		{
		iNrhServer->PrivacyObserver()->SetServerObserver(NULL);
		}
	}
	
void CNrhPrivacyServerSubsession::VirtualRelease()
	{	
	//delete ourselves - can be called on a via CS on a leave
	delete this; // we are allocated via the GetImpl above, and a handle to "us" is returned
	}
	
void CNrhPrivacyServerSubsession::SetClientReady(const RMessage2& aMessage)
	{
	iPrivacyControllerMessage = aMessage;
	
	/* 
	 * See whether there are any messages which have been received since 
	 * sending the last one on to the client
	 */
	TInt pendingMessages = iPendingMessage.Count();
	if(pendingMessages)
		{
		TNrhServerMessageType messageType = iPendingMessage[0];
		iPendingMessage.Remove(0);
		switch(messageType)
			{
			case ENrhServerPrivacyRequest:
				{
				TPrivacyRequestParams privacyRequestParams = 
												iPendingPrivacyRequests[0];
		
				TPrivacyRequestParamsPckgC privacyRequestParamsPckg(privacyRequestParams);
				MessageUtils::Write(iPrivacyControllerMessage, 
							EPrivacyRequestParams, 
							privacyRequestParamsPckg);
				iPrivacyControllerMessage.Complete(ENrhServerPrivacyRequest);
				iPendingPrivacyRequests.Delete(0);
				break;
				}
				
			case ENrhServerLocationUpdate:
				{
				TLocationUpdateParams locationUpdateParams;
				locationUpdateParams.iSessionId = iPendingLocationUpdates[0].iSessionId;
								
				//It is necessary to update tha class type and size to TPositionInfo class type and size
				//in case if the pos update is of TPositionCourseInfo, TPositionSatelliteInfo, 
				//TPositionExtendedSatelliteInfo or HPositionGenericInfo class type.
				// Because only TPositionInfo part is passed to the LbsPrivacyController.
				TPositionInfoClassTypeFixer posFixer = iPendingLocationUpdates[0].iPositionInfo;  
				locationUpdateParams.iPositionInfo = posFixer;

				TLocationUpdateParamsPckgC locationUpdateParamsPckg(locationUpdateParams);
		
				MessageUtils::Write(iPrivacyControllerMessage, 
							ELocationUpdateParams, 
							locationUpdateParamsPckg);
				iPrivacyControllerMessage.Complete(ENrhServerLocationUpdate);
				iPendingLocationUpdates.Remove(0);
				break;
				}
				
			case ENrhServerLbsSessionComplete:
				{
				TLbsSessionCompleteParams lbsSessionCompleteParams = 
												iPendingSessionCompletes[0];
				TLbsSessionCompleteParamsPckgC lbsSessionCompleteParamsPckg(lbsSessionCompleteParams);
		
				MessageUtils::Write(iPrivacyControllerMessage, 
							ELbsSessionCompleteParams, 
							lbsSessionCompleteParamsPckg);
				iPrivacyControllerMessage.Complete(ENrhServerLbsSessionComplete);
				iPendingSessionCompletes.Remove(0);
				break;
				}
			}
		}
	}
	
void CNrhPrivacyServerSubsession::CancelClientReady(const RMessage2& aMessage)
	{
	// Cancel the outstanding request as well.
	if(!iPrivacyControllerMessage.IsNull())
		{
		iPrivacyControllerMessage.Complete(KErrNone); 	
		}

	// Complete this message (the cancel request)
	aMessage.Complete(KErrNone);
	}
	
void CNrhPrivacyServerSubsession::HandlePrivacyResponse(const RMessage2& aMessage)
	{
	
	// Get the data from the IPC message
	TPrivacyResponseParams privacyResponseParams;	
	TPckg<TPrivacyResponseParams> privacyResponseParamsBuf(privacyResponseParams);

	MessageUtils::Read(aMessage, 0, privacyResponseParamsBuf);

	TLbsNetworkEnumInt::TLbsPrivacyResponseInt response;
	
	if(privacyResponseParams.iResult == CLbsPrivacyController::ERequestAccepted)
		{
		response = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
		}
	else
		{
		response = TLbsNetworkEnumInt::EPrivacyResponseRejected;
		}

	iNrhServer->PrivacyObserver()->OnRespondNetworkLocationRequest(privacyResponseParams.iSessionId,
												response, KErrNone);

	aMessage.Complete(KErrNone);
	
	}
	
void CNrhPrivacyServerSubsession::HandlePrivacyCancel(const RMessage2& aMessage)
	{

	// Get the data from the IPC message
	TPrivacyCancelParams privacyCancelParams;	
	TPckg<TPrivacyCancelParams> privacyCancelParamsBuf(privacyCancelParams);

	MessageUtils::Read(aMessage, 0, privacyCancelParamsBuf);

	iNrhServer->PrivacyObserver()->OnCancelNetworkLocationRequest(privacyCancelParams.iSessionId);

	aMessage.Complete(KErrNone);
	
	}
void CNrhPrivacyServerSubsession::ProcessNetworkLocationRequest(TLbsNetSessionIdInt aSessionId, 
																const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt /*aSessionType*/,
                                         						const TLbsExternalRequestInfo&  aRequestInfo, 
                       											const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy,
                       											TBool /*aIsEmergency*/)
	{
	__ASSERT_DEBUG((aRequestInfo.ClassType() == EExternalRequestInfoClass) 
				   || (aRequestInfo.ClassType() == 
						(EExternalRequestInfoClass | EExternalRequestInfoClass2)), 
				   Panic(ENrhPanicInvalidExternalRequestInfoType));
	
	TPrivacyRequestParams privacyRequestData;
	privacyRequestData.iSessionId = aSessionId;
	Mem::Copy(&privacyRequestData.iRequestInfo,
			  &aRequestInfo,
			  aRequestInfo.ClassSize());
	privacyRequestData.iRequestPrivacy = aNetPosRequestPrivacy;

	if(!iPrivacyControllerMessage.IsNull())
		{
		TPrivacyRequestParamsPckgC privacyRequestDataPckg(privacyRequestData);
		MessageUtils::Write(iPrivacyControllerMessage, 
							EPrivacyRequestParams, 
							privacyRequestDataPckg);
		iPrivacyControllerMessage.Complete(ENrhServerPrivacyRequest); 	
		}
	else
		{
		TRAPD(err, iPendingPrivacyRequests.AppendL(privacyRequestData));
		// No point flagging a pending message if we didn't manage to store it
		if(err == KErrNone)
			{
			err = iPendingMessage.Append(ENrhServerPrivacyRequest);
			}
		}

	}
	
void CNrhPrivacyServerSubsession::ProcessNetworkPositionUpdate(TLbsNetSessionIdInt aSessionId, 
                              const TPositionInfo& aPositionInfo)
	{
	//It is necessary to update tha class type and size to TPositionInfo class type and size
	//in case if the pos update is of TPositionCourseInfo, TPositionSatelliteInfo, 
	//or HPositionGenericInfo class type. Because only TPositionInfo part is 
	//passed to the LbsPrivacyController.
	TLocationUpdateParams locationUpdateParams;
	locationUpdateParams.iSessionId = aSessionId;
	TPositionInfoClassTypeFixer posFixer = aPositionInfo;  
	locationUpdateParams.iPositionInfo = posFixer;
    
	if(!iPrivacyControllerMessage.IsNull())
		{
		TLocationUpdateParamsPckgC locationUpdateParamsPckg(locationUpdateParams);
		MessageUtils::Write(iPrivacyControllerMessage, 
							ELocationUpdateParams, 
							locationUpdateParamsPckg);
		iPrivacyControllerMessage.Complete(ENrhServerLocationUpdate); 	
		}
	else
		{
		TInt err = iPendingLocationUpdates.Append(locationUpdateParams);
		// No point flagging a pending message if we didn't manage to store it
		if(err == KErrNone)
			{
			err = iPendingMessage.Append(ENrhServerLocationUpdate);
			}
		}
	}
	
	
void CNrhPrivacyServerSubsession::ProcessRequestComplete(TLbsNetSessionIdInt aSessionId, 
                                TInt aReason)
	{


	TLbsSessionCompleteParams lbsSessionCompleteParams;
	lbsSessionCompleteParams.iSessionId = aSessionId;
	lbsSessionCompleteParams.iReason = aReason;

	if(!iPrivacyControllerMessage.IsNull())
		{
		TLbsSessionCompleteParamsPckgC lbsSessionCompleteParamsPckg(lbsSessionCompleteParams);
		MessageUtils::Write(iPrivacyControllerMessage, 
							ELbsSessionCompleteParams, 
							lbsSessionCompleteParamsPckg);
		iPrivacyControllerMessage.Complete(ENrhServerLbsSessionComplete); 	
		}
	else
		{
		TInt err = iPendingSessionCompletes.Append(lbsSessionCompleteParams);
		// No point flagging a pending message if we didn't manage to store it
		if(err == KErrNone)
			{
			err = iPendingMessage.Append(ENrhServerLbsSessionComplete);
			}
		}
	}

/** Check the version of MLbsPrivacyObserver the client is using is supported. 
*/
TInt CNrhPrivacyServerSubsession::CheckPrivacyObserverVersion(const RMessage2& aMessage)
	{
	TInt result = KErrNone;
	
	TNrhPrivacyControllerData data;
	TPckg<TNrhPrivacyControllerData> dataPckg(data);
	MessageUtils::Read(aMessage, 1, dataPckg);

	if (data.iVersion.iMajor == KLbsPrivacyObserverVersionMajor
		&& data.iVersion.iMinor == KLbsPrivacyObserverVersionMinor)
		{
		result = KErrNone;
		}
	else if (data.iVersion.iMajor == KLbsPrivacyObserverVersion2Major
			 && data.iVersion.iMinor == KLbsPrivacyObserverVersion2Minor)
		{
#if defined(SYMBIAN_LOCATION_PRIVACY_V2)
		result = KErrNone;
#else
		result = KErrNotSupported;
#endif // SYMBIAN_LOCATION_PRIVACY_V2
		}
	else
		{
		// Unrecognised version; so return KErrNotSupported
		result = KErrNotSupported;
		}
	
	return result;
	}
