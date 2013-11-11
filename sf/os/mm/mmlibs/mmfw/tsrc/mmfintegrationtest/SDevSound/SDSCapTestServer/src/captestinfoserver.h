/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file captestinfoserver.h
*/
#ifndef __CAPTESTINFOSERVER_H__
#define __CAPTESTINFOSERVER_H__

#include <e32base.h>

_LIT(KInfoServerName,"SDSCapTestInfoServer");

// Function codes (opcodes) used in message passing between client and server
enum TCapTestInfoRequest
    {
    ECapTestInfoThreadId = 1,
    };

class CCapTestInfoServer : public CServer2
	{
public:
	static CCapTestInfoServer* NewL();
	CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
	~CCapTestInfoServer();
	
private:
	CCapTestInfoServer();

private:
	TBool iSessionConnected;
	};

class CCapTestInfoServerSession : public CSession2
	{
public:
	static CCapTestInfoServerSession* NewL();
	~CCapTestInfoServerSession();
	void ServiceL(const RMessage2 &aMessage);

private:
	CCapTestInfoServerSession();
	void DoGetThreadIdL(const RMessage2 &aMessage);
	};

#endif

