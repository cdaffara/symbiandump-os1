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

/** ClearAllFields */
inline void RDataBuilderBase::ClearAllFields()
	{DataBuilderFuncs::ClearAllFields(*this);}

/**
Returns if Data Builder Element Exists
@param aIndex id of element we are interested in
@return ETrue if element exists, EFalse otherwise 
 */
inline TBool RDataBuilderArrayBase::ElementExists(const TInt& aIndex) const
	{return DataBuilderFuncs::FieldExists(*this, aIndex);}

/**
Clears builder array element
@param aIndex id of element we wish to clear
 */	
inline void RDataBuilderArrayBase::ClearElement(const TInt& aIndex)
	{DataBuilderFuncs::ClearField(*this, aIndex);}


template<typename T, typename Base>
/**
RDataBuilder field exists implementation
@param aFieldId id of field we are interested in
@return TBool if exists
 */	
inline TBool RDataBuilder<T, Base>::FieldExists(const typename T::TFieldId& aFieldId) const
	{return DataBuilderFuncs::FieldExists(*this, static_cast<TInt>(aFieldId));}


template<typename T, typename Base>
/**
RDataBuilder field exists implementation
@param aFieldId id of field we are interested in
@return TBool if exists
 */	
inline TBool RDataBuilder<T, Base>::IsDataAvailable() const
	{return DataBuilderFuncs::AnyFieldExists(*this);}



template<typename T, typename Base>
/**
RDataBuilder clear field exists
@param aFieldId id of field we are interested in
 */
inline void RDataBuilder<T, Base>::ClearField(const typename T::TFieldId& aFieldId)
	{DataBuilderFuncs::ClearField(*this, static_cast<TInt>(aFieldId));}

template<typename T, typename Base>
/**
RDataBuilder array builder implementation
@param aFieldId id of field we are interested in
@param aArrayBuilder array  we are interested in
 */
inline void RDataBuilder<T, Base>::GetArrayBuilder(const typename T::TFieldId& aFieldId, RDataBuilderArrayBase& aArrayBuilder)
	{DataBuilderFuncs::GetFieldBuilder(*this, static_cast<TInt>(aFieldId), static_cast<RDataBuilderBase&>(aArrayBuilder));}

template<typename T, typename BuilderBase>
template<typename S>
/**
RDataBuilder field exists implementation
@param aFieldId id of field we are interested in
@param aDataBuilder field we are interested in
 */
inline void RDataBuilder<T, BuilderBase>::GetFieldBuilder(const typename T::TFieldId& aFieldId, RDataBuilder<S, RDataBuilderBase>& aDataBuilder)
	{DataBuilderFuncs::GetFieldBuilder(*this, static_cast<TInt>(aFieldId), static_cast<RDataBuilderBase&>(aDataBuilder));}
	
template<typename T>
/**
Set array element
@param aIndex id of element we are interested in
@param aData data to set in field we are interested in
 */
inline void RDataBuilderArray<T>::SetElement(const TInt& aIndex, const T& aData)
	{DataBuilderFuncs::SetField(*this, aIndex, aData);}

template<typename T>
/**
GetFieldBuilder field
@param aIndex index of field we are interested in
@param aDataBuilder data builder field we are interested in
 */
inline void RDataBuilderArray<T>::GetFieldBuilder(const TInt& aIndex, RDataBuilder<T, RDataBuilderBase>& aDataBuilder)
	{DataBuilderFuncs::GetFieldBuilder(*this, aIndex, aDataBuilder);}
	
template<typename T, typename BuilderBase>
template<typename S>
/**
Sets field in DataBuilder
@param aFieldId index of field we are interested in
@param aData data to set in field we are interested in
 */
inline void RDataBuilder<T, BuilderBase>::SetField(const typename T::TFieldId& aFieldId, const S& aData)
	{DataBuilderFuncs::SetField(*this, aFieldId, aData);}

/**
Returns maximum entries
@return Count within array
 */
inline TInt RDataBuilderArrayBase::MaxCount() const
	{return DataBuilderFuncs::MaxCount(*this);}
	
//

template<typename T, typename Base>

/**
Returns true if Data Builder Field Exists
@param aFieldId id of field we are interested in
@return ETrue if field exists, EFalse otherwise 
 */
inline TBool RDataReader<T, Base>::FieldExists(const typename T::TFieldId& aFieldId) const
	{return DataReaderFuncs::FieldExists(*this, static_cast<TInt>(aFieldId));}


template<typename T, typename Base>
/**
Returns true if any field exists in the Reader
@param aFieldId id of field we are interested in
@return TBool if exists
 */	
inline TBool RDataReader<T, Base>::IsDataAvailable() const
	{return DataReaderFuncs::AnyFieldExists(*this);}


template<typename T>
/**
Get array element
@param aIndex index of element we are interested in
@param aData data to read in field we are interested in
@return Standard Symbian Error code
 */
inline TInt RDataReaderArray<T>::GetElement(const TInt& aIndex, T& aData) const
	{return DataReaderFuncs::GetField(*this, aIndex, aData);}

template<typename T>
/**
GetFieldReader field exists implementation
@param aIndexid index of field we are interested in
@param aDataReader data reader we are interested in
@return Standard Symbian Error code
 */
inline TInt RDataReaderArray<T>::GetFieldReader(const TInt& aIndex, RDataReader<T, RDataReaderBase>& aDataReader) const
	{return DataReaderFuncs::GetFieldReader(*this, aIndex, aDataReader);}

template<typename T, typename BuilderBase>
template<typename S>
/**
GetField field exists implementation
@param aFieldId index of field we are interested in
@param aData data from reader we are interested in
@return field Id
 */
inline TInt RDataReader<T, BuilderBase>::GetField(const typename T::TFieldId& aFieldId, S& aData) const
	{return DataReaderFuncs::GetField(*this, aFieldId, aData);}

/**
Returns if element exists
@param aIndex index of exists we are interested in
@return ETrue if element exists, EFalse otherwise 
 */	
inline TBool RDataReaderArrayBase::ElementExists(const TInt& aIndex) const
	{return DataReaderFuncs::FieldExists(*this, aIndex);}

template<typename T, typename ReaderBase>

/**
ArrayReader array reader implementation
@param aFieldId id of field we are interested in
@param aArrayBuilder array  we are interested in
@return returns id
 */
inline TInt RDataReader<T, ReaderBase>::GetArrayReader(const typename T::TFieldId& aFieldId, RDataReaderArrayBase& aArrayReader) const
	{return DataReaderFuncs::GetFieldReader(*this, static_cast<TInt>(aFieldId), static_cast<RDataReaderBase&>(aArrayReader));}

template<typename T, typename ReaderBase>
template<typename S>
/**
FieldReader implementation
@param aFieldId id of field we are interested in
@param aDataReader we are interested in
@return returns id
 */
inline TInt RDataReader<T, ReaderBase>::GetFieldReader(const typename T::TFieldId& aFieldId, RDataReader<S, RDataReaderBase>& aDataReader) const
	{return DataReaderFuncs::GetFieldReader(*this, static_cast<TInt>(aFieldId), static_cast<RDataReaderBase&>(aDataReader));}

/**
Returns maximum entries
@return Count within reader array
 */	
inline TInt RDataReaderArrayBase::MaxCount() const
	{return DataReaderFuncs::MaxCount(*this);}
	

