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
// This contains TS_MultiHoming RConnection Start primitive.
// RConnection starts opens and starts a connection, and 
// stores this connection in C
// 
//

/**
 @file
*/
 

#include "TS_CopyComDbFile.h"

/* 
 * Test 1.1
 * Open a default / named connection
 */
CTS_CopyComDbFile::CTS_CopyComDbFile()
	{
	iTestStepName = KCopyComDbFile;
	}

CTS_CopyComDbFile::~CTS_CopyComDbFile()
	{	
	}

enum TVerdict CTS_CopyComDbFile::doTestStepPreambleL()
/**
 * Implements OOM testing in each test
 */
	{
	if (!(GetBoolFromConfig(KCopyComDbFile,KOomTest,iIsOOMTest)))
		{
		iIsOOMTest=EFalse;
		}
	return EPass;
	}


enum TVerdict CTS_CopyComDbFile::doTestStepL()
/**
 * Reads the configuration file to find out which
 * connection(s) need to be closed.
 * Started connections are stored in the array.
 */
	{
	TInt numRenames=0;
	TPtrC connPtr(KNameDefault);
	TInt err = KErrNone;

	
	err = GetStringFromConfig(KCopyComDbFile, KSourceFileName, connPtr);
	
	if(err != 1)		// We have no config available, so 1 default connection
		{	
		return EFail;
		}



	// create a File Server and File Manager
	RFs theFs;
	theFs.Connect();
	CFileMan * Fileman= CFileMan::NewL(theFs);

	_LIT(KCommDb,"c:\\private\\100012a5\\dbs[10004e1d]cdbv3.dat");

	TBuf<100> newComDbname(KCommDb);

	// use filemanager to delete Commmdb
//	Fileman->Delete( KCommDb );

//	LogExtra(((TText8*)(__FILE__)), (__LINE__), ESevrErr,
//		_L("Copying %S to new cdbv3.dat"), connPtr);
	newComDbname.AppendNum(numRenames++);
	Fileman->Rename( KCommDb, newComDbname);
	RFile comDb, sourceFile;

	// Open the file for reading

	err = sourceFile.Open(theFs, connPtr, EFileRead|EFileShareAny);
	err = comDb.Create(theFs, KCommDb, EFileWrite | EFileShareAny);


	TInt fileSize;
	err = sourceFile.Size(fileSize);
	HBufC8 *buffer = HBufC8::New(fileSize);
	TPtr8 ptr = buffer->Des();

	err = sourceFile.Read(ptr);

	err = comDb.Write(ptr); 

	sourceFile.Close();
	comDb.Close();
	theFs.Close();

	delete buffer;

		
	return iTestStepResult;
	}


