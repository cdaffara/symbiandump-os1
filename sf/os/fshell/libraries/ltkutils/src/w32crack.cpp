// w32crack.cpp
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

#include <fshell/common.mmh>
#include <fshell/ltkutils.h>
#include <e32base.h>
#include <fshell/iocli.h>
#include <w32std.h>
#include <fshell/ioutils.h>
#include <fshell/memoryaccess.h>

//
// LtkUtils::W32CrackL
// loads the autometric logging engine, dlogev.dll, into wserv
// enabling Autometric to monitor events from within wserv - key/pointer events, text drawn to screen etc
//
EXPORT_C void LtkUtils::W32CrackL()
	{
#ifndef FSHELL_WSERV_SUPPORT
	User::Leave(KErrNotSupported);
#else

	// Check if P&S says it's already enabled, if so no need to do anything
	if (W32CrackIsEnabled()) return;

	_LIT(KWsIniFile, "z:\\system\\data\\wsini.ini");
	_LIT(KCWsIniFile,"c:\\system\\data\\wsini.ini");
	_LIT(KLogEv, "LOG EV\r\n");
	
	// Open z file
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	RFile file;
	User::LeaveIfError(file.Open(fs, KWsIniFile, EFileRead));
	CleanupClosePushL(file);

	TInt size;
	User::LeaveIfError(file.Size(size));

	RBuf8 buf;
	buf.CreateL(size + KLogEv().Size());
	CleanupClosePushL(buf);
	User::LeaveIfError(file.Read(buf));
	TPtrC16 wptr((TUint16*)buf.Ptr(), buf.Size()/2);
	buf.Insert(2, TPtrC8((TUint8*)KLogEv().Ptr(), KLogEv().Size())); // +2 to get past the BOM
	TInt err = KErrNone;
	err = fs.MkDirAll(KCWsIniFile); // mkdir c:\system\data\ in case it is not exist
	if((err != KErrNone) && (err != KErrAlreadyExists))
		{
		User::Leave(err);
		}
	User::LeaveIfError(file.Replace(fs, KCWsIniFile, EFileWrite));
	User::LeaveIfError(file.Write(buf));
	CleanupStack::PopAndDestroy(2, &file); // file, buf
	
	err = RMemoryAccess::LoadDriver();
	if ((err != KErrNone) && (err != KErrAlreadyExists))
		{
		User::Leave(err);
		}
	
	RMemoryAccess memAccess;
	User::LeaveIfError(memAccess.Open());
	CleanupClosePushL(memAccess);
	
	RWsSession ws;
	User::LeaveIfError(ws.Connect(fs));
	CleanupClosePushL(ws);

#ifdef __WINS__
	// On wins the binary isn't renamed
	_LIT(KWservMatch, "wserv*.exe*");
#else
	_LIT(KWservMatch, "EwSrv.exe*");
#endif
	TFindProcess find(KWservMatch);
	TFullName name;
	User::LeaveIfError(find.Next(name));
	
	RProcess wservProc;
	User::LeaveIfError(wservProc.Open(find));
	if (wservProc.ExitType() != EExitPending)
		{
		// in case wserv kicks off the preferred implementation in a new process then kills itself
		// (is one retry enough or should we be looping here?)
		wservProc.Close();
		User::LeaveIfError(find.Next(name));
		User::LeaveIfError(wservProc.Open(find));
		}
	CleanupClosePushL(wservProc);
	name = wservProc.FileName();
	name[0] = 'c';
	TPtrC8 narrowName = name.Collapse();
	User::LeaveIfError(memAccess.InPlaceSetProcessFileName(wservProc, narrowName));

	// Now tell wserv to reload its wsini.ini and enable logging
	ws.LogCommand(RWsSession::ELoggingEnable);
	ws.Flush();
	
	// cleanup
	CleanupStack::PopAndDestroy(4, &fs);

#endif // FSHELL_WSERV_SUPPORT
	}

EXPORT_C TBool LtkUtils::W32CrackIsEnabled()
	{
	TInt enabled = EFalse;
	TInt err = RProperty::Get(KW32CrackCategory, KW32CrackKey, enabled);
	return (err == KErrNone && enabled);
	}
