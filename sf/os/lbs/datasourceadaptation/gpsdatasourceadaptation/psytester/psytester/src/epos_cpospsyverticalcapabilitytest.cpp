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
#include "epos_cpospsyverticalcapabilitytest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYVerticalCapabilityTest::CPosPSYVerticalCapabilityTest(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo,
	const TPosPSYTestParameters& aTestParameters)
    {
    BaseConstruct(aResults, aPositionArray, aPSYInfo);
	iTestParameters = aTestParameters;
    }

// Two-phased constructor.
CPosPSYVerticalCapabilityTest* CPosPSYVerticalCapabilityTest::NewLC(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo,
	const TPosPSYTestParameters& aTestParameters)
    {
    CPosPSYVerticalCapabilityTest* self = 
        new (ELeave) CPosPSYVerticalCapabilityTest(aResults, 
												   aPositionArray, 
												   aPSYInfo,
												   aTestParameters);
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CPosPSYVerticalCapabilityTest::~CPosPSYVerticalCapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYVerticalCapabilityTest::TestCapabilityL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYVerticalCapabilityTest::TestCapabilityL()
    {
    _LIT(KTests, "---- Testing Vertical Capability ----");
    AddResultL(KTests, EDisplayMessage);

    if ((iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityVertical) &&
        iPositionArray->Count() > 0)
        {
		TestCapabilityForPositionInfoL();
		CheckResultsL();
        }
    else
        {
        _LIT(KASupportInfo, "Vertical Capability not checked because capability not supported.");
        AddResultL(KASupportInfo, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYVerticalCapabilityTest::CheckResultsL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYVerticalCapabilityTest::CheckResultsL()
    {
    TBuf<KCapabilityBufSize> buf;
    if (iAccuracyError > (iPositionArray->Count() / 
		iTestParameters.VerticalAccuracyThreshold()))
        {
        _LIT(KError, "More than 1/%d of vertical values outside specified accuracy interval. Number of values outside interval: %d of %d.");
        buf.Format(KError, 
				   iTestParameters.VerticalAccuracyThreshold(), 
				   iAccuracyError, 
				   iPositionArray->Count());
        AddResultL(buf, EErrorMessage);
        }
    else
        {
        _LIT(KInfo, "Less than 1/%d of vertical values outside specified accuracy interval. Number of values outside interval: %d of %d.");
        buf.Format(KInfo, 
				   iTestParameters.VerticalAccuracyThreshold(), 
				   iAccuracyError, 
				   iPositionArray->Count());
        AddResultL(buf, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYVerticalCapabilityTest::TestCapabilityForPositionInfoL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYVerticalCapabilityTest::TestCapabilityForPositionInfoL()
    {
    TPosition pos;
            
    for (TInt i = 0; i < iPositionArray->Count(); i++)
        {
		(*iPositionArray)[i]->GetPosition(pos);
		if (!Math::IsNaN(pos.VerticalAccuracy()))
			{
			if (pos.VerticalAccuracy() > iPSYInfo.VerticalAccuracy())
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
