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


#ifndef __CXMLCONTENTHANDLER_H__
#define __CXMLCONTENTHANDLER_H__

#include <xml/contenthandler.h>
#include <e32hashtab.h>


#define MAX_BUFFER_LEN  256 // max length of path/filename/column/table name


// Forward class declarations
class RFile;
class CXMLDatabase;
class CXMLTableEntry;


NONSHARABLE_CLASS(CXMLContentHandler) : public CBase,
                                        public Xml::MContentHandler
	{
	public:
		
		static CXMLContentHandler* NewL( CXMLDatabase* aXmlDb,
		                                 const TBool aForceFlag,
		                                 const TBool aAppendFlag );
		virtual ~CXMLContentHandler();
		
		// From MContentHandler
		void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
		void OnEndDocumentL(TInt aErrorCode);
		void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
		void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
		void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
		void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
		void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
		void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
		void OnSkippedEntityL(const RString& aName, TInt aErrorCode) ;
		void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
		void OnExtensionL(const RString& aData, TInt aToken, TInt aErrorCode);
		void OnError(TInt aErrorCode);
		TAny* GetExtendedInterface(const TInt32 aUid);
		
		
		// Set the XML database to use for storing the parsed elements
		inline void SetXMLDatabase(CXMLDatabase* aXmlDb)
			{
			iXmlDb = aXmlDb;
			}
		
		// Set the level of strictness for processing XML files with invalid
		// table entry links
		inline void ForceXMLProcessing(const TBool aFlag)
			{
			iForceXMLProcessing = aFlag;
			}
		
		/**
		This parameter is needed when resolving the COMMDB_IDs.
		If this is true th resolve should be done from the DB otherwise
		from the XML db.
		*/
		inline void SetAppendMode(const TBool aAppendFlag)
		    {
		    isInAppendMode = aAppendFlag;
		    }
		
		TBool ModifyTableEntryReferencesL();
		void  LogTableEntries();
		
	private:
	    //ctors
	    CXMLContentHandler(CXMLDatabase* aXmlDb,
		                   const TBool aForceFlag,
		                   const TBool aAppendFlag);
	    void ConstructL();
	    
		void  SetTableIDs();
		void  RemoveNameParams();
		TBool ResolveTableEntryReferences(
		                  CXMLTableEntry*       entry,
        			const TInt                  paramIndex,
        			const TBuf<MAX_BUFFER_LEN>& refEntryName,
        			const TBuf<MAX_BUFFER_LEN>& refEntryTable,
        			const TBuf<MAX_BUFFER_LEN>& orgEntryTable);
        			
        
        TBool ResolveRefsFromDbL(
                          CXMLTableEntry*                   entry,
        			const TInt                              paramIndex,
        			const TPtrC&                            refEntryName,
        			const TPtrC&                            refEntryTable,
        			const TPtrC&                            orgEntryTable);
        			
        void FillUpHashMapL(RHashMap<TPtrC, TMDBElementId>& aHashMap);
        
        void SetCfgLinkingFromXMLFile(const TDesC&    aRefTable,
                                      const TInt      aParamIndex,
                                            CXMLTableEntry* aEntry);
        
        void SetRecordIDsL();
        
        bool IsTableKnown(const TDesC&    aRefTable);
        
		
	public:
		static const TInt KExpectedLeaveCode;
		
	private:
	    typedef enum {EXMLContentStateFirst = 0, 
	                  ENone                 = EXMLContentStateFirst,
	                  EInDeprecatedTable,
	                  EInTable,
	                  EInRecord,            // 'InRecord' => 'InTable' too (it's a stack).
	                  EInParam,             // 'InParam'  => 'InRecord too (it's a stack).
	                  EXMLContentStateLast  = EInRecord} TXMLContentState;
	    
		TBool                iLeaveOnStartElement;
		TInt                 iNumElements;
		TInt                 iNumSkippedEntities;
		TInt                 iNumPrefixMappings;
		TInt                 iNumPrefixUnmappings;
		TInt                 iError;
		CXMLDatabase*        iXmlDb;
		TBool                iForceXMLProcessing;
		TBool                isInAppendMode;
		TXMLContentState     iXMLContentState;
		TBuf<MAX_BUFFER_LEN> iCurrentRecordName;        // Is actually just iCurrentTableName, without the "Table" suffix.
		TBuf<MAX_BUFFER_LEN> iCurrentTableName;
		TBuf<MAX_BUFFER_LEN> iCurrentParameterValue;
		TBuf<MAX_BUFFER_LEN> iCurrentParameter;
		
		//for reading the database
		CMDBSession*         iCmdbSession;
		
    	RHashMap<TPtrC, TMDBElementId>* iTableNameIDpairs;
		
	};


NONSHARABLE_CLASS(CXMLTableEntry) : public CBase
	{
	
	public:
		
    	enum TLinkType
        /**
        Specifies the type of the link
        @publishedAll
        @released
        */
    	{
    	EResolve_XML_Ref,           //The link is tableName.RecordName
    	ERecord_ID_Reference,       //The link is tableName.RecordID
    	ESkip_XML__Ref,             //The link is tableName.-1
    	ENot_A_Link                 //The parameter is not a link
    	};
    	
		// Constructor with parameter the name of the table this
		// table entry belongs to
		static CXMLTableEntry* NewL(const TDesC& aTableName);
		virtual ~CXMLTableEntry();
		
	public:
		TInt GetNumberParameters() const
			{
			return paramName.Count();
			}
        
		TInt GetRecordID() const
		    {
		    return iRecordID;
		    }
		
		void SetRecordID(TInt aID)
		    {
		    iRecordID = aID;
		    }
		inline void SetDeleteFlag(const TBool aDeleteFlag)
		    {
		    iDeleteFlag = aDeleteFlag;
		    }
		inline const TBool GetDeleteFlag() const
		    {
		    return iDeleteFlag;
		    }
		void AddParameterL(const TBuf<MAX_BUFFER_LEN>& aParamName, const TBuf<MAX_BUFFER_LEN>& aParamValue);
		void SetParameterValue(const TInt aIndex, const TBuf<MAX_BUFFER_LEN>& aParamValue);
		void RemoveParameter(const TInt aIndex);
		void RenameParameter(const TInt aIndex, const TBuf<MAX_BUFFER_LEN>& aNewName);
		
		void SetEntryId(const TInt aId)
			{
			entryId = aId;
			}
		void SetOperation(const TBuf<MAX_BUFFER_LEN>& aOperation)
			{
			operation.Copy(aOperation);
			}
		const TBuf<MAX_BUFFER_LEN>& GetParameterName(const TUint aIndex) const
			{
			return *paramName[aIndex];
			}
		const TBuf<MAX_BUFFER_LEN>& GetParameterValue(const TUint aIndex) const
			{
			return *paramValue[aIndex];
			}
	    inline const TLinkType& GetLinkType(const TUint aIndex) const
	        {
	        return iLinkType[aIndex];
	        }
	    inline void SetLinkType(const TLinkType& aLinkType)
	        {
	        iLinkType.Append(aLinkType);
	        }
	    inline void ModifyLinkType(const TUint aPos, const TLinkType& aLinkType)
	        {
	        iLinkType[aPos] = aLinkType;
	        }
		const TBuf<MAX_BUFFER_LEN>& GetTableName() const
			{
			return tableName;
			}
		const TBuf<MAX_BUFFER_LEN>& GetOperation() const
			{
			return operation;
			}

		TInt GetEntryId() const
			{
			return entryId;
			}
		
	protected:
		CXMLTableEntry();
		
	private:
		void ConstructL(const TDesC& aTableName);
		
	private:
		TInt entryId;
		TInt iRecordID;
		TBuf<MAX_BUFFER_LEN>                  tableName;
		RPointerArray< TBuf<MAX_BUFFER_LEN> > paramName;
		RPointerArray< TBuf<MAX_BUFFER_LEN> > paramValue;
		//as only interger values are stored here it's not important to have
		//RPointerArray
		RArray<TLinkType>                     iLinkType;
		TBuf<MAX_BUFFER_LEN>                  operation;
		TBool                                 iDeleteFlag;
	};

NONSHARABLE_CLASS(CXMLDatabase) : public CBase
	{
	
public:
	// Constructor with parameter the name of the table this
	// table entry belongs to
	static CXMLDatabase* NewL();
	virtual ~CXMLDatabase();
	
public:
	// Functions to manipulate the database elements
	void AddTableEntryL(CXMLTableEntry* aEntry);
	CXMLTableEntry* GetTableEntry(const TInt aIndex);
	TInt GetNumberTableEntries() const;
	CXMLTableEntry* GetLastTableEntry();
	void CheckDeletedElems();
	
	
protected:
	CXMLDatabase();
	
private:
    RPointerArray<CXMLTableEntry> tableEntries;
	};

#endif
