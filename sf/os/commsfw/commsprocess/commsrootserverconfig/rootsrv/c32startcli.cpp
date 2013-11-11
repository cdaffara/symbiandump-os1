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
 @file
 @internalTechnology
*/
#include<e32std.h>
#include<e32cmn.h>
#include<e32base.h>
#include <comms-infras/c32startcli.h>

#include <comms-infras/commsdebugutility.h>

__FLOG_STMT(_LIT8(KSubsysRs,"RootServer");) // subsystem name
__FLOG_STMT(_LIT8(KComponentRs,"event");) // component name

using namespace RootServer;

EXPORT_C RRsConfigurator::RRsConfigurator()
	/** Default Constructor
	*/
	{
	}
	
EXPORT_C TInt  RRsConfigurator::Connect()
	/** Use this function to open a session to the Configurator Server,
	which is used to load the modules on demand.
    Default number of message slots is defined in KDefaultMessageSlots.
    As with any server, the connection should be closed after use.
    RHandleBase provides the necessary Close() function, which should be
    called when the server session is no longer required. 
    @return An error code
    @internalTechnology
    @capability NetworkControl 
	*/
	{
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_OPEN(KSubsysRs, KComponentRs);
	__FLOG_1(_L("RRsConfigurator::Connect: client %d"), (TUint) RThread().Id());

	TInt err = CreateSession(KRsConfiguratorServerName, TVersion(0, 0, 0));
	
	if (err==KErrNotFound)
		{
		err = StartC32();
		if (err==KErrNone)
			{
			err=CreateSession(KRsConfiguratorServerName, TVersion(0, 0, 0));
			}
		}

	__FLOG_CLOSE;
    return(err); 
	}


EXPORT_C void RRsConfigurator::LoadCpm(const TCFModuleName& aName,TRequestStatus& aStatus)
	/** Launch a new Comms Provider Module on demand
	The name of the new CPM instance must be unique and must
	be used in any further communication concerning this CPM instance, such as binding.
	@param aName Name of module to load.
	@see TCFModuleName
    @internalTechnology
    @capability NetworkControl
	*/
	{
	SendReceive(CSLoadCpm, TIpcArgs(&aName), aStatus);
	}

EXPORT_C void RRsConfigurator::UnloadCpm(const TCFModuleName& aName,TRSUnLoadType aType, TRequestStatus& aStatus)
	/** Unloads a Comms Provider Module. This interannly calls root server UnLoadCpm() method
    @param aStatus The request status used to contain completion information for the function.
    @param aName Name of module to unload
    @param aType unload type ( Graceful/Optional/ungraceful/immediate)
	@see TCFModuleName
	@see TRSUnLoadType
	@internalTechnology
    @capability NetworkControl PowerMgmt
    */
	{
	SendReceive(CSUnloadCpm, TIpcArgs(&aName,aType), aStatus);
	}

EXPORT_C void RRsConfigurator::CancelLoadCpm(const TCFModuleName& aName)
	/** Cancels asynchronous loading of a comms Provider Module.
    There is no guarantee the module loading is canceled.
    A best effort is made and the asynchronous request waiting for
    the module to load returns with KErrCanceled on success.
    @param aName Name of module which is loading.
    @see TCFModuleName
    @internalTechnology
    @capability NetworkControl
    */
	{
	SendReceive(CSCancelLoadCpm, TIpcArgs(&aName));
	}

EXPORT_C void RRsConfigurator::CancelUnloadCpm(const TCFModuleName& aName)
	/** Cancel asynchronous unloading of a comms Provider Module.
    There is no guarantee the module unloading is canceled.
    A best effort is made and the asynchronous request waiting for
    the module to load returns KErrCanceled on success.
    @param aName Name of module which is unloading.
    @see TCFModuleName
    @internalTechnology
    @capability NetworkControl PowerMgmt
    */
	{
	SendReceive(CSCancelUnloadCpm, TIpcArgs(&aName));
	}


EXPORT_C TInt RRsConfigurator::EnumerateModules(const TCFGroupName& aGroupName, TRSIter &aPosition, TCFModuleName& aModuleName)
	/** Through repeated calls returns the list of module names that are associated with provided group name. 
	@param aGroupName for which the module names have to be retrieved
	@param aPosition Position in enumeration.
    @param aName Contains module upon successful return.
    @see TCFModuleName
    @see TCFGroupName
    @return An error code.
    @internalTechnology
    @capability NetworkControl
	*/
	{
	__FLOG_STATIC0(KSubsysRs,KComponentRs,_L("RRsConfigurator::EnumerateModules"));
	TPckg<TRSIter> pkgPos(aPosition);
	TInt err = SendReceive(CSEnumerateModules, TIpcArgs(&pkgPos, &aGroupName,&aModuleName));
	return err;
	}

EXPORT_C TInt RRsConfigurator::GetModuleIniData(const TCFModuleName& aModuleName, TDes8& aIniData, TInt& aRequiredInidataSize)
	/** Gets you the module [inidata] section, which contains information specific to CPM
	@param aName Name of module for which [inidata] section is fetched.
	@param aIniData contains inidata on return.
	@param aRequiredInidataSize contains actual size of the inidata on return.Using this the caller may resize the buffer and use the method again in case of KErrOverflow. 
	@return An error code.
	@internalTechnology
	@capability NetworkControl
	*/
	{
	TPckg<TInt> pkgActualSize(aRequiredInidataSize);
	TInt err = SendReceive(CSGetModuleIniData, TIpcArgs(&aModuleName, &aIniData, &pkgActualSize));
	return err;
	}


