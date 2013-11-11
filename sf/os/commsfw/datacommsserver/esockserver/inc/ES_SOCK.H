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
 @publishedAll
 @released
*/

#if !defined(__ES_SOCK_H__)
#define __ES_SOCK_H__


#include <e32base.h>
#include <nifvar.h>

#undef	_DEBUG_SOCKET_FUNCTIONS
#if defined(_DEBUG)
#define	_DEBUG_SOCKET_FUNCTIONS
#endif

#include <comms-infras/metacontainer.h>
#include <elements/sd_errors.h>
#include <comms-infras/es_parameterset.h>

#include <comms-infras/es_event.h>

using Meta::STypeId;
using Meta::SMetaDataECom;
using Meta::RMetaDataEComContainer;


// Forward declaration of CCommsDataObjectBase
namespace ESock
{
	class CCommsDataObjectBase;
}

/**
Canonical names for the core ESOCKSVR modules
*/
_LIT8(SOCKET_SERVER_MAIN_MODULE_NAME, "ESock_Main");		// Worker 0
_LIT8(SOCKET_SERVER_IP_MODULE_NAME, "ESock_IP");			// Worker 1
_LIT8(SOCKET_SERVER_BT_MODULE_NAME, "ESock_Bt");			// Worker 2
_LIT8(SOCKET_SERVER_IR_MODULE_NAME, "ESock_Ir");			// Worker 3
_LIT8(SOCKET_SERVER_SMSWAP_MODULE_NAME, "ESock_SmsWap");	// Worker 4


/**
Progress Notification to inform clients Connection is up
This event has the same numerical values as 
KLinkLayerOpen which is deprecated
@publishedAll
@released
*/

const TUint KConnectionUp    = 7000;    
/**
Progress Notification to inform clients Connection is up
This event has the same numerical values as 
KLinkLayerClosed which is deprecated 

@publishedAll
@released
*/

const TUint KConnectionDown  = 8000;  

      

/**
Default number of message slots.
@publishedAll
@released
*/
const TUint KESockDefaultMessageSlots=8;


/**
Size of Maximum SubConnection event

@publishedAll
@released
@note If you allocate this on the heap, remember to delete through the pointer to the buffer and not any pointers to the events held inside it
if you change this value, you will alter the function signature and break the .def file
*/
const TUint KMaxSubConnectionEventSize = 512;

/**
SubConnection Unique Id

THIS API IS TO BE DEPRECATED

@publishedPartner
@released
*/
typedef TUint TSubConnectionUniqueId;

/**
Buffer for  notification of any change in the state of  SubConnection.

THIS API IS TO BE DEPRECATED

@publishedPartner
@released
*/
typedef TBuf8<KMaxSubConnectionEventSize> TSubConnectionNotificationBuf;
//
const TUint KUseEmbeddedUniqueId = 0; //< Used by RConnection to identify cases where the subconnection id is embedded in the data structure

const TUint KConnProgressDefault = 0; //< Default Connection Progress

class TSessionPref
/**
Hint to the Socket Server on what will be the principal usage of the connection.
It is by no means a restriction but may result in better performance for the session.

@publishedAll
@released
*/
	{
public:
    IMPORT_C TSessionPref();
public:
	/** The protocol's address family. For example, for TCP/IP protocols, KAfInet. */
	TUint iAddrFamily;
	/** The protocol type. */
	TUint iProtocol;
	/** Reserved. */
	TUint iReserved;
	/** Reserved. */
	TUint iReserved1;
	/** Reserved. */
	TUint iReserved2;
	/** Reserved. */
	TUint iReserved3;
	/** Reserved. */
	TUint iReserved4;
	};

class TNifProgress
/**
Contains progress information on a dial-up connection

@publishedAll
@released
*/
	{
public:
	inline TNifProgress();
	inline TNifProgress(TInt aStage, TInt aError);
	inline TBool operator==(const TNifProgress& aRHS) const;
	TInt iStage;
	TInt iError;
	};

class TNifAgentInfo
/**
Contains information describing an agent

@publishedAll
@released
*/
	{
public:
	/**	This class Contains version information	*/
	TVersion iVersion;

	/**	This class Defines a modifiable buffer descriptor that can contain the name of a reference counting object	*/
	TName iName;
	};

/**
Buffer for Network Interface Progress

@publishedAll
@released
*/
typedef TPckgBuf<TNifProgress> TNifProgressBuf;

/**
Socket address offsets.

@publishedAll
@released
*/
struct SSockAddr
/** Socket address offsets.

This class defines the internal offsets of data members for the TSockAddr
class. */
	{
	/** Address family of socket address */
	TUint iFamily;
	/** Port (or equivilent) number */
	TUint iPort;
	};

/**
Maximum sockets address size.
@publishedAll
@released
*/
const TUint KMaxSockAddrSize=0x20;

class TSockAddr : public TBuf8<KMaxSockAddrSize>
/** Represents an end point address.

Protocols interpret the class within the socket server to route packets and
form connections. It can be used on its own or as derived by protocols. The
SSockAddr class acts as an offset map for the TSockAddr class which has a
family field and a port field. The family field may be used by protocols to
"up-cast" the base-class to the correct derived class. The port field is provided
because it is a common practice for protocols to use port equivalents in addressing.

Writing derived classes:

The two protected member functions allow further data members to be defined
past the area of the base SSockAddr. In any derived constructor the length
of the descriptor should be set to the length of the valid data contained
in the address. Subsequent member function calls should also adjust the length
if it affects valid data.
@publishedAll
@released */
	{
public:
	IMPORT_C TSockAddr();
	IMPORT_C TSockAddr(TUint aFamily);
	IMPORT_C TUint Family() const;
	IMPORT_C void SetFamily(TUint aFamily);
	IMPORT_C TUint Port() const;
	IMPORT_C void SetPort(TUint aPort);
	IMPORT_C TBool CmpPort(const TSockAddr& anAddr) const;
	IMPORT_C TInt GetUserLen();
protected:
	IMPORT_C void SetUserLen(TInt aLen);
	inline TUint8* UserPtr() const;
private:
	inline SSockAddr* BasePtr() const;
	};

// Address families (based upon protocol IDs used by ARP/REVARP)
/** Default (unspecified) protocol module.
@publishedAll
@released */
const TUint KAFUnspec=0;

// Socket types
/** Stream socket.
@publishedAll
@released */
const TUint KSockStream=1;
/** Datagram socket. */
const TUint KSockDatagram=2;
/** Datagrams with sequence numbers. */
const TUint KSockSeqPacket=3;
/** Raw socket. */
const TUint KSockRaw=4;

// constants for various socket calls - can't be enums cos they're largely defined by protocols
// The following constants are used to define level parameters for RSocket::Ioctl(), RSocket::GetOpt()
// and RSocket::SetOpt().
/** Generic socket options/commands.
@publishedAll
@released */
const TInt KSOLSocket=1;
/** Unspecified level. */
const TInt KLevelUnspecified=0;

// Socket options defined by the server.
// Used through RSocket::SetOpt(), and RSocket::GetOpt() with anOptionLevel set
// to KSOLSocket. Options can be both get and set unless otherwise.
// stated.
// Notes:
// Setting the send and receive buffer sizes explicitly can help to reduce
// the memory requirements if many data sockets are used in an application. If a
// socket is datagram-oriented and its receive/send buffer size is set
// to KSocketBufSizeUndefined, then the initial buffer size will
// be KSocketDefaultBufferSize and buffers will grow to accommodate
// larger sends/receives. If the buffer size is set explicitly for
// datagram-oriented sockets, sends which exceed the set size will fail
// with KErrTooBig and receives will be truncated. For stream based
// sockets there should be no noticeable effect on client reads, unless the buffer
// size is set to a prohibitively low value.
/** Debugging enabled or disabled . Values are:

(TInt)0. Disabled

(TInt)1. Enabled
@publishedAll
@released */
const TUint KSODebug=1;
/** Socket receive buffer size. Values are:

KSocketBufSizeUndefined

1 to KMaxTUint: explicit buffer size, supplied as a TPckgBuf<TUint> */
const TUint KSORecvBuf=2;
/** Socket send buffer size. Values are:

KSocketBufSizeUndefined

1 to KMaxTUint: explicit buffer size, supplied as a TPckgBuf<TUint> */
const TUint KSOSendBuf=3;
/** Socket nonblocking mode. To set, no option values are required. For getting,
values are:

(TInt)0. Disabled

(TInt)1. Enabled */
const TUint KSONonBlockingIO=4;
/** Socket blocking mode. To set, no values are required. For getting, values are:

(TInt)0. Disabled

(TInt)1. Enabled */
const TUint KSOBlockingIO=5;
/** Getting only: gets a bitmask of flags describing the read/write/exception status
of the socket. Value is a TInt containing a bitmask of socket status (KSockSelectExcept
etc.) constants. */
const TUint KSOSelectPoll=6;
/** Getting only: retrieve the number of bytes currently available for reading.
Value is a TInt. */
const TUint KSOReadBytesPending=7;
/** Getting only: retrieve the urgent data offset (only for stream protocols that
support urgent data).
Value is a TInt. */
const TUint KSOUrgentDataOffset=8;
/** 
Getting only: retrieves the last error. 
Errors are normally reported by the called method and behaviour is protocol dependent.
KSOSelectLastError does not return such errors.
Value is a TInt. */
const TUint KSOSelectLastError=9;


/**
Setting only. Enables socket to be transferred to the process with given capabilities.
The capabilities set should be supplied as TPckgBuf<TSecurityPolicy>.
Each RSocket::Transfer() call must be enabled by setting this option.
@publishedAll
@released */
const TUint KSOEnableTransfer = 10;

/** Setting only. Disables a socket's possibility to be transferred. No option required.
@publishedAll
@released */

const TUint KSODisableTransfer = 11;

// The following constants relating to buffer sizes are defined

/** Use default buffer size. */
const TInt  KSocketBufSizeUndefined=0;
/** Default buffer size. */
const TInt  KSocketDefaultBufferSize=4096;

//internal
const TUint KSocketInternalOptionBit=0x80000000;	//< Must not be set for client requests


/** The aDesc parameter of RSocket::Ioctl() specifies a TUint containing a bitmask
of Socket status constants. The completion status will be the subset of those
conditions which is now true for the socket.

Used through RSocket::Ioctl(), with aLevel set to KSOLSocket.

@see KSOSelectPoll parameter to RSocket::GetOpt(), which allows the
current select state of the socket to be read synchronously, and KSOSelectLastError,
which returns the error code. */
const TUint KIOctlSelect=1;

// Select ioctl states
// Socket status. See also KSOSelectLastError and KSOSelectPoll.
/** Data is available to be read; for listening sockets, a connect is pending.
@publishedAll
@released */
const TUint KSockSelectRead   =0x00000001;
/** Writing to the socket is not currently blocked by flow-control. */
const TUint KSockSelectWrite  =0x00000002;
/** An error has occurred. */
const TUint KSockSelectExcept =0x00000004;
/** Include tail of prior read datagram as available data (ie indicates next read will be with read continuation) */
const TUint KSockSelectReadContinuation = 0x01000000;

// Socket write flags
/** The data to be sent is urgent and is given a higher priority than ordinary data in the send queue.
KSockWriteUrgent may only be provided as a flag to Send() if the protocol's information flag is marked with
KSIUrgentData, otherwise Send() will return with KErrNotSupported. AKA: Out of band or unit data.
@publishedAll
@released */
const TUint KSockWriteUrgent=0x00000001;
/** Must not be set for client requests. */
const TUint KSocketInternalWriteBit=0x80000000;
/** The top 8 bits are reserved for system purposes; protocols must not define these bits. */
const TUint KSockWriteSystemMask=0xFF000000;

// Socket read flags
/** Read data without consuming it, data remains in the receive queue. KSockReadPeek may only be provided
as a flag to Recv() if the protocol's information flag is marked with KSIPeekData, otherwise Recv() will
return with KErrNotSupported.
@publishedAll
@released
*/
const TUint KSockReadPeek=0x00000001;
/** Must not be set for client requests. */
const TUint KSocketInternalReadBit=0x80000000;
/** Read from datagram in a stream-like fashion (not discarding tails). */
const TUint KSockReadContinuation = 0x01000000;
// Types

/** Used in structure TProtocolDesc to describes the endianness of a protocol.
@publishedAll
@released */
enum TByteOrder
	{
	/** Big endian */
	EBigEndian,
	/** Little endian */
	ELittleEndian,
	/** Other byte order */
	EOtherByteOrder,
	};

//
// Protocol Service information bitmasks.
// The following constants are defined for
// TServerProtocolDesc and TProtocolDesc
//
/** The protocol is connectionless.
@publishedAll
@released */
const TUint KSIConnectionLess=0x00000001;
/** The protocol is reliable. */
const TUint KSIReliable=0x00000002;
/** The protocol guarantees in-order delivery. */
const TUint KSIInOrder=0x00000004;
/** The protocol is message based. */
const TUint KSIMessageBased=0x00000008;
/** The same as message based. */
const TUint KSIDatagram=KSIMessageBased;
/** The protocol is stream based. */
const TUint KSIStreamBased=0x00000010;
/** The protocol supports a stream like interface but maintains datagram boundaries. */
const TUint KSIPseudoStream=0x00000020;
/** The protocol offers an expedited data service. */
const TUint KSIUrgentData=0x00000040;
/** The protocol can send user data on a connection request. */
const TUint KSIConnectData=0x00000080;
/** The protocol can send user data on a disconnect request. */
const TUint KSIDisconnectData=0x00000100;
/** The protocol supports broadcast addresses. */
const TUint KSIBroadcast=0x00000200;
/** The protocol supports point to multi-point connections. */
const TUint KSIMultiPoint=0x00000400;
/** The protocol supports a quality of service metric. */
const TUint KSIQOS=0x00000800;
/** The protocol is write only. */
const TUint KSIWriteOnly=0x00001000;
/** The protocol is read only. */
const TUint KSIReadOnly=0x00002000;
/** The protocol supports graceful close. */
const TUint KSIGracefulClose=0x00004000;
/** The same socket can be reconnected if it disconnects (for whatever reason). */
const TUint KSICanReconnect=0x00008000;
/** Protocol supports peeking (looking at the data without removing it from the
protocol). */
const TUint KSIPeekData=0x00010000;
/** Protocol is to be informed of the identity of the client (i.e. process ID,
thread ID and UID) of each SAP (i.e. Socket Service Provider) created.
Note that this value has no meaningful interpretation on the client side.
@see KSoOwnerInfo and TSoOwnerInfo */
const TUint KSIRequiresOwnerInfo=0x00020000;	// SetOption(KSoOwnerInfo) invoked on each SAP
/** @internalTechnology */
const TUint KSIReserved=0xFFFC0000;

//
// Naming service constants
// The following constants are defined for
// TServerProtocolDesc and TProtocolDesc
//
/** Protocol supports resolving human readable entity names into network addresses
(like DNS).
@publishedAll
@released */
const TUint KNSNameResolution=0x00000001;
/** Network naming is hierarchical. */
const TUint KNSHierarchicalNaming=0x00000002;
/** @deprecated Use KNSHierarchicalNaming instead. */
const TUint KNSHeirarchicalNaming=KNSHierarchicalNaming;
/** Addressing is dynamic and should be attempted every time before connecting
(like IrDA). */
const TUint KNSRemoteDiscovery=0x00000004;
/** Protocol supports service name to port number resolution. (For example, you
can look up TCP to get port 48.) */
const TUint KNSServiceResolution=0x00000008;
/** Protocol supports additions to the name database. */
const TUint KNSNameRegistration=0x00000010;
/** Protocol supports additions to the service database. */
const TUint KNSServiceRegistration=0x00000020;
/** Addressing is dynamic - i.e. name to address mapping may change (like IrDA
which randomly chooses machine addresses.) */
const TUint KNSDynamicAddressing=0x00000040;
/** Protocol has another database which is defined by the protocol. */
const TUint KNSInfoDatabase=0x00000080;
/** Protocol may request Socket Server to startup a connection on its behalf (via
the KErrCompletion error code)
Note that this value has no meaningful interpretation on the client side.
*/
const TUint KNSRequiresConnectionStartup=0x00000100;
/** @internalTechnology */
const TUint KNSReserved=0xFFFFFE00;

// Security Schemes
// The following constants are defined for
// TServerProtocolDesc and TProtocolDesc
/** No security
@publishedAll
@released */
const TUint KSocketNoSecurity=0x00000000;
/** Secure Sockets Layer.

@see CSecureSocket */
const TUint KSecureSockets=0x00000001;

// Special message sizes
// The following constants are defined for
// TServerProtocolDesc and TProtocolDesc
/** Reads and writes can be of any size: the data is treated as a stream.
@publishedAll
@released */
const TInt KSocketMessageSizeIsStream=0;
/** Depends on lower layer or is dynamic. */
const TInt KSocketMessageSizeUndefined=1;
/** Data is packet-oriented but packets can be of any size (i.e. the remote end
must specify a Read of the same size as your Write, but there is no limit
on this size.) */
const TInt KSocketMessageSizeNoLimit=-1;


/** Undefined socket type.
@publishedAll
@released */
const TUint KUndefinedSockType=0xFFFFFFFF;


/** Undefined socket type.
Undefined Protocol
@released */
const TUint KUndefinedProtocol=0xFFFFFFFE;

/** Undefined address family
@publishedPartner
@released
 */
const TUint KUndefinedAddressFamily = 0;

/** Contains the name of a protocol in structure TProtocolDesc.
@publishedAll
@released */
typedef TBuf<0x20> TProtocolName;
/** Defines a descriptor to hold a service name string. */
typedef TBuf<0x20> TServiceName;
/** Defines a descriptor to hold a host name string. */
typedef TBuf<0x100> THostName;

/**
Socket protocol information for use by clients

@publishedAll
@released
*/
struct TProtocolDesc
/** Socket protocol information for use by clients. */
	{
public:
	/** The name of the protocol

	@see TProtocolName */
	TProtocolName iName;
	/** An integer identifying the protocol's address family. For example, for TCP/IP
	protocols, KAfInet. */
	TUint iAddrFamily;
	/** An integer specifying the socket type. For example, for TCP, KSockStream. */
	TUint iSockType;
	/** An integer specifying the specific protocol. For example, for TCP, KProtocolInetTcp. */
	TUint iProtocol;
	/** The version of the protocol

	@see TVersion */
	TVersion iVersion;
	/** The byte order that the protocol uses

	@see TByteOrder */
	TByteOrder iByteOrder;
	/** A bit mask of service information flags.

	@see KSockStream etc. */
	TUint iServiceInfo;
	/** A bit mask indicating if the protocol supports name resolution, service resolution,
	or database access services.

	@see KNSNameResolution etc. */
	TUint iNamingServices;
	/** A bitmask of security flags.

	@see KSocketNoSecurity etc. */
	TUint iSecurity;
	/** The message size of datagram protocols.

	@see KSocketMessageSizeIsStream etc. */
	TInt iMessageSize;
	};

// socket errors
/** This error is returned from operations on non-blocking sockets that cannot
be completed immediately, for example receive when no data is queued for reading.
It is a non-fatal error, and the operation should be retried later.
@publishedAll
@released */
const TInt KErrWouldBlock=-1000;

/**
socket errors

The value -17210 is taken from the range allocated for Esock (beginning at -17200)
A gap has been left between the currently existing vals and this one.

@publishedAll
@released
*/
const TInt KErrConnectionTerminated=-17210;

const TInt KErrCannotFindProtocol = Den::KErrPlayerNotFound;
const TInt KErrTierNotFound = -17212;
const TInt KErrConnectionContention = -17213;

/**
The protocol requested for the socket was recognised but was not able to be used.
This can happen with protocols that require specific settings to have been prepared
prior to the socket being opened.

@publishedAll
@released
*/ 
const TInt KErrProtocolNotReady = -17214;

// -17215 used for non published error. Do not use here.


/** Used in RSocket read and write calls to pass the length of data read and written.
@publishedAll
@released */
typedef TPckgBuf<TInt> TSockXfrLength;



namespace ESockDebug
	{
	class TControlMsg;
	}

class RSocket;
class RConnection;

NONSHARABLE_CLASS(RCommsSession) : public RSessionBase
	{
	friend class RCommsApiExtensionBase;
	friend class CCommsSessionApiExtProvider;
	};

class RSocketServ : public RCommsSession
/** Provides the Connect() function to create an IPC communication channel to the
socket server. To close the channel RHandleBase provides a RHandleBase::Close()
function.

The prime use for instances of RSocketServ is to establish subsession communications
for RSocket, RHostResolver, RNetDatabase and RConnection.  Any of the resources
which are open using the session are automatically closed when the session is
terminated, however it is more appropriate to issue a Close() on each subsession object
before closing the session.

The following operations are also provided:

NumProtocols() - enumerates the number of protocols of which the socket server
is currently aware.

GetProtocolInfo()/FindProtocol() - return information about a specific protocol.

StartProtocol() - loads a protocol asynchronously.

@note This class is not intended for user derivation.
@publishedAll
@released  */
	{
public:
    IMPORT_C RSocketServ();
	IMPORT_C TInt Connect(TUint aMessageSlots = (TUint) -1);
	IMPORT_C TInt Connect(const TSessionPref& aPref, TUint aMessageSlots = (TUint) -1);
	IMPORT_C TVersion Version() const;
	IMPORT_C TInt NumProtocols(TUint& aCount);
	IMPORT_C TInt GetProtocolInfo(TUint anIndex,TProtocolDesc& aProtocol);
	IMPORT_C TInt FindProtocol(const TProtocolName& aName,TProtocolDesc& aProtocol);
	IMPORT_C void StartProtocol(TUint anAddrFamily,TUint aSockType,TUint aProtocol,TRequestStatus& aStatus);
	IMPORT_C void StopProtocol(TUint anAddrFamily,TUint aSockType,TUint aProtocol,TRequestStatus& aStatus);
	IMPORT_C TInt InstallExtension(const TDesC& aDllName, const TDesC& aArgs=TPtrC());
	IMPORT_C void SetExclusiveMode(TRequestStatus& aStatus);
	IMPORT_C void ClearExclusiveMode();
	IMPORT_C TInt __DbgMarkHeap();
	IMPORT_C TInt __DbgCheckHeap(TInt aCount);
	IMPORT_C TInt __DbgMarkEnd(TInt aCount);
	IMPORT_C TInt __DbgFailNext(TInt aCount);
	IMPORT_C TBool __DbgCheckFailNext() const;
	IMPORT_C TInt __DbgFailNextMbuf(TInt aCount);
	IMPORT_C TInt __DbgSetMbufPoolLimit(TInt asize);
	IMPORT_C TInt __DbgCheckMbuf(TInt asize);
	IMPORT_C TInt __DbgMbufFreeSpace();
	IMPORT_C TInt __DbgMbufTotalSpace();
	IMPORT_C TInt __DbgControl(const ESockDebug::TControlMsg& aRequestMsg);
	};

NONSHARABLE_CLASS(RCommsSubSession) : public RSubSessionBase
	{
	friend class RCommsApiExtensionBase;
	friend class CCommsSubSessionApiExtProvider;
	};

class RSubConnection;
class RSocket : public RCommsSubSession
/** Provides a client endpoint to a protocol. It provides functions for socket
creation, reading, writing, passive connection, active connection, setting
addresses and querying addresses. Use this class as an endpoint for network
type communications. It provides the following services:

reading from and writing to protocol

binding to addresses

active connecting

passive connection through the listen/accept model

Before using any of these services, a connection to a socket server session
must have been made and the socket must be open.

@publishedAll
@released Since v5.0*/
	{
friend class RSocketServ;
public:
	/** Argument to RSocket::Shutdown() specifying how abruptly the shutdown occurs.
	*/
	enum TShutdown
		{
		/** Complete when socket output/input stopped. */
		ENormal,
		/** Stop socket input and complete when output is stopped. */
		EStopInput,
		/** Stop socket output and complete when input is stopped. */
		EStopOutput,
		/** Stop socket input/output and complete (abortive close). */
		EImmediate
		};
public:
	IMPORT_C RSocket();

	IMPORT_C TInt Open(RSocketServ& aServer,TUint addrFamily,TUint sockType,TUint protocol);
	IMPORT_C TInt Open(RSocketServ& aServer,TUint addrFamily,TUint sockType,TUint protocol, RConnection& aConnection);
	IMPORT_C TInt Open(RSocketServ& aServer,TUint addrFamily,TUint sockType,TUint protocol, RSubConnection& aSubConnection);
	IMPORT_C TInt Open(RSocketServ &aServer,const TDesC& aName);
	IMPORT_C TInt Open(RSocketServ& aServer);
	IMPORT_C void Send(const TDesC8& aDesc,TUint someFlags,TRequestStatus& aStatus);

	IMPORT_C void Send(const TDesC8& aDesc,TUint someFlags,TRequestStatus& aStatus,TSockXfrLength& aLen);
	IMPORT_C void CancelSend();

	IMPORT_C void Recv(TDes8& aDesc,TUint flags,TRequestStatus& aStatus);

	IMPORT_C void Recv(TDes8& aDesc,TUint flags,TRequestStatus& aStatus,TSockXfrLength& aLen);

	IMPORT_C void RecvOneOrMore(TDes8& aDesc,TUint flags,TRequestStatus& aStatus,TSockXfrLength& aLen);
	IMPORT_C void RecvOneOrMore(TDes8& aDesc,TUint flags,TRequestStatus& aStatus);
	IMPORT_C void CancelRecv();

	IMPORT_C void Read(TDes8& aDesc,TRequestStatus& aStatus);
	IMPORT_C void CancelRead();

	IMPORT_C void Write(const TDesC8& aDesc,TRequestStatus& aStatus);
	IMPORT_C void CancelWrite();
	IMPORT_C void SendTo(const TDesC8& aDesc,TSockAddr& anAddr,TUint flags,TRequestStatus& aStatus);

	IMPORT_C void SendTo(const TDesC8& aDesc,TSockAddr& anAddr,TUint flags,TRequestStatus& aStatus,TSockXfrLength& aLen);

	IMPORT_C void RecvFrom(TDes8& aDesc,TSockAddr& anAddr,TUint flags,TRequestStatus& aStatus);

	IMPORT_C void RecvFrom(TDes8& aDesc,TSockAddr& anAddr,TUint flags,TRequestStatus& aStatus,TSockXfrLength& aLen);
	IMPORT_C void Connect(TSockAddr& anAddr,TRequestStatus& aStatus);
	IMPORT_C void Connect(TSockAddr& anAddr,const TDesC8& aConnectDataOut,TDes8& aConnectDataIn,TRequestStatus& aStatus);
	IMPORT_C void CancelConnect();
	IMPORT_C TInt Bind(TSockAddr& anAddr);
	IMPORT_C TInt SetLocalPort(TInt aPort);
	IMPORT_C void Accept(RSocket& aBlankSocket,TRequestStatus& aStatus);
	IMPORT_C void Accept(RSocket& aBlankSocket,TDes8& aConnectData,TRequestStatus& aStatus);
	IMPORT_C void CancelAccept();
	IMPORT_C TInt Listen(TUint qSize);
	IMPORT_C TInt Listen(TUint qSize,const TDesC8& aConnectData);
	IMPORT_C TInt SetOpt(TUint anOptionName,TUint anOptionLevel,const TDesC8& anOption=TPtrC8(NULL,0));
	IMPORT_C TInt SetOpt(TUint anOptionName,TUint anOptionLevel,TInt anOption);
	IMPORT_C TInt GetOpt(TUint anOptionName,TUint anOptionLevel,TDes8& anOption);
	IMPORT_C TInt GetOpt(TUint anOptionName,TUint anOptionLevel,TInt& anOption);
	IMPORT_C void Ioctl(TUint aCommand,TRequestStatus& aStatus,TDes8* aDesc=NULL,TUint aLevel=KLevelUnspecified);
	IMPORT_C void CancelIoctl();
	IMPORT_C TInt GetDisconnectData(TDes8& aDesc);
	IMPORT_C void LocalName(TSockAddr& anAddr);
	IMPORT_C TUint LocalPort();
	IMPORT_C void RemoteName(TSockAddr& anAddr);
	IMPORT_C void Close();
	IMPORT_C void Shutdown(TShutdown aHow,TRequestStatus& aStatus);
	IMPORT_C void Shutdown(TShutdown aHow,const TDesC8& aDisconnectDataOut,TDes8& aDisconnectDataIn,TRequestStatus& aStatus);
	IMPORT_C void CancelAll();
	IMPORT_C TInt Info(TProtocolDesc& aProtocol);
	IMPORT_C TInt Name(TName& aName);
	IMPORT_C TInt Transfer(RSocketServ& aServer, const TDesC& aName);
	};

class TNameRecord
/** Contains the results of name queries.
@publishedAll
@released */
	{
public:
	inline TNameRecord();

	enum
	{
	EAlias=0x00000001,
	EPartial=0x00000002, 	/*!< Indicates a partial (truncated) host name. */
	
	};
	/** A host name

	@see THostName */
	THostName iName;
	/** An address

	@see TSockAddr */
	TSockAddr iAddr;
	/** Flags indicating some attribute about the name, i.e. EAlias */
	TInt iFlags;
	};

/** Packages the TNameRecord class so that it can be passed between a client and
the socket server.
@publishedAll
@released  */
typedef TPckgBuf<TNameRecord> TNameEntry;

class RHostResolver : public RSubSessionBase
/** Provides an interface to host name resolution services, such as DNS, that may
be provided by particular protocol modules.

The interface provides functions to access the following facilities:

Obtaining names from addresses.

Obtaining addresses from names.

Getting and setting local host name.

Not all actual services provide all these facilities. You should also consult
the documentation on the protocol you are intending to use. Functions return
KErrNotSupported if the protocol does not support a given operation. Note
that a description of the protocol family name resolution capabilities is
available at run-time from TProtocolDesc::iNamingServices.

Before using any service, a connection to a socket server session must be
made.

Each function is available in both synchronous and asynchronous versions.

A single RHostResolver can only perform one request of any type at once. A
client is panicked if it makes two requests.
@publishedAll
@released */
	{
public:
	IMPORT_C TInt Open(RSocketServ& aSocketServer,TUint anAddrFamily,TUint aProtocol);
	IMPORT_C TInt Open(RSocketServ& aSocketServer,TUint anAddrFamily,TUint aProtocol, RConnection& aConnection);
	IMPORT_C void GetByName(const TDesC& aName,TNameEntry& aResult,TRequestStatus& aStatus);
	IMPORT_C TInt GetByName(const TDesC& aName,TNameEntry& aResult);
	IMPORT_C void Next(TNameEntry& aResult,TRequestStatus& aStatus);
	IMPORT_C TInt Next(TNameEntry& aResult);
	IMPORT_C void GetByAddress(const TSockAddr& anAddr,TNameEntry& aResult,TRequestStatus& aStatus);
	IMPORT_C TInt GetByAddress(const TSockAddr& anAddr,TNameEntry& aResult);
	IMPORT_C TInt GetHostName(TDes& aName);
	IMPORT_C void GetHostName(TDes& aName,TRequestStatus& aStatus);
	IMPORT_C TInt SetHostName(const TDesC& aName);
	IMPORT_C void Close();
	IMPORT_C void Cancel();

    IMPORT_C void Query(const TDesC8& aQuery, TDes8& aResult, TRequestStatus& aStatus);
    IMPORT_C TInt Query(const TDesC8& aQuery, TDes8& aResult);
    IMPORT_C void QueryGetNext(TDes8& aResult, TRequestStatus& aStatus);
    IMPORT_C TInt QueryGetNext(TDes8& aResult);
    IMPORT_C TInt SetOpt(TUint anOptionName,TUint anOptionLevel,const TDesC8& anOption=TPtrC8(NULL,0));

private:
	};

/**
Port number on service

@publishedAll
@released
*/
typedef TPckgBuf<TInt> TPortNum;

class RServiceResolver : public RSubSessionBase
/** Provides an interface to resolver service names and ports.
@publishedAll
@released */
	{
public:
	IMPORT_C TInt Open(RSocketServ& aSocketServer,TUint anAddrFamily,TUint sockType,TUint aProtocol);
	IMPORT_C void GetByName(const TDesC& aName,TPortNum& aPort,TRequestStatus& aStatus);
	IMPORT_C TInt GetByName(const TDesC& aName,TPortNum& aPort);
	IMPORT_C void GetByNumber(const TUint aPort,TDes& aName,TRequestStatus& aStatus);
	IMPORT_C TInt GetByNumber(const TUint aPort,TDes& aName);
	IMPORT_C void RegisterService(const TDesC& aName,const TUint& aPort,TRequestStatus& aStatus);
	IMPORT_C TInt RegisterService(const TDesC& aName,const TUint& aPort);
	IMPORT_C void RemoveService(const TDesC& aName,const TUint& aPort,TRequestStatus& aStatus);
	IMPORT_C TInt RemoveService(const TDesC& aName,const TUint& aPort);
	IMPORT_C void Close();
	IMPORT_C void Cancel();
private:
	};

class RNetDatabase: public RSubSessionBase
/** Provides an interface to network databases, such as LM-IAS with IrDA, that
may be provided by particular protocol modules.

Before making any queries, a connection to a socket server session must be
made.

Each function is available in both synchronous and asynchronous versions.

A single RNetDatabase can only perform one request of any type at once. A
client is panicked if it makes two requests.

Particular database access services will specify the format of queries and
of responses.
@publishedAll
@released */
	{
public:
	IMPORT_C TInt Open(RSocketServ& aSocketServer,TUint anAddrFamily,TUint aProtocol);
	IMPORT_C void Query(const TDesC8& aQuery,TDes8& aResult,TRequestStatus& aStat);
	IMPORT_C TInt Query(const TDesC8& aQuery,TDes8& aResult);
	IMPORT_C void Add(const TDesC8& anItem,TRequestStatus& aStat);
	IMPORT_C TInt Add(const TDesC8& anItem);
	IMPORT_C void Remove(const TDesC8& anItem,TRequestStatus& aStat);
	IMPORT_C TInt Remove(const TDesC8& anItem);
	IMPORT_C void Close();
	IMPORT_C void Cancel();
private:
	};


/**
Default connection type
@publishedAll
@released
*/
const TUint KConnectionTypeDefault = 0x0800;		// KAfInet is the default connection type


class TConnPref;
class TSubConnectionInfo;
class TSubConnectionEvent;

class RConnection : public RCommsSubSession
/**

The management interface for a network connection or subconnection.

Provides clients with the following functionality:

Opening and closing the connection

Starting a connection, which means associating it with a new underlying interface

Attaching the RConnection instance to an existing interface

Stopping the connection, which means disassociating it from the underlying
interface

Obtaining progress information and notification during connection start-up

Notifying when subconnections come up and go down

Notifying when there is a service change for the connection

Notifying when a given amount of data has been sent or received on a connection
or subconnection

Reading CommDB fields specific to an active connection

Collecting statistical information on the network connection and subconnections.
A UI component can display the collected statistical information in order
to allow the user to examine the status of connections. The information that
can be gathered is the following:

All available internet access point names and internet access point 'friendly'
names as appropriate for each network (GPRS/UMTS) connection

Enumerating the currently active connections and subconnections

The current status of all network connections e.g. active/suspended

The amount of data (in bytes) transferred uplink and downlink by the network
connection and subconnections

The amount of time each network connection has been active (in seconds)

The current status of the connection and subconnections with respect to data
transfer, i.e. active/inactive

The Quality of Service profile associated with each Packet Data Protocol (GPRS/UMTS)
context, e.g. low/medium/high

Note that several of the new functions are asynchronous. It is essential for
these calls that the client ensures that the parameters they pass to the RConnection
API remain in scope for the duration of the asynchronous call.
@publishedAll
@released since v7.0s */
	{
public:
	/** Identifies the intended use of the connection. */
	enum TConnAttachType
		{
		/** The application wishes to use the connection for normal data transfer, and
		the idle timers will take that into account. */
		EAttachTypeNormal,
		/** The system control type of application wishes to monitor the state of the connection
		without otherwise affecting it. In particular, the interface idle timers will
		not be affected. */
		EAttachTypeMonitor
		};
	/** Identifies the type of requirement for stopping the connection. */
	enum TConnStopType
		{
		/** Any sockets or host/service resolvers associated with this interface will be
		errored with KErrCancel. */

		EStopNormal,

		/** Any sockets or host/service resolvers associated with this interface will be
		errored with KErrConnectionTerminated and should clean up quietly without
		prompting the user. */

		EStopAuthoritative
		};

	/**
	Connection Management Interface
	*/
	IMPORT_C RConnection();
	IMPORT_C virtual ~RConnection();
	IMPORT_C TInt Open(RSocketServ& aSocketServer, TUint aConnectionType = KConnectionTypeDefault);
	IMPORT_C TInt Open(RSocketServ& aSocketServer, TName& aName);
	IMPORT_C void Close();

	IMPORT_C void Start(TRequestStatus& aStatus);
	IMPORT_C void Start(TConnPref& aPref, TRequestStatus& aStatus);
	IMPORT_C TInt Start();
	IMPORT_C TInt Start(TConnPref& aPref);
	IMPORT_C TInt Stop();
	IMPORT_C TInt Stop(TConnStopType aStopType);
	IMPORT_C TInt Stop(TSubConnectionUniqueId aSubConnectionUniqueId);
	IMPORT_C TInt Stop(TSubConnectionUniqueId aSubConnectionUniqueId, TConnStopType aStopType);

	IMPORT_C void WaitForIncoming(RSubConnection& aIncomingSubConnection, TRequestStatus& aStatus);
	IMPORT_C TInt WaitForIncoming(RSubConnection& aIncomingSubConnection);
	IMPORT_C void CancelWaitForIncoming();

	IMPORT_C void ProgressNotification(TNifProgressBuf& aProgress, TRequestStatus& aStatus, TUint aSelectedProgress = KConnProgressDefault);
	IMPORT_C void ProgressNotification(TSubConnectionUniqueId aSubConnectionUniqueId, TNifProgressBuf& aProgress, TRequestStatus& aStatus, TUint aSelectedProgress = KConnProgressDefault);
	IMPORT_C void CancelProgressNotification();
	IMPORT_C void CancelProgressNotification(TSubConnectionUniqueId aSubConnectionUniqueId);
	IMPORT_C TInt Progress(TNifProgress& aProgress);
	IMPORT_C TInt Progress(TSubConnectionUniqueId aSubConnectionUniqueId, TNifProgress& aProgress);
	IMPORT_C TInt LastProgressError(TNifProgress& aProgress);
	IMPORT_C void ServiceChangeNotification(TUint32& aNewISPId, TDes& aNewServiceType, TRequestStatus& aStatus);
	IMPORT_C void CancelServiceChangeNotification();

	
	/**
	@deprecated Since SymbianOS v9.5
	*/
	IMPORT_C TInt GetIntSetting(const TDesC& aSettingName, TUint32& aValue);

	/**
	@deprecated Since SymbianOS v9.5
	*/
	IMPORT_C TInt GetBoolSetting(const TDesC& aSettingName, TBool& aValue);

	/**
	@deprecated Since SymbianOS v9.5
	*/
	IMPORT_C TInt GetDesSetting(const TDesC& aSettingName, TDes8& aValue);

	/**
	@deprecated Since SymbianOS v9.5
	*/
	IMPORT_C TInt GetDesSetting(const TDesC& aSettingName, TDes16& aValue);

	/**
	@deprecated Since SymbianOS v9.5
	*/
	IMPORT_C TInt GetLongDesSetting(const TDesC& aSettingName, TDes& aValue);


	IMPORT_C TInt GetParameters(ESock::CCommsDataObjectBase& aDataObject);
	IMPORT_C TInt SetParameters(ESock::CCommsDataObjectBase& aDataObject);

	IMPORT_C TInt Name(TName& aName);

	IMPORT_C TInt EnumerateConnections(TUint& aCount);
	IMPORT_C TInt GetConnectionInfo(TUint aIndex, TDes8& aConnectionInfo);

	IMPORT_C void AllInterfaceNotification(TDes8& aNotification, TRequestStatus& aStatus);
	IMPORT_C void CancelAllInterfaceNotification();

	IMPORT_C void Ioctl(TUint aOptionLevel, TUint aOptionName, TRequestStatus& aStatus, TDes8* aDesc);
	IMPORT_C void Ioctl(TUint aOptionLevel, TUint aOptionName, TRequestStatus& aStatus);
	IMPORT_C void CancelIoctl();
	IMPORT_C TInt Control(TUint aOptionLevel, TUint aOptionName, TDes8& aOption);
	IMPORT_C TInt GetOpt(TUint aOptionLevel, TUint aOptionName, TInt& aOption);
	IMPORT_C TInt SetOpt(TUint aOptionLevel, TUint aOptionName, TInt aOption = 0);

	IMPORT_C TInt Attach(const TDesC8& aConnectionInfo, TConnAttachType aAttachType);

	IMPORT_C TInt EnumerateSubConnections(TUint& aCount);
	IMPORT_C TInt GetSubConnectionInfo(TDes8& aSubConnectionInfo);
	IMPORT_C TInt GetSubConnectionInfo(TUint aIndex, TDes8& aSubConnectionInfo);
	IMPORT_C void AllSubConnectionNotification(TSubConnectionNotificationBuf& aSubConnectionEvent, TRequestStatus& aStatus);
	IMPORT_C void CancelAllSubConnectionNotification();

	IMPORT_C void DataTransferredRequest(TPckg<TUint>& aUplinkVolume, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus);
	IMPORT_C void DataTransferredRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TPckg<TUint>& aUplinkVolume, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus);
	IMPORT_C void DataTransferredCancel();
	IMPORT_C void DataTransferredCancel(TSubConnectionUniqueId aSubConnectionUniqueId);
	IMPORT_C void DataSentNotificationRequest(TUint aThreshold, TPckg<TUint>& aUplinkVolume, TRequestStatus& aStatus);
	IMPORT_C void DataSentNotificationRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aThreshold, TPckg<TUint>& aUplinkVolume, TRequestStatus& aStatus);
	IMPORT_C void DataSentNotificationCancel();
	IMPORT_C void DataSentNotificationCancel(TSubConnectionUniqueId aSubConnectionUniqueId);
	IMPORT_C void DataReceivedNotificationRequest(TUint aThreshold, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus);
	IMPORT_C void DataReceivedNotificationRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aThreshold, TPckg<TUint>& aDownlinkVolume, TRequestStatus& aStatus);
	IMPORT_C void DataReceivedNotificationCancel();
	IMPORT_C void DataReceivedNotificationCancel(TSubConnectionUniqueId aSubConnectionUniqueId);
	IMPORT_C void IsConnectionActiveRequest(TUint aSecs, TPckg<TBool>& aState, TRequestStatus& aStatus);
	IMPORT_C void IsConnectionActiveCancel();
	IMPORT_C void IsSubConnectionActiveRequest(TSubConnectionUniqueId aSubConnectionUniqueId, TUint aSecs, TPckg<TBool>& aState, TRequestStatus& aStatus);
	IMPORT_C void IsSubConnectionActiveCancel(TSubConnectionUniqueId aSubConnectionUniqueId);

	TBool SameSession(TInt aSessionHandle);
private:
	TPckg<TUint32> iNewISPId;
	TUint32 iReserved[4];
	};

class CSubConParameterSet : public XParameterSetBase
/** Base class for all RSubConnection parameter sets.

THIS API IS DEPRECATED IN FAVOUR OF XParameterSet

@publishedAll
@deprecated since v9.6 */
	{
public:
	IMPORT_C static CSubConParameterSet* NewL(const STypeId& aTypeId);
	virtual ~CSubConParameterSet();

protected:
	CSubConParameterSet();
	};


class CSubConGenericParameterSet : public CSubConParameterSet
/** Base class for generic RSubConnection parameter sets.

THIS API IS DEPRECATED IN FAVOUR OF XParameterSet

@publishedAll
@deprecated since v9.6 */
	{
public:
	IMPORT_C ~CSubConGenericParameterSet();

protected:
	IMPORT_C CSubConGenericParameterSet();
	};

class CSubConExtensionParameterSet : public CSubConParameterSet
/** Base class for extended RSubConnection parameter sets.

THIS API IS DEPRECATED IN FAVOUR OF XParameterSet

@publishedAll
@deprecated since v9.6 */
	{
public:
	IMPORT_C ~CSubConExtensionParameterSet();

protected:
	IMPORT_C CSubConExtensionParameterSet();
	};

class RSubConParameterBundle;
class CSubConParameterBundle;

const TInt32 KSubConnParamsInterfaceUid = 0x10204303;
const TInt32 KSubConnEventInterfaceUid = 0x10204305;

const TInt32 KSubConnGenericParamsImplUid  = 0x10204304;
const TInt32 KSubConnGenericEventsImplUid  = 0x10204306;

const TUint32 KSubConGlobalFamily 				= 0;
const TUint32 KSubConQoSFamily 					= 1;
const TUint32 KSubConAuthorisationFamily 		= 2;
const TUint32 KSubConnCallDescrParamsFamily 	= 3;
const TUint32 KSubConnContextDescrParamsFamily 	= 4;

const TUint32 KSubConIPAddressInfoFamily 		= 5;

const TInt32 KProtocolExtensionFamily 		= 6;
const TInt32 KFlowParametersFamily 		= 7;
class RParameterFamily;

class CSubConParameterFamily : public CBase
/** Container of RSubConnection parameter sets.

For each Parameter Type (Requested, Acceptable and Granted) it
contains one generic and 0..N extended parameter sets.

Note:
a CSubConParameterBundle or RSubConParameterBundle object can take ownership of a 
CSubConParameterFamily object, in this case, when the bundle is destroyed, this 
family object will also be destroyed  (along with any parameter sets that are owned 
by the family).

THIS API IS DEPRECATED IN FAVOUR OF RParameterFamily

@publishedAll
@deprecated since v9.6 */
	{
public:

	enum TParameterSetType
         {
         ERequested = 0,
         EAcceptable = 1,
		 EGranted = 2,
		 ENumValues = 3 // The number of values in this enum
         };

public:
	IMPORT_C static CSubConParameterFamily* NewL(RSubConParameterBundle& aBundle, TUint32 aFamilyId);
	IMPORT_C static CSubConParameterFamily* LoadL(RSubConParameterBundle& aBundle, TPtrC8& aBuffer);
	IMPORT_C static CSubConParameterFamily* NewL(CSubConParameterBundle& aBundle, TUint32 aFamilyId);
	IMPORT_C static CSubConParameterFamily* LoadL(CSubConParameterBundle& aBundle, TPtrC8& aBuffer);

public:
	IMPORT_C virtual ~CSubConParameterFamily();
	IMPORT_C void SetGenericSetL(CSubConGenericParameterSet& aGenericSet, TParameterSetType aType);
	IMPORT_C void AddExtensionSetL(CSubConExtensionParameterSet& aExtensionSet, TParameterSetType aType);

	IMPORT_C CSubConExtensionParameterSet* FindExtensionSet(TUid aSetId, TParameterSetType aType);
	IMPORT_C CSubConExtensionParameterSet* FindExtensionSet(STypeId aSetId, TParameterSetType aType);
	IMPORT_C CSubConGenericParameterSet* GetGenericSet(TParameterSetType aType);

	inline TUint32 Id();

	IMPORT_C TUint Length() const;
	IMPORT_C TInt Load(TPtrC8& aDes);
	IMPORT_C TInt Store(TDes8& aDes) const;
	IMPORT_C void ClearAllParameters(TParameterSetType aType);

	/**
	Copy the parameters of this CSubConParameterFamily to a RParameterFamily.
	@param aDest RParameterFamily object to copy to
	*/
	void CopyToFamilyL(RParameterFamily& aDest) const;

    /**
	Copy the parameters of this CSubConParameterFamily from a RParameterFamily.
	@param aSrc RParameterFamily object to copy from
	*/
	void CopyFromFamilyL(RParameterFamily& aSrc);
protected:
	IMPORT_C explicit CSubConParameterFamily(TUint32 aFamilyId);
	IMPORT_C void ConstructL(RSubConParameterBundle& aBundle);
	IMPORT_C void ConstructL(CSubConParameterBundle& aBundle);

	IMPORT_C static TInt32 ExtractFamilyAndCreateBufferL(TPtrC8& aBuffer, TPtrC8& aContainerBuffer);

private:
	const TUint32 iFamilyId;   // This member variable is accessed by an inline function ( Id () ).
	RMetaDataEComContainer iGenericSets;
protected:
	RMetaDataEComContainer iExtensionSets[ENumValues];

	};

class RParameterFamilyBundle;
class RSubConParameterBundle
/** Container for (bundle of) SubConnection parameter families.

May contain and 0..N SubConnection parameter families.

Note:
If the RSubConParameterBundle object takes ownership of any CSubConParameterFamily object, 
then when the bundle object is destroyed, any family owned by this object will also be 
destroyed.

THIS API IS DEPRECATED IN FAVOUR OF RParameterFamilyBundle

@publishedAll
@deprecated since v9.6 */
	{
public:
	IMPORT_C RSubConParameterBundle();
	IMPORT_C void Close();

	IMPORT_C TUint Length() const;
	IMPORT_C TInt Load(const TDesC8& aDes);
	IMPORT_C TInt Store(TDes8& aDes) const;
	IMPORT_C void AddFamilyL(CSubConParameterFamily* aFamily);
	IMPORT_C CSubConParameterFamily* FindFamily(TUint32 aFamilyId);
	IMPORT_C void ClearAllParameters(CSubConParameterFamily::TParameterSetType aType);

    /**
    Copy the contents of this parameter bundle to a RParameterFamilyBundle.
	@param aDest RParameterFamilyBundle to copy parameters to
	*/
	void CopyToFamilyBundleL(RParameterFamilyBundle& aDest) const;

	/**
	Copy the contents of an RParameterFamilyBundle to this RSubConParameterBundle
	@param aSrc RParameterFamilyBundle to copy contents from
	*/
	void CopyFromFamilyBundleL(RParameterFamilyBundle& aSrc);

protected:
	TInt CheckBundle() const;
private:
	RSubConParameterBundle(const RSubConParameterBundle& aBundle);
	RSubConParameterBundle& operator=(const RSubConParameterBundle& aBundle);

private:
	mutable CSubConParameterBundle* iBundle;
	};


const TInt KNotificationEventMaxSize = 2048;
class TNotificationEventBuf : public TBuf8<KNotificationEventMaxSize>
/**
Buffer for Sub-connection event notiifcation

@publishedAll
@released
*/
	{
public:
	IMPORT_C TNotificationEventBuf();
	IMPORT_C ~TNotificationEventBuf();

	IMPORT_C TBool IsGeneric() const;
	IMPORT_C TInt32 GroupId() const;
	IMPORT_C TUint32 Id() const;
	};

#ifndef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
class CSubConNotificationEvent : public SMetaDataECom
#else
class CSubConNotificationEvent : public ESock::XEventBase
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	{
public:
	IMPORT_C static CSubConNotificationEvent* NewL(const STypeId& aTypeId);
	IMPORT_C static CSubConNotificationEvent* NewL(const TNotificationEventBuf& aEventBuffer);

	IMPORT_C TBool IsGeneric() const;
	IMPORT_C TInt32 GroupId() const;
	IMPORT_C TUint32 Id() const;

private:
	};

class TSubConnOpen
/** @internalComponent */
	{
public:
	/** Defines the type of a subconnection	*/
	enum TSubConnType
		{
		EAttachToDefault, 	// will attach to the default sub connection
		ECreateNew, 		// will create a new sub connection

		//private extension to subconnection openning modes below:
		EWaitForIncoming
		};

	TSubConnType iType;
	TInt  iHandle;
	TInt  iReserved;
	};

class RSubConnection : public RCommsSubSession
/** A Sub-Connection, a channel within a Connection. A representation of a channel between this device and remote devices with which we are communicating. This channel will be used by one or more sockets.
 Depending on the state of the channel, it may not be possible to bind arbitary sockets into it.  Attempts to bind sockets from different protocol families to a single channel is an error, as each channel can only be used by one protocol family.
 @note The sub-connection can represent a end-to-end channel and/or a channel from this device to an intermediate device (e.g an access server such as a GGSN which using UMTS and PDP contexts. Properties can be specified simultaneously on protocol and link level.)

Before using any of these services, a connection to a socket server session
must have been made and the connection must be open.

@publishedAll
@released Since v9.0*/
	{
public:
    friend class RConnection;

	enum TSubConnType
		{
		EAttachToDefault,
		ECreateNew,
		EWaitIncoming
		};

	struct TEventFilter
		{
		inline TEventFilter(TInt32 aEventGroupId = KSubConnGenericEventsImplUid, TUint32 aEventMask = 0xffffffff);
		TInt32  iEventGroupUid;
		TUint32 iEventMask;
		};

public:
	// Sub Connection Management
	IMPORT_C RSubConnection();
	IMPORT_C TInt Open(RSocketServ& aServer, TSubConnType aSubConnType, RConnection& aConnection);
	IMPORT_C void Close();
	IMPORT_C void Start(TRequestStatus& aStatus);
	IMPORT_C TInt Start();
	IMPORT_C TInt Stop();		

	// Socket Management
	IMPORT_C void Add(RSocket& aSocket, TRequestStatus& aStatus);
	IMPORT_C void Remove(RSocket& aSocket, TRequestStatus& aStatus);

	// QoS Properties (legacy bundles - to be deprecated)
	IMPORT_C TInt SetParameters(const RSubConParameterBundle& aParametersSet);
	IMPORT_C TInt GetParameters(RSubConParameterBundle& aParametersSet);

	// QoS Properties (new prototype bundles)
	IMPORT_C TInt SetParameters(const RParameterFamilyBundle& aParametersSet);
	IMPORT_C TInt GetParameters(RParameterFamilyBundle& aParametersSet);

	// Event Notification
	IMPORT_C void EventNotification(TNotificationEventBuf& aEventBuffer, TBool aGenericEventsOnly, TRequestStatus& aStatus);
	IMPORT_C void EventNotification(TNotificationEventBuf& aEventBuffer, TEventFilter aEventFilterList[], TUint aEventListLength, TRequestStatus& aStatus);
	IMPORT_C void CancelEventNotification();

	// Generic Control
	IMPORT_C TInt Control(TUint aOptionLevel, TUint aOptionName, TDes8& aOption);
		
	//
	// IMPORT_C TInt Stop(TConnStopType aStopType);
	// IMPORT_C void ProgressNotification(TNifProgressBuf& aProgress, TRequestStatus& aStatus, TUint aSelectedProgress = KConnProgressDefault);
	// IMPORT_C void CancelProgressNotification();
	// IMPORT_C TInt Progress(TNifProgress& aProgress);
	// IMPORT_C void IsSubConnectionActiveRequest(TUint aSecs, TPckg<TBool>& aState, TRequestStatus& aStatus);
	// IMPORT_C void IsSubConnectionActiveCancel();	
	//	

	TBool SameSession(TInt aSessionHandle);

private:
	IMPORT_C TInt Open(RSocketServ& aServer, TSubConnOpen::TSubConnType aSubConnType, RConnection& aConnection);

	//Note : Not used anywhere else in the code, kept in order to avoid the BC Break
	TInt iSpare;
};


class ByteOrder
/** Reverses the byte order in 16 and 32-bit values.
@publishedAll
@released */
	{
public:
	IMPORT_C static TUint32 Swap32(TUint32 aVal);
	IMPORT_C static TUint16 Swap16(TUint16 aVal);
	inline static TUint16 Swap16(TUint aVal);
	};

class BigEndian
/** Inserts and extracts integers in big-endian format.
@publishedAll
@released */
	{
public:
	IMPORT_C static TUint32 Get32(const TUint8* aPtr);
	IMPORT_C static TUint16 Get16(const TUint8* aPtr);
	IMPORT_C static void Put32(TUint8* aPtr, TUint32 aVal);
	IMPORT_C static void Put16(TUint8* aPtr, TUint16 aVal);
	};

class LittleEndian
/** Inserts and extracts integers in little-endian format.
@publishedAll
@released */
	{
public:
	IMPORT_C static TUint32 Get32(const TUint8* aPtr);
	IMPORT_C static TUint16 Get16(const TUint8* aPtr);
	IMPORT_C static void Put32(TUint8* aPtr, TUint32 aVal);
	IMPORT_C static void Put16(TUint8* aPtr, TUint16 aVal);
	};

class TAccessPointInfo
/** Stores Access Point information.
@publishedAll
@released
*/
	{
public:
	TAccessPointInfo(TUint aApId = 0);

	TUint AccessPoint() const;
	void SetAccessPoint(TUint aAccessPoint);

	TBool operator== (const TAccessPointInfo& aRhs) const;
private:
	TUint iAccessPointId;
	};


#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock_internal.h>
#include <es_sock_partner.h>
#endif

#include <es_sock.inl>

#endif	//__ES_SOCK_H__

