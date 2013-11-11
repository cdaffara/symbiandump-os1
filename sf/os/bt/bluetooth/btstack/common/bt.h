// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Define the interface to the Bluetooth protocol module
// 
//

#ifndef BT_H
#define BT_H

#include <es_prot.h>
#include "secman.h"
#include "debug.h"
#include "blogger.h"
#include "btsockettimer.h"
#include "codman.h"

// This is the factory that ESOCK uses
class CBluetoothProtocolBase;
class CL2CAPProtocol;
class CRfcommProtocol;
class CBlogger;
class CIncomingConnectionListener;

typedef TLinkType TPhysicalLinkPort;

enum TBTControlPlaneMessage
	{
	EOverridePark,
	EUndoOverridePark,
	EArbitrateForNew,
	EArbitrateForClosed,
	EPresent,
	EPreauthoriseDevice, // used for multiple SAPs operating in one usecase (eg AVRCP and GAVDP);
	ESubscribePhysicalLink,
	EUnsubscribePhysicalLink,
	EOverrideLPM,
	EUndoOverrideLPM,
	ETryToAndThenPreventHostEncryptionKeyRefresh,
	};

/**
Struct used for protocol control-plane message EPreauthoriseDevice, ECancelPreauthoriseDevice
Example: a headset may (in any order) connect AVCTP and AVDTP connections
We do not want to have the user see a double-authorise.
Care should be taken by protocol implementers when using this functionality
*/
struct TOverrideAuthorise
	{
	TBTDevAddr	iPreauthorisedRemoteAddress;
	TInt		iAuthorisingProtocol; // for checking
	TInt		iAuthorisingPort;	// for checking
	TBool		iPreauthorise;
	};
	
enum TPhysicalLinkPresent
	{
	EYes,			// there is a non-parked, physical link to a device
	EYesParked,		// there is a physical link, but it is parked
	ENo,			// no physical link in place
	ENoWay,			// no physical link and no way to create one
	};

enum TBluetoothFamilyPanic
	{
	EBTPrtProtocolCannotListen,
	};

class MBluetoothControlPlaneToken
	{
public:
	static void Release(MBluetoothControlPlaneToken*& aToken);
private:
	virtual void Release() = 0;
	};

NONSHARABLE_CLASS(RBTControlPlane)
	{
public:
// control-plane management
	void AttachProtocolL(TInt aProtocolNum, CBluetoothProtocolBase& aProtocol);
	void DetachProtocol(CBluetoothProtocolBase& aProtocol);
	void Close();

// control-plane services
	TInt ModifyPhysicalLink(TBTControlPlaneMessage aMessage, const TBTDevAddr& aAddr) const;
	void TryToAndThenPreventHostEncryptionKeyRefresh(const TBTDevAddr& aAddr, MBluetoothControlPlaneToken*& aOutToken);

	TPhysicalLinkPresent PhysicalLinkPresent(const TBTDevAddr& aAddr) const;
	TInt Preauthorise(TInt aTargetProtocolNum,
						const TOverrideAuthorise& aSetPreauthorisation);
	TInt SubscribePhysicalLink  (MPhysicalLinkObserver& aObserver, TBTDevAddr aAddr) const;
	TInt UnsubscribePhysicalLink(MPhysicalLinkObserver& aObserver, TBTDevAddr aAddr) const;
private:
	CBluetoothProtocolBase* Protocol(TInt aProtocolNum) const;
private:
	struct TBTProtocol
		{
		CBluetoothProtocolBase*	iProtocol;	// non-owned
		TInt					iProtocolNum;
		};
	RArray<TBTProtocol>	iProtocols;
	};

NONSHARABLE_CLASS(CBTProtocolFamily): public CProtocolFamilyBase
	{
private:
	CBTProtocolFamily();	
public:
	static CBTProtocolFamily* NewL();
	virtual TInt Install();
	virtual TInt Remove();
	virtual CProtocolBase* NewProtocolL(TUint aSockType,TUint aProtocol);
	virtual TUint ProtocolList(TServerProtocolDesc*& aProtocolList);
	virtual void Open(); // must always override Open and Close together
	virtual void Close();
	static TInt Destruct(TAny* aProtocolFamily); // callback
	~CBTProtocolFamily();
private:
	void ConstructL();
private:
	CBTSecMan*	iSecurityMgr;	// owned by family for all protocols
	SBtTls		iBTTls;
	CAsyncCallBack* iCallback;
	RBTControlPlane	iControlPlane;
	TInt iBTRefCount;
	CBTCodServiceMan*	iCodServiceMan;	
	};

NONSHARABLE_CLASS(CBluetoothProtocolBase) : public CProtocolBase
/**
	Abstract class forming the base classes for all Bluetooth protocol objects
	
	Provides the basics such as listening and security
**/
	{
public:
	virtual TInt BearerConnectComplete(const TBTDevAddr& aAddr, CServProviderBase* aSSP) =0;
	virtual TInt ControlPlaneMessage(TBTControlPlaneMessage aMessage, TAny* aParam);

	virtual TInt StartProtocolListening();
	virtual void StopProtocolListening();

	inline CBTSecMan& SecMan() const;
	inline RBTControlPlane& ControlPlane() const;
	inline TBool IsListening() const;
	inline CIncomingConnectionListener& Listener() const;
	inline CProtocolBase* LowerProtocol() const;
	inline CBTCodServiceMan& CodMan() const;

	TInt IncrementListeners(); // called by any object that is prepared to listen at layer n
	void DecrementListeners(); // called by any object that is prepared to listen at layer n
	virtual void Error(TInt aError,CProtocolBase* aSourceProtocol=NULL);
	
protected:
	CBluetoothProtocolBase*			iLowerProtocol;
	CIncomingConnectionListener*	iListener;

protected:
	TInt StartListening(TUint aPort, TUint aSockType, TUint aQueSize, TUid aUid);
	CBluetoothProtocolBase(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	~CBluetoothProtocolBase();

private:
	void DoStartListeningL(TUint aPort, TUint aSockType, TUint aQueSize, TUid aUid);
private:
	CBTSecMan&			iSecMan;
	RBTControlPlane&	iControlPlane;
	TInt				iListeningEntities;
	CBTCodServiceMan&	iCodMan;
	};

inline CBTSecMan& CBluetoothProtocolBase::SecMan() const
	{
	return iSecMan;
	}

inline RBTControlPlane& CBluetoothProtocolBase::ControlPlane() const
	{
	return iControlPlane;
	}

inline TBool CBluetoothProtocolBase::IsListening() const
	{
	return(iListeningEntities > 0);
	}

inline CProtocolBase* CBluetoothProtocolBase::LowerProtocol() const
	{
	return iLowerProtocol;
	}

inline CIncomingConnectionListener& CBluetoothProtocolBase::Listener() const
	{
	return *iListener;
	}

inline CBTCodServiceMan& CBluetoothProtocolBase::CodMan() const
	{
	return iCodMan;
	}

#endif
