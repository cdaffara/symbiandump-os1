/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Position data Fields manager
*
*/


#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS

#ifndef LBSFIELDINDEX_H
#define LBSFIELDINDEX_H

#include <e32std.h>
#include <lbsfields.h>

/**
 * This is an internal class used for indexing entries in HPositionGenericInfo.
 * Methods in this class are not intended to be called directly. Clients should use
 * HPositionGenericInfo::GetValue() and HPositionGenericInfo::SetValue() methods.
 * @internalTechnology
 * @released
 */
class TPositionFieldIndex
	{
public:
    /** Constructor */
	inline TPositionFieldIndex();

    /**
     * Get the field ID referred to by this object.
     * @return The Field Id. The Id is one of the enumerated values of
     * #_TPositionFieldId.
     */
	inline TPositionFieldId FieldId() const;
	/**
   	 * Get the offset in buffer where this field is contained.
	 * @return The offset in the buffer.
	 */
	inline TUint16 FieldStartPos() const;
	/**
     * Get the data type of this field.
	 * @return The data type of this field. The data type is one of the
	 * enumerated values of PositionFieldManager::TFieldType.
	 */
	inline PositionFieldManager::TFieldType FieldType() const;
    /**
     * Set the field ID referred to by this object.
     * @param aFieldId The Id of the field referred by this object. The Id is
     * one of the enumerated values of #_TPositionFieldId.
     */
	inline void SetFieldId(TPositionFieldId aFieldId);
	/**
	 * Set the offset in buffer where this field is contained.
	 * @param aFieldStartPos The offset in the buffer where this field is added.
	 */
	inline void SetFieldStartPos(TUint16 aFieldStartPos);
	/**
	 * Set the data type of this field.
	 * @param aFieldType The data type of this field. The data type is one of
	 * the enumerated values of PositionFieldManager::TFieldType.
	 */
	inline void SetFieldType(PositionFieldManager::TFieldType aFieldType);

private:
	/** Id of the field that this index refers to. */
	TPositionFieldId iFieldId;
	/** Offset from HPositionGenericInfo::iBuffer that the object referred to by
	this index starts at. */
	TUint16 iFieldStartPos;
	/** Type of the data that this index refers to. */
	PositionFieldManager::TFieldType iFieldType;
	};

#endif //LBSFIELDS_H

#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
