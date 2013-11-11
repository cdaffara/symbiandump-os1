// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <comms-infras/nifprvar.h>
#include <es_sock.h>
#include <comms-infras/sockmes.h>
#include <connpref.h>
#include "es_flog.h"
#include <comms-infras/es_commsdataobject.h>
#include <es_sock_partner.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCSockRCnctn, "ESockCSockRCnctn");
#endif


// Forward declearation
namespace ESock
{
class XConnectionQueryBase;
}

EXPORT_C RConnection::RConnection()
	: iNewISPId(0)	//lint -esym(1401, RConnection::iReserved)	// no BC issue as ctor not inlined
/** Empty constructor. */
	{
	}

EXPORT_C RConnection::~RConnection()
/** Empty destructor. */
	{
	}

EXPORT_C TInt RConnection::Open(RSocketServ& aSocketServer, TUint aConnectionType)
/** Opens a new RConnection instance.

@param aSocketServer Socket Server session.
@param aConnectionType Reserved.
@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	RSessionBase& s = aSocketServer;
	LOG( ESockLog::Printf(_L8("RConnection %08x:\tOpen() tid %d"), this, (TUint)RThread().Id()));
	return CreateSubSession(s, ECNCreate, TIpcArgs(aConnectionType));
	}

EXPORT_C TInt RConnection::Open(RSocketServ& aSocketServer, TName& aName)
/** Opens a new RConnection instance cloned from an existing RConnection instance.

@param aSocketServer Socket Server session.
@param aName Name of an existing RConnection (obtainable via Name() method).
@return KErrNone if successful, otherwise another of the system wide error
codes.
@capability Dependent on the type of connection so deferred to PRT */
	{
	RSessionBase& s = aSocketServer;
	return CreateSubSession(s, ECNCreateWithName, TIpcArgs(&aName));
	}

EXPORT_C void RConnection::Close()
/** Closes the connection.

The connection will not be dropped immediately: it will be dropped when there
is no more data traffic on the connection. So if a client needs to graciously
shutdown the connection, Close(), not Stop(), needs to be used after shutting
down the socket.
*/
	{
	LOG( ESockLog::Printf(_L8("RConnection %08x:\tClose() tid %d"), this, (TUint)RThread().Id()));
	CloseSubSession(ECNClose);
	}

EXPORT_C void RConnection::Start(TRequestStatus& aStatus)
/** Start a connection asynchonously using the default connection preferences.

The request completes once the connection is fully up or an error has occurred.
Successful completion will be accompanied by a KConnectionUp progress. 

If an RConnection has been stopped or has gone down, any subsequent start will
act in the same way as a start with a new RConnection. i.e. The preferences from
previous starts will not be used.

@param aStatus On return, the status of the request, e.g. KErrNone, KErrAlreadyExists.
@capability Dependent on the access point implementation */
	{
	LOG( ESockLog::Printf(_L8("RConnection %08x:\tStart() tid %d"), this, (TUint)RThread().Id()));
	SendReceive(ECNStart, TIpcArgs(), aStatus);
	}

EXPORT_C void RConnection::Start(TConnPref& aPref, TRequestStatus& aStatus)
/** Start a connection asynchonously using the connection preferences specified.

The connection preferences can be of type TConnSnapPref, TCommDbConnPref,
TCommDbMultiConnPref or TConnPrefList. See the definitions of these classes to see
their usage. 

The request completes once the connection is fully up or an error has occurred.
Successful completion will be accompanied by a KConnectionUp progress. 

If an RConnection has been stopped or has gone down, any subsequent start will
act in the same way as a start with a new RConnection. i.e. The preferences from
previous starts will not be used.

@see TConnSnapPref
@see TCommDbConnPref
@see TCommDbMultiConnPref
@see TConnPrefList

@param aPref Connection preferences.
@param aStatus On return, the status of the request, e.g. KErrNone, KErrAlreadyExists.

@capability Dependent on the access point implementation */
	{
	LOG( ESockLog::Printf(_L8("RConnection %08x:\tStart() with preferences, tid %d"), this, (TUint)RThread().Id()));
	
	if (aPref.ExtensionId() != TConnPref::EConnPrefEComList)
		{
		SendReceive(ECNSetStartPrefs, TIpcArgs(&aPref));
		}
	else
		{
		TConnPrefList& custPref = static_cast<TConnPrefList&>(aPref);
		RBuf8 prefFlatteningBuffer;
		TInt err = prefFlatteningBuffer.Create(custPref.Length());
        if (err == KErrNone)
            {
            err = custPref.Store(prefFlatteningBuffer);
            }
		
        if (err != KErrNone)
            {
            prefFlatteningBuffer.Close();
            aStatus = err;
            return;
            }

        SendReceive(ECNSetStartPrefs, TIpcArgs(&prefFlatteningBuffer));
        prefFlatteningBuffer.Close();
		}
	
    SendReceive(ECNStart, TIpcArgs(), aStatus);
	}

EXPORT_C TInt RConnection::Start()
/** Start a connection synchonously using the default connection preferences.

The request completes once the connection is fully up or an error has occurred.
Successful completion will be accompanied by a KConnectionUp progress. 

If an RConnection has been stopped or has gone down, any subsequent start will
act in the same way as a start with a new RConnection. i.e. The preferences from
previous starts will not be used.

@return KErrNone if successful or the connection already exists, otherwise
another of the system wide error codes.
@capability Dependent on the access point implementation */
	{
	TRequestStatus status;
	Start(status);
	User::WaitForRequest(status);
	TInt ret = status.Int();

	if (ret == KErrAlreadyExists)
	    {
	    return KErrNone;
	    }

	return ret;
	}

EXPORT_C TInt RConnection::Start(TConnPref& aPref)
/** Start a connection synchonously using the connection preferences specified.

The connection preferences can be of type TConnSnapPref, TCommDbConnPref,
TCommDbMultiConnPref or TConnPrefList. See the definitions of these classes to see
their usage. 

The request completes once the connection is fully up or an error has occurred.
Successful completion will be accompanied by a KConnectionUp progress. 

If an RConnection has been stopped or has gone down, any subsequent start will
act in the same way as a start with a new RConnection. i.e. The preferences from
previous starts will not be used.

@see TConnSnapPref
@see TCommDbConnPref
@see TCommDbMultiConnPref
@see TConnPrefList

@param aPref Connection preferences.
@return KErrNone if successful or the connection already exists, otherwise
another of the system wide error codes.
@capability Dependent on the access point implementation */
	{
	TRequestStatus status;
	Start(aPref, status);
	User::WaitForRequest(status);
	TInt ret = status.Int();

    if (ret == KErrAlreadyExists)
        {
        return KErrNone;
        }

	return ret;
	}

EXPORT_C TInt RConnection::Stop()
/** Stops the entire connection by disconnecting the underlying network interface
immediately, regardless of whether other clients are using it or not.

Applications using the connection will be sent the socket error code KErrCancel.
The application generally responds with clean up operations and pop-up boxes
alerting the user to the termination of the application.

@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	LOG( ESockLog::Printf(_L8("RConnection %08x:\tStop() tid %d"), this, (TUint)RThread().Id()));
	return Stop(EStopNormal);
	}

EXPORT_C TInt RConnection::Stop(TConnStopType aStopType)
/** Stops the entire connection by disconnecting the underlying network interface
immediately, regardless of whether other clients are using it or not.

If the argument is EStopNormal this is identical to calling Stop() with no
argument. If it is EStopAuthoritative then applications using the connection
will be sent the socket error code KErrConnectionTerminated, which generally
results in the applications closing quietly (without pop-up boxes).

@param aStopType The type of stop which is being requested.
@return KErrNone or another of the system wide error codes; in particular KErrArgument
if the stop type is unrecognised. */
    {
	LOG( ESockLog::Printf(_L8("RConnection %08x:\tStop(TConnStopType = %d) tid %d"), this, aStopType, (TUint)RThread().Id()));
	return SendReceive(ECNStop, TIpcArgs(aStopType));
    }

EXPORT_C TInt RConnection::Stop(TSubConnectionUniqueId aSubConnectionUniqueId)
/** Stops a subconnection.

Applications will be sent a socket error code to indicate that they must perform
clean up operations.

@publishedPartner
@released since v7.0s
@pre An attached connection: as a result of performing either a Start() or an Attach()
@param aSubConnectionUniqueId A valid identifier for a subconnection.
@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	LOG( ESockLog::Printf(_L8("RConnection %08x:\tStop(TSubConnectionUniqueId = %d) tid %d"), this, aSubConnectionUniqueId, (TUint)RThread().Id()));
	return Stop(aSubConnectionUniqueId, EStopNormal);
	}

EXPORT_C TInt RConnection::Stop(TSubConnectionUniqueId aSubConnectionUniqueId, TConnStopType aStopType)
/**
 * Stop a subconnection on an interface
 *
 * @publishedPartner
 * @released since v7.0s
 * @pre Must be attached to a connection; either by performing a Start(), or using Attach()
 * @param aSubConnectionUniqueId A valid identifier for a subconnection
 * @param aStopType The kind of stop to do (controls what error code is reported to interface clients)
 * @return KErrNone if successful, otherwise one of the system-wide error codes
 */
	{
	LOG( ESockLog::Printf(_L8("RConnection %08x:\tStop(TSubConnectionUniqueId = %d, TConnStopType = %d) tid %d"), this, aSubConnectionUniqueId, aStopType, (TUint)RThread().Id()));
	return SendReceive(ESCPSStop, TIpcArgs(aSubConnectionUniqueId, aStopType));
	}


EXPORT_C TInt RConnection::WaitForIncoming(RSubConnection& aIncomingSubConnection)
/**
 * Synchronously wait for an incoming session on this connection.
 *
 * @publishedPartner
 * @released since v9.2
 * @pre Must be attached to a connection by performing a Start().
 * @param aStatus On completion, the status of the request, e.g. KErrNone or another of the system wide error codes.
 * @param aIncomingSubConnection On completion, the subconnection representing the incoming session.
 * The incoming session can be accepted (by calling RSubConnection::Accept) or rejected (by calling RSubConnection::Stop)
 * The subconnection is implicitly started (no need to call RSubConnection::Start). aIncomingSubConnection must not
 * be open prior to calling this method.
 * @return KErrNone if successful, otherwise another of the system wide error codes.
 */
	{
	LOG( ESockLog::Printf(_L8("RConnection %08x:\tWaitForIncoming() sync tid %d"), this, (TUint)RThread().Id()));
	TRequestStatus status;
	WaitForIncoming(aIncomingSubConnection, status);
	User::WaitForRequest(status);
	return status.Int();
	}

EXPORT_C void RConnection::WaitForIncoming(RSubConnection& aIncomingSubConnection, TRequestStatus& aStatus)
/**
 * Subscribe for an incoming session on this connection.
 *
 * @publishedPartner
 * @released since v9.2
 * @pre Must be attached to a connection by performing a Start().
 * @param aStatus On completion, the status of the request, e.g. KErrNone or another of the system wide error codes.
 * @param aIncomingSubConnection On completion, the subconnection representing the incoming session.
 * The incoming session can be accepted (by calling RSubConnection::Accept) or rejected (by calling RSubConnection::Stop)
 * The subconnection is implicitly started (no need to call RSubConnection::Start). aIncomingSubConnection must not
 * be open prior to calling this method.
 */
	{
	LOG( ESockLog::Printf(_L8("RConnection %08x:\tWaitForIncoming() async tid %d"), this, (TUint)RThread().Id()));
	//============================================================================================================
	//TODO: The statement below is really awkward and it is so, because we do not store the handle to the RSocketServ
	//anywhere here. For the same reason we need to call RSubConnection::Open against _both_ RSocketServ and
	//RConnection although RConnection has just been opened against the same RSocketServ.
	//There's nothing except BC that's stopping us from doing the following changes:
	//(1) store RSocketServ in RSubSessionBase so that it can be reused.
	//(2) change APIs like RSubConnection::Open(RSocketServ& aServer, TSubConnType aSubConnType, RConnection& aConnection)
	//                  to RSubConnection::Open(TSubConnType aSubConnType, RConnection& aConnection)
	//============================================================================================================
	const RSessionBase sessionBase = Session();
	TInt error = aIncomingSubConnection.Open(
	    const_cast<RSocketServ&>(static_cast<const RSocketServ&>(sessionBase)), TSubConnOpen::EWaitForIncoming, *this );
	if (error != KErrNone)
    	{
    	TRequestStatus* statusPtr = &aStatus;
    	User::RequestComplete(statusPtr, KErrArgument);
    	return;
    	}
	SendReceive(ECNWaitForIncoming, TIpcArgs(aIncomingSubConnection.SubSessionHandle()), aStatus);
	}

EXPORT_C void RConnection::CancelWaitForIncoming()
/**
 * Cancel the previous subscription for an incoming session.
 *
 * @publishedPartner
 * @released since v9.2
 * @see WaitForIncoming()
 * @pre Must be attached to a connection by performing a Start().
 */
	{
	LOG( ESockLog::Printf(_L8("RConnection %08x:\tCancelWaitForIncoming() tid %d"), this, (TUint)RThread().Id()));
	SendReceive(ECNCancelWaitForIncoming, TIpcArgs());
	}


EXPORT_C void RConnection::ProgressNotification(TNifProgressBuf& aProgress, TRequestStatus& aStatus, TUint aSelectedProgress)
/** Requests asynchronous progress notification for the connection.

@param aProgress A buffer to receive progress notification.
@param aStatus On return, the status of the request.
@param aSelectedProgress The type of progress to report. If set, report only
the particular progress specified and any progress in error. If not set, report
all progress normally. */
	{
	SendReceive(ECNProgressNotification, TIpcArgs(&aProgress,aSelectedProgress ), aStatus);
	}

EXPORT_C void RConnection::ProgressNotification(TSubConnectionUniqueId aSubConnectionUniqueId, TNifProgressBuf& aProgress, TRequestStatus& aStatus, TUint aSelectedProgress)
/** Requests asynchronous progress notification for a subconnection.

NOTE: This function is not supported and will correctly return KErrNotSupported.

@publishedPartner
@released since v7.0s
@pre An attached connection: as a result of performing either a Start() or an Attach()
@pre No outstanding request for progress notifications for this subconnection on
this RConnection
@param aSubConnectionUniqueId A valid identifier for a subconnection.
@param aProgress A buffer to receive progress notification.
@param aStatus On return, the status of the request.
@param aSelectedProgress The type of progress to report. If set, report only
the particular progress specified and any progress in error. If not set, report
all progress normally.
*/
	{
	SendReceive(ESCPSProgressNotification, TIpcArgs(aSubConnectionUniqueId, &aProgress, aSelectedProgress), aStatus);
	}

EXPORT_C void RConnection::CancelProgressNotification()
/** Cancels a request for progress notification for the connection, as issued by
ProgressNotification().
*/
	{
	SendReceive(ECNCancelProgressNotification, TIpcArgs());
	}

EXPORT_C void RConnection::CancelProgressNotification(TSubConnectionUniqueId aSubConnectionUniqueId)
/** Cancels a request for progress notification for a specified subconnection,
as issued by ProgressNotification().

@publishedPartner
@released since v7.0s
@pre An attached connection: as a result of performing either a Start() or an Attach()
@pre An outstanding request for a progress notification on the subconnection on
this RConnection

@param aSubConnectionUniqueId The identifier for the subconnection used to make the request.
*/
	{
	SendReceive(ESCPSCancelProgressNotification, TIpcArgs(aSubConnectionUniqueId));
	}

EXPORT_C TInt RConnection::Progress(TNifProgress& aProgress)
/** Obtains the current progress information for the connection.

@param aProgress A buffer to receive progress information.
@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	TPckg<TNifProgress> prog(aProgress);
	return SendReceive(ECNProgress, TIpcArgs(&prog));
	}

EXPORT_C TInt RConnection::Progress(TSubConnectionUniqueId aSubConnectionUniqueId, TNifProgress& aProgress)
/** Obtains the current progress information on a specified subconnection.

@publishedPartner
@released since v7.0s
@pre An attached connection: as a result of performing either a Start() or an Attach()
@param aSubConnectionUniqueId A valid identifier for the subconnection of
interest.
@param aProgress On return, progress information.
@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	TPckg<TNifProgress> prog(aProgress);
	return SendReceive(ESCPSProgress, TIpcArgs(aSubConnectionUniqueId, &prog));
	}

EXPORT_C TInt RConnection::LastProgressError(TNifProgress& aProgress)
/** Obtains information about the last Progress() call which failed with an error.

@param aProgress A buffer to receive progress information.
@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	TPckg<TNifProgress> prog(aProgress);
	return SendReceive(ECNLastProgressError, TIpcArgs(&prog));
	}

EXPORT_C void RConnection::ServiceChangeNotification(TUint32& aNewISPId, TDes& aNewServiceType, TRequestStatus& aStatus)
/** 
 * Requests service change notification from the agent.
 * This call completes if the underlying service changes (i.e. ISP, GPRS APN
 * or LAN Service).
 * Important: It Panics if used BEFORE "RConnecion::Open(...)" being called.
 * 
 * @param aNewISPId On completion, the new ISP Id.
 * @param aNewServiceType On completion, the new service type.
 * @param aStatus On completion, "KErrNone" if succesful, "KErrNotReady" if
 * 		called before "RConnection::Start(...)" being called, "KErrInUse" if
 * 		called twice. Otherwise another of the system wide error codes. */
	{
	(void) ::new(&iNewISPId) TPckg<TUint32>(aNewISPId);

	SendReceive(ECNServiceChangeNotification, TIpcArgs(&iNewISPId,&aNewServiceType ), aStatus);
	}

EXPORT_C void RConnection::CancelServiceChangeNotification()
/**
 * Cancels a request for notification of change of service for the connection,
 * as issued by ServiceChangeNotification().
 * Nothing happens if called without having 
 * called "RConnection::ServiceChangeNotification(...)" before.
 * Important: It Panics if used BEFORE "RConnecion::Open(...)" being called. */
	{
	SendReceive(ECNCancelServiceChangeNotification,TIpcArgs()); // Ignore return value
	}

EXPORT_C TInt RConnection::GetIntSetting(const TDesC& aSettingName, TUint32& aValue)
//
// Get "active setting", i.e. the current default
// or override, for the specified database field
//
/** Reads current CommDb settings for the active connection.

@pre An attached connection: as a result of performing either a Start() or an Attach()
@param aSettingName The CommDb table name and field name to be accessed. Of
the form "<table name>\<field name>" (for example "IAP\Id" or "ModemBearer\PortName").
@param aValue On return, the value of the table/field pair.
@return KErrNone if succesful, or another of the system-wide error codes.
@capability Dependent on table - deferred to RDBMS

@deprecated Since 9.5 - For replacement functionality see GetParameters
*/
	{
	TPckg<TUint32> intPckg(aValue);
	return SendReceive(ECNGetIntSetting, TIpcArgs(&aSettingName,&intPckg ));
	}

EXPORT_C TInt RConnection::GetBoolSetting(const TDesC& aSettingName, TBool& aValue)
//
// Get "active setting", i.e. the current default
// or override, for the specified database field
//
/** Reads current CommDb settings for the active connection.

@pre An attached connection: as a result of performing either a Start() or an Attach()
@param aSettingName The CommDb table name and field name to be accessed. Of
the form "<table name>\<field name>". (for example "IAP\Id" or "ModemBearer\PortName").
@param aValue On return, the value of the table/field pair.
@return KErrNone if succesful, or another of the system-wide error codes.
@capability Dependent on table - deferred to RDBMS

@deprecated Since 9.5 - For replacement functionality see GetParameters
*/
	{
	TPckg<TBool> boolPckg(aValue);

	return SendReceive(ECNGetBoolSetting, TIpcArgs(&aSettingName,&boolPckg ));
	}

EXPORT_C TInt RConnection::GetDesSetting(const TDesC& aSettingName, TDes8& aValue)
//
// Get "active setting", i.e. the current default
// or override, for the specified database field
//
/** Reads current CommDb settings for the active connection.

@pre An attached connection: as a result of performing either a Start() or an Attach()

@param aSettingName The CommDb table name and field name to be accessed. Of
the form "<table name>\<field name>" (for example "IAP\Id" or "ModemBearer\PortName").
@param aValue On return, the value of the table/field pair.
@return KErrNone if succesful, or another of the system-wide error codes.
@capability Dependent on table - deferred to RDBMS

@deprecated Since 9.5 - For replacement functionality see GetParameters
*/
	{
	return SendReceive(ECNGetDes8Setting, TIpcArgs(&aSettingName,&aValue ));
	}

EXPORT_C TInt RConnection::GetDesSetting(const TDesC& aSettingName, TDes16& aValue)
//
// Get "active setting", i.e. the current default
// or override, for the specified database field
//
/** Reads current CommDb settings for the active connection.

@pre An attached connection: as a result of performing either a Start() or an Attach()

@param aSettingName The CommDb table name and field name to be accessed. Of
the form "<table name>\<field name>" (for example "IAP\Id" or "ModemBearer\PortName").
@param aValue On return, the value of the table/field pair.
@return KErrNone if succesful, or another of the system-wide error codes.
@capability Dependent on table - deferred to RDBMS

@deprecated Since 9.5 - For replacement functionality see GetParameters
*/
	{
	return SendReceive(ECNGetDes16Setting, TIpcArgs(&aSettingName, &aValue));
	}

EXPORT_C TInt RConnection::GetLongDesSetting(const TDesC& aSettingName, TDes& aValue)
//
// Get "active setting", i.e. the current default
// or override, for the specified database field
//
/** Reads current CommDb settings for the active connection.

@pre An attached connection: as a result of performing either a Start() or an Attach()

@param aSettingName The CommDb table name and field name to be accessed. Of
the form "<table name>\<field name>". (for example "IAP\Id" or "ModemBearer\PortName").
@param aValue On return, the value of the table/field pair.
@return KErrNone if succesful, or another of the system-wide error codes.
@capability Dependent on table - deferred to RDBMS

@deprecated Since 9.5 - For replacement functionality see GetParameters
*/
	{
	return SendReceive(ECNGetLongDesSetting, TIpcArgs(&aSettingName,&aValue));
	}



/**
@return KErrNone		- The get parameters call was serviced successfully
		KErrNotReady	- This error will most likely indicate that the
						  connection has not been started						  
		KErrNoMemory	- Failure to allocate the required memory for the retrieved
					      query bundle
		KErrNotFound	- The interface required by the data object was not found
                          in the connection stack
        Other system-wide error codes dependent on specific data object implementations
*/
EXPORT_C TInt RConnection::GetParameters(ESock::CCommsDataObjectBase& aDataObject)
	{
	__ASSERT_DEBUG(aDataObject.iDataObject, User::Panic(KSpecAssert_ESockCSockRCnctn, 1));
	
	if (!aDataObject.iDataObject->IsGetSupported()) 
		{
		return KErrNotSupported;
		}
	
	aDataObject.iDataObject->SetOperationMode(ESock::XCommsDataObject::EOperationGet);
	
	// Serialise the query and send
	HBufC8* buffer = HBufC8::New(aDataObject.Length());
	if (!buffer) 
		{
		return KErrNoMemory;
		}

	TPtr8 ptr = buffer->Des();
	TInt ret = aDataObject.Store(ptr);
	if (ret == KErrNone)
		{
		ret = SendReceive(ECNGetOrSetParameters, TIpcArgs(&ptr));
		}
	delete buffer;
	
	if (ret != KErrNone)
		{
		return ret;
		}
		
	// Retrieve query response
	TInt lengthOrError = SendReceive(ECNGetParametersResponseLength, TIpcArgs());
	if (lengthOrError < KErrNone)
		{
		return lengthOrError;
		}	

	buffer = HBufC8::New(lengthOrError);
	if (!buffer)
		{
		return KErrNoMemory;
		}

	ptr.Set(buffer->Des());
	ret = SendReceive(ECNGetParametersResponse, TIpcArgs(&ptr));
	if (ret == KErrNone)
		{
		TPtrC8 loadPtr(ptr.Ptr(), ptr.Length());
		ret = aDataObject.Load(loadPtr);
		}

	delete buffer;
	return ret;
	}


/**
Speculative method - Not yet supported
Do not call this function, it has ony been introduced for symmetry with RConnection::GetParameters
and will be completed at a later date.
@return KErrNotSupported
*/
EXPORT_C TInt RConnection::SetParameters(ESock::CCommsDataObjectBase& aDataObject)
	{
	__ASSERT_DEBUG(aDataObject.iDataObject, User::Panic(KSpecAssert_ESockCSockRCnctn, 2));
	(void)aDataObject;
	return KErrNotSupported;
	}


EXPORT_C void RConnection::Ioctl(TUint aOptionLevel, TUint aOptionName, TRequestStatus& aStatus, TDes8* aDesc)
/**
  * Get information on the connection in an asynchronous manner
  *
  * @publishedPartner
  * @released
  * @param aOptionLevel Option level to be used.
  * @param aOptionName Option name to be used.
  * @param aOption On return, the option value.
  * @capability Dependent on the type of operation so deferred to PRT.  See documentation
  * of constant values used in aOptionName and aOptionLevel for more information
  *
  */
	{
	SendReceive(ECNIoctl, TIpcArgs(aOptionLevel, aOptionName, aDesc), aStatus);
	}

EXPORT_C void RConnection::Ioctl(TUint aOptionLevel, TUint aOptionName, TRequestStatus& aStatus)
/**
  * Get information on the connection in an asynchronous manner
  *
  * @publishedPartner
  * @released
  * @param aOptionLevel Option level to be used.
  * @param aOptionName Option name to be used.
  * @param aOption On return, the option value.
  * @capability Dependent on the type of operation so deferred to PRT.  See documentation
  * of constant values used in aOptionName and aOptionLevel for more information
  *
  */
	{
	Ioctl( aOptionLevel, aOptionName, aStatus, NULL);
	}

EXPORT_C void RConnection::CancelIoctl()
/**
  * Cancel any outstanding Ioctl request
  *
  * @publishedPartner
  * @released
  */
	{
	SendReceive(ECNCancelIoctl, TIpcArgs());
	}

EXPORT_C TInt RConnection::Control(TUint aOptionLevel, TUint aOptionName, TDes8& aOption)
/** Gets detailed information on connection clients and sockets.

More likely to be used by system control type applications.

@param aOptionLevel Option level to be used.
@param aOptionName Option name to be used.
@param aOption On return, the option value.
@return KErrNone if succesful, or another of the system-wide error codes.
@capability Dependent on the type of operation so deferred to PRT.  See documentation
of constant values used in aOptionName and aOptionLevel for more information */

// Enumeration options - not to be supported in the future:
//
// 1)
// Enumerates Clients of a Connection. RConnection includes the ability to list
// the Thread Id, Process Id and UID of all threads using particular connections.
// The first stage is to perform the following call: Control(KCOLConnection,
// KCoEnumerateConnectionClients, TConnEnumArgBuf&);
//
// The caller specifies the particular connection that they are interested in
// through the iIndex member of the TConnectionEnumArg. The index value is the
// same as that used with GetConnectionInfo(). On completion, the iCount member
// of TConnectionEnumArg contains the number of clients of the connection.
//
// The information about each client can then be retrieved by repeatedly using
// the following call: Control(KCOLConnection, KCoGetConnectionClientInfo, TConnGetClientInfoArgBuf&);
//
// 2)
// Enumerate Sockets on a Connection if required. RConnection includes the ability
// to list all the sockets using a connection. Firstly, the following call is
// performed: Control(KCOLConnection, KCoEnumerateConnectionSockets, TConnEnumArgBuf&);
// This function gathers information about the sockets currently associated with
// a connection. The client specifies the particular connection that they are
// interested in through the iIndex member of the TConnEnumArgBuf.
//
// On completion, the iCount member contains the number of sockets currently
// associated with the connection. The information about each socket can then
// be retrieved by repeatedly calling the following: Control(KCOLConnection,
// KCoGetConnectionSocketInfo, TConnGetSocketInfoArgBuf&);
	{
	return SendReceive(ECNControl, TIpcArgs(aOptionLevel, aOptionName, &aOption));
	}

EXPORT_C TInt RConnection::SetOpt(TUint aOptionLevel, TUint aOptionName, TInt aOption)
/** Sets an option.

@param aOptionLevel Option level to be set.
@param aOptionName Option name to be set
@param aOption Option value.
@return KErrNone if succesful, or another of the system-wide error codes.
@capability Dependent on the type of operation so deferred to PRT.  See documentation
of constant values used in aOptionName and aOptionLevel for more information */
	{
	TPtr8 optionDes((TUint8*)&aOption, sizeof(TInt), sizeof(TInt));
	aOptionName &= ~KConnWriteUserDataBit;
	return Control(aOptionLevel, aOptionName, optionDes);
	}

EXPORT_C TInt RConnection::GetOpt(TUint aOptionLevel, TUint aOptionName, TInt& aOption)
/** Gets an option.

@param aOptionLevel Option level to be queried.
@param aOptionName Option name to be queried.
@param aOption On return, the option value.
@return KErrNone if succesful, or another of the system-wide error codes.
@capability Dependent on the type of operation so deferred to PRT.  See documentation
of constant values used in aOptionName and aOptionLevel for more information */
	{
	TPtr8 optionDes((TUint8*)&aOption, sizeof(TInt), sizeof(TInt));	
	aOptionName &= ~KConnReadUserDataBit;
	return Control(aOptionLevel, aOptionName, optionDes);
	}

EXPORT_C TInt RConnection::Name(TName& aName)
/** Gets the unique name of an RConnection.

Used to create an RConnection which is a clone of an existing RConnection (possibly in
a different process).

@param aName On return, the unique name of the RConnection.
@return KErrNone if succesful, or another of the system-wide error codes. */
	{
	return SendReceive(ECNReference, TIpcArgs(&aName));
	}

TBool RConnection::SameSession(TInt aSessionHandle)
/** Checks that the Session Handle passed is the same as that of this RConnection.

Use to verify the RConnection argument passed to the RSocket and RHostResolver
Open(..., RConnection& aConnection) methods.

@param aSessionHandle The handle which is to be checked against that of this RConnection
@returns ETrue if handle is the same, else EFalse
@internalComponent
*/
	{
	return (Session().Handle() == aSessionHandle);
	}

EXPORT_C TInt RConnection::EnumerateConnections(TUint& aCount)
/** Enumerates the number of currently active interfaces.

@note This does not count the number of RConnections but the number of underlying interfaces.
These may be attached to by varying numbers of RConnections, RSockets etc.
@param aCount On return, contains the number of currently active interfaces on the server.
@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	TPckg<TUint> count(aCount);

	return SendReceive(ECNEnumerateConnections, TIpcArgs(&count));
	}

EXPORT_C TInt RConnection::GetConnectionInfo(TUint aIndex, TDes8& aConnectionInfo)
/** Gets information about one of the currently active connections.

Note that the actual connection information is gathered on a call to EnumerateConnections()
and GetConnectionInfo() is simply used to return the information to the client.
Therefore, if the state of the connections change after the EnumerateConnections()
call, then the information returned by GetConnectionInfo() may be out of date.

@param aIndex The index of the connection - must be between 1 and the value
of aCount returned in EnumerateConnections().
@param aConnectionInfo On return, contains a TPckg<TConnectionInfo> containing
information about the connection.
@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	return SendReceive(ECNGetConnectionInfo, TIpcArgs(aIndex,&aConnectionInfo ));
	}

EXPORT_C void RConnection::AllInterfaceNotification(TDes8& aNotification, TRequestStatus& aStatus)
/** Requests asynchronous change notification for all interfaces.

This allows a client to receive a notification whenever a connection in the
system goes up or down.

This allows the automatic update of the list of active network connections.

@param aNotification On return, a wrapped interface change notification (TInterfaceNotification).
@param aStatus On return, the status of the request. */
	{
	SendReceive(ECNAllInterfaceNotification, TIpcArgs(&aNotification), aStatus);
	}

EXPORT_C void RConnection::CancelAllInterfaceNotification()
/** Cancels a change notification request previously issued by a call to AllInterfaceNotification(). */
	{
	SendReceive(ECNCancelAllInterfaceNotification, TIpcArgs());
	}

EXPORT_C TInt RConnection::Attach(const TDesC8& aConnectionInfo, TConnAttachType aAttachType)
/** Attaches the RConnection object to an existing interface.

This operation will not start an interface, as Start() does, but attaches
to an existing interface if it exists.

@param aConnectionInfo Identity of the connection to attach to. This is a
TPckg<TConnectionInfo>& (possibly obtained from a call to GetConnectionInfo()).
@param aAttachType Identifies the intended use of the connection.
@return KErrNone if succesful, or another of the system-wide error codes.
@capability dependent on the type of connection so deferred to PRT */
	{
	return SendReceive(ECNAttach, TIpcArgs(aAttachType,&aConnectionInfo));
	}


EXPORT_C TInt RConnection::EnumerateSubConnections(TUint& aCount)
/** Asks the server to gather information on the currently active subconnections.

@publishedPartner
@released since v7.0s
@pre An attached connection: as a result of performing either a Start() or an Attach()
@note Unlike EnumerateConnections(), EnumerateSubConnections() does not cache the
information about subconnections, so it is possible for a greater or fewer
number of records to be returned through GetSubConnectionInfo()

@param aCount On return, contains the number of currently active connections.
@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	TPckg<TUint> count(aCount);
	return(SendReceive(ECNEnumerateSubConnections, TIpcArgs(&count)));
	}

EXPORT_C TInt RConnection::GetSubConnectionInfo(TDes8& aSubConnectionInfo)
/** Gets information about one of the currently active subconnections.

This function is used when a valid TSubConnectionUniqueId has been discovered
using the alternative form of GetSubConnectionInfo(), and the client wishes
to update its TSubConnectionInfo-derived structure with the latest subconnection
information.

@publishedPartner
@released since v7.0s
@pre An attached connection: as a result of performing either a Start() or an Attach()

@param aSubConnectionInfo A TPckg<TSubConnectionInfo-derived class>: the actual
class depends upon the type of subconnection, eg. GPRS context, PPP, NCP.
On entry, must contain a valid TSubConnectionUniqueId to indicate which of
the subconnections it wishes to gather information about. On return, contains
information about the subconnection.
@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	return(GetSubConnectionInfo(KUseEmbeddedUniqueId, aSubConnectionInfo));
	}

EXPORT_C TInt RConnection::GetSubConnectionInfo(TUint aIndex, TDes8& aSubConnectionInfo)
/** Gets information about one of the currently active subconnections.

This function is intended for the initial enumeration of connections, when
no TSubConnectionUniqueIds are available, so the client must iterate through
the list of subconnections.

@publishedPartner
@released since v7.0s
@pre An attached connection: as a result of performing either a Start() or an Attach()

@param aIndex The index of the connection - must be between 1 and the value
of aCount returned in EnumerateSubConnections().
@param aSubConnectionInfo On return, contains a TPckg<TSubConnectionInfo-derived
class> containing information about the subconnection; the actual class depends
upon the type of subconnection, eg. GPRS context, PPP NCP.
@return KErrNone if successful, otherwise another of the system wide error
codes. */
	{
	return(SendReceive(ESCPSGetSubConnectionInfo, TIpcArgs(aIndex, &aSubConnectionInfo)));
	}

EXPORT_C void RConnection::AllSubConnectionNotification(TSubConnectionNotificationBuf& aSubConnectionNotification, TRequestStatus& aStatus)
/**
Requests notification of any change in the state of any of the subconnections belonging to this RConnection.

@publishedPartner
@released since v7.0s
@pre An attached connection: as a result of performing either a Start() or an Attach()
@pre No outstanding request for subconnection notifications for this subconnection
on this RConnection

@param &aSubConnectionNotification On return, contains the subconnection event that
occured; this will require casting to the appropriate TSubConnectionEvent-derived
type - the required type can be discovered by casting to a TSubConnectionEvent
and reading the iEventType member.
@param aStatus On return, the status of the request. */
	{
	SendReceive(ECNAllSubConnectionNotification, TIpcArgs(&aSubConnectionNotification), aStatus);
	}

EXPORT_C void RConnection::CancelAllSubConnectionNotification()
/** Cancels the request for notification to changes to the state of subconnections,
made using the AllSubConnectionNotification().

@publishedPartner
@released since v7.0s
@pre An attached connection: as a result of performing either a Start() or an Attach()
@pre An outstanding request for notification about changes to the state of subconnections.
*/
	{
	SendReceive(ECNCancelAllSubConnectionNotification, TIpcArgs());
	}

EXPORT_C void RConnection::DataTransferredRequest(TPckg<TUint>& aUplinkVolume, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus)
/** Gets the amount of data that has been transferred by the entire connection.

@publishedPartner
@released since v7.0s

@pre An attached connection, as a result of performing either a Start() or an Attach()
@param aUplinkVolume On return, contains the amount of data in bytes transferred
by this connection to the remote endpoint.
@param aDownlinkVolume On return, contains the amount of data in bytes transferred
by this connection from the remote endpoint.
@param aStatus On return, the status of the request. */
	{
	DataTransferredRequest(KNifEntireConnectionSubConnectionId, aUplinkVolume, aDownlinkVolume, aStatus);
	}

EXPORT_C void RConnection::DataTransferredRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TPckg<TUint>& aUplinkVolume, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus)
/** Gets the amount of data that has been transferred by the specified subconnection.

@publishedPartner
@deprecated since v9.5. Replaced with RCommsDataMonitoringApiExt::RequestDataTransferred
@pre An attached connection, as a result of performing either a Start() or an Attach()
@param aSubConnectionUniqueId A valid identifier for the subconnection being
queried.
@param aUplinkVolume On return, contains the amount of data in bytes transferred
by this connection to the remote endpoint.
@param aDownlinkVolume On return, contains the amount of data in bytes transferred
by this connection from the remote endpoint.
@param aStatus On return, the status of the request. */
	{
	SendReceive(ESCPSDataTransferred, TIpcArgs(aSubConnectionUniqueId, &aUplinkVolume, &aDownlinkVolume), aStatus);
	}

EXPORT_C void RConnection::DataTransferredCancel()
/** Cancels a request for the amount of data transferred on the entire connection,
issued by DataTransferredRequest().

@publishedPartner
@released since v7.0s
@pre An attached connection; either as a result of performing a Start() or an Attach()
@pre An outstanding request for the amount of data transferred on this connection made using GetDataTransferred()
*/
	{
	DataTransferredCancel(KNifEntireConnectionSubConnectionId);
	}

EXPORT_C void RConnection::DataTransferredCancel(TSubConnectionUniqueId aSubConnectionUniqueId)
/** Cancels a request for the amount of data transferred on a specified subconnection,
issued by DataTransferredRequest().

@publishedPartner
@deprecated since v9.5. Replaced with RCommsDataMonitoringApiExt::CancelDataTransferredRequest
@pre An attached connection; either as a result of performing a Start() or an Attach()
@pre An outstanding request for the amount of data transferred on this connection made using GetDataTransferred()
@param aSubConnectionUniqueId The identifier for the subconnection used to make the request. */
	{
	SendReceive(ESCPSDataTransferredCancel, TIpcArgs(aSubConnectionUniqueId));
	}

EXPORT_C void RConnection::DataSentNotificationRequest(TUint aThreshold, TPckg<TUint>& aUplinkVolume, TRequestStatus& aStatus)
/** Requests a notification after an additional aThreshold bytes have been sent
by the entire connection.

Making this request with an aThreshold of zero will result in the server reading
the value in aUplinkVolume and producing a notification when the absolute
amount of data specified here has been sent.

@publishedPartner
@released since v7.0s
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre No outstanding request for data sent notifications on this RConnection
@param aThreshold The number of additional bytes to be sent by this connection
before the request completes.
@param aUplinkVolume The total number of bytes sent by this connection so far.
@param aStatus On return, the status of the request. */
	{
	DataSentNotificationRequest(KNifEntireConnectionSubConnectionId, aThreshold, aUplinkVolume, aStatus);
	}

EXPORT_C void RConnection::DataSentNotificationRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aThreshold, TPckg<TUint>& aUplinkVolume, TRequestStatus& aStatus)
/** Requests a notification after an additional aThreshold bytes have been sent
on a specified subconnection.

Making this request with an aThreshold of zero will result in the server reading
the value in aUplinkVolume and producing a notification when the absolute
amount of data specified here has been sent.

@publishedPartner
@deprecated since v9.5. Replaced with RCommsDataMonitoringApiExt::RequestDataSentNotification
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre No outstanding request for data sent notifications for this subconnection on this RConnection
@param aSubConnectionUniqueId A valid identifier for the subconnection of
interest.
@param aThreshold The number of additional bytes to be sent by this connection
before the request completes.
@param aUplinkVolume The total number of bytes sent by this connection so far.
@param aStatus On return, the status of the request. */
	{
	SendReceive(ESCPSDataSentNotificationRequest, TIpcArgs(aSubConnectionUniqueId, aThreshold, &aUplinkVolume), aStatus);
	}

EXPORT_C void RConnection::DataSentNotificationCancel()
/** Cancels a request for the amount of data sent on this connection, as issued by
DataSentNotificationRequest().

@publishedPartner
@released since v7.0s
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre An outstanding request for the amount of data transferred on this connection,
made using DataSentNotificationRequest() */
	{
	DataSentNotificationCancel(KNifEntireConnectionSubConnectionId);
	}

EXPORT_C void RConnection::DataSentNotificationCancel(TSubConnectionUniqueId aSubConnectionUniqueId)
/** Cancels a request for the amount of data sent by the specified subconnection.

@publishedPartner
@deprecated since v9.5. Replaced with RCommsDataMonitoringApiExt::CancelDataSentNotificationRequest
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre An outstanding request for the amount of data transferred on this subconnection,
made using DataSentNotificationRequest()
@param aSubConnectionUniqueId The identifier for the subconnection used to
make the request. */
	{
	SendReceive(ESCPSDataSentNotificationCancel, TIpcArgs(aSubConnectionUniqueId));
	}

EXPORT_C void RConnection::DataReceivedNotificationRequest(TUint aThreshold, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus)
/** Requests a notification after an additional aThreshold bytes have been received
by this connection.

Completes when an amount of data equal to aThreshold has been received, starting
from when the request is posted.

Making this request with an aThreshold of zero will result in the server reading
the value in aDownlinkVolume, and producing a notification when the absolute
amount of data specified here has been sent.

@publishedPartner
@released since v7.0s
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre No outstanding request for data received notifications for this connection
on this RConnection
@param aThreshold The number of additional bytes to be received by this connection
before the request completes.
@param aDownlinkVolume The total number of bytes received by this connection so far
@param aStatus On return, the status of the request. */
	{
	DataReceivedNotificationRequest(KNifEntireConnectionSubConnectionId, aThreshold, aDownlinkVolume, aStatus);
	}

EXPORT_C void RConnection::DataReceivedNotificationRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aThreshold, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus)
/** Requests notification after an additional aThreshold bytes have been received
by a specified subconnection.

Completes when an amount of data equal to aThreshold has been received, starting
from when the request is posted.

Making this request with an aThreshold of zero will result in the server reading
the value in aDownlinkVolume, and producing a notification when the absolute
amount of data specified here has been sent.

@publishedPartner
@deprecated since v9.5. Replaced with RCommsDataMonitoringApiExt::RequestDataReceivedNotification
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre No outstanding request for data received notifications for this subconnection on this RConnection
@param aSubConnectionUniqueId A valid identifier for a subconnection.
@param aThreshold The number of additional bytes to be received by this connection
before the request completes.
@param aDownlinkVolume On return, the total number of bytes received by this
connection so far.
@param aStatus On return, the status of the request. */
	{
	SendReceive(ESCPSDataReceivedNotificationRequest, TIpcArgs(aSubConnectionUniqueId, aThreshold, &aDownlinkVolume), aStatus);
	}

EXPORT_C void RConnection::DataReceivedNotificationCancel()
/** Cancels a request (issued by DataReceivedNotificationRequest ()) for the amount
of data received by the entire connection.

@publishedPartner
@released since v7.0s
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre An outstanding request for the amount of data received on this connection made
using DataReceivedNotificationRequest() */
	{
	DataReceivedNotificationCancel(KNifEntireConnectionSubConnectionId);
	}

EXPORT_C void RConnection::DataReceivedNotificationCancel(TSubConnectionUniqueId aSubConnectionUniqueId)
/** Cancels a request (issued by DataReceivedNotificationRequest ()) for the amount
of data received by the specified subconnection.


@publishedPartner
@deprecated since v9.5. Replaced with RCommsDataMonitoringApiExt::CancelDataReceivedNotificationRequest
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre An outstanding request for the amount of data received on this subconnection
made using DataReceivedNotificationRequest()

@param aSubConnectionUniqueId The identifier for the subconnection used to
make the request. */
	{
	SendReceive(ESCPSDataReceivedNotificationCancel, TIpcArgs(aSubConnectionUniqueId));
	}

EXPORT_C void RConnection::IsConnectionActiveRequest(TUint aSecs, TPckg<TBool>& aConnectionActive, TRequestStatus& aStatus)
/**
Checks whether the connection is active or inactive.

Notes whether any data is passed for aSecs after the time of call before responding.
If data is passed the connection is considered active; otherwise it is considered
inactive.

@publishedPartner
@released since v7.0s
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre No outstanding request for activity notifications for this connection on this RConnection
@see IsSubConnectionActiveRequest() for details of when this method will return.
@param aSecs The period for checking whether the connection is active.
The maximum period allowed is approximately 35 mins.

@param aState On entry, the state the client believes the subconnection is
currently in; on return, contains the current state of the subconnection.
@param aStatus On return, the status of the request */
	{
	IsSubConnectionActiveRequest(KNifEntireConnectionSubConnectionId, aSecs, aConnectionActive, aStatus);
	}

EXPORT_C void RConnection::IsConnectionActiveCancel()
/**
Cancels a request for activity notifications on the connection
@publishedPartner
@released since v7.0s
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre An outstanding request for activity notification on this connection made using IsConnectionActiveRequest()
*/
	{
	IsSubConnectionActiveCancel(KNifEntireConnectionSubConnectionId);
	}

EXPORT_C void RConnection::IsSubConnectionActiveRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aSecs, TPckg<TBool>& aSubconnectionActive, TRequestStatus& aStatus)
/**
Checks whether the subconnection is active or has changed state (active/inactive)

@publishedPartner
@released since v7.0s
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre No outstanding request for activity notifications for this subconnection on this RConnection
@note The request will be completed when the aState passed in by the client is not the same as the current state,
but this transition can only occur in quantised periods of aSecs
@note The maximum period allowed is approximately 35 mins
@param aSubConnectionUniqueId A valid identifier for a subconnection
@param aSecs The quantisation period for checking whether the subconnection is active
@param aState The state the client believes the subconnection is currently in; on return, contains the current
state of the subconnection
@param aStatus On return, the status of the request
*/
	{
	SendReceive(ESCPSIsSubConnectionActiveRequest, TIpcArgs(aSubConnectionUniqueId, aSecs, &aSubconnectionActive), aStatus);
	}

EXPORT_C void RConnection::IsSubConnectionActiveCancel(TSubConnectionUniqueId aSubConnectionUniqueId)
/**
Cancels a request for activity notifications on the specified subconnection
@publishedPartner
@released since v7.0s
@pre An attached connection, as a result of performing either a Start() or an Attach()
@pre An outstanding request for an activity notification on this subconnection made using
IsSubConnectionActiveRequest()
@param aSubConnectionUniqueId The identifier for the subconnection used to make the request
*/
	{
	SendReceive(ESCPSIsSubConnectionActiveCancel, TIpcArgs(aSubConnectionUniqueId));
	}


