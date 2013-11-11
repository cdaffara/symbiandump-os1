// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __WAPMESSAGE_H__
#define __WAPMESSAGE_H__


#include <e32base.h>
#include <e32std.h>
#include <in_sock.h>

/** 
* @file
*
* The WAP Messaging API.  
*
* Four interfaces are defined that provide bound and fully-specified versions of WDP and Connectionless Push.
* An instantiation of each may be obtained using the CreateImplementationL() function, and must be released 
* using Release() when no longer required.  Release() causes the instantiation to be deleted.
*
* For error codes returned by methods in this API, see <wapmsgerr.h>.  Most methods can return a set of general 
* errors, with some returning additional specific errors.
*
* @publishedAll
* @released since v7.0
*/

/** 
* Defines WAP-related types and error codes. 
*/
namespace Wap
	{
	// Bearer type definition
	typedef enum
	/** Type definition for an enum defining WAP bearer types. */
		{
		/** All bearers. */
		EAll,
		/** Internet Protocol. */
		EIP,
		/** 7-bit SMS. */
		ESMS7,
		/** 8-bit SMS. */
		ESMS,
		/** 7-bit SMS. The bearers WAPSMS and WAPSMS7 are intended for WAP browsing where 
		delivery reports are not required and the validity period is much shorter (5 minutes). */
		EWAPSMS7,
		/** 8-bit SMS. */
		EWAPSMS
		} TBearer;

	// Port number definition
	/** A port number. */
	typedef TUint16 TPort;
 
	class TAddressInfo
	/** Encapsulates an interface name and address. */
		{
	public:
		/** Interface name. */
		TName iName;
		/** Interface IP address. */
		TInetAddr iAddress;
		};
	}


/**
* The WSP status type definition.
* @internalComponent
*/
typedef TUint8 TWSPStatus;
	
/** Bound WDP */
class CWapBoundDatagramService : public CBase
/** 
* Sends and receives datagrams over WDP using a specified local port.
*
* The class is an ECom plug-in interface. Clients use NewL() to request an implementation 
* of the interface, and then call the interface's virtual functions to access 
* the implementation's services. 
*
* The use of the plug-in architecture allows different implementations to use 
* different underlying WAP stacks.
*
* Functions can return system wide error codes, and also API-specific errors 
* as defined in wapmsgerr.h. 
* 
*/
	{
public: // creation/deletion
	IMPORT_C static CWapBoundDatagramService* NewL();
	IMPORT_C static CWapBoundDatagramService* NewL(const TUid& aImplementation);
	IMPORT_C virtual ~CWapBoundDatagramService();

public: // API methods

	/** 
	* Connects to the WAP stack, opening an endpoint that can be used to listen for 
	* subsequent incoming datagrams.
	*
	* This overload of Connect() allows an IP address associated with a network 
	* interface to be specified. In multihomed systems, this can be used to specify 
	* the network interface to which the endpoint should be bound.
	* 
	* All CWapBoundDatagramService implementations must automatically close this 
	* endpoint upon destruction.
	* 
	* @param aBearer	The bearer to listen on. Use EAll to specify all bearers.
	* @param aPort		The port to listen on. If set to 0, a local port will be chosen 
	* 					for the client's first SendTo()
	* @param aInetAddr	The IP address of the network interface that should be used 
	*					in a multihomed system.
	* @return			KErrNone on successful completion, or one of the system error codes 
	* 					on failure.
	*/
	virtual TInt Connect(Wap::TBearer aBearer, Wap::TPort aPort,TInetAddr aInetAddr)=0;

	/** 
	* Connects to the WAP stack, opening an endpoint that can be used to listen for 
	* subsequent incoming datagrams.
	* 
	* All CWapBoundDatagramService implementations must automatically close this 
	* endpoint upon destruction. 
	* 
	* @param aBearer	The bearer to listen on. Use EAll to specify all bearers.
	* @param aPort		The port to listen on. If set to 0, a local port will be chosen 
	* 					for the client's first SendTo()
	* @return			KErrNone on successful completion, or one of the system error codes 
	* 					on failure.
	*/
	virtual TInt Connect(Wap::TBearer aBearer,Wap::TPort aPort)=0;

	/** Sends data to a remote endpoint. 
	* 
	* @param aRemoteHost	The address of the remote host to which to send the data. 
	* 						The format of the address is bearer-specific.
	* @param aRemotePort	The port on the remote host to which the data will be sent
	* @param aBuffer		The data buffer to be sent
	* @param aBearer		The bearer to be used, if the bound connection was opened with EAll
	* @return				KErrNone on successful completion, or one of the system error codes 
	* 						on failure.
	*/
virtual TInt SendTo(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, const TDesC8& aBuffer, Wap::TBearer aBearer)=0;

	/** 
	* Waits for a datagram to be received, and discovers the size of buffer required 
	* to retrieve it.
	* 
	* This asynchronous method waits for a datagram to be received and will then 
	* complete allowing the client to discover how large a buffer is needed to retrieve 
	* the entire datagram that has been received. A later call to RecvFrom() with 
	* a buffer of sufficient size will then allow the client to retrieve the datagram 
	* fully. 
	* 
	* @param aDataSizePckg	On completion, the size of data received, in bytes
	* @param aReqStatus		Asynchonrous status word, used to signal when a data size is known
	* @return				KErrNone on successful completion, or one of the system error codes 
	* 						on failure.
	*/
	virtual TInt AwaitRecvDataSize(TPckg<TUint16>& aDataSizePckg, TRequestStatus& aReqStatus)=0;

	/** 
	* Receives data on a bound port. 
	* 
	* An asynchronous notification is sent to the client when data arrives. 
	* 
	* @param aRemoteHost	On completion, the bearer-dependent address of the remote 
	* 						host from which the data was received
	* @param aRemotePort	On completion, the port on the remote host from which the 
	* 						data was received
	* @param aBuffer		A client-allocated data buffer that on completion is filled 
	* 						with data received. Data that overflows the buffer is discarded.
	* @param aTruncated		On completion, indicates whether the received datagram was 
	* 						truncated to fit in the client's supplied buffer
	* @param aReqStatus		Asynchronous status word, used to notify the client that 
	* 						a datagram was received
	* @param aTimeout		An optional millisecond time-out which allows a timed read 
	* 						to be made. If no data is received within the timeout period, the request 
	* 						completes with KErrTimedOut. If a value of 0 is supplied, the timeout is infinite.
	* @return				KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt RecvFrom(TDes8& aRemoteHost, Wap::TPort& aRemotePort, TDes8& aBuffer, TBool& aTruncated, 
							TRequestStatus& aReqStatus, TUint32 aTimeout)=0;
 
	/** 
	* Cancels a previously asynchronous RecvFrom() or AwaitRecvDataSize() request. 
	*
	* If a datagram arrives at the local host, it will be discarded. 
	* 
	*/
	virtual void CancelRecv()=0;

	/** 
	* Gets the local port of this endpoint.
	* 
	* This is useful if the port was chosen automatically.
	* 
	* @param aPort	On return, the port number
	* @return		KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt GetLocalPort(Wap::TPort& aPort)=0;

	/** 
	* Gets the local address of this endpoint. 
	* 
	* @param aLocalHost	On return, the address of the local host. Clients must pass 
	* 					in a reference to a NULL HBufC8 pointer. The function allocates a new HBufC8 
	* 					buffer to hold the address, and passes ownership of the buffer to the client.
	* @return			KErrNone on successful completion, or one of the system error codes on failure.
	*/
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost)=0;

	/** 
	* Gets the bearer on which a received datagram arrived. 
	* 
	* This is useful when EAll was specified as the bearer in Connect().
	* 
	* @param aBearer	On return, the bearer
	* @return			KErrNone on successful completion, or one of the system error codes on failure.	
	*/
	virtual TInt GetBearer(Wap::TBearer& aBearer)=0;

	/** 
	* Queries the WDP bearer for its maximum datagram size and its nominal datagram size.
	*
	* The nominal size is the size within which a datagram won't have to be split 
	* into smaller individual messages and then re-assembled at the other end. 
	* 
	* The function will fail for a stream connection.
	*
	* @param aMaxSize		On return, the maximum datagram size
	* @param aNominalSize	On return, the nominal datagram size
	* @return				KErrNone on successful completion, or one of the system error codes 
	*						on failure.
	*/	
	virtual TInt GetDatagramSizes(TUint16& aMaxSize, TUint16& aNominalSize)=0;
	
protected: // methods
	IMPORT_C CWapBoundDatagramService();
	IMPORT_C void ConstructL();

private: // attributes
	// D'tor Key identification required for ECOM
	TUid iDtor_ID_Key;
	};

/** Fully-Specified WDP
*/
class CWapFullySpecDatagramService : public CBase
/** 
* Sends and receives datagrams over WDP using a specified local port and a single, 
* named remote host.
* 
* The class is an ECom plug-in interface. Clients use NewL() to request an implementation 
* of the interface, and then call the interface's virtual functions to access 
* the implementation's services. 
* 
* The use of the plug-in architecture allows different implementations to use 
* different underlying WAP stacks.
* 
* Functions can return system wide error codes, and also API-specific errors 
* as defined in wapmsgerr.h. 
*/
	{
public: // creation/deletion
	IMPORT_C static CWapFullySpecDatagramService* NewL();
	IMPORT_C static CWapFullySpecDatagramService* NewL(const TUid& aImplementation);
	IMPORT_C virtual ~CWapFullySpecDatagramService();

public: // API methods

	// Connect to the wapstack, opening an endpoint which is to be used only with a single, named remote host.
	/** 
	* Connects to the WAP stack, opening an endpoint that can be used to listen for 
	* subsequent incoming datagrams.
	*	
	* This overload of Connect() allows an IP address associated with a network 
	* interface to be specified. In multihomed systems, this can be used to specify 
	* the network interface to which the endpoint should be bound.
	* 
	* All CWapFullySpecDatagramService implementations must automatically close 
	* this endpoint upon destruction.
	* 
	* @param aRemoteHost	The bearer-dependent address of the remote host with which 
	* 						the data will be exchanged
	* @param aRemotePort	The port on the remote host to which data will be sent
	* @param aBearer		The bearer to use. EAll cannot be used.
	* @param aInetAddr		The IP address of the network interface that should be used 
	* 						in a multihomed system.
	* @return				KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TInetAddr aInetAddr)=0;

	// Connect to the wapstack, opening an endpoint which is to be used only with a single, named remote host.
	/** 
	* Connects to the WAP stack, opening an endpoint that can be used to listen for 
	* subsequent incoming datagrams.
	* 
	* All CWapFullySpecDatagramService implementations must automatically close 
	* this endpoint upon destruction.
	* 
	* @param aRemoteHost	The bearer-dependent address of the remote host with which 
	* 						the data will be exchanged
	* @param aRemotePort	The port on the remote host to which data will be sent
	* @param aBearer		The bearer to use. EAll cannot be used.
	* @return 				KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer)=0;

	/** 
	* Sends data on a fully-specified connection.
	* 
	* @param aBuffer	The data buffer to be written over the connection
	* @return			KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt Send(const TDesC8& aBuffer)=0;

	/** 
	* Waits for a datagram to be received, and discover how large a buffer is required 
	* to retrieve it.
	* 
	* This asynchronous request waits for a datagram to be received and will then 
	* complete allowing the client to discover how large a buffer is needed to retrieve 
	* the entire datagram that has been received. A later call to Recv() with a 
	* buffer of sufficent size will then allow the client to retrieve the datagram 
	* fully. 
	* 
	* @param aDataSizePckg	On completion, the size of data received, in bytes
	* @param aReqStatus		Asynchonrous status word, used to signal when a data size is known
	* @return				KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt AwaitRecvDataSize(TPckg<TUint16>& aDataSizePckg, TRequestStatus& aReqStatus)=0;

	/** 
	* Receives data on a bound port. 
	* 
	* An asynchronous notification is sent to the client when data arrives. 
	* 
	* @param aBuffer	A client-allocated data buffer that, on completion, is filled 
	* 					with data received. Data that overflows the buffer is discarded.
	* @param aTruncated	On completion, indicates whether the received datagram was 
	* 					truncated to fit in the client's supplied buffer
	* @param aReqStatus	Asynchronous status word, used to notify the client that 
	* 					a datagram was received
	* @param aTimeout	An optional millisecond time-out which allows a timed read 
	* 					to be made. If no data is received within the timeout period, the request 
	* 					completes with KErrTimedOut. If a value of 0 is supplied, the timeout is infinite.
	* @return			KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt Recv(TDes8& aBuffer, TBool& aTruncated, TRequestStatus& aReqStatus, TUint32 aTimeout)=0;

	/** 
	* Cancels a previously asynchronous Recv() or AwaitRecvDataSize() request. 
	* 
	* If a datagram arrives at the local host, it will be discarded. 
	* 
	*/
	virtual void CancelRecv()=0;

	/** 
	* Gets the local port of this endpoint.
	* 
	* This is useful if the port was chosen automatically.
	* 
	* @param aPort	On return, the port number
	* @return		KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt GetLocalPort(Wap::TPort& aPort)=0;

	/** 
	* Gets the local address of this endpoint. 
	* 
	* @param aLocalHost	On return, the address of the local host. Clients must pass 
	* 					in a reference to a NULL HBufC8 pointer. The function allocates a new HBufC8 
	* 					buffer to hold the address, and passes ownership of the buffer to the client.
	* @return			KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost)=0;

	/** 
	* Queries the WDP bearer for its maximum datagram size and its nominal datagram size.
	* 
	* The nominal size is the size within which a datagram won't have to be split 
	* into smaller individual messages and then re-assembled at the other end. 
	* 
	* @param aMaxSize		On return, the maximum datagram size
	* @param aNominalSize	On return, the nominal datagram size
	* @return				KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt GetDatagramSizes(TUint16& aMaxSize, TUint16& aNominalSize)=0;
	
protected: // methods
	IMPORT_C CWapFullySpecDatagramService();
	IMPORT_C void ConstructL();

private: // ECOM
	// D'tor Key identification required for ECOM
	TUid iDtor_ID_Key;
	};

/** Bound Connectionless Push
*/
class CWapBoundCLPushService : public CBase
/** 
* Listens for WAP Push messages from any sender.
* 
* The class is an ECom plug-in interface. Clients use NewL() to request an implementation 
* of the interface, and then call the interface's virtual functions to access 
* the implementation's services. 
* 
* The use of the plug-in architecture allows different implementations to use 
* different underlying WAP stacks.
* 
* Functions can return system wide error codes, and also API-specific errors 
* as defined in wapmsgerr.h. 
* 
*/
{
public: // creation/deletion
	IMPORT_C static CWapBoundCLPushService* NewL();
	IMPORT_C static CWapBoundCLPushService* NewL(const TUid& aImplementation);
	IMPORT_C virtual ~CWapBoundCLPushService();

public: // API methods

	
	// Opens a socket which is to be used to listen for subsequent incoming Push messages from any sender;
	// i.e. it has only been 'bound' to the local address	 
	/** 
	* Connects to the WAP stack, opening an endpoint that can be used to listen for 
	* Push messages from any sender.
	* 
	* This overload of Connect() allows an IP address associated with a network 
	* interface to be specified. In multihomed systems, this can be used to specify 
	* the network interface to which the endpoint should be bound.
	* 
	* All CWapBoundCLPushService implementations must automatically close this endpoint 
	* upon destruction.
	* 
	* @param aBearer	The bearer to listen on. Use EAll to specify all bearers.
	* @param aPort		The port to listen on. If set to 0, a local port will be chosen 
	* 					for the client's first SendTo()
	* @param aSecure	Security flag to indicate whether WTLS should be used or not
	* @param aInetAddr	The address of the adapter to use
	* @return			KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt Connect(Wap::TBearer aBearer, Wap::TPort aPort, TBool aSecure, TInetAddr aInetAddr)=0;

	
	// Opens a socket which is to be used to listen for subsequent incoming Push messages from any sender;
	// i.e. it has only been 'bound' to the local address
	/** 
	* Connects to the WAP stack, opening an endpoint that can be used to listen for 
	* Push messages from any sender.
	* 
	* All CWapBoundCLPushService implementations must automatically close this endpoint 
	* upon destruction.
	* 
	* @param aBearer	The bearer to listen on. Use EAll to specify all bearers.
	* @param aPort		The port to listen on. If set to 0, a local port will be chosen 
	* 					for the client's first SendTo()
	* @param aSecure	Security flag to indicate whether WTLS should be used or not
	* @return			KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt Connect(Wap::TBearer aBearer, Wap::TPort aPort, TBool aSecure)=0;

	/** 
	* Requests an asynchronous notification upon arrival of the next push message 
	* on the listening connection.
	* 
	* The request completes upon receipt of the message, filling the buffers with 
	* as much received data as possible. A return code indicates whether further 
	* data remains. The call must be re-issued for subsequent messages or to receive 
	* remaining data from a previous push message.
	* 
	* @param aPushHeaders	A client-allocated buffer that, on completion, is filled 
	* 						with the push message's header data
	* @param aPushBody		A client-allocated buffer that, on completion, is filled with 
	* 						the push message's body data
	* @param aPushIdPckg	On completion, an integer ID that uniquely specifies the 
	* 						push message
	* @param aReqStatus		Asynchonrous status word, used by the service provider to 
	* 						notify the client when a push message has arrived. 
	*						EMoreData is returned if more pushed data is available.
	* @return				KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt AwaitPush(TDes8& aPushHeaders, TDes8& aPushBody, TPckgBuf<TUint8>& aPushIdPckg, TRequestStatus& aReqStatus)=0;

	/** 
	* Cancels a previous push message request.
	* 
	* If a push message arrives, the client will not be notified. 
	*
	*/
	virtual void CancelAwaitPush()=0;

	/** 
	* Gets the local port of this endpoint.
	* 
	* This is useful if the port was chosen automatically. 
	* 
	* @param aPort	On return, the port number
	* @return		KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt GetLocalPort(Wap::TPort& aPort)=0;

	/** 
	* Gets the local address of this endpoint. 
	* 
	* @param aLocalHost	On return, the address of the local host. Clients must pass 
	* 					in a reference to a NULL HBufC8 pointer. The function allocates a new HBufC8 
	* 					buffer to hold the address, and passes ownership of the buffer to the client.
	* @return			KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost)=0;

	/** 
	* Gets the bearer on which a received datagram arrived. 
	* 
	* This is useful when EAll was specified as the bearer in Connect().
	* 
	* @param aBearer	On return, the bearer
	* @return			KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt GetBearer(Wap::TBearer& aBearer)=0;

	/**
	 * Gets the address of the remote server.
	 *
	 * This function cannot be called when there is an outstanding AwaitPush().
	 *
	 * @param aRemoteHost	On return, the address of the remote host. Clients must pass 
	 * 						in a reference to a NULL HBufC8 pointer. The function allocates a new HBufC8 
	 * 						buffer to hold the address, and passes ownership of the buffer to the client.
	 * @return				KErrNone on successful completion, KErrNotSupported if not implemented,
	 * 						or one of the system error codes on failure.
	 */
	virtual TInt GetServerAddress(HBufC8*& aRemoteHost)=0;
	
protected: // methods
	IMPORT_C CWapBoundCLPushService();
	IMPORT_C void ConstructL();

private: // ECOM
	// D'tor Key identification required for ECOM
	TUid iDtor_ID_Key;
};

/** Fully-Specified Connectionless Push
*/
class CWapFullySpecCLPushService : public CBase
/** 
* Listens for WAP Push messages from a single, named remote host.
* 
* The class is an ECom plug-in interface. Clients use NewL() to request an implementation 
* of the interface, and then call the interface's virtual functions to access 
* the implementation's services. 
* 
* The use of the plug-in architecture allows different implementations to use 
* different underlying WAP stacks.
* 
* Functions can return system wide error codes, and also API-specific errors 
* as defined in wapmsgerr.h. 
*/
{
public: // creation/deletion
	IMPORT_C static CWapFullySpecCLPushService* NewL();
	IMPORT_C static CWapFullySpecCLPushService* NewL(const TUid& aImplementation);
	IMPORT_C virtual ~CWapFullySpecCLPushService();

public: // API methods

 	// Opens a socket which is to be used only with a single, named remote host.
	/** 
	* Connects to the WAP stack, opening an endpoint that can be used to listen for 
	* Push messages from a specified remote host.
	* 
	* This overload of Connect() allows an IP address associated with a network 
	* interface to be specified. In multihomed systems, this can be used to specify 
	* the network interface to which the endpoint should be bound.
	* 
	* All CWapFullySpecCLPushService implementations must automatically close this 
	* endpoint upon destruction.
	* 
	* @param aRemoteHost	The bearer-dependent address of the remote host with which 
	* 						the data will be exchanged
	* @param aRemotePort	The port on the remote host to which data will be sent
	* @param aBearer		The bearer to listen on. You cannot use EAll.
	* @param aSecure		Security flag to indicate whether WTLS should be used or not
	* @param aInetAddr		The address of the adapter to use
	* @return				KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool aSecure, TInetAddr aInetAddr)=0;

	// Opens a socket which is to be used only with a single, named remote host.
	/** 
	* Connects to the WAP stack, opening an endpoint that can be used to listen for 
	* Push messages from a specified remote host.
	* 
	* All CWapFullySpecCLPushService implementations must automatically close this 
	* endpoint upon destruction.
	* 
	* @param aRemoteHost	The bearer-dependent address of the remote host with which 
	* 						the data will be exchanged
	* @param aRemotePort	The port on the remote host to which data will be sent
	* @param aBearer		The bearer to listen on. You cannot use EAll.
	* @param aSecure		Security flag to indicate whether WTLS should be used or not
	* @return				KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool aSecure)=0;

	/** 
	* Requests an asynchronous notification upon arrival of the next push message 
	* on the listening connection.
	* 
	* The request completes upon receipt of the message, filling the buffers with 
	* as much received data as possible. A return code indicates whether further 
	* data remains. The call must be re-issued for subsequent messages or to receive 
	* remaining data from a previous push message.
	* 
	* @param aPushHeaders	A client-allocated buffer that, on completion, is filled 
	* 						with the push message's header data
	* @param aPushBody		A client-allocated buffer that, on completion, is filled with 
	* 						the push message's body data
	* @param aPushIdPckg	On completion, an integer ID that uniquely specifies the 
	* 						push message
	* @param aReqStatus		Asynchronous status word, used by the service provider to 
	* 						notify the client when a push message has arrived.  
	* 						EMoreData is returned if more pushed data is available.
	* @return				KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt AwaitPush(TDes8& aPushHeaders, TDes8& aPushBody, TPckgBuf<TUint8>& aPushIdPckg, TRequestStatus& aReqStatus)=0;

	/** 
	* Cancels a previous push message request.
	* 
	* If a push message arrives, the client will not be notified. 
	*
	*/
	virtual void CancelAwaitPush()=0;


	/** 
	* Gets the local port of this endpoint.
	* 
	* This is useful if the port was chosen automatically. 
	* 
	* @param aPort	On return, the port number
	* @return		KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt GetLocalPort(Wap::TPort& aPort)=0;	// // (out) the port number


	/** 
	* Gets the local address of this endpoint. 
	* 
	* @param aLocalHost	On return, the address of the local host. Clients must pass 
	* 					in a reference to a NULL HBufC8 pointer. The function allocates a new HBufC8 
	* 					buffer to hold the address, and passes ownership of the buffer to the client.
	* @return			KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost)=0;	// // (out) the address of the local host

protected: // methods
	IMPORT_C CWapFullySpecCLPushService();
	IMPORT_C void ConstructL();

private: // ECOM
	// D'tor Key identification required for ECOM
	TUid iDtor_ID_Key;
};

/** Bound Connectionless WSP
*  @released since v8.0
*/
class CWapBoundCLWSPService : public CBase
{
public: // creation/deletion
	IMPORT_C static CWapBoundCLWSPService* NewL();
	IMPORT_C static CWapBoundCLWSPService* NewL(const TUid& aImplementation);
	IMPORT_C virtual ~CWapBoundCLWSPService();

public: // API methods

	/** 
	 * Connects to the WAP stack, opening an endpoint that can be used for S-Unit-MethodInvoke and 
	 * S-Unit-MethodResult primitives.
	 *
	 * @param aBearer	The bearer to listen on (use EAll for all bearers)
	 * @param aPort		The port to listen on. If set to 0, a local port will be chosen for the 
	 *					client's first MethodInvoke()
	 * @return			KErrNone on successful completion, or one of the system error codes on failure.
	 */
	virtual TInt Connect(Wap::TBearer aBearer,Wap::TPort aPort, TBool aSecure)=0;

	/**
	 * Sends a request to a remote endpoint.
	 *
	 * @param aBearer			The bearer to be used, if the bound connection was opened with EAll
	 * @param aRemoteHost		The address of the remote host to which to send the request. 
	 * 							The format of the address is bearer-specific.
	 * @param aRemotePort		The port on the remote host to which the request will be sent
	 * @param aMethod
	 * @param aURI
	 * @param aReqHeaders
	 * @param aReqBody
	 * @param aTransactionId
	 * @return					KErrNone on successful completion, or one of the system error codes on failure.
	 */ 
	virtual TInt MethodInvoke(Wap::TBearer aBearer, const TDesC8& aRemoteHost, Wap::TPort aRemotePort, TUint aMethod, const TDesC& aURI, const TDesC8& aReqHeaders, const TDesC8& aReqBody, const TUint8 aTransactionId)=0;

	/**
	 * Waits for a datagram to be received.
	 *
	 * @param aReqHeaders
	 * @param aReqBody
	 * @param aTransactionIdPckg
	 * @param aWspStatus
	 * @param aReqStatus			Asynchonrous status word, used to signal when a data size is known
	 * @param aTimeout
	 * @return 						KErrNone on successful completion, or one of the system error codes on failure.
	 */
	virtual TInt MethodResult(TDes8& aReqHeaders, TDes8& aReqBody, TPckgBuf<TUint8>& aTransactionIdPckg, TWSPStatus& aWspStatus, TRequestStatus& aReqStatus, TUint32 aTimeout)=0;

	/**
	 * Cancels a previously requested asynchronous MethodResult() notification.
	 * 
	 * If a datagram arrives at the local host, it will be discarded.
	 * 
	 */	
	virtual void CancelReq()=0;


	/** 
	 * Gets the local port of this endpoint.
	 *
	 * This is useful if the port was chosen automatically.
 	 * 
 	 * @param aPort	On return, the port number
	 * @return		KErrNone on successful completion, or one of the system error codes on failure.
	 */
	virtual TInt GetLocalPort(Wap::TPort& aPort)=0;	// // (out) the port number

	/**
	 * Gets the local address of this endpoint.
	 * 
	 * @param aLocalHost	On return, the address of the local host. Clients must pass 
	 * 						in a reference to a NULL HBufC8 pointer. The function allocates a new HBufC8 
	 * 						buffer to hold the address, and passes ownership of the buffer to the client.
	 * @return				KErrNone on successful completion, or one of the system error codes on failure.
	 */
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost)=0;	// // (out) the address of the local host

	/**
	 * Gets the bearer on which a received datagram arrived.
	 *
	 * Useful when EAll was specified in Connect()
	 *
	 * @param aBearer	On return, the bearer
	 * @return			KErrNone on successful completion, or one of the system error codes on failure.
	 */
	virtual TInt GetBearer(Wap::TBearer& /*aBearer*/)=0;
	
	/**
	 * Gets the remote server address.
	 * 
	 * @param aServerAddress	On return, the address of the remote server. Clients must pass 
	 * 							in a reference to a NULL HBufC8 pointer. The function allocates a new HBufC8 
	 * 							buffer to hold the address, and passes ownership of the buffer to the client.
	 * @return					KErrNone on successful completion, or one of the system error codes on failure.
	 */
	virtual TInt GetServerAddress(HBufC8*& aServerAddress)=0;	// // (out) the address of the remote server
	
protected: // methods
	IMPORT_C CWapBoundCLWSPService();
	IMPORT_C void ConstructL();

private: // ECOM
	// D'tor Key identification required for ECOM
	TUid iDtor_ID_Key;
};

/** 
* Fully-Specified Connectionless WSP
*  @released since v8.0
*/
class CWapFullySpecCLWSPService : public CBase
{
public: // creation/deletion
	IMPORT_C static CWapFullySpecCLWSPService* NewL();
	IMPORT_C static CWapFullySpecCLWSPService* NewL(const TUid& aImplementation);
	IMPORT_C virtual ~CWapFullySpecCLWSPService();

public: // API methods

	// Opens a socket which is to be used only with a single, named remote host.
	/** 
	* Connects to the WAP stack, opening an endpoint that can be used to listen for 
	* Push messages from a specified remote host.
	* 
	* All CWapFullySpecCLWSPService implementations must automatically close this 
	* endpoint upon destruction.
	* 
	* @param aRemoteHost	The bearer-dependent address of the remote host with which 
	*						the data will be exchanged
	* @param aRemotePort	The port on the remote host to which data will be sent
	* @param aBearer		The bearer to listen on. You cannot use EAll.
	* @param aSecure		Security flag to indicate whether WTLS should be used or not
	* @return				KErrNone on successful completion, or one of the system error codes on failure. 
	*/
	virtual TInt Connect(const TDesC8& aRemoteHost, Wap::TPort aRemotePort, Wap::TBearer aBearer, TBool aSecure)=0;

	/**
	 * Sends a request to a remote endpoint.
	 * 
	 * @param aMethod
	 * @param aURI
	 * @param aReqHeaders
	 * @param aReqBody
	 * @param aTransactionId
	 * @return					KErrNone on successful completion, or one of the system error codes on failure.
	 */ 
	virtual TInt MethodInvoke(TUint aMethod, const TDesC& aURI, const TDesC8& aReqHeaders, const TDesC8& aReqBody, TUint8 aTransactionId)=0;

	/**
	 * Waits for a datagram to be received
	 * 
	 * @param aReqHeaders
	 * @param aReqBody
	 * @param aTransactionIdPckg
	 * @param aWspStatus
	 * @param aReqStatus			Asynchonrous status word, used to signal when a data size is known
	 * @param aTimeout
	 * @return						KErrNone on successful completion, or one of the system error codes on failure.
	 */
	virtual TInt MethodResult(TDes8& aReqHeaders, TDes8& aReqBody, TPckgBuf<TUint8>& aTransactionIdPckg, TWSPStatus& aWspStatus, TRequestStatus& aReqStatus, TUint32 aTimeout)=0;


	/**
	 * Cancels a previously-requested MethodResult() notification. 
	 *
	 * If a message arrives the client will not be notified.
	 * 
	 */ 
	virtual void CancelReq()=0;

	/** 
	 * Gets the local port of this endpoint.
	 *
	 * This is useful if the port was chosen automatically.
 	 * 
 	 * @param aPort	On return, the port number
	 * @return		KErrNone on successful completion, or one of the system error codes on failure.
	 */
	virtual TInt GetLocalPort(Wap::TPort& aPort)=0;	// // (out) the port number

	/**
	 * Gets the local address of this endpoint.
	 *
	 * @param aLocalHost	On return, the address of the local host. Clients must pass 
	 * 						in a reference to a NULL HBufC8 pointer. The function allocates a new HBufC8 
	 * 						buffer to hold the address, and passes ownership of the buffer to the client.
	 * @return				KErrNone on successful completion, or one of the system error codes on failure.
	 */
	virtual TInt GetLocalAddress(HBufC8*& aLocalHost)=0;	// // (out) the address of the local host
	
protected: // methods
	IMPORT_C CWapFullySpecCLWSPService();
	IMPORT_C void ConstructL();

private: // ECOM
	// D'tor Key identification required for ECOM
	TUid iDtor_ID_Key;
};


// Utility class for client/implementation use.
class CWapMessageUtils : public CBase
/** Utility functions for use with WAP Messaging. */
	{
public:
	// // Return a list of all the addresses that are available
	IMPORT_C static TInt GetLocalAddressesL(RArray<Wap::TAddressInfo>& aAddressInfo); 
	};

	
#endif // __WAPMESSAGE_H__
