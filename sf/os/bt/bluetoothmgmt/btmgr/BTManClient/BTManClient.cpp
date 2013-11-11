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
//

#include <btmanclient.h>
#include <e32math.h>
#include <utf.h>
#include "btmanclientserver.h"
#include "s32strm.h"
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_MANAGER_CLIENT);
#endif

const TInt KServerRetries = 2;
const TUint8 KDummyDevicePointer = 0xff;

/**
Start the btmanserver
@return Systemwide error code
@internalComponent
@released
*/
static TInt StartServer()
	{
	LOG_STATIC_FUNC

	const TUidType serverUid(KNullUid,KNullUid,KBTManServerUid3);
	
#ifdef __BTMANSERVER_NO_PROCESSES__
	//
	// In EKA1 WINS the server is a DLL, the exported entrypoint returns a TInt
	// which represents the real entry-point for the server thread
	//
	RLibrary lib;
	TInt r=lib.Load(KBTManServerImg,serverUid);
	if (r!=KErrNone)
		return r;
	TLibraryFunction ordinal1=lib.Lookup(1);
	TThreadFunction serverFunc=reinterpret_cast<TThreadFunction>(ordinal1());
	//
	// To deal with the unique thread (+semaphore!) naming in EPOC, and that we may
	// be trying to restart a server that has just exited we attempt to create a
	// unique thread name for the server.
	// This uses Math::Random() to generate a 32-bit random number for the name
	//
	TName name(KBTManServerName);
	_LIT(KHexString, "0x");
 	name.Append(KHexString());
	name.AppendNum(Math::Random(),EHex);
	RThread server;
	r=server.Create(name,serverFunc,
					KBTManServerStackSize,
					NULL,&lib,NULL,
					KBTManServerInitHeapSize,KBTManServerMaxHeapSize,EOwnerProcess);
	lib.Close();	// if successful, server thread has handle to library now
#else
	//
	// EPOC and EKA2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt r=server.Create(KBTManServerImg,KNullDesC,serverUid);
#endif

	LOG1(_L("BTMan Server created, code %d"), r);

	if (r!=KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat!=KRequestPending)
		{
		server.Kill(0);		// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		LOG(_L("BTMan Server Resumed"));
		}
		
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	LOG1(_L("BTMan Server started, code %d (0=>success)"), stat.Int());
	r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	
    server.Close(); 
 	LOG(_L("BTMan Server Closed"));
 	
	return r;
	}


/**
Default Constructor.
@publishedAll
@released
*/
EXPORT_C RBTMan::RBTMan()
	{
	LOG_FUNC
	}


/**
Connects to the btman server.
@return systemwide error code
@publishedAll
@released
*/
EXPORT_C TInt RBTMan::Connect()
	{
	LOG_FUNC
	TInt retry=KServerRetries;
	for (;;)
		{
		TInt r=CreateSession(KBTManServerName,Version());	//gives MessageSlots of -1
																//this uses global pool rather
																//than local pool
		if (r!=KErrNotFound && r!=KErrServerTerminated)
			return r;
		if (--retry==0)
			return r;
		r=StartServer();
		if (r!=KErrNone && r!=KErrAlreadyExists)
			return r;
		}
	}

/**
Returns the version of the server.
@return version of btmanserver
*/
EXPORT_C TVersion RBTMan::Version() const
/**
**/
	{
	LOG_FUNC
	return TVersion(KBTManServerMajorVersionNumber, 
					KBTManServerMinorVersionNumber, 
					KBTManServerBuildVersionNumber);
	}


/**
Default constructor of base class for subsessions
@publishedAll
@released
*/
EXPORT_C RBTManSubSession::RBTManSubSession()
: iClientServerMsg()
	{
	LOG_FUNC
	iClientServerMsg().iClientBusy = EFalse;
	iClientServerMsg().iClientStatusToCancel = NULL;
	}

/**
Cancel an outstanding request on btmanserver
@param aStatus the status of active object that made the original request
@publishedAll
@released
*/
EXPORT_C void RBTManSubSession::CancelRequest(TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		SendReceive(EBTManCancelRequest, TIpcArgs(&aStatus));
		}
	else
		{
		//Client has a bad handle therefore complete the call from here
		LocalComplete(aStatus, KErrBadHandle);
		}
	}

/**
Completes a request locally without involving the server.
Called when a client-side error occurs.
@param	aStatus:	The TRequestStatus of the active object that made the request that needs to be completed.
@param	aErr:		The reason for completion. aErr will be written into the TRequestStatus of the active object.
@internalComponent
@released
**/
void RBTManSubSession::LocalComplete(TRequestStatus& aStatus, TInt aErr)
	{
	LOG_FUNC
	aStatus = KRequestPending;
	TRequestStatus* pS = &aStatus;
	User::RequestComplete(pS, aErr);
	}


TBool RBTManSubSession::IsBusy() const
	{
	LOG_FUNC
	return iClientServerMsg().iClientBusy;
	}

/**
To prevent clients using the server more than once
Business is cleared once the server has completed the job
@param aStatus the status of the client AO making a request on the server. this will can be used for cancelling later
@internalComponent
@released
**/
void RBTManSubSession::SetBusy(TRequestStatus& aStatus)
	{
	LOG_FUNC
	iClientServerMsg().iClientBusy = ETrue;
	iClientServerMsg().iClientStatusToCancel = &aStatus;
	}

/**
Default constructor of registry session
@publishedAll
@released
*/
EXPORT_C RBTRegServ::RBTRegServ()
	{
	LOG_FUNC
	}


/**
Connect to Registry Server
@return System-wide error code
@publishedAll
@released
@capability LocalServices
*/
EXPORT_C TInt RBTRegServ::Connect()
	{
	LOG_FUNC
	// we provide the standard RBTMan session for now, but the client doesnt know this!
	TInt r=RBTMan::Connect();
	return r;
	}

/**
Close session on Registry Server
@publishedAll
@released
*/
EXPORT_C void RBTRegServ::Close()
	{
	LOG_FUNC
	RBTMan::Close();
	}

/**
Return reference to the server session
@return reference to btman session
@publishedAll
@released
*/
EXPORT_C RBTMan& RBTRegServ::Session()
	{
	LOG_STATIC_FUNC
	return static_cast<RBTMan&>(*this);
	}

#ifndef _DEBUG
EXPORT_C TInt RBTRegServ::SetHeapFailure(TInt /*aType*/,TInt /*aRate*/)
#else
EXPORT_C TInt RBTRegServ::SetHeapFailure(TInt aType,TInt aRate)
#endif
/**
Send a message to the server to fail memory allocations (in debug builds)
@internalAll
@released
*/
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(EBTManSetHeapFailure,TIpcArgs(static_cast<RHeap::TAllocFail>(aType), aRate));
#else
	return KErrNotSupported;
#endif
	}


/**
Returns the number of open subsessions.
This should be used by clients to do resource leakage debugging checks
@publishedAll
@released
*/
EXPORT_C TInt RBTRegServ::ResourceCount()
	{
	LOG_FUNC
	TInt count = 0;
	TPckg<TInt> pckg(count);
	SendReceive(EBTManSubSessionCount, TIpcArgs(&pckg));
	return count;
	}

/**
Default constructor for registry subsession
@publishedAll
@released
*/
EXPORT_C RBTRegistry::RBTRegistry()
: RBTManSubSession(), iDevicePckg(*reinterpret_cast<TBTNamelessDevice*>(KDummyDevicePointer))
	{
	LOG_FUNC
	} 

/**
Open a Bluetooth device subsession.
@param	aSession The BTManager session to which this subsession is to be attached.
@return An error code depicting the outcome of the "open".
@publishedAll
@released
@capability LocalServices
*/
EXPORT_C TInt RBTRegistry::Open(RBTRegServ& aSession)
	{
	LOG_FUNC
	iSendBuffer = NULL;
	return CreateSubSession(aSession.Session(), EBTManCreateRegistrySubSession, TIpcArgs(NULL));
	}

/** Create a constrained view of devices on the remote device table of the registry server.

This is in effect a registry search facility. 
It is needed if the user wishes find data in the registry of uncertain size.
For example:-
1) a list of remote devices. 
2) full details of a remote device including its name and/or friendly name.
Note 1: Some methods in the API (e.g. UnpairAllInView) require a view to have been created prior to their use.
Note 2: When a view has been generated, if details of the device(s) found are needed, these should be obtained using an instance of CBTRegistryResponse.
Note 3: A view must be closed before a new view can be created.

@see UnpairAllInView
@see DeleteAllInView
@see CloseView
@see NotifyViewChange
@see TBTRegistrySearch
@see CBTRegistryResponse
@param	aSearch A search struct with details of devices to be contained in the result set
@param	aStatus TRequestStatus supplied by caller
@return systemwide error
@publishedAll
@released
@capability LocalServices
@capability ReadDeviceData  (LocalServices only if link key is not needed)
*/
EXPORT_C void RBTRegistry::CreateView(const TBTRegistrySearch& aSearch, TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (IsBusy())
		{
		LocalComplete(aStatus, KErrInUse);
		}
	else
		{
		if (SubSessionHandle())
			{
			SetBusy(aStatus);
			iSearchPckg = aSearch;
			SendReceive(EBTManRegistrySearch, TIpcArgs(&iSearchPckg, NULL, &iClientServerMsg), aStatus);
			}
		else
			{
			//Client has a bad handle therefore complete the call from here
			LocalComplete(aStatus, KErrBadHandle);
			}
		}
	}

/**
Close the subsession.  Removes all server side resources pertaining to this subsession
@publishedAll
@released
*/
EXPORT_C void RBTRegistry::Close()
	{
	LOG_FUNC
	delete iSendBuffer;
	iSendBuffer = NULL;
	RSubSessionBase::CloseSubSession(EBTManCloseSubSession);
	}
	

/**
Close a previously created view on the registry.
Allows an app to use this subsession for other view or non-view operations
@return Systemwide error
@post Subsession is left open for further use - eg to create another view, or perform a viewless operation
@publishedAll
@released
*/
EXPORT_C TInt RBTRegistry::CloseView()
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		return SendReceive(EBTRegistryCloseView, TIpcArgs(NULL, NULL, NULL));	// synchronous
		}
	else
		{
		return KErrBadHandle;
		}
	}


/**
Unpair a device in the registry
@param aAddress The device to unpair
@param aStatus reference to client AO's TRequestStatus
@publishedPartner
@released
@capability LocalServices
@capability WriteDeviceData
*/
EXPORT_C void RBTRegistry::UnpairDevice(const TBTDevAddr& aAddress, TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (IsBusy())
		{
		LocalComplete(aStatus, KErrInUse);
		}
	else
		{
		if (SubSessionHandle())
			{
			SetBusy(aStatus);
			iAddrBuf = aAddress;
			SendReceive(EBTRegistryDeleteLinkKey, TIpcArgs(&iAddrBuf, NULL,  &iClientServerMsg), aStatus);
			}
		else
			{
			//Client has a bad handle therefore complete the call from here
			LocalComplete(aStatus, KErrBadHandle);
			}
		}
	}

/**
Unpair all the devices in the view - useful to unbond eg all devices (create a View with search All), or eg unbond all headsets
@param aStatus reference to client AO's TRequestStatus
@publishedPartner
@released
@capability LocalServices
@capability WriteDeviceData
*/
EXPORT_C void RBTRegistry::UnpairAllInView(TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		iClientServerMsg().iClientStatusToCancel = &aStatus;
		SendReceive(EBTRegistryUnpairView, TIpcArgs(NULL, NULL, &iClientServerMsg), aStatus);
		}
	else
		{
		//Client has a bad handle therefore complete the call from here
		LocalComplete(aStatus, KErrBadHandle);
		}
	}


/**
Add device to the registry
@leave OOM
@param aDevice reference to CBTDevice of details of device to add
@param aStatus reference to client AO's TRequestStatus
@publishedAll
@released
@capability LocalServices
*/
EXPORT_C void RBTRegistry::AddDeviceL(const CBTDevice& aDevice, TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (IsBusy())
		{
		LocalComplete(aStatus, KErrInUse);
		}
	else
		{
		if (SubSessionHandle())
			{
			if (iSendBuffer)
				{
				delete iSendBuffer;
				iSendBuffer = NULL;
				}
			// need to contiguate CBTDevice into a buffer
			SetBusy(aStatus);
			iSendBuffer = CBufFlat::NewL(sizeof(CBTDevice)); //granularity

			RBufWriteStream stream;
			stream.Open(*iSendBuffer);
			CleanupClosePushL(stream);

			aDevice.ExternalizeL(stream);
			const TPtr8 ptr=iSendBuffer->Ptr(0);
			iSendBufferPtr.Set(ptr);
			// now we can give the buffer to BTMan
			SendReceive(EBTRegistryAddDevice, TIpcArgs(&iSendBufferPtr, NULL,  &iClientServerMsg), aStatus);

			CleanupStack::PopAndDestroy(1);	// stream
			}
		else
			{
			//Client has a bad handle therefore complete the call from here
			LocalComplete(aStatus, KErrBadHandle);
			}
		}
	}

/**
Get a *nameless* device to the registry. To retrieve a full device with names a view should be created
@see CreateView
@pre Clients must ensure that they do not call this method while the same operation is already outstanding on the same RBTRegistry subsession
@param aDevice reference to TBTDevice (used as input and output). The input MUST contain the device address - this is used as the key. Once the method completes the reference will contain all other details found from the registry
@param aStatus reference to client AO's TRequestStatus
@publishedPartner
@released
@capability LocalServices
@capability ReadDeviceData  (LocalServices only if link key is not needed) 
*/
EXPORT_C void RBTRegistry::GetDevice(TBTNamelessDevice& aDevice, TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (IsBusy())
		{
		LocalComplete(aStatus, KErrInUse);
		}
	else
		{
		if (SubSessionHandle())
			{
			SetBusy(aStatus);
			TPckg<TBTNamelessDevice> temp(aDevice);
			iDevicePckg.Set(temp);
			SendReceive(EBTRegistryGetNamelessDevice, TIpcArgs(&iDevicePckg, NULL,  &iClientServerMsg), aStatus);
			}
		else
			{
			//Client has a bad handle therefore complete the call from here
			LocalComplete(aStatus, KErrBadHandle);
			}
		}
	}


/**
Modify the friendly name of a device
@leave OOM
@param aAddress	The address of the device of which to change the name
@param aName		The new name (note - not an 8bit descriptor)
@param aStatus reference to client AO's TRequestStatus
@publishedAll
@released
@capability LocalServices
*/
EXPORT_C void RBTRegistry::ModifyFriendlyDeviceNameL(const TBTDevAddr& aAddress,
											   		 const TDesC& aName,
											   		 TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (IsBusy())
		{
		LocalComplete(aStatus, KErrInUse);
		}
	else
		{
		if (SubSessionHandle())
			{
			SetBusy(aStatus);
			TBuf8<KMaxFriendlyNameLen> tempBuf;
			tempBuf.Zero();
			if (CnvUtfConverter::ConvertFromUnicodeToUtf8(tempBuf, aName) != KErrNone)
				{
				LocalComplete(aStatus, KErrBadName);
				return;
				}
			
			if (iSendBuffer)
				{
				delete iSendBuffer;
				iSendBuffer = NULL;
				}

			iSendBuffer = CBufFlat::NewL(sizeof(TBuf8<KMaxFriendlyNameLen>)); //granularity

			iSendBuffer->Reset();
			iSendBuffer->InsertL(0, tempBuf);

			const TPtr8 ptr=iSendBuffer->Ptr(0);
			iSendBufferPtr.Set(ptr);
			
			iAddrBuf = aAddress;

			SendReceive(EBTRegistryModifyFriendlyName,
						TIpcArgs(&iAddrBuf, &iSendBufferPtr,  &iClientServerMsg), aStatus);
			}
		else
			{
			//Client has a bad handle therefore complete the call from here
			LocalComplete(aStatus, KErrBadHandle);
			}
		}
	}



/**
Modify the Bluetooth name of a device.  This is not used beyond the stack
@param aAddress	The address of the device of which to change the name
@param aName		The new name (note - this is an 8bit descriptor)
@param aStatus reference to client AO's TRequestStatus
@internalAll
@released
@capability LocalServices
@capability WriteDeviceData (localServices only if friendly device name)
*/
EXPORT_C void RBTRegistry::ModifyBluetoothDeviceNameL(const TBTDevAddr& aAddress,
													  const TDesC8& aName,
													  TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (IsBusy())
		{
		LocalComplete(aStatus, KErrInUse);
		}
	else
		{
		if (SubSessionHandle())
			{
			SetBusy(aStatus);
			
			if (iSendBuffer)
				{
				delete iSendBuffer;
				iSendBuffer = NULL;
				}

			iSendBuffer = CBufFlat::NewL(sizeof(TBuf8<KMaxBluetoothNameLen>)); //granularity
			
			iSendBuffer->Reset();
			iSendBuffer->InsertL(0, aName);

			const TPtr8 ptr=iSendBuffer->Ptr(0);
			iSendBufferPtr.Set(ptr);
			
			iAddrBuf = aAddress;

			SendReceive(EBTRegistryModifyBluetoothName,
						TIpcArgs(&iAddrBuf, &iSendBufferPtr,  &iClientServerMsg), aStatus);
			}
		else
			{
			//Client has a bad handle therefore complete the call from here
			LocalComplete(aStatus, KErrBadHandle);
			}
		}
	}


/**
Update details of a device - other than its names: Not used beyond stack
@param aDeviceDetails The new details - the device address MUST be present in aDeviceDetails is used as the key
@param aStatus reference to client AO's TRequestStatus
@internalTechnology
@released
@capability LocalServices
@capability WriteDeviceData
*/
EXPORT_C void RBTRegistry::ModifyDevice(const TBTNamelessDevice& aDeviceDetails, TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (IsBusy())
		{
		LocalComplete(aStatus, KErrInUse);
		}
	else
		{
		if (SubSessionHandle())
			{
			SetBusy(aStatus);
			TPckg<TBTNamelessDevice> temp(aDeviceDetails);
			iDevicePckg.Set(temp);
			SendReceive(EBTRegistryModifyNamelessDevice, TIpcArgs(&iDevicePckg, NULL,  &iClientServerMsg), aStatus);
			}
		else
			{
			//Client has a bad handle therefore complete the call from here
			LocalComplete(aStatus, KErrBadHandle);
			}
		}
	}

/**
Notifies the client when a change has been made to the registry that affects the currently open view of devices.
@see CreateView
@see CloseView
@param aStatus reference to client AO's TRequestStatus
@publishedAll
@released
*/
EXPORT_C void RBTRegistry::NotifyViewChange(TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		// We don't set the session as busy because then no other commands can be sent
		iClientServerMsg().iClientStatusToCancel = &aStatus;
		SendReceive(EBTRegistryNotifyViewChange, TIpcArgs(NULL, NULL, &iClientServerMsg), aStatus);
		}
	else
		{
		//Client has a bad handle therefore complete the call from here
		LocalComplete(aStatus, KErrBadHandle);
		}
	}

/**
@internalComponent
*/
void RBTRegistry::PreLoad(TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		iClientServerMsg().iClientStatusToCancel = &aStatus;
		SendReceive(EBTManExtractRegistryDataIntoServer, TIpcArgs(NULL, NULL, &iClientServerMsg), aStatus);
		}
	else
		{
		//Client has a bad handle therefore complete the call from here
		LocalComplete(aStatus, KErrBadHandle);
		}
	}



/**
@internalComponent
**/
void RBTRegistry::GetResults(TPtr8& aResults, TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		iClientServerMsg().iClientStatusToCancel = &aStatus;
		SendReceive(EBTManRetrieveRegistryData, TIpcArgs(&aResults, NULL, &iClientServerMsg), aStatus);
		}
	else
		{
		//Client has a bad handle therefore complete the call from here
		LocalComplete(aStatus, KErrBadHandle);
		}
	}

/**
Remove all devices in the view from the Registry
@pre View must be created first by client
@see CreateView
@param aStatus a TRequestStatus passed in by the caller
@publishedPartner
@released
@capability LocalServices
@capability WriteDeviceData (Only if different process than the one that created the device)
*/
EXPORT_C void RBTRegistry::DeleteAllInView(TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		iClientServerMsg().iClientStatusToCancel = &aStatus;
		SendReceive(EBTRegistryDeleteDevices, TIpcArgs(NULL, NULL, &iClientServerMsg), aStatus);
		}
	else
		{
		//Client has a bad handle therefore complete the call from here
		LocalComplete(aStatus, KErrBadHandle);
		}
	}

/**
default c'tor
@publishedAll
@released
*/
EXPORT_C RBTLocalDevice::RBTLocalDevice()
:RBTManSubSession(), iLocalDevicePckg(*reinterpret_cast<TBTLocalDevice*>(KDummyDevicePointer))
	{
	LOG_FUNC
	}


/**
Open a local device subsession on the server, used for manipulating settings about the local device
@param aSession A server session
@return systemwide error
@publishedAll
@released
*/
EXPORT_C TInt RBTLocalDevice::Open(RBTRegServ& aSession)
	{
	LOG_FUNC
	return CreateSubSession(aSession.Session(), EBTManCreateLocalDeviceSubSession, TIpcArgs(NULL));
	}


/**
Close the subsession on the server
@publishedAll
@released
*/
EXPORT_C void RBTLocalDevice::Close()
	{
	LOG_FUNC
	RSubSessionBase::CloseSubSession(EBTManCloseSubSession);
	}


/**
Retrieve the local device details. Synchronous - intended mainly for Bluetooth stack use on startup

@param	aLocalDeviceResult	a reference to a TBTLocalDevice to store the results from the Registry
@return	SystemWide error
@publishedAll
@released
*/
EXPORT_C TInt RBTLocalDevice::Get(TBTLocalDevice& aLocalDeviceResult)
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		TPckg<TBTLocalDevice> pckg(aLocalDeviceResult);
		return SendReceive(EBTRegistryGetLocalDevice, TIpcArgs(&pckg, NULL, NULL));	// synchronous
		}
	else
		{
		//Client has a bad handle therefore complete the call from here
		return KErrBadHandle;
		}
	}

/**
Update the details about the local Bluetooth device
@param aLocalDevice New settings for local device
@param aStatus reference Client AO's TRequestStatus
@publishedPartner
@released
@capability LocalServices
@capability WriteDeviceData
*/
EXPORT_C void RBTLocalDevice::Modify(const TBTLocalDevice& aLocalDevice, TRequestStatus& aStatus)
	{
	LOG_FUNC
	if (IsBusy())
		{
		LocalComplete(aStatus, KErrInUse);
		}
	else
		{
		if (SubSessionHandle())
			{
			SetBusy(aStatus);
			TPckg<TBTLocalDevice> pckg(aLocalDevice);
			iLocalDevicePckg.Set(pckg);
			SendReceive(EBTRegistryUpdateLocalDevice, TIpcArgs(&iLocalDevicePckg, NULL, &iClientServerMsg), aStatus);
			}
		else
			{
			//Client has a bad handle therefore complete the call from here
			LocalComplete(aStatus, KErrBadHandle);
			}
		}
	}

/**
Synchronous overload of Modify (primarily for stack operations)
@see RBTLocalDevice::Modify(const TBTLocalDevice& aLocalDevice, TRequestStatus& aStatus)
@publishedPartner
@released
@capability LocalServices
@capability WriteDeviceData
*/
EXPORT_C TInt RBTLocalDevice::Modify(const TBTLocalDevice& aLocalDevice)
	{
	LOG_FUNC
	// synchronous version for stack on close-down
	TRequestStatus status;
	Modify(aLocalDevice, status);
	User::WaitForRequest(status);
	return status.Int();
	}


/**
Two-phase constructor to create a helper class to retrieve a set of results (a view) from the Registry
@leave OOM
@pre A non-empty view on the Registry task has been created
@param aView the Registry subsession that has had a View created on it
@return pointer to allocated object
@see CreateView()
@publishedAll
@released
*/
EXPORT_C CBTRegistryResponse* CBTRegistryResponse::NewL(RBTRegistry& aView)
	{
	LOG_STATIC_FUNC
	return new (ELeave) CBTRegistryResponse(aView);
	}


/**
Start fetching results from the Registry Server.  When the request is complete, the results can be obtained via the Results() method.
@param aClientStatus a TRequestStatus passed in by the caller
@publishedAll
@released
*/
EXPORT_C void CBTRegistryResponse::Start(TRequestStatus& aClientStatus)
	{
	LOG_FUNC
	// we do late construction on the heap buffer
	__ASSERT_DEBUG(!IsActive(), User::Panic(KBTManClientPanic, EBTManClientResultRetrieveAlreadyActive));

	iClientStatus = &aClientStatus;
	*iClientStatus = KRequestPending;
	CActiveScheduler::Add(this);
	DoGet();
	}

/** Get Results previously fetched from the Registry Server.
 
@pre The method Start needs to have been called, and notification of its completion received.
@return reference to the array of results
@publishedAll
@released
*/
EXPORT_C RBTDeviceArray& CBTRegistryResponse::Results()
	{
	LOG_FUNC
	return iArray;
	}

CBTRegistryResponse::CBTRegistryResponse(RBTRegistry& aView)
: CActive(EPriorityStandard), iView(aView), iResponsePtr(NULL,0)
	{
	LOG_FUNC
	}

/**
Destructor
*/
EXPORT_C CBTRegistryResponse::~CBTRegistryResponse()
	{
	LOG_FUNC
	Cancel();

	iArray.ResetAndDestroy();
	delete iResponseBuf;
	}

void CBTRegistryResponse::DoGet()
	{
	LOG_FUNC
// create RPointerArray to point to n CBTDevices
// ask server async size of result set - this causes BTMan to extract from DBMS and return size
// we then allocate and in 2nd phase get the stuff from the server (could be synchronous
// we could wrap all this up in a helper object for client to use: CBTRegistryResponder

	iArray.ResetAndDestroy();
	iState = EGettingSize;
	// Get the size of the entry
	iView.PreLoad(iStatus);
	SetActive();
	}

void CBTRegistryResponse::DoCancel()
	{
	LOG_FUNC
	//cancel the server
	iView.CancelRequest(iStatus);
	//notify our user
	User::RequestComplete(iClientStatus, KErrCancel);
	}

void CBTRegistryResponse::RunL()
	{
	LOG_FUNC
	switch (iState)
		{
		case EGettingSize:
			{
			// get the results
			if (iStatus.Int() >= KErrNone)
				{
				// we have a byte count of the response size
				iResponseBuf = HBufC8::NewMaxL(iStatus.Int());
				iResponsePtr.Set(iResponseBuf->Des());
				iView.GetResults(iResponsePtr, iStatus);
				iState = EGettingResults;
				SetActive();
				}
			else
				{
				User::Leave(iStatus.Int());
				}
			}
			break;
		case EGettingResults:
			{
			if (iStatus.Int() >= KErrNone)
				{
				// got n CBTDevices to return
				TInt c = iStatus.Int();
				RDesReadStream stream(iResponsePtr);
				CleanupClosePushL(stream);
				while (c-->0)
					{
					CBTDevice* device = CBTDevice::NewLC();
					device->InternalizeL(stream);
					// place it into the result array
					User::LeaveIfError(iArray.Append(device));
					CleanupStack::Pop();			//device - don't destroy as 'owned' by array
					}
				CleanupStack::PopAndDestroy();	//stream
				// free up memory:
				delete iResponseBuf;
				iResponseBuf = NULL;

				User::RequestComplete(iClientStatus, KErrNone);
				} // if
			else
				{
				User::Leave(iStatus.Int());
				}
			break;
			} // case
		default:
			User::Panic(KBTManClientPanic, EBTManClientBadResultRetrieveState);
		}

	}

TInt CBTRegistryResponse::RunError(TInt aError)
	{
	LOG_FUNC
	//complete request if its pending
	if (*iClientStatus == KRequestPending)
		{
		User::RequestComplete(iClientStatus, aError);
		}
	return KErrNone;
	}

/**
Default constructor
@publishedAll
@released
*/
EXPORT_C TBTRegistrySearch::TBTRegistrySearch()
	{
	LOG_FUNC
	Reset();
	}

/**
Resets the search criteria
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::Reset()
	{
	LOG_FUNC
	iSearchMask =0;
	}

/**	
Copy Constructor
@publishedAll
@released
*/
EXPORT_C TBTRegistrySearch::TBTRegistrySearch(const TBTRegistrySearch& aSearch)
	{
	LOG_FUNC
	iSearchMask = aSearch.iSearchMask;
	iDeviceClass = aSearch.iDeviceClass;
	iDeviceAddress = aSearch.iDeviceAddress;
	iLastSeen = aSearch.iLastSeen;
	iLastUsed = aSearch.iLastUsed;
	iCurrentProcessSID = aSearch.iCurrentProcessSID;
	iBluetoothName.Set(aSearch.iBluetoothName);
	iFriendlyName.Set(aSearch.iFriendlyName);
	}


/**
Add search criterion for bonded devices
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::FindBonded()
	{
	LOG_FUNC
	iSearchMask |= EBonded;
	}


/**
Add search criterion for trusted devices
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::FindTrusted()
	{
	LOG_FUNC
	iSearchMask |= ETrusted;
	}

/**
Add search criterion to search for specific CoD
@param aClass The CoD to search
@see TBTDeviceClass
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::FindCoD(const TBTDeviceClass& aClass)
	{
	LOG_FUNC
	iDeviceClass = aClass;
	iSearchMask |= ECoD;
	}


/**
Add search criterion to search for certain classes of device
@param aClass the device class to search for
@param aPref describes the type of device search to perform
@see TBTDeviceClass
@see enum TBTDeviceClassSearch
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::FindCoD(const TBTDeviceClass& aClass, TBTDeviceClassSearch aPref)
	{
	LOG_FUNC
	iDeviceClass = aClass;
	iSearchMask |= (aPref & EMajorDevice ? ECoDMajorDev : 0);
	iSearchMask |= (aPref & EMinorDevice ? ECoDMinorDev : 0);
	}

/**
Add search criterion to search for given device
@param	aAddress address of device to search for
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::FindAddress(const TBTDevAddr& aAddress)
	{
	LOG_FUNC
	iDeviceAddress = aAddress;
	iSearchMask |= EAddress;
	}


/**
Add search criterion to search for devices seen since given date
NOTE not currently implemented
@param	aLastSeen date from which to find devices
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::FindSinceSeen(const TTime& aLastSeen)
	{
	LOG_FUNC
	iLastSeen = aLastSeen;
	iSearchMask |= ELastSeen;
	}

/**
Add search criterion to search for devices used since given date
@param	aLastUsed date from which to find devices
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::FindSinceUsed(const TTime& aLastUsed)
	{
	LOG_FUNC
	iLastUsed = aLastUsed;
	iSearchMask |= ELastUsed;
	}



/**
Add search criterion to search for devices with some Bluetooth device name
@param aName device name to search for
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::FindBluetoothName(const TDesC8& aName)
/**
	Add search criteria of specific Bluetooth name
*/
	{
	LOG_FUNC
	iBluetoothName.Set(aName);
	iSearchMask |= EBTName;
	}

/**
Add search criterion to search for devices with some friendly name
@param aName device name to search for
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::FindFriendlyName(const TDesC& aName)
	{
	LOG_FUNC
	iFriendlyName.Set(aName);
	iSearchMask |= EFriendlyName;
	}

/**
Add search criterion to search for finding all devices
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::FindAll()
	{
	LOG_FUNC
	iSearchMask |= EAll;
	}

/**
Add search criterion to search for devices added by the current process
@publishedAll
@released
*/
EXPORT_C void TBTRegistrySearch::FindCurrentProcessOwned()
	{
	LOG_FUNC
	iCurrentProcessSID = User::Identity();
	iSearchMask |= EProcess;
	}

/**
Add search criterion to search for devices with a particular UI Cookie value.
@param aUiCookie The particular UI cookie value to search on.
@publishedPartner
@released
*/
EXPORT_C void TBTRegistrySearch::FindUiCookie(TUint32 aUiCookie)
	{
	LOG_FUNC
	static const TUint32 KAllBitsMasked = 0xffffffff;
	FindUiCookie(aUiCookie, KAllBitsMasked);
	}

/**
Add search criterion to search for devices with a particular UI Cookie value.
@param aUiCookie The particular UI cookie value to search on.
@param aUiCookieMask The mask to indicate which bits of the cookie value are to be searched on.
@publishedPartner
@released
*/
EXPORT_C void TBTRegistrySearch::FindUiCookie(TUint32 aUiCookie, TUint32 aUiCookieMask)
	{
	LOG_FUNC
	iUiCookie = aUiCookie;
	iUiCookieMask = aUiCookieMask;
	iSearchMask |= EUiCookie;
	}

/**
Assignment operator
@param aSearch the search pattern to which to assign this
@publishedAll
@released
*/
EXPORT_C TBTRegistrySearch& TBTRegistrySearch::operator=(const TBTRegistrySearch& aSearch)
	{
	LOG_FUNC
	if (this != &aSearch)
		{
		iSearchMask = aSearch.iSearchMask;
		iDeviceClass = aSearch.iDeviceClass;
		iDeviceAddress = aSearch.iDeviceAddress;
		iLastSeen = aSearch.iLastSeen;
		iLastUsed = aSearch.iLastUsed;
		iCurrentProcessSID = aSearch.iCurrentProcessSID;
		iUiCookie = aSearch.iUiCookie;
		iUiCookieMask = aSearch.iUiCookieMask;
		iBluetoothName.Set(aSearch.iBluetoothName);
		iFriendlyName.Set(aSearch.iFriendlyName);
		}
	return *this;
	}


