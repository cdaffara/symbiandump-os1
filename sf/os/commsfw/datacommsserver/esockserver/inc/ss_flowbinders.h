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

#if !defined(__SS_FLOWBINDERS_H__)
#define __SS_FLOWBINDERS_H__

#include <comms-infras/ss_common.h>
#include <es_enum.h>
#include <e32base.h>
#include <comms-infras/es_protbinder.h>

#include <comms-infras/ss_flowbinders_internal.h>

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

const TInt KErrBindersInvalid = -17205;	//The recepient of this error must abandoned its binders
                                        //as they have become invalid

// =========================================================================================

class MSessionDataNotify
/** Abstract base class used by a CSubConnectionFlowBase to support session binders, through its 
iSocket member, to notify the socket server that various events have occurred. 

The class provides several up-call member functions.

All up-calls on an MSocketNotify should be made in the context of the socket 
server's thread - i.e. the thread which called NewSAPL() on the protocol. 

@publishedPartner
@released Since v9.3 */
	{
public:
	
	/**
	Indicates that new data is available on a service access point

	For a stream-oriented protocol aCount should be a byte count; for datagram-oriented 
	sockets aCount should be a datagram count.
	
	@note aCount is the amount of new data, not the total amount of data waiting 
	to be read.
	
	@param aCount The amount of new data. A value of KNewDataEndofData indicates 
	that the socket is in a half-closed state and will receive no more data. Any subsequent 
	reads will complete with KErrEof and a length of 0. */
	virtual void NewData(TUint aCount) = 0;
	/** Indicates that new buffer space is available on a service. */
	virtual void CanSend() = 0;
	};

class MSessionControlNotify
/** Abstract base class used by a CSubConnectionFlowBase to support session binders, through its 
iSocket member, to notify the socket server that various events have occurred. 

The class provides several up-call member functions.

All up-calls on an MSocketNotify should be made in the context of the socket 
server's thread - i.e. the thread which called NewSAPL() on the protocol. 

@publishedPartner
@released Since v9.3 */
	{
public:
	enum TDelete 
		{
		/** Delete SAP */	
		EDelete,
		/** Don't delete SAP */
		EDetach
		};
	enum TOperationBitmasks
		{
		/** An error has occurred which affects Send() operations */
		EErrorSend=0x0000001,  

		/** An error has occurred which affects Receive() operations. */
		EErrorRecv=0x0000002,

		/** An error has occurred which affects Connect() operations */
		EErrorConnect=0x0000004,

		/** An error has occurred which affects Close() operations */
		EErrorClose=0x00000008,

		/** An error has occurred which affects Ioctl() operations */
		EErrorIoctl=0x00000010,

		/** An error has occurred which only requires to CompleteConnect beeing called **/
		EErrorCompleteConnect=0x00000020,
			/** An error has occurred which affects SetLocalName() operations */
		EErrorCompleteSetLocalName=0x00000040,

		/** A fatal error has occurred */
		EErrorFatal=0x10000000,
		
		/** An error has occurred which affects all operations */
		EErrorAllOperations=0x7fffffff,

#if defined(SYMBIAN_NETWORKING_LEGACY_COMPATIBILITY_SUPPORT)
		/** A pseudo-error: the SAP is requesting a legacy compatibility feature as specified
		by the error code. This flag cannot be combined with any others
		*/
		EErrorLegacySupportRequest = 0x40000000,
#endif
		};
	/** Indicates that a connection attempt has completed successfully (for active opens).
	 
	There are four versions of this up-call: two are for active opens and two are for passive 
	opens. Both active and passive versions support a variant carrying user connection 
	data for protocols which support it.
	
	@note A connection-less protocol should never call the ConnectComplete() up-calls.
	@note A protocol should keep a count of sockets which have called ConnectComplete() 
	but have not had Start() called. Sockets in this state have their Error() 
	and NewData() calls deferred until Start() is called.*/
	virtual void ConnectComplete() = 0;
	/** Indicates that a connection attempt has completed successfully (for active opens).
	 
	There are four versions of this up-call: two are for active opens and two are for passive 
	opens. Both active and passive versions support a variant carrying user connection 
	data for protocols which support it.
	
	@note A connection-less protocol should never call the ConnectComplete() up-calls.
	@note A protocol should keep a count of sockets which have called ConnectComplete() 
	but have not had Start() called. Sockets in this state have their Error() 
	and NewData() calls deferred until Start() is called.
	
	@param aConnectData Connect data (if supported).
	*/
	virtual void ConnectComplete(const TDesC8& aConnectData) = 0;
	/** Indicates that a connection attempt has completed successfully (for passive opens). 
	
	There are four versions of this up-call: two are for active opens and two are for passive 
	opens. Both active and passive versions support a variant carrying user connection 
	data for protocols which support it.
	
	The versions of ConnectComplete() for passive opens carry a new SSP (socket service 
	provider, or SAP) for the newly created socket. A new socket will then be linked up to 
	the SSP and data transfer can take place. The original socket stays listening.
	
	@note A connection-less protocol should never call the ConnectComplete() up-calls.
	@note A protocol should keep a count of sockets which have called ConnectComplete() 
	but have not had Start() called. Sockets in this state have their Error() 
	and NewData() calls deferred until Start() is called.
	
	@param aSSP The new SSP for passive opens. 
	*/
    virtual void ConnectComplete(CSubConnectionFlowBase& aSSP) = 0;
	/** Indicates that a connection attempt has completed successfully (for passive opens). 
	
	There are four versions of this up-call: two are for active opens and two are for passive 
	opens. Both active and passive versions support a variant carrying user connection 
	data for protocols which support it.
	
	The versions of ConnectComplete() for passive opens carry a new SSP (socket service 
	provider, or SAP) for the newly created socket. A new socket will then be linked up to 
	the SSP and data transfer can take place. The original socket stays listening.
	
	@note A connection-less protocol should never call the ConnectComplete() up-calls.
	@note A protocol should keep a count of sockets which have called ConnectComplete() 
	but have not had Start() called. Sockets in this state have their Error() 
	and NewData() calls deferred until Start() is called.
	
	@param aSSP The new SSP for passive opens. 
	@param aConnectData Connect data (if supported). */
	virtual void ConnectComplete(CSubConnectionFlowBase& aSSP,const TDesC8& aConnectData) = 0;
	/** Indicates that the SAP has finished closing down. 
	
	This up-call is the response to a Shutdown(). A connection-oriented protocol should call 
	CanClose() when it has terminated communications with the remote host. Protocols can call 
	CanClose() from within their Shutdown() code. After CanClose() has been called, an SAP may 
	be deleted by the socket server.
	
	@note The protocol must not access the CSubConnectionFlowBase object after calling CanClose().
	@param aDelete Delete SAP. 
	*/
	virtual void CanClose(MSessionControlNotify::TDelete aDelete=MSessionControlNotify::EDelete) = 0;
	/** Indicates that the SAP has finished closing down. 
	
	This up-call is the response to a Shutdown(). A connection-oriented protocol should call 
	CanClose() when it has terminated communications with the remote host. Protocols can call 
	CanClose() from within their Shutdown() code. After CanClose() has been called, an SAP may 
	be deleted by the socket server.
	
	@note The protocol must not access the CSubConnectionFlowBase object after calling CanClose().
	@param aDisconnectData Any user data carried on the disconnect frame.
	@param aDelete Delete SAP. 
	*/
	virtual void CanClose(const TDesC8& aDisconnectData,MSessionControlNotify::TDelete aDelete=MSessionControlNotify::EDelete) = 0;
	/** Tells the socket server that an error state has arisen within the protocol. 
	
	It should not be used to report programmatic errors, either in the protocol 
	itself or the socket server (a panic should be used in these cases). When 
	Error() is called on a connection-oriented socket, the socket is moved into 
	a dead state which denies user access to it.
	
	@param anError KErrNone or another of the system-wide error codes. 
	@param anOperationMask A bitmask of TOperationBitmasks values specifying which 
	@return error code translated by the object
	pending operations are affected by the Error up-call. */
	virtual TInt Error(TInt anError,TUint anOperationMask=MSessionControlNotify::EErrorAllOperations) = 0;
	/** Indicates that the other end of a connection has disconnected.
	
	This is analogous to CanClose(), but in this case the other end initiated it. 
	
	Once the client has called Shutdown() it is	illegal to call Disconnect(). Instead, 
	CanClose() or Error(KErrDisconnected) should be called.
	*/
	virtual void Disconnect(void) = 0;
	/** Indicates that the other end of a connection has disconnected.
	
	This is analogous to CanClose(), but in this case the other end initiated it. 
	
	Once the client has called Shutdown() it is	illegal to call Disconnect(). Instead, 
	CanClose() or Error(KErrDisconnected) should be called. 
	
	@param aConnectData User data in the disconnect frame.
	*/
	virtual void Disconnect(TDesC8& aDisconnectData) = 0;
	/** Indicates that the currently pending Ioctl has completed.
	
	The parameter aBuf is protocol defined - in fact it is defined by the specific 
	Ioctl.
	
	@param aBuf Any data requested by the Ioctl operation. */
	virtual void IoctlComplete(TDesC8* aBuf) = 0;
	/** Indicates that the other end of a passively opened connection which has not been
	accepted yet and is waiting in the accept queue has been disconnected.
	
	This is analogous to Disconnect(), but is directed towards the listening socket
	so that it can remove the SAP from the accept queue. 
	
	It is illegal to call DisconnectFromListener twice.
	*/
	virtual void DisconnectFromListener(CSubConnectionFlowBase& aSSP) = 0;

	/**
	Indicates that the set local name sequence is complete. 
	*/
    virtual void SetLocalNameComplete() = 0;


	};
} //namespace ESock

#endif //__SS_FLOWBINDERS_H__

