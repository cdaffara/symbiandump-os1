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
// Implementation of visitor classes to process MetaDatabase objects
// 
//

/**
 @file 
 @internalTechnology
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypesv1_1_partner.h>
#endif

#include "CommsDatInternalDefs.h"
#include "MetaDatabaseVisitor.h"
#include "CommsDat.h"

using namespace CommsDat;
using namespace Meta;



//
// These functions don't need to be specialised in derived templated visitors
// 
TMDBVisitorBase::TMDBVisitorBase()
: Meta::MMetaType(), iMaxLength(0), iOwner(0), iElementId(0)
    {   
    }


TMDBVisitorBase::TMDBVisitorBase(TMDBElementId* aElementId)
: Meta::MMetaType(), iMaxLength(0), iOwner(0), iElementId(aElementId) 
    {   
    }


TMDBVisitorBase::~TMDBVisitorBase()
/*
Nothing to delete
*/
    {
    }


void TMDBVisitorBase::SetElementId(TMDBElementId* aElementId)
    {
    if (aElementId)
        {
            iElementId = aElementId;
        }
    }

void TMDBVisitorBase::SetElementId(TMDBElementId aElementId)
    {
    if (aElementId)
        {
            *iElementId = aElementId;
        }
    }

void TMDBVisitorBase::ResetElementId(TInt aRecId)
	{
	*iElementId &= ~KCDChangedFlag;
	*iElementId &= (~ECDHidden);
	if (aRecId)
		{
		*iElementId |= aRecId;
		}
	}

TBool TMDBVisitorBase::IsFieldLoaded()
/*
Has the value been changed by the caller?

@internalComponent
*/  { 
	if (iElementId)
    	{
    	return( (*iElementId & KCDNotNullFlag) == KCDNotNullFlag );
    	}
    	
    return false;
    } 

void TMDBVisitorBase::FieldLoaded()
/*
The value has been loaded from the db

@internalComponent
*/  { 
	if (iElementId)
    	{
    		*iElementId |= KCDNotNullFlag;
            *iElementId &= ~KCDChangedFlag;
    	}
    } 
    
void TMDBVisitorBase::SetFieldNull()
/*
The value is not set in the db

@internalComponent
*/  { 
	if (iElementId)
    	{
    		*iElementId &= ~KCDNotNullFlag;
            *iElementId &= ~KCDChangedFlag;
    	}
    } 



void TMDBVisitorBase::Change()
/*
set the changed flag in element id
*/
    { 
    if (iElementId)
    	{
    		*iElementId |= KCDChangedFlag;
    	}
    } 


void TMDBVisitorBase::Sync()
/*
switch off the changed flag in element id
*/
    { 
    if (iElementId)
    	{
    		*iElementId &= ~KCDChangedFlag;
    	}
    }


TUint32 TMDBVisitorBase::TypeId() 
    {
    if (iElementId)
    	{
    		return *iElementId & KCDMaskShowType;
    	}
    	return 0;
    }


TUint32 TMDBVisitorBase::ElementId()
    {
    if (iElementId)
    	{
    		return *iElementId;
    	}
    	return 0;
    }


void TMDBVisitorBase::SetMaxLength(TInt aLength)
    {
    iMaxLength = aLength;
    }


void TMDBVisitorBase::SetOwner(CMDBElement* aOwner)
/*
Set the owner of this element as is needed for callback.
If necessary set the element id or amend it with info from the owner

@internalComponent
*/  
	{ 
    iOwner = aOwner; 

    if (! iElementId)
        {
        iElementId = aOwner->Data();
        }
	else
		{
		*iElementId |= aOwner->ElementId() & (KCDMaskShowRecordId | KCDMaskShowAttributes);
		}
    }

TInt TMDBVisitorBase::GetL(CMDBSessionImpl* /*aSession*/) 
/*
Retrieve this element from the database
not all sub-classes need to provide this function.
*/
    {
    return KErrNotSupported;
    }
    
TBool CompareByRecordId(const TUint32& aFirst, const TUint32& aSecond)
// Utility for RArray Find
    {
    return( (aFirst & KCDMaskShowRecordId) == (aSecond & KCDMaskShowRecordId) );
    }


void TMDBVisitorBase::FindRecordsUsingTemplateL(CMDBSessionImpl* aSession, RArray<TUint32>& aRecordIds, RArray<TUint32>& aCandidateIds)
// because entry exists in template, must include all records that don't have any entries
    {
    // First find all record ids by looking at placeholders
    TUint32 mask = KCDMaskShowType | aSession->GetReadAttributeMask();

    TInt err = aSession->StorageL()->FindL(ElementId() | 0x80000000 | KCDMaskShowFieldType, mask | 0x80000000, aRecordIds);
    
    if (err != KErrNone)
        {
        User::Leave(err); // at this stage know there is something there
        }

    RArray<TUint32> fieldIds;
    CleanupClosePushL(fieldIds);
 

    // then find all fields not using template
    err = aSession->StorageL()->FindL(ElementId() & KCDMaskHideAttrAndRes, mask, fieldIds);

    // The fields not found are the records using the template so add the difference to the list of candidates
    for ( TInt i = 0; i < (aRecordIds.Count() - 1); i++ ) // don't ever want to include the table id.
        {
        if ( KErrNotFound == fieldIds.Find(aRecordIds[i], CompareByRecordId) )
            {
            aCandidateIds.AppendL(aRecordIds[i]);
            }
        }
    CleanupStack::PopAndDestroy(&fieldIds); 
    }



//EOF
