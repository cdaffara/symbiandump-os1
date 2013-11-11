// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <f32file.h>
#include <stringpool.h>
#include "dbdef.h"
#include <xml/documentparameters.h>
#include <xml/taginfo.h>
#include <xml/attribute.h>
#include "CXMLContentHandler.h"

#include "filedump.h"
extern CFileDump* gMsg;		// logging
using namespace Xml;
#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
extern TBool gDeprecatedFields;
#endif

//anonymous namespace
//These things are needed only here... Don't mess up the global namespace
namespace
    {
    // The names of the tables to modify
    const TText* const modTable[] =
    	{
    	_S("IAPTable"),
    	_S("IAPTable"),
    	_S("IAPTable"),
    	_S("IAPTable"),
    	_S("IAPTable"),

    	_S("ProxyTable"),

    	_S("WAPIPBearerTable"),
    	_S("WAPIPBearerTable"),

    	_S("WAPSMSBearerTable"),

    	_S("VpnServiceTable"),
    	_S("VpnServiceTable"),

    	_S("ConnectionPreferencesTable"),

    	_S("GlobalSettingsTable"),
    	_S("GlobalSettingsTable"),
    	_S("GlobalSettingsTable"),
    	_S("GlobalSettingsTable"),
    	_S("OutgoingGPRSTable"),

    	_S("WLANServiceExtensionTable"),

    	_S("EAPSecuritySettingsTable"),

    	_S("TunnelledEAPSettingsTable")

    	};

    // The names of the parameters to modify
    const TText* const modParam[] =
    	{
    	// IAPTable
       _S("IAPService"),
    	_S("IAPBearer"),
    	_S("IAPNetwork"),
    	_S("LocationRef"),
    	_S("ChargecardRef"),
    	
    	// ProxyRef
    	_S("ISPRef"),

    	// WAPIPBearerTable
    	_S("AccessPoint"),
    	_S("IAPRef"),

    	// WAPSMSBearerTable
    	_S("AccessPoint"),

    	// VpnServiceTable
    	_S("HomeIAP"),
    	_S("HomeNetwork"),

    	// ConnectionPreferencesTable
    	_S("IAPRef"),

    	// GlobalSettingsTable
    	_S("DefaultNetwork"),
    	_S("LocationForDataAndFax"),
    	_S("LocationForPhoneServicesAndSMS"),
    	_S("WAPAccess"),

    	// OutgoingGPRSTable
    	_S("UmtsR99QoSAndOnRef"),

    	//WLANServiceExtensionTable
    	_S("Wlan_Security_Data"),

    	//EAPSecuritySettingsTable
    	_S("EAP_Method_Data"),

    	//TunnelledEAPSettingsTable
    	_S("Tun_EAP_Method_Data")
    	};

    // The new names of the modified parameters
    const TText* const newModParam[] =
    	{
    	// IAPTable
    	_S("IAPService"),
    	_S("IAPBearer"),
    	_S("IAPNetwork"),
    	_S("Location"),
    	_S("Chargecard"),
    	
    	// ProxyRef
    	_S("ISP"),

    	// WAPIPBearerTable
    	_S("AccessPointId"),
    	_S("IAP"),

       // WAPSMSBearerTable
    	_S("AccessPointId"),

    	// VpnServiceTable
    	_S("HomeIAP"),
    	_S("HomeNetwork"),

    	// ConnectionPreferencesTable
    	_S("IAP"),

    	// GlobalSettingsTable
    	_S("DefaultNetwork"),
    	_S("LocationForDataAndFax"),
    	_S("LocationForPhoneServicesAndSMS"),
    	_S("WAPAccessPoint"),

    	// OutgoingGPRSTable
    	_S("UmtsR99QoSAndOn"),

    	//WLANServiceExtensionTable
    	_S("Wlan_Security_Data"),

    	//EAPSecuritySettingsTable
    	_S("EAP_Method_Data"),

    	//TunnelledEAPSettingsTable
    	_S("Tun_EAP_Method_Data")
    	};

    /**
      The names of the new parameters to add which indicate the
      table where the table entry reference is
    */
    const TText* const typeParam[] =
    	{
       _S("IAPServiceType"),
    	_S("IAPBearerType"),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S("ProxyServiceType"),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S(""),
    	_S("")
    	};

    const TInt numMods = 20;

    const TText* const noNameTables[] =
    	{
       _S("GlobalSettingsTable"),
    	_S("ProxyTable"),
    	_S("AgentLookupTable"),
    	_S("WAPIPBearerTable"),
    	_S("WAPSMSBearerTable"),
    	_S("SecureSocketTable"),
    	_S("BTDefaultTable"),
    	_S("BTDeviceTable"),
    	_S("BTPersistTable"),
    	_S("BTSecurityTable"),
    	_S("ConnectionPreferencesTable")
    	};

    const TInt numNoNameTables = 11;

    /**
     * These two arrays (tableNamesForPairing, tmdbElementsForPairing) are needed because an
     * RHashMap is built up to handle these pairs. It is needed to serch for the table ID
     * based on the table name read form the xml config file.
     */

        const TText* const tableNamesForPairing[] =
            {
            _S("NetworkTable"),			        //KCDTIdNetworkRecord
            _S("IAPTable"),			            //KCDTIdIAPRecord
            _S("ModemBearerTable"), 	        //KCDTIdModemBearerRecord
            _S("LANBearerTable"), 		        //KCDTIdLANBearerRecord
            _S("LANServiceTable"), 		        //KCDTIdLANServiceRecord
            _S("DialInISPTable"), 		        //KCDTIdDialInISPRecord
            _S("DialOutISPTable"), 		        //KCDTIdDialOutISPRecord
            _S("AgentLookupTable"), 		    //KCDTIdAgentLookupRecord
            _S("ChargecardTable"), 		        //KCDTIdChargecardRecord
            _S("ConnectionPreferencesTable"), 	//KCDTIdConnectionPrefsRecord
            _S("GlobalSettingsTable"), 		    //KCDTIdGlobalSettingsRecord
            _S("IncomingGPRSTable"), 		    //KCDTIdIncomingGprsRecord
            _S("OutgoingGPRSTable"), 		    //KCDTIdOutgoingGprsRecord
            //_S("DefaultGPRSTable"), 		    //<NOKIA magic...>
            _S("ProxyTable"), 			        //KCDTIdProxiesRecord
            _S("LocationTable"), 		        //KCDTIdLocationRecord
            _S("SecureSocketTable"), 		    //KCDTIdSSProtoRecord
            //_S("BTDeviceTable"), 		        //<not used anymore>
            //_S("BTPersistTable"), 		    //<not used anymore>
            //_S("BTSecurityTable"), 		    //<not used anymore>
            //_S("BTDefaultTable"), 		    //<not used anymore>
            _S("WAPAccessPointTable"), 		    //KCDTIdWAPAccessPointRecord
            _S("WAPIPBearerTable"), 		    //KCDTIdWAPIPBearerRecord
            _S("WAPSMSBearerTable"), 		    //KCDTIdWAPSMSBearerRecord
            _S("VirtualBearerTable"), 		    //KCDTIdVirtualBearerRecord
            _S("VpnServiceTable"), 		        //KCDTIdVPNServiceRecord
            _S("WLANServiceExtensionTable"), 	//KCDTIdWLANServiceExtRecord
            _S("PANServiceExtensionsTable"), 	//KCDTIdPANServiceExtRecord
            _S("EAPSecuritySettingsTable"), 	//KCDTIdEAPSecRecord
            _S("TunnelledEAPSettingsTable"), 	//KCDTIdTunEAPRecord
			_S("EAPTLSSettingsTable"), 		    //KCDTIdEAPTLSRecord
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
			_S("LEAPSettingsTable") 		    //KCDTIdLEAPRecord
#else
			_S("LEAPSettingsTable"), 		    //KCDTIdLEAPRecord
			_S("PolicySelectorTable"),			//KCDTIdPolicySelectorRecord
			_S("PolicySelector2QosParametersTable"),	//KCDTIdPolicySelector2ParamsRecord
			_S("GenericQosTable")				//KCDTIdGenericQosRecord
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
            };


        const TMDBElementId tmdbElementsForPairing[] =
            {
            KCDTIdNetworkRecord,
            KCDTIdIAPRecord,
            KCDTIdModemBearerRecord,
            KCDTIdLANBearerRecord,
            KCDTIdLANServiceRecord,
            KCDTIdDialInISPRecord,
            KCDTIdDialOutISPRecord,
            KCDTIdAgentLookupRecord,
            KCDTIdChargecardRecord,
            KCDTIdConnectionPrefsRecord,
            KCDTIdGlobalSettingsRecord,
            KCDTIdIncomingGprsRecord,
            KCDTIdOutgoingGprsRecord,
            //NOKIA magic...>,
            KCDTIdProxiesRecord,
            KCDTIdLocationRecord,
            KCDTIdSSProtoRecord,
            //not used anymore>,
            //not used anymore>,
            //not used anymore>,
            //not used anymore>,
            KCDTIdWAPAccessPointRecord,
            KCDTIdWAPIPBearerRecord,
            KCDTIdWAPSMSBearerRecord,
            KCDTIdVirtualBearerRecord,
            KCDTIdVPNServiceRecord,
            KCDTIdWLANServiceExtRecord,
            KCDTIdPANServiceExtRecord,
            KCDTIdEAPSecRecord,
            KCDTIdTunEAPRecord,
            KCDTIdEAPTLSRecord,
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
            KCDTIdLEAPRecord
#else
            KCDTIdLEAPRecord,
			KCDTIdPolicySelectorRecord,
			KCDTIdPolicySelector2ParamsRecord,
			KCDTIdGenericQosRecord
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
            };

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
        const TInt KPairNumber = 27; //number of pairs in the above arrays
#else
		const TInt KPairNumber = 30; //number of pairs in the above arrays
#endif

        TUint32 DoHash(const TPtrC& aText)
            {
            return DefaultHash::Des16(aText);
            }

        TBool AreTheKeysIdentical(const TPtrC& aKey1, const TPtrC& aKey2)
            {
            return (aKey1 == aKey2);
            }
    } //end of anonymous namespace

//
// CXMLContentHandler
//
const TInt CXMLContentHandler::KExpectedLeaveCode = 1234;

CXMLContentHandler* CXMLContentHandler::NewL( CXMLDatabase* aXmlDb,
		                                      const TBool aForceFlag,
		                                      const TBool aAppendFlag )
    {
    CXMLContentHandler* self = new(ELeave) CXMLContentHandler(aXmlDb,aForceFlag,aAppendFlag);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CXMLContentHandler::CXMLContentHandler(CXMLDatabase* aXmlDb,
		                               const TBool aForceFlag,
		                               const TBool aAppendFlag) :
    iLeaveOnStartElement(EFalse),
    iNumElements(0),
    iNumSkippedEntities(0),
    iNumPrefixMappings(0),
    iNumPrefixUnmappings(0),
    iError(KErrNone),
    iXmlDb(aXmlDb),
    iForceXMLProcessing(aForceFlag),
    isInAppendMode(aAppendFlag),
    iXMLContentState(ENone)
	{
	iCurrentTableName.Copy(_L(""));
	iCurrentRecordName.Copy(_L(""));
	iCurrentParameterValue.Copy(_L(""));
	iCurrentParameter.Copy(_L(""));
	}

void CXMLContentHandler::ConstructL()
    {
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    iCmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
    iCmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif

    THashFunction32<TPtrC> hashFunc(&DoHash);
    TIdentityRelation<TPtrC> idRel(&AreTheKeysIdentical);

    iTableNameIDpairs = new(ELeave) RHashMap<TPtrC, TMDBElementId>(hashFunc, idRel);
    FillUpHashMapL(*iTableNameIDpairs);
    }

CXMLContentHandler::~CXMLContentHandler()
	{
	delete iCmdbSession;
	iCmdbSession = NULL;

	iTableNameIDpairs->Close();
    delete iTableNameIDpairs;
    iTableNameIDpairs = NULL;
	}

// When the beginning of the document is encountered then all counters
// are initialised
void CXMLContentHandler::OnStartDocumentL(const RDocumentParameters&, TInt)
	{
	iNumElements         = 0;
	iNumSkippedEntities  = 0;
	iNumPrefixMappings   = 0;
	iNumPrefixUnmappings = 0;
	}

void CXMLContentHandler::OnEndDocumentL(TInt)
	{
	}

// At the beginning of an XML element we need to find out what does the element
// represent
void CXMLContentHandler::OnStartElementL(const RTagInfo& aInfo, const RAttributeArray& attribs, TInt)
	{
	if (iLeaveOnStartElement)
		{
		if (iNumElements++ == 0)
			{
			User::Leave(KExpectedLeaveCode);
			}
		}

	iNumElements++;

	// Get the name of the tag
	//
	const TDesC8& name = aInfo.LocalName().DesC();
	TBuf<MAX_BUFFER_LEN> tag;
	tag.Copy(name);

	TBuf<MAX_BUFFER_LEN> thisTag;

    switch (iXMLContentState)
        {
        case ENone:
            {

        	// We avoid the "Config" and "InformationTable" tags as well as
        	// the parameters of the "InformationTable" which are not supported
        	// in this version
        	if (tag.Compare(_L("Config")) == 0)
        		{
        		gMsg->Msg(_L("Avoiding Config element"));
        		return;
        		}

        	if (tag.Compare(_L("InformationTable")) == 0)
        		{
        		iXMLContentState = EInDeprecatedTable;
        		iCurrentTableName.Copy(tag);
        		gMsg->Msg(_L("Start of the InformationTable element"));
        		return;
        		}

        	// See if this tagname is a table

        	TInt                 i = 0;
        	TBuf<MAX_BUFFER_LEN> tableName;
        	thisTag = xmlTableArray[0];


        	while (thisTag.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
        		{
        		tableName = thisTag;
        		tableName.Append(_L("Table"));

        		// Check if this is a table
        		//
        		if (tableName.Compare(tag) == 0)
        			{
                	// If the parsed element is a table then save the name of this
                	// table for later use when creating table entries
                	//
                    iXMLContentState = EInTable;
            		iCurrentTableName.Copy(tableName);
            		gMsg->Msg(_L("Located table : [%S]"), &tableName);
            		return;
        			}
        		i++;
        		thisTag = xmlTableArray[i];
        		}

            // If we've got here then there must have been some weirdness in the input file, still
            // perhaps if we carry on, we'll sort ourselves out...

    		gMsg->Msg(_L("ERR: Unknown or deprecated table : [%S]"), &tag);
    		iXMLContentState = EInDeprecatedTable;
    		iCurrentTableName.Copy(tag);
            }
        	break;

        case EInDeprecatedTable:
        	return;
        case EInTable:
            {
        	// Examine if this tagname is a record

        	TInt i       = 0;
        	     thisTag = xmlTableArray[0];
        	while (thisTag.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
        		{
        		// Check whether this is a table entry
        		//
        		if (thisTag.Compare(tag) == 0)
        			{
                	// If the parsed element is a table entry then create a new CXMLTableEntry
                	// object and add it to the XML database

        			iXMLContentState = EInRecord;
        			iCurrentRecordName.Copy(thisTag);

            		CXMLTableEntry* tableEntry = CXMLTableEntry::NewL(iCurrentTableName);
            		TBuf<MAX_BUFFER_LEN> operation;
            		if(attribs.Count() == 0)
            		    {
            			operation.Copy(_L("add"));
            			}
            		else
            		    {
            			operation.Copy(attribs[0].Value().DesC());
            			}

            		tableEntry->SetOperation(operation);

            		gMsg->Msg(_L("Create new table entry for table : [%S] with operation (%S)"),
                            			&iCurrentTableName, &operation);
            		iXmlDb->AddTableEntryL(tableEntry);
            		return;
        			}

        		i++;
        		thisTag = xmlTableArray[i];
        		}


            // If we've got here then there must have been some weirdness in the input file, still
            // perhaps if we carry on, we'll sort ourselves out...

    		gMsg->Msg(_L("ERR: Unknown record : [%S]"), &tag);
            if (!iForceXMLProcessing)
                {
    			User::Leave(KErrCorrupt);
                }
            }
        	break;

        case EInRecord:
            {
        	// If the parsed element is not a table or a table entry then it
        	// represents the beginning of the specification of a table entry
        	// parameter. In this case we need to set the name of this element
        	// as a new parameter for the table entry we last created. Note that
        	// the value of the parameter will be picked by a call to the
        	// OnContentL function

        	iXMLContentState = EInParam;

        	CXMLTableEntry* lastEntry = iXmlDb->GetLastTableEntry();
        	iCurrentParameter.Copy(tag);
        	lastEntry->AddParameterL(tag, _L(""));
        	gMsg->Msg(_L("Create new parameter : [%S]"), &tag);

        	/**
        	Set the linking type to default - ENot_A_Link. Later on this will be
        	changed.
        	*/
        	lastEntry->SetLinkType(CXMLTableEntry::ENot_A_Link);
        	}
        	break;

        default:
            break;
        }
	} // OnStartElementL()


void CXMLContentHandler::OnEndElementL(const RTagInfo& aInfo, TInt)
	{
	// Get the name of the tag
    const TDesC8& name = aInfo.LocalName().DesC();
    TBuf<MAX_BUFFER_LEN> tag;
	tag.Copy(name);

	// Reset the name of the current table if we reached the end of the table

    switch (iXMLContentState)
        {
        case EInDeprecatedTable:
    		{
            if (tag.Compare(iCurrentTableName) == 0)
        		{
        		iCurrentTableName.Copy(_L(""));
        		gMsg->Msg(_L("Reached end of deprecated table [%S]"), &tag);
	    		iXMLContentState = ENone;
				}
    		else
        	    {
                gMsg->Msg(_L("ERR: Use of [%S] inside deprecated [%S]"), &tag, &iCurrentTableName);
#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
				gDeprecatedFields = ETrue;
#endif
        	    }

    		}
        	break;
        case EInTable:
            {
    		iXMLContentState = ENone;
            if (tag.Compare(iCurrentTableName) == 0)
        		{
        		iCurrentTableName.Copy(_L(""));
        		gMsg->Msg(_L("Reached end of table [%S]"), &tag);
        		}
        	else
        	    {
                gMsg->Msg(_L("Error parsing table [%S]"), &tag);
                if (!iForceXMLProcessing)
                    {
        			User::Leave(KErrCorrupt);
                    }
        	    }
        	}
        	break;

        case EInRecord:
            {
            iXMLContentState = EInTable;
            if (tag.Compare(iCurrentRecordName) == 0)
        		{
        		iCurrentRecordName.Copy(_L(""));
        		gMsg->Msg(_L("Reached end of record [%S]"), &tag);
        		}
        	else
        	    {
        		gMsg->Msg(_L("Error parsing record [%S]"), &tag);
                if (!iForceXMLProcessing)
                    {
        			User::Leave(KErrCorrupt);
                    }
                }
            }
        	break;

        case EInParam:
            {
            iXMLContentState = EInRecord;
        	if (tag.Compare(iCurrentParameter) == 0)
        		{
        		// Pick the last table entry added to the database
        		CXMLTableEntry* lastEntry = iXmlDb->GetLastTableEntry();
        		TInt lastParamIndex = lastEntry->GetNumberParameters()-1;

        		// Examine to see if the content is simply the return
        		// character which indicates that there is no assigned
        		// value for this parameter
        		TInt loc = iCurrentParameterValue.Locate('\n');

        		if (loc == 0)
        			{
        			lastEntry->RemoveParameter(lastParamIndex);
                    return;
        			}

        		// Set the value of the last parameter for this table entry
        		lastEntry->SetParameterValue(lastParamIndex, iCurrentParameterValue);

        		gMsg->Msg(_L("Set parameter value to : [%S]"), &iCurrentParameterValue);
        		iCurrentParameterValue.Copy(_L(""));
        		}
        	else
        	    {
        		gMsg->Msg(_L("Error parsing parameter [%S]"), &tag);
                if (!iForceXMLProcessing)
                    {
        			User::Leave(KErrCorrupt);
                    }
        	    }
        	}
            break;

        default:
            break;
        }
	} // OnEndElementL()


void CXMLContentHandler::OnContentL(const TDesC8& aContent, TInt)
	{
    // If we are currently processing table entry parameters then the
	// content represents a fragment of the value of the recently
	// created parameter

	if (iXMLContentState == EInParam)
		{
		TBuf<MAX_BUFFER_LEN> temp;
		temp.Copy(aContent);
		iCurrentParameterValue.Append(temp);
		}
	}

void CXMLContentHandler::OnProcessingInstructionL(const TDesC8&, const TDesC8&, TInt)
	{
	}

void CXMLContentHandler::OnError(TInt aError)
	{
	gMsg->Msg(_L("ERR: Error parsing xml, err=%d"), aError);
	iError = aError;
	}

TAny* CXMLContentHandler::GetExtendedInterface(const TInt32)
	{
	return NULL;
	}

void CXMLContentHandler::OnStartPrefixMappingL(const RString&, const RString&, TInt)
	{
	iNumPrefixMappings++;
	}

void CXMLContentHandler::OnEndPrefixMappingL(const RString&, TInt)
	{
	iNumPrefixUnmappings++;
	}

void CXMLContentHandler::OnIgnorableWhiteSpaceL(const TDesC8&, TInt)
	{
	}

void CXMLContentHandler::OnSkippedEntityL(const RString&, TInt)
	{
	iNumSkippedEntities++;
	}

void CXMLContentHandler::OnExtensionL(const RString&, TInt, TInt)
	{
	}

// Set the IDs for all table entries
void CXMLContentHandler::SetTableIDs()
	{
	TInt iEntry;
	TInt entryId = 1;

    for (iEntry = 1; iEntry < iXmlDb->GetNumberTableEntries(); iEntry++)
		{
		CXMLTableEntry* entry = iXmlDb->GetTableEntry(iEntry);
        const TBuf<MAX_BUFFER_LEN>& tableName    = entry->GetTableName();
        const TBuf<MAX_BUFFER_LEN>& tableNamePre = iXmlDb->GetTableEntry(iEntry-1)->GetTableName();

		if(tableName.Compare(tableNamePre) == 0)
		    {
			entryId++;
		    }
		else
		    {
			entryId = 1;
		    }

		entry->SetEntryId(entryId);
		}
	}

// Some table entries in CommDB do not have a "Name" parameter although
// in the XML format all table entries must have such a parameter in order
// to allow table entry referencing. The following loop removes the
// "Name" parameter from these table entries
void CXMLContentHandler::RemoveNameParams()
	{
	for (TInt iEntry = 0; iEntry < iXmlDb->GetNumberTableEntries(); iEntry++)
		{
		CXMLTableEntry* entry = iXmlDb->GetTableEntry(iEntry);
        const TBuf<MAX_BUFFER_LEN>& tableName = entry->GetTableName();

		for(TInt i = 0; i < numNoNameTables; i++)
			{
			if(tableName.Compare(TPtrC(noNameTables[i])) == 0)
				{
				TInt numParams = entry->GetNumberParameters();

				// Loop through all the parameters of the current table entry
				for(TInt iParam = 0; iParam < numParams; iParam++)
					{
					const TBuf<MAX_BUFFER_LEN>& paramName = entry->GetParameterName(iParam);

					// Check if this parameter needs modification.
					if(paramName.Compare(_L("Name")) == 0)
						{
						entry->RemoveParameter(iParam);
						break;
						}
					}
				}
			}
		}
	}

 /**
 * This function sets the <TableName>.<RecordID> linking in the
 * case of a linked record in the XML file. So from this point the
 * processing of the linked record can continue like in the case of
 * regular cfg files.
 */
void CXMLContentHandler::SetCfgLinkingFromXMLFile(const TDesC&          aRefTable,
                                                  const TInt            aParamIndex,
                                                        CXMLTableEntry* aEntry)
    {
    TBool finished = EFalse;

    for (TInt i = 0; (!finished) && (i < LinkedRecordTableNum); ++i)
        {
        TPtrC linkedRecordTableName(LinkedRecordTableNames[i]);

        if ( linkedRecordTableName == aEntry->GetTableName() )
            {
            /**
              OK, got the table name index. Based on this let's check
              the name of the linked record...
              1 loop for the paramName array in the entry, the other is for the LinkRecordsArray[i]
            */

            for (TInt j = 0; (!finished) && ((TPtrC(LinkRecordsArray[i][j])) != TPtrC(NO_MORE_RECORDS)); ++j)
                {
                TPtrC linkedRecordFromArray(LinkRecordsArray[i][j]);

                if ( linkedRecordFromArray == aEntry->GetParameterName(aParamIndex) )
                /**
                 * YESSSSS, we have found a linked record. Currently it contains only the
                 * recordID of the referenced record. This should be changed to
                 * Link.tableName.recordID which is in our case the
                 * Link.<aRefTable>.<current recordID>
                 */
                    {
                    TBuf<MAX_BUFFER_LEN> buf;
                    /**
                     * It would be simpler if the Append(TDesc16) would return with a
                     * 'this' reference...
                     */
                    buf.Append(aRefTable);
                    buf.Append(TPtrC(_S(".")));
                    buf.Append(aEntry->GetParameterValue(aParamIndex));
                    aEntry->SetParameterValue(aParamIndex, buf);

                    finished = ETrue;
                    }
                }
            }
        }
    }

/**
 * This funciton is called if the record referencing from the XML db is not
 * successful. In this case we try to relove the references from the existing
 * database.
 * DEF103749
 */
TBool CXMLContentHandler::ResolveRefsFromDbL(CXMLTableEntry*             entry,
                                			const TInt                  paramIndex,
                                			const TPtrC&                refEntryName,
                                			const TPtrC&                refEntryTable,
                                			const TPtrC&                orgEntryTable)
    {
    TBool retCode = EFalse;

    TMDBElementId elemendID;

	elemendID = iTableNameIDpairs->FindL(refEntryTable);

    //read all of the records form a given table (entry->GetTableName)
	CMDBRecordSetBase* recordFromDb = new(ELeave) CMDBRecordSetBase(elemendID);
	CleanupStack::PushL(recordFromDb);

	recordFromDb->LoadL(*iCmdbSession);

    for (TInt i = 0; (!retCode) && (i<recordFromDb->iRecords.Count()); ++i)
        {
        if ( CXMLTableEntry::EResolve_XML_Ref == entry->GetLinkType(paramIndex) )
        //The linking is tableName.RecordName
            {
            CMDBField<TDesC> *afn = NULL;
            TInt temp = 0;
            CMDBElement* element = recordFromDb->iRecords[i]->GetFieldByNameL(_L("Name"), temp );

            afn = static_cast<CMDBField<TDesC> *>(element);

            TPtrC name(*afn);
            if (name == refEntryName)
            //OK, found the refenced record in the db.
                {
                TInt recId = recordFromDb->iRecords[i]->RecordId();

            	TBuf<MAX_BUFFER_LEN> buf;
    			buf.Format(_L("%D"), recId);
    			entry->SetParameterValue(paramIndex,buf);

    			SetCfgLinkingFromXMLFile(orgEntryTable, paramIndex, entry);

    			retCode = ETrue;
                }
            }
        else
        //The linking is tableName.recordID
            {
            if ( recordFromDb->iRecords[i]->RecordId() == entry->GetRecordID() )
            //OK, found the referenced record in the DB.
                {
                if ( entry->GetParameterName(paramIndex) == TPtrC(_S("IAPService")) ||
	                     entry->GetParameterName(paramIndex) == TPtrC(_S("IAPBearer")) )
			            {
			            TBuf<MAX_BUFFER_LEN> buf;
    			        buf.Format(_L("%D"), recordFromDb->iRecords[i]->RecordId());

    				    entry->SetParameterValue(paramIndex,buf);
			            }

    				retCode = ETrue;
                }
            }
        }

        CleanupStack::PopAndDestroy(recordFromDb);

    return retCode;
    }


/**
This function fills up the HashMap, which is used to resolve refences
from the existing databse.
*/
void CXMLContentHandler::FillUpHashMapL(RHashMap<TPtrC, TMDBElementId>& aHashMap)
    {
    for (TInt i = 0; i < KPairNumber; ++i)
        {
        //Temporary can be used as the parameters (1st param) because RHashMap copies the elements
        aHashMap.InsertL(TPtrC(tableNamesForPairing[i]), tmdbElementsForPairing[i]);
        }
    }

// Resolve the table entry references specified in the XML configuration file
TBool CXMLContentHandler::ResolveTableEntryReferences(CXMLTableEntry* entry,
                                                      const TInt paramIndex,
                                                      const TBuf<MAX_BUFFER_LEN>& refEntryName,
                                                      const TBuf<MAX_BUFFER_LEN>& refEntryTable,
                                                      const TBuf<MAX_BUFFER_LEN>& orgEntryTable)
	{
    // If the name of the referenced table entry is "-1"
    // i.e. it does not exist, then the ID is set to "0"
    if(refEntryName.Compare(_L("-1")) == 0)
		{
        if(entry->GetOperation().Compare(_L("template")) == 0)
			{
            entry->SetParameterValue(paramIndex,_L("0"));
            return ETrue;
			}
        else
			{
			gMsg->Msg(_L("Cannot resolve table entry reference : [%S.%S]"), &refEntryTable, &refEntryName);
			gMsg->Msg(_L("CED terminates without generating a CommDB database"));
			return EFalse;
			}
		}

	if ( CXMLTableEntry::ESkip_XML__Ref == entry->GetLinkType(paramIndex) )
	/**
	We have a link with tablename.-1 referencing. Skip it...
	*/
	    {
	    return ETrue;
	    }

	// Find the ID of the table entry referenced
    for (TInt k = 0; k < iXmlDb->GetNumberTableEntries(); k++)
		{
        CXMLTableEntry* lookEntry = iXmlDb->GetTableEntry(k);

		// Look for table entries which belong to the table
        // we are looking for
        if(lookEntry->GetTableName().Compare(refEntryTable) == 0)
			{
			if (CXMLTableEntry::EResolve_XML_Ref == entry->GetLinkType(paramIndex))
			/**
			The linking is by record name...
			*/
			    {
    			for(TInt m = 0; m < lookEntry->GetNumberParameters(); m++)
    				{
    				const TBuf<MAX_BUFFER_LEN>& param = lookEntry->GetParameterName(m);
    				if(param.Compare(_L("Name")) == 0)
    					{
    					const TBuf<MAX_BUFFER_LEN>& name = lookEntry->GetParameterValue(m);
    					if(name.Compare(refEntryName) == 0)
    						{
    				        TBuf<MAX_BUFFER_LEN> buf;
    					    buf.Format(_L("%D"), lookEntry->GetRecordID());

    				        entry->SetParameterValue(paramIndex,buf);

        					SetCfgLinkingFromXMLFile(orgEntryTable, paramIndex, entry);

    						return ETrue;
    						}

    						break;
    					}
    				}
			    }
			else
			/**
			The linking is by recordID...
			*/
			    {
				TLex16 conv(refEntryName);
	            TInt recId  = 0;
	            TInt err = conv.Val(recId);

			    if (err == KErrNone && recId == lookEntry->GetRecordID())
			    //We have found the referenced record in the XMLDB
			        {
			        if ( entry->GetParameterName(paramIndex) == TPtrC(_S("IAPService")) ||
	                     entry->GetParameterName(paramIndex) == TPtrC(_S("IAPBearer")) )
			            {
			            TBuf<MAX_BUFFER_LEN> buf;
    			        buf.Format(_L("%D"), lookEntry->GetRecordID());

    				    entry->SetParameterValue(paramIndex,buf);
			            }

    				return ETrue;
			        }
			    }
			}
		}

	// If we reach this point then the ID cannot be resolved. In this case
	// we have the following options:
	//
	// 1. If the table entry currently processing is a template then this
	// is expected.
	//
	// 2. If the table entry currently processing is not a template then
	// the program terminates if CED is running with the "-F" flag. Else
	// we set the ID equal to the refered table entry name converted to an
	// integer

	if (iForceXMLProcessing)
	    {
	    if (entry->GetOperation().Compare(_L("template")) == 0)
    		{
    		TLex16 conv(refEntryName);
    		TInt id  = 0;
    		TInt err = conv.Val(id);

    		if(err == KErrNone)
    			{
    			TBuf<MAX_BUFFER_LEN> buf;
    			buf.Format(_L("%D"),++id);
    			entry->SetParameterValue(paramIndex,buf);
    			}

    		return ETrue;
    		}
        else
            {
            TBool ret = EFalse;
    	    TRAPD(leavingError, ret = ResolveRefsFromDbL(entry, paramIndex, refEntryName, refEntryTable, orgEntryTable));

            if (!leavingError)
                {
                if (!ret)
                    {
                    gMsg->Msg(_L("Cannot resolve table entry reference from the DB: [%S.%S]"), &refEntryTable, &refEntryName);
			        gMsg->Msg(_L("CED terminates without generating a CommDB database"));
                    }
                return ret;
                }
            else
                {
                gMsg->Msg(_L("Cannot resolve table entry reference from the DB: [%S.%S]"), &refEntryTable, &refEntryName);
			    gMsg->Msg(_L("CED terminates without generating a CommDB database"));

			    return EFalse;
                }

            }
	    }
	else
		{
		gMsg->Msg(_L("Cannot resolve table entry reference : [%S.%S]"), &refEntryTable, &refEntryName);
		gMsg->Msg(_L("CED terminates without generating a CommDB database"));
		return EFalse;
		}
	}

/**
 * Map the recordIDs from the database into the data structure in which the
 * the xml elements are stored. It is needed for the proper linking.
 * DEF103749
 */
void CXMLContentHandler::SetRecordIDsL()
    {
    //RAII
    TMDBElementId elemendID      = 0;

	TInt entryNum                = 0,
	     tempEntryNum            = 0,
	     baseRecordID            = 0;

	TBool entryEnd               = EFalse;

	CXMLTableEntry* entry        = NULL,
	              * tempEntry    = NULL;

	CMDBRecordBase* recordFromDb = NULL;

	while (entryNum < iXmlDb->GetNumberTableEntries())
		{
		entry = iXmlDb->GetTableEntry(entryNum);

	    if ( TPtrC(_S("template")) == entry->GetOperation() )
	     /**
	     * Handling templates is a bit trickier than simple addings...
	     *
	     * Note: it's not handled here when 1 table contains more tham 1 template records in the
	     * config file...
	     * Should CED be prepared for this????????????????????????
	     */
	        {
	        if (isInAppendMode)
	            {
    	        elemendID = iTableNameIDpairs->FindL(entry->GetTableName());

            	// Enable ECDHidden on session to be able to read the template records
            	iCmdbSession->SetAttributeMask(ECDHidden);


            	recordFromDb = new(ELeave) CMDBRecordBase(elemendID);
            	CleanupStack::PushL(recordFromDb);

                recordFromDb->SetRecordId(KCDDefaultRecord);

            	/**
            	 * This leave has to be trapped becasuse CED should be prepared for the
            	 * cases where a template record is already existing in the given
            	 * table.
            	 */
            	TRAPD(leavingError, recordFromDb->LoadL(*iCmdbSession));

            	if (!leavingError)
            	/**
            	 * The LoadL didn't leave, which means that we have already a template
            	 * record for the given table in the database. The record read from the xml
            	 * file cannot be appended.
            	 */
            	    {
            	    gMsg->Msg(_L("Warning - Duplicate TEMPLATE records for the table: [%S]"),&entry->GetTableName());
            	    gMsg->Msg(_L("Skipping it..."));

            	    entry->SetDeleteFlag(ETrue);
            	    }
            	else
            	/**
            	 * The given table doesn't have yet a template record so the one which was read from
            	 * the xml file can be appended.
            	 */
            	    {
            	    entry->SetRecordID(0);
            	    }

            	CleanupStack::PopAndDestroy(recordFromDb);
        	    recordFromDb = NULL;
	            }
        	else
            /**
             * Simply give the record number 0 for the template record as we are not in append mode.
             */
        	    {
        	    entry->SetRecordID(0);
        	    }


        	// Disable ECDNoWriteButDelete on session
        	iCmdbSession->ClearAttributeMask(ECDHidden);

        	++entryNum;
	        }
	    else
	    /**
	     * Simple addings...
	     */
	        {
    	    if (isInAppendMode)
    	        {
        	    TRAPD(leavingError, elemendID = iTableNameIDpairs->FindL(entry->GetTableName()));

            	if (!leavingError)
	            	{
	            	CMDBRecordSetBase* recordSetFromDb = new(ELeave) CMDBRecordSetBase(elemendID);
    	        	CleanupStack::PushL(recordSetFromDb);

        	    	recordSetFromDb->LoadL(*iCmdbSession);

            		TInt recIdMax = 0;

            		for (TInt i = 0; i<recordSetFromDb->iRecords.Count(); ++i)
                    	{
                    	recordFromDb = recordSetFromDb->iRecords[i];

                    	if (recordFromDb->RecordId() > recIdMax)
                        	recIdMax = recordFromDb->RecordId();
                    	}

        	    	baseRecordID = recIdMax+1;

        	    	CleanupStack::PopAndDestroy(recordSetFromDb);
	            	}
	            else
	            	{
	           	    gMsg->Msg(_L("Warning - cannot append records to the table: [%S]"),&entry->GetTableName());
            	    gMsg->Msg(_L("Skipping it..."));

            	    entry->SetDeleteFlag(ETrue);
	            	}
    	        }
    	    else
    	        {
    	        baseRecordID = 1;
    	        }

    	    entryEnd = EFalse;

    		for (tempEntryNum = entryNum;
    		     (!entryEnd) && (tempEntryNum < iXmlDb->GetNumberTableEntries());
    		     ++tempEntryNum)
    		    {
    		    tempEntry = iXmlDb->GetTableEntry(tempEntryNum);

    		    if (entry->GetTableName() != tempEntry->GetTableName())
    		        {
    		        entryEnd = ETrue;
    		        break;
    		        }
    		    else
    		        {
    		        tempEntry->SetRecordID(baseRecordID++);
    		        }

    		    entryNum = entryNum+(tempEntryNum-entryNum)+1;

    		    }
	        }
        }
    }


// CommDB requires the conversion of the references to table entries using the qualified
// name scheme of the XML configuration file to one which employs a table entry ID and
// an additional parameter which indicates the name of the table the entry belongs to
TBool CXMLContentHandler::ModifyTableEntryReferencesL()
	{
	// Set the IDs for all table entries
    SetTableIDs();

    TRAPD(leavingError, SetRecordIDsL());

    if (KErrNone != leavingError)
        return EFalse;

    //if some entrys have the deleted flag true they are deleted here...
    iXmlDb->CheckDeletedElems();

	// Modify the table references

    for (TInt iEntry = 0; iEntry < iXmlDb->GetNumberTableEntries(); iEntry++)
		{
		CXMLTableEntry* entry = iXmlDb->GetTableEntry(iEntry);

		// Loop through all the table parameters to modify
		for(TInt iMod = 0; iMod < numMods; iMod++)
			{
			const TBuf<MAX_BUFFER_LEN>& modName = modParam[iMod];

			// Look for table entries which belong to the table
			// we need to modify

			if(entry->GetTableName().Compare(TPtrC(modTable[iMod])) == 0)
				{
				// Loop through all the parameters of the current table entry
				for(TInt iParam = 0; iParam < entry->GetNumberParameters(); iParam++)
					{
					const TBuf<MAX_BUFFER_LEN>& paramName = entry->GetParameterName(iParam);

					// Check if this parameter needs modification.
					if(paramName.Compare(modName) == 0)
						{
						// Extract the value of the name of the table the table entry
						// reference belongs to as well as the name of the table entry
						const TBuf<MAX_BUFFER_LEN>& paramValue = entry->GetParameterValue(iParam);

						gMsg->Msg(_L("Modifying Parameter : [%S] = [%S]"),&paramName,&paramValue);

						TBuf<MAX_BUFFER_LEN> refEntryTable;
						TInt k, m;

						for(k = 0; k < paramValue.Length(); k++)
							{
							TChar c = paramValue[k];
							if(c == TChar('.')) break;
							refEntryTable.Append(c);
							}

                        TBuf<MAX_BUFFER_LEN> orgEntryTable;
                        orgEntryTable.Copy(refEntryTable);
                        refEntryTable.Append(_L("Table"));

						gMsg->Msg(_L("  Referenced Table Name = [%S]"),&refEntryTable);

						if (!IsTableKnown(orgEntryTable))
							{
							//The referenced table is not supported anymore. Skip it...
							gMsg->Msg(_L("ERR: Referenced Table Name = [%S] is not known by CED. Skipping it..."),&refEntryTable);
							}
						else
							{
	                        TBuf<MAX_BUFFER_LEN> refEntryName;
							for(m = k+1; m < paramValue.Length(); m++)
								refEntryName.Append(paramValue[m]);

							gMsg->Msg(_L("  Referenced Table Entry Name = [%S]"),&refEntryName);

							// Rename the modified parameter
							entry->RenameParameter(iParam,newModParam[iMod]);

							/**
							Setting the linking type - if any
							*/
							TLex16 conv(refEntryName);
	    		            TInt recId  = 0;
	    		            TInt err = conv.Val(recId);

	                		if(err == KErrNone)
	                		/**
	                		  OK, the conversion is successful. Let's see whether is it a non relevant link
	                		  (tableName.-1) or a relevant one...
	                		*/
	                			{
	                			if (-1 == recId)
	                			/**
	                			The link can be skipped...
	                			*/
	                			    {
	                			    entry->ModifyLinkType(iParam, CXMLTableEntry::ESkip_XML__Ref);
	                			    }
	                            else
	                            /**
	                            The reference is by record id...
	                            */
	                                {
	                                entry->ModifyLinkType(iParam, CXMLTableEntry::ERecord_ID_Reference);
	                                }
	                			}
	                		else
	                		/**
	                		The conversion wasn't successful, so the given reference is
	                		not record ID.
	                		*/
	                		    {
	                		    entry->ModifyLinkType(iParam, CXMLTableEntry::EResolve_XML_Ref);
	                		    }

							// If the name of the table must be specified then add
							// an additional parameter for that purpose
							if(TPtrC(typeParam[iMod]).Compare(_L("")) != 0)
								{
								if(orgEntryTable.Compare(_L("Proxy")) == 0)
									orgEntryTable.Copy(_L("Proxies"));
								else if(orgEntryTable.Compare(_L("BTDevice")) == 0)
									orgEntryTable.Append(_L("Table"));
								else if(orgEntryTable.Compare(_L("BTDefault")) == 0)
									orgEntryTable.Append(_L("Table"));
								else if(orgEntryTable.Compare(_L("BTSecurity")) == 0)
									orgEntryTable.Append(_L("Table"));
								else if(orgEntryTable.Compare(_L("BTPersist")) == 0)
									orgEntryTable.Append(_L("Table"));
								else if(orgEntryTable.Compare(_L("SecureSocket")) == 0)
									orgEntryTable.Append(_L("Table"));
								else if(orgEntryTable.Compare(_L("UmtsR99QoSAndOn")) == 0)
									orgEntryTable.Append(_L("Table"));

								entry->AddParameterL(typeParam[iMod],orgEntryTable);
								}

	                        // Resolve the table entry references
	                        TBool bResolved = ResolveTableEntryReferences(entry,iParam,refEntryName,refEntryTable, orgEntryTable);

	                        if(!bResolved)
	                            {
	                            return EFalse;
	                            }
							}
						}
					}
				}
			}
		}

	// Some table entries in CommDB do not have a "Name" parameter although
	// in the XML format all table entries must have such a parameter in order
	// to allow table entry referencing. The following loop removes the
	// "Name" parameter from these table entries
    RemoveNameParams();

    return ETrue;
	}

// Dump the contents of the table entries on the log file
void CXMLContentHandler::LogTableEntries()
	{
	gMsg->Msg(_L("Log Dump of CommDB Table Contents"));

	for (TInt iEntry = 0; iEntry < iXmlDb->GetNumberTableEntries(); iEntry++)
		{
		CXMLTableEntry* entry = iXmlDb->GetTableEntry(iEntry);

		gMsg->Msg(_L("Table : [%S]"),&entry->GetTableName());
		gMsg->Msg(_L("Table Entry ID = [%D]"),entry->GetEntryId());

		for(TInt iParam = 0; iParam < entry->GetNumberParameters(); iParam++)
			{
			gMsg->Msg(_L(" [%S] = [%S]"),&entry->GetParameterName(iParam),&entry->GetParameterValue(iParam));
			}
		}
	}

bool CXMLContentHandler::IsTableKnown(const TDesC&    aRefTable)
	{
	TInt i = 0;
	TPtrC actTableName = xmlTableArray[i];

	while(actTableName.Compare(TPtrC(NO_MORE_RECORDS)) != 0 &&
		  actTableName.Compare(aRefTable) != 0)
		{
		actTableName.Set(TPtrC(xmlTableArray[++i]));
		}

	if (aRefTable.Compare(actTableName) == 0)
		{
		return ETrue;
		}

	return EFalse;
	}

//----------------- CXMLTableEntry ----------------


// Constructor for the CXMLTableEntry class
CXMLTableEntry::CXMLTableEntry()
: entryId(1)
, iRecordID(0)
, iDeleteFlag(EFalse)
	{
	__DECLARE_NAME(_S("CXMLTableEntry"));
	}

// Destructor for the CXMLTableEntry class
CXMLTableEntry::~CXMLTableEntry()
	{
	paramName.ResetAndDestroy();
	paramValue.ResetAndDestroy();

	iLinkType.Close();
	}

CXMLTableEntry* CXMLTableEntry::NewL(const TDesC& aTableName)
	{
	CXMLTableEntry* p = new(ELeave) CXMLTableEntry;
	CleanupStack::PushL(p);
	p->ConstructL(aTableName);
	CleanupStack::Pop(p);
	return p;
	}

void CXMLTableEntry::ConstructL(const TDesC& aTableName)
	{
	tableName.Copy(aTableName);
	}

// Add a new pair of paramater name and value in this table entry
void CXMLTableEntry::AddParameterL(const TBuf<MAX_BUFFER_LEN>& aParamName,
								  const TBuf<MAX_BUFFER_LEN>& aParamValue)
	{
	TBuf<MAX_BUFFER_LEN>* name = new(ELeave) TBuf<MAX_BUFFER_LEN>;
	name->Copy(aParamName);
	paramName.AppendL(name);

	TBuf<MAX_BUFFER_LEN>* value = new(ELeave) TBuf<MAX_BUFFER_LEN>;
	value->Copy(aParamValue);
	paramValue.AppendL(value);
	}

// Remove a parameter from the list
void CXMLTableEntry::RemoveParameter(const TInt aIndex)
	{
	delete paramName[aIndex];
	paramName.Remove(aIndex);
	delete paramValue[aIndex];
	paramValue.Remove(aIndex);
	}

// Set the value of a parameter given its index
void CXMLTableEntry::SetParameterValue(const TInt aIndex,
									   const TBuf<MAX_BUFFER_LEN>& aParamValue)
	{
	paramValue[aIndex]->Copy(aParamValue);
	}

// Rename a table entry parameter
void CXMLTableEntry::RenameParameter(const TInt aIndex, const TBuf<MAX_BUFFER_LEN>& aNewName)
	{
	paramName[aIndex]->Copy(aNewName);
	}




//------------------ CXMLDatabase --------------------



// Constructor for the CXMLDatabase class
CXMLDatabase::CXMLDatabase()
	{
	__DECLARE_NAME(_S("CXMLDatabase"));
	}

// Destructor for the CXMLDatabase class
CXMLDatabase::~CXMLDatabase()
	{
	tableEntries.ResetAndDestroy();
	}

CXMLDatabase* CXMLDatabase::NewL()
	{
	CXMLDatabase* p = new(ELeave) CXMLDatabase;
	return p;
	}

// Add a table entry to the database given the pointer
void CXMLDatabase::AddTableEntryL(CXMLTableEntry* aEntry)
	{
	if(aEntry != NULL)
		{
		tableEntries.AppendL(aEntry);
		}
	}

// Get a table entry from the database given its index which represents
// its location in the database
CXMLTableEntry* CXMLDatabase::GetTableEntry(const TInt aIndex)
	{
	if(aIndex >= 0 && aIndex < tableEntries.Count())
		{
		return tableEntries[aIndex];
		}
	else
		{
		return NULL;
		}
	}

// Get the total number of table entries currently in the database
TInt CXMLDatabase::GetNumberTableEntries() const
	{
	return tableEntries.Count();
	}

// Get the last table entry added to the database
CXMLTableEntry* CXMLDatabase::GetLastTableEntry()
	{
	TInt index = tableEntries.Count();
	if(index == 0)
		{
		return NULL;
		}
	else
		{
		return tableEntries[index-1];
		}
	}

void CXMLDatabase::CheckDeletedElems()
    {
    TInt i = 0;

    while (i < tableEntries.Count())
        {
        if (tableEntries[i]->GetDeleteFlag())
            {
            delete tableEntries[i];
            tableEntries.Remove(i);
            }
        else
            {
            ++i;
            }
        }
    }



