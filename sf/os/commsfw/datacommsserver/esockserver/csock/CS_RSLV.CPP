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

#include <es_sock.h>
#include "CS_STD.H"
#include <comms-infras/sockmes.h>
#include "es_flog.h"

EXPORT_C TInt RHostResolver::Open(RSocketServ &aSocketServer,TUint anAddrFamily,TUint aProtocol)
//
// set up the sub session
//
/** Initialises a name resolution service provided by a particular protocol.

This must be called before other object functions are used.

@param aSocketServer The socket server session 
@param anAddrFamily A constant identifying the protocol family 
@param aProtocol A constant that identifies the protocol that provides the 
name resolution service 
@return KErrNone if successful otherwise another of the system-wide error codes. */
	{
	RSessionBase &s=aSocketServer;
	LOG( ESockLog::Printf(_L8("RHostResolver %08x:\tOpen() tid %d"), this, (TUint)RThread().Id()));
	return CreateSubSession(s,EHRCreate,TIpcArgs(anAddrFamily,aProtocol));
	}

EXPORT_C TInt RHostResolver::Open(RSocketServ &aSocketServer,TUint anAddrFamily,TUint aProtocol,RConnection& aConnection)
/** Initialises a name resolution service provided by a particular protocol, and associates the service with the same 
interface as an existing RConnection instance.

This must be called before other object functions are used.

Note that the association is instantaneous, in that the Host Resolver is associated with the
interface that the RConnection is associated with at the present time.  This association
terminates when the underlying interface goes down.

@param aServer The socket server session.
@param anAddrFamily A constant identifying the protocol family.
@param aProtocol A constant that identifies the protocol that provides the name resolution service.
@param aConnection Existing RConnection whose interface this Host Resolver will be associated with.
@return KErrNone if successful otherwise another of the system-wide error codes.
 */
	{
	// passing an RConnection which doesn't belong to the same session is a serious programming error, hence panic.
	if (!aConnection.SameSession(aSocketServer.Handle()))
		Panic(EBadRConnection);

	RSessionBase &s=aSocketServer;
	LOG( ESockLog::Printf(_L8("RHostResolver %08x:\tOpen(RConnection %08x) tid %d"), this, &aConnection, (TUint)RThread().Id()));
	return CreateSubSession(s,EHRCreateWithConnection,TIpcArgs(anAddrFamily, aProtocol, aConnection.SubSessionHandle()));
	}

EXPORT_C void RHostResolver::GetByName(const TDesC &aName,TNameEntry &aResult,TRequestStatus &aStatus)
//
// Get a host by name
//
/** Resolves a machine name to a TSockAddress asynchronously. 

The server will fill in the supplied TNameEntry.

@param aName The name to resolve. The format of the name and any wild-cards 
supported, along with any separators in hierarchical name systems, are protocol 
specific 
@param aResult On return, the result of the name resolution. If more than one 
result is allowed by the protocol, the client can call Next() to find any 
further results. 
@param aStatus Indicates asynchronous operation and, on completion, contains 
an error code: see the system-wide error codes. */
	{
	LOG
	(
		const TInt KBufSize = 32;
		TPtrC limitedName(aName.Ptr(), aName.Length() > KBufSize ? KBufSize : aName.Length());
		TBuf8<KBufSize> name;
		name.Copy(limitedName);

		ESockLog::Printf(_L8("RHostResolver %08x:\tGetByName(\"%S\") tid %d"), this, &name, (TUint)RThread().Id());
	);
	SendReceive(EHRGetByName,TIpcArgs(&aName,&aResult),aStatus);
	}

EXPORT_C TInt RHostResolver::GetByName(const TDesC &aName,TNameEntry &aResult)
//
// Get a host by name
//
/** Resolves a machine name to a TSockAddress.

The server will fill in the supplied TNameEntry.

@param aName The name to resolve. The format of the name and any wild-cards 
supported, along with any separators in hierarchical name systems, are protocol 
specific 
@param aResult On return, the result of the name resolution. If more than one 
result is allowed by the protocol, the client can call Next() to find any 
further results.
@return KErrNone if successful otherwise another of the system-wide error codes.
@capability Dependent on the type of connection so deferred to PRT */
	{

	TRequestStatus stat;
	GetByName(aName,aResult,stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C void RHostResolver::Next(TNameEntry &aResult,TRequestStatus &aStatus)
/**
Get the next response if there is more than one response for a given host name (alias list)

Returns the next answer asynchronously, where there is more than one response
for a given host name. 

For some protocols, GetByName() and GetByAddress() may find more than one answer, 
for example if aliases are allowed. 

@param aResult Returns the next result of the name resolution. 
@param aStatus Indicates asynchronous operation and on completion contains 
an error code: see the system-wide error codes. */
	{
	SendReceive(EHRNext,TIpcArgs(0,&aResult),aStatus); //Question - is the change is right?
	}

EXPORT_C TInt RHostResolver::Next(TNameEntry &aResult)
/**
Get the next response if there is more than one response for a given host name (alias list)



Returns the next answer where there is more than one response for a given host name. 

For some protocols, GetByName() and GetByAddress() may find more than one answer, 
for example if aliases are allowed. 

@param aResult Returns the next result of the name resolution. 
@return KErrNone if successful otherwise another of the system-wide error codes. */
	{
	
	TRequestStatus stat;
	Next(aResult,stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C void RHostResolver::GetByAddress(const TSockAddr &anAddr,TNameEntry &aResult,TRequestStatus &aStatus)
/**
Get host by address (as get by name)

Gets the name of a host from its address asynchronously


@param anAddr The address to use 
@param aResult On return, the result of the query. If more than one result 
is allowed by the protocol, the client can call Next() to find any further 
results. 
@param aStatus Indicates asynchronous operation and on completion contains 
an error code: see the system-wide error codes.
@capability Dependent on the type of connection so deferred to PRT  */
	{
	SendReceive(EHRGetByAddress,TIpcArgs(&anAddr,&aResult),aStatus);
	}

EXPORT_C TInt RHostResolver::GetByAddress(const TSockAddr &anAddr,TNameEntry &aResult)
/**
Get host by address (as get by name)

Gets the name of a host from its address 

@param anAddr The address to use 
@param aResult Returns the result of the query. If more than one result is 
allowed by the protocol, the client can call Next() to find any further results. 

@return KErrNone if successful otherwise another of the system-wide error codes.
@capability Dependent on the type of connection so deferred to PRT */
	{
	
	TRequestStatus stat;
	GetByAddress(anAddr,aResult,stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C TInt RHostResolver::GetHostName(TDes &aName)
/**


Gets the name of the local host.

Note that with some protocols the name of the local host is not necessarily 
known at all times. In some cases, a preceding call to SetHostName() must 
have been made.

@param aName Returns the result of the query. The buffer passed in should 
have a minimum length of 256 characters, otherwise a panic may occur: you 
can use a parameter of the THostName type. 
@return KErrNone if successful otherwise another of the system-wide error codes.
@capability Dependent on the type of connection so deferred to PRT */
	{
	return SendReceive(EHRGetHostName,TIpcArgs(&aName));
	}

EXPORT_C void RHostResolver::GetHostName(TDes &aName,TRequestStatus &aStatus)
/** Gets the name of the local host asynchronously.

Note that with some protocols the name of the local host is not necessarily 
known at all times. In some cases, a preceding call to SetHostName() must 
have been made.

@param aName Returns the result of the query. The buffer passed in should 
have a minimum length of 256 characters, otherwise a panic may occur: you 
can use a parameter of the THostName type. 
@param aStatus Indicates asynchronous operation and on completion contains 
an error code: see the system-wide error codes.
@capability Dependent on the type of connection so deferred to PRT */
	{
	SendReceive(EHRGetHostName,TIpcArgs(&aName),aStatus);
	}

EXPORT_C TInt RHostResolver::SetHostName(const TDesC &aName)
/** Sets the name of the local host.
@param aName The local host name.
@return KErrNone if successful, otherwise another of the system-wide
error codes. 
@capability NetworkControl Service restricting and stored at protocol level
*/
	{
	return SendReceive(EHRSetHostName,TIpcArgs(&aName));
	}


EXPORT_C TInt RHostResolver::SetOpt(TUint anOptionName,TUint anOptionLevel,const TDesC8& anOption)
	{
	LOG( ESockLog::Printf(_L8("RHostResolver %08x:\tSetOpt() tid %d"), this, (TUint)RThread().Id()));
	return SendReceive(EHRSetOpt,TIpcArgs(anOptionName,&anOption,anOptionLevel));
	}


EXPORT_C void RHostResolver::Cancel()
/** Cancels any outstanding asynchronous calls, which will return with error code 
KErrCancel. */
	{
	SendReceive(EHRCancel,TIpcArgs());
	}

EXPORT_C void  RHostResolver::Close()
/**


Closes a name resolution service. If a service has been opened using Open(), 
then it should be closed using Close(). This will ensure all associated resources 
are released. */
	{
	LOG( ESockLog::Printf(_L8("RHostResolver %08x:\tClose() tid %d"), this, (TUint)RThread().Id()));

	CloseSubSession(EHRClose);
	}



EXPORT_C void RHostResolver::Query(const TDesC8& aQuery, TDes8& aResult, TRequestStatus& aStatus)
/**
*   Protocol - independent query. Asynchronous version.
*
*   @param  aQuery      contains query data. The concrete type of the data depends on protocol.
*                       e.g. for tcpip it will imply DNS query.
*   @param  aResult     Descriptor that will receive data. The concrete meaning of this data depends on protocol. 
*   @param  aStatus     indicates asynchronous operation, on completion contains a system-wide error code
*	@capability 		Dependent on the type of connection so deferred to PRT 
*/
{
    LOG( ESockLog::Printf(_L8("RHostResolver %08x:\tQuery() tid %d"), this, (TUint)RThread().Id()));
    
	SendReceive(EHrQuery,TIpcArgs(&aQuery, &aResult),aStatus);
}

EXPORT_C TInt RHostResolver::Query(const TDesC8& aQuery, TDes8& aResult)
/**

Protocol - independent query. Synchronous version.

@param  aQuery      contains query data. The concrete type of the data depends on protocol.
                   e.g. for tcpip it will imply DNS query.
@param  aResult     Descriptor that will receive data. The concrete meaning of this data depends on protocol. 
@return             system-wide error code.
@capability 		Dependent on the type of connection so deferred to PRT 
*/
	{
	TRequestStatus stat;
	Query(aQuery,aResult,stat);
	User::WaitForRequest(stat);

	return stat.Int();
	}

EXPORT_C void RHostResolver::QueryGetNext(TDes8& aResult, TRequestStatus& aStatus)
/**

Get the next query result. For some queries there can be more than one results. Asynchronous version.

@param  aResult     Descriptor that will receive data. The concrete meaning of this data depends on protocol. 
@param  aStatus     indicates asynchronous operation, on completion contains a system-wide error code.
                   In particular KErrNotFound means that there is no more information from resolver.
*/
{
    LOG( ESockLog::Printf(_L8("RHostResolver %08x:\tQueryGetNext() tid %d"), this, (TUint)RThread().Id()));

	SendReceive(EHrQueryNext,TIpcArgs(&aResult),aStatus);

}

EXPORT_C TInt RHostResolver::QueryGetNext(TDes8& aResult)
/**

Get the next query result. For some queries there can be more than one results. Synchronous version.

@param  aResult     Descriptor that will receive data. The concrete meaning of this data depends on protocol. 
@return             system-wide error code. In particular KErrNotFound means that there is no more information from resolver.
*/
	{
	TRequestStatus stat;
	QueryGetNext(aResult,stat);
	User::WaitForRequest(stat);

	return stat.Int();
	}

EXPORT_C TInt RServiceResolver::Open(RSocketServ &aSocketServer,TUint anAddrFamily,TUint aSocketType,TUint aProtocol)
/**
set up the sub session - unlike name resolvers and net databases, services can be socket type specific.
Opens a service resolver service. 

Unlike name resolvers (RHostResolver) and net databases (RNetDatabase), services can be socket type specific.

@param aSocketServer The socket server session.
@param anAddrFamily A constant identifying the protocol family.
@param aSocketType A constant that identifies the socket type.
@param aProtocol A constant that identifies the protocol that provides the 
service.
@return KErrNone if successful otherwise another of the system-wide error codes. */
	{
	RSessionBase &s=aSocketServer;
	return CreateSubSession(s,ESRCreate,TIpcArgs(anAddrFamily,aSocketType,aProtocol));
	}

EXPORT_C void RServiceResolver::GetByName(const TDesC &aName,TPortNum &aPort,TRequestStatus &aStatus)
/** Gets a service by name asynchronously.

@param aName Name of the service to get.
@param aPort On completion, the port associated with the service.
@param aStatus On completion, KErrNone if successful otherwise another of the 
system-wide error codes. */
	{
	SendReceive(ESRGetByName,TIpcArgs(&aName,&aPort),aStatus);
	}

EXPORT_C TInt RServiceResolver::GetByName(const TDesC &aName,TPortNum &aPort)
/**
Get a service by name.

@param aName Name of the service to get.
@param aPort On return, the port associated with the service.
@return KErrNone if successful otherwise another of the system-wide error codes. */
	{
	
	TRequestStatus stat;
	GetByName(aName,aPort,stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C void RServiceResolver::GetByNumber(const TUint aPort,TDes &aName,TRequestStatus &aStatus)
/** Gets the name of the service asynchronously.

@param aPort Port of the service.
@param aName On completion, the name of the service.
@param aStatus On completion, KErrNone if successful otherwise another of the 
system-wide error codes. */
	{
	SendReceive(ESRGetByNumber,TIpcArgs(&aName,aPort),aStatus);
	}

EXPORT_C TInt RServiceResolver::GetByNumber(const TUint aPort,TDes &aName)
/** Gets the name of the service

@param aPort Port of the service.
@param aName On return, the name of the service.
@return KErrNone if successful otherwise another of the system-wide error codes. */
	{

	TRequestStatus stat;
	GetByNumber(aPort,aName,stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C void RServiceResolver::RegisterService(const TDesC &aName,const TUint &aPort,TRequestStatus &aStatus)
/** Registers a new service asynchronously.

@param aName Name of the service.
@param aPort Port of the service.
@param aStatus On completion, KErrNone if successful otherwise another of the 
system-wide error codes. 
@capability NetworkControl To protect against dangerous new services being added, which may steal legimate services resources */
	{
	SendReceive(ESRRegisterService,TIpcArgs(&aName,aPort),aStatus);
	}

EXPORT_C TInt RServiceResolver::RegisterService(const TDesC &aName,const TUint &aPort)
/** Registers a new service.

@param aName Name of the service.
@param aPort Port of the service.
@return KErrNone if successful, otherwise another of the system-wide error codes.
@capability NetworkControl To protect against dangerous new services being added, which may steal legimate services resources */
	{

	TRequestStatus stat;
	RegisterService(aName,aPort,stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C void RServiceResolver::RemoveService(const TDesC &aName,const TUint &aPort,TRequestStatus &aStatus)
/** Removes a service asynchronously.

@param aName Name of the service to remove.
@param aPort Port of the service to remove.
@param aStatus On completion, KErrNone if successful otherwise another of the 
system-wide error codes. 
@capability NetworkControl Ensure that only privileged apps can remove information from the service resolver */
	{
	SendReceive(ESRRemoveService,TIpcArgs(&aName,aPort),aStatus);
	}


EXPORT_C TInt RServiceResolver::RemoveService(const TDesC &aName,const TUint &aPort)
/** Removes a service.

@param aName Name of the service.
@param aPort Port of the service.
@return KErrNone if successful otherwise another of the system-wide error codes.
@capability NetworkControl Ensure that only privileged apps can remove information from the service resolver */
	{

	TRequestStatus stat;
	RemoveService(aName,aPort,stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C void RServiceResolver::Cancel()
/** Cancels any pending request. */
	{
	SendReceive(ESRCancel,TIpcArgs());
	}

EXPORT_C void RServiceResolver::Close()
/**
Closes a service resolver service

If a service has been opened using Open(), 
then it should be closed using Close(). This will ensure all associated resources 
are released. */
	{

	CloseSubSession(ESRClose);
	}

EXPORT_C TInt RNetDatabase::Open(RSocketServ &aSocketServer,TUint anAddrFamily,TUint aProtocol)
//
// set up the sub session
//
/** Initialises a database access service provided by a particular protocol. It 
must be called before other object functions are used.

@param aSocketServer The socket server session 
@param anAddrFamily A constant identifying the protocol family 
@param aProtocol A constant that identifies the protocol that provides the 
database access service 
@return KErrNone if successful otherwise another of the system-wide error codes. */
	{
	RSessionBase &s=aSocketServer;
	return CreateSubSession(s,ENDCreate,TIpcArgs(anAddrFamily,aProtocol));
	}

EXPORT_C void RNetDatabase::Query(const TDesC8 &aQuery,TDes8 &aResult,TRequestStatus &aStat)
/** Makes a query to the database asynchronously.

@param aQuery The query to perform 
@param aResult The result of the query 
@param aStat Indicates asynchronous operation and on completion contains an 
error code: see the system-wide error codes.
@capability Dependent on the type of connection so deferred to PRT */
	{
	SendReceive(ENDQuery,TIpcArgs(&aQuery,Max(aQuery.Length(),aResult.Length()),&aResult),aStat);
	}

EXPORT_C TInt RNetDatabase::Query(const TDesC8 &aQuery,TDes8 &aResult)
//
// unfortunately this API must be so general.
//
/** Makes a query to the database.

@param aQuery The query to perform 
@param aResult The result of the query 
@return KErrNone if successful otherwise another of the system-wide error codes.
@capability Dependent on the type of connection so deferred to PRT */
	{

	TRequestStatus stat;
	Query(aQuery,aResult,stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C void RNetDatabase::Add(const TDesC8& anItem,TRequestStatus& aStat)
/** Adds a record to the database asynchronously.

@param anItem The record to add 
@param aStat Indicates asynchronous operation and on completion contains an 
error code: see the system-wide error codes.
@capability Dependent on the type of connection so deferred to PRT */
	{
	SendReceive(ENDAdd,TIpcArgs(&anItem,anItem.Length()),aStat);
	}

EXPORT_C TInt RNetDatabase::Add(const TDesC8& anItem)
/** Adds a record to the database.

@param anItem The record to add 
@return KErrNone if successful otherwise another of the system-wide error codes.
@capability Dependent on the type of connection so deferred to PRT  */
	{
	TRequestStatus stat;
	Add(anItem,stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C void RNetDatabase::Remove(const TDesC8& anItem,TRequestStatus& aStat)
/** Removes a record from the database asynchronously.

@param anItem The record to remove 
@param aStat Indicates asynchronous operation and on completion contains an 
error code: see the system-wide error codes.
@capability Dependent on the type of connection so deferred to PRT  */
	{
	SendReceive(ENDRemove,TIpcArgs(&anItem,anItem.Length()),aStat);
	}

EXPORT_C TInt RNetDatabase::Remove(const TDesC8& anItem)
/** Removes a record from the database.

@param anItem The record to remove 
@return KErrNone if successful otherwise another of the system-wide error codes.
@capability Dependent on the type of connection so deferred to PRT  */
	{
	TRequestStatus stat;
	Remove(anItem,stat);
	User::WaitForRequest(stat);
	return stat.Int();
	}

EXPORT_C void RNetDatabase::Cancel()
/** Cancels any outstanding asynchronous calls, which will return with error code 
KErrCancel. */
	{
	SendReceive(ENDCancel,TIpcArgs());
	}

EXPORT_C void RNetDatabase::Close()
/** Closes a database access service. If a service has been opened using Open(), 
then it should be closed using Close(). This will ensure all associated resources 
are released. */
	{
	CloseSubSession(ENDClose);
	}

