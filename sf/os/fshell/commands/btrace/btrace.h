// btrace.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <fshell/common.mmh>

#include <fshell/extrabtrace.h>
#include FSHELL_D32BTRACE_HEADER
#include <fshell/iocli.h>
#include <fshell/ioutils.h>
#include <fshell/memoryaccesscmd.h>
#include <fshell/ltkutils.h>

using namespace IoUtils;

class CCmdBtrace : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdBtrace();
protected:
	CCmdBtrace();
	void ConstructL();
	void DumpL();
	void TestL();
	void ResetL();
	void SetMode(TUint aMode);
	void EmptyBuffer();
	void RemovePrimaryFilters();
	void RemoveSecondaryFiltersL();
	const TDesC* ModeText(TUint aMode) const;
protected: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
protected:
	TFileName2 iFileName;
	RArray<TUint> iPrimaryFilters;
	RArray<TUint> iSecondaryFilters;
	TUint iMode;
	TUint iBufferSize;
	TBool iDumpToRdebug;
	TBool iFollow;
	TUint iThreshold;
	TBool iEnableTimestamp2;
	TBool iVerbose;
	TBool iTest;
	TBool iReset;
	RBTrace iBtrace;
	TUint iTextTraceMode;
	TBool iBuildConfig;
	};
