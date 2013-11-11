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
* Description:   Header definition of plugin class implementing custom interface
*                factory interface.
*
*/



#ifndef CIPLATFORMFACTORYPLUGIN_H
#define CIPLATFORMFACTORYPLUGIN_H

// Include files
#include <e32base.h>
#include <cifactoryintfc.h>

/**
 *  Plugin class implementing MCIFactoryIntfc
 *
 *  An instance of this class is created when CCIExtnClientPlugin is created.
 *  When CCIExtnClientPlugin receives CustomInterfaceExtension() message, it is
 *  forwarded to each instace of the plugin in its list.
 *
 *  Several instances this plugin can exist in the system. Platform provides a
 *  default plugin.
 *
 *  This class is intended to be used by CCIExtnClientPlugin.
 *
 *  @since S60 v3.2
 *
 */
class CCIPlatformFactoryPlugin : public CBase,
                                 public MCIFactoryIntfc
    {
public:

    /**
     * Two-phased constructor.
     * Called by ECom framework.
     */
    static MCIFactoryIntfc* NewL();

    /**
    * Destructor.
    */
    ~CCIPlatformFactoryPlugin();

    // From MCIFactoryIntfc begins
    // Called by framework when plugin is constructed
    TInt Initialize( MCustomCommand& aCustomCommand, TUid aDestructorKey );
    // Called by framework when it needs to know plugin implementation uid
    TUid ImplementationUid();
    // Called by framework forwarding request to create a custom interface
    TInt CreateInterface( TUid aUid, TAny*& aInterface );
    // Called by framework when plugin is to be deleted
    void Close();
    // From MCIFactoryIntfc ends

private:

    /**
     * Constructor.
     */
    CCIPlatformFactoryPlugin();

    /**
     * Second phase constructor.
     */
    void ConstructL();

private:

    /**
     * Reference to MCustomCommand interface
     * Not own.
     */
    MCustomCommand*             iMCustomCommand;

    /**
     * Uid of the plugin object passed by the framework via Initialize()
     */
    TUid                        iDestructorKey;
    };

#endif // CIPLATFORMFACTORYPLUGIN_H
