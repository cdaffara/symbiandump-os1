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

#if !defined(SHUTDOWNTIMER_H)
#define SHUTDOWNTIMER_H

#include <e32base.h>

class CServShutdownServer;
/**
This class describes a timer object. 
Its RunL() implemantation calls CServShutdownServer::SwitchOff() to execute the switch off 
when timeout expires.
@internalTechnology
*/
NONSHARABLE_CLASS(CShutdownTimer): public CTimer
	{
public:
	static CShutdownTimer* NewL(CServShutdownServer& aShtdwnSrv);
	void DoCancel();
	void Start(TInt aNumClients);
protected:
	virtual void RunL();
private:
	CShutdownTimer(CServShutdownServer& aShtdwnSrv);
private:
	CServShutdownServer& iShtdwnSrv;
	};
#endif//SHUTDOWNTIMER_H
