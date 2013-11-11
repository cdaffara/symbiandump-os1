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
//

#include <bluetooth/pairing.h>

#include <e32uid.h>
#include <e32base.h>
#include <bluetooth/pairingservershared.h>

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAIRING_SERVER);
#endif

//
// RBluetoothPairingServer
//

EXPORT_C RBluetoothPairingServer::RBluetoothPairingServer()
	{
	LOG_FUNC
	}

/**
@return The version of the pairing server the client library is compiled to operate with.
*/
EXPORT_C TVersion RBluetoothPairingServer::Version() const
	{
	LOG_FUNC
	return(TVersion(KPairingServerMajorVersionNumber, KPairingServerMinorVersionNumber, KPairingServerBuildVersionNumber));
	}

/**
Connects to the Bluetooth Pairing Server.
This must be successfully called before any other function can be used.
@return KErrNone if successful, otherwise the error that occurred
*/
EXPORT_C TInt RBluetoothPairingServer::Connect()
	{
	CONNECT_LOGGER
	LOG_FUNC
	__ASSERT_ALWAYS(!Handle(), PANIC(KPairingServerPanicCat, EPairingServerSessionAlreadyConnected));
	// Don't start the Bluetooth Protocol Stack. It's either loaded and we'll use it, or else this will fail
	return CreateSession(KPairingServerName, Version());
	}

/**
Releases all resources associated with this handle to the pairing server.
*/
EXPORT_C void RBluetoothPairingServer::Close()
	{
	LOG_FUNC
	RSessionBase::Close();
	CLOSE_LOGGER
	}

/**
Returns the session for use when connecting subsessions.
@internalComponent
*/
RSessionBase& RBluetoothPairingServer::Session()
	{
	LOG_FUNC
	return *static_cast<RSessionBase*>(this);
	}


//
// RBluetoothOobData
//

EXPORT_C RBluetoothOobData::RBluetoothOobData()
	{
	LOG_FUNC
	}

/**
Opens the OOB data session to the pairing server through the provided pairing
server session.
@param aPairingServer The pairing server session through which to establish the OOB data session.
@return KErrNone on success, otherwise a Symbian OS error code.
*/
EXPORT_C TInt RBluetoothOobData::Open(RBluetoothPairingServer& aPairingServer)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aPairingServer.Session().Handle(), PANIC(KPairingServerPanicCat, EPairingServerSessionNotConnected));
	__ASSERT_ALWAYS(!SubSessionHandle(), PANIC(KPairingServerPanicCat, EPairingServerOobDataSessionAlreadyOpen));
	return CreateSubSession(aPairingServer.Session(), EPairingServerCreateOobDataSubSession);
	}

/**
Closes the OOB data session and releases all resources associated with it.
Any outstanding asynchronous requests will be cancelled.
*/
EXPORT_C void RBluetoothOobData::Close()
	{
	LOG_FUNC
	RSubSessionBase::CloseSubSession(EPairingServerCloseSubSession);
	}

/**
This invalidates the cached OOB data that has been retrieved from the local Bluetooth
controller.  The next call to ReadLocalOobData will result in the new OOB data from the
controller.
@see RBluetoothOobData::ReadLocalOobData
*/
EXPORT_C void RBluetoothOobData::RefreshLocalOobData()
	{
	LOG_FUNC
	__ASSERT_ALWAYS(SubSessionHandle(), PANIC(KPairingServerPanicCat, EPairingServerOobDataSessionNotOpen));
	static_cast<void>(SendReceive(EPairingServerOobDataRefreshLocal));
	}

/**
Upon completion the provided parameters for the simple pairing hash and randomizer will be populated
with the current OOB data provided by the Bluetooth controller.  The data retrieved from the controller
is cached by the Bluetooth stack, and so if new data is required the RefreshLocalOobData function should
be called.
@see RBluetoothOobData::RefreshLocalOobData
@param aHash Upon completion this will hold the simple pairing hash C for the local Bluetooth controller.
@param aRandomizer Upon completion this will hold the simple pairing randomizer R for the local Bluetooth controller.
@param aStatus The TRequestStatus that will be completed when the operation has completed, its value will be
 KErrNone if successful, and a Symbian OS error code if there was a failure.  The aHash and aRandomizer values
 should be considered invalid if aStatus is not KErrNone.
*/
EXPORT_C void RBluetoothOobData::ReadLocalOobData(TBluetoothSimplePairingHash& aHash, TBluetoothSimplePairingRandomizer& aRandomizer, TRequestStatus& aStatus)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(SubSessionHandle(), PANIC(KPairingServerPanicCat, EPairingServerOobDataSessionNotOpen));
	TIpcArgs args(&aHash, &aRandomizer);
	SendReceive(EPairingServerOobDataReadLocal, args, aStatus);
	}

/**
Aborts the retrieval of OOB data for the local Bluetooth controller.
@see RBluetoothOobData::ReadLocalOobData
*/
EXPORT_C void RBluetoothOobData::CancelReadLocalOobData()
	{
	LOG_FUNC
	__ASSERT_ALWAYS(SubSessionHandle(), PANIC(KPairingServerPanicCat, EPairingServerOobDataSessionNotOpen));
	static_cast<void>(SendReceive(EPairingServerOobDataCancelReadLocal));
	}

/**
This function is to allow the provision of OOB data retrieved from remote Bluetooth devices into
the Bluetooth stack for use when pairing.  This version of the function is for when the device address, 
hash and randomizer have already been parsed from an OOB data packet.
The OOB data provided will replace any existing data that has previously been provided.
@param aAddr The Bluetooth device address for the remote device whose OOB pairing data has been received.
@param aHash The simple pairing hash C value for the remote device.
@param aRandomizer The simple pairing randomizer R value for the remote device.
@return KErrNone if successful, otherwise a Symbian OS error code.
*/
EXPORT_C TInt RBluetoothOobData::ProvideRemoteOobData(const TBTDevAddr& aAddr, const TBluetoothSimplePairingHash& aHash, const TBluetoothSimplePairingRandomizer& aRandomizer)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(SubSessionHandle(), PANIC(KPairingServerPanicCat, EPairingServerOobDataSessionNotOpen));
	TPckgBuf<TBTDevAddr> addrBuf(aAddr);
	TIpcArgs args(&addrBuf, &aHash, &aRandomizer);
	return SendReceive(EPairingServerOobDataProvideRemoteParsed, args);
	}

/**
This function is to allow the provision of OOB data retrieved from remote Bluetooth devices into
the Bluetooth stack for use when pairing.  This version of the function is for when the device address 
and hash have already been parsed from an OOB data packet. (The randomizer value is not available).
The OOB data provided will replace any existing data that has previously been provided.
@param aAddr The Bluetooth device address for the remote device whose OOB pairing data has been received.
@param aHash The simple pairing hash C value for the remote device.
@return KErrNone if successful, otherwise a Symbian OS error code.
*/
EXPORT_C TInt RBluetoothOobData::ProvideRemoteOobData(const TBTDevAddr& aAddr, const TBluetoothSimplePairingHash& aHash)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(SubSessionHandle(), PANIC(KPairingServerPanicCat, EPairingServerOobDataSessionNotOpen));
	TPckgBuf<TBTDevAddr> addrBuf(aAddr);
	TIpcArgs args(&addrBuf, &aHash);
	return SendReceive(EPairingServerOobDataProvideRemoteParsed, args);
	}

/**
This function is to allow the provision of OOB data retrieved from remote Bluetooth devices into
the Bluetooth stack for use when pairing.  This version of the function is for when OOB data has been
received from a remote device but has not been parsed by the manager of OOB data.
The OOB data provided will replace any existing data that has previously been provided.
@param aData The raw OOB data that may contain pairing data.
@return KErrNone if successful, otherwise a Symbian OS error code.
*/
EXPORT_C TInt RBluetoothOobData::ProvideRemoteOobData(const TDesC8& aData)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(SubSessionHandle(), PANIC(KPairingServerPanicCat, EPairingServerOobDataSessionNotOpen));
	TIpcArgs args(&aData);
	return SendReceive(EPairingServerOobDataProvideRemoteRaw, args);
	}

/**
When the OOB data from a remote device is no longer valid, this function should be called.  This will
remove any OOB data associated with a particular remote device.
@param aAddr The Bluetooth device address for the remote device whose OOB pairing data is no longer valid.
*/
EXPORT_C void RBluetoothOobData::ClearRemoteOobData(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(SubSessionHandle(), PANIC(KPairingServerPanicCat, EPairingServerOobDataSessionNotOpen));
	TPckgBuf<TBTDevAddr> addrBuf(aAddr);
	TIpcArgs args(&addrBuf);
	static_cast<void>(SendReceive(EPairingServerOobDataClearRemote, args));
	}


//
// RBluetoothDedicatedBondingInitiator
//

EXPORT_C RBluetoothDedicatedBondingInitiator::RBluetoothDedicatedBondingInitiator()
	{
	LOG_FUNC
	}

/**
Initiates a dedicated bond with a remote Bluetooth device.

Once this function is called, the RBluetoothDedicatedBondingInitiator::Close function must
be called before RBluetoothDedicatedBondingInitiator::Start the instance can be called again.

@param aPairingServer The pairing server session through which to initiate the request.
@param aAddr The Bluetooth device address of the remote device to initiate a dedicated bond with.
@param aStatus Completed when the dedicated bonding process has completed with a value of KErrNone
				if successful, otherwise a Symbian OS error.
*/
EXPORT_C void RBluetoothDedicatedBondingInitiator::Start(RBluetoothPairingServer& aPairingServer, const TBTDevAddr& aAddr, TRequestStatus& aStatus)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aPairingServer.Session().Handle(), PANIC(KPairingServerPanicCat, EPairingServerSessionNotConnected));
	__ASSERT_ALWAYS(!SubSessionHandle(), PANIC(KPairingServerPanicCat, EPairingServerDedicatedBondAlreadyIssued));
	
	// This function encapsulates a connection to the pairing server and a request for a dedicated bond.
	TInt err = CreateSubSession(aPairingServer.Session(), EPairingServerCreateDedicatedBondingSubSession);
	if(err != KErrNone)
		{
		// Error so locally complete TRequestStatus.
		aStatus = KRequestPending;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		return;
		}
	
	iDedicatedBondAddress() = aAddr;
	TIpcArgs args(&iDedicatedBondAddress);
	SendReceive(EPairingServerStartDedicatedBond, args, aStatus);
	}

/**
Releases the resources associated with the dedicated bonding initiator.
If the bonding is in progress, it will be cancelled.
*/
EXPORT_C void RBluetoothDedicatedBondingInitiator::Close()
	{
	LOG_FUNC
	RSubSessionBase::CloseSubSession(EPairingServerCloseSubSession);
	}


//
// RBluetoothPairingResult
//

EXPORT_C RBluetoothPairingResult::RBluetoothPairingResult()
	: iDeviceAddressBuffer(NULL, 0, 0), iRequest(NULL)
	{
	LOG_FUNC
	}

/**
Opens the pairing result session to the pairing server through the provided pairing
server session.
The pairing result session is used to obtain results of simple pairing attempts 
using the Numeric Comparison or Passkey Entry association models.
@param aPairingServer The pairing server session through which to establish the pairing result session.
@return KErrNone on success, otherwise a Symbian OS error code.
*/
EXPORT_C TInt RBluetoothPairingResult::Open(RBluetoothPairingServer& aPairingServer)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aPairingServer.Session().Handle(), PANIC(KPairingServerPanicCat, EPairingServerSessionNotConnected));
	__ASSERT_ALWAYS(!SubSessionHandle(), PANIC(KPairingServerPanicCat, EPairingServerSimplePairingResultSessionAlreadyOpen));
	return CreateSubSession(aPairingServer.Session(), EPairingServerCreateSimplePairingResultSubSession);
	}

/**
Closes the pairing result session and releases all resources associated with it.
Any outstanding asynchronous requests will be cancelled.
*/
EXPORT_C void RBluetoothPairingResult::Close()
	{
	LOG_FUNC
	RSubSessionBase::CloseSubSession(EPairingServerCloseSubSession);
	}

/**
Upon successful completion the TBTDevAddr parameter will be populated with the Bluetooth device address
of the remote device with which a pairing attempt has been made.
Only a single request can be queued on an RBluetoothPairingResult instance.

By the limitations of the Bluetooth specification, the result of PIN code entry based pairings will not
be reported.

@param aAddr Upon completion this will hold the Bluetooth device address for the remote device with 
 which pairing was attempted.
@param aStatus The TRequestStatus that will be completed when the operation has completed, its value 
 will either be a HCI error code (in the Symbian error code space) representing the result of the
 pairing, or a Symbian OS system-wide error code if there was a failure.  The aAddr value should be
  considered invalid if aStatus is not in the HCI error code range.
*/
EXPORT_C void RBluetoothPairingResult::SimplePairingResult(TBTDevAddr& aAddr, TRequestStatus& aStatus)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(!iRequest || *iRequest != KRequestPending, 
		PANIC(KPairingServerPanicCat, EPairingServerSimplePairingResultRequestOutstanding));
	iRequest = &aStatus;
	iDeviceAddressBuffer.Set(TPckg<TBTDevAddr>(aAddr));
	TIpcArgs args(&iDeviceAddressBuffer);
	SendReceive(EPairingServerSimplePairingResult, args, aStatus);
	}

/**
Cancels any outstanding request for a simple pairing result.
*/
EXPORT_C void RBluetoothPairingResult::CancelSimplePairingResult()
	{
	LOG_FUNC
	SendReceive(EPairingServerCancelSimplePairingResult);
	}


//
// RBluetoothAuthenticationResult
//

EXPORT_C RBluetoothAuthenticationResult::RBluetoothAuthenticationResult()
	: iDeviceAddressBuffer(NULL, 0, 0),iRequest(NULL)
	{
	LOG_FUNC
	}

/**
Opens the authentication result session to the pairing server through the provided pairing
server session.
The authentication result session is used to obtain results of authentication complete events, 
which are generated for both simple and legacy pairing attempts.
@param aPairingServer The pairing server session through which to establish the authentication result session.
@return KErrNone on success, otherwise a Symbian OS error code.
*/
EXPORT_C TInt RBluetoothAuthenticationResult::Open(RBluetoothPairingServer& aPairingServer)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(aPairingServer.Session().Handle(), PANIC(KPairingServerPanicCat, EPairingServerSessionNotConnected));
	__ASSERT_ALWAYS(!SubSessionHandle(), PANIC(KPairingServerPanicCat, EPairingServerAuthenticationResultSessionAlreadyOpen));
	return CreateSubSession(aPairingServer.Session(), EPairingServerCreateAuthenticationResultSubSession);
	}

/**
Closes the authentication result session and releases all resources associated with it.
Any outstanding asynchronous requests will be cancelled.
*/
EXPORT_C void RBluetoothAuthenticationResult::Close()
	{
	LOG_FUNC
	RSubSessionBase::CloseSubSession(EPairingServerCloseSubSession);
	}

/**
Upon successful completion the TBTDevAddr parameter will be populated with the Bluetooth device address
of the remote device with which an authentication attempt has been made.
Only a single request can be queued on an RBluetoothAuthenticationResult instance.

By the limitations of the Bluetooth specification, the result of PIN code entry based pairings will not
be reported.

@param aAddr Upon completion this will hold the Bluetooth device address for the remote device with
 which authentication was attempted.
@param aStatus The TRequestStatus that will be completed when the operation has completed, its value
 will either be a HCI error code (in the Symbian error code space) representing the result of the
 authentication, or a Symbian OS system-wide error code if there was a failure.  The aAddr value should be
  considered invalid if aStatus is not in the HCI error code range.
*/
EXPORT_C void RBluetoothAuthenticationResult::AuthenticationResult(TBTDevAddr& aAddr, TRequestStatus& aStatus)
	{
	LOG_FUNC
	__ASSERT_ALWAYS(!iRequest || *iRequest != KRequestPending,
		PANIC(KPairingServerPanicCat, EPairingServerAuthenticationResultRequestOutstanding));
	iRequest = &aStatus;
	iDeviceAddressBuffer.Set(TPckg<TBTDevAddr>(aAddr));
	TIpcArgs args(&iDeviceAddressBuffer);
	SendReceive(EPairingServerAuthenticationResult, args, aStatus);
	}

/**
Cancels any outstanding request for an authentication result.
*/
EXPORT_C void RBluetoothAuthenticationResult::CancelAuthenticationResult()
	{
	LOG_FUNC
	SendReceive(EPairingServerCancelAuthenticationResult);
	}



