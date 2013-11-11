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
// This file defines the interface to the list of remote hosts used
// by the LBS subsystem.
// 
//

/**
 @file
 @publishedPartner
 @deprecated 
*/

#ifndef LBSHOSTSETTINGS_H_
#define LBSHOSTSETTINGS_H_

#include <e32std.h>
#include <lbs/lbshostsettingsclasstypes.h>

/**
 * The ID of the Central Repository store used by the Host Settings API
 */
const TLbsHostStoreId KLbsHostSettingsSuplStoreId = {0x10285AA8};

/**
 * The host settings creator UID used by the device provisioning plug-ins.
 * If host settings have this creator ID they can be used for network initiated location requests
 */
const TLbsHostCreatorId KLbsHostSettingsDevProvCreatorId = {0x10285AA9};

/**
 * A test host settings creator UID
 */
const TLbsHostCreatorId KLbsHostSettingsTestCreatorId = {0x10285AAA};

class CLbsHostSettingsStoreImpl;
class MLbsHostSettingsStoreImpl;

/**
Interface that receives notification of changes to a host settings data store.
A reference to an object of this class can be passed to CLbsHostSettingsStore::NewL()
to register a client as a host settings data store observer.
@see CLbsHostSettingsStore
@publishedPartner
@deprecated
*/
class MLbsHostSettingsStoreObserver
	{
	
public:
	IMPORT_C virtual TVersion Version() const;

	/**
	Receive notification that the contents of the data store have changed.
	
	This method is called when a host settings entry in the data store been changed by
	another client session. It specifies the ID of the entry that has changed
	and also which store it belongs to.
	
	Note: An application will not receive notification updates to changes it
	itself has made to the same instance of CLbsHostSettingsStore.
	
	@param aError		KErrNone if an entry has been changed, otherwise an
						error code from CenRep indicating a notification
						request failure.
	@param aStoreId		Identifies which data store has been updated or
						incurred an error.
	@param aSettingsId	Contains the ID of the host settings that has changed.
						Valid only when aError == KErrNone.
	
	*/
	IMPORT_C virtual void LbsHostSettingsUpdated(TInt aError, TLbsHostStoreId aStoreId, TLbsHostSettingsId aSettingsId) const = 0;


protected:						
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	};

	
/**
A class that encapsulates a host settings data store. 

A host settings data store contains settings that describe the server hosts that the LBS subsystem
uses. For example, for the SUPL protocol, the data store contains settings for the SUPL servers that are used
for location requests (from local client applications and from the network), settings that define the authentication
type that should be used between client and server and the internet access point to use for network connections.
This class has methods to create a new host settings store, to create, modify and delete host settings and set the host
settings that should be used as defaults.
@see MLbsHostSettingsStoreObserver
@publishedPartner
@deprecated
*/
class CLbsHostSettingsStore : public CBase
	{
public:
	IMPORT_C static CLbsHostSettingsStore* NewL(TLbsHostStoreId aStoreId);
	
	IMPORT_C static CLbsHostSettingsStore* NewL(TLbsHostStoreId aStoreId, MLbsHostSettingsStoreObserver& aObserver);


	IMPORT_C TInt CreateHostSettings(const TLbsHostSettingsBase& aSettings, TLbsHostCreatorId aCreatorId, TLbsHostSettingsId& aSettingsId);

	IMPORT_C TInt UpdateHostSettings(TLbsHostSettingsId aSettingsId, const TLbsHostSettingsBase& aSettings);

	IMPORT_C TInt GetHostSettings(TLbsHostSettingsId aSettingsId, TLbsHostSettingsBase& aSettings) const;


	IMPORT_C TInt RewindHostSettings();

	IMPORT_C TInt GetNextHostSettings(TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId);

	IMPORT_C TInt GetNextHostSettingsByCreator(TLbsHostCreatorId aCreatorId, TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId);
	

	IMPORT_C TInt SetDefaultHostSettings(TLbsHostSettingsId aSettingsId);
	
	IMPORT_C TInt GetDefaultHostSettings(TLbsHostSettingsBase& aSettings) const;

	IMPORT_C TInt GetDefaultHostSettings(TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId) const;

	IMPORT_C TInt DeleteHostSettings(TLbsHostSettingsId aSettingsId);

	IMPORT_C ~CLbsHostSettingsStore();

protected:
	/**
	Default constructor for the settings store.
	*/  
	CLbsHostSettingsStore();
	void ConstructL(TLbsHostStoreId aStoreId, MLbsHostSettingsStoreObserver *aObserver);
	
private:
	/**
	 * In order to maintain two implementations behind one interface
	 * we are holding two different versions of the implementation.
	 * The pointer to the C class will only be used for instantiation
	 * and destruction, we shall use the pointer to the mixin for
	 * everything else.
	 */
	CLbsHostSettingsStoreImpl* iImplementation;
	MLbsHostSettingsStoreImpl* iSettingsStore;

private:
	};


#endif // LBSHOSTSETTINGS_H_


