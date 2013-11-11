// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LBSPOSITIONINFO_H__
#define __LBSPOSITIONINFO_H__

#include <lbsposition.h>
#include <lbscommon.h>
#include <lbsfields.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
class TPositionFieldIndex;
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS


/**
The default size for a HPositionGenericInfo position information buffer

@publishedAll
@released
*/
const TInt KPositionGenericInfoDefaultBufferSize = 1024;

/**
The default number of requested fields for a HPositionGenericInfo 
position information buffer

@publishedAll
@released
*/
const TInt KPositionGenericInfoDefaultMaxFields = 32;

/**
Position update types

@publishedAll
@released
 */
enum _TPositionUpdateType
	{
	/** EPositionUpdateUnknown */
	EPositionUpdateUnknown = 0,
	/** EPositionUpdateGeneral */
	EPositionUpdateGeneral = 0x01
	};
/**
TPositionUpdateType code
@publishedAll
@released
 */
typedef TUint32 TPositionUpdateType;


/**
Position mode reasons. These values summarise the reasons why module
was unable to uses its preferred calculation mode when producing a 
position update. The values are a bit mask that can be combined
to describe the various reasons.

NetworkError is used when there is no network coverage or
the service is unavailable.

LocalError represents environmental reasons that prevent a terminal
based calculation - such as no satellites in view.

DataError is returned when there is a problem with the initialisation
data. For example, GPS assistance data was received but it was
either invalid or could not be used..

@publishedAll
@released
 */
enum _TPositionModeReason
	{
	/** Mode reason none */
	EPositionModeReasonNone         = 0,
	/** Mode reason network error */
	EPositionModeReasonNetworkError = 1 << 0,
	/** Mode reason local error */
	EPositionModeReasonLocalError   = 1 << 1,
	/** Mode reason data error */
	EPositionModeReasonDataError    = 1 << 2
	};

/**
TPositionModeReason code
@publishedAll
@released
 */
typedef TUint32 TPositionModeReason;


class TPositionInfoBase : public TPositionClassTypeBase
/**
This class provides the interface used by RPositioner to get a location fix,
and as such all classes used to get location info from the location server should
derive from this class.

Class contains info about the update type and the ID of the module that gave the 
location fix

@publishedAll
@released
 */
	{
public:
	IMPORT_C TPositionModuleId ModuleId() const;
	IMPORT_C void SetModuleId(TPositionModuleId aModuleId);

	IMPORT_C TPositionUpdateType UpdateType() const;
	IMPORT_C void SetUpdateType(TPositionUpdateType aUpdateType);

	IMPORT_C TPositionModuleInfo::TTechnologyType PositionMode() const;
	IMPORT_C void SetPositionMode(TPositionModuleInfo::TTechnologyType aMode);

	IMPORT_C TPositionModeReason PositionModeReason() const;
	IMPORT_C void SetPositionModeReason(TPositionModeReason aReason);

protected:
	IMPORT_C TPositionInfoBase();

protected:
	/** The Id of the module that gave the location fix. */
	TPositionModuleId iModuleId;
	/** The type of update that this location fix represents. */
	TUint32 iUpdateType;

	/** The positioning mode used to calculate this location fix. */
	TPositionModuleInfo::TTechnologyType iPositionMode;

	/** The reason why the prefered position mode was not used. */
	TPositionModeReason iPositionModeReason;

private:
	/** Unused variable for future expansion. */
	TUint8 iReserved[8];
	};


class TPositionInfo : public TPositionInfoBase
/**
Standard class for getting a TPosition location fix from the location server.

@publishedAll
@released
 */
	{
public:
	IMPORT_C TPositionInfo();

	IMPORT_C void GetPosition(TPosition& aPosition) const;
	IMPORT_C void SetPosition(const TPosition& aPosition);

protected:
	/** The TPosition. */
	TPosition iPosition;
	};


class TPositionCourseInfo : public TPositionInfo
/**
Class for getting a TCourse from the location server.

@publishedAll
@released
 */
	{
public:
	IMPORT_C TPositionCourseInfo();

	IMPORT_C void GetCourse(TCourse& aCourse) const;
	IMPORT_C void SetCourse(const TCourse& aCourse);

protected:
	/** The TCourse. */
	TCourse iCourse;
	};


/**
Class for getting arbitrary positioning related information back from the location server. 
The client can set upto KPositionMaxRequestedFields as the number of requested fields.
The definitions of the fields are given in the enum #_TPositionFieldId. The client must 
know what data type will be returned for each of the fields. The server will make a best 
attempt to fill in the requested fields.

The client needs to allocate a big enough buffer to store the information it is requesting.
In order to be sure of getting back all the information the client must know and allocate 
memory considering the return value for each requested field.

@publishedAll
@released
 */
class HPositionGenericInfo : public TPositionInfo
	{
public:

	IMPORT_C static HPositionGenericInfo* New(TInt aBufferSize = KPositionGenericInfoDefaultBufferSize,
											  TInt aMaxFields = KPositionGenericInfoDefaultMaxFields);
	IMPORT_C static HPositionGenericInfo* NewL(TInt aBufferSize = KPositionGenericInfoDefaultBufferSize,
											   TInt aMaxFields = KPositionGenericInfoDefaultMaxFields);
	IMPORT_C static HPositionGenericInfo* NewLC(TInt aBufferSize = KPositionGenericInfoDefaultBufferSize,
												TInt aMaxFields = KPositionGenericInfoDefaultMaxFields);
	IMPORT_C void ClearRequestedFields();
	IMPORT_C TInt SetRequestedField(TPositionFieldId aFieldId);
	IMPORT_C TInt SetRequestedFields(const TPositionFieldIdList aFieldIdList);
	IMPORT_C TInt IsRequestedField(TPositionFieldId aFieldId) const;
	IMPORT_C TPositionFieldId FirstRequestedFieldId() const;
	IMPORT_C TPositionFieldId NextRequestedFieldId(TPositionFieldId aFieldId) const;
	IMPORT_C TBool IsFieldAvailable(TPositionFieldId aFieldId) const;
	IMPORT_C void ClearPositionData();	//Required on server side
	IMPORT_C TInt BufferSize() const;
	IMPORT_C TInt MaxFields() const;

    /**
     * Used to store position information.
     * @param aFieldId Standard position field identifier. See _TPositionFieldId
     * @param [in] aValue The parameter aValue can be of the type 
     * TInt8, TInt16, TInt32, TInt64, TUint8, TUint16, TUint32, 
     * TReal32, TReal64, TTime, TTimeIntervalMicroSeconds, Des8, or Des16.
     * @return a symbian OS error code.
     * @return KErrNone on successful operation.
     * @return KErrPositionBufferOverflow if the data contained in the 
     * parameter aValue cannot be added to the class due to the buffer being too small.
     */
	template <class TType>
		inline TInt SetValue(TPositionFieldId aFieldId, const TType& aValue) 
			{return PositionFieldManager::SetValue(aFieldId, aValue, *this);}

    /**
     * Used to retrieve position information.
     * @param aFieldId Standard position field identifier. See _TPositionFieldId
     * @param [out] aValue The parameter aValue can be of the type 
     * TInt8, TInt16, TInt32, TInt64, TUint8, TUint16, TUint32, 
     * TReal32, TReal64, TTime, TTimeIntervalMicroSeconds, Des8, or Des16.
     * Panics with EPositionGenericInfoMismatchDataType if there is a 
     * mismatch in the data type of a field. 
     * @return a symbian OS error code.
     * @return KErrNone on successful operation.
     * @return KErrOverflow if the supplied descriptor is too short to contain the requested field.
     * This method must use the same data type as that assigned by SetValue() .
     */
	template <class TType>
		inline TInt GetValue(TPositionFieldId aFieldId, TType& aValue) const 
			{return PositionFieldManager::GetValue(aFieldId, aValue, *this);}

private:
	HPositionGenericInfo(TInt aDataBufferSize, TInt aMaxFields, 
		TUint aFieldIndexStartPoint, TUint aDataStartPoint, TBool aResetRequestedFields);
    /**
     * internal private method 
     */
	HPositionGenericInfo& operator=(const HPositionGenericInfo&);
    /**
     * internal private method 
     */
	HPositionGenericInfo(const HPositionGenericInfo&);

	TInt FindEmptyRequestedFieldOffset(TInt& aEmptyFieldOffset) const;

    /**
     * Helper method. For internal usage only 
     */
	inline TPositionFieldId* RequestedFieldPtr();
    /**
     * Helper method. For internal usage only 
     */
	inline const TPositionFieldId* RequestedFieldPtr() const;
    /**
     * Helper method. For internal usage only 
     */
	inline TPositionFieldIndex* FieldIndexPtr();
    /**
     * Helper method. For internal usage only 
     */
	inline const TPositionFieldIndex* FieldIndexPtr() const;

private:
	friend class TPositionFieldSetter;
	friend class TPositionFieldGetter;

	/**	Max number of requesable/returnable fields. */
	const TInt iMaxFields;

	/** Offset into iBuffer of the field index. */
	const TUint iFieldIndexStartPoint;

	/** Offset into iBuffer of the actual location data. */
	const TUint iDataStartPoint;

	/** Total size of the buffer */
	const TInt iTotalBufferSize;

	/** Start of the buffer. The heap cell allocated for an instance of this class
	has space for information up to iBuffer[iTotalBufferSize-1]*/
	TUint8 iBuffer[1];	//Must be at end of class
	};

#endif //__LBSPOSITIONINFO_H__
