// File  MDBVisitorRecordLink.cpp
//

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
// Visitor that interacts with the central repository for CMDBRecordLink
// A record link field in a record is treated as a field of type TMDBElementId
// and is therefore handled by the standard visitor class
// This class is for explicit MMetaDatabase calls on the CMDBRecordLink element itself
// 
//

/**
 @file
 @internalComponent
*/
 

#include "CommsDatInternalDefs.h"
#include <commsdattypesv1_1.h>
#include <comms-infras/commsdatschema.h>


using namespace CommsDat;
using namespace CommsDatInternal;
using namespace Meta;



CommsDat::TMDBRecordLinkVisitor::TMDBRecordLinkVisitor(CMDBElement** aData)
 /* note that data in base class will be set on SetOwner*/
: CommsDat::TMDBVisitor<TInt>(NULL), iLinkedRecord(aData)
/**
 * Constructor
 */
    {
    }


CommsDat::TMDBRecordLinkVisitor* CommsDat::TMDBRecordLinkVisitor::NewL(const TAny* aMem, const TAny* aData)
/**
 * Instantiates a RecordSet Visitor of a particular type.
 * Used for attribute registration (in the data v-table).
 */
    {
    return static_cast<CommsDat::TMDBRecordLinkVisitor*>( ::new ((TUint8*)aMem) TMDBRecordLinkVisitor( (CMDBElement**)aData ) );
    }


TInt CommsDat::TMDBRecordLinkVisitor::Changed() const
/*
when this is not an int
 composite class always returns changed so iterator will look for changed members.
*/
    {
    if (iOwner && (iOwner->Type() != ELink ))
        {
        return TMDBVisitor<TInt>::Changed();
        }

    return 1;
    }



void CommsDat::TMDBRecordLinkVisitor::SetOwner(CMDBElement* aOwner)
/*
Set the owner of this element as is needed for callback.
Also set a pointer to the value in the owner for convenience
@internalComponent
*/  
	{ 
    TMDBVisitorBase::SetOwner(aOwner);

    // now set iTarget to have easy access to the link field value
    if (iTarget == NULL)
        {
        iTarget = reinterpret_cast<SMDBMetaElement<TInt>*>(aOwner->Data());
        }
    }


void CommsDat::TMDBRecordLinkVisitor::CheckLinkValueL(TMDBElementId aLinkedElementId)
/*
utility to check the value of this field specifies <Table><Record>.  
Nothing less and nothing more.  
Caller compliance and validation should already have got us to this point.

@internalComponent
*/     
    {
    if ( (aLinkedElementId & KCDMaskShowRecordType) == 0  ||
         (aLinkedElementId & KCDMaskShowRecordId) == 0 ||
         (aLinkedElementId & ~KCDMaskShowRecordTypeAndId) != 0 )
        {
        __FLOG_STATIC1(KLogComponent, KCDErrLog, 
            _L("TMDBRecordLinkVisitor::CheckLinkedValueL() - link Value of <%08x> does not specify a table and record id and is not valid."), aLinkedElementId);
        
        User::Leave(KErrArgument);   
        }        
    }


void CommsDat::TMDBRecordLinkVisitor::CheckLinkedRecordL(TMDBElementId aLinkedElementId)
/*
utility to check the linked record 

@internalcomponent
*/
    {
    if (iLinkedRecord != NULL  &&  *iLinkedRecord != NULL)
        {
        // just check the record is the right type
        if ( (aLinkedElementId & KCDMaskShowRecordTypeAndId) != 
             ((*iLinkedRecord)->ElementId() & KCDMaskShowRecordTypeAndId) )
            {
            __FLOG_STATIC2(KLogComponent, KCDErrLog, 
                _L("TMDBRecordLinkVisitor::CheckLinkedRecordL() - TypeId given in field value <%08x> and type of linked record <%08x> do not match. Link not usable."), (aLinkedElementId & KCDMaskShowRecordType), (*iLinkedRecord)->TypeId() & KCDMaskShowRecordType);

            User::Leave(KErrArgument);   
            }
        }
    }

void CommsDat::TMDBRecordLinkVisitor::MaintainBCForLegacyLinks()
// yes this is a hack but need something to give clients the value 
// they expect in order to maintain bc
    {
    if (iTarget->iValue > KCDMaxRecords)
        {
        if( ( (iTarget->iElementId & KCDMaskShowRecordType ) < KCDTIdAccessPointRecord    &&
              (iTarget->iElementId & KCDMaskShowRecordType ) != KCDTIdWLANServiceExtRecord  ) )//||
        //    (iTarget->iElementId & KCDMaskShowType ) == KCDTIdSelectionPolicy                 ||
        //    (iTarget->iElementId & KCDMaskShowRecordType ) == KCDTIdApPrioritySelectionPolicyRecord  )
            {
            iTarget->iValue = ((iTarget->iValue & KCDMaskShowRecordId) >> 8);
            }
        }
    }


void CommsDat::TMDBRecordLinkVisitor::CreateLinkedRecordL(CMDBSession& aSession, TMDBElementId aLinkedElementId)
/*
utility to create the linked record 

@internalcomponent
*/
    {
    if (iLinkedRecord != NULL && *iLinkedRecord == NULL )
        {

        if ((aLinkedElementId & KCDMaskShowRecordType) == 0)
            {
            aLinkedElementId = CommsDatSchema::GetLinkIdL(aSession, iTarget->iElementId, iTarget->iValue, (*iLinkedRecord)? (*iLinkedRecord)->ElementId(): 0/*, iOwner*/);
            }
        // Create a new record of the appropriate type
	    // trap is here to maintain behaviour compatibility
	    TRAPD(err, *iLinkedRecord = CCDRecordBase::RecordFactoryL(aLinkedElementId & KCDMaskShowRecordType);)
	    
        if(err != KErrNone)
	      	{
			__FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBRecordLinkVisitor::MaybeCreateLinkedRecordL() Link Record creation for id <%08x> failed with err <%d>."), aLinkedElementId, err);
           
            User::Leave(err);    
            }

        (*iLinkedRecord)->SetElementId(aLinkedElementId | KCDMaskShowFieldType);
        }
    }

TInt CommsDat::TMDBRecordLinkVisitor::GetL(CMDBSessionImpl* aSession) 
/*
Assumes fully resolved element Id but still needs mask for reserved bits

@internalComponent
*/
    {
    TInt err = TMDBVisitor<TInt>::GetL(aSession);
   
    if (err == KErrNone && iOwner->Type() != ELink)
        {
        MaintainBCForLegacyLinks();
        }
        
	return err;
    }

TInt CommsDat::TMDBRecordLinkVisitor::LoadL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags) 
/*
Assumes type and record id set by caller

@internalComponent
*/
    {
    // First load the link id which is the value in this field 
    TInt err = TMDBVisitor<TInt>::LoadL(aSession, (*iElementId & ~KCDMaskShowRecordId) | (aRecordId & KCDMaskShowRecordId), aAttributeFlags);
    
    if (iOwner->Type() == ELink &&
        err == KErrNone &&
        iLinkedRecord != NULL)
        {
        if (*iLinkedRecord != NULL)
            {
            CheckLinkedRecordL(iTarget->iValue);

            (*iLinkedRecord)->SetRecordId(iTarget->iValue);

            (*iLinkedRecord)->LoadL(aSession->iOwner);
            }
        else 
            {
            CreateLinkedRecordL(aSession->iOwner, iTarget->iValue);
            if(*iLinkedRecord != NULL)
            	{
	            TRAPD(err, (*iLinkedRecord)->LoadL(aSession->iOwner));
	            
	            if (err != KErrNone)
	                {
	                delete *iLinkedRecord;
	                *iLinkedRecord = NULL;
	                }
            	}
            }
        }

    MaintainBCForLegacyLinks();
    
    return err;
    }


TInt CommsDat::TMDBRecordLinkVisitor::LoadL(CMDBSessionImpl* aSession, TMDBElementId aElementId, TMDBAttributeFlags aAttributeFlags, RArray<TUint32>& /*aIds*/)    
/* 
// KErrNoMatch
// KErrNoAuthorisation

@internalComponent
*/
    {      
    return LoadL(aSession, aElementId, aAttributeFlags);
    }   
 


TInt CommsDat::TMDBRecordLinkVisitor::FindL(CMDBSessionImpl* aSession, RArray<TUint32>& aIds)
/*
Find fields checking whether the value matches the instantiated one if set.  The value must first be translated into
the <table><record> format so that a match can be found.
// KErrNoMatch
// KErrNoAuthorisation
 * 
@internalComponent
*/
    {   
    TInt err(KErrNone);

    if ( iOwner->Type() == ELink &&
         iLinkedRecord  != NULL  && 
         *iLinkedRecord != NULL)
        {
        // Find the linked record

        TBool found = (*iLinkedRecord)->FindL(aSession->iOwner);
    
        // set the value to match the record if it wasn't specifically changed
        if ( found )
            {
            if (!TMDBVisitor<TInt>::Changed() )
                {
                iTarget->iValue = (*iLinkedRecord)->ElementId() & KCDMaskShowRecordTypeAndId;
                Change();
                }
            else
                {
                CheckLinkedRecordL(iTarget->iValue);
                }
            }
        else
            {
            err = KErrNotFound;
            }
        }
      
    // Then find the linking field
    if ( err == KErrNone  &&  TMDBVisitor<TInt>::Changed() )
        { 
     
        // first validate the linking field - have to trap in case is not valid
        TInt temp(0);
            
 
            {
            TRAPD(trapErr, temp = CommsDatSchema::GetLinkIdL(aSession->iOwner, iTarget->iElementId, 
            								iTarget->iValue, (iLinkedRecord? (*iLinkedRecord)->ElementId(): 0), 
            								iOwner ); );
    
            if (trapErr != KErrNone)
                {
                if (trapErr == KErrArgument)
                    {
                    return KErrNotFound;
                    }
                else
                    {
                    // some other error has occurred
                    User::Leave(trapErr);
                    }
                }
            }

       // Now look for the field with the value of temp    

        TInt mask = KCDMaskShowType;

        // First look for the given value
	    err = aSession->StorageL()->FindEqL(ElementId() & ~aSession->GetReadAttributeMask(), mask, temp, aIds);

        if ( aIds.Count() <= 0 )
            {
            if (err == KErrNotFound)
                {
                // then try just looking for the value of the recordId on its own
                err = aSession->StorageL()->FindEqL(ElementId() & ~aSession->GetReadAttributeMask(), mask, (temp & KCDMaskShowInstance), aIds);
                }
#ifdef __DEBUG // because only doing this for logging
            if ( err != KErrNone  ||  aIds.Count() <= 0 )
                {
                if (err == KErrNone || err == KErrNotFound)
                    {
                    __FLOG_STATIC3(KLogComponent, KCDInfoLog, _L("TMDBRecordLinkVisitor::FindL() storage server FindEqL() did not find field <%08x>, value <%08x>"), ElementId(), iTarget->iValue, err);
                    }
                else
                    {
                    __FLOG_STATIC3(KLogComponent, KCDErrLog, _L("TMDBRecordLinkVisitor::FindL() storage server FindEqL() for field <%08x>, value <%08x>, returned unexpected err <%d>"), ElementId(), iTarget->iValue, err);
                    }
                }
#endif             
            }
        }

    return err;
    }
 
 
 
TInt CommsDat::TMDBRecordLinkVisitor::RefreshL(CMDBSessionImpl* aSession)
/*
Refresh existing data where it has not been changed
// KErrNoMatch
// KErrNoAuthorisation     

@internalComponent
*/
    {         
    if (! aSession || ! aSession->StorageL())
        User::Leave(KErrNotReady);

    TInt err(KErrGeneral); 
    
    //First refresh the field
    if ( TMDBVisitor<TInt>::Changed() )
        {
        err = TMDBVisitor<TInt>::RefreshL(aSession);
        }

    if ( iOwner->Type() == ELink &&
         err == KErrNone  &&
         iLinkedRecord != NULL  && 
         *iLinkedRecord != NULL )
        {          
        // Now refresh the linked record too

        CheckLinkedRecordL(iTarget->iValue);

        (*iLinkedRecord)->SetRecordId(iTarget->iValue);

        (*iLinkedRecord)->RefreshL(aSession->iOwner);
        }

    if (! TMDBVisitor<TInt>::Changed())
        {
        MaintainBCForLegacyLinks();
        }

    return err;
    }


void CommsDat::TMDBRecordLinkVisitor::SetLinkValueL(CMDBSession& aSession)
// always store the fully qualified link id (table and Record)
    {
 //   if ( (iTarget->iValue & KLinkableTag) != NULL)
       // { // change the link value to table/record
        iTarget->iValue = CommsDatSchema::GetLinkIdL(aSession, iTarget->iElementId, iTarget->iValue, 0/*, iOwner*/);
   //     }
   // else
//        { // just validate the link
    //    TInt temp = CommsDatSchema::GetLinkIdL(aSession, iTarget->iElementId, iTarget->iValue, NULL, iOwner);
     //   }
    }


TInt CommsDat::TMDBRecordLinkVisitor::StoreL(CMDBSessionImpl* aSession, TMDBElementId aRecordId, TMDBAttributeFlags aAttributeFlags)
/*
Create a new linked field.
If the linked record is also attached then create it as well if necessary.
if the linked record is present it is assumed the client has changed it appropriately
// KErrNoAuthorisation
// KErrAlreadyExists
@internalComponent
*/
    {         
    if (! iTarget || ! iElementId || ! aSession->StorageL())
        {
        User::Leave(KErrNotReady);
        }

    if ( iTarget->iValue == 0)
        {
        return KErrNone; // pointless to store an empty link  
        }

    // First find the tableid of the linked field...

    // Translate this link field into a storable element id <Table><Record> if enough information is present
    SetLinkValueL(aSession->iOwner);

 //   __FLOG_STATIC2(KLogComponent, KCDInfoLog, _L("TMDBRecordLinkVisitor StoreL() storing link value <%08x> in field <%08x>"), iTarget->iValue,  ElementId());
  //  TUInt32 storeVal = iTarget->iValue > KCDMaxRecords ? (iTarget->iValue >> 8 ): iTarget->iValue)
        
    *iElementId = (*iElementId & ~KCDMaskShowRecordId) | (aRecordId & KCDMaskShowRecordId) | aAttributeFlags;            
    TInt err = aSession->StorageL()->Create( ElementId() & KCDMaskHideRes, iTarget->iValue);
   
    if (err != KErrNone)
        {
        __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("TMDBRecordLinkVisitor StoreL() storage server StoreL failed with err <%d> when storing field <%08x>"), err,  ElementId());
        }
    else if ( iOwner->Type() == ELink  &&
              iLinkedRecord != NULL  &&  *iLinkedRecord != NULL )
        {
        // Now store the linked record too

        // Check that the link value is consistent with the linked record
        CheckLinkedRecordL(iTarget->iValue);
        
        (*iLinkedRecord)->SetRecordId(iTarget->iValue);

        // ensure the record id is set and then call StoreL on the linked record
        (*iLinkedRecord)->StoreL(aSession->iOwner); 
        }

    MaintainBCForLegacyLinks();
    
	return err;
    }
    
 
TInt CommsDat::TMDBRecordLinkVisitor::ModifyL(CMDBSessionImpl* aSession, TMDBAttributeFlags aAttributeFlags)
/*
Modify field in the store
Assumes that client has correctly changed the linkedRecord if changed the link value
// KErrNoAuthorisation
// KErrNoMatch
@internalComponent
*/
    {         
    if (! iElementId || ! aSession->StorageL())
        {
        User::Leave(KErrNotReady);
        }
    
    TInt err = KErrNone;
    
    // First modify the link field which is the value in this field
    if (0 == iTarget->iValue)
    	{
    	/* The value of a linked field is modified to 0. This means that instead of storing
    	 * the 0 value the node (field) should be deleted from the DB.
    	 */
    	err = aSession->MaybeDeleteNodeL(NULL, iTarget->iElementId);
    	return err;
    	}
    else
    // Translate this link field into a storable element id <Table><Record> if enough information is present
        {
        SetLinkValueL(aSession->iOwner);
        }
        
    err = TMDBVisitor<TInt>::ModifyL(aSession, aAttributeFlags);
    
    if (iOwner->Type() == ELink &&
        err == KErrNone &&
        iLinkedRecord != NULL  &&  *iLinkedRecord != NULL  )
        {    
        // Now modify the linked record too.

        // it's the caller's responsibility to ensure the linkedRecord type matches the changed value
        CheckLinkedRecordL(iTarget->iValue);
        
        (*iLinkedRecord)->SetRecordId(iTarget->iValue);

        (*iLinkedRecord)->ModifyL(aSession->iOwner);
        }
        
        MaintainBCForLegacyLinks();
    
    	return err;
    }

 
//EOF
