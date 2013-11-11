// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the LBS Host Settings Interface
// 
//

/**
 @file
 @publishedPartner
 @deprecated 
*/


#include <e32base.h>

#include <lbs/lbshostsettings.h>
#include "lbshostsettingsimpl.h"

//=============================================================================
// MLbsHostSettingsStoreObserver
//=============================================================================

/**
Provides the version of the observer interface. This can be used
by the host settings storage library to determine the functionality
supported by the client application.

@return Version of the observer interface.
*/  
EXPORT_C TVersion MLbsHostSettingsStoreObserver::Version() const
	{
	return TVersion(0, 0, 0);
	}


/**
Should not be used. Provides for future expansion of the observer interface.
*/  
EXPORT_C TAny* MLbsHostSettingsStoreObserver::ExtendedInterface
	(
	TInt  /*aFunctionNumber*/,
	TAny* /*aPtr1*/,
	TAny* /*aPtr2*/
	)
	{
	return NULL;
	}


//=============================================================================
// CLbsHostSettingsBase
// CLbsHostSettingsStore::ConstructL() is defined in the impl.cpp file, in 
// order to transparently provide multiple implementations
//=============================================================================

/**
Default constructor for the settings store.
*/  
CLbsHostSettingsStore::CLbsHostSettingsStore()
	{	
	}


/**
Creates a new connection to the host settings stored for the specified settings store ID.
Each ID defines 

@param aStoreId Specifies the host settings store to open. The value specifies a settings store
for a specific protocol, for example SUPL.

@return An Instance of the storage class. The calling application becomes the
owner of the returned instance and is responsible its cleanup.
*/
EXPORT_C CLbsHostSettingsStore* CLbsHostSettingsStore::NewL(TLbsHostStoreId aStoreId)
	{	
	CLbsHostSettingsStore* newStore = new (ELeave) CLbsHostSettingsStore();
	CleanupStack::PushL(newStore);
	newStore->ConstructL(aStoreId, NULL);
	CleanupStack::Pop(newStore);
	return newStore;
	}

/**
Creates a new connection to the host settings stored for the specified protocol.
By specifying an observer, the calling application will receive a notification
whenever the specified store is updated by another client. 

Note: The client application will not receive an update notification for
changes made by itself.

@param aStoreId Specifies the host settings store to open.
@param aObserver The observer receives notifications of updates to
the host settings store specified by aStoreId.

@return A storage class instance. The calling application becomes the
owner of the returned instance and is responsible its cleanup.
*/  
EXPORT_C CLbsHostSettingsStore* CLbsHostSettingsStore::NewL(TLbsHostStoreId aStoreId, MLbsHostSettingsStoreObserver& aObserver)
	{
	CLbsHostSettingsStore* newStore = new (ELeave) CLbsHostSettingsStore();
	CleanupStack::PushL(newStore);
	newStore->ConstructL(aStoreId, &aObserver);
	CleanupStack::Pop(newStore);
	return newStore;
	}

/**
Retrieves the settings for a particular host settings ID.

The host settings ID is a persistent identifier that uniquely specifies
a host settings within one data store. This method locates and retrieves
the settings for the host indicated by the parameter aSettingsId.
If the method returns succcessfully, the paramter aSettings contains the
details of the specified host.

The ID for a particular host settings can be retrieved by calling
TLbsHostSettingsBase::HostSettingsId().

An application can monitor data store changes using the observer interface
MLbsHostSettingsStoreObserver (passing it as parameter in CLbsHostSettingsStore::NewL()).

The parameter aSettings must be derived from TLbsHostSettingsBase. For SUPL
related host information, the aSettings parameters must be of type
TLbsHostSettingsSupl.

@param aSettingsId Specifies the ID of the host setting that should be retrieved.
@param aSettings On a successful return, contains the settings for host specified by
aSettingsId.

@return KErrNone when the settings for the specified host are successfully returned.
A negative (error) value when it is not possible to return the stored settings
for the specified host. KErrNotFound when the specified host ID
is invalid or the associated host setting has been deleted. KErrArgument if the class type
of parameter aSettings is incompatible for the information contained in the data store.

@see TLbsHostSettingsBase::HostSettingsId()
@see TLbsHostSettingsSupl
@see MLbsHostSettingsStoreObserver
*/  
EXPORT_C TInt CLbsHostSettingsStore::GetHostSettings(TLbsHostSettingsId aSettingsId,TLbsHostSettingsBase& aSettings) const
	{
	return iSettingsStore->GetHostSettings(aSettingsId, aSettings);
	}


/**
Resets the client's view of the data store.

RewindHostSettings() is used in conjunction with GetNextHostSettings()
and GetNextHostSettingsByCreator().

Before calling GetNextHostSettings() or GetNextHostSettingsByCreator()
for the first time, the client application must call RewindHostSettings().
This resets the client's view of the data store. When a client subsequently calls
GetNextHostSettings() or GetNextHostSettingsByCreator() the details of
the first host settings will be returned.

@return KErrNone if successful. A negative (error) if it is not possible
reset the client view of the data store.

@see GetNextHostSettings()
@see GetNextHostSettingsByCreator()
*/  
EXPORT_C TInt CLbsHostSettingsStore::RewindHostSettings()
	{
	return iSettingsStore->RewindHostSettings();
	}


/**
Retrieves index of the next host settings following on from the specified host
settings ID.

Used in conjunction with RewindHostSettings() this method allows a client
application to iterate through the entire contents of the data store.

Before calling GetNextHostSettings() at the beginning of an iteration, the client application must call
RewindHostSettings(). This resets the client's view of the data store.
When the client subsequently calls GetNextHostSettings() the details of
the first host settings will be returned.

To iterate through all host settings contained in the data store, the client
application should continue to call GetNextHostSettings() until the method
returns KErrNotFound. The method returns KErrNotFound if there are no further
host settings contained in the data store.

@param aSettings On a successful return, contains the details of the
next host contained in the data store.

@param aSettingsId On a successful return, contains the ID of the
next host contained in the data store.

@return KErrNone if host settings have been found. A negative (error) value if it
is not possible to return the stored settings for the specified host.
KErrNotFound if there are no further host settings contained in the data store.
KErrArgument if the class type of parameter aSettings is incompatible for the
information contained in the data store.

@see NumHosts()
@see TLbsHostSettingsSupl
@see MLbsHostSettingsStoreObserver
@see NewL()
*/  
EXPORT_C TInt CLbsHostSettingsStore::GetNextHostSettings(TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId
	)
	{
	return iSettingsStore->GetNextHostSettings(aSettings, aSettingsId);
	}



/**
Retrieves the ID and details of the next host settings (provisioned by the specified creator)
in the data store

When a host setting is created, the client application specifies the ID of the
type of "creator". This indicates (for example) if the settings was created by a
control panel application or provisioned from the network.

Used in conjunction with RewindHostSettings(), a client application can use
GetNextHostSettingsByCreator() to retrieve one or more host settings
based on the ID of their creator.

To iterate through all host settings contained in the data store, the client
application should continue to call GetNextHostSettingsByCreator() until the method
returns KErrNotFound. The method returns KErrNotFound when there are no further host settings
contained in the data store.

@param aCreatorId Specifies the creator ID to be searched for.

@param aSettings On a successful return, contains the details of the
next host contained in the data store with the specified Creator ID.

@param aSettingsId On a successful return, contains the ID of the
next host contained in the data store with the specified Creator ID.

@return KErrNone if a host with the specified creator has been found and its details
successfully returned. A negative (error) value if it is not possible to return
the stored settings for the specified host. KErrNotFound when there are no further host
settings contained in the data store with the specified creator ID. KErrArgument if
the class type of parameter aSettings is incompatible for the information contained in the data store.

@see NumHosts()
@see TLbsHostSettingsSupl
@see MLbsHostSettingsStoreObserver
@see NewL()
*/  
EXPORT_C TInt CLbsHostSettingsStore::GetNextHostSettingsByCreator(TLbsHostCreatorId aCreatorId, TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId)
	{
	return iSettingsStore->GetNextHostSettingsByCreator(aCreatorId, aSettings, aSettingsId);
	}

/**

Replaces the settings for an existing host settings entry in the data store.

Note: The data in the parameter aSettings must have been initially obtained by
calling GetHostSettings() on the same data store. For SUPL related hosts, the
aSettings parameters must be of type TLbsHostSettingsSupl.

Updating an entry does not change the default host for the particular data store.
If the application wishes to make the updated entry the default host, it must call
SetDefaultHost() with the index of that entry.

@param aSettingsId Specifies the ID of the host settings to overwrite.
@param aSettings Supplied by the caller and contains the updated configuration
parameters for the specified host.

@return KErrNone when the entry has been successfully updated.
A negative (error) value is returned when it has not been possible to
update the host settings entry.
KErrNotFound if the host settings ID information specified aSettingsId
is invalid or has been deleted.
KErrArgument if the parameter aSettings does not contains valid settings
for host. In particular, it was not obtained using the GetHostSettings() on the same
data store or is of the wrong class type.

@capability WriteDeviceData
@capability NetworkServices
@capability Location
@see GetHostSettings()
@see TLbsHostSettingsSupl()
*/  
EXPORT_C TInt CLbsHostSettingsStore::UpdateHostSettings(TLbsHostSettingsId aSettingsId,	const TLbsHostSettingsBase& aSettings)
	{
	return iSettingsStore->UpdateHostSettings(aSettingsId, aSettings);
	}

/**

Removes the host information with the specified ID from the data store.

If the specified host is currently defined as the default host no other host
automatically becomes the new default. It is the responsibility of the calling
application to select a new default host. This is done using the
SetDefaultHostSettings() method.

@param aSettingsId Indicates which host setting to delete.

@return KErrNone when the host settings specified by aHostIndex has been deleted.
A negative (error) value when there has been a problem removing the specified settings.
KErrNotFound when the specified host ID is invalid or the associated host setting has already been deleted.

@capability WriteDeviceData
@see NumHosts()
@see GetHostSettings()
*/  
EXPORT_C TInt CLbsHostSettingsStore::DeleteHostSettings(TLbsHostSettingsId aSettingsId)
	{
	return iSettingsStore->DeleteHostSettings(aSettingsId);
	}

/**
Retrieves the settings for the host that is currently specified as the default host.

The default host is primarily used to decide which network server should be used to
service terminal initiated location requests.

The default host is only changed by calling SetDefaultHostSettings().Adding or
updating the settings for a host does not automatically make it the default. 

@param aSettings On a successful return, contains the details of the default host.

@return KErrNone when the default host settings ID has been retrieved.
@return A negative (error) value is returned when it is not possible to obtain the
active host.
@return KErrNotFound is returned if there is no currently defined active host.
@return KErrNotFound is also returned when there are no actual host settings
defined.
@return KErrArgument If the class type of parameter aSettings is incompatible
with data types used by the data store.

@see SetDefaultHostSettings
*/  
EXPORT_C TInt CLbsHostSettingsStore::GetDefaultHostSettings(TLbsHostSettingsBase& aSettings) const
	{
	TLbsHostSettingsId discardedId;
	return iSettingsStore->GetDefaultHostSettings(aSettings, discardedId);
	}

/**
Retrieves the settings for the host that is currently specified as the default host.

The default host is primarily used to decide which network server should be used to
service terminal initiated location requests.

The default host is only changed by calling SetDefaultHostSettings().Adding or
updating the settings for a host does not automatically make it the default. 

@param aSettings On a successful return, contains the details of the default host.
@param aSettingsId On a successful return, contains the ID of the default host.

@return KErrNone when the default host settings ID has been retrieved.
@return A negative (error) value is returned when it is not possible to obtain the
active host.
@return KErrNotFound is returned if there is no currently defined active host.
@return KErrNotFound is also returned when there are no actual host settings
defined.
@return KErrArgument If the class type of parameter aSettings is incompatible
with data types used by the data store.

@see SetDefaultHostSettings
*/  
EXPORT_C TInt CLbsHostSettingsStore::GetDefaultHostSettings(TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId) const
	{
	return iSettingsStore->GetDefaultHostSettings(aSettings, aSettingsId);
	}

/**

Changes the default host to that specified by the supplied ID.

The default host is primarily used to decide which network server should be
used to service terminal initiated location requests.

@param aSettingsId Specifies the ID of the host to use as the default. 

@return KErrNone if the default host has been successfully updated.
A negative (error) value if it is not possible to set the active
host to that specified by aHostIndex. KErrNotFound if the specified
host ID is invalid or the associated host setting has been deleted.

@capability WriteDeviceData
@capability NetworkServices
@capability Location
@see GetDefaultHostSettings()
*/  
EXPORT_C TInt CLbsHostSettingsStore::SetDefaultHostSettings(TLbsHostSettingsId aSettingsId)
	{
	return iSettingsStore->SetDefaultHostSettings(aSettingsId);
	}

/**
Creates a new host settings entry in the data store.

The index of the new entry is returned via the parameter aSettingsId.

The aSettings parameter contains the details of the new host entry. This
parameter must be derived from TLbsHostSettingsBase and be compatible
with entries in the data store. For SUPL related hosts, the aSettings
parameter must be of type TLbsHostSettingsSupl.

Creating a new entry in the data store does not change the "default" host
entry. If the application wishes to make the new entry the default host
it must call SetDefaultHost() with the index of that entry.

@param aSettings Supplied by the caller and contains the various configuration
parameters for the specified host.

@param aCreatorId Specifies which application type has created the entry.
This is used to distinguish between network and user provision entries.

@param aSettingsId Returned parameter that contains the ID of
the newly created host entry in the data store.

@return KErrNone when a new entry was successfully created in the data store.
A negative (error) value is returned when it has not been possible to
create a new host settings entry in the data store.
KErrArgument if the class type of parameter aSettings is incompatible
with data types used by the data store or some settings have undefined values.

@capability WriteDeviceData
@see GetHostSettings()
@see SetDefaultHost()
@see DefaultHostIndex()
*/  

EXPORT_C TInt CLbsHostSettingsStore::CreateHostSettings(const TLbsHostSettingsBase& aSettings,TLbsHostCreatorId  aCreatorId,TLbsHostSettingsId& aSettingsId)
	{		
	return iSettingsStore->CreateHostSettings(aSettings, aCreatorId, aSettingsId);
	}
	

