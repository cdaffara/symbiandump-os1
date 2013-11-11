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
 @file Te_LbsBtGpsPsySuiteStepBase.cpp
 @internalTechnology
*/
#include "Te_LbsBtGpsPsyStepBase.h"
#include "Te_LbsBtGpsPsyDefs.h"

#include <bt_sock.h>
#include <btextnotifiers.h>

// Device driver constants

TVerdict CTe_LbsBtGpsPsyStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	iScheduler = new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	
    // Enable he test framework to use bluetooth
    _LIT(KLDDName, "ECOMM");
    #if defined (__WINS__)
    	_LIT(KPDDName, "ECDRV");        
    #else   
    	_LIT(KPDDName, "EUART1");
    #endif
    TInt rerr = User::LoadPhysicalDevice(KPDDName);
    if(rerr != KErrNone && rerr != KErrAlreadyExists)
    	{
    	User::Leave(rerr);
    	}
    rerr = User::LoadLogicalDevice(KLDDName);
    if(rerr != KErrNone && rerr != KErrAlreadyExists)
    	{
        User::Leave(rerr);
        }

    // Connect to the Bluetooth PSY simulation test server
    iBtGpsSim.Connect();

	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsBtGpsPsyStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	iBtGpsSim.Close();
	return TestStepResult();
	}

CTe_LbsBtGpsPsyStepBase::~CTe_LbsBtGpsPsyStepBase()
	{
	delete iScheduler;
	}

CTe_LbsBtGpsPsyStepBase::CTe_LbsBtGpsPsyStepBase()
	{
	}

// ---------------------------------------------------------
// CTDBT1Step::AssertTrueSecL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CTe_LbsBtGpsPsyStepBase::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
    if (!aCondition)
        {
        TBuf<KMaxLogBuffer> buf;
        buf.Format(aErrorMsg, aErrorCode);
        ERR_PRINTF1(buf);
        SetTestStepResult(EFail);
        }
    }

void CTe_LbsBtGpsPsyStepBase::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode1, TInt aErrorCode2)
    {
    if (!aCondition)
        {
        TBuf<KMaxLogBuffer> buf;
        buf.Format(aErrorMsg, aErrorCode1, aErrorCode2);
        ERR_PRINTF1(buf);
        SetTestStepResult(EFail);
        }
    }

void CTe_LbsBtGpsPsyStepBase::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TReal aErrorCode1, TReal aErrorCode2)
    {
    if (!aCondition)
        {
        TBuf<KMaxLogBuffer> buf;
        buf.Format(aErrorMsg, aErrorCode1, aErrorCode2);
        ERR_PRINTF1(buf);
        SetTestStepResult(EFail);
        }
    }

void CTe_LbsBtGpsPsyStepBase::CheckModuleInfoL( TPositionModuleInfo aInfo )
    {
    TBuf<KMaxLogBuffer> name;
    
    aInfo.GetModuleName(name);
    
    INFO_PRINTF1( name );
    
    AssertTrueSecL( KErrNone == name.Compare(KBTPSYName), KWrongPSYName, KErrNone );
    
    AssertTrueSecL( aInfo.ModuleId() == KbtGPSPsyId, KWrongModuleID, aInfo.ModuleId().iUid );
    AssertTrueSecL( aInfo.IsAvailable(), KNotAvailable, KErrNone );
    AssertTrueSecL( aInfo.Version().iMajor == 60, KWrongVersion60, aInfo.Version().iMajor );
    AssertTrueSecL( aInfo.TechnologyType() == TPositionModuleInfo::ETechnologyTerminal, 
    	KNotaTerminal, aInfo.TechnologyType() );
    AssertTrueSecL( aInfo.DeviceLocation() == TPositionModuleInfo::EDeviceExternal, 
    	KNotExternal, aInfo.DeviceLocation() );
    AssertTrueSecL( aInfo.ClassesSupported(EPositionInfoFamily) == ( EPositionInfoClass | EPositionGenericInfoClass | EPositionCourseInfoClass |
        EPositionSatelliteInfoClass ), KNotSupportedClass, aInfo.ClassesSupported(EPositionInfoFamily) );
    TPositionModuleInfo::TCapabilities capability = ( TPositionModuleInfo::ECapabilityHorizontal | 
        TPositionModuleInfo::ECapabilityVertical | TPositionModuleInfo::ECapabilitySpeed | 
        TPositionModuleInfo::ECapabilityDirection | TPositionModuleInfo::ECapabilitySatellite | 
        TPositionModuleInfo::ECapabilityNmea );
    AssertTrueSecL( aInfo.Capabilities() == capability, KNoCorrectCapabilities, aInfo.Capabilities() );
    
    TPositionQuality modQ;
    aInfo.GetPositionQuality( modQ);
    
    TInt32 hAcc = 0;
    TInt32 vAcc = 0;
    Math::Int( hAcc, modQ.HorizontalAccuracy() );
    Math::Int( vAcc, modQ.VerticalAccuracy() );
    
    AssertTrueSecL( modQ.TimeToFirstFix() == TTimeIntervalMicroSeconds(80000000), KWrongTTFF, (TInt) modQ.TimeToFirstFix().Int64());
    AssertTrueSecL( modQ.TimeToNextFix() == TTimeIntervalMicroSeconds(1000000), KWrongTTNF, modQ.TimeToNextFix().Int64());
    AssertTrueSecL( modQ.HorizontalAccuracy() == 10.0, KNullDesC, hAcc );
    AssertTrueSecL( modQ.VerticalAccuracy() == 30.0, KNullDesC, vAcc );
    AssertTrueSecL( modQ.CostIndicator() == TPositionQuality::ECostZero, KNullDesC, modQ.CostIndicator() );
    AssertTrueSecL( modQ.PowerConsumption() == TPositionQuality::EPowerMedium, KNullDesC, modQ.PowerConsumption() );
    }

TInt CTe_LbsBtGpsPsyStepBase::QueryOnePositionL(TPositionInfoBase & aPosInfo,TTimeIntervalMicroSeconds aTimeOut,TTimeIntervalMicroSeconds aUpdateInterval)
    {
    TInt err = KErrNone;
    TRequestStatus status = KRequestPending;
    
    RPositionServer posServer;
    
    err = posServer.Connect();
    
    CleanupClosePushL(posServer);
    
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    RPositioner BTGpsPsy;
    
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    
    AssertTrueSecL( err == KErrNone, KCanNotOpenBTPSY, err );
    
    CleanupClosePushL(BTGpsPsy);
    
    if ( aTimeOut.Int64() > 0 || aUpdateInterval.Int64() > 0)
        {
        TPositionUpdateOptions  updateops;
        if( aTimeOut.Int64() > 0 )
            {
            updateops.SetUpdateTimeOut(aTimeOut);
            }
        if( aUpdateInterval.Int64() > 0 )
            {
            updateops.SetUpdateInterval(aUpdateInterval);
            }
        TInt erri = BTGpsPsy.SetUpdateOptions(updateops);
        AssertTrueSecL( erri == KErrNone  , KFailedUpdateOptions, erri);
        
        }
    
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService , CRequestor::EFormatApplication , KBTPSYRequestorName);
    
    AssertTrueSecL( err == KErrNone, KCanNotSetRequestor, err );
    
    //First request
    status = KRequestPending;
    BTGpsPsy.NotifyPositionUpdate( aPosInfo, status );
    User::WaitForRequest( status ); 
    
    CleanupStack::PopAndDestroy(2, &posServer); // posServer, BTGpsPsy
    
    return status.Int();
    }

void CTe_LbsBtGpsPsyStepBase::PrintPositionInformationL( TPosition aPosition )
	{
	TBuf<KMaxLogBuffer> message;

	TTime fixTime;
	TReal32 horizontalAccuracy;
	TReal32 verticalAccuracy;
	TReal64 longitude;
	TReal64 latitude;
	TReal32 altitude;
	TPositionDatumId datum;

	fixTime = aPosition.Time();
	horizontalAccuracy = aPosition.HorizontalAccuracy();
	verticalAccuracy = aPosition.VerticalAccuracy();
	longitude = aPosition.Longitude();
	latitude = aPosition.Latitude();
	altitude = aPosition.Altitude();
	datum = aPosition.Datum();

	AssertTrueSecL( datum == KPositionDatumWgs84, KWrongDatumTxt, KPositionDatumWgs84.iUid );

	fixTime.FormatL( message, KDateString );
	INFO_PRINTF1( message ); // time

	message.Format( KPositionInformation, longitude, latitude, altitude );
	INFO_PRINTF1( message ); // position

	message.Format( KAccuracies, horizontalAccuracy, verticalAccuracy );
	INFO_PRINTF1( message ); // accuracies
	
	iTime = fixTime;
	}

void CTe_LbsBtGpsPsyStepBase::PrintPositionInformationL( TCourse aCourse )
	{
	TBuf<KMaxLogBuffer> message;

    TReal32 speed;
    TReal32 heading;
    TReal32 courseCourse;
    TReal32 speedAcc;
    TReal32 headingAcc;
    TReal32 courseAcc;
    
    speed = aCourse.Speed();
    heading = aCourse.Heading();
    courseCourse = aCourse.Course();
    speedAcc = aCourse.SpeedAccuracy();
    headingAcc = aCourse.HeadingAccuracy();
    courseAcc = aCourse.CourseAccuracy();
    
    message.Format( KCourse, speed, heading, courseCourse );
    INFO_PRINTF1( message ); // course
    
    message.Format( KCourseAccuracies, speedAcc, headingAcc, courseAcc);
    INFO_PRINTF1( message ); // course accuracies
	}

void CTe_LbsBtGpsPsyStepBase::PrintPositionInformationL( TSatelliteData aSatelliteData )
	{
	TBuf<KMaxLogBuffer> message;
	
	TInt satelliteId = 0;
    TReal32 elevation = 0;
    TReal32 azimuth = 0;
    TInt sigPower = 0;
    
	satelliteId = aSatelliteData.SatelliteId();
    elevation = aSatelliteData.Elevation();
    azimuth = aSatelliteData.Azimuth();
    sigPower = aSatelliteData.SignalStrength();
    
    message.Format( KSatelliteDataSummary, satelliteId, azimuth, elevation, sigPower );
    INFO_PRINTF1( message ); // satellite info
    
    if( aSatelliteData.IsUsed() )
        {
        INFO_PRINTF1( _L( "Satellite is used in the fix" ) );
        }
    else
    	{
    	INFO_PRINTF1( _L( "Satellite is NOT USED in the fix" ) );
    	}
	}
	
void CTe_LbsBtGpsPsyStepBase::PrintPositionInformationL( TPositionInfo aInfo )
    {
    TPosition position;
    
    INFO_PRINTF1( KFixInfoHeaderTxt );
    
    aInfo.GetPosition( position );
    
    PrintPositionInformationL( position );
    
    INFO_PRINTF1( KFixInfoFooterTxt );
    }

void CTe_LbsBtGpsPsyStepBase::PrintPositionInformationL( TPositionCourseInfo aInfo )
    {
    TPosition position;
    TCourse course;
    
    INFO_PRINTF1( KFixInfoHeaderTxt );
    
    aInfo.GetPosition( position );
    aInfo.GetCourse( course );
    
    PrintPositionInformationL( position );
    PrintPositionInformationL( course );
    
    INFO_PRINTF1( KFixInfoFooterTxt );
    }

void CTe_LbsBtGpsPsyStepBase::PrintPositionInformationL( TPositionSatelliteInfo aInfo )
    {
    TBuf<KMaxLogBuffer> message;
    
    TPosition position;
    TCourse course;
    TSatelliteData satellite;
    
    // satellite
    TTime satelliteTime(0);
    TReal32 hdop = 0;
    TReal32 vdop = 0;
    TReal32 tdop = 0;
    
    TInt inView = 0;
    TInt usedSatellites = 0;
    
    INFO_PRINTF1(KFixInfoHeaderTxt);
    
    aInfo.GetPosition( position );
    aInfo.GetCourse( course );
    
    // satellite info
    satelliteTime = aInfo.SatelliteTime();
    hdop = aInfo.HorizontalDoP();
    vdop = aInfo.VerticalDoP();
    tdop = aInfo.TimeDoP();
    inView = aInfo.NumSatellitesInView();
    usedSatellites = aInfo.NumSatellitesUsed();
    
    INFO_PRINTF1(KNewlineTxt);
    INFO_PRINTF1(KPositionData);
    PrintPositionInformationL( position );
    
    INFO_PRINTF1(KNewlineTxt);
    INFO_PRINTF1(KCourseData);
    PrintPositionInformationL( course );
    
    INFO_PRINTF1(KNewlineTxt);
    INFO_PRINTF1(KSatelliteData);
    
    satelliteTime.FormatL( message, KSatelliteTime );
    INFO_PRINTF1( message ); // satellite time
    
    message.Format( KSatelliteAcc, hdop, vdop, tdop );
    INFO_PRINTF1( message ); // satellite accuracies
    
    message.Format( KSatelliteNumbers, inView, usedSatellites );
    INFO_PRINTF1( message ); // satellite amounts
    
    for( TInt i = 0; i < inView; i++ )
        {
        aInfo.GetSatelliteData( i, satellite );
        INFO_PRINTF2(KSatelliteIndex, i);
        PrintPositionInformationL( satellite );
        }
    
    INFO_PRINTF1(KNewlineTxt);
    INFO_PRINTF1(KFixInfoFooterTxt);
}

void CTe_LbsBtGpsPsyStepBase::PrintPositionInformationL( HPositionGenericInfo *aInfo, TPositionFieldId aId )
    {
    
    TInt err = KErrNone;
    
    TReal32 value = 0;
    TUint8 number = 0;
    TInt8 amount = 0;
    TTime time(0);
    TBuf<KMaxLogBuffer> message;

    TPosition position;
    
    INFO_PRINTF1(KFixInfoHeaderTxt);
    
    aInfo->GetPosition( position );
    
    PrintPositionInformationL( position );
    
    INFO_PRINTF1(KFieldExtracting);
    
    switch( aId )
        {

        case EPositionFieldHorizontalSpeed:
            {
            err = aInfo->GetValue( aId, value );
            AssertTrueSecL( err == KErrNone, KGetValueFailedTxt, err );
            message.Format( KValuePositionFieldHorizontalSpeed, value );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldHeading:
            {
            err = aInfo->GetValue( aId, value );
            AssertTrueSecL( err == KErrNone, KGetValueFailedTxt, err );
            message.Format( KValuePositionFieldHeading, value );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldNMEASentences:
            {
            err = aInfo->GetValue( aId, number );
            AssertTrueSecL( err == KErrNone, KGetValueFailedTxt, err );
            INFO_PRINTF1(KFielPrinting);
            message.Format(KValuePositionFieldNMEASentences, number );
            INFO_PRINTF1( message );

            TInt i = 0;
            TBool isnmea = EFalse;
            while( aInfo->IsFieldAvailable(EPositionFieldNMEASentencesStart+i) )
                {
                TBuf8<500> sentence;
                TBuf16<500> sentence16;
                aInfo->GetValue(EPositionFieldNMEASentencesStart+i,sentence);
                sentence16.Copy(sentence);
                message.Format(KNMEAsentenceFromHPositionGenericInfo, i);
                _LIT8(KMessageIdRMC,"$GPRMC");
                if(sentence.Find(KMessageIdRMC))
                	{
                	isnmea = ETrue;
                	}
                INFO_PRINTF1(message);
                INFO_PRINTF1(sentence16);
                i++;
                }
            AssertTrueSecL( isnmea, KDeviceNotNMEA, KErrArgument );
            break;
            }
        case EPositionFieldSatelliteNumInView:
            {
            err = aInfo->GetValue( aId, amount );
            AssertTrueSecL( err == KErrNone, KGetValueFailedTxt, err );
            message.Format( KValuePositionFieldSatelliteNumInView, amount );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatelliteNumUsed:
            {
            err = aInfo->GetValue( aId, amount );
            AssertTrueSecL( err == KErrNone, KGetValueFailedTxt, err );
            message.Format( KValuePositionFieldSatelliteNumUsed, amount );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatelliteTime:
            {
            err = aInfo->GetValue( aId, time );
            time.FormatL( message, KValuePositionFieldSatelliteTime);
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatelliteHorizontalDoP:
            {
            err = aInfo->GetValue( aId, value );
            message.Format( KValuePositionFieldSatelliteHorizontalDoP, value );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatelliteVerticalDoP:
            {
            err = aInfo->GetValue( aId, value );
            message.Format( KValuePositionFieldSatelliteVerticalDoP, value );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatellitePositionDoP:
            {
            err = aInfo->GetValue( aId, value );
            message.Format( KValuePositionFieldSatellitePositionDoP, value );
            INFO_PRINTF1( message );
            break;
            }
        case EPositionFieldSatelliteSeaLevelAltitude:
            {
            err = aInfo->GetValue( aId, value );
            message.Format( KValuePositionFieldSatelliteSeaLevelAltitude, value );
            INFO_PRINTF1( message );
            break;
            }
        }
    INFO_PRINTF1(KFixInfoFooterTxt);
    }

void CTe_LbsBtGpsPsyStepBase::PrintPositionInformationForThreadL( TPositionInfo aInfo)
	{
	PrintPositionInformationForThreadL(aInfo, KThreadLogFile);
	}

void CTe_LbsBtGpsPsyStepBase::PrintPositionInformationForThreadL( TPositionInfo aInfo, const TDesC& aFile )
    {
    TPosition position;
    TBuf<KMaxLogBuffer> message;
    TInt err = KErrNone;
    
    aInfo.GetPosition( position );
    
    TTime fixTime;
    TReal32 horizontalAccuracy;
    TReal32 verticalAccuracy;
    TReal64 longitude;
    TReal64 latitude;
    TReal32 altitude;
    TPositionDatumId datum;
    
    fixTime = position.Time();
    horizontalAccuracy = position.HorizontalAccuracy();
    verticalAccuracy = position.VerticalAccuracy();
    longitude = position.Longitude();
    latitude = position.Latitude();
    altitude = position.Altitude();
    datum = position.Datum();
    
    fixTime.FormatL( message, KDateString );
    
    // Open connection to file server
    RFs fs;
    User::LeaveIfError(fs.Connect());
    
    CleanupClosePushL(fs);
    
    // Open log file
    RFile file;
    CleanupClosePushL(file);
    
    err = file.Open(fs,aFile,EFileStreamText|EFileWrite);
    
    if (err != KErrNone)
        {
        if (err == KErrNotFound)
            {
            err = file.Create(fs,aFile,EFileStreamText|EFileWrite);
            
            if (err != KErrNone)
                User::Leave(err);
            }
        else
            {
            User::Leave(err);
            }
        }
    
    
    // Write all collected time intervals to log file
    
    TFileText fileText;
    fileText.Set(file);
    
    fileText.Write(KFixInfoHeaderTxt);
    fileText.Write( message ); // Time
    
    message.Format( KPositionInformation, longitude, latitude, altitude );
    fileText.Write( message ); // position
    
    message.Format( KAccuracies, horizontalAccuracy, verticalAccuracy );
    fileText.Write( message ); // accuracies
    
    message.Format( KDatum, datum );
    fileText.Write( message ); // datum
    
    fileText.Write(KFixInfoFooterTxt);
    
    CleanupStack::PopAndDestroy(2, &fs); //fs, file
    }

void CTe_LbsBtGpsPsyStepBase::PrintPositionInformationForThreadL( TPositionSatelliteInfo aInfo )
    {
    PrintPositionInformationForThreadL(aInfo, KThreadLogFile);
	}

void CTe_LbsBtGpsPsyStepBase::PrintPositionInformationForThreadL( TPositionSatelliteInfo aInfo, const TDesC& aFile )
    {
    
    TPosition position;
    TCourse course;
    TSatelliteData satellite;
    TBuf<KMaxLogBuffer> message;
    TInt err = 0;
    
    aInfo.GetPosition( position );
    aInfo.GetCourse( course );
    
    TTime fixTime;
    TReal32 horizontalAccuracy;
    TReal32 verticalAccuracy;
    TReal64 longitude;
    TReal64 latitude;
    TReal32 altitude;
    TPositionDatumId datum;
    
    // course
    TReal32 speed;
    TReal32 heading;
    TReal32 speedAcc;
    TReal32 headingAcc;
    
    // satellite
    TTime satelliteTime(0);
    TReal32 hdop = 0;
    TReal32 vdop = 0;
    TReal32 tdop = 0;
    TInt satelliteId = 0;
    TInt inView = 0;
    TInt usedSatellites = 0;
    TReal32 elevation = 0;
    TReal32 azimuth = 0;
    TInt sigPower = 0;
    
    fixTime = position.Time();
    horizontalAccuracy = position.HorizontalAccuracy();
    verticalAccuracy = position.VerticalAccuracy();
    longitude = position.Longitude();
    latitude = position.Latitude();
    altitude = position.Altitude();
    datum = position.Datum();
    
    // course
    speed = course.Speed();
    heading = course.Heading();
    speedAcc = course.SpeedAccuracy();
    headingAcc = course.HeadingAccuracy();
    
    satelliteTime = aInfo.SatelliteTime();
    hdop = aInfo.HorizontalDoP();
    vdop = aInfo.VerticalDoP();
    tdop = aInfo.TimeDoP();
    inView = aInfo.NumSatellitesInView();
    usedSatellites = aInfo.NumSatellitesUsed();
    
    fixTime.FormatL( message, KDateString );
    
    // Open connection to file server
    RFs fs;
    User::LeaveIfError(fs.Connect());
    
    CleanupClosePushL(fs);
    
    // Open log file
    RFile file;
    CleanupClosePushL(file);
    
    err = file.Open(fs,aFile,EFileStreamText|EFileWrite);
    
    if(err != KErrNone)
        {
        if (err == KErrNotFound)
            {
            err = file.Create(fs,aFile,EFileStreamText|EFileWrite);
            
            if (err != KErrNone)
                User::Leave(err);
            }
        else
            {
            User::Leave(err);
            }
        }

    // Write all collected time intervals to log file
    TFileText fileText;
    fileText.Set(file);
    
    fileText.Write(KFixInfoHeaderTxt);
    fileText.Write(KFileNewlineTxt);
    fileText.Write( KPositionData );
    fileText.Write( message ); // Time
    
    message.Format( KPositionInformation, longitude, latitude, altitude );
    fileText.Write( message ); // position
    
    message.Format( KAccuracies, horizontalAccuracy, verticalAccuracy );
    fileText.Write( message ); // accuracies
    
    fileText.Write(KFileNewlineTxt);
    fileText.Write( KCourseData );
    message.Format( KCourse, speed, heading );
    fileText.Write( message ); // course
    
    message.Format( KCourseAccuracies, speedAcc, headingAcc );
    fileText.Write( message ); // course accuracies
    
    fileText.Write(KFileNewlineTxt);
    fileText.Write( KSatelliteData );
    fixTime.FormatL( message, KSatelliteTime );
    fileText.Write( message ); // Satellite time
    
    message.Format( KSatelliteAcc, hdop, vdop, tdop );
    fileText.Write( message ); // satellite accuracies
    
    message.Format( KSatelliteNumbers, inView, usedSatellites );
    fileText.Write( message ); // satellite amounts
    
    for( TInt i = 0; i < inView; i++ )
        {
        aInfo.GetSatelliteData( i, satellite );
        
        satelliteId = satellite.SatelliteId();
        elevation = satellite.Elevation();
        azimuth = satellite.Azimuth();
        sigPower = satellite.SignalStrength();
        
        message.Format( KSatelliteDataSummary, i, satelliteId, azimuth, elevation, sigPower );
        fileText.Write( message ); // satellite info
        
        if( satellite.IsUsed() )
            {
            fileText.Write( _L( "Satellite is USED in the fix\r\n" ) );
            }
        else
            {
            fileText.Write( _L( "Satellite is NOT USED in the fix\r\n" ) );
            }
        
        }
    
    message.Format( KDatum, datum );
    fileText.Write( message ); // datum
    
    fileText.Write(KFileNewlineTxt);
    
    fileText.Write(KFixInfoFooterTxt);
    
    CleanupStack::PopAndDestroy(2, &fs); //fs, file
    }

void CTe_LbsBtGpsPsyStepBase::PrintForThreadL( const TDesC& aMessage)
	{
	PrintForThreadL(aMessage, KThreadLogFile);
	}

void CTe_LbsBtGpsPsyStepBase::PrintForThreadL( const TDesC& aMessage, const TDesC& aFile )
    {
    TInt err = KErrNone;
    
    // Open connection to file server
    RFs fs;
    User::LeaveIfError(fs.Connect());
    
    CleanupClosePushL(fs);
    
    // Open log file
    RFile file;
    CleanupClosePushL(file);
    
    err = file.Open(fs,aFile,EFileStreamText|EFileWrite);
    
    if (err != KErrNone)
        {
        if (err == KErrNotFound)
            {
            err = file.Create(fs,aFile,EFileStreamText|EFileWrite);
            
            if (err != KErrNone)
                User::Leave(err);
            }
        else
            {
            User::Leave(err);
            }
        }
    
    
    // Write all collected time intervals to log file
    
    TFileText fileText;
    fileText.Set(file);
    
    fileText.Write( aMessage );
    
    CleanupStack::PopAndDestroy(2, &fs); // fs, file
    }

TInt CTe_LbsBtGpsPsyStepBase::StartThreadL( TAny* aData )
    {
    // Create cleanupstack for this thread
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    // Create active sheculer (not necessary in all cases)
    CActiveScheduler* newScheduler = new(ELeave) CActiveScheduler();
    CActiveScheduler::Install(newScheduler);
    
    // Read parameters
    TInt* testDriver = (TInt*) aData;
    
    // Run the actual test, and trap possible leave
    TRAPD(err, MakeTenPeriodicUpdatesFromBTGPSPsyL( *testDriver ));
    
    // Delete active scheduler
    delete newScheduler;
    
    // Delete cleanup stack
    delete cleanup;
    
    // Exit this thread with leave code (KErrNone in successful case)
    User::Exit(err);
    
    return 0;
    
    }
