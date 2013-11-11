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
*                factory interface.
*
*/



#ifndef CIFACTORYINTFC_H
#define CIFACTORYINTFC_H

// Include files
#include <mmf/common/mcustomcommand.h>

/**
 * Mixin class for plugins creating Custom Interface objects.
 *
 * Instance of plugins implementing this interface will be used by
 * CCIExtnClientPlugin.
 *
 *  @since S60 v3.2
 *
 */
class MCIFactoryIntfc
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
     * @param aCustomCommand  Reference to implementation of MCustomCommand.
     * @param aDestructorKey The Uid returned by
     *        REComSession::CreateImplementationL() or similar.
     */
    virtual TInt Initialize( MCustomCommand& aCustomCommand, TUid aDestructorKey ) = 0;

    /**
     * Returns implementation UID of this plugin.
     *
     * @return aUid implementation UID of the plugin
     */
    virtual TUid ImplementationUid() = 0;

    /**
     * Called by the framework to obtain a pointer to the Custom Interface.
     *
     * Note that ownership of the interface will be transferred to the caller.
     *
     * @param aUid UID of the required interface.
     * @param aInterface Used to return pointer to the interface. Must be set
     *        to NULL if no matching interface.
     * @return an error code KErrNotFound if requested custom inteface is not
     *         supported by this plugin. KErrNone when custom interface object is
     *         successfully created and returned. Any other error code returned
     *         implies that custom interface is supported but the plugin was
     *         unable to instantiate it when requested.
     */
    virtual TInt CreateInterface( TUid aUid, TAny*& aInterface ) = 0;

    /**
     * Called by the framework when plugin is to be destroyed.
     * Equivalent to destructor. Called to destroy plugin.
     */
    virtual void Close() = 0;
    };

#endif // CIFACTORYINTFC_H
