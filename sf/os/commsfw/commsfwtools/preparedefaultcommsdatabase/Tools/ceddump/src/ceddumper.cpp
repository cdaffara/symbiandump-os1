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
// Contains the global definitions needed across the ceddump module/exe
//
//

/**
 @file ceddumper.cpp
 @internalComponent
*/

#include <e32cons.h>
#include <e32std.h>
#include <bacline.h>
#include "ceddumper.h"
#include "ceddumpglobals.h"
#include "dbdef.h"
#include <etelpckt.h>
#include <etelqos.h>
#include <faxdefn.h>
#include <networking/panroles.h>
#include <nifvar_internal.h>
#include <commsdat_internal.h>
#include <commsdattypesv1_1_partner.h>
#include <commsdattypesv1_1_internal.h>

#include <comms-infras/commsdatschema.h>

using namespace CommsDat;

//_LIT(Comma, 			", "); commented out because not used
_LIT(APPLICATIONNAME,"CommsDat Database Dump Utility");
_LIT(APPLICATIONVERSION,"1.1");

/* This holds the column value */
TBuf<MAX_COL_LONG_VAL_LEN> globalValueBuffer;

#define Hidden 			_S("Hidden=Yes")
#define Private 		_S("Private=Yes")
#define ProtectedWrite 	_S("ProtectedWrite=Yes")
#define ProtectedRead 	_S("ProtectedRead=Yes")

#define KLinkPrefix		_L("Link.")
#define KLinkSeparator     _L(".")
#define TableNameTag     _L("Table")

const TUid KCommsDatUid = {0xcccccc00};

const TText* const gAtttibutesArray[]=
	{
	Hidden,
	Private,
	ProtectedWrite,
	ProtectedRead,
	NO_MORE_RECORDS
	};

CCedDumper::~CCedDumper()
	{
	iGenericTableIds.Close();
	iGenericTableNames.Close();
	for(TInt i=0; i<iGenericTableFields.Count();i++)
		{
		iGenericTableFields[i].Close();
		}
	iGenericTableFields.Close();
	delete iRepository;

    delete iFileDumper;
	delete iDbSession;
	delete iConsole;
	delete iTable;

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
	delete iR99ConversionRecordsHead;
	// no need to delete iR99ConversionRecordsCurrent since it is always linked to the head.
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS
	}

/*static*/ CCedDumper* CCedDumper::NewL()
	{
	CCedDumper* dumper = new (ELeave) CCedDumper;
	CleanupStack::PushL(dumper);
	dumper->ConstructL();
	CleanupStack::Pop(dumper);
	return dumper;
	}

void CCedDumper::ConstructL()
	{
	iConsole = Console::NewL(_L("Ceddump Tool"),TSize(KConsFullScreen,KConsFullScreen));
	iRepository = CRepository::NewL(KCommsDatUid);

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
	iR99Conversion = EFalse;
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS

    iUseHexFormat = EFalse;
    iTemplateRecordBase = NULL;

#ifdef __TOOLS2__
    // ROHID support is enabled via command line parameter for TOOLS2
    iROHIDSupport = EFalse;
#else
    // For other platforms it depends on the __COMMDB_ROHID_SUPPORT__ macro
	#ifdef __COMMDB_ROHID_SUPPORT__
    	iROHIDSupport = ETrue;
	#else
    	iROHIDSupport = EFalse;
	#endif
#endif
	}

TInt CCedDumper::ProcessCommandLineL()
	{
	TInt ret = KErrNone;

	/* Check for command line parameters*/
	if(ParseCommandLine())
		{
		/* connect to file system */
		iFileDumper = CFileDump::NewL(iCfgFileName, TPtrC(APPLICATIONNAME), TPtrC(APPLICATIONVERSION), ETrue, iConsole);
		if (!iFileDumper->IsInitialised())
			{
			iConsole->Printf(_L("Failed to open the output file [%S]\n"), &iCfgFileName );
#ifndef __TOOLS2__
			iConsole->Printf(_L("\nPress any key to finish"));
			iConsole->Getch();
#endif
			ret = KErrArgument;
			}
		else
			{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
			iDbSession = CMDBSession::NewL(KCDVersion1_2);
#else
			iDbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
			if (iROHIDSupport)
				{
				iDbSession->SetAttributeMask(ECDHidden | ECDPrivate | ECDProtectedWrite |ECDNoWriteButDelete );
				}
			else
				{
				iDbSession->SetAttributeMask(ECDHidden);
				}
			}
		}
	else
		{
		HelpDump();
		ret = KErrArgument;
		}
	return ret;
	}

/**
Prints basic help information to the emulator window including command switches
*/
void CCedDumper::HelpDump()
	{
	iConsole->Printf(_L("Creates Comms Database Configuration File"));

	TBuf<MAX_ARG_LEN> buffer;

	buffer = (_L("\n\nCEDDUMP "));
#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
	buffer += (_L("[-CR99] "));
#endif

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	buffer += (_L("[-x] "));
#endif

#ifdef __TOOLS2__
	buffer += (_L("[-rohid] "));
#endif

	buffer += (_L("[-o [path]filename]"));
	iConsole->Printf(_L("%S"), &buffer);

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
	iConsole->Printf(_L("\n  -CR99 Performs the conversion of R97/98 QoS GPRS parameters to R99"));
	iConsole->Printf(_L("\n        Assumes R97/98 parameters are stored in commsdat database"));
#endif // SYMBIAN_NETWORKING_3GPPDEFAULTQOS

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	iConsole->Printf(_L("\n  -x UIDs are output in hex format"));
#endif

#ifdef __TOOLS2__
	iConsole->Printf(_L("\n  -rohid The readonly and hidden attributes are output to the configuration file"));
#endif
	iConsole->Printf(_L("\n  -o Specifies an output configuration file for CEDDUMP. Defaults to '%s'"), DEFAULTCFGFILENAME);

#ifndef __TOOLS2__
	iConsole->Printf(_L("\n\nPress any key to finish"));
	iConsole->Getch();
#endif
	}

/* This method validates CommandLine parameters */
TBool CCedDumper::ParseCommandLine()
	{

	TBool ret = EFalse;
	iCfgFileName = DEFAULTCFGFILENAME;
	CCommandLineArguments *pCmd = NULL;
	TRAP_IGNORE (pCmd = CCommandLineArguments::NewL());

#ifndef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
	TBool bOutFound = EFalse;
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS


	if(pCmd)
		{
		TBuf<MAX_ARG_LEN> arg;
		TInt i = 0;

		/* check all arguments for switches */

		while( ++i < pCmd->Count() )
			{
			arg = pCmd->Arg(i);
			arg.UpperCase();

			if ( arg.FindF(_L("-H")) != KErrNotFound )
				{
				delete pCmd;
				return ret;
				}

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
			// set up a flag to make an conversion of QoS GPRS values from R97/98 to R99/R4
			if (arg.FindF(_L("-CR99"))!= KErrNotFound )
				{
				iR99Conversion = ETrue;
				}
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS
			// Output UIDs in hex format?
			//
			if (arg.FindF(_L("-X")) != KErrNotFound)
				{
				iUseHexFormat = ETrue;
				}

			/* Specification of an output file*/
			if ( arg.FindF(_L("-O")) != KErrNotFound )
				{
				if( i != pCmd->Count()-1 )
					{
					iCfgFileName = pCmd->Arg(++i);
#ifndef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
					bOutFound = ETrue;
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS
					continue;
					}
				else
					{
					iConsole->Printf(_L("Argument missing after '-o' switch"));
					iConsole->Printf(_L("\n\nPress any key to finish"));
					iConsole->Getch();
					delete pCmd;
					return ret;
					}
				}

#ifndef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
			if ( !bOutFound  )
				{
				iCfgFileName = pCmd->Arg(i);
				bOutFound = ETrue;
				continue;
				}
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS
#ifdef __TOOLS2__
			// Output Read Only Hidden columns
			//
			if (arg.FindF(_L("-ROHID")) != KErrNotFound)
				{
				iROHIDSupport = ETrue;
				}
#endif
			}
		if (iCfgFileName.Length() > 0)
			ret = ETrue;

		}
	delete pCmd;
	return ret;
	}

void CCedDumper::PrintError(TInt error)
	{
	iConsole->Printf(_L("Utility failed with error=%d\n"), error);
	iConsole->Getch();
	}

void CCedDumper::DumpContents()
	{
	ListGenericTablesL();
	DumpGenericTablesL();

	TInt loop = 0;
	TBuf<MAX_COL_NAME_LEN> tempTable;

	/* tableArray holds all the table names */
	tempTable = tableArray[loop];

	iConsole->Printf(_L("Processing table: [%S]"), &tempTable);
	/* This loops through all the tables */
	while(tempTable.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
		{
		// Get the record type
		TRAPD(ret, GetRecordTypeL(elementIdArray[loop])); /* Create a record set for table */
		if(ret!=KErrNone)
			{
			iConsole->Printf(_L("(unsupported)"));
			}
		else
			{
			iTemplateRecordBase = NULL;
			// Retrieve the table
			TRAP(ret,(iTable->LoadL(*iDbSession)));/* Loads all the records of particular record type */

			if(ret!=KErrNone)
				{
				iConsole->Printf(_L("\n ERROR: Unable to process table %S, error %d. Continuing...\n"), &tempTable, ret);
				}
			else
				{
				iFileDumper->WriteTableHeader(tempTable);/* Write Table header in the output file */
				//boo WriteTableAttributes(tempTable);/* logs table attributes : _TABLE [ Priveat = Yes, ....] */
				TRAP(ret, WriteTemplateAndRecordsL());/*Logs template and other records within a Record type */
				if(ret!=KErrNone)
					{
					iConsole->Printf(_L("\n ERROR: Unable to WriteTemplateAndRecords for table %S, error %d. Continuing...\n"), &tempTable, ret);
					}
				}
			}
		/* Go to next Record type ( Table) */
		loop++;
		tempTable = tableArray[loop];
        delete iTable;
        iTable = NULL;
		ResetAllAttrubuteFlags();/* Reset attribute flags to EFalse */
		if(tempTable.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
			{
			iConsole->Printf(_L(" [%S]"), &tempTable);
			}
		}

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
	if (iR99Conversion)
		{
		WriteR99Conversion();
		}
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS
	iConsole->Printf(_L(" All done.\n"));
	}


void CCedDumper::WriteTableAttributes(TDesC &/*aTempTable*/)
	{
	TPtrC column;
	TPtrC value;

	column.Set(_L("Hidden"));
	if(iRecordBase->IsSetAttribute(ECDHidden))
		{
		value.Set(_L("TRUE"));
		iFileDumper->WriteColumnValue(column, value);
		}
	else
		{
		value.Set(_L("FALSE"));
		iFileDumper->WriteColumnValue(column, value);
		}

	column.Set(_L("ReadOnly"));
	if(iRecordBase->IsSetAttribute(ECDNoWriteButDelete))
		{
		value.Set(_L("TRUE"));
		iFileDumper->WriteColumnValue(column, value);
		}
	else
		{
		value.Set(_L("FALSE"));
		iFileDumper->WriteColumnValue(column, value);
		}

/*	TBuf<MAX_COL_LONG_VAL_LEN> buf;
	TPtrC column;
	buf= (_S("_TABLE["));

	//boo iFileDumper->WriteTableHeader(aTempTable);// Write Table header in the output file

	//boo iFileDumper->WriteAttributeSectionHeader(); // Write Attribute section header : ADD_ATTRIBUTES

	// Get status of all the attributes for table

	if(iTable->IsSetAttribute(ECDHidden))
		{
		iIsHiddenSet = ETrue;
		buf.Append((TPtrC)Hidden);
		buf.Append(Comma);
		}

	if(iTable->IsSetAttribute(ECDPrivate))
		{
		iIsPrivateSet = ETrue;
		buf.Append((TPtrC)Private);
		buf.Append(Comma);
		}

	if(iTable->IsSetAttribute(ECDNoWriteButDelete))
		{
		iIsProtectedReadSet = ETrue;
		buf.Append((TPtrC)ProtectedRead);
		buf.Append(Comma);
		}

	if(iTable->IsSetAttribute(ECDProtectedWrite))
		{
		iIsProtectedWriteSet = ETrue;
		buf.Append((TPtrC)ProtectedWrite);
		buf.Append(Comma);
		}


	buf.Append(_L("]"));
	column.Set(buf);

	iFileDumper->WriteAttributes(column);// Finally logs the table attributes

	iFileDumper->WriteSectionFooter(1);// Log the section footer : ADD_END
    */
	}



void CCedDumper::WriteTemplateAndRecordsL()
	{
	TInt numSections;
	TBuf<MAX_COL_LONG_VAL_LEN> buf;
	TPtrC column;
	TInt i = 0;
	TBuf<MAX_COL_NAME_LEN> tempColumn;
	tempColumn = gAtttibutesArray[i];

	numSections=iTable->iRecords.Count(); /* No Of Sections within a single Record */

	/* Loops through all the recorsd within a particular record type */
	for(TInt iRecord = 0; iRecord < numSections; iRecord++ )
		{
#ifndef __TOOLS2__
		TInt boo = iTable->iRecords[iRecord]->RecordId();
#endif
		iTable->iRecords[iRecord]->Attributes();


		if (!iROHIDSupport)
			{
			if( (iTable->iRecords[iRecord]->RecordId() != 0) &&
			    ((iTable->iRecords[iRecord]->Attributes() & ECDHidden) ==  ECDHidden) )
				{
				continue;
				}
			}


		iFileDumper->WriteSectionHeader(iTable->iRecords[iRecord]->RecordId());

	/*	if (iTable->iRecords[iRecord]->RecordId() == 0) // The record with the zeroth id is template Record
			{
			buf= (_S("_TEMPLATE["));
			}
		else // for all other records
			{
			buf= (_S("_RECORD["));
			}
		*/
		iRecordBase = iTable->iRecords[iRecord];
		/*
		// Get status of all the attributes of a record/template
		if(iRecordBase->IsSetAttribute(ECDHidden) && !(iIsHiddenSet))
			{
			iIsHiddenSet = ETrue;
			buf.Append((TPtrC)Hidden);
			buf.Append(Comma);
			}

		if(iRecordBase->IsSetAttribute(ECDPrivate) && !(iIsPrivateSet))
			{
			iIsHiddenSet = ETrue;
			buf.Append((TPtrC)Private);
			buf.Append(Comma);
			}

		if(iRecordBase->IsSetAttribute(ECDNoWriteButDelete) && !(iIsProtectedReadSet))
			{
			iIsProtectedReadSet = ETrue;
			buf.Append((TPtrC)ProtectedRead);
			buf.Append(Comma);
			}

		if(iRecordBase->IsSetAttribute(ECDProtectedWrite) && !(iIsProtectedWriteSet))
			{
			iIsProtectedWriteSet = ETrue;
			buf.Append((TPtrC)ProtectedWrite);
			buf.Append(Comma);
			}

		buf.Append(_L("]"));
		column.Set(buf);

		iFileDumper->WriteAttributes(column);	*/

		if (0 == iTable->iRecords[iRecord]->RecordId())
			{
			/* There is a template record in the table. Let's have a pointer to it and if some other
			 * fields in another record in the table contains the very same information ceddump won't
			 * print it - as it's already in the template record.
			 */
			iTemplateRecordBase = iTable->iRecords[iRecord];
			}

		GetColumnAttributeAndValuesL(iRecord);/* Get all the column names with their attributes and values */

		iFileDumper->WriteSectionFooter(iTable->iRecords[iRecord]->RecordId());

		}/* End of for Loop -- Looping through all records */

	}

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

TPtrC FieldAnalyzer(const TPtrC& aFieldName)
    {
    const TUint KFieldNameSeperator = '#';
	TInt pos = aFieldName.Locate(TChar(KFieldNameSeperator));

	if (pos != KErrNotFound)
	    {
	    return (aFieldName.Left(pos));
	    }

	return aFieldName;
    }

#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY


/*
 * This function is for resolving the inconsistent table names in CommsDat and CommDB - unfortunatly CED and CEDDUMP is using
 * the CommDB variant. :(((((
 * Please note: As long as the naming for the new follows the rule, which is to have the same name in the CommsDat and CommDB components,
 * this function doesn't need to be modified.
 */
void CCedDumper::ResolveCommsDatTableNameVariant_to_CommdbTableNameVariant(RBuf& aCommsdatTableName)
    {
    if (TPtrC(KCDTypeNameWLANServiceExt) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(WLAN_SERVICE_EXTENSIONS).Length());
        aCommsdatTableName = WLAN_SERVICE_EXTENSIONS; /*name from commdb*/
        }
    else if (TPtrC(KCDTypeNameAccessPoint) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(ACCESS_POINT_TABLE).Length());
        aCommsdatTableName = ACCESS_POINT_TABLE; /*name from commdb*/
        }
    else if (TPtrC(KCDTypeNameIapPrioritySelectionPolicy) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(IAP_PRIORITY_SELECTION_POLICY_TABLE).Length());
        aCommsdatTableName = IAP_PRIORITY_SELECTION_POLICY_TABLE; /*name from commdb*/
        }
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    else if (TPtrC(KCDTypeNameApPrioritySelectionPolicy) == aCommsdatTableName)/*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(AP_PRIORITY_SELECTION_POLICY_TABLE).Length());
        aCommsdatTableName = AP_PRIORITY_SELECTION_POLICY_TABLE; /*name from commdb*/
        }
    else if (TPtrC(KCDTypeNameTierRecord) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(TIER_TABLE).Length());
        aCommsdatTableName = TIER_TABLE; /*name from commdb*/
        }
    else if (TPtrC(KCDTypeNameMCprRecord) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(MCPR_TABLE).Length());
        aCommsdatTableName = MCPR_TABLE; /*name from commdb*/
        }
    else if (TPtrC(KCDTypeNameCprRecord) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(CPR_TABLE).Length());
        aCommsdatTableName = CPR_TABLE; /*name from commdb*/
        }
    else if (TPtrC(KCDTypeNameSCprRecord) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(SCPR_TABLE).Length());
        aCommsdatTableName = SCPR_TABLE; /*name from commdb*/
        }
    else if (TPtrC(KCDTypeNameProtocolRecord) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(PROTOCOL_TABLE).Length());
        aCommsdatTableName = PROTOCOL_TABLE; /*name from commdb*/
        }
    else if (TPtrC(KCDTypeNameConfigAccessPointRecord) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(CONFIG_ACCESS_POINT_TABLE).Length());
        aCommsdatTableName = CONFIG_ACCESS_POINT_TABLE; /*name from commdb*/
        }
#endif
    else if (TPtrC(KCDTypeNameEapSimProtocol) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(EAPSIM_PROTOCOL_TABLE).Length());
        aCommsdatTableName = EAPSIM_PROTOCOL_TABLE; /*name from commdb*/
        }
    else if (TPtrC(KCDTypeNameEapAkaProtocol) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(EAPAKA_PROTOCOL_TABLE).Length());
        aCommsdatTableName = EAPAKA_PROTOCOL_TABLE; /*name from commdb*/
        }
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
    else if (TPtrC(KCDTTypeNamePolicySelector2Params) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(POLICYSELECTOR2QOSPARAMETERS_TABLE).Length());
        aCommsdatTableName = POLICYSELECTOR2QOSPARAMETERS_TABLE; /*name from commdb*/
        }
    else if (TPtrC(KCDTTypeNameGenericQoSParams) == aCommsdatTableName) /*name from commsdat*/
        {
        aCommsdatTableName.ReAllocL(TPtrC(GENERIC_QOS_TABLE).Length());
        aCommsdatTableName = GENERIC_QOS_TABLE; /*name from commdb*/
        }
#endif
    }



/*
To get the column values and their attributes.
Looping through the record for all the columns
*/
void CCedDumper::GetColumnAttributeAndValuesL(TInt aRecordId)
	{
	TBuf<MAX_COL_LONG_VAL_LEN> buf; /* This holds Column name and its attributes */
	TPtrC setting; /* Pointer for column value */
	TPtrC column;  /* Pointer for column name and attributes */

	const SRecordTypeInfo* recordInfo = 0;
	recordInfo = iRecordBase->GetRecordInfo();
	TInt fieldCount=0; // Counts number of fields so we can print FIELD_COUNT later

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
	CR99QoSConversion::TConvRecordType ret;
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS

	/* Loop through all the columns of the record */
	while(recordInfo->iTypeId != 0)
		{
		/* Get Column */
		TInt tempType(0);

		CMDBElement* iDBField=iRecordBase->GetFieldByNameL(recordInfo->iTypeName,tempType);

		CMDBElement* templateField = NULL;

		if (iTemplateRecordBase)
			{
			templateField = iTemplateRecordBase->GetFieldByNameL(recordInfo->iTypeName,tempType);
			}

		/* Get name of the field into buffer */
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		buf = FieldAnalyzer(recordInfo->iTypeName);
#else
        buf = recordInfo->iTypeName;
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

		if(!(iDBField->IsNull()))
			{
			if ((aRecordId > 0) && templateField && !(templateField->IsNull()))
				{
				if (IsFieldValuesTheSame(iDBField, templateField,
										(recordInfo->iValType & KCDShowBasicTypeInfo)))
					{
					recordInfo++;
					continue;
					}
				}

			/*
			Get the column value
			*/
			switch(recordInfo->iValType & KCDShowBasicTypeInfo)
	            {
				case EText:
	            case ELongText :
	            case EMedText :
					globalValueBuffer = *(static_cast<CMDBField<TDesC>*>(iDBField));
					break;

				case EDesC8:
					globalValueBuffer.Copy((*(static_cast<CMDBField<TDesC8>*>(iDBField))));
					break;

				case EBool:
					if(*(static_cast<CMDBField<TBool>*>(iDBField)))
						{
						globalValueBuffer = TRUE_VAL;
						}
					else
						{
						globalValueBuffer = FALSE_VAL;
						}
					break;

				case EInt:
                    {
                    // UIDs have all been declared as 'TInts' for some reason...

					TInt pos = buf.Find(KUidMarker);
                    const TInt  KUidMarkerLength = 3;
        			TBool isUid = (pos > 0 && pos == (buf.Length() - KUidMarkerLength));

        			if (iUseHexFormat && isUid)
        			    {
        			    globalValueBuffer = KHexMarker;
    					globalValueBuffer.AppendNumFixedWidthUC(*(static_cast<CMDBField<TUint32>*>(iDBField)), EHex, 8);
        			    }
				    else
				        {
    					globalValueBuffer.Num(*(static_cast<CMDBField<TInt>*>(iDBField)));
    					}
					break;
					}
                case ELink:
                    {
                    TUint32 val = *(static_cast<CMDBField<TUint32>*>(iDBField));

                    if (val > KCDMaxRecords)
                        {
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
                        /*here try to load the TAG id of the linked record. If it's not successful the
                         * tableID.recordid format should be used.
                         */

                        //remove the '7F' from the 'column' part of the TMDBElementId
                        TInt cleanId = ((val & KCDMaskShowRecordType) | (val & KCDMaskShowRecordId));

                        CMDBField<TUint>* tagIdOfLinkedRecord = new(ELeave) CMDBField<TUint>(cleanId | KCDTIdRecordTag);
                        CleanupStack::PushL(tagIdOfLinkedRecord);

                        /* The TRAPD is needed here, as it's acceptable _NOT_ to have tag IDs in the records
                         * although it's highly recommended to have one.
                         */

                        TRAPD(err, tagIdOfLinkedRecord->LoadL(*iDbSession););
                        if (KErrNone == err)
                            {
                            //the LoadL was sucessful, so we have the tagID. Let's get the name of the linked table.
                            globalValueBuffer = KLinkPrefix;

                            const CommsDat::STableLookup* tableInfo = CommsDat::CommsDatSchemaV1_1::GetTableInfoL(tagIdOfLinkedRecord->ElementId());

							RBuf tableName;
                            tableName.CleanupClosePushL();

                            tableName.Create(TPtrC(tableInfo->iTableName));

                            ResolveCommsDatTableNameVariant_to_CommdbTableNameVariant(tableName);

                            globalValueBuffer.Append(tableName);

                            CleanupStack::PopAndDestroy(); //close the RBuf - tableName

                            globalValueBuffer.Append(KLinkSeparator);

                            globalValueBuffer.AppendNum(*tagIdOfLinkedRecord, EDecimal);
                            }
                        else
                            {
                            /* The TagId of the linked record cannot be found, so let's convert
                             * the link value to the <tableName>.<recordId> format.
                             */
                            const CommsDat::STableLookup* tableInfo = CommsDat::CommsDatSchemaV1_1::GetTableInfoL(val);

                            RBuf tableName;
                            tableName.CleanupClosePushL();

                            tableName.Create(TPtrC(tableInfo->iTableName));

                            ResolveCommsDatTableNameVariant_to_CommdbTableNameVariant(tableName);

                            globalValueBuffer = tableName;

                            CleanupStack::PopAndDestroy(); //close the RBuf - tableName

                            globalValueBuffer.Append(KLinkSeparator);

                            TInt recId = (val & KCDMaskShowRecordId) >> 8;
                            globalValueBuffer.AppendNum(recId, EDecimal);
                            }

                        CleanupStack::PopAndDestroy(tagIdOfLinkedRecord);
#endif
                        }
					else
                        {
                        globalValueBuffer.Num(*(static_cast<CMDBField<TInt>*>(iDBField)));
                        }
                    break;
					}
				case EUint32:
					globalValueBuffer.Num(*(static_cast<CMDBField<TUint32>*>(iDBField)));
					break;
                default:
                    break;
				};

			setting.Set(globalValueBuffer);
			column.Set(buf);

			/*Convert Enums*/
			ConvertEnums(column, setting);

/*
			if ((recordInfo->iValType & KCDShowBasicTypeInfo) == ELink)
                {
                ConvertRecordLinks(column, setting, iDBField);
			    //There are 3 fields currently where a format of TableName.RecordId has to be followed
			    IsLinkedField(column, setting, *iDBField);
                }
*/
#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
			// The conversion engine will create the uni-direction list of converted tables.
			// The table is created in CheckAndPerformConvrsion()
			// All deprecated parameters from Ougoing and Incoming GPRS table are removed.
			// Additional parameter is added to Incoming and Outgoing GPRS tables to link them with particular R99 QoS Parameters Table

			// Verify if conversion of QoS Gprs parameters is needed
			if (iR99Conversion &&
			((iTable->TableId() == KCDTIdOutgoingGprsRecord) || (iTable->TableId() == KCDTIdIncomingGprsRecord)))
				{
				ret = CheckAndPerformConversion(column, setting);
				// Add the record to the output file only when it is not deprecated
				if (ret == CR99QoSConversion::EValidRecord)
					{
					// Add record to the output file
					if(iFileDumper->WriteColumnValue(column, setting))
						{
						fieldCount++;
						}
					}
				else if (ret == CR99QoSConversion::EConvertionError)
					{
					//We failed to create R99QoSConversiaionObject. Throw an event.
					User::Leave(KErrNoMemory);
					}
				}
			else if (column.Compare(TPtrC(KCDTypeNameIapCountName)) == 0)
				{
					// We dont want to include IAPCOUNT at the cfg output file
				}
			else
				{
				if(iFileDumper->WriteColumnValue(column, setting))
					{
					fieldCount++;
					}
				}
			}

#else

			/* Finally write the contents to output file */
			if(iFileDumper->WriteColumnValue(column, setting))
				{
				fieldCount++;
				}
		}
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS

		/* Switch to Next column */
		recordInfo++;
	}

	if (iROHIDSupport)
		{
		WriteTableAttributes(column);
		fieldCount = fieldCount +2;
		}

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
	if (iR99Conversion)
	{
		if (iR99ConversionRecordsCurrent && iR99ConversionRecordsCurrent->GetModificationFlag())
			{
			//Finish with this table
			//This enforces to create new instance of CR99QoSConversion for next entry within incoming/outgoing GPRS table if needed

			// convert column and setting values
			buf.Num(iR99ConversionRecordsCurrent->GetId());
			column.Set(TPtrC(QOS_UMTS_R99_AND_ON_TABLE));
			setting.Set(buf);

			//write UmtsR99QoSAndOnTable entry to the incoming/outgoing GPRS table.
			if(iFileDumper->WriteColumnValue(column, setting))
				{
				fieldCount++;
				}

			//all is done with this table - already linked to the list so it is safe to assign NULL to it
			iR99ConversionRecordsCurrent = NULL;
			}
	}
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS

	// Update the "FIELD_COUNT=n" metadata
	_LIT(KFieldCount, "FIELD_COUNT");
	column.Set(KFieldCount);
	_LIT(KFieldCountNum, "%d");
	buf.Format(KFieldCountNum, fieldCount);
	setting.Set(buf);
	iFileDumper->WriteColumnValue(column, setting);
	}

TBool CCedDumper::IsFieldValuesTheSame(CMDBElement* aFirstField, CMDBElement* aSecondField, TInt aFieldTypeValue)
	{
	switch(aFieldTypeValue)
        {
		case EText:
        case ELongText :
        case EMedText :
        	{
        	TPtrC tmpFirst(*(static_cast<CMDBField<TDesC>*>(aFirstField)));
        	TPtrC tmpSecond(*(static_cast<CMDBField<TDesC>*>(aSecondField)));

        	if (tmpFirst == tmpSecond)
        		{
        		return ETrue;
        		}
			break;
        	}

		case EDesC8:
			{
        	TPtrC8 tmpFirst(*(static_cast<CMDBField<TDesC8>*>(aFirstField)));
        	TPtrC8 tmpSecond(*(static_cast<CMDBField<TDesC8>*>(aSecondField)));

        	if (tmpFirst == tmpSecond)
        		{
        		return ETrue;
        		}
			break;
			}
		default:
			//convert the value to an int
			{
        	TInt tmpFirst(*(static_cast<CMDBField<TUint32>*>(aFirstField)));
        	TInt tmpSecond(*(static_cast<CMDBField<TUint32>*>(aSecondField)));

        	if (tmpFirst == tmpSecond)
        		{
        		return ETrue;
        		}
			break;
			}
        }

	return EFalse;
	}

/*
This method sets the pointer to recordset type depending on record Id
*/
void CCedDumper::GetRecordTypeL(TMDBElementId aRecordElementId)
	{
	switch(aRecordElementId)
		{
		case KCDTIdConnectionPrefsRecord:
			{
			CMDBRecordSet<CCDConnectionPrefsRecord>* iConnectionPrefsTable = new(ELeave) CMDBRecordSet<CCDConnectionPrefsRecord>(KCDTIdConnectionPrefsRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iConnectionPrefsTable);
			}
			break;

		case KCDTIdIAPRecord:
			{
			CMDBRecordSet<CCDIAPRecord>* iIAPTable = new (ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iIAPTable);
			}
			break;

		case KCDTIdGlobalSettingsRecord:
			{
			CMDBRecordSet<CCDGlobalSettingsRecord>* iGlobalSettingsTable = new(ELeave) CMDBRecordSet<CCDGlobalSettingsRecord>(KCDTIdGlobalSettingsRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iGlobalSettingsTable);
			}
			break;

		case KCDTIdNetworkRecord:
			{
			CMDBRecordSet<CCDNetworkRecord>* iNetworkTable = new(ELeave) CMDBRecordSet<CCDNetworkRecord>(KCDTIdNetworkRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iNetworkTable);
			}
			break;

		case KCDTIdLocationRecord:
			{
			CMDBRecordSet<CCDLocationRecord>* iLocationTable = new(ELeave) CMDBRecordSet<CCDLocationRecord>(KCDTIdLocationRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iLocationTable);
			}
			break;

		case KCDTIdWAPAccessPointRecord:
			{
			CMDBRecordSet<CCDWAPAccessPointRecord>* iWAPAccessPointTable = new(ELeave) CMDBRecordSet<CCDWAPAccessPointRecord>(KCDTIdWAPAccessPointRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iWAPAccessPointTable);
			}
			break;

		case KCDTIdDialOutISPRecord:
			{
			CMDBRecordSet<CCDDialOutISPRecord>* iDialOutISPTable = new(ELeave) CMDBRecordSet<CCDDialOutISPRecord>(KCDTIdDialOutISPRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iDialOutISPTable);
			}
			break;

		case KCDTIdDialInISPRecord:
			{
			CMDBRecordSet<CCDDialInISPRecord>* iDialInISPTable  = new(ELeave) CMDBRecordSet<CCDDialInISPRecord>(KCDTIdDialInISPRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iDialInISPTable);
			}
			break;

		case KCDTIdLANServiceRecord:
			{
			CMDBRecordSet<CCDLANServiceRecord>* iLANServiceTable = new(ELeave) CMDBRecordSet<CCDLANServiceRecord>(KCDTIdLANServiceRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iLANServiceTable);
			}
			break;

		case KCDTIdWLANServiceExtRecord:
			{
			CMDBRecordSet<CCDWLANServiceExtRecord>* iWLANServiceExtTable = new(ELeave) CMDBRecordSet<CCDWLANServiceExtRecord>(KCDTIdWLANServiceExtRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iWLANServiceExtTable);
			}
			break;

		case KCDTIdPANServiceExtRecord:
			{
			CMDBRecordSet<CCDPANServiceExtRecord>* iPANServiceExtTable = new(ELeave) CMDBRecordSet<CCDPANServiceExtRecord>(KCDTIdPANServiceExtRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iPANServiceExtTable);
			}
			break;

		case KCDTIdVPNServiceRecord:
			{
			CMDBRecordSet<CCDVPNServiceRecord>* iVPNServiceTable = new(ELeave) CMDBRecordSet<CCDVPNServiceRecord>(KCDTIdVPNServiceRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iVPNServiceTable);
			}
			break;

			//	case KCDTIdOutgoingWCDMARecord:
			//		{
			//			CMDBRecordSet<CCDOutgoingWCDMARecord>* iOutgoingWCDMATable  = new(ELeave) CMDBRecordSet<CCDOutgoingWCDMARecord>();
			//			iTable = (CMDBRecordSetBase*)iOutgoingWCDMATable;
			//		}
			//		break;

			//	case KCDTIdIncomingWCDMARecord:
			//		{
			//			CMDBRecordSet<CCDIncomingWCDMARecord>* iIncomingWCDMATable = new(ELeave) CMDBRecordSet<CCDIncomingWCDMARecord>();
			//			iTable = (CMDBRecordSetBase*)iIncomingWCDMATable;
			//		}
			//		break;


		case KCDTIdWCDMAPacketServiceRecord:
			{
			CMDBRecordSet<CCDWCDMAPacketServiceRecord>* iWCDMAPacketServiceTable  = new(ELeave) CMDBRecordSet<CCDWCDMAPacketServiceRecord>(KCDTIdWCDMAPacketServiceRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iWCDMAPacketServiceTable);
			}
			break;

		case KCDTIdOutgoingGprsRecord:
			{
			CMDBRecordSet<CCDOutgoingGprsRecord>* iOutgoingGprsTable  = new(ELeave) CMDBRecordSet<CCDOutgoingGprsRecord>(KCDTIdOutgoingGprsRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iOutgoingGprsTable);
			}
			break;

		case KCDTIdIncomingGprsRecord:
			{
			CMDBRecordSet<CCDIncomingGprsRecord>* iIncomingGprsTable  = new(ELeave) CMDBRecordSet<CCDIncomingGprsRecord>(KCDTIdIncomingGprsRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iIncomingGprsTable);
			}
			break;

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
		case KCDTIdUmtsR99QoSAndOnTableRecord:
			{
			CMDBRecordSet<CCDUmtsR99QoSAndOnTableRecord>* 	iGprsQoSR99Param = new (ELeave) CMDBRecordSet<CCDUmtsR99QoSAndOnTableRecord>(KCDTIdUmtsR99QoSAndOnTableRecord);
			iTable = static_cast<CMDBRecordSetBase*> (iGprsQoSR99Param);
			break;
			}
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS

		case KCDTIdDefaultWCDMARecord:
			{
			CMDBRecordSet<CCDDefaultWCDMARecord>* iDefaultWCDMATable = new(ELeave) CMDBRecordSet<CCDDefaultWCDMARecord>(KCDTIdDefaultWCDMARecord);
			iTable = static_cast<CMDBRecordSetBase*>(iDefaultWCDMATable);
			}
			break;


		case KCDTIdModemBearerRecord:
			{
			CMDBRecordSet<CCDModemBearerRecord>* iModemBearerTable = new(ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iModemBearerTable);
			}
			break;

		case KCDTIdLANBearerRecord:
			{
			CMDBRecordSet<CCDLANBearerRecord>* iLANBearerTable  = new(ELeave) CMDBRecordSet<CCDLANBearerRecord>(KCDTIdLANBearerRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iLANBearerTable);
			}
			break;

		case KCDTIdVirtualBearerRecord:
			{
			CMDBRecordSet<CCDVirtualBearerRecord>* iVirtualBearerTable = new(ELeave) CMDBRecordSet<CCDVirtualBearerRecord>(KCDTIdVirtualBearerRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iVirtualBearerTable );
			}
			break;

		case KCDTIdWAPSMSBearerRecord:
			{
			CMDBRecordSet<CCDWAPSMSBearerRecord>* iWAPSMSBearerTable  = new(ELeave) CMDBRecordSet<CCDWAPSMSBearerRecord>(KCDTIdWAPSMSBearerRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iWAPSMSBearerTable);
			}
			break;

		case KCDTIdWAPIPBearerRecord:
			{
			CMDBRecordSet<CCDWAPIPBearerRecord>* iWAPIPBearerTable  = new(ELeave) CMDBRecordSet<CCDWAPIPBearerRecord>(KCDTIdWAPIPBearerRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iWAPIPBearerTable);
			}
			break;

		case KCDTIdChargecardRecord:
			{
			CMDBRecordSet<CCDChargecardRecord>* iChargecardTable = new(ELeave) CMDBRecordSet<CCDChargecardRecord>(KCDTIdChargecardRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iChargecardTable);
			}
			break;

		case KCDTIdProxiesRecord:
			{
			CMDBRecordSet<CCDProxiesRecord>* iProxiesTable  = new(ELeave) CMDBRecordSet<CCDProxiesRecord>(KCDTIdProxiesRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iProxiesTable);
			}
			break;

		case KCDTIdSSProtoRecord:
			{
			CMDBRecordSet<CCDSecureSocketRecord>* iSecureSocketTable = new(ELeave) CMDBRecordSet<CCDSecureSocketRecord>(KCDTIdSSProtoRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iSecureSocketTable);
			}
			break;

		case KCDTIdAgentLookupRecord:
			{
			CMDBRecordSet<CCDAgentLookupRecord>* iAgentLookupTable = new(ELeave) CMDBRecordSet<CCDAgentLookupRecord>(KCDTIdAgentLookupRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iAgentLookupTable);
			}
			break;
    	case KCDTIdAccessPointRecord:
			{
			CMDBRecordSet<CCDAccessPointRecord>* table = new(ELeave) CMDBRecordSet<CCDAccessPointRecord>(KCDTIdAccessPointRecord);
			iTable = static_cast<CMDBRecordSetBase*>(table);
			}
			break;
    	case KCDTIdIapPrioritySelectionPolicyRecord:
			{
			CMDBRecordSet<CCDIAPPrioritySelectionPolicyRecord>* table = new(ELeave) CMDBRecordSet<CCDIAPPrioritySelectionPolicyRecord>(KCDTIdIapPrioritySelectionPolicyRecord);
			iTable = static_cast<CMDBRecordSetBase*>(table);
			}
			break;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		case KCDTIdConfigAccessPointRecord:
			{
			CMDBRecordSet<CCDConfigAccessPointRecord>* table = new(ELeave) CMDBRecordSet<CCDConfigAccessPointRecord>(KCDTIdConfigAccessPointRecord);
			iTable = static_cast<CMDBRecordSetBase*>(table);
			}
			break;
		case KCDTIdApPrioritySelectionPolicyRecord:
			{
			CMDBRecordSet<CCDAPPrioritySelectionPolicyRecord>* table = new(ELeave) CMDBRecordSet<CCDAPPrioritySelectionPolicyRecord>(KCDTIdApPrioritySelectionPolicyRecord);
			iTable = static_cast<CMDBRecordSetBase*>(table);
			}
			break;
    	case KCDTIdTierRecord:
    		{
			CMDBRecordSet<CCDTierRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDTierRecord>(KCDTIdTierRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;
    	case KCDTIdMCprRecord:
			{
			CMDBRecordSet<CCDMCprRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDMCprRecord>(KCDTIdMCprRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;
    	case KCDTIdCprRecord:
			{
			CMDBRecordSet<CCDCprRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDCprRecord>(KCDTIdCprRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;
    	case KCDTIdSCprRecord:
			{
			CMDBRecordSet<CCDSCprRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDSCprRecord>(KCDTIdSCprRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;
    	case KCDTIdProtocolRecord:
			{
			CMDBRecordSet<CCDProtocolRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDProtocolRecord>(KCDTIdProtocolRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;
		case KCDTIdBearerTypeRecord:
		    {
			CMDBRecordSet<CCDBearerTypeRecord>* ptrRecordSet = new (ELeave)CMDBRecordSet<CCDBearerTypeRecord>(KCDTIdBearerTypeRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		case KCDTIdEAPSecRecord:
			{
			CMDBRecordSet<CCDEAPSecRecord>* iEAPSecTable = new(ELeave) CMDBRecordSet<CCDEAPSecRecord>(KCDTIdEAPSecRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iEAPSecTable);
			}
			break;
		case KCDTIdTunEAPRecord:
			{
			CMDBRecordSet<CCDTunEAPRecord>* iTunEAPTable = new(ELeave) CMDBRecordSet<CCDTunEAPRecord>(KCDTIdTunEAPRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iTunEAPTable);
			}
			break;
		case KCDTIdEAPTLSRecord:
			{
			CMDBRecordSet<CCDEAPTLSRecord>* iEAPTLSTable = new(ELeave) CMDBRecordSet<CCDEAPTLSRecord>(KCDTIdEAPTLSRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iEAPTLSTable);
			}
			break;
		case KCDTIdLEAPRecord:
			{
			CMDBRecordSet<CCDLEAPRecord>* iLEAPTable = new(ELeave) CMDBRecordSet<CCDLEAPRecord>(KCDTIdLEAPRecord);
			iTable = static_cast<CMDBRecordSetBase*>(iLEAPTable);
			}
			break;
    	case KCDTIdEapSimProtocolRecord:
			{
			CMDBRecordSet<CCDEapSimProtocolRecord>* ptrRecordSet =
			new(ELeave) CMDBRecordSet<CCDEapSimProtocolRecord>(KCDTIdEapSimProtocolRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;
    	case KCDTIdEapAkaProtocolRecord:
			{
			CMDBRecordSet<CCDEapAkaProtocolRecord>* ptrRecordSet =
			new(ELeave) CMDBRecordSet<CCDEapAkaProtocolRecord>(KCDTIdEapAkaProtocolRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		case KCDTIdPolicySelectorRecord:
			{
			CMDBRecordSet<CCDPolicySelectorRecord>* ptrRecordSet =
			new(ELeave) CMDBRecordSet<CCDPolicySelectorRecord>(KCDTIdPolicySelectorRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;

		case KCDTIdPolicySelector2ParamsRecord:
			{
			CMDBRecordSet<CCDPolicySelector2ParamsRecord>* ptrRecordSet =
			new(ELeave) CMDBRecordSet<CCDPolicySelector2ParamsRecord>(KCDTIdPolicySelector2ParamsRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;
		case KCDTIdGenericQosRecord:
			{
			CMDBRecordSet<CCDGenericQosRecord>* ptrRecordSet =
			new(ELeave) CMDBRecordSet<CCDGenericQosRecord>(KCDTIdGenericQosRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;
		case KCDTIdWifiScanEngineRecord:
			{
			CMDBRecordSet<CCDWifiScanEngineRecord>* ptrRecordSet =
			new(ELeave) CMDBRecordSet<CCDWifiScanEngineRecord>(KCDTIdWifiScanEngineRecord);
			iTable = (CMDBRecordSetBase*)ptrRecordSet;
			}
			break;
#endif //SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		default:
			{
			User::Leave(KErrNotSupported);
			}
			break;
		};
	}

void CCedDumper::ResetAllAttrubuteFlags()
	{
	iIsHiddenSet = EFalse;
	iIsPrivateSet = EFalse;
	iIsProtectedReadSet = EFalse;
	iIsProtectedWriteSet = EFalse;
	}


// REWORK - LINK TRANSLATION NEEDS REWORKING.

TBool CCedDumper::IsLinkedField(TPtrC& aColumn, TPtrC& aSetting, CMDBElement& /*aField*/)
	{
	if ((aColumn.Compare(TPtrC(KCDTypeNameSelectionPolicy)) == 0)
		|| (aColumn.Compare(TPtrC(KCDTypeNameWLANSecData)) == 0)
		|| (aColumn.Compare(TPtrC(KCDTypeNameEAPSecData)) == 0)
		|| (aColumn.Compare(TPtrC(KCDTypeNameTUNEAPData)) == 0)
		)
		{
		TLex lex(aSetting);
		TUint32 elementid;
		lex.Val(elementid,EDecimal);

		TMDBElementId tableId = elementid & KCDMaskShowRecordType;
		TInt recordId = (elementid & KCDMaskShowRecordId) >> 8;

        if (tableId > 0 )  // because otherwise you match accidentally against NULL entries in list (that shd be removed)
            {
		    TInt i(0);
		    TMDBElementId iterId = elementIdArray[i];

		    while (iterId != NO_MORE_ID )
			    {
			    if(iterId == tableId)
				    {
				    globalValueBuffer = tableArray[i];
				    globalValueBuffer.Append(_L("."));
				    globalValueBuffer.AppendNum(recordId);
				    aSetting.Set(globalValueBuffer);
				    return ETrue;
				    }
			    iterId = elementIdArray[++i];
			    }
            }
		}
	return ETrue;
	}

TBool CCedDumper::ConvertRecordLinks(TPtrC& aColumn, TPtrC& aSetting, CMDBElement* aField)
	{
    TBool ret(EFalse);

    switch(aField->TableId())
        {
        case KCDTIdIAPRecord:
			ret = FillBufferWithLink(ELRIAP, aColumn, aSetting, aField);
            break;
        }

    return ret;
	}

TBool CCedDumper::FillBufferWithLink(TUint32 aIndex, TPtrC& aColumn, TPtrC& aSetting, CMDBElement* aField)
	{
	TBool ret(EFalse);
    TInt i(0);

	while(TPtrC(LinkRecordsArray[aIndex][i]).CompareF(TPtrC(NO_MORE_RECORDS)) != 0)
		{
		if(aColumn.CompareF(TPtrC(LinkRecordsArray[aIndex][i])) == 0)
			{
			TInt value = *(static_cast<CMDBField<TInt>*>(aField));
			// check if it is Tag
			if(value & KLinkableFlagTag && value & KCDMaskShowRecordType)
				{
				globalValueBuffer = KLinkPrefix;
				globalValueBuffer.Append(TPtrC(LinkRecordsArray[aIndex][i]));
				globalValueBuffer.Append(_L("."));
				globalValueBuffer.AppendNum(value & ~(KLinkableFlagTag | KCDMaskShowRecordType));
				aSetting.Set(globalValueBuffer);
				ret = ETrue;
				}
			// check if it is a Record with tableId info
			else if(value & ~KLinkableFlagTag && value & KCDMaskShowRecordType)
				{
				globalValueBuffer = TPtrC(LinkRecordsArray[aIndex][i]);
				globalValueBuffer.Append(_L("."));
				globalValueBuffer.AppendNum((value & ~(KLinkableFlagTag | KCDMaskShowRecordType | KCDMaskShowFieldType))>>8);
				aSetting.Set(globalValueBuffer);
				ret = ETrue;
				}
			}
		i++;
		}

	return ret;
	}

TBool CCedDumper::ConvertEnums(TPtrC& aColumn, TPtrC& aSetting)
/**
Converts any enumerations to readable form

  @param theColumn A reference to a descriptor containing the name of a column in the current record.
  @param setting The setting
  @return ETrue if succcessful else EFalse
  */
	{
	TInt valid = ETrue;
	TInt i = 0;
	TBuf<MAX_COL_NAME_LEN> column;

	// already converted these enums
	if (aSetting.Compare(TPtrC(_L("TRUE"))) == 0 ||
		aSetting.Compare(TPtrC(_L("FALSE"))) == 0 )
		{
		return ETrue;
		}

	// loop through ennumeration columns
	column = ENUMcolumnArray[i];
	while (column.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
		{
		// found the column
		if (column.Compare(aColumn) == 0)
			{
			valid = EFalse;

			// convert the setting to an index and assign contents
			TLex iResult = aSetting;
			TInt j = 0;
			if (iResult.Val(j) == KErrNone)
				{
				if (column.Compare(TPtrC(MODEM_RATE)) == 0)
					{
					// Check for Modem RATE
					switch(j)
						{
						case EBpsAutobaud:
							globalValueBuffer = MODEM_RATE_AUTO;
							valid = ETrue;
							break;
						case EBpsSpecial:
							globalValueBuffer = MODEM_RATE_SPECIAL;
							valid = ETrue;
							break;
						default:
							globalValueBuffer = EnumerationArray[i][j];
							valid = ETrue;
							break;
						};
					}
				else if (column.Compare(TPtrC(MODEM_FAX_CLASS_PREF)) == 0)
					{
					// Check for MODEM_FAX_CLASS_PREF enums
					switch(j)
						{
						case EClassAuto:
							globalValueBuffer = MODEM_FAX_AUTO;
							valid = ETrue;
							break;
						case EClass1:
							globalValueBuffer = MODEM_FAX_1;
							valid = ETrue;
							break;
						case EClass2:
							globalValueBuffer = MODEM_FAX_2;
							valid = ETrue;
							break;
						case EClass2point0:
							globalValueBuffer = MODEM_FAX_2POINT0;
							valid = ETrue;
							break;
						case EClass1point0:
							globalValueBuffer = MODEM_FAX_1POINT0;
							valid = ETrue;
							break;
						case EClass2point1:
							globalValueBuffer = MODEM_FAX_2POINT1;
							valid = ETrue;
							break;
						default:
							globalValueBuffer = EnumerationArray[i][j];
							valid = ETrue;
							break;
						};
					}
				else if (column.Compare(TPtrC(ISP_BEARER_NAME)) == 0)
					{
					// Check for ISP_BEARER_NAME enums
					switch(j)
						{
						case RMobileCall::KCapsDataCircuitAsynchronous:
							globalValueBuffer = BEARER_NAME_ASYNCHRONOUS;
							valid = ETrue;
							break;
						case RMobileCall::KCapsDataCircuitAsynchronousRDI:
							globalValueBuffer = BEARER_NAME_ASYNCHRONOUSRDI;
							valid = ETrue;
							break;
						case RMobileCall::KCapsDataCircuitSynchronous:
							globalValueBuffer = BEARER_NAME_SYNCHRONOUS;
							valid = ETrue;
							break;
						case RMobileCall::KCapsDataCircuitSynchronousRDI:
							globalValueBuffer = BEARER_NAME_SYNCHRONOUSRDI;
							valid = ETrue;
							break;
						case RMobileCall::KCapsPADAsyncUDI:
							globalValueBuffer = BEARER_NAME_PADASYNCUDI;
							valid = ETrue;
							break;
						case RMobileCall::KCapsPADAsyncRDI:
							globalValueBuffer = BEARER_NAME_PADASYNCRDI;
							valid = ETrue;
							break;
						case RMobileCall::KCapsPacketAccessSyncUDI:
							globalValueBuffer = BEARER_NAME_ACCESSSYNCUDI;
							valid = ETrue;
							break;
						case RMobileCall::KCapsPacketAccessSyncRDI:
							globalValueBuffer = BEARER_NAME_ACCESSSYNCRDI;
							valid = ETrue;
							break;
						case RMobileCall::KCapsServiceExtended:
							globalValueBuffer = BEARER_NAME_SERVICEEXTENDED;
							valid = ETrue;
							break;
						default:
							// Default to EXTENDED - can't use the value as an index, as it's a
							// bitmap.
							globalValueBuffer = BEARER_NAME_SERVICEEXTENDED;
							valid = ETrue;
							break;
						};
					}
				else if (column.Compare(TPtrC(ISP_IF_CALLBACK_TYPE)) == 0)
					{
					// Check for IFCALLBACKTYPE enums
					switch(j)
						{
						case ECallbackActionMSCBCPRequireClientSpecifiedNumber:
							globalValueBuffer = MSCBCPCLIENTNUM;
							valid = ETrue;
							break;
						case ECallbackActionMSCBCPAcceptServerSpecifiedNumber:
							globalValueBuffer = MSCBCPSERVERNUM;
							valid = ETrue;
							break;
						case ECallbackActionMSCBCPOverrideServerSpecifiedNumber:
							globalValueBuffer = MSCBCPOVERRIDESER;
							valid = ETrue;
							break;
						default:
							globalValueBuffer = EnumerationArray[i][j];
							valid = ETrue;
							break;
						};
					}
				else if (column.Compare(TPtrC(CONNECT_PREF_BEARER_SET)) == 0)
					{
					// Check for CONNECT_PREF_BEARER_SET enums
                    if(j == 0)
                        {
                        globalValueBuffer = BEARER_SET_UNKNOWN;
                        valid = ETrue;
                        }
                    else
                        {
                        TBool first(ETrue);

                        globalValueBuffer = _L("");

                        if(j & KCommDbBearerCSD)
                            {
                            first = EFalse;
                            globalValueBuffer.Append(TPtrC(BEARER_SET_CSD));
                            }

                        if(j & KCommDbBearerLAN)
                            {
                            if(!first)
                                {
                                globalValueBuffer.Append(_L(" & "));
                                }

                            first = EFalse;
                            globalValueBuffer.Append(TPtrC(BEARER_SET_LAN));
                            }

                        if(j & KCommDbBearerWLAN)
                            {
                            if(!first)
                                {
                                globalValueBuffer.Append(_L(" & "));
                                }

                            first = EFalse;
                            globalValueBuffer.Append(TPtrC(BEARER_SET_WLAN));
                            }

                        if(j & KCommDbBearerVirtual)
                            {
                            if(!first)
                                {
                                globalValueBuffer.Append(_L(" & "));
                                }

                            first = EFalse;
                            globalValueBuffer.Append(TPtrC(BEARER_SET_VIRTUAL));
                            }

                        if(j & KCommDbBearerPAN)
                            {
                            if(!first)
                                {
                                globalValueBuffer.Append(_L(" & "));
                                }

                            first = EFalse;
                            globalValueBuffer.Append(TPtrC(BEARER_SET_PAN));
                            }

                        if( (j & KCommDbBearerPSD) == KCommDbBearerPSD)
                            {
                            if(!first)
                                {
                                globalValueBuffer.Append(_L(" & "));
                                }

                            first = EFalse;
                            globalValueBuffer.Append(TPtrC(BEARER_SET_PSD));
                            }
                        else
                            {
                            if(j & KCommDbBearerWcdma)
                                {
                                if(!first)
                                    {
                                    globalValueBuffer.Append(_L(" & "));
                                    }

                                first = EFalse;
                                globalValueBuffer.Append(TPtrC(BEARER_SET_WCDMA));
                                }
                            }

                        if(globalValueBuffer.Length() == 0)
                         	{
							globalValueBuffer = EnumerationArray[i][j];
                        	}

                        valid = ETrue;
                        }

//					globalValueBuffer = EnumerationArray[i][j];
					}
				else if (column.Compare(TPtrC(CDMA_SERVICE_OPTION)) == 0)
					{
					// Check for CDMA_SERVICE_OPTION enums
					switch(j)
						{
						case RPacketContext::KLowSpeedData:
							globalValueBuffer = CDMA_SERVICE_LOWSPEEDDATA;
							valid = ETrue;
							break;

						case RPacketContext::KHighSpeedData:
							globalValueBuffer = CDMA_SERVICE_HIGHSPEEDDATA;
							valid = ETrue;
							break;

							case (RPacketContext::KLowSpeedData |
								RPacketContext::KHighSpeedData):
								globalValueBuffer = CDMA_SERVICE_LOW_AND_HIGHSPEEDDATA;
								valid = ETrue;
								break;

						default:
							globalValueBuffer = EnumerationArray[i][j];
							valid = ETrue;
							break;
						};
					}
				else if (column.Compare(TPtrC(CDMA_RLP_MODE)) == 0)
					{
					// Check for CDMA_RLP_MODE enums
					switch(j)
						{
						case RPacketQoS::KRLPUnknown:
							globalValueBuffer = CDMA_RLP_UNKNOWN;
							valid = ETrue;
							break;
						case RPacketQoS::KRLPTransparent:
							globalValueBuffer = CDMA_RLP_TRANSPARENT;
							valid = ETrue;
							break;
						case RPacketQoS::KRLPNonTransparent:
							globalValueBuffer = CDMA_RLP_NONTRANSPARENT;
							valid = ETrue;
							break;
						case RPacketQoS::KRLPTransparentPref:
							globalValueBuffer = CDMA_RLP_TRANSPARENTPREF;
							valid = ETrue;
							break;
						case RPacketQoS::KRLPNonTransparentPref:
							globalValueBuffer = CDMA_RLP_NONTRANSPARENTPREF;
							valid = ETrue;
							break;
						default:
							globalValueBuffer = EnumerationArray[i][j];
							valid = ETrue;
							break;
						};
					}
				else if ((column.Compare(TPtrC(CDMA_REQ_FWD_PRIORITY)) == 0)
					|| (column.Compare(TPtrC(CDMA_REQ_REV_PRIORITY)) == 0))
					{
					// Check for CDMA_REQ_FWD_PRIORITY
					// and CDMA_REQ_REV_PRIORITY enums
					switch(j)
						{
						case RPacketQoS::ELinkPriority00:
							globalValueBuffer = CDMA_PRIORITY_00;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority01:
							globalValueBuffer = CDMA_PRIORITY_01;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority02:
							globalValueBuffer = CDMA_PRIORITY_02;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority03:
							globalValueBuffer = CDMA_PRIORITY_03;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority04:
							globalValueBuffer = CDMA_PRIORITY_04;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority05:
							globalValueBuffer = CDMA_PRIORITY_05;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority06:
							globalValueBuffer = CDMA_PRIORITY_06;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority07:
							globalValueBuffer = CDMA_PRIORITY_07;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority08:
							globalValueBuffer = CDMA_PRIORITY_08;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority09:
							globalValueBuffer = CDMA_PRIORITY_09;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority10:
							globalValueBuffer = CDMA_PRIORITY_10;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority11:
							globalValueBuffer = CDMA_PRIORITY_11;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority12:
							globalValueBuffer = CDMA_PRIORITY_12;
							valid = ETrue;
							break;
						case RPacketQoS::ELinkPriority13:
							globalValueBuffer = CDMA_PRIORITY_13;
							valid = ETrue;
							break;
						default:
							globalValueBuffer = EnumerationArray[i][j];
							valid = ETrue;
							break;
						};
					}
				else if ((column.Compare(TPtrC(CDMA_REQ_FWD_BITRATE)) == 0)
					|| (column.Compare(TPtrC(CDMA_REQ_REV_BITRATE)) == 0)
					|| (column.Compare(TPtrC(CDMA_MIN_FWD_BITRATE)) == 0)
					|| (column.Compare(TPtrC(CDMA_MIN_REV_BITRATE)) == 0))
					{
					// Check for CDMA_REQ_FWD_BITRATE,
					// CDMA_REQ_REV_BITRATE, CDMA_MIN_FWD_BITRATE
					// and CDMA_MIN_REV_BITRATE enums
					switch(j)
						{
						case RPacketQoS::EDataRate8kbps:
							globalValueBuffer = CDMA_BITRATE_8;
							valid = ETrue;
							break;
						case RPacketQoS::EDataRate32kbps:
							globalValueBuffer = CDMA_BITRATE_32;
							valid = ETrue;
							break;
						case RPacketQoS::EDataRate64kbps:
							globalValueBuffer = CDMA_BITRATE_64;
							valid = ETrue;
							break;
						case RPacketQoS::EDataRate144kbps:
							globalValueBuffer = CDMA_BITRATE_144;
							valid = ETrue;
							break;
						case RPacketQoS::EDataRate384kbps:
							globalValueBuffer = CDMA_BITRATE_384;
							valid = ETrue;
							break;
						default:
							globalValueBuffer = EnumerationArray[i][j];
							valid = ETrue;
							break;
						};
					}
				else if ((column.Compare(TPtrC(CDMA_REQ_FWD_LOSS)) == 0)
					|| (column.Compare(TPtrC(CDMA_REQ_REV_LOSS)) == 0)
					|| (column.Compare(TPtrC(CDMA_ACCPT_FWD_LOSS)) == 0)
					|| (column.Compare(TPtrC(CDMA_ACCPT_REV_LOSS)) == 0))
					{
					// Check for CDMA_REQ_FWD_LOSS,
					// CDMA_REQ_REV_LOSS, CDMA_ACCPT_FWD_LOSS
					// and CDMA_ACCPT_REV_LOSS enums
					switch(j)
						{
						case RPacketQoS::EDataLoss1:
							globalValueBuffer = CDMA_LOSS_1;
							valid = ETrue;
							break;
						case RPacketQoS::EDataLoss2:
							globalValueBuffer = CDMA_LOSS_2;
							valid = ETrue;
							break;
						case RPacketQoS::EDataLoss5:
							globalValueBuffer = CDMA_LOSS_5;
							valid = ETrue;
							break;
						case RPacketQoS::EDataLoss10:
							globalValueBuffer = CDMA_LOSS_10;
							valid = ETrue;
							break;
						default:
							globalValueBuffer = EnumerationArray[i][j];
							valid = ETrue;
							break;
						};
					}
				else if ((column.Compare(TPtrC(CDMA_REQ_FWD_MAXDELAY)) == 0)
					|| (column.Compare(TPtrC(CDMA_REQ_REV_MAXDELAY)) == 0)
					|| (column.Compare(TPtrC(CDMA_ACCPT_FWD_MAXDELAY)) == 0)
					|| (column.Compare(TPtrC(CDMA_ACCPT_REV_MAXDELAY)) == 0))
					{
					// Check for CDMA_REQ_FWD_MAXDELAY,
					// CDMA_REQ_REV_MAXDELAY, CDMA_ACCPT_FWD_MAXDELAY
					// and CDMA_ACCPT_REV_MAXDELAY enums
					switch(j)
						{
						case RPacketQoS::EUnspecifiedDelayClass:
							globalValueBuffer = CDMA_MAXDELAY_CLASS;
							valid = ETrue;
							break;
						case RPacketQoS::EDelayClass1:
							globalValueBuffer = CDMA_MAXDELAY_CLASS1;
							valid = ETrue;
							break;
						case RPacketQoS::EDelayClass2:
							globalValueBuffer = CDMA_MAXDELAY_CLASS2;
							valid = ETrue;
							break;
						case RPacketQoS::EDelayClass3:
							globalValueBuffer = CDMA_MAXDELAY_CLASS3;
							valid = ETrue;
							break;
						case RPacketQoS::EDelayClass4:
							globalValueBuffer = CDMA_MAXDELAY_CLASS4;
							valid = ETrue;
							break;
						case RPacketQoS::EDelay40ms:
							globalValueBuffer = CDMA_MAXDELAY_40MS;
							valid = ETrue;
							break;
						case RPacketQoS::EDelay120ms:
							globalValueBuffer = CDMA_MAXDELAY_120MS;
							valid = ETrue;
							break;
						case RPacketQoS::EDelay360ms:
							globalValueBuffer = CDMA_MAXDELAY_360MS;
							valid = ETrue;
							break;
						default:
							globalValueBuffer = EnumerationArray[i][j];
							valid = ETrue;
							break;
						};
					}
				else if ((column.Compare(TPtrC(ISP_BEARER_PROTOCOL)) == 0))
					{
					// Search the lookup table for this value and map it to the associated
					// descriptor.
					// 0 will always give "UNSPECIFIED" (as it should) rather than "0".
                    const TDescEnumMap *pMap = IspBearerProtocolMap;
					while((pMap->EnumVal != (TUint32)j) && (pMap->EnumVal != RMobileCall::EProtocolUnspecified))
                        ++pMap;
					globalValueBuffer = pMap->OptionDescriptor;
					valid = ETrue;
					}
				else if ((column.Compare(TPtrC(PAN_LOCAL_ROLE)) == 0)
					|| (column.Compare(TPtrC(PAN_PEER_ROLE)) == 0))
					{
						switch(j)
						{
						case EPanRoleU:
							globalValueBuffer = PAN_ROLE_U;
							valid = ETrue;
							break;
						case EPanRoleGn:
							globalValueBuffer = PAN_ROLE_GN;
							valid = ETrue;
							break;
						case EPanRoleNap:
							globalValueBuffer = PAN_ROLE_NAP;
							valid = ETrue;
							break;
						case EPanRoleUnknown:
							globalValueBuffer = PAN_ROLE_UNKNOWN;
							valid = ETrue;
							break;
						default:
							globalValueBuffer = EnumerationArray[i][j];
							valid = ETrue;
							break;
						};
					}
				else if (column.Compare(TPtrC(BEARER_TECHNOLOGY)) == 0)
					{
						switch(j)
						{
						case KCommDbBearerUnknown:
							globalValueBuffer = BEARER_SET_UNKNOWN;
							valid = ETrue;
							break;
						case KCommDbBearerCSD:
							globalValueBuffer = BEARER_SET_CSD;
							valid = ETrue;
							break;
						case KCommDbBearerWcdma:
							globalValueBuffer = BEARER_SET_WCDMA;
							valid = ETrue;
							break;
						case KCommDbBearerLAN:
							globalValueBuffer = BEARER_SET_LAN;
							valid = ETrue;
							break;
						case KCommDbBearerPAN:
							globalValueBuffer = BEARER_SET_PAN;
							valid = ETrue;
							break;
						case KCommDbBearerWLAN:
							globalValueBuffer = BEARER_SET_WLAN;
							valid = ETrue;
							break;
						case KCommDbBearerVirtual:
							globalValueBuffer = BEARER_SET_VIRTUAL;
							valid = ETrue;
							break;
						default:
							//globalValueBuffer = EnumerationArray[i][j];
							//valid = ETrue;
							valid = EFalse;
							break;
						};
					}
				else if(column.Compare(TPtrC(TUN_EAP_INNERTYPE)) == 0)
					{
					valid = EFalse;
					TUint type(0);
					TBuf<MAX_COL_NAME_LEN> value;
					value = ENUM_TUN_EAP_INNERTYPEArray[type];
					while (value.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
						{
						if((TInt)ENUM_TUN_EAP_INNERTYPE_VALArray[type] == j)
							{
							globalValueBuffer = value;
							valid = ETrue;
							break;
							}
						type++;
						value = ENUM_TUN_EAP_INNERTYPEArray[type];
						}
					}
				else if(column.Compare(TPtrC(EAP_SEC_OUTER_EAP_TYPE)) == 0)
					{
					valid = EFalse;
					TUint type(0);
					TBuf<MAX_COL_NAME_LEN> value;
					value = ENUM_EAP_SEC_OUTER_EAP_TYPEArray[type];
					while (value.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
						{
						if((TInt)ENUM_EAP_SEC_OUTER_EAP_TYPE_VALArray[type] == j)
							{
							globalValueBuffer = value;
							valid = ETrue;
							break;
							}
						type++;
						value = ENUM_EAP_SEC_OUTER_EAP_TYPEArray[type];
						}
					}
				else if(column.Compare(TPtrC(WLAN_DES_TRANS_RATE)) == 0)
					{
					valid = EFalse;
					TUint type(0);
					TBuf<MAX_COL_NAME_LEN> value;
					value = ENUM_WLAN_DES_TRANS_RATEArray[type];
					while (value.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
						{
						if((TInt)ENUM_WLAN_DES_TRANS_RATE_VALArray[type] == j)
							{
							globalValueBuffer = value;
							valid = ETrue;
							break;
							}
						type++;
						value = ENUM_WLAN_DES_TRANS_RATEArray[type];
						}
					}
				else if(column.Compare(TPtrC(WLAN_REG_DOMAIN)) == 0 || column.Compare(TPtrC(REGULATORY_DOMAIN)) == 0)
					{
					valid = EFalse;
					TUint type(0);
					TBuf<MAX_COL_NAME_LEN> value;
					value = ENUM_WLAN_REG_DOMAINArray[type];
					while (value.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
						{
						if((TInt)ENUM_WLAN_REG_DOMAIN_VALArray[type] == j)
							{
							globalValueBuffer = value;
							valid = ETrue;
							break;
							}
						type++;
						value = ENUM_WLAN_REG_DOMAINArray[type];
						}
					}
				else if(column.Compare(TPtrC(DEFAULT_REGULATORY_DOMAIN)) == 0)
					{
					valid = EFalse;
					TUint type(0);
					TBuf<MAX_COL_NAME_LEN> value;
					value = ENUM_WLAN_REG_DOMAINArray[type];
					while (value.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
						{
						if((TInt)ENUM_WLAN_REG_DOMAIN_VALArray[type] == j)
							{
							globalValueBuffer = value;
							valid = ETrue;
							break;
							}
						type++;
						value = ENUM_WLAN_REG_DOMAINArray[type];
						}
					}
				else
					{
					globalValueBuffer = EnumerationArray[i][j];
					valid = ETrue;
					}
			}

			// in case they've changed during enum conversion
			if (valid)
				{
				//				console->Printf(_L("[%S][%S]\n"), &theColumn, &globalValueBuffer);
				//				console->Getch(); // get and ignore character
				aSetting.Set(globalValueBuffer);
				break;
				}
		}

		i++;
		column = ENUMcolumnArray[i];
	}

	return valid;
}

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
/**
Check if given column is not deprecated within incoming/outgoing GPRS table.
Additionally converts the setting to new R99 GPRS QoS values if necessary.
Assumes setting is given as R97/98 GPRS QoS parameter.

  @param theColumn A reference to a descriptor containing a name of a column in current record.
  @param setting The setting
  @return CR99QoSConversion::TConvRecordType
  */
CR99QoSConversion::TConvRecordType CCedDumper::CheckAndPerformConversion(TPtrC &theColumn, TPtrC &setting)
	{
	if (iR99ConversionRecordsCurrent == NULL)
		{
		//It is the first parameter spoted for particular instance of incoming/outgoing GPRS table.
		TRAPD(ret, iR99ConversionRecordsCurrent = CR99QoSConversion::NewL() )
		if (ret != KErrNone)
			{
			return CR99QoSConversion::EConvertionError;
			}

		//Link created object to the uni-direction list
		if (iR99ConversionRecordsHead == NULL)
			{
			iR99ConversionRecordsHead = iR99ConversionRecordsCurrent;
			}
		else
			{
			CR99QoSConversion *temp = iR99ConversionRecordsHead;
			while(temp->GetNext()  != NULL)
				{
				temp = temp->GetNext();
				}
			temp->SetNext(iR99ConversionRecordsCurrent);
			}
		}
	//Pass the request to CR99QoSConversion object
	return iR99ConversionRecordsCurrent->CheckAndConvert(theColumn, setting);
	}

/**
Prints out all conversion tables from uni-direction list to the output file
*/
void CCedDumper::WriteR99Conversion()
	{
	TBuf<MAX_COL_NAME_LEN> tempTable;

	//Is there anything to print out?
	if (!iR99ConversionRecordsHead || (iR99ConversionRecordsHead && !iR99ConversionRecordsHead->GetModificationFlag()))
		return;

	//write the table header to the output file
	tempTable = TPtrC(QOS_UMTS_R99_AND_ON_TABLE);
	iFileDumper->WriteTableHeader(tempTable);

	//loop through all object within the list
	iR99ConversionRecordsCurrent = iR99ConversionRecordsHead;
	while(iR99ConversionRecordsCurrent)
		{
		iR99ConversionRecordsCurrent->PrintValues(iFileDumper);
		iR99ConversionRecordsCurrent = iR99ConversionRecordsCurrent->GetNext();
		}

	}
#endif
//SYMBIAN_NETWORKING_3GPPDEFAULTQOS

void CCedDumper::ListGenericTablesL()
	{
	TUint32 KMaskForNames = 0x007FFFFF;
	TUint32 KKeyForNames = KCDMaskShowFieldType | KCDMaskShowRecordId; 
	
	
	RArray<TUint32> tableIds;
	iRepository->FindL(KKeyForNames, KMaskForNames, tableIds);
	for(TInt i=0; i<tableIds.Count();i++)
		{
		if(tableIds[i] >= KCDInitialUDefRecordType &&
		   tableIds[i] <= KCDLastUDefRecordType)
			{
			TBuf<KCDMaxFieldNameLength> name;
			iRepository->Get(tableIds[i], name);
			iGenericTableIds.Append(tableIds[i]);
			iGenericTableNames.Append(name);
			
			RRecordInfoArray recordInfoArray;
			
			RArray<TUint32> fields;
			TUint32 KMaskForFields = 0xFF80FFFF;
			TUint32 tableId = tableIds[i];
			iRepository->FindL(tableIds[i], KMaskForFields, fields);
			for(TInt j=0; j<fields.Count() - 1;j++)
			//Do not care about the last one as it is a delimiter.
				{
				SGenericRecordTypeInfo ptr;
				TPckg<SGenericRecordTypeInfo> package(ptr);
			
				iRepository->Get(fields[j], package);
				
				recordInfoArray.Append(ptr);
				}
			iGenericTableFields.Append(recordInfoArray);
			fields.Close();
			}
		
		}
	
	tableIds.Close();
	}
	
void CCedDumper::ConvertFieldTypeL(TInt aFieldType, TDes &aConvertedFieldType)
	{
	switch(aFieldType)
		{
		case EText:
			aConvertedFieldType.Copy(_L("EText"));
			break;
		case EDesC8:
			aConvertedFieldType.Copy(_L("EDesC8"));
			break;
		case EUint32:
			aConvertedFieldType.Copy(_L("EUint32"));
			break;
		case EInt:
			aConvertedFieldType.Copy(_L("EInt"));
			break;
		case EBool:
			aConvertedFieldType.Copy(_L("EBool"));
			break;
		case EMedText:
			aConvertedFieldType.Copy(_L("EMedText"));
			break;
		case ELongText:
			aConvertedFieldType.Copy(_L("ELongText"));
			break;	
		case ELink:
			aConvertedFieldType.Copy(_L("ELink"));
			break;
		default:
			User::Leave(KErrArgument);
		}
	
	}

void CCedDumper::ConvertFieldAttributeL(TInt aFieldAttribute, TDes &aConvertedFieldAttribute)
	{
	switch(aFieldAttribute)
		{
		case ENoAttrs:
			aConvertedFieldAttribute.Copy(_L("ENoAttrs"));
			break;
		case ENotNull:
			aConvertedFieldAttribute.Copy(_L("ENotNull"));
			break;
		default:
			User::Leave(KErrArgument);
		}
	}
	
void CCedDumper::DumpGenericTablesL()
	{

	for(TInt i = 0; i< 	iGenericTableIds.Count(); i++)
		{
		TUint32 tableId = iGenericTableIds[i] & KCDMaskShowRecordType;
		
		CMDBRecordSet<CMDBGenericRecord>* recordSet = new (ELeave) CMDBRecordSet<CMDBGenericRecord>(tableId);
		CleanupStack::PushL(recordSet);
		
		
		iFileDumper->WriteTableHeader(iGenericTableNames[i]);
		
		//First print out table declaration
		if(iGenericTableFields[i].Count()>0)
			{
			iFileDumper->WriteSectionHeader(-2);
			_LIT(KFieldStructure, "FIELDSTRUCTURE");
			_LIT(KFormat, "%08x,%S,%S,%S");
			TPtrC ptrFieldStructure(KFieldStructure);
			for(TInt f = 0; f< iGenericTableFields[i].Count(); f++)
				{
				// line will hold sth like 00120000,EText,ENotNull,SomeName
				// its total length is:
				// hex id : 8 chars
				// type : max 9 chars
				// attribute: max 8 chars
				// 
				TBuf<KCDMaxFieldNameLength + 30> line;				
				// type: max 9 chars
				TBuf<9> fieldType, fieldAttribute;
				ConvertFieldTypeL(iGenericTableFields[i][f].iValType, fieldType);
				ConvertFieldAttributeL(iGenericTableFields[i][f].iTypeAttr, fieldAttribute);
				line.Format(KFormat, iGenericTableFields[i][f].iTypeId, &fieldType, &fieldAttribute ,&(iGenericTableFields[i][f].iTypeName));
				TPtrC ptrFieldLine(line);
				iFileDumper->WriteColumnValue(ptrFieldStructure, ptrFieldLine);
				}
			iFileDumper->WriteSectionFooter(-2);
			}

		TRAPD(err, recordSet->LoadL(*iDbSession));
		if(err!= KErrNotFound)
			{
			User::LeaveIfError(err);
			}
			
		for(TInt j = 0; j<recordSet->iRecords.Count();j++)
			{
			iFileDumper->WriteSectionHeader(recordSet->iRecords[j]->RecordId());
			TInt noOfFields(0);
			for(TInt k = 0; k < iGenericTableFields[i].Count(); k++)
				{
				SGenericRecordTypeInfo recordTypeInfo = iGenericTableFields[i][k];
				if(WriteFieldValueL(reinterpret_cast<CMDBGenericRecord*>(recordSet->iRecords[j]), iGenericTableFields[i][k].iTypeName))
					{
					noOfFields++;
					}
				}
			iFileDumper->WriteFieldCount(noOfFields);
			iFileDumper->WriteSectionFooter(recordSet->iRecords[j]->RecordId());
			}
		CleanupStack::PopAndDestroy(recordSet);
		}
	}
	
TBool CCedDumper::WriteFieldValueL(CMDBGenericRecord* aRecord, const TDesC& aFieldName)
	{
	TInt valType;
	TPtrC ptrFieldName(aFieldName);
	TBuf<20> bufVal;
	CMDBElement* element = aRecord->GetFieldByNameL(aFieldName, valType); 
	if(element->IsNull())
		{
		return EFalse;
		}
	switch(valType)
		{
		case EText:
		case EMedText:
		case ELongText:
			{
			CMDBField<TDesC>* textField = static_cast<CMDBField<TDesC> *>(element);
			TPtrC ptrTextField(*textField);
			iFileDumper->WriteColumnValue(ptrFieldName, ptrTextField);
			break;
			}
		case EDesC8:
			{
			CMDBField<TDesC8>* text8Field = static_cast<CMDBField<TDesC8> *>(element);
			TPtrC8 ptrText8Field(*text8Field);
			iFileDumper->WriteColumnValue(ptrFieldName, reinterpret_cast<TPtrC&>(ptrText8Field));
			break;
			}
		case EInt:
			{
			TInt intVal;
			CMDBField<TInt>* intField = static_cast<CMDBField<TInt> *>(element);		
			intVal = *intField;
			bufVal.Num(intVal);
			TPtrC ptrIntField(bufVal.MidTPtr(0));
			iFileDumper->WriteColumnValue(ptrFieldName, ptrIntField);
			break; 
			}
		case EBool:
			{
			TBool boolVal;
			CMDBField<TInt>* boolField = static_cast<CMDBField<TInt> *>(element);
			boolVal = *boolField;
			TPtrC ptrBoolField(boolVal?TRUE_VAL:FALSE_VAL);
			iFileDumper->WriteColumnValue(ptrFieldName, ptrBoolField);
			break; 
			}
		case EUint32:
		case ELink:
			{
			TInt int32Val;
			CMDBField<TInt>* int32Field = static_cast<CMDBField<TInt> *>(element);
			int32Val = *int32Field;
			bufVal.Num(int32Val);
			TPtrC ptrInt32Field(bufVal.MidTPtr(0));
			iFileDumper->WriteColumnValue(ptrFieldName, ptrInt32Field);
			break;		
			}
		default:
			User::Leave(KErrArgument);
		}
		return ETrue;
	}
