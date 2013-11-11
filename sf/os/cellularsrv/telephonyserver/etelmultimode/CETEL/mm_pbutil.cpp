// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <etelmm.h>		// for RMobilePhoneBookStore
#include "mpbutil.h"

const TUint8 KTagPadZeroValue = 0x00;

EXPORT_C CPhoneBookBuffer::CPhoneBookBuffer() : iMonitor(NULL,0,0)
/** The CPhoneBookBuffer library class is constructed using a simple, one-phase 
constructor because the CPhoneBookBuffer class does not allocate any member 
data on the heap. */
	{
	}

EXPORT_C void CPhoneBookBuffer::Set(TDes8* aBuf)
/** This member function sets the buffer's internal attribute iPtr to point to 
the supplied 8-bit buffer, which is owned by the caller of this function member.
	
@param aData A pointer to the buffer 
@capability None
*/
	{
	iPtr = aBuf;
	}

EXPORT_C TInt CPhoneBookBuffer::AddNewEntryTag()
/** This function member puts a new entry tag byte (ETagPBNewEntry) into the buffer 
 and sets iMonitor to point to the beginning of each new entry.

@return KErrNone if successful, or KErrOverflow if there is not enough space 
in the buffer.
@see RMobilePhoneBookStore::TMobilePBFieldTags 
@capability None
*/
	{
	// First check whether there is enough space to fit new entry tag
	if( (iPtr->MaxLength() - iPtr->Length()) > 1 )
		{
		AppendInt8(RMobilePhoneBookStore::ETagPBNewEntry);
		const TInt len = iPtr->Length();
		// Make sure iMonitor has been updated and it points to the beggining of the new entry
		iMonitor.Set(&(*iPtr)[len-1], iPtr->MaxLength() - len + 1, iPtr->MaxLength() - len + 1);
		return KErrNone;
		}
	else
		{
		return KErrOverflow;
		}
	}

EXPORT_C TInt CPhoneBookBuffer::AddNewNumberTag()
/** This function member puts a new number tag byte (ETagPBAnrStart) into the buffer. 


@return KErrNone if successful, or KErrOverflow if there is not enough space 
in the buffer.
@see RMobilePhoneBookStore::TMobilePBFieldTags 
@capability None
*/
	{
	// First check whether there is enough space to fit new entry tag
	if( (iPtr->MaxLength() - iPtr->Length()) > 1 )
		{
		AppendInt8(RMobilePhoneBookStore::ETagPBAnrStart);
		return KErrNone;
		}
	else
		{
		return KErrOverflow;
		}
	}


/**
 *  Puts a 8-bit integer value into buffer.
 *
 *  The valid tags for use with 8-bit integers are:
 *
 *  @li  RMobilePhoneBookStore::ETagPBBearerCap
 *  @li  RMobilePhoneBookStore::ETagPBEntryControl
 *  @li  RMobilePhoneBookStore::ETagPBHiddenInfo
 *  @li  RMobilePhoneBookStore::ETagPBTonNpi
 *  @li  RMobilePhoneBookStore::ETagPBUniqueId
 *  @li  RMobilePhoneBookStore::ETagPBEntryStatus
 *
 *  @param aTagValue  Tag byte to use.
 *  @param aInteger   Value to put in the buffer.
 *
 *  @return KErrNone if successful, KErrOverflow if there is not enough
 *          space in the buffer or KErrArgument if the parameter is invalid.
 *
 *  @capability None
 */
EXPORT_C TInt CPhoneBookBuffer::PutTagAndValue(TUint8 aTagValue, TUint8 aInteger)
	{
	//
	// Check that the tag type is valid for this type of value...
	//
	if (aTagValue != RMobilePhoneBookStore::ETagPBBearerCap  &&
		aTagValue != RMobilePhoneBookStore::ETagPBEntryControl  &&
		aTagValue != RMobilePhoneBookStore::ETagPBHiddenInfo  &&
		aTagValue != RMobilePhoneBookStore::ETagPBTonNpi  &&
		aTagValue != RMobilePhoneBookStore::ETagPBUniqueId  &&
		aTagValue != RMobilePhoneBookStore::ETagPBEntryStatus)
		{
		return KErrArgument;
		}

	// First need to ensure that aInteger can fit into the supplied buffer
	if((iPtr->MaxLength()-iPtr->Length()) > 2)
		{
		AppendInt8(aTagValue);
		AppendInt8(aInteger);
		return KErrNone;
		}
	else
		{
		return KErrOverflow;
		}
	}


/**
 *  Puts a 16-bit integer value into buffer.
 *
 *  The valid tags for use with 16-bit integers are:
 *
 *  @li  RMobilePhoneBookStore::ETagPBAdnIndex
 *
 *  @param aTagValue  Tag byte to use.
 *  @param aInteger   Value to put in the buffer.
 *
 *  @return KErrNone if successful, KErrOverflow if there is not enough
 *          space in the buffer or KErrArgument if the parameter is invalid.
 *
 *  @capability None
 */
EXPORT_C TInt CPhoneBookBuffer::PutTagAndValue(TUint8 aTagValue, TUint16 aInteger)
	{
	//
	// Check that the tag type is valid for this type of value...
	//
	if (aTagValue != RMobilePhoneBookStore::ETagPBAdnIndex)
		{
		return KErrArgument;
		}

	// First need to ensure that aInteger can fit into the supplied buffer
	if((iPtr->MaxLength()-iPtr->Length()) > 3)
		{
		AppendInt8(aTagValue);
		AppendInt16(aInteger);
		return KErrNone;
		}
	else
		{
		return KErrOverflow;
		}
	}


/**
 *  Puts a 32-bit integer value into buffer.
 *
 *  The valid tags for use with 32-bit integers are:
 *
 *  @li  None.
 *
 *  @param aTagValue  Tag byte to use.
 *  @param aInteger   Value to put in the buffer.
 *
 *  @return KErrNone if successful, KErrOverflow if there is not enough
 *          space in the buffer or KErrArgument if the parameter is invalid.
 *
 *  @capability None
 */
EXPORT_C TInt CPhoneBookBuffer::PutTagAndValue(TUint8 aTagValue, TUint32 aInteger)
	{
	//
	// No Tags support 32bit integers...
	//
	(void) aTagValue;
	(void) aInteger;
	
	return KErrArgument;

//	// First need to ensure that aInteger can fit into the supplied buffer
//	if((iPtr->MaxLength()-iPtr->Length()) > 5)
//		{
//		AppendInt8(aTagValue);
//		AppendInt32(aInteger);
//		return KErrNone;
//		}
//	else
//		{
//		return KErrOverflow;
//		}
	}


/**
 *  Puts a 16-bit descriptor value into buffer.
 *
 *  The valid tags for use with 16-bit descriptors are:
 *
 *  @li  RMobilePhoneBookStore::ETagPBSecondName
 *  @li  RMobilePhoneBookStore::ETagPBGroupName
 *  @li  RMobilePhoneBookStore::ETagPBEmailAddress
 *  @li  RMobilePhoneBookStore::ETagPBText
 *  @li  RMobilePhoneBookStore::ETagPBNumber
 *
 *  @param aTagValue  Tag byte to use.
 *  @param aData      Value to put in the buffer.
 *
 *  @return KErrNone if successful, KErrOverflow if there is not enough
 *          space in the buffer or KErrArgument if the parameter is invalid.
 *
 *  @capability None
 */
EXPORT_C TInt CPhoneBookBuffer::PutTagAndValue(TUint8 aTagValue, const TDesC16 &aData)
	{
	//
	// Check that the tag type is valid for this type of value...
	//
	if (aTagValue != RMobilePhoneBookStore::ETagPBSecondName  &&
		aTagValue != RMobilePhoneBookStore::ETagPBGroupName  &&
		aTagValue != RMobilePhoneBookStore::ETagPBEmailAddress  &&
		aTagValue != RMobilePhoneBookStore::ETagPBText  &&
		aTagValue != RMobilePhoneBookStore::ETagPBNumber)
		{
		return KErrArgument;
		}

	// First need to ensure that aData can fit into the supplied buffer
	// Need to use Size() because this is Unicode data and we want to know number of bytes not number of characters
	// Also allow for the 1-byte tag and up to 3 zero padding bytes
	if((aData.Size()+3 <= KMaxUint16Val) && ((iPtr->MaxLength()-iPtr->Length()) > (aData.Size()+4)))
		{
		// Make sure buffer is word aligned after adding 3 bytes for tag and length
		switch(iPtr->Size()%4)
			{
		case 0:
			AppendInt8(KTagPadZeroValue);
			break;
		case 1:
			break;
		case 2:
			AppendInt8(KTagPadZeroValue);
			AppendInt8(KTagPadZeroValue);
			AppendInt8(KTagPadZeroValue);
			break;
		case 3:
			AppendInt8(KTagPadZeroValue);
			AppendInt8(KTagPadZeroValue);
			break;
		default:
			break;
			}

		AppendInt8(aTagValue);
		TUint16 len=(TUint16) aData.Size();
		iPtr->Append((const TUint8*)&len,2);
		TUint8* dataPtr=reinterpret_cast<TUint8*>(const_cast<TUint16*>(aData.Ptr()));
		iPtr->Append(dataPtr,len);
		return KErrNone;
		}
	else 
		{
		return KErrOverflow;
		}	
	}


/**
 *  Puts a 8-bit descriptor value into buffer.
 *
 *  The valid tags for use with 8-bit descriptors are:
 *
 *  @li  None.
 *
 *  @param aTagValue  Tag byte to use.
 *  @param aData      Value to put in the buffer.
 *
 *  @return KErrNone if successful, KErrOverflow if there is not enough
 *          space in the buffer or KErrArgument if the parameter is invalid.
 *
 *  @capability None
 */
EXPORT_C TInt CPhoneBookBuffer::PutTagAndValue(TUint8 aTagValue, const TDesC8 &aData)
	{
	//
	// No Tags support 8bit text...
	//
	(void) aTagValue;
	(void) aData;
	
	return KErrArgument;

//	// First need to ensure that aData can fit into the supplied buffer
//	if((aData.Size() <= KMaxUint16Val) && ((iPtr->MaxLength()-iPtr->Length()) > (aData.Size()+1)))
//		{
//		AppendInt8(aTagValue);
//		TUint16 len=(TUint16) aData.Length();
//		iPtr->Append((const TUint8*)&len,2);
//		iPtr->Append(aData);
//		return KErrNone;
//		}
//	else 
//		{
//		return KErrOverflow;
//		}	
	}

EXPORT_C TInt CPhoneBookBuffer::RemovePartialEntry()
/**This function will rollback the buffer to the end of the last complete 
phonebook entry and remove any data after this.

@return KErrNone if successful. 
@capability None
*/
	{
	iPtr->SetLength(iPtr->MaxLength() - iMonitor.Length());
	iMonitor.FillZ();
	return KErrNone;
	}

EXPORT_C void CPhoneBookBuffer::StartRead()
/** This function member sets the CPhoneBookBuffer class' iRead member data to 
point to the start of the populated TLV buffer, ready to start data extraction 
from the buffer. 
@capability None
*/
	{
	iRead.Set(*iPtr);
	}

EXPORT_C TInt CPhoneBookBuffer::GetTagAndType(TUint8 &aTagValue, TPhBkTagType &aDataType)
/** This function member reads the next tag value from the buffer.

@param aTagValue On completion, the next Tag type from the buffer.
@param aDataType On completion, the data type associated with the tag value.
@return KErrNone if successful, or KErrNotFound if there are no more tags in 
the buffer. 
@capability None
*/
	{
	// Check we've not reached end of buffer - if we have then return
	TInt length = iRead.Length();
	
	if (length<=0)
		return KErrNotFound;

	// Extract all padding zero bytes until tag is found
	TInt i=0;
	do
		{
		aTagValue=iRead[i++];
		}
	while ((aTagValue==KTagPadZeroValue) && i<length);

	if (i < length)
		iRead.Set(iRead.Mid(i));
	else
		return KErrNotFound;

	switch(aTagValue) // set tag type according to the tag value
		{
		case RMobilePhoneBookStore::ETagPBNewEntry:
		case RMobilePhoneBookStore::ETagPBAnrStart:
			aDataType = EPhBkTypeNoData;
			break;

		case RMobilePhoneBookStore::ETagPBBearerCap:
		case RMobilePhoneBookStore::ETagPBEntryControl:
		case RMobilePhoneBookStore::ETagPBHiddenInfo:
		case RMobilePhoneBookStore::ETagPBTonNpi:
		case RMobilePhoneBookStore::ETagPBUniqueId:
		case RMobilePhoneBookStore::ETagPBEntryStatus:
			aDataType = EPhBkTypeInt8;
			break;

		case RMobilePhoneBookStore::ETagPBAdnIndex: 
			aDataType = EPhBkTypeInt16; 
			break;

		case RMobilePhoneBookStore::ETagPBSecondName:
		case RMobilePhoneBookStore::ETagPBGroupName:
		case RMobilePhoneBookStore::ETagPBEmailAddress:
		case RMobilePhoneBookStore::ETagPBText:
		case RMobilePhoneBookStore::ETagPBNumber:
			aDataType = EPhBkTypeDes16; 
			break;

		default:
			return KErrNotFound;
		}
	return KErrNone;
	}

EXPORT_C TInt CPhoneBookBuffer::GetValue(TUint16 &aInteger)
/** Gets a 16-bit integer value from the buffer.

@param aInteger On completion, the 16-bit value from the buffer.
@return KErrNone if successful. 
@capability None
*/
	{
	aInteger=(TUint16)((iRead[1]<<8)+iRead[0]);		// Dependant upon endianess
	iRead.Set(iRead.Mid(2));
	return KErrNone;
	}

EXPORT_C TInt CPhoneBookBuffer::GetValue(TUint8 &aInteger)
/** Gets a 8-bit integer value from the buffer.

@param aInteger On completion, the 8-bit value from the buffer.
@return KErrNone if successful. 
@capability None
*/
	{
	aInteger=(TUint8)(iRead[0]);		
	iRead.Set(iRead.Mid(1));
	return KErrNone;
	}

EXPORT_C TInt CPhoneBookBuffer::GetValue(TUint32 &aInteger)
/** Gets a 32-bit integer value from the buffer.

@param aInteger On completion, the 32-bit value from the buffer.
@return KErrNone if successful. 
@capability None
*/
	{
	TUint16 aMSW(0), aLSW(0);
	GetValue(aLSW);
	GetValue(aMSW);
	aInteger=(TUint32)((aMSW<<16)+aLSW);		// Dependant upon endianess
	return KErrNone;
	}

EXPORT_C TInt CPhoneBookBuffer::GetValue(TPtrC8 &aData)
/** Gets a 8-bit descriptor from the buffer. The read pointer is then incremented past the read value.

@param aData On completion, the 8-bit descriptor from the buffer.
@return KErrNone if successful. 
@capability None
*/
	{
	TUint16 len;
	len=(TUint16)((iRead[1]<<8)+iRead[0]);		// Dependant upon endianess
	aData.Set(iRead.Mid(2,len));
	iRead.Set(iRead.Mid(len+2));
	return KErrNone;
	}

EXPORT_C TInt CPhoneBookBuffer::GetValue(TPtrC16 &aData)
/** Gets a 16-bit descriptor from the buffer.

@param aData On completion, the 16-bit descriptor from the buffer. 
The read pointer is then incremented past the read value.
@return KErrNone if successful. 
@capability None
*/
	{
	TUint16 size=(TUint16)((iRead[1]<<8)+iRead[0]);		// Dependant upon endianess
	TUint16 len=(TUint16)(size/2);

	iRead.Set(iRead.Mid(2));

	TUint16* dataPtr=reinterpret_cast<TUint16*>(const_cast<TUint8*>(iRead.Ptr()));

	aData.Set(dataPtr,len);
	
	iRead.Set(iRead.Mid(size));
	return KErrNone;
	}

EXPORT_C TInt CPhoneBookBuffer::BufferLength()
/** This function member returns the length of the populated buffer.

@return The buffer length. 
@capability None
*/
	{
	return iPtr->Length();
	}

EXPORT_C TInt CPhoneBookBuffer::RemainingReadLength()
/** This function member returns the length of the remaining data to read in the 
populated buffer.

@return The length of the remaining data. 
@capability None
*/
	{
	return iRead.Length();
	}

EXPORT_C void CPhoneBookBuffer::SkipValue(TPhBkTagType aDataType)
/** This function member skips the next value in the buffer that is waiting to 
be read. Clients would use this function member if they do not support the 
field contained in the next data value.

@param aDataType The data type of the value to skip 
@capability None
*/
	{
	TUint16 size=0;
	switch (aDataType)
		{
	case EPhBkTypeInt8:
		iRead.Set(iRead.Mid(1));
		break;
	case EPhBkTypeInt16:
		iRead.Set(iRead.Mid(2));
		break;
	case EPhBkTypeInt32:
		iRead.Set(iRead.Mid(4));
		break;
	case EPhBkTypeDes8:
	case EPhBkTypeDes16:
		size=(TUint16)((iRead[1]<<8)+iRead[0]);		// Dependant upon endianess
		iRead.Set(iRead.Mid(2+size));
		break;
	default:
		// EPhBkTypeNoData caught here - no data to skip, so do nothing
		break;
		}
	}

TInt CPhoneBookBuffer::AppendInt8(TUint8 aInteger)
	{
	const TUint8* intAddr=(const TUint8*)&aInteger;
	iPtr->Append(intAddr,1);
	return KErrNone;
	}

TInt CPhoneBookBuffer::AppendInt16(TUint16 aInteger)
	{
	const TUint8* intAddr=(const TUint8*)&aInteger;
	iPtr->Append(intAddr,2);
	return KErrNone;
	}

TInt CPhoneBookBuffer::AppendInt32(TUint32 aInteger)
	{
	const TUint8* intAddr=(const TUint8*)&aInteger;
	iPtr->Append(intAddr,4);
	return KErrNone;
	}
