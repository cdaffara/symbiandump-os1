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
#include "epos_cpospsybuildingcapabilitytest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code, that
// might leave.
//
CPosPSYBuildingCapabilityTest::CPosPSYBuildingCapabilityTest(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    BaseConstruct(aResults, aPositionArray, aPSYInfo);
    }

// Two-phased constructor.
CPosPSYBuildingCapabilityTest* CPosPSYBuildingCapabilityTest::NewLC(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    CPosPSYBuildingCapabilityTest* self = 
        new (ELeave) CPosPSYBuildingCapabilityTest(aResults, aPositionArray, aPSYInfo);
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CPosPSYBuildingCapabilityTest::~CPosPSYBuildingCapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYBuildingCapabilityTest::TestCapabilityL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYBuildingCapabilityTest::TestCapabilityL()
    {  
    _LIT(KTests,"---- Testing Building Capability ----");
    AddResultL(KTests, EDisplayMessage);

    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityBuilding)
        {
        for (TInt i = 0; i < iPositionArray->Count(); i++)
            {
            TBuf16<KBuildingResultBufSize> buf; 
            HPositionGenericInfo* genInfo = static_cast<HPositionGenericInfo*>((*iPositionArray)[i]);
			iNoneSet = ETrue;

            if (genInfo->GetValue(EPositionFieldBuildingName, buf) == KErrNotFound)
                {   
                iNrOfBuildingNameNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }
            if (genInfo->GetValue(EPositionFieldBuildingFloor, buf) == KErrNotFound)
                {   
                iNrOfBuildingFloorNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }
            if (genInfo->GetValue(EPositionFieldBuildingRoom, buf) == KErrNotFound)
                {   
                iNrOfBuildingRoomNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }
            if (genInfo->GetValue(EPositionFieldBuildingZone, buf) == KErrNotFound)
                {   
                iNrOfBuildingZoneNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }
            if (genInfo->GetValue(EPositionFieldBuildingTelephone, buf) == KErrNotFound)
                {   
                iNrOfBuildingTelephoneNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }
        
            if (iNoneSet)
                {
                iNrOfNoneSet++;
                }
            }
        
        CheckResultsL();        
        }
    else
        {
        _LIT(KInfo, "Building capability not checked because capability not supported");
        AddResultL(KInfo, EInfoMessage);
        }
   }

// ---------------------------------------------------------
// CPosPSYBuildingCapabilityTest::CheckResultsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYBuildingCapabilityTest::CheckResultsL()
    {
    TBuf<KCapabilityBufSize> buf;
    
    if (!iNrOfNoneSet &&
        !iNrOfBuildingNameNotSet && 
        !iNrOfBuildingFloorNotSet && 
        !iNrOfBuildingRoomNotSet && 
        !iNrOfBuildingZoneNotSet && 
        !iNrOfBuildingTelephoneNotSet)
        {
        _LIT(KInfo, "Building Capability supported and all requests returned complete building information.");
        AddResultL(KInfo, EInfoMessage);
        }
    else
        {
        if (iNrOfNoneSet > 0)
            {
            _LIT(KError,"Building Capability supported but no fields set in %d of %d cases.");
            buf.Format(KError, iNrOfNoneSet, iPositionArray->Count());
            AddResultL(buf, EErrorMessage);
            }
		if ((iPositionArray->Count() - iNrOfNoneSet) > 0)
			{
			_LIT(KWarningNotSet, "Building Capability supported but not all field set in all cases.");
			AddResultL(KWarningNotSet(), EWarningMessage);
			}
        if (iNrOfBuildingNameNotSet != 0)
            {   
            _LIT(KWarning, "Building Name not set in %d of %d cases.");
            buf.Format(KWarning, iNrOfBuildingNameNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iNrOfBuildingFloorNotSet != 0)
            {   
            _LIT(KWarning, "Building Floor not set in %d of %d cases.");
            buf.Format(KWarning, iNrOfBuildingFloorNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iNrOfBuildingRoomNotSet != 0)
            {   
            _LIT(KWarning, "Building Room not set in %d of %d cases.");
            buf.Format(KWarning, iNrOfBuildingRoomNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iNrOfBuildingZoneNotSet != 0)
            {   
            _LIT(KWarning, "Building Zone not set in %d of %d cases.");
            buf.Format(KWarning, iNrOfBuildingZoneNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iNrOfBuildingTelephoneNotSet != 0)
            {   
            _LIT(KWarning, "Building Telephone not set in %d of %d cases.");
            buf.Format(KWarning, iNrOfBuildingTelephoneNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        }
    }

//  End of File
