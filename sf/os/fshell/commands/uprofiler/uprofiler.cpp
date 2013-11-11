// uprofiler.cpp
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/ioutils.h>
#include <fshell/extrabtrace.h>

using namespace IoUtils;

class CCmdUprofiler : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdUprofiler();
private:
	CCmdUprofiler();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	enum TCommand
		{
		EStart = 0,
		EStop = 1,
		};
	TCommand iCommand;
	TInt iRate; // In Hz, just for fun
	RExtraBtrace iBtrace;
	TBool iVerbose;
	};

CCommandBase* CCmdUprofiler::NewLC()
	{
	CCmdUprofiler* self = new(ELeave) CCmdUprofiler();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdUprofiler::~CCmdUprofiler()
	{
	iBtrace.Close();
	}

CCmdUprofiler::CCmdUprofiler()
	: iRate(1000)
	{
	}

const TDesC& CCmdUprofiler::Name() const
	{
	_LIT(KName, "uprofiler");	
	return KName;
	}

void CCmdUprofiler::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendEnumL((TInt&)iCommand, _L("command"));
	aArguments.AppendIntL(iRate, _L("rate"));
	}

void CCmdUprofiler::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, _L("verbose"));
	}

EXE_BOILER_PLATE(CCmdUprofiler)

void CCmdUprofiler::DoRunL()
	{
	if (iRate <= 0 || iRate > 1000) LeaveIfErr(KErrArgument, _L("rate must be between 1 and 1000"));

	TInt ms = 1000 / iRate;
	LeaveIfErr(iBtrace.Connect(), _L("Couldn't connect to RExtraBtrace"));
	if (iCommand == EStart)
		{
		LeaveIfErr(iBtrace.EnableProfiling(ms), _L("Couldn't enable profiling"));
		}
	else
		{
		LeaveIfErr(iBtrace.EnableProfiling(0), _L("Couldn't disable profiling"));
		}
	}
