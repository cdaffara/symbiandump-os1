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




#ifndef HWRMEXTENDEDLIGHT_H
#define HWRMEXTENDEDLIGHT_H

#include <hwrmlight.h> 

class MHWRMExtendedLightObserver;

/**
* Base class of Extended Light Client API of HWRM
*
* The HWRM Extended Light API provides the ability to control the device 
* display light as well as the keyboard illumination. 
*
* Every new call of the extended light API method stops all ongoing light
* control orders. Light state after duration based orders expire is the state
* specified by the last non-duration based order.
*
* The interface contains synchronous methods that will block the client application.
*
* @publishedPartner
* @released
*/
class CHWRMExtendedLight : public CBase
    {
    public:
		/**
		* Possible light states that can be get for the different light targets
		*/
		enum TLightStatus
			{
			/**
			For debugging/development and signaling an error conditions
			*/
			ELightStatusUnknown = 0, 
			/**
			Light state switch to light on
			*/
			ELightOn,
			/**
			Light state switch to light off
			*/
			ELightOff,               
			/**
			Light state switch to light blinking
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
            Device specific custom target 5.
            */
            ECustomTarget5               = 0x100,   
            /**
            Device specific custom target 6.
            */
            ECustomTarget6               = 0x200,   
            /**
            Device specific custom target 7.
            */
            ECustomTarget7               = 0x400,   
            /**
            Device specific custom target 8.
            */
            ECustomTarget8               = 0x800,   
            /**
            Device specific custom target 9.
            */
            ECustomTarget9               = 0x1000,  
            /**
            Device specific custom target 10.
            */
            ECustomTarget10              = 0x2000,  
            /**
            Device specific custom target 11.
            */
            ECustomTarget11              = 0x4000, 
            /**
            Device specific custom target 12.
            */
            ECustomTarget12              = 0x8000,
            /**
            Device specific custom target 13.
            */
            ECustomTarget13              = 0x10000,
            /**
            Device specific custom target 14.
            */
            ECustomTarget14              = 0x20000,
            /**
            Device specific custom target 15.
            */
            ECustomTarget15              = 0x40000,
            /**
            Device specific custom target 16.
            */
            ECustomTarget16              = 0x80000,
            /**
            Device specific custom target 17.
            */
            ECustomTarget17              = 0x100000,
            /**
            Device specific custom target 18.
            */
            ECustomTarget18              = 0x200000,
            /**
            Device specific custom target 19.
            */
            ECustomTarget19              = 0x400000,
            /**
            Device specific custom target 20.
            */
            ECustomTarget20              = 0x800000,
            /**
            Device specific custom target 21.
            */
            ECustomTarget21              = 0x1000000,
            /**
            Device specific custom target 22.
            */
            ECustomTarget22              = 0x2000000,
            /**
            Device specific custom target 23.
            */
            ECustomTarget23              = 0x4000000,
            /**
            Device specific custom target 24.
            */
            ECustomTarget24              = 0x8000000,
            /**
            Device specific custom target 25.
            */
            ECustomTarget25              = 0x10000000,
            /**
            Device specific custom target 26.
            */
            ECustomTarget26              = 0x20000000,
            /**
            Device specific custom target 27.
            */
            ECustomTarget27              = 0x40000000,
            
            /**
            * Special target used to control all currently available system lights.
            *
            * System lights normally include all displays and keyboards, 
            * but not custom lights. This is however device dependant.
            *
            * A target mask including this target is 
            * always changed to a device state specific target mask.  
            * Note that the  system target with any other target is not supported. 
            *
            * This target is always supported but it is never
            * included in supported targets mask.
            *
            * @see CHWRMExtendedLight::SupportedTargets()
            */
            ESystemTarget                = 0x80000000  
            };
            
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor for implementation class.
        *
        * @return Pointer to new CHWRMExtendedLight instance.
        * @leave KErrNotSupported Device doesn't support Light feature.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        */
        IMPORT_C static CHWRMExtendedLight* NewL();
        
        /**
        * Two-phased constructor for implementation class. 
        * Leaves instance to cleanup stack.
        *
        * @return Pointer to new CHWRMExtendedLight instance.
        * @leave KErrNotSupported Device doesn't support Light feature.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        */
        IMPORT_C static CHWRMExtendedLight* NewLC();

        /**
        * Two-phased constructor for implementation class.
        * Use this method for creating a Light client with callbacks.
        *
        * @param aCallback Pointer to callback instance
        * @return Pointer to new CHWRMExtendedLight instance.
        * @leave KErrNotSupported Device doesn't support Light feature.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        */
        IMPORT_C static CHWRMExtendedLight* NewL(MHWRMExtendedLightObserver* aCallback);
        
        /**
        * Two-phased constructor for implementation class. 
        * Use this method for creating a Light client with callbacks.
        * Leaves instance to cleanup stack.
        *
        * @param aCallback Pointer to callback instance
        * @return Pointer to new CHWRMExtendedLight instance.
        * @leave KErrNotSupported Device doesn't support Light feature.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        */
        IMPORT_C static CHWRMExtendedLight* NewLC(MHWRMExtendedLightObserver* aCallback);

        /**
        * Two-phased constructor for implementation class.
        * On opening passes permitted targets.
        *
        * @param aPermittedTargets permitted targets.
        * @return Pointer to new CHWRMExtendedLight instance.
        * @leave KErrNotSupported Device doesn't support Light feature.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        */       
        IMPORT_C static CHWRMExtendedLight* NewL(TInt aPermittedTargets);
        
       /**
        * Two-phased constructor for implementation class. 
        * Use this method for creating a Light client with callbacks.
        * On opening passes permitted targets.
        *
        * @param aCallback Pointer to callback instance
        * @param aPermittedTargets permitted targets.
        * @return Pointer to new CHWRMExtendedLight instance.
        * @leave KErrNotSupported Device doesn't support Light feature.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        */
		IMPORT_C static CHWRMExtendedLight* NewL(MHWRMExtendedLightObserver* aCallback, TInt aPermittedTargets);


    public: 

    	
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
    	* Calling this method is equal to call ReserveLightL( aTarget, EFalse, EFalse),
    	* i.e. any previously frozen state will not be restored and CCoeEnv
    	* background/foreground status is always used to control further reservations.
    	*
		* @param aTarget Defines which light should be reserved. Multiple lights can
		*          be specified with using bitwise-or.
    	*
    	* @leave KErrNotSupported One or more of specified targets are not supported.
    	* @leave KErrAccessDenied if no CCoeEnv present.
    	* @leave KErrNotReady if trying to reserve while on background.
        * @leave KErrNoMemory if there is a memory allocation failure. 
		*
	 	* @see TLightTarget
    	*/
    	virtual void ReserveLightL(TInt aTarget)=0;
    	
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
		*          be specified with using bitwise-or.
    	* @param aRestoreState If ETrue, the state frozen on last release will be
    	*          restored upon successful reservation.
    	*         I.e. if light was blinking when it was released by this client the last
    	*          time, it would start blinking again upon successful reservation.
    	*         For the first reservation of each session this parameter is always
    	*          considered EFalse regardless of what is supplied, as there is no
    	*          previous frozen state to restore.
    	* @param aForceNoCCoeEnv If EFalse, then reservation requires that
    	*          this client is on the foreground at the time of reservation
    	*          and light target will be automatically released and re-reserved based on
    	*          background/foreground status of the this client. This also implies
    	*          that CCoeEnv::Static() != NULL is required.
    	*         If ETrue, the client will not require CCoeEnv to
    	*          be present nor does it automatically reserve/release light
    	*          by depending on foreground/background status of the client.
    	*         Only trusted clients are allowed to set this flag to ETrue.
    	*         Client is considered trusted if it has nonstandard priority
    	*          defined in the lights policy.
    	*
    	* @leave KErrNotSupported One or more of specified targets are not supported.
    	* @leave KErrAccessDenied if aForceNoCCoeEnv is ETrue and client is not trusted.
    	* @leave KErrBadHandle if aForceNoCCoeEnv is EFalse and no CCoeEnv present.
    	* @leave KErrNotReady if trying to reserve while on background and aForceNoCCoeEnv is EFalse.
        * @leave KErrNoMemory if there is a memory allocation failure. 
		*
	 	* @see TLightTarget
    	*/
    	virtual void ReserveLightL(TInt aTarget, TBool aRestoreState, TBool aForceNoCCoeEnv)=0;

    	/**
    	* Releases light target if it was previously reserved for this client.
    	* If this client has not reserved any of the specified lights, 
    	* this method does nothing.
    	* Any reserved light targets that are released and have no other suspended
    	* clients will be reset to default state, which is either lights on or lights off, 
    	* depending on system inactivity time.
    	*
		* @param aTarget Defines which light should be released. Multiple lights can
		*          be specified with using bitwise-or.
		*
	 	* @see TLightTarget
    	*/
    	virtual void ReleaseLight(TInt aTarget)=0;


	    /** 
	    * The LightOnL method switches the specified target light on
	    * for infinite duration using default intensity. Lights will use fade-in.
	    *
		* @param aTarget Defines which light should be controlled. Multiple lights can
		*                 be specified with using bitwise-or.
		*
    	* @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle if light session has been invalidated.
        * @leave KErrTimedOut if timeout occurred in controlling light.
        * @leave KErrInUse if one or more of specified targets are not reserved for this client but are reserved for others.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        * @leave KErrGeneral if there is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOnL(TInt aTarget) = 0;

	    /** 
	    * The LightOnL method switches the specified target light on
	    * for the specified duration using default intensity. Lights will use fade-in.
	    *
		* @param aTarget   Defines which light should be controlled. Multiple lights can
		*                   be specified with using bitwise-or.
		* @param aDuration Duration of the time the light is switched on measured in milliseconds.
		*                   After the duration expires, the light state for target will be changed 
		*                    to whatever state was caused by the last infinite time duration call, or
		*                    default state determined by inactivity timer, in case there has not 
		*                    been a previous infinite time duration call in this session.
		*                   If the aDuration time is KHWRMInfiniteDuration then it
		*                    means an infinite value that has to be
		*                    stopped by calling of any of the other light control methods.
		*                   Duration can have maximum value of KHWRMLightMaxDuration.
		*
        * @leave KErrArgument aDuration is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle if light session has been invalidated.
        * @leave KErrTimedOut if timeout occurred in controlling light.
        * @leave KErrInUse if one or more of specified targets are not reserved for this client but are reserved for others.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        * @leave KErrGeneral if there is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOnL(TInt aTarget, 
		                      TInt aDuration) = 0;

	    /** 
	    * The LightOnL method switches the specified target light on
	    * for the specified duration using specified intensity. Fade-in can also be controlled.
	    *
		* @param aTarget    Defines which light should be controlled. Multiple lights can
		*                    be specified with using bitwise-or.
		* @param aDuration  Duration of the time the light is switched on measured in milliseconds.
		*                    After the duration expires, the light state for target will be changed 
		*                     to whatever state was caused by the last infinite time duration call, or
		*                     default state determined by inactivity timer, in case there has not 
		*                     been a previous infinite time duration call in this session.
		*                    If the aDuration time is KHWRMInfiniteDuration then it
		*                     means an infinite value that has to be
		*                     stopped by calling of any of the other light control methods.
		*                    Duration can have maximum value of KHWRMLightMaxDuration.
		* @param aIntensity Intensity of the light. If aIntensity is KHWRMDefaultIntensity, device default 
		*                     intensity will be used. 
		*                    Note: All devices might not support user defined intensity, in which case
		*                     device will behave in its default fashion.
		* @param aFadeIn    If ETrue, lights will not turn on instantly but instead smoothly fade-in.
		*                    Note: All devices will not support fade-in, in which case device will
		*                     behave in its default fashion.
		*
        * @leave KErrArgument One of the parameters is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle if light session has been invalidated.
        * @leave KErrTimedOut if timeout occurred in controlling light.
        * @leave KErrInUse if one or more of specified targets are not reserved for this client but are reserved for others.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        * @leave KErrGeneral if there is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOnL(TInt aTarget, 
		                      TInt aDuration, 
		                      TInt aIntensity,
		                      TBool aFadeIn) = 0;

	    /** 
	    * The LightBlinkL method blinks the target light(s) of the device for infinite duration
	    * using default intensity.
	   	*
		* @param aTarget        Defines which light should be controlled. Multiple lights can
		*                        be specified with using bitwise-or.
	   	*
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle if light session has been invalidated.
        * @leave KErrTimedOut if timeout occurred in controlling light.
        * @leave KErrInUse if one or more of specified targets are not reserved for this client but are reserved for others.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        * @leave KErrGeneral if there is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightBlinkL(TInt aTarget) = 0;

	    /** 
	    * The LightBlinkL method blinks the target light(s) of the device for specified duration
	    * using default intensity.
	   	*
		* @param aTarget        Defines which light should be controlled. Multiple lights can
		*                        be specified with using bitwise-or.
		* @param aDuration      Duration of the time the light is set to blink measured in milliseconds.
		*                        After the duration expires, the light state for target will be changed 
		*                         to whatever state was caused by the last infinite time duration call, or
		*                         default state determined by inactivity timer, in case there has not 
		*                         been a previous infinite time duration call in this session.
		*                        If the aTotalDuration time is KHWRMInfiniteDuration then it
		*                         means an infinite value that has to be
		*                         stopped by calling of any of the other light control methods.
		*                        Duration can have maximum value of KHWRMLightMaxDuration.
	   	*
        * @leave KErrArgument aDuration is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle if light session has been invalidated.
        * @leave KErrTimedOut if timeout occurred in controlling light.
        * @leave KErrInUse if one or more of specified targets are not reserved for this client but are reserved for others.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        * @leave KErrGeneral if there is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightBlinkL(TInt aTarget, 
		                         TInt aDuration) = 0;

	    /** 
	    * The LightBlinkL method blinks the target light(s) of the device for specified duration
	    * using specified intensity. On- and Off-cycle times of the blinking can also be controlled.
	   	*
		* @param aTarget        Defines which light should be controlled. Multiple lights can
		*                        be specified with using bitwise-or.
		* @param aDuration      Duration of the time the light is set to blink measured in milliseconds.
		*                        After the duration expires, the light state for target will be changed 
		*                         to whatever state was caused by the last infinite time duration call, or
		*                         default state determined by inactivity timer, in case there has not 
		*                         been a previous infinite time duration call in this session.
		*                        If the aTotalDuration time is KHWRMInfiniteDuration then it
		*                         means an infinite value that has to be
		*                         stopped by calling of any of the other light control methods.
		*                        Duration can have maximum value of KHWRMLightMaxDuration.
	   	* @param aOnDuration    Duration time, measured in milliseconds, of how long the Light is
	   	*                         switched on in every Blink cycle.
		*                        Duration can have maximum value of KHWRMLightMaxDuration.
		*                        For device default cycle duration, use value KHWRMDefaultCycleTime.
		*                        If either of aOnDuration or aOffDuration is KHWRMDefaultCycleTime,
		*                         both must be KHWRMDefaultCycleTime.
		*                        Some devices might not support variable blink cycle times, in which
		*                         case default value will be substituted.
	   	* @param aOffDuration   Duration time, measured in milliseconds, of how long the Light
	   	*                         is switched off in every Blink cycle.
		*                        Duration can have maximum value of KHWRMLightMaxDuration.
        *                        For device default cycle duration, use value KHWRMDefaultCycleTime.
        *                        If either of aOnDuration or aOffDuration is KHWRMDefaultCycleTime,
		*                         both must be KHWRMDefaultCycleTime.
		*                        Some devices might not support variable blink cycle times, in which
		*                         case default value will be substituted.
		* @param aIntensity     Intensity of the light. If aIntensity is KHWRMDefaultIntensity, device default 
		*                         intensity will be used.
		*                        Note: All devices might not support user defined intensity, in which case
		*                         device will behave in its default fashion.
	   	*
        * @leave KErrArgument One of the parameters is out of range or otherwise invalid.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle if light session has been invalidated.
        * @leave KErrTimedOut if timeout occurred in controlling light.
        * @leave KErrInUse if one or more of specified targets are not reserved for this client but are reserved for others.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        * @leave KErrGeneral if there is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightBlinkL(TInt aTarget, 
		                         TInt aDuration, 
		                         TInt aOnDuration, 
		                         TInt aOffDuration, 
		                         TInt aIntensity) = 0;

   	    /**
	    * The LightOffL method switches the device light off for the specified target for
	    * infinite duration. Lights will be switched off with fade-out.
		*
		* @param aTarget          Defines which light should be controlled. Multiple lights can
		*                          be specified with using bitwise-or.
		*
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle if light session has been invalidated.
        * @leave KErrTimedOut if timeout occurred in controlling light.
        * @leave KErrInUse if one or more of specified targets are not reserved for this client but are reserved for others.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        * @leave KErrGeneral if there is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOffL(TInt aTarget) = 0;

   	    /**
	    * The LightOffL method switches the device light off for the specified target for
	    * the specified duration time. Lights will be switched off with fade-out.
		*
		* @param aTarget          Defines which light should be controlled. Multiple lights can
		*                           be specified with using bitwise-or.
		* @param aDuration        Duration of the time the light is switched off measured in milliseconds.
		*                          After the duration expires, the light state for target will be changed 
		*                           to whatever state was caused by the last infinite time duration call, or
		*                           default state determined by inactivity timer, in case there has not 
		*                           been a previous infinite time duration call in this session.
		*                          If the aDuration time is KHWRMInfiniteDuration then it 
		*                           means an infinite value that has to be
		*                           stopped by calling of any of the other light control methods.
		*                          Duration can have maximum value of KHWRMLightMaxDuration.
		*
        * @leave KErrArgument aDuration is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle if light session has been invalidated.
        * @leave KErrTimedOut if timeout occurred in controlling light.
        * @leave KErrInUse if one or more of specified targets are not reserved for this client but are reserved for others.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        * @leave KErrGeneral if there is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOffL(TInt aTarget, 
		                       TInt aDuration) = 0;

   	    /**
	    * The LightOffL method switches the device light off for the specified target for
	    * the specified duration time. Lights fade-out can also be controlled.
		*
		* @param aTarget          Defines which light should be controlled. Multiple lights can
		*                           be specified with using bitwise-or.
		* @param aDuration        Duration of the time the light is switched off measured in milliseconds.
		*                          After the duration expires, the light state for target will be changed 
		*                           to whatever state was caused by the last infinite time duration call, or
		*                           default state determined by inactivity timer, in case there has not 
		*                           been a previous infinite time duration call in this session.
		*                          If the aDuration time is KHWRMInfiniteDuration then it 
		*                           means an infinite value that has to be
		*                           stopped by calling of any of the other light control methods.
		*                          Duration can have maximum value of KHWRMLightMaxDuration.
		* @param aFadeOut         If ETrue, lights will not turn off instantly but instead smoothly fade-out
		*                          Note: All devices will not support fade-out, in which case device will
		*                           behave in its default fashion.
		*
        * @leave KErrArgument aDuration is out of range.
        * @leave KErrNotSupported One or more of specified targets are not supported.
        * @leave KErrBadHandle if light session has been invalidated.
        * @leave KErrTimedOut if timeout occurred in controlling light.
        * @leave KErrInUse if one or more of specified targets are not reserved for this client but are reserved for others.
        * @leave KErrNoMemory if there is a memory allocation failure. 
        * @leave KErrGeneral if there is a hardware error.
		*
	 	* @see TLightTarget
		*/
		virtual void LightOffL(TInt aTarget, 
		                       TInt aDuration, 
		                       TBool aFadeOut) = 0;
		                       
        /**
        * This method retrieves the current light status. 
        *
		* @param aTarget Defines which light status is returned. 
		*                 This method only supports single target, as different
		*                 targets might have different statuses.
        * @return TLightStatus indicating the current light status. If there is a problem or
        *         multiple targets were specified, CHWRMExtendedLight::ELightStatusUnknown is returned.
        * 
        * @see MHWRMExtendedLightObserver
	 	* @see TLightTarget
        */
        virtual TLightStatus LightStatus(TInt aTarget) const = 0;
        
        /**
        * This method retrieves the supported light targets of the device.
        * Any attempt to use or reserve unsupported targets will fail with
        * KErrNotSupported.
        *
        * @return Bitmask containing supported light targets.
        *
        * @see TLightTarget
        */
        virtual TInt SupportedTargets() const = 0;
    };
    
/**
*  An interface the developer implements for handling notification events from the HWRM server. 
*
* @publishedPartner
* @released
*/
class MHWRMExtendedLightObserver
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
	 	* @see CHWRMExtendedLight::TLightTarget
	 	* @see CHWRMExtendedLight::TLightStatus
		*/
        virtual void LightStatusChanged(TInt aTarget, 
                                        CHWRMExtendedLight::TLightStatus aStatus) = 0;
	};


#endif      // HWRMEXTENDEDLIGHT_H   
            
// End of File
