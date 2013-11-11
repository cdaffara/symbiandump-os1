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
#include <c32comm.h>
#include <f32file.h>
#include <bt_subscribe.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <bt_subscribe_partner.h>
#endif

#include <bt_sock.h>
#include "AFHSetter.h"
#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
// #define ETNA_PDD_NAME _L("EUART2") // for debugging over com2
#endif

_LIT(KInstructions, "ffffffffffffffffffff allows all frequencies.\nYou will either be instructed to type in a new number\nor to enter individual bad channels by choosing a sequence\nof numbers in [0, 78]");

inline void __BTDEBUGGER()
// needed to call __DEBUGGER inside an __ASSERT
	{
	__DEBUGGER()
	}

GLDEF_D RTest test(_L("AFH Setter App"));

void LoadLDD_PDD()
	{
	TInt r;
#ifdef __EPOC32__
	r=StartC32();
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Started C32\n"));
#endif
	test.Printf(_L("Loading PDD\n"));
	r=User::LoadPhysicalDevice(PDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else 
		test.Printf(_L("Loaded LDD\n"));
	test.Printf(_L("Loading LDD\n"));
	r=User::LoadLogicalDevice(LDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Loaded PDD\n"));
	}


void TestL()
	{
	test.Printf(_L("%S\n"), &KInstructions);

	// This function is essential!
	LoadLDD_PDD();
	// For some reason, you have to do the following to
	// ensure that the file server behaves properly.


//	File session
	RFs fs;
	fs.Connect();
	fs.Close();
	
	CAFHSetter* AFHsetter = CAFHSetter::NewL(test);
	//Subscriber* AFHSubscriber = CAFHSubscriber::NewL(test);

	CActiveScheduler::Start();
	
	//delete AFHSubscriber;
	delete AFHsetter;
	}


CAFHSetter* CAFHSetter::NewL(RTest& aTest)
	{
	CAFHSetter* s = new(ELeave) CAFHSetter(aTest);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return s;
	}

CAFHSetter::CAFHSetter(RTest& aTest)
: CActive(EPriorityStandard), iTest(aTest)
	{
	CActiveScheduler::Add(this);
	}

CAFHSetter::~CAFHSetter()
	{
	Cancel();
	iTimer.Close();
	iSS.Close();
	}

void CAFHSetter::ConstructL()
	{
	//start timer
	iTimer.CreateLocal();
	//start bluetooth
	iSS.Connect();
	TRequestStatus status;
    //iSS.StartProtocol(KBTAddrFamily,KSockSeqPacket,KBTLinkManager,status);
    iSS.StartProtocol(KBTAddrFamily,KSockSeqPacket,KL2CAP,status);
    User::WaitForRequest(status);
	
	//start publish and subscribe
	_LIT_SECURITY_POLICY_PASS(KPassPolicy);
	TInt err;
	err=iProperty.Define(KPropertyUidBluetoothCategory,
						 KPropertyKeyBluetoothSetAFHHostChannelClassification,
						 RProperty::EByteArray,
						 KPassPolicy,
						 KPassPolicy);
	err=iProperty.Define(KPropertyUidBluetoothCategory,
						 KPropertyKeyBluetoothSetAFHChannelAssessmentMode,
						 RProperty::EInt,
						 KPassPolicy,
						 KPassPolicy);
	__ASSERT_DEBUG(err==0, __BTDEBUGGER());
	
	if (err) test.Printf(_L("Error %d defining property, continuing anyway\n"),err);


	Start();
	}

void CAFHSetter::Start()
	{
	iTest.Console()->ClearScreen();
	SendBluetoothAFHHostChannelClassification(); //send classification as kick off
	GetClassificationBySettingBadChannels();
	iTimer.After(iStatus,1);
	SetActive();
	}

TInt CAFHSetter::SendBluetoothAFHHostChannelClassification()
	{
	//Set channels 3,4,5,6,7,8,9,10,12 and 15 as busy - not to be used by 
	//Bluetooth Adaptive Frequency Hopping.
	RProperty property;
	TBTAFHHostChannelClassification channelClassification;
	_LIT_SECURITY_POLICY_PASS(KPassPolicy);
	TInt ret = KErrNone;
	ret=property.Define(KPropertyUidBluetoothCategory,
				        KPropertyKeyBluetoothSetAFHHostChannelClassification,
				        RProperty::EByteArray,
						KPassPolicy,
						KPassPolicy);
	if(ret!=KErrNone)
			{
			return ret;
			}
	ret = channelClassification.SetChannelRangeBad(3, 10);
	if(ret!=KErrNone)
		{
		return ret;
		}
	ret = channelClassification.SetChannelBad(12);
	if(ret!=KErrNone)
		{
		return ret;
		}
	ret = channelClassification.SetChannelBad(15);
	if(ret!=KErrNone)
		{
		return ret;
		}
	ret = property.Set(KPropertyUidBluetoothCategory,  
					   KPropertyKeyBluetoothSetAFHHostChannelClassification, 
					   channelClassification);
	property.Delete(KPropertyUidBluetoothCategory,
			 KPropertyKeyBluetoothSetAFHHostChannelClassification);
	return ret;
	}

TInt CAFHSetter::GetNextChannelNum()
    {
	TKeyCode code;
	TBuf8<2> character;
	TUint8 byte;
	character.SetLength(0);

	FOREVER
		{
		code = iTest.Console()->Getch();
		iState &= ~KSetChannelsBad;
	
		// If <CR> finish editing string
		if (code == 0x0d)
			{
			return KSendHCC;
			}

		else if (code == 'r' || code == 'R')
			{
			iTest.Console()->Printf(_L("Taking ranges...\n"));
			iState |= KPairs;
			iState &= ~KLast;
			character.SetLength(0);
			}
		
		else if (code == 's' || code == 'S')
			{
			iTest.Console()->Printf(_L("Taking singles...\n"));
			iState &= ~KPairs;
			iState |= KLast;
			character.SetLength(0);
			}
		
		// if <BS> remove last character
		else if (code == 0x08)
			{
			character.SetLength(0);
			}

		else
			{
			if (character.Length() == 0)
				{
				character.Append(code);
				}
			else
				{
				character.Append(code);
				TLex8 lex(character);
				character.SetLength(0);
				TInt err = lex.Val(byte, EDecimal);
				if(!err)
					{
					iTest.Console()->Printf(_L("%02d"),byte);
					if(iState & KLast)
						{
						iTest.Console()->Printf(_L("\n"));
						iChannel2 = byte;
						if(iState & KPairs)
							{
							iState &= ~KLast;
							}
						iState |= KSetChannelsBad;
						}
					else
						{
						iTest.Console()->Printf(_L(", "));
						iChannel1 = byte;
						iState |= KLast;
						}
					return KErrNone;
					}
				else
					{
					__BTDEBUGGER();
					return err;
					}
				}
			}
		}
    }

void CAFHSetter::GetClassificationBySettingBadChannels()
    {
	test.Printf(_L("Please enter a sequence of bad channels [0, 78]\n"));
	test.Printf(_L("Please type 'r' to CHANGE to enter bad channels as RANGES...\n"));
	test.Printf(_L(".... type 's' to CHANGE to enter bad channels SINGLY (default)...\n"));
	test.Printf(_L("Press return to finish\n\n"));
	
	iHCC.Reset();

	for(TUint8 i=0; i<KHCIAFHHostChannelClassificationSize; ++i)
		{
		__ASSERT_DEBUG(iHCC[i] == 0xff, __BTDEBUGGER());
		}
	
	iState = 0;
	iState |= KLast;

	FOREVER
		{
		TInt ret = GetNextChannelNum();
		if(ret==KSendHCC)
			{
			return;
			}
		
		else if(ret!=KErrNone)
			{
			__BTDEBUGGER();
			}
			
		else if(ret==KErrNone && iState&KSetChannelsBad)
			{
			if(iState & KPairs)
				{
				ret = iHCC.SetChannelRangeBad(iChannel1, iChannel2);
				}
			else
				{
				ret = iHCC.SetChannelBad(iChannel2);
				}
				
			if(ret!=KErrNone)
				{
				__BTDEBUGGER();
				}
			}
		else
			{
			__ASSERT_DEBUG(!(iState&KSetChannelsBad), __BTDEBUGGER());
			}
		}
    }

void CAFHSetter::DoCancel()
	{
	iTest.Console()->ReadCancel();
	}

void CAFHSetter::RunL()
	{
	if(TInt err = iStatus.Int()!=KErrNone)
		{
		__BTDEBUGGER();
		}
	TInt ret = iProperty.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetAFHHostChannelClassification, iHCC);
	__ASSERT_DEBUG(ret==0, __BTDEBUGGER());
	//second request
	/**/
	ret = iProperty.Set(KPropertyUidBluetoothCategory, KPropertyKeyBluetoothSetAFHHostChannelClassification, iHCC);
	__ASSERT_DEBUG(ret==0, __BTDEBUGGER());
	/**/
	test.Printf(_L("\n\nContinue?\n"));
	TKeyCode code;
	code = iTest.Console()->Getch();
	switch(code)
		{
		case 'n':
		case 'N':
		case 27:
			CActiveScheduler::Stop();
			return;
		default:
			;//continue
		}

	Start();
	}



//....
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
