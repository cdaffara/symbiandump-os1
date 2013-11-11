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

/**
 @file
 @internalComponent
*/



#ifndef DBACCESS_H
#define DBACCESS_H

#include "dbdef.h"

namespace
	{
	// Used to process Hidden and ReadOnly records.

	// We can't simply set ReadOnly / Hidden columns to their values.
    // We must intercept these columns, examine their values, and set
	// the record state.
	const TPtrC KHidden(COMMDB_HIDDEN); // field
	const TPtrC KReadOnly(COMMDB_READ_ONLY); // field
	const TPtrC KTrueVal(TRUE_VAL); // TRUE
	}

class LinkByTagResolver;
class DBAccess : public CBase
/**
@internalComponent
*/
	{
	public:
	    //not allowing implicit conversions...
		explicit DBAccess(const TBool aXML);
		~DBAccess();

		// access
		GLDEF_C void InitCommsdatL();
		GLDEF_C void Close();
		GLDEF_C void MaybeBeginTransactionL();
		GLDEF_C TInt CommitTransaction();
		GLDEF_C void CloseTable();

		GLDEF_C TInt CommitChanges();

		// miscellaneous
		GLDEF_C void SetFieldCount(TInt fields)	{ FieldCountExpected = fields; }
		GLDEF_C TInt GetFieldCount()			{ return FieldCountExpected; }
		GLDEF_C TVersion Version()				{ return dbVersion; }
		GLDEF_C TInt ErrorCode()				{ return iLastError; }
		GLDEF_C TPtrC ErrorText()				{ return LastErrorMessage; }
		GLDEF_C TInt GetActualFieldCount()     { return iColCount; }
		GLDEF_C void CheckElementValidity(TBool aToggle)     { iValidityChecking = aToggle; }

		GLDEF_C TBool IsNameSet()				{ return iNameSet; }
		GLDEF_C TInt  GetRecordCount()			{ return iRecordCount; }
		GLDEF_C void  RecordIsTemplate(TBool aIsTemplate);

		GLDEF_C void SetCommitIndividualRecords(TBool aValue)
			{
			iCommitIndividualRecords = aValue;
			}
		GLDEF_C TBool CommitIndividualRecords() const
			{
			return iCommitIndividualRecords;
			}

		//new
		enum TCreateType
			{
			ECreateNew,
			ECreateInsert,
			EInsert
			};
		GLDEF_C void CreateOrInsertRecordL(TCreateType aType,TMDBElementId aRecordElementId, TInt aRecordId);
		GLDEF_C void RemoveRecord(TInt aPos);
		GLDEF_C TInt SetColAndAttribL(TPtrC& aColumn,TPtrC& aSetting);
		GLDEF_C TBool ConvertEnumsL(CMDBElement* aPtrField, TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertMiscEnums(CMDBElement* aPtrField, TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertModemEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertGPRSEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertCDMAEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertConnectionPrefEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertISPEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertPANEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
        GLDEF_C TBool ConvertLinkRecords(CMDBElement* aPtrField, TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
        GLDEF_C TBool CheckLegacyLinkRecords(TPtrC& aColumn,TPtrC& aSetting);
		GLDEF_C TBool SetLinkedRecord(TUint32 aIndex, CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertAccessPointEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertIapPrioritySelectionPolicyEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		GLDEF_C TBool ConvertApPrioritySelectionPolicyEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		GLDEF_C void DoInitAndAddL(CCDRecordBase* aNewRecord,TInt aRecordId);
		GLDEF_C void InitAndAddL(CCDRecordBase* aNewRecord,TInt aRecordId);
		GLDEF_C void SaveTableProtection(CMDBRecordSetBase* aRecordSet);
		GLDEF_C TBool AssignFieldValuesL(TPtrC& aColumn,TInt aTempType,CMDBElement* aPtrField,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TInt ConvertSpecialCharsL(TInt aTempType,CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertWAPEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C void RemoveCurrentRecord();
		GLDEF_C CMDBRecordBase* GetCurrentRecord();
		GLDEF_C TBool IsConnectionPrefValid(CCDConnectionPrefsRecord& aConnectionPrefRecord);
		GLDEF_C TUint32 FindTableIdL(TDesC& aTableName);
        GLDEF_C TBool CheckBearer(CMDBElement* aPtrField,TPtrC aSetting);
		GLDEF_C TBool ConvertWLANEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertEAPSecEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C TBool ConvertTUNEAPSecEnums(CMDBElement* aPtrField,TPtrC& aColumn,TPtrC& aSetting,TMDBAttributeFlags aAttribute);
		GLDEF_C void SetInAppendMode(TBool aInAppendMode)	{ iInAppendMode = aInAppendMode; }
		GLDEF_C TBool TemplateFieldPresentL(TMDBElementId aElementId);
		GLDEF_C void SetLinkByTagResolver(LinkByTagResolver* aResolver)	{ iLnkByTagRes = aResolver; }
		GLDEF_C void RemoveLinkByTagResolver()	{ iLnkByTagRes = NULL; }
		//This function is a getter/setter
		GLDEF_C TBool& CfgXmlSetting()	{ return iIsXML; }
		GLDEF_C void SetMeshProcessing(TBool aIsMeshProcessing)	{ iIsMeshFileProcessing = aIsMeshProcessing; }

	private:
		// intermediate data structures
		TInt ColCount;									// amount of columns to update
		COLATTRIBUTES TargetCols[MAX_DB_COLUMNS];		// data for columns to update

		TBool iSecure;
		TBool iIsTemplate;
		TBool iConfusingTemplateBehaviour;
		TBool iNameSet;
		TBool iCommitIndividualRecords;	// for special tables have to commit each record individually, else one duplicate will rollback all edits
		TBool iIsMeshFileProcessing;

		// miscellaneous
		TUint32 access;									// access bitmask on a table
		TVersion dbVersion;								// version of the database
		TInt iLastError;								// ret value of last call
		TPtrC LastErrorMessage;							// associated message if any
		TInt FieldCountExpected;						// amount of field updates expected

		TMDBElementId iTableId;
		CMDBRecordSetBase* iTable;
		CMDBSession* iDbSession;
		TInt iRecordCount;
		TUint32 iRecordId;
		TInt iColCount;
		TBool iValidityChecking;

		TInt i2000PacketServiceTable;
		RPointerArray<CMDBRecordSetBase> iRecordSetHolder;

		GLDEF_C TInt Evaluate(TInt err);				// determines if there's been an error
		TBool iInAppendMode;
		TBool iIsXML;
		//the class USES this pointer and doesn't own it!!!
		LinkByTagResolver* iLnkByTagRes;
	};

#endif
