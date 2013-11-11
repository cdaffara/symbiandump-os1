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
// The Rootserver executeable. Runs the Rootserver from c32rootsrv.dll.
// 
//

/**
 @file
 @internalComponent
*/

#include <rsshared.h>
#include <cflog.h>
#include <commsdattypesv1_1.h>	// for initialising CommsDat
#include "c32Exe.h"
using namespace RootServer;

#include <comms-infras/commsdebugutility.h>


__CFLOG_STMT(_LIT8(KLogSubSysRS, "RootServer");) // subsystem name

CCommsProviderModule* CC32ProviderModule::NewL(CRootServer* aRootServer, 
									const TRSStartModuleParams& aParams, 
								   	HBufC8* aIniData)
    /** Creates a new instance of a CPM.
	 @param aRootServer The root server object
	 @param aParams The data used to load the module
	 @param aIniData The initialisation data for the module
     */
    {

    CC32ProviderModule* pCPM = new (ELeave) CC32ProviderModule();
    CleanupStack::PushL(pCPM);
    pCPM->ConstructL(aRootServer, aParams, aIniData);
    CleanupStack::Pop(pCPM);
    return pCPM;
    }

TInt CC32ProviderModule::CreateModuleThread(const TDesC& aName, TThreadFunction aOtherThreadFunction,
						TInt aStackSize, RHeap* aHeap, TAny* aModuleArgs)
    {
    return iRCFThread.Create(aName, aOtherThreadFunction, aStackSize, aHeap, aModuleArgs);
    }

CRootServer* CC32RootServer::NewL(const TRootServerStartupInfo& aRootServerStartupInfo)
    /** Create new rootserver instance.
     This is the only way of instantiation
     @internalComponent
     @param aRootServerStartupInfo Information containing name of the server and priority of the server active object.
     */
    {
    CC32RootServer* pS=new (ELeave) CC32RootServer(aRootServerStartupInfo.iPriority);
    CleanupStack::PushL(pS);
	pS->iCommsProviderModuleCtorL = CC32ProviderModule::NewL;
    pS->ConstructL(aRootServerStartupInfo);
	// Now that we're guaranteed to be a singleton initialise CommsDat
	__CFLOG(KLogSubSysRS, KLogCode, _L8("RunRootServer - Initializing Commsdat"));
	CommsDat::CMDBSession* ptrCommsdatSession = NULL;
	TRAPD(result, ptrCommsdatSession = CommsDat::CMDBSession::NewL(KCDVersion1_2));
	if (result == KErrNone)
		{
		result = ptrCommsdatSession->PublishProperties();
		if(result != KErrNone)
			{
			__CFLOG_1(KLogSubSysRS, KLogCode, _L8("RunRootServer - Error %d in CMDBSession::PublishProperties"), result);
			}
		delete ptrCommsdatSession;
		}
	else
		{
		__CFLOG_1(KLogSubSysRS, KLogCode, _L8("RunRootServer - Error %d creating Commsdat object"), result);
		}
	__CFLOG(KLogSubSysRS, KLogCode, _L8("RunRootServer - Commsdat initialized"));

    CleanupStack::Pop(pS);
    return pS;
	}

CC32RootServer::~CC32RootServer()
    {
#ifdef SYMBIAN_ZERO_COPY_NETWORKING    
	iCommsBufPond.Close();
#else
	delete iMBufManager;
#endif // SYMBIAN_ZERO_COPY_NETWORKING	
    }
   
#ifdef _DEBUG
TInt CC32RootServer::RunError(TInt aError)
	{
	__CFLOG_1(KLogSubSysRS, KLogCode, _L8("CRootServer::RunError:%d" ), aError);
	return aError;
	}
#endif // _DEBUG

TInt CC32RootServer::AdjustMBufPool(const TRSSetMBufMnr &aMBufParamsPckg)
    /** Adjust the buffer pool size.
  	 Doesn't actually adjust the pool, can only be called when there
     is no pool instantiated. If a pool is already instantiated, returns KErrInUse.
	 @internalComponent
     @param aBufPoolSize The new size of the buffer pool in bytes.
     @return An error code
     */
    {
    const TRSSetMBufMnrContainer &mbufParams = aMBufParamsPckg();
    __CFLOG_2(KLogSubSysRS, KLogCode, _L8("CRootServer - AdjustMBufPool to %d Init and %d Max."),
     mbufParams.iInitSize, mbufParams.iMaxSize);

    // Can't do this if it is already in use
    if(CpmCount()>0)
        return KErrInUse;
#ifndef SYMBIAN_ZERO_COPY_NETWORKING  
   if(iInitMBufPoolSize == mbufParams.iInitSize && iMaxMBufPoolSize == mbufParams.iMaxSize)
      {
	  return KErrNone;
      }
      
	  // Finally register new size.
	  iInitMBufPoolSize = mbufParams.iInitSize;
	  iMaxMBufPoolSize = mbufParams.iMaxSize;

	  // Delete MBufManager, it will be lazy loaded with new value when needed
	  if(iMBufManager!=NULL)
	  	{
 		delete iMBufManager;
	    iMBufManager=NULL;
        }
#endif // SYMBIAN_ZERO_COPY_NETWORKING
    return KErrNone;    
    }

TInt CC32RootServer::AdjustMBufSizeAllocInfo(const TRSAddMBufAllocInfo &aMBufParamsPckg)
    /** Adjust (aka add) the mbuf size alloc info
	 @internalComponent
     @param aBufPoolSize The new size of the buffer pool in bytes.
     @return An error code
     */
    {
	const TRSAddMBufAllocInfoContainer &mBufAllocSizeInfo = aMBufParamsPckg();
	__CFLOG_4(KLogSubSysRS, KLogCode, _L8("CRootServer::AdjustMBufSizeAllocInfo() - mbufSize=%d initialAlloc=%d minGrowth=%d growthThreshold=%d"),
		mBufAllocSizeInfo.iMBufSize, mBufAllocSizeInfo.iInitialAllocation, mBufAllocSizeInfo.iMinGrowth, mBufAllocSizeInfo.iGrowthThreshold);

    // store params - they will be retrieved  set during the lazy load
	return iMBufSizeAllocInfo.Append(mBufAllocSizeInfo);
    }

void CC32RootServer::LazyLoadL()
    /** Instantiate the MBuf Manager.
     If the MBufManager is not already running, then create a new
     instance of it.
	 @internalComponent
     */
    {
#ifdef SYMBIAN_ZERO_COPY_NETWORKING    
    if(TCommsBufPondTLSOp::Get().IsNull())
        {
        __CFLOG_0(KLogSubSysRS, KLogCode, 
        	_L8("CRootServer - Lazy loading CommsBuf Pond."));
			RArray<TCommsBufPoolCreateInfo> bufPoolCreateInfoArray;
			// configure the mbuf size allocation info
			for (int i=0; i < iMBufSizeAllocInfo.Count(); i++)
				{
				TRSAddMBufAllocInfoContainer &allocInfo = iMBufSizeAllocInfo[i];
				__CFLOG_4(KLogSubSysRS, KLogCode,
					_L8("CRootServer::LazyLoadMBufManagerL(); - about to add mbuf size alloc info - mbufSize=%d initialAlloc=%d minGrowth=%d growthThreshold=%d"),
					allocInfo.iMBufSize, allocInfo.iInitialAllocation,
					allocInfo.iMinGrowth, allocInfo.iGrowthThreshold);
					TCommsBufPoolCreateInfo createInfo;
					createInfo.iBufSize = allocInfo.iMBufSize;
					createInfo.iInitialBufs = allocInfo.iInitialAllocation;
					createInfo.iGrowByBufs = allocInfo.iMinGrowth;
					createInfo.iMinFreeBufs = allocInfo.iGrowthThreshold;
					createInfo.iCeiling = allocInfo.iPoolCeiling;
					bufPoolCreateInfoArray.AppendL(createInfo);
				}
			User::LeaveIfError(iCommsBufPond.Open(bufPoolCreateInfoArray));
        }
#else
    if(iMBufManager==NULL)
	    {
	    __CFLOG_2(KLogSubSysRS, KLogCode, 
	    _L8("CRootServer - Lazy loading MBuf Manager. %d initial bytes - can grow to %d bytes."),
		iInitMBufPoolSize,iMaxMBufPoolSize);

		iMBufManager = CMBufManager::NewL(iMaxMBufPoolSize, iMBufSizeAllocator);
    
		if (iMBufSizeAllocator)
	        {
	        // configure the mbuf size allocation info
			for (int i=0; i < iMBufSizeAllocInfo.Count(); i++)
		        {
		        TRSAddMBufAllocInfoContainer &allocInfo = iMBufSizeAllocInfo[i];
		        __CFLOG_4(KLogSubSysRS, KLogCode,
	            _L8("CRootServer::LazyLoadMBufManagerL(); - about to add mbuf size alloc info - mbufSize=%d initialAlloc=%d minGrowth=%d growthThreshold=%d"),
				allocInfo.iMBufSize, allocInfo.iInitialAllocation,
		        allocInfo.iMinGrowth, allocInfo.iGrowthThreshold);
	            iMBufSizeAllocator->AddL(allocInfo.iMBufSize, allocInfo.iInitialAllocation,
	            allocInfo.iMinGrowth, allocInfo.iGrowthThreshold);
				}

			iMBufSizeAllocator->Commit();
		    }
			iMBufSizeAllocInfo.Close();
		}

#endif // SYMBIAN_ZERO_COPY_NETWORKING        
    }
  
    
TBool CC32RootServer::IsCallerConfigurator(const CRootServerSession* aSession, const RMessage2& aMessage)
	/** If The Configurator has been determined then any other session asking isn't. Otherwise the 
	first configurator process to ask becomes The Configurator. Note that this would fail if The
	Configurator opened a second session, but it doesn't (and it's internalTech)
	*/
	{
	if(!iTheConfiguratorSession)
		{
		RThread client;
		TInt err = aMessage.Client(client);
		if(err == KErrNone)
			{
			static _LIT_SECURITY_POLICY_S1(configuratorPolicy, KUidC32StartProcess, ECapabilityProtServ);
			if(configuratorPolicy.CheckPolicy(client))
				{
				iTheConfiguratorSession = aSession;
				}
			client.Close();
			}
		}
	return iTheConfiguratorSession == aSession;
	}

void CC32RootServer::DisconnectSession(const CRootServerSession* aSession)
	{
	if(iTheConfiguratorSession == aSession)
		{
		iTheConfiguratorSession = NULL;
		}
	}    

CSession2* CC32RootServer::NewSessionL(const TVersion &aVersion, const RMessage2& /*aMessage*/) const
    /** Create new session object.
     @internalComponent
     */
    {
    __CFLOG(KLogSubSysRS, KLogCode,_L8("CC32RootServer - NewSession"));

    TVersion v(TVersion(KRS32MajorVersionNumber,KRS32MinorVersionNumber,KRS32BuildVersionNumber));
    TBool r = User::QueryVersionSupported(v, aVersion);
    if (r==EFalse)
        User::Leave(KErrNotSupported);
    CC32RootServerSession* p = new(ELeave) CC32RootServerSession(this);
    return p;
    }


CC32RootServerSession::CC32RootServerSession(const CRootServer* aRootServer):CRootServerSession(aRootServer)
	{
	}

CC32RootServerSession::~CC32RootServerSession()
	{
	}

void CC32RootServerSession::ServiceL(const RMessage2& aMessage)
	{
    TInt result = KErrGeneral;
    TBool completeNow = ETrue;
    switch ((aMessage.Function()))
		{
		case RSSetMBufPoolSize:
			result = SetMBufPoolSize(aMessage);
			break;
	
		case RSAddMBufAllocInfo:
			result = AddMBufAllocInfo(aMessage);
			break;
	
		case RSIsCallerConfigurator:
			result = IsCallerConfigurator(aMessage);
			break;			
			
		case RSDbgFailNextMbuf:
		case RSDbgSetMbufPoolLimit:
		case RSDbgCheckMbuf:
		case RSDbgMbufFreeSpace:
		case RSDbgMbufTotalSpace:
			result = KErrNotSupported;
			break;
			
		default:
			CRootServerSession::ServiceL(aMessage);
			completeNow = EFalse;
			break;
		}
    
    if(completeNow)
            {
            aMessage.Complete(result);
            }
	}

TInt CC32RootServerSession::IsCallerConfigurator(const RMessage2& aMessage)
	{
	return (TInt) iRootServer->IsCallerConfigurator(this, aMessage);
	}

TInt CC32RootServerSession::SetMBufPoolSize(const RMessage2& aMessage)
    {
    // Do the dirty business and return the result
    TInt ret;
    TRSSetMBufMnr params;
    ret = aMessage.Read(0,params);
    if (ret != KErrNone)
    	{
    	return ret;
    	}
    return (static_cast <CC32RootServer*> (iRootServer))->AdjustMBufPool(params);
    }

// set the mbuf size allocation information
TInt CC32RootServerSession::AddMBufAllocInfo(const RMessage2& aMessage)
    {
    TRSAddMBufAllocInfo params;
    TInt ret = aMessage.Read(0, params);
    if (ret != KErrNone)
    	{
    	return ret;
    	}
    return (static_cast <CC32RootServer*> (iRootServer))->AdjustMBufSizeAllocInfo(params);
    }


TInt E32Main()
/** Life starts here. ONly purpose is to run RunRootServer() from c32rootsrv.dll
*/
    {
	__FLOG_DECLARATION_VARIABLE;
	__FLOG_STMT( _LIT8( KRSLogSubSys, "RootServer" ) );
	__FLOG_STMT( _LIT8( KRSLogComp, "event" ) );
	__FLOG_OPEN( KRSLogSubSys, KRSLogComp );  
	__FLOG( _L8("C32exe.exe - E32Main calling RunRootServer in DLL") );
	__FLOG_CLOSE;
 
 	TRootServerStartupInfo info;
 	info.iPriority = CActive::EPriorityStandard;
 	info.iServerName = KRootServerName;
 	info.iRootServerCtor = CC32RootServer::NewL;
 	info.iDeathKey = KUidC32RootModuleDeathKey;
	info.iLeakKey.iUid = KUidCommsModuleLeakCounter;
	info.iProcessKey = KUidCommsProcess;
	return RunRootServer(info); // Load Root Server. Blocks by starting the active scheduler
    }

