// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent
*/

#ifndef UTILS_H
#define UTILS_H

#include <e32math.h>
#include <w32std.h>

/*******************************************************************************
 This is a wrapper round the horrific Math::Rand interface
 *******************************************************************************/

const TInt KPosLimit = 350;

_LIT(KLogFileName, "logging.log"); //to be used to replay the same testrun
_LIT(KAnimDllName, "a0005924.dll");

class CTestExecWatchCat;

class CExecutionContext : public CBase
	{
	friend class CTestExecWatchCat;
	
public:
	class CLogMediator : public CBase
		{
	public:
		TInt PlaybackInt ();
		void RecordInt (TInt aIntToBeRecorded);
		static CLogMediator* NewLC (CExecutionContext& aExecutionContext, RFs& aFs,	const TDesC& aFileName);
		~CLogMediator ();
	
	private:
		CLogMediator (CExecutionContext& aExecutionContext);
		void ConstructL (RFs& aFs, const TDesC& aFileName);
	
	private:
		RFile iLog;
		CExecutionContext& iExecutionContext;
		TInt64 iEntryNo;
		TBool iStalled;
		};
	
	enum TExecutionMode
		{
		ECtxRandom,
		ECtxRandomAndRecord,
		ECtxPlayback
		};

	static CExecutionContext* NewL (TExecutionMode aExecutionMode, CTestExecWatchCat& aWatchCat);
	TExecutionMode ContextMode ();
	~CExecutionContext ();
	void MediatorDestroyed (CLogMediator* aMediator);
	void MediatorEmptied (CLogMediator* aMediator);

private:
	CExecutionContext (TExecutionMode aExecutionMode, CTestExecWatchCat& aWatchCat);
	void ConstructL ();
	CLogMediator& CreateLogMediatorL (const TDesC& aLogName);

private:
	RFs iFs;
	RPointerArray<CLogMediator> iMediators;
	CTestExecWatchCat& iWatchCat;
	TExecutionMode iContextMode;
	};

class CTestExecWatchCat : public CBase
	{
	friend class CExecutionContext;
	
public:
	static CTestExecWatchCat* NewL (CExecutionContext::TExecutionMode aExecutionMode);
	void SetLoggingPathL(const TDesC& aPath);

private:
	CTestExecWatchCat ( );
	~CTestExecWatchCat ( );

	void ConstructL (CExecutionContext::TExecutionMode aExecutionMode );
	void ExecutionContextRunOut (CExecutionContext* aContext );

private:
	CExecutionContext* iExecutionContext;
	};

class TRnd
	{
public:
	static TInt rnd ( TInt x = 0 );
	static TInt64 Seed();
	static void SetSeed(TInt64 aSeed);
	static void SetLogMediator(CExecutionContext::CLogMediator* aMediator);
	static void SetExecutionContext(CExecutionContext* aExecutionContext);
private:
	static TInt GenRand (TInt x );

private:
	static CExecutionContext::CLogMediator* iMediator;
	static CExecutionContext* iExecutionContext;
	static TInt64 iSeed;
	
	friend class CTestExecWatchCat;
	};

class TTickUtils
	{
public:
	static TUint32 CalcTickDelta (TUint32 tick1, TUint32 tick2 );

	};

/*******************************************************************************
 This is a small helper function for drawing regions
 *******************************************************************************/
static void DrawRegion (const TRegion& aRegion, CGraphicsContext& aGc)
	{
	const TRect * rect = aRegion.RectangleList ();
	for (TInt r = 0; r < aRegion.Count (); ++r)
		{
		aGc.DrawRect (*rect);
		++rect;
		}
	}

#endif // UTILS_H
