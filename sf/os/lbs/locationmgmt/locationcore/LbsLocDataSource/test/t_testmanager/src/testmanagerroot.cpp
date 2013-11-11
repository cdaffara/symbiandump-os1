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
// sirfmanagerroot.cpp
// Run from eshell using the entry: "t_testmanager.exe def", which loads
// the default gps module and carries out a TAP request.  To do a TBP request
// change the GPS options in ConstructL() to pass in the correct GPS Mode.
// 
//


//************************************************************************************************************
// System
#include <bacline.h>
#include <e32cons.h>
#include <ecom/ecom.h>

// Project
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbssatellite.h>
#include <lbs/lbsgpsmeasurement.h>
#include <lbs/lbsadmin.h>
#include "testmanagerroot.h"
#include "lbsdevloggermacros.h"

_LIT(KTimeFormat, "%H:%T:%S.%C");
_LIT(KSatTimeFormat, "%Y:%M:%D  %H:%T:%S.%C");

const TUid KSirfUid = {0x10282249};
const TTimeIntervalMicroSeconds KMaxTimeMargin		= 100000;//100ms
const TTimeIntervalMicroSeconds KFirstCallDelay		= 1000000;//1s
const TTimeIntervalMicroSeconds	KInterval			= 1000000;//1s
const TTimeIntervalMicroSeconds	KMaxFixTime			= 1000000;//1s
//TTimeIntervalMicroSeconds32	KNextRequestAfter	= 10000000;	// 10s
const TReal32 					KTargetAccuracy		= 2;		// 2.0m

_LIT(KLbsLocationManagerRoot,"LbsLocationManagerRoot");


//************************************************************************************************************
//   CLocationManagerRoot
//************************************************************************************************************
CLocationManagerRoot* CLocationManagerRoot::NewL(RTest& aTest, TInt aCount)
   {
	CLocationManagerRoot* self = new(ELeave) CLocationManagerRoot(aTest, aCount);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
   };


//------------------------------------------------------------------------------------------------------------
CLocationManagerRoot::CLocationManagerRoot(RTest& aTest, TInt aCount)
	:CTimer(CActive::EPriorityStandard),
	iTest(aTest),
	iShutdown(EFalse),
	iCount(aCount),
	iLoop(0),
	iTargetTime(0),
	iModule(NULL)
	{
	CActiveScheduler::Add(this);
	}


//------------------------------------------------------------------------------------------------------------
void CLocationManagerRoot::ConstructL()
	{
	CTimer::ConstructL();
	iModule = CLbsLocationSourceGpsBase::NewL(*this, KSirfUid);

	//Set Gps mode (using standard TGpsMode)
	TLbsGpsOptions gpsOptions;
	
	//gpsOptions.SetGpsMode(CLbsAdmin::EGpsAutonomous);
	gpsOptions.SetGpsMode(CLbsAdmin::EGpsPreferTerminalAssisted);
	//gpsOptions.SetGpsMode(CLbsAdmin::EGpsAlwaysTerminalAssisted);
	//gpsOptions.SetGpsMode(CLbsAdmin::EGpsPreferTerminalBased);
	
	iModule->SetGpsOptions(gpsOptions);
	
	iTargetTime.UniversalTime();
	CompleteSelf();


//*************************************************************************************
//test	
/*
	TUint16 gpsWeek    = 1377;
	TUint32 gpsSeconds = 405182000;
	
	TDateTime satInitDate(1980, EJanuary,6-1,0,0,0,0);
	TTime sTime(satInitDate);//(KGpsTimeStart);
	//satTime += static_cast<TTimeIntervalDays>(7*(gpsWeek - 1024));
	TInt32 gpsDay = 7*gpsWeek;
	TInt64 gpsMicroSeconds = gpsSeconds;
	gpsMicroSeconds *= 1000;
	sTime += static_cast<TTimeIntervalDays>(gpsDay);
	sTime += static_cast<TTimeIntervalMicroSeconds>(gpsMicroSeconds);


	TBuf<100> sTimeStr;
	sTime.FormatL(sTimeStr, KSatTimeFormat);
	TDateTime sDT = sTime.DateTime();
	iTest.Printf(_L("Sat time: %04d:%02d:%02d %S\n"),sDT.Year(), sDT.Month()+1, sDT.Day()+1, &sTimeStr);
	goto xxx;
*/
//*************************************************************************************
	}


//------------------------------------------------------------------------------------------------------------
CLocationManagerRoot::~CLocationManagerRoot()
	{
	delete iModule;
	iModule = NULL;
	REComSession::FinalClose();
	}

//------------------------------------------------------------------------------------------------------------
void CLocationManagerRoot::RunL()
	{
	static TBool firstReq = ETrue;
	iLoop++;
	
	if (iShutdown)
		{
		CActiveScheduler::Stop();
		}
	else if ((iCount > 0) && (iLoop > iCount))
		{
		//CActiveScheduler::Stop();
		iTest.Printf(_L("Sending Power Advice Close\n"));
		LBSLOG(ELogP5, "NEW TEST: Sending Power Advice Close\n");
		iModule->AdvisePowerMode(CLbsLocationSourceGpsBase::EPowerModeClose);
		}
	else
		{
		iTargetTime.UniversalTime();
		
		if (firstReq)
			{
			firstReq = EFalse;
			iTargetTime += KFirstCallDelay;
			}
		else
			{
			iTargetTime+= TTimeIntervalMicroSeconds(KInterval);
			}
			
		iQuality.SetMinHorizontalAccuracy(KTargetAccuracy);
		iQuality.SetMinVerticalAccuracy(KTargetAccuracy);
		iQuality.SetMaxFixTime(KMaxFixTime);
		
		
		// Logging
			{		
			//FormatL(dt, _L("%F%Y-%M-%D %H:%T:%S\n")); 279 console->Printf(_L("%d ") ... Entry(
			TTime cTime;
			cTime.UniversalTime();
			TBuf<100> cTimeStr;
			cTime.FormatL(cTimeStr, KTimeFormat);

			TBuf<100> tTimeStr;
			iTargetTime.FormatL(tTimeStr, KTimeFormat);
			iTest.Printf(_L("\n[%d/%d] Requesting Location. Time: %S. Target Time: %S. Max Fix Time: %ds\n"), iLoop, iCount, &cTimeStr, &tTimeStr, KMaxFixTime.Int64()/1000000);
			LBSLOG(ELogP5, "");
			LBSLOG4(ELogP5, "NEW TEST: Requesting Location. Time: %S. Target Time: %S. Max Fix Time: %ds\n", &cTimeStr, &tTimeStr, KMaxFixTime.Int64()/1000000);
			}
		
		iModule->RequestLocationUpdate(iTargetTime, iQuality);
		if (iTargetTime == 0)
			{
			HighRes(TTimeIntervalMicroSeconds32(1*KMaxFixTime.Int64()));
			}
		else
			{
			AtUTC (iTargetTime + TTimeIntervalMicroSeconds32(1*KMaxFixTime.Int64()));
			}
		}
	}

//------------------------------------------------------------------------------------------------------------
void CLocationManagerRoot::DoCancel()
	{
	}

//------------------------------------------------------------------------------------------------------------	
void CLocationManagerRoot::CompleteSelf(TInt aReason)
	{
	TRequestStatus* pStat = &iStatus;
	   
	User::RequestComplete(pStat, aReason);
	SetActive();
	}

//************************************************************************************************************
//   From MLbsLocationSourceGpsObserver

//------------------------------------------------------------------------------------------------------------
void CLocationManagerRoot::UpdateLocation(TInt aStatus, const TPositionInfoBase* aPosInfoArray[], TInt aNumItems, const TTime& aTargetTime)
	{
	TTime cTime;
	cTime.UniversalTime();
	TBuf<100> cTimeStr;
	TInt err = KErrNone;
	TRAP(err, cTime.FormatL(cTimeStr, KTimeFormat);)
	if(err)
		User::Panic(KLbsLocationManagerRoot, err);
	
	//Loop through all the elements of aPosInfoArray
	for(TInt i=0; i<aNumItems; i++)
		{
		//Check to see whether the data item is MeasurementInfo or SatelliteInfo 
		if(aPosInfoArray[i]->PositionClassType() & EPositionGpsMeasurementInfoClass)
			{
			//Measurement Info
			const TPositionGpsMeasurementInfo* posInfo = reinterpret_cast<const TPositionGpsMeasurementInfo*>(aPosInfoArray[i]);
			
			//Print out the number of measurements
			TUint numOfMeas = posInfo->NumMeasurements();
			iTest.Printf(_L("Number of Measurements: %d\n"), numOfMeas);
			
			//Loop through all the measurements printing out the data
			iTest.Printf(_L("   Sat Meas[SatId | CNo | Doppler | WholeGpsChips | FractGpsChips | MultiPathInd | PseudoRangeRMSErr]\n"));
			for(TInt i=0; i<numOfMeas; ++i)
				{
				TPositionGpsMeasurementData posData;
				posInfo->GetMeasurementData(i, posData);
				
				TInt satId = posData.SatelliteId();
				TInt cNo = posData.CarrierNoiseRatio();
				TInt doppler = posData.Doppler();
				TInt wholeGpsChips = posData.WholeGpsChips();
				TInt fractionalGpsChips = posData.FractionalGpsChips();
				TInt multiPathIndicator = posData.MultiPathIndicator();
				TUint pseudoRangeRMSError = posData.PseudoRangeRmsError();
				iTest.Printf(_L("	[ %d | %d | %d | %d | %d | %d | %d ]\n"), satId, cNo, doppler, wholeGpsChips, 
							fractionalGpsChips, multiPathIndicator, pseudoRangeRMSError);
				}
			}
		else if(aPosInfoArray[i]->PositionClassType() & EPositionSatelliteInfoClass)
			{
			//Satellite Info
			
			// Check if quality criteria met
			const TPositionSatelliteInfo* posInfo = reinterpret_cast<const TPositionSatelliteInfo*>(aPosInfoArray[i]);
			TPosition position;
			TCourse course;
			posInfo->GetPosition(position);
			posInfo->GetCourse(course);
			TReal32 horAccur = position.HorizontalAccuracy();
			TReal32 vertAccur = position.VerticalAccuracy();
			TReal32 posLat	= position.Latitude();
			TReal32 posLong = position.Longitude();
			TReal32 posAlt = position.Altitude();
			TReal32 couSpeed = course.Speed();
			TReal32 couSpeedAccur = course.SpeedAccuracy();
			TReal32 couHeading = course.Heading();
			TReal32 couHeadingAccur = course.HeadingAccuracy();
			TTime 	sTime	= posInfo->SatelliteTime();
			TTime 	pTime	= position.Time();
			
			TInt satInView = posInfo->NumSatellitesInView();
			TInt satUsed   = posInfo->NumSatellitesUsed();
			
			LBSLOG5(ELogP5, "NEW TEST: Location Update Received. Time: %S. Accuracy: %g/%g. Status: %d\n",  &cTimeStr, horAccur, vertAccur, aStatus);
			iTest.Printf(_L("[%S] Location Update Received. Status: %d\n"),&cTimeStr, aStatus);
			
			TBuf<100> timeStr;
			TRAP(err, sTime.FormatL(timeStr, KSatTimeFormat);)
			if(err)
				User::Panic(KLbsLocationManagerRoot, err);
			
			TDateTime dateTime = sTime.DateTime();
			iTest.Printf(_L("Sat time:               %04d:%02d:%02d %S\n"),dateTime.Year(), dateTime.Month()+1, dateTime.Day()+1, &timeStr);
			
			TRAP(err, pTime.FormatL(timeStr, KSatTimeFormat);)
			if(err)
				User::Panic(KLbsLocationManagerRoot, err);
			dateTime = pTime.DateTime();
			iTest.Printf(_L("Pos time:               %04d:%02d:%02d %S\n"),dateTime.Year(), dateTime.Month()+1, dateTime.Day()+1, &timeStr);

			iTest.Printf(_L("Position[Lat/Long/Alt]: %.4g/%.4g/%.4g\n"), posLat, posLong, posAlt);
			iTest.Printf(_L("Accuracy[Hor/Vert]:     %.4g/%.4g\n"), horAccur, vertAccur);
			iTest.Printf(_L("Speed[Speed/Accuracy]:  %.4g/%.4g\n"), couSpeed, couSpeedAccur);
			iTest.Printf(_L("Heading[Heading/Accu]:  %.4g/%.4g\n"), couHeading, couHeadingAccur);
			iTest.Printf(_L("Sat[InView/Used]:       %d/%d\n"), satInView, satUsed);
			
			iTest.Printf(_L("   Sat Info[IsUsed | Id | SigStren | Azimuth | Elevation]\n"));
			for (TInt i = 0; i < satInView; i++)
				{
				TSatelliteData satData;
				posInfo->GetSatelliteData(i,satData);
				TInt isUsed 	= satData.IsUsed();
				TInt satId 		= satData.SatelliteId();
				TInt sigStren 	= satData.SignalStrength();
				TReal32 azim 	= satData.Azimuth();
				TReal32 elev 	= satData.Elevation();
				iTest.Printf(_L("           |   %d   | %02d |    %02d   | %3.4g | %3.4g\n"), isUsed, satId, sigStren, azim, elev);
				}
			}
		}
	
	if (aTargetTime != iTargetTime)
		{
		iTest.Printf(_L("Error: aTargetTime != iTargetTime\n"));
		LBSLOG(ELogP5, "NEW TEST: Error: aTargetTime != iTargetTime\n");
		}
		
	if (cTime < iTargetTime)
		{
		iTest.Printf(_L("Error: cTime < iTargetTime\n"));
		LBSLOG(ELogP5, "NEW TEST: Error: cTime < iTargetTime\n");
		}
		
	if (cTime > (iTargetTime + iQuality.MaxFixTime() + KMaxTimeMargin))
		{
		iTest.Printf(_L("Error: cTime > (iTargetTime + iQuality.MaxFixTime() + KMaxTimeMargin)\n"));
		LBSLOG(ELogP5, "NEW TEST: Error: cTime > (iTargetTime + iQuality.MaxFixTime() + KMaxTimeMargin)\n");
		}
		
	//After(KNextRequestAfter);
	//CompleteSelf();
	}

//------------------------------------------------------------------------------------------------------------
void CLocationManagerRoot::UpdateDeviceStatus(TPositionModuleStatus::TDeviceStatus /*aDeviceStatus*/)
	{
	}

//------------------------------------------------------------------------------------------------------------
void CLocationManagerRoot::UpdateDataQualityStatus(TPositionModuleStatus::TDataQualityStatus /*aDataQuality*/)
	{
	}

//------------------------------------------------------------------------------------------------------------
TInt CLocationManagerRoot::GetAssistanceDataItem(TLbsAssistanceDataItem /*aItem*/, RDataReaderRootBase& /*aDataRoot*/, TTime& /*aTimeStamp*/)
	{
	iTest.Printf(_L("NEW TEST: Assit data get\n"));
	LBSLOG(ELogP5, "NEW TEST: Assit data get\n");

	return KErrNone;
	}

//------------------------------------------------------------------------------------------------------------
TInt CLocationManagerRoot::GetAssistanceDataItemTimeStamp(TLbsAssistanceDataItem /*aItem*/, TTime& /*aTimeStamp*/)
	{
	return KErrNotSupported;
	}

//------------------------------------------------------------------------------------------------------------
void CLocationManagerRoot::RequestAssistanceData(TLbsAsistanceDataGroup /*aDataItemMask*/)
	{
	iTest.Printf(_L("NEW TEST: Assit Data Req\n"));
	LBSLOG(ELogP5, "NEW TEST: Assit Data Req\n");
	
	//Returns Network error to mainlogic, which should cause it to switch to autonomous in specific modes
	//iModule->AssistanceDataEvent(KPositionNetworkErrorBase, aDataItemMask);
	}

//------------------------------------------------------------------------------------------------------------
void CLocationManagerRoot::Shutdown()
	{
	iTest.Printf(_L("Shutdown\n"));
	LBSLOG(ELogP5, "NEW TEST: Shutdown\n");

	iShutdown=ETrue;
	CompleteSelf();
	}
