// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// rscommon.h
//

#ifndef RSCOMMON_H_
#define RSCOMMON_H_
#include <e32base.h>
#include <rserror.h>
#include <startup.hrh>
#include <cfshared.h>


class CRootServer;
class CCommsProviderModule;

namespace CommsFW
    {
    class TCFMessage;
    }

namespace RootServer
    {
    class TRSStartModuleParams;
    }

struct TRootServerStartupInfo;
typedef CRootServer* (*TRootServerCtor)(const TRootServerStartupInfo&);
typedef CCommsProviderModule* (*TCommsProviderModuleCtor)(CRootServer*, const RootServer::TRSStartModuleParams&, HBufC8*);

struct TRootServerStartupInfo
	{
	TName iServerName;
	TRootServerCtor iRootServerCtor;
	TInt iPriority;
	TUid iDeathKey;
	TUid iLeakKey;
	TUid iProcessKey;
	};

IMPORT_C TInt RunRootServer(const TRootServerStartupInfo& aRootServerStartupInfo);	// Rootserver thread function

class MRootServerNotify
/** Mixin abstract interface class providing the bindmanager's up-call to the
RootServer.
 */
    {
public:
	virtual void ReceivedMessage(const CommsFW::TCFModuleName& aModule,
							     const CommsFW::TCFMessage& aNewMsg)=0;
    };

namespace RootServer
/** Namespace encapsulating the Rootserver components
@publishedPartner
@released
*/
{

#undef	_DEBUG_ROOTSERVER_FUNCTIONS
#if defined(_DEBUG)
#define	_DEBUG_ROOTSERVER_FUNCTIONS
#endif

// The RootServer API and internals extend subtly to support the requirements
// of the multithreaded socket server
#define SYMBIAN_C32ROOT_API_V3



/** Server entry ordinal.
@internalComponent
*/
const TInt KRunRootServerOrd = 1;


/** Debug logging codes
@internalComponent
*/
_LIT8(KLogSubSysRSModule, "RSModule");	// major tag
_LIT8(KLogRSLeakTag, "Leak");			// minor tag

/** Rootserver panics
@publishedPartner
@released
*/
enum TRootServerPanic
	{
	/** The server did not manage to create a new thread, possibly
	   because the thread already existed.
	 */
	KRSPanicDuplicateThreadCreation,

	/** When starting the system property signalling the existence
	   of the server already was present.
	 */
	KRSPubSubFault,

	/** The server entered a bad state.
	 */
	KRSBadState,

	/** Unable to find a reference to the module although the module is know
		to exist.
	 */
	KRSModuleRefMissing,

	/** The Bind Manager had a critical error.
	 */
	KRSBindMgrError,

	/** The active scheduler could not be created or the server left.
	 */
	KRSSchedulerError,

	/** Out of memory when trying to initialize.
	 */
	KRSNoMemory,

	/** CFlogIf interface could not be created.
	 */
	KRSCFlogIfCreationFailed
	};

/** Major version number of the Rootserver.
 @publishedPartner
 @released
  */
const TUint KRProcRootMajorVersionNumber=1;
 
 /** Minor version number of the Rootserver
 @publishedPartner
 @released
  */
const TUint KRProcRootMinorVersionNumber=1;
 
 /** Build version number of the Rootserver.
 @publishedPartner
 @released
  */
const TUint KRProcRootBuildVersionNumber=1;
 


/**
@deprecated Obsolete, always use KUidCommsProviderModule.
@internalComponent
*/
const TInt32 KUidCommsServerModule=0x101f7417;

/** DLL type of a Comms Provider Module (2nd UID).
@publishedPartner
@released
*/
const TInt32 KUidCommsProviderModule=0x101f7418;
/** The states which the Rootserver goes through while initialising.
@publishedPartner
@released
*/

/** This Key is incremented when a leak is detected on thread shutdown.
The category used in NOT the system cateory but rather KUidCommsProcess
As required by Platsec
@internalComponent
@released
*/
const TUint32 KUidCommsModuleLeakCounter = 0x102827D0;

enum TRSStartState
	{
	/** The Rootserver is initialising.
	 */
	EInitialising = 0,
	/** Core system components such as the socket server are running and configured.
	 */
	ECoreComponentsStarted = 10,
	/** All configuration has been applied
	 */
	EConfigurationComplete = 32,
	/** The Rootserver is configured and ready for normal operation.
	@deprecated
	 */
	EConfigured = 1,
	/** The Rootserver should be reconfigured by the configurator (aka warm booting)
	 */
	EReset = -1
	};

enum {
	/** Staged Start-up State values are expanded by this factor to allow fine control
	of start dependencies through .CMI files
	*/
	KC32StartupStateScalingFactor = 0x100,
	/** Start sequence values less than this ceiling are converted into SSA state values by adding
	them to KC32LowStartSequenceScaleBase.
	@see KC32LowStartSequenceScaleBase
	*/
	KC32LowStartSequenceCeiling = 5,
	/** Base for converting low start sequence values to Staged Start-up State values.
	@see KC32LowStartSequenceCeiling
	*/
	KC32LowStartSequenceScaleBase = (EStartupStateCriticalStatic * KC32StartupStateScalingFactor) + 0x40,
	/** Start sequence values greater than KC32LowStartSequenceCeiling and less than this ceiling are
	converted into SSA state values by adding them to KC32MidStartSequenceScaleBase.
	@see KC32MidStartSequenceScaleBase
	@see KC32LowStartSequenceCeiling
	*/
	KC32MidStartSequenceCeiling = 10,
	/** Base for converting mid start sequence values to Staged Start-up State values.
	@see KC32MidStartSequenceCeiling
	*/
	KC32MidStartSequenceScaleBase = (EStartupStateCriticalStatic * KC32StartupStateScalingFactor) + 0x80,
	/**  KC32HighStartSequenceScaleBase.
	@see KC32HighStartSequenceScaleBase
	@see KC32MidStartSequenceCeiling
	*/
	/** Start sequence values greater than KC32MidStartSequenceCeiling are
	converted into SSA state values by adding them to this base.
	@see KC32MidStartSequenceCeiling
	*/
	KC32HighStartSequenceScaleBase = (EStartupStateNonCritical * KC32StartupStateScalingFactor) + 0x40,
	/** Scaled start sequence signalling that core Comms components have completed loading, used by some
	servers to release blocked client connections.
	@see TRSStartState::ECoreComponentsStarted
	*/
	KC32CoreComponentsStartedScaled = (EStartupStateCriticalStatic + 1) * KC32StartupStateScalingFactor - 1,
	};

/** The states of a Comms Provider Module.
@publishedPartner
@released
*/
enum TRSModuleState
	{
	/** Not running, probably just instantiated.
	 */
	EIdle = 1,
	/** Started, waiting for rendezvous.
	 */
	EStarting = 2,
	/** Running normally.
	 */
	ERunning = 3,
	/** In the process of shutting down but thread still alive.
	 */
	EStopping = 4,
	/** Thread suddenly died, in the process of cleaning up.
	 */
	EZombie = 5,
	/** Thread dead, in the process of being cleaned up.
	 */
	EDead = 6
	};

/** RootServer IPC numbers/messages
@internalComponent
*/
enum TRSMessages
	{
	RSIsCallerConfigurator = 1,
	RSLoadModule = 2,
	RSUnloadModule = 3,
	RSBind = 4,
	RSUnbind = 5,
	RSGetModuleInfo = 6,
	RSEnumerateModules = 7,
	RSEnumerateSubModules = 8,
	RSEnumerateBindings = 9,
	RSCancelLoadModule = 10,
	RSCancelUnloadModule = 11,
	RSCancelBind = 12,
	RSCancelUnbind = 13,
	RSCloseSession = 14,
	RSShutdown = 15,
	RSSendMessage = 16,
	RSDbgMarkHeap = 17,
	RSDbgCheckHeap = 18,
	RSDbgMarkEnd = 19,
	RSDbgFailNext = 20
	};

/** Type of binding specified
@publishedPartner
@released
*/
 enum TRSBindType
	 {
	 /** Standard top-bottom binding. that doesn't allow server
	 modules to be the lower part of the binding.
	 */
	 EHierarchical,
	 /** Custom horizontal binding between Modules.
	  */
	 ECustom
	 };

/** Type of unload; send shutdown message or kill thread
@publishedPartner
@released
*/
typedef CommsFW::TCFShutdownType TRSUnLoadType;

/** Type of heap; default is process wide shared heap.
@publishedPartner
@released
*/
enum TRSHeapType
	{
	/** Process wide shared heap.
	 */
	EDefaultHeap,
	/** A heap shared with another module.
	 */
	EShareHeap,
	/** A new heap for the module.
	 */
	ENewHeap
	};

// IPC Data holder
class TRSStartModuleParamContainer
/** Container for necessary parameters to start a CPM.
@publishedPartner
@released
*/
	{
	public:
	inline TRSStartModuleParamContainer();

	public:
    /** Name of the module and its thread.
		@see TCFModuleName
	 */
	CommsFW::TCFModuleName iName;
    /** Filename of the Comms Provider Module DLL.
		@see TFileName
	 */
	TFileName iFilename;
    /** Priority of the CPM thread.
		@see TThreadPriority
	 */
	TThreadPriority iPriority;
    /** Initial size of the stack.
	 */
	TInt iStackSize;
    /** Type of the heap for the module.
		@see TRSHeapType
	 */
	TRSHeapType iHeapType;
	/** Minimum size of the heap for the module.
	 */
	TInt iMinHeapSize;
	/** Maximum size of the heap for the module.
	 */
	TInt iMaxHeapSize;
	/** If the module are to share the heap with
		an existing module this must be the name
		of the other module.
	 */
	CommsFW::TCFModuleName iShareHeapWith;
	/** Ordinal of the CPM main thread function,
		or entrypoint, in the DLL.
	 */
	TInt iThreadFunctionOrdinal;
	/** If ETrue specifies that this module can ot be unloaded.
	 */
	TBool iIsSticky;
	/** Specifies that this module is a server, maening that
		it can only be on top in a EHierarchical binding.
		@see TRSBindType
	 */
	TBool iIsServer;
	/** Additional control flags, or'd together from the the KCF_ flags. All undefined bits
		are reserved for future need and MUST be set to zero
	 */
	TUint32 iControlFlags;
	/** Control flag masks; see individual bit descriptions.
	 */
	enum TControlFlags
		{
		/** Module thread is marked as System() - a panic will halt the device and launch the
			crash debugger or reboot. The decision of action following a panic is normally left
			to the configurator and so this flag should not be commonly set; its purpose is
			to make debugging easier by halting at the source of failure.
		*/
		  KCF_ThreadAsSystem = 0x00000001
#ifdef SYMBIAN_C32ROOT_API_V3
		, KCF_UnstableModule = 0x00000002,
		/** If we shut down a module with EImmediate og EUngraceful, this flag will cause
		the rootserver to not check for memory leaks in the heap belonging to the module.
		*/
		  KCF_ModuleImmediateNoCheck = 0x00000004
#endif
		};

	};

 class TRSStartModuleParams : public TPckg<TRSStartModuleParamContainer>
/** Descriptor encapsulating the TRSModParamContainer for
	IPC transfers.
@publishedPartner
@released
*/
    {
	public:
	TRSStartModuleParams():TPckg<TRSStartModuleParamContainer>(iParams) {};
	inline TRSStartModuleParams(const CommsFW::TCFModuleName& aName, const TFileName& aFileName,
								const TThreadPriority aPriority, TInt aStackSize, const TRSHeapType aHeapType,
								TInt aMinHeapSize, TInt aMaxHeapSize, const CommsFW::TCFModuleName& aShareHeapWith,
								TInt aThreadFunctionOrdinal=0, TBool aIsSticky=EFalse, TUint32 aControlFlags=0);

	inline TRSStartModuleParams(const CommsFW::TCFModuleName& aName, const TFileName& aFileName,
								const TThreadPriority aPriority, TInt aStackSize, const TRSHeapType aHeapType,
								TInt aMinHeapSize, TInt aMaxHeapSize, TInt aThreadFunctionOrdinal=0,
								TBool aIsSticky=EFalse, TUint32 aControlFlags=0);
	inline TRSStartModuleParams(const CommsFW::TCFModuleName& aName, const TFileName& aFileName,
								const TThreadPriority aPriority, TInt aStackSize, const TRSHeapType aHeapType,
								TInt aThreadFunctionOrdinal=0, TBool aIsSticky=EFalse, TUint32 aControlFlags=0);
    private:
	TRSStartModuleParams(const TRSStartModuleParams &aRef); //Never Implemented
	public:
	/** Structure containing the parameters for the request.
	 */
	TRSStartModuleParamContainer iParams;
	};

 class TRSModuleInfoContainer
/** Container for receiving information of a running CPM.
@publishedPartner
@released
*/
	{
	public:
	/** Name of the module.
	 */
	CommsFW::TCFModuleName iName;
    /** Filename of the CPM DLL.
	 */
	TFileName iDll;
	/** State of the CPM.
	 */
	TRSModuleState iState;
	/** Number of submodules this module has reported.
	 */
	TInt iNumSubModules;
	};

class TRSModuleInfo : public TPckg<TRSModuleInfoContainer>
/** Descriptor encapsulating the TRSModuleInfoContainer for
IPC transfers.
@publishedPartner
@released
*/
	{
	public:
	TRSModuleInfo():TPckg<TRSModuleInfoContainer>(iParams) {};
	inline TRSModuleInfo(const CommsFW::TCFModuleName aName, const TFileName& aDll,
						 const TRSModuleState aState, const TInt aNumSubModules);
    private:
	TRSModuleInfo(const TRSModuleInfo &aRef); //Never implemented
	public:
	/** Structure containing the parameters for the request.
	 */
	TRSModuleInfoContainer iParams;
	};

class TRSSubModuleAddress : public TPckg<CommsFW::TCFSubModuleAddress>
/** Descriptor encapsulating a TCFSubModuleAddress for
IPC transfers.
@publishedPartner
@released
*/
	{
	public:
	TRSSubModuleAddress():TPckg<CommsFW::TCFSubModuleAddress>(iAddress) {};
		inline TRSSubModuleAddress(const CommsFW::TCFSubModuleAddress& aAddress);
		inline TRSSubModuleAddress(const CommsFW::TCFModuleName& aModule, const CommsFW::TCFSubModuleName& aSubModule);

    private:
	TRSSubModuleAddress(const TRSSubModuleAddress &aRef); //Never implemented
	public:
	/** Structure containing the parameters for the request.
	 */
	CommsFW::TCFSubModuleAddress iAddress;
	};

class TRSBindingInfoContainer
/** Container for receiving binding information.
@publishedPartner
@released
*/
	{
	public:
	inline TRSBindingInfoContainer& operator=(const TRSBindingInfoContainer& aInfo);
	public:
	/** Type of binding.
		@see TRSBindType
	*/
	TRSBindType iType;
	/** Fully qualified address of the first submodule.
		@see TCFSubModuleAddress
	*/
	CommsFW::TCFSubModuleAddress iAddress1;
	/** Fully qualified address of the second submodule.
		@see TCFSubModuleAddress
	*/
	CommsFW::TCFSubModuleAddress iAddress2;
	/** Status of the binding in reference to the first submodule.
	 */
	TInt iState1;
	/** Status of the binding in reference to the seond submodule.
	 */
	TInt iState2;
	/** The number of channels on the message queue going forward.
	 */
	TInt iForwardQLength;
	/** The number of channels on the message queue going in the reverse direction.
	 */
	TInt iReverseQLength;
	};

class TRSBindingInfo : public TPckg<TRSBindingInfoContainer>
/** Descriptor encapsulating a TRSBindingInfoContainer for
IPC transfers.
@publishedPartner
@released
*/
	{
	public:
	enum
		{
		EMaxQueueLength = KMaxTInt/2,
		EMinQueueLength = 1
		};

	public:
	TRSBindingInfo():TPckg<TRSBindingInfoContainer>(iParams) {};
	inline TRSBindingInfo(const TRSBindType& aType, const CommsFW::TCFSubModuleAddress& aAddress1,
						  const CommsFW::TCFSubModuleAddress& aAddress2,
						  const TInt aForwardQLength = 1, const TInt aReverseQLength = 1);

	inline TRSBindingInfo& operator=(const TRSBindingInfo& aInfo);
	inline TRSBindingInfo(const TRSBindingInfo &aRef);

	public:
	/** Structure containing the parameters for the request.
	 */
	TRSBindingInfoContainer iParams;
	};

class TRSUnBindingInfoContainer
/** Parameter container for unbinding request
@publishedPartner.
@released
*/
	{
	public:
	/** Fully qualified address of the first submodule.
		@see TCFSubModuleAddress
	*/
	CommsFW::TCFSubModuleAddress iAddress1;
	/** Fully qualified address of the second submodule.
		@see TCFSubModuleAddress
	*/
	CommsFW::TCFSubModuleAddress iAddress2;
	/** Status of the binding in reference to the first submodule.
	 */
	TInt iState1;
	/** Status of the binding in reference to the second submodule.
	 */
	TInt iState2;
	};

class TRSUnBindingInfo : public TPckg<TRSUnBindingInfoContainer>
/** Descriptor encapsulating a TRSUnBindingInfoContainer for
IPC transfers.
@publishedPartner
@released
*/
	{
	public:
	TRSUnBindingInfo():TPckg<TRSUnBindingInfoContainer>(iParams) {};
	inline TRSUnBindingInfo(const CommsFW::TCFSubModuleAddress aAddress1, const CommsFW::TCFSubModuleAddress aAddress2);

    private:
	TRSUnBindingInfo(const TRSUnBindingInfo &aRef); //Never Implemented
	public:
	/** Structure containing the parameters for the request.
	 */
	TRSUnBindingInfoContainer iParams;
	};

class TRSIter
/** Encapsulates the iterator position in a Root Server enumeration
@publishedPartner.
@released
*/
	{
	public:
	inline TRSIter();
	inline const TInt& operator()() const;
	inline TInt& operator()();
	inline void Reset();
	private:
	TInt iPosition;
	};


#include <rscommon.inl>

} // namespace RootServer
#endif /*RSCOMMON_H_*/

