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
// Implementation of the Configurator server for on demand loading of CPMs.
// 
//

#include <cfextras.h>
#include <e32svr.h>
#include <e32uid.h>
#include <e32base.h>
#include "c32startshared.h"
#include "c32startserv.h"
#include "c32startPolicy.h"

/**
@file 
Implements the Configurator Server side.

@internalTechnology
*/

CRsConfiguratorServer::CRsConfiguratorServer(CCommsProcessStarter& aCommsProcess)
	: CPolicyServer(CActive::EPriorityStandard,iPolicy,ESharableSessions),
	iCommsProcess(aCommsProcess)
	/** C'tor 
	@param aCommsProcess base class for managing CPMs
	*/
	{
	}

CRsConfiguratorServer::~CRsConfiguratorServer()
	/** D'tor
	*/
	{
	}
CSession2* CRsConfiguratorServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	/** Create new session object.Connect API is policed for its capability.
     @internalComponent
     */
	{
	return new(ELeave) CRsConfiguratorServerSession(this);
	}


CRsConfiguratorServer* CRsConfiguratorServer::NewL(CCommsProcessStarter& aCommsProcess)
	 /** Create new Configurator Server instance.
     This is the only way of instantiation
     @internalComponent
     @param aCommsProcess base class for CPM management
     */
	{
	CRsConfiguratorServer* self = new(ELeave) CRsConfiguratorServer(aCommsProcess);
	CleanupStack::PushL(self);
	self->ConstructL();	
	CleanupStack::Pop(self);
	return self;
	}

CRsConfiguratorServer* CRsConfiguratorServer::NewLC(CCommsProcessStarter& aCommsProcess)
	/** Create new Configurator Server instance.
     This is the only way of instantiation
     @internalComponent
     @param aCommsProcess base class for CPM management
     */
	{
	CRsConfiguratorServer* self=new(ELeave) CRsConfiguratorServer(aCommsProcess);
	CleanupStack::PushL(self);
	self->ConstructL();	
	return self;
	}

void CRsConfiguratorServer::ConstructL()
	/** Second level construction of ConfiguratorServer.
	Configurator Server is started
	@internalComponent
	*/
	{
	StartL(KRsConfiguratorServerName);
	}


TInt CRsConfiguratorServer::EnumerateModules(const TCFGroupNameF& aGroupName, TInt& aStartAt, TCFModuleNameF& aName)
	/** Returns a module name from list of modules which matches the group name
	@param aGroupName name of a group to which module belongs.
	@param aStartAt search for aName starting at this position.
	@param aName returns the name for matching group.
	*/
	{
	RPointerArray<CModuleRef>& modules=GetModulesList();
	TInt count = modules.Count();
    if(aStartAt >= count)
    	{
    	return KErrEof;
    	}
    
    for(TInt i=aStartAt;i<count;i++)
    	{
    	CModuleRef* pModule = modules[i];
		if(aGroupName == pModule->GroupName())
			{
			aName = pModule->Name();
			aStartAt = i; // to retain the last found position
			__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::EnumerateModules Module Found (%S)"),&aName);
			return KErrNone;
			}
    	}
     __FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::EnumerateModules RSModuleUnknown For Group (%S)"),&aGroupName);    
   	return KErrRSModuleUnknown;
	}

TInt CRsConfiguratorServer::LoadCpm(const TCFModuleNameF& aName,const RMessage2& aMessage)
	/** Loads the CPM module.
	retunrs KErrNone on succesfull load request.
	KErrRSModuleAlreadyExist if module is already loaded.
	KErrRSModuleUnknown if Module is not listed.
	KErrRSModuleNotLoaded if load is fails.
	@param aName name of the module to be loaded.
	*/
	{
	__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::LoadCpm (%S)"),&aName);    
	CModuleRef* pModule;
    if((pModule = FindCpm(aName)) != NULL)
		{
		if(pModule->State()==ENotLoaded)
			{
			// RMessage set here, determines if a module's message can be completed or not.
			// message completion is not on "OnDemand" property, as a boot time module can also be On-Demand loaded.
			pModule->SetRMessage(aMessage);
			// check if any of the bindings has this OnDemand module, if yes, set iHasBinding = ETrue
			RPointerArray<CBindingRef>& bindings = iCommsProcess.GetBindingList();
			for(TInt i=0; i<bindings.Count(); i++)
				{
				const CBindingRef& bind(*bindings[i]);
				if(pModule->Name() == bind.FromModule().Module() || pModule->Name() == bind.ToModule().Module())
					{
					pModule->SetHasBindings();
					break;
					}
				}
			iCommsProcess.LoadOnDemandModule(*pModule);
			return KErrNone;
			}
		else if ((pModule->State()==ELoaded) || (pModule->State()==ELoading))
			{
			__FLOG_1(_L8("CRsConfiguratorServer::LoadCpm - %S ModuleAlreadyExist"),&aName);
			return KErrRSModuleAlreadyExist;
			}
		}
	__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::LoadCpm (%S) ModuleUnknown"),&aName);
	return KErrRSModuleUnknown;
	}

TInt CRsConfiguratorServer::GetModuleIniData(const TCFModuleNameF& aName,const RMessage2& aMessage)
	/** Get the data from [inidata] section for a given module name.
	@param aName name of the module for which inidata section has to be retrieved.
	@param aInidata contains inidata for given module name.
	@param aActualSize on return contains actual size of [inidata]
	@return an error code.  
	*/
	{
	__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::GetModuleIniData (%S) "),&aName);
	CModuleRef* pModule;
    TPckgBuf<TInt> actualLength;
    if((pModule = FindCpm(aName)) != NULL)
		{
		TInt deslength = aMessage.GetDesMaxLength(1); // Buffer length to get the contents
		if(deslength <0)
			{
			return deslength;
			}
		HBufC8* inidata = pModule->GetIniData();
		if(inidata != NULL)
			{
			actualLength = inidata->Length(); // Actual length of the inidata
			TInt err = aMessage.Write(2, actualLength);
			if (err == KErrNone )
				{
				if(inidata->Length() <= deslength)
					{
					err = aMessage.Write(1, *inidata);
					}
				else
					{
					return KErrOverflow;	
					}	
				}
			return err;	
			}
		else // No inidata Section return actual size as 0"Zero".	
			{
			actualLength = 0;
			TInt err = aMessage.Write(2, actualLength);	
			return KErrNone; 
			}	
		}
		
	__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::GetModuleIniData (%S) ModuleUnknown "),&aName);
	return KErrRSModuleUnknown;	
	
	}
	
TInt CRsConfiguratorServer::UnLoadCpm(const TCFModuleNameF& aName, const RMessage2& aMessage)
	/** Unloads module loaded by configurator itself. This calls root server UnloadCpm() internally. 
	@param aName name of the module to be unloaded.
	@param aMessage which is to be completed once the root server unloads the module. 
		Else message would be completed by the Configurator server session ServiceL with error returned from here.
	@return an error code.  
	*/
	{
	__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::UnLoadCpm (%S)"),&aName);
	
	CModuleRef* pModule;
	TRSUnLoadType unloadType = (TRSUnLoadType) aMessage.Int1();
    if((pModule = FindCpm(aName)) != NULL)
		{
		if(pModule->State()==ELoaded)
			{
			// RMessage set here, determines if a module's message can be completed or not.
			// message completion is not on "OnDemand" property, as a boot time module can also be On-Demand unloaded.
			pModule->SetRMessage(aMessage);
			pModule->UnLoadCpm(aName,unloadType);
			return KErrNone;
			}
		else
			{
			return KErrRSModuleNotLoaded;	
			}	
		}
	
	__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::UnLoadCpm (%S) ModuleUnknown "),&aName);
	return KErrRSModuleUnknown;
	}
	
TInt CRsConfiguratorServer::CancelUnLoadCpm(const TCFModuleNameF& aName)	
	/** Cancel asynchronous unloading of a comms Provider Module. 
	There is no guarantee the module unloading is canceled. 
	A best effort is made and the asynchronous request waiting for the module 
	to load returns KErrCanceled on success.
	
	@param aName name of the unloading module.
	@return an error code.
	*/
	{
	__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::CancelUnLoadCpm (%S)"),&aName);
	
	CModuleRef* pModule;
    if((pModule = FindCpm(aName)) != NULL)
		{
		pModule->CancelUnLoadCpm(aName);
		return KErrNone;
		}
	
	__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::CancelUnLoadCpm (%S) ModuleUnknown "),&aName);
	return KErrRSModuleUnknown;
	}
	
TInt CRsConfiguratorServer::CancelLoadCpm(const TCFModuleNameF& aName)	
	/**Cancels asynchronous loading of a comms Provider Module. 
	There is no guarantee the module loading is canceled. 
	A best effort is made and the asynchronous request waiting for the module 
	to load returns with KErrCanceled on success. 
	
	@param aName name of the unloading module.
	@return an error code.
	*/
	{
	__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::CancelLoadCpm (%S)"),&aName);
	
	CModuleRef* pModule;
    if((pModule = FindCpm(aName)) != NULL)
		{
		pModule->CancelLoadCpm(aName);
		return KErrNone;
		}
	
	__FLOG_STATIC1(KSubsys,KComponent,_L8("CRsConfiguratorServer::CancelLoadCpm (%S) ModuleUnknown "),&aName);
	return KErrRSModuleUnknown;
	}	
	
CModuleRef* CRsConfiguratorServer::FindCpm(const TCFModuleNameF& aName)		
	/**Find a comms provider module by name.
	Search the modules array for a cpm with the matching name
    if found return a pointer, otherwise return NULL.
    @param aName The name of the cpm to be found.
    @return The cpm found or NULL
	@internalComponent	
	*/
	{
	RPointerArray<CModuleRef>& modules = GetModulesList();
	TInt count = modules.Count();
	for(TInt i=0;i<count;i++)
		{
		CModuleRef* pModule = modules[i];
		if(aName == pModule->Name())
			{
			return pModule;
			}
		}
	return NULL;
	}




