/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Mediator logging services inline implementation.
*                This file contains also empty implementations for release build.
*
*/


#ifdef _DEBUG

// -----------------------------------------------------------------------------
// RMediatorDebug::Initialize
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void RMediatorDebug::Initialize(CMediatorServer* aMediator)
    {
    if (iInstance == NULL)
        {
        TRAPD(err, iInstance = NewL(aMediator));
        
        if (err != KErrNone)
            {
            HandleInitError( err );
            }
        }
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::Uninitialize
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
inline void RMediatorDebug::Uninitialize()
    {
    delete iInstance;
    iInstance = NULL;
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::Log
// Prints log about operation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void RMediatorDebug::Log(const RMessage2& aMessage, CMediatorServerSession& aSession)
    {
    if ( iInstance == NULL )
        return;

    TRAPD(err, iInstance->PrintOperationL(aMessage, aSession));
    iInstance->HandleInternalError(err);
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::LogStatus
// Prints log for current status of mediator
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void RMediatorDebug::LogStatus()
    {
    if ( iInstance == NULL )
        return;
    
    TRAPD(err, iInstance->PrintServerStatusL());
    iInstance->HandleInternalError(err);
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::LogError
// Prints log for a operation error
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void RMediatorDebug::LogError(const RMessage2& aMessage, TInt aError)
    {
    if ( iInstance == NULL )
        return;
    
    TRAPD(err, iInstance->PrintErrorL(aMessage, aError));
    iInstance->HandleInternalError(err);
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::LogPlugin
// Prints information about plugin implementation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void RMediatorDebug::LogPlugin(const CImplementationInformation* aImpl)
    {
    if ( iInstance == NULL )
        return;
    
    TRAPD(err, iInstance->PrintPluginL(aImpl));
    iInstance->HandleInternalError(err);
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::LogData
// Prints event information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void RMediatorDebug::LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const TEvent& aEvent)
    {
     if ( iInstance == NULL )
        return;
    
    TRAPD(err, iInstance->PrintEventL(aMessage, aSession, aEvent, 0));
    iInstance->HandleInternalError(err);
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::LogData
// Prints event list information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
inline void RMediatorDebug::LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const REventList& aEventList)
    {
     if ( iInstance == NULL )
        return;
    
    TRAPD(err, iInstance->PrintEventListL(aMessage, aSession, aEventList));
    iInstance->HandleInternalError(err);
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::LogData
// Prints command information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void RMediatorDebug::LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const TCommand& aCommand)
    {
     if ( iInstance == NULL )
        return;
    
    TRAPD(err, iInstance->PrintCommandL(aMessage, aSession, aCommand, 0));
    iInstance->HandleInternalError(err);
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::LogData
// Prints command list information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void RMediatorDebug::LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const RCommandList& aCommandList)
    {
    if ( iInstance == NULL )
        return;
    
    TRAPD(err, iInstance->PrintCommandListL(aMessage, aSession, aCommandList));
    iInstance->HandleInternalError(err);
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::LogData
// Prints UID list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
inline void RMediatorDebug::LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const RCategoryList& aCategoryList)
    {
    if ( iInstance == NULL )
        return;
    
    TRAPD(err, iInstance->PrintCategoryListL(aMessage, aSession, aCategoryList));
    iInstance->HandleInternalError(err);
    }

// -----------------------------------------------------------------------------
// RMediatorDebug::LogData
// Prints raw data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void RMediatorDebug::LogData(const RMessage2& aMessage, CMediatorServerSession& aSession, const TDesC8& aData)
    {
    if ( iInstance == NULL )
        return;
    
    TRAPD(err, iInstance->PrintRawL(aMessage, aSession, aData));
    iInstance->HandleInternalError(err);
    }

#else // _DEBUG

inline RMediatorDebug::~RMediatorDebug() {}

inline void RMediatorDebug::Initialize(CMediatorServer* /*aMediator*/) {}

inline void RMediatorDebug::Uninitialize() {}

inline void RMediatorDebug::Log(const RMessage2& /*aMessage*/, CMediatorServerSession& /*aSession*/) {}
    
inline void RMediatorDebug::LogData(const RMessage2& /*aMessage*/, CMediatorServerSession& /*aSession*/, const MediatorService::REventList& /*aEventList*/) {}

inline void RMediatorDebug::LogData(const RMessage2& /*aMessage*/, CMediatorServerSession& /*aSession*/, const MediatorService::TEvent& /*aEvent*/) {}

inline void RMediatorDebug::LogData(const RMessage2& /*aMessage*/, CMediatorServerSession& /*aSession*/, const MediatorService::RCommandList& /*aCommandList*/) {}

inline void RMediatorDebug::LogData(const RMessage2& /*aMessage*/, CMediatorServerSession& /*aSession*/, const MediatorService::TCommand& /*aCommand*/) {}

inline void RMediatorDebug::LogData(const RMessage2& /*aMessage*/, CMediatorServerSession& /*aSession*/, const MediatorService::RCategoryList& /*aCategoryList*/) {}

inline void RMediatorDebug::LogData(const RMessage2& /*aMessage*/, CMediatorServerSession& /*aSession*/, const TDesC8& /*aData*/) {}

inline void RMediatorDebug::LogStatus() {}

inline void RMediatorDebug::LogError(const RMessage2& /*aMessage*/, TInt /*aError*/) {}

inline void RMediatorDebug::LogPlugin(const CImplementationInformation* /*aImpl*/) {}

#endif // _DEBUG