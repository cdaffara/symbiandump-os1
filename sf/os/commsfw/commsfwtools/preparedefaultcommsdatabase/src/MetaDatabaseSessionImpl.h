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
// Meta-database session declarations
// 
//

/**
 @file 
*/

#if (! defined METADATABASESSIONIMPL_H)
#define        METADATABASESSIONIMPL_H 

#include <e32property.h>

// Forward declarations
namespace CommsDatInternal
{
class CCDMapper;
class CCDNotifier;
}

namespace CommsDat
{

using namespace CommsDatInternal;



NONSHARABLE_CLASS(CMDBSessionImpl) : public CBase
/*
CommsDat-specific implementation of a meta database session

@internalComponent
*/
{
public : 
    
    /* Constructor/Destructor
   
    @internalComponent
    */ 
    
    CMDBSessionImpl(CMDBSession& aOwner);
    ~CMDBSessionImpl();
    
    
    /*
    Open a session with the storage server and compare data versions
        
    The MetaDatabase always attempts to support the required version.  If the versions match,
    KErrNone will be returned.  If the versions are not the same, but the requested version
    is supported, a warning code will be returned. Otherwise an error will be 
    returned.  
    
    @internalComponent
    */
    TInt ConstructL( TVersion aRequiredVersion );  

    
    /* 
    Close session with storage server.
    
    @internalComponent
    */
    void Close();



    //
    // Accessors
    
    /*
    Lookup latest data format version
    
    @internalComponent
    */
    TVersion LatestVersion();
    
    
    /*
    Lookup data format version currently in use by client
    
    @internalComponent
    */
    TVersion VersionInUse();
    
    
    /*
    Lookup data format version currently in use by client
    Return ETrue if it is the latest version. 
    
    @internalComponent
    */
    TBool UsingLatestVersion(); 

    /*
	Non-leaving method for creating notifier object.

    @internalComponent
    */
	TInt CreateNotifier();
  

    
    /*
    Notifier object currently in use.
    
    @internalComponent
    */
    CCDNotifier* NotifierL();
      
    
    /*
    Storage class to store Comms data.  First call creates the object
        
    @internalComponent
    */
    CRepository* StorageL();
  
    
    
    //
    // Transactionsd with storage server
    //
    
    /* 
    Start transaction with storage server if available
    
    @internalComponent
    */
    TBool MaybeOpenTransactionL();
    
    
    /* 
    Query if transaction is already underway in this session
    
    @internalComponent
    */
    TBool IsInTransaction();
   
   
    /* 
    Rollback transaction restoring database state to before transaction started.
    
    @internalComponent
    */
    void RollbackTransactionL();
    

    /* 
    Commit transaction if outstanding and owned by caller
    
    @internalComponent
    */
    TInt MaybeCommitTransactionL(TBool aOwnLock, CMDBElement* aDoSync, TInt aErr);
      
        
    /* 
    Set flags in attribute mask as requested by client
    
    @internalComponent
    */   
    void SetAttributeMask(TMDBAttributeFlags aAttributeMask);
    
    /* 
    Check flags in attribute mask as requested by client
    
    @internalComponent
    */   
    TBool IsSetAttributeMask(TMDBAttributeFlags aAttributeMask);

    /* 
    Clear flags from attribute mask as requested by client
    
    @internalComponent
    */   
    void ClearAttributeMask(TMDBAttributeFlags aAttributeMask);

    
    /* 
    Retrieve attribute mask relevant to writing the db
    
    @internalComponent
    */
    TMDBAttributeFlags GetWriteAttributeMask();
    
    /* 
    Retrieve attribute mask relevant to reading the db
    
    @internalComponent
    */
    TMDBAttributeFlags GetReadAttributeMask();
    
    /* 
	Decide whether to create a new node in the database
    	and then create it if necessary
	A node means a new table, new column or new record
	 
    @internalComponent
    */
    //void MaybeCreateNodeL(CMDBElement& aElement, TDesc& aName = KNullDesC);    
    void MaybeCreateNodeL(CMDBElement& aElement);    
    
	/* 
    Find the element in the DB if it exists and assign its attributes

    @internalComponent
    */
    TInt LoadElementAttributesL(TMDBElementId& aElementId);
    
    TInt LoadNodeAttributesL(TMDBElementId& aElementId);
  
	/* 
    Locate a new node in the database
   	A node means a new table, new column or new record

    @internalComponent
    */
 	void SetNodeIdL(TMDBElementId& aElementId, TMDBElementId aMask, TMDBElementId aInitialId, TMDBElementId aMaxId, TInt aMaxVal);


	/* 
    Create a new placeholder in the database
   	A placeholder reserves space for a new table, new column or new record
    A placeholder never has attributes set
    tableName is typically KNullDesC, but should be set to something readable for a generic record
    @internalComponent
    */
 	void CreatePlaceholderL(TMDBElementId& aElementId, TMDBElementId aNodeTypeMask, TDesC& aTableName);
 	
 	//first try with an overload...
 	void CreatePlaceholderL(TMDBElementId& aElementId, TMDBElementId aNodeTypeMask, CMDBElement& aElement);
    
    /* 
    Create a new node in the database
   	A node identifies a new table, new column or new record
    It carries type information about the node and identifies default attributes for the node and its children
        Table - table id
        Column - column id, column name, column type
        Record - record id

    A node is used as the primary key to identify an element in the database
    @internalComponent
    */
 	void CreateNodeL(TMDBElementId& aElementId, TMDBElementId aMask, CMDBElement& aElement, SGenericRecordTypeInfo* aFieldTypeInfo = NULL);

	/* 
    If appropriate, modify a node in the database and its placeholder
   	A node identifies a table, column or record

    @internalComponent
    */
    TInt MaybeModifyNodeL(TMDBElementId& aElementId);   


	/* 
    If appropriate, delete a node in the database and its placeholder
   	A node identifies a table, column or record

    @internalComponent
    */
 	TInt MaybeDeleteNodeL(CMDBElement* aElement, TMDBElementId& aElementId);


    /* 
    Check for match between two arrays of element ids
    Utility function used by metadatabase and visitors

    @internalComponent
    */
    TInt FindMatchL(RArray<TUint32>& aCandidates, RArray<TUint32>& aMatches, TUint32 aIdMask = 0);

    void EndModification(){iInModification = EFalse;}

	void ReadAllowedL(TMDBElementId aElementId);

	void WriteAllowedL(TMDBElementId aElementId);
	
	void DeleteAllowedL(TMDBElementId aElementId);
	
	TInt GatherParentAttributesL(TMDBElementId& aElementId);
	
	TInt FindElementInDatabaseL(TMDBElementId& aElementId);

public:

	// Need to call back to owner to make recursive db calls for nested elements
    CMDBSession&                    iOwner;
    
    TBool                           iMapping;

private:
    
    void EstablishVersionL(TVersion aVersionToCheck);   

    /* Flag to check to see if mapping is required
    Worth the cost of a flag here because checking it is cheaper than calculating
    the version all the time*/
    TBool                           iUsingLatestVersion; 
    
    CCDNotifier*                    iNotifier;
    
    TRequestStatus                  iStatusIndicator;
   
    // Storage server interface
    CRepository*                    iCommsStorage;

    TVersion                        iClientsDataSetVersion;   
    
    TBool                           iInTransaction;
    TBool                           iInModification;
    
    TMDBAttributeFlags              iReadAttributeMask;
    TMDBAttributeFlags              iWriteAttributeMask;

	RProperty						iCommitSeqProperty;
};


    

} // end namespace CommsDatInternal

#endif //METADATABASESSIONIMPL_H
