// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <lbscommon.h>
#include <lbsfields.h>
#include <lbspositioninfo.h>
#include <lbserrors.h>
#include "LbsInternal.h"

#define __ASSERT_ALIGNED_2BYTE(aPtr) __ASSERT_DEBUG(!(TUint(aPtr)&1),User::Panic(KPositionInternalFault, EBadAlignment))
#define __ASSERT_ALIGNED_4BYTE(aPtr) __ASSERT_DEBUG(!(TUint(aPtr)&3),User::Panic(KPositionInternalFault, EBadAlignment))

// Fix for GCCE 4.x : bug #1708 - GCC-E compilation error in lbs 
#if defined(_FOFF)
#undef _FOFF
#endif // defined(_FOFF)
#define _FOFF(c,f)			(((TInt)&(((c *)0x1000)->f))-0x1000)
// End of fix for GCCE 4.x

#define __DATA_OFFSET_INTO_HPOS(aOffset) \
		(_FOFF(HPositionGenericInfo, iBuffer[(aOffset+iInfo.iDataStartPoint)]))

const TInt KSizeOfTInt8 = sizeof(TInt8);
const TInt KSizeOfTInt16 = sizeof(TInt16);
const TInt KSizeOfTInt32 = sizeof(TInt32);
const TInt KSizeOfTInt64 = sizeof(TInt64);

//-----------------------------------------------------------------------------
// TPositionFieldSetter
//-----------------------------------------------------------------------------

/**
 * This function returns the size of a particular field in the buffer. For objects that
 * aren't always the same size (i.e. descriptors) the function has to look inside the
 * buffer to see how big the object is.
 *
 * @param aFieldIndex Field index for the object whos size we're interested in
 *
 * @return The size of the field pointed at by aFieldIndex. The size is the number of 8-bit 
 * quantities occupied by the object.
 */
TInt TPositionFieldSetter::FieldLength(TInt aFieldIndexId) const
	{
	const TPositionFieldIndex& fieldIndex = iInfo.FieldIndexPtr()[aFieldIndexId];
	switch(fieldIndex.FieldType())
		{
		case PositionFieldManager::ETInt8:
			return sizeof(TInt8);
		case PositionFieldManager::ETInt16:
			return sizeof(TInt16);
		case PositionFieldManager::ETInt32:
			return sizeof(TInt);
		case PositionFieldManager::ETInt64:
			return sizeof(TInt64);
		case PositionFieldManager::ETUint8:
			return sizeof(TUint8);
		case PositionFieldManager::ETUint16:
			return sizeof(TInt16);
		case PositionFieldManager::ETUint32:
			return sizeof(TInt32);
		case PositionFieldManager::ETReal32:
			return sizeof(TReal32);
		case PositionFieldManager::ETReal64:
			return sizeof(TReal64);
		case PositionFieldManager::ETTimeIntervalMicroSeconds:
			return sizeof(TTimeIntervalMicroSeconds);
		case PositionFieldManager::ETTime:
			return sizeof(TTime);
		case PositionFieldManager::ETDesC16:
			{
			const TUint8* address = &(iInfo.iBuffer[fieldIndex.FieldStartPos()]);
			const TUint lengthOfString = reinterpret_cast<const SPackedTDesC16*>(address)->iLength;
			return _FOFF(SPackedTDesC16, iBuf[lengthOfString]); 
			}
		case PositionFieldManager::ETDesC8:
			{
			const TUint8* address = &(iInfo.iBuffer[fieldIndex.FieldStartPos()]);
			const TUint lengthOfString = reinterpret_cast<const SPackedTDesC8*>(address)->iLength;
			return _FOFF(SPackedTDesC8, iBuf[lengthOfString]);
			}
		default:
			User::Panic(KPositionInternalFault, EUnknownDataType);
			return 0;	//needed as compiler doesn't understand panics.
		}
	}

/**
 * This function rounds up aOffset so that HPositionGenericInfo::iBuffer[aOffset] will
 * be suitable aligned for data of aFieldType, such that the copy operator for this 
 * data type will be able to be used at &HPositionGenericInfo::iBuffer[aOffset].
 *
 * @param aOffset Offset to be rounded up for alignment reasons.
 * @param aFieldType Type of data that will be placed at this offset.
 */
void TPositionFieldSetter::AlignOffset(TInt& aOffset,
                                      PositionFieldManager::TFieldType aFieldType) const
	{
	switch (aFieldType)
		{
		case PositionFieldManager::ETInt16:
		case PositionFieldManager::ETUint16:
		case PositionFieldManager::ETInt8:
		case PositionFieldManager::ETUint8:
		case PositionFieldManager::ETInt32:
		case PositionFieldManager::ETInt64:
		case PositionFieldManager::ETUint32:
		case PositionFieldManager::ETReal32:
		case PositionFieldManager::ETReal64:
		case PositionFieldManager::ETTime:
		case PositionFieldManager::ETTimeIntervalMicroSeconds:
			return;
		case PositionFieldManager::ETDesC8:
		case PositionFieldManager::ETDesC16:
			if(__DATA_OFFSET_INTO_HPOS(aOffset)&3)
				{
				aOffset += 4 - (__DATA_OFFSET_INTO_HPOS(aOffset)&3);	//Makes (HPositionGenericInfo::iBuffer[aOffset])&3 == 0
				}
			return;
		default:
			User::Panic(KPositionInternalFault, EUnknownDataType);
		}
	}

/**
 * See the design document for a full description of how this function works. The algorithm
 * goes something like this...
 * If an object is in the buffer with aFieldId, remove it from the index. No need to blank
 * the buffer as the contents of the buffer can only be accessed via the index anyway.
 * Then we find a empty slot in the index. This will be the first field in the index marked
 * with the field type PositionFieldManager::EUndefined. Then we look at where the previous
 * field ended and pretend we're going to stick this new field onto the end of that. If it 
 * fits then we do the copy. If it doesn't fit then we try compressing the contents of the 
 * buffer and seeing if we can get the field to fit then. If we still can't get it to fit
 * we return an error. Otherwise we copy the data into the buffer.
 *
 * @param aFieldId Id of the field to be copied into the buffer.
 * @param aData a wrapper round the object to be copied into the buffer. It contains 
 * information about the data type/size and a pointer to a copy method.
 * @return KErrNone if the operation was successful.
 * @return KErrPositionBufferOverflow if aData will not fit in the HPositionGenericInfo 
 * object's buffer.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 */
TInt TPositionFieldSetter::DoSetValue(TPositionFieldId aFieldId,
                                     const TDataWrapperC& aData)
	{

	if(aFieldId==EPositionFieldNone)
		{
		return KErrArgument;
		}

	ClearIdIfItAlreadyExists(aFieldId);

	TInt indexToInsertAt = 0;
	TInt err = FindFirstEmptyIndex(indexToInsertAt);

	if(err!=KErrNone)
		{
		return err;
		}
	
	TInt offsetOfNewData;

	TPositionFieldIndex* const fieldIndex = iInfo.FieldIndexPtr();

	if(indexToInsertAt!=0)
		{
		offsetOfNewData =  fieldIndex[indexToInsertAt-1].FieldStartPos()
						+ FieldLength(indexToInsertAt-1);
		}

	else
		{
		offsetOfNewData = iInfo.iDataStartPoint;	//First field always starts at start of buffer
		}

	AlignOffset(offsetOfNewData, aData.Type());
	
	TBool enoughSpace = EFalse;
	if(offsetOfNewData+aData.Size() <= iInfo.iTotalBufferSize)	//bounds check
		{
		enoughSpace = ETrue;
		}
	else
		{
		if (DoCompress())
			{
			// For DoCompress to succeed, there must be at least 1 field in the
			// buffer, and hence (indexToInsertAt > 0), hence we don't need to check
			// that this is the first field. i.e. we don't need to set offsetOfNewData
			// to iInfo.iDataStartPoint.
			offsetOfNewData =  fieldIndex[indexToInsertAt-1].FieldStartPos()
							+ FieldLength(indexToInsertAt-1);

			AlignOffset(offsetOfNewData, aData.Type());
		
			if(offsetOfNewData+aData.Size() <= iInfo.iTotalBufferSize)	//bounds check
				{
				enoughSpace = ETrue;
				}
			}
		}

	if(!enoughSpace)
		{
		return KErrPositionBufferOverflow;
		}

	//We know we have enough space and that offsetOfNewData points to correct offset
	//Now just need to copy the data.

	TUint8* address = &iInfo.iBuffer[offsetOfNewData];
	aData.CopyTo(address);
	TPositionFieldIndex& fieldIndexToSet = (fieldIndex[indexToInsertAt]);
	fieldIndexToSet.SetFieldId(aFieldId);
	fieldIndexToSet.SetFieldStartPos(static_cast<TUint16>(offsetOfNewData));
	fieldIndexToSet.SetFieldType(aData.Type());
	return KErrNone;
	}

//-----------------------------------------------------------------------------
// TPositionFieldGetter
//-----------------------------------------------------------------------------


/**
 * Function finds the index in aInfo that has the fieldId aFieldId. An assertion is made
 * that the data type of the the field is the same as that provided by aData. The data is
 * then copied into the object pointed at by aData.
 *
 * @param aFieldId FieldId of the object that we're interested in getting the value of.
 * @param aData Wrapper round the object we're interested in writing the data into.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @panic "LocationClient EHPositionGenericInfoMismatchDataType(0)" if aFieldId's data type 
 * doesn't match that pointed at by aData.
 */
TInt TPositionFieldGetter::DoGetValue(TPositionFieldId aFieldId,
                                     TDataWrapper& aData) const
	{
	if(aFieldId==EPositionFieldNone)
		{
		return KErrArgument;
		}
	TInt index = 0;
	TInt err = FindIndexFromFieldId(aFieldId, index);
	if(err!=KErrNone)
		{
		return err;
		}
	__ASSERT_ALWAYS(iInfo.FieldIndexPtr()[index].FieldType() == aData.Type(), User::Panic(KPosClientFault, EPositionGenericInfoMismatchDataType));
	const TUint8* address = &iInfo.iBuffer[iInfo.FieldIndexPtr()[index].FieldStartPos()];
	return aData.CopyFrom(address);
	}

//-----------------------------------------------------------------------------
// Copy methods
//-----------------------------------------------------------------------------

/**
 * @param aCopyFrom Address to copy from.
 * @param aCopyTo Address to copy to.
 */
TInt CopyFromTDesC8(TAny* aCopyTo, const TAny* aCopyFrom)
	{
	__ASSERT_ALIGNED_2BYTE(aCopyTo);
	const TDesC8* des = reinterpret_cast<const TDesC8*>(aCopyFrom);
	SPackedTDesC8* packedDesC8 = reinterpret_cast<SPackedTDesC8*>(aCopyTo);

	packedDesC8->iLength = des->Length();
	(void)Mem::Copy(packedDesC8->iBuf,des->Ptr(),des->Length());
	return KErrNone;
	}

/**
 * @param aCopyFrom Address to copy from.
 * @param aCopyTo Address to copy to.
 */
TInt CopyToTDes8(TAny* aCopyTo, const TAny* aCopyFrom)
	{
	__ASSERT_ALIGNED_4BYTE(aCopyTo);
	const SPackedTDesC8* packedDesC8 = reinterpret_cast<const SPackedTDesC8*>(aCopyFrom);
	TDes8* des = reinterpret_cast<TDes8*>(aCopyTo);
	if(des->MaxLength()<packedDesC8->iLength)
		{
		des->SetLength(0);
		return KErrOverflow;
		}
	des->Copy(packedDesC8->iBuf, packedDesC8->iLength);
	return KErrNone;
	}

/**
 * Method doesn't do a copy, it just makes the TPtrC8 point to the data
 * @param aCopyFrom Address to copy from.
 * @param aCopyTo Address to copy to.
 */
TInt CopyToTPtrC8(TAny* aCopyTo, const TAny* aCopyFrom)
	{
	const SPackedTDesC8* packedDesC8 = reinterpret_cast<const SPackedTDesC8*>(aCopyFrom);
	TPtrC8* ptr = reinterpret_cast<TPtrC8*>(aCopyTo);
	ptr->Set(packedDesC8->iBuf, packedDesC8->iLength);
	return KErrNone;
	}

/**
 * @param aCopyFrom Address to copy from.
 * @param aCopyTo Address to copy to.
 */
TInt CopyFromTDesC16(TAny* aCopyTo, const TAny* aCopyFrom)
	{
	__ASSERT_ALIGNED_4BYTE(aCopyTo);
	const TDesC16* des = reinterpret_cast<const TDesC16*>(aCopyFrom);
	SPackedTDesC16* packedDesC16 = reinterpret_cast<SPackedTDesC16*>(aCopyTo);

	packedDesC16->iLength = des->Length();
	(void)Mem::Copy(packedDesC16->iBuf,des->Ptr(),des->Length()*sizeof(TUint16));
	return KErrNone;
	}

/**
 * @param aCopyFrom Address to copy from.
 * @param aCopyTo Address to copy to.
 */
TInt CopyToTDes16(TAny* aCopyTo, const TAny* aCopyFrom)
	{
	__ASSERT_ALIGNED_4BYTE(aCopyTo);
	const SPackedTDesC16* packedDesC16 = reinterpret_cast<const SPackedTDesC16*>(aCopyFrom);
	TDes16* des = reinterpret_cast<TDes16*>(aCopyTo);

	if(des->MaxLength()<packedDesC16->iLength)
		{
		des->SetLength(0);
		return KErrOverflow;
		}

	des->Copy(packedDesC16->iBuf, packedDesC16->iLength);
	return KErrNone;
	}

/**
 * Method doesn't do a copy, it just makes the TPtrC8 point to the data
 * @param aCopyFrom Address to copy from.
 * @param aCopyTo Address to copy to.
 */
TInt CopyToTPtrC16(TAny* aCopyTo, const TAny* aCopyFrom)
	{
	const SPackedTDesC16* packedDesC16 = reinterpret_cast<const SPackedTDesC16*>(aCopyFrom);
	TPtrC16* ptr = reinterpret_cast<TPtrC16*>(aCopyTo);
	ptr->Set(packedDesC16->iBuf, packedDesC16->iLength);
	return KErrNone;
	}

/**
 * @param aCopyFrom Address to copy from.
 * @param aCopyTo Address to copy to.
 */
TInt CopyTInt8(TAny* aCopyTo, const TAny* aCopyFrom)
	{
	*(reinterpret_cast<TInt8*>(aCopyTo)) = *(reinterpret_cast<const TInt8*>(aCopyFrom));
	return KErrNone;
	}

/**
 * @param aCopyFrom Address to copy from.
 * @param aCopyTo Address to copy to.
 */
TInt CopyTInt16(TAny* aCopyTo, const TAny* aCopyFrom)
	{
	Mem::Copy(aCopyTo,aCopyFrom,sizeof(TInt16));
	return KErrNone;
	}

/**
 * @param aCopyFrom Address to copy from.
 * @param aCopyTo Address to copy to.
 */
TInt CopyTInt32(TAny* aCopyTo, const TAny* aCopyFrom)
	{
	Mem::Copy(aCopyTo,aCopyFrom,sizeof(TInt32));
	return KErrNone;
	}

/**
 * @param aCopyFrom Address to copy from.
 * @param aCopyTo Address to copy to.
 */
TInt CopyTInt64(TAny* aCopyTo, const TAny* aCopyFrom)
	{
	Mem::Copy(aCopyTo,aCopyFrom,sizeof(TInt64));
	return KErrNone;
	}

//-----------------------------------------------------------------------------
// PositionFieldManager
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// TInt8
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             TInt8 aValue,
                                             HPositionGenericInfo& aInfo)
	{
	const TDataWrapperC wrapper(ETInt8, sizeof(TInt8), &aValue, CopyTInt8);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TInt8& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	TDataWrapper wrapper(ETInt8, &aValue, CopyTInt8);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TInt16
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             TInt16 aValue,
                                             HPositionGenericInfo& aInfo)
	{
	const TDataWrapperC wrapper(ETInt16, sizeof(TInt16), &aValue, CopyTInt16);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TInt16& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	TDataWrapper wrapper(ETInt16, &aValue, CopyTInt16);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TInt32
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             TInt32 aValue,
                                             HPositionGenericInfo& aInfo)
	{
	const TDataWrapperC wrapper(ETInt32, sizeof(TInt32), &aValue, CopyTInt32);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TInt32& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	TDataWrapper wrapper(ETInt32, &aValue, CopyTInt32);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TInt64
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             const TInt64& aValue,
                                             HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TInt8)==KSizeOfTInt8);
	const TDataWrapperC wrapper(ETInt64, sizeof(TInt64), &aValue, CopyTInt64);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TInt64& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TInt8)==KSizeOfTInt8);
	TDataWrapper wrapper(ETInt64, &aValue, CopyTInt64);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TUint8
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             TUint8 aValue,
                                             HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TUint8)==KSizeOfTInt8);
	const TDataWrapperC wrapper(ETUint8, sizeof(TUint8), &aValue, CopyTInt8);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TUint8& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TUint8)==KSizeOfTInt8);
	TDataWrapper wrapper(ETUint8, &aValue, CopyTInt8);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TUint16
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             TUint16 aValue,
                                             HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TUint16)==KSizeOfTInt16);
	const TDataWrapperC wrapper(ETUint16, sizeof(TUint16), &aValue, CopyTInt16);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TUint16& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TUint16)==KSizeOfTInt16);
	TDataWrapper wrapper(ETUint16, &aValue, CopyTInt16);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TUint32
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             TUint32 aValue,
                                             HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TUint32)==KSizeOfTInt32);
	const TDataWrapperC wrapper(ETUint32, sizeof(TUint32), &aValue, CopyTInt32);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TUint32& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TUint32)==KSizeOfTInt32);
	TDataWrapper wrapper(ETUint32, &aValue, CopyTInt32);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TReal32
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             TReal32 aValue,
                                             HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TReal32)==KSizeOfTInt32);
	const TDataWrapperC wrapper(ETReal32, sizeof(TReal32), &aValue, CopyTInt32);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TReal32& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TReal32)==KSizeOfTInt32);
	TDataWrapper wrapper(ETReal32, &aValue, CopyTInt32);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TReal64
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             const TReal64& aValue,
                                             HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TReal64)==KSizeOfTInt64);
	const TDataWrapperC wrapper(ETReal64, sizeof(TReal64), &aValue, CopyTInt64);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TReal64& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TReal64)==KSizeOfTInt64);
	TDataWrapper wrapper(ETReal64, &aValue, CopyTInt64);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}


//-----------------------------------------------------------------------------
// TDes8
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             const TDesC8& aValue,
                                             HPositionGenericInfo& aInfo)
	{	
	const TDataWrapperC wrapper(ETDesC8, _FOFF(SPackedTDesC8, iBuf[aValue.Length()]), &aValue, CopyFromTDesC8);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 * @return KErrOverflow if the descriptor provided is too small.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TDes8& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	TDataWrapper wrapper(ETDesC8, &aValue, CopyToTDes8);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TDes16
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             const TDesC16& aValue,
                                             HPositionGenericInfo& aInfo)
	{
	const TDataWrapperC wrapper(ETDesC16, (_FOFF(SPackedTDesC16, iBuf[aValue.Length()])), &aValue, CopyFromTDesC16);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 * @return KErrOverflow if the descriptor provided is too small.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TDes16& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	TDataWrapper wrapper(ETDesC16, &aValue, CopyToTDes16);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TPtrC8 and TPtrC16
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TPtrC8& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	TDataWrapper wrapper(ETDesC8, &aValue, CopyToTPtrC8);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TPtrC16& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	TDataWrapper wrapper(ETDesC16, &aValue, CopyToTPtrC16);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TTimeIntervalMicroSeconds
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             const TTimeIntervalMicroSeconds& aValue,
                                             HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TTimeIntervalMicroSeconds)==KSizeOfTInt64);
	const TDataWrapperC wrapper(ETTimeIntervalMicroSeconds, sizeof(TTimeIntervalMicroSeconds), &aValue, CopyTInt64);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TTimeIntervalMicroSeconds& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TTimeIntervalMicroSeconds)==KSizeOfTInt64);
	TDataWrapper wrapper(ETTimeIntervalMicroSeconds, &aValue, CopyTInt64);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}

//-----------------------------------------------------------------------------
// TTime
//-----------------------------------------------------------------------------

/**
 * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
 * @param aValue Value to be copied into the HPositionGenericInfo.
 * @param aInfo The HPositionGenericInfo object to copy the data into.
 *
 * @return KErrArgument if aFieldId is EPositionFieldNone.
 * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo 
 * object.
 * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
 * HPositionGenericInfo object.
 * @return KErrNone if the SetValue was successful.
 */
EXPORT_C TInt PositionFieldManager::SetValue(TPositionFieldId aFieldId,
                                             const TTime& aValue,
                                             HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TTime)==KSizeOfTInt64);
	const TDataWrapperC wrapper(ETTime, sizeof(TTime), &aValue, CopyTInt64);
	TPositionFieldSetter fieldSetter(aInfo);
	return fieldSetter.DoSetValue(aFieldId, wrapper);
	}

/**
 * @param aFieldId Id of the field to be retrieved into aValue.
 * @param aValue Reference to the object who's value is to be set.
 * @param aInfo The HPositionGenericInfo object from which the data is to be retrieved.
 *
 * @return KErrNone is the operation was successful.
 * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
 */
EXPORT_C TInt PositionFieldManager::GetValue(TPositionFieldId aFieldId,
                                             TTime& aValue,
                                             const HPositionGenericInfo& aInfo)
	{
	POSITION_COMPILE_TIME_ASSERT(sizeof(TTime)==KSizeOfTInt64);
	TDataWrapper wrapper(ETTime, &aValue, CopyTInt64);
	TPositionFieldGetter getter(aInfo);
	return getter.DoGetValue(aFieldId, wrapper);
	}
