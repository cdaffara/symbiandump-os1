/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




/**
 @file
 @internalTechnology 
*/


#ifndef HWRMRESERVATIONHANDLER_H
#define HWRMRESERVATIONHANDLER_H

//  INCLUDES
#include <e32base.h>
#include "HWRMPolicy.h"

// CONSTANTS
const TInt KHWRMAllSubResources = 0;

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMService;

// CLASS DECLARATION

/**
*  Handles reservations of single resource. 
*
*/
class CHWRMReservationHandler : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aSubResourceCount Number of subresources this handler needs
        *         to be able to handle.
        * @param aPolicyFilename Policy file name. Must not include path.
        */
        static CHWRMReservationHandler* NewL(TInt aSubResourceCount, 
                                             const TDesC& aPolicyFilename);
        
        /**
        * Destructor.
        */
        virtual ~CHWRMReservationHandler();
        
    public: // New functions
    
        /**
        * Reserve set of subresources
        *
        * @param aMessage Message pointer for identifying the reserving client
        * @param aForceNoCCoeEnv Indicates if reservation is done in force CCoeEnv mode.
        * @param aServiceCallback The callback of service requesting reservation check. 
        * @param aSubResourceMask Mask of subresources for which reservation check is done.
        *         Zero means the whole resource.
        * @return SubresourceMask indicating subresources that were immediately suspended.
        * @leave KErrAccessDenied if trying to force no CCoeEnv and client is not trusted.
        */
        TInt ReserveL(const RMessagePtr2& aMessage, TBool aForceNoCCoeEnv, CHWRMService* aServiceCallback, TInt aSubResourceMask = KHWRMAllSubResources);

        /**
        * Release set of subresources
        *
        * @param aServiceCallback The callback of service requesting release 
        * @param aSubResourceMask Mask of subresources for which release targets.
        *         Zero means the whole resource.
        * @return SubresourceMask indicating subresources that do not have any more reservations.
        *          Note that this includes all unreserved subresources, not just ones among 
        *          aSubResourceMask subresources.
        */
        TInt Release(CHWRMService* aServiceCallback, TInt aSubResourceMask = KHWRMAllSubResources);

        /**
        * Check if set of subresources is reserved. Suspended reservations are also checked.
        *
        * @param aServiceCallback The callback of service requesting reservation check. 
        * @param aSubResourceMask Mask of subresources for which reservation check is done.
        *         Zero means the whole resource.
        * @return ETrue if (sub)resource is reserved for some other service than the requester.
        */
        TBool IsReserved(const CHWRMService* aServiceCallback, TInt aSubResourceMask = KHWRMAllSubResources) const;
        
        /**
        * Get unreserved targets mask
        *
        * @return target mask containing unreserved targets
        */
        TInt GetUnreservedTargets();
        
        /**
        * Updates active targets mask and notifies target activity state changes
        * to sessions that have active reservations. However, activation is not
        * notified to whichever session calls this method to avoid unnecessary 
        * transitory light state. Deactivation must be notified also to caller, 
        * if it has active reservation for that target.
        *
        * @param aOriginalTarget Original target before modification
        * @param aModifiedTarget Target after modification was applied
        * @param aUpdater Session from which the update request comes.
        */
        void UpdateActiveMask(TInt aOriginalTarget, TInt aModifiedTarget, CHWRMService* aUpdater);

        /**
        * Get the active reserver for specified target
        *
        * @param aTargetIndex Single target index for which to get the reserver info.
        * @return Pointer to active reserver session or NULL.
        */
        CHWRMService* GetActiveReserver(TInt aTargetIndex);
        

        /**
        * Get policy instance
        *
        * @return Pointer to policy.
        */
        CHWRMPolicy* GetPolicy();
    public: // Functions from base classes
 
    protected:  // New functions
        
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        *
        * @param aSubResourceCount Number of subresources this handler needs
        *         to be able to handle.
        */
        CHWRMReservationHandler(TInt aSubResourceCount);

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aPolicyFilename Policy file name. Must not include path.
        */
        void ConstructL(const TDesC& aPolicyFilename);
       
    public:     // Data
    
    protected:  // Data

    private:    // Data
    
        /**
        *  Embedded class to contain reservation data.
        *  All data members are public as this is basically a struct.
        *
        */
        class TReservationData
            {
            public:     // Constructors and destructor

                /**
                * C++ constructor to initialize defaults.
                */
                TReservationData()
                    : iCallback(NULL), 
                      iPriority(KMaxTInt), 
                      iSuspendedData(NULL)
                        {
                        };

                // data
                
                CHWRMService*     iCallback;      // Service callback of the reserver. Not owned.

                TInt              iPriority;      // Priority of the reservation
                TReservationData* iSuspendedData; // Link for first suspended data. Not owned.
            };

    
    	TInt         iSubResourceCount;  // Number of subresources supported.
    	TInt         iAllMask;           // This subresource mask is used if 0 mask is specified.
             	                         // Cannot just use constant as number of subs varies.
    	CHWRMPolicy* iPolicy;            // Pointer to policy. Owned.
    	RPointerArray<TReservationData> iReservations;  // Pointers to reservation data objects. 
    	                                 // First reservation is the active one,
    	                                 // Suspended reservations are linked via iSuspendedDatas.
    	                                 // Suspended reservations naturally arrange in priority order
          	                             // NULL value indicates that  
        	                             //  selected subresource is not reserved.    	
        	                             
        TInt         iActiveMask;        // Mask of active targets (i.e. not removed by target modifier plugin)       	                             

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes


    };

#endif      // HWRMRESERVATIONHANDLER_H 
            
// End of File
