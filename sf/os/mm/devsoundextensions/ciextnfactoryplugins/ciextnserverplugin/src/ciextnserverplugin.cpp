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
* Description:   Class definition of plugin implementing devsound server
*                side custom interface extension.
*
*/



// Include files
#include "ciextnserverplugin.h"
#include "citraces.h"
#include <ecom.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <cimsghndlrintfc.h>
#include <cimsghndlrintfc.hrh>
#include <e32const.h>

// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
MDevSoundCIServerExtension* CCIExtnServerPlugin::NewL()
    {
    DEB_TRACE0(_L("*CI* CCIExtnServerPlugin::NewL")); 
    CCIExtnServerPlugin* self = new (ELeave)CCIExtnServerPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    MDevSoundCIServerExtension* ptr = static_cast<MDevSoundCIServerExtension*>(self);
    return ptr;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCIExtnServerPlugin::~CCIExtnServerPlugin()
    {
    DEB_TRACE0(_L("*CI* CCIExtnServerPlugin::~CCIExtnServerPlugin")); 
    }

// ---------------------------------------------------------------------------
// Called by framework when plugin is constructed
// ---------------------------------------------------------------------------
//
TInt CCIExtnServerPlugin::Setup( MCustomInterface& aInterface )
    {
    DEB_TRACE0(_L("*CI* CCIExtnServerPlugin::Setup"));
    TInt status(KErrNone);
    TRAPD(err, iCiExtnServerPluginWrapper = CIExtnServerPluginWrapper::NewL(aInterface));
    if (err == KErrNone)
        {
        iMCustomInterface = iCiExtnServerPluginWrapper->GetInterface();
        }
    TRAP_IGNORE(InitializeMsgHndlrPluginsL());
    return status;
    }

// ---------------------------------------------------------------------------
// Called by framework forwarding passing aMessage to handle
// ---------------------------------------------------------------------------
//
TInt CCIExtnServerPlugin::HandleMessageL(const RMmfIpcMessage& aMessage)
    {
    DEB_TRACE0(_L("*CI* CCIExtnServerPlugin::HandleMessageL"));

    TInt status(KErrNotSupported);

    // Forward request to each message handler plugin in the list,
    TBool msgHandled(EFalse);
    for ( TInt index = 0; index < iMCIMsgHndlrIntfcList.Count(); index++ )
        {
        msgHandled = iMCIMsgHndlrIntfcList[index]->HandleMessage( aMessage );
        // If the plugin handled aMessage, stop forwarding the request to
        // other plugins in the list and break out of loop.
        if ( msgHandled == TBool(ETrue) )
            {
            status = KErrNone;
            break;
            }
        }

    return status;
    }

// ---------------------------------------------------------------------------
// Called by framework when plugin is to be deleted
// ---------------------------------------------------------------------------
//
void CCIExtnServerPlugin::Release()
    {
    DEB_TRACE0(_L("*CI* CCIExtnServerPlugin::Release"));
    
    for ( TInt index = 0; index < iMCIMsgHndlrIntfcList.Count(); index++ )
        {
        iMCIMsgHndlrIntfcList[index]->Close();
        }
    
    iMCIMsgHndlrIntfcList.Reset();
    iMCIMsgHndlrIntfcList.Close();

    iCiExtnServerPluginWrapper->Release();

    REComSession::DestroyedImplementation(iDestructorKey);

    delete this;
    }

// ---------------------------------------------------------------------------
// Called by framework after plugin is created
// ---------------------------------------------------------------------------
//
void CCIExtnServerPlugin::PassDestructorKey( TUid aDestructorKey )
    {
    iDestructorKey = aDestructorKey;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCIExtnServerPlugin::CCIExtnServerPlugin()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CCIExtnServerPlugin::ConstructL()
    {
    // No impl
    }

// ---------------------------------------------------------------------------
// Initializes factory plugins list.
// ---------------------------------------------------------------------------
//
void CCIExtnServerPlugin::InitializeMsgHndlrPluginsL()
    {
    // 1. Query the implementation ids of plugins implementing
    //    KUidCIMsgHndlrIntfcInterface
    // 2. Instantiate it and add it to the list of plugins
    DEB_TRACE0(_L("*CI* CCIExtnServerPlugin::InitializeMsgHndlrPluginsL"));

    iMCIMsgHndlrIntfcList.Reset();

    RImplInfoPtrArray msgHndlrEComUids;
    CleanupResetAndDestroyPushL(msgHndlrEComUids);


    // List all the factory plugins
    TUid msgHndlrPluginInterfaceUid = {KUidCIMsgHndlrIntfcInterface};
    TEComResolverParams resParams;
    REComSession::ListImplementationsL( msgHndlrPluginInterfaceUid, resParams, 
                                        KRomOnlyResolverUid, msgHndlrEComUids );

    // Instantiate and add it to the list of factories
    TUid destructorKey;
    MCIMsgHndlrIntfc* msgHndlrPlugin(NULL);
    TInt status(KErrNone);

    for ( TInt index = 0; index < msgHndlrEComUids.Count(); index++)
        {
        TRAP( status ,
                msgHndlrPlugin = static_cast<MCIMsgHndlrIntfc*>
                                    (REComSession::CreateImplementationL(
                                            msgHndlrEComUids[index]->ImplementationUid(),
                                            destructorKey ) ) );
        // If there was problem instantiating factory plugin, continue trying
        // next one.
        if ( status != KErrNone)
            {
            DEB_TRACE1(_L("*CI* CCIExtnServerPlugin::InitializeMsgHndlrPluginsL creation status=%d"),status);
            continue;
            }
 
        // Initialize the factory plugin
        if ( msgHndlrPlugin->Initialize( *iMCustomInterface, destructorKey ) == KErrNone )
            {
            status = iMCIMsgHndlrIntfcList.Append(msgHndlrPlugin);
            if ( status != KErrNone )
                {// There was problem adding plugin to list, there was a system
                 // wide error. Stop trying and return error code.
                msgHndlrPlugin->Close();
                User::Leave( status );
                }
            }
        else
            {// There was problem initializing the factory plugin instance, close
             // it and continue instantiating the rest
            msgHndlrPlugin->Close();
            }
        }
    CleanupStack::PopAndDestroy(&msgHndlrEComUids);
    }
