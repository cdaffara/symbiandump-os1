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
#include <lbssatellite.h>
#include "epos_cpospsydirectioncapabilitytest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code, that
// might leave.
//
CPosPSYDirectionCapabilityTest::CPosPSYDirectionCapabilityTest(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    BaseConstruct(aResults, aPositionArray, aPSYInfo);
    }

// Two-phased constructor.
CPosPSYDirectionCapabilityTest* CPosPSYDirectionCapabilityTest::NewLC(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    CPosPSYDirectionCapabilityTest* self = 
        new (ELeave) CPosPSYDirectionCapabilityTest(aResults, aPositionArray, aPSYInfo);
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CPosPSYDirectionCapabilityTest::~CPosPSYDirectionCapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYDirectionCapabilityTest::TestCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYDirectionCapabilityTest::TestCapabilityL()
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
// CPosPSYDirectionCapabilityTest::CheckResultsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYDirectionCapabilityTest::CheckResultsL()
    {
    TBuf<KCapabilityBufSize> buf;
    
    if (!iHeadingNotSet &&
        !iHeadingErrorNotSet &&
        !iHeadingCombinationError &&
        iTestingPerformed)
        {
        _LIT(KInfo,"Direction tests where completed successfully.");
        AddResultL(KInfo, EInfoMessage);
        }
    else
        {
        // Not set checks
        if (iHeadingNotSet > 0)
            {
            _LIT(KError,"Heading not set in %d of %d cases.");
            buf.Format(KError, iHeadingNotSet, iPositionArray->Count());
            AddResultL(buf, EErrorMessage);
            }
        if (iHeadingErrorNotSet > 0)
            {
            _LIT(KWarning,"Heading Error not set in %d of %d cases.");
            buf.Format(KWarning, iHeadingErrorNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        // Combination checks
        if (iHeadingCombinationError > 0)
            {
            _LIT(KError,"Heading Error set but not Heading in %d of %d cases.");
            buf.Format(KError, iHeadingCombinationError, (iPositionArray->Count()-iHeadingNotSet));
            AddResultL(buf, EErrorMessage);
            }
        }
    
    ResetResults();
    }

// ---------------------------------------------------------
// CPosPSYDirectionCapabilityTest::ResetResults
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYDirectionCapabilityTest::ResetResults()
    {
    iHeadingNotSet = 0;
    iHeadingErrorNotSet = 0;
    iHeadingCombinationError = 0;
    iTestingPerformed = EFalse;
    }

// ---------------------------------------------------------
// CPosPSYDirectionCapabilityTest::TestCapabilityForCourseInfoL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYDirectionCapabilityTest::TestCapabilityForCourseInfoL()
    {
    if ((*iPositionArray)[0]->PositionClassType() & EPositionCourseInfoClass)
        {   
        _LIT(KInfo,"---- Testing Direction Capability ----");
        AddResultL(KInfo, EDisplayMessage);
        for (TInt i = 0; i < iPositionArray->Count(); i++)
            {
            TCourse courseInfo;
            static_cast<TPositionCourseInfo*>((*iPositionArray)[i])->GetCourse(courseInfo);        
            TRealX heading (courseInfo.Heading());
            TRealX headingError (courseInfo.HeadingAccuracy());
            iTestingPerformed = ETrue;

            if (heading.IsNaN())
                {   
                if (headingError.IsNaN())
                    {
                    iHeadingErrorNotSet++;
                    }
                else
                    {
                    iHeadingCombinationError++;
                    }
                iHeadingNotSet++;
                }
            else
                {
                if (headingError.IsNaN())
                    {
                    iHeadingErrorNotSet++;
                    }
                }
            } 
        }
    }

// ---------------------------------------------------------
// CPosPSYDirectionCapabilityTest::TestCapabilityForGenericInfoL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYDirectionCapabilityTest::TestCapabilityForGenericInfoL()
    {
    if ((*iPositionArray)[0]->PositionClassType() & EPositionGenericInfoClass)
		{
		_LIT(KInfo,"---- Testing Direction Capability for Generic Info----");
		AddResultL(KInfo, EDisplayMessage);
		if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityDirection)
			{
			for (TInt i = 0; i < iPositionArray->Count(); i++)
				{ 
				HPositionGenericInfo* genInfo = static_cast<HPositionGenericInfo*> ((*iPositionArray)[i]);
				TReal32 heading = 0;
				TReal32 headingError = 0;
				iTestingPerformed = ETrue;
            
				if (genInfo->GetValue(EPositionFieldHeading, heading) == KErrNotFound)
					{   
					if (genInfo->GetValue(EPositionFieldHeadingError, headingError) == KErrNotFound)
						{
						iHeadingErrorNotSet++;
						}
					else
						{
						iHeadingCombinationError++;
						}
					iHeadingNotSet++;
					}
				else
					{
					if (genInfo->GetValue(EPositionFieldHeadingError, headingError) == KErrNotFound)
						{
						iHeadingErrorNotSet++;
						}         
					}
				}
			}
		 else
			{
			_LIT(KNotSupported, "Direction capability not checked because capability not supported.");
			AddResultL(KNotSupported, EInfoMessage);
			}
		}
    }

//  End of File
