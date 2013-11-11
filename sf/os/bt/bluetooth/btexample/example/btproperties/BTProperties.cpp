// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32std.h>
#include <e32test.h>
#include <bt_subscribe.h>
#include <bt_sock.h>
#include "BTProperties.h"

_LIT(KNumLinks, "Number of links:");
_LIT(KConnecting, "Connecting...");
_LIT(KRegistry, "Registry changed.");
_LIT(KLimited, "Limited discoverable:");
_LIT(KScanning, "Scans:"); // could resolve this into discoverable/connectable
_LIT(KDeviceClass, "DeviceClass:");
_LIT(KDeviceName, "DeviceName:");
_LIT(KCorruptRegistry, "Registry reset at this point:");
_LIT(KDiscovering, "Discovering...");

GLDEF_D RTest test(_L("Bluetooth P&S Subscription Tests"));

void ShowAddress()
/**
	Shows the example of using a synchronous Get to retrieve local BTAddr

*/
	{
	RProperty property;
	TBuf8<6> addr;

	TInt err = property.Get(KPropertyUidBluetoothCategory,
					KPropertyKeyBluetoothGetLocalDeviceAddress, addr);

	if (err)
		test.Printf(_L("P&S: ERROR retrieving local address\n"));
	else
		{
		TBTDevAddr localAddress(addr);
		TBuf<20> dispBuf;
		localAddress.GetReadable(dispBuf);
		test.Printf(_L("Local address = 0x%S\n"),&dispBuf);
		}
	}

void CreateSubscribersL(RPointerArray<CSubscriber>& aSubscribers)
	{
	CSubscriber* subscriber = CSubscriber::NewL(test, KPropertyKeyBluetoothGetPHYCount, KNumLinks);
	
	CleanupStack::PushL(subscriber);
	aSubscribers.AppendL(subscriber);
	CleanupStack::Pop(subscriber);
	
	subscriber = CSubscriber::NewL(test, KPropertyKeyBluetoothGetRegistryTableChange, KRegistry);
	CleanupStack::PushL(subscriber);
	aSubscribers.AppendL(subscriber);
	CleanupStack::Pop(subscriber);
	
	subscriber = CSubscriber::NewL(test, KPropertyKeyBluetoothGetConnectingStatus, KConnecting);
	CleanupStack::PushL(subscriber);
	aSubscribers.AppendL(subscriber);
	CleanupStack::Pop(subscriber);
		
	subscriber = CSubscriber::NewL(test, KPropertyKeyBluetoothGetScanningStatus, KScanning);
	CleanupStack::PushL(subscriber);
	aSubscribers.AppendL(subscriber);
	CleanupStack::Pop(subscriber);

	subscriber = CSubscriber::NewL(test, KPropertyKeyBluetoothGetLimitedDiscoverableStatus, KLimited);
	CleanupStack::PushL(subscriber);
	aSubscribers.AppendL(subscriber);
	CleanupStack::Pop(subscriber);
	
	subscriber = CSubscriber::NewL(test, KPropertyKeyBluetoothGetDeviceClass, KDeviceClass);
	CleanupStack::PushL(subscriber);
	aSubscribers.AppendL(subscriber);
	CleanupStack::Pop(subscriber);
	
	subscriber = CSubscriber::NewL(test, KPropertyKeyBluetoothGetCorruptRegistryResetIndication, KCorruptRegistry);
	CleanupStack::PushL(subscriber);
	aSubscribers.AppendL(subscriber);
	CleanupStack::Pop(subscriber);
	
	subscriber = CDeviceNameSubscriber::NewL(test, KPropertyKeyBluetoothGetDeviceName, KDeviceName);
	CleanupStack::PushL(subscriber);
	aSubscribers.AppendL(subscriber);
	CleanupStack::Pop(subscriber);
	
	subscriber = CSubscriber::NewL(test, KPropertyKeyBluetoothHostResolverActive, KDiscovering);
	CleanupStack::PushL(subscriber);
	aSubscribers.AppendL(subscriber);
	CleanupStack::Pop(subscriber);
	}


void TestL()
	{
	// first do a sync test
	ShowAddress();

	RPointerArray<CSubscriber> subscribers;

	TRAPD(err, CreateSubscribersL(subscribers));

	test.Printf(_L("%d Subscribers\n"), subscribers.Count());

	if (err == KErrNone)
		{
		// Only start if no errors when creating the subscribers
		for (TInt i=0; i<subscribers.Count(); i++)
			{
			subscribers[i]->Start();
			}
		CActiveScheduler::Start();
		}
	subscribers.ResetAndDestroy();
	}

CSubscriber* CSubscriber::NewL(RTest& aTest, TUint aKey, const TDesC& aString)
	{
	CSubscriber* s = new(ELeave) CSubscriber(aTest, aString);
	CleanupStack::PushL(s);
	s->ConstructL(aKey);
	CleanupStack::Pop(s);
	return s;
	}

CSubscriber::CSubscriber(RTest& aTest, const TDesC& aString)
: CActive(EPriorityStandard), iTest(aTest), iString(aString)
	{
	CActiveScheduler::Add(this);
	}

CSubscriber::~CSubscriber()
	{
	Cancel();
	}

void CSubscriber::ConstructL(TUint aKey)
	{
	User::LeaveIfError(iProperty.Attach(KPropertyUidBluetoothCategory, aKey));
	}

void CSubscriber::Start()
	{
	iProperty.Subscribe(iStatus);
	SetActive();
	}

void CSubscriber::RunL()
	{
	Start();
	TInt val;
	iProperty.Get(val);

	iTest.Printf(_L("%S = %d\n"), &iString, val);
	}

void CSubscriber::DoCancel()
	{
	iProperty.Cancel();
	}

/*************************************************************************/
//
// CDeviceNameSubscriber Implementation
// 
CDeviceNameSubscriber* CDeviceNameSubscriber::NewL(RTest& aTest, TUint aKey, const TDesC& aString)
	{
	CDeviceNameSubscriber* s = new(ELeave) CDeviceNameSubscriber(aTest, aString);
	CleanupStack::PushL(s);
	s->ConstructL(aKey);
	CleanupStack::Pop(s);
	return s;
	}

CDeviceNameSubscriber::CDeviceNameSubscriber(RTest& aTest, const TDesC& aString)
: CSubscriber(aTest,aString)
	{
	}

CDeviceNameSubscriber::~CDeviceNameSubscriber()
	{
	// No need to cancel since it's done in the base class
	}

void CDeviceNameSubscriber::RunL()
	{
	Start();
	
	TBuf16<KMaxBluetoothNameLen> val;
	iProperty.Get(val);

	iTest.Printf(_L("%S = %S\n"), &iString, &val);
	}


TInt E32Main()
	{
	CTrapCleanup* cleanupStack=CTrapCleanup::New();
	CActiveScheduler::Install(new CActiveScheduler);

	TRAPD(err,TestL());	//	Ignore err

	if (err != KErrNone)
		{
		test.Printf(_L("Error %d"), err);
		test.Getch();
		}

	delete cleanupStack;
	return err;
	}
