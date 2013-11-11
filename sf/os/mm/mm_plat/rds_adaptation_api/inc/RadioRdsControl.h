/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides an interface for accessing the Radio Data System (RDS)
*                for FM sound broadcasting. CRadioRdsUtility can also be used with
*                Radio Broadcast Data System (RDBS). Client must first instantiate
*                a tuner control implementing this interface before this class can be used.
*
*
*/



#ifndef M_RADIORDSCONTROL_H
#define M_RADIORDSCONTROL_H

#include <e32base.h>

/**
 *  RDS control error definitions.
 *  In addition to the standard system error code, the following error codes may
 *  be returned.
 */
typedef TInt TRdsControlError;
const TRdsControlError KRdsErrHardwareFaulty                = -13050;
const TRdsControlError KRdsErrNotPermittedInCurrentState    = -13051;
const TRdsControlError KRdsErrDuplicateRequest              = -13052;

/**
 * RDS Programme Type.
 *
 * The interpretation of values of this type depends on the origin of the RDS
 * broadcast: in North America, a slightly different standard, RBDS, is used.
 * These PTY codes are defined by static values KRbdsPtyXxxxx, for example
 * KRbdsPtySoftRock.
 *
 * Elsewhere, including Europe, the RDS standard is used. In these areas, the
 * PTY codes are defined by static values KRdsPtyXxxxx, for example
 * KRdsPtyChildrensProgrammes.
 *
 * In all other important aspects, the European RDS and North American RBDS
 * standards are identical.
 */
typedef TInt TRdsProgrammeType;

const TRdsProgrammeType KRdsPtyNone                = 0;
const TRdsProgrammeType KRdsPtyNews                = 1;
const TRdsProgrammeType KRdsPtyCurrentAffairs      = 2;
const TRdsProgrammeType KRdsPtyInformation         = 3;
const TRdsProgrammeType KRdsPtySport               = 4;
const TRdsProgrammeType KRdsPtyEducation           = 5;
const TRdsProgrammeType KRdsPtyDrama               = 6;
const TRdsProgrammeType KRdsPtyCulture             = 7;
const TRdsProgrammeType KRdsPtyScience             = 8;
const TRdsProgrammeType KRdsPtyVariedSpeech        = 9;
const TRdsProgrammeType KRdsPtyPopMusic            = 10;
const TRdsProgrammeType KRdsPtyRockMusic           = 11;
const TRdsProgrammeType KRdsPtyEasyListening       = 12;
const TRdsProgrammeType KRdsPtyLightClassical      = 13;
const TRdsProgrammeType KRdsPtySeriousClassical    = 14;
const TRdsProgrammeType KRdsPtyOtherMusic          = 15;
const TRdsProgrammeType KRdsPtyWeather             = 16;
const TRdsProgrammeType KRdsPtyFinance             = 17;
const TRdsProgrammeType KRdsPtyChildrensProgrammes = 18;
const TRdsProgrammeType KRdsPtySocialAffairs       = 19;
const TRdsProgrammeType KRdsPtyReligion            = 20;
const TRdsProgrammeType KRdsPtyPhoneIn             = 21;
const TRdsProgrammeType KRdsPtyTravel              = 22;
const TRdsProgrammeType KRdsPtyLeisure             = 23;
const TRdsProgrammeType KRdsPtyJazzMusic           = 24;
const TRdsProgrammeType KRdsPtyCountryMusic        = 25;
const TRdsProgrammeType KRdsPtyNationalMusic       = 26;
const TRdsProgrammeType KRdsPtyOldiesMusic         = 27;
const TRdsProgrammeType KRdsPtyFolkMusic           = 28;
const TRdsProgrammeType KRdsPtyDocumentary         = 29;
const TRdsProgrammeType KRdsPtyAlarmTest           = 30;
const TRdsProgrammeType KRdsPtyAlarm               = 31;

const TRdsProgrammeType KRbdsPtyNone               = 0;
const TRdsProgrammeType KRbdsPtyNews               = 1;
const TRdsProgrammeType KRbdsPtyInformation        = 2;
const TRdsProgrammeType KRbdsPtySports             = 3;
const TRdsProgrammeType KRbdsPtyTalk               = 4;
const TRdsProgrammeType KRbdsPtyRock               = 5;
const TRdsProgrammeType KRbdsPtyClassicRock        = 6;
const TRdsProgrammeType KRbdsPtyAdultHits          = 7;
const TRdsProgrammeType KRbdsPtySoftRock           = 8;
const TRdsProgrammeType KRbdsPtyTop40              = 9;
const TRdsProgrammeType KRbdsPtyCountry            = 10;
const TRdsProgrammeType KRbdsPtyOldies             = 11;
const TRdsProgrammeType KRbdsPtySoft               = 12;
const TRdsProgrammeType KRbdsPtyNostalgia          = 13;
const TRdsProgrammeType KRbdsPtyJazz               = 14;
const TRdsProgrammeType KRbdsPtyClassical          = 15;
const TRdsProgrammeType KRbdsPtyRhythmAndBlues     = 16;
const TRdsProgrammeType KRbdsPtySoftRhythmAndBlues = 17;
const TRdsProgrammeType KRbdsPtyLanguage           = 18;
const TRdsProgrammeType KRbdsPtyReligiousMusic     = 19;
const TRdsProgrammeType KRbdsPtyReligiousTalk      = 20;
const TRdsProgrammeType KRbdsPtyPersonality        = 21;
const TRdsProgrammeType KRbdsPtyPublic             = 22;
const TRdsProgrammeType KRbdsPtyCollege            = 23;
const TRdsProgrammeType KRbdsPtyUnassigned1        = 24;
const TRdsProgrammeType KRbdsPtyUnassigned2        = 25;
const TRdsProgrammeType KRbdsPtyUnassigned3        = 26;
const TRdsProgrammeType KRbdsPtyUnassigned4        = 27;
const TRdsProgrammeType KRbdsPtyUnassigned5        = 28;
const TRdsProgrammeType KRbdsPtyWeather            = 29;
const TRdsProgrammeType KRbdsPtyEmergencyTest      = 30;
const TRdsProgrammeType KRbdsPtyEmergency          = 31;


/**
 *  Table 1: Class codes for RTplus content types
 *  Defined in Radiotext plus (RTplus) Specification Version 2.1
 *  RDS Forum 2006-07-21 - R06/040_1
 */
enum TRdsRTplusClass
    {
    // Category: Item (0 - 10)
    ERTplusItemDummy = 0,
    ERTplusItemTitle,
    ERTplusItemAlbum,
    ERTplusItemTrackNumber,
    ERTplusItemArtist,
    ERTplusItemComposition,
    ERTplusItemMovement,
    ERTplusItemConductor,
    ERTplusItemComposer,
    ERTplusItemBand,
    ERTplusItemComment,
    ERTplusItemGenre,
    // Category: Info (12 - 30)
    ERTplusInfoNews,
    ERTplusInfoNewsLocal,
    ERTplusInfoStockMarket,
    ERTplusInfoSport,
    ERTplusInfoLottery,
    ERTplusInfoHoroscope,
    ERTplusInfoDailyDiversion,
    ERTplusInfoHealth,
    ERTplusInfoEvent,
    ERTplusInfoSzene,
    ERTplusInfoCinema,
    ERTplusInfoTV,
    ERTplusInfoDateTime,
    ERTplusInfoWeather,
    ERTplusInfoTraffic,
    ERTplusInfoAlarm,
    ERTplusInfoAdvisertisement,
    ERTplusInfoURL,
    ERTplusInfoOther,
    // Category: Programme (31 - 40)
    ERTplusStationNameShort,
    ERTplusStationNameLong,
    ERTplusProgramNow,
    ERTplusProgramNext,
    ERTplusProgramPart,
    ERTplusProgramHost,
    ERTplusProgramEditorialStaff,
    ERTplusStationFrequency,
    ERTplusProgramHomepage,
    ERTplusProgramSubChannel,
    // Category: Interactivity (41 - 53)
    ERTplusPhoneHotline,
    ERTplusPhoneStudio,
    ERTplusPhoneOther,
    ERTplusSmsStudio,
    ERTplusSmsOther,
    ERTplusEmailHotline,
    ERTplusEmailStudio,
    ERTplusEmailOther,
    ERTplusMmsOther,
    ERTplusChat,
    ERTplusChatCenter,
    ERTplusVoteQuestion,
    ERTplusVoteCenter,
    // Category: Reserved for future (54 - 55)
    ERTplusRfuClass54,
    ERTplusRfuClass55,
    // Category: Private classes (56 - 58)
    ERTplusPrivateClass56,
    ERTplusPrivateClass57,
    ERTplusPrivateClass58,
    // Category: Descriptor (59 - 63)
    ERTplusPlace,
    ERTplusAppointment,
    ERTplusIdentifier,
    ERTplusPurchase,
    ERTplusGetData
    };


/**
 *  RDS Function Bit Flags used by TRdsCapabilities and TRdsData.
 *
 *  @since S60 3.2
 */
enum TRdsFeatures
    {
    ERdsProgrammeIdentification  = 0x001,
    ERdsProgrammeType            = 0x002,
    ERdsProgrammeService         = 0x004,
    ERdsRadioText                = 0x008,
    ERdsRadioTextPlus            = 0x010,
    ERdsClockTime                = 0x020,
    ERdsTrafficProgramme         = 0x040,
    ERdsTrafficAnnouncement      = 0x080,
    ERdsAlternateFrequency       = 0x100
    };


/**
 *  The RDS Capabilities class defines the capabilities of the RDS control on the
 *  device, as retrieved using the function GetCapabilities.
 *
 *  @since S60 3.2
 */
class TRdsCapabilities
    {
public:
    // Bitfield as defined by TRdsFeatures with the bits of the supported functions set
    // Valid bitfieds are:
    //    ERdsProgrammeIdentification | ERdsProgrammeType | ERdsProgrammeService |
    //    ERdsRadioText | ERdsRadioTextPlus | ERdsClockTime | ERdsTrafficProgramme |
    //    ERdsTrafficAnnouncement | ERdsAlternateFrequency
    TUint32 iRdsFunctions;
    // Reserved for future
    TUint32 iAdditionalFunctions1;
    // Reserved for future
    TUint32 iAdditionalFunctions2;
	};


/**
 *  This class provides RDS features that can be enabled or disabled. Used in
 *  function NotifyRdsDataChange.
 *
 *  @since S60 3.2
 */
class TRdsData
    {
public:
    // A value indicating a set containing all RDS data encapsulated by class TRdsData.
    static const TUint32 KAllRdsData = ( ERdsProgrammeIdentification | ERdsProgrammeType |
          ERdsProgrammeService | ERdsRadioText | ERdsClockTime | ERdsTrafficAnnouncement );

public:
	// Bitfield as defined by TRdsDataFeatures with the bits of the supported functions set
    // Valid bitfieds are:
    //    ERdsProgrammeIdentification | ERdsProgrammeType | ERdsProgrammeService |
    //    ERdsRadioText | ERdsClockTime | ERdsTrafficAnnouncement
	TUint32 iRdsFunctions;
	// Reserved for future
	TUint32 iAdditionalFunctions1;
	// Reserved for future
	TUint32 iAdditionalFunctions2;
	};


/**
 *  This class provides RT+ tags.
 *
 *  @since S60 3.2
 */
class TRdsRTplusTag
    {
public:
	// Specifies the content type according to the class code
	TRdsRTplusClass iContentType;
	// Indicates the position of the first character within the Radio Text
	// (Start marker 0: means first character in RT)
	TUint32 iStart;
	// Length (number of characters) of the RTplus tag.
	TUint32 iLength;
	};


/** Programme Service name, 8 characters */
typedef TBuf8<8> TRdsPSName;
/** Radio Text, 64 characters */
typedef TBuf8<64> TRdsRadioText;


/**
 *  This class provides the interface class to be implemented by object receiving request
 *  complete notification from RDS control. These functions are called once the asynchronous
 *  request completes successfully or with an error.
 *
 *  In addition, this class provides the interface for receiving events from RDS control.
 *
 *  @since S60 3.2
 */
class MRdsControlObserver
	{
public:

    /**
     * Station seek by Programme Type(PTY) complete event. This event is asynchronous and
     * is received after a call to MRdsControl::StationSeekByPTY.
     *
     * @since S60 3.2
     * @param aError A standard system error code or RDS control error (TRdsControlError).
     * @param aFrequency The frequency(Hz) of the radio station that was found.
     */
    virtual void StationSeekByPTYComplete( TInt aError, TInt aFrequency ) = 0;

    /**
     * Station seek by Traffic Announcement(TA) complete event. This event is asynchronous
     * and is received after a call to MRdsControl::StationSeekByTA.
     *
     * @since S60 3.2
     * @param aError A standard system error code or RDS control error (TRdsControlError).
     * @param aFrequency The frequency(Hz) of the radio station that was found.
     */
    virtual void StationSeekByTAComplete( TInt aError, TInt aFrequency ) = 0;

    /**
     * Station seek by Traffic Programme(TP) complete event. This event is asynchronous and
     * is received after a call to MRdsControl::StationSeekByTP.
     *
     * @since S60 3.2
     * @param aError A standard system error code or RDS control error (TRdsControlError).
     * @param aFrequency The frequency(Hz) of the radio station that was found.
     */
    virtual void StationSeekByTPComplete( TInt aError, TInt aFrequency ) = 0;

    /**
     * Get frequencies by Programme Type(PTY) complete event. This event is asynchronous
     * and is received after a call to MRdsControl::GetFreqByPTY.
     *
     * Client takes ownership of the array and therefore is responsible for deallocating it.
     *
     * @since S60 3.2
     * @param aError A standard system error code or RDS control error (TRdsControlError).
     * @param aFreqList Array of frequencies (Hz), valid only if aError is KErrNone.
     */
    virtual void GetFreqByPTYComplete( TInt aError, RArray<TInt>& aFreqList ) = 0;

    /**
     * Get frequencies by Traffic Announcement(TA) complete event. This event is asynchronous
     * and is received after a call to MRdsControl::GetFreqByTA.
     *
     * Client takes ownership of the array and therefore is responsible for deallocating it.
     *
     * @since S60 3.2
     * @param aError A standard system error code or RDS control error (TRdsControlError).
     * @param aFreqList Array of frequencies (Hz), valid only if aError is KErrNone.
     */
    virtual void GetFreqByTAComplete( TInt aError, RArray<TInt>& aFreqList ) = 0;

    /**
     * Get Programme Service names (PS) by Programme Type(PTY) complete event. This event is
     * asynchronous and is received after a call to MRdsControl::GetPSByPTY.
     *
     * Client takes ownership of the array and therefore is responsible for deallocating it.
     *
     * @since S60 3.2
     * @param aError A standard system error code or RDS control error (TRdsControlError).
     * @param aPsList Array of programme service names, valid only if aError is KErrNone.
     */
    virtual void GetPSByPTYComplete( TInt aError, RArray<TRdsPSName>& aPsList ) = 0;

    /**
     * Get Programme Service names (PS) by Traffic Announcement(TA) complete event. This event is
     * asynchronous and is received after a call to MRdsControl::GetPSByTA.
     *
     * Client takes ownership of the array and therefore is responsible for deallocating it.
     *
     * @since S60 3.2
     * @param aError A standard system error code or RDS control error (TRdsControlError).
     * @param aPsList Array of programme service names, valid only if aError is KErrNone.
     */
    virtual void GetPSByTAComplete( TInt aError, RArray<TRdsPSName>& aPsList ) = 0;

    /**
     * New Programme Identification(PI) is available. This event is asynchronous and is
     * received after a call to MRdsControl::NotifyRdsDataChange. Client will continue to
     * receive this notification whenever new PI is available until clients unsubscribes.
     *
     * @since S60 3.2
     * @param aPi Programme identification
     */
    virtual void RdsDataPI( TInt aPi ) = 0;

    /**
     * New Programme Type (PTY) is available. This event is asynchronous and is received after
     * a call to MRdsControl::NotifyRdsDataChange. Client will continue to receive this
     * notification whenever new PTY is available until client cancels notification request.
     *
     * @since S60 3.2
     * @param aPty Programme type
     */
    virtual void RdsDataPTY( TRdsProgrammeType aPty ) = 0;

    /**
     * New Programme Service name (PS) is available. This event is asynchronous and is received
     * after a call to MRdsControl::NotifyRdsDataChange. Client will continue to receive this
     * notification whenever new PS is available until client cancels notification request.
     *
     * @since S60 3.2
     * @param aPs Programme service
     */
    virtual void RdsDataPS( TRdsPSName& aPs ) = 0;

    /**
     * New Radio Text (RT) is available. This event is asynchronous and is received after
     * a call to MRdsControl::NotifyRdsDataChange. Client will continue to receive this
     * notification whenever new RT is available until client cancels notification request.
     *
     * If RT+ is supported, RT+ tags information is sent along with the RT. Client takes
     * ownership of the array and therefore is responsible to deallocate it.
     *
     * @since S60 3.2
     * @param aRt Radio text
     * @param aRTplusTags RTplus tags
     */
    virtual void RdsDataRT( TRdsRadioText& aRt, RArray<TRdsRTplusTag>& aRTplusTags ) = 0;

    /**
     * Clock Time (CT) is available. This event is asynchronous and is received after a call
     * to MRdsControl::NotifyRdsDataChange. Client will continue to receive this notification
     * whenever new CT is available until client cancels notification request.
     *
     * @since S60 3.2
     * @param aCt Clock time
     */
    virtual void RdsDataCT( TDateTime& aCt ) = 0;

    /**
     * Called when Traffice Announcement (TA) status changes. This event is asynchronous
     * and is received after a call to MRdsControl::NotifyRdsDataChange. Client will continue
     * to receive this notification whenever TA status changes until client cancels notification request.
     *
     * @since S60 3.2
     * @param aTaOn ETrue indicates that Traffic Announcement is on.
     */
    virtual void RdsDataTA( TBool aTaOn ) = 0;

    /**
     * Called when tuner starts to search for an Alternate Frequency (AF) with stronger signal.
     * This event is asynchronous and is received after a call to MRdsControl::SetAutomaticSwitching.
     * Client will continue to receive this notification whenever new search is started
     * until clients turns automatic switching off.
     *
     * @since S60 3.2
     */
    virtual void RdsSearchBeginAF() = 0;

    /**
     * Called when search for an Alternate Frequency (AF) ends. This event is asynchronous and
     * is received after a call to MRdsControl::SetAutomaticSwitching. Client will continue
     * to receive this notification until clients turns automatic switching off.
     *
     * Most common error codes:
     * KErrNone - Successful, new frequency found
     * KErrNotFound - Ended, but frequency hasn't changed
     * KErrCancel - Cancelled
     *
     * @since S60 3.2
     * @param aError A standard system error code or RDS control error (TRdsControlError).
     * @param aFrequency The frequency (Hz) of the radio station that was found.
     */
    virtual void RdsSearchEndAF( TInt aError, TInt aFrequency ) = 0;

	/**
	 * Called when the tuner has re-tuned to another frequency that is broadcasting
	 * Traffic Announcement (TA). This notification sent only when the client has turned the
	 * automatic traffic announcement on.
	 *
	 * @since S60 3.2
     * @param aFrequency The frequency (Hz) of the radio station that was found.
	 */
	virtual void RdsStationChangeTA( TInt aFrequency ) = 0;

    /**
     * Called when RDS signal status change (i.e. signal is lost/restored).
     *
     * @since S60 3.2
     * @param aSignal ETrue indicates that RDS signal is available in the tuned frequency.
     */
    virtual void RdsEventSignalChange( TBool aSignal ) = 0;
    };


/**
*  This class provides the interface class to be implemented by object implementing RDS control.
*
*  @since S60 3.2
*/
class MRdsControl
    {
public:

    /**
     * Get the capabilities of the RDS control on the device.
     *
     * @since S60 3.2
     * @param aCaps The capabilities object to fill
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt GetCapabilities( TRdsCapabilities& aCaps ) const = 0;

    /**
     * Get the status of the RDS reception.
     *
     * @since S60 3.2
     * @param aRdsSignal On return, will be ETrue if RDS signal can be recepted, EFalse otherwise.
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt GetRdsSignalStatus( TBool& aRdsSignal ) const = 0;

    /**
     * Subscribe for notification for the specified RDS data. Client should first check
     * the capabilities to see if a feature is supported.
     * Request for notification for non-supported features will simply be ignored.
     *
     * For example, if the client wishes to be notified when new Programme Type and
     * Traffic Announcement is available:
     *      iRdsData = ERdsProgrammeType | ERdsTrafficAnnouncement;
     *      iRdsUtility->NotifyRdsDataChange(iRdsData);
     *
     * @since S60 3.2
     * @param aRdsData Bitfield indicating notification request.
     * @see MRdsControlObserver::RdsDataPI
     * @see MRdsControlObserver::RdsDataPTY
     * @see MRdsControlObserver::RdsDataPS
     * @see MRdsControlObserver::RdsDataRT
     * @see MRdsControlObserver::RdsDataCT
     * @see MRdsControlObserver::RdsDataTA
     */
    IMPORT_C virtual TInt NotifyRdsDataChange( TRdsData aRdsData ) = 0;

    /**
     * Cancel NotifyRdsDataChange request.
     *
     * @since S60 3.2
     */
    IMPORT_C virtual void CancelNotifyRdsDataChange() = 0;

    /**
     * Turn on/off automatic switching of frequency based on Alternate Frequency.
     * This will cause RDS device to search for alternate frequency when the signal strength
     * deteriorates. User should be ready to receive RdsSearchBeginAF and RdsSearchEndAF.
     * Automatic switching is off by default.
     *
     * Client should first check the capabilities to see if ERdsAlternateFrequency is
     * supported. Will return KErrNotSupported if this feature is not supported.
     *
     * @since S60 3.2
     * @param aAuto ETrue to turn automatic switching on, EFalse to turn it off.
     * @return A standard system error code or RDS control error (TRdsControlError).
     * @see MRdsControlObserver::RdsSearchBeginAF
     * @see MRdsControlObserver::RdsSearchEndAF
     */
    IMPORT_C virtual TInt SetAutomaticSwitching( TBool aAuto ) = 0;

    /**
     * Find out whether automatic switching is on or off.
     *
     * @since S60 3.2
     * @param aAuto ETrue indicates that automatic switching is enabled.
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt GetAutomaticSwitching( TBool& aAuto ) = 0;

    /**
     * Cancel ongoing search for an Alternate Frequency (AF) with stronger signal.
     *
     * Client can issue this request to interrupt the search indicated with
     * MRdsControlObserver::RdsSearchBeginAF.
     *
     * @since S60 3.2
     */
    IMPORT_C virtual void CancelAFSearch() = 0;

    /**
     * Turns on/off automatic switching of frequency based on Traffic Announcement.
     * This will cause RDS device to search for frequencies broadcasting traffic announcement.
     * This feature is based on TP/TA fields and/or EON. It is up to the client to return to
     * the previous frequency when the traffic announcement is finished.
     *
     * NOTE: This is only supported in dual tuner configuration since the secondary tuner
     * needs to perform continuous scanning for frequencies broadcasting traffic announcement,
     * while the primary tuner is used for normal tuner activities.
     * Client should first check the tuner capabilities. Will return KErrNotSupported if
     * this feature is not supported.
     *
     * @since S60 3.2
     * @param aAuto ETrue indicates that automatic switching is on.
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt SetAutomaticTrafficAnnouncement( TBool aAuto ) = 0;

    /**
     * Find out whether automatic traffic announcement is enabled.
     *
     * @since S60 3.2
     * @param aAuto ETrue indicates that automatic traffic announcement is on.
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt GetAutomaticTrafficAnnouncement( TBool& aAuto ) = 0;

    /**
     * Asynchronous request to find a radio station with the specified Programme Type (PTY),
     * starting from the currently tuned frequency and searching in the direction specified
     * (i.e. up or down). User must be ready to receive callback method StationSeekByPTYComplete
     * The station found is returned in the callback.
     *
     * @since S60 3.2
     * @param aPty The type of programme to search for.
     * @param aSeekUp The direction to search in. Searches upwards if set to ETrue.
     * @see MRdsControlObserver::StationSeekByPTYComplete
     */
    IMPORT_C virtual void StationSeekByPTY( TRdsProgrammeType aPty, TBool aSeekUp ) = 0;

    /**
     * Asynchronous request to find a radio station with Traffic Announcement (TA),
     * starting from the currently tuned frequency and searching in the direction specified
     * (i.e. up or down). User must be ready to receive callback method StationSeekByTAComplete
     * The station found is returned in the callback.
     *
     * @since S60 3.2
     * @param aSeekUp The direction to search in. Searches upwards if set to ETrue.
     * @see MRdsControlObserver::StationSeekByTAComplete
     */
    IMPORT_C virtual void StationSeekByTA( TBool aSeekUp ) = 0;

    /**
     * Asynchronous request to find a radio station with Traffic Programme (TP),
     * starting from the currently tuned frequency and searching in the direction specified
     * (i.e. up or down). User must be ready to receive callback method StationSeekByTPComplete
     * The station found is returned in the callback.
     *
     * @since S60 3.2
     * @param aSeekUp The direction to search in. Searches upwards if set to ETrue.
     * @see MRdsControlObserver::StationSeekByTPComplete
     */
    IMPORT_C virtual void StationSeekByTP( TBool aSeekUp ) = 0;

    /**
     * Cancels an ongoing retune operation, as initiated by a call to StationSeekByPTY,
     * StationSeekByTA, or StationSeekByTP.
     * The usual callback will not occur if this has been called.
     *
     * @since S60 3.2
     */
    IMPORT_C virtual void CancelRdsStationSeek() = 0;

    /**
     * Asynchronous request to find all frequencies sending the given Programme Type (PTY).
     * User must be ready to receive callback method GetFreqByPTYComplete.
     *
     * NOTE: This is only supported in dual tuner configuration since the secondary tuner
     * needs to perform continuous scanning for frequencies broadcasting given Programme Type
     * while the primary tuner is used for normal tuner activities.
     * Client should first check the tuner capabilities. Will return KErrNotSupported in
     * callback method if this feature is not supported.
     *
     * @since S60 3.2
     * @param aPty The type of programme to search for
     * @see MRdsControlObserver::GetFreqByPTYComplete
     */
    IMPORT_C virtual void GetFreqByPTY( TRdsProgrammeType aPty ) = 0;

    /**
     * Cancels an ongoing request to find all frequencies sending a given Programme Type (PTY).
     * The usual callback will not occur if this has been called.
     *
     * @since S60 3.2
     */
    IMPORT_C virtual void CancelGetFreqByPTY() = 0;

    /**
     * Asynchronous request to find all frequencies sending Traffic Announcement (TA). User must
     * be ready to receive callback method GetFreqByTAComplete.
     *
     * NOTE: This is only supported in dual tuner configuration since the secondary tuner
     * needs to perform continuous scanning for frequencies broadcasting given Traffic Announcement
     * while the primary tuner is used for normal tuner activities.
     * Client should first check the tuner capabilities. Will return KErrNotSupported in
     * callback method if this feature is not supported.
     *
     * @since S60 3.2
     * @see MRdsControlObserver::GetFreqByTAComplete
     */
    IMPORT_C virtual void GetFreqByTA() = 0;

    /**
     * Cancels an ongoing request to find all frequencies sending Traffic Announcement.
     * The usual callback will not occur if this has been called.
     *
     * @since S60 3.2
     */
    IMPORT_C virtual void CancelGetFreqByTA() = 0;

    /**
     * Asynchronous request to find all Programme Service names (PS) sending the given Programme
     * Type (PTY). User must be ready to receive callback method GetPSByPTYComplete.
     *
     * NOTE: This is only supported in dual tuner configuration since the secondary tuner
     * needs to perform continuous scanning for frequencies broadcasting given Programme Type
     * while the primary tuner is used for normal tuner activities.
     * Client should first check the tuner capabilities. Will return KErrNotSupported in
     * callback method if this feature is not supported.
     *
     * @since S60 3.2
     * @param aPty The type of programme to search for
     * @see MRdsControlObserver::GetPSByPTYComplete
     */
    IMPORT_C virtual void GetPSByPTY( TRdsProgrammeType aPty ) = 0;

    /**
     * Cancels an ongoing request to find all Programme Service names (PS) sending a given
     * Programme Type (PTY). The usual callback will not occur if this has been called.
     *
     * @since S60 3.2
     */
    IMPORT_C virtual void CancelGetPSByPTY() = 0;

    /**
     * Asynchronous request to find all Programme Service names (PS) sending Traffic Announcement (TA).
     * User must be ready to receive callback method GetPSByTAComplete.
     *
     * NOTE: This is only supported in dual tuner configuration since the secondary tuner
     * needs to perform continuous scanning for frequencies broadcasting given Traffic Announcement
     * while the primary tuner is used for normal tuner activities.
     * Client should first check the tuner capabilities. Will return KErrNotSupported in
     * callback method if this feature is not supported.
     *
     * @since S60 3.2
     * @see MRdsControlObserver::GetPSByTAComplete
     */
    IMPORT_C virtual void GetPSByTA() = 0;

    /**
     * Cancels an ongoing request to find all Programme Service names (PS) sending Traffic Announcement.
     * The usual callback will not occur if this has been called.
     *
     * @since S60 3.2
     */
    IMPORT_C virtual void CancelGetPSByTA() = 0;

    /**
     * Get the current Programme Identification code.
     * RDS data is received over the air and may not be available immediately following
     * tune operation. If a value is returned, this is the last known value, which may
     * not be up to date.
     * To be notified of the most recent value, client should use NotifyRdsDataChange.
     *
     * @since S60 3.2
     * @param aPi On return contains Programme Identification code
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt GetProgrammeIdentification( TInt& aPi ) = 0;

    /**
     * Get the current Programme Type.
     * RDS data is received over the air and may not be available immediately following
     * tune operation. If a value is returned, this is the last known value, which may
     * not be up to date.
     * To be notified of the most recent value, client should use NotifyRdsDataChange.
     *
     * @since S60 3.2
     * @param aPty On return contains Programme Type
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt GetProgrammeType( TRdsProgrammeType& aPty ) = 0;

    /**
     * Get the current Programme Service name.
     * RDS data is received over the air and may not be available immediately following
     * tune operation. If a value is returned, this is the last known value, which may
     * not be up to date.
     * To be notified of the most recent value, client should use NotifyRdsDataChange.
     *
     * The maximum possible length for Programme Service name is 8 characters.
     *
     * @since S60 3.2
     * @param aPs On return contains Programme Service name
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt GetProgrammeService( TRdsPSName& aPs ) = 0;

    /**
     * Get the current Radio Text.
     * RDS data is received over the air and may not be available immediately following
     * tune operation. If a value is returned, this is the last known value, which may
     * not be up to date.
     * To be notified of the most recent value, client should use NotifyRdsDataChange.
     *
     * The maximum possible length for radio text is 64 characters.
     *
     * If RT+ is supported, RT+ tags information is returned along with the RT. Array is
     * owned by the client.
     *
     * @since S60 3.2
     * @param aRt On return contains Radio Text
     * @param aRTplusTags RTplus tags
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt GetRadioText( TRdsRadioText& aRt, RArray<TRdsRTplusTag>& aRTplusTags ) = 0;

    /**
     * Get the current Clock Time and date.
     * RDS data is received over the air and may not be available immediately following
     * tune operation. If a value is returned, this is the last known value, which may
     * not be up to date.
     * To be notified of the most recent value, client should use NotifyRdsDataChange.
     *
     * The RDS standard states that this is broadcast within 0.1s of the start of a minute,
     * and is accurate to one minute. The latency of the notification reaching the application
     * cannot be guaranteed.
     *
     * @since S60 3.2
     * @param aCt On return contains current time and date
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt GetClockTime( TDateTime& aCt ) = 0;

    /**
     * Get Traffic Announcement status at the current station.
     *
     * @since S60 3.2
     * @param aTaStatus On return, will be ETrue if current station has ongoing traffic announcement
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt GetTrafficAnnouncementStatus( TBool& aTaStatus ) = 0;

    /**
     * Get Traffic Programme status at the current station.
     *
     * @since S60 3.2
     * @param aTpStatus On return, will be ETrue if current station supports traffic programme
     * @return A standard system error code or RDS control error (TRdsControlError).
     */
    IMPORT_C virtual TInt GetTrafficProgrammeStatus( TBool& aTpStatus ) = 0;
	};

#endif      // M_RADIORDSCONTROL_H

