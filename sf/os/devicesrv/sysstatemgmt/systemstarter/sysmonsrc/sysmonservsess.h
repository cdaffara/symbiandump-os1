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
//

/**
 @file
 @internalComponent 
*/

#ifndef __SYSMONSERVSESS_H__
#define __SYSMONSERVSESS_H__

#include <e32std.h>
#include <e32base.h>
#include <e32cmn.h>

class CSysMonServer;

/*
CSysMonSession maintains session with the client
*/
class CSysMonSession: public CSession2
	{
public:
	static CSysMonSession* NewL();
	void ServiceL(const RMessage2 &aMessage);
	void ServiceError(const RMessage2 &aMessage, TInt aError);
	~CSysMonSession();

private:
	CSysMonSession();
	void DoMonitorL(TBool aMonitorSelf);
	void CancelMonitorSelfL() const;
	CSysMonServer* SysMonServer() const;

private:
	RMessage2 iMessage;
	};

#endif // __SYSMONSERVSESS_H__
