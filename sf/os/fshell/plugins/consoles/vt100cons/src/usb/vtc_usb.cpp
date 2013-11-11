// vtc_usb.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32std.h>
#include <fshell/consoleextensions.h>
#include "vtc_usb.h"
#include <usbclassuids.h>

EXPORT_C TAny* NewConsole()
	{
	return new CUsbConsole;
	}
	
TUsbPortConfig::TUsbPortConfig()
	:iPort(KNullDesC), iPersonality(-1)
	{
	}

#define ErrOrDebug(err) (((err) < 0  && (err) != KErrAlreadyExists) ? EError : EDebug)


//______________________________________________________________________________
//						CUsbConsole
CUsbConsole::CUsbConsole()
	{
	}

CUsbConsole::~CUsbConsole()
	{
	iUsb.Close();
	}
	
void CUsbConsole::ConstructL(const TDesC& aTitle)
	{
	TUsbPortConfig portConfig;
	User::LeaveIfError(ReadConfig(aTitle, portConfig));

	User::LeaveIfError(iUsb.Connect());
	
	TRequestStatus stat;
	if (portConfig.iPersonality >= 0)
		{
		iUsb.TryStart(portConfig.iPersonality, stat);
		User::WaitForRequest(stat);
		User::After(500000); // Ugh need to wait for the TryStart to finish. There's probably some other async notification I could wait on but I'm in a hurry to get this working...
		Message(ErrOrDebug(stat.Int()), _L("Starting USB personality %d returned %d"), portConfig.iPersonality, stat.Int());
		}

	// Wait for an enumeration that supports ACM (this is so that if the device defaulted to say mass storage and was then reconfigured to a personality with ACM, we wait for the ACM reconfiguration
	TBool gotAcm = EFalse;
	while (!gotAcm)
		{
		TUsbDeviceState usbState;
		User::LeaveIfError(iUsb.GetDeviceState(usbState));
		if (usbState & (EUsbDeviceStateConfigured|EUsbDeviceStatePowered)) // We should only need to check for EUsbDeviceStateConfigured, but some HW doesn't go to configured state if the cable is inserted before USB is started
			{
			// Check if we have ACM
			TInt currentPersonality;
			TInt err = iUsb.GetCurrentPersonalityId(currentPersonality);
			if (!err)
				{
				err = iUsb.ClassSupported(currentPersonality, KECACMUid, gotAcm);
				}
			_LIT(KGotIt, "Current USB personality has ACM, proceeding");
			_LIT(KNotGotIt, "Current USB personality doesn't have ACM, waiting for re-enumeration");
			if (gotAcm) Message(EInformation, KGotIt);
			else Message(EInformation, KNotGotIt);
			}

		if (!gotAcm)
			{
			// We're not enumerated, or we are but don't have ACM. So wait for a (re-)enumeration
			_LIT(KWaitingForEnumeration, "Waiting for USB enumeration (please connect USB cable)");
			Message(EInformation, KWaitingForEnumeration);
			iUsb.DeviceStateNotification(EUsbDeviceStateConfigured|EUsbDeviceStatePowered, usbState, stat);
			User::WaitForRequest(stat);
			if (stat.Int() != KErrNone)
				{
				_LIT(KUsbError, "Error configuring USB: %d");
				Message(EError, KUsbError, stat.Int());
				User::Leave(stat.Int());
				}
			_LIT(KUsbEnumerated, "USB state changed to %d.");
			Message(EInformation, KUsbEnumerated, usbState);
			}
		}
	
	// Run the preamble script, if we have one. Because iosrv is multithreaded this shouldn't cause a deadlock so long as we use a different console (in this case, nullcons)
	_LIT(KPreamble, "--console nullcons vt100usbcons_preamble");
	RProcess preamble;
	TInt err = preamble.Create(_L("fshell.exe"), KPreamble);
	if (err == KErrNone)
		{
		preamble.Logon(stat);
		if (stat == KRequestPending)
			{
			preamble.Resume();
			User::WaitForRequest(stat);
			}
		err = stat.Int();
		preamble.Close();
		}

	if (err == KErrNone)
		{
		Message(EInformation, _L("Preamble script ran ok"));
		}
	else if (err != KErrNotFound)
		{
		Message(EInformation, _L("Preamble script failed with %d"), err);
		}

	Message(EInformation, _L("Opening %S"), &portConfig.iPort);
	CVtcSerialConsole::ConstructL(portConfig.iPort);
	}

TInt CUsbConsole::ReadConfig(const TDesC& aConfigDes, TUsbPortConfig& aConfig)
	{
	_LIT(KKeywordPort, "port");
	_LIT(KKeywordPersonality, "personality");

	TBool keywordFound(EFalse);
	TLex lex(aConfigDes);
	while (!lex.Eos())
		{
		TPtrC keyword;
		TPtrC value;
		TInt err = ReadKeywordValuePair(lex, keyword, value);
		if (err != KErrNone)
			{
			break;
			}

		if (keyword == KKeywordPort)
			{
			aConfig.iPort.Set(value);
			keywordFound = ETrue;
			}
		else if (keyword == KKeywordPersonality)
			{
			TLex lex(value);
			TInt err = lex.Val(aConfig.iPersonality);
			if (err) return err;
			keywordFound = ETrue;
			}
		}

	if (!keywordFound)
		{
		// Treat unrecognised string as a port (to preserve backwards compatibility with earlier releases).
		aConfig.iPort.Set(aConfigDes);
		}

	return KErrNone;
	}
