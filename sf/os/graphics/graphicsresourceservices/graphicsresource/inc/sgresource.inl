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

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef SGRESOURCE_INL
#define SGRESOURCE_INL

/**
@publishedPartner
@prototype
@deprecated

Compares this identifier with another one to check if they are the same.

@param aId The other identifier to compare against.
@pre None.
@post None.
@return ETrue if the two identifiers are the same, EFalse otherwise.
*/
inline TBool TSgDrawableId::operator ==(const TSgDrawableId& aId) const
	{
	return iId[0] == aId.iId[0]
		&& iId[1] == aId.iId[1]
		&& iId[2] == aId.iId[2]
		&& iId[3] == aId.iId[3]
		&& iId[4] == aId.iId[4]
		&& iId[5] == aId.iId[5];
	}


/**
@publishedPartner
@prototype
@deprecated

Compares this identifier with another one to check if they are different.

@param aId The other identifier to compare against.
@pre None.
@post None.
@return ETrue if the two identifiers are different, EFalse otherwise.
*/
inline TBool TSgDrawableId::operator !=(const TSgDrawableId& aId) const
	{
	return !operator ==(aId);
	}


/**
@publishedPartner
@prototype
@deprecated

Retrieves the type of a handle derived from RSgDrawable as a globally unique identifier.
The type is set by the constructor of the handle and does not depend on the run-time
type of the referenced drawable resource.

@pre None.
@post None.
@return The type of the handle as a globally unique identifier.
@see RSgDrawable::DrawableType()
*/
inline TUid RSgDrawable::HandleType() const
	{
	return iHandleType;
	}

/**
@publishedPartner
@prototype
@deprecated

Makes an extension interface available on a drawable resource. The extension interface
is specified by type. The following example demonstrates how to use this function
to get access to an extension interface.

@code
	MExampleExtensionInterface* interface;
	if (drawable.GetInterface(interface) == KErrNone)
		{
@endcode

@param aInterfacePtr On return, a pointer to the specified interface.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgDrawable handle is valid and not null.
@pre The specified interface is supported on the drawable resource.
@post The specified interface is available until this RSgDrawable handle is closed.
@return KErrNone if successful.
@return KErrBadHandle if this RSgDrawable handle is null.
@return KErrExtensionNotSupported if the specified interface is not supported on
        the drawable resource.
@return KErrNoMemory if there is not enough system memory.
@panic SGRES 2 in debug builds if this RSgDrawable handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
@see RSgDrawable::GetInterface<M>(const M*&) const

WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. 
*/
template<class M>
inline TInt RSgDrawable::GetInterface(M*& aInterfacePtr)
	{
	return GetInterface(TUid::Uid(M::EInterfaceUid), (TAny*&)aInterfacePtr);
	}

/**
@publishedPartner
@prototype
@deprecated

Makes an extension interface available on a drawable resource. The extension interface
is specified by type and returned const-qualified. The following example demonstrates
how to use this function to get access to an extension interface.

@code
	const MExampleExtensionInterface* interface;
	if (drawable.GetInterface(interface) == KErrNone)
		{
@endcode

@param aInterfacePtr On return, a pointer to the specified interface.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgDrawable handle is valid and not null.
@pre The specified interface is supported on the drawable resource.
@post The specified interface is available until this RSgDrawable handle is closed.
@return KErrNone if successful.
@return KErrBadHandle if this RSgDrawable handle is null.
@return KErrExtensionNotSupported if the specified interface is not supported on
        the drawable resource.
@return KErrNoMemory if there is not enough system memory.
@panic SGRES 2 in debug builds if this RSgDrawable handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
@see RSgDrawable::GetInterface<M>(M*&)

WARNING: Function for internal use ONLY.  Compatibility is not guaranteed in future releases. 
*/
template<class M>
inline TInt RSgDrawable::GetInterface(const M*& aInterfacePtr) const
	{
	return GetInterface(TUid::Uid(M::EInterfaceUid), (TAny*&)aInterfacePtr);
	}

#endif // SGRESOURCE_INL
