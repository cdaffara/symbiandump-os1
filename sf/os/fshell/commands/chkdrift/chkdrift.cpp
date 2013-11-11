// chkdrift.cpp
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

#include <hal.h>
#include <fshell/ioutils.h>

using namespace IoUtils;

class CCmdChkdrift : public CCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdChkdrift();
private:
	CCmdChkdrift();
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	};


CCommandBase* CCmdChkdrift::NewLC()
	{
	CCmdChkdrift* self = new(ELeave) CCmdChkdrift();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

CCmdChkdrift::~CCmdChkdrift()
	{
	}

CCmdChkdrift::CCmdChkdrift()
	{
	}

const TDesC& CCmdChkdrift::Name() const
	{
	_LIT(KName, "chkdrift");	

	return KName;
	}

TInt NanoTickPeriod()
	{
	TInt nanoTickPeriod;
	if (HAL::Get(HAL::ENanoTickPeriod, nanoTickPeriod) != KErrNone)
		{
		nanoTickPeriod = 1000;
		}
	return nanoTickPeriod;
	}

TInt FastCounterFrequency()
	{
	TInt fastCounterFrequency;
	if (HAL::Get(HAL::EFastCounterFrequency, fastCounterFrequency) != KErrNone)
		{
		fastCounterFrequency = 1000;
		}
	return fastCounterFrequency;
	}

TBool FastCounterCountsUp()
	{
	TBool countsUp;
	if (HAL::Get(HAL::EFastCounterCountsUp, countsUp) != KErrNone)
		{
		countsUp = EFalse;
		}
	return countsUp;
	}

void FormatTime(const TTimeIntervalMicroSeconds& aInterval, TDes& aBuf)
	{
	const TInt ms = 1000;
	const TInt s = ms * ms;

	TBuf<16> format(_L("%.2f "));

	TReal interval = aInterval.Int64();
	if (interval >= s)
		{
		interval /= s;
		format.Append(_L("s"));
		}
	else if (interval >= ms)
		{
		interval /= ms;
		format.Append(_L("ms"));
		}
	else
		{
		format.Append(_L("us"));
		}

	aBuf.Format(format, interval);
	}

void CCmdChkdrift::DoRunL()
	{
	TInt nanoTickPeriod = NanoTickPeriod();
	Printf(_L("NKern tick period: %d\r\n"), nanoTickPeriod);
	TInt fastCounterFrequency = FastCounterFrequency();
	Printf(_L("Fast counter frequency: %d\r\n\r\n"), fastCounterFrequency);

	Printf(_L("Press any key to start test\r\n"));
	TBuf<1> key;
	ReadL(key);
	TUint fast1 = User::FastCounter();
	TUint nano1 = User::NTickCount();

	Printf(_L("Press any key to stop test\r\n"));
	ReadL(key);
	TUint fast2 = User::FastCounter();
	TUint nano2 = User::NTickCount();

	Printf(_L("before: nano: %u, fast: %u\r\n"), nano1, fast1);
	Printf(_L("after: nano: %u, fast: %u\r\n"), nano2, fast2);
	TUint64 diffNano = nano2 - nano1;
	diffNano *= NanoTickPeriod();
	TUint64 diffFast;
	
	if (FastCounterCountsUp())
		{
		diffFast = fast2 - fast1;
		}
	else
		{
		diffFast = fast1 - fast2;
		}

	TUint64 fastCounterPeriod = 1000000 / FastCounterFrequency();
	diffFast *= fastCounterPeriod;
	TBuf<32> timeNano;
	FormatTime(diffNano, timeNano);
	TBuf<32> timeFast;
	FormatTime(diffFast, timeFast);
	Printf(_L("difference:\r\n\tnano: %u ticks (%Lu us, %S)\r\n\tfast: %u ticks (%Lu us, %S)\r\n"), nano2 - nano1, diffNano, &timeNano, fast2 - fast1, diffFast, &timeFast);
	}


EXE_BOILER_PLATE(CCmdChkdrift)

