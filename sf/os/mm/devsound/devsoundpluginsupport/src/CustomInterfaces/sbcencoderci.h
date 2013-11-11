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

#ifndef SBCENCODERCI_H
#define SBCENCODERCI_H

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
enum TMMFDevSoundCISbcEncoderCommands
	{
	EMMFDevSoundCISbcEncoderGetSupportedSamplingFrequenciesCount = 0,
	EMMFDevSoundCISbcEncoderGetSupportedSamplingFrequenciesArray,
	EMMFDevSoundCISbcEncoderGetSupportedChannelModesCount,
	EMMFDevSoundCISbcEncoderGetSupportedChannelModesArray,
	EMMFDevSoundCISbcEncoderGetSupportedSubbandsCount,
	EMMFDevSoundCISbcEncoderGetSupportedSubbandsArray,
	EMMFDevSoundCISbcEncoderGetSupportedAllocationMethodsCount,
	EMMFDevSoundCISbcEncoderGetSupportedAllocationMethodsArray,
	EMMFDevSoundCISbcEncoderGetSupportedBlocksCount,
	EMMFDevSoundCISbcEncoderGetSupportedBlocksArray,
	EMMFDevSoundCISbcEncoderGetSupportedBitpoolRange,
	EMMFDevSoundCISbcEncoderSetSamplingFrequency,
	EMMFDevSoundCISbcEncoderSetChannelMode,
	EMMFDevSoundCISbcEncoderSetSubbands,
	EMMFDevSoundCISbcEncoderSetBlocks,
	EMMFDevSoundCISbcEncoderSetAllocationMethod,
	EMMFDevSoundCISbcEncoderSetBitpoolSize,
	EMMFDevSoundCISbcEncoderApplyConfig,
	EMMFDevSoundCISbcEncoderGetSamplingFrequency,	
	EMMFDevSoundCISbcEncoderGetChannelMode,
	EMMFDevSoundCISbcEncoderGetSubbands,
	EMMFDevSoundCISbcEncoderGetBlocks,
	EMMFDevSoundCISbcEncoderGetAllocationMethod,
	EMMFDevSoundCISbcEncoderGetBitpoolSize
	};


/**
 CMMFSbcEncoderMux implementation of the SBC encoder custom interface pair
 @internalComponent
 @prototype
 @file
 */
class CMMFSbcEncoderMux : public CBase,
						  public MMMFDevSoundCustomInterfaceMuxPlugin,
						  public MSbcEncoderIntfc
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

	// from MSbcEncoderIntfc
	/** 
	@see MSbcEncoderIntfc
	*/
	virtual TInt GetSupportedSamplingFrequencies(RArray<TUint>& aSamplingFrequencies);

	/** 
	@see MSbcEncoderIntfc
	*/
	virtual TInt GetSupportedChannelModes(RArray<TSbcChannelMode>& aChannelModes);

	/** 
	@see MSbcEncoderIntfc
	*/
	virtual TInt GetSupportedNumOfSubbands(RArray<TUint>& aNumOfSubbands); 

	/** 
	@see MSbcEncoderIntfc
	*/
	virtual TInt GetSupportedAllocationMethods(RArray<TSbcAllocationMethod>& aAllocationMethods);

	/** 
	@see MSbcEncoderIntfc
	*/
	virtual TInt GetSupportedNumOfBlocks(RArray<TUint>& aNumOfBlocks);

	/** 
	@see MSbcEncoderIntfc
	*/
	virtual TInt GetSupportedBitpoolRange(TUint& aMinSupportedBitpoolSize, TUint& aMaxSupportedBitpoolSize);

	/** 
	@see MSbcEncoderIntfc
	*/
	virtual void SetSamplingFrequency(TUint aSamplingFrequency);

	/** 
	@see MSbcEncoderIntfc
	*/	
	virtual void SetChannelMode(TSbcChannelMode aChannelMode);

	/** 
	@see MSbcEncoderIntfc
	*/	
	virtual void SetNumOfSubbands(TUint aNumOfSubbands);

	/** 
	@see MSbcEncoderIntfc
	*/	
	virtual void SetNumOfBlocks(TUint aNumOfBlocks);

	/** 
	@see MSbcEncoderIntfc
	*/	
	virtual void SetAllocationMethod(TSbcAllocationMethod aAllocationMethod);

	/** 
	@see MSbcEncoderIntfc
	*/	
	virtual void SetBitpoolSize(TUint aBitpoolSize);

	/** 
	@see MSbcEncoderIntfc
	*/	
	virtual TInt ApplyConfig();

	/**
	@see MSbcEncoderIntfc
	*/	
	virtual TInt GetSamplingFrequency(TUint& aSamplingFrequency);

	/** 
	@see MSbcEncoderIntfc
	*/	
	virtual TInt GetChannelMode(TSbcChannelMode& aChannelMode);

	/** 
	@see MSbcEncoderIntfc
	*/	
	virtual TInt GetNumOfSubbands(TUint& aNumOfSubbands);

	/** 
	@see MSbcEncoderIntfc
	*/	
	virtual TInt GetNumOfBlocks(TUint& aNumOfBlocks);

	/** 
	@see MSbcEncoderIntfc
	*/
	virtual TInt GetAllocationMethod(TSbcAllocationMethod& aAllocationMethod);

	/** 
	@see MSbcEncoderIntfc
	*/	
	virtual TInt GetBitpoolSize(TUint& aBitpoolSize);

protected:
	CMMFSbcEncoderMux();
	~CMMFSbcEncoderMux();

private:
	void DoGetTUintArrayL(RArray<TUint>& aArray, TInt aCount, TMMFDevSoundCISbcEncoderCommands aCommand);
	void DoGetChannelModesArrayL(RArray<TSbcChannelMode>& aChannelModes, TInt aCount);
	void DoGetAllocMethodsArrayL(RArray<TSbcAllocationMethod>& aAllocationMethods, TInt aCount);
	void DoWriteArrayToClientL(const RMmfIpcMessage& aMessage);
	
protected:
	TUid iDestructorKey;
	TInt iRemoteHandle;
	MMMFDevSoundCustomInterfaceMuxUtility* iUtility;
	};

/**
 CMMFSbcEncoderDeMux - DeMux side of pair
 @internalComponent
 @prototype
 @file
 */
class CMMFSbcEncoderDeMux : public CBase,
							public MMMFDevSoundCustomInterfaceDeMuxPlugin
	{
public:
	/**
	Instantiate a CMMFSbcEncoderDeMux class
	@leave This method may leave with one of the system-wide error codes.
	@return A pointer to the new class cast to the DeMux plugin mixin
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
	~CMMFSbcEncoderDeMux();
	CMMFSbcEncoderDeMux();

	// mirrors MSbcEncoderIntfc methods.
	TInt DoGetSupportedSamplingFrequencies (RArray<TUint>& aSamplingFrequencies );
	TInt DoGetSupportedChannelModes (RArray<MSbcEncoderIntfc::TSbcChannelMode>& aChannelModes );
	TInt DoGetSupportedNumOfSubbands (RArray<TUint>& aNumOfSubbands ); 
	TInt DoGetSupportedAllocationMethods (RArray<MSbcEncoderIntfc::TSbcAllocationMethod>& aAllocationMethods );
	TInt DoGetSupportedNumOfBlocks (RArray<TUint>& aNumOfBlocks );
	TInt DoGetSupportedBitpoolRange (TUint& aMinSupportedBitpoolSize, TUint& aMaxSupportedBitpoolSize);
	void DoSetSamplingFrequency (TUint aSamplingFrequency );	
	void DoSetChannelMode (MSbcEncoderIntfc::TSbcChannelMode aChannelMode );
	void DoSetNumOfSubbands (TUint aNumOfSubbands );
	void DoSetNumOfBlocks (TUint aNumOfBlocks );
	void DoSetAllocationMethod (MSbcEncoderIntfc::TSbcAllocationMethod aAllocationMethod );
	void DoSetBitpoolSize (TUint aBitpoolSize );
	TInt DoApplyConfig();
	TInt DoGetSamplingFrequency (TUint& aSamplingFrequency );
	TInt DoGetChannelMode (MSbcEncoderIntfc::TSbcChannelMode& aChannelMode );
	TInt DoGetNumOfSubbands (TUint& aNumOfSubbands );
	TInt DoGetNumOfBlocks (TUint& aNumOfBlocks );
	TInt DoGetAllocationMethod (MSbcEncoderIntfc::TSbcAllocationMethod& aAllocationMethod );
	TInt DoGetBitpoolSize (TUint& aBitpoolSize );
	
private:
	void DoWriteArrayToClientL(const RMmfIpcMessage& aMessage, RArray<TUint>& aArray);
	void DoWriteArrayToClientL(const RMmfIpcMessage& aMessage, TBool aWriteChannelModeArray);
	
protected:
	MMMFDevSoundCustomInterfaceDeMuxUtility* iUtility;
	MMMFDevSoundCustomInterfaceTarget* iTarget;
	TUid iDestructorKey;
	MSbcEncoderIntfc* iInterfaceSbcEncoder;
	
private:
	RArray<TUint> iSamplingFrequencies;
	RArray<TUint> iNumOfSubbands;
	RArray<TUint> iNumOfBlocks;
	RArray<MSbcEncoderIntfc::TSbcAllocationMethod> iAllocationMethods;
	RArray<MSbcEncoderIntfc::TSbcChannelMode> iChannelModes;
	};

#endif // SBCENCODERCI_H
