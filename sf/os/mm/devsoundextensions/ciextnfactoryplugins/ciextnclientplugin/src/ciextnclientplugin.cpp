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
* Description:   Class definition of plugin implementing devsound client
*                custom interface extension.
*
*/



// Include files
#include "ciextnclientplugin.h"
#include "citraces.h"
#include <ecom.h>
#include <cifactoryintfc.h>
#include <cifactoryintfc.hrh>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#define RET_ERR_IF_ERR(s) if(s!=KErrNone) return s

// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
MDevSoundCIClientExtension* CCIExtnClientPlugin::NewL()
    {
    DEB_TRACE0(_L("*CI* CCIExtnClientPlugin::NewL"));
    CCIExtnClientPlugin* self = new (ELeave)CCIExtnClientPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    MDevSoundCIClientExtension* ptr = static_cast<MDevSoundCIClientExtension*>(self);
    return ptr;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCIExtnClientPlugin::~CCIExtnClientPlugin()
    {
    iMCIFactoryIntfcList.Close();
    DEB_TRACE0(_L("*CI* CCIExtnClientPlugin::~CCIExtnClientPlugin"));
    }

// ---------------------------------------------------------------------------
// Called by framework when plugin is constructed
// ---------------------------------------------------------------------------
//
TInt CCIExtnClientPlugin::Setup( MCustomCommand& aCustomCommand )
    {
    DEB_TRACE0(_L("*CI* CCIExtnClientPlugin::Setup"));
    TInt status(KErrNone);
    iMCustomCommand = &aCustomCommand;
    TRAP_IGNORE(InitializeFactoryPluginsL());
    return status;
    }

// ---------------------------------------------------------------------------
// Called by framework forwarding request to create a custom interface
// ---------------------------------------------------------------------------
//
TInt CCIExtnClientPlugin::CustomInterfaceExtension( TUid aUid, TAny*& aInterface )
    {
    DEB_TRACE1(_L("*CI* CCIExtnClientPlugin::CustomInterfaceExtension 0x%x"), aUid.iUid);

    TInt status(KErrNotFound);
    aInterface = NULL;

    // Forward request to each factory plugin in the list,
    for ( TInt index = 0; index < iMCIFactoryIntfcList.Count(); index++ )
        {
        status = iMCIFactoryIntfcList[index]->CreateInterface( aUid, aInterface );
        // The factory tried creating custom interface successfully or otherwise.
        // stop forwarding the request to other factory plugins in the list.
        // If the factory does not support a custom interface with aUid, it will
        // return KErrNotFound
        if ( status != KErrNotFound )
            {
            break;
            }
        }
    return status;
    }

// ---------------------------------------------------------------------------
// Called by framework when plugin is to be deleted
// ---------------------------------------------------------------------------
//
void CCIExtnClientPlugin::Release()
    {
    DEB_TRACE0(_L("*CI* CCIExtnClientPlugin::Release"));

    for ( TInt index = 0; index < iMCIFactoryIntfcList.Count(); index++ )
        {
        iMCIFactoryIntfcList[index]->Close();
        }

    iMCIFactoryIntfcList.Reset();
    iMCIFactoryIntfcList.Close();

    REComSession::DestroyedImplementation(iDestructorKey);

    delete this;
    }

// ---------------------------------------------------------------------------
// Called by framework after plugin is created
// ---------------------------------------------------------------------------
//
void CCIExtnClientPlugin::PassDestructorKey( TUid aDestructorKey )
    {
    iDestructorKey = aDestructorKey;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCIExtnClientPlugin::CCIExtnClientPlugin()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCIExtnClientPlugin::ConstructL()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// Initializes factory plugins list.
// ---------------------------------------------------------------------------
//
void CCIExtnClientPlugin::InitializeFactoryPluginsL()
    {
    // 1. Query the implementation ids of plugins implementing
    //    KUidCIFactoryIntfcInterface
    // 2. Instantiate it and add it to the list of factories

    DEB_TRACE0(_L("*CI* CCIExtnClientPlugin::InitializeFactoryPluginsL"));

    iMCIFactoryIntfcList.Reset();

    RImplInfoPtrArray factoryEComUids;
    CleanupResetAndDestroyPushL(factoryEComUids);

    // List all the factory plugins
    TUid factoryPluginInterfaceUid = {KUidCIFactoryIntfcInterface};
    TEComResolverParams resParams;
    REComSession::ListImplementationsL( factoryPluginInterfaceUid,  resParams, 
                                        KRomOnlyResolverUid, factoryEComUids );

    // Instantiate and add it to the list of factories
    TUid destructorKey;
    MCIFactoryIntfc* factoryPlugin(NULL);
    TInt status(KErrNone);
    for ( TInt index = 0; index < factoryEComUids.Count(); index++)
        {
        //<415-4087> TN: created with ImplementationUid - KRomOnlyResolverUid not used
        TRAP( status ,
              factoryPlugin = static_cast<MCIFactoryIntfc*>
                                    (REComSession::CreateImplementationL(
                                                        factoryEComUids[index]->ImplementationUid(),
                                                        destructorKey ) ) );
        // If there was problem instantiating factory plugin, continue trying
        // next one.
        if ( status != KErrNone)
            {
            DEB_TRACE1(_L("*CI* CCIExtnClientPlugin::InitializeFactoryPluginsL create failed status=%d"), status);
            continue;
            }

        // Initialize the factory plugin
        if ( factoryPlugin->Initialize( *iMCustomCommand, destructorKey ) == KErrNone )
            {
            status = iMCIFactoryIntfcList.Append(factoryPlugin);
            if ( status != KErrNone )
                {// There was problem adding plugin to list, there was a system
                 // wide error. Stop trying and return error code.
                factoryPlugin->Close();
                User::Leave( status );
                }
            }
        else
            {// There was problem initializing the factory plugin instance, close
             // it and continue instantiating the rest
            factoryPlugin->Close();
            }
        }
    CleanupStack::PopAndDestroy(&factoryEComUids);
    }
