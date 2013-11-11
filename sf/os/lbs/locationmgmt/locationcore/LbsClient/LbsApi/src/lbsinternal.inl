/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#include <lbspositioninfo.h>
#include "lbsfieldindex.h"

inline TPositionFieldId* HPositionGenericInfo::RequestedFieldPtr()
	{
	return reinterpret_cast<TPositionFieldId*>(iBuffer);
	}
	
inline const TPositionFieldId* HPositionGenericInfo::RequestedFieldPtr() const
	{
	return reinterpret_cast<const TPositionFieldId*>(iBuffer);
	}

inline TPositionFieldIndex* HPositionGenericInfo::FieldIndexPtr()
	{
	return reinterpret_cast<TPositionFieldIndex*>(&(iBuffer[iFieldIndexStartPoint]));
	}

inline const TPositionFieldIndex* HPositionGenericInfo::FieldIndexPtr() const
	{
	return reinterpret_cast<const TPositionFieldIndex*>(&(iBuffer[iFieldIndexStartPoint]));
	}


//-----------------------------------------------------------------------------
// TDataWrapperC
//-----------------------------------------------------------------------------

inline TDataWrapperC::TDataWrapperC(PositionFieldManager::TFieldType aType,
                                    TInt aSize,
                                    const TAny* aObject,
                                    TInt (*aCopyFunction)(TAny* aTrg, const TAny* aSrc))
/**
 * This class is provided as a wrapper round a const object.
 * @param aType Enum of the class type of the data pointed to by this.
 * @param aSize The size of the object to be copied.
 * @param aObject A pointer to the object to be copied
 * @param aCopyFunction A function suitable for copying this object to arbitrary address
 * assuming address is suitably aligned.
 */
:	iType(aType),
	iSize(aSize),
	iObject(aObject),
	iCopyFunction(aCopyFunction)
	{}


inline PositionFieldManager::TFieldType TDataWrapperC::Type() const 
/**
 * @return The data type being pointed to by this.
 */
	{return iType;}

inline TInt TDataWrapperC::Size() const
/**
 * @return The size of the object to be copied.
 */
	{return iSize;}

inline void TDataWrapperC::CopyTo(TAny* aCopyTo) const
/**
 * @param aCopyTo Copies the object that this is pointing to to aCopyTo.
 */
	{TInt err = iCopyFunction(aCopyTo, (this->iObject));
	__ASSERT_DEBUG(err==KErrNone, User::Panic(KPositionInternalFault, ECopyingIntoBufferGaveError));
	(void) err;}

//-----------------------------------------------------------------------------
// TDataWrapper
//-----------------------------------------------------------------------------

inline TDataWrapper::TDataWrapper(PositionFieldManager::TFieldType aType,
                                  TAny* aObject,
                                  TInt (*aCopyFunction)(TAny* aTrg, const TAny* aSrc))
/**
 * This class acts as a wrapper to an object that is to have it's value set from data
 * inside a HPositionGenericInfo object.
 *
 * @param aType Enum of the class type of the data pointed to by this.
 * @param aObject Pointer to the object to have its value set.
 * @param aCopyFunction A function suitable for copying from the HPositionGenericInfo
 * to the the object pointed to by aObject.
 */
:	iType(aType),
	iObject(aObject),
	iCopyFunction(aCopyFunction)
	{}

inline TInt TDataWrapper::CopyFrom(const TAny* aCopyFrom)
/**
 * @param aCopyFrom Copies data from aCopyFrom to the object this is a wrapper around.
 */
	{return iCopyFunction((this->iObject), aCopyFrom);}

inline PositionFieldManager::TFieldType TDataWrapper::Type() const 
/**
 * @return Returns the data type of the object this is a wrapper around.
 */
	{return iType;}

//-----------------------------------------------------------------------------
// TPositionFieldIndex
//-----------------------------------------------------------------------------

inline TPositionFieldIndex::TPositionFieldIndex()
/**
 * Default constructor. Sets the Id and start position to zero and the field type to
 * PositionFieldManager::EUndefined.
 */
:	iFieldId(0),
	iFieldStartPos(0),
	iFieldType(PositionFieldManager::EUndefined)
	{}

inline TPositionFieldId TPositionFieldIndex::FieldId() const
/**
 * @return Returns the field Id of the field that this index refers to.
 */
	{
	return iFieldId;
	}

inline TUint16 TPositionFieldIndex::FieldStartPos() const
/**
 * @return Returns the offset into HPositionGenericInfo::iBuffer of the start
 * of the field that this index refers to.
 */
	{
	return iFieldStartPos;
	}

inline PositionFieldManager::TFieldType TPositionFieldIndex::FieldType() const
/**
 * @return Returns the data type of the field that this index refers to.
 */
	{
	return iFieldType;
	}

inline void TPositionFieldIndex::SetFieldId(TPositionFieldId aFieldId)
/**
 * @param aFieldId Sets the field Id of the entry this index refers to.
 */
	{
	iFieldId = aFieldId;
	}

inline void TPositionFieldIndex::SetFieldStartPos(TUint16 aFieldStartPos)
/**
 * @param aFieldStartPos Sets the start position of the entry this index refers to.
 */
	{
	iFieldStartPos = aFieldStartPos;
	}

inline void TPositionFieldIndex::SetFieldType(PositionFieldManager::TFieldType aFieldType)
/**
 * @param aFieldType Sets the field data type of the entry this index refers to.
 */
	{
	iFieldType = aFieldType;
	}

//-----------------------------------------------------------------------------
// TPositionFieldSetter
//-----------------------------------------------------------------------------
// These methods are only ever called once, so it saves space to inline them.

TPositionFieldSetter::TPositionFieldSetter(HPositionGenericInfo& aInfo)
:	iInfo(aInfo)
/**
 * Constructor for a TPositionFieldSetter.
 * @param aInfo HPositionGenericInfo that is to have new data inserted into it.
 */
	{}

inline void TPositionFieldSetter::ClearIdIfItAlreadyExists(TPositionFieldId aFieldId)
/**
 * This method has the responsibility of 'shuffling' up the indices to the left if we 
 * remove one. It is important that the valid indices are all to the left of the index
 * and that all the null entries (those with field type PositionFieldManager::EUndefined)
 * are to the right of the index.
 * @param aFieldId Field Id of the index to remove if it exists.
 */
	{
	TPositionFieldIndex* const fieldIndex = iInfo.FieldIndexPtr();

	for(TInt i=0; i<iInfo.iMaxFields; i++)
		{
		if(fieldIndex[i].FieldId()==aFieldId)
			{
			Mem::Copy(&(fieldIndex[i]), &(fieldIndex[i+1]), 
				sizeof(TPositionFieldIndex)*(iInfo.iMaxFields-i-1));
			//We copy KPositionMaxReturnableFields-(i+1) fields since i starts from zero,
			//we need to add one extra so that it effectively goes from 1 to iMaxFields.
			fieldIndex[iInfo.iMaxFields-1] = TPositionFieldIndex();
			break;
			}
		}
	}

inline TInt TPositionFieldSetter::FindFirstEmptyIndex(TInt& aIndex) const
/**
 * Method searches for the first entry in the index to have data type
 * PositionFieldManager::EUndefined. 
 * @param aIndex Will be set to the the offset into aFieldIndex that has the first entry
 * of data type PositionFieldManager::EUndefined.
 * @return KErrNone if an index was successfully found.
 * @return KErrOverflow if there are no free spaces in the aFieldIndex array.
 */
	{
	const TPositionFieldIndex* const fieldIndex = iInfo.FieldIndexPtr();
	TInt err = KErrOverflow;
	for(TInt i=0; i<iInfo.iMaxFields; i++)
		{
		if(fieldIndex[i].FieldType()==PositionFieldManager::EUndefined)
			{
			aIndex=i;
			err=KErrNone;
			break;
			}
		}
	return err;
	}

inline TBool TPositionFieldSetter::DoCompress()
/**
 * This function shuffles all entries as far left as possible, but doesn't reorder the entries.
 * The algorithm is based on the fact that the pieces of data in the buffer are in the same
 * order as the indices in the index that refer to them - i.e. the offsets of the entries
 * referred to in aFieldIndex are strictly increasing as you iterate through the array.
 * @return ETrue if the compression managed to move anything.
 * @return EFalse if the compression was unable to compress the buffer further.
 */
	{
	TPositionFieldIndex* const fieldIndex = iInfo.FieldIndexPtr();
	TInt previousFieldEndPoint = iInfo.iDataStartPoint;	//Start of data section.
	TInt currentFieldStartPosition;
	TBool compressionActed = EFalse;
	TInt newStartPos;
	for(TInt i=0; i<iInfo.iMaxFields; i++)
		{
		if(fieldIndex[i].FieldType()==PositionFieldManager::EUndefined)
			{
			break;	//We've reached the end of the valid indices in the field array.
			}
		else
			{
			currentFieldStartPosition = fieldIndex[i].FieldStartPos();
			newStartPos = previousFieldEndPoint;
			if(previousFieldEndPoint<currentFieldStartPosition)
				{
				AlignOffset(newStartPos, fieldIndex[i].FieldType());
				if(newStartPos!=currentFieldStartPosition)	//We can move the data left.
					{
					Mem::Copy(&(iInfo.iBuffer[newStartPos]),
						&(iInfo.iBuffer[currentFieldStartPosition]),
						FieldLength(i));
					fieldIndex[i].SetFieldStartPos(static_cast<TUint16>(newStartPos));
					compressionActed = ETrue;
					}
				}
			previousFieldEndPoint = newStartPos + FieldLength(i);
			}
		}
	return compressionActed;
	}

//-----------------------------------------------------------------------------
// TPositionFieldGetter
//-----------------------------------------------------------------------------
// These methods are only ever called once, so it saves space to inline them.

TPositionFieldGetter::TPositionFieldGetter(const HPositionGenericInfo& aInfo)
:	iInfo(aInfo)
/**
 * Constructor for a TPositionFieldGetter.
 * @param aInfo HPositionGenericInfo that is to be searched for a piece of data.
 */
	{}

inline TInt TPositionFieldGetter::FindIndexFromFieldId(TPositionFieldId aFieldId,
                                                      TInt& aIndex) const
/**
 * Function takes a Field Id and finds the offset (aIndex) in the field index (aFieldIndex)
 * of the entry with that Id.
 * @param aFieldId Field Id to be searched for.
 * @param aIndex Is set to the offset into aFieldIndex of an entry with aFieldId
 * @return KErrNone if aFieldId was found in the index
 * @return KErrNotFound if aFieldId could not be found.
 */
	{
	const TPositionFieldIndex* const fieldIndex = iInfo.FieldIndexPtr();

	TInt err = KErrNotFound;
	for (TInt i=0; i<iInfo.iMaxFields; i++)
		{
		if(fieldIndex[i].FieldId()==aFieldId)
			{
			aIndex=i;
			err = KErrNone;
			break;
			}
		}
	return err;
	}
