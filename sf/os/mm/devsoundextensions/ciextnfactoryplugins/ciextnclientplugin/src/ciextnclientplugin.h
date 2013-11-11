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
* Description:   Header definition of plugin classes implementing devsound client
*                custom interface extension.
*
*/



#ifndef CIEXTNCLIENTPLUGIN_H
#define CIEXTNCLIENTPLUGIN_H

// Include files
#include <e32base.h>
#include <a3f/mmfdevsoundcustominterfaceextensions.h>

// Forward declarations
class MCIFactoryIntfc;

/**
 *  Plugin class implementing DevSound CI client Extension
 *
 *  An instance of this class is created when DevSound is created. When client
 *  calls CMMFDevSound::CustomInterface() function with UID that is not supported
 *  by platform version of A3F DevSound, the request is then forwarded to this
 *  plugin.
 *
 *  Only one instance of this plugin can exist in the system.
 *
 *  This class is intended to be used only by DevSound client implementation.
 *
 *  @since S60 v3.2
 *
 */
class CCIExtnClientPlugin : public CBase,
                            public MDevSoundCIClientExtension
    {
public:

    /**
     * Two-phased constructor.
     * Called by ECom framework.
     */
    static MDevSoundCIClientExtension* NewL();

    /**
    * Destructor.
    */
    ~CCIExtnClientPlugin();

    // From MDevSoundCIClientExtension begins
    // Called by framework when plugin is constructed
    TInt Setup( MCustomCommand& aCustomCommand );
    // Called by framework forwarding request to create a custom interface
    TInt CustomInterfaceExtension( TUid aUid, TAny*& aInterface );
    // Called by framework when plugin is to be deleted
    void Release();
    // Called by framework after plugin is created
    void PassDestructorKey( TUid aDestructorKey );
    // From MDevSoundCIClientExtension ends

private:
    /**
     * Constructor.
     */
    CCIExtnClientPlugin();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Initializes factory plugins list.
     *
     * Leaves on error.
     */
    void InitializeFactoryPluginsL();

private:

    /**
     * Reference to MCustomCommand interface
     * Not own.
     */
    MCustomCommand*                 iMCustomCommand;

    /**
     * Uid of the plugin object passed by the framework via PassDestructorKey()
     */
    TUid                            iDestructorKey;

    /**
     * List of factory plugins. When framework calls
     * CustomInterfaceExtension() function, the request will be forwarded to each
     * factory plugin in this list until it is handled.
     *
     * Own.
     */
      RPointerArray<MCIFactoryIntfc>  iMCIFactoryIntfcList;
    };

#endif // CIEXTNCLIENTPLUGIN_H
