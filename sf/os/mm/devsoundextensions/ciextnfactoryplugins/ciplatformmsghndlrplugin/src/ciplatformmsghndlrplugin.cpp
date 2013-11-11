/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Class definition of plugin implementing message handler
*                interface.
*
*/



// Include files
#include "ciplatformmsghndlrplugin.h"
#include "ciplatformmsghndlrplugin.hrh"
#include "citraces.h"
#include <ecom.h>
#include <CustomInterfaceBuilder.h>
#include <CustomInterfaceCustomCommandParser.h>

#define RET_ERR_IF_ERR(s) if(s!=KErrNone) return s

// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
MCIMsgHndlrIntfc* CCIPlatformMsgHndlrPlugin::NewL()
    {
    DEB_TRACE0(_L("*CI* CCIPlatformMsgHndlrPlugin::NewL"));
    CCIPlatformMsgHndlrPlugin* self = new (ELeave) CCIPlatformMsgHndlrPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    MCIMsgHndlrIntfc* ptr = static_cast<MCIMsgHndlrIntfc*>(self);
    return ptr;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCIPlatformMsgHndlrPlugin::~CCIPlatformMsgHndlrPlugin()
    {
    // No Impl
    }

// ---------------------------------------------------------------------------
// Called by framework when plugin is constructed
// ---------------------------------------------------------------------------
//
TInt CCIPlatformMsgHndlrPlugin::Initialize( MCustomInterface& aCustomInterface, TUid aDestructorKey )
    {
    TInt status(KErrNone);
    iMCustomInterface = &aCustomInterface;
    iDestructorKey = aDestructorKey;
    iCIFWObjectsInitialized = EFalse;
    return status;
    }

// ---------------------------------------------------------------------------
// Called by framework when it needs to know plugin implementation uid
// ---------------------------------------------------------------------------
//
TUid CCIPlatformMsgHndlrPlugin::ImplementationUid()
    {
    TUid implId = {KUidCCIPlatformMsgHndlrPlugin};
    return implId;
    }

// ---------------------------------------------------------------------------
// Called by framework forwarding request to handle aMessage
// ---------------------------------------------------------------------------
//
TBool CCIPlatformMsgHndlrPlugin::HandleMessage( const RMmfIpcMessage& aMessage )
    {
    DEB_TRACE0(_L("*CI* CCIPlatformMsgHndlrPlugin::HandleMessage"));

    TBool handled(EFalse);
    // Initialize Custom Interface Framework Objects if not done already
    if (!iCIFWObjectsInitialized)
        {
        TRAPD( status, InitializeCIFWObjectsL());
        if ( status != KErrNone )
            {
            aMessage.Complete(status);
            handled = ETrue;
            }
        }

    // Get the destination info from the client into TMMFMessage.
    TMMFMessage message(aMessage);
    TRAPD(status, message.FetchDestinationL());

    // Check if Custom Command Parser manager can handle the message...
    if (!handled)
        {
        handled = iCustomCommandParserManager->HandleRequest(message);
        }

    // Check if the aMessage is for one of the MMF Objects
    if (!handled)
        {
        CMMFObject* object = NULL;
        // Try to find MMFObject that handles this request
        TInt status = iMMFObjectContainer->FindMMFObject(message.Destination(),
                                                     object);

        // If found, give message to the MMFObject
        if ( KErrNone == status )
            {
            object->HandleRequest(message);
            handled = ETrue;
            }
        }

    return handled;
    }

// ---------------------------------------------------------------------------
// Called by framework when plugin is to be deleted
// ---------------------------------------------------------------------------
//
void CCIPlatformMsgHndlrPlugin::Close()
    {
    DEB_TRACE0(_L("*CI* CCIPlatformMsgHndlrPlugin::Close"));

    iMCustomInterface = NULL;
    delete iCustomCommandParserManager;

    iCustomCommandParserManager =  NULL;
    delete iMMFObjectContainer;
    iMMFObjectContainer = NULL;

    iCIFWObjectsInitialized = EFalse;

    REComSession::DestroyedImplementation(iDestructorKey);

    delete this;
    }

// ---------------------------------------------------------------------------
// Called by framework to get handle to custom interface builder
// ---------------------------------------------------------------------------
//
const TMMFMessageDestination& CCIPlatformMsgHndlrPlugin::GetCustomInterfaceBuilderL()
    {
    // return the handle
    return iCustomInterfaceBuilder->Handle();
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCIPlatformMsgHndlrPlugin::CCIPlatformMsgHndlrPlugin()
:iMCustomInterface(NULL),
iCIFWObjectsInitialized(EFalse),
iMMFObjectContainer(NULL),
iCustomCommandParserManager(NULL),
iCustomInterfaceBuilder(NULL)
    {
    // No Impl
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CCIPlatformMsgHndlrPlugin::ConstructL()
    {
    // No Impl
    }

// ---------------------------------------------------------------------------
// Initializes objects required for Custom Interface Framework.
// ---------------------------------------------------------------------------
//
void CCIPlatformMsgHndlrPlugin::InitializeCIFWObjectsL()
    {
    DEB_TRACE0(_L("*CI* CCIPlatformMsgHndlrPlugin::InitializeCIFWObjectsL"));

    iMMFObjectContainer = new(ELeave) CMMFObjectContainer;

    iCustomCommandParserManager = CMMFCustomCommandParserManager::NewL();

    CCustomInterfaceCustomCommandParser* ciccParser = CCustomInterfaceCustomCommandParser::NewL(*this);
    CleanupStack::PushL( ciccParser );

    iCustomCommandParserManager->AddCustomCommandParserL(*ciccParser);
    CleanupStack::Pop( ciccParser );

    iCustomInterfaceBuilder = CCustomInterfaceBuilder::NewL( *iMMFObjectContainer, *iMCustomInterface);
    iMMFObjectContainer->AddMMFObject(*iCustomInterfaceBuilder);

    iCIFWObjectsInitialized = ETrue;
    }
