/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This API provides access to the display driver.
*
*/

#ifndef __TVOUTCONFIG_H__
#define __TVOUTCONFIG_H__

//- Include Files  ----------------------------------------------------------

#include <e32base.h>
#include <tvoutconfigdef.h>

//- Class and Enumeration Definitions ---------------------------------------

/** API version number:
 2:  Symbian PREQ2102 changes. SettingsChangedListener, SetAvailableTvConfigList and PAR for CVBS
 1:  First version, which have ApiVersion method. @see ApiVersion. */
const TUint KTvOutConfigVersion = 2;

/**
Adaptation API for Tv-out settings
*/
class CTvOutConfig : public CBase
    {
    public:

        /** Gets API version number. Compare this to KTvOutConfigVersion, must match.
            @return  KTvOutConfigVersion which is used for implementation */
        IMPORT_C static TUint ApiVersion();

        ///Destructor
        virtual ~CTvOutConfig(){};

        /** Creates object.
            Method requires ECapabilityReadDeviceData capability.
            @return  new CTvOutConfig, NULL if there is error, otherwise CTvOutConfig */
        IMPORT_C static CTvOutConfig* NewL();

        //-- Capability and Settings ------------------------------------------

        /** Reads number of supported modes what to be called with GetSupportedHdmiMode.
            @see GetSupportedHdmiMode
            @param  aNumberOfModes  Method fills these for caller
            @return  KErrNone if success, KErrNotSupported if not supported. */
        virtual TInt GetNumberOfHdmiModes(TUint& aNumberOfModes) = 0;

        /** Reads supported modes of HDMI hardware
            @see GetNumberOfHdmiModes
            @param  aModeNumber  Mode number, begins from 0, up to GetNumberOfHdmiModes.
            @param  aReadMode    Method fills these for caller
            @return  KErrNone if success, KErrNotSupported if not supported. */
        virtual TInt GetSupportedHdmiMode(TUint aModeNumber, TSupportedHdmiDviMode& aReadMode) = 0;

        /** Reads capabilities of own HDMI hardware.
            @param  aConnector  Capabilities to be fetched for this connector type
            @param  aReadCapabilities  Method fills these for caller
            @return  KErrNone if success, KErrNotSupported if not supported. */
        virtual TInt GetTvHwCapabilities(TTvSettings::TOutput aConnector, THwCapabilities& aReadCapabilities) = 0;

        /** Before Tv-out using, SetConfig must be called (during boot and always
            when settings or connector are changed).
            Fill correct values to TTvSettings object and give it by parameter.

            Some settings can be changed on the fly (while tv-out enabled, some settings
            will be in use only after Tv-out Disable()/Enable() sequency. For example,
            if e.g. overscan is changed on the fly, client gives same early filled struct but only
            overscan members are different. KErrNone will be returned if change was success,
            if HW cannot change settings on the fly (without Disable()/Enable()) then
            KErrNotReady is returned. When KErrNotReady is returned, settings are needed
            to write with KErrNone after Disable() and before Enable()

            If this is called, THdmiDviTimings will not be used anymore and vice versa.

            @param  aTvSettings  Object which contains wanted setting (e.g. PAL/NTSC/...)
            @return Symbian error code, KErrNotSupported if not supported,
                                        KErrArgument if aTvSettings was ignored,
                                        KErrNotFound when Tv-out driver not found.
                                        KErrNotReady - Disable()/Enable() sequence is needed */
        virtual TInt SetConfig(const TTvSettings& aTvSettings) = 0;

        /** Sets HDMI settings to Hw
            @see SetConfig(const TTvSettings& aTvSettings) */
        virtual TInt SetConfig(const THdmiDviTimings& aTvSettings) = 0;

        /** Use CTvOutConfig::OnOffListener method, when Tv is enabled, then new
            settings can be read using this method.

            If this read fails, try also read overloaded version for THdmiDviTimings reading, and vice versa.
            Latest set version of GetConfig can only return without error, that is the version
            what is currently used in HW (TTvSettings/THdmiDviTimings)

            @pre SetConfig is called without errors
            @param  aTvSettings  Object which contains wanted setting (e.g. PAL/NTSC/...)
            @return Symbian error code, KErrNotSupported if not supported,
                                        KErrArgument if aTvSettings was ignored,
                                        KErrNotFound when Tv-out driver not found. */
        virtual TInt GetConfig(TTvSettings& aTvSettings) = 0;

        ///@see GetConfig(TTvSettings& aTvSettings)
        virtual TInt GetConfig(THdmiDviTimings& aTvSettings) = 0;

        /** Adds listener for changed settings
            Check always aListenerRequest.Int(), it is KErrNone when normal operation.
            @param  aListenerRequest Will be completed without errors when HDMI cable connected/disconnected
            @return Symbian error code */
        virtual TInt SettingsChangedListener(TRequestStatus& aListenerRequest) = 0;

        /** Cancels listener, @see SettingsChangedListener
            @return Symbian error code */
        virtual TInt SettingsChangedListenerCancel() = 0;

        //-- Standby Figure ---------------------------------------------------

        /** Returns maximum size of TStandByFigure.iTable in bytes. Deprecated since TB10.2.
            @param  aMaxFigureSize  Max size of standByFigure in bytes
            @return  KErrNotSupported if not supported. */
        virtual TInt StandByFigureMaxSizeInBytes(TUint& aMaxFigureSize) = 0;

        /** Sets user defined figure to show on tv. Deprecated since TB10.2.
            @pre SetConfig() is called without errors.
            @param  aStandByFigure  TStandByFigure object.
            @return KErrNone if all success */
        virtual TInt StandByFigure(const TStandByFigure& aStandByFigure) = 0;

        /** Gets user defined figure to show on tv. Deprecated since TB10.2.
            @see CTvOutConfig::StandByFigureMaxSizeInBytes for getting size of TStandByFigure.iTable.
            @pre StandByFigure() is called without errors.
            @param  aStandByFigure  TStandByFigure object.
            @return KErrNone if all success */
        virtual TInt GetStandByFigure(TStandByFigure& aStandByFigure) = 0;

        //-- Enable and Disable -----------------------------------------------

        /** Enables Tv-out. Latest SetConfig() settings are used.
            @pre SetConfig() is called without errors.
            @return Symbian Error Code, KErrNotSupported if not supported,
                                        KErrNotReady if SetConfig() failed,
                                        KErrAlreadyExists if was enabled,
                                        KErrServerBusy if previous task is going,
                                                       try again later, not immediately */
        virtual TInt Enable() = 0;

        /** Disables Tv-out.
            @pre Enable() is called without errors.
            @return Symbian Error Code, KErrNotSupported if not supported,
                                        KErrNotReady if SetConfig() failed,
                                        KErrAlreadyExists if was disabled,
                                        KErrServerBusy if previous task is going,
                                                       try again later, not immediately */
        virtual TInt Disable() = 0;

        //-- Listeners and Status ---------------------------------------------

        /** Adds listener for tv-out enable/disable situations.
            Check always aListenerRequest.Int(), it is KErrNone when normal operation.
            @param  aListenerRequest Will be completed without errors when tv-out
                                     changing state to on or off.
            @return Symbian error code */
        virtual TInt OnOffListener(TRequestStatus& aListenerRequest) = 0;

        /** Cancels listener, @see OnOffListener
            @return Symbian error code */
        virtual TInt OnOffListenerCancel() = 0;

        /** Asks is TV-out enabled or not
            @return State of TV-out, EFalse when TV-out is disabled or if not supported (or error occured),
                                     ETrue when TV-out is enabled */
        virtual TBool Enabled() = 0;

        /** Adds listener for HDMI connected/disconnected situations.
            Check always aListenerRequest.Int(), it is KErrNone when normal operation.
            @param  aListenerRequest Will be completed without errors when HDMI cable connected/disconnected
            @return Symbian error code */
        virtual TInt HdmiCableListener(TRequestStatus& aListenerRequest) = 0;

        /** Cancels listener, @see HdmiCableListener
            @return Symbian error code */
        virtual TInt HdmiCableListenerCancel() = 0;

        /** Asks is HDMI-cable connected or not
            @return State of HDMI-cable, EFalse when HDMI cable is not connected or if not supported
                                         ETrue when HDMI cable is connected */
        virtual TBool HdmiCableConnected() = 0;

        // Following functions are needed for discussion with Tv-Manager:

        /** Set list of available Tv-configurations
            @param  aAnalogConfigs  Available Analog Tv configurations
            @param  aHdmiConfigs    Available HDMI/DVI Tv configurations
            @return Symbian error code */
        virtual TInt SetAvailableTvConfigList(const RArray<TTvSettings>& aAnalogConfigs, const RArray<THdmiDviTimings>& aHdmiConfigs) = 0;

        /** Get list of available Tv-configurations
            @param  aAnalogConfigs  Available Analog Tv configurations
            @param  aHdmiConfigs    Available HDMI/DVI Tv configurations
            @return Symbian error code */
        virtual TInt GetAvailableTvConfigList(RArray<TTvSettings>& aAnalogConfigs, RArray<THdmiDviTimings>& aHdmiConfigs) = 0;

        /** Listener for SetAvailableTvConfigList()
            Check always aListenerRequest.Int(), it is KErrNone when normal operation.
            @param  aListenerRequest Will be completed without errors when SetAvailableTvConfigList is called
            @return Symbian error code */
        virtual TInt AvailableTvConfigListListener(TRequestStatus& aListenerRequest) = 0;

        /** Cancels listener, @see AvailableTvConfigListListener
            @return Symbian error code */
        virtual TInt AvailableTvConfigListListenerCancel() = 0;

        //-- Protection -------------------------------------------------------

        /** Listener for status of copy protection (e.g. HDCP). After this, check newest status with CopyProtectionStatus()
            HDCP may drop off with CopyProtection(EFalse) or when authentication is failed.
            @see CopyProtectionStatus()
            @param  aListenerRequest Will be completed without errors when HDCP status in output will change (ESS)
            @return Symbian error code */
        virtual TInt CopyProtectionStatusListener(TRequestStatus& aListenerRequest) = 0;

        /** Cancels listener, @see CopyProtectionStatusListener
            @return Symbian error code */
        virtual TInt CopyProtectionStatusCancel() = 0;

        /** Asks is HDCP/macrovision enabled or not at the moment in Tv-output.
            @pre Tv-out is enabled (Enabled() returns ETrue)
            @return ETrue when protection is enabled on Tv-output, EFalse otherwise */
        virtual TBool CopyProtectionStatus() = 0;

        /** Enables/disables HDCP/Macrovision on Tv-output. All clients of CTvOutConfig has to
            disable protection, before protection on output can be disabled.
            CopyProtectionStatus() will be completed when protection state is changed (not necessarily ever).

            ETrue = When client wants to enable protection.
            Protection is enabled after CopyProtectionStatus() returns ETrue (after listener).
            If sink does not support e.g. the HDCP, adaptation will try to enable that periodically
            and will trigger the listener when success and state changes.

            EFalse = When client wants to disable protection. If any other CTvOutConfig's client is not
            called this method with ETrue, output will be unprotected after CopyProtectionStatusListener().
            When HDCP is dropped out (CopyProtectionStatusListener()), this method should be called
            with EFalse (before next try with ETrue), that tells for adaptation that buffer content
            can be shown on Tv without protection.

            @param  aCopyProtectEnabled  ETrue when output is wanted to be protected
            @return Symbian error code, KErrNotSupported if feature is missing. */
        virtual TInt CopyProtection(TBool aCopyProtectEnabled) = 0;

    protected:
        ///Constructor
        CTvOutConfig(){};
        ///Second phase constructor
        virtual void ConstructL() = 0;
    };

//- Inline Functions --------------------------------------------------------

#endif //__TVOUTCONFIG_H__

// End of File


