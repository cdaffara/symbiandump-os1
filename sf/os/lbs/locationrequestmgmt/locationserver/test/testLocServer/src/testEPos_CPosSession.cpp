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
#include <lbs.h>
#include "lbslocservermessageenums.h"
#include <lbs/epos_mposmodulesobserver.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbs/epos_cpositioner.h>
#include "EPos_PositionerConstructParams.h"
#include "lbsdevloggermacros.h"
#include "eposuid.hrh"

#ifdef _DEBUG
#include <lbs/epos_cpostrace.h>
#endif

#include "testEPos_CPosSession.h"
#include "testEPos_CPosSubSession.h"
#include "testEPos_CPosServer.h"
#include "testEPos_CPosSubsessionRegistry.h"

#include "EPos_Global.h"
#include "EPos_CPosModulesStatus.h"
#include "epos_cposmodulessettings.h"


// CONSTANTS
#ifdef _DEBUG
_LIT(KTraceFileName, "testEPos_CPosSession.cpp");
#endif

const TPositionModuleId KPosDefaultModule = { KPosDefaultPsyImplUid };
const TPositionModuleId KPosStrategyModule = { KPosStrategyPsyImplUid };

const TInt KParamCriteria = 0;
const TInt KParamNumModules = 0;
const TInt KParamModuleIdOpen = 0;
const TInt KParamModuleIdGetInfo = 0;
const TInt KParamModuleIdGetStatus = 0;
const TInt KParamModuleIdGetDefault = 0;
const TInt KParamModuleInfo = 1;
const TInt KParamModuleStatusGet = 1;
const TInt KParamSubsession = 3;

// ================= MEMBER FUNCTIONS =======================

/**
 * C++ constructor.
 */
CPosSession::CPosSession(CPosServer& aServer,
                         CPosModuleSettings& aModuleSettings,
                         CPosModulesStatus& aModulesStatus,
                         CPosLocMonitorReqHandlerHub& aLocMonitorReqHandlerHub)
    : iPosServer(aServer),
      iModuleSettings(aModuleSettings),
      iModulesStatus(aModulesStatus),
      iLocMonitorReqHandlerHub(aLocMonitorReqHandlerHub)
    {      
    }

/**
 * C++ destructor.
 */
CPosSession::~CPosSession()
    {

    delete iSubSessionRegistry;
    
    // The flag iServerShutdown prevents calling the server or its members if
    // it is already destroyed.
    if (!iServerShutdown)
        {
        iModulesStatus.NotifySessionClosed(this); 
        iLocMonitorReqHandlerHub.NotifySessionClosed( const_cast<const CSession2*>( static_cast<CSession2*> (this) ) ); //TODO ????????????????????????????????????????????
        
        if (iDecrementSessions)
            {
            iPosServer.DecrementSessions();
            }
        }
    }

/**
 * Two-phased constructor.
 *
 * @param aServer Pointer to the server Active Object
 * @param aModules Modules settings.
 * @param aModulesStatus Pointer to the ModulesStatus object.
 * @param aLastPositionHandler Last Position Handler.
 * @return Pointer to a new instance of CPosSession
 */
CPosSession* CPosSession::NewL(CPosServer& aServer,
                               CPosModuleSettings& aModuleSettings,
                               CPosModulesStatus& aModulesStatus,
                               CPosLocMonitorReqHandlerHub& aLocMonitorReqHandlerHub)
    {
    CPosSession* self = new (ELeave)
        CPosSession(
            aServer, 
            aModuleSettings, 
            aModulesStatus, 
            aLocMonitorReqHandlerHub);
            
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Symbian destructor.
 */
void CPosSession::ConstructL()
    {
    iSubSessionRegistry = CPosSubsessionRegistry::NewL();

    // This call will also cancel any server shutdown in progress.
    iPosServer.IncrementSessions();
    iDecrementSessions = ETrue;

    }

/**
 * From CSession2
 *
 * This function services all requests from clients.
 */
void CPosSession::ServiceL(const RMessage2& aMessage)
    {
    switch (aMessage.Function())
        {
        case ELbsPositionerOpenModuleId:
		    OpenFromModuleIdL(aMessage);
		    break;
	    case ELbsPositionerOpen:
		    OpenDefaultPositionerL(aMessage);
		    break;
	    case ELbsPositionerOpenCriteria:
		    OpenPositionerFromCriteriaL(aMessage);
		    break;
	    case ELbsPositionerClose:
		    ClosePositioner(aMessage);
		    break;
	    case ELbsGetDefaultModuleId:
		    GetDefaultModuleIdL(aMessage);
		    break;
	    case ELbsGetNumModules:
		    GetNumModulesL(aMessage);
		    break;
	    case ELbsGetModuleInfoByIndex:
		    GetModuleInfoByIndexL(aMessage);
		    break;
	    case ELbsGetModuleInfoById:
		    GetModuleInfoByIdL(aMessage);
		    break;
	    case ELbsGetModuleStatus:
		    GetModuleStatusL(aMessage);
		    break;
	    case ELbsNotifyModuleStatusEvent:
		    NotifyModuleStatusEventL(aMessage);
		    break;
	    case ELbsEmptyLastKnownPositionStore:
	    	EmptyLastKnownPositionStore(aMessage);
	    	break;
	    case ELbsServerCancelAsyncRequest:
			LBS_RDEBUG("Client", "LBS", "CancelAsyncRequest");
            HandleCancelAsyncRequestL(aMessage);
		    break;
        default:
            ForwardToSubSessionL(aMessage);
		    break;
        }
    }

/**
 * From CSession2
 *
 * This function is called when ServiceL leaves.
 */
void CPosSession::ServiceError(const RMessage2& aMessage, TInt aError)
    {
    DEBUG_TRACE("Client's request failed with error", aError)
    CSession2::ServiceError(aMessage, aError);
    }

/**
 * Called when a change has been detected in the location settings. 
 * @param aEvent Event information
 */
void CPosSession::HandleSettingsChangeL(TPosModulesEvent aEvent)
    {
    // For all subSessions, call HandleSettingsChangeL()
    TInt nrOfSubSessions = iSubSessionRegistry->Count();
    for (TInt i = 0; i < nrOfSubSessions; i++)
        {
        CPosSubSession* subSession = 
            static_cast <CPosSubSession*> 
            (iSubSessionRegistry->SubSessionFromIndex(i));
		if (subSession)
			{
			subSession->HandleSettingsChangeL(aEvent);
			}
        }
    }

/**
 * Called when the server class is shutting down.
 */
void CPosSession::NotifyServerShutdown()
    {
    DEBUG_TRACE("NotifyServerShutdown", __LINE__)

    iServerShutdown = ETrue;
    TInt nrOfSubSessions = iSubSessionRegistry->Count();
    
    iLocMonitorReqHandlerHub.NotifyServerShutDown(); //TODO ????????????????????????????????????????????
    
    for (TInt i = 0; i < nrOfSubSessions; i++)
        {
        CPosSubSession* subSession = 
            static_cast <CPosSubSession*> 
            (iSubSessionRegistry->SubSessionFromIndex(i));
        if (subSession)
			{
			subSession->NotifyServerShutdown();
			}
        }
    }

void CPosSession::OpenFromModuleIdL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionerOpenModuleId", __LINE__)

	TPckgBuf<TPositionModuleId> moduleIdBuf;
    User::LeaveIfError(Global::Read(aMessage, KParamModuleIdOpen, moduleIdBuf));
    CreateSubSessionL(moduleIdBuf(), EFalse, aMessage);
    RequestComplete(aMessage, KErrNone);
    }

void CPosSession::OpenDefaultPositionerL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionerOpen(Default)", __LINE__)
    CreateSubSessionL(KPosDefaultModule, ETrue, aMessage);
	RequestComplete(aMessage, KErrNone);
    }

void CPosSession::OpenPositionerFromCriteriaL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionerOpenCriteria", __LINE__)

    // Read Criteria
	TPckgBuf<TPositionCriteria> criteria;
    User::LeaveIfError(Global::Read(aMessage, KParamCriteria, criteria));
    CreateSubSessionL(KPosStrategyModule, ETrue, aMessage);
    RequestComplete(aMessage, KErrNone);
    }

void CPosSession::ClosePositioner(const RMessage2& aMessage)
    {
    
    iLocMonitorReqHandlerHub.NotifySubSessionClosed(aMessage);
    
    TInt handle = aMessage.Int3();
    iSubSessionRegistry->CloseSubSession(handle);

	RequestComplete(aMessage, KErrNone);

    DEBUG_TRACE("EPositionerClose", __LINE__)
    }

void CPosSession::GetDefaultModuleIdL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionServerGetDefaultModuleId", __LINE__)

    CPosModuleIdList* moduleList = iModuleSettings.ModuleIdListL();
    CleanupStack::PushL( moduleList );
    TInt nrOfModules = moduleList->Count();
    TInt res = KErrNotFound;

    for (TInt i = 0; (i < nrOfModules) && (res == KErrNotFound); i++)
        {
        TPositionModuleInfo candidate;
        iModuleSettings.GetModuleInfoL((*moduleList)[i], candidate);

        if (candidate.IsAvailable())
            {
            TPckg<TPositionModuleId> moduleId(candidate.ModuleId());
            User::LeaveIfError(Global::Write(aMessage, KParamModuleIdGetDefault, moduleId));
            res = KErrNone;
            }
        }

    CleanupStack::PopAndDestroy(moduleList);
    RequestComplete(aMessage, res);
    }   

void CPosSession::GetNumModulesL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionServerGetNumModules", __LINE__)

    CPosModuleIdList* moduleList = iModuleSettings.ModuleIdListL();
    TInt num = moduleList->Count();
    delete moduleList;

    TPckg<TUint> numPackage(num);
    User::LeaveIfError(Global::Write(aMessage, KParamNumModules, numPackage));
    RequestComplete(aMessage, KErrNone);
    }

void CPosSession::GetModuleInfoByIndexL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionServerGetModuleInfoByIndex in", __LINE__)

    // module index
    TInt modIndex = aMessage.Int0();

    CPosModuleIdList* moduleIdList = iModuleSettings.ModuleIdListL();
    CleanupStack::PushL( moduleIdList );
    if (modIndex >= moduleIdList->Count() || modIndex < 0)
        {
        User::Leave(KErrNotFound);
        }
    TPositionModuleId moduleId = (*moduleIdList)[modIndex];

    // module info
    HBufC8* buffer = Global::CopyClientBuffer8LC(aMessage, KParamModuleInfo);

    TPositionModuleInfoBase* moduleInfoBase =
        reinterpret_cast<TPositionModuleInfoBase*>(
        const_cast<TUint8*>(buffer->Ptr()));

    Global::ValidatePositionClassBufferL(*moduleInfoBase, buffer->Des());
    // TPositionModuleInfo is the only class supported by CPosModules
    Global::ValidatePositionClassTypeL(*moduleInfoBase, EPositionModuleInfoClass);
    DEBUG_TRACE("Buffer validation done", __LINE__)

    CleanupStack::PopAndDestroy(2, moduleIdList);

    // TPositionModuleInfo contains descriptor. If it is corrupt 
    // (malicious client) then writing to this will panic server
    // Local instance of TPositionModuleInfo is created to be sure
    // that module name descriptor is not corrupt

    TPositionModuleInfo modInfo;
    iModuleSettings.GetModuleInfoL(moduleId, modInfo);

    TPckg<TPositionModuleInfo> modInfoPack(modInfo);
    User::LeaveIfError(Global::Write(aMessage, KParamModuleInfo, modInfoPack));

    RequestComplete(aMessage, KErrNone);
    DEBUG_TRACE("EPositionServerGetModuleInfoByIndex out", __LINE__)
    }

void CPosSession::GetModuleInfoByIdL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionServerGetModuleInfoById in", __LINE__)

    TPckgBuf<TPositionModuleId> moduleIdBuf;
	User::LeaveIfError(Global::Read(aMessage, KParamModuleIdGetInfo, moduleIdBuf));
    TPositionModuleId moduleId = moduleIdBuf();

    HBufC8* buffer = Global::CopyClientBuffer8LC(aMessage, KParamModuleInfo);

    TPositionModuleInfoBase* moduleInfoBase = 
	    reinterpret_cast<TPositionModuleInfoBase*>(
        const_cast<TUint8*>(buffer->Ptr()));

    Global::ValidatePositionClassBufferL(*moduleInfoBase, buffer->Des());
    // TPositionModuleInfo is the only class supported by CPosModules
    Global::ValidatePositionClassTypeL(*moduleInfoBase, EPositionModuleInfoClass);
    DEBUG_TRACE("Buffer validation done", __LINE__)

    CleanupStack::PopAndDestroy(buffer);

    // TPositionModuleInfo contains descriptor. If it is corrupt 
    // (malicious client) then writing to this will panic server
    // Local instance of TPositionModuleInfo is created to be sure
    // that module name descriptor is not corrupt

    TPositionModuleInfo modInfo;
    iModuleSettings.GetModuleInfoL(moduleId, modInfo);

    TPckg<TPositionModuleInfo> modInfoPack(modInfo);
    User::LeaveIfError(Global::Write(aMessage, KParamModuleInfo, modInfoPack));

    RequestComplete(aMessage, KErrNone);
    DEBUG_TRACE("EPositionServerGetModuleInfoById out", __LINE__)
    }

void CPosSession::GetModuleStatusL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionServerGetModuleStatus in", __LINE__)

    TPckgBuf<TPositionModuleId> moduleIdBuf;
	User::LeaveIfError(Global::Read(aMessage, KParamModuleIdGetStatus, moduleIdBuf));
    TPositionModuleId moduleId = moduleIdBuf();

    HBufC8* buffer = Global::CopyClientBuffer8LC(aMessage, KParamModuleStatusGet);

    // Read module status and write back to client
    TPositionModuleStatusBase* moduleStatusBase = 
	    reinterpret_cast<TPositionModuleStatusBase*>(
        const_cast<TUint8*>(buffer->Ptr()));

    Global::ValidatePositionClassBufferL(*moduleStatusBase, buffer->Des());

    User::LeaveIfError ( 
        iModulesStatus.GetModuleStatus(moduleId, *moduleStatusBase) );
    User::LeaveIfError(Global::Write(aMessage, KParamModuleStatusGet, *buffer));

    CleanupStack::PopAndDestroy(buffer);
	RequestComplete(aMessage, KErrNone);
    DEBUG_TRACE("EPositionServerGetModuleStatus out", __LINE__)
    }

void CPosSession::NotifyModuleStatusEventL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionServerNotifyModuleStatusEvent", __LINE__)

    iModulesStatus.NotifyModuleStatusEventL(aMessage, this);
    }

void CPosSession::NotifyModuleStatusEventCancelL(const RMessage2& aMessage)
    {
    DEBUG_TRACE("EPositionServerNotifyModuleStatusEventCancel", __LINE__)

    iModulesStatus.CancelNotifyModuleStatusEventL(this);
    RequestComplete(aMessage, KErrNone);
    }

void CPosSession::HandleCancelAsyncRequestL(const RMessage2& aMessage)
    {
    switch (aMessage.Int0())
        {
        case KServerNotifyModuleStatusEventSymbian:
        case KServerNotifyModuleStatusEventVariant:
        case ELbsNotifyModuleStatusEvent:                 // TODO Verify
	        NotifyModuleStatusEventCancelL(aMessage);
            break;
        case ELbsEmptyLastKnownPositionStore:
        	EmptyLastKnownPositionStoreCancelL(aMessage); // TODO Verify
        
        default:
            RequestComplete(aMessage, KErrNotSupported);
            break;
        }
    }

void CPosSession::EmptyLastKnownPositionStore(const RMessage2& aMessage)
	{
	
	iLocMonitorReqHandlerHub.EmptyLastKnownPosStoreReqL(aMessage);
	
	}

void CPosSession::EmptyLastKnownPositionStoreCancelL(const RMessage2& aMessage)
	{
	
	iLocMonitorReqHandlerHub.CancelEmptyLastKnownPosStoreReqL(aMessage);
	
	}


void CPosSession::CreateSubSessionL(TPositionModuleId aId, TBool aIsProxy, const RMessage2& aMessage)
	{
    CPosSubSession* subSession = CPosSubSession::NewLC(
        iModuleSettings, 
        iLocMonitorReqHandlerHub,
        aId,
        aIsProxy,
        &iModulesStatus,
        &iModulesStatus);

    TInt subSessionHandle = iSubSessionRegistry->AddInstanceL(subSession);
    CleanupStack::Pop(subSession); // Now owned by registry
    
    // Set the client subsession identifier.
    TPckg<TInt> handlePackage(subSessionHandle);
    TInt err = Global::Write(aMessage, KParamSubsession, handlePackage);
    if (err)
        {
        iSubSessionRegistry->CloseSubSession(subSessionHandle);
        User::Leave(err);
        }
    DEBUG_TRACE("Subsession created successfully", __LINE__)
	}

void CPosSession::ForwardToSubSessionL(const RMessage2& aMessage)
    {
    CPosSubSession* subSession =
        iSubSessionRegistry->SubSessionFromHandleL(aMessage.Int3());

    if (!subSession)
		{
        User::Leave(KErrNotSupported);
        }

    subSession->ServiceL(aMessage); 
    }

void CPosSession::RequestComplete(const RMessage2& aMessage, TInt aCompleteCode)
    {
    if (!aMessage.IsNull())
        {
        aMessage.Complete(aCompleteCode);
        }
    }

//  End of File
