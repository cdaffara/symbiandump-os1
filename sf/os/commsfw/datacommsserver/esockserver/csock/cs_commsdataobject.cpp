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
 @internalTechnology
*/

#include <comms-infras/metatype.h>
#include <comms-infras/es_commsdataobject.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCSockcscmsd, "ESockCSockcscmsd");
#endif

using namespace ESock;

// No type Id since this class must be derived from.
// We deliberately do not register the attributes of this base class for
// serialization (with the exception of the iOperationMode member).
// They will be provided by the the c'tor of the deriving class. 
EXPORT_START_ATTRIBUTE_TABLE_AND_FN(XCommsDataObject, 0, 0)
	REGISTER_ATTRIBUTE(XCommsDataObject, iOperationMode, TMeta<TUint>)
END_ATTRIBUTE_TABLE()



EXPORT_C CCommsDataObjectBase* CCommsDataObjectBase::NewL(XCommsDataObject* aDataObject)
	{
	if (!aDataObject) 
		{
		User::Leave(KErrArgument);
		}
	return new (ELeave)CCommsDataObjectBase(aDataObject);
	}


EXPORT_C CCommsDataObjectBase* CCommsDataObjectBase::LoadL(TPtrC8& aBuffer)
	{
	CCommsDataObjectBase* self = new (ELeave)CCommsDataObjectBase(NULL);
	CleanupStack::PushL(self);
	User::LeaveIfError(self->Load(aBuffer));
	CleanupStack::Pop();
	return self;
	}


EXPORT_C CCommsDataObjectBase::CCommsDataObjectBase(XCommsDataObject* aDataObject)
	: iDataObject(aDataObject)
	{
	}


EXPORT_C CCommsDataObjectBase::~CCommsDataObjectBase()
	{
	delete iDataObject;
	}


EXPORT_C TInt CCommsDataObjectBase::Load(TPtrC8& aBuffer)
	{
	XCommsDataObject* newDataObject = NULL;
	TRAPD(err, newDataObject = static_cast<XCommsDataObject*>(XCommsDataObject::LoadL(aBuffer)));
	if (err == KErrNone)
		{
		delete iDataObject;
		iDataObject = newDataObject;
		}
	return err;
	}
	
	
EXPORT_C TInt CCommsDataObjectBase::Store(TDes8& aBuffer) const
	{
	return iDataObject->Store(aBuffer);
	}
	
	
EXPORT_C void CCommsDataObjectBase::Copy(const TAny* aData)
	{
	__ASSERT_DEBUG(aData, User::Panic(KSpecAssert_ESockCSockcscmsd, 1));
	const CCommsDataObjectBase* srcData = reinterpret_cast<const CCommsDataObjectBase*>(aData);
	iDataObject->Copy(*srcData->iDataObject);
	}
	
	
EXPORT_C TInt CCommsDataObjectBase::Length() const
	{
	return iDataObject->Length();
	}

