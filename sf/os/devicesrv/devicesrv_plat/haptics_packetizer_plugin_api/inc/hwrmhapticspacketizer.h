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
* Description:  Haptics packetizer interface.
*
*/

#ifndef HWRMHAPTICSPACKETIZER_H
#define HWRMHAPTICSPACKETIZER_H

#include <e32base.h>
#include <badesca.h>
#include <hwrmhaptics.h>

/**
 * Interface for Haptics packetizer. Includes functionality
 * for encoding message data, decoding response data and 
 * getters for constants.
 *
 * @since S60 5.1
 */
class MHWRMHapticsPacketizer
    {
public:

    /**
     * Destructor.
     *
     * @since S60 5.1
     */ 
    virtual ~MHWRMHapticsPacketizer() {};

public: // data encoding

    /**
     * Creates request data for opening a device/actuator.
     * 
     * @param aLogicalActuator Logical actuator type to be opened.
     * @param aBuffer On return contains the request data buffer. 
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncOpenDeviceReq( THWRMLogicalActuators aLogicalActuator,
                                   RBuf8& aBuffer ) =0;

    /**
     * Creates request data for closing device/actuator.
     * 
     * @param aDeviceHandle Device/actuator to be closed.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncCloseDeviceReq( TInt aDeviceHandle, RBuf8& aBuffer ) =0;

    /**
     * Creates request data for playing a magsweep effect.
     * 
     * @param aDeviceHandle Handle to the device/actuator.
     * @param aEffect Effect definition data.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncPlayMagSweepEffectReq( 
                        TInt aDeviceHandle, 
                        CHWRMHaptics::THWRMHapticsMagSweepEffect aEffect,
                        RBuf8& aBuffer ) =0;

    /**
     * Creates request data for playing a periodic effect.
     * 
     * @param aDeviceHandle Handle to the device/actuator.
     * @param aEffect Effect definition data.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncPlayPeriodicEffectReq( 
                        TInt aDeviceHandle, 
                        CHWRMHaptics::THWRMHapticsPeriodicEffect aEffect,
                        RBuf8& aBuffer ) =0;

    /**
     * Creates request data for modifying mag sweep effect.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aEffectHandle Handle to the existing effect.
     * @param aEffect New data for the effect definition.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncModifyPlayingMagSweepEffectReq( 
                        TInt aDeviceHandle, 
                        TInt aEffectHandle, 
                        CHWRMHaptics::THWRMHapticsMagSweepEffect aEffect,
                        RBuf8& aBuffer ) =0;

    /**
     * Creates request data for modifying periodic effect.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aEffectHandle Handle to the existing effect.
     * @param aEffect New data for the effect definition.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncModifyPlayingPeriodicEffectReq( 
                        TInt aDeviceHandle, 
                        TInt aEffectHandle, 
                        CHWRMHaptics::THWRMHapticsPeriodicEffect aEffect,
                        RBuf8& aBuffer ) =0;

    /**
     * Creates request data for pausing effect play.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aEffectHandle Handle to the existing effect.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncPausePlayingEffectReq( TInt aDeviceHandle,
                                           TInt aEffectHandle,
                                           RBuf8& aBuffer ) =0;

    /**
     * Creates request data for resuming paused effect play.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aEffectHandle Handle to the existing effect.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncResumePausedEffectReq( TInt aDeviceHandle, 
                                           TInt aEffectHandle,
                                           RBuf8& aBuffer ) =0;

    /**
     * Creates request data for stopping effect play.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aEffectHandle Handle to the existing effect.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncStopPlayingEffectReq( TInt aDeviceHandle, 
                                          TInt aEffectHandle,
                                          RBuf8& aBuffer ) =0;

    /**
     * Creates request data for stopping all effect play.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncStopAllPlayingEffectsReq( TInt aDeviceHandle,
                                              RBuf8& aBuffer ) =0;

    /**
     * Creates request data for playing periodic effect from effect data.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aData Effect data, which includes the effect to be played.
     * @param aEffectIndex Index of the effect in the effect data.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncPlayEffectIncludeEffectDataReq( TInt aDeviceHandle,
                                                    const TDesC8& aData, 
                                                    TInt aEffectIndex,
                                                    RBuf8& aBuffer ) =0;

    /**
     * Creates request data for playing repeating effect from effect data.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aData Effect data, which includes the effect to be played.
     * @param aEffectIndex Index of the effect in the effect data.
     * @param aRepeat The number of times to repeat the effect.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncPlayEffectRepeatIncludeEffectDataReq( TInt aDeviceHandle,
                                                          const TDesC8& aData,
                                                          TInt aEffectIndex, 
                                                          TUint8 aRepeat,
                                                          RBuf8& aBuffer ) =0;

    /**
     * Creates request data for playing repeating effect from loaded 
     * effect data.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aEffectIndex Index of the effect in the effect data.
     * @param aRepeat The number of times to repeat the effect.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncPlayEffectRepeatNoDataReq( TInt aDeviceHandle,
                                               TInt aEffectIndex, 
                                               TUint8 aRepeat,
                                               RBuf8& aBuffer ) =0;

    /**
     * Creates request data for playing effect from loaded effect data.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aEffectIndex  Index of the effect in the effect data.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncPlayEffectNoDataReq( TInt aDeviceHandle,
                                         TInt aEffectIndex,
                                         RBuf8& aBuffer ) =0;

    /**
     * Creates request data for creating streaming effect.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncCreateStreamingEffectReq( TInt aDeviceHandle,
                                              RBuf8& aBuffer ) =0;

    /**
     * Creates request data for destroying streaming effect.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aEffectHandle Handle to the existing effect.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncDestroyStreamingEffectReq( TInt aDeviceHandle,
                                               TInt aEffectHandle,
                                               RBuf8& aBuffer ) =0;

    /**
     * Creates request data for playing streaming sample.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aStreamingSample Sample data.
     * @param aEffectHandle Handle to an effect.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncPlayStreamingSampleReq( TInt aDeviceHandle,
                                            const TDesC8& aStreamingSample, 
                                            TInt aEffectHandle,
                                            RBuf8& aBuffer ) =0;

    /**
     * Creates request data for playing streaming sample with offset.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aStreamingSample Sample data.
     * @param aOffsetTime Offset time.
     * @param aEffectHandle Handle to an effect.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncPlayStreamingSampleWithOffsetReq( 
                                            TInt aDeviceHandle,
                                            const TDesC8& aStreamingSample,
                                            TInt aOffsetTime,
                                            TInt aEffectHandle,
                                            RBuf8& aBuffer ) =0;
    
    /**
     * Creates request data for getting effect state.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aEffectHandle Handle to the existing effect.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncGetEffectStateReq( TInt aDeviceHandle, 
                                       TInt aEffectHandle,
                                       RBuf8& aBuffer ) =0;

    /**
     * Creates request data for setting boolean device property.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aDevPropValue Device property value.
     * @param aDevPropType Type of the property to be set.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncSetDevicePropertyBoolReq( TInt aDeviceHandle, 
                                              TBool aDevPropValue,
                                              TInt aDevPropType,
                                              RBuf8& aBuffer ) =0;

    /**
     * Creates request data for setting integer device property.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aDevPropValue Device property value.
     * @param aDevPropTypeType Type of the property to be set.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncSetDevicePropertyIntReq( TInt aDeviceHandle, 
                                             TInt aDevPropValue, 
                                             TInt aDevPropType,
                                             RBuf8& aBuffer ) =0;

    /**
     * Creates request data for setting string device property.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aDevPropValue Device property value.
     * @param aDevPropType Type of the property to be set.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncSetDevicePropertyStringReq( TInt aDeviceHandle, 
                                                const TDesC8& aDevPropValue,
                                                TInt aDevPropType,
                                                RBuf8& aBuffer ) =0;

    /**
     * Creates request data for setting platform license key.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncSetPlatformLicenseKeyReq( TInt aDeviceHandle,
                                              RBuf8& aBuffer ) =0;

    /**
     * Creates request data for getting boolean device property.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aDevPropType  Type of the property to be fetched.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncGetDevicePropertyBoolReq( TInt aDeviceHandle,
                                              TInt aDevPropType,
                                              RBuf8& aBuffer ) =0;

    /**
     * Creates request data for getting integer device property.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aDevPropType  Type of the property to be fetched.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncGetDevicePropertyIntReq( TInt aDeviceHandle,
                                             TInt aDevPropType,
                                             RBuf8& aBuffer ) =0;

    /**
     * Creates request data for getting string device property.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aDevPropType Type of the property to be fetched.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncGetDevicePropertyStringReq( TInt aDeviceHandle,
                                                TInt aDevPropType,
                                                RBuf8& aBuffer ) =0;

    /**
     * Creates request data for getting integer device capability.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aDevCapType Type of the capability to be fetched.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncGetDeviceCapabilityIntReq( TInt aDeviceHandle, 
                                               TInt aDevCapType,
                                               RBuf8& aBuffer ) =0;

    /**
     * Creates request data for getting string device capability.
     * 
     * @param aDeviceHandle Handle to the used device/actuator.
     * @param aDevCapType Type of the capability to be fetched.
     * @param aBuffer On return contains the request data buffer.
     *
     * @since S60 5.1
     *
     * @return KErrNone if succesful, otherwise one of the system wide
     * error codes.
     */ 
    virtual TInt EncGetDeviceCapabilityStringReq( TInt aDeviceHandle, 
                                                  TInt aDevCapType,
                                                  RBuf8& aBuffer ) =0;

public: // data decoding

    /**
     * Decodes given message and fills the data received in the message
     * to the returned array.
     *
     * @param aData   Received response data.
     * @param aStatus On return, contains the received status value found 
     * in the response data.
     *
     * @since S60 5.1
     *
     * @return Array filled with values in the response data.
     */
    virtual CDesC8ArraySeg* DecodeMessageL( const TDesC8& aData, 
                                            TInt& aStatus ) =0;
    
    /**
     * Returns device handle value set in DecodeMessageL(). 
     * If device is not open, the value is KErrNotFound.
     *
     * @since S60 5.1
     *
     * @return Device handle value or KErrNotFound.
     */
    virtual TInt DeviceHandle() =0;

public: // constant getters

    /**
     * Returns infinite repeat constant value.
     *
     * @since S60 5.1
     *
     * @return Infinite repeat value.
     */
    virtual TInt InfiniteRepeat() =0;

    /**
     * Returns infinite duration constant value.
     *
     * @since S60 5.1
     *
     * @return Infinite duration value.
     */
    virtual TInt InfiniteDuration() =0;

    /**
     * Returns maximum effect name length constant value.
     *
     * @since S60 5.1
     *
     * @return Maximum effect name length value.
     */
    virtual TInt MaxEffectNameLength() =0;

    /**
     * Returns maximum device name length constant value.
     *
     * @since S60 5.1
     *
     * @return Maximum device name length value.
     */
    virtual TInt MaxDeviceNameLength() =0;

    /**
     * Returns maximum capability string length constant value.
     *
     * @since S60 5.1
     *
     * @return Maximum capability string length value.
     */
    virtual TInt MaxCapabilityStringLength() =0;

    /**
     * Returns maximum property string length constant value.
     *
     * @since S60 5.1
     *
     * @return Maximum property string length value.
     */
    virtual TInt MaxPropertyStringLength() =0;

    /**
     * Returns maximum streaming sample size constant value.
     *
     * @since S60 5.1
     *
     * @return Maximum streaming sample size.
     */
    virtual TInt MaxStreamingSampleSize() =0;

    /**
     * Returns default device priority constant value.
     *
     * @since S60 5.1
     *
     * @return Default device priority.
     */
    virtual TInt DefaultDevicePriority() =0;
    };


/**
 * Factory and base class implementation for Haptics packetizer plugins.
 * 
 * @lib hwrmhapticspacketizer.lib
 * @since S60 5.1
 */
class CHWRMHapticsPacketizer : public CBase, 
                               public MHWRMHapticsPacketizer
    {
public:

    /**
     * Factory method for creating a packetizer plugin. Leaves with 
     * KErrNotSupported, if no plugin implementation based on the 
     * logical actuator type was found.
     *
     * @param aLogicalActuator Type of the logical actuator, which is
     * currently being used. The actuator type affects to which type of 
     * plugin is opened.
     *
     * @return Pointer to the newly created instance.
     */
    IMPORT_C static CHWRMHapticsPacketizer* NewL( 
                          THWRMLogicalActuators aLogicalActuator );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CHWRMHapticsPacketizer();

private:

    /**
     * Unique instance identifier key.
     */
    TUid iDtor_ID_Key;
    };

#endif // HWRMHAPTICSPACKETIZER_H
