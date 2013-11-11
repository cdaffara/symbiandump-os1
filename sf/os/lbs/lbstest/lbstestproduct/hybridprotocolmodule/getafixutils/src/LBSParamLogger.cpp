/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
//
// LBSParamLogger.cpp
//

#define ENABLE_LBS_DEV_LOGGER

#include "LBSParamLogger.h"
#include "LBSTestLogger.h"

#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetclasstypes.h>
#include <lbs/lbsgpsmeasurement.h>
#include <lbs/lbsx3p.h>
#include <lbs/lbslocclasstypes.h>

#include <e32math.h>

//-------------------------------------------------------------------------------------------------

#define KLogPriority  ELogP5

const TInt KLogBufSize = 128;

_LIT(KTimeFormatStr,"(TTime):%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%.%*C4%:3%+B;"); // dd/mm/yyyy hh:mm:ss:uuuu

//
// # paramName(Type):value; for simple types and
// # paramName(Type):{ ... }; for compound types and
// # paramName(Type[count]):a1,a2...aN; for arrays
//

//------------------------------------------------------------------------------------------------

void Output(const TBufBase16& aPtr)
	{
	LbsTestLogger::Write(ELogNormal, KLogPriority, aPtr);
	}
	
void Output(const TPtrC& aPtr)
	{
	LbsTestLogger::Write(ELogNormal, KLogPriority, aPtr);
	}

//------------------------------------------------------------------------------------------------

void Write(const TDesC8& aDes)
	{
	Output(_L("(TDesC8):\""));
	LbsTestLogger::Write(ELogNormal, KLogPriority, aDes);
	Output(_L("\";"));
	}
	
void Write(const TDesC16& aDes)
	{
	Output(_L("(TDesC16):\""));
	LbsTestLogger::Write(ELogNormal, KLogPriority, aDes);
	Output(_L("\";"));
	}
		
void Write(TInt aInt)
	{
	TBuf<KLogBufSize> buf;
	buf.Format(_L("(TInt):%d;"), aInt);	
	Output(buf);
	}

void Write(TUint32 aInt)
	{
	TBuf<KLogBufSize> buf;
	buf.Format(_L("(TUint32):0x%08x;"), aInt);	
	Output(buf);
	}
	
void Write(TInt32 aInt)
	{
	TBuf<KLogBufSize> buf;
	buf.Format(_L("(TInt32):%d;"), aInt);	
	Output(buf);	
	}

void Write(bool aBool)
	{
	if(aBool)
		Output(_L("(TBool):true;"));
	else
		Output(_L("(TBool):false;"));
	}

void Write(const TTimeIntervalMicroSeconds& aValue)
	{
	TBuf<KLogBufSize> buf;
	buf.Format(_L("(TTimeIntervalMicroSeconds):%ld;"), aValue.Int64());	
	Output(buf);
	}

void Write(const TReal32& aValue)
	{
	TBuf<KLogBufSize> buf;
	buf.Format(_L("(TReal32):%e;"), aValue);	
	Output(buf);
	}
	
void Write(const TReal64& aValue)
	{
	TBuf<KLogBufSize> buf;
	
	buf.Format(_L("(TReal64):%E;"), aValue);	
	Output(buf);
	}

void Write(const TInt64& aInt)
	{
	TBuf<KLogBufSize> buf;
	buf.Format(_L("(TInt64):%ld;"), aInt);	
	Output(buf);		
	}

void Write(const TUid& aInt)
	{
	TBuf<KLogBufSize> buf;
	buf.Format(_L("(TUid):0x%08x;"), aInt);	
	Output(buf);		
	}

void Write(const TTime& aTime)
	{
	TBuf<KLogBufSize> buf;
	aTime.FormatL(buf, KTimeFormatStr);
	Output(buf);		
	}
	
void Write(const TUid* aArray, TInt aCount)
	{
	TBuf<KLogBufSize> buf;
	buf.Format(_L("(TUid[%d]:{"), aCount);
	Output(buf);
	
	_LIT(KWriteStop, ",");
	_LIT(KWriteNone, "");
	for(TInt i = 0; i < aCount; ++i)
		{
		buf.Format(_L("0x%08x%S"), aArray[i].iUid, (i < (aCount - 1) ? &KWriteStop : &KWriteNone)); 
		Output(buf);
		}
	Output(_L("};"));
	}
	
void Write(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aValue)
	{
	TBuf<KLogBufSize> buf;
	buf.Format(_L("(TLbsPrivacyResponse):%d;"), aValue);	
	Output(buf);		
	}

void Write(const CLbsNetworkProtocolBase::TLbsSystemStatus& aValue)
	{
	TBuf<KLogBufSize> buf;
	buf.Format(_L("(TLbsSystemStatus):%d;"), aValue);	
	Output(buf);		
	}

void Write(const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aValue)
	{
	TBuf<KLogBufSize> buf;
	buf.Format(_L("(TLbsNetProtocolService):%d;"), aValue);	
	Output(buf);		
	}

//------------------------------------------------------------------------------------------------

void Write(const TLbsNetSessionId& aValue)
	{
	Output(_L("(TLbsNetSessionId):{"));
	
	Output(_L("SessionOwner"));
	Write(aValue.SessionOwner().iUid);
	
	Output(_L("SessionNum"));
	Write(aValue.SessionNum());
	
	Output(_L("};"));
	}
	

void Write(const TLbsNetPosRequestPrivacy& aValue)
	{
	Output(_L("(TLbsNetPosRequestPrivacy):{"));
	
	Output(_L("RequestAdvice"));
	Write(aValue.RequestAdvice());
	
	Output(_L("RequestAction"));
	Write(aValue.RequestAction());
	
	Output(_L("};"));
	}

void Write(const TLbsNetPosRequestQuality& aValue)
	{
	Output(_L("(TLbsNetPosRequestQuality):{"));
	
	Output(_L("MaxFixTime"));
	Write(aValue.MaxFixTime());
	
	Output(_L("MaxFixAge"));
	Write(aValue.MaxFixAge());
	
	Output(_L("MinHorizontalAccuracy"));
	Write(aValue.MinHorizontalAccuracy());
	
	Output(_L("MinVerticalAccuracy"));
	Write(aValue.MinVerticalAccuracy());
	
	Output(_L("};"));
	}


void Write(const TLbsNetPosRequestOptions& aValue)
	{
	Output(_L("(TLbsNetPosRequestOptions):{"));

	TLbsNetPosRequestQuality q;
	aValue.GetRequestQuality(q);
	Output(_L("RequestQuality"));
	Write(q);

	Output(_L("NewClientConnected"));
	Write((aValue.NewClientConnected())?true:false); 
	
	Output(_L("};"));
	}

void Write(const TLbsNetPosRequestOptionsAssistance& aValue)
	{
	Output(_L("(TLbsNetPosRequestOptionsAssistance):{"));

	Output(_L("DataRequestMask"));
	Write(aValue.DataRequestMask());
	
	Output(_L("PosMode"));
	Write(aValue.PosMode());
			
	Output(_L("};"));
	}

void Write(const TLbsNetPosMethod& aValue)
	{
	Output(_L("(TLbsNetPosMethod):{"));

	Output(_L("PosMeans"));
	Write(aValue.PosMeans());
	
	Output(_L("PosMode"));
	Write(aValue.PosMode());
	
	Output(_L("};"));
	}

void Write(const TLbsNetPosMethod* aArray, TInt aCount)
	{
	TBuf<KLogBufSize> buf;
	
	buf.Format(_L("(TLbsNetPosMethod[%d] (PosMeans, PosMode)):{ "), aCount);
	Output(buf);
	_LIT(KWriteStop, ",");
	_LIT(KWriteNone, "");
	
	for(TInt i = 0; i < aCount; ++i)
		{
		buf.Format(_L("{0x%08x, %d}%S"), aArray[i].PosMeans(), aArray[i].PosMode(),(i < (aCount - 1) ? &KWriteStop : &KWriteNone)); 
		Output(buf);
		}
	Output(_L("};"));
	}


void Write(const TLbsNetPosCapabilities& aValue)
	{
	Output(_L("(TLbsNetPosCapabilities):{"));

	//--------------------------------------------------------------
	TInt count1 = aValue.NumPosProtocols();
	TUid *m = new TUid[count1];
	
	if(!m)
		{
		Output(_L("ERROR"));
		}
	else
		{
		TUid *n = m;
		
		for(TInt i = 0; i< count1; ++i)
			{
			aValue.GetPosProtocol(i, *n++);
			}
		Write(m, count1);
		delete [] m;
		}

	//--------------------------------------------------------------
	TInt count2 = aValue.NumPosMethods();
	TLbsNetPosMethod *p = new TLbsNetPosMethod[count2];
	
	if(!p)
		{
		Output(_L("ERROR"));
		}
	else
		{
		TLbsNetPosMethod *q = p;
		
		for(TInt i = 0; i< count2; ++i)
			{
			aValue.GetPosMethod(i, *q++);
			}
		Write(p, count2);
		delete [] p;
		}
	//--------------------------------------------------------------
	
	Output(_L("};"));
	}

void Write(const TLbsNetPosRequestMethod& aValue)
	{
	Output(_L("(TLbsNetPosRequestMethod):{"));

	TInt count = aValue.NumPosMethods();
	TLbsNetPosMethod *p = new TLbsNetPosMethod[count];
	
	if(!p)
		{
		Output(_L("ERROR"));
		}
	else
		{
		TLbsNetPosMethod *q = p;
		
		for(TInt i = 0; i< count; ++i)
			{
			aValue.GetPosMethod(i, *q++);
			}
		Write(p, count);
		delete []p;
		}
	
	Output(_L("};"));
	}


void Write(const TLbsExternalRequestInfo& aValue)
	{
	Output(_L("(TLbsExternalRequestInfo):{"));

		do
		{
		TLbsRequesterId requesterId;
		aValue.GetRequesterId(requesterId);
		Output(_L("RequesterId"));
		Write(requesterId);
		}while(0); // To reduce stack usage... likewise the following params

		do
		{	
		TLbsClientName clientName;
		aValue.GetClientName(clientName);
		Output(_L("ClientName"));
		Write(clientName);
		}while(0);
		
		do
		{	
    	TLbsClientExternalId clientExternalId; 
    	aValue.GetClientExternalId(clientExternalId);
    	Output(_L("ClientExternalId"));
    	Write(clientExternalId);
		}while(0);

	Output(_L("};"));
	}

void Write(const TCourse& aValue)
	{
	Output(_L("(TCourse):{"));
	
	Output(_L("Speed"));
	Write(aValue.Speed());
	
	Output(_L("Heading"));
	Write(aValue.Heading());
	
	Output(_L("VerticalSpeed"));
	Write(aValue.VerticalSpeed());
	
	Output(_L("Course"));
	Write(aValue.Course());
	
	Output(_L("SpeedAccuracy"));
	Write(aValue.SpeedAccuracy());
	
	Output(_L("HeadingAccuracy"));
	Write(aValue.HeadingAccuracy());

	Output(_L("VerticalSpeedAccuracy"));
	Write(aValue.VerticalSpeedAccuracy());
	
	Output(_L("CourseAccuracy"));
	Write(aValue.CourseAccuracy());
		
	Output(_L("};"));
	}

void Write(const TSatelliteData& aValue)
	{
	Output(_L("(TSatelliteData):{"));
	
	Output(_L("SatelliteId"));
	Write(aValue.SatelliteId());

	Output(_L("Azimuth"));
	Write(aValue.Azimuth());
	
	Output(_L("Elevation"));
	Write(aValue.Elevation());

 	Output(_L("IsUsed"));
  	Write((aValue.IsUsed())?true:false);

 	Output(_L("SignalStrength"));
 	Write(aValue.SignalStrength());
	
	Output(_L("};"));	
	}
	
//-------------------------------------------------------------------------------------------------
// Handle inheritance: TCoordinate <-- TLocality <-- TPosition

void WriteBase(const TCoordinate& aValue)
	{
	Output(_L("Latitude"));
	Write(aValue.Latitude());

	Output(_L("Longitude"));
	Write(aValue.Longitude());
	
	Output(_L("Altitude"));
	Write(aValue.Altitude());
	
	Output(_L("Datum"));
	Write(aValue.Datum());
	}
	
void WriteBase(const TLocality& aValue)
	{
	WriteBase((const TCoordinate&)aValue);
	
	Output(_L("HorizontalAccuracy"));
	Write(aValue.HorizontalAccuracy());
	
	Output(_L("VerticalAccuracy"));
	Write(aValue.VerticalAccuracy());
	}

void WriteBase(const TPosition& aValue)
	{
	WriteBase((const TLocality&) aValue);
	
	Output(_L("Time"));
	Write(aValue.Time());
	}
	
void Write(const TPosition& aValue)
	{
	Output(_L("(TPosition):{"));
	WriteBase(aValue);
	Output(_L("};"));
	}


//-------------------------------------------------------------------------------------------------
//  Handle inheritance: TPositionInfoBase <- TPositionInfo <-- TPositionCourseInfo <-- TPositionSatelliteInfo

void WriteBase(const TPositionInfoBase& aValue)
	{
	Output(_L("ModuleId"));
	Write(aValue.ModuleId());
	
	Output(_L("UpdateType"));
	Write(aValue.UpdateType());
	
	Output(_L("PositionMode"));
	Write(aValue.PositionMode());
	
	Output(_L("PositionModeReason"));
	Write(aValue.PositionModeReason());
	}

void WriteBase(const TPositionInfo& aValue)
	{
	WriteBase((const TPositionInfoBase&)aValue);
	
	TPosition pos;
	aValue.GetPosition(pos);
	
	Output(_L("Position"));
	Write(pos);
	}
	
void WriteBase(const TPositionCourseInfo& aValue)
	{
	WriteBase((const TPositionInfo&) aValue);
	
	TCourse course;
	aValue.GetCourse(course);
	
	Output(_L("Course"));
	Write(course);
	}

void Write(const TSatelliteData* aArray, TInt aCount)
	{
	TBuf<KLogBufSize> buf;
	
	buf.Format(_L("(TPositionSatelliteInfo[%d] (SatelliteId, Azimuth,"), aCount);
 	Output(buf);
	Output(_L("Elevation, IsUsed, SignalStrength)):{"));
	
	_LIT(KWriteTrue, "true");
	_LIT(KWriteFalse, "false");
	_LIT(KWriteStop, ",");
	_LIT(KWriteNone, "");
	for(TInt i = 0; i < aCount; ++i)
		{
		buf.Format(_L("{%d, %e, %e, %S, %d}%S"), aArray[i].SatelliteId(), aArray[i].Azimuth(), 
												 aArray[i].Elevation(), ((aArray[i].IsUsed())?&KWriteTrue:&KWriteFalse),
												 aArray[i].SignalStrength(), (i < (aCount - 1) ? &KWriteStop : &KWriteNone)); 

		Output(buf);
		}
	Write(_L("};"));
	}

void WriteBase(const TPositionSatelliteInfo& aValue)
	{
	WriteBase((const TPositionCourseInfo&)aValue);

	Output(_L("SatellitesInUse"));	
	TInt count = aValue.NumSatellitesInView();
	TSatelliteData* satelliteDataArray = new TSatelliteData[count];
	
	if(!satelliteDataArray)
		{
		Output(_L("ERROR"));
		}
	else
		{
		TSatelliteData* currentSatelliteDataItem = satelliteDataArray;
		
		for(TInt i = 0; i< count; ++i)
			{
			aValue.GetSatelliteData(i, *currentSatelliteDataItem);
			++currentSatelliteDataItem;
			}
		Write(satelliteDataArray, count);
		delete [] satelliteDataArray;
		}

	Output(_L("NumSatellitesUsed"));
	Write(aValue.NumSatellitesUsed());
 
	Output(_L("SatelliteTime"));
	Write(aValue.SatelliteTime());
 
	Output(_L("HorizontalDoP"));
	Write(aValue.HorizontalDoP());

	Output(_L("VerticalDoP"));
	Write(aValue.VerticalDoP());

	Output(_L("TimeDoP"));
	Write(aValue.TimeDoP());	
	}

void Write(const TPositionGpsMeasurementData* aArray, TInt aCount)
	{
	TBuf<KLogBufSize> buf;
	
	buf.Format(_L("(TPositionGpsMeasurementData[%d] (SatelliteId, CarrierNoiseRatio, "), aCount);
	Output(buf);
	Output(_L("Doppler, WholeGpsChips, FractionalGpsChips, "));
	Output(_L("MultiPathIndicator, PseudoRangeRmsError)):{"));
	
	_LIT(KWriteStop, ",");
	_LIT(KWriteNone, "");
	for(TInt i = 0; i < aCount; ++i)
		{
		buf.Format(_L("{%d, %d, %d, %d, %d, %d, %u}%S"), aArray[i].SatelliteId(), aArray[i].CarrierNoiseRatio(), 
											    		aArray[i].Doppler(), aArray[i].WholeGpsChips(), 
														aArray[i].FractionalGpsChips(), aArray[i].MultiPathIndicator(),
														aArray[i].PseudoRangeRmsError(), (i < (aCount - 1) ? &KWriteStop : &KWriteNone)); 	
		Output(buf);
		}
	Output(_L("};"));
	}


void WriteBase(const TPositionGpsMeasurementInfo& aValue)
	{
	WriteBase((const TPositionInfoBase&)aValue);
	
	TInt count = aValue.NumMeasurements();
	TPositionGpsMeasurementData* measurementArray = new TPositionGpsMeasurementData[count];
	
	if(!measurementArray)
		{
		Output(_L("ERROR"));
		}
	else
		{
		TPositionGpsMeasurementData* curentMeasurementItem = measurementArray;
		
		for(TInt i = 0; i< count; ++i)
			{
			aValue.GetMeasurementData(i, *curentMeasurementItem);
			++curentMeasurementItem;
			}
		Write(measurementArray, count);
		delete [] measurementArray;
		}

	Output(_L("GpsTimeOfWeek"));
	Write(aValue.GpsTimeOfWeek());
	}
	
	
void Write(const TPositionInfo& aValue)
	{
	Output(_L("(TPositionInfo):{"));
	WriteBase(aValue);
	Output(_L("};"));
	}
	
void Write(const TPositionCourseInfo& aValue)
	{
	Output(_L("(TPositionCourseInfo):{"));
	WriteBase(aValue);
	Output(_L("};"));
	}

void Write(const TPositionSatelliteInfo& aValue)
	{
	Output(_L("(TPositionSatelliteInfo):{"));
	WriteBase(aValue);
	Output(_L("};"));
	}
void Write(const TPositionExtendedSatelliteInfo& aValue)
    {
    Output(_L("(TPositionExtendedSatelliteInfo):{"));
    WriteBase(aValue);
    Output(_L("};"));
    }
void Write(const TPositionGpsMeasurementInfo& aValue)
	{
	Output(_L("(TPositionGpsMeasurementInfo):{"));
	WriteBase(aValue);
	Output(_L("}:"));
	}
	
void Write(const RLbsAssistanceDataBuilderSet& /*aValue*/)
// Not logged!
	{
	Output(_L("(RLbsAssistanceDataBuilderSet):{};"));	
	}

/*
void Write(const TLbsExternalRequestInfo& aValue)
	{
	Output(_L("TLbsExternalRequestInfo):{"));
	
	Output(_L("RequesterId"));
		{
		TLbsRequesterId requesterId;
		aValue.GetRequesterId(requesterId);
		Write(requesterId);
		}

	Output(_L("ClientName"));
		{
		TLbsClientName clientName;
		aValue.GetClientName(clientName);
		Write(clientName);
		}

	Output(_L("ClientExternalId"));
		{
		TLbsClientExternalId clientExternalId;		
		aValue.GetClientName(clientExternalId);
		Write(clientExternalId);
		}
	
	Output(_L("}:"));
	}
*/	
	
void Write(const TLbsLocRequestQuality& aValue)
	{
	Output(_L("(TLbsLocRequestQuality):{"));
	
	Output(_L("MaxFixTime"));
	Write(aValue.MaxFixTime());
	
	Output(_L("MinHorizontalAccuracy"));
	Write(aValue.MinHorizontalAccuracy());
	
	Output(_L("MinVerticalAccuracy"));
	Write(aValue.MinVerticalAccuracy());
	
	Output(_L("}:"));
	}
	
	
void Write(const TLbsGpsOptions& aValue)
	{
	Output(_L("(TLbsGpsOptions):{"));

	Output(_L("GpsMode"));
	Write(aValue.GpsMode());

	Output(_L("}:"));
	}
	
void Write(const TLbsTransmitPositionOptions& aValue)
	{
	Output(_L("(TLbsTransmitPositionOptions):{"));

	Output(_L("TimeOut"));
	Write(aValue.TimeOut());

	Output(_L("}:"));
	}
	

//------------------------------------------------------------------------------------------------


EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TLbsNetSessionId& aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}


EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TLbsNetPosRequestPrivacy& aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TLbsNetPosRequestQuality& aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TLbsNetPosRequestOptionsBase& aParam)
	{
	Output(_L("@"));
	Output(aName);
		
	switch(aParam.ClassType())
		{
		case ELbsNetPosRequestOptionsBaseClass:	Output(_L("(UNKNOWN):UNKNOWN;")); break;
		case ELbsNetPosRequestOptionsClass:		Write((const TLbsNetPosRequestOptions&)aParam); break;
		case ELbsNetPosRequestOptionsAssistanceClass: Write((const TLbsNetPosRequestOptionsAssistance&)aParam);	break;
		default:								Output(_L("(UNKNOWN):UNKNOWN;")); break;
		}
	Output(_L("#"));
	}
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TLbsNetPosMethod& aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}

	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TLbsNetPosCapabilities& aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TLbsNetPosRequestMethod& aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}
	
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TPositionInfoBase& aParam)
	{
	Output(_L("@"));
	Output(aName);
	
	switch(aParam.PositionClassType())
		{
		case EPositionInfoClass:		  
				Write((const TPositionInfo&)aParam); 
				break;
		case EPositionGenericInfoClass:   
				Output(_L("(UNKNOWN):UNKNOWN;")); 
				break;
		case (EPositionInfoClass|EPositionCourseInfoClass):	  
				Write((const TPositionCourseInfo&)aParam); 
				break;
		case (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass): 
				Write((const TPositionSatelliteInfo&)aParam); 
				break;
		case (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass): 
                Write((const TPositionExtendedSatelliteInfo&)aParam); 
                break;
		case EPositionGpsMeasurementInfoClass:
				Write((const TPositionGpsMeasurementInfo&)aParam);
				break;
		default:Output(_L("(UNKNOWN):UNKNOWN;")); 
				break;
		}
	Output(_L("#"));
	}
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const RLbsAssistanceDataBuilderSet& aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, MLbsNetworkProtocolObserver::TLbsNetProtocolService aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, CLbsNetworkProtocolBase::TLbsPrivacyResponse aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, CLbsNetworkProtocolBase::TLbsSystemStatus aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, TInt aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, TUint32 aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}

EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, TInt32 aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}

EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TDesC16& aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}

EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TTime& aParam)
{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
}

	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TLbsExternalRequestInfo& aParam)
	{
	Output(_L("@"));
	Output(aName);
	
	if(EExternalRequestInfoClass != aParam.ClassType())
		{
#ifdef _DEBUG
		User::Invariant();
#endif // _DEBUG
		Output(_L("(UNKNOWN):UNKNOWN;")); 
		}
	else
		{
		Write(aParam);
		}
	Output(_L("#"));
	}
	
EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TLbsLocRequestQuality& aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);
	Output(_L("#"));
	}


EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TLbsGpsOptions& aParam)
	{
	Output(_L("@"));
	Output(aName);
	Write(aParam);	
	Output(_L("#"));
	}
	

EXPORT_C void LBSParamLogger::Log(const TPtrC& aName, const TLbsTransmitPositionOptions& aParam)
	{
	Output(_L("@"));
	Output(aName);
		
	if(ETransmitPositionOptionsClass != aParam.ClassType())
		{
#ifdef _DEBUG
		User::Invariant();
#endif // _DEBUG
		Output(_L("(UNKNOWN):UNKNOWN;")); 
		}
	else
		{
		Write(aParam);
		}
	Output(_L("#"));
	}

	
EXPORT_C void LBSParamLogger::Method(const TPtrC& aDirection, const TPtrC& aMethod)
	{
	TBuf<64> b;
	b.Format(_L("%S %S"), &aDirection, &aMethod);
	Output(b);
	}
	
EXPORT_C void LBSParamLogger::Flush()
	{
	// Nothing just yet
	}

