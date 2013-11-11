/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include "lbsadatacachetest0Step.h"
#include "Te_lbsadatacachetestSuiteDefs.h"
#include "lbsassistancedatacacheapi.h"
#include <lbs/lbsassistancealmanac.h>
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistanceaquisitionassistance.h>


// Writing and reading the cache should not teke more than couple of seconds
const TTimeIntervalSeconds KMaxTimestampAge = 20;

// Some test values; set at the network end and chacked at the AGPS manager end
// to verify the data was set and read OK.

// For RUEPositioningGpsAcquisitionAssistanceReader
const TUint testAcqAssistanceReferenceTime = 0x1234ABCD;

// For RUEPositioningGpsReferenceTimeReader
const TUint testRefTimeWeek = 12;		
const TUint testRefTimeTow1MSec = 567;	
	
// For RUEPositioningGpsAlmanacReader
const  TUint8 testAlmanacGlobalHealth[46] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                          0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
                                          0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
                                          0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                          0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
                                          0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12,
                                          0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                                          0x07, 0x08, 0x09, 0x0A};
const TUint testAlmanacWna = 0x1234ABCD;


Clbsadatacachetest0Step::~Clbsadatacachetest0Step()
	{
	}

Clbsadatacachetest0Step::Clbsadatacachetest0Step()
	{
	SetTestStepName(Klbsadatacachetest0Step);
	}

TVerdict Clbsadatacachetest0Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_lbsadatacachetestSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	   
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsadatacachetest0Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	RAssistanceDataCache cache;
	INFO_PRINTF1(_L("*********** It is a new (TEF) version of the t_lbsassistanceapi test ***********"));
	
	INFO_PRINTF1(_L("Initializing the AD Cache"));
    RAssistanceDataCache::InitializeL();
	INFO_PRINTF1(_L("Openning the AD Cache"));
	cache.OpenL();
	
	SetAssistDataItemsL(cache);
	GetAndValidateAssistDataItemsL(cache);
	
	INFO_PRINTF1(_L("Closing the AD Cache"));
	cache.Close();
	INFO_PRINTF1(_L("Shutting down the AD Cache"));
    RAssistanceDataCache::ShutDownL();

	SetTestStepResult(EPass);		
	return TestStepResult();
	}



TVerdict Clbsadatacachetest0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_lbsadatacachetestSuiteStepBase::doTestStepPostambleL();
	SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}


void Clbsadatacachetest0Step::SetAssistDataItemsL(RAssistanceDataCache& aCache)
	{
	RUEPositioningGpsAcquisitionAssistanceBuilder acquistionAssistanceRootBuilder;
   	RUEPositioningGpsReferenceTimeBuilder         gpsReferenceTimeBuilder;
	RUEPositioningGpsAlmanacBuilder               gpsAlmanacBuilder;
	TTime now;
	now.UniversalTime();
	
	//----------------------------------------------------------------------------------------
	INFO_PRINTF1(_L("Openning Data Builders"));
	acquistionAssistanceRootBuilder.OpenL();
   	gpsReferenceTimeBuilder.OpenL();
	gpsAlmanacBuilder.OpenL();

	//----------------------------------------------------------------------------------------
	INFO_PRINTF1(_L("Creating Acquisition Assistance"));
	acquistionAssistanceRootBuilder.SetTimeStamp(now);
	acquistionAssistanceRootBuilder.SetField(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime, 
	                                          testAcqAssistanceReferenceTime);
	                                          
	INFO_PRINTF1(_L("Setting Acquisition Assistance cache"));
	User::LeaveIfError(aCache.SetAssistDataItem(EAssistanceDataAquisitionAssistance,acquistionAssistanceRootBuilder));

	//----------------------------------------------------------------------------------------
	INFO_PRINTF1(_L("Creating Reference Time"));
	gpsReferenceTimeBuilder.SetTimeStamp(now);
    gpsReferenceTimeBuilder.SetField(TUEPositioningGpsReferenceTime::EGpsWeek,
                                      testRefTimeWeek);
    gpsReferenceTimeBuilder.SetField(TUEPositioningGpsReferenceTime::EGpsTow1Msec, 
                                      testRefTimeTow1MSec);

	INFO_PRINTF1(_L("Setting Reference Time cache"));
    User::LeaveIfError(aCache.SetAssistDataItem(EAssistanceDataReferenceTime, gpsReferenceTimeBuilder));

	//----------------------------------------------------------------------------------------
	INFO_PRINTF1(_L("Creating Almanac"));
	gpsAlmanacBuilder.SetTimeStamp(now);
    gpsAlmanacBuilder.SetField(TUEPositioningGpsAlmanac::EWnA, testAlmanacWna);
    TBuf8<46> buffer;
    buffer.Append(&testAlmanacGlobalHealth[0],sizeof(testAlmanacGlobalHealth));
    gpsAlmanacBuilder.SetField(TUEPositioningGpsAlmanac::ESvGlobalHealth, buffer);
    
	INFO_PRINTF1(_L("Setting Almanac cache"));
    User::LeaveIfError(aCache.SetAssistDataItem(EAssistanceDataAlmanac, gpsAlmanacBuilder));
    
	//----------------------------------------------------------------------------------------
	INFO_PRINTF1(_L("Closing Data Builders"));
	acquistionAssistanceRootBuilder.Close();
   	gpsReferenceTimeBuilder.Close();
	gpsAlmanacBuilder.Close();
	}


void Clbsadatacachetest0Step::GetAndValidateAssistDataItemsL(RAssistanceDataCache& aCache)
	{
	RUEPositioningGpsAcquisitionAssistanceReader acquistionAssistanceRootReader;
	RUEPositioningGpsReferenceTimeReader         gpsReferenceTimeReader;
	RUEPositioningGpsAlmanacReader               gpsAlmanacReader;
	TTime timeStamp;
	TTime now;
	now.UniversalTime();

	//----------------------------------------------------------------------------------------		
	INFO_PRINTF1(_L("Openning Data Readers"));
	acquistionAssistanceRootReader.OpenL();
	gpsReferenceTimeReader.OpenL();
	gpsAlmanacReader.OpenL();

	//----------------------------------------------------------------------------------------
	INFO_PRINTF1(_L("Getting Acquisition Assistance cache"));
	User::LeaveIfError(aCache.GetAssistDataItem(EAssistanceDataAquisitionAssistance, 
										        acquistionAssistanceRootReader));
	timeStamp = acquistionAssistanceRootReader.TimeStamp();

	INFO_PRINTF1(_L("Validating timestamp"));
	if (timeStamp + KMaxTimestampAge < now) User::Leave(KErrCorrupt);
		
	
	//----------------------------------------------------------------------------------------
	INFO_PRINTF1(_L("Getting Reference Time cache"));
	User::LeaveIfError(aCache.GetAssistDataItem(EAssistanceDataReferenceTime, 
												 gpsReferenceTimeReader));
	timeStamp = gpsReferenceTimeReader.TimeStamp();
	
	INFO_PRINTF1(_L("Validating timestamp"));
	if (timeStamp + KMaxTimestampAge < now) User::Leave(KErrCorrupt);
	
	INFO_PRINTF1(_L("Validating data"));
	TUint week;
	TUint tow1MSec;	                                            
	User::LeaveIfError(gpsReferenceTimeReader.GetField(TUEPositioningGpsReferenceTime::EGpsTow1Msec,tow1MSec));
	User::LeaveIfError(gpsReferenceTimeReader.GetField(TUEPositioningGpsReferenceTime::EGpsWeek,week));

	if ((tow1MSec != testRefTimeTow1MSec) || (week != testRefTimeWeek))
		User::Leave(KErrCorrupt);
    
	//----------------------------------------------------------------------------------------
	INFO_PRINTF1(_L("Getting Almanac cache"));
	User::LeaveIfError(aCache.GetAssistDataItem(EAssistanceDataAlmanac, 
												 gpsAlmanacReader));
	timeStamp = gpsAlmanacReader.TimeStamp();
	
	INFO_PRINTF1(_L("Validating timestamp"));
	if (timeStamp + KMaxTimestampAge < now) User::Leave(KErrCorrupt);

	INFO_PRINTF1(_L("Validating data"));
	TBuf8<46> buffer; // Check that the data read is as expected
	buffer.Append(&testAlmanacGlobalHealth[0],sizeof(testAlmanacGlobalHealth));

	TBuf8<46> readBuffer;
	User::LeaveIfError(gpsAlmanacReader.GetField(TUEPositioningGpsAlmanac::ESvGlobalHealth,readBuffer));

	TUint readWna;
	User::LeaveIfError(gpsAlmanacReader.GetField(TUEPositioningGpsAlmanac::EWnA, readWna));

	if ((readBuffer != buffer) || (readWna != testAlmanacWna))
		User::Leave(KErrCorrupt);

	//----------------------------------------------------------------------------------------
	INFO_PRINTF1(_L("Closing Data Readers"));
	acquistionAssistanceRootReader.Close();
	gpsReferenceTimeReader.Close();
	gpsAlmanacReader.Close();
	}
