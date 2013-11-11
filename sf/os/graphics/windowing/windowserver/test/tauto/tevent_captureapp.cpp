// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Helper application for TEvent test case GRAPHICS-WSERV-0757
//

#include <e32base.h>
#include <e32debug.h>
#include <w32std.h>

const TInt KCapturePriority = 2;

LOCAL_C void MainL()
	{
	RWsSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	RWindowGroup group(session);
	User::LeaveIfError(group.Construct(0));

	// Capture keys EKeyF22 and EKeyF23 with raised
	// priority for each of the three capture types.
	TInt handle1 = User::LeaveIfError(group.CaptureKey(EKeyF22, 0, 0, KCapturePriority));
	TInt handle2 = User::LeaveIfError(group.CaptureKeyUpAndDowns(EStdKeyF22, 0, 0, KCapturePriority));
	TInt handle3 = User::LeaveIfError(group.CaptureLongKey(EKeyF22, EKeyF22, 0, 0, KCapturePriority, ELongCaptureNormal));
	TInt handle4 = User::LeaveIfError(group.CaptureKey(EKeyF23, 0, 0, KCapturePriority));
	TInt handle5 = User::LeaveIfError(group.CaptureKeyUpAndDowns(EStdKeyF23, 0, 0, KCapturePriority));
	TInt handle6 = User::LeaveIfError(group.CaptureLongKey(EKeyF23, EKeyF23, 0, 0, KCapturePriority, ELongCaptureNormal));

	// Complete rendezvous with TEvent. This signals that all the capture
	// requests have been made.
	RProcess::Rendezvous(KErrNone);

	// Wait until killed by TEvent
	User::WaitForAnyRequest();

	group.CancelCaptureKey(handle1);
	group.CancelCaptureKeyUpAndDowns(handle2);
	group.CancelCaptureLongKey(handle3);
	group.CancelCaptureKey(handle4);
	group.CancelCaptureKeyUpAndDowns(handle5);
	group.CancelCaptureLongKey(handle6);

	CleanupStack::PopAndDestroy(&session);
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;

	CTrapCleanup* cleanupStack = CTrapCleanup::New(); 	
	TRAPD(ret, MainL());	
	delete cleanupStack;
	
	__UHEAP_MARKEND;
	return ret;
	}
