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
// Meta-database declarations for use in storing comms data
// 
//

/**
 @file
 @publishedAll
 @released
*/

#if (!defined METADATABASE_H)
#define       METADATABASE_H



// NetMeta headers
#include <comms-infras/metadata.h>
#include <comms-infras/metatype.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <metadatabase_partner.h>
#endif

// CommsDat headers
#include <commsdat.h>



/**@plaintext

This file contains :

1/  Class for expressing a session with the storage server

			CMDBSession

		This class contains functions to create and close a session, to set the dataset version
		and to request database actions such as locking for a transaction.

2/  Generic database functionality interface

			MMetaDatabase

		This class uses a CMDBSession object


3/  General base class that inherits from MMetaDatabase to express any database element

			CMDBElement

		This class contains the ElementId for each MetaDatabase object
		An element id locates stored data in the comms database.


4/  Data classes that each inherit from CMDBElement

	a) Fields

			CMDBField<T>

		where T is a basic type value of the field (TInt, TDes, TBool etc)
		
		This class derives from

			CMDBElement

		to implement the thin template idiom


			CMDBRecordLink<T>

		where T is a record of a particular type.  This class expresses a soft-link
		from a field in one record to a record of a different type (i.e. in another table).
		The TMDBElementId value stored in this field in the repository is the id of an instance 
		of the record of type T

		This class derives from

			CMDBRecordLinkBase

		to implement the thin template idiom


	b) Field Containers


			CCDRecord

		base class for record containing specific list of fields - see CommsDatTypesv1_1.h


			CMDBRecordSet<T>

		an array of records where T is the record type.  This can represent a Table in a database.




5/  Other support classes and type definitions

			TMDBElementId

		The TMDBElementId locates a data object or set of data objects in the database.

		A TMDBElementId expresses

		<TableId><ColumnId><RecordId><Attributes>

		A TMDBElementId has a different format depending on the data type it expresses

			Table       <TableId><0><0>
			Column      <TableId><ColumnId><0>

			Record      <TableId><0><RecordId>
			Field       <TAbleId><ColumnId><RecordId>


			TMDBAttributeFlags

		Attribute settings for any storable element.
		Attributes express access control for data in the database

*/



namespace CommsDat
{

using namespace Meta;


// Private to implementation
//@internalTechnology
//@released
class CMDBSessionImpl;

// Forward declarations
class MMetaDatabase;
class CMDBElement;

/**
Attribute flags for an element in the Meta Database
Attributes describe access rights to the data in the database
@publishedAll
@released
*/
typedef TUint32         TMDBAttributeFlags;

/**
The identifier for any entry in the MetaDatabase.
It can specify
	<TableId><ColumnId><RecordId>
This identifies 
    The data location in the database
    The meaning of the data within in the data schema
    The type of the data

@publishedAll
@released
*/
typedef TUint32         TMDBElementId;


//
// COMMSDAT RECORD BASE CLASS
// Contains fields common to all records
//

	// Element Type Ids

    /** 
    Field will contain a user-defined numeric tag for a record.
    Useful for user searches and cheaper to search on a numeric tag than a string name.
    Can be null.
    This tag is not automatically connected with the record id itself,
    which is contained within the ElementId for the record and not in any field.

    @publishedAll
    @released
    */
	const TMDBElementId KCDTIdRecordTag			 = 0x00010000; 

    /**
    Field will contain a user-defined string tag for a record. 
    This is used to uniquely identify business level information within a record.
    
    This field is useful for user searches, however it should be noted that it would be more efficient to use the KCDTIdRecordTag field instead
    as it is quicker and cheaper to search for a number than for a string.

    @publishedAll
    @released
    */
    const TMDBElementId KCDTIdRecordName		 = 0x00020000; 


	// Element Type Names
	#define	KCDTypeNameRecordTag				_S("Id")
	#define	KCDTypeNameRecordName		        _S("Name")
    
    
class CMDBSession : public CBase
/**
A session with the underlying storage server
@publishedAll
@released
*/
{
public:

	/**
	Open a session with the storage server and establish data format version in use
	CommsDat always attempts to support the required version of the client-facing data set.
    The client must carefully choose the dataset version as it affects binary compatibility

    @params aRequiredVersion  
            If the requested version is deprecated but still supported, mapping will be performed 
            within CommsDat to maintain Data and Binary compatibility for the client
            If the requested version is the latest version, no mapping will be performed
            and deprecated fields will be ignored 
            A client that needs to maintain BC must use a specific version
            A client that always wants to use the latest dataset can start the session with KCDLatestVersion
            
    @leave  KErrUnsupported If an old unsupported version is requested
    @leave  System-wide error codes if there is an unexpected error
    
    @pre    None
    @post   on success a session has been created and initialised

	*/
	IMPORT_C static CMDBSession* NewL(TVersion aRequiredVersion);

	/**
	As NewL and adds Session to the CleanupStack

	*/
	IMPORT_C static CMDBSession* NewLC(TVersion aRequiredVersion);

	/**
	Virtual Destructor

	*/
	IMPORT_C ~CMDBSession();


	/**
	Close session with storage server.
	@publishedAll
	@deprecated v9.1 Destructor is all that is required
	*/
	IMPORT_C void Close();


	/**
	Returns the KCDCurrentVersion constant which maps to the KCDVersion1_1 
	constant for backward compatibility reason. Please avoid using this method 
	when creating a CommsDat session object and use explicit schema version 
	instead.
	
	@pre   None
    @post  None
    
	@publishedAll
	@deprecated

	*/
	IMPORT_C static TVersion LatestVersion();


	/**
	Lookup data format version currently in use by client


	*/
	IMPORT_C TVersion VersionInUse();


	/**
	Open Transaction with storage server and obtain write lock - if not already set

    @capability WriteDeviceData

    @leave	Will leave with KErrNotAvailable if Database write locked by another process
	@leave  Will report KErrAlreadyInTransaction if transaction this session is already in a transaction
    @leave  Will leave with a system-wide error code if an error occurs during commit

	@pre    None
    @post   on success the session has exclusive write access to the database


	*/
	IMPORT_C void OpenTransactionL();


	/**
	Commit Transaction in database.

    @leave	Will leave with KErrNotFound if not in transaction
    @leave  Will leave with a system-wide error code if an error occurs during commit

	@pre    None

	@post   On success the database is updated with data added, modified or deleted during the transaction
    @post   On failure, any open data containers may not be in sync with the database and should be discarded or reloaded
	*/
	IMPORT_C void CommitTransactionL();


	/**
	Cancel Transaction with database and rollback all associated changes

    @leave	Will fail with KErrNotFound if not in transaction

    @pre    None - though for correct usage, ensure a transaction is already open
	@post   Any open data containers may not be in sync with the database and should be discarded or reloaded
	*/
	IMPORT_C void RollbackTransactionL();

   /**
	Query if this session is in a transaction with the database
	Return code will indicate
		Not in transaction,
		Already in transaction for this session,
		Write-lock not available.  Another session has it.
    @pre None
	*/
	IMPORT_C TInt IsInTransaction();

	
    /**
	Set attributes to ignore for all database operations during this session 

    @params aAttributeFlags indicates the requested access level to stored data.
            Attributes set by this call will be ignored as restrictions in all database interactions,
	        providing the client has enough platform security capabilities to back up the request
	
    @pre None
	*/
	IMPORT_C void SetAttributeMask(TMDBAttributeFlags aAttributeFlags);

	
	/**
	Check flags in attribute mask
    @params aAttributeFlags indicates the requested access level to stored data
            Attribute flags that have been set will be ignored as restrictions in all database interactions, 
            providing the client has enough platform security capabilities to back up the request

    @return ETrue if the flag is set in the session mask 
            This means that the attribute is ignored in database interactions for this session

    @return EFalse if the flag is not set in the session mask 
            This means that the attribute is obeyed in all database interactions for this session

    @pre None
	
	*/
	IMPORT_C TBool IsSetAttributeMask(TMDBAttributeFlags aAttributeFlags);

    
    /**
	Clear attribute mask settings, indicating required access level to stored data
	Clear flags in the attribute mask for this session
    
        Flags indicate the requested access level to stored data
	    
        Attribute flags that have been set in this function will be cleared from the session's 
        attribute mask and will be obeyed as restrictions in all database interactions, 
        providing the client has enough platform security capabilities to back up the request
        
    @pre None
	
	
	*/
	IMPORT_C void ClearAttributeMask(TMDBAttributeFlags aAttributeFlags);



	/**
	Cancel all notification requests that are outstanding from calls to 
        MMetaDatabase::RequestNotification
    Individual requests can be cancelled via MMetaDatabase::CancelNotify.

    @return KErrNone on success or a system-wide error code
    
    @pre  None

	*/
	IMPORT_C TInt CancelAllNotifications();


    /**
	Attempt to create publish/subscribe properties if not already defined.
    
        This function is normally called as a setup activity by System components

    @capability WriteDeviceData - since these properties are created in KUidSystemCategory category.  

    @return KErrNone on success.  Otherwise a system-wide error code
    	
    @pre None

	*/	
	IMPORT_C TInt PublishProperties();

private:

    TInt ConstructL( TVersion aRequiredVersion );

	CMDBSession();

private:

	CMDBSessionImpl* iMDBSessionImpl;
	
	friend class MMetaDatabase;
	friend class CommsDatSchema;
};





class MMetaDatabase : public Meta::MMetaData
/**
Interface for interaction with a database server
Inherited by each data set type that can be stored
@publishedAll
@released
*/
{
public:

	IMPORT_C virtual ~MMetaDatabase();


	/**
	LoadL - Load this container with data from the database
		
        Any data in the container will be discarded 

        Individual fields may legitimately be NULL in the database, but if no data at all
        is found for an entire table or record, LoadL will leave with 
            KErrNotFound

        The client can request to ignore access control flags by setting the attribute mask
            CMDBSession::SetAttributeMask()
		However, only fields that the client has capabilities to write will be stored in the database
		A request to read data from an unauthorised area of the database will cause the function to leave.with 
            KErrPermissionDenied
        
        In a record, fields in an unauthorised column may be silently ignored and returned as NULL
        
        May also leave with KErrNoMemory or other general error codes
	
        On failure, the container should be discarded or reloaded before it is used again

    @pre

        Ensure the ElementId for this container fully identifies the target data in the database
            <Table><Column><Record>
        Ensure that the session object has been initialised with the correct dataset version
        Ensure that the session object has appropriate access control attributes to manage the target data in the database

	*/
	IMPORT_C void LoadL(CMDBSession& aSession);


	/**
	FindL - Find and Load the data set(s) that match all the data values entered by the 
    client in this container
	     
        Client can request to ignore access control flags by setting the attribute mask,
            CMDBSession::SetAttributeMask()
		However will not load data that the client is not authorised to see (by platsec)
        even if the client has sets the appropriate attribute mask

		Will return
			ETrue on success
			EFalse if no data found that the client is authorised to view
            
        May also leave with KErrGeneral or other general error codes.

        On failure, the container should be discarded or repopulated before it is used again
		
		Please note - a case insensitive search is done in the case of text based fields
		
    @pre

        Populate this container where necessary with valid data to match during the find operation
        Ensure the ElementId for this container correcty identifies the target table in the database
            <Table><Column>
        Ensure that the session object has been initialised with the correct dataset version
        Ensure that the session object has appropriate access control attributes to manage the target data in the database
	 */
	IMPORT_C TBool FindL(CMDBSession& aSession);


	/**
	Refresh all fields in the container that have not been changed by the client

		The client can request to ignore access control flags by setting the attribute mask,
            CMDBSession::SetAttributeMask()
		However, only data that the client has capabilities to view will be loaded
		Unauthorised data elements will be silently ignored and will remain NULL

		If no data at all is found the functon will leave with 
			KErrNotFound

		If fields are not accessible for reading the function may leave with 
            KErrPermissionDenied
	    
        May also leave with other general error codes if there are unexpected 
        problems (e.g. KErrNoMemory when out of memory)

        On failure, the container should be discarded or reloaded before it is used again

	@pre

        Ensure the ElementId for this container fully identifies the target data in the database
            <Table><Column><Record>
        Ensure that the session object has been initialised with the correct dataset version
        Ensure that the session object has appropriate access control attributes to manage the target data in the database
	*/
	IMPORT_C void RefreshL(CMDBSession& aSession);


	/**
	StoreL - Store all fields in the database that have been changed in this container and 
    create new records where necessary.

        StoreL is atomic for this container.  An internal transaction is used even if 
        no overall transaction is set by client.
            All changed fields in the element are stored in the database on success.  
            No fields or records are stored in the database if the function leaves

        If a client has an open transaction in the session, the stored data will be available to this session as 
        soon as StoreL completes but will not be visible to other sessions until the transaction completes successfully

		The client can request to ignore access control flags by setting the attribute mask
            CMDBSession::SetAttributeMask()
		However, only fields that the client has capabilities to write will be stored in the database
		A request to modify data in an unauthorised area of the database will cause the function to leave.with 
            KErrPermissionDenied
        
    	If a field already exists, StoreL will leave with
	        KErrAlreadyExists

        May also leave with other general error codes if there are unexpected 
        problems (e.g. KErrNoMemory when out of memory)

		Store will be atomic - all fields stored after success.  No fields stored if the function leaves
		
        If StoreL fails or the later commit is not successful, The data in the container will not
        match the data in the database.  

        On failure, the container should be discarded or repopulated before it is used again

	@pre

        Populate this container with valid data for storage
        Ensure the ElementId for this container fully identifies the target location in the database
            <Table><Column><Record>
        Ensure that the session object has been initialised with the correct dataset version
        Ensure that the session object has appropriate access control attributes to manage the target data in the database
        
	*/
	IMPORT_C void StoreL(CMDBSession& aSession);


	/**
	Modify all fields in the database that have been changed in this container by the caller
	
        Modification is atomic for this container.  It uses an internal transaction even if 
        no overall transaction set by client.
            All changed fields in the element are modified in database on success.  
            No fields are altered in the database if the function leaves

        Client can request to ignore access control flags by setting the attribute mask
            CMDBSession::SetAttributeMask()
		However, only fields that the client has capabilities to alter will be modified.in the database
		A request to modify data in an unauthorised area will cause the function to leave.with 
            KErrPermissionDenied
		

		May also leave with other general error codes if there are unexpected 
        problems (e.g. KErrNoMemory when out of memory)

        On failure, the container should be discarded or repopulated before it is used again

    @pre

        Populate this container with valid data for storage
        Ensure the ElementId for this container fully identifies the target data in the database
            <Table><Column><Record>
        Ensure that the session object has been initialised with the correct dataset version
        Ensure that the session object has appropriate access control attributes to manage the target data in the database
	*/
	IMPORT_C void ModifyL(CMDBSession& aSession);


	/**
	Delete fields from the database.

		Client can request to ignore access control flags by setting the attribute mask,
		but only fields that the client has capabilities to delete will be removed.

		A request to delete data in unauthorised area will cause the function to leave

	@leave	If client not authorised to read or write the data, will leave with 
	        	KErrPermissionDenied 
            May also leave with other general error codes if there are unexpected 
            problems (e.g. KErrNoMemory when out of memory)

    @pre    Ensure the ElementId for this container fully identifies the target data in the database
                <Table><Column><Record>
            Ensure that the session object has appropriate access control attributes to manage the target data in the database
            Ensure the ElementId for this container correcty identifies the target data in the database

	*/
	IMPORT_C void DeleteL(CMDBSession& aSession);


	/**
	RequestNotification - Register for notification of events from all clients of the database

        aRequestStatus will be completed when the following events occur
            TransactionComplete
            TransactionRollback
            SessionClose
        
        this aRequestStatus can be cancelled by a call to 
            
            MMetaDatabase::CancelNotification

        all outstanding aRequestStatus can be cancelled by a call to
            
            CMDBSession::CancelAllNotifications

    @capability WriteDeviceData
    
    @return KErrNone on success.  A system wide error on failure

    @params aSession        a valid session object
    @params aRequestStatus  the requestStatus object that will be completed on notification
    
    @pre    None
    @post   A notification request will be outstanding
	*/
	IMPORT_C TInt RequestNotification(CMDBSession&          aSession,
									  TRequestStatus&        aRequestStatus);

	/**
	CancelNotification - cancel an outstanding notification request
        
        note that all outstanding notification requests can be cancelled by a call to
            
            CMDBSession::CancelAllNotifications

    @pres
         
        There is an outstanding notification request

	*/
	IMPORT_C TInt CancelNotification(CMDBSession&       aSession,
									 TRequestStatus&     aRequestStatus);


protected:


	IMPORT_C explicit MMetaDatabase();

private:

    /*
    @internalComponent
    */
    void DoLoadL(CMDBSessionImpl* aSession, CMDBElement* aElement, CMDBElement* aMapper, TInt& aErr, TMDBElementId aRecordId, TMDBElementId aAttributes, TBool isTheLoadForMapper = EFalse);

    /*
    @internalComponent
    */
    void DoLoadL(CMDBSessionImpl* aSession, CMDBElement* aElement, CMDBElement* aMapper, TInt& aErr, TMDBElementId aRecordId, TMDBElementId aAttributes, RArray<TUint32>& aMatches, TBool isTheLoadForMapper = EFalse);

    /*
    @internalComponent
    */
    void DoFindL(CMDBSessionImpl* aSession, CMDBElement* aElement, CMDBElement* aMapper, TInt& aErr, RArray<TUint32>& candidates, RArray<TUint32>& matches);

    /*
    @internalComponent
    */
    void DoRefreshL(CMDBSessionImpl* aSession, CMDBElement* aElement, CMDBElement* aMapper, TInt &aErr);

    /*
    @internalComponent
    */
    void DoStoreL(CMDBSessionImpl* aSession, CMDBElement* aElement, CMDBElement* aMapper, TInt &aErr);

    /*
    @internalComponent
    */
    void DoModifyL(CMDBSessionImpl* aSession, CMDBElement* aElement, CMDBElement* aMapper, TInt &aErr);

};





class CMDBElement : public CBase, public MMetaDatabase
/**
Every item stored in the database is represented as a CMDBElement
This class contains the id and attributes of every item in the database
and for individual fields it also contains the value.

@publishedAll
@released
*/
{
public:


	/**
	Default Constructor
	*/
	IMPORT_C CMDBElement();


	/**
	Constructor
	*/
	IMPORT_C CMDBElement(TMDBElementId aElementId);
	
	
	/**
	Destructor
	*/
	IMPORT_C ~CMDBElement();


	// Accessors

	/**
	Returns the ElementId of an object of this type.

	The TypeId is

	<TableId><ColumnId><RecordId>

	This identifies the meaning of the field in the database schema
	It also identifies the type of the data stored in this element
	and the instance of the type

	*/
	IMPORT_C TMDBElementId ElementId() const;


	/**
	Returns the TypeId of an object of this type.

	The TypeId is

	<TableId><ColumnId><x>

	This identifies the meaning of the data in this element in the database schema
	It also identifies the type of the data contained in this element

    For an element that is a Table or a Record, the ColumnId field will be set to 

    KCDMaxColumnId
	*/
	IMPORT_C TMDBElementId  TypeId() const;


	/**
	Returns the TableId of an object of this type.

	The TableId is part of the ElementId

	<TableId><x><x>

	It identifies the table in the database that holds the data contained in this element

	*/
	IMPORT_C TMDBElementId  TableId() const;

    
    /**
	Returns the RecordId of an object of this type.

	The RecordId is part of the ElementId <x><x><RecordId>

	It identifies a particular instance of this type of Element in the database

	*/
	IMPORT_C TInt RecordId() const;


	/**
	Returns the Attributes of an object of this type.

	Attributes express database access rights for the data of this element

	*/
	IMPORT_C TMDBAttributeFlags Attributes() const;

    // SETTER FUNCTIONS

	/**
	Sets the ElementId

	*/
	IMPORT_C void SetElementId(TMDBElementId aElementId);


	/**
	Sets the TypeId

	*/
	IMPORT_C void SetTypeId(TMDBElementId aElementId);


	/**
	Sets the RecordId

	*/
	IMPORT_C void SetRecordId(TInt aRecordId);


	/**
	Sets one or more attributes

	*/
	IMPORT_C void SetAttributes(TMDBAttributeFlags aAttributes);


	/**
	Clears one or more attributes

	*/
	IMPORT_C void ClearAttributes(TMDBAttributeFlags aAttributes);


	/**
	Queries the attributes

	Returns ETrue if all queried attributes are set
	Returns EFalse if any queried attribute is not set.
	*/
	IMPORT_C TBool IsSetAttribute(TMDBAttributeFlags aAttributes) const;


	/**
	Queries the state of the field value

	Returns ETrue if the value of the element is not set
	Returns EFalse if the element contains a value

	*/
	IMPORT_C TBool IsNull() const;	
	
	
	
    EXP_DATA_VTABLE
	
    /**
	Queries the type of the field value

	Returns the enumeration TCDFieldValueTypes that represents the value that can be stored in this element

	*/

	virtual TCDFieldValueTypes Type()=0;

	/**
	@internalComponent
	*/
    TMDBElementId* Data(){return &iElementId;}
	
protected:

	TMDBElementId     iElementId;
	
    TAny*             iValue;
    
    };




//
//  COMMSDAT CONTAINERS
//

/**
Commsdat Container classes qualifying numeric values
@publishedAll 
*/
class CMDBNumFieldBase : public CMDBElement
{
public:
	/**
	Default Constructor
	*/
    IMPORT_C CMDBNumFieldBase();


   	/**
	Constructor setting Field's ElementId
	*/
	IMPORT_C CMDBNumFieldBase(TMDBElementId aFieldId);


    /**
	Assignment operator for field value
	*/
	IMPORT_C CMDBNumFieldBase& operator=(const TInt aValue);
	
    IMPORT_C void SetMaxLengthL(TInt aLength);
    TMDBElementId GetL();
	     
    EXP_DATA_VTABLE
        

private:
	virtual TCDFieldValueTypes Type()
		{
		return EInt;
		}    
    
};

template<class TYPE>
class CMDBField : public CMDBNumFieldBase
/**
A thin template that guards typed data access in a single CMDBElement
This class describes a single numeric field in the database

@publishedAll
@released
*/
{
public:

	/**
	Default Constructor
	*/
	inline CMDBField()
		{
		}

	/**
	Constructor setting Field's ElementId
	*/
	inline CMDBField(TMDBElementId aFieldId)
		: CMDBNumFieldBase(aFieldId)
		{
		}


	/**
	Assignment operator for field value
	*/
	inline CMDBField<TYPE>& operator=(const TYPE& aValue)
	{
    	return (CMDBField<TYPE>&)CMDBNumFieldBase::operator=((TInt&)aValue);
	}


    inline CMDBField<TYPE>& operator=(const CMDBField<TYPE>& aValue)
	{
	    return (CMDBField<TYPE>&)CMDBNumFieldBase::operator=(aValue);
	}
	
	/**
	Conversion operator for field value
	*/
	inline operator TYPE&()
	{
		return (TYPE&)iValue;
	}
	
	/**
	Conversion operator for field value
	*/
	inline CMDBField<TYPE>& GetL()
	{
		if(IsNull() && !(ElementId() & KCDChangedFlag))
			{
			User::Leave(KErrNotFound); //todo commsdat specific error code
			}

		return (TYPE&)iValue;
	}
	
	/**
	Function for setting a value
	*/
	inline void SetL(const TYPE& aValue)
	{
	CMDBNumFieldBase::operator=(aValue);
	}
	
};



//
/**
Base Container class qualifying a text field
@publishedAll
*/
class CMDBTextFieldBase : public CMDBElement
{
public:
	/**
	Default Constructor
	*/
	IMPORT_C CMDBTextFieldBase();

	/**
	Constructor setting Field's ElementId
	*/
	IMPORT_C CMDBTextFieldBase(TMDBElementId aFieldId);    
	
	
	/**
	Destructor that deletes the field value
	*/
	IMPORT_C ~CMDBTextFieldBase();    
	
	
	/**
	Assignment Operator setting Field's value
	*/
	IMPORT_C CMDBTextFieldBase& operator=(const TDesC& aValue);
	
	    	
    IMPORT_C void SetMaxLengthL(TInt aLength);

    EXP_DATA_VTABLE

private:
	virtual TCDFieldValueTypes Type()
		{
		return EText;
		}    

};







template<>
class CMDBField<TDesC> : public CMDBTextFieldBase
/**
A thin template that guards typed data access in a single CMDBElement
This class describes a single field in the database

@publishedAll
@released
*/
{
public:

	/**
	Default Constructor
	*/
	inline CMDBField()
		{
		}

	/**
	Constructor setting Field's ElementId
	*/
	inline CMDBField(TMDBElementId aFieldId)
		: CMDBTextFieldBase(aFieldId)
		{
		}


    inline void SetMaxLengthL(TInt aLength)
	// create a new buffer if necessary.
	{
	    CMDBTextFieldBase::SetMaxLengthL(aLength);
	}
	
    inline CMDBField<TDesC>& operator=(const TDesC& aValue)
	{
        CMDBTextFieldBase::operator=(aValue);	   
        return *this; 
	}

    inline CMDBField<TDesC>& operator=(CMDBField<TDesC>& aValue)
	{
        CMDBTextFieldBase::operator=((const TDesC&)aValue);	
        return *this;   
	}

	/**
	Conversion operator for field value
	*/
	inline operator const TDesC&()
	{
        if (! iValue)
            return KNullDesC;

		return *(HBufC*)iValue;
	}
	
	/**
	Conversion operator for field value
	*/
	inline TDesC& GetL()
	{
		if(IsNull() && !(ElementId() & KCDChangedFlag))
			{
			User::Leave(KErrNotFound); //todo commsdat specific error code
			}

		return *(HBufC*)iValue;
	}
	
	/**
	Function for setting a value
	*/
	inline void SetL(const TDesC& aValue)
	{
	SetMaxLengthL(aValue.Length());
	CMDBTextFieldBase::operator=(aValue);
	}
	
	
};


//
/**
Container class for qualifying binary values
@publishedAll 
*/
class CMDBBinFieldBase : public CMDBElement
{
public:
	/**
	Default Constructor
	*/
	IMPORT_C CMDBBinFieldBase();

	
	/**
	Constructor setting Field's ElementId
	*/
	IMPORT_C CMDBBinFieldBase(TMDBElementId aFieldId);    
	
	
	/**
	Destructor that deletes the field value
	*/
	IMPORT_C ~CMDBBinFieldBase();    
	
	
	/**
	Assignment Operator setting Field's value
	*/
	IMPORT_C CMDBBinFieldBase& operator=(const TDesC8& aValue);
	    	
    /**
	To allocate space for field data.
	*/
	IMPORT_C void SetMaxLengthL(TInt aLength);

    EXP_DATA_VTABLE

private:
	virtual TCDFieldValueTypes Type()
		{
		return EDesC8;
		}    
};


template<>
class CMDBField<TDesC8> : public CMDBBinFieldBase
/**
A thin template that guards typed data access in a single CMDBElement
This class describes a single field in the database

@publishedAll
@released
*/
{
public:

	/**
	Default Constructor
	*/
	inline CMDBField()
		{
		}

	/**
	Constructor setting Field's ElementId
	*/
	inline CMDBField(TMDBElementId aFieldId)
		: CMDBBinFieldBase(aFieldId)
		{
		}

    inline void SetMaxLengthL(TInt aLength)
	// create a new buffer if necessary.
    	{
    	    CMDBBinFieldBase::SetMaxLengthL(aLength);
    	}
	
	
    inline CMDBField<TDesC8>& operator=(const TDesC8& aValue)
    	{
            return (CMDBField<TDesC8>&)CMDBBinFieldBase::operator=(aValue);	    
    	}
	
	
    inline CMDBField<TDesC8>& operator=(CMDBField<TDesC8>& aValue)
	// Copy assignment
        {
            return (CMDBField<TDesC8>&)CMDBBinFieldBase::operator=((const TDesC8&)aValue);	    
    	}


	/**
	Conversion operator for field value
	*/
	inline operator const TDesC8&()
    	{
		    if (! iValue)
                return KNullDesC8;

    		return *(HBufC8*)iValue;
    	}
    	
    /**
	Conversion operator for field value
	*/
	inline TDesC8& GetL()
	{
		if(IsNull() && !(ElementId() & KCDChangedFlag))
			{
			User::Leave(KErrNotFound); //todo commsdat specific error code
			}

    	return *(HBufC8*)iValue;
	}
	
	/**
	Function for setting a value
	*/
	inline void SetL(const TDesC8& aValue)
	{
	SetMaxLengthL(aValue.Length());
	CMDBBinFieldBase::operator=(aValue);
	}
	
};



//

/**
Base class for CMDBRecordLink
@publishedAll 
*/
class CMDBRecordLinkBase : public CMDBNumFieldBase
{
public :	

	IMPORT_C CMDBRecordLinkBase();

	IMPORT_C CMDBRecordLinkBase(TMDBElementId aLinkingFieldId);

	IMPORT_C CMDBRecordLinkBase(TMDBElementId aLinkingFieldId, CMDBElement* aLinkedRecord);


	IMPORT_C ~CMDBRecordLinkBase();

	/**
	Assignment operator for field value
	which sets the id of the element list
	*/
	IMPORT_C CMDBRecordLinkBase& operator=(const TMDBElementId aValue);


	/**
	Conversion operator for field value
	which gets the id of the element list
	*/
	IMPORT_C operator TMDBElementId();


protected:

	EXP_DATA_VTABLE

private:
    TCDFieldValueTypes Type()
		{
		return ELink;
		}    
public:
    
	// the record referenced by the link id stored in the value of this field.
	CMDBElement*     iLinkedRecord;

};


template<class RECORD_TYPE>
class CMDBRecordLink : public CMDBRecordLinkBase
/**
 This container expresses a 'soft-link' to a record in another table.

 The value of a CMDBRecordLink field as stored in the database is the 
 ElementId (<Table><Record>) of the linked record.

 The linked record itself can be viewed via the iLinkedRecord member and its accessors
 
 If a record is loaded then the iLinkedRecord must be created explicitly 
 by the client - either on construction, or by use of the CreateLinkL or 
 SetLinkL functions. Often the linked record is not required by a caller.  
 So creating it automatically would just waste memory
 
 However in the case of an explicit load of a LinkedRecord field the ILinkedRecord
 is created.
 
 When instantiating iLinkedRecord directly take care to match the object's type to the Type 
 ID that will be found at the linking element.  It is better to use the CreateLinkL or 
 SetLinkL functions to ensure type safety
 
 Calling MMetaDatabase functions on a CMDRecordLink field directly will perform the operation
 on the field and also the linked record if it is present 
@publishedAll
@released
*/
{
public :
	/**
	Constructor/Destructor
	*/

	inline CMDBRecordLink()
		: CMDBRecordLinkBase() {}

	/**
	Constructor
	*/
	inline CMDBRecordLink(TMDBElementId aLinkingFieldId)
		: CMDBRecordLinkBase(aLinkingFieldId) {}

	/**
	Constructor
	*/
	inline CMDBRecordLink(TMDBElementId aLinkingFieldId, RECORD_TYPE* aLinkedRecord)
		: CMDBRecordLinkBase(aLinkingFieldId, (CMDBElement*)aLinkedRecord) {}

	inline CMDBRecordLink& operator=(const TMDBElementId aValue)
	{
		return (CMDBRecordLink&)CMDBRecordLinkBase::operator=(aValue);
	}
	

    // SETTERS

	/**
	Set the field value (this will be validated on StoreL)
	*/
	inline void SetL(const TMDBElementId aValue)
		{
		CMDBRecordLinkBase::operator=(aValue);
		}
		

    // OPERATORS

	/**
	conversion operator for linked record of this type
	*/
	inline operator RECORD_TYPE*()
	{
		return static_cast<RECORD_TYPE*>(iLinkedRecord);
	}	

};

enum TFieldTypeAttributes
/**
To express type information for all MetaDatabase fields
*/
	{
	ENoAttrs,	//< No attributes associated with this field.
	ENotNull, 	//< Ensures the field contains a value and must not be null.	
	};

typedef struct
/**
To express type information for all MetaDatabase fields
*/
	{
		const TMDBElementId	        iTypeId;		//< Identifier for the field.	
		const TInt                  iValType;		//< The field type value e.g.EText, EMedText.
	    const TFieldTypeAttributes	iTypeAttr;		//< The field attribute, either ENoAttrs or ENotNull.
        const TText * const         iTypeName;		//< Name of the field type.
	    
	} SRecordTypeInfo;


#define MDB_GENERIC_TYPE_INFO(a, b, c, d) SGenericRecordTypeInfo(a, b, c, d)

typedef TBuf<64> TGenericTypeName;

typedef struct SGenericRecordTypeInfoTag
/**
To express type information for all MetaDatabase fields
*/
	{
	     SGenericRecordTypeInfoTag()
    	     : iTypeId(0), iValType(0), iTypeAttr(ENoAttrs) {}

	     SGenericRecordTypeInfoTag(TMDBElementId        aId,
	                               TInt                 aVal,
	                               TFieldTypeAttributes aAttrType)
   	     : iTypeId(aId), iValType(aVal), iTypeAttr(aAttrType)
    	        {
    	        }

	     SGenericRecordTypeInfoTag(TMDBElementId        aId,
	                               TInt                 aVal,
	                               TFieldTypeAttributes aAttrType,
	                               TText * const        aTypeName)
    	     : iTypeId(aId), iValType(aVal), iTypeAttr(aAttrType), iTypeName(aTypeName)
    	        {
    	        
    	        }


	     SGenericRecordTypeInfoTag(      TMDBElementId        aId,
	                                     TInt                 aVal,
	                                     TFieldTypeAttributes aAttrType,
	                               const TGenericTypeName&    aTypeName)
    	     : iTypeId(aId), iValType(aVal), iTypeAttr(aAttrType), iTypeName(aTypeName) {}


		 TMDBElementId	       iTypeId;			//< Identifier for the field type.	
		 TInt                  iValType;		//< The field type value e.g.EText, EMedText.
	     TFieldTypeAttributes  iTypeAttr;		//< The type's attribute, either ENoAttrs or ENotNull
         TGenericTypeName	   iTypeName;		//< Name of the field type.
	} SGenericRecordTypeInfo;
	
	

class CMDBRecordBase : public CMDBNumFieldBase
/**
Base class for all MetaDatabase records
@publishedAll
@released
*/
	{
	public:

		inline CMDBRecordBase(){}
		inline CMDBRecordBase(TMDBElementId aElementId)
			: CMDBNumFieldBase(aElementId | KCDMaskShowFieldType){}
			
		IMPORT_C virtual CMDBElement* GetFieldByNameL(const TPtrC& aFieldName, TInt& aValType );

		IMPORT_C virtual CMDBElement* GetFieldByIdL(TMDBElementId aId);
		
		IMPORT_C virtual const SRecordTypeInfo* GetRecordInfo();
		
	};



class CMDBGenericRecord : public CMDBRecordBase
/**
Container for data and type information in a MetaDatabase record
For use to contain user-defined records as an alternative to declaring a 
specific record class of type CCDRecordBase

@publishedAll
@released
*/
    {
friend class TMDBGenericRecordVisitor;  
friend class CMDBSessionImpl;  
	public:


		EXP_DATA_VTABLE
		    
		explicit inline CMDBGenericRecord(TMDBElementId aElementId)
			: CMDBRecordBase(aElementId) {}

        IMPORT_C ~CMDBGenericRecord();

		IMPORT_C virtual CMDBElement* GetFieldByNameL(const TPtrC& aFieldName, TInt& aValType );

		IMPORT_C CMDBElement* GetFieldByIdL(TMDBElementId aId);
		
		
		IMPORT_C void InitializeL(const TDesC& aTableName, const SGenericRecordTypeInfo*  aNewTableInfo);
		

	private:

        /*
        The name of this record type
        */
	    TBuf<KCDMaxFieldNameLength>   	iRecordType; 
        
        /*
        array of type information about fields in this record
        order of field info should match order of fields
        */
        RArray<SGenericRecordTypeInfo>  iRecordInfo;
        /*
        array of fields in this record
        order of fields should match order of field info
        */
		RPointerArray<CMDBElement>      iFields;
	};



class CMDBRecordSetBase : public CMDBTextFieldBase
/**
Use this class to express a list of records MetaDatabase Element type
Records can be ordered using the standard RPointerArray functions

@publishedAll
@released
*/
    {
    public:
    	IMPORT_C CMDBRecordSetBase();

    	IMPORT_C CMDBRecordSetBase(TMDBElementId aElementId);

    	IMPORT_C ~CMDBRecordSetBase();

        inline CMDBRecordSetBase& operator=(const TPtrC& aValue)
        {
        return (CMDBRecordSetBase&)CMDBTextFieldBase::operator=(aValue);
        }
	
	protected:

    	EXP_DATA_VTABLE

    public:
       
    	RPointerArray<CMDBRecordBase> iRecords;

    };



template <class TYPE>
class CMDBRecordSet : public CMDBRecordSetBase
/**
Use this class to express a list of records of a particular MetaDatabase Element type
Records can be ordered using the standard RPointerArray functions

@publishedAll
@released
*/
{
public:

	inline CMDBRecordSet(){}

	inline CMDBRecordSet(TMDBElementId aElementId)
		: CMDBRecordSetBase(aElementId) {}
		
	inline CMDBRecordSet<TYPE>& operator=(const TPtrC& aValue)
	{
		return (CMDBRecordSet<TYPE>&)CMDBRecordSetBase::operator=(aValue);
	}
    		
	inline TYPE* operator [](const TInt i)
	{
		return (TYPE*)iRecords[i];
	}

private:

};

} //end namespace CommsDat

#endif  // METADATABASE_H
