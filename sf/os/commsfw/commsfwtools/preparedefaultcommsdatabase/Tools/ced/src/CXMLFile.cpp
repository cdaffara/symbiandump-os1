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

#include "dbdef.h"
#include <ecom/ecom.h>
#include <xml/parser.h>
#include <xml/xmlparsererrors.h>
#include "CXMLFile.h"

#include "filedump.h"
extern CFileDump* gMsg;		// logging

using namespace Xml;

_LIT8(KParserDataType, "text/xml");

CXMLData::CXMLData(CXMLDatabase* aXmlDb, TBool aForceXMLProcessing, TBool aAppendMode) 
: xmlDb(aXmlDb)
, bForceXMLProcessing(aForceXMLProcessing)
, iAppendMode(aAppendMode)
	{
	__DECLARE_NAME(_S("CXMLData"));
	}

CXMLData::~CXMLData()
	{
	}

CXMLData*
CXMLData::NewL(RFs& aFs, const TDesC& aName, CXMLDatabase* aXmlDb, 
               TBool aForceXMLProcessing,
               TBool aAppendMode)
	{
	CXMLData* p = new(ELeave) CXMLData(aXmlDb,aForceXMLProcessing, aAppendMode);
	CleanupStack::PushL(p);

	if(!p->ConstructL(aFs,aName,aXmlDb))
		{
		CleanupStack::PopAndDestroy(p);	
		p = NULL;
		}
	else
		{
		CleanupStack::Pop(p);
		}
	
	return p;
	}

//
// Parse the XML settings file
//
TBool CXMLData::ConstructL(RFs& aFs, const TDesC& aName, CXMLDatabase* aXmlDb)
	{    	
	// Create the content handler for the XML file
	//CXMLContentHandler contentHandler;
	CXMLContentHandler* contentHandler = CXMLContentHandler::NewL(aXmlDb,
	                                                              bForceXMLProcessing,
	                                                              iAppendMode);
	TBool retCode = EFalse;
	
	// Create the XML parser (SAX-type)
	gMsg->Msg(_L("Create the parser"));
	CParser* parser = CParser::NewLC(KParserDataType, *contentHandler);
	
	// Perform the parsing
	gMsg->Msg(_L("Starting the parsing"));
	ParseL(*parser, aFs, aName);
    CleanupStack::PopAndDestroy(parser);    
	
    // Modify the table entry references to the format used by CommDB
	TBool bSuccess = contentHandler->ModifyTableEntryReferencesL();
	if(bSuccess) 
	    {
	    retCode = ETrue;
	    // Log the table entries
	    contentHandler->LogTableEntries();
	    }
	
	CleanupStack::PopAndDestroy(contentHandler);
	
	return retCode;
	}

// Get a setting from the XML database for a specific table entry given the name of the
// table parameter
TInt CXMLData::GetSetting(const TInt aEntryIndex, const TDesC & aParamName, TPtrC & aParamValue)
	{
    CXMLTableEntry* entry = xmlDb->GetTableEntry(aEntryIndex);
	
	for(TInt k = 0; k < entry->GetNumberParameters(); k++)
		{
		const TBuf<MAX_BUFFER_LEN>& paramName = entry->GetParameterName(k);
		
		if(paramName.Compare(aParamName) == 0)
			{
			aParamValue.Set(TPtrC(entry->GetParameterValue(k)));
			return ETrue;
			}
		}
	
    return EFalse;
	}

// Get the starting index for all table entries of a specific table. If table entries
// of the specified table do not exist then -1 is returned
TInt CXMLData::GetStartingIndex(const TDesC& aTableName)
	{
	TInt numEntries = xmlDb->GetNumberTableEntries();
	
    for(TInt i = 0; i < numEntries; i++)
		{
		CXMLTableEntry* entry = xmlDb->GetTableEntry(i);
		const TBuf<MAX_BUFFER_LEN>& tableName = entry->GetTableName();
		
		if(tableName.Compare(aTableName) == 0)
			return i;
		}
	
	return -1;
	}

// Get the last index for the table entries of a specific table. If table entries
// of the specified table do not exist then -1 is returned
TInt CXMLData::GetLastIndex(const TDesC& aTableName)
	{
	TInt numEntries = xmlDb->GetNumberTableEntries();
	
    for(TInt i = numEntries-1; i >= 0; i--)
		{
		CXMLTableEntry* entry = xmlDb->GetTableEntry(i);
		const TBuf<MAX_BUFFER_LEN>& tableName = entry->GetTableName();
		
		if(tableName.Compare(aTableName) == 0)
			return i;
		}
	
	return -1;
	}

// Get the number of parameters for a specific table entry
TInt CXMLData::GetEntryNumberParameters(const TInt aEntryIndex)
	{
	return xmlDb->GetTableEntry(aEntryIndex)->GetNumberParameters();
	}

// Get the type of operation to perform for this table entry
const TBuf<MAX_BUFFER_LEN>& CXMLData::GetOperation(const TInt aEntryIndex)
	{
	return xmlDb->GetTableEntry(aEntryIndex)->GetOperation();
	}
	
TInt CXMLData::GetElementRecordID(const TInt aEntryIndex) const
    {
    return xmlDb->GetTableEntry(aEntryIndex)->GetRecordID();
    }
