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

#include <e32hashtab.h>
#include "CIniFile.h"
#include "CXMLFile.h"

#ifndef INPUT_H
#define INPUT_H

class LinkByTagResolver;
class CFileDump;

class CfgFile : public CBase
/**
@internalComponent
*/
	{
	friend class LinkByTagResolver;
	public:
		CfgFile():file(NULL){}
		~CfgFile()	{CloseConfigFile();}
		
		TInt OpenConfigFile(TDesC &fIn);
		void CloseConfigFile();
		
		TInt OpenSetBlock(const TDesC &aSection)
			{ return file->OpenSetBlock(aSection); }
		TInt OpenTemplateBlock(const TDesC &aSection)
			{ return file->OpenTemplateBlock(aSection); }
		TInt OpenAddBlock(const TDesC &aSection)
			{ return file->OpenAddBlock(aSection); }
		TInt GetSetting(const TDesC &aValue, TPtrC &aSetting)
			{ return file->GetSetting(aValue, aSetting); }
		TInt StepToNextBlock()
			{ return file->StepToNextBlock(); }
	private:
		CIniData* file;
	};


NONSHARABLE_CLASS(LinkByTagResolver) : public CBase
    {
    public:
        static LinkByTagResolver* NewL(CfgFile* aIniData, CFileDump* aLogger);
        static LinkByTagResolver* NewLC(CfgFile* aIniData, CFileDump* aLogger);
        virtual ~LinkByTagResolver();
        
        /**
         returns the recordId for the given Link.Table.Id format.
         param: aTableName - the given linking by tag
         return value: -1 if the linking cannot be found
                       ther record number if the linking is found
        */
        const HBufC* ResolvedIdForTagLink(const TPtrC& aLinkingString/*, TDesC& aTheResolvedPair*/);
        
        
    protected:
        LinkByTagResolver(CfgFile* aIniData, CFileDump* aLogger);
        void ConstructL();
    
    private:
        /**
         Called from ConstructL to fillup the hashmap from the config file
        */
        void FillUpHashMapL();
        void ExemineFieldsL(const TInt aTableIndex);
        TInt SearchForLink(const TPtrC& aSetting);
        
        //this function tries to resolve the linking by tag type linking.
        //If it cannot resolve the link it leaves!!!!
        void ResolveLinkToRecIdL(const TPtrC& aSetting, TBuf<MAX_BUFFER_LEN>& aResolvedLink);
        
        //The class owns this pointer so this has to be deleted!!
        RPtrHashMap<HBufC, HBufC >*   iLinkByTagRecIdPairs;
        
        /**
        This class USES this pointer and doesn't have the right to modify it...
        As the ownership is not here we don't have to take care about deleting it.
        
        Unfortunetaly these cannot be const pointers because the iConfigFile will be changed -
        the pointers inside the object...
        And the iLogger doesn't have any const function...
        */
        CfgFile*      iConfigFile;
        CFileDump*    iLogger;
    };


inline TInt CfgFile::OpenConfigFile(TDesC &fIn)
	{
	TRAPD(ret, file = CIniData::NewL(fIn));
	if (ret == KErrNone)
		{
		if (file)
			{
			return ETrue;
			}
		
		}
	return EFalse;
	}

inline void CfgFile::CloseConfigFile()
	{
	if (file)
		{
		delete file;
		file = NULL;
		}
	}

class CXMLDatabase;


class XMLFile : public CBase
/**
@internalComponent
*/
	{
	public:
		XMLFile():file(NULL){}
		~XMLFile()	{CloseConfigFile();	}
		
		TInt OpenConfigFile(RFs& aFs, TDesC &fIn, CXMLDatabase* aXmlDb, 
		                    TBool aForceXMLProcessing,
		                    TBool aAppendMode);
		void CloseConfigFile();
		TInt GetSetting(const TInt aEntryIndex, const TDesC& aParamName, TPtrC &aParamValue)
			{ return file->GetSetting(aEntryIndex, aParamName, aParamValue); }
		TInt GetStartingIndex(const TDesC& aTableName)
			{ return file->GetStartingIndex(aTableName); }
		TInt GetLastIndex(const TDesC& aTableName)
			{ return file->GetLastIndex(aTableName); }
		TInt GetEntryNumberParameters(const TInt aEntryIndex)
			{ return file->GetEntryNumberParameters(aEntryIndex); }
		const TBuf<MAX_BUFFER_LEN>& GetOperation(const TInt aEntryIndex)
			{ return file->GetOperation(aEntryIndex); }
		TInt GetElementRecordID(const TInt aEntryIndex) const
		    { return file->GetElementRecordID(aEntryIndex); }
		
	private:
		CXMLData* file;
	};


inline TInt XMLFile::OpenConfigFile(RFs& aFs, TDesC &fIn, CXMLDatabase* aXmlDb, 
                                    TBool aForceXMLProcessing,
                                    TBool aAppendMode)
	{
	TRAPD(ret, file = CXMLData::NewL(aFs,fIn,aXmlDb,aForceXMLProcessing, aAppendMode));
	if (ret == KErrNone)
		{
		if (file)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

inline void XMLFile::CloseConfigFile()
	{
	if (file)
		{
		delete file;
		file = NULL;
		}
	}


#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

class CFileDump;

/**
  @internalComponent
*/
class CInputFileParser : public CBase
    {
    public:
        virtual ~CInputFileParser() {}

        static CInputFileParser* FactoryLC(TBool aIsXML, TDesC& aFile, CFileDump* aLog);

                TBool IsMeshCompatibleL();
                
        virtual void  InitialiseL()                     = 0;
        virtual TBool CheckMeshTemplateL(TDesC &aTable) = 0;
        virtual TBool CheckMeshInsertsL(TDesC &aTable)  = 0;
        
    protected:
        CInputFileParser(TDesC& aFile, CFileDump* aLog);
        void Exit(TInt aErr);
        
              TDesC&     iFile;
              CFileDump* iLog;
        const TText**    iTable;
    };


/**
  @internalComponent
*/
class CCfgFileParser : public CInputFileParser
    {
    friend class CInputFileParser;
    public:
        virtual ~CCfgFileParser();
        
        virtual void  InitialiseL();
        virtual TBool CheckMeshTemplateL(TDesC &aTable);
        virtual TBool CheckMeshInsertsL(TDesC &aTable);
        
    protected:
        CCfgFileParser(TDesC& aFile, CFileDump* aLog);
        
    private:
        CfgFile iCfg;
    };
    
/**
  Lightweight XML file parser, read a file in, search it as text content...
  
  This class shamelessly copies from 'CIniData'.
  
  @internalComponent
*/
class CXMLFileParser : public CInputFileParser
    {
    friend class CInputFileParser;
    public:
        virtual ~CXMLFileParser();

        virtual void  InitialiseL();
        virtual TBool CheckMeshTemplateL(TDesC &aTable);
        virtual TBool CheckMeshInsertsL(TDesC &aTable);

    protected:
        CXMLFileParser(TDesC& aFile, CFileDump* aLog);
		
	private:
        TBool SearchXMLFile(const TDesC& aOpType, const TDesC& aTable);
        
        void OpenXMLFileL();
        
    	TPtr iPtr;
    };

#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY

#endif
