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
#include "ctlbsclientpostp227.h"
#include <lbs/epos_cposmodules.h>
#include <lbs/epos_cposmoduleupdate.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>



// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// Constructor.
CT_LbsClientPosTp227::CT_LbsClientPosTp227(CT_LbsServer& aParent): CT_LbsPortedStepBase(aParent)
    {  
    _LIT(KTestName, "TP227 - Settings API performance");
    SetTestStepName(KTestName); 
    }

// Destructor
CT_LbsClientPosTp227::~CT_LbsClientPosTp227() 
	{
    
	}

// ---------------------------------------------------------
// CT_LbsClientPosTp227::CloseTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CT_LbsClientPosTp227::CloseTest()
    {
    
    }

// ---------------------------------------------------------
// CT_LbsClientPosTp227::StartL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp227::StartL()
    {   
    TTime startTime;
    TTime stopTime;
        
    startTime.UniversalTime();
    
    CPosModules* db = CPosModules::OpenL();
    stopTime.UniversalTime();
    
    TInt duration =  stopTime.Int64() - startTime.Int64();
    
    if (duration > 500000)
		{
    	_LIT(KDelayMsg, "Time taken to create CPosModules object was more than 0.5 second, it was %d microsecs.");
        TBuf<100> error;
        error.Format(KDelayMsg, duration);
        INFO_PRINTF1(error);    
		}
    else
        {
        _LIT(KDelayMsg, "Time taken to create CPosModules object was %d microsecs.");
        TBuf<100> debug;
        debug.Format(KDelayMsg, duration);
        INFO_PRINTF1(debug);
        }
    
    CleanupStack::PushL(db);
          
    startTime.UniversalTime();
    CPosModuleIdList* prioList = db->ModuleIdListLC();
    
    stopTime.UniversalTime();
    duration =  stopTime.Int64() - startTime.Int64();
    
    if (duration > 20000)
		{
    	_LIT(KDelayMsg, "Time taken to get list using ModuleIdListLC() was more than 0.02 second, it was %d microsecs.");
        TBuf<100> error;
        error.Format(KDelayMsg, duration);
        INFO_PRINTF1(error);
		}
    else
        {
        _LIT(KDelayMsg, "Time taken to get list using ModuleIdListLC() was %d microsecs.");
        TBuf<100> debug;
        debug.Format(KDelayMsg, duration);
        INFO_PRINTF1(debug);
        }

    TPositionModuleInfo moduleInfo;
    TInt64 MaxDuration=0;
    
    for ( TInt count = 0 ; count < prioList->Count() ; count++)
        {                    
        startTime.UniversalTime();
        db->GetModuleInfoL((*prioList)[count], moduleInfo);
        stopTime.UniversalTime();
        duration =  stopTime.Int64() - startTime.Int64();
        
        if( duration>MaxDuration )
            MaxDuration=duration;           
        }

    if (MaxDuration > 10000)
		{
    	_LIT(KDelayMsg, "Time taken for GetModuleInfoL() was more than 0.01 sec, it was %d microsecs.");
        TBuf<100> error;
        error.Format(KDelayMsg, duration);
        INFO_PRINTF1(error);
		}
    else
        {
        _LIT(KDelayMsg, "Time taken for GetModuleInfoL() was %d microsecs.");
        TBuf<100> debug;
        debug.Format(KDelayMsg, duration);
        INFO_PRINTF1(debug);
        }
    
    CleanupStack::PopAndDestroy(prioList); // prioList
    
    TInt visiblestate=1;
    CPosModuleUpdate* moduleUpdate = CPosModuleUpdate::NewLC(); // destroy!
    moduleUpdate->SetUpdateAvailability(ETrue);
    
    MaxDuration=0;
    for(TInt i=0;i<10;i++)
    {
	    if(visiblestate)
	    { 
	    moduleUpdate->SetUpdateVisibility(EFalse);
	    visiblestate=0;
	    }
	    else
	    { 
	    moduleUpdate->SetUpdateVisibility(ETrue);
	    visiblestate=1;
	    }
	    
	    db->UpdateModuleL(iUidTestPsy2, *moduleUpdate);
	    
	    startTime.UniversalTime();
	    prioList = db->ModuleIdListLC();
	    stopTime.UniversalTime();
	    duration =  stopTime.Int64() - startTime.Int64();
	    
	    if( duration> MaxDuration)
	    MaxDuration=duration;

	    CleanupStack::PopAndDestroy(prioList);
    }
  
    if (MaxDuration > 50000)
		{
    	_LIT(KDelayMsg, "Time taken was more than 0.05 second, it was %d microsecs.");
        TBuf<100> error;
        error.Format(KDelayMsg, duration);
        INFO_PRINTF1(error);
		}
    else
        {
        _LIT(KDelayMsg, "Time taken was %d microsecs.");
        TBuf<100> debug;
        debug.Format(KDelayMsg, duration);
        INFO_PRINTF1(debug);
        }
        
       CleanupStack::PopAndDestroy(2); //moduleUpdate,db   
    }
    
// ---------------------------------------------------------
// CT_LbsClientPosTp227::AssertTrueSecL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsClientPosTp227::AssertTrueL(TBool aCondition, const TDesC& aErrorMsg)
    {
  	if (!aCondition)
		{
		TBuf<100> buf;
		buf.Format(aErrorMsg);
		LogErrorAndLeaveL(buf);
		}
    }


