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
#include <lbspositioninfo.h>
#include <lbssatellite.h>
#include "epos_cpospsysatellitecapabilitytest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYSatelliteCapabilityTest::CPosPSYSatelliteCapabilityTest(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    BaseConstruct(aResults, aPositionArray, aPSYInfo);
    }

// Two-phased constructor.
CPosPSYSatelliteCapabilityTest* CPosPSYSatelliteCapabilityTest::NewLC(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    CPosPSYSatelliteCapabilityTest* self = 
        new (ELeave) CPosPSYSatelliteCapabilityTest(aResults, aPositionArray, aPSYInfo);
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CPosPSYSatelliteCapabilityTest::~CPosPSYSatelliteCapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYSatelliteCapabilityTest::CheckResultsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYSatelliteCapabilityTest::CheckResultsL()
    {
    TBuf<KCapabilityBufSize> buf;
    TPositionInfo* position = (*iPositionArray)[0];
    
    if (!iNumInViewNotSet &&
        !iNumInViewLessThanZero &&
        !iNumInUseNotSet &&
        !iNumInUseLessThanZero &&
        !iNumInUseGreaterThanNumInView &&
        !iTimeNotSet &&
        !iTimeLessThanZero &&
        !iHorizontalDoPNotSet &&
        !iVerticalDoPNotSet &&
        !iTimeDoPNotSet &&
        !iWgs84AltitudeNotSet &&
        !iGeoidalSeparationNotSet &&
        !iPositionDoPNotSet &&
		!iNrOfUsedSatellitesMissmatch &&
		!iNumOfSatelliteIdNotSet &&
		!iNumOfAzimuthNotSet &&
		!iNumOfElevationNotSet &&
		!iNumOfSignalStrengthNotSet &&
        iTestingPerformed)
        {
        _LIT(KInfo,"Satellite tests where completed successfully.");
        AddResultL(KInfo, EInfoMessage);
        }
	else
		{
		if (iNumInViewNotSet > 0)
			{
			_LIT(KError,"Number of Satellites in view must be set when Satellite Capability supported, not set in %d of %d cases.");
			buf.Format(KError, iNumInViewNotSet, iPositionArray->Count());
			AddResultL(buf, EErrorMessage);
			}
		if (iNumInViewLessThanZero > 0)
			{
			_LIT(KError,"Num of Satellites in view less than zero in %d of %d cases.");
			buf.Format(KError, iNumInViewLessThanZero, iPositionArray->Count());
			AddResultL(buf, EErrorMessage);
			}
		if (iNumInUseNotSet > 0)
			{
			_LIT(KWarning,"Number of Satellites in use, not set in %d of %d cases.");
			buf.Format(KWarning, iNumInUseNotSet, iPositionArray->Count());
			AddResultL(buf, EWarningMessage);
			}
		if (iNumInUseLessThanZero > 0)
			{
			_LIT(KError,"Number of Satellites in use less than zero in %d of %d cases.");
			buf.Format(KError, iNumInUseLessThanZero, iPositionArray->Count());
			AddResultL(buf, EErrorMessage);
			}
		if (iNumInUseGreaterThanNumInView > 0)
			{
			_LIT(KError,"Number of Satellites in use greater than Number of Satellites in view %d of %d cases.");
			buf.Format(KError, iNumInUseGreaterThanNumInView, iPositionArray->Count());
			AddResultL(buf, EErrorMessage);
			}
		if (iTimeNotSet > 0)
			{
			_LIT(KWarning,"Satellite Time not set in %d of %d cases.");
			buf.Format(KWarning, iTimeNotSet, iPositionArray->Count());
			AddResultL(buf, EWarningMessage);
			}
		if (iTimeLessThanZero > 0)
			{
			_LIT(KError,"Satellite Time less than zero in %d of %d cases.");
			buf.Format(KError, iTimeLessThanZero, iPositionArray->Count());
			AddResultL(buf, EWarningMessage);
			}
		if (iHorizontalDoPNotSet > 0)
			{
			_LIT(KWarning,"Satellite Horizontal DoP not set in %d of %d cases.");
			buf.Format(KWarning, iHorizontalDoPNotSet, iPositionArray->Count());
			AddResultL(buf, EWarningMessage);
			}
		if (iVerticalDoPNotSet > 0)
			{
			_LIT(KWarning,"Satellite Vertical DoP not set in %d of %d cases.");
			buf.Format(KWarning, iVerticalDoPNotSet, iPositionArray->Count());
			AddResultL(buf, EWarningMessage);
			}
		if (iTimeDoPNotSet > 0)
			{   
			_LIT(KWarning,"Satellite Time DoP not set in %d of %d cases.");
			buf.Format(KWarning, iTimeDoPNotSet, iPositionArray->Count());
			AddResultL(buf, EWarningMessage);
			}
		if (iNrOfUsedSatellitesMissmatch > 0)
			{
			_LIT(KWarning,"Number of used satellites in satellite data does not match with number in satellite info in %d of %d cases.");
			buf.Format(KWarning, iNrOfUsedSatellitesMissmatch, iPositionArray->Count() - iNumInViewNotSet);
			AddResultL(buf, EWarningMessage);
			}
		if (iNumOfSatelliteIdNotSet > 0)
			{
			_LIT(KWarning,"Satellite Id not set for all satellites in satellite data in %d of %d cases.");
			buf.Format(KWarning, iNumOfSatelliteIdNotSet, iPositionArray->Count() - iNumInViewNotSet);
			AddResultL(buf, EWarningMessage);
			}
		if (iNumOfAzimuthNotSet > 0)
			{
			_LIT(KWarning,"Azimuth not set for all satellites in satellite data in %d of %d cases.");
			buf.Format(KWarning, iNumOfAzimuthNotSet, iPositionArray->Count() - iNumInViewNotSet);
			AddResultL(buf, EWarningMessage);
			}
		if (iNumOfElevationNotSet > 0)
			{
			_LIT(KWarning,"Elevation not set for all satellites in satellite data in %d of %d cases.");
			buf.Format(KWarning, iNumOfElevationNotSet, iPositionArray->Count() - iNumInViewNotSet);
			AddResultL(buf, EWarningMessage);
			}
		if (iNumOfSignalStrengthNotSet > 0)
			{
			_LIT(KWarning,"Signal strength not set for all satellites in satellite data in %d of %d cases.");
			buf.Format(KWarning, iNumOfSignalStrengthNotSet, iPositionArray->Count() - iNumInViewNotSet);
			AddResultL(buf, EWarningMessage);
			}
		if (position->PositionClassType() & EPositionGenericInfoClass)
			{
			if (iWgs84AltitudeNotSet > 0)
				{
				_LIT(KWarning,"Satellite WGS84 Altitude not set in %d of %d cases.");
				buf.Format(KWarning, iWgs84AltitudeNotSet, iPositionArray->Count());
				AddResultL(buf, EWarningMessage);
				}
			if (iGeoidalSeparationNotSet > 0)
				{
				_LIT(KWarning,"Satellite Geodial Separation not set in %d of %d cases.");
				buf.Format(KWarning, iGeoidalSeparationNotSet, iPositionArray->Count());
				AddResultL(buf, EWarningMessage);
				}
			if (iPositionDoPNotSet > 0)
				{
				_LIT(KWarning,"Satellite Position DoP not set in %d of %d cases.");
				buf.Format(KWarning, iPositionDoPNotSet, iPositionArray->Count());
				AddResultL(buf, EWarningMessage);
				}
			}
		}

    ResetResults();
    }

// ---------------------------------------------------------
// CPosPSYSatelliteCapabilityTest::CheckSatelliteDataL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYSatelliteCapabilityTest::CheckSatelliteDataL(
	const TPositionSatelliteInfo& satelliteInfo,
	TInt nrOfSatellites)
	{
	TInt nrOfUsedSatellites = 0;
	
	TBool usedSatellitesMissmatch = EFalse;
	TBool satelliteIdNotSet = EFalse;
	TBool signalStrengthNotSet = EFalse;
	TBool azimuthNotSet = EFalse;
	TBool elevationNotSet= EFalse;

	for (TInt i = 0; i < nrOfSatellites; i++)
		{
		TSatelliteData satelliteData;
		satelliteInfo.GetSatelliteData(i,satelliteData);
		
		if (satelliteData.IsUsed())
			{
			nrOfUsedSatellites++;
			}
		
		if (satelliteData.SatelliteId() == -1) //unable to determine identity, default value
			{
			satelliteIdNotSet = ETrue;
			}

		if (satelliteData.SignalStrength() == 0) //default value
			{
			signalStrengthNotSet = ETrue;
			}
		
		TRealX aizmuth = TRealX(satelliteData.Azimuth());
        if (aizmuth.IsNaN())
			{
			azimuthNotSet = ETrue;
			}
        
		TRealX elevation = TRealX(satelliteData.Elevation());
        if (elevation.IsNaN())
			{
			elevationNotSet = ETrue;
			}
		}
	
	if (nrOfUsedSatellites != satelliteInfo.NumSatellitesUsed())
		{
		usedSatellitesMissmatch = ETrue;
		}

	if (satelliteIdNotSet)
		{
		iNumOfSatelliteIdNotSet++;
		}
	if (signalStrengthNotSet)
		{
		iNumOfSignalStrengthNotSet++;
		}
	if (azimuthNotSet)
		{
		iNumOfAzimuthNotSet++;
		}
	if (elevationNotSet)
		{
		iNumOfElevationNotSet++;
		}
	if (usedSatellitesMissmatch)
		{
		iNrOfUsedSatellitesMissmatch++;
		}
	}


// ---------------------------------------------------------
// CPosPSYSatelliteCapabilityTest::ResetResults
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYSatelliteCapabilityTest::ResetResults()
    {    
    iNumInViewNotSet = 0;
    iNumInViewLessThanZero = 0;
    iNumInUseNotSet = 0;
    iNumInUseLessThanZero = 0;
    iNumInUseGreaterThanNumInView = 0;
    iTimeNotSet = 0;
    iTimeLessThanZero = 0;
    iHorizontalDoPNotSet = 0;
    iVerticalDoPNotSet = 0;
    iTimeDoPNotSet = 0;
    iPositionDoPNotSet = 0;
    iWgs84AltitudeNotSet = 0;
    iGeoidalSeparationNotSet = 0;
	iNrOfUsedSatellitesMissmatch = 0;
	iNumOfSatelliteIdNotSet = 0;
	iNumOfAzimuthNotSet = 0;
	iNumOfElevationNotSet = 0;
	iNumOfSignalStrengthNotSet = 0;
    iTestingPerformed = EFalse;
    }

// ---------------------------------------------------------
// CPosPSYSatelliteCapabilityTest::TestCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYSatelliteCapabilityTest::TestCapabilityL()
    {
    if (iPositionArray->Count())
        {    
        TestCapabilityForSatelliteInfoL();
        CheckResultsL();
        TestCapabilityForGenericInfoL();
        CheckResultsL();
        }
    }

// ---------------------------------------------------------
// CPosPSYSatelliteCapabilityTest::TestCapabilityForGenericInfoL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYSatelliteCapabilityTest::TestCapabilityForGenericInfoL()
    {
    if ((*iPositionArray)[0]->PositionClassType() & EPositionGenericInfoClass)
        {
		_LIT(KInfo, "--- Testing Satellite Capability with Generic Info ---");
        AddResultL(KInfo,EDisplayMessage);
		if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilitySatellite)
			{
        
			for (TInt i = 0; i < iPositionArray->Count(); i++)
				{
				iTestingPerformed = ETrue;
				TBool numInViewSet = EFalse;
				TInt8 numInView = 0;
				TInt8 numInUse = 0;
				TTime time;
				TReal32 horizontalDoP = 0;
				TReal32 verticalDoP = 0;
				TReal32 timeDop = 0;
				TReal32 positionDoP = 0;
				TReal32 wgs84Altitude = 0;
				TReal32 geoidalSeparation = 0;
    
				HPositionGenericInfo* genInfo = static_cast<HPositionGenericInfo*> ((*iPositionArray)[i]);

				if (genInfo->GetValue(EPositionFieldSatelliteNumInView, numInView) == KErrNotFound)
					{   
					iNumInViewNotSet++;
					}
				else
					{
					numInViewSet = ETrue;
					if (numInView < 0)
						{
						iNumInViewLessThanZero++;
						}
					}
				if (genInfo->GetValue(EPositionFieldSatelliteNumUsed, numInUse) == KErrNotFound)
					{   
					iNumInUseNotSet++;
					}
				else
					{
					if (numInViewSet && (numInView < numInUse))
						{
						iNumInUseGreaterThanNumInView++;
						}
					if (numInUse < 0)
						{
						iNumInUseLessThanZero++;
						}
					}
				if (genInfo->GetValue(EPositionFieldSatelliteTime, time) == KErrNotFound)
					{   
					iTimeNotSet++;
					}
				else
					{
					if (time.Int64() < 0)
						{
						iTimeLessThanZero++;
						}
					}
				if (genInfo->GetValue(EPositionFieldSatelliteHorizontalDoP, horizontalDoP) == KErrNotFound)
					{   
					iHorizontalDoPNotSet++;
					}
				if (genInfo->GetValue(EPositionFieldSatelliteVerticalDoP, verticalDoP) == KErrNotFound)
					{   
					iVerticalDoPNotSet++;
					}
				if (genInfo->GetValue(EPositionFieldSatelliteTimeDoP, timeDop) == KErrNotFound)
					{   
					iTimeDoPNotSet++;
					}
				if (genInfo->GetValue(EPositionFieldSatellitePositionDoP, positionDoP) == KErrNotFound)
					{   
					iPositionDoPNotSet++;
					}
				if (genInfo->GetValue(EPositionFieldSatelliteSeaLevelAltitude, wgs84Altitude) == KErrNotFound)
					{   
					iWgs84AltitudeNotSet++;
					}
				if (genInfo->GetValue(EPositionFieldSatelliteGeoidalSeparation, geoidalSeparation) == KErrNotFound)
					{   
					iGeoidalSeparationNotSet++;
					}
				} 
			}
		else
			{
			_LIT(KNotSupported, "Satellite capability not checked because capability not supported.");
			AddResultL(KNotSupported, EInfoMessage);
			}
		}
    }

// ---------------------------------------------------------
// CPosPSYSatelliteCapabilityTest::TestCapabilityForSatelliteInfoL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYSatelliteCapabilityTest::TestCapabilityForSatelliteInfoL()
    {    
	if ((*iPositionArray)[0]->PositionClassType() & EPositionSatelliteInfoClass)
        {
        _LIT(KInfo, "--- Testing Satellite Capability with Satellite Info ---");
        AddResultL(KInfo,EDisplayMessage);
        for (TInt i = 0; i < iPositionArray->Count(); i++)
            {
            iTestingPerformed = ETrue;
            TBool numInViewSet = EFalse;
            TPositionSatelliteInfo* satelliteInfo = static_cast<TPositionSatelliteInfo*>((*iPositionArray)[i]);

            if (satelliteInfo->NumSatellitesInView() == 0)
                {   
                iNumInViewNotSet++;
                }
            else
                {
                numInViewSet = ETrue;
                if (satelliteInfo->NumSatellitesInView() < 0)
                    {
                    iNumInViewLessThanZero++;
                    }
				else
					{
					CheckSatelliteDataL(*satelliteInfo, satelliteInfo->NumSatellitesInView());
					}
				}
                
			if (satelliteInfo->NumSatellitesUsed() == 0)
                {   
                iNumInUseNotSet++;
                }
			else
                {
                if (numInViewSet && (satelliteInfo->NumSatellitesInView() < satelliteInfo->NumSatellitesUsed()))
                    {
                    iNumInUseGreaterThanNumInView++;
                    }
                if (satelliteInfo->NumSatellitesUsed() < 0)
                    {
                    iNumInUseLessThanZero++;
                    }
				}    
            TRealX horizontalDoP = TRealX(satelliteInfo->HorizontalDoP());
            if (horizontalDoP.IsNaN())
                {   
                iHorizontalDoPNotSet++;
                }

            TRealX verticalDoP = TRealX(satelliteInfo->VerticalDoP());
            if (verticalDoP.IsNaN())
                {   
                iVerticalDoPNotSet++;
                }

            TRealX timeDoP = TRealX(satelliteInfo->TimeDoP());
            if (timeDoP.IsNaN())
				{   
				iTimeDoPNotSet++;
				}

            if (satelliteInfo->SatelliteTime().Int64() == 0)
                {   
                iTimeNotSet++;
                }
            else
                {
                if (satelliteInfo->SatelliteTime().Int64() < 0)
                    {
                    iTimeLessThanZero++;

                    }
                }
			}
		}
    }

//  End of File
