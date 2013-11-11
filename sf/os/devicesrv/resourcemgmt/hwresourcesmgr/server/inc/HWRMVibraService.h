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

#ifndef HWRMVIBRASERVICE_H
#define HWRMVIBRASERVICE_H

//  INCLUDES
#include <e32base.h>
#include "HWRMGenericTimer.h"
#include "HWRMVibraCommands.h"
#include "HWRMVibra.h"
#include "HWRMService.h"
#include "HWRMVibraCommonService.h"
#include "HWRMVibraCommonData.h"
// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMVibraCommonData;
class CHWRMPluginHandler;
class CHWRMReservationHandler;

// CLASS DECLARATION

/**
*  Hardware Resource Manager server side service object for Vibra sessions.
*
*/
class CHWRMVibraService : public CHWRMService,
                          public MHWRMGenericTimerCallback
    {
    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        *
        * @param aPluginHandler The plugin handler to use with this service.
        * @param aReservationHandler The reservation handler to use with this service.
        * @param aVibraCommonData Reference to vibra common data
        * @param aPrivilegedClient If ETrue the client of this service is privileged 
        *          i.e. ignores vibra profile setting checks
        */
        static CHWRMVibraService* NewL(CHWRMPluginHandler* aPluginHandler,
                                       CHWRMReservationHandler* aReservationHandler,
                                       CHWRMVibraCommonService& aCommonService,
                                       CHWRMVibraCommonData& aVibraCommonData,
                                       TBool aPrivilegedClient);
        
        /**
        * Destructor.
        */
        virtual ~CHWRMVibraService();
        
        /**
        * Forces vibra to stop immediately, regardless of reservations.
        */
        void ForceVibraStop();
        
        /**
        * Checks if it is ok to stop vibra from another session based on what this
        * session desires for the state to be.
        *
        * @return ETrue if ok to stop vibra
        */
        TBool OkToStopVibra();
    
    public: // Base class methods
    
        // From CHWRMService
    	virtual TBool ExecuteMessageL(const RMessage2& aMessage);
    	virtual void ProcessResponseL( TInt aCommandId,  TUint8 aTransId, TDesC8& aData, TBool aTimeout );
    	virtual void SuspendSubResource(TInt aSubResource);
    	virtual void ResumeSubResource(TInt aSubResource);
        virtual void ActivateSubResource(TInt aSubResource, TBool aActivate);    	    	
        
    	// From MHWRMGenericTimerCallback    	
    	virtual void GenericTimerFired(TInt aTimerId, TBool aCutOff);
    	
    private: 
    
        /**
        * C++ default constructor.
        */        
        CHWRMVibraService(CHWRMVibraCommonService& aCommonService,
            CHWRMVibraCommonData& aVibraCommonData, TBool aPrivilegedClient);


        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CHWRMPluginHandler* aPluginHandler,
                        CHWRMReservationHandler* aReservationHandler);
        
        /**
        * Instructs plugin API to start vibra with specified intensity
        *
        * @param aDuration  Duration for which the vibra is to be turned on.
        * @param aIntensity Intensity of the vibra in decimal is -100 to 100,
        *                   which shows the percentage of the vibra motor full
        *                   rotation speed. When intensity is negative, 
        *                   the vibra motor rotates in the negative direction.
        *                   When intensity is positive, the vibra motor rotates
        *                   in the positive direction. Value 0 stops the vibra.
        *                   NOTE: The device might have hardware imposed limits
        *                         on supported vibra intensity values, so actual
        *                         effect might vary between different hardware.
        *                   NOTE2: Special value KUseDefaultIntensity (defined
        *                         in HWRMVibraService.cpp) can be used to
        *                         indicate that default intensity is to be used.
        * @param aMessage   Message related to request. Can have NULL handle for
        *                   restore/release/cleanup related requests.
    	* @param aSetTimer  If ETrue, call needs to set the timer. Some types of
    	*                   calls such as restoring last state should not reset
    	*                   timer should use EFalse.
    	* @param aPulse     If ETrue, vibra pulse is requested and new timer for
    	*                   stopping this request must be created. Original vibra 
    	*                   timer is left always intact, when this parameter ETrue.
        */
	    void StartVibraL(const TTimeIntervalMicroSeconds32& aDuration, 
	                     TInt aIntensity,
	                     const RMessage2& aMessage,
	                     TBool aSetTimer = ETrue,
	                     TBool aPulse = EFalse);
	    
        /**
        * Instructs plugin API to pulse vibra with specified intensity and duration
        *
        * @param aDuration  Duration for which the vibra is to be turned on.
        * @param aIntensity Intensity of the vibra in decimal is -100 to 100,
        *                   which shows the percentage of the vibra motor full
        *                   rotation speed. When intensity is negative, 
        *                   the vibra motor rotates in the negative direction.
        *                   When intensity is positive, the vibra motor rotates
        *                   in the positive direction. Value 0 stops the vibra.
        *                   NOTE: The device might have hardware imposed limits
        *                         on supported vibra intensity values, so actual
        *                         effect might vary between different hardware.
        *                   NOTE2: Special value KUseDefaultIntensity (defined
        *                         in HWRMVibraService.cpp) can be used to
        *                         indicate that default intensity is to be used.
        * @param aMessage   Message related to request. Can have NULL handle for
        *                   restore/release/cleanup related requests.
    	* @param aSetTimer  If ETrue, call needs to set the timer. Some types of
    	*                   calls such as restoring last state should not reset
    	*                   timer should use EFalse.
        */
	    void PulseVibraL(const TTimeIntervalMicroSeconds32& aDuration, 
	                     TInt aIntensity,
	                     const RMessage2& aMessage,
	                     TBool aSetTimer = EFalse);
	    

	    
	    /**
        * Interrupts the device vibration that is started with the StartVibra
        * method immediately.
        * @param aMessage   Message related to request. Can have NULL handle for
        *                   timer initiated requests
        * @param aForceStop If ETrue, stop is forced regardless of reservation
        *                   status.
        * @param aFinalStop If ETrue, indicates that this is final stop by 
        *                   destructor and it doesn't require callback.
        */		
        void StopVibraL(const RMessage2& aMessage, 
                        TBool aForceStop=EFalse,
                        TBool aFinalStop=EFalse); 
        
        /**
        * Changes the device vibration intensity to previous active intensity
        */		
        void ChangeVibraIntensityL();
        
        /**
        * Cleans up the vibra state
        */
        void CleanupVibra();

        /**
        * Cleans up buffered vibra pulse data objects
        */
        void ReleasePulseData();
        
        /**
        * Checks whether vibra pulse should be blocked
        */
        TBool VibraPulseBlocked();

    private:  // class
    
    /**
    *  Embedded class to contain pulse timer objects 
    *  and associated intensities for timer period.
    */
    class CPulseData : public CBase
        {
        public:     // Constructors and destructor

            /**
            * Two-phased constructor.
            * 
            * @param aVibraTimer    Vibra pulse timer
            * @param aLastIntensity Last "suspended" intensity
            * @param aIntensity     Intensity of this pulse request
            * @param aLastTimerId   Last activated timer id
            */
            static CPulseData* NewL( CHWRMGenericTimer& aVibraTimer,
                                     const TInt aLastIntensity, 
                                     const TInt aIntensity,
                                     const TInt aLastTimerId );

            /**
            * Destructor.
            */
            virtual ~CPulseData();
            
        public:     // New functions
        
            /**
            * Method to return the last intensity
            * 
            * @return   Last intensity
            */
            CHWRMGenericTimer& Timer() const;

            /**
            * Method to return the last intensity
            * 
            * @return   Last intensity
            */
            TInt LastIntensity() const;

            /**
            * Method to return this requests's intensity
            * 
            * @return   Intensity
            */
            TInt Intensity() const;
            
            /**
            * Method to set last intensity
            * 
            */
            void SetLastIntensity(TInt aIntensity);

            /**
            * Method to return last timer id
            * 
            * @return   Timer id
            */
            TInt LastTimerId() const;

            /**
            * Method to set last timer id
            * 
            */
            void SetLastTimerId(TInt aTimerId);

        private:    // New functions
        
            /**
            * C++ constructor.
            */
            CPulseData( CHWRMGenericTimer& aVibraTimer,
                        const TInt aLastIntensity, 
                        const TInt aIntensity,
                        const TInt aLastTimerId );
            
        private:    // Data
            
            CHWRMGenericTimer&  iVibraTimer;    // Timer
            TInt                iLastIntensity; // Vibra intensity, when this pulse timer started
            TInt                iIntensity;     // Vibra intensity of this pulse request        
            TInt                iLastTimerId;   // Timer id, which was active when pulse requested
        };


        /**
        * Cleans up the vibra state
        */
        //void CleanupVibra();

    private:  // data
    
        CHWRMVibraCommonData&       iVibraCommonData;    // Pointer to vibra data object common to all sessions. Not owned.                 
        CHWRMGenericTimer*          iVibraTimer;         // Cutoff timer. Owned.
        
        TBool                       iSuspended;          // Flag to indicate if this session is suspended.
        CHWRMVibra::TVibraStatus    iLastStatus;         // Indicates what state vibra should be on.
        TInt                        iLastIntensity;      // Indicates what intensity vibra should have (if status is on)
        
        CHWRMVibra::TVibraStatus    iFrozenStatus;       // Vibra status frozen at last release.
        TInt                        iFrozenIntensity;    // Vibra intensity frozen at last release.
        TTimeIntervalMicroSeconds32 iFrozenTime;         // Remaining vibra timer duration at last release.
        
        TBool                       iReserved;           // Flag to indicate if this session is reserved.
        TBool                       iPrivilegedClient;   // Flag to indicate if this session client is privileged
        
        TBool                       iCleanupDone;        // Flag to indicate if cleanup has been done.
        TBool                       iStateSetAfterReservation; // Flag to indicate that state has changed after reservation. 
                                                               // This is set to EFalse when reservation is activated.
	    CHWRMVibraCommonService&    iCommonService;      // Common vibra service for all sessions
        RPointerArray<CPulseData>   iPulseData;          // Pulse objects holding timers and associated intensities
        TSecureId                   iSid;                // SID of vibra using client.
        TInt                        iActiveTimerId;      // Id of active timer
                                                               
    };

#endif  // HWRMVIBRASERVICE_H
            
// End of File
