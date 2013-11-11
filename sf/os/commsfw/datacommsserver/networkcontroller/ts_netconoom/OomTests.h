// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__OOMTESTS_H__)
#define __OOMTESTS_H__

#include "TestBases.h"

//
//  Test Case NC-5-1
//

class CNetConTest0501 : public COomTestBase
	{
public:
	virtual ~CNetConTest0501();
	static CNetConTest0501* NewLC();

	// from MNetConTestBase
	virtual const TDesC& Name() const;

	// override functions from COomTestBase
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);

protected:
	CNetConTest0501();

	// from COomTestBase
	virtual void StartTestL();

private:
	void CleanupAgent();
	};

//
//  Test Case NC-5-2
//

class CStubbedAgent;
class CCommsDbAccess;
class CNetConTest0502 : public COomTestBase
	{
public:
	virtual ~CNetConTest0502();
	static CNetConTest0502* NewLC();

	// from MNetConTestBase
	virtual const TDesC& Name() const;

	// override functions from COomTestBase
	virtual void ReconnectComplete(TInt aError);

protected:
	CNetConTest0502();
	void ConstructL();

	// from COomTestBase
	virtual void StartTestL();

private:
	CStubbedAgent* iAgent;
	CCommsDbAccess* iDatabase;
	};

#endif // __OOMTESTS_H__

