// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <fbs.h>
#include "SERVER.H"

// general dll routines for starting the server etc.

GLDEF_C void Panic(TFbsPanic aPanic)
	{
	User::Panic(KFBSERVPanicCategory,aPanic);
	}

GLDEF_C void CheckFbsNotRunning()
	{
	TFindChunk find(KFBSERVSharedChunkName);
	TFullName name;
	__ASSERT_ALWAYS(find.Next(name) == KErrNotFound, Panic(EFbsPanicStartupFailed));
	}

GLDEF_C TInt FbsMain()
	{
    TInt ret;
	ret = User::SetProcessCritical(User::ESystemCritical);
	__ASSERT_ALWAYS(ret==KErrNone,User::Panic(KFBSERVPanicCategory,KErrServerTerminated));
    ret = User::SetCritical(User::ESystemCritical);
	__ASSERT_ALWAYS(ret==KErrNone,User::Panic(KFBSERVPanicCategory,KErrServerTerminated));
    RProcess().SetPriority(EPriorityHigh);
    
	CFbActiveScheduler* as=NULL;
	CFontBitmapServer* fbs=NULL;
	__UHEAP_MARK;
	CTrapCleanup* tc=CTrapCleanup::New();
	__ASSERT_ALWAYS(tc,User::Panic(KFBSERVPanicCategory,KErrServerTerminated));
	// Rename thread
	ret = User::RenameThread(KFBSERVGlobalThreadName);
	__ASSERT_ALWAYS(ret==KErrNone,User::Panic(KFBSERVPanicCategory,KErrServerTerminated));
	TRAP(ret,as=new(ELeave) CFbActiveScheduler);
	__ASSERT_ALWAYS(ret==KErrNone,User::Panic(KFBSERVPanicCategory,KErrServerTerminated));
	CActiveScheduler::Install(as);
	TRAP(ret,fbs=CFontBitmapServer::NewL());
	__ASSERT_ALWAYS(ret==KErrNone,User::Panic(KFBSERVPanicCategory,KErrServerTerminated));
	RProcess::Rendezvous(KErrNone);
	CActiveScheduler::Start();
	delete as;
	delete fbs;
	delete tc;
	__UHEAP_MARKEND;
	return(0);
	}

CFbActiveScheduler::CFbActiveScheduler():
	CActiveScheduler()
	{
	}

void CFbActiveScheduler::Error(TInt /*anError*/) const
	{
	Panic(EFbsPanicActiveSchedulerError);
	}

GLDEF_C TInt E32Main()
	{
	CheckFbsNotRunning();
	return(FbsMain());
	}

