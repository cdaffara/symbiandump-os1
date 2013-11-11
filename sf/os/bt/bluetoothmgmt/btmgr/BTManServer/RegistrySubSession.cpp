// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Provides the implementation of the subsession that refers to remote devices
// 
//


#include "BTManServer.h"
#include "btmanserverutil.h"
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_MANAGER_SERVER);
#endif

//=====================================================================
//	CBTRegistrySubSession
//=====================================================================
//Description:	Provides an interface to the registry for device 
//				This is needed to provide device connection information for
//				subsequent connections.
//=====================================================================


CBTRegistrySubSession* CBTRegistrySubSession::NewL(CBTManSession& aSession, CBTRegistry& aRegistry)
	{
	LOG_STATIC_FUNC
	CBTRegistrySubSession* self = new(ELeave) CBTRegistrySubSession(aSession, aRegistry);
	//Since its a CObject derived class so we should use CleanupClosePushL
	CleanupClosePushL(*self);	
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CBTRegistrySubSession::ConstructL()
	{
	LOG_FUNC
	}
	
CBTRegistrySubSession::CBTRegistrySubSession(CBTManSession& aSession, CBTRegistry& aRegistry)
: CBTManSubSession(aSession,aRegistry)
	{
	LOG_FUNC
	}

CBTRegistrySubSession::~CBTRegistrySubSession() 
	{
	LOG_FUNC
	DoCloseView();
	delete iResultBuffer;
	}


void CBTRegistrySubSession::OpenViewL(const TBTRegistrySearch& aSearch, const RMessage2& aMessage)
/**
	@param aSearch	The structure containing the parameters to search for
	@param aMessage	The message to complete
**/
	{
	LOG_FUNC
	if (iDBView)
		{
		// previous view hasn't been retrieved
		User::Leave(KErrInUse);
		}

	// Form the SQL query based on the search criteria
	RBTDbQuery query;
	CleanupClosePushL(query);
	query.SearchL(aSearch);

	// execute the SQL
	iDBView = iRegistry.OpenViewL(query, iBookmark);

	iUnderlyingSearch = query.ConstraintBuf().AllocL();

	// see how many records are in the view
	iViewCount = iDBView->CountL();

	// send count back to client
	if (!iViewCount)
		{
		User::Leave(KErrNotFound);
		}
	else
		{
		// go to the first record in the view for later retrieval
		iDBView->FirstL();
		iSession.CompleteMessage(aMessage, iViewCount);
		}
	CleanupStack::Pop(1); // query
	// **leave view open for client's later requests upon it**
	}

void CBTRegistrySubSession::UnpairL(const TBTDevAddr& aAddress, const RMessage2& aMessage)
/**
	@param	aAddress	The address to unbond
	@param	aMessage	The message to complete
**/
	{
	LOG_FUNC
	// Form the SQL query based on the device address
	RBTDbQuery query;
	CleanupClosePushL(query);
	query.FindDeviceL(aAddress);

	// Execute the SQL
	RDbView* localView = iRegistry.OpenViewL(query, iBookmark);
	CleanupCloseDeletePushL(localView);

	HBufC* localViewUnderlyingSearch = query.ConstraintBuf().AllocLC();

	TInt c = localView->CountL();
	
	if (c)
		{
		// device is there - check to see if client is allowed to change
		__ASSERT_DEBUG(c==1, PanicServer(EBTManTooManyDevicesInView));

		// Try to notify interested parties before deleting the device.
		// If we notify afterwards, then the device will fall out of someone's
		// view and a change would then not be detected on that view.
		NotifyChange(*this, *localViewUnderlyingSearch);

		// the client is allowed to change the details
		iRegistry.UnpairL(*localView);
		iSession.CompleteMessage(aMessage, KErrNone);
		}
	else
		{
		// device wasn't even there
		User::Leave(KErrNotFound);
		}
		
	// Cleanup localViewUnderlyingSearch, localView and query
	CleanupStack::PopAndDestroy(3, &query);
	}

void CBTRegistrySubSession::GetDeviceL(const TBTNamelessDevice& aDevice, const RMessage2& aMessage)
/**
	Get a single nameless device from registry
	@param	aDevice		The device to retrieve (only the address is used as a key at present)
	@param	aMessage	The message to complete
**/
	{
	LOG_FUNC
	// open the device
	TDbBookmark bookmark;
	RDbView* localView = iRegistry.OpenDeviceL(aDevice.Address(), bookmark);
	CleanupCloseDeletePushL(localView);

	TInt localCount = localView->CountL();
	
	if (localCount==0)
		{
		// the device wasn't there!
		User::Leave(KErrNotFound);
		}
	else
		{
		__ASSERT_DEBUG(localCount==1, PanicServer(EBTManTooManyDevicesInView));

		// Get the resulting device (we take ownership), stays on cleanupstack


		const TBTNamelessDevice* device =
			iRegistry.GetNextNamelessDeviceLC(*localView,bookmark,aMessage.HasCapability(ECapabilityReadDeviceData));

		
		// Write back resulting device to client
		TBTNamelessDevicePckgBuf writePckg;
		writePckg = *device;

		aMessage.WriteL(0, writePckg);
		iSession.CompleteMessage(aMessage, KErrNone);
		
		CleanupStack::Pop(1);	// device
		delete const_cast<TBTNamelessDevice*>(device);	//MSDev workaround
		}
	CleanupStack::PopAndDestroy(localView);
	}


void CBTRegistrySubSession::AddDeviceL(const CBTDevice& aDetails, const RMessage2& aMessage)
/**
	@note This is an insertion, which can be slower via SQL; so we stick with C++ methods
	Further, this saves us the effort of converting various binary values into the ASCII
	SQL equivalent, only for DBMS to have to retokenise it ;-)
	Slight disadvantage is that the C++ interface to DBMS is synchronous...
	..but the addition of a new row unordered should not be hard in our small database
**/
	{
	LOG_FUNC

	iRegistry.CreateDeviceL(aDetails,
							aMessage.HasCapability(ECapabilityWriteDeviceData),
							aMessage.SecureId());
	// Form the SQL query based on the device address
	RBTDbQuery query;
	CleanupClosePushL(query);
	query.FindDeviceL(aDetails.BDAddr());
	
	HBufC* localViewUnderlyingSearch = query.ConstraintBuf().AllocLC();

	iSession.CompleteMessage(aMessage, KErrNone);

	// Try to notify interested parties after adding the device.
	// We need the device to be present in other views for change notification to work.
	NotifyChange(*this, *localViewUnderlyingSearch);
	
	// Cleanup localViewUnderlyingSearch and query
	CleanupStack::PopAndDestroy(2, &query);
	}

void CBTRegistrySubSession::ModifyL(const CBTDevice& aNewDetails, const RMessage2& aMessage)
/**
	@param	aNewDetails	The new device details; with the address to modify
	@param	aMessage	The message to complete
**/
	{
	LOG_FUNC
	// check that we can modify this device...
	if (!aNewDetails.IsValidBDAddr())
		{
		// the device address has not been set correctly
		User::Leave(KErrCorrupt);
		}

	if (!iRegistry.DevicePresentL(aNewDetails.BDAddr()))
		{
		// the device record that the client wants to modify is not present in the registry
		User::Leave(KErrNotFound);
		}
	else
		{
		// device is in Registry - open local device view
		TDbBookmark dummy; // ignore position for now

		// Form the SQL query based on the device address
		RBTDbQuery query;
		CleanupClosePushL(query);
		query.FindDeviceL(aNewDetails.BDAddr());

		// Execute the SQL
		RDbView* localView = iRegistry.OpenViewL(query, dummy);
		CleanupCloseDeletePushL(localView);

		HBufC* localViewUnderlyingSearch = query.ConstraintBuf().AllocLC();
	
		// Try to notify interested parties before making the change.
		// If we notify afterwards, then the device may fall out of someone's
		// view and a change would then not be detected on that view.		
		NotifyChange(*this, *localViewUnderlyingSearch);

		iRegistry.UpdateDeviceL(*localView, aNewDetails); 
		iSession.CompleteMessage(aMessage, KErrNone);
	
		// Cleanup localViewUnderlyingSearch, localView and query
		CleanupStack::PopAndDestroy(3, &query);
		}
	}

void CBTRegistrySubSession::ModifyL(const TBTNamelessDevice& aNewDetails, const RMessage2& aMessage)
/**
	@param	aNewDetails	The new device details; with the address to modify
	@param	aMessage	The message to complete
**/
	{
	LOG_FUNC
	// make a CBTDevice based on the T class
	CBTDevice* tempDevice = CBTDevice::NewLC(aNewDetails);
	ModifyL(*tempDevice, aMessage);
	CleanupStack::PopAndDestroy(tempDevice);
	// Notification already handled by overloaded ModifyL() implementation
	}

void CBTRegistrySubSession::ModifyNameL(const TBTDevAddr& aAddress, const RMessage2& aMessage)
/**
	Updates one of the names for a device in the Registry
	Here we decide if we can work with an address or not

	@param	aAddress	The address of the device to modify
	@param	aMessage	The message to complete, and from which we get the name
**/
	{
	LOG_FUNC

	// Form the SQL query based on the device address
	RBTDbQuery query;
	CleanupClosePushL(query);
	query.FindDeviceL(aAddress);

	// Execute the SQL
	RDbView* localView = iRegistry.OpenViewL(query, iBookmark);
	CleanupCloseDeletePushL(localView);

	HBufC* localViewUnderlyingSearch = query.ConstraintBuf().AllocLC();

	TInt c = localView->CountL();

	if (!c)
		{
		// can't modify name cos device isn't there!
		User::Leave(KErrNotFound);
		}
	
	

	// read the names
	TBuf8<KMaxBluetoothNameLen> buf;
	aMessage.ReadL(1, buf);
	
	// Try to notify interested parties before making the change.
	// If we notify afterwards, then the device may fall out of someone's
	// view and a change would then not be detected on that view.		
	NotifyChange(*this, *localViewUnderlyingSearch);

	// read out what's already in the database
	iRegistry.UpdateNameL(aAddress, buf, static_cast<TBTManServerRequest>(aMessage.Function()));
	// done, OK at this stage
	iSession.CompleteMessage(aMessage, KErrNone);

	// Cleanup localViewUnderlyingSearch, localView and query
	CleanupStack::PopAndDestroy(3, &query);
	}

void CBTRegistrySubSession::PreLoadL(const RMessage2& aMessage)
/**
	Load in the results from DBMS
	we'd have to load it all in at some time for giving to client, so we extend
	the lifetime a bit.  We kill the cache upon delivery to client.
	
	This approach allows us to compute the size of the result set, 
	so that we can tell client

	@param	aMessage	The message to complete
**/
	{
	LOG_FUNC
	if (!iViewCount)
		{
		// client has asked for something we dont have
		User::Leave(KErrNotFound);
		}
	
	// In case a previous registry response is cancelled, and then for the next request, 
	// a new registry response is created, the iResultBuffer could still be non-zero.
	// So, release memory now.
	delete iResultBuffer;
	iResultBuffer = NULL;

	// Create a buffer to hold the externalised entry
	iResultBuffer = CBufFlat::NewL(sizeof(CBTDevice)); // just a granularity for expansion

	RBufWriteStream stream;
	CleanupClosePushL(stream);
	stream.Open(*iResultBuffer);	// returns void

	TInt res = KErrNone;
	CBTDevice* device = NULL;

	iNumDevicesInBuffer=0;
	while (res == KErrNone)
		{
		// Get the entry and externalise to stream.
		TRAP(res, 
			 device= iRegistry.GetNextDeviceL(*iDBView, 
									iBookmark, 
									aMessage.HasCapability(ECapabilityReadDeviceData));
			CleanupStack::PushL(device);
			device->ExternalizeL(stream);
			CleanupStack::PopAndDestroy(device);
			device = NULL;
			iNumDevicesInBuffer++;

			);
		}

	// complete message with size of the blob or the error
	TInt code;
	if (res == KErrEof)
		{
		code = iResultBuffer->Size();
		}
	else
		{
		code = res;
		}
	iSession.CompleteMessage(aMessage, code);
	CleanupStack::PopAndDestroy(1); // stream
	
	// do not need to close view here
	}

	
void CBTRegistrySubSession::RetrieveL(const RMessage2& aMessage)
/**
	Returns the previously cached result set to the client

	@param	aMessage	The message to complete
**/
	{
	LOG_FUNC
	if (!iResultBuffer)
		{
		// they haven't preloaded
		User::Leave(KErrNotReady);
		}
	// write the buffer of devices back
	aMessage.WriteL(0, iResultBuffer->Ptr(0));
	iSession.CompleteMessage(aMessage, iNumDevicesInBuffer);

	// release memory now to reduce footprint
	delete iResultBuffer;
	iResultBuffer = NULL;
	}

void CBTRegistrySubSession::DeleteViewL(const RMessage2& aMessage)
/**
	Deletes all devices in the view - could be used to delete many devices in one call

	@param	aMessage The message to complete
**/
	{
	LOG_FUNC
	if (!iViewCount)
		{
		// no point calling delete on a view with no devices contained!
		User::Leave(KErrNotReady);
		}

	// Try to notify interested parties before deleting devices.
	// If we notify afterwards, then the devices will fall out of someone's
	// view and a change would then not be detected on that view.
	NotifyChange(*this, *iUnderlyingSearch);

	//	Delete all in view only if client has W.D.D capability. Otherwise delete the
	// devices whose SID value is same as the SecureId
	iRegistry.DeleteViewL(	*iDBView, 
							aMessage.HasCapability(ECapabilityWriteDeviceData),
							aMessage.SecureId());


	iSession.CompleteMessage(aMessage, KErrNone);
	
	// The view is not null, but probably not of much
	// use (ie. a view containing devices created by processes other than your own)
	// In addition, making the closure conditional would mean a potential SC
	// break for code moving from non-secure to secure world, so DoCloseView
	// remains non-optional at this point.
	DoCloseView();
	}

void CBTRegistrySubSession::UnpairViewL(const RMessage2& aMessage)
/**
	Unpairs all devices in the view

	@param	aMessage	The message to complete
**/
	{
	LOG_FUNC
	if (!iViewCount)
		{
		// no point calling delete on a view with no devices contained!
		User::Leave(KErrNotReady);
		}
	
	// Try to notify interested parties before making the change.
	// If we notify afterwards, then the device may fall out of someone's
	// view and a change would then not be detected on that view.		
	NotifyChange(*this, *iUnderlyingSearch);
		
	iRegistry.UnpairViewL(*iDBView);
	iSession.CompleteMessage(aMessage, KErrNone);
	}
	
void CBTRegistrySubSession::CloseView(const RMessage2& aMessage)
/**
	Client has finished with the view
	
	@param	aMessage	The message to complete
*/
	{
	LOG_FUNC
	DoCloseView();
	iSession.CompleteMessage(aMessage, KErrNone);
	
	// no underlying registry change, so don't notify
	}

void CBTRegistrySubSession::DoCloseView()
/**
	Close the internal view if it exists
**/
	{
	LOG_FUNC
	if (iDBView)
		{
		iDBView->Close();
		delete iDBView;
		iDBView=NULL;

		delete iUnderlyingSearch;
		iUnderlyingSearch=NULL;
		}
	// No point having notify on closed view; yet session may have already processed a cancel on it
	if (iSession.FindMessage(iViewChangeNotificationMessage))
		{
		iSession.CompleteMessage(iViewChangeNotificationMessage, KErrCompletion);
		}

	iViewCount=0;
	}


void CBTRegistrySubSession::Cleanup(TInt /*aError*/)
	{
	LOG_FUNC
	// this subsession can help by closing the view
	// it is unlikely to be in a good state anyway
	DoCloseView();
	}

void CBTRegistrySubSession::NotifyChange(CBTManSubSession& aSubSessionViewOwner, const TDesC& aViewDescriptor)
/**
	Called by methods that know they've changed settings on the remotedevice table
	This method will notify the client that was interested
**/
	{
	LOG_FUNC
	CBTManSubSession::NotifyChange(KRegistryChangeRemoteTable, aSubSessionViewOwner, aViewDescriptor);
	}

TBool CBTRegistrySubSession::IsOverlappingView(const TDesC& aViewDescriptor)
	{
	LOG_FUNC
	// Need to take the view descriptor and test with our view
	TBool match = EFalse;

	// We cannot rely on the handle of iViewChangeNotificationMessage to provide an indicator
	// of whether or not the message is completed, since we have a copy of the RMessage2 held by
	// the CBTManMessage instance.
	if (iDBView && iSession.FindMessage(iViewChangeNotificationMessage))
		{
		// Bookmark current place then use at end
		TDbBookmark bookmark = iDBView->Bookmark();

		// We never expect the following calls to iDBView methods to leave.
		TInt findResult=KErrNotFound;
#ifdef _DEBUG
		TRAPD(err, 
			iDBView->FirstL();
			findResult = iDBView->FindL(RDbRowSet::EForwards, TDbQuery(aViewDescriptor));
			);
		__ASSERT_DEBUG(err == KErrNone, PanicServer(EBTManUnexpectedDbError));
#else
		iDBView->FirstL();
		findResult = iDBView->FindL(RDbRowSet::EForwards, TDbQuery(aViewDescriptor));
#endif

		if (findResult != KErrNotFound)
			{
			match = ETrue;
			iSession.CompleteMessage(iViewChangeNotificationMessage, KErrNone);
			}
			
#ifdef _DEBUG
		TRAP(err, iDBView->GotoL(bookmark));
		__ASSERT_DEBUG(err == KErrNone, PanicServer(EBTManUnexpectedDbError));
#else
		iDBView->GotoL(bookmark);
#endif
		}

	return match;
	}

/*virtual*/ void CBTRegistrySubSession::SetViewChangeNotificationMessage(const RMessage2& aMessage)
	{
	LOG_FUNC
	if (iDBView)
		{
		iViewChangeNotificationMessage = aMessage;
		// Complete later
		}
	else
		{
		iSession.CompleteMessage(aMessage, KErrNotReady);
		}
	}
