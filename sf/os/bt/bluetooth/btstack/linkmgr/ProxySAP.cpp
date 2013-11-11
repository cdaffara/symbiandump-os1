// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of proxy SAP.
// 
//

#include <bluetooth/logger.h>
#include "ProxySAP.h"
#include "linkutil.h"
#include "linkconsts.h"
#include "RawConduit.h"
#include "physicallinksmanager.h"
#include "Basebandmodel.h"
#include "BTSec.h"
#include "linkmgr.h"

#include <bluetooth/hci/aclpacketconsts.h>
#include <bluetooth/hci/hciconsts.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

#ifdef _DEBUG
PANICCATEGORY("proxysap");
#endif

//Diagnostic string for security check failures, in builds without platsec
//diagnostics this will be NULL.
const char* const KBT_PROXYSAP_NAME_DIAG = __PLATSEC_DIAGNOSTIC_STRING("Bluetooth Proxy SAP");

CBTProxySAP::CBTProxySAP(CPhysicalLinksManager& aLinksMan, CPhysicalLink* aPhysicalLink)
: CBTBasebandSAP(aLinksMan, aPhysicalLink),
  iRequestedLinkPolicy(EHoldMode | ESniffMode | EParkMode, ETrue),
  iNotifiedUp(EFalse), iTerminating(ENone), iBasebandNotifyOptions(0),
  iEventNotificationQueue(_FOFF(TBTQueuedBasebandEventNotification, iLink)),
  iEventNotificationStatus(EDisabled)
	{
	LOG1(_L("Creating proxy SAP 0x%08x"), this);
#ifdef PROXY_COMMUNICATES
	iHandle = KHCIBroadcastHandle;
#endif
	}

CBTProxySAP* CBTProxySAP::NewLC(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aPhysicalSAP)
	{
	CBTProxySAP* s = new(ELeave) CBTProxySAP(aConnectionMan, aPhysicalSAP);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}

CBTProxySAP* CBTProxySAP::NewL(CPhysicalLinksManager& aConnectionMan, CPhysicalLink* aPhysicalSAP)
	{
	CBTProxySAP* s = CBTProxySAP::NewLC(aConnectionMan, aPhysicalSAP);
	CleanupStack::Pop(s);
	return s;
	}

void CBTProxySAP::ConstructL()
	{
	CBTBasebandSAP::ConstructL();
	iAsyncCallback = new(ELeave) CAsyncCallBack(CActive::EPriorityStandard);

	if(iPhysicalLink)
		{
		User::LeaveIfError(iPhysicalLink->SubscribeProxySAP(*this));
		}
	// now we need to go async and check whether the physical link is up
	// has to be async because we don't have a socket at the moment
	AsyncCheckLinkUp();
	}

void CBTProxySAP::ClearPhysicalLink()
	{
	if (iPhysicalLink)
		{
		iPhysicalLink->UnsubscribeProxySAP(*this);
		iPhysicalLink = NULL;
		}
	}

CBTProxySAP::~CBTProxySAP()
	{
	LOG1(_L("Deleting proxy SAP 0x%08x"), this);
	
	// If iTerminating is set we are waiting for iLinksMan to call us back 
	// which means that it holds a pointer to this ProxySAP
	if (iTerminating!=ENone)
		{
		__ASSERT_DEBUG(EFalse, Panic(EBTProxySAPBadCanClose));
		iLinksMan.ClearTerminatingProxy(this);
		}
	
	ClearPhysicalLink();
	/*Remove this proxy SAP from the PLM queue*/
	iPLMLink.Deque();
	delete iAsyncCallback;
 	delete iRawConduit;
	}

// from SAP - the proxy will not do all of these
void CBTProxySAP::Start()
	{
	// do nothing
	}

void CBTProxySAP::RemName(TSockAddr& aAddr) const
	{
	// our name is the same is that of the PHY, assuming it is connected
	// note esock doesn't allow a great deal of maneouvour on errors here!
	if(iPhysicalLink && iPhysicalLink->IsConnected())
		{
		TBTSockAddr bbAddr(aAddr);
		bbAddr.SetBTAddr(iPhysicalLink->BDAddr());
		aAddr=bbAddr;   // Convert back
		}
	}

TInt CBTProxySAP::SetRemName(TSockAddr& aAddr)
	{
	TBTSockAddr addr = TBTSockAddr::Cast(aAddr);
	iRemoteDev = addr.BTAddr();

	// we now know our remote address so should try to get a PHY if there
	CPhysicalLink *link = iLinksMan.FindPhysicalLink(iRemoteDev);

	// If we're (re-)using the same physical link, there's no
	// reason to subscribe again
	if (link != iPhysicalLink)
		{
		// Unsubscribe from old link
		if (iPhysicalLink)
			{
			iPhysicalLink->UnsubscribeProxySAP(*this);
			}

		iPhysicalLink = link;

		// Subscribe to new one
		if (iPhysicalLink)
			{
			return iPhysicalLink->SubscribeProxySAP(*this);
			}
		}

	// in any case return no error since the BAP may try to create PHY
	return KErrNone;
	}

TInt CBTProxySAP::GetOption(TUint aLevel,TUint aName,TDes8& aOption) const
	{
	TInt rerr = KErrNone;

	if(aLevel != KSolBtLMProxy)
		{
		rerr = KErrNotSupported;
		}

	switch (aName)
		{
		case EBBEnumeratePhysicalLinks:
			iLinksMan.EnumeratePhysicalLinks(aOption);
			break;
		case EBBGetPhysicalLinkState:
			{
			if (aOption.Length() != sizeof(TBTBasebandEventNotification))
				return KErrArgument;
			
			if (!iPhysicalLink)
				{
				LOG1(_L("GetOption called on non-existent Phy ....so return %d (KErrDisconnected)"), KErrDisconnected);
				return KErrDisconnected;
				}
			TBTBasebandEventNotification basebandState;
			iPhysicalLink->GetCurrentBasebandState(basebandState);
			TBTBasebandEvent pkcgEvent(basebandState);
			aOption = pkcgEvent;
			}
			break;
		default:
			rerr = KErrArgument;
			break;
		}
	return rerr;
	}

void CBTProxySAP::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
	{
	if (aLevel == KSolBtLMProxy)
		{
		switch(aName)
			{
			case KLMReadRssiIoctl:
			case KLMReadLinkQualityIoctl:
			case KLMReadFailedContactCounterIoctl:
			case KLMReadCurrentTransmitPowerLevelIoctl:
				{
				if (!aOption || aOption->Length() != sizeof(TInt))
					{
					IoctlComplete(KErrArgument, aLevel, aName);
					break;
					}
				// Add the current request to a queue; pass this pointer to  call IoctlComplete()
				// when result is available from the controller. Multiple SAPs can queue data on
				// the same physical link.
				TPckgBuf<TInt> currentValuePckg;
				currentValuePckg.Copy(*aOption);
				
				if(iPhysicalLink)
					{
					iPhysicalLink->ReadNewPhysicalLinkMetricValue(aName, *this, currentValuePckg());
					}
				else
					{
					IoctlComplete(KErrDisconnected, KSolBtLMProxy, aName, NULL);
					}
				
				break;
				}
			case KLMBasebandEventOneShotNotificationIoctl:
				{
				ASSERT_DEBUG(iEventNotificationStatus == EDisabled);
				iEventNotificationStatus = EEnabledOneShot;
				const TBTBasebandEventNotification* option = reinterpret_cast<const TBTBasebandEventNotification*>(aOption->Ptr());
				__ASSERT_DEBUG(option->EventType(), Panic(EBTProxySAPInvalidEventMask));
				SetBasebandNotificationOptions(option->EventType());
				}
				break;

			case KLMBasebandEventNotificationIoctl:
				{
				ASSERT_DEBUG(iEventNotificationStatus != EEnabledOneShot);
				const TBTBasebandEventNotification* option = reinterpret_cast<const TBTBasebandEventNotification*>(aOption->Ptr());
				SetBasebandNotificationOptions(option->EventType());

				if(iEventNotificationStatus == EDisabled)
					{
					// Send an initial event describing the current state of the
					// baseband link.
					TBTBasebandEventNotification basebandState;
					
					if (!iPhysicalLink)
						{
						LOG1(_L("Ioctl called on non-existent Phy ....so indicate %d (KErrDisconnected)"), KErrDisconnected);
						SetNullBasebandState(basebandState);
						TBTBasebandEvent pkcgEvent(basebandState);
						IoctlComplete(KErrNone, KSolBtLMProxy, KLMBasebandEventNotificationIoctl, &pkcgEvent);
						}
					else
						{
						iPhysicalLink->GetCurrentBasebandState(basebandState);
						TBTBasebandEvent pkcgEvent(basebandState);
						IoctlComplete(KErrNone, KSolBtLMProxy, KLMBasebandEventNotificationIoctl, &pkcgEvent);
						}
					iEventNotificationStatus = EEnabledQueuing;
					}
				else
					{
					// Check if anything is currently in the queue.
					iEventNotificationStatus = EEnabledCanSend;

					if(!iEventNotificationQueue.IsEmpty())
						{
						TBTQueuedBasebandEventNotification* e = iEventNotificationQueue.First();
						iEventNotificationQueue.Remove(*e);
						TBTBasebandEventNotification event(e->EventType() & iBasebandNotifyOptions, e->ErrorCode());
						delete e;

						if(event.EventType())
							{
							TBTBasebandEvent pkcgEvent(event);
							IoctlComplete(KErrNone, KSolBtLMProxy, KLMBasebandEventNotificationIoctl, &pkcgEvent);
							iEventNotificationStatus = EEnabledQueuing;
							}
						}
					}
				}
				break;

			default:
				{
 				IoctlComplete(KErrNotSupported, aLevel, aName);
   				break;
 				}
			};
		}
	else
		{
		IoctlComplete(KErrNotSupported, aLevel, aName);
		}
	}

void CBTProxySAP::IoctlComplete(TInt aErr, TUint /*aLevel*/, TUint /*aName*/, TDesC8* aBuf)
 	{
 
 	if (iSocket)
 		{
 		if (aErr==KErrNone)
 			{
 			iSocket->IoctlComplete(aBuf);
 			}
 		else 
 			{	
 			iSocket->Error(aErr, MSocketNotify::EErrorIoctl);
 			}
   		}
   	}

void CBTProxySAP::SetNullBasebandState(TBTBasebandEventNotification & aEvent)
	{
	// Populate the event with no physical link baseband state.
	TUint32 events = 0;

	events |= ENotifyPhysicalLinkDown;

	aEvent.SetEventType(events);
	aEvent.SetErrorCode(0);
	}

void CBTProxySAP::CancelIoctl(TUint aLevel,TUint aName)
	{
	 
 	__ASSERT_DEBUG((aLevel == KSolBtLMProxy), Panic(EBTProxySAPInvalidIoctl));
 	
	if (aLevel == KSolBtLMProxy)
		{
		switch(aName)
			{
			case KLMBasebandEventNotificationIoctl:
				SetBasebandNotificationOptions(0);	
				iEventNotificationStatus = EDisabled;
				ClearBasebandEventQueue();
				break;

			case KLMBasebandEventOneShotNotificationIoctl:
				SetBasebandNotificationOptions(0);	
				iEventNotificationStatus = EDisabled;
				ClearBasebandEventQueue();
				break;
			case KLMReadRssiIoctl:
			case KLMReadLinkQualityIoctl:
			case KLMReadFailedContactCounterIoctl:
			case KLMReadCurrentTransmitPowerLevelIoctl:
				iPLMLink.Deque();
				break;
			default:
				__ASSERT_DEBUG(EFalse, Panic(EBTProxySAPInvalidIoctl));
				break;
			};
		}
	}

void CBTProxySAP::ClearBasebandEventQueue()
	{
	TSglQueIter<TBTQueuedBasebandEventNotification> iter(iEventNotificationQueue);
	while (iter)
		{
		TBTQueuedBasebandEventNotification* e = iter++;
		iEventNotificationQueue.Remove(*e);
		delete e;
		}
	}

void CBTProxySAP::SetBasebandNotificationOptions(TUint32 aOption) 
	{
	iBasebandNotifyOptions = aOption;
	}


TInt CBTProxySAP::SAPSetOption(TUint aLevel,TUint aName, const TDesC8 &aOption)
	{
	LOG3(_L("SetOpt %d, %d on proxy SAP 0x%08x"), aLevel, aName, this);
	TInt rerr = KErrNone;

	// Arbitration will be required after most operations.
	TBool arbitrate = ETrue;
	TBool immediateArbitration = EFalse;
	TBool localPriority = EFalse;

	if(aLevel != KSolBtLMProxy)
		{
		rerr = KErrNotSupported;
		}

	if(!iPhysicalLink && 
		aName!=EBBSubscribePhysicalLink)
		{
		rerr = KErrDisconnected;
		}

	if(rerr == KErrNone)
		{
		switch (aName)
			{
			case EBBSubscribePhysicalLink:
				{
				// Make sure we're not already subscribed
				if (iPhysicalLink)
					{
					__ASSERT_DEBUG(EFalse,Panic(EBTProxySAPAlreadySubscribed));
					rerr = KErrArgument;
					break;
					}			
				
				TPckgBuf<TBTDevAddr> pckg;
				pckg.Copy(aOption);
				// try to find a PHY from addr, and if successful subscribe to it
				iPhysicalLink = iLinksMan.FindPhysicalLink(pckg());
				if (iPhysicalLink)
					{
					rerr = iPhysicalLink->SubscribeProxySAP(*this);
					if(rerr == KErrNone)
						{
						iRemoteDev = pckg();
						}
					}
				else
					{
					LOG1(_L("Proxy SAP 0x%08x -- couldn't find physical link"), this);
					LOG(_L("Looking for addr: "));
					LOGBTDEVADDR(pckg());
					iLinksMan.DumpPhysicalLinks();
					rerr = KErrDisconnected;
					}

				arbitrate = EFalse;
				}
				break;
			case EBBBeginRaw:
				//EBBBeginRaw requires additional security checking, NetworkControl
				//in addition to the LocalServices that was required to create the SAP
			
				__ASSERT_DEBUG(iSecurityChecker, User::Panic(KSECURITY_PANIC, EBTPanicNullSecurityChecker));
				
				rerr = iSecurityChecker->CheckPolicy(KNETWORK_CONTROL, KBT_PROXYSAP_NAME_DIAG);
				if (rerr != KErrNone)
					{
				    break;
				    }											
				if(!(iPhysicalLink && iPhysicalLink->Handle()!=KHCIBroadcastHandle))
				//if no existing, connected PHY, try to find a random PHY which is
					{
					// We don't support this option if we're subscribed to the broadcast handle
					if (iPhysicalLink)
						{
						__ASSERT_DEBUG(EFalse,Panic(EBTProxySAPSubscribedToBroadcastHandle));
						rerr = KErrArgument;
						break;
						}
					
					iPhysicalLink = iLinksMan.FindPhysicalLink();
					if (iPhysicalLink)
						{
						rerr = iPhysicalLink->SubscribeProxySAP(*this);
						if(rerr == KErrNone)
							{
							iRemoteDev = iPhysicalLink->BDAddr();
							}
						}
					else
						{
						rerr = KErrDisconnected;
						}
					}
				if(rerr == KErrNone)
					{
					rerr = CreateRawConduit();
					}
				arbitrate = EFalse;
				break;
			case EBBCancelModeRequest:
				rerr = SetModeRequested(EActiveMode);
				break;
			case EBBRequestSniff:
				rerr = SetModeRequested(ESniffMode);
				localPriority = ETrue;
				break;
			// clients cannot ask for Hold
			case EBBRequestPark:
				rerr = SetModeRequested(EParkMode);
				localPriority = ETrue;
				break;
			case EBBRequestRoleMaster:
				if(iRequestedLinkPolicy.IsSwitchAllowed() && iPhysicalLink->IsRoleSwitchSupported())
					{
					rerr = iPhysicalLink->RequestChangeRole(EMaster);
					arbitrate = EFalse;
					}
				else
					{
					rerr = KErrNotSupported;
					}
				break;

			case EBBRequestRoleSlave:
				if(iLinksMan.LinkManagerProtocol().IsRoleSwitchSupportedLocally() && 
					iRequestedLinkPolicy.IsSwitchAllowed() && iPhysicalLink->IsRoleSwitchSupported())
					{
					rerr = iPhysicalLink->RequestChangeRole(ESlave);
					arbitrate = EFalse;
					}
				else
					{
					rerr = KErrNotSupported;
					}
				break;

			case EBBRequestPreventRoleChange:
				iRequestedLinkPolicy.SetSwitchAllowed(EFalse);
				break;

			case EBBRequestAllowRoleChange:
				iRequestedLinkPolicy.SetSwitchAllowed(ETrue);
				break;
				
			case EBBRequestChangeSupportedPacketTypes:
				{
				TUint16 options = *reinterpret_cast<const TUint16*>(aOption.Ptr());
				if(aOption.Length() != sizeof(TUint16))
					{
					rerr = KErrArgument;
					}
				else
					{
					iPhysicalLink->ChangeConnectionPacketType(options);
					arbitrate = EFalse;
					}
				}
				break;
			
			case EBBRequestLinkAuthentication:
				rerr = iPhysicalLink->Authenticate(EFalse);
				break;
				
			case EBBRequestExplicitActiveMode:
				{
				TBool option = *reinterpret_cast<const TBool*>(aOption.Ptr());
				if(aOption.Length() != sizeof(TBool))
					{
					rerr = KErrArgument;
					}
				else
					{
					iRequestedActiveMode = option;
					if(iRequestedActiveMode)
						{
						localPriority = ETrue;
						}
					}
				}
				break;

			default:
				{
				arbitrate = EFalse;
				if(aName & EBBRequestPreventAllLowPowerModes)
					{
					if (aName & EBBRequestPreventSniff)
						{
						if(iRequestedLinkPolicy.IsModeAllowed(ESniffMode))
							{
							iRequestedLinkPolicy.SetModeAllowed(ESniffMode, EFalse);
							arbitrate = ETrue;
							}
						}
					if (aName & EBBRequestPreventHold)
						{
						if(iRequestedLinkPolicy.IsModeAllowed(EHoldMode))
							{
							iRequestedLinkPolicy.SetModeAllowed(EHoldMode, EFalse);
							arbitrate = ETrue;
							}
						}
					if (aName & EBBRequestPreventPark)
						{
						if(iRequestedLinkPolicy.IsModeAllowed(EParkMode))
							{
							iRequestedLinkPolicy.SetModeAllowed(EParkMode, EFalse);
							arbitrate = ETrue;
							}
						}
					
					immediateArbitration = ETrue;
					}
				else if (aName & EBBRequestAllowAllLowPowerModes)
					{
					if (aName & EBBRequestAllowSniff)
						{
						if(!(iRequestedLinkPolicy.IsModeAllowed(ESniffMode)))
							{
							iRequestedLinkPolicy.SetModeAllowed(ESniffMode, ETrue);
							arbitrate = ETrue;
							}
						}
					if (aName & EBBRequestAllowHold)
						{
						if(!(iRequestedLinkPolicy.IsModeAllowed(EHoldMode)))
							{
							iRequestedLinkPolicy.SetModeAllowed(EHoldMode, ETrue);
							arbitrate = ETrue;
							}
						}
					if (aName & EBBRequestAllowPark)
						{
						if(!(iRequestedLinkPolicy.IsModeAllowed(EParkMode)))
							{
							iRequestedLinkPolicy.SetModeAllowed(EParkMode, ETrue);
							arbitrate = ETrue;
							}
						}
					
					immediateArbitration = ETrue;
					}
				else
					{
					rerr = KErrUnknown;
					}
				}
			};
		}	

	// Check if arbitration is required.
	if(arbitrate && rerr == KErrNone)
		{
		rerr = iPhysicalLink->Arbitrate(immediateArbitration, localPriority);
		}

	return rerr;
	}

TInt CBTProxySAP::SetModeRequested(TBTLinkMode aMode)
	{
	TInt retVal = KErrNotSupported;

	if(aMode == EActiveMode)
		{
		retVal = iRequestedLinkPolicy.SetModeRequested(aMode);
		}
	else
		{
		if(iLinksMan.LinkManagerProtocol().IsModeSupportedLocally(aMode) && 
		   iPhysicalLink->IsModeSupportedRemotely(aMode))
			{
			retVal = iRequestedLinkPolicy.SetModeRequested(aMode);
			}
		}
	return retVal;
	}

TBool CBTProxySAP::CanCreatePhysicalLink()
	{
	TBool ret = EFalse;
	// return ETrue if allowed to create phy
	if (iPhysicalLink)
		{
		// already there!
		iSocket->ConnectComplete();
		}
	else if(!(Baseband().IsACLPossible()))
		{
		iSocket->Error(KErrInsufficientBasebandResources, MSocketNotify::EErrorConnect);
		}
	else
		{
		ret = ETrue;
		}
	return ret;
	}


void CBTProxySAP::ActiveOpen()
	{
	// create physical link for bonding
	TBool proceed = CanCreatePhysicalLink();
	if (proceed)
		{
		// right, go and connect - no juice on this overload
		TRAPD(err, iPhysicalLink = &iLinksMan.NewPhysicalLinkL(iRemoteDev));
		if(err == KErrNone)
			{
			iPhysicalLink->SubscribeProxySAP(*this);
			iPhysicalLink->Connect(EPagingNormal);
			}
		else
			{
			iSocket->Error(KErrNoMemory, MSocketNotify::EErrorConnect);
			}
		}
	}

void CBTProxySAP::ActiveOpen(const TDesC8& aConnectionToken)
	{
	// create physical link for faster connection
	TBool proceed = CanCreatePhysicalLink();
	if (proceed)
		{
		// set the cookie
		TPhysicalLinkQuickConnectionTokenBuf tokenBuf;
		tokenBuf.Copy(aConnectionToken);

		TRAPD(err, iPhysicalLink = &iLinksMan.NewPhysicalLinkL(tokenBuf().iDevice));
		if(err == KErrNone)
			{
			iPhysicalLink->SubscribeProxySAP(*this);
			iPhysicalLink->Connect(tokenBuf().iPolicy.iPageTimePolicy); // user determines paging policy with their "cookie"
			}
		else
			{
			iSocket->Error(KErrNoMemory, MSocketNotify::EErrorConnect);
			}
		}
	}

TInt CBTProxySAP::PassiveOpen(TUint /*aQueSize*/)
	{
	return KErrNotSupported;
	}

TInt CBTProxySAP::PassiveOpen(TUint /*aQueSize*/,const TDesC8& /*aConnectionData*/)
	{
	return KErrNotSupported;
	}

void CBTProxySAP::Shutdown(TCloseType aCloseType)
	{
	// just our phy to shutdown
	Shutdown(aCloseType, KDisconnectOnePhysicalLink);
	}

void CBTProxySAP::Shutdown(TCloseType aCloseType,const TDesC8& aDisconnectOption)
	{
	
	//Only ENormal and EImmediate may be used for BaseBand connections
	__ASSERT_DEBUG((aCloseType == CServProviderBase::ENormal||aCloseType == CServProviderBase::EImmediate),Panic(EBTProxySAPInvalidTerminate));
	
	
	if (aDisconnectOption == KDisconnectOnePhysicalLink && aCloseType== CServProviderBase::ENormal)
		{
		// Unbinding Socket, no additional Caps required
		// Remove ourselves from the physical SAP - doesnt detach the PHY itself
		ClearPhysicalLink();
		}
	else
		{		
		//Atleast one Physical link is to be terminated,  any 'Shutdown' on physical 
		//links requires an additional security check for the NetworkControl Cap.
		//This is required in addition to the LocalServices that was required to create the SAP
		__ASSERT_DEBUG(iSecurityChecker, User::Panic(KSECURITY_PANIC, EBTPanicNullSecurityChecker));
			
		TInt rerr = iSecurityChecker->CheckPolicy(KNETWORK_CONTROL, KBT_PROXYSAP_NAME_DIAG);
		if (rerr != KErrNone)
			{
			if(iSocket)
				iSocket->Error(rerr, MSocketNotify::EErrorClose);
			return;
			}										

		if (aDisconnectOption == KDisconnectAllPhysicalLinks || aDisconnectOption == KDisconnectAllPhysicalLinksForPowerOff)
			{
			// Disconnecting All BT Physical Links
			// Only support link *termination*, this is done as normal cos esock weirdness
			__ASSERT_ALWAYS(aCloseType == CServProviderBase::ENormal, Panic(EBTProxySAPInvalidTerminate));
			rerr = iLinksMan.TerminateAllPhysicalLinks(this, aDisconnectOption == KDisconnectAllPhysicalLinksForPowerOff ? ERemoteAboutToPowerOff : ERemoteUserEndedConnection);
			LOG2(_L("Proxy SAP 0x%08x -- Terminating all PHY Links, error: %d"), this, rerr);
			
			// If there was an error terminating any of the physical links then we can 
			// call CanClose straight away, otherwise this is done when iLinksMan calls
			// TerminatePhysicalLinksComplete()
			if (rerr == KErrNone)
				{
				iTerminating=ETerminatingAllLinks;
				return;
				}
			}
		else
			{
			//Disconnecting only One Physical Link
			CPhysicalLink* phy2Term = NULL;
			
			if (aDisconnectOption == KDisconnectOnePhysicalLink)
				{
				// EImmediate Shutdown - Terminate the Physical link
				phy2Term = iPhysicalLink;
				}
			else
				{
				// Shutdown on a specific address (a PHY we arent attached to)
				// again done as normal since esock is weird
				TBTDevAddr addr(aDisconnectOption);
				phy2Term = iLinksMan.FindPhysicalLink(addr);		
				}
				
			if(phy2Term)
				{
				LOG1(_L("Proxy SAP 0x%08x -- Immediate ShutDown, terminate PHY Link"), this);

				// If EImmediate shutdown then esock doesn't expect us to call CanClose() so
				// we don't want iLinksMan to let us know when the physical link has been terminated
				if (aCloseType == CServProviderBase::EImmediate)
					{
					rerr = iLinksMan.TerminatePhysicalLink(phy2Term, NULL);
					}
				else
					{
					rerr = iLinksMan.TerminatePhysicalLink(phy2Term, this);
				
					// If there was an error terminating the physical link then we can 
					// call CanClose straight away, otherwise this is done when iLinksMan calls
					// TerminatePhysicalLinksComplete()
					if (rerr == KErrNone)
						{
						iTerminating=ETerminatingSingleLink;
						return;
						}
					}
				}
			else
				{
				// Possible race Condition - the phy may have gone anyway
				LOG1(_L("Proxy SAP 0x%08x -- PHY Link already terminated by other process"), this);
				}
			}
		}
		
	// this may change if esock is altered
	if (aCloseType==CServProviderBase::ENormal)
		{
		// Signal that SAP can be deleted
		iSocket->CanClose();
		}
	// CAREFUL - might be deleted at this point
	}
	

void CBTProxySAP::AutoBind()
	{
#ifndef PROXY_COMMUNICATES
	Panic(EBTProxySAPUnexpectedEvent);
#endif
	}

#ifdef PROXY_COMMUNICATES
TUint CBTProxySAP::Write(const TDesC8& aData,TUint aOptions, TSockAddr* /*aAddr*/)
	{
	// we can throw away some of the options - we only allow them to be 8bit
	__ASSERT_DEBUG(iRawConduit, Panic(EBTProxySAPNotReadyToSendRawData));
	if(!iRawConduit)
		{
		iSocket->Error(KErrNotReady, MSocketNotify::EErrorSend);
		return 0; //Indicates write could not be completed
		}

	TUint8 flags = static_cast<TUint8>(aOptions);
	__ASSERT_DEBUG((flags & KPacketPBFlagMask) == 0, Panic(EBTProxyUserAttemptingToTransmitL2CAPDirectData));
	if(flags & KPacketPBFlagMask)
		{
		iSocket->Error(KErrNotSupported, MSocketNotify::EErrorSend);
		return 0; //Indicates write could not be completed
		}

	// push the data onto the ACL Pool
	// the ACL Pool in this build has a reserved slot for broadcast (could use flags!)
	TUint retVal =0;
	if (iPhysicalLink)
		{
		// only allowed to write raw stuff when a phy is in place
		// mainly our restriction - but definately the case for broadcasting
		retVal = iRawConduit->Write(aData, flags);
		if (retVal==0)
 			{
			iSocket->Error(KErrInUse, MSocketNotify::EErrorSend);
 			}
		}
	else
		{
		// haven't even got a phy
		iSocket->Error(KErrNotReady, MSocketNotify::EErrorSend);
		}

	return retVal;
#else
TUint CBTProxySAP::Write(const TDesC8& /*aDesc*/,TUint /*aOptions*/, TSockAddr* /*aAddr*/)
	{
	Panic(EBTProxySAPUnexpectedEvent);
	return 0;
#endif
	}

void CBTProxySAP::Timeout(TBasebandTimeout /*aTimeout*/)
	{
	// none of interest
	}

TInt CBTProxySAP::CreateRawConduit()
	{
	// instatiates the method by which a Proxy can send raw data
	// expected to be created via a SetOpt call into this
	__ASSERT_DEBUG(!iRawConduit, Panic(EBTProxySAPRawConduitAlreadyExists));

	TInt err = KErrAlreadyExists;

	if(!iRawConduit)
		{
		TRAP(err, iRawConduit = CACLRawConduit::NewL(*this));
		}

	return err;
	}

void CBTProxySAP::DataReceived()
	{
	// do we dont know if the socket was interested in the data?!
	// but we're unreliable, so people get whatever is here

	// we can check the sock address in getdata and junk if it's not required
	if (iSocket && iNotifiedUp)
		{
		iSocket->NewData(1);
		}
	// else drop
	}

void CBTProxySAP::GetData(TDes8& aData,TUint /*aOptions*/,TSockAddr* /*aAddr*/)
	{
	// the conduit has data ready for us...get it now
	__ASSERT_DEBUG(iRawConduit, Panic(EBTProxySAPNoRawConduit));
	if(iRawConduit)
		{
		iRawConduit->GetData(aData);
		}
	}

TBool CBTProxySAP::IsModeRequested(TBTLinkMode aMode) const
	{
	return !(iRequestedLinkPolicy.IsModeRequested(aMode));
	}

TBool CBTProxySAP::IsAnyModeChangeRequested() const
	{
	return iRequestedLinkPolicy.IsAnyModeRequested();
	}

void CBTProxySAP::AsyncCheckLinkUp()
	{
	TCallBack cb(SignalConnectComplete, this);
	iAsyncCallback->Set(cb);
	iAsyncCallback->SetPriority(CActive::EPriorityHigh);
	iAsyncCallback->CallBack();
	}

/*static*/ TInt CBTProxySAP::SignalConnectComplete(TAny* aCBTProxySAP)
/**
	For breaking synchronous call chain
**/
	{
	// if the PhysicalSAP is connected, we should tell the proxy ASAP (asynchronously though!)
	__ASSERT_ALWAYS(aCBTProxySAP, Panic(EBTProxySAPNullCallback));

	CBTProxySAP& p = *static_cast<CBTProxySAP*>(aCBTProxySAP);

	if (p.iPhysicalLink && p.iPhysicalLink->IsConnected())
		{
		p.iSocket->ConnectComplete();
		p.iNotifiedUp = ETrue;
		// lower priority back - might be useful for other async operations
		p.iAsyncCallback->SetPriority(CActive::EPriorityStandard);
		}
	return EFalse;
	}

void CBTProxySAP::TerminatePhysicalLinksComplete()
	{
	iTerminating=ENone;
	
	if (iSocket)
		{
		// Signal that SAP can be deleted
		iSocket->CanClose();
		}
	// CAREFUL - might be deleted at this point
	}

void CBTProxySAP::PhysicalLinkUp()
	{
	// ah! good! let's tell our socket
	if (iSocket)
		{
		iSocket->ConnectComplete();
		iNotifiedUp = ETrue;
		}
	}

void CBTProxySAP::PhysicalLinkDown()
	{
	if (iSocket && iNotifiedUp)
		{
		iSocket->Disconnect();
		iNotifiedUp = EFalse;
		}
	ClearPhysicalLink();
	}

void CBTProxySAP::PhysicalLinkError(TInt aError)
	{
	if (iSocket)
		{
		iSocket->Error(aError, MSocketNotify::EErrorAllOperations);
		}
	ClearPhysicalLink();
	}

void CBTProxySAP::PhysicalLinkChange(const TBTBasebandEventNotification& aEvent, CPhysicalLink& /*aPhysicalLink*/)
/**
	PHY has notified us of a change - demux here
*/
	{
	switch(aEvent.EventType())
		{
		case ENotifyPhysicalLinkUp:
			{
			PhysicalLinkUp();
			}
			break;

		case ENotifyPhysicalLinkDown:
			{
			PhysicalLinkDown();
			}
			break;

		case ENotifyPhysicalLinkError:
			PhysicalLinkError(aEvent.ErrorCode());
			if(iEventNotificationStatus == EEnabledOneShot)
				{
				iEventNotificationStatus = EDisabled;
				}
			return;

		default:
			break;
		};	
	
	if(iEventNotificationStatus == EEnabledOneShot)
		{
		TBTBasebandEventNotification event(aEvent.EventType() & iBasebandNotifyOptions, aEvent.ErrorCode());
		if(event.EventType())
			{
				TBTBasebandEvent pkcgEvent(event);
				IoctlComplete(KErrNone, KSolBtLMProxy, KLMBasebandEventOneShotNotificationIoctl, &pkcgEvent);

				iEventNotificationStatus = EDisabled;
			}
		}

	else if(iEventNotificationStatus != EDisabled)
		{
		TBTBasebandEventNotification event(aEvent.EventType() & iBasebandNotifyOptions, aEvent.ErrorCode());
		if(event.EventType())
			{
			if(iEventNotificationQueue.IsEmpty() && iEventNotificationStatus == EEnabledCanSend)
				{
				// The queue is empty.  Send the event now.
				TBTBasebandEvent pkcgEvent(event);
				IoctlComplete(KErrNone, KSolBtLMProxy, KLMBasebandEventNotificationIoctl, &pkcgEvent);

				iEventNotificationStatus = EEnabledQueuing;
				}
			else
				{
				TBTQueuedBasebandEventNotification* e = new TBTQueuedBasebandEventNotification(event);
				if (e)
					{
					iEventNotificationQueue.AddLast(*e);
					}
				}
			}
		}
	}

TUint8 CBTProxySAP::GetRequestedModes() const
	{
	return iRequestedLinkPolicy.CurrentModeRequest();
	}

TUint8 CBTProxySAP::GetAllowedModes() const
	{
	return iRequestedLinkPolicy.ModesAllowed();
	}

TBool CBTProxySAP::IsRoleSwitchAllowed() const
	{
	return iRequestedLinkPolicy.IsSwitchAllowed();
	}

TBool CBTProxySAP::RequestedActiveMode() const
	{
	return iRequestedActiveMode;
	}

TRequestedLinkPolicy::TRequestedLinkPolicy(TUint8 aModesAllowed, TBool aSwitchAllowed)
  : iSwitchAllowed(aSwitchAllowed), iModesAllowed(aModesAllowed),
    iCurrentModeRequest(0)
	{
	}

TBool TRequestedLinkPolicy::IsModeAllowed(TBTLinkMode aMode) const
	{
	TBool allowed = EFalse;

	switch (aMode)
		{
		case EActiveMode:
			allowed = ETrue;
			break;

		case EHoldMode:
		case ESniffMode:
		case EParkMode:
			allowed = iModesAllowed & aMode;
			break;

		default:
			Panic(EBTConnectionUnknownLowPowerMode);
		}	
	return allowed;
	}

TBool TRequestedLinkPolicy::IsSwitchAllowed() const
	{
	return iSwitchAllowed;
	}

void TRequestedLinkPolicy::SetModeAllowed(TBTLinkMode aMode, TBool aAllowed)
	{
	switch (aMode)
		{
		case EHoldMode:
		case ESniffMode:
		case EParkMode:
			aAllowed ? iModesAllowed |= aMode : iModesAllowed &= ~aMode;
			break;
		default:
			Panic(EBTConnectionUnknownLowPowerMode);
		}
	}

void TRequestedLinkPolicy::SetSwitchAllowed(TBool aAllowed)
	{
	iSwitchAllowed = aAllowed;
	}


TBool TRequestedLinkPolicy::IsModeRequested(TBTLinkMode aMode) const
	{
	TBool requested = EFalse;

	switch (aMode)
		{
		case EActiveMode:
		case EHoldMode:
		case ESniffMode:
		case EParkMode:
			requested = iCurrentModeRequest & aMode;
			break;

		default:
			Panic(EBTConnectionUnknownLowPowerMode);
		}	
	return requested;
	}

TBool TRequestedLinkPolicy::IsAnyModeRequested() const
	{
	return iCurrentModeRequest;
	}


TInt TRequestedLinkPolicy::SetModeRequested(TBTLinkMode aMode)
	{
	switch (aMode)
		{
		case EActiveMode:
		case EHoldMode:
		case ESniffMode:
		case EParkMode:
			iCurrentModeRequest = TUint8(aMode);
			break;

		default:
			Panic(EBTConnectionUnknownLowPowerMode);
		}
		return KErrNone;
	}

TUint8 TRequestedLinkPolicy::ModesAllowed() const		
	{ 
	return iModesAllowed; 
	}

TUint8 TRequestedLinkPolicy::CurrentModeRequest() const	
	{ 
	return iCurrentModeRequest; 
	}
