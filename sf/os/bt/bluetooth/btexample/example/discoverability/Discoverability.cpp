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

#include <e32base.h>
#include <e32std.h>
#include <e32test.h>
#include <bt_subscribe.h>
#include <bt_sock.h>
#include "Discoverability.h"



_LIT(KInstructions, "Press I to be invisible, V to be visible\nL to be limited, G to be general\nC to change CoD\n");

GLDEF_D RTest test(_L("Discoverability App"));

void TestL()
	{
	test.Printf(_L("%S\n"), &KInstructions);

	CDiscoverability* discoverability = CDiscoverability::NewL(test);

	CActiveScheduler::Start();

	delete discoverability;
	}


CDiscoverability* CDiscoverability::NewL(RTest& aTest)
	{
	CDiscoverability* s = new(ELeave) CDiscoverability(aTest);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return s;
	}

CDiscoverability::CDiscoverability(RTest& aTest)
: CActive(EPriorityStandard), iTest(aTest)
	{
	CActiveScheduler::Add(this);
	}

CDiscoverability::~CDiscoverability()
	{
	Cancel();
	}

void CDiscoverability::ConstructL()
	{
	TInt err;
	

	_LIT_SECURITY_POLICY_PASS(KPassPolicy);
	err=iProperty.Define(KPropertyUidBluetoothCategory,
										KPropertyKeyBluetoothSetScanningStatus,
										RProperty::EInt,
										KPassPolicy,	//	Read policy
										KPassPolicy);	//	Write policy
	if (err) test.Printf(_L("Error %d defining property, continuing anyway\n"),err);
	err=iProperty.Define(KPropertyUidBluetoothCategory,
										KPropertyKeyBluetoothSetLimitedDiscoverableStatus,
										RProperty::EInt,
										KPassPolicy,	//	Read policy
										KPassPolicy);	//	Write policy
	if (err) test.Printf(_L("Error %d defining property, continuing anyway\n"),err);
	err=iProperty.Define(KPropertyUidBluetoothCategory,
										KPropertyKeyBluetoothSetDeviceClass,
										RProperty::EInt,
										KPassPolicy,	//	Read policy
										KPassPolicy);	//	Write policy
	
	if (err) test.Printf(_L("Error %d defining property, continuing anyway\n"),err);
	
	Start();
	}

void CDiscoverability::Start()
	{
	iTest.Console()->Read(iStatus);
	SetActive();
	}


void CDiscoverability::RunL()
	{
	Start();
	
	TChar ch = iTest.Console()->KeyCode();
	TInt err = KErrNone;
	
	switch (ch)
		{
		case 'i': case 'I':
			err=iProperty.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetScanningStatus, EPageScanOnly);
			test.Printf(_L("Set Page Scan mode with result %d\n"),err);
			break;

		case 'v': case 'V':
			err=iProperty.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetScanningStatus, EInquiryAndPageScan);
			test.Printf(_L("Set InquiryAndPage Scan mode with result %d\n"),err);
			break;

		case 'l': case 'L':
			err=iProperty.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetLimitedDiscoverableStatus, ETrue);
			test.Printf(_L("Set Limited Discoverable mode ON with result %d\n"),err);
			break;

		case 'g': case 'G':
			err=iProperty.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetLimitedDiscoverableStatus, EFalse);
			test.Printf(_L("Set Limited Discoverable mode OFF with result %d\n"),err);
			break;

		case 'c': case 'C':
			{
			TUint8 majorDeviceClass = User::TickCount() % 8;
			TBTDeviceClass cod(0,majorDeviceClass,0);
			err=iProperty.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetDeviceClass, cod.DeviceClass());
			test.Printf(_L("Changed COD with result %d\n"),err);
			break;
			}

		default:
			break; // do nothing
		}
	}

void CDiscoverability::DoCancel()
	{
	iTest.Console()->ReadCancel();
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
