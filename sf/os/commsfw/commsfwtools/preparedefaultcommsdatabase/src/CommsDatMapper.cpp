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
// Mapping rules for deprecated elements in the comms database schema
// All data in file
// CONTENTS OF FILE
// 1/  Utilities
// 2/  CCDMapper Base class implementation
// 3/  Mapper classes for individual elements in the database
// a) CCDMapperGlobalSettingsRecord
// b) CCDMapperIAPPrioritySelectionPolicyRecord
// c) CCDMapperAccessPointRecord
// MAINTENANCE - some rules may need adding or amending whenever the database schema changes
// 
//

/**
 @file CommsDatMapper.cpp
 @internalComponent
*/


#include "CommsDatInternalDefs.h"
#include <commsdattypesv1_1.h>
#include "CommsDatMapper.h"
#include <comms-infras/commsdatschema.h>
#include <commsdat.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypesv1_1_partner.h>
#include <commsdat_partner.h>
#endif

using namespace Meta;
using namespace CommsDatInternal;
using namespace CommsDat;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY 
//anonymous namespace
namespace
    {
    const TInt NetworkBaseTagId     = 10000;
    }
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY 
/***************************************************************************************
 * 1/ Utilities
 */



TBool Changed(const CMDBElement& aElement)
/*
Utility function to check if an element has been changed
@internalComponent
*/
    {
    if ( aElement.ElementId() & KCDChangedFlag )
        {
        return ETrue;
        }

    return EFalse;
    }
    

void Sync(CMDBElement& aElement)
/*
Utility function to mark an element in sync with Database
@internalComponent
*/
    {
    aElement.SetElementId(aElement.ElementId() & ~KCDChangedFlag);
    }



TInt CCDMapper::FindLinkElementIdL(TInt aVal, TInt aTargetTable)
/*
utility for mapping link fields - when link fields fixed this will be unnecessary
*/    {
        TInt ret(KErrNone);

        if (KLinkableTag & aVal)
            {
            // this is a link by tag so need to do a find for the tag in the target field
            CMDBField<TUint32>* tagField = new (ELeave) CMDBField<TUint32>(aTargetTable |KCDTIdRecordTag);
            CleanupStack::PushL(tagField);

            *tagField = aVal;

            if (tagField->FindL(iSession.iOwner))
                {
                ret = (tagField->ElementId() | KCDMaskShowRecordTypeAndId);
                }

            CleanupStack::PopAndDestroy(tagField);
       
            }

        // add a check for the record id too.
           
        // otherwise the value already gives the element id directly

        return ret;
        }   


/****************************************************************************************
 * 2/  CCDMapper base class implementation
 ***************************************************************************************/
  
CCDMapper::CCDMapper(TMDBElementId aElementId, CMDBElement& aElement,CMDBSessionImpl& aSession
		             ,const TOperation& aOperation)
	: CMDBRecordBase(aElementId)
    , iElement(aElement)
    , iSession(aSession)
    , iOperation(aOperation)
    {
    iSession.iMapping = ETrue;
    }

        
CCDMapper::~CCDMapper()
    {
    iSession.iMapping = EFalse;
    }
 
   

CCDMapper* CCDMapper::MaybeCreateMapperLC(CMDBElement& aElement,CMDBSessionImpl& aSession, const TOperation& aOperation)
/*
Static Factory function. 

  Creates a mapping object of the appropriate type

@internalComponent
*/
	{
    if (aSession.UsingLatestVersion() || aSession.iMapping)
        {
        // only map if client has asked to manage deprecated fields
        // and if no mapping or validation going on already
        return NULL;
        }

    if(CommsDat::CommsDatSchema::IsTable(aElement.ElementId()))
        {
    	// only need mapper for records and fields, not tables
        return NULL;
        }

	CCDMapper* mapper = NULL;
    
    switch((TUint32)aElement.TypeId() & KCDMaskShowRecordType)
	    {

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	    
        case KCDTIdGlobalSettingsRecord:
	        {
            mapper = new(ELeave)CCDMapperGlobalSettingsRecord(aElement,aSession, aOperation); 
		    break;
	        }
           
	    case KCDTIdIapPrioritySelectionPolicyRecord:
	        {
            mapper = new(ELeave)CCDMapperIAPPrioritySelectionPolicyRecord(aElement,aSession, aOperation); 
            break;
	        }
            
	    case KCDTIdAccessPointRecord:
	        {
	        mapper = new(ELeave)CCDMapperAccessPointRecord(aElement,aSession, aOperation); 
		    break;
	        }

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        
        default :
            {
            // Nothing to do so exit quickly
            mapper = NULL;
            }
		}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

    if(! mapper)
        {
        return NULL;
        }
        
    if (mapper->NeedsMapping(aElement))
        {
       
        CleanupStack::PushL(mapper);
        mapper->ConstructL();
        }
    else 
        {
        delete mapper;
        mapper = NULL;
        }

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

    return mapper;	
	}

   
   

void CCDMapper::ConstructL()
/*
following a unique set of mapping rules implemented in derived classes and
using information from both the input element and the database
1/ Set up record id(s) for elements in the map 
2/ where deprecated elements in the input element (iElement) have values that
have been changed by the user, populate the values of the corresponding map element(s) 
@internalTechnology
*/
    {   
    //  Default is nothing to construct
    return;
    }
    
void CCDMapper::PrepareToStoreL()
    {   
    //  Default is nothing to prepare
    return;
    }


//TBool CCDMapper::Mapped(TMDBElementId aElementId)
/*
Return ETrue if the sub-element (always a field) is mapped.  
Otherwise return EFalse

@internalTechnology
*/
   // {
  //  return aElementId != aElementId;
   // }


void CCDMapper::MapResultsL()
/*
Set up record id(s) for the map and populate each entry in the map
following a unique set of mapping rules (implemented in derived classes)
using information from both the input element and the database

@internalTechnology
*/
    { 
    // Default is nothing to map
    return; 
    }
    
/*virtual*/ void CCDMapper::PostMappingsL()
/*
@internalTechnology
*/    
    {
     // Default is nothing to process...
    (void)iOperation; //just to bypass the comp. warning
    return; 
    }

/*virtual*/ void CCDMapper::PreMappingsL()
/*
@internalTechnology
*/    
    {
     // Default is nothing to process...
    (void)iOperation; //just to bypass the comp. warning
    return; 
    }

/*****************************************************************************************
 * 3/ MAPPER CLASSES FOR INDIVIDUAL ELEMENTS
 *****************************************************************************************/
     
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY 

/************************* 3/ a) Mapper for GlobalSettings Table ************************************
 * KCDTIdGlobalSettingsRecord
 *
 * These 2 fields are now stored against the Default Tier record in the Tier table
 *      DefaultSnap     KCDTIdDefaultSnap    CCDGlobalSettingsRecord::iDefaultSnap
 *      PromptUser      KCDTIdPromptForSnap  CCDGlobalSettingsRecord::iPromptForSnap
 *
 * Corresponding fields in the Tier table (KCDTIdTierRecord) are
 *      DefaultSnap     KCDTIdDefaultAccessPoint  CCDTierRecord::iDefaultAccessPoint
 *	    PromptUser      KCDTIdPromptUser          CCDTierRecord::iPromptUser
 *
 * The mapping relies on the presence of the DefaultTier field in the GlobalSettings Table
 *      DefaultTier     KCDTIdDefaultTier    CCDGlobalSettingsRecord::iDefaultTier
 *
 * The DefaultTier must always be present as a real or default value in the GlobalSettings
 * Table.  A preface config file may be the best way to set this value.
 * 
 * If KCDTIdDefaultTier field is not present in the GlobalSettings Table, 
 * these fields cannot be mapped [They could be set as default values in Template of Tier Table?]
 */
#define KCDTIdGlobalSettingsRecordMapper    9879
START_ATTRIBUTE_TABLE( CCDMapperGlobalSettingsRecord, KCDTIdGlobalSettingsRecordMapper, KCDTIdGlobalSettingsRecordMapper)
	X_REGISTER_ATTRIBUTE( CCDMapperGlobalSettingsRecord, iPromptUser,         TMDBNum)
    //X_REGISTER_ATTRIBUTE( CCDMapperGlobalSettingsRecord, iDefaultAccessPoint, TMDBNum)
    X_REGISTER_ATTRIBUTE( CCDMapperGlobalSettingsRecord, iDefaultAccessPoint, TMDBLinkNum)
END_ATTRIBUTE_TABLE()

CCDMapperGlobalSettingsRecord::CCDMapperGlobalSettingsRecord(CMDBElement& aElement,CMDBSessionImpl& aSession, const TOperation& aOperation)
: CCDMapper(0, aElement,aSession,aOperation), 	
  iPromptUser(KCDTIdPromptUser),
  iDefaultAccessPoint(KCDTIdDefaultAccessPoint)		
    {}


void CCDMapperGlobalSettingsRecord::ConstructL()
/*
Set up record id(s) for elements in the map and populate the values of 
each each element in the map following a unique set of mapping rules 
(implemented in derived classes) using information from both the input 
element and the database

 * Rules 
    a) find DefaultTier in GlobalSettings record (from inputElement or database)
    b) use the value to give the record id of the Tier record

@internalComponent
*/
    {   
    CMDBRecordLink<CCDTierRecord> *defaultTier = new(ELeave) CMDBRecordLink<CCDTierRecord>(KCDTIdGlobalSettingsRecord | KCDTIdDefaultTier); 
    CleanupStack::PushL(defaultTier);
    defaultTier->SetRecordId(1);
    
    defaultTier->LoadL(iSession.iOwner);
    
    if (CommsDat::CommsDatSchema::IsRecord(iElement.ElementId()))
        {
        CCDGlobalSettingsRecord& globalSettings = static_cast<CCDGlobalSettingsRecord&>(iElement);
        
        if (! globalSettings.iDefaultTier.IsNull())
            {
            defaultTier->SetL(globalSettings.iDefaultTier);
            }
        }

    TMDBElementId targetId = CommsDatSchema::GetLinkIdL(iSession.iOwner, defaultTier->ElementId(), defaultTier->GetL(), KCDTIdTierRecord/*, NULL*/);
    
    CleanupStack::PopAndDestroy(defaultTier);

    SetRecordId(targetId);
         
    // Now populate the data from the input element
    CreateMapL();
    }


void CCDMapperGlobalSettingsRecord::CreateMapL()
/*
Populate the values of each each element in the map from data in the input element
following a unique set of mapping rules.
NB. Should only use data in the input element (iElement), not the database
The record ids of the mapping elements have already been set in InitMapL
      
@internalComponent
*/
    {           
    //switch (iElement.TypeId())
    switch (iElement.ElementId() & KCDMaskShowType)
        {
       
        case KCDTIdGlobalSettingsRecord | KCDMaskShowFieldType:
            {
            CCDGlobalSettingsRecord* globalSettings = static_cast<CCDGlobalSettingsRecord*>(&iElement);
            
            if (Changed(globalSettings->iDefaultSnap))
            	{
            	/* There are cases when the 'DefaultSnap' field in the GS record
            	 * is 0 (explicitly). In this case the GS mapper checks wheather
            	 * the given field is changed or not (it's changed) BUT during the
            	 * STORE operation we don't want to store and map the 0 value because
            	 * this would mean to overwrite the default AP field in the NetworkTier,
            	 * which is set to a default value form the meshpreface.cfg file.
            	 * So the check here checks:
            	 * if we are in a STORE operation and the DefualtSNAP field in the GS
            	 *    record is 0 then we change the field back to the 'unchanged state'.*/
            	if (0 == globalSettings->iDefaultSnap.GetL() &&
            	    EStore == iOperation)
            		{
            		//change back the field to the 'unchanged' state
            		iDefaultAccessPoint.SetElementId(iDefaultAccessPoint.ElementId() & ~KCDChangedFlag);
            		}
            	else
            		{
            		//Assuming that the defaultSnap id the Snap config file is an AP record id
            		iDefaultAccessPoint.SetL(KCDTIdAccessPointRecord | (globalSettings->iDefaultSnap << 8));
            		}
            	}

            if (Changed(globalSettings->iPromptForSnap))
                {
                iPromptUser.SetL(globalSettings->iPromptForSnap);
                }

            break;
            }
        //case KCDTIdDefaultAccessPoint :
        case KCDTIdDefaultSnap :
            {
        	if (Changed(iElement))
                {
                iDefaultAccessPoint.SetL(*static_cast<CMDBRecordLinkBase*>(&iElement));
                }
            break;
            }
        case KCDTIdPromptForSnap :
            {
            if (Changed(iElement))
                {
                iPromptUser.SetL(*static_cast<CMDBField<TBool>*>(&iElement)); 
                }
            break;
            }
        default :
            {
            ASSERT(KErrNotFound); // hmmm... shouldn't get here.
            User::Leave(KErrNotFound); 
            }
        }
    }

/**
    if the mapped fields are already existing in the Tier record on the Network
    level, they should be deleted.
*/
/*virtual*/ void CCDMapperGlobalSettingsRecord::PrepareToStoreL()
    {
    //Create the storage for the original GlobalSettings record...
    iSession.MaybeCreateNodeL(iElement);
        
    if (Changed(iPromptUser))
        {
        iPromptUser.SetRecordId(RecordId());
        iPromptUser.DeleteL(iSession.iOwner);
        }
        
    if (Changed(iDefaultAccessPoint))
        {
        iDefaultAccessPoint.SetRecordId(RecordId());
        iDefaultAccessPoint.DeleteL(iSession.iOwner);
        }
	
    }

TBool CCDMapperGlobalSettingsRecord::Mapped(TMDBElementId aElementId)
/*
Return ETrue for any element that is mapped.  

@internalComponent
*/
    {
    //switch(aElementId & KCDMaskShowFieldType)
    switch(aElementId & KCDMaskShowType)
		{
 		case KCDTIdDefaultSnap :
		case KCDTIdPromptForSnap :
		    {
            return ETrue;
		    }
        }
	return EFalse; 
    }
	
TBool CCDMapperGlobalSettingsRecord::NeedsMapping(CMDBElement& aElement)
/*
Return ETrue for any element that is mapped.  

@internalComponent
*/
    {
    if(CommsDat::CommsDatSchema::IsRecord(iElement.ElementId()))
        {
        //this is a record mapping and the session used is not the latest. So let's have a mapper...
        /*CCDGlobalSettingsRecord& globalSettings = static_cast<CCDGlobalSettingsRecord&>(iElement);
   
        if (Changed(globalSettings.iDefaultSnap) ||
            Changed(globalSettings.iPromptForSnap)   )
            {
            return ETrue;
            }*/
        return ETrue;
        }

    return Mapped(aElement.ElementId());
    }


void CCDMapperGlobalSettingsRecord::MapResultsL()
/*
Map results to original element

@internalComponent
*/
    {
    //switch (iElement.TypeId())
    switch (iElement.ElementId() & KCDMaskShowType)
        {
       
        case KCDTIdGlobalSettingsRecord | KCDMaskShowFieldType:
            {
            CCDGlobalSettingsRecord* globalSettings = static_cast<CCDGlobalSettingsRecord*>(&iElement);
            
            globalSettings->iDefaultSnap.SetL(iDefaultAccessPoint);
            //give back just the record number of the linked AP - as in the good old days...
            CMDBRecordLink<CCDAccessPointRecord>* defaultSnap = static_cast<CMDBRecordLink<CCDAccessPointRecord>*>(&(globalSettings->iDefaultSnap));
            *defaultSnap = (*defaultSnap & KCDMaskShowRecordId) >> 8;
            Sync(globalSettings->iDefaultSnap);
            
            globalSettings->iPromptForSnap.SetL(iPromptUser);
            Sync(globalSettings->iPromptForSnap);
            
            break;
            }
        case KCDTIdDefaultSnap :
        	
            {
            //CMDBField<TInt>* defaultSnap = static_cast<CMDBField<TInt>*>(&iElement);
            CMDBRecordLink<CCDAccessPointRecord>* defaultSnap = static_cast<CMDBRecordLink<CCDAccessPointRecord>*>(&iElement);
            defaultSnap->SetL(iDefaultAccessPoint);
            //give back just the record number of the linked AP - as in the good old days...
            *defaultSnap = (*defaultSnap & KCDMaskShowRecordId) >> 8;
            Sync(*defaultSnap);
            break;
            }
        case KCDTIdPromptForSnap :
            {
            CMDBField<TBool>* promptUser = static_cast<CMDBField<TBool>*>(&iElement);
            promptUser->SetL(iPromptUser); 
            Sync(*promptUser);
            break;
            }
        default :
            {
            ASSERT(KErrNotFound); // hmmm... shouldn't get here.
            User::Leave(KErrNotFound); 
            }
        }
    }


/************************* Mapper for IAPSelection Table **************************************
 * KCDTIdIapPrioritySelectionPolicyRecord   CCDIAPPrioritySelectionPolicyRecord
 * 
 * The IAP Selection table is deprecated and completely replaced by the APSelectionTable,
 *
 * KCDTIdApPrioritySelectionPolicyRecord  CCDIAPPrioritySelectionPolicyRecord
 *
 * As IAPs map directly to APs, the IAP Selection Table that links to IAPs can map directly to 
 * an AP selecton Table that points to APs that each map to a single IAP
 * 
 * The mapping can be very simple because the record id, name, tag and so on can be identical
 */

#define KCDTIdIapPrioritySelectionPolicyRecordMapper    9880
START_ATTRIBUTE_TABLE( CCDMapperIAPPrioritySelectionPolicyRecord, KCDTIdIapPrioritySelectionPolicyRecordMapper, KCDTIdIapPrioritySelectionPolicyRecordMapper)
  	X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp1,         TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp2,         TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp3,         TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp4,         TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp5,         TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp6,         TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp7,         TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp8,         TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp9,         TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp10,        TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp11,        TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp12,        TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp13,        TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp14,        TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iAp15,        TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iApCount,     TMDBNum)
    X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iRecordTag,   TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDMapperIAPPrioritySelectionPolicyRecord, iRecordName,  TMDBText)
END_ATTRIBUTE_TABLE()

CCDMapperIAPPrioritySelectionPolicyRecord::CCDMapperIAPPrioritySelectionPolicyRecord(CMDBElement& aElement,CMDBSessionImpl& aSession, const TOperation& aOperation)
  : CCDMapper(KCDTIdApPrioritySelectionPolicyRecord | (aElement.ElementId() & KCDMaskShowRecordId), aElement, aSession, aOperation),
    iAp1(KCDTIdAp1),
    iAp2(KCDTIdAp2),
    iAp3(KCDTIdAp3),
    iAp4(KCDTIdAp4),
    iAp5(KCDTIdAp5),
    iAp6(KCDTIdAp6),
    iAp7(KCDTIdAp7),
    iAp8(KCDTIdAp8),
    iAp9(KCDTIdAp9),
    iAp10(KCDTIdAp10),
    iAp11(KCDTIdAp11),
    iAp12(KCDTIdAp12),
    iAp13(KCDTIdAp13),
    iAp14(KCDTIdAp14),
    iAp15(KCDTIdAp15),
    iApCount(KCDTIdApCount),
    iRecordTag(KCDTIdRecordTag | KCDTIdApPrioritySelectionPolicyRecord),
    iRecordName(KCDTIdRecordName | KCDTIdApPrioritySelectionPolicyRecord)
    {
    //Check whether the original record is set as a 'KCDNewRecordRequest'...
    if (CommsDatSchema::IsNewRecordRequest(iElement.ElementId()))
        {
        SetRecordId(KCDNewRecordRequest);
        }
    }

CCDMapperIAPPrioritySelectionPolicyRecord::~CCDMapperIAPPrioritySelectionPolicyRecord()
	{
	delete origMappedDataInDB;
	}

void CCDMapperIAPPrioritySelectionPolicyRecord::ConstructL()
/*
Iterate through the mapper element and populate each entry 
following a unique set of mapping rules.
The element is populated by information from both the input element and the database
ConstructL is performed on data input and retrieval.  Mustn't include calls specific to only one or  the other
@internalComponent
*/
    {   
     switch (iElement.TypeId())
        {
        case KCDTIdRecordTag :
            {
            if (Changed(iElement))
                {
                iRecordTag.SetL(*static_cast<CMDBField<TInt>*>(&iElement));
                } 
            break;
            }
        case KCDTIdRecordName :
            {
            if (Changed(iElement))
                {
                iRecordName.SetL(*static_cast<CMDBField<TDesC>*>(&iElement));
                }
            break;
            }
        case KCDTIdIapCount :
            {
            if (Changed(iElement))
                {
                iApCount.SetL(*static_cast<CMDBField<TInt>*>(&iElement));
                }
            break;
            }
        
        case KCDTIdIapPrioritySelectionPolicyRecord | KCDMaskShowFieldType:
            {
  
            // NB.  This function relies on APs being already present

            //CMDBElement& thisElement = static_cast<CMDBElement&>(*this);
  
            CCDIAPPrioritySelectionPolicyRecord& inputElement = static_cast<CCDIAPPrioritySelectionPolicyRecord&>(iElement);
            
           if (Changed(inputElement.iRecordTag))
                {
                iRecordTag.SetL(inputElement.iRecordTag);
                }    
            if (Changed(inputElement.iRecordName))
                {
                iRecordName.SetL(inputElement.iRecordName);
                }
            if (Changed(inputElement.iIapCount))
                {
                iApCount.SetL(inputElement.iIapCount);
                }
            	
            // as every other field is the same type and the mapping rule is identical for each 
            // and mapping is 1:1, just iterate through the fields and map each one
            TInt fieldCounter(15); // only need to map this many fields

            TMetaVTableIterator mapperAttribIter(this);
            SVDataTableEntry const* mapperEntry = NULL;

            TMetaVTableIterator inputAttribIter(&iElement);
            SVDataTableEntry const* inputEntry = NULL;
             
            // NB this only works in this case because entries map exactly....
            while ( (mapperEntry = mapperAttribIter++)  != NULL &&
                    (inputEntry = inputAttribIter++)    != NULL &&
                    fieldCounter-- > 0 )
                {
	            CMDBField<TUint32>* inputFieldPtr = reinterpret_cast<CMDBField<TUint32>*>(iElement.GetAttribPtr(inputEntry->iOffset));
	            //CMDBRecordLink<CCDIAPRecord>* inputFieldPtr = reinterpret_cast<CMDBRecordLink<CCDIAPRecord>*>(GetAttribPtr(mapperEntry->iOffset));
                //if (inputFieldPtr && Changed(*inputFieldPtr) )
	            //we need to check the changed flag too -> somebody changes a link to 0...
                if (inputFieldPtr/* && ((*inputFieldPtr != NULL) || Changed(*inputFieldPtr)) */)
                    {
                    CMDBRecordLink<CCDAccessPointRecord>* mapperFieldPtr = reinterpret_cast<CMDBRecordLink<CCDAccessPointRecord>*>(GetAttribPtr(mapperEntry->iOffset));
                    if (mapperFieldPtr)
                        {
                        if ( ( (*inputFieldPtr) & KCDMaskShowType ) == 0 &&
                             ( (*inputFieldPtr) & KLinkableTag ) == 0 )
                             //there is only a number for the IAP link in the table...
                            {
                            if (*inputFieldPtr == 0)
                            	{
                            	//special case, the link is 0 
                            	mapperFieldPtr->SetL(*inputFieldPtr);
                            	
                            	if (!Changed(*inputFieldPtr))
                            		{
                            		//this means that we have to remove the changed flag from our mapped field
                            		//because orginally no value was set for the input field that's why it's 0.
                            		mapperFieldPtr->SetElementId(mapperFieldPtr->ElementId() & ~KCDChangedFlag);
                            		}
                            	}
                            else
                            	{
                            	mapperFieldPtr->SetL(*inputFieldPtr | KLinkableTag);
                            	}
                            }
                        else
                            //the link is correctly set:
                            //it's in Link.<TableName>.<TagId> OR
                            //        <TableName>.<RecordId> format.
                            {
                            /*IMPORTANT: this code will work ONLY if the linking in the original
                                         IAPPrioritySelectionPolicyTable is done by record ID.
                                         
                                         If the linking is by TagId then a FindL is needed here...*/
                            if (!Changed(*inputFieldPtr))
                            	{
                            	//This means that there is a link field in the IAPPrioritySelPol record where
                            	//the link field is correctly set but is't not changed. This means that this field
                            	//is already stored in the DB. So we can try to read the already mapped element.
                            	mapperFieldPtr->SetRecordId(iElement.RecordId());
                            	TRAPD(err, mapperFieldPtr->LoadL(iSession.iOwner));
                            	if (KErrNone != err)
                            		{
                            		//the field is not in the database. Is the client playing with us??
                            		TUint field = *inputFieldPtr & KCDMaskShowRecordId;
                            		
                            		mapperFieldPtr->SetL( (field >> 8) | KLinkableTag);
                            		}
                            	}
                            else
                            	{
                            	//this is a modified field
                            	TUint field = *inputFieldPtr & KCDMaskShowRecordId;
                            	
                            	mapperFieldPtr->SetL( (field >> 8) | KLinkableTag);
                            	}
                            }
                        }
                    }
                }
            break;
            }
        default:
        	{
        	//find out which field is used in the IAPPriorityselPol record
        	CCDIAPPrioritySelectionPolicyRecord* iapPriorSelPol = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord));
        	CleanupStack::PushL(iapPriorSelPol);
        	
            TInt fieldCounter(15); // only need to map this many fields

            TMetaVTableIterator mapperAttribIter(this);
            SVDataTableEntry const* mapperEntry = NULL;

            TMetaVTableIterator inputAttribIter(iapPriorSelPol);
            SVDataTableEntry const* inputEntry = NULL;
            
            CMDBField<TUint32>* origInputFieldPtr = static_cast<CMDBField<TUint32>*>(&iElement);
            
            // NB this only works in this case because entries map exactly....
            while ( (mapperEntry = mapperAttribIter++)  != NULL &&
                    (inputEntry = inputAttribIter++)    != NULL &&
                    fieldCounter-- > 0 )
            	{
            	CMDBField<TUint32>* inputFieldPtr = reinterpret_cast<CMDBField<TUint32>*>(iapPriorSelPol->GetAttribPtr(inputEntry->iOffset));
            	
            	TMDBElementId filteredOrigInputElemId = origInputFieldPtr->ElementId() & KCDMaskShowType; 
            	
            	if ( inputFieldPtr &&
            		 inputFieldPtr->ElementId() == filteredOrigInputElemId  )
            		{
            		CMDBRecordLink<CCDAccessPointRecord>* mapperFieldPtr = reinterpret_cast<CMDBRecordLink<CCDAccessPointRecord>*>(GetAttribPtr(mapperEntry->iOffset));
            		if (mapperFieldPtr)
            			{
                        if ( ( (*origInputFieldPtr) & KCDMaskShowType ) == 0 &&
                             ( (*origInputFieldPtr) & KLinkableTag ) == 0 )
                             //there is only a number for the IAP link in the table...
                            {
                            if (*origInputFieldPtr == 0)
                            	{
                            	//special case, the link is 0 
                            	mapperFieldPtr->SetL(*origInputFieldPtr);
                            	
                            	if (!Changed(*origInputFieldPtr))
                            		{
                            		//this means that we have to remove the changed flag from our mapped field
                            		//because orginally no value was set for the input field that's why it's 0.
                            		mapperFieldPtr->SetElementId(mapperFieldPtr->ElementId() & ~KCDChangedFlag);
                            		}
                            	}
                            else
                            	{
                            	mapperFieldPtr->SetL(*origInputFieldPtr | KLinkableTag);
                            	}
                            }
                        else
                            //the link is correctly set:
                            //it's in Link.<TableName>.<TagId> OR
                            //        <TableName>.<RecordId> format.
                            {
                            /*IMPORTANT: this code will work ONLY if the linking in the original
                                         IAPPrioritySelectionPolicyTable is done by record ID.
                                         
                                         If the linking is by TagId then a FindL is needed here...*/
                            if (!Changed(*origInputFieldPtr))
                            	{
                            	//This means that there is a link field in the IAPPrioritySelPol record where
                            	//the link field is correctly set but is't not changed. This means that this field
                            	//is already stored in the DB. So we can try to read the already mapped element.
                            	mapperFieldPtr->SetRecordId(iElement.RecordId());
                            	TRAPD(err, mapperFieldPtr->LoadL(iSession.iOwner));
                            	if (KErrNone != err)
                            		{
                            		//the field is not in the database. Is the client playing with us??
                            		TUint field = *origInputFieldPtr & KCDMaskShowRecordId;
                            		
                            		mapperFieldPtr->SetL( (field >> 8) | KLinkableTag);
                            		}
                            	}
                            else
                            	{
                            	//this is a modified field
                            	TUint field = *origInputFieldPtr & KCDMaskShowRecordId;
                            	if (field)
                            		{
                            		mapperFieldPtr->SetL( (field >> 8) | KLinkableTag);
                            		}
                            	else
                            		{
                            		mapperFieldPtr->SetL(0);
                            		}
                            	}
                            }
            			}
            		}
            	}
            CleanupStack::PopAndDestroy(iapPriorSelPol);
            break;
        	}
        }
    }

void CCDMapperIAPPrioritySelectionPolicyRecord::PreMappingsL()
	{
	if ( KCDTIdRecordTag != iElement.TypeId() &&
	     KCDTIdRecordName != iElement.TypeId() &&
	     KCDTIdIapCount != iElement.TypeId() )
		{
		if (EFind == iOperation)
			{
			//we don't know the elementId of the orig container so we can't call LoadL
	    	//from the input value we can find the link level AP. Following the link
	    	//backwards we can find the IPProto level AP.
	    	//1. find the already changed element in the mapper object
	    	CMDBRecordLink<CCDAccessPointRecord>* apXLink = &(this->iAp1);
	    	for (TInt i = 1; i<= MaxLinkCount; ++i)
	    		{
	    		if (Changed(*apXLink))
	    			{
	    			//this is the changed element
	    			*apXLink = CommsDatSchema::GetLinkIdL(iSession.iOwner, apXLink->ElementId(), *apXLink, 0);
	    			
	    			//at this point we know that what is the link level AP which is the pair of the IAP given as
	    			//a value in the IAPPrioritySelPol link. Let's find the IPProto level AP which this link level
	    			//AP is linking from.
	    			CMDBField<TInt>* linkLevelAPTagId = new(ELeave) CMDBField<TInt>(*apXLink | KCDTIdRecordTag);
	    			CleanupStack::PushL(linkLevelAPTagId);
	    			
	    			linkLevelAPTagId->SetRecordId((*apXLink & KCDMaskShowRecordId) >> 8);
	    			linkLevelAPTagId->LoadL(iSession.iOwner);
	    			
	    			CMDBRecordSet<CCDAccessPointRecord>* apRecordSet = new(ELeave) CMDBRecordSet<CCDAccessPointRecord>(KCDTIdAccessPointRecord);
	    			CleanupStack::PushL(apRecordSet);
	    			
	    			CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	    			CleanupStack::PushL(apRecord);
	    			
	    			apRecord->iCustomSelectionPolicy.SetL(*linkLevelAPTagId);
	
	    			/* Find all of the AccessPoint records records in the database which have the given 
	    			 * customSelectionPolicy value. It's VERY VERY VERY expensive. However this is the only way we can
	    			 * search back the links from the link level up to the APPrioritySelPol record.
	    			 */
	    			apRecordSet->iRecords.AppendL(apRecord);
	    			
	    			//the ownership is at the recordset already
	    			//CleanupStack::PopAndDestroy(apRecord);
	    			CleanupStack::Pop(apRecord);
	    			
	    			TInt recCount = 0;
	    			if(apRecordSet->FindL(iSession.iOwner))
	    				{
	    				recCount = apRecordSet->iRecords.Count();
	    				}
	    			if (recCount > 0)
	    				{
	    				//we have records with the given customSelectionPolicy value
	    				//now have to find that which accesspoint elementID ws set in the given position of the mapper object
	    				//originally.
	    				
	    				CCDAPPrioritySelectionPolicyRecord* probeApPrioritySelPol = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
	    				CleanupStack::PushL(probeApPrioritySelPol);
	    				
	    				//we know that which field was modified originally
	    				CMDBRecordLink<CCDAccessPointRecord>* probeField = &(probeApPrioritySelPol->iAp1);
	    				
	    				//position the field correctly
	    				for (TInt j = 1; j < i; ++j)
	    					{
	    					++probeField;
	    					}
	    				
	    				TBool found = EFalse;
	    				for (TInt j = 0; (j < apRecordSet->iRecords.Count()) && !found; ++j)
	    					{
	    					probeField->SetL(apRecordSet->iRecords[j]->ElementId());
	    					if (probeApPrioritySelPol->FindL(iSession.iOwner))
	    						{
	    						found = ETrue;
	    						}
	    					}
	    				
	    				if (found)
	    					{
	    					//ok, got the correct record
	    					apXLink->SetL(*probeField);
	    					}
	    				else
	    					{
	    					// no apprioritySelPol record found with the given accesspoint link
	    					//User::Leave(KErrNotFound);
	    					return;
	    					}
	    				
	    				CleanupStack::PopAndDestroy(probeApPrioritySelPol);
	    				}
	    			else
	    				{
	    				//no records were found with the given customSelectionPolicy value
	    				//User::Leave(KErrNotFound);
	    				return;
	    				}
	    			
	    			CleanupStack::PopAndDestroy(apRecordSet);
	    			CleanupStack::PopAndDestroy(linkLevelAPTagId);
	    			}
	    		++apXLink;
	    		}
			}
		else
			{
			//load the original mapped element.
			origMappedDataInDB = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
			origMappedDataInDB->SetRecordId(iElement.RecordId());
			
			origMappedDataInDB->LoadL(iSession.iOwner);
			
			CCDAPPrioritySelectionPolicyRecord* origMappedRecord = static_cast<CCDAPPrioritySelectionPolicyRecord*>(origMappedDataInDB);
			if ( KCDMaskShowFieldType != (iElement.ElementId() & KCDMaskShowFieldType) )
				{
				//this is a field. What happens here is we fill up the mapper with the mapped object 
				//already stored in the db except that field which is currently changed and already set in the
				//ConstructL of the mapper.
				CMDBRecordLink<CCDAccessPointRecord>* origMappedFieldPtr = &(origMappedRecord->iAp1);
				CMDBRecordLink<CCDAccessPointRecord>* currentMappedFieldPtr = &(this->iAp1);
				
				for (TInt i = 1; i <= MaxLinkCount; ++i)
					{
					if (!Changed(*currentMappedFieldPtr) && currentMappedFieldPtr->IsNull())
						{
						//it's not the field which has been changed so fill up from the DB
						currentMappedFieldPtr->SetL(*origMappedFieldPtr);
						currentMappedFieldPtr->SetElementId(currentMappedFieldPtr->ElementId() & ~ KCDChangedFlag);
						//it's possible that the field value is 0 -> so remove the not_null flag in this case
						if (0 == *currentMappedFieldPtr)
							{
							currentMappedFieldPtr->SetElementId(currentMappedFieldPtr->ElementId() & ~ KCDNotNullFlag);
							}
						}
					else
						{
						//there is a changed element
						if (EDelete == iOperation)
							{
							//this is a tricky case here. There is a DeleteL call on a field in some IAPPrioritySelPol
							//record. Since the mapping is record based we cannot delete the whole record (I mean the
							//mapped APPrioritySelPol record) so we have to change the elementId of the mapper object
							//because wwithout this the MaybeDeleteNodeL call would delete the whole record and this is
							//not we would like to do here...
							SetElementId(ElementId() & ~KCDMaskShowFieldType);
							SetElementId(ElementId() | (currentMappedFieldPtr->ElementId() & KCDMaskShowFieldType));
							currentMappedFieldPtr->SetElementId(currentMappedFieldPtr->ElementId() | KCDChangedFlag);
							}
						}
					++origMappedFieldPtr;
					++currentMappedFieldPtr;
					}
				//ok all of the fields are now ready to handle. Set the count field too
				this->iApCount.SetL(origMappedRecord->iApCount);
				}
			}
		}
	}


TBool CCDMapperIAPPrioritySelectionPolicyRecord::NeedsMapping(CMDBElement& /*aElement*/)
    {
    return ETrue;
    }

/*virtual*/ void CCDMapperIAPPrioritySelectionPolicyRecord::PrepareToStoreL()
    {
    CMDBElement& thisElement = static_cast<CMDBElement&>(*this);
    
    iSession.MaybeCreateNodeL(thisElement);
    if (iElement.ElementId() & KCDNewRecordRequest == KCDNewRecordRequest)
        {
        iElement.SetRecordId(this->RecordId());
        }
    }

TBool CCDMapperIAPPrioritySelectionPolicyRecord::Mapped(TMDBElementId /*aElementId*/)
/*
Return ETrue for any element that is mapped.  

@internalComponent
*/
    {
    // Every element is mapped in this record so return true
    return ETrue;
    }
	

void CCDMapperIAPPrioritySelectionPolicyRecord::MapResultsL()
/*
Map results to original element

@internalComponent
*/
    {
    switch (iElement.TypeId())
        {
        case KCDTIdRecordTag :
            {
            CMDBField<TUint32>& recordTag = static_cast<CMDBField<TUint32>&>(iElement);
            recordTag.SetL(iRecordTag);
            Sync(recordTag);
            break;
            }
        case KCDTIdRecordName :
            {
            CMDBField<TDesC>& recordName = static_cast<CMDBField<TDesC>&>(iElement);
            recordName.SetL(iRecordName);
            Sync(recordName);
            break;
            }
        case KCDTIdIapCount :
            {
            CMDBField<TUint32>& iapCount = static_cast<CMDBField<TUint32>&>(iElement);
            iapCount.SetL(iApCount);
            Sync(iapCount);
            break;
            }
        
        case KCDTIdIapPrioritySelectionPolicyRecord | KCDMaskShowFieldType:
            {
            CCDIAPPrioritySelectionPolicyRecord& inputElement = static_cast<CCDIAPPrioritySelectionPolicyRecord&>(iElement);

            inputElement.iRecordTag.SetL(iRecordTag);
            Sync(inputElement.iRecordTag);
 
            inputElement.iRecordName.SetL(iRecordName);
            Sync(inputElement.iRecordName);
                
            inputElement.iIapCount.SetL(iApCount);
            Sync(inputElement.iIapCount);
            
            //NO BREAK HERE - FALL THROUGH TO DEFAULT
            }
        default :
            {
            if ( KCDMaskShowFieldType != (iElement.ElementId() & KCDMaskShowFieldType ) )
            //Field mapping
                {
                const STableLookup* tableInfoPtr = CommsDatSchemaV1_1::iTableLookup;
                TMDBElementId origTableId = iElement.ElementId() & KCDMaskShowRecordType;
                TInt i = 0;
                
                while( tableInfoPtr && tableInfoPtr->iTableId != 0 && (tableInfoPtr->iTableId) != origTableId)
                    {
                    ++i;        
                    ++tableInfoPtr;
                    }
                    
                //At this point we know that what is the table of the mapped field.
                //Now we should search for the field in the table and get the position of it.
                //it is important because as the mapping here is 1:1 this position will be the 
                //position of the mapped field either.
                const CommsDat::SRecordTypeInfo* fieldInfoPtr = CommsDatSchemaV1_1::iTableLookup[i].iFields;
                TMDBElementId cleanOrigFieldId = iElement.ElementId() & KCDMaskShowType;
                TInt fieldPosition = 0;
                
                //Skip the tagId and the name fields...
                ++fieldInfoPtr;
                ++fieldInfoPtr;
                
                while( fieldInfoPtr && fieldInfoPtr->iTypeId != 0 && (fieldInfoPtr->iTypeId) != cleanOrigFieldId)
                {
                ++fieldPosition;        
                ++fieldInfoPtr;
                }
                
                TMetaVTableIterator mapperAttribIter(this);
                SVDataTableEntry const* mapperEntry = NULL;
                CMDBField<TUint32>* mapperFieldPtr = NULL;
                
                //just get into the right position with the attribute iterator for the mapper
                while ( (mapperEntry = mapperAttribIter++) != NULL && 
                        fieldPosition-- >= 0 )
                    {
                    mapperFieldPtr = reinterpret_cast<CMDBField<TUint32>*>(GetAttribPtr(mapperEntry->iOffset));
                    }
                
	                CMDBField<TUint32>* custSelPolOfTheMappedRecord = new(ELeave) CMDBField<TUint32>((*mapperFieldPtr) | KCDTIdCustomSelectionPolicy);
	                CleanupStack::PushL(custSelPolOfTheMappedRecord);
	                custSelPolOfTheMappedRecord->LoadL(iSession.iOwner);
	                
	                TInt mappedIAPRecId = (*custSelPolOfTheMappedRecord) << 8;
	                
	                CleanupStack::PopAndDestroy(custSelPolOfTheMappedRecord);
	                
                    CMDBField<TUint32>& mappedField = reinterpret_cast<CMDBField<TUint32>& >(iElement);
                    mappedField.SetL(KCDTIdIAPRecord | mappedIAPRecId);
                    Sync(mappedField);
                }
            else
            //Record mapping
                {
                // as every other field is the same type and the mapping rule is identical for each 
                // and mapping is 1:1, just iterate through the fields and map each one
                TMetaVTableIterator inputAttribIter(&iElement);
                SVDataTableEntry const* inputEntry = NULL;

                TMetaVTableIterator mapperAttribIter(this);
                SVDataTableEntry const* mapperEntry = NULL;
              
                TInt fieldCounter(15); // only need to map this many fields (from start of record)

                while ( (mapperEntry = mapperAttribIter++) != NULL && 
                        (inputEntry = inputAttribIter++) != NULL &&
                        fieldCounter-- > 0 )
                    {
    	            CMDBField<TUint32>* mapperFieldPtr = reinterpret_cast<CMDBField<TUint32>*>(GetAttribPtr(mapperEntry->iOffset));
                    if( mapperFieldPtr )
                    /*At this point we already know that what is recordNumber of the MAPPED element.
                      So all we have to do is to load the mapped element and read its TagId.*/
                        {                        
                        if ((mapperFieldPtr->ElementId() & KCDNotNullFlag) != 0)
                            {
                            if (0 != *mapperFieldPtr)
                            	{
                            	//it's possible that the link value is 0 because there can be holes
                            	//in the record links...
	                            CommsDatMapperAndValidator::TAPTypes apType = CheckLinkLevelAPL(*mapperFieldPtr);
	                            TInt mappedIAPRecId = 0;
	                            
	                            if (CommsDatMapperAndValidator::ELinkLevel == apType)
	                            //this is already a link level AP so the tagId of it can be used as record ID
	                            //for the pointed IAP record
	                                {
	                                CMDBField<TUint32>* tagFieldOfTheMappedRecord = new(ELeave) CMDBField<TUint32>((*mapperFieldPtr) | KCDTIdRecordTag);
	                                CleanupStack::PushL(tagFieldOfTheMappedRecord);
	                                tagFieldOfTheMappedRecord->LoadL(iSession.iOwner);
	                                
	                                mappedIAPRecId = (*tagFieldOfTheMappedRecord) << 8;
	                                
	                                CleanupStack::PopAndDestroy(tagFieldOfTheMappedRecord);
	                                }
	                            else
	                            //the other case is that this is an IPProto level AP (it cannot be network level
	                            //because it is checked when storing the mapped APPrioritySelPol record).
	                            //here we have to read the customSelectionPolicy field of the AP which gives us
	                            //the TagId of the link level AP which the tag id of it is the same as the 
	                            //recordId of the mapped IAP recordId. :)
	                                {
	                                CMDBField<TUint32>* custSelPolOfTheMappedRecord = new(ELeave) CMDBField<TUint32>((*mapperFieldPtr) | KCDTIdCustomSelectionPolicy);
	                                CleanupStack::PushL(custSelPolOfTheMappedRecord);
	                                custSelPolOfTheMappedRecord->LoadL(iSession.iOwner);
	                                
	                                mappedIAPRecId = (*custSelPolOfTheMappedRecord) << 8;
	                                
	                                CleanupStack::PopAndDestroy(custSelPolOfTheMappedRecord);
	                                }
	                            
	                            CMDBField<TUint32>* inputFieldPtr = reinterpret_cast<CMDBField<TUint32>*>(iElement.GetAttribPtr(inputEntry->iOffset));
	                            if (inputFieldPtr)
	                                {
	                                inputFieldPtr->SetL(KCDTIdIAPRecord | mappedIAPRecId);
	                                Sync(*inputFieldPtr);
	                                }
                            	}
                            }
                        }
                    }
                }
            }
        }
    }

//this method compared the current mapped APPrioritySelPol record with the one in the database which is the default
TBool CCDMapperIAPPrioritySelectionPolicyRecord::IsItTheDefaultAPPrioritySelPolRec(/*CCDAPPrioritySelectionPolicyRecord* aDefAPPrioritySelPolRec*/)
    {
    TInt defSelPolLinkValue = CommsDatMapperAndValidator::GetDefAPPrioritySelPolLinkValueL(iSession.iOwner);
    
    TInt myId = ElementId() & ~KCDMaskShowFieldType;
    
    if (defSelPolLinkValue == myId)
        {
        return ETrue;
        }
    return EFalse;
    }

/*virtual*/ void CCDMapperIAPPrioritySelectionPolicyRecord::PostMappingsL()
    {        
	if ( KCDTIdRecordTag != iElement.TypeId() &&
	     KCDTIdRecordName != iElement.TypeId() &&
	     KCDTIdIapCount != iElement.TypeId() )
		{
	    TBool def = IsItTheDefaultAPPrioritySelPolRec();
	    CMDBRecordLink<CCDAccessPointRecord>* apXField = &iAp1;
	
	    CCDAPPrioritySelectionPolicyRecord* origMappedRecord = NULL;
	    CMDBRecordLink<CCDAccessPointRecord>* origApXField = NULL;
	        
	    if (origMappedDataInDB)
	    	{
	    	origMappedRecord = static_cast<CCDAPPrioritySelectionPolicyRecord*>(origMappedDataInDB);
	    	origApXField = &(origMappedRecord->iAp1);
	    	}
	    
	    TInt lastValidAPLinkNum = 0;
	    /*
	     * We have to go through the list because it's possible that the client modified an IAP link in
	     * the IAPPriorirytSelPol record and didn't update the IAPCount field. This loop here goes through the
	     * AP links in the mapped APPrioritySelPol table and counts the valid links and updates the APCount field.
	     * It's important since it's used during the selection.
	     * we have to go through all of the links because it's possible that there are 'holes' in the links.
	     * iap1 = valid link
	     * iap2 = 0
	     * iap3 = valid link
	     * In this special case the apCount will be 3 meaning that there are valid links in the first 3 slots.
	     */
	    for (TInt i = 1; /*(*apXField != NULL) &&*/ (i<=MaxLinkCount); ++i)
	        {        
	        if ( (EDelete != iOperation) &&
	        	 (0 != *apXField) )
	        	{
		        CommsDatMapperAndValidator::TAPTypes apType;
		        apType = CheckLinkLevelAPL(*apXField);
		        
		        if (CommsDatMapperAndValidator::ELinkLevel == apType)
		        //The IPProto AP has to be generated...
		            {
		            GenerateIPProtoAPL(apXField, i, def);
		            }
		        else if (CommsDatMapperAndValidator::EIPProtoLevel == apType)
		        //this link already points to an IPProto level AP check whether it has the correct linking 
		        //to the link level AP
		            {
		            CheckIPProtoAPL(apXField, i, def);
		            }
		        else
		        //hm... Something is really bad here. Leaving...
		            {
		            User::Leave(KErrNotFound);
		            }
		        
		        lastValidAPLinkNum = i;
	        	
	        	}
	        else
	        	{
	        	if (KCDMaskShowFieldType == (ElementId() & KCDMaskShowFieldType))
	        		{
		        	//the given field is NULL. Is this NULL a result of a modification or was it originally NULL? If it's 
		        	//NULL because of a modification (user set the field to 0) this means that we have to delete the IPProto
		        	//level AP linked from the mapped APPrioritySelPol record.
	        		
	                /* There is a little trick here. If there are 2 APPrioritySelPol records in the DB where
	                 * 1 link in each record is linking against the same IPProto AP then the situation 
	                 * can be the following:
	                 * APPrioritySelPolRec_1:
	                 * 		AP1 = IPProto_link_record1
	                 * 
	                 * APPrioritySelPolRec_2:
	                 * 		AP1 = IPProto_link_record1
	                 * 
	                 * One APPrioritySelPol record can be already deleted (by the MaybeDeleteNodeL call which was made
	                 * previously to this postmappings call).
	                 * So when the program logic tries to find that how many reference does a given
	                 * IPProto AP have (IPProto_link_record1 in the example above) then it finds that it's only 1 because the
	                 * other record from which it was linked the 2nd time is already deleted. That's why
	                 * we give for the search algorithm the original (already deleted) APPrioritySelPol record.
	                 */ 
	        		
		        	if (origApXField && (*origApXField) && origMappedRecord )
		        		{
		        		//originally this was not NULL. So we have to delete the linked IPProto level AP.
		        		//do a deep copy from the original APPrioritySelPol record
		        		CCDAPPrioritySelectionPolicyRecord* origSelPolRecCopy = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
		        		CleanupStack::PushL(origSelPolRecCopy);
		        		
		                TMetaVTableIterator inputAttribIter(origMappedRecord);
		                SVDataTableEntry const* inputEntry = NULL;
		                
		                TMetaVTableIterator copiedAttribIter(origSelPolRecCopy);
		                SVDataTableEntry const* copiedEntry = NULL;
		                
		                TInt fieldCounter = CCDAPPrioritySelectionPolicyRecord::EMaxNrOfAps;
		                
		                while ( (inputEntry = inputAttribIter++)    != NULL &&
		                		(copiedEntry = copiedAttribIter++)    != NULL &&
		                        fieldCounter-- > 0 )
		                    {
		                    //loop for the fields
		                    CMDBField<TUint32>* inputFieldPtr = reinterpret_cast<CMDBField<TUint32>*>(origMappedRecord->GetAttribPtr(inputEntry->iOffset));
		                    if ( inputFieldPtr )
		                        {
		                        CMDBField<TUint32>* copyFieldPtr = reinterpret_cast<CMDBField<TUint32>*>(origSelPolRecCopy->GetAttribPtr(copiedEntry->iOffset));                        
		                        copyFieldPtr->SetL(*inputFieldPtr);
		                        }
		                    }
		                
		                origSelPolRecCopy->iApCount.SetL(origMappedRecord->iApCount);
		                
		                //we transfer the ownership of this pointer!!!!!!
		                
		                CleanupStack::Pop(origSelPolRecCopy);
		                
		        		DeleteIPProtoAPL(origApXField, origSelPolRecCopy);
		        		
		        		lastValidAPLinkNum = ForcedCountAPLinks();
		        		}
	        		}
	        	else
	        		{
	        		//deleting a field
	        		//in the PreMappings function the elementId of the field to be deleted was changed by turning on the
	        		//changed flag on it. In this step we can exemine this flag. If found the corresponding IPProto level
	        		//AP should be deleted.
	        		if (Changed(*apXField))
	        			{
	        			DeleteIPProtoAPL(origApXField);
	        			
	        			lastValidAPLinkNum = ForcedCountAPLinks();
	        			}
	        		}
		        }
		        ++apXField;
		        
		        if (origMappedRecord)
		        	{
		        	++origApXField;
		        	}
	        }
	    
	    TBool countChanged = EFalse;
	    
	    if  (iElement.TypeId() == (KCDTIdIapPrioritySelectionPolicyRecord | KCDMaskShowFieldType))
	        {
	        CCDIAPPrioritySelectionPolicyRecord& inputElement = static_cast<CCDIAPPrioritySelectionPolicyRecord&>(iElement);
	        if (KCDChangedFlag == ((inputElement.iIapCount).ElementId() & KCDChangedFlag))
	        	{
	        	//the client has changed the iap count field, don't overwrite it
	        	countChanged = ETrue;
	        	}
	        }
	    
    	if ( ( ( KCDMaskShowFieldType == (ElementId() & KCDMaskShowFieldType) &&
    		     EDelete != iOperation ) || 
    		   ( 0 != (ElementId() & KCDMaskShowFieldType) ) ) &&
    		static_cast<TInt>(iApCount) != lastValidAPLinkNum &&
    		!countChanged )
    		{
    		//   ( if it's not a record delete _OR_ 
    		//     it's a simple field operation)  _AND_
    		//   the original link number is not the same as we counted _AND_
    		//   the user isn't modifying explicitly the link number
    		//then we update the record count field
	        iApCount.SetL(lastValidAPLinkNum);
	        this->ModifyL(iSession.iOwner);
    		}
	    
/*	    if (EDelete != aOperation &&
	    	!countChanged &&
	    	static_cast<TInt>(iApCount) != lastValidAPLinkNum)
	    	{
	    	//modify the APCount field. It's important since it's used during the selection process in ESock.
	        iApCount.SetL(lastValidAPLinkNum);
	        this->ModifyL(iSession.iOwner);
	    	}*/
	    
	    /* final check: 
	     * if the default IAPPrioritySelPol (mapped APPrioritySelPol) OR
	     * the 1st link form the default  IAPPrioritySelPol (mapped APPrioritySelPol)
	     * was deleted then the default IPProto AP (from the meshpreface file)
	     * should be linked from the APPrioritySelPol and the IPProto tier records.
	     */
	    if ( (EDelete == iOperation ||
	    	  0 == iApCount ) &&
	    	 def &&
	    	 ( KCDMaskShowFieldType == (ElementId() & KCDMaskShowFieldType) ||
	    	   KCDTIdAp1 == (ElementId() & KCDMaskShowType) ) )
	    	{
	    	//generate/link the def IPProto AP
	    	//1. Is DefIPProto AP in the database
	    	
	    	CCDAccessPointRecord* defIpprotoAP = NULL;
	    	
	    	if (CommsDatMapperAndValidator::IsDefIPProtoAPInDB(iSession.iOwner))
	    		{
	    		//yes, there is -> use it
	    		defIpprotoAP = CommsDatMapperAndValidator::GetTheDefIPProtoAP(iSession.iOwner);
	    		CleanupStack::PushL(defIpprotoAP);
	    		}
	    	else
	    		{
	    		//no there isn't -> generate one
	    		defIpprotoAP = CommsDatMapperAndValidator::GenerateIPProtoAPL(IPProtoBaseTagId, 
					                                                          CCDAccessPointRecord::KNoPolicy,
	    		                                                              iSession.iOwner);
	    		CleanupStack::PushL(defIpprotoAP);
	    		
	    		defIpprotoAP->StoreL(iSession.iOwner);
	    		}
	    	
	    	TPtrC recName(defIpprotoAP->iRecordName);
	    	if (0 != recName.Compare(KDefaultIPProtoRecName))
	    		{
	    		//the name of the selected default IPProto AP is not 'IPProtoDefault'.
	    		defIpprotoAP->iRecordName.SetMaxLengthL(KDefaultIPProtoRecName().Length());
	    		defIpprotoAP->iRecordName.SetL(KDefaultIPProtoRecName);
	    		
	    		defIpprotoAP->ModifyL(iSession.iOwner);
	    		}
	    	
	    	//2. correct the link in the ipproto Tier record to point to the default IPProto AP
	    	TInt ipproroTierRecId = CommsDatMapperAndValidator::GetIPProtoTierRecordIdL(iSession.iOwner);
	    	
	    	//CMDBRecordLink<CCDAccessPointRecord>* defTierAP = new(ELeave)CMDBRecordLink<CCDAccessPointRecord>(KCDTIdDefaultAccessPoint);
	    	CMDBField<TInt>* defTierAP = new(ELeave)CMDBField<TInt>(KCDTIdDefaultAccessPoint);
	    	CleanupStack::PushL(defTierAP);
	    	
	    	defTierAP->SetRecordId(ipproroTierRecId);
	    	defTierAP->LoadL(iSession.iOwner);
	    	
		    TMDBElementId elemId = defIpprotoAP->ElementId();
		    elemId &= KCDMaskHideAttrAndRes;
		    TMDBElementId tableAndRecordBitMask = KCDMaskShowRecordType | KCDMaskShowRecordId;
		    elemId &= tableAndRecordBitMask;
	    	
		    *defTierAP = elemId;
		    
		    defTierAP->ModifyL(iSession.iOwner);
		    
	    	CleanupStack::PopAndDestroy(defTierAP);
	    	CleanupStack::PopAndDestroy(defIpprotoAP);
	    	
	    	}
		}
    }

TInt CCDMapperIAPPrioritySelectionPolicyRecord::ForcedCountAPLinks()
	{
	TInt linkNum = 15;
	
	/* Walk backwards on the AP links and return the first valid AP link.
	 * This is the APCount number.
	 */
	CMDBRecordLink<CCDAccessPointRecord>* currAPLink = &(this->iAp15);
	
	for (; linkNum > 0; --linkNum)
		{
		if ( currAPLink &&
			 *currAPLink )
			{
			break;
			}
		--currAPLink;
		}
	
	return linkNum;
	}

void CCDMapperIAPPrioritySelectionPolicyRecord::CheckIPProtoAPL(CMDBRecordLink<CCDAccessPointRecord>*& aAPPriorityField,
                                                                TInt aLinkNumber,
                                                                TBool aIsDefaultAP)
    {
    TInt linkAPTagId = 0;
    
    if ( KCDMaskShowFieldType == (iElement.ElementId() & KCDMaskShowFieldType) )
    	{
	    //it's a record mapping
	    CCDIAPPrioritySelectionPolicyRecord* ptrIAPPrioritySelPol = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(&iElement);
	    linkAPTagId = GetTheLinkAPTagIdL(ptrIAPPrioritySelPol, aLinkNumber);
    	}
    else
    	{
    	//it's field mapping
    	if (Changed(*aAPPriorityField))
    		{
    		CMDBRecordLink<CCDIAPRecord>* ptrIAPPrioritySelPol = static_cast<CMDBRecordLink<CCDIAPRecord>*>(&iElement);
    		linkAPTagId = GetTheLinkAPTagIdL(ptrIAPPrioritySelPol);
    		}
    	else
    		{
    		return;
    		}
    	}
    
    //got the original elementID of the linked IAP record
    
    //read the CustomSelectionPolicy field of the IPProto level AP
    CMDBField<TInt>* custSelPolField = new(ELeave) CMDBField<TInt>(*aAPPriorityField | KCDTIdCustomSelectionPolicy);
    CleanupStack::PushL(custSelPolField);
    
    custSelPolField->LoadL(iSession.iOwner);
    
    if (linkAPTagId != *custSelPolField)
    //The IPProto AP has a wrong linking to the Link level AP. Modify it...
        {
        *custSelPolField = linkAPTagId;
        
        custSelPolField->ModifyL(iSession.iOwner);
        }
    
    //if this is the default APPrioritySelPol record and the linkNumber is 1 then 
    //update the defaultAccessPoint link in the network Tier record
    if (aIsDefaultAP/* &&
        1 == aLinkNumber*/)
        {
        CommsDatMapperAndValidator::ModifyDefaultTierRecordL((custSelPolField->ElementId() & ~KCDMaskShowFieldType) & ~KCDNotNullFlag, 
                                                             iSession.iOwner);
        }
    
    CleanupStack::PopAndDestroy(custSelPolField);
    }

void CCDMapperIAPPrioritySelectionPolicyRecord::GenerateIPProtoAPL(CMDBRecordLink<CCDAccessPointRecord>*& aAPPriorityFieldToModify,
                                                                   TInt aLinkNumber,
                                                                   TBool aIsDefaultAP)

    {    
    TInt linkAPTagId = 0;
        
    if ( KCDMaskShowFieldType == (iElement.ElementId() & KCDMaskShowFieldType) )
    	{
	    //it's a record mapping
	    CCDIAPPrioritySelectionPolicyRecord* ptrIAPPrioritySelPol = static_cast<CCDIAPPrioritySelectionPolicyRecord*>(&iElement);
	    linkAPTagId = GetTheLinkAPTagIdL(ptrIAPPrioritySelPol, aLinkNumber);
    	}
    else
    	{
    	//it's field mapping
    	if (Changed(*aAPPriorityFieldToModify))
    		{
    		CMDBRecordLink<CCDIAPRecord>* ptrIAPPrioritySelPol = static_cast<CMDBRecordLink<CCDIAPRecord>*>(&iElement);
    		linkAPTagId = GetTheLinkAPTagIdL(ptrIAPPrioritySelPol);
    		}
    	else
    		{
    		//the given field is not changed -> don't touch anything...
    		return;
    		}
    	}
    
    CCDAccessPointRecord* ipProtoAP = CommsDatMapperAndValidator::LoadIPProtoAccessPoint(linkAPTagId, iSession.iOwner);
    
    if (!ipProtoAP)
    	{
    	//The IPProto AP is not existing yet. Generate one.
        ipProtoAP = CommsDatMapperAndValidator::GenerateIPProtoAPL(IPProtoBaseTagId, linkAPTagId, iSession.iOwner);
	    CleanupStack::PushL(ipProtoAP);
	    
	    //save the generated AP record
	    ipProtoAP->StoreL(iSession.iOwner);
    	}
    else
    	{
    	//The IPProto AP is already exsiting. Use that one.
    	CleanupStack::PushL(ipProtoAP);
    	}
    
    TMDBElementId elemId = ipProtoAP->ElementId();
    elemId &= KCDMaskHideAttrAndRes;
    TMDBElementId tableAndRecordBitMask = KCDMaskShowRecordType | KCDMaskShowRecordId;
    elemId &= tableAndRecordBitMask;
    
    //a) update the APPrioritySelPol record to point to the newly creeated AP record
    *aAPPriorityFieldToModify = elemId;
    
    this->ModifyL(iSession.iOwner);
    
    //b) if this is the default APPrioritySelPol record and the linkNumber is 1 then 
    //   update the defaultAccessPoint link in the network Tier record
    if (aIsDefaultAP/* &&
        1 == aLinkNumber*/)
        {
        CommsDatMapperAndValidator::ModifyDefaultTierRecordL(elemId, iSession.iOwner);
        }
    
    CleanupStack::PopAndDestroy(ipProtoAP);
    }

void CCDMapperIAPPrioritySelectionPolicyRecord::DeleteIPProtoAPL(CMDBRecordLink<CCDAccessPointRecord>*& aAPPriorityField)
	{
	TInt refCount = CommsDatMapperAndValidator::CountReferenceToThisIPProtoAPL(*aAPPriorityField, iSession.iOwner);
	if (1 == refCount)
		{
		//ok, we have only 1 reference to the given IPProto AP -> it can be deleted
		CCDAccessPointRecord* apRecordToBeDeleted = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(*aAPPriorityField));
		CleanupStack::PushL(apRecordToBeDeleted);
		
		apRecordToBeDeleted->DeleteL(iSession.iOwner);
		
		CleanupStack::PopAndDestroy(apRecordToBeDeleted);
		}
	}

//Note - the ownership of the 'aTheAlreadyDeletedRecord' is transferred here!!!!!!
void CCDMapperIAPPrioritySelectionPolicyRecord::DeleteIPProtoAPL(CMDBRecordLink<CCDAccessPointRecord>*& aAPPriorityField,
									 							 CCDAPPrioritySelectionPolicyRecord* aTheAlreadyDeletedRecord)
	{
	//Note - the ownership of the 'aTheAlreadyDeletedRecord' is transferred to the CountReferenceToThisIPProtoAPL!!!!!!
	TInt refCount = CommsDatMapperAndValidator::CountReferenceToThisIPProtoAPL(*aAPPriorityField, aTheAlreadyDeletedRecord, iSession.iOwner);
	if (1 == refCount)
		{
		//ok, we have only 1 reference to the given IPProto AP -> it can be deleted
		CCDAccessPointRecord* apRecordToBeDeleted = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(*aAPPriorityField));
		CleanupStack::PushL(apRecordToBeDeleted);
		
		apRecordToBeDeleted->DeleteL(iSession.iOwner);
		
		CleanupStack::PopAndDestroy(apRecordToBeDeleted);
		}
	}

TInt CCDMapperIAPPrioritySelectionPolicyRecord::GetTheLinkAPTagIdL(CCDIAPPrioritySelectionPolicyRecord* aOrigIAPPrioritySelPolRec,
                                                                   TInt aLinkNumber)
    {
    //TInt iapCount = aOrigIAPPrioritySelPolRec->iIapCount;
    CMDBRecordLink<CCDIAPRecord>* iapXField = &aOrigIAPPrioritySelPolRec->iIap1;
    CMDBField<TInt>* apTagId = NULL;
    TInt tagId = 0;
    
    /*for (TInt i = 1; i!=aLinkNumber && i<=iapCount; ++i)
        {
        ++iapXField;
        }*/
    
    for (TInt i = 1; i!=aLinkNumber && i<=MaxLinkCount; ++i)
    	{
    	++iapXField;
    	}
    
    //got the original elementID of the linked IAP record
    
    
    //read the tagId of the generated Link level AP (if there is any)...
    
    apTagId = new(ELeave) CMDBField<TInt>(KCDTIdAccessPointRecord | KCDTIdRecordTag);
    CleanupStack::PushL(apTagId);
    
    
    if ( ( (*iapXField) & KCDMaskShowType ) == 0 &&
         ( (*iapXField) & KLinkableTag ) == 0 )
    	{
         //there is only a number for the IAP link in the table...
    	*apTagId = *iapXField;
    	}
    else
    	{
    	//the IAP link is correctly set. Let's use the recordId from the value field.
    	*apTagId = (*iapXField & KCDMaskShowRecordId) >> 8;
    	}
    
    if ( !(apTagId->FindL(iSession.iOwner)) )
    //the link level AP cannot be found. Leaving...
        {
        User::Leave(KErrNotFound);
        }
    else
        {
        tagId = *apTagId;
        }
    
    CleanupStack::PopAndDestroy(apTagId);
    
    return tagId;
    }


TInt CCDMapperIAPPrioritySelectionPolicyRecord::GetTheLinkAPTagIdL(CMDBRecordLink<CCDIAPRecord>* aOrigIAPPrioritySelPolField/*,
                                                                   TInt aLinkNumber*/)
    {
    CMDBField<TInt>* apTagId = NULL;
    TInt tagId = 0;
    //read the tagId of the generated Link level AP (if there is any)...
    
    apTagId = new(ELeave) CMDBField<TInt>(KCDTIdAccessPointRecord | KCDTIdRecordTag);
    CleanupStack::PushL(apTagId);
    
    
    if ( ( (*aOrigIAPPrioritySelPolField) & KCDMaskShowType ) == 0 &&
         ( (*aOrigIAPPrioritySelPolField) & KLinkableTag ) == 0 )
    	{
         //there is only a number for the IAP link in the table...
    	*apTagId = *aOrigIAPPrioritySelPolField;
    	}
    else
    	{
    	//the IAP link is correctly set. Let's use the recordId from the value field.
    	*apTagId = (*aOrigIAPPrioritySelPolField & KCDMaskShowRecordId) >> 8;
    	}
    
    if ( !(apTagId->FindL(iSession.iOwner)) )
    //the link level AP cannot be found. Leaving...
        {
        User::Leave(KErrNotFound);
        }
    else
        {
        tagId = *apTagId;
        }
    
    CleanupStack::PopAndDestroy(apTagId);
    
    return tagId;
    }

CommsDatMapperAndValidator::TAPTypes CCDMapperIAPPrioritySelectionPolicyRecord::CheckLinkLevelAPL(TMDBElementId aElementId)
//Assumption: an AP record always has a Tier field. 
    {
    CommsDatMapperAndValidator::TAPTypes ret = CommsDatMapperAndValidator::EUndefined;
    
    CMDBRecordLink<CCDTierRecord>* tierField = new(ELeave) CMDBRecordLink<CCDTierRecord>(aElementId | KCDTIdTier);
    CleanupStack::PushL(tierField);
    
    tierField->LoadL(iSession.iOwner);
    
    CMDBField<TInt>* tiertagId = new(ELeave) CMDBField<TInt>(*tierField | KCDTIdRecordTag);
    CleanupStack::PushL(tiertagId);
    
    tiertagId->LoadL(iSession.iOwner);
    
    TInt tagId = *tiertagId;
    
    switch (tagId)
        {
        case CommsDatMapperAndValidator::ENetworkLevelTierId :
            {
            ret = CommsDatMapperAndValidator::ENetworkLevel;
            break;
            }
            case CommsDatMapperAndValidator::EIPProtoTierId :
            {
            ret = CommsDatMapperAndValidator::EIPProtoLevel;
            break;
            }
            case CommsDatMapperAndValidator::ELinkTierId :
            {
            ret = CommsDatMapperAndValidator::ELinkLevel;
            break;
            }
            default:
            {
            User::Leave(KErrNotFound);
            }
        };
    
    
    CleanupStack::PopAndDestroy(tiertagId);
    CleanupStack::PopAndDestroy(tierField);
    
    return ret;
    }

/************************* Mapper for AccessPoint Table ***************************************
 * KCDTIdAccessPointRecord      CCDAccessPointRecord
 *
 * The following fields are deprecated
 *   APGID                  CCDAccessPointRecord::iAccessPointGID
 * 
 * They map to new fields within the AccessPointRecord
 *   APTier                 CCDAccessPointRecord::iTier
 *   
 * The following fields have some deprecated usage which is mapped 
 * though the same field remains
 *   APSelectionPolicy      CCDAccessPointRecord::iSelectionPolicy
 * 
 * 
 */

#define KCDTIdMapperAccessPointRecord    9881
START_ATTRIBUTE_TABLE( CCDMapperAccessPointRecord, KCDTIdMapperAccessPointRecord, KCDTIdMapperAccessPointRecord)
 	X_REGISTER_ATTRIBUTE( CCDMapperAccessPointRecord, iTier,            TMDBLinkNum)
    X_REGISTER_ATTRIBUTE( CCDMapperAccessPointRecord, iMCpr,            TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDMapperAccessPointRecord, iCpr,             TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDMapperAccessPointRecord, iSCpr,            TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDMapperAccessPointRecord, iProtocol,        TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDMapperAccessPointRecord, iCprConfig,       TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDMapperAccessPointRecord, iAppSID,          TMDBNum)
    X_REGISTER_ATTRIBUTE( CCDMapperAccessPointRecord, iConfigAPIdList,  TMDBText)
    X_REGISTER_ATTRIBUTE( CCDMapperAccessPointRecord, iSelectionPolicy, TMDBNum)
    X_REGISTER_ATTRIBUTE( CCDMapperAccessPointRecord, iRecordTag, 		TMDBNum)
END_ATTRIBUTE_TABLE()

CCDMapperAccessPointRecord::CCDMapperAccessPointRecord(CMDBElement& aElement,CMDBSessionImpl& aSession, const TOperation& aOperation)
  : CCDMapper(aElement.ElementId(), aElement, aSession, aOperation),
    iTier(KCDTIdTier),
	iMCpr(KCDTIdMCpr),
	iCpr(KCDTIdCpr),
	iSCpr(KCDTIdSCpr),
	iProtocol(KCDTIdProtocol),
    iCprConfig(KCDTIdCprConfig),
    iAppSID(KCDTIdAppSID),
	iConfigAPIdList(KCDTIdConfigAPIdList),
	iSelectionPolicy(KCDTIdSelectionPolicy),
	iRecordTag(KCDTIdRecordTag | KCDTIdAccessPointRecord)
    {}    

void CCDMapperAccessPointRecord::ConstructL()
/*
Initialise the mapper element for any read or write operation
Only do the minimum work here as the call might be LoadL

@internalTechnology
*/
    {   
    // Will either be mappng the whole record or just the GID field

    CMDBField<TUint32>* theGIDField = NULL;
    
    SetRecordId(iElement.RecordId());
    
    if(CommsDat::CommsDatSchema::IsRecord(iElement.ElementId()))
        {
        CCDAccessPointRecord& inputRecord = static_cast<CCDAccessPointRecord&>(iElement);

        theGIDField = static_cast<CMDBField<TUint32>*>(&inputRecord.iAccessPointGID);
        
        if (!inputRecord.iSelectionPolicy.IsNull())
            {
            //create the link which will be valid at the time of the insertion of the IAPPrioritySelPol
            //record
            iSelectionPolicy.SetL(KCDTIdApPrioritySelectionPolicyRecord | inputRecord.iSelectionPolicy & KCDMaskShowRecordId);
            }

        // These elements are quite likely to be NULL
        if (!inputRecord.iTier.IsNull())
            {
            iTier.SetL(inputRecord.iTier);
            }
        if (!inputRecord.iMCpr.IsNull())
            {
            iMCpr.SetL(inputRecord.iMCpr);
            }
        if (!inputRecord.iCpr.IsNull())
            {
            iCpr.SetL(inputRecord.iCpr);
            }
        if (!inputRecord.iSCpr.IsNull())
            {
            iSCpr.SetL(inputRecord.iSCpr);
            }
        if (!inputRecord.iProtocol.IsNull())
            {
            iProtocol.SetL(inputRecord.iProtocol);
            }
        if (!inputRecord.iCprConfig.IsNull())
            {
            iCprConfig.SetL(inputRecord.iCprConfig);
            }
        if (!inputRecord.iAppSID.IsNull())
            {
            iAppSID.SetL(inputRecord.iAppSID);
            }
        if (!inputRecord.iConfigAPIdList.IsNull())
            {
            iConfigAPIdList.SetL(inputRecord.iConfigAPIdList);
            }
        }
    else
        {            	
        if ( (iElement.ElementId() & KCDMaskShowType) == KCDTIdAccessPointGID )
        	{
        	theGIDField = static_cast<CMDBField<TUint32>*>(&iElement);
        	}
        else
        	{
        	//selectionPolicy field...
	        this->iSelectionPolicy.SetL(KCDTIdApPrioritySelectionPolicyRecord | static_cast<CMDBField<TUint32>&>(iElement) & KCDMaskShowRecordId);
	        this->iSelectionPolicy.SetRecordId(iElement.RecordId());
        	}
        }

    // Don't do validation - leave that til later.  Just copy the fields
    if (theGIDField &&
    	0 != (*theGIDField) &&
    	iTier.IsNull())
        {
        if (Changed(*theGIDField))
            {  
            iTier.SetL(*theGIDField);
            }
        }      
    }    

void CCDMapperAccessPointRecord::PreMappingsL()
	{
	if (EStore == iOperation)
		{
		// 2.  Map the fields
	    if ( iMCpr.IsNull() )
	        {
	        // this would indicate the mapper was incomplete
	        // not trying to validate away completely pathological case where scattered fields are set
	        
	        //load the template AP for the Network layer form the BearerType table
	        _LIT(KTemplateRecName,"NetworkDefaultTemplate");
	        
	        CCDBearerTypeRecord* templateForTheAPRec = static_cast<CCDBearerTypeRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdBearerTypeRecord));
	        CleanupStack::PushL(templateForTheAPRec);
	        
	        templateForTheAPRec->iRecordName.SetMaxLengthL(KTemplateRecName().Length());
	        templateForTheAPRec->iRecordName = KTemplateRecName();
	            
	        if (!templateForTheAPRec->FindL(iSession.iOwner))
	            {
	            User::Leave(KErrNotFound);
	            }
	        
	        // Copy the fields from the template record. The recordId will be the same as the one of the 
	        //input record
	        iTier.SetL(templateForTheAPRec->iTier);
	        iMCpr.SetL(templateForTheAPRec->iMCpr);
	        iCpr.SetL(templateForTheAPRec->iCpr);
	        iSCpr.SetL(templateForTheAPRec->iSCpr);
	        iProtocol.SetL(templateForTheAPRec->iProtocol);
	        //the SelectionPolicy field was set in the ConstructL function...
	        
	        CCDAccessPointRecord& inputRecord = static_cast<CCDAccessPointRecord&>(iElement);
	        if (inputRecord.iRecordTag.IsNull() )
	        //APs have to have tagIds....
	            {
	            iRecordTag = CommsDatMapperAndValidator::GenerateTagIdL(NetworkBaseTagId, iSession.iOwner);
	            }
	        
	        CleanupStack::PopAndDestroy(templateForTheAPRec);
	        }     
		}
	else if (EDelete == iOperation)
		{
		if (KCDMaskShowFieldType != (iElement.ElementId() & KCDMaskShowFieldType))
			{
			//field mapping -> change the elementId of the whole mapper object so the MaybeDeleteNodeL will delete
			//only the given field and not the whole mapper.
			if (KCDTIdSelectionPolicy == (iElement.ElementId() & KCDMaskShowType))
				{
				SetElementId(ElementId() & ~KCDMaskShowFieldType);
				SetElementId(ElementId() | KCDTIdSelectionPolicy);
				}
			else
				{
				//the GID field is mapped into the Tier field
				SetElementId(ElementId() | KCDTIdTier);
				}
			}
		}
	}
    

void CCDMapperAccessPointRecord::PrepareToStoreL()
/*
Prepare the mapper for a write operation

Create any new node(s) that may be required.  
Sometimes more than one mapper node might be needed for one input node.

The mapper element will have been populated from the input element already, 
but now complete the population where necessary following the unique set of 
rules for this class

validate the information (only where valid data is critical)

The element can be populated by using information from both the input element and the database

@internalTechnology
*/
    {     
    // Create new node if necessary (only one new node in this case) and only when the input element is a record
    iSession.MaybeCreateNodeL((CMDBElement&)*this);
    if ( (iElement.ElementId() & KCDNewRecordRequest == KCDNewRecordRequest) &&
    	 (iElement.ElementId() & KCDMaskShowFieldType) ==  KCDMaskShowFieldType )
    	{
    	iElement.SetRecordId(this->RecordId());
    	}

    // Check that the tier matches the default tier (leave if it doesn't)
    CMDBField<TUint32>* theGIDField = NULL;
        
    if(CommsDat::CommsDatSchema::IsRecord(iElement.ElementId()))
        {
        CCDAccessPointRecord& inputRecord = static_cast<CCDAccessPointRecord&>(iElement);

        theGIDField = static_cast<CMDBField<TUint32>*>(&inputRecord.iAccessPointGID);
        }
    else
        {
        if ( (iElement.ElementId() & KCDMaskShowType) == KCDTIdAccessPointGID )
        	{
        	theGIDField = static_cast<CMDBField<TUint32>*>(&iElement);
        	}
        }
    
    // 1. lookup defaultTier record from global settings and check it matches.
    if (NULL == theGIDField ||
    	0 == *theGIDField)
    	{
    	//GID field is not filled in the record so let's try to read the tier field in the mapped AP with the same
    	//recordId as the one from the client...
    	
    	//tier link in the AP record
    	CMDBRecordLink<CCDTierRecord>* apTierField = new(ELeave) CMDBRecordLink<CCDTierRecord>(KCDTIdTier);
    	CleanupStack::PushL(apTierField);
    	apTierField->SetRecordId(iElement.RecordId());
    	
    	//tier link in the GS record
    	CMDBRecordLink<CCDTierRecord>* gsTierField = new(ELeave) CMDBRecordLink<CCDTierRecord>(KCDTIdDefaultTier);
    	CleanupStack::PushL(gsTierField);
    	gsTierField->SetRecordId(1);
    	
    	apTierField->LoadL(iSession.iOwner);
    	gsTierField->LoadL(iSession.iOwner);
    	
    	if ( *gsTierField != ((*apTierField & KCDMaskShowRecordId) >> 8) )
    		{
    		//this is not a network level AP -> leaving
    		User::Leave(KErrArgument);
    		}
    	CleanupStack::PopAndDestroy(gsTierField);
    	CleanupStack::PopAndDestroy(apTierField);
    	}
    else
    	{
    	CheckIsSnapL(*theGIDField); // returned default tier id is not used here
    	}
    }    



TBool CCDMapperAccessPointRecord::NeedsMapping(CMDBElement& aElement)
// NB it is correct that the fields listed here are different from the fields in the Mapped() function 
    {
    if( CommsDat::CommsDatSchema::IsRecord(aElement.ElementId()) )
        {
        CCDAccessPointRecord& ap = static_cast<CCDAccessPointRecord&>(aElement);
        if (ap.iTier.IsNull() )
            {
            // either reading into an empty record or storing and haven't set the tier so map
            return ETrue; 
            }

        return EFalse;
        }
        
        
    if( (aElement.ElementId() & KCDMaskShowType) == KCDTIdAccessPointGID ||
    	(aElement.ElementId() & KCDMaskShowType) == KCDTIdSelectionPolicy )
        {
        return ETrue;
        }

    return EFalse;
    }


TBool CCDMapperAccessPointRecord::Mapped(TMDBElementId aElementId)
// Lists the fields in this record that will be handled by the mapper not the caller
    {
    TInt fieldTypeId = aElementId & KCDMaskShowType;

    switch(fieldTypeId)
		{
 		case KCDTIdAccessPointGID :
        case KCDTIdTier :
        case KCDTIdMCpr :
        case KCDTIdCpr :
        case KCDTIdSCpr :
        case KCDTIdProtocol :
        case KCDTIdCprConfig :
        case KCDTIdAppSID :
        case KCDTIdConfigAPIdList :
		case KCDTIdSelectionPolicy :
   		   {
           return ETrue;
	       }
        }
	return EFalse;
    }

//Delete the previously stored selectionPolicy field...
/*virtual*/ void CCDMapperAccessPointRecord::FinalPreparationsL()
    {
    CMDBRecordLink<CCDSelectionPolicyRecordBase>* selectionPolicy = new(ELeave) CMDBRecordLink<CCDSelectionPolicyRecordBase>(KCDTIdSelectionPolicy);
    selectionPolicy->SetRecordId(RecordId());
    
    selectionPolicy->DeleteL(iSession.iOwner);
    }


TMDBElementId CCDMapper::CheckIsSnapL(TMDBElementId aInputId)
    {
    // first find the default tier record
    CMDBRecordLink<CCDTierRecord> *tierLinkField = new(ELeave) CMDBRecordLink<CCDTierRecord>(KCDTIdGlobalSettingsRecord | KCDTIdDefaultTier | KCDGlobalSettingsRecordId); 
    CleanupStack::PushL(tierLinkField);
    
    tierLinkField->LoadL(iSession.iOwner);
    
    TMDBElementId defaultTierId = (tierLinkField->iLinkedRecord->ElementId()) & KCDMaskShowRecordTypeAndId;
    
    // now prime the same container with aId and check we get to the same tier record id
    tierLinkField->SetL(aInputId);

    TMDBElementId resolvedInputId = CommsDatSchema::GetLinkIdL(iSession.iOwner, tierLinkField->ElementId(), tierLinkField->GetL(), KCDTIdTierRecord/*, NULL*/);

    if (defaultTierId != resolvedInputId)
        {
        __FLOG_STATIC3(KLogComponent, KCDErrLog,
                       _L("CCDMapper::CheckIsSnapL() - iGID <%08x> resolves to <%08x> not to the default (SNAP) Tier <%08x>"),
                       aInputId, resolvedInputId, defaultTierId);
      
        User::Leave(KErrArgument);
        }
   
    CleanupStack::PopAndDestroy(tierLinkField); 
    
    return defaultTierId;
    }



void CCDMapperAccessPointRecord::MapResultsL()
/*
Map results to client element following mapping rules

only the GID and SelPol field need mapping

@internalComponent
*/
    {    
    CMDBField<TUint32>* gidInputElement = NULL;
    CMDBRecordLink<CCDSelectionPolicyRecordBase>* selectionPolicy;
    
    if(CommsDat::CommsDatSchema::IsRecord(iElement.ElementId()))
        {
        CCDAccessPointRecord& inputRecord = static_cast<CCDAccessPointRecord&>(iElement);
        gidInputElement = static_cast<CMDBField<TUint32>*>(&inputRecord.iAccessPointGID);
        selectionPolicy = static_cast<CMDBRecordLink<CCDSelectionPolicyRecordBase>*>(&inputRecord.iSelectionPolicy);
        
        TInt mappedLinkValue = static_cast<TInt>(iSelectionPolicy);
        if (mappedLinkValue)
        	{
        	*selectionPolicy = KCDTIdIapPrioritySelectionPolicyRecord | (mappedLinkValue & KCDMaskShowRecordId);
        	}
        }
    else
        {
        if( (iElement.ElementId() & KCDMaskShowType) == KCDTIdAccessPointGID)
        	{
        	gidInputElement=static_cast<CMDBField<TUint32>*>(&iElement);
        	}
        else
        	{
        	//selection policy field mapping
        	TInt selPolVal = static_cast<TInt>(iSelectionPolicy);
        	if (selPolVal)
        		{
	        	CMDBField<TInt>* inputSelPolField = static_cast<CMDBField<TInt>*>(&iElement);
	        	inputSelPolField->SetL(KCDTIdIapPrioritySelectionPolicyRecord | selPolVal & KCDMaskShowRecordId);
	        	Sync(*inputSelPolField);
        		}
        	}
        }
    
    // Tier may not be default, but it's still ok to copy tier id to GID 
    // Though remember only default tier can be stored via a mapped access point
    if ( (CommsDat::CommsDatSchema::IsRecord(iElement.ElementId())) ||
    	 ((iElement.ElementId() & KCDMaskShowType) == KCDTIdAccessPointGID) &&
    	!iTier.IsNull() )
        {
        gidInputElement->SetL(iTier);
        Sync(*gidInputElement);
        }
    }


#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY


		
//EOF













