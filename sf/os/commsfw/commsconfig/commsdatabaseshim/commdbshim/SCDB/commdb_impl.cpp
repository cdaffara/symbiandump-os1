// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Comms Database Table View defintions
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include <cdbtemp.h>
#include <commdb.h>
#include <cdbpreftable.h>
#include <commsdat_partner.h>
#include <commsdattypesv1_1_partner.h>

#include "commdb_impl.H"
#include "Commsdat_Log.h"
#include "commsdattypeinfov1_1_internal.h"

const TInt KFirstRecord = 0;

// Additional funtions to implement Connection Preference table protection - in a separate
// class so as not to break BC.
CCommDbTableExtension::CCommDbTableExtension()
/**
Constructor
*/
    {
    }

CCommDbTableExtension* CCommDbTableExtension::NewL()
	{
	CCommDbTableExtension* self = new(ELeave) CCommDbTableExtension;
	TInt err = self->iCommitSeqProperty.Attach(KUidCommsDatStatusEvent, KCommsDatStatusEventCommitSeq);
	if(err != KErrNone)
		{
		delete self;
		User::Leave(err);
		}
	return self;
	}

void CCommDbTableExtension::GetRecordInfoL(const TDesC& aColumn, TDbColType& aColType, TUint32& aAttrib)
	{
	CMDBRecordSetBase* recSet = GetRecordSetL();
	
	TBuf<KCommsDbSvrMaxColumnNameLength> buf; // This holds Column name
	TBool found(EFalse);
	
	// create dummy record to read info from it
	CMDBRecordBase* dummyRecord = CCDRecordBase::RecordFactoryL(recSet->TypeId());
	const SRecordTypeInfo* recordInfo = dummyRecord->GetRecordInfo();

	// Loop through all the columns of the record
	while(recordInfo->iTypeId != 0 && !found)
		{
		// Get name of the field into buffer
		buf = recordInfo->iTypeName;
		
		if(buf.CompareF(aColumn) == 0)
			{
			found = ETrue;
			}
		else
			{
			// Switch to Next column
			recordInfo++;
			}
		}
	
	// check if column is not found
	if(!found)
		{
		delete dummyRecord;
		User::Leave(KErrNotFound);
		}
	else
		{
		aColType = GetColType(recordInfo->iValType);
		recordInfo->iTypeAttr == ENotNull ? aAttrib = TDbCol::ENotNull : aAttrib = 0;
		}
	
	delete dummyRecord;
	}

TDbColType CCommDbTableExtension::GetColType(TInt aType)
    {
    TDbColType ret;
    switch(aType)
        {
        case EText:
        case EMedText:
            ret = EDbColText16;
            break;
        case ELongText:
            ret = EDbColLongText;
            break;
        case EDesC8:
            ret = EDbColText8;
            break;
        case EUint32:
            ret = EDbColUint32;
            break;
        case EInt:
            ret = EDbColUint32;
            break;
        case EBool:
            ret = EDbColBit;
            break;
        default:
            ret = EDbColInt32;
        }

    return ret;
    }

TInt CCommDbTableExtension::GetMaxTypeLength(TInt aType)
    {
    TInt ret;

    switch(aType)
        {
        case EText:
            ret = KMaxTextLength;
            break;
        case EDesC8:
            ret = KMaxTextLength;
            break;
        case EUint32:
            ret = KMaxNumLength;
            break;
        case EInt:
            ret = KMaxNumLength;
            break;
        case EBool:
            ret = KMaxNumLength;
            break;
        case ELongText:
            ret = KMaxLongTextLength;
            break;
        case EMedText:
            ret = KMaxMedTextLength;
            break;
        default:
            ret = 0;
        }

    return ret;
    }

void CCommDbTableExtension::UpdateLastCommitSeq()
	{
	// Update the global commit sequence, for detecting possible changes when deciding whether to refresh records
	TInt currCommitSeq;
    if(iCommitSeqProperty.Get(currCommitSeq) == KErrNone)
		{
		iLastCommitSeq = currCommitSeq;
		}
	}

TInt CCommDbTableExtension::MaybeRefreshCurrentRecord(CMDBSession& aDbs)
	{
	// If the global commit sequence number differs from the last one set or observed by us then we need to refresh to ensure we
	// acquire any relevant changes
	TInt err = KErrNone;
	TInt currCommitSeq = iLastCommitSeq;
    if(iCommitSeqProperty.Get(currCommitSeq) != KErrNone || currCommitSeq != iLastCommitSeq)
		{
	    TRAP(err,iCurrentRecord->RefreshL(aDbs));
		if(err == KErrNone)
			{
			iLastCommitSeq = currCommitSeq;
			}
		}
	return err;
	}

TInt CCommDbTableExtension::GotoFirstRecord(CMDBSession& aDbs)
    {
	TInt err(KErrNotFound);
	
	TInt countContRec = iRecordSetContainer->iRecords.Count();
	
    if (iRecordSetContainer && countContRec)
        {
		// set up stating point
		iCurrentRecord = NULL;
		// set up first valid record
		TInt valid = FindFirstNextValidRecord(ETrue);

		if(valid)
			{
			// save current record before change
			CMDBRecordBase* currRecBackup(iCurrentRecord);

            if ( iCurrentRecord->RecordId() == KCDDefaultRecord  &&
                 (iCurrentRecord->Attributes() & ECDHidden ) &&
                 ! aDbs.IsSetAttributeMask(ECDHidden) )
                {
                err = GotoNextRecord(aDbs); 
                }
            else 
                {
			    err = MaybeRefreshCurrentRecord(aDbs);
				if (err != KErrNone) 
					{
					__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommDbTableExtension::GotoFirstRecord failed to Refresh the data Err=%d"),err);
					iCurrentRecord = currRecBackup;
					}
                }
			}

		iEndOfTableRecordBackup = NULL;
		iBeginingOfTableRecordBackup = NULL;
        }

    // record set is empty
    return err;
    }

TInt CCommDbTableExtension::GotoNextRecord(CMDBSession& aDbs)
    {
    TInt err(KErrNone);
	TInt currIndex(0);
	
	if (!iRecordSetContainer) 
		{
		return KErrNotFound;
		}
	
    if (iCurrentRecord)
        {
        TInt count = iRecordSetContainer->iRecords.Count();
		currIndex = iRecordSetContainer->iRecords.Find(iCurrentRecord);
        if (currIndex == count-1)
            {
            // record set is either empty or going out of scope
            err = KErrNotFound;
			// the marker is going beyond end of the table, so backup last record and use the value as end marker
			iEndOfTableRecordBackup = iCurrentRecord;
			iCurrentRecord = NULL;
            }
        else
            {
			// save current record before change
			CMDBRecordBase* currRecBackup(iCurrentRecord);
			// set up first next valid record
			TInt valid = FindFirstNextValidRecord(EFalse);
			if (valid) 
				{
			    err = MaybeRefreshCurrentRecord(aDbs);
				if (err != KErrNone) 
					{
					// return to status prior to failure
					iCurrentRecord = currRecBackup;
					__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommDbTableExtension::GotoNextRecord failed to Refresh the data Err=%d"),err);
					}
				}
			else
				{
				err = KErrNotFound;
				}
            }
        }
    else
    	{
		// if position was on the begining of the table then retrieve back first record and set up begining of table marker to null
        if (iBeginingOfTableRecordBackup)
			{
			iCurrentRecord = iBeginingOfTableRecordBackup;
			iBeginingOfTableRecordBackup = NULL;
			}
		else
	        {
	        err = KErrNotFound;
	        }
    	}

    return err;
    }

TInt CCommDbTableExtension::GotoPreviousRecord(CMDBSession& aDbs)
    {
    TInt err = KErrNone;

	if (!iRecordSetContainer) 
		{
		return KErrNotFound;
		}
	
    if (iCurrentRecord)
        {
        const TInt count = iRecordSetContainer->iRecords.Count();
		TInt currIndex = iRecordSetContainer->iRecords.Find(iCurrentRecord);
        if (currIndex == 0)
            {
            // record set is either empty or going out of scope
            err = KErrNotFound;
            }
        else
            {
			// save current record before change
			CMDBRecordBase* currRecBackup(iCurrentRecord);
			// set up first prev valid record
			TInt valid = FindFirstPrevValidRecord();
			if (valid) 
				{
			    err = MaybeRefreshCurrentRecord(aDbs);
				if (err != KErrNone) 
					{
					// return to status prior to failure
					iCurrentRecord = currRecBackup;
					__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommDbTableExtension::GotoPreviousRecord failed to Refresh the data Err=%d"),err);
					}
				}
			else
				{
				err = KErrNotFound;
				}
            }
        }
    else
        {
		// if position was on the end of the table then retrieve back last record and set up end of table marker to null
        if (iEndOfTableRecordBackup) 
			{
			iCurrentRecord = iEndOfTableRecordBackup;
			iEndOfTableRecordBackup = NULL;
			}
		else
			{
	        err = KErrNotFound;
			}
        }

    return err;
    }

void CCommDbTableExtension::InsertConnPrefRecordL(CCDConnectionPrefsRecord* aConnPref, CMDBSession& aDbs)
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommDbTableExtension::InsertConnPrefRecordL -> Entry Point"));

	CMDBRecordSetBase* recSet = GetRecordSetL();

	// store newly created record
	aConnPref->SetRecordId(KCDNewRecordRequest);

	aConnPref->StoreL(aDbs);
	User::LeaveIfError(recSet->iRecords.Append(aConnPref));
	
	iCurrentRecord = recSet->iRecords[recSet->iRecords.Count()-1];
	iInsertedRecords.InsertL(iCurrentRecord->RecordId(), KFirstRecord);
	
	TInt index = iDeletedRecords.Find(iCurrentRecord->RecordId());
	
	// check if inserted record has the same record id as previously deleted record	
	if(index>=0)
		{
		iDeletedRecords.Remove(index);
		}

	SortByRank();
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommDbTableExtension::InsertConnPrefRecordL -> Record Inserted"));
	}


TBool CCommDbTableExtension::IsGenericRecord(TMDBElementId aElementId) const
    {
    const TMDBElementId tableType = (aElementId & KCDMaskShowRecordType);
    return (tableType >= KCDInitialUDefRecordType);
    }

void CCommDbTableExtension::InsertRecordL(TBool aCopy, TUint32& aId, CMDBSession& aDbs, TBool aIsTemplate)
/**
Deletes the current record from commsdat and from queue.
Updates the value of record index.
*/
    {
	CMDBRecordSetBase* recSet = GetRecordSetL();
	// storing empty string in order to get RecordId for newly inserted record
	CMDBRecordBase* newRecord;
	
	if(aCopy)
		{
		CMDBRecordBase* record = GetCurrentRecordL();
		newRecord = CCDRecordBase::CreateCopyRecordL(*record);
		CleanupStack::PushL(newRecord);
		if(IsGenericRecord(newRecord->ElementId()))
			{
			newRecord->LoadL(aDbs);
			}		
		}
	else
		{
		newRecord = CCDRecordBase::RecordFactoryL(recSet->TypeId());
		CleanupStack::PushL(newRecord);
		if(IsGenericRecord(newRecord->ElementId()))
			{
			newRecord->LoadL(aDbs);
			}	
		}
	
    TBool removeHiddenMask = EFalse;
	if(aIsTemplate)
		{
		// this will mark record as template record
		newRecord->SetRecordId(KCDDefaultRecord);
        if (! aDbs.IsSetAttributeMask(ECDHidden))
            {
            aDbs.SetAttributeMask(ECDHidden);
            removeHiddenMask = ETrue;
            }
		}
	else
		{
		// this will give next valid record number
		newRecord->SetRecordId(KCDNewRecordRequest);		
		}
	
	// create and store record placeholder
	newRecord->StoreL(aDbs);

    if (removeHiddenMask)
        {
        aDbs.ClearAttributeMask(ECDHidden);
        }

	// check if inserted record has the same record id as previously deleted record
	TInt index = iDeletedRecords.Find(newRecord->RecordId());
	
	if(index >= 0)
		{
		iDeletedRecords.Remove(index);
		}

	// append inserted record
	User::LeaveIfError(recSet->iRecords.Append(newRecord));
	CleanupStack::Pop(newRecord);
	
	iCurrentRecord = newRecord;
	iInsertedRecords.InsertL(iCurrentRecord->RecordId(), KFirstRecord);
	SortById();
	
	aId = newRecord->RecordId();
	}

TInt CCommDbTableExtension::CancelRecordChanges(CMDBSession& aDbs)
	{
	TInt err = KErrNone;
	if (!iRecordSetContainer)
		{
		return KErrNotFound;
		}
	
	TRAP(err,iCurrentRecord->LoadL(aDbs));
	if (err != KErrNone) 
		{
		// return to status prior to failure
		__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommDbTableExtension::CancelRecordChanges failed to Refresh the data, Err=%d"),err);
		return err;
		}
	
	if (iInsertedRecords.Count()) 
		{
		TInt index = FindValueInRecSet(iInsertedRecords[KFirstRecord]);
		
		if(index >= 0)
			{
			delete iRecordSetContainer->iRecords[index];
			iRecordSetContainer->iRecords.Remove(index);
			iRecordSetContainer->iRecords.Compress();
			// reuse index variable
			index = iRecordSetContainer->iRecords.Count();
			index ? iCurrentRecord = iRecordSetContainer->iRecords[index-1] : iCurrentRecord = NULL;
			}
		}

	ResetArrays(aDbs);
	
	return err;
	}

void CCommDbTableExtension::ResetArrays(CMDBSession& aSession) 
	{
	if(!aSession.IsInTransaction())
		{
		iInsertedRecords.Reset();
		iDeletedRecords.Reset();
		}
	// else they will be reseted on Commit or Rolback Transaction
	}

void CCommDbTableExtension::ResetAndDestroyL() 
	{
	GetRecordSetL()->iRecords.ResetAndDestroy();
	iCurrentRecord = NULL;
	}

void CCommDbTableExtension::DeleteCurrentRecordL(CMDBSession& aDbs, TBool aContainerOnly)
/**
Deletes the current record from commsdat and from queue.
Updates the value of record index.
*/
    {
    // check for validity of current record through GetCurrentRecord call
    CMDBRecordBase* currRec = GetCurrentRecordL();
    /* TODO: Add iShowHidden in conditions. This might not be necessary checking */
    /*if(iRecordIndex == 0) 
        {
        // it is not possible to delete template record
        User::Leave(KErrNotSupported);
        }*/
	if(!aContainerOnly)
		{
		if (currRec->IsSetAttribute(ECDProtectedWrite))
    		{
    		User::Leave(KErrAccessDenied);
    		}
		
		User::LeaveIfError(iDeletedRecords.Append(currRec->RecordId()));
		
		__FLOG_STMT(TInt count = iDeletedRecords.Count();)
		__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CCommDbTableExtension::DeleteCurrentRecordL added to deleted record queue, as No:%d"), count);
    	
    	currRec->DeleteL(aDbs);
		}

	TInt currIndex = iRecordSetContainer->iRecords.Find(currRec);

	iCurrentRecord = NULL;
	
	// If we just deleted the last or first non-deleted record then update the appropriate "backup" so that a next/prev
	// will work
	TInt probeIndex = currIndex;
	iBeginingOfTableRecordBackup = NULL;
	iEndOfTableRecordBackup = NULL;
	if(!AdvanceIndexToPrevValidRecord(probeIndex))
		{
		// Deleted first record; Next from here should move to first record
		probeIndex = currIndex;
		if(AdvanceIndexToNextValidRecord(probeIndex))
			{
			iBeginingOfTableRecordBackup = iRecordSetContainer->iRecords[probeIndex];
			}
		}
	else
		{
		iBeginingOfTableRecordBackup = iRecordSetContainer->iRecords[probeIndex];
		}
	probeIndex = currIndex;
	if(!AdvanceIndexToNextValidRecord(probeIndex))
		{
		// Deleted last record; Prev from here should move to last record
		probeIndex = currIndex;
		if(AdvanceIndexToPrevValidRecord(probeIndex))
			{
			iEndOfTableRecordBackup = iRecordSetContainer->iRecords[probeIndex];
			}
		}
	else
		{
		iEndOfTableRecordBackup = iRecordSetContainer->iRecords[probeIndex];
		}

	if(!aContainerOnly)
		{
		// remove an entry from container
		delete iRecordSetContainer->iRecords[currIndex];
		iRecordSetContainer->iRecords.Remove(currIndex);
		iRecordSetContainer->iRecords.Compress();
		}
    }

void CCommDbTableExtension::SetRecordSet(CMDBRecordSetBase* aRecordSetContainer)
    {
    iRecordSetContainer = aRecordSetContainer;
	TInt currIndex = iRecordSetContainer->iRecords.Count();

	currIndex == 0 ? iCurrentRecord = NULL : iCurrentRecord = iRecordSetContainer->iRecords[0];
	
	#ifdef _DEBUG
       	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("Start of Table Contents"));
		CCDRecordBase* ptrTemp;
		CMDBRecordBase* currRec;
		CMDBRecordSetBase* recSet = static_cast<CMDBRecordSetBase*>(iRecordSetContainer);
		TInt count = recSet->iRecords.Count();
		for(TInt i=0;i<count;++i)
			{
	        currRec = recSet->iRecords[i];
			ptrTemp = static_cast<CCDRecordBase*>(currRec);
			const TDesC& field = ptrTemp->iRecordName;
			
			__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("Record[%d], RecordId:%d, RecordName:%S"),
				i, currRec->RecordId(), &field);
			}
       	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("End of Table Contents"));
	#endif
    };

CMDBRecordSetBase* CCommDbTableExtension::RecordSetFactoryL(const TDesC& aTableName, TBool preparePriming, CMDBSession& aDbs)
	{
	TMDBElementId recordSetId = 0;
	CMDBRecordSetBase* recordSetContainer = NULL;
	
    if (aTableName.CompareF(TPtrC(KCDTypeNameConnectionPrefs)) == 0)
        {
        // connection preferance needs to be created via CommDbConnectionPref class
        User::Leave(KErrNotSupported);
        }
	else if (aTableName.CompareF(TPtrC(KCDTypeNameGlobalSettings)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDGlobalSettingsRecord>(KCDTIdGlobalSettingsRecord);
		recordSetId = KCDTIdGlobalSettingsRecord;
		}
    else if (aTableName.CompareF(TPtrC(KCDTypeNameNetwork)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDNetworkRecord>(KCDTIdNetworkRecord);
        recordSetId = KCDTIdNetworkRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameLocation)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDLocationRecord>(KCDTIdLocationRecord);
        recordSetId = KCDTIdLocationRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameIAP)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
        recordSetId = KCDTIdIAPRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameWAPAccessPoint)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDWAPAccessPointRecord>(KCDTIdWAPAccessPointRecord);
        recordSetId = KCDTIdWAPAccessPointRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameDialOutISP)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDDialOutISPRecord>(KCDTIdDialOutISPRecord);
        recordSetId = KCDTIdDialOutISPRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameDialInISP)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDDialInISPRecord>(KCDTIdDialInISPRecord);
        recordSetId = KCDTIdDialInISPRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameLANService)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDLANServiceRecord>(KCDTIdLANServiceRecord);
        recordSetId = KCDTIdLANServiceRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameWLANServiceExt)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDWLANServiceExtRecord>(KCDTIdWLANServiceExtRecord);
        recordSetId = KCDTIdWLANServiceExtRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNamePANServiceExt)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDPANServiceExtRecord>(KCDTIdPANServiceExtRecord);
        recordSetId = KCDTIdPANServiceExtRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameVPNService)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDVPNServiceRecord>(KCDTIdVPNServiceRecord);
        recordSetId = KCDTIdVPNServiceRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameDefaultWCDMA)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDWCDMAPacketServiceRecord>(KCDTIdDefaultWCDMARecord);
        recordSetId = KCDTIdDefaultWCDMARecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameIncomingWCDMA)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDIncomingGprsRecord>(KCDTIdIncomingGprsRecord);
        recordSetId = KCDTIdIncomingGprsRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameOutgoingWCDMA)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDOutgoingGprsRecord>(KCDTIdOutgoingGprsRecord);
        recordSetId = KCDTIdOutgoingGprsRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameModemBearer)) == 0 || aTableName.CompareF(TPtrC(OLD_MODEM_TABLE)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);
        recordSetId = KCDTIdModemBearerRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameLANBearer)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDLANBearerRecord>(KCDTIdLANBearerRecord);
        recordSetId = KCDTIdLANBearerRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameVirtualBearer)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDVirtualBearerRecord>(KCDTIdVirtualBearerRecord);
        recordSetId = KCDTIdVirtualBearerRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameWAPSMSBearer)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDWAPSMSBearerRecord>(KCDTIdWAPSMSBearerRecord);
        recordSetId = KCDTIdWAPSMSBearerRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameWAPIPBearer)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDWAPIPBearerRecord>(KCDTIdWAPIPBearerRecord);
        recordSetId = KCDTIdWAPIPBearerRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameChargecard)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDChargecardRecord>(KCDTIdChargecardRecord);
        recordSetId = KCDTIdChargecardRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameProxies)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDProxiesRecord>(KCDTIdProxiesRecord);
        recordSetId = KCDTIdProxiesRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameSSProto)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDSecureSocketRecord>(KCDTIdSSProtoRecord);
        recordSetId = KCDTIdSSProtoRecord;
        }
    else if (aTableName.CompareF(TPtrC(KCDTypeNameAgentLookup)) == 0)
        {
		recordSetContainer = new (ELeave) CMDBRecordSet<CCDAgentLookupRecord>(KCDTIdAgentLookupRecord);
        recordSetId = KCDTIdAgentLookupRecord;
        }
    else
        {
        // Create a temporary example of the user defined record, in order to do the lookup by name and get the Id back.
        
        CMDBGenericRecord* tempUserDefinedRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(0));
        CleanupStack::PushL(tempUserDefinedRecord);
    	tempUserDefinedRecord->InitializeL(aTableName, NULL);
        tempUserDefinedRecord->LoadL(aDbs);

        // Get the Id that we're interested in...
        recordSetId = tempUserDefinedRecord->TableId();

        // ..and validate it.
        if ((recordSetId & KCDMaskShowRecordType) < KCDInitialUDefRecordType)
            {
            User::Leave(KErrNotFound);
            }
        
        
		recordSetContainer = new (ELeave) CMDBRecordSet<CMDBGenericRecord>(recordSetId);
		
		if (preparePriming)
			{
			CleanupStack::PushL(recordSetContainer);
			recordSetContainer->iRecords.AppendL(tempUserDefinedRecord);	
			CleanupStack::Pop(2);
			}
		else
			{
			CleanupStack::PopAndDestroy(tempUserDefinedRecord);
			}
		return recordSetContainer;			
        }

    if (preparePriming)
        {
		CleanupStack::PushL(recordSetContainer);
		CMDBRecordBase* record = CCDRecordBase::RecordFactoryL(recordSetId);
		TInt err = recordSetContainer->iRecords.Append(record);
		if(err != KErrNone)
			{
			delete record;
			User::Leave(err);
			}
		CleanupStack::Pop(recordSetContainer);
        }
    return recordSetContainer;
	}

void CCommDbTableExtension::SetRecordSetL(const TDesC& aTableName, CMDBSession& aDbs, TBool aPreparePriming, TBool aLoadTable, TBool aTemplate)
/**
Sets the value of the member iRecordSetContainer
*/
    {
    __FLOG_STATIC1(KLogComponent, KCommDbShim, _L("SetRecordSetL -> TableName=%S"), &aTableName);

    // delete existing container
    delete iRecordSetContainer;
    iCurrentRecord = NULL;
	
    TBool removeHiddenMask = EFalse;
	if (aTemplate) 
		{
		// keeping BC compatibility
		TRAPD(err, iRecordSetContainer = RecordSetFactoryL(aTableName, aPreparePriming, aDbs));
		if (err == KErrNotFound) 
			{
			err = KErrArgument;
			}
		User::LeaveIfError(err);
        iRecordSetContainer->iRecords[0]->SetAttributes(ECDHidden);
        if (! aDbs.IsSetAttributeMask(ECDHidden))
            {
            aDbs.SetAttributeMask(ECDHidden);
            removeHiddenMask = ETrue;
            }
		}
		else
		{
		iRecordSetContainer = RecordSetFactoryL(aTableName, aPreparePriming, aDbs);
		}
	
    // load data
    if (aLoadTable)
        {
        TRAPD(err, iRecordSetContainer->LoadL(aDbs));
        // if table is empty don't leave
        if (err != KErrNone && err != KErrNotFound)
        	{
        	User::LeaveIfError(err);
        	}
       	#ifdef _DEBUG
        else
        	{
        	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("Start of Table Contents"));
			CCDRecordBase* ptrTemp;
			CMDBRecordBase* currRec;
			CMDBRecordSetBase* recSet = static_cast<CMDBRecordSetBase*>(iRecordSetContainer);
			TInt count = recSet->iRecords.Count();
			for(TInt i=0;i<count;++i)
				{
		        currRec = recSet->iRecords[i];
				ptrTemp = static_cast<CCDRecordBase*>(currRec);
				const TDesC& field = ptrTemp->iRecordName;

				__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("Record[%d], RecordId:%d, RecordName:%S"),
					i, currRec->RecordId(), &field);
				}
			__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("End of Table Contents"));
        	}
		#endif

		UpdateLastCommitSeq();
        }

    if (removeHiddenMask)
        {
        aDbs.ClearAttributeMask(ECDHidden);
        }


    // reset the index
	TInt currIndex = iRecordSetContainer->iRecords.Count();
	currIndex ?  iCurrentRecord = iRecordSetContainer->iRecords[0] : iCurrentRecord = NULL;
    }


CMDBRecordSetBase* CCommDbTableExtension::GetRecordSetL()
/**
Gets the value of the member iRecordIndex

@return A wrapper for an SQL string combined with a text comparison mode.
*/	{
    if (!iRecordSetContainer)
        {
        User::Leave(KErrNotFound);
        }
    return iRecordSetContainer;
    }

TInt CCommDbTableExtension::GetRecordSet(CMDBRecordSetBase*& aRecSet)
    {
    TInt ret = KErrNone;
    if (!iRecordSetContainer)
        {
        ret = KErrNotFound;
        }
    else
        {
        aRecSet = iRecordSetContainer;
        }
    return ret;
    }

TInt CCommDbTableExtension::GetCurrentRecord(CMDBRecordBase*& aRecord)
    {
    TInt index(0);
    TInt err = GetRecordSetIndex(index);

    aRecord = 0;

    if (err == KErrNone)
        {
        aRecord = iRecordSetContainer->iRecords[index];
        err = KErrNone;
        }

    return err;
    }

CMDBRecordBase* CCommDbTableExtension::GetCurrentRecordL()
    {
    if(iCurrentRecord)
        {
        return iCurrentRecord;
        }

    User::Leave(KErrNotFound);

    return NULL;
    }

TInt CCommDbTableExtension::GetRecordSetIndex(TInt &aIndex)
/**
Gets the value of the member iRecordIndex

@return A wrapper for an SQL string combined with a text comparison mode.
*/
    {
    TInt err = KErrNotFound;

    if(iRecordSetContainer)
        {
        if (iCurrentRecord)
            {
            aIndex = iRecordSetContainer->iRecords.Find(iCurrentRecord);
            err = KErrNone;
            }
        }

    return err;
    }

TInt CCommDbTableExtension::GetRecordSetIndexL()
/**
Gets the value of the member iRecordIndex

@return A wrapper for an SQL string combined with a text comparison mode.
*/
    {
    if (iCurrentRecord)
        {
        User::Leave(KErrNotFound);
        }

    return iRecordSetContainer->iRecords.Find(iCurrentRecord);
    }

void CCommDbTableExtension::RefreshDatabaseL(CMDBSession& aDbs)
/**
Refresh the container
*/
	{
	if(iRecordSetContainer)
		{
		iRecordSetContainer->RefreshL(aDbs);
		}
	}

CCommDbTableExtension::~CCommDbTableExtension()
/**
Deletes the memory allocated to the members iColSet, iQueryText and iAccess
*/
    {
    delete iRecordSetContainer;
	iInsertedRecords.Reset();
	iDeletedRecords.Reset();
	iCommitSeqProperty.Close();
    }

TInt CCommDbTableExtension::CompareRecordsById(const CMDBRecordBase& aLeft, const CMDBRecordBase& aRight)
/**

@param
@return
*/
    {
    //We assume there can't be 2records with same record IDs
    return (aLeft.RecordId()) < (aRight.RecordId()) ? -1 : 1;
    }

TInt CCommDbTableExtension::CompareRecordsByRank(
            const CMDBRecordBase& aLeft, const CMDBRecordBase& aRight)
/**

@param
@param
@return
*/
    {
    return ( ((CCDConnectionPrefsRecord&)aLeft).iRanking < ((CCDConnectionPrefsRecord&)aRight).iRanking) ? -1 : 1;
    }

void CCommDbTableExtension::SortById()
/**
    Sorting values in iRecordSetContainer by Id value
*/
    {
    TLinearOrder<CMDBRecordBase> order(CompareRecordsById);
	if(iRecordSetContainer)
		{
    	iRecordSetContainer->iRecords.Sort(order);
		}
    }

void CCommDbTableExtension::SortByRank()
/**

@param
@return
*/
    {
    TLinearOrder<CMDBRecordBase> order(CompareRecordsByRank);
	if(iRecordSetContainer)
		{
	    iRecordSetContainer->iRecords.Sort(order);
		}
    }

TInt CCommDbTableExtension::GetTableAccess(RDbRowSet::TAccess &aAccess)
    {
    if( ! iRecordSetContainer)
        return KErrNotFound;

    if (iRecordSetContainer->IsSetAttribute(ECDProtectedWrite))
       {
       aAccess = RDbRowSet::EReadOnly;
       }
    else
       {
       aAccess = RDbRowSet::EUpdatable;
       }
    
    return KErrNone;
    }

void CCommDbTableExtension::CommitTransaction()
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommDbTableExtension::CommitTransaction()"));
	// remove flags since everything is valid
	iInsertedRecords.Reset();
	iDeletedRecords.Reset();
	}

void CCommDbTableExtension::RollBackTransaction()
	{
	__FLOG_STATIC0(KLogComponent, KCommDbShim, _L("CCommDbTableExtension::RollBackTransaction()"));

 	// we don't need to keep inserted records anymore
	TInt index;
	for (TInt i = 0; i < iInsertedRecords.Count(); ++i) 
		{
		index = FindValueInRecSet(iInsertedRecords[i]);

		if(index >= 0)
			{
			delete iRecordSetContainer->iRecords[index];
			iRecordSetContainer->iRecords.Remove(index);
			}
		}
	iRecordSetContainer->iRecords.Compress();
	index =	iRecordSetContainer->iRecords.Count();
	index ? iCurrentRecord = iRecordSetContainer->iRecords[index-1] : iCurrentRecord = NULL;

	// remove flags since everything is returned to begining
	iInsertedRecords.Reset();
	iDeletedRecords.Reset();
	}

TInt CCommDbTableExtension::GetCurrentRecordIndex() const
	{
	// If iCurrentRecord is valid then start with it, else start with most recently deleted record
	const TInt numRecs = iRecordSetContainer->iRecords.Count();
	TInt idx = -1;
	const TInt numDeleted = iDeletedRecords.Count();
	if(iCurrentRecord)
		{
		ASSERT(iDeletedRecords.Find(iCurrentRecord->RecordId()) == KErrNotFound);	// dbl-check it isn't somehow deleted too
		idx = iRecordSetContainer->iRecords.Find(iCurrentRecord);
		ASSERT(idx >= 0);
		}
	else 
		{
		if(numDeleted > 0)
			{
			idx = FindValueInRecSet(iDeletedRecords[numDeleted - 1]);
			}
		else
			{
			// No deleted records either, special case is going to -1, ie preparatory to advancing to first record
			}
		}
	return idx;
	}
	
TBool CCommDbTableExtension::AdvanceIndexToNextValidRecord(TInt& aIdx) const
	{
	// Advance to the next visible & not deleted in this view; hidden may not be visible
	const TInt numRecs = iRecordSetContainer->iRecords.Count();
	TInt idx;
	for(idx = aIdx + 1; idx < numRecs; ++idx)
		{
		CMDBRecordBase* rec = iRecordSetContainer->iRecords[idx];
		if(iDeletedRecords.Find(rec->RecordId()) >= 0)
			{
			continue;	// deleted are always invisible
			}
		if(iShowHidden || !rec->IsSetAttribute(ECDHidden))
			{
			break;
			}
		}
	if(idx < numRecs)
		{
		aIdx = idx;
		return ETrue;
		}
	return EFalse;
	}

TBool CCommDbTableExtension::AdvanceIndexToPrevValidRecord(TInt& aIdx) const
	{
	// Advance to the previous visible & not deleted in this view; hidden may not be visible
	TInt idx;
	for(idx = aIdx - 1; idx >= 0; --idx)
		{
		CMDBRecordBase* rec = iRecordSetContainer->iRecords[idx];
		if(iDeletedRecords.Find(rec->RecordId()) >= 0)
			{
			continue;	// deleted are always invisible
			}
		if(iShowHidden || !rec->IsSetAttribute(ECDHidden))
			{
			break;
			}
		}
	if(idx >= 0)
		{
		aIdx = idx;
		return ETrue;
		}
	return EFalse;
	}

TBool CCommDbTableExtension::FindFirstNextValidRecord(TBool aResetToStart)
	{
	TInt idx;
	if(aResetToStart)
		{
		iCurrentRecord = NULL;
		idx = -1;
		}
	else 
		{
		idx = GetCurrentRecordIndex();
		}
	if(!AdvanceIndexToNextValidRecord(idx))
		{
		return EFalse;
		}
	iCurrentRecord = iRecordSetContainer->iRecords[idx];
	__FLOG_STMT(TInt recNo = iRecordSetContainer->iRecords.Find(iCurrentRecord);)
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("GotoNextRecord -> CurrentRecord is RecNo:%d"), recNo);
	return ETrue;
	}

TBool CCommDbTableExtension::FindFirstPrevValidRecord()
	{
	TInt idx = GetCurrentRecordIndex();
	if(!AdvanceIndexToPrevValidRecord(idx))
		{
		return EFalse;
		}
	iCurrentRecord = iRecordSetContainer->iRecords[idx];
	__FLOG_STMT(TInt recNo = iRecordSetContainer->iRecords.Find(iCurrentRecord);)
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("GotoPreviousRecord -> CurrentRecord is RecNo:%d"), recNo);
	return ETrue;
	}

TInt CCommDbTableExtension::FindValueInRecSet(TInt aValue) const
	{
	// in case of insertion remove inserted record
	for(TInt index=0; index < iRecordSetContainer->iRecords.Count();index++)
		{
		if(iRecordSetContainer->iRecords[index]->RecordId() == aValue)
			{
			return index;
			}
		}
	
	return KErrNotFound;
	}

//--------------------------------------------------------------------
//	CCommsDatabaseImpl
//--------------------------------------------------------------------

CCommsDatabaseImpl::CCommsDatabaseImpl()
	:iHaveCheckedGSTableExists(EFalse), iDbs(NULL)
	{
	
	}
CCommsDatabaseImpl::~CCommsDatabaseImpl()
	{
	iTableViews.ResetAndDestroy();
	iConnPrefViews.ResetAndDestroy();
	
	//Cancel any pending notifications
	CancelRequestNotification();
	
	
	// do not delete iDbs before destroying the view arrays above
	delete iDbs;
	}

void CCommsDatabaseImpl::RegisterViewL(CCommsDbTableView* aTable)
	{
	iTableViews.AppendL(aTable);
	}

void CCommsDatabaseImpl::UnRegisterView(CCommsDbTableView* aTable)
	{
	TInt index = iTableViews.Find(aTable);
	if (index>=0) 
		{
		iTableViews.Remove(index);
		}
	}

void CCommsDatabaseImpl::RegisterConnPrefViewL(CCommsDbConnectionPrefTableView* aTable)
	{
	iConnPrefViews.AppendL(aTable);
	}

void CCommsDatabaseImpl::UnRegisterConnPrefView(CCommsDbConnectionPrefTableView* aTable)
	{
	TInt index = iConnPrefViews.Find(aTable);
	if (index>=0) 
		{
		iConnPrefViews.Remove(index);
		}
	}

void CCommsDatabaseImpl::CheckGlobalSettingsTableExistsL()
/**
    Important, this function requires 'ECapabilityWriteDeviceData' capability.
*/
    {
    // We only need to check that the global settings table exists once, because if it doesn't we'll create a placeholder.
    if (!iHaveCheckedGSTableExists)
        {
        // Ensure that a global record always exists.
    
        CCDGlobalSettingsRecord* pNewGlobalRecord = static_cast<CCDGlobalSettingsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdGlobalSettingsRecord));
        CleanupStack::PushL(pNewGlobalRecord);

        const TInt KGlobalSettingsRecordID = 1;
        
        pNewGlobalRecord->SetRecordId(KGlobalSettingsRecordID);
        
        TRAPD(err, pNewGlobalRecord->StoreL(*iDbs));
        if (!(err == KErrNone || err == KErrAlreadyExists))
            {
            User::Leave(err);
            }
            
        iHaveCheckedGSTableExists = ETrue;
                
        CleanupStack::PopAndDestroy(pNewGlobalRecord);
        }
    }

TInt CCommsDatabaseImpl::RequestNotification(TRequestStatus& aStatus)
	{  
        TInt ret(KErrNone);		
	if(!iShimNotifier.Handle())
                {
                ret = iShimNotifier.Connect(); //Also starts the server if not running allready
                }	
	
	if(ret == KErrNone)
               {
               iShimNotifier.RequestNotification(aStatus);
               }
               
        return ret;
	}

void CCommsDatabaseImpl::CancelRequestNotification()
	{
	if(iShimNotifier.Handle())
		{
		iShimNotifier.CancelNotification(); 	
		}
	iShimNotifier.Close();	
	}



// ------------------------ CommDb Shim Utils -------------------------

/**
@internalTechnology
*/
typedef struct 
	{
	const TText* iTableName;
	TUint iTableId;
	} TCommDbTableMapping;
		
/**
Number of mappings in the table
@internalTechnology
*/
const TInt KTableMappingsCount = 30;

/**
Mappings between legacy commdb table names and the commsdat table element id's
@internalTechnology
*/
const TCommDbTableMapping tableMappings[KTableMappingsCount] = 
	{
		{ KCDTypeNameModemBearer, CommsDat::KCDTIdModemBearerRecord },
		{ KCDTypeNameLANBearer, CommsDat::KCDTIdLANBearerRecord },
		{ KCDTypeNameVirtualBearer, CommsDat::KCDTIdVirtualBearerRecord },
		{ KCDTypeNameWAPSMSBearer, CommsDat::KCDTIdWAPSMSBearerRecord },
		{ KCDTypeNameWAPIPBearer, CommsDat::KCDTIdWAPIPBearerRecord },
		{ KCDTypeNameGlobalSettings, CommsDat::KCDTIdGlobalSettingsRecord },
		{ KCDTypeNameConnectionPrefs, CommsDat::KCDTIdConnectionPrefsRecord },
		{ KCDTypeNameNetwork, CommsDat::KCDTIdNetworkRecord },
		{ KCDTypeNameLocation, CommsDat::KCDTIdLocationRecord },
		{ KCDTypeNameIAP, CommsDat::KCDTIdIAPRecord },
		{ KCDTypeNameWAPAccessPoint, CommsDat::KCDTIdWAPAccessPointRecord },
		{ KCDTypeNameDialOutISP, CommsDat::KCDTIdDialOutISPRecord },
		{ KCDTypeNameDialInISP, CommsDat::KCDTIdDialInISPRecord },
		{ KCDTypeNameLANService, CommsDat::KCDTIdLANServiceRecord },
		{ KCDTypeNameWLANServiceExt, CommsDat::KCDTIdWLANServiceExtRecord },
		{ KCDTypeNameVPNService, CommsDat::KCDTIdVPNServiceRecord },
		{ KCDTypeNameOutgoingWCDMA, CommsDat::KCDTIdOutgoingGprsRecord },
		{ KCDTypeNameIncomingWCDMA, CommsDat::KCDTIdIncomingGprsRecord },
		{ KCDTypeNamePANServiceExt, CommsDat::KCDTIdPANServiceExtRecord },
		{ KCDTypeNameDefaultWCDMA, CommsDat::KCDTIdDefaultWCDMARecord },
		{ KCDTypeNameChargecard, CommsDat::KCDTIdChargecardRecord },
		{ KCDTypeNameProxies, CommsDat::KCDTIdProxiesRecord },
		{ KCDTypeNameSSProto, CommsDat::KCDTIdSSProtoRecord },
		{ KCDTypeNameAgentLookup, CommsDat::KCDTIdAgentLookupRecord },
		{ KCDTTypeNameEAPSec, CommsDat::KCDTIdEAPSecRecord },
		{ KCDTTypeNameTunEAP, CommsDat::KCDTIdTunEAPRecord },
		{ KCDTTypeNameEAPTLS, CommsDat::KCDTIdEAPTLSRecord },
		{ KCDTTypeNameLEAP, CommsDat::KCDTIdLEAPRecord },

		// These didn't exist in Commdb and don't have textual names
		//		KCDTIdAccessPointRecord
		//		KCDTIdIapPrioritySelectionPolicyRecord
		//		KCDTIdWCDMAPacketServiceRecord
		//		KCDTIdUmtsR99QoSAndOnTableRecord
	};



/**
@internalTechnology
*/
EXPORT_C CommsDat::TMDBElementId CommDbShimUtil::GetElementIdFromFieldNameL(const TDesC& aFieldName)
	{
	_LIT(KSlashCharacter, "\\");
   	TInt slashPos = aFieldName.Find(KSlashCharacter);
   	if(slashPos < 0)
   		{
   		User::Leave(KErrArgument);
   		}
   	TPtrC tableName(aFieldName.Ptr(), slashPos);
   	TPtrC fieldName(aFieldName.Ptr() + slashPos + 1, aFieldName.Length() - slashPos - 1);

   	CMDBRecordBase* record = NULL;
	TInt i = 0;
	while (record == NULL && i < KTableMappingsCount)
		{
		if (!tableName.CompareF(TPtrC(tableMappings[i].iTableName))) 
			{
			record = CCDRecordBase::RecordFactoryL(tableMappings[i].iTableId);
			CleanupStack::PushL(record);
			break;
			}
		i++;
		}

	TMDBElementId elementId;
	if (!record)
		{
        // Create a temporary example of the user defined record, in order to do the lookup by name and get the Id back.
        CMDBGenericRecord* tempUserDefinedRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(0));
        CleanupStack::PushL(tempUserDefinedRecord);
        
    	tempUserDefinedRecord->InitializeL(tableName, NULL);

    	CMDBSession* dbs = CMDBSession::NewLC(KCDVersion1_1);
        tempUserDefinedRecord->LoadL(*dbs);

        // Get the Id that we're interested in...
        TUint tableId = tempUserDefinedRecord->TableId();

        CleanupStack::PopAndDestroy(); // dbs
        CleanupStack::PopAndDestroy(); // tempUserDefinedRecord
        
		record = CCDRecordBase::RecordFactoryL(tableId);
		CleanupStack::PushL(record);	
		}
	
	TInt tempType;
	CMDBElement* field = record->GetFieldByNameL(fieldName, tempType);
	elementId = field->ElementId();
	CleanupStack::PopAndDestroy(); // record
	return elementId;
	}

//EOF
