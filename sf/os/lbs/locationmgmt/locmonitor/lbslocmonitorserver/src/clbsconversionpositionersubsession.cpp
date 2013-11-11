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

#include "clbsconversionpositionersubsession.h"
#include "lbslocmonitorserverdata.h"
#include "clbslocmonitorutils.h"
#include "clbslocmonitorserver.h"
#include "lbsdevloggermacros.h"
#include "clbslocmonitorconversionhandler.h"
#include "lbslocmonitorclientconsts.h"

//----------------------------------------------------------------------------
// CLbsConversionPositionerSubsession::NewL
//
//-----------------------------------------------------------------------------
CLbsConversionPositionerSubsession* CLbsConversionPositionerSubsession::NewL()
    {
    LBSLOG(ELogP1,"CLbsConversionPositionerSubsession::NewL()");
    CLbsConversionPositionerSubsession* self = new (ELeave) CLbsConversionPositionerSubsession;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// -----------------------------------------------------------------------------
// CLbsConversionPositionerSubsession::~CLbsConversionPositionerSubsession
//------------------------------------------------------------------------------
CLbsConversionPositionerSubsession::CLbsConversionPositionerSubsession()
    {
    LBSLOG(ELogP1,"CLbsConversionPositionerSubsession::CLbsConversionPositionerSubsession()");
    }

// -----------------------------------------------------------------------------
// CLbsConversionPositionerSubsession::ConstructL
//------------------------------------------------------------------------------
void CLbsConversionPositionerSubsession::ConstructL()
    {
    LBSLOG(ELogP1,"CLbsConversionPositionerSubsession::ConstructL()");
    // Nothing to do
    }
//----------------------------------------------------------------------------
// CLbsConversionPositionerSubsession::~CLbsConversionPositionerSubsession
//
//-----------------------------------------------------------------------------
CLbsConversionPositionerSubsession::~CLbsConversionPositionerSubsession()
    {
    LBSLOG(ELogP1,"->CLbsConversionPositionerSubsession::~CLbsConversionPositionerSubsession");     

    // Tell the DB reader that this subsession is being deleted
    // so that if there are any outanding requests from this session
    // they are terminated with KErrServerTerminated.
    iLocMonitorServer->ReadRequestHandler().ClientTerminated(iSubsessionId);

    LBSLOG(ELogP1,"<-CLbsConversionPositionerSubsession::~CLbsConversionPositionerSubsession");
    }

//----------------------------------------------------------------------------
// CLbsConversionPositionerSubsession::DispatchL
//
//-----------------------------------------------------------------------------
void CLbsConversionPositionerSubsession::DispatchL(const RMessage2& aMessage)
    {
    LBSLOG(ELogP1,"CLbsConversionPositionerSubsession::DispatchL()");
    switch(aMessage.Function())
        {
        case EConvertSingleLocationInfoBufferSize:
        case EConvertMultipleLocationInfoBufferSize:
            {
            // Extract client preferences
            TLbsClientPrefs clientPrefs; 
            TPckg<TLbsClientPrefs> clientPrefsPckg(clientPrefs);
            aMessage.ReadL(KParamClientPrefs,clientPrefsPckg);
            
            // If client has not specified any conversion plugin id, read the default 
            // plugin from cenrep and load it.
            TUid conversionPluginId;
            if(clientPrefs.iConverterModuleId.iUid == KNullUidValue)
                {
                TInt error = iLocMonitorServer->PluginResolver()->DefaultConverterUid(
                                                                  conversionPluginId);
                if(error != KErrNone)
                    {
                    aMessage.Complete(error);
                    return;
                    }
                }
            else
                {
                conversionPluginId = clientPrefs.iConverterModuleId;
                }
            
            CLbsLocMonitorConversionHandler* conversionHandler = 
                                    iLocMonitorServer->ConversionHandlerL(conversionPluginId);
            conversionHandler->GetConvertLocationInfoSizeL(aMessage);
            break;
            }
            
        case EGetSingleLocationInfo:
        case EGetMultipleLocationInfo:
            {
            CLbsLocMonitorConversionHandler* conversionHandler = 
                                                 iLocMonitorServer->ConversionHandler(aMessage);
            
            conversionHandler->GetConvertedLocationInfoL(aMessage);
            break;
            }
            
            
            
        case ECancelConvertLocationInfo:
            {
            CLbsLocMonitorConversionHandler* conversionHandler = 
                                                 iLocMonitorServer->ConversionHandler(aMessage);
            
            // If there is no outstanding request from the subsession which requested cancel operation,
            // complete the request.
            if(conversionHandler)
                {
                conversionHandler->CancelLocationInfoConversionL(aMessage);
                }
            else
                {
                aMessage.Complete(KErrNone);
                }
            break;
            }
        default:
            __ASSERT_DEBUG(EFalse, User::Invariant());
            break;
        }
    }


//----------------------------------------------------------------------------
// CLbsConversionPositionerSubsession::DispatchError
//
//-----------------------------------------------------------------------------
void CLbsConversionPositionerSubsession::DispatchError(const RMessage2& aMessage, 
                                                        TInt aError)
    {
    LBSLOG(ELogP1,"CLbsConversionPositionerSubsession::DispatchError()");
    // errors from DispatchL end up in here
    aMessage.Complete(aError);
    }


//----------------------------------------------------------------------------
// CLbsConversionPositionerSubsession::DispatchError
//
//-----------------------------------------------------------------------------
void CLbsConversionPositionerSubsession::CreateSubSessionL(const RMessage2& aMessage, 
                                                      const CSecureServerBase* aServer)
    {
    LBSLOG(ELogP1,"CLbsConversionPositionerSubsession::CreateSubSessionL()");
    CSecureServerBase* server = const_cast<CSecureServerBase*> (aServer);
    iLocMonitorServer = reinterpret_cast<CLbsLocMonitorServer*>(server);
    iSubsessionId.iSessionPtr = static_cast<CLbsLocMonitorSession*>(aMessage.Session());
    
    TInt subsessionId;
    TPckg<TInt> subsessionIdBuf(subsessionId);
    User::LeaveIfError(aMessage.Read(3, subsessionIdBuf));
    
    iSubsessionId.iSubsessionId = subsessionId;
    }

//----------------------------------------------------------------------------
// CLbsConversionPositionerSubsession::CloseSubSession
//
//-----------------------------------------------------------------------------
void CLbsConversionPositionerSubsession::CloseSubSession()
    {
    LBSLOG(ELogP1,"CLbsConversionPositionerSubsession::CloseSubSession()");
    // Release anything allocated in CreateSubSessionL.
    // In this case  there is nothing to do.
    }

//----------------------------------------------------------------------------
// CLbsConversionPositionerSubsession::VirtualRelease
//
//-----------------------------------------------------------------------------
void CLbsConversionPositionerSubsession::VirtualRelease()
    {
    LBSLOG(ELogP1,"CLbsConversionPositionerSubsession::VirtualRelease()");
    delete this;
    }
