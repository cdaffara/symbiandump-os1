/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is the main interface to FMRadioTunerControl Adaptation API.
*
*/



#ifndef C_FMRADIOTUNERCONTROL_H
#define C_FMRADIOTUNERCONTROL_H

#include <e32base.h>

class MRdsControl;
class MRdsControlObserver;

/**
 *  FM tuner error definitions.
 *  In addition to the standard system error code, the following error codes may
 *  be returned.
 */
typedef TInt TFMRadioTunerError;
const TFMRadioTunerError KFMRadioErrHardwareFaulty              = -12050;
const TFMRadioTunerError KFMRadioErrTuning                      = -12051;
const TFMRadioTunerError KFMRadioErrFrequencyOutOfBandRange     = -12052;
const TFMRadioTunerError KFMRadioErrFrequencyNotValid           = -12053;
const TFMRadioTunerError KFMRadioErrNotPermittedInCurrentState 	= -12054;
const TFMRadioTunerError KFMRadioErrDuplicateRequest            = -12055;
const TFMRadioTunerError KFMRadioErrAntennaNotConnected	        = -12056;
const TFMRadioTunerError KFMRadioErrFmTransmitterActive	        = -12057;


/**  FM radio frequency range */
enum TFMRadioFrequencyRange
	{
	EFMRadioFrequencyEuroAmerica	= 0x01,		// European and American FM frequency range
	EFMRadioFrequencyJapan			= 0x02,		// Japanese FM frequency range
	EFMRadioFrequencyAmericaNarrow  = 0x04		// North American band (87.9-108 Mhz)
	};


/**  FM radio seek direction */
enum TFMRadioSeekDirection
	{
	EFMRadioSeekUp,
	EFMRadioSeekDown
	};


/**  FM radio audio mode  */
enum TFMRadioAudioMode
	{
	EFMRadioAudioMono	= 0x01,
	EFMRadioAudioStereo	= 0x02
	};

/**  FM radio sample rates */
enum TFMRadioSampleRate
	{
	EFMRadioSampleRate8000Hz  = 0x0001,
	EFMRadioSampleRate11025Hz = 0x0002,
	EFMRadioSampleRate12000Hz = 0x0004,
	EFMRadioSampleRate16000Hz = 0x0008,
	EFMRadioSampleRate22050Hz = 0x0010,
	EFMRadioSampleRate24000Hz = 0x0020,
	EFMRadioSampleRate32000Hz = 0x0040,
	EFMRadioSampleRate44100Hz = 0x0080,
	EFMRadioSampleRate48000Hz = 0x0100,
	EFMRadioSampleRate64000Hz = 0x0200,
	EFMRadioSampleRate88200Hz = 0x0400,
	EFMRadioSampleRate96000Hz = 0x0800
	};


/**
 *  The FM Tuner Capabilities class defines the capabilities of the FM tuner on the
 *  device, as retrieved using the function GetCapabilities. Adaptation can utilize this
 *  class to indicate the hardware limitations.
 *
 *  @since S60 3.0
 */
class TFMRadioCapabilities
    {

#ifdef RD_FM_RADIO_ENHANCEMENTS

    // Tuner Features bit flags - may be extended in future
    enum TFMTunerFeatures
        {
		// This bit is set if the tuner can be used even when the device is in
		// offline mode. Some tuner may not allow this due to hw conflict.
		ETunerAvailableInOfflineMode	  = 0x01,
        // This bit is set if the tuner has RDS support.
		ETunerRdsSupport                  = 0x02,
		// This bit is set when the device supports dual tuner, which allows
		// more extensive RDS feature support.
		ETunerDualTunerSupport            = 0x04
		};
#endif

	public:
		// Bitmask using TFMRadioSampleRate
		TUint	iSampleRate;
		// Bitmask using TFMRadioAudioMode
		TUint	iChannels;
		// Encodings supported - FourCC
		TUint32	iEncoding ;

		// Bitmask using TFMRadioFrequencyRange; used to indicate supported ranges.
		TUint32 iTunerBands;
		// Bitmask using TFMTunerFeatures; used to indicate supported tuner features.
		TUint32 iTunerFunctions;
	    // Reserved for future
	    TUint32 iAdditionalFunctions1;
	    // Reserved for future
    	TUint32 iAdditionalFunctions2;

	};


/**
 *  This class provides the interface class to be implemented by object receiving request
 *  complete notification from FM tuner control. These functions are called once the asynchronous
 *  request completes successfully or with an error.
 *
 *  In addition, this class provides the interface for receiving events from FM tuner control.
 *
 *  @since S60 3.0
 */
class MFMRadioTunerControlObserver
	{
public:

    /**
     * Notification mechanism provided to inform client that TunerOn request has completed.
     *
     * @since S60 3.0
     * @param aError The result of the request indicating whether the operation was
     *               successful or not.
     */
    virtual void TunerOnComplete( TFMRadioTunerError aError ) = 0;

    /**
     * Notification mechanism provided to inform client that TunerOff request has completed.
     *
     * @since S60 3.0
     * @param aError The result of the request indicating whether the operation was
     *               successful or not.
     */
    virtual void TunerOffComplete( TFMRadioTunerError aError ) = 0;

    /**
     * Notification mechanism provided to inform client that SetFrequencyRange request has completed.
     *
     * @since S60 3.0
     * @param aError The result of the request indicating whether the operation was
     *               successful or not.
     */
    virtual void SetFrequencyRangeComplete( TFMRadioTunerError aError ) = 0;

    /**
     * Notification mechanism provided to inform client that SetFrequency request has completed.
     *
     * @since S60 3.0
     * @param aError The result of the request indicating whether the operation was
     *               successful or not.
     */
    virtual void SetFrequencyComplete( TFMRadioTunerError aError ) = 0;

    /**
     * Notification mechanism provided to inform client that StationSeek request has completed.
     *
     * @since S60 3.0
     * @param aError The result of the request indicating whether the operation was
     *               successful or not.
     * @param aFrequency The frequency(Hz) of the radio station that was found.
     */
    virtual void StationSeekComplete( TFMRadioTunerError aError, TInt aFrequency ) = 0;

    /**
     * Notification mechanism provided to inform client that AudioMode request has completed.
     *
     * @since S60 3.0
     * @param aError The result of the request indicating whether the operation was
     *               successful or not.
     * @param aMode The audio mode of the radio.
     */
    virtual void AudioModeComplete( TFMRadioTunerError aError, TFMRadioAudioMode aMode ) = 0;

    /**
     * Notification mechanism provided to inform client that SetAudioMode request has completed.
     *
     * @since S60 3.0
     * @param aError The result of the request indicating whether the operation was
     *               successful or not.
     */
    virtual void SetAudioModeComplete( TFMRadioTunerError aError ) = 0;

    /**
     * Notification mechanism provided to inform client that MaxSignalStrength request has completed.
     *
     * @since S60 3.0
     * @param aError The result of the request indicating whether the operation was
     *               successful or not.
     * @param aStrength The maximum possible signal strength of a tuned signal.
     */
    virtual void MaxSignalStrengthComplete( TFMRadioTunerError aError, TInt aStrength ) = 0;

    /**
     * Notification mechanism provided to inform client that SignalStrength request has completed.
     *
     * @since S60 3.0
     * @param aError The result of the request indicating whether the operation was
     *               successful or not.
     * @param aStrength The signal strength of the currently tuned frequency.
     */
    virtual void SignalStrengthComplete( TFMRadioTunerError aError, TInt aStrength ) = 0;

    /**
     * Notification mechanism provided to inform client that Squelch request has completed.
     *
     * @since S60 3.0
     * @param aError The result of the request indicating whether the operation was
     *               successful or not.
     * @param aEnabled ETrue if squelching is enabled, EFalse if disabled.
     */
    virtual void SquelchComplete( TFMRadioTunerError aError, TBool aEnabled ) = 0;

    /**
     * Notification mechanism provided to inform client that SetSquelch request has completed.
     *
     * @since S60 3.0
     * @param aError The result of the request indicating whether the operation was
     *               successful or not.
     */
    virtual void SetSquelchComplete( TFMRadioTunerError aError ) = 0;

    /**
     * Notification mechanism provided to inform client that BufferToBeFilled request has completed.
     *
     * @since S60 3.0
     * @param aBuffer Buffer with data to be played.
     */
    virtual void BufferFilled( TDes8& aBuffer ) = 0;

    /**
     * Notification mechanism provided to inform that the antenna status has changed.
     *
     * @since S60 3.0
     * @param aAttached ETrue if antenna is attached.
     */
    virtual void RadioEventAntennaStatusChange( TBool aAttached ) = 0;

#ifdef RD_FM_RADIO_ENHANCEMENTS
    /**
     * Notification mechanism indicating tuner control change. When tuner receiver is
     * forced to be turned off due to hardware conflict such as a FM transmitter, this
     * event is sent with an error code.
     * NOTE: Once the tuner control is restored, this event is sent again with KErrNone
     * and the client can re-issue TunerOn.
     *
     * @since S60 3.2
     * @param aError A standard system error code or FM tuner error (TFMRadioTunerError).
     */
    virtual void RadioEventTunerControlChange( TInt aError ) = 0;
#endif

	};

/**
 *  This class provides an interface to allow clients to control the FM tuner hardware
 *  present on a device.
 *
 *  @lib FMRadioTunerControl.lib
 *  @since S60 3.0
 */
class CFMRadioTunerControl : public CBase
	{
public:  // Constructors

    /**
     * Factory function to create a new instance of the tuner control.
     *
     * @since S60 3.0
     * @param aObserver The observer object
     * @return A pointer to a new instance of this class
     */
    IMPORT_C static CFMRadioTunerControl* NewL( MFMRadioTunerControlObserver& aObserver );

public: // New functions

    /**
     * Turns on the tuner. The method does not block, but returns immediately
     * and once the tuner is actually turned on the calling client will receive
     * a notification via the callback mechanism.
     *
     * @since S60 3.0
     * @param aRange The frequency range the tuner should be set to.
     * @param aFrequency The frequency (Hz) the tuner should be tuned to.
     */
    virtual void TunerOn( TFMRadioFrequencyRange aRange, TInt aFrequency ) = 0;

    /**
     * Cancels an outstanding TunerOn request. The method blocks while cancelling
     * the outstanding request. It is possible that the outstanding request may
     * complete normally before the cancel request can be processed.
     *
     * @since S60 3.0
     */
    virtual void CancelTunerOn() = 0;

    /**
     * Turns off the tuner. The method does not block, but returns immediately and
     * once the tuner is actually turned off the calling client will receive a
     * notification via the callback mechanism.
     * @since S60 3.0
     */
    virtual void TunerOff() = 0;

    /**
     * Cancels an outstanding TunerOff request. The method blocks while cancelling
     * the outstanding request. It is possible that the outstanding request may
     * complete normally before the cancel request can be processed.
     *
     * @since S60 3.0
     */
    virtual void CancelTunerOff() = 0;

    /**
     * Sets the tuner to the specified frequency range. The method does not block,
     * but returns immediately and once the frequency range has been set the calling
     * client will receive a notification via the callback mechanism.
     *
     * When not set, default to EFMRadioFrequencyEuroAmerica.
     *
     * This method can only be called when tuner is off. Otherwise, KErrInUse will be
     * returned in callback method.
     *
     * @since S60 3.0
     * @param aRange The frequency range the tuner should be set to.
     */
    virtual void SetFrequencyRange( TFMRadioFrequencyRange aRange ) = 0;

    /**
     * Cancels an outstanding SetFrequencyRange request. The method blocks while
     * cancelling the outstanding request. It is possible that the outstanding
     * request may complete normally before the cancel request can be processed.
     *
     * @since S60 3.0
     */
    virtual void CancelSetFrequencyRange() = 0;

    /**
     * Tunes the tuner to the specified frequency. The method does not block,
     * but returns immediately and once the tuner has been tuned the calling
     * client will receive a notification via the callback mechanism.
     *
     * @since S60 3.0
     * @param aFrequency The frequency (Hz) the tuner should be tuned to.
     */
    virtual void SetFrequency( TInt aFrequency ) = 0;

    /**
     * Cancels an outstanding SetFrequency request. The method blocks while
     * cancelling the outstanding request. It is possible that the outstanding
     * request may complete normally before the cancel request can be processed.
     *
     * @since S60 3.0
     */
    virtual void CancelSetFrequency() = 0;

    /**
     * Finds a station, starting at the currently tuned frequency and searches in
     * the direction specified (i.e. up or down). The method does not block, but
     * returns immediately and once a station is found the calling client will
     * receive a notification via the callback mechanism. The station found is
     * returned in the callback.
     *
     * @since S60 3.0
     * @param aDirection The direction to search in.
     */
    virtual void StationSeek( TFMRadioSeekDirection aDirection ) = 0;

    /**
     * Cancels an outstanding StationSeek request. The method blocks while
     * cancelling the outstanding request. It is possible that the outstanding
     * request may complete normally before the cancel request can be processed.
     *
     * @since S60 3.0
     */
    virtual void CancelStationSeek() = 0;

    /**
     * Retrieves the tuner’s current audio mode (i.e. stereo or mono). The
     * method does not block, but returns immediately and once the request
     * is completed the calling client will receive a notification via the
     * callback mechanism. Audio mode is returned in the callback.
     *
     * @since S60 3.0
     */
    virtual void AudioMode() = 0;

    /**
     * Cancels an outstanding AudioMode request. The method blocks while cancelling
     * the outstanding request. It is possible that the outstanding request may
     * complete normally before the cancel request can be processed.
     *
     * @since S60 3.0
     */
    virtual void CancelAudioMode() = 0;

    /**
     * Sets the tuner’s audio mode (i.e. stereo or mono). The method does not block,
     * but returns immediately and once the audio mode is actually set the calling
     * client will receive a notification via the callback mechanism.
     *
     * When not set, default to EFMRadioAudioStereo.
     *
     * @since S60 3.0
     * @param aMode The audio mode the tuner should be set to.
     */
    virtual void SetAudioMode( TFMRadioAudioMode aMode ) = 0;

    /**
     * Cancels an outstanding SetAudioMode request. The method blocks while cancelling
     * the outstanding request. It is possible that the outstanding request may
     * complete normally before the cancel request can be processed.
     *
     * @since S60 3.0
     */
    virtual void CancelSetAudioMode() = 0;

    /**
     * Retrieves the maximum possible signal strength (RSSI) of a tuned signal.
     * The method does not block, but returns immediately and once the request
     * is completed the calling client will receive a notification via the callback
     * mechanism. Maximun signal strength is returned in the callback.
     *
     * @since S60 3.0
     */
    virtual void MaxSignalStrength() = 0;

    /**
     * Cancels an outstanding MaxSignalStrength request. The method blocks while
     * cancelling the outstanding request. It is possible that the outstanding
     * request may complete normally before the cancel request can be processed.
     *
     * @since S60 3.0
     */
    virtual void CancelMaxSignalStrength() = 0;

    /**
     * Retrieves the signal strength (RSSI) of the currently tuned frequency.
     * The method does not block, but returns immediately and once the request
     * is completed the calling client will receive a notification via the callback
     * mechanism. Signal strength is returned in the callback.
     *
     * @since S60 3.0
     */
    virtual void SignalStrength() = 0;

    /**
     * Cancels an outstanding SignalStrength request. The method blocks while
     * cancelling the outstanding request. It is possible that the outstanding
     * request may complete normally before the cancel request can be processed.
     *
     * @since S60 3.0
     */
    virtual void CancelSignalStrength() = 0;

    /**
     * Retrieves the current squelching (muting in frequencies without reception)
     * setting. The method does not block, but returns immediately and once the
     * request is completed the calling client will receive a notification via
     * the callback mechanism. Squelch setting is returned in the callback.
     *
     * @since S60 3.0
     */
    virtual void Squelch() = 0;

    /**
     * Cancels an outstanding Squelch request. The method blocks while cancelling
     * the outstanding request. It is possible that the outstanding request may
     * complete normally before the cancel request can be processed.
     *
     * @since S60 3.0
     */
    virtual void CancelSquelch() = 0;

    /**
     * Sets the current squelching (muting in frequencies without reception) setting.
     * The method does not block, but returns immediately and once the squelch setting
     * is actually set the calling client will receive a notification via the callback
     * mechanism.
     *
     * When not set, default to enabled.
     *
     * @since S60 3.0
     * @param aEnabled ETrue to enable squelching, EFalse to disable it.
     */
    virtual void SetSquelch( TBool aEnabled ) = 0;

    /**
     * Cancels an outstanding SetSquelch request. The method blocks while cancelling
     * the outstanding request. It is possible that the outstanding request may
     * complete normally before the cancel request can be processed.
     *
     * @since S60 3.0
     */
    virtual void CancelSetSquelch() = 0;

    /**
     * This function is called when data is needed for playing. BufferFilled should
     * be called as quickly as possible after the data is read into buffer or an
     * underflow situation may occur.
     *
     * @since S60 3.0
     * @param aBuffer Buffer into which data should be read.
     */
    virtual void BufferToBeFilled( TDes8& aBuffer ) = 0;

    /**
     * Retrieves the audio capabilities supported by the device.
     *
     * @since S60 3.0
     * @return Supported audio capabilities
     */
    virtual TFMRadioCapabilities Capabilities() = 0;

#ifdef RD_FM_RADIO_ENHANCEMENTS
    /**
     * Returns a pointer to RDS control.
     *
     * @since S60 3.2
     * @param aObserver RDS control observer
     */
    virtual MRdsControl* RdsControl( MRdsControlObserver& aObserver ) = 0;

    /**
     * Gets the current frequency range. It also returns the minimum and maximum frequencies (Hz)
     * for the returned range. Client should always use the minimum and maximum frequencies returned
     * by the tuner device, since it may vary slightly depending on the hardware.
     *
     * @since S60 3.2
     * @param aRange On return contains the current frequency range.
     * @param aMinFreq On return contains the minimum frequency for the current frequency range.
     * @param aMaxFreq On return contains the maximum frequency for the current frequency range.
     * @return A standard system error code or FM tuner error (TFMRadioTunerError).
     */
    virtual TInt GetFrequencyRange( TFMRadioFrequencyRange& aRange, TInt& aMinFreq, TInt& aMaxFreq ) const = 0;
#endif

	};

#endif      // C_FMRADIOTUNERCONTROL_H

