// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TS_RConnPanic.CPP
// File Created in order to automate Test50, Contains the starting point for the thread that
// will panic on starting a connection
// @file TS_RConnPanic.CPP
// @internalComponent
// 
//


#include "TE_RConnectionStep.h"
#include <commdbconnpref.h>
#include <dummynifvar.h>
#include <c32root.h>
#include <comms-infras/esocktestutils.h>
#include <cs_subconparams.h>
#include <cs_subconevents.h>
#include <ecom/ecom.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestTERCnPn, "ESockTestTERCnPn");
#endif

struct TRConnParams
	{
	TRConnParams(TInt aTimeOut, TInt aEchoPort): 
		iTimeOut(aTimeOut), 
		iEchoPort(aEchoPort)
		{
		}
	TInt iTimeOut;
	TInt iEchoPort;
	};

class PanicTestThread : public CBase
	{
public:
	static void EntryPoint(TRConnParams* aParams)
		{
		
		//ASSERT(aParams);
		CTrapCleanup* cleanup = CTrapCleanup::New();
		CActiveScheduler* Sched = new (ELeave) CActiveScheduler;
		CActiveScheduler::Install(Sched); 
		TRAPD(err, PanicTestThread* self = PanicTestThread::NewLC();
					self->DoItL(aParams->iTimeOut, aParams->iEchoPort);
					CleanupStack::PopAndDestroy(self);
		); 
		__ASSERT_DEBUG(err == KErrNone, User::Panic(KSpecAssert_ESockTestTERCnPn, 1));
		CActiveScheduler::Install(NULL); 
		delete Sched;
		delete cleanup;
		}
		
	static PanicTestThread* NewLC()
		{
		PanicTestThread* self = new (ELeave) PanicTestThread;
		CleanupStack::PushL(self);
		return self;
		}
		
	void DoItL(TInt aTimeOut, TInt aEchoPort)
		{
		TInt err;

		RSocketServ ss1, ss2;
		RConnection conn1, conn2;
		RSocket sock;

		err = ss1.Connect();
		__ASSERT_DEBUG(KErrNone == err, User::Panic(KSpecAssert_ESockTestTERCnPn, 2));
		CleanupClosePushL(ss1);

		err = ss2.Connect();
		__ASSERT_DEBUG(KErrNone == err, User::Panic(KSpecAssert_ESockTestTERCnPn, 3));
		CleanupClosePushL(ss2);

		err = conn1.Open(ss1);
		__ASSERT_DEBUG(KErrNone == err, User::Panic(KSpecAssert_ESockTestTERCnPn, 4));
		CleanupClosePushL(conn1);

		err = conn2.Open(ss2);
		__ASSERT_DEBUG(KErrNone == err, User::Panic(KSpecAssert_ESockTestTERCnPn, 5));
		CleanupClosePushL(conn2);

		// this should error but not panic (connection not started yet)
		err = OpenUdpSocketExplicitL(sock, ss1, conn1, aEchoPort);
		__ASSERT_DEBUG(KErrNotReady == err, User::Panic(KSpecAssert_ESockTestTERCnPn, 6));

		// this should panic because the connection is in a different socket server from that supplied
		err = OpenUdpSocketExplicitL(sock, ss1, conn2, aEchoPort);

		/*
		* Tidy up, although should never reach here
		*/

		conn2.Close();
		CleanupStack::Pop();

		conn1.Close();
		CleanupStack::Pop();

		ss2.Close();
		CleanupStack::Pop();

		ss1.Close();
		CleanupStack::Pop();

		User::After(aTimeOut);
		}
private: 
		
	TInt OpenUdpSocketExplicitL(RSocket& sock, RSocketServ& ss, RConnection& conn, TInt aEchoPortNum)
		{
		TInt err;
		TInt ret = KErrNone;
		err = sock.Open(ss, KAfInet, KSockDatagram, KProtocolInetUdp, conn);
		if(err!= KErrNone)
			{
			return err;
			}

		err = sock.SetOpt(KSoReuseAddr, KSolInetIp, 1); 
		if(err!= KErrNone)
			{
			return err;
			}
			
		err = sock.SetLocalPort(aEchoPortNum); 
		if(err!= KErrNone)
			{
			return err;
			}
		return ret;
		}
		
	};

