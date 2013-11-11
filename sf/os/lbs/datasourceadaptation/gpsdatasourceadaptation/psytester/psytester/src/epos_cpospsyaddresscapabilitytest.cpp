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
#include "epos_cpospsyaddresscapabilitytest.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYAddressCapabilityTest::CPosPSYAddressCapabilityTest(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    BaseConstruct(aResults, aPositionArray, aPSYInfo);
    }

// Two-phased constructor.
CPosPSYAddressCapabilityTest* CPosPSYAddressCapabilityTest::NewLC(
    CPosPSYResults& aResults,
    const RPointerArray<TPositionInfo>& aPositionArray,
    const TPosPSYInfo& aPSYInfo)
    {
    CPosPSYAddressCapabilityTest* self 
        = new (ELeave) CPosPSYAddressCapabilityTest(aResults, aPositionArray, aPSYInfo);
    CleanupStack::PushL( self );
    return self;
    }

// Destructor
CPosPSYAddressCapabilityTest::~CPosPSYAddressCapabilityTest()
    {
    }

// ---------------------------------------------------------
// CPosPSYAddressCapabilityTest::TestCapabilityL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYAddressCapabilityTest::TestCapabilityL()
    {
    _LIT(KTests,"---- Testing Address Capability ----");
    AddResultL(KTests, EDisplayMessage);

    if (iPSYInfo.Capabilities() & TPositionModuleInfo::ECapabilityAddress)
        {
        for (TInt i = 0; i < iPositionArray->Count(); i++)
            {
            TBuf16<KAddressResultBufSize> resultBuf;        
            HPositionGenericInfo* genInfo = static_cast<HPositionGenericInfo*>((*iPositionArray)[i]);
			iNoneSet = ETrue;

            if (genInfo->GetValue(EPositionFieldCountry, resultBuf) == KErrNotFound)
                {   
                iCountryNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }

            if (genInfo->GetValue(EPositionFieldCountryCode, resultBuf) == KErrNotFound)
                {   
                iCountryCodeNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }

            if (genInfo->GetValue(EPositionFieldState, resultBuf) == KErrNotFound)
                {   
                iStateNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }

            if (genInfo->GetValue(EPositionFieldCity, resultBuf) == KErrNotFound)
                {   
                iCityNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }

            if (genInfo->GetValue(EPositionFieldDistrict, resultBuf) == KErrNotFound)
                {   
                iDistrictNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }

            if (genInfo->GetValue(EPositionFieldStreet, resultBuf) == KErrNotFound)
                {   
                iStreetNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }

            if (genInfo->GetValue(EPositionFieldStreetExtension, resultBuf) == KErrNotFound)
                {   
                iStreetExtensionNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }

            if (genInfo->GetValue(EPositionFieldLocationName, resultBuf) == KErrNotFound)
                {   
                iLocationNameNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }

            if (genInfo->GetValue(EPositionFieldPostalCode, resultBuf) == KErrNotFound)
                {   
                iPostalCodeNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }

            if (genInfo->GetValue(EPositionFieldLocality, resultBuf) == KErrNotFound)
                {   
                iLocalityNotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }
    
            if (genInfo->GetValue(EPositionFieldCrossing1, resultBuf) == KErrNotFound)
                {   
                iCrossing1NotSet++;
                }
            else
                {
                iNoneSet = EFalse;
                }
            if (genInfo->GetValue(EPositionFieldCrossing2, resultBuf) == KErrNotFound)
                {   
                iCrossing2NotSet++;
                }
            else
                {
                iNoneSet = EFalse;
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
        _LIT(KASupportInfo, "Address Capability not checked because capability not supported.");
        AddResultL(KASupportInfo, EInfoMessage);
        }
    }

// ---------------------------------------------------------
// CPosPSYAddressCapabilityTest::CheckResultsL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYAddressCapabilityTest::CheckResultsL()
    {
    TBuf<KCapabilityBufSize> buf;
    
    if (!iNoneSetError &&
        !iCountryNotSet &&
        !iCountryCodeNotSet &&
        !iStateNotSet &&
        !iCityNotSet &&
        !iDistrictNotSet &&
        !iStreetNotSet &&
        !iStreetExtensionNotSet &&
        !iLocationNameNotSet &&
        !iPostalCodeNotSet &&
        !iLocalityNotSet &&
        !iCrossing1NotSet &&
        !iCrossing2NotSet)
        {
        _LIT(KInfo,"Address capability test was succesfully completed!");
        AddResultL(KInfo, EInfoMessage);
        }
    else
        {
        if (iNoneSetError > 0)
            {
            _LIT(KError, "Address Capability supported but no fields set in %d of %d cases.");
            buf.Format(KError, iNoneSetError, iPositionArray->Count());
            AddResultL(buf, EErrorMessage);
            }
		if ((iPositionArray->Count() - iNoneSetError) > 0)
			{
			_LIT(KWarningNotSet, "Address Capability supported but not all field set in all cases.");
			AddResultL(KWarningNotSet(), EWarningMessage);
			}
        if (iCountryNotSet > 0)
            {   
            _LIT(KWarning, "Country not set in %d of %d cases.");
            buf.Format(KWarning, iCountryNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iCountryCodeNotSet > 0)
            {   
            _LIT(KWarning, "Country code not set in %d of %d cases.");
            buf.Format(KWarning, iCountryCodeNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iStateNotSet > 0)
            {   
            _LIT(KWarning, "State not set in %d of %d cases.");
            buf.Format(KWarning, iStateNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iCityNotSet > 0)
            {   
            _LIT(KWarning, "City not set in %d of %d cases.");
            buf.Format(KWarning, iCityNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iDistrictNotSet > 0)
            {   
            _LIT(KWarning, "District not set in %d of %d cases.");
            buf.Format(KWarning, iDistrictNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iStreetNotSet > 0)
            {   
            _LIT(KWarning, "Street not set in %d of %d cases.");
            buf.Format(KWarning, iStreetNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iStreetExtensionNotSet > 0)
            {   
            _LIT(KWarning, "Street Extension not set in %d of %d cases.");
            buf.Format(KWarning, iStreetExtensionNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iLocationNameNotSet > 0)
            {   
            _LIT(KWarning, "Location Name not set in %d of %d cases.");
            buf.Format(KWarning, iLocationNameNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iPostalCodeNotSet > 0)
            {   
            _LIT(KWarning, "Postal Code not set in %d of %d cases.");
            buf.Format(KWarning, iPostalCodeNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iLocalityNotSet > 0)
            {   
            _LIT(KWarning, "Locality not set in %d of %d cases.");
            buf.Format(KWarning, iLocalityNotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iCrossing1NotSet > 0)
            {   
            _LIT(KWarning, "Crossing1 not set in %d of %d cases.");
            buf.Format(KWarning, iCrossing1NotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        if (iCrossing2NotSet > 0)
            {   
            _LIT(KWarning, "Crossing2 not set in %d of %d cases.");
            buf.Format(KWarning, iCrossing2NotSet, iPositionArray->Count());
            AddResultL(buf, EWarningMessage);
            }
        }
    }

//  End of File
