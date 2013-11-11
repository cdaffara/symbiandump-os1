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
#include "epos_cpospsyspeedcapabilitytest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYSpeedCapabilityTest::CPosPSYSpeedCapabilityTest(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    BaseConstruct(aResults, aPositionArray, aPSYInfo);
    }

// Two-phased constructor.
CPosPSYSpeedCapabilityTest* CPosPSYSpeedCapabilityTest::NewLC(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    CPosPSYSpeedCapabilityTest* self = 
        new (ELeave) CPosPSYSpeedCapabilityTest(aResults, aPositionArray, aPSYInfo);
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CPosPSYSpeedCapabilityTest::~CPosPSYSpeedCapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYSpeedCapabilityTest::TestCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYSpeedCapabilityTest::TestCapabilityL()
    {
    if (iPositionArray->Count())
        {    
        TestCapabilityForCourseInfoL();
        CheckResultsL();
        TestCapabilityForGenericInfoL();
        CheckResultsL();
        }
    }

// ---------------------------------------------------------
// CPosPSYSpeedCapabilityTest::CheckResultsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYSpeedCapabilityTest::CheckResultsL()
    {
    TBuf<KCapabilityBufSize> buf;
    TPositionInfo* position = (*iPositionArray)[0];

    if (!iHorizontalSpeedNotSet &&
        !iHorizontalSpeedErrorNotSet &&
        !iVerticalSpeedNotSet &&
        !iVerticalSpeedErrorNotSet &&
        !iVSpeedLessThanZero &&
        !iVSpeedErrorLessThanZero &&
        !iHSpeedLessThanZero &&
        !iHSpeedErrorLessThanZero &&
        !iHorizontalCombinationError &&
        !iVerticalCombinationError &&
        iTestingPerformed)
        {
        _LIT(KInfo,"Speed capability test was succesfully completed.");
        AddResultL(KInfo, EInfoMessage);
        }
    else
        {
        if (iHorizontalSpeedNotSet > 0)
            {
            _LIT(KError,"Horizontal speed not set in %d of %d cases.");
            buf.Format(KError, iHorizontalSpeedNotSet, iPositionArray->Count());
            AddResultL(buf, EErrorMessage);
            }
        if (iHorizontalSpeedErrorNotSet > 0)
            {
            _LIT(KWarning,"Horizontal speed error not set in %d of %d cases.");
            buf.Format(KWarning, iHorizontalSpeedErrorNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }        
        if (iHSpeedLessThanZero > 0)
            {
            _LIT(KError,"Horizontal speed less than zero in %d of %d cases.");
            buf.Format(KError, iHSpeedLessThanZero, (iPositionArray->Count()-iHorizontalSpeedNotSet));
            AddResultL(buf, EErrorMessage);
            }
        if (iHSpeedErrorLessThanZero > 0)
            {
            _LIT(KError,"Horizontal speed error less than zero in %d of %d cases.");
            buf.Format(KError, iHSpeedErrorLessThanZero, (iPositionArray->Count()-iHorizontalSpeedNotSet));
            AddResultL(buf, EErrorMessage);
            }

        // Combiantion checks
        if (iHorizontalCombinationError > 0)
            {
            _LIT(KError,"Horizontal speed error set but not Horizontal speed in %d of %d cases.");
            buf.Format(KError, iHorizontalCombinationError, (iPositionArray->Count()-iHorizontalSpeedNotSet));
            AddResultL(buf, EErrorMessage);
            }

        if (position->PositionClassType() & EPositionGenericInfoClass)
            {
            if (iVerticalSpeedNotSet > 0)
                {
                _LIT(KWarning,"Vertical speed not set in %d of %d cases.");
                buf.Format(KWarning, iVerticalSpeedNotSet, iPositionArray->Count());
                AddResultL(buf, EWarningMessage);
                }
            if (iVerticalSpeedErrorNotSet > 0)
                {
                _LIT(KWarning,"Vertical speed error not set in %d of %d cases.");
                buf.Format(KWarning, iVerticalSpeedErrorNotSet, iPositionArray->Count());
                AddResultL(buf, EWarningMessage);
                }     
            if (iVSpeedLessThanZero > 0)
                {
                _LIT(KError,"Vertical speed less than zero in %d of %d cases.");
                buf.Format(KError, iVSpeedLessThanZero, (iPositionArray->Count()-iVerticalSpeedNotSet));
                AddResultL(buf, EErrorMessage);
                }
            if (iVSpeedErrorLessThanZero > 0)
                {
                _LIT(KError,"Vertical speed error less than zero in %d of %d cases.");
                buf.Format(KError, iVSpeedErrorLessThanZero, (iPositionArray->Count()-iVerticalSpeedNotSet));
                AddResultL(buf, EErrorMessage);
                }
            if (iVerticalCombinationError > 0)
                {
                _LIT(KError,"Vertical speed error set but not Vertical speed in %d of %d cases.");
                buf.Format(KError, iVerticalCombinationError, (iPositionArray->Count()-iVerticalSpeedNotSet));
                AddResultL(buf, EErrorMessage);
                }
            }
        }

    ResetResults();
    }

// ---------------------------------------------------------
// CPosPSYSpeedCapabilityTest::ResetResults
//
// (other items were commented in a header).
// ---------------------------------------------------------
//   
void CPosPSYSpeedCapabilityTest::ResetResults()
    {
    iHorizontalSpeedNotSet = 0;
    iHorizontalSpeedErrorNotSet = 0;
    iVerticalSpeedNotSet = 0;
    iVerticalSpeedErrorNotSet = 0;
    iVSpeedLessThanZero = 0;
    iVSpeedErrorLessThanZero = 0;
    iHSpeedLessThanZero = 0;
    iHSpeedErrorLessThanZero = 0;
    iHorizontalCombinationError = 0;
    iVerticalCombinationError = 0;
    iTestingPerformed = EFalse;
    }

// ---------------------------------------------------------
// CPosPSYSpeedCapabilityTest::TestCapabilityForCourseInfoL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYSpeedCapabilityTest::TestCapabilityForCourseInfoL()
    {
	if ((*iPositionArray)[0]->PositionClassType() & EPositionCourseInfoClass)
        {
        _LIT(KInfo, "--- Testing Speed Capability ---");
        AddResultL(KInfo, EDisplayMessage);

        for (TInt i = 0; i < iPositionArray->Count(); i++)
            {
            iTestingPerformed = ETrue;
            TCourse courseInfo;                
            static_cast<TPositionCourseInfo*>((*iPositionArray)[i])->GetCourse(courseInfo);
            TRealX speed(courseInfo.Speed());
            TRealX speedAccuracy(courseInfo.SpeedAccuracy());

            if (speed.IsNaN())
                {   
                if (speedAccuracy.IsNaN())
                    {
                    iHorizontalSpeedErrorNotSet++;
                    }
                else
                    {
                    iHorizontalCombinationError++;
                    }
                iHorizontalSpeedNotSet++;
                }
            else
                {
                if (speedAccuracy.IsNaN())
                    {
                    iHorizontalSpeedErrorNotSet++;
                    }
                if ((static_cast<TReal32> (speed)) < 0)
                    {
                    iHSpeedLessThanZero++;
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// CPosPSYSpeedCapabilityTest::TestCapabilityForGenericInfoL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYSpeedCapabilityTest::TestCapabilityForGenericInfoL()
    {    
	if ((*iPositionArray)[0]->PositionClassType() & EPositionGenericInfoClass)
		{
		_LIT(KInfo, "--- Testing Speed Capability with Generic Info ---");
		AddResultL(KInfo,EDisplayMessage);
		if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilitySpeed)
			{
			for (TInt i = 0; i < iPositionArray->Count(); i++)
				{
				iTestingPerformed = ETrue;
				TReal32 hSpeed = 0;
				TReal32 hSpeedError = 0;
				TReal32 vSpeed = 0;
				TReal32 vSpeedError = 0;

				HPositionGenericInfo* genInfo = static_cast<HPositionGenericInfo*> ((*iPositionArray)[i]);
    
				//Horizontal check
				if (genInfo->GetValue(EPositionFieldHorizontalSpeed, hSpeed) == KErrNotFound)
					{   
					if (genInfo->GetValue(EPositionFieldHorizontalSpeedError, hSpeedError) == KErrNotFound)
						{
						iHorizontalSpeedErrorNotSet++;
						}
					else
						{
						if (hSpeedError < 0)
							{
							iHSpeedErrorLessThanZero++;
							}
						iHorizontalCombinationError++;
						}
					iHorizontalSpeedNotSet++;
					}
				else
					{
					if (genInfo->GetValue(EPositionFieldHorizontalSpeedError, hSpeedError) == KErrNotFound)
						{
						iHorizontalSpeedErrorNotSet++;
						}
					else
						{
						if (hSpeedError < 0)
							{
							iHSpeedErrorLessThanZero++;
							}
						}
					if (hSpeed < 0)
						{
						iHSpeedLessThanZero++;
						}
					}

				//Vertical check    
				if (genInfo->GetValue(EPositionFieldVerticalSpeed, vSpeed) == KErrNotFound)
					{   
					if (genInfo->GetValue(EPositionFieldVerticalSpeedError, vSpeedError) == KErrNotFound)
						{
						iVerticalSpeedErrorNotSet++;
						}
					else
						{
						if (vSpeedError < 0)
							{
							iVSpeedErrorLessThanZero++;
							}
						iVerticalCombinationError++;
						}
					iVerticalSpeedNotSet++;
					}
				else
					{
					if (genInfo->GetValue(EPositionFieldVerticalSpeedError, vSpeedError) == KErrNotFound)
						{
						iVerticalSpeedErrorNotSet++;
						}
					else
						{
						if (vSpeedError < 0)
							{
							iVSpeedErrorLessThanZero++;
							}
						}
					if (vSpeed < 0)
						{
						iVSpeedLessThanZero++;
						}
					}

				}
			}
		else
			{
			_LIT(KNotSupported, "Speed capability not checked because capability not supported.");
			AddResultL(KNotSupported, EInfoMessage);
			}
		}
    }

//  End of File
