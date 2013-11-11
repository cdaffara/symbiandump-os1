// error.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#ifndef __ERROR_H__
#define __ERROR_H__

#include <e32base.h>

class RIoWriteHandle;
namespace IoUtils
	{
	class CEnvironment;
	}


class TError
	{
public:
	enum TReason
		{
		EUnknown,
		EFailedToCreatePipeLine,
		EFailedToConstructCommand,
		EFailedToRunCommand,
		EFailedToSetChildErrorVar,
		ECommandError,
		EFailedToSetScriptLineVar,
		};
public:
	TError(RIoWriteHandle& aStderr, IoUtils::CEnvironment& aEnv);
	void Set(const TError& aError);
	void Set(TInt aError, TReason aReason);
	void Set(TInt aError, TReason aReason, TRefByValue<const TDesC> aFmt, ...);
	void Report() const;
	TInt Error() const;
	TReason Reason() const;
	const TDesC* Context() const;
	const TDesC& ScriptFileName() const;
	TInt ScriptLineNumber() const;
private:
	void Format(TRefByValue<const TDesC> aFmt, ...);
	void FormatList(TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	void LogIfRequired(TInt aError, TReason aReason, TRefByValue<const TDesC> aFmt, ...);
	void LogListIfRequired(TInt aError, TReason aReason, TRefByValue<const TDesC> aFmt, VA_LIST& aList);
	const TDesC* StringifyReason(TReason aReason) const;
	TBool Verbose() const;
	void NewLine() const;
	void PrintError() const;
	void FormatError(TRefByValue<const TDesC> aFmt, ...) const;
private:
	RIoWriteHandle* iStderr;
	IoUtils::CEnvironment* iEnv;
	TInt iError;
	TReason iReason;
	TBool iSet;
	TBuf<256> iContext;
	mutable TBuf<512> iScratch;
	TFileName iScriptFileName;
	TInt iScriptLineNumber;
	};


#endif // __ERROR_H__
