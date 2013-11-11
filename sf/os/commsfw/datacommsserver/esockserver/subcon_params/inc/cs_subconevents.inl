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
 @publishedAll
 @released
*/

#ifndef __CS_SUBCONEVENTS_INL__
#define __CS_SUBCONEVENTS_INL__


CSubConGenEventParamsGranted* CSubConGenEventParamsGranted::NewL()
/** Creates a granted parameter event sub-connection.

@return a CSubConGenEventParamsGranted object pointer if successful,
 otherwise leaves with system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(KSubConnGenericEventsImplUid, KSubConGenericEventParamsGranted);
	return static_cast<CSubConGenEventParamsGranted*>(CSubConNotificationEvent::NewL(typeId));
	}

CSubConGenEventParamsGranted::CSubConGenEventParamsGranted()
	: CSubConNotificationEvent(), iFamily(KSubConGlobalFamily), iGenericSet(NULL)
/** Empty CSubConGenEventParamsGranted constructor
*/
	{
	}

const CSubConGenericParameterSet* const CSubConGenEventParamsGranted::GetGenericSet() const
/** Gets Generic Parameter set.

@return pointer to CSubConGenericParameterSet. */
	{
	return iGenericSet;
	}

void CSubConGenEventParamsGranted::SetGenericSet(CSubConGenericParameterSet* aGenericSet)
/** Sets Generic Parameter set.

@param aGenericSet pointer to Generic Parameter set. */
	{
	if (iGenericSet != NULL)
		{
		delete iGenericSet;
		}
	iGenericSet = aGenericSet;
	}

const CSubConExtensionParameterSet* const CSubConGenEventParamsGranted::GetExtensionSet(TUint aIndex) const
/** Gets Extension Parameter set.

@param aIndex the index number of the extension parameter to be returned (index number must not be greater
			than the value returned by CSubConGenEventParamsGranted::GetNumExtensionSets())

@return pointer to selected CSubConExtensionParameterSet. */
	{
	return static_cast<CSubConExtensionParameterSet*>(iExtensionSets[aIndex]);
	}

void CSubConGenEventParamsGranted::AddExtensionSetL(CSubConExtensionParameterSet* aExtensionSet)
/** Adds Extension Parameter set.

@param aExtensionSet extension parameter set to be added in the list. */
	{
	iExtensionSets.AppendL(aExtensionSet);
	}

TInt CSubConGenEventParamsGranted::GetNumExtensionSets()
/** Gets the number of Extension Parameter set.

@return number of Extension Parameter set. */
	{
	return iExtensionSets.Count();
	}

TUint32 CSubConGenEventParamsGranted::GetFamily() const
/** Gets parameter family type.

@return parameter family type. */
	{
	return iFamily;
	}

void CSubConGenEventParamsGranted::SetFamily(TUint32 aFamily)
/** Sets parameter family type.

@param aFamily parameter family type. */
	{
	iFamily = aFamily;
	}

//====================================

CSubConGenEventParamsChanged* CSubConGenEventParamsChanged::NewL()
/** Creates a changed parameter event sub-connection.

@return CSubConGenEventParamsChanged pointer if successful,
 otherwise leaves with system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(KSubConnGenericEventsImplUid, KSubConGenericEventParamsChanged);
	return static_cast<CSubConGenEventParamsChanged*>(CSubConNotificationEvent::NewL(typeId));
	}

CSubConGenEventParamsChanged::CSubConGenEventParamsChanged()
	: CSubConGenEventParamsGranted()
/** Empty CSubConGenEventParamsChanged constructor
*/
	{
	}

TInt CSubConGenEventParamsChanged::Error() const
/** Gets Error value.

@return error value. */
	{
	return iError;
	}

void CSubConGenEventParamsChanged::SetError(TInt aError)
/** Sets Error value.

@param aError error value. */
	{
	iError = aError;
	}

//====================================

CSubConGenEventParamsRejected* CSubConGenEventParamsRejected::NewL()
/** Creates a rejected parameter event sub-connection.

@return CSubConGenEventParamsRejected pointer if successful,
 otherwise leaves with system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(KSubConnGenericEventsImplUid, KSubConGenericEventParamsRejected);
	return static_cast<CSubConGenEventParamsRejected*>(CSubConNotificationEvent::NewL(typeId));
	}

CSubConGenEventParamsRejected::CSubConGenEventParamsRejected()
	: CSubConNotificationEvent()
/** Empty CSubConGenEventParamsRejected constructor
*/
	{
	}

TInt CSubConGenEventParamsRejected::Error() const
/** Gets Error value.

@return error value. */
	{
	return iError;
	}

void CSubConGenEventParamsRejected::SetError(TInt aError)
/** Sets Error value.

@param aError error value. */
	{
	iError = aError;
	}

TInt CSubConGenEventParamsRejected::FamilyId() const
/** Gets parameter family type.

@return parameter family type. */
	{
	return iFamilyId;
	}

void CSubConGenEventParamsRejected::SetFamilyId(TInt aFamilyId)
/** Sets parameter family type.

@param aFamilyId parameter family type. */
	{
	iFamilyId = aFamilyId;
	}

//====================================
CSubConGenEventDataClientBase::CSubConGenEventDataClientBase()
	: CSubConNotificationEvent()
/** Empty CSubConGenEventDataClientBase constructor
*/
	{
	}

TUint CSubConGenEventDataClientBase::GetIap() const
/** Gets sub-connection's Internet Access Point.

@return Internet Access Point. */
	{
	return iIap;
	}

void CSubConGenEventDataClientBase::SetIap(TUint aIap)
/** Sets sub-connection's Internet Access Point.

@param aIap Internet Access Point. */
	{
	iIap = aIap;
	}

const TSockAddr& CSubConGenEventDataClientBase::GetSourceAdress() const
/** Gets sub-connection's source socket address.

@return Internet Access Point. */
	{
	return iSourceAddress;
	}

void CSubConGenEventDataClientBase::SetSourceAddress(const TSockAddr& aSourceAddress)
/** Sets sub-connection's source socket address.

@param aIap Internet Access Point. */
	{
	iSourceAddress = aSourceAddress;
	}

const TSockAddr& CSubConGenEventDataClientBase::GetDestAdress() const
/** Gets sub-connection's destination socket address.

@return Internet Access Point. */
	{
	return iDestAddress;
	}

void CSubConGenEventDataClientBase::SetDestAddress(const TSockAddr& aDestAddress)
/** Sets sub-connection's destination socket address.

@param aIap Internet Access Point. */
	{
	iDestAddress = aDestAddress;
	}

//====================================
CSubConGenEventDataClientJoined* CSubConGenEventDataClientJoined::NewL()
/** Creates a data client joined event sub-connection.

@return CSubConGenEventDataClientJoined pointer if successful,
 otherwise leaves with system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(KSubConnGenericEventsImplUid, KSubConGenericEventDataClientJoined);
	return static_cast<CSubConGenEventDataClientJoined*>(CSubConNotificationEvent::NewL(typeId));
	}

CSubConGenEventDataClientJoined::CSubConGenEventDataClientJoined()
	: CSubConGenEventDataClientBase()
/** Empty CSubConGenEventDataClientJoined constructor
*/
	{
	}

//=====================================
CSubConGenEventDataClientLeft* CSubConGenEventDataClientLeft::NewL()
/** Creates a data client left event sub-connection.

@return CSubConGenEventDataClientLeft pointer if successful,
 otherwise leaves with system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(KSubConnGenericEventsImplUid, KSubConGenericEventDataClientLeft);
	return static_cast<CSubConGenEventDataClientLeft*>(CSubConNotificationEvent::NewL(typeId));
	}

CSubConGenEventDataClientLeft::CSubConGenEventDataClientLeft()
	: CSubConGenEventDataClientBase()
/** Empty CSubConGenEventDataClientLeft constructor
*/
	{
	}

//====================================
CSubConGenEventSubConDown* CSubConGenEventSubConDown::NewL()
/** Creates a sub-connection down event.

@return a CSubConGenEventSubConDown pointer if successful,
 otherwise leaves with system error code.
*/
	{
	STypeId typeId = STypeId::CreateSTypeId(KSubConnGenericEventsImplUid, KSubConGenericEventSubConDown);
	return static_cast<CSubConGenEventSubConDown*>(CSubConNotificationEvent::NewL(typeId));
	}

CSubConGenEventSubConDown::CSubConGenEventSubConDown()
	: CSubConNotificationEvent()
/** Empty CSubConGenEventSubConDown constructor
*/
	{
	}

TInt CSubConGenEventSubConDown::Error() const
/** Gets Error value.

@return error value. */
	{
	return iError;
	}

void CSubConGenEventSubConDown::SetError(TInt aError)
/** Sets Error value.

@param aError error value. */
	{
	iError = aError;
	}

#endif	// __CS_SUBCONEVENTS_INL__

