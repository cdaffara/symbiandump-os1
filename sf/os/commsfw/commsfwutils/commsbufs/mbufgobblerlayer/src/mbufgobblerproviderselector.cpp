// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// MbufGobbler selector. Used by the Tier Manager to select the access point below
//  (management plane)
//
//
//  This is a 3-plane comms layer implementation example, which has been customised to be a test layer which gobbles and releases ESOCK MBUFs.
//  The MBuf gobbling functionality can be disabled by undefining the macro SYMBIAN_COMMSFW_MBUF_GOBBLER which is specified in mbufgobblerproviders.mmp.
//  When SYMBIAN_COMMSFW_MBUF_GOBBLER is undefined, the source code specified by mbufgobblerproviders.mmp becomes a pass through layer i.e. it passes the data
//  through to the layer above or below without altering it. This makes it useful as a starting point for implementing your own layers / providers;
//  useful documentation on how to customise your own passthrough layer can be found in ..\docs\MbufGobblerLayer.doc
//

/**
 @file
 @internalComponent
*/

#include "mbufgobblerproviderselector.h"
#include "mbufgobblerlog.h"

//extra includes 
#include <ss_select.h> //needed as declares MProviderSelector. 
#include <metadatabase.h> //needed for CommsDat::CMDBSession
#include <ss_tiermanagerutils.h> //needed for ESock::TierManagerUtils
#include <commsdattypesv1_1_partner.h> //needed for CCDTierRecord (iTierRecord)

#include "mbufgobbler_panic.h" //needed for ASSERT_DEBUG
#include "mbufgobblertiermanagerfactory.h" //needed for CMbufGobblerTierManagerFactory::iUid
#include "mbufgobblermetaconnproviderfactory.h" //needed for CMbufGobblerMetaConnectionProviderFactory::iUid


ESock::MProviderSelector* CMbufGobblerProviderSelector::NewL(const Meta::SMetaData& aSelectionPreferences)
	{
	//LOG_STATIC_FUNC
	ASSERT_DEBUG(aSelectionPreferences.IsTypeOf(ESock::TSelectionPrefs::TypeId()), EUnexpectedTSelectionPrefsType);
	
	CMbufGobblerProviderSelector* self = NULL;
	self = new(ELeave) CMbufGobblerProviderSelector(aSelectionPreferences);
	CleanupStack::PushL(self);
	
	CommsDat::CMDBSession* dbs = CommsDat::CMDBSession::NewLC(KCDVersion1_2);
	
	self->iTierRecord = ESock::TierManagerUtils::LoadTierRecordL(TUid::Uid(CMbufGobblerTierManagerFactory::iUid), *dbs);
	
	ASSERT_DEBUG(static_cast<TUint32>(self->iTierRecord->iDefaultAccessPoint)!=0,EUnexpectedTSelectionPrefsType);
	
	CleanupStack::Pop(dbs);
	CleanupStack::Pop(self);
	ASSERT_DEBUG(self->iDbs == NULL, EInvalidNullPtr);
	self->iDbs = dbs;
	return self;
	}

CMbufGobblerProviderSelector::CMbufGobblerProviderSelector(const Meta::SMetaData& aSelectionPreferences)
: ASimpleSelectorBase(aSelectionPreferences)
	{
	}

CMbufGobblerProviderSelector::~CMbufGobblerProviderSelector()
	{
	}

void CMbufGobblerProviderSelector::Destroy()
	{
	delete this;
	}


/**
Find or create a Mbufgobbler Meta Connection Provider
*/
ESock::CMetaConnectionProviderBase* CMbufGobblerProviderSelector::FindOrCreateProviderL(TUint aAccessPoint)
	{
	return ASimpleSelectorBase::FindOrCreateProviderL(aAccessPoint);
	}


/**
Perform the selection of a Meta Connection Provider
*/
void CMbufGobblerProviderSelector::SelectL(ESock::ISelectionNotify& aSelectionNotify)
	{	
	ASSERT_DEBUG(iDbs, EInvalidNullPtr);
	ASSERT_DEBUG(iTierRecord, EInvalidNullPtr);
	TUint32 defaultAccessPoint = iTierRecord->iDefaultAccessPoint;
	
	//Must be set
	User::LeaveIfError(defaultAccessPoint!=0 ? KErrNone : KErrCorrupt);
	
	// Send select complete with the provider, and the final select complete
	aSelectionNotify.SelectComplete(this, FindOrCreateProviderL(defaultAccessPoint));
	aSelectionNotify.SelectComplete(this, NULL);
	}
