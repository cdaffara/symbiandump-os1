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

#include <dscstore.h>
#include <dscitem.h>

#ifdef DSCSTORE_SQL
	#include "dscdatabase_sql.h"	// SQL version
#else
	#include "dscdatabase.h"	// DBMS version
#endif

//Create a RDscStore object
EXPORT_C RDscStore::RDscStore() : iDscDatabase(NULL), iSpare(NULL)
	{
	}
		

/** Opens a connection with the DSC database. This function will create a secure DSC database 
including DSC and DscItem tables if any do not already exist.
If the connection is already open this method will return without error.
This should be the first function called on a RDscStore object after it is created.
The connection is terminated by calling the Close() member function
@leave With one of the system-wide error codes
@capability ReadDeviceData and WriteDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/
EXPORT_C   void RDscStore::OpenL()
	{
	if(!iDscDatabase)
		{
		iDscDatabase = CDscDatabase::NewL();
		}
	}


/** Close the connection with the DSC database
@publishedPartner
@released
@see RDbNamedDatabase
@see RDbView
@see RDbs
*/
EXPORT_C   void RDscStore::Close()
	{
	delete iDscDatabase;
	iDscDatabase = NULL; 
	}

	
/** Check if a database connection is opened
@return ETrue if this instance is connected to DSC database, else EFalse
@publishedPartner
@released
*/	
EXPORT_C	TBool RDscStore::IsOpened() const
	{
	return (iDscDatabase != NULL);
	}


/**
Create a new DSC in the DSC database
@param aDscId the Id of the DSC to be added to the database
@param aDescription the description of the DSC being added
@leave	KErrArgument - invalid parameter passed
@leave	KErrAlreadyExists - the DSC already exists
@leave	KErrNotReady - the RDscStore is not connected to the DSC database
@leave	KErrPermissionDenied - the caller does not have enough rights to do the operation
@leave	KErrLocked - the database is opened for enumeration or is locked by other client
@capability WriteDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/
EXPORT_C  void RDscStore::CreateDscL(const TUid& aDscId, const TDesC& aDescription)
	{
	CheckDbIsOpenL();
	iDscDatabase->CreateDscL(aDscId, aDescription); //lint !e613 Suppress warning: possible use of null pointer
	}

	
/**
Create a new DSC in the DSC database using the default DSC Id KDefaultSymbianDsc
@see RDscStore::CreateDscL(const TUid& aDscId, const TDesC& aDescription)
@publishedPartner
@released
*/
EXPORT_C  void RDscStore::CreateDscL()
	{
	CreateDscL(TUid::Uid(KDefaultSymbianDsc), KNullDesC);
	}

	
/**
Delete a DSC and all associated DSC items from the DSC database
@param aDscId the ID of the DSC to be deleted
@leave KErrNotReady - the RDscStore is not connected to the DSC database
@leave KErrPermissionDenied - the caller does not have enough rights to do the operation
@leave	KErrLocked - the database is opened for enumeration or is locked by other client
@leave KErrNotFound - the DSC does not exist in the database
@capability ReadDeviceData and WriteDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/
EXPORT_C  void RDscStore::DeleteDscL(const TUid& aDscId)
	{
	CheckDbIsOpenL();
	iDscDatabase->DeleteDscL(aDscId); //lint !e613 Suppress warning: possible use of null pointer
	}
		

/**
Check the existance of a DSC in the DSC database
@param aDscId the ID of the DSC being checked
@return ETrue if the aDscId exists, else EFalse
@leave	KErrNotReady - the RDscStore is not connected to the DSC database
@leave	KErrPermissionDenied - the caller does not have enough rights to do the operation
@leave	KErrLocked - the database is opened for enumeration or is locked by other client
@capability ReadDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/
EXPORT_C  TBool RDscStore::DscExistsL(const TUid& aDscId) const
	{
	CheckDbIsOpenL();
	return iDscDatabase->DscExistsL(aDscId); //lint !e613 Suppress warning: possible use of null pointer
	}

	
/**
Check the existance of DSC in the DSC database using the default dscId KDefaultSymbianDsc
@see RDscStore::DscExistsL(const TUid& aDscId)
@publishedPartner
@released
*/
EXPORT_C  TBool RDscStore::DscExistsL() const
	{
	return DscExistsL(TUid::Uid(KDefaultSymbianDsc));
	}

	
/** Accessor function returning the description of aDscId
@param aDscId the ID of the DSC being checked
@param aDescription the description returned from the DSC database
@leave KErrNotReady - the RDscStore is not connected to the DSC database
@leave KErrPermissionDenied - the caller does not have enough rights to do the operation
@leave	KErrLocked - the database is opened for enumeration or is locked by other client
@leave KErrNotFound - DSC or item does not exist
@leave KErrOverflow - if the length of aDescription is too short. Call MaxStringLength( TInt& aStringLength ) 
to interrogate the server for the maximum permissible length.
@capability ReadDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/	
EXPORT_C void RDscStore::GetDscDescriptionL(const TUid &aDscId, TDes& aDescription) const
	{		
	CheckDbIsOpenL();
	iDscDatabase->GetDscDescriptionL(aDscId, aDescription); //lint !e613 Suppress warning: possible use of null pointer
	}

	
/**
Check the existance of a DSC item in a DSC list.
Uses DscId and ItemId within aItem to determine the item.
If ItemId is not set, will check existance using DscId and Filename+Arguments.
No wildcards can be used in filename or arguments, only exact matching.

@param aItem the item to be checked in the DSC database
@return ETrue if the item exists, else EFalse
@leave KErrNotReady - the RDscStore is not connected to the DSC database
@leave KErrPermissionDenied - the caller does not have enough rights to do the operation
@leave	KErrLocked - the database is opened for enumeration or is locked by other client
@capability ReadDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/
EXPORT_C  TBool RDscStore::ItemExistsL(const CDscItem& aItem) const
	{
	CheckDbIsOpenL();
	return iDscDatabase->ItemExistsL(aItem); //lint !e613 Suppress warning: possible use of null pointer
	}	
	
/**
Add an item to a DSC list. 
@param	aItem the item to be added to the database. At least DscId and Filename has to be set
		before calling this function. ItemId will be assigned by the database.
@param	aPos the position in the DSC to add the item - either EFirst or ELast
@leave	KErrNotReady - the RDscStore is not connected to the DSC database
@leave	KErrPermissionDenied - the caller does not have enough rights to do the operation
@leave	KErrLocked - the database is opened for enumeration or is locked by other client
@leave	KErrNotFound - the DSC does not exist
@leave	KErrArgument - aItem is invalid
@leave	KErrAlreadyExists - an item with the same fileName and args already exists in the DSC
@capability ReadDeviceData and WriteDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/
EXPORT_C void RDscStore::AddItemL(CDscItem& aItem, TDscPosition aPos)
	{
	CheckDbIsOpenL();
	iDscDatabase->AddItemL(aItem, aPos); //lint !e613 Suppress warning: possible use of null pointer
	}

	
/**
Append an item to the end of a DSC
@see RDscStore::AddItemL(CDscItem& aItem,TDscPosition aPos)
@publishedPartner
@released
*/
EXPORT_C void RDscStore::AddItemL(CDscItem& aItem )
	{
	AddItemL(aItem, ELast);
	}

	
/**
Open a DSC for enumeration. This function must be called before the items in the table
can be enumerated using EnumReadNextL(). Only one DSC can be opened for enumeration at a time.

This function will create a read-lock on the database and push it onto the CleanupStack. 
Call CleanupStack::PopAndDestroy() to close the enumaration and release the database lock.
No updates to the database is possible until the read-lock is released.

Usage example:
@code
	RDscStore client;
	client.OpenL();
	CleanupClosePushL(client);
	client.EnumOpenLC();
	while(CDscItem* item = client.EnumReadNextL())
		{
		//Do something...
	 	delete item;
	 	item = NULL;
	 	}
	CleanupStack::PopAndDestroy(); //client.EnumClose()
	CleanupStack::PopAndDestroy(); //client.Close()
@endcode

@param aDscId the ID of the DSC to be opened for enumeration
@leave	KErrNotReady - the RDscStore is not connected to the DSC database
@leave	KErrPermissionDenied - the caller does not have enough rights to do the operation
@leave	KErrLocked - the database is already opened for enumeration or is locked by other client
@leave	KErrNotFound - the DSC does not exist in the DSC database
@capability ReadDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/
EXPORT_C  void RDscStore::EnumOpenLC(const TUid& aDscId)
	{
	CheckDbIsOpenL();
	iDscDatabase->EnumOpenLC(aDscId); //lint !e613 Suppress warning: possible use of null pointer
	}
	

/**
Open the default DSC for enumeration using default dscId KDefaultSymbianDsc
@see RDscStore::EnumOpenLC(const TUid& aDscId)
@publishedPartner
@released
*/
EXPORT_C  void RDscStore::EnumOpenLC()
	{
	EnumOpenLC(TUid::Uid(KDefaultSymbianDsc));
	}

		
/**
Read the next item in the DSC opened for enumeration. An enumeration must have been
opened by a call to EnumOpenLC(). The caller takes ownership of the returned object.

@return the next available item in the DSC or NULL if there are no more items.
@leave	one of the system-wide error codes, including:
		KErrNotReady - the RDscStore is not connected to the DSC database or EnumOpenL() has not been called
@capability ReadDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/
EXPORT_C CDscItem* RDscStore::EnumReadNextL()
	{
	CheckDbIsOpenL();
	return iDscDatabase->EnumReadNextL(); //lint !e613 Suppress warning: possible use of null pointer
	}


/**
Close an open enumeration.

@capability None
@see EnumOpenLC()
@publishedPartner
@released
*/	
EXPORT_C  void RDscStore::EnumClose()
	{
	if(iDscDatabase)
		iDscDatabase->EnumClose();
	}


/**
Read an item from the DSC. 
Uses DscId and ItemId within aItem to determine the item.
If ItemId is not set, will check existance using DscId and Filename+Arguments.
No wildcards can be used in filename or arguments, only exact matching.

@param aItem item to be retrieved
@leave	KErrNotReady - the RDscStore is not connected to the DSC database
@leave	KErrPermissionDenied - the caller does not have enough rights to do the operation
@leave	KErrLocked - the database is opened for enumeration or is locked by other client
@leave	KErrNotFound - DSC or item does not exist
@capability ReadDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/ 
EXPORT_C void RDscStore::ReadItemL(CDscItem& aItem)
	{
	CheckDbIsOpenL();
	iDscDatabase->ReadItemL(aItem); //lint !e613 Suppress warning: possible use of null pointer
	}

	
/**
Update an item in the DSC. 
Uses DscId and ItemId within aItem to determine the item.
If ItemId is not set, will check existance using DscId and Filename+Arguments.
No wildcards can be used in filename or arguments, only exact matching.

ItemId, Filename, ArgList and DscId cannot be updated.

@param aItem item to be updated
@leave	KErrNotReady - the RDscStore is not connected to the DSC database
@leave	KErrPermissionDenied - the caller does not have enough rights to do the operation
@leave	KErrLocked - the database is opened for enumeration or is locked by other client
@leave	KErrNotFound - DSC or item does not exist
@leave	KErrArgument - aItem contains invalid settings
@capability ReadDeviceData and WriteDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/ 			
EXPORT_C void RDscStore::UpdateItemL(const CDscItem& aItem)
	{
	CheckDbIsOpenL();
	iDscDatabase->UpdateItemL(aItem); //lint !e613 Suppress warning: possible use of null pointer
	}

	
/**
Delete a DSC item matching specified DscId and ItemId, or specified DscId and Filename and ArgList.
If ItemId is not set, will check existance using DscId and Filename+ArgList.
No wildcards can be used in Filename or ArgList, only exact matching.

@param aItem item to be deleted
@leave KErrNotReady - the RDscStore is not connected to the DSC database
@leave KErrPermissionDenied - the caller does not have enough rights to do the operation
@leave KErrLocked - the database is opened for enumeration or is locked by other client
@leave KErrNotFound - DSC or item does not exist
@capability ReadDeviceData and WriteDeviceData
@see RDbNamedDatabase
@publishedPartner
@released
*/ 		
EXPORT_C void RDscStore::DeleteItemL(const CDscItem& aItem)
	{
	CheckDbIsOpenL();
	iDscDatabase->DeleteItemL(aItem); //lint !e613 Suppress warning: possible use of null pointer
	}

/**
Interrogate the DscDatabase to discover the maximum supported length of a string.
@param aStringLength The result is supplied via this integer.
@capability None
@publishedPartner
@released
*/
EXPORT_C void RDscStore::GetMaxStringLength( TInt& aStringLength ) const
	{
	aStringLength = CDscDatabase::MaxStringLength();
	}


//
// PRIVATE METHODS
//

/** check the database is already open */
void RDscStore::CheckDbIsOpenL() const
	{
	if (!iDscDatabase)
		{
		User::Leave(KErrNotReady);
		}
	}
