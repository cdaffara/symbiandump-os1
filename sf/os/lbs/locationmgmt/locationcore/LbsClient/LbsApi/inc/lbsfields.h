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
* Name        : LbsFields.h
* Part of     : Mobile Location Framework/Location FW
* Interface   : Location Acquisition API
* Position data Fields manager
* Version     :
*
*/



#ifndef __LBSFIELDS_H__
#define __LBSFIELDS_H__

#include <e32std.h>
#include <lbsfieldids.h>

/*
 * This header file defines the helper classes used by HPositionGenericInfo
 * The client should not use this class to access any positioning related information.
 * Instead the cleitn should use the methods provided by HGenericPositionInfo.
 */

/**
 * Standard position field identifiers. It takes one of the enumerated  values of _TPositionFieldId
 * @publishedAll
 * @released
*/
typedef TUint16 TPositionFieldId;

/**
 * Array of standard position field identifiers
 * @publishedAll
 * @released
 */
typedef TPositionFieldId TPositionFieldIdList[];
class HPositionGenericInfo;


/**
 * This is an internal class to provide the interface for the
 * HPositionGenericInfo::GetValue() and HPositionGenericInfo::SetValue()
 * methods. Methods in this class are not intended to be called directly.
 * Clients should use HPositionGenericInfo::GetValue() and
 * HPositionGenericInfo::SetValue() methods.
 * @publishedAll
 * @released
 */
class PositionFieldManager
	{
public:
	/**
	 * Supported data types
	 */
	enum TFieldType
		{
		/// Undefined data type. This value should not be used.
		EUndefined,
		/// TInt8 datatype
		ETInt8,
		/// TInt16 datatype
		ETInt16,
		/// TInt32 datatype
		ETInt32,
		/// TInt64 datatype
		ETInt64,
		/// TUint8 datatype
		ETUint8,
		/// TUint16 datatype
		ETUint16,
		/// TUint32 datatype
		ETUint32,
		/// TReal32 datatype
		ETReal32,
		/// TReal64 datatype
		ETReal64,
		/// TTime datatype
		ETTime,
		/// Time specified in microseconds
		ETTimeIntervalMicroSeconds,
		/// 8-bit descriptor datatype
		ETDesC8,
		/// 16-bit descriptor datatype
		ETDesC16
		};

friend class HPositionGenericInfo;
private:
    /**
     * Set the value for a location field of data type TInt8 in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              TInt8 aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TInt8 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param [in] aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TInt8& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TInt16 in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              TInt16 aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TInt16 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [in] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TInt16& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TInt32 in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              TInt32 aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TInt32 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TInt32& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TInt64 in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              const TInt64& aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TInt64 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TInt64& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TUint8 in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              TUint8 aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TUint8 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TUint8& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TUint16 in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              TUint16 aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TUint16 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TUint16& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TUint32 in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              TUint32 aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TUint32 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TUint32& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TReal32 in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              TReal32 aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TReal32 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TReal32& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TReal64 in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              const TReal64& aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TReal64 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TReal64& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TDesC8 & TDes8 in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              const TDesC8& aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TDes8 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TDes8& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TDesC16 & TDes16 in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              const TDesC16& aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TDes16 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TDes16& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TPtrC8 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TPtrC8& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TPtrC16 from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TPtrC16& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TTime in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              const TTime& aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TTime from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TTime& aValue,
	                              const HPositionGenericInfo& aInfo);

    /**
     * Set the value for a location field of data type TTimeIntervalMicroSeconds in the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field to be copied into the HPositionGenericInfo.
     * @param aValue Value to be copied into the HPositionGenericInfo.
     * @param aInfo The HPositionGenericInfo object where this field needs to
     * be added.
     * @return KErrArgument if aFieldId is EPositionFieldNone.
     * @return KErrPositionBufferOverflow if aValue will not fit in the HPositionGenericInfo
     * object.
     * @return KErrOverflow if there are already KPositionMaxReturnableFields set in the
     * HPositionGenericInfo object.
     * @return KErrNone if the SetValue was successful.
     */
	IMPORT_C static TInt SetValue(TPositionFieldId aFieldId,
	                              const TTimeIntervalMicroSeconds& aValue,
	                              HPositionGenericInfo& aInfo);

    /**
     * Get the value for a location field of data type TTimeIntervalMicroSeconds from the
     * HPositionGenericInfo.
     * @param aFieldId Id of the field for which the value should be read from
     * HPositionGenericInfo.
     * @param [out] aValue The object where the data from the
     * HPositionGenericInfo is returned.
     * @param aInfo The HPositionGenericInfo object from where this field needs
     * to be read.
     * @return KErrNone is the operation was successful.
     * @return KErrNotFound if an object with aFieldId cannot be found in aInfo.
     */
	IMPORT_C static TInt GetValue(TPositionFieldId aFieldId,
	                              TTimeIntervalMicroSeconds& aValue,
	                              const HPositionGenericInfo& aInfo);
	};


#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
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
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

#endif //__LBSFIELDS_H__
