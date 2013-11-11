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

/**
 @file
 @internalTechnology
*/

#include <bluetooth/eirdatacodec.h>
#include <utf.h>
#include <bluetooth/logger.h>
#include "eirmanclientlogger.h"

void Panic(TEirCodecPanics aCode)
	{
	User::Panic(KEirCodecPanicName, aCode);
	}

// Offset from Length to EIR Tag in EIR data
const TUint8 KEIRLengthToTagOffset = 1;
// Offset from EIR Tag to EIR data (value) in EIR data
const TUint8 KEIRTagToDataOffset = 1;
// number of bits in eir tag bit mask
const TUint8 KSizeOfEIRTagBitMask = 32;

/**
Extended Inquiry Response Parser Class
This class can take a TNameRecord reference from inquiry result, 
which could hold both local name and Extended Inquiry Response. 
It provides API to parse and return the local name and data for any Extended Inquiry Response tag.
*/

/** 
 Default Constructs an TExtendedInquiryResponseDataCodec object.
@internalTechnology
 */
EXPORT_C TExtendedInquiryResponseDataCodec::TExtendedInquiryResponseDataCodec()
: iEir(NULL, 0, 0), iOffset(0)
	{
	LOG_FUNC
	}

/** 
 Constructs an TExtendedInquiryResponseDataCodec object from an TNameRecord.

 TNameRecord.iName will be copied into a 8-bit descriptor and stored in the object.

 @param aDes Data buffer for extended inquiry response 
 @internalTechnology
*/
EXPORT_C TExtendedInquiryResponseDataCodec::TExtendedInquiryResponseDataCodec(const TNameRecord& aNameRecord)
: iEir(NULL, 0, 0), iOffset(0)
	{
	LOG_FUNC
	// We won't check if the descriptor is the standard EIR size, as it can be reformed to have a whole name.
	Set(aNameRecord);
	}

/** 
 Constructs an TExtendedInquiryResponseDataCodec object from an TNameRecord.

 TNameRecord.iName will be copied into a 8-bit descriptor and stored in the object.

 @param aDes Data buffer for extended inquiry response 
 @internalTechnology
*/
EXPORT_C TExtendedInquiryResponseDataCodec::TExtendedInquiryResponseDataCodec(TNameRecord& aNameRecord)
: iEir(NULL, 0, 0), iOffset(0)
	{
	LOG_FUNC
	// We won't check if the descriptor is the standard EIR size, as it can be reformed to have a whole name.
	Set(aNameRecord);
	}

/** 
 Constructs an TExtendedInquiryResponseDataCodec object from an 8-bit data buffer.

 A copy of the buffer is stored in the object.

 @param aDes Data buffer for extended inquiry response 
 @internalTechnology
*/
EXPORT_C TExtendedInquiryResponseDataCodec::TExtendedInquiryResponseDataCodec(const TDesC8& aDes)
:  iEir(NULL, 0, 0), iOffset(0)
	{
	LOG_FUNC
	Set(aDes);
	}
/** 
 Constructs an TExtendedInquiryResponseDataCodec object from an 8-bit data buffer.

 A copy of the buffer is stored in the object.

 @param aDes Data buffer for extended inquiry response 
 @internalTechnology
*/
EXPORT_C TExtendedInquiryResponseDataCodec::TExtendedInquiryResponseDataCodec(TDes8& aDes)
:  iEir(NULL, 0, 0), iOffset(0)
	{
	LOG_FUNC
	Set(aDes);
	}

/**
 Retrieve the EIR data for a particular Data Type 
 @param aDataType Data Type to look for
 @param aDes Pointer descriptor that will point to the EIR data for the Data Type requested if successful 
 (please note this does not contain the Data Type byte) 
 @return an error code
 @internalTechnology
 */
EXPORT_C TInt TExtendedInquiryResponseDataCodec::GetData(TExtendedInquiryResponseDataType aDataType, TPtrC8& aDes) const
	{
	LOG_FUNC
	TInt offset = NextDataType(0);

	while(offset >= KErrNone)
		{
		if(iEir[offset] == aDataType)
			{
			aDes.Set(iEir.Mid(offset + KEIRTagToDataOffset, iEir[offset - KEIRLengthToTagOffset] - KEIRLengthToTagOffset));
			return KErrNone;
			}
		else
			{
			offset = NextDataType(offset);
			}
		}

	return offset;
	}
/**
 Retrieve the EIR data for the next Data Type 
 @param aOffset Offset to current data type (0 to start traversing)
 @param aDes Pointer descriptor that will point to the EIR data for the Data Type requested if successful 
 (please note this does not contain the Data Type byte) 
 @return TExtendedInquiryResponseDataType (TExtendedInquiryResponseDataType::EEirInvalid means there is no more eir data to get)
 @internalTechnology
 */
EXPORT_C TExtendedInquiryResponseDataType TExtendedInquiryResponseDataCodec::GetNextData(TPtrC8& aDes)
	{
	LOG_FUNC
	TExtendedInquiryResponseDataType dataType;
	iOffset = NextDataType(iOffset);

	if(iOffset >= KErrNone && iEir.Length() >= (iOffset +iEir[iOffset - KEIRLengthToTagOffset]))
		{
		aDes.Set(iEir.Mid(iOffset + KEIRTagToDataOffset, iEir[iOffset-KEIRLengthToTagOffset] - KEIRLengthToTagOffset));
		dataType = static_cast<TExtendedInquiryResponseDataType>(iEir[iOffset]);
		}
	else
		{
		// Data must be malformed or corrupted
		dataType = EEirInvalid;
		}

	return dataType;
	}
	
/**
 Find out whether a particular Data Type is present in this TExtendedInquiryResponseDataCodec
 @param aDataType Data Type to look for
 @return ETrue if Data Type present, EFalse otherwise
 @internalTechnology
 */
EXPORT_C TBool TExtendedInquiryResponseDataCodec::IsDataTypePresent(TExtendedInquiryResponseDataType aDataType) const
	{
	LOG_FUNC
	TInt offset = NextDataType(0);
	TBool found = EFalse;

	while(offset >= KErrNone)
		{
		if(iEir[offset] == aDataType)
			{
			found = ETrue;
			break;
			}

		offset = NextDataType(offset);
		}

	return found;
	}

/**
 Obtain the Device Name from inquiry response. This may be a complete or partial name depending on what is available.
 @param aName is the Device Name converted into Unicode format
 @return TEIRDataCompleteness (Partial or Complete) or an error code
 @internalTechnology
 */
EXPORT_C TInt TExtendedInquiryResponseDataCodec::GetDeviceName(TPtrC8& aName) const
	{
	LOG_FUNC
	TInt completeness = EDataComplete;
	TInt error = GetData(EEirLocalNameComplete, aName);
	if(error != KErrNone)
		{
		error = GetData(EEirLocalNamePartial, aName);
		completeness = EDataPartial;
		}

	return error == KErrNone ? completeness : error;
	}

/**
 Set the Device Name in the EIR buffer from a TPtrC8. This may be a complete or partial name.
 @param aName is the Device Name converted into Unicode format
 @param aIsComplete is marking if the name is complete or partial
 @return TInt an error code
 @internalTechnology
 */
EXPORT_C TInt TExtendedInquiryResponseDataCodec::SetDeviceName(const TPtrC8& aName, TBool aIsComplete)
	{
	TPtrC8 name;
	TInt error = KErrNotFound;
	TInt offset = NextDataType(0);
	TInt nameTag = (aIsComplete ? EEirLocalNameComplete : EEirLocalNamePartial);
	TBool replaceCurrentName = ETrue;

	while(offset >= KErrNone)
		{
		if(iEir[offset] == EEirLocalNameComplete || iEir[offset] == EEirLocalNamePartial)
			{
			name.Set(iEir.Mid(offset + KEIRTagToDataOffset, iEir[offset-KEIRLengthToTagOffset] - KEIRLengthToTagOffset));
			if(iEir[offset] == EEirLocalNameComplete && name.Compare(aName) == 0)
				{
				// The only scenario we don't want to replace the existing name is when
				// it's complete and same as the new one (aName)
				replaceCurrentName = EFalse;
				LOG(_L("We won't replace the current name"));
				}
			error = KErrNone;
			break;
			}
		else
			{
			offset = NextDataType(offset);
			}
		}

	if(error == KErrNotFound)
		// no device name present, we will add the name to the end of the eir data
		{
		if(iEir.MaxLength() < (iEir.Length() + KEIRTagToDataOffset + KEIRLengthToTagOffset + aName.Length()))
			{
			// Not enough space to store this name and its length & tag
			return KErrNoMemory;
			}
		// Add length
		iEir.Append(aName.Length() + KEIRTagToDataOffset);
		// Append Tag
		iEir.Append(nameTag);
		// Append value
		iEir.Append(aName);
		LOG1(_L("EIR data with name appended: %d bytes of data"), iEir.Size());
		LOGHEXDESC(iEir);
		error = KErrNone;
		}
	else if(replaceCurrentName)
		// device name exists in current eir and it's either partial or different from aName, we'll update it with aName
		{
		// move all the data on the right of device name to the left and then append new name after it
		TPtr8 rightPtr = iEir.RightTPtr(iEir.Length() - offset - name.Length() - KEIRLengthToTagOffset);
		iEir.Replace(offset-KEIRLengthToTagOffset, iEir.Length() - offset + KEIRLengthToTagOffset, rightPtr);
		iEir.SetLength(offset + rightPtr.Length() - KEIRTagToDataOffset);
		// Check if the new name is too big for iEir
		if(iEir.MaxLength() < (iEir.Length() + KEIRTagToDataOffset + KEIRLengthToTagOffset + aName.Length()))
			{
			// Not enough space to store this name and its length & tag
			return KErrNoMemory;
			}
		// Add length
		iEir.Append(aName.Length() + KEIRTagToDataOffset);
		// Append Tag
		iEir.Append(nameTag);
		// Append value
		iEir.Append(aName);
		LOG(_L("Reshuffled EIR data:"));
		LOGHEXDESC(iEir);
		error = KErrNone;
		}
	// otherwise we do nothing, as this is the case of an identical complete name is already present.
	if(iNameRecord)
		{
		if(iEir.Length() > 0)
			{
			iNameRecord->iName.SetLength(((iEir.Length() + iEir.Length()%2)/2));
			}
		else
			{
			iNameRecord->iName.SetLength(0);
			}
		}
	return error;
	}

/**
 Private Setter, used by the constructor and the assignment operator
 @param aNameRecord TNameRecord to create extended inquiry response from
 */
EXPORT_C void TExtendedInquiryResponseDataCodec::Set(const TNameRecord& aNameRecord)
	{
	LOG_FUNC
	Set(const_cast<TNameRecord&>(aNameRecord));
	}

/**
 Private Setter, used by the constructor and the assignment operator
 @param aNameRecord TNameRecord to create extended inquiry response from
 */
EXPORT_C void TExtendedInquiryResponseDataCodec::Set(TNameRecord& aNameRecord)
	{
	LOG_FUNC
	TUint8* name = (TUint8*)(aNameRecord.iName.Ptr());
	TPtr8 namePtr(name, aNameRecord.iName.Size(), aNameRecord.iName.MaxSize());
	iNameRecord = &aNameRecord; 
	Set(namePtr);
	}

/**
 Private Setter, used by the constructor and the assignment operator
 @param aDes Data buffer to create extended inquiry response from
 */
EXPORT_C void TExtendedInquiryResponseDataCodec::Set(const TDesC8& aDes)
	{
	LOG_FUNC
	TPtr8 ptr(const_cast<TUint8*>(aDes.Ptr()), aDes.Length(), aDes.Length());
	Set(ptr);
	}
/**
 Private Setter, used by the constructor and the assignment operator
 @param aDes Data buffer to create extended inquiry response from
 */
EXPORT_C void TExtendedInquiryResponseDataCodec::Set(TDes8& aDes)
	{
	LOG_FUNC
	// Check length consistency
	TUint16 length = ComputeSignificantLength(aDes);
	if(length > KNameRecord8BitMaxLength)
		{
		// Reset the EIR and discard any data it may contain, since it was malformed anyway
		iEir.Set(NULL, 0, 0);
		}
	else
		{
		iEir.Set(const_cast<TUint8*>(aDes.Ptr()), length, aDes.MaxLength());
		}
	}

EXPORT_C void TExtendedInquiryResponseDataCodec::Copy(TDesC8& aDes)
	{
	__ASSERT_DEBUG(iEir.MaxLength() >= aDes.Length(), Panic(EEirCodecDataTooLarge));
	TUint16 length = ComputeSignificantLength(aDes);
	iEir.Copy(aDes);
	iEir.SetLength(length);
	iNameRecord->iName.SetLength((iEir.Length()/2)+1);
	}

EXPORT_C TInt TExtendedInquiryResponseDataCodec::DoSanityCheck(TDes8& aDes)
	{
	TInt error = KErrNone;
	TInt offset = NextDataType(0);
	TUint32 eirTagPresentFlag = 0;	// this is a bit mask for existing eir types
	TInt tag = 0;

	while(offset >= KErrNone)
		{
		if(!IsValideDataType(iEir[offset]))
			{
			// the next data type isn't a known(valid) eir tag
			error = KErrNotSupported;
			break;
			}
		if(static_cast<TInt>(iEir[offset - KEIRLengthToTagOffset]) < 0)
			{
			// length is less than 0
			error = KErrUnderflow;
			break;
			}
		tag = (iEir[offset] == EEirVendorSpecific ? KSizeOfEIRTagBitMask : iEir[offset]);
		// check if the tag is already present from previous parsing
		if(eirTagPresentFlag & (1 << tag))
			{
			error = KErrAlreadyExists;
			break;
			}
		else
			{
			// set flag for this data tag
			eirTagPresentFlag |= (1 << tag);
			offset = NextDataType(offset);
			}
		}
	error = offset == KErrCorrupt ? offset : error;
	if(error != KErrNone)
		{
		// remove all the data after this LTV unit
		iEir.SetLength(offset - KEIRLengthToTagOffset);
		aDes.SetLength(offset - KEIRLengthToTagOffset);
		}
	return error;
	}

/**
 Computes the length of Significant part in an Extended Inquiry Response data.

 @param aDes Extended Inquiry Response data
 */
TUint16 TExtendedInquiryResponseDataCodec::ComputeSignificantLength(const TDesC8 &aDes)
	{
	LOG_FUNC
	TUint desLen = aDes.Length();
	const TUint8 *ptr = aDes.Ptr();
	TUint16 i = 0;
	while(i < desLen && ptr[i] != 0x00)
		{
		i += ptr[i] + KEIRLengthToTagOffset;
		}
	return i;
	}

/**
 @param aOffset Offset to current data type (0 to start traversing)
 @return an error code or Offset to the next EIR data type
 */
TInt TExtendedInquiryResponseDataCodec::NextDataType(TInt aOffset) const
	{
	LOG_FUNC
	// Check bounds
	TInt length = iEir.Length();
	TInt ret = KErrNotFound;
	
	// Check the values of aOffset and length are correct, and aOffset is not bigger than the length
	if(aOffset > 0 && aOffset <= length)
		{
		// Get the Offset for next data type, aDes[aOffset-1] holds the length for each EIR structure
		TInt newOffset = iEir[aOffset - KEIRLengthToTagOffset] + aOffset + KEIRLengthToTagOffset;
		if(newOffset > length)
			{
			if(newOffset != length + KEIRLengthToTagOffset)
				{
				ret = KErrCorrupt;
				}
			}
		else
			{
			ret = newOffset;
			}
		}
	
	// Offset for the first data type
	else if(aOffset == 0 && length > 0)
		{
		ret = KEIRLengthToTagOffset;
		}
	return ret;
	}

TBool TExtendedInquiryResponseDataCodec::IsValideDataType(TInt aDataType)
	{
	TBool ret = EFalse;
	if((aDataType >= EEirFlags && aDataType <= EEirOobSimplePairingRandomizerR) || aDataType == EEirVendorSpecific)
		{
		ret = ETrue;
		}
	return ret;
	}

