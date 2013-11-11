// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TBTNotifiers app
// 
//

#include <e32keys.h>
#include <coemain.h>
#include <eikenv.h>
#include <eikdef.h>
#include <tbtnotifiers.rsg>
#include "TBTNotifiers.hrh"
#include "TBTNotifiers.h"
#include <btextnotifiers.h>
#include <utf.h>
#include <btdevice.h>

void Panic(TInt aPanic)
	{
	User::Panic(_L("TBTNotifiers App"),aPanic);
	}


// Class CTBTNotifiersContainer

void CTBTNotifiersContainer::ConstructL(const TRect& aRect)
	{
	CreateWindowL();
	SetBlank();
	SetRect(aRect);
	ActivateL();
	}

CTBTNotifiersContainer::~CTBTNotifiersContainer()
	{
	}

void CTBTNotifiersContainer::SizeChanged()
	{
	}

TInt CTBTNotifiersContainer::CountComponentControls() const
	{
	return 0;
	}

CCoeControl* CTBTNotifiersContainer::ComponentControl(TInt /*aIndex*/) const
	{
	return NULL;
	}

void CTBTNotifiersContainer::Draw(const TRect& /*aRect*/) const
	{
	}


//
// CTBTNotifiersAppUi
//
CTBTNotifiersAppUi::~CTBTNotifiersAppUi()
	{
	RemoveFromStack(iAppView);
	delete iAppView;
	}

void CTBTNotifiersAppUi::ConstructL()
	{
	BaseConstructL();
	iAppView=new(ELeave) CTBTNotifiersContainer;
	iAppView->ConstructL(ClientRect());
	AddToStackL(iAppView);
	}

void CTBTNotifiersAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
			Exit();
			break;
		case ETBTNotifiersCmdTestPin:
			iEikonEnv->InfoMsg(_L("testing pin..."));
			TestPinL();
			break;
		case ETBTNotifiersCmdTestAuth:
			iEikonEnv->InfoMsg(_L("testing authorisation..."));
			TestAuthorisationL();
			break;
		case ETBTNotifiersCmdTestDeviceSearch:
			iEikonEnv->InfoMsg(_L("testing device search..."));
			TestDeviceSearchL();
			break;
		default:
			Panic(0);
			break;
		}
	}

void CTBTNotifiersAppUi::TestAuthorisationL()
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
		CEikonEnv::InfoWinL(_L("Error:"), error);
		}
	User::WaitForRequest(stat);
	notif.CancelNotifier(KBTManAuthNotifierUid);
	notif.Close();

	TBuf<16> resultbuf;
	resultbuf.Zero();
	if (resultPckg())
		resultbuf.Append(_L("Accepted"));
	else
		resultbuf.Append(_L("Denied"));
	CEikonEnv::InfoWinL(_L("Authorisation Complete:"), resultbuf);
	}

void CTBTNotifiersAppUi::TestPinL()
	{
	TBTPinCode pin;
	TBTDevAddr addr = TInt64(0x1234);
	RNotifier notif;
	User::LeaveIfError(notif.Connect());
	TRequestStatus stat;
	notif.StartNotifierAndGetResponse(stat, KBTManPinNotifierUid, TPckgBuf<TBTDevAddr>(addr), pin);
	User::After(2000000);
	//now send device name
	TBuf<16> name16 = _L("Nick's Dongle");
	TBTNotifierUpdateParamsPckg pckg;
	pckg().iName = name16;
	pckg().iResult = KErrNone;

	TBuf8<1> answer;
	TInt err = notif.UpdateNotifier(KBTManPinNotifierUid, pckg, answer);
	if (err)
		{
		TBuf<4> error;
		error.Zero();
		error.AppendNum(err);
		CEikonEnv::InfoWinL(_L("Error:"), error);
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
		buf16.Append(_L("CANCELLED"));
		}
	CEikonEnv::InfoWinL(_L("Pin Input Complete:"), buf16);
	}

void CTBTNotifiersAppUi::TestDeviceSearchL()
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
	notif.CancelNotifier(KBTManPinNotifierUid);
	notif.Close();

	if (stat.Int() == KErrNone)
		{
		TBuf<14> addressText;
		pckg().BDAddr().GetReadable(addressText, _L("0x"), KNullDesC, KNullDesC);
		CEikonEnv::InfoWinL(_L("Pin Input Complete:"),addressText);
		}
	else
		{
		CEikonEnv::InfoWinL(_L("Pin Input Complete:"),_L("CANCELLED"));
		}
	}

//
// CTBTNotifiersEikDocument
//

CEikAppUi* CTBTNotifiersEikDocument::CreateAppUiL()
	{
	return(new(ELeave) CTBTNotifiersAppUi);
	}

void CTBTNotifiersEikDocument::ConstructL()
	{
	}

//
// CTBTNotifiersApplication
//

TUid CTBTNotifiersApplication::AppDllUid() const
	{
	return KUidTBTNotifiersApp;
	}

CApaDocument* CTBTNotifiersApplication::CreateDocumentL()
	{
	CTBTNotifiersEikDocument* document=new(ELeave) CTBTNotifiersEikDocument(*this);
	CleanupStack::PushL(document);
	document->ConstructL();
	CleanupStack::Pop();
	return(document);
	}

CApaApplication* NewApplication()
	{
	return new CTBTNotifiersApplication;
	}


#include <eikstart.h>

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}
