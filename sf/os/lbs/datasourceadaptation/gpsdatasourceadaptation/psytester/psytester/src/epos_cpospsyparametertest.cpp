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
#include "epos_cpospsyparametertest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYParameterTest::CPosPSYParameterTest()
    {
    }

// EPOC default constructor can leave.
void CPosPSYParameterTest::ConstructL()
    {
    BaseConstructL();
    }

CPosPSYParameterTest* CPosPSYParameterTest::NewL()
    {
    CPosPSYParameterTest* self = new (ELeave) CPosPSYParameterTest;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosPSYParameterTest::~CPosPSYParameterTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::GetName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::GetName(
    TDes& aName) const
    {
    _LIT(KTestName, "Parameter Test.");
    aName = KTestName;
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::StartTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::StartTest(
    const TPosPSYInfo& aPSYInfo, 
    TRequestStatus& aStatus,
	const TPosPSYTestParameters& aTestParameters)
    {
    iMaxTimeToFirstFix = aTestParameters.MaxTimeToFirstFix();
	iMaxVerticalAccuracy = aTestParameters.MaxVerticalAccuracy();
	iReasonableVerticalAccuracy = aTestParameters.ResonableVertictalAccuracy();
	iMaxHorizontalAccuracy = aTestParameters.MaxHorizontalAccuracy();
	iReasonableHorizontalAccuracy = aTestParameters.ResonableHorizontalAccuracy();
	
	aStatus = KRequestPending;
    iRequestStatus = &aStatus;
	iTestStatus = KErrNone;
    iPSYInfo = aPSYInfo;
    
    TRAP(iTestStatus, ExecuteTestL());
    if (iTestStatus == KErrNone)
    {
    	CompleteTest();	
    }
	}

// ---------------------------------------------------------
// CPosPSYParameterTest::CancelTest
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CancelTest()
    {
	iTestStatus = KErrCancel;
	}

// ---------------------------------------------------------
// CPosPSYParameterTest::ExecuteTestL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::ExecuteTestL()
    {
    if(!(iPSYInfo.ValuesSet()))
    {
    	_LIT(KErrorMsg, "Parameters could not be read from the central repository");
    	AddTestResultL(KErrorMsg, EErrorMessage);
    }
    else
    {
	    CheckInterfaceVersionL();
	    CheckPriorityL();
	    CheckStatusL();
	    CheckTimeToFirstFixL();
	    CheckTimeToFixL();
	    CheckVerticalAccuracyL();
	    CheckHorizontalAccuracyL();
	    CheckCostIndicatorL();
	    CheckPowerConsumptionL();
	    CheckTechnologyTypeL();
	    CheckDeviceLocationL();
	    CheckCapabilitiesL();
		CheckClassesSupportedL();
	    CheckVisibilityL();
    }
    }


// ---------------------------------------------------------
// CPosPSYParameterTest::CheckInterfaceVersionL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckInterfaceVersionL()
    {
    TBuf<KParameterResultBufSize> buf;
    if (iPSYInfo.InterfaceVersion() < 0 )
		{
        _LIT(KError, "Interface Version negative, InterfaceVersion = %d");
        buf.Format(KError, iPSYInfo.InterfaceVersion());
        AddTestResultL(buf, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "InterfaceVersion = %d.");
        buf.Format(KInfo, iPSYInfo.InterfaceVersion());
        AddTestResultL(buf, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckInterfaceVersionL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckPriorityL()
    {
    TBuf<KParameterResultBufSize> buf;
    if (iPSYInfo.Priority() < 0 || iPSYInfo.Priority() > 2)
		{
        _LIT(KError, "Invalid Priority Value, Priority = %d");
        buf.Format(KError, iPSYInfo.Priority());
        AddTestResultL(buf, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "Priority = ");
        buf.Append(KInfo);
        
        switch(iPSYInfo.Priority())
        {
        	case 0:
        		_LIT(KAuto, "Auto");
        		buf.Append(KAuto);
        		break;	
        	case 1:
        		_LIT(KFirst, "First");
        		buf.Append(KFirst);
        		break;
        	case 2:
        		_LIT(KLast, "Last");
        		buf.Append(KLast);
        	    break;
        	default:
        	    // Do not do anything
        	    break;
        }
        
        AddTestResultL(buf, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckStatus
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckStatusL()
    {
    
    TBuf<KParameterResultBufSize> buf;
    if (iPSYInfo.Status() < 0 || iPSYInfo.Status() > 1)
		{
        _LIT(KError, "Invalid Status Value, Status = %d");
        buf.Format(KError, iPSYInfo.Status());
        AddTestResultL(buf, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "Status = ");
        
        buf.Append(KInfo);
        
        switch(iPSYInfo.Status())
        {
	        case 0:
        		_LIT(KNotAvailable, "Not Available");
        		buf.Append(KNotAvailable);
        		break;    		

        	case 1:
        		_LIT(KAvailable, "Available");
        		buf.Append(KAvailable);
        		break;
        	
        	default:
        		// Do nothing
        		break;
        		
         }
        
         AddTestResultL(buf, EInfoMessage);
        
        }
    }


// ---------------------------------------------------------
// CPosPSYParameterTest::CheckTimeToFirstFixL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckTimeToFirstFixL()
    {
    TBuf<KParameterResultBufSize> buf;
    if (iPSYInfo.TimeToFirstFix() < TTimeIntervalMicroSeconds(0) || 
		iPSYInfo.TimeToFirstFix() >= TTimeIntervalMicroSeconds(iMaxTimeToFirstFix))
        {
        _LIT(KError, "TimeToFirstFix negative or greater than %d microsecs, TimeToFirstFix = %d microsecs.");
        buf.Format(KError, iMaxTimeToFirstFix, I64LOW(iPSYInfo.TimeToFirstFix().Int64()));
        AddTestResultL(buf, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "TimeToFirstFix = %d microsecs.");
        buf.Format(KInfo, I64LOW(iPSYInfo.TimeToFirstFix().Int64()));
        AddTestResultL(buf, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckTimeToFixL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckTimeToFixL()
    {
    TBuf<KParameterResultBufSize> buf;
    if (iPSYInfo.TimeToFix() < TTimeIntervalMicroSeconds(0))
        {
        _LIT(KError, "TimeToFix negative, TimeToFix = %d microsecs.");
        buf.Format(KError, I64LOW(iPSYInfo.TimeToFix().Int64()));
        AddTestResultL(buf, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "TimeToFix = %d microsecs.");
        buf.Format(KInfo, I64LOW(iPSYInfo.TimeToFix().Int64()));
        AddTestResultL(buf, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckVerticalAccuracyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckVerticalAccuracyL()
    {
    TBuf<KParameterResultBufSize> buf;
    if (iPSYInfo.VerticalAccuracy() < 0 || iPSYInfo.VerticalAccuracy() > iMaxVerticalAccuracy)
        {
        _LIT(KError, "VerticalAccuracy less than zero or greater than %f m, VerticalAccuracy = %f m.");
        buf.Format(KError, (TReal)iMaxVerticalAccuracy, iPSYInfo.VerticalAccuracy());
        AddTestResultL(buf, EErrorMessage);
        }
    else if (iPSYInfo.VerticalAccuracy() > iReasonableVerticalAccuracy)
        {
        _LIT(KWarning, "Bad accuracy, VerticalAccuracy = %f m.");
        buf.Format(KWarning, iPSYInfo.VerticalAccuracy());
        AddTestResultL(buf, EWarningMessage);
        }
    else
        {
        _LIT(KInfo, "VerticalAccuracy = %f m.");
        buf.Format(KInfo, iPSYInfo.VerticalAccuracy());
        AddTestResultL(buf, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckHorizontalAccuracyL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckHorizontalAccuracyL()
    {
    TBuf<KParameterResultBufSize> buf;
    if (iPSYInfo.HorizontalAccuracy() < 0 || 
		iPSYInfo.HorizontalAccuracy() > iMaxHorizontalAccuracy)
        {
        _LIT(KError, "HorizontalAccuracy less than zero or greater than %f m, HorizontalAccuracy = %f m.");
        buf.Format(KError, (TReal)iMaxHorizontalAccuracy, iPSYInfo.HorizontalAccuracy());
        AddTestResultL(buf, EErrorMessage);
        }
    else if (iPSYInfo.HorizontalAccuracy() > iReasonableHorizontalAccuracy)
        {
        _LIT(KWarning, "Bad accuracy, HorizontalAccuracy = %f m.");
		buf.Format(KWarning, iPSYInfo.HorizontalAccuracy());
        AddTestResultL(buf, EWarningMessage);
        }
    else
        {
        _LIT(KInfo, "HorizontalAccuracy = %f m.");
        buf.Format(KInfo, iPSYInfo.HorizontalAccuracy());
        AddTestResultL(buf, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckCostIndicatorL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckCostIndicatorL()
    {
    _LIT(KInfoUnknown, "CostIndicator = CostUnknown.");
    _LIT(KInfoZero, "CostIndicator = CostZero.");
    _LIT(KInfoPossible, "CostIndicator = CostPossible.");
    _LIT(KInfoCharge, "CostIndicator = CostCharge.");
    _LIT(KError, "CostIndicator illegal must be CostUnknown, CostZero, CostPossible or CostCharge.");
    
    switch (iPSYInfo.CostIndicator()) 
        {
        case TPositionQuality::ECostUnknown:
            AddTestResultL(KInfoUnknown, EInfoMessage);
            break;
        case TPositionQuality::ECostZero:
            AddTestResultL(KInfoZero, EInfoMessage);
            break;
        case TPositionQuality::ECostPossible:
            AddTestResultL(KInfoPossible, EInfoMessage);
            break;
        case TPositionQuality::ECostCharge:
            AddTestResultL(KInfoCharge, EInfoMessage);
            break;
        default:
            AddTestResultL(KError, EErrorMessage);
            break;
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckPowerConsumptionL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckPowerConsumptionL()
    {
    _LIT(KInfoUnknown, "PowerConsumption = PowerUnknown.");
    _LIT(KInfoZero, "PowerConsumption = PowerZero.");
    _LIT(KInfoLow, "PowerConsumption = PowerLow.");
    _LIT(KInfoMedium, "PowerConsumption = PowerMedium.");
    _LIT(KInfoHigh, "PowerConsumption = PowerHigh");
    _LIT(KError, "PowerConsumption illegal must be PowerUnknown, PowerZero, PowerLow, PowerMedium or PowerHigh.");
    
    switch (iPSYInfo.PowerConsumption()) 
        {
        case TPositionQuality::EPowerUnknown:
            AddTestResultL(KInfoUnknown, EInfoMessage);
            break;
        case TPositionQuality::EPowerZero:
            AddTestResultL(KInfoZero, EInfoMessage);
            break;
        case TPositionQuality::EPowerLow:
            AddTestResultL(KInfoLow, EInfoMessage);
            break;
        case TPositionQuality::EPowerMedium:
            AddTestResultL(KInfoMedium, EInfoMessage);
            break;
        case TPositionQuality::EPowerHigh:
            AddTestResultL(KInfoHigh, EInfoMessage);
            break;
        default:
            AddTestResultL(KError, EErrorMessage);
            break;
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckTechnologyTypeL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckTechnologyTypeL()
    {
    TBuf<KParameterResultBufSize> buf;
    if (iPSYInfo.TechnologyType() & TPositionModuleInfo::ETechnologyNetwork &&
		iPSYInfo.TechnologyType() & TPositionModuleInfo::ETechnologyTerminal)
        {
        _LIT(KError, "TechnologyType can't be TechnologyNetwork and TechnologyTerminal at the same time.");
        AddTestResultL(KError, EErrorMessage);
        }

    else if (!((iPSYInfo.TechnologyType() & TPositionModuleInfo::ETechnologyTerminal) ||
        (iPSYInfo.TechnologyType() & TPositionModuleInfo::ETechnologyNetwork)) &&
        iPSYInfo.TechnologyType() & TPositionModuleInfo::ETechnologyAssisted)
        {
        _LIT(KError, "TechnologyType can't be TechnologyAssisted only. Must be combination with either TechnologyTerminal or TechnologyNetwork.");
        AddTestResultL(KError, EErrorMessage);
        }
    
    else if (!(iPSYInfo.TechnologyType() == TPositionModuleInfo::ETechnologyUnknown) &&
        !(iPSYInfo.TechnologyType() & TPositionModuleInfo::ETechnologyTerminal) &&
        !(iPSYInfo.TechnologyType() & TPositionModuleInfo::ETechnologyNetwork) &&
        !(iPSYInfo.TechnologyType() & TPositionModuleInfo::ETechnologyAssisted))
        {
        _LIT(KError, "TechnologyType illegal must be TechnologyUnknown, TechnologyTerminal, TechnologyNetwork or combination with TechnologyAssisted, TechnologyType = %d.");
        buf.Format(KError, iPSYInfo.TechnologyType());
        AddTestResultL(buf, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "TechnologyType = %d.");
        buf.Format(KInfo, iPSYInfo.TechnologyType());
        AddTestResultL(buf, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckDeviceLocationL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckDeviceLocationL()
    {
    TBuf<KParameterResultBufSize> buf;
    _LIT(KInfoUnknown, "DeviceLocation = DeviceUnknown.");
    _LIT(KInfoInternal, "DeviceLocation = DeviceInternal.");
    _LIT(KInfoExternal, "DeviceLocation = DeviceExternal.");
    _LIT(KError, "DeviceLocation illegal must be DeviceUnknown, DeviceInternal or DeviceExternal: DeviceLocation = %d.");
    
    switch (iPSYInfo.DeviceLocation()) 
        {
        case TPositionModuleInfo::EDeviceUnknown:
            AddTestResultL(KInfoUnknown, EInfoMessage);
            break;
        case TPositionModuleInfo::EDeviceInternal:
            AddTestResultL(KInfoInternal, EInfoMessage);
            break;
        case TPositionModuleInfo::EDeviceExternal:
            AddTestResultL(KInfoExternal, EInfoMessage);
            break;
        default:
            buf.Format(KError, iPSYInfo.DeviceLocation());
            AddTestResultL(buf, EErrorMessage);
            break;
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckCapabilitiesL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckCapabilitiesL()
    {
    if (!iPSYInfo.Capabilities())
        {
        _LIT(KError, "No Capabilities supported, at least Horizontal Capability must be supported.");
        AddTestResultL(KError, EErrorMessage);
        }
    
    // Horizontal Capability check
    if (!(iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityHorizontal))
        {
        _LIT(KError, "PSY does not support Horizontal Capability.");  
		AddTestResultL(KError, EErrorMessage);
        }
    else
        {
        if (!(iPSYInfo.ClassesSupported() & EPositionInfoClass))
            {
            _LIT(KError, "PSY supports Horizontal Capability but doesn't support TPositionInfo class.");  
		    AddTestResultL(KError, EErrorMessage);
            }
        else
            {
            _LIT(KInfo, "Horizontal Capability supported as well as necessary TPositionInfo class.");
            AddTestResultL(KInfo, EInfoMessage);    
            }
        }

    // Vertical Capability check    
    if (!(iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityVertical))
        {
        _LIT(KInfo, "Vertical Capability not supported.");
        AddTestResultL(KInfo, EInfoMessage);    
        }
    else
        {
        if (!(iPSYInfo.ClassesSupported() & EPositionInfoClass))
            {
            _LIT(KError, "PSY supports Vertical Capability but doesn't support TPositionInfo class.");  
		    AddTestResultL(KError, EErrorMessage);
            }
        else
            {
            _LIT(KInfo, "Vertical Capability supported as well as necessary TPositionInfo class.");
            AddTestResultL(KInfo, EInfoMessage);    
            }
        }

    // Speed Capability check    
    if (!(iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilitySpeed))
        {
        _LIT(KInfo, "Speed Capability not supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    else
        {
        if (!(iPSYInfo.ClassesSupported() & EPositionCourseInfoClass))
            {
            _LIT(KError, "PSY supports Speed Capability but doesn't support TPositionCourseInfo class.");  
		    AddTestResultL(KError, EErrorMessage);
            }
        else
            {
            _LIT(KInfo, "Speed Capability supported as well as necessary TPositionCourseInfo class.");
            AddTestResultL(KInfo, EInfoMessage);    
            }
        }

    // Direction Capability check    
    if (!(iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityDirection))
        {
        _LIT(KInfo, "Direction Capability not supported.");
        AddTestResultL(KInfo, EInfoMessage);    
        }
    else
        {
        if (!(iPSYInfo.ClassesSupported() & EPositionCourseInfoClass))
            {
            _LIT(KError, "PSY supports Direction Capability but doesn't support TPositionCourseInfo class.");  
		    AddTestResultL(KError, EErrorMessage);
            }
        else
            {
            _LIT(KInfo, "Direction Capability supported as well as necessary TPositionCourseInfo class.");
            AddTestResultL(KInfo, EInfoMessage);    
            }
        }

    // Satellite Capability check    
    if (!(iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilitySatellite))
        {
        _LIT(KInfo, "Satellite Capability not supported.");
        AddTestResultL(KInfo, EInfoMessage);   
        }
    else
        {
        if (!(iPSYInfo.ClassesSupported() & EPositionSatelliteInfoClass))
            {
            _LIT(KError, "PSY supports Satellite Capability but doesn't support TPositionSatelliteInfo class.");  
		    AddTestResultL(KError, EErrorMessage);
            }
        else
            {
            _LIT(KInfo, "Satellite Capability supported as well as necessary TPositionSatelliteInfo class.");
            AddTestResultL(KInfo, EInfoMessage);    
            }
        
        }
    
    // Compass Capability check
    if (!(iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityCompass))
        {
        _LIT(KInfo, "Compass Capability not supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    else
        {
        _LIT(KInfo, "Compass Capability supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    
    // NMEA Capability check
    if (!(iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityNmea))
        {
        _LIT(KInfo, "NMEA Capability not supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    else
        {
        _LIT(KInfo, "NMEA Capability supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }

    // Address Capability check
    if (!(iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityAddress))
        {
        _LIT(KInfo, "Address Capability not supported");
        AddTestResultL(KInfo, EInfoMessage);
        }
    else
        {
        _LIT(KInfo, "Address Capability supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }

    // Building Capability check
    if (!(iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityBuilding))
        {
        _LIT(KInfo, "Building Capability not supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    else
        {
        _LIT(KInfo, "Building Capability supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }

    // Media Capability check
    if (!(iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityMedia))
        {
        _LIT(KInfo, "Media Capability not supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    else
        {
        _LIT(KInfo, "Media Capability supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckClassesSupportedL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckClassesSupportedL()
    {
    if (!(iPSYInfo.ClassesSupported() & EPositionInfoClass))
        {
        _LIT(KError, "TPositionInfo class must be supported.");
        AddTestResultL(KError, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "TPositionInfo class supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
	 
	if (!(iPSYInfo.ClassesSupported() & EPositionGenericInfoClass))
        {
        _LIT(KError, "HPositionGenericInfo class must be supported.");
        AddTestResultL(KError, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "HPositionGenericInfo class supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    
    if (!(iPSYInfo.ClassesSupported() & EPositionCourseInfoClass))
        {
        _LIT(KInfo, "TPositionCourseInfo class not supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    else
        {
        _LIT(KInfo, "TPositionCourseInfo class supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    
    if (!(iPSYInfo.ClassesSupported() & EPositionSatelliteInfoClass))
        {
        _LIT(KInfo, "TPositionSatelliteInfo class not supported.");
        AddTestResultL(KInfo, EInfoMessage);
        }
    else
        {
        if (!(iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilitySatellite))
            {
            _LIT(KWarning, "TPositionSatelliteInfo class supported but Satellite Capability not supported.");
            AddTestResultL(KWarning, EWarningMessage);   
            }
        else
            {
            _LIT(KInfo, "TPositionSatelliteInfo class supported as well as Satellite Capability.");
            AddTestResultL(KInfo, EInfoMessage);
            }
        }
    }

// ---------------------------------------------------------
// CPosPSYParameterTest::CheckVisibilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYParameterTest::CheckVisibilityL()
    {
    if(iPSYInfo.Visibility())
    {
	      
	    _LIT(KInfo, "PSY is visible.");
	    AddTestResultL(KInfo,EInfoMessage);
    }
    else
    {
    	_LIT(KInfo, "PSY is not visible");
    	AddTestResultL(KInfo, EInfoMessage);
    }
    }

//  End of File
