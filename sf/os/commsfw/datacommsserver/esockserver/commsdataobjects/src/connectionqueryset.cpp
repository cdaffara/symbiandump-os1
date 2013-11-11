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

#include "connectionqueryset.h"
#include <commsdat.h>
#include <comms-infras/ss_commsdataobject.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCommDatObjc, "ESockCommDatObjc");
#endif

using namespace Meta;
using namespace ESock;


// -------- Connection Settings Query Set -------- 

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(XConnectionQuerySet, CCommsDataObjectFactory::iUid, XConnectionQuerySet::iUid)
	REGISTER_ATTRIBUTE(XConnectionQuerySet, iQueries, TMetaObject<XConnectionQuerySet::RMetaDataQueryContainer>)
END_ATTRIBUTE_TABLE_BASE(XCommsDataObject, 0)


EXPORT_C XConnectionQueryBase* XConnectionQuerySet::FindQuery(const XConnectionQueryBase& aQuery) const
	{
	XConnectionQueryBase* query = NULL;
	TConnectionQueryIter iter(GetQueryIter(XConnectionQueryBase::ENotAnswered | XConnectionQueryBase::EAnswered));
	query = iter++;
	while (query) 
		{
		if (query->Match(aQuery)) 
			{
			return query;
			}
			
		query = iter++;
		}
		
	return NULL;
	}
	

EXPORT_C XCommsDataObject::TProgressAction XConnectionQuerySet::DispatchL(TAny *aItfPtr, ESock::MPlatsecApiExt* aPlatsecItf)
	{
	MQueryConnSettingsApiExt* interface = reinterpret_cast<MQueryConnSettingsApiExt*>(aItfPtr);

	TConnectionQueryIter iter = GetQueryIter(XConnectionQueryBase::ENotAnswered);
	XConnectionQueryBase* query = iter++;
	TInt queriesRemaining(0);
	while (query) 
		{
		queriesRemaining++;
		query->GetSetting(interface, aPlatsecItf);
		if (query->IsQueryAnswered()) 
			{
			queriesRemaining--;
			}
		query = iter++;
		}
	
	return (queriesRemaining > 0) ? XCommsDataObject::EContinue : XCommsDataObject::EComplete;
	}

EXPORT_C void XConnectionQuerySet::XNullQuery::GetSetting(MQueryConnSettingsApiExt* /*aItfPtr*/, MPlatsecApiExt* /*aPlatsecItf*/)
	{
	// Null Query has no value to get - impl here to satisfy abstract base class

#ifdef __X86GCC__
	// Ugly hack: GCC3.4 needs extra persuasion to unroll enough of the template; without this it won't link with a missing vtbl
	RMetaDataContainer<ESock::XConnectionQueryBase> unused;
	(void) unused;
#endif
	}



// -------- Individual Connection Queries -------- 

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(XConnectionQueryBase, XConnectionQuerySet::iUid, XConnectionQueryBase::ENull)
	REGISTER_ATTRIBUTE(XConnectionQueryBase, iQueryFlags, TMeta<TUint32>)
	REGISTER_ATTRIBUTE(XConnectionQueryBase, iError, TMetaNumber)
	REGISTER_ATTRIBUTE(XConnectionQueryBase, iElementId, TMeta<CommsDat::TMDBElementId>)
	REGISTER_ATTRIBUTE(XConnectionQueryBase, iType, TMeta<CommsDat::TCDFieldValueTypes>)
END_ATTRIBUTE_TABLE()


EXPORT_START_ATTRIBUTE_TABLE_AND_FN(XConnectionQuerySet::XNullQuery, CConnectionQueryFactory::iUid, XConnectionQueryBase::ENull)
END_ATTRIBUTE_TABLE_BASE(XConnectionQueryBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(XBoolQuery, CConnectionQueryFactory::iUid, XConnectionQueryBase::EBool)
	REGISTER_ATTRIBUTE(XBoolQuery, iData, TMeta<TBool>)
END_ATTRIBUTE_TABLE_BASE(XConnectionQueryBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(XUintQuery, CConnectionQueryFactory::iUid, XConnectionQueryBase::EUint)
	REGISTER_ATTRIBUTE(XUintQuery, iData, TMeta<TUint32>)
END_ATTRIBUTE_TABLE_BASE(XConnectionQueryBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(XIntQuery, CConnectionQueryFactory::iUid, XConnectionQueryBase::EInt)
	REGISTER_ATTRIBUTE(XIntQuery, iData, TMeta<TInt>)
END_ATTRIBUTE_TABLE_BASE(XConnectionQueryBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(XText8Query, CConnectionQueryFactory::iUid, XConnectionQueryBase::EText8)
	REGISTER_ATTRIBUTE(XText8Query, iData, TMetaBuf8)
END_ATTRIBUTE_TABLE_BASE(XConnectionQueryBase, 0)

EXPORT_START_ATTRIBUTE_TABLE_AND_FN(XText16Query, CConnectionQueryFactory::iUid, XConnectionQueryBase::EText16)
	REGISTER_ATTRIBUTE(XText16Query, iData, TMetaBuf16)
END_ATTRIBUTE_TABLE_BASE(XConnectionQueryBase, 0)



EXPORT_C XConnectionQueryBase* XConnectionQueryBase::LoadL(TPtrC8& aDes)
	{
	// LoadL methods are expected to strip the type Id from the buffer
	STypeId typeId = STypeId::CreateSTypeId(aDes); // This STypeId c'tor overload will adjust aDes
	__ASSERT_DEBUG(typeId.iUid.iUid == CConnectionQueryFactory::iUid && typeId.iType != XConnectionQueryBase::ENull, User::Panic(KSpecAssert_ESockCommDatObjc, 1));
	
	XConnectionQueryBase* query = NULL;
	switch (typeId.iType)
		{
		case XConnectionQueryBase::EBool:
			query = new (ELeave)XBoolQuery();
			break;
			
		case XConnectionQueryBase::EInt:
			query = new (ELeave)XIntQuery();
			break;
			
		case XConnectionQueryBase::EUint:
			query = new (ELeave)XUintQuery();
			break;
			
		case XConnectionQueryBase::EText8: 
			query = new (ELeave)XText8Query();
			break;
			
		case XConnectionQueryBase::EText16:
			query = new (ELeave)XText16Query();
			break;

		default:
			// Some unknown type in the descriptor
			User::Leave(KErrCorrupt);
		}
	
	CleanupStack::PushL(query);
	TInt err = query->Load(aDes);
	User::LeaveIfError(err);
	CleanupStack::Pop(query);
	return query;
	}
	


EXPORT_C void XConnectionQueryBase::SetQueryAnswered()
	{
	SetFlags(EAnswered);
	ClearFlags(ENotAttempted);
	}
	

EXPORT_C void XConnectionQueryBase::SetFlags(TUint32 aFlags)
	{
	const TUint answeredFlags = ENotAnswered | EAnswered;
	
	// Can't be answered and not answered
	__ASSERT_DEBUG((aFlags & answeredFlags) != answeredFlags, User::Panic(KSpecAssert_ESockCommDatObjc, 2));
	
	if (aFlags & EAnswered)
		{
		iQueryFlags &= ~ENotAnswered;	// Clear ENotAnswered
		iQueryFlags |= EAnswered;		// Set EAnswered
		}
	else if (aFlags & ENotAnswered) 
		{
		iQueryFlags &= ~ EAnswered;		// Clear EAnswered
		iQueryFlags |= ENotAnswered;	// Set ENotAnswered
		}
		
	// Set flags except for EAnswered or ENotAnswered
	iQueryFlags |= (aFlags & ~answeredFlags);
	}


EXPORT_C void XConnectionQueryBase::ClearFlags(TUint32 aFlags)
	{
	const TUint answeredFlags = ENotAnswered | EAnswered;
	
	// Can't be answered and not answered
	__ASSERT_DEBUG((aFlags & answeredFlags) != answeredFlags, User::Panic(KSpecAssert_ESockCommDatObjc, 3));	
	
	if (aFlags & EAnswered)
		{
		iQueryFlags &= ~EAnswered;		// Clear EAnswered
		iQueryFlags |= ENotAnswered;	// Set ENotAnswered
		}
	else if (aFlags & ENotAnswered) 
		{
		iQueryFlags &= ~ENotAnswered;	// Clear ENotAnswered
		iQueryFlags |= EAnswered;		// Set EAnswered
		}
		
	// Clear flags except for EAnswered or ENotAnswered
	iQueryFlags &= (~aFlags | answeredFlags);
	}


EXPORT_C void XConnectionQueryBase::SetError(TInt aError)
	{
	iError = aError;
	if (aError == KErrNone) 
		{
		ClearFlags(EFailed|ENotAttempted);
		}
	else
		{
		ClearFlags(ENotAttempted);
		SetFlags(EFailed);
		}
	}


EXPORT_C XConnectionQuerySet::~XConnectionQuerySet()
	{
	iQueries.ResetAndDestroy();
	iQueries.Close();
	}



EXPORT_C XBoolQuery::~XBoolQuery()
	{
	}
	
EXPORT_C void XBoolQuery::GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf)
	{
	TInt err = aItfPtr->GetBool(ElementId(), iData, aPlatsecItf);
	if (err == KErrNone)
		{
		SetQueryAnswered();
		}
	SetError(err);
	}

	
EXPORT_C XUintQuery::~XUintQuery()
	{
	}

EXPORT_C void XUintQuery::GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf)
	{
	TInt err = aItfPtr->GetInt(ElementId(), iData, aPlatsecItf);
	if (err == KErrNone)
		{
		SetQueryAnswered();
		}
	SetError(err);
	}
	
	
EXPORT_C XIntQuery::~XIntQuery()
	{
	}

EXPORT_C void XIntQuery::GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf)
	{
	TInt err = aItfPtr->GetInt(ElementId(), iData, aPlatsecItf);
	if (err == KErrNone)
		{
		SetQueryAnswered();
		}
	SetError(err);
	}
	
	
EXPORT_C XText8Query::~XText8Query()
	{
	iData.Close();
	}

EXPORT_C void XText8Query::GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf)
	{
	HBufC8* data = NULL;
	TInt err = aItfPtr->GetText(ElementId(), data, aPlatsecItf);
	if (err == KErrNone)
		{
		iData.Close();
		iData.Create(data->Des());

		SetQueryAnswered();
		}
	delete data;
	SetError(err);
	}


EXPORT_C XText16Query::~XText16Query()
	{
	iData.Close();
	}

EXPORT_C void XText16Query::GetSetting(MQueryConnSettingsApiExt* aItfPtr, MPlatsecApiExt* aPlatsecItf)
	{
	HBufC16* data = NULL;
	TInt err = aItfPtr->GetText(ElementId(), data, aPlatsecItf);
	if (err == KErrNone)
		{
		iData.Close();
		iData.Create(data->Des());
		
		SetQueryAnswered();
		}
	delete data;
	SetError(err);
	}

