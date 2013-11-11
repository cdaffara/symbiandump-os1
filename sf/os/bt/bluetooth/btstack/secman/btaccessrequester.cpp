// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>

#include "secman.h"
#include "secevent.h"
#include "hostresolver.h"
#ifdef BT_LINKMGR_V2
#include "physicallinks.h"
#include "physicallinksmanager.h"
#else
#include "PhysicalLinks.h"
#include "PhysicalLinksManager.h"
#endif

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SECMAN);
#endif

#ifdef _DEBUG
PANICCATEGORY("btaccreq");
#endif

/**
	class CBTAccessRequester


	The access requester handles the state machine involved in allowing accesses:
	setting authentication, entering PINs, asking for authorisation and encrypting

	This class deals only with security procedures we initiate (which could be for
	inbound or outbound connections)

*/

CBTAccessRequester* CBTAccessRequester::NewLC(CPhysicalLink& aConnection,
											  const TBTServiceSecurity& aSecurityRequired,
											  const TBTServiceSecurityPerDevice* const aOverride,
											  MAccessRequestResponseHandler& aRequester,
											  TAccessType aAccessType,
											  TBool aNoSecurityRequired,
											  CBTSecMan& aParent)
	{
	LOG_STATIC_FUNC
	CBTAccessRequester* s = new(ELeave) CBTAccessRequester(aConnection, aSecurityRequired, aOverride, aRequester, aAccessType, aNoSecurityRequired, aParent);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}


CBTAccessRequester::CBTAccessRequester(CPhysicalLink& aConnection,
									   const TBTServiceSecurity& aServiceSecurity,
									   const TBTServiceSecurityPerDevice* const aOverride,
									   MAccessRequestResponseHandler& aRequester,
									   TAccessType aAccessType,
									   TBool aNoSecurityRequired,
									   CBTSecMan& aParent)
	: iRequester(aRequester)
	, iSecMan(aParent)
	, iBaseband(&aConnection)
	, iDevAddr(aConnection.BDAddr())
	, iServiceRequirements(aServiceSecurity)
	, iOverride(aOverride)
	, iIsSubscribedToConnection(EFalse)
	, iDeviceRetrievedFromRegistry(EFalse)
	, iQueLink(this)
	, iPrefetchQueueLink(this)
	, iAuthenticationInProgress(EFalse)
	, iAccessType(aAccessType)
	, iRemoteIndicatedNoBonding(EFalse)
	, iCurrentState(EBTUninitialised)
	, iNoSecurityRequired(aNoSecurityRequired)
	{
	LOG_FUNC
	// try to get name for UI dialogs
	SetDeviceName();
	}

CBTAccessRequester::~CBTAccessRequester()
	{
	LOG_FUNC
	if (iBaseband && iIsSubscribedToConnection)
		{
		iBaseband->UnsubscribeLinkObserver(*this);
		}
	iPrefetchQueueLink.Deque();
	delete iAuthorisor;
	delete iTimer;
	}

void CBTAccessRequester::ConstructL()
	{
	LOG_FUNC
	iTimer = CAuthenticationTimer::NewL(*this);
	SubscribeToLinkObserver();
	}

void CBTAccessRequester::SubscribeToLinkObserver()
	{
	LOG_FUNC
	iBaseband->SubscribeLinkObserver(*this);
	iIsSubscribedToConnection = ETrue;
	if (BasebandConnected() && iBaseband->SimplePairingMode() != EPhySimplePairingUndefined)
		{
		TBTSecEvent event(TBTSecEvent::EPhysicalLinkUp);
		SendEvent(event);
		}
	}


//
// Events
//

template<class XDerivedSecEventType, TBTSecEvent::TEventType XEventType>
XDerivedSecEventType* DynamicEventCast(TBTSecEvent* aEvent)
	{
	if(aEvent && aEvent->Event() == XEventType)
		{
		return static_cast<XDerivedSecEventType*>(aEvent);
		}
	return NULL;
	}

TBTSecEventStart* TBTSecEventStart::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventStart, EStart>(aEvent);
	}

TBTSecEventAuthorisationComplete* TBTSecEventAuthorisationComplete::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventAuthorisationComplete, EAuthorisationComplete>(aEvent);
	}

TBTSecEventIoCapabilityResponse* TBTSecEventIoCapabilityResponse::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventIoCapabilityResponse, EIOCapsResponse>(aEvent);
	}

TBTSecEventIoCapabilityRequested* TBTSecEventIoCapabilityRequested::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventIoCapabilityRequested, EIOCapsRequested>(aEvent);
	}

TBTSecEventKeypressEntry* TBTSecEventKeypressEntry::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventKeypressEntry, EKeypressEntry>(aEvent);
	}

TBTSecEventUserConfirmationRequest* TBTSecEventUserConfirmationRequest::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventUserConfirmationRequest, EUserConfirmationRequested>(aEvent);
	}

TBTSecEventRemoteOOBDataRequest* TBTSecEventRemoteOOBDataRequest::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventRemoteOOBDataRequest, ERemoteOOBDataRequested>(aEvent);
	}

TBTSecEventPhysicalLinkUp* TBTSecEventPhysicalLinkUp::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventPhysicalLinkUp, EPhysicalLinkUp>(aEvent);
	}

TBTSecEventAuthenticationComplete* TBTSecEventAuthenticationComplete::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventAuthenticationComplete, EAuthenticationComplete>(aEvent);
	}

TBTSecEventEncryptionChangeComplete* TBTSecEventEncryptionChangeComplete::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventEncryptionChangeComplete, EEncryptionChangeComplete>(aEvent);
	}

TBTSecEventAuthenticationRequested* TBTSecEventAuthenticationRequested::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventAuthenticationRequested, EAuthenticationRequested>(aEvent);
	}

TBTSecEventRequestAuthentication* TBTSecEventRequestAuthentication::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventRequestAuthentication, ERequestAuthentication>(aEvent);
	}

TBTSecEventAuthorisationRequested* TBTSecEventAuthorisationRequested::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventAuthorisationRequested, EAuthorisationRequested>(aEvent);
	}

TBTSecEventRequestAuthorisation* TBTSecEventRequestAuthorisation::Cast(TBTSecEvent* aEvent)
	{
	return DynamicEventCast<TBTSecEventRequestAuthorisation, ERequestAuthorisation>(aEvent);
	}



// This function is expected to be re-entrant
TBool CBTAccessRequester::SendEvent(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	LOG2(_L8("iCurrentState = %d, aEvent.Event() = %d"), iCurrentState, aEvent.Event());
	TInt err = KErrNotFound; // Let the caller know if the event has been accepted and therefore is being handled ok.
	err = iSecMan.StateMachine()->ProcessRequesterState(iCurrentState, *this, aEvent);
	if(err != KErrNone)
		{
		__ASSERT_DEBUG(err != KErrNotFound, PANIC(KBTSecPanic, EBTSecCouldNotFindStateTransition));
		CompleteRequest(err);
		return EFalse;
		}
	return ETrue;
	}


void CBTAccessRequester::Ready(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEvent* event = TBTSecEventPhysicalLinkUp::Cast(&aEvent);
	event = event ? event : TBTSecEventStart::Cast(&aEvent);
	__ASSERT_DEBUG(event, PANIC(KBTSecPanic, EBTSecUnexpectedStateMachineEventId));

	iRequirements = OverallRequirements(iServiceRequirements, iBaseband->RemoteDevice());
		
	if (AuthenticationRecommended())
		{
		LOG(_L8("\tAuthentication recommended"))
		/* For dedicated bonding we always want to request authentication (even if we are already authenticated)
		 * so that we can upgrade the linkkey if the new linkkey is stronger.
		 */
		if(iBaseband->Authenticated() && LinkKeyGoodEnough() && (iAccessType != EDedicatedBonding))
			{
			TBTSecEvent newevent(TBTSecEvent::EAuthenticationComplete);
			SendEvent(newevent);
			}
		else
			{
			LOG(_L8("\tPhysical link needs to be authenticated"))
			TBTSecEvent newevent(TBTSecEvent::ERequestAuthentication);
			SendEvent(newevent);
			}
		}
	else
		{
		LOG(_L8("\tAuthentication is not required or recommended"))
		TBTSecEvent newevent(TBTSecEvent::EAuthenticationComplete);
		SendEvent(newevent);
		}
	}

void CBTAccessRequester::RequestAuthentication(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEventRequestAuthentication* event = TBTSecEventRequestAuthentication::Cast(&aEvent);
	__ASSERT_DEBUG(event, User::Panic(KBTSecPanic, EBTSecUnexpectedStateMachineEventId));

	// Here we issue the request if authentication is "recommended" - not neccessarily required.
	if	(
		AuthenticationRecommended()
		&&
			(
			!iBaseband->Authenticated()
			||
			!LinkKeyGoodEnough()
			||
			iAccessType == EDedicatedBonding
			)
		)
		{
		LOG(_L8("\tAuthentication required..."));
		LOG(_L8("\tStarting Authentication..."));
		if(iBaseband->Authenticate(iRequirements.MitmProtection() != EMitmNotRequired) !=KErrNone)
			{
			// the remote device is authenticated, but longer passkey is required
			CompleteRequest(EBTSecManAccessDenied);
			}
		else
			{
			SetAuthenticationInProgress();
			// Pending authentication...
			}
		}
	else
		{
		LOG(_L8("\tAuthentication not required go to next state"))
		TBTSecEvent event(TBTSecEvent::EAuthenticationComplete);
		SendEvent(event);
		}
	}

void CBTAccessRequester::AuthenticationRequested(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEventAuthenticationRequested* event = TBTSecEventAuthenticationRequested::Cast(&aEvent);
	__ASSERT_DEBUG(event, User::Panic(KBTSecPanic, EBTSecUnexpectedStateMachineEventId));

	if(aEvent.Error() == KErrNone)
		{
		// Start guard timer?
		iTimer->Start();

		SetAuthenticationInProgress();
		// Pending authentication...
		}
	else
		{
		CompleteRequest(EBTSecManAccessDenied);
		}
	}

void CBTAccessRequester::AuthenticationComplete(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEventAuthenticationComplete* event = TBTSecEventAuthenticationComplete::Cast(&aEvent);
	__ASSERT_DEBUG(event, User::Panic(KBTSecPanic, EBTSecUnexpectedStateMachineEventId));
	
	iTimer->Cancel();
	
	// Here we check that we have successfully authenticated the link before progressing to the
	// next stage (encryption).  Here success is considered as when ...
	if (
		!AuthenticationRequired() // either authentication is not required...
		|| // or authentication is required...
			( 
			iBaseband->Authenticated() // and the phys is already authenticated, ...
			&&
			LinkKeyGoodEnough() // and the link key is of a sufficient strength.
			)
		)
			{
			// Initiate encryption
			TBTSecEvent newevent(TBTSecEvent::ERequestEncryptionChange);
			SendEvent(newevent);
			}
		else if (
				aEvent.Error() == ELMPErrorTransactionCollision
				||
				aEvent.Error() == EDifferentTransactionCollision
				)
			{
			// Re-attempt authentication request
			TBTSecEvent newevent(TBTSecEvent::ERequestAuthentication);
			SendEvent(newevent);
			}
		else if(aEvent.Error() == ERemoteUserEndedConnection)
			{
			// If end user brough the connection down then we'll see a disconnection in
			// a minute - and that will handle prefetch cases.
			}
		else
			{
			// Failed to set-up authentication conditions required by the access requester.
			CompleteRequest(EBTSecManAccessDenied);
			}
	} 

void CBTAccessRequester::EncryptionChangePending(TBTSecEvent& aEvent)
	{
	LOG_FUNC

	switch(aEvent.Event())
		{
		case TBTSecEvent::ERequestEncryptionChange:
			{
			if(EncryptionRequired() && !iBaseband->Encrypted())
				{
				LOG(_L8("\tEncryption required..."))

				if (!iBaseband->IsEncryptionDisabledForRoleSwitch())
					{
					TInt err = iBaseband->ChangeEncryption(EPointToPointEncryption);
					if(err!=KErrNone)
						{
						CompleteRequest(EBTSecManAccessDenied);
						}
						// Pending encryption...
					}
					// Pending encryption...
				}
			else
				{
				TBTSecEvent event(TBTSecEvent::EEncryptionChangeComplete);
				SendEvent(event);
				}
			}
		break;
		case TBTSecEvent::EEncryptionChangeRequested:
			// start guard timer
			iTimer->Start();
		break;
		default:
			User::Panic(KBTSecPanic,EBTSecUnexpectedStateMachineEventId);
		break;
		}
	}

void CBTAccessRequester::EncryptionChangeComplete(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEventEncryptionChangeComplete* event = TBTSecEventEncryptionChangeComplete::Cast(&aEvent);
	__ASSERT_DEBUG(event, User::Panic(KBTSecPanic, EBTSecUnexpectedStateMachineEventId));
	
	// cancel guard timer
	iTimer->Cancel();
	
	// We can try again if there was a race with some other PHY modification
	// Curiously, the firmware always return ELMPErrorTransactionCollision (0x23) for both
	// kinds of transaction collisions (0x23,0x2a), we guard against both situations here
	// anyway just to be safe.
	if((aEvent.Error() == ELMPErrorTransactionCollision) || (aEvent.Error() == EDifferentTransactionCollision))
		{	
		// This will force the state machine logic to try sending the command again
		TBTSecEvent newevent(TBTSecEvent::ERequestEncryptionChange);
		SendEvent(newevent);
		}
	else if((aEvent.Error() == KErrNone) && 
				(!EncryptionRequired() || // The extra checks may not be needed...
				(EncryptionRequired() && iBaseband->Encrypted())))
		{
		TBTSecEvent newevent(TBTSecEvent::ERequestAuthorisation);
		SendEvent(newevent);
		}
	else
		{
		CompleteRequest(EBTSecManAccessDenied);
		}
	}

void CBTAccessRequester::RequestAuthorisation(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEventRequestAuthorisation* event = TBTSecEventRequestAuthorisation::Cast(&aEvent);
	__ASSERT_ALWAYS(event, User::Panic(KBTSecPanic, EBTSecUnexpectedStateMachineEventId));
	if (iRequirements.AuthorisationRequired())
		{
		LOG(_L8("\tAuthorisation required..."))
		LOG(_L8("\tStarting Authorisation..."))
		__ASSERT_ALWAYS(!iAuthorisor, User::Panic(KBTSecPanic,EBTSecAuthorisationRequestAlreadyExists));
		TRAPD(err,iAuthorisor = CBTAuthorisor::NewL(*this, iSecMan, iServiceRequirements.Uid()));
		if(err != KErrNone)
			{
			// the remote device is authenticated, but longer passkey is required
			CompleteRequest(EBTSecManAccessDenied);
			}
			// Pending authorisation...
		}
	else
		{
		TBTSecEventAuthorisationComplete event(ETrue);
		SendEvent(event);
		}
	}

void CBTAccessRequester::AuthorisationRequested(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEventAuthorisationRequested* event = TBTSecEventAuthorisationRequested::Cast(&aEvent);
	__ASSERT_ALWAYS(event, User::Panic(KBTSecPanic, EBTSecUnexpectedStateMachineEventId));
	iTimer->Start();
	}

void CBTAccessRequester::AuthorisationComplete(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEventAuthorisationComplete* event = TBTSecEventAuthorisationComplete::Cast(&aEvent);
	__ASSERT_DEBUG(event, User::Panic(KBTSecPanic, EBTSecUnexpectedStateMachineEventId));
		
	// cancel guard timer
	iTimer->Cancel();
	if(event->Error() != KErrNone && iRequirements.AuthorisationRequired())
		{
		CompleteRequest(EBTSecManAccessDenied);
		}
	else
		{
		if(event->AccessAllowed())
			{
			CompleteRequest(EBTSecManAccessGranted);
			}
		else
			{
			CompleteRequest(EBTSecManAccessDenied);
			}
		}
	}

void CBTAccessRequester::IOCapsResponse(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEventIoCapabilityResponse* event = TBTSecEventIoCapabilityResponse::Cast(&aEvent);
	__ASSERT_ALWAYS(event, User::Panic(KBTSecPanic,EBTSecBadStateMachineEvent));
	
	// store/do something with the remote device's IO caps
	iRemoteIOCapability = event->IoCapability();
	iRemoteOOBDataPresence = event->OobDataPresent();
	
	// If remote device says "no bonding" when asked for dedicated
	// bonding, remember this for sending a negative reply later
	THCIAuthenticationRequirement authReq = event->AuthenticationRequirements();
	if (iAccessType == EDedicatedBonding && (authReq == EMitmReqNoBonding || authReq == EMitmNotReqNoBonding))
		{
		iRemoteIndicatedNoBonding = ETrue;
		}
	}

void CBTAccessRequester::IOCapsRequested(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEventIoCapabilityRequested* event = TBTSecEventIoCapabilityRequested::Cast(&aEvent);
	__ASSERT_ALWAYS(event, User::Panic(KBTSecPanic,EBTSecBadStateMachineEvent));
	
	// Send local IO caps to remote device
	
	// Host and Controller supports SSP.
	
	// Send HCI_IO_Capability_Request_Reply with...
	// BD_ADDR
	// IO_Capability:		DisplayYesNo 
	// OOB_Data_Present:	check secman::IsOOBDataAvailable(BD_ADDR)
	// Authentication_Requirements:	depends on connection type (Bonding, ACL, Service)
	// 								check secman::ConnectionType()
	
	// For test assume...
	// OOB_Data_Present = EOOBDataNotPresent
	// Authentication_Requirements = EMITMNRGB

	if(iBaseband->IsPairable())
		{
		THCIAuthenticationRequirement authReq = EMitmNotReqGeneralBonding;
		
		// If MITM protection is required by a service, or the device is in paired only connections mode
		// the require MITM protection.
		if (iRequirements.MitmProtection() != EMitmNotRequired)
			{
			if (iAccessType == EDedicatedBonding)
				{
				authReq = EMitmReqDedicatedBonding;
				}
			else
				{
				authReq = EMitmReqGeneralBonding;
				}
			iBaseband->SetLocalMITM(ETrue);
			}
		else
			{
			if (iAccessType == EDedicatedBonding)
				{
				authReq = EMitmNotReqDedicatedBonding;
				}
			else
				{
				authReq = EMitmNotReqGeneralBonding;
				}
			iBaseband->SetLocalMITM(EFalse);
			}
		
		THCIOobDataPresence oobPresence = EOOBDataNotPresent;
		if (iBaseband->HasRemoteOobData())
			{
			oobPresence = EOOBDataPresent;
			}
		
		TRAPD(err, iSecMan.CommandController().IOCapabilityRequestReplyL(iBaseband->BDAddr(), EIOCapsDisplayYesNo, oobPresence, authReq));
		if(err!=KErrNone)
			{
			CompleteRequest(EBTSecManAccessDenied);
			}
		}
	else
		{
		TRAPD(err, iSecMan.CommandController().IOCapabilityRequestNegativeReplyL(iBaseband->BDAddr(), EPairingNotAllowed));
		if(err!=KErrNone)
			{
			CompleteRequest(EBTSecManAccessDenied);
			}
		}
	}

void CBTAccessRequester::UserConfirmation(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEventUserConfirmationRequest* event = TBTSecEventUserConfirmationRequest::Cast(&aEvent);
	__ASSERT_ALWAYS(event, User::Panic(KBTSecPanic, EBTSecBadStateMachineEvent));
		
	// start guard timer...
	iTimer->Start();
	}

void CBTAccessRequester::PasskeyEntry(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	//just progressing through the state machine in case of a CBTAccessRequester present.
	//do nothing here because the action has been handled by secman
	switch(aEvent.Event())
		{
		case TBTSecEvent::EPasskeyNotfication:
			//do nothing as secman handles the processing
		break;
		case TBTSecEvent::EKeypressEntry:
			{
			TBTSecEventKeypressEntry* event = TBTSecEventKeypressEntry::Cast(&aEvent);
			__ASSERT_ALWAYS(event, User::Panic(KBTSecPanic,EBTSecBadStateMachineEvent));
			//do nothing as secman handles the processing
			}
		break;
		default:
			User::Panic(KBTSecPanic,EBTSecUnexpectedStateMachineEventId);
		break;
		}
	}

void CBTAccessRequester::RemoteOOBDataRequest(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	TBTSecEventRemoteOOBDataRequest* event = TBTSecEventRemoteOOBDataRequest::Cast(&aEvent);
	__ASSERT_ALWAYS(event, User::Panic(KBTSecPanic, EBTSecBadStateMachineEvent));
		
	// just progressing through the state machine in case of a CBTAccessRequester present.
	// do nothing here because the action has been handled by secman
	}

void CBTAccessRequester::SimplePairingPending(TBTSecEvent& aEvent)
	{
	LOG_FUNC
	switch(aEvent.Event())
		{
		case TBTSecEvent::EUserConfirmationRequestComplete:
			{
			// restart guard timer
			iTimer->Start();
			}
		break;
		case TBTSecEvent::EKeypressComplete:
			// restart guard timer
			iTimer->Start();
		break;
		case TBTSecEvent::ERemoteOOBDataRequestComplete:
			// restart guard timer
			iTimer->Start();
		break;
		default:
			User::Panic(KBTSecPanic,EBTSecUnexpectedStateMachineEventId);
		break;
		}
	}


void CBTAccessRequester::SetDeviceName()
	{
	LOG_FUNC
	// not *that* bad - mostly inlines
	
	iDeviceName = iSecMan.ConnectionsManager().
					LinkManagerProtocol().InquiryMgr().
					  DeviceNameFromCache(iBaseband->BDAddr());
	}


const MAccessRequestResponseHandler& CBTAccessRequester::ServiceRequester() const
	{
	LOG_FUNC
	return iRequester;
	}

const TBTDevAddr& CBTAccessRequester::DeviceAddress() const
	{
	LOG_FUNC
	return iDevAddr;
	}

void CBTAccessRequester::Start()
	{
	LOG_FUNC
	TBTSecEvent event(TBTSecEvent::EStart, KErrNone);
	SendEvent(event);
	}

void CBTAccessRequester::CompleteRequest(TInt aResult)
	{
	LOG_FUNC
#ifdef __FLOG_ACTIVE
	if(aResult == EBTSecManAccessDenied)
		{
		LOG(_L8("\tACCESS DENIED"));
		}
	else if(aResult == EBTSecManAccessGranted)
		{
		LOG(_L8("\tACCESS GRANTED"));
		}
	else
		{
		LOG1(_L8("\tERROR (%d)"), aResult);
		}
#endif // __FLOG_ACTIVE
	
	if (aResult == EBTSecManAccessGranted && RemoteIndicatedNoBondingToDedicatedBonding())
		{
		// We allow the device to bond, but tell theUI layer so it can delete the link key if it wants to
		aResult = KErrRemoteDeviceIndicatedNoBonding;
		LOG(_L8("\t... but remote indicated no bonding"));
		}
	
	iSecMan.AccessRequestComplete(this, aResult);
	}

/**
Take the access requirements of the service and compare them with the permissions given 
to the device by the user.  This results in a list of tasks to be carried out by the 
access requester before it may let the connection proceed.
**/
TBTAccessRequirements CBTAccessRequester::OverallRequirements(const TBTServiceSecurity& aServiceSecurity,
														 const TBTNamelessDevice& aDevice)
	{
	LOG_FUNC
	//Create the initial stab at the access requirements from the requirements of this service...
	TBTAccessRequirements req;
	req.SetAuthentication(aServiceSecurity.MitmProtection());
	req.SetAuthorisation(aServiceSecurity.AuthorisationRequired());
	req.SetEncryption(aServiceSecurity.EncryptionRequired());
	req.SetDenied(aServiceSecurity.Denied());
	req.SetPasskeyMinLength(aServiceSecurity.PasskeyMinLength());
	
	//Look at the global security setting of the device...
	if (aDevice.IsValidGlobalSecurity())
		{
		LOG(_L8("\tDevice has global security settings"));
		TBTDeviceSecurity devSec = aDevice.GlobalSecurity();
		if (devSec.Banned())
			req.SetDenied(ETrue);
		if (devSec.Encrypt())
			req.SetEncryption(ETrue);
		if (devSec.MitmRequirements() == TBTDeviceSecurity::EMitmRequired)
			req.SetAuthentication(EMitmRequired);
		if (devSec.NoAuthorise())
			req.SetAuthorisation(EFalse);
		if (devSec.PasskeyMinLength() && devSec.PasskeyMinLength() > req.PasskeyMinLength())
			req.SetPasskeyMinLength(devSec.PasskeyMinLength());
		}

	//Check to see if the device has anything specific about this service...

	if (iOverride)
		{
		// get the device security from the override (the override points to the correct device)
		LOG(_L8("\tSAP has overriden global security settings for device"));
		const TBTDeviceSecurity& servSec = iOverride->DeviceSecurity();
		if (servSec.Banned())
			req.SetDenied(ETrue);
		if (servSec.Encrypt())
			req.SetEncryption(ETrue);
		if (servSec.MitmRequirements() == TBTDeviceSecurity::EMitmRequired)
			req.SetAuthentication(EMitmRequired);
		if (servSec.NoAuthorise())
			req.SetAuthorisation(EFalse);
		if (servSec.PasskeyMinLength() && servSec.PasskeyMinLength() > req.PasskeyMinLength() )
			req.SetPasskeyMinLength(servSec.PasskeyMinLength());
		}

	// If encryption is required then authentication is implicitly required.  The logic for 
	// determining this is in the AuthenticationRequired function, that for any particular
	// moment determies whether or not authentication is required.

	return req;
	}

void CBTAccessRequester::PhysicalLinkChange(const TBTBasebandEventNotification & aEvent, CPhysicalLink& /*aPhysicalLink*/)
	{
	LOG_FUNC
	// only forward events that secman is interested in
	// linkup, linkdown, encryption, authentication, error
	// Care needed: other events may harm operation of secman 
	// and open security hole, such as ENotifySniffMode, ENotifyParkMode
	// and ENotifyHoldMode
	TBTPhysicalLinkStateNotifier secmanEvents = static_cast<TBTPhysicalLinkStateNotifier>
	           (ENotifyPhysicalLinkUp |
	            ENotifyPhysicalLinkDown |
	            ENotifyPhysicalLinkError |
	            ENotifyAuthenticationComplete |
	            ENotifyEncryptionChangeOn |
	            ENotifyEncryptionChangeOff );

	if (aEvent.EventType() & secmanEvents)
		{
		PhysicalLinkChange(aEvent);
		}
	// else drop
	}

TPhysicalLinkObserverQLink& CBTAccessRequester::ObserverQLink()
	{
	LOG_FUNC
	return iQueLink;
	}

void CBTAccessRequester::PhysicalLinkChange(const TBTBasebandEventNotification & aEvent)
	{
	LOG_FUNC;
	if(aEvent.EventType() == ENotifyPhysicalLinkUp && iBaseband->SimplePairingMode() != EPhySimplePairingUndefined)
		{
		LOG(_L8("\tPhysical link up..."))
		TBTSecEvent event(TBTSecEvent::EPhysicalLinkUp, aEvent.ErrorCode());
		SendEvent(event);
		}
	else if(aEvent.EventType() == ENotifyAuthenticationComplete)
		{
		LOG(_L8("\tAuthentication complete..."))
		TBTSecEvent event(TBTSecEvent::EAuthenticationComplete, aEvent.ErrorCode());
		SendEvent(event);
		}
	else if((aEvent.EventType() == ENotifyEncryptionChangeOn)||(aEvent.EventType() == ENotifyEncryptionChangeOff))
		{
		LOG(_L8("\tEncryption Change complete..."))
		TBTSecEvent event(TBTSecEvent::EEncryptionChangeComplete, aEvent.ErrorCode());
		SendEvent(event);
		}
	else if((aEvent.EventType() == ENotifyPhysicalLinkDown))
		{
		LOG(_L8("\tPhysical link down...Complete(ACCESS DENIED)"));
		LinkError(aEvent.ErrorCode());
		}
	else if((aEvent.EventType() == ENotifyPhysicalLinkError))
		{
		LOG(_L8("\tPhysical link error...Complete(ACCESS DENIED)"));
		iSecMan.ConnectionsManager().PrefetchMan().RemovePrefetch(DeviceAddress());		
		LinkError(aEvent.ErrorCode());
		}
	}

TPrefetchNotifierQLink& CBTAccessRequester::MbpnQueLink()
	{
	LOG_FUNC
	return iPrefetchQueueLink;
	}

void CBTAccessRequester::MbpnPrefetchComplete(TInt aError)
	{
	LOG_FUNC
	if(aError == KErrNone)
		{
		aError = EBTSecManAccessDeferred;
		}
	CompleteRequest(aError);
	}

void CBTAccessRequester::LinkError(TInt aError)
	{
	LOG_FUNC
	// Speculatively register for a prefetch notification
	TInt error = iSecMan.ConnectionsManager().PrefetchMan().RegisterForPrefetching(DeviceAddress(), *this);

	// Whatever we do - our CPhysicalLink representation is going to disappear
	if (iIsSubscribedToConnection)
		{
		iBaseband->UnsubscribeLinkObserver(*this);
		iIsSubscribedToConnection = EFalse;
		}
	if(error != KErrNone)
		{
		// No prefetch available, so fail the access request now.
		CompleteRequest(EBTSecManAccessDenied);
		return; //CompleteRequest deletes us
		}
	else
		{
		// Prefetch is available - so we need to enter an unconnected state.
		TBTSecEvent event(TBTSecEvent::EPhysicalLinkDown, aError);
		if(SendEvent(event))
			{
			// Physical link representation can be considered gone.
			iBaseband = NULL;
			}
		}
	}

TBool CBTAccessRequester::IsAuthenticationReqPending(const TBTDevAddr& aAddr, TUint& aPasskeyMinLength, TBluetoothMitmProtection& aMitmLevel)
	{
	LOG_FUNC
	if(aAddr == iBaseband->BDAddr() && (iCurrentState == EBTAuthenticationRequested || iCurrentState == EBTRequestAuthentication ||
			                            iCurrentState == EBTIOCapsResponse  || 	iCurrentState == EBTIOCapsRequested ||
			                            iCurrentState == EBTUserConfirmation || iCurrentState == EBTPasskeyEntry ||
			                            iCurrentState == EBTRemoteOOBDataRequest || iCurrentState == EBTSimplePairingPending))
		{

		aPasskeyMinLength = iServiceRequirements.PasskeyMinLength();
		aMitmLevel = iServiceRequirements.MitmProtection();
		return ETrue;
		}
	else 
		{
		aPasskeyMinLength = 0;
		aMitmLevel = EMitmNotRequired;
		return EFalse;
		}
	}

TBool CBTAccessRequester::AuthenticationRequired() const
	{
	LOG_FUNC
	ASSERT_DEBUG(iBaseband->SimplePairingMode() != EPhySimplePairingUndefined);
	
	// Here we determine if authentication is required on the link.
	TBool authenticationRequired = EFalse;
	
	if (iNoSecurityRequired)
		{
		// This is used for Security Mode 4 "no security required", e.g. SDP, 
		// which we never authenticate
		// <NOP> // authenticationRequired is already EFalse
		}
	else if(iBaseband->SimplePairingMode() == EPhySimplePairingEnabled)
		{
		// If operating in simple pairing mode then authentication is always required
		// due to security mode 4.
		authenticationRequired = ETrue;
		}
	else
		{
		// Otherwise we are operating a legacy pre-v2.1 link, and we have to be more
		// clever about whether authentication is required.
		if(iAccessType == EGeneralBondingSecurityMode4Outgoing)
			{
	        // If this is the security mode 4 access requests and there is no simple pairing
			// then don't bother - we will authenticate later when the security mode 2 access
			// requester is used.
			// <NOP> // authenticationRequired is already EFalse
			}
		else if(iRequirements.MitmProtection() != EMitmNotRequired)
			{
			// If we have any form of MITM specification then we should perform authentication
			// to provide the best MITM protection we can (PIN code entry).
			authenticationRequired = ETrue;
			}
		else if(iRequirements.EncryptionRequired())
			{
			// If encryption is required then authentication is needed to establish the
			// encrypted link.
			authenticationRequired = ETrue;
			}
		}
	return authenticationRequired;
	}

TBool CBTAccessRequester::AuthenticationRecommended() const
	{
	LOG_FUNC
	TBool authenticationRecommended = AuthenticationRequired();
	if(!authenticationRecommended && LinkKeyGoodEnough() && !iNoSecurityRequired)
		{
		// If the current link key is good enough then we should be able to use that
		// if it is available.
		if(iSecMan.ConnectionsManager().LinkManagerProtocol().IsSecureSimplePairingSupportedLocally())
			{
			// Of course this is only "recommended" when using controllers capable of renegotiating
			// stronger (in terms of PIN length) link keys.  We don't want to be stuck we a
			// low quality link key from the first service level connection (probably
			// SDP) that the device initiates...
			authenticationRecommended = ETrue;
			}
		}
	return authenticationRecommended;
	}

TBool CBTAccessRequester::EncryptionRequired() const
	{
	LOG_FUNC
	ASSERT_DEBUG(iBaseband->SimplePairingMode() != EPhySimplePairingUndefined);

	// Here we determine if encryption is required on the link.
	TBool encryptionRequired = EFalse;

	if(iRequirements.EncryptionRequired())
		{
		// If the access request is for a dedicated bond then there is no need to perform any
		// encryption.  If the access requester has specified encryption and dedicated bonding then
		// this can be considered a defect.
		__ASSERT_DEBUG(iAccessType != EDedicatedBonding, PANIC(KBTSecPanic, EBTSecEncryptionRequestForDedicatedBond));
		encryptionRequired = ETrue; // If a specification of encryption is made explicitly then we honour it.
		}
	else if(iAccessType != EDedicatedBonding)
		{
		// Otherwise dedicated bonding is incidental...so only for general bonding accesses
		// do we consider enabling encryption.
		if(iBaseband->SimplePairingMode() == EPhySimplePairingEnabled && !iNoSecurityRequired)
			{
			// If the link is SSP capable then for security mode 4 we must encrypt the link
			// for general bonding requests.
			encryptionRequired = ETrue;
			}
		else if(iBaseband->Authenticated() && iBaseband->IsEncryptionSupported())
			{
			// If the link is authenticated, for security mode 4 we might as well
			// encrypt it (assuming that it is supported).
			encryptionRequired = ETrue;
			}
		}

	// Ensure that if encryption is required then the state machine has already authenticated the link.
	__ASSERT_DEBUG(!encryptionRequired || iBaseband->Authenticated(), PANIC(KBTSecPanic, EBTSecEncryptionRequiredOnUnauthenticatedLink));

	return encryptionRequired;
	}

TBool CBTAccessRequester::RemoteIndicatedNoBondingToDedicatedBonding() const
	{
	LOG_FUNC
	return (iAccessType == EDedicatedBonding && iRemoteIndicatedNoBonding);
	}

TAccessType CBTAccessRequester::AccessType() const
	{
	return iAccessType;
	}

TBool CBTAccessRequester::AuthenticationInProgress() const
	{
	LOG_FUNC
	return iAuthenticationInProgress;
	}

void  CBTAccessRequester::SetAuthenticationInProgress()
	{
	LOG_FUNC
	iAuthenticationInProgress = ETrue;
	}
	
TBool CBTAccessRequester::BasebandConnected() const
	{
	LOG_FUNC
	return iBaseband && iBaseband->IsConnected();
	}

TBool CBTAccessRequester::LinkKeyGoodEnough() const
	{
	LOG_FUNC
	if(!iBaseband->RemoteDevice().IsValidLinkKey())
		{
		// If there is no valid link key then it isn't good enough.
		return EFalse;
		}
	
	switch(iBaseband->RemoteDevice().LinkKeyType())
		{
	case ELinkKeyAuthenticated:
		// An authenticated link key is the best we can do, so it is always good enough.
		LOG(_L8("\tLink key is authenticated"))
		return ETrue;
		
	case ELinkKeyUnauthenticatedUpgradable:
		// If the link key is unauthenticated but upgradable then, only in the cases where
		// MITM is not required is it good enough since there is the potential to be stronger. 
		if(iRequirements.MitmProtection() == EMitmNotRequired)
			{
			LOG(_L8("\tLink key is unauthenticated(upgradable) and is sufficient"));
			return ETrue;
			}
		else if(iSecMan.ConnectionsManager().IsAcceptPairedOnlyMode() && !iBaseband->Authenticated())
			{
			LOG(_L8("\tLink key is unauthenticated(upgradable) and in paired only connections"));
			// we don't want to pair if not yet authenticated in paired only connection mode...
			return ETrue;
			}
		else
			{
			LOG(_L8("\tLink key is unauthenticated but can be upgraded"));
			return EFalse;
			}
		
	case ELinkKeyUnauthenticatedNonUpgradable:
		// If the link key is unauthenticated but not upgradable then it is good enough so longer
		// as MITM protection is not required.  Unless OOB data has been received for that device
		// in which case we can attempt to upgrade through supplied data.
		if (iBaseband->HasRemoteOobData() && iRequirements.MitmProtection() != EMitmNotRequired)
			{
			LOG(_L8("\tLink key is not upgradable but OOB data is available"));
			return EFalse;
			}
		else if (iRequirements.MitmProtection() != EMitmRequired)
			{
			LOG(_L8("\tLink key is unauthenticated(non-upgradable) and is sufficient"));
			return ETrue;
			}
		else
			{
			LOG(_L8("\tLink key is unauthenticated but not sufficient"));
			return EFalse;
			}
		
	case ELinkKeyCombination:
		// A combination key not suitable if the remote device is now SSP capable.  Also we
		// determine if the key is sufficiently strong based on the length of the PIN code used.
		if(iBaseband->SimplePairingMode() == EPhySimplePairingEnabled)
			{
			LOG(_L8("\tCombination key is not good enough for an SSP enabled device!"));
			return EFalse;
			}
		else
			{
			LOG(_L("\tChecking min passkey length...")) 
			TBTPinCode passKey = iBaseband->PassKey();
			if(passKey().iLength >= iRequirements.PasskeyMinLength())
				{
				LOG(_L8("\tPIN code is of a sufficient length"));
				return ETrue;
				}
			else
				{
				LOG(_L8("\tPIN code length is not long enough!"))
				return EFalse;
				}
			}
		
	case ELinkKeyDebug:
		// Debug keys are used during development of Bluetooth services, as such they are
		// only appropriate for use if the device is in a debugging mode.
		if(iSecMan.DebugMode())
			{
			LOG(_L8("\tDebug Key is acceptable for use in debug mode"));
			return ETrue;
			}
		else
			{
			LOG(_L8("\tDebug Key being used when not in debug mode!"));
			return EFalse;
			}
		}
	LOG1(_L8("!!! Unknown link key type (%d)"), iBaseband->RemoteDevice().LinkKeyType());
	DEBUG_PANIC_LINENUM
	return EFalse; 
	}

void CBTAccessRequester::SetCurrentState(TBTAccessRequesterState aState)
	{
	LOG_FUNC
	iCurrentState = aState;
	}

TBool CBTAccessRequester::RequirementsDenied()
	{
	LOG_FUNC
	return iRequirements.Denied();
	}
	
void CBTAccessRequester::AuthenticationTimerElapsed()
	{
	LOG_FUNC
	LOG(_L8("\tTimer has elapsed during an authentication request... Complete(ACCESS DENIED)"));
	CompleteRequest(EBTSecManAccessDenied);
	}


//
// CBTAccessRequester::CAuthenticationTimer
//
	
CBTAccessRequester::CAuthenticationTimer* CBTAccessRequester::CAuthenticationTimer::NewL(CBTAccessRequester& aObserver)
	{
	CAuthenticationTimer* self = new(ELeave) CAuthenticationTimer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBTAccessRequester::CAuthenticationTimer::~CAuthenticationTimer()
	{
	LOG_FUNC
	Cancel();
	}
	
CBTAccessRequester::CAuthenticationTimer::CAuthenticationTimer(CBTAccessRequester& aObserver)
	: CTimer(CActive::EPriorityIdle)
	, iObserver(aObserver)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CBTAccessRequester::CAuthenticationTimer::ConstructL()
	{
	LOG_FUNC
	CTimer::ConstructL();
	}
	
void CBTAccessRequester::CAuthenticationTimer::Start()
	{	
	LOG_FUNC
	Cancel();
	After(KAuthenticationRequestTimeout);
	}

void CBTAccessRequester::CAuthenticationTimer::RunL()
	{
	LOG_FUNC
	iObserver.AuthenticationTimerElapsed();
	}



