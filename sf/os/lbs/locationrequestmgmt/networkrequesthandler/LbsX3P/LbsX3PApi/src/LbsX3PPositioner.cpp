// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32mem.h>
#include <lbs/lbsx3p.h>
#include <lbs.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbslocerrors.h>
#include <lbs/lbslocclasstypes.h>
#include "LbsX3PImpl.h"
#include "lbsdevloggermacros.h"

// Version of Lbs Transfer Position To Third Party
const TInt8 KPosX3PMajorVersionNumber 	= 1;
const TInt8 KPosX3PMinorVersionNumber 	= 0;
const TInt16 KPosX3PBuildVersionNumber	= 0;

//-----------------------------------------------------------------------------
// RLbsTransmitPositionServer
//-----------------------------------------------------------------------------
EXPORT_C RLbsTransmitPositionServer::RLbsTransmitPositionServer()
/**
Constructor for RLbsTransmitPositionServer

 */ 
 	: iImpl(NULL)
 	{
 	LBSLOG(ELogP9, "->S RLbsTransmitPositionServer::() lbsnetworkrequesthandler.exe\n");
	}

EXPORT_C void RLbsTransmitPositionServer::ConstructL()
/**
Leaving constructor for RLbsTransmitPositionServer. Just creates the ptr-holder.
@internalComponent
 */
	{
	__ASSERT_ALWAYS(iImpl == NULL, User::Panic(KLbsLocFrameworkClientFault, EPositionServerHandleNotClosed));
	
	iImpl = CLbsTransmitPositionServerImpl::NewL();
	}

EXPORT_C void RLbsTransmitPositionServer::Destruct()
/**
Destruction method for RLbsTransmitPositionServer. Just deletes the ptr-holder.
@internalComponent 
@capability None
 */
	{
	delete iImpl;
	iImpl = NULL;
	}

EXPORT_C TInt RLbsTransmitPositionServer::Connect()
/**
Creates a session with the positioning server. If there are no other
sessions when this is called, the method will start the server.

@capability Location NetworkServices
@return a Symbian OS error code.
 */
	{
	LBSLOG(ELogP9, "->S RLbsTransmitPositionServer::Connect() lbsnetworkrequesthandler.exe\n");
	TInt err;
	TRAP(err, ConstructL());
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}

EXPORT_C void RLbsTransmitPositionServer::Close()
/**
Closes a session with the positioning server.

@capability None
 */
	{
	LBSLOG(ELogP9, "->S RLbsTransmitPositionServer::Close() lbsnetworkrequesthandler.exe\n");
	Destruct();
	}


EXPORT_C TVersion RLbsTransmitPositionServer::Version() const
/**
Obtains the current version number of the location server.

@capability None
@return the version of the client API.
 */
	{
	LBSLOG(ELogP9, "->S RLbsTransmitPositionServer::Version() lbsnetworkrequesthandler.exe\n");
	LBSLOG4(ELogP9, "  Return TVersion  = (%d, %d, %d)\n", KPosX3PMajorVersionNumber, KPosX3PMinorVersionNumber, 
																KPosX3PBuildVersionNumber);
	return TVersion(
		KPosX3PMajorVersionNumber,
		KPosX3PMinorVersionNumber,
		KPosX3PBuildVersionNumber);
	}

/**
Returns reference to implementation. Impl() is for internal use only.
@internalComponent
@released
@return Returns reference to implementation. Impl() is for internal use only.
 */
CLbsTransmitPositionServerImpl& RLbsTransmitPositionServer::Impl()
	{
	__ASSERT_ALWAYS(iImpl != NULL, User::Panic(KLbsLocFrameworkClientFault, EPositionServerBadHandle));
	return *iImpl;
	}

/**
Reserved for future expansion - derived classes should see documentation
@param aFunctionNumber contains the Id of the function to be invoked.
@param aPtr1 a pointer to any data 
@param aPtr2 a pointer to any data.
@return TAny* pointer to any function
 */
EXPORT_C TAny* RLbsTransmitPositionServer::ExtendedInterface(TInt /*aFunctionNumber*/, TAny* /*aPtr1*/, TAny* /*aPtr2*/)
	{
	return NULL;
	}

//-----------------------------------------------------------------------------
// RLbsTransmitPosition
//-----------------------------------------------------------------------------
EXPORT_C RLbsTransmitPosition::RLbsTransmitPosition()
/**
Constructor for RLbsTransmitPosition


 */
 	: iImpl(NULL)
	{
	LBSLOG(ELogP9, "->S RLbsTransmitPosition::RLbsTransmitPosition() lbsnetworkrequesthandler.exe\n");
	}

EXPORT_C void RLbsTransmitPosition::ConstructL(RLbsTransmitPositionServer& aServer)
/**
Leaving constructor for RLbsTransmitPosition. Just creates the ptr-holder.
@internalComponent
@param aServer Position Server
@released  
 */
	{
	__ASSERT_ALWAYS(iImpl == NULL, User::Panic(KLbsLocFrameworkClientFault, EPositionServerHandleNotClosed));
	
	iImpl = CLbsTransmitPositionImpl::NewL(aServer);
	}

EXPORT_C void RLbsTransmitPosition::Destruct()
/**
Destruction method for RLbsTransmitPosition.
@internalComponent
@released 
 */
	{
	delete iImpl;
	iImpl = NULL;
	}

EXPORT_C TInt RLbsTransmitPosition::Open(RLbsTransmitPositionServer& aServer)
/**
Creates a sub-session with the positioning server.

@capability Location NetworkServices
@param aServer is a connected session with the positioning server.
@return a Symbian OS error code.
 */
	{
	LBSLOG(ELogP9, "->S RLbsTransmitPosition::Open() lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > RLbsTransmitPositionServer aServer  = 0x%08X\n", &aServer);
	TInt err;
	TRAP(err, ConstructL(aServer));
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}


EXPORT_C void RLbsTransmitPosition::Close()
/**
Closes a sub-session with the positioning server.

 */
	{
	LBSLOG(ELogP9, "->S RLbsTransmitPosition::Close() lbsnetworkrequesthandler.exe\n");
	Destruct();
	}

/**
This is method is used to set the options that will be used when
requesting that the current position be sent to a third party.

@capability Location NetworkServices
@param aTransmitOptions is the options to be used.
@return a Symbian OS error code.
 */
EXPORT_C TInt RLbsTransmitPosition::SetTransmitOptions(const TLbsTransmitPositionOptions& aTransmitOptions)
	{
	LBSLOG(ELogP9, "->S RLbsTransmitPosition::SetTransmitOptions() lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > TLbsTransmitPositionOptions aTransmitOptions  = %ld\n", aTransmitOptions.TimeOut().Int64());
	__ASSERT_ALWAYS(iImpl != NULL, User::Panic(KLbsLocFrameworkClientFault, 
										EPositionServerBadHandle));
	
	TInt err = iImpl->SetTransmitOptions(aTransmitOptions);
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}

/**
This is method is used to get the current setting for the options that will be used when
requesting that the current position be sent to a third party.

@capability Location NetworkServices
@param aTransmitOptions is the options currently set.
@return a Symbian OS error code.
 */
EXPORT_C TInt RLbsTransmitPosition::GetTransmitOptions(TLbsTransmitPositionOptions& aTransmitOptions) const
	{
	LBSLOG(ELogP9, "->S RLbsTransmitPosition::GetTransmitOptions() lbsnetworkrequesthandler.exe\n");
	__ASSERT_ALWAYS(iImpl != NULL, User::Panic(KLbsLocFrameworkClientFault, 
										EPositionServerBadHandle));
	
	TInt err = iImpl->GetTransmitOptions(aTransmitOptions);
	LBSLOG2(ELogP9, "  < TLbsTransmitPositionOptions aTransmitOptions  = %ld\n", aTransmitOptions.TimeOut().Int64());
	LBSLOG2(ELogP9, "  Return  = %d\n", err);
	return err;
	}


 /**
This is an asynchronous method for requesting that the current position be sent to a third 
party. Upon successful completion, the position sent will be returned to the caller in aTransmittedPosInfo.


@capability Location NetworkServices
@param aDestinationID      identifies the third party. For example, it may be 
the phone number or email address of the recipient of the position.
@param aTransmitPriority   Priority that this request must be given. The priority
determines the order in which transfer requests are processed.
A priority of zero is the highest priority.
@param aTransmittedStatus  TRequestStatus of the Active Object whose RunL will be called
when this asynchronous request completes (the value should be checked by the caller to ensure
that the operation was successful).
@param aTransmittedPosInfo When the request completes succesfully, this variable will store 
the position of the handset that was made available to the remote third party . This position
might have been calculated by an on-board GPS receiver (terminal based and autonomous GPS modes)
or may have been calculated by the network (terminal assisted mode).
 */
EXPORT_C void RLbsTransmitPosition::TransmitPosition(const TDesC& aDestinationID, 
													 TUint aTransmitPriority, 
													 TRequestStatus& aTransmittedPosStatus, 
													 TPositionInfo& aTransmittedPosInfo)
	{
	LBSLOG(ELogP9, "->A RLbsTransmitPosition::TransmitPosition(4) lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > TDesC aDestinationID  = %S\n", &aDestinationID);
	LBSLOG2(ELogP9, "  > TRequestStatus aTransmittedPosStatus  = %d\n", aTransmittedPosStatus.Int());
	LBSLOG2(ELogP9, "  > TUint aTransmitPriority  = %u\n", aTransmitPriority);

	__ASSERT_ALWAYS(iImpl != NULL, User::Panic(KLbsLocFrameworkClientFault, 
										EPositionServerBadHandle));
	
	iImpl->TransmitPosition(aDestinationID, aTransmitPriority,
							aTransmittedPosStatus, aTransmittedPosInfo);
	}

/**
This asynchronous method is used to request that the current handset position be transmitted
to a third party.

If there are no errors or cancellations, invoking this method will result in the user
being called back twice and given two sequencial location upates.

The first location update will take place when aRefPosStatus is signalled and will consist of
a Reference Location which is provided by the network to the handset in the early stages
of the transmition procedure. This position is made available to the caller in aRefPosInfo.

The second location update will take place when aTransmittedPosStatus is signalled. When that
happens aTransmitedPosInfo will contain the final position sent to the third party. This is a
more accurate position than the Reference Location and is calculated (either in the handset 
or in the network) with GPS data collected by the handset.


@capability Location NetworkServices
@param aDestinationID      identifies the third party. For example, it may be
the phone number or email address of the recipient of the position.
@param aTransmitPriority   Priority that this request must be given. The priority
determines the order in which transfer requests are processed. A priority of zero is the highest priority.
@param aRefPosStatus  TRequestStatus of the Active Object whose RunL will be called
as soon as the Reference Location is available in aRefPosInfo. The status value should be checked by the 
caller to ensure that the operation was successful.
@param aRefPosInfo When aRefPosStatus is signalled, this variable will contain the Reference 
Location provided by the network.
@param aTransmittedPosStatus TRequestStatus of the Active Object whose RunL will be called
when the transmition has completed. The status value should be checked by the caller to ensure
that the operation was successful, in which case the transmitted position is available in aTransmittedPosInfo.
@param aTransmittedPosInfo When the request completes succesfully, this variable will store 
the position of the handset that was made available to the remote third party . This position
might have been calculated by an on-board GPS receiver (terminal based and autonomous GPS modes)
or may have been calculated by the network (terminal assisted mode).
 */
EXPORT_C void RLbsTransmitPosition::TransmitPosition(const TDesC& aDestinationID,
													 TUint aTransmitPriority, 
													 TRequestStatus& aRefPosStatus,
													 TPositionInfo& aRefPosInfo,
													 TRequestStatus& aTransmittedPosStatus,
													 TPositionInfo& aTransmittedPosInfo)
	{
	LBSLOG(ELogP9, "->A RLbsTransmitPosition::TransmitPosition(6) lbsnetworkrequesthandler.exe\n");
	LBSLOG2(ELogP9, "  > TDesC aDestinationID  = %S\n", &aDestinationID);
	LBSLOG2(ELogP9, "  > TUint aTransmitPriority  = %d\n", aTransmitPriority);
	LBSLOG2(ELogP9, "  > TRequestStatus aRefPosStatus  = %d\n", aRefPosStatus.Int());
	LBSLOG2(ELogP9, "  > TRequestStatus aTransmittedPosStatus  = %d\n", aTransmittedPosStatus.Int());

	__ASSERT_ALWAYS(iImpl != NULL, User::Panic(KLbsLocFrameworkClientFault, 
										EPositionServerBadHandle));
	
	iImpl->TransmitPosition(aDestinationID, aTransmitPriority,
							aRefPosStatus, aRefPosInfo,
							aTransmittedPosStatus, aTransmittedPosInfo);
	}


EXPORT_C void RLbsTransmitPosition::CancelTransmitPosition()
/**
Cancels a previously issued asynchronous request. The TRequestStatus of the
original request will be set to KErrCancel if the cancellation request was
successful, or any other Symbian OS error code if CancelTransmitPosition was
called too late.
 
@capability Location NetworkServices
 */
	{
	LBSLOG(ELogP9, "->S RLbsTransmitPosition::CancelTransmitPosition() lbsnetworkrequesthandler.exe\n");
	__ASSERT_ALWAYS(iImpl != NULL, User::Panic(KLbsLocFrameworkClientFault, 
										EPositionServerBadHandle));
	
	iImpl->CancelTransmitPosition();
	}

/**
Reserved for future expansion - derived classes should see documentation
@param aFunctionNumber contains the Id of the function to be invoked.
@param aPtr1 a pointer to any data 
@param aPtr2 a pointer to any data
@return TAny* pointer to any function
 */
EXPORT_C TAny* RLbsTransmitPosition::ExtendedInterface(TInt /*aFunctionNumber*/, TAny* /*aPtr1*/, TAny* /*aPtr2*/)
	{
	LBSLOG(ELogP9, "->S RLbsTransmitPosition::ExtendedInterface() lbsnetworkrequesthandler.exe\n");
	return NULL;
	}

//-----------------------------------------------------------------------------
// TLbsTransmitPositionOptions
//-----------------------------------------------------------------------------	
/**
This is the constructor for TTransmitPositionOptions. The time out period 
for the TransmitPosition requests is set to the default value thus 
timeouts are disabled.

 */
EXPORT_C TLbsTransmitPositionOptions::TLbsTransmitPositionOptions()
: TLbsLocClassTypeBase()
	{
	iClassType |= ETransmitPositionOptionsClass;
	iClassSize = sizeof(TLbsTransmitPositionOptions);	
	iTimeOut = 0;
	}


/**
This  constructor for TTransmitPositionOptions which allows the client 
to define the time out period for the TransmitPosition requests. The value
zero may be used to disable timeouts. 

@param aTimeOut identifies the time out period
  */
EXPORT_C TLbsTransmitPositionOptions::TLbsTransmitPositionOptions(const TTimeIntervalMicroSeconds& aTimeOut)
: TLbsLocClassTypeBase()
	{
	iClassType |= ETransmitPositionOptionsClass;
	iClassSize = sizeof(TLbsTransmitPositionOptions);
	SetTimeOut(aTimeOut); // call function so it will panic on -ve timeout
	}

/**
SetTimeOut allows the client to define the time out period for the 
TransmitPosition requests. The value  zero may be used to disable timeouts.

@param aTimeOut identifies the time out period
 */
EXPORT_C void TLbsTransmitPositionOptions::SetTimeOut(const TTimeIntervalMicroSeconds& aTimeOut)
	{
	__ASSERT_ALWAYS(aTimeOut >= TTimeIntervalMicroSeconds(0),
			User::Panic(KPosClientFault, EPositionBadTime));
	iTimeOut = aTimeOut;
	}

/**
TimeOut returns the current timeout period for the TransmitPosition requests.

@return the time out period in micro seconds
 */

EXPORT_C TTimeIntervalMicroSeconds TLbsTransmitPositionOptions::TimeOut() const
	{
	return iTimeOut;
	}

