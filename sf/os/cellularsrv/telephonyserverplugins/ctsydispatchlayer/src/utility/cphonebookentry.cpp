// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ctsy/ltsy/cphonebookentry.h>

/**
 * Factory function
 * 
 * @return Pointer to a new CPhoneBookEntry object
 */
EXPORT_C CPhoneBookEntry* CPhoneBookEntry::NewLC()
	{
	CPhoneBookEntry* self = new (ELeave) CPhoneBookEntry();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 * Factory function
 * 
 * @return Pointer to a new CPhoneBookEntry object left on the cleanup stack
 */
EXPORT_C CPhoneBookEntry* CPhoneBookEntry::NewL()
	{
	CPhoneBookEntry* self = CPhoneBookEntry::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

/**
 * Second phase construction
 */
void CPhoneBookEntry::ConstructL()
	{
	}

/**
 * First phase construction
 */
CPhoneBookEntry::CPhoneBookEntry() : 
	iIndex(0),
	iIndexSet(EFalse)
	{
	}

/**
 * Destruction
 */

CPhoneBookEntry::~CPhoneBookEntry()
	{
	Reset();
	}

/**
 * Resets the object to its initial state.
 */
EXPORT_C void CPhoneBookEntry::Reset()
	{
	iIndex = 0;
	iIndexSet = EFalse;
	iFirstName.Close();
	iDiallingNumber.Close();

	TInt i = 0;
	
	const TInt KEmailAddressCount = iEmailAddresss.Count();
	for(i = 0; i < KEmailAddressCount; ++i)
		{
		iEmailAddresss[i].Close();
		}
	iEmailAddresss.Close();
	
	const TInt KSecondNamesCount = iSecondNames.Count();
	for(i = 0; i < KSecondNamesCount; ++i)
		{
		iSecondNames[i].Close();
		}
	iSecondNames.Close();
	
	const TInt KAdditionalNumbersCount = iAdditionalNumbers.Count();
	for(i = 0; i < KAdditionalNumbersCount; ++i)
		{
		iAdditionalNumbers[i].Close();
		}
	iAdditionalNumbers.Close();
	}

/**
 * @return The Entry Index.
 */
EXPORT_C TInt CPhoneBookEntry::GetIndex() const
	{
	return iIndex;
	}

/**
 * @return The entries firstname.
 */
EXPORT_C const TDesC& CPhoneBookEntry::GetFirstName() const
	{
	return iFirstName;
	}

/**
 * @return The entries dialling number.
 */
EXPORT_C const TDesC& CPhoneBookEntry::GetDiallingNumber() const
	{
	return iDiallingNumber;
	}

/**
 * @return An array of Email address's in the entry.
 */

EXPORT_C const RArray<RBuf>& CPhoneBookEntry::GetEmailAddresss() const
	{
	return iEmailAddresss;
	}

/**
 * @return An array of second names in the entry.
 */
EXPORT_C const RArray<RBuf>& CPhoneBookEntry::GetSecondNames() const
	{
	return iSecondNames;
	}

/**
 * @return An array of additional numbers.
 */
EXPORT_C const RArray<RBuf>& CPhoneBookEntry::GetAdditionalNumbers() const
	{
	return iAdditionalNumbers;
	}

/**
 * @param aIndex The index to set.
 */
EXPORT_C void CPhoneBookEntry::SetIndex(TInt aIndex)
	{
	iIndex = aIndex;
	iIndexSet = ETrue;
	}

/**
 * @param aFirstName The firstname to be set.
 * @return System wide error code.
 */
EXPORT_C TInt CPhoneBookEntry::SetFirstName(const TDesC& aFirstName)
	{
	TInt error = iFirstName.ReAlloc(aFirstName.Length());
	
	if(error == KErrNone)
		{
		iFirstName.Copy(aFirstName);
		}
	
	return error;
	}

/**
 * @param aFirstName The dialling number to be set.
 * @return System wide error code.
 * 
 */
EXPORT_C TInt CPhoneBookEntry::SetDiallingNumber(const TDesC& aDiallingNumber)
	{
	TInt error = iDiallingNumber.ReAlloc(aDiallingNumber.Length());
	
	if(error == KErrNone)
		{
		iDiallingNumber.Copy(aDiallingNumber);
		}
	
	return error;
	}

/**
 * @param aEmailAddress An email address to be added to the entries list.
 * @return System wide error code.
 */
EXPORT_C TInt CPhoneBookEntry::AddEmailAddress(const TDesC& aEmailAddress)
	{
	RBuf emailAddress;
	TInt error = emailAddress.Create(aEmailAddress);
	
	if(error == KErrNone)
		{
		error = iEmailAddresss.Append(emailAddress);
		}
	
	return error;
	}

/**
 * @param aSecondName An secondname to be added to the entries list.
 * @return System wide error code.
 * 
 */
EXPORT_C TInt CPhoneBookEntry::AddSecondName(const TDesC& aSecondName)
	{
	RBuf secondName;
	TInt error = secondName.Create(aSecondName);
	
	if(error == KErrNone)
		{
		error = iSecondNames.Append(secondName);
		}
	
	return error;
	}

/**
 * @param aAdditionalNumber An additional number to be added to the entries list.
 * @return System wide error code.
 */
EXPORT_C TInt CPhoneBookEntry::AddAdditionalNumber(const TDesC& aAdditionalNumber)
	{
	RBuf additionalNumber;
	TInt error = additionalNumber.Create(aAdditionalNumber);
	
	if(error == KErrNone)
		{
		error = iAdditionalNumbers.Append(additionalNumber);
		}
	
	return error;
	}

/**
 * @return If the two entries are equal, equivalence is defined is all entries are equal and in
 * the same order.
 */
EXPORT_C TBool CPhoneBookEntry::operator== (const CPhoneBookEntry& aPhoneBookEntry) const
	{
	TBool ret =	(iIndex == aPhoneBookEntry.GetIndex()) &&
				(iFirstName == aPhoneBookEntry.GetFirstName()) &&
				(iDiallingNumber == aPhoneBookEntry.GetDiallingNumber()) &&
				(iEmailAddresss.Count() == aPhoneBookEntry.GetEmailAddresss().Count()) &&
				(iSecondNames.Count() == aPhoneBookEntry.GetSecondNames().Count()) &&
				(iAdditionalNumbers.Count() == aPhoneBookEntry.GetAdditionalNumbers().Count());
	
	TInt i = 0;
	for(i = 0; i < iEmailAddresss.Count() && ret; ++i)
		{
		ret = iEmailAddresss[i] == aPhoneBookEntry.GetEmailAddresss()[i];
		}

	for(i = 0; i < iSecondNames.Count() && ret; ++i)
		{
		ret = iSecondNames[i] == aPhoneBookEntry.GetSecondNames()[i];
		}
	
	for(i = 0; i < iAdditionalNumbers.Count() && ret; ++i)
		{
		ret = iAdditionalNumbers[i] == aPhoneBookEntry.GetAdditionalNumbers()[i];
		}
	
	return ret;
	}

/**
 * Writes the entry to a CPhoneBookBuffer in the TLV format.
 * 
 * @param aPhoneBookBuffer The buffer to write the entry to.
 * @return System wide error code.
 */
EXPORT_C TInt CPhoneBookEntry::ExternalizeToTlvEntry(CPhoneBookBuffer& aPhoneBookBuffer) const
	{
	TInt error = KErrNone;
	
	if(error == KErrNone)
		{
		error = aPhoneBookBuffer.AddNewEntryTag();
		}

	if(iIndexSet && (error == KErrNone))
		{
		error = aPhoneBookBuffer.PutTagAndValue(RMobilePhoneBookStore::ETagPBAdnIndex, static_cast<TUint16>(iIndex));
		}
		
	if(iFirstName.Length() > 0 && (error == KErrNone))
		{
		error = aPhoneBookBuffer.PutTagAndValue(RMobilePhoneBookStore::ETagPBText,iFirstName);
		}
		
	if(iDiallingNumber.Length() > 0 && (error == KErrNone))
		{
		error = aPhoneBookBuffer.PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber,iDiallingNumber);
		}
		
	TInt i = 0;

	for(i = 0; i < iEmailAddresss.Count() && (error == KErrNone); ++i)
		{
		if(iEmailAddresss[i].Length() > 0)
			{
			error = aPhoneBookBuffer.PutTagAndValue(RMobilePhoneBookStore::ETagPBEmailAddress,iEmailAddresss[i]);
			}
		}
	
	for(i = 0; i < iSecondNames.Count() && (error == KErrNone); ++i)
		{
		if(iSecondNames[i].Length() > 0)
			{
			error = aPhoneBookBuffer.PutTagAndValue(RMobilePhoneBookStore::ETagPBSecondName,iSecondNames[i]);
			}
		}
		
	for(i = 0; i < iAdditionalNumbers.Count() && (error == KErrNone); ++i)
		{
		if(iAdditionalNumbers[i].Length() > 0)
			{
			error = aPhoneBookBuffer.AddNewNumberTag();
			if(error == KErrNone)
				{
				error = aPhoneBookBuffer.PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber,iAdditionalNumbers[i]);
				}
			}
		}
	
	return error;
	}

/**
 * Reads an entry from a CPhoneBookBuffer, this does not reset the object first.
 * 
 * @param aPhoneBookBuffer The buffer to read from.
 * @param aNewEntryTagRead If the TLV being read has a new entry tag or not.
 * 
 * @return System wide error code.
 * 
 */
EXPORT_C TInt CPhoneBookEntry::InternalizeFromTlvEntry(CPhoneBookBuffer& aPhoneBookBuffer, TBool aNewEntryTagRead)
	{
    TUint8 tag = 0;
    CPhoneBookBuffer::TPhBkTagType tagType = CPhoneBookBuffer::EPhBkTypeNoData;
	TInt error = KErrNone;
	TInt newEntryCount = aNewEntryTagRead ? 1 : 0;
	TBool gettingAdditionalNumber = EFalse;
	
	while(error == KErrNone && aPhoneBookBuffer.RemainingReadLength() > 0 && newEntryCount < 2)
		{
		error = aPhoneBookBuffer.GetTagAndType(tag,tagType);

		if(gettingAdditionalNumber)
			{
			if(tag != RMobilePhoneBookStore::ETagPBNumber)
				{
				error = KErrArgument;
				}
			}
		
		if(error == KErrNone)
			{
			switch(tag)
				{
				case RMobilePhoneBookStore::ETagPBNewEntry:
					{
					newEntryCount += 1;
					break; //RMobilePhoneBookStore::ETagPBNewEntry
					}

				case RMobilePhoneBookStore::ETagPBAdnIndex:
					{
					iIndexSet = ETrue;
					
					switch(tagType)
						{
						case CPhoneBookBuffer::EPhBkTypeInt8:
							{
							TUint8 value = 0;
							error = aPhoneBookBuffer.GetValue(value);
							iIndex = (error == KErrNone ? value : 0);
							break;
							}
						case CPhoneBookBuffer::EPhBkTypeInt16:
							{
							TUint16 value = 0;
							error = aPhoneBookBuffer.GetValue(value);
							iIndex = (error == KErrNone ? value : 0);
							break;
							}
						case CPhoneBookBuffer::EPhBkTypeInt32:
							{
							TUint32 value = 0;
							error = aPhoneBookBuffer.GetValue(value);
							iIndex = (error == KErrNone ? value : 0);
							break;
							}
						default:
							{
							error = KErrArgument;
							break;
							}
						}
					break; //RMobilePhoneBookStore::ETagPBAdnIndex
					}

				case RMobilePhoneBookStore::ETagPBText:
					{
					TPtrC16 text;
					error = GetText(text,tagType,aPhoneBookBuffer);
					if((error == KErrNone) && text.Length() > 0)
						{
						error = SetFirstName(text);
						}
					break; //RMobilePhoneBookStore::ETagPBText
					}
					
				case RMobilePhoneBookStore::ETagPBNumber:
					{
					TPtrC16 number;
					error = GetText(number,tagType,aPhoneBookBuffer);
					if((error == KErrNone) && number.Length() > 0)
						{
						if(gettingAdditionalNumber)
							{
							error = AddAdditionalNumber(number);
							gettingAdditionalNumber = EFalse;
							}
						else
							{
							error = SetDiallingNumber(number);
							}
						}
					break; //RMobilePhoneBookStore::ETagPBNumber
					}
				
				case RMobilePhoneBookStore::ETagPBEmailAddress:
					{
					TPtrC16 emailAddress;
					error = GetText(emailAddress,tagType,aPhoneBookBuffer);
					if((error == KErrNone) && emailAddress.Length() > 0)
						{
						error = AddEmailAddress(emailAddress);
						}
					break; //RMobilePhoneBookStore::ETagPBEmailAddress
					}
					
				case RMobilePhoneBookStore::ETagPBSecondName:
					{
					TPtrC16 secondName;
					error = GetText(secondName,tagType,aPhoneBookBuffer);
					if((error == KErrNone) && secondName.Length() > 0)
						{
						error = AddSecondName(secondName);
						}
					break; //RMobilePhoneBookStore::ETagPBNumber
					}
					
				case RMobilePhoneBookStore::ETagPBAnrStart:
					{
					gettingAdditionalNumber = ETrue;
					break; //RMobilePhoneBookStore::ETagPBNumber
					}
					
				default:
					{
					aPhoneBookBuffer.SkipValue(tagType);
					break;
					}
				} //switch(tag)
			}
		} //while loop
	
	if((error == KErrNone) && (newEntryCount == 0))
		{
		//no error but no new entry found
		error = KErrArgument;
		}
	
	if(error != KErrNone)
		{
		//convert all error codes to KErrArgument
		error = KErrArgument;
		}
	
	return error;
	}

/**
 * Helper function
 * 
 * Gets a the next TPtrC16
 * @param aText A TPtrC to the next text entry
 * @param aPhoneBookBuffer The phonebook buffer to read from
 * 
 * @return System wide error code.
 */
TInt CPhoneBookEntry::GetText(TPtrC16& aText, CPhoneBookBuffer::TPhBkTagType aTagType, CPhoneBookBuffer& aPhoneBookBuffer) const
	{

	TInt error = KErrNone;
	
	switch(aTagType)
		{
		case CPhoneBookBuffer::EPhBkTypeDes16:
			{
			error = aPhoneBookBuffer.GetValue(aText);
			break;
			}
			
		default:
			{
			error = KErrArgument;
			break;
			}
		}
	
	return error;

	}

/**
 * Helper function
 * 
 * @param aSize The size to be updated.
 * @param aDes The descriptor being added.
 */
void CPhoneBookEntry::DesSize(TInt& aSize, const TDesC& aDes) const
	{
	switch(aSize % 4)
		{
		case 0:
			aSize += 1;
			break;
		case 1:
			break;
		case 2:
			aSize += 3;
			break;
		case 3:
			aSize += 2;
			break;
		default:
			break;
		}
	aSize += 1 /*the tag*/ + 2 /*the length*/ +  aDes.Size();
	}

/**
 * @return The TLV length required for the CPhoneBookEntry's current state .
 */
EXPORT_C TInt CPhoneBookEntry::TlvLength() const
	{
	TInt size = 0;
		
	size += 1; //new entry tag
		
	size += 1; //index tag
	//tag TUint16
	size += 2;
	
	if(iFirstName.Length() > 0)
		{
		DesSize(size,iFirstName);
		}

	if(iDiallingNumber.Length() > 0)
		{
		DesSize(size,iDiallingNumber);
		}
	
	TInt i = 0;
	for(i = 0; i < iEmailAddresss.Count(); ++i)
		{
		if(iEmailAddresss[i].Length() > 0)
			{
			DesSize(size,iEmailAddresss[i]);
			}
		}
	
	for(i = 0; i < iSecondNames.Count(); ++i)
		{
		if(iSecondNames[i].Length() > 0)
			{
			DesSize(size,iSecondNames[i]);
			}
		}
	
	for(i = 0; i < iAdditionalNumbers.Count(); ++i)
		{
		if(iAdditionalNumbers[i].Length() > 0)
			{
			size += 1; //new number tag
			DesSize(size,iAdditionalNumbers[i]);
			}
		}
	
	size += 3; 
	//add this as CPhoneBookBuffer::PutTagAndValue must allow for the worst case padding
	//i.e. 3 "Also allow for the 1-byte tag and up to 3 zero padding bytes"
		
	return size;
	}

