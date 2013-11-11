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

#include "sgdriver.h"
#include "sgresourceadapter.h"


/**
@publishedPartner
@prototype
@deprecated

Default constructor.

@pre None.
@post This RSgDrawable handle is null.
*/
EXPORT_C RSgDrawable::RSgDrawable()
	: iImpl(NULL)
	{
	iHandleType = KSgDrawableTypeUid;
	}


/**
@publishedPartner
@prototype
@deprecated

Opens a new handle to a drawable resource.

@param aId The unique identifier of the drawable resource.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgDrawable handle is null.
@pre aId identifies an existing drawable resource.
@post This RSgDrawable handle references the drawable resource specified by
      its unique identifier. The reference count for the drawable resource is
      incremented by one.
@return KErrNone if successful.
@return KErrInUse if this RSgDrawable handle was not null.
@return KErrArgument if aId is the null drawable resource identifier.
@return KErrNotFound if aId cannot be found to refer to an existing drawable resource.
@return KErrPermissionDenied if this process is not permitted to access the drawable
        resource specified by aId.
@return KErrNoMemory if there is not enough system memory.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C TInt RSgDrawable::Open(const TSgDrawableId& aId)
	{
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
#endif
	TInt err = gPls.iDriver->OpenDrawable(aId, KSgDefaultOpenMode, iHandleType, iImpl);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@internalTechnology
@prototype

Opens a new handle to a drawable resource. This overload of Open() is intended for
the adaptation layer of renderers to be able to request special options when opening
a drawable resource.

@param aId The unique identifier of the drawable resource.
@param aMode Flags controlling how the drawable resource is opened. The bits of this
       argument are defined by the enumeration TSgDrawableOpenModes.
@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgDrawable handle is null.
@pre aId identifies an existing drawable resource.
@pre All of the requested opening options in aMode are supported.
@post This RSgDrawable handle references the drawable resource specified by
      its unique identifier. The reference count for the drawable resource is
      incremented by one.
@return KErrNone if successful.
@return KErrInUse if this RSgDrawable handle was not null.
@return KErrArgument if aId is the null drawable resource identifier.
@return KErrNotFound if aId cannot be found to refer to an existing drawable resource.
@return KErrPermissionDenied if this process is not permitted to access the drawable
        resource specified by aId.
@return KErrNotSupported if any of the requested opening options in aMode is not
        supported.
@return KErrNoMemory if there is not enough system memory.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
@see TSgDrawableOpenModes
*/
EXPORT_C TInt RSgDrawable::Open(const TSgDrawableId& aId, TUint32 aMode)
	{
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
#endif
	TInt err = gPls.iDriver->OpenDrawable(aId, aMode, iHandleType, iImpl);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}


/**
@publishedPartner
@prototype
@deprecated

Closes a handle to a drawable resource. If there are no remaining handles to the
drawable resource, then it can be destroyed by the Graphics Resource driver.
Calling Close() on a null handle is allowed but has no effect.

@pre If this RSgDrawable handle is not null then the Graphics Resource driver is
     initialised for use in the context of the calling process.
@pre This RSgDrawable handle is valid.
@post This RSgDrawable handle is null. The reference count for the previously
      referenced drawable resource, if any, is decremented by one.
@panic SGRES 2 in debug builds if this RSgDrawable handle is invalid.
@panic SGRES 5 in debug builds if this RSgDrawable handle is not null and the Graphics
       Resource driver is not initialised for use in the context of the calling process.
*/
EXPORT_C void RSgDrawable::Close()
	{
	if (iImpl)
		{
#ifdef _DEBUG
		gPls.iMutex.Wait();
		__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
		__ASSERT_DEBUG(gPls.iDriver->CheckDrawable(*iImpl), Panic(ESgPanicBadDrawableHandle));
#endif
		iImpl->Close();
		iImpl = NULL;
#ifdef _DEBUG
		gPls.iMutex.Signal();
#endif
		}
	}


/**
@publishedPartner
@prototype
@deprecated

Retrieves the unique identifier of a drawable resource. The unique identifier can be
used to share the drawable resource with another process.

@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgDrawable handle is valid.
@post None.
@return The unique identifier of the drawable resource or the null drawable resource
        identifier if this RSgDrawable handle is null.
@panic SGRES 2 in debug builds if this RSgDrawable handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
*/
EXPORT_C const TSgDrawableId& RSgDrawable::Id() const
	{
	if (!iImpl)
		{
		return KSgNullDrawableId;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckDrawable(*iImpl), Panic(ESgPanicBadDrawableHandle));
#endif
	const TSgDrawableId& id = iImpl->Id();
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return id;
	}


/**
@publishedPartner
@prototype
@deprecated

Tests whether this RSgDrawable handle is null.

@pre None.
@post None.
@return ETrue, if this RSgDrawable handle is null, EFalse otherwise.
*/
EXPORT_C TBool RSgDrawable::IsNull() const
	{
	return iImpl == NULL;
	}


/**
@publishedPartner
@prototype
@deprecated

Retrieves the run-time type of a drawable resource as a globally unique identifier.

@pre The Graphics Resource driver is initialised for use in the context of the
     calling process.
@pre This RSgDrawable handle is valid.
@post None.
@return The run-time type of the drawable resource as a globally unique identifier
        or the null globally unique identifier if this RSgDrawable handle is null.
@panic SGRES 2 in debug builds if this RSgDrawable handle is invalid.
@panic SGRES 5 in debug builds if the Graphics Resource driver is not initialised
       for use in the context of the calling process.
@see RSgDrawable::HandleType()
*/
EXPORT_C TUid RSgDrawable::DrawableType() const
	{
	if (!iImpl)
		{
		return KNullUid;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckDrawable(*iImpl), Panic(ESgPanicBadDrawableHandle));
#endif
	TUid type = iImpl->DrawableType();
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return type;
	}


/**
@internalComponent

Makes an extension interface available on a drawable resource. The extension interface
is specified by globally unique identifier.

@param aInterfaceUid Globally unique identifier of the interface to be made available.
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
@see RSgDrawable::GetInterface<M>(const M*&) const
*/
EXPORT_C TInt RSgDrawable::GetInterface(TUid aInterfaceUid, TAny*& aInterfacePtr) const
	{
	if (!iImpl)
		{
		return KErrBadHandle;
		}
#ifdef _DEBUG
	gPls.iMutex.Wait();
	__ASSERT_DEBUG(gPls.iDriver, Panic(ESgPanicNoDriver));
	__ASSERT_DEBUG(gPls.iDriver->CheckDrawable(*iImpl), Panic(ESgPanicBadDrawableHandle));
#endif
	TInt err = iImpl->GetInterface(aInterfaceUid, aInterfacePtr);
#ifdef _DEBUG
	gPls.iMutex.Signal();
#endif
	return err;
	}
