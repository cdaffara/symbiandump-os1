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
 
#include "DbUndo.h"


// UNDO Class for Integers
void CDbUndoInt::ConstructL(const TDesC &aColumn, const TUint32 &aValue)
	{
	iColumn.Set(aColumn);
	iValue=aValue;
	}

CDbUndoInt *CDbUndoInt::NewL(const TDesC &aColumn, const TUint32 &aValue)
	{
	CDbUndoInt *undo = new(ELeave) CDbUndoInt();
	CleanupStack::PushL(undo);
	undo->ConstructL(aColumn, aValue);
	CleanupStack::Pop();
	return undo;
	}

void CDbUndoInt::UndoL(CDbAgtBase *aDb)
	{
	// Reset value in the table back to the values stored in this undo object
	aDb->ModifyColumnIntL(iColumn, iValue);
	}

// UNDO Class for Booleans
void CDbUndoBool::ConstructL(const TDesC &aColumn, const TBool &aValue)
	{
	iColumn.Set(aColumn);
	iValue=aValue;
	}

CDbUndoBool *CDbUndoBool::NewL(const TDesC &aColumn, const TBool &aValue)
	{
	CDbUndoBool *undo = new(ELeave) CDbUndoBool();
	CleanupStack::PushL(undo);
	undo->ConstructL(aColumn, aValue);
	CleanupStack::Pop();
	return undo;
	}

void CDbUndoBool::UndoL(CDbAgtBase *aDb)
	{
	// Reset value in the table back to the values stored in this undo object
	aDb->ModifyColumnBoolL(iColumn, iValue);
	}

// UNDO Class for text
void CDbUndoText::ConstructL(const TDesC &aColumn, const TDesC16& aValue)
	{
	iColumn.Set(aColumn);
	iText.Copy(aValue);
	}

CDbUndoText *CDbUndoText::NewL(const TDesC &aColumn, const TDesC16& aValue)
	{
	CDbUndoText *undo = new(ELeave) CDbUndoText();
	CleanupStack::PushL(undo);
	undo->ConstructL(aColumn, aValue);
	CleanupStack::Pop();
	return undo;
	}

void CDbUndoText::UndoL(CDbAgtBase *aDb)
	{
	// Reset text value in the table back to the text values stored in this undo object
	aDb->ModifyColumnTextL(iColumn, iText);
	}

// UNDO Class for long text
void CDbUndoLongText::ConstructL(const TDesC &aColumn, const HBufC* aValue)
	{
	iColumn.Set(aColumn);
	// Save the long text from CommDB, allocate it!
	iLongText = aValue->AllocL();
	}

CDbUndoLongText *CDbUndoLongText::NewL(const TDesC &aColumn, const HBufC* aValue)
	{
	CDbUndoLongText *undo = new(ELeave) CDbUndoLongText();
	CleanupStack::PushL(undo);
	undo->ConstructL(aColumn, aValue);
	CleanupStack::Pop();
	return undo;
	}

void CDbUndoLongText::UndoL(CDbAgtBase *aDb)
	{
	// Reset text value in the table back to the text values stored in this undo object
	TPtr value(iLongText->Des());
	aDb->ModifyColumnLongTextL(iColumn, value);
	// Allocated in ConstructL, remeber to delete!
	delete iLongText;
	}

// UNDO Class for Agent Extension name
void CDbUndoAgentExt::ConstructL(const TDesC &aService, const TDesC& aAgentExt)
	{
	iColumn.Set(aService);
	iText.Copy(aAgentExt);
	}

CDbUndoAgentExt *CDbUndoAgentExt::NewL(const TDesC &aService, const TDesC& aAgentExt)
	{
	CDbUndoAgentExt *undo = new(ELeave) CDbUndoAgentExt();
	CleanupStack::PushL(undo);
	undo->ConstructL(aService, aAgentExt);
	CleanupStack::Pop();
	return undo;
	}

void CDbUndoAgentExt::UndoL(CDbAgtBase* /*aDb*/)
	{
	}

// Base class routines, used to open a CCommsDatabase and modify records
CDbAgtBase::CDbAgtBase() : iQHeader(_FOFF(CDbUndoBase,iLink)), iQIter(iQHeader)
	{
	}

void CDbAgtBase::ConstructL()
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	iDb = CMDBSession::NewL(KCDVersion1_2);
#else
	iDb = CMDBSession::NewL(KCDVersion1_1);
#endif
	}

CDbAgtBase::~CDbAgtBase()
	{
	delete iDb;
	}


void CDbAgtBase::SetColumnIntL(const TDesC &aColumn, const TUint32 &aValue)
	{
	// Used by a client to set a value in the database, also remembers the change
	// for undo purposes later
	
	TUint32 OldValue;
	CDbUndoInt *oldRecord;

	// Get the current value in the table
	TInt ValType;
	CMDBField<TUint>* field = static_cast<CMDBField<TUint>*>(iTable->iRecords[iCurrentRecord]->GetFieldByNameL(aColumn, ValType));
	OldValue = *field;

	// Add to the undo list
	oldRecord = CDbUndoInt::NewL(aColumn, OldValue);
	iQHeader.AddLast(*oldRecord);

	// Change the record to the new value
	ModifyColumnIntL(aColumn, aValue);
	}


void CDbAgtBase::ModifyColumnIntL(const TDesC &aColumn, const TUint32 &aValue)
	{
	TInt ValType;
	CMDBField<TUint>* field = static_cast<CMDBField<TUint>*>(iTable->iRecords[iCurrentRecord]->GetFieldByNameL(aColumn, ValType));
	*field = aValue;
	field->ModifyL(*iDb);
	}

void CDbAgtBase::SetColumnBoolL(const TDesC &aColumn, const TBool &aValue)
	{
	// Used by a client to set a value in the database, also remembers the change
	// for undo purposes later
	
	TBool OldValue;
	CDbUndoBool *oldRecord;
	
	// Get the current value in the table
	TInt ValType;
	CMDBField<TBool>* field = static_cast<CMDBField<TBool>*>(iTable->iRecords[iCurrentRecord]->GetFieldByNameL(aColumn, ValType));
	OldValue = static_cast<TBool>(*field);

	// Add to the undo list
	oldRecord = CDbUndoBool::NewL(aColumn, OldValue);
	iQHeader.AddLast(*oldRecord);

	// Change the record to the new value
	ModifyColumnBoolL(aColumn, aValue);
	}


void CDbAgtBase::ModifyColumnBoolL(const TDesC &aColumn, const TBool &aValue)
	{
	TInt ValType;
	CMDBField<TBool>* field = static_cast<CMDBField<TBool>*>(iTable->iRecords[iCurrentRecord]->GetFieldByNameL(aColumn, ValType));
	*field = aValue;
	field->ModifyL(*iDb);
	}

void CDbAgtBase::SetColumnTextL(const TDesC &aColumn, const TDesC16& aValue)
	{
	// Used by a client to set a text value in the database, also remembers the change
	// for undo purposes later
	
	TBuf16<KCommsDbSvrMaxFieldLength> OldText;
	CDbUndoText *oldRecord;

	// Get the current value in the table
	TInt ValType;
	CMDBField<TDesC>* field = static_cast<CMDBField<TDesC>*>(iTable->iRecords[iCurrentRecord]->GetFieldByNameL(aColumn, ValType));
	OldText = static_cast<TDesC>(*field);

	// Add to the undo list
	oldRecord = CDbUndoText::NewL(aColumn, OldText);
	iQHeader.AddLast(*oldRecord);

	// Change the record to the new value
	ModifyColumnTextL(aColumn, aValue);
	}

void CDbAgtBase::ModifyColumnTextL(const TDesC &aColumn, const TDesC16& aValue)
	{
	TInt ValType;
	CMDBField<TDesC>* field = static_cast<CMDBField<TDesC>*>(iTable->iRecords[iCurrentRecord]->GetFieldByNameL(aColumn, ValType));
	*field = aValue;
	field->ModifyL(*iDb);
	}

void CDbAgtBase::SetColumnLongTextL(const TDesC &aColumn, const TDesC16& aValue)
	{
	// Used by a client to set a long text value in the database, also remembers the change
	// for undo purposes later
	
	CDbUndoLongText *oldRecord;

	// Get the current long text in the table, it is allocated in CommDB in ReadLongTextLC.
	// The method returns a pointer into HBufC.
	TInt ValType;
	CMDBField<TDesC>* field = static_cast<CMDBField<TDesC>*>(iTable->iRecords[iCurrentRecord]->GetFieldByNameL(aColumn, ValType));
	HBufC* OldLongText = HBufC::NewL(static_cast<TDesC>(*field).Length());
	CleanupStack::PushL(OldLongText);
	*OldLongText = *field;

	// Add to the undo list
	oldRecord = CDbUndoLongText::NewL(aColumn, OldLongText);
	iQHeader.AddLast(*oldRecord);

	// Allocated into CDbUndoLongText in ConstructL, and this one was created in CommDB so delete it!
	// this is pushed above so clean up properly
	CleanupStack::PopAndDestroy(OldLongText);

	// Change the record to the new value
	ModifyColumnLongTextL(aColumn, aValue);
	}

void CDbAgtBase::ModifyColumnLongTextL(const TDesC &aColumn, const TDesC16& aValue)
	{
	TInt ValType;
	CMDBField<TDesC>* field = static_cast<CMDBField<TDesC>*>(iTable->iRecords[iCurrentRecord]->GetFieldByNameL(aColumn, ValType));
	field->SetMaxLengthL(aValue.Length());
	*field = aValue;
	field->ModifyL(*iDb);
	}


void CDbAgtBase::UndoDatabaseChangesL()
	{
	CDbUndoBase* currentitem;

	// Traverse back through the changes and reset database to the way it was
	iQIter.SetToLast();

	while((currentitem = iQIter--) != NULL)
        {
        // reset the value of this record in the database
		currentitem->UndoL(this);
        currentitem->iLink.Deque();
        delete currentitem;
        };
	}

// GPRS OUTGOING TABLE access class
CDbGPRSOutgoingTable *CDbGPRSOutgoingTable::NewL()
	{
	CDbGPRSOutgoingTable *gprs = new(ELeave) CDbGPRSOutgoingTable();
	CleanupStack::PushL(gprs);
	gprs->ConstructL();
	CleanupStack::Pop();
	return gprs;
	}

CDbGPRSOutgoingTable::CDbGPRSOutgoingTable()
	{
	}

void CDbGPRSOutgoingTable::ConstructL()
	{
	CDbAgtBase::ConstructL();
	iTable = new CMDBRecordSet<CCDOutgoingGprsRecord>(KCDTIdOutgoingGprsRecord);
	iTable->LoadL(*iDb);
	iCurrentRecord = 0;
	}

CDbGPRSOutgoingTable::~CDbGPRSOutgoingTable()
	{
	delete iTable;
	}

// CSD DIAL OUT table class
CDbCsdTable *CDbCsdTable::NewL()
	{
	CDbCsdTable *csd = new(ELeave) CDbCsdTable();
	CleanupStack::PushL(csd);
	csd->ConstructL();
	CleanupStack::Pop();
	return csd;
	}

CDbCsdTable::CDbCsdTable()
	{
	}

void CDbCsdTable::ConstructL()
	{
	CDbAgtBase::ConstructL();
	iTable = new CMDBRecordSet<CCDDialOutISPRecord>(KCDTIdDialOutISPRecord);
	iTable->LoadL(*iDb);
	iCurrentRecord = 0;
	}

CDbCsdTable::~CDbCsdTable()
	{
	delete iTable;
	}


// MODEM table class
CDbModemTable *CDbModemTable::NewL()
	{
	CDbModemTable *modem = new(ELeave) CDbModemTable();
	CleanupStack::PushL(modem);
	modem->ConstructL();
	CleanupStack::Pop();
	return modem;
	}

CDbModemTable::CDbModemTable()
	{
	}

void CDbModemTable::ConstructL()
	{
	CDbAgtBase::ConstructL();
	iTable = new CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);
	iTable->LoadL(*iDb);
	iCurrentRecord = 0;
	}

CDbModemTable::~CDbModemTable()
	{
	delete iTable;
	}

// Preference table class
CDbPrefTable *CDbPrefTable::NewL( 
	CMDBField<TCommDbConnectionDirection> aDirection,
	CBearerUpdate aBearerUpdate,
	TBool aReadOnly,
	TUint32 aDialogPref )
	{
	CDbPrefTable *pref = new(ELeave) CDbPrefTable();
	CleanupStack::PushL(pref);
	pref->iDirection = static_cast<TCommDbConnectionDirection>(aDirection);
	pref->iBearerUpdate.iBearerSet = aBearerUpdate.iBearerSet;
	pref->iBearerUpdate.iIapId = aBearerUpdate.iIapId;
	pref->iReadOnly = aReadOnly;
	pref->iDialogPref = aDialogPref;
	pref->ConstructL();
	CleanupStack::Pop();
	return pref;
	}

CDbPrefTable::CDbPrefTable()
	{
	}

void CDbPrefTable::ConstructL()
	{
	CDbAgtBase::ConstructL();
	iPrefTable = new (ELeave) CMDBRecordSet<CCDConnectionPrefsRecord>(KCDTIdConnectionPrefsRecord);
	iPrefTable->LoadL(*iDb);
	
	iOldPrefs.iRanking = (static_cast<CCDConnectionPrefsRecord*>(iPrefTable->iRecords[iCurrentPrefRecord]))->iRanking;
	iOldPrefs.iDirection = static_cast<TCommDbConnectionDirection>((static_cast<CCDConnectionPrefsRecord*>(iPrefTable->iRecords[iCurrentPrefRecord]))->iDirection);
	iOldPrefs.iDialogPref = (static_cast<CCDConnectionPrefsRecord*>(iPrefTable->iRecords[iCurrentPrefRecord]))->iDialogPref;
	iOldPrefs.iBearer.iBearerSet = (static_cast<CCDConnectionPrefsRecord*>(iPrefTable->iRecords[iCurrentPrefRecord]))->iBearerSet;
	iOldPrefs.iBearer.iIapId = (static_cast<CCDConnectionPrefsRecord *>(iPrefTable->iRecords[iCurrentPrefRecord]))->iDefaultIAP;
	
	iCurrentPrefRecord = 0;
	((CCDConnectionPrefsRecord *)iPrefTable->iRecords[iCurrentPrefRecord])->iDialogPref = iDialogPref;
	((CCDConnectionPrefsRecord *)iPrefTable->iRecords[iCurrentPrefRecord])->iBearerSet = iBearerUpdate.iBearerSet;
	((CCDConnectionPrefsRecord *)iPrefTable->iRecords[iCurrentPrefRecord])->iDefaultIAP = iBearerUpdate.iIapId;
	if(iReadOnly)
		{
		iPrefTable->iRecords[iCurrentPrefRecord]->SetAttributes(ECDProtectedWrite);
		}
	iPrefTable->iRecords[iCurrentPrefRecord]->ModifyL(*iDb);
	}

CDbPrefTable::~CDbPrefTable()
	{
	if(iPrefTable)
		{
		// Don't care if this fails since we're in the destructor anyway (but need to trap any leaves as we can't leave from a destructor)
		TRAP_IGNORE(RevertPreferenceTableL());
		}
	delete iTable;
	delete iPrefTable;
	}

void CDbPrefTable::RevertPreferenceTableL()
	{	
	// Revert back to old ones
	iCurrentPrefRecord = 0;
	// iOldPrefs has the old info
	((CCDConnectionPrefsRecord *)iPrefTable->iRecords[iCurrentPrefRecord])->iDialogPref = iOldPrefs.iDialogPref;
	((CCDConnectionPrefsRecord *)iPrefTable->iRecords[iCurrentPrefRecord])->iBearerSet = iOldPrefs.iBearer.iBearerSet;
	((CCDConnectionPrefsRecord *)iPrefTable->iRecords[iCurrentPrefRecord])->iDefaultIAP = iOldPrefs.iBearer.iIapId;
	if(iReadOnly)
		{
		iPrefTable->iRecords[iCurrentPrefRecord]->SetAttributes(ECDProtectedWrite);
		}
	iPrefTable->iRecords[iCurrentPrefRecord]->ModifyL(*iDb);
	}

// IAP table class
CDbIapTable *CDbIapTable::NewL(const TDesC16& aIapName)
	{
	CDbIapTable *iap = new(ELeave) CDbIapTable();
	CleanupStack::PushL(iap);
	iap->ConstructL();
	iap->iIapName.Copy( aIapName );
	CleanupStack::Pop();
	return iap;
	}

CDbIapTable::CDbIapTable()
	{
	}

void CDbIapTable::ConstructL()
	{
	CDbAgtBase::ConstructL();
	}

void CDbIapTable::GetBearerSetIapIdL(TUint32& aIapId)
	{
	CCDIAPRecord* iRecord = static_cast<CCDIAPRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdIAPRecord));
	iRecord->iRecordName = iIapName;

	//iTable = new CMDBRecordSet<CCDIAPRecord>();
	//CleanupStack::Pop(iTable);
	iRecord->LoadL(*iDb);
	aIapId = iRecord->RecordId();
	iCurrentRecord = 0;
	}

CDbIapTable::~CDbIapTable()
	{
	delete iTable;
	}
