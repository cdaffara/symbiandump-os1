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

#include <lbsrequestor.h>
#include <lbscommon.h>
#include <lbserrors.h>
#include "LbsInternal.h"
#include <s32strm.h>

// Constant definitions
const TUint16 KRequestorVersion = 1;
const TUint16 KRequestorStackVersion = 1;


//-----------------------------------------------------------------------------
// CRequestorBase
//-----------------------------------------------------------------------------

EXPORT_C CRequestorBase::CRequestorBase()
/**
Constructor for CRequestorBase.

*/
	{}

EXPORT_C CRequestorBase::~CRequestorBase()
/**
Destructor for CRequestorBase.

*/
	{
	delete iData;
	}

EXPORT_C void CRequestorBase::ConstructL(TRequestorType aType,
										 TRequestorFormat aFormat,
										 const TDesC& aData)
/**
Second phase constructor. Must be called by all derived classes to initialise
CRequestorBase.

@param aType identifies the type of requestor, a service or a contact.
@param aFormat determines the type of data held in aData
@param aData is requestor data. Can be a telephone number, a URL etc.
 */
	{
	iRequestorType = aType;
	iFormat = aFormat;
	iData = aData.AllocL();
	}

EXPORT_C void CRequestorBase::SetRequestorL(TRequestorType aType,
                                        TRequestorFormat aFormat,
                                        const TDesC& aData)
/**
Replaces the requestor data for this object with the specified data.

@param aType identifies the type of requestor, a service or a contact.
@param aFormat determines the type of data held in aData
@param aData is requestor data. Can be a telephone number, a URL etc.
 */
	{
	__ASSERT_DEBUG(iData, User::Panic(KPositionInternalFault, ERequestorDataNull));	
	if(aData.Length()>iData->Des().MaxLength())
		{
		HBufC* newData = aData.AllocL();
		delete iData;
		iData = newData;
		}
	else
		{
		*iData = aData;
		}

	iRequestorType = aType;
	iFormat = aFormat;
	}

EXPORT_C CRequestorBase::TRequestorType CRequestorBase::RequestorType() const
/**
Simple accessor for this requestors member data.

@return the type of requestor, a service or a contact.
 */
	{
	return iRequestorType;
	}

EXPORT_C CRequestorBase::TRequestorFormat CRequestorBase::RequestorFormat() const
/**
Simple accessor for this requestors member data.

@return the type of data held in this requestor.
 */
	{
	return iFormat;
	}

EXPORT_C TDesC& CRequestorBase::RequestorData() const
/**
Simple accessor for this requestors member data.

@return the requestor data.
 */
	{
	return *iData;
	}

EXPORT_C void CRequestorBase::GetRequestor(TRequestorType& aType,
                                       TRequestorFormat& aFormat,
                                       TPtrC& aData) const
/**
Simple accessor for this requestors member data.

@param aType will be set to the type of requestor.
@param aFormat will be set to the type of data held in aData.
@param aData will be set to point to this requestors data.
 */
	{
	aType = iRequestorType;
	aFormat = iFormat;
	aData.Set(*iData);
	}

EXPORT_C void CRequestorBase::InternalizeL(RReadStream& aStream)
/**
Restores this requestors member data from a stream.

@param aStream the stream from which the requestors data is read.
 */
	{
	const TUint16 version = aStream.ReadUint16L();
	__ASSERT_ALWAYS(version == KRequestorVersion,
		User::Panic(KPosClientFault, EPositionInvalidClassType));
	const TUint16 sizeOfCRequestorBase = aStream.ReadUint16L();
	__ASSERT_ALWAYS(sizeOfCRequestorBase == sizeof(CRequestorBase),
		User::Panic(KPosClientFault, EPositionInvalidClassType));

	const TRequestorType type = aStream.ReadInt32L();
	const TRequestorFormat format = aStream.ReadInt32L();
	HBufC* data = HBufC::NewL(aStream, KMaskDesLength16);

	//OK, we've got all the data now, no way we can leave from here on.
	iRequestorType = type;
	iFormat = format;
	delete iData;
	iData = data;	//Take ownership of data here.
	}

EXPORT_C void CRequestorBase::ExternalizeL(RWriteStream& aStream) const
/**
Stores this requestors member data to a stream.

@param aStream the stream to which the requestors data is written.
 */
	{
	aStream.WriteUint16L(KRequestorVersion);
	aStream.WriteUint16L(sizeof(CRequestorBase));
	aStream.WriteInt32L(iRequestorType);
	aStream.WriteInt32L(iFormat);
	aStream << *iData;
	}

EXPORT_C TAny* CRequestorBase::ExtendedInterface(TInt /*aFunctionNumber*/,
											 TAny* /*aPtr1*/,
											 TAny* /*aPtr2*/)
/**
This method is used to allow polymorphic extensions to the API without breaking
BC. See documentation for explanation.
 
@param aFunctionNumber contains the Id of the function to be invoked.
@param aPtr1 a pointer to any data
@param aPtr2 a pointer to any data.
@return TAny* a pointer to any function
 */
	{
	return NULL;
	}

//-----------------------------------------------------------------------------
// CRequestor
//-----------------------------------------------------------------------------

EXPORT_C CRequestor* CRequestor::New(TRequestorType aType,
                                     TRequestorFormat aFormat,
                                     const TDesC& aData)
/**
Allocates and constructs a <code>CRequestor</code> object.
The function returns NULL if there is insufficient memory.
Slightly unusual construction is to ensure that iData can never be NULL
even if someone inherits from this class.

@param aType identifies the type of requestor, a service or a contact.
@param aFormat determines the type of data held in aData
@param aData is requestor data. Can be a telephone number, a URL etc.
@return a newly created CRequestor object, or NULL.
 */
	{
	TInt err = KErrNone;
	CRequestor* self = NULL;
	TRAP(err, self = CRequestor::NewL(aType, aFormat, aData));
	return self;
	}

EXPORT_C CRequestor* CRequestor::NewL(TRequestorType aType,
                                      TRequestorFormat aFormat,
                                      const TDesC& aData)
/**
Allocates and constructs a <code>CRequestor</code> object.
The function leaves if there is insufficient memory.

@param aType identifies the type of requestor, a service or a contact.
@param aFormat determines the type of data held in aData
@param aData is requestor data. Can be a telephone number, a URL etc.
@return a newly created CRequestor object.
 */
	{
	CRequestor* self = CRequestor::NewLC(aType, aFormat, aData);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CRequestor* CRequestor::NewLC(TRequestorType aType,
                                       TRequestorFormat aFormat,
                                       const TDesC& aData)
/**
Allocates and constructs a <code>CRequestor</code> object, and leaves
the object on the cleanup stack.

The function leaves if there is insufficient memory.

@param aType identifies the type of requestor, a service or a contact.
@param aFormat determines the type of data held in aData
@param aData is requestor data. Can be a telephone number, a URL etc.
@return a newly created CRequestor object.
 */
	{
	CRequestor* self = new(ELeave) CRequestor;
	CleanupStack::PushL(self);
	self->ConstructL(aType, aFormat, aData);
	return self;
	}

CRequestor::CRequestor()
/**
Constructor for CRequestor.

@internalComponent
*/
	{}

CRequestor::~CRequestor()
/**
Destructor for CRequestor.

@internalComponent
*/
	{}

EXPORT_C CRequestor* CRequestor::NewL(RReadStream& aStream)
/**
Allocates and constructs a <code>CRequestor</code> object, and initialises
its member variables by reading the internalizing the object from the
specified stream.

The function leaves if there is insufficient memory.

@param aStream is a stream that a CRequestor object was previously externalised to.
@return a newly created CRequestor object.
 */
	{
	CRequestor* self = new(ELeave) CRequestor;
	CleanupStack::PushL(self);
	aStream >> *self;
	CleanupStack::Pop(self);
	return self;
	}


//-----------------------------------------------------------------------------
// RRequestorStack
//-----------------------------------------------------------------------------

EXPORT_C void RRequestorStack::ExternalizeL(RWriteStream& aStream) const
/**
Stores a collection of requestors member data to a stream.

@param aStream the stream to which each of the requestors data is written.
 */
	{
	aStream.WriteUint16L(KRequestorStackVersion);

	const TInt count = Count();
	aStream.WriteInt32L(count);

	const CRequestor* req;
	for(TInt i = 0; i<count; i++)
		{
		req = (*this)[i];
		__ASSERT_ALWAYS(req!=NULL, User::Panic(KPosClientFault, EPositionNullRequestor));
		aStream << *req;
		}
	}

EXPORT_C void RRequestorStack::InternalizeL(RReadStream& aStream)
/**
Restores a collection of requestors member data from a stream.

@param aStream the stream from which each of the requestors data is read.
 */
	{
	const TUint16 version = aStream.ReadUint16L();
	__ASSERT_ALWAYS(version == KRequestorStackVersion,
		User::Panic(KPosClientFault, EPositionInvalidClassType));

	ResetAndDestroy();
	CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, this));
	const TInt count = aStream.ReadInt32L();

	CRequestor* requestor;
	for (TInt i=0; i<count; i++)
		{
		requestor = CRequestor::NewL(aStream);
		CleanupStack::PushL(requestor);
		User::LeaveIfError(Append(requestor));
		CleanupStack::Pop(requestor);
		}

	CleanupStack::Pop(this); // TCleanupItem(CleanupResetAndDestroy, this)
	}

void RRequestorStack::CleanupResetAndDestroy(TAny* aArray)
/**
An internal function for cleaning up the requestor stack if a leave
occurs during the internalization process.

@internalComponent
@param aArray is a pointer to an RRequestorStack
 */
	{
	reinterpret_cast<RRequestorStack*>(aArray)->ResetAndDestroy();
	}


