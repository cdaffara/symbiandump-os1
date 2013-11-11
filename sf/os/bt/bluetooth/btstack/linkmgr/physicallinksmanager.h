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

#ifndef PHYSICALLINKSMANAGER_H_
#define PHYSICALLINKSMANAGER_H_

#include "MBtHostControllerEventInternalNotifier.h"
#include "linkmuxer.h"
#include "linkmgr.h"
#include "bluetooth/physicallinksstate.h"
#include "RegistryHelpers.h"
#include "pairingscache.h"

#include <bt_sock.h>

static const TInt KBTBasebandConnectionArrayGranularity = 4;
static const TInt KBTStateChangeSubscriberArrayGranularity = 2;

class CBTBasebandModel;
class CHCIFacade;
class CBTSecMan;
class RBTRegServ;
class CPhysicalLink;
class CBTProxySAP;
class MPINCodeResponseHandler;
class MLinkKeyResponseHandler;
class MLogicalLink;

struct TLogicalLinkListener;
class CRoleSwitcher;
class CRoleSwitcherStateFactory;

NONSHARABLE_CLASS(TPageTimeoutController) : public MHCICommandQueueClient
	{
public:
	TPageTimeoutController(MHCICommandQueue& aCommandQueue);
	void Abort();

	void WritePageTimeout(TBasebandTime aPageTimeout);

private:	// events from MHCICommandQueueClient
	virtual void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	virtual void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);

private:
	void WritePageTimeoutL(TBasebandTime aPageTimeout);

private:
	MHCICommandQueue&	iCommandQueue;
	TInt				iOutstandingCommands;
	TInt				iLastCommandId;
	};


NONSHARABLE_CLASS(CBluetoothPrefetchManager)
	: public CBase
	, public MPINCodeResponseHandler
	{
public:
	static CBluetoothPrefetchManager* NewL();
	~CBluetoothPrefetchManager();

	TInt HandleOverPinRequester(const TBTDevAddr& aAddr, CBTPinRequester* aPinRequester);
	TInt RegisterForPrefetching(const TBTDevAddr& aAddr, MBluetoothPrefetchNotifier& aNotifier);

	TBool GetPrefetch(const TBTDevAddr& aAddr, TBTPinCode& aPinCode) const;
	void RemovePrefetch(const TBTDevAddr& aAddr);

private:
	NONSHARABLE_CLASS(RPinRequest)
		{
	friend class CBluetoothPrefetchManager;
	public:
		RPinRequest();
		TInt Create(const TBTDevAddr& aAddr, CBTPinRequester* aPinRequester);
		void AddNotifier(MBluetoothPrefetchNotifier& aNotifier);
		TBool NotifiersWaiting() const;
		void CompleteRequest(TInt aResult);

	private:
		TBTDevAddr							iAddr;
		CBTPinRequester*					iPinRequester;
		TDblQue<TPrefetchNotifierQLink>*	iNotifiers;
		};

	NONSHARABLE_STRUCT(TPrefetchedPin)
		{
		TBTDevAddr		iAddr;
		TBTPinCode		iPin;
		};

private:
	CBluetoothPrefetchManager();

	static TBool CompareAddressInRequest(const TBTDevAddr* aDevAddr, const RPinRequest& aRequest);
	static TBool CompareAddressInStore(const TBTDevAddr* aDevAddr, const TPrefetchedPin& aRequest);
	
	TInt IsPrefetchAvailable(const TBTDevAddr& aAddr) const;

private: // from MPINCodeResponseHandler
	TInt PINCodeRequestReply(const TBTDevAddr& aDevAddr, const TDesC8& aPin) const;
	TInt PINCodeRequestNegativeReply(const TBTDevAddr& aDevAddr) const;

private:
	mutable RArray<RPinRequest>		iPinRequesters;
	mutable RArray<TPrefetchedPin>	iPrefetchedPins;
	};


NONSHARABLE_CLASS(CPhysicalLinksManager) : public CBase,
							  public MBtHostControllerEventInternalNotifier,
							  public MBTPairingsCacheObserver
/**
	Manages CPhysicalLink objects.
	One connections manager exists to take care of all CPhysicalLink objects.
*/
	{
public:

	static CPhysicalLinksManager* NewL(CLinkMgrProtocol& aLinkMgrProtocol,
									   CHCIFacade& aHCIFacade,
									   CRegistrySession& aRegSess,
									   CBTSecMan& aSecMan,
									   CBTCodServiceMan& aCodMan);
	~CPhysicalLinksManager();
	inline CBTSecMan& SecMan() const;
	inline CBTCodServiceMan& CodMan() const;
	inline CBTBasebandModel& Baseband() const;
	CPhysicalLink& NewPhysicalLinkL(const TBTDevAddr& aDevice);			// factory function
	CPhysicalLink& NewPhysicalLinkL(const TBTNamelessDevice& aDevice);	// factory function
	void AuthenticateL(const TBTDevAddr& aBDAddr);
	void EncryptL(const TBTDevAddr& aBDAddr);

	void RemovePhysicalLink(const CPhysicalLink& aConnection);
	TInt AddListener(MLogicalLink& aLogicalLink, TPhysicalLinkPort aPort);
	void RemoveListener(MLogicalLink& aLogicalLink);
	void ClearTerminatingProxy(CBTProxySAP* aProxySAP);
	TInt TerminateAllPhysicalLinks(CBTProxySAP* aProxySAP, THCIErrorCode aErrorCode);
	TInt TerminatePhysicalLink(CPhysicalLink* aConnection, CBTProxySAP* aProxySAP);

	void FatalError(TInt aErr);
	void UpdateRemoteDevicesDetails();
	TInt RefreshPairedBDAddrs();
	
	void SetAcceptPairedOnlyMode(TBool aOn);
	void SetSimplePairingDebugMode(TBool aOn);
	
	void SimplePairingDebugModeChanged(TBool aOn);

	inline TInt NumberOfPhysicalLinks() const;
	void EnumeratePhysicalLinks(TDes8& aPackagedArray);
	void DumpPhysicalLinks();
	TBool IsAcceptPairedOnlyMode();
	
	void RequireSlotSpace();	// for HostResolver (or others) who need to reduce connection commitments
	TBasebandTime TryToChangePageTimeout(TBasebandTime aPageTimeout);
	
	// role change methods to control locally initiated requests
	void AddRoleSwitcher(CRoleSwitcher& aRoleSwitcher);
	// This method must be called only from the D'tor of CRoleSwitcher	
	void RemoveRoleSwitcher(CRoleSwitcher& aRoleSwitcher);
	// to enable to generate BTBaseband event, RoleChange error in case of RoleChange
	// command was 'not allowed' by HW
	void RoleChangeRejectedByHW(THCIErrorCode aErr);
	CRoleSwitcherStateFactory& RoleSwitcherStateFactory() const;
	
public:	
			
	
// events from MBtHostControllerEventInternalNotifier
	virtual void ConnectionComplete(THCIErrorCode aErr, const TBTConnect& aConn);
	virtual void ConnectionRequest(const TBTConnect& aConn);
	virtual void Disconnection(THCIErrorCode aErr, THCIConnHandle aConnH, THCIErrorCode aResult);
	virtual void CompletedPackets(THCIConnHandle aConnH, TUint16 aNumPackets);
	virtual void AuthenticationComplete(THCIErrorCode aErr, THCIConnHandle aConnH);
	virtual void EncryptionChange(THCIErrorCode aErr, THCIConnHandle aConnH,TBool aEncrypted);
	virtual void ACLDataReceived(THCIConnHandle aConnH, TUint8 aFlag, const TDesC8& aData);
	virtual void SCODataReceived(THCIConnHandle aConnH, const TDesC8& aData);
	virtual void SynchronousConnectionComplete(THCIErrorCode aErr, const TBTConnect& aConn, const TBTSyncConnectOpts& aSyncOpts);
	virtual void PinRequest(const TBTDevAddr& aBDAddr, MPINCodeResponseHandler& aRequester);
	virtual void NewLinkKey(const TBTDevAddr& aBDAddr, const TBTLinkKey& aLinkKey, THCILinkKeyType aLinkKeyType);
	virtual void LinkKeyRequest(const TBTDevAddr& aBDAddr, MLinkKeyResponseHandler& aRequester);
	virtual void ReadRemoteSupportedFeaturesComplete(THCIErrorCode aErr, THCIConnHandle aConnH, const TBTFeatures& aBitMask);
	virtual void ReadRemoteExtendedFeaturesComplete(THCIErrorCode aErr, THCIConnHandle aConnH, TUint64 aBitMask, TUint8 aPageNumber, TUint8 aMaximimPageNumber);
	virtual void ReadRemoteVersionInfoComplete(THCIErrorCode aErr, THCIConnHandle aConnH, const TBTDevRemoteHwVersion& aVer);
	virtual void PacketTypeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewPacket);
	virtual void LinkSupervisionTimeoutChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewTimeout);
	virtual void MaxSlotsChange(THCIConnHandle aConnH, TUint8 aSlots);
	virtual void ModeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TBTLinkMode aMode, TBasebandTime aInterval);
	virtual void WriteLinkPolicySettingsCompleteEvent(THCIErrorCode aErr, THCIConnHandle aConnH);
	virtual void RoleChange(THCIErrorCode aErr, const TBTDevAddr& aAddr, TBTBasebandRole aRole);
	virtual void ClockOffset(THCIErrorCode aErr, THCIConnHandle aConnH, TBasebandTime aClockOffset);
	virtual void RemoteName(THCIErrorCode aErr, const TBTDevAddr& aAddr, const TBTDeviceName8& aName);
	virtual void SimplePairingComplete(const TBTDevAddr& aBDAddr, THCIErrorCode aErr);
	virtual void EncryptionKeyRefreshComplete(THCIErrorCode aErr, THCIConnHandle aConnH);


// events from MBTPairingsCacheObserver
	virtual void MbpcoDeferredDecisionResolved(TInt aError);

// Getters
	inline CHCIFacade& HCIFacade() const;
	inline CLinkMgrProtocol& LinkManagerProtocol() const;
	inline CBTPairingsCache& PairingsCache() const;
	inline CBluetoothPrefetchManager& PrefetchMan() const;

	CPhysicalLink* FindPhysicalLink() const;	// mainly for finding random coonected PHY for proxy SAP
	CPhysicalLink* FindPhysicalLink(THCIConnHandle aConnH) const; // mainly for LinkMgrProtocol
	CPhysicalLink* FindPhysicalLink(const TBTDevAddr& aBDAddr) const;
	TLogicalLinkListener* FindListener(TPhysicalLinkPort aPort);

	TInt ChangeMode(TBTLinkMode aMode, THCIConnHandle aConnHandle);
	TInt ExitMode(TBTLinkMode aMode, THCIConnHandle aConnHandle);
	TInt ChangeRole(TBTBasebandRole aRole, CPhysicalLink& aPhysicalLink);
	TInt Encrypt(TBool aEnable, CPhysicalLink& aPhysicalLink);
	
	TBool ActiveConnectRoleSwitchAllowed() const;
	TBool PassiveConnectBecomeMaster() const;
	TBool RoleSwitchAllowed() const;
	
	void ArbitrateAllPhysicalLinks();

	TInt GetConnectionHandles(RHCIConnHandleArray& aConnectionHandles, TLinkType aLinkType) const;
	TInt GetNumPendingHandles(TInt& aConnectionHandles, TLinkType aLinkType) const;
	TInt GetConnectionHandles(RHCIConnHandleArray& aConnectionHandles, TLinkType aLinkType, const TBTDevAddr& aBDAddr) const;
	TInt GetNumPendingHandles(TInt& aConnectionHandles, TLinkType aLinkType, const TBTDevAddr& aBDAddr) const;
	TInt GetRemoteAddress(TBTDevAddr& aBDAddr, THCIConnHandle aConnectionHandle) const;
	TInt GetRemoteDeviceClass(TBTDeviceClass& aDeviceClass, const TBTDevAddr& aBDAddr) const;
	TInt GetRemoteSupportedFeatures(TBTFeatures& aRemoteSupportedFeatures, const TBTDevAddr& aBDAddr) const;
	TInt GetLinkPolicySettings(TLinkPolicy& aLinkPolicySettings, const TBTDevAddr& aBDAddr) const;
	TInt GetBasebandLinkState(TBTBasebandLinkState& aBasebandLinkState, const TBTDevAddr& aBDAddr) const;

private:

	CPhysicalLinksManager(CLinkMgrProtocol& aLinkMgrProtocol,
						  CHCIFacade& aHCIFacade,
						  CRegistrySession& aRegSess, CBTSecMan& aSecMan, CBTCodServiceMan& aCodMan);
	void ConstructL();
	void CancelPendingConnections(THCIErrorCode aErr, const TBTConnect& aConn);
	void DoConnectionRequestL(const TBTConnect& aConn);
	static TBool ListenerMatch(const TLogicalLinkListener& aA, const TLogicalLinkListener& aB);
	void UpdateTerminatingProxy(CBTProxySAP* aProxySAP);

	TBasebandTime CheckPageTimeoutAgainstLSTO(TBasebandTime aPageTimeout);

private:
	RPointerArray<CPhysicalLink>	iPhysicalLinks;

	CHCIFacade&						iHCIRequestHandler;
	CRegistrySession&				iRegSess;	// we don't "own" an "access".
	CBTSecMan&						iSecMan;
	CBTCodServiceMan&				iCodMan;
	CLinkMgrProtocol&				iLinkManagerProtocol;

	CBTProxySAP*					iTerminatingProxy; // unowned
	CPhysicalLink*					iTerminatingConnection; // unowned
	
	CBTBasebandModel*				iBaseband;
	RArray<TLogicalLinkListener>	iListeners;

	// role change control variables
	TSglQue<CRoleSwitcher> 			iRoleSwitchersQ;
	CRoleSwitcherStateFactory*		iRoleSwitcherStateFactory;
	
	TBool							iAcceptPairedOnlyMode;
	CBTPairingsCache*				iPairingsCache;

	TPageTimeoutController			iPageTimeoutController;
	CBluetoothPrefetchManager*		iPrefetchMan;
	};

inline CLinkMgrProtocol& CPhysicalLinksManager::LinkManagerProtocol() const
	{
	return iLinkManagerProtocol;
	}

inline CHCIFacade& CPhysicalLinksManager::HCIFacade() const
	{
	return iHCIRequestHandler;
	}

inline CBTSecMan& CPhysicalLinksManager::SecMan() const
	{
	return iSecMan;
	}

inline CBTBasebandModel& CPhysicalLinksManager::Baseband() const
	{
	return *iBaseband;
	}

inline TInt CPhysicalLinksManager::NumberOfPhysicalLinks() const
	{
	return iPhysicalLinks.Count();
	}

inline CBTCodServiceMan& CPhysicalLinksManager::CodMan() const
	{
	return iCodMan;
	}

inline CBTPairingsCache& CPhysicalLinksManager::PairingsCache() const
	{
	return *iPairingsCache;
	}

inline CRoleSwitcherStateFactory& CPhysicalLinksManager::RoleSwitcherStateFactory() const
	{
	return *iRoleSwitcherStateFactory;
	}

inline CBluetoothPrefetchManager& CPhysicalLinksManager::PrefetchMan() const
	{
	return *iPrefetchMan;
	}

#endif //PHYSICALLINKSMANAGER
