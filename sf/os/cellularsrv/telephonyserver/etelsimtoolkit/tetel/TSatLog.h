// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CTestLogger header file
// 
//

/**
 @file
 @internalComponent
*/

#define __LOGGER__

#ifdef __LOGGER__
#define LOGTEXT(AAA)	CTestLogger::Write(AAA)
#else
#define LOGTEXT(AAA)
#endif

#ifdef __LOGGER__
#define LOGTEXT2(AAA,BBB)	CTestLogger::WriteFormat(TRefByValue<const TDesC8>(AAA),BBB)
#else
#define LOGTEXT2(AAA,BBB)
#endif

#ifdef __LOGGER__
#define LOGTEXT3(AAA,BBB,CCC)	CTestLogger::WriteFormat(TRefByValue<const TDesC8>(AAA),BBB,CCC)
#else
#define LOGTEXT3(AAA,BBB,CCC)
#endif

#ifdef __LOGGER__
#define LOGTEXT4(AAA,BBB,CCC,DDD)	CTestLogger::WriteFormat(TRefByValue<const TDesC8>(AAA),BBB,CCC,DDD)
#else
#define LOGTEXT4(AAA,BBB,CCC,DDD)
#endif

#ifdef __LOGGER__
#define LOGDESTROY()	CTestLogger::Destruct()
#else
#define LOGDESTROY()
#endif

#ifndef __SLOGGER_H
#define __SLOGGER_H

#ifdef __LOGGER__
#include <f32file.h>

class CTestLogger: public CBase
	{
public:
	static CTestLogger* NewL();
	CTestLogger();
	void ConstructL();
	static void Destruct();
	~CTestLogger();
	static void Write(const TDesC8& aText);
	static void WriteFormat(TRefByValue<const TDesC8> aFmt,...);
	void WriteRecord(const TDesC8& aText);
private:
	TBool iValid;
	RFs iFs;
	RFile iFile;
	};

#endif	// __LOGGER__

#endif	// __SLOGGER_H
