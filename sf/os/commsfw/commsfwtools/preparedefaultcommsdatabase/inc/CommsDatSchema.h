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
//

/**
 * @file CommsDatSchema.h
 *
 * Declaration of the comms data schema
 *
 * @internalComponent
 * @released
 *
 */

#if (! defined COMMSDATSCHEMA_H)
#define        COMMSDATSCHEMA_H

#include <commsdattypeinfov1_1.h>
#include <commsdattypeinfov1_1_internal.h>

namespace CommsDat
{
    // table lookup structure 
    typedef struct 
	    {
	    TMDBElementId				        iTableId;
	    const TText* 						iTableName;
      	const CommsDat::SRecordTypeInfo*	iFields;
      } STableLookup;


NONSHARABLE_CLASS(CommsDatSchemaV1_1)
/*
CommsDatSchemaV1_1 encapsulates

  Schema (Version 1.1) of tables in the Comms Database
	Linking table id with table name and field info
*/
	{

	public :

        //
        // Field Linktypes
        //
        typedef enum
        {
#ifdef SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
// Integer record links used up to OS 9.3
            /* links for particular record types (can be extended as necessary)*/
            ELinkIAPRecord          = EInt,
            ELinkAPRecord           = EInt,
            ELinkEAPSecRecord       = EInt,
            ELinkWAPAPRecord        = EInt,
            ELinkModemBearerRecord  = EInt,
            ELinkAgentLookupRecord  = EInt,
            ELinkNetworkRecord      = EInt,
            ELinkLocationRecord     = EInt,
            ELinkChargecardRecord   = EInt,
            ELinkUMTSR99QoSRecord   = EInt
#else
            /* links for particular record types (can be extended as necessary)*/
            ELinkIAPRecord          = ELink | KCDTIdIAPRecord,
            ELinkAPRecord           = ELink | KCDTIdAccessPointRecord,
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
            ELinkTierRecord         = ELink | KCDTIdTierRecord,
            ELinkMCprRecord         = ELink | KCDTIdMCprRecord,
            ELinkCprRecord          = ELink | KCDTIdCprRecord,            
            ELinkSCprRecord         = ELink | KCDTIdSCprRecord,            
            ELinkProtocolRecord     = ELink | KCDTIdProtocolRecord,
            ELinkCAPRecord          = ELink | KCDTIdConfigAccessPointRecord,
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
            ELinkEAPSecRecord       = ELink | KCDTIdEAPSecRecord,
            ELinkWAPAPRecord        = ELink | KCDTIdWAPAccessPointRecord,
            ELinkModemBearerRecord  = ELink | KCDTIdModemBearerRecord,
            ELinkAgentLookupRecord  = ELink | KCDTIdAgentLookupRecord,
            ELinkNetworkRecord      = ELink | KCDTIdNetworkRecord,
            ELinkLocationRecord     = ELink | KCDTIdLocationRecord,
            ELinkChargecardRecord   = ELink | KCDTIdChargecardRecord,
            ELinkUMTSR99QoSRecord   = ELink | KCDTIdUmtsR99QoSAndOnTableRecord
#endif // SYMBIAN_COMMSDAT_USE_INT_RECORD_LINKS
        } TCDFieldLinkTypes;
		/*
		1/ Schema of tables in the database
		*/

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		static const SRecordTypeInfo iParamsRecordBaseInfo[];				// base class for all PARAMS tables
#endif
		static const SRecordTypeInfo iServiceRecordBaseInfo[];			// base class for all SERVICE tables
		static const SRecordTypeInfo iBearerRecordBaseInfo[];				// base class for all BEARER tables
		static const SRecordTypeInfo iConnectionPrefsRecordInfo[];		// Connection Preferences table - config for client preferences for connection behaviour
		static const SRecordTypeInfo iGlobalSettingsRecordInfo[];			// Global settings - contains single record - config for global settings for comms behaviour
		static const SRecordTypeInfo iNetworkRecordInfo[];				// Network table - config for service network information
		static const SRecordTypeInfo iLocationRecordInfo[];				// Location table - config for host phone location
		static const SRecordTypeInfo iIAPRecordInfo[];					// IAP table - config for a connection at the link layer, oonnects service, bearer, location and network information
		static const SRecordTypeInfo iWAPAccessPointRecordInfo[];			// WAP access point table - config for WAP access point connection
		static const SRecordTypeInfo iDialOutISPRecordInfo[];				// SERVICE: DialOut ISP table - config to allow dial-up connection with remote ISP
		static const SRecordTypeInfo iDialInISPRecordInfo[];				// SERVICE: DialIn ISP table - config to allow dial-back from remote ISP (testing only)
		static const SRecordTypeInfo iLANServiceRecordInfo[];				// SERVICE: LAN Service table - config for a LAN service
		static const SRecordTypeInfo iWLANServiceExtRecordInfo[];			// SERVICE: WLAN Service table - config for a WLAN service
		static const SRecordTypeInfo iVPNServiceRecordInfo[];				// SERVICE: VPN Service table - config for a VPN service
		static const SRecordTypeInfo iUmtsR99QoSAndOnTableRecordInfo[];	// UMTSR99QoSAndOn Table - config for QoS parameters for default context
		static const SRecordTypeInfo iWCDMAPacketServiceRecordInfo[];		// SERVICE:	WCDMA Packet service table - config for a WCDMA packet service (GPRS) connection
		static const SRecordTypeInfo iDefaultWCDMARecordInfo[];			// Default WCDMA table - default settings for WCDMA service (only one record in this table)
		static const SRecordTypeInfo iModemBearerRecordInfo[];				// BEARER: Modem Bearer table - settings for any modem bearer
		static const SRecordTypeInfo iLANBearerRecordInfo[];				// BEARER: LAN bearer table - settings for any LAN bearer
		static const SRecordTypeInfo iVirtualBearerRecordInfo[];			// BEARER: Virtual Bearer table - settings for any virtual bearer
		static const SRecordTypeInfo iWAPSMSBearerRecordInfo[];			// WAP: WAP SMS Bearer table - settings for a WAP SMS bearer
		static const SRecordTypeInfo iWAPIPBearerRecordInfo[];				// WAP: WAP IP Bearer table - settings for a WAP IP bearer
		static const SRecordTypeInfo iChargecardRecordInfo[];				// Chargecard table - settings for chargecard payment
		static const SRecordTypeInfo iProxiesRecordInfo[];					// Proxies table - settings for HTTP and other proxies
		static const SRecordTypeInfo iSecureSocketRecordInfo[];			// @deprecated - Secure Socket table - settings for SSL/TLS
		static const SRecordTypeInfo iAgentLookupRecordInfo[];				// @deprecated - settings for AgentLookupTable
		static const SRecordTypeInfo iPANServiceExtRecordInfo[];			// PAN service extension table - extension table to use PAN via LAN service
		static const SRecordTypeInfo iAccessPointRecordInfo[];				// Access Point table - definition of a single layer or access point within the comms stack
		static const SRecordTypeInfo iIAPPrioritySelectionPolicyRecordInfo[];// IAP Priority Selection Policy table - indication of priority selection of next-level Access Point
		static const SRecordTypeInfo iEAPSecRecordInfo[];					// EAP Sec table - settings for EAP authenticiation
		static const SRecordTypeInfo iTunEAPRecordInfo[];					// Tun EAP table - settings for Tunnelled EAP authentication method
		static const SRecordTypeInfo iEAPTLSRecordInfo[];					// EAP TLS table - settings for EAP TLS authentication method
		static const SRecordTypeInfo iLEAPRecordInfo[];						// LEAP table - settings for LEAP authentication method
		static const SRecordTypeInfo iEAPSIMRecordInfo[];					// EAP-SIM table - settings for EAP-SIM authentication method
		static const SRecordTypeInfo iEAPAKARecordInfo[];					// EAP-AKA table - settings for EAP-AKA authentication method
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		static const SRecordTypeInfo iAPPrioritySelectionPolicyRecordInfo[];// IAP Priority Selection Policy table - indication of priority selection of next-level Access Point
		static const SRecordTypeInfo iTierRecordInfo[];
	    static const SRecordTypeInfo iMCprRecordInfo[];
	    static const SRecordTypeInfo iCprRecordInfo[];
	    static const SRecordTypeInfo iSCprRecordInfo[];
	    static const SRecordTypeInfo iProtocolRecordInfo[];
	    static const SRecordTypeInfo iBearerTypeRecordInfo[];
	    static const SRecordTypeInfo iConfigAccessPointRecordInfo[];

	    static const SRecordTypeInfo iPolicySelectorRecordInfo[];
	    static const SRecordTypeInfo iPolicySelector2ParamsRecordInfo[];

		static const SRecordTypeInfo iGenericQosRecordInfo[];
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

		static const SRecordTypeInfo iWifiScanEngineRecordInfo[];

		static const STableLookup iTableLookup[];
		
		/*
		Retrieve type info for a table given a table id
		@internalComponent
		*/
		static const SRecordTypeInfo* GetRecordTypeInfo(TMDBElementId aId);

        /*
        Retrieve full type info for a field given a <table><column> id
        @internalcomponent
        */
		IMPORT_C static TInt GetFieldTypeInfoL(TMDBElementId aId);
		
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		/* This function is to get full table info based on the table ID. The main
		 * purpose to have this function is CEDDUMP. It has got a functionality,
		 * which resolves the link values to Link.<Tablename>.<TagId> format. In order
		 * to get the tableID we need this function.
		 * It's marked as internal component, because CEDDUMP should use it only.
		 * @internalcomponent
		 */
		IMPORT_C static const STableLookup* GetTableInfoL(TMDBElementId aId);
#endif
		/* Get back the table ID based on the table name */
        static TMDBElementId LookupTableId(TDesC& tableName);
        
        /*
        This function is used when some linked record is given as tagId. This can be done
        only with some tables and this function can tell whether the forced link resolving
        can be applied to the given table or not...
        @internalcomponent
        */
        static TBool IsEnabledForcedLinkedTypeResolving(TMDBElementId aId);
        
        /*
        If the forced link resolving is enabled this function resolves the linked record
        given as tagId and returns the linked type.
        @internalcomponent
        */
        static TMDBElementId ForcedFindLinkedIdL(TMDBElementId aTableId, TMDBElementId aLinkingFieldId);

	private :

		CommsDatSchemaV1_1(){} // at the moment, no need to instantiate this class - just used as namespace

	};

NONSHARABLE_CLASS(CommsDatSchema)
/*
CommsDatSchema encapsulates

  Type Validation Functions
		Lookup for Table Type info by id
		 to check container type matches data type
*/
	{
	public:

		/*******************************************************
		Functions to check the meaning of an elementId
		*/
		static TBool IsNode(TMDBElementId aElementId);

		static TBool IsTable(TMDBElementId aElementId);

		static TBool IsColumn(TMDBElementId aElementId);

		static TBool IsRecord(TMDBElementId aElementId);

		static TBool IsGenericRecord(TMDBElementId aElementId);

		static TBool IsTemplate(TMDBElementId aElementId);

		static TBool HasValidTableId(TMDBElementId aElementId);

		static TBool HasValidColumnId(TMDBElementId aElementId);

		static TBool HasValidRecordId(TMDBElementId aElementId);

		static TBool IsNewRecordRequest(TMDBElementId aElementId);

		static TBool IsNewColumnRequest(TMDBElementId aElementId);

		static TBool IsNewTableRequest(TMDBElementId aElementId); // New Table Request is not currently supported

		static TBool IsDeprecatedRecord(
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		                                TMDBElementId aElementId
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		                               );
		/*****************************************************************************************
		* Functions to check validity of container being used to store or modify data
		*/

		/*
		Validate the container type of aElement is appropriate to carry the data specified by its element id
		*/
		static void ValidateFieldTypeL(CMDBSession& aSession, CMDBElement& aElement);

        /*
        Get link information about legacy fields that link with base classes (NB new links should be used properly)
        */
		static TMDBElementId GetLegacyLinkTableIdL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, CMDBElement* aLinkingRecord);

        /*
        Fully qualify link fields to give table and record id
        */
        static TMDBElementId GetLinkIdL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, TMDBElementId aLinkingFieldValue, TMDBElementId aLinkedTableId, CMDBElement* aLinkingRecord = NULL);
        
        /*
        Get the linked element ID from the database
        */
        static TMDBElementId IdTagResolverL(CMDBSession& aSession, TMDBElementId aLinkedTypeId, TMDBElementId aTargetValue);
        
        static TMDBElementId ConstructLinkFromLegacyValueL(CMDBSession& aSession, CMDBElement* aLegacyField, TInt aLegacyValue);
        
        /* This funciton here is for the case when somebody tries to read a
                  * legacy link field, which is now contains the fully resolved TMDBElementId
                  * of the linked record, to contain only the record id...
                  * 
                  * IMPORTANT: the function expects ONLY fields as parameter!!!!!!!!!
        */
        static void ChangeLegacyLinkFieldValues(CMDBElement*& aLegacyField);
        
        static TMDBElementId ForcedLegacyLinkFindL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, TMDBElementId aLinkingFieldValue, CMDBElement* aElement);
        
	private :
		static TMDBElementId ForcedIAPFindL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, TMDBElementId aLinkingFieldValue, CMDBElement* aElement);
		static TMDBElementId ForcedProxiesFindL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, TMDBElementId aLinkingFieldValue, CMDBElement* aElement);
		static TMDBElementId ForcedLanServiceFindL(CMDBSession& aSession, TMDBElementId aLinkingFieldId, TMDBElementId aLinkingFieldValue, CMDBElement* aElement);
		
		CommsDatSchema(){} // at the moment, no need to instantiate this class - just used as namespace

	}; //class CommsDatSchema


}// end namespace CommsDat

#endif // COMMSDATSCHEMA_H
