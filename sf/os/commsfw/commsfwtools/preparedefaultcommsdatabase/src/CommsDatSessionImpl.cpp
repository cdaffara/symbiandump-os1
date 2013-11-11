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
// Implementation of comms database session functions
// dealing with :
// session creation and destruction
// data version checking
// transaction management
// containment of helper classes for notification, mapping, database interaction etc    
// 
//

/**
 @file 
 @internalComponent
*/


#include "CommsDatInternalDefs.h"
//#include <commsdattypeinfov1_1.h>
#include <comms-infras/commsdatschema.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif

/**
   Last 4 digits of binary UID. This number has no meaning,
   but must be something that is very unlikely to be used by real
   fields in a database.
*/
#define MEANINGLESS_NUMBER 0x4DDB 

using namespace CommsDat; 
using namespace CommsDatInternal;




//
// Utilities for comparing items in arrays

TInt InsertNode(const TUint32& aFirst, const TUint32& aSecond)
/*
Utility function to find node in array
@internalComponent
*/
    {
    TUint mask = KCDMaskShowField | KCDUtilityFlag;
    
    if ((aFirst & mask ) == (aSecond & mask))
        return 0;

    if ((aFirst & mask) < (aSecond & mask))
        return -1;
    
     return 1;
    }

TBool FindNode(const TUint32& aFirst, const TUint32& aSecond)
/*
Utility function to find node in array
@internalComponent
*/
    {
    TUint mask = KCDMaskShowField | KCDUtilityFlag;    
    return (aFirst & mask ) == (aSecond & mask);
    }


TBool CompareElements(const CMDBElement& aFirst, const CMDBElement& aSecond)
/*
Utility function to compare elements in array
@internalComponent
*/
    {
    return  &aFirst == &aSecond;
    }


TInt InsertElements(const CMDBElement& aFirst, const CMDBElement& aSecond)
/*
Utility function to allow insertion of elements in array
@internalComponent
*/
    {
    if (&aFirst == &aSecond)
        return 0;
    if (&aFirst < &aSecond)
        return -1;
    return 1;
    }

TBool CompareElementsById(const CMDBElement& aFirst, const CMDBElement& aSecond)
/*
Utility function to compare elements in array by matching Table, Column and RecordId
@internalComponent
*/
    {
    return  (aFirst.ElementId() & KCDMaskShowField) == (aSecond.ElementId() & KCDMaskShowField);
    }



//



CMDBSessionImpl::CMDBSessionImpl(CMDBSession& aOwner)
/*
Constructor 
@internalComponent
*/
  : iOwner(aOwner), 
    iReadAttributeMask(KCDMaskShowReadAttributes),
    iWriteAttributeMask(KCDMaskShowReadWriteAttributes)
    {
    }


CMDBSessionImpl::~CMDBSessionImpl()
/*
Destructor

@internalComponent
*/
    {   
    Close();
    }
      


    
TInt CMDBSessionImpl::ConstructL( TVersion aRequiredVersion )
/*
Open a session with the storage server and compare data versions
        
The MetaDatabase always attempts to support the required version.  If the versions match,
KErrNone will be returned.  If the versions are not the same, but the requested version
is supported, a warning code will be returned. Otherwise an error will be 
returned.  The latest version is returned in the aLatestVersion parameter for information

@internalComponent
*/
    {
    if (iCommsStorage)
        {
        return KErrNone;
        }
    
#ifdef __FLOG_ACTIVE
    RThread self;
    TBuf<KMaxFullName> name;
    self.FullName(name);
    __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("*** CMDBSessionImpl::ConstructL() constructing session object <%08x> in thread \"%S\""), this, &name);
#endif

    EstablishVersionL(aRequiredVersion); 
 
    // Connect to the repository
    iCommsStorage = ::CRepository::NewL(KCDCommsRepositoryId);
   
    // register for events
#ifndef __TOOLS2__
    TInt err = iCommitSeqProperty.Attach(KUidCommsDatStatusEvent, KCommsDatStatusEventCommitSeq);

    if (err != KErrNone)
        {
        __FLOG_STATIC1(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::ConstructL() failed with error <%d>"), err);                                
        User::Leave(err);    
        }
#endif
        
    return KErrNone;
    }

    
void CMDBSessionImpl::Close()
/* 
Close session with storage server.
No session required with Central repository

@internalComponent
*/
    {
    __FLOG_STATIC1(KLogComponent, KCDInfoLog, _L("*** CMDBSessionImpl::Close() closing session object <%08x>"), this);
#ifndef __TOOLS2__
    TRAPD(
        err, 
        
        // Don't signal any roll-back, the reason we're performing a roll-back is because
        // the session is being CLOSED.
        NotifierL()->SuppressRollBackEvents();
        
        RollbackTransactionL();

        NotifierL()->NotifyClients(TCDNotifiableEvent::EClose);
        );
	err = err;	// suppress "unused-var" warning

    delete iNotifier;
    iNotifier=NULL;
#endif

#ifdef __TOOLS2__
	if (iCommsStorage)
		{
    		iCommsStorage->Flush();
		}
#endif
    delete iCommsStorage;
    iCommsStorage=NULL;    

	iCommitSeqProperty.Close();
    }



TBool CMDBSessionImpl::UsingLatestVersion()
/*
Check the flag that indicates whether using latest version

@internalComponent
*/
    {
    return iUsingLatestVersion;
    }



void CMDBSessionImpl::EstablishVersionL(TVersion aRequiredVersion)
/*
Check the version in use at the moment and set a flag if using latest version
Return an error code KErrDeprecated if version is deprecated
Return KErrNone if the version is current

MAINTENANCE - this function needs updating whenever the latest version changes

@internalComponent
*/
    {
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY  
    if ( aRequiredVersion.iMajor == KCDVersion1_2.iMajor  &&
    	 aRequiredVersion.iMinor == KCDVersion1_2.iMinor  &&
    	 aRequiredVersion.iBuild == KCDVersion1_2.iBuild    )
	    {
        iClientsDataSetVersion = aRequiredVersion;
        iUsingLatestVersion = ETrue;

        __FLOG_STATIC3(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::EstablishVersionL()  Current dataset version <%d.%d.%d> in use.  Mapping disabled"), KCDVersion1_2.iMajor, KCDVersion1_2.iMinor, KCDVersion1_2.iBuild);
        
        return;
        }	       
	else if( (aRequiredVersion.iMajor == KCDVersion1_1.iMajor &&
			  aRequiredVersion.iMinor == KCDVersion1_1.iMinor &&
			  aRequiredVersion.iBuild == KCDVersion1_1.iBuild) ||
			  //the orig KCDLatesVersion was (0,0,0)
			 (aRequiredVersion.iMajor == 0 &&
			  aRequiredVersion.iMinor == 0 &&
			  aRequiredVersion.iBuild == 0) )
	    {
        iClientsDataSetVersion = KCDVersion1_1;
        iUsingLatestVersion = EFalse;
        
        __FLOG_STATIC3(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::EstablishVersionL()  Deprecated dataset version <%d.%d.%d> in use.  Mapping enabled"), aRequiredVersion.iMajor, aRequiredVersion.iMinor, aRequiredVersion.iBuild);
        
        return;
	    }
#else
	if( (aRequiredVersion.iMajor == KCDVersion1_1.iMajor &&
	     aRequiredVersion.iMinor == KCDVersion1_1.iMinor &&
	     aRequiredVersion.iBuild == KCDVersion1_1.iBuild) ||
		  //the orig KCDLatesVersion was (0,0,0)
		 (aRequiredVersion.iMajor == 0 &&
		  aRequiredVersion.iMinor == 0 &&
		  aRequiredVersion.iBuild == 0) )
	    {
        iClientsDataSetVersion = KCDVersion1_1;
        iUsingLatestVersion = ETrue;
        
        __FLOG_STATIC3(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::EstablishVersionL()  Current dataset version <%d.%d.%d> in use.  Mapping disabled"), aRequiredVersion.iMajor, aRequiredVersion.iMinor, aRequiredVersion.iBuild);
        
        return;
	    }
#endif    

    // MAINTENANCE - as versions are deprecated, allow them to still be used and return KCDDeprecatedVersion here.
    
    __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("*** CMDBSessionImpl::EstablishVersionL()  Requested dataset version <%d.%d.%d> not supported"), aRequiredVersion.iMajor, aRequiredVersion.iMinor, aRequiredVersion.iBuild);
        
    User::Leave(KErrNotSupported);
    
    return;
    }


//
// Accessors

TVersion CMDBSessionImpl::LatestVersion()
/*
Lookup latest data format version

@Deprecated 
@internalComponent
*/
    {
    return KCDCurrentVersion;
    }


TVersion CMDBSessionImpl::VersionInUse()
/*
Lookup data format version currently in use by client

@internalComponent
*/
    {
    return iClientsDataSetVersion;
    }
 

CRepository* CMDBSessionImpl::StorageL()
/*
Return Storage class for Comms data

@internalComponent
*/
    {
    if (! iCommsStorage)
        {
        iCommsStorage = ::CRepository::NewL(KCDCommsRepositoryId);
        }
    return iCommsStorage;
    }


#ifndef __TOOLS2__
CCDNotifier* CMDBSessionImpl::NotifierL()
/*
return notifier object, creating it if necessary.

@internalComponent
*/
    {
    if (! iNotifier)
        {
        iNotifier = ::CCDNotifier::NewL(*this);
        }
    return iNotifier;
    }
#endif




//
//  TRANSACTIONS WITH THE DATABASE SERVER
//
   
TBool CMDBSessionImpl::MaybeOpenTransactionL()
/*
Start transaction with storage server if available

Returns ETrue if opens transaction
Returns EFalse if already in transaction
    
@internalComponent
*/
    {     
    TBool ownTransaction = EFalse;

    if ( ! iInTransaction )
        {
    	TInt err = StorageL()->StartTransaction(CRepository::EReadWriteTransaction); 

        if (err != KErrNone)
		    {
			__FLOG_STATIC1(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::MaybeOpenTransactionL() - CentralRepository::StartTransactionL() failed with err = %d"), err);
            User::Leave(err);
            }
	    else
		    {
			iInTransaction = ETrue;
			ownTransaction = ETrue;
            }
		}

    return ownTransaction;
    }   




void CMDBSessionImpl::RollbackTransactionL()
/* 
Explicitly request rollback of transaction restoring database state to before transaction started.

@internalComponent
*/
    {
    if (iInTransaction)
	    {
		StorageL()->RollbackTransaction();
                
	    __FLOG_STATIC(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::RollbackTransactionL()  Has rolled back storage server Transaction "));

		iInTransaction = EFalse;
		
#ifndef __TOOLS2__
        //delete notifications if there are any
        NotifierL()->ClearPubSubNotifications();
        
        // Notify any clients.
        NotifierL()->NotifyClients(TCDNotifiableEvent::ERollback);
#endif
	    }
    }

  
TInt CMDBSessionImpl::MaybeCommitTransactionL(TBool aOwnTransaction, CMDBElement* aElement, TInt aErr)
/*

Commit transaction if outstanding and owned by caller

Rollback transaction if an error has occurred

Mapping and notification tasks may be required after commit
        
    Load/Find    ->  map  & sync
    Update       ->  map 
    Store/Modify ->  sync & notify       
    Delete       ->         notify 
    
@internalComponent
*/
    {  
    __FLOG_STATIC3(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::MaybeCommitTransactionL() aOwnTransaction = %b, aElementId = %08x, Err = %d"), aOwnTransaction, aElement ? aElement->ElementId() : 0, aErr);

    // Stop compiler complaining abot unused variables in UREL builds.
    (void)aElement;

	if (aErr != KErrNone)
	    {	
		__FLOG_STATIC2(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::MaybeCommitTransactionL() Commit transaction for element <%08x> failed with error <%d>.  The transaction will be rolled back"), aElement ? aElement->ElementId() : 0, aErr);                                                  
		RollbackTransactionL();
        User::Leave(aErr);
        }

    TInt err(KErrNone);

    if ( aOwnTransaction && iInTransaction )
        {
  		TUint32 errLoc;

#ifndef __TOOLS2__
		// Update the global commit sequence. If we can't fetch the current one then we set it to the FastCounter() as a best effort
		// signal of change to observers
		TInt currCommitSeq;
        if(iCommitSeqProperty.Get(currCommitSeq) != KErrNone)
			{
			currCommitSeq = User::FastCounter();
			}
		iCommitSeqProperty.Set(currCommitSeq + 1);
#endif

        err = StorageL()->CommitTransaction(errLoc);
                
        if (err != KErrNone)  
            {
            __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::MaybeCommitTransactionL() CentralRepository::CommitTransaction failed with err = %d at %08x"), err, errLoc);

			RollbackTransactionL();
            
            User::Leave(err);
            }
        

    
#ifndef __TOOLS2__
        //notify any changes via pubsub if necessary
        NotifierL()->NotifyAllChanges();

        // Notify any clients.
        
        /*
        NOTE: Documented or not, our Licensees expect EUnlock notification to be returned if
        CommitTransaction didn't have anything to commit (probably because user container remained unchanged or
        the user used it as a read transaction). So we are to notify with ECommit, if and only the database actually changed.        
        
        This behaviour SHOULD not be changed without a BC break. Death, Destruction, rage, pillage and horror will follow if not.. 
        */
        if(errLoc > 0)
        	{
        	NotifierL()->NotifyClients(TCDNotifiableEvent::ECommit);	
        	}       	
        else
        	{
        	//errLoc will be 0 if the db wasn't changed..
        	NotifierL()->NotifyClients(TCDNotifiableEvent::EUnlock);	
        	}
#endif
         
        iInTransaction = EFalse;
        }
    
    return aErr;
    }


TBool CMDBSessionImpl::IsInTransaction()
/* 
Query if transaction is already underway in this session

@internalComponent
*/
    {
    //__FLOG_STATIC1(KLogComponent, KCDInfoLog, _L("IsInTransaction - returning = %b "), iInTransaction );
					    
    return iInTransaction;
    }


void CMDBSessionImpl::SetAttributeMask(TMDBAttributeFlags aAttributeMask)
/*
CLEAR flags from read and write attribute masks so that attributes are ignored

@internalComponent
*/
    {
    TUint32 tmpWrite = (~aAttributeMask & KCDMaskShowReadWriteAttributes);
	iWriteAttributeMask &= tmpWrite;
    
    TUint32 tmpRead = (~aAttributeMask & KCDMaskShowReadAttributes);
    iReadAttributeMask &= tmpRead;

    __FLOG_STATIC3(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::SetAttributeMask() aAttributeMask = %08x, iReadAttributeMask now = %08x. iWriteAttributeMask now = %08x"), aAttributeMask, iReadAttributeMask, iWriteAttributeMask);
    }


TBool CMDBSessionImpl::IsSetAttributeMask(TMDBAttributeFlags aAttributeMask)
/*
Check flags in read and write attribute masks as requested

@internalComponent
*/
    {
    TUint32 currentMask = (iWriteAttributeMask | iReadAttributeMask);
    TBool retval = ((currentMask & (~aAttributeMask & KCDMaskShowAttributes)) == currentMask);
	return retval;
    }



void CMDBSessionImpl::ClearAttributeMask(TMDBAttributeFlags aAttributeMask)
/*
SET flags in read and write attribute masks so that attributes are obeyed

@internalComponent
*/
    {
    TUint32 tmpWrite = (aAttributeMask & KCDMaskShowReadWriteAttributes);
	iWriteAttributeMask |= tmpWrite;
    
    TUint32 tmpRead = (aAttributeMask & KCDMaskShowReadAttributes);
    iReadAttributeMask |= tmpRead;

    __FLOG_STATIC3(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::ClearAttributeMask() aAttributeMask = %08x, iReadAttributeMask now = %08x. iWriteAttributeMask now = %08x"), aAttributeMask, iReadAttributeMask, iWriteAttributeMask);
    }


TMDBAttributeFlags CMDBSessionImpl::GetWriteAttributeMask() 
/*
Return current write attribute mask

@internalComponent
*/
    {
    return iWriteAttributeMask;
    }


TMDBAttributeFlags CMDBSessionImpl::GetReadAttributeMask() 
/*
Return current read attribute mask

@internalComponent
*/
    {
    return iReadAttributeMask;
    }


void CMDBSessionImpl::ReadAllowedL(TMDBElementId aElementId)
/*
If one or more read attributes are SET in the aElementId and in the write attribute mask, 
the session does not have permission to read this element.
*/
	{
    if ( (iReadAttributeMask & (aElementId & KCDMaskShowReadAttributes) ) )
		{
        __FLOG_STATIC1(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::ReadAllowed() This client does not have the correct access attributes to view Element <%08x>"), aElementId);                                
		User::Leave(KErrPermissionDenied);
		}
 	}

void CMDBSessionImpl::WriteAllowedL(TMDBElementId aElementId)
/*
If one or more write attributes are SET in the aElementId and in the write attribute mask, 
the session does not have permission to write or update this element.
*/
	{
	if ( (iWriteAttributeMask & (aElementId & KCDMaskShowReadWriteAttributes) ) )
		{
		__FLOG_STATIC1(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::WriteAllowed() This client does not have the correct access attributes to write Element <%08x>"), aElementId);                                
		User::Leave(KErrPermissionDenied);
		}
	}

void CMDBSessionImpl::DeleteAllowedL(TMDBElementId aElementId)
/*
If one or more write attributes are SET in the aElementId and in the write attribute mask, 
the session does not have permission to delete this element.

However ECDNoWriteButDelete must be ignored in the writeAttributeMask as it is anomalous
for BC reasons
*/
	{
    if ( iWriteAttributeMask & (aElementId & (KCDMaskShowReadWriteAttributes & ~ECDNoWriteButDelete)) )
		{
		__FLOG_STATIC1(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::DeleteAllowed() This client does not have the correct access attributes to delete Element <%08x>"), aElementId);                                
		User::Leave(KErrPermissionDenied);
		}
	}
TInt CMDBSessionImpl::LoadNodeAttributesL(TMDBElementId& aElementId)
/*
Find out appropriate attributes for this node id by checking specified node 
and gathering attributes from parent nodes if aElementId does not exist in the database.

Return KErrNone if the node does exist and gather attributes from the existing node into aElementId
Return KErrNotFound if node does not exist, but gather attributes from parent nodes anyway (if they exist)

Leave with KErrPermissionDenied if database forbids access to any element.

@internalComponent
*/
    {
    ASSERT(aElementId);

	TInt retval(KErrNotFound);
	
	if(!(CommsDatSchema::IsNewRecordRequest(aElementId) || CommsDatSchema::IsNewColumnRequest(aElementId)))
		{
		retval = FindElementInDatabaseL(aElementId);        
		}

	if ( retval == KErrNotFound && ! CommsDatSchema::IsTable(aElementId) )
		{
		// This must be a new field or node so gather attributes from parent nodes, 
		// Report if don't find parent Record or Table nodes, 
		TInt err(GatherParentAttributesL(aElementId));

		if ( err != KErrNone  )
			{
			// don't worry about this, but just log it as might be of interest
			__FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::LoadNodeAttributes for id %08x.  GatherParentAttributesL returned err %d"), aElementId, err);
			}
		}	   

    return retval;
	}


TInt CMDBSessionImpl::GatherParentAttributesL(TMDBElementId& aElementId)
// Get table attributes for any record or column node
// Get Record and Column attributes for any field

// Do not call this function to find a Table placeholder and element directly

// Leave with  KErrAccessDenied if cannot access a node.
// return with KErrNotFound if expected parent node is not present

// May leave with other errors from repository
// May return with other error codes from repository
	{
	ASSERT(aElementId);
	
	TInt retval(KErrNotFound);
	TMDBElementId id(aElementId);

	if ( CommsDatSchema::IsNode(aElementId) )
		{
		// element is a record or column, or table, so load Table attributes 
 		id |= (KCDMaskShowRecordId | KCDMaskShowFieldType);
                                
		retval = FindElementInDatabaseL(id);

		aElementId |= (id & KCDMaskShowAttributes);
		}
	else
		{
		// Element is a field so get attributes from the parent record 
		id = aElementId | KCDMaskShowFieldType;

		retval = FindElementInDatabaseL(id);
		
		aElementId |= (id & KCDMaskShowAttributes);
		
		// Finally, get extra attributes from parent column node if the node exists (it may not)
		id = aElementId | KCDMaskShowRecordId;
        
		if ( KErrNone == FindElementInDatabaseL(id) )
			{
			aElementId |= (id & KCDMaskShowAttributes);
			}
		}
	return retval;
	}


//
// NODE MANAGEMENT FUNCTIONS ...
//

TInt CMDBSessionImpl::FindElementInDatabaseL(TMDBElementId& aElementId)
/*
Locates a single element in the database, retrieving the full id of the element including its attributes

Only locates one field, node or placeholder

returns EFalse if the element is not found
returns ETrue if the element is found

If the client process does not have the capabilities to read this element
this function will leave with KErrPermissionDenied

*/
	{	
    ASSERT(aElementId);
		
	// Find this element with any attributes so only specify the <T><C><R>, 
	// but don't want placeholder.as well as node so pay attention to the utility flag
    TUint32 mask(KCDUtilityFlag | KCDMaskShowField); 

	//__FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::LocateElementInDatabaseL for id %08x.  CRepository::FindL using mask %08x"), aElementId, mask);

	RArray<TUint32> ids;
    CleanupClosePushL(ids);

	// set the utility flag so don't find placeholder.  Will leave if permission denied
	TInt retval = StorageL()->FindL((aElementId & ~KCDUtilityFlag), mask, ids);

    if (ids.Count())
        {
        // element already exists in database so set attributes
        aElementId = (aElementId & ~KCDMaskShowAttrAndRes) | (ids[0] & KCDMaskShowAttributes);

#ifdef __DEBUG
	    if (ids.Count() > 1)
			{
			// Must not find more than one this would be a fundamental error!
			__FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::LocateElementInDatabase() for id <%08x>.  CRepository::FindL  with mask %08x returned %d entries.  Should only be 1"), aElementId, mask, ids.Count());
			}
#endif
        }
	    
	CleanupStack::PopAndDestroy(&ids);

	if ( retval != KErrNone && retval != KErrNotFound )
		{
		__FLOG_STATIC2(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::LocateElementInDatabase() for id <%08x>.  CRepository::FindL. Returned err was %d"), aElementId, retval);
		}
	
	return retval;
	}


TInt CMDBSessionImpl::LoadElementAttributesL(TMDBElementId& aElementId)
/*
Find out appropriate attributes for this element id by checking nodes

  If element is table just look for corresponding table node
  If element is record, get attributes from existing record node or from table node if record doesn't yet exist
  If element is column, get attributes from existing column node or from table node if column doesn't yet exist

  If element is field, get attributes from existing record and column too, where that has been set.

  If node does not exist, return KErrNotFound

  But if element is single field and does not exist set attributes from the node and return KErrNone unless parent
  node doesn't exist in which case return KErrNotFound

  In non-user-defined record types, column nodes are only set when attributes differ from general table attributes. 
  In user defined record types, column nodes are always set.

@internalComponent
*/
    {
    TInt retval(KErrNotFound);
	
    if( aElementId )
	    {
		RArray<TUint32> ids;
        CleanupClosePushL(ids);

        // first look for the element itself directly...

        // find this element with any attributes, but don't find placeholders.
        TUint32 mask(KCDUtilityFlag | KCDMaskShowField); 

      //  __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::LoadElementAttributes for id %08x.  CRepository::FindL using mask %08x"), aElementId, mask);
	    
    	retval = StorageL()->FindL((aElementId & ~KCDUtilityFlag), mask, ids);

        if (ids.Count() > 1)
           {
           // don't expect to find more than one !!
		   __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::LoadElementAttributes() for id <%08x>.  CRepository::FindL()  with mask <%08x> returned %d entries.  Should only be 1"), aElementId, mask, ids.Count());
           }
        

		if ( retval != KErrNone && CommsDatSchema::IsNode(aElementId))
			{
			if (retval != KErrNotFound)
				{
				__FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::LoadElementAttributes for id %08x.  CRepository::FindL. mask %08x Returned err was %d"), aElementId, mask, retval);
				}
			}
		else
            {
            retval = KErrNotFound;

	        if (ids.Count())
                {
                // node already exists so set attributes
	            aElementId = (aElementId & ~KCDMaskShowAttrAndRes) | (ids[0] & KCDMaskShowAttributes);
                retval = KErrNone;
                }
		    else
		        {
                // Need to gather attributes from parent nodes to ensure new element is created appropriately
                // but error is still KErrNotFound

              //  __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::LoadElementAttributes for id %08x.  CRepository::FindL with mask %08x returned no entries.  Checking parent nodes"), aElementId, mask);

                TMDBElementId id(aElementId);

                if ( CommsDatSchema::IsRecord(aElementId) || CommsDatSchema::IsColumn(aElementId) )
                    {
                    // Get attributes from parent table
                    
                    id |= (KCDMaskShowRecordId | KCDMaskShowFieldType);
                    
                    // Last step in (potentially) recursive call chain (note we deliberately drop the KErrNone return value).
                    if (LoadElementAttributesL(id) == KErrNone)
                        {
                        aElementId |= (id & KCDMaskShowAttributes);
                        }
                    }
                else if ( !CommsDatSchema::IsTable(aElementId) )
                    {
                    // This is a single field.  
                    // Get attributes from the parent record...
                    
                    id = aElementId | KCDMaskShowFieldType;

                    if ( LoadElementAttributesL(id) == KErrNone )
                        {
                        aElementId |= (id & KCDMaskShowAttributes);

                        // ...and might need to get attributes from the column node too..., 
                        id = aElementId | KCDMaskShowRecordId;
                        if ((retval = LoadElementAttributesL(id)) == KErrNone)
                            { 
                            aElementId |= (id & KCDMaskShowAttributes);
                            }
                        }
                    }
		        }
            }

        if ( retval != KErrNone && retval != KErrNotFound )
            {
            __FLOG_STATIC3(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::LoadElementAttributesL() for id <%08x>  CRepository::FindL with mask %08x returned err %d"), aElementId, mask, retval);
            }
    
        CleanupStack::PopAndDestroy(&ids);
        }	   

    return retval;
	}

void CMDBSessionImpl::SetNodeIdL(TMDBElementId& aElementId, TMDBElementId aNodeTypeMask, TMDBElementId aInitialId, TMDBElementId aMaxId, TInt aMaxVal)
/*
Set id to a new node in the database recording the item's attributes etc.
Will leave with KErrOverflow if there is no more space to create a new item
*/
	{
    // First see which placeholder nodes already exist at this level
    // placeholder nodes will never have any attributes set and will always be readable by those who can write to central repository
    
    __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::SetNodeIdL() for id <%08x>  CRepository::FindL using mask <%08x>"), aElementId, aNodeTypeMask);

    RArray<TUint32> ids;
    CleanupClosePushL(ids);

    TInt err = StorageL()->FindL(aElementId, aNodeTypeMask, ids);
    
    
	//Ids now contains all the nodes for all the tables, Filter out those only for generic tables
	if(aMaxVal == KCDMaxUserDefTables)
		{
		TInt i(ids.Count());
		while ( i > 0 )
            {
            i--;
            if((ids[i] & KCDMaskShowRecordType) < KCDInitialUDefRecordType)
            	{
            	ids.Remove(i);	            		            	
            	}	            
            }  
        ids.Compress();		
		}

    if (ids.Count() >= aMaxVal)
        {
		err = KErrOverflow;
        }

    if (err != KErrNone  &&  err != KErrNotFound)
        {
        __FLOG_STATIC1(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::SetNodeIdL for id %08x.  CRepository::FindL returned no entries.  Should have found 1"), aElementId);
	    
        CleanupStack::PopAndDestroy(&ids);
        User::Leave(err);
        }
    else 
        {
         while ( ids.Count() > 0   &&  (ids[ids.Count() - 1] & aMaxId) == aMaxId )
                {
                ids.Remove(ids.Count() - 1);
                ids.Compress();
                }
            

        if( ids.Count() == 0 || 
		   ( ids.Count() == 1  &&  (ids[0] & aMaxId) == 0 ) ) // just default entry
            {   
            if(aMaxVal == KCDMaxUserDefTables)
				{
				//if for generic table request
            	aElementId |= KCDInitialUDefRecordType;
				}
			else
				{
				aElementId |= aInitialId;	
				}
	        }
        else if (ids.Count() == 1)
            {
            TUint32 temp = (ids[0] & aMaxId);
            temp += aInitialId;
            /**
            if the remaining recordId is FE (AccessPointPrioritySelectionPolicy record) then the new record id
            should be 1 and not FF.
            */
            if (temp == aMaxId)
                {
                temp = KCDInitialRecordId;
                }
            aElementId |= temp;
            }
	    else 
            {
            TMDBElementId testId = (ids[ids.Count() - 1] & aMaxId) + aInitialId;
      
            if ( testId  <  aMaxId )
		        {
			    // assign next free id at end of list
			    aElementId |= testId; 
		        }
		    else 
			    {
                testId = ids[0] & aMaxId;

                if ( testId > aInitialId )
                    {
                    // assign free id from the beginning of the list
                    aElementId |= aInitialId;
                    }
                else
                    {
                    // assign first free id found in the list 
			        for (TInt i = 0; i < ids.Count() -1; i++)
			            {
                        testId = (ids[i] & aMaxId) + aInitialId;
                        
			            if ( (ids[i+1] & aMaxId)  >  testId )
                            {
                            aElementId |= testId;
                            break;
                            }
			            }
                    }
                }
            }      	
    
        __FLOG_STATIC1(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::SetNodeIdL() successfully set aElementId to <%08x>"), aElementId);
       
        CleanupStack::PopAndDestroy(&ids);
        }
    }


void CMDBSessionImpl::CreatePlaceholderL(TMDBElementId& aElementId, TMDBElementId aNodeTypeMask, TDesC& aTableName)
/*
Create placeholdertype info node which must never have any attributes set

   Table   <U><T><ef><ff><0>  Int
   Record  <U><T><ef><R><0>   Int
   Column  <U><T><C><ff><0>   Int
*/    
    {
    TInt err(KErrNone);
    // always might need to create a table placeholder - create one if client is storing
    // first record or column in a table as well as explicitly creating the table
    TUint32 id = (aElementId & KCDMaskShowRecordType) | (KCDNewTableRequest & ~KCDChangedFlag) | KCDUtilityFlag ;

    RArray<TUint32>ids;
    CleanupClosePushL(ids);

    TUint32 tablePlaceholder = id | KCDMaskShowFieldType | KCDMaskShowRecordId | KCDUtilityFlag;
    if (KErrNotFound == StorageL()->FindL(tablePlaceholder, KCDCenRepNoMask, ids))
        {
    	err = StorageL()->Create(tablePlaceholder, MEANINGLESS_NUMBER); 
        if (err != KErrNone)
            {  
            __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::CreatePlaceholderL() for id <%08x>, prepared id <%08x>  CRepository::Create() for missing table returned %d"), aElementId, tablePlaceholder, err);
            }
        else
            {
            if (aNodeTypeMask != KCDMaskShowRecordType)
                {
                TUint32 tableNode = tablePlaceholder & ~KCDUtilityFlag;

/*                if (CommsDatSchema::IsGenericRecord(aElementId))
                    {
                    // Not using dynamic_cast<>() here. RTTI is disabled (winscw).
                    // 
                    // Presumably this works OK if the iRecordType is "null".
                    err = StorageL()->Create(tableNode, static_cast<CMDBGenericRecord*>(iCurrentRecord)->iRecordType);  
                    }
                else
                    {
                    // and now create a node too, but without any attributes and without a table name
                    // because don't know what these should be as just storing first record or column
*/                    err = StorageL()->Create(tableNode, aTableName);  
//                    }                

                if (err != KErrNone)
                    {
                    // unexpected error that should be logged and passed back to caller
                    __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::CreatePlaceholderL() creating Node for id <%08x>, prepared id <%08x>  CRepository::Create() returned %d"), aElementId, tableNode & ~KCDUtilityFlag, err);
                    }
                }
            } 
        }
    CleanupStack::PopAndDestroy(&ids);

    if (aNodeTypeMask != KCDMaskShowRecordType)
        {
        // create the other node types as required

        switch(aNodeTypeMask)
            {
            case KCDMaskShowRecordId :
                {
                id = (aElementId & (KCDMaskShowRecordId | KCDMaskShowRecordType)) | (KCDNewRecordRequest & KCDMaskHideRes) | KCDUtilityFlag;
                break;
                }
            case KCDMaskShowFieldType :
                {
                id = (aElementId & (KCDMaskShowFieldType | KCDMaskShowRecordType)) | (KCDNewColumnRequest & KCDMaskHideRes) | KCDUtilityFlag;
                break;
                }
            };
       
        err = StorageL()->Create(id, MEANINGLESS_NUMBER);
    
        if (err != KErrNone)
            {
            __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::CreatePlaceholderL() for id <%08x>  CRepository::Create() used id <%08x> and returned %d"), aElementId, id, err);
            }
        }
    }

void CMDBSessionImpl::CreatePlaceholderL(TMDBElementId& aElementId, TMDBElementId aNodeTypeMask, CMDBElement& aElement)
/*
Create placeholdertype info node which must never have any attributes set

   Table   <U><T><ef><ff><0>  Int
   Record  <U><T><ef><R><0>   Int
   Column  <U><T><C><ff><0>   Int
*/    
    {
    TInt err(KErrNone);
    // always might need to create a table placeholder - create one if client is storing
    // first record or column in a table as well as explicitly creating the table
    TUint32 id = (aElementId & KCDMaskShowRecordType) | (KCDNewTableRequest & ~KCDChangedFlag) | KCDUtilityFlag ;

    RArray<TUint32>ids;
    CleanupClosePushL(ids);

    TUint32 tablePlaceholder = id | KCDMaskShowFieldType | KCDMaskShowRecordId | KCDUtilityFlag;
    if (KErrNotFound == StorageL()->FindL(tablePlaceholder, KCDCenRepNoMask, ids))
        {
        err = StorageL()->Create(tablePlaceholder, MEANINGLESS_NUMBER); 
        if (err != KErrNone)
            {  
            __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::CreatePlaceholderL() for id <%08x>, prepared id <%08x>  CRepository::Create() for missing table returned %d"), aElementId, tablePlaceholder, err);
            }
        else
            {
            if (aNodeTypeMask != KCDMaskShowRecordType)
                {
                TUint32 tableNode = tablePlaceholder & ~KCDUtilityFlag;

                if (CommsDatSchema::IsGenericRecord(aElementId))
                    {
                    // Not using dynamic_cast<>() here. RTTI is disabled (winscw).
                    // 
                    // Presumably this works OK if the iRecordType is "null".
                    err = StorageL()->Create(tableNode, static_cast<CMDBGenericRecord*>(&aElement)->iRecordType);  
                    }
                else
                    {
                    // and now create a node too, but without any attributes and without a table name
                    // because don't know what these should be as just storing first record or column
                    err = StorageL()->Create(tableNode, KNullDesC);  
                    }                

                if (err != KErrNone)
                    {
                    // unexpected error that should be logged and passed back to caller
                    __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::CreatePlaceholderL() creating Node for id <%08x>, prepared id <%08x>  CRepository::Create() returned %d"), aElementId, tableNode & ~KCDUtilityFlag, err);
                    }
                }
            } 
        }
    CleanupStack::PopAndDestroy(&ids);

    if (aNodeTypeMask != KCDMaskShowRecordType)
        {
        // create the other node types as required

        switch(aNodeTypeMask)
            {
            case KCDMaskShowRecordId :
                {
                id = (aElementId & (KCDMaskShowRecordId | KCDMaskShowRecordType)) | (KCDNewRecordRequest & KCDMaskHideRes) | KCDUtilityFlag;
                break;
                }
            case KCDMaskShowFieldType :
                {
                id = (aElementId & (KCDMaskShowFieldType | KCDMaskShowRecordType)) | (KCDNewColumnRequest & KCDMaskHideRes) | KCDUtilityFlag;
                break;
                }
            };
       
        err = StorageL()->Create(id, MEANINGLESS_NUMBER);
    
        if (err != KErrNone)
            {
            __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::CreatePlaceholderL() for id <%08x>  CRepository::Create() used id <%08x> and returned %d"), aElementId, id, err);
            }
        }
    }


void CMDBSessionImpl::CreateNodeL(TMDBElementId& aElementId, TMDBElementId aNodeTypeMask, CMDBElement& aElement, SGenericRecordTypeInfo* aFieldTypeInfo)
/*  
Create a new node that shows the appropriate attributes and stores type information
given in the element passed in to the function
   Table   <T><ef><ff><0>  Text
   Record  <T><ef><R><0>   Int
   Column  <T><C><ff><0>   Bin
*/
    {
    TInt err(KErrNone);
    TUint32 id = aElementId;
    switch(aNodeTypeMask)
        {
        case KCDMaskShowRecordType :
            {
            CMDBTextFieldBase& ref = static_cast<CMDBTextFieldBase&>(aElement);
			id = (aElementId & (KCDMaskShowRecordType | iWriteAttributeMask)) | KCDNewTableRequest;
            __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::CreateNodeL() for id <%08x>  prepared id is <%08x>"), aElementId, id);
            if (aElement.IsNull())
                {
                TPtrC nothing(_L("_"));
                err = StorageL()->Create(id, nothing);
                }
            else
                err = StorageL()->Create(id, (TDesC&)ref); 
            break;
            }
        case KCDMaskShowRecordId :
            {
            id = (aElementId & (KCDMaskShowRecordId | KCDMaskShowRecordType | (~iWriteAttributeMask & KCDMaskShowAttributes)));
            id |= (KCDNewRecordRequest & KCDMaskShowFieldType);
            __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::CreateNodeL() for id <%08x>  prepared id is <%08x>"), aElementId, id);
			err = StorageL()->Create(id, MEANINGLESS_NUMBER);
            break;
            }
        case KCDMaskShowFieldType :
            {
        //    CMDBBinFieldBase& ref = static_cast<CMDBBinFieldBase&>(aElement);
            id = (aElementId & (KCDMaskShowFieldType | KCDMaskShowRecordType | iWriteAttributeMask)) | (KCDNewColumnRequest & KCDMaskShowRecordId);
            __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::CreateNodeL() for id <%08x>  prepared id is <%08x>"), aElementId, id);
            if (aFieldTypeInfo)
                {
                // Serialize to a binary "stream" the field info.
                TPckg<SGenericRecordTypeInfo> serializedTypeInfo(*aFieldTypeInfo);
                err = StorageL()->Create(id, serializedTypeInfo);
                }
            break;
            }
        };
    
    
    if (err != KErrNone)
        {
        __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::CreateNodeL() for id %08x, prepared id %08x  CRepository::Create() returned %d"), aElementId, id, err);
        }

    }



void CMDBSessionImpl::MaybeCreateNodeL(CMDBElement& aElement)
/*
Check to see if a new node needs to be created and if so, create it.
Never allows creation of deprecated record types
    
else lookup attributes
	
Will leave with
    KErrAlreadyExists if requested node already exists.
    KErrOverflow if no space to store a new node
    KErrNotSupported if client asks to create deprecated record (will only get here
    if mapping turned off because current version of session chosen by client)
@internalComponent
*/
    {    
	TMDBElementId elementId = aElement.ElementId();

    // Checking for nodes the user has asked to be created

    if ( (elementId & KCDMaskHideAttrAndRes) == KCDNewTableRequest )
        {
       	//Not supported at the moment
        }
    else
    	{
    	TInt err(KErrNotFound);
		TPtrC tableName(KNullDesC);
		
		if (CommsDatSchema::IsNode(elementId))
			{
            if (CommsDatSchema::IsDeprecatedRecord(
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                                                   elementId
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                                                   ))
                {
                // Client is using a deprecated record in the 'latest' session
                __FLOG_STATIC1(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::MaybeCreateNode() this client has chosen not to use an old data schema version, but is accessing a deprecated node <%08x>"), aElement.ElementId());                                
		        ASSERT(KErrNotSupported);
                User::Leave(KErrNotSupported);
                }
			
/*            if (CommsDatSchema::IsGenericRecord(elementId) )
                {
                CMDBGenericRecord* genRec = static_cast<CMDBGenericRecord*>(&aElement);
                if (NULL != genRec->iRecordType.Ptr())
                	{
                	__FLOG_STATIC0(KLogComponent, KCDInfoLog,_L("janos: genRec.iRecordType is not NULL"));
                	tableName.Set(genRec->iRecordType);
                	}
                }*/
        
            // check if the node exists and find out the attributes the element should have
			err = LoadNodeAttributesL(elementId);
			}
		else
			{
			// First check if the node exists and find out the attributes the element should have
			err = LoadElementAttributesL(elementId);
			}

		// Don't need to do anything more if element already exists
		// or if element is a field
		if (err == KErrNotFound)
			{
			WriteAllowedL(elementId); 

			if ( CommsDatSchema::IsNewRecordRequest(elementId) )
		        {
		        // assign new record id and create node
		        SetNodeIdL(elementId, KCDUtilityFlag | KCDMaskShowType, KCDInitialRecordId, KCDMaxRecordId, KCDMaxRecords);
		        elementId &= ~KCDUtilityFlag;
//		        CreatePlaceholderL(elementId, KCDMaskShowRecordId, tableName);
		        CreatePlaceholderL(elementId, KCDMaskShowRecordId, aElement);
		        CreateNodeL(elementId, KCDMaskShowRecordId, aElement);
		        }
		    else if ( CommsDatSchema::IsNewColumnRequest(elementId) )
		        {
		        elementId &= ~KCDUtilityFlag;
		        // assign new column id and create node
				SetNodeIdL(elementId, KCDMaskShowFieldType, KCDInitialColumnId, KCDMaxColumnId, KCDMaxColumns);
//		        CreatePlaceholderL(elementId, KCDMaskShowFieldType, tableName);
		        CreatePlaceholderL(elementId, KCDMaskShowFieldType, aElement);
		        CreateNodeL(elementId, KCDMaskShowFieldType, aElement);
		        }
		    else if ( (elementId & KCDMaskShowRecordType) != 0 )
		        {
				// Is an element where the id has already been set by caller
				if ( CommsDatSchema::IsRecord(elementId) )
		            {
		            // create new record
//		            CreatePlaceholderL(elementId, KCDMaskShowRecordId, tableName);
		            CreatePlaceholderL(elementId, KCDMaskShowRecordId, aElement);
		            if ( CommsDatSchema::IsTemplate(elementId) )
		                {
		                // Template records must be hidden
		                elementId |= ECDHidden;
		                }
		            CreateNodeL(elementId, KCDMaskShowRecordId, aElement);
		            }
		        // columns shouldn't be set by user.explicitly - new column request should be used
		        // other entries would be individual fields and wouldn't get here because
				// LoadElementAttributes would have returned KErrNone.
		        }
		    else
		        {
		        // user has set an invalid id
                __FLOG_STATIC1(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::MaybeCreateNode()  Element <%08x> is not a valid element id"), aElement.ElementId());                                
		        User::Leave(KErrNotFound);
		        }
			}
		}
    //pass back new node id or set of appropriate attributes
	aElement.SetElementId(elementId);
    }	



TInt CMDBSessionImpl::MaybeModifyNodeL(TMDBElementId& aElementId)   
/*
*/
    {
    TBool retval(0);
    if (! iInModification)
        {        
        TMDBElementId storedId(aElementId & KCDMaskHideRes);
        TMDBElementId mask = ~(KCDMaskShowAttributes | KCDChangedFlag);

        TInt err = LoadElementAttributesL(storedId);

        if ( err == KErrNotFound && 
             ( ( (storedId & KCDMaskShowFieldType) != KCDMaskShowFieldType ) || 
               ( (storedId & KCDMaskShowRecordId)  != KCDMaskShowRecordId  )    ) )
            {
            err = KErrNone;
            }
        
        if ( (aElementId & KCDMaskShowAttributes)  !=  (storedId & KCDMaskShowAttributes) )
            {
            // this means the element was found but didn't have these attributes. 
            // So need to move it and all linked elements.

            TUint32 partialSourceId(storedId);
            TUint32 partialTargetId(aElementId);
            TUint32 targetAttributes = (aElementId ^ storedId) & KCDMaskShowAttributes;
     
            if ( CommsDatSchema::IsTable(aElementId) ) 
                {
                // move all records, columns and fields in this table
                mask = KCDMaskShowRecordType | targetAttributes;
                partialSourceId &= (KCDMaskShowRecordType | KCDMaskShowAttributes);
                partialTargetId &= (KCDMaskShowRecordType | KCDMaskShowAttributes);
                }
            else if ( CommsDatSchema::IsRecord(aElementId) )
                {
                // move all fields in this record
                mask = KCDMaskShowRecordType | KCDMaskShowRecordId | targetAttributes;
                partialSourceId &= (KCDMaskShowRecordType | KCDMaskShowRecordId | KCDMaskShowAttributes);
                partialTargetId &= (KCDMaskShowRecordType | KCDMaskShowRecordId | KCDMaskShowAttributes);
                }
            else if ( CommsDatSchema::IsColumn(aElementId) )
                {
                // move all fields in this column
                mask = KCDMaskShowRecordType | KCDMaskShowFieldType | targetAttributes;
                partialSourceId &= (KCDMaskShowRecordType | KCDMaskShowFieldType | KCDMaskShowAttributes);
                partialTargetId &= (KCDMaskShowRecordType | KCDMaskShowFieldType | KCDMaskShowAttributes);
                }
            else
                {
                // this element is not a node, but a field and yet was not found with expected set of attributes.
                // Can't change attributes of single fields so in error situation.
                __FLOG_STATIC1(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::MaybeModifyNodeL() for id <%08x> being asked to change attributes on single field which is an error"), partialSourceId );   
                User::Leave(KErrNotFound);
                }

            __FLOG_STATIC3(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::MaybeModifyNodeL() About to call CRepository::MoveL() to move everything at id %08x to %08x using the mask %08x."), partialSourceId, partialTargetId, mask);   
           
            TUint32 errorId(0);
            err = StorageL()->Move(partialSourceId, partialTargetId, mask, errorId);
 
            if (err != KErrNone)
                {
                __FLOG_STATIC5(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::MaybeModifyNodeL() for id %08x, moving to <%08x> with the mask <%08x>.  CRepository::MoveL() returned %d and failed at <%08x>"), partialSourceId, partialTargetId, mask, retval, errorId);   
                }
            else
                {
                iInModification = ETrue;
                retval = ETrue;
                }
            }
        }

    return retval; 
    }


TBool CompareInstance(const TUint32& aFirst, const TUint32& aSecond)
    {
    return( (aFirst & KCDMaskShowRecordId) == (aSecond & KCDMaskShowRecordId));
    }
    

TInt CMDBSessionImpl::MaybeDeleteNodeL(CMDBElement* /* aElement */, TMDBElementId& aElementId)   
    {
    TMDBElementId id(aElementId);
    
    // check element id exists
    TInt err = LoadElementAttributesL(id);
        
    if (err == KErrNone) 
        {
    	RArray<TUint32> ids;
        CleanupClosePushL(ids);
    
        // set mask
        TUint32 mask(KCDMaskHideRes);
        TUint32 nodemask(KCDMaskHideRes);

        // if is table
        if ( CommsDatSchema::IsTable(id) )    
            {
            mask = KCDMaskShowRecordType | KCDUtilityFlag;
            nodemask = mask | KCDMaskShowFieldType | iWriteAttributeMask; // will find records and table.  Not enough - need to do two finds
    	    }
        // if is record
        else if ( CommsDatSchema::IsRecord(id) )
            {
            mask = KCDMaskShowRecordType | KCDMaskShowRecordId | KCDUtilityFlag;
            nodemask = mask | KCDMaskShowFieldType | iWriteAttributeMask;
            }
        // if is column 
        else if ( CommsDatSchema::IsColumn(id) ) 
            {
            mask = KCDMaskShowRecordType | KCDMaskShowFieldType | KCDUtilityFlag;
            nodemask = mask | KCDMaskShowRecordId | iWriteAttributeMask;
            }

        // otherwise just delete single field with no mask

        __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::MaybeDeleteNode() for id <%08x>  CRepository::FindL using mask <%08x>"), aElementId, mask);

      
        if ( mask !=  KCDMaskHideRes )
            {
    		RArray<TUint32> placeholders;
            CleanupClosePushL(placeholders);
            // find all relevant placeholders
            err = StorageL()->FindL((aElementId & ~iWriteAttributeMask) | KCDUtilityFlag, mask, placeholders);
	        	           
            if (placeholders.Count() > 0)
                {
                RArray<TUint32> nodes;
                CleanupClosePushL(nodes);

                err = StorageL()->FindL(aElementId & ~iWriteAttributeMask, nodemask, nodes);
 
 				if(CommsDatSchema::IsGenericRecord(aElementId))
 					{
 					TUint32 checkid = KCDMaskShowRecordId;
 					TInt instanceid;
 					while((instanceid = nodes.Find(checkid,TIdentityRelation<TUint32>(CompareInstance))) != KErrNotFound)
 						{
 						nodes.Remove(instanceid);
 						}
 					while((instanceid = placeholders.Find(checkid,TIdentityRelation<TUint32>(CompareInstance))) != KErrNotFound)
 						{
 						placeholders.Remove(instanceid);
 						}
 					placeholders.Compress();
 					nodes.Compress(); 					
 					}
 					
 					
				if (nodes.Count() == placeholders.Count() )
                    {
                    // delete the placeholders in a batch
                    for (TInt i = 0; i < placeholders.Count(); i++)
                        {
                        err = StorageL()->Delete(placeholders[i]);
						if( err != KErrNone)
                            {
                            __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::MaybeDeleteNode() failed to delete a placeholder <%08x> with error %d"), placeholders[i], err);                                
                            User::Leave(err);
                            }
                        }
					
					if(nodes.Count())
						{
#ifndef __TOOLS2__
						TBool modemBearerDeleted = ((nodes[0] & KCDMaskShowRecordType) == KCDTIdModemBearerRecord);
                    	NotifierL()->MaybeNotifyChangeForDelete(nodes[0], modemBearerDeleted);  	
#endif
						}                         
                    }
                else
                    {
                    // place holders have no attributes, but nodes do.
                    // if cannot see same number of nodes and placeholders this indicates
                    // client may need extra capabilities to see or manipulate all nodes
                    __FLOG_STATIC1(KLogComponent, KCDErrLog, _L("CMDBSessionImpl::MaybeDeleteNode() client does not have permission to delete all elements in this node <%08x>"), aElementId);                                
                    User::Leave(KErrPermissionDenied);
                    }

                CleanupStack::PopAndDestroy(&nodes);
                }
       
            CleanupStack::PopAndDestroy(&placeholders);
            }
        
        // find all relevant items
        err = StorageL()->FindL(aElementId & ~iWriteAttributeMask, mask, ids);
        
    	if(CommsDatSchema::IsGenericRecord(aElementId))
				{
				TUint32 checkid = KCDMaskShowRecordId;
				TInt instanceid;
				while((instanceid = ids.Find(checkid,TIdentityRelation<TUint32>(CompareInstance))) != KErrNotFound)
					{
					ids.Remove(instanceid);
					}
				ids.Compress();	
				}
 					
                   
        if (ids.Count() > 0)
            {
            __FLOG_STATIC3(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::MaybeDeleteNode() for id <%08x>  CRepository::FindL()  with mask <%08x> returned %d entries"), aElementId, mask, ids.Count());
            // delete everything in a batch
            for (TInt i = 0; i < ids.Count(); i++)
                {
                err = StorageL()->Delete(ids[i]);
#ifndef __TOOLS2__
				NotifierL()->MaybeNotifyChangeForDelete(ids[i], 0);       
#endif
                }
			// reset NotNull flag
			aElementId &= ~KCDNotNullFlag;
            }
        else
            {
            __FLOG_STATIC3(KLogComponent, KCDInfoLog, _L("CMDBSessionImpl::MaybeDeleteNode() for id <%08x>  CRepository::FindL()  with mask <%08x> returned err %d"), aElementId, mask, err);
            }

        CleanupStack::PopAndDestroy(&ids);
        }
    
    return err;
    }




TInt CMDBSessionImpl::FindMatchL(RArray<TUint32>& aCandidates, RArray<TUint32>& aMatches, TUint32 aIdMask)
/*
Utility function to match a CMDBElement with a list of possible candidates by id.

@internalComponent
*/
    {   

    TInt retval = KErrNotFound;

    //ignore template record
    if ( aCandidates.Count() )
        {
        if ( (aCandidates[0] & KCDMaskShowRecordId) == KCDDefaultRecord )
            {
            aCandidates.Remove(0);
            }
        }

    if ( aCandidates.Count() && aMatches.Count())
        {
        TInt count(aMatches.Count());
        while(count--)
        	{
        	if(KErrNotFound == aCandidates.Find(aMatches[count],TIdentityRelation<TUint32>(CompareInstance)))
                {
                aMatches.Remove(count);
                }
        	}	   
		 aMatches.Compress();
        }
    else if ( aCandidates.Count())
        {
        for (TInt i = 0; i < aCandidates.Count(); i++)
            {
            retval = aMatches.Append(aCandidates[i] | aIdMask);
            
            if (retval != KErrNone)
                {
                aMatches.Close();   
                if (retval == KErrNoMemory)
                    {
                    User::Leave(KErrNoMemory);
                    }
                }
            }
        }

    if ( aMatches.Count() )
        {
        retval = KErrNone;
        }
    
    return retval;
 
    }



//EOF
