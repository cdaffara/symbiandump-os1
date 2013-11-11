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
#include "ctlbsclientpostp169.h"
#include <badesca.h>
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
CT_LbsClientPosTp169::CT_LbsClientPosTp169(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
	{  
	_LIT(KTestName, "TP169 - Modules Db Tests");
	SetTestStepName(KTestName); 
	}

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
CT_LbsClientPosTp169::~CT_LbsClientPosTp169()
	{
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp169::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp169::StartL()
    {
    _LIT(KErrorMsg1, "Wrong priority order in module list");
   
        
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);


    CPosModuleIdList* prioList = db->ModuleIdListLC();

// TC194 // List Modules
    
    _LIT(KErrorMsg4, "Mismatch between list by settings & aquisition API");
    _LIT(KErrorMsg2, "Installed module in list is returned as uninstalled by IsModuleInstalled");
    TUid moduleUid;
    TUid moduleid1;
    TUint lastPrio = 0;
    TUint thisPrio = 0;
	TInt count = 0;
	TPositionModuleInfo moduleInfo;
    for (count = 0 ; count < prioList->Count() ; count++)
        {
        moduleUid = (*prioList)[count];
     //
        
        
        TBool IsInstalled=db->IsModuleInstalled(moduleUid);
        AssertTrueL(IsInstalled,KErrorMsg2);
                        
        db->GetModuleInfoL((*prioList)[count], moduleInfo);
        moduleid1=moduleInfo.ModuleId();
        AssertTrueL(moduleid1==moduleUid,KErrorMsg4);
      //  
        
        thisPrio = db->PriorityL(moduleUid);

        if (count > 0)
            {
            AssertTrueL(thisPrio > lastPrio,
                        KErrorMsg1);
            }

        lastPrio = thisPrio;
        }

    CleanupStack::PopAndDestroy(prioList); // prioList

// TC195 // Get Module Properties
    

    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC(); // destroy!
    moduleUpdate->SetUpdateAvailability(ETrue);
    moduleUpdate->SetUpdateCost(TPositionQuality::ECostCharge); // EEPosHigh
    //
    moduleUpdate->SetUpdateVisibility(ETrue);
    db->UpdateModuleL(iUidTestPsy2, *moduleUpdate);

        
    db->SetModulePriorityL(iUidTestPsy2, 3);
    
    db->GetModuleInfoL(iUidTestPsy2, 
                       moduleInfo);

    

    TPositionQuality quality;
    moduleInfo.GetPositionQuality(quality);

    _LIT(KModName, "Test PSY 2");
    _LIT(KStatusErrorText, "Wrong status data in module info");
    _LIT(KVersionErrorText, "Wrong version data in module info");
    _LIT(KUidErrorText, "Wrong uid data in module info");
    _LIT(KNameErrorText, "Wrong name data in module info");
    _LIT(KPosTechErrorText, "Wrong pos tech data in module info");
    _LIT(KTTFFErrorText, "Wrong TTFF data in module info");
    _LIT(KTTFErrorText, "Wrong TTF data in module info");
    _LIT(KHAccErrorText, "Wrong HAcc data in module info");
    _LIT(KVAccErrorText, "Wrong VAcc data in module info");
    _LIT(KCostErrorText, "Wrong cost data in module info");
    _LIT(KPowerCostErrorText, "Wrong power cost data in module info");
    _LIT(KClassesErrorText, "Wrong class info in module info");


    AssertTrueL(moduleInfo.IsAvailable(), KStatusErrorText);
    TVersion version = moduleInfo.Version();
    AssertTrueL(version.iMajor == 1, KVersionErrorText);
    AssertTrueL(version.iMinor == 0, KVersionErrorText);
    AssertTrueL(version.iBuild == 0, KVersionErrorText);

    AssertTrueL(moduleInfo.ModuleId() == iUidTestPsy2, KUidErrorText);
    TBuf<100> moduleName;
    moduleInfo.GetModuleName(moduleName);
    AssertTrueL(moduleName == KModName, KNameErrorText);
    AssertTrueL(moduleInfo.TechnologyType() == TPositionModuleInfo::ETechnologyTerminal,
                                                KPosTechErrorText); 
    
	AssertTrueL(moduleInfo.ClassesSupported(EPositionInfoFamily) ==
                                                (EPositionGenericInfoClass | EPositionInfoClass),
                                                KClassesErrorText);

	AssertTrueL(quality.TimeToFirstFix() == TTimeIntervalMicroSeconds(3000000),
                                               KTTFFErrorText);
    AssertTrueL(quality.TimeToNextFix() == TTimeIntervalMicroSeconds(3000000),
                                               KTTFErrorText);
    AssertTrueL(quality.HorizontalAccuracy() == 2, KHAccErrorText);
    AssertTrueL(quality.VerticalAccuracy() == 3, KVAccErrorText);
    AssertTrueL(quality.CostIndicator() == TPositionQuality::ECostCharge, 
                                            KCostErrorText); // Updated cost value
    AssertTrueL(quality.PowerConsumption() == TPositionQuality::EPowerZero, 
                                                KPowerCostErrorText);

// TC196 // Set Module Properties

    _LIT(KErrorTp196b, "Cost not updated correctly");
    _LIT(KErrorMsg3, "Wrong Visibility");
    _LIT(KErrorMsgPrio, "Changing priority of an invisible module");

    // Make sure test psy 2 is enabled.
    CPosModuleUpdate* moduleUpdate196 = CPosModuleUpdate::NewLC(); // destroy!
    moduleUpdate196->SetUpdateAvailability(ETrue);
    moduleUpdate->SetUpdateCost(TPositionQuality::ECostZero);
    db->UpdateModuleL(iUidTestPsy2, *moduleUpdate196);

    // Disable psy
    moduleUpdate196->SetUpdateAvailability(EFalse);
    db->UpdateModuleL(iUidTestPsy2, *moduleUpdate196);

    // Update cost variable to ECostZero
    moduleUpdate196->SetUpdateCost(TPositionQuality::ECostZero);
    db->UpdateModuleL(iUidTestPsy2, *moduleUpdate196);
	db->GetModuleInfoL(iUidTestPsy2, 
                       moduleInfo);
    moduleInfo.GetPositionQuality(quality);
    AssertTrueL(quality.CostIndicator() == TPositionQuality::ECostZero, KErrorTp196b);

    // Update cost variable to ECostPossible
    moduleUpdate196->SetUpdateCost(TPositionQuality::ECostPossible);
    db->UpdateModuleL(iUidTestPsy2, *moduleUpdate196);
	db->GetModuleInfoL(iUidTestPsy2, 
                       moduleInfo);
    moduleInfo.GetPositionQuality(quality);
    AssertTrueL(quality.CostIndicator() == TPositionQuality::ECostPossible, KErrorTp196b);

    // Update cost variable to high
    moduleUpdate196->SetUpdateCost(TPositionQuality::ECostCharge);
    db->UpdateModuleL(iUidTestPsy2, *moduleUpdate196);
	db->GetModuleInfoL(iUidTestPsy2, 
                       moduleInfo);
    moduleInfo.GetPositionQuality(quality);
    AssertTrueL(quality.CostIndicator() == TPositionQuality::ECostCharge, KErrorTp196b);
    
    
    //
    moduleUpdate196->SetUpdateAvailability(ETrue);
        
    moduleUpdate196->SetUpdateVisibility(EFalse);
    db->UpdateModuleL(iUidTestPsy2, *moduleUpdate196);
    TBool IsVisble=db->IsModuleVisibleL(iUidTestPsy2);
    AssertTrueL(IsVisble==EFalse,
                        KErrorMsg3);
                        
    // Change priority of an invisible module
    
    TRAPD(err,db->SetModulePriorityL(iUidTestPsy2, 4));
    AssertTrueL(err==KErrNotFound,KErrorMsgPrio);
          
                        
    moduleUpdate196->SetUpdateVisibility(ETrue);
    db->UpdateModuleL(iUidTestPsy2, *moduleUpdate196);
    IsVisble=db->IsModuleVisibleL(iUidTestPsy2);
    AssertTrueL(IsVisble,KErrorMsg3);   
   //

// TC197 // Module Priority

    // Change the priority of the external gps module, and check that the other
    // modules are updated correctly
    // Change from 6 to 3

    _LIT(KErrorTextPrio, "Priority test failed after change of priority");
    _LIT(KErrorVisibility1, "Invisible module is in list");
    _LIT(KErrorVisibility2, "visible module is not in list after changing it from invisible");
    db->SetModulePriorityL(iUidTestPsy2, 6);
    prioList = db->ModuleIdListLC();
    lastPrio = 0;
    thisPrio = 0;
    for (count = 0 ; count < prioList->Count() ; count++)
        {
        moduleUid = (*prioList)[count];
        thisPrio = db->PriorityL(moduleUid);

        if (count > 0)
            {
            AssertTrueL(thisPrio > lastPrio,
                        KErrorMsg1);
            }

        lastPrio = thisPrio;
        }

    // Remember the uid:s of the modules close to iUidTestPsy2:
    TUid uid2a = (*prioList)[2];
    TUid uid3a = (*prioList)[3];
    TUid uid4a = (*prioList)[4];
    TUid uid5a = (*prioList)[5];
    TUid uid6a = (*prioList)[6];
    TUid uid7a = (*prioList)[7];
    TUid uid8a = (*prioList)[8];
    TUid uid0a = (*prioList)[0];

    db->SetModulePriorityL(iUidTestPsy2, 3);

    CleanupStack::PopAndDestroy(prioList); // prioList
    prioList = db->ModuleIdListLC();

    TUid uid2b = (*prioList)[2];
    TUid uid3b = (*prioList)[3];
    TUid uid4b = (*prioList)[4];
    TUid uid5b = (*prioList)[5];
    TUid uid6b = (*prioList)[6];
    TUid uid7b = (*prioList)[7];
    TUid uid8b = (*prioList)[8];
    TUid uid0b = (*prioList)[0];

    AssertTrueL(uid2a == uid2b, KErrorTextPrio);
    AssertTrueL(uid3a == uid4b, KErrorTextPrio);
    AssertTrueL(uid4a == uid5b, KErrorTextPrio);
    AssertTrueL(uid5a == uid6b, KErrorTextPrio);
    AssertTrueL(uid6a == uid3b, KErrorTextPrio);
    AssertTrueL(uid7a == uid7b, KErrorTextPrio);
    AssertTrueL(uid8a == uid8b, KErrorTextPrio);
    AssertTrueL(uid0a == uid0b, KErrorTextPrio);
    
    db->SetModulePriorityL(iUidTestPsy2, 7);
                                    
    CleanupStack::PopAndDestroy(prioList); // prioList
    prioList = db->ModuleIdListLC();

    uid2a = (*prioList)[2];
    uid3a = (*prioList)[3];
    uid4a = (*prioList)[4];
    uid5a = (*prioList)[5];
    uid6a = (*prioList)[6];
    uid7a = (*prioList)[7];
    uid8a = (*prioList)[8];
    uid0a = (*prioList)[0];
    TUid uid1a = (*prioList)[1];

    AssertTrueL(uid2b == uid2a, KErrorTextPrio);
    AssertTrueL(uid3b == uid7a, KErrorTextPrio);
    AssertTrueL(uid4b == uid3a, KErrorTextPrio);
    AssertTrueL(uid5b == uid4a, KErrorTextPrio);
    AssertTrueL(uid6b == uid5a, KErrorTextPrio);
    AssertTrueL(uid7b == uid6a, KErrorTextPrio);
    AssertTrueL(uid8b == uid8a, KErrorTextPrio);
    AssertTrueL(uid0b == uid0a, KErrorTextPrio);


    TInt bottomPrio = prioList->Count() - 1;

    // Change top priority to bottom priority
    
    db->SetModulePriorityL(uid0a, bottomPrio);
    CleanupStack::PopAndDestroy(prioList); // prioList
    prioList = db->ModuleIdListLC();

    AssertTrueL((*prioList)[0] == uid1a, KErrorTextPrio);
    AssertTrueL(uid0a == (*prioList)[bottomPrio], KErrorTextPrio);
    AssertTrueL(uid7a == (*prioList)[6], KErrorTextPrio);
    

    // Change bottom priority to top priority
    TUid topPrioUid = (*prioList)[0];
    TUid bottomPrioUid = (*prioList)[bottomPrio];
    TUid nextBottomPrioUid = (*prioList)[bottomPrio - 1];

    db->SetModulePriorityL(bottomPrioUid, 0);
    CleanupStack::PopAndDestroy(prioList); // prioList
    prioList = db->ModuleIdListLC();

    AssertTrueL((*prioList)[0] == bottomPrioUid, KErrorTextPrio);
    AssertTrueL((*prioList)[1] == topPrioUid, KErrorTextPrio);
    AssertTrueL((*prioList)[bottomPrio] == nextBottomPrioUid, KErrorTextPrio);

    CleanupStack::PopAndDestroy(prioList); // prioList
    
    //
    
    db->SetModulePriorityL(iUidTestPsy2, 0);
                                    
    prioList = db->ModuleIdListLC();

    uid0a = (*prioList)[0];
    uid1a = (*prioList)[1];
    uid2a = (*prioList)[2];
    uid3a = (*prioList)[3];
    uid4a = (*prioList)[4];
    uid5a = (*prioList)[5];
    uid6a = (*prioList)[6];
    uid7a = (*prioList)[7];
    uid8a = (*prioList)[8];
    
    moduleUpdate196->SetUpdateVisibility(EFalse);
    db->UpdateModuleL(iUidTestPsy2, *moduleUpdate196);
    
    CleanupStack::PopAndDestroy(prioList); // prioList
    prioList = db->ModuleIdListLC();
    
    TInt IsInList=prioList->Find(iUidTestPsy2);
    AssertTrueL(IsInList==KErrNotFound, KErrorVisibility1);
    
   
    uid0b = (*prioList)[0];
    TUid uid1b = (*prioList)[1];
    uid2b = (*prioList)[2];
    uid3b = (*prioList)[3];
    uid4b = (*prioList)[4];
    uid5b = (*prioList)[5];
    uid6b = (*prioList)[6];
    uid7b = (*prioList)[7];
    
    
    AssertTrueL(uid1a == uid0b, KErrorTextPrio);
    AssertTrueL(uid2a == uid1b, KErrorTextPrio);
    AssertTrueL(uid3a == uid2b, KErrorTextPrio);
    AssertTrueL(uid4a == uid3b, KErrorTextPrio);
    AssertTrueL(uid5a == uid4b, KErrorTextPrio);
    AssertTrueL(uid6a == uid5b, KErrorTextPrio);
    AssertTrueL(uid7a == uid6b, KErrorTextPrio);
    AssertTrueL(uid8a == uid7b, KErrorTextPrio);
    
     moduleUpdate196->SetUpdateVisibility(ETrue);
    db->UpdateModuleL(iUidTestPsy2, *moduleUpdate196);
    
    CleanupStack::PopAndDestroy(prioList); // prioList
    prioList = db->ModuleIdListLC();
    
    IsInList=prioList->Find(iUidTestPsy2);
        
    TInt priority=db->PriorityL(iUidTestPsy2);
   
    
    AssertTrueL(IsInList==priority, KErrorVisibility2);
    
    CleanupStack::PopAndDestroy(prioList); // prioList
    CleanupStack::PopAndDestroy(2, moduleUpdate);
    
    //
    
    CleanupStack::PopAndDestroy(db); // db

	}


// ---------------------------------------------------------
// CT_LbsClientPosTp169::TestNaNFromPSY
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp169::TestNaNFromPSY()
	{
	_LIT(KFunctionName, "We are inside TestNaNFromPSY()");
	INFO_PRINTF1(KFunctionName);
    CPosModules* db = CPosModules::OpenL();
    CleanupStack::PushL(db);

	TPositionModuleInfo moduleInfo;
	TBuf<100> moduleName;

	// Use Panic Psy in this test, should have Horizontal accuracy defined to 0
	// and vertical accuracy not defined at all in resource file (should default to 0)
	// and when reading these attributes using the modulesdb they should be of type NaN (Not A Number)
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC();
    moduleUpdate->SetUpdateAvailability(ETrue);
    db->UpdateModuleL(iUidPanicPsy, *moduleUpdate);

    db->GetModuleInfoL(iUidPanicPsy, 
                       moduleInfo);

	moduleInfo.GetModuleName(moduleName);

    TPositionQuality quality;
    moduleInfo.GetPositionQuality(quality);

	if (!Math::IsNaN(quality.HorizontalAccuracy())) 
		{
		_LIT(KErrorAndLeave, "HorizontalAccuracy should be NaN");
		LogErrorAndLeaveL(KErrorAndLeave);
		}
	else 
		{
		_LIT(KHorizontalAccuracy, "HorizontalAccuracy NaN");
		INFO_PRINTF1(KHorizontalAccuracy);
		}

	if (!Math::IsNaN(quality.VerticalAccuracy())) 
		{
		_LIT(KErrorAndLeave, "VerticalAccuracy should be NaN");
		LogErrorAndLeaveL(KErrorAndLeave);
		}
	else 
		{
		_LIT(KVerticalAccuracy, "VerticalAccuracy NaN");
		INFO_PRINTF1(KVerticalAccuracy);
		}

    CleanupStack::PopAndDestroy(moduleUpdate);
	CleanupStack::PopAndDestroy(db);
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp169::AssertTrueL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp169::AssertTrueL(TBool aCondition, const TDesC& aErrorMsg)
    {
  	if (!aCondition)
		{
		LogErrorAndLeaveL(aErrorMsg);
		}
    }


// ---------------------------------------------------------
// CT_LbsClientPosTp169::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp169::AssertTrueSecL(TBool aCondition, const TDesC& aErrorMsg, TInt aErrorCode)
    {
  	if (!aCondition)
		{
		TBuf<100> buf;
		buf.Format(aErrorMsg, aErrorCode);
		LogErrorAndLeaveL(buf);
		}
    }


