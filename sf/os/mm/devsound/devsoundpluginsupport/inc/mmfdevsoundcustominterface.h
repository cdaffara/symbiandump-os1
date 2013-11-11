// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @released
 @file
*/

#ifndef MMFDEVSOUNDCUSTOMINTERFACE_H
#define MMFDEVSOUNDCUSTOMINTERFACE_H

// forward reference
class MMMFDevSoundCustomInterfaceMuxPlugin;
class MMMFDevSoundCustomInterfaceDeMuxPlugin;

/**
* structure to hold command, handle pair
*/
class TMMFDevSoundCustomInterfaceCommand
	{
public:
	TInt iCommand;
	TInt iHandle;
	};

typedef TPckgBuf<TMMFDevSoundCustomInterfaceCommand> TMMFDevSoundCustomInterfaceCommandPackage;

/**
* structure to hold custom interface data
*/
class TMMFDevSoundCustomInterfaceData
	{
public:	
	TUid iId;
	TInt iHandle;
	MMMFDevSoundCustomInterfaceMuxPlugin* iInterface;
	};

/**
* structure to hold custom interface data on server side
*/
class TMMFDevSoundCustomInterfaceDeMuxData
	{
public:
	TUid iId;
	MMMFDevSoundCustomInterfaceDeMuxPlugin* iInterface;	
	};


/**
* mixin for custom command pipeline used by utility to reroute commands
*/
class MMMFDevSoundCustomInterfaceChannel
	{
public:
	/**
	Implements a synchronous custom command
	@param aUid
		   The UID of the custom command
	@param aParam1
		   A buffer of data to be supplied to the receiver
	@param aParam2
		   A buffer of data to be supplied to the receiver
	@param aOutParam
		   A buffer that will be written into by the receiver
		   and returned to the client
	@return the result of the custom command
	 */
	virtual TInt SyncCustomCommand(TUid aUid, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam) = 0;

	/**
	Implements an asynchronous custom command
	@param aUid
		   The UID of the custom command
	@param aStatus
		   The request status of the active object that
		   will be called upon the completion of the request
	@param aParam1
		   A buffer of data to be supplied to the receiver
	@param aParam2
		   A buffer of data to be supplied to the receiver
	@param aOutParam
		   A buffer that will be written into by the receiver
		   and returned to the client
	 */
	virtual void AsyncCustomCommand(TUid aUid, TRequestStatus& aStatus, const TDesC8& aParam1, const TDesC8& aParam2, TDes8* aOutParam) = 0;
	};


/**
* custom interface API mixin
*/
class MMMFDevSoundCustomInterfaceTarget
	{
public:	
	/**
	Obtain a custom interface
	@param aInterfaceId
		   The UID of the required custom interface
	@return a pointer to the custom interface
	 */
	virtual TAny* CustomInterface(TUid aInterfaceId) = 0;
	};


/**
* the custom interface MUX Mixin class implemented by the mux utility
*/
class MMMFDevSoundCustomInterfaceMuxUtility
	{
public:	
	/**
	Attempt to open the slave plugin
	@param aInterface
		   The UID of the requested slave
	@param aPackageBuf
		   A buffer that can be used to supply initialisation data
	@return one of the system-wide error codes
	 */
	virtual TInt OpenSlave(TUid aInterface, const TDesC8& aPackageBuf) = 0;

	/**
	Close the slave plugin
	@param aHandle
		   The handle of the slave to close
	 */
	virtual void CloseSlave(TInt aHandle) = 0;

	/**
	Send a synchronous command to the slave
	@param aHandle
		   The handle of the remote slave plugin
	@param aCommand
		   The command to send to the slave
	@param aPackageBuf
		   Buffer of data to send to slave
	@return the return code from the command
	 */
	virtual TInt SendSlaveSyncCommand(TInt aHandle, TInt aCommand, const TDesC8& aPackageBuf) = 0;
	
	/**
	Send a synchronous command to the slave and obtain a result
	@param aHandle
		   The handle of the remote slave plugin
	@param aCommand
		   The command to send to the slave
	@param aPackageBuf
		   Buffer of data to send to slave
	@param aResultBuf
		   Buffer to contain the result from the destination.
		   This must be large enough to contain the result.
	@return the return code from the command
	 */
	virtual TInt SendSlaveSyncCommandResult(TInt aHandle, TInt aCommand, const TDesC8& aPackageBuf, TDes8& aResultBuf) = 0; 
	
	/**
	Send an asychronous command to the slave
	@param aComPackage
		   A package containing the handle of the remote slave and
		   the command to be sent
	@see TMMFDevSoundCustomInterfaceCommandPackage
	@param aStatus
		   The request status of the active object that will called
		   upon the completion of the request
	@param aPackageBuf
		   Buffer of data to send to slave
	 */
	virtual void SendSlaveAsyncCommand(TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf) = 0;
	
	/**
	Send an asychronous command to the slave and obtain a result
	@param aComPackage
		   A package containing the handle of the remote slave and
		   the command to be sent
	@see TMMFDevSoundCustomInterfaceCommandPackage
	@param aStatus
		   The request status of the active object that will be called
		   upon the completion of the request
	@param aPackageBuf
		   Buffer of data to send to slave
	@param aResultBuf
		   Buffer to contain the result from the destination.
		   This must be large enough to contain the result.
	 */
	virtual void SendSlaveAsyncCommandResult(TMMFDevSoundCustomInterfaceCommandPackage& aComPackage, TRequestStatus& aStatus, const TDesC8& aPackageBuf, TDes8& aResultBuf) = 0;	
	};


/**
* mixin implemented by Mux plugin itself
*/
class MMMFDevSoundCustomInterfaceMuxPlugin
	{
public:
	/**
	Attempt to open the interface.
	@param aInterfaceId
		   The UID of the required interface
	@return one of the system wide error codes
	*/
	virtual TInt OpenInterface(TUid aInterfaceId) = 0;

	/**
	Equivalent to destructor.  Called to destroy plugin.
	*/
	virtual void Release() = 0;
	/**
	Pass destructor key.
	Called on construction so plugin takes responsibility to call REComSession::DestroyedImplementation()
	@param aDestructorKey
	       The Uid returned by REComSession::CreateImplementationL() or similar
	*/
	virtual void PassDestructorKey(TUid aDestructorKey) = 0;
	
	/**
	Complete construction.
	Pass additional values from the construction phase, used subsequently by the plugin.
	@param aCustomUtility
	       The custom interface utility used by the plugin to communicate with the remote
		   server side DeMux plugin
	*/
	virtual void CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility) = 0;

	/** 
	Return the custom interface
	@param aInterfaceId
		   The id of the required custom interface
	@return the custom interface supported by this plugin
	*/
	virtual TAny* CustomInterface(TUid aInterfaceId) = 0;
	};


/**
* the custom interface DEMUX Mixin class implemented by the DeMux interface
* this is likely to be the DevSound server implementation
*/
class MMMFDevSoundCustomInterfaceDeMuxInterface
	{
public:	
	/**
	Open the slave
	@param aInterface
		   The UID of the requested interface
	@param aPackageBuf
		   A package of data that can be supplied for initialisation
	@return the result of the operation
	 */
	virtual TInt DoOpenSlaveL(TUid aInterface, const TDesC8& aPackageBuf) = 0;

	/** 
	Close the slave
	@param aHandle
		   The handle of the slave plugin
	 */
	virtual void DoCloseSlaveL(TInt aHandle) = 0;

	// original RMessage is supplied so that remote demux plugin can extract necessary details
	// using DeMux utility

	/**
	Relay the synchronous custom command onto the slave
	@param aMessage
		   The IPC message to be sent to the slave
	@return the result of the operation
	 */
	virtual TInt DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage) = 0;

	/**
	Relay the synchronous custom command onto the slave and obtain a result
	@param aMessage
		   The IPC message to be sent to the slave
	@return the result of the operation
	 */
	virtual TInt DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage) = 0;

	/**
	Relay an asynchronous command onto the slave
	@param aMessage
		   The IPC message to be sent to the slave
	 */
	virtual void DoSendSlaveAsyncCommandL(const RMmfIpcMessage& aMessage) = 0;

	/**
	Relay an asynchronous command onto the slave and obtain a result
	@param aMessage
		   The IPC message to be sent to the slave@param aMessage
	 */
	virtual void DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& aMessage) = 0;
	};


/**
* message data structure for demux utility
*/
class TMMFDevSoundCIMessageData
	{
public:
	TInt iCommand;
	TInt iHandle;
	};


/**
* mixin class implemented by the demux utility
*/
class MMMFDevSoundCustomInterfaceDeMuxUtility
	{
public:
	/**
	Get all settings at once into a client supplied package
	@param aMessage
		   The IPC message
	@param aData
		   Structure containing the command and handle
	@see TMMFDevSoundCIMessageData
	 */
	virtual void GetSyncMessageDataL(const RMmfIpcMessage& aMessage, TMMFDevSoundCIMessageData& aData) = 0;

	/**
	Get all settings at once into a client supplied package
	from an asynchronous message
	@param aMessage
		   The IPC message
	@param aData
		   Structure containing the command and handle
	@see TMMFDevSoundCIMessageData
	 */
	virtual void GetAsyncMessageDataL(const RMmfIpcMessage& aMessage, TMMFDevSoundCIMessageData& aData) = 0;
	
	/**
	Obtain the input descriptor length from the message
	@param aMessage
		   The IPC message
	@return the length of the input descriptor in the message
	 */
	virtual TInt InputDesLength(const RMmfIpcMessage& aMessage) = 0;

	/**
	Read the data from the input descriptor into the supplied buffer
	@param aMessage
		   The IPC message
	@param aBufToFill
		   The descriptor to copy the data from the input message into
	 */
	virtual void ReadFromInputDesL(const RMmfIpcMessage& aMessage, TDes8* aBufToFill) = 0;

	/**
	Write the supplied buffer into the output buffer in the IPC message
	@param aMessage
		   The IPC message
	@param aBufToWrite
		   The buffer to write to the output buffer in the supplied IPC message
	 */
	virtual void WriteToOutputDesL(const RMmfIpcMessage& aMessage, TDesC8& aBufToWrite) = 0;
	
	/**
	Complete the supplied IPC message with the supplied error code
	@param aMessage
		   The IPC message
	@param aError
		   One of the system wide error codes to complete the message with
	 */
	virtual void CompleteMessage(const RMmfIpcMessage& aMessage, TInt aError) = 0;
	};


/**
* mixin implemented by DeMux plugin itself
*/
class MMMFDevSoundCustomInterfaceDeMuxPlugin : public MMMFDevSoundCustomInterfaceDeMuxInterface
	{
public:
	/**
	Attempt to open the interface.
	@param aInterfaceId
		   The UID of the required interface
	@return a handle to the remote plugin
	*/
	virtual TInt OpenInterface(TUid aInterfaceId) = 0;

	/**
	Equivalent to destructor.  Called to destroy plugin.
	*/
	virtual void Release() = 0;
	/**
	Pass destructor key.
	Called on construction so plugin takes responsibility to call REComSession::DestroyedImplementation()
	@param aDestructorKey
	       The Uid returned by REComSession::CreateImplementationL() or similar
	*/
	virtual void PassDestructorKey(TUid aDestructorKey) = 0;
	
	/**
	Complete construction.
	Set the target of the custom interface call
	@param aDevSoundServer
	      The DevSound to call the custom interface on.
	*/
	virtual void SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget) = 0;

	/**
	Complete construction.
	@param aUtility
	       A pointer to the DeMux utility to use
	*/
	virtual void CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aUtility) = 0;

	/**
	Refresh the interfaces if circumstances / other areas of DevSound have changed
	*/
	virtual void RefreshL() = 0;
	};



#endif 
