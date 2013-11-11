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
 @internalComponent - Internal Symbian test code
*/

#ifndef __SSMCLETESTAPP_H
#define __SSMCLETESTAPP_H

#include "ssmtestapps.h"

const TInt KSsmCleTestPanic = 999;
enum TCleTestFailHow
	{
	EDontFail,
	EPanic,
	ENoRendezvous,
	EBadRendezvous,
	EBadPath,
	EMultipleTimeout
	};

struct TSsmCleTestAppArgs
	{
	TBuf<KTestCommandLineMaxLength> iLogPrefix;
	TInt iWaitTime;
	TInt iSucceedOnRun;
	TCleTestFailHow iFailHow;
	};

class XSsmCleTestApp
	{
public:
	static void WriteStartTimeL(RFs aFs, const TDesC& aLogPrefix);
	static TInt GetRunCountL(RFs aFs, const TDesC& aLogPrefix);
	static TInt IncrementRunCountL(RFs aFs, const TDesC& aLogPrefix);
	static void GetCommandLineArgsL(TSsmCleTestAppArgs& aArgStruct);
	static void WriteResultL(RFs aFs, const TDesC& aLogPrefix,const TCleTestFailHow& aFailHow);
	};
	
#endif // __SSMCLETESTAPP_H
