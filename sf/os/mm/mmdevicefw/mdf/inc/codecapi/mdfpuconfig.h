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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef MDFPUCONFIG_H
#define MDFPUCONFIG_H

#include <e32base.h>
#include <mmf/server/taskconfig.h>

const TInt KUidTTaskConfig =  0x102737DF;
const TInt KUidTTaskConfig2 =  0x10273822 ; 
const TInt KUidPCMConfig =  0x102737E0;

/**
Configuration structure base class. 
Represents a configuration structure identified by a base class.
*/
class TPuConfig
	{
public:
	inline TUid Uid() const;
protected:
	inline TPuConfig();
	TUid iUid;
	};

/**
Template class to create a TPuConfig configuration given a structure and an unique UID to identify it.
*/
template<int U , class T>
class TPuConfigParam : public TPuConfig
	{
public:
	TPuConfigParam(const T& aParameterStructure);
	TPuConfigParam();
	operator T&();
	static const T* GetStructure(const TPuConfig& aConfig);	
	static T* GetStructure(TPuConfig& aConfig);	
private:
	TPckgBuf<T> iBuf;
	};

/**
Constructor
@param aParameterStructure The structure to store
*/	
template <int U, class T>
TPuConfigParam<U,T>::TPuConfigParam(const T& aParameterStructure)
	{
	iUid = TUid::Uid(U);
	iBuf = TPckgBuf<T>(aParameterStructure);
	}
	
template <int U, class T>
TPuConfigParam<U,T>::TPuConfigParam()
	{
	iUid = TUid::Uid(U);
	}

/** 
Operator to return the structure represented by the class.
@return	The structure represented by the class.
*/
template <int U, class T>
TPuConfigParam<U,T>::operator T&()
	{
	return iBuf();		
	}

/** 
Static method to return the structure represented by TPuConfig.
@param	A const reference to the base structure.
@return	A const pointer to the structure represented by the class.
*/
template <int U, class T>
const T* TPuConfigParam<U,T>::GetStructure(const TPuConfig& aConfig)
	{
	const T* ptr  = &((static_cast<const TPuConfigParam<U,T>& >(aConfig)).iBuf());
	return ptr;
	}
	
/** 
Static method to return the structure represented by TPuConfig.
@param	A reference to the base structure.
@return	A pointer to the structure represented by the class.
*/
template <int U, class T>
T* TPuConfigParam<U,T>::GetStructure(TPuConfig& aConfig)
	{
	T* ptr  = &((static_cast<TPuConfigParam<U,T>& >(aConfig)).iBuf());
	return ptr;
	}

/**
 Numeric data type.
*/
enum TDataType
	{
	/**
	Signed data
	*/
	ESigned,
	/** 
	Unsigned data
	*/
	EUnsigned
	};
	
/**
 Numeric endian type.
*/ 	
enum TEndian
	{
	/**
	Little endian data, (LSB to MSB)
	*/
	ELittleEndian,
	/** 
	Big endian data. (MSB to LSB)
	*/
	EBigEndian
	};

typedef TPuConfigParam<KUidTTaskConfig, TTaskConfig> TPuTaskConfig;
typedef TPuConfigParam<KUidTTaskConfig2, TTaskConfig2> TPuTaskConfig2;

#include <mdf/mdfpuconfig.inl>

#endif  // MDFPUCONFIG_H
