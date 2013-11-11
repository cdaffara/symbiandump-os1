// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32math.h>

#include "LbsLasUpdateSource.h"
#include "LbsLasNmeaSource.h"
#include "LbsLasCommon.h"

const TReal32  KKnotsToMetersPerSecond = 0.5144444;
const TReal32  KDopToRealScaleFactor = 1.5;

const TInt     KMicroSecondsDigits = 6;

enum TRmcFields
	{
	ERmcString = 0,
	ERmcTimeUtc,
	ERmcStatusIndicator,
	ERmcLatitude,
	ERmcNorthSouth,
	ERmcLongitude,
	ERmcEastWest,
	ERmcKnots,
	ERmcCourse,
	ERmcDate,
	ERmcMagVariation,
	ERmcMagVarEastWest,
	ERmcModeIndicator,		// Some GPS devices (eg Emtac) do not send this field
	ERmcNumFields
	};

enum TGgaFields
	{
	EGgaString = 0,
	EGgaTimeUtc,
	EGgaLatitude,
	EGgaNorthSouth,
	EGgaLongitude,
	EGgaEastWest,
	EGgaGpsQuality,
	EGgaNumSatellitesInUse,
	EGgaHDop,
	EGgaAltitude,
	EGgaAltitudeMeters,
	EGgaGeoidalSeparation,
	EGgaGeoidalSeparationMeters,
	EGgaAgeDifferential,
	EGgaDifferentialStationId,
	EGgaNumFields
	};

_LIT(KLbsLasUpdateSource,"LbsLasUpdateSource");

//************************************************************************************************************
//   CLasUpdateSource
//************************************************************************************************************
CLasUpdateSource* CLasUpdateSource::NewL(MLasUpdateSourceObserver* aObserver)
	{
	CLasUpdateSource* self = new(ELeave) CLasUpdateSource(aObserver);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CLasUpdateSource::CLasUpdateSource(MLasUpdateSourceObserver* aObserver)
	:
	iObserver(aObserver)
	{
	}

void CLasUpdateSource::ConstructL()
	{
	iTimer = CLbsTimer::NewL(this, 1);
	
	iNmeaSource = CLasNmeaSource::NewL(this);
	
	iObserver->HandleDeviceStatusUpdate(TPositionModuleStatus::EDeviceInactive);
	
	iNmeaSource->OpenNmeaSource();

	iObserver->HandleDeviceStatusUpdate(TPositionModuleStatus::EDeviceInitialising);

	//<<TEST>> Turn on monitoring
	//iNmeaSource->MonitorNmeaSentence();
	}

CLasUpdateSource::~CLasUpdateSource()
	{
	delete iTimer;
	delete iNmeaSource;
	}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLasUpdateSource::RequestLocationUpdate(const TTime& aTargetTime)
	{
	LASLOG("CLasUpdateSource::RequestLocationUpdate - Enter");

	iTimer->SetTimer(aTargetTime);
	
	LASLOG("CLasUpdateSource::RequestLocationUpdate - Exit");
	}

void CLasUpdateSource::CancelLocationRequest()
	{
	iTimer->CancelTimer();
	iNmeaSource->CancelMonitor();
	}
	
void CLasUpdateSource::SetPowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode)
	{
	LASLOG("CLasUpdateSource::SetPowerMode - Enter");	

	switch(aMode)
		{
	case CLbsLocationSourceGpsBase::EPowerModeUnknown:
	case CLbsLocationSourceGpsBase::EPowerModeOn:
	case CLbsLocationSourceGpsBase::EPowerModeStandby:
	case CLbsLocationSourceGpsBase::EPowerModeOff:
		break;
	case CLbsLocationSourceGpsBase::EPowerModeClose:
		iObserver->Shutdown();
		break;
	default:
		break;
		}

	LASLOG("CLasUpdateSource::SetPowerMode - Exit");	
	}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLasUpdateSource::HandleTimer(TInt /*aTimerId*/, const TTime& /*aTargetTime*/)
	{
	iObserver->HandleDeviceStatusUpdate(TPositionModuleStatus::EDeviceActive);

	iNmeaSource->MonitorNmeaSentence();
	}
	
void CLasUpdateSource::PublishLocationUpdate(TPositionSatelliteInfo& aInfo)
	{
	LASLOG("CLasUpdateSource::PublishLocationUpdate - Enter");	

	TPosition position;
	aInfo.GetPosition(position);

	TReal hDop = aInfo.HorizontalDoP();

	if (! Math::IsNaN(hDop))
		{
		position.SetHorizontalAccuracy(hDop*KDopToRealScaleFactor);
		}
	
	TReal vDop = aInfo.VerticalDoP();

	if (! Math::IsNaN(vDop))
		{
		position.SetVerticalAccuracy(vDop*KDopToRealScaleFactor);
		}

	position.SetCurrentTime();
		
	TCourse course;
	aInfo.GetCourse(course);
	course.SetSpeedAccuracy(5.0);
	course.SetHeadingAccuracy(10.0);

	//<<TODO>> For the moment just publish course info
	TPositionCourseInfo courseInfo;
	courseInfo.SetPosition(position);
	courseInfo.SetCourse(course);
	
	iObserver->HandleLocationUpdate(courseInfo);
	
	(void) new(&iSatInfo) TPositionSatelliteInfo;
	
	iNmeaSource->CancelMonitor();

	LASLOG("CLasUpdateSource::PublishLocationUpdate - Exit");
	}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLasUpdateSource::ConvertDecimalMinutesToDecimalDegrees(TReal& aCoord)
	{
	// Converts degrees and decimal minutes format (DDMM.MMMM) to
	// decimal degrees (DD.DDDDDD)
	// e.g. converts 5430.0 to 54.5

	TReal minutes;
	Math::Mod(minutes, aCoord, 100.0);

	aCoord -= minutes;
	aCoord /= 100.0;
	aCoord += minutes / 60.0;
	}

void CLasUpdateSource::ExtractValueL(const TPtrC8& aField, TReal32& aValue)
	{
	TRealX nan;
	nan.SetNaN();
	
	aValue = nan;
	
	if (aField.Length() != 0)
		{
		TLex8 lex(aField);
		TInt err = lex.Val(aValue);
		User::LeaveIfError(err);
		}
	}

void CLasUpdateSource::ExtractDegreesL(const TPtrC8& aField, TReal64& aDegrees)
	{
	TRealX nan;
	nan.SetNaN();

	aDegrees = nan;

	if (aField.Length() != 0)
		{
		TLex8 lex(aField);
		TInt err = lex.Val(aDegrees);
		User::LeaveIfError(err);
		
		ConvertDecimalMinutesToDecimalDegrees(aDegrees);
		}
	}

#define DECVAL(D) (D - '0')

void CLasUpdateSource::ExtractTimeL(const TPtrC8& aTime, TDateTime& aTheTime)
	{
	if (aTime.Length() < 6)
	
		{
		User::Leave(KErrArgument);
		}

	// Zero the date
	TTime zeroTime(0);
	aTheTime = zeroTime.DateTime();

	// Time is in HHMMSS.xxx where xxx are the fractions of a sec
	TInt hour  = DECVAL(aTime[0]) * 10 + DECVAL(aTime[1]);
	TInt mins  = DECVAL(aTime[2]) * 10 + DECVAL(aTime[3]);
	TInt secs  = DECVAL(aTime[4]) * 10 + DECVAL(aTime[5]);

	TInt uSecs = 0;

	TInt dot = aTime.Locate('.');
	
	if (dot == 6)  // Time should be in HHMMSS.xxx format
		{
		TPtrC8 fracSecsPtr = aTime.Mid(dot+1);
		TBuf8<KMicroSecondsDigits> uSecsBuf;
		
		// Fill the uSecsBuf with the fractions of a second and pad with trailing zeros
		// Eg "25" would become "250000"
		// After padding, this representing the number of micro seconds.
		//
		uSecsBuf.Justify(fracSecsPtr, KMicroSecondsDigits, ELeft, '0');

		TLex8  uSecsLex(uSecsBuf);
		TInt   err = uSecsLex.Val(uSecs);

		User::LeaveIfError(err);
		}
	
	aTheTime.SetHour(hour);
	aTheTime.SetMinute(mins);
	aTheTime.SetSecond(secs);
	aTheTime.SetMicroSecond(uSecs);
	}

void CLasUpdateSource::ExtractDateL(const TPtrC8& aDate, TDateTime& aTheDate)
	{
	if (aDate.Length() != 6)
		{
		User::Leave(KErrArgument);
		}

	// Zero the date
	TTime zeroTime(0);
	aTheDate = zeroTime.DateTime();

	// Date is in DDMMYY format (i.e. European!)
	TInt day   = DECVAL(aDate[0]) * 10 + DECVAL(aDate[1]);
	TInt month = DECVAL(aDate[2]) * 10 + DECVAL(aDate[3]);
	TInt year  = DECVAL(aDate[4]) * 10 + DECVAL(aDate[5]);
	
	TInt   theYear  = year + 2000;
	TMonth theMonth = static_cast<TMonth>(month-1);
	
	aTheDate.SetDay(day);
	aTheDate.SetMonth(theMonth);
	aTheDate.SetYear(theYear);
	}


void CLasUpdateSource::ExtractRmcDataL
	(
	const TPtrC8* aNmeaFields,
	TInt /*aNumFields*/,
	TPositionSatelliteInfo& aInfo,
	TTime& aRmcTime
	)
	{
	LASLOG("ExtractRmcDataL - Enter");
	TPosition position;
	TCourse   course;

	aInfo.GetPosition(position);
	aInfo.GetCourse(course);

	//TBool isPartial = EFalse;
	//isPartial = aNmeaFields[ERmcStatusIndicator].Compare(_L8("A")) != 0;
	
	TDateTime rmcTime;
	ExtractTimeL(aNmeaFields[ERmcTimeUtc], rmcTime);
	
	// We want to pass back only the time portion contained in the RMC sentence
	aRmcTime = rmcTime;

	TDateTime rmcDate;
	ExtractDateL(aNmeaFields[ERmcDate], rmcDate);

	rmcTime.SetDay  (rmcDate.Day());
	rmcTime.SetMonth(rmcDate.Month());
	rmcTime.SetYear (rmcDate.Year());
	
	TTime satUtcTime = rmcTime;
	satUtcTime.UniversalTime();

	aInfo.SetSatelliteTime(satUtcTime);
		
	TReal64 latitude;
	ExtractDegreesL(aNmeaFields[ERmcLatitude], latitude);
	
	if (! Math::IsNaN(latitude))
		{
		if (aNmeaFields[ERmcNorthSouth].Compare(_L8("S")) == 0) latitude = -latitude;
		}

	TReal64 longitude;	
	ExtractDegreesL(aNmeaFields[ERmcLongitude], longitude);

	if (! Math::IsNaN(longitude))
		{
		if (aNmeaFields[ERmcEastWest].Compare(_L8("W")) == 0) longitude = -longitude;
		}

	TReal32 altitude = position.Altitude();

	position.SetCoordinate(latitude, longitude, altitude);
	LASLOG3("ExtractRmcDataL - Lat: %f, Lon: %f", latitude, longitude);

	TReal32 speed;
	ExtractValueL(aNmeaFields[ERmcKnots], speed);

	if (! Math::IsNaN(speed))
		{
		speed = speed * KKnotsToMetersPerSecond;
		}
	
	TReal32 heading;
	ExtractValueL(aNmeaFields[ERmcCourse], heading);
	
	LASLOG3("ExtractRmcDataL - Speed: %f, Heading: %f", speed, heading);

	course.SetSpeed(speed);
	course.SetHeading(heading);

	aInfo.SetPosition(position);
	aInfo.SetCourse(course);
	
	LASLOG("ExtractRmcDataL - Exit");
	}

void CLasUpdateSource::ExtractGgaDataL
	(
	const TPtrC8* aNmeaFields,
	TInt /*aNumFields*/,
	TPositionSatelliteInfo& aInfo,
	TTime& aGgaTime
	)
	{
	LASLOG("ExtractGgaDataL - Enter");
	TPosition position;

	TDateTime ggaTime;
	ExtractTimeL(aNmeaFields[EGgaTimeUtc], ggaTime);
	aGgaTime = ggaTime;

	aInfo.GetPosition(position);

	//TBool isPartial = EFalse;
	//isPartial = aNmeaFields[EGgaGpsQuality].Compare(_L8("0")) != 0;

	TReal64 latitude;
	ExtractDegreesL(aNmeaFields[EGgaLatitude], latitude);
	if (aNmeaFields[EGgaNorthSouth].Compare(_L8("S")) == 0) latitude = -latitude;

	TReal64 longitude;	
	ExtractDegreesL(aNmeaFields[EGgaLongitude], longitude);
	if (aNmeaFields[EGgaEastWest].Compare(_L8("W")) == 0) longitude = -longitude;

	TReal32 altitude;	
	ExtractValueL(aNmeaFields[EGgaAltitude], altitude);

	position.SetCoordinate(latitude, longitude, altitude);
	LASLOG4("ExtractGgaDataL - Lat: %f, Lon: %f, Alt: %f", latitude, longitude, altitude);

	TReal32 hDop;
	ExtractValueL(aNmeaFields[EGgaHDop], hDop);
	LASLOG2("ExtractGgaDataL - HDop: %f", hDop);
	
	aInfo.SetHorizontalDoP(hDop);
	
	aInfo.SetPosition(position);
	
	LASLOG("ExtractGgaDataL - Exit");
	}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CLasUpdateSource::HandleNmeaSentence(const TPtrC8* aNmeaFields, TInt aNumFields)
	{
	LASLOG("HandleNmeaSentence - Enter");

	LASLOG2("HandleNmeaSentence - NumFields: %d", aNumFields);
	
	for (TInt index = 0; index < aNumFields; index++)
		{
		LASLOGDES(aNmeaFields[index]);
		}
	
	if (aNumFields == 0)
		{
		return;
		}
	
	if (aNmeaFields[0].Compare(_L8("GPRMC")) == 0)
		{
		TInt err = KErrNone;
		TRAP(err, ExtractRmcDataL(aNmeaFields, aNumFields, iSatInfo, iRmcTime);)
		if(err)
			User::Panic(KLbsLasUpdateSource, err);
		
		if (iRmcTime == iGgaTime)
			{
			PublishLocationUpdate(iSatInfo);
			iRmcTime = 0;
			iGgaTime = 0;
			}		
		}

	if (aNmeaFields[0].Compare(_L8("GPGGA")) == 0)
		{
		TInt err = KErrNone;
		TRAP(err, ExtractGgaDataL(aNmeaFields, aNumFields, iSatInfo, iGgaTime);)
		if(err)
			User::Panic(KLbsLasUpdateSource, err);
		

		if (iRmcTime == iGgaTime)
			{
			PublishLocationUpdate(iSatInfo);
			iRmcTime = 0;
			iGgaTime = 0;
			}		
		}

	
	LASLOG("HandleNmeaSentence - Exit");
	}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
