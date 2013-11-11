// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @prototype
*/

#ifndef CS_GENEVENT_INL
#define CS_GENEVENT_INL

using namespace ESock;

CEventParamsGranted* CEventParamsGranted::NewL(TUint32 aFamilyId)
/** Creates a granted parameter event sub-connection.

@return a CEventParamsGranted object pointer if successful,
 otherwise leaves with system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(KEventTypeImplUid, KEventParamsGranted);
	CEventParamsGranted* self = static_cast<CEventParamsGranted*>(Meta::SMetaDataECom::NewInstanceL(typeId));
	self->iFamilyBundle.CreateL();
	self->iFamilyBundle.CreateFamilyL(aFamilyId);
	return self;
	}

CEventParamsGranted::CEventParamsGranted()
/** Empty CEventParamsGranted constructor
*/
	{
	}

const XParameterSetBase* CEventParamsGranted::FindParameterSet(TUint32 aFamilyId, TUint aIndex) const
/** Find Parameter set.

@param aIndex the index number of the parameter to be returned 

@return pointer to selected XParameterSetBase. */
	{
	return const_cast<RParameterFamilyBundle&>(iFamilyBundle).FindFamily(aFamilyId).GetParameterSetAtIndex(aIndex, RParameterFamily::EGranted);
	}

void CEventParamsGranted::AddParameterSetL(TUint32 aFamilyId, XParameterSetBase* aParameterSet)
/** Adds Parameter set.

@param aParameterSet parameter set to be added in the list. */
	{
	iFamilyBundle.FindFamily(aFamilyId).AddParameterSetL(aParameterSet, RParameterFamily::EGranted);
	}

TInt CEventParamsGranted::CountParameterSets(TUint32 aFamilyId)
/** Gets the number of Parameter set.

@return number of Parameter set. */
	{
	return iFamilyBundle.FindFamily(aFamilyId).CountParameterSets(RParameterFamily::EGranted);
	}


//====================================

CEventParamsChanged* CEventParamsChanged::NewL(TUint32 aFamilyId)
/** Creates a changed parameter event sub-connection.

@return CEventParamsChanged pointer if successful,
 otherwise leaves with system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(KEventTypeImplUid, KEventParamsChanged);
	CEventParamsChanged* self =  static_cast<CEventParamsChanged*>(Meta::SMetaDataECom::NewInstanceL(typeId));
	self->iFamilyBundle.CreateL();
	self->iFamilyBundle.CreateFamilyL(aFamilyId);
	return self;
	}

CEventParamsChanged::CEventParamsChanged()
	: CEventParamsGranted()
/** Empty CEventParamsChanged constructor
*/
	{
	}

TInt CEventParamsChanged::Error() const
/** Gets Error value.

@return error value. */
	{
	return iError;
	}

void CEventParamsChanged::SetError(TInt aError)
/** Sets Error value.

@param aError error value. */
	{
	iError = aError;
	}



#endif //CS_GENEVENT_INL

