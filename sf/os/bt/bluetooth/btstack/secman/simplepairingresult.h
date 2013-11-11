// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SIMPLEPAIRINGRESULT_H
#define SIMPLEPAIRINGRESULT_H

#include <e32base.h>
#include <bttypes.h>
#include "internaltypes.h"

/**
This constant defines the maximum number of simple pairing results
that are queued up in the system.  If the list grows above this
size then older results will be dropped and lost.
*/
const TInt KSimplePairingResultListMaxSize = 10;

/**
This constant defines the maximum number of authentication results
that are queued up in the system.  If the list grows above this
size then older results will be dropped and lost.
*/
const TInt KAuthenticationResultListMaxSize = 10;


NONSHARABLE_CLASS(TBluetoothSecurityResult)
	{
public:
	TBluetoothSecurityResult(const TBTDevAddr& aDevAddr, TInt aResult);

	const TBTDevAddr& DeviceAddress() const;
	TInt Result() const;

public:
	TBTDblQueLink	iLink;

private:
	TBTDevAddr	iDeviceAddress;
	TInt		iResult;
	};

class MBluetoothSecurityResultObserver
	{
public:
	virtual TInt MbsroResult(const TBTDevAddr& aDevAddr, TInt aResult) = 0;
	};

NONSHARABLE_CLASS(CBluetoothSecurityResultList)
	: public CBase
	{
public:
	~CBluetoothSecurityResultList();

	void NewResult(const TBTDevAddr& aDevAddr, TInt aResult);

	void RegisterObserverL(MBluetoothSecurityResultObserver& aObserver);
	void ReleaseObserver(MBluetoothSecurityResultObserver& aObserver);

	void ReturnResult();
	void CancelReturn();

protected:
	CBluetoothSecurityResultList(TInt aMaxListCount);
	void ConstructL();

private:
	MBluetoothSecurityResultObserver*	iObserver;
	TBool								iOutstandingNotification;

	TDblQue<TBluetoothSecurityResult>	iResultList;
	TInt								iListCount;
	TInt								iMaxListCount;
	};


NONSHARABLE_CLASS(CSimplePairingResultList)
	: public CBluetoothSecurityResultList
	{
public:
	static CSimplePairingResultList* NewL();

private:
	CSimplePairingResultList();
	};

NONSHARABLE_CLASS(CAuthenticationResultList)
	: public CBluetoothSecurityResultList
	{
public:
	static CAuthenticationResultList* NewL();

private:
	CAuthenticationResultList();
	};

_LIT(KBluetoothSecListFaultCat, "SecListFault");
enum TBluetoothSecListFault
	{
	EResultListCountMismatch,
	ENoBoundObserver,
	ENullResultListEntry,
	ENotificationAlreadyOutstanding,
	};

#endif // SIMPLEPAIRINGRESULT_H
