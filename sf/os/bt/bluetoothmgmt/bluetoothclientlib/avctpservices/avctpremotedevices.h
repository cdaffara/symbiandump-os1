// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef AVCTPREMOTEDEVICES_H
#define AVCTPREMOTEDEVICES_H

#include <es_sock.h>
#include <avctpservices.h>
#include "avctpserviceutils.h"

// forward declaration
class CAvctpBody;
class CPrimaryChannelController;
class CSecondaryChannelController;

//TODO (FBO) It will be moved somewhere in common with other code
/**
A wrapper class around an RFastLock that enables locks to be nested.
*/
NONSHARABLE_CLASS(RNestableLock)
	{
public:
	RNestableLock();
	TInt CreateLocal();
	void Close();
	
	void Wait();
	void Signal();

private:
	static const TUint64 KInvalidThreadId = ~0u;
	RFastLock	iLock;
	RFastLock	iMetaLock;
	TInt		iRefCount;
	TThreadId	iThreadId;
	};

NONSHARABLE_CLASS(TAvctpRemoteDeviceInfo)
	{
public:
	TAvctpRemoteDeviceInfo(const TBTDevAddr& aAddr);
	TBool HasSecondChannel() const;
	void SetHasSecondChannel(TBool aHasSecondChannel);
	const TBTDevAddr& RemoteAddress() const;
private:
	TBTDevAddr	iAddr;
	TBool		iHasSecondChannel;
	};

/**
 MChannelControlNotify 
 **/
class MControlChannelNotify
	{
public:
	// Main channel notifications
	virtual void PrimaryChannelAttachConfirm(const TBTDevAddr& aAddr, TInt aMtu, TInt aError) = 0;
	virtual void PrimaryChannelAttachIndicate(const TBTDevAddr& aAddr, TInt aMtu) = 0;
	virtual void PrimaryChannelDetachConfirm(const TBTDevAddr& aAddr, TInt aError) = 0;
	virtual void PrimaryChannelDetachIndicate(const TBTDevAddr& aAddr) = 0;
	virtual void PrimaryChannelIoctlError(const TBTDevAddr& aAddr, TInt aError) = 0;
	virtual void PrimaryChannelAgreementError(const TBTDevAddr& aAddr, TInt aError) = 0;
	
	// Second channel notifications
	virtual void SecondaryChannelAttachConfirm(const TBTDevAddr& aAddr, TInt aMtu, TInt aError) = 0;
	virtual void SecondaryChannelAttachIndicate(const TBTDevAddr& aAddr, TInt aMtu) = 0;
	virtual void SecondaryChannelDetachConfirm(const TBTDevAddr& aAddr, TInt aError) = 0;
	virtual void SecondaryChannelDetachIndicate(const TBTDevAddr& aAddr) = 0;
	virtual void SecondaryChannelIoctlError(const TBTDevAddr& aAddr, TInt aError) = 0;
	virtual void SecondaryChannelAgreementError(const TBTDevAddr& aAddr, TInt aError) = 0;
	};

/**
AVCTP active class to deal with informing the prt of requests relating to 
remote devices

@internalComponent
*/
NONSHARABLE_CLASS(CAvctpRemoteDevices) : public CBase, public MControlChannelNotify
	{
public:
	static CAvctpRemoteDevices* NewL(MAvctpEventNotify& aNotify, RSocketServ& aSocketServ, SymbianAvctp::TPid aPid);
	static CAvctpRemoteDevices* NewLC(MAvctpEventNotify& aNotify, RSocketServ& aSocketServ, SymbianAvctp::TPid aPid);
	~CAvctpRemoteDevices();
	
	// Methods to support RAvctp
	TInt PrimaryChannelAttachRequest(const TBTDevAddr& aBTDevice);
	TInt PrimaryChannelDetachRequest(const TBTDevAddr& aBTDevice);
	void PrimaryChannelCancelAttach(const TBTDevAddr& aBTDevice);
	
	TInt SecondaryChannelAttachRequest(const TBTDevAddr& aBTDevice);
	TInt SecondaryChannelDetachRequest(const TBTDevAddr& aBTDevice);
	void SecondaryChannelCancelAttach(const TBTDevAddr& aBTDevice);
	
	void SetSecondaryChannelNotifyL(MAvctpEventNotify* aSecondChannelNotify);
	// Methods to support CAvctpBody
	TBool IsAttached(const TBTDevAddr& aBTDevice) const;

	// from MControlChannelNotify
	void PrimaryChannelAttachConfirm(const TBTDevAddr& aAddr, TInt aMtu, TInt aError);
	void PrimaryChannelAttachIndicate(const TBTDevAddr& aAddr, TInt aMtu);
	void PrimaryChannelDetachConfirm(const TBTDevAddr& aAddr, TInt aError);
	void PrimaryChannelDetachIndicate(const TBTDevAddr& aAddr);
	void PrimaryChannelIoctlError(const TBTDevAddr& aAddr, TInt aError);
	void PrimaryChannelAgreementError(const TBTDevAddr& aAddr, TInt aError);
	
	void SecondaryChannelAttachConfirm(const TBTDevAddr& aAddr, TInt aMtu, TInt aError);
	void SecondaryChannelAttachIndicate(const TBTDevAddr& aAddr, TInt aMtu);
	void SecondaryChannelDetachConfirm(const TBTDevAddr& aAddr, TInt aError);
	void SecondaryChannelDetachIndicate(const TBTDevAddr& aAddr);
	void SecondaryChannelIoctlError(const TBTDevAddr& aAddr, TInt aError);
	void SecondaryChannelAgreementError(const TBTDevAddr& aAddr, TInt aError);

private:
	CAvctpRemoteDevices(MAvctpEventNotify& aNotify, RSocketServ& aSocketServ);
	void ConstructL(SymbianAvctp::TPid aPid);
	
	void RemoveRemoteDevice(const TBTDevAddr& aBTDevice);
	const TAvctpRemoteDeviceInfo* RemoteDeviceInfo(const TBTDevAddr& aBTDevice) const;
	TInt PropagateAttachRsp(const TBTDevAddr& aBTDevice, 
							 TBool aClientsAccepts,
							 TInt aChannel);

	void NotifyError(const TBTDevAddr& aBTDevice, TInt aError, TInt aChannel);
	
private:
	MAvctpEventNotify&	iNotify;
	RSocketServ&		iSocketServ;	

	// This is an array of remote addresses that are either connected or connecting
	RArray<TAvctpRemoteDeviceInfo> iRemoteAddrs;

	enum TState {EListening, ECreatingControlLink, ECreatingSecondLink};
	TState	iState;
	
	MAvctpEventNotify* 				iSecondaryChannelNotify;	// non-owned
	CPrimaryChannelController*		iPrimaryChannelController;
	CSecondaryChannelController*	iSecondaryChannelController;
	mutable RNestableLock		 	iLock; // to use in the const functions
	SymbianAvctp::TPid				iPid;
	};

#endif // AVCTPREMOTEDEVICES_H

