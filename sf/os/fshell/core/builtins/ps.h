// ps.h
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <fshell/common.mmh>
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
#include <fshell/memoryaccess.h>
#endif

using namespace IoUtils;


class CCmdPs : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdPs();
private:
	CCmdPs();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
#if defined(__WINS__) && !defined(EKA2)
	void PrintInfoL(RThread& aThread);
#else
	void PrintInfoL(RProcess& aProcess);
#endif
	void PrintStackInfoL(RThread& aThread, const TDesC& aThreadName);
	void PrintHeapInfoL(RThread& aThread, const TDesC& aThreadName);
	void PrintCpuTimeL(RThread& aThread, const TDesC& aThreadName);
	void PrintChunkInfoL(const TDesC& aProcessName);
	void PrintSizeL(const TDesC& aCaption, TInt aSize);
	void PrintCodeSegsL(RProcess& aProcess);
private:
	TUint iProcessId;
	HBufC* iMatch;
	TBool iPrintPriority;
	TBool iPrintThreads;
	TBool iPrintStackInfo;
	TBool iPrintHeapInfo;
	TBool iPrintCpuTime;
	TBool iPrintChunkInfo;
	TBool iPrintFileName;
	TBool iPrintCommandLine;
	TBool iPrintFlags;
	TBool iPrintMemoryInfo;
	TBool iPrintCodesegs;
	TBool iVerbose;
	TBool iHuman;
	TBool iExcludeDead;
	TBool iOnlyDead;
	TBool iAddresses;
	TBool iHandleCount;
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
	RMemoryAccess iMemoryAccess;
#endif
	TFullName iProcessName;
	TFullName iThreadName;
	TFullName iChunkName;
	CTextFormatter* iFormatter;
	};
