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

/**
 @file 
 @internalTechnology
 @released
*/

#include <procroot.h>

using namespace RootServer; // Allowing the usage of definitions from the RS
using namespace CommsFW; // Allowing the usage of definitions from the Comms Framework

EXPORT_C TInt RProcessRoot::Connect(const TDesC& aServerName)
    /** Use this function to open a session to the Root Server.
    Default number of message slots is defined in KDefaultMessageSlots.
    As with any server, the connection should be closed after use.
    RHandleBase provides the necessary Close() function, which should be
    called when the server session is no longer required. 
    @return An error code
    @internalTechnology 
    @released
    */
    {

    TInt r = CreateSession(aServerName, Version());

    return r; 
    }
    
EXPORT_C TVersion RProcessRoot::Version() const
    /** Returns the client side version number.
    Use this function to get the version number. The version number may be
    incremented in future releases of the Root Server. If extra features are
    added in such releases, the version number may be used by application programs
    as a basis for assessing the capabilities of the Root Server. 
    @return version number
    @internalTechnology 
    @released
    */
    {
    return(TVersion(KRProcRootMajorVersionNumber,KRProcRootMinorVersionNumber,KRProcRootBuildVersionNumber));
    }
    

EXPORT_C void RProcessRoot::LoadModule(TRequestStatus& aStatus,
                                 const TRSStartModuleParams& aParams, const TDesC8& aIniData)
    /** Launch a new Service Provider Module.
    The name of the new SPM instance (aParams.iParams.iName) must be unique and must
    be used in any further communication concerning this SPM instance, such as binding.
    The contents of the initialisation data parameter (aIniData) are specific to the module.
    It could be empty, contain the contents of an associated INI file or contain some other
    data specified by the specific SPM implementation.
    Attempts to load a SPM with a name equal to that of an existing SPM is not possible.
    It is required that the DLL contains the SPM secondary UID of 0x101f7418. 
    @param aStatus The request status used to contain completion information for the function.
    @param aParams Parameters needed to load the Provider Module.
    @param aIniData Module specific ini data. 
    @see TRSStartModuleParams
    @internalTechnology 
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that 
    this request came from the comms configurator
    */
    {
    RSessionBase::SendReceive(RSLoadModule, TIpcArgs(&aParams, &aIniData), aStatus);
    }
    
EXPORT_C void RProcessRoot::UnloadModule(TRequestStatus& aStatus, const TCFModuleName& aName, TRSUnLoadType aType)
    /** Unloads a Service Provider Module.
    @param aStatus The request status used to contain completion information for the function.
    @param aName Name of module to unload
	@param aType Type of unload (optional/graceful/ungraceful).
	@see TCFModuleName
	@see TRSUnLoadType
	@internalTechnology
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that 
    this request came from the comms configurator
    */
    {
    RSessionBase::SendReceive(RSUnloadModule, TIpcArgs(&aName, aType), aStatus);
    }
    
EXPORT_C void RProcessRoot::SendMessage(TRequestStatus& aStatus, const TCFModuleName& aName, TInt aType, TDes8& aData)
	/**
	Sends a message to the module
	@param aStatus The request status used to contain completion information for the function.
	@param aName xxx
	@param aType a type determining the type of data in aData param
	@param aData a data specific to the message and it's type given by aType. All aData is expected to be
			CNetMessage based serialised instance
	@see TCFMessageType for message/data types
	@see CNetMessage
	*/
	{
   RSessionBase::SendReceive(RSSendMessage, TIpcArgs(&aName, aType, &aData), aStatus);
	}

EXPORT_C TInt RProcessRoot::SendMessage(const TCFModuleName& aName, TInt aType, TDes8& aData)
	/**
	Sends a message to the module
	@param aName xx
	@param aType a type determining the type of data in aData param
	@param aData a data specific to the message and it's type given by aType. All aData is expected to be
			CNetMessage based serialised instance
	@see TCFMessageType for message/data types
	@see CNetMessage
	*/
	{
   return RSessionBase::SendReceive(RSSendMessage, TIpcArgs(&aName, aType, &aData));
	}

EXPORT_C void RProcessRoot::Bind(TRequestStatus& aStatus, TRSBindingInfo& aBindInfo)
    /** Binds two modules.
    Several types of binding are available: EHierarchical is the standard top-bottom
    binding in the protocol hierarchy. If this type is specified, iName1 of TRSBindingInfo is considered top-level
    module of the binding and iName2 is considered the bottom-level part. Also, this type of binding
    allows the top-level module to be a server. ECustom is a "Horizontal" binding between two SPM's.
    Typical use is when a SPM employs another SPM for doing some processing and communicates via the
    standard message queues.
    @param aStatus The request status used to contain completion information for the function.
    @param aBindInfo The information regarding the binding request. 
    @see TRSBindingInfo
    @internalTechnology 
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that 
    this request came from the comms configurator */
    {
    RSessionBase::SendReceive(RSBind, TIpcArgs(&aBindInfo), aStatus);
	}
    
EXPORT_C void RProcessRoot::Unbind(TRequestStatus& aStatus, TRSUnBindingInfo& aUnBindInfo)
    /** Unbinds two modules.
    All bindings between the two modules are unbound and the states for each module
    are updated in the object.
    @param aStatus The request status used to contain completion information for the function.
    @param aUnBindInfo Holds the unbinding info.
    @see TRSUnBindingInfo
    @internalTechnology 
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that 
    this request came from the comms configurator
    */
    {
    RSessionBase::SendReceive(RSUnbind, TIpcArgs(&aUnBindInfo), aStatus);
    }
    
EXPORT_C TInt RProcessRoot::GetModuleInfo(const TCFModuleName& aName, TRSModuleInfo& aModuleInfo)
    /** Gets information about one module.
    @param aName Name of module to query.
    @param aModuleInfo Information about the module state.
    @see TCFModuleName
    @see TRSModuleInfo
    @return An error code.
    @internalTechnology 
    @released
    */
    {
    return RSessionBase::SendReceive(RSGetModuleInfo, TIpcArgs(&aName, &aModuleInfo));
    }  
  
EXPORT_C TInt RProcessRoot::EnumerateModules(TRSIter &aPosition, TCFModuleName& aModuleName)
    /** Through repeated calls returns the list of names of running Service Provider Modules. 
    Notes:
	- The current position in an enumeration is maintained for each client session, meaning that 
	  to perform two consecutive enumerations two RProcessRoot's must be used. 
	- The order in which the modules are returned is undefined.
	- The enumeration returned is not a snapshot from the start, i.e. if modules are loaded or 
	  unloaded during the enumeration then a module could be skipped or returned twice.
    @param aPosition Position in enumeration.
    @param aModuleName Contains module upon successful return.
    @return An error code: KErrNone upon success, KErrEOF upon reaching the end of the enumeration.
    @see TRSIter
    @see TCFModuleName
    @internalTechnology 
    @released
    */
    {     
	TPckg<TRSIter> pkgPos(aPosition);
    return RSessionBase::SendReceive(RSEnumerateModules, TIpcArgs(&pkgPos, &aModuleName));
    }
    
EXPORT_C TInt RProcessRoot::EnumerateSubModules(const TCFModuleName& aModuleName, TRSIter &aPosition, TCFSubModuleName& aSubModuleName)
    /** Returns a list of running Service Provider Modules.
    The client can enumerate sub-modules one at a time by providing a TCFSubModuleName.
    The enumerator aPosition is a correlator for Rootserver use.
    @param aModuleName Name of module.
    @param aPosition Position in enumeration, correlator for the Rootserver.
    @param aSubModuleName A name to be returned
    @return An error code.
    @see TCFModuleName
    @see TRSIter
    @see TCFSubModuleName
    @internalTechnology 
    @released
    */
    {        
	TPckg<TRSIter> pkgPos(aPosition);
    return RSessionBase::SendReceive(RSEnumerateSubModules, TIpcArgs(&aModuleName, &pkgPos, &aSubModuleName));
    }
    
EXPORT_C TInt RProcessRoot::EnumerateBindings(const TCFSubModuleAddress& aSubModuleAddr, TRSIter &aPosition, TRSBindingInfo& aBinding)
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
    @internalTechnology 
    @released
    */
    {
	TPckg<TRSIter> pkgPos(aPosition);
	TRSSubModuleAddress pckgSubModuleAddr(aSubModuleAddr);
    return RSessionBase::SendReceive(RSEnumerateBindings, TIpcArgs(&pckgSubModuleAddr, &pkgPos, &aBinding));
    }
    
EXPORT_C void RProcessRoot::CancelLoadModule(const TCFModuleName& aName)
    /** Cancels asynchronous loading of a Service Provider Module.
    There is no guarantee the module loading is canceled.
    A best effort is made and the asynchronous request waiting for
    the module to load returns with KErrCanceled on success.
    @param aName Name of module which is loading.
    @see TCFModuleName
    @internalTechnology 
    @released
    */
    {
    (void)RSessionBase::SendReceive(RSCancelLoadModule, TIpcArgs(&aName));
    }
    
EXPORT_C void RProcessRoot::CancelUnloadModule(const TCFModuleName& aName)
    /** Cancel asynchronous unloading of a Service Provider Module.
    There is no guarantee the module unloading is canceled.
    A best effort is made and the asynchronous request waiting for
    the module to load returns KErrCanceled on success.
    @param aName Name of module which is unloading.
    @see TCFModuleName
    @internalTechnology 
    @released
    */
    {
    (void)RSessionBase::SendReceive(RSCancelUnloadModule, TIpcArgs(&aName));
    }
    
EXPORT_C void RProcessRoot::CancelBind(TRSSubModuleAddress& aName1, TRSSubModuleAddress& aName2)
    /** Cancels asynchronous binding of two modules.
    There is no guarantee the binding is canceled.
    A best effort is made and the asynchronous request waiting for
    the module to load returns KErrCanceled on success.
    @param aName1 Name of module which is binding to aName2.
    @param aName2 Name of module which is binding to aName1.
    @see TRSSubModuleAddress
    @internalTechnology 
    @released
    */
    {
    (void)RSessionBase::SendReceive(RSCancelBind, TIpcArgs(&aName1, &aName2));
    }
    
EXPORT_C void RProcessRoot::CancelUnbind(TRSSubModuleAddress& aName1, TRSSubModuleAddress& aName2)
    /** Cancels asynchronous unbinding of two modules.
    There is no guarantee the unbinding is canceled.
    A best effort is made and the asynchronous request waiting for
    the module to load returns KErrCanceled on success.
    @param aName1 Name of module which is unbinding from aName2.
    @param aName2 Name of module which is unbinding from aName1.
    @see TRSSubModuleAddress
    @internalTechnology 
    @released
    */
    {
    (void)RSessionBase::SendReceive(RSCancelUnbind, TIpcArgs(&aName1, &aName2));
    }
    
EXPORT_C TInt RProcessRoot::Shutdown()
	/** Shuts down the process RootServer
	@internalTechnology 
    @released
    @capability NetworkControl The Security ID of the caller will be checked to ensure that 
    this request came from the comms configurator
    */
	{
	return RSessionBase::SendReceive(RSShutdown);
	}

EXPORT_C void RProcessRoot::Close()
	/** Tells server that this session is being closed
 	@internalTechnology 
    @released
     */
    {
    (void)RSessionBase::SendReceive(RSCloseSession);
	RHandleBase::Close();
    }


