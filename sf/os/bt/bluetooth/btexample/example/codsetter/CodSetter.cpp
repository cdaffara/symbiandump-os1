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
#include "CodSetter.h"

_LIT(KInstructions, "O - ObjectTransfer\nP = Positioning\nN - Networking\nC - Capturing\nA - Audio\nT - Telephony\nI - Information\nR - Rendering\n");

GLDEF_D RTest test(_L("Cod Setter App"));

void TestL()
	{
	test.Printf(_L("%S\n"), &KInstructions);

	CCodSetter* codsetter = CCodSetter::NewL(test);

	CActiveScheduler::Start();

	delete codsetter;
	}


CCodSetter* CCodSetter::NewL(RTest& aTest)
	{
	CCodSetter* s = new(ELeave) CCodSetter(aTest);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return s;
	}

CCodSetter::CCodSetter(RTest& aTest)
: CActive(EPriorityStandard), iTest(aTest)
	{
	CActiveScheduler::Add(this);
	}

CCodSetter::~CCodSetter()
	{
	Cancel();
	}

void CCodSetter::ConstructL()
	{
	TInt err;
	

_LIT_SECURITY_POLICY_PASS(KPassPolicy);
	err=iProperty.Define(KPropertyUidBluetoothCategory,
							KPropertyKeyBluetoothSetDeviceClass,
							RProperty::EInt,
							KPassPolicy,		//	Read policy
							KPassPolicy);		//	Write policy
	
	if (err) test.Printf(_L("Error %d defining property, continuing anyway\n"),err);
	Start();
	}

void CCodSetter::Start()
	{
	iTest.Console()->Read(iStatus);
	SetActive();
	}

void CCodSetter::RunL()
	{
	Start();
	
	TChar ch = iTest.Console()->KeyCode();

	TUint16 bit = 0;

	switch (ch)
		{
		case 'i': case 'I':
			bit = EMajorServiceInformation;
			break;

		case 'p': case 'P':
			bit = EMajorServicePositioning;
			break;

		case 'o': case 'O':
			bit = EMajorServiceObjectTransfer;
			break;

		case 'n': case 'N':
			bit = EMajorServiceNetworking;
			break;

		case 'c': case 'C':
			bit = EMajorServiceCapturing;
			break;

		case 'r': case 'R':
			bit = EMajorServiceRendering;
			break;

		case 't': case 'T':
			bit = EMajorServiceTelephony;
			break;

		default:
			break; // do nothing
		}

	TUint16 s = iServiceClassToSet;
	s ^= bit;
	TBTDeviceClass cod(s,KMajorDeviceClass,KMinorDeviceClass);

	iProperty.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetDeviceClass, cod.DeviceClass());

	iServiceClassToSet = static_cast<TBTMajorServiceClass>(s);
	}

void CCodSetter::DoCancel()
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
