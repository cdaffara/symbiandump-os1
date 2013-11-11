// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTSERVER_H__
#define __TESTSERVER_H__

#include <e32base.h>
#include <lbsipc.h>

// needed for creating server thread.
const TUint KDefaultHeapSize = 0x100000;

class CEposServServer : public CServer2
	{
public:
	enum { EPriority = 950 };
public:
	virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	static TInt ThreadFunction(TAny* aStarted);
	static TInt StartEposServer();
public:
	// New functions
	void AddSession();
	void DropSession();
protected:
	CEposServServer(TInt aPriority);
private:
	static void StartEposServerL();
private:
	TInt iActive;
	TInt iSessionCount;
	};

#endif // __TESTSERVER_H__
