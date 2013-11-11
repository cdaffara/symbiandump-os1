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
 @internalComponent
*/


#ifndef TESTLOG_H
#define TESTLOG_H

#include <e32base.h>
#include <f32file.h>
#include <e32cons.h>
#include "epocerror.h"


#define MAX_BUF_LENGTH      1024
#define MAX_FILENAME_LENGTH 64

class CTestLog : public CBase
/**
@internalComponent
*/
	{
	public:
		static CTestLog* NewLC(RFs &aFsSession, TPtrC aFilename, TBool aDebugOn, CConsoleBase* aConsole);
		void Msg(TPtrC aText, ...);
		void Dbg(TPtrC aText, ...);
		void CloseFile();
		inline TPtrC GetError();
		void WriteErrorMessage(const TPtrC &aMessage);
		~CTestLog();
		
		void DiagPrint(const TText8* aFile, TInt aLine, TPtrC aSeverity, TPtrC aText, ...);
		
	protected:
		CTestLog() 
			{
			}
		void ConstructL(RFs &aFsSession, TPtrC aFilename, TBool aDebugOn, CConsoleBase* aConsole);
	private:
		TBool iInitialised;
		TBool iDebug;
		RFile iFile;
		CConsoleBase* iConsole;
		CEpocError iErr;
		// temp buffer 
		TBuf<MAX_BUF_LENGTH> iBuf;	
		
		TBool WriteBufferToFile(TDesC16 &aBuffer, RFile &aHandle);
	};

inline TPtrC CTestLog::GetError() 
	{
		return iErr.LastErrorMessage();
	}


#endif
