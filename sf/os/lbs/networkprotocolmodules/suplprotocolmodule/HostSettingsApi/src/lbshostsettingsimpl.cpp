// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Internal implentation of the LBS Host Settings Interface
// 
//

/**
 @file
 @internalAll
 @deprecated 
*/

#include <e32base.h>
#include <centralrepository.h>
#include <e32debug.h>

#include <lbs/lbshostsettingsclasstypes.h>
#include <lbs/lbshostsettings.h>
#include "lbshostsettingsconsts.h"
#include "lbshostsettingsimpl.h"
#include "lbshostsettingsnotifier.h"

const TInt KFormattedDateSize = 22;
_LIT(KDateFormat,"%3%2%1%:1%H%T%S%.%C");

/**
Provides a seamless interface to the Central Repository.

The uniqueness of each key will be achieved by storing a key in the 
table, which in effect is a counter that represents the next key to
be used and that is incremented each time a group of settings is added
in the table. 

The group of settings associated with a SUPL account will be stored separately
in the Central Repository instead of storing them as one record. The last 24 bits 
of the 32-bit key will be used to represent a general key and we will vary the first 
8 bits to represent each particular setting. They could then be read back by masking 
those bits. The top 8 bits are defined in TLbsHostSettingRecordType.   

For example:

If the setting id is 4 then the associated record keys will be:
CreatorId(0x1):        0x10000004
ConnectionId(0x2):     0x20000004
ConnectionIdType(0x3): 0x30000004
HostAddress(0x4):      0x40000004
etc...

There is a reserved range of keys used for storing metadata values where the upper 4 bits 
are set to record type EMetaData and the bottom 28 bits are defined by 
TLbsHostSettingMetaRecordTypes. e.g:
NextKey(0x1):     0x00000001
DefaultHost(0x2): 0x00000002 

This class also offers the ability to update a group of settings associated
with an account, to set a particular account as default, to iterate over settings
and finally to delete them.

@see CRepository
@see CLbsHostSettingsStore 
*/

class CLbsHostSettingsStoreImpl : public CBase, public MLbsHostSettingsStoreImpl
	{
public:
	static CLbsHostSettingsStoreImpl* NewL(TLbsHostStoreId aStoreId, MLbsHostSettingsStoreObserver* aObserver);
	
	TInt CreateHostSettings(const TLbsHostSettingsBase& aSettings, TLbsHostCreatorId aCreatorId, TLbsHostSettingsId& aSettingsId);
	TInt UpdateHostSettings(TLbsHostSettingsId aSettingsId, const TLbsHostSettingsBase& aSettings);
	TInt GetHostSettings(TLbsHostSettingsId aSettingsId, TLbsHostSettingsBase& aSettings) const;
	TInt RewindHostSettings();
	TInt GetNextHostSettings(TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId);
	TInt GetNextHostSettingsByCreator(TLbsHostCreatorId aCreator, TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId);
	TInt SetDefaultHostSettings(TLbsHostSettingsId aSettingsId);
	TInt GetDefaultHostSettings(TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId) const;
	TInt DeleteHostSettings(TLbsHostSettingsId aSettingsId);
	~CLbsHostSettingsStoreImpl();
	
protected:
	CLbsHostSettingsStoreImpl();
	void ConstructL(TLbsHostStoreId aStoreId, MLbsHostSettingsStoreObserver* aObserver);

private:
	TInt GetSettingNumber(const TUint32 aKey) const;
	TUint32 CreateKey(const TLbsHostSettingRecordType aRecordType, const TUint32 aSettingNumber) const;
	TInt UpdateTimestamp(TLbsHostSettingsId aSettingsId) const;
	TInt ValidateHostSettings(const TLbsHostSettingsSupl& aSettings) const;
	
	CRepository* 					iRepository;
	CLbsHostSettingsNotifier*		iStoreNotifier;
	
	// structure to hold setting addresses and 
	// creator ids
	struct TIdCreatorPair
		{
	    TInt iSettingId;
	    TUid iCreator;	   	
		};
	RArray<TIdCreatorPair> iCurrentView;	
	TInt iIterator;
	};

//=============================================================================
// CLbsHostSettingsStore::ConstructL()
//=============================================================================

/**
Creates and assigns the internal resources required by the data store
library.
@param aStoreId Specifies which host settings store to utilise.
@param aObserver If non NULL, the observer receives notifications about
updates to store specified by aStoreId.
@internalComponent
*/  
void CLbsHostSettingsStore::ConstructL(TLbsHostStoreId aStoreId, MLbsHostSettingsStoreObserver* aObserver)
	{
	iImplementation = CLbsHostSettingsStoreImpl::NewL(aStoreId, aObserver);
	iSettingsStore = iImplementation;
	}

/**
Closes the connection to the host settings store and disposes of all open
or used resources.
@deprecated
*/  
EXPORT_C CLbsHostSettingsStore::~CLbsHostSettingsStore()
	{
	delete iImplementation;
	iImplementation = NULL;
	}

//=============================================================================
// CLbsHostSettingsStoreImpl
//=============================================================================

CLbsHostSettingsStoreImpl::~CLbsHostSettingsStoreImpl()
	{
	iCurrentView.Close();
	delete iStoreNotifier;
	delete iRepository;
	}


/**
Creates a new connection to the host settings stored for the specified protocol
*/  
CLbsHostSettingsStoreImpl* CLbsHostSettingsStoreImpl::NewL(TLbsHostStoreId aStoreId, MLbsHostSettingsStoreObserver* aObserver)
	{
	CLbsHostSettingsStoreImpl* self = new (ELeave) CLbsHostSettingsStoreImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aStoreId, aObserver);
	CleanupStack::Pop(self);
	return self;
	}

/**
Default constructor for the settings store.
*/  
CLbsHostSettingsStoreImpl::CLbsHostSettingsStoreImpl(): iIterator(KErrNotFound)
	{
	}

void CLbsHostSettingsStoreImpl::ConstructL(TLbsHostStoreId aStoreId, MLbsHostSettingsStoreObserver* aObserver)
	{
	iRepository = CRepository::NewL(aStoreId);

	// Create and start notifier if an observer exists
	if (aObserver)
		{
		iStoreNotifier = CLbsHostSettingsNotifier::NewL(aStoreId, *iRepository, aObserver);
		iStoreNotifier->RequestNotificationL();
		}
	}

/**
Resets the client's view of the data store.
@see CLbsHostSettingsStore::RewindHostSettings
*/  
TInt CLbsHostSettingsStoreImpl::RewindHostSettings()
	{
	// set iterator to -1
	// clear structure
	
	iIterator = KErrNotFound;
	iCurrentView.Reset();
	
	// rebuild our data structure
	RArray<TUint32> foundKeys; //method is non-leaving so no need to put this on the CUS
	
	const TUint32 KSettingNumberMask = 0;
	const TUint32 KPartialKey = CreateKey(ECreatorId, KSettingNumberMask); 
	
	// Find all the Creator id keys in the store so that we can generate a list of setting ids.
	TRAPD(err, err = iRepository->FindL(KPartialKey, KRecordTypeMask, foundKeys));
	if(err == KErrNone)
		{
		for(TInt i = 0, x = foundKeys.Count(); i < x; i++)
			{		
			// retrieve the CreatorId values foreach key	
			TInt creatorId;
			TIdCreatorPair keyPair;	
			err = iRepository->Get(foundKeys[i], creatorId);		
			if(err != KErrNone)
				{
				break;
				}
			
			// now add pair to structure
			// convert creatorValue to TUid for convenience
			keyPair.iSettingId = GetSettingNumber(foundKeys[i]);
			keyPair.iCreator = TUid::Uid(creatorId);
			err = iCurrentView.Append(keyPair);
			if(err != KErrNone)
				{
				break;
				}
			}
		}
	
	foundKeys.Close();
	return err;
	}


/**
Retrieves the settings for a particular host settings ID.
@see CLbsHostSettingsStore::GetHostSettings
*/  
TInt CLbsHostSettingsStoreImpl::GetHostSettings(TLbsHostSettingsId aSettingsId,	TLbsHostSettingsBase& aSettings) const
	{
	ASSERT(aSettingsId.iUid >= 0);
	if((aSettings.ClassType() & ELbsHostSettingsSuplClass))
		{
		TLbsHostSettingsSupl& tgt = (TLbsHostSettingsSupl&)aSettings;
		
		// Set the settings id
		tgt.SetHostSettingsId(aSettingsId);
		
		// Get the creator id
		TInt creatorId;
		TUint32 creatorIdKey = CreateKey(ECreatorId, aSettingsId.iUid);
		TInt err = iRepository->Get(creatorIdKey, creatorId);
		if(err != KErrNone)
			{
			return err;
			}
		tgt.SetCreatorId(TUid::Uid(creatorId));
		
		// Get the connection id
		TInt connectionId;
		TUint32 connectionIdKey = CreateKey(EConnectionId, aSettingsId.iUid);
		err = iRepository->Get(connectionIdKey, connectionId);
		if(err != KErrNone)
			{
			return err;
			}
		
		// Get the connection id type
		TInt connectionIdType;
		TUint32 connectionIdTypeKey = CreateKey(EConnectionIdType, aSettingsId.iUid);
		err = iRepository->Get(connectionIdTypeKey, connectionIdType);
		if(err != KErrNone)
			{
			return err;
			}
		tgt.SetConnectionPoint(connectionId, static_cast<TLbsHostSettingsSupl::TLbsConnectionType>(connectionIdType));
		
		// Get the host address
		TLbsHostNameAddress hostAddr;
		TUint32 hostAddrKey = CreateKey(EHostAddress, aSettingsId.iUid);
		err = iRepository->Get(hostAddrKey, hostAddr);
		if(err != KErrNone)
			{
			return err;
			}
		tgt.SetHostNameAddress(hostAddr);
		
		// Get the read only flag
		TBool readOnly;
		TUint32 readOnlyKey = CreateKey(EReadOnly, aSettingsId.iUid);
		err = iRepository->Get(readOnlyKey, readOnly);
		if(err != KErrNone)
			{
			return err;
			}
		tgt.SetReadOnly(readOnly);
		
		// Get the name
		TLbsHostName hostName;
		TUint32 hostNameKey = CreateKey(EHostName, aSettingsId.iUid);
		err = iRepository->Get(hostNameKey, hostName);
		if(err != KErrNone)
			{
			return err;
			}
		tgt.SetName(hostName);
		
		// Get the port id
		TInt portId;
		TUint32 portIdKey = CreateKey(EHostPort, aSettingsId.iUid);
		err = iRepository->Get(portIdKey, portId);
		if(err != KErrNone)
			{
			return err;
			}
		tgt.SetPortId(portId);
		
		// Get the providerId
		TLbsProviderId providerId;
		TUint32 providerIdKey = CreateKey(EProviderId, aSettingsId.iUid);
		err = iRepository->Get(providerIdKey, providerId);
		if(err != KErrNone)
			{
			return err;
			}
		tgt.SetProviderId(providerId);
		
		// Get the MOLR auth modes
		TInt authmolr;
		TUint32 authmolrKey = CreateKey(EAuthMoLr, aSettingsId.iUid);
		err = iRepository->Get(authmolrKey, authmolr);
		if(err != KErrNone)
			{
			return err;
			}
		tgt.SetAuthModesMOLR(authmolr);

		// Get the MTLR auth modes
		TInt authmtlr;
		TUint32 authmtlrKey = CreateKey(EAuthMtLr, aSettingsId.iUid);
		err = iRepository->Get(authmtlrKey, authmtlr);
		if(err != KErrNone)
			{
			return err;
			}
		tgt.SetAuthModesMTLR(authmtlr);
		
		//Get the last modified timestamp
		TBuf16<KFormattedDateSize> lastMod;
		TUint32 lastModKey = CreateKey(ETimestamp, aSettingsId.iUid);
		err = iRepository->Get(lastModKey, lastMod);
		if(err != KErrNone)
			{
			return err;
			}
		TRAP(err, tgt.SetLastModifiedL(lastMod));
		if(err != KErrNone)
			{
			return err;
			}
		
		return KErrNone;
		}
	return KErrArgument;
	}


/**
Retrieves the next host settings following on from the specified host
setttings ID.
@see CLbsHostSettingsStore::GetNextHostSettings
*/  
TInt CLbsHostSettingsStoreImpl::GetNextHostSettings(TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId)
	{
	if((aSettings.ClassType() & ELbsHostSettingsSuplClass))
		{
		++iIterator;
		
		// check if we have reached end of iteration
		// return KErrNotFound
		// this also implies that we need to have called RewindHostSettings
		// in order to use this method
		if(iIterator >= iCurrentView.Count())
			{
			return KErrNotFound;
			}
				
		TIdCreatorPair currentPair = iCurrentView[iIterator];
		aSettingsId.iUid = currentPair.iSettingId;
		return GetHostSettings(aSettingsId, aSettings);
		}
	return KErrArgument;
	}

/**
Retrieves the next host settings that was provisioned by the specified
creator.
@see CLbsHostSettingsStore::GetNextHostSettingsByCreator
*/  
TInt CLbsHostSettingsStoreImpl::GetNextHostSettingsByCreator(TLbsHostCreatorId aCreator, TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId)
	{
	ASSERT(aCreator.iUid >= 0);
	if((aSettings.ClassType() & ELbsHostSettingsSuplClass))
		{
		TIdCreatorPair currentPair;	
			
		for(;;) 
			{							
			++iIterator;
			
			// check if we have reached end of iteration
			// return KErrNotFound		
			if(iIterator >= iCurrentView.Count())
				{
				return KErrNotFound;
				}
			
			currentPair = iCurrentView[iIterator];
			
			// do we have the right creator id?
			if(aCreator == currentPair.iCreator)
				{
				break;
				}			
			}
									
		// at this stage we have the right creator
		// just return setting
		aSettingsId.iUid = currentPair.iSettingId;
		return GetHostSettings(aSettingsId, aSettings);
		}
	return KErrArgument;
	}

/**
Replaces the settings for an existing host settings entry in the data store.
@see CLbsHostSettingsStore::UpdateHostSettings
*/  
TInt CLbsHostSettingsStoreImpl::UpdateHostSettings(TLbsHostSettingsId aSettingsId, const TLbsHostSettingsBase& aSettings)
	{
	ASSERT(aSettingsId.iUid >= 0);
	if((aSettings.ClassType() & ELbsHostSettingsSuplClass))
		{
		//cast aSettings to a type we can use
		const TLbsHostSettingsSupl& src = reinterpret_cast<const TLbsHostSettingsSupl&>(aSettings);
		
		TInt err = ValidateHostSettings(src);
		if(err != KErrNone)
			{
			return err;
			}
		
		// Check the setting exists by getting the creator Id
		TInt creatorId;
		TUint32 creatorIdKey = CreateKey(ECreatorId, aSettingsId.iUid);
		err = iRepository->Get(creatorIdKey, creatorId);
		if(err != KErrNone)
			{
			return err;
			}
		
		//set connectionId & type
		TInt connectionId;
		TLbsHostSettingsSupl::TLbsConnectionType connectionIdType;
		src.GetConnectionPoint(connectionId, connectionIdType);
		TUint32 connectionIdKey = CreateKey(EConnectionId, aSettingsId.iUid);
		err = iRepository->Set(connectionIdKey, connectionId);
		if(err != KErrNone)
			{
			return err;
			}
		TUint32 connectionIdTypeKey = CreateKey(EConnectionIdType, aSettingsId.iUid);
		err = iRepository->Set(connectionIdTypeKey, connectionIdType);
		if(err != KErrNone)
			{
			return err;
			}
		
		//set hostAddr
		TLbsHostNameAddress hostAddr;
		TInt hostAddrKey = CreateKey(EHostAddress, aSettingsId.iUid);
		src.GetHostNameAddress(hostAddr);
		err = iRepository->Set(hostAddrKey, hostAddr);
		if(err != KErrNone)
			{
			return err;
			}
		
		// set read only
		TBool readOnly;
		TUint32 readOnlyKey = CreateKey(EReadOnly, aSettingsId.iUid);
		src.GetReadOnly(readOnly);
		err = iRepository->Set(readOnlyKey, readOnly);
		if(err != KErrNone)
			{
			return err;
			}
		
		//set hostName
		TLbsHostName hostName;
		TInt hostNameKey = CreateKey(EHostName, aSettingsId.iUid);
		src.GetName(hostName);
		err = iRepository->Set(hostNameKey, hostName);
		if(err != KErrNone)
			{
			return err;
			}
		
		//set port id
		TInt portId;
		TUint32 portIdKey = CreateKey(EHostPort, aSettingsId.iUid);
		src.GetPortId(portId);
		err = iRepository->Set(portIdKey, portId);
		if(err != KErrNone)
			{
			return err;
			}
		
		//set providerId setting
		TLbsProviderId providerId;
		TInt providerIdKey = CreateKey(EProviderId, aSettingsId.iUid);
		src.GetProviderId(providerId);
		err = iRepository->Set(providerIdKey, providerId);
		if(err != KErrNone)
			{
			return err;
			}
		
		// Get the MOLR auth modes
		TLbsHostSettingsSupl::TAuthModes authmolr;
		TUint32 authmolrKey = CreateKey(EAuthMoLr, aSettingsId.iUid);
		src.GetAuthModesMOLR(authmolr);
		err = iRepository->Set(authmolrKey, (TInt)authmolr);
		if(err != KErrNone)
			{
			return err;
			}

		// Get the MTLR auth modes
		TLbsHostSettingsSupl::TAuthModes authmtlr;
		TUint32 authmtlrKey = CreateKey(EAuthMtLr, aSettingsId.iUid);
		src.GetAuthModesMTLR(authmtlr);
		err = iRepository->Set(authmtlrKey, (TInt)authmtlr);
		if(err != KErrNone)
			{
			return err;
			}

		// Set the last modified timestamp.
		// Note that this should be the last setting updated since the
		// notifier relies on listening for changes to ETimestamp.
		return UpdateTimestamp(aSettingsId);
		}
	return KErrArgument;
	}

/**
Removes the host information with the specified ID from the data store.
@see CLbsHostSettingsStore::DeleteHostSettings
*/  
TInt CLbsHostSettingsStoreImpl::DeleteHostSettings(TLbsHostSettingsId aSettingsId)
	{
	ASSERT(aSettingsId.iUid >= 0);
	// We want to carry on with all of the deletes even if individual deletes fail in
	// order to clean up after ourselves as best we can. We return the first error that occurs.
	TInt returnErr = KErrNone;
	
	//delete creatorId
	TUint32 creatorIdKey = CreateKey(ECreatorId, aSettingsId.iUid);
	TInt err = iRepository->Delete(creatorIdKey);
	returnErr = err;
	
	//delete connection setting 
	//(no need to check it exists first as Delete will return KErrorNotFound)
	TUint32 connectionIdKey = CreateKey(EConnectionId, aSettingsId.iUid);
	err = iRepository->Delete(connectionIdKey);
	if(returnErr == KErrNone)
		{
		returnErr = err;
		}
	TUint32 connectionIdTypeKey = CreateKey(EConnectionIdType, aSettingsId.iUid);
	err = iRepository->Delete(connectionIdTypeKey);
	if(returnErr == KErrNone)
		{
		returnErr = err;
		}
	
	//delete hostAddr setting
	TUint32 hostAddrKey = CreateKey(EHostAddress, aSettingsId.iUid);
	err = iRepository->Delete(hostAddrKey);
	if(returnErr == KErrNone)
		{
		returnErr = err;
		}

	//delete read only setting
	TUint32 readOnlyKey = CreateKey(EReadOnly, aSettingsId.iUid);
	err = iRepository->Delete(readOnlyKey);
	if(returnErr == KErrNone)
		{
		returnErr = err;
		}

	//delete name setting
	TUint32 hostNameKey = CreateKey(EHostName, aSettingsId.iUid);
	err = iRepository->Delete(hostNameKey);
	if(returnErr == KErrNone)
		{
		returnErr = err;
		}
	
	// delete port id setting
	TUint32 portIdKey = CreateKey(EHostPort, aSettingsId.iUid);
	err = iRepository->Delete(portIdKey);
	if(returnErr == KErrNone)
		{
		returnErr = err;
		}
	
	//delete providerId setting
	TUint32 providerIdKey = CreateKey(EProviderId, aSettingsId.iUid);
	err = iRepository->Delete(providerIdKey);
	if(returnErr == KErrNone)
		{
		returnErr = err;
		}

	//delete authmolr setting
	TUint32 authMoLr = CreateKey(EAuthMoLr, aSettingsId.iUid);
	err = iRepository->Delete(authMoLr);
	if(returnErr == KErrNone)
		{
		returnErr = err;
		}

	//delete authmtlr setting
	TUint32 authMtLr = CreateKey(EAuthMtLr, aSettingsId.iUid);
	err = iRepository->Delete(authMtLr);
	if(returnErr == KErrNone)
		{
		returnErr = err;
		}
	
	
	// delete the last modified timestamp
	TUint32 lastModKey = CreateKey(ETimestamp, aSettingsId.iUid);
	err = iRepository->Delete(lastModKey);
	if (iStoreNotifier && err == KErrNone)
		{
		// Suppress notification to this client session
		iStoreNotifier->SuppressNotification(aSettingsId);
		}

	if(returnErr == KErrNone)
		{
		returnErr = err;
		}

	// reset the nextID to 0 when no records in cenrep
	RArray<TUint32> foundKeys;
	const TUint32 KSettingNumberMask = 0;
	const TUint32 KPartialKey = CreateKey(ECreatorId, KSettingNumberMask); 
	TRAP(err, err = iRepository->FindL(KPartialKey, KRecordTypeMask, foundKeys));
	if (err == KErrNotFound)
		{
		TInt nextIdKey = CreateKey(EMetaData, ENextKey);
		err = iRepository->Set(nextIdKey, 0);
		}
	foundKeys.Close();
	
	if(returnErr == KErrNone)
		{
		returnErr = err;
		}

	//reset the default host if the default host is being deleted.
	TInt defaultHost;
	TInt defaultHostKey = CreateKey(EMetaData, EDefaultHost);
	err = iRepository->Get(defaultHostKey, defaultHost);
	if (err == KErrNone)
		{
		if (defaultHost == aSettingsId.iUid) 
			{
			err = iRepository->Set(defaultHostKey, KMaxTInt);
			}
		}
	
	if(returnErr == KErrNone)
		{
		returnErr = err;
		}

	return returnErr;
	}

/**
Retrieves the settings for the host that is currently specified as the default host.
@see CLbsHostSettingsStore::GetDefaultHostSettings
*/  
TInt CLbsHostSettingsStoreImpl::GetDefaultHostSettings(TLbsHostSettingsBase& aSettings, TLbsHostSettingsId& aSettingsId) const
	{
	TInt defaultHost;
	TInt defaultHostKey = CreateKey(EMetaData, EDefaultHost);
	TInt err = iRepository->Get(defaultHostKey, defaultHost);
	if(err != KErrNone)
		{
		return err;
		}
	//now we have the key for the setting, we can read it if it exists
	aSettingsId = TUid::Uid(defaultHost);
	return GetHostSettings(TUid::Uid(defaultHost), aSettings);
	}

/**
Changes the default host to that specified by the supplied index.
@internalComponent
@see CLbsHostSettingsStore::SetDefaultHostSettings
*/  
TInt CLbsHostSettingsStoreImpl::SetDefaultHostSettings(TLbsHostSettingsId aSettingsId)
	{
	ASSERT(aSettingsId.iUid >= 0);
	TInt defaultHostKey = CreateKey(EMetaData, EDefaultHost);
	return iRepository->Set(defaultHostKey, static_cast<TInt>(aSettingsId.iUid));
	}


/**
Creates a new host settings entry in the data store.
@see CLbsHostSettingsStore::CreateHostSettings
*/  
TInt CLbsHostSettingsStoreImpl::CreateHostSettings(const TLbsHostSettingsBase& aSettings,	TLbsHostCreatorId  aCreator, TLbsHostSettingsId& aSettingsId)
	{
	ASSERT(aCreator.iUid >= 0);
	if((aSettings.ClassType() & ELbsHostSettingsSuplClass))
		{						
		const TLbsHostSettingsSupl& src = reinterpret_cast<const TLbsHostSettingsSupl&>(aSettings);										
		
		TInt err = ValidateHostSettings(src);
		if(err != KErrNone)
			{
			return err;
			}
		
		//get the next row id from the cenrep
		TInt nextId;
		TInt nextIdKey = CreateKey(EMetaData, ENextKey);
		err = iRepository->Get(nextIdKey, nextId);
		if(err != KErrNone)
			{
			return err;
			}
					
		//create keys for storing settings in the CenRep store
		TUint32 creatorIdKey = CreateKey(ECreatorId, nextId);				
		err = iRepository->Create(creatorIdKey, TInt(aCreator.iUid));
		if(err != KErrNone)
			{
			return err;
			}
		
		// Create the connection id & connection type settings
		TInt connectionId;
		TLbsHostSettingsSupl::TLbsConnectionType connectionIdType;
		src.GetConnectionPoint(connectionId, connectionIdType);
		
		TUint32 connectionIdKey = CreateKey(EConnectionId, nextId);
		err = iRepository->Create(connectionIdKey, connectionId);
		if(err != KErrNone)
			{
			// Cleanup
			DeleteHostSettings(TLbsHostSettingsId::Uid(nextId));
			return err;
			}
		TUint32 connectionIdTypeKey = CreateKey(EConnectionIdType, nextId);
		err = iRepository->Create(connectionIdTypeKey, connectionIdType);
		if(err != KErrNone)
			{
			// Cleanup
			DeleteHostSettings(TLbsHostSettingsId::Uid(nextId));
			return err;
			}
		
		// Create the host address setting
		TLbsHostNameAddress hostAddr;			
		src.GetHostNameAddress(hostAddr);
		
		TUint32 hostAddrKey = CreateKey(EHostAddress, nextId);
		err = iRepository->Create(hostAddrKey, hostAddr);
		if(err != KErrNone)
			{
			// Cleanup
			DeleteHostSettings(TLbsHostSettingsId::Uid(nextId));
			return err;
			}
		
		// Create the read only setting
		TBool readOnly;
		src.GetReadOnly(readOnly);
		TUint32 readOnlyKey = CreateKey(EReadOnly, nextId);
		err = iRepository->Create(readOnlyKey, readOnly);
		if(err != KErrNone)
			{
			// Cleanup
			DeleteHostSettings(TLbsHostSettingsId::Uid(nextId));
			return err;
			}
		
		// create the name setting
		TLbsHostName hostName;
		src.GetName(hostName);	
		TUint32 hostNameKey = CreateKey(EHostName, nextId);
		err = iRepository->Create(hostNameKey, hostName);
		if(err != KErrNone)
			{
			// Cleanup
			DeleteHostSettings(TLbsHostSettingsId::Uid(nextId));
			return err;
			}
		
		//set port id
		TInt portId;
		TUint32 portIdKey = CreateKey(EHostPort, nextId);
		src.GetPortId(portId);
		err = iRepository->Create(portIdKey, portId);
		if(err != KErrNone)
			{
			// Cleanup
			DeleteHostSettings(TLbsHostSettingsId::Uid(nextId));
			return err;
			}
		
		// create the providerId setting
		TLbsProviderId providerId;
		src.GetProviderId(providerId);	
		TUint32 providerIdKey = CreateKey(EProviderId, nextId);
		err = iRepository->Create(providerIdKey, providerId);
		if(err != KErrNone)
			{
			// Cleanup
			DeleteHostSettings(TLbsHostSettingsId::Uid(nextId));
			return err;
			}
		
		// Get the MOLR auth modes
		TLbsHostSettingsSupl::TAuthModes authmolr;
		TUint32 authmolrKey = CreateKey(EAuthMoLr, nextId);
		src.GetAuthModesMOLR(authmolr);
		err = iRepository->Create(authmolrKey, (TInt)authmolr);
		if(err != KErrNone)
			{
			return err;
			}

		// Get the MTLR auth modes
		TLbsHostSettingsSupl::TAuthModes authmtlr;
		TUint32 authmtlrKey = CreateKey(EAuthMtLr, nextId);
		src.GetAuthModesMTLR(authmtlr);
		err = iRepository->Create(authmtlrKey, (TInt)authmtlr);
		if(err != KErrNone)
			{
			return err;
			}		
				
		// Create the last modified timestamp.
		// Note that this should be the last setting created since the
		// notifier relies on listening for changes to ETimestamp.
		err = UpdateTimestamp(TLbsHostSettingsId::Uid(nextId));
		if(err != KErrNone)
			{
			// Cleanup
			DeleteHostSettings(TLbsHostSettingsId::Uid(nextId));
			return err;
			}
		
		aSettingsId.iUid = nextId;
	
		//increment row id & store it
		err = iRepository->Set(nextIdKey, ++nextId);
		return err;
		}
	return KErrArgument;
	}

TInt CLbsHostSettingsStoreImpl::GetSettingNumber(const TUint32 aKey) const
	{
	return (aKey & KSettingIdMask); 
	}

TUint32 CLbsHostSettingsStoreImpl::CreateKey(const TLbsHostSettingRecordType aRecordType, const TUint32 aSettingNumber) const
	{
	return ((aRecordType << KRecordTypeShift) | (aSettingNumber & KSettingIdMask));
	}

TInt CLbsHostSettingsStoreImpl::UpdateTimestamp(TLbsHostSettingsId aSettingsId) const
	{
	//set the timestamp
	TTime currentTime;
	currentTime.UniversalTime();
	TUint32 lastModKey = CreateKey(ETimestamp, aSettingsId.iUid);
	TBuf16<KFormattedDateSize> formattedDate;
  	TLocale locale;
  	// Make sure that the timestamp will always be recorded in YYYYMMDD format
  	locale.SetDateFormat(EDateEuropean);
  	TRAPD(err, currentTime.FormatL(formattedDate, KDateFormat, locale));

	if(err != KErrNone)
		{
		return err;
		}
	err = iRepository->Set(lastModKey, formattedDate);
	if (iStoreNotifier && err == KErrNone)
		{
		// Suppress notification to this client session
		iStoreNotifier->SuppressNotification(aSettingsId);
		}

	return err;
	}

TInt CLbsHostSettingsStoreImpl::ValidateHostSettings(const TLbsHostSettingsSupl& aSettings) const
	{
	//Validate Connection Type
	TInt connectionId;
	TLbsHostSettingsSupl::TLbsConnectionType connectionIdType;
	aSettings.GetConnectionPoint(connectionId, connectionIdType);
	if(connectionIdType > TLbsHostSettingsSupl::ELbsConnectionTypeProxy || 
			connectionIdType < TLbsHostSettingsSupl::ELbsConnectionTypeInvalid)
		{
		return KErrArgument;
		}
	
	//Validate port, cannot be negative
	TInt portId;
	aSettings.GetPortId(portId);
	if((portId < 0) || (portId > 0xFFFF))
		{
		return KErrArgument;
		}
	
	TUint32 maxAuth = TLbsHostSettingsSupl::EAuthNone + 
						TLbsHostSettingsSupl::EAuthTls + 
						TLbsHostSettingsSupl::EAuthAcaTls + 
						TLbsHostSettingsSupl::EAuthPskTls;
	// Validate molr host settings cannot be larger than:
	// EAuthNone(0x1) + EAuthTls(0x2) + EAuthAcaTls(0x4) + EAuthPskTls(0x8) = (0xF)
	TLbsHostSettingsSupl::TAuthModes authmolr;
	aSettings.GetAuthModesMOLR(authmolr);
	if(authmolr > maxAuth)
		{
		return KErrArgument;
		}
	
	// PSK/TLS is currently not supported by SUPL implementation.
	// Return KErrNotSupported if PskTls is the only specified authmode.
	if(authmolr == TLbsHostSettingsSupl::EAuthPskTls)
		{
		return KErrNotSupported;
		}

	// Validate mtlr host settings cannot be larger than:
	// EAuthNone(0x1) + EAuthTls(0x2) + EAuthAcaTls(0x4) + EAuthPskTls(0x8) = (0xF)
	TLbsHostSettingsSupl::TAuthModes authmtlr;
	aSettings.GetAuthModesMTLR(authmtlr);
	if(authmtlr > maxAuth)
		{
		return KErrArgument;
		}
	
	// PSK/TLS is currently not supported by SUPL implementation.
	// Return KErrNotSupported if PskTls is the only specified authmode.
	if(authmtlr == TLbsHostSettingsSupl::EAuthPskTls)
		{
		return KErrNotSupported;
		}
		
	// Passed validation
	return KErrNone;
	}

