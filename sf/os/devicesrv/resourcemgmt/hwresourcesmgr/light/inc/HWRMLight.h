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




#ifndef HWRMLIGHT_H
#define HWRMLIGHT_H

// INCLUDES
#include <e32base.h>

// CONSTANTS

/**
* Minimum allowed intensity setting for Light.
*
* @publishedAll
* @released
*/
const TInt KHWRMLightMinIntensity = 1;

/**
* Maximum allowed intensity setting for Light.
*
* @publishedAll
* @released
*/
const TInt KHWRMLightMaxIntensity = 100;

/**
* Indicates default intensity in various methods.
*
* @publishedAll
* @released
*/
const TInt KHWRMDefaultIntensity = 0;

/**
* Maximum allowed duration value.
*
* @publishedAll
* @released
*/
const TInt KHWRMLightMaxDuration = (KMaxTInt / 1000) - 1;

/**
* Infinite duration value.
*
* @publishedAll
* @released
*/
const TInt KHWRMInfiniteDuration = 0;

/**
* Indicates device default Blink cycle time.
*
* @publishedAll
* @released
*/
const TInt KHWRMDefaultCycleTime = 0;


class MHWRMLightObserver;
class CHWRMEnhancedLight;

/**  
* RGB values.
*
* @publishedAll
*/

struct THWRMLightColor
{
    TUint8 iRed;
    TUint8 iGreen;
    TUint8 iBlue;
};


/**
* The class used to control the device lights.
*
* The HW Resource Manager Light API is a library API providing the ability 
* to control the various light targets of the device. The API provides also
* methods to retrieve the current light status and the supported light targets
* of the device. The API is meant for all applications which need to control 
* lights of the device.
*
* Type of the HW Resource Manager Light API is a synchronous method call meaning 
* the method call will block the client application. Every new call of the light
* API method stops all ongoing light control orders. Light state after duration
* based orders expire is the state specified by the last non-duration based order. 
*
* The API consist of the classes CHWRMLight and MHWRMLightObserver. If the client
* requires up-to-date status information, it should also provide callback pointer
* of the MHWRMLightObserver implementing class for the NewL-method.
*
* Usage:
*
* @code
* #include <hwrmlight.h> 
*
* // A CHWRMLight instance can be created by using NewL() or NewLC() methods. 
* // Up-to-date status information not required, no callbacks.
* CHWRMLight* light = CHWRMLight::NewL();
*
* // After this, lights can be directly controlled via the provided class methods. 
* light-> LightOnL (EPrimaryDisplay, 5000); // Turn display lights on for five seconds.
* light->LightOffL(EPrimaryDisplay); // Turn display lights off indefinitely.
*
* // To clean up, delete the created object:
* delete light;
* @endcode
*
* @publishedAll
* @released
*/
class CHWRMLight : public CBase
    {
    public:

		/**
		* Possible light states that can be get for the different light targets
		*/
		enum TLightStatus
			{
			/**
			For debugging/development and signaling an error conditions.
			*/
			ELightStatusUnknown = 0,
			/**
			Light state switch to light on.
			*/
			ELightOn,              
			/**
			Light state switch to light off.
			*/
			ELightOff,             
			/**
			Light state switch to light blinking.
			*/
			ELightBlink            
			};

        /**
        * Possible light targets. 
        * Targets can be used as bitmask. Some common masks are provided as enum.
        * 
        * Note that all targets are not supported by all devices.
        * Attempting to use unsupported target will result in
        * KErrNotSupported.
        *
        * At least one target must be defined.
        */
        enum TLightTarget
            {
            /**
            No target. Not a valid target value, used only for error checking.
            */
            ENoTarget                    = 0x0,    
            /**
            Primary display of the device.
            */            
            EPrimaryDisplay              = 0x1,     
            /**
            Primary keyboard of the device. 
            */
            EPrimaryKeyboard             = 0x2,     
            /**
            Both primary display and the primary keyboard of the device.  
            */
            EPrimaryDisplayAndKeyboard   = 0x3,                
            /**
            Secondary display of the device.
            */
            ESecondaryDisplay            = 0x4,     
            /**
            Secondary keyboard of the device. 
            */
            ESecondaryKeyboard           = 0x8,     
            /**
            Both secondary display and the secondary keyboard of the device.  
            */
            ESecondaryDisplayAndKeyboard = 0xC,          
            /**
            Device specific custom target 1.
            */
            ECustomTarget1               = 0x10, 
            /**
            Device specific custom target 2.
            */
            ECustomTarget2               = 0x20, 
            /**
            Device specific custom target 3.
            */
            ECustomTarget3               = 0x40, 
            /**
            Device specific custom target 4.
            */
            ECustomTarget4               = 0x80,             
            /**
            * Special target used to control all currently available system lights.
            *
            * System lights normally include all displays and keyboards, 
            * but not custom lights. This is however device dependent.
            *
            * A target mask including this target is 
            * always changed to a device state specific target mask. 
            * Note that the  system target with any other target is not supported. 
            *
            * This target is always supported but it is never
            * included in supported targets mask.
            *
            * @see CHWRMLight::SupportedTargets()
            */
            ESystemTarget                = 0x80000000  
            };
            
    public:  // Constructors
        
        /**
        * Two-phased constructor.
        *
        * @return A pointer to a new instance of the CHWRMLight class.
        *
        * @leave KErrNotSupported Device doesn't support Light feature.
        * @leave KErrNoMemory There is a memory allocation failure. 
        */
        IMPORT_C static CHWRMLight* NewL();
        
        /**
        * Two-phased constructor. 
        * Leaves instance to cleanup stack.
        *
        * @return A pointer to a new instance of the CHWRMLight class.
        *
        * @leave KErrNotSupported Device doesn't support Light feature.
        * @leave KErrNoMemory There is a memory allocation failure. 
        */
        IMPORT_C static CHWRMLight* NewLC();

        /**
        * Two-phased constructor.
        * Use this method for creating a Light client with callbacks.
        *
        * @param aCallback Pointer to the callback instance.
        * @return A pointer to a new instance of the CHWRMLight class.
        *
        * @leave KErrNotSupported Device doesn't support Light feature.
        * @leave KErrNoMemory There is a memory allocation failure. 
        */
        IMPORT_C static CHWRMLight* NewL(MHWRMLightObserver* aCallback);
        
        /**
        * Two-phased constructor. 
        * Use this method for creating a Light client with callbacks.
        * Leaves instance to cleanup stack.
        *
        * @param aCallback Pointer to the callback instance
        * @return A pointer to a new instance of the CHWRMLight class.
        *
        * @leave KErrNotSupported Device doesn't support Light feature.
        * @leave KErrNoMemory There is a memory allocation failure. 
        */
        IMPORT_C static CHWRMLight* NewLC(MHWRMLightObserver* aCallback);

		
		/**
		* Destructor
		*/
		IMPORT_C ~CHWRMLight();
		
    public: // New functions

    	
    	/**
    	* Reserves light target exclusively for this client.
    	* A higher priority client may cause lower priority client reservation
    	* to be temporarily suspended. Commands can still be issued in suspended 
    	* state, but they will not be acted upon unless suspension is lifted
    	* within specified duration.
    	* The suspended client will not get any notification about suspension.
    	* If light target is already reserved by a higher or equal priority application, 
    	* reserving will still succeeds, but reservation is immediately suspended.
    	*
    	* Calling this method is equal to calling ReserveLightL( aTarget, EFalse, EFalse),
    	* i.e. any previously frozen state will not be restored and CCoeEnv
    	* background/foreground status is always used to control further reservations.
    	*
		* @param aTarget Defines which light should be reserved. Multiple lights can
		*                be specified with using bitwise-or.
    	*
    	* @leave KErrNotSupported One or more of specified targets are not supported.
    	* @leave KErrAccessDenied No CCoeEnv present.
    	* @leave KErrNotReady Trying to reserve while on background.
        * @leave KErrNoMemory There is a memory allocation failure. 
		*
	 	* @see TLightTarget
    	*/
    	virtual void ReserveLightL(TInt aTarget);
    	
    	/**
    	* Reserves light target exclusively for this client.
    	* A higher priority client may cause lower priority client reservation
    	* to be temporarily suspended. Commands can still be issued in suspended 
    	* state, but they will not be acted upon unless suspension is lifted
    	* within specified duration.
    	* The suspended client will not get any notification about suspension.
    	* If light target is already reserved by a higher or equal priority application, 
    	* reserving will still succeeds, but reservation is immediately suspended.
    	*
    	*
		* @param aTarget Defines which light should be reserved. Multiple lights can
		*                be specified with using bitwise-or.
    	* @param aRestoreState If ETrue, the state frozen on last release will be
    	*                      restored upon successful reservation.
    	*                      I.e. if light was blinking when it was released by this
        *                      client the last time, it would start blinking again upon
        *                      successful reservation.
    	*                      For the first reservation of each session this parameter 
        *                      is always considered EFalse regardless of what is supplied,
        *                      as there is no previous frozen state to restore.
    	* @param aForceNoCCoeEnv If EFalse, then reservation requires that this client is
        *                        on the foreground at the time of reservation and light
        *                        target will be automatically released and re-reserved based
        *                        on background/foreground status of the this client. This 
        *                        also implies that CCoeEnv::Static() != NULL is required.
    	*                        If ETrue, the client will not require CCoeEnv to be present
        *                        nor does it automatically reserve/release light by depending
        *                        on foreground/background status of the client.
        *                        Only trusted clients are allowed to set this flag to ETrue.
    	*                        A client is considered trusted if it has nonstandard
        *                        priority defined in the internal lights policy of the 
        *                        HW Resource Manager. A client can be defined trusted
        *                        only by a product.
    	*
    	* @leave KErrNotSupported One or more of specified targets are not supported.
    	* @leave KErrAccessDenied Paramenter aForceNoCCoeEnv is ETrue and client is not trusted.
    	* @leave KErrBadHandle Parameter ForceNoCCoeEnv is EFalse and no CCoeEnv present.
    	* @leave KErrNotReady Trying to reserve while on background and parameter 
        *                     aForceNoCCoeEnv is EFalse.
        * @leave KErrNoMemory There is a memory allocation failure. 
		*
	 	* @see TLightTarget
    	*/
    	virtual void ReserveLightL(TInt aTarget, TBool aRestoreState, TBool aForceNoCCoeEnv);

    	/**
    	* Releases light target if it was previously reserved for this client.
    	* If this client has not reserved any of the specified lights, 
    	* this method does nothing.
    	* Any reserved light targets that are released and have no other suspended
    	* clients will be reset to default state, which is either lights on or lights off, 
    	* depending on system inactivity time.
    	*
		* @param aTarget Defines which light should be released. Multiple lights can
		*                be specified with using bitwise-or.
		*
	 	* @see TLightTarget
    	*/
    	virtual void ReleaseLight(TInt aTarget);


	    /** 
	    * The LightOnL method switches the specified target light on
	    * for infinite duration using default intensity. Lights will use fade-in.
        *
        * Calling this method is equal to calling 
        * LightOnL(aTarget, KHWRMInfiniteDuration, KHWRMDefaultIntensity, ETrue).
	    *
		* @param aTarget Defines which light should be controlled. Multiple lights can
		*                be specified with using bitwise-or.
		*
    	* @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOnL(TInt aTarget);

	    /** 
	    * The LightOnL method switches the specified target light on
	    * for the specified duration using default intensity. Lights will use fade-in.
        *
        * Calling this method is equal to call 
        * LightOnL(aTarget, aDuration, KHWRMDefaultIntensity, ETrue).
	    *
		* @param aTarget Defines which light should be controlled. Multiple lights can
		*                be specified with using bitwise-or.
		* @param aDuration Duration of the time the light is switched on measured in milliseconds.
		*                  After the duration expires, the light state for target will be changed 
		*                  to whatever state was caused by the last infinite time duration call, or
		*                  default state determined by inactivity timer, in case there has not 
		*                  been a previous infinite time duration call in this session.
		*                  If the aDuration time is KHWRMInfiniteDuration then it means an 
        *                  infinite value that has to be stopped by calling of any of the other
        '                  light control methods.
		*                  Duration can have maximum value of KHWRMLightMaxDuration.
		*
        * @leave KErrArgument Parameter aDuration is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOnL(TInt aTarget, 
		                      TInt aDuration);

	    /** 
	    * The LightOnL method switches the specified target light on
	    * for the specified duration using specified intensity. Fade-in can also be controlled.
	    *
		* @param aTarget Defines which light should be controlled. Multiple lights can
		*                be specified with using bitwise-or.
		* @param aDuration Duration of the time the light is switched on measured in milliseconds.
		*                  After the duration expires, the light state for target will be changed 
		*                  to whatever state was caused by the last infinite time duration call, or
		*                  default state determined by inactivity timer, in case there has not 
		*                  been a previous infinite time duration call in this session.
		*                  If the aDuration time is KHWRMInfiniteDuration then it means 
        *                  an infinite value that has to be stopped by calling of any of 
        *                  the other light control methods.
		*                  Duration can have maximum value of KHWRMLightMaxDuration.
		* @param aIntensity Intensity of the light. If aIntensity is KHWRMDefaultIntensity, device default 
		*                   intensity will be used. 
		*                   Note: All devices might not support user defined intensity, in which case
		*                   device will behave in its default fashion.
		* @param aFadeIn If ETrue, lights will not turn on instantly but instead smoothly fade-in.
		*                Note: All devices will not support fade-in, in which case device will
		*                behave in its default fashion.
		*
        * @leave KErrArgument One of the parameters is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOnL(TInt aTarget, 
		                      TInt aDuration, 
		                      TInt aIntensity,
		                      TBool aFadeIn);

	    /** 
	    * The LightBlinkL method blinks the target light(s) of the device for infinite duration
	    * using default intensity.
        *
        * Calling this method is equal to call 
        * @code
        * LightBlinkL(aTarget, KHWRMInfiniteDuration, KHWRMDefaultCycleTime, 
        *             KHWRMDefaultCycleTime, KHWRMDefaultIntensity).
	   	* @endcode
	   	*
		* @param aTarget Defines which light should be controlled. Multiple lights can
		*                be specified with using bitwise-or.
	   	*
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightBlinkL(TInt aTarget);

	    /** 
	    * The LightBlinkL method blinks the target light(s) of the device for specified duration
	    * using default intensity.
        *
        * Calling this method is equal to calling
        * LightBlinkL(aTarget, aDuration, KHWRMDefaultCycleTime, 
        *             KHWRMDefaultCycleTime, KHWRMDefaultIntensity).
	   	*
		* @param aTarget Defines which light should be controlled. Multiple lights can
		*                be specified with using bitwise-or.
		* @param aDuration Duration of the time the light is set to blink measured in milliseconds.
		*                  After the duration expires, the light state for target will be changed 
		*                  to whatever state was caused by the last infinite time duration call, or
		*                  default state determined by inactivity timer, in case there has not 
		*                  been a previous infinite time duration call in this session.
		*                  If the aTotalDuration time is KHWRMInfiniteDuration then it
		*                  means an infinite value that has to be
		*                  stopped by calling of any of the other light control methods.
		*                  Duration can have maximum value of KHWRMLightMaxDuration.
	   	*
        * @leave KErrArgument Parameter aDuration is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightBlinkL(TInt aTarget, 
		                         TInt aDuration);

	    /** 
	    * The LightBlinkL method blinks the target light(s) of the device for specified duration
	    * using specified intensity. On- and Off-cycle times of the blinking can also be controlled.
	   	*
		* @param aTarget Defines which light should be controlled. Multiple lights can
		*                be specified with using bitwise-or.
		* @param aDuration Duration of the time the light is set to blink measured in milliseconds.
		*                  After the duration expires, the light state for target will be changed 
		*                  to whatever state was caused by the last infinite time duration call, or
		*                  default state determined by inactivity timer, in case there has not 
		*                  been a previous infinite time duration call in this session.
		*                  If the aTotalDuration time is KHWRMInfiniteDuration then it
		*                  means an infinite value that has to be
		*                  stopped by calling of any of the other light control methods.
		*                  Duration can have maximum value of KHWRMLightMaxDuration.
	   	* @param aOnDuration Duration time, measured in milliseconds, of how long the Light is
	   	*                    switched on in every Blink cycle.
		*                    Duration can have maximum value of KHWRMLightMaxDuration.
		*                    For device default cycle duration, use value KHWRMDefaultCycleTime.
		*                    If either of aOnDuration or aOffDuration is KHWRMDefaultCycleTime,
		*                    both must be KHWRMDefaultCycleTime.
		*                    Some devices might not support variable blink cycle times, in which
		*                    case default value will be substituted.
	   	* @param aOffDuration Duration time, measured in milliseconds, of how long the Light
	   	*                     is switched off in every Blink cycle.
		*                     Duration can have maximum value of KHWRMLightMaxDuration.
        *                     For device default cycle duration, use value KHWRMDefaultCycleTime.
        *                     If either of aOnDuration or aOffDuration is KHWRMDefaultCycleTime,
		*                     both must be KHWRMDefaultCycleTime.
		*                     Some devices might not support variable blink cycle times, in which
		*                     case default value will be substituted.
		* @param aIntensity Intensity of the light. If aIntensity is KHWRMDefaultIntensity, device default 
		*                   intensity will be used.
		*                   Note: All devices might not support user defined intensity, in which case
		*                   device will behave in its default fashion.
	   	*
        * @leave KErrArgument One of the parameters is out of range or otherwise invalid.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightBlinkL(TInt aTarget, 
		                         TInt aDuration, 
		                         TInt aOnDuration, 
		                         TInt aOffDuration, 
		                         TInt aIntensity);

   	    /**
	    * The LightOffL method switches the device light off for the specified target for
	    * infinite duration. Lights will be switched off with fade-out. 
        *
        * Calling this method is equal to call 
        * LightOffL(aTarget, KHWRMInfiniteDuration, ETrue).
		*
		* @param aTarget Defines which light should be controlled. Multiple lights can
		*                be specified with using bitwise-or.
		*
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOffL(TInt aTarget);

   	    /**
	    * The LightOffL method switches the device light off for the specified target for
	    * the specified duration time. Lights will be switched off with fade-out.
        *
        * Calling this method is equal to call LightOffL(aTarget, aDuration, ETrue).
		*
		* @param aTarget Defines which light should be controlled. Multiple lights can
		*                be specified with using bitwise-or.
		* @param aDuration Duration of the time the light is switched off measured in milliseconds.
		*                  After the duration expires, the light state for target will be changed 
		*                  to whatever state was caused by the last infinite time duration call, or
		*                  default state determined by inactivity timer, in case there has not 
		*                  been a previous infinite time duration call in this session.
		*                  If the aDuration time is KHWRMInfiniteDuration then it 
		*                  means an infinite value that has to be
		*                  stopped by calling of any of the other light control methods.
		*                  Duration can have maximum value of KHWRMLightMaxDuration.
		*
        * @leave KErrArgument Parameter aDuration is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOffL(TInt aTarget, 
		                       TInt aDuration);

   	    /**
	    * The LightOffL method switches the device light off for the specified target for
	    * the specified duration time. Lights fade-out can also be controlled.
		*
		* @param aTarget Defines which light should be controlled. Multiple lights can
		*                be specified with using bitwise-or.
		* @param aDuration Duration of the time the light is switched off measured in milliseconds.
		*                  After the duration expires, the light state for target will be changed 
		*                  to whatever state was caused by the last infinite time duration call, or
		*                  default state determined by inactivity timer, in case there has not 
		*                  been a previous infinite time duration call in this session.
		*                  If the aDuration time is KHWRMInfiniteDuration then it 
		*                  means an infinite value that has to be
		*                  stopped by calling of any of the other light control methods.
		*                  Duration can have maximum value of KHWRMLightMaxDuration.
		* @param aFadeOut If ETrue, lights will not turn off instantly but instead smoothly fade-out
		*                 Note: All devices will not support fade-out, in which case device will
		*                 behave in its default fashion.
		*
        * @leave KErrArgument aDuration is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOffL(TInt aTarget, 
		                       TInt aDuration, 
		                       TBool aFadeOut);
		                       
        /**
        * This method retrieves the current light status. 
        *
		* @param aTarget Defines which light status is returned. 
		*                This method only supports single target, as different
		*                targets might have different statuses.
        * @return TLightStatus indicating the current light status. If there is a problem or
        *         multiple targets were specified, CHWRMLight::ELightStatusUnknown is returned.
        * 
        * @see MHWRMLightObserver
	 	* @see TLightTarget
        */
        virtual TLightStatus LightStatus(TInt aTarget) const;
        
        /**
        * This method retrieves the supported light targets of the device.
        * Any attempt to use or reserve unsupported targets will fail with
        * KErrNotSupported.
        *
        * @return Bitmask containing supported light targets.
        *
        * @see TLightTarget
        */
        virtual TInt SupportedTargets() const;

        
        /*
        *   Used for setting color of Light Target(s).
        *
        * @param aTarget Defines which light should be controlled. Multiple lights can
        *                be specified with using bitwise-or.
        *
        * @param aRGB RGB Values to be set for the target(s).
        *
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 

        * @see THWRMLightColor
        */
        IMPORT_C void SetLightColorL(TInt aTarget, THWRMLightColor& aRGB);
        
        /*
        *   Used for setting default color of Light Target(s).
        *
        * @param aTarget Defines which light should be controlled. Multiple lights can
        *                be specified with using bitwise-or.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 

        */
        
        IMPORT_C void SetLightDefaultColorL(TInt aTarget);

        /** 
        * The LightOnL method switches the specified target light on
        * for the specified duration and specified color.
        * 
        * @param aTarget Defines which light should be controlled. Multiple lights can
        *                be specified with using bitwise-or.
        * @param aDuration Duration of the time the light is switched on measured in milliseconds.
        *                  After the duration expires, the light state for target will be changed 
        *                  to whatever state was caused by the last infinite time duration call, or
        *                  default state determined by inactivity timer, in case there has not 
        *                  been a previous infinite time duration call in this session.
        *                  If the aDuration time is KHWRMInfiniteDuration then it means 
        *                  an infinite value that has to be stopped by calling of any of 
        *                  the other light control methods.
        *                  Duration can have maximum value of KHWRMLightMaxDuration.
        * @param aRGBParam RGB Values to be set for the target(s).
        *
        * @leave KErrArgument One of the parameters is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
        *
        * @see TLightTarget
        * @see THWRMLightColor
        */
        IMPORT_C void LightOnL(TInt aTarget, TInt aDuration, const THWRMLightColor& aRGBParam);

        /** 
        * The LightOnL method switches the specified target light on
        * for the specified duration using specified intensity and color. Fade-in can also be controlled.
        * 
        * @param aTarget Defines which light should be controlled. Multiple lights can
        *                be specified with using bitwise-or.
        * @param aDuration Duration of the time the light is switched on measured in milliseconds.
        *                  After the duration expires, the light state for target will be changed 
        *                  to whatever state was caused by the last infinite time duration call, or
        *                  default state determined by inactivity timer, in case there has not 
        *                  been a previous infinite time duration call in this session.
        *                  If the aDuration time is KHWRMInfiniteDuration then it means 
        *                  an infinite value that has to be stopped by calling of any of 
        *                  the other light control methods.
        *                  Duration can have maximum value of KHWRMLightMaxDuration.
        * @param aIntensity Intensity of the light. If aIntensity is KHWRMDefaultIntensity, device default 
        *                   intensity will be used. 
        *                   Note: All devices might not support user defined intensity, in which case
        *                   device will behave in its default fashion.
        * @param aFadeIn If ETrue, lights will not turn on instantly but instead smoothly fade-in.
        *                Note: All devices will not support fade-in, in which case device will
        *                behave in its default fashion.
        * @param aRGBParam RGB Values to be set for the target(s).
        *
        * @leave KErrArgument One of the parameters is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
        *
        * @see TLightTarget
        * @see THWRMLightColor
        */

        IMPORT_C void LightOnL(TInt aTarget, TInt aDuration, TInt aIntensity, TBool aFadeIn, 
                      const  THWRMLightColor& aRGBParam);

        /** 
        * The LightBlinkL method blinks the target light(s) of the device for specified duration and color
        * using default intensity.
        *
        *
        * @param aTarget Defines which light should be controlled. Multiple lights can
        *                be specified with using bitwise-or.
        * @param aDuration Duration of the time the light is set to blink measured in milliseconds.
        *                  After the duration expires, the light state for target will be changed 
        *                  to whatever state was caused by the last infinite time duration call, or
        *                  default state determined by inactivity timer, in case there has not 
        *                  been a previous infinite time duration call in this session.
        *                  If the aTotalDuration time is KHWRMInfiniteDuration then it
        *                  means an infinite value that has to be
        *                  stopped by calling of any of the other light control methods.
        *                  Duration can have maximum value of KHWRMLightMaxDuration.
        *
        * @param aRGBParam RGB Values to be set for the target(s).
        *
        * @leave KErrArgument Parameter aDuration is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
        *
        * @see TLightTarget
        * @see THWRMLightColor
        */

        IMPORT_C void LightBlinkL(TInt aTarget, TInt aDuration, const THWRMLightColor& aRGBParam);

        /** 
        * The LightBlinkL method blinks the target light(s) of the device for specified duration
        * using specified intensity and color. On- and Off-cycle times of the blinking can also be controlled.
        *
        * @param aTarget Defines which light should be controlled. Multiple lights can
        *                be specified with using bitwise-or.
        * @param aDuration Duration of the time the light is set to blink measured in milliseconds.
        *                  After the duration expires, the light state for target will be changed 
        *                  to whatever state was caused by the last infinite time duration call, or
        *                  default state determined by inactivity timer, in case there has not 
        *                  been a previous infinite time duration call in this session.
        *                  If the aTotalDuration time is KHWRMInfiniteDuration then it
        *                  means an infinite value that has to be
        *                  stopped by calling of any of the other light control methods.
        *                  Duration can have maximum value of KHWRMLightMaxDuration.
        * @param aOnDuration Duration time, measured in milliseconds, of how long the Light is
        *                    switched on in every Blink cycle.
        *                    Duration can have maximum value of KHWRMLightMaxDuration.
        *                    For device default cycle duration, use value KHWRMDefaultCycleTime.
        *                    If either of aOnDuration or aOffDuration is KHWRMDefaultCycleTime,
        *                    both must be KHWRMDefaultCycleTime.
        *                    Some devices might not support variable blink cycle times, in which
        *                    case default value will be substituted.
        * @param aOffDuration Duration time, measured in milliseconds, of how long the Light
        *                     is switched off in every Blink cycle.
        *                     Duration can have maximum value of KHWRMLightMaxDuration.
        *                     For device default cycle duration, use value KHWRMDefaultCycleTime.
        *                     If either of aOnDuration or aOffDuration is KHWRMDefaultCycleTime,
        *                     both must be KHWRMDefaultCycleTime.
        *                     Some devices might not support variable blink cycle times, in which
        *                     case default value will be substituted.
        * @param aIntensity Intensity of the light. If aIntensity is KHWRMDefaultIntensity, device default 
        *                   intensity will be used.
        *                   Note: All devices might not support user defined intensity, in which case
        *                   device will behave in its default fashion.
        *
        * @param aRGBParam RGB Values to be set for the target(s).
        *
        * @leave KErrArgument One of the parameters is out of range or otherwise invalid.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle Light session has been invalidated.
        * @leave KErrTimedOut Timeout occurred in controlling light.
        * @leave KErrInUse One or more of specified targets are not reserved for
        *                  this client but are reserved for others.
        * @leave KErrNoMemory There is a memory allocation failure. 
        * @leave KErrGeneral There is a hardware error.
        *
        * @see TLightTarget
        * @see THWRMLightColor
        */
        
        IMPORT_C void LightBlinkL(TInt aTarget, TInt aDuration, TInt aOnDuration, TInt aOffDuration, 
                         TInt aIntensity, const THWRMLightColor& aRGBParam);



        
private:
		CHWRMLight();
		void ConstructL(MHWRMLightObserver* aCallback);        
        
private:
        CHWRMEnhancedLight* iEnhanced;

    };
    
/**
* A callback interface for light status reporting.
*
* If the client requires up-to-date status information, the client needs 
* to derive a class from the MHWRMlightObserver interface and implement 
* the LightStatusChanged() method. 
* 
* A callback object header example:
*
* @code 
* // INCLUDES
* #include <hwrmlight.h> // Link against HWRMLightClient.lib.
*
* class CTests : public CBase,
*                public MHWRMLightObserver
*     {
*     public:
*         CTests();
*         ~CTests();
*                           
*         void ConstructL();
*         static CTests* NewL();
*                
*         // from MHWRMLightObserver
*         virtual void LightStatusChanged(TInt aTarget, 
*                                         CHWRMLight::TLightStatus aStatus);
*
*    private:
*         CHWRMLight* iLight;
*    };
*
* @endcode
*
* A callback method implementation example:
*
* @code
* void CTests::LightStatusChanged(TInt aTarget, 
*                          CHWRMLight::TLightStatus aStatus)
*     {
*     RDebug::Print(_L("### Light state changed for target: 0x%x"), aTarget);
*     switch ( aStatus )
*         {
*         case CHWRMLight::ELightOn:
*             RDebug::Print(_L("### Light state changed: ELightOn"));
*             break;
*         case CHWRMLight::ELightOff:
*             RDebug::Print(_L("### Light state changed: ELightOff"));
*             break;
*         case CHWRMLight::ELightBlink:
*             RDebug::Print(_L("### Light state changed: ELightBlink"));
*             break;
*         case CHWRMLight::ELightStatusUnknown:
*             RDebug::Print(_L("### Light state changed: ELightStatusUnknown"));
*             break;
*         default:
*             RDebug::Print(_L("### Light state changed: UNDEFINED !"));
*             break;
*         }
*     }
*
* @endcode
*
* @publishedAll
* @released
*/
class MHWRMLightObserver
    {    
    public:
        
        /** 
        * Called when the device light status changes.
        * Note that if the light status for certain target changes
        * very rapidly, some state transitions might be missed.
        * It is however guaranteed that latest state is always obtained.
        *
        * @param aTarget Indicates target(s) the new status applies to.
        * @param aStatus Indicates light request status.
		*
	 	* @see CHWRMLight::TLightTarget
	 	* @see CHWRMLight::TLightStatus
		*/
        virtual void LightStatusChanged(TInt aTarget, 
                                        CHWRMLight::TLightStatus aStatus) = 0;
	};


#endif      // HWRMLIGHT_H   
            
// End of File
