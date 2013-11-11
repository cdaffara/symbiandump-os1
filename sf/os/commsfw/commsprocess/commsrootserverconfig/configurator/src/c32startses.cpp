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
// Implementation of the Configurator server Session for on demand loading of CPMs.
// 
//

#include "c32startshared.h"
#include "c32startserv.h"

/**
@file 
Implements Configurator server session implementation.

@publishedPartner
*/

CRsConfiguratorServerSession::CRsConfiguratorServerSession(const CRsConfiguratorServer* aConfiguratorServer)
:CSession2(),
iConfiguratorServer((CRsConfiguratorServer*) aConfiguratorServer)
	/** C'tor
	*/
	{
	}


TInt CRsConfiguratorServerSession::LoadCpm(const RMessage2& aMessage)
	/** Loads comms provider module which is marked as on demand in CMI file.
	*/
	{
	TCFModuleNameF moduleName;
	TInt result = aMessage.Read(0, moduleName);

	if(KErrNone == result)
		{
		__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServerSession::LoadCpm (%S)"), &moduleName);
		result = iConfiguratorServer->LoadCpm(moduleName, aMessage);
		}
	else
		{
		__FLOG_STATIC1(KSubsys,KComponent,_L("CRsConfiguratorServerSession::LoadCpm error reading module name %d"), result);
		}
			
	return result;	
	}
	
TInt CRsConfiguratorServerSession::EnumerateModules(const RMessage2& aMessage)
	/** Returns a module name from list of modules which matches the group name. 
	*/
	{
	__FLOG_STATIC0(KSubsys,KComponent,_L("CRsConfiguratorServerSession::EnumerateModules"));
	TPckgBuf<TRSIter> ipcPosition;
	TInt result = aMessage.Read(0, ipcPosition);
	if(KErrNone == result)
		{
		TInt& position = ipcPosition()();
		TCFGroupNameF groupName;
		result = aMessage.Read(1, groupName);
		TCFModuleNameF moduleName;
		result = iConfiguratorServer->EnumerateModules(groupName,position, moduleName);
		if(KErrNone == result)
       		{
			result = aMessage.Write(2, moduleName);
			if(KErrNone == result)
       			{
				++position;
				result = aMessage.Write(0, ipcPosition);
				}
			}
		}
    return result;
	}


TInt CRsConfiguratorServerSession::GetModuleIniData(const RMessage2& aMessage)
	/** Fetches data contained by [inidata] section of CMI file for Comms Provider Module.
	*/
	{
	TCFModuleNameF moduleName;
	TInt result = aMessage.Read(0, moduleName);
 
	if(KErrNone == result)
		{
		__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServerSession::GetModuleIniData (%S)"), &moduleName);
    	result = iConfiguratorServer->GetModuleIniData(moduleName, aMessage);
		}
	else
		{
		__FLOG_STATIC1(KSubsys,KComponent,_L("CRsConfiguratorServerSession::GetModuleIniData error reading module name %d"), result);
		}

    return result;	
	}


void CRsConfiguratorServerSession::ServiceL(const RMessage2& aMessage)
	{	
	TInt result;
	switch(aMessage.Function())
		{
		case CSLoadCpm:
			// if load request is successful KErrNone is returned
			// message is completed once the root server completes loading.
			// if any other error before load request, message is completed here.
			result = LoadCpm(aMessage); 
			if(result!=KErrNone)
				{
				aMessage.Complete(result);
				}
			break;
		
		case CSUnloadCpm:
			result = UnloadCpm(aMessage);
			if(result!=KErrNone)
				{
				aMessage.Complete(result);
				}
			break;
		
		case CSCancelLoadCpm:
			result = CancelLoadCpm(aMessage);
			aMessage.Complete(result);
			break;
		
		case CSCancelUnloadCpm:
			result = CancelUnloadCpm(aMessage);
			aMessage.Complete(result);
			break;
			
		case CSEnumerateModules: 
			result=EnumerateModules(aMessage);
			aMessage.Complete(result);
			break;
		
		case CSGetModuleIniData:
			result=GetModuleIniData(aMessage);
			aMessage.Complete(result);
			break;
		
		default:
			aMessage.Complete(KErrNotSupported);
			break;	
		}
		
	
	}



TInt CRsConfiguratorServerSession::UnloadCpm(const RMessage2& aMessage)
	/**Unloads comms Provider Module loaded by the configurator
	*/
	{
	TCFModuleNameF moduleName;
	TInt result = aMessage.Read(0, moduleName);

	if(KErrNone == result)
		{
		__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServerSession::UnloadCpm (%S)"), &moduleName);
		result = iConfiguratorServer->UnLoadCpm(moduleName, aMessage);
		}
	else
		{
		__FLOG_STATIC1(KSubsys,KComponent,_L("CRsConfiguratorServerSession::UnloadCpm error reading module name %d"), result);
		}

	return result;	
	}

TInt CRsConfiguratorServerSession::CancelLoadCpm(const RMessage2& aMessage)
	/**Cancel asynchronous loading of a comms Provider Module. 
	There is no guarantee the module unloading is canceled. 
	A best effort is made and the asynchronous request waiting for the module 
	to load returns KErrCanceled on success.
	*/
	{
	TCFModuleNameF moduleName;
	TInt result = aMessage.Read(0, moduleName);

	if(KErrNone == result)
		{
		__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServerSession::CancelLoadCpm (%S)"), &moduleName);
		result = iConfiguratorServer->CancelLoadCpm(moduleName);
		}
	else
		{
		__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServerSession::CancelLoadCpm error %d reading module name"), result);
		}

	return result;	
	}

TInt CRsConfiguratorServerSession::CancelUnloadCpm(const RMessage2& aMessage)
	/**Cancel asynchronous unloading of a comms Provider Module. 
	There is no guarantee the module unloading is canceled. 
	A best effort is made and the asynchronous request waiting for the module 
	to load returns KErrCanceled on success.
	*/
	{
	TCFModuleNameF moduleName;
	TInt result = aMessage.Read(0, moduleName);

	if(KErrNone == result)
		{
		__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServerSession::CancelUnloadCpm (%S)"), &moduleName);
		result = iConfiguratorServer->CancelUnLoadCpm(moduleName);
		}
	else
		{
		__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServerSession::CancelUnloadCpm error %d reading module name"), result);
		}

	return result;	
	}	


