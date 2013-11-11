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
// Meta-database declarations for use to store Comms Data on the device
// All Data PublishedAll
// 
//

/**
 @file CCommsDatUtilImpl.cpp
*/

 #include "CommsDatInternalDefs.h"
#include <commsdatutils.h>
 #include "CommsDatTypesV1_1.h"
 #include <comms-infras/commsdatschema.h>

 using namespace CommsDat;
 using namespace CommsDatUtils;


EXPORT_C CCommsDatUtils* CCommsDatUtils::NewL()
 /*

 @publishedAll
 */
	{
	CCommsDatUtils* ptrUtils=new(ELeave) CCommsDatUtils();
	CleanupStack::PushL(ptrUtils);
	ptrUtils->ConstructL();
	CleanupStack::Pop(ptrUtils);
	return ptrUtils;
	}

void CCommsDatUtils::ConstructL()
	{
	iUtilsImpl = new (ELeave) CCommsDatUtilImpl();
	}

EXPORT_C CCommsDatUtils::~CCommsDatUtils()
 /*

 @publishedAll
 */
	 {
	 delete iUtilsImpl;
	 }

EXPORT_C void CCommsDatUtils::ResolvePhoneNumberL(TDesC& aNumber, TDes& aDialString, TParseMode aDialParseMode, TUint32 aLocationId, TUint32 aChargecardId)
 /*

 @publishedAll
 */
	{
#ifndef __TOOLS2__
     CCommsDatUtilImpl* tUtilsImpl = new (ELeave) CCommsDatUtilImpl();
	 CleanupStack::PushL(tUtilsImpl);
	 tUtilsImpl->ResolvePhoneNumberL(aNumber, aDialString, aDialParseMode, aLocationId, aChargecardId);
	 CleanupStack::PopAndDestroy(tUtilsImpl);
#endif
	 }

EXPORT_C void CCommsDatUtils::ResolvePhoneNumberFromDatabaseL(TDesC& aNumber, TDes& aDialString, TParseMode aDialParseMode, TUint32 aLocationId, TUint32 aChargecardId)
 /*

 @publishedAll
 */
	 {
	 CCommsDatUtils::ResolvePhoneNumberL(aNumber, aDialString, aDialParseMode, aLocationId, aChargecardId);
	 }


EXPORT_C TInt CCommsDatUtils::CheckReadCapability( const TDesC& aField, const RMessagePtr2* aMessage )
 /*

 @publishedAll
 */
	 {
	 return (iUtilsImpl->CheckReadCapability(aField,aMessage));
	 }


EXPORT_C TInt CCommsDatUtils::CheckWriteCapability( const TDesC& aField, const RMessagePtr2* aMessage )
 /*

 @publishedAll
 */
	 {
	 return (iUtilsImpl->CheckWriteCapability(aField,aMessage));
	 }


//CCommsDatUtilImpl//


 void CCommsDatUtilImpl::ResolvePhoneNumberL(TDesC& aNumber, TDes& aDialString, TParseMode aDialParseMode, TUint32 aLocationId, TUint32 aChargecardId)
 	{
#ifndef __TOOLS2__
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
 	idbSession =CMDBSession::NewL(KCDVersion1_2);
#else
 	idbSession =CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(idbSession);
	idbSession->OpenTransactionL();

	TRAPD(ret,this->ResolvePhoneNumberFromDatabaseL(aNumber,aDialString,aDialParseMode,aLocationId,aChargecardId));

	if (ret==KErrNone)
		idbSession->CommitTransactionL();
	else
		{
		idbSession->RollbackTransactionL();
		User::Leave(ret);
		}
	CleanupStack::PopAndDestroy(idbSession);
	idbSession = NULL;
#endif
 	}


 void CCommsDatUtilImpl::ResolvePhoneNumberFromDatabaseL(TDesC& aNumber, TDes& aDialString, TParseMode aDialParseMode, TUint32 aLocationId, TUint32 aChargecardId)
 {
#ifndef __TOOLS2__
	TDialLocation location;
	PopulateDialLocationL(aLocationId,location);

	if (location.IsMobile())	// Special case - don't resolve for mobile locations
		{
		aDialString.Copy(aNumber);
		return;
		}

	TChargeCard chargeCard;
	if (aChargecardId!=0)
		{
		PopulateChargeCardL(aChargecardId,chargeCard);
		if (aDialParseMode==EForDisplay)		// Hide the pin if displaying
			{
			TChargeCardPin blank;
			blank.Fill(TChar(KStarChar),chargeCard.Pin().Length());
			chargeCard.SetPin(blank);
			}
		}

	// Do parsing (using DIAL parse routine)

	// Work around for bug in DIAL where if there is no plus the number will not be appended
	TDialString tempDialString;
	if (aNumber.Locate(KPlusChar)==KErrNotFound && aNumber.Length()>0)		// needs to be Locate() to mimic DIAL
		User::LeaveIfError(TelephoneNumber::Parse(tempDialString,location,chargeCard,KNullDesC,aDialParseMode));

	TDialString dialString;
	User::LeaveIfError(TelephoneNumber::Parse(dialString,location,chargeCard,aNumber,aDialParseMode));

	// more of the work around....
	if (dialString.CompareF(tempDialString)==KErrNone)
		{
		// the number has not been appended
		if ((dialString.Length()+aNumber.Length())<=dialString.MaxLength())
			dialString.Append(aNumber);
		else
			User::Leave(KErrOverflow);
		}

	aDialString.Copy(dialString);
#endif
 }



void CCommsDatUtilImpl::PopulateDialLocationL(TUint32 aId, TDialLocation& aLocation)
{
#ifndef __TOOLS2__
	CCDLocationRecord *locationRecord = static_cast<CCDLocationRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdLocationRecord));
	CleanupStack::PushL(locationRecord);

	locationRecord->SetRecordId(aId);
	locationRecord->LoadL(*idbSession);

	HBufC* buffer = HBufC::NewLC(KCommsDatSvrRealMaxFieldLength);
	TPtr buf (buffer->Des());

	buf = locationRecord->iIntlPrefixCode;
	aLocation.SetIntlPrefCode(buf);
	buf = locationRecord->iNatPrefixCode;
	aLocation.SetNatPrefCode(buf);
	buf = locationRecord->iNatCode;
	aLocation.SetNatCode(buf);
	buf = locationRecord->iAreaCode;
	aLocation.SetAreaCode(buf);
	buf = locationRecord->iDialOutCode;
	aLocation.SetDialOutLocalCode(buf);
	aLocation.SetDialOutLongDistanceCode(buf);
	buf = locationRecord->iDisableCallWaitingCode;
	aLocation.SetDisableCallWaitingCode(buf);

	TBool b;
	TUint flags=0;
	b = locationRecord->iMobile;
	if (b)
		flags|=KPhoneNumberMobile;
	b = locationRecord->iUsePulseDial;
	if (b)
		flags|=KPhoneNumberUsePulseDial;
	b = locationRecord->iWaitForDialTone;
	if (b)
		flags|=KPhoneNumberWaitForDialingTone;
	aLocation.SetDialLocationFlags(flags);

	TUint32 i = locationRecord->iPauseAfterDialOut;

	aLocation.SetPauseAfterDialout(i);

	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::PopAndDestroy(locationRecord);
#endif
}



void CCommsDatUtilImpl::PopulateChargeCardL(TUint32 aId, TChargeCard& aChargeCard)
{
#ifndef __TOOLS2__
	CCDChargecardRecord *chargecardRecord = static_cast<CCDChargecardRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdChargecardRecord));
	CleanupStack::PushL(chargecardRecord);

	// load data for requested recordId
	chargecardRecord->SetRecordId(aId);
	chargecardRecord->LoadL(*idbSession);

	HBufC* buffer = HBufC::NewLC(KCommsDatSvrRealMaxFieldLength);
	TPtr buf (buffer->Des());

    buf = chargecardRecord->iAccountNo;
	aChargeCard.SetNumber(buf);
	buf = chargecardRecord->iPin;
	aChargeCard.SetPin(buf);
	buf = chargecardRecord->iLocalRule;
	aChargeCard.SetLocalRule(buf);
	buf = chargecardRecord->iNatRule;
	aChargeCard.SetNatRule(buf);
	buf = chargecardRecord->iIntlRule;
	aChargeCard.SetIntlRule(buf);

	CleanupStack::PopAndDestroy(buffer);
	CleanupStack::PopAndDestroy(chargecardRecord);
#endif
}


TInt CCommsDatUtilImpl::CheckReadCapability( const TDesC& /*aField*/, const RMessagePtr2* /*aMessage*/ )
	 {
	 return KErrNone; //todo to be implemented
	 }


TInt CCommsDatUtilImpl::CheckWriteCapability( const TDesC& /*aField*/, const RMessagePtr2* /*aMessage*/ )
	 {
	 return KErrNone; //todo to be implemented
	 }



CCommsDatUtilImpl::~CCommsDatUtilImpl()
	{
	//delete idbSession;
	}


#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
namespace
{


_LIT(KSlash, "\\");

inline TBool EndOfTable(const STableLookup& aEntry )
	{
	TBool retval = ( aEntry.iTableId == 0
						&& aEntry.iTableName == NULL
						&& aEntry.iFields == NULL );
	return retval;
	}

inline TBool EndOfTable(const CommsDat::SRecordTypeInfo& aEntry)
	{
	TBool retval = ( aEntry.iTypeId == 0
						&& aEntry.iValType == 0
						&& aEntry.iTypeAttr == CommsDat::ENoAttrs
						&& aEntry.iTypeName == NULL );

	return retval;
	}


const STableLookup* GetTable(CommsDat::TMDBElementId aTable)
	{
	const STableLookup* retval = NULL;
	TInt index = 0;
	
	const STableLookup* entry = &CommsDatSchemaV1_1::iTableLookup[0];

	while ( !EndOfTable( *entry ) )
		{
		if ( entry->iTableId == aTable )
			{
			retval = entry;
			break; // while
			}
		index++;
		entry = &CommsDatSchemaV1_1::iTableLookup[index];
		}

	return retval;
	}

const CommsDat::SRecordTypeInfo* FieldWithinTable(CommsDat::TMDBElementId aField, const CommsDat::SRecordTypeInfo aTable[])
	{
	const CommsDat::SRecordTypeInfo* retval = NULL;
	const CommsDat::SRecordTypeInfo* entry = &aTable[0];
	TInt index = 0;

	while( !EndOfTable( *entry ) )
		{
		if ( entry->iTypeId == aField )
			{
			retval = entry;
			break;
			}
		index++;
		entry = &aTable[index];
		}

	return retval;
	}
} // end of nameless namespace


EXPORT_C TInt NameLookUp::FieldNameLookup(CommsDat::TMDBElementId aElementId, HBufC*& aFieldName, TInt& aDataType )
	{
	TInt retval = KErrNotFound;
	CommsDat::TMDBElementId cleanElementId = aElementId & KCDMaskShowType; // 0x7fff0000; // remove last two bytes; not needed
	CommsDat::TMDBElementId table = cleanElementId & KCDMaskShowRecordType;
	CommsDat::TMDBElementId fieldInTable = cleanElementId;

	const STableLookup* tablePtr = NULL;
	const CommsDat::SRecordTypeInfo* fieldPtr = NULL;

	// find table
	tablePtr = GetTable(table);
	if ( tablePtr )
		{
		fieldPtr = FieldWithinTable(fieldInTable, tablePtr->iFields);

		if ( fieldPtr == NULL )
			{
			// field not found, this could be because we're dealing with a base
			// class id, mask out table info and search again.
			fieldInTable = cleanElementId & KCDMaskShowFieldType;
			fieldPtr = FieldWithinTable(fieldInTable, tablePtr->iFields);
			}
		}

	if ( fieldPtr )
		{
		const TPtrC tableName( tablePtr->iTableName );
		const TPtrC fieldName( fieldPtr->iTypeName );

		TInt stringLength = tableName.Length() + fieldName.Length() + KSlash().Length();
		TInt error = KErrNone;
		HBufC* ptr = HBufC::New( stringLength );
		TPtr destination( 0, 0);

		if ( ptr != NULL )
			{
			destination.Set( ptr->Des() );
			}
		else
			{
			error = KErrNoMemory;
			}

		if ( error == KErrNone )
			{
			destination.Append( tableName );
			destination.Append( KSlash );
			destination.Append( fieldName );
			aFieldName = ptr;
			aDataType = fieldPtr->iValType;
			retval = KErrNone;
			}
		else
			{
			delete ptr;
			aFieldName = NULL;
			retval = error;
			}

		}

	return retval;
	}

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

