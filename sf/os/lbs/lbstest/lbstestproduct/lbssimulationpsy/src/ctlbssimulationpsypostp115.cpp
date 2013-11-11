// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDES
#include "ctlbssimulationpsypostp115.h"
#include "ctlbssimulationpsyposnmeafilereader.h"
#include "ctlbssimulationpsyfixconstants.h"


// CONSTANTS
const TInt KEPosAndPsyOverheadTime = 2000000; // Accepted EPos overhead delay

// KMaxNrOfFixes is the maximum nr of requests we accept. If expected result has 
// not been achieved after this amount of fixes the test is considered to be unsuccessful
const TInt KMaxNrOfFixes = 300;

// KMinNrOfFixes is the minimum nr of fixes we require to be allowed to say that
// the expected result was achieved.
const TInt KMinNrOfFixes = 25;

// Nr of tests in this TP
const TInt KNrOfTests = 13;

// Nr of NMEA PSY types existing, 
const TInt KNrOfNMEATypes = 6;

const TInt KMaxStringLength = 50;

_LIT(KService, "service");

_LIT(KName0, "TP115 - SimPSY, NMEA raw data");
_LIT(KName1, "TP115 - SimPSY, power-up time");
_LIT(KName2, "TP115 - SimPSY, time to fix");
_LIT(KName3, "TP115 - SimPSY, deterministic failure");
_LIT(KName4, "TP115 - SimPSY, random failure");
_LIT(KName5, "TP115 - SimPSY, altitude");
_LIT(KName6, "TP115 - SimPSY, latitude and longitude");
_LIT(KName7, "TP115 - SimPSY, movement");
_LIT(KName8, "TP115 - SimPSY, bad input parameters");
_LIT(KName9, "TP115 - Extended1 NMEA raw data");
_LIT(KName10, "TP115 - Extended2 NMEA raw data");
_LIT(KName11, "TP115 - SimPSY, correct fixes");
_LIT(KName12, "TP115 - SimPSY, correct timing after cancel");

 
#ifdef __WINS__
  // Nmea raw data files
  _LIT(KMagellanGPS315, "z:\\system\\test\\testdata\\MagellanGPS315.nme");
  _LIT(KFix10MagellanGPS315, "z:\\system\\test\\testdata\\Fix10MagellanGPS315.nme");
  _LIT(KMagellanTracker, "z:\\system\\test\\testdata\\MagellanTracker.nme");
  _LIT(KGarmin12cx, "z:\\system\\test\\testdata\\garmin12cx.nme");
  _LIT(KGarmin3plus, "z:\\system\\test\\testdata\\garmin3plus.nme");
  _LIT(KGarminetrexlegend, "z:\\system\\test\\testdata\\garminetrexlegend.nme");
  _LIT(KMagellanMeridian, "z:\\system\\test\\testdata\\magellanmeridianplatinum.nme");
  //Ini files to corresponding nmea raw data file
  _LIT(KMagellanTrackerIni, "z:\\system\\test\\testdata\\MagellanTracker.ini");
  _LIT(KGarmin12cxIni, "z:\\system\\test\\testdata\\garmin12cx.ini");
  _LIT(KGarmin3plusIni, "z:\\system\\test\\testdata\\garmin3plus.ini");
  _LIT(KGarminetrexlegendIni, "z:\\system\\test\\testdata\\garminetrexlegend.ini");
  _LIT(KMagellanMeridianIni, "z:\\system\\test\\testdata\\magellanmeridianplatinum.ini");
#else
   // Nmea raw data files
  _LIT(KMagellanGPS315, "c:\\system\\test\\testdata\\MagellanGPS315.nme");
  _LIT(KFix10MagellanGPS315, "c:\\system\\test\\testdata\\Fix10MagellanGPS315.nme");
  _LIT(KMagellanTracker, "c:\\system\\test\\testdata\\MagellanTracker.nme");
  _LIT(KGarmin12cx, "c:\\system\\test\\testdata\\garmin12cx.nme");
  _LIT(KGarmin3plus, "c:\\system\\test\\testdata\\garmin3plus.nme");
  _LIT(KGarminetrexlegend, "c:\\system\\test\\testdata\\garminetrexlegend.nme");
  _LIT(KMagellanMeridian, "c:\\system\\test\\testdata\\magellanmeridianplatinum.nme");
#endif

// ================= MEMBER FUNCTIONS =======================
  
// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
 CT_LbsSimulationPSYPosTp115::CT_LbsSimulationPSYPosTp115(CT_LbsServer& aParent, TInt aIndex): CT_LbsPortedStepBase(aParent)
  	{  
  	TBuf<KMaxStringLength> aName;
  	GetIndexName(aIndex,aName);
  	SetTestStepName(aName);
  	iIndex = aIndex;
  	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsSimulationPSYPosTp115::~CT_LbsSimulationPSYPosTp115()
  	{
  	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::StartL()
    {
    // Setup test environment
  
    iNrOfRequests = 0;
    ConnectL();
    SetupPsyL(iUidSimulationPsy);

    TRAPD(err, ReplaceSwitchInTrapL(iIndex));

    if (err != KErrNone)
        {
        ClosePositioner();
        }

    Disconnect();
    // Wait for EPos server to terminate
    User::After(2500000);

    if (err != KErrNone)
        {
        User::Leave(err);
        }    
  }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::GetIndexName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::GetIndexName(TInt aIndex, TDes& aName) const
    {
    const TDesC* names[KNrOfTests] = {&KName0, &KName1, &KName2, &KName3, &KName4,
                                      &KName5, &KName6, &KName7, &KName8, &KName9, &KName10, &KName11, &KName12};
    aName = *names[aIndex];
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::TestSimulationNmeaReaderL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::TestSimulationNmeaReaderL(const TDesC& aNMEASource)
    {
    const TInt KMaxNmeaSentenceLength = 82; // The maximum length of an NMEA sentence
    _LIT(KNmeaDataMismatchError1, "NMEA data mismatch. NMEA data from Simulation Psy:");
    _LIT(KNmeaDataMismatchError2, "NMEA data read from input file:");
    _LIT(KNmeaSetError, "Not possible to set retrieval of request data");
    _LIT(KNmeaSetError2, "The field is not in the desired fields list");
    _LIT(KNmeaNoDataError, "No data returned from NMEA positioner");
    _LIT(KNmeaGetError, "Error when calling GetValue() %d");
    _LIT(KNmeaGetError2, "Error when calling GetValue(), field could not be found %d");
    _LIT(KNmeaPositionError, "Position (lat/long/alt) is not a real finite number");
    _LIT(KReqError, "Position could not be requested, err %d");
    _LIT(KStart, "Testing TestSimulationNmeaReaderL() PSY:");
    _LIT(KEnd, "TestSimulationNmeaReaderL() passed");

    TInt err = 0;
    
    // Set correct data file
    SetSimDataFileL(aNMEASource);
        
    _LIT(KServiceName, "SAAA");

    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

    TBuf<200> buf;
    buf.Append(KStart);
	buf.Append(aNMEASource);
    INFO_PRINTF1(buf);

    //Check that SimPSY supports TPositionInfo   
    TPositionInfo posInfo = TPositionInfo();

     // Request data from simulation psy
     err = PerformSyncRequest(KServiceName, &posInfo);
    
    AssertTrueL(err == KErrNone, KReqError, err);
    
    TPosition position = TPosition();
    posInfo.GetPosition(position);
    
    TReal64 lat = position.Latitude();
    TReal64 lon = position.Longitude();
   
    // FrBo Add check that nmea data is correctly compiled into position
    if (!Math::IsFinite(lat))
    {
      LogErrorAndLeaveL(KNmeaPositionError);
    }
    if (!Math::IsFinite(lon))
    {
      LogErrorAndLeaveL(KNmeaPositionError);
    }

    ClosePositioner();

    // Check that  Simulation PSY supports HPositionGenericInfo

    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));
    CT_LbsSimulationPSYPosNmeaFileReader* fileReader = CT_LbsSimulationPSYPosNmeaFileReader::NewLC(aNMEASource);

    // Do everything two times just to test that it is
    // possible to do two NotifyPositionUpdate()
    for (TInt j=0; j<2; j++)
      {
      TBuf8<KMaxNmeaSentenceLength> nmeaDataFromFile;
      
      HPositionGenericInfo* genericInfo = HPositionGenericInfo::NewLC();
      
      AssertTrueL(genericInfo->SetRequestedField(EPositionFieldNMEASentences) == KErrNone, KNmeaSetError, -1);
      AssertTrueL(genericInfo->SetRequestedField(EPositionFieldNMEASentencesStart) == KErrNone, KNmeaSetError, -1);
      
      // Request data from simulation psy
      err = PerformSyncRequest(KServiceName, genericInfo);
      
      AssertTrueL(err == KErrNone, KReqError, err);
      
      TPosition position = TPosition();
      genericInfo->GetPosition(position);
      
      TReal64 lat = position.Latitude();
      TReal64 lon = position.Longitude();
      
      // FrBo Add check that nmea data is correctly compiled into position
      if (!Math::IsFinite(lat))
        {
        LogErrorAndLeaveL(KNmeaPositionError);
        }
      if (!Math::IsFinite(lon))
        {
        LogErrorAndLeaveL(KNmeaPositionError);
        }
      
      //Check that reuqested fields has been set    
      AssertTrueL(genericInfo->IsRequestedField(EPositionFieldNMEASentences), KNmeaSetError2, -1);
      AssertTrueL(genericInfo->IsRequestedField(EPositionFieldNMEASentencesStart), KNmeaSetError2, -1);
      
      TUint8 nrOfSentences;
      err = genericInfo->GetValue(EPositionFieldNMEASentences, nrOfSentences);
      
      AssertTrueL(err != KErrNotFound, KNmeaGetError2, err);
      AssertTrueL(err == KErrNone, KNmeaGetError, err);
      
      // Should always return something
      AssertTrueL(nrOfSentences != 0, KNmeaNoDataError, -1);
      
      for (TInt i=0; i<nrOfSentences; i++)
        {
        TPtrC8 nmeaDataFromPsy;
        TUint16 sentnr = static_cast<TUint16> (EPositionFieldNMEASentencesStart + i);
        
        genericInfo->GetValue(sentnr, nmeaDataFromPsy);
        
        User::LeaveIfError(fileReader->ReadNmeaSentence(nmeaDataFromFile));
        
        // Analyze NMEA data
        if (nmeaDataFromPsy != nmeaDataFromFile)
          {
          ERR_PRINTF1(KNmeaDataMismatchError1);
          INFO_PRINTF1(KNmeaDataMismatchError2);
          User::Leave(KErrGeneral);
          }
        } // End for
      CleanupStack::PopAndDestroy(genericInfo);
      } // End for 
    CleanupStack::PopAndDestroy(fileReader);
    ClosePositioner();
    INFO_PRINTF1(KEnd);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::Extended2TestSimulationNmeaReaderL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::Extended2TestSimulationNmeaReaderL(const TDesC& aNMEASource)
    {
    _LIT(KStart, "Testing Extended2TestSimulationNmeaReaderL() PSY: ");
    _LIT(KEnd, "Extended2TestSimulationNmeaReaderL() passed");

    TInt err = 0;
    // Set correct nmea data file
    SetSimDataFileL(aNMEASource);

    TBuf<200> buf;
    buf.Append(KStart);
    buf.Append(aNMEASource);
    INFO_PRINTF1(buf);

    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

    CT_LbsSimulationPSYPosNmeaFileReader* fileReader = CT_LbsSimulationPSYPosNmeaFileReader::NewLC(aNMEASource);
    TPositionCourseInfo courseInfo = TPositionCourseInfo();
    
    // Request data from simulation psy
    err = PerformSyncRequest(KService, &courseInfo);

    _LIT(KERROR, "PerformSyncRequest Status should be OK when requesting TPositionCourseInfo");
    AssertTrueL(err == KErrNone, KERROR, 0);
    //<<New Code
    //TODO The NMEA data from fileReader should be compared to data form courseInfo ?
    
    //>>Orginal Code
    CleanupStack::PopAndDestroy(fileReader);
    
    ClosePositioner();
    INFO_PRINTF1(KEnd);
    }


void CT_LbsSimulationPSYPosTp115::Extended1TestSimulationNmeaReaderL(const TDesC& aNMEASource)
  {
    
    // Set correct nmea data file
    SetSimDataFileL(aNMEASource);
    
    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));
    
    // Create HPositionGenericInfo object to pass to PerformSyncRequest method
    HPositionGenericInfo* genericInfo = HPositionGenericInfo::NewLC();

    _LIT(KErrKNMEA, "KNMEA Set error");
    AssertTrueL(genericInfo->SetRequestedField(EPositionFieldSatelliteSeaLevelAltitude) == KErrNone, 
    		KErrKNMEA, -1);
    
    // Request data from simulation psy
    TInt err = PerformSyncRequest(KService, genericInfo);

    //Assure that everything is OK
    _LIT(KErrOpenSimpsy1, "Error when openeing simpsy1");
    AssertTrueL(err == KErrNone, KErrOpenSimpsy1, err);
          
     TReal32 satelliteAltitude;
    genericInfo->GetValue(EPositionFieldSatelliteSeaLevelAltitude, satelliteAltitude);
    
    CleanupStack::PopAndDestroy(genericInfo);
    
    ClosePositioner();
    
    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));
    
   TPositionInfo posInfo = TPositionInfo();
   err = PerformSyncRequest(KService, &posInfo);
   _LIT(KErrOpenSimpsy2, "Error when openeing simpsy2");
   AssertTrueL(err == KErrNone, KErrOpenSimpsy2, err);
   
   TPosition myPos = TPosition();
        posInfo.GetPosition(myPos);
   
    ClosePositioner();
}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::TestPowerUpTimeL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::TestPowerUpTimeL()
    {
  // Time granularity on WINS is 0.1s (see ESLI-5HZKHN)
#ifdef __WINS__
    const TInt KPowerUpTime = 4900000;
#else
    const TInt KPowerUpTime = 5000000;
#endif

#ifdef __WINS__
    _LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationPowerUp.sps");
    //unused _LIT(KIniSource, "z:\\system\\test\\testdata\\simulationpsySIM.ini");
#else
    _LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationPowerUp.sps");
    //unused _LIT(KIniSource, "c:\\system\\test\\testdata\\simulationpsySIM.ini");
#endif

    _LIT(KError1, "Error when requesting position");
    _LIT(KStart, "Testing TestPowerUpTimeL()");
    _LIT(KEnd, "TestPowerUpTimeL() passed");
    INFO_PRINTF1(KStart);
    
    // Set correct data file
    SetSimDataFileL(KSimulationSource);
    
    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

    TTime requestStart;
    requestStart.UniversalTime();
    
    // Request a pos. fix
    TPositionInfo posInfo = TPositionInfo();
    TInt err = PerformSyncRequest(KService, &posInfo);
    if (err != KErrNone)
      {
      LogErrorAndLeaveL(KError1);
      }

    // Measure power-up time
    TTime requestFinished;
    requestFinished.UniversalTime();
    TTimeIntervalMicroSeconds requestTime = 
        requestFinished.MicroSecondsFrom(requestStart);

    // Time to complete first request should not be less than power-up time
    if (requestTime.Int64() < KPowerUpTime || 
        requestTime.Int64() > KPowerUpTime + KEPosAndPsyOverheadTime)
        {
        _LIT(KErr, "Power Up time = %d, is out of range");
        iMsg.Format(KErr, requestTime.Int64());
        LogErrorAndLeaveL(iMsg);
        }

    ClosePositioner();
    INFO_PRINTF1(KEnd);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::TestTimeToFixL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::TestTimeToFixL()
    {
  // Time granularity on WINS is 0.1s (see ESLI-5HZKHN)
#ifdef __WINS__    
    const TInt KTimeToFixMin = 2900000;
    const TInt KTimeToFixMax = 5100000;
#else
    const TInt KTimeToFixMin = 3000000;
    const TInt KTimeToFixMax = 5000000;
#endif
    const TInt KNrOfFixes    = 5;

#ifdef __WINS__
    _LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationTTF.sps");
    //unused _LIT(KIniSource, "z:\\system\\test\\testdata\\simulationpsySIM.ini");
#else
    _LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationTTF.sps");
    //unused _LIT(KIniSource, "c:\\system\\test\\testdata\\simulationpsySIM.ini");
#endif

    // Set correct data file
    SetSimDataFileL(KSimulationSource);
    
    _LIT(KError1, "Error when requesting position");
    _LIT(KStart, "Testing TestTimeToFixL()");
    _LIT(KEnd, "TestTimeToFixL() passed");
    INFO_PRINTF1(KStart);
    
    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));
   
    TPositionInfo posInfo = TPositionInfo();

    TTime requestStart, requestFinished;
    TTimeIntervalMicroSeconds requestTime;
    
    // Request TTF for KNrOfFixes pos. fixes
    for (TInt i = 1; i <= KNrOfFixes; i++)
        {
        requestStart.UniversalTime();
        TInt err = PerformSyncRequest(KService, &posInfo);
        if (err != KErrNone)
          {
          LogErrorAndLeaveL(KError1);
          }

        requestFinished.UniversalTime();
        requestTime = requestFinished.MicroSecondsFrom(requestStart);

        // Verify that request time is kept within boundaries
        if (requestTime.Int64() < KTimeToFixMin ||
            requestTime.Int64() > KTimeToFixMax + KEPosAndPsyOverheadTime)
            {
            _LIT(KErr, "Time to fix (request nr. %d) = %d, is out of range");
            iMsg.Format(KErr, i, requestTime.Int64());
            LogErrorAndLeaveL(iMsg);
            }
        }

    ClosePositioner();
    INFO_PRINTF1(KEnd);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::TestDeterministicFailureL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CT_LbsSimulationPSYPosTp115::TestDeterministicFailureL()
    {
    const TInt KDeterministicFailureProbability = 3;
    const TInt KNrOfFixes = 15;

#ifdef __WINS__
    _LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationDeterministic.sps");
    //unused _LIT(KIniSource, "z:\\system\\test\\testdata\\simulationpsySIM.ini");
#else
    _LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationDeterministic.sps");
    //unused _LIT(KIniSource, "c:\\system\\test\\testdata\\simulationpsySIM.ini");
#endif
    
    // Set correct data file
    SetSimDataFileL(KSimulationSource);

    _LIT(KStart, "Testing TestDeterministicFailureL()");
    _LIT(KEnd, "TestDeterministicFailureL() passed");
    INFO_PRINTF1(KStart);
    _LIT(KError1, "Wrong error occurred, was %d should be %d");
    
    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

    TPositionInfo posInfo = TPositionInfo();
    // Request KNrOfFixes pos. fixes
    for (TInt i = 1; i <= KNrOfFixes; i++)
        {
        TBool willFail = (i % KDeterministicFailureProbability == 0);
        TInt expectedErrorCode = willFail ? KPositionQualityLoss : KErrNone;

        TInt err = PerformSyncRequest(KService, &posInfo);
        if (err != expectedErrorCode)
          {
          TBuf<100> buf;
          buf.Format(KError1, err, expectedErrorCode);
          LogErrorAndLeaveL(buf);
          }
        }

    ClosePositioner();
    INFO_PRINTF1(KEnd);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::TestRandomFailureL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::TestRandomFailureL()
    {
    // KRandomFailureProbability is the probability that a fix fails (percentage)
    const TReal64 KRandomFailureProbability = 33.33;
    _LIT(KMsg, "Random fail rate was achieved, %f");

#ifdef __WINS__
    _LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationRandom.sps");
    //unused _LIT(KIniSource, "z:\\system\\test\\testdata\\simulationpsySIM.ini");
#else
    _LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationRandom.sps");
    //unused _LIT(KIniSource, "c:\\system\\test\\testdata\\simulationpsySIM.ini");
#endif


	// Set correct data file
    SetSimDataFileL(KSimulationSource);
    
    _LIT(KStart, "Testing TestRandomFailureL()");
    _LIT(KEnd, "TestRandomFailureL() passed");
    INFO_PRINTF1(KStart);
    
    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

    TPositionInfo posInfo = TPositionInfo();
    TInt nrOfFailedFixes = 0;
    TReal64 failRate = 0;
    TBool resultsAchieved = EFalse;
    TInt err = KErrNone;

    // Request as many pos. fixes as necessary to achieve fail rate
    for (TInt i = 1; i <= KMaxNrOfFixes && !resultsAchieved; i++)
        {
        err = PerformSyncRequest(KService, &posInfo);
        
        if (err != KErrNone)
            {
            nrOfFailedFixes++;
            }

        if (i > KMinNrOfFixes)
            {
            failRate = (100.0 * nrOfFailedFixes) / i;
            //need to make gap bit larger
            if (Abs(failRate - KRandomFailureProbability) <= 6)
                {
                iMsg.Format(KMsg, failRate);
                INFO_PRINTF1(iMsg);
                resultsAchieved = ETrue;
                }
            }
        }

    if (!resultsAchieved)
        {
        iMsg.Format(KMsg, failRate);
        LogErrorAndLeaveL(iMsg);
        }

    ClosePositioner();
    INFO_PRINTF1(KEnd);
    }
// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::TestAltitudeL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::TestAltitudeL()
    {
    const TInt KVerticalAccuracy = 30;
    const TInt KMeanAltitude = 0;
    _LIT(KMeanMsg, "Mean altitude of about zero meters was achieved, %f m");
    _LIT(KStdDevMsg, "Standard deviation of altitude was achieved, %f");

#ifdef __WINS__
    _LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationPowerUp.sps");
    //unused _LIT(KIniSource, "z:\\system\\test\\testdata\\simulationpsySIM.ini");
#else
    _LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationPowerUp.sps");
    //unused _LIT(KIniSource, "c:\\system\\test\\testdata\\simulationpsySIM.ini");
#endif
   
    // Set correct data file
    SetSimDataFileL(KSimulationSource);
    
    _LIT(KStart, "Testing TestAltitudeL()");
    _LIT(KEnd, "TestAltitudeL() passed");
    _LIT(KError1, "Error when requesting location");
    INFO_PRINTF1(KStart);

    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

    TPositionInfo posInfo = TPositionInfo();
    TReal altitudeVariance = 0;
    TReal altitudeStandardDeviation = 0;
    TReal altitudeSum = 0;
    TReal squaredAltitudeSum = 0;
    TReal altitudeMean = 0;
    TBool meanAchieved = EFalse, stdDevAchieved = EFalse;
    
    //need to change it because of the random genetatro which is not ideal
    const TInt KAcceptedMeanError = 4;
	const TInt KAcceptedStdError = 12;

    // Request as many pos. fixes as necessary to achieve expected result
    for (TInt i = 1; i <= KMaxNrOfFixes && !(meanAchieved && stdDevAchieved); i++)
        {
        TInt err = PerformSyncRequest(KService, &posInfo);
           
        if (err != KErrNone)
          {
          LogErrorAndLeaveL(KError1);
          }
        TPosition myPos = TPosition();
        posInfo.GetPosition(myPos);
        TReal32 altitude = myPos.Altitude();
        TReal64 latitude = myPos.Latitude();
                
        TBuf<100> buf;
        _LIT(KAltitude, "Altitude %g Lat: %g");
        buf.Format(KAltitude, altitude, latitude);
        INFO_PRINTF1(buf);
        
        // calculate altitudeMean
        altitudeSum += altitude;
        altitudeMean = altitudeSum / i;

        // calculate altitudeVariance
        squaredAltitudeSum += altitude*altitude;
        if (i > 1)
            {
            altitudeVariance = ((squaredAltitudeSum-(altitudeSum * altitudeSum / i)) / (i-1));
            }

        if (i > KMinNrOfFixes)
            {
            // Check altitudeMean
            TReal res = Abs(altitudeMean - KMeanAltitude);

            // Log
            TBuf<100> buf;
            _LIT(KRes, "Mean. Alt.: %.9g");
            buf.Format(KRes, res);
            INFO_PRINTF1(buf);

            // Check
            if (!meanAchieved &&
                Abs(altitudeMean - KMeanAltitude) <= KAcceptedMeanError)
                {
                meanAchieved = ETrue;
                _LIT(KRes1, "meanAchieved: ETrue");
                INFO_PRINTF1(KRes1);
                
                iMsg.Format(KMeanMsg, altitudeMean);
                INFO_PRINTF1(iMsg);
                }

            // Check altitudeStdMean
            if (!stdDevAchieved)
                {
                User::LeaveIfError(Math::Sqrt(altitudeStandardDeviation, altitudeVariance));

                // Check standard deviation of altitude 
                TReal res = Abs(altitudeStandardDeviation - KVerticalAccuracy);

                // Log
                _LIT(KStdDev, "Std. Dev. Alt.: %.9g");
                buf.Format(KStdDev, res);
                INFO_PRINTF1(buf);

                // Check
                if (res <= KAcceptedStdError)
                    {
                    stdDevAchieved = ETrue;
                    _LIT(KRes1, "stdDevAchieved: ETrue");
                    INFO_PRINTF1(KRes1);
                    
                    iMsg.Format(KStdDevMsg, altitudeStandardDeviation);
                    INFO_PRINTF1(iMsg);
                    }
                }
            }
        }

    // Log result
    if (!meanAchieved || !stdDevAchieved)
        {
        if (!meanAchieved)
            {
            iMsg.Format(KMeanMsg, altitudeMean);
            ERR_PRINTF1(iMsg);
            }
        if (!stdDevAchieved)
            {
            User::LeaveIfError(Math::Sqrt(altitudeStandardDeviation, altitudeVariance));
            iMsg.Format(KStdDevMsg, altitudeStandardDeviation);
            ERR_PRINTF1(iMsg);
            }
        User::Leave(KErrGeneral);
        }

    ClosePositioner();
    INFO_PRINTF1(KEnd);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::TestLongLatitudeL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::TestLongLatitudeL()
    {
    const TInt KHorizontalAccuracy = 20;
    const TReal64 KMeanLatitude = 78.338500;
    const TReal64 KMeanLongitude = 56.338500;
    // On earth surface KMetersPerDegree corresponds to an angle of 1 degree
    // Following formula was used: 
    // KMetersPerDegree = 2*pi*R / 360, where R = radius of earth = 6371010 m
    const TReal KMetersPerDegree = 111195.10117748393677876603122949;
    
    _LIT(KMeanMsg, "Mean latitude and longitude was calculated to ; %10.7f ; %10.7f; respectively");
    _LIT(KStdDevMsg, "Standard deviation of lat. and long. was calculated to ; %10.7f; %10.7f ; m resp.");
    

#ifdef __WINS__
    _LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationNoMove.sps");
    //unused _LIT(KIniSource, "z:\\system\\test\\testdata\\simulationpsySIM.ini");
#else
    _LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationNoMove.sps");
    //unused _LIT(KIniSource, "c:\\system\\test\\testdata\\simulationpsySIM.ini");
#endif

	// Set correct data file
    SetSimDataFileL(KSimulationSource);
    
    _LIT(KStart, "Testing TestLongLatitudeL()");
    _LIT(KEnd, "TestLongLatitudeL() passed");
    _LIT(KError1, "Error when requesting location");
    INFO_PRINTF1(KStart);
    
    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));

    TPositionInfo posInfo = TPositionInfo();

    TReal longVar = 0,                 latVar = 0;
    TReal longStdDev = 0,              latStdDev = 0;
    TReal longitudeSum = 0,            latitudeSum = 0; 
    TReal sumOfSquaredLongitudes = 0,  sumOfSquaredLatitudes = 0;
    TReal longitudeMean = 0,           latitudeMean = 0;
    TBool meanValuesAchieved = EFalse, stdDevAchieved = EFalse;

    // Request as many pos. fixes as necessary to achieve expected result
    for (TInt i = 1; i <= KMaxNrOfFixes && !(meanValuesAchieved && stdDevAchieved); i++)
        {
        TInt err = PerformSyncRequest(KService, &posInfo);
        if (err != KErrNone)
          {
          LogErrorAndLeaveL(KError1);
          }

        TPosition myPos = TPosition();
        posInfo.GetPosition(myPos);

        TReal64 longitude = myPos.Longitude();
        TReal64 latitude = myPos.Latitude();
        
        TBuf<100> buf;
        _LIT(KLongLat, "Long %g Lat: %g");
        buf.Format(KLongLat, longitude, latitude);
        INFO_PRINTF1(buf);

        // calculate longitudeMean
        longitudeSum += longitude;
        longitudeMean = longitudeSum / i;

        // calculate latitudeMean
        latitudeSum += latitude;
        latitudeMean = latitudeSum / i;

		const TInt KAcceptedMeanError = 2;
		const TInt KAcceptedStdError = 4;
		
        // calculate longitudeVariance and latitudeVariance
        sumOfSquaredLongitudes += longitude*longitude;
        sumOfSquaredLatitudes += latitude*latitude;
        if (i > 1)
            {
            longVar = ((sumOfSquaredLongitudes-(longitudeSum * longitudeSum / i)) / (i-1));
            latVar = ((sumOfSquaredLatitudes-(latitudeSum * latitudeSum / i)) / (i-1));
            }

        if (i > KMinNrOfFixes)
            {
            TReal meanLat = Abs(latitudeMean - KMeanLatitude) * KMetersPerDegree;
            TReal meanLong = Abs(longitudeMean - KMeanLongitude) * KMetersPerDegree;

            // Log
            _LIT(KMeanLongLat, "Mean. Long %.9g, Mean. Lat: %.9g");
            buf.Format(KMeanLongLat, meanLong, meanLat);
            INFO_PRINTF1(buf);

            // Check latitudeMean and longitudeMean
            if (!meanValuesAchieved &&
                Abs(latitudeMean - KMeanLatitude) * KMetersPerDegree <= KAcceptedMeanError &&
                Abs(longitudeMean - KMeanLongitude) * KMetersPerDegree <= KAcceptedMeanError)
                {
                meanValuesAchieved = ETrue;
                // Log purposes
                iMsg.Format(KMeanMsg, Abs(latitudeMean - KMeanLatitude) * KMetersPerDegree, Abs(longitudeMean - KMeanLongitude) * KMetersPerDegree);

                INFO_PRINTF1(iMsg);
                }

            if (!stdDevAchieved)
                {
                // Check standard deviation
                User::LeaveIfError(Math::Sqrt(latStdDev, latVar));
                User::LeaveIfError(Math::Sqrt(longStdDev, longVar));
                
                // Log
                TReal latDev = Abs(latStdDev * KMetersPerDegree - KHorizontalAccuracy);
                TReal longDev = Abs(longStdDev * KMetersPerDegree - KHorizontalAccuracy);                
                _LIT(KStdLongLat, "Std.Dev. Long.: %.9g, Std.Dev. Lat.: %.9g");
                buf.Format(KStdLongLat, longDev, latDev);
                //INFO_PRINTF1(buf);

                // Check
                if (Abs(latStdDev * KMetersPerDegree - KHorizontalAccuracy) <= KAcceptedStdError &&
                    Abs(longStdDev * KMetersPerDegree - KHorizontalAccuracy) <= KAcceptedStdError)
                    {
                    stdDevAchieved = ETrue;
                    // Log purposes
                    iMsg.Format(KStdDevMsg, Abs(latStdDev * KMetersPerDegree - KHorizontalAccuracy), Abs(longStdDev * KMetersPerDegree - KHorizontalAccuracy));
                    INFO_PRINTF1(iMsg);
                    }
                }
            }
        }

    // Log result
    if (!meanValuesAchieved || !stdDevAchieved)
        {
        if (!meanValuesAchieved)
            {
            iMsg.Format(KMeanMsg, latitudeMean, longitudeMean);
            ERR_PRINTF1(iMsg);
            }
        if (!stdDevAchieved)
            {
            User::LeaveIfError(Math::Sqrt(latStdDev, latVar));
            User::LeaveIfError(Math::Sqrt(longStdDev, longVar));
            iMsg.Format(KStdDevMsg, latStdDev * KMetersPerDegree, longStdDev * KMetersPerDegree);
            ERR_PRINTF1(iMsg);
            }
        User::Leave(KErrGeneral);
        }

    ClosePositioner();
    INFO_PRINTF1(KEnd);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::TestMovementL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CT_LbsSimulationPSYPosTp115::TestMovementL()
    {
    const TInt KNrOfFixes = 10;

#ifdef __WINS__
    _LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationPowerUp.sps");
#else
    _LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationPowerUp.sps");
#endif
    
	// Set correct data file
    SetSimDataFileL(KSimulationSource);

	// Open simulationPSY
    User::LeaveIfError(OpenPositionerByName(iUidSimulationPsy));
    
    _LIT(KStart, "Testing TestMovementL()");
    _LIT(KEnd, "TestMovementL() passed");
    _LIT(KError1, "Error when requesting location");
    INFO_PRINTF1(KStart);

    // Make origin position request
     TPositionInfo posInfo = TPositionInfo();
     TInt err = PerformSyncRequest(KService, &posInfo);
     if (err != KErrNone)
     {
       LogErrorAndLeaveL(KError1);
     }

    TPosition myPos = TPosition();
    posInfo.GetPosition(myPos);
    TTime timeOfFix = myPos.Time();
    TReal64 longitude = myPos.Longitude();
    TReal64 latitude  = myPos.Latitude();

    // Request the rest of the KMaxNrOfFixes pos. fixes with no 
    // errors and verify direction of movement
    for (TInt i = 2; i <= KNrOfFixes; i++)
        {
        // save preceding values
        TTime timeOfLastFix = timeOfFix;
        TReal64 lastLongitude = longitude;
        TReal64 lastLatitude = latitude;

        // get new position info
        TInt err = PerformSyncRequest(KService, &posInfo);
        if (err != KErrNone)
        {
          LogErrorAndLeaveL(KError1);
        }
        posInfo.GetPosition(myPos);
        longitude = myPos.Longitude();
        latitude  = myPos.Latitude();
        timeOfFix = myPos.Time();

        TTimeIntervalMicroSeconds fixTimeDiff = timeOfFix.MicroSecondsFrom(timeOfLastFix);
        if (fixTimeDiff.Int64() <= 0)
            {
            _LIT(KErr, "Time difference between position fixes was non-positive");
            LogErrorAndLeaveL(KErr);
            }

        // We are moving south-west at the norhern earth globe. Thus, Longitude 
        // should increase and latitude decrease. We don't care that the
        // course changes dynamically as fixes are given. Speed is slow and course
        // changes are small.
        if (longitude <= lastLongitude)
            {
            _LIT(KErr, "Longitude was not increased as expected. Last longitude = %10.7f, current longitude = %10.7f");
            iMsg.Format(KErr, lastLongitude, longitude);
            LogErrorAndLeaveL(iMsg);
            }
        if (latitude >= lastLatitude)
            {
            _LIT(KErr, "Latitude was not decreased as expected. Last latitude = %10.7f, current latitude = %10.7f");
            iMsg.Format(KErr, lastLatitude, latitude);
            LogErrorAndLeaveL(iMsg);
            }
        }
    ClosePositioner();
    INFO_PRINTF1(KEnd);
    }
// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::TestBadInputFileL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::TestBadInputFileL()
    {
    // This test tests only the syntax error caused by a misspelled parameter name.
    // There are LOTS of other faults one can cause as developer. However, these
    // are not tested here but should be tested in unit tests.

#ifdef __WINS__
    _LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationBadSyntax.sps");
	_LIT(KSimulationSource1, "z:\\system\\test\\testdata\\simulationBadSyntax1.sps");
	_LIT(KSimulationSource2, "z:\\system\\test\\testdata\\simulationBadSyntax2.sps");
	_LIT(KSimulationSource3, "z:\\system\\test\\testdata\\simulationBadSyntax3.sps");
	_LIT(KSimulationSource4, "z:\\system\\test\\testdata\\simulationBadSyntax4.sps");
	_LIT(KSimulationSource5, "z:\\system\\test\\testdata\\simulationBadSyntax5.sps");
    //unused _LIT(KIniSource, "z:\\system\\test\\testdata\\simulationpsySIM.ini");
    _LIT(KGarmin12cx, "z:\\system\\test\\testdata\\garmin12cxBadSyntax.nme");
    //unused _LIT(KGarmin12cxIni, "z:\\system\\test\\testdata\\garmin12cxBadSyntax.ini");
    _LIT(KGarmin3plus, "z:\\system\\test\\testdata\\garmin3plussatlost.nme");
    //unused _LIT(KGarmin3plusIni, "z:\\system\\test\\testdata\\garmin3plussatlost.ini");
    //unused _LIT(KWrongSimDataFileNameIni, "z:\\system\\test\\testdata\\WrongSimDataFileName.ini");
    _LIT(KWrongSimDataFileNameSource, "z:\\system\\test\\testdata\\WrongSimDataFileName.txt");
#else
    _LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationBadSyntax.sps");
	_LIT(KSimulationSource1, "c:\\system\\test\\testdata\\simulationBadSyntax1.sps");
	_LIT(KSimulationSource2, "c:\\system\\test\\testdata\\simulationBadSyntax2.sps");
	_LIT(KSimulationSource3, "c:\\system\\test\\testdata\\simulationBadSyntax3.sps");
	_LIT(KSimulationSource4, "c:\\system\\test\\testdata\\simulationBadSyntax4.sps");
	_LIT(KSimulationSource5, "c:\\system\\test\\testdata\\simulationBadSyntax5.sps");
    //unused _LIT(KIniSource, "c:\\system\\test\\testdata\\simulationpsySIM.ini");
    _LIT(KGarmin12cx, "c:\\system\\test\\testdata\\garmin12cxBadSyntax.nme");
    //unused _LIT(KGarmin12cxIni, "c:\\system\\test\\testdata\\garmin12cxBadSyntax.ini");
    _LIT(KGarmin3plus, "c:\\system\\test\\testdata\\garmin3plussatlost.nme");
    //unused _LIT(KGarmin3plusIni, "c:\\system\\test\\testdata\\garmin3plussatlost.ini");
    //unused _LIT(KWrongSimDataFileNameIni, "c:\\system\\test\\testdata\\WrongSimDataFileName.ini");
    _LIT(KWrongSimDataFileNameSource, "c:\\system\\test\\testdata\\WrongSimDataFileName.txt");
#endif

    _LIT(KStart, "Testing TestBadInputFileL()");
    _LIT(KEnd, "TestBadInputFileL() passed");
    _LIT(KServiceName, "Service");
    TInt errorsFound = KErrNone;
    
    INFO_PRINTF1(KStart);
	//Verification of error report EKJL-5M9JNF
	CheckBadInputHelperL(KSimulationSource );
	CheckBadInputHelperL(KSimulationSource1);
	CheckBadInputHelperL(KSimulationSource2);
	CheckBadInputHelperL(KSimulationSource3);
	CheckBadInputHelperL(KSimulationSource4);
	CheckBadInputHelperL(KSimulationSource5);

    // Test that a .txt file generates error
    SetSimDataFileL(KWrongSimDataFileNameSource);
    
    TInt err = OpenPositionerByName(iUidSimulationPsy);
     
    if (err == KErrNone)
        {
        _LIT(KErr, "ERROR: The data file with .txt name was unexpectedly successfully read");
        INFO_PRINTF1(KErr);
        errorsFound++;
        ClosePositioner();
        }
        
    if (err != KErrNotSupported)
        {
        _LIT(KErr, "ERROR: The positioner returned wrong error, error code = %d");
        iMsg.Format(KErr, err);
        INFO_PRINTF1(iMsg);
        errorsFound++;
        }

    // Test bad syntax for Garmin12cx below
    // However not really bad syntax but causing problem in the SimPSY parser!
    SetSimDataFileL(KGarmin12cx);

    err = OpenPositionerByName(iUidSimulationPsy);
    if (err != KErrNone)
        {
        _LIT(KErr, "ERROR: Simulation PSY could not be openend, error : %d");
        TBuf<100> buf;
        buf.Format(KErr, err);
        INFO_PRINTF1(buf);
        errorsFound++;
        }

    TPositionInfo posInfo = TPositionInfo();
     // Request data from simulation psy
     err = PerformSyncRequest(KServiceName, &posInfo);
    
     // Behaviour changed in iteration 11 - KPositionQualityLoss has nothing to do with quality loss in PSY
     _LIT(KReqError, "PerformSyncRequest should return KErrNone from SimPSY");
    // The burst does not contain any position info and
    // thus the quality factor should be 0
    AssertTrueL(err == KErrNone, KReqError, err);
   
    ClosePositioner();

    // Test bad syntax for Garmin3plus below
    // However not really bad syntax but causing problem in the SimPSY parser!
    // Set data file
    SetSimDataFileL(KGarmin3plus);

    err = OpenPositionerByName(iUidSimulationPsy);
    if (err != KErrNone)
        {
        _LIT(KErr, "ERROR: Simulation PSY could not be openend, error : %d");
        TBuf<100> buf;
        buf.Format(KErr, err);
        INFO_PRINTF1(buf);
        errorsFound++;
        }
        
    // Test nmea data when satellite data is lost as well as 
    // test that the simulationPSY starts over reading from file when EOF is reached
    // 50 is enough to read through the whole file
    //
    for (TInt k=0;k<50;k++)
      {
      TPositionInfo posInfo2 = TPositionInfo();
      // Request data from simulation psy
      err = PerformSyncRequest(KServiceName, &posInfo2);

      if (err != KErrNone)
        {
        if (err != KPositionQualityLoss)
          {
          _LIT(KErr, "PerformSyncRequest returned error other than KPositionQualityLoss, error: %d in burst nr: %d");
          TBuf<100> buf;
          buf.Format(KErr, err, k);
          INFO_PRINTF1(buf);
          errorsFound++;
          }
        }
      }
      
    ClosePositioner();
    if (errorsFound != KErrNone) 
    	{
    	_LIT(KErrorAndLeave, "Errors found in TestBadInputFileL");
    	LogErrorAndLeaveL(KErrorAndLeave);
    	}
    INFO_PRINTF1(KEnd);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::CheckBadInputHelperL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::CheckBadInputHelperL(const TDesC& aSimulationSource)
	{
    // Set correct data file
    SetSimDataFileL(aSimulationSource);

    TInt err = OpenPositionerByName(iUidSimulationPsy);
    if (err == KErrNone)
        {
        TPositionInfo posInfo = TPositionInfo();
      	// Request data from simulation psy
        _LIT(KKallesService, "KallesService");
      	err = PerformSyncRequest(KKallesService, &posInfo);

      	if (err == KErrCorrupt) 
      	{
      	_LIT(KInfoCorrupt, "********** Corrupt");
      	INFO_PRINTF1(KInfoCorrupt);
      	ClosePositioner();
      	}
      	else
			{
			ClosePositioner();
        	_LIT(KErr, "The simulation data file was unexpectedly successfully read");
        	LogErrorAndLeaveL(KErr);
			}
        }
    if (err != KErrCorrupt)
        {
        _LIT(KErr, "The positioner was unable to read input file, error code = %d");
        iMsg.Format(KErr, err);
        LogErrorAndLeaveL(iMsg);
        }
	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::TestCorrectFixL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::TestCorrectFixL()
	{
	_LIT(KServiceName, "SAAA");
	_LIT(KStart, "Testing TestCorrectFixL()");
    _LIT(KEnd, "TestCorrectFixL() passed");
    INFO_PRINTF1(KStart);

	// Set correct data file
    SetSimDataFileL(KFix10MagellanGPS315);
    
    TInt err = OpenPositionerByName(iUidSimulationPsy);
    if (err != KErrNone)
        {
        _LIT(KErr, "Simulation PSY could not be openend, error : %d");
        TBuf<100> buf;
        buf.Format(KErr, err);
        LogErrorAndLeaveL(buf);
        }

    // The file contains 10 fixes, but 11 is read to ensure that the reading starts all over
	// again
    for (TInt k=1;k<12;k++)
		{
		TPositionInfo posInfo = TPositionInfo();
		// Request data from simulation psy
		err = PerformSyncRequest(KServiceName, &posInfo);
		if (k == 11)
			{
			CheckPositionL(posInfo,1);
			}
		else 
			{
			CheckPositionL(posInfo,k);
			}
		}
	
    ClosePositioner();
    INFO_PRINTF1(KEnd);

	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::TestCorrectTimingAfterCancelL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::TestCorrectTimingAfterCancelL()
	{
	TTimeIntervalMicroSeconds timetofixTime_MAX = TTimeIntervalMicroSeconds(8000000);
	TTimeIntervalMicroSeconds timetofixTime_MIN = TTimeIntervalMicroSeconds(4000000);
	
#ifdef __WINS__
    _LIT(KSimulationSource, "z:\\system\\test\\testdata\\simulationTimingAfterCancel.sps");
    // unused _LIT(KIniSource, "z:\\system\\test\\testdata\\simulationpsySIM.ini");
#else
    _LIT(KSimulationSource, "c:\\system\\test\\testdata\\simulationTimingAfterCancel.sps");
    // unused _LIT(KIniSource, "c:\\system\\test\\testdata\\simulationpsySIM.ini");
#endif
	_LIT(KStart, "Testing CorrectTimingAfterCancelL()");
    _LIT(KEnd, "CorrectTimingAfterCancelL() passed");
   
	INFO_PRINTF1(KStart);

	// Set data file
    SetSimDataFileL(KSimulationSource);
    	
	//Connecting to Position Server
	_LIT(KConnectErr, "Error when connecing to Position Server,  %d");
	TInt err = iPosServer.Connect();
	AssertTrueL(err == KErrNone, KConnectErr, err); //Leave if error

	//Open Positioner Module
	_LIT(KOpenErr, "Error when opening positioner,  %d");
	err = iPositioner.Open(iPosServer, iUidSimulationPsy);
	AssertTrueL(err == KErrNone, KOpenErr, err);	//Leave if error
	
	//Set Requestor
	_LIT(KServiceName3, "Some Name Uhh");
	iPositioner.SetRequestor(CRequestor::ERequestorService,
							 CRequestor::EFormatApplication, KServiceName3);
	//Request new position
	TPositionInfo posInfo = TPositionInfo();
	TRequestStatus status = KRequestPending;
	iPositioner.NotifyPositionUpdate(posInfo, status);
	
	User::After(2000000);
	if(status.Int() != KRequestPending)
		{
		_LIT(KTooFastError, "PSY completed request too fast!");
		TBuf<100> buf;
        buf.Format(KTooFastError, KErrGeneral);
		iPositioner.Close();
		iPosServer.Close();
		LogErrorAndLeaveL(buf);
		}
	else
		{
		err = iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
		if (err != KErrNone)
			{
			_LIT(KCancelError, "Cancel of request failed!");
			TBuf<100> buf;
			buf.Format(KCancelError, err);
			LogErrorAndLeaveL(buf);
			}
		}
	User::WaitForRequest(status); //Wait for position request to complete

	//check if request was cancelled
	if(status.Int() != KErrCancel)
		{
		_LIT(KNoCancel, "Request was not cancelled!, error code = %d");
		TBuf<100> buf;
        buf.Format(KNoCancel, status.Int());
		LogErrorAndLeaveL(buf);
		}
	
	TTime startTime;
    TTime stopTime;
	
	//Make another request
	iPositioner.NotifyPositionUpdate(posInfo, status);

	startTime.UniversalTime();
	User::WaitForRequest(status);
	stopTime.UniversalTime();

	if(status.Int() != KErrNone)
		{
		iPositioner.Close();
		iPosServer.Close();
		_LIT(KSecondReqErr, "Second position request failed!, error code = %d");
		TBuf<100> buf;
        buf.Format(KSecondReqErr, status.Int());
		LogErrorAndLeaveL(buf);
		}

	TTimeIntervalMicroSeconds requestTime = stopTime.MicroSecondsFrom(startTime);
	
	//Check if request was performed within timeTofix_min and timeTofix_max
	_LIT(KReqText, "Checking if request was performed within timeTofix_min(4s) and timeTofix_max(8s)->");
	INFO_PRINTF1(KReqText);
	TBuf<100> timeR;
    timeR.Num(requestTime.Int64());
	INFO_PRINTF1(timeR);
	if(requestTime<timetofixTime_MIN)
		{
		_LIT(KRequestTimeErrMin, "Second request returned position too fast!");
		TBuf<100> buf;
        buf.Format(KRequestTimeErrMin, KErrGeneral);
		LogErrorAndLeaveL(buf);
		}
	else if(requestTime>timetofixTime_MAX)
		{
		_LIT(KRequestTimeErrMax, "Second request returned position too slow!");
		TBuf<100> buf;
        buf.Format(KRequestTimeErrMax, KErrGeneral);
		LogErrorAndLeaveL(buf);
		}
		
	//cleaning up the pieces...
	iPositioner.Close();
	iPosServer.Close();
	INFO_PRINTF1(KEnd);	
	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::CheckPositionL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp115::CheckPositionL(TPositionInfo& aPosInfo,TInt aFixNumber)
	{
	TPosition pos;
	aPosInfo.GetPosition(pos);
	TReal lat = pos.Latitude();
	TReal lon = pos.Longitude();
	TReal alt = pos.Altitude();
	TReal horAcc = pos.HorizontalAccuracy();
	TReal verAcc = pos.VerticalAccuracy();
	TReal latComp = 0.0;
	TReal lonComp = 0.0;
	TReal altComp = 0.0;
	TReal horAccComp = 0.0;
	TReal verAccComp = 0.0;
	
	_LIT(KFunctionName, "We are inside CheckPositionL()");
	INFO_PRINTF1(KFunctionName);
	
	switch (aFixNumber)
		{
		case 1:
			{
			horAccComp = KHdop1*10;
			verAccComp = 1.5f*KHdop1*10;
			latComp = ToCoordinate(KLat1(), KLatDir1());
			lonComp = ToCoordinate(KLon1(), KLonDir1());
			altComp = KAltitude1;
			break;
			}
		case 2:
			{
			horAccComp = KHdop2*10;
			verAccComp = 1.5f*KHdop2*10;
			latComp = ToCoordinate(KLat2(), KLatDir2());
			lonComp = ToCoordinate(KLon2(), KLonDir2());
			altComp = KAltitude2;
			break;
			}
		case 3:
			{
			horAccComp = KHdop3*10;
			verAccComp = 1.5f*KHdop3*10;
			latComp = ToCoordinate(KLat3(), KLatDir3());
			lonComp = ToCoordinate(KLon3(), KLonDir3());
			altComp = KAltitude3;
			break;
			}
		case 4:
			{
			horAccComp = KHdop4*10;
			verAccComp = 1.5f*KHdop4*10;
			latComp = ToCoordinate(KLat4(), KLatDir4());
			lonComp = ToCoordinate(KLon4(), KLonDir4());
			altComp = KAltitude4;
			break;
			}
		case 5:
			{
			horAccComp = KHdop5*10;
			verAccComp = 1.5f*KHdop5*10;
			latComp = ToCoordinate(KLat5(), KLatDir5());
			lonComp = ToCoordinate(KLon5(), KLonDir5());
			altComp = KAltitude5;
			break;
			}
		case 6:
			{
			horAccComp = KHdop6*10;
			verAccComp = 1.5f*KHdop6*10;
			latComp = ToCoordinate(KLat6(), KLatDir6());
			lonComp = ToCoordinate(KLon6(), KLonDir6());
			altComp = KAltitude6;
			break;
			}
		case 7:
			{
			horAccComp = KHdop7*10;
			verAccComp = 1.5f*KHdop7*10;
			latComp = ToCoordinate(KLat7(), KLatDir7());
			lonComp = ToCoordinate(KLon7(), KLonDir7());
			altComp = KAltitude7;
			break;
			}
		case 8:
			{
			horAccComp = KHdop8*10;
			verAccComp = 1.5f*KHdop8*10;
			latComp = ToCoordinate(KLat8(), KLatDir8());
			lonComp = ToCoordinate(KLon8(), KLonDir8());
			altComp = KAltitude8;
			break;
			}
		case 9:
			{
			horAccComp = KHdop9*10;
			verAccComp = 1.5f*KHdop9*10;
			latComp = ToCoordinate(KLat9(), KLatDir9());
			lonComp = ToCoordinate(KLon9(), KLonDir9());
			altComp = KAltitude9;
			break;
			}
		case 10:
			{
			horAccComp = KHdop10*10;
			verAccComp = 1.5f*KHdop10*10;
			latComp = ToCoordinate(KLat10(), KLatDir10());
			lonComp = ToCoordinate(KLon10(), KLonDir10());
			altComp = KAltitude2;
			break;
			}
		}
				
	if( lat != latComp ||
	    lon != lonComp ||
	    alt != altComp ||
	    horAcc != horAccComp ||
	    verAcc != verAccComp)
		{
		_LIT(KErrPosition, "Wrong Postion from SimPSY:n, pos number = %d");
		TBuf<100> buf;
        buf.Format(KErrPosition, aFixNumber);
		LogErrorAndLeaveL(buf);
		}
	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::ToCoordinate
// used for creating simulation PSY settings files
// (other items were commented in a header).
// ---------------------------------------------------------
//
TReal CT_LbsSimulationPSYPosTp115::ToCoordinate(
	const TDesC8& aDes, const TDesC8& aDir)
    {
    const TInt  KPosMinutesIntegerLength = 2;
    const TReal KPosMinutesPerDegree = 60.0;
	const signed int KCoordTokenS = 'S';
	const signed int KCoordTokenW = 'W';
	const signed int KCoordTokenDot = '.';

	TInt point = aDes.Locate(KCoordTokenDot);

    TInt deg = DesToInt(aDes.Left(point - KPosMinutesIntegerLength));
    TReal min = DesToReal(aDes.Mid(point - KPosMinutesIntegerLength));
    TReal coordinate = TReal(deg) + min / KPosMinutesPerDegree;

    if (aDir[0] == KCoordTokenS || aDir[0] == KCoordTokenW)
        {
        coordinate = - coordinate;
        }
    return coordinate;
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::DesToInt
// used for creating simulation PSY settings files
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsSimulationPSYPosTp115::DesToInt(const TPtrC8& aDes)
    {
    TLex8	lexer(aDes);
    TInt	integer;

    lexer.Val(integer);
    return integer;
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp115::DesToReal
// used for creating simulation PSY settings files
// (other items were commented in a header).
// ---------------------------------------------------------
//
TReal CT_LbsSimulationPSYPosTp115::DesToReal(const TPtrC8& aDes)
    {
    TLex8 lexer(aDes);
    TReal realNumber;

    lexer.Val(realNumber, TChar('.'));
    return realNumber;
    }

void CT_LbsSimulationPSYPosTp115::ReplaceSwitchInTrapL(TInt aIndex)
    {
    const TDesC *NMEASource[KNrOfNMEATypes] = {&KMagellanGPS315,
            &KMagellanTracker, 
            &KGarmin12cx,
            &KGarmin3plus,
            &KGarminetrexlegend,
            &KMagellanMeridian};
    
    switch (aIndex)
        {
        case 0 : 
          {
          for (TInt i=0; i<KNrOfNMEATypes;i++)
            {
            TestSimulationNmeaReaderL(*NMEASource[i]);
            }
          }
        break;
        case 1 : TestPowerUpTimeL(); break;
        case 2 : TestTimeToFixL(); break;
        case 3 : TestDeterministicFailureL(); break;
        case 4 : TestRandomFailureL(); break;
        case 5 : TestAltitudeL(); break;
        case 6 : TestLongLatitudeL(); break;
        case 7 : TestMovementL(); break;
        case 8 : TestBadInputFileL(); break;
    
        case 9:
            {
            for (TInt i=0; i<KNrOfNMEATypes;i++)
              {
              Extended1TestSimulationNmeaReaderL(*NMEASource[i]);
              }
            }
            break;
        case 10:
            {
            for (TInt i=0; i<KNrOfNMEATypes;i++)
              {
              Extended2TestSimulationNmeaReaderL(*NMEASource[i]);
              }
            }
            break;
        case 11: TestCorrectFixL(); break;
        case 12: TestCorrectTimingAfterCancelL(); break;
    
        }
    }
//  End of File
