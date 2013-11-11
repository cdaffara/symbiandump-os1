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
#include "CDBSTD.H"
#include "Commsdat_Log.h"
#include "commdb_impl.H"
#include <metadatabase.h>
#include <commsdat_partner.h>


/**
Maps a bearer type and direction to a service table (Dialout,GPRS)
Used by `OpenIAPTableViewMatchingBearerSetLC()`.
@internalComponent
*/
struct TMapBearerToServiceType
	{
	TCommDbBearer				bearerMask;
	TCommDbConnectionDirection	direction;
	const TText*				serviceType;
	};
/**
Service Type
@internalComponent
*/
const TInt KNumServiceType = 13;

// used 1st DoInsertRecord parameter
const TBool KCreateCopyRecord 	= ETrue;
const TBool KCreateNewRecord 	= EFalse;

// used as 3th DoInsertRecord parameter
const TBool KUseTemplate 		= ETrue;
const TBool KDoNotUseTemplate 	= EFalse;

const TBool KDeleteFromContainerOnly = ETrue;
/**
Array of type TMapBearerToServiceType
@internalComponent
*/
const TMapBearerToServiceType KMap[KNumServiceType] =
	{
	{KCommDbBearerCSD,		ECommDbConnectionDirectionOutgoing, DIAL_OUT_ISP},
	{KCommDbBearerCSD,		ECommDbConnectionDirectionIncoming, DIAL_IN_ISP},
	{KCommDbBearerWcdma,	ECommDbConnectionDirectionOutgoing, OUTGOING_GPRS},
	 /** The following two mappings are only required for tornado */
	{KCommDbBearerWcdma,	ECommDbConnectionDirectionIncoming, INCOMING_GPRS},
	{KCommDbBearerLAN,		ECommDbConnectionDirectionIncoming,	LAN_SERVICE},
	{KCommDbBearerLAN,		ECommDbConnectionDirectionOutgoing,	LAN_SERVICE},
	{KCommDbBearerVirtual,		ECommDbConnectionDirectionIncoming,	VPN_SERVICE},
	{KCommDbBearerVirtual,		ECommDbConnectionDirectionOutgoing,	VPN_SERVICE},
	/** PAN profile uses LAN service table, plus an extension service table */
	{KCommDbBearerPAN,		ECommDbConnectionDirectionIncoming,	LAN_SERVICE},
	{KCommDbBearerPAN,		ECommDbConnectionDirectionOutgoing,	LAN_SERVICE},
	/** WLAN uses LAN service table, plus an extension service table */
	{KCommDbBearerWLAN,		ECommDbConnectionDirectionIncoming,	LAN_SERVICE},
	{KCommDbBearerWLAN,		ECommDbConnectionDirectionOutgoing,	LAN_SERVICE}
	};
/**
Index in RPointerArray where Template Record is stored
*/
using namespace CommsDat;

// CCommsDbTableView definitions
CCommsDbTableView* CCommsDbTableView::NewL(CCommsDatabaseBase& /*aDb*/,const TDesC& /*aTableName*/,const TDbQuery& /*aQuery*/,TBool /*aUseTemplate*/)
/** Internal low level view creation.

Not in use after shim is introduced

Use if CCommsDatabase::OpenTableLC() or CCommsDatabase::OpenViewLC...() are not sufficient.

Note that OpenTableLC() and OpenViewLC...() have been withdrawn in CCommsDatabase
and are now implemented in CCommsDatabaseBase.

@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@param aDb Reference to the instance of the Comms Database to use.
@param aTableName Name of the table of view.
@param aQuery Query to perform.
@param aUseTemplate Use the template values from the database - defaults to ETrue.
@return A reference to the newly created view. */
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

/** Internal low level view creation.

Internal use as replacement for SQL queries.
Introduced with commdb shim

@param aDb Reference to the instance of the Comms Database to use.
@param aTableName Name of the table of view.
@param aIncludeHiddenRecords Value for Hidden attribute.
@return A reference to the newly created view. */
CCommsDbTableView* CCommsDbTableView::NewLC(CCommsDatabaseBase& aDb, const TDesC& aTableName, TBool aUseTemplate)
	{
	CCommsDbTableView* r = new(ELeave) CCommsDbTableView(aDb, aTableName);
	CleanupStack::PushL(r);
	r->ConstructL(aTableName,aDb.iShowHiddenRecords, aUseTemplate);
	// register view in Database
	aDb.iImpl->RegisterViewL(r);
	return r;
	}

/** Internal low level view creation.

Used only if is called from TemplateRecord class

@param aDb Reference to the instance of the Comms Database to use.
@param aTableName Name of the table of view.
@param aIncludeHiddenRecords Value for Hidden attribute.
@return A reference to the newly created view. */
CCommsDbTableView* CCommsDbTableView::NewLC(const TDesC& aTableName, CCommsDatabaseBase& aDb)
	{
	CCommsDbTableView* r = new(ELeave) CCommsDbTableView(aDb, aTableName);
	CleanupStack::PushL(r);
	r->ConstructL(aTableName);
	// register view in Database
	aDb.iImpl->RegisterViewL(r);
	return r;
	}

/** Internal low level view creation.

Use if CCommsDatabase::OpenTableLC() or CCommsDatabase::OpenViewLC...() are not sufficient.

Note that OpenTableLC() and OpenViewLC...() have been withdrawn in CCommsDatabase
and are now implemented in CCommsDatabaseBase.

@param aDb Reference to the instance of the Comms Database to use.
@param aTableName Name of the table of view.
@param aIncludeHiddenRecords Value for Hidden attribute.
@return A reference to the newly created view. */
CCommsDbTableView* CCommsDbTableView::NewL(CCommsDatabaseBase& aDb, const TDesC& aTableName, const TDesC& aColumnToMatch, const TDesC8& aValueToMatch)
	{
	CCommsDbTableView* r = new(ELeave) CCommsDbTableView(aDb,aTableName);
	CleanupStack::PushL(r);
	r->ConstructL(aTableName, aColumnToMatch, aValueToMatch);
	// register view in Database
	aDb.iImpl->RegisterViewL(r);
	CleanupStack::Pop(r);
	return r;
	}

/** Internal low level view creation.

Use if CCommsDatabase::OpenTableLC() or CCommsDatabase::OpenViewLC...() are not sufficient.

Note that OpenTableLC() and OpenViewLC...() have been withdrawn in CCommsDatabase
and are now implemented in CCommsDatabaseBase.

@param aDb Reference to the instance of the Comms Database to use.
@param aTableName Name of the table of view.
@param aIncludeHiddenRecords Value for Hidden attribute.
@return A reference to the newly created view. */
CCommsDbTableView* CCommsDbTableView::NewL(CCommsDatabaseBase& aDb, const TDesC& aTableName, const TDesC& aColumnToMatch, const TDesC16& aValueToMatch)
	{
	CCommsDbTableView* r = new(ELeave) CCommsDbTableView(aDb,aTableName);
	CleanupStack::PushL(r);
	r->ConstructL(aTableName, aColumnToMatch, aValueToMatch);
	// register view in Database
	aDb.iImpl->RegisterViewL(r);
	CleanupStack::Pop(r);
	return r;
	}

/** Internal low level view creation.

Use if CCommsDatabase::OpenTableLC() or CCommsDatabase::OpenViewLC...() are not sufficient.

Note that OpenTableLC() and OpenViewLC...() have been withdrawn in CCommsDatabase
and are now implemented in CCommsDatabaseBase.

@param aDb Reference to the instance of the Comms Database to use.
@param aTableName Name of the table of view.
@param aIncludeHiddenRecords Value for Hidden attribute.
@return A reference to the newly created view. */
CCommsDbTableView* CCommsDbTableView::NewL(CCommsDatabaseBase& aDb, const TDesC& aTableName, const TDesC& aColumnToMatch, TUint32 aValueToMatch)
	{
	CCommsDbTableView* r = new(ELeave) CCommsDbTableView(aDb,aTableName);
	CleanupStack::PushL(r);
	r->ConstructL(aTableName, aColumnToMatch, aValueToMatch);
	// register view in Database
	aDb.iImpl->RegisterViewL(r);
	CleanupStack::Pop(r);
	return r;
	}

/** Internal low level view creation.

Use if CCommsDatabase::OpenTableLC() or CCommsDatabase::OpenViewLC...() are not sufficient.

Note that OpenTableLC() and OpenViewLC...() have been withdrawn in CCommsDatabase
and are now implemented in CCommsDatabaseBase.

@param aDb Reference to the instance of the Comms Database to use.
@param aTableName Name of the table of view.
@param aIncludeHiddenRecords Value for Hidden attribute.
@return A reference to the newly created view. */
CCommsDbTableView*  CCommsDbTableView::NewLC(CCommsDatabaseBase& aDb, const TDesC& aTableName, const TDesC& aColumnToMatch, TBool aValueToMatch)
	{
	CCommsDbTableView* r = new(ELeave) CCommsDbTableView(aDb,aTableName);
	CleanupStack::PushL(r);
	r->ConstructL(aTableName, aColumnToMatch, aValueToMatch);
	// register view in Database
	aDb.iImpl->RegisterViewL(r);
	return r;
	}
// this two are for IAP table only so table name is not needed
/** Internal low level view creation.

Use if CCommsDatabase::OpenTableLC() or CCommsDatabase::OpenViewLC...() are not sufficient.

Note that OpenTableLC() and OpenViewLC...() have been withdrawn in CCommsDatabase
and are now implemented in CCommsDatabaseBase.

@param aDb Reference to the instance of the Comms Database to use.
@param aTableName Name of the table of view.
@param aIncludeHiddenRecords Value for Hidden attribute.
@return A reference to the newly created view. */
CCommsDbTableView* CCommsDbTableView::NewLC(CCommsDatabaseBase& aDb, TUint32 aBearerSet,	TCommDbConnectionDirection aDirection)
	{
	CCommsDbTableView* r = new(ELeave) CCommsDbTableView(aDb,TPtrC(KCDTypeNameIAP));
	CleanupStack::PushL(r);
	r->ConstructL(aBearerSet, aDirection);
	// register view in Database
	aDb.iImpl->RegisterViewL(r);
	return r;
	}
/** Internal low level view creation.

Use if CCommsDatabase::OpenTableLC() or CCommsDatabase::OpenViewLC...() are not sufficient.

Note that OpenTableLC() and OpenViewLC...() have been withdrawn in CCommsDatabase
and are now implemented in CCommsDatabaseBase.

@param aDb Reference to the instance of the Comms Database to use.
@param aTableName Name of the table of view.
@param aIncludeHiddenRecords Value for Hidden attribute.
@return A reference to the newly created view. */
CCommsDbTableView* CCommsDbTableView::NewLC(CCommsDatabaseBase& aDb, TUint32 aNetworkId)
	{
	CCommsDbTableView* r = new(ELeave) CCommsDbTableView(aDb,TPtrC(KCDTypeNameIAP));
	CleanupStack::PushL(r);
	r->ConstructL(aNetworkId);
	// register view in Database
	aDb.iImpl->RegisterViewL(r);
	return r;
	}
//  this one is for Proxie table only
/** Internal low level view creation.

Use if CCommsDatabase::OpenTableLC() or CCommsDatabase::OpenViewLC...() are not sufficient.

Note that OpenTableLC() and OpenViewLC...() have been withdrawn in CCommsDatabase
and are now implemented in CCommsDatabaseBase.

@param aDb Reference to the instance of the Comms Database to use.
@param aTableName Name of the table of view.
@param aIncludeHiddenRecords Value for Hidden attribute.
@return A reference to the newly created view. */
CCommsDbTableView* CCommsDbTableView::NewLC(CCommsDatabaseBase& aDb, TUint32 aServiceId, const TDesC& aServiceType)
	{
	CCommsDbTableView* r = new(ELeave) CCommsDbTableView(aDb,TPtrC(KCDTypeNameProxies));
	CleanupStack::PushL(r);
	r->ConstructL(aServiceId, aServiceType);
	// register view in Database
	aDb.iImpl->RegisterViewL(r);
	return r;
	}

CCommsDbTableView::CCommsDbTableView(CCommsDatabaseBase& aDb,const TDesC& aTableName)
	: iDb(aDb), iTableName(aTableName)
/** Internal constructor.

@param aDb Reference to the instance of the Comms Database.
@param aTableName The name of the table to view. */
	{
	}

void CCommsDbTableView::ConstructL(const TDbQuery& /*aQuery*/,TBool /*aUseTemplate*/)
/**
Prepares and evaluates the table view using the SQL query aQuery

@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@param aQuery A reference to TDbQuery which contains the query to perform
@param aUseTemplate A boolean value whether to use template while creating the object or not
*/
	{
	User::Leave(KErrNotSupported);
	}

void CCommsDbTableView::ConstructL(const TDesC& aTableName, TBool aHidden, TBool aUseTemplate)
/**
Prepares and evaluates the table view

@param aTableName
@param aUseTemplate A boolean value whether to use template while creating the object or not
*/
	{
	if (iTableName.CompareF(TPtrC(KCDTypeNameConnectionPrefs)) == 0)
		{
		User::Leave(KErrAccessDenied);          // must use proper connection prefs class
		}

	if (iTableName.CompareF(TPtrC(KCDTypeNameGlobalSettings)) == 0)
		{
		User::Leave(KErrNotFound);          // must use proper connection prefs class
		}

	if (iTableName.Length() == 0)
		{
		User::Leave(KErrArgument);
		}

	if(aHidden)
		{
		iDb.iImpl->iDbs->SetAttributeMask(ECDHidden);
		}

	iTableExt = CCommDbTableExtension::NewL();

	// create container and load data
	iTableExt->SetRecordSetL(aTableName, *iDb.iImpl->iDbs, KDoNotPreparePriming, KLoadTable);

	if(TableWithTemplateRecord() && aUseTemplate)
		{
		iTemplate = CCommsDbTemplateRecord::NewL(&iDb,iTableName);
		}
	else
		{
		iTemplate = NULL;
		}

	if(aHidden)
		{
		iDb.iImpl->iDbs->ClearAttributeMask(ECDHidden);
		}

	iState=EOpenIdle;
	}

void CCommsDbTableView::ConstructL(const TDesC& aTableName, const TDesC& aColumnToMatch, const TDesC8& aValueToMatch)
/**

@param aTableName
@param aColumnToMatch
@param aValueToMatch
@param includeHiddenRecords
*/
	{
	CMDBRecordSetBase* rs(NULL);
	TInt type(0);

	iTableExt = CCommDbTableExtension::NewL();

	iTableExt->SetRecordSetL(aTableName, *iDb.iImpl->iDbs, KPreparePriming, KDoNotLoadTable);
	rs = iTableExt->GetRecordSetL();

	CMDBElement * dbElement = iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumnToMatch, type);

	//-- check if we try to write TDesC8 string to the field of type TDes16
	//-- if yes, we have to allow this by converting the type of the string to TDes16.
	//-- just to avoid Behaviour Break. Nasty.
	if(type == EText || type == EMedText || type == ELongText)
		{//-- the actual field type is TDes16
		TInt valLen = aValueToMatch.Length();
		RBuf16 convBuf;
		convBuf.CreateL(valLen);
		CleanupClosePushL(convBuf);
		convBuf.Copy(aValueToMatch); //--  convert the string to the 16bit representation

		CMDBField<TDesC>*   fieldDes16 = static_cast<CMDBField<TDesC>*>(dbElement);
		fieldDes16->SetMaxLengthL(convBuf.Length());
		*fieldDes16 = convBuf;

		CleanupStack::PopAndDestroy(&convBuf);
		}
	else
		{
		CMDBField<TDesC8>* fieldDes8 = static_cast<CMDBField<TDesC8>*>(dbElement);
		fieldDes8->SetMaxLengthL(aValueToMatch.Length());
		*fieldDes8 = aValueToMatch;
		}

	TBool searchResult = rs->FindL(*iDb.iImpl->iDbs);
	if(!searchResult)
		{
		// delete primed record and leave empty recordset
		iTableExt->ResetAndDestroyL();

		__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("Table: %S with Column Name: %S, Hidden:%b is empty"),&aTableName,&aColumnToMatch,iDb.iShowHiddenRecords);
		}
	else
		{
		//-- aValueToMatch isn't logged here, because mixing _L and TDes8 kills the system
		__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("Table: %S with Column Name: %S, Hidden:%b ..."), &aTableName,&aColumnToMatch,iDb.iShowHiddenRecords);
		__FLOG_STATIC1(KLogComponent, KCommDbShim, _L8("Table: ... ValueToMatch: %S"), &aValueToMatch);
		}

	TableWithTemplateRecord() ? iTemplate = CCommsDbTemplateRecord::NewL(&iDb,iTableName) : iTemplate = NULL;

	iState=EOpenIdle;
	}

void CCommsDbTableView::ConstructL(const TDesC& aTableName, const TDesC& aColumnToMatch, const TDesC16& aValueToMatch)
/**

@param aTableName
@param aColumnToMatch
@param aValueToMatch
@param includeHiddenRecords
*/
	{
	CMDBRecordSetBase* rs(NULL);
	TInt type(0);

	iTableExt = CCommDbTableExtension::NewL();

	iTableExt->SetRecordSetL(aTableName, *iDb.iImpl->iDbs, KPreparePriming, KDoNotLoadTable);
	rs = iTableExt->GetRecordSetL();

	CMDBElement * dbElement = iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumnToMatch, type);

	//-- check if we try to write TDesC16 string to the field of type TDes8
	//-- if yes, we have to allow this by converting the type of the string to TDes8.
	//-- just to avoid Behaviour Break. Nasty.
	if(type == EDesC8)
		{
		TInt valLen = aValueToMatch.Length();
		RBuf8 convBuf;
		convBuf.CreateL(valLen);
		CleanupClosePushL(convBuf);
		convBuf.Copy(aValueToMatch); //--  convert the string to the 8bit representation

		CMDBField<TDesC8>* fieldDes8 = static_cast<CMDBField<TDesC8>*>(dbElement);
		fieldDes8->SetMaxLengthL(aValueToMatch.Length());
		*fieldDes8 = convBuf;

		CleanupStack::PopAndDestroy(&convBuf);
		}
	else
		{//-- types match OK.
		CMDBField<TDesC>*   fieldDes16 = static_cast<CMDBField<TDesC>*>(dbElement);
		fieldDes16->SetMaxLengthL(aValueToMatch.Length());
		*fieldDes16 = aValueToMatch;
		}

	TBool searchResult = rs->FindL(*iDb.iImpl->iDbs);

	if(!searchResult)
		{
		// delete primed record and leave empty recordset
		iTableExt->ResetAndDestroyL();

		__FLOG_STATIC4(KLogComponent, KCommDbShim, _L("Table: %S with Column Name: %S, Value:%S, Hidden:%b is empty"),
						&aTableName,&aColumnToMatch,&aValueToMatch,iDb.iShowHiddenRecords);
		}
	else
		{
		__FLOG_STATIC4(KLogComponent, KCommDbShim, _L("Table: %S with Column Name: %S, Value:%S, Hidden:%b"),
					&aTableName,&aColumnToMatch,&aValueToMatch,iDb.iShowHiddenRecords);
		}

	TableWithTemplateRecord() ? iTemplate = CCommsDbTemplateRecord::NewL(&iDb,iTableName) : iTemplate = NULL;

	iState=EOpenIdle;
	}

void CCommsDbTableView::ConstructL(const TDesC& aTableName, const TDesC& aColumnToMatch, TUint32 aValueToMatch)
/**

@param aTableName
@param aColumnToMatch
@param aValueToMatch
@param includeHiddenRecords
*/
	{
	CMDBRecordSetBase* rs(NULL);
	TInt type(0);

	iTableExt = CCommDbTableExtension::NewL();

	iTableExt->SetRecordSetL(aTableName, *iDb.iImpl->iDbs, KPreparePriming, KDoNotLoadTable);
	rs = iTableExt->GetRecordSetL();

	TBool searchResult(EFalse);
	if(aColumnToMatch.CompareF(TPtrC(COMMDB_ID)) == 0)
		{
		rs->iRecords[0]->SetRecordId(aValueToMatch);
		TRAPD(err, rs->iRecords[0]->LoadL(*iDb.iImpl->iDbs));
		if(err == KErrNone)
			{
			searchResult = ETrue;
			}
		else if(err == KErrNotFound)	// returning an empty record set is DBMS-compatible
			{
			searchResult = EFalse;
			}
		else
			{
			User::Leave(err);
			}
		}
	else
		{
		CMDBField<TUint32>* field = static_cast<CMDBField<TUint32>*>(iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumnToMatch, type));
		*field = aValueToMatch;
		searchResult = rs->FindL(*iDb.iImpl->iDbs);
		}



	if(!searchResult)
		{
		// delete primed record and leave empty recordset
		iTableExt->ResetAndDestroyL();

		__FLOG_STATIC4(KLogComponent, KCommDbShim, _L("Table: %S with Column Name: %S, Value:%d, Hidden:%b is empty"),&aTableName,&aColumnToMatch,aValueToMatch,iDb.iShowHiddenRecords);
		}
	else
		{
		__FLOG_STATIC4(KLogComponent, KCommDbShim, _L("Table: %S with Column Name: %S, Value:%d, Hidden:%b"),
					&aTableName,&aColumnToMatch,aValueToMatch,iDb.iShowHiddenRecords);
		}

	TableWithTemplateRecord() ? iTemplate = CCommsDbTemplateRecord::NewL(&iDb,iTableName) : iTemplate = NULL;

	iState=EOpenIdle;
	}

void CCommsDbTableView::ConstructL(const TDesC& aTableName, const TDesC& aColumnToMatch, TBool aValueToMatch)
/**

@param aTableName
@param aColumnToMatch
@param aValueToMatch
@param includeHiddenRecords
*/
	{
	CMDBRecordSetBase* rs(NULL);
	TInt type(0);

	iTableExt = CCommDbTableExtension::NewL();

	iTableExt->SetRecordSetL(aTableName, *iDb.iImpl->iDbs, KPreparePriming, KDoNotLoadTable);
	rs = iTableExt->GetRecordSetL();

	CMDBField<TBool>* field = static_cast<CMDBField<TBool>*>(iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumnToMatch, type));
	*field = aValueToMatch;

	TBool searchResult = rs->FindL(*iDb.iImpl->iDbs);

	if(!searchResult)
		{
		// delete primed record and leave empty recordset
		iTableExt->ResetAndDestroyL();
		__FLOG_STATIC4(KLogComponent, KCommDbShim, _L("Table: %S with Column Name: %S, Value:%b, Hidden:%b is empty"),
						&aTableName,&aColumnToMatch,aValueToMatch,iDb.iShowHiddenRecords);
		}
	else
		{
		__FLOG_STATIC4(KLogComponent, KCommDbShim, _L("Table: %S with Column Name: %S, Value:%b, Hidden:%b"),
					&aTableName,&aColumnToMatch,aValueToMatch,iDb.iShowHiddenRecords);
		}

	TableWithTemplateRecord() ? iTemplate = CCommsDbTemplateRecord::NewL(&iDb,iTableName) : iTemplate = NULL;

	iState=EOpenIdle;
	}

void CCommsDbTableView::ConstructL(TUint32 aBearerSet,	TCommDbConnectionDirection aDirection)
/**

*/
	{
	iTableExt = CCommDbTableExtension::NewL();

	CMDBRecordSet<CCDIAPRecord>* iapRS = new(ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRS);
	CMDBRecordSet<CCDIAPRecord>* searchRS = new(ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
	CleanupStack::PushL(searchRS);

	//  Note if requesting all types this does not produce the best result
	//  i.e. no where expression. This could be optimised by pre testing for something like
	//  aBearerSet == KCommDbBearerAny
	TInt searchResult;
	CCDIAPRecord* iapRecord = NULL;
	// Add all the records into recordset matching criteria
	for (int i = 0; i < KNumServiceType; i++)
		{
		if ((aBearerSet & KMap[i].bearerMask) != 0 &&
			(aDirection == KMap[i].direction ||
			 aDirection == ECommDbConnectionDirectionUnknown ))
			{
			const TPtrC serviceType(KMap[i].serviceType);

			// create new record
			iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
			CleanupStack::PushL(iapRecord);

			// prime the service type and bearerset
			iapRecord->iServiceType.SetMaxLengthL(serviceType.Length());
			iapRecord->iServiceType = serviceType;
			//iapRecord->iBearer = aBearerSet;

			// Add record to recordset container only if there is anything to add
			searchRS->iRecords.AppendL(iapRecord);

			CleanupStack::Pop(iapRecord);

			// Load record
			searchResult = searchRS->FindL(*iDb.iImpl->iDbs);

			if(searchResult)
				{
				// if records are found copy them to iapRS
				for(TInt i=0;i<searchRS->iRecords.Count();++i)
					{
					iapRS->iRecords.AppendL(searchRS->iRecords[i]);
					}
				// only clear array
				searchRS->iRecords.Reset();
				}
			else
				{
				// remove and destroy primed record
				searchRS->iRecords.ResetAndDestroy();
				}
			}
		}

	CleanupStack::PopAndDestroy(searchRS); 	// searchRS
	CleanupStack::Pop(iapRS); 		// iapRS

	// set RSContainer with preloaded iap record set
	iTableExt->SetRecordSet(iapRS);
	// sort data in recordset by recordId
	iTableExt->SortById();

	iState=EOpenIdle;

	__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Modem Bearer Table constructed with BearerSet: %d and Direction:%d"),aBearerSet, aDirection);
	}

void CCommsDbTableView::ConstructL(TUint32 aNetworkId)
/**

*/
	{
	iTableExt = CCommDbTableExtension::NewL();

	CMDBRecordSet<CCDIAPRecord>* iapRS = new(ELeave) CMDBRecordSet<CCDIAPRecord>(KCDTIdIAPRecord);
	CleanupStack::PushL(iapRS);

	// create new record
	CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));

	// Prime record
	iapRecord->iNetwork = aNetworkId;
	// Append primed record to recordset
	TInt err = iapRS->iRecords.Append(iapRecord);
	if(err != KErrNone)
		{
		delete iapRecord;
		User::Leave(err);
		}

	TBool searchResult = iapRS->FindL(*iDb.iImpl->iDbs);
	if(!searchResult)
		{
		// delete primed record and leave empty recordset
		iTableExt->ResetAndDestroyL();

		__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("IAP Table constructed with Network Id: %d is empty"),aNetworkId);
		}
	else
		{
		__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("IAP Table constructed with Network Id: %d"),aNetworkId);
		}

	iTableExt->SetRecordSet(iapRS);

	CleanupStack::Pop(iapRS);

	iState=EOpenIdle;
	}

void CCommsDbTableView::ConstructL(TUint32 aServiceId, const TDesC& aServiceType)
/**

*/
	{
	iTableExt = CCommDbTableExtension::NewL();

	CMDBRecordSet<CCDProxiesRecord>* proxieRS = new(ELeave) CMDBRecordSet<CCDProxiesRecord>(KCDTIdProxiesRecord);
	CleanupStack::PushL(proxieRS);

	// create new record
	CCDProxiesRecord* proxiesRecord = static_cast<CCDProxiesRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdProxiesRecord));
	CleanupStack::PushL(proxiesRecord);

	// Prime record
	proxiesRecord->iService = aServiceId;
	proxiesRecord->iServiceType.SetMaxLengthL(aServiceType.Length());
	proxiesRecord->iServiceType = aServiceType;
	// Append primed record to recordset
	proxieRS->iRecords.AppendL(proxiesRecord);
	CleanupStack::Pop(proxiesRecord);

	TBool searchResult = proxieRS->FindL(*iDb.iImpl->iDbs);
	if(!searchResult)
		{
		// delete primed record and leave empty recordset
		proxieRS->iRecords.ResetAndDestroy();

		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Proxy Table constructed with ServiceID: %d, ServiceType:%S is empty"), aServiceId, &aServiceType);
		}
	else
		{
		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Proxy Table constructed with ServiceID: %d, ServiceType:%S"),aServiceId,&aServiceType);
		}

	iTableExt->SetRecordSet(proxieRS);

	CleanupStack::Pop(proxieRS); // proxieRS

	iState=EOpenIdle;
	}

void CCommsDbTableView::ConstructL(const TDesC& aTableName)
/**

Used only for template record creation

@param
*/
	{
	iTableExt = CCommDbTableExtension::NewL();

	// create container and load data
	iTableExt->SetRecordSetL(aTableName, *iDb.iImpl->iDbs, KPreparePriming, KDoNotLoadTable, KTemplateCall);

	CMDBRecordBase* currRecord = iTableExt->GetCurrentRecordL();

	// enable for view hidden records
	iDb.iImpl->iDbs->SetAttributeMask(ECDHidden);

	// set template record id to search for
	currRecord->SetRecordId(KCDDefaultRecord);

    // find template record
	TRAPD(err, currRecord->LoadL(*iDb.iImpl->iDbs));

	// disable for view hidden records if not required globally
	if (! iDb.iShowHiddenRecords )
        {
        iDb.iImpl->iDbs->ClearAttributeMask(ECDHidden);
        }

	if(err != KErrNone)
		{
		// if template record is not found remove primed record
		iTableExt->ResetAndDestroyL();
		}

	iState=EOpenIdle;
	}

EXPORT_C CCommsDbTableView::~CCommsDbTableView()
/** Frees all resources owned by this object, prior to its destruction. Specifically,
it closes the view. */
	{
	iDb.iImpl->UnRegisterView(this);
	Close();

	delete iTableExt;
	delete iTemplate;
	}

EXPORT_C TInt CCommsDbTableView::InsertRecord(TUint32& aId)

/** Prepares database to insert a new record and returns a unique ID for the
record to be added.

PutRecordChanges() must be called to add the new entry.

This function sets the CCommsDbTableView onto the new record.

Once all changes to the new record are complete, a call must be made to either
PutRecordChanges() or CancelRecordChanges() as appropriate.

This function raises a CommsDbServer 1 panic if a previous call to InsertRecord()
or UpdateRecord() has already been made.

This function must be called before any attempt is made to write to a new
column, otherwise subsequent write operations raise a CommsDbServer 12 panic.

Once this function has completed successfully, no attempt can be made to read
from a column until either PutRecordChanges() or CancelRecordChanges() has
been called to complete the record insertion operation, otherwise the read
operations raise a CommsDbServer 10 panic.

@param aId On successful return, contains the unique Id associated with the new
record.
@return KErrNone if successful, otherwise another of the system-wide error
codes. */
	{
	// Create a new record of TableName type
	TInt err = DoInsertRecord(KCreateNewRecord, aId, KDoNotUseTemplate);

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("InsertRecord -> TableName=%S, Id=%08x, Err%d"), &iTableName, aId, err);

	return err;
	}

EXPORT_C TInt CCommsDbTableView::InsertCopyRecord(TUint32& aId)
/**
Inserts a row into the the table and writes the {incremented) id to aId
Filling all empty fields with values copied from record currently pointed at.
The record number is returned.

@param aId On return contains the unique Id associated with this new record.
@return KErrNone if successful, otherwise another of the system-wide error codes.
@capability Dependent on table, see the guide page referenced below.
*/
	{
	// Create a new record of TableName type
	TInt err = DoInsertRecord(KCreateCopyRecord, aId, KDoNotUseTemplate);

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("InsertCopyRecord -> TableName=%S, Id=%08x, Err%d"), &iTableName, aId, err);

	return err;
	}

TInt CCommsDbTableView::InsertTemplateRecord()
/**
Inserts a row into the the table as template record

*/
	{
	TUint32 aId;
	TInt err = DoInsertRecord(KCreateNewRecord, aId, KUseTemplate);

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("InsertTemplateRecord -> TableName=%S, Id=%08x, Err%d"), &iTableName, aId, err);

	return err;
	}

/* TODO: Remove this function if template thing is not working */
TInt CCommsDbTableView::DoInsertRecord(TBool aCopy, TUint32& aId, TBool aIsTemplate)
/**

@param aCopyRecord - ETrue if inserted record is copied from some other record
@param aTemlateRecord - If it is default (NULL) then current record is base for new
*/
	{
	__ASSERT_ALWAYS(iState==EOpenIdle, CommsDbPanic(ECommsDbInsertNotOpenIdle));

	CMDBRecordSetBase* recordSet(NULL);
	TInt err = iTableExt->GetRecordSet(recordSet);

	// check for read only status
	if (err == KErrNone)
		{
    	if ( recordSet->IsSetAttribute(ECDNoWriteButDelete) ||
         	 recordSet->IsSetAttribute(ECDProtectedWrite) )
	        {
	        return KErrAccessDenied;
	        }
		}

	TRAP(err, iTableExt->InsertRecordL(aCopy, aId, *iDb.iImpl->iDbs, aIsTemplate));
	if (err == KErrNone)
		{
		iState = EInsert;
		}

	return err;
	}

EXPORT_C TInt CCommsDbTableView::UpdateRecord()
/** Prepares to update the current record PutRecordChanges() must be called before
editing any existing column.

Once all changes to the new record are complete, a call must be made to either
PutRecordChanges() or CancelRecordChanges() as appropriate.

This function raises a CommsDbServer 2 panic if a previous call to InsertRecord()
or UpdateRecord() has already been made.

This function must be called before any attempt is made before editing any
existing column otherwise subsequent write operations raise a CommsDbServer
12 panic.

Once this function has completed successfully, no attempt can be made to read
from a column until either PutRecordChanges() or CancelRecordChanges() has
been called to complete the record insertion operation, otherwise the read
operations raise a CommsDbServer 10 panic.

@return KErrNone if successful; KErrAccessDenied if the record is read only;
otherwise another of the system wide error codes. */
	{
	__ASSERT_ALWAYS(iState==EOpenIdle, CommsDbPanic(ECommsDbUpdateNotOpenIdle));

	CMDBRecordBase* record(NULL);
	TInt err = iTableExt->GetCurrentRecord(record);

	// check for read only status
	if (err == KErrNone)
		{
    	if ( record->IsSetAttribute(ECDNoWriteButDelete) ||
         	 record->IsSetAttribute(ECDProtectedWrite) )
	        {
	        err = KErrAccessDenied;
	        }
	    else
	    	{
			iState = EUpdate;
	    	}
		}

	return err;
	}

EXPORT_C TInt CCommsDbTableView::DeleteRecord()
/** Deletes the record in the table at the current cursor position.

This function raises a CommsDbServer 3 panic if a previous call to InsertRecord()
or UpdateRecord() has been made.

@return KErrNone if successful, otherwise another of the system-wide error
codes.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS(iState == EOpenIdle, CommsDbPanic(ECommsDbDeleteNotOpenIdle));

	CMDBRecordBase* record(NULL);
	TInt err = iTableExt->GetCurrentRecord(record);

	// check for read only status
	if (err == KErrNone)
		{
    	if ( record->IsSetAttribute(ECDNoWriteButDelete) ||
         	 record->IsSetAttribute(ECDProtectedWrite) )
	        {
	        return KErrAccessDenied;
	        }
		}
	else
		{
		// return KErrNotFound
		return err;
		}

	// delete current row from container
	TRAP(err, iTableExt->DeleteCurrentRecordL(*iDb.iImpl->iDbs));
	if (err != KErrNone)
		{
		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("DeleteRecord -> TableName=%S returned Err:%d"), &iTableName, err);
		return err;
		}

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("DeleteRecord -> TableName=%S"), &iTableName);

	return err;
	}

// Put the inserted or updated row of the table.  Set hidden column to aHidden (defaults
// to false i.e. unhidden) Set the flag to say a successful put has occurred so that we know
// to compact the database on closing. Notify that a record has been created or updated
// If the column is MODEM_TSY_NAME in a modem table notify again.


EXPORT_C TInt CCommsDbTableView::PutRecordChanges(TBool aHidden, TBool aReadOnly)
//
// Put the inserted or updated row of the table.  Set hidden column to aHidden (defaults
// to false i.e. unhidden) Set the flag to say a successful put has occurred so that we know
// to compact the database on closing. Notify that a record has been created or updated
// If the column is MODEM_TSY_NAME in a modem table notify again.
//
/** Confirms changes made to a record, which were started by calls to one of InsertRecord()
or UpdateRecord().

The record can be marked as hidden by passing ETrue as a parameter. By default,
records are not hidden.

A call to one of either InsertRecord() or UpdateRecord() must have previously
been made otherwise the function raises a CommsDbServer 4 panic.

@param aHidden Defines whether or not the record is to be marked as hidden.
A true value means that the record is to be hidden. If not explicitly specified,
EFalse is taken as default.
@param aReadOnly Defines whether or not the record is to be stored as read-only.
Read-only storage prevents the record from being altered at a later time.
@return KErrNone if successful, otherwise another of the system-wide error
codes.
@capability Dependent on table, see the guide page referenced below. */
	{
	return DoPutRecordChanges(aHidden, aReadOnly);
	}

TInt CCommsDbTableView::DoPutRecordChanges(TBool aHidden, TBool aReadOnly, TBool aIsTemplateCall)
//
// Put the inserted or updated row of the table.  Set hidden column to aHidden (defaults
// to false i.e. unhidden) Set the flag to say a successful put has occurred so that we know
// to compact the database on closing. Notify that a record has been created or updated
// If the column is MODEM_TSY_NAME in a modem table notify again.
//
/** Confirms changes made to a record, which were started by calls to one of InsertRecord()
or UpdateRecord().

The record can be marked as hidden by passing ETrue as a parameter. By default,
records are not hidden.

A call to one of either InsertRecord() or UpdateRecord() must have previously
been made otherwise the function raises a CommsDbServer 4 panic.

@param aHidden Defines whether or not the record is to be marked as hidden.
A true value means that the record is to be hidden. If not explicitly specified,
EFalse is taken as default.
@param aReadOnly Defines whether or not the record is to be stored as read-only.
Read-only storage prevents the record from being altered at a later time.
@return KErrNone if successful, otherwise another of the system-wide error
codes.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS((iState == EInsert || iState == EUpdate), CommsDbPanic(ECommsDbSvrPanicCommitNotInsertUpdate));

	CMDBRecordBase* record(NULL);
	CMDBSession* dbSession = iDb.iImpl->iDbs;

	TInt err = iTableExt->GetCurrentRecord(record);

	if(err != KErrNone)
		{
		__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("PutRecordChanges -> TableName=%S, Hidden=%b, returned Err:%d"),
				&iTableName, aHidden, err);
		return err;
		}

    if ( record->IsSetAttribute(ECDNoWriteButDelete) ||
         record->IsSetAttribute(ECDProtectedWrite) )
        {
        return KErrAccessDenied;
        }

	TBool tempHiddenMask(EFalse);
	TBool hiddenChanged = (record->Attributes() & ECDHidden) != aHidden;

    if (record->RecordId() == KCDDefaultRecord  ||  hiddenChanged)
        {
        if (! dbSession->IsSetAttributeMask(ECDHidden))
            {
            dbSession->SetAttributeMask(ECDHidden);
            tempHiddenMask=ETrue;
            }

		if( hiddenChanged )
			{
			if(aHidden)
				{
				// set up attributes against current record
				record->SetAttributes(ECDHidden);
				}
			else
				{
				// set up attributes against current record
				record->ClearAttributes(ECDHidden);
				}
			}
        }

	TBool tempReadOnlyMask(EFalse);
	TBool readOnlyChanged = (record->Attributes() & ECDNoWriteButDelete) != aReadOnly;

	if( readOnlyChanged )
        {
 		// save original mask value
		if (! dbSession->IsSetAttributeMask(ECDNoWriteButDelete))
			{
			dbSession->SetAttributeMask(ECDNoWriteButDelete);
			tempReadOnlyMask = ETrue;
			}

		if(aReadOnly)
			{
			// set up attributes against current record
			record->SetAttributes(ECDNoWriteButDelete);
			}
		else
			{
			// set up attributes against current record
			record->ClearAttributes(ECDNoWriteButDelete);
			}
		}

	// store changes to commsdat
	TRAP(err, record->ModifyL(*dbSession));

	// restore back original mask values
	if(tempHiddenMask)
		{
		dbSession->ClearAttributeMask(ECDHidden);
		}

    if(aReadOnly && tempReadOnlyMask)
		{
		dbSession->ClearAttributeMask(ECDNoWriteButDelete);
		}

	// check returned error code.
	if(err != KErrNone)
		{
		__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("PutRecordChanges -> TableName=%S, Hidden=%b, returned Err:%d"),
				&iTableName, aHidden, err);

		return err;
		}

	// delete record from container if record is hidden, not template call and not allowed to see
	if(aHidden && !aIsTemplateCall && !iDb.iShowHiddenRecords)
		{
    	TRAP(err, iTableExt->DeleteCurrentRecordL(*dbSession, KDeleteFromContainerOnly));
		// check returned error code.
		if(err != KErrNone)
			{
			__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("PutRecordChanges -> TableName=%S, Hidden=%b, returned Err:%d"),
					&iTableName, aHidden, err);

			return err;
			}
		}

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("PutRecordChanges -> TableName=%S, Hidden=%b, returned Err:%d"),
			&iTableName, aHidden, err);

	// clear pointer for inserted record
	iTableExt->ResetArrays(*dbSession);
	iState = EOpenIdle;

	return err;
	}

EXPORT_C void CCommsDbTableView::CancelRecordChanges()
/** Abandons changes made to a record which were started by calls to either:InsertRecord()
or UpdateRecord().

A call to one of either InsertRecord() or UpdateRecord() must have previously
been made otherwise the function raises a CommsDbServer 5 panic.
 */
	{
	__ASSERT_ALWAYS(iState != EClosed, CommsDbPanic(ECommsDbCancelNotInsertUpdate));

	if (iState == EInsert || iState == EUpdate)		// don't do anything if it's already idle
		{
		TInt err = iTableExt->CancelRecordChanges(*iDb.iImpl->iDbs);
		if(err != KErrNone)
			{
			CommsDbPanic(ECommsDbCancelNotInsertUpdate);
			}
		iState = EOpenIdle;
		}

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("CancelRecordChanges -> TableName=%S"), &iTableName);
	}

EXPORT_C TInt CCommsDbTableView::GotoFirstRecord()
/** Sets the first record in the view as the current record; i.e. set the view's
cursor to point to the first record.

The CommsDbServer 6 panic is also raised if this function is called after
the view has been closed.

This function must not be called if any of the operations involved in inserting
or updating a record are still outstanding, otherwise the function raises
a CommsDbServer 6 panic. For example, the panic is raised if this function
is called between calls to UpdateRecord() and PutRecordChanges().

@return KErrNone if the record exists in the view and navigation is successful,
KErrNotFound if the view is empty and has no records, otherwise another of
the system-wide error codes.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS(iState==EOpenIdle, CommsDbPanic(ECommsDbGotoFirstNotOpenIdle));

	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L("GotoFirstRecord -> TableName=%S"), &iTableName);
	return iTableExt->GotoFirstRecord(*iDb.iImpl->iDbs);
	}

EXPORT_C TInt CCommsDbTableView::GotoNextRecord()
/** Sets the next record in the view as the current record; i.e. set the view's
cursor to point to the next record.

This function must not be called if any of the operations involved in inserting
or updating a record are still outstanding, otherwise the function raises
a CommsDbServer 7 panic. For example, the panic is raised if this function
is called between calls to UpdateRecord() and PutRecordChanges().

The CommsDbServer 7 panic is also raised if this function is called after
the view has been closed.

@return KErrNone if the record exists in the view and navigation is successful,
KErrNotFound if the current record is already the last in the view and there
is no 'next' record, otherwise another of the system-wide error codes.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS(iState==EOpenIdle, CommsDbPanic(ECommsDbGotoNextNotOpenIdle));

	#ifdef _DEBUG
		TInt index(0);
		TInt err = iTableExt->GetRecordSetIndex(index);

		if(err == KErrNone)
			{
			__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("GotoNextRecord -> TableName=%S, RecordNum=%d"), &iTableName, index);
			}
	#endif

	return iTableExt->GotoNextRecord(*iDb.iImpl->iDbs);
	}

EXPORT_C TInt CCommsDbTableView::GotoPreviousRecord()
/** Sets the previous record in the view as the current record; i.e. set the view's
cursor to point to the previous record.

This function must not be called if any of the operations involved in inserting
or updating a record are still outstanding, otherwise the function raises
a CommsDbServer 8 panic. For example, the panic is raised if this function
is called between calls to UpdateRecord() and PutRecordChanges().

The CommsDbServer 8 panic is also raised if this function is called after
the view has been closed.

@return KErrNone if the record exists in the view and navigation is successful,
KErrNotFound if the current record is already the first in the view and there
is no 'previous' record, otherwise another of the system-wide error codes.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS(iState==EOpenIdle, CommsDbPanic(ECommsDbGotoPreviousNotOpenIdle));

	#ifdef _DEBUG
		TInt index(0);
		TInt err = iTableExt->GetRecordSetIndex(index);

		if(err == KErrNone)
			{
			__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("GotoPreviousRecord -> TableName=%S, RecordNum=%d"), &iTableName, index);
			}
	#endif

	return iTableExt->GotoPreviousRecord(*iDb.iImpl->iDbs);
	}

EXPORT_C void CCommsDbTableView::ReadTextL(const TDesC& aColumn, TDes8& aValue)
/** Reads narrow (ASCII) text located in a specific column within the current record
and copies it into an 8 bit modifiable descriptor. The column is identified
by the name supplied in the descriptor aColumn.

If the column in the current record has a NULL value and the table has a template
record, then the text located in the corresponding column in that template
record is read and copied into aValue.

If the column in the current record has a NULL value and the table does not
have a template record, then the length of the descriptor aValue is set to
zero and the descriptor contains no text.

The maximum length of text expected by this function is the value of the constant
KCommsDbSvrMaxColumnNameLength. The maximum length of aValue supplied by the
caller can, therefore, be the same.

This read operation must not occur if any of the operations involved in inserting
or updating a record are still outstanding, otherwise the function raises
a CommsDbServer 10 panic. For example, the panic is raised if this function
is called between calls to UpdateRecord() and PutRecordChanges().

This panic is also raised if this function is called after the view has been
closed.

@note If the column to be read is encrypted then the function decrypts it.

@param aColumn A reference to a descriptor containing the name of the column
in the current record whose (narrow text) value is to be read.
@param aValue A reference to an 8 bit descriptor passed by the caller.
@leave KErrNotFound Specified column does not exist in the table.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS(iState==EOpenIdle, CommsDbPanic(ECommsDbReadNotOpenIdle));

	TInt type(0);
	CMDBElement* baseField = iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumn, type);

	aValue.Zero();

	// Check if value is NULL
	if (baseField->IsNull())
		{
		// look for template value
		if (iTemplate)
			{
			iTemplate->ReadTextL(aColumn, aValue);
			}
		}
	else
		{
		// check for type
		switch(type)
			{
			case EMedText:
			case EText:
					{
					CMDBField<TDesC>* field16 = static_cast<CMDBField<TDesC>*>(baseField);
					const TDesC& refField = *field16;
					aValue.Copy(refField);
					}
					break;
			case EDesC8:
					{
					CMDBField<TDesC8>* field = static_cast<CMDBField<TDesC8>*>(baseField);
					aValue = *field;
					}
					break;
			default:
				__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrNotFound) in ReadTextL -> TableName=%S, Column=%S, Value[8]"), &iTableName, &aColumn);
				User::Leave(KErrNotFound);
			}
		}

	__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("ReadTextL -> TableName=%S, Column=%S ..."), &iTableName, &aColumn);
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L8("ReadTextL -> ... Value[8]=%S"), &aValue);
	}

EXPORT_C void CCommsDbTableView::ReadTextL(const TDesC& aColumn, TDes16& aValue)
/** Reads wide (Unicode) text located in a specific column within the current record
and copies it into a 16 bit modifiable descriptor. The column is identified
by the name supplied in the descriptor aColumn.

If the column in the current record has a NULL value and the table has a template
record, then the text located in the corresponding column in that template
record is read and copied into aValue.

If the column in the current record has a NULL value and the table does not
have a template record, then the length of the descriptor aValue is set to
zero and the descriptor contains no text.

The maximum length of text expected by this function is the value of the constant
KCommsDbSvrMaxColumnNameLength. The maximum length of aValue supplied by the
caller can, therefore, be the same.

This read operation must not occur if any of the operations involved in inserting
or updating a record are still outstanding, otherwise the function raises
a CommsDbServer 10 panic. For example, the panic is raised if this function
is called between calls to UpdateRecord() and PutRecordChanges().

This panic is also raised if this function is called after the view has been
closed.

@note If the column to be read is encrypted then the function decrypts it.

@param aColumn A reference to a descriptor containing the name of the column
in the current record whose (wide text) value is to be read.
@param aValue A reference to a 16 bit descriptor passed by the caller.
@leave KErrNotFound Specified column does not exist in the table
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS(iState==EOpenIdle, CommsDbPanic(ECommsDbReadNotOpenIdle));

	TInt type(0);
	CMDBElement* baseField = iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumn, type);

	aValue.Zero();

	if (baseField->IsNull())
		{
		// look for template value
		if (iTemplate)
			{
			iTemplate->ReadTextL(aColumn, aValue);
			}
		}
	else
		{
		// check for type
		switch(type)
			{
			case EMedText:
			case EText:
					{
					CMDBField<TDesC>* field = static_cast<CMDBField<TDesC>*>(baseField);
					aValue = *field;
					}
					break;
			case EDesC8:
					{
					// des16 needs to be cast to des8
					CMDBField<TDesC8>* field8 = static_cast<CMDBField<TDesC8>*>(baseField);
					const TDesC8& refField = *field8;
					aValue.Copy(refField);
					}
					break;
			default:
				__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrNotFound) in ReadTextL -> TableName=%S, Column=%S, Value[16]"), &iTableName, &aColumn);
				User::Leave(KErrNotFound);
			}
		}

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("ReadTextL -> TableName=%S, Column=%S, Value[16]=%S"), &iTableName, &aColumn, &aValue);
	}

//Return the text in the column aColumn from the row at the current cursor position
// in aValue.  If this is null and this is the modem config table, then use the default.
EXPORT_C HBufC* CCommsDbTableView::ReadLongTextLC(const TDesC& aColumn)
/** Reads the long text located in a specific column within the current record
and copies this text to a heap descriptor. The heap descriptor is allocated
and its pointer returned by this function. The column is identified by the
name supplied in the descriptor aColumn.

If the column in the current record has a NULL value and the table has a template
record, then the long text located in the corresponding column in that template
record is read and copied.

If the column in the current record has a NULL value and the table does not
have a template record, then the length of the returned heap descriptor is
zero and the descriptor contains no text.

While the text in columns retrieved by the ReadTextL() functions is limited
in length, there is no restriction on the length of long text.

The HBufC* returned is left on the cleanup stack. The caller is responsible
for calling CleanupStack::PopAndDestroy().

This read operation must not occur if any of the operations involved in inserting
or updating a record are still outstanding, otherwise the function raises
a CommsDbServer 10 panic. For example, the panic is raised if this function
is called between calls to UpdateRecord() and PutRecordChanges().

This panic is also raised if this function is called after the view has been
closed.

@param aColumn A reference to a descriptor containing the name of a column
in the current record.
@leave KErrNotFound if the specified column does not exist in the table;
KErrNoMemory if out of memory.
@return Pointer to a heap descriptor containing the long text.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS(iState==EOpenIdle, CommsDbPanic(ECommsDbReadNotOpenIdle));

	TInt type(0);
	HBufC* buf(NULL);

	CMDBField<TDesC>* field = static_cast<CMDBField<TDesC>*>(iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumn, type));
	// check if value is null
	if (field->IsNull())
		{
		// look for template value
		if (iTemplate)
			{
			buf = iTemplate->ReadLongTextLC(aColumn);
			}
		else
			{
			// return empty buffer
			buf = HBufC::NewLC(0);
			}
		}
	else
		{
		const TDesC& ptrField = *field;
		buf = HBufC::NewMaxLC(ptrField.Length());
		buf->Des().Copy(ptrField);
		}

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("ReadLongTextLC -> TableName=%S, Column=%S, Value=%S"), &iTableName, &aColumn, &(*buf));

	return buf;
	}

EXPORT_C void CCommsDbTableView::ReadUintL(const TDesC& aColumn, TUint32& aValue)
/** Reads an unsigned integer value located in a specific column within the current
record and copies it into a reference to an unsigned integer. The column is
identified by the name supplied in the descriptor aColumn.

If the column in the current record has a NULL value and the table has a template
record, then the unsigned integer value located in the corresponding column
in that template record is read and copied into aValue.

If the column in the current record has a NULL value and the table does not
have a template record, then the function leaves.

This read operation must not occur if any of the operations involved in inserting
or updating a record are still outstanding, otherwise the function raises
a CommsDbServer 10 panic. For example, the panic is raised if this function
is called between calls to UpdateRecord() and PutRecordChanges().

This panic is also raised if this function is called after the view has been
closed.

@param aColumn A reference to a descriptor containing the name of the column
in the current record whose (unsigned integer) value is to be read.
@param aValue A reference to an unsigned integer passed by the caller.
@leave KErrUnknown if the specified column is null; KErrNotFound if the specified column
does not exist in the table.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS(iState==EOpenIdle, CommsDbPanic(ECommsDbReadNotOpenIdle));

	TInt type(0);


	if(aColumn.CompareF(TPtrC(COMMDB_ID)) == 0)
		{
		CMDBRecordBase* currRec = iTableExt->GetCurrentRecordL();
		aValue = currRec->RecordId();
		}
	else
		{
		CMDBField<TUint32>* field = static_cast<CMDBField<TUint32>*>(iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumn, type));
		// check if value is null
		if (field->IsNull())
			{
			// look for template value
			if (iTemplate)
				{
				iTemplate->ReadUintL(aColumn, aValue);
				}
			else
				{
				__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrUnknown) in ReadUintL -> TableName=%S, Column=%S"), &iTableName, &aColumn);
				// template value is not there
				User::Leave(KErrUnknown);
				}
			}
		else
			{
			aValue = *field;
			}
		}

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("ReadUintL -> TableName=%S, Column=%S, Value=%d"), &iTableName, &aColumn, aValue);
	}

EXPORT_C void CCommsDbTableView::ReadBoolL(const TDesC& aColumn, TBool& aValue)
/** Reads a boolean value located in a specific column within the current record
and copies it into a specified value. The column is identified by the name
supplied in the descriptor aColumn.

If the column in the current record has a NULL value and the table has a template
record, then the value located in the corresponding column in that template
record is read and copied into aValue.

If the column in the current record has a NULL value and the table does not
have a template record, then the function leaves.

This read operation must not occur if any of the operations involved in inserting
or updating a record are still outstanding, otherwise the function raises
a CommsDbServer 10 panic. For example, the panic is raised if this function
is called between calls to UpdateRecord() and PutRecordChanges().

This panic is also raised if this function is called after the view has been
closed.

@param aColumn A reference to a descriptor containing the name of the column
in the current record whose (boolean) value is to be read.
@param aValue A reference to a TBool passed by the caller.
@leave KErrUnknown if the specified column is null; KErrNotFound if the specified column
does not exist in the table.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS(iState==EOpenIdle, CommsDbPanic(ECommsDbReadNotOpenIdle));

	TInt type(0);

	CMDBField<TBool>* field = static_cast<CMDBField<TBool>*>(iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumn, type));
	// check if value is null
	if (field->IsNull())
		{
		// look for template value
		if (iTemplate)
			{
			iTemplate->ReadBoolL(aColumn, aValue);
			}
		else
			{
			__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrUnknown) in ReadBoolL -> TableName=%S, Column=%S"), &iTableName, &aColumn);
			// template value is not there
			User::Leave(KErrUnknown);
			}
		}
	else
		{
		aValue = *field;
		}

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("ReadBoolL -> TableName=%S, Column=%S, Value=%d"), &iTableName, &aColumn, aValue);
	}

EXPORT_C void CCommsDbTableView::ReadTypeAttribL(const TDesC& aColumn, TDbColType& aColType, TUint32& aAttrib)
/** Gets the type and the attributes of a specific column within the current record
and puts them into a TDbColType object and a reference to an unsigned integer
respectively. The column is identified by the name supplied in the descriptor
aColumn.

The column attributes are one or more of the values TDbCol::ENotNull and TDbCol::EAutoIncrement.

From v9.1
COMMDB_ID doesn't exist. Bacause of BC it is neccessary to return expected values

This function must not be called if any of the operations involved in inserting
or updating a record are still outstanding, otherwise the function raises
a CommsDbServer 10 panic. For example, the panic is raised if this function
is called between calls to UpdateRecord() and PutRecordChanges().

This panic is also raised if this function is called after the view has been
closed.

@param aColumn A reference to a descriptor containing the name of the column
in the current record whose type and attributes are to be fetched.
@param aColType A reference to a TDbColType object passed by the caller. On
successful return from this function, contains a copy of the column type.
@param aAttrib A reference to an unsigned integer passed by the caller. On
successful return from this function, contains a copy of the column attributes.

@leave KErrNotFound Specified column does not exist in the table
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS(iState==EOpenIdle, CommsDbPanic(ECommsDbReadNotOpenIdle));

	if(aColumn.CompareF(TPtrC(COMMDB_ID)) == 0)
		{
		aColType = EDbColUint32;
		aAttrib = TDbCol::EAutoIncrement | TDbCol::ENotNull;
		}
	else
		{
		iTableExt->GetRecordInfoL(aColumn, aColType, aAttrib);
		}

	__FLOG_STATIC4(KLogComponent, KCommDbShim, _L("ReadTypeAttribL -> TableName=%S, Column=%S, Value=%d, Attrib=%d"), &iTableName, &aColumn, aColType, aAttrib);
	}

EXPORT_C void CCommsDbTableView::ReadColumnMaxLengthL(const TDesC& aColumn, TInt& aLength)
/**
Reads the MAXIMUM length of the contents of aColumn from the current record into aLength

@param aColumn A reference to a descriptor containing the name of a column in the current record.
@param aLength A reference to an integer passed by the caller. On successful return from this function, contains a copy of the length of the column.
@capability Dependent on table, see the guide page referenced below.
*/
	{
	TInt type = 0;

	iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumn, type);
	aLength = iTableExt->GetMaxTypeLength(type);

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("ReadColumnMaxLengthL -> TableName=%S, Column=%S, MaxLength=%d"), &iTableName, &aColumn, aLength);
	}

EXPORT_C void CCommsDbTableView::ReadColumnLengthL(const TDesC& aColumn, TInt& aLength)
/** Gets the length of a specific column within the current record and copies it
into a reference to an integer. The column is identified by the name supplied
in the descriptor aColumn.

If the length of the column is zero and the table has a template record, then
the length of the corresponding column in that template record is copied into
aValue.

Null columns return 0; integers and bools return 1; strings return their length in
characters.

This read operation must not occur if any of the operations involved in inserting
or updating a record are still outstanding, otherwise the function raises
a CommsDbServer 10 panic. For example, the panic is raised if this function
is called between calls to UpdateRecord() and PutRecordChanges().

This panic is also raised if this function is called after the view has been
closed.

@param aColumn A reference to a descriptor containing the name of a column
in the current record.
@param aLength A reference to an integer passed by the caller. On successful
return from this function, contains a copy of the length of the column.
@leave KErrNotFound Specified column does not exist in the table.
@capability Dependent on table, see the guide page referenced below. */
	{
    TInt type;

	aLength = 0;

    CMDBElement* field = iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumn, type);

	if(field->IsNull())
		{
		if(iTemplate)
			{
			iTemplate->ReadColumnLengthL(aColumn, aLength);
			}
		}
	else
		{
		switch(type)
			{
			case EDesC8:
				{
				CMDBField<TDesC8>* textField = static_cast<CMDBField<TDesC8>*>(field);
				const TDesC8& refField = *textField;
				const TPtrC8 ptr(refField);
				aLength = ptr.Length();
				}
				break;
			case EText:
			case EMedText:
			case ELongText:
				{
				CMDBField<TDesC>* textField = static_cast<CMDBField<TDesC>*>(field);
				const TDesC& refField = *textField;
				const TPtrC ptr(refField);
				aLength = ptr.Length();
				}
				break;
			default:
				aLength = KMaxNumLength;
			}
		}

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("ReadColumnLengthL -> TableName=%S, Column=%S, Length=%d"), &iTableName, &aColumn, aLength);
	}

EXPORT_C void CCommsDbTableView::WriteTextL(const TDesC& aColumn, const TDesC8& aValue)
/** Writes the narrow (ASCII) text from an 8 bit descriptor to a specific column
within the current record. The column is identified by the name supplied in
the descriptor aColumn. The length of the text cannot be greater than the
value of the constant KCommsDbSvrMaxColumnNameLength, otherwise the function
leaves.

An earlier call to either UpdateRecord() or InsertRecord() must have been
made before calling this function otherwise the function raises a CommsDbServer
12 panic. If the column identified is the COMMDB_ID field, than this function
raises a CommsDbServer 13 panic.

PutRecordChanges() must be called to store the changes.

@note If the column to be read is encrypted then the function decrypts it.

@param aColumn A reference to a descriptor containing the name of a column
in the current record.
@param aValue A reference to an 8 bit descriptor containing the narrow text
to be written into the column.
@leave KErrOverflow The function can leave for reasons defined by DBMS but
it also leaves with KErrOverflow if the length of the text is greater than
the maximum permitted.
@leave KErrNotFound The column name does not exist.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS((iState==EInsert || iState==EUpdate), CommsDbPanic(ECommsDbWriteNotInsertUpdate));

	TInt type(0);

	CMDBRecordBase* record = iTableExt->GetCurrentRecordL();
	CMDBElement* element = record->GetFieldByNameL(aColumn, type);

	// Is it writable?
	if ( record->IsSetAttribute(ECDNoWriteButDelete) ||
     	 record->IsSetAttribute(ECDProtectedWrite) )
        {
		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrAccessDenied) in WriteTextL -> TableName=%S, Column=%S"), &iTableName, &aColumn);
        User::Leave(KErrAccessDenied);
        }

	TInt size = aValue.Length();


	//Commdb only allows binary fields to be of length KMaxTextLength.
	//This is unecessarily limiting for generic records. Below is a
	//work around to allow binary fields in generic records to be of longer length.
	if (type == EDesC8 && iTableExt->IsGenericRecord(record->ElementId()))
		{
		if (size > KMaxLongTextLength)
			{
			__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrOverflow) in WriteTextL -> TableName=%S, Column=%S, Value[8]"), &iTableName, &aColumn);
			User::Leave(KErrOverflow);
			}
		}
	else
		{
		if (size>iTableExt->GetMaxTypeLength(type))
			{
			__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrOverflow) in WriteTextL -> TableName=%S, Column=%S, Value[8]"), &iTableName, &aColumn);
			User::Leave(KErrOverflow);
			}
		}


	CMDBField<TDesC8>* field = static_cast<CMDBField<TDesC8>*>(element);
	field->SetMaxLengthL(size);
	*field = aValue;

	__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("WriteTextL -> TableName=%S, Column=%S ..."), &iTableName, &aColumn);
	__FLOG_STATIC1(KLogComponent, KCommDbShim, _L8("WriteTextL -> ... Value[8]=%S"), &aValue);
	}

EXPORT_C void CCommsDbTableView::WriteTextL(const TDesC& aColumn, const TDesC16& aValue)
/** Writes the wide (UNICODE) text from a 16 bit descriptor to a specific column
within the current record. The column is identified by the name supplied in
the descriptor aColumn. The length of the text cannot be greater than the
value of the constant KCommsDbSvrMaxColumnNameLength, otherwise the function
leaves.

An earlier call to either UpdateRecord() or InsertRecord() must have been
made before calling this function otherwise the function raises a CommsDbServer
12 panic. If the column identified is the COMMDB_ID field, than this function
raises a CommsDbServer 13 panic. This panic is also raised if this function
is called after the view has been closed.

PutRecordChanges() must be called to store the changes.

@note If the column to be read is encrypted then the function decrypts it.

@param aColumn A reference to a descriptor containing the name of a column
in the current record.
@param aValue A reference to a 16 bit descriptor containing the wide text to
be written into the column.
@leave The function can leave for reasons defined by DBMS but also
leaves with KErrOverflow if the length of the text is greater than the maximum
permitted or KErrNotFound if the column name does not exist.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS((iState==EInsert || iState==EUpdate), CommsDbPanic(ECommsDbWriteNotInsertUpdate));

	if( (aColumn.CompareF(TPtrC(GPRS_APN)) == 0) || (aColumn.CompareF(TPtrC(WAP_START_PAGE)) == 0) )
		{
		WriteLongTextL(aColumn,aValue);
		return;
		}

	TInt type(0);
	CMDBRecordBase* record = iTableExt->GetCurrentRecordL();
	CMDBElement* element = record->GetFieldByNameL(aColumn, type);

	// Is it writable?
	if ( record->IsSetAttribute(ECDNoWriteButDelete) ||
     	 record->IsSetAttribute(ECDProtectedWrite) )
        {
		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrAccessDenied) in WriteTextL -> TableName=%S, Column=%S, Value[16]"), &iTableName, &aColumn);
        User::Leave(KErrAccessDenied);
        }

	CMDBField<TDesC>* field = static_cast<CMDBField<TDesC>*>(element);

	TInt size = aValue.Length();

	if (size>iTableExt->GetMaxTypeLength(type))
		{
		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrOverflow) in WriteTextL -> TableName=%S, Column=%S, Value[16]"), &iTableName, &aColumn);
		User::Leave(KErrOverflow);
		}

	field->SetMaxLengthL(size);
	*field = aValue;

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("WriteTextL -> TableName=%S, Column=%S, Value[16]=%S"), &iTableName, &aColumn, &aValue);
	}

EXPORT_C void CCommsDbTableView::WriteLongTextL(const TDesC& aColumn, const TDesC& aValue)
/** Writes the long text from a descriptor to a specific column within the current
record. The column is identified by the name supplied in the descriptor aColumn.

An earlier call to either UpdateRecord() or InsertRecord() must have been
made before calling this function otherwise the function raises a CommsDbServer
12 panic. If the column identified is the COMMDB_ID field, than this function
raises a CommsDbServer 13 panic. This panic is also raised if this function
is called after the view has been closed.

PutRecordChanges() must be called to store the changes.

While the text written by WriteTextL() functions is limited in length, there
is no restriction on the length of long text.

@param aColumn A reference to a descriptor containing the name of a column
in the current record.
@param aValue A reference to a descriptor containing the long text to be written
into the column.
@leave KErrNotFound The column name does not exist.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS((iState==EInsert || iState==EUpdate), CommsDbPanic(ECommsDbWriteNotInsertUpdate));

	TInt type(0);
	CMDBRecordBase* record = iTableExt->GetCurrentRecordL();
	CMDBElement* element = record->GetFieldByNameL(aColumn, type);

	// Is it writable?
	if ( record->IsSetAttribute(ECDNoWriteButDelete) ||
     	 record->IsSetAttribute(ECDProtectedWrite) )
        {
		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrAccessDenied) in WriteLongTextL -> TableName=%S, Column=%S"), &iTableName, &aColumn);
        User::Leave(KErrAccessDenied);
        }

	CMDBField<TDesC>* field = static_cast<CMDBField<TDesC>*>(element);

	field->SetMaxLengthL(aValue.Length());
	*field = aValue;

	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("WriteLongTextL -> TableName=%S, Column=%S, Value=%S"), &iTableName, &aColumn, &aValue);
	}

EXPORT_C void CCommsDbTableView::WriteUintL(const TDesC& aColumn, const TUint32& aValue)
/** Writes an unsigned integer value to a specific column within the current record.
The column is identified by the name supplied in the descriptor aColumn.

An earlier call to either UpdateRecord() or InsertRecord() must have been
made before calling this function otherwise the function raises a CommsDbServer
12 panic. If the column identified is the COMMDB_ID field, than this function
raises a CommsDbServer 13 panic. This panic is also raised if this function
is called after the view has been closed.

PutRecordChanges() must be called to store the changes.

@param aColumn A reference to a descriptor containing the name of a column
in the current record.
@param aValue A reference to an unsigned integer containing the value to be
written into the column.
@leave KErrNotFound The column name does not exist.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS((iState==EInsert || iState==EUpdate), CommsDbPanic(ECommsDbWriteNotInsertUpdate));

	TInt type(0);

	CMDBRecordBase* record = iTableExt->GetCurrentRecordL();
	CMDBElement* element = record->GetFieldByNameL(aColumn, type);

	// Is it writable?
	if ( record->IsSetAttribute(ECDNoWriteButDelete) ||
     	 record->IsSetAttribute(ECDProtectedWrite) )
        {
		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrAccessDenied) in WriteUintL -> TableName=%S, Column=%S"), &iTableName, &aColumn);
        User::Leave(KErrAccessDenied);
        }

	CMDBField<TUint32>* field = static_cast<CMDBField<TUint32>*>(element);
	*field = aValue;
	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("WriteUintL -> TableName=%S, Column=%S, Value=%d"), &iTableName, &aColumn, aValue);
	}


EXPORT_C void CCommsDbTableView::WriteBoolL(const TDesC& aColumn, const TBool& aValue)
/** Writes a Boolean value to a specific column within the current record. The
column is identified by the name supplied in the descriptor aColumn.

An earlier call to either UpdateRecord() or InsertRecord() must have been
made before calling this function otherwise the function raises a CommsDbServer
12 panic. If the column identified is the COMMDB_ID field, than this function
raises a CommsDbServer 13 panic. This panic is also raised if this function
is called after the view has been closed.

PutRecordChanges() must be called to store the changes.

@param aColumn A reference to a descriptor containing the name of a column
in the current record.
@param aValue A reference to TBool containing the value to be written into
the column.
@leave KErrNotFound The column name does not exist.
@capability Dependent on table, see the guide page referenced below. */
	{
	__ASSERT_ALWAYS((iState==EInsert || iState==EUpdate), CommsDbPanic(ECommsDbWriteNotInsertUpdate));

	TInt type(0);

	CMDBRecordBase* record = iTableExt->GetCurrentRecordL();
	CMDBElement* element = record->GetFieldByNameL(aColumn, type);

	// Is it writable?
	if ( record->IsSetAttribute(ECDNoWriteButDelete) ||
     	 record->IsSetAttribute(ECDProtectedWrite) )
        {
		__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("Leave(KErrAccessDenied) in WriteBoolL -> TableName=%S, Column=%S"), &iTableName, &aColumn);
        User::Leave(KErrAccessDenied);
        }

	CMDBField<TBool>* field = static_cast<CMDBField<TBool>*>(element);
	*field = aValue;
	__FLOG_STATIC3(KLogComponent, KCommDbShim, _L("WriteBoolL -> TableName=%S, Column=%S, Value=%b"), &iTableName, &aColumn, aValue);
	}

EXPORT_C void CCommsDbTableView::SetNullL(const TDesC& aColumn)
/** Sets a specific column within the current record to NULL. For text type columns,
this is the same as setting the column to an empty string. The column is identified
by the name supplied in the descriptor aColumn.

An earlier call to either UpdateRecord() or InsertRecord() must have been
made before calling this function otherwise the function raises a CommsDbServer
12 panic. This panic is also raised if this function is called after the view
has been closed.

PutRecordChanges() must be called to store the changes.

@param aColumn A reference to a descriptor containing the name of a column
in the current record. */
	{
	__ASSERT_ALWAYS((iState==EInsert || iState==EUpdate), CommsDbPanic(ECommsDbWriteNotInsertUpdate));

	TInt type(0);

	CMDBRecordBase* record = iTableExt->GetCurrentRecordL();
	// Is it writable?
	if ( record->IsSetAttribute(ECDNoWriteButDelete) ||
     	 record->IsSetAttribute(ECDProtectedWrite) )
        {
        User::Leave(KErrAccessDenied);
        }

	CMDBField<TBool>* field = static_cast<CMDBField<TBool>*>(iTableExt->GetCurrentRecordL()->GetFieldByNameL(aColumn, type));
	// setting null is implemented as deleting a field under commsdat
	field->DeleteL(*iDb.iImpl->iDbs);

	__FLOG_STATIC2(KLogComponent, KCommDbShim, _L("SetNullL -> TableName=%S, Column=%S"), &iTableName, &aColumn);
	}

void CCommsDbTableView::Close()
/**
Closes the rowset and releases any owned resources.
*/
	{
	iState=EClosed;
	}

TBool CCommsDbTableView::TableWithTemplateRecord() const
/**
Is the table one with template record - all except global settings,
IAPs and Proxies.
*/
	{
	if (iTableName.CompareF(TPtrC(KCDTypeNameGlobalSettings)) == 0)
		{
		return EFalse;
		}
	if (iTableName.CompareF(TPtrC(KCDTypeNameIAP)) == 0)
		{
		return EFalse;
		}
	if (iTableName.CompareF(TPtrC(KCDTypeNameProxies)) == 0)
		{
		return EFalse;
		}
	if (iTableName.CompareF(TPtrC(KCDTypeNameConnectionPrefs)) == 0)
		{
		return EFalse;
		}

	return ETrue;
	}
TDbColNo CCommsDbTableView::ColNumL(const TDesC& /*aColumn*/) const
/**
Return the column number of the Column aColumn in the table

Coloumn concept doesn't exist in CommsDat
Not supported from v9.1

@param aColumn A reference to a descriptor containing the name of a column in the current record.
@return The ordinal number of the column else KDbNullColNo if no such column exists.
*/
	{
	return 0;
	}
RDbRowSet::TAccess CCommsDbTableView::GetL()
/**
Gets the operations that can be performed on the current view

@return EUpdatable, if All operations can be performed on the rowset.
EReadOnly, Row navigation and reading are permitted.
EInsertOnly, Inserting new rows is the only valid operation on the rowset.
*/
	{
	RDbRowSet::TAccess access;
	iTableExt->GetTableAccess(access);
	return access;
	}

void CCommsDbTableView::OpenL(const TDbQuery& /*aQuery*/)
/**
@deprecated This method is deprecated from 9.1 and always returns/leaves with KErrNotSupported instead of describing past operation/parameters.

@param aQuery A reference to TDbQuery which contains the query to perform
*/
	{
	User::Leave(KErrNotSupported);
	}
