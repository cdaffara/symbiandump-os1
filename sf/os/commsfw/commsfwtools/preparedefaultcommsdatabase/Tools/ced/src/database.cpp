// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/


#include "database.h"
#include "dbdef.h"
#include "filedump.h"
#include <etelqos.h>
#include <etelmm.h>
#include <etelpckt.h>
#include <faxdefn.h>
#include "T_tabdef.h"
#include "input.h"
#include <networking/panroles.h>
#include <nifvar_internal.h>

#include <e32hashtab.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypesv1_1_partner.h>
#include <commsdattypesv1_1_internal.h>
#include <nifvar_internal.h>
#endif

/**
makes logging accessible to all files
*/
extern CFileDump* gMsg;
/**
text for local error codes
@internalComponent
*/
TPtrC errorCode[] = {
	_L("Not all required field data supplied"),							//< E_NOTENOUGHDATA
	_L("Command not possible in this transaction state"),				//< E_INVALIDCOMMAND
	_L("Field data supplied has incorrect data type"),					//< E_INCORRECTTYPE
	_L("Column data is invalid for the last written column's type"),	//< E_INCORRECTCOLNAME
	_L("Object already exists"),										//< E_ALREADYEXISTS
	_L("Enumeration supplied is incorrect for this field"),				//< E_INCORRECTENUM
	_L("Incorrect number of Agent fields supplied"),					//< E_INCORRECTAGENT
	_L("Field count does not match expected amount"),					//< E_BADFIELDCOUNT
	_L("No fields recognised"),											//< E_NOFIELDS
	_L("Not supported"),												//< E_NOTSUPPORTED
	_L("No Global Settings specified"),									//< E_INCORRECTGLOBAL
	_L("Bad escaped character"),										//< E_BADESCAPEDCHAR
	_L("Bad hexadecimal")												//< E_BADHEXADECIMAL
	};

#define KTableLinkSeparator		_S("Link")


DBAccess::DBAccess(const TBool aXML)
:
iSecure(EFalse),
iCommitIndividualRecords(EFalse),
iIsMeshFileProcessing(EFalse),
access(DBUnknown),
iLastError(KErrNone),
LastErrorMessage(_L("")),
FieldCountExpected(0),
iColCount(0),
i2000PacketServiceTable(1),
iInAppendMode(0),
iIsXML(aXML),
iLnkByTagRes(NULL)
	{
	}


DBAccess::~DBAccess()
	{
	Close();
	CloseTable();
	iRecordSetHolder.ResetAndDestroy();
	}

void DBAccess::InitCommsdatL()
	{
	#ifdef __COMMDB_ROHID_SUPPORT__
		iSecure = ETrue;
	#endif

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	if (iIsMeshFileProcessing)
		{
		//in this case no mapper is needed.
		iDbSession = CMDBSession::NewL(KCDVersion1_2);
		}
	else
		{
		iDbSession = CMDBSession::NewL(KCDVersion1_1);
		}
#else
	iDbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	iDbSession->SetAttributeMask(ECDHidden | ECDPrivate | ECDProtectedWrite |ECDNoWriteButDelete );
	}

void DBAccess::MaybeBeginTransactionL()
	{
	if(!iDbSession->IsInTransaction())
		{
		iDbSession->OpenTransactionL();
		}
	}

TInt DBAccess::CommitTransaction()
	{
	TInt ret = KErrNone;
	if(iDbSession && iDbSession->IsInTransaction())
		{
		TRAP(ret, iDbSession->CommitTransactionL());
		iRecordSetHolder.ResetAndDestroy();
		}
	return ret;
	}

void DBAccess::Close()
	{
	if(iDbSession)
		{
		delete iDbSession;
		iDbSession = NULL;
		}
	}


void DBAccess::CloseTable()
/**
Closes the table
*/
	{
	if(iTable)
		{
		iTable->iRecords.ResetAndDestroy();
		iTable->iRecords.Close();

		delete iTable;
		iTable = NULL;
		iTableId = 0;
		}
	}


TInt DBAccess::CommitChanges()
/**
Finalise changes to database

  @return ETrue if successful or EFalse
  */
	{
	TInt ret = KErrNone;
	if(iTable)
		{
		gMsg->Dbg(_L("  Committing record"));
        // ******
        // Modify, don't store as if appending don't know what will already be there.
        // This was introduced with meshpreface file helping migration to new
        // comms framework, but is generally useful idea
		// TRAP(ret, iTable->ModifyL(*iDbSession));
        // ******

		/**
		only for testing...
		CCDConnectionPrefsRecord* elem = NULL;
		TInt elemId = iTable->iRecords[0]->ElementId();
		if ( (elemId & KCDMaskShowRecordType) == KCDTIdConnectionPrefsRecord)
		    {
		    elem = static_cast<CCDConnectionPrefsRecord*>(iTable->iRecords[0]);
		    }
		*/
		TRAP(ret, iTable->StoreL(*iDbSession));
		if(ret == KErrNone && iDbSession->IsInTransaction())
			{
			ret = iRecordSetHolder.Append(iTable);
			}
		else
			{
			delete iTable;
			}
		iTable = NULL;
		}
	return ret;
	}

// PRIVATE METHODS

void DBAccess::RemoveCurrentRecord()
	{
	CMDBRecordBase* tempptr = iTable->iRecords[iTable->iRecords.Count()-1];
	delete tempptr;

	iTable->iRecords.Remove(iTable->iRecords.Count()-1);

	if(iRecordId)
		{
		iRecordId = iRecordId - 1;
		}
	}


CMDBRecordBase* DBAccess::GetCurrentRecord()
	{
	return (iTable->iRecords[iTable->iRecords.Count()-1]);
	}

void DBAccess::RecordIsTemplate(TBool aIsTemplate)
	{
	iIsTemplate = aIsTemplate;
	if(iIsTemplate)
		{
		iConfusingTemplateBehaviour = iIsTemplate;
		}
	}

void DBAccess::DoInitAndAddL(CCDRecordBase* aNewRecord,TInt aRecordId)
	{
	iColCount = 0;

	if(iIsTemplate)
		{
		aNewRecord->SetRecordId(KCDDefaultRecord);
		aNewRecord->SetAttributes(ECDHidden);
		}
	else if(iInAppendMode)
	    {
	    if (!iIsXML)
	        {
    		//We are appending and we don't have a clue about what record ids are already in use
    		//So we ask CommsDat to give us an unused one.
    		aNewRecord->SetRecordId(KCDNewRecordRequest);
    		User::LeaveIfError(iTable->iRecords.Append(aNewRecord));
    		return;
	        }
        else
            /**
             * When CED is processing XML files it always has recordIDs.
             */
            aNewRecord->SetRecordId(aRecordId);
	    }
    else if(aRecordId)
		{
		aNewRecord->SetRecordId(aRecordId);
		iRecordId = aRecordId + 1;
		}
	else if(!iTable->iRecords.Count())
		{
		//general case where template records are missing
		aNewRecord->SetRecordId(iRecordId);
		iRecordId = iRecordId + 1;
		}
	else
		{
		if(iConfusingTemplateBehaviour && iRecordId == 1)
			{
			iRecordId = iRecordId + 1;
			}

		aNewRecord->SetRecordId(iRecordId);
		iRecordId = iRecordId + 1;
		}

	// Check whether we've already inserted this record id - if so then we must reject it as a duplicate,
	// lest the database roll back the entire transaction (its behaviour at present)
	TInt newRecordId = aNewRecord->RecordId();
	for(TInt idx = iTable->iRecords.Count() - 1; idx >= 0; --idx)
		{
		if(iTable->iRecords[idx]->RecordId() == newRecordId)
			{
			gMsg->Msg(_L("ERR: record id %d already inserted, skipping (check input file)"), newRecordId);
			delete aNewRecord;
			return;
			}
		}

	User::LeaveIfError(iTable->iRecords.Append(aNewRecord));
	}

void DBAccess::InitAndAddL(CCDRecordBase* aNewRecord,TInt aRecordId)
	{
	CleanupStack::PushL(aNewRecord);
	DoInitAndAddL(aNewRecord,aRecordId);
	CleanupStack::Pop(aNewRecord);
	}

void DBAccess::SaveTableProtection(CMDBRecordSetBase* aRecordSet)
	{
	CloseTable();
	iTable = (CMDBRecordSetBase*)aRecordSet;
	}

void DBAccess::CreateOrInsertRecordL(TCreateType aType,TMDBElementId aRecordElementId, TInt aRecordId)
/*
This method returns the pointer to recordset type depending on record Id
*/
	{
	if(aType == EInsert && iTable->iRecords.Count() >= KCDMaxRecords)
		{
		User::Leave(KErrTooBig); //actually KErrTooManyRecords
		}

	if(aType != EInsert)
		{
		if(aType == ECreateNew)
			{
			iRecordId =	1;
			}
		iTableId = aRecordElementId;
		}

	iNameSet = EFalse;

	switch(iTableId)
		{
		case KCDTIdConnectionPrefsRecord:
			{
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDConnectionPrefsRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDConnectionPrefsRecord>(KCDTIdConnectionPrefsRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDConnectionPrefsRecord* ptrNewRecord = (CCDConnectionPrefsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			}
			break;

		case KCDTIdIAPRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDIAPRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDIAPRecord* ptrNewRecord = (CCDIAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdNetworkRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDNetworkRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDNetworkRecord>(KCDTIdNetworkRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDNetworkRecord* ptrNewRecord = (CCDNetworkRecord*)CCDRecordBase::RecordFactoryL(KCDTIdNetworkRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdLocationRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDLocationRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDLocationRecord>(KCDTIdLocationRecord | 1);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDLocationRecord* ptrNewRecord = (CCDLocationRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLocationRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;


		case KCDTIdGlobalSettingsRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDGlobalSettingsRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDGlobalSettingsRecord>(KCDTIdGlobalSettingsRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDGlobalSettingsRecord* ptrNewRecord = (CCDGlobalSettingsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdGlobalSettingsRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdWAPAccessPointRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDWAPAccessPointRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDWAPAccessPointRecord>(KCDTIdWAPAccessPointRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDWAPAccessPointRecord* ptrNewRecord = (CCDWAPAccessPointRecord*)CCDRecordBase::RecordFactoryL(KCDTIdWAPAccessPointRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdDialOutISPRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDDialOutISPRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDDialOutISPRecord>(KCDTIdDialOutISPRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDDialOutISPRecord* ptrNewRecord = (CCDDialOutISPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdDialOutISPRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdDialInISPRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDDialInISPRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDDialInISPRecord>(KCDTIdDialInISPRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDDialInISPRecord* ptrNewRecord = (CCDDialInISPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdDialInISPRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdLANServiceRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDLANServiceRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDLANServiceRecord>(KCDTIdLANServiceRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDLANServiceRecord* ptrNewRecord = (CCDLANServiceRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLANServiceRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdWLANServiceExtRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDWLANServiceExtRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDWLANServiceExtRecord>(KCDTIdWLANServiceExtRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDWLANServiceExtRecord* ptrNewRecord = (CCDWLANServiceExtRecord*)CCDRecordBase::RecordFactoryL(KCDTIdWLANServiceExtRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdVPNServiceRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDVPNServiceRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDVPNServiceRecord>(KCDTIdVPNServiceRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDVPNServiceRecord* ptrNewRecord = (CCDVPNServiceRecord*)CCDRecordBase::RecordFactoryL(KCDTIdVPNServiceRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdPANServiceExtRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDPANServiceExtRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDPANServiceExtRecord>(KCDTIdPANServiceExtRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDPANServiceExtRecord* ptrNewRecord = (CCDPANServiceExtRecord*)CCDRecordBase::RecordFactoryL(KCDTIdPANServiceExtRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;


		case KCDTIdWCDMAPacketServiceRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDWCDMAPacketServiceRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDWCDMAPacketServiceRecord>(KCDTIdWCDMAPacketServiceRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				iRecordId = i2000PacketServiceTable;
				}
			else
				{
				CCDWCDMAPacketServiceRecord* ptrNewRecord = (CCDWCDMAPacketServiceRecord*)CCDRecordBase::RecordFactoryL(KCDTIdWCDMAPacketServiceRecord);
				if(i2000PacketServiceTable == 1)
					{
					InitAndAddL(ptrNewRecord,aRecordId);
					}
				else
					{
					InitAndAddL(ptrNewRecord,i2000PacketServiceTable);
					}
				i2000PacketServiceTable = iRecordId;
				}
			break;
		case KCDTIdOutgoingGprsRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDOutgoingGprsRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDOutgoingGprsRecord>(KCDTIdOutgoingGprsRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDOutgoingGprsRecord* ptrNewRecord = (CCDOutgoingGprsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdOutgoingGprsRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;
		case KCDTIdIncomingGprsRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDIncomingGprsRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDIncomingGprsRecord>(KCDTIdIncomingGprsRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDIncomingGprsRecord* ptrNewRecord = (CCDIncomingGprsRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIncomingGprsRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdUmtsR99QoSAndOnTableRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDUmtsR99QoSAndOnTableRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDUmtsR99QoSAndOnTableRecord>(KCDTIdUmtsR99QoSAndOnTableRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDUmtsR99QoSAndOnTableRecord* ptrNewRecord = static_cast<CCDUmtsR99QoSAndOnTableRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdUmtsR99QoSAndOnTableRecord));
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdDefaultWCDMARecord:

			if(aType != EInsert)
				{
				CMDBRecordSet<CCDDefaultWCDMARecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDDefaultWCDMARecord>(KCDTIdDefaultWCDMARecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDDefaultWCDMARecord* ptrNewRecord = (CCDDefaultWCDMARecord*)CCDRecordBase::RecordFactoryL(KCDTIdDefaultWCDMARecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;


		case KCDTIdModemBearerRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDModemBearerRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDModemBearerRecord* ptrNewRecord = (CCDModemBearerRecord*)CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdLANBearerRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDLANBearerRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDLANBearerRecord>(KCDTIdLANBearerRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDLANBearerRecord* ptrNewRecord = (CCDLANBearerRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLANBearerRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdVirtualBearerRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDVirtualBearerRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDVirtualBearerRecord>(KCDTIdVirtualBearerRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDVirtualBearerRecord* ptrNewRecord = (CCDVirtualBearerRecord*)CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdWAPSMSBearerRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDWAPSMSBearerRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDWAPSMSBearerRecord>(KCDTIdWAPSMSBearerRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDWAPSMSBearerRecord* ptrNewRecord = (CCDWAPSMSBearerRecord*)CCDRecordBase::RecordFactoryL(KCDTIdWAPSMSBearerRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdWAPIPBearerRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDWAPIPBearerRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDWAPIPBearerRecord>(KCDTIdWAPIPBearerRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDWAPIPBearerRecord* ptrNewRecord = (CCDWAPIPBearerRecord*)CCDRecordBase::RecordFactoryL(KCDTIdWAPIPBearerRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdChargecardRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDChargecardRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDChargecardRecord>(KCDTIdChargecardRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDChargecardRecord* ptrNewRecord = (CCDChargecardRecord*)CCDRecordBase::RecordFactoryL(KCDTIdChargecardRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdProxiesRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDProxiesRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDProxiesRecord>(KCDTIdProxiesRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDProxiesRecord* ptrNewRecord = (CCDProxiesRecord*)CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdSSProtoRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDSecureSocketRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDSecureSocketRecord>(KCDTIdSSProtoRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDSecureSocketRecord* ptrNewRecord = (CCDSecureSocketRecord*)CCDRecordBase::RecordFactoryL(KCDTIdSSProtoRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdAgentLookupRecord: //todo exist?
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDAgentLookupRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDAgentLookupRecord>(KCDTIdAgentLookupRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDAgentLookupRecord* ptrNewRecord = (CCDAgentLookupRecord*)CCDRecordBase::RecordFactoryL(KCDTIdAgentLookupRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

    	case KCDTIdAccessPointRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDAccessPointRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDAccessPointRecord>(KCDTIdAccessPointRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDAccessPointRecord* ptrNewRecord = (CCDAccessPointRecord*)CCDRecordBase::RecordFactoryL(KCDTIdAccessPointRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        case KCDTIdConfigAccessPointRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDConfigAccessPointRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDConfigAccessPointRecord>(KCDTIdConfigAccessPointRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDConfigAccessPointRecord* ptrNewRecord = (CCDConfigAccessPointRecord*)CCDRecordBase::RecordFactoryL(KCDTIdConfigAccessPointRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;
		case KCDTIdApPrioritySelectionPolicyRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDAPPrioritySelectionPolicyRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDAPPrioritySelectionPolicyRecord>(KCDTIdApPrioritySelectionPolicyRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDAPPrioritySelectionPolicyRecord* ptrNewRecord = (CCDAPPrioritySelectionPolicyRecord*)CCDRecordBase::RecordFactoryL(KCDTIdApPrioritySelectionPolicyRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;
		case KCDTIdTierRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDTierRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDTierRecord>(KCDTIdTierRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDTierRecord* ptrNewRecord = (CCDTierRecord*)CCDRecordBase::RecordFactoryL(KCDTIdTierRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

    	case KCDTIdMCprRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDMCprRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDMCprRecord>(KCDTIdMCprRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDMCprRecord* ptrNewRecord = (CCDMCprRecord*)CCDRecordBase::RecordFactoryL(KCDTIdMCprRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

    	case KCDTIdCprRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDCprRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDCprRecord>(KCDTIdCprRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDCprRecord* ptrNewRecord = (CCDCprRecord*)CCDRecordBase::RecordFactoryL(KCDTIdCprRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

    	case KCDTIdSCprRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDSCprRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDSCprRecord>(KCDTIdSCprRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDSCprRecord* ptrNewRecord = (CCDSCprRecord*)CCDRecordBase::RecordFactoryL(KCDTIdSCprRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

    	case KCDTIdProtocolRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDProtocolRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDProtocolRecord>(KCDTIdProtocolRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDProtocolRecord* ptrNewRecord = (CCDProtocolRecord*)CCDRecordBase::RecordFactoryL(KCDTIdProtocolRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdBearerTypeRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDBearerTypeRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDBearerTypeRecord>(KCDTIdBearerTypeRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDBearerTypeRecord* ptrNewRecord = static_cast<CCDBearerTypeRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdBearerTypeRecord));
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    	case KCDTIdIapPrioritySelectionPolicyRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDIAPPrioritySelectionPolicyRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDIAPPrioritySelectionPolicyRecord>(KCDTIdIapPrioritySelectionPolicyRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDIAPPrioritySelectionPolicyRecord* ptrNewRecord = (CCDIAPPrioritySelectionPolicyRecord*)CCDRecordBase::RecordFactoryL(KCDTIdIapPrioritySelectionPolicyRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdEAPSecRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDEAPSecRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDEAPSecRecord>(KCDTIdEAPSecRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDEAPSecRecord* ptrNewRecord = (CCDEAPSecRecord*)CCDRecordBase::RecordFactoryL(KCDTIdEAPSecRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdTunEAPRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDTunEAPRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDTunEAPRecord>(KCDTIdTunEAPRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDTunEAPRecord* ptrNewRecord = (CCDTunEAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdTunEAPRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdEAPTLSRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDEAPTLSRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDEAPTLSRecord>(KCDTIdEAPTLSRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDEAPTLSRecord* ptrNewRecord = (CCDEAPTLSRecord*)CCDRecordBase::RecordFactoryL(KCDTIdEAPTLSRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

		case KCDTIdLEAPRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDLEAPRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDLEAPRecord>(KCDTIdLEAPRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDLEAPRecord* ptrNewRecord = (CCDLEAPRecord*)CCDRecordBase::RecordFactoryL(KCDTIdLEAPRecord);
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

    	case KCDTIdEapSimProtocolRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDEapSimProtocolRecord>* ptrRecordSet =
				  new(ELeave) CMDBRecordSet<CCDEapSimProtocolRecord>(KCDTIdEapSimProtocolRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDEapSimProtocolRecord* ptrNewRecord =
				  static_cast<CCDEapSimProtocolRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdEapSimProtocolRecord));
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

    	case KCDTIdEapAkaProtocolRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDEapAkaProtocolRecord>* ptrRecordSet =
				  new(ELeave) CMDBRecordSet<CCDEapAkaProtocolRecord>(KCDTIdEapAkaProtocolRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDEapAkaProtocolRecord* ptrNewRecord =
				  static_cast<CCDEapAkaProtocolRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdEapAkaProtocolRecord));
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		case KCDTIdPolicySelectorRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDPolicySelectorRecord>* ptrRecordSet =
				  new(ELeave) CMDBRecordSet<CCDPolicySelectorRecord>(KCDTIdPolicySelectorRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDPolicySelectorRecord* ptrNewRecord =
				  static_cast<CCDPolicySelectorRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdPolicySelectorRecord));
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;
		case KCDTIdPolicySelector2ParamsRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDPolicySelector2ParamsRecord>* ptrRecordSet =
				  new(ELeave) CMDBRecordSet<CCDPolicySelector2ParamsRecord>(KCDTIdPolicySelector2ParamsRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDPolicySelector2ParamsRecord* ptrNewRecord =
				  static_cast<CCDPolicySelector2ParamsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdPolicySelector2ParamsRecord));
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;
		case KCDTIdGenericQosRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDGenericQosRecord>* ptrRecordSet =
				  new(ELeave) CMDBRecordSet<CCDGenericQosRecord>(KCDTIdGenericQosRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDGenericQosRecord* ptrNewRecord =
				  static_cast<CCDGenericQosRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdGenericQosRecord));
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;
		case KCDTIdWifiScanEngineRecord:
			if(aType != EInsert)
				{
				CMDBRecordSet<CCDWifiScanEngineRecord>* ptrRecordSet =
				  new(ELeave) CMDBRecordSet<CCDWifiScanEngineRecord>(KCDTIdWifiScanEngineRecord);
				SaveTableProtection((CMDBRecordSetBase*)ptrRecordSet);
				}
			else
				{
				CCDWifiScanEngineRecord* ptrNewRecord =
				  static_cast<CCDWifiScanEngineRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdWifiScanEngineRecord));
				InitAndAddL(ptrNewRecord,aRecordId);
				}
			break;
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		default:
			User::Leave(KErrArgument);
			break;
		};

	}

void DBAccess::RemoveRecord(TInt aPos)
	{
	CMDBRecordBase* recordToRemove = iTable->iRecords[aPos];
	iTable->iRecords.Remove(aPos);

	delete recordToRemove;
	}

TBool DBAccess::ConvertLinkRecords(CMDBElement* aPtrField, TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute)
    {
    TBool err(EFalse);

    switch(iTableId)
        {
        case KCDTIdIAPRecord:
			err = SetLinkedRecord(ELRIAP, aPtrField,aColumn,aSetting,aAttribute);
            break;
        case KCDTIdAccessPointRecord:
			err = SetLinkedRecord(ELRAccessPoints, aPtrField,aColumn,aSetting,aAttribute);
			break;
        case KCDTIdWLANServiceExtRecord:
			err = SetLinkedRecord(ELRWLANServiceExt, aPtrField,aColumn,aSetting,aAttribute);
			break;
        case KCDTIdEAPSecRecord:
			err = SetLinkedRecord(ELREAPSec, aPtrField,aColumn,aSetting,aAttribute);
			break;
        case KCDTIdTunEAPRecord:
			err = SetLinkedRecord(ELRTunEapSettings, aPtrField,aColumn,aSetting,aAttribute);
			break;
		/**
		 * in the case of xml processing the LocationForDataAndFax, LocationForPhoneServicesAndSMS,
		 * DefaultNetwork records in the ConnectionPref table are a links
		 */
        case KCDTIdGlobalSettingsRecord:
        	err = SetLinkedRecord(ELRGlobalSettings, aPtrField,aColumn,aSetting,aAttribute);
			break;
		/**
		 * in the case of xml processing the IAP record in the ConnectionPref table is a link
		 */
		case KCDTIdConnectionPrefsRecord:
        	err = SetLinkedRecord(ELRConnectionPreference, aPtrField,aColumn,aSetting,aAttribute);
			break;
		case KCDTIdWAPIPBearerRecord:
		    err = SetLinkedRecord(ELRWAPIPBearer, aPtrField,aColumn,aSetting,aAttribute);
			break;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        case KCDTIdBearerTypeRecord:
            err = SetLinkedRecord(ELRBearerType, aPtrField,aColumn,aSetting,aAttribute);
			break;
		case KCDTIdApPrioritySelectionPolicyRecord:
            err = SetLinkedRecord(ELRAPPrioritySel, aPtrField,aColumn,aSetting,aAttribute);
			break;
		case KCDTIdTierRecord:
            err = SetLinkedRecord(ELRTier, aPtrField,aColumn,aSetting,aAttribute);
			break;
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                case KCDTIdVPNServiceRecord:
	            err = SetLinkedRecord(ELRVPNService, aPtrField,aColumn,aSetting,aAttribute);
	        break;
		default: ;
        }

    return err;
    }
/* This is a special case here because in the cfg files there can be legacy links.
 * Legacy link menas that there is a table name in the given record and another field
 * which is the record is of the referenced record. For example Bearer and BearerType
 * in the IAP record.
 * The trick here is that we should check particular text fields, which basically can
 * contain anything, but in this case only specific values, the known table names, can
 * be in the field value.*/
TBool DBAccess::CheckLegacyLinkRecords(TPtrC& aColumn,TPtrC& aSetting)
	{
	TInt i = 0;
	TPtrC actField = LegacyLinkFields[i];
	while (actField.Compare(aColumn) != 0 &&
		   actField.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
		{
		actField.Set(TPtrC(LegacyLinkFields[++i]));
		}

	if (actField.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
		{
		//we've found a legacy link, where the table name is set. Check whether
		//the given table name is valid -> known by CED
		i = 0;
		TPtrC actTableName = tableArray[i];
		while (actTableName.Compare(aSetting) != 0 &&
			   actTableName.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			actTableName.Set(TPtrC(tableArray[++i]));
			}

		if (actTableName.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			//we got the referenced table as known table
			return KErrNone;
			}
		else
			{
			//we don't get the referenced table -> CED doesn't know this tablename
			return KErrNotFound;
			}
		}
	return KErrNone;
	}

TBool DBAccess::ConvertEnumsL(CMDBElement* aPtrField, TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute)
	{

	if(ConvertMiscEnums(aPtrField,aColumn,aSetting,aAttribute))
		{
		return ETrue;
		}
	TBool result(EFalse);
	switch(iTableId)
		{
		case KCDTIdConnectionPrefsRecord:
			result =  ConvertConnectionPrefEnums(aPtrField,aColumn,aSetting,aAttribute);
			break;
		case KCDTIdDialInISPRecord:
		case KCDTIdDialOutISPRecord:
			result =  ConvertISPEnums(aPtrField,aColumn,aSetting,aAttribute);
			break;
		case KCDTIdModemBearerRecord:
		case KCDTIdLANBearerRecord:
		case KCDTIdVirtualBearerRecord:
			result =  ConvertModemEnums(aPtrField,aColumn,aSetting,aAttribute);
			break;
		case KCDTIdWAPSMSBearerRecord:
		case KCDTIdWAPIPBearerRecord:
			result =  ConvertWAPEnums(aPtrField,aColumn,aSetting,aAttribute);
			break;
		case KCDTIdPANServiceExtRecord:
			result =  ConvertPANEnums(aPtrField,aColumn,aSetting,aAttribute);
			break;
		case KCDTIdIapPrioritySelectionPolicyRecord:
			result = ConvertIapPrioritySelectionPolicyEnums(aPtrField,aColumn,aSetting,aAttribute);
			break;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		case KCDTIdApPrioritySelectionPolicyRecord:
			result = ConvertApPrioritySelectionPolicyEnums(aPtrField,aColumn,aSetting,aAttribute);
			break;
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

		case KCDTIdEAPSecRecord:
			result = ConvertEAPSecEnums(aPtrField,aColumn,aSetting,aAttribute);
			break;
		case KCDTIdTunEAPRecord:
			result = ConvertTUNEAPSecEnums(aPtrField,aColumn,aSetting,aAttribute);
			break;
		case KCDTIdGlobalSettingsRecord:
		case KCDTIdWLANServiceExtRecord:
			result = ConvertWLANEnums(aPtrField,aColumn,aSetting,aAttribute);
			break;
		case KCDTIdOutgoingGprsRecord:
		case KCDTIdIncomingGprsRecord:
			result = ConvertGPRSEnums(aPtrField,aColumn,aSetting,aAttribute);
			break;

		default:
			return EFalse;
	      	}
	return result;
	}



TInt DBAccess::ConvertSpecialCharsL(TInt aTempType,CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute)
	{
	const TInt E_NOERROR = -1;	// not added to global list to avoid its use in wider contexts, eg error code lookup
	TInt errorReason = E_NOERROR;
	TInt i = 0;
	TBuf<MAX_COL_LONG_VAL_LEN> TempValue;

	gMsg->Dbg(_L("Special char  precess Input Column: [%S]  , Value:[%S] "),&aColumn,&aSetting);
	while(errorReason == E_NOERROR && i < aSetting.Length())
		{
		TUint16 settingChar = aSetting[i++];
		if(settingChar == '\\')
			{
			if(i < aSetting.Length())
				{
				settingChar = aSetting[i++];
				switch(settingChar)
					{
					case 't':	// Emit a tab
						settingChar = '\t';
						break;
					case 'n':	// Emit a LF
						settingChar = '\n';
						break;
					case '[':	// Emit a [
						break;
					case ']':	// Emit a ]
						break;
					case '\\':	// Emit a backslash
						break;
					case 'x':	// Emit hex-coded char
						if(i + 4 <= aSetting.Length())
							{
							TPtrC hexStr(aSetting.Mid(i, 4));
							TLex hexLex(hexStr);
							if(hexLex.Val(settingChar, EHex) != KErrNone)
								{
								errorReason = E_BADHEXADECIMAL;
								}
							i += 4;
							}
						else
							{
							errorReason = E_BADHEXADECIMAL;
							}
						break;
					default:
						errorReason = E_BADESCAPEDCHAR;
						break;
					}
				}
			else
				{
				errorReason = E_BADESCAPEDCHAR;
				}
			}
		TempValue.Append(settingChar);
		}

	TPtrC NewSetting;
	NewSetting.Set(TempValue);

	if(errorReason != E_NOERROR)
		{
		gMsg->Msg(_L("ERR: Special char processing of %S failed due to %S"), &aColumn, &errorCode[errorReason]);
		return KErrArgument;
		}

	AssignFieldValuesL(aColumn,aTempType,aPtrField,NewSetting,aAttribute);

	return KErrNone;
	}


TBool DBAccess::AssignFieldValuesL(TPtrC& aColumn,TInt aTempType,CMDBElement* aPtrField,TPtrC& aSetting,TMDBAttributeFlags /*aAttribute*/)
	{
	TBool valueIsValid=ETrue;

	switch(aTempType)
		{
		case EText:
			{
			CMDBTextFieldBase* ptrTempField  = (CMDBTextFieldBase*)aPtrField;
			if(aSetting.Length() > (MAX_COL_VAL_LEN - 1))
				{
				//truncate truncate
				ptrTempField->SetMaxLengthL(MAX_COL_VAL_LEN - 1);
				*ptrTempField =   aSetting.Mid(0,MAX_COL_VAL_LEN - 1);
				}
			else
				{
				ptrTempField->SetMaxLengthL(aSetting.Length());
				*ptrTempField =   aSetting;
				}

		//	(*ptrTempField).SetAttributes(aAttribute);
			TBuf<MAX_COL_VAL_LEN> pColumnV;
			pColumnV = *((CMDBField<TDesC>*)ptrTempField);
			gMsg->Msg(_L("  Column [%S] set to [%S]"), &aColumn, &pColumnV);
			}
			break;
		case EDesC8:
			{
			CMDBBinFieldBase* ptrTempField  = (CMDBBinFieldBase*)aPtrField;
			TBuf8<MAX_COL_VAL_LEN> pColumnV;
			TBuf<MAX_COL_VAL_LEN> pColumnVUni;
			if(aSetting.Length() > (MAX_COL_VAL_LEN - 1))
				{
				pColumnV.Mid(0, MAX_COL_VAL_LEN - 1);
				}
			else
				{
				pColumnV.Copy(aSetting );
				}

			ptrTempField->SetMaxLengthL(pColumnV.Length());
			*ptrTempField = pColumnV;

			pColumnVUni.Copy(pColumnV);
			gMsg->Msg(_L("  Column [%S] set to [%S]"), &aColumn, &pColumnVUni);
			}
			break;
		case EBool:
			{
			CMDBField<TBool>* ptrTempField  = (CMDBField<TBool>*)aPtrField;
			TBool valueToSet=ETrue;

			// check value isn't TRUE enum
			if (aSetting.Compare(TPtrC(TRUE_VAL)) == 0)
				{
				gMsg->Dbg(_L(" The Value is set to ETrue"));
				valueToSet = ETrue;
				}
			else if (aSetting.Compare(TPtrC(FALSE_VAL)) == 0)
				{
				gMsg->Dbg(_L(" The Value is set to EFalse"));
				valueToSet = EFalse;
				}
			else
				{
				gMsg->Msg(_L("  ERR: Boolean mismatch column [%S] value [%S] - will not populate field."), &aColumn, &aSetting);
				valueIsValid=EFalse;
				if (iValidityChecking) User::Leave(KErrArgument);
				}

			if(valueIsValid)
				{
				*ptrTempField = valueToSet;
				gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, (TBool)*ptrTempField);
				}
			}
			break;


		case EInt:
			{
			CMDBField<TInt>* ptrTempField  = (CMDBField<TInt>*)aPtrField;
			TInt32 valueToSet;
			TBool isHex = (aSetting.FindF(KHexMarker) == 0);

			if (isHex)
			    {
				TUint32 hexVal;
				TLex lex(aSetting.Mid(KHexMarker().Length()));
				TInt err = lex.Val(hexVal, EHex);
				if(err != KErrNone)
					{
					gMsg->Msg(_L("ERR: Corrupt hex value '%S' - will not populate field."), &aSetting);
					if (iValidityChecking) User::Leave(KErrArgument);
					valueIsValid=EFalse;
					}
				else
					{
					valueToSet = hexVal;
					}
			    }
			else
			    {
				TLex lex(aSetting);
				TInt err = lex.Val(valueToSet);
				if(err != KErrNone)
					{
					gMsg->Msg(_L("ERR: Integer mismatch column [%S] value [%S] - will not populate field."), &aColumn, &aSetting);
					if (iValidityChecking) User::Leave(KErrArgument);
					valueIsValid=EFalse;
					}
				}

			if(valueIsValid)
				{
				*ptrTempField = valueToSet;
				gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, valueToSet);
				}
			}
			break;
		case EUint32:
			{
			CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;
			TUint32 valueToSet;
			TBool isHex = (aSetting.FindF(KHexMarker) == 0);
			TBool valueIsValid=ETrue;

			if (isHex)
			    {
				TLex lex(aSetting.Mid(KHexMarker().Length()));
				TInt err = lex.Val(valueToSet, EHex);
				if(err != KErrNone)
					{
					gMsg->Msg(_L("ERR: Corrupt hex value '%S' - will not populate field."), &aSetting);
					if (iValidityChecking) User::Leave(KErrArgument);
					valueIsValid=EFalse;
					}
			    }
			else
				{
				TLex lex(aSetting);
				TInt err = lex.Val(valueToSet,EDecimal);
				if(err != KErrNone)
					{
					gMsg->Msg(_L("ERR: UInt32 mismatch column [%S] value [%S] - will not populate field."), &aColumn, &aSetting);
					if (iValidityChecking) User::Leave(KErrArgument);
					valueIsValid=EFalse;
					}
				}

			if(valueIsValid)
				{
				*ptrTempField = valueToSet;
				gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, valueToSet);
				}
			}
			break;

		case EMedText:
			{
			CMDBTextFieldBase* ptrTempField  = (CMDBTextFieldBase*)aPtrField;

			if(aSetting.Length() > (MAX_COL_MED_VAL_LEN - 1))
				{
				//truncate truncate
				ptrTempField->SetMaxLengthL(MAX_COL_MED_VAL_LEN - 1);
				*ptrTempField =   aSetting.Mid(0,MAX_COL_MED_VAL_LEN - 1);
				}
			else
				{
				ptrTempField->SetMaxLengthL(aSetting.Length());
				*ptrTempField =   aSetting;
				}
			//(*ptrTempField).SetAttributes(aAttribute);

			TBuf<MAX_COL_MED_VAL_LEN> pColumnV;
			pColumnV = *((CMDBField<TDesC>*)ptrTempField);
			gMsg->Msg(_L("  Column [%S] set to [%S]"), &aColumn, &pColumnV);
			}
			break;
		case ELongText:
			{
			CMDBTextFieldBase* ptrTempField  = (CMDBTextFieldBase*)aPtrField;
			if(aSetting.Length() > (MAX_COL_LONG_VAL_LEN - 1))
				{
				//truncate truncate
				ptrTempField->SetMaxLengthL(MAX_COL_LONG_VAL_LEN - 1);
				*ptrTempField =   aSetting.Mid(0,MAX_COL_LONG_VAL_LEN - 1);
				}
			else
				{
				ptrTempField->SetMaxLengthL(aSetting.Length());
				*ptrTempField =   aSetting;
				}

			//(*ptrTempField).SetAttributes(aAttribute);

			TBuf<MAX_COL_LONG_VAL_LEN> pColumnV;
			pColumnV = *((CMDBField<TDesC>*)ptrTempField);
			gMsg->Msg(_L("  Column [%S] set to [%S]"), &aColumn, &pColumnV);
			}
			break;
		default:
			User::Panic(_L("ERR: Field has unknown type: AssignFieldValuesL()"),KErrNotFound);
			break;
		}

		iColCount++;

		return valueIsValid;
	}

TInt DBAccess::SetColAndAttribL(TPtrC& aColumn,TPtrC& aSetting)
	{

	TMDBAttributeFlags attribute = 0;
	if(iSecure)
		{
		attribute = (iTable->iRecords[iTable->iRecords.Count()-1])->Attributes();
		if(aColumn.CompareC(KReadOnly) == 0 )
			{
			if(aSetting == KTrueVal)
				{
				attribute |= ECDNoWriteButDelete;
				}
			else
				{
				attribute &= ~ECDNoWriteButDelete;
				}
			iColCount++;
			(iTable->iRecords[iTable->iRecords.Count()-1])->SetAttributes(attribute);
			return KErrNone;
			}

		//you can't set hidden field in connpref
			if(aColumn.CompareC(KHidden) == 0)
				{
				if(((iTable->iRecords[iTable->iRecords.Count()-1])->TypeId() & KCDMaskShowRecordType) != KCDTIdConnectionPrefsRecord)
					{
					if(aSetting == KTrueVal)
						{
						attribute |= ECDHidden;
						}
					else
						{
						attribute &= ECDHidden;
						}
					}
				else
					{
					attribute &= ECDHidden;
					}
				iColCount++;
				(iTable->iRecords[iTable->iRecords.Count()-1])->SetAttributes(attribute);
				return KErrNone;
				}
		}




	TInt tempType(0);

	gMsg->Dbg(_L("  Setting column [%S] value [%S]"), &aColumn, &aSetting);

	CMDBElement* ptrField = NULL;

	TRAPD(ret,(ptrField = (iTable->iRecords[iTable->iRecords.Count()-1])->GetFieldByNameL(aColumn,tempType)));

	if(ret !=  KErrNone)
		{
		gMsg->Dbg(_L("  ERR: GetFieldByNameL failed  column [%S] value [%S] errorcode [%d]"), &aColumn, &aSetting, ret);
		return ret;
		}

	if((ptrField->ElementId() & KCDMaskShowFieldType) == KCDTIdRecordName) //good engineer writes code in hexadecimals ;)
		{
		iNameSet = ETrue;
		}


	if(ConvertEnumsL(ptrField,aColumn,aSetting,attribute))
		{
		iColCount++;
		return KErrNone;
		}

	if(ConvertLinkRecords(ptrField,aColumn,aSetting,attribute))
		{
		iColCount++;
		return KErrNone;
		}

		ret = CheckLegacyLinkRecords(aColumn,aSetting);
		if (KErrNone != ret)
		{
		return ret;
		}

	_LIT(KSpecialChar, "\\");

	if(aSetting.Find(KSpecialChar) != KErrNotFound)
		{
		return ConvertSpecialCharsL(tempType,ptrField,aColumn,aSetting,attribute);
		}

	ret = AssignFieldValuesL(aColumn,tempType,ptrField,aSetting,attribute);

	return ret?KErrNone:KErrNotFound;
	}


//
//ENUM handlers//
//



TBool DBAccess::ConvertConnectionPrefEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags /*aAttribute*/)
	{

	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;

	TInt i(0);
	TBuf<MAX_COL_NAME_LEN> column;
	column = ENUM_ConnectionPrefArray[i];

	if (aColumn.Compare(TPtrC(CONNECT_PREF_BEARER_SET)) == 0)
		{
        TLex lex(aSetting);

        // get the first token
        TBool err = CheckBearer(aPtrField, lex.NextToken());
        if ( err )
            {
            return EFalse;
            }

        TPtrC token = lex.NextToken();

        TBool exit = token.Length() > 0 && token.CompareF(_L("&")) == 0;

        while( exit )
            {
            err = CheckBearer(aPtrField, lex.NextToken());

            if( !err )
                {
                TPtrC token = lex.NextToken();
                exit = token.Length() > 0 && token.CompareF(_L("&")) == 0;
                }
            else
                {
                return EFalse;
                }
            }

		TUint32 pColumnV;
		pColumnV = *ptrTempField;
		gMsg->Dbg(_L("  Column match in enumeration list"));
		gMsg->Dbg(_L("  Column %S old value %S"), &aColumn, &aSetting);
		gMsg->Dbg(_L("  Column %S new value %d"), &aColumn, pColumnV);

		gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV); //todo remove
		return ETrue;
		}
	else
		{
		while (column.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if (column.Compare(aColumn) == 0)
				{
				TInt j(0);
				TBuf<MAX_COL_VAL_LEN> value;
				value = ENUM_ConnectionPrefValueArray[i][j];
				while(value.Compare(TPtrC(NO_MORE_RECORDS)))
					{
					if (value.Compare(aSetting) == 0)
						{
						*ptrTempField =   j;

						TUint32 pColumnV;
						pColumnV = *ptrTempField;
						gMsg->Dbg(_L("  Column match in enumeration list"));
						gMsg->Dbg(_L("  Column %S old value %S"), &aColumn, &aSetting);
						gMsg->Dbg(_L("  Column %S new value %d"), &column, pColumnV);

						gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV); //todo remove
						return ETrue;
						}
					j++;
					value = ENUM_ConnectionPrefValueArray[i][j];
					}
				}
			column = ENUM_ConnectionPrefArray[++i];
			}
		}

	return EFalse;
	}

TBool DBAccess::CheckBearer(CMDBElement* aPtrField,TPtrC aSetting)
    {
    TBool err(EFalse);

	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;

    // Check for CONNECT_PREF_BEARER_SET enums
  	if (aSetting.CompareF(TPtrC(BEARER_SET_UNKNOWN)) == 0)
		{
		*ptrTempField = KCommDbBearerUnknown | *ptrTempField;
		}
	else if (aSetting.CompareF(TPtrC(BEARER_SET_CSD)) == 0)
        {
        *ptrTempField = KCommDbBearerCSD | *ptrTempField;
        }
    else if (aSetting.CompareF(TPtrC(BEARER_SET_WCDMA)) == 0)
        {
        *ptrTempField = KCommDbBearerWcdma | *ptrTempField;
        }
    else if (aSetting.CompareF(TPtrC(BEARER_SET_LAN)) == 0)
        {
        *ptrTempField = KCommDbBearerLAN | *ptrTempField;
        }
    else if (aSetting.CompareF(TPtrC(BEARER_SET_VIRTUAL)) == 0)
        {
        *ptrTempField = KCommDbBearerVirtual | *ptrTempField;
        }
    else if (aSetting.CompareF(TPtrC(BEARER_SET_PAN)) == 0)
        {
        *ptrTempField = KCommDbBearerPAN | *ptrTempField;
        }
    else if (aSetting.CompareF(TPtrC(BEARER_SET_WLAN)) == 0)
        {
        *ptrTempField = KCommDbBearerWLAN | *ptrTempField;
        }
    else if (aSetting.CompareF(TPtrC(BEARER_SET_PSD)) == 0)
        {
        *ptrTempField = KCommDbBearerPSD | *ptrTempField;
        }
    else
        {
		gMsg->Msg(_L("ERR:  Token error [%S]"), &aSetting);
        err = ETrue;
        // default used because of BC
        *ptrTempField = KCommDbBearerCSD | KCommDbBearerPSD;
        }

    return err;
    }

TBool DBAccess::ConvertModemEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags /*aAttribute*/)
	{

	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;

	if (aColumn.Compare(TPtrC(MODEM_RATE)) == 0)
		{
		// Check for Modem RATE enums
		if (aSetting.Compare(TPtrC(MODEM_RATE_AUTO)) == 0)
			{
			*ptrTempField =   (TUint32)EBpsAutobaud;
			}
		else if (aSetting.Compare(TPtrC(MODEM_RATE_SPECIAL)) == 0)
			{
			/* Note use of EBpsSpecial which has the value 0x80000000
			strictly 0x80000000 is not a valid value for a enum
			as an enum is a int !.  It would be better if EBpsSpecial was
			defined as a different value */
			*ptrTempField =  (TUint32)EBpsSpecial;
			}
		else
			{
			TBuf<MAX_COL_NAME_LEN> columnValue;
			TUint32 i(0);
			columnValue = ENUM_MODEM_RATEArray[i];
			while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
				{
				if(aSetting.Compare(columnValue) == 0)
					{
					*ptrTempField =   i;
					break;
					}
				columnValue = ENUM_MODEM_RATEArray[++i];
				}
			}
		}
	else if(aColumn.Compare(TPtrC(MODEM_FAX_CLASS_PREF)) == 0)
		{

		// Check for MODEM_FAX_CLASS_PREF enums
		if (aSetting.Compare(TPtrC(MODEM_FAX_AUTO)) == 0)
			{
			*ptrTempField =   EClassAuto;
			}
		else if (aSetting.Compare(TPtrC(MODEM_FAX_1)) == 0)
			{
			*ptrTempField =   EClass1;
			}
		else if (aSetting.Compare(TPtrC(MODEM_FAX_2)) == 0)
			{
			*ptrTempField =   EClass2;
			}
		else if (aSetting.Compare(TPtrC(MODEM_FAX_2POINT0)) == 0)
			{
			*ptrTempField =   EClass2point0;
			}
		else if (aSetting.Compare(TPtrC(MODEM_FAX_1POINT0)) == 0)
			{
			*ptrTempField =   EClass1point0;
			}
		else
			{
			*ptrTempField =   EClass2point1;
			}
		}
	else
		{
		TInt i(0);
		TBuf<MAX_COL_NAME_LEN> column;
		column = ENUM_ModemBearerArray[i];

		while (column.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if (column.Compare(aColumn) == 0)
				{
				TInt j(0);
				TBuf<MAX_COL_VAL_LEN> value;
				value = ENUM_ModemValueArray[i][j];
				while(value.Compare(TPtrC(NO_MORE_RECORDS)))
					{
					if (value.Compare(aSetting) == 0)
						{
						*ptrTempField =   j;

						TUint32 pColumnV;
						pColumnV = *ptrTempField;
						gMsg->Dbg(_L("  Column match in enumeration list"));
						gMsg->Dbg(_L("  Column %S old value %S"), &aColumn, &aSetting);
						gMsg->Dbg(_L("  Column %S new value %d"), &column, j);
						gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV); //todo remove
						return ETrue;
						}
					j++;
					value = ENUM_ModemValueArray[i][j];
					}
				}
			column = ENUM_ModemBearerArray[++i];
			}
		return EFalse;
		}


	TUint32 pColumnV;
	pColumnV = *ptrTempField;

	gMsg->Dbg(_L("  Column match in enumeration list"));
	gMsg->Dbg(_L("  Column %S old value %S"), &aColumn, &aSetting);
	gMsg->Dbg(_L("  Column %S new value %d"), &aColumn, pColumnV);

	gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV); //todo remove
	return ETrue;
	}

TBool DBAccess::ConvertGPRSEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags /*aAttribute*/)
	{
	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;

	TBuf<MAX_COL_NAME_LEN> value;
	TInt j(0);

	if (aColumn.Compare(TPtrC(GPRS_R5_DATA_COMPRESSION)) == 0)
		{
		value = ENUM_GPRS_R5_DATA_COMPRESSIONArray[j];
		while (value.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if (value.Compare(aSetting) == 0)
				{
				*ptrTempField =   j;
				TUint32 pColumnV;
				pColumnV = *ptrTempField;
				gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV);
				return ETrue;
				}
			value = ENUM_GPRS_R5_DATA_COMPRESSIONArray[++j];
			}
		}
	else if (aColumn.Compare(TPtrC(GPRS_R5_HEADER_COMPRESSION)) == 0)
		{
		value = ENUM_GPRS_R5_HEADER_COMPRESSIONArray[j];
		while (value.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if (value.Compare(aSetting) == 0)
				{
				*ptrTempField =   j;
				TUint32 pColumnV;
				pColumnV = *ptrTempField;
				gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV);
				return ETrue;
				}
			value = ENUM_GPRS_R5_HEADER_COMPRESSIONArray[++j];
			}
		}

	else if (aColumn.Compare(TPtrC(GPRS_R4_PACKET_FLOW_IDENTIFIER)) == 0)
		{
		value = ENUM_GPRS_PACKET_FLOW_IDENTIFIERArray[j];
		while (value.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if (value.Compare(aSetting) == 0)
				{
				*ptrTempField =   j;
				TUint32 pColumnV;
				pColumnV = *ptrTempField;
				gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV);
				return ETrue;
				}
			value = ENUM_GPRS_PACKET_FLOW_IDENTIFIERArray[++j];
			}
		}

	else if (aColumn.Compare(TPtrC(GPRS_UMTS_GPRS_RELEASE)) == 0)
		{
		value = ENUM_GPRS_UMTS_GPRS_RELEASEArray[j];
		while (value.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if (value.Compare(aSetting) == 0)
				{
				*ptrTempField =   j;
				TUint32 pColumnV;
				pColumnV = *ptrTempField;
				gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV);
				return ETrue;
				}
			value = ENUM_GPRS_UMTS_GPRS_RELEASEArray[++j];
			}
		}

	return EFalse;
	}

TBool DBAccess::ConvertWAPEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags /*aAttribute*/)
	{
	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;
	if (aColumn.Compare(TPtrC(WAP_WSP_OPTION)) == 0)
		{
		TBuf<MAX_COL_NAME_LEN> value;
		TInt j(0);
		value = ENUM_WAP_WSP_OPTIONArray[j];
		while (value.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if (value.Compare(aSetting) == 0)
				{
				*ptrTempField =   j;
				TUint32 pColumnV;
				pColumnV = *ptrTempField;
				gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV);
				return ETrue;
				}
			value = ENUM_WAP_WSP_OPTIONArray[++j];
			}
		}
	return EFalse;
	}

TBool DBAccess::ConvertISPEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags /*aAttribute*/)
	{
	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;

	if (aColumn.Compare(TPtrC(ISP_BEARER_NAME)) == 0)
		{
		// Check for IFCALLBACKTYPE enums
		if (aSetting.Compare(TPtrC(BEARER_NAME_ASYNCHRONOUS)) == 0)
			{
			*ptrTempField =   RMobileCall::KCapsDataCircuitAsynchronous;
			}
		else if (aSetting.Compare(TPtrC(BEARER_NAME_ASYNCHRONOUSRDI)) == 0)
			{
			*ptrTempField =   RMobileCall::KCapsDataCircuitAsynchronousRDI;
			}
		else if (aSetting.Compare(TPtrC(BEARER_NAME_SYNCHRONOUS)) == 0)
			{
			*ptrTempField =   RMobileCall::KCapsDataCircuitSynchronous;
			}
		else if (aSetting.Compare(TPtrC(BEARER_NAME_SYNCHRONOUSRDI)) == 0)
			{
			*ptrTempField =   RMobileCall::KCapsDataCircuitSynchronousRDI;
			}
		else if (aSetting.Compare(TPtrC(BEARER_NAME_PADASYNCUDI)) == 0)
			{
			*ptrTempField =   RMobileCall::KCapsPADAsyncUDI;
			}
		else if (aSetting.Compare(TPtrC(BEARER_NAME_PADASYNCRDI)) == 0)
			{
			*ptrTempField =   RMobileCall::KCapsPADAsyncRDI;
			}
		else if (aSetting.Compare(TPtrC(BEARER_NAME_ACCESSSYNCUDI)) == 0)
			{
			*ptrTempField =   RMobileCall::KCapsPacketAccessSyncUDI;
			}
		else if (aSetting.Compare(TPtrC(BEARER_NAME_ACCESSSYNCRDI)) == 0)
			{
			*ptrTempField =   RMobileCall::KCapsPacketAccessSyncRDI;
			}
		else
			{
			*ptrTempField =   (TUint32)RMobileCall::KCapsServiceExtended;
			}
		}
	else if (aColumn.Compare(TPtrC(ISP_IF_CALLBACK_TYPE)) == 0)
		{
		// Check for IFCALLBACKTYPE enums
		if (aSetting.Compare(TPtrC(MSCBCPCLIENTNUM)) == 0)
			{
			*ptrTempField =   ECallbackActionMSCBCPRequireClientSpecifiedNumber;
			}
		else if (aSetting.Compare(TPtrC(MSCBCPSERVERNUM)) == 0)
			{
			*ptrTempField =   ECallbackActionMSCBCPAcceptServerSpecifiedNumber;
			}
		else if (aSetting.Compare(TPtrC(MSCBCPOVERRIDESER)) == 0)
			{
			*ptrTempField =   ECallbackActionMSCBCPOverrideServerSpecifiedNumber;
			}
		else
			{
			//todo Optimize or Cry!
			TBuf<MAX_COL_NAME_LEN> columnValue;
			TUint32 i(0);
			columnValue = ENUM_ISP_IFCALLBACKTYPEArray[i];
			while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
				{
				if(aSetting.Compare(columnValue) == 0)
					{
					*ptrTempField =   i;
					break;
					}
				columnValue = ENUM_ISP_IFCALLBACKTYPEArray[++i];
				}
			}
		}
	else if (aColumn.Compare(TPtrC(ISP_BEARER_PROTOCOL)) == 0)
		{

		// map the descriptor to an entry in RMobileCall::TMobileCallDataProtocol.
		// Can't just use the index, as the first two entries both mean
		// 'unspecified'.
		const TDescEnumMap *pMap = IspBearerProtocolMap;
		while((aSetting.Compare(TPtrC(pMap->OptionDescriptor)) != 0) && (pMap->EnumVal != RMobileCall::EProtocolUnspecified))
			{
			++pMap;
			}
		*ptrTempField =   pMap->EnumVal;
		}
	else
		{
		TInt i(0);
		TBuf<MAX_COL_NAME_LEN> column;
		column = ENUM_ISPArray[i];

		while (column.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if (column.Compare(aColumn) == 0)
				{
				TInt j(0);
				TBuf<MAX_COL_VAL_LEN> value;
				value = ENUM_ISPValueArray[i][j];
				while(value.Compare(TPtrC(NO_MORE_RECORDS)))
					{
					if (value.Compare(aSetting) == 0)
						{
						*ptrTempField =   j;

						TUint32 pColumnV;
						pColumnV = *ptrTempField;
						gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV);
						return ETrue;
						}
					j++;
					value = ENUM_ISPValueArray[i][j];
					}
				}
			column = ENUM_ISPArray[++i];
			}
		return EFalse;
		}

	TUint32 pColumnV;
	pColumnV = *ptrTempField;
	gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV);

	return ETrue;
	}


TBool DBAccess::ConvertMiscEnums(CMDBElement* aPtrField, TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags /*aAttribute*/)
	{
	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;

	TInt i(0);
	TBuf<MAX_COL_NAME_LEN> column;
	column = ENUM_MiscArray[i];



	//General Enums
	while (column.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
		{
		if (column.Compare(aColumn) == 0)
			{
			TInt j(0);
			TBuf<MAX_COL_VAL_LEN> value;
			value = ENUM_MiscValueArray[i][j];
			while(value.Compare(TPtrC(NO_MORE_RECORDS)))
				{
				if (value.Compare(aSetting) == 0)
					{
					*ptrTempField =   j;

					TUint32 pColumnV;
					pColumnV = *ptrTempField;
					gMsg->Dbg(_L("  Column match in enumeration list"));
					gMsg->Dbg(_L("  Column %S old value %S"), &aColumn, &aSetting);
					gMsg->Dbg(_L("  Column %S new value %d"), &column, pColumnV);

					gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV); //todo remove
					return ETrue;
					}
				j++;
				value = ENUM_MiscValueArray[i][j];
				}
			}
		column = ENUM_MiscArray[++i];
		}


	//Bearer Type Enums in Bearer tables
	if (aColumn.CompareF(TPtrC(BEARER_TECHNOLOGY)) == 0)
		{
		gMsg->Dbg(_L("  Column match in enumeration list"));
		if (aSetting.CompareF(TPtrC(BEARER_SET_UNKNOWN)) == 0)
			{
			*ptrTempField =   KCommDbBearerUnknown;
			}
		else if (aSetting.CompareF(TPtrC(BEARER_SET_CSD)) == 0)
			{
			*ptrTempField =   KCommDbBearerCSD;
			}
		else if (aSetting.CompareF(TPtrC(BEARER_SET_WCDMA)) == 0)
			{
			*ptrTempField =   KCommDbBearerWcdma;
			}
		else if (aSetting.CompareF(TPtrC(BEARER_SET_LAN)) == 0)
			{
			*ptrTempField =   KCommDbBearerLAN;
			}
		else if (aSetting.CompareF(TPtrC(BEARER_SET_PAN)) == 0)
			{
			*ptrTempField =   KCommDbBearerPAN;
			}
		else if (aSetting.CompareF(TPtrC(BEARER_SET_WLAN)) == 0)
			{
			*ptrTempField =   KCommDbBearerWLAN;
			}
	/*	else if (aSetting.CompareF(TPtrC(BEARER_SET_PSD)) == 0)
			{
			*ptrTempField =   ;
			}*/
		else if (aSetting.CompareF(TPtrC(BEARER_SET_VIRTUAL)) == 0)
			{
			*ptrTempField =   KCommDbBearerVirtual;
			}
		else
			{
			return EFalse;
			}
		TUint32 pColumnV;
		pColumnV = *ptrTempField;
		gMsg->Dbg(_L("  Column %S old value %S"), &aColumn, &aSetting);
		gMsg->Dbg(_L("  Column %S new value %d"), &column, pColumnV);
		gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV);
		return ETrue;
		}

	return EFalse;
	}

TBool DBAccess::ConvertPANEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags /*aAttribute*/)
	{
	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;

	TBool ret(EFalse);

	if ((aColumn.Compare(TPtrC(PAN_LOCAL_ROLE)) == 0) ||
		 (aColumn.Compare(TPtrC(PAN_PEER_ROLE)) == 0))
		{
		TBuf<MAX_COL_NAME_LEN> columnValue;
		TUint32 i(0);

		columnValue = ENUM_PAN_ROLEArray[i];

		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{

			if (aSetting.Compare(TPtrC(PAN_ROLE_U)) == 0)
				{
				*ptrTempField =   EPanRoleU;
				ret	= ETrue;
				}
			else if (aSetting.Compare(TPtrC(PAN_ROLE_GN)) == 0)
				{
				*ptrTempField =   EPanRoleGn;
				ret	= ETrue;
				}
			else if (aSetting.Compare(TPtrC(PAN_ROLE_NAP)) == 0)
				{
				*ptrTempField =   EPanRoleNap;
				ret	= ETrue;
				}
			else if (aSetting.Compare(TPtrC(PAN_ROLE_UNKNOWN)) == 0)
				{
				*ptrTempField =   EPanRoleUnknown;
				ret	= ETrue;
				}

			if(ret)
				{
				TUint32 pColumnV;
				pColumnV = *ptrTempField;
				gMsg->Msg(_L("  Column [%S] set to [%d]"), &aColumn, pColumnV);
				return ret;
				}
			columnValue = ENUM_PAN_ROLEArray[++i];
			}
		}

	return ret;
	}


TUint32 DBAccess::FindTableIdL(TDesC& aTableName)
	{
	TInt i(0);

	//TBuf<MAX_COL_VAL_LEN> table = tableArray[i];

	TBuf<MAX_COL_VAL_LEN> table = ( (iIsXML) ? xmlTableArray[i]
	                                         : tableArray[i] );

	while (table.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
		{
		if(table.CompareF(aTableName) == 0)
			{
			return elementIdArray[i];
			}
		//table = tableArray[++i];

		table = ( (iIsXML) ? xmlTableArray[++i]
	                       : tableArray[++i] );
		}

	//This means we havent found the table yet...so it could be a user defined record
	CMDBGenericRecord* genericRecord = static_cast<CMDBGenericRecord *>(CCDRecordBase::RecordFactoryL(0));
	CleanupStack::PushL(genericRecord);
	genericRecord->InitializeL(aTableName, NULL);
	genericRecord->LoadL(*iDbSession);
	TUint32 tableid = genericRecord->TableId();
	CleanupStack::PopAndDestroy(genericRecord);
	return tableid;
	}

TBool DBAccess::SetLinkedRecord(TUint32 aIndex, CMDBElement* aPtrField,TPtrC& aColumn, TPtrC& aSetting, TMDBAttributeFlags /*aAttribute*/)
	{
	TBool ret(EFalse);
	TBool linkByTag(EFalse);
	TUint32 i(0);

	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;

	while(TPtrC(LinkRecordsArray[aIndex][i]).CompareF(TPtrC(NO_MORE_RECORDS)) != 0)
		{
		if(aColumn.CompareF(TPtrC(LinkRecordsArray[aIndex][i++])) == 0)
			{
			//support for "Link.Table.Id" and "Table.Id" paterns
			const TUint KTableColumnSeperator = '.';
			TInt pos = aSetting.Locate(TChar(KTableColumnSeperator));

			if (pos != KErrNotFound)
				{
				TBuf<KCommsDbSvrMaxColumnNameLength> buffer = aSetting.Left(pos);

				if(buffer.CompareF(TPtrC(KTableLinkSeparator)) == 0)
					{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                    //found a link by tag linking. Instead of this the resolved TableName.RecId
                    //format should be used if the resolver object is present...
                    TBool foundThePair = EFalse;

                    if (iLnkByTagRes)
                    //ok, we have the resolver object
                        {
                        const HBufC* theResolvedPair = iLnkByTagRes->ResolvedIdForTagLink(aSetting);

                        if ( 0 < theResolvedPair->Size() )
                        //ok, found the pair, let's use this...
                            {
                            foundThePair = ETrue;

                            aSetting.Set(*theResolvedPair);
                            //search for the '.' character once again...
                            pos = aSetting.Locate(TChar(KTableColumnSeperator));

                            buffer = aSetting.Left(pos);

                            }
                        }

                    if (!foundThePair)
                    //hm.... Didn't find the pair for the link by tag linking. Let's try to handle
                    //it by this way...
                    {
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

					linkByTag = ETrue;

					// search for next separator that will define table name
					TPtrC tableName = aSetting.Mid(pos+1);

					TInt posTableName = tableName.Locate(TChar(KTableColumnSeperator));

					if (posTableName != KErrNotFound)
						{
						// extract table name to buffer
						buffer = aSetting.Mid(pos+1, posTableName);
						// adjust position
						pos += posTableName +1;
						}
					else
						{
						return ret;
						}
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
					}
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
					}

				TPtrC id(aSetting.Mid(pos + 1));

				TLex lex(id);
				TUint32 recordid;
				lex.Val(recordid,EDecimal);

				TUint32 linkedfield(0);
				//now we have a table name and the record id..
				TRAPD(err, linkedfield = FindTableIdL(buffer);)

				if(err == KErrNone)
					{
					if (linkByTag)
						{
						// for linked by tag format is Linked flag | TableId | Tag
						//linkedfield |= KLinkableFlagTag | recordid;
                        linkedfield = KLinkableFlagTag | recordid;
						}
					else
						{
						// for linked by record id format is ElementId
						linkedfield |= ((recordid << 8) & KCDMaskShowRecordId) | KCDMaskShowFieldType;
						}

					*ptrTempField = linkedfield;
					ret	= ETrue;
					}
				else
					{
					ret	= EFalse;
					}
				}
			}
		}

	if(ret)
		{
		TUint32 pColumnV = *ptrTempField;
		gMsg->Msg(_L("Column [%S] set to [%08x] was [%S]"), &aColumn, pColumnV, &aSetting);
		}

	return ret;
	}

TBool DBAccess::ConvertIapPrioritySelectionPolicyEnums(CMDBElement* /*aPtrField*/,TPtrC& aColumn, TPtrC& /*aSetting*/, TMDBAttributeFlags /*aAttribute*/)
	{
	TInt tempType(0);
	CMDBElement* ptrField = NULL;
	TRAPD(ret,(ptrField = (iTable->iRecords[iTable->iRecords.Count()-1])->GetFieldByNameL(TPtrC(KCDTypeNameIapCountName),tempType)));

	//if ok update counter, otherwise ignore everything
	if (ret==KErrNone)
	    {
	    CMDBField<TUint32>* ptrTempField = (CMDBField<TUint32>*)ptrField;
    	if (aColumn.Compare(TPtrC(KCDTypeNameIap1Name)) == 0)
			{
    		ASSERT(*ptrTempField==0);
			*ptrTempField = 1;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap2Name)) == 0)
			{
			ASSERT(*ptrTempField==1);
    	    *ptrTempField = 2;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap3Name)) == 0)
			{
			ASSERT(*ptrTempField==2);
            *ptrTempField = 3;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap4Name)) == 0)
			{
			ASSERT(*ptrTempField==3);
            *ptrTempField = 4;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap5Name)) == 0)
			{
			ASSERT(*ptrTempField==4);
            *ptrTempField = 5;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap6Name)) == 0)
			{
			ASSERT(*ptrTempField==5);
            *ptrTempField = 6;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap7Name)) == 0)
			{
			ASSERT(*ptrTempField==6);
            *ptrTempField = 7;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap8Name)) == 0)
			{
			ASSERT(*ptrTempField==7);
            *ptrTempField = 8;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap9Name)) == 0)
			{
			ASSERT(*ptrTempField==8);
            *ptrTempField = 9;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap10Name)) == 0)
			{
			ASSERT(*ptrTempField==9);
            *ptrTempField = 10;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap11Name)) == 0)
			{
			ASSERT(*ptrTempField==10);
            *ptrTempField = 11;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap12Name)) == 0)
			{
			ASSERT(*ptrTempField==11);
            *ptrTempField = 12;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap13Name)) == 0)
			{
			ASSERT(*ptrTempField==12);
            *ptrTempField = 13;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap14Name)) == 0)
			{
			ASSERT(*ptrTempField==13);
            *ptrTempField = 14;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameIap15Name)) == 0)
			{
			ASSERT(*ptrTempField==14);
            *ptrTempField = 15;
			}
	    }

	//always return EFalse, as we only update the IAP counter here.
	return EFalse;
	}
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
TBool DBAccess::ConvertApPrioritySelectionPolicyEnums(CMDBElement* /*aPtrField*/,TPtrC& aColumn, TPtrC& /*aSetting*/, TMDBAttributeFlags /*aAttribute*/)
	{
	TInt tempType(0);
	CMDBElement* ptrField = NULL;
	TRAPD(ret,(ptrField = (iTable->iRecords[iTable->iRecords.Count()-1])->GetFieldByNameL(TPtrC(KCDTypeNameApCountName),tempType)));

	//if ok update counter, otherwise ignore everything
	if (ret==KErrNone)
	    {
	    CMDBField<TUint32>* ptrTempField = (CMDBField<TUint32>*)ptrField;
    	if (aColumn.Compare(TPtrC(KCDTypeNameAp1Name)) == 0)
			{
    		ASSERT(*ptrTempField==0);
			*ptrTempField = 1;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp2Name)) == 0)
			{
			ASSERT(*ptrTempField==1);
    	    *ptrTempField = 2;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp3Name)) == 0)
			{
			ASSERT(*ptrTempField==2);
            *ptrTempField = 3;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp4Name)) == 0)
			{
			ASSERT(*ptrTempField==3);
            *ptrTempField = 4;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp5Name)) == 0)
			{
			ASSERT(*ptrTempField==4);
            *ptrTempField = 5;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp6Name)) == 0)
			{
			ASSERT(*ptrTempField==5);
            *ptrTempField = 6;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp7Name)) == 0)
			{
			ASSERT(*ptrTempField==6);
            *ptrTempField = 7;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp8Name)) == 0)
			{
			ASSERT(*ptrTempField==7);
            *ptrTempField = 8;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp9Name)) == 0)
			{
			ASSERT(*ptrTempField==8);
            *ptrTempField = 9;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp10Name)) == 0)
			{
			ASSERT(*ptrTempField==9);
            *ptrTempField = 10;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp11Name)) == 0)
			{
			ASSERT(*ptrTempField==10);
            *ptrTempField = 11;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp12Name)) == 0)
			{
			ASSERT(*ptrTempField==11);
            *ptrTempField = 12;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp13Name)) == 0)
			{
			ASSERT(*ptrTempField==12);
            *ptrTempField = 13;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp14Name)) == 0)
			{
			ASSERT(*ptrTempField==13);
            *ptrTempField = 14;
			}
    	else if (aColumn.Compare(TPtrC(KCDTypeNameAp15Name)) == 0)
			{
			ASSERT(*ptrTempField==14);
            *ptrTempField = 15;
			}
	    }

	//always return EFalse, as we only update the IAP counter here.
	return EFalse;
	}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

TBool DBAccess::IsConnectionPrefValid(CCDConnectionPrefsRecord& aConnectionPrefRecord)
	{

	gMsg->Dbg(_L("  Dialog preference set to %d"), (TInt)aConnectionPrefRecord.iDialogPref);
	gMsg->Dbg(_L("  IAP ID set to %d"), (TInt)aConnectionPrefRecord.iDefaultIAP);
	gMsg->Dbg(_L("  Bearer Direction set to %d"),(TInt)aConnectionPrefRecord.iDirection);
	gMsg->Dbg(_L("  Bearer Set set to %d"), (TInt)aConnectionPrefRecord.iBearerSet);


	gMsg->Dbg(_L("  Checking preference fields..."));

	if (aConnectionPrefRecord.iRanking == BADNUMVAL ||
		aConnectionPrefRecord.iDirection == ECommDbConnectionDirectionUnknown ||
		(TCommDbDialogPref&)aConnectionPrefRecord.iDialogPref == ECommDbDialogPrefUnknown ||
		aConnectionPrefRecord.iBearerSet == BADNUMVAL ||
		aConnectionPrefRecord.iDefaultIAP == BADNUMVAL)
		{
		iLastError = E_NOTENOUGHDATA;
		LastErrorMessage.Set(errorCode[E_NOTENOUGHDATA]);
		return EFalse;
		}

	gMsg->Msg(_L("All preference fields valid"));
	return ETrue;
	}

TBool DBAccess::ConvertWLANEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags /*aAttribute*/)
	{
	TBool ret(EFalse);

	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;

	if (aColumn.Compare(TPtrC(WLAN_ENCRYPTION_TYPE)) == 0)
		{
		TBuf<MAX_COL_NAME_LEN> columnValue;
		TUint32 i(0);
		columnValue = ENUM_WLAN_ENCRYPTION_TYPEArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
		    {
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_ENCRYPTION_TYPE_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_ENCRYPTION_TYPEArray[i];
			}
		}
	else if (aColumn.Compare(TPtrC(WLAN_DIALOG_PREF)) == 0)
		{
		TUint32 i(0);
		TBuf<MAX_COL_NAME_LEN> columnValue = ENUM_WLAN_DIALOG_PREFArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_DIALOG_PREF_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_DIALOG_PREFArray[i];
			}
		}
	else if (aColumn.Compare(TPtrC(WLAN_NETWORK_TYPE)) == 0)
		{
		TUint32 i(0);
		TBuf<MAX_COL_NAME_LEN> columnValue = ENUM_WLAN_NETWORK_TYPEArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_NETWORK_TYPE_VALArray[i];;
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_NETWORK_TYPEArray[i];
			}
		}
	else if (aColumn.Compare(TPtrC(WLAN_DES_TRANS_RATE)) == 0)
		{
		TUint32 i(0);
		TBuf<MAX_COL_NAME_LEN> columnValue = ENUM_WLAN_DES_TRANS_RATEArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_DES_TRANS_RATE_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_DES_TRANS_RATEArray[i];
			}
		}
	else if (aColumn.Compare(TPtrC(WLAN_POWERSAVE_MODE)) == 0)
		{
		TUint32 i(0);
		TBuf<MAX_COL_NAME_LEN> columnValue = ENUM_WLAN_POWERSAVE_MODEArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_POWERSAVE_MODE_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_POWERSAVE_MODEArray[i];
			}
		}
	else if (aColumn.Compare(TPtrC(WLAN_PREAMBLE_TYPE)) == 0)
		{
		TUint32 i(0);
		TBuf<MAX_COL_NAME_LEN> columnValue = ENUM_WLAN_PREAMBLE_TYPEArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_PREAMBLE_TYPE_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_PREAMBLE_TYPEArray[i];
			}
		}
	else if (aColumn.Compare(TPtrC(WLAN_REG_DOMAIN)) == 0 || aColumn.Compare(TPtrC(REGULATORY_DOMAIN)) == 0)
		{
		TUint32 i(0);
		TBuf<MAX_COL_NAME_LEN> columnValue = ENUM_WLAN_REG_DOMAINArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_REG_DOMAIN_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_REG_DOMAINArray[i];
			}
		}
	else if (aColumn.Compare(TPtrC(DEFAULT_REGULATORY_DOMAIN)) == 0)
		{
		TUint32 i(0);
		TBuf<MAX_COL_NAME_LEN> columnValue = ENUM_WLAN_REG_DOMAINArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_REG_DOMAIN_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_REG_DOMAINArray[i];
			}
		}
	else if (aColumn.Compare(TPtrC(WLAN_AUTHENTICATION_MODE)) == 0)
		{
		TUint32 i(0);
		TBuf<MAX_COL_NAME_LEN> columnValue = ENUM_WLAN_AUTHENTICATION_MODEArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_AUTHENTICATION_MODE_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_AUTHENTICATION_MODEArray[i];
			}
		}
	else if (aColumn.Compare(TPtrC(WLAN_ENCRYPTION_STATUS)) == 0)
		{
		TUint32 i(0);
		TBuf<MAX_COL_NAME_LEN> columnValue = ENUM_WLAN_ENCRYPTION_STATUSArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_BOOLEAN_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_ENCRYPTION_STATUSArray[i];
			}
		}
	else if (aColumn.Compare(TPtrC(WLAN_ALLOW_UNENCRYPT_MODE)) == 0)
		{
		TUint32 i(0);
		TBuf<MAX_COL_NAME_LEN> columnValue = ENUM_WLAN_ALLOW_UNENCRYPT_MODEArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_BOOLEAN_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_ALLOW_UNENCRYPT_MODEArray[i];
			}
		}
	else if (aColumn.Compare(TPtrC(WLAN_ALLOW_SHORT_PRE)) == 0)
		{
		TUint32 i(0);
		TBuf<MAX_COL_NAME_LEN> columnValue = ENUM_WLAN_ALLOW_SHORT_PREArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_WLAN_BOOLEAN_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_WLAN_ALLOW_SHORT_PREArray[i];
			}
		}

	if(ret)
		{
		TUint32 pColumnV;
		pColumnV = *ptrTempField;
		gMsg->Msg(_L("  Column %S with enum value %S set to %d"), &aColumn, &aSetting, pColumnV);
		}
	return ret;
	}

TBool DBAccess::ConvertEAPSecEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags /*aAttribute*/)
	{
	TBool ret(EFalse);
	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;

	if (aColumn.Compare(TPtrC(EAP_SEC_OUTER_EAP_TYPE)) == 0)
		{
		TBuf<MAX_COL_NAME_LEN> columnValue;
		TUint32 i(0);
		columnValue = ENUM_EAP_SEC_OUTER_EAP_TYPEArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
		    {
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_EAP_SEC_OUTER_EAP_TYPE_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_EAP_SEC_OUTER_EAP_TYPEArray[i];
			}
		}
	if(ret)
		{
		TUint32 pColumnV;
		pColumnV = *ptrTempField;
		gMsg->Msg(_L("  Column %S with enum value %S set to %d"), &aColumn, &aSetting, pColumnV);
		}
	return ret;
	}

TBool DBAccess::ConvertTUNEAPSecEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags /*aAttribute*/)
	{

	TBool ret(EFalse);
	CMDBField<TUint32>* ptrTempField  = (CMDBField<TUint32>*)aPtrField;

	if (aColumn.Compare(TPtrC(TUN_EAP_INNERTYPE)) == 0)
		{
		TBuf<MAX_COL_NAME_LEN> columnValue;
		TUint32 i(0);
		columnValue = ENUM_TUN_EAP_INNERTYPEArray[i];
		while (columnValue.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
		    {
			if(aSetting.Compare(columnValue) == 0)
				{
				*ptrTempField = ENUM_TUN_EAP_INNERTYPE_VALArray[i];
				ret = ETrue;
				break;
				}
			i++;
			columnValue = ENUM_TUN_EAP_INNERTYPEArray[i];
			}
		}
	if(ret)
		{
		TUint32 pColumnV;
		pColumnV = *ptrTempField;
		gMsg->Msg(_L("  Column %S with enum value %S set to %d"), &aColumn, &aSetting, pColumnV);
		}
	return ret;
	}

TBool DBAccess::TemplateFieldPresentL(TMDBElementId aElementId)
/**
Check if a particular field is present in the template record (if any) for the current table.

@param aElementId table and column id of field to check
@return ETrue if field exists, else EFalse
*/
	{
	// We assume that iTable has been setup with at least the first record of the current table
	// being processed.
	if (iTable && iTable->iRecords.Count() > 0)
		{
		const TInt KFirstRecord = 0;
		const TUint KTemplateRecordId = 0;
		const TMDBElementId id = iTable->iRecords[KFirstRecord]->ElementId();
		// Ccheck if the first record is a template record of the requested table.
		if ((id & KCDMaskShowRecordId)   == KTemplateRecordId &&
			(id & KCDMaskShowRecordType) == (aElementId & KCDMaskShowRecordType))
			{
			// now check that the field itself is present and is set to a non-null value
			CMDBElement *el = NULL;
			TRAPD(err, el = iTable->iRecords[0]->GetFieldByIdL(aElementId));
			return err == KErrNone && el && !el->IsNull();
			}
		}
	return EFalse;
	}
