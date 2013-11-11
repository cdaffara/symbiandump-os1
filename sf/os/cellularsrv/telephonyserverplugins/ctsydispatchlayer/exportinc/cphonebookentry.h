// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @internalAll 
*/

#ifndef CPHONEBOOKENTRY_H_
#define CPHONEBOOKENTRY_H_

#include <e32def.h>
#include <e32cmn.h>

#include <mpbutil.h>

class CPhoneBookEntry : public CBase
	{
public:

	IMPORT_C static CPhoneBookEntry* NewL();
	IMPORT_C static CPhoneBookEntry* NewLC();
	
	IMPORT_C void Reset();
	
	IMPORT_C TInt GetIndex() const;
	IMPORT_C const TDesC& GetFirstName() const;
	IMPORT_C const TDesC& GetDiallingNumber() const;
	IMPORT_C const RArray<RBuf>& GetEmailAddresss() const;
	IMPORT_C const RArray<RBuf>& GetSecondNames() const;
	IMPORT_C const RArray<RBuf>& GetAdditionalNumbers() const;
	
	IMPORT_C void SetIndex(TInt aIndex);
	IMPORT_C TInt SetFirstName(const TDesC& aFirstName);
	IMPORT_C TInt SetDiallingNumber(const TDesC& aDiallingNumber);
	
	IMPORT_C TInt AddEmailAddress(const TDesC& aEmailAddress);
	IMPORT_C TInt AddSecondName(const TDesC& aSecondName);
	IMPORT_C TInt AddAdditionalNumber(const TDesC& aAdditionalNumber);
	
	IMPORT_C TInt ExternalizeToTlvEntry(CPhoneBookBuffer& aPhoneBookBuffer) const;
	IMPORT_C TInt InternalizeFromTlvEntry(CPhoneBookBuffer& aPhoneBookBuffer, TBool aNewEntryTagRead = EFalse);

	IMPORT_C TInt TlvLength() const;
	
	IMPORT_C TBool operator== (const CPhoneBookEntry& aPhoneBookEntry) const;

	~CPhoneBookEntry();
	
private:

	void ConstructL();
	CPhoneBookEntry();
	
	TInt GetText(TPtrC16& text, CPhoneBookBuffer::TPhBkTagType aTagType, CPhoneBookBuffer& aPhoneBookBuffer) const;
	void DesSize(TInt& aSize, const TDesC& aDes) const;

	
private:
	
	TInt iIndex;
	TBool iIndexSet;
	RBuf iFirstName;
	RBuf iDiallingNumber;
	RArray<RBuf> iEmailAddresss;
	RArray<RBuf> iSecondNames;
	RArray<RBuf> iAdditionalNumbers;
	};

#endif /*CPHONEBOOKENTRY_H_*/
