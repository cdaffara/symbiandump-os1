// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of physical links
// 
//

#include <bluetooth/logger.h>
#include "physicallinksmanager.h"
#include "physicallinks.h"
#include "AclDataQController.h"
#include "ACLSAP.h"
#include "SCOSAP.h"
#include "ProxySAP.h"
#include "linkconsts.h"
#include "hcifacade.h"
#include "hostresolver.h"
#include "roleswitchhelper.h"
#include "pairingscache.h"
#include "oobdata.h"
#include "pairingserver.h"
#include "encryptionkeyrefreshhelper.h"

#include <bt_sock.h>

#include <bluetooth/hci/commandstatusevent.h>
#include <bluetooth/hci/sniffmodecommand.h>
#include <bluetooth/hci/exitsniffmodecommand.h>
#include <bluetooth/hci/holdmodecommand.h>
#include <bluetooth/hci/parkmodecommand.h>
#include <bluetooth/hci/exitparkmodecommand.h>
#include <bluetooth/hci/modechangeevent.h>
#include <bluetooth/hci/disconnectcommand.h>
#include <bluetooth/hci/disconnectioncompleteevent.h>
#include <bluetooth/hci/hciconsts.h>

#include <bluetooth/hci/linkkeyrequestreplycommand.h>
#include <bluetooth/hci/linkkeyrequestreplynegativecommand.h>
#include <btextnotifierspartner.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

#ifdef _DEBUG
PANICCATEGORY("plink");
#endif

static const THCIErrorCode KDefaultRejectReason = EHostSecurityRejection; // see spec Error Codes

#ifdef _DEBUG
#define __CHECK_CONNECTION_HANDLE(aHandle) __ASSERT_DEBUG(HasHandle(aHandle), Panic(EBTLinkMgrConnectionEventInWrongSAP));
#else
#define __CHECK_CONNECTION_HANDLE(aHandle) aHandle=aHandle; // to suppress warnings
#endif

CPhysicalLink* CPhysicalLink::NewLC(CPhysicalLinksManager& aConnectionMan, CRegistrySession& aRegSess, const TBTNamelessDevice& aDevice)
	{
	LOG_STATIC_FUNC
	CPhysicalLink* s = new(ELeave) CPhysicalLink(aConnectionMan, aRegSess, aDevice);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}

CPhysicalLink* CPhysicalLink::NewL(CPhysicalLinksManager& aConnectionMan, CRegistrySession& aRegSess, const TBTNamelessDevice& aDevice)
	{
	LOG_STATIC_FUNC
	CPhysicalLink* s = NewLC(aConnectionMan, aRegSess, aDevice);
	CleanupStack::Pop(s);
	return s;
	}

CPhysicalLink::CPhysicalLink(CPhysicalLinksManager& aConnectionMan, CRegistrySession& aRegSess, const TBTNamelessDevice& aDevice)
	: iLinksMan(aConnectionMan)
	, iRegSess(aRegSess)
	, iDevice(aDevice)
	, iHandle(KHCIBroadcastHandle)
	, iRemoteFeatures(KInvalidRemoteFeatures)
	, iDeviceResult(KDeviceNotObtained)
	, iRegistryHelpers(_FOFF(CBTRegistryHelperBase,iLink))
	, iProxySAPs(_FOFF(CBTProxySAP, iQueueLink))
	, iOverrideParkRequests(EFalse)
	, iOverrideLPMRequests(EFalse)
	, iConnectionPacketTypeChanged(EFalse)
	, iLowPowModeCtrl(*this, iLinksMan.HCIFacade().CommandQController())
	, iDisconnectCtrl(*this, iLinksMan.HCIFacade().CommandQController())
	, iAuthenticationCtrl(*this, iLinksMan.HCIFacade().CommandQController())
	, iLSTO(0)
	, iAutoKeyRefreshQue(_FOFF(XAutoKeyRefreshToken, iQueLink))
	, iRemoteDeviceRecordedByStack(ENull)
	{
	LOG_FUNC
	// don't initialise physical link policy just yet - wait until connection completes
	// by then we'll have best information to go on
	LOG1(_L("New CPhysicalLink [0x%08x]"), this);
	}

CPhysicalLink::~CPhysicalLink()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iACLLogicalLinks.Count()==0, Panic(EBTACLLogicalLinkBadDebind));
	__ASSERT_DEBUG(iSyncLogicalLink == NULL, Panic(EBTSCOLogicalLinkBadDebind));

	LOG1(_L("CPhysicalLink Destructing this = 0x%08x"), this);

	// tell ConnectionsManager...
	iLinksMan.RemovePhysicalLink(*this);
	iACLLogicalLinks.Close();

	RemoveIdleTimer();

	LOG(_L("sec\tClosing subscribers..."))

	LOG(_L("sec\tClosing helpers..."))
	TSglQueIter<CBTRegistryHelperBase> iter(iRegistryHelpers);
	CBTRegistryHelperBase* helper;
	// Detach from any Registry Helpers for which this object
	// is the parent.
	while(iter)
		{
		helper = iter++;
		helper->DetachParent();
		}

	iLowPowModeCtrl.Abort();
	iDisconnectCtrl.Abort();
	iAuthenticationCtrl.Abort();

	delete iPhysicalLinkMetrics;
	delete iPinRequester;
	delete iNumericComparator;
	delete iUserConfirmer;
	delete iPasskeyEntry;
	delete iArbitrationDelay;
	delete iRoleSwitchCompleteCallBack;
	delete iKeyRefreshCompleteCallBack;
	delete iEncryptionEnforcer;

	DeleteRoleSwitcher();
	DeleteKeyRefresher();
	}

void CPhysicalLink::ConstructL()
	{
	LOG_FUNC
	LOG1(_L("CPhysicalLink::ConstructL() this = 0x%08x"), this);
	GetDeviceFromRegistryL();
	iArbitrationDelay = CArbitrationDelayTimer::NewL(this);

	TCallBack cb1(RoleSwitchCompleteCallBack, this);
	iRoleSwitchCompleteCallBack = new (ELeave)CAsyncCallBack(cb1, EActiveHighPriority);

	TCallBack cb2(KeyRefreshCompleteCallBack, this);
	iKeyRefreshCompleteCallBack = new (ELeave)CAsyncCallBack(cb2, EActiveHighPriority);
	
	iPhysicalLinkMetrics = CPhysicalLinkMetrics::NewL(*this, iLinksMan.HCIFacade().CommandQController());
	}

TBool CPhysicalLink::HasHandle(THCIConnHandle aHandle) const
	{
	LOG_FUNC
	if (iHandle == aHandle)
		return ETrue;	// Only one ACL handle is available.
	if (iSyncLogicalLink && iSyncLogicalLink->Handle() == aHandle)
		return ETrue;
	return EFalse;
	}

TBool CPhysicalLink::HasSyncLink() const
	{
	LOG_FUNC
	return (iSyncLogicalLink != NULL);
	}

void CPhysicalLink::SubscribeLinkObserver(MPhysicalLinkObserver& aObserver)
/**
The subscribed object will be notified whenever a link state change occurs.
**/
	{
	LOG_FUNC
#ifdef _DEBUG
	//ensure the subscriber isn't already subscribed...
	TInt found = 0;
	TDblQueIter<MPhysicalLinkObserver> iter(iBasebandSubscribers);
	while (iter)
		{
		if (iter++ == &aObserver)
			{
			found++;
			}
		}

	__ASSERT_DEBUG(found==0, Panic(ELinkMgrBadBasebandArray));
#endif	//_DEBUG

	iBasebandSubscribers.AddFirst(aObserver.ObserverQLink());
	}

void CPhysicalLink::UnsubscribeLinkObserver(MPhysicalLinkObserver& aObserver)
/**
aSubscriber will no longer be notified of state changes.
**/
	{
	LOG_FUNC
	aObserver.ObserverQLink().Deque();
	}

TInt CPhysicalLink::TryToAndThenPreventHostEncryptionKeyRefresh(TAny* aOutToken)
	{
	LOG_FUNC
	TInt err = KErrNone;
	// The handling of the TAny* parameter seems a bit wacky - but it makes sense as follows
	// this call handles a call from the bluetooth control plane (which passes
	// only a TAny* as a parameter).  We need to return a value back through as well, so we need
	// a pointer to a pointer (so after using the input it can be modified to point to the
	// output).  We need a Bluetooth device address so a pointer to a pointer to a TBTDevAddr 
	// is passed down. Then the pointer to a pointer is used to update the pointer to a control
	// plane token (which represents a handle preventing host encryption key refreshes).
	if (!IsEncryptionPauseResumeSupported())
		{
		err = KErrNotSupported;
		*reinterpret_cast<MBluetoothControlPlaneToken**>(aOutToken) = NULL;
		}
	else
		{
		// Only refresh the key if no-one is preventing it and we aren't still 
		// refreshing the key from a previous request.  Note that although
		// the previous key refresh may actually have finished if the key
		// refresher is waiting for async delete we have only just refreshed 
		// the key and there's no point doing it again.
		if (iAutoKeyRefreshQue.IsEmpty() && !iKeyRefresher)
			{
			TRAPD(err, iKeyRefresher = CEncryptionKeyRefresher::NewL(iLinksMan, *this));
			if(!err)
				{
				// Kick off the helper state machine
				iKeyRefresher->StartHelper();
				}
			// If we can't refresh the encryption key, there's not much we can do
			}
		XAutoKeyRefreshToken* token = new XAutoKeyRefreshToken();
		if (token)
			{
			iAutoKeyRefreshQue.AddLast(*token);
			}
		else
			{
			err = KErrNoMemory;
			}
		*reinterpret_cast<MBluetoothControlPlaneToken**>(aOutToken) = token;
		}
	return err;
	}

void CPhysicalLink::RegistryTaskComplete(CBTRegistryHelperBase* aHelper, TInt /*aResult*/)
/**
	A task has completed where we don't expect a response - just cleanup helper
**/
	{
	LOG_FUNC
	// remove from array
	iRegistryHelpers.Remove(*aHelper);
	// delete
	delete aHelper;
	}

/**
The completion function of a registry helper returning a remote device entry.
*/
void CPhysicalLink::RegistryTaskComplete(CBTRegistryHelperBase* aHelper,
										 const TBTNamelessDevice& aDevice,
										 TInt aResult)
/**
	A registry task has completed where we expect a device to be returned
**/
	{
	LOG_FUNC
	LOG1(_L("CPhysicalLink: Registry task complete; result %d"), aResult);

	// The TBTNamelessDevice assignment operator only performs an update on valid
	// entries, as we need a bitwise copy (since we want to mirror what is in the registry)
	// we need to go to these extras steps.
	TPckg<TBTNamelessDevice> cachedVersion(iRegistryDevice);
	cachedVersion.Copy(TPckg<TBTNamelessDevice>(aDevice));

	// We have to be careful about information flow from the registry to the actual representation (iDevice).
	// Although the values in the returned entry are useful if the stack has not yet got the information,
	// if the stack has already gathered values then they will be more up-to-date.  Therefore we need to
	// assign the current values first if they are valid to build up the latest representation.
	TBTNamelessDevice device = aDevice; // Take a local copy to work on.
	if(iDevice.IsValidDeviceClass())
		{
		device.SetDeviceClass(iDevice.DeviceClass());
		}
	if(iDevice.IsValidPageScanRepMode())
		{
		device.SetPageScanRepMode(iDevice.PageScanRepMode());
		}
	if(iDevice.IsValidPageScanPeriodMode())
		{
		device.SetPageScanPeriodMode(iDevice.PageScanPeriodMode());
		}
	if(iDevice.IsValidPageScanMode())
		{
		device.SetPageScanMode(iDevice.PageScanMode());
		}
	if(iDevice.IsValidClockOffset())
		{
		device.SetClockOffset(iDevice.ClockOffset());
		}
	if(iDevice.IsValidUsed())
		{
		device.SetUsed(iDevice.Used());
		}
	if(iDevice.IsValidSeen())
		{
		device.SetSeen(iDevice.Seen());
		}

	// With the link key (and friends) we need to be even more careful, as there is effectively
	// two-way flow of information:
	// 1) Link key storage from the stack.
	// 2) Unbonding from the clients of the registry.
	// Further to this, if the registry entry does not have a link key, it can mean one of several
	// things:
	// a) No link key has been generated with the remote device yet.
	// b) A client of the registry has unbonded the remote device.
	// c) The current link key is not to form a bond.
	// Taking this into account we make a suitable choice about what to do.
	if(!iDevice.IsValidLinkKey())
		{
		// If there isn't any link key in the system yet - and so it is safe to update the representation
		// to use the value from the registry.
		// <NOP> - see the global update below.
		}
	else
		{
		// We currently have a link key....
		if(device.IsValidLinkKey() || iRegistryDevBeingMod.InUse())
			{
			// Even if the linkkey in registry is valid, it must be older or same as the current
			// link key (as the stack is the only one who should be adding keys). Also, if we are in the
			// middle of updating the registry (for any purpose) then we can ignore the registry value as it
			// may not be up-to date. So, for both the cases just update with the latest.
			// There is an interesting case if client modifies the registry by unpairing/or deleting the device
			// and stack also tries to updates the link key in registry at the same time. We will give priority
			// to stack's updations by keeping the latest copy as it is.
			device.SetLinkKey(iDevice.LinkKey(), iDevice.LinkKeyType());
			if(iDevice.IsValidPassKey())
				{
				device.SetPassKey(iDevice.PassKey());
				}
			}
		else
			{
			// This is the interesting case where a client has unbonded, but we already have a link
			// key.  What we do here is interesting.
			if(IsAuthenticationPending() || LinkState().Authenticated() || SimplePairingMode() == EPhySimplePairingEnabled)

				{
				// If we're already authenticated / authenticating then we're already using our link
				// key.  As such we need to continue using it, but we don't want to update
				// Also if the link is SSP enabled then we can re-authenticate - this removes any chance of a
 				// race condition with the registry.
				iPreventLinkKeyUpdateReg = ETrue; // prevent locally cached link key being given back to the registry
				}
			else
				{
				// The link key hasn't been used yet - so we can update the representation to not
				// have a link key (as requested by the user).
				// Currently there is no chance of a race with the registry as combination link keys are
 				// always form a bond.
				iDevice.DeleteLinkKey(); // delete locally cached link key
				}
			}
		}

	// Finally update the representation - we should have manipulated it correctly.
	iDevice = device;

	// Store the result of the retrieval for usage later.
	iDeviceResult = aResult;

	if (iLinkKeyRequestOutstanding)
		{
		// the HW asked earlier for a link key - we can respond now
		__ASSERT_DEBUG(iDevice.IsValidAddress(), Panic(EBTPhysicalLinksInvalidAddress));
		LinkKeyRequestResponseAttempt(ETrue);
		}

	RegistryTaskComplete(aHelper, aResult);	 // cleans up our helper
	}

void CPhysicalLink::RegistryTaskComplete(CBTRegistryHelperBase* aHelper, TRegistryUpdateStatus aRegUpdateStatus, TInt aResult)
	{
/**
	This method allows the RegistryHelpers to indicate the registry action taking place

**/
	LOG_FUNC
	if(iRemoteDeviceRecordedByStack == EPending && aRegUpdateStatus != ENoChange)
		{
		if (aResult == KErrNone)
			{
			iRemoteDeviceRecordedByStack = EComplete;
			}
		else
			{
			iRemoteDeviceRecordedByStack = ENull;
			}
		}

	iRegistryDevBeingMod.RequestCompleted();
	RegistryTaskComplete(aHelper, aResult);	 // cleans up our helper
	}

void CPhysicalLink::CompletedPackets(THCIConnHandle aConnH, TUint16 aNumPackets)
/**
	In this domain we are happy that a valid event has come in telling us that
	the buffers in the local controller have been freed

	Since this reflects both local hardware features (buffers) and flow control
	we treat them separately

**/
	{
	LOG_FUNC
	// firstly report that local hardware buffers free (via inlines!)
	iLinksMan.LinkManagerProtocol().LinkMuxer().DataQController().CompletedPackets(aConnH, aNumPackets);

	// we forward this to the thing that sends the packets so that they can do Flow Control

	// could speed up - already done this handle test to get into *this* object
	if (iSyncLogicalLink && iSyncLogicalLink->Handle() == aConnH)
		{
		iSyncLogicalLink->PacketsSent(aConnH, aNumPackets);
		}
	else
		{
		for (TInt i=iACLLogicalLinks.Count()-1; i>=0; i--)
			{
			iACLLogicalLinks[i]->PacketsSent(aConnH, aNumPackets);
			}
		}
	}

void CPhysicalLink::TryToSend()
	{
	LOG_FUNC
	for (TInt i=iACLLogicalLinks.Count()-1; i>=0; i--)
		{
		iACLLogicalLinks[i]->TryToSend();
		}
	}

void CPhysicalLink::NewLinkKey(const TBTDevAddr& /*aAddr*/, const TBTLinkKey& aLinkKey, THCILinkKeyType aLinkKeyType)
	{
	LOG_FUNC
	if(iLinkKeyPending)
		//user has entered a PIN and a new link key has come in.
		{
		iLinkKeyPending = EFalse;
		iPreventLinkKeyUpdateReg = EFalse; //override previous unpair so that link key will be stored in the registry.
		}

	TBTLinkKeyType linkKeyType = ELinkKeyCombination;

	switch (aLinkKeyType)
		{
	case ECombinationKey:
		linkKeyType = ELinkKeyCombination;
		break;
	case EDebugCombinationKey:
		linkKeyType = ELinkKeyDebug;
		break;
	case EAuthenticatedCombinationKey:
		linkKeyType = ELinkKeyAuthenticated;
		break;
	case EUnauthenticatedCombinationKey:
		if (iLocalMITM || (iAuthenticationRequirement & KAuthenticationMitmReqMask) ||
			(iIOCapability == EIOCapsDisplayOnly) || (iIOCapability == EIOCapsNoInputNoOutput))
			{
			linkKeyType = ELinkKeyUnauthenticatedNonUpgradable;
			}
		else
			{
			linkKeyType = ELinkKeyUnauthenticatedUpgradable;
			}
		break;
	case EChangedCombinationKey:
		linkKeyType = iDevice.LinkKeyType(); // The link key type hasn't changed
		break;

	case ELocalUnitKey:
	case ERemoteUnitKey:
	default:
		LOG1(_L("CPhysicalLink: Unexpected link key type (%d)"), aLinkKeyType);
		__ASSERT_DEBUG(EFalse, Panic(EBTUnexpectedLinkKeyType));
		break;
		}

	if (linkKeyType == ELinkKeyCombination && iNewPinCodeValid)
		{
		// Only bind a PIN code to the device if we receive a link key based on it.
		iNewPinCodeValid = EFalse;
		iDevice.SetPassKey(iNewPinCode);
		}

	if (iLinksMan.SecMan().IsDedicatedBondingAttempted(iDevice.Address()))
		{
		iLinkKeyObtainedThroughDedicatedBonding = ETrue;
		}

	SetLinkKey(aLinkKey, linkKeyType);	// keeps a copy in our 'cache', updates paired list in PHYs mananger

	TRAP_IGNORE(StoreDeviceL(EFalse)); //EFalse: new meaning - do not prevent addition
	// if that errored we just have to keep it here - but it won't be in registry
	// so it'll be a transient pairing
	}

void CPhysicalLink::UpdateFromInquiryCache()
	{
	LOG_FUNC
	CBTInqResultRecord* juice = NULL; //juice is data from remote which can help/speed up a connection
	// see if we can speed up the connection - the inquiry cache may have some juice
	juice = iLinksMan.LinkManagerProtocol().InquiryMgr().BasebandParametersFromCache(iDevice.Address());
	if(!juice)
	//no juice in inquiry manager
		{
		return;
		}

	TInquiryLogEntry& jle = juice->LogEntry();
	// Inquiry cache may have useful info we can use: put them in our device
	// so that we can use this, and update this into the Registry later.
	// This will allow Apps to get this to initiate faster connections.
	// The update to the Registry doesn't happen here, but at other interesting times
	ASSERT_DEBUG(jle.iBdaddr == iDevice.Address());
	//Only update with juice values from inquiry manager if they are have come from HCI
	//(i.e. from remote) - records in the inquiry manager are filled with default
	//values and then fields are updated as relevant HCI events occur.
	// - iDevice may already contain values found in the registry which are
	//not default and therefore should have come themselves from the HCI
	if(juice->IsPageScanModeFromHCI())
		{
		iDevice.SetPageScanMode(jle.iPageScanMode);
		}
	if(juice->IsPageScanRepModeFromHCI())
		{
		iDevice.SetPageScanRepMode(jle.iPageScanRepetitionMode);
		}
	if(juice->IsClockOffsetFromHCI())
		{
		iDevice.SetClockOffset(jle.iClockOffset);
		}
	if(juice->IsCoDFromHCI())
		{
		iDevice.SetDeviceClass(jle.iCoD);
		}
	}

void CPhysicalLink::StoreDeviceL( TBool aPreventDeviceAddition )
	{
	LOG_FUNC

	UpdateFromInquiryCache(); //check juice has not just come in - update iDevice if so

	//only write back the necessary attributes
	TBTNamelessDevice device;
	if(iDevice.IsValidAddress())
		{
		device.SetAddress(iDevice.Address());
		}
	if(iDevice.IsValidDeviceClass())
		{
		device.SetDeviceClass(iDevice.DeviceClass());
		}
	if(iDevice.IsValidLinkKey() && !iPreventLinkKeyUpdateReg)
		{
		// We store the link key if it was obtained through dedicated bonding, even if the remote device indicated "no bonding"
		// This fixes issues with a lot of remote stacks, and we'll let the UI delete the link key if they want to.
		if(iLinkKeyObtainedThroughDedicatedBonding ||
			!(iIOCapsReceived && (iAuthenticationRequirement == EMitmNotReqNoBonding || iAuthenticationRequirement == EMitmReqNoBonding)))
			{
			LOG(_L("!!! Storing Link Key in Registry"));
			device.SetLinkKey(iDevice.LinkKey(), iDevice.LinkKeyType());
			}
		}
	if(iDevice.IsValidPageScanRepMode())
		{
		device.SetPageScanRepMode(iDevice.PageScanRepMode());
		}
	if(iDevice.IsValidPageScanPeriodMode())
		{
		device.SetPageScanPeriodMode(iDevice.PageScanPeriodMode());
		}
	if(iDevice.IsValidPageScanMode())
		{
		device.SetPageScanMode(iDevice.PageScanMode());
		}
	if(iDevice.IsValidClockOffset())
		{
		device.SetClockOffset(iDevice.ClockOffset());
		}
	if(iDevice.IsValidUsed())
		{
		device.SetUsed(iDevice.Used());
		}
	if(iDevice.IsValidSeen())
		{
		device.SetSeen(iDevice.Seen());
		}
	if(iDevice.IsValidPassKey())
		{
		device.SetPassKey(iDevice.PassKey());
		}

	// if already exists an outstanding request with the same device info
	// is useless to store it again. Just return doing nothing.
	if (iRegistryDevBeingMod.InUse() && iRegistryDevBeingMod.IsEqual(device))
		{
		return;	// do nothing
		}

	CBTDeviceModifier* modifier = CBTDeviceModifier::NewL(iRegSess, *this, iLinksMan.LinkManagerProtocol().InquiryMgr());
	iRegistryHelpers.AddLast(*modifier);
	TBool allowAdd = EFalse;

	if(!aPreventDeviceAddition && iRemoteDeviceRecordedByStack == ENull)
		//Only allow the device to be added to the registry if
		// 1) this StoreDeviceL function has not been called
		//	  with its "prevent device addition" parameter set to true
		// 2) we believe the cached remote device details have not yet
		//    been recorded in the registry.
		//    (If they have and if they are no longer there it is
		// 	   because an app has deleted them from the registry...
		//	   so we should not re-add them )
		{
		allowAdd = ETrue;
		}
	modifier->Start(device, allowAdd);
	iRegistryDevBeingMod.Begin(device);

	if(allowAdd)
		//The physical link object has now made its attempt to add its cached
		//remote device details to the registry.
		{
		iRemoteDeviceRecordedByStack = EPending;
		}

	}

void CPhysicalLink::SetLinkKey(const TBTLinkKey& aLinkKey, TBTLinkKeyType aLinkKeyType)
	{
	LOG_FUNC
	iDevice.SetLinkKey(aLinkKey, aLinkKeyType); // keeps a copy in our 'cache'
	}

void CPhysicalLink::ReadRemoteSupportedFeaturesComplete(THCIErrorCode aErr, THCIConnHandle aConnH, const TBTFeatures& aBitMask)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);

	if (aErr == KErrNone)
		{
		iRemoteFeatures = aBitMask;

		if(!iConnectionPacketTypeChanged)
			{
			//Send change connection packet type HCI command if application hasn't, we have to do this
			//because some hardware defaults to using 1-slot packets unless told otherwise by the host.
			ChangeConnectionPacketType(EAnyACLPacket);
			}

		//If remote host supports extended features, then send request to read these.
		if (iRemoteFeatures[ESupportedExtendedFeaturesBit])
			{
			TRAP_IGNORE(iLinksMan.HCIFacade().ReadRemoteExtendedFeaturesL(iHandle, KRemoteExtendedFeaturesPage1));
			}
		else
			{
			// If the remote doesn't support extended features, then they cannot support SSP
			// (no way to indicate the host supported bit).  So set feature as disabled.
			RemoteSimplePairingModeDetermined(EPhySimplePairingDisabled);
			}
		}
	else
		{
		iRemoteFeatures = TBTFeatures(KInvalidRemoteFeatures);
		}
	}

void CPhysicalLink::ReadRemoteExtendedFeaturesComplete(THCIErrorCode aErr, THCIConnHandle aConnH, TUint64 aBitMask, TUint8 aPageNumber, TUint8 /* aMaximumPageNumber */)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);

	LOG1(_L("CPhysicalLink: ReadRemoteExtendedFeaturesComplete; result %d"), aErr);

	switch (aPageNumber)
		{
		case KRemoteExtendedFeaturesPage1:
			{
			TPhysicalLinkSimplePairingMode currentSetting = SimplePairingMode();

			if (aErr == EOK && aBitMask & (1 << ESecureSimplePairingHostSupportBit) && iLinksMan.SecMan().LocalSimplePairingMode())
				{
				RemoteSimplePairingModeDetermined(EPhySimplePairingEnabled);
				}
			else
				{
				RemoteSimplePairingModeDetermined(EPhySimplePairingDisabled);
				}
			break;
			}
		default:
			{
			// Ignore
			}
		}
	}

void CPhysicalLink::ReadRemoteVersionInfoComplete(THCIErrorCode aErr, THCIConnHandle aConnH, const TBTDevRemoteHwVersion& aVer)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);
	if (aErr == KErrNone)
		{
		iRemoteVersion = aVer;
		}
	else
		{
		iRemoteVersion = KInvalidRemoteHwVersion;
		}
	}

void CPhysicalLink::AuthenticationComplete(THCIErrorCode aErr, THCIConnHandle aConnH)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);
	iLinkState.SetAuthenticated(aErr == EOK);
	//__ASSERT_DEBUG(iAuthStateMask == EAuthenticationRequestPending,Panic(EUnexpectedAuthenticationState));
	AuthenticationComplete(EAuthenticationRequestPending);

	if (aErr!=EOK)
		{
		if(aErr == ERemoteUserEndedConnection)
			{
			HandlePrefetch();
			}
		iNewPinCodeValid = EFalse;
		delete iPinRequester;
		iPinRequester = NULL;
		iPinHandler = NULL;
		}
	else
		{
		__ASSERT_DEBUG(!iPinHandler, Panic(EBTPhysicalLinkPinHandlerStillPresent));
		}

	iLinksMan.SecMan().AuthenticationComplete(BDAddr(), aErr);

	TBTBasebandEventNotification event(ENotifyAuthenticationComplete, aErr);
	NotifyStateChange(event);
	}

void CPhysicalLink::HandlePrefetch()
	{
	// Here we determine whether we need to handle a pre-fetch case.  If there is an
	// outbound authentication attempt and we have a PIN notifier we need to hand it
	// over to the pre-fetch manager.
	// We also note the fact that we've done this for any access requester to know whether
	// it should indicate to defer.
	if(iPinRequester && iLinksMan.SecMan().IsOutboundAccessRequest(BDAddr()) && iSimplePairingMode != EPhySimplePairingEnabled)
		{
		TInt err = iLinksMan.PrefetchMan().HandleOverPinRequester(BDAddr(), iPinRequester);
		if(err == KErrNone)
			{
			iPinRequester = NULL; // As far as phys links is concerned the pin requester doesn't exist anymore.
			iPinHandler = NULL; // So the handler is also not needed.
			}
		}
	}

void CPhysicalLink::EncryptionChange(THCIErrorCode aErr, THCIConnHandle aConnH, TBool aEncrypted)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);

	if(aErr == EOK)
		{
		// Only change state if it's an actual encryption change - not an error (because if an error
		// we should presumably stay the same).
		if(aEncrypted)
			{
			iLinkState.SetEncrypted(ETrue);
			if(!iLinkState.Authenticated())
				{
				// If a link is encrypted then we know that the link is also authenticated.
				iLinkState.SetAuthenticated(ETrue);
				}
			}
		else
			{
			iLinkState.SetEncrypted(EFalse);
			}
		}

	TBTBasebandEventNotification event(aEncrypted ? ENotifyEncryptionChangeOn : ENotifyEncryptionChangeOff, aErr);
	NotifyStateChange(event);

	// Having the encryption enforcer present means that we have asked for encryption.
	// But we should only act if this is a valid encryption change event.
	if (iEncryptionEnforcer && aErr == EOK)
		{
		// if the roleswitch has been requested by us then CRoleSwitcher has already parked / unparked
		// the ACL controller. That means that the request is submitted twice.
		// This is not supposed to be a problem, but we need to be aware of it.
		if (!aEncrypted)
			{
			iLinksMan.LinkManagerProtocol().ACLController().SetParked(iHandle, ETrue);
			iEncryptionEnforcer->EncryptionDisabled(IsEncryptionPauseResumeSupported());
			}
		else
			{
			iLinksMan.LinkManagerProtocol().ACLController().SetParked(iHandle, EFalse);
			iEncryptionEnforcer->EncryptionEnabled();
			}
		}
	}

TInt CPhysicalLink::ChangeEncryption(THCIEncryptModeFlag aEnable)
	{
	LOG_FUNC
	// no policy to adjust, but test local & remote features
	if (!IsEncryptionSupported())
		{
		return KErrNotSupported;
		}

	TRAPD(err, iLinksMan.HCIFacade().SetEncryptL(Handle(), aEnable));
	if (err == KErrNone)
		{
		err = ManageEncryptionEnforcement(aEnable);
		}

	return err;
	}

TInt CPhysicalLink::Authenticate(TBool aRequireAuthenticatedLinkKey)
	{
	LOG_FUNC
	// no policy to adjust, just check to see if the link is already authenticated
	TInt err = KErrNone;
	__ASSERT_DEBUG(iSimplePairingMode != EPhySimplePairingUndefined, Panic(EBTPhysicalLinksInvalidArgument));

	/* If its a dedicated bonding attempt, then we always want to authenticate again in order to generate
	 * a stronger linkkey if possible
	 */
	if (!iLinksMan.SecMan().IsDedicatedBondingAttempted(iDevice.Address()) && iLinkState.Authenticated() && !iLinksMan.LinkManagerProtocol().IsSecureSimplePairingSupportedLocally())
		{
		err = KErrAlreadyExists;
		}
	else
		{
		iRequireAuthenticatedLinkKey = aRequireAuthenticatedLinkKey;
		TRAP(err, iLinksMan.HCIFacade().AuthenticateL(Handle()));
		if(err == KErrNone)
			{
			iLinkKeyReturnedInThisAuthentication = EFalse;
			SetAuthenticationPending(EAuthenticationRequestPending);
			}
		}
	return err;
	}

TInt CPhysicalLink::ChangeLinkKey()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iLinkState.Authenticated(), Panic(EBTPhysicalLinkNotAuthenticated));

	// Check if the peer device can support the link key regeneration procedure.
	if(PeerSupportsLinkKeyRegeneration())
		{
		return iLinksMan.HCIFacade().ChangeConnectionLinkKey(Handle());
		}
	return KErrNone;
	}

TBool CPhysicalLink::PeerSupportsLinkKeyRegeneration() const
	{
	LOG_FUNC
	// Any future manufacturers or manufacturer versions that do not support
	// link key re-generation should be added to this method.

	// See Bluetooth assigned numbers for an explanation of this value.
	return (iRemoteVersion.iManufacturerID != 0x000a);
	}

void CPhysicalLink::ACLDataReceived(THCIConnHandle aConnH, TUint8 aFlag, const TDesC8& aData)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);

	// there may be a race condition in that we have got ACL data but just started
	// shutdown procedures...

	// find the ACL wanting this type of data...
	for (TInt i=iACLLogicalLinks.Count()-1; i>=0; i--)
		{
		iACLLogicalLinks[i]->DataReceived(aConnH, aFlag, aData);
		}
	// else just dump
	}

void CPhysicalLink::SCODataReceived(THCIConnHandle aConnH, const TDesC8& aData)
	{
	LOG_FUNC
	if (iSyncLogicalLink)
		iSyncLogicalLink->DataReceived(aConnH, 0, aData);	// flags ignored

	}

void CPhysicalLink::ConnectionComplete(THCIErrorCode aErr, const TBTConnect& aConn)
	{
	LOG_FUNC
	ConnectionComplete(CHciUtil::SymbianErrorCode(aErr), aConn);
	}

void CPhysicalLink::ConnectionComplete(TInt aResult, const TBTConnect& aConn)
	{
	if (aResult == KErrNone)
		{
		if(aConn.iLinkType == ESCOLink && !iSyncLogicalLink)
			{
			LOG(_L("Got a ConnectionComplete for a non-existant SCO link"))
			//This situation can occur if ESock deletes the iSyncLogicalLink whilst it is waiting for
			//a remote device to respond to a connection request.
			iLinksMan.Baseband().UpdateModelForConnectionError(aConn.iBdaddr, aConn.iLinkType);

			//The baseband might actually have established a SCO link, so send a Disconnect.
			//If no SCO link exists the command will fail gracefully.
			TRAP_IGNORE(iLinksMan.HCIFacade().DisconnectL(aConn.iConnH, ERemoteUserEndedConnection));

			return;
			}

		// update bb model
		iLinksMan.Baseband().UpdateModel(aConn);
		if (aConn.iLinkType == EACLLink)
			{
			// we need to store the physical handle we're on - in BT1.2 this is also taken to be
			// the ACL Link handle too...
			// do the PHY stuff first
			LOG(_L("CPhysicalLink: PHY ready"))

			iHandle = aConn.iConnH;
			iDevice.SetAddress(aConn.iBdaddr);

			//Set Default Local Link Policy;
			//Get this in early to try and avoid conditions where remote requests
			//something we haven't told the controller we allow yet.
			SetModesAllowed(iLinksMan.LinkManagerProtocol().ModesSupportedLocally(),
							iLinksMan.RoleSwitchAllowed());

			// try to get details of remote device
			TRAP_IGNORE(GetRemoteDetailsL(aConn));
			// ignore error - only optimisations

			// assume we are going with 1 slot packets until notified otherwise, and in activemode
			iLinkState.SetLinkState(TBTBasebandLinkState::ELinkUp);
			iLinkState.SetMaxSlots(1);
			iLinkState.SetLinkMode(EActiveMode);

			// If the peer is in security mode 3 then authenication has completed.
			if(iPeerInSecurityMode3)
				{
				iLinkState.SetAuthenticated(ETrue);
				AuthenticationComplete(EAuthenticationRequestPending);
				}

			// update bb model for max slots
			iLinksMan.Baseband().UpdateModel(aConn.iConnH, 1);

			TBTBasebandEventNotification event(ENotifyPhysicalLinkUp);
			NotifyStateChange(event);

			if (aConn.iEncryptMode)
				{
				// pretend there's been an encryption event
				EncryptionChange(EOK, aConn.iConnH, aConn.iEncryptMode);
				}
			}

		// This is assuming that our stack only allows one synchronous link per phy link.
 		// SCO (not eSCO) Link getting notified here.
 		else if(aConn.iLinkType == ESCOLink)
 			{
 			TBTBasebandEventNotification event(ENotifySynchronousLinkUp);
 			NotifyStateChange(event);
 			}

		if (iACLLogicalLinks.Count() == 0)
			{
			// We don't already have an ACL logical link associated with this phy, 
			// so, in order to not miss ACL data from the remote, associate this 
			// phy with an ACL listener if there is one.
			TLogicalLinkListener* listener = iLinksMan.FindListener(EACLLink);
	
			if (listener)
				{
				TBTConnect conn;
		
				conn.iBdaddr = iDevice.Address();
				conn.iCoD = iDevice.DeviceClass().DeviceClass();
				conn.iLinkType = EACLLink;
		
				// Ignore the return from ConnectRequest, if not accepted then we are in no
				// worse situation than if we didn't try.
				ASSERT_DEBUG(listener->iObserver);
				(void)listener->iObserver->ConnectRequest(conn, *this);
				}
			}
		
		// tell the logical links
		NotifyLogicalLinkUp(aConn);
		iLinksMan.ArbitrateAllPhysicalLinks();

		// Check to see if we got a disconnect request during the period before the link was
		// fully established.
		if (iDisconnectRequested)
			{
			__ASSERT_DEBUG(aConn.iLinkType == EACLLink, Panic(EDisconnectRequestedNotOnACLLink));

			// We allow the link to come up fully so that the link is not in a strange state between
			// pending and up. This also means that the notifications to the logical links all work
			// correctly, they just see a link come up and then go down again straight away.
			TRAP_IGNORE(iLinksMan.HCIFacade().DisconnectL(aConn.iConnH, ERemoteUserEndedConnection));
			}
		else
			{
			TTime t;
			t.UniversalTime();
			iDevice.SetUsed(t);
			TRAP_IGNORE(StoreDeviceL(EFalse));
			}
		}
	else
		{
		// error occurred - need to see if it's PHY(and so ACL) or SCO that failed
		// tell logical links
		LOG2(_L("Physical link: connection complete returned an error on handle %d, type %d"), aConn.iConnH, aConn.iLinkType);

		if(((aConn.iLinkType == ESCOLink)||(aConn.iLinkType == EeSCOLink)) && iSyncLogicalLink &&
		iSyncLogicalLink->Handle() != KInvalidConnectionHandle && iSyncLogicalLink->Handle() != aConn.iConnH)
			{
			// This is a secondary SCO link we just rejected or something - don't mess with our own
			// The KInvalidConnectionHandle test ensures that we ignore errored outgoing SCO connects.
			return;
			}

		iLinksMan.Baseband().UpdateModelForConnectionError(aConn.iBdaddr, aConn.iLinkType);
		NotifyLogicalLinkError(aConn.iLinkType, aResult);
		if (aConn.iLinkType == EACLLink)
			{
			// BT 1.2 says that as the ACL Link goes up and down, so does the physical link
			// so if the ACL Link has gone, so has this
			// for SCO we remain in place.
			TBTBasebandEventNotification event(ENotifyPhysicalLinkError, aResult);
			NotifyStateChange(event);
			delete this;
			}
		}
	// ***Watchout*** delete this above: careful about code here
	}


void CPhysicalLink::SynchronousConnectionComplete(THCIErrorCode aErr,
													const TBTConnect& aConn,
													const TBTSyncConnectOpts& aSyncOpts)
	{
	LOG_FUNC
	__ASSERT_DEBUG(((aConn.iLinkType == ESCOLink) || (aConn.iLinkType == EeSCOLink)), Panic(EBTNonSyncConnectInSyncConnectFunc));

	if(!iSyncLogicalLink)
		{
		LOG(_L("Got a SynchronousConnectionComplete when no SCO link is bound"))
		//This situation can occur if ESock deletes the iSyncLogicalLink whilst it is waiting for
		//a remote device to respond to a connection request.
		iLinksMan.Baseband().UpdateModelForConnectionError(aConn.iBdaddr, aConn.iLinkType);

		if(aErr==EOK) // if error, aConn.iConnH will refer to the ACL link used to initialise the SCO link, so dont disconnect that
			{
			//The baseband might actually have established a SCO link, so send a Disconnect.
			//If no SCO link exists the command will fail gracefully.
			TRAP_IGNORE(iLinksMan.HCIFacade().DisconnectL(aConn.iConnH, ERemoteUserEndedConnection));
			}

		return;
		}

	if (aErr == KErrNone)
		{
		// update bb model
		iLinksMan.Baseband().UpdateModel(aConn);

 		// This is assuming that our stack only allows one synchronous link per phy link.
 		// eSCO (not SCO) Link getting notified here.
 		TBTBasebandEventNotification event(ENotifySynchronousLinkUp);
 		NotifyStateChange(event);

		// tell the logical links
		NotifyLogicalSyncLinkUp(aConn, aSyncOpts);
		iLinksMan.ArbitrateAllPhysicalLinks();
		}
	else
		{
		// error occurred - need to see if it's PHY(and so ACL) or SCO that failed
		// tell logical links
		LOG2(_L("Physical link: connection complete returned an error on handle %d, type %d"), aConn.iConnH, aConn.iLinkType);
		iLinksMan.Baseband().UpdateModelForConnectionError(aConn.iBdaddr, aConn.iLinkType);

		// Before passing error around, check if it's actually for this link...
		// A listening link will have a NULL handle, otherwise the handle should match.
		// If the eSCO connection fails, the handle is KInvalidConnectionHandle (as initialised in CBTBasebandSAP)
		// This avoids the situation where we get notified of a rejection.
		if (iSyncLogicalLink &&
			(iSyncLogicalLink->Handle() == 0 || iSyncLogicalLink->Handle() == KInvalidConnectionHandle || iSyncLogicalLink->Handle() == aConn.iConnH))
			{
			NotifyLogicalLinkError(aConn.iLinkType, CHciUtil::SymbianErrorCode(aErr));
			}
		}
	}


void CPhysicalLink::GetRemoteDetailsL(const TBTConnect& aConn)
	{
	LOG_FUNC
	GetDeviceFromRegistryL();
	iLinksMan.HCIFacade().ReadClockOffsetL(aConn.iConnH);
	iLinksMan.HCIFacade().ReadRemoteSupportedFeaturesL(aConn.iConnH);
	iLinksMan.HCIFacade().ReadRemoteVersionL(aConn.iConnH);
	iLinksMan.LinkManagerProtocol().InquiryMgr().ReadRemoteNameL(aConn.iBdaddr);
	}

void CPhysicalLink::LinkSupervisionTimeoutChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewTimeout)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);

	if(aErr == EOK)
		{
		iLSTO = aNewTimeout;
		}
	}

void CPhysicalLink::PacketTypeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewPacket)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);

	if(aErr==EOK)
		{
		iLinksMan.Baseband().UpdateModelIfRecordExists(aConnH, aNewPacket);
		iLinkState.SetPacketTypes(aNewPacket);
		}

	TUint32 eventType = 0;

	if(aNewPacket & EPacketsDM1)
		eventType |= ENotifyPacketsDM1;
	if(aNewPacket & EPacketsDH1)
		eventType |= ENotifyPacketsDH1;
	if(aNewPacket & EPacketsDM3)
		eventType |= ENotifyPacketsDM3;
	if(aNewPacket & EPacketsDH3)
		eventType |= ENotifyPacketsDH3;
	if(aNewPacket & EPacketsDM5)
		eventType |= ENotifyPacketsDM5;
	if(aNewPacket & EPacketsDH5)
		eventType |= ENotifyPacketsDH5;

	if(aNewPacket & EPacketsHV1)
		eventType |= ENotifyPacketsHV1;
	if(aNewPacket & EPacketsHV2)
		eventType |= ENotifyPacketsHV2;
	if(aNewPacket & EPacketsHV3)
		eventType |= ENotifyPacketsHV3;

	TBTBasebandEventNotification event(eventType, aErr);
	NotifyStateChange(event);
	}

void CPhysicalLink::MaxSlotsChange(THCIConnHandle aConnH, TUint8 aSlots)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);
	LOG2(_L("Connection Handle 0x%04x, using %d slots"), aConnH, aSlots);
	iLinksMan.Baseband().UpdateModel(aConnH, aSlots);	//event only for ACL
	iLinkState.SetMaxSlots(aSlots);

	TBTBasebandEventNotification event;
	switch(aSlots)
		{
		case 1:
			event.SetEventType(ENotifyMaxSlots1);
			break;
		case 3:
			event.SetEventType(ENotifyMaxSlots3);
			break;
		case 5:
			event.SetEventType(ENotifyMaxSlots5);
			break;
		};
	NotifyStateChange(event);
	}

void CPhysicalLink::ModeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TBTLinkMode aMode, TBasebandTime aInterval)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);
	LOG3(_L("Connection Handle 0x%04x: ModeChange event %d->%d"), aConnH, iLinkState.LinkMode(), aMode);

	TBTLinkMode oldMode = iLinkState.LinkMode();

	iLinkState.SetLinkMode(aMode);	// remember the state for ourselves

	if (aErr == EOK)
		{
		if (aMode == EParkMode)
			{
			iLinksMan.Baseband().ParkLink(aConnH);	//remove from model whilst parked
			}

		if (aMode == EActiveMode && oldMode==EParkMode)
			{
			// unparking we must tell the DataQ - we could always get the dataQ to ask us before sending of course
			// but that might be performance harming
			iLinksMan.Baseband().UnParkLink(aConnH); //NB Max Slots = 1 default
			}

		if (aMode == ESniffMode )
			{
			// Store the Sniff Interval
			iSniffInterval = aInterval;
			}

		if (aMode == EActiveMode && iSniffInterval)
			{
			//Active mode enable and SniffInterval is set.
			//Therefore must be leaving sniff mode.
			__ASSERT_DEBUG(oldMode==ESniffMode, Panic(EBTPhysicalLinkModeChangeErrorLeavingSniffMode));

			//When not in sniff mode, sniff interval is obviously 0!
			iSniffInterval = 0;
			}
		}

	// Generate a baseband event, and offer it to the proxySAP's associated
	// with this link.
	TBTBasebandEventNotification event;
	switch(aMode)
		{
		case EActiveMode:
			event.SetEventType(ENotifyActiveMode);
			break;
		case ESniffMode:
			event.SetEventType(ENotifySniffMode);
			break;
		case EHoldMode:
			event.SetEventType(ENotifyHoldMode);
			break;
		case EParkMode:
			event.SetEventType(ENotifyParkMode);
			break;
		case EScatterMode:
			break; // shouldnt be seen yet - if so, ignore for now
		};
	event.SetErrorCode(aErr);
	NotifyStateChange(event);
	}

void CPhysicalLink::RoleChange(THCIErrorCode aErr, const TBTDevAddr& /*aAddr*/, TBTBasebandRole aRole)
	{
	LOG_FUNC
	LOG1(_L("CPhysicalLink: Role change - new role %S"), aRole == EMaster ? _S("Master") : _S("Slave"));

	if (aErr == EOK)
		{
		iLinkState.SetLinkRole(aRole);
		}

	// Generate a baseband event, and offer it to the proxySAP's associated
	// with this link.
	TBTBasebandEventNotification event;
	if(aRole == EMaster)
		{
		event.SetEventType(ENotifyMaster);
		}
	else
		{
		event.SetEventType(ENotifySlave);
		}
	event.SetErrorCode(aErr);
	NotifyStateChange(event);

	// if iEncryptionEnforcer is present it means we asked for encryption, so we inform it
	// about the roleswitch
	if (iEncryptionEnforcer)
		{
		iEncryptionEnforcer->RoleSwitchEvent();
		}
	}

void CPhysicalLink::WriteLinkPolicySettingsCompleteEvent(THCIErrorCode aErr, THCIConnHandle aConnH)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);
	// we don't get the things back we set...
	if (aErr != EOK)
		{
		// The command has failed.	Clear the cached values and re-arbitrate.
		iLinkPolicy.SetModesAllowed(0);
		iLinkPolicy.SetSwitchAllowed(EFalse);
		Arbitrate();
		}
	}

void CPhysicalLink::ClockOffset(THCIErrorCode aErr, THCIConnHandle aConnH, TBasebandTime aClockOffset)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);
	if (aErr==EOK)
		{
		// tell inquiry manager - we found this information to help it
		iLinksMan.LinkManagerProtocol().InquiryMgr().ClockOffsetResult(BDAddr(), aClockOffset);
		}
	// we don't do anything more useful knowing this information
	}

void CPhysicalLink::RemoteName(THCIErrorCode aErr, const TBTDevAddr& /*aAddr*/, const TBTDeviceName8& aName)
/**
	We don't keep this, but we're interested in persisting it in el Reg
	By receiving this we now our attempts have completed in getting name
*/
	{
	LOG_FUNC
	if (aErr==EOK)
		{
		// slam it into the Registry - this will be useful to UIs if not us
		TRAP_IGNORE(DoUpdateNameL(aName));
		}
	}

void CPhysicalLink::DoUpdateNameL(const TBTDeviceName8& aName)
	{
	LOG_FUNC
	CBTDeviceNameChanger* nameChanger = CBTDeviceNameChanger::NewL(iRegSess, *this);
	iRegistryHelpers.AddLast(*nameChanger);
	nameChanger->Start(BDAddr(), aName);
	}

void CPhysicalLink::EncryptionKeyRefreshComplete(THCIErrorCode aErr, THCIConnHandle aConnH)
	{
	LOG_FUNC
	__CHECK_CONNECTION_HANDLE(aConnH);

	if(iKeyRefresher)
		{
		iKeyRefresher->EncryptionKeyRefreshComplete(aErr);
		}
	}

void CPhysicalLink::Disconnection(THCIErrorCode aErr, THCIConnHandle aConnH, THCIErrorCode aResult)
	{
	LOG_FUNC
	// tell our socket if it's our handle - otherwise the SCO SAP

	LOG1(_L("Physical link: disconnection on handle %d"), aConnH);
	LOG1(_L(" ACL link handle:	 %d"), Handle());
#ifdef __FLOG_ACTIVE
	if (iSyncLogicalLink) LOG1(_L(" Sync link handle: %d"), iSyncLogicalLink->Handle());
#endif

	//no matter if it is the socket or SCO SAP the ui dialogs will be cancelled
	if (iNumericComparator && iNumericComparator->IsActive())
		{
		iNumericComparator->Cancel();
		}
	if (iPasskeyEntry && iPasskeyEntry->IsActive())
		{
		iPasskeyEntry->Cancel();
		}

	if (aConnH == Handle())
		{
		TBTBasebandEventNotification event;
		switch(aErr)
			{
			case EHardwareFail:
				{

				if(iLinkState.LinkState() == TBTBasebandLinkState::ELinkUp ||
				   iLinkState.LinkState() == TBTBasebandLinkState::ELinkPending)
					{
					//Only propagate if link is up or pending, otherwise
					//we may get disconnection in unexpected states

					//Fall through
					}
				else
					{
					// this object must go when no link
					delete this;
					break;
					}
				}
			case EOK:
				{
				iLinkState.SetLinkState(TBTBasebandLinkState::ELinkDown);

				// PHY went down - so have logical links then
				NotifyLogicalLinkDown(EACLLink);
				NotifyLogicalLinkDown(ESCOLink);

				event.SetEventType(ENotifyPhysicalLinkDown);
				event.SetErrorCode(aResult);
				iLinksMan.Baseband().UpdateModelForDisconnection(aConnH, EACLLink);
				HandlePrefetch();
				NotifyStateChange(event);
				// this object must go when no link
				delete this;
				break;
				}
			case ECommandDisallowed:
				{
				// The connection is still up, so do nothing, and wait for the next idle timer to fire
				break;
				}
			default:
				{
				iLinkState.SetLinkState(TBTBasebandLinkState::ELinkDown);

				// there's an error
				NotifyLogicalLinkError(EACLLink, aErr);
				NotifyLogicalLinkError(ESCOLink, aErr);
				event.SetEventType(ENotifyPhysicalLinkError);
				event.SetErrorCode(aErr);
				// baseband model needs to err on the side of least bandwidth usage -
				// a link COULD have come down
				iLinksMan.Baseband().UpdateModelForDisconnection(aConnH, EACLLink);
				HandlePrefetch();
				NotifyStateChange(event);
				// this object must go when no link
				delete this;
				break;
				}
			}
		}

	else if (iSyncLogicalLink && aConnH == iSyncLogicalLink->Handle())
		{
		TBTBasebandEventNotification event;
		// just the SCO link is down so we'd better notify
		if (aErr)
			{
			LOG(_L("Physical link: error disconnection on sync link"));
			TLinkType type = iSyncLogicalLink->LinkType();
			iSyncLogicalLink->Error(CHciUtil::SymbianErrorCode(aErr));
			event.SetEventType(ENotifySynchronousLinkError);
			event.SetErrorCode(aErr);
			// baseband model needs to err on the side of least bandwidth usage -
			// a sync link COULD have come down
			iLinksMan.Baseband().UpdateModelForDisconnection(aConnH, type);
			}
		else
			{
			LOG(_L("Physical link: normal disconnection on sync link"));
			TLinkType type = iSyncLogicalLink->LinkType();
			iSyncLogicalLink->Disconnection();
			event.SetEventType(ENotifySynchronousLinkDown);
			event.SetErrorCode(aResult);
			iLinksMan.Baseband().UpdateModelForDisconnection(aConnH, type);
			}

		NotifyStateChange(event);
		}
	// NOTE!! delete this above - careful about adding code
	}

void CPhysicalLink::ConnectionRequest(const TBTConnect& aConn)
	{
	LOG_FUNC
	// we must be page scan enable for this to come in
	// but still need to see if it got routed a listening SAP
	TBool accept = EFalse;
	TLogicalLinkListener* listener = NULL;

	__ASSERT_DEBUG(iPendingConnection == EFalse, Panic(EPendingConnectionNotCleared));

	switch (aConn.iLinkType)
		{
		case ESCOLink:
		case EeSCOLink:
			if (!iSyncLogicalLink)
				{
				// If we already have a sync link active, reject request.
				LOG(_L("No current link, checking for listener"));
				listener = iLinksMan.FindListener(aConn.iLinkType);
				}
			break;

		default:
			listener = iLinksMan.FindListener(aConn.iLinkType);
			break;
		}

	if (listener)
		{
		accept = ETrue;
		if (iLinksMan.IsAcceptPairedOnlyMode())
			{
			// do not accept if we are only accepting connection
			// requests from paired devices and we are not already
			// paired with the remote
			CBTPairingsCache::TPairingState pairingState = iLinksMan.PairingsCache().IsPaired(aConn.iBdaddr);

			if(pairingState == CBTPairingsCache::EDeferred)
				{
				// We're still waiting for the Pairing Caches paired device list to be filled.
				// We'll respond when this is complete, so store details away for then.
				LOG(_L("CPhysicalLink: Waiting for physical link manager's paired device list from Registry!"))
				iPendingConnection = ETrue;
				iLastPendingConnection.iConnH = aConn.iConnH;
				iLastPendingConnection.iBdaddr = aConn.iBdaddr;
				iLastPendingConnection.iCoD = aConn.iCoD;
				iLastPendingConnection.iLinkType = aConn.iLinkType;
				iLastPendingConnection.iEncryptMode = aConn.iEncryptMode;
				// Return now as we are waiting and don't want to 'RejectConnection'
				return;
				}

			// If here then the cache has either informed us that the device is paired
 			// or not.  We only accept paired connections.
 			accept = (pairingState == CBTPairingsCache::EPaired) ? ETrue : EFalse;
			}
		if (accept)
			{
			// this may mean the physical link's acceptance is determined by the
			// ACL Logical link - but that's what the spec implies :-)
			accept = static_cast<MLogicalLink*>(listener->iObserver)->ConnectRequest(aConn, *this);
			// since we only support one SCO listener at present we dont need to mark it as the acceptor
			// or indeed find a specific one: later we could choose via CoD
			}
		}

	if (accept)
		{

		// store CoD now, not told it ever again! if connection fails this object will go
		// Don't update CoD if it is 0 (NULL) as this is the default.
		// This also prevents a valid CoD being overwritten.
		if (aConn.iCoD != 0)
			{
			iDevice.SetDeviceClass(aConn.iCoD);
			}

		TUint8 roleSwitch = static_cast<TUint8>(iLinksMan.PassiveConnectBecomeMaster() ? 0x00 : 0x01);

		TInt err;
		if (aConn.iLinkType == EeSCOLink)
			{
			TBTeSCOLinkParams options;

			CeSCOLink* eSCO = static_cast<CeSCOLink*>(listener->iObserver);
			TUint16 packetMask = eSCO->GetPacketMask();
			eSCO->GetExtOptions(options);

			TRAP(err, iLinksMan.HCIFacade().AcceptSynchronousConnectionL(aConn.iBdaddr,
				options.iBandwidth.iTransmit, options.iBandwidth.iReceive,
				options.iLatency, options.iCoding, options.iRetransmissionEffort,
				packetMask
				));
			}
		else
			{
			TRAP(err, iLinksMan.HCIFacade().AcceptConnectionRequestL(aConn.iBdaddr, roleSwitch));
			}

		if (err == KErrNone)
			{
			// assume we're slave until told otherwise on ACL(=PHY) links
			if (aConn.iLinkType == EACLLink)
				{
				// this is a PHY connect request too, so we should store our role
				//The handle in 'aConn' is bogus, so..
				iLinkState.SetLinkState(TBTBasebandLinkState::ELinkPending);
				iLinksMan.Baseband().UpdateModel(aConn.iBdaddr, KHCIDefaultPacketType, aConn.iLinkType);
				iLinkState.SetLinkRole((roleSwitch == 0x01) ? ESlave : EMaster);

				// have to store it now, since the complete wont tell us anything!
				}
			else
				{
				//The handle in 'aConn' is bogus, so..
				iLinksMan.Baseband().UpdateModel(aConn.iBdaddr, KHCIDefaultSCOPacketType, aConn.iLinkType);
				}
			}
		else
			{
			// Out of memory:
			// Cause newly created spawned SAP (whether ACL or SCO)
			// to die.
			// Attempt to reject connection with newly freed memory...
			// ...this of course may fail in which case we have
			// to rely on supervison timeouts.
			if (aConn.iLinkType == EACLLink)
				{
				TInt last = iACLLogicalLinks.Count() - 1;
				if(last>=0)
					{
					iACLLogicalLinks[last]->Error(err);
					}
				}
			else
				{
				if(iSyncLogicalLink)
					{
					iSyncLogicalLink->Error(err);
					}
				}
			RejectConnection(aConn);
			}
		}
	else
		{
		// reject
		RejectConnection(aConn);
		}

	// Tell inquiry manager - should have a valid CoD within 'aConn'
	// Don't update CoD if it is 0 (NULL) as this is the default.
	// This also prevents a valid CoD being overwritten.
	if (aConn.iCoD != 0)
		{
		iLinksMan.LinkManagerProtocol().InquiryMgr().CoDResult(aConn.iBdaddr, aConn.iCoD);
		}
	}

void CPhysicalLink::RejectConnection(const TBTConnect& aConn)
	{
	LOG_FUNC
	if (aConn.iLinkType == EeSCOLink)
		{
		TRAP_IGNORE(iLinksMan.HCIFacade().RejectSynchronousConnectionL(aConn.iBdaddr, KDefaultRejectReason));
		}
	else
		{
		TRAP_IGNORE(iLinksMan.HCIFacade().RejectConnectionRequestL(aConn, KDefaultRejectReason));
		}
	}


void CPhysicalLink::GetDeviceFromRegistryL()
	{
	LOG_FUNC
	CBTDeviceGetter* getter = CBTDeviceGetter::NewL(iRegSess, *this);
	iRegistryHelpers.AddLast(*getter);
	getter->Start(BDAddr());		// now get the rest of the details
	// just BDAddr?
	}

TInt CPhysicalLink::GetOption(TUint aLevel,TUint aName,TDes8& aOption) const
	{
	LOG_FUNC
	// good stuff here!
	if (aLevel == KSolBtLM)
		{
		switch (aName)
			{
			case KLMGetBasebandHandle:
				{
				if (aOption.Length() != sizeof(THCIConnHandle))
					{
					return KErrArgument;
					}
				aOption = TPtrC8(reinterpret_cast<const TUint8*>(&iHandle), sizeof(THCIConnHandle));
				return KErrNone;
				}

			case EBBGetSniffInterval:
				if (aOption.Length() != sizeof(TBasebandTime))
					{
					return KErrArgument;
					}
				aOption = TPtrC8(reinterpret_cast<const TUint8*>(&iSniffInterval), sizeof(TBasebandTime));
				return KErrNone;

			default:
				return KErrNotSupported;
			}
		}
	else
		{
		return KErrNotSupported;
		}
	}

TInt CPhysicalLink::Arbitrate(TBool aImmediately, TBool aLocalPriority)
	{
	LOG_FUNC
	if (!IsConnected())
		{
		LOG(_L8("Physical link not connected, no arbitration executed"));
		return KErrDisconnected;
		}
	// The arbitration delay object will decide how much delay
	return iArbitrationDelay->Start(aImmediately, aLocalPriority);
	}

TInt CPhysicalLink::DoArbitrate(TBool aLocalPriority)
	{
	LOG_FUNC
	if (!IsConnected())
		{
		LOG(_L8("Physical link not connected, no arbitration executed"));
		return KErrDisconnected;
		}

	//start arbitrate process with what our local controller supports
	TUint8 allowedModesMask = EHoldMode | EParkMode | ESniffMode; // local features sorted out later
	TBool roleSwitchAllowed = iLinksMan.LinkManagerProtocol().IsRoleSwitchSupportedLocally() && iLinksMan.RoleSwitchAllowed();
	LOG2(_L8("Arbitration: link policy (LPM:0x%02x, Role:0x%x) - Prior to proxies"), allowedModesMask, roleSwitchAllowed);
	
	// ask proxies what they want from the PHY
	TUint16 requestedModeMask = 0; // mask of current LPM requests from proxy's
	static const TUint16 KExplicitActiveMode = 0x0100; // special bit for explicit active mode requests
	
	TSglQueIter<CBTProxySAP> iter(iProxySAPs);
	while (iter)
		{
		CBTProxySAP* proxy = iter++;

		TUint8 requestedMode = proxy->GetRequestedModes();
		requestedModeMask |= requestedMode;

		TBool explicitActiveModeRequest = proxy->RequestedActiveMode();
		if (requestedMode == EActiveMode && explicitActiveModeRequest)
			{
			requestedModeMask |= KExplicitActiveMode;
			}

		TUint8 allowedModes = proxy->GetAllowedModes();
		allowedModesMask &= allowedModes;
		
		TBool roleSwitchAllowedByProxy = proxy->IsRoleSwitchAllowed();
		roleSwitchAllowed = roleSwitchAllowed && roleSwitchAllowedByProxy;
		
		LOG4(_L8("Arbitration: Proxy(0x%08x) - requested mode = 0x%04x, link policy (LPM:0x%02x, Role:0x%x)"), proxy, requestedMode, allowedModes, roleSwitchAllowedByProxy);
		}
	LOG2(_L8("Arbitration: link policy (LPM:0x%02x, Role:0x%x) - after proxies"), allowedModesMask, roleSwitchAllowed);

	// clear out modes not supported by local Controller
	// Future improvement - what about modes supported by the remote device?
	allowedModesMask &= iLinksMan.LinkManagerProtocol().ModesSupportedLocally();
	LOG2(_L8("Arbitration: link policy (LPM:0x%02x, Role:0x%x) - only supported modes"), allowedModesMask, roleSwitchAllowed);

	if(iOverrideParkRequests)
		{
		// We wish to ensure the physical link is not in PARK mode.
		// The only way to guarantee this is to disallow PARK via the link policy settings.
		allowedModesMask &= ~EParkMode;
		}
	LOG2(_L8("Arbitration: link policy (LPM:0x%02x, Role:0x%x) - overrides applied"), allowedModesMask, roleSwitchAllowed);

	// Controller policy for the connection may need updating
	SetModesAllowed(allowedModesMask, roleSwitchAllowed);
	LOG2(_L8("Arbitration: link policy (LPM:0x%02x, Role:0x%x) - submitted"), allowedModesMask, roleSwitchAllowed);

	//If OverrideLPM flag is set, we do not disable LP modes via the link policy settings
	//This is done because OverrideLPM should not prevent remotes putting us into an LPM
	//Later on, when OverrideLPM flag is cancelled, it would allow us to enforce requested LPM
	if(iOverrideLPMRequests)
		{
		// We need to ensure the physical link is in active mode.
		allowedModesMask = EActiveMode;
		}
	LOG2(_L8("Arbitration: link policy (LPM:0x%02x, Role:0x%x) - post setting overrides applied"), allowedModesMask, roleSwitchAllowed);

	TUint16 modeChangeMask = requestedModeMask & (static_cast<TUint16>(allowedModesMask)|KExplicitActiveMode);
	TUint16 modeCompareMask = 0;
	LOG2(_L8("Arbitration: mode change mask = 0x%04x, local priority = 0x%x"), modeChangeMask, aLocalPriority);

	if(aLocalPriority)
		{
		// If we want local priority, we go with what we want to do
		// irrespective of what the remote may have previously requested.
		modeCompareMask = modeChangeMask;
		}
	else
		{
 		// This logic allows us to see if the current low power mode has recently
 		// changed NOT because of a change in the local proxies' requests, but (probably)
 		// because a remote device has changed it..

		// modeCompareMask should start only having zero bits where
		// requestedModeMask has a zero bit and iPreviousRequestedModeMask does not
		// i.e. a mode is newly no longer requested.
		modeCompareMask = ~((requestedModeMask ^ iPreviousRequestedModeMask) & iPreviousRequestedModeMask);

		// Remove bits from modeCompareMask that are not in allowedModesMask
		// We cannot stay in a power mode that we do not allow.
		modeCompareMask &= (static_cast<TUint16>(allowedModesMask)|KExplicitActiveMode);
		}
	LOG1(_L8("Arbitration: Comparison mask = 0x%04x"), modeCompareMask);

	iPreviousRequestedModeMask = requestedModeMask; // Update previous requested to current value.

	// get the current mode.
	TBTLinkMode currentMode = iLinkState.LinkMode();
	TUint16 currentModeMask = static_cast<TUint16>(currentMode);
	if(currentModeMask == EActiveMode)
		{
		// if in active mode then could have been because of an explicit active mode request
		currentModeMask |= KExplicitActiveMode;
		}
	LOG1(_L8("Arbitration: Current mode mask = 0x%04x"), currentModeMask);
	
	if(modeCompareMask & currentModeMask)
		{
		LOG2(_L8("Arbitration: Comparison mask (0x%04x) matched, so staying in current mode (0x%04x)"), modeCompareMask, currentModeMask);
		// The current state is the same as the permitted required role(s).
		return KErrNone;
		}
	
	TBTLinkMode targetMode = EActiveMode;
	// Determine which LPM we should be in (if any)
	if(modeChangeMask & KExplicitActiveMode)
		{
		targetMode = EActiveMode;
		}
	else if(modeChangeMask & EHoldMode)
		{
		targetMode = EHoldMode;
		}
	else if(modeChangeMask & ESniffMode)
		{
		targetMode = ESniffMode;
		}
	else if(modeChangeMask & EParkMode)
		{
		targetMode = EParkMode;
		}
	LOG2(_L8("Arbitration: Arbitrating mode 0x%02x -> 0x%02x"), currentMode, targetMode);
	
	if(IsConnected())
	    {
        TInt err = iLowPowModeCtrl.ExecuteModeChange(targetMode);
        LOG1(_L8("Arbitration: iLowPowModeCtrl.ExecuteModeChange: err:%d"), err);
        return err;
	    }
	else
	    {
        return KErrDisconnected;
	    }
	}

void CPhysicalLink::SetPassKey(const TDesC8& aPassKey)
	{
	LOG_FUNC
	// We store the key for use if it succeeds.
	iNewPinCode.Copy(aPassKey);
	iNewPinCodeValid = ETrue;
	}

const TBTPinCode& CPhysicalLink::PassKey() const
	{
	LOG_FUNC
	return iDevice.PassKey();
	}

void CPhysicalLink::StartArbitrationTimer() const
	{
	LOG_FUNC
	iArbitrationDelay->Restart();
	}

void CPhysicalLink::Connect(TBasebandPageTimePolicy aPolicy)
	{
	LOG_FUNC
	// assume that we will be master until told otherwise
	ASSERT_DEBUG(!IsConnected());

	UpdateFromInquiryCache();

	TUint8 psrm = iDevice.IsValidPageScanRepMode() ? iDevice.PageScanRepMode() : TUint8(KDefaultBluetoothPageScanRepMode);
	TUint8 psm = iDevice.IsValidPageScanMode() ? iDevice.PageScanMode() : TUint8(KDefaultBluetoothPageScanMode);
	TUint16 clockOffset = iDevice.IsValidClockOffset() ? iDevice.ClockOffset() : TUint16(KDefaultBluetoothClockOffset);

	TUint8 allowRoleSwitch = static_cast<TUint8>(iLinksMan.ActiveConnectRoleSwitchAllowed());

	TUint16 pkt = KHCIDefaultPacketType;

	// optimise paging (as a best-effort attempt).
	TBasebandTime pagetimeout = CalculatePageTimeout(aPolicy, psrm, clockOffset & KHCIClockOffsetValidBit);
	iLinksMan.TryToChangePageTimeout(pagetimeout);
	
	// Set state in anticipation of the connection
	iLinkState.SetLinkState(TBTBasebandLinkState::ELinkPending);
	iLinksMan.Baseband().UpdateModel(iDevice.Address(), pkt, EACLLink);
	iLinkState.SetLinkRole(EMaster);

	TRAPD(ret, iLinksMan.HCIFacade().ConnectL(iDevice.Address(), pkt, psrm, psm, clockOffset, allowRoleSwitch));
	if(ret != KErrNone) // a physical link is in charge of it's own destiny.
		{
		TBTConnect conn;
		conn.iBdaddr = BDAddr();
		conn.iLinkType = EACLLink;
		ConnectionComplete(ret, conn);
		}
	}

TInt CPhysicalLink::SCOConnect()
/**
	A utility service provided to the SCO transport
	We bring up a "default" SCO link, choosing the packet based on what we
	know the controllers can support
*/
	{
	LOG_FUNC
	return SCOConnect(EAnySCOPacket);
	}

TInt CPhysicalLink::SCOConnect(const TUint16 aUserHVPacketTypes)
/**
	A utility service provided to the SCO transport
	We bring up a SCO link which allows only the user specified packet types
	Implementation of SCOSAP::SetOption guarantees aUserHVPacketTypes contains at
	least one valid SCO packet type
*/
	{
	LOG_FUNC
	TBTSCOPackets scoPackets = iLinksMan.LinkManagerProtocol().PacketsSupportedLocally().SCOPackets();

	scoPackets &= aUserHVPacketTypes;
	// combine with remote supported packets
	scoPackets &= iRemoteFeatures.SCOPackets();

	TInt ret;

	if (!scoPackets)
		ret = KErrNotSupported;
	else
		{
		// we put all in for now - if HW doesn't support that, the HCI can be changed
		OverridePark();

		TRAP(ret, iLinksMan.HCIFacade().SCOConnectL(Handle(), scoPackets, BDAddr()));
		if(ret==KErrNone)
			{
			iLinksMan.Baseband().UpdateModel(Handle(), EACLLink, scoPackets, ESCOLink);
			}
		}
	return ret;
	}

TInt CPhysicalLink::SynchronousConnect(TUint aTransmitBandwidth, TUint aReceiveBandwidth,
		TUint16 aMaxLatency, TUint16 aVoiceSettings,
		TUint8 aRetransmissionEffort, const TBTSyncPacketTypes aUserPacketTypes)
/**
	A utility service provided to the eSCO transport
	We bring up a eSCO link which allows only the user specified packet types
	Implementation of SCOSAP::SetOption guarantees aUserPacketTypes contains at
	least one valid SCO packet type
*/
	{
	LOG_FUNC
	TBTSyncPacketTypes escoPackets = iLinksMan.LinkManagerProtocol().PacketsSupportedLocally().SyncPackets();

	escoPackets &= aUserPacketTypes;
	// combine with remote supported packets
	escoPackets &= iRemoteFeatures.SyncPackets();

	TInt ret;

	if (!escoPackets)
		ret = KErrNotSupported;
	else
		{
		// we put all in for now - if HW doesn't support that, the HCI can be changed
		// THCIConnHandle aACLHandle, TUint aTransmitBandwidth, TUint aReceiveBandwidth,
		//	TUint16 aMaxLatency, TUint16 aVoiceSettings, TUint8	 aRetransmissionEffort,
		//	TUint16 aPacketTypeMask
		TRAP(ret, iLinksMan.HCIFacade().SetupSynchronousConnectionCommandL(
			Handle(),
			aTransmitBandwidth, aReceiveBandwidth,
			aMaxLatency, aVoiceSettings, aRetransmissionEffort,
			escoPackets,
			BDAddr()
			));
		if(ret==KErrNone)
			{
			iLinksMan.Baseband().UpdateModel(Handle(), EACLLink, escoPackets, EeSCOLink);
			}
		}
	return ret;
	}

TInt CPhysicalLink::SubscribeProxySAP(CBTProxySAP& aProxySAP)
	{
	LOG_FUNC
	// a new Proxy SAP wishes to attach to this physical SAP
#ifdef _DEBUG
	// check to see if the same proxy is already there
	TSglQueIter<CBTProxySAP> iter(iProxySAPs);
	while (iter)
		{
		ASSERT_DEBUG(iter++!=&aProxySAP);
		}
#endif
	SubscribeLinkObserver(aProxySAP);
	iProxySAPs.AddLast(aProxySAP);
	Arbitrate();
	return KErrNone;
	}

void CPhysicalLink::UnsubscribeProxySAP(CBTProxySAP& aProxySAP)
	{
	LOG_FUNC
	ASSERT_DEBUG(!iProxySAPs.IsEmpty());
	iProxySAPs.Remove(aProxySAP);
	UnsubscribeLinkObserver(aProxySAP);
	Arbitrate(); // good time to see what phy properties should be used
	PhysicalLinkUserIdle();
	}


TInt CPhysicalLink::AttachLogicalLink(TLinkType aLink, CBTBasebandSAP& aSAP)
	{
	// effectively this part of this class encapsulates the Logical Transports
	// but they're not really worthy of a class - just some understanding that they're
	// not ignored
	LOG_FUNC
	TInt retVal = KErrNone;

	switch (aLink)
		{
		case EACLLink:
			{
			// it is important for error handling at the end of
			// CPhysicalLink::ConnectionRequest that this remains an
			// append i.e. 'aSAP' is added to the end of the queue
			retVal = iACLLogicalLinks.Append(static_cast<CACLLink*>(&aSAP));
			break;
			}

		case ESCOLink:
			{
			if (!(iRemoteFeatures.IsSCOLinkSupported()))
				retVal = KErrNotSupported;
			else
				{
				// Don't attach SCO if already exists, avoids confusion with some h/w.
				// .....return the outcome, higher levels depend on this action
				if (iSyncLogicalLink)
					{
					retVal = KErrInUse;
					}
				else
					{
					iSyncLogicalLink = static_cast<CSCOLink*>(&aSAP);
					}
				}
			break;
			}

		case EeSCOLink:
			{
			if (!(iRemoteFeatures.IsExtendedSCOLinkSupported()))
				retVal = KErrNotSupported;
			else
				{
				if (iSyncLogicalLink)
					{
					retVal = KErrInUse;
					}
				else
					{
					iSyncLogicalLink = static_cast<CeSCOLink*>(&aSAP);
					}
				}
			break;
			}

		default:
			Panic(EBTUnknownLogicalLink);
		}

	if (retVal == KErrNone)
		{
		RemoveIdleTimer();
		}

	return retVal;
	}

void CPhysicalLink::DetachLogicalLink(TLinkType aLink, CBTBasebandSAP& aSAP)
	{
	LOG_FUNC
	LOG3(_L("CPhysicalLink: Logical Link type %d, 0x%08x detaching from PHY 0x%08x"), aLink, &aSAP, this);
	switch (aLink)
		{
		case EACLLink:
			{
#ifdef _DEBUG
			TInt numFound = 0;
#endif
			for (TInt i=iACLLogicalLinks.Count()-1; i>=0; i--)
				{
				if (iACLLogicalLinks[i] == &aSAP)
					{
#ifdef _DEBUG
					numFound++;
#endif
					iACLLogicalLinks.Remove(i); // don't delete - it's not ours!
					}
				}
			__ASSERT_DEBUG(numFound==1, Panic(EBTACLLogicalLinkBadDebind));
			break;
			}

		case ESCOLink:
		case EeSCOLink:
			{
			__ASSERT_DEBUG(iSyncLogicalLink==&aSAP, Panic(EBTSCOLogicalLinkBadDebind));
			iSyncLogicalLink = NULL;
			break;
			}

		default:
			Panic(EBTUnknownLogicalLink);
		}
	}

void CPhysicalLink::PhysicalLinkUserIdle()
	{
	LOG_FUNC
	if(IsPhysicalLinkIdle() && iLinkState.LinkState() == TBTBasebandLinkState::ELinkUp)
		{
		// try to stash device details - juice and last used date
		TTime t;
		t.UniversalTime();
		iDevice.SetUsed(t);
		TRAPD(ignore, StoreDeviceL(ETrue)); //ETrue: new meaning - prevent addition of device in registry

		if (!iIdleTimerQueued)
			// we don't want to send the read clock offset command more than one
			// here, so just check if we already passed from here.
			{
			// Take this opportunity to get latest info on clock offset
			// so that we can stash it in HR - subsequent connections may then be quicker
			TRAP(ignore, iLinksMan.HCIFacade().ReadClockOffsetL(iHandle));
			// ignore error - was an optimisation

			// Disconnect the PHY, it is no longer required.
			QueueIdleTimer(KPhysicalLinkIdleTimeout);
			}
		}
	}


TBool CPhysicalLink::IsPhysicalLinkIdle() const
	{
	LOG_FUNC
	TBool physicalLinkIdle = ETrue;
	// Check ACL and SCO links.	 If any link is not idle, then the PHY remains
	// active.
	for (TInt i=iACLLogicalLinks.Count()-1; i>=0; i--)
		{
		physicalLinkIdle &= iACLLogicalLinks[i]->IsIdle();
		}

	if(iSyncLogicalLink)
		{
		physicalLinkIdle &= iSyncLogicalLink->IsIdle();
		}

 	if (iPinRequester)
 		{
 		physicalLinkIdle = EFalse;
 		}

 	if (IsAuthenticationPending())
 		{
 		physicalLinkIdle = EFalse;
 		}

	return physicalLinkIdle;
	}


void CPhysicalLink::QueueIdleTimer(TInt aTime)
/**
	Queue idle timer entry.
	When this timer expires, it'll call TryToClose, which actually
	causes the thing to finally close down.
**/
	{
	LOG_FUNC
	if (!iIdleTimerQueued)
		{
		TCallBack cb(TerminateCallback, this);
		iIdleTimerEntry.Set(cb);
		iIdleTimeout = aTime;
		BTSocketTimer::Queue(iIdleTimeout * 1000000, iIdleTimerEntry);
		iIdleTimerQueued = ETrue;
		}
	}


void CPhysicalLink::RemoveIdleTimer()
/**
	Called whenever we're opened.
	Checks there are no idle timer entries queued.
**/
	{
	LOG_FUNC
	if (!iIdleTimerQueued)
		{
		// it's fine for callers to just try to remove the timer
		return;
		}
	BTSocketTimer::Remove(iIdleTimerEntry);
	iIdleTimerQueued = EFalse;
	}

void CPhysicalLink::NotifyStateChange(TBTBasebandEventNotification& aEvent)
	{
	LOG_FUNC
	// If this event is 'physical link down' / 'physical link error' then any ProxySAP
	// subscribers will unsubscribe as a result of PhysicalLinkChange being called.
	// This will result in the entry being removed from the iBasebandSubscribers array.
	// Consequently the array must be traversed from end to start.
	TDblQueIter<TPhysicalLinkObserverQLink> iter(iBasebandSubscribers);
	TPhysicalLinkObserverQLink* l;
	while ((l=iter++)!=NULL)
		{
		l->Item()->PhysicalLinkChange(aEvent, *this);
		}

	if(((aEvent.EventType() & ENotifyActiveMode) ||
		(aEvent.EventType() & ENotifySniffMode)||
		(aEvent.EventType() & ENotifyParkMode)||
		(aEvent.EventType() & ENotifyHoldMode)) &&
		(aEvent.ErrorCode() == KErrNone))
		{
		iArbitrationDelay->Restart();
		}
	}

/*static*/ TInt CPhysicalLink::TerminateCallback(TAny* aCPhysicalLink)
	{
	LOG_STATIC_FUNC
	CPhysicalLink* c = reinterpret_cast<CPhysicalLink*>(aCPhysicalLink);
	c->iIdleTimerQueued = EFalse;

	TInt retVal = EFalse;
	
	// Queue another idle timer in case this disconnect fails
	
	// Check if the physical link is still idle after the timer.
	if(c->IsPhysicalLinkIdle() && c->iLinkState.LinkState() == TBTBasebandLinkState::ELinkUp)
		{
		// Increase the idle time for the next callback by just over 50%
		TInt newIdleTime = (c->iIdleTimeout * 3)/2 + 1;
		if (newIdleTime > KMaxPhysicalLinkIdleTimeout)
			{
			newIdleTime = KMaxPhysicalLinkIdleTimeout;
			}

		c->QueueIdleTimer(newIdleTime);
		if (c->Terminate(ERemoteUserEndedConnection) == KErrNone)
			{
			retVal = EFalse;
			}
		else
			{
			retVal = ETrue;
			}
		}
	return retVal;
	}

TInt CPhysicalLink::Terminate(THCIErrorCode aReason)
	{
	LOG_FUNC
	TInt err = KErrNone;

	__ASSERT_DEBUG(aReason == EAuthenticationFailure
				|| aReason == ERemoteUserEndedConnection
				|| aReason == ERemoteLowResources
				|| aReason == ERemoteAboutToPowerOff
				|| aReason == EUnsupportedRemoteLMPFeature
				|| aReason == EPairingWithUnitKeyNotSupported,
				Panic (EInvalidDisconnectReason)); // Check the error code is valid with the spec
	
	if (iLinkState.LinkState() == TBTBasebandLinkState::ELinkPending)
		{
		// If the Link is not yet up then we cannot know the correct connection handle
		// to disconnect so remember the request so we disconnect the link straight away
		// after it does come up.
		iDisconnectRequested = ETrue;
		}
	else
		{
		__ASSERT_DEBUG(iLinkState.LinkState() == TBTBasebandLinkState::ELinkUp, Panic(EInvalidLinkStateDuringDisconnect));

		TRAP(err, iDisconnectCtrl.Disconnect(aReason));
		}

	return err;
	}

void CPhysicalLink::SetModesAllowed(TUint8 aModesAllowed, TBool aRoleSwitchAllowed)
	{
	LOG_FUNC
	TBTLinkModeSet supportedModes = iRemoteFeatures.LinkModes();
	aModesAllowed &= supportedModes;
	aRoleSwitchAllowed &= IsRoleSwitchSupported();

	if(aModesAllowed != iLinkPolicy.LowPowerModePolicy() ||
	   aRoleSwitchAllowed != iLinkPolicy.IsSwitchAllowed())
		{
		TLinkPolicy tempPolicy;
		tempPolicy.SetModesAllowed(aModesAllowed);
		tempPolicy.SetSwitchAllowed(aRoleSwitchAllowed);

		TRAPD(err, iLinksMan.HCIFacade().WriteLinkPolicySettingsL(Handle(), tempPolicy.LinkPolicy()));

		if (err==KErrNone)
			{
			iLinkPolicy = tempPolicy;
			}
		}
	}

void CPhysicalLink::NotifyLogicalLinkDown(TPhysicalLinkPort aPort)
	{
	LOG_FUNC
	switch (aPort)
		{
		case EACLLink:
			{
			for (TInt i=iACLLogicalLinks.Count()-1; i>=0; i--)
				{
				// subscribers will unsubscribe as a result of this
				// This will result in the entry being removed from the iACLLogicalLinks array.
				// Consequently the array must be traversed from end to start.
				iACLLogicalLinks[i]->Disconnection();
				}
			break;
			}
		case ESCOLink:
		case EeSCOLink:
			{
			if (iSyncLogicalLink)
				{
				iSyncLogicalLink->Disconnection();
				}
			break;
			}

		default:
			Panic(EBTPhysicalLinkBadPort);
		}
	}

void CPhysicalLink::NotifyLogicalLinkUp(const TBTConnect& aConnect)
	{
	LOG_FUNC
	switch (aConnect.iLinkType)
		{
		case EACLLink:
			{
			for (TInt i=iACLLogicalLinks.Count()-1; i>=0; i--)
				{
				// all of them are open
				iACLLogicalLinks[i]->ConnectComplete(aConnect);
				}
			break;
			}

		case ESCOLink:
		case EeSCOLink:
			{
			if (iSyncLogicalLink)
				{
				iSyncLogicalLink->ConnectComplete(aConnect);
				}
			break;
			}

		default:
			Panic(EBTPhysicalLinkBadPort);
		}
	}

void CPhysicalLink::NotifyLogicalSyncLinkUp(const TBTConnect& aConnect, const TBTSyncConnectOpts& aSyncOpts)
	{
	LOG_FUNC
	switch (aConnect.iLinkType)
		{
		case ESCOLink:
		case EeSCOLink:
			{
			if (iSyncLogicalLink)
				{
				iSyncLogicalLink->SyncConnectComplete(aConnect, aSyncOpts);
				}
			break;
			}

		default:
			Panic(EBTPhysicalLinkBadPort);
		}
	}


void CPhysicalLink::NotifyLogicalLinkError(TPhysicalLinkPort aPort, TInt aError)
	{
	LOG_FUNC
	switch (aPort)
		{
		case EACLLink:
			{
			for (TInt i=iACLLogicalLinks.Count()-1; i>=0; i--)
				{
				// all of them are open
				iACLLogicalLinks[i]->Error(aError);
				}
			break;
			}

		case ESCOLink:
		case EeSCOLink:
			{
			if (iSyncLogicalLink)
				{
				iSyncLogicalLink->Error(aError);
				}
			break;
			}

		default:
			Panic(EBTPhysicalLinkBadPort);
		}
	}


TBool CPhysicalLink::IsModeSupportedRemotely(TBTLinkMode aMode) const
	{
	LOG_FUNC
	return iRemoteFeatures.LinkModes() & aMode;
	}

TBool CPhysicalLink::IsEncryptionPauseResumeSupported() const
	{
	LOG_FUNC
	return (iLinksMan.LinkManagerProtocol().IsEncryptionPauseResumeSupportedLocally() && iRemoteFeatures.IsEncryptionPauseResumeSupported());
	}

TBool CPhysicalLink::IsEncryptionSupported() const
	{
	LOG_FUNC
	return (iRemoteFeatures.IsEncryptionSupported() && iLinksMan.LinkManagerProtocol().IsEncryptionSupportedLocally());
	}

TBool CPhysicalLink::IsRoleSwitchSupported() const
	{
	LOG_FUNC
	// the physical link _can_ switch when encryption is on - see RequestChangeRole
	return (iRemoteFeatures.IsRoleSwitchSupported() && iLinksMan.RoleSwitchAllowed());
	}


//
// Change the packet types allowed on the link.
// Note that this is only suitable for use on ACL and old-style (pre eSCO) SCO links.
//
TInt CPhysicalLink::ChangeConnectionPacketType(TUint16 aType)
	{
	LOG_FUNC
	TInt rerr = KErrNone;

	// Build a mask of locally supported packet types (we shouldn't care about
	// what the remote supports (this is alluded to in the spec) probably
	// because they can change too.

	// DM1, DH1 and HV1 packets are always supported.
	TUint16 supportedTypes = EPacketsDM1 | EPacketsDH1 | EPacketsHV1;

	// get extra locally-supported packets (our controller may not support things)
	TBTFeatures localPackets = iLinksMan.LinkManagerProtocol().PacketsSupportedLocally();

	// 3 slot ACL packets
	if (localPackets[ESupportedThreeSlotPacketsBit])
		{
		supportedTypes |= (EPacketsDM3 | EPacketsDH3);
		}

	// 5 slot ACL packets
	if (localPackets[ESupportedFiveSlotPacketsBit])
		{
		supportedTypes |= (EPacketsDM5 | EPacketsDH5);
		}

	// HV2 single-slot packets
	if (localPackets[ESupportedHV2PacketsBit])
		{
		supportedTypes |= EPacketsHV2;
		}

	// HV3 single-slot packets
	if (localPackets[ESupportedHV3PacketsBit])
		{
		supportedTypes |= EPacketsHV3;
		}

	// Note: If one of the following EDR bits is set it indicates that the EDR
	// packet type shall NOT be used on this physical link. However, we must
	// not set any packet types that are not supported, so check first.

	// EDR 2Mbps
	if (localPackets[EEDR_ACL_2MbpsModeBit])
		{
		supportedTypes |= EPackets2_DH1;
		// EDR 3 slot ACL packets
		if (localPackets[ESupportedEDRThreeSlotPacketsBit])
			{
			supportedTypes |= EPackets2_DH3;
			}
		// EDR 5 slot ACL packets
		if (localPackets[ESupportedEDRFiveSlotPacketsBit])
			{
			supportedTypes |= EPackets2_DH5;
			}

		// EDR 3Mbps - Have to have 2Mbps to have 3Mbps
		if (localPackets[EEDR_ACL_3MbpsModeBit])
			{
			supportedTypes |= EPackets3_DH1;
			// EDR 3 slot ACL packets
			if (localPackets[ESupportedEDRThreeSlotPacketsBit])
				{
				supportedTypes |= EPackets3_DH3;
				}
			// EDR 5 slot ACL packets
			if (localPackets[ESupportedEDRFiveSlotPacketsBit])
				{
				supportedTypes |= EPackets3_DH5;
				}
			}
		}

	supportedTypes &= aType;
	// Check if the new supported types differ from the current configuration.
	if(supportedTypes != iLinkState.PacketTypes())
		{
		rerr = iLinksMan.HCIFacade().ChangeConnectionPacketType(iHandle, supportedTypes);
		}

	if(rerr == KErrNone)
		{
		iConnectionPacketTypeChanged = ETrue;
		}

	return rerr;
	}

TInt CPhysicalLink::RequestChangeRole(TBTBasebandRole aRole)
	{
	LOG_FUNC
	TInt err = KErrNone;

	// Check that the requested role is not the current role.
	if(iLinkState.LinkRole() != aRole)
		{
		if(!iLinkPolicy.IsSwitchAllowed())
			{
			err = KErrNotSupported;
			}

		// 1) Role switch is not allowed if there is a sync connection
		//    (or there is a sync connection negotiation in progress)
		else if (HasSyncLink() || SyncConnectPending())
			{
			err = KErrNotReady;
			}

		// 2) Check if role switch is supported on the remote device
		else if (!IsRoleSwitchSupported())
			{
 			err = KErrNotSupported;
			}

		// 3) Check if role switch is supported locally
 		else if (!iLinksMan.LinkManagerProtocol().IsRoleSwitchSupportedLocally())
 			{
 			err = KErrNotSupported;
 			}

		// Same role request is already with LinkMgr, or most probably running just now
		else if (iRoleSwitcher)
			{
			LOG(_L("CPhysicalLink - Same role change in progress!"))
			err = KErrNone;
			}

		// ok to change role
		// we kick off a role switcher state machine, to remove LPM and encryption, prior to
		// role switch
		else
			{
			TRAP(err, iRoleSwitcher = CRoleSwitcher::NewL(iLinksMan, *this, aRole));
			}
		}
#ifdef _DEBUG
	else
		{
		LOG(_L("CPhysicalLink We are in requested role!!!"))		
		}
#endif

	return err;
	}

void CPhysicalLink::AsyncDeleteRoleSwitcher()
	{
	LOG_FUNC
	iRoleSwitchCompleteCallBack->CallBack();
	}

/*static*/ TInt CPhysicalLink::RoleSwitchCompleteCallBack(TAny* aPhysicalLink)
	{
	LOG_STATIC_FUNC
	CPhysicalLink* physicalLink = static_cast<CPhysicalLink*>(aPhysicalLink);
	physicalLink->DeleteRoleSwitcher();
	return EFalse;
	}

void CPhysicalLink::DeleteRoleSwitcher()
	{
	LOG_FUNC
	delete iRoleSwitcher;
	iRoleSwitcher = NULL;
	}

void CPhysicalLink::AsyncDeleteKeyRefresher()
	{
	LOG_FUNC
	iKeyRefreshCompleteCallBack->CallBack();
	}

/*static*/ TInt CPhysicalLink::KeyRefreshCompleteCallBack(TAny* aPhysicalLink)
	{
	LOG_STATIC_FUNC
	CPhysicalLink* physicalLink = static_cast<CPhysicalLink*>(aPhysicalLink);
	physicalLink->DeleteKeyRefresher();
	return EFalse;
	}

void CPhysicalLink::DeleteKeyRefresher()
	{
	LOG_FUNC
	delete iKeyRefresher;
	iKeyRefresher = NULL;
	}

TBool CPhysicalLink::IsEncryptionDisabledForRoleSwitch() const
/**
	If link is encrypted, but role switcher temporarily disabled encryption, returns true.
**/
	{
	LOG_FUNC
	if (iRoleSwitcher)
		{
		return iRoleSwitcher->IsEncryptionDisabledForRoleSwitch();
		}
	return EFalse;
	}

TInt CPhysicalLink::OverridePark()
/**
	A request has come in that requires us to ensure we are not parked.

	Try to make it happen
**/
	{
	LOG_FUNC
	iOverrideParkRequests = ETrue;

	// Only arbitrate if Park mode is currently active.
	TInt rerr = KErrNone;
	if(iLinkState.LinkMode() & EParkMode)
		{
		rerr = Arbitrate();
		}
	return rerr;
	}

TInt CPhysicalLink::UndoOverridePark()
/**
	A need to ensure we are not parked has gone.

	Try to go back to power mode resulting from an unoverriden Arbitrate
**/
	{
	LOG_FUNC
	iOverrideParkRequests = EFalse;

	//Arbitrate even if there isn't an outstanding local park mode request beacuse
	//the remote device may be requesting park mode.
	return Arbitrate();
	}

TInt CPhysicalLink::OverrideLPM()
/**
	A request has come in that requires us to ensure we are not using
	an LPM (low power mode).

	Try to make it happen
**/
	{
	LOG_FUNC
	iOverrideLPMRequests = ETrue;

	return Arbitrate();
	}

TInt CPhysicalLink::UndoOverrideLPM()
/**
	A need to ensure we are not in LPM has gone.

	Try to go back to power mode resulting from an unoverriden Arbitrate
**/
	{
	LOG_FUNC
	iOverrideLPMRequests = EFalse;

	// Call Arbitrate(...) to determine if removing the override requires any change to
	// the physical link state.
	return Arbitrate();
	}

// security stuff
// connection looks after this as it may be passively initiated and there wont
// be a accessrequester in those situations

void CPhysicalLink::PinRequest(const TBTDevAddr& aAddr, MPINCodeResponseHandler& aRequester)
	{
	LOG_FUNC
	// If we receive a Pin_Request while another is outstanding then the controller is faulty. 
	// So we just ignore any multiple requests, as they do not make sense.
	// However, in UDEB we panic to raise awareness that the hardware is behaving incorrectly.
	__ASSERT_DEBUG(!iPinRequester, Panic(EBTConnectionPINRequestedTwice));

	SetAuthenticationPending(EPinRequestPending); // if not already set (because the remote initiated authentication).

	if (!IsConnected())
		{
		// If the ACL to the peer device is not yet connected, and the peer has initiated
		// authentication then it must be in security mode 3.  This information is stored and
		// if the connection completes the link will be set as authenticated.
		SetPeerInSecurityMode3();
		}

	// We can receive "fast" PIN requests if the remote device initiates pairing and indicates
	// it doesn't have a link key.  If we see this then we know that we are not engaging in 
	// simple pairing on this particular link.
	RemoteSimplePairingModeDetermined(EPhySimplePairingDisabled);

	if (iPinRequester)
		{
		return;
		}

	__ASSERT_DEBUG(aAddr == BDAddr(), Panic(EBTConnectionBadDeviceAddress));
	
	if(!IsPairable())
		{
		aRequester.PINCodeRequestNegativeReply(aAddr);
		return;
		}

	TBTPinCode pinCode;
	if(iLinksMan.PrefetchMan().GetPrefetch(aAddr, pinCode))
	    {
        iLinksMan.PrefetchMan().RemovePrefetch(aAddr);
        aRequester.PINCodeRequestReply(aAddr, pinCode);
        return;
	    }

	iPinHandler = &aRequester;

	TRAPD(err, DoPinRequestL(aAddr, *this)); // physical links will proxy a request.

	// if there is an error we should reply PIN negative...
	if (err)
		{
		iPinHandler->PINCodeRequestNegativeReply(aAddr);
		iPinHandler = NULL;
		}
	}

void CPhysicalLink::DoPinRequestL(const TBTDevAddr& aAddr, MPINCodeResponseHandler& aResponseHandler)
	{
	LOG_FUNC
	// Bluetooth is fairly poor at allowing name requests on partially connected links
	// this typically occurs in security mode 3

	// find out is it locally or remotley initiated authentication ...
	TBool locallyInitiated; //Authentication
	TBool strongKeyRequired;
	TUint minPasskeyLength=0;
	TUint recommendedPasskeyLength=0;

	iLinksMan.SecMan().GetPassKeyLengthAndOriginator(aAddr, minPasskeyLength, locallyInitiated, strongKeyRequired);

	/* If we have an activeAccessRequester and authentication was not actually required,
	 * this is a speculative authentication.
	 * In that case we did not expect to get a PinRequest. 
	 * We leave as the link already exists, causing the PinRequest to get a negative reply.
	 */
	if(iDevice.IsValidLinkKey())
		{
		CBTAccessRequester* activeAccessRequester = iLinksMan.SecMan().FindActiveAccessRequester(aAddr);
		if (activeAccessRequester)
			{
			if (!activeAccessRequester->AuthenticationRequired())
				{
				User::Leave(KErrAlreadyExists);
				}
			}
		}

	/* If link is already authenticated with a link key greater than the currently required minimum key length, then store 
	 * the current passkey length as a recommended length. If this recommendation is followed, then the strength of 
	 * the current link key will be maintained. This recommended length is made available via 
	 * TBTPinCodeEntryNotifierParams::RecommendedPinCodeMinLength().
	 */
	TUint devicePasskeyLength = iDevice.PassKeyLength();
	if(iDevice.IsValidLinkKey()&&devicePasskeyLength>minPasskeyLength)
		{
		//Ignore the previous link key length if it is not at least the minimum we require now anyway.
		recommendedPasskeyLength=devicePasskeyLength;
		}
	else
		{
		//Recommend the currently required minimum length.
		recommendedPasskeyLength=minPasskeyLength;
		}

	iPinRequester = CBTPinRequester::NewL(BDAddr(), aResponseHandler, iLinksMan.SecMan(),
										  minPasskeyLength, locallyInitiated, strongKeyRequired, recommendedPasskeyLength);
	}

TBool CPhysicalLink::LinkKeyRequestPending()
	{
	LOG_FUNC
	return iAuthStateMask & ELinkKeyRequestPending;
	}

TBool CPhysicalLink::IsAuthenticationRequestPending() const
	{
	LOG_FUNC
	return iAuthStateMask & EAuthenticationRequestPending;
	}


void CPhysicalLink::SetAuthenticationPending(TUint8 aState)
	{
	LOG_FUNC
	iAuthStateMask |= aState;
	LOG1(_L("SetAuthenticationPending: set state [%d] in an authentication state mask"), aState);
	}

void CPhysicalLink::AuthenticationComplete(TUint8 aState)
	{
	LOG_FUNC
	iAuthStateMask &= ~aState;
	LOG1(_L("AuthenticationComplete: reset state [%d] in an authentication state mask"), aState);
	PhysicalLinkUserIdle();
	}

TBool CPhysicalLink::IsPairable() const
	{
	// Determines if pairing can be performed on the physical link;
	// it previously was determined by the PIN code notifier, but with
	// 2.1 pairable mode has been replaced with bondable mode and
	// so being pairable is now a function of the security level
	// the device is operating under.
	// Here the policy is as follows:
	//
	// Pairable if...
	// 1) Not in paired only connections mode
	// OR 
	// 2) The link is already authenticated
	// OR
	// 3) A logical link was locally initiated
	// OR
	// 4) A dedicated bonding attempt is in progress
	// (it might be better to migrate dedicated bonding above the logical
	// link to remove this special case...)
	//
	// This policy provides strong security for while not applying
	// blanket restricts on explicit user requests (i.e. outgoing
	// connections).
	TBool locallyInitiatedLogicalLink = EFalse;
	for (TInt i=iACLLogicalLinks.Count()-1; i>=0; i--)
		{
		if(iACLLogicalLinks[i]->IsLocallyInitiated())
			{
			locallyInitiatedLogicalLink = ETrue;
			}
		}
	return (!iLinksMan.IsAcceptPairedOnlyMode()) 
		|| iLinkState.Authenticated() 
		|| locallyInitiatedLogicalLink
		|| iLinksMan.SecMan().IsDedicatedBondingAttempted(iDevice.Address());
	}

TBool CPhysicalLink::IsPairingExpected() const
	{
	LOG_FUNC
	return !(IsAuthenticationPending() && iLinkKeyReturnedInThisAuthentication);
	}

void CPhysicalLink::DeleteLinkKeyL()
/**
ensure that the LinkKey is removed from the device in the registry
**/
	{
	LOG_FUNC
	CBTLinkKeyDeleter* deleter = CBTLinkKeyDeleter::NewL(iRegSess, *this);
	iRegistryHelpers.AddLast(*deleter);
	deleter->Start(BDAddr());
	}

TBool CPhysicalLink::IsConnectionRequestPending() const
	{
	LOG_FUNC
	return iPendingConnection;
	}

void CPhysicalLink::PendingConnectionRequest(TInt /*aError*/)
	{
	LOG_FUNC
 	__ASSERT_DEBUG(iPendingConnection, Panic(EPhysicalLinkNoConnectionPending));

 	// Note: We could do something clever with the error code returned
 	// to prevent us thrashing in OOM situations, but it is complicated.
 	// We want to try again as now the pending connection address may
 	// now be in the cache even if we have a non-KErrNone error code,
 	// and so with that we will proceed sucessfully.  But if we are
 	// deferred we may end up here again and again with, perhaps with a
 	// KErrNoMemory for example ... which is how the thrashing will occur.
 	// This thrashing will be limited however by the "Connection Accept
 	// Timeout" imposed by the controller on the host, and for this
 	// reason we live with it.
	iPendingConnection = EFalse;
	ConnectionRequest(iLastPendingConnection);
	}

TBool CPhysicalLink::IsPasskeyMinLengthOK()
	{
	LOG_FUNC
/**
	check whether current passkey (retrieved from BTRegistry) is long enough to fulfill user passkey requirement
	It must be called only when iDevice is ready and has LinkKey
**/
	__ASSERT_DEBUG(iDeviceResult==KErrNone && iDevice.IsValidLinkKey(),
		 Panic(EBTPhysicalLinkNotAuthenticated));

	TUint newMinPasskeyLength=0;
	TBool isPasskeyMinLengthOK = ETrue;

	TBool locInit, strongKey;
	iLinksMan.SecMan().GetPassKeyLengthAndOriginator( iDevice.Address(), newMinPasskeyLength, locInit, strongKey);

	TBTPinCode currentPassKey = iDevice.PassKey();

	// check current PIN code length against user requirement
	if ( newMinPasskeyLength )
		{
		 if ( newMinPasskeyLength > currentPassKey().iLength )
			{
			// longer PIN code is requested
			isPasskeyMinLengthOK = EFalse;
			}
		}

	return isPasskeyMinLengthOK;
	}

void CPhysicalLink::LinkKeyRequest(const TBTDevAddr& __DEBUG_ONLY(aAddr), MLinkKeyResponseHandler& /*aRequester*/)
	{
	LOG_FUNC
	__ASSERT_DEBUG(aAddr == BDAddr(), Panic(EBTConnectionBadDeviceAddress));
	ASSERT_DEBUG(!iLinkKeyRequestOutstanding);

	iLinkKeyRequestOutstanding = ETrue;

	// we don't keep a copy of the device record - we just leave the one copy with
	// the baseband - it can tell us if there's a link key
	// we can tell if the baseband has the device record or not

	SetAuthenticationPending(ELinkKeyRequestPending); //authentication process started by remote

	// If the ACL to the peer device is not yet connected, and the peer has initiated
	// authentication then it must be in security mode 3.  This information is stored and
	// if the connection completes the link will be set as authenticated.
	if (!IsConnected())
		{
		SetPeerInSecurityMode3();
		}

	if (iLinkKeyRequestOutstanding)
		{ // might have already been called via SetPeerInSecurityMode3()
		LinkKeyRequestResponseAttempt();
		}
	}

TInt CPhysicalLink::PINCodeRequestReply(const TBTDevAddr& aDevAddr, const TDesC8& aPin) const
	{
	ASSERT_DEBUG(aDevAddr == this->BDAddr());
	const_cast<CPhysicalLink*>(this)->PINCodeRequestReply(aDevAddr, aPin);
	return KErrNone;
	}

TInt CPhysicalLink::PINCodeRequestNegativeReply(const TBTDevAddr& aDevAddr) const
	{
	ASSERT_DEBUG(aDevAddr == this->BDAddr());
	const_cast<CPhysicalLink*>(this)->PINCodeRequestNegativeReply(aDevAddr);
	return KErrNone;
	}

void CPhysicalLink::PINCodeRequestReply(const TBTDevAddr& aDevAddr, const TDesC8& aPin)
	{
	SetPassKey(aPin);
	PinRequestSent();
	iPinHandler->PINCodeRequestReply(aDevAddr, aPin);
	PinRequestComplete();
	}

void CPhysicalLink::PINCodeRequestNegativeReply(const TBTDevAddr& aDevAddr)
	{
	iPinHandler->PINCodeRequestNegativeReply(aDevAddr);
	PinRequestComplete();
	}


void CPhysicalLink::PinRequestSent()
	{
	LOG_FUNC
	iLinkKeyPending = ETrue;
	}

void CPhysicalLink::PinRequestComplete()
/**
	Just clean up now that PIN request has completed
**/
	{
	LOG_FUNC
	delete iPinRequester;
	iPinRequester = NULL;
	iPinHandler = NULL;
	AuthenticationComplete(EPinRequestPending);
	}

TInt CPhysicalLink::RequestHold()
	{
	LOG_FUNC
	if (!IsConnected())
        {
        return KErrDisconnected;
        }
	 return iLowPowModeCtrl.ChangeMode(EHoldMode, iHandle);
	}

void CPhysicalLink::ReadNewPhysicalLinkMetricValue(TUint aIoctlName, CBTProxySAP& aSAP, TInt aCurrentValue)
	{
	LOG_FUNC
	iPhysicalLinkMetrics->ReadNewPhysicalLinkMetricValue(aIoctlName, aSAP, aCurrentValue);
	}

void CPhysicalLink::GetCurrentBasebandState(TBTBasebandEventNotification & aEvent)
	{
	LOG_FUNC
	// Populate the event with the current baseband state.
	TUint32 events = 0;
	switch(iLinkState.LinkRole())
		{
		case EMaster:
			events |= ENotifyMaster;
			break;
		case ESlave:
			events |= ENotifySlave;
			break;
		case ERoleUnknown:
		default:
			break;
		};

	switch(iLinkState.LinkMode())
		{
		case EActiveMode:
			events |= ENotifyActiveMode;
			break;
		case ESniffMode:
			events |= ENotifySniffMode;
			break;
		case EParkMode:
			events |= ENotifyParkMode;
			break;
		case EHoldMode:
			events |= ENotifyHoldMode;
			break;
		case EScatterMode:
		default:
			break;
		};

	switch(iLinkState.MaxSlots())
		{
		case 1:
			events |= ENotifyMaxSlots1;
			break;
		case 3:
			events |= ENotifyMaxSlots3;
			break;
		case 5:
			events |= ENotifyMaxSlots5;
			break;
		default:
			break;
		};

	TUint16 packetTypes = iLinkState.PacketTypes();
	if(packetTypes & EPacketsDM1)
		{
		events |= ENotifyPacketsDM1;
		}
	if(packetTypes & EPacketsDM3)
		{
		events |= ENotifyPacketsDM3;
		}
	if(packetTypes & EPacketsDM5)
		{
		events |= ENotifyPacketsDM5;
		}
	if(packetTypes & EPacketsDH1)
		{
		events |= ENotifyPacketsDH1;
		}
	if(packetTypes & EPacketsDH3)
		{
		events |= ENotifyPacketsDH3;
		}
	if(packetTypes & EPacketsDH5)
		{
		events |= ENotifyPacketsDH5;
		}
	if(packetTypes & EPacketsHV1)
		{
		events |= ENotifyPacketsHV1;
		}
	if(packetTypes & EPacketsHV2)
		{
		events |= ENotifyPacketsHV2;
		}
	if(packetTypes & EPacketsHV3)
		{
		events |= ENotifyPacketsHV3;
		}

	if(iLinkState.Authenticated())
		{
		events |= ENotifyAuthenticationComplete;
		}

	if(iLinkState.Encrypted())
		{
		events |= ENotifyEncryptionChangeOn;
		}
	else
		{
		events |= ENotifyEncryptionChangeOff;
		}

	switch(iLinkState.LinkState())
		{
		case TBTBasebandLinkState::ELinkUp:
			events |= ENotifyPhysicalLinkUp;

			if (iSyncLogicalLink)
				{
				if (iSyncLogicalLink->IsOpen())
					{
					events |= ENotifySynchronousLinkUp;
					}
				else
					{
					events |= ENotifySynchronousLinkDown;
					}
				}
			break;
		case TBTBasebandLinkState::ELinkDown:
			events |= ENotifyPhysicalLinkDown;
			break;
		default:
			break;
		};

	aEvent.SetEventType(events);
	aEvent.SetErrorCode(0);
	}

TBool CPhysicalLink::ACLConnectPending() const
	{
	LOG_FUNC
	return !IsConnected();
	}

TBool CPhysicalLink::SyncConnectPending() const
	{
	LOG_FUNC
	if (!iSyncLogicalLink)
		{
		return EFalse;
		}

	return iSyncLogicalLink->ConnectPending();
	}

TInt CPhysicalLink::ManageEncryptionEnforcement(THCIEncryptModeFlag aEnable)
	{
	LOG_FUNC
	// Although this code has been written as if this method could be called more then once,
	// with different argument values:
	// ChangeEncryption() (and then ManageEncryptionEnforcement()) is called only
	// once and only with aEnable = EPointToPointEncryption.
	// It's called if during the physical link creation the encryption is requested.
	// That is because the assertion below.
	__ASSERT_DEBUG(aEnable != EEncryptionDisabled, Panic(EBTInvalidEncryptionDisableRequest));

	// although "&& (iEncryptionEnforcer)" is not necessary, we leave it because
	// clarify that we had asked for the encryption, so it's our responsability to
	// delete iEncryptionEnforcer.
	if ((aEnable == EEncryptionDisabled) && (iEncryptionEnforcer))
		{
		delete iEncryptionEnforcer;
		iEncryptionEnforcer = NULL;
		}
	else if ((aEnable != EEncryptionDisabled) && (!iEncryptionEnforcer))
		{
		TRAPD(err, iEncryptionEnforcer = CEncryptionEnforcer::NewL(*this, aEnable));
		return err;
		}
	return KErrNone;
	}

TBasebandTime CPhysicalLink::CalculatePageTimeout(TBasebandPageTimePolicy aPolicy, TUint8 aRepMode, TBool aValidClockOffset)
	{
	LOG_FUNC
	TBasebandTime pageTimeout = TBasebandPolicy::KPageTimeoutR0;

	switch (aRepMode)
		{
		// PSRM defined in spec
		case 0x00:
			// already set
			break;

		case 0x01:
			pageTimeout = TBasebandPolicy::KPageTimeoutR1;
			break;

		case 0x02:
			// fallthrough
		default:
			pageTimeout = TBasebandPolicy::KPageTimeoutR2;
			break;
		}

	if (!aValidClockOffset)
		{
		pageTimeout*=2;
		}
	else
		{
//		normalPageTimeout+=5; //slots
		}

	// policy may not be valid - or dontcare/normal
	switch (aPolicy)
		{
		case EPagingBestEffort:
			pageTimeout = static_cast<TBasebandTime>(TBasebandPolicy::KBestEffortTimeMultiplier * pageTimeout);
			break;

		case EPagingQuick:
			pageTimeout = static_cast<TBasebandTime>(TBasebandPolicy::KQuickTimeMultiplier * pageTimeout);
			break;

		default:
			break;
			// leave as is

		}
	return pageTimeout;
	}

// CArbitrationDelayTimer

CArbitrationDelayTimer::CArbitrationDelayTimer(CPhysicalLink* aParent)
	: CTimer(CActive::EPriorityStandard)
	, iParent(aParent)
	{
	LOG_FUNC
	ASSERT_DEBUG(iParent);
	CActiveScheduler::Add(this);
	}

void CArbitrationDelayTimer::ConstructL()
	{
	LOG_FUNC
	CTimer::ConstructL();
	}

CArbitrationDelayTimer* CArbitrationDelayTimer::NewL(CPhysicalLink* aParent)
	{
	LOG_STATIC_FUNC
	CArbitrationDelayTimer* self = new (ELeave) CArbitrationDelayTimer(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TInt CArbitrationDelayTimer::Start(TBool aImmediate, TBool aLocalPriority)
	{
	LOG_FUNC
	// Work out what the local priority will be now
	iLocalPriority = iLocalPriority || aLocalPriority;
	LOG1(_L8("Arbitration: Local Priority now %d"), iLocalPriority);
	if(aImmediate)
		{
		LOG(_L8("Arbitration: Immediate Arbitration Requested..."));
		CancelButPreserveLocalPriority();
		return DoArbitrate();
		}
	else if(!IsActive())
		{
		LOG(_L8("Arbitration: Arbitration requested, will execute after delay timer..."));
		After(KBTArbitrationDelay);
		}
	else // timer is already on its way
		{
		LOG(_L8("Arbitration: Arbitration delay timer still pending..."));
		}
	return KErrNone;
	}

void CArbitrationDelayTimer::Restart()
	{
	LOG_FUNC
	LOG(_L8("Arbitration: Arbitration timer restarted..."));
	CancelButPreserveLocalPriority();
	After(KBTArbitrationDelay);
	}

void CArbitrationDelayTimer::CancelButPreserveLocalPriority()
	{
	LOG_FUNC
	TBool localPriority = iLocalPriority;
	Cancel();
	iLocalPriority = localPriority;
	}


void CArbitrationDelayTimer::RunL()
/**
Allow arbitration of low power modes when the timer expires
**/
	{
	LOG_FUNC
	LOG(_L8("Arbitration: Delayed Arbitration executing..."));
	static_cast<void>(DoArbitrate()); // ignore the error (always has been...)
	}

TInt CArbitrationDelayTimer::DoArbitrate()
	{
	LOG_FUNC
	TBool localPriority = iLocalPriority;
	iLocalPriority = EFalse;
	return iParent->DoArbitrate(localPriority);
	}

void CArbitrationDelayTimer::DoCancel()
	{
	LOG_FUNC
	CTimer::DoCancel();
	iLocalPriority = EFalse;
	}

TInt CPhysicalLink::GetNumPendingHandles(TInt& aConnectionHandles, TLinkType aLinkType) const
	{
	LOG_FUNC
	aConnectionHandles = 0;

	switch(aLinkType)
		{
	case EeSCOLink:

		if (iSyncLogicalLink && iSyncLogicalLink->ConnectPending())
			{
			// Currently there is a maximum of one synclink per
			// physical link

			++aConnectionHandles;
			}

		break;

	case ESCOLink:

		if (iSyncLogicalLink && iSyncLogicalLink->ConnectPending())
			{
			// Currently there is a maximum of one synclink per
			// physical link

			++aConnectionHandles;
			}

		break;

	case EACLLink:

		if (!IsConnected())
			{
			// Currently there is a maximum of one asynclink per
			// physical link
			++aConnectionHandles;
			}

		break;

	default:
		return KErrUnknown;
		}

	return KErrNone;
	}

TInt CPhysicalLink::GetConnectionHandles(RHCIConnHandleArray& aConnectionHandles,
					 TLinkType aLinkType) const
	{
	LOG_FUNC
	// Always append to client supplied array - do not clear the
	// contents.  Only return connected handles.

	switch(aLinkType)
		{
	case EeSCOLink:

		if (!iSyncLogicalLink || iSyncLogicalLink->ConnectPending())
			{
			return KErrNone;
			}

		if (iSyncLogicalLink->LinkType() == EeSCOLink)
			{
			// Currently there is a maximum of one synclink per
			// physical link
			aConnectionHandles.Append(iSyncLogicalLink->Handle());
			}

		break;

	case ESCOLink:

		if (!iSyncLogicalLink || iSyncLogicalLink->ConnectPending())
			{
			return KErrNone;
			}

		if (iSyncLogicalLink->LinkType() == ESCOLink)
			{
			// Currently there is a maximum of one synclink per
			// physical link
			aConnectionHandles.Append(iSyncLogicalLink->Handle());
			}

		break;

	case EACLLink:
		// Currently there is a maximum of one asynclink per physical
		// link and they share the same handle - this may change in
		// the future for QoS.

		if (!IsConnected())
			{
			return KErrNone;
			}

		aConnectionHandles.Append(iHandle);
		break;

	default:
		return KErrUnknown;
		}

	return KErrNone;
	}

void CPhysicalLink::IOCapabilityAskForResponse(THCIIoCapability aIOCapability, THCIOobDataPresence aOOBDataPresence, THCIAuthenticationRequirement aAuthenticationRequirement)
	{
	LOG_FUNC
	iIOCapsReceived = ETrue;
	iIOCapability = aIOCapability;
	iOOBDataPresence = aOOBDataPresence;
	iAuthenticationRequirement = aAuthenticationRequirement;
	
	// If we haven't determined the SSP pairing mode the link is operating in yet then enable it,
	// since we have received a I/O cap response the simple pairing must be enabled.
	// This condition is to cater the fast remote device which responds very quickly,
	// even before we determine whether it supports simple pairing!
	RemoteSimplePairingModeDetermined(EPhySimplePairingEnabled);
	}


TBool CPhysicalLink::AuthWithMITM() const
	{
	LOG_FUNC
   	return (iLocalMITM || (iAuthenticationRequirement & KAuthenticationMitmReqMask))
  			&& (iIOCapability != EIOCapsNoInputNoOutput);
	}

void CPhysicalLink::SetLocalMITM(TBool aLocalMITM)
	{
	LOG_FUNC
	iLocalMITM = aLocalMITM;
	}

TPhysicalLinkSimplePairingMode CPhysicalLink::SimplePairingMode() const
	{
	LOG_FUNC
	return iSimplePairingMode;
	}

TBool CPhysicalLink::HasRemoteOobData() const
	{
	LOG_FUNC
	return iLinksMan.SecMan().OobDataManager().HasRemoteOobData(BDAddr());
	}

THCIAuthenticationRequirement CPhysicalLink::AuthenticationRequirement() const
	{
	LOG_FUNC
	return iAuthenticationRequirement;
	}

void CPhysicalLink::NewNumericComparatorL(const TBTDevAddr aAddr,
 										  CBTSecMan& aSecMan,
 										  TUint32 aNumericValue,
 										  TBool	aInternallyInitiated)
   	{
	LOG_FUNC
 	__ASSERT_DEBUG(aAddr == BDAddr(), Panic(EBTConnectionBadDeviceAddress));
 	
 	// Check remote IO capabilities so that the BTNumericComparator can tell the user
 	TBTNumericComparisonParams::TComparisonScenario compScenario = TBTNumericComparisonParams::ERemoteCanConfirm;
 	if(iIOCapsReceived)
 		{
 		switch(iIOCapability)
 			{
 			case EIOCapsDisplayOnly:
 				compScenario = TBTNumericComparisonParams::ERemoteCannotConfirm;
 				break;
 			case EIOCapsDisplayYesNo:
 				compScenario = TBTNumericComparisonParams::ERemoteCanConfirm;
 				break;
 			default:
 				// If iIOCapability = EIOCapsKeyboardOnly or EIOCapsNoInputNoOutput 
 				// we should not be using a Numeric Comparitor
 				__ASSERT_DEBUG(EFalse, Panic(EUnexpectedIOCapability));
 				break;
 			}
 		}
 	iNumericComparator = CBTNumericComparator::NewL(aAddr, aSecMan, aNumericValue, compScenario, aInternallyInitiated);
   	}

CBTNumericComparator* CPhysicalLink::InstanceNumericComparator() const
	{
	LOG_FUNC
	return iNumericComparator;
	}

TBool CPhysicalLink::IsNumericComparatorActive()const
	{
	LOG_FUNC
	return iNumericComparator->IsActive();
	}

void CPhysicalLink::DeleteNumericComparator()
	{
	LOG_FUNC
	delete iNumericComparator;
	iNumericComparator = NULL;
	}
void CPhysicalLink::CancelNumericComparator()
	{
	LOG_FUNC
	iNumericComparator->Cancel();
	}

void CPhysicalLink::NewPasskeyEntryL(const TBTDevAddr aAddr,
												CBTSecMan& aSecMan,
												TUint32 aNumericValue,
												TBool aInternallyInitiated)
	{
	LOG_FUNC
	iPasskeyEntry = CBTPasskeyEntry::NewL(aAddr, aSecMan, aNumericValue, aInternallyInitiated);
	}

CBTPasskeyEntry* CPhysicalLink::InstancePasskeyEntry() const
	{
	LOG_FUNC
	return iPasskeyEntry;
	}

TBool CPhysicalLink::IsPasskeyEntryActive()const
	{
	LOG_FUNC
	return iPasskeyEntry->IsActive();
	}

void CPhysicalLink::DeletePasskeyEntry()
	{
	LOG_FUNC
	delete iPasskeyEntry;
	iPasskeyEntry = NULL;
	}

void CPhysicalLink::CancelPasskeyEntry()
	{
	LOG_FUNC
	iPasskeyEntry->Cancel();
	}

void CPhysicalLink::PasskeyEntryKeyPressed(THCIPasskeyEntryNotificationType aKey)
	{
	LOG_FUNC
	iPasskeyEntry->KeyPressed(aKey);
	}

void CPhysicalLink::NewUserConfirmerL(const TBTDevAddr aAddr,
 										  CBTSecMan& aSecMan,
 										  TBool	aInternallyInitiated)
   	{
	LOG_FUNC
 	__ASSERT_DEBUG(aAddr == BDAddr(), Panic(EBTConnectionBadDeviceAddress));
  	iUserConfirmer = CBTUserConfirmer::NewL(aAddr, aSecMan, aInternallyInitiated);
   	}

CBTUserConfirmer* CPhysicalLink::InstanceUserConfirmer() const
	{
	LOG_FUNC
	return iUserConfirmer;
	}

TBool CPhysicalLink::IsUserConfirmerActive()const
	{
	LOG_FUNC
	return iUserConfirmer->IsActive();
	}

void CPhysicalLink::DeleteUserConfirmer()
	{
	LOG_FUNC
	delete iUserConfirmer;
	iUserConfirmer = NULL;
	}

void CPhysicalLink::CancelUserConfirmer()
	{
	LOG_FUNC
	iUserConfirmer->Cancel();
	}


TBasebandTime CPhysicalLink::GetSniffInterval() const
	{
	return iSniffInterval;
	}

void CPhysicalLink::LinkKeyRequestResponseAttempt(TBool aForceResponse)
	{
	ASSERT_DEBUG(iLinkKeyRequestOutstanding);

	if(!iPeerInSecurityMode3 && iLinksMan.SecMan().IsDedicatedBondingAttempted(iDevice.Address()))
		{
		// If we are doing DedicatedBonding then we should ignore the existing linkkey
		// in an attempt to generate a stronger one if possible.
		// Security mode 3 is a odd case - because we get what looks like double pairing (the remote
		// initiated pairing on connection, then the dedicated bonding pairing).  So we have removed
		// this feature for security mode 3 devices...they will have to suffer for their transgressions
		LOG(_L("CPhysicalLink: Dedicated bonding attempt - Sending link key request negative reply"));
		DoLinkKeyResponse(EFalse);
		iRequireAuthenticatedLinkKey = EFalse;
		}
	else if (iDeviceResult==KErrNone && iDevice.IsValidLinkKey())
		{
		if (iLinksMan.SecMan().DebugMode() && iDevice.LinkKeyType() != ELinkKeyDebug)
			{
			LOG(_L("CPhysicalLink: Debug mode - Link to debug link key"))
			DoLinkKeyResponse(EFalse);
			}
		else if (iDevice.LinkKeyType() != ELinkKeyCombination)
			{
			if (iRequireAuthenticatedLinkKey && iDevice.LinkKeyType() == ELinkKeyUnauthenticatedUpgradable && IsPairable())
				{
				LOG(_L("CPhysicalLink: Requiring Authenticated link key but currently only have unauthenticated"))
				DoLinkKeyResponse(EFalse);
				}
			else
				{
				LOG(_L("CPhysicalLink: non - combination key, auth OK"))
				DoLinkKeyResponse(ETrue);
				}
			}
		else // Standard (legacy) Combination Key
			{
			if (SimplePairingMode() == EPhySimplePairingUndefined)
				{
				LOG(_L("CPhysicalLink: Waiting for Secure Simple Pairing mode to be determined"));
				// wait for ssp mode to be determined...then try again
				}
			else if (IsPasskeyMinLengthOK() && SimplePairingMode() == EPhySimplePairingDisabled)
				{
				LOG(_L("CPhysicalLink: Combination key, Passkey len OK, no SSP"));
				DoLinkKeyResponse(ETrue);
				}
			else
				{
				LOG(_L("CPhysicalLink: Current link key is not sufficient!"))
				DoLinkKeyResponse(EFalse);
				}
			}
		iRequireAuthenticatedLinkKey = EFalse;
		}
	else if (iDeviceResult==KErrNone && !iDevice.IsValidLinkKey() || iDeviceResult==KErrNotFound)
		{
		LOG(_L("CPhysicalLink: No Link key available for the device"));
		DoLinkKeyResponse(EFalse);
		iRequireAuthenticatedLinkKey = EFalse;
		}
	else if (aForceResponse)
		{
		LOG(_L("CPhysicalLink: Forcing a link key response (-ve as we don't have a link key yet)"));
		DoLinkKeyResponse(EFalse);
		}
	else
		{
		LOG(_L("CPhysicalLink: Waiting for link key from Registry!"))
		// we're still waiting for the device....we'll respond when it turns up
		}
	}

/**
Send a link key response for an outstanding request, assumes that all details
have be validated.
*/
void CPhysicalLink::DoLinkKeyResponse(TBool aPositive)
	{
	LOG_FUNC
	ASSERT_DEBUG(iLinkKeyRequestOutstanding);

	if(aPositive)
		{
		LOG(_L("CPhysicalLink: Providing link key to HC..."))
		ASSERT_DEBUG(iDevice.IsValidLinkKey());
		if (IsAuthenticationPending())
			{
			iLinkKeyReturnedInThisAuthentication = ETrue;
			}
		iAuthenticationCtrl.LinkKeyRequestReply(iDevice.Address(), iDevice.LinkKey());
		}
	else
		{
		LOG(_L("CPhysicalLink: Indicating no link key to HC..."));
		iAuthenticationCtrl.LinkKeyRequestNegativeReply(iDevice.Address());
		}
	iLinkKeyRequestOutstanding = EFalse;
	}

void CPhysicalLink::RemoteSimplePairingModeDetermined(TPhysicalLinkSimplePairingMode aSimplePairingMode)
	{
	LOG2(_L8("Current SimplePairingMode = %d, aSimplePairingMode = %d"), SimplePairingMode(), aSimplePairingMode);
	ASSERT_DEBUG(aSimplePairingMode != EPhySimplePairingUndefined); // must be a definite value
	__ASSERT_DEBUG(SimplePairingMode() == aSimplePairingMode || SimplePairingMode() == EPhySimplePairingUndefined, Panic(EBTSSPModeChangedDuringConnection));

	const TPhysicalLinkSimplePairingMode previousSetting = iSimplePairingMode;
	iSimplePairingMode = aSimplePairingMode;
	if (previousSetting != iSimplePairingMode)
		{
		iLinksMan.SecMan().SimplePairingSupportDetermined(BDAddr());

		// Also we may be waiting to respond to a link key request.
		if (iLinkKeyRequestOutstanding)
			{
			LinkKeyRequestResponseAttempt();
			}
		}
	}

void CPhysicalLink::SetPeerInSecurityMode3()
	{
	iPeerInSecurityMode3 = ETrue;

	// We also now know that the remote cannot possibly do SSP, *and* the LMP will
	// likely lock our finding if the remote does SSP anyway while we do SM3.
	RemoteSimplePairingModeDetermined(EPhySimplePairingDisabled);
	}

//
// TLowPowModeCmdController
//

TLowPowModeCmdController::TLowPowModeCmdController(CPhysicalLink& aLink, MHCICommandQueue& aCmdController) : 	
	iParent(aLink),
	iCmdController(aCmdController),
	iOutstandingCmd(EFalse)
	{
	LOG_FUNC
	}

void TLowPowModeCmdController::Abort()
	{
	LOG_FUNC
	LOG(_L("TLowPowModeCmdController::Abort"));
	iCmdController.MhcqRemoveAllCommands(*this);
	}

void TLowPowModeCmdController::DoExitModeL(TBTLinkMode aMode, THCIConnHandle aConnHandle)
	{
	LOG_FUNC
	LOG2(_L("TLowPowModeCmdController::DoExitModeL: mode:%d iOutstandingCmd:%d"), aMode, iOutstandingCmd);
	if (!iOutstandingCmd)
		{
		switch (aMode)
			{
			case ESniffMode:
				{
				ExitSniffL(aConnHandle);
				break;
				}
			case EParkMode:
				{
				ExitParkL(aConnHandle);
				break;
				}
			// Not possile to prematurely exit hold mode
			default:
				break;
			}
		iOutstandingCmd = ETrue;
		}
	else
		{
		LOG1(_L("TLowPowModeCmdController::DoExitModeL: cannot exit from mode:%d due to an outstanding command"), aMode);
		}
	}

TInt TLowPowModeCmdController::ExitMode(TBTLinkMode aMode, THCIConnHandle aHandle)
	{
	LOG_FUNC
	TRAPD(err, DoExitModeL(aMode, aHandle));
	return err;
	}

void TLowPowModeCmdController::DoChangeModeL(TBTLinkMode aMode, THCIConnHandle aConnHandle)
	{
	LOG_FUNC
	LOG2(_L("TLowPowModeCmdController::DoChangeModeL: mode:%d iOutstandingCmd:%d"), aMode, iOutstandingCmd);
	if (!iOutstandingCmd)
		{
		switch (aMode)
			{
			case ESniffMode:
				{
				SniffL(aConnHandle);
				break;
				}
			case EParkMode:
				{
				ParkL(aConnHandle);
				break;
				}
			case EHoldMode:
				{
				HoldL(aConnHandle);
				break;
				}
			case EScatterMode:
			case EActiveMode:
				__ASSERT_DEBUG(0, Panic(EHCICommandBadArgument));
				break;
			}
		iOutstandingCmd = ETrue;
		}
	else
		{
		LOG1(_L("TLowPowModeCmdController::DoChangeModeL: cannot change to mode:%d due to an outstanding command"), aMode);
		}
	}

TInt TLowPowModeCmdController::ChangeMode(TBTLinkMode aMode, THCIConnHandle aHandle)
	{
	LOG_FUNC
	TRAPD(err, DoChangeModeL(aMode, aHandle));
	return err;
	}

TInt TLowPowModeCmdController::ExecuteModeChange(TBTLinkMode aTargetMode)
	{
	LOG_FUNC
	iTargetMode = aTargetMode;
	if(iTargetMode != iParent.LinkState().LinkMode())
		{
		if(iParent.LinkState().LinkMode() != EActiveMode)
			{
			//the current mode is not in Active Mode, therefore the mode need to change to active first before change to other mode.
			LOG(_L8("ExecuteModeChange: Exiting existing LPM mode..."));
			return ExitMode(iParent.LinkState().LinkMode(), iParent.Handle());
			}
		//the current mode is in Active mode, therefore the mode is ready to go other mode.
		if(iTargetMode == EHoldMode)
			{
			LOG(_L8("ExecuteModeChange: Entering Hold mode..."));
			return ChangeMode(EHoldMode, iParent.Handle());
			}
		else if(iTargetMode == ESniffMode)
			{
			LOG(_L8("ExecuteModeChange: Entering Sniff mode..."));
			return ChangeMode(ESniffMode, iParent.Handle());
			}
		else if(iTargetMode == EParkMode)
			{
			LOG(_L8("ExecuteModeChange: Entering Park mode..."));
			return ChangeMode(EParkMode, iParent.Handle());
			}
		else if(iTargetMode == EActiveMode)
			{
			LOG(_L8("ExecuteModeChange: Staying in Active mode..."));
			return KErrNone;
			}
		// Shouldn't reach here, we have a strange mode
		DEBUG_PANIC_LINENUM;
		}
	LOG(_L8("ExecuteModeChange: Already in correct LPM, not doing anything"));
	return KErrNone;
	}

void TLowPowModeCmdController::SniffL(THCIConnHandle aHandleToRemote)
	{
	LOG_FUNC
	CSniffModeCommand* cmd = CSniffModeCommand::NewL(aHandleToRemote, KBTSniffModeMaxInterval, KBTSniffModeMinInterval, KBTSniffModeAttempt, KBTSniffModeTimeout);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController.MhcqAddCommandL(cmd, *this);
	}

void TLowPowModeCmdController::ExitSniffL(THCIConnHandle aHandleToRemote)
	{
	LOG_FUNC
	CExitSniffModeCommand* cmd = CExitSniffModeCommand::NewL(aHandleToRemote);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController.MhcqAddCommandL(cmd, *this);
	}

void TLowPowModeCmdController::HoldL(THCIConnHandle aHandle)
	{
	LOG_FUNC
	CHoldModeCommand* cmd = CHoldModeCommand::NewL(aHandle, KBTHoldModeMaxInterval, KBTHoldModeMinInterval);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController.MhcqAddCommandL(cmd, *this);
	}

void TLowPowModeCmdController::ParkL(THCIConnHandle aHandle)
	{
	LOG_FUNC
	CParkModeCommand* cmd = CParkModeCommand::NewL(aHandle, KBTParkModeBeaconMaxInterval, KBTParkModeBeaconMinInterval);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController.MhcqAddCommandL(cmd, *this);
	}

void TLowPowModeCmdController::ExitParkL(THCIConnHandle aHandle)
	{
	LOG_FUNC
	CExitParkModeCommand* cmd = CExitParkModeCommand::NewL(aHandle);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController.MhcqAddCommandL(cmd, *this);
	}

void TLowPowModeCmdController::MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand)
	{
	LOG_FUNC
	THCIEventCode code = aEvent.EventCode();

	__ASSERT_DEBUG(!(code != EModeChangeEvent && code != ECommandStatusEvent), Panic(EHCIUnknownCommandEvent));	// this should never happen
	__ASSERT_DEBUG(aRelatedCommand != NULL, Panic(EHCIUnknownCommandEvent));	// this should never happen

	if (!aRelatedCommand)	// it is not the response to our command - should never happen
		return;

	if ((code == ECommandStatusEvent) && (aEvent.ErrorCode() != EOK))
		{
		LOG3(_L("TLowPowModeCmdController::MhcqcCommandEventReceived: event:%d opcode:0x%x error:0x%x"), code, aRelatedCommand->Opcode(), aEvent.ErrorCode());
		iOutstandingCmd = EFalse;	// reset the outstanding flag
		iParent.StartArbitrationTimer();
		}
	else if (code == EModeChangeEvent)
		{
		iOutstandingCmd = EFalse;	// reset the outstanding flag

		LOG2(_L("TLowPowModeCmdController::MhcqcCommandEventReceived: event:%d opcode:0x%x"), code, aRelatedCommand->Opcode());

		const TModeChangeEvent& modeChangeEvent = TModeChangeEvent::Cast(aEvent);
		TBTLinkMode currentmode = EActiveMode;
		switch(modeChangeEvent.CurrentMode())
			{
			// Mode 0, as defined for the Mode Change Event, is Active Mode
			case 0:
				break;
			case 1:
				currentmode = EHoldMode;
				break;
			case 2:
				currentmode = ESniffMode;
				break;
			case 3:
				currentmode = EParkMode;
				break;
			default:
				__ASSERT_ALWAYS(EFalse, Panic(EHCICommandBadArgument));
				break;
			}
		// In the HCI_Facade, in this situation, CPhysicalLinksManager::ModeChanged() is called.
		// Since this methods find the CPhysicalLink object (that is iParent) and then call its
		// ModeChange method, we can call it directly.
		iParent.ModeChange(aEvent.ErrorCode(), modeChangeEvent.ConnectionHandle(), currentmode, modeChangeEvent.Interval());
		//pass the current mode into Gear box. let gear box to decide if the mode is on the target mode. 
		//if it is, the gear box returns KErrNone, if it is not, the gear box will make another request for the target mode
		if (aEvent.ErrorCode() == EOK)
			{
            TInt err = ExecuteModeChange(iTargetMode);
			LOG1(_L("TLowPowModeCmdController::ExecuteModeChange: err:%d"), err);
			}
		}
	}

void TLowPowModeCmdController::MhcqcCommandErrored(TInt __DEBUG_ONLY(aErrorCode), const CHCICommandBase* __DEBUG_ONLY(aCommand))
	{
	LOG_FUNC
	__ASSERT_DEBUG(aCommand, Panic(EHCIUnknownCommandEvent));	// this should never happen
	#ifdef _DEBUG
	LOG2(_L("MhcqcCommandErrored: error code:%d opcode:0x%x"), aErrorCode, aCommand->Opcode());
	#endif
	iOutstandingCmd = EFalse;	// reset the outstanding flag
	iParent.StartArbitrationTimer();
	}




//
// CEncryptionEnforcer
//

CEncryptionEnforcer::CEncryptionEnforcer (CPhysicalLink& aLink, THCIEncryptModeFlag aEncryptionMode) :
	CActive(EPriorityStandard), // Standard priority
	iLink(aLink),
	iEncryptionMode(aEncryptionMode),
	iState(EInactive)
	{
	LOG_FUNC
	}

CEncryptionEnforcer* CEncryptionEnforcer::NewLC(CPhysicalLink& aLink, THCIEncryptModeFlag aEncryptionMode)
	{
	LOG_STATIC_FUNC
	CEncryptionEnforcer* self = new ( ELeave ) CEncryptionEnforcer(aLink, aEncryptionMode);
	CleanupStack::PushL (self);
	self->ConstructL ();
	return self;
	}

CEncryptionEnforcer* CEncryptionEnforcer::NewL(CPhysicalLink& aLink, THCIEncryptModeFlag aEncryptionMode)
	{
	LOG_STATIC_FUNC
	CEncryptionEnforcer* self = CEncryptionEnforcer::NewLC (aLink, aEncryptionMode);
	CleanupStack::Pop(self);
	return self;
	}

void CEncryptionEnforcer::ConstructL()
	{
	LOG_FUNC
	User::LeaveIfError (iTimer.CreateLocal () ); // Initialize timer
	CActiveScheduler::Add (this); // Add to scheduler
	}

CEncryptionEnforcer::~CEncryptionEnforcer()
	{
	LOG_FUNC
	Cancel (); // Cancel any request, if outstanding
	iTimer.Close (); // Destroy the RTimer object
	}

void CEncryptionEnforcer::DoCancel ()
	{
	LOG_FUNC
	iTimer.Cancel ();
	ChangeState(EInactive);
	}

void CEncryptionEnforcer::Start(TInt delay)
	{
	LOG_FUNC
	Cancel (); // Cancel any request, just to be sure
	LOG(_L("CEncryptionEnforcer activate timer"))
	iTimer.After (iStatus, delay); // Set for later
	SetActive (); // Tell scheduler a request is active
	}

void CEncryptionEnforcer::Stop()
	{
	LOG_FUNC
	Cancel();
	}

void CEncryptionEnforcer::RunL ()
	{
	LOG_FUNC
	switch(iState)
		{
		case ERoleSwitchTimerInProgress:
			LOG(_L("CEncryptionEnforcer::RunL() : timer expired after a roleswitch, enforce encryption"))
			ChangeState(EForcingInProgress);
			User::LeaveIfError(iLink.ChangeEncryption(iEncryptionMode));
			break;
		case ENoRoleSwitchTimerInProgress:
			LOG(_L("CEncryptionEnforcer::RunL() : timer expired and no roleswitch performed, terminate the link"))
			iLink.Terminate(ERemoteUserEndedConnection);
			break;
		default:
			// this should never happen, so we assert in debug so to raise the problem.
			// however it doesn't break our behaviour, so we don't care in release.
			LOG(_L("CEncryptionEnforcer::RunL() : bad state"))
			__ASSERT_DEBUG(EFalse, Panic(EEncryptionEnforcerBadState));
			break;
		}
	}

TInt CEncryptionEnforcer::RunError(TInt IF_FLOGGING(aError))
	{
	LOG_FUNC
	LOG1(_L("CEncryptionEnforcer::RunError() : error = %d"), aError);
	LOG(_L("CEncryptionEnforcer::RunError() : try to terminate the link"));
	return iLink.Terminate(ERemoteUserEndedConnection);
	}

void CEncryptionEnforcer::ChangeState(TState aState)
	{
	LOG_FUNC
	LOG2(_L("CEncryptionEnforcer::ChangeState(): old state = %d; new state = %d"), iState, aState);
	iState = aState;
	}

void CEncryptionEnforcer::EncryptionEnabled()
	{
	LOG_FUNC
	LOG1(_L("CEncryptionEnforcer::EncryptionEnabled() : current status = %d"), iState);
	switch(iState)
		{
		case EInactive:
			// do nothing, it means is the first encryption event as reply to our ChangeEncryption cmd.
			break;
		case EForcingInProgress:
			ChangeState(EInactive);
			break;
		case ENoRoleSwitchTimerInProgress:
		case ERoleSwitchTimerInProgress:
			Stop();
			break;
		}
	}

void CEncryptionEnforcer::EncryptionDisabled(TBool aEncryptionPauseResumeSupported)
	{
	LOG_FUNC
	LOG1(_L("CEncryptionEnforcer::EncryptionDisabled() : current status = %d"), iState);

	if(aEncryptionPauseResumeSupported)
		{
		Stop();
		ChangeState(EForcingInProgress);
		}

	switch(iState)
		{
		case EInactive:
			LOG(_L("CEncryptionEnforcer::EncryptionDisabled() : start timer"))
			ChangeState(ENoRoleSwitchTimerInProgress);
			Start(KTimeOutDelay);
			break;
		case EForcingInProgress:
			LOG(_L("CEncryptionEnforcer::EncryptionDisabled() : disconnect the link"))
			iLink.Terminate(ERemoteUserEndedConnection);
			break;
		default:
			// this should never happen, so we assert in debug so to raise the problem.
			// however it doesn't break our behaviour, so we don't care in release.
			LOG(_L("CEncryptionEnforcer::EncryptionDisabled() : bad state"))
			__ASSERT_DEBUG(EFalse, Panic(EEncryptionEnforcerBadState));
			break;
		}
	}

void CEncryptionEnforcer::RoleSwitchEvent()
	{
	LOG_FUNC

	switch(iState)
		{
		case EInactive:
			// This is possible with EPR support - we don't need to do anything in this case.
			break;
		case ENoRoleSwitchTimerInProgress:
			ChangeState(ERoleSwitchTimerInProgress);
			break;
		default:
			// this should never happen, so we assert in debug so to raise the problem.
			// however it doesn't break our behaviour, so we don't care in release.
			LOG(_L("CEncryptionEnforcer::EncryptionDisabled() : bad state"))
			__ASSERT_DEBUG(EFalse, Panic(EEncryptionEnforcerBadState));
			break;
		}
	}

void XAutoKeyRefreshToken::Release()
	{
	LOG_FUNC
	iQueLink.Deque();
	delete this;
	}

//
// TRegistryDeviceOutChecker
//
TRegistryDeviceBeingModified::TRegistryDeviceBeingModified() :
	iReferenceCount(0)
	{
	LOG_FUNC
	}

TBool TRegistryDeviceBeingModified::InUse() const
	{
	LOG_FUNC
		return iReferenceCount > 0;
	}

TBool TRegistryDeviceBeingModified::IsEqual(const TBTNamelessDevice& aDevice) const
	{
	LOG_FUNC
	return (iDevice == aDevice);
	}

void TRegistryDeviceBeingModified::Begin(const TBTNamelessDevice& aDevice)
	{
	LOG_FUNC
	iDevice = aDevice;
	iReferenceCount++;
	}

void TRegistryDeviceBeingModified::RequestCompleted()
	{
	LOG_FUNC
	iReferenceCount--;
	__ASSERT_DEBUG(iReferenceCount >= 0, Panic(ETRegistryDevBeingModUnexpectedCompleted));
	}

//
// TDisconnectCmdController
// HCI Command Queue Interface for Disconnect Commands
//

TDisconnectCmdController::TDisconnectCmdController(CPhysicalLink& aLink, MHCICommandQueue& aCmdController) : 	
	iParent(aLink),
	iCmdController(aCmdController)
	{
	LOG_FUNC
	}

void TDisconnectCmdController::Abort()
	{
	LOG_FUNC
	iCmdController.MhcqRemoveAllCommands(*this);
	}

TInt TDisconnectCmdController::Disconnect(THCIErrorCode aReason)
	{
	LOG_FUNC
	TRAPD(err, DoDisconnectL(aReason));
	return err;
	}

void TDisconnectCmdController::DoDisconnectL(THCIErrorCode aReason)
	{
	LOG_FUNC
	CDisconnectCommand* cmd = CDisconnectCommand::NewL(iParent.Handle(), aReason);

	// Ownership of cmd transfered even if MhcqAddCommandL leaves
	iCmdController.MhcqAddCommandL(cmd, *this);
	}

void TDisconnectCmdController::MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand)
	{
	LOG_FUNC
	THCIEventCode code = aEvent.EventCode();
	
	__ASSERT_DEBUG(!(code != EDisconnectionCompleteEvent && code != ECommandStatusEvent), Panic(EDiscCtrlUnexpectedCommandEvent));	// this should never happen
	__ASSERT_DEBUG(aRelatedCommand != NULL, Panic(EDiscCtrlUnmatchedCommandEvent));	// this should never happen
	
	if (!aRelatedCommand)	// it is not the response to our command - should never happen
		return;
	
	if ((code == ECommandStatusEvent) && (aEvent.ErrorCode() != EOK))
		{
		iParent.Disconnection(aEvent.ErrorCode(), iParent.Handle(), aEvent.ErrorCode());
		}
	else if (code == EDisconnectionCompleteEvent)
		{
		const TDisconnectionCompleteEvent& disconnectionCompleteEvent = TDisconnectionCompleteEvent::Cast(aEvent);
		// The connection handle in the disconnect complete event should be the same as the physical link's connection handle
		__ASSERT_DEBUG(disconnectionCompleteEvent.ConnectionHandle() == iParent.Handle(), Panic(EDiscCtrlUnexpectedConnectionHandle));
		iParent.Disconnection(aEvent.ErrorCode(), disconnectionCompleteEvent.ConnectionHandle(), static_cast<THCIErrorCode>(disconnectionCompleteEvent.Reason()));
		}
	}

void TDisconnectCmdController::MhcqcCommandErrored(TInt __DEBUG_ONLY(aErrorCode), const CHCICommandBase* __DEBUG_ONLY(aCommand))
	{
	LOG_FUNC
	__ASSERT_DEBUG(aCommand, Panic(EDiscCtrlUnmatchedCommandEvent));	// this should never happen
	#ifdef _DEBUG
	LOG2(_L("MhcqcCommandErrored: error code:%d opcode:0x%x"), aErrorCode, aCommand->Opcode());
	#endif
	// Pass this to the physical link as Command Disallowed
	iParent.Disconnection(ECommandDisallowed, iParent.Handle(), ECommandDisallowed);
	}


//
// TAuthenticationCmdController
//

TAuthenticationCmdController::TAuthenticationCmdController(CPhysicalLink& aLink, MHCICommandQueue& aCmdController) : 	
	iParent(aLink),
	iCmdController(aCmdController)
	{
	LOG_FUNC
	}

void TAuthenticationCmdController::Abort()
	{
	LOG_FUNC
	iCmdController.MhcqRemoveAllCommands(*this);
	}

TInt TAuthenticationCmdController::LinkKeyRequestReply(const TBTDevAddr& aBdaddr, const TDesC8& aLinkKey) const
/**
	We have obtained a linkkey, tell HC
**/
	{
	CLinkKeyRequestReplyCommand* cmd = NULL;
	TBTLinkKey linkkey;
	linkkey.Copy(aLinkKey);

	TRAPD(err, cmd = CLinkKeyRequestReplyCommand::NewL(aBdaddr, linkkey));
	if (err == KErrNone)
		{
		// Ownership of cmd transfered even if MhcqAddCommandL leaves
		TRAP(err,iCmdController.MhcqAddCommandL(cmd, (MHCICommandQueueClient&)(*this)));
		}
	if(err != KErrNone)
		{
		iParent.AuthenticationComplete(ELinkKeyRequestPending);
		}
	return err;
	}
	
TInt TAuthenticationCmdController::LinkKeyRequestNegativeReply(const TBTDevAddr& aBdaddr) const
/**
	We have not obtained a linkkey, tell HC
**/
	{
	CLinkKeyRequestReplyNegativeCommand* cmd = NULL;

	TRAPD(err, cmd = CLinkKeyRequestReplyNegativeCommand::NewL(aBdaddr));
	if (err == KErrNone)
		{
		// Ownership of cmd transfered even if MhcqAddCommandL leaves
		TRAP(err, iCmdController.MhcqAddCommandL(cmd, (MHCICommandQueueClient&)(*this)));
		}
	
	if(err != KErrNone)
		{
		iParent.AuthenticationComplete(ELinkKeyRequestPending);
		}
	return err;
	}

void TAuthenticationCmdController::MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand)
	{
	LOG_FUNC
	THCIEventCode code = aEvent.EventCode();

	__ASSERT_DEBUG(!(code != ECommandCompleteEvent && code != ECommandStatusEvent), Panic(EHCIUnknownCommandEvent));	// this should never happen
	__ASSERT_DEBUG(aRelatedCommand != NULL, Panic(EHCIUnknownCommandEvent));	// this should never happen

	if (!aRelatedCommand)	// it is not the response to our command - should never happen
		return;

	if ((code == ECommandStatusEvent) && (aEvent.ErrorCode() != EOK))
		{
		LOG3(_L("TAuthenticationCmdController::MhcqcCommandEventReceived: event:%d opcode:0x%x error:0x%x"), code, aRelatedCommand->Opcode(), aEvent.ErrorCode());
		}
	else if (code == ECommandCompleteEvent)
		{
		LOG2(_L("TAuthenticationCmdController::MhcqcCommandEventReceived: event:%d opcode:0x%x"), code, aRelatedCommand->Opcode());
		iParent.AuthenticationComplete(ELinkKeyRequestPending);
		}
	}

void TAuthenticationCmdController::MhcqcCommandErrored(TInt __DEBUG_ONLY(aErrorCode), const CHCICommandBase* __DEBUG_ONLY(aCommand))
	{
	LOG_FUNC
	__ASSERT_DEBUG(aCommand, Panic(EHCIUnknownCommandEvent));	// this should never happen
	#ifdef _DEBUG
	LOG2(_L("MhcqcCommandErrored: error code:%d opcode:0x%x"), aErrorCode, aCommand->Opcode());
	#endif
	iParent.AuthenticationComplete(ELinkKeyRequestPending);
	}

