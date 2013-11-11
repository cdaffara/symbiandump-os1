/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES

#include <e32base.h>
#include <e32svr.h>
#include <ecom/ecom.h>
#include <connect/sbdefs.h>

//#include <LbsIpc.h>
//#include <StartPosServer.h>
#include <lbsipc.h>
#include "lbslocservermessageenums.h"
#include "EPos_CPosResourceReader.h"
#include <eposserver.rsg>
#include <lbs/epos_mposmodulesobserver.h>

#include "epos_cposmodulessettings.h"
#include "EPos_ServerPanic.h"
#include "EPos_CPosServerDelayedShutdown.h"
#include "EPos_CPosModulesStatus.h"

#include "testEPos_CPosSession.h"
// Test version of the location server components to use the test location
// monitor api instead of the actual location monitor api
#include "testEPos_CPosServer.h"
#include "testEPos_CPosLocMonitorReqHandlerHub.h"

#include "EPos_Global.h"

// CONSTANTS
_LIT(KPosResourceFile, "\\private\\102869E5\\testlocserver.rsc");

#ifdef _DEBUG
_LIT(KTraceFileName, "testEPos_CPosServer.cpp");
#endif

// ================= SERVER'S POLICY =======================

//Definition of the ranges of IPC numbers
const TInt eposServerPolicyRanges[] = 
    {
	0,
	ELbsServerCancelAsyncRequest,
	ELbsGetDefaultModuleId,
	ELbsServerLastMessageId,
	ELbsPositionerCancelAsyncRequest,
	ELbsPositionerOpen,
	ELbsPositionerClose,
	ELbsSetSingleRequestor,
	ELbsPositionerLastMessageId,
	ELbsPositionCustomCodesFirst
    }; 
    
// Total number of ranges
const TUint eposServerPolicyRangeCount = 
    sizeof(eposServerPolicyRanges) / sizeof(TInt);

// Types of Policies
enum TPolicies
    {
    EPolicyLocationNeeded = 0
    };

// Specific capability checks
const CPolicyServer::TPolicyElement eposServerPolicyElements[] = 
    {
    // policy EPolicyLocationNeeded - fail call if Location not present
    {_INIT_SECURITY_POLICY_C1(ECapabilityLocation), CPolicyServer::EFailClient}
    };

//Policy to implement for each of the above ranges        
const TUint8 eposServerPolicyElementsIndex[eposServerPolicyRangeCount] = 
    {
    CPolicyServer::ENotSupported,
    CPolicyServer::EAlwaysPass,	            // server cancel
    EPolicyLocationNeeded,                  // GetDefModId -> NotifyModStatusEv
    CPolicyServer::ENotSupported,           // reserved server's codes
    CPolicyServer::EAlwaysPass,	            // positioner cancel
    CPolicyServer::EAlwaysPass,	            // open methods
    CPolicyServer::EAlwaysPass,	            // close positioner
    EPolicyLocationNeeded,                  // SetSingReq -> NotifyPosUpdate
    CPolicyServer::ENotSupported,           // reserved positioner's codes
    CPolicyServer::ECustomCheck             // custom requests
    };

//Package all the above together into a policy
const CPolicyServer::TPolicy eposServerPolicy =
    {
    CPolicyServer::EAlwaysPass,             // onConnect
    eposServerPolicyRangeCount,	            // number of ranges                                   
    eposServerPolicyRanges,	                // ranges array
    eposServerPolicyElementsIndex,	        // elements<->ranges index
    eposServerPolicyElements,		        // array of elements
    };
        
// ================= MEMBER FUNCTIONS =======================

/**
 * C++ constructor.
 *
 * @param aPriority Priority at which to run the server active object.
 * @param aPolicy Reference to a policy object describing the security 
 *                checks required for each message type.
 * @param aServerType Sharable or non-sharable sessions.
 */
CPosServer::CPosServer(TInt aPriority, const TPolicy &aPolicy, TServerType aServerType)
        : CPolicyServer(aPriority, aPolicy, aServerType)
    {
    }

/**
 * Symbian default constructor.
 */
void CPosServer::ConstructL() 
    {
    
    // Backup listener
    DEBUG_TRACE("Checking for backup or restore...", __LINE__)
    iBackupListener = CPosBackupListener::NewL();
    if (iBackupListener->IsBackupOperationOngoing())
        {
        User::Leave(KErrNotReady);
        }
    iBackupListener->Start(*this);

    //DEBUG_TRACE("Reading last known position...", __LINE__)
    // Settings and last position handler
    CPosResourceReader* resources = CPosResourceReader::NewLC(KPosResourceFile);
    iServerShutdownDelay = resources->ReadInt32L(R_POS_SERVER_SHUTDOWN_DELAY);
    
//    HBufC* lastKnownPositionFileName = resources->ReadHBufCL(R_LAST_KNOWN_POSITION_FILENAME);
//    CleanupStack::PushL(lastKnownPositionFileName);
    
    TInt dumpInterval = resources->ReadInt32L(R_POS_SERVER_DUMP_INTERVAL);

    iLocMonitorReqHandlerHub = CPosLocMonitorReqHandlerHub::NewL(); 

    
    CleanupStack::PopAndDestroy(resources);

    // Shutdown timer
    iShutdown = CPosServerDelayedShutdown::NewL();

    
    DEBUG_TRACE("Opening Location Settings...", __LINE__)
    // Location Settings
    iModuleSettings = CPosModuleSettings::NewL();
    iModulesStatus = CPosModulesStatus::NewL(iModuleSettings->PosModules());
    DEBUG_TRACE("Setting Location Settings observer...", __LINE__)
    iModuleSettings->AddListenerL(*this);

    DEBUG_TRACE("Starting server active object...", __LINE__)
    StartL(KPositionServerName);
    
   
    }

/**
 * Two-phased constructor.
 */
CPosServer* CPosServer::NewL()
    {
    CPosServer* self = new (ELeave) CPosServer(EPriority, eposServerPolicy);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Destructor.
 */
CPosServer::~CPosServer()
    {
    
    delete iLocMonitorReqHandlerHub;
    
    delete iShutdown;
    delete iModulesStatus;
    delete iModuleSettings;
    delete iBackupListener;
   

    // This is needed because CPositioner might have used ECom (PSYs)
    REComSession::FinalClose();
    }

/**
 * From MPosModuleSettingsListener.
 *
 * Called when a change has been detected in the modules database.
 * @param aEvent Event information
 */
void CPosServer::HandleModuleSettingsChangedL(TPosModulesEvent aEvent)
    {
    // Notify iModuleStatus
    iModulesStatus->HandleSettingsChangeL(aEvent);

    // For all sessions, call HandleSettingsChangeL()
    iSessionIter.SetToFirst();
    CPosSession* session = static_cast <CPosSession*> (iSessionIter++);
    while (session)
        {
        session->HandleSettingsChangeL(aEvent);
        session = static_cast <CPosSession*> (iSessionIter++);
        }
    }

/** 
 * From MPosBackupEventCallback
 *
 * Called by iBackupListener when a backup or restore 
 * operation either starts. Shuts down the server.
 */
void CPosServer::HandleBackupOperationEventL()
    {
    
    DEBUG_TRACE("HandleBackupOperationEventL", __LINE__)
    conn::TBURPartType state = conn::EBURUnset;
    conn::TBackupIncType type = conn::ENoBackup;
    iBackupListener->GetCurrentValue(state, type);

    // At the moment it just shuts down if backup/restore is in progress
    if ((state != conn::EBURUnset) && (state != conn::EBURNormal))
        {
        iSessionIter.SetToFirst();
        CPosSession* session = static_cast <CPosSession*> (iSessionIter++);
        while (session)
            {
            session->NotifyServerShutdown();
            session = static_cast <CPosSession*> (iSessionIter++);
            }

        // Shutdown the server by shutting down the active scheduler.
        CActiveScheduler::Stop();
        }
    else
        {
        iBackupListener->Start(*this); // continue listening
        }
    }

/**
 * From CServer2
 */
CSession2* CPosServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
    {
    // Check we're the right version
    TVersion version(KPosMajorVersionNumber,
                     KPosMinorVersionNumber,
                     KPosBuildVersionNumber);
    if (!User::QueryVersionSupported(version, aVersion))
        {
        User::Leave(KErrNotSupported);
        }

    // Make new session
    CPosSession* newSession = 
        CPosSession::NewL(
            *(const_cast<CPosServer*>(this)), 
            *iModuleSettings,
            *iModulesStatus, 
            *iLocMonitorReqHandlerHub);

    return newSession;
    }

/**
 * From CPolicyServer
 */
CPolicyServer::TCustomResult CPosServer::CustomSecurityCheckL(const RMessage2& /*aMsg*/, TInt& /*aAction*/,TSecurityInfo& /*aMissing*/)
    {
    DEBUG_TRACE("CustomSecurityCheck", __LINE__)
    return CPolicyServer::EFail;
    }

/**
 * Increment number of sessions
 */
void CPosServer::IncrementSessions()
    {
    DEBUG_TRACE("IncrementSessions", __LINE__)

    iNumSessions++;
    iShutdown->Cancel();
    }

/**
 * Decrement number of sessions
 */
void CPosServer::DecrementSessions()
    {    
    DEBUG_TRACE("DecrementSessions", __LINE__)

    // This panic handles an internal error.
    __ASSERT_DEBUG(iNumSessions > 0,
        DebugPanic(EPosServerPanicSessionsCountInconsistency));

    if (--iNumSessions == 0)
        {
        iShutdown->Start(iServerShutdownDelay);
        }
    }

//  End of File

