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
// Implementation of MetaDatabase interface functions for CommsDat
// 
//

/**
 @file 
 @publishedAll
 @released
*/


#include <commsdattypesv1_1.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif

#include "CommsDatInternalDefs.h"
#include "CommsDatValidator.h"
#include "CommsDatMapper.h"
#include "MetaDatabaseVisitor.h"
#include <commsdatutils.h>
#include <comms-infras/commsdatschema.h>


using namespace CommsDat;
using namespace CommsDatInternal;
using namespace Meta;

/*  
MMetaDatabase 

Implementation of main high-level algorithm for each MMetaDatabase call.
Detailed activity left to visitors for each field that needs to be handled.
*/


EXPORT_C MMetaDatabase::MMetaDatabase()
/**
 * Protected and explicit constructor - nothing to construct
 */
    {
    }



EXPORT_C MMetaDatabase::~MMetaDatabase()
/**
 * Virtual destructor - nothing to destroy
 */
    {
    }


TBool CompareColumn(const TUint32& aFirst, const TUint32& aSecond)
// Utility for RArray Find
    {
    TBool ret =  ((aFirst & KCDMaskShowType) == (aSecond & KCDMaskShowType));
    return ret;
    }

//




EXPORT_C void MMetaDatabase::LoadL(CMDBSession& aSession)
/**
 * Loads each member of the MMetaDatabase object from the database.
 * Will (delete and) overwrite any data already in the container.
 **/
    {   
    CMDBSessionImpl* session = aSession.iMDBSessionImpl;

    // no need for a transaction for read, but do want the write lock.
   // TBool ownTransaction = session->MaybeOpenTransactionL();
    
    TAny* mem[KMMetaDBVisitorMaxSize];
    TMetaVTableIterator attribIter(this);
    SVDataTableEntry const* entry = NULL;
    TMDBVisitorBase* metaDBVisitor = NULL;
            
    CMDBElement* thisElement = static_cast<CMDBElement*>(this);
    
    __FLOG_STATIC1(KLogComponent, KCDInfoLog, _L("MMetaDatabase::LoadL() called for Element %08x..."), thisElement->ElementId());

    TInt err(KErrNotFound);
    
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CCDMapper* mapper = CCDMapper::MaybeCreateMapperLC(*thisElement,*aSession.iMDBSessionImpl,ELoad);
#else
    CCDMapper* mapper = NULL;
#endif
    
       
    TUint32& elementId = *thisElement->Data();
        
    if ((elementId & KCDMaskShowRecordType) != 0) 
        {
        // Table id has been set
        err = session->LoadElementAttributesL(elementId);
        }        
    
    if (err == KErrNotFound)
        {
        if (mapper ||                                     //we have a mapper object  
            ! CommsDatSchema::IsNode(elementId)        || // Could be a generic record trying to load by name, so let it carry on.
            CommsDatSchema::IsGenericRecord(elementId)   )// let it find the template record
            {
	        err = KErrNone;	
	        }
        }
    
    if (err == KErrNone)
        {
        if (!mapper &&
            (CommsDatSchema::IsRecord(elementId) ) && 
            !CommsDatSchema::IsTemplate(elementId) 
            && !CommsDatSchema::IsGenericRecord(elementId))
            {
        
            // This section gets template field where appropriate and ensures minimal call to find in database
        
            RArray<TUint32> recordIds;
            CleanupClosePushL(recordIds);
            RArray<TUint32> templateIds;
            CleanupClosePushL(templateIds);

            // Find all entries in the record
            TUint32 mask = KCDMaskShowRecordType | KCDMaskShowRecordId | session->GetReadAttributeMask() | KCDUtilityFlag;
            TUint32 id = elementId & KCDMaskHideAttrAndRes;

            TInt err = session->StorageL()->FindL(id, mask, recordIds);  

            if (recordIds.Count())
                {
                // Find all entries for the template
                mask = (KCDMaskShowRecordType | KCDMaskShowRecordId | session->GetReadAttributeMask() | KCDUtilityFlag) & ~ECDHidden;
            
                session->StorageL()->FindL(id & ~KCDMaskShowRecordId, mask, templateIds); // there might not be a template don't need to check err
        
                TInt loc(0);
                TInt fieldId(0);
        
                while ( (err == KErrNone || err == KErrNotFound)  &&  (entry = attribIter++) != NULL )
                    {
                    metaDBVisitor = static_cast<TMDBVisitorBase*>(entry->iMetaNewL(mem,GetAttribPtr(entry->iOffset))); // target
                    if (metaDBVisitor) // might be deprecated
                    	{
                        metaDBVisitor->SetOwner(thisElement);
                        // can optimise further by not doing array finds in this way
                        loc = recordIds.Find(metaDBVisitor->ElementId(), CompareColumn);
                        if (loc != KErrNotFound)
                            {
                            metaDBVisitor->SetElementId(recordIds[loc]);
                            // know the value is there so just get it
                            err = metaDBVisitor->GetL(session);
                
                            if (err != KErrNone)
                                {
                                // oh dear, should have found this.  Something's gone horribly wrong
                                __FLOG_STATIC1(KLogComponent, KCDErrLog, _L("MMetaDatabase::LoadL() VisitorGetL failed unexpectedly for Element %08x with err %d"), metaDBVisitor->ElementId());
                                User::Leave(err);
                                }
                
                            recordIds.Remove(loc);
                            }
                        else if (templateIds.Count())
                            {
                            loc = templateIds.Find(metaDBVisitor->ElementId(), CompareColumn);
                            if (loc != KErrNotFound)
                                {
                                // get the template value instead
                    
                                fieldId = metaDBVisitor->ElementId();
                                metaDBVisitor->SetElementId(templateIds[loc]);
                                err =metaDBVisitor->GetL(session);
                                if (err != KErrNone)
                                    {
                                    // Should have found this too  Something else has gone horribly wrong 
                                    __FLOG_STATIC1(KLogComponent, KCDErrLog, _L("MMetaDatabase::LoadL() VisitorGetL failed unexpectedly for Element %08x with err %d"), metaDBVisitor->ElementId());
                                    metaDBVisitor->SetElementId(fieldId);
                                    User::Leave(err);
                                    }
               
                                metaDBVisitor->SetElementId(fieldId);
                                metaDBVisitor->FieldLoaded();
                                
                                templateIds.Remove(loc);
                                }
                            }

                        if (loc == KErrNotFound)
                            {
                            metaDBVisitor->ClearValue();
                            }
                        }
                    }
                }
            CleanupStack::PopAndDestroy(&templateIds);
            CleanupStack::PopAndDestroy(&recordIds);
            }
        else
            {
            DoLoadL(session, thisElement, mapper, err, thisElement->ElementId() & KCDMaskShowRecordId, thisElement->Attributes());

            /* This is a work around solution for the following case:
             * There is a table with legacy links. [IAP table, BearerType and Bearer fields in it]. With the
             * new CommsDat the Bearer field value is the fully resolved TMDBElementId of the linked record.
             * BUT there is a lot of code which expects this value to be only a record number (as it is 
             * originally in the config file) after the reading it.
             * Even worse there is code which tries to read this field via a CMDBField<TInt> while this field is
             * from now on a CMDBField<someLink>. So if somebody tries to read an integer field we have to check
             * whether is it a legacy link or not. If it is, then we have to do conversion on the value to contain
             * only the record number of the linked record and not the TMDBElementId.
             */
            if (CommsDatSchema::IsColumn(thisElement->ElementId() | KCDMaskShowRecordId) && 
             	(thisElement->Type() ==EInt || thisElement->Type()==EUint32))
             	{
             	CommsDatSchema::ChangeLegacyLinkFieldValues(thisElement);
             	}
            }
        
        /***********************************************************/
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        // Now load any mapped elements too 
        if (!mapper)
            {
            mapper = CCDMapper::MaybeCreateMapperLC(*thisElement,*aSession.iMDBSessionImpl,ELoad);
            }

	    if ( mapper )
            {   
            DoLoadL(session, mapper, NULL, err, thisElement->ElementId() & KCDMaskShowRecordId, thisElement->Attributes(), ETrue);
            
            mapper->MapResultsL();
            
            /* This is a work around solution for the following case:
             * There is a table with legacy links. [IAP table, BearerType and Bearer fields in it]. With the
             * new CommsDat the Bearer field value is the fully resolved TMDBElementId of the linked record.
             * BUT there is a lot of code which expects this value to be only a record number (as it is 
             * originally in the config file) after the reading it.
             * Even worse there is code which tries to read this field via a CMDBField<TInt> while this field is
             * from now on a CMDBField<someLink>. So if somebody tries to read an integer field we have to check
             * whether is it a legacy link or not. If it is, then we have to do conversion on the value to contain
             * only the record number of the linked record and not the TMDBElementId.
             */
            if (CommsDatSchema::IsColumn(thisElement->ElementId() | KCDMaskShowRecordId) && 
             	(thisElement->Type() ==EInt || thisElement->Type()==EUint32))
             	{
             	CommsDatSchema::ChangeLegacyLinkFieldValues(thisElement);
             	}
        
            CleanupStack::PopAndDestroy(mapper);                
            }
#endif
        }
        else
        {
        // Nothing at all found  or some other error
        __FLOG_STATIC1(KLogComponent, KCDErrLog, _L("MMetaDatabase::LoadL() failed for Element %08x as this node was not found in the database."), thisElement->ElementId() );
        User::Leave(err);
        }
    }    
    

/*
 This DoLoadL is called from the LoadL method if the client tries to load a field and not a record.
*/
void MMetaDatabase::DoLoadL(CMDBSessionImpl* aSession, 
                            CMDBElement* aElement, 
                            CMDBElement* aMapper, 
                            TInt &aErr, 
                            TMDBElementId aRecordId,
                            TMDBElementId aAttributes,
                            TBool isTheLoadForMapper)
    {
    TAny* mem[KMMetaDBVisitorMaxSize];
    TMetaVTableIterator attribIter(aElement);
    SVDataTableEntry const* entry = 0;
    TMDBVisitorBase* metaDBVisitor = 0;
 
    CCDMapper* mapper = static_cast<CCDMapper*>(aMapper);

    TBool loadTemplateField = !CommsDatSchema::IsTable(aElement->ElementId());


    // Apply Load function to each unmapped entry
    while ( (aErr == KErrNone  ||  aErr == KErrNotFound) &&  (entry = attribIter++) != NULL)
        {
        metaDBVisitor = static_cast<TMDBVisitorBase*>(entry->iMetaNewL(mem,aElement->GetAttribPtr(entry->iOffset))); // target
        if (metaDBVisitor) // might be deprecated
            {
            metaDBVisitor->SetOwner(aElement);
            if ( ! mapper  || ! mapper->Mapped(metaDBVisitor->ElementId()) )
                {
                aErr = metaDBVisitor->LoadL(aSession, aRecordId, aAttributes);

                if ( aErr == KErrNotFound && loadTemplateField )
                    {
                    if (!aSession->IsSetAttributeMask(ECDHidden))  
                        {
                        aSession->SetAttributeMask(ECDHidden);
                        aErr = metaDBVisitor->LoadL(aSession, KCDDefaultRecord, ECDHidden);
                        aSession->ClearAttributeMask(ECDHidden); //need to cleanup if LoadL leaves
                        }
                    else
                        {
                        aErr = metaDBVisitor->LoadL(aSession, KCDDefaultRecord, ECDHidden);
                        }
                    
                    if (KCDInitialRecordId <= aRecordId && 
                		KErrNone == aErr)
                		{
                		/* This means that the given field is read from the template record and the recordId of the
                		 * record in question (on which the FindL is called) is not a template record as the aRecordId
                		 * is not 0. This means that all of the flags should be removed from the elementId of the
                		 * metaDBVisitor (hidden, modified) and the value should be given back to the client. In this 
                		 * case the user gets the value from the template record, the TMDBElementId of the given field 
                		 * is set so as the value would have been read from the record itself. When StoreL will be called
                		 * on this record and this field is not changed it won't be stored once again in this record but
                		 * remains only in the template record. Of course if the user changes the value of this field it
                		 * will be stored into the given reocrd as the changed flag will be set.
                		 */
                		metaDBVisitor->ResetElementId(aRecordId);
                		}
                    //We don't do this branch for mappers because it can just mix up the logic
                    //implemented in the PostMapping functions...
                    else if (KErrNone != aErr &&
                    		!isTheLoadForMapper)
                    	{
                    	/* The reading of the field is still erronous, and this means at this stage that it
                    	 * simply doesn't exist in the given record. What is important in this case is that
                    	 * the value of the given field should be set to NULL, because if it's a container
                    	 * re-use case, meaning that the container is re-used for reading from a record set
                    	 * for example, this is the way we can indicate that the given field is not existing
                    	 * in the record, nor in the template record...
                    	 * This is the 9.5 propagation of the INC123203 
                    	 */
                    	metaDBVisitor->ResetElementId(aRecordId);
                    	metaDBVisitor->ClearValue();
                    	}
                    }
                }
            }
        }
        
    if (aErr == KErrNotFound)
        {
        if ( aElement->ElementId() &&
             !CommsDatSchema::IsNode(aElement->ElementId()) )
         //When the client tried to load a field explicitly and the given one cant be found we leave.
            {
            User::Leave(aErr);
            }
        else
            { 
            aErr = KErrNone;
            }
        }
    }
    
/*
 This DoLoadL is called from the FindL method.
*/
void MMetaDatabase::DoLoadL(CMDBSessionImpl* aSession, 
                            CMDBElement* aElement, 
                            CMDBElement* aMapper,
                            TInt &aErr, 
                            TMDBElementId aRecordId,
                            TMDBElementId aAttributes,
                            RArray<TUint32>& aMatches,
                            TBool isTheLoadForMapper)
    {
    TAny* mem[KMMetaDBVisitorMaxSize];
    TMetaVTableIterator attribIter(aElement);
    SVDataTableEntry const* entry = 0;
    TMDBVisitorBase* metaDBVisitor = 0;
 
    CCDMapper* mapper = static_cast<CCDMapper*>(aMapper);

    TBool loadTemplateField = !CommsDatSchema::IsTable(aElement->ElementId());


    // Apply Load function to each unmapped entry
    while ( (aErr == KErrNone  ||  aErr == KErrNotFound) &&  (entry = attribIter++) != NULL)
        {
        metaDBVisitor = static_cast<TMDBVisitorBase*>(entry->iMetaNewL(mem,aElement->GetAttribPtr(entry->iOffset))); // target
        if (metaDBVisitor) // might be deprecated
            {
            metaDBVisitor->SetOwner(aElement);
            if ( ! mapper  || ! mapper->Mapped(metaDBVisitor->ElementId()) )
                {
                aErr = metaDBVisitor->LoadL(aSession, aRecordId, aAttributes, aMatches);

                if ( aErr == KErrNotFound && loadTemplateField )
                    {
                    if (!aSession->IsSetAttributeMask(ECDHidden))  
                        {
                        aSession->SetAttributeMask(ECDHidden);
                        aErr = metaDBVisitor->LoadL(aSession, KCDDefaultRecord, ECDHidden);
                        aSession->ClearAttributeMask(ECDHidden); //need to cleanup if LoadL leaves
                        }
                    else
                        {
                        aErr = metaDBVisitor->LoadL(aSession, KCDDefaultRecord, ECDHidden);
                        }
                    
                    if (KCDInitialRecordId <= aRecordId &&
	                    KErrNone == aErr)
	                    {
	                    /* This means that the given field is read from the template record and the recordId of the
	                     * record in question (on which the FindL is called) is not a template record as the aRecordId
	                     * is not 0. This means that all of the flags should be removed from the elementId of the
	                     * metaDBVisitor (hidden, modified) and the value should be given back to the client. In this 
	                     * case the user gets the value from the template record, the TMDBElementId of the given field 
	                     * is set so as the value would have been read from the record itself. When StoreL will be called
	                     * on this record and this field is not changed it won't be stored once again in this record but
	                     * remains only in the template record. Of course if the user changes the value of this field it
	                     * will be stored into the given reocrd as the changed flag will be set.
	                     */
	                    metaDBVisitor->ResetElementId(aRecordId);
	                    }
                    //We don't do this branch for mappers because it can just mix up the logic
                    //implemented in the PostMapping functions...
                    else if (KErrNone != aErr &&
                    		!isTheLoadForMapper)
                    	{
                    	/* The reading of the field is still erronous, and this means at this stage that it
                    	 * simply doesn't exist in the given record. What is important in this case is that
                    	 * the value of the given field should be set to NULL, because if it's a container
                    	 * re-use case, meaning that the container is re-used for reading from a record set
                    	 * for example, this is the way we can indicate that the given field is not existing
                    	 * in the record, nor in the template record...
                    	 * This is the 9.5 propagation of the INC123203 
                    	 */
                    	metaDBVisitor->ResetElementId(aRecordId);
                    	metaDBVisitor->ClearValue();
                    	}
                    }
                }
            }
        }
        
    if (aErr == KErrNotFound)
        {
        aErr = KErrNone;
        }
    }



EXPORT_C TBool MMetaDatabase::FindL(CMDBSession& aSession)
/*
Loads the MMetaDatabase object from the database where match is found for fields primed by the client 
    
NB this is a depth-first search that assumes that caller will only instantiate links to records that contain 
primed fields that need to be matched.  

This is not attempting to be a generic optimised search solution, but assumes that clients will only search 
down short trees.

Searching time can be significantly reduced by appropriate choice of data container by client.
*/
{
    CMDBSessionImpl* session = aSession.iMDBSessionImpl; 
                            
    RArray<TUint32> candidates;
    RArray<TUint32> matches;
    
    CleanupClosePushL(candidates);	
    CleanupClosePushL(matches);	

    TBool retval = EFalse;
  	TInt err = KErrNone;

    CMDBElement* thisElement = static_cast<CMDBElement*>(this);
    
    __FLOG_STATIC1(KLogComponent, KCDInfoLog, _L("MMetaDatabase::FindL() called for Element %08x..."), thisElement->ElementId());

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CCDMapper* mapper = CCDMapper::MaybeCreateMapperLC(*thisElement,*aSession.iMDBSessionImpl,EFind);    

    if (mapper)
        {   
        //  LOG( ECommsDatLog::Printf(_L8("MMetaDBVisitor %08x:\tFindL() Mapping"), this););
                    
         // Find primed fields that don't need mapping
        DoFindL(session, thisElement, mapper, err, candidates, matches);
        
        if (err == KErrNone)
            {
            //do some pre-mappings
            mapper->PreMappingsL();
            
            // Then find any mapped primed fields
            DoFindL(session, mapper, NULL, err, candidates, matches);
        
            if (err == KErrNone  &&  matches.Count() > 0 )
                {           		
                DoLoadL(session, thisElement, mapper, err, matches[0] & KCDMaskShowRecordId, matches[0] & session->GetReadAttributeMask(), matches);
        
                if (err == KErrNone || err == KErrNotFound)
			        {
                    // and then Load each mapped entry
                    DoLoadL(session, mapper, NULL, err, matches[0] & KCDMaskShowRecordId, matches[0] & session->GetReadAttributeMask(), matches, ETrue);
            
                    if (err == KErrNone)
                        {
                        retval = ETrue;
                     
                        if ( matches.Count() > 0 && thisElement->RecordId() == 0 )
                            {
                            thisElement->SetElementId(thisElement->ElementId() | (matches[0] & (KCDMaskShowRecordId | KCDMaskShowAttributes)));
                            }
                        }
                    }
                }
            }
        
        mapper->MapResultsL();

        CleanupStack::PopAndDestroy(mapper);
        }   
    else
#endif
        {
        // No mapper...

		// find all possible matches
		DoFindL(session, thisElement, NULL, err, candidates, matches);
        
		__FLOG_STATIC1(KLogComponent, KCDInfoLog, _L("MMetaDatabase::FindL() - the 'err' after the DoFindL: %d"), err);
		
        if (err == KErrNone  &&  matches.Count() > 0 )
            {            
            //Case: FindL for TableIDs, when primed with a TableName
       		if(!thisElement->TableId() && 
                matches.Count()==1)
       			{
       			thisElement->SetElementId((thisElement->ElementId()) | (matches[0] & KCDMaskShowRecordType));
       			}           			
           			
            // and load the elements
		    DoLoadL(session, thisElement, NULL, err, matches[0] & KCDMaskShowRecordId, matches[0] & session->GetReadAttributeMask(), matches);
            
            /* This is a work around solution for the following case:
             * There is a table with legacy links. [IAP table, BearerType and Bearer fields in it]. With the
             * new CommsDat the Bearer field value is the fully resolved TMDBElementId of the linked record.
             * BUT there is a lot of code which expects this value to be only a record number (as it is 
             * originally in the config file) after the reading it.
             * Even worse there is code which tries to read this field via a CMDBField<TInt> while this field is
             * from now on a CMDBField<someLink>. So if somebody tries to read an integer field we have to check
             * whether is it a legacy link or not. If it is, then we have to do conversion on the value to contain
             * only the record number of the linked record and not the TMDBElementId.
             */
             if (CommsDatSchema::IsColumn(thisElement->ElementId() | KCDMaskShowRecordId) && 
             	(thisElement->Type() ==EInt || thisElement->Type()==EUint32))
             	{
             	CommsDatSchema::ChangeLegacyLinkFieldValues(thisElement);
             	}
		    
            if (err == KErrNone)
                {
                retval = ETrue;
            
                if ( matches.Count() > 0 && thisElement->RecordId() == 0 )
                    {
                    thisElement->SetElementId(thisElement->ElementId() | (matches[0] & (KCDMaskShowRecordId | KCDMaskShowAttributes)));
                    }
                }
 	        }    
	    }
    
    CleanupStack::PopAndDestroy(&matches); 
	CleanupStack::PopAndDestroy(&candidates);
   
    return retval;
    }



void MMetaDatabase::DoFindL(CMDBSessionImpl* aSession, CMDBElement* aElement, CMDBElement* aMapper, TInt& aErr, RArray<TUint32>& aCandidates, RArray<TUint32>& aMatches)
    {
    TAny* mem[KMMetaDBVisitorMaxSize];
    TMDBVisitorBase*  metaDBVisitor = 0;
    SVDataTableEntry const* entry = 0;    
    TMetaVTableIterator attribIter(aElement); 

    CCDMapper* mapper = static_cast<CCDMapper*>(aMapper);

    while ( aErr == KErrNone  &&  (entry = attribIter++) != NULL ) 
        {
        metaDBVisitor = static_cast<TMDBVisitorBase*>(entry->iMetaNewL(mem,aElement->GetAttribPtr(entry->iOffset))); // target
        if (metaDBVisitor) // might be deprecated
            {
            metaDBVisitor->SetOwner(aElement);
            if ( metaDBVisitor->Changed()  &&  
                ( !mapper || ! mapper->Mapped(metaDBVisitor->ElementId()) ) )
                {  
                aErr = metaDBVisitor->FindL(aSession, aCandidates);

                if (aErr == KErrNone )
                    {
                    aErr = aSession->FindMatchL(aCandidates, aMatches);
                    }
                }
            }
        }
    }

    

EXPORT_C void MMetaDatabase::RefreshL(CMDBSession& aSession)
/**
 * Updates each unchanged member of the MMetaDatabase object from the database
 */
    {
    CMDBSessionImpl* session = aSession.iMDBSessionImpl;
     
    CMDBElement* thisElement = static_cast<CMDBElement*>(this);

    __FLOG_STATIC1(KLogComponent, KCDInfoLog, _L("MMetaDatabase::RefreshL() called for Element %08x..."), thisElement->ElementId());

    TInt err = KErrNotFound;
        
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CCDMapper* mapper = CCDMapper::MaybeCreateMapperLC(*thisElement,*aSession.iMDBSessionImpl,ERefresh);     	
#else
    CCDMapper* mapper = NULL;
#endif
    
    err = session->LoadElementAttributesL(*thisElement->Data());

    if (err == KErrNone)
        {
        if (mapper) 
        {   
            DoRefreshL(session, thisElement, mapper, err );
            
		    //if (err == KErrNone )
		    if (mapper)
		        {
		        DoRefreshL(session, mapper, NULL, err);
            
                if (err == KErrNone)
                    {
                    mapper->MapResultsL();
                    }
                }
            CleanupStack::PopAndDestroy(mapper);
            }
        else
            {
            DoRefreshL(session, thisElement, mapper, err);
            }
        }
    }


void MMetaDatabase::DoRefreshL(CMDBSessionImpl* aSession, CMDBElement* aElement, CMDBElement* aMapper, TInt &aErr)
    {
    CCDMapper* mapper = static_cast<CCDMapper*>(aMapper);

    TAny* mem[KMMetaDBVisitorMaxSize];
    TMetaVTableIterator attribIter(aElement);
    SVDataTableEntry const* entry = 0;
    TMDBVisitorBase* metaDBVisitor = 0;

    while ( (aErr == KErrNone  ||  aErr == KErrNotFound)  &&  (entry = attribIter++) != NULL)
        {
        metaDBVisitor = static_cast<TMDBVisitorBase*>(entry->iMetaNewL(mem,aElement->GetAttribPtr(entry->iOffset)));
        if (metaDBVisitor) // might be deprecated
            {
            metaDBVisitor->SetOwner(aElement);
            if ( (! mapper || ! mapper->Mapped(metaDBVisitor->ElementId()) ) &&
                 ! metaDBVisitor->Changed() || 
                 (metaDBVisitor->ElementId() & KCDMaskShowFieldType) == KCDMaskShowFieldType )   
                {
                aErr = metaDBVisitor->RefreshL(aSession);
                }
            }
        }

    if (aErr == KErrNotFound)
        {
        aErr = KErrNone; 
        }
    }


EXPORT_C void MMetaDatabase::StoreL(CMDBSession& aSession)
/*
	Store all initialised fields in the database - this will create new entries in the database
		Client can request to ignore access control flags by setting the attribute mask,
		but only fields that the client has capabilities to set will be created

		A request to store data in an unauthorised area will cause the function to leave.
		A request to create data in an already populated field will cause the function to leave

		Store will be atomic - all fields stored after success.  No fields stored if the function leaves

	Will return
		KErrNone on success
        KErrNotFound if data cannot be found
		KErrPermissionDenied if client not authorised to read or write the data

	@publishedAll
	*/
    {
	TInt err = KErrNone;

    CMDBSessionImpl* session = aSession.iMDBSessionImpl;

    CMDBElement* thisElement = static_cast<CMDBElement*>(this);
 
    __FLOG_STATIC1(KLogComponent, KCDInfoLog, _L("MMetaDatabase::StoreL() called for Element %08x..."), thisElement->ElementId());

    TBool ownTransaction = session->MaybeOpenTransactionL();    
    
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CCDMapper* mapper = CCDMapper::MaybeCreateMapperLC(*thisElement,*aSession.iMDBSessionImpl,EStore);    
 
    if ( mapper )
        {   
        mapper->PrepareToStoreL();
        
        CCDValidator::ValidateL(thisElement,*aSession.iMDBSessionImpl,EStore);
        
        mapper->PreMappingsL();
        
        DoStoreL(session, thisElement, mapper, err);
		  
     	if ( err == KErrNone)
		    {
            // Now Store mapped entries all together	
            DoStoreL(session, mapper, NULL, err);
            
            if ( err == KErrNone)
                {
                mapper->PostMappingsL();
                }
		    }
        
        CleanupStack::PopAndDestroy(mapper);
        }
    else 
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        {   
        session->MaybeCreateNodeL(*thisElement);
        
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        CCDValidator::ValidateL(thisElement,*aSession.iMDBSessionImpl,EStore);
#else
        CCDValidator::ValidateL(thisElement,*aSession.iMDBSessionImpl);
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        
        DoStoreL(session, thisElement, NULL, err);
        }
   
  	session->MaybeCommitTransactionL(ownTransaction, thisElement, err);
    }


void MMetaDatabase::DoStoreL(CMDBSessionImpl* aSession, CMDBElement* aElement, CMDBElement* aMapper, TInt &aErr)
    {
    TAny* mem[KMMetaDBVisitorMaxSize];
    TMetaVTableIterator attribIter(aElement);
    SVDataTableEntry const* entry = 0;
    TMDBVisitorBase* metaDBVisitor = 0;
 
    CCDMapper* mapper = static_cast<CCDMapper*>(aMapper);

    while ( aErr == KErrNone  &&  (entry = attribIter++) != NULL )
        {
        metaDBVisitor = static_cast<TMDBVisitorBase*>(entry->iMetaNewL(mem,aElement->GetAttribPtr(entry->iOffset))); 
        if (metaDBVisitor) // might be deprecated
            {
            metaDBVisitor->SetOwner(aElement);
            if ( metaDBVisitor->Changed() &&  
                 ( ! mapper  || !mapper->Mapped(metaDBVisitor->ElementId())) )
                {
                aErr = metaDBVisitor->StoreL(aSession, (aElement->ElementId() & KCDMaskShowRecordId), aElement->Attributes());                         
                if (aErr == KErrNone)
                    {
                    metaDBVisitor->Sync();
#ifndef __TOOLS2__
                    aSession->NotifierL()->MaybeNotifyChange(metaDBVisitor->ElementId());       
#endif
                    }
                else
                    {
                    __FLOG_STATIC2(KLogComponent, KCDErrLog, _L("MMetaDatabase::DoStoreL() StoreL failed with err <%d> for Element <%08x>"), aErr, metaDBVisitor->ElementId());

                    }
                }
            }
        }
    }


EXPORT_C void MMetaDatabase::ModifyL(CMDBSession& aSession)
/**
 * Modifies each changed member of the MMetaDatabase object in the database
 */
    {   
    CMDBSessionImpl* session = aSession.iMDBSessionImpl;
    
    TBool ownTransaction = session->MaybeOpenTransactionL();

    CMDBElement* thisElement = static_cast<CMDBElement*>(this);

    __FLOG_STATIC1(KLogComponent, KCDInfoLog, _L("MMetaDatabase::ModifyL() called for Element %08x..."), thisElement->ElementId());

	// ensure do not enter wrong type of data against this element
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CCDValidator::ValidateL(thisElement,*aSession.iMDBSessionImpl,EModify);
#else
    CCDValidator::ValidateL(thisElement,*aSession.iMDBSessionImpl);
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    
    TInt err = KErrNone;
    TBool modifiedNode(0);

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    CCDMapper* mapper = CCDMapper::MaybeCreateMapperLC(*thisElement,*aSession.iMDBSessionImpl,EModify);    
 
    if ( mapper )
        { 	    
        mapper->PrepareToStoreL();

        DoModifyL(session, thisElement, mapper, err);
	
        if (err == KErrNone )
		    {
		    //before doing the modifications on the mapped object do some pre mappings...
		    
		    mapper->PreMappingsL();
		    
	        // Now Modify mapped entries all together
			DoModifyL(session, mapper, NULL, err);
			
			if ( err == KErrNone)
                {
                mapper->PostMappingsL();
                }
            }
       CleanupStack::PopAndDestroy(mapper);
        }
    else
#endif
        {   
        // No mapping.  just get on with it.

        // First, move the elements if there are any attribute changes 
        // this is only performed once - at the highest level in the case of a recursive walk through a record set
        modifiedNode = session->MaybeModifyNodeL(*thisElement->Data());
        
        DoModifyL(session, thisElement, NULL, err);         	;
	
/*
        if (err == KErrNone )
            {
            // make any general attribute changes for every item in the node
            err = session->MaybeModifyNodeL(*thisElement->Data());
            }
*/	    }

    if (modifiedNode)
        {
        session->EndModification();
        }
     
    session->MaybeCommitTransactionL(ownTransaction, thisElement, err);
    }


void MMetaDatabase::DoModifyL(CMDBSessionImpl* aSession, CMDBElement* aElement, CMDBElement* aMapper, TInt &aErr)
    {

    CCDMapper* mapper = static_cast<CCDMapper*>(aMapper); 

    TAny* mem[KMMetaDBVisitorMaxSize];
    TMetaVTableIterator attribIter(aElement);
    SVDataTableEntry const* entry = 0;
    TMDBVisitorBase* metaDBVisitor = 0;
    
    // apply the Modify function to each changed entry that isn't mapped
    while ( (aErr == KErrNone  || aErr == KErrNotFound) &&  
            (entry = attribIter++) != NULL )
        {
        metaDBVisitor = static_cast<TMDBVisitorBase*>(entry->iMetaNewL(mem,aElement->GetAttribPtr(entry->iOffset)));
        if (metaDBVisitor) // might be deprecated
            {
            metaDBVisitor->SetOwner(aElement);
            TMDBElementId id = metaDBVisitor->ElementId();
            aErr = aSession->LoadElementAttributesL(id);
      
            if (aErr == KErrNone && ( id & (aSession->GetReadAttributeMask() | aSession->GetWriteAttributeMask()) ) )
                {
                aErr = KErrAccessDenied;
                }
            else
            {
	            if ( metaDBVisitor->Changed()  && 
                     ( ! mapper || ! mapper->Mapped(metaDBVisitor->ElementId()) ) &&
                    ( aErr == KErrNotFound  ||  aErr == KErrNone ) )
	                {
	                metaDBVisitor->SetElementId(&id);
	                aErr = metaDBVisitor->ModifyL(aSession, aElement->Attributes());
	                if (aErr == KErrNone)
	                 {
	                 metaDBVisitor->Sync();
#ifndef __TOOLS2__
	            	    aSession->NotifierL()->MaybeNotifyChange(metaDBVisitor->ElementId());
#endif       
	                    }
	                }
                }
            }
        }

    if (aErr == KErrNotFound)
        {
        aErr = KErrNone;
        }
    }



  
EXPORT_C void MMetaDatabase::DeleteL(CMDBSession& aSession)
/*
Deletes each member of the MMetaDatabase object from the database

@publishedAll
*/
    {
    CMDBSessionImpl* session = aSession.iMDBSessionImpl;
    
    TBool resetReadOnly = ! session->IsSetAttributeMask(ECDNoWriteButDelete);
    if (resetReadOnly)
        {
        session->SetAttributeMask(ECDNoWriteButDelete);
        }

    TBool ownTransaction = session->MaybeOpenTransactionL();
    
    CMDBElement* thisElement = static_cast<CMDBElement*>(this);

    __FLOG_STATIC1(KLogComponent, KCDInfoLog, _L("MMetaDatabase::DeleteL() called for Element %08x..."), thisElement->ElementId());

    TInt err = KErrNone;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY    
    CCDValidator::ValidateL(thisElement,*aSession.iMDBSessionImpl, EDelete);
    
    CCDMapper* mapper = CCDMapper::MaybeCreateMapperLC(*thisElement,*aSession.iMDBSessionImpl,EDelete);    
    if (mapper)
        {
        //before doing the delete on the mapped object do some pre mappings...
        mapper->PreMappingsL();
        
        err = session->MaybeDeleteNodeL(mapper, *(mapper->Data()));
        
        mapper->PostMappingsL();

        CleanupStack::PopAndDestroy(mapper);
        }
    else
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY 
        {
        err = session->MaybeDeleteNodeL(thisElement, *thisElement->Data());
        }

    if ( err == KErrNone )
        {
        TAny* mem[KMMetaDBVisitorMaxSize];
        SVDataTableEntry const* entry = NULL;
        TMDBVisitorBase* metaDBVisitor = NULL;
        TMetaVTableIterator attribIter(this);     
		while ((entry = attribIter++) != NULL)
			{
			metaDBVisitor = static_cast<TMDBVisitorBase*>(entry->iMetaNewL(mem,GetAttribPtr(entry->iOffset)));
 			if ( metaDBVisitor && metaDBVisitor->IsFieldLoaded() )
                {
                metaDBVisitor->Change();
                }
			}
        }        

       
    if (resetReadOnly)
        {
        session->ClearAttributeMask(ECDNoWriteButDelete);
        }

    session->MaybeCommitTransactionL(ownTransaction, NULL, err);
    }


#ifndef __TOOLS2__
EXPORT_C TInt MMetaDatabase::RequestNotification(CMDBSession& aSession, TRequestStatus& aStatus)
/*
Registers each set member of the MMetaDatabase object for notification of change in the database
If this function leaves, no effort made to unregister fields..  
 
FUTURE - At the moment CommsDat makes a simple general request for notification on the whole repository.
Because this is all that Commdb does.  It would be good to change this to register for individual areas of change.

@publishedAll
*/  
    {
    if (aSession.iMDBSessionImpl)
        {
        __FLOG_STATIC(KLogComponent, KCDInfoLog, _L("MMetaDatabase::RequestNotification() called"));
    
        TRAPD(retval, aSession.iMDBSessionImpl->NotifierL()->RegisterNotifyL(aStatus));
        return retval;
        }
    
    // LOG error
    return KErrNotFound;
    }
   
   
EXPORT_C TInt MMetaDatabase::CancelNotification(CMDBSession& aSession, TRequestStatus& aStatus)
/*
Cancels Registration of change notification 
 
FUTURE - At the moment CommsDat makes a simple general request for notification on the whole repository.
Because this is all that Commdb does.  It would be good to change this to register for individual areas of change.

@publishedAll
*/  
    {
    if (aSession.iMDBSessionImpl)
        {
        __FLOG_STATIC(KLogComponent, KCDInfoLog, _L("MMetaDatabase::CancelNotification() called"));
        TRAPD(retval, aSession.iMDBSessionImpl->NotifierL()->CancelNotify(aStatus));
        return retval;
        }
    
    // LOG error
    return KErrNotFound;
    } 
#endif // __TOOLS2__






//
// Session object used by MMetaDatabase
//

EXPORT_C CMDBSession* CMDBSession::NewL(::TVersion aRequiredVersion)
/*
NewL

@publishedAll
*/
    {
	CMDBSession* r=NewLC(aRequiredVersion);
	CleanupStack::Pop(r);
	return r;
    }


EXPORT_C CMDBSession* CMDBSession::NewLC(::TVersion aRequiredVersion)
/*
NewLC

@publishedAll
*/
    {
	CMDBSession* r=new(ELeave) CMDBSession();
	CleanupStack::PushL(r);
	r->ConstructL(aRequiredVersion);
	return r;
    }


EXPORT_C CMDBSession::CMDBSession() 
/*
Constructor

@publishedAll
*/
    {
    }



EXPORT_C CMDBSession::~CMDBSession()
/*
Constructor

@publishedAll
*/
    {
    delete iMDBSessionImpl;
    }
    
    

EXPORT_C TInt CMDBSession::ConstructL(TVersion  aRequiredVersion)
/*
Open a session with the storage server and compare data versions
        
The MetaDatabase always attempts to support the required version.  If the versions match,
KErrNone will be returned.  If the versions are not the same, but the requested version
is supported, a warning code will be returned. Otherwise an error will be 
returned.  The latest version is returned in the aLatestVersion parameter for information

@publishedAll
*/
    {
    if ( ! iMDBSessionImpl )
        {
	    iMDBSessionImpl = new(ELeave)CMDBSessionImpl(*this);
        }
    
    return iMDBSessionImpl->ConstructL(aRequiredVersion);
    } 


/* 
Close session with storage server.
*/
EXPORT_C void CMDBSession::Close()
    {
    if (iMDBSessionImpl)
        {
        iMDBSessionImpl->Close();
        }
    }



EXPORT_C TVersion CMDBSession::LatestVersion()
/*
Static Lookup of latest data format version

@publishedAll
*/
    {
    return KCDLatestVersion;
    }



EXPORT_C TVersion CMDBSession::VersionInUse()
/*
Lookup data format version currently in use by client

@publishedAll
*/
    {
    if ( iMDBSessionImpl )
        {
        return iMDBSessionImpl->VersionInUse();
        }

    return TVersion(0,0,0);
    }
    


EXPORT_C void CMDBSession::OpenTransactionL()
/*
Open Transaction with storage server and obtain write lock - if not already set

Will fail with KErrNotAvailable if Database write locked by another process

Will fail if client does not have capabilities to obtain write lock on storage server.

Will report KErrAlreadyInTransaction if transaction already open in this process.

@publishedAll
*/
    {
    if (! iMDBSessionImpl )
        {
        User::Leave(KErrNotFound);
        }
    
    iMDBSessionImpl->MaybeOpenTransactionL();

    __FLOG_STATIC(KLogComponent, KCDInfoLog, _L("CMDBSession::OpenTransactionL - Transaction is opened"));
    }



EXPORT_C void CMDBSession::CommitTransactionL()
/*
Commit Transaction in database.

Will fail with KErrNotFound if not in transaction

Will leave if error occurs during commit.

@publishedAll
*/
    {
    if (! iMDBSessionImpl)
        {
        __FLOG_STATIC(KLogComponent, KCDErrLog, _L("CMDBSession::CommitTransactionL - No Session; commit failed!!!"));
        
        User::Leave(KErrNotFound);
        }   
    
    __ASSERT_DEBUG(iMDBSessionImpl->IsInTransaction(), User::Panic(KCommsDatName,ECommitBeforeOpenTransaction));
    
    // If client requests commit they take control of any existing transaction
    iMDBSessionImpl->MaybeCommitTransactionL(ETrue, 0, KErrNone);
    
    __FLOG_STATIC(KLogComponent, KCDInfoLog, _L("CMDBSession::CommitTransactionL - Transaction is comitted"));
    }



EXPORT_C void CMDBSession::RollbackTransactionL()
/*
Cancel Transaction with database and rollback all associated changes

Will fail with KErrNotFound if not in transaction

@publishedAll
*/
    {
    if( ! iMDBSessionImpl  ||  ! iMDBSessionImpl->IsInTransaction() )
    	{
    	__FLOG_STATIC(KLogComponent, KCDErrLog, _L("CMDBSession::RollbackTransactionL - No Session, or no open Transaction; commit failed!!!"));
    	
    	User::Leave(KErrNotFound);
    	}
 
    // If client requests rollback they take control of any existing transaction
    iMDBSessionImpl->RollbackTransactionL();
    
    __FLOG_STATIC(KLogComponent, KCDInfoLog, _L("CMDBSession::RollbackTransactionL - Transaction is rolled back"));
    }



EXPORT_C TInt CMDBSession::IsInTransaction()
/*
Query if in transaction with database
Return code will indicate 
    Not in transaction, 
    Already in transaction for this session, 

@publishedAll
*/
    {
    if( iMDBSessionImpl)
        {
        return iMDBSessionImpl->IsInTransaction();        
        }

    return KErrNotFound;    
    }


EXPORT_C void CMDBSession::SetAttributeMask(TMDBAttributeFlags aAttributeFlags)
/*
Set the mask the user wishes to use for access control attributes
some attributes are governed by platform security capabilities and can therefore
only be obeyed if user has appropriate capabilities.  

@publishedAll
*/
    {
    if( iMDBSessionImpl)
        {
        iMDBSessionImpl->SetAttributeMask(aAttributeFlags);        
        }
    }


EXPORT_C TBool CMDBSession::IsSetAttributeMask(TMDBAttributeFlags aAttributeFlags)
/*
Check attribute mask flags

@publishedAll
*/
    {
    if( iMDBSessionImpl)
        {
        return iMDBSessionImpl->IsSetAttributeMask(aAttributeFlags);        
        }
    return EFalse;
    }


EXPORT_C void CMDBSession::ClearAttributeMask(TMDBAttributeFlags aAttributeFlags)
/*
Clear the flags from the mask to use for access control attributes

@publishedAll
*/
    {
    if( iMDBSessionImpl)
        {
        iMDBSessionImpl->ClearAttributeMask(aAttributeFlags);        
        }
    }

#ifndef __TOOLS2__
EXPORT_C TInt CMDBSession::CancelAllNotifications()
/*
Set the mask the user wishes to use for access control attributes
some attributes are governed by platform security capabilities and can therefore
only be obeyed if user has appropriate capabilities.  

@publishedAll
*/
    {
    if( iMDBSessionImpl)
        {
        TRAPD(retval, iMDBSessionImpl->NotifierL()->CancelAllNotifications());
        return retval;
        }
    else
        return KErrNone; // No notifications extant
    }


EXPORT_C TInt CMDBSession::PublishProperties()
/*
Attempt to create publish/subscribe properties if not allready defined. The client calling this function 
needs WriteDeviceData capability since these properties are either created in KUidSystemCategory category or
otherwise require protection against spoofing
@publishedAll
*/
	{
    __FLOG_STATIC0(KLogComponent, KCDInfoLog, _L("CMDBSession::PublishProperties()"));

	TInt result(KErrNone);
	TInt propval(KErrNone);
	TSecurityPolicy readPolicy(ECapability_None);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);
    TSecurityPolicy notificationWritePolicy(KCommsDatNotificationSetCapability);

 	result = RProperty::Get(KUidSystemCategory, KUidCommDbProxiesRecordChange.iUid, propval);

    if (result == KErrNotFound)
		{
		result = RProperty::Define(KUidSystemCategory,KUidCommDbSMSBearerChange.iUid, RProperty::EInt, readPolicy, writePolicy);
		__ASSERT_DEBUG(result == KErrPermissionDenied || result == KErrNone,User::Invariant());

        result = RProperty::Define(KUidSystemCategory,KUidCommDbNotificationEvent.iUid, RProperty::EInt, readPolicy, notificationWritePolicy);
		__ASSERT_DEBUG(result == KErrPermissionDenied || result == KErrNone,User::Invariant());

		result = RProperty::Define(KUidSystemCategory,KUidCommDbSMSReceiveModeChange.iUid, RProperty::EInt, readPolicy, writePolicy);
		__ASSERT_DEBUG(result == KErrPermissionDenied || result == KErrNone,User::Invariant());
		result = RProperty::Define(KUidSystemCategory,KUidCommDbGPRSAttachModeChange.iUid, RProperty::EInt, readPolicy, writePolicy);
		__ASSERT_DEBUG(result == KErrPermissionDenied || result == KErrNone,User::Invariant());
		result = RProperty::Define(KUidSystemCategory,KUidCommDbModemTsyNameChange.iUid, RProperty::EInt, readPolicy, writePolicy);
		__ASSERT_DEBUG(result == KErrPermissionDenied || result == KErrNone,User::Invariant());
		result = RProperty::Define(KUidSystemCategory,KUidCommDbModemDataAndFaxChange.iUid, RProperty::EInt, readPolicy, writePolicy);
		__ASSERT_DEBUG(result == KErrPermissionDenied || result == KErrNone,User::Invariant());

		result = RProperty::Define(KUidSystemCategory,KUidCommDbModemPhoneServicesAndSMSChange.iUid, RProperty::EInt, readPolicy, writePolicy);
		__ASSERT_DEBUG(result == KErrPermissionDenied || result == KErrNone,User::Invariant());
		result = RProperty::Define(KUidSystemCategory,KUidCommDbGPRSDefaultParamsChange.iUid, RProperty::EInt, readPolicy, writePolicy);
		__ASSERT_DEBUG(result == KErrPermissionDenied || result == KErrNone,User::Invariant());
		result = RProperty::Define(KUidSystemCategory,KUidCommDbModemRecordChange.iUid, RProperty::EInt, readPolicy, writePolicy);
		__ASSERT_DEBUG(result == KErrPermissionDenied || result == KErrNone,User::Invariant());
		result = RProperty::Define(KUidSystemCategory,KUidCommDbProxiesRecordChange.iUid, RProperty::EInt, readPolicy, writePolicy);
		__ASSERT_DEBUG(result == KErrPermissionDenied || result == KErrNone,User::Invariant());

		}

 	result = RProperty::Get(KUidCommsDatStatusEvent, KCommsDatStatusEventCommitSeq, propval);
    if (result == KErrNotFound)
		{
		result = RProperty::Define(KUidCommsDatStatusEvent, KCommsDatStatusEventCommitSeq, RProperty::EInt, readPolicy, writePolicy);
		__ASSERT_DEBUG(result == KErrPermissionDenied || result == KErrNone,User::Invariant());
		}
	
	return result;
	}	
#endif // __TOOLS2__

//EOF


