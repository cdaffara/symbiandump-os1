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
// NB - no mapping necessary at the table level
// 
//

/**
 @file MetaDatabaseVisitorRecordSet.cpp
 @internalComponent
*/
 

#include "CommsDatInternalDefs.h"
#include "CommsDatTypesV1_1.h"
#include "MetaDatabaseVisitor.h"
#include "CommsDat.h"
#include "Commsdat_Log.h"


using namespace CommsDat;

using namespace Meta;



CommsDat::TMDBRecordSetVisitor::TMDBRecordSetVisitor(RPointerArray<CMDBRecordBase>* aData)
: TMDBVisitorBase(0), iTarget(aData)
/**
 * Constructor
 */
    {
    }

CommsDat::TMDBRecordSetVisitor* CommsDat::TMDBRecordSetVisitor::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a RecordSet Visitor of a particular type.
 * Used for attribute registration (in the data v-table).
 */
    {
    return static_cast<CommsDat::TMDBRecordSetVisitor*>( ::new ((TUint8*)aMem) CommsDat::TMDBRecordSetVisitor( (RPointerArray<CMDBRecordBase>*)(aData) ) );
    }


TInt CommsDat::TMDBRecordSetVisitor::Changed() const
/*
this composite class always returns changed so iterator will look for changed members.
*/
    {
    return 1;
    }   


TInt CommsDat::TMDBRecordSetVisitor::LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags) 
/*
Assumes type and record id set by caller

@internalComponent
*/
    {    

    RArray<TUint32> ids;
    CleanupClosePushL(ids);

    // find out record ids from storage server
    TUint32 mask = KCDMaskShowType | aSession->GetReadAttributeMask() | KCDUtilityFlag;
  //  TUint32 mask = KCDMaskShowType  | (~aAttributeFlags & KCDMaskShowAttributes) | KCDUtilityFlag;
    TUint32 id = (ElementId() & (KCDMaskShowRecordType | aSession->GetReadAttributeMask())) | KCDMaskShowFieldType | (aAttributeFlags & aSession->GetReadAttributeMask()) ; // look for nodes

    TInt err = aSession->StorageL()->FindL( id, mask, ids );
        
    if ( err != KErrNone && err != KErrNotFound )
        {
        CleanupStack::PopAndDestroy(&ids);
        User::Leave(err);
        }
    
    if (ids.Count() == 0)
        {
        err = KErrNotFound;
        }

    else
        {
        err = LoadL(aSession, aRecordId, aAttributeFlags, ids);
        }

    CleanupStack::PopAndDestroy(&ids);
    return err;
    }


TInt CommsDat::TMDBRecordSetVisitor::LoadL(CMDBSessionImpl* aSession, TMDBElementId /*aRecordId*/, TMDBAttributeFlags /*aAttributeFlags*/, RArray<TUint32>& aIds)    
/* 
Load all records of this type
When re-loading a record set, all data will be overwritten with the current data from the database
// KErrNotFound
// KErrPermissionDenied

@internalComponent
*/
    {
    if (!iTarget)
        {
        User::Leave(KErrNotReady);
        }
    
    // just a check.  Should not be empty
    if ( aIds.Count() == 0)
        {
        return KErrNotFound;
        }    
    
    TInt i(0);

    TUint32 idMask = KCDMaskShowType | KCDMaskShowRecordId | KCDMaskShowAttributes;
    
    //TUint32 idMask = KCDMaskShowType | KCDMaskShowRecordId | (~aAttributeFlags & KCDMaskShowAttributes);

    if ( iTarget->Count() == 0 )
        {
        for ( i = 0; i < aIds.Count(); i++ )
            {
            // Don't want to show the column id here or any attributes that shouldn't be visible
            TUint32 id = aIds[i] & idMask;
            if ( id != 0  &&  
                (id & KCDMaskShowRecordId) != KCDMaskShowRecordId )
                {
                // create each record and load it
                CMDBRecordBase* record = CCDRecordBase::RecordFactoryL(id & ~KCDMaskShowFieldType);
			    CleanupStack::PushL(record);
                record->LoadL(aSession->iOwner);
				iTarget->AppendL(record);
				CleanupStack::Pop(record);
                }
            }
        }  
    else
        {
		if ((*iTarget)[0])
            {
			(*iTarget)[0]->SetElementId(aIds[0] & idMask);
		    }
		
		for (i = 0; i < iTarget->Count(); i++)
            {
            if( (*iTarget)[i] )
                {
                // Check each Target record is included in the list of found ids
                TInt loc = aIds.Find((*iTarget)[i]->ElementId());
                
                if ( loc >= 0 )
                    {
                    // refresh any record that is in the aIds list
                    (*iTarget)[i]->LoadL(aSession->iOwner);
                    aIds.Remove(loc); 
                    }
                
                if ( loc < 0 )
                    {
                    // remove any record that is not in the aIds list
                   // __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("MMetaDBVisitor::LoadL().  deleting record %08x after err %d"), (*iTarget)[i].ElementId(), err);
	            
                    delete (*iTarget)[i];
                    iTarget->Remove(i);
                    --i;
                    }
                }
            else
                {
                // target record already null so just remove it
                iTarget->Remove(i); // record already null so remove it.     
                --i;
                }
            } 
        
         
        // add any remaining new records left in the approved aIds list to the target container
        for (i = 0; i < aIds.Count(); i++)
            {
            CMDBRecordBase* newRecord = CCDRecordBase::RecordFactoryL(aIds[i] & idMask);
            CleanupStack::PushL(newRecord);
            newRecord->LoadL(aSession->iOwner);
            iTarget->AppendL(newRecord);
            CleanupStack::Pop(newRecord);   
            }
        }
    
    if(iTarget->Count()==0)
    	{
    	//We did not leave with KErrNotFound till now beacuse of hidden records
    	//We now have to leave with KErrNotFound as we have no records at all to give back
    	 User::Leave(KErrNotFound);
    	}
    return KErrNone;
    }

    

TInt CommsDat::TMDBRecordSetVisitor::FindL(CMDBSessionImpl* aSession, RArray<TUint32>& aMatchingIds)
/*
Load all requested fields that match any instantiated values

// KErrNotFound
/ 
@internalComponent
*/
    {         
    // Finding must be done at this level - can't hand off work to records
    
    TInt retval = KErrNone;
   
    if ( iTarget->Count() == 0 )
        {
        return KErrNotFound;
        }
    
    RArray<TUint32> candidates;
    CleanupClosePushL(candidates);

    // Try to find one or more matches in depth-first search
    // assumption is that depth will be v short

    // Act on the first record's data table, not your own.    
    DoFindL(*aSession, *((*iTarget)[0]), retval, candidates, aMatchingIds);

    if ( ! aMatchingIds.Count())
        {
        retval = KErrNotFound;
        }
	
    CleanupStack::PopAndDestroy(&candidates);

    return retval;
    }
 


void CommsDat::TMDBRecordSetVisitor::DoFindL(CMDBSessionImpl& aSession, CMDBElement& aElement, TInt& aErr, RArray<TUint32>& aCandidates, RArray<TUint32>& aMatches)
    {
    SVDataTableEntry const* entry = NULL;
    TMDBVisitorBase*  metaDBVisitor = NULL;   
    TAny* mem[KMMetaDBVisitorMaxSize];
    
    TMetaVTableIterator attribIter(&aElement); 
    
    while ( aErr == KErrNone  &&  (entry = attribIter++) != NULL ) 
        {
        metaDBVisitor = static_cast<TMDBVisitorBase*>(entry->iMetaNewL(mem,aElement.GetAttribPtr(entry->iOffset))); // target

        if (metaDBVisitor)
            {
            metaDBVisitor->SetOwner(&aElement);
            
            if ( metaDBVisitor->Changed() )
                {  
                aErr = metaDBVisitor->FindL(&aSession, aCandidates);

                if (aErr == KErrNone )
                    {
                    aErr = aSession.FindMatchL(aCandidates, aMatches, KCDMaskShowFieldType);
                    }
                }
            }
        }
    }

 
 
TInt CommsDat::TMDBRecordSetVisitor::RefreshL(CMDBSessionImpl* aSession)
/*
Update existing records
Do not add new records
Destroy deleted records only if they have not been changed at all.- but log the error
// KErrNotFound
// KErrPermissionDenied     

@internalComponent
*/
    {        
    for (TInt i = 0; i < iTarget->Count(); i++)
        {
        if((*iTarget)[i])
            {
            (*iTarget)[i]->RefreshL(aSession->iOwner); 
            }
        }
    
    return KErrNone;
    }

     
     

TInt CommsDat::TMDBRecordSetVisitor::StoreL(CMDBSessionImpl* aSession, TMDBElementId /*aRecordId*/, TMDBAttributeFlags /*aAttributeFlags*/)
/*
Create new records in the repository
Will Leave on failure with the following specific errors (and KErrNoMemory etc)
Stores always done in a batch and will 
// KErrPermissionDenied
// KErrAlreadyExists

@internalComponent
*/
    {         
    for (TInt i = 0; i < iTarget->Count(); i++)
        {
        if((*iTarget)[i])
            {
            (*iTarget)[i]->StoreL(aSession->iOwner); 
            }
        }
    
    return KErrNone;
    }


 
TInt CommsDat::TMDBRecordSetVisitor::ModifyL(CMDBSessionImpl* aSession, TMDBAttributeFlags /*aAttributeFlags*/)
/*
Modify all changed fields in the store
May return
  KErrPermissionDenied
  KErrNotFound
*/
    {          
    for (TInt i = 0; i < iTarget->Count(); i++)
        {
        if((*iTarget)[i])
            {
            (*iTarget)[i]->ModifyL(aSession->iOwner);
            }
        }
    
    return KErrNone;
    }
 

//EOF
