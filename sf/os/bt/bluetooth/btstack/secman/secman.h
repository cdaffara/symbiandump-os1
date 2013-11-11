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

/**
 @file
 @internalComponent
*/

#ifndef SECMAN_H
#define SECMAN_H

#include <es_prot.h>
#include <bt_sock.h>
#include <btmanclient.h>
#include <btextnotifiers.h>
#include <bluetooth/hcicommandqueueclient.h>

#include "notification.h"
#include "SecManNotifiers.h"

#include "btaccessrequesterstatemachine.h"
#include <btextnotifierspartner.h>

//forward decs
class CBTSecMan;
class CPairingServer;
class CBTAccessRequester;
class CBTAuthorisor;
class CPhysicalLink;
class CPhysicalLinksManager;
class CLinkMgrProtocol;
class CBTInquiryMgr;
class CBTHostResolver;
class CBTAccessRequesterStateFactory;
class MHCICommandQueue;
class THCICommandCompleteEvent;

class COobDataManager;
class CSimplePairingResultList;
class CAuthenticationResultList;

// used in physicallinks.cpp
const TInt KDeviceNotObtained = KMaxTInt;

/**
Interface for a class that is making an access request - typically a SAP
**/
enum TBTSecurityClearance
	{
	EBTSecManAccessGranted,
	EBTSecManAccessDenied,
	EBTSecManAccessDeferred,
	};

/** 
Security Notifier Update Class
Handles the updating of the Bluetooth Notifiers
**/
template <class T>
NONSHARABLE_CLASS(CSecNotifierUpdateAO) : public CActive
	{
public:
	static CSecNotifierUpdateAO<T>* NewL(RNotifier& aNotifier, TUid aNotifierUid);
	static CSecNotifierUpdateAO<T>* NewLC(RNotifier& aNotifier, TUid aNotifierUid);
	~CSecNotifierUpdateAO();
	
	void DoUpdate(const T& aPckg);
	void DoUpdateSynchronous(const T& aPckg);
	
private: // from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	CSecNotifierUpdateAO();
	void ConstructL(RNotifier& aNotifier, TUid aNotifierUid);
	
private:
	RNotifier	iNotifier;
	TUid		iNotifierUid;
	TBuf8<1>	iAnswer;
	T			iPckg;
	RArray<T>	iPckgQueue;
	};


/**
Base class that makes requests using RNotifier.
This is used by the security manager to queue the notifier requests.
**/
NONSHARABLE_CLASS(CSecNotifierRequester) : public CActive, public MResolverNotify
	{
public:
	virtual void DoRequest() = 0;
	virtual void DoUpdateNotifier() = 0;
	virtual void HandleTimeout();

	~CSecNotifierRequester();
protected:
	CSecNotifierRequester(CBTSecMan& aSecMgr);
	void ConstructL(const TBTDevAddr& aAddr);
	void QueryComplete(TInt aResult);
	void RemoveMyselfFromQue();
protected:
	const TDesC8*			iDeviceName;	//non-owned; points to InqMgr result
	TBTDevAddr				iDevAddr;
	TBool					iIsAddedToNotifierQue;
	RNotifier				iNotifier;
	CBTHostResolver*		iHR;
	TNameRecord*			iHRNameRecord;
	CBTInquiryMgr&			iInquiryMgr;
	CBTSecMan&				iSecMgr;
	};

NONSHARABLE_CLASS(CSecNotifierRequesterTimer)
	: public CTimer
	{
public:
	static CSecNotifierRequesterTimer* NewL(CSecNotifierRequester& aObserver);
	~CSecNotifierRequesterTimer();
	void Start(TTimeIntervalMicroSeconds32 aTimeout);

private:
	CSecNotifierRequesterTimer(CSecNotifierRequester& aObserver);
	void ConstructL();
	void RunL();

private:
	CSecNotifierRequester&	iObserver;
	};


/**
	This class handles CBTSecMan commands to and from the HCI
	It is owned, constructed and destructed by CBTSecMan
**/
NONSHARABLE_CLASS(CSecManCommandController) 
	: public CBase
	, public MHCICommandQueueClient
	{
public:
	~CSecManCommandController();
	static CSecManCommandController* NewL(CBTSecMan& aSecMan);
	static CSecManCommandController* NewLC(CBTSecMan& aSecMan);
	
	void SetHCICommandQueue(MHCICommandQueue& aCommandQueue);
	void ClearHCICommandQueue();
	
public: // command functions
	void WriteSimplePairingDebugModeL(TUint8 aSimplePairingDebugMode);
	void IOCapabilityRequestReplyL(const TBTDevAddr& aBDADDR, THCIIoCapability aIOCapability, THCIOobDataPresence aOOBDataPresent, THCIAuthenticationRequirement aAuthenticationRequirements);
	void IOCapabilityRequestNegativeReplyL(const TBTDevAddr& aBDADDR, TUint8 aReason);
	void UserConfirmationRequestReplyL(const TBTDevAddr& aBDADDR);
	void UserConfirmationRequestNegativeReplyL(const TBTDevAddr& aBDADDR);
	void RemoteOOBDataRequestReplyL(const TBTDevAddr& aBDADDR, const TBluetoothSimplePairingHash& aC, const TBluetoothSimplePairingRandomizer& aR);
	void RemoteOOBDataRequestNegativeReplyL(const TBTDevAddr& aBDADDR);
	
private: // events from MHCICommandQueueClient
	void MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* aRelatedCommand);
	void MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* aCommand);
	
private:
	CSecManCommandController(CBTSecMan& aSecMan);
	void ConstructL();
	
	MHCICommandQueue& CommandQueue() const;
	
	void CommandCompleteEvent(const THCIEventBase& aEvent);
	
	void CommandStatusEvent(const THCIEventBase& aEvent);
	void IOCapabilityRequestEvent(const THCIEventBase& aEvent);
	void IOCapabilityResponseEvent(const THCIEventBase& aEvent);
	void UserConfirmationRequestEvent(const THCIEventBase& aEvent);
	void UserPasskeyNotificationEvent(const THCIEventBase& aEvent);
	void RemoteOOBDataRequestEvent(const THCIEventBase& aEvent);
	void SimplePairingCompleteEvent(const THCIEventBase& aEvent);
	void KeypressNotificationEvent(const THCIEventBase& aEvent);

	void RemoteOOBDataRequestReplyOpcode(const THCICommandCompleteEvent& aCompleteEvent);
	void RemoteOOBDataRequestNegativeReplyOpcode(const THCICommandCompleteEvent& aCompleteEvent);
	void WriteSimplePairingDebugModeOpcode(const THCICommandCompleteEvent& aCompleteEvent);

private: // checking cast functions.
	static THCIIoCapability CastToIoCapability(TUint8 aIOCapability);
	static THCIOobDataPresence CastToOobDataPresence(TUint8 aOOBDataPresent);
	static THCIAuthenticationRequirement CastToAuthenticationRequirements(TUint8 aAuthenticationRequirements);

private:
	CBTSecMan&			iSecMan;
	MHCICommandQueue*	iCommandQueue;
	};

enum TAccessType
	{
	EGeneralBonding,
	EGeneralBondingSecurityMode4Outgoing,
	EDedicatedBonding,
	};

/**
BT Security Manager.
Provides access to all functionality of the security manager.
**/
NONSHARABLE_CLASS(CBTSecMan) : public CBase
	{
public:
	static CBTSecMan* NewL();
	static CBTSecMan* NewLC();
	~CBTSecMan();

	void SetLocalSimplePairingMode(TBool aEnabled);
	TBool LocalSimplePairingMode() const;
	
	void SimplePairingSupportDetermined(const TBTDevAddr& aBDAddr);

	void SetPhysicalLinksMgr(const CPhysicalLinksManager& aConnectionsMgr);
	void ClearPhysicalLinksMgr();
	void SetLinksMgrProtocol(CLinkMgrProtocol& aLinkMgrProtocol);
	void ClearLinksMgrProtocol();
	void SetHCICommandQueue(MHCICommandQueue& aCommandQueue);
	void ClearHCICommandQueue();
	CPhysicalLinksManager& ConnectionsManager() const;
	COobDataManager& OobDataManager() const;
	
	TBool DebugMode() const;
	void SetDebugMode(TBool aOn);
	void ClearDebugMode();
	void DebugModeChanged(TBool aOn);

	void AccessRequestL(const TBTServiceSecurity& aSecurity,
						const TBTServiceSecurityPerDevice* const aOverride,
						const TBTDevAddr& aBDAddr,
						TAccessType aAccessType,
						TBool aNoSecurityRequired,
						MAccessRequestResponseHandler& aRequester);

	void AccessRequestComplete(CBTAccessRequester* aRequester, TInt aResult);

	void AddNotifierRequestToQueL(CSecNotifierRequester& aRequest);
	void RemoveNotifierRequestFromQue(CSecNotifierRequester& aRequest);

	void CancelRequest(MAccessRequestResponseHandler& aRequester);
	void DeleteLinkKeyL(const TBTDevAddr& aAddr);

	void GetPassKeyLengthAndOriginator(const TBTDevAddr& aAddr, TUint& aPasskeyMinLength,
									   TBool& aLocallyInitiatedAuthentication,
									   TBool& aStrongKeyRequired);
									   
	void AuthenticationInProgress();
	void IOCapabilityRequestFromRemote(const TBTDevAddr& aAddr);
	void IOCapabilityAskForResponse(const TBTDevAddr& aAddr, THCIIoCapability aIOCapability, THCIOobDataPresence aOOBDataPresent, THCIAuthenticationRequirement aAuthentication_Requirements);
	void RemoteOOBDataRequest(const TBTDevAddr& aAddr);
	void RemoteOOBDataRequestComplete(const TBTDevAddr& aAddr);
	void UserConfirmationRequest(const TBTDevAddr& aAddr, TUint32 aNumericValue);
	void NumericComparisonComplete(const TBTDevAddr& aAddr, TBool aResult, TInt aError);
	void UserConfirmationComplete(const TBTDevAddr& aAddr, TBool aResult, TInt aError);
	void PasskeyNotification(const TBTDevAddr& aAddr, TUint32 aPasskey);
	void PasskeyNotificationComplete(const TBTDevAddr& aAddr, TInt aError);
	void KeypressNotification(const TBTDevAddr& aAddr, TUint8 aNotificationType);
	void SimplePairingComplete(const TBTDevAddr& aAddr, THCIErrorCode aError);
	void AuthenticationComplete(const TBTDevAddr& aAddr, THCIErrorCode aError);

	TBool IsDedicatedBondingAttempted(const TBTDevAddr& aAddr);
	TBool IsOutboundAccessRequest(const TBTDevAddr& aAddr);

	CBTAccessRequesterStateFactory* StateMachine();
	
	inline MHCICommandQueueClient& HCIEventHandler() const { return *iCommandController; }
	inline CSecManCommandController& CommandController() const { return *iCommandController; }
	CBTAccessRequester* FindActiveAccessRequester(const TBTDevAddr& aAddr) const;
	
private:
	MAccessRequestResponseHandler& FinishAccessRequest(CBTAccessRequester* aAccessRequester);

private:
	static TBool CompareAccessRequesterByBDAddr(const TBTDevAddr* aKey, const CBTAccessRequester& aAccessRequester);
	static TBool CompareAccessRequesterByRequester(const MAccessRequestResponseHandler* aKey, const CBTAccessRequester& aAccessRequester);
			
private:
	CSecManCommandController* 				iCommandController;

private:
	CPairingServer*							iPairingServer;
	COobDataManager*						iOobDataManager;
	CSimplePairingResultList*				iSimplePairingResultList;
	CAuthenticationResultList*				iAuthenticationResultList;

private:
	CBTSecMan();
	void ConstructL();
private:
	CPhysicalLinksManager*					iPhysicalLinksManager;
	RPointerArray<CBTAccessRequester>		iAccessRequesters;
	RPointerArray<CBTAccessRequester>		iPendingAccessRequesters;
	RPointerArray<CSecNotifierRequester>	iNotifierRequesters;
	CSecNotifierRequester*					iActiveNotifierRequester;
	TBool									iLocalSimplePairingMode;
	TBool									iDebugMode;
	CBTAccessRequesterStateFactory*			iStateMachine;
	};


/**
Enforces security policy and answers access requests.
An object of this class is created when an access request is first made from a particular device to a
particular service.  It is destroyed when the request may be answered with "accepted" or "denied".
**/
NONSHARABLE_CLASS(CBTAccessRequester)
	: public CBase
	, public MPhysicalLinkObserver
	, public MBluetoothPrefetchNotifier
	{
public:
	static CBTAccessRequester* NewLC(CPhysicalLink& aConnection,
									 const TBTServiceSecurity& aSecurity,
									 const TBTServiceSecurityPerDevice* const aOverride,
									 MAccessRequestResponseHandler& aRequester,
									 TAccessType aAccessType,
									 TBool aNoSecurityRequired,
									 CBTSecMan& aParent);
	~CBTAccessRequester();

	void Start();
	TBool SendEvent( TBTSecEvent& aEvent );

public: // State machine actions
	void Ready( TBTSecEvent& aEvent );
	void AuthenticationRequested( TBTSecEvent& aEvent );
	void RequestAuthentication( TBTSecEvent& aEvent );
	void AuthenticationComplete( TBTSecEvent& aEvent );
	void EncryptionChangePending( TBTSecEvent& aEvent );
	void EncryptionChangeComplete( TBTSecEvent& aEvent );
	void AuthorisationRequested( TBTSecEvent& aEvent );
	void RequestAuthorisation( TBTSecEvent& aEvent );
	void AuthorisationComplete( TBTSecEvent& aEvent );

	void IOCapsResponse( TBTSecEvent& aEvent );
	void IOCapsRequested( TBTSecEvent& aEvent );
	void UserConfirmation( TBTSecEvent& aEvent );
	void PasskeyEntry( TBTSecEvent& aEvent );
	void RemoteOOBDataRequest( TBTSecEvent& aEvent );
	void SimplePairingPending( TBTSecEvent& aEvent );

	const MAccessRequestResponseHandler& ServiceRequester() const;
	const TBTDevAddr& DeviceAddress() const;

	TBool IsAuthenticationReqPending(const TBTDevAddr& aAddr, TUint& aPasskeyMinLength, TBluetoothMitmProtection& aMitmLevel);
	TBool AuthenticationRequired() const;
	TBool EncryptionRequired() const;
	TBool AuthenticationInProgress() const;
	TBool RemoteIndicatedNoBondingToDedicatedBonding() const;
	TBool BasebandConnected() const;
	
	void CompleteRequest(TInt aReason);
	void SetCurrentState(TBTAccessRequesterState aState);
	TBool RequirementsDenied();
	TAccessType AccessType() const;
	
private:
	NONSHARABLE_CLASS(CAuthenticationTimer)
		: public CTimer
		{
	private:
		static const TInt KAuthenticationRequestTimeout = 60000000; // 60 seconds in microseconds
		
	public:
		static CAuthenticationTimer* NewL(CBTAccessRequester& aObserver);
		~CAuthenticationTimer();
		void Start();
	
	private:
		CAuthenticationTimer(CBTAccessRequester& aObserver);
		void ConstructL();
		void RunL();
		
	private:
		CBTAccessRequester&	iObserver;
		};
		
private:
	CBTAccessRequester(CPhysicalLink& aConnection,
					   const TBTServiceSecurity& aServiceSecurity,
					   const TBTServiceSecurityPerDevice* const aOverride,
					   MAccessRequestResponseHandler& aRequester,
					   TAccessType aDedicatedBonding,
					   TBool aNoSecurityRequired,
					   CBTSecMan& aParent);
	void ConstructL();
	void SubscribeToLinkObserver();

	TBTAccessRequirements OverallRequirements(const TBTServiceSecurity& aServiceSecurity,
											  const TBTNamelessDevice& aDevice);

	void PhysicalLinkChange(const TBTBasebandEventNotification & aEvent);
	void QueryComplete(TInt aErr);
	void SetDeviceName();
	
	TBool LinkKeyGoodEnough() const;
	TBool AuthenticationRecommended() const;
	
	void SetAuthenticationInProgress();
	
	void AuthenticationTimerElapsed();
	void LinkError(TInt aError);
	
private: //from MPhysicalLinkObserver
	void PhysicalLinkChange(const TBTBasebandEventNotification & aEvent, CPhysicalLink& aPhysicalLink);
	TPhysicalLinkObserverQLink& ObserverQLink();

private: // from MBluetoothPrefetchNotifier
	TPrefetchNotifierQLink& MbpnQueLink();
	void MbpnPrefetchComplete(TInt aError);

private:
	MAccessRequestResponseHandler&	iRequester;                     //< the SAP who instructed us to check sec
	CBTSecMan&						iSecMan;                        //< our parent
	CPhysicalLink*					iBaseband;                      //< the baseband we're operating on
	TBTDevAddr						iDevAddr;                       //< Bluetooth device address we are representing.

	CBTAuthorisor*					iAuthorisor;                    //< looks after authorisation UI/state

	
	const TBTServiceSecurity		iServiceRequirements;           //< the security required by our service
	TBTAccessRequirements			iRequirements;                  //< the requirements of device and service and its UID

	const TBTServiceSecurityPerDevice* const	iOverride;          //< any overrides the service has
	TBool							iIsSubscribedToConnection;
	TBool							iDeviceRetrievedFromRegistry;
	
	const TDesC8*					iDeviceName;                    //< non-owned; points to InqMgr result
	TPhysicalLinkObserverQLink		iQueLink;
	TPrefetchNotifierQLink			iPrefetchQueueLink;
	TBool 							iAuthenticationInProgress;      //< If authentication is in progress
	
	TAccessType						iAccessType;
	
	TBool							iRemoteIndicatedNoBonding;  //< Remote IOCapabilitiesResponse says no dedicated bonding
	THCIIoCapability				iRemoteIOCapability;
	THCIOobDataPresence				iRemoteOOBDataPresence;
	
	CAuthenticationTimer*			iTimer;
	
	TBTAccessRequesterState 		iCurrentState;
	
	TBool							iNoSecurityRequired; //< Security mode 4 "no security required" - do not do any authentiction, not even just works
	};


/**
Prompts the user for permission for a particular connection to proceed.
Uses the RNotifier framework to produce a dialog containing information
about the remote device and the local service in question.
**/
NONSHARABLE_CLASS(CBTAuthorisor) : public CSecNotifierRequester
	{
public:
	static CBTAuthorisor* NewL(CBTAccessRequester& aAccessRequester, CBTSecMan& aSecMan, TUid aServiceUID);
	static CBTAuthorisor* NewLC(CBTAccessRequester& aAccessRequester, CBTSecMan& aSecMan, TUid aServiceUID);
	~CBTAuthorisor();

private: // from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private: // from CSecNotifierRequester
	void DoRequest();
	void DoUpdateNotifier();

private:
	CBTAuthorisor(CBTAccessRequester& aAccessRequester, CBTSecMan& aSecMan, TUid aServiceUID);
	
private:
	CSecNotifierUpdateAO<TBTNotifierUpdateParamsPckg>*	iUpdateNotifierAO;
	CBTAccessRequester&			iAccessRequester;
	TBTAuthorisationParamsPckg	iAuthorisationParamsPckg;
	TPckgBuf<TBool>				iResultPckg;
	};

/**
RNotifier Pin Provider.
Uses an RNotifier plugin to get a pin from the user.
Generally owned by physical link to cope with passive authentication
**/
NONSHARABLE_CLASS(CBTPinRequester)
	: public CSecNotifierRequester
	{
public:
	static CBTPinRequester* NewL(const TBTDevAddr& aAddr,
								 MPINCodeResponseHandler& aHandler,
								 CBTSecMan& aSecMan,
								 TUint aPasskeyMinLength,
								 TBool aInternallyInitiated,
								 TBool aStrongKeyRequired,
								 TUint aRecommendedPasskeyMinLength);
								 
	static CBTPinRequester* NewLC(const TBTDevAddr& aAddr,
								  MPINCodeResponseHandler& aHandler,
								  CBTSecMan& aSecMan,
								  TUint aPasskeyMinLength,
								  TBool aInternallyInitiated,
								  TBool aStrongKeyRequired,
								  TUint aRecommendedPasskeyMinLength);

	~CBTPinRequester();

	void UpdateHandler(MPINCodeResponseHandler& aNewHandler);

	void RequestPinL(const TBTDevAddr& aDevAddr, const TDesC8* aDeviceName);
	virtual void FriendlyNameRetrieved(const TDesC& aName, TInt aResult);

private: // from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private: //from CSecNotifierRequester
	virtual void DoRequest();
	virtual void DoUpdateNotifier();
	virtual void HandleTimeout();

private:
	static const TInt KPinRequesterTimeout = 60000000; // 60 seconds in microseconds
private:
	CBTPinRequester(MPINCodeResponseHandler& aRequester,
					CBTSecMan& aSecMan,
					TUint aPasskeyMinLength,
					TBool aInternallyInitiated,
					TBool aStrongKeyRequired,
					TUint aRecommendedPasskeyMinLength);
	void ConstructL(const TBTDevAddr& aAddr);

	void HandleSspAwareNotifierL();
	void HandleLegacyNotifierL();
	
	void UpdateSspAwareNotifier();
	void UpdateLegacyNotifier();
	
private:
	enum TNotifierTypeInUse
		{
		EInvalid,
		ESspAwareNotifier,
		ELegacyNotifier,
		};
	
private:
	MPINCodeResponseHandler*			iHandler;
	CBTSecMan&							iSecMan;
	
	TNotifierTypeInUse					iNotifierType;
	
	TBTPinCodeEntryNotifierParamsPckg	iPinCodeEntryParamsPckg;
	TBTPasskeyNotifierParamsPckg		iLegacyPinCodeParamsPckg;
	
	union
		{
		CSecNotifierUpdateAO<TBTNotifierUpdateParamsPckg>*		iLegacyUpdater;
		CSecNotifierUpdateAO<TBTDeviceNameUpdateParamsPckg>*	iNameUpdater;
		} iUpdateNotifier;

	CSecNotifierRequesterTimer*			iTimer;

	TUint 								iPasskeyMinLength;
	TBool 								iInternallyInitiated;
	TBool								iStrongKeyRequired;
	TUint								iRecommendedPasskeyMinLength;
	
	TBTPinCode							iPassKey;
	};

/**
Prompts the user to confirm whether or not the supplied numeric value is the same 
as that of the remote device. 
Uses the RNotifier framework to produce a dialog containing information
about the remote device and the numeric value to be compared.
**/
NONSHARABLE_CLASS(CBTNumericComparator) 
	: public CSecNotifierRequester
	{
public:
	static CBTNumericComparator* NewL(const TBTDevAddr aAddr,
									  CBTSecMan& aSecMan, 
									  TUint32 aNumericValue, 
									  TBTNumericComparisonParams::TComparisonScenario aComparisonScenario, 
									  TBool aInternallyInitiated);
	static CBTNumericComparator* NewLC(const TBTDevAddr aAddr,
									   CBTSecMan& aSecMan, 
			  						   TUint32 aNumericValue, 
									   TBTNumericComparisonParams::TComparisonScenario aComparisonScenario, 
									   TBool aInternallyInitiated);
	~CBTNumericComparator();

private: // from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private: //from CSecNotifierRequester
	virtual void DoRequest();
	virtual void DoUpdateNotifier();

private:
	CBTNumericComparator(CBTSecMan& aSecMan, 
			  			 TUint32 aNumericValue, 
						 TBTNumericComparisonParams::TComparisonScenario aComparisonScenario, 
						 TBool aInternallyInitiated);
	
private:
	CSecNotifierUpdateAO<TBTDeviceNameUpdateParamsPckg>*	iNameUpdater;
	CBTSecMan&												iSecMan;
	TUint32 												iNumericValue;
	TBTNumericComparisonParams::TComparisonScenario 		iComparisonScenario;
	TBool 													iInternallyInitiated;
	TBTNumericComparisonParamsPckg							iNumericComparisonParamsPckg;
	TPckgBuf<TBool>											iResultPckg;
	};

/**
Prompts the user to enter the supplied numeric value on the remote device 
Uses the RNotifier framework to produce a dialog containing information
about the remote device and the numeric value to be compared.
**/
NONSHARABLE_CLASS(CBTPasskeyEntry) : public CSecNotifierRequester
	{
public:
	static CBTPasskeyEntry* NewL(const TBTDevAddr aAddr, 
									  CBTSecMan& aSecMan,
									  TUint32 aNumericValue,
									  TBool aInternallyInitiated);
	static CBTPasskeyEntry* NewLC(const TBTDevAddr aAddr, 
									   CBTSecMan& aSecMan,
									   TUint32 aNumericValue,
									   TBool aInternallyInitiated);
	~CBTPasskeyEntry();

	void KeyPressed(THCIPasskeyEntryNotificationType aKey);
	
private: // from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private: // from CSecNotifierRequester
	virtual void DoRequest();
	virtual void DoUpdateNotifier();

private:
	CBTPasskeyEntry(CBTSecMan& aSecMan, TUint32 aNumericValue, TBool aInternallyInitiated);
private:
	CSecNotifierUpdateAO<TBTPasskeyDisplayUpdateParamsPckg>*	iKeypressUpdater;		
	CSecNotifierUpdateAO<TBTDeviceNameUpdateParamsPckg>*		iNameUpdater;	
	
	CBTSecMan&							iSecMan;
	TUint32								iNumericValue;
	TBool 								iInternallyInitiated;
	TBTPasskeyDisplayParamsPckg			iPasskeyDisplayParamsPckg;
	TBTDeviceNameUpdateParamsPckg		iDeviceNameUpdateParamsPckg;
	TBuf8<1>							iResultPckg;
	};

/**
Prompts the user to confirm whether or not they want to pair
Uses the RNotifier framework to produce a dialog containing information
about the remote device
**/
NONSHARABLE_CLASS(CBTUserConfirmer) 
	: public CSecNotifierRequester
	{
public:
	static CBTUserConfirmer* NewL(const TBTDevAddr aAddr,
									  CBTSecMan& aSecMan, 
									  TBool aInternallyInitiated);
	static CBTUserConfirmer* NewLC(const TBTDevAddr aAddr,
									   CBTSecMan& aSecMan, 
									   TBool aInternallyInitiated);
	~CBTUserConfirmer();

private: // from CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
private: //from CSecNotifierRequester
	virtual void DoRequest();
	virtual void DoUpdateNotifier();

private:
	CBTUserConfirmer(CBTSecMan& aSecMan, 
						 TBool aInternallyInitiated);
	
private:
	CSecNotifierUpdateAO<TBTDeviceNameUpdateParamsPckg>*	iNameUpdater;
	CBTSecMan&												iSecMan;
	TBool 													iInternallyInitiated;
	TBTUserConfirmationParamsPckg							iUserConfirmationParamsPckg;
	TPckgBuf<TBool>											iResultPckg;
	};


_LIT(KBTSecPanic, "BT Security");
enum TBTSecPanic
	{
	EBTSecBadAuthorisationRequest,
	EBTSecBadNotifierArray,
	EBTSecPinRequesterAlreadyExists,
	EBTSecPINRequestedTwice,
	EBTSecAuthorisationRequestAlreadyExists,
	EBTSecBadNotifierUpdate,
	EBTSecBadDeviceAddress,
	EBTSecUnexpectedSecurityResponse,
	EBTSecUnexpectedStateMachineEventId,
	EBTSecUnexpectedStateMachineError,
	EBTSecBadStateMachineEvent,
	EBTSecConnectionNumericComparisonTwice,
	EBTSecConnectionPasskeyNotificationTwice,
	EBTSecNullPhysicalLinksManager,
	EBTSecPhysicalLinkMissing,
	EBTSecBadUserPasskeyRequest,
	EBTSecNotImplemented,
	EBTSecEncryptionRequestForDedicatedBond,
	EBTSecUnexpectedIoCapability,
	EBTSecUnexpectedOobDataPresence,
	EBTSecUnexpectedAuthenticationRequirements,
	EBTSecCommandQueueAlreadyProvided,
	EBTSecCommandQueueNotAvailable,
	EBTSecDebugModeTurnedOff,
	EBTSecNoCommandAssociatedWithErrorEvent,
	EBTSecUnknownHCIEvent,
	EBTSecUnknownHCICommandCompleteOpcode,
	EBTSecMultipleActiveAccessRequestersForService,
	EBTSecMultiplePendingAccessRequestersForService,
	EBTSecServiceTryingToQueueMultipleAccessRequesters,
	EBTSecAccessRequesterCompletedWhenNotActive,
	EBTSecEncryptionRequiredOnUnauthenticatedLink,
	EBTSecCouldNotFindStateTransition,
	EBTSecAccessRequesterShouldHaveNotBeenFound,
	EBTSecAccessRequesterShouldHaveBeenFound,
	EBTSecNotifierRequesterUsingTimerWithoutHandling,
	EBTSecConnectionUserConfirmationTwice,
	};


#endif // SECMAN_H

