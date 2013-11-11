// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalTechnology
*/

#ifndef LBS_DEV_LOGGER_MACROS_H
#define LBS_DEV_LOGGER_MACROS_H

#include "lbsdevlogger.h"
#include <e32debug.h>

#if defined(ENABLE_LBS_DEV_LOGGER) && (defined(_DEBUG)||defined(__WINS__))
/**
 -----------------------
 Standard Logging Macros 
 -----------------------
**/
	#define LBSLOG_BEGIN() 						LBSLOG(ELogP9, "StartSP\n");
	#define LBSNEWLOG(L) 						LbsDevLogger::OverWrite(_L(L))
	#define LBSLOG(L1,L2) 						LbsDevLogger::Write(ELogNormal, L1, _L(L2))
	#define LBSLOG2(L1,L2,L3) 					LbsDevLogger::Write(ELogNormal, L1, _L(L2), L3)
	#define LBSLOG3(L1,L2,L3,L4) 				LbsDevLogger::Write(ELogNormal, L1, _L(L2), L3, L4)
	#define LBSLOG4(L1,L2,L3,L4,L5) 			LbsDevLogger::Write(ELogNormal, L1, _L(L2), L3, L4, L5)
	#define LBSLOG5(L1,L2,L3,L4,L5,L6) 			LbsDevLogger::Write(ELogNormal, L1, _L(L2), L3, L4, L5, L6)
	#define LBSLOG6(L1,L2,L3,L4,L5,L6,L7)		LbsDevLogger::Write(ELogNormal, L1, _L(L2), L3, L4, L5, L6, L7)
	#define LBSLOGHEX(L1,L2,L3)					LbsDevLogger::HexDump(L1,L2,L3)

	#define LBSLOG2_L8(L1,L2,L3) 				LbsDevLogger::Write(ELogNormal, L1, _L8(L2), L3)
	
	#define LBSLOG_ERR(L1,L2) 					LbsDevLogger::Write(ELogError, L1, _L(L2))
	#define LBSLOG_ERR2(L1,L2,L3) 				LbsDevLogger::Write(ELogError, L1, _L(L2), L3)
	#define LBSLOG_ERR3(L1,L2,L3,L4) 			LbsDevLogger::Write(ELogError, L1, _L(L2), L3, L4)
	#define LBSLOG_ERR4(L1,L2,L3,L4,L5) 		LbsDevLogger::Write(ELogError, L1, _L(L2), L3, L4, L5)
	#define LBSLOG_ERR5(L1,L2,L3,L4,L5,L6) 		LbsDevLogger::Write(ELogError, L1, _L(L2), L3, L4, L5, L6)
	#define LBSLOG_ERR6(L1,L2,L3,L4,L5,L6,L7)	LbsDevLogger::Write(ELogError, L1, _L(L2), L3, L4, L5, L6, L7)

	#define LBSLOG_WARN(L1,L2) 					LbsDevLogger::Write(ELogWarning, L1, _L(L2))
	#define LBSLOG_WARN2(L1,L2,L3) 				LbsDevLogger::Write(ELogWarning, L1, _L(L2), L3)
	#define LBSLOG_WARN3(L1,L2,L3,L4) 			LbsDevLogger::Write(ELogWarning, L1, _L(L2), L3, L4)
	#define LBSLOG_WARN4(L1,L2,L3,L4,L5) 		LbsDevLogger::Write(ELogWarning, L1, _L(L2), L3, L4, L5)
	#define LBSLOG_WARN5(L1,L2,L3,L4,L5,L6) 	LbsDevLogger::Write(ELogWarning, L1, _L(L2), L3, L4, L5, L6)
	#define LBSLOG_WARN6(L1,L2,L3,L4,L5,L6,L7)	LbsDevLogger::Write(ELogWarning, L1, _L(L2), L3, L4, L5, L6, L7)

	//Macro for getting the processname
	#define LBSLOG_GETPROCESSNAME(L1,L2)		LbsDevLogger::GetMessageProcessName(L1,L2)

/**
 -------------------------------------------------
 Common logging macros (for logging large values)
 -------------------------------------------------
**/

// Logs TLbsNetSessionId
	#define LBSLOG_SESSIONID(L1)	\
		{	\
		LBSLOG2(ELogP9, "    - TUid iSessionOwner  = 0x%08X\n", L1.SessionOwner().iUid);	\
		LBSLOG2(ELogP9, "    - TUint32 iSessionNum  = %u\n", L1.SessionNum());			\
		}

/* Logs TPositionInfoBase
 * This also logs the variables for all of the derived classes (if Applicable):
 * 	- TPositionInfo
 * 	- TPositionCourseInfo
 * 	- TPositionSatelliteInfo
 *  - TPositionGpsMeasurementInfo
 */
	#define LBSLOG_TPOSITIONINFOBASE(L1)		\
		{	\
		/* Find out what the class type is*/	\
		TUint32 classTypeM = L1.PositionClassType();	\
		/* Log the TPositionInfoBase information */	\
		LBSLOG2(ELogP9, "    - TUint32 iPositionClassType  = 0x%08X\n", classTypeM);	\
		LBSLOG2(ELogP9, "    - TPositionModuleId iModuleId  = 0x%08X\n", L1.ModuleId().iUid);	\
		LBSLOG2(ELogP9, "    - TUint32 iUpdateType  = %u\n", L1.UpdateType());	\
		LBSLOG2(ELogP9, "    - TPositionModuleInfo::TTechnologyType iPositionMode  = 0x%04X\n", L1.PositionMode());	\
		LBSLOG2(ELogP9, "    - TPositionModeReason iPositionModeReason  = %u\n", L1.PositionModeReason());	\
		/* Log the TPositionInfo information*/	\
		if(classTypeM & EPositionInfoClass)	\
			{	\
			const TPositionInfo& posInfoM = (reinterpret_cast<const TPositionInfo&>(L1));	\
			TPosition posM;	\
			posInfoM.GetPosition(posM);	\
			TTime timeM = posM.Time();	\
			LBSLOG5(ELogP9, "    - TTime iTime  = %02d:%02d:%02d.%06d\n", timeM.DateTime().Hour(), 	\
																	timeM.DateTime().Minute(),	\
																	timeM.DateTime().Second(),	\
																	timeM.DateTime().MicroSecond());	\
			LBSLOG2(ELogP9, "    - TReal32 iHorizontalAccuracy  = %3.6g\n", posM.HorizontalAccuracy());	\
			LBSLOG2(ELogP9, "    - TReal32 iVerticalAccuracy  = %3.6g\n", posM.VerticalAccuracy());	\
			LBSLOG2(ELogP9, "    - TReal64 iLatitude  = %3.6g\n", posM.Latitude());	\
			LBSLOG2(ELogP9, "    - TReal64 iLongitude  = %3.6g\n", posM.Longitude());	\
			LBSLOG2(ELogP9, "    - TReal32 iAltitude  = %3.6g\n", posM.Altitude());	\
			LBSLOG2(ELogP8, "    - TPositionDatumId iDatum  = 0x%08X\n", posM.Datum());	\
			}	\
		/* Log the TPositionCourseInfo information*/	\
		if(classTypeM & EPositionCourseInfoClass)	\
			{	\
			const TPositionCourseInfo& courseInfoM = (reinterpret_cast<const TPositionCourseInfo&>(L1));	\
			TCourse courseM;	\
			courseInfoM.GetCourse(courseM);	\
			LBSLOG2(ELogP8, "    - TReal32 iSpeed  = %3.6g\n", courseM.Speed());	\
			LBSLOG2(ELogP8, "    - TReal32 iHeading  = %3.6g\n", courseM.Heading());	\
			LBSLOG2(ELogP8, "    - TReal32 iSpeedAccuracy  = %3.6g\n", courseM.SpeedAccuracy());	\
			LBSLOG2(ELogP8, "    - TReal32 iHeadingAccuracy  = %3.6g\n", courseM.HeadingAccuracy());	\
			}	\
		/* Log the TPositionSatelliteInfo information*/	\
		if(classTypeM & EPositionSatelliteInfoClass)	\
			{	\
			const TPositionSatelliteInfo& satInfoM = (reinterpret_cast<const TPositionSatelliteInfo&>(L1));	\
			TInt numSatInViewM = satInfoM.NumSatellitesInView();	\
			LBSLOG2(ELogP8, "    - TUint iNumSatellitesInView  = %u\n", numSatInViewM);	\
			LBSLOG2(ELogP8, "    - TUint iNumSatellitesUsed  = %u\n", satInfoM.NumSatellitesUsed());	\
			LBSLOG2(ELogP8, "    - TReal32 iHorizontalDoPValue  = %3.6g\n", satInfoM.HorizontalDoP());	\
			LBSLOG2(ELogP8, "    - TReal32 iVerticalDoPValue  = %3.6g\n", satInfoM.VerticalDoP());	\
			LBSLOG2(ELogP8, "    - TReal32 iTimeDoPValue  = %3.6g\n", satInfoM.TimeDoP());	\
			for(TInt satNumM=0; ((satNumM < numSatInViewM) && (satNumM < KPositionMaxSatellitesInView)); ++satNumM)	\
				{	\
				TSatelliteData satDataM;	\
				satInfoM.GetSatelliteData(satNumM, satDataM);	\
				LBSLOG2(ELogP8, "    - TSatelliteData iSatellitesInView[%d]  =\n", satNumM);	\
				LBSLOG2(ELogP8, "      - TInt iSatelliteId  = %d\n", satDataM.SatelliteId());	\
				LBSLOG2(ELogP8, "      - TReal32 iAzimuth  = %3.6g\n", satDataM.Azimuth());	\
				LBSLOG2(ELogP8, "      - TReal32 iElevation  = %3.6g\n", satDataM.Elevation());	\
				LBSLOG2(ELogP8, "      - TBool iIsUsed  = %d\n", satDataM.IsUsed());	\
				LBSLOG2(ELogP8, "      - TInt iSignalStrength  = %d\n", satDataM.SignalStrength());	\
				}	\
			}	\
		/* Log the TPositionGpsMeasurementInfo*/	\
		if(classTypeM & EPositionGpsMeasurementInfoClass)	\
			{	\
			const TPositionGpsMeasurementInfo& gpsInfoM = (reinterpret_cast<const TPositionGpsMeasurementInfo&>(L1));	\
			TInt numMeasurementsM = gpsInfoM.NumMeasurements();	\
			LBSLOG2(ELogP8, "    - TInt iGpsTimeOfWeek  = %d\n", gpsInfoM.GpsTimeOfWeek());	\
			LBSLOG2(ELogP8, "    - TUint iNumMeasurements  = %d\n", numMeasurementsM);	\
			for(TInt gpsNumM=0; ((gpsNumM < numMeasurementsM) && (gpsNumM < KPositionGpsMaxMeasurements)); ++gpsNumM)	\
				{	\
				TPositionGpsMeasurementData tempGpsDataM;	\
				gpsInfoM.GetMeasurementData(gpsNumM, tempGpsDataM);	\
				LBSLOG2(ELogP8, "    - TPositionGpsMeasurementData iMeasurementData[%d]  = \n", gpsNumM);	\
				LBSLOG2(ELogP8, "      - TInt iSatelliteId  = %d\n", tempGpsDataM.SatelliteId());	\
				LBSLOG2(ELogP8, "      - TInt iCarrierNoiseRatio  = %d\n", tempGpsDataM.CarrierNoiseRatio());	\
				LBSLOG2(ELogP8, "      - TInt iDoppler  = %d\n", tempGpsDataM.Doppler());	\
				LBSLOG2(ELogP8, "      - TInt iWholeGpsChips  = %d\n", tempGpsDataM.WholeGpsChips());	\
				LBSLOG2(ELogP8, "      - TInt iFractionalGpsChips  = %d\n", tempGpsDataM.FractionalGpsChips());	\
				LBSLOG2(ELogP8, "      - TMultiPathIndicator iMultiPathIndicator  = %d\n", tempGpsDataM.MultiPathIndicator());	\
				LBSLOG2(ELogP8, "      - TUint iPseudoRangeRmsError  = %u\n", tempGpsDataM.PseudoRangeRmsError());	\
				}	\
			}	\
		}

// Logs TPositionQuality
	#define LBSLOG_TPOSITIONQUALITY(L1)	\
		{	\
		LBSLOG2(ELogP9, "    - TReal32 iHorizontalAccuracy  = %3.6g\n", L1.HorizontalAccuracy());	\
		LBSLOG2(ELogP9, "    - TReal32 iVerticalAccuracy  = %3.6g\n", L1.VerticalAccuracy());	\
		LBSLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iTimeToFirstFix  = %ld\n", L1.TimeToFirstFix().Int64());	\
		LBSLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iTimeToNextFix  = %ld\n", L1.TimeToNextFix().Int64());	\
		LBSLOG2(ELogP9, "    - TCostIndicator iCostIndicator  = %d\n", L1.CostIndicator());	\
		LBSLOG2(ELogP9, "    - TPowerConsumption iPowerConsumption  = %d\n", L1.PowerConsumption());	\
		}

// Logs TPositionModuleInfo
	#define LBSLOG_TPOSITIONMODULE(L1)	\
		{	\
		if(L1.PositionClassType() & EPositionModuleInfoClass)	\
			{	\
			const TPositionModuleInfo& positionInfoM = (reinterpret_cast<const TPositionModuleInfo&>(L1));	\
			LBSLOG2(ELogP9, "    - TPositionModuleId iModuleId  = 0x%08X\n", positionInfoM.ModuleId().iUid);	\
			LBSLOG2(ELogP9, "    - TBool iIsAvailable  = %d\n", positionInfoM.IsAvailable());	\
			TBuf<KPositionMaxModuleName> tempModuleNameM;	\
			positionInfoM.GetModuleName(tempModuleNameM);	\
			LBSLOG2(ELogP9, "    - TBuf iModuleName  = %S\n", &tempModuleNameM);	\
			TPositionQuality tempPositionQualityM;	\
			positionInfoM.GetPositionQuality(tempPositionQualityM);	\
			LBSLOG_TPOSITIONQUALITY(tempPositionQualityM);	\
			LBSLOG2(ELogP9, "    - TTechnologyType iTechnologyType  = %x02X\n", positionInfoM.TechnologyType());	\
			LBSLOG2(ELogP9, "    - TDeviceLocation iDeviceLocation  = %x02X\n", positionInfoM.DeviceLocation());	\
			LBSLOG2(ELogP9, "    - TCapabilities iCapabilities  = %x04X\n", positionInfoM.Capabilities());	\
			LBSLOG4(ELogP9, "    - TVersion iVersion  = (%d %d %d)\n", positionInfoM.Version().iMajor,	\
																	positionInfoM.Version().iMinor,	\
																	positionInfoM.Version().iBuild);\
			}	\
		}

// Logs TLbsExternalRequestInfo
	#define LBSLOG_TLBSEXTERNALREQ(L1)	\
		{	\
		/* Find out what the class type is*/	\
		TUint32 classTypeM = L1.ClassType();	\
		/* Output shared info */	\
		LBSLOG2(ELogP9, "    - TNetworkType iNetworkType  = %x02X\n", L1.NetworkType());	\
		LBSLOG2(ELogP9, "    - TRequestType iRequestType  = %x02X\n", L1.RequestType());	\
		LBSLOG2(ELogP9, "    - TFormatIndicator iRequesterIdFormat  = %x02X\n", L1.RequesterIdFormat());	\
		LBSLOG2(ELogP9, "    - TFormatIndicator iClientNameFormat  = %x02X\n", L1.ClientNameFormat());	\
		TBuf8<KLbsMaxClientExternalIdSize> externalSizeM;	\
		L1.GetClientExternalId(externalSizeM);	\
		LBSLOG2_L8(ELogP9, "    - TLbsClientExternalId iClientExternalId  = %S", &externalSizeM);	\
		/* Output type specific info */	\
		if (classTypeM == EExternalRequestInfoClass)	\
			{	\
			const TLbsExternalRequestInfo& info = reinterpret_cast<const TLbsExternalRequestInfo&>(L1);	\
			TBuf8<KLbsMaxRequesterIdSize> requesterIdM;	\
			info.GetRequesterId(requesterIdM);	\
	 		TBuf8<KLbsMaxClientNameSize> clientNameM;	\
			info.GetClientName(clientNameM);	\
			LBSLOG2_L8(ELogP9, "    - TLbsRequesterId iRequesterId  = %S", &requesterIdM);	\
			LBSLOG2_L8(ELogP9, "    - TLbsClientName iClientName  = %S", &clientNameM);	\
			LBSLOG2(ELogP9, "    - TFormatIndicator iRequesterIdCodingScheme  = %x02X\n", info.RequesterIdCodingScheme());	\
			LBSLOG2(ELogP9, "    - TFormatIndicator iClientNameCodingScheme  = %x02X\n", info.ClientNameCodingScheme());	\
			}	\
		else if (classTypeM == (EExternalRequestInfoClass | EExternalRequestInfoClass2))	\
			{	\
			const TLbsExternalRequestInfo2& info = reinterpret_cast<const TLbsExternalRequestInfo2&>(L1);	\
			TBuf16<KLbsMaxRequesterIdSize2> requesterIdM;	\
			info.GetRequesterId(requesterIdM);	\
	 		TBuf16<KLbsMaxClientNameSize2> clientNameM;	\
			info.GetClientName(clientNameM);	\
			LBSLOG2(ELogP9, "    - TLbsRequesterId iRequesterId  = %S", &requesterIdM);	\
			LBSLOG2(ELogP9, "    - TLbsClientName iClientName  = %S", &clientNameM);	\
			}	\
		}

// Logs TLbsNetPosRequestOptionsAssistance
	#define LBSLOG_REQUESTOPTIONSASSISTANCE(L1)	\
		{	\
		LBSLOG2(ELogP9, "    - TLbsAsistanceDataGroup iDataRequestMask  = 0x%08X\n", L1.DataRequestMask());	\
		LBSLOG2(ELogP9, "    - TPositionModuleInfo::TTechnologyType iPosMode  = 0x%04X\n", L1.PosMode());	\
		}

// Logs TLbsNetPosRequestPrivacy
	#define LBSLOG_POSREQUESTPRIVACY(L1)	\
		{	\
		LBSLOG2(ELogP9, "    - TLbsRequestAction iAction  = %d", L1.RequestAction());	\
		LBSLOG2(ELogP9, "    - TLbsRequestAdvice iAdvice  = %d", L1.RequestAdvice());	\
		}

// Logs TLbsNetPosRequestQuality
	#define LBSLOG_POSREQUESTQUALITY(L1)	\
		{	\
		LBSLOG2(ELogP9, "    - TReal32 iMinHorizontalAccuracy  = %3.6g\n", L1.MinHorizontalAccuracy());	\
		LBSLOG2(ELogP9, "    - TReal32 iMinVerticalAccuracy  = %3.6g\n", L1.MinVerticalAccuracy());	\
		LBSLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iMaxFixTime  = %ld\n", L1.MaxFixTime().Int64());	\
		LBSLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iMaxFixAge  = %ld\n", L1.MaxFixAge().Int64());	\
		}

// Logs TLbsNetPosRequestOptions
	#define LBSLOG_POSREQUESTOPTIONS(L1) \
		{	\
		LBSLOG2(ELogP9, "    - TBool iNewClient  = %d\n", L1.NewClientConnected());	\
		TLbsNetPosRequestQuality qualityM;	\
		L1.GetRequestQuality(qualityM);	\
		LBSLOG_POSREQUESTQUALITY(qualityM);	\
		}

// Logs TPositionUpdateOptionsBase
	#define LBSLOG_POSITIONOPTIONBASE(L1)	\
		{	\
		LBSLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iUpdateInterval  = %ld\n", L1.UpdateInterval().Int64());	\
		LBSLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iUpdateTimeOut  = %ld\n", L1.UpdateTimeOut().Int64());	\
		LBSLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iMaxUpdateAge    = %ld\n", L1.MaxUpdateAge().Int64());	\
		}

// Logs TPositionCriteriaBase
	#define LBSLOG_POSITIONCRITERIABASE(L1)	\
		{	\
		LBSLOG2(ELogP9, "    - TPositionModuleInfo::TCapabilities iRequiredCaps  = 0x%04X\n", L1.RequiredCapabilities());	\
		TPositionQuality tempPositionQualityM;	\
		L1.GetRequiredQuality(tempPositionQualityM);	\
		LBSLOG_TPOSITIONQUALITY(tempPositionQualityM);	\
		}

// Logs TPositionModuleStatusBase
	#define LBSLOG_POSMODULESTATUS(L1)	\
		{	\
		if(L1.PositionClassType() & EPositionModuleStatusClass)	\
			{	\
			const TPositionModuleStatus& positionStatusM = (reinterpret_cast<const TPositionModuleStatus&>(L1));	\
			LBSLOG2(ELogP9, "    - TDeviceStatus iDeviceStatus  = %d\n", positionStatusM.DeviceStatus());	\
			LBSLOG2(ELogP9, "    - TDataQualityStatus iDataQualityStatus  = %d\n", positionStatusM.DataQualityStatus());	\
			}	\
		}

// Logs TPositionModuleStatusEventBase
	#define LBSLOG_POSMODULESTATUSEVENT(L1)	\
		{	\
		LBSLOG2(ELogP9, "    - TModuleEvent iRequestedEvents  = %u\n", L1.RequestedEvents());	\
		LBSLOG2(ELogP9, "    - TModuleEvent iOccurredEvents  = %u\n", L1.OccurredEvents());	\
		LBSLOG2(ELogP9, "    - TSystemModuleEvent iSystemModuleEvent  = %u\n", L1.SystemModuleEvent());	\
		LBSLOG2(ELogP9, "    - TPositionModuleId  iModuleId  = 0x%08X\n", L1.ModuleId().iUid);	\
		}

// Logs TLbsNetPosRequestMethod
	#define LBSLOG_POSREQUESTMETHOD(L1)	\
		{	\
		TInt numPosMethodsM = L1.NumPosMethods();	\
		LBSLOG2(ELogP9, "    - TInt iNumPosMethods  = %d\n", numPosMethodsM);	\
		TLbsNetPosMethod posMethodM;	\
		for(TInt posNumM=0; ((posNumM < numPosMethodsM) && ( posNumM < KLbsMaxNetPosMethods)); ++posNumM)	\
			{	\
			LBSLOG2(ELogP9, "    - TLbsNetPosMethod posMethod[%d]\n", posNumM);	\
			if((L1.GetPosMethod(posNumM, posMethodM)))	\
				{	\
				LBSLOG2(ELogP9, "      - TUid iPosMeans  = 0x%08X\n", posMethodM.PosMeans());	\
				LBSLOG2(ELogP9, "      - TPositionModuleInfo::TTechnologyType iPosMode  = %d\n", posMethodM.PosMode());	\
				}	\
			}	\
		}

// Logs TLbsGpsOptions
	#define LBSLOG_TLBSGPSOPTIONS(L1)	\
		{	\
		LBSLOG2(ELogP9, "    - CLbsAdmin::TGpsMode iGpsMode  = 0x%02X\n", L1.GpsMode());	\
		if (L1.ClassType() & ELbsGpsOptionsArrayClass)	\
			{	\
			const TLbsGpsOptionsArray& optionsArrayM = (reinterpret_cast<const TLbsGpsOptionsArray&>(L1));	\
			TInt numOptionsM = optionsArrayM.NumOptionItems();	\
			LBSLOG2(ELogP9, "    - TUint iNumItems  = %u\n", numOptionsM);	\
			for(TInt optionsNumM=0; ((optionsNumM < numOptionsM) && (optionsNumM < KLbsMaxGpsOptions)); ++optionsNumM)	\
				{	\
				TLbsGpsOptionsItem tempOptionsM;	\
				optionsArrayM.GetOptionItem(optionsNumM, tempOptionsM);	\
				LBSLOG2(ELogP9, "    - TLbsGpsOptionsItem iOptions[%d]  =\n", optionsNumM);	\
				LBSLOG2(ELogP9, "      - TPosUpdateType iPosUpdateType  = %d", tempOptionsM.PosUpdateType());	\
				}	\
			}	\
		LBSLOG2(ELogP9, "    - TBool iGpsTimeRelationReq  = %d\n", L1.GpsTimeRelationReq());	\
		}

#else
/**
 Standard Logging Macros 
**/
	#define LBSLOG_BEGIN()
	#define LBSNEWLOG(L)
	#define LBSLOG(L1,L2)
	#define LBSLOG2(L1,L2,L3)
	#define LBSLOG3(L1,L2,L3,L4)
	#define LBSLOG4(L1,L2,L3,L4,L5)
	#define LBSLOG5(L1,L2,L3,L4,L5,L6)
	#define LBSLOG6(L1,L2,L3,L4,L5,L6,L7)
	#define LBSLOGHEX(L1,L2,L3)

	#define LBSLOG2_L8(L1,L2,L3)
	
	#define LBSLOG_ERR(L1,L2)
	#define LBSLOG_ERR2(L1,L2,L3)
	#define LBSLOG_ERR3(L1,L2,L3,L4)
	#define LBSLOG_ERR4(L1,L2,L3,L4,L5)
	#define LBSLOG_ERR5(L1,L2,L3,L4,L5,L6)
	#define LBSLOG_ERR6(L1,L2,L3,L4,L5,L6,L7)

	#define LBSLOG_WARN(L1,L2)
	#define LBSLOG_WARN2(L1,L2,L3)
	#define LBSLOG_WARN3(L1,L2,L3,L4)
	#define LBSLOG_WARN4(L1,L2,L3,L4,L5)
	#define LBSLOG_WARN5(L1,L2,L3,L4,L5,L6)
	#define LBSLOG_WARN6(L1,L2,L3,L4,L5,L6,L7)

/**
 Common logging macros (for logging large values)
**/
	#define LBSLOG_SESSIONID(L1)
	#define LBSLOG_TPOSITIONINFOBASE(L1)
	#define LBSLOG_TPOSITIONQUALITY(L1)
	#define LBSLOG_TPOSITIONMODULE(L1)
	#define LBSLOG_TLBSEXTERNALREQ(L1)
	#define LBSLOG_REQUESTOPTIONSASSISTANCE(L1)
	#define LBSLOG_POSREQUESTPRIVACY(L1)
	#define LBSLOG_POSREQUESTQUALITY(L1)
	#define LBSLOG_POSREQUESTOPTIONS(L1)
	#define LBSLOG_POSITIONOPTIONBASE(L1)
	#define LBSLOG_POSITIONCRITERIABASE(L1)
	#define LBSLOG_POSMODULESTATUS(L1)
	#define LBSLOG_POSMODULESTATUSEVENT(L1)
	#define LBSLOG_POSREQUESTMETHOD(L1)
	#define LBSLOG_TLBSGPSOPTIONS(L1)

	#define LBSLOG_GETPROCESSNAME(L1,L2)

#endif // ENABLE_LBS_DEV_LOGGER

// Need to enable the RDebug logging from here. If you want the time to be logged then you also need 
// to enable ENABLE_LBS_RDEBUG_LOGTIME.
//#define ENABLE_LBS_RDEBUG_LOGGER
//#define ENABLE_LBS_RDEBUG_LOGTIME
// If you want to see the logs of the inputs and outputs of LBS (so the messages to/from
// the client, the AGPS module, the PSYs, the Protocol Module) then enable the macro below.
// In addition you can also see the times if you enable ENABLE_LBS_RDEBUG_LOGTIME
//#define ENABLE_LBS_RDEBUG_SUBSYSTEM_MESSAGES

#if defined(ENABLE_LBS_RDEBUG_LOGTIME)
_LIT8(KLbsRDebugLoggerTimeFormat, "%02d:%02d.%02d ");

inline TBuf8<9> TimeNowForLog()
	{
	TTime time;
	time.HomeTime();
	TBuf8<9> timeBuf;
	TDateTime timeR(time.DateTime());
	timeBuf.AppendFormat(KLbsRDebugLoggerTimeFormat,timeR.Minute(),timeR.Second(),timeR.MicroSecond());
	return timeBuf;
	}

	#define LBS_TIME_LOG &(TimeNowForLog())
#else
	#define LBS_TIME_LOG &KNullDesC
#endif

// The LBS_RDEBUG_INFO outputs the string passed in together with the time of the log (if that is 
// enabled) and the file and line where the log is made.
// The LBS_RDEBUG_VAR_INT and LBS_RDEBUG_VAR_DES output the name of a variable (passed in as a string) 
// together with it's value
// Example: LBS_RDEBUG_INFO("DoSomethingL");
//          LBS_RDEBUG_VAR_INT("aValue",aValue);
#if defined(ENABLE_LBS_RDEBUG_LOGGER) 
	#define LBS_RDEBUG_INFO(INFO) 						RDebug::Printf("%S\t LBSLog: %s (in %s:%d)", LBS_TIME_LOG, INFO, __FILE__, __LINE__)
	#define LBS_RDEBUG_VAR_INT(NAME,VAL) 				RDebug::Printf("\t\t LBSLog: %s = %d (in %s:%d)", NAME, VAL, __FILE__, __LINE__)
	#define LBS_RDEBUG_VAR_DES(NAME,VAL) 				RDebug::Printf("\t\t LBSLog: %s = %S (in %s:%d)", NAME, &VAL, __FILE__, __LINE__)
#else
	#define LBS_RDEBUG_INFO(INFO)
	#define LBS_RDEBUG_VAR_INT(NAME,VAL)
	#define LBS_RDEBUG_VAR_DES(NAME,VAL)
#endif

#if defined(ENABLE_LBS_RDEBUG_SUBSYSTEM_MESSAGES) 
	#define LBS_RDEBUG(SOURCE, TARGET, CALL) RDebug::Printf("%S LBSLog: %s -> %s: %s (in %s:%d)", LBS_TIME_LOG, SOURCE, TARGET, CALL, __FILE__, __LINE__)
	#define LBS_RDEBUG_ARGINT(SOURCE, TARGET, CALL, VAL) RDebug::Printf("%S LBSLog: %s -> %s: %s(%d) (in %s:%d)", LBS_TIME_LOG, SOURCE, TARGET, CALL, VAL, __FILE__, __LINE__)
	#define LBS_RDEBUG_ARGTEXT(SOURCE, TARGET, CALL, VAL) RDebug::Printf("%S LBSLog: %s -> %s: %s(%s) (in %s:%d)", LBS_TIME_LOG, SOURCE, TARGET, CALL, VAL, __FILE__, __LINE__)
	#define LBS_RDEBUG_ARGDES(SOURCE, TARGET, CALL, VAL) RDebug::Printf("%S LBSLog: %s -> %s: %s(%s) (in %s:%d)", LBS_TIME_LOG, SOURCE, TARGET, CALL, VAL, __FILE__, __LINE__)
#else
	#define LBS_RDEBUG(SOURCE, TARGET, CALL) 
	#define LBS_RDEBUG_ARGINT(SOURCE, TARGET, CALL, VAL) 
	#define LBS_RDEBUG_ARGTEXT(SOURCE, TARGET, CALL, VAL)
	#define LBS_RDEBUG_ARGDES(SOURCE, TARGET, CALL, VAL)
#endif

#endif //LBS_DEV_LOGGER_MACROS_H

