/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics plugin API service definition header file.
*
*/



#ifndef HWRMHAPTICSPLUGINSERVICE_H
#define HWRMHAPTICSPLUGINSERVICE_H


#include <e32base.h>
#include <ecom/ecom.h>
#include <badesca.h>
#include <hwrmlogicalactuators.h>

/**
 * ECOM Plugin interface UID
 */
const TUid KCHWRMHapticsPluginServiceInterfaceUid = { 0x2001CBE4 };
    
/**
 *  Callback interface for the HWRMHaptics plugins.
 *  Used to return data and/or error codes from an asynchronous plugin calls.
 *
 * @since S60 5.1
 */
class MHWRMHapticsPluginCallback
    {
public:
    
    /**
     * Method to indicate the state of the plugin to haptics subsystem.
     * The value is directly forwarded to the user of the haptics 
     * subsystem, if it has requested notification of actuator status
     * changes.
     *
     * Plugin implementation should not assume that this information is
     * always passed to the user or that it would affect its implementation.
     * I.e. the plugin should be able to handle commands also when 
     * it is in disabled state, since the user may still request command 
     * processing from the plugin.
     * 
     * Note: When the plugin is instantiated, the initial state of the plugin
     * should always be informed through this callback method. The user of
     * haptics subsystem may use this information e.g. as a notification 
     * when to start playing haptics effect.
     *
     * @param aActuator Logical actuator type, whose state is informed by
     * this call.
     * @param aEnabled Whether or not the plugin is currently enabled.
     * If ETrue, the plugin is enabled and ready to be used. If EFalse,
     * plugin is currently disabled and cannot perform commands passed 
     * to it.
     */
    virtual void PluginEnabled( THWRMLogicalActuators aActuator, 
                                TBool aEnabled ) = 0;

    /**
     * Method to return data in response to a message from a HWRMHaptics plugin.
     * Related ProcessCommandL call must return before this method can
     * be called.
     *
     * @param aCommandId Command ID for which the response comes
     * @param aTransId   Unique transcation identifier of the original command.
     * @param aData      Data returned from call. 
     *                   Data package contents are defined by command.
     */
    virtual void ProcessResponseL( TInt aCommandId,
                                   TUint8 aTransId, 
                                   const TDesC8& aData ) = 0;
    };


/**
 * Plugin interface class to be implemented by adaptation.
 * Responsible for invoking the adaptation implementation of
 * each command and cancelling the command.
 *
 * @since S60 5.1
 */
class CHWRMHapticsPluginService : public CBase
    {
public:
    
    /**
     * Instantiation method.
     * Uses ECom to find correct instance.
     *
     * @param aImplementationUid Determines the plugin to be instantiated.
     * @param aResponseCallback Pointer to plugin callback handler.
     */
    IMPORT_C static 
    CHWRMHapticsPluginService* CHWRMHapticsPluginService::NewL( 
            TUid aImplementationUid, 
            MHWRMHapticsPluginCallback* aResponseCallback );

    /**
     * Destructor
     */
    IMPORT_C virtual ~CHWRMHapticsPluginService();

public:

    /**
     * Method to invoke a particular command in the plugin. 
     * Response to method is returned via separate ProcessResponseL
     * callback. Call to ProcessResponseL is done after the call to
     * ProcessCommandL returns. Plugin must be able to handle another
     * ProcessCommandL before ProcessResponseL is called for previous
     * call(s) (i.e. multiple clients are attempting to access resource
     * simultaneously). If ProcessCommandL leaves, no corresponding
     * ProcessResponseL is expected. 
     *
     * @param aCommandId Command ID
     * @param aTransId   Transaction ID. Unique Id allocated by the 
     *                   issuer of this command that is used to
     *                   distinguish which response relates
     *                   to which command. Also used in CancelCommandL 
     *                   to distinguish which command is cancelled.
     * @param aData      Data associated with command.
     *                   Data package contents are defined by command.
     *                   Some commands require no data and pass
     *                   empty buffer as aData;
     */
    virtual void ProcessCommandL( TInt aCommandId,
                                  TUint8 aTransId,
                                  TDesC8& aData ) = 0;

    /**
     * Method to cancel a particular command. The corresponding
     * ProcessResponseL will not be called for cancelled commands,
     * whether cancel was successful or not. 
     *
     * @param aTransId   Transaction ID
     * @param aCommandId Command ID to optionally double check with the 
     *                   transaction ID
     */
    virtual void CancelCommandL( TUint8 aTransId, 
                                 TInt aCommandId) = 0;
    
protected:
   
    /**
     * Destructor identifier to be used with ECom framework.
     */
    TUid iDestructorIDKey;

    /**
     * Callback pointer to be used with responses to commands.
     * Not owned.
     */
    MHWRMHapticsPluginCallback* iResponseCallback;
                                      
    };

#endif      // HWRMHAPTICSPLUGINSERVICE_H   
            
// End of File
