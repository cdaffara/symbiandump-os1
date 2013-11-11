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
// Implementation of g711 encoder config custom interface pair
// 
//

#ifndef G711ENCODERCONFIGCI_H
#define G711ENCODERCONFIGCI_H

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
Enum to represent the method called by this custom interface
@internalComponent
@prototype
@file
*/
enum TMMFDevSoundCIG711EncoderIntfcCommands
	{
	EMMFDevSoundCIG711EncoderIntfcSetEncoderMode = 0,
	EMMFDevSoundCIG711EncoderIntfcSetVadMode	 = 1,
	EMMFDevSoundCIG711EncoderIntfcGetVadMode	 = 2
	};


/**
Implementation of the g711 encoder config custom interface Mux
@internalComponent
@prototype
@file
*/
class CMMFG711EncoderIntfcMux : public CBase,
								public MMMFDevSoundCustomInterfaceMuxPlugin,
								public MG711EncoderIntfc  
	{
public:

	// from MMMFDevSoundCustomInterfaceMuxPlugin
	/**
	Attempt to open the interface.
	@param  aInterfaceId
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
	@leave This method may leave with one of the system-wide error codes.
	*/
	virtual void CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility);

	/**
	Return the custom interface
	@param  aInterfaceId
		    The UID of the required custom interface
	@return The custom interface supported by this plugin
	*/
	virtual TAny* CustomInterface(TUid aInterfaceId);

	/**
	Instantiate a CI Mux class
	@return The pointer to the new class, cast to the Mux plugin mixin
	@leave  This method may leave with one of the system-wide error codes.
	*/
	static MMMFDevSoundCustomInterfaceMuxPlugin* NewL();

	// from MG711EncoderIntfc
	//@see MG711EncoderIntfc
	virtual TInt SetEncoderMode (TEncodeMode aEncodeMode);
	//@see MG711EncoderIntfc
	virtual TInt SetVadMode (TBool aVadModeOn);
	//@see MG711EncoderIntfc
	virtual TInt GetVadMode (TBool& aVadModeOn);


protected:
	CMMFG711EncoderIntfcMux();
	~CMMFG711EncoderIntfcMux();

protected:
	TUid iKey;
	TInt iRemoteHandle;
	MMMFDevSoundCustomInterfaceMuxUtility* iUtility;
	};


/**
Implementation of the G711 Decoder config custom interface DeMux
@internalComponent
@prototype
@file
*/
class CMMFG711EncoderIntfcDeMux : public CBase,
								  public MMMFDevSoundCustomInterfaceDeMuxPlugin
	{
public:
	/**
	Instantiate a CMMFG711EncoderIntfcDeMux class
	@return A pointer to the new class cast to the DeMux plugin mixin
	@leave  This method may leave with one of the system-wide error codes.
	*/
	static MMMFDevSoundCustomInterfaceDeMuxPlugin* NewL();

	/**
	Attempt to open the interface.
	@param  aInterfaceId
		    The UID of the interface to open.
	@return A handle to the remote plugin
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
	@leave This method may leave with one of the system-wide error codes.
	*/
	virtual void CompleteConstructL(MMMFDevSoundCustomInterfaceDeMuxUtility* aUtility);

	/**
	Refresh the current custom interface connections
	@leave This method may leave with one of the system-wide error codes.
	*/
	virtual void RefreshL();

	// from MMMFDevSoundCustomInterfaceDeMuxPlugin
	/**
	Open the slave
	@param  aInterface
		    The UID of the requested interface
	@param  aPackageBuf
		    A package of data that can be supplied for initialisation
	@return The result of the operation
	@leave  This method may leave with one of the system-wide error codes.
	*/
	virtual TInt DoOpenSlaveL(TUid aInterface, const TDesC8& aPackageBuf);

	/**
	Close the slave
	@param aHandle
		   The handle of the slave plugin
	@leave This method may leave with one of the system-wide error codes.
	*/
	virtual void DoCloseSlaveL(TInt aHandle);

	/**
	Relay the synchronous custom command onto the slave
	@param  aMessage
		    The IPC message to be sent to the slave
	@return the result of the operation
	@leave  This method may leave with one of the system-wide error codes.
	*/
	virtual TInt DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage);

	/**
	Relay the synchronous custom command onto the slave and obtain a result
	@param  aMessage
		    The IPC message to be sent to the slave
	@return the result of the operation
	@leave  This method may leave with one of the system-wide error codes.
	*/
	virtual TInt DoSendSlaveSyncCommandResultL(const RMmfIpcMessage& aMessage);

	/**
	Relay an asynchronous command onto the slave
	@param aMessage
		   The IPC message to be sent to the slave
	@leave This method may leave with one of the system-wide error codes.
	*/
	virtual void DoSendSlaveAsyncCommandL(const RMmfIpcMessage& aMessage);

	/**
	Relay an asynchronous command onto the slave and obtain a result
	@param aMessage
		   The IPC message to be sent to the slave@param aMessage
	@leave This method may leave with one of the system-wide error codes.
	*/
	virtual void DoSendSlaveAsyncCommandResultL(const RMmfIpcMessage& aMessage);

protected:
	~CMMFG711EncoderIntfcDeMux();
	CMMFG711EncoderIntfcDeMux();

	// from mirror MG711EncoderIntfc method.
	TInt DoSetEncoderMode (MG711EncoderIntfc::TEncodeMode aEncodeMode);
	TInt DoSetVadMode (TBool aVadModeOn);
	TInt DoGetVadMode (TBool& aVadModeOn);
	
protected:
	MMMFDevSoundCustomInterfaceDeMuxUtility* iUtility;
	MMMFDevSoundCustomInterfaceTarget* iTarget;
	TUid				iKey;
	MG711EncoderIntfc*	iInterfaceG711EncoderIntfc; 
	};

#endif 
