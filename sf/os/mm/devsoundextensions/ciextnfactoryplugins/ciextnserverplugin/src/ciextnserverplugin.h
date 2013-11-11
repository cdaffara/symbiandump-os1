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
* Description:   Header definition of plugin class implementing devsound server
*                side custom interface extension.
*
*/



#ifndef CIEXTNSERVERPLUGIN_H
#define CIEXTNSERVERPLUGIN_H

// Include files
#include <e32base.h>
#include <a3f/mmfdevsoundcustominterfaceextensions.h>
#include "ciextnserverpluginwrapper.h"

// Forward declarations
class MCustomInterface;
class MCIMsgHndlrIntfc;

/**
 *  Plugin class implementing DevSound CI server Extension
 *
 *  An instance of this class is created when DevSound Session is created. When
 *  DevSound session object receives a RMmfIpcMessage that cannot be handled, the
 *  message is forwarded to this plugin.
 *
 *  Only one instance of this plugin can exist in the system.
 *
 *  This class is intended to be used only by DevSound Session implementation.
 *
 *  @since S60 v3.2
 *
 */
class CCIExtnServerPlugin : public CBase,
                            public MDevSoundCIServerExtension
    {
public:

    /**
     * Two-phased constructor.
     * Called by ECom framework.
     */
    static MDevSoundCIServerExtension* NewL();

    /**
    * Destructor.
    */
    ~CCIExtnServerPlugin();

    // From MDevSoundCIServerExtension begins
    // Called by framework when plugin is constructed
    TInt Setup( MCustomInterface& aInterface );
    // Called by framework passing aMessage to handle
    TInt HandleMessageL(const RMmfIpcMessage& aMessage);
    // Called by framework when plugin is to be deleted
    void Release();
    // Called by framework after plugin is created
    void PassDestructorKey( TUid aDestructorKey );
    // From MDevSoundCIClientExtension ends

private:

    /**
     * Constructor.
     */
    CCIExtnServerPlugin();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Initializes factory plugins list.
     *
     * Leaves on error.
     */
    void InitializeMsgHndlrPluginsL();

private:

    /**
     * Reference to MCustomInterface interface
     * Not own.
     */
    MCustomInterface*               iMCustomInterface;

    /**
     * Reference to MCustomInterface interface
     * Not own.
     */
    CIExtnServerPluginWrapper*       iCiExtnServerPluginWrapper;
    
    /**
     * Uid of the plugin object passed by the framework via PassDestructorKey()
     */
    TUid                            iDestructorKey;

    /**
     * List of message handler plugins. When framework calls
     * HandleMessageL() function, the request will be forwarded to each
     * message handler plugin in this list until it is handled.
     *
     * Own.
     */
    RPointerArray<MCIMsgHndlrIntfc>  iMCIMsgHndlrIntfcList;
    };

#endif // CIEXTNSERVERPLUGIN_H
