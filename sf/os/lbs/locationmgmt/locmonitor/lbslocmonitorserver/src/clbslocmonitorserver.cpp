/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "clbslocmonitorserver.h"
#include "lbslocmonitorserverdata.h"
#include "lbsdevloggermacros.h"
#ifdef SYMBIAN_FEATURE_MANAGER
    #include <featdiscovery.h>
    #include <featureuids.h>
#endif

// -------------------------------------------------------------------------------
// --------------------  Server's security policy  -------------------------------
// -------------------------------------------------------------------------------


const TInt KShutDownDelay = 10000000; // 10 seconds

// Definition of the ranges of IPC numbers
const TInt locMonitorServerPolicyRanges[] = 
    {
	0,								// subsessionbase open and close (from ServerFramework)	
	ECancelDbWipeOut,				// wipe out db and cancel wipe out
	ECancelGetLastKnownPositionArea,// All PositionArea subsession messages
	EAreaPositionerLastMessageId,	// Not a message...not supported
    }; 
    
// Total number of ranges
const TUint locMonitorServerPolicyRangeCount = 
    sizeof(locMonitorServerPolicyRanges) / sizeof(TInt);

// Types of Policies
enum TPolicies
    {
    EPolicyLocationNeeded = 0
    };

// Specific capability checks
const CPolicyServer::TPolicyElement locMonitorServerPolicyElements[] = 
    {
     //policy EPolicyLocationNeeded - fail call if Location not present
	 {_INIT_SECURITY_POLICY_C1(ECapabilityLocation), CPolicyServer::EFailClient}
    };
    
//Policy to implement for each of the above ranges        
const TUint8 locMonitorServerPolicyElementsIndex[locMonitorServerPolicyRangeCount] = 
  {
    CPolicyServer::EAlwaysPass,  // subsessionbase open and close
    CPolicyServer::EAlwaysPass,  // wipe out db and cancel wipe out (could be EPolicyLocationNeeded,)
    CPolicyServer::EAlwaysPass,	 // Position Area subsession messages (could be EPolicyLocationNeeded)
    CPolicyServer::ENotSupported // Invalid values 
    };

//Package all the above together into a policy
const CPolicyServer::TPolicy locMonitorServerPolicy =
    {
    CPolicyServer::ECustomCheck,            
    locMonitorServerPolicyRangeCount,	    // number of ranges                                   
    locMonitorServerPolicyRanges,	        // ranges array
    locMonitorServerPolicyElementsIndex,	// elements<->ranges index
    locMonitorServerPolicyElements,		    // array of elements
    };


CPolicyServer::TCustomResult CLbsLocMonitorServer::CustomSecurityCheckL(const RMessage2& /*aMsg*/, TInt& /*aAction*/,TSecurityInfo& /*aMissing*/)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorServer::CustomSecurityCheckL");

	CPolicyServer::TCustomResult result = CPolicyServer::EPass;

    return result;
    }

// -------------------------------------------------------------------------------
// -------------  Methods required by the Server Framework  ----------------------
// -------------  for server startup                        ----------------------
// -------------------------------------------------------------------------------

MCreateServerImpl* CSecureASBase::GetImplLC()
	{
	LBSLOG(ELogP1,"CSecureASBase::GetImplLC()");
	CLbsLocMonitorCreator* impl = new(ELeave) CLbsLocMonitorCreator();
	CleanupStack::PushL(impl);
	return impl;
	}

void CLbsLocMonitorCreator::CreateServerLC(TServerStartParams& aParams)
	{
	LBSLOG(ELogP1,"CLbsLocMonitorCreator::CreateServerLC");
	CLbsLocMonitorServer* s = new(ELeave) CLbsLocMonitorServer(CActive::EPriorityStandard, locMonitorServerPolicy);
	CleanupStack::PushL(s);
	s->ConstructL(aParams.GetServerName());
	}

// -------------------------------------------------------------------------------
// -------------  Location Monitor Server implementation   -----------------------
// -------------------------------------------------------------------------------

CLbsLocMonitorServer::CLbsLocMonitorServer(TInt aPriority, const CPolicyServer::TPolicy& aSecurityPolicy) : 
										   CSecureServerBase(aPriority, aSecurityPolicy),
										   iVersion(KLbsLocMonitorMajorVersionNumber,
										   KLbsLocMonitorMinorVersionNumber,
										   KLbsLocMonitorBuildVersionNumber)
	{
	LBSLOG(ELogP1,"CLbsLocMonitorServer::CLbsLocMonitorServer");
	}
		
void CLbsLocMonitorServer::ConstructL(const TDesC& aServerName)
	{
	LBSLOG(ELogP1,"CLbsLocMonitorServer::ConstructL");
	StartL(aServerName);

	// Server Framework requires the method BaseConstructL(TBool)
	// to be called.
	// Passing EFalse means the Location Monitor would be a
	// permanent process only terminated from root.
	//
	
    #ifdef SYMBIAN_FEATURE_MANAGER
        TBool locationManagementSupported = CFeatureDiscovery::IsFeatureSupportedL(NFeature::KLocationManagement);
    #else
        TBool locationManagementSupported(ETrue);
    #endif
        if(locationManagementSupported)
            {
            BaseConstructL(EFalse);	
            }
        else
            {
            BaseConstructL(ETrue);
            //Set timer to two seconds
            SetShutdownDelay(KShutDownDelay);
            }

    // Create the monitor which detects a closedown signal from
    // the LBS Root Process.
    iCloseDownRequestDetector = CLbsCloseDownRequestDetector::NewL(
    								this, 
									KLbsLocMonitorUid);

	// Object for writing/reading to/from the DB
	iRequestHandler = CLbsLocMonitorRequestHandler::NewL();

	// Instantiate area info finders (just one for now, a network info finder)
	// and set DB reader and writer as its observers.	
	iAreaInfoFinders.ReserveL(1);
	
	iAreaInfoFinders.InsertL(static_cast<CLbsLocMonitorAreaInfoFinder*>(CLbsLocMonitorNetworkInfoFinder::NewL()), ENetworkInfoFinder);
	iAreaInfoFinders[ENetworkInfoFinder]->RegisterObserverL(*iRequestHandler);

	// Instantiate a position listener that will feed positions to the DB writer
	iPosListener = CLbsLocMonitorPosListener::NewL(*iRequestHandler);
	
	// Initiate monitoring of network information
	static_cast<CLbsLocMonitorNetworkInfoFinder*>(iAreaInfoFinders[ENetworkInfoFinder])->StartGettingNetworkInfoL();
	
	// Initiate plugin resolver.
	iPluginResolver = CLbsLocMonitorPluginResolver::NewL();
	}

CLbsLocMonitorServer::~CLbsLocMonitorServer()
	{
	LBSLOG(ELogP1,"CLbsLocMonitorServer::~CLbsLocMonitorServer");
	
	// inform observers (subsessions) that server destructed
	for(TInt i=0; i < iDestructionObservers.Count(); i++)
	    {
        iDestructionObservers[i]->LocMonServerDestructed();
	    }
	
	iDestructionObservers.Reset();
	delete iRequestHandler;
	delete iPosListener;
	iAreaInfoFinders.ResetAndDestroy();
	delete iCloseDownRequestDetector;
	iConversionHandlerArray.ResetAndDestroy();
	delete iPluginResolver;
	}

CSession2* CLbsLocMonitorServer::DoNewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{
	LBSLOG(ELogP1,"CLbsLocMonitorServer::DoNewSessionL");
	return new(ELeave) CLbsLocMonitorSession(); // version number already checked at this point
	}
	
TVersion CLbsLocMonitorServer::GetServerVersion() const
	{
	LBSLOG(ELogP1,"CLbsLocMonitorServer::GetServerVersion()");
	return iVersion;
	}

CLbsLocMonitorRequestHandler& CLbsLocMonitorServer::ReadRequestHandler() const
	{
	LBSLOG(ELogP1,"CLbsLocMonitorServer::ReadRequestHandler()");
	return (*iRequestHandler);
	}

CLbsLocMonitorConversionHandler* CLbsLocMonitorServer::ConversionHandlerL( 
                                              TUid aConversionPluginId )
    {
    LBSLOG(ELogP1,"CLbsLocMonitorServer::ConversionHandlerL()");
    
    // Iterate through the conversion handler array and check whether 
    // there is already a handler that has loaded user specified 
    // plugin
    for( TInt i=0;i<iConversionHandlerArray.Count();i++ )
        {
        if( iConversionHandlerArray[i]->ConversionPluginUid() == 
            aConversionPluginId )
            {
            return iConversionHandlerArray[i];
            }
        }
    
    CLbsLocMonitorServer* monitorServer = const_cast<CLbsLocMonitorServer*>
                                                  ( this );
    CLbsLocMonitorConversionHandler* conversionHandler = 
                                    CLbsLocMonitorConversionHandler::NewL( *monitorServer,
                                                                            aConversionPluginId );
    
    iConversionHandlerArray.AppendL( conversionHandler );
    return conversionHandler;
    }

CLbsLocMonitorConversionHandler* CLbsLocMonitorServer::ConversionHandler( 
                                                              const RMessage2& aMessage )
    {
    LBSLOG(ELogP1,"CLbsLocMonitorServer::ConversionHandler()");
    
    // Iterate through the conversion handler array and check whether 
    // there is already a handler that has loaded user specified 
    // plugin
    for( TInt i=0;i<iConversionHandlerArray.Count();i++ )
        {
        if( iConversionHandlerArray[i]->IsMessagePresent( aMessage ) )
            {
            return iConversionHandlerArray[i];
            }
        }
    return NULL;
    }

CLbsLocMonitorPluginResolver* CLbsLocMonitorServer::PluginResolver()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorServer::PluginResolver()");
    return iPluginResolver;
    }

/* Intended for use by subsessions
   Called to register as an observer
 
 */
void CLbsLocMonitorServer::SetLocMonServerDestructObserverL(const MLocMonServerDestructObserver& aDestructObserver)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorServer::SetLocMonServerDestructObserver()");
    iDestructionObservers.AppendL(&aDestructObserver);
    }

/* Intended for use by subsessions
   Called to deregister as an observer
 */
void CLbsLocMonitorServer::UnsetLocMonServerDestructObserver(const MLocMonServerDestructObserver& aDestructObserver)
    {
    LBSLOG(ELogP1,"CLbsLocMonitorServer::UnsetLocMonServerDestructObserver()");
    
    TInt i = iDestructionObservers.Find(&aDestructObserver);
    if(KErrNotFound != i)
        {
        iDestructionObservers.Remove(i);
        }
    }

/* Called when LbsRoot has requested that the Location Monitor should 
   closedown.
*/
void CLbsLocMonitorServer::OnProcessCloseDown()
    {
    LBSLOG(ELogP1,"CLbsLocMonitorServer::OnProcessCloseDown()");
    CActiveScheduler::Stop();
    }
 
/**
 * This method is called by CLbsLocMonitorConversionHandler after it completes 
 * all the requests
 */
void CLbsLocMonitorServer::HandleConversionComplete( CLbsLocMonitorConversionHandler*
                                                     aConversionHandler )
    {
    LBSLOG(ELogP1,"CLbsLocMonitorServer::HandleConversionComplete()");
    // Remove the conversion handler object from the array and delete it.
    TInt index = iConversionHandlerArray.Find( aConversionHandler );
    if( index != KErrNotFound )
        {
        iConversionHandlerArray.Remove(index);
        }
    delete aConversionHandler;
    }
    
TBool CLbsLocMonitorServer::FindRootProcess()
	{
	_LIT(KLbsRootProcessName, "lbsroot.exe*");
	TInt err(KErrNotFound);
	TFullName fullName;			
	TFindProcess processFinder(KLbsRootProcessName);
	while (err = processFinder.Next(fullName), err == KErrNone)
		{
		RProcess process;
		TInt processOpenErr = process.Open(processFinder);
		if (processOpenErr == KErrNone)
			{
			TExitType exitType = process.ExitType();
			if (exitType == EExitPending)
				{
				// Found a running instance of lbsroot.exe,
				return ETrue;
				}
			}
		process.Close();		
		}
		
	return EFalse;
	}
