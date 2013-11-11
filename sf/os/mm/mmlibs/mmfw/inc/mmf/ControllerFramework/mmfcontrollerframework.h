// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MMFCONTROLLERFRAMEWORK_H__
#define __MMFCONTROLLERFRAMEWORK_H__

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include <mmf/common/mmfipc.h>

// fwd ref
class CMMFControllerImplementationInformation;
class CLogonMonitor;

_LIT(KMMFControllerProxyServerName, "MMFControllerProxyServer-");

/**
@publishedAll
@released

Signals that the message is destined for the controller
*/
const TInt KMMFObjectHandleController = -2;

/**
@publishedAll
@released

Null, or invalid, object handle
*/
const TInt KMMFObjectHandleNull = 0;

/**
@publishedAll
@released

Describes the destination of a message in the controller framework.

The destination is defined by a UID describing the interface and an integer object handle.
The controller framework uses this information when it receives a message to decide where
to send the message.

The controller plugin and controller proxy use the special constant handles
KMMFObjectHandleControllerProxy and KMMFObjectHandleController for
efficiency.

@since 7.0s
*/
class TMMFMessageDestination
	{
public:

	/**
	Default constructor.
	
	Assigns a null UID to the interface, and a null handle to the handle.

	@since 7.0s
	*/
	IMPORT_C TMMFMessageDestination();

	/**
	Constructor for sending messages to the controller plugin.

	Assigns KMMFObjectHandleController to the destination handle.

	@param  aInterfaceId
	        The UID of the interface of this message destination.

	@since  7.0s
	*/
	IMPORT_C TMMFMessageDestination(TUid aInterfaceId);

	/**
	Full constructor.
	
	@param  aInterfaceId
	        The UID of the interface of this message destination.
	@param  aDestinationHandle
	        The handle of the destination object in the controller framework.

	@since  7.0s
	*/
	IMPORT_C TMMFMessageDestination(TUid aInterfaceId, TInt aDestinationHandle);

	/**
	Copy constructor.

	@param  aOther
	        The TMMFMessageDestination object to be copied.

	@since  7.0s
	*/
	IMPORT_C TMMFMessageDestination(const TMMFMessageDestination& aOther);

	/**
	Returns the UID of the interface of the message destination.

	@return	The interface UID.

	@since 7.0s
	*/
	IMPORT_C TUid InterfaceId() const;

	/**
	Returns the handle of the destination object in the controller framework.

	@return	The destination handle.

	@since 7.0s
	*/
	IMPORT_C TInt DestinationHandle() const;

	/**
	Compares two destinations.

	@param  aOther
	        The object to be compared with this object.

	@return A boolean indicating if the objects are the same. ETrue if the objects are the same, 
	        EFalse if the objects are different.

	@since 7.0s
	*/
	IMPORT_C TBool operator==(const TMMFMessageDestination& aOther) const;

private:
	TUid iInterfaceId;
	TInt iDestinationHandle;
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	TInt iReserved2;
	TInt iReserved3;
	};

/**
@publishedAll
@released

Package buffer for TMMFMessageDestination
*/
typedef TPckgBuf<TMMFMessageDestination> TMMFMessageDestinationPckg;


/**
@publishedAll
@released

Encapsulation of an RMessage or an RMessage2 to provide protection to controller plugins
from the changes occurring with secure APIs.

Adds required destination information to the message so the controller framework
can determine which object the message is for.

Adds capability to check whether the message has been completed.

Allows two descriptors to be read from the client, and one descriptor to be copied to the client.

@see RMMFControllerProxy::SendSync
@see RMMFControllerProxy::SendAsync

@since 7.0s
*/
class TMMFMessage
	{
friend class CMMFControllerProxySession;
friend class CMMFDevSoundSession;
public:

	/**
	Copy constructor.
	
	A controller plugin should use this to take a copy of the message when it must wait for some
	other event before completing it.

	@param  aMessage
	        The message object to copy.

	@since 7.0s
	*/
	IMPORT_C TMMFMessage(const TMMFMessage& aMessage);

	/**
	Returns the destination of the message.

	@return	The destination of the message, consisting of a UID defining the interface and the
			handle of the destination object.

	@since	7.0s
	*/
	IMPORT_C const TMMFMessageDestination& Destination();

	/**
	Returns an integer describing the function being called. Equivalent to 
	RMessage::Function().

	@return The function being called over IPC.

	@see    RMessage::Function()

	@since  7.0s
	*/
	IMPORT_C TInt Function();

	/**
	Returns the size of the first descriptor that can be copied over from the client.
	
	Use this to create a descriptor of appropriate size before reading the data from the client.

	@return	The size of data 1.
	@since	7.0s
	*/
	IMPORT_C TInt SizeOfData1FromClient();

	/**
	Copy the first descriptor from the client.
	
	@param  aDes
	        A descriptor defining the area of memory into which the data from the client
	        will be copied. Note, the descriptor must be of sufficient size.

	@return	One of the system-wide error codes. In particular, KErrBadDescriptor will be returned
	        if the client did not provide a valid descriptor.

	@since  7.0s
	*/
	IMPORT_C TInt ReadData1FromClient(TDes8& aDes);

	/**
	Copy the first descriptor from the client.

	This functino can leave with one of the system-wide error codes. In particular, the leave code 
	KErrBadDescriptor will be used if the client did not provide a valid descriptor.

	@param  aDes
	        A descriptor defining the area of memory into which the data from the client
	        will be copied. Note, the descriptor must be of sufficient size.

	@since 7.0s
	*/
	IMPORT_C void ReadData1FromClientL(TDes8& aDes);


	/**
	Returns the size of the second descriptor that can be copied over from the client.
	
	Use this to create a descriptor of appropriate size before reading the data from the client.

	@return The size of data 2.

	@since  7.0s
	*/
	IMPORT_C TInt SizeOfData2FromClient();

	/**
	Copies the second descriptor from the client.

	@param  aDes
	        A descriptor defining the area of memory into which the data from the client
	        will be copied. Note, the descriptor must be of sufficient size.

	@return One of the system-wide error codes. In particular, KErrBadDescriptor will be returned
	        if the client did not provide a valid descriptor.

	@since  7.0s
	*/
	IMPORT_C TInt ReadData2FromClient(TDes8& aDes);

	/**
	Copies the second descriptor from the client.

	This function can leave with one of the system-wide error codes. In particular, the leave code
	KErrBadDescriptor will be used if the client did not provide a valid descriptor.

	@param  aDes
	        A descriptor defining the area of memory into which the data from the client
	        will be copied. Note, the descriptor must be of sufficient size.

	@since  7.0s
	*/
	IMPORT_C void ReadData2FromClientL(TDes8& aDes);

	/**
	Copies data back to the client.

	@param  aDes
	        The data to be copied back to the client.  Note that the client must
	        have already created a descriptor of sufficient size for the data to be
	        copied into.

	@return One of the system-wide error codes. In particular, the error code KErrBadDescriptor will 
	        be returned if the client did not provide a valid descriptor.

	@since  7.0s
	*/
	IMPORT_C TInt WriteDataToClient(const TDesC8& aDes);

	/**
	Copies the data back to the client.

	This function can leave with one of the system-wide error codes. In particular, the leave code
	KErrBadDescriptor will be used if the client did not provide a valid descriptor.

	@param  aDes
	        The data to be copied back to the client. Note that the client must have already
	        created a descriptor of sufficient size for the data to be copied into.

	@since 7.0s
	*/
	IMPORT_C void WriteDataToClientL(const TDesC8& aDes);

	/**
	Signals to the client that its request has been completed.

	Equivalent to RMessage::Complete.

	@param  aReason
	        The result code to be given back to the client.

	@since  7.0s
	*/
	IMPORT_C void Complete(TInt aReason);
	
	IMPORT_C void AdoptFileHandleFromClientL(TInt aFsHandleIndex, TInt aFileHandleIndex, RFile& aFile);

	/**
	Checks whether this message has already been completed.

	@return A boolean indicating if the message has been completed. ETrue if the message has been 
	        completed, EFalse if not.

	@since  7.0s
	*/
	IMPORT_C TBool IsCompleted();

	/**
	Constructs a TMMFMessage from an RMmfIpcMessage.

	@param	aMessage
			The handle to the remote message.

	@since  7.0s
	*/
	IMPORT_C TMMFMessage(const RMmfIpcMessage& aMessage);

	/**
	Gets the destination info from the client.

	@since  7.0s
	*/
	IMPORT_C void FetchDestinationL();
private:
#ifdef __MMF_USE_IPC_V2__
	/**
	The encapsulated message
	*/
	RMessagePtr2 iMessage;
	/**
	The encapsulated function
	*/
	TInt iFunction;
	/**
	Private padding to preserve BC with earlier versions
	*/
	TInt iDummyPadding[6];
#else
	/**
	The RMessage encapsulated by this object.
	*/
	RMessage iMessage;
#endif // __MMF_USE_IPC_V2__

	/**
	The destination of this message.
	*/
	TMMFMessageDestination iDestination;

	/**
	Defines know about object completion state.
	*/
	TBool iAmCompleted;
	};

/**
@publishedAll

Mixin class used by CLogonMonitor to inform about its state

@since 9.3
*/
class MLogonMonitorObserver
	{
public:
	/**
	The monitored thread has been terminated.
	*/
	virtual void ThreadTerminated() = 0;
	};
	
/**
@publishedAll

RSessionBase-based client class to handle the creation of the controller thread and
message sending over IPC.

The controller proxy is responsible for sending and recieving messages over IPC; it does not
care exactly what those messages are.

Note the fact that, although this is an internal component, its size cannot be changed because 
it's embedded in a publishedAll class (RMMFController) and the size of the latter would also be
changed raising a binary compatibility problem.

@since 7.0s
*/
NONSHARABLE_CLASS(RMMFControllerProxy) : public RMmfSessionBase, public MLogonMonitorObserver
	{
public:

	/**
	Constructor.
	@since 7.0s
	*/
	IMPORT_C RMMFControllerProxy();

	/**
	Create a new thread and load a controller plugin into that thread.
	
	@param  aControllerUid
	        The UID of the controller plugin to be loaded.
	@param	aUseSharedHeap
			If this value is EFalse each controller is created with its own heap. The alternative,
			if the value is ETrue, is that controllers share a special heap with other controllers
			created the same way. Each heap uses a chunk, so this avoids situations where the
			number of chunks per process is limited. The default behaviour is generally to be
			preferred, and should give lower overall memory usage. However, if many controllers are
			to be created for a particular thread, then ETrue should be provided to prevent running
			out of heaps or chunks.

	@return One of the system-wide error codes.
	@since	7.0s
	*/
	IMPORT_C TInt LoadController(TUid aControllerUid, TBool aUseSharedHeap);

	/**
	Create a new thread and load a controller plugin into that thread.
	
	@param  aControllerInfo
	        A reference to a CMMFControllerImplementationInformation object
	        used for determining the controller UID and heap size
	@param	aUseSharedHeap
			If this value is EFalse each controller is created with its own heap. The alternative,
			if the value is ETrue, is that controllers share a special heap with other controllers
			created the same way. Each heap uses a chunk, so this avoids situations where the
			number of chunks per process is limited. The default behaviour is generally to be
			preferred, and should give lower overall memory usage. However, if many controllers are
			to be created for a particular thread, then ETrue should be provided to prevent running
			out of heaps or chunks.

	@return	One of the system-wide error codes.
	@since	7.0s
	*/
	IMPORT_C TInt LoadController(const CMMFControllerImplementationInformation& aControllerInfo, TBool aUseSharedHeap);

	/**
	Create a new thread in another process through Secure DRM Plugin server and load a controller plugin 
	into that thread.  The motivation of this model is to allow applications without DRM capability able 
	to play protected content through MMF.
	
	@param  aControllerUid
	        The UID of the controller plugin to be loaded.
	@param	aUseSharedHeap
			If this value is EFalse each controller is created with its own heap. The alternative,
			if the value is ETrue, is that controllers share a special heap with other controllers
			created the same way. Each heap uses a chunk, so this avoids situations where the
			number of chunks per process is limited. The default behaviour is generally to be
			preferred, and should give lower overall memory usage. However, if many controllers are
			to be created for a particular thread, then ETrue should be provided to prevent running
			out of heaps or chunks.

	@return One of the system-wide error codes.
	*/
	IMPORT_C TInt LoadControllerInSecureDRMProcess(TUid aControllerUid, TBool aUseSharedHeap);
	
	/**
	Create a new thread in another process through Secure DRM Plugin server and load a controller plugin 
	into that thread.  The motivation of this model is to allow applications without DRM capability able 
	to play protected content through MMF.
	
	@param  aControllerInfo
	        A reference to a CMMFControllerImplementationInformation object
	        used for determining the controller UID and heap size
	@param	aUseSharedHeap
			If this value is EFalse each controller is created with its own heap. The alternative,
			if the value is ETrue, is that controllers share a special heap with other controllers
			created the same way. Each heap uses a chunk, so this avoids situations where the
			number of chunks per process is limited. The default behaviour is generally to be
			preferred, and should give lower overall memory usage. However, if many controllers are
			to be created for a particular thread, then ETrue should be provided to prevent running
			out of heaps or chunks.

	@return	One of the system-wide error codes.
	*/
	IMPORT_C TInt LoadControllerInSecureDRMProcess(const CMMFControllerImplementationInformation& aControllerInfo, TBool aUseSharedHeap);
	
	/**
	Delete the controller plugin and close its thread.
	
	Clients must call Close() on the controller proxy, otherwise leaks will occur.

	@since	7.0s
	*/
	IMPORT_C void Close();

	/**
	Send a message synchronously to the controller thread.

	The controller framework will analyze the message to determine its correct destination.

	Note: This method will not return until the controller framework has serviced the command.

	@param  aDestination
	        The destination of the message, consisting of the uid of
	        the interface of this message.
	@param  aFunction
	        The function number to indicate which function is to be called
	        on the interface defined in the aDestination parameter.
	@param  aDataTo1
	        A reference to the first chunk of data to be copied to the controller
	        framework. The exact contents of the data are dependent on the
	        interface being called.  Can be NULL.
	@param  aDataTo2
	        A reference to the second chunk of data to be copied to the controller
	        framework. The exact contents of the data are dependent on the
	        interface being called.  Can be NULL.
	@param  aDataFrom
	        A reference to an area of memory to which the controller framework will
	        write any data to be passed back to the client.  Can't be NULL.

	@return The result of the request.  Exact range of values is dependent on the interface.
	@since	7.0s
	*/
	IMPORT_C TInt SendSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom) const;

	/**
	Send a message synchronously to the controller thread.

	The controller framework will analyze the message to determine its correct destination.

	Note: This method will not return until the controller framework has serviced the command.

	@param  aDestination
	        The destination of the message, consisting of the uid of
	        the interface of this message.
	@param  aFunction
	        The function number to indicate which function is to be called
	        on the interface defined in the aDestination parameter.
	@param  aDataTo1
	        A reference to the first chunk of data to be copied to the controller
	        framework. The exact contents of the data are dependent on the
	        interface being called.  Can be NULL.
	@param  aDataTo2
	        A reference to the second chunk of data to be copied to the controller
	        framework. The exact contents of the data are dependent on the
	        interface being called.  Can be NULL.

	@return The result of the request.  Exact range of values is dependent on the interface.
	@since	7.0s
	*/
	IMPORT_C TInt SendSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2) const;

	/**
	Send a message asynchronously to the controller thread.
	
	The controller framework will analyze the message to determine its correct destination.

	Note: This method will return immediately.  The RunL of the active object owning the
	aStatus parameter will be called when the command is completed by the
	controller framework.

	@param  aDestination
	        The destination of the message, consisting of the uid of
	        the interface of this message.
	@param  aFunction
	        The function number to indicate which function is to be called
	        on the interface defined in the aDestination parameter.
	@param  aDataTo1
	        A reference to the first chunk of data to be copied to the controller
	        framework. The exact contents of the data are dependent on the
	        interface being called.  Can be NULL.
	@param  aDataTo2
	        A reference to the second chunk of data to be copied to the controller
	        framework. The exact contents of the data are dependent on the
	        interface being called.  Can be NULL.
	@param  aDataFrom
	        A reference to an area of memory to which the controller framework will
	        write any data to be passed back to the client.  Can't be NULL.
	@param  aStatus
	        The TRequestStatus of an active object.  This will contain the
	        result of the request on completion.  The exact range of
	        result values is dependent on the interface.

	@since	7.0s
	*/
	IMPORT_C void SendAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction,  const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus) const;

	/**
	Send a message asynchronously to the controller thread.
	
	The controller framework will analyze the message to determine its correct destination.
	
	Note: This method will return immediately.  The RunL of the active object owning the 
	aStatus parameter will be called when the command is completed by the
	controller framework.

	@param  aDestination
	        The destination of the message, consisting of the uid of
	        the interface of this message.
	@param  aFunction
	        The function number to indicate which function is to be called
	        on the interface defined in the aDestination parameter.
	@param  aDataTo1
	        A reference to the first chunk of data to be copied to the controller
	        framework. The exact contents of the data are dependent on the
	        interface being called.  Can be NULL.
	@param  aDataTo2
	        A reference to the second chunk of data to be copied to the controller
	        framework. The exact contents of the data are dependent on the
	        interface being called.  Can be NULL.
	@param  aStatus
	        The TRequestStatus of an active object.  This will contain the
	        result of the request on completion.  The exact range of
	        result values is dependent on the interface.

	@since	7.0s
	*/
	IMPORT_C void SendAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction,  const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus) const;

	IMPORT_C TInt SendSync(TInt aFunction, const TIpcArgs& aIpcArgs) const;
	IMPORT_C void SendAsync(TInt aFunction, const TIpcArgs& aIpcArgs, TRequestStatus& aStatus) const;
	/**
	Register to receive events from the controller framework.
	
	Events can be generated at any time, and are generally associated with things that occur
	due to something happening internally within the controller. For example, an event will
	be generated if the controller stops playing due to reaching the end of a file.

	@param  aEvent
	        A reference to a TMMFEventPckg object that must be member data
	        of the active object calling this method.  The details of the event
	        will be copied into this object when an event occurs.
	@param  aStatus
	        The TRequestStatus of the active object calling this method.

	@see	TMMFEvent
	@see	CMMFControllerEventMonitor
	@since	7.0s
	*/
	IMPORT_C void ReceiveEvents(TMMFEventPckg& aEvent, TRequestStatus& aStatus);

	/**
	Cancel a previous registration to receive events from the controller framework.

	This must be called from the DoCancel() method of the active object using the
	ReceiveEvents() API.

	@return One of the standard system-wide error codes.

	@see	TMMFEvent
	@since	7.0s
	*/
	IMPORT_C TInt CancelReceiveEvents();
	
	/**
	Set the priority of the controller's sub thread.
	
	This can be used to increase the responsiveness of the audio plugin to minimise
	any lag in processing. This function should be used with care as it may have knock-on
	effects elsewhere in the system.
	
	@param	aPriority
			The TThreadPriority that the thread should run under.  The default is EPriorityNormal.
	@return	TInt
			A standard error code: KErrNone if successful, KErrNotReady if the thread does not have a
			valid handle.
	*/
	IMPORT_C TInt SetThreadPriority(const TThreadPriority& aPriority) const;
	
	/**
	The controller subthread has been terminated.
	
	This is called by CLogonMonitor when the the thread terminates
	*/
	void ThreadTerminated();
private:
	/**
	Creates the new thread
	*/
	TInt DoCreateSubThread(RServer2* aServer2, TUint aMaxHeapSize, TBool aUseSharedHeap, TUint aStackSize);

	/**
	Determines the maximum heap size required by the selected controller
	*/
	TUint ControllersMaxHeapSizeL(TUid aControllerUid);

	/**
	Load a controller by UID
	
	@param  aControllerUid
	        The UID of the controller plugin to be loaded.
	@param  aControllerInfo
	        A reference to a CMMFControllerImplementationInformation object
	        used for determining the heap size and DRM server supporting state.
	@param	aUseSharedHeap
			If this value is EFalse each controller is created with its own heap. The alternative,
			if the value is ETrue, is that controllers share a special heap with other controllers
			created the same way. Each heap uses a chunk, so this avoids situations where the
			number of chunks per process is limited. The default behaviour is generally to be
			preferred, and should give lower overall memory usage. However, if many controllers are
			to be created for a particular thread, then ETrue should be provided to prevent running
			out of heaps or chunks.
	@param	aNoDRMCap
			The indicator supplied from user used to determine whether the user's process has DRM 
			capability
	@return	TInt
			One of the system-wide error codes.
	*/
	TInt LoadController(TUid aControllerUid, const CMMFControllerImplementationInformation& aControllerInfo, TBool aUseSharedHeap,
						TBool aNoDRMCap = EFalse);
	
	/**
	Release heap - for use when a shared heap has been registered
	*/
	void ReleaseHeap();

	/**
	Create a session to handle situation of playing DRM protected content in
	process with no DRM capability.
	DRM server process will be created, if not yet created. This server is responsible to
	manage and monitor all the MMF Controller servers created as seperate threads belong to 
	DRM server process. Once the new MMF Controller server is running, a MMF Controller server 
	session will be created and the handle is passing back.
	*/
	TInt DoCreateSessionForNoDRMCapClient(TUint aMaxHeapSize, TBool aUseSharedHeap, TUint aStackSize);
	
private:
	enum
		{
		/**
		Shared heap has been opened for this object, and must be closed
		*/
		EFlagOpenedSharedHeap=0x01	
		};

private:
	/**
	The thread in which the controller plugin will run.
	*/
	RThread iSubThread;
	/**
	Used to hold destination for messages from the controller proxy
	(not from clients calling SendSync/SendAsync!!)
	*/
	TMMFMessageDestinationPckg iDestinationPckg;
	/**
	Used to determine the success of a logon.  If the status is not pending, the logon has failed
	and the thread should be closed.
	*/
	CLogonMonitor* iLogonAO;
	/**
	The priority at which the controller plugin thread runs.
	*/
	TThreadPriority iThreadPriority;
	/**
	Flags field
	*/
	TInt iFlags;	
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	};

/**
@publishedAll
@released

Mixin class to define an object capable of handling an event within the controller framework.
*/
class MAsyncEventHandler
	{
public:

	/**
	Sends an event to the client.

	@param  aEvent
	        The event.

	@since 7.0s
	*/
	virtual TInt SendEventToClient(const TMMFEvent& aEvent) = 0;
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfcontrollerframeworkclasses.h>
#endif

#endif
