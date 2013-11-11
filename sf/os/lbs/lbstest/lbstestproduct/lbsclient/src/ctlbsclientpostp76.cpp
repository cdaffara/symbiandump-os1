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
#include "ctlbsclientpostp76.h"
#include <badesca.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp76::CT_LbsClientPosTp76(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName,"TP76 - Get module data");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp76::~CT_LbsClientPosTp76()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp76::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp76::StartL()
    {
	// Open connection to Epos    
	RPositionServer posServer;
	CleanupClosePushL(posServer);
	TInt err = posServer.Connect();    
	_LIT(KConnectError, "TP76. Unable to open connection to Epos, return code %d");
	AssertTrueSecL(err == KErrNone, KConnectError, err);

	// Set priority for those psys that will be used in this tp.
	// The order will be testpsy1(externalgps),testpsy2,testpsy3
	TUid uidPsy1 = iUidTestPsy1;
	TUid uidPsy2 = iUidTestPsy2;
	TUid uidPsy3 = iUidTestPsy3;

    SetupPsyL(uidPsy1); // will disable all except uidPsy1

    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

    // Enable the PSY test psys 
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    moduleUpdate -> SetUpdateAvailability(ETrue);
    db->SetModulePriorityL(uidPsy1, 0);
    db->SetModulePriorityL(uidPsy2, 1);
    db->SetModulePriorityL(uidPsy3, 1);
    db->UpdateModuleL(uidPsy1, *moduleUpdate);
    db->UpdateModuleL(uidPsy2, *moduleUpdate);
    db->UpdateModuleL(uidPsy3, *moduleUpdate);
    CleanupStack::PopAndDestroy(moduleUpdate);

    // Setup the PSY:s in the correct order
    CPosModuleIdList* prioList = db->ModuleIdListLC();
    TInt numModules = prioList->Count();
    TPositionModuleInfo modInfoPsy2; //used to comapre that the same result is returned.
    db -> GetModuleInfoL(uidPsy2, modInfoPsy2);
    CleanupStack::PopAndDestroy(2); // db, prioList
	
	// Get default positioning module info. (testpsy1 or externalgps)
	TPositionModuleInfo modInfo;
    TPositionModuleId defaultId;
    err = posServer.GetDefaultModuleId(defaultId);
    _LIT(KDefaultModuleId, "Not possible to get default module id");
    AssertTrueSecL(err == KErrNone, KDefaultModuleId, err);
    // Check that the correct id was returned.
    if (defaultId != uidPsy1)
        {
        _LIT(KErrorWrongModule, "Default module was not returned in GetDefaultModuleId()");
        LogErrorAndLeaveL(KErrorWrongModule);
        }

    // Get default module info by using the id returned.
    err = posServer.GetModuleInfoById(defaultId, modInfo);
	_LIT(KGetDefPosModInfoError,
            "TP76. Error occured at GetModuleInfoById, return code %d");
	AssertTrueSecL(err == KErrNone, KGetDefPosModInfoError, err);
	// Check if the received psy is the right one
	_LIT(KIncorrectPsyError,
           "TP76. Incorrect PSY returned from Epos.");
	AssertTrueL(uidPsy1 == modInfo.ModuleId(), KIncorrectPsyError);
    //END TEST DEFAULT//

	//
    // TestGetModuleInfoById //
	//

    // Get info from specific psy (testpsy2)
	_LIT(KGetPosModInfoError, "Wrong data received from Epos");
    err = posServer.GetModuleInfoById(uidPsy2, modInfo);
	AssertTrueSecL(err == 0, KGetPosModInfoError, err);
	AssertTrueL(modInfo.ModuleId() == uidPsy2, KGetPosModInfoError);
    CheckModuleInfoL(modInfoPsy2, modInfo);
    //END TEST GetModuleInfoById//

	//
    // Test GetModuleInfoByIndex //
	//

	// Get info from the testpsys (testpsy1, testpsy2, testpsy3)
    TPositionModuleId id1;
    TPositionModuleId id2;
    TPositionModuleId id3;
    TUint count;
	err = posServer.GetNumModules(count);
	_LIT(KNumPosModInfoError,
            "TP76. Error occured at GetNumModules error code returned, return code %d");
	AssertTrueSecL(err == 0,KNumPosModInfoError,err);
	_LIT(KIncorrectNoOfModules,
           "TP76. Incorrect number of modules.");
    TUint no = (TUint) numModules; //Bad API implementation
	AssertTrueL(count == no, KIncorrectNoOfModules);

    err = posServer.GetModuleInfoByIndex(0, modInfo);
    AssertTrueSecL(err == KErrNone, KGetDefPosModInfoError, err);
    id1 = modInfo.ModuleId();
    if (!(id1 == uidPsy1 ||
        id1 == uidPsy2 ||
        id1 == uidPsy3))
        {
        LogErrorAndLeaveL(KGetPosModInfoError);
        }

    err = posServer.GetModuleInfoByIndex(1, modInfo);
    AssertTrueSecL(err == 0, KGetDefPosModInfoError, err);
    id2 = modInfo.ModuleId();
    if (!(id2 == uidPsy1 ||
        id2 == uidPsy2 ||
        id2 == uidPsy3))
        {
        LogErrorAndLeaveL(KGetPosModInfoError);
        }

    err = posServer.GetModuleInfoByIndex(2, modInfo);
    AssertTrueSecL(err == 0, KGetDefPosModInfoError, err);
    id3 = modInfo.ModuleId();
    if (!(id3 == uidPsy1 ||
        id3 == uidPsy2 ||
        id3 == uidPsy3))
        {
        LogErrorAndLeaveL(KGetPosModInfoError);
        }
    
    // Assuming that only 3 PSYs are installed
    err = posServer.GetModuleInfoByIndex(3, modInfo);
    AssertTrueSecL(err == 0, KGetDefPosModInfoError, err);

    // END TEST GetModuleInfoByIndex //

	CleanupStack::PopAndDestroy(); //posServer
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp76::CheckModuleInfoL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp76::CheckModuleInfoL(const TPositionModuleInfo& dbInfo,
                                const TPositionModuleInfo& clientInfo)
    {
    _LIT(KNotSame, "Module Info from settings is not the same as when read by GetModuleInfoById");
    if (dbInfo.IsAvailable() != clientInfo.IsAvailable() ||
        dbInfo.ModuleId() != clientInfo.ModuleId() ||
        dbInfo.DeviceLocation() != clientInfo.DeviceLocation() ||
        dbInfo.Capabilities() != clientInfo.Capabilities() ||
        dbInfo.TechnologyType() != clientInfo.TechnologyType() ||
        dbInfo.Version().iBuild != clientInfo.Version().iBuild ||
        dbInfo.Version().iMajor != clientInfo.Version().iMajor ||
        dbInfo.Version().iMinor != clientInfo.Version().iMinor ||
        dbInfo.Version().Name() != clientInfo.Version().Name() ||
        dbInfo.ClassesSupported(EPositionInfoFamily) != clientInfo.ClassesSupported(EPositionInfoFamily))
        {
        LogErrorAndLeaveL(KNotSame);
        }
    TPositionQuality dbQuality;
    TPositionQuality clientQuality;
    dbInfo.GetPositionQuality(dbQuality);
    clientInfo.GetPositionQuality(clientQuality);
    if (dbQuality.TimeToFirstFix() != clientQuality.TimeToFirstFix() ||
        dbQuality.TimeToNextFix() != clientQuality.TimeToNextFix() ||
        dbQuality.HorizontalAccuracy() != clientQuality.HorizontalAccuracy() ||
        dbQuality.VerticalAccuracy() != clientQuality.VerticalAccuracy() ||
        dbQuality.CostIndicator() != clientQuality.CostIndicator() || 
        dbQuality.PowerConsumption() != clientQuality.PowerConsumption() )
        {
        LogErrorAndLeaveL(KNotSame);
        }


    TBuf<30> name1;
    TBuf<30> name2;
    dbInfo.GetModuleName(name1);
    clientInfo.GetModuleName(name2);
    if (name1 != name2)
        {
        LogErrorAndLeaveL(KNotSame);
        }
    }


// ---------------------------------------------------------
// CT_LbsClientPosTp76::AssertTrueL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp76::AssertTrueL(TBool aCondition, const TDesC& aErrorMsg)
    {
  	if (!aCondition)
		{
		LogErrorAndLeaveL(aErrorMsg);
		}
    }


// ---------------------------------------------------------
// CT_LbsClientPosTp76::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp76::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
  	if (!aCondition)
		{
		TBuf<100> buf;
		buf.Format(aErrorMsg, aErrorCode);
		LogErrorAndLeaveL(buf);
		}
    }
