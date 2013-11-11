// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SUPL_DEV_LOGGER_MACROS_H
#define SUPL_DEV_LOGGER_MACROS_H

#include "supldevlogger.h"

#if defined(ENABLE_SUPL_DEV_LOGGER)&& (defined(_DEBUG)||defined(__WINS__))
/**
 -----------------------
 Standard Logging Macros 
 -----------------------
**/
	#define SUPLLOG_BEGIN() 					SUPLLOG(ELogP9, "StartSP\n");
	#define SUPLNEWLOG(L) 						SuplDevLogger::OverWrite(_L(L))
	#define SUPLLOG(L1,L2) 						SuplDevLogger::Write(ELogNormal, L1, _L(L2))
	#define SUPLLOG2(L1,L2,L3) 					SuplDevLogger::Write(ELogNormal, L1, _L(L2), L3)
	#define SUPLLOG3(L1,L2,L3,L4) 				SuplDevLogger::Write(ELogNormal, L1, _L(L2), L3, L4)
	#define SUPLLOG4(L1,L2,L3,L4,L5) 			SuplDevLogger::Write(ELogNormal, L1, _L(L2), L3, L4, L5)
	#define SUPLLOG5(L1,L2,L3,L4,L5,L6) 		SuplDevLogger::Write(ELogNormal, L1, _L(L2), L3, L4, L5, L6)
	#define SUPLLOG6(L1,L2,L3,L4,L5,L6,L7)		SuplDevLogger::Write(ELogNormal, L1, _L(L2), L3, L4, L5, L6, L7)
	#define SUPLLOGHEX(L1,L2,L3)				SuplDevLogger::HexDump(L1,L2,L3)

	#define SUPLLOG2_L8(L1,L2,L3) 				SuplDevLogger::Write(ELogNormal, L1, _L8(L2), L3)
	#define SUPLLOG3_L8(L1,L2,L3,L4) 			SuplDevLogger::Write(ELogNormal, L1, _L8(L2), L3, L4)
	#define SUPLLOG4_L8(L1,L2,L3,L4,L5)			SuplDevLogger::Write(ELogNormal, L1, _L8(L2), L3, L4, L5)
	
	#define SUPLLOG_ERR(L1,L2) 					SuplDevLogger::Write(ELogError, L1, _L(L2))
	#define SUPLLOG_ERR2(L1,L2,L3) 				SuplDevLogger::Write(ELogError, L1, _L(L2), L3)
	#define SUPLLOG_ERR3(L1,L2,L3,L4) 			SuplDevLogger::Write(ELogError, L1, _L(L2), L3, L4)
	#define SUPLLOG_ERR4(L1,L2,L3,L4,L5) 		SuplDevLogger::Write(ELogError, L1, _L(L2), L3, L4, L5)
	#define SUPLLOG_ERR5(L1,L2,L3,L4,L5,L6) 	SuplDevLogger::Write(ELogError, L1, _L(L2), L3, L4, L5, L6)
	#define SUPLLOG_ERR6(L1,L2,L3,L4,L5,L6,L7)	SuplDevLogger::Write(ELogError, L1, _L(L2), L3, L4, L5, L6, L7)

	#define SUPLLOG_WARN(L1,L2) 				SuplDevLogger::Write(ELogWarning, L1, _L(L2))
	#define SUPLLOG_WARN2(L1,L2,L3) 			SuplDevLogger::Write(ELogWarning, L1, _L(L2), L3)
	#define SUPLLOG_WARN3(L1,L2,L3,L4) 			SuplDevLogger::Write(ELogWarning, L1, _L(L2), L3, L4)
	#define SUPLLOG_WARN4(L1,L2,L3,L4,L5) 		SuplDevLogger::Write(ELogWarning, L1, _L(L2), L3, L4, L5)
	#define SUPLLOG_WARN5(L1,L2,L3,L4,L5,L6) 	SuplDevLogger::Write(ELogWarning, L1, _L(L2), L3, L4, L5, L6)
	#define SUPLLOG_WARN6(L1,L2,L3,L4,L5,L6,L7)	SuplDevLogger::Write(ELogWarning, L1, _L(L2), L3, L4, L5, L6, L7)

	//Macro for getting the processname
	#define SUPLLOG_GETPROCESSNAME(L1,L2)		SuplDevLogger::GetMessageProcessName(L1,L2)
	
	//Macro for logging contents of ObjSys Data Structures (not for general use)
	#define SUPLLOG_PDU(L1)						L1->Print()

/**
 -------------------------------------------------
 Common logging macros (for logging large values)
 -------------------------------------------------
**/

// Logs TLbsNetSessionId
	#define SUPLLOG_SESSIONID(L1)	\
		{	\
		SUPLLOG2(ELogP9, "    - TUid iSessionOwner  = 0x%08X\n", L1.SessionOwner().iUid);	\
		SUPLLOG2(ELogP9, "    - TUint32 iSessionNum  = %u\n", L1.SessionNum());			\
		}

/* Logs TPositionInfoBase
 * This also logs the variables for all of the derived classes (if Applicable):
 * 	- TPositionInfo
 * 	- TPositionCourseInfo
 * 	- TPositionSatelliteInfo
 *  - TPositionGpsMeasurementInfo
 */
	#define SUPLLOG_TPOSITIONINFOBASE(L1)		\
		{	\
		/* Find out what the class type is*/	\
		TUint32 classTypeM = L1.PositionClassType();	\
		/* Log the TPositionInfoBase information */	\
		SUPLLOG2(ELogP9, "    - TUint32 iPositionClassType  = 0x%08X\n", classTypeM);	\
		SUPLLOG2(ELogP9, "    - TPositionModuleId iModuleId  = 0x%08X\n", L1.ModuleId().iUid);	\
		SUPLLOG2(ELogP9, "    - TUint32 iUpdateType  = %u\n", L1.UpdateType());	\
		SUPLLOG2(ELogP9, "    - TPositionModuleInfo::TTechnologyType iPositionMode  = 0x%04X\n", L1.PositionMode());	\
		SUPLLOG2(ELogP9, "    - TPositionModeReason iPositionModeReason  = %u\n", L1.PositionModeReason());	\
		/* Log the TPositionInfo information*/	\
		if(classTypeM & EPositionInfoClass)	\
			{	\
			const TPositionInfo& posInfoM = (reinterpret_cast<const TPositionInfo&>(L1));	\
			TPosition posM;	\
			posInfoM.GetPosition(posM);	\
			TTime timeM = posM.Time();	\
			SUPLLOG5(ELogP9, "    - TTime iTime  = %02d:%02d:%02d.%06d\n", timeM.DateTime().Hour(), 	\
																	timeM.DateTime().Minute(),	\
																	timeM.DateTime().Second(),	\
																	timeM.DateTime().MicroSecond());	\
			SUPLLOG2(ELogP9, "    - TReal32 iHorizontalAccuracy  = %3.6g\n", posM.HorizontalAccuracy());	\
			SUPLLOG2(ELogP9, "    - TReal32 iVerticalAccuracy  = %3.6g\n", posM.VerticalAccuracy());	\
			SUPLLOG2(ELogP9, "    - TReal64 iLatitude  = %3.6g\n", posM.Latitude());	\
			SUPLLOG2(ELogP9, "    - TReal64 iLongitude  = %3.6g\n", posM.Longitude());	\
			SUPLLOG2(ELogP9, "    - TReal32 iAltitude  = %3.6g\n", posM.Altitude());	\
			SUPLLOG2(ELogP8, "    - TPositionDatumId iDatum  = 0x%08X\n", posM.Datum());	\
			}	\
		/* Log the TPositionCourseInfo information*/	\
		if(classTypeM & EPositionCourseInfoClass)	\
			{	\
			const TPositionCourseInfo& courseInfoM = (reinterpret_cast<const TPositionCourseInfo&>(L1));	\
			TCourse courseM;	\
			courseInfoM.GetCourse(courseM);	\
			SUPLLOG2(ELogP8, "    - TReal32 iSpeed  = %3.6g\n", courseM.Speed());	\
			SUPLLOG2(ELogP8, "    - TReal32 iHeading  = %3.6g\n", courseM.Heading());	\
			SUPLLOG2(ELogP8, "    - TReal32 iSpeedAccuracy  = %3.6g\n", courseM.SpeedAccuracy());	\
			SUPLLOG2(ELogP8, "    - TReal32 iHeadingAccuracy  = %3.6g\n", courseM.HeadingAccuracy());	\
			}	\
		/* Log the TPositionSatelliteInfo information*/	\
		if(classTypeM & EPositionSatelliteInfoClass)	\
			{	\
			const TPositionSatelliteInfo& satInfoM = (reinterpret_cast<const TPositionSatelliteInfo&>(L1));	\
			TInt numSatInViewM = satInfoM.NumSatellitesInView();	\
			SUPLLOG2(ELogP8, "    - TUint iNumSatellitesInView  = %u\n", numSatInViewM);	\
			SUPLLOG2(ELogP8, "    - TUint iNumSatellitesUsed  = %u\n", satInfoM.NumSatellitesUsed());	\
			SUPLLOG2(ELogP8, "    - TReal32 iHorizontalDoPValue  = %3.6g\n", satInfoM.HorizontalDoP());	\
			SUPLLOG2(ELogP8, "    - TReal32 iVerticalDoPValue  = %3.6g\n", satInfoM.VerticalDoP());	\
			SUPLLOG2(ELogP8, "    - TReal32 iTimeDoPValue  = %3.6g\n", satInfoM.TimeDoP());	\
			for(TInt satNumM=0; ((satNumM < numSatInViewM) && (satNumM < KPositionMaxSatellitesInView)); ++satNumM)	\
				{	\
				TSatelliteData satDataM;	\
				satInfoM.GetSatelliteData(satNumM, satDataM);	\
				SUPLLOG2(ELogP8, "    - TSatelliteData iSatellitesInView[%d]  =\n", satNumM);	\
				SUPLLOG2(ELogP8, "      - TInt iSatelliteId  = %d\n", satDataM.SatelliteId());	\
				SUPLLOG2(ELogP8, "      - TReal32 iAzimuth  = %3.6g\n", satDataM.Azimuth());	\
				SUPLLOG2(ELogP8, "      - TReal32 iElevation  = %3.6g\n", satDataM.Elevation());	\
				SUPLLOG2(ELogP8, "      - TBool iIsUsed  = %d\n", satDataM.IsUsed());	\
				SUPLLOG2(ELogP8, "      - TInt iSignalStrength  = %d\n", satDataM.SignalStrength());	\
				}	\
			}	\
		/* Log the TPositionGpsMeasurementInfo*/	\
		if(classTypeM & EPositionGpsMeasurementInfoClass)	\
			{	\
			const TPositionGpsMeasurementInfo& gpsInfoM = (reinterpret_cast<const TPositionGpsMeasurementInfo&>(L1));	\
			TInt numMeasurementsM = gpsInfoM.NumMeasurements();	\
			SUPLLOG2(ELogP8, "    - TInt iGpsTimeOfWeek  = %d\n", gpsInfoM.GpsTimeOfWeek());	\
			SUPLLOG2(ELogP8, "    - TUint iNumMeasurements  = %d\n", numMeasurementsM);	\
			for(TInt gpsNumM=0; ((gpsNumM < numMeasurementsM) && (gpsNumM < KPositionGpsMaxMeasurements)); ++gpsNumM)	\
				{	\
				TPositionGpsMeasurementData tempGpsDataM;	\
				gpsInfoM.GetMeasurementData(gpsNumM, tempGpsDataM);	\
				SUPLLOG2(ELogP8, "    - TPositionGpsMeasurementData iMeasurementData[%d]  = \n", gpsNumM);	\
				SUPLLOG2(ELogP8, "      - TInt iSatelliteId  = %d\n", tempGpsDataM.SatelliteId());	\
				SUPLLOG2(ELogP8, "      - TInt iCarrierNoiseRatio  = %d\n", tempGpsDataM.CarrierNoiseRatio());	\
				SUPLLOG2(ELogP8, "      - TInt iDoppler  = %d\n", tempGpsDataM.Doppler());	\
				SUPLLOG2(ELogP8, "      - TInt iWholeGpsChips  = %d\n", tempGpsDataM.WholeGpsChips());	\
				SUPLLOG2(ELogP8, "      - TInt iFractionalGpsChips  = %d\n", tempGpsDataM.FractionalGpsChips());	\
				SUPLLOG2(ELogP8, "      - TMultiPathIndicator iMultiPathIndicator  = %d\n", tempGpsDataM.MultiPathIndicator());	\
				SUPLLOG2(ELogP8, "      - TUint iPseudoRangeRmsError  = %u\n", tempGpsDataM.PseudoRangeRmsError());	\
				}	\
			}	\
		}

// Logs TPositionQuality
	#define SUPLLOG_TPOSITIONQUALITY(L1)	\
		{	\
		SUPLLOG2(ELogP9, "    - TReal32 iHorizontalAccuracy  = %3.6g\n", L1.HorizontalAccuracy());	\
		SUPLLOG2(ELogP9, "    - TReal32 iVerticalAccuracy  = %3.6g\n", L1.VerticalAccuracy());	\
		SUPLLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iTimeToFirstFix  = %ld\n", L1.TimeToFirstFix().Int64());	\
		SUPLLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iTimeToNextFix  = %ld\n", L1.TimeToNextFix().Int64());	\
		SUPLLOG2(ELogP9, "    - TCostIndicator iCostIndicator  = %d\n", L1.CostIndicator());	\
		SUPLLOG2(ELogP9, "    - TPowerConsumption iPowerConsumption  = %d\n", L1.PowerConsumption());	\
		}

// Logs TPositionModuleInfo
	#define SUPLLOG_TPOSITIONMODULE(L1)	\
		{	\
		if(L1.PositionClassType() & EPositionModuleInfoClass)	\
			{	\
			const TPositionModuleInfo& positionInfoM = (reinterpret_cast<const TPositionModuleInfo&>(L1));	\
			SUPLLOG2(ELogP9, "    - TPositionModuleId iModuleId  = 0x%08X\n", positionInfoM.ModuleId().iUid);	\
			SUPLLOG2(ELogP9, "    - TBool iIsAvailable  = %d\n", positionInfoM.IsAvailable());	\
			TBuf<KPositionMaxModuleName> tempModuleNameM;	\
			positionInfoM.GetModuleName(tempModuleNameM);	\
			SUPLLOG2(ELogP9, "    - TBuf iModuleName  = %S\n", &tempModuleNameM);	\
			TPositionQuality tempPositionQualityM;	\
			positionInfoM.GetPositionQuality(tempPositionQualityM);	\
			SUPLLOG_TPOSITIONQUALITY(tempPositionQualityM);	\
			SUPLLOG2(ELogP9, "    - TTechnologyType iTechnologyType  = %x02X\n", positionInfoM.TechnologyType());	\
			SUPLLOG2(ELogP9, "    - TDeviceLocation iDeviceLocation  = %x02X\n", positionInfoM.DeviceLocation());	\
			SUPLLOG2(ELogP9, "    - TCapabilities iCapabilities  = %x04X\n", positionInfoM.Capabilities());	\
			SUPLLOG4(ELogP9, "    - TVersion iVersion  = (%d %d %d)\n", positionInfoM.Version().iMajor,	\
																	positionInfoM.Version().iMinor,	\
																	positionInfoM.Version().iBuild);\
			}	\
		}

// Logs TLbsExternalRequestInfo
	#define SUPLLOG_TLBSEXTERNALREQ(L1)	\
		{	\
		/* Find out what the class type is*/	\
		TUint32 classTypeM = L1.ClassType();	\
		/* Output shared info */	\
		SUPLLOG2(ELogP9, "    - TNetworkType iNetworkType  = %x02X\n", L1.NetworkType());	\
		SUPLLOG2(ELogP9, "    - TRequestType iRequestType  = %x02X\n", L1.RequestType());	\
		SUPLLOG2(ELogP9, "    - TFormatIndicator iRequesterIdFormat  = %x02X\n", L1.RequesterIdFormat());	\
		SUPLLOG2(ELogP9, "    - TFormatIndicator iClientNameFormat  = %x02X\n", L1.ClientNameFormat());	\
		TBuf8<KLbsMaxClientExternalIdSize> externalSizeM;	\
		L1.GetClientExternalId(externalSizeM);	\
		SUPLLOG2_L8(ELogP9, "    - TLbsClientExternalId iClientExternalId  = %S", &externalSizeM);	\
		/* Output type specific info */	\
		if (classTypeM == EExternalRequestInfoClass)	\
			{	\
			const TLbsExternalRequestInfo& info = reinterpret_cast<const TLbsExternalRequestInfo&>(L1);	\
			TBuf8<KLbsMaxRequesterIdSize> requesterIdM;	\
			info.GetRequesterId(requesterIdM);	\
	 		TBuf8<KLbsMaxClientNameSize> clientNameM;	\
			info.GetClientName(clientNameM);	\
			SUPLLOG2_L8(ELogP9, "    - TLbsRequesterId iRequesterId  = %S", &requesterIdM);	\
			SUPLLOG2_L8(ELogP9, "    - TLbsClientName iClientName  = %S", &clientNameM);	\
			SUPLLOG2(ELogP9, "    - TFormatIndicator iRequesterIdCodingScheme  = %x02X\n", info.RequesterIdCodingScheme());	\
			SUPLLOG2(ELogP9, "    - TFormatIndicator iClientNameCodingScheme  = %x02X\n", info.ClientNameCodingScheme());	\
			}	\
		else if (classTypeM == (EExternalRequestInfoClass | EExternalRequestInfoClass2))	\
			{	\
			const TLbsExternalRequestInfo2& info = reinterpret_cast<const TLbsExternalRequestInfo2&>(L1);	\
			TBuf16<KLbsMaxRequesterIdSize2> requesterIdM;	\
			info.GetRequesterId(requesterIdM);	\
	 		TBuf16<KLbsMaxClientNameSize2> clientNameM;	\
			info.GetClientName(clientNameM);	\
			SUPLLOG2(ELogP9, "    - TLbsRequesterId iRequesterId  = %S", &requesterIdM);	\
			SUPLLOG2(ELogP9, "    - TLbsClientName iClientName  = %S", &clientNameM);	\
			}	\
		}

// Logs TLbsNetPosRequestOptionsAssistance
	#define SUPLLOG_REQUESTOPTIONSASSISTANCE(L1)	\
		{	\
		SUPLLOG2(ELogP9, "    - TLbsAsistanceDataGroup iDataRequestMask  = 0x%08X\n", L1.DataRequestMask());	\
		SUPLLOG2(ELogP9, "    - TPositionModuleInfo::TTechnologyType iPosMode  = 0x%04X\n", L1.PosMode());	\
		}

// Logs TLbsNetPosRequestPrivacy
	#define SUPLLOG_POSREQUESTPRIVACY(L1)	\
		{	\
		SUPLLOG2(ELogP9, "    - TLbsRequestAction iAction  = %d", L1.RequestAction());	\
		SUPLLOG2(ELogP9, "    - TLbsRequestAdvice iAdvice  = %d", L1.RequestAdvice());	\
		}

// Logs TLbsNetPosRequestQuality
	#define SUPLLOG_POSREQUESTQUALITY(L1)	\
		{	\
		SUPLLOG2(ELogP9, "    - TReal32 iMinHorizontalAccuracy  = %3.6g\n", L1.MinHorizontalAccuracy());	\
		SUPLLOG2(ELogP9, "    - TReal32 iMinVerticalAccuracy  = %3.6g\n", L1.MinVerticalAccuracy());	\
		SUPLLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iMaxFixTime  = %ld\n", L1.MaxFixTime().Int64());	\
		SUPLLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iMaxFixAge  = %ld\n", L1.MaxFixAge().Int64());	\
		}

// Logs TLbsNetPosRequestOptions
	#define SUPLLOG_POSREQUESTOPTIONS(L1) \
		{	\
		SUPLLOG2(ELogP9, "    - TBool iNewClient  = %d\n", L1.NewClientConnected());	\
		TLbsNetPosRequestQuality qualityM;	\
		L1.GetRequestQuality(qualityM);	\
		SUPLLOG_POSREQUESTQUALITY(qualityM);	\
		}

// Logs TPositionUpdateOptionsBase
	#define SUPLLOG_POSITIONOPTIONBASE(L1)	\
		{	\
		SUPLLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iUpdateInterval  = %ld\n", L1.UpdateInterval().Int64());	\
		SUPLLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iUpdateTimeOut  = %ld\n", L1.UpdateTimeOut().Int64());	\
		SUPLLOG2(ELogP9, "    - TTimeIntervalMicroSeconds iMaxUpdateAge    = %ld\n", L1.MaxUpdateAge().Int64());	\
		}

// Logs TPositionCriteriaBase
	#define SUPLLOG_POSITIONCRITERIABASE(L1)	\
		{	\
		SUPLLOG2(ELogP9, "    - TPositionModuleInfo::TCapabilities iRequiredCaps  = 0x%04X\n", L1.RequiredCapabilities());	\
		TPositionQuality tempPositionQualityM;	\
		L1.GetRequiredQuality(tempPositionQualityM);	\
		SUPLLOG_TPOSITIONQUALITY(tempPositionQualityM);	\
		}

// Logs TPositionModuleStatusBase
	#define SUPLLOG_POSMODULESTATUS(L1)	\
		{	\
		if(L1.PositionClassType() & EPositionModuleStatusClass)	\
			{	\
			const TPositionModuleStatus& positionStatusM = (reinterpret_cast<const TPositionModuleStatus&>(L1));	\
			SUPLLOG2(ELogP9, "    - TDeviceStatus iDeviceStatus  = %d\n", positionStatusM.DeviceStatus());	\
			SUPLLOG2(ELogP9, "    - TDataQualityStatus iDataQualityStatus  = %d\n", positionStatusM.DataQualityStatus());	\
			}	\
		}

// Logs TPositionModuleStatusEventBase
	#define SUPLLOG_POSMODULESTATUSEVENT(L1)	\
		{	\
		SUPLLOG2(ELogP9, "    - TModuleEvent iRequestedEvents  = %u\n", L1.RequestedEvents());	\
		SUPLLOG2(ELogP9, "    - TModuleEvent iOccurredEvents  = %u\n", L1.OccurredEvents());	\
		SUPLLOG2(ELogP9, "    - TSystemModuleEvent iSystemModuleEvent  = %u\n", L1.SystemModuleEvent());	\
		SUPLLOG2(ELogP9, "    - TPositionModuleId  iModuleId  = 0x%08X\n", L1.ModuleId().iUid);	\
		}

// Logs TLbsNetPosRequestMethod
	#define SUPLLOG_POSREQUESTMETHOD(L1)	\
		{	\
		TInt numPosMethodsM = L1.NumPosMethods();	\
		SUPLLOG2(ELogP9, "    - TInt iNumPosMethods  = %d\n", numPosMethodsM);	\
		TLbsNetPosMethod posMethodM;	\
		for(TInt posNumM=0; ((posNumM < numPosMethodsM) && ( posNumM < KLbsMaxNetPosMethods)); ++posNumM)	\
			{	\
			SUPLLOG2(ELogP9, "    - TLbsNetPosMethod posMethod[%d]\n", posNumM);	\
			if((L1.GetPosMethod(posNumM, posMethodM)))	\
				{	\
				SUPLLOG2(ELogP9, "      - TUid iPosMeans  = 0x%08X\n", posMethodM.PosMeans());	\
				SUPLLOG2(ELogP9, "      - TPositionModuleInfo::TTechnologyType iPosMode  = %d\n", posMethodM.PosMode());	\
				}	\
			}	\
		}

// Logs TLbsGpsOptions
	#define SUPLLOG_TLBSGPSOPTIONS(L1)	\
		{	\
		SUPLLOG2(ELogP9, "    - CLbsAdmin::TGpsMode iGpsMode  = 0x%02X\n", L1.GpsMode());	\
		if (L1.ClassType() & ELbsGpsOptionsArrayClass)	\
			{	\
			const TLbsGpsOptionsArray& optionsArrayM = (reinterpret_cast<const TLbsGpsOptionsArray&>(L1));	\
			TInt numOptionsM = optionsArrayM.NumOptionItems();	\
			SUPLLOG2(ELogP9, "    - TUint iNumItems  = %u\n", numOptionsM);	\
			for(TInt optionsNumM=0; ((optionsNumM < numOptionsM) && (optionsNumM < KLbsMaxGpsOptions)); ++optionsNumM)	\
				{	\
				TLbsGpsOptionsItem tempOptionsM;	\
				optionsArrayM.GetOptionItem(optionsNumM, tempOptionsM);	\
				SUPLLOG2(ELogP9, "    - TLbsGpsOptionsItem iOptions[%d]  =\n", optionsNumM);	\
				SUPLLOG2(ELogP9, "      - TPosUpdateType iPosUpdateType  = %d", tempOptionsM.PosUpdateType());	\
				}	\
			}	\
		}

#else
/**
 Standard Logging Macros 
**/
	#define SUPLLOG_BEGIN()
	#define SUPLNEWLOG(L)
	#define SUPLLOG(L1,L2)
	#define SUPLLOG2(L1,L2,L3)
	#define SUPLLOG3(L1,L2,L3,L4)
	#define SUPLLOG4(L1,L2,L3,L4,L5)
	#define SUPLLOG5(L1,L2,L3,L4,L5,L6)
	#define SUPLLOG6(L1,L2,L3,L4,L5,L6,L7)
	#define SUPLLOGHEX(L1,L2,L3)

	#define SUPLLOG2_L8(L1,L2,L3)
	#define SUPLLOG3_L8(L1,L2,L3,L4)
	#define SUPLLOG4_L8(L1,L2,L3,L4,L5)
	
	#define SUPLLOG_ERR(L1,L2)
	#define SUPLLOG_ERR2(L1,L2,L3)
	#define SUPLLOG_ERR3(L1,L2,L3,L4)
	#define SUPLLOG_ERR4(L1,L2,L3,L4,L5)
	#define SUPLLOG_ERR5(L1,L2,L3,L4,L5,L6)
	#define SUPLLOG_ERR6(L1,L2,L3,L4,L5,L6,L7)

	#define SUPLLOG_WARN(L1,L2)
	#define SUPLLOG_WARN2(L1,L2,L3)
	#define SUPLLOG_WARN3(L1,L2,L3,L4)
	#define SUPLLOG_WARN4(L1,L2,L3,L4,L5)
	#define SUPLLOG_WARN5(L1,L2,L3,L4,L5,L6)
	#define SUPLLOG_WARN6(L1,L2,L3,L4,L5,L6,L7)

/**
 Common logging macros (for logging large values)
**/
	#define SUPLLOG_SESSIONID(L1)
	#define SUPLLOG_TPOSITIONINFOBASE(L1)
	#define SUPLLOG_TPOSITIONQUALITY(L1)
	#define SUPLLOG_TPOSITIONMODULE(L1)
	#define SUPLLOG_TLBSEXTERNALREQ(L1)
	#define SUPLLOG_REQUESTOPTIONSASSISTANCE(L1)
	#define SUPLLOG_POSREQUESTPRIVACY(L1)
	#define SUPLLOG_POSREQUESTQUALITY(L1)
	#define SUPLLOG_POSREQUESTOPTIONS(L1)
	#define SUPLLOG_POSITIONOPTIONBASE(L1)
	#define SUPLLOG_POSITIONCRITERIABASE(L1)
	#define SUPLLOG_POSMODULESTATUS(L1)
	#define SUPLLOG_POSMODULESTATUSEVENT(L1)
	#define SUPLLOG_POSREQUESTMETHOD(L1)
	#define SUPLLOG_TLBSGPSOPTIONS(L1)

	#define SUPLLOG_GETPROCESSNAME(L1,L2)
	
	#define SUPLLOG_PDU(L1)

#endif // ENABLE_SUPL_DEV_LOGGER

#endif //SUPL_DEV_LOGGER_MACROS_H

