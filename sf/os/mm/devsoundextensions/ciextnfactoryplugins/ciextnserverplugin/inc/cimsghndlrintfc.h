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
* Description:   Interface for plugin classes implementing custom interface
*                message handler interface.
*
*/



#ifndef CIMSGHNDLRINTFC_H
#define CIMSGHNDLRINTFC_H

// Include files
#include <mmf/common/mcustominterface.h>
#include <mmf/common/mmfipc.h>

/**
 * Mixin class for plugins handling custom interface related messages
 *
 * Instance of plugins implementing this interface will be used by
 * CCIExtnServerPlugin.
 *
 *  @since S60 v3.2
 *
 */
class MCIMsgHndlrIntfc
    {
public:
    /**
     * Called by framework after plugin construction.
     * The passed UID must be retained for the lifetime of the plugin and, on a
     * subsequent call to Close(), REComSession::DestroyedImplementation() must
     * be called by the plugin, using the stored UID.
     * The implementation should return KErrNone upon successful initialization.
     * Otherwise the plugin will be immediately destroyed.
     *
     * @param aCustomInterface  Reference to implementation of MCustomInterface.
     * @param aDestructorKey The Uid returned by
     *        REComSession::CreateImplementationL() or similar.
     */
    virtual TInt Initialize( MCustomInterface& aCustomInterface, TUid aDestructorKey ) = 0;

    /**
     * Returns implementation UID of this plugin.
     *
     * @return aUid implementation UID of the plugin
     */
    virtual TUid ImplementationUid() = 0;

    /**
     * Called by the framework to give the plugin a chance to handle the
     * custom interface message.
     *
     * @param aMessage Message received by MM framework.
     * @return ETrue if message can be handled by the plugin. EFalse if the
     * 		   message is not supported by the plugin. Returning EFalse imples
     * 		   aMessage not completed and should be processed further by the
     * 		   framework.
     */
    virtual TBool HandleMessage( const RMmfIpcMessage& aMessage ) = 0;

    /**
     * Called by the framework when plugin is to be destroyed.
     * Equivalent to destructor. Called to destroy plugin.
     */
    virtual void Close() = 0;

    };

#endif /*CIMSGHNDLRINTFC_H*/
