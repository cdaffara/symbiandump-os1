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
//

#ifndef _PHYSICALLINKS_H
#define _PHYSICALLINKS_H

#include "linkmgr.h"
#include "RegistryHelpers.h"
#include "MBtHostControllerEventInternalNotifier.h"
#include "basebandsap.h"
#include "bluetooth/physicallinksstate.h"
#include "physicallinkmetrics.h"

#include <bt_sock.h>
#include <btmanclient.h>
#include <bluetooth/hcicommandqueueclient.h>
#include <bluetooth/hcicmdqcontroller.h>


static const TBTDevRemoteHwVersion KInvalidRemoteHwVersion={0xFF};
static const TUint64 KInvalidRemoteFeatures = KMaxTUint64; 

static const TInt KBTArbitrationDelay = 1000000;	// 1s

class CPhysicalLink;
class CBTSynchronousLink;
class CBTProxySAP;
class MPINCodeResponseHandler;
class MLinkKeyResponseHandler;
class CBTRegistryHelperBase;
class MPhysicalLinkObserver;
class CBTPinRequester;
class CACLLink;
class CRoleSwitcher;
class CBTNumericComparator;
class CBTPasskeyEntry;
class CEncryptionKeyRefresher;

enum TPhysicalLinkSimplePairingMode
	{
	EPhySimplePairingUndefined,
	EPhySimplePairingDisabled,
	EPhySimplePairingEnabled,
	};

enum TAuthStates
	{
	EAuthenticationRequestPending = 0x1,
	ELinkKeyRequestPending = 0x2,
	EPinRequestPending = 0x4,
	ESimplePairingPending = 0x8,
	};	

NONSHARABLE_CLASS(CArbitrationDelayTimer) : public CTimer
/**
	This class provides a timer which is used when the physical link goes into active mode
	to prevent an early switch back into a low power mode.
**/
	{
public:
	static CArbitrationDelayTimer* NewL(CPhysicalLink* aParent);
	TInt Start(TBool aImmediate, TBool aLocalPriority);
	void Restart();

private:
	CArbitrationDelayTimer(CPhysicalLink* aParent);
	void ConstructL();
	void RunL();
	void DoCancel();
	TInt DoArbitrate();
	void CancelButPreserveLocalPriority();

private:
	CPhysicalLink* iParent;
	TBool iLocalPriority;
	};


NONSHARABLE_STRUCT(TLogicalLinkListener)
	{
	MLogicalLink*		iObserver;
	TPhysicalLinkPort	iPort;
	};

/**
	The following class controls low power mode command requests and related events. 
	It bypasses HCI_Facade to keep deep control of all events including errors. 
	Its aim is also to forbid having more than one outstanding low power mode command.
	It doesn't manage the trasmission of the "cancel" command when the other one has
	completed; it simply doesn't send a command if another one is pending.
	However, using the Arbitrate and SAPSetOption mechanism, if a command is first dropped
	because another one is pending, it will be executed (if necessary) when the new
	arbitrate method will be called.
	The class has a flag set when a new command is delivered and reset when the modechange
	event is received or an error occurs. When the flag is set no other commands are executed.
**/
NONSHARABLE_CLASS(TLowPowModeCmdController) : public MHCICommandQueueClient
	{
public:
	TLowPowModeCmdController(CPhysicalLink& aLink, MHCICommandQueue& aCmdController);
	void Abort();
	
	TInt ExitMode(TBTLinkMode aMode, THCIConnHandle aHandle);
	TInt ChangeMode(TBTLinkMode aMode, THCIConnHandle aHandle);
	
	TInt ExecuteModeChange(TBTLinkMode aTargetMode);

private:	// events from MHCICommandQueueClient
	virtual void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	virtual void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);
	
private:
	void DoChangeModeL(TBTLinkMode aMode, THCIConnHandle aConnHandle);
	void DoExitModeL(TBTLinkMode aMode, THCIConnHandle aConnHandle);
	
	void SniffL(THCIConnHandle aHandleToRemote);
	void ExitSniffL(THCIConnHandle aHandleToRemote);
	void HoldL(THCIConnHandle aHandle);
	void ParkL(THCIConnHandle aHandleToRemote);
	void ExitParkL(THCIConnHandle aHandleToRemote);
	
private:
	CPhysicalLink&		iParent;
	MHCICommandQueue&	iCmdController;
	TBool				iOutstandingCmd;
	TBTLinkMode			iTargetMode;
	};

/**
	The following class controls disconnect commands and related events. 
	It bypasses HCI_Facade to keep deep control of all events including errors. 
**/
NONSHARABLE_CLASS(TDisconnectCmdController) : public MHCICommandQueueClient
	{
public:
	TDisconnectCmdController(CPhysicalLink& aLink, MHCICommandQueue& aCmdController);
	void Abort();
	
	TInt Disconnect(THCIErrorCode aReason);

private:	// events from MHCICommandQueueClient
	virtual void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	virtual void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);
	
private:
	void DoDisconnectL(THCIErrorCode aReason);

private:
	CPhysicalLink&		iParent;
	MHCICommandQueue&	iCmdController;
	};


/**
	The following class controls authentication requests and related events. 
	It bypasses HCI_Facade to keep deep control of all events including errors. 
**/

NONSHARABLE_CLASS(TAuthenticationCmdController) : public MHCICommandQueueClient
	{
public:
	TAuthenticationCmdController(CPhysicalLink& aLink, MHCICommandQueue& aCmdController);
	void Abort();
	
	// events from MLinkKeyResponseHandler
    TInt LinkKeyRequestReply(const TBTDevAddr& aBdaddr, const TDesC8& aLinkKey) const;
    TInt LinkKeyRequestNegativeReply(const TBTDevAddr& aBdaddr) const;

private:	// events from MHCICommandQueueClient
	virtual void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	virtual void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);
	
private:
	CPhysicalLink&		iParent;
	MHCICommandQueue&	iCmdController;
	};

/**
 * CEncryptionEnforcer.
 * The following class avoid that a wanted encrypted connection becomes an unencrypted connection permanently.
 * Due to the use of a timer, it switchs the encryption on when a previous "switch off" command is not paired
 * with the corrisponding "switch on" one. 
 * i.e.: due to a switch role the remote device disables the encryption but it doesn't enables it when the
 * operation is finished. In this case, after a while, CEncryptionEnforcer enables the encryption on its own.
 * If a request to enable the encryption is refused CEncryptionEnforcer try to disconnect the physical link
 * because is not wanted that a requested encrypted connection is unencrypted.
 **/
NONSHARABLE_CLASS(CEncryptionEnforcer) : public CActive
	{
private:
	enum TState
		{
		EInactive,
		ENoRoleSwitchTimerInProgress,
		ERoleSwitchTimerInProgress,
		EForcingInProgress,
		};
public:
	~CEncryptionEnforcer();
	static CEncryptionEnforcer* NewL(CPhysicalLink& aLink, THCIEncryptModeFlag aEncryptionMode);
	static CEncryptionEnforcer* NewLC(CPhysicalLink& aLink, THCIEncryptModeFlag aEncryptionMode);
	
	void EncryptionEnabled();
	void EncryptionDisabled(TBool aEncryptionPauseResumeSupported);
	void RoleSwitchEvent();
	
private:
	CEncryptionEnforcer(CPhysicalLink& aLink, THCIEncryptModeFlag aEncryptionMode);
	void ConstructL();
	void ChangeState(TState aState);
	void Start(TInt delay);
	void Stop();

	// inherited from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	static const TInt	KTimeOutDelay		= 	5000000;	// 5 secs delay
	
	CPhysicalLink&		iLink;
	THCIEncryptModeFlag	iEncryptionMode;
	TState				iState;
	RTimer				iTimer;
	};

NONSHARABLE_CLASS(XAutoKeyRefreshToken): public MBluetoothControlPlaneToken
	{
public:
	TDblQueLink iQueLink;
private:
	virtual void Release();
	};
	
/**
 	This class avoid requesting to store device info to the registry, if a pending 
 	request for the same device info already exists.
 **/
NONSHARABLE_CLASS(TRegistryDeviceBeingModified)
	{
public:
	TRegistryDeviceBeingModified();
	TBool InUse() const;
	TBool IsEqual(const TBTNamelessDevice& aDevice) const;
	void Begin(const TBTNamelessDevice& aDevice);
	void RequestCompleted();
private:
	TInt					iReferenceCount;	
	TBTNamelessDevice 		iDevice;
	};

/**
	The following class captures all the 'physical link' stuff such as
	whether parked/sniffed, role, etc; to a *given* device - as that is 
	what can be done: and is incorrect to talk about a parked 'ACL' transport
	as its a physical property that impacts SCO too.

	At present the stack does not keep track of which of these are in the
	same piconet (hopping sequence): that would be a PhysicalLink class

	Whether there is need for that - possibly for piconet broadcast - then
	there might be a use for a CBTBroadcast connection - which would begin
	to reflect a piconet

 **/
NONSHARABLE_CLASS(CPhysicalLink) : public CBase,
					  public MBTRegistryTaskNotifier,
					  public MBtHostControllerEventInternalNotifier,
					  public MPINCodeResponseHandler

	{
public:			
	static CPhysicalLink* NewLC(CPhysicalLinksManager& aConnectionMan,
								CRegistrySession& aRegSess, const TBTNamelessDevice& aDevice);
	static CPhysicalLink* NewL(CPhysicalLinksManager& aConnectionMan,
								CRegistrySession& aRegSess, const TBTNamelessDevice& aDevice);
	~CPhysicalLink();
	TInt SubscribeProxySAP(CBTProxySAP& aProxySAP);
	void UnsubscribeProxySAP(CBTProxySAP& aProxySAP);

	void SubscribeLinkObserver(MPhysicalLinkObserver& aSubscriber);
	void UnsubscribeLinkObserver(MPhysicalLinkObserver& aSubscriber);

	TInt TryToAndThenPreventHostEncryptionKeyRefresh(TAny* aOutToken);

	inline const TBTDevAddr& BDAddr() const;
	inline const TBTBasebandLinkState& LinkState() const;
	inline TBool IsConnected() const;
	inline TBool IsListening() const;
	inline TBTBasebandRole Role() const;
	void SetDeviceNamePending(TBool aBool);
	TInt GetOption(TUint aLevel,TUint aName,TDes8& aOption) const;
	void Connect(TBasebandPageTimePolicy aPolicy=EPagingNormal);
	TInt SCOConnect();
	TInt SCOConnect(const TUint16 aUserHVPacketTypes);
	TInt SynchronousConnect(TUint aTransmitBandwidth, TUint aReceiveBandwidth,
		TUint16 aMaxLatency, TUint16 aVoiceSettings,
		TUint8 aRetransmissionEffort, const TBTSyncPacketTypes aUserPacketTypes);
	TInt Arbitrate(TBool aImmediately=EFalse, TBool aLocalPriority=EFalse); 
	TInt DoArbitrate(TBool aLocalPriority);
	void SetPassKey(const TDesC8& aPassKey);
	const TBTPinCode& PassKey() const;		

	void StartArbitrationTimer() const;
	
	// from RegistryNotifier
	virtual void RegistryTaskComplete(CBTRegistryHelperBase* aHelper, TInt aResult);
	virtual void RegistryTaskComplete(CBTRegistryHelperBase* aHelper, const TBTNamelessDevice& aDevice, TInt aResult);
	virtual void RegistryTaskComplete(CBTRegistryHelperBase* aHelper, const TRegistryUpdateStatus aRegUpdateStatus, TInt aResult);

	void SetModesAllowed(TUint8 aModesAllowed, TBool aRoleSwitchAllowed);

	TBool IsModeSupportedRemotely(TBTLinkMode aMode) const;
	TBool IsEncryptionPauseResumeSupported() const;
	TBool IsRoleSwitchSupported() const;
	TBool IsEncryptionSupported() const;

	inline TBool Authenticated() const;
	inline TBool Encrypted() const;
	TInt ChangeEncryption(THCIEncryptModeFlag aEnable);
	TInt Authenticate(TBool aRequireAuthenticatedLinkKey);
	TInt ChangeLinkKey();

	TInt ChangeConnectionPacketType(TUint16 aType);

	TInt RequestHold();
	TInt RequestChangeRole(TBTBasebandRole aRole);

	void ReadNewPhysicalLinkMetricValue(TUint aIoctlName, CBTProxySAP& aSAP, TInt aCurrentValue);

	TInt OverridePark();
	TInt UndoOverridePark();
	TInt OverrideLPM();
	TInt UndoOverrideLPM();
	inline TBool IsParked() const;
	
	TBool ACLConnectPending() const;
	TBool SyncConnectPending() const;
	
	// events from MBtHostControllerEventInternalNotifier
	virtual void ConnectionComplete(THCIErrorCode aErr, const TBTConnect& aConn);
	virtual void SynchronousConnectionComplete(THCIErrorCode aErr, const TBTConnect& aConn, const TBTSyncConnectOpts& aSyncOpts);
	virtual void ConnectionRequest(const TBTConnect& aConn);
	virtual void Disconnection(THCIErrorCode aErr, THCIConnHandle aConnH, THCIErrorCode aResult);
	virtual void CompletedPackets(THCIConnHandle aConnH, TUint16 aNumPackets);
	virtual void AuthenticationComplete(THCIErrorCode aErr, THCIConnHandle aConnH);
	virtual void EncryptionChange(THCIErrorCode aErr, THCIConnHandle aConnH,TBool aEncrypted);
	virtual void ACLDataReceived(THCIConnHandle aConnH, TUint8 aFlag, const TDesC8& aData);
	virtual void SCODataReceived(THCIConnHandle aConnH, const TDesC8& aData);

	virtual void PinRequest(const TBTDevAddr& aBDAddr, MPINCodeResponseHandler& aRequester);
	virtual void NewLinkKey(const TBTDevAddr& aBDAddr, const TBTLinkKey& aLinkKey, THCILinkKeyType aLinkKeyType);
	virtual void LinkKeyRequest(const TBTDevAddr& aBDAddr, MLinkKeyResponseHandler& aRequester);
	virtual void ReadRemoteSupportedFeaturesComplete(THCIErrorCode aErr, THCIConnHandle aConnH, const TBTFeatures& aBitMask);
	virtual void ReadRemoteExtendedFeaturesComplete(THCIErrorCode aErr, THCIConnHandle aConnH, TUint64 aBitMask, TUint8 aPageNumber, TUint8 aMaximumPageNumber);
	virtual void ReadRemoteVersionInfoComplete(THCIErrorCode aErr, THCIConnHandle aConnH, const TBTDevRemoteHwVersion& aVer);
	virtual void PacketTypeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewPacket);
	virtual void LinkSupervisionTimeoutChange(THCIErrorCode aErr, THCIConnHandle aConnH, TUint16 aNewTimeout);
	virtual void MaxSlotsChange(THCIConnHandle aConnH, TUint8 aSlots);
	virtual void ModeChange(THCIErrorCode aErr, THCIConnHandle aConnH, TBTLinkMode aMode, TBasebandTime aInterval);
	virtual void WriteLinkPolicySettingsCompleteEvent(THCIErrorCode aErr, THCIConnHandle aConnH);
	virtual void RoleChange(THCIErrorCode aErr, const TBTDevAddr& aAddr, TBTBasebandRole aRole);
	virtual void ClockOffset(THCIErrorCode aErr, THCIConnHandle aConnH, TBasebandTime aClockOffset);
	virtual void RemoteName(THCIErrorCode aErr, const TBTDevAddr& aAddr, const TBTDeviceName8& aName);
	virtual void EncryptionKeyRefreshComplete(THCIErrorCode aErr, THCIConnHandle aConnH);

	void ConnectionComplete(TInt aResult, const TBTConnect& aConn);

	TBool LinkKeyRequestPending();
	void SetAuthenticationPending(TUint8 aFlag);
	virtual void AuthenticationComplete(TUint8 aFlag);

	TBool IsAuthenticationRequestPending() const;

	
	static TInt OverrideLPMTimeoutCallback(TAny* aCPhysicalLink);// async callback
	static TInt TerminateCallback(TAny* aCPhysicalLink);// async callback
	TInt Terminate(THCIErrorCode aReason);

	TInt AttachLogicalLink(TLinkType aLink, CBTBasebandSAP& aSAP);
	void DetachLogicalLink(TLinkType aLink, CBTBasebandSAP& aSAP);
	void PhysicalLinkUserIdle();

	void PinRequestSent();
	void PinRequestComplete();
	void ConfirmationComplete();
	void DeleteLinkKeyL();

	TBool IsConnectionRequestPending() const;
	void PendingConnectionRequest(TInt aError);
	inline THCIConnHandle Handle() const;
	TBool HasHandle(THCIConnHandle aConnH) const;
	inline const TBTNamelessDevice& RemoteDevice() const;

	void GetCurrentBasebandState(TBTBasebandEventNotification & aEvent);
	void TryToSend();
	
	TBool HasSyncLink() const;
	void GetDeviceFromRegistryL();
	TBTLinkMode LinkMode() const; 
	
	void AsyncDeleteRoleSwitcher();
	void AsyncDeleteKeyRefresher();
	
	inline const TLinkPolicy& LinkPolicy() const;
	inline const TBTFeatures& RemoteFeatures() const;
	inline const TBTDeviceClass& DeviceClass() const;
	TInt GetConnectionHandles(RHCIConnHandleArray& aConnectionHandles,
							  TLinkType aLinkType) const;

	TInt GetNumPendingHandles(TInt& aConnectionHandles,
							  TLinkType aLinkType) const;
	TBool IsEncryptionDisabledForRoleSwitch() const;

	void IOCapabilityAskForResponse(THCIIoCapability aIOCapability, THCIOobDataPresence aOOBDataPresence, THCIAuthenticationRequirement aAuthenticationRequirement);

	void SetLocalMITM(TBool aLocalMITM);
	TBool AuthWithMITM() const;
	TUint LSTO() const { return iLSTO; }
	
	TPhysicalLinkSimplePairingMode SimplePairingMode() const;

	TBool HasRemoteOobData() const;
	
	THCIAuthenticationRequirement AuthenticationRequirement() const;

	void NewNumericComparatorL(const TBTDevAddr aAddr,CBTSecMan& aSecMan,TUint32 aNumericValue, TBool aInternallyInitiated);

	CBTNumericComparator* InstanceNumericComparator() const;
	TBool IsNumericComparatorActive()const;
	void DeleteNumericComparator();
	void CancelNumericComparator();


	void NewPasskeyEntryL(const TBTDevAddr aAddr,CBTSecMan& aSecMan,TUint32 aNumericValue, TBool aInternallyInitiated);

	CBTPasskeyEntry* InstancePasskeyEntry() const;
	TBool IsPasskeyEntryActive()const;
	void DeletePasskeyEntry();
	void CancelPasskeyEntry();
	void PasskeyEntryKeyPressed(THCIPasskeyEntryNotificationType aKey);

	void NewUserConfirmerL(const TBTDevAddr aAddr,CBTSecMan& aSecMan,TBool aInternallyInitiated);

	CBTUserConfirmer* InstanceUserConfirmer() const;
	TBool IsUserConfirmerActive()const;
	void DeleteUserConfirmer();
	void CancelUserConfirmer();

	
	TBasebandTime GetSniffInterval() const;
	
	TBool IsPairable() const;

	TBool IsPairingExpected() const;
	
private:
	CPhysicalLink(CPhysicalLinksManager& aParent, CRegistrySession& aRegSess, const TBTNamelessDevice& aDevice);
	void ConstructL();
	void GetRemoteDetailsL(const TBTConnect& aConn);
	void DoUpdateNameL(const TBTDeviceName8& aName);

	void NotifyLogicalLinkUp(const TBTConnect& aConnect);
	void NotifyLogicalSyncLinkUp(const TBTConnect& aConnect, const TBTSyncConnectOpts& aSyncOpts);
	void NotifyLogicalLinkDown(TPhysicalLinkPort aPort);
	void NotifyLogicalLinkError(TPhysicalLinkPort aPort, TInt aError);
	void RejectConnection(const TBTConnect& aConn);

	void DoPinRequestL(const TBTDevAddr& aAddr, MPINCodeResponseHandler& aRequester);
	void UpdateFromInquiryCache();
	void StoreDeviceL(TBool aPreventDeviceAddition);
	void SetLinkKey(const TBTLinkKey& aLinkKey, TBTLinkKeyType aLinkKeyType);

	void QueueIdleTimer(TInt aTime);
	void RemoveIdleTimer();

	void NotifyStateChange(TBTBasebandEventNotification & aEvent);

	TBool IsPhysicalLinkIdle() const;
	TBasebandTime CalculatePageTimeout(TBasebandPageTimePolicy aPolicy, TUint8 aRepMode, TBool aValidClockOffset);
	TBool IsPasskeyMinLengthOK();
	TBool PeerSupportsLinkKeyRegeneration() const;

	void DeleteRoleSwitcher();
	static TInt RoleSwitchCompleteCallBack(TAny* CPhysicalLink);
	TInt ManageEncryptionEnforcement(THCIEncryptModeFlag aEnable);
	
	void DeleteKeyRefresher();
	static TInt KeyRefreshCompleteCallBack(TAny* CPhysicalLink);

	void HandlePrefetch();
	void PINCodeRequestReply(const TBTDevAddr& aDevAddr,const TDesC8& aPin);
	void PINCodeRequestNegativeReply(const TBTDevAddr& aDevAddr);
	
	inline TBool IsAuthenticationPending() const;
	
	void LinkKeyRequestResponseAttempt(TBool aForceResponse = EFalse);
	void DoLinkKeyResponse(TBool aPositive);

	void RemoteSimplePairingModeDetermined(TPhysicalLinkSimplePairingMode aSimplePairingMode);
	void SetPeerInSecurityMode3();

private: // from MPINCodeResponseHandler
	TInt PINCodeRequestReply(const TBTDevAddr& aDevAddr,const TDesC8& aPin) const;
	TInt PINCodeRequestNegativeReply(const TBTDevAddr& aDevAddr) const;

private:
	CPhysicalLinksManager&					iLinksMan; // The manager of remote physical link representations
	CRegistrySession&						iRegSess; // The RegistryServer session

	// the PHY's properties
	TBTNamelessDevice						iDevice; // names not stored here by in InqMgr
	TBTNamelessDevice						iRegistryDevice; // the view from the registry.
	TBTBasebandLinkState					iLinkState;
	THCIConnHandle							iHandle;
	TBTFeatures								iRemoteFeatures;
	TBTDevRemoteHwVersion					iRemoteVersion;

	TInt									iDeviceResult; // Whether we got device from registry OK
	// the PHY's subscribers
	TDblQue<TPhysicalLinkObserverQLink>		iBasebandSubscribers;
	// the PHY's outstanding registry helpers
	TSglQue<CBTRegistryHelperBase>			iRegistryHelpers;
	
	// the PHY's UI handlers
	CBTPinRequester*						iPinRequester;	// looks after PIN entry UI/state
	CBTNumericComparator*					iNumericComparator; // looks after the numeric comparison UI/state
	CBTPasskeyEntry*						iPasskeyEntry; // looks after the passkey entry UI/state
	CBTUserConfirmer*					iUserConfirmer; // looks after the user confirmation UI/state
	
	CEncryptionEnforcer*					iEncryptionEnforcer;
	
	// the PHY's supported logical links...
	RPointerArray<CACLLink>					iACLLogicalLinks;
	CBTSynchronousLink*						iSyncLogicalLink; // stack only supports a signal one per PHY

	TBool						iLinkKeyRequestOutstanding;	// for if we have to wait for Registry or SSP support status

	MPINCodeResponseHandler*	iPinHandler;		// for forwarding responses to

	TBTConnect 					iLastPendingConnection;		// for if we have to wait for Registry to decide whether to rject or accept a connection
	TBool						iPendingConnection;  // is a connection request waiting for a reply
	TSglQue<CBTProxySAP>		iProxySAPs;			// the proxies bound to us
	TDeltaTimerEntry			iIdleTimerEntry;
	TBool						iIdleTimerQueued;

	TLinkPolicy					iLinkPolicy;
	TUint16						iPreviousRequestedModeMask;
	TBool						iOverrideParkRequests; //for maybe temporary unpark
	TBool						iOverrideLPMRequests; //for maybe temporary force active
	TBool						iConnectionPacketTypeChanged; //for triggering h/w 
	
	TBool						iPreventLinkKeyUpdateReg; //for case e.g. user has called unpair when there is a paired logical channel
	TUint8						iAuthStateMask; // Authentication state mask to keep track of the authentication process
	TBool						iLinkKeyPending; //PIN has been sent, awaiting a link key.
	
	TBool						iPeerInSecurityMode3;
	TBasebandTime				iSniffInterval;
	CArbitrationDelayTimer*		iArbitrationDelay; //for lower power modes
	CRoleSwitcher*				iRoleSwitcher; //class handles role switch, prevents LPM, removes encryption		
	CAsyncCallBack*				iRoleSwitchCompleteCallBack;// Async Callback to delete role swticher class.
	CEncryptionKeyRefresher*	iKeyRefresher; //class handles key refresh 
	CAsyncCallBack*				iKeyRefreshCompleteCallBack;// Async Callback to delete key refresher class.

	TLowPowModeCmdController	iLowPowModeCtrl;
	CPhysicalLinkMetrics*		iPhysicalLinkMetrics;
	TDisconnectCmdController	iDisconnectCtrl;
	TAuthenticationCmdController iAuthenticationCtrl;
	
	TBool						iIOCapsReceived;
	THCIIoCapability				iIOCapability;
	THCIOobDataPresence			iOOBDataPresence;
	THCIAuthenticationRequirement	iAuthenticationRequirement;
	
	TBool						iRequireAuthenticatedLinkKey;
	TBool						iLocalMITM; // Whether MITM protection was requested
	TUint						iLSTO;
	
	
	TPhysicalLinkSimplePairingMode		iSimplePairingMode;
	
	TDblQue<XAutoKeyRefreshToken>		iAutoKeyRefreshQue;
	TRegistryDeviceBeingModified		iRegistryDevBeingMod;	// avoid duplicate equal registry device store req
	
	TInt						iIdleTimeout;

	TBool						iNewPinCodeValid;
	TBTPinCode					iNewPinCode;

	TBool						iLinkKeyReturnedInThisAuthentication;
	TBool						iLinkKeyObtainedThroughDedicatedBonding;
private:
	/**
	Enumeration to represent the current state of the physical links storage in the registry,
	this is to cover the asynchronous properties of actions to the registry.
	*/
	enum TRemoteDeviceRecordedByStack
		{
		ENull,
		EPending,
		EComplete
		};
	TRemoteDeviceRecordedByStack	iRemoteDeviceRecordedByStack;
	TBool						iDisconnectRequested; //
	};

// Removed - LogicalLinkTrackers - if needed to prevent Park mode for legacy apps see in Perforce


#include "physicallinks.inl"
#endif //_PHYSICALLINKS_H
