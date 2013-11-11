// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "BTRegistryDB.h"
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_MANAGER_SERVER);
#endif

const TInt KMaxLengthConstraintInitial = 60;	// constraint length is initially this
const TInt KMaxLengthConstraintIncrement = 60;	// - then we extend as necessary by this amount

RBTDbQuery::RBTDbQuery()
/**
	Required for non-default constructed RTextBuf
**/
	{
	LOG_FUNC
	}

void RBTDbQuery::Close()
	{
	LOG_FUNC
	iQueryBuf.Close();
	}

const TPtrC RBTDbQuery::QuoteEscapeLC(const TBTDevAddr& aBDAddr) const
	{
	LOG_FUNC
	TBuf<6> wideAddr;	// needed to stop implicit converstion back to DevAddr!
	wideAddr.Copy(aBDAddr.Des());
	return QuoteEscapeLC(wideAddr);
	}

const TPtrC RBTDbQuery::QuoteEscapeLC(const TDesC& aDes) const
/**
	Can't search for a string in SQL with a quote (') in un-escaped
	Escape by putting extra quote in for every quote in the descriptor
**/
	{
	LOG_FUNC
	const TUint16 KEscapeCode = 0x0027;	// SQL escape value
	// maximum size of new descriptor will be twice the original (if it contained all quotes!)
	HBufC* escaped = HBufC::NewMaxLC(2*aDes.Length());
	TPtr escapedPtr= escaped->Des();

	TInt i=0;
	TInt escapedIndex = 0;
	while (i<aDes.Length())
		{
		const TChar& element = aDes[i++];	// copes with narrow and wide
#pragma warning (disable : 4244)  // conversion - its ok here
		escapedPtr[escapedIndex++] = element;
#pragma warning (default : 4244)
		if (element == KEscapeCode)
			{
			escapedPtr[escapedIndex++] = KEscapeCode;
			}
		}
	escapedPtr.SetLength(escapedIndex);	// bring down from the MaxLength
	return escapedPtr;	// returns ownership of the HBuf via cleanupstack
	}

void RBTDbQuery::FindDeviceL(const TBTDevAddr& aBDAddr)
/**
	Construct a simple SQL query to see if the device is present
**/
	{
	LOG_FUNC
	// this is used to see if the device is in the registry
	TBuf<KMaxConstraintLen> constraint;

	constraint.Append(KColName_DeviceAddress);
	constraint.Append(KSQLEqual);
	constraint.Append(KSQLQuote);
	constraint.Append(QuoteEscapeLC(aBDAddr));
	constraint.Append(KSQLQuote);

	iQueryBuf.SetMaxLengthL(constraint.Length() + KSQLSelectAllConstrained().Length() + KDeviceTable().Length());
	iQueryBuf.Text().Format(KSQLSelectAllConstrained, &KDeviceTable, &constraint);

	CleanupStack::PopAndDestroy(1); // address buffer
	}

void RBTDbQuery::FindCommPortL(TUint32 aUnitNumber)
/**
	Construct a SQL query to find the virtual serial port with aUnitNumber
**/
	{
	LOG_FUNC
	TBuf<KMaxConstraintLen> constraint;

	constraint.Append(KBTCOMMColName_Port);
	constraint.Append(KSQLEqual);
	constraint.AppendNum(static_cast<TUint>(aUnitNumber), EDecimal);

	iQueryBuf.SetMaxLengthL(constraint.Length() + KSQLSelectAllConstrained().Length() + KCSYTable().Length());
	iQueryBuf.Text().Format(KSQLSelectAllConstrained, &KCSYTable, &constraint);
	}

void RBTDbQuery::MatchLinkKeyTypeL(TBTLinkKeyType aLinkKeyType)
/**
 	Construct a SQL query to find entries that have a link key of a specific type.
**/
	{
	LOG_FUNC
	TBuf<KMaxConstraintLen> constraint;

	constraint.Append(KDeviceColName_LinkKeyType);
	constraint.Append(KSQLEqual);
	constraint.AppendNum(static_cast<TUint>(aLinkKeyType), EDecimal);

	iQueryBuf.SetMaxLengthL(constraint.Length() + KSQLSelectAllConstrained().Length() + KDeviceTable().Length());
	iQueryBuf.Text().Format(KSQLSelectAllConstrained, &KDeviceTable, &constraint);
	}

const TDesC& RBTDbQuery::QueryBuf() const
	{
	LOG_FUNC
	return iQueryBuf.Text();
	}

TPtrC RBTDbQuery::ConstraintBuf() const
	{
	LOG_FUNC
	TInt index = QueryBuf().Find(KSQLWhere);
	return (index==KErrNotFound) ? TPtrC() : QueryBuf().Mid(index+KSQLWhere().Length());
	}

void RBTDbQuery::ExtendForAnotherTokenL(RTextBuf& aSQLBuf, TUint aMask)
/**
	Sees if the token 'AND' is required in a constrained search
	If so, add into the SQL query
	If the buffer is getting small, extend it
**/
	{
	LOG_FUNC
	if (aMask)
		{
		// need an AND - more constraints follow
		aSQLBuf.Text().Append(KSQLAnd);
		//Need to increase the size of the bufferno
		// RTextBuf::SetMaxLengthL is bad as it closes the textbuf
		RTextBuf copy;
		copy.SetMaxLengthL(aSQLBuf.Text().Length());
		CleanupClosePushL(copy);
		copy.Text().Append(aSQLBuf.Text());
		aSQLBuf.SetMaxLengthL(aSQLBuf.Text().Length()+KMaxLengthConstraintIncrement);
		//copy back from the copy to the aSQLBuf
		aSQLBuf.Text().Append(copy.Text());
		CleanupStack::PopAndDestroy();
		}
	// else do nothing - no further constraint to follow
	}

void RBTDbQuery::SearchL(const TBTRegistrySearch& aPattern)
/**
	Converts a search mask into SQL search tokens
**/
	{
	LOG_FUNC
	// do the catch all first - saves time
	if (aPattern.iSearchMask & TBTRegistrySearch::EAll)
		{
		iQueryBuf.SetMaxLengthL(KSQLSelectAll().Length() + KDeviceTable().Length());
		iQueryBuf.Text().Format(KSQLSelectAll, &KDeviceTable);
		return;
		}

	// we have a constrained search; formulate the SQL
	// we use a RTextBuf that can grow as needed (to reduce stack usage)

	RTextBuf constraintBuf;

	constraintBuf.SetMaxLengthL(KMaxLengthConstraintInitial);

	TPtr& constraint = constraintBuf.Text();

	TUint toSearch = aPattern.iSearchMask;
	
	if (toSearch & TBTRegistrySearch::EAddress)
		{
		constraint.Append(KColName_DeviceAddress);
		constraint.Append(KSQLEqual);
		constraint.Append(KSQLQuote);
		constraint.Append(QuoteEscapeLC(aPattern.iDeviceAddress));
		constraint.Append(KSQLQuote);
		CleanupStack::PopAndDestroy(1); // address buffer
		
		toSearch &= ~TBTRegistrySearch::EAddress;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}


	if (toSearch & TBTRegistrySearch::ECoD)
		{
		constraint.Append(KDeviceColName_CoD);
		constraint.Append(KSQLEqual);
		TUint cod = aPattern.iDeviceClass.DeviceClass();
		constraint.AppendNum(cod, EDecimal);

		toSearch &= ~TBTRegistrySearch::ECoD;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}
		
	if (toSearch & TBTRegistrySearch::ECoDMajorDev)
		{
		constraint.Append(KDeviceColName_CoD_MajorDev);
		constraint.Append(KSQLEqual);
		TUint majDev = aPattern.iDeviceClass.MajorDeviceClass();
		constraint.AppendNum(majDev, EDecimal);

		toSearch &= ~TBTRegistrySearch::ECoDMajorDev;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}

	if (toSearch & TBTRegistrySearch::ECoDMinorDev)
		{
		constraint.Append(KDeviceColName_CoD_MinorDev);
		constraint.Append(KSQLEqual);
		TUint minDev = aPattern.iDeviceClass.MinorDeviceClass();
		constraint.AppendNum(minDev, EDecimal);

		toSearch &= ~TBTRegistrySearch::ECoDMinorDev;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}

	if (toSearch & TBTRegistrySearch::EBTName)
		{
		constraint.Append(KDeviceColName_BluetoothName);
		constraint.Append(KSQLEqual);
		constraint.Append(KSQLQuote);
		TBuf<KMaxBluetoothNameLen> widened;
		widened.Copy(aPattern.iBluetoothName);
		constraint.Append(QuoteEscapeLC(widened));
		constraint.Append(KSQLQuote);
		CleanupStack::PopAndDestroy(1); //name

		toSearch &= ~TBTRegistrySearch::EBTName;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}

	if (toSearch & TBTRegistrySearch::EFriendlyName)
		{
		constraint.Append(KDeviceColName_FriendlyName);
		constraint.Append(KSQLEqual);
		constraint.Append(KSQLQuote);
		constraint.Append(QuoteEscapeLC(aPattern.iFriendlyName));
		constraint.Append(KSQLQuote);
		CleanupStack::PopAndDestroy(1); // name

		toSearch &= ~TBTRegistrySearch::EFriendlyName;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}

	if (toSearch & TBTRegistrySearch::EProcess)
		{
		constraint.Append(KColName_ProcessSID);
		constraint.Append(KSQLEqual);
		constraint.AppendNum(MAKE_TUINT64(0,aPattern.iCurrentProcessSID.iUid), EDecimal);

		toSearch &= ~TBTRegistrySearch::EProcess;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}

	if (toSearch & TBTRegistrySearch::EBonded)
		{
		constraint.Append(KDeviceColName_LinkKey);
		constraint.Append(KSQLIsNotNull);

		toSearch &= ~TBTRegistrySearch::EBonded;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}

	if (toSearch & TBTRegistrySearch::ETrusted)
		{
		// check global security for no authorisation AND authentication
		constraint.Append(KSQLOpenParentheses);
		constraint.Append(KDeviceColName_GlobalSecSecurity);
		constraint.Append(KSQLEqual);

		// we do this here in case TBTDeviceSecurity changes
		// trusted device = 'NoAuthentication'=No (MUST authenticate!),
		//					'NoAuthorisation'=Yes
		//					'Encryption' could be either.
		//					'Banned' =No (!)

		TBTDeviceSecurity noEncryption(EFalse, ETrue, EFalse, EFalse);
		constraint.AppendNum(static_cast<TUint>(noEncryption.SecurityValue()));

		constraint.Append(KSQLOr);
		constraint.Append(KDeviceColName_GlobalSecSecurity);
		constraint.Append(KSQLEqual);
		
		TBTDeviceSecurity encryption(EFalse, ETrue, ETrue, EFalse);
		constraint.AppendNum(static_cast<TUint>(encryption.SecurityValue()));
		constraint.Append(KSQLCloseParentheses);

		toSearch &= ~TBTRegistrySearch::ETrusted;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}

	if (toSearch & TBTRegistrySearch::ELastSeen)
		{
		constraint.Append(KDeviceColName_LastSeen);
		constraint.Append(KSQLGreaterEqual);
		constraint.AppendNum(aPattern.iLastSeen.Int64());
		toSearch &= ~TBTRegistrySearch::ELastSeen;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}

	if (toSearch & TBTRegistrySearch::ELastUsed)
		{
		constraint.Append(KDeviceColName_LastUsed);
		constraint.Append(KSQLGreaterEqual);
		constraint.AppendNum(aPattern.iLastUsed.Int64());
		toSearch &= ~TBTRegistrySearch::ELastUsed;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}
	
	if (toSearch & TBTRegistrySearch::EUiCookie)
		{
		constraint.Append(KDeviceColName_UiCookie);
		constraint.Append(KSQLLike);
		constraint.Append(KSQLQuote);
		static const TUint8 KCookieBinaryRepresentationWidth = 32; // 32bits encoded as binary string.
		TBuf<KCookieBinaryRepresentationWidth> cookiePattern;
		cookiePattern.NumFixedWidth(aPattern.iUiCookie, EBinary, KCookieBinaryRepresentationWidth);
		TUint32 shiftedMask = aPattern.iUiCookieMask;
		for(TInt i=KCookieBinaryRepresentationWidth-1; i>=0; --i)
			{
			if(!(shiftedMask&1)) // check the bottom bit (we loop through them all by shifting.)
				{
				// bit "i" isn't masked, and so we should ignore it.
				cookiePattern[i] = '?';
				}
			shiftedMask >>= 1;
			}
		constraint.Append(cookiePattern);
		constraint.Append(KSQLQuote);
		
		toSearch &= ~TBTRegistrySearch::EUiCookie;
		ExtendForAnotherTokenL(constraintBuf, toSearch);
		}
	
	if(toSearch)
		{
		// There is an unhandled search attribute that has been provided by the user.
		// If left the trailing SQL "AND" would trigger the KErrArgument error anyway.
		User::Leave(KErrArgument);
		}

	iQueryBuf.SetMaxLengthL(constraint.Length() + KSQLSelectAllConstrained().Length() + KDeviceTable().Length());
	iQueryBuf.Text().Format(KSQLSelectAllConstrained, &KDeviceTable, &constraint);
	}

