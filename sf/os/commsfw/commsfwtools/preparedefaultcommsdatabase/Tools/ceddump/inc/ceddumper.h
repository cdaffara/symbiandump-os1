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
 @file
 @publishedAll
 @released
*/

#ifndef CEDDUMPER_H
#define CEDDUMPER_H


#include <e32base.h>
#include <centralrepository.h>
#include "filedump.h"
#include "ceddumpglobals.h"

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
#include "R99QoSConversion.h"
#endif

class CConsoleBase;

#define MAX_BUFFER_LEN          256 // max length of path/filename/column/table name
#define MAX_COL_NAME_LEN        64
#define MAX_ARG_LEN				(MAX_BUFFER_LEN * 3) // max length of arg list to program

#ifdef __TOOLS2__
#define DEFAULTCFGFILENAME		_S("cedout.cfg") //Default out put file ( .cfg format )
#else
#define DEFAULTCFGFILENAME		_S("C:\\cedout.cfg") //Default out put file ( .cfg format )
#endif

//_LIT(APPLICATIONNAME,"CommsData Database Dump Utility");
//_LIT(APPLICATIONVERSION,"1.0");
#define TRUE_VAL				_S("TRUE")
#define FALSE_VAL				_S("FALSE")

typedef RArray<SGenericRecordTypeInfo> RRecordInfoArray;
typedef TBuf<KCDMaxFieldNameLength> TGenericTableName;

class CCedDumper : public CBase
	{
	public:
		static CCedDumper* NewL();
		~CCedDumper();

		TInt OpenL();
		void HelpDump();
		TInt ProcessCommandLineL();
		TBool ParseCommandLine();
		void DumpContents();
		void PrintError(TInt error);
		void GetRecordTypeL(TMDBElementId aRecordElementId);
		void WriteTableAttributes(TDesC &iTableName);
		void WriteTemplateAndRecordsL();
		void GetColumnAttributeAndValuesL(TInt aRecordId);
		void ResetAllAttrubuteFlags();
		TBool ConvertEnums(TPtrC &theColumn, TPtrC &setting);
		TBool ConvertRecordLinks(TPtrC &theColumn, TPtrC &setting, CMDBElement* aField);
		TBool IsLinkedField(TPtrC &theColumn, TPtrC &setting, CMDBElement& aField);

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
		CR99QoSConversion::TConvRecordType CheckAndPerformConversion(TPtrC &theColumn, TPtrC &setting);
		void WriteR99Conversion();
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS

	protected:
		void ConstructL();

	private:
		TBool FillBufferWithLink(TUint32 aIndex, TPtrC& aColumn, TPtrC& aSetting, CMDBElement* aField);

		// This method can handle only Fields pointed by CMDBElement pointers!!!
		TBool IsFieldValuesTheSame(CMDBElement* aFirstField, CMDBElement* aSecondField, TInt aFieldTypeValue);
		
		//This function is for resolving the inconsistent table names in CommsDat and CommDB - unfortunatly CED and CEDDUMP is using
		//the CommDB variant. :(((((
		//Please note: As long as the naming for the new follows the rule, which is to have the same name in the CommsDat and CommDB components,
		//this function doesn't need to be modified.
		void ResolveCommsDatTableNameVariant_to_CommdbTableNameVariant(RBuf& aCommsdatTableName);

		void ListGenericTablesL();
		void DumpGenericTablesL();
		void ConvertFieldTypeL(TInt aFieldType, TDes &aConvertedFieldType);
		void ConvertFieldAttributeL(TInt aFieldAttribute, TDes &aConvertedFieldAttribute);
		TBool WriteFieldValueL(CMDBGenericRecord* aRecord, const TDesC& aFieldName);

	private:
		CMDBSession* iDbSession; // Session to connect to CenRep
		CMDBRecordSetBase *iTable; //To get the Record type
		CMDBRecordBase* iRecordBase;
		CMDBRecordBase* iTemplateRecordBase;
		CConsoleBase* iConsole; // Handle to write to console

		CFileDump* iFileDumper; // Handle to write to output file

		TBuf<MAX_BUFFER_LEN> iCfgFileName; // To hold the out put file name

		TBool iIsHiddenSet;
		TBool iIsPrivateSet;
		TBool iIsProtectedReadSet;
		TBool iIsProtectedWriteSet;

#ifdef SYMBIAN_NETWORKING_3GPPDEFAULTQOS
		TBool iR99Conversion; // flag indicating a conversion of QoS GPRS values from R97/98 to R99/R4
		CR99QoSConversion *iR99ConversionRecordsHead;  	// uni direction list head of conversion output tables
		CR99QoSConversion *iR99ConversionRecordsCurrent; 	// conversion output table which is being build
#endif
// SYMBIAN_NETWORKING_3GPPDEFAULTQOS
        TBool iUseHexFormat;
        TBool iROHIDSupport;
		CRepository* iRepository;

        RArray<TUint32> iGenericTableIds;
		RArray<TGenericTableName> iGenericTableNames;
		RArray<RRecordInfoArray> iGenericTableFields;
	};

#endif
