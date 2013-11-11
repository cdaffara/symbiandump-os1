// memspy.cpp
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

#include <fshell/ioutils.h>
#include <fshell/common.mmh>
#include <fshell/ltkutils.h>
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyenginehelpersysmemtracker.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/MemSpyEngineObjectContainer.h>

using namespace IoUtils;

class CCmdMemspy : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdMemspy();
private:
	CCmdMemspy();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum TCmd
		{
		ESymtDump,
		ESymtStart,
		EHeapDump,
		ECellDump,
		EChunkInfo,
		};
	TCmd iCmd;
	HBufC* iParameter;

	CMemSpyEngine* iEngine;
	};

EXE_BOILER_PLATE(CCmdMemspy)

CCommandBase* CCmdMemspy::NewLC()
	{
	CCmdMemspy* self = new(ELeave) CCmdMemspy();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdMemspy::~CCmdMemspy()
	{
	delete iEngine;
	delete iParameter;
	}

CCmdMemspy::CCmdMemspy()
	: CCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdMemspy::Name() const
	{
	_LIT(KName, "memspy");	
	return KName;
	}

void CCmdMemspy::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iCmd, _L("command"));
	aArguments.AppendStringL(iParameter, _L("parameter"));
	}

void CCmdMemspy::OptionsL(RCommandOptionList& /*aOptions*/)
	{
	}

void CCmdMemspy::DoRunL()
	{
	TRAPL(iEngine = CMemSpyEngine::NewL(FsL()), _L("Couldn't construct CMemSpyEngine"));
	iEngine->InstallSinkL(ESinkTypeFile);

	//LtkUtils::BreakpointPushL();

	TMemSpyEngineHelperSysMemTrackerConfig config;
	config.iEnabledCategories = TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategorySystemMemory
							  | TMemSpyEngineHelperSysMemTrackerConfig::EMemSpyEngineSysMemTrackerCategoryUserHeap
							  ;
	config.iDumpData = EFalse;
	TBool complete = ETrue;

	if (iCmd == ESymtDump)
		{
		iEngine->HelperSysMemTracker().SetConfigL(config);
        iEngine->HelperSysMemTracker().CheckForChangesNowL();
		}
	else if (iCmd == ESymtStart)
		{
		iEngine->HelperSysMemTracker().StartL(config);
		complete = EFalse;
		}
	else if (iCmd == EHeapDump)
		{
		CMemSpyProcess* process = NULL;
		CMemSpyThread* thread = NULL;
		LeaveIfErr(iEngine->Container().ProcessAndThreadByPartialName(iParameter ? *iParameter : KNullDesC(), process, thread), _L("Couldn't find thread"));

		iEngine->HelperHeap().OutputHeapDataUserL(*thread);
		}
	else if (iCmd == ECellDump)
		{
		CMemSpyProcess* process = NULL;
		CMemSpyThread* thread = NULL;
		LeaveIfErr(iEngine->Container().ProcessAndThreadByPartialName(iParameter ? *iParameter : KNullDesC(), process, thread), _L("Couldn't find thread"));

		iEngine->HelperHeap().OutputCellListingUserL(*thread);
		}
	else if (iCmd == EChunkInfo)
		{
		CMemSpyProcess* process = NULL;
		CMemSpyThread* thread = NULL;
		LeaveIfErr(iEngine->Container().ProcessAndThreadByPartialName(iParameter ? *iParameter : KNullDesC(), process, thread), _L("Couldn't find process"));
		iEngine->HelperChunk().OutputChunkInfoForProcessL(*process);
		}

	//CleanupStack::Pop();

	if (complete) Complete();
	}
