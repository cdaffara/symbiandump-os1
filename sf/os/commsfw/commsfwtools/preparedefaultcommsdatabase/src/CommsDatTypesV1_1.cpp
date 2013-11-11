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
// Comms Data Type Definitions version 1.1
// Definitions of Symbian OS Comms Data Types that can be stored in the Comms Repository
// Type Definitions are version controlled.  New versions may be introduced from time to time.
// Then older versions are marked as deprecated and given a planned removal date.
// This version introduced with Release 9.1
// 
//

/**
 @file
*/



#include <e32base.h>
#include <e32std.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdattypesv1_1_partner.h>
#include <commsdattypesv1_1_internal.h>
#include <commsdat_internal.h>
#include <commsdat_partner.h>
#endif

// Internal headers
#include "MetaDatabase.h"
#include "CommsDat.h"
#include "CommsDatInternalDefs.h"
#include "MetaDatabaseVisitor.h"
#include "CommsDatTypesV1_1.h"
#include <comms-infras/commsdatschema.h>




using namespace CommsDat;
using namespace CommsDatInternal;
using namespace Meta;



EXPORT_C CMDBRecordBase* CCDRecordBase::RecordFactoryL(TMDBElementId aId)
    {
    const TMDBElementId recordType = aId & KCDMaskShowRecordType;

    CCDRecordBase * ptr = NULL;
    switch (recordType)
        {            
        case KCDTIdConnectionPrefsRecord :
            {
            ptr = new(ELeave)CCDConnectionPrefsRecord(aId);
            break;
            }
        case KCDTIdGlobalSettingsRecord :
            {
            ptr = new(ELeave)CCDGlobalSettingsRecord(aId);
            break;
            }
        case KCDTIdNetworkRecord :
            {
            ptr = new(ELeave)CCDNetworkRecord(aId);
            break;
            }
        case KCDTIdLocationRecord :
            {
            ptr = new(ELeave)CCDLocationRecord(aId);
            break;
            }
        case KCDTIdIAPRecord :
            {
            ptr = new(ELeave)CCDIAPRecord(aId);
            break;
            }
        case KCDTIdWAPAccessPointRecord :
            {
            ptr = new(ELeave)CCDWAPAccessPointRecord(aId);
            break;
            }
        case KCDTIdDialOutISPRecord :
            {
            ptr = new(ELeave)CCDDialOutISPRecord(aId);
            break;
            }
        case KCDTIdDialInISPRecord :
            {
            ptr = new(ELeave)CCDDialInISPRecord(aId);
            break;
            }
        case KCDTIdLANServiceRecord :
            {
            ptr = new(ELeave)CCDLANServiceRecord(aId);
            break;
            }
        case KCDTIdWLANServiceExtRecord :
            {
            ptr = new(ELeave)CCDWLANServiceExtRecord(aId);
            break;
            }
        case KCDTIdVPNServiceRecord :
            {
            ptr = new(ELeave)CCDVPNServiceRecord(aId);
            break;
            }
        case KCDTIdWCDMAPacketServiceRecord :
            {
            ptr = new(ELeave)CCDWCDMAPacketServiceRecord(aId);
            break;
            }
        case KCDTIdOutgoingGprsRecord :
            {
            ptr = new(ELeave)CCDOutgoingGprsRecord(aId);
            break;
            }
        case KCDTIdIncomingGprsRecord :
            {
            ptr = new(ELeave)CCDIncomingGprsRecord(aId);
            break;
            }
        case KCDTIdDefaultWCDMARecord :
            {
            ptr = new(ELeave)CCDDefaultWCDMARecord(aId);
            break;
            }
        case KCDTIdModemBearerRecord :
            {
            ptr = new(ELeave)CCDModemBearerRecord(aId);
            break;
            }
        case KCDTIdLANBearerRecord :
            {
            ptr = new(ELeave)CCDLANBearerRecord(aId);
            break;
            }
        case KCDTIdVirtualBearerRecord :
            {
            ptr = new(ELeave)CCDVirtualBearerRecord(aId);
            break;
            }
        case KCDTIdWAPSMSBearerRecord :
            {
            ptr = new(ELeave)CCDWAPSMSBearerRecord(aId);
            break;
            }
        case KCDTIdWAPIPBearerRecord :
            {
            ptr = new(ELeave)CCDWAPIPBearerRecord(aId);
            break;
            }
        case KCDTIdChargecardRecord :
            {
            ptr = new(ELeave)CCDChargecardRecord(aId);
            break;
            }
        case KCDTIdProxiesRecord :
            {
            ptr = new(ELeave)CCDProxiesRecord(aId);
            break;
            }
        case KCDTIdSSProtoRecord :
            {
            ptr = new(ELeave)CCDSecureSocketRecord(aId);
            break;
            }
        case KCDTIdAgentLookupRecord :
            {
            ptr = new(ELeave)CCDAgentLookupRecord(aId);
            break;
            }
        case KCDTIdPANServiceExtRecord :
            {
            ptr = new(ELeave)CCDPANServiceExtRecord(aId);
            break;
            }
        case KCDTIdAccessPointRecord :
            {
            ptr = new(ELeave)CCDAccessPointRecord(aId);
            break;
            }
		case KCDTIdIapPrioritySelectionPolicyRecord :
            {
            ptr = new(ELeave)CCDIAPPrioritySelectionPolicyRecord(aId);
            break;
            }
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY        
        case KCDTIdApPrioritySelectionPolicyRecord :
            {
            ptr = new(ELeave)CCDAPPrioritySelectionPolicyRecord(aId);
            break;
            }
#endif
        case KCDTIdUmtsR99QoSAndOnTableRecord :
			{
			ptr = new(ELeave)CCDUmtsR99QoSAndOnTableRecord(aId);
			break;
            }

        case KCDTIdEAPSecRecord :
            {
            ptr = new(ELeave)CCDEAPSecRecord(aId);
            break;
            }
        case KCDTIdTunEAPRecord :
            {
            ptr = new(ELeave)CCDTunEAPRecord(aId);
            break;
            }
        case KCDTIdEAPTLSRecord :
            {
            ptr = new(ELeave)CCDEAPTLSRecord(aId);
            break;
            }
        case KCDTIdLEAPRecord :
            {
            ptr = new(ELeave)CCDLEAPRecord(aId);
            break;
            }
		case KCDTIdEapSimProtocolRecord :
            {
            ptr = new(ELeave)CCDEapSimProtocolRecord(aId);
            break;
            }
        case KCDTIdEapAkaProtocolRecord :
            {
            ptr = new(ELeave)CCDEapAkaProtocolRecord(aId);
            break;
            }             
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        case KCDTIdTierRecord :
            {
            ptr = new(ELeave)CCDTierRecord(aId);
            break;
            }
        case KCDTIdMCprRecord :
            {
            ptr = new(ELeave)CCDMCprRecord(aId);
            break;
            }
        case KCDTIdCprRecord :
            {
            ptr = new(ELeave)CCDCprRecord(aId);
            break;
            }
        case KCDTIdSCprRecord :
            {
            ptr = new(ELeave)CCDSCprRecord(aId);
            break;
            }
        case KCDTIdProtocolRecord :
            {
            ptr = new(ELeave)CCDProtocolRecord(aId);
            break;
            }
        case KCDTIdBearerTypeRecord :
            {
            ptr = new(ELeave)CCDBearerTypeRecord(aId);
            break;
            }
        case KCDTIdConfigAccessPointRecord :
	        {
	        ptr = new(ELeave)CCDConfigAccessPointRecord(aId);
            break;
	        }
        case KCDTIdPolicySelector2ParamsRecord :
			{
			ptr = new(ELeave)CCDPolicySelector2ParamsRecord(aId);
            break;
			}
        case KCDTIdGenericQosRecord :
            {
            ptr = new(ELeave)CCDGenericQosRecord(aId);
            break;
            }
        case KCDTIdPolicySelectorRecord :
            {
            ptr = new(ELeave)CCDPolicySelectorRecord(aId);
            break;
            }

        case KCDTIdWifiScanEngineRecord :
            {
            return new(ELeave)CCDWifiScanEngineRecord(aId);
            }
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

        case 0x00000000 :
        case KCDNewRecordRequest :
            {
            // Both meaning, I wish to create a new generic table for which I don't yet have an Id.
            return new(ELeave)CMDBGenericRecord(aId);
            }
        default :
            {
            if (recordType >= KCDInitialUDefRecordType && recordType <= KCDLastUDefRecordType)
                {
                return new(ELeave)CMDBGenericRecord(aId); // return.  
                }
            else
                {
                User::Leave(KErrNotFound);
                }
            }
        }
        
    ptr->ConstructL();

    return ptr;
    }


EXPORT_C CMDBRecordBase* CCDRecordBase::CreateCopyRecordL(CMDBRecordBase& aCopyFromRecord)
	{
	//create the duplicate type
	CMDBRecordBase* ptrCopyToRecord = static_cast<CCDRecordBase&>(aCopyFromRecord).RecordFactoryL(aCopyFromRecord.ElementId() & KCDMaskShowRecordType);
	CleanupStack::PushL(ptrCopyToRecord);

	//copy the values
	const SRecordTypeInfo* recordInfo = aCopyFromRecord.GetRecordInfo();
   	
    if ( recordInfo == NULL)
        {
        User::Leave(KErrNotFound);
        }

    CMDBElement* ptrCopyFrom = NULL;
   	CMDBElement* ptrCopyTo = NULL;
    
    while( recordInfo != NULL && recordInfo->iTypeId != 0)
        {
        ptrCopyFrom  = aCopyFromRecord.GetFieldByIdL(recordInfo->iTypeId);
        ptrCopyTo    = ptrCopyToRecord->GetFieldByIdL(recordInfo->iTypeId);

        switch(recordInfo->iValType & KCDShowBasicTypeInfo) // because want to ignore linked record details
            {
        	case EInt:
        	case EBool:
               	{
        		static_cast<CMDBField<TInt>&>(*ptrCopyTo).SetL( static_cast<CMDBField<TInt>&>(*ptrCopyFrom) );
        		}
        		break;
        	case EUint32:
            case ELink:
        		{
        		static_cast<CMDBField<TUint32>&>(*ptrCopyTo).SetL( static_cast<CMDBField<TUint32>&>(*ptrCopyFrom) );
                break;
                }
        	case EDesC8:
        		{
        		static_cast<CMDBField<TDesC8>&>(*ptrCopyTo).SetL( static_cast<CMDBField<TDesC8>&>(*ptrCopyFrom) );
        		}
        		break;
        	case EText:
           	case EMedText:
        	case ELongText:
        		{
        		static_cast<CMDBField<TDesC>&>(*ptrCopyTo).SetL( static_cast<CMDBField<TDesC>&>(*ptrCopyFrom) );
        		}
        		break;
        	default:
        		User::Leave(KErrNotFound);

            }
        ptrCopyTo->SetAttributes(ptrCopyFrom->Attributes());
        ++recordInfo;
        }
       
	CleanupStack::Pop(ptrCopyToRecord);
	return 	ptrCopyToRecord;
	}



void CCDRecordBase::ConstructL()
    /* Need to initialise the element ids of each member of a CCDRecord class. 
    It takes up huge amount of code to do this in a constructor so just iterate 
    through netmetawise and set each elementid*/
    {
    // NB this will not populate the 
    const SRecordTypeInfo* recordInfo = GetRecordInfo();

    ASSERT(recordInfo != NULL);

    TMetaVTableIterator attribIter(this);
    SVDataTableEntry const* entry = NULL;

    // for bc reasons, keeping base class data at start of static tables so iterate past this
    ++recordInfo;
    ++recordInfo;
    
    while ( recordInfo->iTypeId != 0  && (entry = attribIter++) != NULL ) // recordInfo will get to end first because of name and tag at beginning
        {  
	    CMDBElement* fieldPtr = reinterpret_cast<CMDBElement*>(GetAttribPtr(entry->iOffset));

		ASSERT(fieldPtr != NULL);
        
        //Set the table, 
        fieldPtr->SetElementId( recordInfo->iTypeId  | (ElementId() & ~KCDMaskShowFieldType) );
         
        ++recordInfo;
        }
    }


// Initialisation of static members

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
const SRecordTypeInfo* const CCDParamsRecordBase::iRecordInfo = CommsDatSchemaV1_1::iParamsRecordBaseInfo;
#endif
const SRecordTypeInfo* const CCDServiceRecordBase::iRecordInfo = CommsDatSchemaV1_1::iServiceRecordBaseInfo;
const SRecordTypeInfo* const CCDBearerRecordBase::iRecordInfo = CommsDatSchemaV1_1::iBearerRecordBaseInfo;
const SRecordTypeInfo* const CCDConnectionPrefsRecord::iRecordInfo = CommsDatSchemaV1_1::iConnectionPrefsRecordInfo;
const SRecordTypeInfo* const CCDGlobalSettingsRecord::iRecordInfo = CommsDatSchemaV1_1::iGlobalSettingsRecordInfo;
const SRecordTypeInfo* const CCDNetworkRecord::iRecordInfo = CommsDatSchemaV1_1::iNetworkRecordInfo;
const SRecordTypeInfo* const CCDLocationRecord::iRecordInfo = CommsDatSchemaV1_1::iLocationRecordInfo;
const SRecordTypeInfo* const CCDIAPRecord::iRecordInfo = CommsDatSchemaV1_1::iIAPRecordInfo;
const SRecordTypeInfo* const CCDWAPAccessPointRecord::iRecordInfo = CommsDatSchemaV1_1::iWAPAccessPointRecordInfo;
const SRecordTypeInfo* const CCDDialOutISPRecord::iRecordInfo = CommsDatSchemaV1_1::iDialOutISPRecordInfo;
const SRecordTypeInfo* const CCDDialInISPRecord::iRecordInfo = CommsDatSchemaV1_1::iDialInISPRecordInfo;
const SRecordTypeInfo* const CCDLANServiceRecord::iRecordInfo = CommsDatSchemaV1_1::iLANServiceRecordInfo;
const SRecordTypeInfo* const CCDWLANServiceExtRecord::iRecordInfo = CommsDatSchemaV1_1::iWLANServiceExtRecordInfo;
const SRecordTypeInfo* const CCDVPNServiceRecord::iRecordInfo = CommsDatSchemaV1_1::iVPNServiceRecordInfo;
const SRecordTypeInfo* const CCDWCDMAPacketServiceRecord::iRecordInfo = CommsDatSchemaV1_1::iWCDMAPacketServiceRecordInfo;
const SRecordTypeInfo* const CCDDefaultWCDMARecord::iRecordInfo = CommsDatSchemaV1_1::iDefaultWCDMARecordInfo;
const SRecordTypeInfo* const CCDModemBearerRecord::iRecordInfo = CommsDatSchemaV1_1::iModemBearerRecordInfo;
const SRecordTypeInfo* const CCDLANBearerRecord::iRecordInfo = CommsDatSchemaV1_1::iLANBearerRecordInfo;
const SRecordTypeInfo* const CCDVirtualBearerRecord::iRecordInfo = CommsDatSchemaV1_1::iVirtualBearerRecordInfo;
const SRecordTypeInfo* const CCDWAPSMSBearerRecord::iRecordInfo = CommsDatSchemaV1_1::iWAPSMSBearerRecordInfo;
const SRecordTypeInfo* const CCDWAPIPBearerRecord::iRecordInfo = CommsDatSchemaV1_1::iWAPIPBearerRecordInfo;
const SRecordTypeInfo* const CCDChargecardRecord::iRecordInfo = CommsDatSchemaV1_1::iChargecardRecordInfo;
const SRecordTypeInfo* const CCDProxiesRecord::iRecordInfo = CommsDatSchemaV1_1::iProxiesRecordInfo;
const SRecordTypeInfo* const CCDSecureSocketRecord::iRecordInfo = CommsDatSchemaV1_1::iSecureSocketRecordInfo;
const SRecordTypeInfo* const CCDAgentLookupRecord::iRecordInfo = CommsDatSchemaV1_1::iAgentLookupRecordInfo;
const SRecordTypeInfo* const CCDPANServiceExtRecord::iRecordInfo = CommsDatSchemaV1_1::iPANServiceExtRecordInfo;
const SRecordTypeInfo* const CCDAccessPointRecord::iRecordInfo = CommsDatSchemaV1_1::iAccessPointRecordInfo;
const SRecordTypeInfo* const CCDIAPPrioritySelectionPolicyRecord::iRecordInfo = CommsDatSchemaV1_1::iIAPPrioritySelectionPolicyRecordInfo;
const SRecordTypeInfo* const CCDUmtsR99QoSAndOnTableRecord::iRecordInfo = CommsDatSchemaV1_1::iUmtsR99QoSAndOnTableRecordInfo;
const SRecordTypeInfo* const CCDEAPSecRecord::iRecordInfo = CommsDatSchemaV1_1::iEAPSecRecordInfo;
const SRecordTypeInfo* const CCDTunEAPRecord::iRecordInfo = CommsDatSchemaV1_1::iTunEAPRecordInfo;
const SRecordTypeInfo* const CCDEAPTLSRecord::iRecordInfo = CommsDatSchemaV1_1::iEAPTLSRecordInfo;
const SRecordTypeInfo* const CCDLEAPRecord::iRecordInfo = CommsDatSchemaV1_1::iLEAPRecordInfo;
const SRecordTypeInfo* const CCDEapSimProtocolRecord::iRecordInfo = CommsDatSchemaV1_1::iEAPSIMRecordInfo;
const SRecordTypeInfo* const CCDEapAkaProtocolRecord::iRecordInfo = CommsDatSchemaV1_1::iEAPAKARecordInfo;

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
const SRecordTypeInfo* const CCDTierRecord::iRecordInfo = CommsDatSchemaV1_1::iTierRecordInfo;
const SRecordTypeInfo* const CCDMCprRecord::iRecordInfo = CommsDatSchemaV1_1::iMCprRecordInfo;
const SRecordTypeInfo* const CCDCprRecord::iRecordInfo = CommsDatSchemaV1_1::iCprRecordInfo;
const SRecordTypeInfo* const CCDSCprRecord::iRecordInfo = CommsDatSchemaV1_1::iSCprRecordInfo;
const SRecordTypeInfo* const CCDProtocolRecord::iRecordInfo = CommsDatSchemaV1_1::iProtocolRecordInfo;
const SRecordTypeInfo* const CCDBearerTypeRecord::iRecordInfo = CommsDatSchemaV1_1::iBearerTypeRecordInfo;
const SRecordTypeInfo* const CCDConfigAccessPointRecord::iRecordInfo = CommsDatSchemaV1_1::iConfigAccessPointRecordInfo;
const SRecordTypeInfo* const CCDAPPrioritySelectionPolicyRecord::iRecordInfo = CommsDatSchemaV1_1::iAPPrioritySelectionPolicyRecordInfo;
const SRecordTypeInfo* const CCDPolicySelectorRecord::iRecordInfo = CommsDatSchemaV1_1::iPolicySelectorRecordInfo;
const SRecordTypeInfo* const CCDPolicySelector2ParamsRecord::iRecordInfo = CommsDatSchemaV1_1::iPolicySelector2ParamsRecordInfo;
const SRecordTypeInfo* const CCDGenericQosRecord::iRecordInfo = CommsDatSchemaV1_1::iGenericQosRecordInfo;
const SRecordTypeInfo* const CCDWifiScanEngineRecord::iRecordInfo = CommsDatSchemaV1_1::iWifiScanEngineRecordInfo;

#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

// Base class netmeta ids for groups of records - not related to ktid of tables
#define KCDTIdRecordBase					600
#define KCDTIdServiceRecordBase				610
#define KCDTIdBearerRecordBase				620
#define KCDTIdISPRecordBase					630
#define KCDTIdSelectionPolicyRecordBase   	640
#define KCDTIdBaseSecRecord 				650
#define KCDTIdParamsRecordBase				660




// CCDRecordBase
EXP_START_ATTRIBUTE_TABLE(      CCDRecordBase,      KCDTIdRecordBase,   KCDTIdRecordBase )
    X_REGISTER_ATTRIBUTE(     CCDRecordBase,      iRecordTag,          TMDBNum )
	X_REGISTER_ATTRIBUTE(     CCDRecordBase,      iRecordName,        TMDBText )
END_ATTRIBUTE_TABLE()

// CCDParamsRecordBase
// empty class with base class members
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
START_ATTRIBUTE_TABLE(  CCDParamsRecordBase,   KCDTIdParamsRecordBase,  KCDTIdParamsRecordBase )
	X_REGISTER_ATTRIBUTE( CCDParamsRecordBase,		iSTypeId,			TMDBNum)
END_ATTRIBUTE_TABLE_BASE(   CCDRecordBase,          KCDTIdRecordBase)
#endif

// CCDServiceRecordBase
// empty class with base class members
START_ATTRIBUTE_TABLE(  CCDServiceRecordBase,   KCDTIdServiceRecordBase,  KCDTIdServiceRecordBase )
	X_REGISTER_ATTRIBUTE( CCDServiceRecordBase,	    iServiceEnableLlmnr,    TMDBNum)
END_ATTRIBUTE_TABLE_BASE(   CCDRecordBase,          KCDTIdRecordBase)


// CCDBearerRecordBase
// empty class with base class members
START_ATTRIBUTE_TABLE(  CCDBearerRecordBase,    KCDTIdBearerRecordBase,  KCDTIdBearerRecordBase )
	X_REGISTER_ATTRIBUTE( CCDBearerRecordBase,		iBearerAgent,           TMDBText)
END_ATTRIBUTE_TABLE_BASE(   CCDRecordBase,          KCDTIdRecordBase)



// 02/  CCDConnectionPrefsRecord
START_ATTRIBUTE_TABLE( CCDConnectionPrefsRecord,   	KCDTIdConnectionPrefsRecord,    KCDTIdConnectionPrefsRecord )
	X_REGISTER_ATTRIBUTE( CCDConnectionPrefsRecord, 	 iRanking ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDConnectionPrefsRecord, 	 iDirection ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDConnectionPrefsRecord, 	 iBearerSet ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDConnectionPrefsRecord, 	 iDialogPref ,	TMDBNum)
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDConnectionPrefsRecord, 	 iDefaultIAP ,	TMDBNum)
#else
	X_REGISTER_ATTRIBUTE( CCDConnectionPrefsRecord, 	 iDefaultIAP ,	TMDBLinkNum)
#endif 
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )


// 03/  CCDGlobalSettingsRecord
START_ATTRIBUTE_TABLE( CCDGlobalSettingsRecord,   	KCDTIdGlobalSettingsRecord,    KCDTIdGlobalSettingsRecord )
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iWAPAccessPoint ,	                TMDBNum)
#else
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iWAPAccessPoint ,	                TMDBLinkNum)
#endif
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iMaxConnectionAttempts ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iMaxRedialAttempts ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iSMSBearer ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iSMSReceiveMode ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iGPRSAttachMode ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iAcceptIncomingGPRS ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iGPRSClassCBearer ,	TMDBNum)
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iModemForDataAndFax ,	            TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iModemForPhoneServicesAndSMS ,	    TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iLocationForDataAndFax ,	        TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iLocationForPhoneServicesAndSMS ,	TMDBNum)
#else
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iModemForDataAndFax ,	            TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iModemForPhoneServicesAndSMS ,	    TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iLocationForDataAndFax ,	        TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iLocationForPhoneServicesAndSMS ,	TMDBLinkNum)
#endif
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iMaxMBufHeap,	                    TMDBNum)
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iDefaultAgent ,	                TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iDefaultNetwork ,	                TMDBNum)
#else
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iDefaultAgent ,	                TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iDefaultNetwork ,	                TMDBLinkNum)
#endif
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iBearerAvailabilityCheckTSY ,	TMDBText)
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY 
    X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iDefaultSnap ,                     TMDBDeprecated)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iPromptForSnap ,                   TMDBDeprecated)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iDefaultTier ,                     TMDBLinkNum)
#else
	#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	// For OS 9.3 and below Record Links use the TMBDNum type. 
		X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iDefaultSnap ,                     TMDBNum)
	#else
		X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iDefaultSnap ,                     TMDBLinkNum)
	#endif
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iPromptForSnap ,                   TMDBNum)    
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iRegulatoryDomain , TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iWmmEnabled, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iEnableCountryCode , TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGlobalSettingsRecord, 	 iDefaultRegulatoryDomain , TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )


// 04/  CCDNetworkRecord
START_ATTRIBUTE_TABLE( CCDNetworkRecord,   	KCDTIdNetworkRecord,    KCDTIdNetworkRecord )
        X_REGISTER_ATTRIBUTE( CCDNetworkRecord,   iHostName,           TMDBText )
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )


// 05/  CCDLocationRecord
START_ATTRIBUTE_TABLE(  CCDLocationRecord,   KCDTIdLocationRecord,      KCDTIdLocationRecord )
	X_REGISTER_ATTRIBUTE( CCDLocationRecord,   iIntlPrefixCode,           TMDBText )
	X_REGISTER_ATTRIBUTE( CCDLocationRecord,   iNatPrefixCode,            TMDBText )
	X_REGISTER_ATTRIBUTE( CCDLocationRecord,   iNatCode,                  TMDBText )
	X_REGISTER_ATTRIBUTE( CCDLocationRecord,   iAreaCode,                 TMDBText )
	X_REGISTER_ATTRIBUTE( CCDLocationRecord,   iDialOutCode,              TMDBText )
	X_REGISTER_ATTRIBUTE( CCDLocationRecord,   iDisableCallWaitingCode,   TMDBText )
	X_REGISTER_ATTRIBUTE( CCDLocationRecord,   iMobile,                   TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDLocationRecord,   iUsePulseDial,             TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDLocationRecord,   iWaitForDialTone,          TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDLocationRecord,   iPauseAfterDialOut,        TMDBNum )
END_ATTRIBUTE_TABLE_BASE(   CCDRecordBase,       KCDTIdRecordBase)


// 06/  CCDIAPRecord
START_ATTRIBUTE_TABLE(  CCDIAPRecord,   KCDTIdIAPRecord,    KCDTIdIAPRecord )
	X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iServiceType,         TMDBText )
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iService,             TMDBNum )
#else
    X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iService,             TMDBLinkNum )
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iBearerType,          TMDBText )
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iBearer,              TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iNetwork,             TMDBNum )
#else
    X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iBearer,              TMDBLinkNum )
    X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iNetwork,             TMDBLinkNum )
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
    X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iNetworkWeighting,    TMDBNum )
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
    X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iLocation,            TMDBNum )
#else
    X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iLocation,            TMDBLinkNum )
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS

#ifdef SYMBIAN_NETWORKING_UMTSR5
	X_REGISTER_ATTRIBUTE( CCDIAPRecord,   iAppSid,              TMDBNum )
#endif
END_ATTRIBUTE_TABLE_BASE(   CCDRecordBase,  KCDTIdRecordBase)


// 07/  CCDWAPAccessPointRecord
START_ATTRIBUTE_TABLE( CCDWAPAccessPointRecord,   	KCDTIdWAPAccessPointRecord,    KCDTIdWAPAccessPointRecord )
	X_REGISTER_ATTRIBUTE( CCDWAPAccessPointRecord, 	 iWAPCurrentBearer ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDWAPAccessPointRecord, 	 iWAPStartPage ,	TMDBLongText)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )




// 08/ DialOutISPRecordRecord
START_ATTRIBUTE_TABLE( CCDDialOutISPRecord,   KCDTIdDialOutISPRecord,   KCDTIdDialOutISPRecord )
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iDescription ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iType ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iDefaultTelNum ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iDialResolution ,	TMDBNum)
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iChargecard ,	            TMDBNum)
#else
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iChargecard ,	            TMDBLinkNum)
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iUseLoginScript ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iLoginScript ,	TMDBLongText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iPromptForLogin ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iLoginName ,	TMDBMedText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iLoginPass ,	TMDBMedText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iDisplayPct ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIfParams ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIfNetworks ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIfPromptForAuth ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIfAuthName ,	TMDBMedText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIfAuthPass ,	TMDBMedText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIfAuthRetries ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIfCallbackEnabled ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIfCallbackType ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIfCallbackInfo ,	TMDBBin)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iCallbackTimeout ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIfServerMode ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIpAddrFromServer ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIpAddr ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIpNetMask ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIpGateway ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIpDnsAddrFromServer ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIpNameServer1 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIpNameServer2 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIp6DnsAddrFromServer ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIp6NameServer1 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIp6NameServer2 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIpAddrLeaseValidFrom ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIpAddrLeaseValidTo ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iConfigDaemonManagerName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iConfigDaemonName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iEnableIpHeaderComp ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iEnableLcpExtension ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iDisablePlainTextAuth ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iEnableSwComp ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iBearerName ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iBearerSpeed ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iBearerCe ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord,	 iInitString ,	TMDBBin)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iBearerType ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iChannelCoding ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iAiur ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iRequestedTimeSlots ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iMaximumTimeSlots ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iBearerService ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iBearerProtocol ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iRlpVersion ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iIwfToMs ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iMsToIwf ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iAckTimer ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iRetransmissionAttempts ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iResequencePeriod ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iV42Compression ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iV42Codewords ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iV42MaxLength ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iAsymmetry ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iUserInitUpgrade ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDDialOutISPRecord, 	 iUseEdge ,	TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDServiceRecordBase, KCDTIdServiceRecordBase )


// 09/  DialInISPRecordRecord
START_ATTRIBUTE_TABLE( CCDDialInISPRecord,   KCDTIdDialInISPRecord,   KCDTIdDialInISPRecord )
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iDescription ,	TMDBText)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iType ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iUseLoginScript ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iLoginScript ,	TMDBLongText)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iAuthentication ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIfAuthRetries ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIfParams ,	TMDBText)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIfNetworks ,	TMDBText)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIpAddrFromServer ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIpAddr ,	TMDBText)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIpNetMask ,	TMDBText)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIpGateway ,	TMDBText)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIpDnsAddrFromServer ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIpNameServer1 ,	TMDBText)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIpNameServer2 ,	TMDBText)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIp6DnsAddrFromServer ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIp6NameServer1 ,	TMDBText)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iIp6NameServer2 ,	TMDBText)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iEnableIpHeaderComp ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iEnableLcpExtension ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iDisablePlainTextAuth ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iEnableSwComp ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iBearerName ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iBearerSpeed ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iBearerCe ,	TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iInitString ,	TMDBBin)
		X_REGISTER_ATTRIBUTE( CCDDialInISPRecord, 	 iUseEdge ,	TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDServiceRecordBase, KCDTIdServiceRecordBase )

// 0A/ LAN ServiceRecord
START_ATTRIBUTE_TABLE( CCDLANServiceRecord,   KCDTIdLANServiceRecord,   KCDTIdLANServiceRecord )
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIfNetworks ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIpNetmask ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIpGateway ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIpAddrFromServer ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIpAddr ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIpDnsAddrFromServer ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIpNameServer1 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIpNameServer2 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIp6DnsAddrFromServer ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIp6NameServer1 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIp6NameServer2 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIpAddrLeaseValidFrom ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iIpAddrLeaseValidTo ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iConfigDaemonManagerName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iConfigDaemonName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iServiceExtensionTableName ,	TMDBText)
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iServiceExtensionTableRecordId ,	TMDBNum)
#else
	X_REGISTER_ATTRIBUTE( CCDLANServiceRecord, 	 iServiceExtensionTableRecordId ,	TMDBLinkNum)
#endif // SYMBIAN_COMMSDAT_USE_RECORD_LINKS
	END_ATTRIBUTE_TABLE_BASE( CCDServiceRecordBase, KCDTIdServiceRecordBase )


// 0B/  CCDWLanServiceRecord
START_ATTRIBUTE_TABLE( CCDWLANServiceExtRecord,   	KCDTIdWLANServiceExtRecord	,KCDTIdWLANServiceExtRecord )
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANProfile,			TMDBText)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANSSID,				TMDBText)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANEncryptionStatus,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANEncryptionType,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANDefaultWepKey ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANEncryptKey1Valid,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANWepKey1,			TMDBText)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANEncryptKey2Valid,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANWepKey2,			TMDBText)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANEncryptKey3Valid,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANWepKey3,			TMDBText)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANEncryptKey4Valid,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANWepKey4,			TMDBText)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANRanking,			TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANDialogPref,		TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANNetworkType,		TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANFragThres,			TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANRTSThres,			TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANDesTransRate,		TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANBeaconInt,			TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANChannelId,			TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANAuthenticationMode,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANPowerSaveMode,		TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANAllowUnencryptMode,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANAllowShortPre,		TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANPreambleType,		TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANTxPowerLevel,		TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANEapPresharedKey,	TMDBMedText)
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANRegDomain,			TMDBNum)
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANSecData,			TMDBNum)
#else
	X_REGISTER_ATTRIBUTE( CCDWLANServiceExtRecord,		iWLANSecData,			TMDBLinkNum)
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )


// 0C/  CCDVPNServiceRecord
START_ATTRIBUTE_TABLE( CCDVPNServiceRecord,   KCDTIdVPNServiceRecord,   KCDTIdVPNServiceRecord )
	X_REGISTER_ATTRIBUTE( CCDVPNServiceRecord,  iServicePolicy,       TMDBText )
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDVPNServiceRecord,  iServiceIAP,          TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDVPNServiceRecord,  iServiceSNAP,         TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDVPNServiceRecord,  iServiceNetwork,      TMDBNum )
#else
	X_REGISTER_ATTRIBUTE( CCDVPNServiceRecord,  iServiceIAP,          TMDBLinkNum )
#endif 
	X_REGISTER_ATTRIBUTE( CCDVPNServiceRecord,  iServiceSNAP,         TMDBLinkNum )
#ifndef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
    X_REGISTER_ATTRIBUTE( CCDVPNServiceRecord,  iServiceNetwork,      TMDBLinkNum )
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
END_ATTRIBUTE_TABLE_BASE( CCDServiceRecordBase,      KCDTIdServiceRecordBase)



// 10/  CCDWCDMAPacketServiceRecord
START_ATTRIBUTE_TABLE( CCDWCDMAPacketServiceRecord,   KCDTIdWCDMAPacketServiceRecord,   KCDTIdWCDMAPacketServiceRecord )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSAPN,                    TMDBMedText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSPDPType,                TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSPDPAddress,              TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSReqPrecedence,          TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSReqDelay,               TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSReqReliability,          TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSReqPeakThroughput,      TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSReqMeanThroughput,      TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSMinPrecedence,          TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSMinDelay,               TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSMinReliability,          TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSMinPeakThroughput,      TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSMinMeanThroughput,      TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSDataCompression,        TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSHeaderCompression,      TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSUseEdge,                TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSAnonymousAccess,        TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIfParams,               TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIfNetworks,             TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIfPromptForAuth,        TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIfAuthName,             TMDBMedText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIfAuthPass,             TMDBMedText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIfAuthRetries,          TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIPNetMask,              TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIPGateway,              TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIPAddrFromServer,       TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIPAddr,                 TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIPDNSAddrFromServer,    TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIPNameServer1,          TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIPNameServer2,          TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIP6DNSAddrFromServer,   TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIP6NameServer1,         TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIP6NameServer2,         TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIPAddrLeaseValidFrom,   TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSIPAddrLeaseValidTo,     TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSConfigDaemonManagerName,TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSConfigDaemonName,       TMDBText )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSEnableLCPExtension,     TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSDisablePlainTextAuth,   TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSAPType,                 TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSQOSWarningTimeOut,      TMDBNum )
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iUmtsR99QoSAndOnTable,       TMDBNum )
#else
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iUmtsR99QoSAndOnTable,       TMDBLinkNum )
#endif
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSR5DataCompression, 	 TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSR5HeaderCompression, 	 TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSPacketFlowIdentifier, 	 TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDWCDMAPacketServiceRecord,  iGPRSUmtsGprsRelease, 	 	 TMDBNum )


END_ATTRIBUTE_TABLE_BASE( CCDServiceRecordBase, KCDTIdServiceRecordBase )

// 10.a/  CCDOutgoingGprsRecord
START_ATTRIBUTE_TABLE( CCDOutgoingGprsRecord,   KCDTIdOutgoingGprsRecord,   KCDTIdOutgoingGprsRecord )
END_ATTRIBUTE_TABLE_BASE( CCDWCDMAPacketServiceRecord, KCDTIdWCDMAPacketServiceRecord )

// 10.b/  CCDIncomingGprsRecord
START_ATTRIBUTE_TABLE( CCDIncomingGprsRecord,   KCDTIdIncomingGprsRecord,   KCDTIdIncomingGprsRecord )
END_ATTRIBUTE_TABLE_BASE( CCDWCDMAPacketServiceRecord, KCDTIdWCDMAPacketServiceRecord )


// 10.c/
START_ATTRIBUTE_TABLE( CCDUmtsR99QoSAndOnTableRecord,   KCDTIdUmtsR99QoSAndOnTableRecord,   KCDTIdUmtsR99QoSAndOnTableRecord )
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqTrafficClass,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSMinTrafficClass,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqDeliveryOrder,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSMinDeliveryOrder,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqDeliverErroneousSDU,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSMinDeliverErroneousSDU,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqMaxSDUSize,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSMinAcceptableMaxSDUSize,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqMaxUplinkRate,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqMinUplinkRate,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqMaxDownlinkRate,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqMinDownlinkRate,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqBER,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSMaxBER,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqSDUErrorRatio,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSMaxSDUErrorRatio,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqTrafficHandlingPriority,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSMinTrafficHandlingPriority,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqTransferDelay,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSMaxTransferDelay,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqGuaranteedUplinkRate,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSMinGuaranteedUplinkRate,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSReqGuaranteedDownlinkRate,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSMinGuaranteedDownlinkRate,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSSignallingIndication,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRS_ImCnSignallingIndication,  TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDUmtsR99QoSAndOnTableRecord,  iGPRSSourceStatisticsDescriptor,  TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )


//  12/  CCDDefaultWCDMARecord
START_ATTRIBUTE_TABLE( CCDDefaultWCDMARecord,   KCDTIdDefaultWCDMARecord,   KCDTIdDefaultWCDMARecord )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iUsage,     		TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iAPN,     		TMDBText )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iPdpType,    		TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iPdpAddress,    	TMDBText )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iPrecedence,     	TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iDelay,     		TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iReliability, 	TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iPeakThroughput, 	TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iMeanThroughput,  TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iMinPrecedence,   TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iMinDelay,     	TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iMinReliability, 	TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iMinPeakThroughput,TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iMinMeanThroughput,TMDBNum  )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iDataCompression, TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iHeaderCompression,TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iUseEdge, TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDDefaultWCDMARecord,  iAnonymousAccess, TMDBNum )
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )


// 13/  CCDModemBearerRecord
START_ATTRIBUTE_TABLE( CCDModemBearerRecord,   	KCDTIdModemBearerRecord,    KCDTIdModemBearerRecord )
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iNifName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iPortName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iTsyName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iCsyName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iLastSocketActivityTimeout ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iLastSessionClosedTimeout ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iLastSocketClosedTimeout ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iDataBits ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iStopBits ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iParity ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iRate ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iHandshaking ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iSpecialRate ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iXonChar ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iXoffChar ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iFaxClassPref ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iSpeakerPref ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iSpeakerVolPref ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iModemInitString ,	TMDBBin)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iDataInitString ,	TMDBBin)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iFaxInitString ,	TMDBBin)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iIspInitString ,	TMDBBin)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iDialPauseLength ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iCarrierTimeOut ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iAutoAnswerRingCount ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iSpeakerVolControlLow ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iSpeakerVolControlMedium ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iSpeakerVolControlHigh ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iSpeakerAlwaysOff ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iSpeakerOnUntilCarrier ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iSpeakerAlwaysOn ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iSpeakerOnAfterUntilCarrier ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iDialToneWaitModifier ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iCallProgress1 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iCallProgress2 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iCallProgress3 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iCallProgress4 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iEchoOff ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iVerboseText, 	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iQuietOff ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iQuietOn ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iDialCommandStateModifier ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iOnLine ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iResetConfiguration ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iReturnToFactoryDefs ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iDcdOnDuringLink ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iDtrHangUp ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iDsrAlwaysOn ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iRtsCtsHandshake ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iXonXoffHandshake ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iEscapeCharacter ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iEscapeGuardPeriod ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iFaxClassInterrogate ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iFaxClass ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iNoDialTone ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iBusy ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iNoAnswer ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iCarrier ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iConnect ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iCompressionClass5 ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iCompressionV42bis ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iCompressionNone ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iProtocolLapd ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iProtocolAlt ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iProtocolAltcellular ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iProtocolNone ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iMessageCentreNumber ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iMessageValidityPeriod ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iMessageDeliveryReport ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iMinSignalLevel ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iCommRole ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iControlChannelPortName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iSirSettings ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iBcaStack ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDModemBearerRecord, 	 iBearerTechnology ,	TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDBearerRecordBase, KCDTIdBearerRecordBase )


// 14/  CCDLANBearerRecord
START_ATTRIBUTE_TABLE( CCDLANBearerRecord,   KCDTIdLANServiceRecord,   KCDTIdLANServiceRecord )
	X_REGISTER_ATTRIBUTE( CCDLANBearerRecord, 	 iLanBearerNifName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANBearerRecord, 	 iLanBearerLddFilename ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANBearerRecord, 	 iLanBearerLddName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANBearerRecord, 	 iLanBearerPddFilename ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANBearerRecord, 	 iLanBearerPddName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANBearerRecord, 	 iLanBearerPacketDriverName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDLANBearerRecord, 	 iLastSocketActivityTimeout ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDLANBearerRecord, 	 iLastSessionClosedTimeout ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDLANBearerRecord, 	 iLastSocketClosedTimeout ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDLANBearerRecord, 	 iBearerTechnology ,	TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDBearerRecordBase, KCDTIdBearerRecordBase )


// 15/  CCDVirtualBearerRecord
START_ATTRIBUTE_TABLE( CCDVirtualBearerRecord,   KCDTIdVirtualBearerRecord,   KCDTIdVirtualBearerRecord )
	X_REGISTER_ATTRIBUTE( CCDVirtualBearerRecord,  iVirtualBearerNifName,  TMDBText )
	X_REGISTER_ATTRIBUTE( CCDVirtualBearerRecord,  iLastSocketActivityTimeout,    TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDVirtualBearerRecord,  iLastSessionClosedTimeout,    TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDVirtualBearerRecord,  iLastSocketClosedTimeout,    TMDBNum )
	X_REGISTER_ATTRIBUTE( CCDVirtualBearerRecord,  iBearerTechnology ,	TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDBearerRecordBase, KCDTIdBearerRecordBase )


// 16/  CCDWAPSMSBearerRecord
START_ATTRIBUTE_TABLE( CCDWAPSMSBearerRecord,   	KCDTIdWAPSMSBearerRecord,    KCDTIdWAPSMSBearerRecord )
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDWAPSMSBearerRecord, 	 iWAPAccessPointId ,	    TMDBNum)
#else
	X_REGISTER_ATTRIBUTE( CCDWAPSMSBearerRecord, 	 iWAPAccessPointId ,	    TMDBLinkNum)
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDWAPSMSBearerRecord, 	 iWAPGatewayAddress ,	    TMDBText)
	X_REGISTER_ATTRIBUTE( CCDWAPSMSBearerRecord, 	 iWAPServiceCentreAddress ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDWAPSMSBearerRecord, 	 iWAPWSPOption ,	        TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWAPSMSBearerRecord, 	 iWAPSecurity ,	            TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )


// 17/  CCDWAPIPBearerRecord
START_ATTRIBUTE_TABLE( CCDWAPIPBearerRecord,   	KCDTIdWAPIPBearerRecord,    KCDTIdWAPIPBearerRecord )
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDWAPIPBearerRecord, 	 iWAPAccessPointId,	    TMDBNum)
#else
	X_REGISTER_ATTRIBUTE( CCDWAPIPBearerRecord, 	 iWAPAccessPointId,	    TMDBLinkNum)
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDWAPIPBearerRecord, 	 iWAPGatewayAddress,	TMDBText)
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDWAPIPBearerRecord, 	 iWAPIAP,	            TMDBNum)
#else
	X_REGISTER_ATTRIBUTE( CCDWAPIPBearerRecord, 	 iWAPIAP,	            TMDBLinkNum)
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDWAPIPBearerRecord, 	 iWAPWSPOption,	        TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWAPIPBearerRecord, 	 iWAPSecurity,	        TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWAPIPBearerRecord, 	 iWAPProxyPort,	        TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWAPIPBearerRecord, 	 iWAPProxyLoginName,	TMDBMedText)
	X_REGISTER_ATTRIBUTE( CCDWAPIPBearerRecord, 	 iWAPProxyLoginPass,	TMDBMedText)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )



// 18/  CCDChargecardRecord
START_ATTRIBUTE_TABLE( CCDChargecardRecord,   	KCDTIdChargecardRecord,    KCDTIdChargecardRecord )
	X_REGISTER_ATTRIBUTE( CCDChargecardRecord, 	 iAccountNo ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDChargecardRecord, 	 iPin ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDChargecardRecord, 	 iLocalRule ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDChargecardRecord, 	 iNatRule ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDChargecardRecord, 	 iIntlRule ,	TMDBText)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// 19/  CCDProxiesRecord
START_ATTRIBUTE_TABLE( CCDProxiesRecord,   	KCDTIdProxiesRecord,    KCDTIdProxiesRecord )
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
    X_REGISTER_ATTRIBUTE( CCDProxiesRecord, 	 iService ,			TMDBNum)
#else
    X_REGISTER_ATTRIBUTE( CCDProxiesRecord, 	 iService ,			TMDBLinkNum)
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDProxiesRecord, 	 iServiceType ,	    TMDBText)
	X_REGISTER_ATTRIBUTE( CCDProxiesRecord, 	 iUseProxyServer ,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDProxiesRecord, 	 iServerName ,	    TMDBLongText)
	X_REGISTER_ATTRIBUTE( CCDProxiesRecord, 	 iProtocolName ,	TMDBText)
	X_REGISTER_ATTRIBUTE( CCDProxiesRecord, 	 iPortNumber ,	    TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDProxiesRecord, 	 iExceptions ,	    TMDBLongText)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )


// 1A/  CCDSecureSocketRecord
START_ATTRIBUTE_TABLE( CCDSecureSocketRecord,   KCDTIdSSProtoRecord,   KCDTIdSSProtoRecord )
	X_REGISTER_ATTRIBUTE( CCDSecureSocketRecord,  iSSProtoName,     TMDBText )
	X_REGISTER_ATTRIBUTE( CCDSecureSocketRecord,  iSSProtoLibrary,  TMDBText )
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )


// 1b/  CCDAgentLookupRecord
START_ATTRIBUTE_TABLE( CCDAgentLookupRecord,   	KCDTIdAgentLookupRecord	,KCDTIdAgentLookupRecord )
	X_REGISTER_ATTRIBUTE( CCDAgentLookupRecord,		iAgentFriendlyName, TMDBText)
	X_REGISTER_ATTRIBUTE( CCDAgentLookupRecord,		iAgentFileName,     TMDBText)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// 1c/ CCDPANServiceExtRecord
START_ATTRIBUTE_TABLE( CCDPANServiceExtRecord,   	KCDTIdPANServiceExtRecord ,KCDTIdPANServiceExtRecord )
	X_REGISTER_ATTRIBUTE( CCDPANServiceExtRecord,		iLocalRole,					TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPANServiceExtRecord,		iPeerRole,					TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPANServiceExtRecord,		iPeerMACAddresses,			TMDBText)
	X_REGISTER_ATTRIBUTE( CCDPANServiceExtRecord,		iPromptForRemoteDevices,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPANServiceExtRecord,		iDisableSdpQuery,			TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPANServiceExtRecord,		iAllowIncoming,				TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPANServiceExtRecord,		iPromptIfMACListFails,		TMDBNum)
#ifdef SYMBIAN_NETWORKING_DHCPSERVER	
	X_REGISTER_ATTRIBUTE( CCDPANServiceExtRecord,		iNapServiceEnabled,			TMDBNum)
#endif // SYMBIAN_NETWORKING_DHCPSERVER	
END_ATTRIBUTE_TABLE_BASE( CCDServiceRecordBase, KCDTIdServiceRecordBase )  // VCT - it is not correct that this inherits from Service Record base

// 1D/  CCDAccessPointRecord
START_ATTRIBUTE_TABLE( CCDAccessPointRecord, KCDTIdAccessPointRecord, KCDTIdAccessPointRecord )
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iAccessPointGID,        TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iSelectionPolicy,       TMDBNum)  
#else
    X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iAccessPointGID,        TMDBNum)
	#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
    	X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iSelectionPolicy,       TMDBNum)  
	#else
    	X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iSelectionPolicy,       TMDBLinkNum)  
	#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
#endif // SYMBIAN NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iTier,                  TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iMCpr,                  TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iCpr,                   TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iSCpr,                  TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iProtocol,              TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iCprConfig,             TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iAppSID,                TMDBNum)
    X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iConfigAPIdList,        TMDBText)
    X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iCustomSelectionPolicy, TMDBNum)    
	X_REGISTER_ATTRIBUTE( CCDAccessPointRecord, iPriority,	      TMDBNum)
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// 1E/ CCDSelectionPolicyRecordBase
START_ATTRIBUTE_TABLE( CCDSelectionPolicyRecordBase, KCDTIdSelectionPolicyRecordBase, KCDTIdSelectionPolicyRecordBase )
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// 1F/  CCDIAPPrioritySelectionPolicyRecord
/*
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
This record Type DEPRECATED AND MAPPED to CCDAPPrioritySelectionPolicyRecord
*/
START_ATTRIBUTE_TABLE( CCDIAPPrioritySelectionPolicyRecord, KCDTIdIapPrioritySelectionPolicyRecord, KCDTIdIapPrioritySelectionPolicyRecord )
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap1,      TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap2,      TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap3,      TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap4,      TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap5,      TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap6,      TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap7,      TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap8,      TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap9,      TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap10,     TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap11,     TMDBDeprecated)
	X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap12,     TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap13,     TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap14,     TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap15,     TMDBDeprecated)
    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIapCount,  TMDBDeprecated)
#else
	#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap1,      TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap2,      TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap3,      TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap4,      TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap5,      TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap6,      TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap7,      TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap8,      TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap9,      TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap10,     TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap11,     TMDBNum)
		X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap12,     TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap13,     TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap14,     TMDBNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap15,     TMDBNum)
	#else
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap1,      TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap2,      TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap3,      TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap4,      TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap5,      TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap6,      TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap7,      TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap8,      TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap9,      TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap10,     TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap11,     TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap12,     TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap13,     TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap14,     TMDBLinkNum)
	    X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIap15,     TMDBLinkNum)
	#endif  // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDIAPPrioritySelectionPolicyRecord,  iIapCount,  TMDBNum)
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
END_ATTRIBUTE_TABLE_BASE( CCDSelectionPolicyRecordBase, KCDTIdSelectionPolicyRecordBase )

// 20/  CCDBaseSecRecord
START_ATTRIBUTE_TABLE( CCDBaseSecRecord, KCDTIdBaseSecRecord,	KCDTIdBaseSecRecord )
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// 21/  CCDEAPSecRecord
START_ATTRIBUTE_TABLE( CCDEAPSecRecord,	KCDTIdEAPSecRecord,	KCDTIdEAPSecRecord )
	X_REGISTER_ATTRIBUTE( CCDEAPSecRecord, 	iEAPSecOuterEAPType,	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDEAPSecRecord, 	iEAPSecId, 				TMDBMedText)
	X_REGISTER_ATTRIBUTE( CCDEAPSecRecord, 	iEAPSecPassword,		TMDBMedText)
	X_REGISTER_ATTRIBUTE( CCDEAPSecRecord, 	iEAPSecConfigId,		TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDEAPSecRecord, 	iEAPSecData,			TMDBNum)
    X_REGISTER_ATTRIBUTE( CCDEAPSecRecord, 	iEAPSecVendorId,		TMDBText)
    X_REGISTER_ATTRIBUTE( CCDEAPSecRecord, 	iEAPSecVendorType,		TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDBaseSecRecord, KCDTIdBaseSecRecord )

// 22/  CCDTunEAPRecord
START_ATTRIBUTE_TABLE( CCDTunEAPRecord, KCDTIdTunEAPRecord,	KCDTIdTunEAPRecord )
	X_REGISTER_ATTRIBUTE( CCDTunEAPRecord, 	iTUNEAPInnerType, 		TMDBNum)
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
	X_REGISTER_ATTRIBUTE( CCDTunEAPRecord, 	iTUNEAPData,			TMDBNum)
#else
	X_REGISTER_ATTRIBUTE( CCDTunEAPRecord, 	iTUNEAPData,			TMDBLinkNum)
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
END_ATTRIBUTE_TABLE_BASE( CCDBaseSecRecord, KCDTIdBaseSecRecord )

// 23/  CCDEAPTLSRecord
START_ATTRIBUTE_TABLE( CCDEAPTLSRecord, KCDTIdEAPTLSRecord,	KCDTIdEAPTLSRecord )
	X_REGISTER_ATTRIBUTE( CCDEAPTLSRecord, 	iEAPTLSClientCertName, 	TMDBText)
END_ATTRIBUTE_TABLE_BASE( CCDBaseSecRecord, KCDTIdBaseSecRecord )

// 24/  CCDLEAPRecord
START_ATTRIBUTE_TABLE( CCDLEAPRecord, KCDTIdLEAPRecord,	KCDTIdLEAPRecord )
	X_REGISTER_ATTRIBUTE( CCDLEAPRecord, 	iLEAPTimeout, 			TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDBaseSecRecord, KCDTIdBaseSecRecord )

// CCDEapSimProtocolRecord
START_ATTRIBUTE_TABLE( CCDEapSimProtocolRecord, KCDTIdEapSimProtocolRecord, KCDTIdEapSimProtocolRecord )
	X_REGISTER_ATTRIBUTE( CCDEapSimProtocolRecord, iProtocolId, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDEapSimProtocolRecord, iPseudonym, TMDBBin)
	X_REGISTER_ATTRIBUTE( CCDEapSimProtocolRecord, iUseNaiRealm, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDEapSimProtocolRecord, iNaiRealm, TMDBBin)
	X_REGISTER_ATTRIBUTE( CCDEapSimProtocolRecord, iMinRands, TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// CCDEapAkaProtocolRecord
START_ATTRIBUTE_TABLE( CCDEapAkaProtocolRecord, KCDTIdEapAkaProtocolRecord, KCDTIdEapAkaProtocolRecord )
	X_REGISTER_ATTRIBUTE( CCDEapAkaProtocolRecord, iProtocolId, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDEapAkaProtocolRecord, iPseudonym, TMDBBin)
	X_REGISTER_ATTRIBUTE( CCDEapAkaProtocolRecord, iUseNaiRealm, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDEapAkaProtocolRecord, iNaiRealm, TMDBBin)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
// CCDAPPrioritySelectionPolicyRecord
START_ATTRIBUTE_TABLE( CCDAPPrioritySelectionPolicyRecord, KCDTIdApPrioritySelectionPolicyRecord, KCDTIdApPrioritySelectionPolicyRecord )
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp1,     TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp2,     TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp3,     TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp4,     TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp5,     TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp6,     TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp7,     TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp8,     TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp9,     TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp10,    TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp11,    TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp12,    TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp13,    TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp14,    TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iAp15,    TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDAPPrioritySelectionPolicyRecord, iApCount, TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDSelectionPolicyRecordBase, KCDTIdSelectionPolicyRecordBase )

// CCDTierRecord
START_ATTRIBUTE_TABLE( CCDTierRecord, KCDTIdTierRecord, KCDTIdTierRecord )
	X_REGISTER_ATTRIBUTE( CCDTierRecord, iTierImplUid,       	TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDTierRecord, iTierThreadName,       TMDBText)
	X_REGISTER_ATTRIBUTE( CCDTierRecord, iTierManagerName,      TMDBText)
	X_REGISTER_ATTRIBUTE( CCDTierRecord, iDefaultAccessPoint,   TMDBLinkNum)
	X_REGISTER_ATTRIBUTE( CCDTierRecord, iPromptUser,           TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// CCDMCprRecord
START_ATTRIBUTE_TABLE( CCDMCprRecord, KCDTIdMCprRecord, KCDTIdMCprRecord )
	X_REGISTER_ATTRIBUTE( CCDMCprRecord, iMCprUid, TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// CCDCprRecord
START_ATTRIBUTE_TABLE( CCDCprRecord, KCDTIdCprRecord, KCDTIdCprRecord )
	X_REGISTER_ATTRIBUTE( CCDCprRecord, iCprUid, TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// CCDSCprRecord
START_ATTRIBUTE_TABLE( CCDSCprRecord, KCDTIdSCprRecord, KCDTIdSCprRecord )
	X_REGISTER_ATTRIBUTE( CCDSCprRecord, iSCprUid, TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// CCDProtocolRecord
START_ATTRIBUTE_TABLE( CCDProtocolRecord, KCDTIdProtocolRecord, KCDTIdProtocolRecord )
	X_REGISTER_ATTRIBUTE( CCDProtocolRecord, iProtocolUid, TMDBNum)
    X_REGISTER_ATTRIBUTE( CCDProtocolRecord, iProtocolConfigLoaderUid, TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// CCDBearerTypeRecord
START_ATTRIBUTE_TABLE( CCDBearerTypeRecord, KCDTIdBearerTypeRecord, KCDTIdBearerTypeRecord )
	X_REGISTER_ATTRIBUTE( CCDBearerTypeRecord,   iTier,      TMDBLinkNum )
	X_REGISTER_ATTRIBUTE( CCDBearerTypeRecord,   iMCpr,      TMDBLinkNum )
	X_REGISTER_ATTRIBUTE( CCDBearerTypeRecord,   iCpr,       TMDBLinkNum )
	X_REGISTER_ATTRIBUTE( CCDBearerTypeRecord,   iSCpr,      TMDBLinkNum )
	X_REGISTER_ATTRIBUTE( CCDBearerTypeRecord,   iProtocol,  TMDBLinkNum )
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// CCDConfigAccessPointRecord
START_ATTRIBUTE_TABLE( CCDConfigAccessPointRecord, KCDTIdConfigAccessPointRecord, KCDTIdConfigAccessPointRecord )
	X_REGISTER_ATTRIBUTE( CCDConfigAccessPointRecord,   iSCpr,           TMDBLinkNum )
	X_REGISTER_ATTRIBUTE( CCDConfigAccessPointRecord,   iProtocol,       TMDBLinkNum )
	X_REGISTER_ATTRIBUTE( CCDConfigAccessPointRecord,   iProtocolConfig, TMDBNum )
    X_REGISTER_ATTRIBUTE( CCDConfigAccessPointRecord,   iLayerBelow,     TMDBLinkNum )
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// CCDPolicySelectorRecord
START_ATTRIBUTE_TABLE( CCDPolicySelectorRecord, KCDTIdPolicySelectorRecord, KCDTIdPolicySelectorRecord )
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iPolicyId, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iAppUid, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iSrcAddress, TMDBText)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iSrcMask, TMDBText)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iDstAddress, TMDBText)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iDstMask, TMDBText)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iSrcPort, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iDstPort, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iSrcPortMax, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iDstPortMax, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iProtocolId, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iIapId, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPolicySelectorRecord, iPriority, TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// CCDPolicySelector2ParamsRecord
START_ATTRIBUTE_TABLE( CCDPolicySelector2ParamsRecord, KCDTIdPolicySelector2ParamsRecord, KCDTIdPolicySelector2ParamsRecord )
	X_REGISTER_ATTRIBUTE( CCDPolicySelector2ParamsRecord, iPolicySelectorId, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDPolicySelector2ParamsRecord, iParamsId, TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// CCDGenericQosRecord
START_ATTRIBUTE_TABLE( CCDGenericQosRecord, KCDTIdGenericQosRecord, KCDTIdGenericQosRecord )
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iDownlinkBandwidth, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iUplinkBandwidth, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iDownLinkMaximumBurstSize, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iUpLinkMaximumBurstSize, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iDownLinkAveragePacketSize, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iUpLinkAveragePacketSize, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iDownLinkMaximumPacketSize, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iUpLinkMaximumPacketSize, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iDownLinkDelay, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iUpLinkDelay, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iDownLinkDelayVariation, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iUpLinkDelayVariation, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iDownLinkPriority, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iUpLinkPriority, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iHeaderMode, TMDBNum)
	//X_REGISTER_ATTRIBUTE( CCDGenericQosRecord, iQosName, TMDBText)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

// CCDWifiScanEngineRecord
START_ATTRIBUTE_TABLE( CCDWifiScanEngineRecord, KCDTIdWifiScanEngineRecord, KCDTIdWifiScanEngineRecord )
	X_REGISTER_ATTRIBUTE( CCDWifiScanEngineRecord, iScanPeriodMs, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWifiScanEngineRecord, iRSSIMin, TMDBNum)
	X_REGISTER_ATTRIBUTE( CCDWifiScanEngineRecord, iRSSIMax, TMDBNum)
END_ATTRIBUTE_TABLE_BASE( CCDRecordBase, KCDTIdRecordBase )

#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

// CONSTRUCTORS FOR BASE CLASSES FOR COMMS CONFIGURATION RECORDS

        // CCDRecordBase
        EXPORT_C CCDRecordBase::CCDRecordBase(TMDBElementId aElementId)
		  : CMDBRecordBase(aElementId),
		   iRecordTag((aElementId & KCDMaskShowRecordType) | KCDTIdRecordTag),
           iRecordName((aElementId & KCDMaskShowRecordType) | KCDTIdRecordName)
			// MAINT - if add fields here these need to have element id |rd with type id
		   {}


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        // ParamsRecord Base
		CCDParamsRecordBase::CCDParamsRecordBase(TMDBElementId aElementId)
			: CCDRecordBase(aElementId),
			iSTypeId((aElementId & KCDMaskShowRecordType) | KCDTIdParamsSTypeId)
			// MAINT - if add fields here these need to have element id |rd with type id
			{}
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

// CONSTRUCTORS FOR BASE CLASSES FOR COMMS CONFIGURATION RECORDS
// ServiceRecord Base
CCDServiceRecordBase::CCDServiceRecordBase(TMDBElementId aElementId)
	: CCDRecordBase(aElementId)/*,
	iServiceEnableLlmnr((aElementId & KCDMaskShowRecordType) | KCDTIdServiceEnableLlmnr)*/
	// MAINT - if add fields here these need to have element id |rd with type id
	{}


//  Bearer RecordBase
CCDBearerRecordBase::CCDBearerRecordBase(TMDBElementId aElementId)
	: CCDRecordBase(aElementId)/*,
	iBearerAgent((aElementId & KCDMaskShowRecordType) | KCDTIdBearerAgent)*/
	// MAINT - if add fields here these need to have element id |rd with type id
	{}



// CONSTRUCTORS FOR COMMS CONFIGURATION RECORDS


        // 02 Connection Preferences
         CCDConnectionPrefsRecord::CCDConnectionPrefsRecord(TMDBElementId aElementId)
  : CCDRecordBase(aElementId)/*,
   iRanking(KCDTIdRanking),
   iDirection(KCDTIdDirection),
   iBearerSet(KCDTIdBearerSet),
   iDialogPref(KCDTIdDialogPref),
   iDefaultIAP(KCDTIdIAPLink)*/
		   {}


        // 03 GlobalSettings Record
		CCDGlobalSettingsRecord::CCDGlobalSettingsRecord(TMDBElementId aElementId)
  : CCDRecordBase(aElementId)/*,
  iWAPAccessPoint(KCDTIdGlobalWapAcessPoint),
  iMaxConnectionAttempts(KCDTIdMaxConnectionAttempts),
  iMaxRedialAttempts(KCDTIdMaxRedialAttempts),
  iSMSBearer(KCDTIdSMSBearer),
  iSMSReceiveMode(KCDTIdSMSReceiveMode),
  iGPRSAttachMode(KCDTIdGPRSAttachMode),
  iAcceptIncomingGPRS(KCDTIdAcceptIncomingGPRS),
  iGPRSClassCBearer(KCDTIdGPRSClassCBearer),
  iModemForDataAndFax(KCDTIdModemDataFax),
  iModemForPhoneServicesAndSMS(KCDTIdModemPhoneServicesSMS),
  iLocationForDataAndFax(KCDTIdLocationDataFax),
  iLocationForPhoneServicesAndSMS(KCDTIdLocationPhoneServicesSMS),
  iMaxMBufHeap(KCDTIdMaxMBufHeap),
  iDefaultAgent(KCDTIdDefaultAgent),
  iDefaultNetwork(KCDTIdDefaultNetwork),
  iBearerAvailabilityCheckTSY(KCDTIdBearerAvailabilityCheckTSY),
  iCDMAOPCapability(KCDTIdCDMAOPCapability),
  iCDMASimIPMaxNumNai(KCDTIdCDMASimIPMaxNumNai),
  iCDMASimIPMaxNaiLength(KCDTIdCDMASimIPMaxNaiLength),
  iCDMASimIPMaxSSLength(KCDTIdCDMASimIPMaxSSLength) ,
  iCDMASimIPAuthSupported(KCDTIdCDMASimIPAuthSupported),
  iCDMAMIPMaxNumNai(KCDTIdCDMAMIPMaxNumNai),
  iCDMAMIPMaxNaiLength(KCDTIdCDMAMIPMaxNaiLength),
  iCDMAMIPMaxMNAAASSLength(KCDTIdCDMAMIPMaxMNAAASSLength),
  iCDMAMIPMNAAAAuthAlgorithm(KCDTIdCDMAMIPMNAAAAuthAlgorithm),
  iCDMAMIPMaxMNHASSLength(KCDTIdCDMAMIPMaxMNHASSLength),
  iCDMAMIPMNHAAuthAlgorithm(KCDTIdCDMAMIPMNHAAuthAlgorithm),
  iDefaultSnap(KCDTIdDefaultSnap),
  iPromptForSnap(KCDTIdPromptForSnap)
		  iSMSBearer(KCDTIdSMSBearer),
          iSMSReceiveMode(KCDTIdSMSReceiveMode),
		  iGPRSClassCBearer(KCDTIdGPRSClassCBearer),
		  iModemForPhoneServicesAndSMS(KCDTIdModemPhoneServicesSMS),
		  iLocationForPhoneServicesAndSMS(KCDTIdLocationPhoneServicesSMS),
          iMaxMBufHeap(KCDTIdMaxMBufHeap),
		  iDefaultNetwork(KCDTIdDefaultNetwork),
          iBearerAvailabilityCheckTSY(KCDTIdBearerAvailabilityCheckTSY),
		  iCDMASimIPMaxNumNai(KCDTIdCDMASimIPMaxNumNai),
          iCDMASimIPMaxNaiLength(KCDTIdCDMASimIPMaxNaiLength),
		  iCDMASimIPAuthSupported(KCDTIdCDMASimIPAuthSupported),
          iCDMAMIPMaxNumNai(KCDTIdCDMAMIPMaxNumNai),
		  iCDMAMIPMaxMNAAASSLength(KCDTIdCDMAMIPMaxMNAAASSLength),
          iCDMAMIPMNAAAAuthAlgorithm(KCDTIdCDMAMIPMNAAAAuthAlgorithm),
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
  ,iDefaultTier(KCDTIdDefaultTier)
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	      ,iRegulatoryDomain(KCDTIdRegulatoryDomain)
		  ,iWmmEnabled(KCDTIdWmmEnabled)
		  ,iEnableCountryCode(KCDTIdEnableCountryCode)
		  ,iDefaultRegulatoryDomain(KCDTIdDefaultRegulatoryDomain)
*/
		  {}

		// 04 NetworkRecord
		CCDNetworkRecord::CCDNetworkRecord(TMDBElementId aElementId)
    : CCDRecordBase(aElementId)/*,
    iHostName(KCDTIdHostName)*/
    {}


	    // 05 Location Record
		CCDLocationRecord::CCDLocationRecord(TMDBElementId aElementId)
	: CCDRecordBase(aElementId)/*,
	iIntlPrefixCode(KCDTIdIntlPrefixCode),
    iNatPrefixCode(KCDTIdNatPrefixCode),
    iNatCode(KCDTIdNatCode),
	iAreaCode(KCDTIdAreaCode),
    iDialOutCode(KCDTIdDialOutCode),
    iDisableCallWaitingCode(KCDTIdDisableCallWaitingCode),
	iMobile(KCDTIdMobile),
    iUsePulseDial(KCDTIdUsePulseDial),
    iWaitForDialTone(KCDTIdWaitForDialTone),
	iPauseAfterDialOut(KCDTIdPauseAfterDialOut)*/
		    {}


	    // 06 IAP Record
	    CCDIAPRecord::CCDIAPRecord(TMDBElementId aElementId)
   : CCDRecordBase(aElementId)/*,
   iServiceType(KCDTIdIAPServiceType),
   iService(KCDTIdIAPService),
   iBearerType(KCDTIdIAPBearerType),
   iBearer(KCDTIdIAPBearer),
   iNetwork(KCDTIdIAPNetwork),
   iNetworkWeighting(KCDTIdIAPNetworkWeighting),
   iLocation(KCDTIdIAPLocation)
#ifdef SYMBIAN_NETWORKING_UMTSR5
   ,iAppSid(KCDTIdIAPAppSid)
#endif // SYMBIAN_NETWORKING_UMTSR5*/
		   {}


		// 07  WAP ACCESS POINT RECORD
	    CCDWAPAccessPointRecord::CCDWAPAccessPointRecord(TMDBElementId aElementId)
   : CCDRecordBase(aElementId)/*,
	iWAPCurrentBearer(KCDTIdWAPCurrentBearer),
    iWAPStartPage(KCDTIdWAPStartPage)*/
			{}


        // SERVICES

		// 08  DIAL OUT ISP SERVICE RECORD
    	CCDDialOutISPRecord::CCDDialOutISPRecord(TMDBElementId aElementId)
   : CCDServiceRecordBase(aElementId)/*,
	iDescription(KCDTIdDescription),
    iType(KCDTIdType),
    iDefaultTelNum(KCDTIdDefaultTelNum),
	iDialResolution(KCDTIdDialResolution),
    iChargecard(KCDTIdChargecard),
    iUseLoginScript(KCDTIdUseLoginScript),
    iLoginScript(KCDTIdLoginScript),
    iPromptForLogin(KCDTIdPromptForLogin),
    iLoginName(KCDTIdLoginName),
    iLoginPass(KCDTIdLoginPass),
    iDisplayPct(KCDTIdDisplayPct),
    iIfParams(KCDTIdIfParams),
    iIfNetworks(KCDTIdIfNetworks),
    iIfPromptForAuth(KCDTIdIfPromptForAuth),
    iIfAuthName(KCDTIdIfAuthName),
    iIfAuthPass(KCDTIdIfAuthPass),
    iIfAuthRetries(KCDTIdIfAuthRetries),
    iIfCallbackEnabled(KCDTIdIfCallbackEnabled),
    iIfCallbackType(KCDTIdIfCallbackType),
    iIfCallbackInfo(KCDTIdIfCallbackInfo),
    iCallbackTimeout(KCDTIdCallbackTimeout),
    iIfServerMode(KCDTIdIfServerMode),
    iIpAddrFromServer(KCDTIdIpAddrFromServer),
    iIpAddr(KCDTIdIpAddr),
    iIpNetMask(KCDTIdIpNetMask),
    iIpGateway(KCDTIdIpGateway),
    iIpDnsAddrFromServer(KCDTIdIpDNSAddrFromServer),
    iIpNameServer1(KCDTIdIpNameServer1),
    iIpNameServer2(KCDTIdIpNameServer2),
    iIp6DnsAddrFromServer(KCDTIdIp6DNSAddrFromServer),
    iIp6NameServer1(KCDTIdIp6NameServer1),
    iIp6NameServer2(KCDTIdIp6NameServer2),
    iIpAddrLeaseValidFrom(KCDTIdIpAddrLeaseValidFrom),
    iIpAddrLeaseValidTo(KCDTIdIpAddrLeaseValidTo),
    iConfigDaemonManagerName(KCDTIdConfigDaemonManagerName),
    iConfigDaemonName(KCDTIdConfigDaemonName),
    iEnableIpHeaderComp(KCDTIdEnableIpHeaderComp),
    iEnableLcpExtension(KCDTIdEnableLCPExtension),
    iDisablePlainTextAuth(KCDTIdDisablePlainTextAuth),
    iEnableSwComp(KCDTIdEnableSwComp),
    iBearerName(KCDTIdBearerName),
    iBearerSpeed(KCDTIdBearerSpeed),
    iBearerCe(KCDTIdBearerCe),
    iInitString(KCDTIdInitString),
    iBearerType(KCDTIdBearerType),
    iChannelCoding(KCDTIdChannelCoding),
    iAiur(KCDTIdAiur),
    iRequestedTimeSlots(KCDTIdRequestedTimeSlots),
    iMaximumTimeSlots(KCDTIdMaximumTimeSlots),
    iBearerService(KCDTIdBearerService),
    iBearerProtocol(KCDTIdBearerProtocol),
    iRlpVersion(KCDTIdRlpVersion),
    iIwfToMs(KCDTIdIwfToMs),
    iMsToIwf(KCDTIdMsToIwf),
    iAckTimer(KCDTIdAckTimer),
    iRetransmissionAttempts(KCDTIdRetransmissionAttempts),
    iResequencePeriod(KCDTIdResequencePeriod),
    iV42Compression(KCDTIdV42Compression),
    iV42Codewords(KCDTIdV42Codewords),
    iV42MaxLength(KCDTIdV42MaxLength),
    iAsymmetry(KCDTIdAsymmetry),
    iUserInitUpgrade(KCDTIdUserInitUpgrade),
    iUseEdge(KCDTIdUseEdge)*/
            {}


		// 09/  DIAL IN ISP RECORD
	    CCDDialInISPRecord::CCDDialInISPRecord(TMDBElementId aElementId)
   : CCDServiceRecordBase(aElementId)/*,
	iDescription(KCDTIdDINDescription),
    iType(KCDTIdDINType),
    iUseLoginScript(KCDTIdDINUseLoginScript),
    iLoginScript(KCDTIdDINLoginScript),
    iAuthentication(KCDTIdDINAuthentication),
    iIfAuthRetries(KCDTIdDINIfAuthRetries),
    iIfParams(KCDTIdDINIfParams),
    iIfNetworks(KCDTIdDINIfNetworks),
    iIpAddrFromServer(KCDTIdDINIpAddrFromServer),
    iIpAddr(KCDTIdDINIpAddr),
    iIpNetMask(KCDTIdDINIpNetMask),
    iIpGateway(KCDTIdDINIpGateway),
    iIpDnsAddrFromServer(KCDTIdDINIpDNSAddrFromServer),
    iIpNameServer1(KCDTIdDINIpNameServer1),
    iIpNameServer2(KCDTIdDINIpNameServer2),
    iIp6DnsAddrFromServer(KCDTIdDINIp6DNSAddrFromServer),
    iIp6NameServer1(KCDTIdDINIp6NameServer1),
    iIp6NameServer2(KCDTIdDINIp6NameServer2),
    iEnableIpHeaderComp(KCDTIdDINEnableIpHeaderComp),
    iEnableLcpExtension(KCDTIdDINEnableLCPExtensions),
    iDisablePlainTextAuth(KCDTIdDINDisablePlainTextAuth),
    iEnableSwComp(KCDTIdDINEnableSwComp),
    iBearerName(KCDTIdDINBearerName),
    iBearerSpeed(KCDTIdDINBearerSpeed),
    iBearerCe(KCDTIdDINBearerCe),
    iInitString(KCDTIdDINInitString),
    iUseEdge(KCDTIdDINUseEdge)*/
			{}


		// 0A/  LAN Service Record
		CCDLANServiceRecord::CCDLANServiceRecord(TMDBElementId aElementId)
   : CCDServiceRecordBase(aElementId)/*,
	iIfNetworks(KCDTIdLANIfNetworks),
    iIpNetmask(KCDTIdLANIpNetMask),
    iIpGateway(KCDTIdLANIpGateway),
    iIpAddrFromServer(KCDTIdLANIpAddrFromServer),
    iIpAddr(KCDTIdLANIpAddr),
    iIpDnsAddrFromServer(KCDTIdLANIpDNSAddrFromServer),
    iIpNameServer1(KCDTIdLANIpNameServer1),
    iIpNameServer2(KCDTIdLANIpNameServer2),
    iIp6DnsAddrFromServer(KCDTIdLANIp6DNSAddrFromServer),
    iIp6NameServer1(KCDTIdLANIp6NameServer1),
    iIp6NameServer2(KCDTIdLANIp6NameServer2),
    iIpAddrLeaseValidFrom(KCDTIdLANIpAddrLeaseValidFrom),
    iIpAddrLeaseValidTo(KCDTIdLANIpAddrLeaseValidTo),
    iConfigDaemonManagerName(KCDTIdLANConfigDaemonManagerName),
    iConfigDaemonName(KCDTIdLANConfigDaemonName),
	iServiceExtensionTableName(KCDTIdLANServiceExtensionTableName),
    iServiceExtensionTableRecordId(KCDTIdLANServiceExtensionTableRecordId)*/
            {}


		// 0B/  WLAN SERVICE EXTENSION RECORD
		CCDWLANServiceExtRecord::CCDWLANServiceExtRecord(TMDBElementId aElementId)
   : CCDRecordBase(aElementId)/*,
	iWLANProfile(KCDTIdWLANProfile),
	iWLANSSID(KCDTIdWLANSSID),
	iWLANEncryptionStatus(KCDTIdWLANEncryptionStatus),
	iWLANEncryptionType(KCDTIdWLANEncryptionType),
	iWLANDefaultWepKey (KCDTIdWLANDefaultWepKey ),
	iWLANEncryptKey1Valid(KCDTIdWLANEncryptKey1Valid),
	iWLANWepKey1(KCDTIdWLANWepKey1),
	iWLANEncryptKey2Valid(KCDTIdWLANEncryptKey2Valid),
	iWLANWepKey2(KCDTIdWLANWepKey2),
	iWLANEncryptKey3Valid(KCDTIdWLANEncryptKey3Valid),
	iWLANWepKey3(KCDTIdWLANWepKey3),
	iWLANEncryptKey4Valid(KCDTIdWLANEncryptKey4Valid),
	iWLANWepKey4(KCDTIdWLANWepKey4),
	iWLANRanking(KCDTIdWLANRanking),
	iWLANDialogPref(KCDTIdWLANDialogPref),
	iWLANNetworkType(KCDTIdWLANNetworkType),
	iWLANFragThres(KCDTIdWLANFragThres),
	iWLANRTSThres(KCDTIdWLANRTSThres),
	iWLANDesTransRate(KCDTIdWLANDesTransRate),
	iWLANBeaconInt(KCDTIdWLANBeaconInt),
	iWLANChannelId(KCDTIdWLANChannelId),
	iWLANAuthenticationMode(KCDTIdWLANAuthenticationMode),
	iWLANPowerSaveMode(KCDTIdWLANPowerSaveMode),
	iWLANAllowUnencryptMode(KCDTIdWLANAllowUnencryptMode),
	iWLANAllowShortPre(KCDTIdWLANAllowShortPre),
	iWLANPreambleType(KCDTIdWLANPreambleType),
	iWLANTxPowerLevel(KCDTIdWLANTxPowerLevel),
	iWLANEapPresharedKey(KCDTIdWLANEapPresharedKey),
	iWLANRegDomain(KCDTIdWLANRegDomain),
	iWLANSecData(KCDTIdWLANSecData)*/
			{}


		// 0C/  VPN SERVICE RECORD
		CCDVPNServiceRecord::CCDVPNServiceRecord(TMDBElementId aElementId)
   : CCDServiceRecordBase(aElementId)/*,
   iServicePolicy(KCDTIdVPNPolicyName),
   iServiceIAP(KCDTIdVPNIAPRecord),
   iServiceNetwork(KCDTIdVPNNetwork)*/
			{}


        // 10/  WCDMA Packet Service Record
			CCDWCDMAPacketServiceRecord::CCDWCDMAPacketServiceRecord(TMDBElementId aElementId)
   : CCDServiceRecordBase(aElementId)/*,
   iGPRSAPN((aElementId & KCDMaskShowRecordType) | KCDTIdAPN),
    iGPRSPDPType((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMPDPType),
    iGPRSPDPAddress((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAPDPAddress),
    iGPRSReqPrecedence((aElementId & KCDMaskShowRecordType) | KCDTIdReqPrecedence),
    iGPRSReqDelay((aElementId & KCDMaskShowRecordType) | KCDTIdReqDelay),
    iGPRSReqReliability((aElementId & KCDMaskShowRecordType) | KCDTIdReqReliability),
    iGPRSReqPeakThroughput((aElementId & KCDMaskShowRecordType) | KCDTIdReqPeakThroughput),
    iGPRSReqMeanThroughput((aElementId & KCDMaskShowRecordType) | KCDTIdReqMeanThroughput),
    iGPRSMinPrecedence((aElementId & KCDMaskShowRecordType) | KCDTIdMinPrecedence),
    iGPRSMinDelay((aElementId & KCDMaskShowRecordType) | KCDTIdMinDelay),
    iGPRSMinReliability((aElementId & KCDMaskShowRecordType) | KCDTIdMinReliability),
    iGPRSMinPeakThroughput((aElementId & KCDMaskShowRecordType) | KCDTIdMinPeakThroughput),
    iGPRSMinMeanThroughput((aElementId & KCDMaskShowRecordType) | KCDTIdMinMeanThroughput),
    iGPRSDataCompression((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMADataCompression),
    iGPRSHeaderCompression((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAHeaderCompression),
    iGPRSUseEdge((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAUseEdge),
    iGPRSAnonymousAccess((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAAnonymousAccess),
    iGPRSIfParams((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIfParams),
    iGPRSIfNetworks((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIfNetworks),
    iGPRSIfPromptForAuth((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIfPromptForAuth),
    iGPRSIfAuthName((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIfAuthName),
    iGPRSIfAuthPass((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIfAuthPass),
    iGPRSIfAuthRetries((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIfAuthRetries),
    iGPRSIPNetMask((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIPNetMask),
    iGPRSIPGateway((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIPGateway),
    iGPRSIPAddrFromServer((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIPAddrFromServer),
    iGPRSIPAddr((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIPAddr),
    iGPRSIPDNSAddrFromServer((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIPDNSAddrFromServer),
    iGPRSIPNameServer1((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIPNameServer1),
    iGPRSIPNameServer2((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIPNameServer2),
    iGPRSIP6DNSAddrFromServer((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIP6DNSAddrFromServer),
    iGPRSIP6NameServer1((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIP6NameServer1),
    iGPRSIP6NameServer2((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIP6NameServer2),
    iGPRSIPAddrLeaseValidFrom((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIPAddrLeaseValidFrom),
    iGPRSIPAddrLeaseValidTo((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAIPAddrLeaseValidTo),
    iGPRSConfigDaemonManagerName((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAConfigDaemonManagerName),
    iGPRSConfigDaemonName((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAConfigDaemonName),
    iGPRSEnableLCPExtension((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMAEnableLCPExtensions),
    iGPRSDisablePlainTextAuth((aElementId & KCDMaskShowRecordType) | KCDTIdWCDMADisablePlainTextAuth),
    iGPRSAPType((aElementId & KCDMaskShowRecordType) | KCDTIdAPType),
    iGPRSQOSWarningTimeOut((aElementId & KCDMaskShowRecordType) | KCDTIdQOSWarningTimeOut),
    iUmtsR99QoSAndOnTable((aElementId & KCDMaskShowRecordType) | KCDTIdUmtsR99QoSAndOn)*/

				{}

		// 10.a/  Outgoing GPRS Table
			CCDOutgoingGprsRecord::CCDOutgoingGprsRecord(TMDBElementId aElementId)
		   : CCDWCDMAPacketServiceRecord(aElementId)
		   		{}


		// 10.b/  Incoming GPRS Table
			CCDIncomingGprsRecord::CCDIncomingGprsRecord(TMDBElementId aElementId)
		   : CCDWCDMAPacketServiceRecord(aElementId)
		   		{}

		// 10.c/
			CCDUmtsR99QoSAndOnTableRecord::CCDUmtsR99QoSAndOnTableRecord(TMDBElementId aElementId)
	:  CCDRecordBase(aElementId)/*,
	iGPRSReqTrafficClass(KCDTIdWCDMAReqTrafficClass),
	iGPRSMinTrafficClass(KCDTIdWCDMAMinTrafficClass),
	iGPRSReqDeliveryOrder(KCDTIdWCDMAReqDeliveryOrder),
	iGPRSMinDeliveryOrder(KCDTIdWCDMAMinDeliveryOrder),
	iGPRSReqDeliverErroneousSDU(KCDTIdWCDMAReqDeliverErroneousSDU),
	iGPRSMinDeliverErroneousSDU(KCDTIdWCDMAMinDeliverErroneousSDU),
	iGPRSReqMaxSDUSize(KCDTIdWCDMAReqMaxSDUSize),
	iGPRSMinAcceptableMaxSDUSize(KCDTIdWCDMAMinAcceptableMaxSDUSize),
	iGPRSReqMaxUplinkRate(KCDTIdWCDMAReqMaxUplinkRate),
	iGPRSReqMinUplinkRate(KCDTIdWCDMAReqMinUplinkRate),
	iGPRSReqMaxDownlinkRate(KCDTIdWCDMAReqMaxDownlinkRate),
	iGPRSReqMinDownlinkRate(KCDTIdWCDMAReqMinDownlinkRate),
	iGPRSReqBER(KCDTIdWCDMAReqBER),
	iGPRSMaxBER(KCDTIdWCDMAMaxBER),
	iGPRSReqSDUErrorRatio(KCDTIdWCDMAReqSDUErrorRatio),
	iGPRSMaxSDUErrorRatio(KCDTIdWCDMAMaxSDUErrorRatio),
	iGPRSReqTrafficHandlingPriority(KCDTIdWCDMAReqTrafficHandlingPriority),
	iGPRSMinTrafficHandlingPriority(KCDTIdWCDMAMinTrafficHandlingPriority),
	iGPRSReqTransferDelay(KCDTIdWCDMAReqTransferDelay),
	iGPRSMaxTransferDelay(KCDTIdWCDMAMaxTransferDelay),
	iGPRSReqGuaranteedUplinkRate(KCDTIdWCDMAReqGuaranteedUplinkRate),
	iGPRSMinGuaranteedUplinkRate(KCDTIdWCDMAMinGuaranteedUplinkRate),
	iGPRSReqGuaranteedDownlinkRate(KCDTIdWCDMAReqGuaranteedDownlinkRate),
	iGPRSMinGuaranteedDownlinkRate(KCDTIdWCDMAMinGuaranteedDownlinkRate),
	iGPRSSignallingIndication(KCDTIdWCDMASignallingIndication),
	iGPRS_ImCnSignallingIndication(KCDTIdWCDMAImCmSignallingIndication),
	iGPRSSourceStatisticsDescriptor(KCDTIdWCDMASourceStatisticsDescriptor)*/
					{}


		    // 12/ Default WCDMA
		    CCDDefaultWCDMARecord::CCDDefaultWCDMARecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
	iUsage(KCDTIdDefaultGPRSUsage),
	iAPN(KCDTIdDefaultGPRSAPN),
	iPdpType(KCDTIdDefaultGPRSPDPType),
	iPdpAddress(KCDTIdDefaultGPRSPDPAddress),
	iPrecedence(KCDTIdDefaultGPRSPrecedence),
	iDelay(KCDTIdDefaultGPRSDelay),
	iReliability(KCDTIdDefaultGPRSReliability),
	iPeakThroughput(KCDTIdDefaultGPRSPeakThroughput),
	iMeanThroughput(KCDTIdDefaultGPRSMeanThroughput),
	iMinPrecedence(KCDTIdDefaultGPRSMinPrecedence),
	iMinDelay(KCDTIdDefaultGPRSMinDelay),
	iMinReliability(KCDTIdDefaultGPRSMinReliability),
	iMinPeakThroughput(KCDTIdDefaultGPRSMinPeakThroughput),
	iMinMeanThroughput(KCDTIdDefaultGPRSMinMeanThroughput),
	iDataCompression(KCDTIdDefaultGPRSDataCompression),
	iHeaderCompression(KCDTIdDefaultGPRSHeaderCompression),
	iUseEdge(KCDTIdDefaultGPRSUseEdge),
	iAnonymousAccess(KCDTIdDefaultGPRSAnonymousAccess)*/
				{}


            // BEARERS

            // 13   Modem Bearer
    CCDModemBearerRecord::CCDModemBearerRecord(TMDBElementId aElementId)
	: CCDBearerRecordBase(aElementId)/*,
	iNifName(KCDTIdNifName),
    iPortName(KCDTIdPortName),
    iTsyName(KCDTIdTsyName),
    iCsyName(KCDTIdCsyName),
    iLastSocketActivityTimeout(KCDTIdLastSocketActivityTimeout),
    iLastSessionClosedTimeout(KCDTIdLastSessionClosedTimeout),
    iLastSocketClosedTimeout(KCDTIdLastSocketClosedTimeout),
    iDataBits(KCDTIdDataBits),
    iStopBits(KCDTIdStopBits),
    iParity(KCDTIdParity),
    iRate(KCDTIdRate),
    iHandshaking(KCDTIdHandshaking),
    iSpecialRate(KCDTIdSpecialRate),
    iXonChar(KCDTIdXonChar),
    iXoffChar(KCDTIdXoffChar),
    iFaxClassPref(KCDTIdFaxClassPref),
    iSpeakerPref(KCDTIdSpeakerPref),
    iSpeakerVolPref(KCDTIdSpeakerVolPref),
    iModemInitString(KCDTIdModemInitString),
    iDataInitString(KCDTIdDataInitString),
    iFaxInitString(KCDTIdFaxInitString),
    iIspInitString(KCDTIdIspInitString),
    iDialPauseLength(KCDTIdDialPauseLength),
    iCarrierTimeOut(KCDTIdCarrierTimeOut),
    iAutoAnswerRingCount(KCDTIdAutoAnswerRingCount),
    iSpeakerVolControlLow(KCDTIdSpeakerVolControlLow),
    iSpeakerVolControlMedium(KCDTIdSpeakerVolControlMedium),
    iSpeakerVolControlHigh(KCDTIdSpeakerVolControlHigh),
    iSpeakerAlwaysOff(KCDTIdSpeakerAlwaysOff),
    iSpeakerOnUntilCarrier(KCDTIdSpeakerOnUntilCarrier),
    iSpeakerAlwaysOn(KCDTIdSpeakerAlwaysOn),
    iSpeakerOnAfterUntilCarrier(KCDTIdSpeakerOnAfterUntilCarrier),
    iDialToneWaitModifier(KCDTIdDialToneWaitModifier),
    iCallProgress1(KCDTIdCallProgress1),
    iCallProgress2(KCDTIdCallProgress2),
    iCallProgress3(KCDTIdCallProgress3),
    iCallProgress4(KCDTIdCallProgress4),
    iEchoOff(KCDTIdEchoOff),
    iVerboseText(KCDTIdVerboseText),
    iQuietOff(KCDTIdQuietOff),
    iQuietOn(KCDTIdQuietOn),
    iDialCommandStateModifier(KCDTIdDialCommandStateModifier),
    iOnLine(KCDTIdOnLine),
    iResetConfiguration(KCDTIdResetConfiguration),
    iReturnToFactoryDefs(KCDTIdReturnToFactoryDefs),
    iDcdOnDuringLink(KCDTIdDcdOnDuringLink),
    iDtrHangUp(KCDTIdDtrHangUp),
    iDsrAlwaysOn(KCDTIdDsrAlwaysOn),
    iRtsCtsHandshake(KCDTIdRtsCtsHandshake),
    iXonXoffHandshake(KCDTIdXonXoffHandshake),
    iEscapeCharacter(KCDTIdEscapeCharacter),
    iEscapeGuardPeriod(KCDTIdEscapeGuardPeriod),
    iFaxClassInterrogate(KCDTIdFaxClassInterrogate),
    iFaxClass(KCDTIdFaxClass),
    iNoDialTone(KCDTIdNoDialTone),
    iBusy(KCDTIdBusy),
    iNoAnswer(KCDTIdNoAnswer),
    iCarrier(KCDTIdCarrier),
    iConnect(KCDTIdConnect),
    iCompressionClass5(KCDTIdCompressionClass5),
    iCompressionV42bis(KCDTIdCompressionV42bis),
    iCompressionNone(KCDTIdCompressionNone),
    iProtocolLapd(KCDTIdProtocolLapd),
    iProtocolAlt(KCDTIdProtocolAlt),
    iProtocolAltcellular(KCDTIdProtocolAltcellular),
    iProtocolNone(KCDTIdProtocolNone),
    iMessageCentreNumber(KCDTIdMessageCentreNumber),
    iMessageValidityPeriod(KCDTIdMessageValidityPeriod),
    iMessageDeliveryReport(KCDTIdMessageDeliveryReport),
    iMinSignalLevel(KCDTIdMinSignalLevel),
    iCommRole(KCDTIdCommRole),
    iControlChannelPortName(KCDTIdControlChannelPortName),
    iSirSettings(KCDTIdSirSettings),
	iBcaStack(KCDTIdBCAStack),
	iBearerTechnology((aElementId & KCDMaskShowRecordType) | KCDTIdBearerTechnology ) */
    		{}


		// 14/ LAN BEARER RECORD
CCDLANBearerRecord::CCDLANBearerRecord(TMDBElementId aElementId)
  : CCDBearerRecordBase(aElementId)/*,
    iLanBearerNifName(KCDTIdLANBearerNifName),
    iLanBearerLddFilename(KCDTIdLANBearerLddFilename),
    iLanBearerLddName(KCDTIdLANBearerLddName),
    iLanBearerPddFilename(KCDTIdLANBearerPddFilename),
    iLanBearerPddName(KCDTIdLANBearerPddName),
    iLanBearerPacketDriverName(KCDTIdLANBearerPacketDriverName),
    iLastSocketActivityTimeout(KCDTIdLANLastSocketActivityTimeout),
    iLastSessionClosedTimeout(KCDTIdLANLastSessionClosedTimeout),
	iLastSocketClosedTimeout(KCDTIdLANLastSocketClosedTimeout),
	iBearerTechnology((aElementId & KCDMaskShowRecordType) | KCDTIdBearerTechnology ) */
		    {}



        //	15/  Virtual Bearer
CCDVirtualBearerRecord::CCDVirtualBearerRecord(TMDBElementId aElementId)
    :CCDBearerRecordBase(aElementId)/*,
    iVirtualBearerNifName(KCDTIdVirtualBearerNifName),
	iLastSocketActivityTimeout(KCDTIdVBLastSocketActivityTimeout),
	iLastSessionClosedTimeout(KCDTIdVBLastSessionClosedTimeout),
	iLastSocketClosedTimeout(KCDTIdVBLastSocketClosedTimeout),
	iBearerTechnology((aElementId & KCDMaskShowRecordType) | KCDTIdBearerTechnology ) */
				{}

	    // 16/ WAP SMS Bearer
CCDWAPSMSBearerRecord::CCDWAPSMSBearerRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
	iWAPAccessPointId(KCDTIdWAPSMSAccessPointId),
	iWAPGatewayAddress(KCDTIdWAPSMSGatewayAddress),
	iWAPServiceCentreAddress(KCDTIdWAPSMSServiceCentreAddress),
	iWAPWSPOption(KCDTIdWAPSMSWSPOption),
	iWAPSecurity(KCDTIdWAPSMSSecurity) */
				{}


	    // 	17/ WAP IP Bearer
CCDWAPIPBearerRecord::CCDWAPIPBearerRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
	iWAPAccessPointId(KCDTIdWAPAccessPointId),
	iWAPGatewayAddress(KCDTIdWAPGatewayAddress),
	iWAPIAP(KCDTIdWAPIAP),
	iWAPWSPOption(KCDTIdWAPWSPOption),
	iWAPSecurity(KCDTIdWAPSecurity),
	iWAPProxyPort(KCDTIdWAPProxyPort),
	iWAPProxyLoginName(KCDTIdWAPProxyLoginName),
	iWAPProxyLoginPass(KCDTIdWAPProxyLoginPass)*/
				{}


// 18/ Chargecard
CCDChargecardRecord::CCDChargecardRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
	iAccountNo(KCDTIdAccountNo),
	iPin(KCDTIdPin),
	iLocalRule(KCDTIdLocalRule),
	iNatRule(KCDTIdNatRule),
	iIntlRule(KCDTIdIntlRule)*/
	{}

// 19/ Proxies Record
CCDProxiesRecord::CCDProxiesRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
	iService(KCDTIdISP),
	iServiceType(KCDTIdProxyServiceType),
	iUseProxyServer(KCDTIdUseProxyServer),
	iServerName(KCDTIdServerName),
	iProtocolName(KCDTIdProtocolName),
	iPortNumber(KCDTIdPortNumber),
	iExceptions(KCDTIdExceptions) */
	{}


// 1A/ Secure Socket
CCDSecureSocketRecord::CCDSecureSocketRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
	iSSProtoName(KCDTIdSSProtoName),
	iSSProtoLibrary(KCDTIdSSProtoLibrary)*/
	{}

// 1B/ Agent Lookup
CCDAgentLookupRecord::CCDAgentLookupRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
	iAgentFriendlyName(KCDTIdAgentFriendlyName),
	iAgentFileName(KCDTIdAgentFileName)*/
	{}

// 1C/ CCDPANServiceExtRecord
CCDPANServiceExtRecord::CCDPANServiceExtRecord(TMDBElementId aElementId)
	:CCDServiceRecordBase(aElementId)/*,
	iLocalRole(KCDTIdLocalRole),
	iPeerRole(KCDTIdPeerRole),
	iPeerMACAddresses(KCDTIdPeerMACAddresses),
	iPromptForRemoteDevices(KCDTIdPromptForRemoteDevices),
	iDisableSdpQuery(KCDTIdDisableSdpQuery),
	iAllowIncoming(KCDTIdAllowIncoming),
	iPromptIfMACListFails(KCDTIdPromptIfMACListFails)*/
	{}

// 1D/ SNAP RECORD
CCDAccessPointRecord::CCDAccessPointRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,			
	iAccessPointGID(KCDTIdAccessPointGID),
	iSelectionPolicy(KCDTIdSelectionPolicy)
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	,iTier(KCDTIdTier),
	iMCpr(KCDTIdMCpr),
	iCpr(KCDTIdCpr),
	iSCpr(KCDTIdSCpr),
	iProtocol(KCDTIdProtocol),
	iCprConfig(KCDTIdCprConfig),
	iAppSID(KCDTIdAppSID),
	iConfigAPIdList(KCDTIdConfigAPIdList),
	iPriority(KCDTIdAccessPointPriority)
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY */
	{}

// 1E IAP PRIORITY SELECTION POLICY RECORD
		CCDSelectionPolicyRecordBase::CCDSelectionPolicyRecordBase(TMDBElementId aElementId)
			:CCDRecordBase(aElementId)
				{}
		
// 1F IAP PRIORITY SELECTION POLICY RECORD
CCDIAPPrioritySelectionPolicyRecord::CCDIAPPrioritySelectionPolicyRecord(TMDBElementId aElementId)
	:CCDSelectionPolicyRecordBase(aElementId)/*,
	iIap1(KCDTIdIap1),
	iIap2(KCDTIdIap2),
	iIap3(KCDTIdIap3),
	iIap4(KCDTIdIap4),
	iIap5(KCDTIdIap5),
	iIap6(KCDTIdIap6),
	iIap7(KCDTIdIap7),
	iIap8(KCDTIdIap8),
	iIap9(KCDTIdIap9),
	iIap10(KCDTIdIap10),
	iIap11(KCDTIdIap11),
	iIap12(KCDTIdIap12),
	iIap13(KCDTIdIap13),
	iIap14(KCDTIdIap14),
	iIap15(KCDTIdIap15),
	iIapCount(KCDTIdIapCount)*/
	{}    	

// 20/  EAP SECURITY SETTINGS RECORD
CCDBaseSecRecord::CCDBaseSecRecord(TMDBElementId aElementId)
   :CCDRecordBase(aElementId)
	{}

// 21/  EAP SECURITY SETTINGS RECORD
CCDEAPSecRecord::CCDEAPSecRecord(TMDBElementId aElementId)
   :CCDBaseSecRecord(aElementId)/*,
	iEAPSecOuterEAPType(KCDTIdEAPSecOuterEAPType),
	iEAPSecId(KCDTIdEAPSecId),
	iEAPSecPassword(KCDTIdEAPSecPassword),
	iEAPSecConfigId(KCDTIdEAPSecConfigId),
	iEAPSecData(KCDTIdEAPSecData)*/
	{}

// 22/  TUNNELLED EAP SETTINGS RECORD
CCDTunEAPRecord::CCDTunEAPRecord(TMDBElementId aElementId)
   :CCDBaseSecRecord(aElementId)/*,
	iTUNEAPInnerType(KCDTIdTUNEAPInnerType),
	iTUNEAPData(KCDTIdTUNEAPData)*/
	{}

// 23/  EAP-TLS SETTINGS RECORD
CCDEAPTLSRecord::CCDEAPTLSRecord(TMDBElementId aElementId)
   :CCDBaseSecRecord(aElementId)/*,
	iEAPTLSClientCertName(KCDTIdEAPTLSClientCertName)*/
	{}

// 24/  LEAP SETTINGS RECORD
CCDLEAPRecord::CCDLEAPRecord(TMDBElementId aElementId)
   :CCDBaseSecRecord(aElementId)/*,
	iLEAPTimeout(KCDTIdLEAPTimeout)*/
	{}

CCDEapSimProtocolRecord::CCDEapSimProtocolRecord(TMDBElementId aElementId)
	: CCDRecordBase(aElementId)/*,
    iProtocolId(KCDTIdEapSimProtocolId),
    iPseudonym(KCDTIdEapSimPseudonym),
    iUseNaiRealm(KCDTIdEapSimUseNaiRealm),
    iNaiRealm(KCDTIdEapSimNaiRealm),
    iMinRands(KCDTIdEapSimMinRands)*/
{}


CCDEapAkaProtocolRecord::CCDEapAkaProtocolRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
	 iProtocolId(KCDTIdEapAkaProtocolId),
	 iPseudonym(KCDTIdEapAkaPseudonym),
	 iUseNaiRealm(KCDTIdEapAkaUseNaiRealm),
	 iNaiRealm(KCDTIdEapAkaNaiRealm)*/
{}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY    	
// AP PRIORITY SELECTION POLICY RECORD
CCDAPPrioritySelectionPolicyRecord::CCDAPPrioritySelectionPolicyRecord(TMDBElementId aElementId)
	:CCDSelectionPolicyRecordBase(aElementId)/*,
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
		iApCount(KCDTIdApCount)*/
		{}
						
CCDTierRecord::CCDTierRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
		iTierThreadName(KCDTIdTierThreadName),
		iTierManagerName(KCDTIdTierManagerName),
		iPromptUser(KCDTIdPromptUser),
		iDefaultAccessPoint(KCDTIdDefaultAccessPoint)*/
		{}
CCDMCprRecord::CCDMCprRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
		iMCprUid(KCDTIdMCprUid)*/
		{}
CCDCprRecord::CCDCprRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
		iCprUid(KCDTIdCprUid)*/
		{}
  	

CCDSCprRecord::CCDSCprRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
		iSCprUid(KCDTIdSCprUid)*/
		{}
CCDProtocolRecord::CCDProtocolRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
                        iProtocolUid(KCDTIdProtocolUid),
			iProtocolConfigLoaderUid(KCDTIdProtocolConfigLoaderUid)*/
		{}

// 140/
CCDBearerTypeRecord::CCDBearerTypeRecord(TMDBElementId aElementId)
	: CCDRecordBase(aElementId)/*,
		iTier(KCDTIdBearerTypeTier),
		iMCpr(KCDTIdBearerTypeMCpr),
		iCpr(KCDTIdBearerTypeCpr),
		iSCpr(KCDTIdBearerTypeSCpr),
		iProtocol(KCDTIdBearerTypeProtocol)*/
		{}

CCDConfigAccessPointRecord::CCDConfigAccessPointRecord(TMDBElementId aElementId)
   : CCDRecordBase(aElementId)/*,
	  iSCpr(KCDTIdConfigAccessPointSCpr),
	  iProtocol(KCDTIdConfigAccessPointProtocol),
	  iProtocolConfig(KCDTIdConfigAccessPointProtocolConfig),
	  iLayerBelow(KCDTIdConfigAccessPointLayerBelow)*/
      {}

CCDPolicySelectorRecord::CCDPolicySelectorRecord(TMDBElementId aElementId)
			:CCDRecordBase(aElementId)/*,
				iPolicyId(KCDTIdPolicyId),
				iAppUid(KCDTIdAppUid),
				iSrcAddress(KCDTIdSrcAddress),
				iSrcMask(KCDTIdSrcMask),
				iDstAddress(KCDTIdDstAddress),
				iDstMask(KCDTIdDstMask),
				iSrcPort(KCDTIdSrcPort),
				iDstPort(KCDTIdDstPort),
				iSrcPortMax(KCDTIdSrcPortMax),
				iDstPortMax(KCDTIdDstPortMax),
				iProtocolId(KCDTIdProtocolId),
				iIapId(KCDTIdIAPid),
				iPriority(KCDTIdPriority)*/
				{}		

CCDPolicySelector2ParamsRecord::CCDPolicySelector2ParamsRecord(TMDBElementId aElementId)
	:CCDRecordBase(aElementId)/*,
		iPolicySelectorId(KCDTIdPolicySelectorId),
		iParamsId(KCDTIdParamsId)*/
		{}		

CCDGenericQosRecord::CCDGenericQosRecord(TMDBElementId aElementId)
	:CCDParamsRecordBase(aElementId)/*,
		iDownlinkBandwidth(KCDTIdDownlinkBandwidth),
		iUplinkBandwidth(KCDTIdUplinkBandwidth),
		iDownLinkMaximumBurstSize(KCDTIdDownLinkMaximumBurstSize),
		iUpLinkMaximumBurstSize(KCDTIdUpLinkMaximumBurstSize),
		iDownLinkAveragePacketSize(KCDTIdDownLinkAveragePacketSize),
		iUpLinkAveragePacketSize(KCDTIdUpLinkAveragePacketSize),
		iDownLinkMaximumPacketSize(KCDTIdDownLinkMaximumPacketSize),
		iUpLinkMaximumPacketSize(KCDTIdUpLinkMaximumPacketSize),
		iDownLinkDelay(KCDTIdDownLinkDelay),
		iUpLinkDelay(KCDTIdUpLinkDelay),
		iDownLinkDelayVariation(KCDTIdDownLinkDelayVariation),
		iUpLinkDelayVariation(KCDTIdUpLinkDelayVariation),
		iDownLinkPriority(KCDTIdDownLinkPriority),
		iUpLinkPriority(KCDTIdUpLinkPriority),
		iHeaderMode(KCDTIdHeaderMode)
//				iQosName(KCDTIdQosName)*/
		{}	

/*
		CCDEapSimProtocolRecord::CCDEapSimProtocolRecord(TMDBElementId aElementId)
			: CCDRecordBase(aElementId),
			  iProtocolId(KCDTIdEapSimProtocolId),
			  iPseudonym(KCDTIdEapSimPseudonym),
			  iUseNaiRealm(KCDTIdEapSimUseNaiRealm),
			  iNaiRealm(KCDTIdEapSimNaiRealm),
			  iMinRands(KCDTIdEapSimMinRands)
		{}

		CCDEapAkaProtocolRecord::CCDEapAkaProtocolRecord(TMDBElementId aElementId)
			:CCDRecordBase(aElementId),
			 iProtocolId(KCDTIdEapAkaProtocolId),
			 iPseudonym(KCDTIdEapAkaPseudonym),
			 iUseNaiRealm(KCDTIdEapAkaUseNaiRealm),
			 iNaiRealm(KCDTIdEapAkaNaiRealm)
		{}	

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY    	
    	// AP PRIORITY SELECTION POLICY RECORD
		CCDAPPrioritySelectionPolicyRecord::CCDAPPrioritySelectionPolicyRecord(TMDBElementId aElementId)
			:CCDSelectionPolicyRecordBase(aElementId),
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
				iApCount(KCDTIdApCount)
				{}
						
		CCDMCprRecord::CCDMCprRecord(TMDBElementId aElementId)
			:CCDRecordBase(aElementId),
				iMCprUid(KCDTIdMCprUid)
				{}

		CCDCprRecord::CCDCprRecord(TMDBElementId aElementId)
			:CCDRecordBase(aElementId),
				iCprUid(KCDTIdCprUid)
				{}

		CCDSCprRecord::CCDSCprRecord(TMDBElementId aElementId)
			:CCDRecordBase(aElementId),
				iSCprUid(KCDTIdSCprUid)
				{}

		CCDProtocolRecord::CCDProtocolRecord(TMDBElementId aElementId)
			:CCDRecordBase(aElementId),
                                iProtocolUid(KCDTIdProtocolUid),
			        iProtocolConfigLoaderUid(KCDTIdProtocolConfigLoaderUid)
				{}

    	// 140/
		CCDBearerTypeRecord::CCDBearerTypeRecord(TMDBElementId aElementId)
			: CCDRecordBase(aElementId),
				iTier(KCDTIdBearerTypeTier),
				iMCpr(KCDTIdBearerTypeMCpr),
				iCpr(KCDTIdBearerTypeCpr),
				iSCpr(KCDTIdBearerTypeSCpr),
				iProtocol(KCDTIdBearerTypeProtocol)
				{}

                CCDConfigAccessPointRecord::CCDConfigAccessPointRecord(TMDBElementId aElementId)
                        : CCDRecordBase(aElementId),
			  iSCpr(KCDTIdConfigAccessPointSCpr),
			  iProtocol(KCDTIdConfigAccessPointProtocol),
			  iProtocolConfig(KCDTIdConfigAccessPointProtocolConfig),
			  iLayerBelow(KCDTIdConfigAccessPointLayerBelow)
                                {}

		CCDPolicySelectorRecord::CCDPolicySelectorRecord(TMDBElementId aElementId)
			:CCDRecordBase(aElementId),
				iPolicyId(KCDTIdPolicyId),
				iAppUid(KCDTIdAppUid),
				iSrcAddress(KCDTIdSrcAddress),
				iSrcMask(KCDTIdSrcMask),
				iDstAddress(KCDTIdDstAddress),
				iDstMask(KCDTIdDstMask),
				iSrcPort(KCDTIdSrcPort),
				iDstPort(KCDTIdDstPort),
				iSrcPortMax(KCDTIdSrcPortMax),
				iDstPortMax(KCDTIdDstPortMax),
				iProtocolId(KCDTIdProtocolId),
				iIapId(KCDTIdIAPid),
				iPriority(KCDTIdPriority)
				{}		

		CCDPolicySelector2ParamsRecord::CCDPolicySelector2ParamsRecord(TMDBElementId aElementId)
			:CCDRecordBase(aElementId),
				iPolicySelectorId(KCDTIdPolicySelectorId),
				iParamsId(KCDTIdParamsId)
				{}		

		CCDGenericQosRecord::CCDGenericQosRecord(TMDBElementId aElementId)
			:CCDParamsRecordBase(aElementId),
				iDownlinkBandwidth(KCDTIdDownlinkBandwidth),
				iUplinkBandwidth(KCDTIdUplinkBandwidth),
				iDownLinkMaximumBurstSize(KCDTIdDownLinkMaximumBurstSize),
				iUpLinkMaximumBurstSize(KCDTIdUpLinkMaximumBurstSize),
				iDownLinkAveragePacketSize(KCDTIdDownLinkAveragePacketSize),
				iUpLinkAveragePacketSize(KCDTIdUpLinkAveragePacketSize),
				iDownLinkMaximumPacketSize(KCDTIdDownLinkMaximumPacketSize),
				iUpLinkMaximumPacketSize(KCDTIdUpLinkMaximumPacketSize),
				iDownLinkDelay(KCDTIdDownLinkDelay),
				iUpLinkDelay(KCDTIdUpLinkDelay),
				iDownLinkDelayVariation(KCDTIdDownLinkDelayVariation),
				iUpLinkDelayVariation(KCDTIdUpLinkDelayVariation),
				iDownLinkPriority(KCDTIdDownLinkPriority),
				iUpLinkPriority(KCDTIdUpLinkPriority),
				iHeaderMode(KCDTIdHeaderMode)
//				iQosName(KCDTIdQosName)
				{}	
*/

		CCDWifiScanEngineRecord::CCDWifiScanEngineRecord(TMDBElementId aElementId)
			:CCDRecordBase(aElementId),
				iScanPeriodMs(KCDTIdScanPeriodMs),
				iRSSIMin(KCDTIdRSSIMin),
				iRSSIMax(KCDTIdRSSIMax)
				{}

#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
//EOF
