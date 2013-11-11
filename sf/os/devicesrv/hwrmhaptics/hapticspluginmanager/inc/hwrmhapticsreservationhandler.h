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
* Description:  Haptics server reservation handler header file 
*
*/


#ifndef C_HWRMHAPTICSRESERVATIONHANDLER_H
#define C_HWRMHAPTICSRESERVATIONHANDLER_H

#include <e32base.h>

class CHWRMHapticsService;
class CHWRMHapticsPolicy;


/**
 *  Handles reservations of haptics resource. 
 *
 *  @since S60 5.1
 */
class CHWRMHapticsReservationHandler : public CBase
    {
public:
        
    /**
     * Two-phased constructor.
     *
     * @param aPolicyFilename Policy file name without path.
     */
    IMPORT_C static CHWRMHapticsReservationHandler* NewL( 
            const TDesC& aPolicyFilename );
        
    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsReservationHandler();
        
public:
    
    /**
     * Reserves haptics for the given service.
     *
     * @param aSid Secure Id identifying the reserving client.
     * @param aForceNoCCoeEnv Indicates if reservation is done in force 
     *        CCoeEnv mode.
     * @param aServiceCallback The callback of service requesting 
     *        reservation check. 
     * @leave KErrAccessDenied if trying to force no CCoeEnv and client 
     *        is not trusted.
     * @return Whether or not the reservation was suspended (i.e. some other 
     *         service with higher priority has an active reservation).
     */
    TBool ReserveL( TSecureId aSid, 
                    TBool aForceNoCCoeEnv, 
                    CHWRMHapticsService* aServiceCallback );

    /**
     * Release haptics for the given service.
     *
     * @param aServiceCallback The callback of service requesting release 
     * @return Whether or not there is an active reservation after the
     *         release of the given service.
     */
    TBool Release( CHWRMHapticsService* aServiceCallback );

    /**
     * Checks whether or not haptics is reserved for some other service
     * than the given one.
     *
     * @param aServiceCallback Service requesting reservation check.
     * @return ETrue if resource is reserved for some other service
     *         than the requester. EFalse otherwise.
     */
    TBool IsReserved( const CHWRMHapticsService* aServiceCallback ) const;

    /**
     * Checks whether or not haptics is reserved for the given client.
     *
     * @param aServiceCallback Service requesting reservation check.
     * @return ETrue if resource is reserved the given service.
     */
    TBool ActiveReservation( const CHWRMHapticsService* aServiceCallback ) const;
    
    /**
     * Checks whether or not the priority of the client who has 
     * active reservation has higher priority than the currently
     * calling one.
     *
     * @param aSid Secure ID of the calling client.
     * @return ETrue if reserved client's priority is higher or equal
     *         to caller's priority. EFalse if caller's priority is higher
     *         than that of current reservation.
     */
    TBool ReservedPriorityHigher( TSecureId aSid ) const;
    
    /**
     * Sets the priority of a client.
     *
     * @param aSid Secure ID identifying the calling client.
     * @param aPriority Priority value.
     */
    void SetPriorityL( TSecureId aSid, TInt aPriority );

    /**
     * Removes the priority of a client. This method is used in the
     * cleanup phase of a service object, so that no unnecessary
     * priority values would be stored in the server.
     *
     * @param aSid Secure ID identifying the calling client.
     */
    void RemovePriority( TSecureId aSid );

    /**
     * Fetches the priority of a client identified using the given
     * secure id. If the priority is not found from the internal 
     * priority array (i.e. client has not set it), the priority 
     * value in the policy file is used. If not found from the 
     * policy file either, default value is used.
     *
     * @param aSid Secure ID identifying the calling client.
     * @param aTrusted On return specifies whether or not 
     * the client with the given secure Id is trusted 
     * (i.e. its priority is defined in a policy file). 
     * The value is set to ETrue, if the client is trusted,
     * EFalse otherwise.
     * @param aPriority The priority of the client.
     */
    void GetPriority( TSecureId aSid, TBool& aTrusted, TInt &aPriority ) const;

private:
    
    /**
     * Embedded class to contain reservation data.
     * All data members are public as this is basically a struct.
     *
     * @since S60 5.1
     */
    class TReservationData
        {
    public:

        /**
         * C++ constructor.
         */
        TReservationData()
            : iCallback( NULL ), iPriority( KMaxTInt ), iSuspendedData( NULL )
            {
            // nothing to do
            };

    public: // data
                
        /**
         * Service callback of the reserver. Not owned.
         */
        CHWRMHapticsService* iCallback;

        /**
         * Priority of the reservation
         */
        TInt iPriority;
        
        /**
         * Link for first suspended data. Not owned.
         */
        TReservationData* iSuspendedData;
        };

    /**
     * Structure for storing priority values of a single client.
     *
     * @since S60 5.1
     */
    struct TPriority
        {
        /**
         * C++ constructor.
         */
        TPriority( TSecureId aSid, TInt aPriority )
            : iSid( aSid ), iPriority( aPriority )
            {
            // nothing to do
            };

        /**
         * Secure ID identifying the client.
         */
        TSecureId iSid;

        /**
         * Priority of a client.
         */
        TInt iPriority;
        };

private:

    /**
     * C++ constructor.
     */
    CHWRMHapticsReservationHandler();

    /**
     * By default Symbian 2nd phase constructor is private.
     *
     * @param aPolicyFilename Policy file name. Must not include path.
     */
    void ConstructL( const TDesC& aPolicyFilename );
    
    /**
     * Returns whether or not a reservation exists for the given service.
     *
     * @param aService Service object, which may already have a reservation.
     * @return ETrue if a reservation exists for the given service,
     *         EFalse otherwise
     */
    TBool AlreadyReserved( const CHWRMHapticsService* aService ) const;

    /**
     * Notifies the callback in the given reservation data about
     * suspension of its reservation.
     *
     * @aData Reservation data, whose callback is notified.
     */
    void NotifySuspend( TReservationData* aData ) const;

    /**
     * Notifies the callback in the given reservation data about
     * resume of its reservation.
     *
     * @aData Reservation data, whose callback is notified.
     */
    void NotifyResume( TReservationData* aData ) const;

private: // Data
    
    /**
     * Pointer to policy. Owned.
     */
    CHWRMHapticsPolicy* iPolicy;
    
    /**
     * Reservation data. Suspended reservations are linked via 
     * iSuspendedData -pointers. Suspended reservations arrange in 
     * priority order. If NULL, there are no reservations.
     */
    TReservationData* iReservation;

    /**
     * Array of priorities or all clients received from SetPriority -commands.
     * If the priority is not found from this array nor in the priority file,
     * default priority is used for that client.
     */
    RArray<TPriority> iPriorities;
    };

#endif // C_HWRMHAPTICSRESERVATIONHANDLER_H 
            
// End of File
