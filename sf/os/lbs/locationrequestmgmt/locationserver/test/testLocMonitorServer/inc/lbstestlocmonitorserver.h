/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __LBSTESTLOCMONITORSERVER_H__
#define __LBSTESTLOCMONITORSERVER_H__

#include <e32base.h>
#include <lbsipc.h>
#include <e32std.h>
#include <e32svr.h>

// needed for creating server thread.
const TUint KDefaultHeapSize = 0x100000;

_LIT(KLocMonServerName,"testlocmonitorserver");

class CTe_LocMonitorServer : public CServer2
	{
public:

public:
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	//static TInt ThreadFunction(TAny* aStarted);
	static TInt StartLocMonitorServer();
public:
	// New functions
	void AddSession();
	void DropSession();
protected:
	CTe_LocMonitorServer(TInt aPriority);
private:
	static void StartLocMonitorServerL();
private:
	TInt iActive;
	TInt iSessionCount;

	};

#endif // __LBSTESTLOCMONITORSERVER_H__
