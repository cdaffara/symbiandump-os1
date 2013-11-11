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
// Implementation of DSP control custom interface pair
// 
//

#ifndef DSPCONTROLCI_H
#define DSPCONTROLCI_H

#include <e32base.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipc.h>
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
enum TMMFDevSoundCIDspControlCommands
	{
	EMMFDevSoundCIDspCtrlGetAudioPlaybackInfo,
	EMMFDevSoundCIDspCtrlAcceptRecordBuffersInvalidFollowingStop,
	EMMFDevSoundCIDspCtrlAcceptPlaybackBuffersInvalidFollowingStop
	};

/**
class to hold the paramenters to MmdspcGetAudioPlaybackInfo for message passing
@internalComponent
@prototype
@file
*/
class TAudioPlaybackInfo 
	{
public:
	TInt64 iSamplesPlayed;
	TInt64 iSystemTime;
	TUint  iBytesDecoderConsumed;
	TUint  iBytesDecoderDecoded;
	};

/**
Implementation of the DSP control custom interface Mux
@internalComponent
@prototype
@file
*/
class CMMFDspControlMux : public CBase,
						  public MMMFDevSoundCustomInterfaceMuxPlugin,
						  public MMMFDSPControl
	{
public:

	// from MMMFDevSoundCustomInterfaceMuxPlugin
	/**
	Attempt to open the interface.
	@param  aInterfaceId
		    The UID of the interface to open.
	@return one of the system wide error codes
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
	@return the custom interface supported by this plugin
	*/
	virtual TAny* CustomInterface(TUid aInterfaceId);

	/**
	Instantiate a CI Mux class
	@return the pointer to the new class, cast to the Mux plugin mixin
	@leave  This method may leave with one of the system-wide error codes.
	*/
	static MMMFDevSoundCustomInterfaceMuxPlugin* NewL();

	// from MMMFDSPControl
	//@see MMMFDSPControl
	virtual TInt MmdspcGetAudioPlaybackInfo(TInt64& aSamplesPlayed,
											TInt64& aSystemTime,
											TUint& aBytesDecoderConsumed,
											TUint& aBytesDecoderDecoded);
	//@see MMMFDSPControl
	virtual TInt MmdspcAcceptRecordBuffersInvalidFollowingStop();
	//@see MMMFDSPControl
	virtual TInt MmdspcAcceptPlaybackBuffersInvalidFollowingStop();

protected:
	CMMFDspControlMux();
	~CMMFDspControlMux();

protected:
	TUid iKey;
	TInt iRemoteHandle;
	MMMFDevSoundCustomInterfaceMuxUtility* iUtility;
	};


/**
Implementation of the DSP control custom interface DeMux
@internalComponent
@prototype
@file
*/
class CMMFDspControlDeMux : public CBase,
							public MMMFDevSoundCustomInterfaceDeMuxPlugin
	{
public:
	/**
	Instantiate a CMMFDspControlDeMux class
	@return a pointer to the new class cast to the DeMux plugin mixin
	@leave  This method may leave with one of the system-wide error codes.
	*/
	static MMMFDevSoundCustomInterfaceDeMuxPlugin* NewL();

	/**
	Attempt to open the interface.
	@param  aInterfaceId
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
	@return the result of the operation
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

	// original RMessage is supplied so that remote DeMux plugin can extract necessary details
	// using DeMux utility

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
	~CMMFDspControlDeMux();
	CMMFDspControlDeMux();

	// from mirror MMMFDSPControl method.
	TInt DoMmdspcGetAudioPlaybackInfoL(TInt64& aSamplesPlayed, TInt64& aSystemTime, TUint& aB, TUint& aT);
	TInt DoMmdspcAcceptRecordBuffersInvalidFollowingStopL();
	TInt DoMmdspcAcceptPlaybackBuffersInvalidFollowingStopL(); 

protected:
	MMMFDevSoundCustomInterfaceDeMuxUtility* iUtility;
	MMMFDevSoundCustomInterfaceTarget*		 iTarget;
	TUid			iKey;
	MMMFDSPControl* iInterfaceDspControl; 
	};

#endif 
