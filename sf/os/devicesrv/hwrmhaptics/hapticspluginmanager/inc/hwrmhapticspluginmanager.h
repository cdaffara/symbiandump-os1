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
* Description:  Haptics server plugin handler header file.
*
*/


#ifndef C_HWRMHAPTICSPLUGINMANAGER_H
#define C_HWRMHAPTICSPLUGINMANAGER_H

#include <e32base.h>
#include <hwrmlogicalactuators.h>
#include <hwrmhapticspluginservice.h>
#include "hwrmhapticscommondata.h"
#include "hwrmhapticsgenerictimer.h"

class CHWRMHapticsService;
class CHWRMHapticsPluginTransactionList;
class CHWRMHapticsUid;
class CRepository;

/**
 * Haptics server side plugin handler implementation.
 */
class CHWRMHapticsPluginManager : public CBase,
                                  public MHWRMHapticsPluginCallback,
                                  public MHWRMHapticsGenericTimerCallback
    {
public:

    /**
     * Symbian two-phased constructor.
     *
     * @param Server common data object.
     * @param aRequestTimeout Timeout in microseconds for requests
     */
    IMPORT_C static CHWRMHapticsPluginManager* NewL( 
                            CHWRMHapticsCommonData& aHapticsCommonData, 
                            TInt aRequestTimeout );

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsPluginManager();

public:

    /**
     * Method to invoke a particular command in the plugin.
     * Only one concurrent request is supported, will leave
     * with KErrNotSupported if request is attempted while
     * another is executing
     *
     * @param aCommandId Command ID
     * @param aData Data associated with command
     * @param aCompletionCallback Callback for completion.
     *                            If NULL, no callback is attempted.
     * @return Transaction ID (can be used later to cancel)
     *         Returns zero if transaction was already completed.
     */
    TUint8 ProcessCommandL( TInt aCommandId, TDesC8& aData,
                                 CHWRMHapticsService* aCompletionCallback );

    /**
     * Method to cancel currently executing command
     *
     * @param aTransID Transaction ID.
     */
    void CancelCommandL( TUint8 aTransId );

public:

    /**
     * From MHWRMHapticsPluginCallback.
     * @see MHWRMHapticsPluginCallback
     */
    void PluginEnabled( THWRMLogicalActuators aActuator, TBool aEnabled );
    
    /**
     * From MHWRMHapticsPluginCallback.
     * @see MHWRMHapticsPluginCallback
     */
    void ProcessResponseL( TInt aCommandId, TUint8 aTransId, const TDesC8& aData );

public:
    
    /**
     * From MHWRMHapticsGenericTimerCallback.
     * @see MHWRMHapticsGenericTimerCallback
     */
    virtual void GenericTimerFired( TInt aTimerId, TBool aCutOff );

public:
    
    /**
     * Returns the supported actuator information from plugins.
     *
     * @return A mask value containing the supported actuators.
     */
    TUint32 GetSupportedActuatorInfo();

    /**
     * Opens the plugin appropriate for the actuator to be opened.
     *
     * @param aActuator Logical actuator type to be opened.
     * @return Whether or not the plugin was created (ETrue = created). 
     * If plugin was
     * not created and the method did not leave, the plugin is 
     * already created and therefore not created again. Basically this 
     * return value indicates also whether or not the plugin already
     * exists (EFalse = already exists, not created).
     */
    TBool OpenPluginToActuatorL( THWRMLogicalActuators aActuator );

    /**
     * Checks if automatic license setting is allowed for the sender of
     * the RMessage2.
     *
     * @param aMessage The message from which VendorId and SID of the
     *                 sender is read (in order to determine whether
     *                 automatic license setting is allowed or not).
     *
     * @return Whether or not automatic license setting is allowed.
     */
    TBool LicenseAutoSettingAllowed( const RMessage2& aMessage );

private: 

    /**
     * C++ constructor.
     * 
     * @param aRequestTimeout Timeout in microseconds for requests
     * @param Server common data object.
     */
    CHWRMHapticsPluginManager( CHWRMHapticsCommonData& aHapticsCommonData,
                               TInt aRequestTimeout );

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Finds the supported actuator information from plugins.
     *
     * @return A mask value containing the supported actuators.
     */
    TUint32 FindSupportedActuatorInfoL();

    /**
     * Adds actuator type to given actuator mask, if it is 
     * supported in the system.
     *
     * @param aMatch Actuator type CR key.
     * @param aMask A mask value to be added to aActuators.
     * @param aActuators A value, where to add the mask value.
     */
    void AddInfoIfSupportedL( TInt aMatch, 
                              THWRMLogicalActuators aMask, 
                              TUint32& aActuators );

private:

    /**
     * Pointer to plugin instance.
     */
    CHWRMHapticsPluginService* iPlugin;

    /**
     * Pointer to haptics data object common to all sessions. Not owned. 
     */
    CHWRMHapticsCommonData& iHapticsCommonData; 
    
    /**
     * Counter to create aTransIds
     */
    TUint8 iTransIdCounter;

    /**
     * List of open transactions 
     */
    CHWRMHapticsPluginTransactionList* iTransactionList;

    /**
     * Plugin call timeout timer.
     */
    CHWRMHapticsGenericTimer* iPluginTimer;

    /**
     * Maximum time request can be open.
     */
    TTimeIntervalMicroSeconds32 iRequestTimeout;

    /**
     * Supported actuator information.
     */
    TUint32 iSupActuators;
    
    /**
     * Reference to uid handler, owned.
     */
    CHWRMHapticsUid* iHapticsUid;

    /**
     * Access to settings.
     * Own.
     */    
    CRepository* iRepository;
    };

#endif  // C_HWRMHAPTICSPLUGINMANAGER_H
            
// End of File
