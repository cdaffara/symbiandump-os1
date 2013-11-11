// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <e32base.h>
#include <ecom/ecom.h>
#include <barsread.h> //TResourceReader
#include <centralrepository.h> // Central Rep
#include <lbs/positioningpluginfwinternalcrkeys.h> // Centarl Rep Keys
#include "epos_cpospsyecomhandler.h"

// ==================== LOCAL FUNCTIONS ====================

// ---------------------------------------------------------
// ResetAndDestroy Resets and destroys the members in a RImplInfoPtrArray. This
// function can be used as a TCleanupItem.
// ---------------------------------------------------------
//
void ResetAndDestroy(
    TAny* aArray)
    {
    ((RImplInfoPtrArray*)aArray)->ResetAndDestroy();
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYEComHandler::CPosPSYEComHandler()
    {
    }

// Two-phased constructor.
CPosPSYEComHandler* CPosPSYEComHandler::NewL()
    {
    CPosPSYEComHandler* self = new (ELeave) CPosPSYEComHandler;
    return self;
    }

// Destructor
CPosPSYEComHandler::~CPosPSYEComHandler()
    {
	REComSession::FinalClose();
	}

// ---------------------------------------------------------
// CPosPSYEComHandler::GetPsyInfoListL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYEComHandler::GetPsyInfoListL(
    RArray<TPosPSYBaseInfo>& aPSYInfoArray)
	{
   	RImplInfoPtrArray implInfoArray;
	CleanupStack::PushL(TCleanupItem(*ResetAndDestroy, &implInfoArray));
    REComSession::ListImplementationsL(KPosPsyInterfaceUid, implInfoArray);

    for (TInt index = 0; index < implInfoArray.Count(); index++)
        {
            const TUid moduleId = implInfoArray[index]->ImplementationUid();
            const TDesC& displayName = implInfoArray[index]->DisplayName();
            const TInt version = implInfoArray[index]->Version();
            TPosPSYBaseInfo psyInfo(moduleId, displayName, version);
            aPSYInfoArray.Append(psyInfo);
        }
    CleanupStack::PopAndDestroy(&implInfoArray);
   	}

// ---------------------------------------------------------
// CPosPSYEComHandler::GetSpecificPSYInformationL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//   	
void CPosPSYEComHandler::GetSpecificPSYInformationL(
    TPosPSYInfo& aPSYInfo)
	{
	const TInt KPosSecond = 1000000;
	
    RImplInfoPtrArray implInfoArray;
    CleanupStack::PushL(TCleanupItem(*ResetAndDestroy, &implInfoArray));
    REComSession::ListImplementationsL(KPosPsyInterfaceUid, implInfoArray);
    TUid moduleId = aPSYInfo.PSYBaseInfo().Uid();
    TBool found(EFalse);
    TInt index = 0;
    for (index = 0; index < implInfoArray.Count() && !found; index++)
        {
        found = (moduleId == implInfoArray[index]->ImplementationUid());
        }
    if (!found)
        {
        User::Leave(KErrNotFound);
        }
        
    // PSY parameters in the central repository 
    CRepository *centralRep = NULL;
    TRAPD(error, centralRep = CRepository::NewL(moduleId));
    
    if(!error && centralRep)
    {
	    
	    aPSYInfo.SetValuesSet(ETrue);
	    
	    TInt ret = -1;
	    
	    //+ Interface Version
	    TInt interfaceVersion;
	    ret = centralRep->Get(KPSYInterfaceVersion, interfaceVersion);
	    if(KErrNone == ret)
	    {
	    	aPSYInfo.SetInterfaceVersion(interfaceVersion);
	    }
	    
	    //+ priority
	    TInt priority;
	    ret = centralRep->Get(KPSYPriority, priority);
	    if(KErrNone == ret)
	    {
	    	aPSYInfo.SetPriority(priority);
	    }
	    
	    //+ Availability
	    TInt availStatus;
	    ret = centralRep->Get(KPSYStatus, availStatus);
	    	   
	    if(0 == ret)
		{
			aPSYInfo.SetStatus(availStatus);
		}
		
		//+ TimeToFirstFix
		TInt timeToFirstFix;
		
		ret = centralRep->Get(KPSYTimeToFirstFix, timeToFirstFix);
		if(0 == ret)
		{
			
			aPSYInfo.SetTimeToFirstFix(TTimeIntervalMicroSeconds(timeToFirstFix * KPosSecond));
		}
		
		
		//+ TimeToNextFix
		TInt timeToNextFix;
		ret = centralRep->Get(KPSYTimeToNextFix, timeToNextFix);
		if(0 == ret)
		{
			aPSYInfo.SetTimeToFix(TTimeIntervalMicroSeconds(timeToNextFix * KPosSecond));
		}
			
	    
	    //+ Horizontal Accuray
		TReal64 horizontalAccuracy;
		ret = centralRep->Get(KPSYHorizontalAccuracy, horizontalAccuracy);
		if(0 == ret)
		{
			aPSYInfo.SetHorizontalAccuracy(
				static_cast<TReal32>(horizontalAccuracy));
		}
			
		//+ Vertical Accuracy
		TReal64 verticalAccuracy;
		ret = centralRep->Get(KPSYVerticalAccuracy, verticalAccuracy);
		if(0 == ret)
		{
			aPSYInfo.SetVerticalAccuracy(
				static_cast<TReal32>(verticalAccuracy));
		}

		//+ Cost Indicator
		TInt costIndicator;
		ret = centralRep->Get(KPSYCostIndicator, costIndicator);
		if(0 == ret)
		{
			aPSYInfo.SetCostIndicator(
				static_cast<TPositionQuality::TCostIndicator>(costIndicator));
		}

		//+ Power Consumption
		TInt powerConsumption;
		ret = centralRep->Get(KPSYPowerConsumption, powerConsumption);
		if(0 == ret)
		{
			aPSYInfo.SetPowerConsumption(
				static_cast<TPositionQuality::TPowerConsumption>(powerConsumption));
		}

		//+ Technology Type
		TInt technologyType;
		ret = centralRep->Get(KPSYTechnologyType, technologyType);
		if(0 == ret)
		{
			aPSYInfo.SetTechnologyType(
				static_cast<TPositionModuleInfo::TTechnologyType>(technologyType));
		}

		//+ Device Location
		TInt deviceLocation;
		ret = centralRep->Get(KPSYDeviceLocation, deviceLocation);
		if(0 == ret)
		{
			aPSYInfo.SetDeviceLocation(
				static_cast<TPositionModuleInfo::TDeviceLocation>(deviceLocation));
		}

		//+ Capabilities
		TInt capabilities;
		ret = centralRep->Get(KPSYCapabilities, capabilities);
		if(0 == ret)
		{
			aPSYInfo.SetCapabilities(
				static_cast<TPositionModuleInfo::TCapabilities>(capabilities));
		}

		//+ Classes Supported
		TInt classesSupported;
		ret = centralRep->Get(KPSYClassesSupported, classesSupported);
		if(0 == ret)
		{
			aPSYInfo.SetClassesSupported(classesSupported);
		}

		//+ Visibility
		TInt visibility;
		ret = centralRep->Get(KPSYVisibility, visibility);
		if(0 == ret)
		{
			aPSYInfo.SetVisibility(visibility);
		}
			
		//+ delete centralRep
	    delete centralRep;
    } 
	    
    CleanupStack::PopAndDestroy(&implInfoArray);
    }
   	
//  End of File
