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
// DPACKETLOG.H
// 
//

/**
 @file
 @internalComponent 
*/

#define __LOGGER__

#ifdef __LOGGER__
#define LOGTEXT(AAA)	CDTSYLogger::Write(AAA)
#else
#define LOGTEXT(AAA)
#endif

#ifdef __LOGGER__
#define LOGTEXT2(AAA,BBB)	CDTSYLogger::WriteFormat(TRefByValue<const TDesC8>(AAA),BBB)
#else
#define LOGTEXT2(AAA,BBB)
#endif

#ifdef __LOGGER__
#define LOGTEXT3(AAA,BBB,CCC)	CDTSYLogger::WriteFormat(TRefByValue<const TDesC8>(AAA),BBB,CCC)
#else
#define LOGTEXT3(AAA,BBB,CCC)
#endif

#define LOGTEXTREL(AAA)
#define LOGTEXTREL2(AAA,BBB)
#define LOGTEXTREL3(AAA,BBB,CCC)


#ifdef __LOGGER__
#define LOGDESTROY()	CDTSYLogger::Destruct()
#else
#define LOGDESTROY()
#endif


#ifndef __SLOGGER_H
#define __SLOGGER_H

#ifdef __LOGGER__
#include <f32file.h>

class CDTSYLogger : public CBase
/**
@internalComponent
*/
	{
public:
	static CDTSYLogger* NewL();
	CDTSYLogger();
	void ConstructL();
	static void Destruct();
	~CDTSYLogger();
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
