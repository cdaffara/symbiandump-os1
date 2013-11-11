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

#include "devvideobase.h"
#include "devvideoconstants.h"

//Include the following headers here just to check they can be compiled OK. 
#include "Mpeg4Visual.h"
#include "AVC.h"
#include "H263.h"
#include "vc1.h"
#include "on2vp6.h"
#include "sorensonspark.h"

EXPORT_C CCompressedVideoFormat* CCompressedVideoFormat::NewL(const TDesC8& aMimeType,const TDesC8& aOptionalData)
	{
	CCompressedVideoFormat* s = new(ELeave) CCompressedVideoFormat;
	CleanupStack::PushL(s);
	s->ConstructL(aMimeType, aOptionalData);
	CleanupStack::Pop(s);
	return s;
	}

EXPORT_C CCompressedVideoFormat* CCompressedVideoFormat::NewL(const CCompressedVideoFormat& aFormat)
	{
	CCompressedVideoFormat* s = new(ELeave) CCompressedVideoFormat;
	CleanupStack::PushL(s);
	s->ConstructL(aFormat.MimeType(), aFormat.OptionalData());
	CleanupStack::Pop(s);
	return s;
	}

EXPORT_C CCompressedVideoFormat::~CCompressedVideoFormat()
	{
	delete iMimeType;
	delete iOptionalData;
	}

EXPORT_C const TDesC8& CCompressedVideoFormat::MimeType() const
	{
	return *iMimeType;
	}

EXPORT_C const TDesC8& CCompressedVideoFormat::OptionalData() const
	{
	return *iOptionalData;
	}

CCompressedVideoFormat::CCompressedVideoFormat()
	{
	}

void CCompressedVideoFormat::ConstructL(const TDesC8& aMimeType, const TDesC8& aOptionalData)
	{
	iMimeType = aMimeType.AllocL();
	iOptionalData = aOptionalData.AllocL();
	}

EXPORT_C TBool CCompressedVideoFormat::operator==(const CCompressedVideoFormat& aOther) const
	{
	TBool retval = EFalse;
	// only test optionalData if aOther has an optional data of length > 0.  Need a
	// check like this for performing matches on mimetype only.
	if (aOther.OptionalData().Length() > 0)
		retval = ((aOther.MimeType().CompareF(*iMimeType) == 0) && (aOther.OptionalData().CompareF(*iOptionalData) == 0));
	else
		retval = ((aOther.MimeType().CompareF(*iMimeType) == 0));
	return retval;
	}



EXPORT_C CSystemClockSource* CSystemClockSource::NewL()
	{
	CSystemClockSource* s = new(ELeave) CSystemClockSource;
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return s;
	}

CSystemClockSource::CSystemClockSource()
	{
	iStartTime.UniversalTime();
	iTimeSuspended = 0;
	}

void CSystemClockSource::ConstructL()
	{
	User::LeaveIfError(iCriticalSection.CreateLocal());
	}

EXPORT_C CSystemClockSource::~CSystemClockSource()
	{
	iCriticalSection.Close();
	}

TAny* CSystemClockSource::CustomInterface(TUid /*aInterface*/)
	{
	return NULL;
	}

EXPORT_C void CSystemClockSource::Reset()
	{
	iCriticalSection.Wait();
	iStartTime.UniversalTime();
	iOffset = 0;
	iTimeSuspended = 0;
	iCriticalSection.Signal();
	}

EXPORT_C void CSystemClockSource::Reset(const TTimeIntervalMicroSeconds& aOffset)
	{
	iCriticalSection.Wait();
	iStartTime.UniversalTime();
	iOffset = aOffset;
	iTimeSuspended = 0;
	iCriticalSection.Signal();
	}

TTimeIntervalMicroSeconds CSystemClockSource::Time()
	{
	iCriticalSection.Wait();
	TTimeIntervalMicroSeconds elapsed(0);
	if (!iSuspended)
		{
		iCurrentTime.UniversalTime();
		elapsed = iCurrentTime.MicroSecondsFrom(iStartTime);
		}
	else
		{
		// If we're currently suspended, current time == time when we were suspended
		elapsed = iTimeWhenSuspended.MicroSecondsFrom(iStartTime);
		}

	// Perceived elapsed time == true elapsed + offset - time spent suspended
	TInt64 time = elapsed.Int64() + iOffset.Int64() - iTimeSuspended.Int64();
	iCriticalSection.Signal();
	return time;
	}

EXPORT_C void CSystemClockSource::Suspend()
	{
	__ASSERT_DEBUG(!iSuspended, DevVideoPanic(EDevVideoPanicPreConditionViolation));
	iCriticalSection.Wait();
	iTimeWhenSuspended.UniversalTime();
	iSuspended = ETrue;
	iCriticalSection.Signal();
	}

EXPORT_C void CSystemClockSource::Resume()
	{
	__ASSERT_DEBUG(iSuspended, DevVideoPanic(EDevVideoPanicPreConditionViolation));
	iCriticalSection.Wait();
	iSuspended = EFalse;
	iCurrentTime.UniversalTime();
	iTimeSuspended = iTimeSuspended.Int64() + iCurrentTime.MicroSecondsFrom(iTimeWhenSuspended).Int64();
	iCriticalSection.Signal();
	}


EXPORT_C CMMFClockSourcePeriodicUtility* CMMFClockSourcePeriodicUtility::NewL(MMMFClockSource& aClockSource, MMMFClockSourcePeriodicUtilityObserver& aObserver)
	{
	CMMFClockSourcePeriodicUtility* s = new(ELeave) CMMFClockSourcePeriodicUtility(aClockSource, aObserver);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return s;
	}

CMMFClockSourcePeriodicUtility::CMMFClockSourcePeriodicUtility(MMMFClockSource& aClockSource, MMMFClockSourcePeriodicUtilityObserver& aObserver) :
	iClockSource(aClockSource),
	iObserver(aObserver)
	{
	}

void CMMFClockSourcePeriodicUtility::ConstructL()
	{
	iTimer = CPeriodic::NewL(EPriorityNormal);
	}

EXPORT_C void CMMFClockSourcePeriodicUtility::Start(TTimeIntervalMicroSeconds32 aPeriod)
	{
	TCallBack callback(CMMFClockSourcePeriodicUtility::Callback, this);
	iTimer->Start(aPeriod, aPeriod, callback);
	}

EXPORT_C void CMMFClockSourcePeriodicUtility::Stop()
	{
	iTimer->Cancel();
	}

EXPORT_C CMMFClockSourcePeriodicUtility::~CMMFClockSourcePeriodicUtility()
	{
	delete iTimer;
	}

TInt CMMFClockSourcePeriodicUtility::Callback(TAny* aAny)
	{
	CMMFClockSourcePeriodicUtility* me = reinterpret_cast<CMMFClockSourcePeriodicUtility*>(aAny);
	me->DoCallback();
	return KErrNone;
	}

void CMMFClockSourcePeriodicUtility::DoCallback()
	{
	iObserver.MmcspuoTick(iClockSource.Time());
	}


