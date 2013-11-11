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
#include <e32math.h>
#include <lbssatellite.h>
#include "epos_cpospsyhorizontalcapabilitytest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYHorizontalCapabilityTest::CPosPSYHorizontalCapabilityTest(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo,
	const TPosPSYTestParameters& aTestParameters)
    {
    BaseConstruct(aResults, aPositionArray, aPSYInfo);
	iHorizontalAccuracyThreshold = aTestParameters.HorizontalAccuracyThreshold();
    }

// Two-phased constructor.
CPosPSYHorizontalCapabilityTest* CPosPSYHorizontalCapabilityTest::NewLC(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo,
	const TPosPSYTestParameters& aTestParameters)
    {
    CPosPSYHorizontalCapabilityTest* self = 
        new (ELeave) CPosPSYHorizontalCapabilityTest(aResults, 
													 aPositionArray, 
													 aPSYInfo, 
													 aTestParameters);
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CPosPSYHorizontalCapabilityTest::~CPosPSYHorizontalCapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYHorizontalCapabilityTest::TestCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYHorizontalCapabilityTest::TestCapabilityL()
    {
    _LIT(KTests, "---- Testing Horizontal Capability ----");
    AddResultL(KTests, EDisplayMessage);

    if ((iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityHorizontal) &&
        iPositionArray->Count() > 0)
        {
        TestCapabilityForPositionInfoL();
        CheckResultsL();
        }
    else
        {
        _LIT(KASupportInfo, "Horizontal Capability not checked because capability not supported.");
        AddResultL(KASupportInfo, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYHorizontalCapabilityTest::CheckResultsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYHorizontalCapabilityTest::CheckResultsL()
    {
    TBuf<KCapabilityBufSize> buf;
    
	if (iAccuracyError > (iPositionArray->Count() / iHorizontalAccuracyThreshold))
        {
        _LIT(KError, "More than 1/%d of horizontal values outside specified accuracy interval. Number of values outside interval: %d of %d.");
        buf.Format(KError, 
				   iHorizontalAccuracyThreshold, 
				   iAccuracyError, 
				   iPositionArray->Count());
        AddResultL(buf, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "1/%d or less of horizontal values outside specified accuracy interval. Number of values outside interval: %d of %d.");
        buf.Format(KInfo, 
				   iHorizontalAccuracyThreshold,
				   iAccuracyError, 
				   iPositionArray->Count());
        AddResultL(buf, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYHorizontalCapabilityTest::TestCapabilityForPositionInfoL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYHorizontalCapabilityTest::TestCapabilityForPositionInfoL()
    {
    TPosition pos;
    
	for (TInt i = 0; i < iPositionArray->Count(); i++)
        {
		(*iPositionArray)[i]->GetPosition(pos);
		if (!Math::IsNaN(pos.HorizontalAccuracy()))
			{
			if (pos.HorizontalAccuracy() > iPSYInfo.HorizontalAccuracy())
				{
				iAccuracyError++;
				}
			}
		else
			{
			iAccuracyError++;
			}
        }
    }

//  End of File
