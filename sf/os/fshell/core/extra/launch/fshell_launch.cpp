// fshell_launch.cpp
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
#include <e32base.h>

#include <fshell/common.mmh>

void RunL()
	{
	_LIT(KProcess, "fshell.exe");
	RProcess process;
#ifdef FSHELL_CORE_LAUNCH_TCP
	_LIT(KTcpArgs, "--console vt100tcpcons --console-title port=8080");
	User::LeaveIfError(process.Create(KProcess, KTcpArgs));
#endif

#ifdef FSHELL_CORE_LAUNCH_BT
	_LIT(KBtArgs, "--console vt100btcons");
	User::LeaveIfError(process.Create(KProcess, KBtArgs));
#endif

//#define LQUOTE(x) L ## #x
#define LCONS(x) L ## x
#define EXPAND(f, x) f(x)

#ifdef FSHELL_CORE_LAUNCH_USB
	// FSHELL_CORE_SUPPORT_USB_ICON is "ACM::3" or whatever
	_LIT(KUsbArgs, "--console vt100usbcons --console-title '" EXPAND(LCONS, FSHELL_CORE_SUPPORT_USB_ICON) L"'");
	User::LeaveIfError(process.Create(KProcess, KUsbArgs));
#endif

#ifdef FSHELL_CORE_LAUNCH_RCONS
	_LIT(KRconsArgs, "--console rcons");
	User::LeaveIfError(process.Create(KProcess, KRconsArgs));
#endif

#ifdef FSHELL_CORE_LAUNCH_SERIAL
	// FSHELL_CORE_SUPPORT_SERIAL_ICON is "COMM::1" or whatever
	_LIT(KSerialArgs, "--console vt100cons --console-title '" EXPAND(LCONS, FSHELL_CORE_SUPPORT_SERIAL_ICON) L"'");
	User::LeaveIfError(process.Create(KProcess, KSerialArgs));
#endif

	process.Resume();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TInt err = KErrNoMemory;
	CActiveScheduler* sheduler = new CActiveScheduler;
	if (sheduler)
		{
		CActiveScheduler::Install(sheduler);
		CTrapCleanup* cleanup = CTrapCleanup::New();
		if (cleanup)
			{
			TRAP(err, RunL());
			delete cleanup;
			}
		delete sheduler;
		}
	__UHEAP_MARKEND;
	return err;
	}

