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


#include <e32base.h>


/**
Testserver is launched with commandline parameter defined in testprocgoodsession.h
*/
class CTestServerGood : public CServer2
	{
public:
	static CServer2* NewLC();
	~CTestServerGood();
	
	// from CServer2
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	
private:
	CTestServerGood();
	void ConstructL();
	};

/**
Session to enable interaction with server
*/
class CTestServerGoodSession : public CSession2
	{
public:
	// from CSession2
	void ServiceL(const RMessage2& aMessage);
	};
