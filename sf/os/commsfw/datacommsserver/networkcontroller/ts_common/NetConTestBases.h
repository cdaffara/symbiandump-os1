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

#if !(defined __NETCONTESTBASES_H__)
#define __NETCONTESTBASES_H__

#include <e32base.h>

#ifdef _DEBUG
#include "NetConLog.h"
#undef ASSERT
/** @todo there must be a better way to output __FILE__ */
#define ASSERT(COND) { TBuf8<KMaxFileName> narrowFileName; narrowFileName.Copy((TText8*)__FILE__); TFileName wideFileName; wideFileName.Copy(narrowFileName); if(!(COND)) { NetConLog::Printf(_L("Assertion failed in %S at line %d"), &wideFileName, __LINE__); User::Invariant(); } }
#endif

//
//  The interface for all netcon tests
//
class MNetConTest
	{
public:
	enum TResult
		{
		EUnknown,
		EPassed,
		EFailed
		};

	virtual void Start() = 0;
	virtual TResult Result() const = 0;
	virtual const TDesC& Name() const = 0;
	};

class CNetConTestBase : public CActive, public MNetConTest
	{
public:
	virtual ~CNetConTestBase();

	// MNetConTest
	virtual void Start();
	virtual TResult Result() const;

protected:
	CNetConTestBase();

	void CompleteTest(TResult aResult);
	
	// pure virtual
	virtual void StartTestL() = 0;

private:
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();

	void StopTest();

private:
	enum TNetConTestAction
		{
		ENone,
		EStart,
		EStop
		};

	enum TNetConTestState
		{
		EInitialised,
		EStarting,
		EComplete
		};

private:
	TResult iResult;
	TNetConTestAction iAction;
	TNetConTestState iState;
	};


#endif // __NETCONTESTBASES_H__

