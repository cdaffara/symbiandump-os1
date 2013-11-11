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



#ifndef HWRMVIBRA_H
#define HWRMVIBRA_H

// INCLUDES
#include <e32base.h>

// CONSTANTS

/**
* Minimum allowed intensity setting for vibra. When intensity is negative, 
* the vibra motor rotates in the negative direction.

*
* @publishedAll
* @released
*/
const TInt KHWRMVibraMinIntensity = -100;

/**
* Minimum allowed intensity setting for vibra pulse.
*
* @publishedAll
* @released
*/
const TInt KHWRMVibraMinPulseIntensity = 1;


/**
* Maximum allowed intensity setting for vibra. When intensity is positive, 
* the vibra motor rotates in the positive direction. Value 0 effectively 
* stops the vibra.

* @publishedAll
* @released
*/
const TInt KHWRMVibraMaxIntensity = 100;

/**
* Maximum allowed duration value in milliseconds. Maximum vibrating time 
* supported by device is declared in KVibraCtrlMaxTime cenrep-key.

*
* Note that duration probably has device specific
* maximum duration that is much lower.
*
* @publishedAll
* @released
*/
const TInt KHWRMVibraMaxDuration = (KMaxTInt / 1000) - 1;

/**
* KHWRMVibraInfiniteDuration specifies, that vibrating should continue maximum 
* vibrating time supported by device if vibrating is not explicitly stopped.
*
*
* @publishedAll
* @released
*/
const TInt KHWRMVibraInfiniteDuration = 0;


class MHWRMVibraObserver;
class MHWRMVibraFeedbackObserver;

/**
* The class used to control the device vibra.
*
* HW Resource Manager Vibra API is a library API providing ability to control
* the device vibra. The API provides also methods to retrieve the current settings
* of the vibration feature in the user profile and the current status of the vibra. 
*
* The type of HW Resource Manager Vibra API is a synchronous method call meaning 
* the method call will block the client application. The API is meant for all 
* applications which need to control the device vibra.
*
* The API consist of the classes CHWRMVibra and MHWRMVibraObserver. If the client
* requires up-to-date status information, it should also provide callback pointer
* of the MHWRMVibraObserver implementing class for the NewL-method.
*
* Usage:
*
* @code
* #include <hwrmvibra.h> 
*
* // A CHWRMVibra instance can be created by using NewL() or NewLC() methods. 
* // Up-to-date status information not required, no callbacks.
* CHWRMVibra* vibra = CHWRMVibra::NewL();
*
* // After this, vibra can be directly controlled via the provided class methods. 
* vibra->StartVibraL(5000); // Start vibra for five seconds
* vibra->StopVibraL(); // Immediately stop vibra
*
* // To clean up, delete the created object:
* delete vibra;
* @endcode
*
* @publishedAll
* @released
*/
class CHWRMVibra : public CBase
    {
    public: // enums
        /**
        * Vibration setting in the user profile.
        */
        enum TVibraModeState 
            {
            /**
            Not initialized yet or there is an error condion.
            */
            EVibraModeUnknown = 0, 
            /**
            Vibration setting in the user profile is on.
            */
            EVibraModeON,          
            /**
            Vibration setting in the user profile is off.
            */
            EVibraModeOFF        
            };

        /**
        * Status of the vibration feature
        */
        enum TVibraStatus 
            {
            /**
            Vibra is not initialized yet or status is uncertain because of an error condition.
            */
            EVibraStatusUnknown = 0, 
            /**
            Vibra is set off in the user profile or some application is specifically blocking vibra.
            */ 
            EVibraStatusNotAllowed,  
            /**
            Vibra is stopped.
            */            
            EVibraStatusStopped,     
            /**
            Vibra is on.
            */
            EVibraStatusOn          
            };
        /**
        * Tactile feedback vibration setting in the user profile.
        */
        enum TVibraFeedbackModeState 
            {
            EVibraFeedbackModeUnknown = 0, ///< Not initialized yet or there is an error condion.
            EVibraFeedbackModeON,          ///< Feedback vibration setting in the user profile is on.
            EVibraFeedbackModeOFF          ///< Feedback vibration setting in the user profile is off.
            };            
    public:         
        /**
        * Two-phased constructor.
        *
        * @return A pointer to a new instance of the CHWRMVibra class.
        *
        * @leave KErrNotSupported Device doesn't support vibration feature.
        * @leave KErrNoMemory There is a memory allocation failure. 
        */
        IMPORT_C static CHWRMVibra* NewL();
        
        /**
        * Two-phased constructor.
        * Leaves instance to cleanup stack.
        *
        * @return A pointer to a new instance of the CHWRMVibra class.
        *
        * @leave KErrNotSupported Device doesn't support vibration feature.
        * @leave KErrNoMemory There is a memory allocation failure. 
        */
        IMPORT_C static CHWRMVibra* NewLC();

        /**
        * Two-phased constructor.
        * Use this method for creating a vibra client with callbacks.
        *
        * @param aCallback Pointer to callback instance
        * @return A pointer to a new instance of the CHWRMVibra class.
        *
        * @leave KErrNotSupported Device doesn't support vibration feature.
        * @leave KErrNoMemory There is a memory allocation failure. 
        */
        IMPORT_C static CHWRMVibra* NewL(MHWRMVibraObserver* aCallback);
        
        /**
        * Two-phased constructor. 
        * Use this method for creating a vibra client with callbacks.
        * Leaves instance to cleanup stack.
        *
        * @param aCallback Pointer to callback instance
        * @return A pointer to a new instance of the CHWRMVibra class.
        *
        * @leave KErrNotSupported Device doesn't support vibration feature.
        * @leave KErrNoMemory There is a memory allocation failure. 
        */
        IMPORT_C static CHWRMVibra* NewLC(MHWRMVibraObserver* aCallback);

    public: // New functions

    	/**
    	* Reserves vibration feature exclusively for this client.
    	* A higher priority client may cause lower priority client reservation
    	* to be temporarily suspended. Commands can still be issued in suspended 
    	* state, but they will not be acted upon unless suspension is lifted
    	* within specified duration.
    	* The suspended client will not get any notification about suspension.
    	* If vibra is already reserved by a higher or equal priority application, 
    	* reserving will still succeed, but reservation is immediately suspended.
    	*
    	* Calling this method is equal to call ReserveVibraL(EFalse, EFalse),
    	* i.e. any previously frozen state will not be restored and CCoeEnv
    	* background/foreground status is always used to control further reservations.
    	*
    	* @leave KErrAccessDenied No CCoeEnv present.
    	* @leave KErrNotReady Trying to reserve while on background.
        * @leave KErrNoMemory There is a memory allocation failure. 
    	*/
    	virtual void ReserveVibraL()=0;
    	
    	/**
    	* Reserves vibration feature exclusively for this client.
    	* A higher priority client may cause lower priority client reservation
    	* to be temporarily suspended. Commands can still be issued in suspended 
    	* state, but they will not be acted upon unless suspension is lifted
    	* within specified duration.
    	* The suspended client will not get any notification about suspension.
    	* If vibra is already reserved by a higher or equal priority application, 
    	* reserving will still succeed, but reservation is immediately suspended.
    	*
    	*
    	* @param aRestoreState If ETrue, the state frozen on last release will be
    	*                      restored upon successful reservation.
    	*                      I.e. if vibra was on when it was released by this 
        *                      client the last time, it would continue the vibrating
        *                      upon successful reservation.
    	*                      For the first reservation of each session this 
        *                      parameter is always considered EFalse regardless of 
        *                      what is supplied, as there is no previous frozen state
        *                      to restore.
    	* @param aForceNoCCoeEnv If EFalse, then reservation requires that this client
        *                        has the keyboard focus at the time of reservation and
        *                        vibra will be automatically released and re-reserved 
        *                        based on the keyboard focus status of this client. 
        *                        This also implies that CCoeEnv::Static() != NULL is 
        *                        required.
    	*                        If ETrue, the client will not require CCoeEnv to be 
        *                        present nor does it automatically reserve/release vibra 
    	*                        by depending on foreground/background status of the 
        *                        client.
    	*                        Only trusted clients are allowed to set this flag to
        *                        ETrue. A client is considered trusted if it has nonstandard
        *                        priority defined in the internal vibra policy of the 
        *                        HW Resource Manager. A client can be defined trusted
        *                        only by a product.
    	*
    	* @leave KErrAccessDenied Parameter aForceNoCCoeEnv is ETrue and client is not
    	*                         trusted.
    	* @leave KErrBadHandle Parameter aForceNoCCoeEnv is EFalse and no CCoeEnv present.
    	* @leave KErrNotReady Trying to reserve while on background and parameter 
        *                     aForceNoCCoeEnv is EFalse.
        * @leave KErrNoMemory There is a memory allocation failure. 
    	*/
    	virtual void ReserveVibraL(TBool aRestoreState, TBool aForceNoCCoeEnv)=0;

    	/**
    	* Releases vibration feature if it was previously reserved for this client.
    	* If this client has not reserved vibration feature, does nothing.
    	* If vibra is on when it is released and no other client has a suspended 
    	* reservation, vibra is stopped.
    	*/
    	virtual void ReleaseVibra()=0;


        /**
        * Starts the device vibration feature with the product specific default
        * intensity.
        * If StartVibraL is called again before the first vibration completes
        * then the first vibration is interrupted and the second vibrations
        * starts immediately -- i.e. The periods of vibration are not cumulative.
        *
        * The vibration can be interrupted with the method StopVibraL before
        * the specified interval has elapsed. 
        *
        * Vibra settings of the vibration feature in the user profile 
        * must be active. 
        *
        * Note: The device may have implementation defined or hardware imposed
        *       limits to the duration of the vibration feature. In such 
        *       circumstances any vibration will cut off at that limit even if
        *       the duration parameter is greater than the limit.
        *
        * @param aDuration Duration of the vibration measured in milliseconds. 
        *                  A value of 0 specifies that the vibration should
        *                  continue indefinetely and should be stopped with a 
        *                  call to StopVibraL. Duration can have maximum value
        *                  of KHWRMVibraMaxDuration.
        *
        * @leave KErrArgument Duration is invalid.
        * @leave KErrAccessDenied Vibration setting in the user profile is not set.
        * @leave KErrBadHandle Vibra session has been invalidated.
        * @leave KErrLocked Vibra is locked down because too much continuous use
        *                   or explicitly blocked by for example some vibration 
        *                   sensitive accessory.
        * @leave KErrTimedOut Timeout occurred in controlling vibra.
        * @leave KErrInUse Vibra is not reserved to this client but it is
        *                  reserved to some other client.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
        *
        * @see MHWRMVibraObserver
        */
        virtual void StartVibraL(TInt aDuration)=0;
		
        /**
        * Starts the device vibration feature. If StartVibraL is called again before
        * the first vibration completes then the first vibration is interrupted
        * and the second vibrations starts immediately -- i.e. The periods of
        * vibration are not cumulative.
        *
        * The vibration can be interrupted with the method StopVibraL before
        * the specified interval has elapsed.
        *
        * Vibra settings of the vibration feature in the user profile 
        * must be active. 
        *
        * Note: The device may have implementation defined or hardware imposed
        *       limits to the duration of the vibration feature. In such 
        *       circumstances any vibration will cut off at that limit even if
        *       the duration parameter is greater than the limit.
        *
        * @param aDuration Duration of the vibration measured in milliseconds. 
        *                  A value of 0 specifies that the vibration should
        *                  continue indefinitly and should be stopped with a 
        *                  call to StopVibraL.  Duration can have maximum value
        *                  of KHWRMVibraMaxDuration.
        * @param aIntensity Intensity of the vibra in decimal is -100 to 100,
        *                   which shows the percentage of the vibra motor full
        *                   rotation speed. When intensity is negative, 
        *                   the vibra motor rotates in the negative direction.
        *                   When intensity is positive, the vibra motor rotates
        *                   in the positive direction. Value 0 stops the vibra.
        *                   NOTE: The device might have hardware-imposed limits
        *                         on supported vibra intensity values, so actual
        *                         effect might vary between different hardware.
        *
        * @leave KErrNotSupported The device doesn't support user-defined 
        *                         vibra intensity.
        * @leave KErrArgument One of the parameters is out of range.
        * @leave KErrAccessDenied Vibration setting in the user profile
        *                         is not set.
        * @leave KErrBadHandle Vibra session has been invalidated.
        * @leave KErrLocked Vibra is locked down because too much continuous use
        *                   or explicitly blocked by for example some vibration
        *                   sensitive accessory.
        * @leave KErrTimedOut Timeout occurred in controlling vibra.
        * @leave KErrInUse Vibra is not reserved to this client but it is
        *                  reserved to some other client.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
        *
        * @see MHWRMVibraObserver
        */
	    virtual void StartVibraL(TInt aDuration, TInt aIntensity)=0;
	    
	    /**
        * Interrupts the device vibration that is started with the StartVibraL
        * method immediately.
        *
        * @leave KErrBadHandle Vibra session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling vibra.
        * @leave KErrInUse Vibra is not reserved to this client but it is
        *                  reserved to some other client.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
        *
        * @see MHWRMVibraObserver
        */		
        virtual void StopVibraL()=0; 
		
        /**
        * This method retrieves the current settings of the vibration feature
        * in the user profile. The developer can check the Vibra settings 
        * in the profile and if there is no Vibra active but it is needed by 
        * the client application then the user can be informed.
        *
        * @return TVibraModeState indicating the current vibra mode setting.
        *
        * @see TVibraModeState
        * @see MHWRMVibraObserver
        */
        virtual TVibraModeState VibraSettings() const=0;

        /**
        * This method retrieves the current vibra status. 
        *
        * @return TVibraStatus indicating the current vibra status
        * 
        * @see TVibraStatus
        * @see MHWRMVibraObserver
        */
        virtual TVibraStatus VibraStatus() const=0;
        
        /**
        * This method is intended only for firmware build time configured 
        * privileged clients.
        *
        * Executes a tactile feedback vibration pulse with product 
        * specific default intensity and duration.
        * If PulseVibraL is called before ongoing vibration completes and 
        * PulseVibraL calling client has higher priority than executing client,
        * pulse request is accepted. Also possible vibra-reservations are bypassed.
        * If client calling PulseVibraL has lower or equal priority 
        * than executing client, ongoing vibration is not affected.
        *
        * Tactile feedback vibration settings of the vibration feature in the 
        * user profile must be active. 
        *
        * Note: The device may have implementation defined or hardware imposed
        *       limits to the duration of the vibration feature. In such 
        *       circumstances any vibration will cut off at that limit even if
        *       the duration parameter is greater than the limit.
        *
        * @param aDuration Duration of the vibration measured in milliseconds. 
        *                  Duration can have maximum value
        *                  of KHWRMVibraMaxDuration.
        *
        * @leave KErrAccessDenied Vibration setting in the user profile is not set
        *                         or client is not privileged to use pulse feature. 
        * @leave KErrBadHandle Vibra session has been invalidated.
        * @leave KErrLocked Vibra is locked down because too much continuous use
        *                   or explicitly blocked by for example some vibration 
        *                   sensitive accessory.
        * @leave KErrTimedOut Timeout occurred in controlling vibra.
        * @leave KErrInUse Vibra is not reserved to this client but it is
        *                  reserved to some other client or ongoing vibration
        *                  has been requested by higher priority client.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
        *
        * @see MHWRMVibraFeedbackObserver
        */
        virtual void PulseVibraL()=0;
		
        /**
        * This method is intended only for firmware build time configured 
        * privileged clients.
        *
        * Executes a tactile feedback vibration pulse with product 
        * specific default intensity and specified duration.
        * If PulseVibraL is called before ongoing vibration completes and 
        * PulseVibraL calling client has higher priority than executing client,
        * pulse request is accepted. Also possible vibra-reservations are bypassed.
        * If client calling PulseVibraL has lower or equal priority 
        * than executing client, ongoing vibration is not affected.
        *
        * Tactile feedback vibration settings of the vibration feature in the 
        * user profile must be active. 
        *
        * Note: The device may have implementation defined or hardware imposed
        *       limits to the duration of the vibration feature. In such 
        *       circumstances any vibration will cut off at that limit even if
        *       the duration parameter is greater than the limit.
        *
        * @param aDuration Duration of the vibration measured in milliseconds. 
        *                  Duration can have maximum value
        *                  of KHWRMVibraMaxDuration.
        *
        * @leave KErrArgument One of the parameters is out of range.
        * @leave KErrAccessDenied Vibration setting in the user profile is not set
        *                         or client is not privileged to use pulse feature. 
        * @leave KErrBadHandle Vibra session has been invalidated.
        * @leave KErrLocked Vibra is locked down because too much continuous use
        *                   or explicitly blocked by for example some vibration 
        *                   sensitive accessory.
        * @leave KErrTimedOut Timeout occurred in controlling vibra.
        * @leave KErrInUse Vibra is not reserved to this client but it is
        *                  reserved to some other client or ongoing vibration
        *                  has been requested by higher priority client.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
        *
        * @see MHWRMVibraFeedbackObserver
        */
        virtual void PulseVibraL(TInt aDuration)=0;
		
        /**
        * This method is intended only for firmware build time configured 
        * privileged clients.
        *
        * Executes a tactile feedback vibration pulse.
        * If PulseVibraL is called before ongoing vibration completes and 
        * PulseVibraL calling client has higher priority than executing client,
        * pulse request is accepted. Also possible vibra-reservations are bypassed.
        * If client calling PulseVibraL has lower or equal priority 
        * than executing client, ongoing vibration is not affected.
        *
        * Tactile feedback vibration settings of the vibration feature in the 
        * user profile must be active. 
        *
        * Note: The device may have implementation defined or hardware imposed
        *       limits to the duration of the vibration feature. In such 
        *       circumstances any vibration will cut off at that limit even if
        *       the duration parameter is greater than the limit.
        *
        * @param aDuration Duration of the vibration measured in milliseconds. 
        *                  Duration can have maximum value
        *                  of KHWRMVibraMaxDuration.
        * @param aIntensity Intensity of the pulse in decimal is KHWRMVibraMinPulseIntensity
        *                   to KHWRMVibraMaxIntensity, which shows the percentage 
        *                   of the vibra motor full rotation speed. 
        *                   NOTE: The device might have hardware-imposed limits
        *                         on supported vibra intensity values, so actual
        *                         effect might vary between different hardware.
        * @leave KErrNotSupported The device doesn't support user-defined 
        *                         vibra intensity.
        *
        * @leave KErrArgument One of the parameters is out of range.
        * @leave KErrAccessDenied Vibration setting in the user profile is not set
        *                         or client is not privileged to use pulse feature. 
        * @leave KErrBadHandle Vibra session has been invalidated.
        * @leave KErrLocked Vibra is locked down because too much continuous use
        *                   or explicitly blocked by for example some vibration 
        *                   sensitive accessory.
        * @leave KErrTimedOut Timeout occurred in controlling vibra.
        * @leave KErrInUse Vibra is not reserved to this client but it is
        *                  reserved to some other client or ongoing vibration
        *                  has been requested by higher priority client.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
        *
        * @see MHWRMVibraFeedbackObserver
        */
        virtual void PulseVibraL(TInt aDuration, TInt aIntensity)=0;
		
        /**
        * Use this method for setting feedback observer.
        *
        * @param aCallback Pointer to callback instance
        */
        virtual void SetFeedbackObserver(MHWRMVibraFeedbackObserver* aCallback)=0;

        /**
        * This method retrieves the current settings of the feedback vibration feature
        * in the user profile. The developer can check the feedback vibration settings 
        * in the profile and if there is no feedback vibration active but it is needed by 
        * the client application then the user can be informed. However, client needs to 
        * explicitly register to listen these changes via SetFeedbackObserver-method.
        *
        * @return TVibraFeedbackModeState indicating the current vibra feedback mode setting.
        *
        * @see TVibraFeedbackModeState
        * @see MHWRMVibraFeedbackObserver
        */
        virtual TVibraFeedbackModeState VibraFeedbackSettings() const=0;
    };
    
/**
* A callback interface for vibra status reporting.
*
* If the client requires up-to-date status information, the client needs 
* to derive a class from the MHWRMVibraObserver interface and implement 
* the VibraModeChanged() and VibraStatusChanged() methods. 
* 
* A callback object header example:
*
* @code 
* // INCLUDES
* #include <hwrmvibra.h> // Link against HWRMVibraClient.lib.
*
* class CTest : public CBase, 
*               public MHWRMVibraObserver    
*    {
*    public:
*        CTest();
*        ~CTest();
*                       
*        void ConstructL();
*        static CTest* NewL();
*                
*        // from MHWRMVibraObserver
*        virtual void VibraModeChanged(CHWRMVibra::TVibraModeState aStatus);
*        virtual void VibraStatusChanged(CHWRMVibra::TVibraStatus aStatus);
*
*    private:
*        CHWRMVibra* iVibra;
*    };
* @endcode
*
* A callback method implementation example:
*
* @code
* void CTest::VibraStatusChanged(CHWRMVibra::TVibraStatus aStatus)
*    {
*    switch ( aStatus )
*        {
*        case CHWRMVibra::EVibraStatusUnknown:
*            RDebug::Print(_L("### Vibra state changed: EVibraStatusUnknown"));
*            break;
*        case CHWRMVibra::EVibraStatusNotAllowed:
*            RDebug::Print(_L("### Vibra state changed: EVibraStatusNotAllowed"));
*            break;
*        case CHWRMVibra::EVibraStatusStopped:
*            RDebug::Print(_L("### Vibra state changed: EVibraStatusStopped"));
*            break;
*        case CHWRMVibra::EVibraStatusOn:
*            RDebug::Print(_L("### Vibra state changed: EVibraStatusOn"));
*            break;
*        default:
*            RDebug::Print(_L("### Vibra state changed: UNDEFINED !"));
*            break;
*        }
*    }
* @endcode
*
* @publishedAll
* @released
*/
class MHWRMVibraObserver
    {    
    public:
        
        /** 
        * Called when the vibration setting in the user profile is changed.
        *
        * @param aStatus Indicates the new setting.
        *
        * @see CHWRMVibra::TVibraModeState
        */
        virtual void VibraModeChanged(CHWRMVibra::TVibraModeState aStatus) = 0;
        
        /** 
        * Called when the device vibration feature state changes
        *
        * @param aStatus Indicates vibra status.
        *
        * @see CHWRMVibra::TVibraStatus
		*/
        virtual void VibraStatusChanged(CHWRMVibra::TVibraStatus aStatus) = 0;
	};

/**
*
* @publishedAll
* @released
*/
class MHWRMVibraFeedbackObserver
    {    
    public:
        
        /** 
        * Called when the tactile feedback vibration setting in the user profile is changed.
        *
        * @param aMode Indicates the new setting.
        *
        * @see CHWRMVibra::TVibraFeedbackModeState
        */
        virtual void VibraFeedbackModeChanged(CHWRMVibra::TVibraFeedbackModeState aMode) = 0;
	};


#endif      // HWRMVIBRA_H   
            
// End of File
