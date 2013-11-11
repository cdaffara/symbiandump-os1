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
// clientside.h
// client side exmaple of how to use the abstract server framework
// 
//

#include <e32base.h>
#include <e32debug.h>
#include "clientside.h"
#include "examplemessageenums.h"
#include "cquietus.h"
#include "lbsdevloggermacros.h"

RExTimeServer::RExTimeServer()
	{
	// nothing to do
	}
 	
	
RExTimeServer::~RExTimeServer()
	{
	// nothing to do
	}

TVersion RExTimeServer::Version()
	{
	return TVersion(0,0,2); // now suports multiple personality sessions :)
	}

//------------------------------------------------------------	

RExTimeSession::RExTimeSession()
	{
	// nothing to do
	}
	
RExTimeSession::~RExTimeSession()
	{
	// nothing to do
	}
	
	
TInt RExTimeSession::GetTimeSync(TDateTime& aDateTime)
	{
	TInt err;
	TPckg<TDateTime> buf(aDateTime);
	TIpcArgs args(&buf);
	err = SendReceive(EExTimeServerGetTimeSync, args);
	return err;
	}

void RExTimeSession::GetTimeAsync(TDateTime& aDateTime, TRequestStatus& aTrs)
	{
	//TPckg<TDateTime> buf(aDateTime);
	iBuf = aDateTime;
	TDateTime temp = iBuf();
	TIpcArgs args(&iBuf);
	SendReceive(EExTimeServerGetTimeAsync, args, aTrs);
	}
	
TInt RExTimeSession::CancelGetTime()
	{
	TInt err;
	TIpcArgs args(TIpcArgs::ENothing);
	err = SendReceive(EExTimeServerCancelGetTime, args);
	return err;	
	}

TInt RExTimeSession::InvalidMessage()
	{
	TInt err;
	TIpcArgs args(TIpcArgs::ENothing);
	const TInt EExInVaildMessage = 12345; // this is not a valid message ID
	err = SendReceive(EExInVaildMessage, args);
	return err;	// will be KErrNotSupported
	}

RExServerNameGetter::RExServerNameGetter()
	{
	// nothing to do
	}
	
RExServerNameGetter::~RExServerNameGetter()
	{
	// nothing to do
	}
		
TInt RExServerNameGetter::GetServerName(TName& aShortName, TFullName& aFullName)
	{
	TName shortName;
	TFullName fullName;
	TIpcArgs args(&shortName, &fullName);
	TInt err  = SendReceive(EExTimeServerGetServerName, args);
	aShortName = shortName;
	aFullName = fullName;
	return err;
	}

//-----------------------------------------------------------


CGetTimeObs::CGetTimeObs(MGetTimeObs* aObs, RExTimeSession& aSession)
	: CActive(CActive::EPriorityStandard), iObs(aObs), iSession(aSession)
	{
	CActiveScheduler::Add(this);
	}

CGetTimeObs::~CGetTimeObs()
	{
	
	}

void CGetTimeObs::ConstructL()
	{
	}
	
void CGetTimeObs::GetTime()
	{
	iSession.GetTimeAsync(iStoredDateTime, iStatus);
	SetActive();
	}
	
void CGetTimeObs::CancelGetTime()
	{
	Cancel();
	}
	
void CGetTimeObs::RunL()
	{
	if(iStatus.Int()==KErrNone)
		{
		iStoredDateTime = iSession.iBuf();
		iObs->TimeUpdate(iStoredDateTime);
		}
	if(iStatus.Int()==KErrCancel)
		{
		// request was canceled
		}
	}
	
void CGetTimeObs::DoCancel()
	{
	iSession.CancelGetTime();
	}
	


//-----------------------------------------------------------


_LIT(KServerFileName, "\\sys\\bin\\ExFrameworkServer.exe");
_LIT(KServerName, "ExFrameworkServer");


CExClient::CExClient()
	{
	
	}
	
CExClient::~CExClient()
	{
	if(iQuietus)
		{
		delete iQuietus;
		}
	iTimeObs->CancelGetTime();
	delete iTimeObs;
	iTimeObs = NULL;
	iNameSession.Close();
	iTimeSession.Close();
	iServer.Close();
	}
	
void CExClient::ConstructL()
	{
	TUid uid;
	uid = TUid::Uid(0x10281D6B); // From the servers MMP file
	
	// server launch paramaters
	iParams.SetServerFileName(KServerFileName);
	iParams.SetServerName(KServerName);
	iParams.SetNumberOfServerSlots(KServerFrameworkDefaultMessageSlots);
	//iParams.SetSecurityPolicy(myPolicy);
	iParams.SetServerUids(KNullUid, KNullUid, uid);

	// dummy session parameters for the time session
	iOpenSessionParams.iDummyNumberParam = 12345;
	iOpenSessionParams.iDummyBufParam = KServerName;
	
	// dummy session paramaters for the server name session
	TServerNameOpenSessionParams serverNameParams = 6789;
	
	TSubSessionType timeSubSession(TSubSessionType::EExTimeSubSession);
	TSubSessionType nameSubSession(TSubSessionType::EExServerNameSubSession);
	
	
	// connect to the server
	TInt err;
	err = iServer.Connect(iParams);
	User::LeaveIfError(err);
	// open a time sub session
	err = iTimeSession.Open<TSubSessionType, TTimeOpenSessionParams>(iServer, timeSubSession, iOpenSessionParams);
	User::LeaveIfError(err);	
	// open a name sub session
	err = iNameSession.Open<TSubSessionType, TServerNameOpenSessionParams>(iServer, nameSubSession, serverNameParams);
	User::LeaveIfError(err);
	
	// async time observer	
	iTimeObs = new(ELeave) CGetTimeObs(this, iTimeSession);	
	iTimeObs->ConstructL();

	}


void CExClient::ServerConnectCloseL()
	{
	LBSLOG(ELogP1,"->CExClient::ServerConnectCloseL");
	TInt err = KErrNone;
	RExTimeServer server;
	err = server.Connect(iParams);
	User::LeaveIfError(err);
	server.Close();	
	LBSLOG(ELogP1,"<-CExClient::ServerConnectCloseL");
	}


void CExClient::SessionOpenCloseL()
	{
	LBSLOG(ELogP1,"->CExClient::SessionOpenCloseL");
	TInt err = KErrNone;
	RExTimeServer server;
	err = server.Connect(iParams);
	User::LeaveIfError(err);
	RExTimeSession session;
	
	TSubSessionType timeSubSession(TSubSessionType::EExTimeSubSession);
	err = session.Open<TSubSessionType, TTimeOpenSessionParams>(iServer, timeSubSession, iOpenSessionParams);

	User::LeaveIfError(err);	
	session.Close();
	server.Close();	
	LBSLOG(ELogP1,"<-CExClient::SessionOpenCloseL");
	}
 
void CExClient::SessionMultiConnectCloseL()
	{
	LBSLOG(ELogP1,"->CExClient::SessionMultiConnectCloseL");
	RExTimeServer server1;
	RExTimeServer server2;
	TInt err = KErrNone;
	err = server1.Connect(iParams);
	User::LeaveIfError(err);
	err = server2.Connect(iParams);
	User::LeaveIfError(err);
	server1.Close();
	server2.Close();	
	LBSLOG(ELogP1,"<-CExClient::SessionMultiConnectCloseL");	
	}
	
void CExClient::SessionMultiOpenCloseL()
	{
	LBSLOG(ELogP1,"->SessionMultiOpenCloseL");
	TInt err = KErrNone;
	err = iServer.Connect(iParams);
	User::LeaveIfError(err);
	RExTimeSession session1;
	RExTimeSession session2;
	TSubSessionType timeSubSession(TSubSessionType::EExTimeSubSession);
	err = session1.Open<TSubSessionType, TTimeOpenSessionParams>(iServer, timeSubSession, iOpenSessionParams);
	User::LeaveIfError(err);
	err = session2.Open<TSubSessionType, TTimeOpenSessionParams>(iServer, timeSubSession, iOpenSessionParams);
	User::LeaveIfError(err);		
	session1.Close();
	session2.Close();
	iServer.Close();	
	LBSLOG(ELogP1,"<-SessionMultiOpenCloseL");
	}
	
void CExClient::SessionMutiConnectOpenCloseL()
	{
	LBSLOG(ELogP1,"->SessionMutiConnectOpenCloseL");
	TInt err = KErrNone;
	RExTimeServer server1;
	RExTimeServer server2;
	err = server1.Connect(iParams);
	User::LeaveIfError(err);
	err = server2.Connect(iParams);
	User::LeaveIfError(err);
	RExTimeSession session1;
	RExTimeSession session2;
	TSubSessionType timeSubSession(TSubSessionType::EExTimeSubSession);
	err = session1.Open<TSubSessionType, TTimeOpenSessionParams>(server1, timeSubSession, iOpenSessionParams);
	User::LeaveIfError(err);
	err = session2.Open<TSubSessionType, TTimeOpenSessionParams>(server2, timeSubSession, iOpenSessionParams);
	User::LeaveIfError(err);		
	session1.Close();
	session2.Close();
	server1.Close();
	server2.Close();	
	LBSLOG(ELogP1,"<-SessionMutiConnectOpenCloseL");
	}

// call a method on the session	
void CExClient::GetTimeSync()
	{
	TDateTime dateTime;
	TInt err = iTimeSession.GetTimeSync(dateTime);
	}
	
void CExClient::GetTimeAsync()
	{
	// delagate
	iTimeObs->GetTime();	
	}

void CExClient::CancelGetTime()
	{
	if(iTimeObs->IsActive())
		{
		iTimeObs->CancelGetTime();
		}
	} 
	
void CExClient::TimeUpdate(TDateTime& aDateTime)
	{
	iNewDateTime = aDateTime;
	// look at iTime now :)
	CActiveScheduler::Stop(); // stop our AS
	}
	
void CExClient::InvalidMessage()
	{
	TInt err = iTimeSession.InvalidMessage();
	}
	
TInt CExClient::GetNames()
	{
	TName shortName;
	TFullName longName;
	TInt err = iNameSession.GetServerName(shortName, longName);
	// look at shortName & long naem in the debugger here
	return err;
	}
	
void CExClient::ConstructQuietusL()
	{
	iQuietus = CQuietus::NewL();
	}
	
void CExClient::CoupDeGrace()
	{
	iQuietus->CoupDeGrace();
	}
	
