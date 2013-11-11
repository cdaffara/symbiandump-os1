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
* Description:  This file contains the header of the CHWRMHaptics class.
*
*/


#ifndef C_HWRMHAPTICS_H
#define C_HWRMHAPTICS_H

#include <e32base.h>
#include <hwrmlogicalactuators.h>
#include <hwrmhapticsobserver.h>
#include <hwrmhapticsactuatorobserver.h>

/** Minimum magnitude value. */
const TInt KHWRMHapticsMinMagnitude = 0;

/** Maximum magnitude value. */
const TInt KHWRMHapticsMaxMagnitude = 10000;

/**
* Minimum device priority.
*
* To be used with SetDeviceProperty().
*/
const TInt KHWRMHapticsMinDevicePriority = 0;

/**
* Maximum device priority.
*
* To be used with SetDeviceProperty().
*/
const TInt KHWRMHapticsMaxDevicePriority = 15;


/** Minimum strength value. */
const TInt KHWRMHapticsMinStrength = 0;

/** Maximum strength value. */
const TInt KHWRMHapticsMaxStrength = 10000;


 /**
 * The class used to control the device's haptics feature.
 *
 * The Haptics API provides the ability to control the device's haptics feature.
 * The API provides methods for receiving the current status and effect
 * completion of the haptics. The API provides synchronous and asynchronous
 * versions of playing methods due to the nature of effect playing
 * where multiple calls may be made to play effects.
 * Synchronous methods are provided for other API functionality.
 * They will block the calling client during their execution.
 * The API is meant for all applications which need to control the
 * device's haptics feature.
 *
 * The API consist of the class CHWRMHaptics and related observer classes:
 * MHWRMHapticsObserver and MHWRMHapticsActuatorObserver. If the client
 * requires up-to-date status information, it can be achieved by deriving
 * the client from MHWRMHapticsObserver or MHWRMHapticsActuatorObserver or 
 * from both and providing a callback pointer(s) of the implementing class
 * for the NewL() method.
 *
 * @code
 *
 * // ===================================================================
 * // Usage example 1:
 * //    - Setting the license key.
 * //    - Playing a periodic effect
 * //    - Preconditions:
 * //        - Haptics feature must be enabled by the system.
 * // ===================================================================
 *
 * #include <hwrmhaptics.h>          // link against hwrmhapticsclient.lib
 * #include <hwrmlogicalactuators.h> // enumeration of logical actuators
 *
 * TInt minPeriod( 0 );
 * TInt effectHandle( 0 );
 * TInt suppAct( 0 );
 *
 * CHWRMHaptics* haptics = CHWRMHaptics::NewL( NULL, NULL );
 *
 * haptics->SupportedActuators( suppAct );
 *
 * if( EHWRMLogicalActuatorDevice & suppAct )
 *     {
 *     haptics->OpenActuatorL( EHWRMLogicalActuatorDevice );
 *     }
 * else if ( EHWRMLogicalActuatorAny & suppAct )
 *     {
 *     haptics->OpenActuatorL( EHWRMLogicalActuatorAny );
 *     }
 *
 *
 * // 3rd party developers can obtain the license key from Forum Nokia
 * _LIT8( KLicenseKey,"_this_value_must_be_32_in_length" );
 *
 * User::LeaveIfError(
 *     haptics->SetDeviceProperty( 
 *         THWRMHapticsDevicePropertyTypes::EHWRMHapticsLicenseKey,
 *         KLicenseKey ) );
 *
 * // --> now playing effects is possible
 *
 * CHWRMHaptics::THWRMHapticsPeriodicEffect periodicEff;
 *
 * periodicEff.iDuration = 5000;
 * periodicEff.iMagnitude = 5000;
 * periodicEff.iPeriod = minPeriod;
 * periodicEff.iStyle = CHWRMHaptics::EHWRMHapticsStyleSharp;
 * periodicEff.iAttackTime = 250;
 * periodicEff.iAttackLevel = 10000;
 * periodicEff.iFadeTime = 250;
 * periodicEff.iFadeLevel = 0;
 *
 * haptics->PlayPeriodicEffect( periodicEff, effectHandle );
 *
 * // ... something happened in the application and it has lost focus
 * // so stop the effect immediately
 *
 * haptics->StopPlayingEffect( effectHandle );
 *
 * // ================================================================
 * // Usage example 2:
 * //    - Loading effect data from a file and playing effects from the
 * //      loaded data.
 * //    - Preconditions:
 * //        - license key is set
 * //    - Recommended usage style:
 * //        - Effect data file can contain definition(s) of periodic
 * //          effects, magsweep effects or a combination of these basic
 * //          types called timeline effects, which call basic effects in
 * //          sequence thus forming new more complex effects.
 * //        - Load the effect data once in the client application.
 * //        - Play effects from the loaded data using the effectIndex
 * // ================================================================
 *
 * // Use S60 FileSystem to load the effect data file to a buffer
 *
 * ...
 *
 * // Effect data has been read into a descriptor by the user.
 * // Load the effect data to be used by the haptic subsystem.
 * TInt fileHandle( 0 );
 * User::LeaveIfError( haptics->LoadEffectData( data, fileHandle ) );
 * 
 * TInt effectIndex = 0;
 * TInt hapticsStatus = haptics->PlayEffect( fileHandle,
 *                                              effectIndex,
 *                                              effectHandle );
 *                                                
 * hapticsStatus = haptics->DeleteEffectData( fileHandle );
 *
 * if( KErrNone != hapticsStatus )
 *     {
 *     // do some error handling...
 *     }
 *
 * delete haptics;
 * haptics = NULL;
 *
 * @endcode
 *
 * Common error codes returned by the Haptics API methods.
 *
 * KErrArgument Argument is invalid, e.g., malformatted effect data, or too
 *              large magnitude value.
 * KErrAccessDenied The license key is not set when trying to use some
 *                  haptics method.
 * KErrNoMemory There is insufficient memory available for the method to 
 *              complete.
 * KErrNotReady Initialization has not been done properly when trying to use
 *              a haptics method.
 *
 * @lib hwrmhapticsclient.dll
 * @since S60 5.1
 */

 class CHWRMHaptics : public CBase
  {
public:

    /**
    * Defines the paramaters used in a magsweep effect.
    *
    * Used by
    * PlayMagSweepEffect(),
    * ModifyPlayingMagSweepEffect(),
    * GetMagSweepEffectDefinition().
    */
    struct THWRMHapticsMagSweepEffect
        {
        /**
        * Duration of the effect. Unit is milliseconds.
        *
        * To specify an infinite duration, the effect duration
        * should be set to a value returned by InfiniteDuration().
        * For a finite duration, the effect duration is clamped to a value
        * from 0 to the value returned by GetDeviceCapability()
        * for the EHWRMHapticsMaxEffectDuration capability type.
        */
        TInt iDuration;

        /**
        * Magnitude of the effect.
        *
        * The effect magnitude is clamped to a value from 
        * KHWRMHapticsMinMagnitude to KHWRMHapticsMaxMagnitude.
        */
        TInt iMagnitude;

        /**
        * Style of the effect.
        *
        * Can be one of THWRMHapticsEffectStyles.
        */
        TInt iStyle;

        /**
        * Attack time of the effect. Unit is milliseconds.
        *
        * The attack time is clamped to a value from 0 to the value returned
        * by GetDeviceCapability() for the EHWRMHapticsMaxEnvelopeTime
        * capability type.
        */
        TInt iAttackTime;

        /**
        * Attack level of the effect.
        *
        * The attack level is clamped to a value from KHWRMHapticsMinMagnitude
        * to KHWRMHapticsMaxMagnitude.
        */
        TInt iAttackLevel;

        /**
        * Fade time of the effect. Unit is milliseconds.
        *
        * The fade time is clamped to a value from 0 to the value returned
        * by GetDeviceCapability() for the EHWRMHapticsMaxEnvelopeTime
        * capability type.
        */
        TInt iFadeTime;

        /**
        * Fade level of the effect.
        *
        * The fade level is clamped to a value from KHWRMHapticsMinMagnitude
        * to KHWRMHapticsMaxMagnitude inclusive.
        */
        TInt iFadeLevel;
        };

    /**
    * Defines the parameters used in a periodic effect.
    *
    * Used by
    * PlayPeriodicEffect(),
    * ModifyPlayingPeriodicEffect(),
    * GetPeriodicEffectDefinition().
    */
    struct THWRMHapticsPeriodicEffect
        {
        /**
        * Duration of the effect. Unit is milliseconds.
        *
        * To specify an infinite duration, the effect duration
        * should be set to InfiniteDuration().
        * For a finite duration, the effect duration is clamped to a value
        * from 0 to the value returned by GetDeviceCapability()
        * for the EHWRMHapticsMaxEffectDuration capability type.
        */
        TInt iDuration;

        /**
        * Magnitude of the effect.
        *
        * The effect magnitude is clamped to a value from
        * KHWRMHapticsMinMagnitude to KHWRMHapticsMaxMagnitude.
        */
        TInt iMagnitude;

        /**
        * Period of the effect. Unit is milliseconds.
        *
        * The period is clamped to a value returned by GetDeviceCapability()
        * for EHWRMHapticsMinPeriod capability type to the value returned
        * by GetDeviceCapability() for the EHWRMHapticsMaxEnvelopeTime
        * capability type.
        */
        TInt iPeriod;

        /**
        * Style of the effect.
        *
        * Can be one of THWRMHapticsEffectStyles.
        */
        TInt iStyle;

        /**
        * Attack time of the effect. Unit is milliseconds.
        *
        * The attack time is clamped to a value from 0 to the value returned
        * by GetDeviceCapability() for the EHWRMHapticsMaxEnvelopeTime
        * capability type.
        */
        TInt iAttackTime;

        /**
        * Attack level of the effect.
        *
        * The attack level is clamped to a value from KHWRMHapticsMinMagnitude
        * to KHWRMHapticsMaxMagnitude.
        */
        TInt iAttackLevel;

        /**
        * Fade time of the effect. Unit is milliseconds.
        *
        * The fade time is clamped to a value from 0 to the value returned
        * by GetDeviceCapability() for the EHWRMHapticsMaxEnvelopeTime
        * capability type.
        */
        TInt iFadeTime;

        /**
        * Fade level of the effect.
        *
        * The fade level is clamped to a value from KHWRMHapticsMinMagnitude
        * to KHWRMHapticsMaxMagnitude.
        */
        TInt iFadeLevel;
        };

    /**
    * THWRMHapticsDevicePropertyTypes enumeration
    * Use SetDeviceProperty() to set properties for the haptics
    * and GetDeviceProperty() to get properties currently in use.
    */
    enum THWRMHapticsDevicePropertyTypes
        {
        /**
        * License key property. Usable with SetDeviceProperty() only.
        * Use const TDesC8& overloaded version of the method.
        *
        * Setting this property to a valid license key unlocks the haptics
        * subsystem in the device. Setting this property to an invalid
        * license key locks the haptics in the device (for this client). 
        * The haptics in the device are locked from clients by default.
        * When haptics is locked, all haptics related operations on the 
        * device, except for setting the license key and closing the device,
        * fail.
        * Haptics must be unlocked on a per instance basis.
        */
        EHWRMHapticsLicensekey = 0,

        /**
        * Property used to set/get the priority for effects to played for
        * the given haptics instance (i.e., for the given client).
        *
        * Use TInt overloaded version of the methods SetDeviceProperty()and
        * GetDeviceProperty() to use this property.
        *
        * Different haptics instances can use different priorities
        * on the same physical device. The priority determines which haptics
        * instance's effects are played when multiple haptics instances
        * are attempting to play effects at the same time.
        * The default priority is DefaultDevicePriority().
        * Priority values can range from KHWRMHapticsMinDevicePriority
        * to KHWRMHapticsMaxDevicePriority.
        * GetDeviceProperty() returns a value inside
        * TInt& aDevicePropertyValue in the range of 
        * KHWRMHapticsMinDevicePriority
        * to KHWRMHapticsMaxDevicePriority. If the client has not set
        * the EHWRMHapticsPriority property GetDeviceProperty() returns
        * a value of DefaultDevicePriority().
        */
        EHWRMHapticsPriority,

        /**
        * Property used to disable effects for the client's haptics instance.
        *
        * Use TBool overloaded version of the methods SetDeviceProperty() and
        * GetDeviceProperty() to use this property.
        *
        * When this property is set to true, any playing effects are
        * immediately stopped and subsequent requests to play
        * effects are ignored. Applies to the calling client's
        * haptics instance only. When this property is set to false,
        * subsequent requests to play effects are honored.
        * The default value for this property is false.
        */
        EHWRMHapticsDisableEffects,

        /**
        * A property that reduces/increases the magnitude of all effects
        * for a particular haptics instance.
        *
        * Use TInt overloaded version of the methods SetDeviceProperty()and
        * GetDeviceProperty() to use this property.

        * Strength values can vary from KHWRMHapticsMinStrength ("mute")
        * to KHWRMHapticsMaxStrength.
        * The default value for EHWRMHapticsStrength is 
        * KHWRMHapticsMaxStrength. If the EHWRMHapticsStrength property is
        * not set, the default value is used.
        * When reducing/increasing the magnitude of the effects by setting
        * the EHWRMHapticsStrength property, it only applies to the haptics
        * instance of the client which called the function. If there is a
        * second haptics instance held by the same or a different client,
        * it is not affected by the setting of the EHWRMHapticsStrength
        * property of the first client's haptics instance.
        *
        * Modifying the EHWRMHapticsStrength property of the haptics instance
        * does not affect currently playing effects, only effects played or
        * modified after calling the SetDeviceProperty() method using the new
        * EHWRMHapticsStrength value.
        */
        EHWRMHapticsStrength,

        /**
        * A property that reduces/increases the magnitude of all effects
        * for all haptics instances (whole device).
        *
        * Use TInt overloaded version of the methods SetDeviceProperty()and
        * GetDeviceProperty() to use this property.
        *
        * Strength values can vary from KHWRMHapticsMinStrength ("mute")
        * to KHWRMHapticsMaxStrength.
        * The default value for Master Strength is KHWRMHapticsMaxStrength.
        * If the client does not set the EHWRMHapticsMasterStrength property
        * of the haptics instance, the default value is used.
        * When reducing/increasing the magnitude of the effects,
        * EHWRMHapticsMasterStrength property affects all playing effects on
        * all haptics instances (whole device).
        * This means that all the haptics instances, held by other
        * clients are affected by the setting of EHWRMHapticsMasterStrength
        * property of the first client's haptics instance.
        *
        * The client which wants to set the EHWRMHapticsMasterStrength
        * property must have a haptics instance that has a maximum effect
        * priority.
        * The haptics client instance must set itself to a maximum priority
        * by calling SetDeviceProperty() using EHWRMHapticsPriority property
        * type and KMaxDevicePriority value before changing the
        * EHWRMHapticsMasterStrength property's value.
        *
        * Note: A licensee license key, provided to licensees only,
        * changes the EHWRMHapticsMasterStrength property.
        * A call to SetDeviceProperty( EHWRMHapticsMasterStrength, aValue )
        * always returns KErrAccessDenied for haptics instances that are
        * not using a licensee license key.
        */
        EHWRMHapticsMasterStrength
        };

    /**
    * Device's capability types.
    *
    * Use TInt& aDeviceCapabilityValue overloaded version of the
    * method GetDeviceCapability() unless otherwise mentioned.
    */
    enum THWRMHapticsDeviceCapabilityTypes
        {
        /**
        * Device category. See THWRMHapticsDeviceCategory enumeration defined
        * later in this API header for possible values.
        */
        EHWRMHapticsDeviceCategory = 0,

        /**
        * The maximum number of nested repeat bars supported for Timeline effects.
        *
        * Any repeat bars nested beyond this level are played only once.
        */
        EHWRMHapticsMaxNestedRepeats,

        /**
        * Number of vibration actuators present in the device.
        */
        EHWRMHapticsNumActuators,

        /**
        * Actuator type See THWRMHapticsActuatorType enumeration defined
        * later in this API header for possible values.
        */
        EHWRMHapticsActuatorType,

        /**
        * Number of effect slots.
        *
        * The number of effect slots represents the maximum number
        * of simple effects that can play simultaneously.
        * If an attempt is made to play more than this number of effects
        * at the same time, some of the simple effects do not play.
        */
        EHWRMHapticsNumEffectSlots,

        /**
        * Supported effect styles, stored as a bitfield. See
        * THWRMHapticsSupportedEffectStyles enumeration defined later in this
        * API header for possible values.
        */
        EHWRMHapticsSupportedStyles,

        /**
        * Minimum period for Periodic effects.
        */
        EHWRMHapticsMinPeriod,

        /**
        * Maximum period for Periodic effects.
        */
        EHWRMHapticsMaxPeriod,

        /**
        * Maximum duration for MagSweep and Periodic effects measured
        * in milliseconds.
        */
        EHWRMHapticsMaxEffectDuration,

        /**
        * Supported effect types. Stored as a bitfield. See 
        * THWRMHapticsSupportedEffectTypes enumeration defined later in this
        * API header for possible values.
        */
        EHWRMHapticsSupportedEffects,

        /**
        * Device name.
        *
        * Use TDes8& aDeviceCapabilityValue overloaded version of the
        * method GetDeviceCapability().
        */
        EHWRMHapticsDeviceName,

        /**
        * Maximum start time or fade time in milliseconds for
        * effect envelopes of MagSweep or periodic effects.
        */
        EHWRMHapticsMaxEnvelopeTime,

        /**
        * Version number of the physical haptics player in the device in
        * hexadecimal format.
        *
        * The format is OxMMNNSSBB, where MM is the major
        * version number, NN is the minor version number,
        * SS is a special build number and BB is the VTMP build number.
        * For example, for the hexadecimal format 0x02000053 the version
        * number is 2.0.83
        */
        EHWRMHapticsAPIVersionNumber,

        /**
        * Maximum size in bytes of effect data (buffer) that can be played
        * on a non-tethered device. 
        */
        EHWRMHapticsMaxEffectDataSize = 14
        };

    /**
    * Device category.
    */
    enum THWRMHapticsDeviceCategory
        {
        EHWRMHapticsVirtual = 2,
        EHWRMHapticsEmbedded = 3,
        };

    /**
    * Bitmask for effect support.
    *
    * To be used to analyze value returned by GetDeviceCapability().
    */
    enum THWRMHapticsSupportedEffectTypes
        {
        EHWRMHapticsSupportPeriodic = 1,
        EHWRMHapticsSupportMagSweep = 2,
        EHWRMHapticsSupportTimeline = 4,
        EHWRMHapticsSupportStreaming = 8
        };

    /**
    * Effect types
    */
    enum THWRMHapticsEffectTypes
        {
        EHWRMHapticsTypePeriodic = 0,
        EHWRMHapticsTypeMagSweep,
        EHWRMHapticsTypeTimeline,
        EHWRMHapticsTypeStreaming
        };

    /**
    * Bitmask for supported effect styles.
    *
    * To be used to analyze the value returned by GetDeviceCapability().
    */
    enum THWRMHapticsSupportedEffectStyles
        {
        EHWRMHapticsSupportSmooth = 1,
        EHWRMHapticsSupportStrong = 2,
        EHWRMHapticsSupportSharp  = 4
        };

    /**
    * Effect styles
    *
    * Used to specify Periodic or MagSweep effect style when calling
    * PlayMagSweepEffect(),
    * PlayPeriodicEffect(),
    * ModifyPlayingMagSweepEffect() and
    * ModifyPlayingPeriodicEffect().
    */
    enum THWRMHapticsEffectStyles
        {
        EHWRMHapticsStyleSmooth = 0,
        EHWRMHapticsStyleStrong,
        EHWRMHapticsStyleSharp
        };

    /**
    * Actuator types.
    *
    * To be used with GetDeviceCapability().
    */
    enum THWRMHapticsActuatorTypes
        {
        /**
        * Eccentric Rotating Mass actuator
        */
        EHWRMHapticsTypeERM = 0,

        /**
        * Linear Resonant actuator
        */
        EHWRMHapticsTypeLRA = 2
        };

    /**
    * Effect states.
    *
    * As returned in a call to GetEffectState().
    */
    enum THWRMHapticsEffectStates
        {
        EHWRMHapticsEffectNotPlaying = 0,
        EHWRMHapticsEffectPlaying,
        EHWRMHapticsEffectPaused
        };

    /**
    * Two-phased constructor.
    * Use this method for creating a haptics instance with callbacks.
    *
    * @param aHapticsCallback Pointer to callback instance. Can be NULL.
    * @param aActuatorCallback Pointer to callback instance. Can be NULL.
    *
    * @return A pointer to a new instance of the CHWRMHaptics class.
    *
    * @since S60 5.1
    */
    IMPORT_C static CHWRMHaptics* NewL( 
        MHWRMHapticsObserver* aHapticsCallback,
        MHWRMHapticsActuatorObserver* aActuatorCallback );
    
    /**
    * @overload
    *
    * @param[out] aStatus Request status. On completion contains:
    *                     KErrNone, if successful,
    *                     otherwise one of the other system-wide error codes.
    */
    IMPORT_C static CHWRMHaptics* NewL( 
        MHWRMHapticsObserver* aHapticsCallback,
        MHWRMHapticsActuatorObserver* aActuatorCallback,
        TRequestStatus& aStatus );

    /**
    * Method for opening a logical actuator for use.
    *
    * This method must be called before using any other methods of this class,
    * except when using GetXXX methods.
    * The Haptics API supports a limited number of instances of CHWRMHaptics 
    * class.
    * If all instances are currently in use, the next attempt to call
    * OpenActuatorL() from any client fails.
    * Applications should not use more instances than necessary as this
    * may prevent other applications from instantiating the CHWRMHaptics
    * class.
    *
    * @param aActuator Enumeration of the type of logical actuator the client
    *                  wants to use.
    *
    * @leave TInt KErrNotSupported, if the used logical actuator is not
    *              supported by the system.
    * @leave TInt KErrAlreadyExists, if the used logical actuator is already
    *              opened.
    * @leave TInt KErrInUse, if some other actuator is already opened.
    * @leave TInt KErrArgument, if aActuator is not valid enumeration value.
    *
    * @see THWRMLogicalActuators for a list of usable actuators.
    *
    * @since S60 5.1
    */
    virtual void OpenActuatorL( THWRMLogicalActuators aActuator ) = 0;

    /**
    * Method for getting a bitmask value of supported logical actuators.
    *
    * Developer needs to evaluate the returned bitmask against
    * THWRMLogicalActuators enumeration values to know the
    * supported logical actuators.
    *
    * @param[out] aActuators Bitmask of supported logical actuators.
    *
    * @return TInt KErrNone, if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @see THWRMLogicalActuators for a list of usable actuators.
    *
    * @since S60 5.1
    */
    virtual TInt SupportedActuators( TUint32& aActuators ) = 0;

    /**
    * Reserves haptics feature exclusively for this client.
    * A higher priority client may cause lower priority client reservation
    * to be temporarily suspended. The suspended client does not get 
    * any notification about suspension. If haptics is already reserved 
    * by a higher or equal priority client, reserving still succeeds,
    * but reservation is immediately suspended. When the reservation 
    * is suspended, playing effects do not actually cause the effects to be 
    * played. 
    * Note: Unless the client has instantiated the Haptics API with the status
    * observer, it does not receive any notifications about the fact that its 
    * effects are not actually played by the physical player when the client
    * has been suspended by a higher priority reservation.
    * Note also that even if haptics is reserved by some client, a higher 
    * priority client succeeds in playing its effects.
    *
    * Calling this method is equal to call ReserveHapticsL(EFalse),
    * i.e. foreground status is always used to control further reservations.
    * However, this is a future feature, and not implemented in current version.
    */
    virtual void ReserveHapticsL() = 0;

    /**
    * Reserves haptics feature exclusively for this client.
    * A higher priority client may cause lower priority client reservation
    * to be temporarily suspended. The suspended client does not get 
    * any notifications about suspension. If haptics is already reserved 
    * by a higher or equal priority client, reserving still succeeds,
    * but reservation is immediately suspended. When the reservation 
    * is suspended, playing effects does not actually cause the effects to be 
    * played.
    * Note: Unless the client has instantiated the Haptics API with the status
    * observer, it does not receive any notifications about the fact that its 
    * effects are not actually played by the physical player when the client
    * has been suspended by a higher priority reservation.
    * Note also that even if haptics is reserved by some client, a higher 
    * priority client succeeds in playing its effects.
    *
    * @param aForceNoCCoeEnv (Note: This is a future feature, and not
    *                        implemented in current version.  You can think of
    *                        it ETrue always.)
    *                        If EFalse, then reservation requires that
    *                        this client has the keyboard focus at the time of
    *                        reservation and haptics is automatically
    *                        released and re-reserved based on the keyboard
    *                        focus status of this client.
    *                        This also implies that CCoeEnv::Static() != NULL
    *                        is required.
    *                        If ETrue, the client does not require CCoeEnv to
    *                        be present nor does it automatically reserve or
    *                        release haptics by depending on the foreground or
    *                        background status of the client. Only trusted
    *                        clients are allowed to set this flag to ETrue.
    *                        The client application is considered trusted if 
    *                        it has a priority defined in haptics policy file. 
    *                        The policy files can be modified by S60 licensees. 
    *
    * @leave KErrAccessDenied Parameter aForceNoCCoeEnv is ETrue
    *                         and client is not trusted.
    * @leave KErrBadHandle Parameter aForceNoCCoeEnv is EFalse
    *                      and no CCoeEnv present.
    * @leave KErrNotReady Trying to reserve while on background and parameter
    *                     aForceNoCCoeEnv is EFalse.
    * @leave KErrNoMemory There is a memory allocation failure.
    */
    virtual void ReserveHapticsL( TBool aForceNoCCoeEnv ) = 0;

    /**
    * Releases haptics feature if it was previously reserved for this client.
    *
    * If this client has not reserved haptics feature, does nothing.
    * If haptics is on when it is released and no other client has a suspended
    * reservation, haptics is stopped.
    */
    virtual void ReleaseHaptics() = 0;

    /**
    * This method retrieves the current haptics status.
    *
    * @return THWRMHapticsStatus indicating the current haptics status
    *
    * @see THWRMHapticsStatus
    */
    virtual MHWRMHapticsObserver::THWRMHapticsStatus HapticsStatus() const=0;

    /**
    * Sets a property of the haptics.
    *
    * Some properties affect all haptics instances, some only
    * the current instance of the haptics. More about that can be found
    * in THWRMHapticsDevicePropertyTypes.
    *
    * @param[in] aDevicePropertyType Property type for the
    *            property to be set.
    * @param[in] aDevicePropertyValue Value of the property to set.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @see THWRMHapticsDevicePropertyTypes for a list of valid property types
    *
    * @since S60 5.1
    */
    virtual TInt SetDeviceProperty( TInt aDevicePropertyType,
                                    TInt aDevicePropertyValue ) = 0;

    /**
    * @overload
    */
    virtual TInt SetDeviceProperty( TInt aDevicePropertyType,
                                    const TDesC8& aDevicePropertyValue ) = 0;

    /**
    * Gets a property value of the haptics.
    *
    * @param[in] aDevicePropertyType Property type for the property to get.
    * @param[out] aDevicePropertyValue Reference to the variable that 
    *                                  receives the requested property
    *                                  value of the device.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @see THWRMHapticsDevicePropertyTypes for a list of the valid property
    *      types.
    *
    * @since S60 5.1
    */
    virtual TInt GetDeviceProperty( TInt aDevicePropertyType,
                                    TInt& aDevicePropertyValue ) = 0;

    /**
    * @overload
    *
    * @return KErrNone if successful, 
    *         KErrArgument if the length of the given string is less 
    *         than MaxPropertyStringLength(), 
    *         otherwise one of the other system-wide error codes.
    */
    virtual TInt GetDeviceProperty( TInt aDevicePropertyType,
                                    TDes8& aDevicePropertyValue ) = 0;

    /**
    * Gets a capability value of the haptics.
    *
    * @param[in] aDeviceCapabilityType Capability type of the
    *                                  capability to get.
    * @param[out] aDeviceCapabilityValue Reference to the variable that 
    *                                    receives the requested capability
    *                                    value of the device.
    *
    * @return TInt KErrNone if successful, 
    *              KErrNotReady if no actuator has been opened,
    *              otherwise one of the other system-wide error codes.
    *
    * @see THWRMHapticsDeviceCapabilityTypes
    *
    * @since S60 5.1
    */
    virtual TInt GetDeviceCapability( TInt aDeviceCapabilityType,
                                      TInt& aDeviceCapabilityValue ) = 0;

    /**
    * @overload
    * 
    * @return TInt KErrNone if successful, 
    *              KErrNotReady if no actuator has been opened,
    *              KErrArgument if the length of the given string is less 
    *              than MaxCapabilityStringLength(), 
    *              otherwise one of the other system-wide error codes.
    */
    virtual TInt GetDeviceCapability( TInt aDeviceCapabilityType,
                                      TDes8& aDeviceCapabilityValue ) = 0;

    /**
    * Retrieves the status of an effect (playing, not playing, paused).
    *
    * @param[in] aEffectHandle Handle to the effect which must have been
    *                          obtained by calling
    *                          PlayMagSweepEffect(),
    *                          PlayPeriodicEffect(),
    *                          PlayEffect(),
    *                          PlayEffectRepeat() or
    *                          CreateStreamingEffect()
    * @param[out] aEffectState Pointer to the variable that receives
    *                          the status bits of the effect.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @see THWRMHapticsEffectStates for a list of valid effect states.
    *
    * @since S60 5.1
    */
    virtual TInt GetEffectState( TInt aEffectHandle, TInt& aEffectState ) = 0;

    /**
    * Creates a streaming effect.
    *
    * Client calls CreateStreamingEffect() to create a new streaming
    * effect and gets a new handle for it; it should use that effect handle
    * to play a streaming sample by calling PlayStreamingSample().
    *
    * @param[out] aEffectHandle Reference to the variable that receives
    *                           a handle to the streaming effect.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
    virtual TInt CreateStreamingEffect( TInt& aEffectHandle ) = 0;

    /**
    * Plays a streaming sample given in the parameter defining the effect.
    *
    * Streaming sample can only be played after calling
    * CreateStreamingEffect() and by using the effecthandle it returns.
    *
    * Synchronous method returns when the haptic command has been evaluated
    * and the return value is valid.
    *
    * @param[in] aEffectHandle Handle to the streaming effect to play.
    * @param[in] aStreamingSample Reference to streaming sample data
    *                             containing the definition of
    *                             the effect to play.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    * @return TInt KErrInUse when haptics is reserved for a higher or 
    *              equal priority client.
    *
    * @since S60 5.1
    */
    virtual TInt PlayStreamingSample( TInt aEffectHandle,
                                      const TDesC8& aStreamingSample ) = 0;
    /**
    * @overload
    *
    * @param[out] aStatus Request status. On completion contains:
    *                     KErrNone, if successful,
    *                     KErrInUse when haptics is reserved for a higher or 
    *                     equal priority client,
    *                     otherwise one of the other system-wide error codes.
    */
    virtual void PlayStreamingSample( TInt aEffectHandle,
                                      const TDesC8& aStreamingSample,
                                      TRequestStatus& aStatus ) = 0;

    /**
    * Plays a streaming sample with a time offset given in the parameters
    * defining the effect.
    *
    * Client calls CreateStreamingEffect() to create a new streaming
    * effect and gets a new handle for it; it should use that effect handle
    * to play the streaming sample with this method.
    *
    * Synchronous method returns when the haptic command has been evaluated
    * and the return value is valid.
    *
    * @param[in] aEffectHandle Handle to the streaming effect to play.
    * @param[in] aStreamingSample Reference to streaming sample data
    *                             containing the definition of the
    *                             effect to play.
    * @param[in] aOffsetTime For aOffsetTime values that are greater than 0,
    *                        playback is delayed for aOffsetTime
    *                        in milliseconds.
    *                        For aOffsetTime values that are less than 0,
    *                        sample playback begins in offset time
    *                        in milliseconds into the current sample.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    * @return TInt KErrInUse when haptics is reserved for a higher or 
    *              equal priority client.
    *
    * @since S60 5.1
    */
    virtual TInt PlayStreamingSampleWithOffset( TInt aEffectHandle,
        const TDesC8& aStreamingSample,
        TInt aOffsetTime ) = 0;

    /**
    * @overload
    *
    * @param[out] aStatus Request status. On completion contains:
    *                     KErrNone, if successful,
    *                     KErrInUse when haptics is reserved for a higher or 
    *                     equal priority client,
    *                     otherwise one of the other system-wide error codes.
    */
    virtual void PlayStreamingSampleWithOffset( TInt aEffectHandle,
        const TDesC8& aStreamingSample,
        TInt aOffsetTime,
        TRequestStatus& aStatus ) = 0;

    /**
    * Destroys a streaming effect previously created in a successful
    * call to CreateStreamingEffect().
    *
    * @param[in] aEffectHandle Handle to the streaming effect to destroy.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
    virtual TInt DestroyStreamingEffect( TInt aEffectHandle ) = 0;

    /**
    * Modifies a playing MagSweep effect.
    *
    * Synchronous method returns when the haptic command has been evaluated
    * and the return value is valid.
    *
    * @param[in] aEffectHandle Handle to the playing MagSweep effect
    *                          to modify. The handle to the effect must have
    *                          been obtained by calling
    *                          PlayMagSweepEffect(),
    *                          PlayEffect() or
    *                          PlayEffectRepeat().
    * @param[in] aEffect Reference to a struct defining the effect parameters.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @see THWRMHapticsMagSweepEffect for effect definition.
    *
    * @since S60 5.1
    */
    virtual TInt ModifyPlayingMagSweepEffect( TInt aEffectHandle,
        const CHWRMHaptics::THWRMHapticsMagSweepEffect& aEffect ) = 0;

    /**
    * @overload
    *
    * @param[out] aStatus Request status. On completion contains:
    *                     KErrNone, if successful,
    *                     otherwise one of the other system-wide error codes.
    */
    virtual void ModifyPlayingMagSweepEffect( TInt aEffectHandle,
        const CHWRMHaptics::THWRMHapticsMagSweepEffect& aEffect,
        TRequestStatus& aStatus ) = 0;

    /**
    * Modifies a playing periodic effect.
    *
    * Synchronous method returns when the haptic command has been evaluated
    * and the return value is valid.
    *
    * @param[in] aEffectHandle Handle to the playing periodic effect
    *                          to modify. The handle to the effect must have
    *                          been obtained by calling
    *                          PlayPeriodicEffect(),
    *                          PlayEffect() or
    *                          PlayEffectRepeat().
    * @param[in] aEffect Reference to a struct defining the effect parameters.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @see THWRMHapticsPeriodicEffect for effect definition.
    *
    * @since S60 5.1
    */
    virtual TInt ModifyPlayingPeriodicEffect( TInt aEffectHandle,
        const CHWRMHaptics::THWRMHapticsPeriodicEffect& aEffect ) = 0;

    /**
    * @overload
    *
    * @param[out] aStatus Request status. On completion contains:
    *                     KErrNone, if successful,
    *                     otherwise one of the other system-wide error codes.
    */
    virtual void ModifyPlayingPeriodicEffect( TInt aEffectHandle,
        const CHWRMHaptics::THWRMHapticsPeriodicEffect& aEffect,
        TRequestStatus& aStatus ) = 0;


    /**
    * Load effect data defined in effect data buffer (obtained e.g. from a
    * file containing the effect data).
    *
    * @param[in] aData Reference to allocated effect data buffer.
    *
    * @param[out] aFileHandle On return contains a handle to the passed
    *                         effect data. This handle is haptics specific,
    *                         i.e., it is not a file system handle but just
    *                         a handle to the loaded effect data buffer.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    * @return TInt KErrArgument if the effect data is invalid.
    *
    * @since S60 5.1
    */
    virtual TInt LoadEffectData( const TDesC8& aData, TInt& aFileHandle ) = 0;

    /**
    * Delete loaded effect data referenced by file handle.
    *
    * @param[in] aFileHandle Handle to file.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
    virtual TInt DeleteEffectData( TInt aFileHandle ) = 0;

    /**
    * Delete all loaded effect datas.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
    virtual TInt DeleteAllEffectData() = 0;

    /**
    * Plays an effect defined in loaded effect data buffer.
    *
    * Synchronous method returns when the haptic command has been evaluated
    * and the return value is valid.
    *
    * @param[in] aFileHandle Handle to the loaded effect data.
    *
    * @param[in] aEffectIndex Index of the effect to play. The index of the
    *                         effect must be greater than or equal to 0 and
    *                         less than the number of effects returned by
    *                         GetEffectCount().
    * @param[out] aEffectHandle Reference to the variable that receives
    *                           a handle to the playing effect.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    * @return TInt KErrInUse when haptics is reserved for a higher or 
    *              equal priority client.
    *
    * @since S60 5.1
    */
    virtual TInt PlayEffect( TInt aFileHandle, 
                             TInt aEffectIndex, 
                             TInt& aEffectHandle ) = 0;

    /**
    * @overload
    *
    * @param[out] aStatus Request status. On completion contains:
    *                     KErrNone, if successful,
    *                     KErrInUse when haptics is reserved for a higher or 
    *                     equal priority client,
    *                     otherwise one of the other system-wide error codes.
    */
    virtual void PlayEffect( TInt aFileHandle,
                             TInt aEffectIndex,
                             TInt& aEffectHandle,
                             TRequestStatus& aStatus ) = 0;

    /**
    * Repeatedly plays a Timeline effect defined in loaded effect data buffer.
    *
    * The current implementation of PlayEffectRepeat() repeats only
    * Timeline effects. If the given effect index refers to a simple effect,
    * PlayEffectRepeat() ignores the aRepeat parameter and plays the
    * simple effect once. In that case, PlayEffectRepeat() behaves
    * like PlayEffect(). PlayEffectRepeat() does not return a warning
    * when requested to repeat a simple effect.
    *
    * Synchronous method returns when the haptic command has been evaluated
    * and the return value is valid.
    *
    * @param[in] aFileHandle Handle to the loaded effect data.
    * @param[in] aEffectIndex Index of the effect to play. The index of the
    *                         effect must be greater than or equal to 0 and
    *                         less than the number of effects returned by
    *                         GetEffectCount().
    * @param[in] aRepeat Number of times to repeat the effect. To play the
    *                    effect indefinitely, set aRepeat to
    *                    InfiniteRepeat(). To repeat the effect a
    *                    finite number of times, set aRepeat to a value from
    *                    0 to  InfiniteRepeat() - 1.
    *                    The effect can be repeated at most
    *                    InfiniteRepeat() - 1 times.
    *                    Setting aRepeat to 0 plays the effect once (repeats
    *                    the effect zero times) and is equivalent to calling
    *                    PlayEffect().
    *                    To stop the effect before it has
    *                    repeated the requested number of times or to stop
    *                    an effect that is playing indefinitely, call
    *                    StopPlayingEffect() or StopAllPlayingEffects()
    * @param[out] aEffectHandle Reference to the variable that receives
    *                           a handle to the playing effect.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    * @return TInt KErrInUse when haptics is reserved for a higher or 
    *              equal priority client.
    *
    * @since S60 5.1
    */
    virtual TInt PlayEffectRepeat( TInt aFileHandle,
                                   TInt aEffectIndex,
                                   TUint8 aRepeat,
                                   TInt& aEffectHandle ) = 0;

    /**
    * @overload
    *
    * @param[out] aStatus Request status. On completion contains:
    *                     KErrNone, if successful,
    *                     KErrInUse when haptics is reserved for a higher or 
    *                     equal priority client,
    *                     otherwise one of the other system-wide error codes.
    */
    virtual void PlayEffectRepeat( TInt  aFileHandle,
                                   TInt aEffectIndex,
                                   TUint8 aRepeat,
                                   TInt& aEffectHandle,
                                   TRequestStatus& aStatus ) = 0;

    /**
    * Plays an effect defined in effect data buffer.
    *
    * Synchronous method returns when the haptic command has been evaluated
    * and the return value is valid.
    *
    * @param[in] aData Reference to effect data buffer containing the 
    *                  definition of the effect to play.
    * @param[in] aEffectIndex Index of the effect to play. The index of the
    *                         effect must be greater than or equal to 0 and
    *                         less than the number of effects returned by
    *                         GetEffectCount().
    * @param[out] aEffectHandle Reference to the variable that receives
    *                           a handle to the playing effect.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    * @return TInt KErrArgument if the data is invalid.
    * @return TInt KErrInUse when haptics is reserved for a higher or 
    *              equal priority client.
    *
    * @since S60 5.1
    */
    virtual TInt PlayEffect( const TDesC8& aData,
                             TInt aEffectIndex,
                             TInt& aEffectHandle ) = 0;

    /**
    * @overload
    *
    * @param[out] aStatus Request status. On completion contains:
    *                     KErrNone, if successful,
    *                     KErrArgument, if the data is invalid,
    *                     KErrInUse when haptics is reserved for a higher or 
    *                     equal priority client,
    *                     otherwise one of the other system-wide error codes.
    */
    virtual void PlayEffect( const TDesC8& aData,
                             TInt aEffectIndex,
                             TInt& aEffectHandle,
                             TRequestStatus& aStatus ) = 0;

    /**
    * Repeatedly plays a Timeline effect defined in effect data buffer.
    *
    * The current implementation of PlayEffectRepeat() repeats only
    * Timeline effects. If the given effect index refers to a simple effect,
    * PlayEffectRepeat() ignores the aRepeat parameter and plays the
    * simple effect once. In that case, PlayEffectRepeat() behaves
    * similarly to PlayEffect(). PlayEffectRepeat() does not return a warning
    * when requested to repeat a simple effect.
    *
    * Synchronous method returns when the haptic command has been evaluated
    * and the return value is valid.
    *
    * @param[in] aData Reference to effect data buffer containing the
    *                  definition of the effect to play.
    * @param[in] aEffectIndex Index of the effect to play. The index of the
    *                         effect must be greater than or equal to 0 and
    *                         less than the number of effects returned by
    *                         GetEffectCount().
    * @param[in] aRepeat Number of times to repeat the effect. To play the
    *                    effect indefinitely, set aRepeat to
    *                    InfiniteRepeat(). To repeat the effect a
    *                    finite number of times, set aRepeat to a value from
    *                    0 to InfiniteRepeat() - 1. The effect can
    *                    be repeated at most InfiniteRepeat() - 1
    *                    times.
    *                    Setting aRepeat to 0 plays the effect once (repeats
    *                    the effect zero times) and is equivalent to calling
    *                    PlayEffect().
    *                    To stop the effect before it has
    *                    repeated the requested number of times or to stop
    *                    an effect that is playing indefinitely, call
    *                    StopPlayingEffect() or StopAllPlayingEffects()
    * @param[out] aEffectHandle Reference to the variable that receives
    *                           a handle to the playing effect.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    * @return TInt KErrArgument if the data is invalid.
    * @return TInt KErrInUse when haptics is reserved for a higher or 
    *              equal priority client.
    *
    * @since S60 5.1
    */
    virtual TInt PlayEffectRepeat( const TDesC8& aData,
                                   TInt aEffectIndex,
                                   TUint8 aRepeat,
                                   TInt& aEffectHandle )=0;

    /**
    * @overload
    *
    * @param[out] aStatus Request status. On completion contains:
    *                     KErrNone, if successful,
    *                     KErrArgument, if the data is invalid,
    *                     KErrInUse when haptics is reserved for a higher or 
    *                     equal priority client,
    *                     otherwise one of the other system-wide error codes.
    */
    virtual void PlayEffectRepeat( const TDesC8& aData,
                                   TInt aEffectIndex,
                                   TUint8 aRepeat,
                                   TInt& aEffectHandle,
                                   TRequestStatus& aStatus )=0;

    /**
    * Plays a MagSweep effect given in the parameters defining the effect.
    *
    * Synchronous method returns when the haptic command has been evaluated
    * and the return value is valid.
    *
    * @param[in] aEffect Reference to a struct defining the effect parameters.
    * @param[out] aEffectHandle Reference to the variable that receives
    *                           a handle to the playing effect.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    * @return TInt KErrInUse when haptics is reserved for a higher or 
    *              equal priority client.
    *
    * @see THWRMHapticsMagSweepEffect for effect definition.
    *
    * @since S60 5.1
    */
    virtual TInt PlayMagSweepEffect(
        const CHWRMHaptics::THWRMHapticsMagSweepEffect& aEffect,
        TInt& aEffectHandle ) = 0;

    /**
    * @overload
    *
    * @param[out] aStatus Request status. On completion contains:
    *                     KErrNone, if successful,
    *                     KErrInUse when haptics is reserved for a higher or 
    *                     equal priority client,
    *                     otherwise one of the other system-wide error codes.
    */
    virtual void PlayMagSweepEffect(
        const CHWRMHaptics::THWRMHapticsMagSweepEffect& aEffect,
        TInt& aEffectHandle,
        TRequestStatus& aStatus ) = 0;

    /**
    * Plays a Periodic effect given in the parameters defining the effect.
    *
    * Synchronous method returns when the haptic command has been evaluated
    * and the return value is valid.
    *
    * @param[in] aEffect Reference to a struct defining the effect parameters.
    * @param[out] aEffectHandle Reference to the variable that receives
    *                           a handle to the playing effect.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    * @return TInt KErrInUse when haptics is reserved for a higher or 
    *              equal priority client.
    *
    * @see THWRMHapticsPeriodicEffect for effect definition.
    *
    * @since S60 5.1
    */
    virtual TInt PlayPeriodicEffect(
        const CHWRMHaptics::THWRMHapticsPeriodicEffect& aEffect,
        TInt& aEffectHandle ) = 0;

    /**
    * @overload
    *
    * @param[out] aStatus Request status. On completion contains:
    *                     KErrNone, if successful,
    *                     KErrInUse when haptics is reserved for a higher or 
    *                     equal priority client,
    *                     otherwise one of the other system-wide error codes.
    */
    virtual void PlayPeriodicEffect(
        const CHWRMHaptics::THWRMHapticsPeriodicEffect& aEffect,
        TInt& aEffectHandle,
        TRequestStatus& aStatus ) = 0;

    /**
    * Pauses a playing effect.
    *
    * @param[in] aEffectHandle Handle to the playing effect to pause.
    *                          The handle to the effect must have been
    *                          obtained by calling
    *                          PlayMagSweepEffect(),
    *                          PlayPeriodicEffect() ,
    *                          PlayEffect() ,
    *                          PlayEffectRepeat() or
    *                          CreateStreamingEffect().
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
     virtual TInt PausePlayingEffect( TInt aEffectHandle ) = 0;

    /**
    * Resumes playback on a paused effect from the point where
    * the effect was paused.
    *
    * Depending on the available slots, it is possible that all simple
    * effects from a paused effect data buffer or streaming sample cannot
    * be resumed. The API returns success when it is able to play one of
    * these simple effects.
    *
    * @param[in] aEffectHandle Handle to the paused effect to resume.
    *                          The handle to the effect must have been
    *                          obtained by calling
    *                          PlayMagSweepEffect(),
    *                          PlayPeriodicEffect(),
    *                          PlayEffect(),
    *                          PlayEffectRepeat() or
    *                          CreateStreamingEffect().
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
    virtual TInt ResumePausedEffect( TInt aEffectHandle ) = 0;

    /**
    * Stops a playing effect.
    *
    * @param[in] aEffectHandle Handle to the playing effect to stop.
    *                          The handle to the effect must have been
    *                          obtained by calling
    *                          PlayMagSweepEffect(),
    *                          PlayPeriodicEffect(),
    *                          PlayEffect(),
    *                          PlayEffectRepeat() or
    *                          CreateStreamingEffect().
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
    virtual TInt StopPlayingEffect( TInt aEffectHandle ) = 0;

    /**
    * Stops all playing and paused effects of a haptics instance.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
    virtual TInt StopAllPlayingEffects() = 0;

    /**
    * Get a number of effects defined in a loaded effect data buffer.
    *
    * @param[in] aFileHandle Handle to the loaded effect data buffer.
    * @param[out] aCount Number of effects in the effect data buffer.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
    virtual TInt GetEffectCount ( TInt aFileHandle,
                                  TInt& aCount ) const = 0;

    /**
    * Get the duration of an effect defined in a loaded effect data buffer.
    *
    * @param[in] aFileHandle Handle to the loaded effect data buffer.
    * @param[in] aEffectIndex Effect for which the duration is wanted.
    * @param[out] aEffectDuration Reference to the variable that receives
    *                             the requested effect's duration value.
    *                             Duration is in milliseconds.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    * @since S60 5.1
    */
    virtual TInt GetEffectDuration ( TInt aFileHandle,
                                     TInt aEffectIndex,
                                     TInt& aEffectDuration ) const = 0;

    /**
    * Gets the index of an effect defined in a loaded effect data buffer
    * from the name of the effect.
    *
    * @param[in] aFileHandle Handle to the loaded effect data buffer.
    * @param[in] aEffectName Name of the effect for which the index is wanted.
    * @param[out] aEffectIndex Reference to the variable that receives
    *             the requested effect's index value.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
    virtual TInt GetEffectIndexFromName ( TInt aFileHandle,
                                          const TDesC8& aEffectName,
                                          TInt& aEffectIndex ) const = 0;

    /**
    * Gets the type of an effect defined in loaded effect data buffer.
    *
    * @param[in] aFileHandle Handle to loaded effect data buffer.
    * @param[in] aEffectIndex Index of an effect for which a type is wanted.
    * @param[out] aEffectType Reference to the variable that receives
    *                         the requested effect's type value.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
    virtual TInt GetEffectType( TInt aFileHandle,
                                TInt aEffectIndex,
                                TInt& aEffectType ) const = 0;

    /**
    * Gets the name of an effect defined in a loaded effect data buffer.
    *
    * @param[in] aFileHandle Handle to the loaded effect data buffer.
    * @param[in] aEffectIndex Index of an effect for which a name is wanted.
    * @param[out] aEffectName Reference to the variable that receives
    *                         the requested effect's name. Note that the 
    *                         descriptor's size must be at least the size
    *                         given from MaxPropertyStringLength().
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @since S60 5.1
    */
    virtual TInt GetEffectName( TInt aFileHandle,
                                TInt aEffectIndex,
                                TDes8& aEffectName ) const = 0;

    /**
    * Gets the parameters of a MagSweep effect defined in a loaded effect data
    * buffer.
    *
    * @param[in] aFileHandle Handle to the loaded effect data buffer.
    * @param[in] aEffectIndex Index of an effect for which a definition is
    *                         wanted.
    * @param[out] aEffect Reference to the variable that receives
    *                     the effect definition.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @see THWRMHapticsMagSweepEffect for effect definition.
    *
    * @since S60 5.1
    */
    virtual TInt GetMagSweepEffectDefinition( 
        TInt aFileHandle,
        TInt aEffectIndex,
        CHWRMHaptics::THWRMHapticsMagSweepEffect& aEffect ) const = 0;

    /**
    * Gets the parameters of a periodic effect defined in a loaded effect data
    * buffer.
    *
    * @param[in] aFileHandle Handle to the loaded effect data buffer.
    * @param[in] aEffectIndex Index of an effect for which a definition is wanted.
    * @param[out] aEffect Reference to the variable that receives
    *                     the effect definition.
    *
    * @return TInt KErrNone if successful, otherwise one of the other
    *              system-wide error codes.
    *
    * @see THWRMHapticsPeriodicEffect for effect definition.
    *
    * @since S60 5.1
    */
    virtual TInt GetPeriodicEffectDefinition( 
        TInt aFileHandle,
        TInt aEffectIndex,
        CHWRMHaptics::THWRMHapticsPeriodicEffect& aEffect ) const = 0;

    /**
    * Gets the value that represents infinite repeats. Method may be used
    * only after an actuator has been opened successfully.
    *
    * @return TInt Value that represents infinite repeats. KErrNotReady, if
    * an actuator has not been opened.
    */
    virtual TInt InfiniteRepeat() const = 0;

    /**
    * Gets the value that represents infinite duration. Method may be used
    * only after an actuator has been opened successfully.
    *
    * @return TInt Value that represents infinite duration. KErrNotReady, if
    * an actuator has not been opened.
    */
    virtual TInt InfiniteDuration() const = 0;

    /**
    * Gets the maximum length of an effect name stored in a loaded
    * effect data file. Method may be used only after an actuator 
    * has been opened successfully.
    *
    * @return TInt Maximum effect name length. KErrNotReady, if
    * an actuator has not been opened.
    */
    virtual TInt MaxEffectNameLength() const = 0;

    /** 
    * Gets the maximum device name length.  Method may be used
    * only after an actuator has been opened successfully.
    *
    * @return TInt Maximum device name length. KErrNotReady, if
    * an actuator has not been opened.
    */
    virtual TInt MaxDeviceNameLength() const = 0;

    /**
    * Gets the maximum capability string length. Method may be used
    * only after an actuator has been opened successfully.
    *
    * @return TInt Maximum capability string length. KErrNotReady, if
    * an actuator has not been opened.
    */
    virtual TInt MaxCapabilityStringLength() const = 0;

    /**
    * Gets the maximum property string length. Method may be used
    * only after an actuator has been opened successfully.
    *
    * @return TInt Maximum property string length. KErrNotReady, if
    * an actuator has not been opened.
    */
    virtual TInt MaxPropertyStringLength() const = 0;

    /**
    * Gets the maximum streaming sample size. Method may be used
    * only after an actuator has been opened successfully.
    *
    * @return TInt Maximum streaming sample size. KErrNotReady, if
    * an actuator has not been opened.
    */
    virtual TInt MaxStreamingSampleSize() const = 0;

    /**
    * Gets the default device priority. Method may be used
    * only after an actuator has been opened successfully.
    *
    * @return TInt Default device property. KErrNotReady, if
    * an actuator has not been opened.
    */
    virtual TInt DefaultDevicePriority() const = 0;
    };

#endif
