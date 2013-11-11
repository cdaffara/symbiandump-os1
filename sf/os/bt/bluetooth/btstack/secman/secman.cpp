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

#include <bluetooth/hci/hciutil.h>

#include "secman.h"
#include "secevent.h"
#include "hostresolver.h"
#include "pairingserver.h"
#include "oobdata.h"
#include "simplepairingresult.h"
#include "btaccessrequesterstatemachine.h"

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
PANICCATEGORY("secman");
#endif

static const TInt KBTSecManAccessRequesterArrayGranularity = 4;
static const TInt KBTSecManNotifierRequesterArrayGranularity = 4;


//------------------------------------------------------------------------//
//class CBTSecMan
//------------------------------------------------------------------------//

CBTSecMan* CBTSecMan::NewL()
	{
	LOG_STATIC_FUNC
	CBTSecMan* self = CBTSecMan::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CBTSecMan* CBTSecMan::NewLC()
	{
	LOG_STATIC_FUNC
	CBTSecMan* self = new(ELeave) CBTSecMan();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CBTSecMan::CBTSecMan()
	: iAccessRequesters(KBTSecManAccessRequesterArrayGranularity)
	, iPendingAccessRequesters(KBTSecManAccessRequesterArrayGranularity)
	, iNotifierRequesters(KBTSecManNotifierRequesterArrayGranularity)
	, iLocalSimplePairingMode(EFalse)
	, iDebugMode (EFalse)
	{
	LOG_FUNC
	}

void CBTSecMan::ConstructL()
	{
	LOG_FUNC
	iStateMachine = CBTAccessRequesterStateFactory::NewL();
	iCommandController = CSecManCommandController::NewL(*this);
	
	iOobDataManager = COobDataManager::NewL(*this);
	iSimplePairingResultList = CSimplePairingResultList::NewL();
	iAuthenticationResultList = CAuthenticationResultList::NewL();
	iPairingServer = CPairingServer::NewL(*iOobDataManager, *iSimplePairingResultList, *iAuthenticationResultList);
	}

void CBTSecMan::SetLocalSimplePairingMode(TBool aEnabled)
	{
	LOG_FUNC
	iLocalSimplePairingMode=aEnabled;
	}

TBool CBTSecMan::LocalSimplePairingMode() const
	{
	LOG_FUNC
	return iLocalSimplePairingMode;
	}

void CBTSecMan::SimplePairingSupportDetermined(const TBTDevAddr& aBDAddr)
	{
	LOG_FUNC

	// Pending requesters must be notified first than the active requester. 
	// Otherwise it wouldn't work properly and pending requesters would not be notified at all.
	for (TInt i=0; i<iPendingAccessRequesters.Count(); i++)
		{
		CBTAccessRequester* requester = iPendingAccessRequesters[i];
		if (requester->DeviceAddress() == aBDAddr && requester->BasebandConnected())
			{
			TBTSecEvent event(TBTSecEvent::EPhysicalLinkUp);
			requester->SendEvent(event);
			}	
		}

	CBTAccessRequester* requester = this->FindActiveAccessRequester(aBDAddr);
	// There should only be one active access requester
	if (requester && requester->BasebandConnected())
		{
		TBTSecEvent event(TBTSecEvent::EPhysicalLinkUp);
		requester->SendEvent(event);
		}
	}

void CBTSecMan::SetPhysicalLinksMgr(const CPhysicalLinksManager& aConnectionsMgr)
	{
	LOG_FUNC
	iPhysicalLinksManager = &const_cast<CPhysicalLinksManager&>(aConnectionsMgr);
	iPairingServer->SetPhysicalLinksManager(*iPhysicalLinksManager);
	}

void CBTSecMan::ClearPhysicalLinksMgr()
	{
	LOG_FUNC
	iPhysicalLinksManager = NULL;
	iPairingServer->ClearPhysicalLinkMgr();
	}

// Passes a reference to the LinksMgrProtocol ultimately to the PairingServer 
void CBTSecMan::SetLinksMgrProtocol(CLinkMgrProtocol& aLinkMgrProtocol)
	{
	LOG_FUNC
	iPairingServer->SetLinksMgrProtocol(aLinkMgrProtocol);
	}

void CBTSecMan::ClearLinksMgrProtocol()
	{
	LOG_FUNC
	iPairingServer->ClearLinksMgrProtocol();
	}

void CBTSecMan::SetHCICommandQueue(MHCICommandQueue& aCommandQueue)
	{
	LOG_FUNC
	iCommandController->SetHCICommandQueue(aCommandQueue);
	iOobDataManager->SetHCICommandQueue(aCommandQueue);
	}

void CBTSecMan::ClearHCICommandQueue()
	{
	LOG_FUNC
	iCommandController->ClearHCICommandQueue();
	iOobDataManager->ClearHCICommandQueue();
	}

CPhysicalLinksManager& CBTSecMan::ConnectionsManager() const
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iPhysicalLinksManager, PANIC(KBTSecPanic,EBTSecNullPhysicalLinksManager));
	return *iPhysicalLinksManager;
	}

COobDataManager& CBTSecMan::OobDataManager() const
	{
	LOG_FUNC
	return *iOobDataManager;
	}

TBool CBTSecMan::DebugMode() const
	{
	LOG_FUNC
	return iDebugMode;
	}

void CBTSecMan::SetDebugMode(TBool aOn)
	{
	LOG_FUNC
	// SecMan owns the policy about simple pairing debug mode - here we only allow it to be 
	// turned on.  So ignore any "off" requests.  (Stack must be unloaded for debug mode to be
	// turned off).
	if(aOn && !iDebugMode)
		{
		// First try to set debug mode. If we fail to send the command then we will just be
		// stuck in non-debug mode...
		static const TUint8 KSimplePairingDebugModeEnabled = 0x01;
		TRAP_IGNORE(iCommandController->WriteSimplePairingDebugModeL(KSimplePairingDebugModeEnabled));
		}
	}

void CBTSecMan::ClearDebugMode()
	{
	LOG_FUNC
	// Used to clear debug mode when the stack is reset
	iDebugMode = EFalse;
	ConnectionsManager().SimplePairingDebugModeChanged(EFalse);
	}

void CBTSecMan::DebugModeChanged(TBool aOn)
	{
	__ASSERT_DEBUG(aOn, PANIC(KBTSecPanic, EBTSecDebugModeTurnedOff));
	iDebugMode = aOn;
	ConnectionsManager().SimplePairingDebugModeChanged(aOn);
	}

CBTSecMan::~CBTSecMan()
	{
	LOG_FUNC
	
	iAccessRequesters.ResetAndDestroy();
	iAccessRequesters.Close();

	iPendingAccessRequesters.ResetAndDestroy();
	iPendingAccessRequesters.Close();
	
	iNotifierRequesters.ResetAndDestroy();
	iNotifierRequesters.Close();
	
	delete iStateMachine;
	delete iCommandController;
	delete iPairingServer;
	delete iOobDataManager;
	delete iSimplePairingResultList;
	delete iAuthenticationResultList;
	}

CBTAccessRequester* CBTSecMan::FindActiveAccessRequester(const TBTDevAddr& aAddr) const
	{
	LOG_FUNC
	CBTAccessRequester* ret = NULL;
	TInt ix = iAccessRequesters.Find(aAddr, CompareAccessRequesterByBDAddr);
	if(ix >= 0)
		{
		ret = iAccessRequesters[ix];
		}
	return ret;
	}

TBool CBTSecMan::CompareAccessRequesterByBDAddr(const TBTDevAddr* aKey, const CBTAccessRequester& aAccessRequester)
	{
	LOG_STATIC_FUNC
	return (!aKey || (*aKey) == aAccessRequester.DeviceAddress());
	}

TBool CBTSecMan::CompareAccessRequesterByRequester(const MAccessRequestResponseHandler* aKey, const CBTAccessRequester& aAccessRequester)
	{
	LOG_STATIC_FUNC
	return aKey == &aAccessRequester.ServiceRequester();
	}

void CBTSecMan::AccessRequestL(const TBTServiceSecurity& aSecurity,
							   const TBTServiceSecurityPerDevice* const aOverride,
							   const TBTDevAddr& aBDAddr,
							   TAccessType aAccessType,
							   TBool aNoSecurityRequired,
							   MAccessRequestResponseHandler& aRequester)
/**
Handle an access request...
Create a new CBTAccessRequester object to handle the request.
**/
	{
	LOG_FUNC

#ifdef _DEBUG
	// Check that a service isn't queuing multiple access requesters.
	TInt ix = iAccessRequesters.Find(aRequester, CompareAccessRequesterByRequester);
	__ASSERT_DEBUG(ix == KErrNotFound, PANIC(KBTSecPanic, EBTSecServiceTryingToQueueMultipleAccessRequesters));
	ix = iPendingAccessRequesters.Find(aRequester, CompareAccessRequesterByRequester);
	__ASSERT_DEBUG(ix == KErrNotFound, PANIC(KBTSecPanic, EBTSecServiceTryingToQueueMultipleAccessRequesters));
#endif // _DEBUG

	// find the baseband this SAP is running on
	CPhysicalLink& con = *iPhysicalLinksManager->FindPhysicalLink(aBDAddr);
	CBTAccessRequester* p = CBTAccessRequester::NewLC(con, aSecurity,
													  aOverride,
													  aRequester,
													  aAccessType,
													  aNoSecurityRequired,
													  *this);
	
	CBTAccessRequester* requester = FindActiveAccessRequester(aBDAddr);
	if(requester)
		{
		User::LeaveIfError(iPendingAccessRequesters.Append(p));
		CleanupStack::Pop(p); //clean up of p now handled by iPendingAccessRequesters
		}
	else
		{
		User::LeaveIfError(iAccessRequesters.Append(p));
		CleanupStack::Pop(p); //clean up of p now handled by iAccessRequesters
		// Try to start- it may not happen (depends on if device retrieved from registry)
		p->Start();
		}
	
	}

void CBTSecMan::CancelRequest(MAccessRequestResponseHandler& aRequester)
	{
	LOG_FUNC
	// search through access requesters to find correct one
	LOG1(_L8("\tCBTSecMan::CancelRequest from SAP 0x%08x"), &aRequester)

	CBTAccessRequester* request = NULL;
	
	TInt ix = iAccessRequesters.Find(aRequester, CompareAccessRequesterByRequester);
	if(ix >= 0)
		{
		request = iAccessRequesters[ix];
		}
	else
		{
		ix = iPendingAccessRequesters.Find(aRequester, CompareAccessRequesterByRequester);
		if(ix >= 0)
			{
			request = iPendingAccessRequesters[ix];
			}
		}
	
	if(request)
		{
		static_cast<void>(FinishAccessRequest(request)); // don't need to call back to requester...
		}
	
#ifdef _DEBUG
	// Do a sanity test to check that there aren't multiple access requesters per service
	ix = iAccessRequesters.Find(aRequester, CompareAccessRequesterByRequester);
	__ASSERT_DEBUG(ix == KErrNotFound, PANIC(KBTSecPanic, EBTSecMultipleActiveAccessRequestersForService));
	ix = iPendingAccessRequesters.Find(aRequester, CompareAccessRequesterByRequester);
	__ASSERT_DEBUG(ix == KErrNotFound, PANIC(KBTSecPanic, EBTSecMultiplePendingAccessRequestersForService));
#endif // _DEBUG
	}

void CBTSecMan::GetPassKeyLengthAndOriginator(const TBTDevAddr& aAddr, TUint& aPasskeyMinLength,
											  TBool& aLocallyInitiatedAuthentication,
											  TBool& aStrongKeyRequired)

/**
If authorisation request was initiated locally it will return true and will
return the passkey minimal length requred by user
**/
	{
	LOG_FUNC
	TUint tmpPasskeyLength = 0;
	TBluetoothMitmProtection mitmReqs = EMitmNotRequired;
	TBool locallyInitiated = EFalse;
	TInt count = iAccessRequesters.Count();
	aStrongKeyRequired = EFalse;

	// find all pending AccessRequesters for given BTAddr and find maximum of PasskeyMinLength
	if(count != 0)
		{
		for (TInt i=0; i<count;i++)
			{
			CBTAccessRequester* requester = iAccessRequesters[i];
			if (requester->IsAuthenticationReqPending(aAddr, tmpPasskeyLength, mitmReqs))
				{
				locallyInitiated = ETrue;
				if (aPasskeyMinLength < tmpPasskeyLength)
					{
					aPasskeyMinLength = tmpPasskeyLength;
					}
				if(mitmReqs == EMitmRequired)
					{
					aStrongKeyRequired = ETrue;
					}
				}
			}
		}
	aLocallyInitiatedAuthentication = locallyInitiated;
	}



TBool CBTSecMan::IsDedicatedBondingAttempted(const TBTDevAddr& aAddr)
	{
	LOG_FUNC

	CBTAccessRequester* requester = FindActiveAccessRequester(aAddr);

	if(requester && (requester->AccessType() == EDedicatedBonding) )
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

TBool CBTSecMan::IsOutboundAccessRequest(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	// For now the only outbound access request is for dedicated bonding
	// attempts (so we share the code).
	return IsDedicatedBondingAttempted(aAddr);
	}


void CBTSecMan::AuthenticationInProgress()
/**
When authentication request was sent to HW, HCI will notify SecMan
**/
	{
	LOG_FUNC
	// find first pending AccessRequesters and set AuthenticationInProgress flag
	for (TInt i=0; i<iAccessRequesters.Count(); i++)
		{
		CBTAccessRequester* requester = iAccessRequesters[i];

		if (requester->AuthenticationRequired() && !requester->AuthenticationInProgress())
			{
			TBTSecEvent event(TBTSecEvent::EAuthenticationRequested);
			requester->SendEvent(event);
			break;
			}
		}
	}

void CBTSecMan::IOCapabilityRequestFromRemote(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	CBTAccessRequester* requester = FindActiveAccessRequester(aAddr);
	if(requester)
		{
		LOG(_L8("\tCBTAccessRequester FOUND!\n"));
		TBTSecEvent event(TBTSecEvent::EIOCapsRequested);
		requester->SendEvent(event);
		}
	else
		{
		LOG(_L8("\tCBTAccessRequester NOT FOUND!\n"));
		CPhysicalLink* link = iPhysicalLinksManager->FindPhysicalLink(aAddr);
		__ASSERT_ALWAYS(link, PANIC(KBTSecPanic, EBTSecPhysicalLinkMissing)); // If we've got an IO Capability Response we should have a link

		if(link->IsPairable())
			{
			link->SetAuthenticationPending(ESimplePairingPending);
			THCIOobDataPresence oobPresence = EOOBDataNotPresent;
			if (link->HasRemoteOobData())
				{
				oobPresence = EOOBDataPresent;
				}
			THCIAuthenticationRequirement authReq = link->AuthenticationRequirement();
			switch(authReq)
				{
				case EMitmNotReqNoBonding:
				case EMitmNotReqDedicatedBonding:
				case EMitmNotReqGeneralBonding:
					link->SetLocalMITM(EFalse);
					break;
				case EMitmReqNoBonding:
				case EMitmReqDedicatedBonding:
				case EMitmReqGeneralBonding:
					link->SetLocalMITM(ETrue);
					break;
				default:
					PANIC(KBTSecPanic, EBTSecUnexpectedIoCapability);
					break;    
				}
			TRAP_IGNORE(iCommandController->IOCapabilityRequestReplyL(aAddr, EIOCapsDisplayYesNo, oobPresence, authReq));
			}
		else
			{
			TRAP_IGNORE(iCommandController->IOCapabilityRequestNegativeReplyL(aAddr, EPairingNotAllowed));
			}
		}
	}


void CBTSecMan::IOCapabilityAskForResponse(const TBTDevAddr& aAddr,
										THCIIoCapability aIOCapability,
										THCIOobDataPresence aOOBDataPresent,
										THCIAuthenticationRequirement aAuthenticationRequirements)
	{
	LOG_FUNC
	
	CPhysicalLink* link = iPhysicalLinksManager->FindPhysicalLink(aAddr);
	__ASSERT_ALWAYS(link, PANIC(KBTSecPanic, EBTSecPhysicalLinkMissing)); // If we've got an IO Capability Response we should have a link
	link->IOCapabilityAskForResponse(aIOCapability, aOOBDataPresent, aAuthenticationRequirements);
	
	CBTAccessRequester* requester = FindActiveAccessRequester(aAddr);
	if(requester)
		{
		LOG(_L8("\tCBTAccessRequester FOUND!\n"));
		TBTSecEventIoCapabilityResponse event(aIOCapability, aOOBDataPresent, aAuthenticationRequirements);
		requester->SendEvent(event);
		}
	}

void CBTSecMan::RemoteOOBDataRequest(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	CBTAccessRequester* requester = FindActiveAccessRequester(aAddr);
	if(requester)
		{
		LOG(_L8("\tCBTAccessRequester FOUND!\n"));
		TBTSecEvent event(TBTSecEvent::ERemoteOOBDataRequested);
		requester->SendEvent(event);
		}

	TBluetoothSimplePairingHash hashC;
	TBluetoothSimplePairingRandomizer randomizerR;
	if(OobDataManager().GetRemoteOobData(aAddr, hashC, randomizerR))
		{
		TRAP_IGNORE(iCommandController->RemoteOOBDataRequestReplyL(aAddr, hashC, randomizerR));
		}
	else
		{
		TRAP_IGNORE(iCommandController->RemoteOOBDataRequestNegativeReplyL(aAddr));
		}
	}

void CBTSecMan::RemoteOOBDataRequestComplete(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	CBTAccessRequester* requester = FindActiveAccessRequester(aAddr);
	if(requester)
		{
		LOG(_L8("\tCBTAccessRequester FOUND!\n"));
		TBTSecEvent event(TBTSecEvent::ERemoteOOBDataRequestComplete);
		requester->SendEvent(event);
		}
	}

void CBTSecMan::UserConfirmationRequest(const TBTDevAddr& aAddr, TUint32 aNumericValue)
	{
	LOG_FUNC
	CBTAccessRequester* requester = FindActiveAccessRequester(aAddr);
	if(requester)
		{
		LOG(_L8("\tCBTAccessRequester FOUND!\n"));
		TBTSecEventUserConfirmationRequest event(aNumericValue);
		requester->SendEvent(event);
		}

	if(requester != FindActiveAccessRequester(aAddr))
		{
		// The previous requester has been completed so move on.
		TRAP_IGNORE(iCommandController->UserConfirmationRequestNegativeReplyL(aAddr));
		return; // No passkey or comparison dialogs; disconnect instead
		}

	CPhysicalLink* link = iPhysicalLinksManager->FindPhysicalLink(aAddr);
	__ASSERT_ALWAYS(link, PANIC(KBTSecPanic, EBTSecPhysicalLinkMissing));
	__ASSERT_DEBUG(!link->InstanceNumericComparator(), PANIC(KBTSecPanic, EBTSecConnectionNumericComparisonTwice));
	__ASSERT_DEBUG(!link->InstanceUserConfirmer(), PANIC(KBTSecPanic, EBTSecConnectionUserConfirmationTwice));
	if(link->InstanceNumericComparator() || link->InstanceUserConfirmer())
		{
		return;
		}

	if(link->AuthWithMITM())
		{
		TBool locallyInitiated; //Authentication
		TBool strongKeyRequired;
		TUint minPasskeyLength=0;

		GetPassKeyLengthAndOriginator(aAddr, minPasskeyLength, locallyInitiated, strongKeyRequired);
		TRAPD(err,link->NewNumericComparatorL(aAddr, *this, aNumericValue, locallyInitiated));
		if(err)
			{
			if(requester)
				{
				requester->CompleteRequest(EBTSecManAccessDenied);
				}
			else
				{
				TRAP_IGNORE(iCommandController->UserConfirmationRequestNegativeReplyL(aAddr));
				return;// No passkey or comparison dialogs; disconnect instead
				}
			}
		}
	else if (!link->IsPairingExpected()
			|| ((link->AuthenticationRequirement() == EMitmNotReqDedicatedBonding 
					|| link->AuthenticationRequirement() == EMitmReqDedicatedBonding)
				&& !link->IsAuthenticationRequestPending()))
		{
		TRAPD(err,link->NewUserConfirmerL(aAddr, *this, ETrue));
		if(err)
			{
			if(requester)
				{
				requester->CompleteRequest(EBTSecManAccessDenied);
				}
			else
				{
				TRAP_IGNORE(iCommandController->UserConfirmationRequestNegativeReplyL(aAddr));
				return;// No passkey or comparison dialogs; disconnect instead
				}
			}
		}
	else
		{
		// Just Work...
		if(requester)
			{
			LOG(_L8("\tCBTAccessRequester FOUND!\n"));
			TBTSecEventUserConfirmationComplete event(ETrue);
			requester->SendEvent(event);
			}
		link->PinRequestSent();
		// note: -- check errors here
		TRAP_IGNORE(iCommandController->UserConfirmationRequestReplyL(aAddr));
		}
	}

void CBTSecMan::NumericComparisonComplete(const TBTDevAddr& aAddr, TBool aResult, TInt aError)
	{
	LOG_FUNC
	CBTAccessRequester* requester = FindActiveAccessRequester(aAddr);
	if (requester)
		{
		LOG(_L8("\tCBTAccessRequester FOUND!\n"));
		if (aError == KErrNone)
			{
			TBTSecEventUserConfirmationComplete event(aResult);
			requester->SendEvent(event);
			}
		else
			{
			TBTSecEventUserConfirmationComplete event(EFalse);  // Failed, so send EFalse
			requester->SendEvent(event);
			}
		}

	CPhysicalLink* link = iPhysicalLinksManager->FindPhysicalLink(aAddr);
	__ASSERT_ALWAYS(link, PANIC(KBTSecPanic, EBTSecPhysicalLinkMissing));

	if (aError!=KErrNone)
		{
		// there was an error somewhere a long the way so respond negatively
		// note: -- check errors here
		TRAP_IGNORE(iCommandController->UserConfirmationRequestNegativeReplyL(aAddr));
		}
	else
		{
		// got a result
		if(aResult)
			{
			link->PinRequestSent();
			// note: -- check errors here
			TRAP_IGNORE(iCommandController->UserConfirmationRequestReplyL(aAddr));
			}
		else
			{
			// note: -- check errors here
			TRAP_IGNORE(iCommandController->UserConfirmationRequestNegativeReplyL(aAddr));
			}
		}
	link->DeleteNumericComparator();
	}

void CBTSecMan::UserConfirmationComplete(const TBTDevAddr& aAddr, TBool aResult, TInt aError)
	{
	LOG_FUNC
	CBTAccessRequester* requester = FindActiveAccessRequester(aAddr);
	if (requester)
		{
		LOG(_L8("\tCBTAccessRequester FOUND!\n"));
		if (aError == KErrNone)
			{
			TBTSecEventUserConfirmationComplete event(aResult);
			requester->SendEvent(event);
			}
		else if (aError == KErrNotFound) // KErrNotFound -> Notifier isn't present, so allow anyway
			{
			TBTSecEventUserConfirmationComplete event(ETrue);
			requester->SendEvent(event);
			}
		else
			{
			TBTSecEventUserConfirmationComplete event(EFalse);  // Failed, so send EFalse
			requester->SendEvent(event);
			}
		}

	CPhysicalLink* link = iPhysicalLinksManager->FindPhysicalLink(aAddr);
	__ASSERT_ALWAYS(link, PANIC(KBTSecPanic, EBTSecPhysicalLinkMissing));

	if (aError==KErrNotFound) // KErrNotFound -> Notifier isn't present, so allow anyway
		{
		link->PinRequestSent();
		// note: -- check errors here
		TRAP_IGNORE(iCommandController->UserConfirmationRequestReplyL(aAddr));
		}
	else if (aError!=KErrNone)
		{
		// there was an error somewhere a long the way so respond negatively
		// note: -- check errors here
		TRAP_IGNORE(iCommandController->UserConfirmationRequestNegativeReplyL(aAddr));
		}
	else
		{
		// got a result
		if(aResult)
			{
			link->PinRequestSent();
			// note: -- check errors here
			TRAP_IGNORE(iCommandController->UserConfirmationRequestReplyL(aAddr));
			}
		else
			{
			// note: -- check errors here
			TRAP_IGNORE(iCommandController->UserConfirmationRequestNegativeReplyL(aAddr));
			}
		}
	link->DeleteUserConfirmer();
	}

void CBTSecMan::PasskeyNotification(const TBTDevAddr& aAddr, TUint32 aPasskey)
	{
	LOG_FUNC
	CBTAccessRequester* requester = FindActiveAccessRequester(aAddr);
	if (requester)
		{
		LOG(_L8("\tCBTAccessRequester FOUND!\n"));
		TBTSecEventPasskeyNotification event(aPasskey);
		requester->SendEvent(event);
		}

	if(requester != FindActiveAccessRequester(aAddr))
		{
		// the previous requester has been completed so move on.
		return;
		}

	CPhysicalLink* link = iPhysicalLinksManager->FindPhysicalLink(aAddr);
	__ASSERT_ALWAYS(link, PANIC(KBTSecPanic, EBTSecPhysicalLinkMissing));

	__ASSERT_DEBUG(!link->InstancePasskeyEntry(), PANIC(KBTSecPanic, EBTSecConnectionPasskeyNotificationTwice));

	if(link->InstancePasskeyEntry())
		{
		return;
		}

	TBool locallyInitiated; //Authentication
	TBool strongKeyRequired;
	TUint minPasskeyLength=0;

	GetPassKeyLengthAndOriginator(aAddr, minPasskeyLength, locallyInitiated, strongKeyRequired);
	TRAPD(err, link->NewPasskeyEntryL(aAddr, *this,aPasskey, locallyInitiated));
	if(err != KErrNone)
		{
		if(requester)
			{
			requester->CompleteRequest(EBTSecManAccessDenied);
			}
		}
	}

void CBTSecMan::KeypressNotification(const TBTDevAddr& aAddr, TUint8 aNotificationType)
	{
	LOG_FUNC
	CBTAccessRequester* requester = FindActiveAccessRequester(aAddr);
	if (requester)
		{
		LOG(_L8("\tCBTAccessRequester FOUND!\n"));
		TBTSecEventKeypressEntry event(aNotificationType);
		requester->SendEvent(event);
		}

	CPhysicalLink* link = iPhysicalLinksManager->FindPhysicalLink(aAddr);
	__ASSERT_ALWAYS(link, PANIC(KBTSecPanic, EBTSecPhysicalLinkMissing));

	if (link->InstancePasskeyEntry())
		{
		THCIPasskeyEntryNotificationType keyType = static_cast<THCIPasskeyEntryNotificationType>(aNotificationType);
		link->PasskeyEntryKeyPressed(keyType);
		}
	}

void CBTSecMan::PasskeyNotificationComplete(const TBTDevAddr& aAddr, TInt aError)
	{
	LOG_FUNC
	CBTAccessRequester* requester = FindActiveAccessRequester(aAddr);
	if (requester)
		{
		LOG(_L8("\tCBTAccessRequester FOUND!\n"));
		TBTSecEvent event(TBTSecEvent::EKeypressComplete, aError);
		requester->SendEvent(event);
		}

	CPhysicalLink* link = iPhysicalLinksManager->FindPhysicalLink(aAddr);
	__ASSERT_ALWAYS(link, PANIC(KBTSecPanic, EBTSecPhysicalLinkMissing));
	link->DeletePasskeyEntry();
	}

void CBTSecMan::AccessRequestComplete(CBTAccessRequester* aAccessRequester, TInt aResult)
/**
The access request has been fully completed.
Delete the CBTAccessRequester that was handling the request.
**/
	{
	LOG_FUNC
	MAccessRequestResponseHandler& service = FinishAccessRequest(aAccessRequester);
	// now tell the service
	service.AccessRequestComplete(aResult);
	}
	
MAccessRequestResponseHandler& CBTSecMan::FinishAccessRequest(CBTAccessRequester* aAccessRequester)
	{
	LOG_FUNC
	TBTDevAddr addr = aAccessRequester->DeviceAddress();

	// find the originating service *now*
	MAccessRequestResponseHandler& service = const_cast<MAccessRequestResponseHandler&>
											(aAccessRequester->ServiceRequester());

	CBTAccessRequester* newRequester = NULL;
	// Determine if this is an active access requester.
	TInt ix = iAccessRequesters.Find(aAccessRequester);
	if(ix >= 0)
		{
		// We need to replace this requester with a new one.
		// Find the new access requester
		ix = iPendingAccessRequesters.Find(addr, CompareAccessRequesterByBDAddr);
		if(ix >= 0)
			{
			newRequester = iPendingAccessRequesters[ix];
			iPendingAccessRequesters.Remove(ix);
			iPendingAccessRequesters.GranularCompress();
			}
		// Remove bindings for the completed access requester (and substitute with new one if neccessary)
		ix = iAccessRequesters.Find(aAccessRequester);
		__ASSERT_DEBUG(ix >= 0, PANIC(KBTSecPanic, EBTSecAccessRequesterCompletedWhenNotActive));
		if(newRequester)
			{
			iAccessRequesters[ix] = newRequester;
			}
		else
			{
			iAccessRequesters.Remove(ix);
			}
		}
	else
		{
		// This must be a pending requester - in which case we just complete it.
		__ASSERT_DEBUG(ix == KErrNotFound, PANIC(KBTSecPanic, EBTSecAccessRequesterShouldHaveNotBeenFound));
		ix = iPendingAccessRequesters.Find(aAccessRequester);
		__ASSERT_DEBUG(ix >= 0, PANIC(KBTSecPanic, EBTSecAccessRequesterShouldHaveBeenFound));
		iPendingAccessRequesters.Remove(ix);
		iPendingAccessRequesters.GranularCompress();
		}

	// Cleanup the access requester
	delete aAccessRequester, aAccessRequester = NULL;

	// Start the new requester if there is one.
	if(newRequester)
		{
		newRequester->Start();
		}
	
	return service;
	}

void CBTSecMan::SimplePairingComplete(const TBTDevAddr& aAddr, THCIErrorCode aError)
	{
	LOG_FUNC
	TInt err = CHciUtil::SymbianErrorCode(aError);

	CPhysicalLink* link = iPhysicalLinksManager->FindPhysicalLink(aAddr);
	__ASSERT_ALWAYS(link, PANIC(KBTSecPanic, EBTSecPhysicalLinkMissing));

	if (link->InstancePasskeyEntry() && link->IsPasskeyEntryActive())
		{
		link->CancelPasskeyEntry();
		PasskeyNotificationComplete(aAddr, err);
		}
	if (link->InstanceNumericComparator() && link->IsNumericComparatorActive())
		{
		link->CancelNumericComparator();
		NumericComparisonComplete(aAddr, EFalse, err);
		}
	if (link->InstanceUserConfirmer() && link->IsUserConfirmerActive())
		{
		link->CancelUserConfirmer();
		UserConfirmationComplete(aAddr, EFalse, err);
		}
	iPhysicalLinksManager->SimplePairingComplete(aAddr, aError);

 	// Add result to list (always with HCI error code).
 	// Pairing results will be added only if pairing that involved numeric Comparison or passkey entry.
 	// It should not be added if the pairing completed using Just Works
 	if(link->AuthWithMITM())
 		{
 		iSimplePairingResultList->NewResult(aAddr, (KHCIErrorBase - aError));
 		}
	}

void CBTSecMan::AuthenticationComplete(const TBTDevAddr& aAddr, THCIErrorCode aError)
	{
	LOG_FUNC
	iAuthenticationResultList->NewResult(aAddr, (KHCIErrorBase - aError));
	}

void CBTSecMan::AddNotifierRequestToQueL(CSecNotifierRequester& aRequest)
/**
Add notifier request to front of queue.  If there are no other requests already in the queue,
initiate this request.
**/
	{
	LOG_FUNC
	TInt count = iNotifierRequesters.Count();
	User::LeaveIfError(iNotifierRequesters.Insert(&aRequest,0));	//add to front of queue since requests are taken from the back
	if(count == 0)	// ok since count was calculated before we inserted the new element
		{
		iActiveNotifierRequester = &aRequest;
		aRequest.DoRequest();
		}
	else
		{
		LOG(_L8("\tSecman: Request queued, should start later..."));
		}
	}

void CBTSecMan::RemoveNotifierRequestFromQue(CSecNotifierRequester& aRequest)
/**
Remove the request from the queue.  If aRequest is the currently active request then we can activate
the next one in the queue.  Otherwise, aRequest is being deleted prematurely and we must simply
remove it from the array.
**/
	{
	LOG_FUNC
	TInt count = iNotifierRequesters.Count();
	TInt found = 0;
	for (TInt i=(count-1); i>=0; i--)
		{
		if (iNotifierRequesters[i] == &aRequest)
			{
			found++;
			iNotifierRequesters.Remove(i);
			}
		}
	__ASSERT_DEBUG(found, PANIC(KBTSecPanic, EBTSecBadNotifierArray));

	if(&aRequest == iActiveNotifierRequester)
		{
		//start the next request if there is one...
		count = iNotifierRequesters.Count();
		if (count > 0)
			{
			LOG(_L8("\tCBTSecMan - auto-starting next notifier request from queue"));
			iActiveNotifierRequester = iNotifierRequesters[count-1];
			iActiveNotifierRequester->DoRequest();
			}
		else
			{
			iActiveNotifierRequester = NULL;
			}
		}
	}

CBTAccessRequesterStateFactory* CBTSecMan::StateMachine()
	{
	LOG_FUNC
	return iStateMachine;
	}

CSecNotifierRequester::CSecNotifierRequester(CBTSecMan& aSecMan)
	: CActive(EPriorityStandard)
	, iInquiryMgr(aSecMan.ConnectionsManager().LinkManagerProtocol().InquiryMgr())
	, iSecMgr(aSecMan)
	{
	LOG_FUNC
	}

void CSecNotifierRequester::ConstructL(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	LEAVEIFERRORL(iNotifier.Connect());

	// find the name at this stage for this device - may not be there yet
	iDeviceName = iInquiryMgr.DeviceNameFromCache(aAddr);

	if (!iDeviceName || iDeviceName->Length() ==0)
		{
		// cache didn't have name - so we'll ask for it as a HR action
		TRAP_IGNORE(iHR = iInquiryMgr.NewHostResolverL());
		iHRNameRecord = new TNameRecord;

		// ignore error - only an optimisation - don't want to leave if there's
		// a problem doing this optimisation
		if (iHR && iHRNameRecord)
			{
			iHR->SetNotify(this);

			TInquirySockAddr i;
			i.SetAction(KHostResName);
			i.SetBTAddr(aAddr);
			iHRNameRecord->iAddr = i;
			iHR->GetByAddress(*iHRNameRecord);
			}
		}

	iDevAddr = aAddr;

	iSecMgr.AddNotifierRequestToQueL(*this);
	iIsAddedToNotifierQue = ETrue;
	}

void CSecNotifierRequester::RemoveMyselfFromQue()
	{
	LOG_FUNC
	iSecMgr.RemoveNotifierRequestFromQue(*this);
	}

CSecNotifierRequester::~CSecNotifierRequester()
	{
	LOG_FUNC
	Cancel();

	//remove ourself from the notifier que if we're still on it.
	if (iIsAddedToNotifierQue)
		{
		RemoveMyselfFromQue();
		iIsAddedToNotifierQue = EFalse;
		}

	delete iHR;
	delete iHRNameRecord;

	iNotifier.Close();
	}

void CSecNotifierRequester::QueryComplete(TInt aErr)
	{
	LOG_FUNC
	if (aErr==KErrNone && iHRNameRecord)
		{
		// now have device name - update notifiers
		// we do have a copy of the name - but it is now wide :-|
		// and also we have iDeviceName that is still NULL, so best bet is
		// to just set our pointer and use the cache one (which we *know* is there!)
		TBTDevAddr a = TBTSockAddr::Cast(iHRNameRecord->iAddr).BTAddr();
		iDeviceName = iInquiryMgr.DeviceNameFromCache(a);
		DoUpdateNotifier();
		}

	delete iHRNameRecord;
	iHRNameRecord = NULL;
	}

void CSecNotifierRequester::HandleTimeout()
	{
	LOG_FUNC
	// A timer user should implement this function
	__ASSERT_DEBUG(EFalse, PANIC(KBTSecPanic, EBTSecNotifierRequesterUsingTimerWithoutHandling));
	}



CSecNotifierRequesterTimer* CSecNotifierRequesterTimer::NewL(CSecNotifierRequester& aObserver)
	{
	LOG_STATIC_FUNC
	CSecNotifierRequesterTimer* self = new(ELeave) CSecNotifierRequesterTimer(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSecNotifierRequesterTimer::CSecNotifierRequesterTimer(CSecNotifierRequester& aObserver)
	: CTimer(EPriorityStandard)
	, iObserver(aObserver)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

CSecNotifierRequesterTimer::~CSecNotifierRequesterTimer()
	{
	LOG_FUNC
	Cancel();
	}

void CSecNotifierRequesterTimer::Start(TTimeIntervalMicroSeconds32 aTimeout)
	{
	LOG_FUNC
	After(aTimeout);
	}

void CSecNotifierRequesterTimer::ConstructL()
	{
	LOG_FUNC
	CTimer::ConstructL();
	}

void CSecNotifierRequesterTimer::RunL()
	{
	LOG_FUNC
	iObserver.HandleTimeout();
	}


//------------------------------------------------------------------------//
//class CBTPinRequester
//------------------------------------------------------------------------//


CBTPinRequester* CBTPinRequester::NewL(const TBTDevAddr& aAddr,
									   MPINCodeResponseHandler& aRequester,
									   CBTSecMan& aSecMan,
									   TUint aPasskeyMinLength,
									   TBool aInternallyInitiated,
									   TBool aStrongKeyRequired,
									   TUint aRecommendedPasskeyMinLength)
	{
	LOG_STATIC_FUNC
	CBTPinRequester* s = CBTPinRequester::NewLC(aAddr, aRequester, aSecMan,
	                                            aPasskeyMinLength, aInternallyInitiated, aStrongKeyRequired, aRecommendedPasskeyMinLength);
	CleanupStack::Pop(s);
	return s;
	}

CBTPinRequester* CBTPinRequester::NewLC(const TBTDevAddr& aAddr,
										MPINCodeResponseHandler& aRequester,
										CBTSecMan& aSecMan,
										TUint aPasskeyMinLength,
										TBool aInternallyInitiated,
										TBool aStrongKeyRequired,
										TUint aRecommendedPasskeyMinLength)
	{
	LOG_STATIC_FUNC
	CBTPinRequester* s = new(ELeave) CBTPinRequester(aRequester, aSecMan,
	                                                 aPasskeyMinLength, aInternallyInitiated, aStrongKeyRequired, aRecommendedPasskeyMinLength);
	CleanupStack::PushL(s);
	s->ConstructL(aAddr);
	return s;
	}

CBTPinRequester::CBTPinRequester(MPINCodeResponseHandler& aHandler,
								 CBTSecMan& aSecMan,
								 TUint aPasskeyMinLength,
								 TBool aInternallyInitiated,
								 TBool aStrongKeyRequired,
								 TUint aRecommendedPasskeyMinLength)
	: CSecNotifierRequester(aSecMan)
	, iHandler(&aHandler)
	, iSecMan(aSecMan)
	, iPasskeyMinLength(aPasskeyMinLength)
	, iInternallyInitiated(aInternallyInitiated)
	, iStrongKeyRequired(aStrongKeyRequired)
	, iRecommendedPasskeyMinLength(aRecommendedPasskeyMinLength)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CBTPinRequester::ConstructL(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	iTimer = CSecNotifierRequesterTimer::NewL(*this);
	CSecNotifierRequester::ConstructL(aAddr);
	}

CBTPinRequester::~CBTPinRequester()
	{
	LOG_FUNC
	delete iTimer;
	Cancel();
	CBase::Delete(iUpdateNotifier.iNameUpdater); // Delete through the CBase virtual destructor.
	}


void CBTPinRequester::UpdateHandler(MPINCodeResponseHandler& aNewHandler)
	{
	LOG_FUNC
	iHandler = &aNewHandler;
	}


void CBTPinRequester::DoUpdateNotifier()
	{
	LOG_FUNC
 	if(IsActive())
 		{
 		switch(iNotifierType)
	 		{
	 	case ESspAwareNotifier:
	 		UpdateSspAwareNotifier();
	 		break;
	 	case ELegacyNotifier:
	 		UpdateLegacyNotifier();
	 		break;
	 	default:
			DEBUG_PANIC_LINENUM
	 		break;
	 		}
		}
	}

void CBTPinRequester::UpdateSspAwareNotifier()
	{
	LOG_FUNC
	if(!iUpdateNotifier.iNameUpdater)
		{
		//Create a new CSecNotifierUpdateAO object
		TRAP_IGNORE(iUpdateNotifier.iNameUpdater = CSecNotifierUpdateAO<TBTDeviceNameUpdateParamsPckg>::NewL(iNotifier, KBTPinCodeEntryNotifierUid));
		}
	if(iUpdateNotifier.iNameUpdater)
		{
		TBTDeviceName deviceName(KNullDesC);
		TInt err = KErrNotFound;
		if(iDeviceName)
			{
			TRAP(err, deviceName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName)); // Best effort attempt.
			}
		TBTDeviceNameUpdateParamsPckg pckg = TBTDeviceNameUpdateParams(deviceName, err);
		iUpdateNotifier.iNameUpdater->DoUpdate(pckg);
		}
	}

void CBTPinRequester::UpdateLegacyNotifier()
	{
	LOG_FUNC
	if(!iUpdateNotifier.iLegacyUpdater)
		{
		//Create a new CSecNotifierUpdateAO object
		TRAP_IGNORE(iUpdateNotifier.iLegacyUpdater = CSecNotifierUpdateAO<TBTNotifierUpdateParamsPckg>::NewL(iNotifier, KBTManPinNotifierUid));
		}
	if(iUpdateNotifier.iLegacyUpdater)
		{
		TBTNotifierUpdateParamsPckg pckg;
		if(iDeviceName)
			{
			TRAPD(err, pckg().iName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName));
			pckg().iResult = err; 	// Error code can be KErrNone
			if(err != KErrNone)
				{
				pckg().iName = KNullDesC;
				}
			}
		else
			{
			pckg().iName = KNullDesC;
			pckg().iResult = KErrNotFound;
			}

		iUpdateNotifier.iLegacyUpdater->DoUpdate(pckg);
		}
	}

void CBTPinRequester::DoRequest()
	{
	LOG_FUNC
	// The initial request for a PIN code entry dialog (that is SSP aware).  If this fails
	// we will attempt to use the legacy notifier.

	TBTDeviceName deviceName(KNullDesC);
	if(iDeviceName)
		{
		TRAP_IGNORE(deviceName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName)); // Best effort attempt.
		}

	iPinCodeEntryParamsPckg
		= TBTPinCodeEntryNotifierParams(iDevAddr, deviceName, iPasskeyMinLength, iInternallyInitiated, iStrongKeyRequired, iRecommendedPasskeyMinLength);

	iNotifierType = ESspAwareNotifier;
	iNotifier.StartNotifierAndGetResponse(iStatus, KBTPinCodeEntryNotifierUid, iPinCodeEntryParamsPckg, iPassKey);
	iTimer->Start(KPinRequesterTimeout);
	SetActive();
	}

void CBTPinRequester::FriendlyNameRetrieved(const TDesC& /*aName*/, TInt /*aResult*/)
	{
	LOG_FUNC
	// do nothing for now as it is not used anywhere
	__ASSERT_DEBUG(0, PANIC(KBTSecPanic, EBTSecNotImplemented));
	}


void CBTPinRequester::DoCancel()
	{
	LOG_FUNC
	iTimer->Cancel();
	// Cancel the appropriate notifier.
	switch(iNotifierType)
		{
	case ESspAwareNotifier:
		iNotifier.CancelNotifier(KBTPinCodeEntryNotifierUid);
		if(iUpdateNotifier.iNameUpdater)
			{
			iUpdateNotifier.iNameUpdater->Cancel();
			}
		break;
	case ELegacyNotifier:
		iNotifier.CancelNotifier(KBTManPinNotifierUid);
		if(iUpdateNotifier.iLegacyUpdater)
			{
			iUpdateNotifier.iNameUpdater->Cancel();
			}
		break;
	default:
		DEBUG_PANIC_LINENUM
		break;
		}
	iNotifierType = EInvalid;
	}

void CBTPinRequester::RunL()
	{
	LOG_FUNC
	iTimer->Cancel();
	switch(iNotifierType)
		{
	case ESspAwareNotifier:
		HandleSspAwareNotifierL();
		break;
	case ELegacyNotifier:
		HandleLegacyNotifierL();
		break;
	default:
		DEBUG_PANIC_LINENUM
		break;
		}
	}


void CBTPinRequester::HandleSspAwareNotifierL()
	{
	LOG_FUNC
	ASSERT_DEBUG(iNotifierType == ESspAwareNotifier);

	//got a PIN or error, so we are finished with this notifier.
	iNotifier.CancelNotifier(KBTPinCodeEntryNotifierUid);

	TInt err = iStatus.Int();

	if(err == KErrNotFound)
		{
		// Failed to find the SSP aware notifier, as such we should attempt to use
		// the legacy notifier.

		// First we prevent any updaters to the new notifier type.
		delete iUpdateNotifier.iNameUpdater;
		iUpdateNotifier.iNameUpdater = NULL;

		// Next we notify using the legacy notifier.
		iLegacyPinCodeParamsPckg().iBDAddr = iDevAddr;
		if(iDeviceName)
			{
			TRAPD(err, iLegacyPinCodeParamsPckg().iName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName));
			if(err != KErrNone)
				{
				iLegacyPinCodeParamsPckg().iName = KNullDesC;
				}
			}
		else
			{
			iLegacyPinCodeParamsPckg().iName = KNullDesC;
			}
		if(iStrongKeyRequired)
			{
			// As we cannot signal this any other way if a strong key is required, then we must
			// force a long passkey length.  This has potential IOP issues.
			iLegacyPinCodeParamsPckg().iPasskeyMinLength = KHCIPINCodeSize;
			}
		else
			{
			iLegacyPinCodeParamsPckg().iPasskeyMinLength = iPasskeyMinLength;
			}
		iLegacyPinCodeParamsPckg().iLocallyInitiated = iInternallyInitiated;

		iNotifierType = ELegacyNotifier;
		iNotifier.StartNotifierAndGetResponse(iStatus, KBTManPinNotifierUid, iLegacyPinCodeParamsPckg, iPassKey);
		iTimer->Start(KPinRequesterTimeout); // restart
		SetActive();
		}
	else
		{
		// The SSP aware PIN code entry notifier was found...

		//remove ourself from the notifier que, allowing the next notifier to be activated
		iSecMan.RemoveNotifierRequestFromQue(*this);
		iIsAddedToNotifierQue = EFalse;

		iNotifierType = EInvalid; // whatever happened we're done.
		if(err != KErrNone)
			{
			// The notifier signalled some other error - so be unpairable.
			iHandler->PINCodeRequestNegativeReply(iDevAddr);
			}
		else
			{
			// got a PIN
			iHandler->PINCodeRequestReply(iDevAddr, iPassKey);
			}
		}
	}


void CBTPinRequester::HandleLegacyNotifierL()
	{
	LOG_FUNC
	ASSERT_DEBUG(iNotifierType == ELegacyNotifier);

	//got a PIN or error, so finish off: unload the plugin
	iNotifier.CancelNotifier(KBTManPinNotifierUid);

	//remove ourself from the notifier que, allowing the next notifier to be activated
	RemoveMyselfFromQue();
	iIsAddedToNotifierQue = EFalse;

	iNotifierType = EInvalid; // whatever happened we're done.
	if (iStatus.Int())
		{
		// it failed - be unpairable
		iHandler->PINCodeRequestNegativeReply(iDevAddr);
		}
	else
		{
		// got a PIN
		iHandler->PINCodeRequestReply(iDevAddr, iPassKey);
		}
	}


TInt CBTPinRequester::RunError(TInt IF_FLOGGING(aError))
	{
	LOG_FUNC
	LOG1(_L8("\tCBTPinRequester::RunError(%d)\n"), aError);

	switch(iNotifierType)
		{
	case ESspAwareNotifier:
	case ELegacyNotifier:
		break;
	default:
		DEBUG_PANIC_LINENUM
		break;
		}

	iNotifierType = EInvalid;
	iHandler->PINCodeRequestNegativeReply(iDevAddr);
	return KErrNone;
	}

void CBTPinRequester::HandleTimeout()
	{
	LOG_FUNC
	// Cancel the request...
	Cancel();
	// Complete with a -ve reply
	iHandler->PINCodeRequestNegativeReply(iDevAddr);
	}

//------------------------------------------------------------------------//
//class CBTAuthorisor
//------------------------------------------------------------------------//

CBTAuthorisor* CBTAuthorisor::NewL(CBTAccessRequester& aParent, CBTSecMan& aSecMan, TUid aServiceUID)
	{
	LOG_STATIC_FUNC
	CBTAuthorisor* s = CBTAuthorisor::NewLC(aParent, aSecMan, aServiceUID);
	CleanupStack::Pop(s);
	return s;
	}

CBTAuthorisor* CBTAuthorisor::NewLC(CBTAccessRequester& aParent, CBTSecMan& aSecMan, TUid aServiceUID)
	{
	LOG_STATIC_FUNC
	CBTAuthorisor* s = new(ELeave) CBTAuthorisor(aParent, aSecMan, aServiceUID);
	CleanupStack::PushL(s);
	s->ConstructL(aParent.DeviceAddress());
	return s;
	}

CBTAuthorisor::CBTAuthorisor(CBTAccessRequester& aAccessRequester, CBTSecMan& aSecMan, TUid aServiceUID) :
	CSecNotifierRequester(aSecMan),iAccessRequester(aAccessRequester)
	{
	LOG_FUNC
	iAuthorisationParamsPckg().iUid = aServiceUID;
	CActiveScheduler::Add(this);
	}

CBTAuthorisor::~CBTAuthorisor()
	{
	LOG_FUNC
	Cancel();
	delete iUpdateNotifierAO;
	}


void CBTAuthorisor::DoUpdateNotifier()
	{
	LOG_FUNC
	if(IsActive())
		{
		if(!iUpdateNotifierAO)
			{
			//Create a new CSecNotifierUpdateAO object
			TRAP_IGNORE(iUpdateNotifierAO = CSecNotifierUpdateAO<TBTNotifierUpdateParamsPckg>::NewL(iNotifier, KBTManAuthNotifierUid));
			}

		if( (iUpdateNotifierAO) && (!iUpdateNotifierAO->IsActive()) )
			{
			TBTNotifierUpdateParamsPckg pckg;
			if(iDeviceName)
				{
				TRAPD(err, pckg().iName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName));
				pckg().iResult = err; 	// Error code can be KErrNone
				if (err!=KErrNone)
					{
					pckg().iName = KNullDesC;
					}
				}
			else
				{
				pckg().iName = KNullDesC;
				pckg().iResult = KErrNotFound;
				}

			iUpdateNotifierAO->DoUpdate(pckg);
			}
		}
	}

void CBTAuthorisor::DoRequest()
/**
Start the RNotifier plugin that deals with authorisation.
**/
	{
	LOG_FUNC
	TInt err(KErrNone);

	if (iDeviceName)
		{
		TRAP(err, iAuthorisationParamsPckg().iName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName));
		if (err!=KErrNone)
			{
			iAuthorisationParamsPckg().iName = KNullDesC;
			}
		}
	else
		{
		iAuthorisationParamsPckg().iName = KNullDesC;
		}
	iAuthorisationParamsPckg().iBDAddr  = iDevAddr;

	TBTSecEvent event(TBTSecEvent::EAuthorisationRequested);
	iAccessRequester.SendEvent(event);

	iNotifier.StartNotifierAndGetResponse(iStatus, KBTManAuthNotifierUid, iAuthorisationParamsPckg, iResultPckg);
	SetActive();
	}


void CBTAuthorisor::DoCancel()
	{
	LOG_FUNC
	iNotifier.CancelNotifier(KBTManAuthNotifierUid);
	}

void CBTAuthorisor::RunL()
	{
	LOG_FUNC

	// unload the plugin
	iNotifier.CancelNotifier(KBTManAuthNotifierUid);

	// remove ourself from the notifier queue, allowing the next notifier to be activated
	RemoveMyselfFromQue();
	iIsAddedToNotifierQue = EFalse;

	// notify owner of completion
	LOG1(_L8("\tCBTAuthorisor::RunL(): iStatus = %d\n"), iStatus.Int());
	if (iStatus.Int() != KErrNone)
		{
		// If anything went wrong, then deny access
		TBTSecEventAuthorisationComplete event(EFalse);
		iAccessRequester.SendEvent(event);
		}
	else
		{
		// otherwise allow/deny depends on the notifier
		TBTSecEventAuthorisationComplete event(iResultPckg());
		iAccessRequester.SendEvent(event);
		}
	}

TInt CBTAuthorisor::RunError(TInt aError)
	{
	LOG_FUNC
	//will never get called as our RunL doesn't leave.
	LOG1(_L8("\tCBTAuthorisor::RunError(%d)\n"), aError);
	return aError;
	}


//------------------------------------------------------------------------//
//class CSecNotifierUpdateAO
//------------------------------------------------------------------------//

template <class T>
CSecNotifierUpdateAO<T>* CSecNotifierUpdateAO<T>::NewL(RNotifier& aNotifier, TUid aNotifierUid)
	{
	LOG_STATIC_FUNC
	CSecNotifierUpdateAO* s = CSecNotifierUpdateAO::NewLC(aNotifier, aNotifierUid);
	CleanupStack::Pop(s);
	return s;
	}

template <class T>
CSecNotifierUpdateAO<T>* CSecNotifierUpdateAO<T>::NewLC(RNotifier& aNotifier, TUid aNotifierUid)
	{
	LOG_STATIC_FUNC
	CSecNotifierUpdateAO* s = new(ELeave) CSecNotifierUpdateAO();
	CleanupStack::PushL(s);
	s->ConstructL(aNotifier, aNotifierUid);
	return s;
	}

template <class T>
CSecNotifierUpdateAO<T>::CSecNotifierUpdateAO()
	: CActive(EPriorityStandard)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

template <class T>
CSecNotifierUpdateAO<T>::~CSecNotifierUpdateAO()
	{
	LOG_FUNC
	Cancel();
	}

template <class T>
void CSecNotifierUpdateAO<T>::ConstructL(RNotifier& aNotifier, TUid aNotifierUid)
	{
	LOG_FUNC
	iNotifier = aNotifier;
	iNotifierUid = aNotifierUid;
	}


template <class T>
void CSecNotifierUpdateAO<T>::DoUpdate(const T& aPckg)
	{
	LOG_FUNC
	if (IsActive())
		{
		TRAP_IGNORE(iPckgQueue.AppendL(aPckg));  // An update will be missed if OOM
		}
	else
		{
		//Retain a copy so that it does not go out of memory scope
		iPckg = aPckg;

		// We're not expecting an answer... but we want to use an asynchronous API
		// and so we need to "get a response"
		iNotifier.UpdateNotifierAndGetResponse(iStatus, iNotifierUid, iPckg, iAnswer);
		SetActive();
		}
	}

template <class T>
void CSecNotifierUpdateAO<T>::DoUpdateSynchronous(const T& aPckg)
	{
	LOG_FUNC
	//we're not expecting an answer so discard error as well...
	TInt err = iNotifier.UpdateNotifier(iNotifierUid, aPckg, iAnswer);
	LOG1(_L8("\tCSecNotifierUpdateAO::DoUpdateSynchronous error (%d)\n"), err);
	}


template <class T>
void CSecNotifierUpdateAO<T>::RunL()
	{
	LOG_FUNC
	//We can't do anything if an error is returned - just make sure we haven't done anything stupid...
	__ASSERT_DEBUG((iStatus==KErrNone)||(iStatus==KErrNoMemory)||(iStatus==KErrNotReady), PANIC(KBTSecPanic, EBTSecBadNotifierUpdate));
	if (iPckgQueue.Count() != 0)
		{
		iPckg = iPckgQueue[0];
		iPckgQueue.Remove(0);
		iNotifier.UpdateNotifierAndGetResponse(iStatus, iNotifierUid, iPckg, iAnswer);
		SetActive();
		}
	}

template <class T>
void CSecNotifierUpdateAO<T>::DoCancel()
	{
	LOG_FUNC
	iPckgQueue.Close();
	iNotifier.CancelNotifier(iNotifierUid); // no other API on Notifier to just cancel the update; but typically we'll want to cancel the whole notifier at this point(?)
	}

template <class T>
TInt CSecNotifierUpdateAO<T>::RunError(TInt IF_FLOGGING(aError))
	{
	LOG_FUNC
	LOG1(_L8("\tCSecNotifierUpdateAO::RunError(%d)\n"), aError);
	return KErrNone;
	}



//------------------------------------------------------------------------//
//class CBTNumericComparator
//------------------------------------------------------------------------//
CBTNumericComparator* CBTNumericComparator::NewL(const TBTDevAddr aAddr,
												 CBTSecMan& aSecMan,
												 TUint32 aNumericValue,
												 TBTNumericComparisonParams::TComparisonScenario aComparisonScenario, 
												 TBool aInternallyInitiated)
	{
	LOG_STATIC_FUNC
	CBTNumericComparator* s = CBTNumericComparator::NewLC(aAddr, aSecMan, aNumericValue, aComparisonScenario, aInternallyInitiated);
	CleanupStack::Pop(s);
	return s;
	}

CBTNumericComparator* CBTNumericComparator::NewLC(const TBTDevAddr aAddr,
												  CBTSecMan& aSecMan,
												  TUint32 aNumericValue,
												  TBTNumericComparisonParams::TComparisonScenario aComparisonScenario, 
												  TBool aInternallyInitiated)
	{
	LOG_STATIC_FUNC
	CBTNumericComparator* s = new(ELeave) CBTNumericComparator(aSecMan, aNumericValue, aComparisonScenario, aInternallyInitiated);
	CleanupStack::PushL(s);
	s->ConstructL(aAddr);
	return s;
	}

CBTNumericComparator::CBTNumericComparator(CBTSecMan& aSecMan, TUint32 aNumericValue, TBTNumericComparisonParams::TComparisonScenario aComparisonScenario, TBool aInternallyInitiated)
	: CSecNotifierRequester(aSecMan)
	, iSecMan(aSecMan)
	, iNumericValue(aNumericValue)
	, iComparisonScenario(aComparisonScenario)
	, iInternallyInitiated(aInternallyInitiated)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

CBTNumericComparator::~CBTNumericComparator()
	{
	LOG_FUNC
	Cancel();
	delete iNameUpdater;
	}


void CBTNumericComparator::DoUpdateNotifier()
	{
	LOG_FUNC
	if(IsActive())
		{
		if(!iNameUpdater)
			{
			//Create a new CSecNotifierUpdateAO object
			TRAP_IGNORE(iNameUpdater = CSecNotifierUpdateAO<TBTDeviceNameUpdateParamsPckg>::NewL(iNotifier, KBTNumericComparisonNotifierUid));
			}
		if(iNameUpdater)
			{
			TBTDeviceName deviceName(KNullDesC);
			TInt err = KErrNotFound;
			if(iDeviceName)
				{
				TRAP(err, deviceName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName)); // Best effort attempt.
				}

			TBTDeviceNameUpdateParamsPckg pckg = TBTDeviceNameUpdateParams(deviceName, err);
			iNameUpdater->DoUpdate(pckg);
			}
		}
	}

void CBTNumericComparator::DoRequest()
/**
Start the RNotifier plugin that deals with authorisation.
**/
	{
	LOG_FUNC
	TInt err(KErrNone);

	TBTDeviceName deviceName;

	if (iDeviceName)
		{
		TRAP(err, deviceName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName));
		if (err!=KErrNone)
			{
			deviceName = KNullDesC;
			}
		}
	else
		{
		deviceName = KNullDesC;
		}
	iNumericComparisonParamsPckg = TBTNumericComparisonParams(iDevAddr, deviceName, iNumericValue, iComparisonScenario, iInternallyInitiated);

	iNotifier.StartNotifierAndGetResponse(iStatus, KBTNumericComparisonNotifierUid, iNumericComparisonParamsPckg, iResultPckg);
	SetActive();
	}


void CBTNumericComparator::DoCancel()
	{
	LOG_FUNC
	iNotifier.CancelNotifier(KBTNumericComparisonNotifierUid);
	if(iNameUpdater)
		{
		iNameUpdater->Cancel();
		}
	}

void CBTNumericComparator::RunL()
	{
	LOG_FUNC
	// got an answer so unload the notifier
	iNotifier.CancelNotifier(KBTNumericComparisonNotifierUid);

	//remove ourself from the notifier que, allowing the next notifier to be activated
	RemoveMyselfFromQue();
	iIsAddedToNotifierQue = EFalse;

	__ASSERT_DEBUG(iNumericComparisonParamsPckg().DeviceAddress() == iDevAddr, PANIC(KBTSecPanic, EBTSecBadDeviceAddress));

	iSecMan.NumericComparisonComplete(iDevAddr, iResultPckg(), iStatus.Int());
	}

TInt CBTNumericComparator::RunError(TInt aError)
	{
	LOG_FUNC
	//will never get called as our RunL doesn't leave.
	LOG1(_L8("\tCBTNumericComparator::RunError(%d)\n"), aError);
	return aError;
	}


//------------------------------------------------------------------------//
//class CBTPasskeyEntry
//------------------------------------------------------------------------//
CBTPasskeyEntry* CBTPasskeyEntry::NewL(const TBTDevAddr aAddr,
												 CBTSecMan& aSecMan,
												 TUint32 aNumericValue,
												 TBool aInternallyInitiated)
	{
	LOG_STATIC_FUNC
	CBTPasskeyEntry* s = CBTPasskeyEntry::NewLC(aAddr, aSecMan, /*aAccessRequester,*/ aNumericValue, aInternallyInitiated);
	CleanupStack::Pop(s);
	return s;
	}

CBTPasskeyEntry* CBTPasskeyEntry::NewLC(const TBTDevAddr aAddr,
												  CBTSecMan& aSecMan,
												  TUint32 aNumericValue,
												  TBool aInternallyInitiated)
	{
	LOG_STATIC_FUNC
	CBTPasskeyEntry* s = new(ELeave) CBTPasskeyEntry(aSecMan, /*aAccessRequester,*/ aNumericValue, aInternallyInitiated);
	CleanupStack::PushL(s);
	s->ConstructL(aAddr);
	return s;
	}

CBTPasskeyEntry::CBTPasskeyEntry(CBTSecMan& aSecMan,
								 TUint32 aNumericValue,
								 TBool aInternallyInitiated)
	: CSecNotifierRequester(aSecMan)
	, iSecMan(aSecMan)
	, iNumericValue(aNumericValue)
	, iInternallyInitiated(aInternallyInitiated)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

CBTPasskeyEntry::~CBTPasskeyEntry()
	{
	LOG_FUNC
	Cancel();
	delete iKeypressUpdater;
	delete iNameUpdater;
	}

void CBTPasskeyEntry::KeyPressed(THCIPasskeyEntryNotificationType aKeyType)
	{
	LOG_FUNC
	if(IsActive())
		{
		if(!iKeypressUpdater)
			{
			//Create a new CSecNotifierUpdateAO object
			TRAP_IGNORE(iKeypressUpdater = CSecNotifierUpdateAO<TBTPasskeyDisplayUpdateParamsPckg>::NewL(iNotifier, KBTPasskeyDisplayNotifierUid));
			}
		if (iKeypressUpdater)
			{
			TBTPasskeyDisplayUpdateParamsPckg pckg = TBTPasskeyDisplayUpdateParams(aKeyType);
			iKeypressUpdater->DoUpdate(pckg);
			}
		}
	}

void CBTPasskeyEntry::DoUpdateNotifier()
	{
	LOG_FUNC
	if(IsActive())
		{
		if(!iNameUpdater)
			{
			//Create a new CSecNotifierUpdateAO object
			TRAP_IGNORE(iNameUpdater = CSecNotifierUpdateAO<TBTDeviceNameUpdateParamsPckg>::NewL(iNotifier, KBTPasskeyDisplayNotifierUid));
			}
		if(iNameUpdater)
			{
			TBTDeviceName deviceName(KNullDesC);
			TInt err = KErrNotFound;
			if(iDeviceName)
				{
				TRAP(err, deviceName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName)); // Best effort attempt.
				}
			TBTDeviceNameUpdateParamsPckg pckg = TBTDeviceNameUpdateParams(deviceName, err);
			iNameUpdater->DoUpdate(pckg);
			}
		}
	}

void CBTPasskeyEntry::DoRequest()
/**
Start the RNotifier plugin that deals with authorisation.
**/
	{
	LOG_FUNC
	TBTDeviceName deviceName;

	if (iDeviceName)
		{
		TRAPD(err, deviceName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName));
		if (err!=KErrNone)
			{
			deviceName = KNullDesC;
			}
		}
	else
		{
		deviceName = KNullDesC;
		}
	iPasskeyDisplayParamsPckg = TBTPasskeyDisplayParams(iDevAddr, deviceName, iNumericValue, iInternallyInitiated);

	iNotifier.StartNotifierAndGetResponse(iStatus, KBTPasskeyDisplayNotifierUid, iPasskeyDisplayParamsPckg, iResultPckg);
	SetActive();
	}


void CBTPasskeyEntry::DoCancel()
	{
	LOG_FUNC
	iNotifier.CancelNotifier(KBTPasskeyDisplayNotifierUid);
	if(iKeypressUpdater)
		{
		iKeypressUpdater->Cancel();
		}
	if(iNameUpdater)
		{
		iNameUpdater->Cancel();
		}
	}

void CBTPasskeyEntry::RunL()
	{
	LOG_FUNC
	//got a PIN or error, so finish off: unload the plugin
	iNotifier.CancelNotifier(KBTPasskeyDisplayNotifierUid);

	//remove ourself from the notifier que, allowing the next notifier to be activated
	RemoveMyselfFromQue();
	iIsAddedToNotifierQue = EFalse;

	__ASSERT_DEBUG(iPasskeyDisplayParamsPckg().DeviceAddress() == iDevAddr, PANIC(KBTSecPanic, EBTSecBadDeviceAddress));

	iSecMan.PasskeyNotificationComplete(iDevAddr, iStatus.Int());
	}

TInt CBTPasskeyEntry::RunError(TInt aError)
	{
	LOG_FUNC
	//will never get called as our RunL doesn't leave.
	LOG1(_L8("\tCBTPasskeyEntry::RunError(%d)\n"), aError);
	return aError;
	}

//------------------------------------------------------------------------//
//class CBTNumericComparator
//------------------------------------------------------------------------//
CBTUserConfirmer* CBTUserConfirmer::NewL(const TBTDevAddr aAddr,
												 CBTSecMan& aSecMan,
												 TBool aInternallyInitiated)
	{
	LOG_STATIC_FUNC
	CBTUserConfirmer* s = CBTUserConfirmer::NewLC(aAddr, aSecMan, aInternallyInitiated);
	CleanupStack::Pop(s);
	return s;
	}

CBTUserConfirmer* CBTUserConfirmer::NewLC(const TBTDevAddr aAddr,
												  CBTSecMan& aSecMan,
												  TBool aInternallyInitiated)
	{
	LOG_STATIC_FUNC
	CBTUserConfirmer* s = new(ELeave) CBTUserConfirmer(aSecMan, aInternallyInitiated);
	CleanupStack::PushL(s);
	s->ConstructL(aAddr);
	return s;
	}

CBTUserConfirmer::CBTUserConfirmer(CBTSecMan& aSecMan, TBool aInternallyInitiated)
	: CSecNotifierRequester(aSecMan)
	, iSecMan(aSecMan)
	, iInternallyInitiated(aInternallyInitiated)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

CBTUserConfirmer::~CBTUserConfirmer()
	{
	LOG_FUNC
	Cancel();
	delete iNameUpdater;
	}


void CBTUserConfirmer::DoUpdateNotifier()
	{
	LOG_FUNC
	if(IsActive())
		{
		if(!iNameUpdater)
			{
			//Create a new CSecNotifierUpdateAO object
			TRAP_IGNORE(iNameUpdater = CSecNotifierUpdateAO<TBTDeviceNameUpdateParamsPckg>::NewL(iNotifier, KBTUserConfirmationNotifierUid));
			}
		if(iNameUpdater)
			{
			TBTDeviceName deviceName(KNullDesC);
			TInt err = KErrNotFound;
			if(iDeviceName)
				{
				TRAP(err, deviceName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName)); // Best effort attempt.
				}

			TBTDeviceNameUpdateParamsPckg pckg = TBTDeviceNameUpdateParams(deviceName, err);
			iNameUpdater->DoUpdate(pckg);
			}
		}
	}

void CBTUserConfirmer::DoRequest()
/**
Start the RNotifier plugin that deals with authorisation.
**/
	{
	LOG_FUNC
	TInt err(KErrNone);

	TBTDeviceName deviceName;

	if (iDeviceName)
		{
		TRAP(err, deviceName = BTDeviceNameConverter::ToUnicodeL(*iDeviceName));
		if (err!=KErrNone)
			{
			deviceName = KNullDesC;
			}
		}
	else
		{
		deviceName = KNullDesC;
		}
	iUserConfirmationParamsPckg = TBTUserConfirmationParams(iDevAddr, deviceName, iInternallyInitiated);

	iNotifier.StartNotifierAndGetResponse(iStatus, KBTUserConfirmationNotifierUid, iUserConfirmationParamsPckg, iResultPckg);
	SetActive();
	}


void CBTUserConfirmer::DoCancel()
	{
	LOG_FUNC
	iNotifier.CancelNotifier(KBTUserConfirmationNotifierUid);
	if(iNameUpdater)
		{
		iNameUpdater->Cancel();
		}
	}

void CBTUserConfirmer::RunL()
	{
	LOG_FUNC
	// got an answer so unload the notifier
	iNotifier.CancelNotifier(KBTUserConfirmationNotifierUid);

	//remove ourself from the notifier que, allowing the next notifier to be activated
	RemoveMyselfFromQue();
	iIsAddedToNotifierQue = EFalse;

	__ASSERT_DEBUG(iUserConfirmationParamsPckg().DeviceAddress() == iDevAddr, PANIC(KBTSecPanic, EBTSecBadDeviceAddress));

	iSecMan.UserConfirmationComplete(iDevAddr, iResultPckg(), iStatus.Int());
	}

TInt CBTUserConfirmer::RunError(TInt aError)
	{
	LOG_FUNC
	//will never get called as our RunL doesn't leave.
	LOG1(_L8("\tCBTUserConfirmation::RunError(%d)\n"), aError);
	return aError;
	}



