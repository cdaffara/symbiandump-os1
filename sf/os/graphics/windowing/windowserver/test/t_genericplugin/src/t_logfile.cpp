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

#include "t_logfile.h"

/**
Constructor for CLogFile
*/
CLogFile::CLogFile()
	{
	}

/**
Destructor for CLogFile
@post The log file is closed.
*/
CLogFile::~CLogFile()
	{
	iFile.Close();
	iFs.Close();
	}

/**
Creates a new CLogFile object. 
Standardized safe construction which leaves nothing on the cleanup stack.
@return A pointer to the newly created object.
@post	This object is fully constructed and initialized.
*/ 
CLogFile* CLogFile::NewL()
	{
	CLogFile* self = new(ELeave) CLogFile();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Initialisation phase of two phase construction.
@post	The logfile is opened and seeked to the end for writing. 
		The logfile is created if it doesn't exist.
*/
void CLogFile::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iFs.MkDirAll(KLogFileName);
	TInt err = iFile.Open(iFs,KLogFileName,EFileStreamText|EFileWrite|EFileShareReadersOrWriters);
	if (err == KErrNotFound)
		{
		User::LeaveIfError(iFile.Create(iFs,KLogFileName,EFileStreamText|EFileWrite|EFileShareReadersOrWriters));
		}
	else
		{	
		User::LeaveIfError(err);
		}	
	TInt seekpos = 0;
	User::LeaveIfError(iFile.Seek(ESeekEnd,seekpos));	
	iEol8=TPtrC8((TUint8 *)"\r\n");
	}

/**
Writes the message to the log file.
@param	aDes The message to be written to the log file.
*/
void CLogFile::WriteToLogL(const TDesC &aDes)
   	{
  	TBuf8<255> des1;
  	des1.Copy(aDes);
  	User::LeaveIfError(iFile.Write(des1));
  	User::LeaveIfError(iFile.Write(iEol8));
  	iFile.Flush();		//Ignore Error
  	}

/**
Delete the log file.
*/
void CLogFile::DeleteLogFileL()
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	fs.Delete(KLogFileName);
	CleanupStack::PopAndDestroy(&fs);
	}
