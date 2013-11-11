// btrace_utils.cpp
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

#include <e32math.h>
#include <hal.h>
#include "btrace_parser.h"
#include <fshell/ltkutils.h>

void Panic(TBtraceParserPanic aReason)
	{
	_LIT(KCat, "btrace_parser");
	User::Panic(KCat, aReason);
	}

enum TTlsFlags
	{
	ENanoSet = 1,
	EFastCounterSet = 2,
	EFastCountsUpSet = 4,
	};

struct SBtraceParserTls
	{
	TUint32 iFlags;
	TInt iNanoTickPeriod;
	TInt iFastCounterFrequency;
	TBool iFastCounterCountsUp;
	};

EXPORT_C TUint32 TBtraceUtils::MicroSecondsToNanoTicks(TTimeIntervalMicroSeconds32 aInterval)
	{
	TUint32 numTicks = aInterval.Int();
	numTicks /= NanoTickPeriod();
	return numTicks;
	}

EXPORT_C TUint64 TBtraceUtils::MicroSecondsToFastTicks(TTimeIntervalMicroSeconds32 aInterval)
	{
	TUint64 numTicks = aInterval.Int();
	numTicks *= FastCounterFrequency();
	numTicks /= 1000000;
	return numTicks;
	}

EXPORT_C TTimeIntervalMicroSeconds TBtraceUtils::NanoTicksToMicroSeconds(TUint32 aNanoTicks)
	{
	TUint64 ms = aNanoTicks;
	ms *= NanoTickPeriod();
	return ms;
	}

EXPORT_C TTimeIntervalMicroSeconds TBtraceUtils::FastTicksToMicroSeconds(const TUint64& aFastTicks)
	{
	TUint64 ms = aFastTicks;
	ms *= 1000000;
	TUint64 f = FastCounterFrequency();
	ms /= f;
	return ms;
	}

TInt TBtraceUtils::NanoTickPeriod()
	{
	SBtraceParserTls* tls = (SBtraceParserTls*)Dll::Tls();
	if (tls && tls->iFlags & ENanoSet)
		{
		return tls->iNanoTickPeriod;
		}
	else
		{
		TInt res = CalculateNanoTickPeriod();
		tls = CreateTls();
		if (tls)
			{
			tls->iNanoTickPeriod = res;
			tls->iFlags |= ENanoSet;
			}
		return res;
		}
	}

TInt TBtraceUtils::CalculateNanoTickPeriod()
	{
	TInt nanoTickPeriod;
	if (HAL::Get(HAL::ENanoTickPeriod, nanoTickPeriod) != KErrNone)
		{
		nanoTickPeriod = 1000;
		}
	return nanoTickPeriod;
	}

TInt TBtraceUtils::FastCounterFrequency()
	{
	SBtraceParserTls* tls = (SBtraceParserTls*)Dll::Tls();
	if (tls && tls->iFlags & EFastCounterSet)
		{
		return tls->iFastCounterFrequency;
		}
	else
		{
		TInt freq = CalculateFastCounterFrequency();
		tls = CreateTls();
		if (tls) 
			{
			tls->iFastCounterFrequency = freq;
			tls->iFlags |= EFastCounterSet;
			}
		return freq;
		}
	}

TInt TBtraceUtils::CalculateFastCounterFrequency()
	{
	TInt fastCounterFrequency;
	if (HAL::Get(HAL::EFastCounterFrequency, fastCounterFrequency) != KErrNone)
		{
		fastCounterFrequency = 1000;
		}
	return fastCounterFrequency;
	}

TBool TBtraceUtils::FastCounterCountsUp()
	{
	SBtraceParserTls* tls = (SBtraceParserTls*)Dll::Tls();
	if (tls && tls->iFlags & EFastCountsUpSet)
		{
		return tls->iFastCounterCountsUp;
		}
	else
		{
		TInt res = CalculateFastCounterCountsUp();
		tls = CreateTls();
		if (tls)
			{
			tls->iFastCounterCountsUp = res;
			tls->iFlags |= EFastCountsUpSet;
			}
		return res;
		}
	}

TBool TBtraceUtils::CalculateFastCounterCountsUp()
	{
	TBool countsUp;
	if (HAL::Get(HAL::EFastCounterCountsUp, countsUp) != KErrNone)
		{
		countsUp = EFalse;
		}

	// Hack for N96 which returns countsup=false even though the fast counter is slaved to the nanokernel tick counter (and thus does actually count upwards)
	TInt fastCount = User::FastCounter();
	TInt ntick = User::NTickCount();
	if (FastCounterFrequency() == NanoTickPeriod() && fastCount == ntick)
		{
		countsUp = ETrue;
		}

	return countsUp;
	}

SBtraceParserTls* TBtraceUtils::CreateTls()
	{
	SBtraceParserTls* res = (SBtraceParserTls*)Dll::Tls(); // Sort any reentrancy issues by checking if we are actually created
	if (res) return res;
	res = new SBtraceParserTls;
	if (!res) return NULL;
	Mem::FillZ(res, sizeof(SBtraceParserTls));
	LtkUtils::MakeHeapCellInvisible(res);
	TInt err = Dll::SetTls(res);
	if (err)
		{
		delete res;
		return NULL;
		}
	return res;
	}

EXPORT_C void TBtraceUtils::DebugOverrideTimerSettings(TInt aNanoPeriod, TInt aFastCounterFreq, TBool aFastCountUp)
	{
	SBtraceParserTls* tls = CreateTls();
	if (tls)
		{
		tls->iFlags |= ENanoSet | EFastCounterSet | EFastCountsUpSet;
		tls->iNanoTickPeriod = aNanoPeriod;
		tls->iFastCounterFrequency = aFastCounterFreq;
		tls->iFastCounterCountsUp = aFastCountUp;
		}
	}

EXPORT_C TBtraceTickCount::TBtraceTickCount()
	: iNano(0), iFast(0)
	{
	}

EXPORT_C void TBtraceTickCount::SetToNow()
	{
	iFast = User::FastCounter();
	iNano = User::NTickCount();
	}

EXPORT_C TUint32 TBtraceTickCount::IntervalInNanoTicks(const TBtraceTickCount& aTickCount) const
	{
	__ASSERT_ALWAYS(iNano >= aTickCount.iNano, Panic(EBtpPanicNegativeTickInterval));
	return (iNano - aTickCount.iNano);
	}

EXPORT_C TUint64 TBtraceTickCount::IntervalInFastTicks(const TBtraceTickCount& aTickCount) const
	{
	// Calc the number of nano-kernel ticks (simple subtraction)
	TUint64 n = IntervalInNanoTicks(aTickCount);

	// Convert to microseconds;
	n *= TBtraceUtils::NanoTickPeriod();

	// Convert this into fast counter ticks.
	n *= TBtraceUtils::FastCounterFrequency();
	n /= 1000000;

	// Round n down to the the nearest KMaxTUint. The next bit of maths is concerned with calculating the remainder
	n /= KMaxTUint;
	n *= KMaxTUint;

	/*
	This represents how the fast counter overflows (assuming fast counter counting up):

	|-----|-----|-----| <- tick periods
	     /     /     /
	    /     /     /|
	   /     /     / |
	  /     /     /  |
	 /|    /     /   |
	/ |   /     /    |
	  |              |
	  aTickCount     this
	  
	In the case where aTickCount and this are roughly as presented in the above diagram, the answer
	is (2 tick periods) + this.iFast - aTickCount.iFast

	Where aTickCount.iFast is numerically greater than this.iFast:

  	|-----|-----|-----|-----| <- tick periods
	     /     /     /     /
	    /|    /     /     /
	   / |   /     /     /
	  /  |  /     /     /
	 /   | /     /     /
	/    |/     /     /|
	     |             |
	     aTickCount    this
         

	The sum is slightly different: (2 tick periods) + this.iFast + (tick period - aTickCount.iFast)
	When the counter counts down, the maths is reversed
	*/

	// Now calculate the duration (in fast ticks).
	if (TBtraceUtils::FastCounterCountsUp())
		{
		if (aTickCount.iFast < iFast)
			{
			n = n + iFast - aTickCount.iFast;
			}
		else if (aTickCount.iFast > iFast)
			{
			n = n + iFast + (KMaxTUint - aTickCount.iFast);
			}
		else
			{
			// Do nothing - the fast counts are the numerically the same, so n being a round number of tick periods is already correct
			}
		}
	else
		{
		if (iFast < aTickCount.iFast)
			{
			n = n + aTickCount.iFast - iFast;
			}
		else if (aTickCount.iFast > iFast)
			{
			n = n + aTickCount.iFast + (KMaxTUint - iFast);
			}
		else
			{
			// Do nothing as above
			}
		}

	return n;
	}

EXPORT_C TTimeIntervalMicroSeconds TBtraceTickCount::IntervalInMicroSeconds(const TBtraceTickCount& aTickCount) const
	{
	TUint64 n = IntervalInFastTicks(aTickCount);
	n *= 1000000;
	n /= TBtraceUtils::FastCounterFrequency();
	return n;
	}

EXPORT_C TBool TBtraceTickCount::operator==(const TBtraceTickCount& aTickCount) const
	{
	return (aTickCount.iNano == iNano) && (aTickCount.iFast == iFast);
	}

EXPORT_C TBool TBtraceTickCount::operator!=(const TBtraceTickCount& aTickCount) const
	{
	return (aTickCount.iNano != iNano) || (aTickCount.iFast != iFast);
	}

EXPORT_C TBool TBtraceTickCount::operator>=(const TBtraceTickCount& aTickCount) const
	{
	if ((iNano > aTickCount.iNano) || (*this == aTickCount))
		{
		return ETrue;
		}
	else if (iNano < aTickCount.iNano)
		{
		return EFalse;
		}
	else
		{
		return TBtraceUtils::FastCounterCountsUp() ? (iFast > aTickCount.iFast) : (iFast < aTickCount.iFast);
		}
	}

EXPORT_C TBool TBtraceTickCount::operator<=(const TBtraceTickCount& aTickCount) const
	{
	if ((iNano < aTickCount.iNano) || (*this == aTickCount))
		{
		return ETrue;
		}
	else if (iNano > aTickCount.iNano)
		{
		return EFalse;
		}
	else
		{
		return TBtraceUtils::FastCounterCountsUp() ? (iFast < aTickCount.iFast) : (iFast > aTickCount.iFast);
		}
	}

EXPORT_C TBool TBtraceTickCount::operator>(const TBtraceTickCount& aTickCount) const
	{
	if (iNano > aTickCount.iNano)
		{
		return ETrue;
		}
	else if ((*this == aTickCount) || (iNano < aTickCount.iNano))
		{
		return EFalse;
		}
	else
		{
		return TBtraceUtils::FastCounterCountsUp() ? (iFast > aTickCount.iFast) : (iFast < aTickCount.iFast);
		}
	}

EXPORT_C TBool TBtraceTickCount::operator<(const TBtraceTickCount& aTickCount) const
	{
	if (iNano < aTickCount.iNano)
		{
		return ETrue;
		}
	else if ((*this == aTickCount) || (iNano > aTickCount.iNano))
		{
		return EFalse;
		}
	else
		{
		return TBtraceUtils::FastCounterCountsUp() ? (iFast < aTickCount.iFast) : (iFast > aTickCount.iFast);
		}
	}

EXPORT_C void CRefCountedObject::IncRef()
	{
	++iRefCount;
	}

EXPORT_C void CRefCountedObject::DecRef()
	{
	__ASSERT_ALWAYS(iRefCount > 0, Panic(EBtpPanicNegativeRefCount));
	--iRefCount;
	}

EXPORT_C TInt CRefCountedObject::RefCount() const
	{
	return iRefCount;
	}

CRefCountedObject::CRefCountedObject()
	{
	}
