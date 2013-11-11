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
* Description:  Haptic service header file.
*
*/


#ifndef C_HWRMHAPTICSSERVICE_H
#define C_HWRMHAPTICSSERVICE_H

#include <e32base.h>
#include "hwrmhapticsgenerictimer.h"
#include "hwrmhapticscommondata.h"

class MHWRMHapticsPacketizer;
class CHWRMHapticsPluginManager;
class CHWRMHapticsReservationHandler;
class CHWRMHapticsPluginTransactionList;

/**
 * Haptics server side service object. Each session has one service object.
 *
 * @since S60 5.1
 */
class CHWRMHapticsService : public CBase
    {
public:

    /**
     * Symbian two-phased constructor.
     *
     * @param aPluginHandler  The plugin handler to use with this service.
     * @param aReservationHandler The reservation handler to use with 
     * this service.
     * @param aHapticsCommonData Haptics common data.
     * @param aMessage Message used to instantiate this service instance.
     * @return A pointer to the created instance.
     */
    IMPORT_C static CHWRMHapticsService* NewL( 
                        CHWRMHapticsPluginManager* aPluginHandler,
                        CHWRMHapticsReservationHandler* aReservationHandler,
                        CHWRMHapticsCommonData& aHapticsCommonData,
                        const RMessage2& aMessage );

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsService();

public:

    /**
     * CHWRMHapticsSession calls this method to handle plugin related messages.
     *
     * @param aMessage Service message to be completed in ProcessResponseL.
     * @return ETrue if completion is needed for message, EFalse otherwise.
     */
    IMPORT_C TBool ExecuteMessageL( const RMessage2& aMessage );

    /**
     * Callback method for plugin handlers to notify this service
     * that request has been processed. 
     * This method is not called in case request is canceled.
     *
     * @param aCommandId The command id for which the response came
     * @param aTransId Related transaction Id
     * @param aData Response data
     */
    void ProcessResponseL( TInt aCommandId, TUint8 aTransId, 
                                    const TDesC8& aData );

    /**
     * Method to suspend the use of resource of service. Used by the 
     * reservation handler.
     */
    void SuspendResource();

    /**
     * Method to resume suspended resource. Used by the reservation handler.
     */
    void ResumeResource();

    /**
     * Method for cancelling outstanding request
     *
     * @param aTransId Related transaction Id
     */
    void CancelRequest( TUint8 aTransId );

private:

    /**
     * C++ constructor.
     * 
     * @param aHapticsCommonData Haptics common data.
     * @param aMessage Message used to instantiate this service instance.
     */
    CHWRMHapticsService( CHWRMHapticsCommonData& aHapticsCommonData,
                         const RMessage2& aMessage );

    /**
     * Second phase construction.
     *
     * @param aPluginManager The plugin handler to use with this service.
     * @param aReservationHandler The reservation handler to use with 
     * this service.
     */
    void ConstructL( CHWRMHapticsPluginManager* aPluginManager,
                     CHWRMHapticsReservationHandler* aReservationHandler );

    /**
     * CompleteRequest can be called from ProcessResponseL to 
     * complete any outstanding request and remove transaction
     * from transaction list.
     *
     * @param aTransId Transaction identifier
     * @param aData Request response data.
     * @return Error code if the transaction is not found.
     */
    TInt CompleteRequestL( TUint8 aTransId,
                           const TDesC8& aData );

    /**
     * Checks that no transaction for this message still remains in transaction list
     *
     * @param aHandle Message handle.
     * @return ETrue if list still contains transaction data for checked message.
     */
    TBool CheckForMessage( TInt aHandle );

    /**
     * Cleans up the haptics state
     */
    void CleanupHaptics();

    /**
     * Send the message to pluginManager.This method is used by the
     * ExecuteMessageL() to handle plugin related messages.
     *
     * @param aMessage Service message to be completed in ProcessResponseL.
     */
    void SendMsgToPluginManagerL( const RMessage2& aMessage );

    /**
     * Send the message to close device to pluginManager.
     *
     * @param aMessage Service message to be completed in ProcessResponseL.
     */
    void SendCloseActuatorMsgL( const RMessage2& aMessage );

    /**
     * Reserves haptics.
     *
     * @param aMessage Command message received from the client.
     */
    void ReserveHapticsL( const RMessage2& aMessage );

    /**
     * Releases haptics.
     */
    void ReleaseHaptics();

    /**
     * Sets given haptics status to iLastStatus.
     *
     * @param aHapticStatus Status to be stored.
     */
    void SetHapticsStatus( 
        MHWRMHapticsObserver::THWRMHapticsStatus aHapticStatus );

private: // data

    /**
     * Reference to plugin handler, not owned.
     */
    CHWRMHapticsPluginManager* iPluginManager;

    /**
     * List of open transactions.
     */
    CHWRMHapticsPluginTransactionList* iTransactionList;

    /**
     * Reservation handler, not owned
     */
    CHWRMHapticsReservationHandler* iReservationHandler;

    /**
     * Haptic packetizer.
     */
    MHWRMHapticsPacketizer* iPacketizer;

    /**
     * Pointer to haptics data object common to all sessions. Not owned. 
     */
    CHWRMHapticsCommonData& iHapticsCommonData; 

    /**
     * Flag to indicate if this session is suspended.
     */
    TBool iSuspended;

    /**
     * Flag to indicate if this session is reserved.
     */
    TBool iReserved;

    /**
     * Flag to indicate if cleanup has been done.
     */
    TBool iCleanupDone;

    /**
     * Secure ID identifying the client.
     */
    TSecureId iSid;

    /**
     * Pointer to the session, which created this instance. 
     * Used to identify the session.
     */
    const CSession2* iSession;
    };

#endif  // C_HWRMHAPTICSSERVICE_H
            
// End of File
