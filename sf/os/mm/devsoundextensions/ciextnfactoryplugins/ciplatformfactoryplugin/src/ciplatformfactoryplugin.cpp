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
* Description:   Class definition of plugin custom interface factory interface.
*
*/



// Include files
#include "ciplatformfactoryplugin.h"
#include "ciplatformfactoryplugin.hrh"
#include "citraces.h"
#include <ecom.h>
#include <CustomInterfaceUtility.h>

#define RET_ERR_IF_ERR(s) if(s!=KErrNone) return s

// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
MCIFactoryIntfc* CCIPlatformFactoryPlugin::NewL()
    {
    DEB_TRACE0(_L("*CI* CCIPlatformFactoryPlugin::NewL"));
    CCIPlatformFactoryPlugin* self = new (ELeave) CCIPlatformFactoryPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    MCIFactoryIntfc* ptr = static_cast<MCIFactoryIntfc*>(self);
    return ptr;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCIPlatformFactoryPlugin::~CCIPlatformFactoryPlugin()
    {
    // No Impl
    }

// ---------------------------------------------------------------------------
// Called by framework when plugin is constructed
// ---------------------------------------------------------------------------
//
TInt CCIPlatformFactoryPlugin::Initialize( MCustomCommand& aCustomCommand, TUid aDestructorKey )
    {
    iMCustomCommand = &aCustomCommand;
    iDestructorKey = aDestructorKey;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Called by framework when it needs to know plugin implementation uid
// ---------------------------------------------------------------------------
//
TUid CCIPlatformFactoryPlugin::ImplementationUid()
    {
    TUid implId = {KUidCCIPlatformFactoryPlugin};
    return implId;
    }

// ---------------------------------------------------------------------------
// Called by framework forwarding request to create a custom interface
// ---------------------------------------------------------------------------
//
TInt CCIPlatformFactoryPlugin::CreateInterface( TUid aUid, TAny*& aInterface )
    {
    DEB_TRACE1(_L("*CI* CCIPlatformFactoryPlugin::CreateInterface 0x%x"), aUid.iUid);

    TInt status(KErrNone);
    aInterface = NULL;
    CCustomInterfaceUtility* ccInterfaceUtil = NULL;
    TRAP(status, ccInterfaceUtil = CCustomInterfaceUtility::NewL( *iMCustomCommand ));
    DEB_TRACE1(_L("*CI* CCIPlatformFactoryPlugin::CreateInterface status = %d"), status);

    RET_ERR_IF_ERR(status);

    // If custom interface is created, the ownership of ccInterfaceUtil
    // is automatically
    aInterface = ccInterfaceUtil->CustomInterface(aUid);
    if ( aInterface == NULL )
        {
        delete ccInterfaceUtil;
        status = KErrNotFound;
        }

    return status;
    }

// ---------------------------------------------------------------------------
// Called by framework when plugin is to be deleted
// ---------------------------------------------------------------------------
//
void CCIPlatformFactoryPlugin::Close()
    {
    DEB_TRACE0(_L("*CI* CCIPlatformFactoryPlugin::Close"));

    iMCustomCommand = NULL;

    REComSession::DestroyedImplementation(iDestructorKey);

    delete this;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCIPlatformFactoryPlugin::CCIPlatformFactoryPlugin()
:iMCustomCommand(NULL)
    {
    // No Impl
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CCIPlatformFactoryPlugin::ConstructL()
    {
    // No Impl
    }
