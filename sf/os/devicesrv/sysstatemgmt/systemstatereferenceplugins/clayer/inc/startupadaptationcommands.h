/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface   : Domain, Startup Adaptation API
* This header file defines the startup adaptation API commands
* and their parameters.
* This file contains the following sections:
* - Command definitions
* - Command parameter definitions
* - Response parameter definitions
*
*/



#ifndef __STARTUPADAPTATIONCOMMANDS_H__
#define __STARTUPADAPTATIONCOMMANDS_H__

#include <e32std.h>

/*****************************************************
*       Series 60 Customer
*       Needs customer implementation
*****************************************************/

/**
* All command and event definitions of Startup Adaptation API belong to
* StartupAdaptation namespace.
*
* @file
* @publishedPartner
* @released
*
*/
namespace StartupAdaptation
    {

    /**
    * Command definitions
    * All commands contain a command id and a set of parameters. The command ids
    * are defined first and after that parameters for each command.
    *
    * This interface is asynchronous. A command is interpreted of being
    * completed after a response for that command is received.
    *
    * @param field means the parameter for the command.
    * @return field means the parameter for the response.
    */
    enum TCommand
        {
        /**
        * This command is used to change the global state of an S60 device.
        *
        * When changing the global state, S60 starter server issues a state
        * change command to startup adaptation via the startup adaptation API.
        * After receiving a response, S60 starter server will publish the new
        * state using Publish & Subscribe and execute operations required in
        * that state.
        *
        * @param  TGlobalStateChangePckg The requested global state.
        * @return TResponsePckg The success status of the state change.
        */
        EGlobalStateChange = 100,

        /**
        * Security state change request. Used only when global state is security
        * check.
        *
        * @param  TSecurityStateChangePckg The requested security state.
        * @return TSecurityStateResponsePckg The success status of the state
        * change and, if the operation was successful, additional information
        * which depends on the state.
        */
        ESecurityStateChange = 101,

        /**
        * Get global startup mode.
        *
        * The global startup mode indicates the mode in which the device has
        * been started. The startup mode can also be seen as a reason for the
        * startup.
        *
        * @param  None.
        * @return TGlobalStartupModeResponsePckg The success status of the
        * operation and, if the operation was successful, the global startup
        * mode.
        */
        EGetGlobalStartupMode = 102,

        /**
        * Request Domestic OS to perform self tests.
        *
        * Before launching any software components that are using some specific
        * hardware, self-tests need to be executed for the hardware. This
        * command is used to request licensee-specific self-tests to be
        * executed. It is allowed to perform the self-tests before this command,
        * but when this command is issued, S60 starter server expects to get the
        * result of the self-tests.
        *
        * @param  None.
        * @return TResponsePckg The success status of the self tests.
        */
        EExecuteSelftests = 103,

        /**
        * Get the language codes that are stored in the SIM card preferred
        * language lists.
        *
        * Primary language list will be queried first. Unless it contains
        * at least one language that can be taken into use, the secondary
        * preferred list will be queried for. Only if there is no acceptable
        * languages in the secondary list either, will the tertiary list be
        * queried for.
        *
        * @param  TLanguageListPriorityPckg.
        * @return TLanguageListResponsePckg The list of languages mapped to
        * Symbian language codes.
        */
        EGetSIMLanguages = 104,

        /**
        * This command is used to check the validity of Real Time Clock (RTC).
        *
        * @param  None.
        * @return TResponsePckg The success status of the operation. KErrNone
        * if the RTC time is valid, any other value indicates that it is not
        * valid.
        */
        EValidateRTCTime = 107,

        /**
        * Query whether the current SIM card is different from the previous one.
        *
        * The SIM changed status indicates whether the currently inserted SIM
        * card has changed since the last startup. This information can be used
        * for example to query whether the user wants to clear phone call logs.
        *
        * S60 starter server makes this query only if there is a SIM card
        * present. This command will only be issued in ESWStateCriticalPhaseOK
        * state.
        *
        * @param  None.
        * @return TBooleanResponsePckg Success status of the operation, and if
        * the operation was successful, indication whether the SIM card has been
        * changed after the last boot or not.
        */
        EGetSimChanged = 108, //This Enum should not be used.

        /**
        * Query whether the current SIM card is among the last used cards.
        *
        * It is up to the startup adaptation to decide the number of SIM cards
        * that are considered owned.
        *
        * If the SIM owned status is false, then the SIM changed status must be
        * true.
        *
        * S60 starter server makes this query only if there is a SIM card
        * present. This command will only be issued in ESWStateCriticalPhaseOK
        * state.
        *
        * @param  None.
        * @return TBooleanResponsePckg Success status of the operation, and if
        * the operation was successful, indication whether the SIM card is among
        * the five last used cards or not.
        */
        EGetSimOwned = 109,

        /**
        * This command is used to set a wake-up alarm time so that the device is
        * powered up by an alarm when it goes off, i.e. the device will be
        * started in the alarm mode.
        *
        * @see Command EGetGlobalStartupMode.
        *
        * @param  TWakeupAlarmPckg The time for the alarm.
        * @return TResponsePckg Success status of the operation.
        */
        ESetWakeupAlarm = 110,

        /**
        * This command is used to cancel a wake-up alarm.
        *
        * @param  None.
        * @return None Cancels must always succeed.
        */
        ECancelWakeupAlarm = 111,

        /**
        * A reboot request.
        *
        * This command is used to inform the startup adaptation that a reset
        * will be executed. The command also reports the reason for the reset
        * in case there is a need for some licensee-specific actions.
        *
        * This command will only be executed in the state ESWStateShuttingDown.
        * After receiving a response to the command, the S60 starter server will
        * only clean up itself and then call UserHal::SwitchOff().
        *
        * @param  TResetReasonPckg The reason for resetting.
        * @return TResponsePckg Success status of the operation.
        */
        EExecuteReset = 112,

        /**
        * A shutdown request.
        *
        * This command is used to inform the startup adaptation that the device
        * is going to be shut down.
        *
        * This command will only be executed in the state ESWStateShuttingDown.
        * After receiving a response to the command, the S60 starter server will
        * only clean up itself and then call UserHal::SwitchOff().
        *
        * @param  None.
        * @return TResponsePckg Success status of the operation.
        */
        EExecuteShutdown = 113,

        /**
        * Restore the factory settings of Domestic OS.
        *
        * Restoring factory settings may require some licensee-specific actions
        * to be performed. This command is used to request those actions to be
        * executed. The command will be issued during start-up sequence
        * following a reset caused by user-initiated RFS operation.
        *
        * @param  TRFSReasonPckg Indicates whether normal or deep RFS is
        * required.
        * @return TResponsePckg Success status of the operation.
        */
        EExecuteDOSRfs = 114,

        /**
        * Activate RF tranceiver for making an emergency call.
        *
        * Emergency calls have a high priority in S60 devices. Emergency calls
        * must be allowed whenever they are technically possible. There are two
        * cases, where the state of the device does not allow a mobile
        * originated call, but it must allow an emergency call.
        * - When the device is in offline mode. Applies also for the global
        *   state ESWStateEmergencyCallsOnly.
        * - When the device startup is in progress and RF transceiver is not
        *   yet switched on.
        * This command is used to switch RF transceiver on for making an
        * emergency call. As soon as the emergency call is finished, there will
        * be a command for switching RF transceiver off.
        *
        * @see Command EDeactivateRfAfterEmergencyCall.
        *
        * @param  None.
        * @return TResponsePckg Success status of the operation.
        */
        EActivateRfForEmergencyCall = 115,

        /**
        * Deactivate RF transceiver after making an emergency call.
        *
        * This command is used to switch RF transceiver off after an emergency
        * call.
        *
        * @see Command EActivateRfForEmergencyCall.
        *
        * @param  None.
        * @return TResponsePckg Success status of the operation.
        */
        EDeactivateRfAfterEmergencyCall = 116,
        
        /**
        * Query whether the current boot is a hidden reset.
        * Hidden reset means that boot-time queries and animations should not
        * be shown to the user.
        *
        * Boot-time queries and animations will be hidden from the user if the
        * system is executing a controlled reset (EExecuteReset command) or
        * this query returns ETrue.
        *
        * This query should return ETrue if the ongoing boot is caused by SW
        * reset (either controlled, initiated by EExecuteReset command, or
        * uncontrolled, caused by critical SW failure).
        * This query should return EFalse if the ongoing boot is initiated by
        * any other means (such as user pressing the power key or a wakeup
        * alarm occurring).
        *
        * This command will always be issued in ESWStateStartingUiServices
        * state.
        */
        EGetHiddenReset = 117

        };

    /*
    * ==========================================================================
    * Command parameter definitions
    * ==========================================================================
    */

    /**
    * Parameter definition for EGlobalStateChange.
     * Parameter indicates the global state to move to.
    *
    * Possible state transitions are:
    * ESWStateStartingUiServices -> ESWStateStartingCriticalApps
    * ESWStateStartingCriticalApps -> ESWStateSelfTestOK
    * ESWStateSelfTestOK -> ESWStateSecurityCheck
    * ESWStateSelfTestOK -> ESWStateCharging
    * ESWStateSelfTestOK -> ESWStateAlarm
    * ESWStateSelfTestOK -> ESWStateTest
    * ESWStateSecurityCheck -> ESWStateCriticalPhaseOK
    * ESWStateSecurityCheck -> ESWStateEmergencyCallsOnly
    * ESWStateCriticalPhaseOK -> ESWStateNormalRfOn
    * ESWStateCriticalPhaseOK -> ESWStateNormalRfOff
    * ESWStateNormalRfOn -> ESWStateNormalRfOff
    * ESWStateNormalRfOn -> ESWStateNormalBTSap
    * ESWStateNormalRfOff -> ESWStateNormalRfOn
    * ESWStateNormalRfOff -> ESWStateNormalBTSap
    * ESWStateNormalBTSap -> ESWStateNormalRfOn
    * ESWStateNormalBTSap -> ESWStateNormalRfOff
    * ESWStateCharging -> ESWStateChargingToAlarm
    * ESWStateCharging -> ESWStateChargingToNormal
    * ESWStateAlarm -> ESWStateAlarmToCharging
    * ESWStateAlarm -> ESWStateAlarmToNormal
    * ESWStateChargingToAlarm -> ESWStateAlarm
    * ESWStateAlarmToCharging -> ESWStateCharging
    * ESWStateChargingToNormal -> ESWStateSecurityCheck
    * ESWStateAlarmToNormal -> ESWStateSecurityCheck
    * (Any state except ESWStateStartingUiServices, ESWStateTest,
    *  ESWStateFatalStartupError and ESWStateShuttingDown) -> ESWStateFatalStartupError
    * (Any state except ESWStateTest and ESWStateShuttingDown) -> ESWStateShuttingDown
    */
    enum TGlobalState
        {
        ESWStateStartingUiServices = 100,
        ESWStateStartingCriticalApps,
        ESWStateSelfTestOK,

        // States for the security check phase.
        ESWStateSecurityCheck,
        ESWStateCriticalPhaseOK,
        ESWStateEmergencyCallsOnly,

        // Terminal states defined by the boot mode (and some other variables such as offline mode).
        ESWStateTest,
        ESWStateCharging,
        ESWStateAlarm,
        ESWStateNormalRfOn,
        ESWStateNormalRfOff,
        ESWStateNormalBTSap,

        // States for notifying adaptation about a terminal state change.
        ESWStateAlarmToCharging,
        ESWStateChargingToAlarm,
        ESWStateChargingToNormal,
        ESWStateAlarmToNormal,

        // Shutdown-related states.
        ESWStateShuttingDown,

        // Error states.
        ESWStateFatalStartupError
        };

    /**
    * Buffer descriptor containing TGlobalState object.
    */
    typedef TPckgBuf<TGlobalState> TGlobalStateChangePckg;

    /**
    * Parameter definition for ESecurityStateChange.
    * Parameter indicates the security state to move to.
    *
    * Possible state transitions are:
    * ESIMPresent -> ESIMReadable
    * ESIMPresent -> ESIMLessOfflineSupported
    * ESIMReadable -> ESIMInvalid
    * ESIMInvalid -> ESecurityCheckFailed
    * ESIMInvalid -> ESIMRejected
    * ESIMRejected -> ESIMBlocked
    * ESIMRejected -> ESecurityCheckFailed
    * ESIMBlocked -> EPINRequired
    * ESIMBlocked -> EAskPUK
    * EPINRequired -> EAskPIN
    * EPINRequired -> ESIMCodesOK
    * EAskPIN -> EAskPUK
    * EAskPIN -> ESIMCodesOK
    * EAskPUK -> ESIMCodesOK
    * EAskPUK -> ESecurityCheckFailed
    * ESIMCodesOK -> ESIMLock
    * ESIMCodesOK -> ESECCodeRequired
    * ESIMLock -> ESecurityCheckOK
    * ESIMLock -> ESecurityCheckFailed
    * ESIMLessOfflineSupported -> ESIMLock
    * ESIMLessOfflineSupported -> ESECCodeRequired
    * ESIMLessOfflineSupported -> ESecurityCheckFailed
    * ESECCodeRequired -> EAskSEC
    * ESECCodeRequired -> ESECCodeOK
    * EAskSEC -> ESECBlocked
    * EAskSEC -> ESECCodeOK
    * ESECBlocked -> EAskSEC
    * ESECCodeOK -> ESIMLock
    */
    enum TSecurityState
        {
        ESIMPresent = 100,       ///< Response param: Yes/No
        ESIMReadable,            ///< Response param: None (wait until SIM readable)
        ESIMRejected,            ///< Response param: Yes/No
        ESIMBlocked,             ///< Response param: No/PUK1/UPUK
        EPINRequired,            ///< Response param: No/PIN1/UPIN
        ESECCodeRequired,        ///< Response param: No/SEC/Master

        ESIMLessOfflineSupported,///< Response param: None (inform adaptation only)
        ESIMLock,                ///< Response param: SimLockRestricted/SimLockRestrictionOn/
                                 ///< SimLockRestrictionPending/SimLockOk
        ESECBlocked,             ///< Response param: None (inform adaptation only)

        EAskPIN,                 ///< Response param: None (inform adaptation only)
        EAskPUK,                 ///< Response param: None (inform adaptation only)
        EAskSEC,                 ///< Response param: None (inform adaptation only)
        ESIMCodesOK,             ///< Response param: None (inform adaptation only)
        ESECCodeOK,              ///< Response param: None (inform adaptation only)
        ESecurityCheckOK,        ///< Response param: None (inform adaptation only)
        ESecurityCheckFailed,     ///< Response param: None (inform adaptation only)
        ESIMInvalid              ///< Response param: Yes/No
        };

    /**
    * Buffer descriptor containing TSecurityState object.
    */
    typedef TPckgBuf<TSecurityState> TSecurityStateChangePckg;


    /**
    * Parameter definition for EGetSIMLanguages.
    */
    enum TLanguageListPriority
        {
        /**
        * Return list of language codes based on the most preferred language
        * list. In GSM the most preferred language list is stored in EF_LI
        * (see 3GPP TS 31.102).
        */
        EPrimaryLanguages = 100,
        /**
        * Return list of language codes based on the second preferred language
        * list. In GSM the second preferred language list is stored in EF_PL
        * (see 3GPP TS 51.011).
        */
        ESecondaryLanguages,
        /**
        * Return list of language codes based on the third preferred language
        * list. In GSM the third preferred language list is stored in EF_LP
        * (see 3GPP TS 51.011).
        */
        ETertiaryLanguages,
        };

    /**
    * Buffer descriptor containing TLanguageListPriority object.
    */
    typedef TPckgBuf<TLanguageListPriority> TLanguageListPriorityPckg;

    /**
    * Parameter definition for ESetWakeupAlarm and ECancelWakeupAlarm.
    */
    NONSHARABLE_STRUCT( TWakeupAlarmTime )
        {
        /**
        * The local time for the alarm. Startup adaptation needs to take the
        * offsets into account.
        */
        TTime iTime;
        /**
        * Universal time offset in seconds.
        */
        TTimeIntervalSeconds iUniversalTimeOffset;
        /**
        * Daylight saving offset in seconds, if not taken into account by the
        * universal time offset parameter.
        */
        TTimeIntervalSeconds iDaylightSavingOffset;
        };

    /**
    * Buffer descriptor containing TWakeupAlarmTime object.
    */
    typedef TPckgBuf<TWakeupAlarmTime> TWakeupAlarmPckg;

    /**
    * Parameter definition for EExecuteReset.
    */
    enum TResetReason
        {
        /** A reset due to display language switch. */
        ELanguageSwitchReset = 100,
        /** A reset due to restoring factory settings. */
        ENormalRFSReset = 101,
        /** A reset due to restoring factory settings (deep). */
        EDeepRFSReset = 102,
        /** A reset due to firmware update. */
        EFirmwareUpdate = 104,
        /** A reset due to an error. */
        EUnknownReset = 105,
        /** A reset due to removing operator settings. */
        EOperatorSettingReset = 106,
        /** A reset due to network mode change. */
        ENetworkModeChangeReset = 107,
        /** A reset due removing/inserting SIM card. */
        ESIMStatusChangeReset = 108,
        EDRMReset = 109,
        /** A reset after restoring backed-up data. */
        EDataRestoreReset = 110,
        /** A reset required by Field Test software. */
        EFieldTestReset = 111
        };

    /**
    * Buffer descriptor containing TResetReason object.
    */
    typedef TPckgBuf<TResetReason> TResetReasonPckg;


    /**
    * Parameter definition for EExecuteDOSRfs.
    */
    enum TRFSReason
        {
        ENormalRFS = 100,
        EDeepRFS
        };

    /**
    * Buffer descriptor containing TRFSReason object.
    */
    typedef TPckgBuf<TRFSReason> TRFSReasonPckg;

    /*
    * ==========================================================================
    * Response parameter definitions
    * ==========================================================================
    */

    /**
    * Parameter definition for command responses that need only success or
    * failure information.
    */
    typedef TPckgBuf<TInt> TResponsePckg;


    /**
    * Parameter definition for ESecurityStateChange command response.
    */
    enum TSecurityStateInfo
        {
        EYes = 100,
        ENo,

        EPUK1Required,              // Only for SIMBlocked
        EUPUKRequired,              // Only for SIMBlocked

        EPIN1Required,              // Only for PINRequired
        EUPINRequired,              // Only for PINRequired

        ESecurityCodeRequired,      // Only for SECCodeRequired
        EMasterCodeRequired,        // Only for SECCodeRequired

        ESimLockRestricted,         // Only for SIMLock
        ESimLockRestrictionOn,      // Only for SIMLock
        ESimLockRestrictionPending, // Only for SIMLock
        ESimLockOk,                 // Only for SIMLock

        ENoValue                    // For SIMReadable, SIMLessOfflineSupported,
                                    // SECBlocked, AskPIN, AskPUK, AskSEC,
                                    // SIMCodesOK, SECCodeOK,
                                    // SecurityCheckOK, SecurityCheckFailed
        };

    /**
    * Parameter definition for ESecurityStateChange command response.
    */
    NONSHARABLE_STRUCT( TSecurityStateResponse )
        {
        /**
        * Indicates whether operation was successful or if not, the reason why.
        */
        TInt iErrorCode;
        /**
        * Response value. Valid only if iErrorCode is KErrNone.
        */
        TSecurityStateInfo iValue;
        };

    /**
    * Buffer descriptor containing TSecurityStateResponse object.
    */
    typedef TPckgBuf<TSecurityStateResponse> TSecurityStateResponsePckg;


    /**
    * Parameter definition for EGetGlobalStartupMode command response.
    */
    enum TGlobalStartupMode
        {
        ENormal = 100,
        EAlarm,
        ECharging,
        ETest
        };

    /**
    * Parameter definition for EGetGlobalStartupMode command response.
    */
    struct TGlobalStartupModeResponse
        {
        /**
        * Indicates whether operation was successful or if not, the reason why.
        */
        TInt iErrorCode;
        /**
        * Response value. Valid only if iErrorCode is KErrNone.
        */
        TGlobalStartupMode iValue;
        };

    /**
    * Buffer descriptor containing TGlobalStartupModeResponse object.
    */
    typedef TPckgBuf<TGlobalStartupModeResponse> TGlobalStartupModeResponsePckg;


    /**
    * Enumeration of preferred languages for EGetSIMLanguages command response.
    *
    * Implementation should map each entry in the physical list stored in
    * SIM card to a corresponding S60 preferred language code (one of the values
    * of TPreferredLanguage enumerations).
    * Implementation may skip the entries for which there is no corresponding
    * value in the TPreferredLanguage enumeration.
    */

    enum TPreferredLanguage
        {									// ISO 639 GSM 0338
        EPrefLangEnglish = 1,				// EN      1
        EPrefLangFrench = 2,				// FR      3
        EPrefLangGerman = 3,				// DE      0
        EPrefLangSpanish = 4,				// ES      4
        EPrefLangItalian = 5,				// IT      2
        EPrefLangSwedish = 6,				// SV      6
        EPrefLangDanish = 7,				// DA      7
        EPrefLangNorwegian = 8,				// NO      10
        EPrefLangFinnish = 9,				// FI      9
        EPrefLangAmerican = 10,				// EN
        EPrefLangPortuguese = 13,			// PT      8
        EPrefLangTurkish = 14,				// TR      12
        EPrefLangIcelandic = 15,			// IS      36
        EPrefLangRussian = 16,				// RU      35
        EPrefLangHungarian = 17,			// HU      13
        EPrefLangDutch = 18,				// NL      5
        EPrefLangCzech = 25,				// CS      32
        EPrefLangSlovak = 26,				// SK
        EPrefLangPolish = 27,				// PL      14
        EPrefLangSlovenian = 28,			// SL
        EPrefLangTaiwanChinese = 29,		// TC
        EPrefLangHongKongChinese = 30,		// HK
        EPrefLangPrcChinese = 31,			// ZH
        EPrefLangJapanese = 32,				// JA
        EPrefLangThai = 33,					// TH
        EPrefLangArabic = 37,				// AR      34
        EPrefLangTagalog = 39,				// TL
        EPrefLangBulgarian = 42,			// BG
        EPrefLangCatalan = 44,				// CA
        EPrefLangCroatian = 45,				// HR
        EPrefLangEstonian = 49,				// ET
        EPrefLangFarsi = 50,				// FA
        EPrefLangCanadianFrench = 51,		// CF
        EPrefLangGreek = 54,				// EL      11
        EPrefLangHebrew = 57,				// IW      33
        EPrefLangHindi = 58,				// HI
        EPrefLangIndonesian = 59,			// IN
        EPrefLangKorean = 65,				// KO
        EPrefLangLatvian = 67,				// LV
        EPrefLangLithuanian = 68,			// LT
        EPrefLangMalay = 70,				// MS
        EPrefLangMarathi = 72,				// MR
        EPrefLangBrazilianPortuguese = 76,	// BP
        EPrefLangRomanian = 78,				// RO
        EPrefLangSerbian = 79,				// SR
        EPrefLangLatinAmericanSpanish = 83,	// LS 
        EPrefLangUkrainian = 93,			// UK
        EPrefLangUrdu = 94,					// UR
        EPrefLangVietnamese = 96,			// VI
        EPrefLangBasque = 102,				// EU
        EPrefLangGalician = 103				// GL
        };


    /**
    * Array of preferred languages for EGetSIMLanguages command response.
    */
    typedef RArray<TPreferredLanguage> RLanguageList;


    /**
    * Parameter definition for EGetSIMLanguages command response.
    */
    NONSHARABLE_CLASS( RLanguageListResponse )
        {
    private:
        RLanguageListResponse( const RLanguageListResponse& );
        RLanguageListResponse& operator= ( const RLanguageListResponse& );

    public:

        /**
        * Constructor.
        */
        inline RLanguageListResponse();

        /**
        * Closes the language code array and frees all memory allocated to the
        * array.
        *
        * The function must be called before this object is destroyed.
        */
        inline void Close();

        /**
        * Use KErrNotFound to indicate that there is no SIM card present or the
        * SIM card does not contain the requested list.
        */
        TInt iErrorCode;

        /**
        * Array of zero or more languages codes.
        */
        RLanguageList iLanguages;
        };

    /**
    * Pointer descriptor representing an RLanguageListResponse object.
    */
    typedef TPckg<RLanguageListResponse> TLanguageListResponsePckg;


    /**
    * Parameter definition for command responses that need success or failure
    * information and a boolean return value.
    */
    struct TBooleanResponse
        {
        /**
        * Indicates whether operation was successful or if not, the reason why.
        */
        TInt iErrorCode;
        /**
        * Response value. Valid only if iErrorCode is KErrNone.
        */
        TBool iValue;
        };

    /**
    * Buffer descriptor containing TBooleanResponse object.
    */
    typedef TPckgBuf<TBooleanResponse> TBooleanResponsePckg;

    }

#include <ssm/startupadaptationcommands.inl>

#endif // __STARTUPADAPTATIONCOMMANDS_H__
