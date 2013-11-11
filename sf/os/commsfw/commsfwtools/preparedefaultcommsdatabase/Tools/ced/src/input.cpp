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
 @file 
 @internalComponent
*/

#include "input.h"
#include "filedump.h"

namespace
{
TUint32 DoHash(const HBufC& aText)
    {
    const TDesC& ref = aText;
    return DefaultHash::Des16(ref);
    }

TBool AreTheKeysIdentical(const HBufC& aKey1, const HBufC& aKey2)
    {
    return (aKey1 == aKey2);
    }
} //anonymous namespace



CInputFileParser* CInputFileParser::FactoryLC(TBool aIsXML, TDesC& aFile, CFileDump* aLog)
    {
    CInputFileParser* p = NULL;
    
    if (aIsXML)
        {
        p = new(ELeave) CXMLFileParser(aFile, aLog);
        }
    else
        {
        p = new(ELeave) CCfgFileParser(aFile, aLog);
        }

	CleanupStack::PushL(p);
	return p;
    }


CInputFileParser::CInputFileParser(TDesC& aFile, CFileDump* aLog) :
    iFile(aFile),
    iLog(aLog),
    iTable(NULL)
    {
    }

/**
*/
TBool CInputFileParser::IsMeshCompatibleL()
	{
	iLog->Msg(_L("MESHINFO: Checking if input file <%S> is mesh compatible"), &iFile);
	
    InitialiseL();

    const TText** pTable = iTable;
    
	TBuf<MAX_BUFFER_LEN> table            = *pTable;
	TBool                isMeshCompatible = EFalse;
	while (table.Compare(TPtrC(NO_MORE_RECORDS)) != 0)
		{
		iLog->Msg(_L(" "));
		iLog->Msg(_L("Parsing %S Table"), &table);
		
		if (CheckMeshTemplateL(table) || CheckMeshInsertsL(table))
        {
    		iLog->Msg(_L("MESHINFO: Found mesh table <%S>, the configuration file is assumed to be mesh-compatible"), &table);
			isMeshCompatible = ETrue;
			break;
			}					
		table = *(++pTable);
		}	
	return isMeshCompatible;
	}


void CInputFileParser::Exit(TInt aErr)
    {
	iLog->Msg(_L("MESHERR: Configuration file [%S] could not be opened"), &iFile);
	iLog->Msg(_L("==================="));
	iLog->Msg(_L("ERROR"));
	User::Leave(aErr);
    }


CCfgFileParser::CCfgFileParser(TDesC& aFile, CFileDump* aLog) :
    CInputFileParser(aFile, aLog)
    {
    }


CCfgFileParser::~CCfgFileParser()
    {
    iCfg.CloseConfigFile();
    }


void CCfgFileParser::InitialiseL()
    {
    iTable = const_cast<const TText**>(tableMeshMarkerArray);
    if (!iCfg.OpenConfigFile(iFile))
		{
		Exit(KErrNotFound);
		}
    }
    
    
TBool CCfgFileParser::CheckMeshTemplateL(TDesC& aTable)
    {
	return iCfg.OpenTemplateBlock(aTable);
    }
    
    
TBool CCfgFileParser::CheckMeshInsertsL(TDesC& aTable)
    {
	return iCfg.OpenAddBlock(aTable);
    }


CXMLFileParser::CXMLFileParser(TDesC& aFile, CFileDump* aLog) :
    CInputFileParser(aFile, aLog),
    iPtr(NULL, 0)
    {
    }


CXMLFileParser::~CXMLFileParser()
    {
    delete iPtr.Ptr();
    }


void CXMLFileParser::InitialiseL()
    {
    iTable = const_cast<const TText**>(tableXMLMeshMarkerArray);
	TRAPD(ret, OpenXMLFileL());
	if (ret != KErrNone)
		{
		Exit(ret);
		}
    }
    
    
void CXMLFileParser::OpenXMLFileL()
    {
    // Shameless copy of 'CIniData::ConstructL()'
    
	// Connect to file server
	//
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();


	// Open file
	//
	TAutoClose<RFile> file;
	TInt size;
	User::LeaveIfError(file.iObj.Open(fs.iObj, iFile, EFileStreamText | EFileRead));
	file.PushL();


	// Get file size and read in
	//
	User::LeaveIfError(file.iObj.Size(size));
	TText* data = (TText*)User::AllocL(size);
	iPtr.Set(data, TUint(size)/sizeof(TText), TUint(size)/sizeof(TText));
	TPtr8 dest((TUint8*)data, 0, size);
	User::LeaveIfError(file.iObj.Read(dest));
	TUint8* ptr = (TUint8*)data;


	// This is orderred as FEFF assuming the processor is Little Endian
	// The data in the file is FFFE.		PRR 28/9/98
	//
	if (size >= (TInt)sizeof(TText) && iPtr[0]==0xFEFF)
		{
		// TODO: strip out UNICODE spaces.
		
		// UNICODE Text file so lose the FFFE
		//
		Mem::Copy(ptr, ptr+sizeof(TText), size-sizeof(TText));
		iPtr.Set(data, TUint(size)/sizeof(TText)-1, TUint(size)/sizeof(TText)-1);
		}
	else if(size)
		{
		// NON-UNICODE so convert to UNICODE
		//
		TText* newdata = (TText*)User::AllocL(size * sizeof(TText));
		iPtr.Set(newdata, size, size);
		TInt i;
		TInt actualLength = 0;
		for (i = 0; i < size ; ++i)
			{
			// Lose the spaces (at the cost of having allocated a buffer slightly to big...).
			// This makes the subsequent string searches so much easier and more efficient.
			//
			if (!TChar(ptr[i]).IsSpace())
			    {
    			iPtr[actualLength++] = ptr[i];
    			}
			}
		iPtr.Set(newdata, actualLength, size);
		delete data;
		}

	file.Pop();
	fs.Pop();
    }


// Assemble all the XML validation related strings together.
//
// In essence, a file is considered mesh compatible (has already been updated with mesh tables) if the record
// names in 'dbdef.h::tableXMLMeshMarkerArray' exist in stringsof the form "operation="add">" or 
// "operation="template">"
//
_LIT(KTemplate, "template");
_LIT(KAdd, "add");
_LIT(KRecordInsertFormat, "<%Soperation=\"%S\">");           


TBool CXMLFileParser::CheckMeshTemplateL(TDesC& aTable)
    {
	return (SearchXMLFile(KTemplate, aTable));
    }
    
    
TBool CXMLFileParser::CheckMeshInsertsL(TDesC& aTable)
    {
	return SearchXMLFile(KAdd, aTable);
    }


TBool CXMLFileParser::SearchXMLFile(const TDesC& aOpType, const TDesC& aTable)
	{
	TBuf<MAX_BUFFER_LEN> xmlTableName;
	xmlTableName.Format(KRecordInsertFormat, &aTable, &aOpType);
	
	// No folding or collation.
	//
    return (iPtr.Find(xmlTableName) >= 0);
	}

//------------------------ LinkByTagResolver ---------------------------


LinkByTagResolver* LinkByTagResolver::NewL(CfgFile* aIniData, CFileDump* aLogger)
    {
    LinkByTagResolver* self = LinkByTagResolver::NewLC(aIniData, aLogger);
    CleanupStack::Pop(self);
    return self;
    }

LinkByTagResolver* LinkByTagResolver::NewLC(CfgFile* aIniData, CFileDump* aLogger)
    {
    LinkByTagResolver* self = new(ELeave) LinkByTagResolver(aIniData, aLogger);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

//the ctor...
LinkByTagResolver::LinkByTagResolver(CfgFile* aIniData, CFileDump* aLogger)
    : iLinkByTagRecIdPairs(0)
    , iConfigFile(aIniData)
    , iLogger(aLogger)
    {
    }

//the dtor
LinkByTagResolver::~LinkByTagResolver()
    {
    
    TPtrHashMapIter<HBufC, HBufC > iter(*iLinkByTagRecIdPairs);
    
    for (TInt i = 0; i < iLinkByTagRecIdPairs->Count(); ++i)
        {
        iter.NextValue();
        
        const HBufC* cKey = iter.CurrentKey();
        const HBufC* cValue = iter.CurrentValue();
        
        delete cKey;
        delete cValue;
        
        cKey = NULL;
        cValue = NULL;
        }
    
    iLinkByTagRecIdPairs->Close();
    delete iLinkByTagRecIdPairs;
    iLinkByTagRecIdPairs = NULL;
    
    }

void LinkByTagResolver::ConstructL()
    {
    //the hash function for hashing the table
    const THashFunction32<HBufC > theHashFunc(&DoHash);
    
    //the key identity relation to campare the keys
    const TIdentityRelation<HBufC > theIdentityRelFunc(&AreTheKeysIdentical);
    
    //create the hashmap on the heap
    iLinkByTagRecIdPairs = new(ELeave) RPtrHashMap<HBufC, HBufC >(theHashFunc, theIdentityRelFunc);
    
    FillUpHashMapL();
    }

void LinkByTagResolver::FillUpHashMapL()
    {
    TInt i = 0;
    TPtrC actTable = TPtrC(TablesWithLinkRecords[i]);
    
    
    while (actTable != TPtrC(NO_MORE_RECORDS))
        {
        //first search for template of the given table
        if (iConfigFile->OpenTemplateBlock(actTable))
        //got it, search for the possible link fields in the record
            {
            ExemineFieldsL(i);
            }
        
        //and now for ADD sections...
        if (iConfigFile->OpenAddBlock(actTable))
        //got it, search for the possible link fields in the record
            {
            ExemineFieldsL(i);
            }
		
        actTable.Set(TPtrC(TablesWithLinkRecords[++i]));
        }
        //if everything went OK, we insert two empt string at the end of the RPtrHashMap
        //to be able to return an empty string instead of leave in the case if the given
        //search string not found...
        /*TBuf<0> temp;
        HBufC* key = temp.Alloc();
        HBufC* value = temp.Alloc();*/
        
        HBufC* key = HBufC::New(0);
        HBufC* value = HBufC::New(0);
        iLinkByTagRecIdPairs->Insert(key, value);
    }

void LinkByTagResolver::ExemineFieldsL(const TInt aTableIndex)
    {
    TInt i = 0;
    TBuf<MAX_BUFFER_LEN> resolvedLink;
    HBufC* tempForHashSearch;
    //TBuf<MAX_BUFFER_LEN> tempForHashSearch;
    
    //the fileds form the config file from the section
    TPtrC actField = TPtrC(LinkRecordsArray[aTableIndex][i]);
    //the link setting from the file
    TPtrC setting;
    //the resolved link to Table.RecId format
    TPtrC link(resolvedLink);
                        
    TBool firstRec = ETrue;
    while (firstRec || iConfigFile->StepToNextBlock())
        {
        i = 0;
        actField.Set(TPtrC(LinkRecordsArray[aTableIndex][i]));
        
        while (actField != TPtrC(NO_MORE_RECORDS))
            {
            if (KErrNone == iConfigFile->GetSetting(actField, setting))
            //ok, got a field from the section
                {
                if (SearchForLink(setting))
                    {
                    //just for optimization - if the linking is already in the hashmap
                    //don't resolve it once again... tempForHashSearch should be empty!!!
                    tempForHashSearch = setting.Alloc();
                    CleanupStack::PushL(tempForHashSearch);
                    
                    if (NULL == iLinkByTagRecIdPairs->Find(*tempForHashSearch))
                        {
                        //save the state of the CIniFile object...
                        /**
                           This is needed here because in the following sections the pointers in the
                           CIniFile object will shift and the next call of StepToNextBlock could bring
                           some unexpected results...
                        */
                        TInt aBlockState = iConfigFile->file->BlockState;
                    	TInt aBlockStart = iConfigFile->file->blockStart;
                    	TInt aBlockEnd = iConfigFile->file->blockEnd;
                    	TInt aScanStart = iConfigFile->file->scanStart;
                        TPtr aSection = iConfigFile->file->section;
                	    TPtr aBlock = iConfigFile->file->block;
                	    
                	    resolvedLink.Zero();
                        TRAPD(err,ResolveLinkToRecIdL(setting, resolvedLink));
                        if (err != KErrNone)
                        	{
                        	iLogger->Error(_L("ERROR: [%d] Failed to resolve LinkByTag: [%S] - database will be incomplete"), err, &setting);
                        	User::Leave(err);
                        	}
                        
                        //restore the state...
                        iConfigFile->file->BlockState = aBlockState;
                        iConfigFile->file->blockStart = aBlockStart;
                        iConfigFile->file->blockEnd = aBlockEnd;
                        iConfigFile->file->scanStart = aScanStart;
                        iConfigFile->file->section.Set(aSection);
                        iConfigFile->file->block.Set(aBlock);
                        
                        //these pointers will be deleted in the dtor!!!!!
                        HBufC* key = setting.Alloc();
                        HBufC* value = resolvedLink.Alloc();
                        
                        
                        
                        /*TBuf<MAX_BUFFER_LEN>* key = new(ELeave) TBuf<MAX_BUFFER_LEN>(setting);
                        TBuf<MAX_BUFFER_LEN>* value = new(ELeave) TBuf<MAX_BUFFER_LEN>(linkToResolve);*/
                        
                        iLogger->Msg(_L("LinkByTag resolved: [%S] to [%S]"), &setting, &resolvedLink);
                        
                        //the key and value are on the heap. What here happens is that 
                        //the 2 pointers pointing to the heap cells are copied into the 
                        //RPtrHasMap.
                        iLinkByTagRecIdPairs->Insert(key, value);
                        }
                        
                        CleanupStack::PopAndDestroy(tempForHashSearch);
                        //tempForHashSearch.Zero();
                    }
                }
            actField.Set(TPtrC(LinkRecordsArray[aTableIndex][++i]));
            }
            
        firstRec = EFalse;
        }
    }

/**
 if the param is 'Link.TableName.someId' format returns the position of the first
 '.' which is after the 'Link' word. Else returns 0
*/
TInt LinkByTagResolver::SearchForLink(const TPtrC& aSetting)
    {
    _LIT(linkTag, "Link");
    
    //almost the same code as in DBAccess::SetLinkedRecord
    const TUint KTableColumnSeperator = '.';
	TInt pos = aSetting.Locate(TChar(KTableColumnSeperator));
	
	if (pos != KErrNotFound) 
	    {
	    TBuf<KCommsDbSvrMaxColumnNameLength> buffer = aSetting.Left(pos);
	    
	    if(buffer == TPtrC(linkTag))
	        {
	        return pos;
	        }
	    }
	
	return 0;
    }


void LinkByTagResolver::ResolveLinkToRecIdL(const TPtrC& aSetting, TBuf<MAX_BUFFER_LEN>& aResolvedLink)
    {
    TInt i = SearchForLink(aSetting);
    TPtrC tableName = aSetting.Mid(i+1); //now the tableName is TableName.someId
    
    //the Id setting from the linked record
    TPtrC idTagSetting;
    TPtrC recordId;
    
    _LIT(idTag, "Id");
    _LIT(commdID, "COMMDB_ID ");
    
    //now search for the next '.'
    const TUint KTableColumnSeperator = '.';
	TInt pos = tableName.Locate(TChar(KTableColumnSeperator));
	if (pos != KErrNotFound)
	    {
	    TPtrC idTagFromLink = tableName.Right(tableName.Length()-pos-1);
	    tableName.Set(tableName.Left(pos));
	    
	    //ok, got the table name and the idTag. Now the tableName should be searched for.
	    //As this cannot be a template record we use just the 'OpenAddBlock' method.

	    TBool idTagFound = EFalse;	    
        if (iConfigFile->OpenAddBlock(tableName))
        //got it, search for the 'Id' fields in the record
            {
            TBool firstRec = ETrue;
            
    	    while ( firstRec || (!idTagFound && iConfigFile->StepToNextBlock()) )
                {
                if (KErrNone == iConfigFile->GetSetting(idTag, idTagSetting))
                    {
                    if (idTagFromLink == idTagSetting)
                    //found the record with the given Id field. Read the record id of it
                        {
                        if (KErrNone == iConfigFile->GetSetting(commdID, recordId))
                            {
                            idTagFound = ETrue;
                            
                            aResolvedLink.Append(tableName);
                            aResolvedLink.Append('.');
                            aResolvedLink.Append(recordId);
                            }
                        }
                    }
                firstRec = EFalse;
                }
            }
            
	    if (!idTagFound)
	        {
	        //The linking should be valid
	        User::Leave(KErrNotFound);
	        }
	    }
	else
	    {
	    //The linking should be valid
	    User::Leave(KErrNotFound);
	    }
    }

const HBufC* LinkByTagResolver::ResolvedIdForTagLink(const TPtrC& aLinkingString/*, TDesC& aTheResolvedPair*/)
    {
    HBufC* tempForHashSearch = aLinkingString.Alloc();
    CleanupStack::PushL(tempForHashSearch);
    
    //this will point to an element in the RPtrHashMap, so not needed to push
    //it on the CleanupStack
    HBufC* resolvedLink;
    
    if ( NULL == (resolvedLink = iLinkByTagRecIdPairs->Find(*tempForHashSearch)) )
    /**
    hm... Normaly we should leave here but this would cause the rewrite of the 
    DBAccess::SetLinkedRecord method which is a non leaving method. On the 
    other side TRAPPing would be very expensive as this method is called from 
    a loop...
    So we return the last element of the RPtrHashMap which will be an empty 
    descriptior.
    */
        {
        TPtrHashMapIter<HBufC, HBufC > iter(*iLinkByTagRecIdPairs);
        
        //iterating at the end of the hashMap...
        for (TInt i = 0; i < iLinkByTagRecIdPairs->Count(); ++i)
            {
            iter.NextValue();
            }
            
        //return the empty string...
        //return const_cast<HBufC*>(iter.CurrentKey())->Des();
        return const_cast<HBufC*>(iter.CurrentKey());
        
        //aTheResolvedPair(const_cast<HBufC*>(iter.CurrentKey())->Des());
        }
        
    CleanupStack::PopAndDestroy(tempForHashSearch);
    
    //return resolvedLink->Des();
    return resolvedLink;
    //aTheResolvedPair(resolvedLink->Des());
    }

//EOF
