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
 @publishedPartner
 @released
*/

#ifndef ES_COMMSDATAOBJECT_H
#define ES_COMMSDATAOBJECT_H


#include <es_sock.h>
#include <comms-infras/api_ext_list.h>
#include <comms-infras/metacontainer.h>
#include <comms-infras/metatype.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESocks_cmsdtbjct, "ESocks_cmsdtbjct");
#endif

namespace ESock
{
class CConnection;
class MPlatsecApiExt;
class TGetOrSetParameters;


/**
The base class for any CommsDataObject. 

A CommsDataObject is a serializable data holding object that knows how
it should be persisted within the comms framework when dispatched on a
node with a supported interface.

Depending on the specific implementation of the object it can be used to
store or retrieve information from one or more nodes in the stack of
connection providers.

Note: A CommsDataObject has no connection to the similarly named CommsDat
database or any of its associated classes.
*/
class XCommsDataObject : public Meta::SMetaDataECom
	{
	friend class ::RConnection;
	friend class ESock::CConnection;
	friend class ESock::TGetOrSetParameters;
	
public:
	inline TUint RequiredItfExtId() const
		{
		return iReqItfExtId;
		}

	inline TBool IsGetSupported() const
		{
		return (iFlags & EGetSupported) == EGetSupported;
		}
		
	inline TBool IsSetSupported() const
		{
		return (iFlags & ESetSupported) == ESetSupported;
		}
	
	inline TBool IsProgressive() const
		{
		return (iFlags & EProgressive) == EProgressive;
		}
	
protected:
    /**
    Indicates the action to be taken by the framework after the
    CommsDataObject has been dispatched. 
    */
	enum TProgressAction
		{
		/** The selected operation is finished and the client should be completed.
		This should always be returned if the CommsDataObject does not support the
		EProgressive operation */
		EComplete,
		
		/** The selected operation has more work to do and framework should continue
		to forward the CommsDataObject to the next provider */
		EContinue,
		};

	/**
	Flags that identify the client requested operation. These are set
	on the CommsDataObject internally and should only be used in the
	implementation of a CommsDataObject when comparing the result of
	OperationMode().
	*/
	enum 
		{
		/** Client requested that the object store its data on a provider */
		EOperationSet = 0,

		/** Client requested that the object retreive data from a provider */
		EOperationGet = 1,
		
		/** 
		Default value for uninitialized requests.
		This is only used as a sanity checking measure where the implementation of a 
		CommsDataObject has failed to explicitly specify the client requested operation, i.e. get or set	
		*/
		EOperationUninitialized = 0xFFFFFFFF
		};

	
	/**
	Flags that identify the operations that a CommsDataObject supports.
	The framework will not allow requests for unsupported operations.
	*/
	enum
		{
		/** The CommsDataObject supports storing its data on a provider */
		ESetSupported = 1,

		/** The CommsDataObject supports retreiving data from a provider */
		EGetSupported = 2,

		/** The CommsDataObject is required to progressively be dispatched
		on all providers supporting the required interface. When this flag
		is present the framework will listen to the TProgressAction returned
		by the DispatchL() method */
		EProgressive = 4,
		
		/**
		Mask that covers all operation flags
		*/
		EOperationSupportMask = ESetSupported | EGetSupported,
		
		/**
		Mask that covers all currently used flags
		*/
		EAllFlags = ESetSupported | EGetSupported | EProgressive,
		};

	
	/**
	Constructor for the CommsDataObject.
	
	This version of the constructor will default to providing a 
	MAccessPointConfigApi interface when the CommsDataObject is dispatched.
	
	@param aFlags Flags that identify the operations supported by, and
	desired framework handling of the CommsDataObject. Only use flags
	supported by XCommsDataObject. The CommsDataObject must support at
	least the Get or Set operation.
	
	@see MAccessPointConfigApi
	*/
	XCommsDataObject(TUint aFlags)
		: iOperationMode(EOperationUninitialized), iReqItfExtId((TUint)EAccessPointConfigApi), iFlags(aFlags)
		{
		__ASSERT_DEBUG((aFlags & ~EAllFlags) == 0, User::Panic(KSpecAssert_ESocks_cmsdtbjct, 1));
		__ASSERT_DEBUG((aFlags & EOperationSupportMask) != 0, User::Panic(KSpecAssert_ESocks_cmsdtbjct, 2));
		}

	
	/**
	Constructor for the CommsDataObject.
	
	@param aFlags Flags that identify the operations supported by, and
	desired framework handling of the CommsDataObject. Only use flags
	supported by XCommsDataObject. The CommsDataObject must support at
	least the Get or Set operation.
	
	@param aReqItfExtId An interface identifier from TSupportedCommsApiExt.
	*/
	XCommsDataObject(TUint aFlags, TUint aReqItfExtId)
		: iOperationMode(EOperationUninitialized), iReqItfExtId(aReqItfExtId), iFlags(aFlags)
		{
		__ASSERT_DEBUG((aFlags & ~EAllFlags) == 0, User::Panic(KSpecAssert_ESocks_cmsdtbjct, 3));
		__ASSERT_DEBUG((aFlags & EOperationSupportMask) != 0, User::Panic(KSpecAssert_ESocks_cmsdtbjct, 4));
		}

	
	/**
	Called by the framework. Needs to be implemented by the CommsDataObject to perform
	the operation indicated by OperationMode().
	
	@param aItfPtr A pointer to the interface that the CommsDataObject has requested.
	@param aItfPtr A pointer to a ESock::MPlatsecApiExt interface to be used for any
	required platsec checks.
	
	@return A value from TProgressAction indicating whether or not the CommsDataObject
	has finished its operation. Should always be EComplete if the object does not support
	the EProgressive operation.
	
	@see ESock::MPlatsecApiExt
	*/
	virtual TProgressAction DispatchL(TAny* aItfPtr, ESock::MPlatsecApiExt* aPlatsecItf) = 0;

	/**
	@return The operation requested by the client.
	*/
	inline TUint OperationMode() const
		{
		return iOperationMode;
		}
		
	/**
	@return The supported operation flags
	*/
	inline TUint Flags() const
		{
		return iFlags;
		}

private:		
    /**
	Used internally to set the operation mode requested by the client.
	*/
	inline void SetOperationMode(TUint aMode)
		{
		iOperationMode = aMode;
		}
	
protected:
	EXPORT_DATA_VTABLE_AND_FN	
	
private:
	TUint iOperationMode;
	TUint iReqItfExtId;
	TUint iFlags;
	};


/**
Container class for the XCommsDataObject. Responsible for
owning, serialising, and deserialising the CommsDataObject during API
calls

Note: A CommsDataObject has no connection to the similarly named CommsDat
database or any of its associated classes.

@see XCommsDataObject
*/
class CCommsDataObjectBase : public CBase, public Meta::MMetaType
	{
	friend class ::RConnection;	
	
public:
	/**
	NewL for the CCommsDataObject class.
	@param aDataObject A ptr to the data object to take ownership of
	*/
	IMPORT_C static CCommsDataObjectBase* NewL(XCommsDataObject* aDataObject);
	IMPORT_C static CCommsDataObjectBase* LoadL(TPtrC8& aBuffer);
	
	/**
	@D'tor for the CCommsDataObject class
	*/
	IMPORT_C ~CCommsDataObjectBase();

	// From  Meta::MetaType
	IMPORT_C virtual TInt Load(TPtrC8& aBuffer);
	IMPORT_C virtual TInt Store(TDes8& aBuffer) const;
    IMPORT_C virtual void Copy(const TAny* aData);
	IMPORT_C virtual TInt Length() const;
	
protected:
	/**
	C'tor for the CCommsDataObject class.
	@param aDataObject A ptr to the data object to take ownership of
	*/
	IMPORT_C CCommsDataObjectBase(XCommsDataObject* aDataObject);

protected:
	XCommsDataObject* iDataObject;
	};


/**
Thin wrapper template class to provide type safety around CCommsDataObjectBase
*/
template<class SUBCLASS, class XCLASS>
class CCommsDataObject : public CCommsDataObjectBase
	{
public:	
	inline static SUBCLASS* NewL(XCLASS* aDataObject)
		{
		__ASSERT_DEBUG(aDataObject, User::Panic(KSpecAssert_ESocks_cmsdtbjct, 5));
		if (XCLASS::TypeId() != aDataObject->GetTypeId()) 
			{
			User::Leave(KErrArgument);
			}
		return static_cast<SUBCLASS*>(CCommsDataObjectBase::NewL(aDataObject));
		}
		
	inline static SUBCLASS* LoadL(TPtrC8& aBuffer)
		{
		TPtrC8 typeCheckPtr(aBuffer);
		STypeId typeId = STypeId::CreateSTypeId(typeCheckPtr);
		if (XCLASS::TypeId() != typeId) 
			{
			User::Leave(KErrCorrupt);
			}
		return static_cast<SUBCLASS*>(CCommsDataObjectBase::LoadL(aBuffer));
		}

    inline XCLASS& DataObject()
        {
        return static_cast<XCLASS&>(*iDataObject);
        }

protected:
    inline CCommsDataObject(XCommsDataObject* aDataObject)
        : CCommsDataObjectBase(aDataObject)
        {
        }
	};

} // namespace ESock

#endif
// ES_COMMSDATAOBJECT_H

