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

#ifndef EAACPLUSDECODERCI_H
#define EAACPLUSDECODERCI_H

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
 Enum to represent the methods called by this custom interface
 @internalComponent
 @prototype
 @file
 */
enum TMMFDevSoundCIEAacPlusDecoderCommands
	{
	EMMFDevSoundCIEAacPlusDecoderSetInputSamplingFrequency = 0,
	EMMFDevSoundCIEAacPlusDecoderSetAudioObjectType,
	EMMFDevSoundCIEAacPlusDecoderSetNumOfChannels,
	EMMFDevSoundCIEAacPlusDecoderSetSbr,
	EMMFDevSoundCIEAacPlusDecoderSetDownSampledMode,
	EMMFDevSoundCIEAacPlusDecoderApplyConfig,
	EMMFDevSoundCIEAacPlusDecoderGetInputSamplingFrequency,
	EMMFDevSoundCIEAacPlusDecoderGetAudioObjectType,
	EMMFDevSoundCIEAacPlusDecoderGetNumOfChannels,
	EMMFDevSoundCIEAacPlusDecoderGetSbr,
	EMMFDevSoundCIEAacPlusDecoderGetDownSampledMode
	};


/**
 CMMFEAacPlusDecoderMux - implementation of the EAAC+ Decoder custom interface pair
 @internalComponent
 @prototype
 @file
 */
class CMMFEAacPlusDecoderMux : public CBase,
							   public MMMFDevSoundCustomInterfaceMuxPlugin,
							   public MEAacPlusDecoderIntfc  
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
	@leave This method may leave with one of the system-wide error codes.
	*/
	virtual void CompleteConstructL(MMMFDevSoundCustomInterfaceMuxUtility* aCustomUtility);

	/** 
	Return the custom interface
	@param aInterfaceId
		   The UID of the required custom interface
	@return The custom interface supported by this plugin
	*/
	virtual TAny* CustomInterface(TUid aInterfaceId);

	/**
	Instantiate a CI Mux class
	@leave This method may leave with one of the system-wide error codes.
	@return The pointer to the new class, cast to the Mux plugin mixin
	*/
	static MMMFDevSoundCustomInterfaceMuxPlugin* NewL();

	// from MEAacPlusDecoderIntfc
	/** 
	@see MEAacPlusDecoderIntfc
	*/
	virtual void SetInputSamplingFrequency(TUint aInputSamplingFrequency);
	
	/** 
	@see MEAacPlusDecoderIntfc
	*/
	virtual void SetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType aAudioObjectType);
	
	/** 
	@see MEAacPlusDecoderIntfc
	*/
	virtual void SetNumOfChannels(TUint aNumOfChannels);
	
	/** 
	@see MEAacPlusDecoderIntfc
	*/
	virtual void SetSbr(TBool aSbrEnabled);
	
	/** 
	@see MEAacPlusDecoderIntfc
	*/
	virtual void SetDownSampledMode(TBool aDsmEnabled);
	
	/** 
	@see MEAacPlusDecoderIntfc
	*/
	virtual TInt ApplyConfig();
	
	/** 
	@see MEAacPlusDecoderIntfc
	*/
	virtual TInt GetInputSamplingFrequency ( TUint& aInputSamplingFrequency );
	
	/** 
	@see MEAacPlusDecoderIntfc
	*/
	virtual TInt GetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType& aAudioObjectType);
	
	/** 
	@see MEAacPlusDecoderIntfc
	*/
	virtual TInt GetNumOfChannels ( TUint& aNumOfChannels );
	
	/** 
	@see MEAacPlusDecoderIntfc
	*/
	virtual TInt GetSbr (TBool& aSbrEnabled);
	
	/** 
	@see MEAacPlusDecoderIntfc
	*/
	virtual TInt GetDownSampledMode (TBool& aDsmEnabled);

protected:
	CMMFEAacPlusDecoderMux();
	~CMMFEAacPlusDecoderMux();

protected:
	TUid iDestructorKey;
	TInt iRemoteHandle;
	MMMFDevSoundCustomInterfaceMuxUtility* iUtility;
	};

/**
 CMMFEAacPlusDecoderDeMux - DeMux side of pair
 @internalComponent
 @prototype
 @file
 */
class CMMFEAacPlusDecoderDeMux : public CBase, public MMMFDevSoundCustomInterfaceDeMuxPlugin
	{
public:
	/**
	Instantiate a CMMFEAacPlusDecoderDeMux class
	@leave This method may leave with one of the system-wide error codes.
	@return A pointer to the new class cast to the DeMux plugin mixin
	*/
	static MMMFDevSoundCustomInterfaceDeMuxPlugin* NewL();
		
	/**
	Attempt to open the interface.
	@param aInterfaceId
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
	@param aInterface
		   The UID of the requested interface
	@param aPackageBuf
		   A package of data that can be supplied for initialisation
	@leave This method may leave with one of the system-wide error codes.
	@return The result of the operation
	*/
	virtual TInt DoOpenSlaveL(TUid aInterface, const TDesC8& aPackageBuf);

	/** 
	Close the slave
	@param aHandle
		   The handle of the slave plugin
	@leave This method may leave with one of the system-wide error codes.
	*/
	virtual void DoCloseSlaveL(TInt aHandle);

	// original RMessage is supplied so that remote demux plugin can extract necessary details
	// using DeMux utility

	/**
	Relay the synchronous custom command onto the slave
	@param aMessage
		   The IPC message to be sent to the slave
	@leave This method may leave with one of the system-wide error codes.
	@return The result of the operation
	*/
	virtual TInt DoSendSlaveSyncCommandL(const RMmfIpcMessage& aMessage);

	/**
	Relay the synchronous custom command onto the slave and obtain a result
	@param aMessage
		   The IPC message to be sent to the slave
	@leave This method may leave with one of the system-wide error codes.
	@return The result of the operation
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
	~CMMFEAacPlusDecoderDeMux();
	CMMFEAacPlusDecoderDeMux();

	// mirrors MEAacPlusDecoderIntfc methods.
	void DoSetInputSamplingFrequency(TUint aInputSamplingFrequency);
	void DoSetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType aAudioObjectType);
	void DoSetNumOfChannels(TUint aNumOfChannels);
	void DoSetSbr(TBool aSbrEnabled);
	void DoSetDownSampledMode(TBool aDsmEnabled);
	TInt DoApplyConfig();
	TInt DoGetInputSamplingFrequency(TUint& aInputSamplingFrequency);
	TInt DoGetAudioObjectType(MEAacPlusDecoderIntfc::TAudioObjectType& aAudioObjectType);
	TInt DoGetNumOfChannels(TUint& aNumOfChannels);
	TInt DoGetSbr(TBool& aSbrEnabled);
	TInt DoGetDownSampledMode(TBool& aDsmEnabled);
	
protected:
	MMMFDevSoundCustomInterfaceDeMuxUtility* iUtility;
	MMMFDevSoundCustomInterfaceTarget* iTarget;
	TUid iDestructorKey;
	MEAacPlusDecoderIntfc* iInterfaceEAacPlusDecoder;
	};

#endif 
