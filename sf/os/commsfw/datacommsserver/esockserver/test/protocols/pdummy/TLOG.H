// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__TLOG_H__)
#define __TLOG_H__
#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
#if !defined(__E32CONS_H__)
#include <e32cons.h>
#endif
#if !defined(__E32VER_H__)
#include <e32ver.h>
#endif
#if !defined(__E32FILE_H__)
#include <f32file.h>
#endif
#if !defined(__E32TEST_H__)
#include <e32test.h>
#endif


class RLog
	{

private:
	RFile	file;			// Handle to the file
	RFs		fileServer;		// File server session
	RTest	*test;			// A pointer to the actual test class


public:
	IMPORT_C		RLog();		// Default C'or (also copy C'or)
	IMPORT_C		RLog(const TDesC &aLogFileName, RTest *atest);	// C'or (Needds a file name and A Rtest pointer...)
	IMPORT_C void	Construct(const TDesC &aLogFileName, RTest *atest);
	IMPORT_C void	Close();


	IMPORT_C void	Start(const TDesC &aHeading);
	IMPORT_C void	Next(const TDesC &aHeading);
	IMPORT_C void	Printf(TRefByValue<const TDesC> aFmt,...);
	IMPORT_C void	Log(TRefByValue<const TDesC> aFmt,...);
	IMPORT_C void	SeekEnd();
};

// Macros

#define LINEFEED	Log.Printf(_L("\n"));

	
#endif		// __TLOG_H__

