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

// EPOC includes
#include <e32base.h>

// Test system includes
#include "LogFile.h"


/*
 * destructor for CFileLogger.
 */
CFileLogger::~CFileLogger()
	{
	// make sure session to file server is closed
	Close();
	}


/**
 *
 * Static constructor for CFileLogger.
 *
 * @return	"CFileLogger*"
 *			The constructed CFileLogger
 *
 * @xxxx 
 *
 */
CFileLogger* CFileLogger::NewL()
	{
	CFileLogger* s = new(ELeave) CFileLogger;
	return s;
	}

/**
 *
 * Create and open a log file. The file is shareable, and a mutex
 * and file seeks employed in case more than one session is using 
 * the file.
 *
 * @param	"const TDesC& aDir"
 *          The file path of the logfile to be created. If the path
 *			does not exist, the logfile will not be created.
 *
 * @param   "const TDesC& aName"
 *          The file name of same.
 * 
 * @xxxx
 *
 */
void CFileLogger::CreateLog(const TDesC& aDir, const TDesC& aName)
	{
	// if aDir or aName are empty, then panic
	__ASSERT_ALWAYS(aDir.Ptr() != NULL, User::Panic(_L("CFileLogger"), 0));
	__ASSERT_ALWAYS(aName.Ptr() != NULL, User::Panic(_L("CFileLogger"), 1));


	// if aDir doesn't exist, create it anyway
	TUint aAttValue;
	if (iFs.Att(aDir, aAttValue) != KErrNone) 
		{
		TInt returnCode = iFs.MkDirAll(aDir);
		if (returnCode != KErrNone)
			{
			iEnabled = EFalse;
			User::Panic(_L("CFileLogger"), 2);
			}
		}

	iEnabled = ETrue;
	iLogName = aName;

	// create logfile mutex
	RMutex logMutex;
	logMutex.CreateGlobal(iLogName);

	// make the correct file name
	TFileName logFile;
	logFile.Format(_L("%S%S"), &aDir, &aName);	// aDir will be \\-terminated

	// If the file does not exist, create it. If it does, truncate it
	// Open it for sharing
	TInt returnCode=iLogfile.Replace(iFs, logFile, EFileWrite | EFileStreamText | EFileShareAny); 

	// check if open fails 
	if (returnCode==KErrNone)
		{
		//file has opened ok
		//add the start to the HTML
		iLogfile.Write(_L8("<html><body><pre>\n"));
		TInt dummy;
		iLogfile.Seek(ESeekEnd, dummy);
		}
	else
		iEnabled = EFalse;
	}

/**
 *
 * Close the logfile. The flag iEnabled will be clear if there
 * is no logfile to close.
 * 
 * @xxxx
 *
 */
void CFileLogger::CloseLog()
	{

	// this should be mutex-safe as each thread has its own handle to 
	// the log file
	// if logging enabled, flush buffers
	if (iEnabled)
		{
		iLogfile.Flush();
		iLogfile.Close();
		iEnabled = EFalse;
		}

	// disconnect from the file server
	iFs.Close();
	}

/**
 *
 * Write output to the logfile.
 *
 * @param	"const TDesC& aMsg"
 *          The message (string) to be written.
 * 
 * @xxxx
 *
 */
void CFileLogger::WriteLog(const TDesC& aMsg)
	{

	if (!iEnabled) 
		return;

	// convert from unicode to 8 bit
	TBuf8<KMaxLogLineLength> lineBuf8;
	const TText* msgBuf = aMsg.Ptr();
	for (TInt i = 0; i < aMsg.Length() ;i++)
		lineBuf8.Append(STATIC_CAST(TText, msgBuf[i]));

	// wait on mutex
	RMutex logMutex;
   	logMutex.OpenGlobal(iLogName);

	logMutex.Wait();

	// write to log file
	TInt dummy=0;
	iLogfile.Seek(ESeekEnd, dummy);
	iLogfile.Write(lineBuf8);
	// seek to end after write - in case another process or thread is also using the file
	dummy=0; //dummy is updated after a seek to contain the new file position. With ESeekEnd
	         //it is used as an offest value to control the seek.
	iLogfile.Seek(ESeekEnd, dummy);

	logMutex.Signal();
	logMutex.Close();
	}

/**
 *
 * Connect to the file server. Used as a helper function
 * by classes owning a CFileLogger, to ensure that a
 * file server session can be opened
 *
 * @return	"TInt"
 *			standard EPOC error code
 * 
 * @xxxx
 *
 */
TInt CFileLogger::Connect()
	{
	TInt err = iFs.Connect();
	if (err == KErrNone)
		iFsOpen = ETrue;
	return err;
	}

/**
 *
 * Close a file server session. Used as a helper function
 * by classes owning a CFileLogger.
 *
 * @xxxx
 *
 */
void CFileLogger::Close()
	{
	if (iFsOpen)
		{
		iFs.Close();
		iFsOpen = EFalse;
		}
	}
