// ffstrace.cpp
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

#include <fshell/extrabtrace.h>
#include <fshell/btrace_parser.h>

#include <fshell/ioutils.h>
#include <fshell/common.mmh>

using namespace IoUtils;

class CCmdFfstrace : public CCommandBase, public MBtraceObserver
	{
public:
	static CCommandBase* NewLC();
	~CCmdFfstrace();
private:
	CCmdFfstrace();
	void HandleBtraceFrameL(const TBtraceFrame& aFrame);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum TCmd
		{
		EMonitor,
		ELoad,
		EUnload,
		};
	TCmd iCommand;
private:
	CBtraceReader* iBtraceReader;
	};

EXE_BOILER_PLATE(CCmdFfstrace)

CCommandBase* CCmdFfstrace::NewLC()
	{
	CCmdFfstrace* self = new(ELeave) CCmdFfstrace();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdFfstrace::~CCmdFfstrace()
	{
	delete iBtraceReader;
	}

CCmdFfstrace::CCmdFfstrace()
	: CCommandBase(EManualComplete)
	{
	}

const TDesC& CCmdFfstrace::Name() const
	{
	_LIT(KName, "ffstrace");	
	return KName;
	}

void CCmdFfstrace::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iCommand, _L("command"));
	}

void CCmdFfstrace::OptionsL(RCommandOptionList& /*aOptions*/)
	{
	//TODO: aOptions.AppendBoolL(iOpt, _L("example_opt"));
	//TODO: Also remember to update the CIF file for any options you add.
	}

void CCmdFfstrace::DoRunL()
	{
	_LIT(KPluginDll, "ffstraceplugin.fxt");
	_LIT(KFfsTracerPluginName, "FfsTracePlugin");
	if (iCommand == EMonitor || iCommand == ELoad)
		{
		TInt err = FsL().AddPlugin(KPluginDll);
		if (err && err != KErrAlreadyExists) LeaveIfErr(err, _L("Couldn't load filesystem plugin %S"), &KPluginDll);

		err = FsL().MountPlugin(KFfsTracerPluginName);
		//TODO handle this being called repeatedly
		LeaveIfErr(err, _L("Couldn't mount filesystem plugin %S"), &KFfsTracerPluginName);
		}
	else if (iCommand == EUnload)
		{
		FsL().DismountPlugin(KFfsTracerPluginName);
		LeaveIfErr(FsL().RemovePlugin(KFfsTracerPluginName), _L("No plugin to unload"));
		}

	if (iCommand == EMonitor)
		{
		iBtraceReader = CBtraceReader::NewL(CBtraceReader::EFlushOnBtraceThreshold, 1024*1024, 512*1024);
		iBtraceReader->AddObserverL(BTrace::EFfsTrace, *this);
		iBtraceReader->SetMultipartReassemblyL(10);
		TBtraceTickCount now;
		now.SetToNow();
		iBtraceReader->Start(now, 1000000);
		}
	else
		{
		Complete(KErrNone);
		}
	}

void CCmdFfstrace::HandleBtraceFrameL(const TBtraceFrame& aFrame)
	{
	if (aFrame.iCategory == BTrace::EFfsTrace)
		{
		TInt fn = aFrame.iSubCategory & ~BTrace::EFfsPost;
		TBool post = aFrame.iSubCategory & BTrace::EFfsPost;
		TUint threadId = *(TUint*)aFrame.iData.Ptr();
		TPtrC name((TUint16*)(aFrame.iData.Ptr()+4), (aFrame.iData.Size()-4)/2);
		TPtrC newName;
		// Not everything gets as far as post - if an error occurs during the pre checking for eg
		if (post) Printf(_L("-"));
		else Printf(_L("+"));

		TBool twoNames = (fn == BTrace::EFfsRename || fn == BTrace::EFfsFileRename);
		if (twoNames)
			{
			TInt separator = name.Locate(0);
			if (separator != KErrNotFound)
				{
				newName.Set(name.Mid(separator+1));
				name.Set(name.Left(separator));
				}
			}

		switch (fn)
			{
			case BTrace::EFfsDelete:
				Printf(_L("Delete %S"), &name);
				break;
			case BTrace::EFfsFileOpen:
				Printf(_L("Open %S"), &name);
				break;
			case BTrace::EFfsFileCreate:
				Printf(_L("Create %S"), &name);
				break;
			case BTrace::EFfsFileSubClose:
				Printf(_L("Close %S"), &name);
				break;
			case BTrace::EFfsFileReplace:
				Printf(_L("Replace %S"), &name);
				break;
			case BTrace::EFfsFileTemp:
				Printf(_L("Open temp %S"), &name);
				break;
			case BTrace::EFfsRename:
			case BTrace::EFfsFileRename:
				Printf(_L("Rename from %S to %S"), &name, &newName);
				break;
			case BTrace::EFfsEntry:
				Printf(_L("RFs::Entry %S"), &name);
				break;
			default:
				Printf(_L("Event %d"), fn);
				break;
			}
		Printf(_L(" from thread %u"), threadId);
		RThread thread;
		TInt err = thread.Open(threadId);
		if (err == KErrNone)
			{
			Printf(_L(" "));
			TFullName name = thread.FullName();
			Write(name);
			thread.Close();
			}
		Printf(_L("\r\n"));
		}
	}
