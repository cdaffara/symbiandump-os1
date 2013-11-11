/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This file define all the common values thoughout your test project
*
*/



/**
 @file Te_LbsBtGpsPsySuiteDefs.h
 @internalTechnology
*/
#ifndef __TE_LBSBTGPSPSY_SUITEDEFS_H__
#define __TE_LBSBTGPSPSY_SUITEDEFS_H__

// Time constants for human interactions
const TInt K1Second = 1000000;

// Value for BTPSY settings.
const TInt KPropertyWatcherNumber = 5;
const TInt KHundredPercent = 100;
const TInt KHeapOffset = 256;

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_LbsBtGpsPsySuiteInt,"Test");
_LIT(KTe_LbsBtGpsPsySuiteString,"Address"); // Primary Device
_LIT(KTe_LbsBtGpsPsySuiteSecondaryString,"SecondaryAddress"); // Secondary device
_LIT(KTe_LbsBtGpsPsySuiteNotifierType,"NotifierType"); // This indicates the max number of allowed attempts on the primary device
_LIT(KTe_LbsBtGpsPsySuiteDeviceType,"DeviceType");
// For test step panics
_LIT(KTe_LbsBtGpsPsySuitePanic,"Te_LbsBtGpsPsySuite");

_LIT(KThreadLogFile, "c:\\logs\\threadprints.txt");
_LIT(KThreadNumberLogFile, "c:\\logs\\Thread%d.txt");

_LIT(KtdBT1,"tdBT1");
_LIT(KtdBT6,"tdBT6");

_LIT(KCreateThread, "Create the thread");
_LIT(KStartThread, "Start the thread");

_LIT(KNewlineTxt, "");
_LIT(KFileNewlineTxt, "\r\n");
_LIT(KStartTxt, "Environment configuration start");
_LIT(KStopTxt, "Environment configuration stop");
_LIT(KEnvCleaningStartTxt, "Environment cleaning start");
_LIT(KEnvCleaningStopTxt, "Environment cleaning stop");
_LIT(KOpenMLFWTxt, "Open connection to MLFW server");
_LIT(KOpenPSYTxt, "Open BT GPS PSY" );
_LIT(KOpenForTimeoutPSYTxt, "Open BT GPS PSY for timeouting" );
_LIT(KFixInfoHeaderTxt, "***************FIX INFO**************");
_LIT(KFixInfoFooterTxt, "*************************************");
_LIT(KMakePositionRequestTxt, "Make position request");
_LIT(KMakeTenPositionRequestTxt, "Make ten position requests");
_LIT(KSetRequestorTxt, "Set requestor to BT GPS PSY" );
_LIT(KSetRequestorForTimeoutTxt, "Set requestor to BT GPS PSY for timeouting" );

_LIT(KGetPsyInformationTxt, "Get BT GPS PSY module information");
_LIT(KPsyInformationAreCorrectTxt, "The BT GPS PSY module Informations are correct");
_LIT(KWaitModuleReady, "Wait module to be ready");
_LIT(KWaitOneSecond, "Wait for 1 second");

_LIT(KSettingUpdateFailedTxt, "Setting the update options to BT GPS failed, code %d" );
_LIT(KSettingRequestorFailedTxt, "Setting the requestor failed for BT GPS PSY, code %d" );
_LIT(KGettingModuleStatusFailedTxt, "Getting module status of BT GPS PSY failed, code %d");
_LIT(KPSYNotRetreivedTxt, "BT GPS PSY could not be retrieved, code %d" );
_LIT(KPSYNotRetreivedButGracefullTxt, "BT GPS PSY could not be found, but we leave as expected with the error code %d");
_LIT(KPSYNotOpenTxt, "Connection to BT GPS PSY could not be opened, code %d" );
_LIT(KMLFWNotOpenTxt, "MLFW server could not be opened, code %d" );
_LIT(KOpenFailedTxt, "Open failed, got %d " );
_LIT(KRequestFailedTxt, "Request failed, got %d " );
_LIT(KPositionRequestFailedTxt, "position request failed with code %d" );
_LIT(KGetValueFailedTxt, "Get value failed, code %d" );

_LIT(KWrongDatumTxt, "Wrong datum, wanted 270510633, got %d ");
_LIT(KSameTimestampTxt, "Timestamp same" );
_LIT(KNoTimestampTxt, "We don't get a Timestamp, and we should always have one: %d" );

_LIT(KPositionData, "Position Data");
_LIT(KCourseData, "Course Data");
_LIT(KSatelliteData, "Satellite Data");

_LIT(KPositionInformation, "The position was: Longitude => %f, Latitude => %f, Altitude => %f");
_LIT(KAccuracies, "Accuracies were: Horizontal accuracy  => %f, Vertical accuracy  => %f" );
_LIT(KCourse, "Speed is %f, heading %f, and course is %f" );
_LIT(KCourseAccuracies, "Speed accuracy was %f, heading accuracy was %f, and course accuracy was %f" );
_LIT(KDateString, "%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B" );
_LIT(KSatelliteTime, "Satellite time is %D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B" );
_LIT(KSatelliteAcc, "Satellite accuracies are: horizontal  %f, vertical %f, time %f" );
_LIT(KSatelliteNumbers, "Satellites in view %d, used for fix %d" );
_LIT(KSatelliteDataSummary, "Satellite : id %d, azimuth %f, elevation %f, signal strength %d" );
_LIT(KSatelliteIndex, "Satellite%d" );
_LIT(KDatum, "Datum is: %d" );


_LIT(KBTPSYName, "Bluetooth GPS");
_LIT(KWrongPSYName, "Wrong name for PSY");
_LIT(KWrongModuleID, "Wrong module id: %d");

_LIT(KNotAvailable, "Not available");
_LIT(KWrongVersion60, "Wrong version should be 60 is %d");
_LIT(KNotaTerminal, "Not a terminal, but: %d");
_LIT(KNotExternal, "Not external");
_LIT(KNotSupportedClass, "Not expected classes supported: %d");
_LIT(KNoCorrectCapabilities, "No correct capabilities: %d");
_LIT(KWrongTTFF, "Wrong TTFF value %d, wanted 80");
_LIT(KWrongTTNF, "Wrong TTNF value %d, wanted 1");
_LIT(KCanNotOpenBTPSY, "BTGPSPY could not be opened"); 
_LIT(KFailedUpdateOptions, "Update options failed");
_LIT(KBTPSYRequestorName, "BT GPS PSY");
_LIT(KCanNotSetRequestor, "Could not set requestor, code %d");
_LIT(KFieldExtracting, "Field extracting...");
_LIT(KFielPrinting, "Field printing");
_LIT(KValuePositionFieldHorizontalSpeed, "EPositionFieldHorizontalSpeed was %f");
_LIT(KValuePositionFieldHeading, "EPositionFieldHeading was %f");
_LIT(KValuePositionFieldNMEASentences, "EPositionFieldNMEASentences was %d");
_LIT(KNMEAsentenceFromHPositionGenericInfo, "HPositionGenericInfo: NMEA sentence %d:");
_LIT(KDeviceNotNMEA,"device not nmea");

_LIT(KValuePositionFieldSatelliteNumInView, "EPositionFieldSatelliteNumInView was %d");
_LIT(KValuePositionFieldSatelliteNumUsed, "EPositionFieldSatelliteNumUsed was %d");
_LIT(KValuePositionFieldSatelliteTime, "EPositionFieldSatelliteTime %D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
_LIT(KValuePositionFieldSatelliteHorizontalDoP, "EPositionFieldSatelliteHorizontalDoP was %f");
_LIT(KValuePositionFieldSatelliteVerticalDoP, "EPositionFieldSatelliteVerticalDoP was %f");
_LIT(KValuePositionFieldSatellitePositionDoP, "EPositionFieldSatellitePositionDoP was %f");
_LIT(KValuePositionFieldSatelliteSeaLevelAltitude, "EPositionFieldSatelliteSeaLevelAltitude was %f");
_LIT(KStartThreadTenUpdates, "Start the a thread which makes ten periodic position updates");

_LIT(KDisableEmulatorPanic, "Disable that panic crashes the emulator");
_LIT(KErrorCreatingThread, "Error when creating thread with code %d");
_LIT(KCheckModuleActive, "Check module is inactive");

_LIT(KStartListeningModuleEvents, "Start listening module events");
_LIT(KWaitModuleReportInitializing, "Wait for the module to be reported initializing");
_LIT(KRoundNumber, "Round %d");
_LIT(KCheckModuleStatus, "Check module status");
_LIT(KWrongModuleStatus, "wrong module status wanted %d, got %d");
_LIT(KContinueListeningModuleEvents, "Continue listening module events");
_LIT(KWaitRequestComplete, "Wait the request to complete");
_LIT(KMakeRequest, "Make request");
_LIT(KMakeTestDriverMonitorThread, "Make the test driver to monitor the thread");

#endif
