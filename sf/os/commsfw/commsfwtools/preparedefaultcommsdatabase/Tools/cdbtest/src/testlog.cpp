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
 @file testlog.h
*/

#include "testlog.h"

_LIT(KLineNumber, " line %d ");

#ifdef _WIN32
_LIT(KLineEnd, "\r\n");
#else
_LIT(KLineEnd, "\n");
#endif

CTestLog* CTestLog::NewLC(RFs& aFsSession, TPtrC aFilename, TInt aDebugOn, CConsoleBase* aConsole)
	{
	CTestLog* fd = new (ELeave) CTestLog;
	CleanupStack::PushL(fd);
	fd->ConstructL(aFsSession, aFilename, aDebugOn, aConsole);
	return fd;
	}
	
CTestLog::~CTestLog()
	{
	iFile.Close();
	iInitialised = EFalse;
	}
	
	
void CTestLog::ConstructL(RFs &aFsSession, TPtrC aFilename, TInt aDebugOn, CConsoleBase* aConsole)
/**
Opens the output file

@param aFsSession A session with the file server
@param aFilename
@param aDebugOn
@param aConsole
*/
	{
	if (iErr.Evaluate(iFile.Replace(aFsSession, aFilename, EFileShareAny|EFileStream|EFileWrite)))
		{
		iDebug = aDebugOn;
		iConsole = aConsole;
		iInitialised = ETrue;
		}
	}


void CTestLog::WriteErrorMessage(const TPtrC &aMessage)
/**
Writes a message to the file

@param aMessage The error message 
*/
	{
	_LIT(KWriteErrorStr,"ERROR - %S\r\n");
	
	if (iInitialised && aMessage.Length() && aMessage.Length() < (MAX_BUF_LENGTH))
		{
		iBuf.Format(KWriteErrorStr,
			&aMessage);
		WriteBufferToFile(iBuf, iFile);
		}
	}

void CTestLog::Msg(TPtrC aText, ...)
	{
	if (iInitialised && aText.Length() && aText.Length() < MAX_BUF_LENGTH)
		{
		VA_LIST list;
		VA_START(list, aText);
		
		iBuf.FormatList(aText, list);
		iBuf.Append(KLineEnd);
		
		if (iConsole)
			iConsole->Printf(iBuf);
		
		WriteBufferToFile(iBuf, iFile);
		
		VA_END(list);
		}
	}

void CTestLog::Dbg(TPtrC aText, ...)
	{
	if (iInitialised && iDebug && aText.Length() && aText.Length() < MAX_BUF_LENGTH)
		{
		VA_LIST list;
		VA_START(list, aText);
		
		iBuf.FormatList(aText, list);
		iBuf.Append(KLineEnd);
		
		if (iConsole)
			iConsole->Printf(iBuf);
		
		WriteBufferToFile(iBuf, iFile);
		
		VA_END(list);
		}
	}

void CTestLog::DiagPrint(const TText8* aFile, TInt aLine, TPtrC aSeverity, TPtrC aText, ...)
{
	if (iInitialised && iDebug && aText.Length() && aText.Length() < MAX_BUF_LENGTH)
		{
	    VA_LIST list;
	    VA_START(list, aText);
	    
	    // Clear the buffer
	    iBuf.Zero();
	    
	    // Add the file name
	    TInt i = 0;
	    while((aFile[i] != '\0') && (i < MAX_FILENAME_LENGTH))
	        {
	    	iBuf.Append(aFile[i++]);
	        }
	    
	    // Add the line number and message
	    iBuf.AppendFormat(KLineNumber, aLine);
	    iBuf.AppendFormat(aSeverity);
	    iBuf.AppendFormatList(aText, list);
	    iBuf.Append(KLineEnd);
	
	    if (iConsole)
		   iConsole->Printf(iBuf);
	
	    WriteBufferToFile(iBuf, iFile);
	
	    VA_END(list);
		}
}


TBool CTestLog::WriteBufferToFile(TDesC16 &aBuffer, RFile &aHandle)
	{
	static TBuf8<1024> outputBuffer;
	static TPtrC16 ptr;
	static TInt consumed;
	static TInt remainder = 0;
	
	TBool valid = EFalse;
	
	if (aBuffer.Length())
		{
		ptr.Set(aBuffer);
		do
			{
			// get something to write
			consumed = Min(outputBuffer.MaxLength(), ptr.Length());
			
			// write it
			outputBuffer.Copy(ptr.Left(consumed));
			if (aHandle.Write(outputBuffer) != KErrNone)
				return EFalse;
			
			// get the next chunk
			remainder = ptr.Length() - consumed;
			if (remainder > 0)
				ptr.Set(ptr.Right(remainder));
			
			}while (remainder > 0);
			
			valid = ETrue;
			
			// Make sure the latest log messages are flushed to the file.
			aHandle.Flush();
		}
	return valid;
	}
