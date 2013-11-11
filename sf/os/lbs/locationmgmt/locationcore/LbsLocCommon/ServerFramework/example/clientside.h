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
// client side exmaple of how to use the abstract server framework
// 
//

#ifndef __CLIENTSIDE_H__
#define __CLIENTSIDE_H__

#include "rsecuresessionbase.h"
#include "tserverstartparams.h"
#include "opensessionparams.h"
#include "cquietus.h"

// this s really a session with server
class RExTimeServer : public RSecureSessionBase
	{
public:
	RExTimeServer();
	~RExTimeServer();
	TVersion Version();
	};	
	
	
// this is really a subsession of the above
class RExTimeSession : public RSecureSubSessionBase
	{
public:
	RExTimeSession();
	~RExTimeSession();
	TVersion Version();
	TInt GetTimeSync(TDateTime& aDateTime);
	void GetTimeAsync(TDateTime& aDateTime, TRequestStatus& aTrs);
	TInt CancelGetTime();
	TInt InvalidMessage();
	// property
	TPckgBuf<TDateTime> iBuf;
	};

// this is another type of session of the above
// the two subsession types do not haev to bear any relation to each other
// apart from both being derived from RSecureSubSessionBase
class RExServerNameGetter : public RSecureSubSessionBase
	{
public:
	RExServerNameGetter();
	~RExServerNameGetter();
	TVersion Version();
	TInt GetServerName(TName& aShortName, TFullName& aFullName);
	
	TName iShortName;
	TFullName iFullName;	
	};

class MGetTimeObs
	{
public:
	virtual void TimeUpdate(TDateTime& aDateTime) = 0;		
	};
		
class CGetTimeObs : public CActive
	{
public:
	CGetTimeObs(MGetTimeObs* aObs, RExTimeSession& aSession);
	~CGetTimeObs();
	void ConstructL();
	void GetTime();
	void CancelGetTime();
protected:
	void RunL();
	void DoCancel();
protected:	
	MGetTimeObs* iObs;
	RExTimeSession iSession;
	TDateTime iStoredDateTime;
	};


class CExClient : public CBase, public MGetTimeObs
	{
public:
	CExClient();
	~CExClient();
	void ConstructL();
	void ServerConnectCloseL();
	void SessionOpenCloseL();
	void SessionMultiConnectCloseL();
	void SessionMultiOpenCloseL();
	void SessionMutiConnectOpenCloseL();
	void GetTimeSync();
	void GetTimeAsync();
	void InvalidMessage();
	TInt GetNames();	
	void ConstructQuietusL();
	void CoupDeGrace();

	
	// from MGetTimeObs
	void TimeUpdate(TDateTime& aDateTime);
	void CancelGetTime();
protected:
	// property
	RExTimeServer 		iServer;
	RExTimeSession 	iTimeSession;
	RExServerNameGetter iNameSession;
	TServerStartParams 	iParams; // MUST live for as long as the client
	TTimeOpenSessionParams	iOpenSessionParams; // must live for a long as the RSecureSessioBase::Open call 
	// for the async command
	CGetTimeObs* iTimeObs;
	TDateTime iNewDateTime;
	CQuietus*	iQuietus;
	};
	
	
#endif // __CLIENTSIDE_H__
