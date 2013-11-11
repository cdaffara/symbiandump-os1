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
#include "ctlbssimulationpsypostp254.h"
#include "ctlbssimulationpsypostp242constants.h"

// CONSTANTS
_LIT(KServiceName, "TP254");

#ifdef __WINS__
    _LIT(KNmeaFile,         "z:\\system\\test\\testdata\\tp242.nme");           // Full fix
    _LIT(KPartialNmeaFile,  "z:\\system\\test\\testdata\\TP254Partial.nme");    // Missing longitude
    _LIT(KPartialNmeaFile2, "z:\\system\\test\\testdata\\TP254Partial2.nme");   // Missing latitude
    _LIT(KPartialNmeaFile3, "z:\\system\\test\\testdata\\TP254Partial3.nme");   // Full fix
    _LIT(KPartialNmeaFile4, "z:\\system\\test\\testdata\\TP254Partial4.nme");   // Missing longitude and latitude 
    _LIT(KPartialNmeaFile5, "z:\\system\\test\\testdata\\TP254Partial5.nme");   // Missing longitude, latitude 
    // Contains 8 locations fixes: 
    // 1-full, 2-partial, 3-full, 4-partial, 5-partial(missing GGA), 6-full, 7-partial(missing GGA), 8-full 
    _LIT(KPartialNmeaFile6, "z:\\system\\test\\testdata\\TP254Partial6.nme");   
    
#else
    _LIT(KNmeaFile,         "c:\\system\\test\\testdata\\tp242.nme");           
    _LIT(KPartialNmeaFile,  "c:\\system\\test\\testdata\\TP254Partial.nme");    
    _LIT(KPartialNmeaFile2, "c:\\system\\test\\testdata\\TP254Partial2.nme");   
    _LIT(KPartialNmeaFile3, "c:\\system\\test\\testdata\\TP254Partial3.nme");   
    _LIT(KPartialNmeaFile4, "c:\\system\\test\\testdata\\TP254Partial4.nme");   
    _LIT(KPartialNmeaFile5, "c:\\system\\test\\testdata\\TP254Partial5.nme");
    _LIT(KPartialNmeaFile6, "c:\\system\\test\\testdata\\TP254Partial6.nme");
    
#endif

const TInt KTimeOut = 3000000;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsSimulationPSYPosTp254::CT_LbsSimulationPSYPosTp254(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{
	_LIT(KTestName, "TP254 - Partial update");
	SetTestStepName(KTestName);
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsSimulationPSYPosTp254::~CT_LbsSimulationPSYPosTp254()
	{
	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp254::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp254::StartL()
    {    	
	_LIT(KConnectErr, "Error when connecting to Position Server,  %d");
    _LIT(KNotifyErr, "NotifyPositionUpdate returns err %d when expected KErrNone");
    _LIT(KPartialErr, "NotifyPositionUpdate returns err %d when expected KPositionPartialUpdate");
    _LIT(KTimedOutErr, "NotifyPositionUpdate returns err %d when expected KErrTimedOut");   
    
    TInt err = iPosServer.Connect();
	AssertTrueL(err == KErrNone, KConnectErr, err); 

    SetupPsyL(iUidSimulationPsy);
    TPositionInfo posInfo;
    
    
    SetFileInUseL(KPartialNmeaFile, EFalse);
    SetUpdateTimeOutL(KTimeOut);
    
    err = Request(posInfo);
    AssertTrueL(err == KErrTimedOut, KTimedOutErr, err); // Since partial not accepted
    
    SetFileInUseL(KNmeaFile);
    posInfo = TPositionInfo();
    err = Request(posInfo);
    AssertTrueL(err == KErrNone, KNotifyErr, err);    
    CheckTPositionInfoL(posInfo, ENone);
    
    SetFileInUseL(KPartialNmeaFile);
    posInfo = TPositionInfo();
    err = Request(posInfo);
    AssertTrueL(err == KPositionPartialUpdate, KPartialErr, err);
    CheckTPositionInfoL(posInfo, ELongitude);
    
    SetFileInUseL(KPartialNmeaFile2);
    posInfo = TPositionInfo();
    err = Request(posInfo);
    AssertTrueL(err == KPositionPartialUpdate, KPartialErr, err);
    CheckTPositionInfoL(posInfo, ELatitude);
    
    SetFileInUseL(KPartialNmeaFile3);
    posInfo = TPositionInfo();
    err = Request(posInfo);
    AssertTrueL(err == KErrNone, KNotifyErr, err);
    CheckTPositionInfoL(posInfo, ENone);
    
    SetFileInUseL(KPartialNmeaFile4);
    posInfo = TPositionInfo();
    err = Request(posInfo);
    AssertTrueL(err == KPositionPartialUpdate, KPartialErr, err);
    CheckTPositionInfoL(posInfo, ELongitude | ELatitude | ETimeStamp);
    
    SetFileInUseL(KPartialNmeaFile5);
    posInfo = TPositionInfo();
    err = Request(posInfo);
    AssertTrueL(err == KPositionPartialUpdate, KPartialErr, err);
    CheckTPositionInfoL(posInfo, ELongitude | ELatitude | ETimeStamp);
    
    SetFileInUseL(KPartialNmeaFile6);
    SetUpdateTimeOutL(KTimeOut);
    posInfo = TPositionInfo();
    
    // 1
    err = Request(posInfo);
    AssertTrueL(err == KErrNone, KNotifyErr, err);
    CheckTPositionInfoL(posInfo, ENone);
    
    // 2
    err = Request(posInfo);
    AssertTrueL(err == KPositionPartialUpdate, KPartialErr, err);
    CheckTPositionInfoL(posInfo, ELongitude);
    
    // 3
    err = Request(posInfo);
    AssertTrueL(err == KErrNone, KNotifyErr, err);
    CheckTPositionInfoL(posInfo, ENone);
    
    // 4
    err = Request(posInfo);
    AssertTrueL(err == KPositionPartialUpdate, KPartialErr, err);
    CheckTPositionInfoL(posInfo, ELatitude);
     
    // 5
    err = Request(posInfo);
    AssertTrueL(err == KPositionPartialUpdate, KPartialErr, err);
    CheckTPositionInfoL(posInfo, ELatitude | ELongitude | ETimeStamp | EAltitude);
    
    // 6
    err = Request(posInfo);
    AssertTrueL(err == KErrNone, KNotifyErr, err);
    CheckTPositionInfoL(posInfo, ENone);
    
    // 7
    err = Request(posInfo);
    AssertTrueL(err == KPositionPartialUpdate, KPartialErr, err);
    CheckTPositionInfoL(posInfo, ELatitude | ELongitude | ETimeStamp | EAltitude);
    
    // 8
    err = Request(posInfo);
    AssertTrueL(err == KErrNone, KNotifyErr, err);
    CheckTPositionInfoL(posInfo, ENone);
     
    // 9 The NMEA file is parsed from the beginning 
    SetPartialUpdateL(EFalse);
    
    err = Request(posInfo);
    AssertTrueL(err == KErrNone, KNotifyErr, err);
    CheckTPositionInfoL(posInfo, ENone);

    // 10 - skipped by PSY since partial and partial not allowed
    // 11
    err = Request(posInfo);
    AssertTrueL(err == KErrNone,KNotifyErr, err); // Since partial not accepted
    CheckTPositionInfoL(posInfo, ENone);
        
    // 12
    SetPartialUpdateL(ETrue);
    err = Request(posInfo);
    AssertTrueL(err == KPositionPartialUpdate, KPartialErr, err);
    CheckTPositionInfoL(posInfo, ELatitude);

    // 13
    err = Request(posInfo);
    AssertTrueL(err == KPositionPartialUpdate, KPartialErr, err);
    CheckTPositionInfoL(posInfo, ELatitude | ELongitude | ETimeStamp | EAltitude);

    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp254::CloseTest()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp254::CloseTest()
	{
    iPositioner.Close();
	iPosServer.Close();
	}

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp254::RequestL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CT_LbsSimulationPSYPosTp254::Request(TPositionInfoBase& aPositionInfo)
	{
	_LIT(KFunctionName, "We are inside Request()");
	INFO_PRINTF1(KFunctionName);
    iPositioner.NotifyPositionUpdate(aPositionInfo, iStatus);
    User::WaitForRequest(iStatus);
    
    return iStatus.Int();
    }
    
// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp254::CheckTPositionInfo()
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp254::CheckTPositionInfoL(const TPositionInfo& aPosInfo, TMissingPositionData aMissingData)
    {
    _LIT(KLongitudeErr, "Longitude is incorrect in TPositionInfo");
    _LIT(KLatitudeErr, "Latitude is incorrect in TPositionInfo");
    _LIT(KHorizontalErr, "Horizontal Accuracy is incorrect in TPositionInfo");
    _LIT(KVerticalErr, "Vertical Accuracy is incorrect in TPositionInfo");
    _LIT(KAltitudeErr, "Altitude is incorrect in TPositionInfo");
    _LIT(KTimeErr, "Time is incorrect in TPositionInfo");

    TPosition pos;
	aPosInfo.GetPosition(pos);
	TBool fullFix = ETrue;

    if (aMissingData & ELongitude)
        {
        AssertTrueL(Math::IsNaN(pos.Longitude()), KLongitudeErr, KErrGeneral);
        fullFix = EFalse;
        }
    else
        {
        AssertTrueL(pos.Longitude() == KLongitude, KLongitudeErr, KErrGeneral);
        }
    if (aMissingData & ELatitude)
        {
        AssertTrueL(Math::IsNaN(pos.Latitude()), KLatitudeErr, KErrGeneral);
        fullFix = EFalse;
        }
    else
        {
        AssertTrueL(pos.Latitude() == KLatitude, KLatitudeErr, KErrGeneral);
        }
    if (aMissingData & ETimeStamp)
        {
        AssertTrueL(pos.Time().Int64() == 0, KTimeErr, KErrGeneral); 
        fullFix = EFalse;
        }

	if (aMissingData & EAltitude)
        {
        AssertTrueL(Math::IsNaN(pos.Altitude()), KAltitudeErr, KErrGeneral);
        _LIT(KIsNaN, "Altitude IsNaN");
        INFO_PRINTF1(KIsNaN);
        fullFix = EFalse;
        }
    else
        {
        AssertTrueL(pos.Altitude() == KAltitude, KAltitudeErr, KErrGeneral);
        }
        	    
    AssertTrueL(pos.HorizontalAccuracy() == KHorizontalAcc, KHorizontalErr, KErrGeneral);
    AssertTrueL(pos.VerticalAccuracy() == KVerticalAcc, KVerticalErr, KErrGeneral);
    
   	if (fullFix) 
   		{
   		_LIT(KFullFix, "Read fullFix");
   		INFO_PRINTF1(KFullFix);
   		}
	else 
		{
		_LIT(KPartialUpdate, "Read partial update");
		INFO_PRINTF1(KPartialUpdate);
		}
    
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp254::SetFileInUse()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp254::SetFileInUseL(const TDesC& aFileName, TBool aPartial)
    {
    _LIT(KOpenErr, "Error when opening positioner,  %d");
    _LIT(KSetUpdateOptionsErr, "Error %d when setting update options");
    
    iPositioner.Close();
    SetSimDataFileL(aFileName); 

    TInt err;
    err = iPositioner.Open(iPosServer, iUidSimulationPsy);
    AssertTrueL(err == KErrNone, KOpenErr, err);
    
    iPositioner.SetRequestor(CRequestor::ERequestorService,
							CRequestor::EFormatApplication, KServiceName);

    TPositionUpdateOptions posOption;
	posOption.SetAcceptPartialUpdates(aPartial);
	        
	err = iPositioner.SetUpdateOptions(posOption);    
    AssertTrueL(err == KErrNone, KSetUpdateOptionsErr, err);
    
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp254::SetPartialUpdateL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp254::SetPartialUpdateL(TBool aPartial)
    {
    _LIT(KSetUpdateOptionsErr, "Error %d when setting update options");

    TPositionUpdateOptions posOption;
    iPositioner.GetUpdateOptions(posOption);

	posOption.SetAcceptPartialUpdates(aPartial);
	        
	TInt err = iPositioner.SetUpdateOptions(posOption);    
    AssertTrueL(err == KErrNone, KSetUpdateOptionsErr, err);
    }

// ---------------------------------------------------------
// CT_LbsSimulationPSYPosTp254::SetUpdateTimeOutL()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp254::SetUpdateTimeOutL(const TInt& aInterval)
    {
    _LIT(KSetUpdateOptionsErr, "Error %d when setting update options");

    TPositionUpdateOptions posOption;
    iPositioner.GetUpdateOptions(posOption);
    
	posOption.SetUpdateTimeOut(TTimeIntervalMicroSeconds(aInterval));
	        
	TInt err = iPositioner.SetUpdateOptions(posOption);    
    AssertTrueL(err == KErrNone, KSetUpdateOptionsErr, err);
    }
    
// ---------------------------------------------------------
// TNmeaSentenceParser::PrintChecksum
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsSimulationPSYPosTp254::PrintChecksum(const TDesC8& aSentence)
    {
    TUint8 checksum = 0;
    
    TInt i;
    for (i = 1; i < aSentence.Length() && aSentence[i] != '*'; i++)
        {
        checksum ^= aSentence[i];
        }

    TUint8 lsb = (TUint8)(checksum & 0x0f);
    TUint8 msb = (TUint8)(checksum >> 4); 
   
    _LIT(KStatus, "Checksum field  = %x%x");
    TBuf<32> info;
    info.Format(KStatus, msb, lsb);
	RDebug::Print(info);
    }

//  End of File
