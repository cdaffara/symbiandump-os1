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
// This contains file utilities which can be called from Test Framework scripts
// 
//

// EPOC includes
#include <e32base.h>
#include <f32file.h> 

// Test system includes
#include <testframework.h>

// do not export if Unit Testing
#if defined (__TSU_TESTFRAMEWORK__)
#undef EXPORT_C
#define EXPORT_C
#endif


/**
 *
 * Initialise the test utilities.
 *
 * @param	"CLog* aLogSystem"
 *			The logger the test utilities are to use.
 *
 * @xxxx 
 *
 */
EXPORT_C CTestUtils* CTestUtils::NewL(CLog* aLogSystem)
	{
	CTestUtils* self = new(ELeave) CTestUtils;
	self->Construct(aLogSystem);
	return self;
	}

/**
 *
 * Initialise the test utilities (second phase).
 *
 * @param	"CLog* aLogSystem"
 *			The logger the test utilities are to use.
 *
 * @xxxx
 *
 */
void CTestUtils::Construct(CLog* aLogSystem)
	{
	iLogSystem = aLogSystem;
	}

/**
 *
 * Run the test utilities inside a trap harness.
 *
 * @param	"const TDesC& aText"
 *			Test string giving the utility to run.
 *
 * @xxxx 
 *
 */
EXPORT_C void CTestUtils::RunUtils(const TDesC& aText)
	{
	TRAPD(r, RunUtilsL(aText));

	if (r != KErrNone)
		{
		WARN_PRINTF3(_L("Warning: Test Utils : %S left, error %d"), &aText, r);
		}
	}

/**
 *
 * Run the test utilities inside a trap harness.
 *
 * @param	"const TDesC& aText"
 *			Test string giving the utility to run.
 *
 * @xxxx
 *
 */
void CTestUtils::RunUtilsL(const TDesC& aText)
	{
	// use Tlex to decode the cmd line
	TLex lex(aText);

	// step over the keyword
	lex.NextToken();

	// get util required
	TPtrC token;
	token.Set(lex.NextToken());

	if (token.FindF( _L("ChangeDir")) == 0)
		{
		User::Panic(_L("ChangeDir is not supported on EKA2"), KErrNotSupported);
		}
	else if (token.FindF( _L("CopyFile")) == 0)
		{
		// get the parameter
		TPtrC file1 = lex.NextToken();
		TPtrC file2 = lex.NextToken();

		CopyFileL(file1, file2);
		}
	else if (token.FindF( _L("CopyAndInvertFile")) == 0)
		{
		// get the parameter
		TPtrC file1 = lex.NextToken();
		TPtrC file2 = lex.NextToken();

		CopyAndInvertFileL(file1, file2);
		}	
	else if (token.FindF( _L("MkDir")) == 0)
		{
		// get the parameter
		token.Set(lex.NextToken());

		MakeDirL(token);
		}
	else if (token.FindF( _L("Delete")) == 0)
		{
		// get the parameter
		token.Set(lex.NextToken());

		DeleteFileL(token);
		}
	else if (token.FindF( _L("MakeReadWrite")) == 0)
		{
		// get the parameter
		token.Set(lex.NextToken());

		MakeReadWriteL(token);
		}
	else
		{
		ERR_PRINTF2(_L("Failed to decode RUN_UTILS command : %S"), &aText);
		User::Leave(KErrNotFound);
		}
	}


/**
 *
 * Test utility : make new directory
 *
 * @param	"const TDesC& aDirname"
 *			New directory path.
 *
 * @xxxx
 *
 */
void CTestUtils::MakeDirL(const TDesC& aDirname) 
	{
	// parse the filenames
	_LIT(KDefault,"C:\\"); 
	TParse fullFileName;
	TInt returnCode = fullFileName.Set(aDirname, &KDefault, NULL);
	if (returnCode != KErrNone)
		{
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);
		ERR_PRINTF3(_L("Failed to decode full path name %S - %S"), 
			&fullFileName.FullName(), 
			&errortxt);
		User::Leave(returnCode);
		}

	// connect to file server
	returnCode = iFs.Connect();
	if (returnCode != KErrNone)
		{
		ERR_PRINTF1(_L("Error connecting to file server"));	
		User::Leave(returnCode);
		}

	// now create the new directory
	returnCode = iFs.MkDirAll(fullFileName.DriveAndPath()); 
	if ((returnCode != KErrNone) && (returnCode != KErrAlreadyExists))
		{		
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);

		ERR_PRINTF3(_L("Error %S making directory %S"), 
			&errortxt, 
			&fullFileName.FullName());
		iFs.Close();
		User::Leave(returnCode);
		}
	if(returnCode == KErrNone)
		{
		// display full (including path) file name
		INFO_PRINTF2( _L("Made directory %S"), &fullFileName.FullName());	
		}
	else
		{
		INFO_PRINTF2( _L("directory already exists %S"), &fullFileName.FullName());		
		}
	iFs.Close();
	}


/**
 *
 * Test utility : copy a file
 *
 * @param	"const TDesC& aOld"
 *			Source file.
 *
 * @param	"const TDesC& aNew"
 *			Destination file.
 *
 * @xxxx
 *
 */
void CTestUtils::CopyFileL (const TDesC& aOld,const TDesC& aNew) 
	{
	// connect to file server
	TInt returnCode = iFs.Connect();
	if (returnCode != KErrNone)
		{
		ERR_PRINTF1(_L("Error connecting to file server"));	
		User::Leave(returnCode);
		}

	// create a file manager
	CFileMan* fileMan = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileMan);

	// parse the filenames
	TParse parseSource;
	returnCode = parseSource.Set(aOld, NULL, NULL);
	if (returnCode != KErrNone)
		{
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);
		ERR_PRINTF3( _L("Failed to parse %S - %S"), 
			&aOld, 
			&errortxt);
		iFs.Close();
		User::Leave(returnCode);
		}
 
	// parse the filenames
	TParse parseTarget;
	returnCode = parseTarget.Set(aNew, NULL, NULL);
	if (returnCode != KErrNone)
		{
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);
		ERR_PRINTF3(_L("Failed to parse %S - %S"), 
			&aNew, 
			&errortxt);
		iFs.Close();
		User::Leave(returnCode);
		}

	// do the copy
	returnCode = fileMan->Copy(parseSource.FullName(), 
		parseTarget.FullName(), 
		CFileMan::EOverWrite | CFileMan::ERecurse);

	if (returnCode != KErrNone)
		{
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);

		ERR_PRINTF4(_L("Failed to copy %S to %S - %S"), 
			&parseSource.FullName(), 
			&parseTarget.FullName(),
			&errortxt);
		iFs.Close();
		User::Leave(returnCode);
		}

	INFO_PRINTF3(_L("Copied file from %S to %S"), 
					&parseSource.FullName(), 
					&parseTarget.FullName());

	iFs.Close();
	CleanupStack::PopAndDestroy(fileMan);
	}


/**
 *
 * Test utility : invert a file and copy it to a tartget file
 *
 * @param	"const TDesC& aOld"
 *			Source file.
 *
 * @param	"const TDesC& aNew"
 *			Destination file.
 *          Will contain the inverted file
 *          
 *
 * @xxxx
 *
 */
void CTestUtils::CopyAndInvertFileL (const TDesC& aOld,const TDesC& aNew) 
	{
    
    TRAPD(error, DoCopyAndInvertL(aOld, aNew));  

    if (error != KErrNone)
		{
   		TPtrC errortxt = CLog::EpocErrorToText(error);
		ERR_PRINTF2(_L("Failed to copy Files - %S"), &errortxt);
		User::Leave(error);
		}	
    	
	}
	

	
	
/**
 *
 * Test utility : Do the invert and copy of a source file
 *				  into a target file. 
 *
 * @param	"const TDesC& aOld"
 *			Source file.
 *
 * @param	"const TDesC& aNew"
 *			Destination file.
 *          Will contain the inverted file
 *          
 *
 * @xxxx
 *
 */
void CTestUtils::DoCopyAndInvertL (const TDesC& aOld,const TDesC& aNew)
	{
	// connect to file server
	TInt returnCode = iFs.Connect();
	if (returnCode != KErrNone)
		{
		ERR_PRINTF1(_L("Error connecting to file server"));	
		User::Leave(returnCode);
		}
    CleanupClosePushL(iFs);	
		
	// parse the filenames
	TParse parseSource;
	User::LeaveIfError(parseSource.Set(aOld, NULL, NULL));

	// parse the filenames
	TParse parseTarget;
	User::LeaveIfError(parseTarget.Set(aNew, NULL, NULL));
	
	RFile sourceFile;
	
	// open the source file for reading
	User::LeaveIfError(sourceFile.Open(iFs, parseSource.FullName(), EFileRead));
	CleanupClosePushL(sourceFile);
	
	RFile targetFile;
	
	// open the target file for writing
	User::LeaveIfError(targetFile.Replace(iFs, parseTarget.FullName(), EFileWrite));
	CleanupClosePushL(targetFile);
	
	const TInt KSizeOfInvertBuf = 256;
	
	// Buffers used for read,invert and write to the target file
	TBuf8<KSizeOfInvertBuf> invertBuffer;
	TInt8 byteBuf;//A temp byte size buffer
	
	//Invert the source file and copy it into the target file 
	do 
		{
		User::LeaveIfError(sourceFile.Read(invertBuffer));
		
		for (TInt index=0; index<invertBuffer.Length(); index++)
			 {
			 byteBuf = invertBuffer[index];
			 byteBuf ^= 0xFF; //inverting the buf bits
			 invertBuffer[index] = byteBuf;
			 }
		User::LeaveIfError(targetFile.Write(invertBuffer));
	 	}
   while (invertBuffer.Length() != 0);
   

    // Flush the data to the target file
	User::LeaveIfError(targetFile.Flush());
	
    // display full (including path) file name
	INFO_PRINTF3(_L("Copied and Inverted successfully file %S to file %S"), 
		&parseSource.FullName(),
		&parseTarget.FullName());
	
	//closing the files
	CleanupStack::PopAndDestroy(3, &iFs);
	
	}	





/**
 *
 * Test utility : delete a file
 *
 * @param	"const TDesC& aFile"
 *			File to be deleted.
 *
 * @xxxx
 *
 */
void CTestUtils::DeleteFileL (const TDesC& aFile) 
	{
	// parse the filenames
	_LIT(KDefault,"C:\\xxxxx.xxx"); 
	TParse fullFileName;
	TInt returnCode = fullFileName.Set(aFile, &KDefault, NULL);
	if (returnCode != KErrNone)
		{
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);
		ERR_PRINTF3(_L("Failed to decode full path name %S - %S"), 
			&fullFileName.FullName(), 
			&errortxt);
		User::Leave(returnCode);
		}

	// connect to file server
	returnCode = iFs.Connect();
	if (returnCode != KErrNone)
		{
		ERR_PRINTF1(_L("Error connecting to file server"));	
		User::Leave(returnCode);
		}

	// now do the delete
	returnCode = iFs.Delete(fullFileName.FullName()); 
	if (returnCode != KErrNone)
		{		
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);
		ERR_PRINTF3(_L("Error %S deleting %S"), 
			&errortxt, 
			&fullFileName.FullName());
		iFs.Close();
		User::Leave(returnCode);
		}

	// display full (including path) file name
	INFO_PRINTF2(_L("Deleted %S"), &fullFileName.FullName());	
	iFs.Close();
	}


/**
 *
 * Test utility : make a file read-write (clear the read-only attribute)
 *
 * @param	"const TDesC& aFile"
 *			File to be made read-write.
 *
 * @xxxx
 *
 */
void CTestUtils::MakeReadWriteL (const TDesC& aFile) 
	{
	// parse the filenames
	_LIT(KDefault,"C:\\xxxxx.xxx"); 
	TParse fullFileName;
	TInt returnCode = fullFileName.Set(aFile, &KDefault, NULL);
	if (returnCode != KErrNone)
		{
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);
		ERR_PRINTF3(_L("Failed to decode full path name %S - %S"), 
			&fullFileName.FullName(), 
			&errortxt);
		User::Leave(returnCode);
		}

	// connect to file server
	returnCode = iFs.Connect();
	if (returnCode != KErrNone)
		{
		ERR_PRINTF1(_L("Error connecting to file server"));	
		User::Leave(returnCode);
		}

	// now do the delete
	returnCode = iFs.SetAtt(fullFileName.FullName(), 0, KEntryAttReadOnly); 

	// check for errors
	if (returnCode != KErrNone )
		{		
		TPtrC errortxt = CLog::EpocErrorToText(returnCode);

		ERR_PRINTF3(_L("Error %S making %S read-write"), 
			&errortxt, 
			&fullFileName.FullName());
		iFs.Close();
		User::Leave(returnCode);
		}

	// display full (including path) file name
	INFO_PRINTF2(_L("Made %S read-write"), &fullFileName.FullName());	
	iFs.Close();
	}

/**
 *
 * General logging function for test utils.
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx 
 *
 */
void CTestUtils::Log(TRefByValue<const TDesC16> aFmt, ...)
	{

	VA_LIST aList;
	VA_START(aList, aFmt);

	if(iLogSystem) 
		iLogSystem->Log(aFmt, aList);

	VA_END(aList);
	}

/**
 *
 * General logging function for test utils, with severity.
 *
 * @param	"TInt aSeverity"
 *			Severity level required to log
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx 
 *
 */
void CTestUtils::Log(TInt /* aSeverity */, TRefByValue<const TDesC16> aFmt, ...)
	{
	VA_LIST aList;
	VA_START(aList, aFmt);

	// NB : Test Utils log regardless of severity (as they are tied to no suite)
	if(iLogSystem) 
		iLogSystem->Log(aFmt, aList);

	VA_END(aList);
	}

/**
 *
 * Traceable logging function for test utils.
 *
 * @param	"const TText8* aFile"
 *			Source code file name
 *
 * @param	"TInt aLine"
 *			Source code line
 *
 * @param	"TInt aSeverity"
 *			Severity level required to log
 *
 * @param	"TRefByValue<const TDesC16> aFmt"
 *			Printf-style format.
 *
 * @param	"..."
 *			Variable print parameters
 *
 * @xxxx 
 *
 */
void CTestUtils::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC16> aFmt,...)
	{
	VA_LIST aList;
	VA_START(aList, aFmt);

	// NB : Test Utils log regardless of severity (as they are tied to no suite)
	if(iLogSystem)
		iLogSystem->LogExtra(aFile, aLine, aSeverity, aFmt, aList);

	VA_END(aList);
	}


