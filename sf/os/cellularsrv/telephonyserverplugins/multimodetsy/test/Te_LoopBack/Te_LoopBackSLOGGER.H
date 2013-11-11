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
// This file contains the class definitions and normal definitions for use by 
// the CTelLogger.
// There are two defines that are used to determine which set of macros are to be 
// used.  First, if DEBUG is defined the LOGTEXTn macros are implemented, but the 
// LOGTEXTRELn macros are not. If DEBUG is NOT defined the LOGTEXTn macros are not 
// implemented but the LOGTEXTRELn macros are.
// All of these macros are abstractions for calls to CETelLogger::Write or 
// CETelLogger::WriteFormat.
// 
//

/**
 @file
 @internalComponent
*/

#include <f32file.h>

#define __LOGGER__

#if defined (_DEBUG)

#ifdef __LOGGER__
#define LOGTEXT(AAA)	CETelLogger::Write(AAA)
#else
#define LOGTEXT(AAA)
#endif

#ifdef __LOGGER__
#define LOGTEXT2(AAA,BBB)	CETelLogger::WriteFormat(TRefByValue<const TDesC8>(AAA),BBB)
#else
#define LOGTEXT2(AAA,BBB)
#endif

#ifdef __LOGGER__
#define LOGTEXT3(AAA,BBB,CCC)	CETelLogger::WriteFormat(TRefByValue<const TDesC8>(AAA),BBB,CCC)
#else
#define LOGTEXT3(AAA,BBB,CCC)
#endif

#define LOGTEXTREL(AAA)
#define LOGTEXTREL2(AAA,BBB)
#define LOGTEXTREL3(AAA,BBB,CCC)

#else

#define LOGTEXT(AAA)
#define LOGTEXT2(AAA,BBB)
#define LOGTEXT3(AAA,BBB,CCC)

#ifdef __LOGGER__
#define LOGTEXTREL(AAA)	CETelLogger::Write(AAA)
#else
#define LOGTEXTREL(AAA)
#endif

#ifdef __LOGGER__
#define LOGTEXTREL2(AAA,BBB)	CETelLogger::WriteFormat(TRefByValue<const TDesC8>(AAA),BBB)
#else
#define LOGTEXTREL2(AAA,BBB)
#endif

#ifdef __LOGGER__
#define LOGTEXTREL3(AAA,BBB,CCC)	CETelLogger::WriteFormat(TRefByValue<const TDesC8>(AAA),BBB,CCC)
#else
#define LOGTEXTREL3(AAA,BBB,CCC)
#endif

#endif		

#ifdef __LOGGER__
#define LOGDESTROY()	CETelLogger::Destruct()
#else
#define LOGDESTROY()
#endif


#ifndef __SLOGGER_H
#define __SLOGGER_H

#ifdef __LOGGER__

/**
 * This class inherits from the CBase class.  It defines the methods necessary for 
 * driving the logging functions.  
 */
class CETelLogger : public CBase
	{
public:
	static CETelLogger* NewL();
	CETelLogger();
	void ConstructL();
	static void Destruct();
	~CETelLogger();
	static void WriteL(const TDesC8& aText);
	static void Write(const TText8* aText);
	static void WriteFormat(TRefByValue<const TDesC8> aFmt,...);
	void WriteRecord(const TDesC8& aText);
private:
	TBool FolderExists();
private:
	TBool iValid;		//< flag that denotes if logging is possible.  Set to True if 
	                    //< if appropriate directory exists and the file can be opened.
						//< if False, then logging calls will exit without writing data.
	RFs iFs;			//< Handle for the File Server
	RFile iFile;		//< File Handle
	};

#endif	// __LOGGER__

#endif	// __SLOGGER_H
