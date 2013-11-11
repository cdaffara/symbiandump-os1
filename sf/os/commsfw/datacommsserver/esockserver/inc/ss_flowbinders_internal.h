// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__SS_FLOWBINDERS_INTERNAL_H__)
#define __SS_FLOWBINDERS_INTERNAL_H__

class RMBufChain;
class MProvdSecurityChecker;

namespace ESock
{

class MUpperDataReceiver;
class MUpperControl;
class MLowerDataSender;
class MLowerControl;

class MSessionDataNotify;
class MSessionControlNotify;
class MSessionData;
class MSessionControl;

class CSubConnectionFlowBase;
class MFlowBinderControl
/**
Interface that a flow implements to allow creation of binders on it.

@internalTechnology
*/
	{
public:
    //protocol binder
	IMPORT_C virtual MLowerControl* GetControlL(const TDesC8& aProtocol);
	IMPORT_C virtual MLowerDataSender* BindL(const TDesC8& aProtocol, MUpperDataReceiver* aReceiver, MUpperControl* aControl);
	IMPORT_C virtual void Unbind( MUpperDataReceiver* aReceiver, MUpperControl* aControl);

    //session binder
	IMPORT_C virtual MSessionControl* GetControlL(TInt aSessionType, MSessionControlNotify& aSessionControlNotify);
	IMPORT_C virtual MSessionData* BindL(MSessionDataNotify& aNotify);
	IMPORT_C virtual void Unbind();

	IMPORT_C virtual CSubConnectionFlowBase& CloneFlowL(); //creates an unitialised duplicate of
	                        //its flow
	virtual CSubConnectionFlowBase* Flow() = 0; //returns its own flow
	};
    
/****************************************************************************************
Interface Module related classes

The following four classes are used by stacked interface modules to pass data and control
information up and down.  These interfaces do not cross a thread boundary.  In the comments
that follow, the terms "upper" and "lower" are used to describe, in relative terms, which
classes are implemented by which of a pair of stacked interface modules.

               -----------------------------------------
               |             Upper Module              |
               -----------------------------------------
                 ^    ^                         |    |       CSubConnectionFlowBase
   MUpperControl |    | MUpperDataReceiver      |    |                  |
                 |    |                         |    |                  |
                 |    |        MLowerDataSender |    | MLowerControl    |
                 |    |                         v    v                  |
               -----------------------------------------                |
               |             Lower Module              |<---------------+
               -----------------------------------------
*/

class MUpperDataReceiver
/**
Class implemented by an "upper" interface module to accept inbound data travelling
upwards from the interface module below.

@since Argus
@internalTechnology
*/
	{
public:
	virtual void Process(RMBufChain& aData) = 0;
	};

class MLowerDataSender
/**
Class implemented by a "lower" interface module to accept outbound data travelling downwards
from the interface module above.

@since Argus
@internalTechnology
*/
	{
public:
	enum TSendResult
		{
		// Send Flow Control
		// The 0 and 1 values below explicitly match the legacy return values from a NIF Send() call.
		ESendBlocked  = 0,		// data accepted, send no more until MUpperControl::Unblock()
		ESendAccepted = 1		// data accepted, can send more
		};
	
	virtual TSendResult Send(RMBufChain& aData) = 0;
	};

class MUpperControl
/**
Class implemented by an "upper" interface module to accept control signals from the
interface module below.

@since Argus
@internalTechnology
*/
	{
public:
	virtual void StartSending() = 0;
	virtual void Error(TInt anError) = 0;
	};

class MLowerControl
/**
Class implemented by a "lower" interface module to accept control signals from the
interface module above.

@since Argus
@internalTechnology
*/
	{
public:
	enum TBlockOption
		{
		EDisableAllOtherBindersOnFlow,
		EDisableAllOtherBindersOnInterface,
		EEnableAllBinders
		};

	IMPORT_C virtual TInt GetName(TDes& aName);
	IMPORT_C virtual TInt BlockFlow(TBlockOption aOption);
	IMPORT_C virtual TInt GetConfig(TBinderConfig& aConfig);
	IMPORT_C virtual TInt Control(TUint aLevel, TUint aName, TDes8& aOption);
	};

class MSessionControl
/**
@internalTechnology
*/
    {
public:
	/** Describes the behaviour the SAP should take on shutdown.*/
	//the values of the enum directly map to CServProviderBase::TCloseType enum values
	enum TCloseType 
	{
		ENormal,        //< The protocol should shutdown gracefully & no further input or output will be requested.
		EStopInput,     //< The protocol should shut down gracefully & all further and pending input should be discarded.
		EStopOutput,    //< The protocol should shutdown gracefully & all pending output should be discarded.
		EImmediate      //< The protocol should close the connection immediately and free all resources without performing a graceful disconnect.
	};
	/**
	Start a service provider.
	*/
	virtual void Start()=0;
	/** Gets the local name (address) of the socket service provider entity. The format 
	of the data in the TSockAddr object is defined by individual protocols.
	
	The local address is the address of the local machine plus a local port number. 
	Generally only the port number is important, unless you have two IP interfaces, 
	for example.
	
	@param anAddr The address to be filled in */
	virtual void LocalName(TSockAddr& anAddr) const =0;

       /**When the set local name operation has completed, the protocol should call SetLocalNameComplete() 
	on its TNotify	*/
	virtual void SetLocalName(TSockAddr& anAddr)=0;

	/** Gets the remote name (address) of the socket service provider entity. The format 
	of the data in the TSockAddr object is defined by individual protocols.
	
	A remote address is either the address you're sending data to (non connection-oriented 
	sockets)* or the remote end of the connection. It is the address of the remote 
	machine (your peer in the network) plus a port number.
	
	@note RemName is only meaningful if the socket server client has called Connect() 
	to set up a default address for SendTo(). This function will only be called 
	on the protocol if this is the case.
	
	@param anAddr The address to be filled in */
	virtual void RemName(TSockAddr& anAddr) const =0;
	/** Sets the remote name (address) of the socket service provider entity. The format 
	of the data in the TSockAddr object is defined by individual protocols.
	
	@param anAddr The address 
	@return Returns KErrNone if the remote name is correctly set or, if this is 
	not the case, an informative error number. */
	virtual TInt SetRemName(TSockAddr& anAddr)=0;
	/** Gets some protocol specific option when called by the socket server on behalf of a 
	client. A protocol may pass the request down a protocol	stack (to protocols it is bound 
	to) using the GetOption() function of CProtocolBase.
	
	@param aLevel Option level.
	@param aName Option name.
	@param anOption Option data.
	@return System wide error code.
	*/
	virtual TInt GetOption(TUint level,TUint name,TDes8& anOption)const =0;
	/** Performs some protocol specific IO control. 
	
	@note If this function is called erroneously, the protocol should call Error() on the 
	socket. If an Ioctl call is already outstanding, the client will be panicked with the 
	value ETwoIoctls.
	
	@param aLevel Option level.
	@param aName Option name.
	@param anOption Option data.
	@return System wide error code.
	*/
	virtual void Ioctl(TUint level,TUint name,TDes8* anOption)=0;
	/** Cancels an outstanding Ioctl call. You are guaranteed only to have one outstanding 
	at once. 
	
	@param aLevel IOCTL level. 
	@param aName IOCTL name. 
	*/
	virtual void CancelIoctl(TUint aLevel,TUint aName)=0;
	/** Sets some protocol specific option when called by the socket server on behalf of a 
	client. A protocol may pass the request down a protocol	stack (to protocols it is bound 
	to) using the SetOption() function	of CProtocolBase.
	
	@param aLevel Option level.
	@param aName Option name.
	@param anOption Option data.
	@return System wide error code.
	*/
	virtual TInt SetOption(TUint level,TUint name,const TDesC8& anOption)=0;
	virtual void ActiveOpen()=0;
	/** Initiates a connection operation - this means that it tells the protocol to 
	attempt to connect to a peer. It is called by the socket server in response 
	to a connect request from a client. 
	
	This version of the function has user data in the connection frame.
	
	Only ever called on connection-oriented sockets. Such a socket 
	should always have both the local address and the remote address specified 
	before this function is called. If this is not the case then the protocol 
	should panic.
	
	When a connection has completed, the protocol should call ConnectComplete() 
	on its TNotify. If an error occurs during connection the protocol should not 
	call ConnectComplete() at all; instead it should call Error().
	
	@param aConnectionData If the protocol supports user specified connection 
	data, then it will be held in this buffer. */
	virtual void ActiveOpen(const TDesC8& aConnectionData)=0;
	/** Tells the protocol to start waiting for an incoming connection request on this 
	socket (i.e. port). It is called by the socket server in response to a listen 
	request from a client.
	
	Only ever called on connection-oriented sockets. Such a socket 
	should always have both the local address and the remote address specified 
	before this function is called. If this is not the case, then the protocol 
	should panic.
	
	The aQue parameter is the number of sockets which can be waiting for an outstanding 
	Start after calling ConnectComplete(). The protocol should keep a count of 
	sockets in this state - incrementing a variable in ConnectComplete(), and 
	decrementing it in Start().
	
	When a connection has completed, the protocol should call ConnectComplete() 
	on its TNotify. If an error occurs during connection the protocol should not 
	call ConnectComplete() at all; instead it should call Error().
	
	@param aQueSize Size of connect queue. 
	*/
	virtual TInt PassiveOpen(TUint aQueSize)=0;
	/** Tells the protocol to start waiting for an incoming connection request on this 
	socket (i.e. port). It is called by the socket server in response to a listen 
	request from a client.
	
	This version of the function has user data in the connection frame.
	
	Only ever called on connection-oriented sockets. Such a socket 
	should always have both the local address and the remote address specified 
	before this function is called. If this is not the case then the protocol 
	should panic.
	
	The aQue parameter is the number of sockets which can be waiting for an outstanding 
	Start after calling ConnectComplete(). The protocol should keep a count of 
	sockets in this state - incrementing a variable in ConnectComplete(), and 
	decrementing it in Start().
	
	When a connection has completed the protocol should call ConnectComplete() 
	on its TNotify. If an error occurs during connection the protocol should not 
	call ConnectComplete() at all; instead it should call Error().
	
	@param aQueSize size of connect queue 
	@param aConnectionData if the protocol supports user specified connection data 
	then it will be held in this buffer. */
	virtual TInt PassiveOpen(TUint aQueSize,const TDesC8& aConnectionData)=0;
	/** Terminates a connection (or closes a non connection-oriented socket down).
	 
	The value of the option argument specifies the type of processing which will 
	be required of the protocol after this function is called.
	
	Normally, when the socket server has called Shutdown() for a socket, it will 
	wait for the socket to call CanClose() before destroying the CSubConnectionFlowBase 
	object. However, if the option argument is EImmediate, the CSubConnectionFlowBase 
	will be destroyed as soon as Shutdown() returns.
	
	@param option The shutdown type. */
	virtual void Shutdown(MSessionControl::TCloseType option)=0;
	/** Terminates a connection (or closes a non connection-oriented socket down). 
	
	The value of the option argument specifies the type of processing which will 
	be required of the protocol after this function is called.
	
	Normally, when the socket server has called Shutdown() for a socket, it will 
	wait for the socket to call CanClose() before destroying the CSubConnectionFlowBase 
	object. However, if the option argument is EImmediate, the CSubConnectionFlowBase 
	will be destroyed as soon as Shutdown() returns.
	
	@param option The shutdown type. 
	@param aDisconnectionData If the protocol supports disconnect data, any such 
	data required will be held in this buffer. */
	virtual void Shutdown(MSessionControl::TCloseType option, const TDesC8& aDisconnectionData)=0;
	/** Specifies that the protocol should choose a local address for the service access 
	point itself. */
	virtual void AutoBind()=0;


	/**  Use the class instance argument to perform security policy checks on the originating client process.
	
	This method is called when a SAP is created and when a socket is transferred between sessions.  The SAP is
	required to check whether the originating client process has enough privileges to request services from the SAP.
	The MProvdSecurityChecker class instance is used to perform security policy checks.  The SAP may choose
	to perform a security policy check in its SecurityCheck(...) method, or it may choose to store the
	MProvdSecurityChecker class instance argument and perform checking later (i.e. when subsequent
	SAP methods are called).
	
	@param aSecurityChecker Pointer to class used by SAP to perform security checks on the client process.  This
	pointer becomes invalid when the SAP is destroyed or detached.
	@returns KErrPermissionDenied if SAP wishes to disallow access to the client, else KErrNone.  This would
	normally be as a result of calling MProvdSecurityChecker::CheckPolicy(...) with a suitable security policy argument.
	*/
	virtual TInt SecurityCheck(MProvdSecurityChecker *aSecurityChecker) = 0;
    };
    
class MSessionData
    {
/**
@internalTechnology
*/
public:
// protocol read/write data; ESock v1.5 calls down to the v1.5 itf (RMBufChain). 
// v1.5-ready protocols implement this, laggard protocols inherit these base class
// implementations which call the v1.0 descriptor itf instead. 

	/** Sends data onto the network via the protocol.
	
	Connection-oriented sockets must be in a connected state (that is ConnectComplete() has 
	been called on their MSocketNotify before Write() is called).
	 
	The socket server keeps track of how much data is waiting and then tries to send it all 
	until the protocol tells it to hold off by returning 0 (datagram sockets) or 'less than 
	all data consumed' (stream sockets) to Write(). The protocol should call CanSend() when it 
	is ready to send more data.
	
	anAddr is the address to write the data to.	Connection oriented sockets always use the 
	default value.
	
	@param aDesc The data to be sent.
	@param aOptions Protocol specific options.
	@param anAddr Address to write the data to.
	
	@returns For stream-oriented protocols the return value is the number of bytes actually written. 
	If this is less than the length of the descriptor then the protocol should call CanSend() 
	when it is ready to send more data. For datagram-oriented protocols, the write should return 
	either 0 if the write cannot be completed, or the length of the descriptor if the write succeeds - 
	no other values are valid. If the Write() must return 0, then it should call CanSend() when it is 
	ready to send more data. If the Write() fails due to some error, then it should call Error() with 
	an informative error number.
	*/
	virtual TUint Write(const TDesC8& aDesc,TUint options, TSockAddr* anAddr=NULL) = 0;
	/** Sends data onto the network via the protocol.
	
	Connection-oriented sockets must be in a connected state (that is ConnectComplete() has 
	been called on their MSocketNotify before Write() is called).
	 
	The socket server keeps track of how much data is waiting and then tries to send it all 
	until the protocol tells it to hold off by returning 0 (datagram sockets) or 'less than 
	all data consumed' (stream sockets) to Write(). The protocol should call CanSend() when it 
	is ready to send more data.
	
	anAddr is the address to write the data to.	Connection oriented sockets always use the 
	default value.
	
	@param aData The data to be sent.
	@param aOptions Protocol specific options.
	@param anAddr Address to write the data to.
	
	@returns For stream-oriented protocols the return value is the number of bytes actually written. 
	If this is less than the length of the descriptor then the protocol should call CanSend() 
	when it is ready to send more data. For datagram-oriented protocols, the write should return 
	either 0 if the write cannot be completed, or the length of the descriptor if the write succeeds - 
	no other values are valid. If the Write() must return 0, then it should call CanSend() when it is 
	ready to send more data. If the Write() fails due to some error, then it should call Error() with 
	an informative error number.
	*/
	virtual TInt Write(RMBufChain& aData, TUint aOptions, TSockAddr* anAddr=NULL) = 0;

	/** Gets data which the protocol has indicated is waiting in its buffers using the NewData 
	up-call on the MSocketNotify. 
	
	GetData() will only ever be called for as much data as the protocol has specified it can process 
	using the NewData up-call. 
	
	For stream oriented protocols GetData() should fill the descriptor with data from the stream. On 
	a datagram protocol GetData() should copy one datagram into the descriptor and set the length of 
	the descriptor. If a full datagram will not fit into the supplied descriptor, the overflow should 
	be discarded. 
	
	anAddr should be filled in by the protocol with the address of where the data came from.
	
	@param aDesc  The buffer for data.
	@param aOptions Protocol specific options.
	@param anAddr Address where the data came from.
	*/
	virtual void GetData(TDes8& aDesc,TUint options,TSockAddr* anAddr=NULL) = 0;
	/** Gets data which the protocol has indicated is waiting in its buffers using the NewData 
	up-call on the MSocketNotify. 
	
	GetData() will only ever be called for as much data as the protocol has specified it can process 
	using the NewData up-call. 
	
	For stream oriented protocols GetData() should fill the descriptor with data from the stream. On 
	a datagram protocol GetData() should copy one datagram into the descriptor and set the length of 
	the descriptor. If a full datagram will not fit into the supplied descriptor, the overflow should 
	be discarded. 
	
	anAddr should be filled in by the protocol with the address of where the data came from.
	
	@param aDesc  The buffer for data.
	@param aOptions Protocol specific options.
	@param anAddr Address where the data came from.
	*/
	virtual TInt GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr=NULL) = 0;
    };
}

#endif

