// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MMRCSERVERSTART_H
#define MMRCSERVERSTART_H

_LIT(KMMRCServerName,"MMRCServer");
_LIT(KMMRCServerFileName,"mmrcserver");

const TUid KMMRCServerUID3 = {0x102021d5};

const TInt KMMRCServerStackSize=0x2000;			//  8KB
const TInt KMMRCServerInitHeapSize=0x1000;		//  4KB
const TInt KMMRCServerMaxHeapSize=0x1000000;		// 16MB

const TUint KMMRCServerVersion=1;
const TUint KMMRCServerMinorVersionNumber=0;
const TUint KMMRCServerBuildVersionNumber=0;

class TServerStart
	{
	public:
	TServerStart() {};
	TServerStart(TRequestStatus& aStatus);
	TPtrC AsCommand() const;
	TInt GetCommand();
	void SignalL();
	private:
	TThreadId iId;
	TRequestStatus* iStatus;
	};

inline TServerStart::TServerStart(TRequestStatus& aStatus)
:iId(RThread().Id()),iStatus(&aStatus)
	{
	aStatus = KRequestPending;
	}

// Descriptorizes ’this’ to pass it from client to server
inline TPtrC TServerStart::AsCommand() const
	{
	return TPtrC(reinterpret_cast<const TText*>(this),sizeof(TServerStart)/sizeof(TText));
	}

void TServerStart::SignalL()
	{
	RThread client;
	User::LeaveIfError(client.Open(iId));
	client.RequestComplete(iStatus, KErrNone);
	client.Close();
	}
#endif //__MMRCSERVERSTART_H

