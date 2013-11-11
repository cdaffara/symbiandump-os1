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

#include "TBTNotifiersText.h"
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>			// Console

#include <btextnotifiers.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <btextnotifierspartner.h>
#endif

#include <utf.h>
#include <btdevice.h>

//  Constants

_LIT(KTextConsoleTitle, "Notifiers Test");

LOCAL_C void DoStartL ()
	{
	// Create active scheduler (to run active objects)
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL (scheduler);
	CActiveScheduler::Install (scheduler);
	
	CBTNotifierConsole* console = CBTNotifierConsole::NewLC();
	
	CActiveScheduler::Current()->Start();
	CleanupStack::PopAndDestroy (console);
	
	// Delete active scheduler
	CleanupStack::PopAndDestroy (scheduler);
	}

//  Global Functions

GLDEF_C TInt E32Main()
	{
	// Create cleanup stack
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	// Run application code inside TRAP harness, wait keypress when terminated
	TRAPD(mainError, DoStartL());

	delete cleanup;
	__UHEAP_MARKEND;
	return mainError;
	}

CBTNotifierConsole* CBTNotifierConsole::NewL()
	{
	CBTNotifierConsole* self = CBTNotifierConsole::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CBTNotifierConsole* CBTNotifierConsole::NewLC()
	{
	CBTNotifierConsole* self = new(ELeave) CBTNotifierConsole;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CBTNotifierConsole::CBTNotifierConsole(): CActive(EPriorityStandard)
	{
	}

CBTNotifierConsole::~CBTNotifierConsole()
	{
	Cancel();
	delete iConsole;
	}

void CBTNotifierConsole::ConstructL()
	{
	iConsole = Console::NewL(KTextConsoleTitle, TSize(KConsFullScreen,KConsFullScreen));
	CActiveScheduler::Add(this);
	DrawMenu();
	RequestKey();
	}


void CBTNotifierConsole::RunL()
	{
	switch (iConsole->KeyCode())
		{
		case 'a':
			TestAuthorisationL();
			break;
		case 'p':
			TestPinL();
			break;
		case 's':
			TestDeviceSearchL();
			break;
		case 'n':
			TestNumericComparisonL();
			break;
		case 'e':
			TestPasskeyL();
			break;
		case 'l':
			TestLegacyPinL();
			break;
			
		case 'x':
			CActiveScheduler::Current()->Stop();
			break;
		default:
			break;
		}
	DrawMenu();
	RequestKey();
	}

void CBTNotifierConsole::DoCancel()
	{
	iConsole->ReadCancel();
	}

void CBTNotifierConsole::RequestKey()
	{
	iConsole->Read(iStatus);
	SetActive();
	}

void CBTNotifierConsole::DrawMenu()
	{
	iConsole->ClearScreen();
	iConsole->Write(_L("Test BT Notifiers\n\n"));
	
	iConsole->Write(_L("p\tPIN Entry\n"));
	iConsole->Write(_L("l\tLegacy PIN Entry\n"));
	iConsole->Write(_L("s\tDevice Search\n"));
	iConsole->Write(_L("a\tAuthorisation\n"));
	iConsole->Write(_L("n\tNumeric Comparison\n"));
	iConsole->Write(_L("e\tPasskey Entry\n"));

	iConsole->Write(_L("\nx\tExit\n\n"));
	}

void CBTNotifierConsole::TestAuthorisationL()
	{
	TBTAuthorisationParamsPckg pckg;
	pckg().iBDAddr = TInt64(0x12345678);
	pckg().iUid.iUid = 0xabcd1234;
	TPckgBuf<TBool> resultPckg;

	RNotifier notif;
	User::LeaveIfError(notif.Connect());
	TRequestStatus stat;
	notif.StartNotifierAndGetResponse(stat, KBTManAuthNotifierUid, pckg, resultPckg);
	User::After(2000000);
	//now send device name
	TBuf<16> name16 = _L("Tam's Dongle");
	TBTNotifierUpdateParamsPckg updpckg;
	updpckg().iName = name16;
	updpckg().iResult = KErrNone;

	TBuf8<1> answer;
	TInt err = notif.UpdateNotifier(KBTManAuthNotifierUid, updpckg, answer);
	if (err)
		{
		TBuf<4> error;
		error.Zero();
		error.AppendNum(err);
		iConsole->Write(_L("Error: "));
		iConsole->Write(error);
		iConsole->Write(_L("\n"));
		}
	User::WaitForRequest(stat);
	notif.CancelNotifier(KBTManAuthNotifierUid);
	notif.Close();
	if (resultPckg())
		{
		iConsole->Write(_L("Authorisation Accepted"));
		}
	else
		{
		iConsole->Write(_L("Authorisation Denied"));
		}
	User::After(2000000);
	}

void CBTNotifierConsole::TestLegacyPinL()
	{
	TBTDevAddr devAddr(TInt64(0x1234));
	TBTPasskeyNotifierParamsPckg pckg;
	pckg().iPasskeyMinLength = 4;
	pckg().iLocallyInitiated = ETrue;
	
	TBTPinCode pin;
	
	RNotifier notif;
	User::LeaveIfError(notif.Connect());
	TRequestStatus stat;
	notif.StartNotifierAndGetResponse(stat, KBTManPinNotifierUid, pckg, pin);
	User::After(2000000);
	//now send device name
	TBuf<16> name16 = _L("Nick's Dongle");
	TBTNotifierUpdateParamsPckg updatePckg;
	updatePckg().iName = name16;
	updatePckg().iResult = KErrNone;

	TBuf8<1> answer;
	TInt err = notif.UpdateNotifier(KBTManPinNotifierUid, updatePckg, answer);
	if (err)
		{
		TBuf<4> error;
		error.Zero();
		error.AppendNum(err);
		//CEikonEnv::InfoWinL(_L("Error:"), error);
		}
	User::WaitForRequest(stat);
	notif.CancelNotifier(KBTManPinNotifierUid);
	notif.Close();

	TBuf<16> buf16;
	buf16.Zero();

	if (stat.Int() == KErrNone)
		{
		TBuf8<16> buf8;
		buf8.Zero();
		for (TInt i=0; i<pin().iLength; i++)
			{
			buf8.Append(pin().iPIN[i]);
			}
		CnvUtfConverter::ConvertToUnicodeFromUtf8(buf16, buf8);
		}
	else
		{
		buf16.AppendFormat(_L("Error: %d"), stat.Int());
		}
	iConsole->Write(_L("Pin Input Complete: "));
	iConsole->Write(buf16);
	User::After(2000000);
	}

void CBTNotifierConsole::TestDeviceSearchL()
	{
	TBTDevAddr addr = TInt64(0x1234);
	TBTDeviceResponseParamsPckg pckg;
	pckg().SetDeviceAddress(addr);
	pckg().SetDeviceName(_L(""));
	pckg().SetDeviceClass(TBTDeviceClass());

	RNotifier notif;
	User::LeaveIfError(notif.Connect());

	TRequestStatus stat;
	TBuf8<1> dummy;
	notif.StartNotifierAndGetResponse(stat, KDeviceSelectionNotifierUid, dummy, pckg);
	
	User::WaitForRequest(stat);
	notif.CancelNotifier(KDeviceSelectionNotifierUid);
	notif.Close();

	if (stat.Int() == KErrNone)
		{
		TBuf<14> addressText;
		pckg().BDAddr().GetReadable(addressText, _L("0x"), KNullDesC, KNullDesC);
		iConsole->Write(_L("Device Search Complete: "));
		iConsole->Write(addressText);
		}
	else
		{
		iConsole->Write(_L("Device search cancelled"));
		}
	User::After(2000000);
	}

void CBTNotifierConsole::TestNumericComparisonL()
	{
	TBTDevAddr devAddr(TInt64(0x12345678));
	TBTNumericComparisonParamsPckg pckg;
	pckg() = TBTNumericComparisonParams(devAddr, _L("Test Device"), 0x12345678, TBTNumericComparisonParams::ERemoteCanConfirm, ETrue);

	TPckgBuf<TBool> resultPckg;
	
	RNotifier notif;
	User::LeaveIfError(notif.Connect());

	TRequestStatus stat;
	notif.StartNotifierAndGetResponse(stat, KBTNumericComparisonNotifierUid, pckg, resultPckg);
	User::WaitForRequest(stat);
	notif.CancelNotifier(KBTNumericComparisonNotifierUid);

	notif.Close();

	if (resultPckg())
		{
		iConsole->Write(_L("Numeric comparison succeeded"));
		}
	else
		{
		iConsole->Write(_L("Numeric comparison failed"));
		}
	
	User::After(2000000);
	}

void CBTNotifierConsole::TestPasskeyL()
	{
	TBTDevAddr devAddr(TInt64(0x12345678));
	TBTPasskeyDisplayParamsPckg pckg;
	pckg() = TBTPasskeyDisplayParams(devAddr, _L("Test Device"), 0x12345678, ETrue);

	TPckgBuf<TBool> resultPckg;
	
	RNotifier notif;
	User::LeaveIfError(notif.Connect());

	TRequestStatus stat;
	notif.StartNotifierAndGetResponse(stat, KBTPasskeyDisplayNotifierUid, pckg, resultPckg);

	TBTPasskeyDisplayUpdateParamsPckg updatePckg;
	TBuf8<1> answer;

	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyEntryStarted);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);

	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyDigitEntered);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);

	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyDigitEntered);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);

	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyDigitEntered);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);

	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyDigitDeleted);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);

	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyDigitDeleted);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);

	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyDigitEntered);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);

	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyCleared);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);
	
	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyDigitEntered);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);

	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyDigitEntered);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);

	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyDigitEntered);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);

	
	User::After(500000);
	updatePckg() = TBTPasskeyDisplayUpdateParams(EPasskeyEntryCompleted);
	notif.UpdateNotifier(KBTPasskeyDisplayNotifierUid, updatePckg, answer);


	User::WaitForRequest(stat);
	notif.CancelNotifier(KBTPasskeyDisplayNotifierUid);
	notif.Close();
	
	iConsole->Printf(_L("Notifier completed with error %d"), stat.Int());
	
	User::After(2000000);
	}
	
	
void CBTNotifierConsole::TestPinL()
	{
	TBTPinCode pin;
	TBTDevAddr devAddr(TInt64(0x12345678));
	
	TBTPinCodeEntryNotifierParamsPckg pckg;
	pckg() = TBTPinCodeEntryNotifierParams(devAddr, _L("Test Device"), 4, ETrue, ETrue, 4);

	RNotifier notif;
	User::LeaveIfError(notif.Connect());
	CleanupClosePushL(notif);
	TRequestStatus stat;
	notif.StartNotifierAndGetResponse(stat, KBTPinCodeEntryNotifierUid, pckg, pin);
	User::After(2000000);

	//now send device name
	TBTDeviceNameUpdateParamsPckg updatePckg;
	updatePckg() = TBTDeviceNameUpdateParams(_L("New Device Name"), KErrNone);
	TBuf8<1> answer;
	User::LeaveIfError(notif.UpdateNotifier(KBTPinCodeEntryNotifierUid, updatePckg, answer));
	User::WaitForRequest(stat);

	notif.CancelNotifier(KBTPinCodeEntryNotifierUid);
	CleanupStack::PopAndDestroy(&notif);

	TBuf<16> buf16;
	buf16.Zero();

	if (stat.Int() == KErrNone)
		{
		TBuf8<16> buf8;
		buf8.Zero();
		for (TInt i=0; i<pin().iLength; i++)
			{
			buf8.Append(pin().iPIN[i]);
			}
		CnvUtfConverter::ConvertToUnicodeFromUtf8(buf16, buf8);
		}
	else
		{
		buf16.AppendFormat(_L("Error: %d"), stat.Int());
		}
	iConsole->Write(_L("Pin Input Complete: "));
	iConsole->Write(buf16);
	User::After(2000000);
	}
