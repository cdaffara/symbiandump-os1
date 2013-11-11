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
#include "epos_cpospsycompasscapabilitytest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYCompassCapabilityTest::CPosPSYCompassCapabilityTest(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    BaseConstruct(aResults, aPositionArray, aPSYInfo);
    }

// Two-phased constructor.
CPosPSYCompassCapabilityTest* CPosPSYCompassCapabilityTest::NewLC(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    CPosPSYCompassCapabilityTest* self = 
        new (ELeave) CPosPSYCompassCapabilityTest(aResults, aPositionArray, aPSYInfo);
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CPosPSYCompassCapabilityTest::~CPosPSYCompassCapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYCompassCapabilityTest::TestCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCompassCapabilityTest::TestCapabilityL()
    {
    _LIT(KTests,"---- Testing Compass Capability ----");
    AddResultL(KTests, EDisplayMessage);

    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityCompass)
        {
        for (TInt i = 0; i < iPositionArray->Count(); i++)
            {
            TReal32 trueCourse = 0;
            TReal32 trueCourseError = 0;
            
            HPositionGenericInfo* genInfo = static_cast<HPositionGenericInfo*>((*iPositionArray)[i]);
			iNoneSet = ETrue;

            if (genInfo->GetValue(EPositionFieldTrueCourse, trueCourse) == KErrNotFound)
                {   
                if (genInfo->GetValue(EPositionFieldTrueCourseError, trueCourseError) == KErrNotFound)
                    {
                    iTrueCourseErrorNotSet++;
                    }
                else
                    {
                    iNoneSet = EFalse;
                    iTrueCourseCombinationError++;
                    }
                iTrueCourseNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                if (genInfo->GetValue(EPositionFieldTrueCourseError, trueCourseError) == KErrNotFound)
                    {
                    iTrueCourseErrorNotSet++;
                    }
                }

          if (iNoneSet)
                {
                iNoneSetError++;
                }
            }
        
        CheckResultsL();
        }
    else
        {
        _LIT(KASupportInfo, "Compass Capability not checked because capability not supported.");
        AddResultL(KASupportInfo, EInfoMessage);
        }
    } 

// ---------------------------------------------------------
// CPosPSYCompassCapabilityTest::CheckResultsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCompassCapabilityTest::CheckResultsL()
    {
    TBuf<KCapabilityBufSize> buf;
    
    if (!iNoneSetError &&
        !iTrueCourseNotSet &&
        !iTrueCourseErrorNotSet &&
        !iMagneticCourseNotSet &&
        !iMagneticCourseVariationNotSet &&
        !iOrientationNotSet &&
        !iOrientationErrorNotSet &&
        !iTrueCourseCombinationError &&
        !iMagneticCourseCombinationError &&
        !iOrientationCombinationError)
        {
        _LIT(KInfo,"Compass tests where completed successfully.");
        AddResultL(KInfo, EInfoMessage);
        }
    else
        {
        _LIT(KWarningNotSet, "Compass Capability supported but not all field set in all cases.");
        AddResultL(KWarningNotSet(), EWarningMessage);

        if (iNoneSetError > 0)
            {
            _LIT(KError,"Compass Capability supported but no fields set in %d of %d cases.");
            buf.Format(KError, iNoneSetError, iPositionArray->Count());
            AddResultL(buf, EErrorMessage);
            }
        if (iTrueCourseNotSet > 0)
            {
            _LIT(KWarning,"True Course not set in %d of %d cases.");
            buf.Format(KWarning, iTrueCourseNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iTrueCourseErrorNotSet > 0)
            {
            _LIT(KWarning,"True Course Error not set in %d of %d cases.");
            buf.Format(KWarning, iTrueCourseErrorNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iMagneticCourseNotSet > 0)
            {
            _LIT(KWarning,"Magnetic Course not set in %d of %d cases.");
            buf.Format(KWarning, iMagneticCourseNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iMagneticCourseVariationNotSet > 0)
            {
            _LIT(KWarning,"Magnetic Course Variation not set in %d of %d cases.");
            buf.Format(KWarning, iMagneticCourseVariationNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iOrientationNotSet > 0)
            {
            _LIT(KWarning,"Orientation not set in %d of %d cases.");
            buf.Format(KWarning, iOrientationNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iOrientationErrorNotSet > 0)
            {
            _LIT(KWarning,"Orientation Error not set in %d of %d cases.");
            buf.Format(KWarning, iOrientationErrorNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }

        // Combination checks
        if (iTrueCourseCombinationError > 0)
            {
            _LIT(KError,"True Course Error set but not True Course in %d of %d cases.");
            buf.Format(KError, iTrueCourseCombinationError, 
                Max(iTrueCourseCombinationError, (iPositionArray->Count() - iTrueCourseNotSet)));
            AddResultL(buf, EErrorMessage);
            }
        if (iMagneticCourseCombinationError > 0)
            {
            _LIT(KError,"Magnetic Course Variation set but not Magnetic Course in %d of %d cases.");
            buf.Format(KError, iMagneticCourseCombinationError, 
                Max(iMagneticCourseCombinationError, (iPositionArray->Count() - iMagneticCourseNotSet)));
            AddResultL(buf, EErrorMessage);
            }
        if (iOrientationCombinationError > 0)
            {
            _LIT(KError,"Orientation Error set but not Orientation in %d of %d cases.");
            buf.Format(KError, iOrientationCombinationError, 
                Max(iOrientationCombinationError, (iPositionArray->Count() - iOrientationNotSet)));
            AddResultL(buf, EErrorMessage);
            }
        }
    }       

//  End of File
