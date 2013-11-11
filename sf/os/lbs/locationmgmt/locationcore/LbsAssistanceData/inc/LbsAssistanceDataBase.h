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

#ifndef LBSASSISTANCEDATABASE_H
#define LBSASSISTANCEDATABASE_H

#include <e32std.h>
#include <lbs/lbsassistancestaticfuncs.h>


/**
Assistance Data bitfields
@publishedPartner
@released
*/
enum TLbsAssistanceDataItem
	{
	/** EAssistanceDataNone */
	EAssistanceDataNone = 0x00000000,
	/** EAssistanceDataAquisitionAssistance */
	EAssistanceDataAquisitionAssistance		= 0x00000001,
	/** EAssistanceDataBadSatList */
	EAssistanceDataBadSatList				= 0x00000002,
	/** EAssistanceDataNavigationModel */
	EAssistanceDataNavigationModel			= 0x00000004,
	/** EAssistanceDataReferenceTime */
	EAssistanceDataReferenceTime			= 0x00000008,
	/** EAssistanceDataIonosphericModel */
	EAssistanceDataIonosphericModel			= 0x00000010,
	/** EAssistanceDataDgpsCorrections */
	EAssistanceDataDgpsCorrections			= 0x00000020,
	/** EAssistanceDataReferenceLocation */
	EAssistanceDataReferenceLocation		= 0x00000040,
	/** EAssistanceDataAlmanac */
	EAssistanceDataAlmanac					= 0x00000080,
	/** EAssistanceDataPositioningGpsUtcModel */
	EAssistanceDataPositioningGpsUtcModel	= 0x00000100
	};

/**
@publishedPartner
@released
defines type for TLbsAsistanceDataGroup
 */
typedef TUint32 TLbsAsistanceDataGroup;

/**
@publishedPartner
@released
Fix for above spelling mistake
 */
typedef TLbsAsistanceDataGroup TLbsAssistanceDataGroup;

class TLbsAssistanceDataLayoutMap;

class TLbsAssistanceDataBody
	{
public:
	TLbsAssistanceDataBody(const TLbsAssistanceDataLayoutMap* aLayout)	: iLayout(aLayout) {}
public:
	const TLbsAssistanceDataLayoutMap* iLayout;
	TUint32* iDataPtr;
	TUint32* iIndexPtr;
	TInt iBitOffset;//Offset in bits into the index.
	TTime* iUtcTimeStampPtr;
	};

//

/**
Data Builder Base class
@publishedPartner
@released
*/
class RDataBuilderBase
	{
public:
	inline void ClearAllFields();
	//Private implementation functions
	TLbsAssistanceDataBody& Body();
	const TLbsAssistanceDataBody& Body() const;
protected:
	/**
	RDataBuilderBase constructor
	 */
	RDataBuilderBase(const TLbsAssistanceDataLayoutMap* aLayout);
protected:
	TLbsAssistanceDataBody iBody;
	/** reserved */
	TUint8 iReserved[12];
	};

/**
Data Builder Base class
@publishedPartner
@released
*/
class RDataBuilderRootBase : public RDataBuilderBase
	{
public:
	IMPORT_C TPtrC8 DataBuffer() const;	// For externalizing buffer for IPC.
	IMPORT_C TPtr8 DataBufferAccess(); // For copying another builder's buffer into this one.
	IMPORT_C void Close();
    IMPORT_C TBool IsOpen() const; // Returns ETrue if the builder is open.
	IMPORT_C const TTime& TimeStamp() const; // Returns iUtcTimeStamp;
	IMPORT_C void SetTimeStamp(const TTime& aUtcTime); // Set a time in iUtcTimeStamp;
protected:
	/**
	Open method
	 */	
	void DoOpenL(TInt aDataSize);
	/**
	RDataBuilderRootBase constructor
	 */	
	RDataBuilderRootBase(const TLbsAssistanceDataLayoutMap* aLayout);
protected:
	TUint32* iData;
	TInt iDataSize;
	/** reserved */
	TUint8 iReserved[24];
	};

class RDataBuilderArrayBase;
/**
Data Builder Array Base class
@publishedPartner
@released
*/
template<typename T, typename BuilderBase>
class RDataBuilder : public BuilderBase
	{
public:
	
	/**
	Returns if Data Builder Field Exists

	@param aFieldId id of field we are interested in
	@return TBool if true
	*/
	inline TBool FieldExists(const typename T::TFieldId& aFieldId) const;

	/**
	Returns if Any Data Builder Field Exists

	@return TBool if true
	*/
	inline TBool IsDataAvailable() const;

	/**
	GetFieldBuilder
	Get must get a RDataBuilder<S, RDataBuilderBase>, not a RDataBuilder<S, RDataBuilderRootBase> - hence 	the template as is...
	@param aFieldId id of field we are interested in
	@param aDataBuilder reference to RDataBuilderBase
	*/
	template<typename S>
		inline void GetFieldBuilder(const typename T::TFieldId& aFieldId, RDataBuilder<S, RDataBuilderBase>& aDataBuilder);	
	
	/**
	GetArrayBuilder

	Array builder
	@param aFieldId id of field we are interested in
	@param aArrayBuilder rereference to RDataBuilderArrayBase
	*/	
	inline void GetArrayBuilder(const typename T::TFieldId& aFieldId, RDataBuilderArrayBase& aArrayBuilder);

	/**
	Set field
	@param aFieldId id of field we are interested in
	@param aData data to insert
	*/
	template<typename S>
		inline void SetField(const typename T::TFieldId& aFieldId, const S& aData);

	/**
	Clear field
	@param aFieldId Id of field we are interested in
	*/
	inline void ClearField(const typename T::TFieldId& aFieldId);
protected:
	RDataBuilder(const TLbsAssistanceDataLayoutMap* aLayout);
private:
	/** reserved */
	TUint8 iReserved[32];
	};

/**
Data Builder Array Base class
@publishedPartner
@released
*/
class RDataBuilderArrayBase : public RDataBuilderBase
	{
public:
	/** Max count
	@return Returns count for this array
	*/
	inline TInt MaxCount() const;

	/**
	Element Exists
	@param aIndex Index of element to check
	@return returns ETrue if element exists, EFalse otherwise
	*/
	inline TBool ElementExists(const TInt& aIndex) const;

	/**
	Clear Element
	@param aIndex Index of element we are interested in
	*/
	inline void ClearElement(const TInt& aIndex);	
protected:
	/**
	RDataBuilderArrayBase: constructor
	 */
	RDataBuilderArrayBase(const TLbsAssistanceDataLayoutMap* aLayout);
private:
	/** reserved */
	TUint8 iReserved[32];
	};

template<typename T>	//template on the array type.

/**
Data Builder Array class
@publishedPartner
@released
*/
class RDataBuilderArray : public RDataBuilderArrayBase
	{
public:
	/**
	Get Field Builder
	@param aIndex id of field we are interested in
	@param aDataBuilder reference to RDataBuilderBase
	*/
	inline void GetFieldBuilder(const TInt& aIndex, RDataBuilder<T, RDataBuilderBase>& aDataBuilder);

	/**
	Set Element
	@param aIndex Index of element we are interested in
	@param aData data to set at Index
	*/
	inline void SetElement(const TInt& aIndex, const T& aData);
protected:
	RDataBuilderArray(const TLbsAssistanceDataLayoutMap* aLayout);
private:
	/** reserved */
	TUint8 iReserved[32];
	};


//

/**
Data Reader Base class
@publishedPartner
@released
*/
class RDataReaderBase
	{
public:
	//Implementation only functions
	TLbsAssistanceDataBody& Body();
	const TLbsAssistanceDataBody& Body() const;
protected:
	/**
	RDataReaderBase constructor
	 */
	RDataReaderBase(const TLbsAssistanceDataLayoutMap* aLayout);	//Class is abstract
protected:
	TLbsAssistanceDataBody iBody;
	/** reserved */
	TUint8 iReserved[12];
	};

/**
Data Reader Root Base class
@publishedPartner
@released
*/
class RDataReaderRootBase : public RDataReaderBase
	{
public:
	IMPORT_C TPtr8 DataBuffer();	//For use copying into from IPC
	IMPORT_C void Close();
	IMPORT_C TBool IsOpen() const; // Returns ETrue is the reader is open.
	IMPORT_C const TTime& TimeStamp() const; // eturns iUtcTimeStamp;
protected:
	/**
	Leaving Open method
	 */
	void DoOpenL(TInt aDataSize);
	/**
	RDataReaderRootBase constructor
	 */
	RDataReaderRootBase(const TLbsAssistanceDataLayoutMap* aLayout);
protected:
	TUint32* iData;
	TInt iDataSize;
	/** reserved */
	TUint8 iReserved[24];
	};

class RDataReaderArrayBase;

template<typename T, typename ReaderBase>

/**
Data Reader
@publishedPartner
@released
*/
class RDataReader : public ReaderBase
	{
public:
	/** Field Exists
	@param aFieldId index to check for field
	@return ETrue if element exists, EFalse otherwise
	*/
	inline TBool FieldExists(const typename T::TFieldId& aFieldId) const;

	/**
	Returns if Any Data Reader Field Exists

	@released
	@return TBool if true
	*/
	inline TBool IsDataAvailable() const;

	template<typename S>
	/**
	Get Field Reader
	Get must get a RDataReader<S, RDataReaderBase>, not a RDataReader<S, RDataReaderRootBase> - hence the 	template as is...
	@param aFieldId id of field we are interested in
	@param aDataReader reference to RDataBuilderBase
	@return Standard Symbian OS error code
	*/
		inline TInt GetFieldReader(const typename T::TFieldId& aFieldId, RDataReader<S, RDataReaderBase>& aDataReader) const;

	/**
	GetArrayReader
	@publishedPartner
	@released
	@param aFieldId id of field we are interested in
	@param aArrayReader reference to RDataReaderArrayBase
	@return Standard Symbian OS error code
	*/	
	inline TInt GetArrayReader(const typename T::TFieldId& aFieldId, RDataReaderArrayBase& aArrayReader) const;
		
	template<typename S>
	/** Get Field
	@param aFieldId index to check for field
	@param aData Element in field to get
	@return Standard Symbian OS error code
	*/
		inline TInt GetField(const typename T::TFieldId& aFieldId, S& aData) const;

protected:
	RDataReader(const TLbsAssistanceDataLayoutMap* aLayout);
private:
	/** reserved */
	TUint8 iReserved[32];
	};

/**
Data Reader Array Base class
@publishedPartner
@released
*/
class RDataReaderArrayBase : public RDataReaderBase
	{
public:
	/** Max count
	@return Returns count for this array
	*/
	inline TInt MaxCount() const;
	/** ElementExists
	@param aIndex index to check for element
	@return ETrue if element exists, EFalse otherwise
	*/
	inline TBool ElementExists(const TInt& aIndex) const;
protected:
	/**
	RDataReaderArrayBase constructor
	 */
	RDataReaderArrayBase(const TLbsAssistanceDataLayoutMap* aLayout);
private:
	/** reserved */
	TUint8 iReserved[32];
	};

template<typename T>

/**
Data Reader Array Base class
@publishedPartner
@released
*/
class RDataReaderArray : public RDataReaderArrayBase
	{
public:

	/** GetFieldReader
	@param aIndex index to check for element
	@param aDataReader reference to a data reader
	@return Standard Symbian OS error code
	*/
	inline TInt GetFieldReader(const TInt& aIndex, RDataReader<T, RDataReaderBase>& aDataReader) const;

	/** GetElement
	@param aIndex index to check for element
	@param aData Element at index to get
	@return Standard Symbian OS error code
	*/
	inline TInt GetElement(const TInt& aIndex, T& aData) const;
protected:
	RDataReaderArray(const TLbsAssistanceDataLayoutMap* aLayout);
private:
	/** reserved */
	TUint8 iReserved[32];
	};	


//


#include <lbs/lbsassistancedatabase.inl>

#endif //LBSASSISTANCEDATABASE_H

