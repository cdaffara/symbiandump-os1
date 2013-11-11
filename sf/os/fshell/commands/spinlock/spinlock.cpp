// spinlock.cpp
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

#include <fshell/memoryaccesscmd.h>
#include <fshell/ioutils.h>
#include <HAL.H>

using namespace IoUtils;

class CCmdSpinlock : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdSpinlock();
private:
	CCmdSpinlock();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
	virtual void OptionsL(RCommandOptionList& aOptions);
private:
	TInt iStartIn;
	TInt iDuration;
	TInt iLoad;
	TInt iPriority;
	volatile TInt iCounter;
	};


CCommandBase* CCmdSpinlock::NewLC()
	{
	CCmdSpinlock* self = new(ELeave) CCmdSpinlock();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdSpinlock::~CCmdSpinlock()
	{
	}

CCmdSpinlock::CCmdSpinlock()
	{
	}

const TDesC& CCmdSpinlock::Name() const
	{
	_LIT(KName, "spinlock");
	return KName;
	}

void CCmdSpinlock::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendIntL(iLoad, _L("load"));
	}

void CCmdSpinlock::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendIntL(iStartIn, _L("start-after"));
	aOptions.AppendIntL(iDuration, _L("duration"));
	aOptions.AppendIntL(iPriority, _L("priority"));
	}

EXE_BOILER_PLATE(CCmdSpinlock)

void CCmdSpinlock::DoRunL()
	{
	if (iPriority)
		{
#ifdef FSHELL_MEMORY_ACCESS_SUPPORT
		LoadMemoryAccessL();

		RThread me;
		CleanupClosePushL(me);
		LeaveIfErr(me.Open(me.Id()), _L("Couldn't open myself (!?!)"));
		LeaveIfErr(iMemAccess.SetThreadPriority(me, iPriority), _L("Couldn't set priority"));
		CleanupStack::PopAndDestroy(&me);
#else
		PrintWarning(_L("--priority is not supported if memoryaccess is not available."));
#endif
		}

	if (iStartIn)
		{
		Printf(_L("Starting in %d seconds...\r\n"), iStartIn);
		User::After(iStartIn * 1000000);
		}

	if (iDuration)
		{
		Printf(_L("Starting to spin for %d seconds... (Press CTRL-C to exit earlier)\r\n"), iDuration);
		}
	else
		{
		Printf(_L("Starting to spin... (Press CTRL-C to exit)\r\n"));
		}

	// We have ~300,000 cycles per millisecond to play with. We will do some amount of work then wait a millisecond.
	// This is pretty arbitrary.

	TInt tickPeriod;
	HAL::Get(HAL::ENanoTickPeriod, tickPeriod);

	TInt endTime; // in nkern ticks
	if (iDuration)
		{
		endTime = User::NTickCount() + (iDuration * 1000000)/tickPeriod;
		}
	else
		{
		endTime = KMaxTInt;
		}

	if (iLoad)
		{
		//TODO this loop needs sorting out. The User::After is a bit broken.
		TInt n = iLoad * 1000;
		while (User::NTickCount() < (TUint)endTime)
			{
			for (iCounter = 0; iCounter < n; iCounter++)
				{
				// Let's do an exec call just for fun
				UserSvr::DebugMask(0);
				}

			User::After(1000); // Wait 1 ms
			}
		}
	else
		{
		// Just spin
		while (User::NTickCount() < (TUint)endTime)
			{
			}
		}
	}
