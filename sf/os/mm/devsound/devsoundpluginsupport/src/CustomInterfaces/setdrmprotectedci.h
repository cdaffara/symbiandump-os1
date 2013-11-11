// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SETDRMPROTECTEDCI_H
#define SETDRMPROTECTEDCI_H

#include <e32base.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include <mmf/server/mmfdevsoundcustominterface.h>
#include <mmf/server/mmfdevsoundcustomcommands.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>
#include <mmf/plugin/mmfdevsoundcustominterface.hrh>

/**
 @internalComponent
 @prototype
 @file
 */
enum TMMFDevSoundCISetDRMProtectedCommands
	{
	EMMFDevSoundCISetDRMProtected = 0
	};


/**
 @internalComponent
 @prototype
 @file
 */

/**
 * CMMFSetDRMProtectedMux
 *
 * implementation of the Play back status custom interface pair
 */
class CMMFSetDRMProtectedMux : public CBase,
			      	          public MMMFDevSoundCustomInterfaceMuxPlugin,
			      	 	      public MMMFSetDRMProtected
	 {
public:

	// from MMMFDevSoundCustomInterfaceMuxPlugin
	/**
	Attempt to open the interface.
	@param aInterfaceId
		   The UID of the interface to open.
	@return One of the system wide error codes
	*/
	virtual TInt OpenInterface(TUid aInterfaceId);

	/**
	Equivalent to destructor.  Called to destroy plugin.
	*/
	virtual void Release();
	/**
	Pass destructor key.
	Called on construction so plugin takes responsibility to call REComSession::DestroyedImplementation()
	@param aDestructorKey
	       The Uid returned by REComSession::CreateImplementationL() or similar
	*/
	virtual void PassDestructorKey(TUid aDestructorKey);
	
	/**
	Complete construction.
	Pass additional values from the construction phase, used subsequently by the plugin.
	@param aCustomUtility
	       The custom interface utility used by the plugin to communicate with the remote
		   server side DeMux plugin
	*/
	virtual void CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility);

	/** 
	Return the custom interface
	@param aInterfaceId
		   The UID of the required custom interface
	@return the custom interface supported by this plugin
	*/
	virtual TAny* CustomInterface(TUid aInterfaceId);

	/**
	Instantiate a CI Mux class
	@return the pointer to the new class, cast to the Mux plugin mixin
	*/
	static MMMFDevSoundCustomInterfaceMuxPlugin* NewL();

	// from MMMFSetDRMProtected
	/** 
	@see MMMFSetDRMProtected
	*/
	virtual TInt MmsdpMarkDataAsDRMProtected(TBool aDRMProtected);

protected:
	CMMFSetDRMProtectedMux();
	~CMMFSetDRMProtectedMux();

protected:
	TUid iDestructorKey;
	TInt iRemoteHandle;
	MMMFDevSoundCustomInterfaceMuxUtility* iUtility;
	};


/**
 @internalComponent
 @prototype
 @file
 */
 
/**
 * CMMFSetDRMProtectedDeMux - DeMux side of pair
 */

class CMMFSetDRMProtectedDeMux : public CBase,
			      	   				public MMMFDevSoundCustomInterfaceDeMuxPlugin
	{
public:
	/**
	Instantiate a CMMFSetDRMProtectedDeMux class
	@return a pointer to the new class cast to the DeMux plugin mixin
	*/
	static MMMFDevSoundCustomInterfaceDeMuxPlugin* NewL();
		
	/**
	Attempt to open the interface.
	@param aInterfaceId
		   The UID of the interface to open.
	@return a handle to the remote plugin
	*/
	virtual TInt OpenInterface(TUid aInterfaceId);

	/**
	Equivalent to destructor.  Called to destroy plugin.
	*/
	virtual void Release();
	/**
	Pass destructor key.
	Called on construction so plugin takes responsibility to call REComSession::DestroyedImplementation()
	@param aDestructorKey
	       The Uid returned by REComSession::CreateImplementationL() or similar
	*/
	virtual void PassDestructorKey(TUid aDestructorKey);
	
	/**
	Set the target of the custom interface call
	@param aTarget
	       The DevSound to call the custom interface on.
	*/
	virtual void SetInterfaceTarget(MMMFDevSoundCustomInterfaceTarget* aTarget);
	
	/**
	Complete construction.
	@param aUtility
	       The DeMux utility to use
	*/
	virtual void CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aUtility);

	/**
	Refresh the current custom interface connections
	*/
	virtual void RefreshL();

	// from MMMFDevSoundCustomInterfaceDeMuxPlugin
	/**
	Open the slave
	@param aInterface
		   The UID of the requested interface
	@param aPackageBuf
		   A package of data that can be supplied for initialisation
	@return the result of the operation
	 */
	virtual TInt DoOpenSlaveL(TUid aInterface, const TDesC8& aPackageBuf);

	/** 
	Close the slave
	@param aHandle
		   The handle of the slave plugin
	 */
	virtual void DoCloseSlaveL(TInt aHandle);

	// original RMessage is supplied so that remote demux plugin can extract necessary details
	// using DeMux utility

	/**
	Relay the synchronous custom command onto the slave
	@param aMessage
		   The IPC message to be sent to the slave
	@return the result of the operation
	 */
	virtual TInt DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage);

	/**
	Relay the synchronous custom command onto the slave and obtain a result
	@param aMessage
		   The IPC message to be sent to the slave
	@return the result of the operation
	 */
	virtual TInt DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage);

	/**
	Relay an asynchronous command onto the slave
	@param aMessage
		   The IPC message to be sent to the slave
	 */
	virtual void DoSendSlaveAsyncCommandL(const RMmfIpcMessage& aMessage);

	/**
	Relay an asynchronous command onto the slave and obtain a result
	@param aMessage
		   The IPC message to be sent to the slave@param aMessage
	 */
	virtual void DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& aMessage);

protected:
	~CMMFSetDRMProtectedDeMux();
	CMMFSetDRMProtectedDeMux();

	// mirrors MMMFSetDRMProtected method.
	TInt DoMmsdpMarkDataAsDRMProtected(TBool aDRMProtected);
	
protected:
	MMMFDevSoundCustomInterfaceDeMuxUtility* iUtility;
	MMMFDevSoundCustomInterfaceTarget* iTarget;
	TUid iDestructorKey;
	MMMFSetDRMProtected* iInterfaceSetDRMProtected;
	};

#endif 
