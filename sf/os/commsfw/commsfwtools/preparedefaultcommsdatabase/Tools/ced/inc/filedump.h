// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "dbdef.h"

/**
Theoretical worst case is longest field is populated with non-ASCII data, so
each byte expands to "\xHHHH"
*/
const TInt KMaxExpandedFieldLen = MAX_COL_LONG_VAL_LEN * 6;


class CFileDump : public CBase
/**
@internalComponent
*/
	{
	public:
		static CFileDump* NewL(RFs &fsSession, TPtrC filename, TPtrC appname, TPtrC version, TBool bDebugOn, CConsoleBase* aConsole, TBool bWriteHeader);
		void WriteTableHeader(TPtrC table);
		void WriteSectionHeader(TInt aCommDbId);
		void WriteColumnValue(TPtrC &column, TPtrC &setting);
		void WriteFieldCount(TUint count);
		void WriteErrorMessage(const TPtrC &message);
		void WriteSectionFooter(TInt aCommDbId);
		void Msg(TPtrC text, ...);
		void Dbg(TPtrC text, ...);
		void Error(TPtrC text, ...);
		void CloseFile();
		TPtrC GetError() { return oErr.GetMessage(); }
		~CFileDump();
		TBool IsInitialised() {return bInitialised;}
		
	protected:
		CFileDump() 
			{
			}
		void ConstructL(RFs &fsSession, TPtrC filename, TPtrC appname, TPtrC version, TBool bDebugOn, CConsoleBase* aConsole, TBool bWriteHeader);
	private:
		TBool bInitialised;
		TBool bDebug;
		RFile file;
		CConsoleBase* pConsole;
		EpocError oErr;
		/** temp buffer */
		TBuf<MAX_COL_LONG_VAL_LEN> buf;	
		
		TBool WriteBufferToFile(TDesC16 &buffer, RFile &handle);
	};

#endif
