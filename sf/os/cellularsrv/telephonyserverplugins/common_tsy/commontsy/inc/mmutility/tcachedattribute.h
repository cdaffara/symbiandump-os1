// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/


#ifndef TCACHEDATTRIBUTE_H
#define TCACHEDATTRIBUTE_H

#include <e32base.h>

/**
This class hold an external attribute and its cache status and ability

*/

// cache ability
typedef enum 
	{
	ECacheUnknown, // Initial value of the caching ability
	ECacheDisabled, // Caching is disabled.
	ECacheEnabled, // Caching is enabled.
	ECacheLegacy // It is always assumed that cached value is up to date.
	} TCacheAbility;	

// cache status
typedef enum
	{
	ECacheDirty,	// no value cached
	ECacheClean		// a value is cached
	} TCacheStatus;

template <class T>
class TCachedAttribute
	{	
public:
	inline TCachedAttribute();

	inline const T& Value();
	inline void SetValue(const T& aValue);

	inline TCacheAbility CacheAbility() const;
	inline TCacheStatus CacheStatus() const;
	
	inline void SetCacheEnabled();
	inline void SetCacheDisabled();
	inline void SetCacheDirty();
	inline void SetCacheClean();
	inline TBool IsCacheValid() const;
	
protected:
	TCacheAbility iCacheAbility;
	TCacheStatus iCacheStatus;
	T iValue;
	};

/**
Constuctor : set the cache as Dirty
*/
template <class T>
TCachedAttribute<T>::TCachedAttribute()
	: iCacheAbility(ECacheUnknown), iCacheStatus(ECacheDirty) 
	{
	}

	
/**
Mark the cache status as enabled.
*/
template <class T>
void TCachedAttribute<T>::SetCacheEnabled()
	{
	iCacheAbility = ECacheEnabled;
	}

/**
Mark the cache status as disabled.
*/
template <class T>
void TCachedAttribute<T>::SetCacheDisabled()
	{
	iCacheAbility = ECacheDisabled;
	SetCacheDirty();
	}

/**
Mark the cache status as dirty
*/
template <class T>
void TCachedAttribute<T>::SetCacheDirty()
	{
	iCacheStatus = ECacheDirty;
	}

/**
Mark the cache status as clean
*/
template <class T>
void TCachedAttribute<T>::SetCacheClean()
	{
	iCacheStatus = ECacheClean;
	}

/**
Check if the cache is up to date

@return ETrue if cache is enabled and in clean state. EFalse otherwise.
*/
template <class T>
TBool TCachedAttribute<T>::IsCacheValid() const
	{
	return ( (iCacheAbility == ECacheEnabled) && (iCacheStatus == ECacheClean) );
	}
/**
Getter
@return the cache ability
*/
template <class T>
TCacheAbility TCachedAttribute<T>::CacheAbility() const
	{
	return iCacheAbility;
	}

/**
Getter
@return the cache status
*/
template <class T>
TCacheStatus TCachedAttribute<T>::CacheStatus() const
	{
	return iCacheStatus;
	}

/**
Getter
@return the attribute value
*/
template <class T>
const T& TCachedAttribute<T>::Value()
	{
	return iValue;
	}

/**
Setter: set the attribute value, mark the cache clean if it was enable
@param aValue the attribute value
*/
template <class T>
void TCachedAttribute<T>::SetValue(const T& aValue)
	{
	iValue = aValue;
	if (iCacheAbility == ECacheEnabled)
		iCacheStatus = ECacheClean;
	}

#endif // TCACHEDATTRIBUTE_H
