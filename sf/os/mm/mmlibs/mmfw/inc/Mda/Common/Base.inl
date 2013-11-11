// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mda\Common\Base.inl
// Inline functions for package classes defined in MdaBase.h
// 
//

inline TPtr8& TMdaRawPackage::Package()
	{ 
	((TMdaRawPackage*)this)->iThis.Set((TUint8*)this,iThis.Length(),iThis.MaxLength()); 
	return iThis; 
	}

inline const TPtr8& TMdaRawPackage::Package() const
	{ 
	((TMdaRawPackage*)this)->iThis.Set((TUint8*)this,iThis.Length(),iThis.MaxLength()); 
	return iThis; 
	}

inline void TMdaRawPackage::SetSize(TInt aDerivedSize)
	{ 
	iThis.Set((TUint8*)this,aDerivedSize,aDerivedSize);
	}

/**
Gets the package type. This is set during construction of the derived class.

@return  A UID that identifies the package type.
*/
inline TUid TMdaPackage::Type() const
	{
	return iType; 
	}

/**
Gets the derived class's type. This is set during construction of the derived class.

@return A UID that identifies the type of the derived class.
*/
inline TUid TMdaPackage::Uid() const
	{ 
	return iUid;
	}

/**
Equality operator. Tests whether the package specified has the same UID and type as this package.

@param  aPackage 
        The package to compare with this one.

@return True if the two packages are the same, false if not.
*/
inline TBool TMdaPackage::operator==(const TMdaPackage& aPackage)
	{
	return ((aPackage.Type()==Type())&&(aPackage.Uid()==Uid()));
	}

inline void TMdaPackage::SetUid(TUid aUid)
	{
	iUid = aUid;
	}

inline TMdaRawPackage::TMdaRawPackage(TInt aDerivedSize)
#pragma warning( disable : 4355 )	// 'this' : used in base member initializer list
: iThis((TUint8*)this,aDerivedSize,aDerivedSize) 
#pragma warning( default : 4355 )
	{ 
	}

/**
Protected constructor with two UIDs and the size of the derived class.

@param  aType 
        A UID that identifies the package's concrete class.
@param  aUid 
        A UID that identifies the package type. This may be KNullUid if not required.
@param  aDerivedSize 
        The size of the derived class.
*/
inline TMdaPackage::TMdaPackage(TUid aType, TUid aUid, TInt aDerivedSize)
: TMdaRawPackage(aDerivedSize),iType(aType),iUid(aUid)
	{
	}
