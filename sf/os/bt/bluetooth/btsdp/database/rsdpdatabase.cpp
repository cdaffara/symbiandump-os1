// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include <btsdp.h>
#include "ipcinternals.h"
#include "DataEncoder.h"
#include "EncoderVisitor.h"
#include <e32math.h>
#include <utf.h>
#include <es_sock.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDPDATABASE);
#endif

static TInt StartServer()
//
// Start the server process or thread
//
	{
	LOG_STATIC_FUNC

	const TUidType serverUid(KNullUid,KNullUid,KSdpServerUid3);
	
#ifdef __SDPSERVER_NO_PROCESSES__
	//
	// In EKA1 WINS the server is a DLL, the exported entrypoint returns a TInt
	// which represents the real entry-point for the server thread
	//
	RLibrary lib;
	TInt r=lib.Load(KSdpServerImg,serverUid);
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
	TName name(KSdpServerName);
	_LIT(KHexString, "0x");
 	name.Append(KHexString());
	name.AppendNum(Math::Random(),EHex);
	RThread server;
	r=server.Create(name,serverFunc,
					KSdpServerStackSize,
					NULL,&lib,NULL,
					KSdpServerInitHeapSize,KSdpServerMaxHeapSize,EOwnerProcess);
	lib.Close();	// if successful, server thread has handle to library now
#else
	//
	// EPOC and EKA2 is easy, we just create a new server process. Simultaneous
	// launching of two such processes should be detected when the second one
	// attempts to create the server object, failing with KErrAlreadyExists.
	//
	RProcess server;
	TInt r=server.Create(KSdpServerImg,KNullDesC,serverUid);
#endif

	LOG(KSdpServerImg);
	LOG1(_L("c\tSdp Server created, code %d"), r);
	if (r!=KErrNone)
		return r;

	TRequestStatus status;
	// this is a variant of RProcess::Logon which completes either when the process 
	// terminates or when the new process calls "RProcess::Rendezvous();"
    server.Rendezvous(status);        
    if (status!=KRequestPending)
		{
		server.Kill(0);                // abort startup 
		}
    else 
		{
		server.Resume();        // Rendezvous OK - start the server 
		LOG(_L("c\tSdp Server Resumed"));
		}
    User::WaitForRequest(status);                // wait for start or death 
	LOG1(_L("c\tSdp Server started, code %d (0=>success)"), status.Int());
    server.Close(); 
	LOG(_L("c\tSdp Server Closed"));
    return status.Int(); 
	}
	

//==================================
// RSdp
//==================================

EXPORT_C RSdp::RSdp()
/** Default constructor. */
	{
	LOG_FUNC
	}

EXPORT_C TInt RSdp::Connect()
/** Connects a client process to the database.

The connection should be closed after use with RHandleBase::Close().

@return System-wide error code */
	{
	LOG_FUNC
	TInt retry=2;
	for (;;)
		{
		TInt r=CreateSession(KSdpServerName,TVersion(0,0,0));	//gives MessageSlots of -1
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

EXPORT_C TVersion RSdp::Version() const
/** Gets the version of the database server.

@return Version information */
	{
	LOG_FUNC
	return TVersion(KSdpServerMajorVersionNumber, 
					KSdpServerMinorVersionNumber, 
					KSdpServerBuildVersionNumber);
	}

EXPORT_C void RSdp::ResourceCountMarkStart()
/** Starts the counter to keep track of open subsessions. 

This should be used by clients to do resource leakage debugging checks. */
	{
	LOG_FUNC
	SendReceive(ESdpResourceCountMarkStart, TIpcArgs(NULL)); //ss//
	}

EXPORT_C void RSdp::ResourceCountMarkEnd()
/** Stops the counter to keep track of open subsessions.

This should be used by clients to do resource leakage debugging checks. */
	{
	LOG_FUNC
	SendReceive(ESdpResourceCountMarkEnd, TIpcArgs(NULL)); //ss//
	}

EXPORT_C TInt RSdp::ResourceCount()
/** Gets the number of open subsessions to the database.

This should be used by clients to do resource leakage debugging checks.

@return Number of open subsessions to the database */
	{
	LOG_FUNC
	TInt count = 0;
	TPckgBuf<TInt> pckg(count);
	SendReceive(ESdpResourceCount, TIpcArgs(&pckg)); //ss//
	return pckg();
	}

/**
@internalTechnology
@released
*/
EXPORT_C TInt RSdp::__DbgMarkHeap()
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ESdpServerDbgMarkHeap);
#else
	return KErrNone;
#endif
	}

/**
@internalTechnology
@released
*/
EXPORT_C TInt RSdp::__DbgCheckHeap(TInt aCount)
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ESdpServerDbgCheckHeap, TIpcArgs(aCount));
#else
	(void)aCount;
	return KErrNone;
#endif
	}

/**
@internalTechnology
@released
*/
EXPORT_C TInt RSdp::__DbgMarkEnd(TInt aCount)
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ESdpServerDbgMarkEnd, TIpcArgs(aCount));
#else
	(void)aCount;
	return KErrNone;
#endif
	}

/**
@internalTechnology
@released
*/
EXPORT_C TInt RSdp::__DbgFailNext(TInt aCount)
	{
	LOG_FUNC
#ifdef _DEBUG
	return SendReceive(ESdpServerDbgFailNext, TIpcArgs(aCount));
#else
	(void)aCount;
	return KErrNone;
#endif
	}

//==================================
// RSdpDatabase
//==================================

EXPORT_C TInt RSdpDatabase::Open(RSdp& aSession)
/** Opens a SDP database subsession.

@param aSession SDP database session
@return System-wide error code
@capability LocalServices */
	{
	LOG_FUNC
	return CreateSubSession(aSession, ESdpCreateDatabaseSubSession/*, TIpcArgs(&p[0])*/); //ss//
	}

EXPORT_C RSdpDatabase::RSdpDatabase() :
	iBuffer(0)
/** Default constructor.

@capability LocalServices */
	{
	LOG_FUNC
	}

EXPORT_C void RSdpDatabase::Close()
/** Closes the SDP database subsession.

Any records that have been added during this session will be removed when the session is closed.
@capability LocalServices */
	{
	LOG_FUNC
	//delete resources
	delete iBuffer;
	iBuffer = NULL;

	RSubSessionBase::CloseSubSession(ESdpCloseSubSession);
	}


EXPORT_C void RSdpDatabase::CreateServiceRecordL(const TUUID& aUUID, TSdpServRecordHandle& aHandle)
/** Creates a new service record, with a single service class, in the SDP database. 


@param aUUID The service class UUID for the service record
@param aHandle On return, the service record handle of the new record
@capability LocalServices */
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		CSdpAttrValueDES* list = CSdpAttrValueDES::NewDESL(NULL);
		CleanupStack::PushL(list);
		MSdpElementBuilder* bldr = list;

		bldr
			->StartListL()
				->BuildUUIDL(aUUID)
			->EndListL();
		CreateServiceRecordL(*list, aHandle);
		CleanupStack::PopAndDestroy(); //list
		}
	else
		{
		//Client has a bad handle therefore panic
		User::Panic(_L("SDP-Subsession"), KErrBadHandle);
		}
	}

EXPORT_C void RSdpDatabase::CreateServiceRecordL(CSdpAttrValueDES& aUUIDList,
												 TSdpServRecordHandle& aHandle)
/** Creates a new service record, with multiple service classes, in the SDP database.

@param aUUIDList The service class attribute of the new record. This should 
consist of a list of UUIDs of the service classes to which the record belongs. 
The UUIDs should be ordered from the most derived service class to the base 
one.
@param aHandle On return, the service record handle of the new record
@capability LocalServices */
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		delete iBuffer;
		iBuffer = 0;
		TUint size = TElementEncoder::EncodedSize(ETypeDES, aUUIDList.DataSize());
		iBuffer = HBufC8::NewL(size);
		TPtr8 ptr = iBuffer->Des();

		TElementEncoder ee(ptr);
		CAttrEncoderVisitor::EncodeAttributeL(ee, aUUIDList);

		TSdpServRecordHandlePckgBuf pckg;
		User::LeaveIfError(SendReceive(ESdpDatabaseCreateServiceRecord, TIpcArgs(iBuffer, &pckg))); //ss//
		aHandle = pckg(); //return handle in handle reference
		}
	else
		{
		//Client has a bad handle therefore panic
		User::Panic(_L("SDP-Subsession"), KErrBadHandle);
		}
	}


EXPORT_C void RSdpDatabase::UpdateAttributeL(TSdpServRecordHandle aHandle, 
											 TSdpAttributeID aAttrID, 
											 CSdpAttrValue& aAttrValue)
/** Updates a service record attribute (value encapsulated in a CSdpAttrValue).

If the attribute does not exist, it is created. If it already exists, the 
current definition is removed and the new value inserted. 

No check is made that the attribute value is suitable for the service class 
to which the record belongs. It is up to the user to ensure that the semantics 
of the attribute defined by the service class are respected.

@param aHandle The service record handle to update
@param aAttrID The attribute ID to update
@param aAttrValue The attribute value
@capability LocalServices */
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		TSdpAttributeIDPckgBuf idPckg;
		TSdpServRecordHandlePckgBuf handlePckg;
		handlePckg() = aHandle; //put record handle into package
		idPckg() = aAttrID; //put attr ID into package

		delete iBuffer;
		iBuffer = 0;
		TUint size = TElementEncoder::EncodedSize(aAttrValue.Type(), aAttrValue.DataSize());
		iBuffer = HBufC8::NewL(size);
		TPtr8 ptr = iBuffer->Des();

		TElementEncoder ee(ptr);
		CAttrEncoderVisitor::EncodeAttributeL(ee, aAttrValue);
		
		User::LeaveIfError(SendReceive(ESdpDatabaseUpdateAttribute, TIpcArgs(&handlePckg, &idPckg, iBuffer))); //ss//
		}
	else
		{
		//Client has a bad handle therefore panic
		User::Panic(_L("SDP-Subsession"), KErrBadHandle);
		}
	}

EXPORT_C void RSdpDatabase::UpdateAttributeL(TSdpServRecordHandle aHandle, 
											 TSdpAttributeID aAttrID, 
											 TUint aUintValue)
/** Updates a service record attribute (integer value).

If the attribute does not exist, it is created. If it already exists, the 
current definition is removed and the new value inserted. 

No check is made that the attribute value is suitable for the service class 
to which the record belongs. It is up to the user to ensure that the semantics 
of the attribute defined by the service class are respected.

@param aHandle The service record handle to update
@param aAttrID The attribute ID to update
@param aUintValue The attribute value
@capability LocalServices */
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		TBuf8<4> buf;
		buf.SetLength(4);
		BigEndian::Put32(&buf[0], aUintValue);
		CSdpAttrValueUint* attrValueUint = CSdpAttrValueUint::NewUintL(buf);
		CleanupStack::PushL(attrValueUint);
		UpdateAttributeL(aHandle, aAttrID, *attrValueUint);
		CleanupStack::PopAndDestroy(); //attrValueUint
		}
	else
		{
		//Client has a bad handle therefore panic
		User::Panic(_L("SDP-Subsession"), KErrBadHandle);
		}
	}

EXPORT_C void RSdpDatabase::UpdateAttributeL(TSdpServRecordHandle aHandle, 
											 TSdpAttributeID aAttrID, 
											 const TDesC16& aDesCValue)
/** Updates a service record attribute (wide descriptor value).

If the attribute does not exist, it is created. If it already exists, the 
current definition is removed and the new value inserted. 

No check is made that the attribute value is suitable for the service class 
to which the record belongs. It is up to the user to ensure that the semantics 
of the attribute defined by the service class are respected.

@param aHandle The service record handle to update
@param aAttrID The attribute ID to update
@param aDesCValue The attribute value
@capability LocalServices */
/** Updates a service record attribute (narrow descriptor value).

If the attribute does not exist, it is created. If it already exists, the 
current value is removed and the new value inserted. 

No check is made that the attribute value is suitable for the service class 
to which the record belongs. It is up to the user to ensure that the semantics 
of the attribute defined by the service class are respected.

@param aHandle The service record handle to update
@param aAttrID The attribute ID to update
@param aDesCValue The attribute value
@capability LocalServices */
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		 //The '5' below is max ratio of number of UTF_8 chars to number of unicode chars .
		HBufC8* buf = HBufC8::NewLC(5*aDesCValue.Length());
		TPtr8 ptr = buf->Des();
		CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr, aDesCValue);
		UpdateAttributeL(aHandle, aAttrID, ptr);
		CleanupStack::PopAndDestroy(); //buf
		}
	else
		{
		//Client has a bad handle therefore panic
		User::Panic(_L("SDP-Subsession"), KErrBadHandle);
		}
	}

EXPORT_C void RSdpDatabase::UpdateAttributeL(TSdpServRecordHandle aHandle, 
											 TSdpAttributeID aAttrID, 
											 const TDesC8& aDesCValue)
/** Updates a service record attribute (wide descriptor value).

If the attribute does not exist, it is created. If it already exists, the 
current value is removed and the new value inserted. 

No check is made that the attribute value is suitable for the service class 
to which the record belongs. It is up to the user to ensure that the semantics 
of the attribute defined by the service class are respected.

@param aHandle The service record handle to update
@param aAttrID The attribute ID to update
@param aDesCValue The attribute value
@capability LocalServices */
/** Updates a service record attribute (narrow descriptor value).

If the attribute does not exist, it is created. If it already exists, the 
current definition is removed and the new value inserted. 

No check is made that the attribute value is suitable for the service class 
to which the record belongs. It is up to the user to ensure that the semantics 
of the attribute defined by the service class are respected.

@param aHandle The service record handle to update
@param aAttrID The attribute ID to update
@param aDesCValue The attribute value
@capability LocalServices */
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		CSdpAttrValueString* attrValueString = CSdpAttrValueString::NewStringL(aDesCValue);
		CleanupStack::PushL(attrValueString);
		UpdateAttributeL(aHandle, aAttrID, *attrValueString);
		CleanupStack::PopAndDestroy(); //attrValueString
		}
	else
		{
		//Client has a bad handle therefore panic
		User::Panic(_L("SDP-Subsession"), KErrBadHandle);
		}
	}

/** Delete an attribute from a service record. 

@param aHandle The service record from which to delete the attribute
@param aAttrID The ID of the attribute
@capability LocalServices
@deprecated Use non-leaving version instead
@see RSdpDatabase::DeleteAttribute
*/
EXPORT_C void RSdpDatabase::DeleteAttributeL(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID)
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		TPckg<TSdpAttributeID> idPckg(aAttrID);
		TPckg<TSdpServRecordHandle> handlePckg(aHandle);
		User::LeaveIfError(SendReceive(ESdpDatabaseDeleteAttribute, TIpcArgs(&handlePckg, &idPckg))); 
		}
	else
		{
		//Client has a bad handle therefore panic
		User::Panic(_L("SDP-Subsession"), KErrBadHandle);
		}
	}

/** Delete an attribute from a service record. 

@param aHandle The service record from which to delete the attribute
@param aAttrID The ID of the attribute
@capability LocalServices 
*/
EXPORT_C void RSdpDatabase::DeleteAttribute(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID)
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		TPckg<TSdpAttributeID> idPckg(aAttrID);
		TPckg<TSdpServRecordHandle> handlePckg(aHandle);

		SendReceive(ESdpDatabaseDeleteAttribute, TIpcArgs(&handlePckg, &idPckg));
		}
	else
		{
		//Client has a bad handle therefore panic
		User::Panic(_L("SDP-Subsession"), KErrBadHandle);
		}
	}

/** Deletes a service record.

@param aHandle Handle of the service record to delete
@capability LocalServices 
@deprecated Use non-leaving version instead
@see RSdpDatabase::DeleteRecord
*/
EXPORT_C void RSdpDatabase::DeleteRecordL(TSdpServRecordHandle aHandle)
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		TPckg<TSdpServRecordHandle> handlePckg(aHandle);

		User::LeaveIfError(SendReceive(ESdpDatabaseDeleteServiceRecord, TIpcArgs(&handlePckg))); 
		}
	else
		{
		//Client has a bad handle therefore panic
		User::Panic(_L("SDP-Subsession"), KErrBadHandle);
		}
	}

/** Deletes a service record.

@param aHandle Handle of the service record to delete
@capability LocalServices 
*/
EXPORT_C void RSdpDatabase::DeleteRecord(TSdpServRecordHandle aHandle)
	{
	LOG_FUNC
	if (SubSessionHandle())
		{
		TPckg<TSdpServRecordHandle> handlePckg(aHandle);

		SendReceive(ESdpDatabaseDeleteServiceRecord, TIpcArgs(&handlePckg));
		}
	else
		{
		//Client has a bad handle therefore panic
		User::Panic(_L("SDP-Subsession"), KErrBadHandle);
		}
	}

