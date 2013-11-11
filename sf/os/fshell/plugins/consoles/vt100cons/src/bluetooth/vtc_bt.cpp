// vtc_bt.cpp
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
#include "vtc_bt.h"


EXPORT_C TAny* NewConsole()
	{
	return new CBtConsole;
	}
	
//______________________________________________________________________________
//						CBtConsole
CBtConsole::CBtConsole()
	{
	}

CBtConsole::~CBtConsole()
	{
	iBtConnection.Close();
	}

_LIT(KWaitingForBt, "Waiting for Bluetooth connection");
_LIT(KConnectionErrorFmt, "Bluetooth connection error - %d");
_LIT(KConnectedNameAddrFmt, "BT device '%S' (%02x:%02x:%02x:%02x:%02x:%02x) connected");
_LIT(KConnectedAddrFmt, "BT device %02x:%02x:%02x:%02x:%02x:%02x connected");

void CBtConsole::ConstructL(const TDesC& aTitle)
	{
	// Run the preamble script, if we have one. Because iosrv is multithreaded this shouldn't cause a deadlock so long as we use a different console (in this case, nullcons)
	_LIT(KPreamble, "--console nullcons vt100btcons_preamble");
	RProcess preamble;
	TRequestStatus stat;
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


	User::LeaveIfError(iBtConnection.Connect());
	iBtConnection.WaitForConnection(stat);
	Message(EInformation, KWaitingForBt);
	User::WaitForRequest(stat);
	if (stat.Int() != KErrNone)
		{
		Message(EError, KConnectionErrorFmt, stat.Int());
		User::Leave(stat.Int());
		}
		
	TBTDevAddr addr;
	User::LeaveIfError(iBtConnection.GetConnectedDeviceAddr(addr));
	TName name;
	
	TInt nerr = iBtConnection.GetConnectedDeviceName(name);
	if (nerr == KErrOverflow) nerr = KErrNone;
	
	if (nerr == KErrNone)
		{
		Message(EInformation, KConnectedNameAddrFmt, &name, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
		}
	else
		{
		Message(EInformation, KConnectedAddrFmt, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
		}
	
	CVtcConsoleBase::ConstructL(aTitle);
	}
	
TInt CBtConsole::Output(const TDesC8& aDes)
	{
	TRequestStatus stat;
	iBtConnection.Write(aDes, stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

void CBtConsole::Input(TDes8& aDes, TRequestStatus& aStatus)
	{
	iBtConnection.Read(aDes, aStatus);
	}

void CBtConsole::CancelInput(TRequestStatus&)
	{
	iBtConnection.CancelRead();
	}
