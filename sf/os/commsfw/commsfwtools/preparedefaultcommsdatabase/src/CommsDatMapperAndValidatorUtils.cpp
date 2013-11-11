// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CommsDatMapperAndValidatorUtils.h
// 
//

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY


#include "CommsDatMapperAndValidatorUtils.h"

#include <metadatabase.h>
#include <commsdattypesv1_1.h>

#include <commsdattypesv1_1_partner.h>

#include "CommsDatInternalDefs.h"
#include "MetaDatabaseVisitor.h"
#include <commsdatutils.h>
#include <comms-infras/commsdatschema.h>

using namespace Meta;
using namespace CommsDatInternal;
using namespace CommsDat;

TInt CommsDatMapperAndValidator::GenerateTagIdL(TInt aBaseTagId, CMDBSession& aSession)
    {
    TInt tagId = aBaseTagId;
    CMDBField<TInt>* possibleTagId = new(ELeave) CMDBField<TInt>(KCDTIdAccessPointRecord | KCDTIdRecordTag);
    CleanupStack::PushL(possibleTagId);
    *possibleTagId = tagId;
    
    //as we don't know anything about the AccessPoints in the AccessPointTable we have to try the 
    //uniqueness of the given tagId... 
    while ( possibleTagId->FindL(aSession) )
        {
        *possibleTagId = ++tagId;
    
        }
    CleanupStack::PopAndDestroy(possibleTagId);
    return tagId;
    }

void CommsDatMapperAndValidator::GenerateRecordName(TDes& aBuffer, TInt aUniqueNumber)
    {
    aBuffer.Append(KIPProtoTag());
    
    TBuf<MAX_BUFFER_LEN_FOR_NAMING> buf;
    buf.Format(_L("%D"), aUniqueNumber);
    
    aBuffer.Append(buf);
    }

TInt CommsDatMapperAndValidator::GetDefAPPrioritySelPolLinkValueL(CMDBSession& aSession)
    {
    // Search for the default Tier in the GlobalSettings Record 
    // (The Global Settings record has to be stored before inserting 
    // the ConnectionPreferences record!!!!).
    CMDBRecordLink<CCDTierRecord>* defaultTierField = new(ELeave) CMDBRecordLink<CCDTierRecord>(KCDTIdGlobalSettingsRecord | KCDTIdDefaultTier);
    defaultTierField->SetRecordId(1);
    CleanupStack::PushL(defaultTierField);
    
    // this field HAS to be in the GlobalSettings table as we are in 399 config...
    defaultTierField->LoadL(aSession);
    
    // Follow the defaultAccessPoint link in the defaultTier record which points to
    // the default AP record on the Network level.
    TInt recId = (*defaultTierField) << 8;
    CMDBRecordLink<CCDAccessPointRecord>* networkDefAPField = 
                    new(ELeave) CMDBRecordLink<CCDAccessPointRecord>(recId |
                                                                     KCDTIdDefaultAccessPoint);
    CleanupStack::PushL(networkDefAPField);
    
    networkDefAPField->LoadL(aSession);
    
    //Follow the link from the default AP record to the linked APPrioritySelPol record.
    CMDBRecordLink<CCDAPPrioritySelectionPolicyRecord>* networkSelPolField = 
                    new(ELeave) CMDBRecordLink<CCDAPPrioritySelectionPolicyRecord>(((*networkDefAPField) & KCDMaskShowRecordId) |
                                                                                   KCDTIdSelectionPolicy);
    CleanupStack::PushL(networkSelPolField);
    
    networkSelPolField->LoadL(aSession);
    
    TInt defSelPolLinkValue = *networkSelPolField;
    
    CleanupStack::PopAndDestroy(networkSelPolField);
    CleanupStack::PopAndDestroy(networkDefAPField);
    CleanupStack::PopAndDestroy(defaultTierField);
    
    return defSelPolLinkValue;
    }

CCDAPPrioritySelectionPolicyRecord* CommsDatMapperAndValidator::FindTheAPPrioritySelPolRecL(CMDBSession& aSession)
    {
    TInt defSelPolLinkValue = GetDefAPPrioritySelPolLinkValueL(aSession);
    //at this point we already know the complete elementId of the default APPrioritySelPol record.
    //Read the whole record because it will be needed by the IPProto AP generation logic...
    
    CCDAPPrioritySelectionPolicyRecord* defAPSelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
                    
    CleanupStack::PushL(defAPSelPolRec);
    
    defAPSelPolRec->SetElementId(defSelPolLinkValue);
    defAPSelPolRec->LoadL(aSession);
    
    //we will transfer the ownership of this object to the caller method
    CleanupStack::Pop(defAPSelPolRec);
    
    return defAPSelPolRec;
    }

CCDAccessPointRecord* CommsDatMapperAndValidator::GenerateIPProtoAPL(TInt aTagGenerationBase, 
                                                                     TInt aLinkLevelAPTagId,
                                                                     CMDBSession& aSession)
    {
    CCDAccessPointRecord* apRecordToBeCreated = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(apRecordToBeCreated);
    
    CCDBearerTypeRecord* templateForTheAPRec = static_cast<CCDBearerTypeRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdBearerTypeRecord));
    CleanupStack::PushL(templateForTheAPRec);
    
    templateForTheAPRec->iRecordName.SetMaxLengthL(KIPProtoTemplateRecName().Length());
    templateForTheAPRec->iRecordName = KIPProtoTemplateRecName();
        
    if (!templateForTheAPRec->FindL(aSession))
        {
        User::Leave(KErrNotFound);
        }
    
    //buffer for the record name
    TBuf<KMaxName> recordName;
    
    //as the IAP and AP records are inserted before it won't cause 
    //any problem to use the 'KCDNewRecordRequest' for the record IDs.
    apRecordToBeCreated->SetRecordId(KCDNewRecordRequest);
    
    apRecordToBeCreated->iRecordTag = GenerateTagIdL(aTagGenerationBase, aSession);
	
	GenerateRecordName(recordName, apRecordToBeCreated->iRecordTag);
	
	apRecordToBeCreated->iRecordName.SetMaxLengthL(recordName.Length());
    apRecordToBeCreated->iRecordName = recordName;
    apRecordToBeCreated->iSelectionPolicy = 0;
	apRecordToBeCreated->iTier.SetL(templateForTheAPRec->iTier);
	apRecordToBeCreated->iMCpr.SetL(templateForTheAPRec->iMCpr);
	apRecordToBeCreated->iCpr.SetL(templateForTheAPRec->iCpr);
	apRecordToBeCreated->iSCpr.SetL(templateForTheAPRec->iSCpr);
	apRecordToBeCreated->iProtocol.SetL(templateForTheAPRec->iProtocol);
	apRecordToBeCreated->iCustomSelectionPolicy.SetL(aLinkLevelAPTagId);
    apRecordToBeCreated->iCprConfig.SetL(aLinkLevelAPTagId);
    
    CleanupStack::PopAndDestroy(templateForTheAPRec);
    
    //the ownership will be transferred
    CleanupStack::Pop(apRecordToBeCreated);
    
    return apRecordToBeCreated;
    }

/**
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    The basic assumption here is that the ConnectionPreferences table (if there is one in the config 
    file) and the IAPPrioritySelectionPolicy table are consistent. This means that if the ConnPref table
    contains ranking2 IAP too then the IAPPrioritySelPol table contanins the same number of the IAP
    links. So here in this method the only cases which are handled are that this record contains 1 or 2
    links.
    
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
void CommsDatMapperAndValidator::ModifyAPPrioritySelPolRecL(CMDBRecordLink<CCDAccessPointRecord>*& aFieldInRecord,
                                                           CCDAPPrioritySelectionPolicyRecord*& aAPSelPolRec,
                                                           TInt aAPElementId)
    {
    //the link update  
    *aFieldInRecord = aAPElementId;
    
    //The APCount update
    TMetaVTableIterator attribIter(aAPSelPolRec);
    SVDataTableEntry const* entry = 0;
    TInt fieldCounter(1);
    CMDBRecordLink<CCDAccessPointRecord>* apPrioritySelPolLink = NULL;
    
    while ( (entry = attribIter++) != NULL && 
            fieldCounter <= 2 )
        {
        apPrioritySelPolLink = reinterpret_cast<CMDBRecordLink<CCDAccessPointRecord>*>(aAPSelPolRec->GetAttribPtr(entry->iOffset));
        ++fieldCounter;        
        }
    
    if (0 != (*apPrioritySelPolLink))
    	{
    	//the 2nd AP link is filled in the APPrioritySelPol rec -> the APCount is 2
    	aAPSelPolRec->iApCount.SetL(MaxNumberOfLinks);
    	}
    else
    	{
    	aAPSelPolRec->iApCount.SetL(1);
    	}
    
    }

void CommsDatMapperAndValidator::CheckTheAPPrioritySelPolFromConnPrefModL(CCDAPPrioritySelectionPolicyRecord*& aAPSelPolRec)
    {
    //TBool modified = EFalse;
    
    TMetaVTableIterator attribIter(aAPSelPolRec);
    SVDataTableEntry const* entry = 0;
    TInt fieldCounter(1);
    CMDBRecordLink<CCDAccessPointRecord>* apPrioritySelPolLink = NULL;
    
    while ( (entry = attribIter++) != NULL && 
            fieldCounter <= 15 )
        {
        apPrioritySelPolLink = reinterpret_cast<CMDBRecordLink<CCDAccessPointRecord>*>(aAPSelPolRec->GetAttribPtr(entry->iOffset));
        
        if (apPrioritySelPolLink &&
            *apPrioritySelPolLink &&
            fieldCounter > MaxNumberOfLinks)
            //This branch is executed if there is inconsistency between the ConnPref and the IAPPrioritySelPol
            //table. Now we are processing a ConnectionPreference table and the default APPrioritySelPol record
            //contains more the 2 AP links. This means that earlier an IAPPrioritySelPol record was converted
            //to this APPrioritySelPol record (meaning a SNAP configuration) and that record contains more than
            //2 IAP links. At the same time the configuration contains a ConnPref table too which can contain
            //maximum 2 IAP links. So here the previosly stored APPrioritySelPol record will be modified to
            //contain only 2 AP links - according to the ConnPref table.
            {
            /**
                the link is lost.
                e.g. if it was an AP3 link, which is not valid in the default APPriotySelPol record, it will be 0ed.
            */
            *apPrioritySelPolLink = 0;
            __FLOG_STATIC0(KLogComponent, KCDInfoLog, _L("CommsDatMapperAndValidator::CheckTheAPPrioritySelPolFromConnPrefMod - inconsistency between the ConnPref and IAPPrioritySelPol table!!"));
            aAPSelPolRec->iApCount.SetL(MaxNumberOfLinks);
            //modified = ETrue;
            }
            
        ++fieldCounter;
        
        }
    //return modified;
    }

//this method will read and modify (storing the modified record)    
void CommsDatMapperAndValidator::ModifyDefaultTierRecordL(TInt aAPElementId, 
                                                          CommsDat::CMDBSession& aSession)
    {
    CCDTierRecord* ipProtoTierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
    CleanupStack::PushL(ipProtoTierRec);
    
    ipProtoTierRec->SetRecordId(GetIPProtoTierRecordIdL(aSession));
    
    ipProtoTierRec->LoadL(aSession);
    
    ipProtoTierRec->iDefaultAccessPoint.SetL(aAPElementId);
    
    //save the modified IPProto Tier Record
    ipProtoTierRec->ModifyL(aSession);
        
    CleanupStack::PopAndDestroy(ipProtoTierRec);
    }

//this function returns the record id of the network tier record
TInt CommsDatMapperAndValidator::GetNetworkTierRecordIdL(CommsDat::CMDBSession& aSession)
	{
	CMDBField<TInt>* networkTierTag = new(ELeave)CMDBField<TInt>(KCDTIdTierRecord | KCDTIdRecordTag);
	CleanupStack::PushL(networkTierTag);
	
	*networkTierTag = ENetworkLevelTierId;
	
	TInt recId = 0;
	if(networkTierTag->FindL(aSession))
		{
		recId = networkTierTag->RecordId();
		}
	
	CleanupStack::PopAndDestroy(networkTierTag);
	
	return recId;
	}

TInt CommsDatMapperAndValidator::GetIPProtoTierRecordIdL(CommsDat::CMDBSession& aSession)
	{
	CMDBField<TInt>* ipprotoTierTag = new(ELeave)CMDBField<TInt>(KCDTIdTierRecord | KCDTIdRecordTag);
	CleanupStack::PushL(ipprotoTierTag);
	
	*ipprotoTierTag = EIPProtoTierId;
	
	TInt recId = 0;
	if(ipprotoTierTag->FindL(aSession))
		{
		recId = ipprotoTierTag->RecordId();
		}
	
	CleanupStack::PopAndDestroy(ipprotoTierTag);
	
	return recId;
	}


/*
Populates an access point record with the first matching IP Proto Access Point record.

@param aAccessPoint An access point record that will be populated with data from the
                    first matching record. This can have fields primed in order to
                    make the match more specific
@param aSession     The commsdat session to use  
@return             ETrue if a matching IP Proto access point is found. EFalse if it is not.
@exception          KErrCorrupt if the IP Proto Tier entry is missing from the database.
                    It can leave with other errors from other parts of commsdat/cenrep
*/
TBool CommsDatMapperAndValidator::FindIPProtoAccessPointRecordL(CommsDat::CCDAccessPointRecord& aAccessPoint, CommsDat::CMDBSession& aSession)
    {
    // Find the IP Proto tier record - we can then use this information to more efficently find the ip proto access points
    CMDBField<TInt> tierRecord;
    tierRecord.SetElementId(KCDTIdTierRecord | KCDTIdRecordTag);
    tierRecord.SetL(EIPProtoTierId);
    TBool found = tierRecord.FindL(aSession);
    if (!found)
        {
        __FLOG_STATIC0(KLogComponent, KCDInfoLog, _L("CommsDatMapperAndValidator::FindIPProtoAccessPointRecordL - ERROR: IP Proto Tier entry missing!!"));
        User::Leave(KErrCorrupt);
        }

    TMDBElementId tierElement; // This is where the AP record will link if it is an IP Proto AP
    tierElement = tierRecord.ElementId() & (KCDMaskShowType | KCDMaskShowRecordId);
    
    aAccessPoint.iTier = tierElement;
    found = aAccessPoint.FindL(aSession);
    
    return found;
    }

TBool CommsDatMapperAndValidator::IsIPProtoAPAlreadyExistL(TInt aLinkLevelTagId,
														  CommsDat::CMDBSession& aSession)
	{
    CCDAccessPointRecord* ap = (CCDAccessPointRecord*)CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord);
    CleanupStack::PushL(ap);
    
    ap->iCustomSelectionPolicy = aLinkLevelTagId;
    
    TBool found = FindIPProtoAccessPointRecordL(*ap, aSession);
	CleanupStack::PopAndDestroy(ap);
    
	return found;
	}

TBool CommsDatMapperAndValidator::IsIPProtoAPAlreadyExistL(TInt aLinkLevelTagId, 
														   TInt aConnPrefElementId, 
														   CommsDat::CMDBSession& aSession)
	{
    CCDAccessPointRecord* ap = (CCDAccessPointRecord*)CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord);
    CleanupStack::PushL(ap);
    
    ap->iCprConfig = aConnPrefElementId;
    ap->iCustomSelectionPolicy = aLinkLevelTagId;

    TBool found = FindIPProtoAccessPointRecordL(*ap, aSession);
    CleanupStack::PopAndDestroy(ap);
    
    return found;
	}


CommsDat::CCDAccessPointRecord* CommsDatMapperAndValidator::LoadIPProtoAccessPoint(TInt aLinkLevelTagId,
										   							   CommsDat::CMDBSession& aSession)
	{
    CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(apRec);
    
    apRec->iCustomSelectionPolicy = aLinkLevelTagId;
    
    if (!FindIPProtoAccessPointRecordL(*apRec, aSession))
        {
        CleanupStack::PopAndDestroy(apRec);
        return NULL;
        }
    
    CleanupStack::Pop(apRec); // ownership of the AP record is given to the caller
    return apRec;
	}


/* All of the APPrioritySelectionPolicy fields are exemined checking that how many
 * fields, in different APPrioritySelPol records, are referencing the given AP
 * record. The recordSet->FindL with a primed record is not suitable here because
 * the link can occur in any field of the APPrioritySelPol record.
 */
TInt CommsDatMapperAndValidator::CountReferenceToThisIPProtoAPL(TUint aElementId, CommsDat::CMDBSession& aSession)
	{
	TInt refCount = 0;
	
	CMDBRecordSet<CCDAPPrioritySelectionPolicyRecord>* apprioritySelPolRecSet = new(ELeave)CMDBRecordSet<CCDAPPrioritySelectionPolicyRecord>(KCDTIdApPrioritySelectionPolicyRecord);
	CleanupStack::PushL(apprioritySelPolRecSet);
	
	//do the hard work and read all of the appriorityselpol recs...
	apprioritySelPolRecSet->LoadL(aSession);
	
	TInt recCount = apprioritySelPolRecSet->iRecords.Count();
	
	CCDAPPrioritySelectionPolicyRecord* curAPPrioritySelPolRec = NULL;

	TInt fieldCounter = 0;
    
	for (TInt i = 0; i < recCount; ++i)
		{
		//loop for the records
		curAPPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(apprioritySelPolRecSet->iRecords[i]);
		
        TMetaVTableIterator inputAttribIter(curAPPrioritySelPolRec);
        SVDataTableEntry const* inputEntry = NULL;
        
        fieldCounter = CCDAPPrioritySelectionPolicyRecord::EMaxNrOfAps;
        
        while ( (inputEntry = inputAttribIter++)    != NULL &&
                fieldCounter-- > 0 )
            {
            //loop for the fields
            CMDBField<TUint32>* inputFieldPtr = reinterpret_cast<CMDBField<TUint32>*>(curAPPrioritySelPolRec->GetAttribPtr(inputEntry->iOffset));
            if ( inputFieldPtr && 
            	 aElementId == *inputFieldPtr )
                {
                ++refCount;
                }
            }
		}
	
	CleanupStack::PopAndDestroy(apprioritySelPolRecSet);
	
	return refCount;
	}

//Note - the ownership of the 'aTheAlreadyDeletedRecord' is transferred here!!!!!!
TInt CommsDatMapperAndValidator::CountReferenceToThisIPProtoAPL(TUint aElementId, 
										   						CommsDat::CCDAPPrioritySelectionPolicyRecord* aTheAlreadyDeletedRecord,
										   						CommsDat::CMDBSession& aSession)
	{
	CleanupStack::PushL(aTheAlreadyDeletedRecord);
	
	TInt refCount = 0;
		
	CMDBRecordSet<CCDAPPrioritySelectionPolicyRecord>* apprioritySelPolRecSet = new(ELeave)CMDBRecordSet<CCDAPPrioritySelectionPolicyRecord>(KCDTIdApPrioritySelectionPolicyRecord);
	CleanupStack::PushL(apprioritySelPolRecSet);
	
	//do the hard work and read all of the appriorityselpol recs...
	apprioritySelPolRecSet->LoadL(aSession);
	
	//and add the already deleted APPrioritySelPol record to the list
	//note that the ownership of the pointer is now at the recordset!!!!!
	apprioritySelPolRecSet->iRecords.AppendL(aTheAlreadyDeletedRecord);
	
	/* here as the ownership of the 'aTheAlreadyDeletedRecord' pointer is transferred
	 * to the recordset so we have to pop it from the CleanupStack. BUT there is already
	 * another pointer 'apprioritySelPolRecSet'. So in order to pop the 'aTheAlreadyDeletedRecord'
	 * we have to pop the 'apprioritySelPolRecSet' too and push it back again...
	 */
	CleanupStack::Pop(apprioritySelPolRecSet);
	CleanupStack::Pop(aTheAlreadyDeletedRecord);
	
	CleanupStack::PushL(apprioritySelPolRecSet);
	
	TInt recCount = apprioritySelPolRecSet->iRecords.Count();
	
	CCDAPPrioritySelectionPolicyRecord* curAPPrioritySelPolRec = NULL;

	TInt fieldCounter = 0;
    
	for (TInt i = 0; i < recCount; ++i)
		{
		//loop for the records
		curAPPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(apprioritySelPolRecSet->iRecords[i]);
		
        TMetaVTableIterator inputAttribIter(curAPPrioritySelPolRec);
        SVDataTableEntry const* inputEntry = NULL;
        
        fieldCounter = CCDAPPrioritySelectionPolicyRecord::EMaxNrOfAps;
        
        while ( (inputEntry = inputAttribIter++)    != NULL &&
                fieldCounter-- > 0 )
            {
            //loop for the fields
            CMDBField<TUint32>* inputFieldPtr = reinterpret_cast<CMDBField<TUint32>*>(curAPPrioritySelPolRec->GetAttribPtr(inputEntry->iOffset));
            if ( inputFieldPtr && 
            	 aElementId == *inputFieldPtr )
                {
                ++refCount;
                }
            }
		}
	
	CleanupStack::PopAndDestroy(apprioritySelPolRecSet);
	
	return refCount;
	}


TBool CommsDatMapperAndValidator::IsIAPRecInDB(CommsDat::CMDBSession& aSession)
	{
	TBool ret = EFalse;
	
	CMDBRecordSet<CCDIAPRecord>* iapRecSet = new(ELeave)CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRecSet);
	
	TRAPD(err, iapRecSet->LoadL(aSession));
	
	if (KErrNone == err &&
		iapRecSet->iRecords.Count() > 0)
		{
		ret = ETrue;
		}
	
	CleanupStack::PopAndDestroy(iapRecSet);
	
	return ret;
	}

TBool CommsDatMapperAndValidator::IsDefIPProtoAPInDB(CommsDat::CMDBSession& aSession)
	{
	TBool ret = EFalse;
	
	CMDBField<TInt>* customSelPolField = new(ELeave)CMDBField<TInt>(KCDTIdCustomSelectionPolicy);
	CleanupStack::PushL(customSelPolField);
	
	*customSelPolField = CCDAccessPointRecord::KNoPolicy;
	if (customSelPolField->FindL(aSession))
		{
		ret = ETrue;
		}
	
	CleanupStack::PopAndDestroy(customSelPolField);
	
	return ret;
	}

CommsDat::CCDAccessPointRecord* CommsDatMapperAndValidator::GetTheDefIPProtoAP(CommsDat::CMDBSession& aSession)
	{
    CCDAccessPointRecord* apRec = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
    CleanupStack::PushL(apRec);
	
    apRec->iCustomSelectionPolicy = CCDAccessPointRecord::KNoPolicy;
	
	if(!apRec->FindL(aSession))
		{
		//no records were found with the given customSelectionPolicy value
		User::Leave(KErrNotFound);
		}
	
	CleanupStack::Pop(apRec);
	
	return apRec;
	}

TInt CommsDatMapperAndValidator::GetNumberOfThisIPProtoAPL(CommsDat::CCDAccessPointRecord* aAPRecToBeDeleted, CommsDat::CMDBSession& aSession)
	{
	CMDBRecordSet<CCDAccessPointRecord>* apRecSet = new(ELeave)CMDBRecordSet<CCDAccessPointRecord>(KCDTIdAccessPointRecord);
	CleanupStack::PushL(apRecSet);
	
	CCDAccessPointRecord* apForPriming = static_cast<CCDAccessPointRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord));
	CleanupStack::PushL(apForPriming);
	
	apForPriming->iTier.SetL(aAPRecToBeDeleted->iTier);
	apForPriming->iCprConfig.SetL(aAPRecToBeDeleted->iCprConfig);
	apForPriming->iCustomSelectionPolicy.SetL(aAPRecToBeDeleted->iCustomSelectionPolicy);
	
	apRecSet->iRecords.AppendL(apForPriming);
	//ownership of the primed record is now at the recordset
	CleanupStack::Pop(apForPriming);
	
	TInt recCount=0;
	if(apRecSet->FindL(aSession))
		{
		recCount = apRecSet->iRecords.Count();
		}
	CleanupStack::PopAndDestroy(apRecSet);
	
	return recCount;
	}

void CommsDatMapperAndValidator::CorrectLinksL(TUint aElementIdToBeDeleted, CommsDat::CMDBSession& aSession)
	{
	//1st correct the apprioritySelPol rec
	TInt defSelPolElemId = GetDefAPPrioritySelPolLinkValueL(aSession);
	CCDAPPrioritySelectionPolicyRecord* defAPPrioritySelPolRec = static_cast<CCDAPPrioritySelectionPolicyRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord));
	CleanupStack::PushL(defAPPrioritySelPolRec);
	
	defAPPrioritySelPolRec->SetElementId(defSelPolElemId);
	defAPPrioritySelPolRec->LoadL(aSession);

	TInt fieldCounter(defAPPrioritySelPolRec->iApCount);
	
    TMetaVTableIterator inputAttribIter(defAPPrioritySelPolRec);
    SVDataTableEntry const* inputEntry = NULL;
    
    //this fieldpointer will point to the link in the APPrioritySelPol record
    //which will not be deleted.
    CMDBField<TUint32>* apLinkWhichWontBeDeleted = NULL;
    
    //as this call is from the connpref mapping logic we can assume that this record
    //contains only 2 links maximum (rank1, rank2)
    while ( (inputEntry = inputAttribIter++)    != NULL &&
            fieldCounter-- > 0 )
        {
        //loop for the fields
        apLinkWhichWontBeDeleted = reinterpret_cast<CMDBField<TUint32>*>(defAPPrioritySelPolRec->GetAttribPtr(inputEntry->iOffset));
        if ( apLinkWhichWontBeDeleted && 
        	 aElementIdToBeDeleted != *apLinkWhichWontBeDeleted )
            {
            //ok got the link to the AP which WON'T be deleted
            break;
            }
        }
	
	//modify the links in the APPrioritySelPol rec
    TMetaVTableIterator inputAttribIter1(defAPPrioritySelPolRec);
    SVDataTableEntry const* inputEntry1 = NULL;
    
    CMDBField<TUint32>* inputFieldPtr = NULL;
    
    fieldCounter = defAPPrioritySelPolRec->iApCount;
    
    //set all of the links to point the AP which won't be deleted
    while ( (inputEntry1 = inputAttribIter1++)    != NULL &&
            fieldCounter-- > 0 )
        {
        //loop for the fields
        inputFieldPtr = reinterpret_cast<CMDBField<TUint32>*>(defAPPrioritySelPolRec->GetAttribPtr(inputEntry1->iOffset));
        
        if ( inputFieldPtr &&
        	 *apLinkWhichWontBeDeleted != *inputFieldPtr )
        	{
    		//modify the link
    		inputFieldPtr->SetL(*apLinkWhichWontBeDeleted);
    		break;
            }
        }
	
    //now modify the ipproto Tier too
    TInt ipprotoTierRecId = GetIPProtoTierRecordIdL(aSession);
    CCDTierRecord* ipprotoTierRec = static_cast<CCDTierRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdTierRecord));
    CleanupStack::PushL(ipprotoTierRec);
	
    ipprotoTierRec->SetRecordId(ipprotoTierRecId);
    ipprotoTierRec->LoadL(aSession);
    ipprotoTierRec->iDefaultAccessPoint.SetL(*apLinkWhichWontBeDeleted);
    ipprotoTierRec->ModifyL(aSession);
    
    defAPPrioritySelPolRec->ModifyL(aSession);
    
    CleanupStack::PopAndDestroy(ipprotoTierRec);
    CleanupStack::PopAndDestroy(defAPPrioritySelPolRec);
	}


#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

