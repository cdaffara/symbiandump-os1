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

#ifndef __LOGFILE_H__
#define __LOGFILE_H__

#include <f32file.h>

#include <testframework.h>

/**
 *
 * File logger class for server sessions.
 * Manages one logfile which is shareable and mutex-safe.
 *
 * @xxxx
 *
 */
class CFileLogger : public CBase
{
public:
	static CFileLogger* NewL();
	~CFileLogger();
	void CreateLog(const TDesC& aDir, const TDesC& aName);
	void OpenLog(const TDesC& aDir, const TDesC& aName);
	void CloseLog();
	void WriteLog(const TDesC& aMsg);
	TInt Connect();
	void Close();

private:
	TBuf<KMaxLogFilenameLength> iLogName;
	RFile iLogfile;
	RFs	  iFs;
	TBool iEnabled;
	TBool iFsOpen;	// connected to File system if true
};


#endif // __LOGFILE_H__
