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
 @file SS_PROT.CPP
*/

#include <ss_std.h>
#include <ss_glob.h>
#include <comms-infras/ss_roles.h>
#include <comms-infras/ss_log.h>
#include <ss_protprov.h>
#include <ss_sock.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockS_PROT, "ESockSSockS_PROT");
#endif

using namespace ESock;

NONSHARABLE_CLASS(CLibUnloader) : public CAsyncOneShot
/**
@internalComponent
*/
	{
public:
	static CLibUnloader* NewL(RLibrary &aLibrary);
	inline void Unload();
protected:
	CLibUnloader();
	~CLibUnloader();
	virtual void RunL();
private:
	RLibrary iLib;
	};

CLibUnloader::CLibUnloader()
/**
Constructor
*/
	:CAsyncOneShot(ECAsyncDeferredPriority)
	{
	__DECLARE_NAME(_S("CLibUnloader"));
	}

CLibUnloader* CLibUnloader::NewL(RLibrary &aLibrary)
	{
	CLibUnloader* u=new(ELeave)CLibUnloader;
	u->iLib=aLibrary;
	return u;
	}

inline void CLibUnloader::Unload()
	{
	Call();
	}

CLibUnloader::~CLibUnloader()
	{
	Cancel();
	iLib.Close();
	}

void CLibUnloader::RunL()
	{
	delete this;
	}

EXPORT_C CServProviderBase::CServProviderBase()
	{
	}

EXPORT_C CServProviderBase::~CServProviderBase()
	{
	delete iV1ShimDataOut;
	delete iV1ShimDataIn;
	}

EXPORT_C void CServProviderBase::SetNotify(MSocketNotify* aSocket)
/**
Set notification for transport services to a single protocol

@param aSocket, notify the socket server that various events have occurred
*/
	{
	iSocket = aSocket;
	}

EXPORT_C void CServProviderBase::SetSockType(TUint aSockType)
	{
	iSockType = aSockType;
	}

EXPORT_C TUint CServProviderBase::SockType() const
	{
	return iSockType;
	}

EXPORT_C void CServProviderBase::JoinSubConnectionL(CSubConnectionProviderBase& /*aSubConnProvider*/)
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CServProviderBase::LeaveSubConnection(CSubConnectionProviderBase& /*aSubConnProvider*/)
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockS_PROT, 1));
	}

EXPORT_C TUint CServProviderBase::Write(const TDesC8& /*aDesc*/, TUint /*options*/, TSockAddr* /*anAddr*/)
	{
	Panic(EProviderIncomplete);
	return 0;
	}

EXPORT_C void CServProviderBase::GetData(TDes8& /*aDesc*/, TUint /*options*/, TSockAddr* /*anAddr*/)
	{
	Panic(EProviderIncomplete);
	}

EXPORT_C TInt CServProviderBase::Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr)
	{
	// This version only gets called when the provider neglects to provide a better v1.5
	// implementation; efficiency is not paramount
	if(!iV1ShimDataOut)
		{
		iV1ShimDataOut = HBufC8::New(aData.Length());
		if(!iV1ShimDataOut)
			{
			return KErrNoMemory;
			}
		}
	else if(iV1ShimDataOut->Size() < aData.Length())
		{
		// We need extra room for data. As ReAlloc returns NULL if fails, we
		// need to temporary save the previous storage in order to clear it
		// in the case of ReAlloc failure.
		HBufC8* aTempPtr = iV1ShimDataOut;
		iV1ShimDataOut = iV1ShimDataOut->ReAlloc(aData.Length());
		if(!iV1ShimDataOut)
			{
			delete aTempPtr;
			return KErrNoMemory;
			}
		}

	TPtr8 des = iV1ShimDataOut->Des();
	des.SetLength(aData.Length());
	aData.CopyOut(des, 0);
	TInt ret = Write(des, aOptions, anAddr);
	if(ret > 0)
		{
		if(iSockType == KSockStream)
			{
			aData.TrimStart(ret);
			}
		else
			{
			aData.Free();
			}
		}
	return ret;
	}

EXPORT_C TInt CServProviderBase::GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr)
	{
	// This version only gets called when the provider neglects to provide a better v1.5
	// implementation; efficiency is not paramount

	// With datagram continuation reads it is important that the provider returns the whole datagram even when
	// the client's transfer buffer is too small for this to be done as a single read. Because legacy protocols
	// may respect the length field ESOCK sets a high-order bit to make it look like a large buffer. Continuation
	// reads aren't supported with this descriptor interface (due to the need to allocate an arbitarily large
	// buffer) so here the bit is reset
	aLength &= ~KGetDataWholeDatagram;

	if(!iV1ShimDataIn)
		{
		iV1ShimDataIn = HBufC8::New(aLength);
		if(!iV1ShimDataIn)
			{
			return KErrNoMemory;
			}
		}
	else if(iV1ShimDataIn->Size() < TInt(aLength))
		{
		// We need extra room for data. As ReAlloc returns NULL if fails, we
		// need to temporary save the previous storage in order to clear it
		// in the case of ReAlloc failure.
		HBufC8* aTempPtr = iV1ShimDataIn;
		iV1ShimDataIn = iV1ShimDataIn->ReAlloc(aLength);
		if(!iV1ShimDataIn)
			{
			delete aTempPtr;
			return KErrNoMemory;
			}
		}

	TInt extraNeeded = aLength - aData.Length();
	if(extraNeeded > 0)
		{
		TInt err = aData.Append(extraNeeded);
		if(err != KErrNone)
			{
			return KErrNoMBufs;
			}
		}

	TPtr8 des = iV1ShimDataIn->Des();
	des.SetLength(aLength);
	GetData(des, aOptions, anAddr);
	aData.CopyIn(des, 0);
	// Handle protocol reneging (eg when read being cancelled)
	if(des.Length() < TInt(aLength))
		{
		aData.TrimEnd(des.Length());
		}

	if(iSockType == KSockStream)
		{
		return des.Length();
		}
	else
		{
		return 1;	// datagrams are counted as "atoms" not bytes
		}
	}


EXPORT_C CServProviderBase * CProtocolBase::NewSAPL(TUint /*aProtocol*/)
//
// Default socket creation
//
/** Creates a new service access point.

If the protocol cannot create the CServProviderBase for any reason, it should leave.

@param aProtocol Protocol.
*/
	{

	Fault(EOddSock);
	return (CServProviderBase*)NULL; //lint !e527	// unreachable
	}

EXPORT_C TInt CHostResolvProvdBase::SetOption(TUint /*level*/, TUint /*name*/, const TDesC8& /*anOption*/)
/**
Default Set Option for Host Resolver

Sets options for the protocol on behalf of socket server clients

@param level, The level in the stack the option request is ‘aimed’ at
@param name, The name of the option
@param anOption, Descriptor to hold the option data
@return KErrNotSupported if the option is not supported.
*/
	{
	return KErrNotSupported;
	}


EXPORT_C void CHostResolvProvdBase::Query(const TDesC8& /*aQryBuf*/, TDes8& /*aResBuf*/, TInt /*aCounter*/)
/**
* Make a query to the protocol.
* @param    aQryBuf  descriptor representing query data.
* @param    aResBuf  descriptor representing query response data.
* @param    aCounter query sequential number counter. From the client's point of view it will be 0 for "Query" call
*           and increased by 1 for each "QueryGetNext" call.
* @note     for aCounter > 0 data in aQryBuf are not supposed to be used.
*/
	{
    if(iNotify)
    	{
        iNotify->QueryComplete(KErrNotSupported);
    	}
	}


EXPORT_C TInt CServProviderBase::SecurityCheck(MProvdSecurityChecker* /*aChecker*/)
/**
Ask the socket provider to perform a security policy check on the originating process
(default implementation).

Called when a socket is opened or transferred, and after the PRT presents a new accepting
socket to ESOCK.

@param aChecker class instance that socket provider can use to check capabilities of originating
process.
@return KErrNone if check passes, KErrPermissionDenied if check fails, else a system-wide error code.
*/
	{
	// ********************************************************************
	// NOTE: KErrNone for now, but this should be changed to KErrNotSupported to
	// turn on strict checking and fail PRTs that have not been secured.
	// ********************************************************************
	return KErrNone;
	}

EXPORT_C TInt CResolverProvdBase::SecurityCheck(MProvdSecurityChecker* /*aChecker*/)
/**
Ask the resolver provider to perform a security policy check on the originating process
(default implementation).

Called when a resolver is opened.

@param aChecker class instance that resolver can use to check capabilities of originating process.
@return KErrNone if check passes, KErrPermissionDenied if check fails, else a system-wide error code.
*/
	{
	// ********************************************************************
	// NOTE: KErrNone for now, but this should be changed to KErrNotSupported to
	// turn on strict checking and fail PRTs that have not been secured.
	// ********************************************************************
	return KErrNone;
	}
	


EXPORT_C void CProtocolBase::StartSending(CProtocolBase* /*aProtocol*/)
//
// Default StartSending - ignored
//
/** Indicates that data can now be sent. Implementations can call bound higher-level
protocols and service access points in turn.

With Send(), this function forms a flow control pair. A zero return from a
Send() should flow control off the calling protocol. StartSending() acts as
a flow control on.

@note This implementation does nothing.

@param aProtocol The calling protocol */
	{
	}

EXPORT_C void CProtocolBase::InitL(TDesC &/*aTag*/)
//
// Default InitL - ignored
//
/** Initialises a protocol before any binding is performed. A protocol can override
it to create any further resources it may require - this might include allocating
buffers, opening devices or reading more configuration information.

If the protocol encounters any errors it should leave.

@note This implementation does nothing.

@param aTag The string identifier for the protocol as given by its section
header in esock.ini */
	{
	}

EXPORT_C void CProtocolBase::StartL(void)
//
// Default StartL - ignored
//
/** Indicates to a protocol that all binding has completed successfully and it
can start processing datagrams. This is mainly of interest to the lowest levels
of a protocol stack which would queue a read on the network media device driver
in response to this call.

If a protocol cannot start to process data (for example it may not have bound
correctly) it should leave.

@note This implementation does nothing.
 */
	{
	}

EXPORT_C void CProtocolBase::BindL(CProtocolBase* /*protocol*/, TUint /*id*/)
//
// Default BindL - Panic
//
/** This function panics with a panic number of EDoesNotBindBelow.

@param protocol The higher level protocol requesting to bind
@param id The ID number of the protocol requesting to bind */
	{
	Panic(EDoesNotBindBelow);
	}

EXPORT_C void CProtocolBase::BindToL(CProtocolBase* /*protocol*/)
//
// Default BindToL - Panic
//
/** This function panics with a panic number of EDoesNotBindAbove.

@param protocol The lower level protocol whose bind function must be called.
*/
	{
	Panic(EDoesNotBindAbove);
	}

EXPORT_C TInt CProtocolBase::Send(RMBufChain& /*aPDU*/,CProtocolBase* /*aSourceProtocol=NULL*/)
/**
This function panics with a panic number of ECantSendMBufs.

@param aPDU, MBuf chain
@param aSourceProtocol, The protocol from which the data has been sent
*/
	{
	Panic(ECantSendMBufs);
	return 0;
	}

EXPORT_C TInt CProtocolBase::Send(TDes8& /*aPDU*/,TSockAddr* /*to*/,TSockAddr* /*from=NULL*/,CProtocolBase* /*aSourceProtocol=NULL*/)
/** 
This function panics with a panic number of ECantSendDescriptors.

@param aPDU The datagram to be sent.
@param aTo  The address to send the datagram to.
@param aFrom The address originating the send.
@param aSourceProtocol The protocol from which the data has been sent.
@return KErrNone or another of the system wide error codes.
*/
	{
	Panic(ECantSendDescriptors);
	return 0;
	}

EXPORT_C void CProtocolBase::Process(RMBufChain &,CProtocolBase* /*aSourceProtocol=NULL*/)
/**
This function panics with a panic number of ECantProcessMbufs.

@param MBuf Chain
@param aSourceProtocol, The datagram to be sent
*/
	{
	Panic(ECantProcessMbufs);
	}

EXPORT_C void CProtocolBase::Process(TDes8& /*aPDU*/,TSockAddr* /*from*/,TSockAddr* /*to=NULL*/,CProtocolBase* /*aSourceProtocol=NULL*/)
/** 
This function panics with a panic number of ECantProcessDescriptors.

@param aPDU The datagram to process.
@param aFrom The address the datagram is from.
@param aTo The address the datagram is to.
@param aSourceProtocol The higher level protocol which we want to
process our incoming datagram.
*/
	{
	Panic(ECantProcessDescriptors);
	}

EXPORT_C TInt CProtocolBase::GetOption(TUint /*level*/,TUint /*name*/,TDes8& /*option*/,CProtocolBase* /*aSourceProtocol=NULL*/)
//
// default Getoption - return KErrNotSupported
//
/** Gets options for the protocol on behalf of socket server clients.

The option parameter is protocol-defined - this is a generic API.
The protocol is passed in because you might get GetOption() calls from
multiple bindees - this is actually more meaningful for SetOption().

@param aLevel  The level in the stack the option request is aimed at.
@param aName  The name of the option.
@param anOption Descriptor to hold the option data.
@param aSourceProtocol The protocol making the request.
@return  KErrNone or another of the system wide error codes; this version returns KErrNotSupported.
*/
	{
	return KErrNotSupported;
	}

EXPORT_C TInt CProtocolBase::SetOption(TUint /*level*/,TUint /*name*/,const TDesC8& /*option*/,CProtocolBase* /*aSourceProtocol=NULL*/)
//
// default SetOption - return KErrNotSupported
//
/** Sets options for the protocol on behalf of socket server clients.

The option parameter is protocol-defined - this is a generic API.

The protocol is passed in because you might get SetOption() calls from
multiple bindees and need to distinguish who is setting what.

@param aLevel  The level in the stack the option request is aimed at.
@param aName  The name of the option.
@param anOption Descriptor to hold the option data.
@param aSourceProtocol The protocol making the request.
@return  KErrNone or another of the system wide error codes; this version returns KErrNotSupported.
*/
	{
	return KErrNotSupported;
	}

EXPORT_C void CProtocolBase::Error(TInt anError,CProtocolBase* aSourceProtocol)
/** 
This function panics with a panic number of EErrorCallNotHandled unless the error is KErrCancel
which is simply ignored.

@param anError The error code
@param aSourceProtocol The protocol raising the error */
	{
	#ifdef ESOCK_LOGGING_ACTIVE
		_LIT(KUnknownProtocolTag, "unknown");

		TPtrC tagPtr(KUnknownProtocolTag);
		if (iManagerRef)
			{
			tagPtr.Set(iManagerRef->Tag());
			}

		TPtrC srcTagPtr(KUnknownProtocolTag);
		if (aSourceProtocol && aSourceProtocol->iManagerRef)
			{
			srcTagPtr.Set(aSourceProtocol->iManagerRef->Tag());
			}

		TPtrC panicCatPtr(KESockProtocolPanic);

		if(anError == KErrCancel)
			{
			LOG( ESockLog::Printf(KESockErrorTag, _L("CProtocolBase %08x:\tError() - error: KErrCancel, protocol: %S, source protocol: %S (%08X)"), this, &srcTagPtr, &tagPtr, aSourceProtocol ) );
			}
		else
			{
			LOG( ESockLog::Printf(KESockErrorTag, _L("CProtocolBase %08x:\tError() - error: %d, source protocol: %S (%08X), %S protocol panicing with category: %S, reason: EErrorCallNotHandled"), this, anError, &srcTagPtr, aSourceProtocol, &tagPtr, &panicCatPtr ) );
			}
	#else
		(void)anError;
		(void)aSourceProtocol;
	#endif

	// We do not panic if an operation (e.g., StartSending in tcpip6) was merely cancelled.
	if(anError != KErrCancel)
		{
		Panic(EErrorCallNotHandled);
		}
	}

CProtocolFamilyBase * CProtocolBase::ProtocolFamily()
/**
Get our family - through the manager ref.

@return protocol family
*/
	{
	return iManagerRef->Family();
	}

EXPORT_C CHostResolvProvdBase *CProtocolBase::NewHostResolverL()
/** 
This function panics with a panic number of EBadHostResolver.

@return The host name resolver service */
	{
	Fault(EBadHostResolver);
	return (CHostResolvProvdBase*)NULL;	 //lint !e527	// unreachable
	}

EXPORT_C CServiceResolvProvdBase *CProtocolBase::NewServiceResolverL()
/** 
This function panics with a panic number of EBadServiceResolver.

@return The service resolver service */
	{
	Fault(EBadServiceResolver);
	return (CServiceResolvProvdBase*)NULL;	 //lint !e527	// unreachable
	}


EXPORT_C CNetDBProvdBase *CProtocolBase::NewNetDatabaseL()
/** 
This function panics with a panic number of EBadNetDBRequest.

@return The network database access service */
	{

	Fault(EBadNetDBRequest);
	return (CNetDBProvdBase*)NULL;	 //lint !e527	// unreachable
	}

EXPORT_C CProtocolBase::~CProtocolBase()
/**
Destroy the Protocol Base
*/
	{
	LOG( ESockLog::Printf(KESockProvChoresTag, _L("CProtocolBase %08x:\t~CProtocolBase()"), this ) );

	if (iManagerRef)
		{
		iManagerRef->ProtocolClosed();
		}
	}

EXPORT_C CProtocolBase::CProtocolBase()
/**
Constructor for the Protocol Base
*/
	:CBase()
	{
	LOG( ESockLog::Printf(KESockProvChoresTag, _L("CProtocolBase %08x:\tCProtocolBase()"), this ) );
	}

EXPORT_C void CProtocolBase::Open()
//
// C'Tor for the Protocol Base
//
/** Opens a client connection.

The default implementation simply increments the reference count.

Any implementations by derived classes should call this base
class implementation. */
	{
	iRefCount++;
	LOG	(
		const TDesC& tag(Tag());
		ESockLog::Printf(KESockProvChoresTag, _L("CProtocolBase %08x:\tOpen() '%S' RefCount %d"), this , &tag, iRefCount)
		);
	}

EXPORT_C void CProtocolBase::Close()
//
// C'Tor for the Protocol Base
//
/** Closes a client connection.

The default implementation decrements the reference count and calls
CloseNow() if no clients are connected.

Any implementations by derived classes should call this base class implementation. */
	{
	iRefCount--;
	LOG	(
		const TDesC& tag(Tag());
		ESockLog::Printf(KESockProvChoresTag, _L("CProtocolBase %08x:\tClose() '%S' RefCount %d"), this, &tag, iRefCount)
		);

	if (iRefCount<=0)
		{
		if (SocketServer::IsShuttingDown())
			{
			delete this;
			}
		else
			{
			CloseNow();
			}
		}
	}

EXPORT_C void CProtocolBase::CloseNow()
//
// Default CloseNow function - just delete the protocol
//
/** Closes the protocol.

Called by Close() when all clients referencing a protocol have disconnected.

The default implementation simply deletes the object. Derived classes can implement
this function to provide a delayed closedown: for example, to allow reliable
protocols to ensure that all data has drained from the stack before it is deleted.
When an acceptable closedown condition has been reached, the protocol should call
CanClose(). */
	{
	delete this;
	}

EXPORT_C void CProtocolBase::CanClose()
//
// Up call from derived classes
//
/** Called by derived classes when an acceptable closedown condition has been reached.

Derived classes should not override this function.

@see CloseNow() for more details. */
	{
	delete this;
	}

void CProtocolBase::TryDelete()
/**
delete Protocol Base if ref count <0
Used from Protocol manager cleanup stack entries
*/
	{
	if (iRefCount<=0)
		{
		delete this;
		}
	}

EXPORT_C TBool CProtocolFamilyBase::QueryVersionSupported( TVersion const & aVersion ) const
/**
Queries whether the version is supported by the protocol

@param aVersion, version information
@return ETrue if supported else EFalse
*/
	{
	return(User::QueryVersionSupported(iVersion,aVersion));
	}

void CProtocolFamilyBase::SetLibraryL(RLibrary& aLib)
/**
Sets the Protocol Family's resource library to dynamically loadable DLL.

@param aLib, a handle to a dynamically loadable DLL
*/
	{
	iLibUnloader=CLibUnloader::NewL(aLib);
	}

EXPORT_C CProtocolFamilyBase::~CProtocolFamilyBase()
/**
Destructor for the protocol family
Calling this means that all protocols have been unloaded and we can close the
library after our death.
*/
	{

	if (iManagerRef)
		{
		iManagerRef->FamilyClosed();
		}

	// Can't close the library because of the ret instruction for this function is in it.
	// Ask a CAsync to unload the library for us.
	if(iLibUnloader)
		{
		LOG( ESockLog::Printf(KESockProvChoresTag, _L8("~CProtocolFamilyBase(%08x) - unload of PRT requested"), this ) );
		iLibUnloader->Unload();
		}

	CSockManData * globals=SockManGlobals::Get();
	globals->iNumFamilies--;

	if (globals->iShutdownGracefully)
		{
//		if (globals->iNumSessions<=0 && globals->iNumFamilies<=0)
//			{
//			globals->iShutdownWatchDog->Shutdown();
//			}
//		else
//			{
//			LOG( ESockLog::Printf(_L8("CProtocolFamilyBase::~CProtocolFamilyBase() - shutdown requested: %d sessions, %d families remaining"), globals->iNumSessions, globals->iNumFamilies ) );
//			}
		LOG( ESockLog::Printf(KESockProvChoresTag, _L8("~CProtocolFamilyBase(%08x) - shutdown requested: %d families remaining"), this, globals->iNumFamilies ) );
		if (globals->iNumFamilies <= 0)
			{
//			globals->SelfWorker()->PlayerShutdownComplete();
			CWorkerThread* selfWorker = globals->SelfWorker();
			selfWorker->Player()->MaybeSetPlayerShutdownComplete(EFalse);
			selfWorker->MaybeTriggerThreadShutdownCallback();
			}
		else
			{
#ifdef _DEBUG
			globals->LogActiveProtocols();
#endif
			}
		}
	}

EXPORT_C TPtrC CProtocolBase::Tag()
//
// return the tag name
//
/** Gets a TPtrC to the protocol's tag name. The default implementation returns
the tag passed to Init().

@return Protocol's tag name */
	{
	return iManagerRef->Tag();
	}

EXPORT_C CProtocolFamilyBase::CProtocolFamilyBase() :CBase()
/**
Constructor for the protocol family
*/
	{

    SockManGlobals::Get()->iNumFamilies++;
	}

EXPORT_C void CProtocolFamilyBase::Open()
//
// C'Tor for the Protocol Base
//
/** Opens a protocol family.

When a socket is created, the socket server first calls this function on
the protocol family. It is not compulsory for the protocol
family to implement this function - the default behaviour of the socket
server, which is to increment an access count for the protocol family, may
be sufficient. */
	{
	iRefCount++;
	}

EXPORT_C void CProtocolFamilyBase::Close( )
/**
replacment close gives any derived class chance to delete anything it wants
or to signal an error.

Closes this reference counting object
*/
	{

	iRefCount--;
	if (iRefCount<=0)
		{
		if (Remove()==KErrNone)
			{
			delete this;
			}
		else
			{
			Panic(ERemoveRefused);
			}
		}
	}

EXPORT_C TInt CProtocolFamilyBase::Remove( )
/** Prepares the protocol family to be removed in a derived class.

The socket server calls Remove() before unloading the library for a given
protocol family.

@note This implementation returns KErrNone.

@return System-wide error code. The socket server guarantees that the protocol
family will always be in a suitable position to perform closedown. If this
is not the case, for example there are still active connections, then this
function should return an error code other that KErrNone. */
	{

	return(KErrNone);
	}
