/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of CSystemStartupExtension class.
*
*/


#ifndef __SYSTEMSTARTUPEXTENSION_H__
#define __SYSTEMSTARTUPEXTENSION_H__

#include <e32base.h>

#pragma message( "Systemstartupextension.h has been deprected. Please use ssmcustomcommand.h instead." )

class RFs;

/**
 *  Base class for all System Startup Extension Plugin implementations.
 *
 *  System startup extension plugins can be used to add small tasks to startup
 *  process without need to start a process to handle the task.
 *  CSystemStartupExtension class is the base class all startup extension
 *  plugin implementations need to derive from.
 *
 *  System startup extension plugins should not be used to execute very
 *  time-consuming tasks, because that makes the overall startup time longer.
 *  The plugins are loaded to S60 Starter servers thread, so they also consume
 *  S60 Starter servers heap and stack space - when writing system startup
 *  extension plugins you should be very careful with performance and memory
 *  consumption issues.
 *
 *  Because startup extension plugins run in S60 Starter servers thread,
 *  they need to have enough capabilities, meaning the capability set of
 *  CAP_GENERAL_DLL. Due to security reasons, system startup extension plugins
 *  can only be loaded from the Z: drive.
 *
 *  @lib None
 *  @since S60 3.2
 */
class CSystemStartupExtension : public CBase
    {

public:

    /**
    * Constructor.
    *
    * @param aConstructionParameters Construction parameters from ECom. May NOT be NULL.
    *
    * Implement a factory method with this signature, and give the parameter
    * to this constructor.
    *
    * Example:
    *
    * CMyExtensionPlugIn* CMyExtensionPlugIn::NewL( TAny* aConstructionParameters )
    *     {
    *     CMyExtensionPlugIn* self =
    *         new( ELeave ) CMyExtensionPlugIn( aConstructionParameters );
    *     CleanupStack::PushL( self );
    *     self->ConstructL();
    *     CleanupStack::Pop( self );
    *     return self;
    *     }
    *
    * CMyExtensionPlugIn::CMyExtensionPlugIn( TAny* aConstructionParameters )
    *  : CSystemStartupExtension( aConstructionParameters )
    *    {
    *    }
    */
    inline CSystemStartupExtension( TAny* aConstructionParameters );

    /**
     * Initiates the startup extension task.
     *
     * A derived class must provide an implementation to actually initiate the
     * task the plugin is intended to handle.
     *
     * ExecuteL should pass the request status object to the service handler
     * that implements the actual service logic. Time-consuming synchronous
     * function calls in ExecuteL should be avoided if possible, because they
     * block the S60 Starter servers main thread.
     * Never call User::WaitForRequest in ExecuteL!
     *
     * The plugin DLL is not unloaded until the request status is completed, or
     * a timeout occurs. The timeout value can be configured in the system
     * startup configuration resource. If a timeout occurs while the request is
     * pending, the Cancel() function of the derived class is called before the
     * plugin DLL is unloaded.
     *
     * Example:
     *
     * void CMyExtensionPlugin::ExecuteL(
     *     TRequestStatus& aStatus,
     *     const TDesC& aParams )
     *     {
     *     TInt errorCode = iServer.Connect();
     *     if ( errorCode == KErrNone )
     *         {
     *         iServer.DoAsyncOperation( aStatus );
     *         }
     *     else
     *         {
     *         TRequestStatus* status = &aStatus;
     *         User::RequestComplete( status, errorCode );
     *         }
     *     }
     *
     * void CMyExtensionPlugin::~CMyExtensionPlugin(
     *     {
     *     iServer.Close();
     *     }
     *
     * @since S60 3.2
     * @param aStatus The request status to complete after the task has been
     * finished.
     * @param aParams Additional plugin-specific data. S60 Starter server reads
     * the value from the system startup configuration resource and passes it
     * to the plugin implementation.
     */
    virtual void ExecuteL( TRequestStatus& aStatus, const TDesC& aParams ) = 0;

    /**
     * Cancels the startup extension task.
     *
     * A derived class must provide an implementation to cancel the request
     * made in ExecuteL.
     *
     * Example:
     *
     * void CMyExtensionPlugin::Cancel()
     *     {
     *     iServer.CancelAsyncOperation();
     *     }
     *
     * @since S60 3.2
     */
    virtual void Cancel() = 0;

protected:

    // File server session that can be used by the plug-in implementation if
    // it requires file server access.
    RFs& iFs;

    };

#include <systemstartupextension.inl>

#endif // __SYSTEMSTARTUPEXTENSION_H__
