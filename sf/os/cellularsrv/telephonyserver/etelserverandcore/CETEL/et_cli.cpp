// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
*/

#include <f32file.h>
#include "ETELEXT.H"
#include "ETEL.H"
#include <et_clsvr.h>
#include "ET_STRUC.H"
#include "ETELPTR.H"
#include <rsshared.h>
#include <e32property.h>
#include <c32root.h>

#include <etelutils.h>
#include <et_struct.h>

#define NUM_FAXPTR		1
#define NUM_FAXPTRC		1
#define NUM_CALLPTR		6
#define NUM_CALLPTRC	3
#define NUM_LINEPTR		6
#define NUM_PHONEPTR	3

EXPORT_C void PanicClient(TInt aFault)
/**
Panics the client on client side.

@internalComponent
*/
	{
	_LIT(KETelClientFault,"Etel Client Fault");
	User::Panic(KETelClientFault,aFault);
	}

//
// RTelSubSessionBase
//
EXPORT_C RTelSubSessionBase::RTelSubSessionBase()
	:iTelSession(NULL)
	,iPtrHolder(NULL)
	{}

EXPORT_C TInt RTelSubSessionBase::CancelReq(const TInt aIpc,const TInt aIpcToCancel) const

//
//	Set up a cancel request before pass to server
//
/**
Sends a request to the ETel server/ TSY, to cancel a request.

@param aIpc The request, which may or may not be running, that should
            be cancelled.
@param aIpcToCancel Cancel IPC for the aIpc request.  This IPC is sent
                    to the server side as the cancel request.
@capability None
*/
	{
	__ASSERT_ALWAYS(aIpc!=aIpcToCancel,PanicClient(KErrArgument));
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));

	// The err variable should help in debugging, for example:
	//
	// 1) If an IPC does not exist (i.e. it has not been assigned a
	//    security policy) the server will return an error (for ETel
	//    IPC ranges KErrNotSupported is returned) and the cancel
	//    request will have no effect.
	//
	// 2) TSYs can decide to not accept an IPC, and thus, will return a
	//    chosen error code.
	//
	// 3) The client has not been assigned the capabilities it needs to
	//    use the cancel IPC (aIpcToCancel), thus, KErrPermissionDenied.
	//
	// For such cases, the err variable can help identify if the server
	// and client are behaving correctly.
	TInt err = SendReceive(aIpc,TIpcArgs(aIpcToCancel,EIsaCancelMessage));
	// TODO add logging code to display err as a Warning.
	return err;
	}

EXPORT_C TInt RTelSubSessionBase::CancelSubSession() const
//
//	Set up a cancel request before pass to server
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	return SendReceive(EEtelCancelSubSession,TIpcArgs(TIpcArgs::ENothing, EIsaCancelSubSession));
	}

EXPORT_C void RTelSubSessionBase::CancelAsyncRequest(TInt aReqToCancel) const
/**
Cancels a request of the specified IPC number.

@param aReqToCancel The IPC of a request that should be cancelled.  If
                    a request of this IPC is not currently outstanding
                    the TSY should decide what to do, but this usually
                    has no effect.
@capability None
*/
	{
	TInt cancelIpc;
	cancelIpc = aReqToCancel + EMobileCancelOffset;
	CancelReq(cancelIpc, aReqToCancel);
	}

EXPORT_C TInt RTelSubSessionBase::Blank(const TInt aIpc,TReqPriorityType aType) const
//
//	Set up a Null SYNC request before pass to server
//	There is no info passing between client/server
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set (0,TIpcArgs::ENothing);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaNull | KEmergencyClientReq));
	else
		args.Set(1,EIsaNull);
	return SendReceive(aIpc, args);
	}

EXPORT_C void RTelSubSessionBase::Blank(const TInt aIpc,TRequestStatus& aStatus,TReqPriorityType aType) const
//
//	Set up a Null ASYNC request before pass to server
//	There is no info passing between client/server
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,TIpcArgs::ENothing);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaNull | KEmergencyClientReq));
	else
		args.Set(1,(EIsaNull));
	SendReceive(aIpc,args,aStatus);
	}


EXPORT_C TInt RTelSubSessionBase::Set(const TInt aIpc,const TDesC8& aDes,TReqPriorityType aType) const
//
//	Set up a SYNCH. request which the client provide a Des in p[0] for server to Set
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set (0, CONST_CAST(TDesC8*,&aDes));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaDesTobeSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaDesTobeSet);
	return SendReceive(aIpc, args);
	}

EXPORT_C void RTelSubSessionBase::Set(const TInt aIpc,TRequestStatus& aStatus,const TDesC8& aDes,TReqPriorityType aType) const
//
//	Set up a synch request which the client provide a Des in p[0] for server to Set
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC8*,&aDes));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaDesTobeSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaDesTobeSet);
	SendReceive(aIpc,args,aStatus);
	
	//
	// Flush code...
	//
	SendReceive(EEtelFlushInterfaceQueue, TIpcArgs());
	}

EXPORT_C TInt RTelSubSessionBase::Get(const TInt aIpc,TDes8& aDes,TReqPriorityType aType) const
//
//	Set up an SYNC request which the client provide a Des in p[0] for server to Set
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,&aDes);
	
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaDesTobeRead | KEmergencyClientReq));
	else
		args.Set(1,EIsaDesTobeRead);
	
	return SendReceive(aIpc,args);
	}

EXPORT_C void RTelSubSessionBase::Get(const TInt aIpc,TRequestStatus& aStatus,TDes8& aDes,TReqPriorityType aType) const
//
//	Set up an async request which the client provide a Des in p[0]
//	Server will write back data to client address space in aPtr
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,&aDes);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaDesTobeRead | KEmergencyClientReq));
	else
		args.Set(1,EIsaDesTobeRead);
	SendReceive(aIpc,args,aStatus);
	}

EXPORT_C void RTelSubSessionBase::Get(const TInt aIpc,TRequestStatus& aStatus,
								  TDes8& aDes1,TDes8& aDes2,TReqPriorityType aType) const
//
//	Set up a asynch request - server write data back into descriptors P0 and P2
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,&aDes1);

	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,EIsaDoubleDesTobeRead | KEmergencyClientReq);
	else
		args.Set(1,EIsaDoubleDesTobeRead);
	args.Set(2,&aDes2);
	SendReceive(aIpc,args,aStatus);
	}

EXPORT_C TInt RTelSubSessionBase::Get(const TInt aIpc,TDes8& aDes1,TDes8& aDes2,TReqPriorityType aType) const
//
//	Set up a synch request - server write data back into descriptors P0 and P2
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,&aDes1);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaDoubleDesTobeRead | KEmergencyClientReq));
	else
		args.Set(1,EIsaDoubleDesTobeRead);
	args.Set(2,&aDes2);
	return SendReceive(aIpc,args);
	}

EXPORT_C TInt RTelSubSessionBase::Set(const TInt aIpc,const TDesC8& aDes1,const TDesC8& aDes2,TReqPriorityType aType) const
//
//	Set up a SYNCH. request which the client provide a Des in p[0] for server to Set
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC8*,&aDes1));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaDoubleDesTobeSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaDoubleDesTobeSet);
	args.Set(2,CONST_CAST(TDesC8*,&aDes2));
	return SendReceive(aIpc,args);
	}

EXPORT_C void RTelSubSessionBase::Set(const TInt aIpc,TRequestStatus& aStatus,const TDesC8& aDes1,
									const TDesC8& aDes2,TReqPriorityType aType) const
//
//	Set up a synch request which the client provide a Des in p[0] for server to Set
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC8*,&aDes1));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaDoubleDesTobeSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaDoubleDesTobeSet);
	args.Set(2,CONST_CAST(TDesC8*,&aDes2));
	SendReceive(aIpc,args,aStatus);
		
	//
	// Flush code...
	//
	SendReceive(EEtelFlushInterfaceQueue, TIpcArgs());
	}

EXPORT_C TInt RTelSubSessionBase::Set(const TInt aIpc,const TDesC16& aDes,TReqPriorityType aType) const
//
//	Set up a SYNCH. request which the client provide a Des in p[0] for server to Set
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC16*,&aDes));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaUnicodeDesTobeSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaUnicodeDesTobeSet);
	return SendReceive(aIpc,args);
	}

EXPORT_C void RTelSubSessionBase::Set(const TInt aIpc,TRequestStatus& aStatus,const TDesC16& aDes,TReqPriorityType aType) const
//
//	Set up a synch request which the client provide a Des in p[0] for server to Set
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC16*,&aDes));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaUnicodeDesTobeSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaUnicodeDesTobeSet);
	SendReceive(aIpc,args,aStatus);
	
	//
	// Flush code...
	//
	SendReceive(EEtelFlushInterfaceQueue, TIpcArgs());
	}

EXPORT_C TInt RTelSubSessionBase::Get(const TInt aIpc,TDes16& aDes,TReqPriorityType aType) const
//
//	Set up an SYNC request which the client provide a Des in p[0] for server to Set
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0, &aDes);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaUnicodeDesTobeRead | KEmergencyClientReq));
	else
		args.Set(1,EIsaUnicodeDesTobeRead);
	return SendReceive(aIpc,args);
	}

EXPORT_C void RTelSubSessionBase::Get(const TInt aIpc,TRequestStatus& aStatus,TDes16& aDes,TReqPriorityType aType) const
//
//	Set up an async request which the client provide a Des in p[0]
//	Server will write back data to client address space in aPtr
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,&aDes);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaUnicodeDesTobeRead | KEmergencyClientReq));
	else
		args.Set(1,EIsaUnicodeDesTobeRead);
	SendReceive(aIpc,args,aStatus);
	}

EXPORT_C void RTelSubSessionBase::Get(const TInt aIpc,TRequestStatus& aStatus,
								  TDes16& aDes1,TDes16& aDes2,TReqPriorityType aType) const
//
//	Set up a asynch request - server write data back into descriptors P0 and P2
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,&aDes1);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaUnicodeDoubleDesTobeRead | KEmergencyClientReq));
	else
		args.Set(1,EIsaUnicodeDoubleDesTobeRead);
	args.Set(2,&aDes2);
	SendReceive(aIpc,args,aStatus);
	}

EXPORT_C TInt RTelSubSessionBase::Get(const TInt aIpc,TDes16& aDes1,TDes16& aDes2,TReqPriorityType aType) const
//
//	Set up a synch request - server write data back into descriptors P0 and P2
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,&aDes1);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaUnicodeDoubleDesTobeRead | KEmergencyClientReq));
	else
		args.Set(1,EIsaUnicodeDoubleDesTobeRead);
	args.Set(2,&aDes2);
	return SendReceive(aIpc,args);
	}
	 
EXPORT_C void RTelSubSessionBase::Get(const TInt aIpc,TRequestStatus& aStatus,
								  TDes8& aDes1,TDes16& aDes2,TReqPriorityType aType) const
//
//	Set up a asynch request - server write data back into descriptors P0 and P2
//	P0 is a narrow descriptor, P2 is a unicode descriptor
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,&aDes1);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaNarrowAndUnicodeDoubleDesTobeRead | KEmergencyClientReq));
	else
		args.Set(1,EIsaNarrowAndUnicodeDoubleDesTobeRead);
	args.Set(2,&aDes2);
	SendReceive(aIpc,args,aStatus);
	}

EXPORT_C TInt RTelSubSessionBase::Get(const TInt aIpc,TDes8& aDes1,TDes16& aDes2,TReqPriorityType aType) const
//
//	Set up a synch request - server write data back into descriptors P0 and P2
//	P0 is a narrow descriptor, P2 is a unicode descriptor
//
/**
@capability None
*/
	{
	TIpcArgs args;
	args.Set(0,&aDes1);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaNarrowAndUnicodeDoubleDesTobeRead | KEmergencyClientReq));
	else
		args.Set(1,EIsaNarrowAndUnicodeDoubleDesTobeRead);
	args.Set(2,&aDes2);
	return SendReceive(aIpc,args);
	}

EXPORT_C TInt RTelSubSessionBase::Set(const TInt aIpc,const TDesC16& aDes1,const TDesC16& aDes2,TReqPriorityType aType) const
//
//	Set up a SYNCH. request which the client provide a Des in p[0] for server to Set
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC16*,&aDes1));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaUnicodeDoubleDesTobeSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaUnicodeDoubleDesTobeSet);
	args.Set(2,CONST_CAST(TDesC16*,&aDes2));
	return SendReceive(aIpc,args);
	}

EXPORT_C void RTelSubSessionBase::Set(const TInt aIpc,TRequestStatus& aStatus,const TDesC16& aDes1,
									const TDesC16& aDes2,TReqPriorityType aType) const
//
//	Set up a synch request which the client provide a Des in p[0] for server to Set
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC16*,&aDes1));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaUnicodeDoubleDesTobeSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaUnicodeDoubleDesTobeSet);
	args.Set(2,CONST_CAST(TDesC16*,&aDes2));
	SendReceive(aIpc,args,aStatus);
	
	//
	// Flush code...
	//
	SendReceive(EEtelFlushInterfaceQueue, TIpcArgs());
	}

EXPORT_C TInt RTelSubSessionBase::Set(const TInt aIpc,const TDesC8& aDes1,const TDesC16& aDes2,TReqPriorityType aType) const
//
//	Set up a SYNCH. request which the client provide a Des in p[0] for server to Set
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC8*,&aDes1));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaNarrowAndUnicodeDoubleDesTobeSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaNarrowAndUnicodeDoubleDesTobeSet);
	args.Set(2,CONST_CAST(TDesC16*,&aDes2));
	return SendReceive(aIpc,args);
	}

EXPORT_C void RTelSubSessionBase::Set(const TInt aIpc,TRequestStatus& aStatus,const TDesC8& aDes1,
									const TDesC16& aDes2,TReqPriorityType aType) const
//
//	Set up a synch request which the client provide a Des in p[0] for server to Set and a Des in p[1] to Get
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC8*,&aDes1));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaNarrowAndUnicodeDoubleDesTobeSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaNarrowAndUnicodeDoubleDesTobeSet);
	args.Set(2,CONST_CAST(TDesC16*,&aDes2));
	SendReceive(aIpc,args,aStatus);
		
	//
	// Flush code...
	//
	SendReceive(EEtelFlushInterfaceQueue, TIpcArgs());
	}

EXPORT_C void RTelSubSessionBase::SetAndGet(const TInt aIpc, TRequestStatus& aStatus, const TDesC8& aDes1, TDes8& aDes2, TReqPriorityType aType) const
/**
@capability None
*/
	{
//
//	Set up a synch request which the client provide a Des in p[0] for server to Set
//
	
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC8*,&aDes1));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaNarrowDesToSetAndGet | KEmergencyClientReq));
	else
		args.Set(1,EIsaNarrowDesToSetAndGet);
	args.Set(2,&aDes2);
	SendReceive(aIpc,args, aStatus);
	
	//
	// Flush code...
	//
	SendReceive(EEtelFlushInterfaceQueue, TIpcArgs());
	}

EXPORT_C void RTelSubSessionBase::SetAndGet(const TInt aIpc, TRequestStatus& aStatus, TDes8& aDes1, const TDesC16& aDes2, TReqPriorityType aType) const
/**
@capability None
*/
	{
//
//	Set up a synch request which the client provide a Des in p[0] for server to Get and a Des in p[1] to Set
//
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,&aDes1);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaNarrowDesToGetUnicodeDesToSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaNarrowDesToGetUnicodeDesToSet);
	args.Set(2,CONST_CAST(TDesC16*,&aDes2));
	SendReceive(aIpc,args,aStatus);
	
	//
	// Flush code...
	//
	SendReceive(EEtelFlushInterfaceQueue, TIpcArgs());
	}

EXPORT_C void RTelSubSessionBase::SetAndGet(const TInt aIpc, TRequestStatus& aStatus, const TDesC16& aDes1, TDes16& aDes2, TReqPriorityType aType) const
/**
@capability None
*/
	{	
//
//	Set up a synch request which the client provide a Des in p[0] for server to Set and a Des in p[1] to Get
//
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC16*,&aDes1));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaUnicodeDesToSetAndGet | KEmergencyClientReq));
	else
		args.Set(1,EIsaUnicodeDesToSetAndGet);
	args.Set(2, &aDes2);
	SendReceive(aIpc,args, aStatus);
	
	//
	// Flush code...
	//
	SendReceive(EEtelFlushInterfaceQueue, TIpcArgs());
	}

EXPORT_C void RTelSubSessionBase::SetAndGet(const TInt aIpc, TRequestStatus& aStatus, const TDesC8& aDes1, TDes16& aDes2, TReqPriorityType aType) const
/**
@capability None
*/
	{	
//
//	Set up a synch request which the client provide a Des in p[0] for server to Set and a Des in p[1] to Get
//
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));
	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC8*,&aDes1));
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest)
		args.Set(1,(EIsaUnicodeDesToGetNarrowDesToSet | KEmergencyClientReq));
	else
		args.Set(1,EIsaUnicodeDesToGetNarrowDesToSet);
	args.Set(2,&aDes2);
	SendReceive(aIpc,args,aStatus);
	
	//
	// Flush code...
	//
	SendReceive(EEtelFlushInterfaceQueue, TIpcArgs());
	}

EXPORT_C TInt RTelSubSessionBase::Set(const TInt aIpc, const RFile& aFile, TReqPriorityType aType) const
/**
@capability None
*/
	{	
//
//	Set up a synch request which the client provides an RFile handle for the server to Set.
//	Slots 0 and 2 are used to transfer the session and file handles respectively.
//
	__ASSERT_ALWAYS(&(SessionHandle()),PanicClient(EEtelPanicNullHandle));	
	TIpcArgs args;
	aFile.TransferToServer(args, 0, 2);
	if (aType == EIsaEmergencyRequest || aType == EIsaPriorityRequest) //use EIsaNull as we don't want to set up any buffers for the args in CTelObject::ReqAnalyserL
		args.Set(1,(EIsaNull | KEmergencyClientReq)); 
	else
		args.Set(1,(EIsaNull));

	return SendReceive(aIpc,args);	
	}
//
// RFax
//
EXPORT_C RFax::RFax()
//
//	C'tor
//
	:iProgressPtr(NULL)
	{}

EXPORT_C TInt RFax::Open(RCall& aCall)
//
//	Setup request to open a fax port and attempt to get a handle on chunk of fax progress 
//  which may have already been created server side when a fax call was initiated.
//	Return KErrNone even if the fax chunk is not found.
//
/**

@capability NetworkControl
*/
	{
	RSessionBase* session=&aCall.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));

	TInt subSessionHandle=aCall.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}
	TPtrC null(_L("FAX"));		// if this is a NULL, server will assume TSY is going to assign
								// a name.
								// Didn't update to _LIT as GCC is inefficient at short names.
	TIpcArgs args;
	args.Set(0,&null);
	args.Set(1,TIpcArgs ::ENothing);
	args.Set(2,subSessionHandle);
	SetSessionHandle(*session);
	TInt res = CreateSubSession(*session,EEtelOpenFromSubSession,args);

	if (res)
		{
		Destruct();
		return res;
		}
		
		TInt chunkHandle= SendReceive(EEtelGlobalKernelObjectHandle);
		if(chunkHandle <=0)
		{
		 Destruct();
		 return KErrBadHandle;
		}
		res= iChunk.SetReturnedHandle(chunkHandle);	
		if( res !=KErrNone)
			Destruct();
	if (res==KErrNone)
		iProgressPtr = REINTERPRET_CAST(TProgress*,iChunk.Base());
	return KErrNone;
	}

EXPORT_C void RFax::Close()
//
//	close a fax port
//
/**
@capability None
*/
 	{
	if (iProgressPtr)
		iChunk.Close();
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C void RFax::ConstructL()
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iPtrHolder = CPtrHolder::NewL(NUM_FAXPTR,NUM_FAXPTRC);
	}

EXPORT_C void RFax::Destruct()
/**
@capability None
*/
	{
	if (iPtrHolder)
		delete iPtrHolder;
	iPtrHolder=NULL;
	}

EXPORT_C void RFax::Read(TRequestStatus& aStatus,TDes8& aDes)
/**
@capability NetworkServices
@capability ReadUserData
*/
	{
	__ASSERT_ALWAYS(aDes.MaxLength()!=0,PanicClient(KErrArgument));
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(0)).Set(CONST_CAST(TUint8*,aDes.Ptr()),aDes.Length(),aDes.MaxLength());
	Get(EEtelFaxRead,aStatus,iPtrHolder->Ptr(0));
	}

EXPORT_C void RFax::Write(TRequestStatus& aStatus,const TDesC8& aDes)
/**
@capability NetworkServices
@capability WriteUserData
*/
	{
	__ASSERT_ALWAYS(aDes.Length()!=0,PanicClient(KErrArgument));
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->PtrC(0)).Set(aDes);
	Set(EEtelFaxWrite,aStatus,iPtrHolder->PtrC(0));
	}

EXPORT_C void RFax::WaitForEndOfPage(TRequestStatus& aStatus) const
/**
@capability None
*/
	{
	Blank(EEtelFaxWaitForEndOfPage,aStatus);
	}

EXPORT_C TInt RFax::TerminateFaxSession() const
/**
@capability NetworkServices
*/
	{
	TInt status=CancelSubSession();
	if (status==KErrNone)
		return Blank(EEtelFaxTerminateFaxSession);
	return(status);
	}

EXPORT_C TInt RFax::GetProgress(TProgress& aProgress) 
// 
//	The handle on the chunk may not have been obtained in RFax::Open() as the chunk may not
//	have existed at that point. Once the fax server is connecting, however, the chunk exists
//	so the handle must be opened here.
//
/**
@capability ReadUserData
*/
	{

	if (iProgressPtr==NULL)
		{
		if (iChunk.Handle() == KNullHandle)
			return KErrBadHandle;
		iProgressPtr = REINTERPRET_CAST(TProgress*,iChunk.Base());
		}
	aProgress = *iProgressPtr;
	return KErrNone;
	}

//
// RCall
//
EXPORT_C RCall::RCall()
/** The default empty constructor, present only to support virtual 
function table export.

A call is identified by TDesC Phone_Name::Line_Name::Call_Name. Whenever the 
string Phone_Name::Line_Name::Call_Name appears in this document it refers 
to this descriptor, and not a subclass. */
	{}

EXPORT_C TInt RCall::OpenNewCall(RTelServer& aServer,const TDesC& aName,TDes& aNewName)
//
//	Setup a request to open a RCall
//	assume RTelServer is connected and phone module loaded
//
/** Opens a new call from a server session and returns the call name.

Opened call subsessions must be explicitly closed by the client - a memory 
leak will result if the RCall object is destroyed before the subsession is 
closed.

Getting the server to name the call is the normal use of this function, as 
the client shouldn't normally need to bother naming the calls itself. However 
it is also possible for aName to explicitly specify the call name e.g. 
Phone_Name::Line_Name::Call_Name. 
If the specified call exists, then the function acts like OpenExistingCall() 
and returns its handle. If the call name does not exist then the TSY creates 
a call with that name. In either case, the aNewName variable is empty on return.

@param aServer The server opening the call.
@param aName The name of the line from which the call is to be opened. The 
format of the name must be Phone_Name::Line_Name::.
@param aNewName On return, contains the name of the new call.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	__ASSERT_ALWAYS(aServer.Handle()!=0,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(aName.Length()!=0,PanicClient(KErrBadName));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}

	TIpcArgs args;
	args.Set(0,CONST_CAST(TDesC*,&aName));
	args.Set(1,CONST_CAST(TDes*,&aNewName));
	SetSessionHandle(aServer);
	ret = CreateSubSession(aServer,EEtelOpenFromSession,args);

	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C TInt RCall::OpenNewCall(RTelServer& aServer,const TDesC& aName)
/** Opens a new call from a server, but does not return the new name to the client.

Opened call subsessions must be explicitly closed by the client - a memory 
leak will result if the RCall object is destroyed before the subsession is 
closed.

Getting the server to name the call is the normal use of this function, as 
the client shouldn't normally need to bother naming the calls itself. However 
it is also possible for aName to explicitly specify the call name e.g. 
Phone_Name::Line_Name::Call_Name. 
If the specified call exists, then the function acts like OpenExistingCall() 
and returns its handle. If the call name does not exist then the TSY creates 
a call with that name.

@param aServer The server opening the call.
@param aName The name of the line from which the call is to be opened. The 
format of the name must be: Phone_Name::Line_Name::
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TName aNewName;
	return OpenNewCall(aServer,aName,aNewName);
	}

EXPORT_C TInt RCall::OpenNewCall(RPhone& aPhone,const TDesC& aName,TDes& aNewName)
//
//	Setup a request to open a RCall
//	assume RTelServer is connected and phone module loaded
//
/** Opens a new call from a phone and returns the name to the client.

Opened call subsessions must be explicitly closed by the client - a memory 
leak will result if the RCall object is destroyed before the subsession is 
closed.

@param aPhone The phone opening the call.
@param aName The name of the line to open the call from. The format of the 
name must be: Line_Name::. The name of the line can be obtained using the 
RPhone::GetLineInfo() function.
@param aNewName On return, contains the name of the new call.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(aName.Length()!=0,PanicClient(KErrBadName));

	TIpcArgs args(CONST_CAST(TDesC*,&aName),CONST_CAST(TDes*,&aNewName),subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);

	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C TInt RCall::OpenNewCall(RPhone& aPhone,const TDesC& aName)
/** Opens a new call from a phone, but does not return the new name to the client.

Opened call subsessions must be explicitly closed by the client - a memory 
leak will result if the RCall object is destroyed before the subsession is 
closed.

@param aPhone The phone opening the call.
@param aName The name of the line to open the call from. The format of the 
name must be: Line_Name::. The name of the line can be obtained using the 
RPhone::GetLineInfo() function.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TName aNewName;
	return OpenNewCall(aPhone,aName,aNewName);
	}

EXPORT_C TInt RCall::OpenNewCall(RLine& aLine,TDes& aNewName)
//
//	Setup a request to open a RCall
//	assume RTelServer is connected and phone module loaded
//
/** Opens a new call from a line and returns the name to the client.

Opened call subsessions must be explicitly closed by the client - a memory 
leak will result if the RCall object is destroyed before the subsession is 
closed.

@param aLine The line opening the call.
@param aNewName On return, contains the name of the new call.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	RSessionBase* session=&aLine.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}
	TInt subSessionHandle=aLine.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle));
	TPtrC name(_L("::"));	// necessary so that server knows to ask TSY for new name

	TIpcArgs args(&name,CONST_CAST(TDes*,&aNewName),subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);

	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C TInt RCall::OpenNewCall(RLine& aLine)
//
//	Setup a request to open a RCall
//	assume RTelServer is connected and phone module loaded
//	pass a null descriptor so the server know and generate a dafault name
//
/** Opens a new call from a line. The name of the new call is not returned.

Opened call subsessions must be explicitly closed by the client - a memory 
leak will result if the RCall object is destroyed before the subsession is 
closed.

@param aLine The line opening the call.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	RSessionBase* session=&aLine.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}
	TInt subSessionHandle=aLine.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle));
	TPtrC null(_L("::"));
	TName newName;	// which is passed back but is not needed
	
	TIpcArgs args(&null,&newName,subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);

	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C TInt RCall::OpenExistingCall(RTelServer& aServer,const TDesC& aName)
//
//	Opens a handle on existing call object. Returns KErrNotFound if it does not exist
//
/** Opens an existing call from a server.

Opened call subsessions must be explicitly closed by the client - a memory 
leak will result if the RCall object is destroyed before the subsession is 
closed.

@param aServer The server opening the call.
@param aName The name of the call to open. Must be of the form: Phone_Name::Line_Name::Call_Name.
@return KErrNone if successful, KErrNotFound if the call name does not exist, 
otherwise another of the system-wide error codes. 
@capability None
*/
	{
	__ASSERT_ALWAYS(aServer.Handle()!=0,PanicClient(EEtelPanicNullHandle));
	//Don't panic the client for zero length names, just return the error code
	if(aName.Length()==0) return KErrBadName;
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}

	TIpcArgs args(CONST_CAST(TDesC*,&aName));
	SetSessionHandle(aServer);
	ret = CreateSubSession(aServer,EEtelOpenByNameFromSession,args);

	if (ret)
		Destruct();
	return ret;
	}
EXPORT_C TInt RCall::OpenExistingCall(RPhone& aPhone,const TDesC& aName)
//
//	Opens a handle on existing call object. Returns KErrNotFound if it does not exist
//
/** Opens an existing call from a phone.

Opened call subsessions must be explicitly closed by the client - a memory 
leak will result if the RCall object is destroyed before the subsession is 
closed.

@param aPhone The phone opening the call.
@param aName The name of the call to open. Must be of the form: Line_Name::Call_Name.
@return KErrNone if successful, KErrNotFound if the call name does not exist, 
otherwise another of the system-wide error codes. 
@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle));
	//Don't panic the client for zero length names, just return the error code
	if(aName.Length()==0) return KErrBadName;

	TIpcArgs args(CONST_CAST(TDesC*,&aName),0,subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenByNameFromSubSession,args);

	if (ret)
		Destruct();
	return ret;
	}
EXPORT_C TInt RCall::OpenExistingCall(RLine& aLine,const TDesC& aName)
//
//	Opens a handle on existing call object. Returns KErrNotFound if it does not exist
//
/** Opens an existing call from a line.

Opened call subsessions must be explicitly closed by the client - a memory 
leak will result if the RCall object is destroyed before the subsession is 
closed.

@param aLine The line opening the call.
@param aName The name of the call to open.
@return KErrNone if successful, KErrNotFound if the call name does not exist, 
otherwise another of the system-wide error codes. 
@capability None
*/
	{
	RSessionBase* session=&aLine.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}
	TInt subSessionHandle=aLine.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle));
		//Don't panic the client for zero length names, just return the error code
	if(aName.Length()==0) return KErrBadName;

	TIpcArgs args(CONST_CAST(TDesC*,&aName),TIpcArgs::ENothing,subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenByNameFromSubSession,args);

	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RCall::ConstructL()
//
//	virtual function which extensions may overload. Called in Open()
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iPtrHolder = CCallPtrHolder::NewL(NUM_CALLPTR,NUM_CALLPTRC);
	}

EXPORT_C void RCall::Destruct()
//
//	virtual function which extensions may overload. Called in Close()
//
/**
@capability None
*/
	{
	if (iPtrHolder)
		delete iPtrHolder;
	iPtrHolder=NULL;
	}

EXPORT_C void RCall::Close()
//
//	Close RCall
//
/** Closes the RCall subsession. This function must be called for all RCall subsessions 
which have been opened - to prevent memory leakage. 
@capability None
*/
	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C TInt RCall::LoanDataPort(TCommPort& aDataPort) const
//
//	Return with infos comm port currently used ( CSY module and COMM::number)
//	TSY then block any requests to the server until  RecoverDataPort() is called
//
/** Allows a client to gain control of the serial communications abstraction used 
to communicate with a modem.

This is the synchronous version of this method.

Once this function has successfully completed, the ETel server queues any 
commands it has to the modem, leaving the channel free for client data. The 
server retakes control of the communications port when the RecoverDataPort() 
function is called. After recovery, queued commands are executed.

@param aDataPort On return, contains the information required to open the 
communications port.
@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code.KErrEtelCallNotActive is returned if the 
function is called before a connection has been established to a remote modem. 
KErrEtelNotCallOwner is returned by the server if this client does not own 
the active call (The default owner of a call is the client who initiated or 
answered the call in the first place - ownership may change however if 
another client requests it and the owner transfers it or closes unexpectedly). 

@capability NetworkServices
@capability NetworkControl
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aDataPort),sizeof(TCommPort),sizeof(TCommPort));
	return Get(EEtelCallLoanDataPort,ptr);
	}

EXPORT_C void RCall::LoanDataPort(TRequestStatus& aStatus,TCommPort& aDataPort)
/** Allows a client to gain control of the serial communications abstraction used 
to communicate with the modem. 

This is the asynchronous version of this method.

Once this function has successfully completed, the ETel server queues any 
commands it has to the modem, leaving the channel free for client data. The 
server retakes control of the communications port when the RecoverDataPort() 
function is called.

@param aStatus A variable that indicates the completion status of the request. 
KErrEtelCallNotActive is returned if the function is called before a connection 
has been established to a remote modem.KErrEtelNotCallOwner is returned by the 
server if this client does not own the active call (The default owner of a call 
is the client who initiated or answered the call in the first place - ownership 
may change however if another client requests it and the owner transfers it or 
closes unexpectedly).
@param aDataPort On request completion, contains the information required to 
open the communications port. 
@capability NetworkServices
@capability NetworkControl
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(0)).Set(REINTERPRET_CAST(TText8*,&aDataPort),sizeof(TCommPort),sizeof(TCommPort));
	Get(EEtelCallLoanDataPort,aStatus,iPtrHolder->Ptr(0));
	}

EXPORT_C void RCall::LoanDataPortCancel() const
/** Allows a client to cancel an outstanding loan communications port request, 
placed using the asynchronous version of the LoanDataPort() function. 

@capability None
*/
	{
	CancelReq(EEtelCallLoanDataPortCancel,EEtelCallLoanDataPort);
	}

EXPORT_C TInt RCall::RecoverDataPort() const
//
//	Recover a comm port after loaning it
//
/** Indicates that the client has finished using the serial communication channel 
- so the ETel server can retake control.

@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. KErrEtelPortNotLoanedToClient is returned 
if the communications port is not on loan. KErrEtelNotCallOwner is returned 
if communications port is loaned, but not by the current call. 
@capability NetworkServices
@capability NetworkControl
*/
	{
	return Blank(EEtelCallRecoverDataPort);
	}

EXPORT_C TInt RCall::GetStatus(TStatus& aCallStatus) const
/** Gets the current call status.
	
@param aStatus On return, contains the current call status
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aCallStatus),sizeof(TStatus),sizeof(TStatus));
	return Get(EEtelCallGetStatus,ptr);
	}

EXPORT_C TInt RCall::GetCaps(TCaps& aCaps) const
/** Gets a snapshot of the current call capabilities. 

@param aCaps On return, contains the call capabilities
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aCaps),sizeof(TCaps),sizeof(TCaps));
	return Get(EEtelCallGetCaps,ptr);
	}

EXPORT_C void RCall::NotifyCapsChange(TRequestStatus& aStatus, TCaps& aCaps)
/** Provides notification of a change in the call capabilities.

@param aStatus A variable that indicates the completion status of the request.
@param aCaps On request completion, contains the call's capability information 
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(1)).Set(REINTERPRET_CAST(TText8*,&aCaps),sizeof(TCaps),sizeof(TCaps));
	Get(EEtelCallCapsChangeNotification,aStatus,iPtrHolder->Ptr(1));
	}

EXPORT_C void RCall::NotifyCapsChangeCancel() const
/** Cancels a "call capabilities change" notification request, placed using the 
NotifyCapsChange() function. 
@capability None
*/
	{
	CancelReq(EEtelCallCapsChangeNotificationCancel,EEtelCallCapsChangeNotification);
	}

EXPORT_C TInt RCall::Dial(const TDesC8& aCallParams,const TTelNumberC& aTelNumber) const
/** Attempts to establish a connection using the telephone number specified.

This is a synchronous version of this method.

The mode of the connection, i.e. whether it is a data or fax connection, is 
determined by whether the RCall object has been opened from a data line or 
a fax line.

@param aCallParams The call parameters used by the TSY (a TCallParamsPckg 
object).
@param aTelNumber A telephone number.
@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. 
@capability NetworkControl
@capability NetworkServices
*/
	{
	if(aTelNumber.Length()==0)
		return KErrArgument;
	return Set(EEtelCallDial,aCallParams,aTelNumber);
	}

EXPORT_C void RCall::Dial(TRequestStatus& aStatus,const TDesC8& aCallParams,const TTelNumberC& aTelNumber)
/** Attempts to establish a connection using the telephone number specified.

This is an asynchronous version of this method.

The mode of the connection, i.e. whether it is a data or fax connection, is 
determined by whether the RCall object has been opened from a data line or 
a fax line.

@param aStatus A variable that indicates the completion status of the request.
@param aCallParams The call parameters used by the TSY (a TCallParamsPckg object).
@param aTelNumber The telephone number 
@capability NetworkControl
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	if(aTelNumber.Length()==0)
		{
		TRequestStatus* status=&aStatus;
		User::RequestComplete(status,KErrArgument);
		return;
		}
	(iPtrHolder->PtrC(0)).Set(aCallParams);
	Set(EEtelCallDial,aStatus,iPtrHolder->PtrC(0),aTelNumber);
	}

EXPORT_C TInt RCall::Dial(const TTelNumberC& aTelNumber) const
/** Attempts to establish a connection using the telephone number specified.

This is a synchronous version of this method.

The mode of the connection, i.e. whether it is a data or fax connection, is 
determined by whether the RCall object has been opened from a data line or 
a fax line.

The default call parameters are used.

@param aTelNumber A telephone number.
@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. 
@capability NetworkControl
@capability NetworkServices
*/
	{
	if(aTelNumber.Length()==0)
		return KErrArgument;
	TPtr8 null(NULL,0);
	return Dial(null,aTelNumber);
	}

EXPORT_C void RCall::Dial(TRequestStatus& aStatus,const TTelNumberC& aTelNumber)
/** Attempts to establish a connection using the telephone number specified.

This is an asynchronous version of this method.

The mode of the connection, i.e. whether it is a data or fax connection, is 
determined by whether the RCall object has been opened from a data line or 
a fax line.

The default call parameters are used.

@param aStatus A variable that indicates the completion status of the request.
@param aTelNumber A telephone number 
@capability NetworkControl
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	if(aTelNumber.Length()==0)
		{
		TRequestStatus* status=&aStatus;
		User::RequestComplete(status,KErrArgument);
		return;
		}
	(iPtrHolder->PtrC(0)).Set(NULL,0);
	Set(EEtelCallDial,aStatus,iPtrHolder->PtrC(0),aTelNumber);
	}

EXPORT_C void RCall::DialCancel() const
/** Cancels an outstanding dial request, placed with the asynchronous version of 
the Dial() function. 
@capability NetworkControl
@capability NetworkServices
*/
	{
	CancelReq(EEtelCallDialCancel,EEtelCallDial);
	}

EXPORT_C TInt RCall::Connect(const TDesC8& aCallParams) const
/** Attempts to connect to a remote modem without dialling - synchronous. It is 
intended for use when other equipment has been used to dial the call.

The mode of the connection, i.e. whether it is a data or fax connection, is 
determined by whether the RCall object has been opened from a data line or 
a fax line.

@param aCallParams The call parameters used by the TSY (a TCallParamsPckg 
object).
@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. 
@capability NetworkServices
*/
	{
	return Set(EEtelCallConnect,aCallParams);
	}
EXPORT_C void RCall::Connect(TRequestStatus& aStatus,const TDesC8& aCallParams)
/** Attempts to connect to a remote modem without dialling - asynchronous. It 
is intended for use when other equipment has been used to dial the call.

The mode of the connection, i.e. whether it is a data or fax connection, is 
determined by whether the RCall object has been opened from a data line or 
a fax line.

@param aStatus A variable that indicates the completion status of the request.
@param aCallParams The call parameters used by the TSY (a TCallParamsPckg object). 
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->PtrC(1)).Set(aCallParams);
	Set(EEtelCallConnect,aStatus,iPtrHolder->PtrC(1));
	}

EXPORT_C TInt RCall::Connect() const
/** Attempts to connect to a remote modem without dialling - synchronous. It 
is intended for use when other equipment has been used to dial the call. 

The mode of the connection, i.e. whether it is a data or fax connection, is 
determined by whether the RCall object has been opened from a data line or 
a fax line.

The default call parameters are used.

@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. 
@capability NetworkServices
*/
	{
	TPtr8 null(NULL,0);
	return Connect(null);
	}

EXPORT_C void RCall::Connect(TRequestStatus& aStatus)
/** Attempts to connect to a remote modem without dialling - asynchronous. It 
is intended for use when other equipment has been used to dial the call.

The mode of the connection, i.e. whether it is a data or fax connection, is 
determined by whether the RCall object has been opened from a data line or 
a fax line.

The default call parameters are used.

@param aStatus A variable that indicates the completion status of the request. 
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->PtrC(1)).Set(NULL,0);
	Set(EEtelCallConnect,aStatus,iPtrHolder->PtrC(1));
	}

EXPORT_C void RCall::ConnectCancel() const
/** Allows a client to cancel a 'connection' request, placed using the asynchronous 
versions of the Connect() function. 
@capability NetworkServices
*/
	{
	CancelReq(EEtelCallConnectCancel,EEtelCallConnect);
	}

EXPORT_C TInt RCall::AnswerIncomingCall(const TDesC8& aCallParams) const
/** Waits for an incoming call and answers it when it arrives - synchronous. 
The function also answers calls which are ringing when it is invoked.

Answering can take some time. Use of the asynchronous variant of this function 
is recommended.

@param aCallParams The call parameters used by the TSY (a TCallParamsPckg 
object).
@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. 
@capability NetworkControl
@capability NetworkServices
*/
	{
	return Set(EEtelCallAnswer,aCallParams);
	}

EXPORT_C void RCall::AnswerIncomingCall(TRequestStatus& aStatus,const TDesC8& aCallParams) 
/** Waits for an incoming call and answers it when it arrives - asynchronous. 
The function also answers calls which are ringing when it is invoked.

@param aStatus A variable that indicates the completion status of the request.
@param aCallParams The call parameters used by the TSY (a TCallParamsPckg object). 
@capability NetworkControl
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->PtrC(2)).Set(aCallParams);
	Set(EEtelCallAnswer,aStatus,iPtrHolder->PtrC(2));
	}

EXPORT_C TInt RCall::AnswerIncomingCall() const
/** Waits for an incoming call and answers it when it arrives - synchronous. 

The default call parameters are used.

Answering can take some time. Use of the asynchronous variant of this function 
is recommended.

@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. 
@capability NetworkControl
@capability NetworkServices
*/
	{
	TPtr8 null(NULL,0);
	return AnswerIncomingCall(null);
	}

EXPORT_C void RCall::AnswerIncomingCall(TRequestStatus& aStatus)
/** Waits for an incoming call and answers it when it arrives - asynchronous. 
The function also answers calls which are ringing when it is invoked.

The default call parameters are used.

@param aStatus A variable that indicates the completion status of the request. 
@capability NetworkControl
@capability NetworkServices
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->PtrC(2)).Set(NULL,0);
	Set(EEtelCallAnswer,aStatus,iPtrHolder->PtrC(2));
	}

EXPORT_C void RCall::AnswerIncomingCallCancel() const
/** Allows a client to cancel an outstanding call answer request, that was placed 
using the asynchronous versions of the AnswerIncomingCall() function. If a 
call has not arrived the TSY notes that it should not answer when one arrives; 
if the call is answering, then an attempt is made to abort. 
@capability NetworkControl
@capability NetworkServices
*/
	{
	CancelReq(EEtelCallAnswerCancel,EEtelCallAnswer);
	}

EXPORT_C TInt RCall::HangUp() const
/** Terminates a call established with the Dial(), Connect() or AnswerIncomingCall() 
functions - synchronous.

When using an AT-command based TSY, HangUp() can only be used once the call 
is in a connected state. However, in a one-box solution, where the TSY interfaces 
more directly to the phone, HangUp() can also be used in a call ringing state.

Hanging up can take some time. Use of the asynchronous variant of this function 
is recommended.

@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. 
@capability NetworkServices
*/
	{
	return Blank(EEtelCallHangUp);
	}

EXPORT_C void RCall::HangUp(TRequestStatus& aStatus) const
/** Terminates a call established with the Dial(), Connect() or AnswerIncomingCall() 
functions - asynchronous.

When using an AT-command based TSY, HangUp() can only be used once the call 
is in a connected state. However, in a one-box solution, where the TSY interfaces 
more directly to the phone, HangUp() can also be used in a call ringing state.

@param aStatus KErrNone if successful, otherwise another of the system-wide 
error codes or an ETel Core error code. 
@capability NetworkServices
*/
	{
	Blank(EEtelCallHangUp,aStatus);
	}

EXPORT_C void RCall::HangUpCancel() const
/** Cancels an outstanding HangUp request, placed using the asynchronous version 
of the HangUp() function. It should be noted that if the call termination 
sequence has begun there is little hope of retaining the call even if this 
function is used. 
@capability NetworkServices
*/
	{
	CancelReq(EEtelCallHangUpCancel,EEtelCallHangUp);
	}

EXPORT_C TInt RCall::TransferOwnership() const
/** Allows clients with call ownership to transfer ownership to another interested 
client.

@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. KErrEtelNoClientInterestedInThisCall is 
returned if no clients are interested in the ownership, i.e. none have a pending 
AcquireOwnership() request. Ownership remains with the client. KErrEtelNotCallOwner 
is returned if the client does not have call ownership. 
@capability NetworkServices
*/
	{
	return Blank(EEtelCallTransferOwnership);
	}

EXPORT_C void RCall::AcquireOwnership(TRequestStatus& aStatus) const
/** Aquires ownership from another client. This function completes when the current 
call owner calls the TransferOwnership() function, or if it closes its handle 
without hanging up. When (and if) the request completes, call ownership has 
transferred to the new client.

Only the client with ownership may terminate the call or gain control of the 
underlying communications channel. All other clients may just monitor the 
call and view its state.

@param aStatus A variable that indicates the completion status of the request. 
@capability NetworkServices
*/
	{
	Blank(EEtelCallAcquireOwnership,aStatus);
	}

EXPORT_C void RCall::AcquireOwnershipCancel() const
/** Cancels an outstanding request to acquire call ownership, placed with the AcquireOwnership() 
function. 
@capability NetworkServices
*/
	{
	CancelReq(EEtelCallAcquireOwnershipCancel,EEtelCallAcquireOwnership);
	}

EXPORT_C TInt RCall::GetBearerServiceInfo(TBearerService& aBearerService) const
/** Gets the bearer service information associated with the call. Bearer service 
information is only available if the call is currently active.

@param aBearerService On return, contains the bearer service information.
@return KErrNone if successful, KErrNotFound if the call name does not exist, 
otherwise another of the system-wide error codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aBearerService),sizeof(TBearerService),sizeof(TBearerService));
	return Get(EEtelCallGetBearerServiceInfo,ptr);
	}

EXPORT_C void RCall::NotifyHookChange(TRequestStatus& aStatus,THookStatus& aHookStatus)
/** Provides notification of a change in the hook status.

@param aStatus A variable that indicates the completion status of the request.
@param aHookStatus On request completion, contains the new hook status 
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(2)).Set(REINTERPRET_CAST(TText8*,&aHookStatus),sizeof(THookStatus),sizeof(THookStatus));
	Get(EEtelCallNotifyHookChange,aStatus,iPtrHolder->Ptr(2));
	}

EXPORT_C void RCall::NotifyHookChangeCancel() const
/** Cancels an outstanding 'hook status change' notification request, placed with 
the NotifyHookChange() function. 
@capability None
*/
	{
	CancelReq(EEtelCallNotifyHookChangeCancel,EEtelCallNotifyHookChange);
	}

EXPORT_C TInt RCall::GetOwnershipStatus(TOwnershipStatus& aOwnershipStatus) const
/** Gets ownership information associated with the call.

@param aOwnershipStatus On return, contains the call ownership status information
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aOwnershipStatus),sizeof(TOwnershipStatus),sizeof(TOwnershipStatus));
	return Get(EEtelCallGetOwnershipStatus,ptr);
	}

EXPORT_C TInt RCall::GetCallParams(TDes8& aParams) const
/** Gets the current call parameters. 

@param aParams On return, contains the call parameters. The variable must 
be an instance of the TCallParamsPckg class.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	return Get(EEtelCallGetCallParams,aParams);
	}

EXPORT_C RCall::TCallParams::TCallParams()
	: iSpeakerControl(RCall::EMonitorSpeakerControlOnUntilCarrier)
	,iSpeakerVolume(RCall::EMonitorSpeakerVolumeLow),iInterval(4),
	iWaitForDialTone(RCall::EDialToneWait),iIsDefault(ETrue)
	{
	iExtensionId=0;
	}

EXPORT_C RCall::TCallParams::TCallParams(TMonitorSpeakerControl aSpeakerControl,
										 TMonitorSpeakerVolume aSpeakerVolume,
										 TUint aInterval,
										 TWaitForDialTone aWaitForDialTone)
		: iSpeakerControl(aSpeakerControl),iSpeakerVolume(aSpeakerVolume),
		  iInterval(aInterval),iWaitForDialTone(aWaitForDialTone),iIsDefault(EFalse)
//
// C'tor for RCall::TCallParams
//
	{
	iExtensionId=0;
	}

EXPORT_C TInt RCall::TCallParams::ExtensionId()
//
// Retrieve Extension Ids supported by this class
//
	{
	return iExtensionId;
	}

EXPORT_C TInt RCall::TCallParams::IsDefault()
//
// Determine whether this struct has been created default or not
//
	{
	return iIsDefault;
	}

EXPORT_C TInt RCall::GetCallDuration(TTimeIntervalSeconds& aTime) const
/** Gets the call duration.

@param aTime On return, contains the call duration (in seconds)
@return KErrNone if successful, otherwise another of the system-wide error 
codes.KErrEtelCallNotActive is returned if there is no active call. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,CONST_CAST(TTimeIntervalSeconds*,&aTime)),sizeof(TTimeIntervalSeconds),sizeof(TTimeIntervalSeconds));
	return Get(EEtelCallGetCallDuration,ptr);
	}

EXPORT_C void RCall::NotifyCallDurationChange(TRequestStatus& aStatus,TTimeIntervalSeconds& aTime)
/** Provides notification of an increase in call duration (by one second).

@param aStatus A variable that indicates the completion status of the request.
@param aTime On request completion, contains the current update of the call 
duration. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(3)).Set(REINTERPRET_CAST(TText8*,&aTime),sizeof(TTimeIntervalSeconds),sizeof(TTimeIntervalSeconds));
	Get(EEtelCallNotifyDurationChange,aStatus,iPtrHolder->Ptr(3));
	}

EXPORT_C void RCall::NotifyCallDurationChangeCancel() const
/** Cancels an outstanding 'call duration change' notification request, placed 
using the NotifyCallDurationChange() function. 
@capability None
*/
	{
	CancelReq(EEtelCallNotifyDurationChangeCancel,EEtelCallNotifyDurationChange);
	}

EXPORT_C void RCall::NotifyStatusChange(TRequestStatus& aStatus,TStatus& aCallStatus)
/** Provides notification about a change in the status of a call.

@param aStatus A variable that indicates the completion status of the request.
@param aCallStatus On request completion, contains the new call status. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(4)).Set(REINTERPRET_CAST(TText8*,&aCallStatus),sizeof(TStatus),sizeof(TStatus));
	Get(EEtelCallNotifyStatusChange,aStatus,iPtrHolder->Ptr(4));
	}

EXPORT_C void RCall::NotifyStatusChangeCancel() const
/** Cancels an outstanding 'call status change' notification request, placed using 
the NotifyStatusChange() function. 
@capability None
*/
	{
	CancelReq(EEtelCallNotifyStatusChangeCancel,EEtelCallNotifyStatusChange);
	}

EXPORT_C RCall::TCallInfo::TCallInfo()
		:	iHookStatus(EHookStatusUnknown),
			iStatus(EStatusUnknown),
			iDuration(0)
	{}

EXPORT_C TInt RCall::GetInfo(TCallInfo& aCallInfo) const
/** Gets the current call information.

@param aCallInfo On return, contains the current call information
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aCallInfo),sizeof(TCallInfo),sizeof(TCallInfo));
	return Get(EEtelCallGetInfo,ptr);
	}

EXPORT_C TInt RCall::GetFaxSettings(TFaxSessionSettings& aSettings) const
/** Gets the current fax settings.

@param aSettings On return, contains the fax settings
@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aSettings),sizeof(TFaxSessionSettings),sizeof(TFaxSessionSettings));
	return Get(EEtelCallGetFaxSettings,ptr);
	}

EXPORT_C TInt RCall::SetFaxSettings(const TFaxSessionSettings& aSettings) const
/** Assigns the fax settings.

@param aSettings The new fax settings
@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. 
@capability WriteDeviceData
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,CONST_CAST(TFaxSessionSettings*,&aSettings)),sizeof(TFaxSessionSettings),sizeof(TFaxSessionSettings));
	return Set(EEtelCallSetFaxSettings,ptr);
	}

/** 
Passes an open file handle to the server in order that the file may be adopted by the server.  
This function is not intended for public use.

@internalTechnology
@released Released in v9.0 for sharing access to files betwen client and Etel server.
@param aSharedFile Handle to file to be passed to the server.
@return KErrNone if successful, otherwise another of the system-wide error 
codes or an ETel Core error code. 
@capability NetworkServices
*/
EXPORT_C TInt RCall::AdoptFaxSharedHeaderFile(const RFile& aSharedFile) const

	{
	return Set(EEtelAdoptFaxSharedHeaderFile, aSharedFile);
	}

EXPORT_C TInt RCall::ReferenceCount() const
/**
@capability None
*/
 	{
 	TInt count;
 	TPtr8 ptr(REINTERPRET_CAST(TText8*,&count),sizeof(TInt),sizeof(TInt));
 	Get(EEtelCallReferenceCount,ptr);
 	return count;
	}

//
// RLine
//
EXPORT_C RLine::RLine()
/** Constructs an empty default constructor which is present only to support virtual 
function table export. */
	{}

EXPORT_C TInt RLine::Open(RTelServer& aServer,const TDesC& aName)
//
//	Setup a request to open a RLine
//	assume RTelServer is connected and phone module loaded
//
/** Opens a line from a server connection. It might be used, in preference to the 
previous version, when access to phone functionality is not required.

Opened line subsessions must be explicitly closed by the client - a memory 
leak will result if the RLine object is destroyed before the subsession is 
closed.

The line name must be known before this function can be used. Developers intending 
to develop cross-platform applications should not use this function.

@param aServer The server from which the line is to be opened.
@param aName The name of the line. The format of the name must be as shown 
below:PhoneName::LineName.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	__ASSERT_ALWAYS(aServer.Handle()!=0,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(aName.Length()!=0,PanicClient(KErrBadName));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}

	TIpcArgs args(CONST_CAST(TDesC*,&aName));
	SetSessionHandle(aServer);
	ret = CreateSubSession(aServer,EEtelOpenFromSession,args);

	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C TInt RLine::Open(RPhone& aPhone,const TDesC& aName)
//
//	open a line by name
//
/** Opens a line from a phone connection.

Opened line subsessions must be explicitly closed by the client - a memory 
leak will result if the RLine object is destroyed before the subsession is 
closed.

@param aPhone The phone from which the line is to be opened.
@param aName The name of the line. Must match the name returned by the 
RPhone::GetLineInfo() function.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(aName.Length()!=0,PanicClient(KErrBadName));

	TIpcArgs args(CONST_CAST(TDesC*,&aName),TIpcArgs::ENothing,subSessionHandle);
	SetSessionHandle(*session);
	ret = CreateSubSession(*session,EEtelOpenFromSubSession,args);

	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RLine::Close()
/** Closes the line. This function must be called for all RLine subsessions which 
have been opened - to prevent memory leakage. 
@capability None
*/
	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C void RLine::ConstructL()
//
//	virtual function which extensions may overload. Called in Open()
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iPtrHolder = CPtrHolder::NewL(NUM_LINEPTR);
	}

EXPORT_C void RLine::Destruct()
//
//	virtual function which extensions may overload. Called in Close()
//
/**
@capability None
*/
	{
	if (iPtrHolder)
		delete iPtrHolder;
	iPtrHolder=NULL;
	}

EXPORT_C TInt RLine::GetCaps(TCaps& aCaps) const
/** Gets the line capabilities. 

@param aCaps On return, contains the line capabilities.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aCaps),sizeof(TCaps),sizeof(TCaps));
	return Get(EEtelLineGetCaps,ptr);
	}

EXPORT_C TInt RLine::GetStatus(RCall::TStatus& aStatus) const
/** Gets line status information.

@param aStatus On return, contains the line status.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aStatus),sizeof(RCall::TStatus),sizeof(RCall::TStatus));
	return Get(EEtelLineGetStatus,ptr);
	}

EXPORT_C TInt RLine::GetHookStatus(RCall::THookStatus& aHookStatus) const
/** Gets the current hook status.

@param aHookStatus On return, contains the current hook status
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aHookStatus),sizeof(RCall::THookStatus),sizeof(RCall::THookStatus));
	return Get(EEtelLineGetHookStatus,ptr);
	}

EXPORT_C TInt RLine::EnumerateCall(TInt& aCount) const
/** Gets the number of calls opened from a line.

@param aCount On return, contains the number of calls opened from the line.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aCount),sizeof(TInt),sizeof(TInt));
	return Get(EEtelLineEnumerateCall,ptr);
	}

EXPORT_C TInt RLine::GetCallInfo(TInt aIndex,TCallInfo& aCallInfo) const
/** Gets enough information about a call to differentiate it from other calls on 
the line. 

@param aIndex The index of the call for which information is to be obtained.
@param aCallInfo On return, contains the call information.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TCallInfoIndex info;
	info.iIndex=aIndex;
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&info),sizeof(TCallInfoIndex),sizeof(TCallInfoIndex));
	TInt ret=Get(EEtelLineGetCallInfo,ptr);
	//
	// MP - do bitwise copy of a whole structure this helps
	// later on if decided to add another item in the structure
	//
	if(ret==KErrNone)
		aCallInfo=info.iInfo;
	return ret;
	}


EXPORT_C RLine::TLineInfo::TLineInfo()
		:	iHookStatus(RCall::EHookStatusUnknown),
			iStatus(RCall::EStatusUnknown),
			iNameOfLastCallAdded(NULL),
			iNameOfCallForAnswering(NULL)
	{}

EXPORT_C TInt RLine::GetInfo(TLineInfo& aLineInfo) const
/** Gets a snapshot of the current line information.

@param aLineInfo On return, contains the current line information
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aLineInfo),sizeof(TLineInfo),sizeof(TLineInfo));
	return Get(EEtelLineGetInfo,ptr);
	}

EXPORT_C void RLine::NotifyCapsChange(TRequestStatus& aStatus, TCaps& aCaps)
/** Provides notification of a change in the line capabilities.

@param aStatus A variable that indicates the completion status of the request.
@param aCaps On request completion, contains the new line capabilities 
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(1)).Set(REINTERPRET_CAST(TText8*,&aCaps),sizeof(TCaps),sizeof(TCaps));
	Get(EETelLineCapsChangeNotification,aStatus,iPtrHolder->Ptr(1));
	}

EXPORT_C void RLine::NotifyCapsChangeCancel() const
/** Cancels an "line capabilities change" notification request, placed using the 
NotifyCapsChange() function. 
@capability None
*/
	{
	CancelReq(EETelLineCapsChangeNotificationCancel,EETelLineCapsChangeNotification);
	}

EXPORT_C void RLine::NotifyIncomingCall(TRequestStatus& aStatus, TName& aName)
/** Provides notification to a client when an incoming call is detected.

If there is no outstanding request to answer the next call, a call is automatically 
created by the server and its name is passed back to the client.

@param aStatus A variable that indicates the completion status of the request.
@param aName On request completion, contains the name of the incoming call. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(2)).Set(REINTERPRET_CAST(TText8*,&aName),sizeof(TName),sizeof(TName));
	Get(EEtelLineNotifyIncomingCall,aStatus,iPtrHolder->Ptr(2));
	}

EXPORT_C void RLine::NotifyIncomingCallCancel() const
/** Cancels an outstanding incoming call notification, placed with the NotifyIncomingCall() 
function. 
@capability None
*/
	{
	CancelReq(EEtelLineNotifyIncomingCallCancel,EEtelLineNotifyIncomingCall);
	}

EXPORT_C void RLine::NotifyHookChange(TRequestStatus& aStatus,RCall::THookStatus& aHookStatus)
/** Provides notification when the hook status changes.

@param aStatus A variable that indicates the completion status of the request.
@param aHookStatus On request completion, contains the new hook status. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(3)).Set(REINTERPRET_CAST(TText8*,&aHookStatus),sizeof(RCall::THookStatus),sizeof(RCall::THookStatus));
	Get(EEtelLineNotifyHookChange,aStatus,iPtrHolder->Ptr(3));
	}

EXPORT_C void RLine::NotifyHookChangeCancel() const
/** Cancels an outstanding hook change notification request, placed using the NotifyHookChange() 
function. 
@capability None
*/
	{
	CancelReq(EEtelLineNotifyHookChangeCancel,EEtelLineNotifyHookChange);
	}

EXPORT_C void RLine::NotifyStatusChange(TRequestStatus& aStatus,RCall::TStatus& aLineStatus)
/** Provides notification about a change in the line status.

@param aStatus A variable that indicates the completion status of the request.
@param aLineStatus On request completion, contains the new line status. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(4)).Set(REINTERPRET_CAST(TText8*,&aLineStatus),sizeof(RCall::TStatus),sizeof(RCall::TStatus));
	Get(EEtelLineNotifyStatusChange,aStatus,iPtrHolder->Ptr(4));
	}

EXPORT_C void RLine::NotifyStatusChangeCancel() const
/** Cancels an outstanding line status change notification request, placed using 
the NotifyStatusChange() function. 
@capability None
*/
	{
	CancelReq(EEtelLineNotifyStatusChangeCancel,EEtelLineNotifyStatusChange);
	}

EXPORT_C void RLine::NotifyCallAdded(TRequestStatus& aStatus,TName& aName)
/** Provides notification that a new call is being added to the line.

@param aStatus A variable that indicates the completion status of the request.
@param aName On request completion, contains the name of the new call. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(5)).Set(REINTERPRET_CAST(TText8*,&aName),sizeof(TName),sizeof(TName));
	Get(EEtelLineNotifyCallAdded,aStatus,iPtrHolder->Ptr(5));
	}

EXPORT_C void RLine::NotifyCallAddedCancel() const
/** Cancels an outstanding 'new call added' notification request, placed using 
the NotifyCallAdded() function. 
@capability None
*/
	{
	CancelReq(EEtelLineNotifyCallAddedCancel,EEtelLineNotifyCallAdded);
	}

//
// RPhone
//
EXPORT_C RPhone::RPhone()
/** Empty default constructor.

Constructs an empty RPhone object, and is present only to support virtual 
function table export. */
	{}
			
EXPORT_C void RPhone::Close()
/** Closes the phone. This function must be called for all RPhone subsessions which 
have been opened - to prevent memory leakage. 
@capability None
*/
	{
	Destruct();
	CloseSubSession(EEtelClose);
	}

EXPORT_C TInt RPhone::Open(RTelServer& aServer,const TDesC &aName)
/** Opens a phone subsession by name, and starts the modem initialisation process.
	
Opened phone subsessions must be explicitly closed by the client - a memory 
leak will result if the RPhone object is destroyed before the subsession is 
closed.

@param aSession A session, that must be connected to the server.
@param aName The name of the phone to be opened
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 

@capability None
*/
	
	
	{
	__ASSERT_ALWAYS(aServer.Handle()!=0,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(aName.Length()!=0,PanicClient(KErrBadName));
	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}

	TIpcArgs args(CONST_CAST(TDesC*,&aName),0);
	SetSessionHandle(aServer);
	ret = CreateSubSession(aServer,EEtelOpenFromSession,args);

	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RPhone::ConstructL()
//
//	virtual function which extensions may overload. Called in Open()
//
/**
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iPtrHolder = CPtrHolder::NewL(NUM_PHONEPTR);
	}

EXPORT_C void RPhone::Destruct()
//
//	virtual function which extensions may overload. Called in Close()
//
/**
@capability None
*/
	{
	if (iPtrHolder)
		delete iPtrHolder;
	iPtrHolder=NULL;
	}

EXPORT_C TInt RPhone::Initialise()
//
// Synchronous Initialisation
//
/** Initialises, or re-initialises, a phone - synchronous. If the function is 
called during the automatic initialisation, the function completes when the 
phone is initialised. If the function is called after the automatic initialisation 
has finished, the phone is re-initialised.

Initialisation may take significant time to complete.

The phone hardware is usually automatically initialised before the first command 
is sent.

Functions which retrieve information about the modem should be used once this 
function has completed - such information may be incorrect before initialisation 
is complete.

@return KErrNone if successful, otherwise one of the ETel Core error codes 
or another system-wide error codes. Specific ETel errors which might be returned 
are: KErrEtelInitialisationFailure and KErrEtelModemNotDetected. 
@capability NetworkControl
*/
	{
	return Blank(EEtelPhoneInitialise);
	}

EXPORT_C void RPhone::Initialise(TRequestStatus& aStatus)
/** Initialises or re-initialises a phone - asynchronous.

If the function is called during the automatic initialisation, the function 
completes when the phone is initialised. If the function is called after the 
automatic initialisation has finished, the phone is re-initialised.

Initialisation may take significant time to complete.

The phone hardware is usually automatically initialised before the first command 
is sent.

Functions which retrieve information about the modem should be used once this 
function has completed - such information may be incorrect before initialisation 
is complete.

@param aStatus A variable that indicates the completion status of the request. 
ETel errors which might be returned are: KErrEtelInitialisationFailure and 
KErrEtelModemNotDetected. 
@capability NetworkControl
*/
	{
	Blank(EEtelPhoneInitialise,aStatus);
	}

EXPORT_C void RPhone::InitialiseCancel()
/** Cancels a request to initialise the modem, placed using the asynchronous variant 
of the Initialise() function. 
@capability NetworkControl
*/
	{
	CancelReq(EEtelPhoneInitialiseCancel,EEtelPhoneInitialise);
	}

EXPORT_C RPhone::TPhoneInfo::TPhoneInfo()
/** Empty constructor. */
	{}

EXPORT_C TInt RPhone::GetInfo(TPhoneInfo& aPhoneInfo) const
/** Gets a snapshot of the current phone information. 

This function is expected to complete quickly. Consequently, an asynchronous 
variant has not been provided.

@param aPhoneInfo On return, contains the current phone information
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aPhoneInfo),sizeof(TPhoneInfo),sizeof(TPhoneInfo));
	return Get(EEtelPhoneGetInfo,ptr);
	}

EXPORT_C void RPhone::NotifyModemDetected(TRequestStatus& aStatus,TModemDetection& aDetection)
/** Provides notification when the local modem is detected, or when a previously 
connected modem can no longer be contacted.

Clearly, this function has no use if the modem (DCE) is permanently attached 
to the computing device (DTE).

@param aStatus A flag that indicates the completion status of the request.
@param aDetection On return, contains the new status. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(1)).Set(REINTERPRET_CAST(TText8*,&aDetection),sizeof(TModemDetection),sizeof(TModemDetection));
	Get(EEtelPhoneNotifyModemDetected,aStatus,iPtrHolder->Ptr(1));
	}

EXPORT_C void RPhone::NotifyModemDetectedCancel() const
/** Cancels an outstanding modem detection notification, placed using the NotifyModemDetected() 
function. 
@capability None
*/
	{
	CancelReq(EEtelPhoneNotifyModemDetectedCancel,EEtelPhoneNotifyModemDetected);
	}

EXPORT_C void RPhone::NotifyCapsChange(TRequestStatus& aStatus, TCaps& aCaps)
/** Provides notification of changes in phone capabilities.

@param aStatus A variable that indicates the completion status of the request.
@param aCaps On request completion, contains the phone capabilities. 
@capability None
*/
	{
	__ASSERT_ALWAYS(iPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	(iPtrHolder->Ptr(2)).Set(REINTERPRET_CAST(TText8*,&aCaps),sizeof(TCaps),sizeof(TCaps));
	Get(EETelPhoneCapsChangeNotification,aStatus,iPtrHolder->Ptr(2));
	}

EXPORT_C void RPhone::NotifyCapsChangeCancel() const
/** Cancels a 'Capabilities change' notification request, placed using the NotifyCapsChange() 
function. 
@capability None
*/
	{
	CancelReq(EETelPhoneCapsChangeNotificationCancel,EETelPhoneCapsChangeNotification);
	}

EXPORT_C TInt RPhone::GetCaps(TCaps& aCaps) const
/** Gets the phone's capabilities.

@param aCaps On return, contains the phone capabilities.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aCaps),sizeof(TCaps),sizeof(TCaps));
	return Get(EEtelPhoneGetCaps,ptr);
	}

EXPORT_C TInt RPhone::GetStatus(TStatus& aStatus) const
/** Gets information about the status of the phone.

@param aStatus On return, contains the current phone status.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aStatus),sizeof(TStatus),sizeof(TStatus));
	return Get(EEtelPhoneGetStatus,ptr);
	}

EXPORT_C TInt RPhone::EnumerateLines(TInt& aCount) const
/** Gets the number of lines associated with the phone.

@see RPhone::GetLineInfo()

@param aCount On return, contains the number of lines associated with the 
phone.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aCount),sizeof(TInt),sizeof(TInt));
	return Get(EEtelPhoneEnumerateLines,ptr);
	}

EXPORT_C TInt RPhone::GetLineInfo(const TInt aIndex,TLineInfo& aLineInfo) const
/** 

Returns a RPhone::TLineInfo containing information about a 
line.  Select the line by passing RPhone::GetLineInfo() a 
number from zero to (number of lines-1). 
RPhone::EnumerateLines() returns the number of lines.  A 
line's information is enough to differentiate it from the 
other lines associated with the phone. 

Included in the line information is the line's name, 
RPhone::TLineInfo::iName. Pass this to RLine::Open() to open the line 
and hence to control the line's calls.

Typically, line zero is the phone's voice line. However, you 
should not presume this.  Instead, RPhone::TLineInfo::iLineCapsFlags 
contains a flag that describes the type of line.

@see RPhone::EnumerateLines()

@param aIndex The index of the line for which information is required.
@param aLineInfo On return, contains the line information.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TLineInfoIndex info;
	info.iIndex=aIndex;
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&info),sizeof(TLineInfoIndex),sizeof(TLineInfoIndex));
	TInt ret=Get(EEtelPhoneGetLineInfo,ptr);
	//
	// MP - do bitwise copy of a whole structure this helps
	// later on if decided to add another item in the structure
	//
	if(ret==KErrNone)
		aLineInfo=info.iInfo;
	return ret;
	}

EXPORT_C TInt RPhone::SetEmergencyClient(const TUint32 aEmergencyRequest) const
/** This method allows the client to instruct the ETel server and the TSY to preallocate 
extra memory for any potential emergency request handling from this client. 
It allows other, non-priority, clients who operate under emergency conditions, 
to have memory preallocated for them in the ETel server. The request gets passed down 
to the TSY, so that the TSY can also proceed with the necessary memory pre-allocation. 
The client specifies the type of emergency requests that it wishes to handle in the 
aEmergencyRequest parameter. aEmergencyRequest is a bit-mask where each bit-flag represents 
the emergency request type that the client wishes to handle. See definition of TEmergencyRequest for the 
emergency request types bit-flag positions.

The memory required is calculated by the server and the TSY according to the 
aEmergencyRequest parameter. If the pre-allocation of memory within this method 
fails, the method returns KErrNoMemory. If the same emergency client calls this method 
multiple times, the ETel server will return the KErrAlreadyExists error. KErrAlreadyExists 
will also be returned if another RPhone client created using the same RTelServer session 
also calls this method. 

Note that the ETel allocates emergency heap per client or more precisely per client's 
RTelServer session. The client is expected to use only one emergency 
sub-session of the same type at a time. If the client has a requirement to use multiple, 
concurrent subsessions that handle the same emergency functionality, then it needs 
to open separate RTelServer sessions, otherwise ETel might run out of the preallocated emergency memory.

If the API is called after the call to RTelServer::SetPriorityClient from the 
same session then the function returns KErrAlreadyExists.  
 
@param aEmergencyRequest,the type of emergency request passed from enum TEmergencyRequest.
@see TEmergencyRequest
@return KErrNone if successful,KErrNoMemory if a leave occurs during the 
pre-allocation of memory, otherwise another of the system-wide error codes.
@capability WriteDeviceData
*/
	{
	TPtr8 ptr(reinterpret_cast<TText8*>(const_cast<TUint32*>(&aEmergencyRequest)),sizeof(TUint32),sizeof(TUint32));
	return Set(EETelPhoneSetEmergencyClient,ptr);
	}

//
// RTelServer
//
EXPORT_C RTelServer::RTelServer()
/** Empty default C++ constructor.

Constructs and empty RTelServer object, and is present only to support virtual 
function table export. */
	{
	} // RTelServer::RTelServer


/**
 *  Connects the client to the ETel Server. It must be used before any of other 
 *  functions during a telephony session.
 *
 *  @param aMessageSlots  The number of message slots available to this session.
 *
 *  @return KErrNone if successful, otherwise another of the system-wide error 
 *          codes. 
 *
 *  @capability None
 */
EXPORT_C TInt RTelServer::Connect(TInt aMessageSlots)
	{
	//
	// Because ETel is loaded by the Comms Root Server which is generally
	// started during the boot the first CreateSession should commonly succeed;
	// however for test code this is still a possibility that it is not running.
	// Hence here we try starting it.
	//
	TInt  result;

	result = CreateSession(ETEL_SERVER_NAME, Version(), aMessageSlots);
	if (result == KErrNotFound  ||  result == KErrServerTerminated)
		{	
		//
		// Need to call StartC32() to get ETel running...
		//
		result = StartC32();
		if (result != KErrNone  &&  result != KErrAlreadyExists)
	    	{
			return result;
	    	}

		//
		// At this point, etel server should definately be running...
		//
		result = CreateSession(ETEL_SERVER_NAME, Version(), aMessageSlots);
		}

	return result;
	} // RTelServer::Connect


EXPORT_C TInt RTelServer::GetPhoneInfo(const TInt aIndex,TPhoneInfo& aPhoneInfo) const
/** Gets information associated with the specified phone.
	
@param aIndex Index of the specified phone.
@param aPhoneInfo On return, contains the phone information.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPckg<TPhoneInfo> result(aPhoneInfo);

	TIpcArgs args(&result,aIndex);
	return SendReceive(EEtelServerPhoneInfoByIndex,args);
	}

EXPORT_C TInt RTelServer::EnumeratePhones(TInt& aCount) const
/** Get the total number of phones supported by all the ETel (TSY) modules
currently loaded.

@see RTelServer::GetPhoneInfo()

@param aCount On return, contains the number of phones available.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPckg<TInt> result(aCount);
	TIpcArgs args(&result);
	return SendReceive(EEtelServerEnumeratePhones,args);
	}

EXPORT_C TInt RTelServer::GetTsyName(const TInt aIndexOfPhone, TDes& aTsyName) const
/** Gets the name of the TSY module to which the phone belongs.

@param aIndexOfPhone Index of the specified phone.
@param aTsyName On return, contains the name of the TSY module.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 

@capability None
*/
	{
	TIpcArgs args(aIndexOfPhone,&aTsyName);
	return SendReceive(EEtelServerGetTsyName,args);
	}

EXPORT_C TInt RTelServer::LoadPhoneModule(const TDesC& aName) const
//
//	load phone module by name
//
/** Loads an ETel TSY module. The TSY name should either be know beforehand and be hard coded or
the name of the TSY to be loaded can be obtained by querying the CommDb for the current TSY. 

The file name need not include the TSY extension, as this is added by the 
server.
	
Once loaded, information about the phone(s) added by the module can be accessed 
using the EnumeratePhones() and GetPhoneInfo() functions.
	
	
@param aName The name of the module to be loaded.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TIpcArgs args(CONST_CAST(TDesC*,&aName));
	return SendReceive(EEtelServerLoadPhoneModule,args);
	}

EXPORT_C TInt RTelServer::UnloadPhoneModule(const TDesC& aName) const
//
//	Unload phone module by name
//
/** Unloads an ETel TSY module.
	
The file name need not include the TSY extension, as this is added by the 
server.
	
TSY modules are automatically unloaded when the RTelServer session is closed, 
unless in use by another session.
	
@param aName The name of the module to be unloaded.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TIpcArgs args(CONST_CAST(TDesC*,&aName));
	return SendReceive(EEtelServerClosePhoneModule,args);
	}

EXPORT_C TInt RTelServer::SetPriorityClient() const
//
// Request that this client be made the priority client and write
// the name of the priority call back to aCallName
//
/** Requests the server to allocate priority status to the client that calls it. 

To prevent an out of memory error, the server uses pre-allocated memory to 
service the requests of priority clients.

Only one client may have priority status in the system, and the first client 
to ask for it receives it. If another application has been given priority 
client status, the function returns KErrAlreadyExists.

If the API is called after the call to RPhone::SetEmergencyClient from the 
same session then the function returns KErrAlreadyExists.

Priority status should be requested early in an application's execution.

The server pre-allocates only a small amount of memory. If there is more than 
one active priority request it may run out. This was not an issue in v5 as 
there was only one priority request - the emergency call.

The API can be called only by the default phone application.

In case of hardware the API can be called by providing the SID of the default 
phone application in ETel.iby file found at \epoc32\rom\include

A patchable constant KPriorityClientSid is used for defining the SID value.

For example:
patchdata etel.dll@KPriorityClientSid 0x12345678

In case of emulator the API can be called by providing the SID of the default 
phone application in epoc.ini file found at \epoc32\data

In epoc.ini file find the following line
etel_KPriorityClientSid <SID>

For example:
etel_KPriorityClientSid 0x12345678

@return KErrNone if successful, KErrNoMemory if a leave occurs during the 
pre-allocation of memory, otherwise another of the system-wide error codes. 
@capability NetworkControl
@capability SID
@deprecated Use RTelServer::SetPriorityClientV2 followed by RPhone::SetEmergencyClient(EEmergencyCSVoiceCallRequest)
*/
	{
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(EEtelServerSetPriorityClient,args);
	}

EXPORT_C TInt RTelServer::IsSupportedByModule(const TDesC& aTsyName, const TInt aMixin, TBool& aResult) const
/** Tests what types of extended functionality a TSY module supports. It is assumed 
the TSY module supports all functionality defined in this, the core, API.

@param aTsyName The name of the TSY module.
@param aMixin A value defined in an enumeration defined in a public header file (etelmm.h)
available both to the client and the TSY which represents a limited subset of functionality.
@see TMultimodeETelV1Api
@see TMultimodeETelV2Api
@see TMultimodeETelV3Api
@see TMultimodeETelV4Api
@see TMultimodeEtelV5Api
@see TMultimodeEtelV6Api
@see TMultimodeETel3rdPartyV1Api
@param aResult On return, contains the result of the query: TRUE (functionality 
available) or FALSE (functionality unavailable).
@return KErrNone if successful, KErrNotFound if the specified TSY module is 
not available, otherwise another of the system-wide error codes. 
@capability None
*/
	{
	TPtr8 ptr1(REINTERPRET_CAST(TText8*,CONST_CAST(TInt*,&aMixin)),sizeof(TInt),sizeof(TInt));
	TPtr8 ptr2(REINTERPRET_CAST(TText8*,&aResult),sizeof(TBool),sizeof(TBool));
	TIpcArgs args(CONST_CAST(TDesC*,&aTsyName),&ptr1,&ptr2);
	return SendReceive(EEtelServerQueryTsyFunctionality,args);
	}

EXPORT_C TInt RTelServer::GetTsyVersionNumber(const TDesC& aTsyName,TVersion& aVersion) const
/** Gets the version number of the specified TSY module.

@param aTsyName TSY module for which to get version number
@param aVersion On return, the version number
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aVersion),sizeof(TVersion),sizeof(TVersion));
	TIpcArgs args(CONST_CAST(TDesC*,&aTsyName),&ptr);
	return SendReceive(EEtelServerGetTsyVersionNo,args);
	}

EXPORT_C TInt RTelServer::SetExtendedErrorGranularity(const TErrorGranularity aGranularity) const
/** Requests the server to register the specified error granularity against the 
client which calls it.

The options for error granularity are:

core error granularity, the default, means that a client will only receive 
errors using ETel and System error codes

extended error granularity allows a client to receive richer error information, 
using additional proprietary client/TSY error codes

After registering interest in extended errors, the client will receive an 
extended error code whenever a requests ends in an error that is translated 
by the TSY into one of the proprietary extended error codes. If an error occurs 
that the TSY cannot translate into an extended error, then the TSY will return 
one of the core errors instead. For this reason, the core error space and 
the extended error space must not clash. 

@param aGranularity Error granularity to set
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability None
*/
	{
	TPtr8 ptr(REINTERPRET_CAST(TText8*,CONST_CAST(TErrorGranularity*,&aGranularity)),
		sizeof(TErrorGranularity),sizeof(TErrorGranularity));
	TIpcArgs args(&ptr);
	return SendReceive(EEtelServerSetExtendedErrorGranularity, args);
	}


/**
 *  Set a heap mark using the standard Symbian OS __UHEAP_MARK macro in
 *  the ETel Server thread.
 *
 *  @note This API is only available on debug releases.
 *
 *  @see __UHEAP_MARK
 *
 *  @capability None
 */
EXPORT_C TInt RTelServer::__DbgMarkHeap()
	{
#if defined(_DEBUG)
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(EEtelDbgMarkHeap,args);
#else
	return KErrNone;
#endif
	}


/**
 *  Performs a heap mark check using the standard Symbian OS __UHEAP_CHECK
 *  macro in the ETel Server thread.
 *
 *  @param aCount  The number of heap cells expected to be allocated at
 *                 the current nest level.
 *
 *  @note This API is only available on debug releases.
 *
 *  @see __UHEAP_CHECK
 *
 *  @capability None
 */
EXPORT_C TInt RTelServer::__DbgCheckHeap(TInt aCount)
	{
#if defined(_DEBUG)
	TIpcArgs args(aCount);
	return SendReceive(EEtelDbgCheckHeap,args);
#else
	aCount+=0;	// to prevent unreferenced parameter warning in release builds
	return KErrNone;
#endif
	}


/**
 *  Perfom a heap mark end check using the standard Symbian OS __UHEAP_MARKENDC
 *  macro in the ETel Server thread.
 *
 *  @param aCount  The number of heap cells expected to remain allocated
 *                 at the current nest level.
 *
 *  @note This API is only available on debug releases.
 *
 *  @see __UHEAP_MARKENDC
 *
 *  @capability None
 */
EXPORT_C TInt RTelServer::__DbgMarkEnd(TInt aCount)
	{
#if defined(_DEBUG)
	TIpcArgs args(aCount);
	return SendReceive(EEtelDbgMarkEnd,args);
#else 
	aCount+=0;	// to prevent unreferenced parameter warning in release builds
	return KErrNone;
#endif
	}


/**
 *  Request a simulated heap allocation failure using the standard Symbian OS
 *  __UHEAP_FAILNEXT macro in the ETel Server thread.
 *
 *  @param aCount  Determines when the allocation will fail. This should be
 *                 greater than 0 (although a value of 0 would cause the
 *                 __UHEAP_RESET to be called to disable the failure settings).
 *
 *  @note This API is only available on debug releases.
 *
 *  @see __UHEAP_FAILNEXT
 *
 *  @capability None
 */
EXPORT_C TInt RTelServer::__DbgFailNext(TInt aCount)
	{
#if defined(_DEBUG)
	TIpcArgs args(aCount);
	return SendReceive(EEtelDbgFailNext,args);
#else
	aCount+=0;	// to prevent unreferenced parameter warning in release builds
	return KErrNone;
#endif
	}


/**
 *  Check that the simulated heap allocation failure event is still available
 *  in the ETel Server thread. This is performed using a peek into the heap
 *  allocation data.
 *
 *  @note This API is only available on debug releases.
 *
 *  @param aResult  Return parameter indicating if the failure event is enabled.
 *                  (e.g. the previous call to __DbgFailNext() is still active
 *                  and has not completed yet).
 *
 *  @capability None
 */
EXPORT_C TInt RTelServer::__DbgFailNextAvailable(TBool& aResult)
	{
#if defined(_DEBUG)
	TPtr8 ptr(REINTERPRET_CAST(TText8*,&aResult),sizeof(TBool),sizeof(TBool));
	TIpcArgs args(&ptr);
	return SendReceive(EEtelDbgFailNextAvail,args);
#else
	aResult = EFalse;
	return KErrNone;
#endif
	}

EXPORT_C TInt RTelServer::SetPriorityClientV2() const
/** Requests the server to allocate priority status to the client that calls it. 

Only one client may have priority status in the system, and the first client 
to ask for it receives it. If another application has been given priority 
client status, the function returns KErrAlreadyExists.

Priority status should be requested early in an application's execution.

The API can be called only by the default phone application.

In case of hardware the API can be called by providing the SID of the default 
phone application in ETel.iby file found at \epoc32\rom\include

A patchable constant KPriorityClientSid is used for defining the SID value.

In ETel.iby file find the following line
patchdata etel.dll@KPriorityClientSid <SID>

For example:
patchdata etel.dll@KPriorityClientSid 0x12345678

In case of emulator the API can be called by providing the SID of the default 
phone application in epoc.ini file found at \epoc32\data

In epoc.ini file find the following line
etel_KPriorityClientSid <SID>

For example:
etel_KPriorityClientSid 0x12345678

@return KErrNone if successful, KErrAlreadyExists if another application has 
been given priority client status. 
@capability NetworkControl
@capability SID
*/
	{
	TIpcArgs args(TIpcArgs::ENothing);
	return SendReceive(EEtelServerSetPriorityClientV2,args);
	}


CPtrHolder* CPtrHolder::NewL(const TInt aSizeOfPtrArray,const TInt aSizeOfPtrCArray)
	{
	CPtrHolder* p = new (ELeave) CPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

CPtrHolder::CPtrHolder()

	{}

CPtrHolder::~CPtrHolder()
	{
	iPtrArray.Close();
	iPtrCArray.Close();
	}

void CPtrHolder::ConstructL(const TInt aSizeOfPtrArray,const TInt aSizeOfPtrCArray)
	{
	TPtr8 ptr(NULL,0);
	TInt i;
	for (i=0;i<aSizeOfPtrArray;i++)
		User::LeaveIfError(iPtrArray.Append(ptr));
	TPtrC8 ptrC(NULL,0);
	for (i=0;i<aSizeOfPtrCArray;i++)
		User::LeaveIfError(iPtrCArray.Append(ptrC));
	}

TPtr8& CPtrHolder::Ptr(const TInt aIndex)
	{
	__ASSERT_ALWAYS(aIndex<iPtrArray.Count(),PanicClient(EEtelPanicIndexOutOfRange));
	return iPtrArray[aIndex];
	}

TPtrC8& CPtrHolder::PtrC(const TInt aIndex)
	{
	__ASSERT_ALWAYS(aIndex<iPtrCArray.Count(),PanicClient(EEtelPanicIndexOutOfRange));
	return iPtrCArray[aIndex];
	}

RCall::TCallParamsPckg* CPtrHolder::CallParamsPckg()
	{
	return NULL;
	}

CCallPtrHolder* CCallPtrHolder::NewL(const TInt aSizeOfPtrArray,const TInt aSizeOfPtrCArray)
	{
	CCallPtrHolder* p = new (ELeave) CCallPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

CCallPtrHolder::CCallPtrHolder()
	:CPtrHolder()
	,iCallParamsPckg(iCallParams)
	{}

CCallPtrHolder::~CCallPtrHolder()
	{}

RCall::TCallParamsPckg* CCallPtrHolder::CallParamsPckg()
	{
	return &iCallParamsPckg;
	}


/***********************************************************************************/
//
// CAsyncRetrieveVariableLengthBufferV2 is the base class used by any class in telephony 
// which retrieves variable length buffers from the Tsy.  Buffer retrieval must be
// performed in two stages.  Each stage has its own separate IPC number, which are
// stored in iIpcPhase1 and iIpcPhase2 respectively.  There is only one cancel IPC
// for both stages.  Classes deriving from CAsyncRetrieveVariableLengthBufferV2 should 
// initialise these values appropriately.  Each request is accompanied by two parameters.
// In phase1 requests the first parameter can be used by derived classes to provide
// the Tsy with information about the sort of buffer being retrieved.  The second
// parameter represents the size of the buffer required to hold the requested data.
// The Tsy should initialise this parameter to contain the appropriate value.  
// The first parameter of the phase 2 request also contains information which the TSY
// can inspect to determine which data to return.  The second parameter references a
// client side buffer into which the TSY should write the requested data.  Once the
// data has been retrieved from the TSY the RestoreList method is called to allow derived
// classes to convert the data in the iResultsBuf into the correct format.  
// Note it is acceptable for the first parameters of either phase to be blank
//
/***********************************************************************************/

/**
Default constructor.
*/
EXPORT_C CAsyncRetrieveVariableLengthBufferV2::CAsyncRetrieveVariableLengthBufferV2() 
: 	CActive(EPriorityStandard), 
	iState(EIdle), 
	iResultsPtr(NULL,0)
	{
	CActiveScheduler::Add(this);
	}

/**
Default destructor.
*/
EXPORT_C CAsyncRetrieveVariableLengthBufferV2::~CAsyncRetrieveVariableLengthBufferV2()
	{
	Cancel();
	delete iResultsBuf;
	}

/**
This method performs the first phase of the retrieval of a variable length buffer
The first phase consists of retrieving the size of buffer to be read

@param aReqStatus returns the result code after the asynchronous call completes
@param aPhase1Request Pointer to data associated with phase 1
@param aPhase2Request Pointer to data associated with phase 2
*/
EXPORT_C void CAsyncRetrieveVariableLengthBufferV2::Start(TRequestStatus& aReqStatus, TDes8* aPhase1Request, TDes8* aPhase2Request)
	{
	__ASSERT_DEBUG(aPhase1Request != NULL, PanicClient(EEtelPanicNullHandle));
	__ASSERT_DEBUG(aPhase2Request != NULL, PanicClient(EEtelPanicNullHandle));

	delete iResultsBuf;
	iResultsBuf = NULL;

	iUserStatus = &aReqStatus;
	*iUserStatus = KRequestPending;

	iPhase1RequestData = aPhase1Request;
	iPhase2RequestData = aPhase2Request;

	Get(iIpcPhase1, iStatus, *iPhase1RequestData, iBufferSize);

	SetActive();
	iState = ERetrievePhase1;
	}


void CAsyncRetrieveVariableLengthBufferV2::StartPhase2L()
/**
This method performs the second phase of the retrieval of a variable length buffer
The second phase consists of allocating a buffer of the appropriate size and then filling
it by reading the data across from the TSY.
*/
	{
	*iUserStatus=KRequestPending;
	
	iResultsBuf = CBufFlat::NewL(1);
	iResultsBuf->ResizeL(iBufferSize());	

	TPtr8 tempPtr(iResultsBuf->Ptr(0));
	tempPtr.FillZ();

	iResultsPtr.Set(tempPtr);

	Get(iIpcPhase2,iStatus,*iPhase2RequestData,iResultsPtr);
	SetActive();
	iState=ERetrievePhase2;
	}

EXPORT_C void CAsyncRetrieveVariableLengthBufferV2::DoCancel()
/**
This method cancels the active phase of the retrieval
It is called from the CActive::Cancel() method. 
*/
	{
	__ASSERT_DEBUG(iState != EIdle, PanicClient(EEtelPanicInvalidRequestType)); // TODO - Is this really needed?

	if (iState == ERetrievePhase1)
		{
		CancelReq(iIpcCancel, iIpcPhase1);
		}
	else if (iState == ERetrievePhase2)
		{
		CancelReq(iIpcCancel, iIpcPhase2);
		}

	if (iUserStatus != NULL)
		{
		// have to complete the user status too
		*iUserStatus = KErrCancel;
		User::RequestComplete(iUserStatus, iUserStatus->Int()); 
		iUserStatus = NULL;
		}
	iState = EIdle;
	}

EXPORT_C void CAsyncRetrieveVariableLengthBufferV2::RunL()
/**
This method processes the completion of each asynchronous phase of the retrieval
If phase 1 is now complete, it starts phase 2
If phase 2 is now complete, it complete's the client's request. 
*/
	{
	if (iStatus.Int() != KErrNone)
		{
		*iUserStatus = iStatus.Int();
		User::RequestComplete(iUserStatus, iUserStatus->Int());
		iUserStatus = NULL;
		iState = EIdle;
		}
	else
		{
		switch(iState)
			{
		case ERetrievePhase1:
				{
				TRAPD(ret, StartPhase2L());
				if (ret != KErrNone)
					{
					*iUserStatus = ret;
					User::RequestComplete(iUserStatus, iUserStatus->Int());
					iUserStatus = NULL;
					iState = EIdle;		
					}
				break;	
				}

		case ERetrievePhase2:
				{
				TRAPD(ret, RestoreListL());
				*iUserStatus = ret;
				User::RequestComplete(iUserStatus, iUserStatus->Int());
				iUserStatus = NULL;
				iState = EIdle;
				break;	
				}

		case EIdle:
		default:
				{
				*iUserStatus = KErrGeneral;
				User::RequestComplete(iUserStatus, iUserStatus->Int());
				iUserStatus = NULL;
				iState = EIdle;
				break;
				}
			}
		}
	}

/**
This method checks whether the AO is currently in use or not
If it is in use, then the client's request is completed with KErrInUse

@return TBool A boolean indicating whether the AO is in use or not 
*/
EXPORT_C TBool CAsyncRetrieveVariableLengthBufferV2::CompleteIfInUse(TRequestStatus& aReqStatus)
	{
	TBool retVal = EFalse;
	if (iState != EIdle)
		{
		TRequestStatus* rs = &aReqStatus;
		retVal = ETrue;
		User::RequestComplete(rs, KErrInUse);
		}
	return retVal;
	}

/**
This method deallocates the buffer used to hold the retrieved data
*/
EXPORT_C void CAsyncRetrieveVariableLengthBufferV2::FreeBuffer()
	{
	delete iResultsBuf;
	iResultsBuf = NULL;
	iResultsPtr.Set(NULL, 0, 0);
	}

/**
Does nothing. Should be overriden in concrete derived classes.
*/
EXPORT_C void CAsyncRetrieveVariableLengthBufferV2::RestoreListL()
	{
	// NOP
	}


EXPORT_C TTlvStructBase::TTlvStructBase(TPtr8& aPtr,TUint8 aFreeSpaceChar):iPtr(aPtr),iCursorPos(0),iFreeSpacePos(aPtr.Length()),iFreeSpaceChar(aFreeSpaceChar)
	{	
	TPtr8 tempPtr(iPtr);
	tempPtr.SetLength(iPtr.MaxLength());	
	TPtr8 ptr(tempPtr.MidTPtr(iFreeSpacePos,tempPtr.MaxLength()-iFreeSpacePos));
	ptr.Fill(iFreeSpaceChar);
	}
			
EXPORT_C void TTlvStructBase::ResetCursorPos()
	{
	iCursorPos=0;
	}


EXPORT_C TInt TTlvStructBase::AppendItemL(MTlvItemIdType& aId, MTlvItemDataLengthType& aDataLength,const TPtr8& aData)
	{		
	TPtr8 tempPtr(iPtr);
	tempPtr.SetLength(iPtr.MaxLength());
	
	TUint itemLen=aId.SerializedLength() + aDataLength.SerializedLength() +aData.Length();
	if((TInt)(tempPtr.MaxLength()-itemLen) < (TInt)iFreeSpacePos)
		return KErrOverflow;
	TPtr8 ptr(tempPtr.MidTPtr(iFreeSpacePos,aId.SerializedLength()));
	aId.ExternalizeL(ptr);	
	ptr.Set(tempPtr.MidTPtr(iFreeSpacePos+aId.SerializedLength(),aDataLength.SerializedLength()));
	
	aDataLength.SetDataLength(aData.Length());
	
	aDataLength.ExternalizeL(ptr);	
	ptr.Set(tempPtr.MidTPtr(iFreeSpacePos+aId.SerializedLength()+aDataLength.SerializedLength()));
	Mem::Copy((TUint8* )(ptr.Ptr()),aData.Ptr(),aData.Length());	
	iFreeSpacePos+=itemLen;
	iPtr.SetLength(iFreeSpacePos);	
	return KErrNone;
	}
	
EXPORT_C TInt TTlvStructBase::RemoveNextItemL(MTlvItemIdType& aIdToRemove,MTlvItemIdType& aId, MTlvItemDataLengthType& aDataLength)
	{
	TPtr8 tempPtr(iPtr);
	tempPtr.SetLength(iPtr.MaxLength());
	
	TBool found(EFalse);
	TUint pos=iCursorPos;
	TPtr8 ptr(0,0);
	while(!found && pos + aId.SerializedLength()+ aDataLength.SerializedLength() <= iFreeSpacePos)
		{	
		ptr.Set(tempPtr.MidTPtr(pos,aId.SerializedLength()));
		aId.InternalizeL(ptr);			
		ptr.Set(tempPtr.MidTPtr(pos+aId.SerializedLength(),aDataLength.SerializedLength()));	
		aDataLength.InternalizeL(ptr);	
		if(aIdToRemove.IsEqual(aId))
			{								
			TUint itemLen=aId.SerializedLength()+aDataLength.SerializedLength()+aDataLength.DataLength();			
			if(pos+itemLen>tempPtr.Length())
				return KErrNotFound;								
			found=ETrue;			
			TUint nextItemPos=pos+itemLen;
			TUint8* dest= (TUint8* )(tempPtr.Ptr()+pos);
			TUint8* src= (TUint8* )(dest+itemLen);
			while(nextItemPos<iFreeSpacePos)
				{
				*dest++=*src++;
				nextItemPos++;
				}
			iFreeSpacePos-=itemLen;
			TPtr8 ptr(tempPtr.MidTPtr(iFreeSpacePos,tempPtr.MaxLength()-iFreeSpacePos));
			ptr.Fill(iFreeSpaceChar);
			}
		else
			{
			pos+=aId.SerializedLength()+aDataLength.SerializedLength()+aDataLength.DataLength();
			}
		}
	if(found)
		{
		iPtr.SetLength(iFreeSpacePos);	
	 	return KErrNone;
	 	}
	return KErrNotFound;
	}

EXPORT_C TInt TTlvStructBase::AnyNextItemL(MTlvItemIdType& aIdFound,TPtr8& aData,MTlvItemIdType& aId, MTlvItemDataLengthType& aDataLength)
	{	
	if(iCursorPos>=iFreeSpacePos)
		return KErrNotFound;
	
	if(iCursorPos+aId.SerializedLength() + aDataLength.SerializedLength()> iFreeSpacePos)
		return KErrNotFound;
	TPtr8 ptr(iPtr.MidTPtr(iCursorPos,aId.SerializedLength()));	
	aIdFound.InternalizeL(ptr);			
	
	ptr.Set(iPtr.MidTPtr(iCursorPos+aId.SerializedLength(),aDataLength.SerializedLength()));	
	aDataLength.InternalizeL(ptr);					
	
	if(iCursorPos+aId.SerializedLength() + aDataLength.SerializedLength() + aDataLength.DataLength() > iFreeSpacePos)
		return KErrNotFound;
	
	ptr.Set(iPtr.MidTPtr(iCursorPos+aId.SerializedLength()+aDataLength.SerializedLength(),aDataLength.DataLength()));	
	aData.Set(ptr);		
	iCursorPos+=aId.SerializedLength()+aDataLength.SerializedLength()+aDataLength.DataLength();				
	return KErrNone;
	}	

EXPORT_C TInt TTlvStructBase::NextItemL(const MTlvItemIdType& aWantedId,TPtr8& aData,MTlvItemIdType& aId, MTlvItemDataLengthType& aDataLength)
	{
	TBool found(EFalse);	
	TUint pos=iCursorPos;
	TPtr8 ptr(0,0);	
	while(!found && pos + aId.SerializedLength()+aDataLength.SerializedLength() <= iFreeSpacePos)
		{			
		ptr.Set(iPtr.MidTPtr(pos,aId.SerializedLength()));	
		aId.InternalizeL(ptr);			
		ptr.Set(iPtr.MidTPtr(pos+aId.SerializedLength(),aDataLength.SerializedLength()));	
		aDataLength.InternalizeL(ptr);	
		if(aId.IsEqual(aWantedId))
			{		
			if(pos+aId.SerializedLength()+aDataLength.SerializedLength()+aDataLength.DataLength()>iFreeSpacePos)					
				return KErrNotFound;
			ptr.Set(iPtr.MidTPtr(pos+aId.SerializedLength()+aDataLength.SerializedLength(),aDataLength.DataLength()));	
			aData.Set(ptr);	
			found=ETrue;
			iCursorPos=pos+aId.SerializedLength()+aDataLength.SerializedLength()+aDataLength.DataLength();
			}
		else
			{
			pos+=aId.SerializedLength()+aDataLength.SerializedLength()+aDataLength.DataLength();
			}
		};
	if(found)
		{	
	 	return KErrNone;
	 	}
	return KErrNotFound;
	}
