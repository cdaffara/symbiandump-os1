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

#include <es_ver.h>
#include "CS_STD.H"
#include "es_flog.h"
#include <rsshared.h>

#include <comms-infras/esockdebugmessages.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock_internal.h>
#endif

#if defined (_DEBUG_SOCKET_FUNCTIONS)
#include <comms-infras/trbuf.h>
#endif

#if defined(_DEBUG) && !defined(__ESOCK_SUPPRESS_ESOCK_HANDLE_OVERWRITE_PANICS)
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockCSockCS_CLI, "ESockCSockCS_CLI");
#endif


#if defined (__FLOG_ACTIVE)

	/**
	@internalComponent
	*/
	struct TNo2NameMap
		{
		TUint iNumber;
		const char* iName;
		};
	/**
	they must be sorted by number!

	@internalComponent
	*/
	LOCAL_D const TNo2NameMap KAddrFamMap[] =
		{
			{ 0x0010, "smsprot" },
			{ 0x0011, "wapprot" },
			{ 0x0100, "irda"    },
			{ 0x0101, "bt"      },
			{ 0x0111, "plp"     },
			{ 0x0800, "tcpip"   },
			{ 0x0801, "ipsec6"  },
			{ 0x0803, "eaysymb" },
			{ 0x0806, "tcpip6"  },
			{ 0x08b5, "mip6"    }
		};

	/**
	@internalComponent
	*/
	#define NO2NAME(table,no) (MapNo2Name(table, sizeof(table)/sizeof(table[0]), no))

	/**
	do a binary search through the mapping table and return the
	name, if found.

	@internalComponent
	*/
	LOCAL_C const char* MapNo2Name(const TNo2NameMap* aMapping, TInt aSize, TUint aNumber)
	{
		TInt min = -1;
		TInt max = aSize;
		TInt median = 0;

		TBool found = EFalse;
		while(!found)
			{
			median = (max + min) / 2;

			if(aNumber > aMapping[median].iNumber)
				min = median;
			else if(aNumber < aMapping[median].iNumber)
				max = median;
			else
				found = ETrue;

			// to avoid forever loops
			if(median == ((max + min) / 2))
				break;
			if(min==max)
				break;
			}

		if(found)
			return aMapping[median].iName;
		else
			return "???";
		}

	/**
	@internalComponent
	*/
	LOCAL_D const char* const KSockTypeMap[5] =
		{
		"NULL",      // 0
		"stream",    // 1
		"datagram",  // 2
		"seqpacket", // 3
		"raw"        // 4
		};

	/**
	@internalComponent
	*/
	LOCAL_C const char* GetSockTypeMap(TUint aSockType)
		{
		return ( aSockType < sizeof(KSockTypeMap)/sizeof(KSockTypeMap[0]))
			?
			KSockTypeMap[aSockType]
			:
		"???";
		}

	/**
	they must be sorted by number!

	@internalComponent
	*/
	LOCAL_D const TNo2NameMap KProtocolMap[] =
		{
			{ 1,  "ICMP" },
			{ 4,  "IPv4" },
			{ 6,  "TCP"  },
			{ 17, "UDP"  },
			{ 41, "IPv6" },
			{ 50, "ESP"  },
			{ 51, "AH"   },
			{ 0x0101, "PROT_KEY"  }, // temp assignment
			{ 0x0103, "INET_HOOK" }, // temp assignment
			{ 0x0f01, "INET6_RES" }  // temp assignment (tcpip6\inc\res.h)
		};
#endif


EXPORT_C RSocketServ::RSocketServ()
/**
Default Constructor
*/
    {
    }

EXPORT_C TInt RSocketServ::Connect(TUint aAsyncMessageSlots /* = (TUint) -1 */)
/** Opens a session to the socket server.

Each outstanding asynchronous request made upon the session (typically via
a subsession such as RSocket) requires a message slot. If there is no free slot
the request is immediately completed with KErrServerBusy. If a client specifies
a fixed number of slots then they form an inelastic pool exclusive to the 
session. Alternatively a client can specify -1 to use the global free message
pool, which can grow upon demand but has poorer time bounds and can in principle
fail any request. For most clients the global pool will be appropriate since it 
removes the need to accurately determine the worst case size.

Note that in previous releases the default value was KESockDefaultMessageSlots (8).

RHandleBase::Close() should be called once the session is no longer required. 
All resources which are opened using the session will be automatically closed 
when the session terminates.

When the last session which has open resources for a protocol is closed a 
protocol module will be unloaded automatically by the socket server.

@param aAsyncMessageSlots The number of message slots required. If not specified then -1 to
use the global pool of free messages.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	LOG( ESockLog::Printf(_L8("RSocketServ %08x:\tConnect() tid %d"), this, (TUint)RThread().Id() ));
#ifndef __ESOCK_SUPPRESS_ESOCK_HANDLE_OVERWRITE_PANICS
	__ASSERT_DEBUG(Handle() == 0, User::Panic(KSpecAssert_ESockCSockCS_CLI, 1));
#endif
	
	TSessionPref pref;
	TInt r = Connect(pref, aAsyncMessageSlots);

   // Because ESock is now loaded by the Comms Root Server which is generally started during
	// the boot this should commonly succeed; however for test code this is still a possibility
	// Hence here we try starting it; this is an atomic operation (and cheap if it's already started)
	if (r==KErrNotFound)
		{
		r=StartC32();
		if (r==KErrNone || r==KErrAlreadyExists)
			{
			r = Connect(pref, aAsyncMessageSlots);
			}
		}

	return(r);
	}


/**
Default Constructor, makes sure that member data doesn't contain unnitialised values.
*/
EXPORT_C TSessionPref::TSessionPref():
    iAddrFamily(KUndefinedAddressFamily),
    iProtocol(KUndefinedProtocol),
    iReserved(0),
    iReserved1(0),
    iReserved2(0),
    iReserved3(0),
    iReserved4(0)
    {}

EXPORT_C TInt RSocketServ::Connect(const TSessionPref& aPref, TUint aMessageSlots /* = (TUint) -1 */)
/** Opens a session to the socket server.

By providing session prefs the client states what services they require of the server. This
allows the server to configure the client connection in an optimal manner. Such a 
configuration may not support all normal abilities of a session. For example a connection
opened specifying KAfInet for the address family will be able to create TCP sockets but
might not be able to create SMS sockets.

Each outstanding asynchronous request made upon the session (typically via
a subsession such as RSocket) requires a message slot. If there is no free slot
the request is immediately completed with KErrServerBusy. If a client specifies
a fixed number of slots then they form an inelastic pool exclusive to the 
session. Alternatively a client can specify -1 to use the global free message
pool, which can grow upon demand but has poorer time bounds and can in principle
fail any request. For most clients the global pool will be appropriate since it 
removes the need to accurately determine the worst case size.

Note that in previous releases the default value was KESockDefaultMessageSlots (8).

RHandleBase::Close() should be called once the session is no longer required.
All resources which are opened using the session will be automatically closed
when the session terminates.

When the last session which has open resources for a protocol is closed a
protocol module will be unloaded automatically by the socket server.

@param aPrefs Hint for server to create most optimal connection possible.
@param aAsyncMessageSlots The number of message slots required. If not specified then -1 to
use the global pool of free messages.
@return KErrNone if successful, otherwise another of the system-wide error
codes. */
	{
	LOG( ESockLog::Printf(_L8("RSocketServ %08x:\tConnect(TSessionPref(%d,%d,%d)) tid %d"), this, aPref.iAddrFamily, aPref.iProtocol, aPref.iReserved, (TUint)RThread().Id() ));
#ifndef __ESOCK_SUPPRESS_ESOCK_HANDLE_OVERWRITE_PANICS
	__ASSERT_DEBUG(Handle() == 0, User::Panic(KSpecAssert_ESockCSockCS_CLI, 2));
#endif
	TInt err = CreateSession(SOCKET_SERVER_NAME, TVersion(), aMessageSlots);
	if(err == KErrNone)
		{
		// Now we request the optimal server and if we get one we reconnect to that one.
		TServerName prefServerName;
		TPckg<TSessionPref> prefDesc(aPref);
		err = SendReceive(ESSRequestOptimalDealer, TIpcArgs(&prefDesc, &prefServerName));
		if(err == KErrNone && prefServerName.Length() > 0)
			{
			RSocketServ oldSess = *this;
			SetHandle(0);
			err = CreateSession(prefServerName, TVersion(), aMessageSlots);
			oldSess.Close();
			}
		else if (err == KErrNotFound)
			{
			//Well then the pit boss is the optimal dealer
			err = KErrNone;
			}
		}
	return err;
	}

EXPORT_C TInt RSocketServ::NumProtocols(TUint &aCount)
/** Gets the number of protocols the socket server is currently aware of.

The count returned can be used in conjunction with GetProtocolInfo().

@param aCount The number of protocols is returned in aCount
@return KErrNone if successful, otherwise another of the system-wide error
codes. */
    {
	TPtr8 n((TUint8 *)&aCount,sizeof(aCount));
    return SendReceive(ESSNumProtocols,TIpcArgs(&n));
    }

EXPORT_C TInt RSocketServ::GetProtocolInfo(TUint anIndex,TProtocolDesc &aProtocol)
//
// Only really implemented for completeness and debugging.
//
/** Gets information about a specific protocol.

@param anIndex Index of required protocol description. 
@param aProtocol A protocol description type to hold protocol information. 
@return KErrNone if successful, otherwise another of the system-wide error codes. */
    {
	TPckg<TProtocolDesc> protDesc(aProtocol);
	return SendReceive(ESSProtocolInfo,TIpcArgs(&protDesc,anIndex));
    }

EXPORT_C TInt RSocketServ::FindProtocol(const TProtocolName& aName,TProtocolDesc &aProtocol)
/** Gets information about a specific protocol, identified by it's name.

There is no wildcard support.

@param aName Typed descriptor which identifies a protocol name. 
@param aProtocol A protocol description type to hold protocol information. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
    {
	TPckg<TProtocolDesc> protDesc(aProtocol);
	return SendReceive(ESSProtocolInfoByName,TIpcArgs(&protDesc,&aName));
    }

EXPORT_C void RSocketServ::StartProtocol(TUint anAddrFamily,TUint aSockType,TUint aProtocol,TRequestStatus &aStatus)
//
// Effectively makes all the lengthy parts of Open asynchronous.
//
/** Loads a specified protocol asynchronously.

The protocol is specified by family, socket type and protocol identifier. 

Note that client programs do not normally need to call this 
function, as loading of a protocol is done automatically by the Sockets Server 
when a socket of that protocol is opened. Some applications may, however, need 
to ensure that an open socket call will not take a significant amount of time 
(e.g. IrCOMM). This function can be called by such applications to preload the 
protocol. 

There is no way to cancel this operation once it has been called. 

@param anAddrFamily Integer which identifies a protocol suite.
@param aSockType Integer which identifies a type of socket.
@param aProtocol Integer which identifies a specific protocol in a family.
@param aStatus On completion will contain an error code: see the system-wide error codes. 

@capability NetworkControl Protocol starting is a critical operation in the comms process and must be restricted
*/
	{
	LOG( ESockLog::Printf(_L8("StartProtocol: client %d [addrfam=0x%04x,socktype=%d,prot=%d]"), (TUint)RThread().Id(), anAddrFamily, aSockType, aProtocol));
	SendReceive(ESSProtocolStart,TIpcArgs(anAddrFamily,aSockType,aProtocol),aStatus);
	}

EXPORT_C void RSocketServ::StopProtocol(TUint anAddrFamily,TUint aSockType,TUint aProtocol,TRequestStatus& aStatus)
/**
Unload a protocol asynchronously - note it will fail if there are any resources open

@param anAddrFamily Integer which identifies a protocol suite
@param aSockType Integer which identifies a type of socket
@param aProtocol Integer which identifies a specific protocol in a family
@param aStatus On completion, will contain a system-wide error codes

@capability NetworkControl Protocol stopping is a critical operation in the comms process and must be restricted
*/
    {
	LOG(ESockLog::Printf(_L8("StopProtocol: client %d [addrfam=0x%04x,socktype=%d,prot=%d]"), (TUint)RThread().Id(), anAddrFamily, aSockType, aProtocol));
	SendReceive(ESSProtocolStop,TIpcArgs(anAddrFamily,aSockType,aProtocol),aStatus);
    }

EXPORT_C void RSocketServ::SetExclusiveMode(TRequestStatus& aStatus)
/**
Set this session as the socket server exclusive session.
PLEASE NOTE: This API is currently unsupported, calling it will always return KErrNone but without actually doing anything.

@param aStatus On completion, will contain a system-wide error codes

@capability NetworkControl Restricting Esock to one session is very dangerous and must be highly restricted
*/
    {
	SendReceive(ESSExclusiveMode,TIpcArgs(),aStatus);
    }

EXPORT_C void RSocketServ::ClearExclusiveMode()
/**
Clear exclusive mode for this session
PLEASE NOTE: This API is currently unsupported, calling it will always return KErrNone but without actually doing anything.
*/
    {
	SendReceive(ESSClearExclusiveMode,TIpcArgs());
    }

EXPORT_C TInt RSocketServ::InstallExtension(const TDesC& aDllName, const TDesC& aArgs)
/**
Install an Esock Extension.
PLEASE NOTE: This API is currently unsupported, calling it will always return KErrNotSupported.

@param aDllName DllName for Esock Extension
@param aArgs argument for Esock Extension
@return KErrNone if successful, otherwise another of the system-wide error codes.

@capability NetworkControl Loading extensions to Esock will be behavioural changing and must be restricted to privileged apps

@deprecated Because it always return KErrNotSupported thus useless
*/
    {
		return  (aArgs.Length())
			?SendReceive(ESSInstallExtension,TIpcArgs(&aDllName,&aArgs,aArgs.Length()))
			: SendReceive(ESSInstallExtension,TIpcArgs(&aDllName,(TAny*)NULL,(TAny*)NULL));
    }

EXPORT_C TVersion RSocketServ::Version(void) const
/** Gets the version number of this client.
	
@return Client side version number */
	{

	return(TVersion(KES32MajorVersionNumber,KES32MinorVersionNumber,KES32BuildVersionNumber));
	}

#if defined (_DEBUG_SOCKET_FUNCTIONS)
EXPORT_C TInt RSocketServ::__DbgMarkHeap()
/**
Set a heap mark in the socket server

Marks the start of checking the current thread's heap.

@return In debug builds, KErrNone, if successful, otherwise one of the other system wide error codes. In release builds, KErrNone always.
*/
	{
	return SendReceive(ESSDbgMarkHeap,TIpcArgs());
	}

EXPORT_C TInt RSocketServ::__DbgCheckHeap(TInt aCount)
/**
Checks that the number of allocated cells on the current thread's heap is the same as the specified value

@param aCount In debug builds, the number of heap cells expected to be allocated.
@return In debug builds, KErrNone, if successful, otherwise one of the other system wide error codes
*/
	{
	return SendReceive(ESSDbgCheckHeap,TIpcArgs(aCount));//check if it's right 
	}

EXPORT_C TInt RSocketServ::__DbgMarkEnd(TInt aCount)
/**
Set a heap mark in the socket server

Marks the end of checking the current thread's heap

@param aCount In debug builds, the number of heap cells expected to be allocated.
@return In debug builds, KErrNone, if successful, otherwise one of the other system wide error codes
*/
	{
	return SendReceive(ESSDbgMarkEnd,TIpcArgs(aCount));//check if it's right 
	}

EXPORT_C TInt RSocketServ::__DbgFailNext(TInt aCount)
/**
Set a heap mark in the socket server

Simulates heap allocation failure.

@param aCount In debug builds, the number of heap cells expected to be allocated.
@return In debug builds, KErrNone, if successful, otherwise one of the other system wide error codes
*/
	{
	return SendReceive(ESSDbgFailNext,TIpcArgs(aCount));//check if it's right 
	}
	
EXPORT_C TBool RSocketServ::__DbgCheckFailNext() const
/**
Check whether a simulated OOM fail-next set via __DbgFailNext() is still active, generally indicates that some code
under test hasn't explored all allocation points
 
@return EFalse if any ESOCK server heap has left FailNext simulation mode
*/
    {
	return SendReceive(ESSDbgCheckFailNext);
	}

EXPORT_C TInt RSocketServ::__DbgFailNextMbuf(TInt aCount)
/**
Set a Mbuf mark in the socket server

Simulates Mbuf allocation failure.

@param aCount In debug builds, the number of heap cells expected to be allocated.
@return In debug builds, KErrNone, if successful, otherwise one of the other system wide error codes
*/
	{
	return SendReceive(ESSDbgFailNextMbuf,TIpcArgs(aCount));//check if it's right 
	}

EXPORT_C TInt RSocketServ::__DbgSetMbufPoolLimit(TInt aSize)
/**
Set the Mbuf pool limit

@param aSize indiactes the limit of MBuf pool.
@return In debug builds, KErrNone, if successful, otherwise one of the other system wide error codes
*/
	{
	return SendReceive(ESSDbgSetMbufPoolLimit,TIpcArgs(aSize));//check if it's right 
	}

EXPORT_C TInt RSocketServ::__DbgCheckMbuf(TInt aSize)
/**
Checks that the number of allocated cells on the current thread's MBuf is the same as the specified value

@param aSize indiactes the limit of MBuf pool.
@return In debug builds, KErrNone, if successful, otherwise one of the other system wide error codes
*/
	{
	return SendReceive(ESSDbgCheckMbuf,TIpcArgs(aSize));//check if it's right 
	}

EXPORT_C TInt RSocketServ::__DbgMbufFreeSpace()
/**
Get the amount of free space in the MBuf manager

@return free space in the MBuf manager
*/
	{
	TPckgBuf<TInt> i;
	TInt result=SendReceive(ESSDbgMbufFreeSpace,TIpcArgs(&i));
	if (KErrNone == result)
		return i();
	else
		return -1;
	}

EXPORT_C TInt RSocketServ::__DbgMbufTotalSpace()
/**
Get the amount of total space  available in the MBuf manager

@return total space  available in the MBuf manager
*/
	{
	TPckgBuf<TInt> i;
	TInt result = SendReceive(ESSDbgMbufTotalSpace,TIpcArgs(&i));
	if (KErrNone == result)
		return i();
	else
		return -1;
	}

EXPORT_C TInt RSocketServ::__DbgControl(const ESockDebug::TControlMsg& aRequestMsg)
/**
General purpose control message for esock debug

@return 
*/
	{
	Elements::TRBuf8* requestMsgBuffer = Elements::TRBuf8::New(aRequestMsg.Length());
	aRequestMsg.Store(*requestMsgBuffer);

	TPckgBuf<TInt> i;
	TInt result=SendReceive(ESSDbgControl, TIpcArgs(&i, requestMsgBuffer));
	if (KErrNone == result)
		return i();
	else
		return -1;
	}

#else
EXPORT_C TInt RSocketServ::__DbgMarkHeap()
/**
Set a heap mark in the socket server
*/
	{
   return KErrNone;
	}

EXPORT_C TInt RSocketServ::__DbgCheckHeap(TInt /*aCount*/)
/**
Set a heap mark in the socket server
*/
	{
   return KErrNone;
	}

EXPORT_C TInt RSocketServ::__DbgMarkEnd(TInt /*aCount*/)
/**
Set a heap mark in the socket server
*/
	{
   return KErrNone;
	}

EXPORT_C TInt RSocketServ::__DbgFailNext(TInt /*aCount*/)
/**
Set a heap mark in the socket server
*/
	{
   return KErrNone;
	}

EXPORT_C TBool RSocketServ::__DbgCheckFailNext() const
/**
Empty release implementation
*/
    {
    return KErrNone;
	}
	
EXPORT_C TInt RSocketServ::__DbgFailNextMbuf(TInt /*aCount*/)
/**
Set a Mbuf mark in the socket server
*/
   {
   return KErrNone;
	}

EXPORT_C TInt RSocketServ::__DbgSetMbufPoolLimit(TInt /*aSize*/)
/**
Set the Mbuf pool limit
*/
   {
   return KErrNone;
	}

EXPORT_C TInt RSocketServ::__DbgCheckMbuf(TInt /*aSize*/)
/**
Set the Mbuf pool limit
*/
   {
   return KErrNone;
	}

EXPORT_C TInt RSocketServ::__DbgMbufFreeSpace()
/**
Get the amount of free space in the MBuf manager
*/
   {
   return KErrNone;
	}

EXPORT_C TInt RSocketServ::__DbgMbufTotalSpace()
/**
Get the amount of free space in the MBuf manager
*/
   {
   return KErrNone;
	}

EXPORT_C TInt RSocketServ::__DbgControl(const ESockDebug::TControlMsg& /*aRequestMsg*/)
/**
General purpose control message for esock debug, self dispatching messages to be thrown over it

@return 
*/
	{
	return KErrNone;
	}

#endif // _DEBUG

EXPORT_C RSocket::RSocket()
/**
Default Constructor
*/
	{
	}

EXPORT_C TInt RSocket::Open(RSocketServ &aServer,TUint anAddrFamily,TUint aSockType,TUint aProtocol)
/** Opens an implicit socket by creating a new subsession to the socket server. Implicit socket is not explicitly associated
with any connection. The socket will choose the default connection for sending the packets. 

Implicit socket accepts packets routed through any connection.


Opens a channel to a protocol identified by a tuple of constants. If a socket 
is the first to be opened for a protocol it will have the additional effect 
of loading the protocol in the socket server.

NOTE: Deprecated default connection scenario.
Applications exercising the default connection scenario must (from 9.3 onwards) switch to explicit 
connection scenario. 
- The default connection scenario is where an application holding one started RConnection opens a socket 
  without explicitly associating it with the RConnection. The explicit association can be made by passing 
  the RConnection or a derived RSubConnection to an appropriate overload of RSocket::Open(...) 
  (not this method). 
- The implicit connection scenario is where an application holding either none or many started 
  RConnections (to different access points) opens a socket without explicitly associating it an RConnection.
- The explicit connection scenario is where an application holding started RConnections opens a 
  socket explicitly associating it with one of its RConnections.

Applications attempting to exercise the default connection scenario shall (from 9.3 onwards) 
receive an error from RSocket::Open(...). The actual error code is yet to be defined.

@param aServer The socket server session.
@param anAddrFamily A valid address constant for a protocol family.
@param aSockType A valid socket type for the protocol.
@param aProtocol A protocol constant which identifies a specific protocol. 
@return KErrNone if successful, otherwise another of the system-wide error codes

@capability Dependent Capability required depends on the type of socket so deferred to PRT
*/
    {
	RSessionBase &s=aServer;

	const TInt ret = CreateSubSession(s,ESoCreate,TIpcArgs(anAddrFamily,aSockType,aProtocol));

	LOG( ESockLog::Printf(_L8("RSocket %08x:\tOpen(addrfam=%s(0x%04x), socktype=%s(%d), prot=%s(%d)): tid %d, ret=%d]"),
		this, NO2NAME(KAddrFamMap, anAddrFamily), anAddrFamily,
		GetSockTypeMap(aSockType), aSockType,
		NO2NAME(KProtocolMap, aProtocol), aProtocol,
		(TUint)RThread().Id(), ret));

	return ret;
	}

EXPORT_C TInt RSocket::Open(RSocketServ &aServer,const TDesC& aName)
/** Opens an implicit socket by creating a new subsession to the socket server. Implicit socket is not explicitly associated
with any connection. The socket will choose the default connection for sending the packets. 

Implicit socket accepts packets routed through any connection.

Opens a channel to a protocol identified by a name. If a socket is the 
first to be opened for a protocol it will have the additional effect of loading 
the protocol in the socket server.

NOTE: Deprecated default connection scenario.
Applications exercising the default connection scenario must (from 9.3 onwards) switch to explicit 
connection scenario. 
- The default connection scenario is where an application holding one started RConnection opens a socket 
  without explicitly associating it with the RConnection. The explicit association can be made by passing 
  the RConnection or a derived RSubConnection to an appropriate overload of RSocket::Open(...) 
  (not this method). 
- The implicit connection scenario is where an application holding either none or many started 
  RConnections (to different access points) opens a socket without explicitly associating it an RConnection.
- The explicit connection scenario is where an application holding started RConnections opens a 
  socket explicitly associating it with one of its RConnections.

Applications attempting to exercise the default connection scenario shall (from 9.3 onwards) 
receive an error from RSocket::Open(...). The actual error code is yet to be defined.

@param aServer The socket server session.
@param aName Name of a protocol.
@return KErrNone if successful, otherwise another of the system-wide error codes. 

@capability Dependent Capability required depends on the type of socket so deferred to PRT
*/
	{
	LOG(TBuf8<64> buf8);
	LOG(buf8.Copy(aName));
	LOG(ESockLog::Printf(_L8("RSocket %08x:\tOpen(name=%S) tid %d"), this, &buf8, (TUint)RThread().Id() ));
	if ((aName.Locate(KMatchAny)!=KErrNotFound) || (aName.Locate(KMatchOne)!=KErrNotFound))
		return KErrBadName;
	TProtocolDesc d;
	TInt r=aServer.FindProtocol(aName,d);
	if (r!=KErrNone)
		return r;
	return Open(aServer,d.iAddrFamily,d.iSockType,d.iProtocol);
	}

EXPORT_C TInt RSocket::Open(RSocketServ &aServer)
/**Opens an implicit socket by creating a new subsession to the socket server. Implicit socket is not explicitly associated
with any connection. The socket will choose the default connection for sending the packets. 

Implicit socket accepts packets routed through any connection.


Provides a blank channel to the socket server which has no protocol 
associated. A socket opened in this manner is suitable as an argument to Accept(), 
which will marry the blank socket to a protocol when a connection is established 
to a remote endpoint.

@param aServer The socket server session.
@return KErrNone if successful, otherwise another of the system-wide error 
codes.

@capability Dependent Capability required depends on the type of socket so deferred to PRT
*/
    {
	return CreateSubSession(aServer,ESoCreateNull,TIpcArgs());
	}

EXPORT_C TInt RSocket::Open(RSocketServ &aServer,TUint anAddrFamily,TUint aSockType,TUint aProtocol,RConnection& aConnection)
/**Opens an explicit socket by creating a new subsession to the socket server. The socket is explicitly associated with the
same underlying connection as an existing RConnection instance.

Socket traffic is directed to and from the specified connection only.


@note The association is instantaneous, in that the socket is associated with the
interface that the RConnection is associated with at the present time.  This association
terminates when the underlying interface goes down.

@released since 7.0S

@param aServer The socket server session.
@param anAddrFamily A valid address constant for a protocol family.
@param aSockType A valid socket type for the protocol.
@param aProtocol A protocol constant which identifies a specific protocol.
@param aConnection Existing RConnection whose interface this socket will be associated with.
@return KErrNone if successful, otherwise another of the system-wide error codes.

@capability Dependent Capability required depends on the type of connection so deferred to PRT

 */
    {
	// passing an RConnection which doesn't belong to the same session is a serious programming error, hence panic.
	if (!aConnection.SameSession(aServer.Handle()))
		Panic(EBadRConnection);

	TSockOpenBufC openArgPkg;
	openArgPkg.iArgs.iAddrFamily = anAddrFamily;
	openArgPkg.iArgs.iSockType = aSockType;
	openArgPkg.iArgs.iProtocol = aProtocol;
	openArgPkg.iArgs.iHandle = aConnection.SubSessionHandle();

	RSessionBase &s=aServer;
	const TInt ret = CreateSubSession(s,ESoCreateWithConnection,TIpcArgs(&openArgPkg));

	LOG(ESockLog::Printf(_L8("RSocket::Open(addrfam=0x%04x(%s), socktype=%d(%s), prot=%d(%s), rconn=%x): tid %d ,ret=%d"),
		anAddrFamily, NO2NAME(KAddrFamMap, anAddrFamily),
		aSockType, GetSockTypeMap(aSockType),
		aProtocol, NO2NAME(KProtocolMap, aProtocol),
		aConnection.SubSessionHandle(), (TUint)RThread().Id(), ret));

	return ret;
	}

EXPORT_C TInt RSocket::Open(RSocketServ &aServer,TUint anAddrFamily,TUint aSockType,TUint aProtocol,RSubConnection& aSubConnection)
/**Opens an explicit socket by creating a new subsession to the socket server. The socket is explicitly associated with the
same underlying connection as an existing RSubConnection instance.

Socket traffic is directed to and from the specified connection only.
	

@released since 7.0S

@param aServer The socket server session.
@param anAddrFamily A valid address constant for a protocol family.
@param aSockType A valid socket type for the protocol.
@param aProtocol A protocol constant which identifies a specific protocol.
@param aSubConnection Existing RSubConnection this socket will be associated with.
@return KErrNone if successful, otherwise another of the system-wide error codes.

@capability Dependent Capability required depends on the type of connection so deferred to PRT

 */
    {
	// passing an RSubConnection which doesn't belong to the same session is a serious programming error, hence panic.
	if (!aSubConnection.SameSession(aServer.Handle()))
		{
		Panic(EBadRConnection);
		}

	TSockOpenBufC openArgPkg;
	openArgPkg.iArgs.iAddrFamily = anAddrFamily;
	openArgPkg.iArgs.iSockType = aSockType;
	openArgPkg.iArgs.iProtocol = aProtocol;
	openArgPkg.iArgs.iHandle = aSubConnection.SubSessionHandle();

	RSessionBase &s=aServer;
	const TInt ret = CreateSubSession(s, ESoCreateWithSubConnection,TIpcArgs(&openArgPkg));

	LOG( ESockLog::Printf(_L8("RSocket::Open(addrfam=0x%04x(%s), socktype=%d(%s), prot=%d(%s), rsubconn=%x): tid %d ,ret=%d"),
		anAddrFamily, NO2NAME(KAddrFamMap, anAddrFamily),
		aSockType, GetSockTypeMap(aSockType),
		aProtocol, NO2NAME(KProtocolMap, aProtocol),
		aSubConnection.SubSessionHandle(), (TUint)RThread().Id(), ret));

	return ret;
	}

EXPORT_C void RSocket::Close()
//
// Blocking close
//
/** Closes a socket. 

If a socket has been opened using Open() then it should be closed using Close(). 
This will ensure all associated resources are released. 

Closing serves two distinct purposes:

To release resources associated with the IPC channel to the socket server.

To disconnect a socket if it is connected.

If a socket is connected, then calling close is equivalent to calling Shutdown() 
with an argument of RSocket::ENormal, synchronously waiting for the request 
to complete, and then closing the IPC channel. If asynchronous or alternative 
methods of disconnecting are required then Shutdown() should be called before 
Close().

If the RSocketServ session on which a protocol was opened is closed, then 
all sockets associated with that session will be abortively closed and any 
further requests on the sockets will result in panics.

If a protocol has the flag KSIGracefulClose in its protocol information, when 
Close() is called on a connected socket, the socket will synchronously block 
until a response to a close request has been received or some other protocol 
condition causes the call to complete. */
	{
	LOG( ESockLog::Printf(_L8("RSocket %08x:\tClose() tid %d"), this, (TUint)RThread().Id()));
	if (SubSessionHandle())
		{
		CloseSubSession(ESoClose);
		}
	}

EXPORT_C void RSocket::Shutdown(TShutdown aHow,TRequestStatus &aStatus)
/** Shuts down a connected socket - asynchronous.

This method is asynchronous as non emergency shutdown may take a while.

The shut down method allows input and output to be individually stopped for a 
protocol endpoint. For protocols which support data-in disconnect message, 
additional arguments are provided.

Shutdown() can be used for protocols which do not have the KSIConnectionLess 
flag in their protocol information.

To use data in disconnection a protocol must have the flag KSIDisconnectData 
in its protocol information.

There is no way to cancel a socket shutdown once it has started.

@param aHow Shutdown option. All variants complete when a socket is disconnected.
If the parameter is within the range of TShutdown values, pending read and write
operations are cancelled.  If the parameter is outside the range of TShutdown values,
then the behaviour is as if ENormal were specified except that pending read and write
operations are not cancelled.  Note that the behaviour of using parameters outside the
range of TShutdown values may change in a future release and should not be relied upon.

@param aStatus On return KErrNone if successful, otherwise another of the system-wide error 
codes.

@capability Dependent Capability required depends on the type of socket so deferred to PRT
*/
	{
    SendReceive(ESoShutdown,TIpcArgs(aHow),aStatus);
	}

EXPORT_C void RSocket::Shutdown(TShutdown aHow,const TDesC8 &aDisconnectDataOut,TDes8 &aDisconnectDataIn,TRequestStatus &aStatus)
/** 
Shuts down a connected socket with disconnect data - asynchronous.

This method is asynchronous as non emergency shutdown may take a while.

The shut down method allows input and output to be individually stopped for a 
protocol endpoint. For protocols which support data-in disconnect message, 
additional arguments are provided.

Shutdown() can be used for protocols which do not have the KSIConnectionLess 
flag in their protocol information.

To use data in disconnection a protocol must have the flag KSIConnectData 
in its protocol information.

There is no way to cancel a socket shutdown once it has started.

@param aHow Shutdown option. All variants complete when a socket is disconnected.
@param aDisconnectDataOut A descriptor containing data to be sent.
@param aDisconnectDataIn A descriptor to receive data.
@param aStatus On return KErrNone if successful, otherwise another of the system-wide error 
codes.

@capability Dependent Capability required depends on the type of socket so deferred to PRT
*/
	{
    SendReceive(ESoShutdown,TIpcArgs(aHow,&aDisconnectDataOut,&aDisconnectDataIn),aStatus);
	}

EXPORT_C void RSocket::Send(const TDesC8 &aBuffer,TUint someFlags,TRequestStatus &aStatus)
/** Sends data to a remote host on a connected socket with options set by protocol 
specific flags.

A socket may only have one send operation in progress at any one time. Send() 
should only be used with connected sockets. 
	
If a protocol's information flag is marked with KSIUrgentData, then KSockWriteUrgent 
may be provided as a flag to Send(). All other flags are protocol specific.

@param aBuffer The data to be sent.
@param someFlags Flags which are passed through to protocol
@param aStatus On return KErrNone if successful, otherwise another of the system-wide error 
codes. Note that KErrEof indicates that the socket has been shutdown with option 
EStopOutput. 

@capability Dependent on the type of socket so deferred to PRT */
	{
	SendReceive(ESoSendNoLength,TIpcArgs(someFlags,aBuffer.Length(),&aBuffer),aStatus);
	}

EXPORT_C void RSocket::Send(const TDesC8 &aBuffer,TUint someFlags,TRequestStatus &aStatus,TSockXfrLength &aLen)
/** Sends data to a remote host on a connected socket with options set by protocol specific flags.

The length of the descriptor indicates the amount of data to be sent. The 
TSockXfrLength argument will return the amount of data actually sent. 

A socket may only have one send operation in progress at any one time.

Send() should only be used with connected sockets. 

If a protocol's information flag is marked with KSIUrgentData, then KSockWriteUrgent 
may be provided as a flag to Send(). All other flags are protocol specific.

@param aBuffer The data to be sent.
@param someFlags Flags which are passed through to protocol
@param aStatus On return KErrNone if successful, otherwise another of the system-wide error 
codes. Note that KErrEof indicates that the socket has been shutdown 
with option EStopOutput
@param aLen Filled in with amount of data sent before completion

@capability Dependent Capability required depends on the type of socket so deferred to PRT */
	{
	//see comment in  RSocket::Send(const TDesC8 &aBuffer,TUint someFlags,TRequestStatus &aStatus)
	SendReceive(ESoSend,TIpcArgs(someFlags,&aLen,&aBuffer),aStatus);
	}


EXPORT_C void RSocket::Recv(TDes8 &aBuffer,TUint someFlags,TRequestStatus &aStatus)
/** Receives data from a remote host, allowing flags for protocol specific information.
	
For a stream-interfaced socket the function only completes when the full 
amount of requested data has been received (or the connection breaks). This 
means when the descriptor has been filled to its maximum length (not its current 
length). 

For a datagram-interface socket, the function completes when one datagram 
arrives - even if it is not sufficient to fill the buffer. If the datagram 
does not fit in the buffer supplied then the remaining data will be lost. 

Recv() should only be used with connected sockets. 

A socket may only have one receive operation outstanding at any one time.

If a protocol's information flag is marked with KSIPeekData, then KSockReadPeek 
may be provided as a flag to Recv(). All other flags are protocol specific.

@param aBuffer A descriptor where data received will be placed.
@param someFlags Flags for protocol specific information.
@param aStatus On return, KErrNone if successful, otherwise another of the system-wide 
error codes. Note that KErrEof indicates either that a remote connection is closed, 
and that no more data is available for reading, or the socket has been shutdown 
with option RSocket::EStopInput.

@capability Dependent on the type of socket so deferred to PRT */
	{
	//see comment in  RSocket::Send(const TDesC8 &aBuffer,TUint someFlags,TRequestStatus &aStatus)	
	SendReceive(ESoRecvNoLength,TIpcArgs(someFlags,aBuffer.MaxLength(),&aBuffer),aStatus);
	}

EXPORT_C void RSocket::Recv(TDes8 &aBuffer,TUint someFlags,TRequestStatus &aStatus,TSockXfrLength &aLen)
/** Receives data from a remote host, allowing flags for protocol specific information. 
		
For a stream-interfaced sockets, the function only completes when the full 
amount of requested data has been received (or the connection breaks). This 
means when the descriptor has been filled to its maximum length (not its current 
length). 

For a datagram-interface socket, the function completes when one datagram 
arrives - even if it is not sufficient to fill the buffer. If the datagram does not fit
in the buffer supplied, remaining data may be retrieved using the Datagram Continuation feature.

This function implements the Datagram Continuation feature for PRTs implementing PRT1.5: the ability to read a datagram in parts. 
For a client request to read a datagram, using the TSockXfrLength parameter, remaining unread octets of the datagram 
are returned in the TSockXfrLength parameter.
The client can then read the remaining octets by further reads with the  'KSockReadContinuation' flag OR'd into the flags field. 
Remaining octets are discarded upon the next read without the KSockReadContinuation flag set.

Recv() should only be used with connected sockets. 

A socket may only have one receive operation outstanding at any one time.

If a protocol's information flag is marked with KSIPeekData, then KSockReadPeek 
may be provided as a flag to Recv(). All other flags are protocol specific.

@param aBuffer A descriptor where data received will be placed.
@param someFlags Flags for protocol specific information.
@param aStatus Reference to the request status object. On completion, KErrNone if 
successful, otherwise one of the system wide error codes. Note that KErrEof 
indicates either that a remote connection is closed, and that no more data 
is available for reading, or the socket has been shutdown with option RSocket::EStopInput.
@param aLen For non-datagram sockets, on return, a length which indicates how 
much data was read. This is the same as length of the returned aDesc. 
For datagram sockets, this parameter returns the number of remaining unread octets.

@capability Dependent on the type of socket so deferred to PRT */
	{
	//see comment in  RSocket::Send(const TDesC8 &aBuffer,TUint someFlags,TRequestStatus &aStatus)	
	SendReceive(ESoRecv,TIpcArgs(someFlags,&aLen,&aBuffer),aStatus);

	}

EXPORT_C void RSocket::RecvOneOrMore(TDes8 &aBuffer,TUint someFlags,TRequestStatus &aStatus,TSockXfrLength &aLen)
/** Receives data from a remote host and completes when data is available. 
	
The function reads at least one byte of data, but will complete as soon as 
any data is available. The amount of data received is returned via the TSockXfrLength 
argument.

RecvOneOrMore() can only be used with stream-interfaced connected sockets; 
datagram interface sockets will return KErrNotSupported.

A socket may only have one receive operation outstanding at any one time.

@param aBuffer A descriptor where data read will be placed.
@param someFlags Flags which are passed through to protocol.
@param aStatus On completion, KErrNone if successful, otherwise one of the system 
wide error codes. Note that KErrEof indicates either that a remote connection is 
closed, and that no more data is available for reading, or the socket has 
been shutdown with option RSocket::EStopInput.
@param aLen For non-datagram sockets, on return, a length which indicates how 
much data was read. This is the same as length of the returned aDesc. For 
datagram sockets, this parameter is not used.

@capability Dependent on the type of socket so deferred to PRT */
	{
	//see comment in  RSocket::Send(const TDesC8 &aBuffer,TUint someFlags,TRequestStatus &aStatus)	
	SendReceive(ESoRecvOneOrMore,TIpcArgs(someFlags,&aLen,&aBuffer),aStatus);
	}

EXPORT_C void RSocket::RecvOneOrMore(TDes8 &aBuffer,TUint someFlags,TRequestStatus &aStatus)
/** Receives data from a remote host and completes when data is available. 
	
The function reads at least one byte of data, but will complete as soon as 
any data is available. The amount of data received is returned via the TSockXfrLength 
argument.

RecvOneOrMore() can only be used with stream-interfaced connected sockets; 
datagram interface sockets will return KErrNotSupported.

A socket may only have one receive operation outstanding at any one time.

@param aBuffer A descriptor where data read will be placed.
@param someFlags Flags which are passed through to protocol.
@param aStatus On completion, KErrNone if successful, otherwise one of the system 
wide error codes. Note that KErrEof indicates either that a remote connection is 
closed, and that no more data is available for reading, or the socket has 
been shutdown with option RSocket::EStopInput.

@capability Dependent on the type of socket so deferred to PRT */
	{
	SendReceive(ESoRecvOneOrMoreNoLength,TIpcArgs(someFlags,aBuffer.MaxLength(),&aBuffer),aStatus);
	}

EXPORT_C void RSocket::Read(TDes8 &aBuffer,TRequestStatus &aStatus)
/** Receives data from a remote host.
	
For a stream-interfaced sockets, the function only completes when the full 
amount of requested data has been received (or the connection breaks). This 
means when the descriptor has been filled to its maximum length (not its current 
length). For a connection-oriented datagram-interface, the function completes 
when a datagram arrives even if it is not sufficient to fill the buffer.

Read() should only be used with connected sockets.

A socket may only have one receive operation outstanding at any one time.

@param aBuffer A descriptor where data read will be placed.
@param aStatus On completion, KErrNone if successful, otherwise one of the system 
wide error codes. Note that KErrEof indicates either that a remote connection is 
closed, and that no more data is available for reading, or the socket has 
been shutdown with option RSocket::EStopInput.

@capability Dependent on the type of socket so deferred to PRT */
	{
	SendReceive(ESoRead,TIpcArgs(0,aBuffer.MaxLength(),&aBuffer),aStatus);
	}

EXPORT_C void RSocket::Write(const TDesC8 &aBuffer,TRequestStatus &aStatus)
/** Sends data to a remote host.

Write() should only be used with connected sockets. 

@param aBuffer The data to be sent.
@param aStatus On completion, KErrNone if successful, otherwise one of the 
system wide error codes. Note that KErrEof indicates that the socket has been shutdown 
with option EStopOutput.

@capability Dependent on the type of socket so deferred to PRT */
	{
	SendReceive(ESoWrite,TIpcArgs(0,aBuffer.Length(),&aBuffer),aStatus);
	}


EXPORT_C void RSocket::SendTo(const TDesC8 &aBuffer,TSockAddr &anAddr,TUint someFlags,TRequestStatus &aStatus)
/** Sends data to a remote host through a (possibly) unconnected socket to a specified destination 
address.
	
Flags are provided to add protocol specific information. The length of the 
descriptor indicates the amount of data to be sent. A socket may only have 
one send operation in progress at any one time.

@param aBuffer The data to be sent.
@param anAddr A remote destination address for unconnected sends
@param someFlags Flags which are passed through to protocol
@param aStatus On completion, KErrNone if successful, otherwise one of the 
system wide error codes. Note that KErrEof indicates that the socket has been shutdown 
with option EStopOutput.

@capability Dependent on the type of socket so deferred to PRT */
	{
	//see comment in  RSocket::Send(const TDesC8 &aBuffer,TUint someFlags,TRequestStatus &aStatus)
	SendReceive(ESoSendToNoLength,TIpcArgs(someFlags,&anAddr,&aBuffer),aStatus);
	}

EXPORT_C void RSocket::RecvFrom(TDes8 &aBuffer,TSockAddr &anAddr,TUint someFlags,TRequestStatus &aStatus)
/** Receives data from a remote host through a (possibly) unconnected socket and returns a source address.

Flags are provided to add protocol specific information. 

A socket may only have one receive operation outstanding at any one time.

@param aDesc A descriptor where data read will be placed.
@param anAddr A remote source address for unconnected receives. Returns KAfInet6 in TSockAddr::Family() 
for IPv4 ICMP packets. Returns KAfInet for IPv4 TCP and UDP sockets.
@param flags Flags which are passed through to protocol.
@param aStatus On completion, KErrNone if successful, otherwise one of the system wide error codes. 
Note that KErrEof indicates either that a remote connection is 
closed, and that no more data is available for reading, or the socket has 
been shutdown with option RSocket::EStopInput.

@capability Dependent on the type of socket so deferred to PRT */
	{
	//see comment in  RSocket::Send(const TDesC8 &aBuffer,TUint someFlags,TRequestStatus &aStatus)	
	SendReceive(ESoRecvFromNoLength,TIpcArgs(someFlags,&anAddr,&aBuffer),aStatus);
	}

EXPORT_C void RSocket::SendTo(const TDesC8 &aBuffer,TSockAddr &anAddr,TUint someFlags,TRequestStatus &aStatus,TSockXfrLength &aLen)
/** Sends data to a remote host through a (possibly) unconnected socket to a specified destination 
address. 

Flags are provided to add protocol specific information. The length 
of the descriptor indicates the amount of data to be sent. A socket may only 
have one send operation in progress at any one time. The TSockXfrLength argument 
will return the amount of data sent.

@param aBuffer The data to be sent.
@param anAddr A remote destination address for unconnected sends
@param someFlags Flags which are passed through to protocol
@param aStatus On completion, KErrNone if successful, otherwise one of the system 
wide error codes. Note that KErrEof indicates that the socket has been shutdown 
with option EStopOutput. In the case of KSONonBlockingIO & writeFlowedoff, 
it has to return KerrWouldBlock, but it returns KErrNone for compatibiity
@param aLen Filled in with amount of data sent before completion

@capability Dependent on the type of socket so deferred to PRT */
	{
	aLen = someFlags;

	//see comment in  RSocket::Send(const TDesC8 &aBuffer,TUint someFlags,TRequestStatus &aStatus)
	SendReceive(ESoSendTo,TIpcArgs(&aLen,&anAddr,&aBuffer),aStatus);
	}

EXPORT_C void RSocket::RecvFrom(TDes8 &aBuffer,TSockAddr &anAddr,TUint someFlags,TRequestStatus &aStatus,TSockXfrLength &aLen)
/** Receives data from a remote host through a (possibly) unconnected socket where a source address 
is returned.

Flags are provided to add protocol specific information. 

A socket may only have one receive operation outstanding at any one time.
	
@param aBuffer A descriptor where data read will be placed
@param anAddr A remote source address for unconnected receives. Returns KAfInet6 in TSockAddr::Family() 
for IPv4 ICMP packets. Returns KAfInet for IPv4 TCP and UDP sockets.
@param someFlags Flags which are passed through to protocol
@param aStatus On completion, KErrNone if successful, otherwise one of the system wide error codes. 
Note that KErrEof indicates either that a remote connection is 
closed, and that no more data is available for reading, or the socket has 
been shutdown with option RSocket::EStopInput.
@param aLen For non-datagram sockets, on return, a length which indicates how 
much data was read. This is the same as length of the returned aDesc. For 
datagram sockets, this parameter is not used.

@capability Dependent on the type of socket so deferred to PRT */
	{
	aLen = someFlags;

	//see comment in  RSocket::Send(const TDesC8 &aBuffer,TUint someFlags,TRequestStatus &aStatus)	
	SendReceive(ESoRecvFrom,TIpcArgs(&aLen,&anAddr,&aBuffer),aStatus);
	}

EXPORT_C void RSocket::Connect(TSockAddr &anAddr,TRequestStatus &aStatus)
//
// Start a socket connecting ("active open")
//
/** Connects to a remote host asynchronously. 

The address provided specifies the address of the remote host. 

A socket may only have one connect operation outstanding at 
any one time. Once the connect is completed, the socket is ready to send 
or receive data. If a socket is unbound - i.e. Bind() has not been called 
yet - then it will automatically have a local address allocated. 

Connect() is always required for protocols which do not have the KSIConnectionLess 
flag in their protocol information. If a protocol has the KSIConnectionLess 
flag, then Connect() may be used to set the address for all data sent from 
the socket, in which case Send()/Write() may be used in addition to SendTo().

To use data in connection a protocol must have the flag KSIConnectData in 
its protocol information.

To cancel a connect use CancelConnect().

@param anAddr Address of remote host.
@param aStatus On completion, will contain an error code, see the system-wide 
error codes.

@capability Dependent on the type of socket so deferred to PRT */
	{
	LOG( ESockLog::Printf(_L8("RSocket %08x:\tConnect(port=%d): tid %d"), this, anAddr.Port(), (TUint)RThread().Id() ) );

	SendReceive(ESoConnect,TIpcArgs(&anAddr, NULL, NULL),aStatus);
	}

EXPORT_C void RSocket::Connect(TSockAddr &anAddr,const TDesC8 &aConnectDataOut,TDes8 &aConnectDataIn,TRequestStatus &aStatus)
/** Connects to a remote host asynchronously. 

The address provided specifies the address of the 
remote host. 

Some protocols allow data to be sent in connect request packets 
which may be provided in the data-out descriptor. Some protocols may allow 
data to be sent in connect responses which may be collected in the data-in 
descriptor. 

A socket may only have one connect operation outstanding at any 
one time. Once the connect is completed, the socket is ready to send or receive 
data. If a socket is unbound - i.e. Bind() has not been called yet - then 
it will automatically have a local address allocated. 

Connect() is always required for protocols which do not have the KSIConnectionLess 
flag in their protocol information. If a protocol has the KSIConnectionLess 
flag, then Connect() may be used to set the address for all data sent from 
the socket, in which case Send()/Write() may be used in addition to SendTo().

To use data in connection a protocol must have the flag KSIConnectData in 
its protocol information.

To cancel a connect use CancelConnect().

@param anAddr Address of remote host.
@param aDataOut A descriptor containing data to be sent.
@param aDataIn A descriptor to receive data.
@param aStatus On completion, will contain an error code. KErrBadHandle if the socket has already been
 closed by the client; KErrAlreadyExists if the socket is already connected and it isn't a blocking 
 connection; otherwise one of the system-wide error codes.
@panic EConnectingAlready if the socket is connection-oriented and a blocking connection is already in progress for the socket

@capability Dependent on the type of socket so deferred to PRT */
	{
	SendReceive(ESoConnect,TIpcArgs(&anAddr,&aConnectDataOut,&aConnectDataIn),aStatus);
	}
		
EXPORT_C TInt RSocket::Bind(TSockAddr &anAddr)
/** Sets the local address of a socket. 

When a socket is opened it has no name associated with it, and binding is 
  required so data can be routed to the socket. 


Bind() should be called before Listen() or Connect(). The address supplied 
should be a derived class specific to the particular protocol the socket was 
opened on.

@param anAddr Desired local address of socket.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	const TInt ret = SendReceive(ESoBind,TIpcArgs(&anAddr));
	LOG( ESockLog::Printf(_L8("RSocket %08x:\tBind(family=0x%04x, port=%d): client %d, ret=%d"),
			this, anAddr.Family(), anAddr.Port(), (TUint)RThread().Id(), ret));
	return ret;
	}

EXPORT_C TInt RSocket::SetLocalPort(TInt aPort)
/** Sets the local port of a socket. Setting the local port is equivalent to calling 
Bind() with only the port set in the address.

@param aPort Desired local port of socket.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TSockAddr addr;
	addr.SetPort(aPort);
	return Bind(addr);
	}

EXPORT_C void RSocket::Accept(RSocket& aBlankSocket,TRequestStatus& aStatus)
//
// Wait for incoming connections - accept a connection after a "passive open".
//
/** Facilitates a client/server connection from a remote socket. 

Extracts the first pending connection on a queue of sockets, the queue size being 
previously specified by Listen(). On successful completion the blank socket is given 
the handle of the new socket and it may then be used to transfer data. After 
completion the accept socket may be used to make further connections with 
new blank sockets (see Open() on how to open a blank socket). 

Accept() may be used for protocols which do not have the KSIConnectionLess 
flag in their protocol information.

@param aBlankSocket A socket opened as a blank socket.
@param aStatus On completion, will contain an error code: see the system-wide 
error codes.

@capability Dependent on the type of socket so deferred to PRT */
	{
	SendReceive(ESoAccept,TIpcArgs(NULL,aBlankSocket.SubSessionHandle(),NULL),aStatus);
	}

EXPORT_C void RSocket::Accept(RSocket& aBlankSocket,TDes8 &aConnectData,TRequestStatus& aStatus)
//
// Wait for incoming connections - accept a connection after a "passive open".
//
/** Facilitates a client/server connection from a remote socket. 

Extracts the first pending connection on a queue of sockets, the queue size being 
previously specified by Listen(). On successful completion the blank socket is given 
the handle of the new socket and it may then be used to transfer data. After 
completion the accept socket may be used to make further connections with 
new blank sockets (see Open() on how to open a blank socket). 

This variant provides an additional descriptor argument to receive data
which may have been sent in a connect request. If there is a pending connection
in the listen queue when Accept() is called, the call will
complete immediately. Otherwise it will wait until a socket becomes available
in the queue and complete asynchronously.

Accept() may be used for protocols which do not have the KSIConnectionLess 
flag in their protocol information.

To receive data-in accepting, a protocol must have the flag KSIConnectData 
in its protocol information.

@param aBlankSocket A socket opened as a blank socket.
@param aConnectData Data which may have been received in connection.
@param aStatus On completion, will contain an error code: see the system-wide 
error codes.

@capability Dependent on the type of socket so deferred to PRT */
	{
	SendReceive(ESoAccept,TIpcArgs(NULL,aBlankSocket.SubSessionHandle(),&aConnectData),aStatus);
	}

EXPORT_C TInt RSocket::Listen(TUint aQSize)
//
// Set up a socket for "passive open"
//
/** Sets up a socket to listen for incoming connections. 

Before calling this procedure a socket should be opened on a specific protocol 
using Open() and the socket should be bound to a local address using Bind(). 

Listen() creates a queue to hold incoming connections which can be married with 
blank sockets using Accept(). The call also allows data to be sent back to 
connecting peers if a protocol allows data to be passed in connect responses. 
Once a listen queue has been created it will continue to allow peers to connect 
until it is full, at which point it will reject any incoming connections as 
specified by protocol behaviour. When a socket is accepted by the client a space 
is made available in the queue.

@param aQSize Size of listen queue.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	return SendReceive(ESoListen,TIpcArgs(aQSize,NULL));
	}

EXPORT_C TInt RSocket::Listen(TUint aQSize,const TDesC8 &aConnectData)
//
// Set up socket for "passive open"
//
/** Sets up a socket to listen for incoming connections. 

Before calling this procedure a socket should be opened on a specific protocol 
using Open() and the socket should be bound to a local address using Bind(). 

Listen() creates a queue to hold incoming connections which can be married with 
blank sockets using Accept(). The call also allows data to be sent back to 
connecting peers if a protocol allows data to be passed in connect responses. 
Once a listen queue has been created it will continue to allow peers to connect 
until it is full, at which point it will reject any incoming connections as 
specified by protocol behaviour. When a socket is accepted by the client a space 
is made available in the queue.

To use data-in listening, a protocol must have the flag KSIConnectData in 
its protocol information.

@param aQSize Size of listen queue.
@param aDataOut A descriptor containing data to be sent in connect responses.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	return SendReceive(ESoListen,TIpcArgs(aQSize,&aConnectData,aConnectData.Length()));
	}

EXPORT_C TInt RSocket::SetOpt(TUint anOptionName,TUint anOptionLevel,const TDesC8& anOption /*=TPtrC(NULL,0)*/)
/** Sets a socket option. The socket server has options which are generic to all 
sockets and protocols may add specific options.

Options available for all protocols can be set with anOptionLevel set to KSOLSocket. 
See individual protocol notes for other socket options. 

@param anOptionName An integer constant which identifies an option.
@param anOptionLevel An integer constant which identifies level of an option: 
i.e. an option level groups related options together.
@param anOption Option value packaged in a descriptor.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 

@capability Dependent on the type of operation so deferred to PRT.  See documentation
of constant values used in aOptionName and aOptionLevel for more information */
	{
	//return SendReceive(ESoSetOpt,TIpcArgs(anOptionName,&anOption,anOption.Length(),anOptionLevel));
	return SendReceive(ESoSetOpt,TIpcArgs(anOptionName,&anOption,anOptionLevel));

	}

EXPORT_C TInt RSocket::SetOpt(TUint anOptionName,TUint anOptionLevel,TInt anOption)
/** Sets a socket option. The socket server has options which are generic to all 
sockets and protocols may add specific options.

Options available for all protocols can be set with anOptionLevel set to KSOLSocket. 
See individual protocol notes for other socket options. 

@param anOptionName An integer constant which identifies an option.
@param anOptionLevel An integer constant which identifies level of an option: 
i.e. an option level groups related options together.
@param anOption Option value as an integer.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 

@capability Dependent on the type of operation so deferred to PRT.  See documentation
of constant values used in aOptionName and aOptionLevel for more information */
	{

	TPtr8 optionDes((TUint8*)&anOption,sizeof(TInt),sizeof(TInt));
	return SetOpt(anOptionName,anOptionLevel,optionDes);
	}

EXPORT_C TInt RSocket::GetOpt(TUint anOptionName,TUint anOptionLevel,TDes8& anOption)
/** Gets a socket option. The socket server has options which are generic to all 
sockets and protocols may add specific options.

Options available for all protocols can be got with anOptionLevel set to KSOLSocket. 
See individual protocol notes for other socket options. 

@param anOptionName An integer constant which identifies an option.
@param anOptionLevel An integer constant which identifies level of an option.
@param anOption On return, option value packaged in a descriptor.
@return KErrNone if successful, otherwise another of the system-wide error 
codes.

@capability Dependent on the type of operation so deferred to PRT.  See documentation
of constant values used in aOptionName and aOptionLevel for more information */
	{
	//return SendReceive(ESoGetOpt,TIpcArgs(anOptionName,&anOption,anOption.MaxLength(),anOptionLevel));
	return SendReceive(ESoGetOpt,TIpcArgs(anOptionName,&anOption,anOptionLevel));
	}

EXPORT_C TInt RSocket::GetOpt(TUint anOptionName,TUint anOptionLevel,TInt& anOption)
/** Gets a socket option. The socket server has options which are generic to all 
sockets and protocols may add specific options.

Options available for all protocols can be got with anOptionLevel set to KSOLSocket. 
See individual protocol notes for other socket options. 

@param anOptionName An integer constant which identifies an option.
@param anOptionLevel An integer constant which identifies level of an option.
@param anOption On return, option value as an integer.
@return KErrNone if successful, otherwise another of the system-wide error 
codes.

@capability Dependent on the type of operation so deferred to PRT.  See documentation
of constant values used in aOptionName and aOptionLevel for more information */
	{
	TPtr8 optionDes((TUint8*)&anOption,sizeof(TInt),sizeof(TInt));
	return GetOpt(anOptionName,anOptionLevel,optionDes);
	}

EXPORT_C void RSocket::Ioctl(TUint aCommand,TRequestStatus& aStatus,TDes8* aDesc /*=NULL*/,TUint aLevel /*=KLevelUnspecified*/)
/** Applies an asynchronous I/O control operation on a socket. Data may be passed and 
received if a descriptor address is provided as an argument. Only one Ioctl() 
operation may be outstanding for each socket.

Commands available for all protocols can be set withaLevel set to KSOLSocket. 
See individual protocol notes for other commands. 

@param aCommand Ioctl command.
@param aStatus On completion, will contain an error code: see the system-wide 
error codes.
@param aDesc Pointer to a descriptor in which data may be sent and received 
on completion
@param aLevel Control operation level

@capability Dependent on the type of operation so deferred to PRT.  See documentation
of constant values used in aOptionName and aOptionLevel for more information */
	{
	SendReceive(ESoIoctl,TIpcArgs(aCommand,aDesc,aLevel),aStatus);
	}

EXPORT_C TInt RSocket::GetDisconnectData(TDes8 &aDesc)
/** Gets data for use in the disconnection of a socket, namely the remote name of 
the connected socket.

This data has been received in a protocol disconnect message.

To use the data in disconnection, a protocol must have the flagKSIConnectData in 
its protocol information.

@param aDesc A descriptor to receive data.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	return SendReceive(ESoGetDiscData,TIpcArgs(&aDesc));
	}

EXPORT_C void RSocket::LocalName(TSockAddr &anAddr)
/** Gets the local address of a bound socket. 

The local address is set either by calling Bind(), or is automatically set 
when Connect() is called. 

If a socket is created through Accept() then a socket will inherit the port 
of its parent unless otherwise specified by a protocol's behaviour. 

Depending on a protocol implementation, additional information may be gained 
through this call.

@param anAddr Local address which is filled in on return. */
	{
	SendReceive(ESoGetLocalName,TIpcArgs(&anAddr));
	}

EXPORT_C TUint RSocket::LocalPort()
/** Gets the local port number of a bound socket. 

Getting the local port is similar to getting the local name.

@see LocalName() for a description.
@return The local port of a socket. */
	{
	TSockAddr addr;
	LocalName(addr);
	return addr.Port();
	}
	
EXPORT_C void RSocket::RemoteName(TSockAddr &anAddr)
/** Gets the remote name of a connected socket. 

The remote name of a socket is associated with the remote host a socket is 
connected to. The remote name is only valid for a connected socket. A socket 
is either connected through calling Connect() or Accept().

@param anAddr Remote address which is filled in on return. */
	{
	SendReceive(ESoGetRemoteName,TIpcArgs(&anAddr));
	}

EXPORT_C void RSocket::CancelSend()
/** Cancels an outstanding Send() operation. 

Calling the function will cause any outstanding send operation to complete 
prematurely. The state of a socket after a send is cancelled is defined by 
the characteristics of the protocol. */
	{
	SendReceive(ESoCancelSend,TIpcArgs());
	}

EXPORT_C void RSocket::CancelRecv()
/** Cancels an outstanding Recv() operation. 

Calling this function will cause any outstanding receive operation to complete 
prematurely. The state of a socket after a receive is cancelled is defined 
by the characteristics of the protocol. */
	{
	SendReceive(ESoCancelRecv,TIpcArgs());
	}

EXPORT_C void RSocket::CancelRead()
/** Cancels an outstanding Read() operation. 

Calling this function will cause any outstanding Read() operation to complete 
prematurely. The state of a socket after a receive is cancelled is defined 
by the characteristics of the protocol. */
	{
	SendReceive(ESoCancelRecv,TIpcArgs());
	}

EXPORT_C void RSocket::CancelWrite()
/** Cancels an outstanding Write() operation. 

Calling the function will cause any outstanding Write() operation to complete 
prematurely. The state of a socket after a send is cancelled is defined by 
the characteristics of the protocol. */
	{
	SendReceive(ESoCancelSend,TIpcArgs());
	}

EXPORT_C void RSocket::CancelConnect()
/** Cancels an outstanding connect operation. 

Will cause any outstanding connect operation to complete prematurely.

The state of a socket after a connect is cancelled is defined by the characteristics 
of the protocol. */
	{
	SendReceive(ESoCancelConnect,TIpcArgs());
	}

EXPORT_C void RSocket::CancelIoctl()
/** Cancels an outstanding Ioctl() operation. 

Will cause any outstanding Ioctl operation to complete prematurely. 

The state of a socket after a connect is cancelled is defined by the characteristics 
of the protocol. */
	{
	SendReceive(ESoCancelIoctl,TIpcArgs());
	}


EXPORT_C void RSocket::CancelAccept()
/** Cancels an outstanding accept operation. 

Will cause any outstanding accept operation to complete prematurely. */
	{
	SendReceive(ESoCancelAccept,TIpcArgs());
	}


EXPORT_C void RSocket::CancelAll()
/** Cancels all outstanding operations. 

Will cause all outstanding operations to complete prematurely. 

Outstanding operations for a socket include: read, write, Ioctl, connect, 
accept and shutdown. All of these operations will be completed by this call. */
	{
	SendReceive(ESoCancelAll,TIpcArgs());
	}

EXPORT_C TInt RSocket::Info(TProtocolDesc &aProtocol)
/** Gets information in the protocol description for the protocol which a socket 
is opened on.

@param aProtocol A protocol description type to hold protocol information.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TPckg<TProtocolDesc> protDesc(aProtocol);

	return SendReceive(ESoSocketInfo,TIpcArgs(&protDesc));
	}

EXPORT_C TInt RSocket::Name(TName& aName)
/** Gets a unique system name for the socket. The purpose of this is to identify 
the socket in a call to Transfer().

@param aName System name for the socket
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
    {
	return SendReceive(ESoReference,TIpcArgs(&aName));
    }

EXPORT_C TInt RSocket::Transfer(RSocketServ& aServer, const TDesC& aName)
//
// Transfer socket identified by name to this session
//
/** Transfers a socket from one socket server session to another. It creates the 
socket in the target session, and removes the socket from the source session. 
The call is made on an uninitialised RSocket object. The socket system name 
is used to identify the socket to transfer. 

If the call fails, the socket that is being transferred remains with the original 
session. Success or failure can be checked on the originating socket by calling 
Info(), which returns KErrNone if the transfer failed, and KErrBadHandle if 
it succeeded.

Platsec considerations require that the source socket must set itself transferable
before any attempt to transfer the socket to the destination socket. This is done using 
a setopt in the following way

@code
		_LIT_SECURITY_POLICY_Cn(KProcPolicy, cap1,cap2,...capn);
		ret = destsock.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy().Package());
@endcode

where cap1,cap2...capn are the capabilities that the destination process MUST have in 
order to affect the transfer.

An example is:

@code
		_LIT_SECURITY_POLICY_C2(KProcPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl);
		ret = destsock.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy().Package());
@endcode

If the setOpt is not set or the destination process does not have sufficient capabilities then
the function will return KErrPermissionDenied    

@param aServer The session into which to transfer the socket.
@param aName The system name, as obtained from a call to Name(), of the socket 
that you want to transfer.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 

@capability Dependent on the capabilities defined in the setOpt by the source Socket */
	{
	return CreateSubSession(aServer, ESoTransfer, TIpcArgs(&aName));
	}
