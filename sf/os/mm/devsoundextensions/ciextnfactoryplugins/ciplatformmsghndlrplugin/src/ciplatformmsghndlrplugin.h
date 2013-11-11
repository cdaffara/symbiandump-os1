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
* Description:   Header definition of plugin class implementing message handler
*                interface.
*
*/



#ifndef CIPLATFORMMSGHNDLRPLUGIN_H
#define CIPLATFORMMSGHNDLRPLUGIN_H

// Include files
#include <e32base.h>
#include <cimsghndlrintfc.h>
#include <mmfcontrollerframework.h>
#include <MCustomInterfaceCustomCommandImplementor.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmfcontroller.h>
#include <mmf/common/mmfcustomcommandparsermanager.h>
#endif

// Forward declarations
class CMMFObjectContainer;
class CMMFCustomCommandParserManager;
class CCustomInterfaceBuilder;

/**
 *  Plugin class implementing MCIMsgHndlrIntfc
 *
 *  An instance of this class is created when CCIExtnServerPlugin is created.
 *  When CCIExtnClientPlugin receives HandleMessageL() message, it is
 *  forwarded to each instace of the plugin in its list.
 *
 *  Several instances this plugin can exist in the system. Platform provides a
 *  default plugin.
 *
 *  This class is intended to be used by CCIExtnServerPlugin.
 *
 *  @since S60 v3.2
 *
 */
class CCIPlatformMsgHndlrPlugin : public CBase,
                                  public MCIMsgHndlrIntfc,
                                  public MCustomInterfaceCustomCommandImplementor
    {
public:

    /**
     * Two-phased constructor.
     * Called by ECom framework.
     */
    static MCIMsgHndlrIntfc* NewL();
    /**
    * Destructor.
    */
    ~CCIPlatformMsgHndlrPlugin();

    // From MCIMsgHndlrIntfc begins
    // Called by framework when plugin is constructed
    TInt Initialize( MCustomInterface& aCustomInterface, TUid aDestructorKey );
    // Called by framework when to know plugin implementation uid
    TUid ImplementationUid();
    // Called by framework forwarding request to handle aMessage
    TBool HandleMessage( const RMmfIpcMessage& aMessage );
    // Called by framework when plugin is to be deleted
    void Close();
    // From MCIMsgHndlrIntfc ends

    // From MCustomInterfaceCustomCommandImplementor begins
    // Called by framework to get handle to custom interface builder
    const TMMFMessageDestination& GetCustomInterfaceBuilderL();
    // From MCustomInterfaceCustomCommandImplementor ends

private:

    /**
     * Constructor.
     */
    CCIPlatformMsgHndlrPlugin();

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Initializes objects required for Custom Interface Framework.
     *
     * Leaves on error.
     */
    void InitializeCIFWObjectsL();

private:

    /**
     * Reference to MCustomInterface interface
     * Not own.
     */
    MCustomInterface*               iMCustomInterface;

    /**
     * Uid of the plugin object passed by the framework via Initialize()
     */
    TUid                            iDestructorKey;

    /**
     * Holds ETrue of objects required for CIFW is initialized else EFalse
     * This is used for lazy initialization. Plugins may be loaded during
     * FW initailization time and not all instances of FW may use CIFW
     */
    TBool                           iCIFWObjectsInitialized;

    /**
     * MMF object container. Container for Custom Interface builder
     * and message handlers created by it.
     *
     * Own.
     */
    CMMFObjectContainer*            iMMFObjectContainer;

    /**
     * Custom Command Parser Manager. Container for Custom Interface
     * Custom Command Parser object.
     *
     * Own.
     */
    CMMFCustomCommandParserManager* iCustomCommandParserManager;

    /**
     * Pointer to the custom interface builder. This is just a reference.
     * plugin instance does not own even though it creates this.
     * Ownership is transfered to iMMFObjectContainer.
     *
     * Not own.
     */
    CCustomInterfaceBuilder*        iCustomInterfaceBuilder;
    };

#endif // CIPLATFORMMSGHNDLRPLUGIN_H
