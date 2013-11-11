// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @publishedPartner
 @released
*/

#include <c32root.h>
#include <comms-infras/commsdebugutility.h>
#include <e32property.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_RootSrvc32rootc, "RootSrvc32root.c");
#endif

__FLOG_STMT(_LIT8(KSubsys,"RootServer");) // subsystem name
__FLOG_STMT(_LIT8(KComponent,"event");) // component name
static const TInt KMaxKillRetry = 10;


EXPORT_C TInt RRootServ::Connect()
    /** Use this function to open a session to the Root Server.
    Default number of message slots is defined in KDefaultMessageSlots.
    As with any server, the connection should be closed after use.
    RHandleBase provides the necessary Close() function, which should be
    called when the server session is no longer required.
    @return An error code
    @publishedPartner
    @released
    */
    {

	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KSubsys, KComponent);
    __FLOG_1(_L("RRootServ::Connect: client %d"), (TUint) RThread().Id());

    TInt r = CreateSession(KRootServerName, Version());

	__FLOG_CLOSE;
    return r;
    }

EXPORT_C TVersion RRootServ::Version() const
    /** Returns the client side version number.
    Use this function to get the version number. The version number may be
    incremented in future releases of the Root Server. If extra features are
    added in such releases, the version number may be used by application programs
    as a basis for assessing the capabilities of the Root Server.
    @return version number
    @publishedPartner
    @released
    */
    {
    return(TVersion(KRS32MajorVersionNumber,KRS32MinorVersionNumber,KRS32BuildVersionNumber));
    }
    
EXPORT_C TBool RRootServ::IsCallerConfigurationAuthority() const
	/** Tests whether the calling process is deemed to be the RootServer Configurator. While
	multiple instances of the configurator process may connect only the first (at one time) can
	take the role of being the authority for configuration; others might watch but should not
	take action.
    @return True if the calling process is deemed to be the RootServer Configurator. 
    @internalTechnology
    @released
	*/
	{
    return SendReceive(RSIsCallerConfigurator) > 0;
	}    

EXPORT_C TInt RRootServ::SetMBufPoolSize(TUint aPoolSize)
    /** Set the size of the MBuf pool.
    This function can only be called before the first server module is loaded.
    @param aPoolSize Defines the new size of the Comms buffer pool in number of bytes.
    @return An error code.
    @publishedPartner
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that
    this request came from the comms configurator
    */
    {
    TRSSetMBufMnr paramPckg;
    TRSSetMBufMnrContainer &container = paramPckg();
    container.iInitSize = aPoolSize;
    container.iMaxSize = aPoolSize;

    return SendReceive(RSSetMBufPoolSize, TIpcArgs(&paramPckg));
    }


EXPORT_C TInt RRootServ::SetMBufPoolSize(TUint aInitPoolSize, TUint aMaxPoolSize)
 /** Set the size of the MBuf pool.
    This function can only be called before the first server module is loaded.
    @param aInitPoolSize Defines the initial size of the new Comms Buffer Pool
    @param aInitPoolSize Defines the maximum size of the new Comms Buffer Pool can grow to
    @return An error code.
    @publishedPartner
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that
    this request came from the comms configurator
    */
    {
    TRSSetMBufMnr paramPckg;
    TRSSetMBufMnrContainer &container = paramPckg();
    container.iInitSize = aInitPoolSize;
    container.iMaxSize = aMaxPoolSize;

    return SendReceive(RSSetMBufPoolSize, TIpcArgs(&paramPckg));
    }

EXPORT_C TInt RRootServ::AddMBufAllocInfo(const TRSAddMBufAllocInfoContainer& aMBufSizeAllocInfo)
/** Adds information to the MBufManager about how it should allocate MBufs of a given.
    @param aMBufSizeAllocInfo defines the mbuf size alloc info
    @return An error code.
    @internalTechnology
    @capability NetworkControl The Security ID of the caller will be checked to ensure that
    this request came from the comms configurator
    */
 	{
    TRSAddMBufAllocInfo paramPckg;
    TRSAddMBufAllocInfoContainer &container = paramPckg();
    container  = aMBufSizeAllocInfo;

    return SendReceive(RSAddMBufAllocInfo, TIpcArgs(&paramPckg));
	}

EXPORT_C void RRootServ::LoadCpm(TRequestStatus& aStatus,
                                 const TRSStartModuleParams& aParams, const TDesC8& aIniData)
    /** Launch a new Comms Provider Module.
    The name of the new CPM instance (aParams.iParams.iName) must be unique and must
    be used in any further communication concerning this CPM instance, such as binding.
    The contents of the initialisation data parameter (aIniData) are specific to the module.
    It could be empty, contain the contents of an associated INI file or contain some other
    data specified by the specific CPM implementation.
    Attempts to load a CPM with a name equal to that of an existing CPM is not possible.
    It is required that the DLL contains the CPM secondary UID of 0x101f7418.
    @param aStatus The request status used to contain completion information for the function.
    @param aParams Parameters needed to load the Provider Module.
    @param aIniData Module specific ini data.
    @see TRSStartModuleParams
    @publishedPartner
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that
    this request came from the comms configurator
    */
    {
    SendReceive(RSLoadModule, TIpcArgs(&aParams, &aIniData), aStatus);
    }

EXPORT_C void RRootServ::UnloadCpm(TRequestStatus& aStatus, const TCFModuleName& aName, TRSUnLoadType aType)
    /** Unloads a Comms Provider Module.
    @param aStatus The request status used to contain completion information for the function.
    @param aName Name of module to unload
	@param aType Type of unload (optional/graceful/ungraceful).
	@see TCFModuleName
	@see TRSUnLoadType
	@publishedPartner
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that
    this request came from the comms configurator
    */
    {
    SendReceive(RSUnloadModule, TIpcArgs(&aName, aType), aStatus);
    }

EXPORT_C void RRootServ::SendMessage(TRequestStatus& aStatus, const TCFModuleName& aName, TInt aType, TDes8& aData)
	/**
	Sends a message to the module
	@param aStatus The request status used to contain completion information for the function.
	@param aType a type determining the type of data in aData param
	@param aData a data specific to the message and it's type given by aType. All aData is expected to be
			CNetMessage based serialised instance
	@see TCFMessageType for message/data types supported by ESOCK directly
	@see CNetMessage
	*/
	{
   SendReceive(RSSendMessage, TIpcArgs(&aName, aType, &aData), aStatus);
	}

EXPORT_C TInt RRootServ::SendMessage(const TCFModuleName& aName, TInt aType, TDes8& aData)
	/**
	Sends a message to the module
	@param aStatus The request status used to contain completion information for the function.
	@param aType a type determining the type of data in aData param
	@param aData a data specific to the message and it's type given by aType. All aData is expected to be
			CNetMessage based serialised instance
	@see TCFMessageType for message/data types supported by ESOCK directly
	@see CNetMessage
	*/
	{
   return SendReceive(RSSendMessage, TIpcArgs(&aName, aType, &aData));
	}

EXPORT_C void RRootServ::Bind(TRequestStatus& aStatus, TRSBindingInfo& aBindInfo)
    /** Binds two modules.
    Several types of binding are available: EHierarchical is the standard top-bottom
    binding in the protocol hierarchy. If this type is specified, iName1 of TRSBindingInfo is considered top-level
    module of the binding and iName2 is considered the bottom-level part. Also, this type of binding
    allows the top-level module to be a server. ECustom is a "Horizontal" binding between two CPM's.
    Typical use is when a CPM employs another CPM for doing some processing and communicates via the
    standard message queues.
    @param aStatus The request status used to contain completion information for the function.
    @param aBindInfo The information regarding the binding request.
    @see TRSBindingInfo
    @publishedPartner
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that
    this request came from the comms configurator */
    {
    SendReceive(RSBind, TIpcArgs(&aBindInfo), aStatus);
	}

EXPORT_C void RRootServ::Unbind(TRequestStatus& aStatus, TRSUnBindingInfo& aUnBindInfo)
    /** Unbinds two modules.
    All bindings between the two modules are unbound and the states for each module
    are updated in the object.
    @param aStatus The request status used to contain completion information for the function.
    @param aUnBindInfo Holds the unbinding info.
    @see TRSUnBindingInfo
    @publishedPartner
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that
    this request came from the comms configurator
    */
    {
    SendReceive(RSUnbind, TIpcArgs(&aUnBindInfo), aStatus);
    }

EXPORT_C TInt RRootServ::GetModuleInfo(const TCFModuleName& aName, TRSModuleInfo& aModuleInfo)
    /** Gets information about one module.
    @param aName Name of module to query.
    @param aModuleInfo Information about the module state.
    @see TCFModuleName
    @see TRSModuleInfo
    @return An error code.
    @publishedPartner
    @released
    */
    {
    return SendReceive(RSGetModuleInfo, TIpcArgs(&aName, &aModuleInfo));
    }

EXPORT_C TInt RRootServ::EnumerateModules(TRSIter &aPosition, TCFModuleName& aModuleName)
    /** Through repeated calls returns the list of names of running Comms Provider Modules.
    Notes:
	- The current position in an enumeration is maintained for each client session, meaning that
	  to perform two consecutive enumerations two RRootServ's must be used.
	- The order in which the modules are returned is undefined.
	- The enumeration returned is not a snapshot from the start, i.e. if modules are loaded or
	  unloaded during the enumeration then a module could be skipped or returned twice.
    @param aPosition Position in enumeration.
    @param aName Contains module upon successful return.
    @return An error code: KErrNone upon success, KErrEOF upon reaching the end of the enumeration.
    @see TRSIter
    @see TCFModuleName
    @publishedPartner
    @released
    */
    {
	TPckg<TRSIter> pkgPos(aPosition);
    return SendReceive(RSEnumerateModules, TIpcArgs(&pkgPos, &aModuleName));
    }

EXPORT_C TInt RRootServ::EnumerateSubModules(const TCFModuleName& aModuleName, TRSIter &aPosition, TCFSubModuleName& aSubModuleName)
    /** Returns a list of running Comms Provider Modules.
    The client can enumerate sub-modules one at a time by providing a TCFSubModuleName.
    The enumerator aPosition is a correlator for Rootserver use.
    @param aModuleName Name of module.
    @param aPosition Position in enumeration, correlator for the Rootserver.
    @param aSubModuleName A name to be returned
    @return An error code.
    @see TCFModuleName
    @see TRSIter
    @see TCFSubModuleName
    @publishedPartner
    @released
    */
    {
	TPckg<TRSIter> pkgPos(aPosition);
    return SendReceive(RSEnumerateSubModules, TIpcArgs(&aModuleName, &pkgPos, &aSubModuleName));
    }

EXPORT_C TInt RRootServ::EnumerateBindings(const TCFSubModuleAddress& aSubModuleAddr, TRSIter &aPosition, TRSBindingInfo& aBinding)
    /** Gets information about the bindings on one module.
	The client can enumerate bindings one at a time, with
    the first message specifying the iReset flag (in the TRSEnumerateBindInfo struct) true, and
    subsequent requests for all bind infos specify this as false, in which case the Root Server will
    write from where it left off from the last request.
    @param aSubModuleAddr The address of the SubModule in question
    @param aPosition Position in enumeration. Correlator for the Rootserver.
    @param aBinding On succesful return contains binding information.
    @return An error code.
    @see TCFSubModuleAddress
    @see TRSIter
    @see TRSBindingInfo
    @publishedPartner
    @released
    */
    {
	TPckg<TRSIter> pkgPos(aPosition);
	TRSSubModuleAddress pckgSubModuleAddr(aSubModuleAddr);
    return SendReceive(RSEnumerateBindings, TIpcArgs(&pckgSubModuleAddr, &pkgPos, &aBinding));
    }

EXPORT_C void RRootServ::CancelLoadCpm(const TCFModuleName& aName)
    /** Cancels asynchronous loading of a comms Provider Module.
    There is no guarantee the module loading is canceled.
    A best effort is made and the asynchronous request waiting for
    the module to load returns with KErrCanceled on success.
    @param aName Name of module which is loading.
    @see TCFModuleName
    @publishedPartner
    @released
    */
    {
    (void)SendReceive(RSCancelLoadModule, TIpcArgs(&aName));
    }

EXPORT_C void RRootServ::CancelUnloadCpm(const TCFModuleName& aName)
    /** Cancel asynchronous unloading of a comms Provider Module.
    There is no guarantee the module unloading is canceled.
    A best effort is made and the asynchronous request waiting for
    the module to load returns KErrCanceled on success.
    @param aName Name of module which is unloading.
    @see TCFModuleName
    @publishedPartner
    @released
    */
    {
    (void)SendReceive(RSCancelUnloadModule, TIpcArgs(&aName));
    }

EXPORT_C void RRootServ::CancelBind(TRSSubModuleAddress& aName1, TRSSubModuleAddress& aName2)
    /** Cancels asynchronous binding of two modules.
    There is no guarantee the binding is canceled.
    A best effort is made and the asynchronous request waiting for
    the module to load returns KErrCanceled on success.
    @param aName1 Name of module which is binding to aName2.
    @param aName2 Name of module which is binding to aName1.
    @see TRSSubModuleAddress
    @publishedPartner
    @released
    */
    {
    (void)SendReceive(RSCancelBind, TIpcArgs(&aName1, &aName2));
    }

EXPORT_C void RRootServ::CancelUnbind(TRSSubModuleAddress& aName1, TRSSubModuleAddress& aName2)
    /** Cancels asynchronous unbinding of two modules.
    There is no guarantee the unbinding is canceled.
    A best effort is made and the asynchronous request waiting for
    the module to load returns KErrCanceled on success.
    @param aName1 Name of module which is unbinding from aName2.
    @param aName2 Name of module which is unbinding from aName1.
    @see TRSSubModuleAddress
    @publishedPartner
    @released
    */
    {
    (void)SendReceive(RSCancelUnbind, TIpcArgs(&aName1, &aName2));
    }

EXPORT_C TInt RRootServ::Shutdown()
	/** Shuts down RootServer
	@publishedPartner
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that
    this request came from the comms configurator
    */
	{
	return SendReceive(RSShutdown);
	}

EXPORT_C void RRootServ::Close()
	/** Tells server that this session is being closed
 	@publishedPartner
    @released
     */
    {
    (void)SendReceive(RSCloseSession);
	RHandleBase::Close();
    }

#if defined (_DEBUG_ROOTSERVER_FUNCTIONS)
EXPORT_C TInt RRootServ::__DbgMarkHeap()
	/** Sets a heap mark in the root server
	@internalComponent
    */
	{
	return SendReceive(RSDbgMarkHeap,TIpcArgs());
	}

EXPORT_C TInt RRootServ::__DbgCheckHeap(TInt aCount)
	/** Sets a heap mark in the root server
	@internalComponent
    */
	{
	return SendReceive(RSDbgCheckHeap,TIpcArgs(aCount));//check if it's right
	}

EXPORT_C TInt RRootServ::__DbgMarkEnd(TInt aCount)
	/** Sets a heap mark in the root server
	@internalComponent
    */
	{
	return SendReceive(RSDbgMarkEnd,TIpcArgs(aCount));//check if it's right
	}
EXPORT_C TInt RRootServ::__DbgFailNext(TInt aCount)
	/** Sets a heap mark in the root server
	@internalComponent
    */
	{
	return SendReceive(RSDbgFailNext,TIpcArgs(aCount));//check if it's right
	}

EXPORT_C TInt RRootServ::__DbgFailNextMbuf(TInt aCount)
	/** Sets a Mbuf mark in the root server
	@internalComponent
    */
	{
	return SendReceive(RSDbgFailNextMbuf,TIpcArgs(aCount));//check if it's right
	}

EXPORT_C TInt RRootServ::__DbgSetMbufPoolLimit(TInt aSize)
	/** Sets the Mbuf pool limit
	@internalComponent
    */
	{
	return SendReceive(RSDbgSetMbufPoolLimit,TIpcArgs(aSize));//check if it's right
	}

EXPORT_C TInt RRootServ::__DbgCheckMbuf(TInt aSize)
	/** Sets the Mbuf pool limit
	@internalComponent
    */
	{
	return SendReceive(RSDbgCheckMbuf,TIpcArgs(aSize));//check if it's right
	}

EXPORT_C TInt RRootServ::__DbgMbufFreeSpace()
	/** Gets the amount of free space in the MBuf manager
	@internalComponent
    */
	{
	TPckgBuf<TInt> i;
	TInt result=SendReceive(RSDbgMbufFreeSpace,TIpcArgs(&i));
	if (KErrNone == result)
		{
		return i();
		}
	else
		{
		return result;
		}
	}

EXPORT_C TInt RRootServ::__DbgMbufTotalSpace()
	/** Gets the amount of free space in the MBuf manager
	@internalComponent
    */
	{
	TPckgBuf<TInt> i;
	TInt result = SendReceive(RSDbgMbufTotalSpace,TIpcArgs(&i));
	if (KErrNone == result)
		{
		return i();
		}
	else
		{
		return result;
		}
	}
#else
EXPORT_C TInt RRootServ::__DbgMarkHeap()
	/** Sets a heap mark in the root server
	@internalComponent
    */
	{
	return KErrNone;
	}

EXPORT_C TInt RRootServ::__DbgCheckHeap(TInt /*aCount*/)
	/** Sets a heap mark in the root server
	@internalComponent
    */
	{
   return KErrNone;
	}

EXPORT_C TInt RRootServ::__DbgMarkEnd(TInt /*aCount*/)
	/** Sets a heap mark in the root server
	@internalComponent
    */
	{
   return KErrNone;
	}
EXPORT_C TInt RRootServ::__DbgFailNext(TInt /*aCount*/)
	/** Sets a heap mark in the root server
	@internalComponent
    */
	{
   return KErrNone;
	}

EXPORT_C TInt RRootServ::__DbgFailNextMbuf(TInt /*aCount*/)
	/** Sets a Mbuf mark in the root server
	@internalComponent
    */
   {
   return KErrNone;
	}

EXPORT_C TInt RRootServ::__DbgSetMbufPoolLimit(TInt /*aSize*/)
	/** Sets the Mbuf pool limit
	@internalComponent
    */
   {
   return KErrNone;
	}

EXPORT_C TInt RRootServ::__DbgCheckMbuf(TInt /*aSize*/)
	/** Sets the Mbuf pool limit
	@internalComponent
    */
   {
   return KErrNone;
	}

EXPORT_C TInt RRootServ::__DbgMbufFreeSpace()
	/** Gets the amount of free space in the MBuf manager
	@internalComponent
    */
   {
   return KErrNone;
	}

EXPORT_C TInt RRootServ::__DbgMbufTotalSpace()
	/** Gets the amount of free space in the MBuf manager
	@internalComponent
    */
   {
   return KErrNone;
	}
#endif // _DEBUG


static TInt DoStartC32(const TDesC& aCMISuppressionList, const TRSStartState aFinishedState)
    /** Starts the Root Server and the c32start.exe example configuration program.
    Any caller of this function is blocked until the Root Server is
    fully loaded and configured by the example configuration utility: c32start.exe.
    This is because any call expects that it can instantly use networking
    services offered by the modules loaded during configuration.
    return An error code.
    @publishedPartner
    @released
    */
    {

	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KSubsys, KComponent);
	__FLOG_1(_L("DoStartC32(\"%S\")"), &aCMISuppressionList);

    TInt propertyResult = KErrNone;
    
	// Four possibilities exist at this point:
	//	(1) C32Start has not yet been created
	//	(2) C32Start has been created but has yet to define the start property key
	//	(3) C32Start has defined the key but its state hasn't yet reached "core components started"
	//	(4) Boot is past "core components started" and we can return control to our caller    
	RProperty configurationProperty;
	TInt propertyValue = EInitialising;	// set to safe state
	TInt result = configurationProperty.Attach(KUidSystemCategory, KUidC32StartPropertyKey.iUid);	
	if(result == KErrNone)
		{
		result = configurationProperty.Get(propertyValue);
		}

	if(result!=KErrNone || propertyValue==EReset)
		{
		RProcess C32Start;
		result = C32Start.Create(KCommC32StartExe, aCMISuppressionList);

		if (result == KErrNone)
			{
			__FLOG(_L("StartC32 - Starting C32Start.exe"));
			TRequestStatus status;
			C32Start.Rendezvous(status);
			C32Start.Resume();
			__FLOG(_L("StartC32 - waiting for C32Start.exe to complete init."));

			User::WaitForRequest(status);
			// we are not worried about the rendezvous status, as we only wait on it
			// so that we know there is a property we can subscribe to below.  Basically
			// we should either get KErrAlreadyExists, or KErrNone, but either way we
			// have a property to subscribe to, so everything is ok.

			// Now that the configurator has read the suppression list we delete to avoid persistent settings between
			// different startups
			}
		else if (result == KErrAlreadyExists)
			{
			result = KErrNone;
			}
 		// In case of missing c32start.exe the error code is returned to avoid silent deadlock
 		else if (result != KErrNone) 
 			{
 			C32Start.Close();
 			return result;
			}

		// for the case where C32 might already be in the process of starting up
		// we use a double-check-type pattern to ensure the property has been
		// published before we try and use it

		propertyResult = configurationProperty.Get(propertyValue);

		if(propertyResult != KErrNone)
			{
			TRequestStatus status;
			C32Start.Rendezvous(status);
			propertyResult = configurationProperty.Get(propertyValue);

			if(propertyResult == KErrNone)
				{
				C32Start.RendezvousCancel(status);
				}
			User::WaitForRequest(status);
			}
		C32Start.Close();
		}

	// we have a property and have a value now lets check the value.
	// If it is EConfigured then we are done, and can return to the waiting client
	// however, if it is not yet at level aFinishedState, then we have to wait for the
	// property value to change, by subscribing to it, and doing a synchronous wait.  If the
	// Get() fails, then we assume something went wrong, and return the error to the client!
	TRequestStatus propertyStatus = KErrNone;

	while(propertyStatus == KErrNone && propertyValue < aFinishedState)
		{
		__FLOG_1(_L("StartC32 - Published configuration sequence is %d."), propertyValue);
		configurationProperty.Subscribe(propertyStatus);

		if(configurationProperty.Get(propertyValue) == KErrNone && propertyValue>= aFinishedState)
			{
			__FLOG(_L("StartC32 - Cancelling subscription."));
			configurationProperty.Cancel();
			User::WaitForRequest(propertyStatus);	// absorb signal
			__FLOG(_L("StartC32 - Absorbed signal."));
			break;
			}
		User::WaitForRequest(propertyStatus);
		}

	__FLOG_1(_L("StartC32 - Published configuration sequence is %d."), propertyValue);
	__ASSERT_DEBUG(result == KErrNone && propertyResult == KErrNone, User::Panic(KSpecAssert_RootSrvc32rootc, 1)); // if it isn't defined then either C32Start didn't define the property before rendezvousing or PubSub went wrong
	configurationProperty.Close();
	__FLOG(_L("StartC32 - Root Server configured"));

	__FLOG_CLOSE;
    return result;
    }

EXPORT_C TInt StartC32()
    /** Start the Root Server and the c32start.exe example configuration program.
    Any caller of this function is blocked until the Root Server is
    fully loaded and configured by the example configuration utility: c32start.exe.
    This is because any called expects that it can instantly use networking
    services offered by the modules loaded during configuration.
    return TInt - An error code.
    @publishedPartner
    @released
    */
    {
	// Start with all CMI's enabled
	return DoStartC32(KNullDesC, ECoreComponentsStarted);
    }

EXPORT_C TInt StartC32WithCMISuppressions(const TDesC& aCMISuppressionList)
    /** Start the Root Server and the c32start.exe example configuration program,
	supplying a list of CMI files that must not be processed. This functionality is
	provided expressly for the ease of legacy testing.
    Any caller of this function is blocked until the Root Server is
    fully loaded and configured by the example configuration utility: c32start.exe.
    This is because any called expects that it can instantly use networking
    services offered by the modules loaded during configuration.
    return An error code.
    @publishedPartner
    @released
    */
    {
	return DoStartC32(aCMISuppressionList, ECoreComponentsStarted);
    }

EXPORT_C TInt StartC32Full()
    /** As StartC32 but doesn't return until c32 is fully up.
		
	@see StartC32()
    @internalTechnology
    */
    {
	// Start with all CMI's enabled
	return DoStartC32(KNullDesC, EConfigurationComplete);
    }

EXPORT_C TInt WarmBootC32(const TDesC& aCMISuppressionList)
    {
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KSubsys, KComponent);
	__FLOG_1(_L("WarmBootC32(\"%S\")"), &aCMISuppressionList);

	// Start by looking for the running configurator and kill it if found. The caller
	// needs the appropriate capability to do this

	__FLOG(_L("Finding existing configurator process."));
	_LIT(KC32StartName, "*");
	TInt result;
	TBool configuratorWasPresent;
	TInt count = 0;
	do
		{
		configuratorWasPresent = EFalse;
		TFullName fn;
		TFindProcess fp(KC32StartName);
		while(fp.Next(fn) == KErrNone)
			{
			RProcess proc;
			result = proc.Open(fn);
			if(result == KErrNone)
				{
				TUidType type = proc.Type();
				if(type[2] == TUid::Uid(KUidC32StartProcess) && proc.ExitType() == EExitPending) 
					{
					// Kill the existing configurator process.
					__FLOG_VA( (_L("Opened existing configurator process \"%S\""), &fn) );
					TRequestStatus status;
					proc.Logon(status);
					proc.Kill(KErrNone);
					__FLOG_VA( (_L("Killed process."), &fn) );
					User::WaitForRequest(status);
					__FLOG_STMT( TExitType exitType = proc.ExitType() );
					
					// Create a timer in case some other entity holds an open handle on the
					// configurator which prevents the kernel from destroying it.  We timeout
					// after one second.
					TAutoClose<RTimer> timer;
					if(timer.iObj.CreateLocal() == KErrNone)
						{
						// Request destruction notification so we know when it is safe to start
						// the process again.
						TRequestStatus destructionStatus;
						proc.NotifyDestruction(destructionStatus);
						proc.Close();
					
						enum{ KProcessDestructionTimeout = 1000000 };
  					
						TRequestStatus timerStatus;
						timer.iObj.After(timerStatus, KProcessDestructionTimeout);
						
						// Wait for the process to be destroyed or for the timeout.
						User::WaitForRequest(destructionStatus, timerStatus);
						if(timerStatus.Int() == KRequestPending)
							{
							timer.iObj.Cancel();
							User::WaitForRequest(timerStatus);
							}
						else
							{
							User::CancelMiscNotifier(destructionStatus);
							User::WaitForRequest(destructionStatus);
			
							__FLOG_1(_L("Existing configurator process has still not been destroyed after %f.0s"), KProcessDestructionTimeout / 1000000.0);
							}
						}
					else
						{
						proc.Close();
						}

					__FLOG_VA( (_L("Process logon completed with %d, exitType %d"), status.Int(), exitType) );
					configuratorWasPresent = ETrue;
					}
				else
					{
					proc.Close();
					}
				}
			}
		} while(configuratorWasPresent && ++count < KMaxKillRetry);

		if (count >= KMaxKillRetry)
			{
			__FLOG(_L("WarmBootC32 - Cannot kill the configurator process, we will try continue and hope all is well"));
			}

	// Restart the boot sequence from scratch - the configurator reads this.
	RProperty::Set(KUidSystemCategory, KUidC32StartPropertyKey.iUid, EReset);
	__FLOG_CLOSE;
	result=StartC32WithCMISuppressions(aCMISuppressionList);
	return result;
	}


