// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Resolution of phone numbers
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include <commdb.h>
#include "commdb_impl.H"
#include <commsdatutils.h>

namespace commdb_dll
/**
@internalComponent
*/

    {
//	const TText KStarChar='*';
//	const TText KPlusChar='+';
    }
using namespace commdb_dll;
using namespace CommsDatUtils;


EXPORT_C void CCommsDatabaseBase::ResolvePhoneNumberL(TDesC& aNumber, TDes& aDialString, TParseMode aDialParseMode, TUint32 aLocationId, TUint32 aChargecardId)
/** Resolves a given telephone number based on a specified dialling location and chargecard.
The function does not require a connection to the database server, but will
form one itself.

The function does not resolve for mobile phones; aNumber is just copied to
aDialString.

@param aNumber The number to be parsed.
@param aDialString On return, contains the parsed number.
@param aDialParseMode Indicates whether the parsed string is intended for dialling
or for display.
@param aLocationId ID of the dialling location record to be used for resolution.
@param aChargecardId ID of the chargecard ID to be used for resolution.
@leave KErrNotFound if the specified location and chargecard records cannot be found;
KErrOverflow if aDialString is not long enough to hold the resolved number. */
    {
    CCommsDatUtils::ResolvePhoneNumberL(aNumber, aDialString, aDialParseMode, aLocationId, aChargecardId);
    }

EXPORT_C void CCommsDatabaseBase::ResolvePhoneNumberFromDatabaseL(TDesC& aNumber, TDes& aDialString, TParseMode aDialParseMode, TUint32 aLocationId, TUint32 aChargecardId)
/** Resolves a given telephone number based on a specified dialling location and chargecard.
A connection must already exist to the database.

The function does not resolve for mobile phones; aNumber is just copied to
aDialString.

@param aNumber A reference to the descriptor containing the number to be parsed.
@param aDialString On return, a reference to a descriptor which contains the parsed number.
@param aDialParseMode Indicates whether the parsed string is intended for dialling
or for display.
@param aLocationId ID of the location record.
@param aChargecardId ID of the chargecard ID to be used for resolution.
@leave KErrNotFound if the specified location and chargecard records cannot be found;
KErrOverflow if aDialString is not long enough to hold the resolved number. */
    {
    CCommsDatUtils::ResolvePhoneNumberFromDatabaseL(aNumber, aDialString, aDialParseMode, aLocationId, aChargecardId);
    }
void CCommsDatabaseBase::PopulateDialLocationL(TUint32 aId, TDialLocation& aLocation)
/**
populates aLocation with configuration details related to the current location of the telephone equipment
getting the details from the database based on aId.

@param aId Unique Id associated with the record in the database.
@param aLocation On return, contains the configuration details related to the current location of the telephone equipment
*/
    {
    CCDLocationRecord* locationRecord = static_cast<CCDLocationRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLocationRecord));
    CleanupStack::PushL(locationRecord);

    // Prime for record id
    locationRecord->SetRecordId(aId);
    // load data from database
    locationRecord->LoadL(*iImpl->iDbs);

    // read values from database
    aLocation.SetIntlPrefCode(locationRecord->iIntlPrefixCode);
    aLocation.SetNatPrefCode(locationRecord->iNatPrefixCode);
    aLocation.SetNatCode(locationRecord->iNatCode);
    aLocation.SetAreaCode(locationRecord->iAreaCode);
    aLocation.SetDialOutLocalCode(locationRecord->iDialOutCode);
    aLocation.SetDialOutLongDistanceCode(locationRecord->iDialOutCode);
    aLocation.SetDisableCallWaitingCode(locationRecord->iDisableCallWaitingCode);

    TUint flags(0);

    if (locationRecord->iMobile)
        {
        flags |= KPhoneNumberMobile;
        }
    if (locationRecord->iUsePulseDial)
        {
        flags |= KPhoneNumberUsePulseDial;
        }
    if (locationRecord->iWaitForDialTone)
        {
        flags |= KPhoneNumberWaitForDialingTone;
        }

    aLocation.SetDialLocationFlags(flags);
    aLocation.SetPauseAfterDialout(locationRecord->iPauseAfterDialOut);

    CleanupStack::PopAndDestroy(); // table
    }

void CCommsDatabaseBase::PopulateChargeCardL(TUint32 aId, TChargeCard& aChargeCard)
/**
Populates aChargeCard with charge card account and PIN details and network operator specific dialling rules
for local, national and international calls by getting the details from the database based on aId.

@param aId Unique Id associated with the record in the database.
@param aChargeCard On return, contains the charge card account and PIN details and network operator specific dialling rules
for local, national and international calls
*/
    {
    CCDChargecardRecord* ccardRecord = static_cast<CCDChargecardRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdChargecardRecord));
    CleanupStack::PushL(ccardRecord);
    // Prime for record id
    ccardRecord->SetRecordId(aId);
    // load data from database
    ccardRecord->LoadL(*iImpl->iDbs);

    aChargeCard.SetNumber(ccardRecord->iAccountNo);
    aChargeCard.SetPin(ccardRecord->iPin);
    aChargeCard.SetLocalRule(ccardRecord->iLocalRule);
    aChargeCard.SetNatRule(ccardRecord->iNatRule);
    aChargeCard.SetIntlRule(ccardRecord->iIntlRule);

    CleanupStack::PopAndDestroy(); // table
    }
