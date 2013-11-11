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




#ifndef LBSASSISTANCESTATICFUNCS_H
#define LBSASSISTANCESTATICFUNCS_H

#include <lbs/lbsassistanceenum.h>

class RDataBuilderBase;
class RDataReaderBase;
class RDataBuilderArrayBase;
class RDataReaderArrayBase;

//The methods in this header are not intended for clients to call explicitly.
/**
Data Builder Functions
The methods in this header are not intended for clients to call explicitly.
@publishedPartner
@released
*/
class DataBuilderFuncs
	{
public:
	/** SetField
	@param aBuilder RDataBuilderBase base class
	@param aFieldId id of the field we are interested in
	@param aData data to set in field
	 */
	IMPORT_C static void SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TInt& aData);
	/** SetField
	@param aBuilder RDataBuilderBase base class
	@param aFieldId id of the field we are interested in
	@param aData data to set in field
	 */
	IMPORT_C static void SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TDesC8& aData);
	/** SetField
	@param aBuilder RDataBuilderBase base class
	@param aFieldId id of the field we are interested in
	@param aData data to set in field
	 */
	IMPORT_C static void SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TUint& aData);
	/** SetField
	@param aBuilder RDataBuilderBase base class
	@param aFieldId id of the field we are interested in
	@param aData TDopplerUncertainty data to set in field
	 */
	IMPORT_C static void SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TDopplerUncertainty& aData);
	/** SetField
	@param aBuilder RDataBuilderBase base class
	@param aFieldId id of the field we are interested in
	@param aData TSatelliteStatus data to set in field
	 */
	IMPORT_C static void SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TSatelliteStatus& aData);
	/** SetField
	@param aBuilder RDataBuilderBase base class
	@param aFieldId id of the field we are interested in
	@param aData TUtranGpsDriftRate data to set in field
	 */
	IMPORT_C static void SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TUtranGpsDriftRate& aData);
	/** SetField
	@param aBuilder RDataBuilderBase base class
	@param aFieldId id of the field we are interested in
	@param aData TSfnTowUncertainty data to set in field
	 */
	IMPORT_C static void SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TSfnTowUncertainty& aData);
	/** Set Field
	@param aBuilder RDataBuilderBase base class
	@param aFieldId id of the field we are interested in
	@param aData TCodePhaseSearchWindow data to set in field
	 */
	IMPORT_C static void SetField(RDataBuilderBase& aBuilder, const TInt& aFieldId, const TCodePhaseSearchWindow& aData);
	/** Clear field
	@param aBuilder RDataBuilderBase base class
	@param aFieldId id of the field to clean
	 */
	IMPORT_C static void ClearField(RDataBuilderBase& aBuilder, const TInt& aFieldId);
	/** Clear all fields
	@param aBuilder RDataBuilderBase base class
	 */
	IMPORT_C static void ClearAllFields(RDataBuilderBase& aBuilder);
	/** Field exists
	@param aBuilder RDataBuilderBase base class
	@param aFieldId id of the field we are interested in
	 */
	IMPORT_C static TBool FieldExists(const RDataBuilderBase& aBuilder, const TInt& aFieldId);
	/** Any Field exists
	@param aBuilder RDataBuilderBase base class
	@return Bolean indicating if any field exists in the reader
	 */
	IMPORT_C static TBool AnyFieldExists(const RDataBuilderBase& aBuilder);
	/** Get field builder
	@param aParentBuilder RDataBuilderBase base class
	@param aFieldId id of the field we are interested in
	@param aChildBuilder child data builder
	 */
	IMPORT_C static void GetFieldBuilder(RDataBuilderBase& aParentBuilder, const TInt& aFieldId, RDataBuilderBase& aChildBuilder);
	/** Returns count
	@param aArrayBuilder RDataBuilderBase base class
	@return returns count in builder array
	 */
	IMPORT_C static TInt MaxCount(const RDataBuilderArrayBase& aArrayBuilder);
	};

//The methods in this header are not intended for clients to call explicitly.
/**
Data Reader Functions
The methods in this header are not intended for clients to call explicitly.
@publishedPartner
@released
*/	
class DataReaderFuncs
	{
public:
	/** GetField
	@param aReader RDataReaderBase base class
	@param aFieldId id of the field we are interested in
	@param aData data to read in field
	@return field
	 */
	IMPORT_C static TInt GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TInt& aData);
	/** GetField
	@param aReader RDataReaderBase base class
	@param aFieldId id of the field we are interested in
	@param aData data to read in field
	@return field
	 */
	IMPORT_C static TInt GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TDes8& aData);
	/** GetField
	@param aReader RDataReaderBase base class
	@param aFieldId id of the field we are interested in
	@param aData data to read in field
	@return field
	 */
	IMPORT_C static TInt GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TUint& aData);
	/** GetField
	@param aReader RDataReaderBase base class
	@param aFieldId id of the field we are interested in
	@param aData TDopplerUncertainty data to read in field
	@return field
	 */
	IMPORT_C static TInt GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TDopplerUncertainty& aData);
	/** GetField
	@param aReader RDataReaderBase base class
	@param aFieldId id of the field we are interested in
	@param aData TSatelliteStatus data to read in field
	@return field
	 */
	IMPORT_C static TInt GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TSatelliteStatus& aData);
	/** GetField
	@param aReader RDataReaderBase base class
	@param aFieldId id of the field we are interested in
	@param aData TUtranGpsDriftRate data to read in field
	@return field
	 */
	IMPORT_C static TInt GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TUtranGpsDriftRate& aData);
	/** GetField
	@param aReader RDataReaderBase base class
	@param aFieldId id of the field we are interested in
	@param aData TSfnTowUncertainty data to read in field
	@return field
	 */
	IMPORT_C static TInt GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TSfnTowUncertainty& aData);
	/** GetField
	@param aReader RDataReaderBase base class
	@param aFieldId id of the field we are interested in
	@param aData TCodePhaseSearchWindow data to read in field
	@return field
	 */
	IMPORT_C static TInt GetField(const RDataReaderBase& aReader, const TInt& aFieldId, TCodePhaseSearchWindow& aData);
	/** GetFieldReader
	@param aReader RDataReaderBase base class
	@param aFieldId id of the field we are interested in
	@param aDataReader Gets reader associated with this
	@return field id
	 */
	IMPORT_C static TInt GetFieldReader(const RDataReaderBase& aReader, const TInt& aFieldId, RDataReaderBase& aDataReader);
	/**  FieldExists
	@param aReader RDataReaderBase base class
	@param aFieldId id of the field we are interested in
	@param TBool for field exists status
	 */
	IMPORT_C static TBool FieldExists(const RDataReaderBase& aReader, const TInt& aFieldId);
	/**  AnyFieldExists
	@param aReader RDataReaderBase base class
	@param TBool for field exists status for any field in the reader
	 */
	IMPORT_C static TBool AnyFieldExists(const RDataReaderBase& aReader);
	/** MaxCount
	@param aReader RDataReaderBase base class
	@return Count for this reader array
	 */
	IMPORT_C static TInt MaxCount(const RDataReaderArrayBase& aReader);
	};
	
#endif //LBSASSISTANCESTATICFUNCS_H

