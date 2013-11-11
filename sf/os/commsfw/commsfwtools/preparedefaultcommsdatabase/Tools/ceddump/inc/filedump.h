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
 @file filedump.h
 @internalComponent
*/


#ifndef FILEDUMP_H
#define FILEDUMP_H

#include <e32base.h>
#include <f32file.h>
#include <e32cons.h>
#include "epocerror.h"


/**
Theoretical worst case is longest field is populated with non-ASCII data, so
each byte expands to "\xHHHH"
*/
#define MAX_COL_LONG_VAL_LEN 1024
const TInt KMaxExpandedFieldLen = MAX_COL_LONG_VAL_LEN * 6;


class CFileDump : public CBase
/**
@internalComponent
*/
	{
	public:
		static CFileDump* NewL(TPtrC aFilename, TPtrC aAppname, TPtrC aVersion, TBool aDebugOn=EFalse, CConsoleBase* aConsole=NULL, TBool aWriteHeader=ETrue);
		void ConstructL(TPtrC aFilename, TPtrC aAppname, TPtrC version, TBool aWriteHeader);
		~CFileDump();
		void WriteTableHeader(TPtrC aTable);
		void WriteSectionHeader(TInt aCommDbId);
		TBool WriteColumnValue(TPtrC &aColumn, TPtrC &aSetting);
		void WriteFieldCount(TUint aCount);
		void WriteErrorMessage(const TPtrC &aMessage);
		void WriteSectionFooter(TInt aCommDbId);
		void Msg(TPtrC aText, ...);
		void Dbg(TPtrC aText, ...);
		void CloseFile();
		TPtrC GetError() { return iError.GetMessage(); }
		void WriteAttributeSectionHeader();
		void WriteAttributes(TPtrC &aColumn);
		void WriteAttributesToFile(TUint32 aAttributes, TInt aType);
		TBool IsInitialised() {return iInitialised;};

	protected:
		CFileDump(TInt aDebugOn, CConsoleBase* aConsole);
			
	private:
		TBool WriteBufferToFile(TDesC16 &aBuffer, RFile &aHandle);
		
	private:
		RFs iFsSession;
		TBool iInitialised;
		TBool iDebug;
		RFile iFile;
		CConsoleBase* iConsole;
		EpocError iError;
		/** temp buffer */
		TBuf<MAX_COL_LONG_VAL_LEN> iBuffer;	
	};



#endif
