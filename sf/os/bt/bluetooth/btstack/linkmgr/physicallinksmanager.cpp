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
// Implementation of physical links manager
// 
//

#include <bluetooth/logger.h>
#include "physicallinksmanager.h"
#include "physicallinks.h"
#include "hcifacade.h"
#include "Basebandmodel.h"
#include "ProxySAP.h"

#include <bt_sock.h>
#include "roleswitchhelper.h"
#include "hostresolver.h"

#include <bluetooth/hci/writepagetimeoutcommand.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

#ifdef _DEBUG
PANICCATEGORY("plinkmgr");
#endif

// macro to dump invalid commands from hardware
#define	RETURN_IF_NULL_CONNECTION(found) {if (!found) {LOG(_L("CPhysicalLinksManager: Bad event received - dropping")); return;}}


CPhysicalLinksManager* CPhysicalLinksManager::NewL(CLinkMgrProtocol& aLinkMgrProtocol,
												   CHCIFacade& aHCIFacade,
												   CRegistrySession& aRegSess,
												   CBTSecMan& aSecMan,
												   CBTCodServiceMan& aCodMan)
	{
	CPhysicalLinksManager* s = new(ELeave) CPhysicalLinksManager(aLinkMgrProtocol,
																 aHCIFacade,
																 aRegSess,
																 aSecMan,
																 aCodMan);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return s;
	}

CPhysicalLinksManager::CPhysicalLinksManager(CLinkMgrProtocol& aLinkMgrProtocol,
											 CHCIFacade& aHCIFacade,
											 CRegistrySession& aRegSess,
											 CBTSecMan& aSecMan,
											 CBTCodServiceMan& aCodMan)
	: iPhysicalLinks(KBTBasebandConnectionArrayGranularity)
	, iHCIRequestHandler(aHCIFacade)
	, iRegSess(aRegSess)
	, iSecMan(aSecMan)
	, iCodMan(aCodMan)
	, iLinkManagerProtocol(aLinkMgrProtocol)
	, iTerminatingProxy(NULL)
	, iTerminatingConnection(NULL)
	, iRoleSwitchersQ(_FOFF(CRoleSwitcher, iQLink))
	, iPageTimeoutController(aHCIFacade.CommandQController())
	{
	}

void CPhysicalLinksManager::ConstructL()
	{
	iPairingsCache = CBTPairingsCache::NewL(*this, iRegSess.RegServ());
	iBaseband = CBTBasebandModel::NewL(iLinkManagerProtocol);
	iPrefetchMan = CBluetoothPrefetchManager::NewL();
	}

CPhysicalLinksManager::~CPhysicalLinksManager()
	{
	LOG(_L("sec\tCBTConnectionsManager destructing"));

	iPageTimeoutController.Abort();

	iPhysicalLinks.ResetAndDestroy();
	iPhysicalLinks.Close();
	__ASSERT_ALWAYS(iRoleSwitchersQ.IsEmpty(), Panic(ERoleSwitchersNotDeleted));
	iListeners.Close();

	delete iBaseband;
	delete iRoleSwitcherStateFactory;
	delete iPairingsCache;
	delete iPrefetchMan;
	}

void CPhysicalLinksManager::RequireSlotSpace()
/**
	At present we put all those connections that are holdable into hold
	To release baseband space for certain activities such
	as device discovery

	In future other operations may be performed here...
**/
	{
	if (LinkManagerProtocol().IsModeSupportedLocally(EHoldMode))
		{
		CPhysicalLink* con = NULL;
		// locally can support Hold mode - see if we can free up links that do
		TInt count = iPhysicalLinks.Count();

		for (TInt i=(count-1); i>=0; i--)
			{
			con = iPhysicalLinks[i];
			static_cast<void>(con->RequestHold()); //ignore error - best effort
			}
		}
	}

TBasebandTime CPhysicalLinksManager::TryToChangePageTimeout(TBasebandTime aPageTimeout)
	{
	// First check the page timeout is suitable wrt. link supervision timeouts.
	aPageTimeout = CheckPageTimeoutAgainstLSTO(aPageTimeout);
	iPageTimeoutController.WritePageTimeout(aPageTimeout);
	return aPageTimeout; // return the timeout that was tried (not necessarily set).
	}

CPhysicalLink& CPhysicalLinksManager::NewPhysicalLinkL(const TBTDevAddr& aAddr)
	{
	// create a PHY just knowing its address
	TBTNamelessDevice device;
	device.SetAddress(aAddr);
	return NewPhysicalLinkL(device);
	}

CPhysicalLink& CPhysicalLinksManager::NewPhysicalLinkL(const TBTNamelessDevice& aDevice)
	{
	// create a PHY with a full Device - make sure it isn't us!
	if (aDevice.Address() == iLinkManagerProtocol.LocalBTAddress())
		{
		// the app may be at fault - so don't panic
		User::Leave(KErrReflexiveBluetoothLink);
		}

	// create role switch SM
	// delete is in destructor of CPhysicalLinksManager
	if (!iRoleSwitcherStateFactory)
		{
		iRoleSwitcherStateFactory = CRoleSwitcherStateFactory::NewL();
		}
	
	CPhysicalLink* phy = CPhysicalLink::NewLC(*this, iRegSess, aDevice);
	User::LeaveIfError(iPhysicalLinks.Append(phy)); //transfers ownership
	
	CleanupStack::Pop(phy);
	LOG1(_L("CPhysicalLinksManager: Number of physical links %d"), iPhysicalLinks.Count());
	return *phy;
	}

CPhysicalLink* CPhysicalLinksManager::FindPhysicalLink(const TBTDevAddr& aBDAddr) const
	{
	CPhysicalLink* con = NULL;
	CPhysicalLink* found = NULL;
	
	
	TInt count = iPhysicalLinks.Count();

	for (TInt i=(count-1); i>=0; i--)
		{
		con = iPhysicalLinks[i];

		if (con->BDAddr() == aBDAddr)
			{
#ifndef _DEBUG
			found = con;
			break;
#else
			if (found) //make sure there's only one
				{
				__DEBUGGER()
				}
			else
				{
				found = con;
				continue; // just to show we do want to!
				}
#endif
			}
		}
	return found;
	}

void CPhysicalLinksManager::DumpPhysicalLinks()
	{
#ifdef __FLOG_ACTIVE
	TInt count = iPhysicalLinks.Count();

	CPhysicalLink* con = NULL;
	for (TInt i=(count-1); i>=0; i--)
		{
		con = iPhysicalLinks[i];
		LOG(_L(" - Link: "));
		LOGBTDEVADDR(con->BDAddr());
		}
#endif
	}

TBool CPhysicalLinksManager::IsAcceptPairedOnlyMode()
	{
	return iAcceptPairedOnlyMode;
	}

CPhysicalLink* CPhysicalLinksManager::FindPhysicalLink(THCIConnHandle aConnH) const
/**
	Handy for when a disconnection comes in - disconnections dont give either
	the address or (if SCO) the 'bound' handle.

	This method asks each CPhysicalLink if it knows of the handle (ACL or SCO)
**/
	{
	CPhysicalLink* con = NULL;
	CPhysicalLink* found = NULL;
	
	TInt count = iPhysicalLinks.Count();
	for (TInt i=(count-1); i>=0; i--)
		{
		con = iPhysicalLinks[i];
		if (con->HasHandle(aConnH))
			{
#ifndef _DEBUG
			found = con;
			break;
#else
			if (found) //make sure there's only one
				{
				__DEBUGGER()
				}
			else
				{
				found = con;
				continue; // just to show we do want to!
				}
#endif
			}
		}
	return found;
	}

CPhysicalLink* CPhysicalLinksManager::FindPhysicalLink() const
/**
	Handy for finding a random connected PHY
**/
	{
	CPhysicalLink* con = NULL;
	CPhysicalLink* found = NULL;


	TInt count = iPhysicalLinks.Count();
	if(!count)
		{
		return NULL;
		}
	for (TInt i=(count-1); i>=0; i--)
		{
		con = iPhysicalLinks[i];
		if (con->Handle()!=KHCIBroadcastHandle)
			{
			found = con;
			break;
			}
		}
	return found;
	}

TLogicalLinkListener* CPhysicalLinksManager::FindListener(TPhysicalLinkPort aPort)
	{
	// go through listeners until we find a listener on the required port
	TLogicalLinkListener* found = NULL;
	TLogicalLinkListener* l = NULL;

	TInt count = iListeners.Count();
	for (TInt i=(count-1); i>=0; i--)
		{
		l = &iListeners[i];
		if (l->iPort == aPort)
			{
			found = l;
			break;
			}
		}

	return found;
	}

void CPhysicalLinksManager::ClearTerminatingProxy(CBTProxySAP* aProxySAP)
	{
	if (aProxySAP==iTerminatingProxy)
		{
		iTerminatingProxy=NULL;
		}
	}
	
void CPhysicalLinksManager::UpdateTerminatingProxy(CBTProxySAP* aProxySAP)
	{
	// If there is an outstanding terminate request for a different Proxy SAP
	// complete it here and deal with the new request
	if (iTerminatingProxy && (iTerminatingProxy!=aProxySAP))
		{
		iTerminatingProxy->TerminatePhysicalLinksComplete();
		}
	iTerminatingProxy=aProxySAP;
	}
	
TInt CPhysicalLinksManager::TerminateAllPhysicalLinks(CBTProxySAP* aProxySAP, THCIErrorCode aErrorCode)
	{
	TInt count=iPhysicalLinks.Count();
	TInt retVal = (count==0) ? KErrNotFound : KErrNone;
		
	for (TInt i=0; i<count; i++)
		{
		// If any one of the physical links return an error then this 
		// function needs to return an error.
		TInt err = iPhysicalLinks[i]->Terminate(aErrorCode);
		if (err != KErrNone)
			{
			retVal=err;
			}
		}
		
	// aProxy==NULL	means that the calling Proxy SAP does not want to be told when the
	// physical link has terminated
	if ((retVal==KErrNone) || !aProxySAP)
		{
		UpdateTerminatingProxy(aProxySAP);
		}
	
	return retVal;
	}

TInt CPhysicalLinksManager::TerminatePhysicalLink(CPhysicalLink* aConnection, CBTProxySAP* aProxySAP)
	{
	TInt count = iPhysicalLinks.Count();
	TInt retVal=KErrNotFound;
	for (TInt i=(count-1); i>=0; i--)
		{
		if (iPhysicalLinks[i]==aConnection)
			{
			retVal=iPhysicalLinks[i]->Terminate(ERemoteUserEndedConnection);
			break;	
			}
		}
	
	// aProxy==NULL	means that the calling Proxy SAP does not want to be told when the
	// physical link has terminated
	if ((retVal==KErrNone) || !aProxySAP)
		{
		UpdateTerminatingProxy(aProxySAP);
		iTerminatingConnection=aConnection;
		}
	
	return retVal;
	}

void CPhysicalLinksManager::UpdateRemoteDevicesDetails()
	{
	//Update paired device list
	iPairingsCache->UpdateCache();
	
	//Update cached device info for existing PHYs
	TInt count=iPhysicalLinks.Count();
	for (TInt i=0; i<count; i++)
		{
		// try to get details of remote device
		TRAP_IGNORE(iPhysicalLinks[i]->GetDeviceFromRegistryL());
		// ignore error - as with other registry accessing routines
		}
	}

void CPhysicalLinksManager::SetAcceptPairedOnlyMode(TBool aOn)
	{
	iAcceptPairedOnlyMode = aOn;
	LinkManagerProtocol().SetAcceptPairedOnlyMode(aOn);
	}


void CPhysicalLinksManager::SetSimplePairingDebugMode(TBool aOn)
	{
	if(!SecMan().LocalSimplePairingMode())
		{
		LOG(_L("CPhysicalLinksManager: Simple pairing not supported by controller - ignoring request for debug mode"));
		return;
		}
	if(aOn)
		{
		iSecMan.SetDebugMode(aOn);
		}
	}

void CPhysicalLinksManager::SimplePairingDebugModeChanged(TBool aOn)
	{
	LinkManagerProtocol().SetSimplePairingDebugMode(aOn);
	}

void CPhysicalLinksManager::FatalError(TInt /*aErr*/)
	{
	// simulate a disconnection complete on all PHYs
	TInt count=iPhysicalLinks.Count();

	//The call to l.Disconnection removes an element from array iPhysicalLink, therefore invalidates 
	//count. So it's best iterating through the array backwards because count is only used for 
	//initialising i, and i is always valid. 
	for (TInt i=count-1; i>=0; i--)
		{
		// we say remote, as this is a passive-like disconnection
		CPhysicalLink& l = *iPhysicalLinks[i];
		l.Disconnection(EHardwareFail, l.Handle(), EHardwareFail);
		}

	// Don't error the Listeners
	}

void CPhysicalLinksManager::NewLinkKey(const TBTDevAddr& aBDAddr, const TBTLinkKey& aLinkKey, THCILinkKeyType aLinkKeyType)
	{
	CPhysicalLink* found = FindPhysicalLink(aBDAddr);
	RETURN_IF_NULL_CONNECTION(found);
	found->NewLinkKey(aBDAddr, aLinkKey, aLinkKeyType);
	}

void CPhysicalLinksManager::RemovePhysicalLink(const CPhysicalLink& aConnection)
/**
	A Physical Link wants us to remove our knowledge of it
**/
	{
	TInt count = iPhysicalLinks.Count();
	TInt i;
	for (i=(count-1); i>=0; i--)
		{
		if (iPhysicalLinks[i]==&aConnection)
			{
			iPhysicalLinks.Remove(i);
			count--;
			break;
			}
		}
		
	if (count!=0)
		{
		//compress the array if needs be
		if (i!=count)
			{
			iPhysicalLinks.GranularCompress();
			}

		// A physical link has been removed. Arbitrate for role switch allowed 
		// on the remainder of the links.
		ArbitrateAllPhysicalLinks();
		}
	
	// IF we have a Terminate request from a ProxySAP AND
	// either it was a request to terminate all connections and we have no connections left OR
	// it was a request for a specific connection that we have just removed THEN
	// tell the Proxy SAP that the request is complete
	if (iTerminatingProxy && 
		((!iTerminatingConnection && (count==0)) || 
		 ((iTerminatingConnection==&aConnection) && (i!=-1))))
		{
		iTerminatingProxy->TerminatePhysicalLinksComplete();
		iTerminatingProxy=NULL;
		iTerminatingConnection=NULL;
		}
	
	// good time to update that UI
	LinkManagerProtocol().SetUINumPhysicalLinks(count);
	LOG1(_L("CPhysicalLinksManager: Number of physical links %d"), count);
	}

void CPhysicalLinksManager::ArbitrateAllPhysicalLinks()
	{
	TInt i;
	for (i=(iPhysicalLinks.Count()-1); i>=0; i--)
		{
		iPhysicalLinks[i]->Arbitrate();
		}
	}

void CPhysicalLinksManager::SimplePairingComplete(const TBTDevAddr& aBDAddr, THCIErrorCode /*aErr*/)
	{
	//find the connection and pass on the request
	CPhysicalLink* found = FindPhysicalLink(aBDAddr);
	RETURN_IF_NULL_CONNECTION(found);
	found->AuthenticationComplete(ESimplePairingPending);
	}

void CPhysicalLinksManager::PinRequest(const TBTDevAddr& aBDAddr,
										MPINCodeResponseHandler& aRequester)
	{
	//find the connection and pass on the request
	CPhysicalLink* found = FindPhysicalLink(aBDAddr);
	RETURN_IF_NULL_CONNECTION(found);
	found->PinRequest(aBDAddr, aRequester);
	}

void CPhysicalLinksManager::LinkKeyRequest(const TBTDevAddr& aBDAddr, MLinkKeyResponseHandler& aRequester)
	{
	CPhysicalLink* found = FindPhysicalLink(aBDAddr);
	RETURN_IF_NULL_CONNECTION(found);
	found->LinkKeyRequest(aBDAddr, aRequester);
	}

void CPhysicalLinksManager::ReadRemoteSupportedFeaturesComplete(THCIErrorCode aErr, THCIConnHandle aConnH, const TBTFeatures& aBitMask)
	{
	CPhysicalLink* found = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(found);
	found->ReadRemoteSupportedFeaturesComplete(aErr, aConnH, aBitMask);
	}

void CPhysicalLinksManager::ReadRemoteExtendedFeaturesComplete(THCIErrorCode aErr, THCIConnHandle aConnH, TUint64 aBitMask, TUint8 aPageNumber, TUint8 aMaximumPageNumber)
	{
	CPhysicalLink* found = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(found);
	found->ReadRemoteExtendedFeaturesComplete(aErr, aConnH, aBitMask, aPageNumber, aMaximumPageNumber);
	}


void CPhysicalLinksManager::ReadRemoteVersionInfoComplete(THCIErrorCode aErr, THCIConnHandle aConnH, const TBTDevRemoteHwVersion& aVer)
	{
	CPhysicalLink* found = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(found);
	found->ReadRemoteVersionInfoComplete(aErr, aConnH, aVer);
	}

void CPhysicalLinksManager::PacketTypeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewPacket)
	{
	CPhysicalLink* found = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(found);
	found->PacketTypeChange(aErr, aConnH, aNewPacket);
	}

void CPhysicalLinksManager::LinkSupervisionTimeoutChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewTimeout)
	{
	CPhysicalLink* found = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(found);
	found->LinkSupervisionTimeoutChange(aErr, aConnH, aNewTimeout);
	}

TBasebandTime CPhysicalLinksManager::CheckPageTimeoutAgainstLSTO(TBasebandTime aPageTimeout)
	{
	// Calculate the page timeout:
	//   Iterate through all physical links, and take the minimum
	//   LSTO of them all. This is maximum possible page timeout value
	TInt connections = iPhysicalLinks.Count();
	TInt newPageTimeout = aPageTimeout;
	for(TInt i = 0; i < connections; i++)
		{
		// Reduce pageTimeout if greater than LSTO for any physical link
		// LSTO could be zero if no LSTO event received; ignore zero values
		TUint lsto = iPhysicalLinks[i]->LSTO();
		if (lsto > 0 && newPageTimeout > lsto)
			{
			newPageTimeout = lsto;
			}
		}
	
	// The pageTimeout has been changed. Ensure it is 5% less
	// than the minimum LSTO that it has already been set to.
	if (newPageTimeout != aPageTimeout)
		{
		const TReal32 KNinetyFivePercentMultiplier = 0.95;
		newPageTimeout = newPageTimeout * KNinetyFivePercentMultiplier; // Reduce by 5%
		}
	return newPageTimeout;
	}

void CPhysicalLinksManager::MaxSlotsChange(THCIConnHandle aConnH, TUint8 aSlots)
	{
	CPhysicalLink* found = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(found);
	found->MaxSlotsChange(aConnH, aSlots);
	}

void CPhysicalLinksManager::EncryptionKeyRefreshComplete(THCIErrorCode aErr, THCIConnHandle aConnH)
	{
	CPhysicalLink* found = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(found);
	found->EncryptionKeyRefreshComplete(aErr, aConnH);
	}

void CPhysicalLinksManager::ModeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TBTLinkMode aMode, TBasebandTime aInterval)
	{
	CPhysicalLink* found = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(found);
	found->ModeChange(aErr, aConnH, aMode, aInterval);
	}

void CPhysicalLinksManager::RoleChange(THCIErrorCode aErr, const TBTDevAddr& aAddr, TBTBasebandRole aRole)
	{
	// interesting that this is an address, not a Conn handle! Good old H:1.
	CPhysicalLink* found = FindPhysicalLink(aAddr);
	RETURN_IF_NULL_CONNECTION(found);
	found->RoleChange(aErr, aAddr, aRole);
	}

void CPhysicalLinksManager::WriteLinkPolicySettingsCompleteEvent(THCIErrorCode aErr, THCIConnHandle aConnH)
	{
	CPhysicalLink* found = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(found);
	found->WriteLinkPolicySettingsCompleteEvent(aErr, aConnH);
	}

void CPhysicalLinksManager::ClockOffset(THCIErrorCode aErr, THCIConnHandle aConnH, TBasebandTime aClockOffset)
	{
	CPhysicalLink* found = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(found);
	found->ClockOffset(aErr, aConnH, aClockOffset);
	}

void CPhysicalLinksManager::RemoteName(THCIErrorCode aErr, const TBTDevAddr& aAddr, const TBTDeviceName8& aName)
	{
	CPhysicalLink* found = FindPhysicalLink(aAddr);
	RETURN_IF_NULL_CONNECTION(found);
	found->RemoteName(aErr, aAddr, aName);
	}

void CPhysicalLinksManager::ConnectionComplete(THCIErrorCode aErr, const TBTConnect& aConn)
	{
	// we at this stage demux ACL and SCO
	CPhysicalLink* phy = FindPhysicalLink(aConn.iBdaddr);

	if (!phy)
		{
		// to be safe we should cancel all outstanding connections of correct type
		// since some HW leaves the address blank when errroring a connect
		CancelPendingConnections(aErr, aConn);
		// disconnect this weirdo link if it came up
		if (aErr==EOK)
			{
			TRAP_IGNORE(HCIFacade().DisconnectL(aConn.iConnH, ERemoteUserEndedConnection));
			// if error just have to wait for a timeout
			}
		}
	else
		{
		phy->ConnectionComplete(aErr, aConn);
		// good time to update that UI
		LinkManagerProtocol().SetUINumPhysicalLinks(iPhysicalLinks.Count());
		}
	}

void CPhysicalLinksManager::SynchronousConnectionComplete(THCIErrorCode aErr,
													const TBTConnect& aConn,
													const TBTSyncConnectOpts& aSyncOpts)
	{
	__ASSERT_DEBUG(((aConn.iLinkType == ESCOLink) || (aConn.iLinkType == EeSCOLink)), Panic(EBTNonSyncConnectInSyncConnectFunc));

	if((aConn.iLinkType == ESCOLink) || (aConn.iLinkType == EeSCOLink))
		{
		CPhysicalLink* phy = FindPhysicalLink(aConn.iBdaddr);

		if (!phy)
			{
			// to be safe we should cancel all outstanding connections of correct type
			// since some HW leaves the address blank when errroring a connect
			CancelPendingConnections(aErr, aConn);
			// disconnect this weirdo link if it came up
			if (aErr==EOK)
				{
				TRAP_IGNORE(HCIFacade().DisconnectL(aConn.iConnH, ERemoteUserEndedConnection));
				// if error just have to wait for a timeout
				}
			}
		else
			{
			phy->SynchronousConnectionComplete(aErr, aConn, aSyncOpts);
			// good time to update that UI
			LinkManagerProtocol().SetUINumPhysicalLinks(iPhysicalLinks.Count());
			}
		}
	else
		{
		TRAP_IGNORE(HCIFacade().DisconnectL(aConn.iConnH, ERemoteUserEndedConnection));
		}
	}

void CPhysicalLinksManager::MbpcoDeferredDecisionResolved(TInt aError)
	{
	// See if any connection requests are pending,
	// having been held up whilst the stack was
	// being updated with all paired devices
	// in the registry.
	for(TInt i = iPhysicalLinks.Count() - 1; i>=0; i--)
		{
		if(iPhysicalLinks[i]->IsConnectionRequestPending())
			{
			iPhysicalLinks[i]->PendingConnectionRequest(aError); //try now we have a list of paired devices
			}
		}
	}

TInt CPhysicalLinksManager::AddListener(MLogicalLink& aLogicalLink, TPhysicalLinkPort aPort)
	{
	TLogicalLinkListener l;
	l.iObserver = &aLogicalLink;
	l.iPort = aPort;

	TInt retVal = KErrNone;

	// only allowed one listener on either port at moment
	for (TInt i=0; i<iListeners.Count(); i++)
		{
		if (iListeners[i].iPort == aPort)
			{
			retVal = KErrInUse;
			}
		}

	if (retVal == KErrNone)
		{
		retVal = iListeners.Append(l);
		if (retVal == KErrNone)
			{
			retVal = LinkManagerProtocol().IncrementListeners();
			}
		}
	
	return retVal;
	}

/*static*/ TBool CPhysicalLinksManager::ListenerMatch(const TLogicalLinkListener& aA,
										   const TLogicalLinkListener& aB)
	{
	return (aA.iObserver == aB.iObserver);
	}

void CPhysicalLinksManager::RemoveListener(MLogicalLink& aLogicalLink)
	{
	__ASSERT_DEBUG(iListeners.Count(), Panic(EBTConnectionMgrCountBelowZero));

	TLogicalLinkListener key;
	key.iObserver = &aLogicalLink;

	TIdentityRelation<TLogicalLinkListener> relation(ListenerMatch);

	TInt result = iListeners.Find(key, relation);
	__ASSERT_DEBUG(result!=KErrNotFound, Panic(EBTConnectionMgrBadListener));

	// remove from listener queue
	iListeners.Remove(result);
	// and tell protocol we have one fewer
	LinkManagerProtocol().DecrementListeners();
	// ignore err
	}

void CPhysicalLinksManager::CancelPendingConnections(THCIErrorCode aErr, const TBTConnect& aConn)
/**
	When we cannot work out who to notify, we regrettably have to cancel all
**/
	{
	for (TInt index = 0; index < iPhysicalLinks.Count(); index++)
		{
		CPhysicalLink& l = *iPhysicalLinks[index];
		if (   (aConn.iConnH && l.HasHandle(aConn.iConnH))
			|| ((aConn.iLinkType == EACLLink) && l.ACLConnectPending())
			|| ((aConn.iLinkType != EACLLink) && l.SyncConnectPending())
			)
			{
			l.ConnectionComplete(aErr, aConn);
			}
		}
	}

void CPhysicalLinksManager::ConnectionRequest(const TBTConnect& aConn)
	{
	// we haven't got much to go on to find a Listener
	// we don't support directed listening on address
	// so just go on link type
	// this isn't too bad as Bluetooth only has one ACL listener at the moment: L2CAP
	LOG1(_L("Connection request received, link type %d"), aConn.iLinkType);
	
	switch (aConn.iLinkType)
		{
		case EACLLink:
			{
			// this also (see BT1.2) implies that a new PHY is coming up

			// Check that either there exist listeners or the protocol knows
			// that it should stop listening (even if the message has evidently
			// not yet reached the controller).
			__ASSERT_DEBUG(iListeners.Count() || !iLinkManagerProtocol.IsListening(), Panic(EBTConnectionRequestWithNoListeners));
			if (iListeners.Count())
				{
				TRAP_IGNORE(DoConnectionRequestL(aConn));
				}
			else
				{
				// should never reach here
				// leave to timeout - not required
				}
			}
			// ignore err for timeout
			break;
		case ESCOLink:
		case EeSCOLink:
			{
			// should by a non-listening PHY already in place!
			CPhysicalLink* c = FindPhysicalLink(aConn.iBdaddr);
			ASSERT_DEBUG(c);
			c->ConnectionRequest(aConn);
			}
		}
	}


void CPhysicalLinksManager::DoConnectionRequestL(const TBTConnect& aConn)
	{
	// Only called for ACL links
	// ASSERT aConn.iLinkType == EACLLink
	
	// First check that this is not a request for a duplicate address, some hardware
	// can be tediously erratic.
	CPhysicalLink* reqAddr = FindPhysicalLink(aConn.iBdaddr);
	if (!reqAddr)
		{
		CPhysicalLink& c = NewPhysicalLinkL(aConn.iBdaddr);
		// introduce the first listener to the PHY
		// IF WE SUPPORT connections distinguished on aConn then we can find the correct listener 
		// now tell the (new) PHY
		c.ConnectionRequest(aConn);
		}
	else
		{
		// reject incoming connection
		LOG(_L("CPhysicalLinksManager: Duplicate address received - reject"));
		TRAP_IGNORE(HCIFacade().RejectConnectionRequestL(aConn, EHostSecurityRejection));
		}
	}

void CPhysicalLinksManager::Disconnection(THCIErrorCode aErr, THCIConnHandle aConnH, THCIErrorCode aResult)
	{
	// notify the correct connection
	CPhysicalLink* con = FindPhysicalLink(aConnH);
	LOG1(_L("Links manager: disconnection, phys. link 0x%08x"), con);
	RETURN_IF_NULL_CONNECTION(con);
	con->Disconnection(aErr, aConnH, aResult);
	}

void CPhysicalLinksManager::CompletedPackets(THCIConnHandle aConnH, TUint16 aNumPackets)
	{
	// forward to the connections - they can possibly unblock their sockets
	CPhysicalLink* con = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(con);
	con->CompletedPackets(aConnH, aNumPackets);
	
	//now unblock all connections to prevent any deadlock
	TInt connections = iPhysicalLinks.Count();
	for(TInt i = 0; i < connections;i++)
		{
		CPhysicalLink* other_con = iPhysicalLinks[i];
		if(other_con != con)
			{
			other_con->TryToSend();
			}
		}
	}

void CPhysicalLinksManager::AuthenticationComplete(THCIErrorCode aErr, THCIConnHandle aConnH)
	{
	// tell the anonymous notifiers?  could be useful for updating UI?
	CPhysicalLink* con = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(con);
	con->AuthenticationComplete(aErr, aConnH);
	}

void CPhysicalLinksManager::EncryptionChange(THCIErrorCode aErr, THCIConnHandle aConnH,TBool aEncrypted)
	{
	// tell the anonymous notifiers?  could be useful for updating UI?
	CPhysicalLink* con = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(con);
	con->EncryptionChange(aErr, aConnH, aEncrypted);
	}

void CPhysicalLinksManager::ACLDataReceived(THCIConnHandle aConnH, TUint8 aFlag, const TDesC8& aData)
	{
	CPhysicalLink* con = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(con);
	con->ACLDataReceived(aConnH, aFlag, aData);
	}

void CPhysicalLinksManager::SCODataReceived(THCIConnHandle aConnH, const TDesC8& aData)
	{
	CPhysicalLink* con = FindPhysicalLink(aConnH);
	RETURN_IF_NULL_CONNECTION(con);
	con->SCODataReceived(aConnH, aData);
	}

TInt CPhysicalLinksManager::ChangeMode(TBTLinkMode aMode, THCIConnHandle aConnHandle)
	{
	return iHCIRequestHandler.ChangeMode(aMode, aConnHandle);
	}

TInt CPhysicalLinksManager::ExitMode(TBTLinkMode aMode, THCIConnHandle aConnHandle)
	{
	return iHCIRequestHandler.ExitMode(aMode, aConnHandle);
	}

TInt CPhysicalLinksManager::ChangeRole(TBTBasebandRole aRole, CPhysicalLink& aPhysicalLink)
	{
	TRAPD(err, iHCIRequestHandler.ChangeRoleL(aRole, aPhysicalLink.BDAddr()));
	return err;
	}
	
TInt CPhysicalLinksManager::Encrypt(TBool aEnable, CPhysicalLink& aPhysicalLink)
	{
	TRAPD(err, iHCIRequestHandler.SetEncryptL( aPhysicalLink.Handle(), aEnable));
	return err;
	}
	

TBool CPhysicalLinksManager::ActiveConnectRoleSwitchAllowed() const
	{
	// An active connection role change will be allowed in all cases,
	// except when there is an existing master connection from this device.
	if(iPhysicalLinks.Count() > 1)
		{
		if(iPhysicalLinks[0]->Role() == EMaster)
			{
			return EFalse;
			}
		}
	return ETrue;
	}

TBool CPhysicalLinksManager::PassiveConnectBecomeMaster() const
	{
	// If this device is currently working as a master for an existing
	// connection, request a role switch to master.
	return (iPhysicalLinks.Count() > 1 && iPhysicalLinks[0]->Role() == EMaster);
	}

TBool CPhysicalLinksManager::RoleSwitchAllowed() const
	{
	// By default the stack will not try and prevent this device scatterneting.
	// If the BT hardware can't gracefully cope with requests to scatternet (i.e.,
	// disconnects existing phy's if a new scatternet connection is attempted) then
	// this macro should be defined.
#ifdef HARDWARE_DOES_NOT_SUPPORT_SCATTERNET_OPERATION
	return (iPhysicalLinks.Count() <= 1);
#else
	return ETrue;
#endif	
	}


void CPhysicalLinksManager::EnumeratePhysicalLinks(TDes8& aPackagedArrayBuffer)
/**
	This method fills the memory passed from the client with a number of TBTDevAddrs.
	Useful for control panel apps that need to see what links are present

	Because the array passed to us here may be smaller than the number of active links
	at this point, we shall return as much as possible in the array.

	Also if the array has enough space for more addresses than we currently have, the
	descriptor size will be changed to reflect reality.
*/
	{
	TInt connections = Min(iPhysicalLinks.Count(),
							aPackagedArrayBuffer.MaxLength()/sizeof(TBTDevAddr));
	aPackagedArrayBuffer.Zero(); // start from the err.. start and then append

	for(TInt i=0; i<connections; i++)
		{
		if (iPhysicalLinks[i]->IsConnected())
			{
			TBTDevAddr theDevAddr=iPhysicalLinks[i]->BDAddr();

			TPckg<TBTDevAddr> pckg(theDevAddr);
			aPackagedArrayBuffer.Append(pckg);
			}
		}
	}

void CPhysicalLinksManager::RoleChangeRejectedByHW(THCIErrorCode aErr)
	{	
	// we can notify the users about Role Change failure on HW with BTBasebandEvent error	
	// Make sure the Q is not empty before trying to access First()
	if(!iRoleSwitchersQ.IsEmpty())
    	{
    	RoleChange(aErr, iRoleSwitchersQ.First()->BDAddr(), iRoleSwitchersQ.First()->RequestedRole());
		}
	
	}
 
 void CPhysicalLinksManager::AddRoleSwitcher(CRoleSwitcher& aRoleSwitcher)
 	{
 	iRoleSwitchersQ.AddLast( aRoleSwitcher ); // ownership still remains in CPhysicalLink
 
 	if (iRoleSwitchersQ.IsFirst(&aRoleSwitcher))
 		{
 		// this the only role request, it's safe to kick off the state machine
 		// first suspend host resolver
 		LinkManagerProtocol().InquiryMgr().Suspend();
 		aRoleSwitcher.StartHelper();
 		}
 	}
 
 void CPhysicalLinksManager::RemoveRoleSwitcher(CRoleSwitcher& aRoleSwitcher)
 	{
 	TBool startNextRoleSwitcher = EFalse;
 	
 	if (iRoleSwitchersQ.IsFirst(&aRoleSwitcher))
 		{
 		startNextRoleSwitcher = ETrue;
 		}
 
	if (!iRoleSwitchersQ.IsEmpty())
		{
		iRoleSwitchersQ.Remove(aRoleSwitcher);
		}
 	
 	if (startNextRoleSwitcher && !iRoleSwitchersQ.IsEmpty())
 		{
 		iRoleSwitchersQ.First()->StartHelper();
 		}
 	else 
 		{
 		// resume host resolver
 		LinkManagerProtocol().InquiryMgr().Resume();
 		}
 	}

TInt CPhysicalLinksManager::GetConnectionHandles(RHCIConnHandleArray& aConnectionHandles, TLinkType aLinkType) const
	{
	aConnectionHandles.Reset(); // sub-optimal

	const TInt phycount(iPhysicalLinks.Count());
	if (phycount == 0)
		{
		return KErrNone;
		}

	TInt err(aConnectionHandles.Reserve(phycount));

	if (err != KErrNone)
		{
		return err;
		}

	for (TInt i(0); i < phycount; ++i)
		{
		const CPhysicalLink* const phy(iPhysicalLinks[i]);
		if (!phy)
			{
			return KErrNotFound;
			}

		// append to aConnectionHandles
		err = phy->GetConnectionHandles(aConnectionHandles, aLinkType);

		if (err != KErrNone)
			{
			return err;
			}
		}

	return KErrNone;
	}

TInt CPhysicalLinksManager::GetNumPendingHandles(TInt& aConnectionHandles, TLinkType aLinkType) const
	{
	aConnectionHandles = 0;
	
	const TInt phycount(iPhysicalLinks.Count());
	if (phycount == 0)
		{
		return KErrNone;
		}

	for (TInt i(0); i < phycount; ++i)
		{
		const CPhysicalLink* const phy(iPhysicalLinks[i]);
		if (!phy)
			{
			return KErrNotFound;
			}

		TInt count(0);
		// append to aConnectionHandles
		TInt err(phy->GetNumPendingHandles(count, aLinkType));

		if (err != KErrNone)
			{
			return err;
			}

		aConnectionHandles += count;
		}

	return KErrNone;
	}

TInt CPhysicalLinksManager::GetConnectionHandles(RHCIConnHandleArray& aConnectionHandles, TLinkType aLinkType, const TBTDevAddr& aBDAddr) const
	{
	aConnectionHandles.Reset(); // sub-optimal

	const CPhysicalLink* const phy(FindPhysicalLink(aBDAddr));

	if (!phy)
		{
		// In the event of an error the client should not inspect
		// aConnectionHandle.
		return KErrNotFound;
		}

	return phy->GetConnectionHandles(aConnectionHandles,
									 aLinkType);
	}

TInt CPhysicalLinksManager::GetNumPendingHandles(TInt& aConnectionHandles, TLinkType aLinkType, const TBTDevAddr& aBDAddr) const
	{
	aConnectionHandles = 0;

	const CPhysicalLink* const phy(FindPhysicalLink(aBDAddr));

	if (!phy)
		{
		// In the event of an error the client should not inspect
		// aConnectionHandle.
		return KErrNotFound;
		}

	return phy->GetNumPendingHandles(aConnectionHandles, aLinkType);
	}

TInt CPhysicalLinksManager::GetRemoteAddress(TBTDevAddr& aBDAddr, THCIConnHandle aConnectionHandle) const
	{
	const CPhysicalLink* const phy(FindPhysicalLink(aConnectionHandle));

	if (!phy)
		{
		// In the event of an error the client should not inspect
		// aBDAddr.
		return KErrNotFound;
		}

	aBDAddr = phy->BDAddr();
	return KErrNone;
	}

TInt CPhysicalLinksManager::GetRemoteDeviceClass(TBTDeviceClass& aDeviceClass, const TBTDevAddr& aBDAddr) const
	{
	const CPhysicalLink* const phy(FindPhysicalLink(aBDAddr));

	if (!phy)
		{
		// In the event of an error the client should not inspect
		// aBasebandLinkState.
		return KErrNotFound;
		}

	aDeviceClass = phy->DeviceClass();
	return KErrNone;
	}

TInt CPhysicalLinksManager::GetRemoteSupportedFeatures(TBTFeatures& aRemoteSupportedFeatures, const TBTDevAddr& aBDAddr) const
	{
	const CPhysicalLink* const phy(FindPhysicalLink(aBDAddr));

	if (!phy)
		{
		// In the event of an error the client should not inspect
		// aRemoteSupportedFeatures.
		return KErrNotFound;
		}

	aRemoteSupportedFeatures = phy->RemoteFeatures();
	return KErrNone;
	}

TInt CPhysicalLinksManager::GetLinkPolicySettings(TLinkPolicy& aLinkPolicySettings, const TBTDevAddr& aBDAddr) const
	{
	const CPhysicalLink* const phy(FindPhysicalLink(aBDAddr));

	if (!phy)
		{
		// In the event of an error the client should not inspect
		// aLinkPolicySettings.
		return KErrNotFound;
		}

	aLinkPolicySettings = phy->LinkPolicy();
	return KErrNone;
	}

TInt CPhysicalLinksManager::GetBasebandLinkState(TBTBasebandLinkState& aBasebandLinkState, const TBTDevAddr& aBDAddr) const
	{
	const CPhysicalLink* const phy(FindPhysicalLink(aBDAddr));

	if (!phy)
		{
		// In the event of an error the client should not inspect
		// aBasebandLinkState.
		return KErrNotFound;
		}

	aBasebandLinkState = phy->LinkState();
	return KErrNone;
	}


//
// TPageTimeoutController
//

TPageTimeoutController::TPageTimeoutController(MHCICommandQueue& aCommandQueue)
	: iCommandQueue(aCommandQueue)
	, iOutstandingCommands(0)
	{
	}

void TPageTimeoutController::Abort()
	{
	iCommandQueue.MhcqRemoveAllCommands(*this);
	}

void TPageTimeoutController::WritePageTimeout(TBasebandTime aPageTimeout)
	{
	// Setting the page-timeout is typically best effort, hence this non-erroring
	// API.
	TRAP_IGNORE(WritePageTimeoutL(aPageTimeout));
	}

void TPageTimeoutController::MhcqcCommandEventReceived(const THCIEventBase& /*aEvent*/, const CHCICommandBase* /*aRelatedCommand*/)
	{
	// Nothing else needs to be done.
	--iOutstandingCommands;
	}

void TPageTimeoutController::MhcqcCommandErrored(TInt /*aErrorCode*/, const CHCICommandBase* /*aCommand*/)
	{
	// This is a best effort service so errors are just dropped.
	--iOutstandingCommands;
	}

void TPageTimeoutController::WritePageTimeoutL(TBasebandTime aPageTimeout)
	{
	if(iOutstandingCommands)
		{
		// best effort removal of any existing command on the queue.
		static_cast<void>(iCommandQueue.MhcqRemoveCommand(iLastCommandId, *this));
		}
	CWritePageTimeoutCommand* cmd = CWritePageTimeoutCommand::NewL(aPageTimeout);
	// ownership of cmd is taken by command queue.
	iLastCommandId = iCommandQueue.MhcqAddCommandL(cmd, *this); // "leak" any previous command id.
	++iOutstandingCommands;
	}



CBluetoothPrefetchManager* CBluetoothPrefetchManager::NewL()
	{
	CBluetoothPrefetchManager* self = new(ELeave) CBluetoothPrefetchManager;
	return self;
	}

CBluetoothPrefetchManager::CBluetoothPrefetchManager()
	{
	}

CBluetoothPrefetchManager::~CBluetoothPrefetchManager()
	{
	while(iPinRequesters.Count() > 0)
		{
		iPinRequesters[0].CompleteRequest(KErrCancel);
		iPinRequesters.Remove(0);
		}
	iPinRequesters.Close();
	}

TBool CBluetoothPrefetchManager::CompareAddressInRequest(const TBTDevAddr* aDevAddr, const RPinRequest& aRequest)
	{
	return aDevAddr && *aDevAddr == aRequest.iAddr;
	}

TBool CBluetoothPrefetchManager::CompareAddressInStore(const TBTDevAddr* aDevAddr, const TPrefetchedPin& aPin)
	{
	return aDevAddr && *aDevAddr == aPin.iAddr;
	}

TInt CBluetoothPrefetchManager::HandleOverPinRequester(const TBTDevAddr& aAddr, CBTPinRequester* aPinRequester)
	{
	RPinRequest req;
	TInt err = req.Create(aAddr, aPinRequester);
	if(err == KErrNone)
		{
		err = iPinRequesters.Append(req);
		}
	if(err == KErrNone)
		{
		aPinRequester->UpdateHandler(*this);
		}
	return err;
	}

TInt CBluetoothPrefetchManager::RegisterForPrefetching(const TBTDevAddr& aAddr, MBluetoothPrefetchNotifier& aNotifier)
	{
	TInt ix = iPinRequesters.Find(aAddr, CompareAddressInRequest);
	if (ix < 0)
		{
		return ix;
		}
	iPinRequesters[ix].AddNotifier(aNotifier);
	return KErrNone;
	}


TInt CBluetoothPrefetchManager::IsPrefetchAvailable(const TBTDevAddr& aAddr) const
    {
    return iPrefetchedPins.Find(aAddr, CompareAddressInStore);
    }

TBool CBluetoothPrefetchManager::GetPrefetch(const TBTDevAddr& aAddr, TBTPinCode& aPinCode) const
    {
    TInt ix = IsPrefetchAvailable(aAddr);
    if (ix >= 0)
        {
        aPinCode.Copy(iPrefetchedPins[ix].iPin);
        return ETrue;
        }
    return EFalse;
    }

void CBluetoothPrefetchManager::RemovePrefetch(const TBTDevAddr& aAddr)
	{
	TInt ix = IsPrefetchAvailable(aAddr);
	if (ix >= 0)
		{
        iPrefetchedPins.Remove(ix);
		}
	}

TInt CBluetoothPrefetchManager::PINCodeRequestReply(const TBTDevAddr& aDevAddr, const TDesC8& aPin) const
	{
	TInt ix = iPinRequesters.Find(aDevAddr, CompareAddressInRequest);
	__ASSERT_DEBUG(ix >= 0, Panic(EBTPrefetchManagerReplyForNoRequest));

	// Always check if someone is prefetching before storing a prefetch result.
	TInt result = EBTSecManAccessDenied;
	if(iPinRequesters[ix].NotifiersWaiting())
		{
		TPrefetchedPin pin;
		pin.iAddr = aDevAddr;
		pin.iPin.Copy(aPin);
		result = iPrefetchedPins.Append(pin);
		}
	iPinRequesters[ix].CompleteRequest(result);
	iPinRequesters.Remove(ix);
	return KErrNone;
	}

TInt CBluetoothPrefetchManager::PINCodeRequestNegativeReply(const TBTDevAddr& aDevAddr) const
	{
	TInt ix = iPinRequesters.Find(aDevAddr, CompareAddressInRequest);
	__ASSERT_DEBUG(ix >= 0, Panic(EBTPrefetchManagerReplyForNoRequest));
	iPinRequesters[ix].CompleteRequest(EBTSecManAccessDenied);
	iPinRequesters.Remove(ix);
	return KErrNone;
	}

CBluetoothPrefetchManager::RPinRequest::RPinRequest()
	: iNotifiers(NULL)
	{
	}

TInt CBluetoothPrefetchManager::RPinRequest::Create(const TBTDevAddr& aAddr, CBTPinRequester* aPinRequester)
	{
	iAddr = aAddr;
	iPinRequester = aPinRequester;
	iNotifiers = new TDblQue<TPrefetchNotifierQLink>();
	return iNotifiers ? KErrNone : KErrNoMemory;
	}

void CBluetoothPrefetchManager::RPinRequest::AddNotifier(MBluetoothPrefetchNotifier& aNotifier)
	{
	iNotifiers->AddLast(aNotifier.MbpnQueLink());
	}

TBool CBluetoothPrefetchManager::RPinRequest::NotifiersWaiting() const
	{
	return !iNotifiers->IsEmpty();
	}

void CBluetoothPrefetchManager::RPinRequest::CompleteRequest(TInt aError)
	{
	while(!iNotifiers->IsEmpty())
		{
		MBluetoothPrefetchNotifier* notifier = iNotifiers->First()->Item();
		ASSERT_DEBUG(notifier);
		notifier->MbpnQueLink().Deque();
		notifier->MbpnPrefetchComplete(aError);
		}
	delete iPinRequester;
	iPinRequester = NULL;
	delete iNotifiers;
	iNotifiers = NULL;
	}


