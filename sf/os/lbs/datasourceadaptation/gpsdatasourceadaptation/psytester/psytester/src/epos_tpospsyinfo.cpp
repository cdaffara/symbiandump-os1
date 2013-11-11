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
#include    "epos_tpospsyinfo.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TPosPSYInfo::TPosPSYInfo() : 
	iValuesSet(EFalse),
	iInterfaceVersion(0),
	iPriority(0),
	iStatus(0),
	iFirstTimeToFix(0),
  iTimeToFix(0),
  iHorizontalAccuracy(0),
  iVerticalAccuracy(0),
  iCostIndicator(TPositionQuality::ECostUnknown),
  iPowerConsumption(TPositionQuality::EPowerUnknown),
  iTechnologyType(TPositionModuleInfo::ETechnologyUnknown),
  iDeviceLocation(TPositionModuleInfo::EDeviceUnknown),
  iCapability(TPositionModuleInfo::ECapabilityNone),
  iClassesSupported(0),
  iVisibility(EFalse),
  iPSYBaseInfo()
    {
    }
    
// ---------------------------------------------------------
// TPosPSYInfo::SetInterfaceVersion()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetInterfaceVersion(
    const TInt aIntfVersion)
    {
    iInterfaceVersion = aIntfVersion;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetInterfaceVersion()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetPriority(
    const TInt aPriority)
    {
    iPriority = aPriority;
    }
    
// ---------------------------------------------------------
// TPosPSYInfo::SetInterfaceVersion()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetStatus(
    const TInt aStatus)
    {
    iStatus = aStatus;
    }
    
// ---------------------------------------------------------
// TPosPSYInfo::SetTimeToFirstFix()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetTimeToFirstFix(
    const TTimeIntervalMicroSeconds aTime)
    {
    iFirstTimeToFix = aTime;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetTimeToFix()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetTimeToFix(
    const TTimeIntervalMicroSeconds aTime)
    {
    iTimeToFix = aTime;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetHorizontalAccuracy()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetHorizontalAccuracy(
    const TReal32 aAccuracy)
    {
    iHorizontalAccuracy = aAccuracy;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetVerticallAccuracy()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetVerticalAccuracy(
    const TReal32 aAccuracy)
    {
    iVerticalAccuracy = aAccuracy;   
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetCostIndicator()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetCostIndicator(
    const TPositionQuality::TCostIndicator aCostIndicator)
    {
    iCostIndicator = aCostIndicator;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetPowerConsumption()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetPowerConsumption(
    const TPositionQuality::TPowerConsumption aPowerConsumption)
    {
    iPowerConsumption = aPowerConsumption;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetTechnologyType()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetTechnologyType(
    const TPositionModuleInfo::TTechnologyType aTechnologyType)
    {
    iTechnologyType = aTechnologyType;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetDeviceLocation()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetDeviceLocation(
    const TPositionModuleInfo::TDeviceLocation aDeviceLocation)
    {
    iDeviceLocation = aDeviceLocation;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetCapabilities()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetCapabilities(
    const TPositionModuleInfo::TCapabilities aCapability)
    {
    iCapability = aCapability;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetClassesSupported()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetClassesSupported(
    const TUint32 aClassesSupported)
    {
    iClassesSupported = aClassesSupported;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetVisibility()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetVisibility(
    const TInt aVisibility)
    {
    iVisibility = aVisibility;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetPSYBaseInfo()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetPSYBaseInfo(
    const TPosPSYBaseInfo aPSYBaseInfo)
    {
    iPSYBaseInfo = aPSYBaseInfo;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetValuesSet()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void TPosPSYInfo::SetValuesSet(
    const TBool aSet)
    {
    iValuesSet = aSet;
    }

// ---------------------------------------------------------
// TPosPSYInfo::InterfaceVersion()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYInfo::InterfaceVersion() const
    {
    return iInterfaceVersion;
    }
// ---------------------------------------------------------
// TPosPSYInfo::InterfaceVersion()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYInfo::Priority() const
    {
    return iPriority;
    }

// ---------------------------------------------------------
// TPosPSYInfo::Status()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYInfo::Status() const
    {
    return iStatus;
    }


// ---------------------------------------------------------
// TPosPSYInfo::TimeToFirstFix()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds TPosPSYInfo::TimeToFirstFix() const
    {
    return iFirstTimeToFix;
    }

// ---------------------------------------------------------
// TPosPSYInfo::TimeToFix()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds TPosPSYInfo::TimeToFix() const
    {
    return iTimeToFix;
    }

// ---------------------------------------------------------
// TPosPSYInfo::HorizontalAccuracy()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TReal32 TPosPSYInfo::HorizontalAccuracy() const
    {
    return iHorizontalAccuracy;
    }

// ---------------------------------------------------------
// TPosPSYInfo::VerticallAccuracy()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TReal32 TPosPSYInfo::VerticalAccuracy() const
    {
    return iVerticalAccuracy;   
    }

// ---------------------------------------------------------
// TPosPSYInfo::CostIndicator()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPositionQuality::TCostIndicator TPosPSYInfo::CostIndicator() const
    {
    return iCostIndicator;
    }

// ---------------------------------------------------------
// TPosPSYInfo::PowerConsumption()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPositionQuality::TPowerConsumption TPosPSYInfo::PowerConsumption() const
    {
    return iPowerConsumption;
    }

// ---------------------------------------------------------
// TPosPSYInfo::SetTechnologyType()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPositionModuleInfo::TTechnologyType TPosPSYInfo::TechnologyType() const
    {
    return iTechnologyType;
    }

// ---------------------------------------------------------
// TPosPSYInfo::DeviceLocation()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPositionModuleInfo::TDeviceLocation TPosPSYInfo::DeviceLocation() const
    {
    return iDeviceLocation;
    }

// ---------------------------------------------------------
// TPosPSYInfo::Capabilities()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPositionModuleInfo::TCapabilities TPosPSYInfo::Capabilities() const
    {
    return iCapability;
    }

// ---------------------------------------------------------
// TPosPSYInfo::ClassesSupported()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TUint32 TPosPSYInfo::ClassesSupported() const
    {
    return iClassesSupported;
    }

// ---------------------------------------------------------
// TPosPSYInfo::Visibility()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TPosPSYInfo::Visibility() const
    {
    return iVisibility;
    }

// ---------------------------------------------------------
// TPosPSYInfo::PSYBaseInfo()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TPosPSYBaseInfo TPosPSYInfo::PSYBaseInfo() const
    {
    return iPSYBaseInfo;
    }

// ---------------------------------------------------------
// TPosPSYInfo::ValuesSet()
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool TPosPSYInfo::ValuesSet() const
    {
    return iValuesSet;
    }


//  End of File
