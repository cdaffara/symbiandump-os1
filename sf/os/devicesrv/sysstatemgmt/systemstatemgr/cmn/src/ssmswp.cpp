// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ssm/ssmswp.h>

/**
Constructor taking the property key and value
@param aKey The key that identifies the System Wide Property
@param aValue The value to be assigned to the System Wide Property
*/	
EXPORT_C TSsmSwp::TSsmSwp(TUint aKey, TInt32 aValue) 
	: iKey(aKey), iValue(aValue)
	{
	} //lint !e1746 Suppress parameter 'aKey' could be made const reference

/**
Copy constructor.
@param aSwp The existing object who's key and value will be assigned to this object.
*/	
EXPORT_C TSsmSwp::TSsmSwp(const TSsmSwp& aSwp)
	: iKey(aSwp.iKey), iValue(aSwp.iValue)
	{		
	}

/**
Set this object.
@param aKey The key that identifies the System Wide Property
@param aValue The value to be assigned to the System Wide Property
*/	
EXPORT_C void TSsmSwp::Set(TUint aKey, TInt32 aValue)
	{
	iKey = aKey;
	iValue = aValue;
	} //lint !e1746 Suppress parameter 'aKey' could be made const reference

/**
@return This objects key
*/	
EXPORT_C TUint TSsmSwp::Key() const
	{
	return iKey;
	}

/**
@return This objects value
*/
EXPORT_C TInt32 TSsmSwp::Value() const
	{
	return iValue;
	}

/**
@param aSwp The existing object who's key and value will be assigned to this object.
@return A reference to this object.
*/
EXPORT_C TSsmSwp& TSsmSwp::operator=(const TSsmSwp& aSwp)
	{
	// protect against self assignment
	if( &aSwp == this)
		{
		return(*this);
		}
		
	// do assigment
	iKey = aSwp.iKey;
	iValue = aSwp.iValue;	
	return(*this);
	}

/**
@param aSwp The object to be compared.
@return True, if the key and value are equal; false otherwise.
*/
EXPORT_C TBool TSsmSwp::operator==(const TSsmSwp& aSwp) const
	{
	return ((iKey == aSwp.iKey) && (iValue == aSwp.iValue));
	}

/**
@param aSwp The object to be compared.
@return True, if the key and value are not equal; false otherwise.
*/
EXPORT_C TBool TSsmSwp::operator!=(const TSsmSwp& aSwp) const
	{
	return !(*this == aSwp);
	}
