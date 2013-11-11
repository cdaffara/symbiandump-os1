// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// ETel (U)SAT header file.
// Describes the (U)SAT API - classes, methods and types.
// 
//

/**
 @file
 @publishedPartner
 @released
*/


#ifndef __ETELSAT_H__
#define __ETELSAT_H__

#include <e32base.h>
#include <s32mem.h>
#include <etel.h>

const TInt KETelExtSatV1=5000;  //< The global identifier for the first version of the ETelSat API, for passing into RTelServer::IsSupportedByModule()
const TInt KETelExtSatV2=9000;  //< The global identifier for the second version of the ETelSat API, for passing into RTelServer::IsSupportedByModule()
const TInt KETelExtSatV3=11000; //< The global identifier for the third version of the ETelSat API, for passing into RTelServer::IsSupportedByModule()
const TInt KEtelExtSatV4=11100; //< The global identifier for the fourth version of the ETelSat API, for passing into RTelServer::IsSupportedByModule()
const TInt KEtelExtSatV5=11200;	//< The global identifier for the fifth version of the ETelSat API, for passing into RTelServer::IsSupportedByModule()
const TInt KEtelExtSatV6=11300;	//< The global identifier for the sixth version of the ETelSat API, for passing into RTelServer::IsSupportedByModule()
const TInt KEtelExtSatV7=11400;	//< The global identifier for the seventh version of the ETelSat API, for passing into RTelServer::IsSupportedByModule()

/**
(U)SAT allocated error space ranges from -5250 to -5299.
Do not return KErrSatBase as an error.
*/
const TInt KErrSatBase=-5250;
/** The (U)SIM Application Toolkit is busy. */
const TInt KErrSatBusy=KErrSatBase-1;
/**
The error code can be received following Call Control.  It is returned when the
(U)SIM (or MO SM Control by (U)SIM) either bars or modifies the original
action.
*/
const TInt KErrSatControl=KErrSatBase-2;
/**
The error code can be received following Multimedia Service Notification Download.
It is returned when the (U)SIM either bars the original action,The ME shall not retry the same command.
*/
const TInt KErrSatMmsFailure=KErrSatBase-3;

enum {KTpduMaxSize=248};
typedef TBuf8<KTpduMaxSize> TTpdu;

const TInt KPdpActivationParametersMaxLengh=512;  //< Global constant defines maximum length of the buffer that can hold PDP CONTEXT ACTIVATION REQUEST
typedef TBuf8<KPdpActivationParametersMaxLengh> TPdpParameters;

// Forward declaration
class CSatPtrHolder;
class RSat: public RTelSubSessionBase
/**
 * Provides an interface to Symbian's ETelSat API.  RSat inherits from
 * RTelSubSessionBase defined in ETEL.H.
 *
 * On GSM/WCDMA networks, this API offers access to the (U)SIM Application
 * Toolkit, or (U)SAT.  On CDMA networks, it offers access to Cdma Card Application
 * Toolkit, or CCAT. Both toolkits are very similar.
 *
 * Phones on both networks contain an electronic card that stores the information
 * such as subscriber's identification, the phone's number, an address book, etc.
 * The (U)SAT and CCAT toolkits allow the card to be more than just a storage
 * device; they define a protocols that allow the card to ask the phone to perform
 * tasks.  For instance, a card can ask the phone to display a message on the
 * screen, add new menu items to the phone's menus, ask the phone to dial a number,
 * or browse to a URL.
 *
 * (U)SAT and CCAT are based around 'Proactive Commands': each is a task for the
 * phone to perform.  For the four tasks mentioned in the previous paragraph, the
 * card sends the phone proactive commands called DISPLAY TEXT, SET UP MENU, SET
 * UP CALL, and LAUNCH BROWSER. Understanding the range of proactive commands and
 * their capabilities is the first step to understanding (U)SAT and CCAT.
 *
 * To use Symbian's ETelSat API you request notification when proactive commands
 * arrive at the phone.  For each proactive command (DISPLAY TEXT, SET UP
 * MENU etc.) there is an RSat member function.  This function tells Symbian OS
 * when you are interested in a proactive command.  It also gives
 * Symbian OS a TRequestStatus that is completed when the next proactive
 * command arrives.
 *
 * Some of the proactive commands are not accessible to Symbian's ETelSat API.
 * Your phone's TSY deals with them for you, hiding them from RSat.  The remaining
 * proactive commands are handled by RSat.  Request notification for each one
 * using the functions listed below.
 *
 * (U)SAT and CCAT are defined in 3GPP TS 31.111 and 3GPP2 C.S0035-0 respectively.
 * Both of these specifications are based on ETSI TS 102 223.  We suggest you read
 * ETSI TS 102 223 before reading anything else: it is much more informative and
 * easier to read than the other two specifications. We also suggest you have a
 * basic understanding of ETSI TS 102 223 before using Symbian's ETelSat API.
 *
 * Before starting a (U)SAT or CCAT session, start a session with a phone.  A phone
 * session is represented by either RPhone or RMobilePhone.  Now you can create an
 * RSat interface and start the RSat session with RSat::Open().  Pass this method
 * the RPhone or RMobilePhone instance.  This tells the (U)SAT or CCAT session
 * which phone to work with.
 *
 * This documentation uses the word "UICC" to mean  the card. Here are
 * the phrases used to describe the card on various networks:
 *
 * On GSM networks, the physical card is called the ICC and the software it
 * contains is called the SIM (Subscriber Identity Module).  SAT is an interface
 * to the software.
 *
 * On WCDMA networks, the physical card is called the UICC and the software it
 * contains is called the USIM (Universal Subscriber Identity Module).  USAT is an
 * interface to the software.
 *
 * On 3G CDMA networks, the card and the software are called a R-UIM.  CCAT is an
 * interface to the software.
 *
 * To use the ETelSat API, follow these steps.  Firstly, create an interface to the phone, if you haven't already done so:
 *
 * - Create an RPhone
 * - Call RPhone::ConstructL()
 * - Call RPhone::Open()
 *
 * Secondly, create and open an interface to the ETelSat client:
 *
 * - Create an RSat
 * - Call RSat::ConstructL()
 * - Call RSat::Open(), passing it the RPhone from the previous step.
 *
 * Now tell the UICC what SAT features the phone supports.  The UICC expects this
 * information in a "profile" object.  The structure of this object is described in
 * ETSI TS 102 223.
 *
 * - Firsly, call RSat::GetMeSideSatProfile().  The TSY returns a profile
 * describing the SAT features that are supported by the phone's hardware and the
 * TSY.  These will be features that are not under the ETelSat client's control; you
 * cannot use the RSat API to access them.
 *
 * - Now modify the profile to include the SAT features that you support.  This
 * will mainly involve settings flags in the profile object.
 *
 * - Finally, send the profile to the UICC by calling RSat::ClientSatProfileIndication()
 *
 * Here is a list of the proactive commands supported by RSat.  Once you have
 * completed all the steps above, queue some notification requests by
 * calling some RSat::Notify..... command listed below:
 *
 * - DISPLAY TEXT - displays text to the user - see RSat::NotifyDisplayTextPCmd()
 * - GET INKEY -
 *   instructs phone to display text and/or an icon and to expect the user to enter a single character -
 *   RSat::NotifyGetInkeyPCmd()
 * - GET INPUT -
 *   instructs phone to display text and/or an icon and to expect the user to enter a string -
 *   RSat::NotifyGetInputPCmd()
 * - PLAY TONE -
 *   instructs phone to play a tone -
 *   RSat::NotifyPlayTonePCmd()
 * - SET UP MENU -
 *   provides a set of menu items to be integrated with the phone's menu system -
 *   RSat::NotifySetUpMenuPCmd()
 * - SELECT ITEM -
 *   provides a set of menu items to be shown to the user. The user can choose one of these menu items -
 *   RSat::NotifySelectItemPCmd()
 * - SEND SHORT MESSAGE -
 *   asks the phone to send an SMS message -
 *   RSat::NotifySendSmPCmd()
 * - SEND SS -
 *   asks the phone to send a supplementary service request -
 *   RSat::NotifySendSsPCmd()
 * - SEND USSD -
 *   asks the phone to send a Unstructured Supplementary Service Data request to the network -
 *   RSat::NotifySendUssdPCmd()
 * - SET UP CALL -
 *   asks the phone to connect a call -
 *   RSat::NotifySetUpCallPCmd()
 * - REFERSH -
 *   indicates that changes have occurred to the UICC that may affect the phone -
 *   RSat::NotifyRefreshPCmd()
 * - SET UP EVENT LIST -
 *   gives the phone a list of events that may occur to the phone.
 *   The phone must inform the UICC when an event on the list occurs -
 *   RSat::NotifySetUpEventListPCmd()
 * - SET UP IDLE MODE TEXT -
 *   provides text (and optionally an icon) to be displayed when the phone is idle -
 *   RSat::NotifySetUpIdleModeTextPCmd()
 * - SEND DTMF -
 *   asks the phone to send a DTMF string -
 *   RSat::NotifySendDtmfPCmd()
 * - PERFORM CARD APDU -
 *   asks the phone to send an APDU command -
 *   RSat::NotifyPerformCardApduPCmd()
 * - POWER OFF CARD -
 *   closes the session with the card specified in the command -
 *   RSat::NotifyPowerOffCardPCmd()
 * - POWER ON CARD -
 *   starts a session with the card specified in the command -
 *   RSat::NotifyPowerOnCardPCmd()
 * - GET READER STATUS -
 *   requests the status of the card reader specified in the command -
 *   RSat::NotifyGetReaderStatusPCmd()
 * - RUN AT COMMAND -
 *   asks the phone to perform the AT command provided -
 *   RSat::NotifyRunAtCommandPCmd()
 * - LANGUAGE NOTIFICATION -
 *   tells the phone the language in which any text strings from the UICC are written -
 *   RSat::NotifyLanguageNotificationPCmd()
 * - LAUNCH BROWSER -
 *   asks the phone to browse to the supplied URL -
 *   RSat::NotifyLaunchBrowserPCmd()
 * - OPEN CHANNEL -
 *   asks the phone to open a channel -
 *   RSat::NotifyOpenChannelPCmd()
 * - CLOSE CHANNEL -
 *   asks the phone to close a channel previously opened with the OPEN CHANNEL proactive command -
 *   RSat::NotifyCloseChannelPCmd()
 * - RECEIVE DATA -
 *   asks the phone for data from  a channel opened with the OPEN CHANNEL proactive command -
 *   RSat::NotifyReceiveDataPCmd()
 * - SEND DATA -
 *   asks the phone to send data on a channel opened with the OPEN CHANNEL proactive command -
 *   RSat::NotifySendDataPCmd()
 * - GET CHANNEL STATUS -
 *   asks the phone to return the status of a channel opened with the OPEN CHANNEL proactive command -
 *   RSat::NotifyGetChannelStatusPCmd()
 * - SERVICE SEARCH -
 *   RSat::NotifyServiceSearchPCmd()
 * - GET SERVICE INFO -
 *   RSat::NotifyGetServiceInfoPCmd()
 * - DECLARE SERVICE -
 *   RSat::NotifyDeclareServicePCmd()
 * - TIMER MANAGEMENT -
 *   asks the phone to start, stop and read the current value of timers -
 *   RSat::NotifyTimerMgmtPCmd()
 * - PROVIDE LOCAL INFORMATION -
 *   instructs the phone to send local information to the UICC -
 *   RSat::NotifyLocalInfoPCmd()
 * - POLLING INTERVAL -
 *   tells the phone how often the UICC would like to be polled by the phone -
 *   RSat::NotifyPollingIntervalPCmd()
 * - POLLING OFF -
 *   tells the phone not to poll the UICC for proactive commands any more -
 *   RSat::NotifyPollingOffPCmd()
 * - RETRIEVE MULTIMEDIA MESSAGE
 *   asks the phone to store the Multimedia Message received from the Network on UICC-
 *   RSat::NotifyRetrieveMultimediaMsgPCmd()
 * - SUBMIT MULTIMEDIA MESSAGE
 *   asks the phone to submit the Multimedia Message stored in the UICC to the Network-
 *   RSat::NotifySubmitMultimediaMsgPCmd()
 * - DISPLAY MULTIMEDIA MESSAGE
 *   asks the phone to display the Multimedia message to the user-
 *   RSat::NotifyDisplayMultimediaMsgPCmd() 
 * - SET FRAMES
 *   This command instructs the terminal to divide the terminal screen into multiple, scrollable rectangular regions called frames inorder to present multiple information at once-
 *   RSat::NotifySetFramesPCmd()
 * - GET FRAMES STATUS
 *   This command requests the terminal to return a Frames parameters data object-
 *   RSat::NotifyGetFramesStatusPCmd()
 *
 * Note: the phone continuously polls the card, waiting for proactive commands.
 * However, the polling process is hidden from the ETelSat API - simply request
 * notification and wait for the commands.
 */
    {
public:
    IMPORT_C RSat();
    IMPORT_C TInt Open(RPhone& aPhone);
    IMPORT_C void Close();
    IMPORT_C void NotifyProactiveSimSessionEnd(TRequestStatus& aReqStatus) const;
    IMPORT_C void NotifyProactiveSimSessionStart(TRequestStatus& aReqStatus) const;

    class TSatBase
    /**
    Base class for all the (U)SAT ETel API parameters.

    There may be several versions of a parameter.  Parameter classes are named
    'name version number', such as RSat::TGetInkeyV1 and RSat::TGetInkeyV2.
    Both of these classes are used with the "GET INKEY" proactive command. They
    hold similar information but are designed for use with different versions
    of the (U)SAT ETel API. Version 2 contains the version 1 class plus extra
    information.  A third version would contain version 2 plus extra
    information, etc.

    In classes derived from RSat::TSatBase, the base class holds the class's
    version number.  The version number is a value from RSat::TSatVersion.
    Each value in RSat::TSatVersion describes the versions of Symbian OS with
    which they are used.

    You never need to set the version number; the class's constructor does
    this. Generally, RSat clients do not need to read the version number.

    The TSY can use RSat::TSatBase::ExtensionId() to return the version number.
    A typical situation is this:  the TSY is passed a (U)SAT ETel API parameter
    in a descriptor.  The TSY does not know which version it has been given so
    it should guess. For example, it could unpack the descriptor into the
    latest version of the parameter then call RSat::TSatBase::ExtensionId() to
    retrieve the actual version number.  If the TSY chose the incorrect version
    number then the descriptor can be unpacked into the correct parameter class.
    */
        {
    public:
        IMPORT_C TInt ExtensionId() const;
    protected:
        TSatBase();
    protected:
        /** The extension identifier or version number. */
        TInt iExtensionId;
        };


    class TSatPCmdBase : public TSatBase
    /**
    Base class for all the (U)SAT ETel API proactive command classes.

    TSatPCmdBase inherits from TSatBase and is the base class for all the (U)SAT
    ETel API proactive command classes, for which it holds the associated
    proactive command number.
    */
        {
    public:
        IMPORT_C TUint8 PCmdNumber() const;
        IMPORT_C TInt SetPCmdNumber(TUint8 aNumber);
    protected:
        TSatPCmdBase();
    protected:
        /** Proactive command number from 3GPP 31.111 and ETSI TS 102 223.
        Valid range is 0x01 to 0xFE. */
        TUint8 iPCmdNumber;
        };

    enum TSatVersion
    /**
    The version number of a (U)SAT ETel API parameter,
    returned by RSat::TSatBase::ExtensionId().
    @see RSat::TSatBase
    */
        {
        KSatVersionNotSet=-1,   //< (U)SAT version has not been set yet
        KSatV1=1,               //< API version 1, (U)SAT API delivered by v6.1 and v7.0.
        KSatV2=2,               //< API version 2, (U)SAT API delivered by 7.0s
        KSatV3=3,               //< API version 3, (U)SAT API delivered by v8.0a, v8.0b, v8.1a, v8.1b.
   		KSatV4=4,				//< API version 4, (U)SAT API delivered by v9.1
        KSatV5=5,				//< API version 5, (U)SAT API delivered by v9.2
        KSatV6=6,				//< API version 6, (U)SAT API delivered by v9.4
        KSatV7=7				//< API version 7, (U)SAT API delivered by v9.5
        };

    enum THelp
    /**
    The UICC may offer help to accompany a GET INKEY, GET INPUT, SET UP MENU or
    SELECT ITEM proactive command. This help is normally text for the phone's
    user.

    Below are links to four member functions.  They request notification for
    the GET INKEY, GET INPUT, SET UP MENU or SELECT ITEM proactive commands.
    When they complete, an object is returned containing the proactive
    command's details.  This includes an RSat::THelp that states whether help
    is available for the command.

    It is up to you whether you tell the user that help is available, and
    whether to display the help should the user ask for it. The proactive
    command does not contain the help; it only indicates whether help is
    available with RSat::THelp.  The process of getting help depends upon the
    proactive command; see the links to the member functions below for details.

    @see RSat::NotifyGetInkeyPCmd()
    @see RSat::NotifyGetInputPCmd()
    @see RSat::NotifySetUpMenuPCmd()
    @see RSat::NotifySelectItemPCmd()
    */
        {
        EHelpNotSet,        //< Initialization value used within constructor(s).
        EHelpAvailable,    //< Help information available to the user.
        ENoHelpAvailable    //< No help information is available to the user.
        };

    enum TTimeUnit
    /**
    Describes the units in RSat::TDuration.
    @see RSat::TDuration
    */
        {
        ETimeUnitNotSet,    //< Initialization value used within constructor(s).
        ENoDurationAvailable,     //< No duration has been provided. The ME should default to general beep.
        EMinutes,    //< The duration is expressed in minutes.
        ESeconds,     //< The duration is expressed in seconds.
        ETenthsOfSeconds    //< The duration is expressed in tenths of seconds.
        };

    struct TDuration
    /** Class used in RSat whenever a measure of duration is required. */
        {
        TTimeUnit iTimeUnit;    //< Unit used to encode the duration in iNumOfUnits
        TUint8 iNumOfUnits;     //< Valid range is 1 to 255.
        };

    enum TDeviceId
    /** Some proactive commands are aimed at particular devices.  The device will be on of those defined here. */
        {
        KDeviceIdNotSet         =-1,      //< Initialisation value
        KKeypad                 =0x01,    //< The destination device for the proactive command is the keypad.
        KDisplay                =0x02,    //< The destination device for the proactive command is the display.
        KEarPiece               =0x03,    //< The destination device for the proactive command is the earpiece.
        KAdditionalCardReader0  =0x10,    //< The destination device for the proactive command is the additional card reader 0.
        KAdditionalCardReader1  =0x11,    //< The destination device for the proactive command is the additional card reader 1.
        KAdditionalCardReader2  =0x12,    //< The destination device for the proactive command is the additional card reader 2.
        KAdditionalCardReader3  =0x13,    //< The destination device for the proactive command is the additional card reader 3.
        KAdditionalCardReader4  =0x14,    //< The destination device for the proactive command is the additional card reader 4.
        KAdditionalCardReader5  =0x15,    //< The destination device for the proactive command is the additional card reader 5.
        KAdditionalCardReader6  =0x16,    //< The destination device for the proactive command is the additional card reader 6.
        KAdditionalCardReader7  =0x17,    //< The destination device for the proactive command is the additional card reader 7.
        KChannel1               =0x21,    //< The destination device for the proactive command is the channel 1.
        KChannel2               =0x22,    //< The destination device for the proactive command is the channel 2.
        KChannel3               =0x23,    //< The destination device for the proactive command is the channel 3.
        KChannel4               =0x24,    //< The destination device for the proactive command is the channel 4.
        KChannel5               =0x25,    //< The destination device for the proactive command is the channel 5.
        KChannel6               =0x26,    //< The destination device for the proactive command is the channel 6.
        KChannel7               =0x27,    //< The destination device for the proactive command is the channel 7.
        KUICC                   =0x81,    //< The destination device for the proactive command is the UICC.
        KME                     =0x82,    //< The destination device for the proactive command is the Mobile Equipment, typically a phone.
        KNetwork                =0x83     //< The destination device for the proactive command is the network.
        };

	/**
	Specifies the text coding scheme for RSat buffers. It may be used in
	conjunction with any RSat buffer to indicate the coding scheme used.
	(See ETSI TS 102 223 V5.9.0 section 8.15.)
	*/
	enum TSatTextCoding
		{
		/** 7-bit packed coding */
		E7bitPacked,
		/** 7-bit unpacked (i.e. 8-bit) coding */
		E7bitUnpacked,
		/** 16-bit Unicode (UCS-2) coding */
		E16bitUCS2
		};

	/**
	@see RSat::TTextString
	*/
	enum {KTextStringMaxSize=250};

    /** Generic Text String

    This text string definition is to be used through the API where a text
    string is required. Text string should be presented to the client in
    unicode format. This type can be used in envelope commands, proactive
    commands and terminal response.
    @see RSat::KTextStringMaxSize
    */
    typedef TBuf<KTextStringMaxSize> TTextString;


    // Alpha Identifier

    /**
    @see RSat::TAlphaIdBuf
    @see RSat::TAlphaId

	@see RMobileCall::KAlphaIdMaxSize
	*/
    enum {KAlphaIdMaxSize=254};

    /**
    Buffer containing the alpha identifier in the proactive command sent from the ICC.

	@see RSat::TAlphaId

	@see RMobileCall::TAlphaIdBuf
    */
    typedef TBuf<KAlphaIdMaxSize> TAlphaIdBuf;

    /**
    Indicates the status of the alpha ID to the client.  It can be present or
    not present.  If it is present, then it can contain data (typically text)
    or it can be a null object.
    @see RSat::TAlphaId
    */
    enum TAlphaIdStatus
        {
        EAlphaIdNotSet,     //< Value used at initialisation
        EAlphaIdProvided,   //< The alpha ID was provided by the ICC and contains data, typically text.
        EAlphaIdNull,       //< The alpha ID was provided by the ICC but was a null object.
        EAlphaIdNotPresent  //< No alpha ID provided by the ICC.
        };

    /**
    Proactive commands can include some text called an 'alpha identifier'.
    The meaning of the text depends upon the proactive command; see each
    command's description for more details.

    In Symbian OS, details of each proactive command are encapsulated in a
    class.  Each class includes an RSat::TAlphaId.  Inside this, iStatus
    contains the status of the command's alpha identifier:

    If iStatus is RSat::EAlphaIdProvided then RSat::TAlphaId::iAlphaId contains
    the proactive command's alpha identifier text.  The description of each
    proactive command will tell you what to do with the text.

    If iStatus is RSat::EAlphaIdNull then the proactive command's alpha
    identifier is an empty string.  RSat::TAlphaId::iAlphaId contains nothing.
    This commonly indicates that nothing should be display to the user while
    the proactive command is executed; see the proactive command's description.

    If iStatus is RSat::EAlphaIdNotPresent then the proactive command did not
    contain an alpha identifier.  This is commonly indicates that you can
    display whatever you like to the user while the proactive command is
    executed; see the proactive command's description.

    It is intended by ETSI / 3GPP to be a short identifier of one or two words.
    However the maximum buffer size is 254 in this API.
    */
    struct TAlphaId
        {
        TAlphaIdStatus iStatus; //< The status of the alpha ID.
        TAlphaIdBuf iAlphaId; 	//< The alpha ID's text.  Only valid if iStatus is RSat::EAlphaIdProvided.
        };

	/** 
	Size in bytes of a text attribute as defined in spec. ETSI TS 102 223 
	@see RSat::TTextAttributeBuf
	*/
	enum {KTextAttributeBufSize=4};

	/** Contains text attribute data. A text attribute contains
	formatting information such as whether the text it is applied to
	is bold, underlined, aligned left, etc. The text to which the
	text attribute applies to depends on the specific proactive
	UICC command and is specified in ETSI TS 102 223.
	@see RSat::TTextAttribute
	*/
	typedef TBuf8<KTextAttributeBufSize> TTextAttributeBuf;

	enum TTextAttributeStatus
	/** Indicates the status of the text attribute to the client.
	It can be present or not present.  If it is present, then it will
	provide text attribute data.	This data is valid only if the
	text to which the text attribute data applies to is also valid.
	The text to which the text attribute applies to depends on the
	specific proactive UICC command and is specified in ETSI TS 102 223.
	@see RSat::TTextAttribute
	*/
		{
		ETextAttributeNotSet,		 	//< Value used at initialisation
		ETextAttributeProvided,			//< The text attribute was provided by the ICC and contains data.
		ETextAttributeNotPresent		//< No text attribute provided by the ICC.
		};

	/**
	Proactive commands which contain text may also contain formatting
	information, called text attributes, which apply to the text
	when it is displayed by the ME. If a Terminal receives a text
	attribute that it does not support then it shall use the default
	text attribute it supports.

	For some proactive commands, the text attribute information
	is a conditional parameter. This means it is only present in the
	command if the text to which it applies is present as the text
	itself is an optional part of the command.
	For other proactive commands that always contain some text such
	as an alpha identifier, the text attribute information is optional,
	and can be present or not present.

	The actual text to which the text attribute applies to depends on
	the specific proactive UICC command and is specified in
	ETSI TS 102 223.

	The TTextAttribute structure is present in RSat classes which
	represent proactive commands that support text attributes. The
	structure contains iStatus which represents the status of
	iTextAttribute.

	If the iStatus is RSat::ETextAttributeNotPresent then no text
	attribute data was supplied in the command. This commonly
	indicates that the default formatting should be applied to
	the text when it is displayed by the ME.

	If the iStatis is RSat::ETextAttributeProvided then text
	attribute data was supplied in the command.	This means that
	the supplied formatting should be applied when the text to which
	it applies is displayed on the ME.	If the supplied formatting
	is not supported by the terminal, then default formatting should
	be applied.
	*/
	struct TTextAttribute
		{
		/**
		The status of the text attribute.  
		@see RSat::TTextAttributeStatus
		*/
		TTextAttributeStatus iStatus;	  		
		/**
		The text attribute data. Only valid if the iStatus is 
		RSat::ETextAttributeProvided and the text to which it applies is valid.  
		@see RSat::TTextAttributeBuf
		*/
		TTextAttributeBuf iTextAttributeData;
		};

	/**
	@see RSat::TCcp
	
	@see RMobileCall::KCcpMaxSize
	*/
    enum {KCcpMaxSize=14}; 

    /**
    TCcp ( Capability configuration parameters ) is a 8-bit data buffer that
    contains the bearer capabilities as coded in ETSI 11.11 and ETSI 04.08. It
    is 14 bytes as it does not include the IEI (Information Element Identity).

    It is used by the SET UP CALL proactive command.  It is also used in call control.

    @see RSat::NotifySetUpCallPCmd()
    @see RSat::NotifyCallControlRequest()
    
    @see RMobileCall::TCcp
    */
    typedef TBuf8<KCcpMaxSize> TCcp;

	/**
	@see RSat::TSubAddress
	
	@see RMobileCall::KSubAddressMaxSize
	*/
    enum {KSubAddressMaxSize=21};

    /**
    Sub-address of a telephone number. Maximum size is 21 digits.

    It is used by the SET UP CALL and OPEN CHANNEL proactive commands. It is
    also used in call control.

    @see RSat::NotifySetUpCallPCmd()
    @see RSat::NotifyCallControlRequest()
    @see RSat::NotifyOpenChannelPCmd()
    
    @see RMobileCall::TSubAddress
    */
    typedef TBuf<KSubAddressMaxSize> TSubAddress;

    /**
    Type of number, part of a complete telephone number address along with the
    numbering plan and the number's digits.

    @see RSat::TAddress
    @see RSat::TSsString
    */
    enum  TTypeOfNumber
        {
        ETypeOfNumberNotSet,    //< Initialization value used within constructor(s).
        EUnknownNumber,         //< The user or the network has no knowledge of the type of network.
        EInternationalNumber,   //< International number.
        ENationalNumber,        //< National number.

        /** Administration/service number specific to the serving network, e.g.
        used to access an operator.*/
        ENetworkSpecificNumber,

        /** Called dedicated access / short code in GSM/WCDMA. Corresponds to
        TMobileTON::ESubscriberNumber in ETel MM API. */
        EDedicatedNumber,

        EAlphanumericNumber,    //< Alphanumeric number.
        EAbbreviatedNumber      //< Abbreviated number.
        };

    /**
    Numbering plan, part of a complete telephone number
    address along with the type of number and the number's
    digits.

    @see RSat::TAddress
    @see RSat::TSsString
    */
    enum TNumberingPlan
        {
        ENumberingPlanNotSet,          //< Initialization value used within constructor(s).
        EUnknownNumberingPlan,         //< The user or the network has no knowledge of the numbering plan.
        EIsdnNumberPlan,               //< ISDN / telephony numbering plan.
        EDataNumberPlan,               //< Data numbering plan.
        ETelexNumberPlan,              //< Telex numbering plan.

        /** The service centre specific plan is used to indicate a numbering
        plan specific to External Short Message Entities attached to the SMS
        centre.*/
        EServiceCentreSpecificPlan1,

        /** The service centre specific plan is used to indicate a numbering
        plan specific to External Short Message Entities attached to the SMS
        centre. */
        EServiceCentreSpecificPlan2,

        ENationalNumberPlan,           //< National numbering plan.
        EPrivateNumberPlan,            //< Private numbering plan.
        EERMESNumberPlan               //< ERMES numbering plan.
        };

	/**
	@see RSat::TSatTelNumber
	*/
    enum {KMaxMobileTelNumberSize=251};

    /**
    The phone number’s digits and other valid dial-string characters, coded as
    Unicode characters. RSat::TSatTelNumber is part of a complete telephone
    number address, along with the type of number and the numbering plan.

    Maximum size is 251 digits.
    @see RSat::TAddress
    */
    typedef TBuf<KMaxMobileTelNumberSize> TSatTelNumber;

    /**
    A complete telephone number.
    */
    struct TAddress
        {
        TTypeOfNumber iTypeOfNumber;    //< Type of number
        TNumberingPlan iNumberPlan;     //< Numbering plan
        TSatTelNumber iTelNumber;       //< Telephone number
        };

    /** 
     The originator of a call.
     
     This list of enumerations matches RMobileCall::TCallParamOrigin in Etelmm.
     
     @see RMobileCall::TCallParamOrigin
    */
    enum TCallParamOrigin
        {
        EOriginatorUnknown,       //< The call originator is unknown.
        EOriginatorSIM,           //< The call originated/originates from the SIM.
        EOriginatorEtelClient,    //< The call originated/originates from an Etel Client.
        EOriginatorExternal       //< The call originated/originates from an external device connected to the modem.
        }; 
        
        
    /** Maximum size of a SS or a USSD string
    @see RSat::TSsString
    @see RSat::TUssdString */
    enum {KStringMaxSize=242};

    struct TSsString
    /** A Supplementary Service string. */
        {
        TTypeOfNumber iTypeOfNumber;    //< Type of number
        TNumberingPlan iNumberPlan;     //< Numbering plan
        TBuf8<KStringMaxSize> iSsString;//< SS string. Max. length is 242 characters.
        };

    struct TUssdString
    /** An Unstructured Supplementary Services Data string. */
        {
        TUint8 iDcs;                        //< Data Coding Scheme
        TBuf<KStringMaxSize> iUssdString;   //< USSD string. Max. length is 242 characters.
        };

    enum TIconQualifier
    /** Included in each type of proactive command that can support icons.  It
    describes the icon, if there is one.
    
    @see RSat::TIconId
    @see RMobileCall::TIconQualifier
    */
        {
        EIconQualifierNotSet,   //< Icon qualifier is not set
        ENoIconId,              //< Icon not present in this proactive command.
        ESelfExplanatory,       //< Icon is included in the proactive command.  It is self explanatory. Display instead of the alpha ID or text string.
        ENotSelfExplanatory     //< Icon is included in the proactive command.  It is not self explanatory. Display both the icon and the alpha ID or text string.
        };

    struct TIconId
    /**
    Some types of proactive command can include an icon identifier. Commands
    that can include an icon will have an RSat::TIconId field. Icons are
    intended to provide graphical information to the user, although the display
    of icons is optional.

    If RSat::TIconId::iQualifier is RSat::ENoIconId then the proactive command
    does not provide an icon.

    Otherwise, an icon is provided.  The alpha identifier or text string in the
    proactive command will be present and will not be a null string.

    - If RSat::TIconId.iQualifier is RSat::ESelfExplanatory then the icon should
    be displayed instead of the alpha identifier or text string.

    - If RSat::TIconId.iQualifier is RSat::ENotSelfExplanatory then both icon
    and the alpha identifier or text string should be displayed.    If you are
    not able to display both (such as when the screen is too small) then
    display just the alpha identifier or text string.

    RSat::TIconId does not contain the icon itself.  Instead it contains an
    icon identifier, RSat::TIconId::iIdentifier.  Use RSat::GetIcon() followed
    by RSat::GetImageInstance() to get the icon from the UICC.

    There are two error cases related to icons:

    - If the UICC provides an icon identifier with a proactive command but
    you cannot display the icon for any reason, then inform the UICC by sending
    the general result "Command performed successfully, but requested icon
    could not be displayed"; see RSat::KSuccessRequestedIconNotDisplayed.

    - If you receive an icon, and either an empty or no alpha identifier/text
    string is given by the UICC, than reject the command with general result
    "Command data not understood by terminal"; see RSat::KCmdDataNotUnderstood.

    @see RSat::GetIcon()
    @see RSat::GetImageInstance()
    
    @see RMobileCall::TIconId
    */
        {
        /** Describes the state of the icon. */
        TIconQualifier iQualifier;
        /** Icon identifier, addresses a record in EF IMG. */
        TUint8 iIdentifier;
        };

    //
    // Read Icon
    //
    enum {KImageInstanceDescriptorsMaxSize=255}; //< @see RSat::TIcon

    /** RSat::GetIcon()  stores the an icon's EF record in this buffer. The
    record's structure is defined in ETSI TS 131 102; search for EFimg or
    "Contents of files at the DFGRAPHICS level"
    @see RSat::GetIcon()
    @see RSat::TIconId
    */
    typedef TBuf8<KImageInstanceDescriptorsMaxSize> TIcon;

    IMPORT_C void GetIcon(TRequestStatus& aReqStatus, TUint8 aRecordNumber, TIcon& aIconEf) const;
    IMPORT_C void GetImageInstance(TRequestStatus& aReqStatus, const TDesC8& aInstanceInfo, TDes8& aInstance) const;

    class TInstanceInfoV2 : public TSatPCmdBase
    /**
    Describes the exact position of a record within a EF IMG hold instance descriptor.
    RSat::TInstanceInfoV2 is packaged into a descriptor, RSat::TInstanceInfoV2Pckg.
    @see RSat::GetImageInstance()
    */
        {
    public:
        IMPORT_C TInstanceInfoV2();
    public:
        TUint iInstanceNumber;  //< The requested image instance EF.
        TUint16 iOffset;        //< The offset of the requested image instance EF.
        TUint16 iLength;        //< The length of the requested image instance EF in bytes.
        };

    /** A packaged RSat::TInstanceInfoV2. */
    typedef TPckg<TInstanceInfoV2>  TInstanceInfoV2Pckg;

    enum TImageCoding
    /** Coding used for icons.
    @see RSat::TInstanceInfoV3. */
    {
    KBasic             =0x11,   //< Basic Image Coding
    KColour            =0x21    //< Colour Image Coding
    };

    class TInstanceInfoV3 : public TInstanceInfoV2
    /**
    Describes the exact position, coding, size and File ID of a record within a
    EF IMG hold instance descriptor.

    The EF record is described in ETSI TS 131 102; search for EFimg or "Contents
    of files at the DFGRAPHICS level"

    To retrieve an image's data from the UICC, create an instance of this class
    and fill in fields with data retrieved with RSat::GetIcon().  Then pass the
    packaged version of this class (RSat::TInstanceInfoV3Pckg) to
    RSat::GetImageInstance().

    @see RSat::GetImageInstance()
    */
        {
    public:
        IMPORT_C TInstanceInfoV3();
    public:
        /**Image Coding Scheme*/
        TImageCoding iCoding;
        /**Image Width*/
        TUint8  iImgWidth;
        /**Image Height*/
        TUint8  iImgHeight;
        /**Image Instance File Identifier*/
        TUint16 iFileId;
        };

    /** A packaged RSat::TInstanceInfoV3. */
    typedef TPckg<TInstanceInfoV3>  TInstanceInfoV3Pckg;

    // API/TSY internal type
    struct TInstanceNumberAndOffset
        {
        TUint iInstanceNumber;  //< Image instance number
        TUint iOffset;          //< Image instance offset within the image record
        };

    IMPORT_C void GetClut(TRequestStatus& aReqStatus, TUint aInstanceNumber, TUint aOffset, TDes8& aClut) const;

    //
    // Terminal Response - Answer from the client to a proactive command
    //

    /** General Result of a proactive command.

    All proactive commands require a response from you to the UICC via the
    RSat::TerminalRsp() method.  When you send a response, include a value from
    this enumeration to indicate the level of success or failure. Some of these
    values require additional information to be returned to the UICC, too.  For
    instance, you cannot return just RSat::KMeUnableToProcessCmd, you must
    return a value from RSat::TMeProblem, too.  The description of each
    proactive command in 3GPP 31.111 and ETSI TS 102 223 will tell you what you
    need to return.

    The values KSuccess to KUssdTransactionTerminatedByUser indicate that the
    proactive command was successful.  It can be either completely or partially
    successful.

    The values from KMeUnableToProcessCmd to KLaunchBrowserError indicate
    temporary errors.  The UICC may make another attempt by resending the
    proactive command.

    The values from KCmdBeyondMeCapabilities onwards are permanent errors.  Use
    them to tell the UICC that the command cannot be executed at all.

    TPCmdResult enum values are dictated by 3GPP 31.111 and ETSI TS 102 223;
    see the description of the 'Result' data object.
    */
    enum TPCmdResult
        {
        KPCmdResultNotSet                       =-1,

        // ------------------
        // Successful results
        // ------------------

        /** Command performed successfully. There were no problems. */
        KSuccess                                =0x00,

        /** Command performed with partial comprehension; see 'Command results'
        in ETSI TS 102 223. */
        KPartialComprehension                   =0x01,

        /** Command performed, with missing information. Use when you received
        a minimum set of components but you believe some components to be
        missing. */
        KMissingInformation                     =0x02,

        /** REFRESH performed with additional EFs read. */
        KRefreshAdditionEFRead                  =0x03,

        /** Command performed successfully but requested icon could not be displayed. */
        KSuccessRequestedIconNotDisplayed       =0x04,

        /** Command performed, but modified by call control. Use this to
        indicate that call control modified the type of request indicated in
        the proactive command, and that the action requested by call control
        was performed successfully. */
        KModifiedByCallControl                  =0x05,

        /** Command performed successfully, limited service. */
        KSuccessLimitedService                  =0x06,

        /** Command performed with modification. Use this to indicate that you
        were unable to process the command using the exact parameters provided,
        but the command was processed with the best possible parameters.*/
        KPerformedWithModifications             =0x07,

        /** REFRESH performed but either the (U)SIM or R-UIM was not active. */
        KRefreshUSIMNotActive                   =0x08,

        /** PLAY TONE command performed successfully. */
		KPlayTonePerformedSuccessfully		=0x09,

        /** (U)SAT / CCAT session terminated by the user. */
        KPSessionTerminatedByUser               =0x10,

        /** Backward move in the (U)SAT / CCAT  session requested by the user. */
        KBackwardModeRequestedByUser            =0x11,

        /** No response from user. */
        KNoResponseFromUser                     =0x12,

        /** Help information required by the user.

        The UICC may offer help to accompany a GET INKEY, GET INPUT, SET UP
        MENU or SELECT ITEM proactive command. This help is normally text for
        the phone's user. If you tell the user that help is available, and if
        they would like the help, then respond with this value.

        @see RSat::THelp */
        KHelpRequestedByUser                    =0x13,

        /** USSD (Unstructured Supplementary Services Data) session terminated
        by the user. */
        KUssdTransactionTerminatedByUser        =0x14,

        // ----------------
        // Temporary faults
        // ----------------

        /** The phone is currently unable to process command.

        Besides returning this value, you must return additional information.
        Return a value from RSat::TMeProblem and set the additional information
        type to RSat::KMeProblem. */
        KMeUnableToProcessCmd                   =0x20,

        /** The network is currently unable to process the command.

        The network will return a cause value to indicate the problem. These
        values are defined in the appendices of 3GPP TS 24.008.  Set bit 8 of
        the 8-bit code to 1, then include this code in the 'additional
        information' field in your response to the UICC. Set the additional
        information type to RSat::KSatNetworkErrorInfo.

        If no cause code is available, set the 'additional information' field to zero.*/
        KNetworkUnableToProcessCmd              =0x21,

        /** In some proactive commands, you are required to solicit and receive
        the user's approval before executing the proactive command. If the user
        does not give approval then you should not execute it, and you should
        give this response to the UICC. */
        KPCmdNotAcceptedByUser                  =0x22,

        /** The user cleared down the call before the call
        connected or before the network released the call. */
        KCallClearedBeforeConnectionOrReleased  =0x23,

        /** Use this code when the UICC requests an action for a timer to be
        taken by the terminal and the state of the timer does not allow that
        action. */
        KContradictionWithTimerState            =0x24,

        /** Use this to indicate that call control modified
        the type of request indicated in the proactive command, and that the
        action requested by call control encounters a temporary problem. */
        KInteractionWithCCTemporaryError        =0x25,

        /** Error launching the browser.

        Besides returning this value, you must return an additional value
        from RSat::TBrowserError. Set the additional information
        type to RSat::KMeProblem. */
        KLaunchBrowserError                     =0x26,

        /** The ME has rejected a RETRIEVE/SUBMIT/DISPLAY MUTIMEDIA MESSAGE 
        proactive command, because it is busy processing some other MMS transaction.
        
        @see RSat::KMeProblem
        */
        KMMSTemporaryProblem					=0x27,

        // ----------------
        // Permanent faults
        // ----------------

        /** Command is beyond your capabilities. You understand what the
        command is asking you to do, but you do not have the capability to do
        it.  E.g. your code is running on a device that only supports SMS and
        the command asks you to set up a call. */
        KCmdBeyondMeCapabilities                =0x30,

        /** Proactive command type not understood. It is unlikely this will be
        needed - it is to allow future expansion of commands. */
        KCmdTypeNotUnderstood                   =0x31,

        /** Command data not understood. You understand the command type is
        understood but not the accompanying data object(s). */
        KCmdDataNotUnderstood                   =0x32,

        /** Command number not known. */
        KCmdNumberNotKnown                      =0x33,

        /** The network returns a Supplementary Service (SS) error in response
        to a previous SS command. Specific cause values are the same as given
        by the network in the Return Error message.

        The network will return an error value given in the Facility
        information element to indicate the problem.  Set bit 8 of
        the 8-bit code to 1, then include this code in the 'additional
        information' field in your response to the UICC. Set the additional
        information type to RSat::KSatNetworkErrorInfo.

        If no cause code is available, set the 'additional information' field to zero.*/
        KSsReturnError                          =0x34,

        /** The network returns an error in response to the phone trying to send a
        short message.

        To indicate the problem, the network will return a cause value in the
        RP cause element from the RP-ERROR message.  Set bit 8 of
        the 8-bit code to 1, then include this code in the 'additional
        information' field in your response to the UICC. Set the additional
        information type to RSat::KSatNetworkErrorInfo.

        If no cause code is available, set the 'additional information' field to zero.*/
        KSmsRpError  =0x35,

        /** Required values are missing.  You understand the command but one or
        more required data objects are missing.  The specifications defining
        the (U)SAT and CCAT toolkits define the minimum required objects for
        each command. */
        KErrorRequiredValuesMissing             =0x36,

        /** The network returns a USSD error in response to a previous USSD
        command. Specific cause values are the same as given by the network in
        a Return Error message.  This has additional information: the error value
        given in the Facility information element returned by the network. Set bit 8 of
        the 8-bit code to 1, then include this code in the 'additional
        information' field in your response to the UICC. Set the additional
        information type to RSat::KSatNetworkErrorInfo.

        If no cause code is available, set the 'additional information' field to zero.*/
        KUssdReturnError                        =0x37,

        /** Multiple card commands error.

        Besides returning this value, you must return an additional value
        from RSat::TMultipleCardError. Set the additional information
        type to RSat::KMeProblem. */
        KMultipleCardCmdsError                  =0x38,

        /** Interaction with call control, permanent problem.

        Besides returning this value, you must return an additional value
        from RSat::TCcInteractionError. Set the additional information
        type to RSat::KMeProblem. */
        KInteractionWithCCPermanentError        =0x39,

        /** Bearer Independent Protocol error.

        Besides returning this value, you must return an additional value
        from RSat::TBearerIndProtoError. Set the additional information
        type to RSat::KMeProblem. */
        KBearerIndepProtocolError               =0x3A,

        /** Access Technology unable to process command. */
        KAccessTechUnableProcessCmd             =0x3B,
        
        /** An error occured while trying to process the SET FRAMES or GET FRAMES STATUS command. */
        KFramesError							=0x3C,
        
        /** A permanent error occured while trying to process one of the RETRIEVE/SUBMIT/DISPLAY
        MUTIMEDIA MESSAGE proactive commands. 
        
        @see RSat::KMeProblem
        */
        KMMSError								=0x3D

        };

    enum TAdditionalInfoType
    /**
    All proactive commands require a response from you to the UICC via the
    RSat::TerminalRsp() method.  Some require a descriptor containing 'additional
    information', RSat::TAdditionalInfo.  The information in this descriptor will
    depend upon the type of proactive command to which you are responding; see
    the description of each proactive command.

    Besides returning the descriptor, you must indicate the type of
    additional information by returning a value from RSat::TAdditionalInfoType.

    @see RSat::TAdditionalInfo
    @see RSat::TerminalRsp()
    */
        {
        KNoAdditionalInfo           = 0x0001,  //< No additional result info has been provided.
        KMeProblem                  = 0x0002,  //< Possible additional information, in the event of an phone problem, is described by RSat::TMeProblem.

        /** The additional information, in this case, consists of the error
        code returned by the network. The error code returned by the network is
        available to the client, as it is passed up to the client in the
        extended GSM error code as a response to a SET UP CALL, a SEND SS, a
        SEND SHORT MESSAGE, SEND USSD or SEND DTMF. */
        KSatNetworkErrorInfo        = 0x0004,

        KControlInteraction         = 0x0008,  //< Interaction with call control or MO SM control.
        KSendSsInfo                 = 0x0010,  //< Additional info on a successful result for a SEND SS proactive command.
        KTextString                 = 0x0020,  //< Text string - additional info for a successful GET INKEY, GET INPUT or SEND USSD.
        KItemIdentifier             = 0x0040,  //< Item selected as a result to a SELECT ITEM proactive command.
        KCallControlRequestedAction = 0x0080,  //< Required if a call control by SIM has modified a proactive command SET UP CALL, SEND SS or SEND USSD in another type of request.
        KRApdu                      = 0x0100,  //< Required in response to PERFORM CARD APDU.
        KCardATR                    = 0x0200,  //< Required in response to POWER ON CARD.
        KATResponse                 = 0x0400,  //< Required in response to RUN AT COMMAND.
        KChannelData                = 0x0800,  //< Required in response to RECEIVE DATA.
        KChannelStatusInfo          = 0x1000,  //< Required in response to GET CHANNEL STATUS and OPEN CHANNEL.
        KServiceAvailability        = 0x2000,  //< Required in response to Service Search.
        KCardReaderInfo             = 0x4000,  //< Required in response to GET READER STATUS.
        KLocalInformation           = 0x8000,  //< Required in response to PROVIDE LOCAL INFORMATION.
        KTimerMgmt                  = 0x10000, //< Required in response to TIMER MANAGEMENT.
        KPollingIntv                = 0x20000, //< Required in response to POLLING INTERVAL.
        KServiceRecord              = 0x40000, //< Required in response to GET SERVICE INFORMATION.
        KFramesInfo                 = 0x80000  //< Required in response to SET FRAMES and GET FRAMES STATUS.
        };

    /**
    Addition information provide to the UICC in response to proactive
    commands. A value from this enumeration should be provided when the
    general response to a command is RSat::KMeUnableToProcessCmd
    Set the additional information type to RSat::KMeProblem.

    Enum values are dictated by 3GPP 31.111 and ETSI TS 102 223; see the
    description of the 'Result' data objects.
    */
    enum TMeProblem
        {
        KNoSpecificMeProblem        = 0x00, //< No specific cause can be given.
        KScreenBusy                 = 0x01, //< Screen is busy.
        KMeBusyOnCall               = 0x02, //< Busy on call.
        KMeBusyOnSs                 = 0x03, //< Busy on supplementary service.
        KNoService                  = 0x04, //< No service.
        KAccBar                     = 0x05, //< Access control class bar.
        KRadioRessourceNotGranted   = 0x06, //< Radio resource not granted.
        KNotInSpeechCall            = 0x07, //< Not in speech call.
        KMeBusyOnUssd               = 0x08, //< Busry on USSD.
        KMeBusyOnSendDtmf           = 0x09, //< Busy on SEND DTMF command.
        KNoUSIMActive               = 0x0A  //< No (U)SIM or R-UIM active.
        };

    /**
    Addition information provide to the UICC in response to proactive
    commands. A value from this enumeration should be provided when the
    general response to a command is RSat::KInteractionWithCCPermanentError.
    Set the additional information type to RSat::KMeProblem.

    Enum values are dictated by ETSI TS 102 223; see the description of the
    'Result' data objects.
    */
    enum TCcInteractionError
        {
        KNoSpecificInteractionError = 0x00, //< No specific cause can be given.
        KActionNotAllowed           = 0x01, //< Action not allowed.
        KRequestTypeChanged         = 0x02  //< The type of request has changed.
        };

    /**
    Addition information provide to the UICC in response to proactive
    commands. A value from this enumeration should be provided when the
    general response to a command is RSat::KMultipleCardCmdsError.
    Set the additional information type to RSat::KMeProblem.

    Enum values are dictated by ETSI TS 102 223; see the description of the
    'Result' data objects.
    */
    enum TMultipleCardError
        {
        KNoSpecificMultiCardError   = 0x00, //< No specific cause can be given.
        KNoCardReader               = 0x01, //< Card reader removed or not present
        KNoCard                     = 0x02, //< Card removed or not present
        KCardReadyBusy              = 0x03, //< Card reader busy.
        KCardPoweredOff             = 0x04, //< Card powered off.
        KCApduFormatError           = 0x05, //< C-APDU format error.
        KMuteCard                   = 0x06, //< Mute card.
        KTransmissionError          = 0x07, //< Transmission error.
        KProtocolNotSupported       = 0x08, //< Protocol not supported.
        KSpecifiedReaderNotValid    = 0x09  //< Specified reader not valid.
        };

    /**
    Addition information provide to the UICC in response to proactive
    commands. A value from this enumeration should be provided when the
    general response to a command is RSat::KLaunchBrowserError.
    Set the additional information type to RSat::KMeProblem.
    */
    enum TBrowserError
        {
        KNoSpecificBrowserError         = 0x00,
        KBearerUnvailable               = 0x01,
        KBrowserUnavailable             = 0x02,
        KMeUnableToReadProvisioningData = 0x04
        };

    /**
    Addition information provide to the UICC in response to proactive
    commands. A value from this enumeration should be provided when the
    general response to a command is RSat::KBearerIndepProtocolError
    Set the additional information type to RSat::KMeProblem.

    Enum values are dictated by ETSI TS 102 223; see the description of the
    'Result' data objects.
    */
    enum TBearerIndProtoError
        {
        KNoSpecificBIPError         = 0x00, //< No specific cause can be given.
        KNoChannelAvailable         = 0x01, //< No channel available.
        KChannelClosed              = 0x02, //< Channel closed.
        KChannelIdNotValid          = 0x03, //< Channel identifier not valid.
        KBufferSizeNotAvailable     = 0x04, //< Requested buffer size not available.
        KSecurityError              = 0x05, //< Security error (unsuccessful authentication).
        KTranportLevelNotAvailable  = 0x06, //< Requested UICC/terminal interface transport level not available.
        KRemoteDeviceNotReachable   = 0x07, //< Remote device is not reachable (not present, not physically connected, switched off, etc.).
        KServiceError               = 0x08, //< Service error (service not available on remote device).
        KServiceIdUnknown           = 0x09, //< Service identifier unknown.
        KPortNotAvailable			= 0x10	//< Port not available (applicable for OPEN CHANNEL related to UICC Server Mode).
        };

	/**
    Addition information provide to the UICC in response to proactive
    commands. A value from this enumeration should be provided when the
    general response to a command is RSat::KFramesError
    Set the additional information type to RSat::KMeProblem.

    Enum values are dictated by ETSI TS 102 223; see the description of the
    'Result' data objects.
    */
	enum TFramesError
   		{
   		KNoSpecificCauseCanBeGiven 					= 0x00, //< This error is reported when a specific reason for the KFramesError is not reported.
		KFrameIdentifierIsNotValid 					= 0x01, //< The indicator is flagged when the frame identifier is not valid.                                 
		KNumberOfFramesExceeded                 	= 0x02, //< This indicator is flagged when the number of frames are beyond the terminal capability.
		KNoFrameIdentified          				= 0x03, //< This indicator is flagged when no frame is identified.
		KRequestSizeNotSupported     				= 0x04, //< This indicator is flagged when the requested size is not supported.
		KDefaultActiveFrameIsNotValid				= 0x05  //< This indicator is flagged when the default active frames is not valid.
    	};
    	
	/**
	@see TAdditionalInfo
	*/
    enum {KAdditionalInfoMaxSize=244};

    /**
    All proactive commands require a response from you to the UICC via the
    RSat::TerminalRsp() method.  Some require a buffer containing 'additional
    information'.  The information in this buffer will depend upon the type of
    proactive command to which you are responding; see the description of each
    proactive command.

    Besides returning a RSat::TAdditionalInfo, you must indicate the type of
    additional information by returning a value from RSat::TAdditionalInfoType.
    @see RSat::TAdditionalInfoType
    @see RSat::TerminalRsp()
    */
    typedef TBuf<KAdditionalInfoMaxSize> TAdditionalInfo;

    enum TPCmd
    /**
    Enumeration containing proactive commands for use with the
    RSat::TerminalRsp(), RSat::NotifyTsyStateUpdated() and
    RTelSubSessionBase::CancelAsyncRequest() methods only.  Members of this
    enumeration do not have the same value as the proactive command codes in
    3GPP TS 31.111 and ETSI TS 102 223.
    */
        {
        EDisplayText,
        EGetInkey,
        EGetInput,
        EPlayTone,
        ERefresh,
        ESelectItem,
        ESendDtmf,
        ESendSm,
        ESendSs,
        ESendUssd,
        ESetUpCall,
        ESetUpEventList,
        ESetUpIdleModeText,
        ESetUpMenu,
        EPerformCardApdu,
        EPowerOffCard,
        EPowerOnCard,
        EGetReaderStatus,
        ERunAtCommand,
        ELanguageNotification,
        ELaunchBrowser,
        EOpenChannelCs,
        EOpenChannelGprs,
        EOpenChannelLocal,
        ECloseChannel,
        EReceiveData,
        ESendData,
        EGetChannelStatus,
        EDeclareService,
        EServiceSearch,
        EGetServiceInformation,
        ETimerMngt,
        ELocalInfo,
        EPollingInterval,
        EPollingOff,
        ERetrieveMultimediaMsg,
        ESubmitMultimediaMsg,
        EDisplayMultimediaMsg,
        ESetFrames,
        EGetFramesStatus,
        EOpenChannelUiccServerMode
        };

    IMPORT_C void TerminalRsp(TRequestStatus& aReqStatus, TPCmd aPCmd, const TDesC8& aRsp) const;

    //
    // Client ready indication
    //
    IMPORT_C TInt UsatClientReadyIndication() const;

    //
    // TERMINAL PROFILE
    //

    // TSatProfileByteX enum values are dictated by 3GPP 31.111

    enum TSatProfileByte1
    /** Flags for byte 1 of the terminal profile.
    @see RSat::TSatProfileV1 */
        {
        KCapsProfileDownload        =0x01,  //< Profile download
        KCapsSmsPpDataDownload      =0x02,  //< USAT only: SMS-PP data download
        KCapsCbDataDownload         =0x04,  //< USAT only: Cell Broadcast data download
        KCapsMenuSelection          =0x08,  //< Menu selection
        KCaps9EXX                   =0x10,  //< To be used only in v1.0 API
        KCapsSmsPpDataDownload2     =0x10,  //< v2.0 API only: SMS-PP data download supported.
        KCapsTimerExpiration        =0x20,  //< Timer expiration
        KCapsUssdStringInCC         =0x40,  //< To be used only in v1.0 API
        KCapsCCByUSim2              =0x40,  //< USAT only: Call Control by USIM supported.
        KCapsCCByUSim3              =0x80   //< v2.0 API only: Call Control supported.
        };

    enum TSatProfileByte2
    /** Flags for byte 2 of the terminal profile.
    @see RSat::TSatProfileV1 */
        {
        KCapsCommandResult          =0x01,  //< Command result
        KCapsCCBySim                =0x02,  //< v2.0 API only: Call Control by USIM
        KCapsCellIdInCCBySim        =0x04,  //< To be used only in v1.0 API
        KCapsCCByUSim4              =0x04,  //< v2.0 API only: Call Control supported.
        KCapsMOSMControl            =0x08,  //< USAT only: MO short message control support
        KCapsAlphaIdHandling        =0x10,  //< To be used only in v1.0 API
        KCapsCCByUSim5              =0x10,  //< v2.0 API only: Call Control supported.
        KCapsUcs2Entry              =0x20,  //< UCS2 Entry supported
        KCapsUcs2Display            =0x40,  //< UCS2 Display supported
        KCapsExtensionTextDisplay   =0x80,  //< To be used only in v1.0 API
        KCapsDisplayText2           =0x80   //< v2.0 API only: DISPLAY TEXT supported.
        };

    enum TSatProfileByte3
    /** Flags for byte 3 of the terminal profile.
    @see RSat::TSatProfileV1 */
        {
        KCapsDisplayText    =0x01, //< Proactive UICC: DISPLAY TEXT
        KCapsGetInkey       =0x02, //< Proactive UICC: GET INKEY
        KCapsGetInput       =0x04, //< Proactive UICC: GET INPUT
        KCapsMoreTime       =0x08, //< Proactive UICC: MORE TIME
        KCapsPlayTone       =0x10, //< Proactive UICC: PLAY TONE
        KCapsPollInterval   =0x20, //< Proactive UICC: POLL INTERVAL
        KCapsPollingOff     =0x40, //< Proactive UICC: POLLING OFF
        KCapsRefresh        =0x80  //< Proactive UICC: REFRESH
        };

    enum TSatProfileByte4
    /** Flags for byte 4 of the terminal profile.
    @see RSat::TSatProfileV1 */
        {
        KCapsSelectItem                 =0x01, //< Proactive UICC: SELECT ITEM
        KCapsSendSM                     =0x02, //< Proactive UICC: SEND SHORT MESSAGE
        KCapsSendSS                     =0x04, //< Proactive UICC: SEND SS
        KCapsSendUSSD                   =0x08, //< Proactive UICC: SEND USSD
        KCapsSetUpCall                  =0x10, //< Proactive UICC: SET UP CALL
        KCapsSetUpMenu                  =0x20, //< Proactive UICC: SET UP MENU
        KCapsProvideLocalInformation    =0x40, //< Proactive UICC: PROVIDE LOCAL INFORMATION (MCC, MNC, LAC, Cell ID & IMEI)
        KCapsProvideLocalInformationNMR =0x80  //< Proactive UICC: PROVIDE LOCAL INFORMATION (NMR)
        };

    enum TSatProfileByte5
    /** Flags for byte 5 of the terminal profile.
    @see RSat::TSatProfileV1 */
        {
        KCapsSetUpEventList             =0x01, //< Proactive UICC: SET UP EVENT LIST
        KCapsMtCallEvent                =0x02, //< Event: MT call
        KCapsCallConnectedEvent         =0x04, //< Event: Call connected
        KCapsCallDisconnectedEvent      =0x08, //< Event: Call disconnected
        KCapsLocationStatusEvent        =0x10, //< Event: Location status
        KCapsUserActivityEvent          =0x20, //< Event: User activity
        KCapsIdleScreenAvailableEvent   =0x40, //< Event: Idle screen available
        KCapsCardReaderStatusEvent      =0x80  //< Event: Card reader status
        };

    enum TSatProfileByte6
    /** Flags for byte 6 of the terminal profile. */
        {
        KCapsLanguageSelectionEvent     =0x01, //< Event: Language selection
        KCapsBrowserTerminationEvent    =0x02, //< Event: Browser Termination
        KCapsDataAvailableEvent         =0x04, //< Event: Data available
        KCapsChannelStatusEvent         =0x08, //< Event: Channel status
        KCapsAccTechnoChangedEvent      =0x10, //< Event: Access Technology Change
        KCapsDisplayParamsChangedEvent  =0x20, //< Event: Display parameters changed
        KCapsLocalConnectionEvent       =0x40,  //< Event: Local Connection
        KCapsNetworkSearchModeChangeEvent	=0x80  //< Event: Network Search Mode Change (added in V6)
        };

    enum TSatProfileByte7
    /** Flags for byte 7 of the terminal profile.
    @see RSat::TSatProfileV1 */
        {
        KCapsPowerOnCard        =0x01, //< Proactive UICC: POWER ON CARD
        KCapsPowerOffCard       =0x02, //< Proactive UICC: POWER OFF CARD
        KCapsPerformCardApdu    =0x04, //< Proactive UICC: PERFORM CARD APDU
        KCapsGetReaderStatus    =0x08, //< Proactive UICC: GET READER STATUS (Card reader status)
        KCapsGetReaderStatusId  =0x10  //< Proactive UICC: GET READER STATUS (Card reader identifier)
        //Other Values are RFU
        };

    enum TSatProfileByte8
    /** Flags for byte 8 of the terminal profile.
    @see RSat::TSatProfileV1 */
        {
        KCapsTimerManagementStartStop           =0x01, //< Proactive UICC: TIMER MANAGEMENT (start, stop)
        KCapsTimerManagementGetCurrentValue     =0x02, //< Proactive UICC: TIMER MANAGEMENT (get current value)
        KCapsProvideLocalInfoDateTimeTimeZone   =0x04, //< Proactive UICC: PROVIDE LOCAL INFORMATION (date, time and time zone)
        KCapsBinaryChoiceGetInkey               =0x08, //< To be used only in v1.0 API
        KCapsGetInkey2                          =0x08, //< v2.0 API only: GET INKEY supported.
        KCapsSetUpIdleModeText                  =0x10, //< Proactive UICC: SET UP IDLE MODE TEXT
        KCapsRunAtCmd                           =0x20, //< RUN AT COMMAND (i.e. class "b" is supported)
        KCaps2ndAlphaIdSetUpCall                =0x40, //< To be used only in v1.0 API
        KCapsSetUpCall2                         =0x40, //< v2.0 API only: SET UP CALL supported.
        KCaps2ndCapabilityConfigParam           =0x80, //< To be used only in v1.0 API
        KCapsCCByUSim6                          =0x80  //< v2.0 API only: Call Control supported.
        };

    enum TSatProfileByte9
    /** Flags for byte 9 of the terminal profile.
    @see RSat::TSatProfileV1 */
        {
        KCapsSustainedDisplayText       =0x01,  //< To be used only in v1.0 API
        KCapsDisplayText3               =0x01,  //< v2.0 API only: DISPLAY TEXT supported.
        KCapsSendDTMFCmd                =0x02,  //< Proactive UICC: SEND DTMF
        KCapsProvideLocalInfoBcchCoding =0x04,  //< To be used only in v1.0 API
        KCapsProvideLocalnfoNmr2        =0x04,  //< v2.0 API only: Proactive UICC: PROVIDE LOCAL INFORMATION (NMR) supported.
        KCapsProvideLocalInfoLanguage   =0x08,  //< Proactive UICC: PROVIDE LOCAL INFORMATION (language)
        KCapsProvideLocalInfoTimingAdv  =0x10,  //< USAT only: Proactive UICC: PROVIDE LOCAL INFORMATION (Timing Advance)
        KCapsLanguageNotification       =0x20,  //< Proactive UICC: LANGUAGE NOTIFICATION
        KCapsLaunchBrowser              =0x40,  //< Proactive UICC: LAUNCH BROWSER
        KCapsProvideLocalInfoAccTech    =0x80   //< Proactive UICC: PROVIDE LOCAL INFORMATION (Access Technology)
        };

    enum TSatProfileByte10
     /** Flags for byte 10 of the terminal profile.
     @see RSat::TSatProfileV2 */
       {
        KCapsSoftKeySelectItem  =0x10, //< Soft keys support for SELECT ITEM
        KCapsSoftKeySetUpMenu   =0x20  //< Soft Keys support for SET UP MENU
        };

    enum TSatProfileByte12
    /** Flags for byte 12 of the terminal profile.
    @see RSat::TSatProfileV2 */
        {
        KCapsOpenChannel        =0x01, //< Proactive UICC: OPEN CHANNEL
        KCapsCloseChannel       =0x02, //< Proactive UICC: CLOSE CHANNEL
        KCapsReceiveData        =0x04, //< Proactive UICC: RECEIVE DATA
        KCapsSendData           =0x08, //< Proactive UICC: SEND DATA
        KCapsGetChannelStatus   =0x10, //< Proactive UICC: GET CHANNEL STATUS
        KCapsServiceSearch      =0x20, //< Proactive UICC: SERVICE SEARCH
        KCapsGetServiceInfo     =0x40, //< Proactive UICC: GET SERVICE INFORMATION
        KCapsDeclareService     =0x80  //< Proactive UICC: DECLARE SERVICE
        };

    enum TSatProfileByte13
     /** Flags for byte 13 of the terminal profile.
     @see RSat::TSatProfileV2 */
		{
        KCapsCsdSupportByMe     =0x01, //< CSD supported by phone
        KCapsGprsSupportByMe    =0x02, //< GPRS supported by phone
        KCapsBtSupportByMe      =0x04, //< Bluetooth supported by phone
        KCapsIrdaSupportByMe    =0x08, //< IrDA supported by phone
        KCapsRs232SupportByMe   =0x10  //< RS232 supported by phone
        };

    enum TSatProfileByte14
    /** Flags for byte 14 of the terminal profile.
    @see RSat::TSatProfileV2 */
        {
        KCapsScreenSizingParams =0x80 //< Screen Sizing Parameters supported, as defined in bytes 14, 15 and 16.
        };

    enum TSatProfileByte15
    /** Flags for byte 15 of the terminal profile.
    @see RSat::TSatProfileV2 */
        {
        KCapsVariableSizeFonts  =0x80 //< Variable size fonts Supported
        };

    enum TSatProfileByte16
    /** Flags for byte 16 of the terminal profile.
    @see RSat::TSatProfileV2 */
        {
        KCapsDisplayResize          =0x01, //< Display can be resized
        KCapsTextWrapping           =0x02, //< Text Wrapping supported
        KCapsTextScrolling          =0x04, //< Text Scrolling supported
        KCapsWidthReductionInMenu   =0x80  //< Text Attributes supported.
        };

    enum TSatProfileByte17
    /** Flags for byte 17 of the terminal profile.
    @see RSat::TSatProfileV2 */
        {
        KCapsTcp    		=0x01, 	//< TCP, UICC in client mode
        KCapsUdp    		=0x02,  //< UDP, UICC in client mode
        KCapsTcpServerMode	=0x04	//< TCP, UICC in server mode
        							//	 Next four bits RFU, bit = 0
        							//	 Last bit, reserved by 3GPP (HSDPA)
        };

    enum TSatProfileByte18
    /** Flags for byte 18 of the terminal profile.
    @see RSat::TSatProfileV2 */
        {
        KCapsDisplayTextVariableTimeout =0x01, //< Proactive UICC: DISPLAY TEXT (Variable Time out)
        KCapsGetInkeyHelp               =0x02, //< Proactive UICC: GET INKEY (help is supported while waiting for immediate response or variable timeout)
        KCapsUsbSupportByMe             =0x04, //< USB supported by phone.
		KCapsGetInkeyVariableTimeout    =0x08, //< PROACTIVE UICC: GET INKEY (Variable Timeout)
        									   //< Bit 5 is reserved for ETSI SCP
        KCapsCallControlOnGprs          =0x20,  //< Call Control on GPRS
        KCapsProvideLocalInfoIMEISV    			=0x40, //< Proactive UICC: PROVIDE LOCAL INFORMATION (IMEISV, added in V6)
        KCapsProvideLocalInfoSearchModeChange 	=0x80  //< Proactive UICC: PROVIDE LOCAL INFORMATION (Search Mode Change, added in V6)
        };

	// Byte 19 reserved for TIA/EIA-136-C facilities
	// Byte 20 reserved for TIA/IS-820-A facilities
	
	enum TSatProfileByte21
    /** Flags for byte 21 of the terminal profile (extended browser capability).
    @see RSat::TSatProfileV6
    */
     	{
		KCapsWML     =0x01, //< WML Browser is supported by phone.
		KCapsXHTML   =0x02, //< XHTML Browser is supported by phone.
		KCapsHTML    =0x04, //< HTML Browser is supported by phone.   
		KCapsCHTML   =0x08  //< CHTML Browser is supported by phone.
		// The rest of the bits in this byte are RFU 
    	};

	enum TSatProfileByte22
    /** Flags for byte 22 of the terminal profile.
    @see RSat::TSatProfileV6 
    */
		{
		KCapsProvideLocalInfoUTRAN			=0x01, //< Reserved by 3GPP (UTRAN PS with extended params)
		KCapsProvideLocalInfoBatteryState	=0x02, //< Proactive UICC: PROVIDE LOCAL INFORMATION (battery state)  
		KCapsPlayThemedAndMelodyTone		=0x04, //< Proactive UICC: PLAY TONE (themed and melody tones supported (compare this with 5th bit of byte 3)    
		KCapsSetUpCallMultimediaCall		=0x08, //< Multimedia calls in SET UP CALL
		KCapsRefreshGBA						=0x10, //< Reserved by 3GPP (Toolkit-Initiated GBA) 
		KCapsRetrieveMultimediaMessage 		=0x20, //< Proactive UICC: Reserved by 3GPP (RETRIEVE MULTIMEDIA MESSAGE)   
		KCapsSubmitMultimediaMessage	 	=0x40, //< Proactive UICC: Reserved by 3GPP (SUBMIT MULTIMEDIA MESSAGE)  
		KCapsDisplayMultimediaMesage 		=0x80  //< Proactive UICC: Reserved by 3GPP (DISPLAY MULTIMEDIA MESSAGE)  
   		};

	enum TSatProfileByte23
    /** Flags for byte 23 of the terminal profile.
    @see RSat::TSatProfileV6 
    */
     	{
		KCapsSetFrames     				=0x01, //< Proactive UICC: SET FRAMES
		KCapsGetFramesStatus  			=0x02, //< Proactive UICC: GET FRAMES STATUS
		KCapsMmsNotificationDownload 	=0x04, //< Reserved by 3GPP (MMS Notification Download)
		//< Bits x and y RFU, bits = 0." where x and y are the bit numbers that are missing.
		KCapsProvideLocalInfoMEID		=0x20, //< Proactive UICC: PROVIDE LOCAL INFORMATION (MEID)
		KCapsProvideLocalInfoNmrUTRAN 	=0x40, //< Reserved by 3GPP (Proactive UICC: PROVIDE LOCAL INFORMATION (NMR-UTRAN))
		KCapsUssdDataDownload			=0x80  //< Reserved by 3GPP (USSD Data download and application mode)
  	 	};

	enum TSatProfileByte24
    /** Bits for byte 24 of the terminal profile.
    
    The first four bits contain the maximum number of frames supported, including frames created
    within existing frames.
    
    The remaining four bits are RFU
    
    @see RSat::TSatProfileV6 
    */
     	{
		KCapsMaxNumFramesBit1 =0x01,
		KCapsMaxNumFramesBit2 =0x02,
		KCapsMaxNumFramesBit3 =0x04,
		KCapsMaxNumFramesBit4 =0x08
		// The rest of the bits in this byte are RFU 
	   	};

	enum TSatProfileByte25
    /** Flags for byte 25 of the terminal profile.
    @see RSat::TSatProfileV6 
    */
     	{
		KCapsBrowsingStatusEvent 		=0x01, //< Event: Browsing Status
		KCapsMmsTransferStatusEvent     =0x02, //< Event: MMS Transfer Status 
		KCapsFrameParametersChangeEvent	=0x04  //< Event: Frame Parameters Changed
		// The rest of the bits in this byte are RFU 
		};
	
	// Bytes 26 and 27 reserved for release 7 and later features.


	enum TSatProfileByte28
	/** Flags for byte 28 of the terminal profile.
	@see RSat::TSatProfileV5 */
		{
		KCapsTextAttributeAlignmentLeft		=0x01,	//< Text attribute alignment left supported.
		KCapsTextAttributeAlignmentCentre	=0x02,	//< Text attribute alignment centre supported.
		KCapsTextAttributeAlignmentRight 	=0x04,	//< Text attribute alignment right supported.
		KCapsTextAttributeFontSizeNormal 	=0x08,	//< Text attribute font size normal supported.
		KCapsTextAttributeFontSizeLarge		=0x10,	//< Text attribute font size large supported.
		KCapsTextAttributeFontSizeSmall		=0x20	//< Text attribute font size small supported.
													//< Bits 7 and 8 RFU, bits = 0.
		};

	enum TSatProfileByte29
	/** Flags for byte 29 of the terminal profile.
	@see RSat::TSatProfileV5 */
		{
		KCapsTextAttributeStyleNormal 		=0x01,	//< Text attribute style normal supported.
		KCapsTextAttributeStyleBold			=0x02,	//< Text attribute style bold supported.
		KCapsTextAttributeStyleItalic 		=0x04,	//< Text attribute style italic supported.
		KCapsTextAttributeStyleUnderline 	=0x08,	//< Text attribute style underline supported.
		KCapsTextAttributeStyleStrikethrough=0x10,	//< Text attribute style strikethrough supported.
		KCapsTextAttributeStyleForeground	=0x20,	//< Text attribute foreground colour supported.
		KCapsTextAttributeStyleBackground	=0x40	//< Text attribute background colour supported.
													//< Bit 8 RFU, bit = 0.
		};

    class TSatProfileV1 : public TSatBase
    /**
    States the SAT facilities supported by the Client and/or the ME-side.
    @see RSat::ClientSatProfileIndication()
    @see RSat::GetMeSideSatProfile()
    */
        {
    public:
        IMPORT_C TSatProfileV1();

        TUint8 iSatProfileByte1;    //< Byte 1 of the (U)SAT Profile; see RSat::TSatProfileByte1
        TUint8 iSatProfileByte2;    //< Byte 2 of the (U)SAT Profile; see RSat::TSatProfileByte2
        TUint8 iSatProfileByte3;    //< Byte 3 of the (U)SAT Profile; see RSat::TSatProfileByte3
        TUint8 iSatProfileByte4;    //< Byte 4 of the (U)SAT Profile; see RSat::TSatProfileByte4
        TUint8 iSatProfileByte5;    //< Byte 5 of the (U)SAT Profile; see RSat::TSatProfileByte5
        TUint8 iSatProfileByte6;    //< Byte 6 of the (U)SAT Profile; see RSat::TSatProfileByte6
        TUint8 iSatProfileByte7;    //< Byte 7 of the (U)SAT Profile; see RSat::TSatProfileByte7
        TUint8 iSatProfileByte8;    //< Byte 8 of the (U)SAT Profile; see RSat::TSatProfileByte8
        TUint8 iSatProfileByte9;    //< Byte 9 of the (U)SAT Profile; see RSat::TSatProfileByte9
        };

    /** A packaged RSat::TSatProfileV1. */
    typedef TPckg<TSatProfileV1>    TSatProfileV1Pckg;


    class TSatProfileV2 : public TSatProfileV1
    /**
    States the (U)SAT facilities supported by the Client and/or the ME-side
    @see RSat::ClientSatProfileIndication()
    @see RSat::GetMeSideSatProfile()
    */
        {
    public:
        IMPORT_C TSatProfileV2();

        TUint8 iSatProfileByte10;   //< Byte 10 of the (U)SAT Profile; see RSat::TSatProfileByte10
        TUint8 iSatProfileByte11;   //< Maximum number of soft keys available. 'FF' value is reserved for future use; see RSat::TSatProfileByte10
        TUint8 iSatProfileByte12;   //< Byte 12 of the (U)SAT Profile; see RSat::TSatProfileByte12
        TUint8 iSatProfileByte13;   //< Byte 13 of the (U)SAT Profile; see RSat::TSatProfileByte13
        TUint8 iNumOfChannels;      //< Number of channels supported by the ME. Max value=7. Part of Byte 13.
        TUint8 iSatProfileByte14;   //< Byte 14 of the (U)SAT Profile; see RSat::TSatProfileByte14
        TUint8 iScreenHeight;       //< Guaranteed number of characters supported down the phone's display without scrolling. Max value=31. Part of Byte 14.
        TUint8 iSatProfileByte15;   //< Byte 15 of the (U)SAT Profile; see RSat::TSatProfileByte15
        TUint8 iScreenWidth;        //< Guaranteed number of characters supported across the phone's display without scrolling. Max value=127. Part of Byte 15.
        TUint8 iSatProfileByte16;   //< Byte 16 of the (U)SAT Profile; see RSat::TSatProfileByte16
        TUint8 iWidthReduction;     //< Width reduction when in a menu. Max value=7. Part of byte 16.
        TUint8 iSatProfileByte17;   //< Byte 17 of the (U)SAT Profile; see RSat::TSatProfileByte17
        TUint8 iSatProfileByte18;   //< Byte 18 of the (U)SAT Profile; see RSat::TSatProfileByte18
        TUint8 iTiaEiaProtoVersion; //< Protocol version as indicated in TIA/EIA 136-123
        };

    /** A packaged RSat::TSatProfileV2. */
    typedef TPckg<TSatProfileV2>    TSatProfileV2Pckg;

	class TSatProfileV5 : public TSatProfileV2
	/**
	States the (U)SAT facilities supported by the Client and/or the ME-side
	@see RSat::ClientSatProfileIndication()
	@see RSat::GetMeSideSatProfile()
	*/
		{
	public:
		IMPORT_C TSatProfileV5();
									//< Byte 19 reserved for TIA/EIA-136 facilities
			 						//< Byte 20 reserved for TIA/EIA/IS-820 facilities
									//< Bytes 21 to 27 inclusive are reserved for release 6 and later features.
		TUint8 iSatProfileByte28;	//< Byte 28 of the (U)SAT Profile (used for text attributes); see RSat::TSatProfileByte28
		TUint8 iSatProfileByte29;	//< Byte 29 of the (U)SAT Profile (used for text attributes); see RSat::TSatProfileByte29
		};

	/** A packaged RSat::TSatProfileV5. */
	typedef TPckg<TSatProfileV5>		TSatProfileV5Pckg;

	class TSatProfileV6 : public TSatProfileV5
	/**
	States the (U)SAT facilities supported by the Client and/or the ME-side
	@see RSat::ClientSatProfileIndication()
	@see RSat::GetMeSideSatProfile()
    */
		{
	public:
		IMPORT_C TSatProfileV6();

		TUint8 iSatProfileByte21;	//< Byte 21 of the (U)SAT Profile; see RSat::TSatProfileByte21
		TUint8 iSatProfileByte22;	//< Byte 22 of the (U)SAT Profile; see RSat::TSatProfileByte22
		TUint8 iSatProfileByte23;	//< Byte 23 of the (U)SAT Profile; see RSat::TSatProfileByte23
		TUint8 iSatProfileByte24;	//< Byte 24 of the (U)SAT Profile; see RSat::TSatProfileByte24
		TUint8 iSatProfileByte25;	//< Byte 25 of the (U)SAT Profile; see RSat::TSatProfileByte25
		};

	/** A packaged RSat::TSatProfileV6. */
	typedef TPckg<TSatProfileV6> TSatProfileV6Pckg;

    IMPORT_C void GetMeSideSatProfile(TRequestStatus& aReqStatus,TDes8& aMeSimSatProfile) const;
    IMPORT_C TInt ClientSatProfileIndication(const TDesC8& aClientSatProfile) const;

    //
    // MENU SELECTION
    //
    /** Part of a MENU SELECTION envelope command in response to a
    SET UP MENU proactive command.  Indicates whether help is required
    with the user's menu selection.
    @see RSat::TMenuSelectionV1 */
    enum THelpRequest
        {
        EHelpRequestNotSet, //< Initialization value used within constructor(s).
        EHelpRequested,     //< The user/client has requested help.
        EHelpNotRequested   //< The user/client has not requested help.
        };

    class TMenuSelectionV1 : public TSatBase
    /**
    Part of a MENU SELECTION envelope command in response to a SET UP MENU
    proactive command.  When the user selects an item from the SET UP MENU
    command's menu, send a RSat::TMenuSelectionV1 to the UICC with
    RSat::MenuSelection().
    @see RSat::NotifySetUpMenuPCmd()
    */
        {
    public:
        IMPORT_C TMenuSelectionV1();
    public:
        /** Each menu item has ID. This field contains the ID of the item that the user selected. */
        TUint8 iItemId;

        /** This indicates that the user requested help with a menu item. */
        THelpRequest iHelp;
        };

    /** A packaged RSat::TMenuSelectionV1. */
    typedef TPckg<TMenuSelectionV1> TMenuSelectionV1Pckg;

    IMPORT_C void MenuSelection(TRequestStatus& aReqStatus, const TDesC8& aSelection) const;

    //
    // CALL CONTROL
    //
    /** Describes the result of call control request, and hence the action required.
    Used by RSat::TCallControlV1::GetCcGeneralResult()
    and RSat::TCallControlV1::SetCcGeneralResult(). */
    enum TControlResult
        {
        /** Initialization value used within constructor(s). */
        EControlResultNotSet,

        /** The action has been allowed by the ICC with no
        modification. However, the call control params might
        contain an alpha identifier to be displayed to the
        user; call RSat::TCallControlV1::GetAlphaId() to get the
        alpha ID (a RSat::TAlphaId) and the alpha ID's validity
        (an RSat::TAlphaIdValidity). */
        EAllowedNoModification,

        /** The action has not been allowed by the ICC. However, the call
        control params might contain an alpha identifier to be displayed to the
        user; call RSat::TCallControlV1::GetAlphaId() to get the alpha ID (a
        RSat::TAlphaId) and the alpha ID's validity (an RSat::TAlphaIdValidity)
        */
        ENotAllowed,

        /** The action has been allowed by the ICC with
        modifications. The call control params contain
        new modified params.

        It may also contain an alpha identifier to be displayed to the user;
        call RSat::TCallControlV1::GetAlphaId() to get the alpha ID (a
        RSat::TAlphaId) and the alpha ID's validity (an RSat::TAlphaIdValidity)
        */
        EAllowedWithModifications
        };

    /** Describes the type of call control request.
    Used by RSat::TCallControlV1::GetCallControlType(). */
    enum TCallControlType
        {
        ECcTypeNotSet,  	//< Initialization value used within constructor(s).
        ECcAddress,     	//< Call control by SIM has transformed the original request into a new call set up.
        ECcSsString,    	//< Call control by SIM has transformed the original request into a new SEND SS
  		ECcUssdString,   	//< Call control by SIM has transformed the original request into a new SEND USSD
        ECcPDPParameters   	//< Call control by SIM has transformed the original request into a new OPEN CHANNEL
        };

    struct TCallSetUpParams
        {
        /** Capability configuration parameters
        corresponding to the bearer capability 1 information
        element of a mobile originating Set Up message. */
        TCcp iCcp1;

        /** Called party subaddress. */
        TSubAddress iSubAddress;

        /** Capability configuration parameters
        corresponding to the bearer capability 2 information
        element of a mobile originating Set Up message. */
        TCcp iCcp2;

        /** Address to use to set up the call. */
        TAddress iAddress;
        };

    /** Indicates the validity of the alpha ID from RSat::TCallControlV1::GetAlphaId(). */
    enum TAlphaIdValidity
        {
        EAlphaIdValidityNotSet, //< Value used with constructor
        EValidAlpaId,           //< Alpha ID is valid.  It should be displayed
        ENullAlphaId,           //< Alpha ID is null.  Display nothing.
        ENoAlphaId              //< Alpha ID is not present.  The client can dislay whatever it likes.
        };

    /** Bearer Capability Repeat Indicator.

	@see RMobileCall::TBCRepeatIndicator
	*/
    enum TBCRepeatIndicator
        {
        EBCAlternateMode, 	//< The alternate mode of the Bearer Capability Repeat Indicator.
   		EBCSequentialMode, 	//< Depreciated.
        EBCFallbackMode,   	//< Support of fallback - mode 1 preferred, mode 2 selected if setup of mode 1 fails.
        EBCServiceChangeAndFallbackMode //< Service change and fallback - mode 1 alternate mode 2, mode 1 preferred
        };


    class TCallControlV1 : public TSatBase
    /**
    Describes details of the action that
    the client has to perform as a result of Call Control by UICC.
    @see RSat::NotifyCallControlRequest()
    */
        {
    public:
        IMPORT_C TCallControlV1();

        IMPORT_C void SetCcGeneralResult    (TControlResult aResult);
        IMPORT_C void SetCallSetUpDetails   (const TCallSetUpParams& aParams);
        IMPORT_C void SetSendSsDetails      (const TSsString& aString);
        IMPORT_C void SetSendUssdDetails    (const TUssdString& aString);
        IMPORT_C void SetAlphaId            (TAlphaIdValidity aValidity, TAlphaId& aAlphaId);
        IMPORT_C void SetBCRepeatIndicator  (const TBCRepeatIndicator& aBCRepeatIndicator);

        IMPORT_C void GetCallControlType    (TCallControlType& aType) const;
        IMPORT_C void GetCcGeneralResult    (TControlResult& aResult) const;
        IMPORT_C TInt GetCallSetUpDetails   (TCallSetUpParams& aParams) const;
        IMPORT_C TInt GetSendSsDetails      (TSsString& aString) const;
        IMPORT_C TInt GetSendUssdDetails    (TUssdString& aString) const;
        IMPORT_C void GetAlphaId            (TAlphaIdValidity& aValidity,TAlphaId& aAlphaId) const;
        IMPORT_C void GetBCRepeatIndicator  (TBCRepeatIndicator& aBCRepeatIndicator) const;
    protected:
        TCallControlType iType;             //< Call control type
        TCcp iCcp1;                         //< Capability Configuration Parameters 1
        TSubAddress iSubAddress;            //< Remote party sub address
        TCcp iCcp2;                         //< Capability Configuration Parameters 2
        TUint8 iDcs;                        //< Data Coding Scheme
        TTypeOfNumber iTypeOfNumber;        //< TON
        TNumberingPlan iNumberPlan;         //< NPI
        TBuf<KMaxMobileTelNumberSize> iBuf; //< Telephone number
        TControlResult iResult;             //< Result of the call control procedure
        TAlphaIdValidity iValidity;         //< Alpha identifier validity
        TAlphaId iAlphaId;                  //< Alpha identifier that may contain text to display to the user.
        TBCRepeatIndicator iBCRepeatIndicator; //< Bearer Capability Repeat Indicator
        };

    /** A packaged RSat::TCallControlV1. */
    typedef TPckg<TCallControlV1>   TCallControlV1Pckg;

    /**  Specifies to the client who should perform the
    modified action: the client side or the ME Side. */
    enum TActionOriginator
        {

        /** Initialization value used within constructor(s)
        or if the call control does not require a new action
        to be performed. */
        EActionOriginatorNotSet,

        /** The client is responsible for performing the
        action described in the call control class. */
        EClientOriginator,

        /** The ME Side is responsible for performing the
        action described in the call control class. The
        client side should still display any provided alpha
        identifier. */
        EMeSideOriginator
        };

    class TCallControlV2 : public TCallControlV1
    /**
    Describes details of the action that
    the client has to perform as a result of Call Control by UICC.
    Adds originator name and type.
    */
        {
    public:
        IMPORT_C TCallControlV2();

        IMPORT_C void SetCallName           (const TName& aName);
        IMPORT_C void SetActionOriginator   (const TActionOriginator aOriginator);

        IMPORT_C void GetCallName           (TName& aName) const;
        IMPORT_C void GetActionOriginator   (TActionOriginator& aOriginator) const;
    protected:
        TName               iCallName;              //< Name of action originator
        TActionOriginator   iNewActionOriginator;   //< Who originated action
        };

    /** A packaged RSat::TCallControlV2. */
    typedef TPckg<TCallControlV2>   TCallControlV2Pckg;


	class TCallControlV5 : public TCallControlV2
    /**
    Adds PDP context activation parameters
    */
        {
    public:
        IMPORT_C TCallControlV5();

        IMPORT_C void SetPdpParameters        (const TPdpParameters& aPdpParams);
        IMPORT_C void GetPdpParameters        (TPdpParameters& aPdpParams) const;
    protected:
        TPdpParameters   iPdpParameters;    //< buffer that holds PDP activation context parameters
        };
    /** A packaged RSat::TCallControlV5. */
    typedef TPckg<TCallControlV5>   TCallControlV5Pckg;

	class TCallControlV6 : public TCallControlV5
    /**
    Adds a parameter to allow the originator of the call to
    be specified.
    
    Request for notification of a call control request using
    RSat::NotifyCallControlRequest passing a packaged descriptor
    of this class to the request.  For future versions of Symbian
    OS, check for updated versions of this class 
    (TSat::TCallControlVx etc.)
    
    @see RSat::TCallControlV6Pckg
    */
        {
    public:
        IMPORT_C TCallControlV6();

        IMPORT_C void SetCallParamOrigin (TCallParamOrigin aCallParamOrigin);
        IMPORT_C void GetCallParamOrigin (TCallParamOrigin& aCallParamOrigin) const;
        
    protected:
    	/**
    	Holds the originator of the call.
    	
    	RSat::TCallParamOrigin contains the same enumerations as
    	RMobileCall::TCallParamOrigin.  The TSY should use the 
    	information received from the incoming call parameters 
    	to populate this member.
    	*/
        TCallParamOrigin   iCallParamOrigin;
        };

    /** A packaged RSat::TCallControlV6. */
    typedef TPckg<TCallControlV6>   TCallControlV6Pckg;
        
    IMPORT_C void NotifyCallControlRequest(TRequestStatus& aReqStatus, TDes8& aCallControlDetails ) const;

    //
    // MO SMS CONTROL
    //
    
    /**
    @see RSat::TChannelStatus
    */
    enum {KChannelStatusMaxSize = 2};
    
    /** The channel status returned in the channel status event envelope when
    channel status event is part of the current event list (as set up in the
    SET UP EVENT LIST proactive command).
     
    For UICC Server Mode, this event can indicate a state change in a 
    TCP connection, for example, TCP in LISTEN state, TCP in CLOSED state, TCP
    in ESTABLISHED state (for details of specific encoding see ETSI TS 102 223 
    release 7 onwards).   
    */
    typedef TBuf8<KChannelStatusMaxSize> TChannelStatus; 

    /** A packaged RSat::TChannelStatus. */
    typedef TPckg<TChannelStatus>        TChannelStatusPckg;


    class TMoSmControlV1 : public TSatBase
    /**
    Describes details of the modified (by SIM MO SM Control) SMS.
    */
        {
    public:
        IMPORT_C TMoSmControlV1();
    public:
        TAddress iRpAddress;    //< Rp address
        TAddress iTpAddress;    //< Tp address
        TControlResult iResult; //< Call control procedure result
        TAlphaId iAlphaId;      //< Alpha identifier that may contain text to display to the user.
        };

    /** A packaged RSat::TMoSmControlV1. */
    typedef TPckg<TMoSmControlV1>   TMoSmControlV1Pckg;

    IMPORT_C void NotifyMoSmControlRequest(TRequestStatus& aReqStatus,TDes8& aMoSmDetails) const;

    //
    //  TIMER EXPIRATION
    //

    enum TTimerId
    /** Identifies a timer.  It is part of both the  TIMER MANAGEMENT proactive
    command and the Timer Expiration envelope command.
    @see RSat::TTimerExpirationV3
    @see RSat::TTimerMgmtV3 */
        {
        KTimer1 =0x01,       //< Timer 1 identifier.
        KTimer2 =0x02,       //< Timer 2 identifier.
        KTimer3 =0x03,       //< Timer 3 identifier.
        KTimer4 =0x04,       //< Timer 4 identifier.
        KTimer5 =0x05,       //< Timer 5 identifier.
        KTimer6 =0x06,       //< Timer 6 identifier.
        KTimer7 =0x07,       //< Timer 7 identifier.
        KTimer8 =0x08,       //< Timer 8 identifier.
        KTimerNotFound =0xFF //< Timer does not exist.
        };

    class TTimerExpirationV3 : public TSatBase
    /**
    Contains details of an expired timer.

    @see RSat::NotifyTimerExpiration()
    @see NotifyTimerMgmtPCmd()
    */
        {
    public:
        IMPORT_C TTimerExpirationV3();
    public:
        /**  The ID of the timer that has expired. */
        TTimerId    iTimerId;

        /** Timing difference between when you start the timer and when you
        send notification that the timer has expired. This should be
        as close as possible to the value of the timer given in the initial
        TIMER MANAGEMENT command. */
        TDuration   iTimerValue;
        };

    /** A packaged RSat::TTimerExpirationV3. */
    typedef TPckg<TTimerExpirationV3>   TTimerExpirationV3Pckg;

    IMPORT_C void NotifyTimerExpiration(TRequestStatus& aReqStatus, TDes8& aTimerExpiration) const;

    //
    //  SMS-PP Data download
    //

    class TSmsPpDownloadV3 : public TSatBase
    /**
    Point to Point data download over SMS.
    */
        {
    public:
        IMPORT_C TSmsPpDownloadV3();
    public:
        TDeviceId iDeviceId;        //< Device Identification
        TAddress iRpAddress;        //< RP Originating Address of the Service Center to which the Client is proposing to send the Short Message.
        TTpdu iPdu;                 //< Pdu to be sent
        };

    /** A packaged RSat::TSmsPpDownloadV3. */
    typedef TPckg<TSmsPpDownloadV3> TSmsPpDownloadV3Pckg;

    IMPORT_C void NotifySmsPpDownload(TRequestStatus& aReqStatus, TDes8& aSmsPpDownload) const;

    //
    // Cell Broadcast (CB) data download
    //

	/**
	@see RSat::TCbPage
	*/
    enum {KCbPageSize  = 88};
    /**
    Cell broadcast (CB) page.
    */
    typedef TBuf8<KCbPageSize> TCbPage;

    class TCbDownloadV3 : public TSatBase
    /**
    Cell Broadcast data download.
    */
        {
    public:
        IMPORT_C TCbDownloadV3();
    public:
        TDeviceId iDeviceId;    //< Device Identification
        TCbPage iPdu;           //< Pdu of the Cell Broadcast
        };

    /** A packaged RSat::TCbDownloadV3. */
    typedef TPckg<TCbDownloadV3> TCbDownloadV3Pckg;

    IMPORT_C void NotifyCbDownload(TRequestStatus& aReqStatus, TDes8& aCbDownload) const;


    //
    // EVENT DOWNLOAD
    //

    class TEventDownloadBaseV2 : public TSatBase
    /**

    Base class for the Event Download envelope commands. Use when no additional
    information is passed for the event, eg. User Activity event.

    @see RSat::EventDownload()
    @see RSat::NotifySetUpEventListPCmd()
    */
        {
    public:
        IMPORT_C TEventDownloadBaseV2();
    public:
        /** Specifies whether the event has any associated data. Do not set
        this yourself - it is set automatically in the constructor of this class and any
        derived classes. */
        TBool iDataAvailable;
        };

    /** A packaged RSat::TEventDownloadBaseV2. */
    typedef TPckg<TEventDownloadBaseV2> TEventDownloadBaseV2Pckg;

    enum TEventList
    /** These flags form the list of events in a SET UP EVENT LIST
    proactive command.  They are also used in Event Download envelope commands
    to identify the event that you are reporting to the UICC.

    @see RSat::NotifySetUpEventListPCmd()
    @see RSat::EventDownload()
    */
        {
        KMTCall                 	=0x00001, //< MT call
        KCallConnected          	=0x00002, //< Call connected
        KCallDisconnected       	=0x00004, //< Call disconnected
        KLocationStatus         	=0x00008, //< Location status
        KUserActivity           	=0x00010, //< User activity
        KIdleScreenAvailable    	=0x00020, //< Idle screen available
        KCardReaderStatus       	=0x00040, //< Card reader status
        KLanguageSelection      	=0x00080, //< Language selection
        KBrowserTermination     	=0x00100, //< Browser termination
        KDataAvailable          	=0x00200, //< Data available
        KChannelStatus          	=0x00400, //< Channel status
        KAccessTechnologyChange 	=0x00800, //< Access Technology Change
        KDisplayParamsChanges   	=0x01000, //< Display parameters changed
        KLocalConnection        	=0x02000, //< Local connection
 		KNetworkSearchModeChange	=0x04000, //< Network Search Mode Change
		KBrowsingStatusChange		=0x08000, //< Browsing Status Change
		KFramesInformationChange	=0x10000  //< Frames Information Change
        };

    class TCardReaderStatusEventV2 : public TEventDownloadBaseV2
    /**
    To inform the UICC that a "Card Reader Status" occurred, call
    RSat::EventDownload(), passing it the new status in a
    RSat::TCardReaderStatusEventV2Pckg, a packaged
    RSat::TCardReaderStatusEventV2.

    @see RSat::NotifySetUpEventListPCmd()
    @see RSat::EventDownload()
    */
        {
    public:
        IMPORT_C TCardReaderStatusEventV2();
    public:
        TUint8 iCardReaderStatus; //< The "Card Reader Status" object defined in ETSI TS 102 223's object definitions.
        };

    /** A packaged RSat::TCardReaderStatusEventV2. */
    typedef TPckg<TCardReaderStatusEventV2> TCardReaderStatusEventV2Pckg;


    class TLanguageSelectionEventV2 : public TEventDownloadBaseV2
    /**
    To inform the UICC that a "Language Selection Event" occurred, call
    RSat::EventDownload(), passing it the new language in a
    RSat::TLanguageSelectionEventV2Pckg, a packaged
    RSat::TLanguageSelectionEventV2

    @see RSat::NotifySetUpEventListPCmd()
    @see RSat::EventDownload()
    */
        {
    public:
        IMPORT_C TLanguageSelectionEventV2();
    public:

        /** Currently used language by UICC.

        Consists of 2 bytes. Each language code is a pair of
        alpha numeric characters, defined in ISO 639. Each
        alpha numeric character is coded on one byte using
        the SMS default 7-bit coded alphabet as defined in
        ISO 639 (1988): "Code for the representation of
        names of languages". */
        TUint16 iLanguage;
        };

    /** A packaged RSat::TLanguageSelectionEventV2. */
    typedef TPckg<TLanguageSelectionEventV2> TLanguageSelectionEventV2Pckg;

    enum TBrowserTerminationCause
    /** Describes the reason that the browser terminated.  This for use with RSat::TBrowserTerminationEventV2. */
        {
        EUnknownCause,     //< The browser termination cause is unknown.
        EUserTermination,  //< The browser was terminated by the user.
        EErrorTermination  //< The browser was terminated by an error.
        };


    class TBrowserTerminationEventV2 : public TEventDownloadBaseV2
    /**
    To inform the UICC that a "Browser Termination Event" occurred, call
    RSat::EventDownload(), passing it the cause of termination in a
    RSat::TBrowserTerminationEventV2Pckg, a packaged
    RSat::TBrowserTerminationEventV2.  This contains a
    RSat::TBrowserTerminationCause that describes the reason that the browser
    terminated.

    @see RSat::NotifySetUpEventListPCmd()
    @see RSat::EventDownload()
    */
        {
    public:
        IMPORT_C TBrowserTerminationEventV2();
    public:
        TBrowserTerminationCause iCause; //< Cause of browser termination
        };

    /** A packaged RSat::TBrowserTerminationEventV2. */
    typedef TPckg<TBrowserTerminationEventV2>   TBrowserTerminationEventV2Pckg;


    class TDataAvailableEventV2 : public TEventDownloadBaseV2
    /**
    To inform the UICC that a "Data Available Event" occurred, call
    RSat::EventDownload(), passing it the new status and data length in a
    RSat::TDataAvailableEventV2Pckg, a packaged RSat::TDataAvailableEventV2.
    Fill in the status of the channel that received the data and the length
    of data received.

    @see RSat::NotifySetUpEventListPCmd()
    @see RSat::EventDownload()
    */
        {
    public:
        IMPORT_C TDataAvailableEventV2();
    public:
        TChannelStatus  iStatus;    //< Status of the channel.
        TInt8           iLength;    //< Channel data length. Return 0xFF if more than 255 bytes are available.
        };

    /** A packaged RSat::TDataAvailableEventV2. */
    typedef TPckg<TDataAvailableEventV2>    TDataAvailableEventV2Pckg;


    class TChannelStatusEventV2 : public TEventDownloadBaseV2
    /**
    To inform the UICC that a "Channel Status Event" occurred, call
    RSat::EventDownload(), passing it the new status in a
    RSat::TChannelStatusEventV2Pckg, a packaged RSat::TChannelStatusEventV2.
    Fill in the new status of the channel.

    @see RSat::NotifySetUpEventListPCmd()
    @see RSat::EventDownload()
    */
        {
    public:
        IMPORT_C TChannelStatusEventV2();
    public:
        TChannelStatus iStatus; //< Status of the channel
        };

    /** A packaged RSat::TChannelStatusEventV2. */
    typedef TPckg<TChannelStatusEventV2>    TChannelStatusEventV2Pckg;


    class TDisplayParamsEventV2 : public TEventDownloadBaseV2
    /**
    To inform the UICC that a "Display Parameters Changed Event" occurred, call
    RSat::EventDownload(), passing it the new parameters in a
    RSat::TDisplayParamsEventV2Pckg, a packaged RSat::TDisplayParamsEventV2.
    Fill in the new display parameters.

    @see RSat::NotifySetUpEventListPCmd()
    @see RSat::EventDownload()
    */
        {
    public:
        IMPORT_C TDisplayParamsEventV2();
    public:
        TUint8 iSatProfileByte14;   //< Byte 14 of the (U)SAT profile
        TUint8 iScreenHeight;       //< Screen height
        TUint8 iSatProfileByte15;   //< Byte 15 of the (U)SAT profile
        TUint8 iScreenWidth;        //< Screen width
        TUint8 iSatProfileByte16;   //< Byte 16 of the (U)SAT profile
        TUint8 iWidthReduction;     //< Width reduction
        };

    /** A packaged RSat::TDisplayParamsEventV2. */
    typedef TPckg<TDisplayParamsEventV2>    TDisplayParamsEventV2Pckg;

    enum TLocalBearerId
    /** Identifies the physical method of transferring data. */
        {
        ELocalBearerIdNotSet,     //< Initialisation value.
        ELocalBearerIdNotPresent, //< Bearer not present.
        ETechnologyIndependent,   //< Technology independent.
        ELocalBearerBT,           //< Currently only valid if the local bearer ID is Bluetooth.
        ELocalBearerIrda,         //< IrDA (infra-red).
        ELocalBearerRs232,        //< RS232 (serial).
        ELocalBearerUsb           //< USB.
        };

	/**
	@see RSat::TServiceRecord
	*/
    enum {KServiceRecordMaxSize=244};
    typedef TBuf8<KServiceRecordMaxSize> TServiceRecord;

    /** Service record information. */
    struct TServiceRecordInfo
        {
        TLocalBearerId iBearerId;   //< Bearer Identifier
        TUint8 iServiceId;          //< Service Identifier
        TServiceRecord iRecord;     //< Service Record
        };

    /** The coding of the remote entity's address.
    @see RSat::TRemoteEntityAddress */
    enum TRemoteAddressCoding
        {
        ECodingNotSet, //< Initialisation value.
        E48BitAddress //< 48 bit address.
        };

	/**
	@see RSat::TRemoteAddress
	*/
    enum {KRemoteAddressMaxSize=237};

    /** Descriptor containing a remote entity's address.
    Maximum size is 237.
    @see RSat::TRemoteEntityAddress */
    typedef TBuf8<KRemoteAddressMaxSize> TRemoteAddress;

    /** Remote entity address for local connections.  It has two uses:

    - An OPEN CHANNEL proactive command requests that you open a channel.  The
    command specifies the type of channel.  If it requests a local connection
    then the command will contain a TRemoteEntityAddress that describes the
    connection's remote enitity; see RSat::TOpenLocalLinksChannelV2.

    - Also, when you inform the UICC that a local connection has been created,
    tell the UICC about the connections remote entity in a TRemoteEntityAddress;
    see RSat::TLocalConnectionEventV2.
    */
    struct TRemoteEntityAddress
        {
        TRemoteAddressCoding iCoding; //< The coding of the remote entity's address.
        TRemoteAddress iAddress;      //< Descriptor containing the remote entity's address.
        };


    class TLocalConnectionEventV2 : public TEventDownloadBaseV2
    /**
    To inform the UICC that a "Local Connection Event" occurred, call
    RSat::EventDownload(), passing it the new connection information in a
    RSat::TLocalConnectionEventV2Pckg, a packaged
    RSat::TLocalConnectionEventV2.  Fill in the address of the new device.

    @see RSat::NotifySetUpEventListPCmd()
    @see RSat::EventDownload()
    */
        {
    public:
        IMPORT_C TLocalConnectionEventV2();
    public:
        TServiceRecordInfo iRecord;     //< Service record
        TRemoteEntityAddress iAddress;  //< Remote entity address
        };

    /** A packaged RSat::TLocalConnectionEventV2. */
    typedef TPckg<TLocalConnectionEventV2>  TLocalConnectionEventV2Pckg;

	enum TNetworkSearchMode
		{
		EUnknown, //< Unknown Network type 
		EManual,  //< Manual Network mode
		EAutomatic //< Automatic Network Mode			
		};
		
    class TNetworkSearchModeChangeEventV6 : public TEventDownloadBaseV2
    /**
    To inform the UICC that a "Network Search Mode ChangeEvent" occurred, call
    RSat::EventDownload(), passing it the new NetworkSearchMode Change information in a
    RSat::TNetworkSearchModeChangeEventV6Pckg, a packaged
    RSat::TNetworkSearchModeChangeEventV6.  Fill in the details of type of Network.

    @see RSat::NotifySetUpEventListPCmd()
    @see RSat::EventDownload()
    */
        {
    public:
        IMPORT_C TNetworkSearchModeChangeEventV6();
    public:
        TNetworkSearchMode iNetworkSearchMode; //< Network type selection
        };

    /** A packaged RSat::TNetworkSearchModeChangeEventV6. */
    typedef TPckg<TNetworkSearchModeChangeEventV6>  TNetworkSearchModeChangeEventV6Pckg;
    
    class TBrowsingStatusEventV6 : public TEventDownloadBaseV2
    /**
    To inform the UICC that a "Browsing Status Event" occurred, call
    RSat::EventDownload(), passing it the Browsing Status information in a
    RSat::TBrowsingStatusEventV6Pckg, a packaged
    RSat::TBrowsingStatusEventV6.  Fill in the Browsing status.

    @see RSat::NotifySetUpEventListPCmd()
    @see RSat::EventDownload()
    */
        {
    public:
        IMPORT_C TBrowsingStatusEventV6();
    public:
		TUint8 iBrowserStatus; //< Browsing status information coded as per 3GPP 31.111
        };

    /** A packaged RSat::TBrowsingStatusEventV6. */
    typedef TPckg<TBrowsingStatusEventV6>  TBrowsingStatusEventV6Pckg;
    
    /** 
    @see RSat::TFrameList
    */
   	enum {KMaxFrameNumber = 254};
   	
   	/** 
   	@see RSat::TFramesInformation 
  	*/
	typedef TBuf8<KMaxFrameNumber> TFrameList;
	
	/**
	This structure contains the "Frames information". 
	refer to ETSI TS 102 223 for more details.
	@see RSat::TFrameList
	*/
	struct TFramesInformation 
		{
		TUint8  iFrameId; //< Each frame has an ID. This field contains the ID of the frame. 
		TFrameList iFrameList; //< It holds the frame information.
		};
		
    class TFramesInformationChangedEventV6 : public TEventDownloadBaseV2
    /**
    To inform the UICC that a "Frames Information Change" occurred, call
    RSat::EventDownload(), passing it the Frames information in a
    RSat::TFramesInformationChangedEventV6Pckg, a packaged
    RSat::TFramesInformationChangedEventV6.  Fill in the frames information.
    @see RSat::TFramesInformation
    @see RSat::NotifySetUpEventListPCmd()
    @see RSat::EventDownload()
    */
        {
    public:
        IMPORT_C TFramesInformationChangedEventV6();
    public:
		TFramesInformation iFramesInformation;
        };

    /** A packaged RSat::TBrowsingStatusEventV6. */
    typedef TPckg<TFramesInformationChangedEventV6>  TFramesInformationChangedEventV6Pckg;
    
    IMPORT_C void EventDownload(TRequestStatus& aReqStatus, TEventList aSingleEvent, const TDesC8& aEventInfo) const;

    //
    // DISPLAY TEXT
    //
    enum TClearScreenTrigger
    /** Part of a DISPLAY TEXT proactive command.  After the text has been
    displayed, this determines the events that clear the text or icon.
    @see RSat::TDisplayTextV1
    @see RSat::NotifyDisplayTextPCmd()
    @see RSat::TDisplayMultimediaMessageV6
    @see RSat::NotifyDisplayMultimediaMsgPCmd()*/
        {
        EClearScreenTriggerNotSet,  //< Clear screen trigger not set

        /** Clear screen after a specified delay. The UICC may specify the
        delay in RSat::TDisplayTextV2::iDuration.  Otherwise the delay is at
        your discretion. */
        EClearAfterDelay,

        EUserClear                  //< Clear screen on user action.
        };

    enum TDisplayPriority
    /** Part of a DISPLAY TEXT proactive command.  This determines the text's priority.
    @see RSat::TDisplayTextV1
    @see RSat::NotifyDisplayTextPCmd()
    @see RSat::TDisplayMultimediaMessageV6
    @see RSat::NotifyDisplayMultimediaMsgPCmd()*/
        {
        EDisplayPriorityNotSet, //< Display priority not set
        ENormalPriority,        //< Normal display priority
        EHighPriority           //< High display priority
        };

    enum TImmediateRsp
    /** Part of a DISPLAY TEXT proactive command.  This determines your response to the UICC.
    @see RSat::TDisplayTextV1
    @see RSat::NotifyDisplayTextPCmd()
    @see RSat::TDisplayMultimediaMessageV6
    @see RSat::NotifyDisplayMultimediaMsgPCmd()*/
        {
        EImmediateRspNotSet,    //< Immediate response param not set
        EImmediateRsp,          //< Immediate response requested
        ENoImmediateRsp         //< Immediate response not requested
        };

	/**
	@see RSat::TTextToDisplay
	*/
    enum {KTextToDisplayMaxSize=242};

    /** Part of a DISPLAY TEXT proactive command.  This contains the text to display.
    @see RSat::TDisplayTextV1
    @see RSat::NotifyDisplayTextPCmd()
    @see RSat::NotifyDisplayMultimediaMsgPCmd()*/
    typedef TBuf<KTextToDisplayMaxSize> TTextToDisplay;

    class TDisplayTextV1 : public TSatPCmdBase
    /**
    Contains a DISPLAY TEXT proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifyDisplayTextPCmd() for more information about
    the command.

    Request notification for DISPLAY TEXT with RSat::NotifyDisplayTextPCmd(),
    passing it an object in which to store the command when it arrives.  Pass
    RSat::TDisplayTextV1Pckg (a packaged RSat::TDisplayTextV1) when using v6.1
    and v7.0 of Symbian OS.  For later versions of Symbian OS, use
    RSat::TDisplayTextV2, RSat::TDisplayV3 etc.

    @see RSat::TDisplayTextV2
    @see RSat::TDisplayTextV1Pckg
    @see RSat::NotifyDisplayTextPCmd()
    */
        {
    public:
        IMPORT_C TDisplayTextV1();
    public:
        TDisplayPriority iPriority;                 //< Priority of icon/text

        /** Determines whether the text should be removed from the screen
        after a delay or after user action. */
        TClearScreenTrigger iClearScreenTrigger;
        TTextToDisplay iText;                       //< Text to display
        TIconId iIconId;                            //< Icon to display

        /** Determines whether you should respond to the UICC straight away or
        when the text/icon is removed from the screen. */
        TImmediateRsp iImmediateRsp;
        };

    /** A packaged RSat::TDisplayTextV1. */
    typedef TPckg<TDisplayTextV1>  TDisplayTextV1Pckg;


    class TDisplayTextV2 : public TDisplayTextV1
    /**
    Contains a DISPLAY TEXT proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifyDisplayTextPCmd() for more information about
    the command.

    Request notification for DISPLAY TEXT with RSat::NotifyDisplayTextPCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TDisplayText" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TDisplayTextV2Pckg, a packaged RSat::TDisplayTextV2.
    Use RSat::TDisplayTextV1 with older versions of Symbian OS.  For later
    versions of Symbian OS, check for an updated API (RSat::TDisplayTextV3
    etc).

    @see RSat::TDisplayTextV1
    @see RSat::TDisplayTextV2Pckg
    @see RSat::NotifyDisplayTextPCmd()
    */
        {
    public:
        IMPORT_C TDisplayTextV2();
    public:

        /** Required duration to display the text.  Only applicable if
        RSat::TDisplayTextV1::iClearScreenTrigger is set to
        RSat::EClearAfterDelay. */
        TDuration iDuration;
        };

    /** A packaged RSat::TDisplayTextV2. */
    typedef TPckg<TDisplayTextV2>  TDisplayTextV2Pckg;

    class TDisplayTextV5 : public TDisplayTextV2
	/**
	Contains a DISPLAY TEXT proactive command issued by the UICC.  See ETSI TS
	102 223 and RSat::NotifyDisplayTextPCmd() for more information about
	the command.

	Request notification for DISPLAY TEXT with RSat::NotifyDisplayTextPCmd(),
	passing it an object in which to store the command when it arrives.	Pass a
	packaged version of the latest "TDisplayText" class. For Symbian OS 7.0s,
	8.0 and 8.1 use RSat::TDisplayTextV2Pckg, a packaged RSat::TDisplayTextV2.
	For Symbian OS 9.2, use RSat::TDisplayTextV5Pckg, a packaged
	RSat::TDisplayTextV5. For later versions of
	Symbian OS, check for an updated API (RSat::TDisplayTextV6 etc).
	Use RSat::TDisplayTextV1 with older versions of Symbian OS.

	@see RSat::TDisplayTextV1
	@see RSat::TDisplayTextV2
	@see RSat::TDisplayTextV5Pckg
	@see RSat::NotifyDisplayTextPCmd()
	*/
		{
	public:
		IMPORT_C TDisplayTextV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TDisplayTextV1::iText is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TDisplayTextV5. */
	typedef TPckg<TDisplayTextV5>  TDisplayTextV5Pckg;

    class TDisplayTextV6 : public TDisplayTextV5
	/**
	Contains a DISPLAY TEXT proactive command issued by the UICC.  See ETSI TS
	102 223 and RSat::NotifyDisplayTextPCmd() for more information about
	the command.

	Request notification for DISPLAY TEXT with RSat::NotifyDisplayTextPCmd(),
	passing it an object in which to store the command when it arrives.	Pass a
	packaged version of the latest "TDisplayText" class. For Symbian OS 7.0s,
	8.0 and 8.1 use RSat::TDisplayTextV2Pckg, a packaged RSat::TDisplayTextV2.
	For Symbian OS 9.2, use RSat::TDisplayTextV5Pckg, a packaged
	RSat::TDisplayTextV5.For Symbian OS 9.4, use RSat::TDisplayTextV6Pckg, a packaged
	RSat::TDisplayTextV6. For later versions of
	Symbian OS, check for an updated API (RSat::TDisplayTextV7 etc).

	@see RSat::TDisplayTextV1
	@see RSat::TDisplayTextV2
	@see RSat::TDisplayTextV5
	@see RSat::TDisplayTextV6Pckg
	@see RSat::NotifyDisplayTextPCmd()
	*/
		{
	public:
		IMPORT_C TDisplayTextV6();

	public: 
		TUint8 iFrameId;  //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TDisplayTextV6. */
	typedef TPckg<TDisplayTextV6>  TDisplayTextV6Pckg;
    
    IMPORT_C void NotifyDisplayTextPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TDisplayTextRspV1 : public TSatPCmdBase
    /**
    Contains your response to a DISPLAY TEXT proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyDisplayTextPCmd() for more information about
    the command.

    For Symbian OS versions up to and including  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TDisplayTextRspV1Pckg (a packaged
    TDisplayTextRspV1). For later versions of Symbian OS, check for an updated
    API (RSat::TDisplayTextRspV2 etc).

    @see RSat::NotifyDisplayTextPCmd()
    */
        {
    public:
        IMPORT_C TDisplayTextRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TDisplayTextRspV1. */
    typedef TPckg<TDisplayTextRspV1>  TDisplayTextRspV1Pckg;

    //
    //GET INKEY
    //
    /** Part of the GET INKEY proactive command.
    This tells you what response to get from the user.
    @see RSat::TGetInkeyV1
    @see RSat::NotifyGetInkeyPCmd()*/
    enum TGetInkeyRspFormat
        {
        EGetInkeyRspFormatNotSet,   //< GET INKEY response format not set.
        EDigitOnly,                 //< Get a digit (0-9, *, #, and +) from the user.
        ECharSmsDefaultAlphabet,    //< Get a character from the SMS default alphabet from the user.
        ECharUcs2Alphabet,          //< Get a character from the UCS2 alphabet from the user.
        EYesNo                      //< Get a Yes or No decision from the user.
        };

    enum {KGetInkeyTextMaxSize=242}; //< @see RSat::TGetInkeyText

    /** Part of the GET INKEY proactive command.  Contains the text to display
    to the user before getting a response.  Maximum length is 242 characters.
    @see RSat::TGetInkeyV1
    @see RSat::NotifyGetInkeyPCmd()*/
    typedef TBuf<KGetInkeyTextMaxSize> TGetInkeyText;

    class TGetInkeyV1 : public TSatPCmdBase
    /**
    Contains a GET INKEY proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifyGetInkeyPCmd() for more information about
    the command.

    Request notification for GET INKEY with RSat::NotifyGetInkeyPCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TGetInkey" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TGetInkeyV2Pckg, a packaged RSat::TGetInkeyV2.
    Use RSat::TGetInkeyV1 with older versions of Symbian OS.  For later
    versions of Symbian OS, check for an updated API (RSat::TGetInkeyV3
    etc).

    @see RSat::TGetInkeyV2
    @see RSat::TGetInkeyV1Pckg
    @see RSat::NotifyGetInkeyPCmd()
    */
        {
    public:
        IMPORT_C TGetInkeyV1();
    public:
        /** Response required. */
        TGetInkeyRspFormat iRspFormat;

        /** Text to display. */
        TGetInkeyText iText;

        /** Icon to display. */
        TIconId iIconId;

        /** Determines whether help is available. */
        THelp iHelp;
        };

    /** A packaged RSat::TGetInkeyV1. */
    typedef TPckg<TGetInkeyV1>  TGetInkeyV1Pckg;

    enum TGetInkeyRspMode
    /** Part of the GET INKEY proactive command.  Determines how you get the
    response from the user.
    @see RSat::TGetInkeyV1
    @see RSat::NotifyGetInkeyPCmd()*/
        {
        EGetInkeyRspModeNotSet, //< Initialisation value.

        /** Display character that the user chooses.  You may allow the user to
        alter and/or confirm their choice before passing the character back to
        the UICC. */
        EUserRspDisplayed,

        /** An immediate response is requested. The response must be a digit (0-
        9, *, #, and +). Do not display the digit, and do not allow the user to
        alter or confirm their choice before returning the digit to the UICC. */
        EImmediateDigitRsp
        };


    class TGetInkeyV2 : public TGetInkeyV1
    /**
    Contains a GET INKEY proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifyGetInkeyPCmd() for more information about
    the command.

    Request notification for GET INKEY with RSat::NotifyGetInkeyPCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TGetInkey" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TGetInkeyV2Pckg, a packaged RSat::TGetInkeyV2.
    Use RSat::TGetInkeyV1 with older versions of Symbian OS.  For later
    versions of Symbian OS, check for an updated API (RSat::TGetInkeyV3
    etc).

    @see RSat::TGetInkeyV2
    @see RSat::TGetInkeyV2Pckg
    @see RSat::NotifyGetInkeyPCmd()
    */
        {
    public:
        IMPORT_C TGetInkeyV2();
    public:
        /** Wait for this length of time for a response from the user.
        ETSI TS 102 223 describes what to do when the time expires. */
        TDuration iDuration;

        /**  Determines whether you let the user alter/confirm their choice of character. */
        TGetInkeyRspMode iMode;
        };

    /** A packaged RSat::TGetInkeyV2. */
    typedef TPckg<TGetInkeyV2>  TGetInkeyV2Pckg;

	class TGetInkeyV5 : public TGetInkeyV2
	/**
	Contains a GET INKEY proactive command issued by the UICC.   See ETSI TS
	102 223 and RSat::NotifyGetInkeyPCmd() for more information about
	the command.

	Request notification for GET INKEY with RSat::NotifyGetInkeyPCmd(),
	passing it an object in which to store the command when it arrives.  Pass a
	packaged version of the latest "TGetInkey" class. For Symbian OS 7.0s,
	8.0 and 8.1 use RSat::TGetInkeyV2Pckg, a packaged RSat::TGetInkeyV2.
	Use RSat::TGetInkeyV1 with older versions of Symbian OS.
	For Symbian OS 9.2, use RSat::TGetInkeyV5Pckg.
	For later versions of Symbian OS, check for an updated API (RSat::TGetInkeyV6 etc).

	@see RSat::TGetInkeyV1
	@see RSat::TGetInkeyV2
	@see RSat::TGetInkeyV5Pckg
	@see RSat::NotifyGetInkeyPCmd()
	*/
		{
	public:
		IMPORT_C TGetInkeyV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TGetInkeyV1::iText is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TGetInkeyV5. */
	typedef TPckg<TGetInkeyV5>  TGetInkeyV5Pckg;

    class TGetInkeyV6 : public TGetInkeyV5
	/**
	Contains a GET INKEY proactive command issued by the UICC.   See ETSI TS
	102 223 and RSat::NotifyGetInkeyPCmd() for more information about
	the command.

	Request notification for GET INKEY with RSat::NotifyGetInkeyPCmd(),
	passing it an object in which to store the command when it arrives.  Pass a
	packaged version of the latest "TGetInkey" class. For Symbian OS 7.0s,
	8.0 and 8.1 use RSat::TGetInkeyV2Pckg, a packaged RSat::TGetInkeyV2.
	Use RSat::TGetInkeyV1 with older versions of Symbian OS.
	For Symbian OS 9.2, use RSat::TGetInkeyV5Pckg.
	For Symbian OS 9.4 and later, use RSat::TGetInkeyV6Pckg.
	For later versions of Symbian OS, check for an updated API (RSat::TGetInkeyV7 etc).

	@see RSat::TGetInkeyV1
	@see RSat::TGetInkeyV2
	@see RSat::TGetInkeyV5
	@see RSat::TGetInkeyV6Pckg
	@see RSat::NotifyGetInkeyPCmd()
	*/
		{
	public:
		IMPORT_C TGetInkeyV6();

	public:
		TUint8 iFrameId;  //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TGetInkeyV5. */
	typedef TPckg<TGetInkeyV6>  TGetInkeyV6Pckg;

    IMPORT_C void NotifyGetInkeyPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TGetInkeyRspV1 : public TSatPCmdBase
    /**
    Contains your response to a GET INKEY proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyGetInkeyPCmd() for more information about
    the command.

    Return the key press in iAdditionalInfo, plus the other information as
    indicated.

    For Symbian OS v6.1 or v7.0 pass RSat::TerminalRsp() a
    RSat::TGetInkeyRspV1Pckg (a packaged TGetInkeyRspV1).  For versions up to
    and including v8.1a and v8.1b use RSat::TGetInkeyRspV2Pckg (a packaged
    TGetInkeyRspV2).   For later versions of Symbian OS, check for an updated
    API (RSat::TGetInkeyRspV3 etc).

    @see RSat::TGetInkeyRspV2
    @see RSat::NotifyGetInkeyPCmd()
    */
        {
    public:
        IMPORT_C TGetInkeyRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** Set to RSat::KTextString to return a key press.  Some error values
        of iGeneralResult require additional information, in which case set
        this to RSat::KMeProblem.  Otherwise set to RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** The format of the key press, as given in
        RSat::TGetInkeyV1::iRspFormat. */
        TGetInkeyRspFormat iRspFormat;

        /** The key press. */
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TGetInkeyRspV1. */
    typedef TPckg<TGetInkeyRspV1>  TGetInkeyRspV1Pckg;


    class TGetInkeyRspV2 : public TGetInkeyRspV1
    /**
    Contains your response to a GET INKEY proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyGetInkeyPCmd() for more information about
    the command.

    Return the values as described in RSat::TGetInkeyRspV1, plus the total
    display duration.

    For Symbian OS v6.1 or v7.0 pass RSat::TerminalRsp() a
    RSat::TGetInkeyRspV1Pckg (a packaged TGetInkeyRspV1).  For versions up to
    and including v8.1a and v8.1b use RSat::TGetInkeyRspV2Pckg (a packaged
    TGetInkeyRspV2).   For later versions of Symbian OS, check for an updated
    API (RSat::TGetInkeyRspV3 etc).

    @see RSat::TGetInkeyRspV1
    @see RSat::NotifyGetInkeyPCmd()
    */
        {
    public:
        IMPORT_C TGetInkeyRspV2();
    public:
        TDuration iDuration; //< Total display duration.
        };

    /** A packaged RSat::TGetInkeyRspV2. */
    typedef TPckg<TGetInkeyRspV2>  TGetInkeyRspV2Pckg;

    //
    //GET INPUT
    //
    struct TRspLength
    /** Part of the GET INPUT proactive command.
    Determines the maximum and minimum length of string you return to the UICC.
    @see RSat::TGetInputV1
    @see RSat::TGetInputV5
    @see RSat::NotifyGetInputPCmd()*/
        {
        TUint8 iMinRspLength;   //< Minimum length of string you return to the UICC.
        TUint8 iMaxRspLength;   //< Maximum length of string you return to the UICC.
        };

    enum TInputDisplayOption
    /** Part of the GET INPUT proactive command.
    Deetermine whether you display the user's string while they are entering it.
    @see RSat::TGetInputV1
    @see RSat::TGetInputV5
    @see RSat::NotifyGetInputPCmd()*/
        {
        EInputDisplayOptionNotSet, //< Value used in constructor.
        EDisplayUserInput,         //< Display user's input while they type.
        EHideUserInput             //< Hide user's input while they type.
        };

    enum TGetInputRspFormat
    /** Part of the GET INPUT proactive command.
    Determines the format of the string returned to the UICC.
    @see RSat::TGetInputV1
    @see RSat::TGetInputV5
    @see RSat::NotifyGetInputPCmd()*/
        {
        EGetInputRspFormatNotSet,    //< Value used in constructor.
        EDigitOnlyUnpacked,          //< Digits only, unpacked.
        EDigitOnlyPacked,            //< Digits only, packed.
        ESmsDefaultAlphabetUnpacked, //< String in SMS default alphabet, unpacked.
        ESmsDefaultAlphabetPacked,   //< String in SMS default alphabet, packed.
        EUcs2Alphabet                //< String in UCS2 alphabet
        };

	/**
	@see RSat::TGetInputText
	*/
    enum {KGetInputTextMaxSize=238};

    /** Part of the GET INPUT proactive command. Contains the text to display
    to the user before getting a response.  Maximum length is 238 characters.
    @see RSat::TGetInputV1
    @see RSat::TGetInputV5
    @see RSat::NotifyGetInputPCmd()*/
    typedef TBuf<KGetInputTextMaxSize> TGetInputText;

	/**
	@see RSat::TGetInputDefaultText
	*/
    enum {KGetInputDefaultTextMaxSize=235};

    /** Part of the GET INPUT proactive command. A default string: allow the
    user to edit this, or to delete it and replace it with their own text.
    @see RSat::TGetInputV1
    @see RSat::NotifyGetInputPCmd()*/
    typedef TBuf<KGetInputDefaultTextMaxSize> TGetInputDefaultText;

    class TGetInputV1 : public TSatPCmdBase
    /**
    Contains a GET INPUT proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyGetInputPCmd() for more information about
    the command.

    Request notification for GET INPUT with RSat::NotifyGetInputPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TGetInput" class. For Symbian OS versions
    up to and including v8.1a and 8.1b use RSat::TGetInputV1Pckg, a packaged
    RSat::TGetInputV1.  For later versions of Symbian OS, check for an updated
    API (RSat::TGetInputV2 etc).

    @see RSat::TGetInputV1Pckg
    @see RSat::NotifyGetInputPCmd()
    */
        {
    public:
        IMPORT_C TGetInputV1();
    public:
        /** Determines whether the user can see the characters they enter. */
        TInputDisplayOption iInputDisplayOption;

        /** Determines whether digits or characters should be entered,
        and whether they should be in a packed or unpacked format. */
        TGetInputRspFormat iRspFormat;

        /** Text to display. */
        TGetInputText iText;

        /** The maximum and minimum length of string. */
        TRspLength iRspLength;

        /** A default string can be provided. This field determines whether the
        user is allowed to edit it, or whether they can only delete it and
        replace it with their own text. */
        TGetInputDefaultText iDefaultText;

        /** Icon to display. */
        TIconId iIconId;

        /** Determine whether help is available. */
        THelp iHelp;
        };

    /** A packaged RSat::TGetInputV1. */
    typedef TPckg<TGetInputV1>  TGetInputV1Pckg;

	class TGetInputV5 : public TGetInputV1
	/**
	Contains a GET INPUT proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifyGetInputPCmd() for more information about
	the command.

	Request notification for GET INPUT with RSat::NotifyGetInputPCmd(), passing
	it an object in which to store the command when it arrives.  Pass a
	packaged version of the  latest "TGetInput" class. For Symbian OS versions
	up to and including v8.1a and 8.1b use RSat::TGetInputV1Pckg, a packaged
	RSat::TGetInputV1. For Symbian OS v9.2, use RSat::TGetInputV5.
	For later versions of Symbian OS, check for an updated
	API (RSat::TGetInputV6 etc).

	@see RSat::TGetInputV1
	@see RSat::TGetInputV5Pckg
	@see RSat::NotifyGetInputPCmd()
	*/
		{
	public:
		IMPORT_C TGetInputV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TGetInputV1::iText is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TGetInputV5. */
	typedef TPckg<TGetInputV5>  TGetInputV5Pckg;

	class TGetInputV6 : public TGetInputV5
	/**
	Contains a GET INPUT proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifyGetInputPCmd() for more information about
	the command.

	Request notification for GET INPUT with RSat::NotifyGetInputPCmd(), passing
	it an object in which to store the command when it arrives.  Pass a
	packaged version of the  latest "TGetInput" class. For Symbian OS versions
	up to and including v8.1a and 8.1b use RSat::TGetInputV1Pckg, a packaged
	RSat::TGetInputV1. For Symbian OS v9.2, use RSat::TGetInputV5.
	For Symbian OS v9.4, use RSat::TGetInputV6.
	For later versions of Symbian OS, check for an updated
	API (RSat::TGetInputV7 etc).

	@see RSat::TGetInputV1
	@see RSat::TGetInputV5
	@see RSat::TGetInputV6Pckg
	@see RSat::NotifyGetInputPCmd()
	*/
		{
	public:
		IMPORT_C TGetInputV6();

	public:
		TUint8 iFrameId;  //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TGetInputV6. */
	typedef TPckg<TGetInputV6>  TGetInputV6Pckg;
	
    IMPORT_C void NotifyGetInputPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TGetInputRspV1 : public TSatPCmdBase
    /**
    Contains your response to a GET INPUT proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyGetInputPCmd() for more information about
    the command.

    Return the string in iAdditionalInfo, plus the other information as
    indicated.

    For Symbian OS versions up to and including  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TGetInputRspV1Pckg (a packaged TGetInputRspV1).
    For later versions of Symbian OS, check for an updated API
    (RSat::TGetInputRspV2 etc).

    @see RSat::NotifyGetInputPCmd()
    */
        {
    public:
        IMPORT_C TGetInputRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** Set to RSat::KTextString to return a string.  Some error values
        of iGeneralResult require additional information, in which case set
        this to RSat::KMeProblem.  Otherwise set to RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** The format of the key press, as given in
        RSat::TGetInputV1::iRspFormat. */
        TGetInputRspFormat iRspFormat;

        /** Additional information, such as the string. */
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TGetInputRspV1. */
    typedef TPckg<TGetInputRspV1>  TGetInputRspV1Pckg;

    //
    //PLAY TONE
    //

    enum TTone
    // TTone enum values are dictated by 3GPP 31.111
    /** Part of the PLAY TONE proactive command.
    This determines the type of tone to play.
    @see RSat::TPlayToneV1
    @see RSat::NotifyPlayTonePCmd() */
        {
        KToneNotSet                         =-1,   //< Initialization value used within constructor(s).
        KDialTone                           =0x01, //< Dial tone.
        KCalledSubscriberBusy               =0x02, //< Called subscriber busy.
        KCongestion                         =0x03, //< Congestion.
        KRadioPathAcknowledge               =0x04, //< Radio path acknowledge.
        KRadioPathNotAvailableCallDropped   =0x05, //< Radio path not available/Call dropped.
        KErrorSpecialInfo                   =0x06, //< Error/Special information.
        KCallWaitingTone                    =0x07, //< Call waiting tone.
        KRingingTone                        =0x08, //< Ringing tone.
        KGeneralBeep                        =0x10, //< General beep.
        KPositiveTone                       =0x11, //< Positive acknowledgement tone.
        KNegativeTone                       =0x12, //< Negative acknowledgement or error tone.
        KUserSelectedToneIncomingSpeech     =0x13, //< Ringing tone as selected by the user for incoming speech call.
        KUserSelectedToneIncomingSms        =0x14,  //< Alert tone as selected by the user for incoming SMS.
        KCriticalAlert						=0x15, //< Independent of volume setting in the terminal.
		KVibrateOnly						=0x20, //< Vibrate only, if available
		KHappyTone							=0x30, //< Themed tone
		KSadTone							=0x31, //< Themed tone
		KUrgentActionTone					=0x32, //< Themed tone
		KQuestionTone						=0x33, //< Themed tone
		KMessageReceivedTone				=0x34, //< Themed tone
		KMelody1							=0x40, //< The Melody tones are intended to allow the UICC to cause the terminal to play tunes.
		KMelody2							=0x41, //< The Melody tones are intended to allow the UICC to cause the terminal to play tunes.
		KMelody3							=0x42, //< The Melody tones are intended to allow the UICC to cause the terminal to play tunes.
		KMelody4							=0x43, //< The Melody tones are intended to allow the UICC to cause the terminal to play tunes.
		KMelody5							=0x44, //< The Melody tones are intended to allow the UICC to cause the terminal to play tunes.
		KMelody6							=0x45, //< The Melody tones are intended to allow the UICC to cause the terminal to play tunes.
		KMelody7							=0x46, //< The Melody tones are intended to allow the UICC to cause the terminal to play tunes.
		KMelody8							=0x47  //< The Melody tones are intended to allow the UICC to cause the terminal to play tunes.
        };


    class TPlayToneV1 : public TSatPCmdBase
    /**
    Contains a PLAY TONE proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyPlayTonePCmd() for more information about
    the command.

    Request notification for PLAY TONE with RSat::NotifyPlayTonePCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TPlayTone" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TPlayToneV2Pckg, a packaged RSat::TPlayToneV2.
    Use RSat::TPlayToneV1 with older versions of Symbian OS.  For later
    versions of Symbian OS, check for an updated API (RSat::TPlayToneV3
    etc).

    @see RSat::TPlayToneV2
    @see RSat::TPlayToneV1Pckg
    @see RSat::NotifyPlayTonePCmd()
    */
        {
    public:
        IMPORT_C TPlayToneV1();
    public:
        /** An alpha identifier containing text to display. */
        TAlphaId iAlphaId;

        /** The tone to play. */
        TTone iTone;

        /** The duration of the tone. */
        TDuration iDuration;
        };

    /** A packaged RSat::TPlayToneV1. */
    typedef TPckg<TPlayToneV1>  TPlayToneV1Pckg;


    class TPlayToneV2 : public TPlayToneV1
    /**
    Contains a PLAY TONE proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyPlayTonePCmd() for more information about
    the command.

    Request notification for PLAY TONE with RSat::NotifyPlayTonePCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TPlayTone" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TPlayToneV2Pckg, a packaged RSat::TPlayToneV2.
    Use RSat::TPlayToneV1 with older versions of Symbian OS.  For later
    versions of Symbian OS, check for an updated API (RSat::TPlayToneV3
    etc).

    @see RSat::TPlayToneV1
    @see RSat::TPlayToneV2Pckg
    @see RSat::NotifyPlayTonePCmd()
    */
        {
    public:
        IMPORT_C TPlayToneV2();
    public:
        /** An icon to display. */
        TIconId iIconId;
        };

    /** A packaged RSat::TPlayToneV2. */
    typedef TPckg<TPlayToneV2>  TPlayToneV2Pckg;

	class TPlayToneV5 : public TPlayToneV2
	/**
	Contains a PLAY TONE proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifyPlayTonePCmd() for more information about
	the command.

	Request notification for PLAY TONE with RSat::NotifyPlayTonePCmd(),
	passing it an object in which to store the command when it arrives.  Pass a
	packaged version of the latest "TPlayTone" class. For Symbian OS 7.0s,
	8.0 and 8.1 use RSat::TPlayToneV2Pckg, a packaged RSat::TPlayToneV2.
	For Symbian OS 9.2, use RSat::TPlayToneV5Pckg.
	Use RSat::TPlayToneV1 with older versions of Symbian OS.
	For later versions of Symbian OS, check for an updated API
	(RSat::TPlayToneV6 etc).

	@see RSat::TPlayToneV1
	@see RSat::TPlayToneV2
	@see RSat::TPlayToneV5Pckg
	@see RSat::NotifyPlayTonePCmd()
	*/
		{
	public:
		IMPORT_C TPlayToneV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TPlayToneV1::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TPlayToneV5. */
	typedef TPckg<TPlayToneV5>  TPlayToneV5Pckg;
	
	class TPlayToneV6 : public TPlayToneV5
	/**
	Contains a PLAY TONE proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifyPlayTonePCmd() for more information about
	the command.

	Request notification for PLAY TONE with RSat::NotifyPlayTonePCmd(),
	passing it an object in which to store the command when it arrives.  Pass a
	packaged version of the latest "TPlayTone" class. For Symbian OS 7.0s,
	8.0 and 8.1 use RSat::TPlayToneV2Pckg, a packaged RSat::TPlayToneV2.
	For Symbian OS 9.2, use RSat::TPlayToneV5Pckg.
	Use RSat::TPlayToneV1 with older versions of Symbian OS.
	For Symbian OS 9.4, use RSat::TPlayToneV6Pckg.
	For later versions of Symbian OS, check for an updated API
	(RSat::TPlayToneV7 etc).

	@see RSat::TPlayToneV1
	@see RSat::TPlayToneV2
	@see RSat::TPlayToneV5
	@see RSat::TPlayToneV6Pckg
	@see RSat::NotifyPlayTonePCmd()
	*/
		{
	public:
		IMPORT_C TPlayToneV6();

	public:
		TUint8 iFrameId; //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TPlayToneV6. */
	typedef TPckg<TPlayToneV6>  TPlayToneV6Pckg;
    
    IMPORT_C void NotifyPlayTonePCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TPlayToneRspV1 : public TSatPCmdBase
    /**
    Contains your response to a PLAY TONE proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyPlayTonePCmd() for more information about
    the command.

    For Symbian OS versions up to and including  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TPlayToneRspV1Pckg (a packaged TPlayTonetRspV1).
    For later versions of Symbian OS, check for an updated API
    (RSat::TPlayToneRspV2 etc).

    @see RSat::NotifyPlayTonePCmd()
    */
        {
    public:
        IMPORT_C TPlayToneRspV1();
    public:
        /** Proactive command general result.  The description of the PLAY TONE
        proactive command in ETSI TS 102 223 suggests results to return. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TPlayToneRspV1. */
    typedef TPckg<TPlayToneRspV1>  TPlayToneRspV1Pckg;

    //
    //Menu Items Base Class
    //
    /** Part of the SELECT ITEM and SET UP MENU proactive commands.
    Flags describing a menu's content.  RSat::TMenuItemBaseV1::MenuContent()
    returns a combination of these flags. */
    enum TMenuContent
        {
        KItems              = 0x01, //< Menu is made of items.
        KItemNextActions    = 0x02, //< Menu is made of items and item next actions.
   		KItemIconIds        = 0x04, //< Menu is made of items and item icon IDs.
   		KItemTextAttributes = 0x08  //< Menu items contain text attribute data as well as ID and item string.
        };

	/**
	@see RSat::TItem
	*/
    enum {KMenuItemMaxSize=240};

    /**
    Each item in a SET UP MENU or SELECT ITEM command's menu has these two
    properties.  RSat::TItem::iItemString is the string to display in the menu.
    If the user selects an item, return the item's RSat::TItem::iItemId to the
    UICC so that the UICC knows which item has been selected.

    Use RSat::TMenuItemBaseV1::GetItem() to retrieve an item's RSat::TItem.

    Menu items may also have an icon and/or a "next action identifier"
    associated with them.  These are also retrieved with
    RSat::TMenuItemBaseV1::GetItem()
    */
    struct TItem
        {
        TUint8 iItemId;                         //< (U)SAT menu item indentifier
        TBuf16<KMenuItemMaxSize> iItemString;   //< (U)SAT menu item string
        };

	/**
	@see RSat::TItemList
	*/
    enum {KItemListMaxSize=486};

    /** Contains the menu in a SET UP MENU and SELECT ITEM proactive commands.
    Do not access the menu directly; use RSat::TMenuItemBaseV1::GetItem() and
    RSat::TMenuItemBaseV1::AddItem() instead. */
    typedef TBuf16<KItemListMaxSize> TItemList;

    class TMenuItemBaseV1 : public TSatPCmdBase
    /**
    Base class for menu item based classes.

    The SET UP MENU and SELECT ITEM proactive commands both provide menus
    that are displayed to the user.  When the user chooses one of the menu's options,
    you should tell the UICC which option was chosen.

    In Symbian OS versions before 9.2, the proactive command contains menus in
    classes derived from RSat::TMenuItemBaseV1.
    For SET UP MENU they are RSat::TSetUpMenuV1, RSat::TSetUpMenuV2 etc.
    For SELECT ITEM these classes are RSat::TSelectItemV1, RSat::TSelectItemV2 etc.

    In Symbian OS version 9.2, the proactive command contains menus in
    classes derived from RSat::TMenuItemBaseV5 which itself derives from
    RSat::TMenuItemBaseV1.

    The description of the SET UP MENU and SELECT ITEM commands describes
    how to display the menu and what to do when the user makes a selection; see
    RSat::NotifySetUpMenuPCmd() and RSat::NotifySelectItemPCmd().

    RSat::TMenuItemBaseV1 provides methods to add and get the menu items, to
    return the number of items, and to get a description of the menu's content.
    RSat::TMenuItemBaseV5 extends the functionality provided by RSat::TMenuItemBaseV1
    by providing methods that allow items with text attributes to be added and
    retrieved.

    To read menu items:

    - Firstly, if you have received a SET UP MENU proactive command, call
    RSat::TMenuItemBaseV1::IsRemoveMenu().  If it returns ETrue then the SET UP
    MENU command does not contain a menu. Instead, the command indicates you
    should remove the menu provided by the previous command.  (Note that the
    SELECT ITEM proactive command always contains a menu; there is no need to
    call RSat::TMenuItemBaseV1::IsRemoveMenu().

    - If RSat::TMenuItemBaseV1::IsRemoveMenu() returns EFalse then the
    command contains a menu that replaces the previous menu.  Find out
    how many items are in the menu by calling
    RSat::TMenuItemBaseV1::NumberOfItems()

    - Now call RSat::TMenuItemBaseV1::MenuContent() to find out about
    the menu items.  All the items in each menu will have some text and an
    identifier.  In some menus, items may have "text attributes",
    "next item indicators" and/or an icons.

    - Finally call RSat::TMenuItemBaseV1::GetItem() and
    RSat::TMenuItemBaseV1::GetItemIcon() for each item.  These methods require
    an index number from 1 to X, where X is the number of items.  Choose the
    appropriate version of GetItem()  or GetItemIcon() to get the information
    you require.  For Symbian OS version 9.2, it is also possible to call
    RSat::TMenuItemBaseV5::GetItem() and RSat::TMenuItemBaseV5::GetItemIcon()
    to get menu item information if the items contain text attributes.

    @see RSat::TMenuItemBaseV5
    */
        {
    public:
        IMPORT_C TMenuItemBaseV1();
        IMPORT_C TBool IsRemoveMenu() const;

        IMPORT_C TInt MenuContent() const;
        IMPORT_C TUint NumberOfItems() const;

        IMPORT_C TInt GetItem    (TUint aIndex, TItem& aItem) const;
        IMPORT_C TInt GetItem    (TUint aIndex, TItem& aItem, TInt& aAction) const;
        IMPORT_C TInt GetItem    (TUint aIndex, TItem& aItem, TInt& aAction, TInt& aIconId) const;
        IMPORT_C TInt GetItemIcon(TUint aIndex, TItem& aItem, TInt& aIconId) const;

        IMPORT_C TInt AddItem    (const TItem& aItem);
        IMPORT_C TInt AddItem    (const TItem& aItem, TUint aAction);
        IMPORT_C TInt AddItem    (const TItem& aItem, TUint aAction, TUint aIconId);
        IMPORT_C TInt AddItemIcon(const TItem& aItem, TUint aIconId);

        IMPORT_C void Reset();

    protected:
        //TMenuItemHeader enum values are chosen arbitrary
        enum TMenuItemHeader
            {
            KItemHeader     				=0xAA00,
            KActionHeader   				=0xBB00,
            KIconIdHeader   				=0xCC00,
            KItemWithTextAttributeHeader 	=0xDD00
            };

        enum TBitMask
            {
            KLowerByte  =0x00FF,
            KUpperByte  =0xFF00
            };

        // Length = 1 word = 2 bytes
        enum {KHeaderLength=0x01};
        enum {KItemIdLength=0x01};
        enum {KActionLength=0x01};
        enum {KIconIdLength=0x01};

        TInt FindItemInBuffer(TUint aIndex, TInt& aOffset) const;
        TUint iNumberOfItems;   //< Number of items in the menu
        TUint8 iMenuContent;    //< Content of the menu: items + item text attributes + next actions + icon IDs
        TItemList iBuf;         //< List of the items in the menu
        };

	class TMenuItemBaseV5 : public TMenuItemBaseV1
	/**
    Base class for menu item based classes.

    The SET UP MENU and SELECT ITEM proactive commands both provide menus
    that are displayed to the user.  When the user chooses one of the menu's options,
    you should tell the UICC which option was chosen.

    In Symbian OS versions < 9.2, the proactive command contains menus in
    classes derived from RSat::TMenuItemBaseV1.
    For SET UP MENU they are RSat::TSetUpMenuV1, RSat::TSetUpMenuV2 etc.
    For SELECT ITEM these classes are RSat::TSelectItemV1, RSat::TSelectItemV2 etc.

    In Symbian OS version 9.2, the proactive command contains menus in
    classes derived from RSat::TMenuItemBaseV5 which itself derives from
    RSat::TMenuItemBaseV5.
    For SET UP MENU they are RSat::TSetUpMenuV5.
    For SELECT ITEM these classes are RSat::TSelectItemV5.

    The description of the SET UP MENU and SELECT ITEM commands describes
    how to display the menu and what to do when the user makes a selection; see
    RSat::NotifySetUpMenuPCmd() and RSat::NotifySelectItemPCmd().

    RSat::TMenuItemBaseV1 provides methods to add and get the menu items, to
    return the number of items, and to get a description of the menu's content.

    This class extends that functionality by providing support for text attributes
    for the menu item text.

    For more details on how to read menu items, see RSat::TMenuItemBaseV1

    @see TMenuItemBaseV1
    */
		{
	public:
		IMPORT_C TMenuItemBaseV5();
	public:
		using TMenuItemBaseV1::GetItem;
		using TMenuItemBaseV1::GetItemIcon;

		IMPORT_C TInt GetItem	(TUint aIndex, TItem& aItem, TTextAttribute& aTextAttribute) const;
		IMPORT_C TInt GetItem	(TUint aIndex, TItem& aItem, TTextAttribute& aTextAttribute, TInt& aAction) const;
		IMPORT_C TInt GetItemIcon	(TUint aIndex, TItem& aItem, TTextAttribute& aTextAttribute, TInt& aIconId) const;
		IMPORT_C TInt GetItem	(TUint aIndex, TItem& aItem, TTextAttribute& aTextAttribute, TInt& aAction, TInt& aIconId) const;

		using TMenuItemBaseV1::AddItem;
		using TMenuItemBaseV1::AddItemIcon;

		IMPORT_C TInt AddItem (const TItem& aItem, const TTextAttribute& aTextAttribute);
		IMPORT_C TInt AddItem (const TItem& aItem, const TTextAttribute& aTextAttribute, TUint aAction);
		IMPORT_C TInt AddItemIcon (const TItem& aItem, const TTextAttribute& aTextAttribute, TUint aIconId);
		IMPORT_C TInt AddItem (const TItem& aItem, const TTextAttribute& aTextAttribute, TUint aAction, TUint aIconId);
		};

    //
    //SET UP MENU
    //

    class TSetUpMenuV1 : public TMenuItemBaseV1
    /**
    Contains a SET UP MENU proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifySetUpMenuPCmd() for more information about
    the command.

    RSat::TSetUpMenuV1 derives from RSat::TMenuItemBaseV1, which provides the basic
    mechanism to handle the menu items, item next actions and item icon IDs.

    Request notification for SET UP MENU with RSat::NotifySetUpMenuPCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TSetUpMenu" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TSetUpMenuV2Pckg, a packaged RSat::TSetUpMenuV2.
    Use RSat::TSetUpMenuV1 with older versions of Symbian OS.  For later
    versions of Symbian OS, check for an updated API (RSat::TSetUpMenuV3
    etc).

    @see RSat::TSetUpMenuV2
    @see RSat::TSetUpMenuV1Pckg
    @see RSat::NotifySetUpMenuPCmd()
    */
        {
    public:
        IMPORT_C TSetUpMenuV1();
    public:
        TAlphaId iAlphaId;                  //< Alpha identifier that may contain text.  If so, this text is the title of the menu.
        THelp iHelp;                        //< Determines whether help is available.
        TIconId iIconId;                    //< Identifies the icon to display in the title of the menu.
        TIconQualifier iIconListQualifier;  //< Icon list qualifier.
        };

    /** A packaged RSat::TSetUpMenuV1. */
    typedef TPckg<TSetUpMenuV1>  TSetUpMenuV1Pckg;

    enum TSelectionPreference
    /**
    A menu from SET UP MENU or SELECT ITEM can be marked as
    RSat::ESoftKeyPreferred.  Then, if this feature is supported and the number
    of icon items does not exceed the number of soft keys available then
    display those icons as soft keys.
    */
        {
        ESelectionPreferenceNotSet, //< Initialization value.
        ENoSelectionPreference,     //< Use of soft keys is not required.
        ESoftKeyPreferred           //< Use soft keys where appropriate.
        };


    class TSetUpMenuV2 : public TSetUpMenuV1
    /**
    Contains a SET UP MENU proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifySetUpMenuPCmd() for more information about
    the command.

    RSat::TSetUpMenuV2 derives from RSat::TMenuItemBaseV1 and
    RSat::TSetUpMenuV1.  RSat::TSetUpMenuV1 provides the basic mechanism to
    handle the menu items, item next actions and item icon IDs.

    Request notification for SET UP MENU with RSat::NotifySetUpMenuPCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TSetUpMenu" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TSetUpMenuV2Pckg, a packaged RSat::TSetUpMenuV2.
    Use RSat::TSetUpMenuV1 with older versions of Symbian OS.  For later
    versions of Symbian OS, check for an updated API (RSat::TSetUpMenuV3
    etc).

    @see RSat::TSetUpMenuV1
    @see RSat::TSetUpMenuV2Pckg
    @see RSat::NotifySetUpMenuPCmd()
    */
        {
    public:
        IMPORT_C TSetUpMenuV2();
    public:
        TSelectionPreference iPreference; //< Determines whether soft key is used with the menu, if available.
        };

    /** A packaged RSat::TSetUpMenuV2. */
    typedef TPckg<TSetUpMenuV2>  TSetUpMenuV2Pckg;

	class TSetUpMenuV5 : public TMenuItemBaseV5
    /**
    Contains a SET UP MENU proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifySetUpMenuPCmd() for more information about
    the command.

    RSat::TSetUpMenuV5 derives from RSat::TMenuItemBaseV5 and
    RSat::TMenuItemBaseV1.  RSat::TMenuItemBaseV1 and RSat::TMenuItemBaseV5
    provide the basic mechanism to handle the menu items, menu item text attributes,
    item next actions and item icon IDs.

    Note that RSat::TSetUpMenuV5 does not derive from RSat::TSetUpMenuV2 and
    RSat::TSetUpMenuV1 but contains all of the attributes present in these
    two classes.

    Request notification for SET UP MENU with RSat::NotifySetUpMenuPCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TSetUpMenu" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TSetUpMenuV2Pckg, a packaged RSat::TSetUpMenuV2.
    Use RSat::TSetUpMenuV1 with older versions of Symbian OS.
    For Symbian OS 9.2, use RSat::TSetUpMenuV5.
    For later versions of Symbian OS, check for an updated API (RSat::TSetUpMenuV6
    etc).

    @see RSat::TMenuItemBaseV1
    @see RSat::TMenuItemBaseV5
    @see RSat::TSetUpMenuV5Pckg
    @see RSat::NotifySetUpMenuPCmd()
    */
		{
	public:
		IMPORT_C TSetUpMenuV5();
	public:
		TAlphaId iAlphaId;					//< Alpha identifier that may contain text.  If so, this text is the title of the menu.
		THelp iHelp;						//< Determines whether help is available.
		TIconId iIconId;					//< Identifies the icon to display in the title of the menu.
		TIconQualifier iIconListQualifier;	//< Icon list qualifier.

		TSelectionPreference iPreference;	//< Determines whether soft key is used with the menu, if available.
		TTextAttribute iTextAttribute;  	//< Text attribute data for the alpha identifier.
		};

	/** A packaged RSat::TSetUpMenuV5. */
	typedef TPckg<TSetUpMenuV5>  TSetUpMenuV5Pckg;

    IMPORT_C void NotifySetUpMenuPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    /** Cancels notification for a SET UP MENU proactive command. */
    IMPORT_C void NotifySetUpMenuPCmdCancel() const;



    class TSetUpMenuRspV1 : public TSatPCmdBase
    /**
    Contains your response to a SET UP MENU proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See  RSat::NotifySetUpMenuPCmd() for
    more information about the command.

    For Symbian OS versions up to and including  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TSetUpMenuRspV1Pckg (a packaged
    TSetUpMenuRspV1). For later versions of Symbian OS, check for an updated
    API (RSat::TSetUpMenuRspV2 etc).

    @see RSat::NotifySetUpMenuPCmd()
    */
        {
    public:
        IMPORT_C TSetUpMenuRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TSetUpMenuRspV1. */
    typedef TPckg<TSetUpMenuRspV1>  TSetUpMenuRspV1Pckg;

    //
    //SELECT ITEM
    //

    /** Part of the SELECT ITEM proactive command.  This describes the content
    of the menu items. This is information is provided to help you display the
    menu. */
    enum TPresentationType
        {
        EPresentationTypeNotSet, //< Value used in constructor.
        ENotSpecified,           //<Menu content type not specified
        EDataValues,             //< The menu contains a choice of data values
        ENavigationOptions       //< The menu contains a choice of navigation options
        };


    class TSelectItemV1 : public TMenuItemBaseV1
    /**
    Contains a SELECT ITEMS proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifySelectItemPCmd() for more information about
    the command.

    RSat::TSelectItemV1 derives from RSat::TMenuItemBaseV1 which provides the
    basic mechanism to handle the menu items, item next actions and item icon
    IDs.

    Request notification for SELECT ITEM with RSat::NotifySelectItemPCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TSelectItem" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TSelectItemV2Pckg, a packaged RSat::TSelectItemV2.
    Use RSat::TSelectItemV1 with older versions of Symbian OS.  For later
    versions of Symbian OS, check for an updated API (RSat::TSelectItemV3
    etc).

    @see RSat::TSelectItemV2
    @see RSat::TSelectItemV1Pckg
    @see RSat::NotifySelectItemPCmd()
    */
        {
    public:
        IMPORT_C TSelectItemV1();
    public:
        TPresentationType iPresentationType;//< Describes the menu's content type, which may aid presentation.
        TAlphaId iAlphaId;                  //< Alpha identifier that may contain text.  If so, this text is the title of the menu.
        THelp iHelp;                        //< Determines whether help is available.
        TUint8 iDefaultItemId;              //< ID of the default menu item.
        TIconId iIconId;                    //< Identifies the icon to display in the title of the menu.
        TIconQualifier iIconListQualifier;  //< Icon list qualifier.
        };

    /** A packaged RSat::TSelectItemV1. */
    typedef TPckg<TSelectItemV1>  TSelectItemV1Pckg;


    class TSelectItemV2 : public TSelectItemV1
    /**
    Contains a SELECT ITEM proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifySelectItemPCmd() for more information about
    the command.

    RSat::TSelectItemV2 derives from RSat::TSelectItemV1 and
    RSat::TMenuItemBaseV1. RSat::TMenuItemBaseV1 provides the basic mechanism
    to handle the menu items, item next actions and item icon IDs.

    Request notification for SELECT ITEM with RSat::NotifySelectItemPCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TSelectItem" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TSelectItemV2Pckg, a packaged RSat::TSelectItemV2.
    Use RSat::TSelectItemV1 with older versions of Symbian OS.  For later
    versions of Symbian OS, check for an updated API (RSat::TSelectItemV3
    etc).

    @see RSat::TSelectItemV1
    @see RSat::TSelectItemV2Pckg
    @see RSat::NotifySelectItemPCmd()
    */
    	{
    public:
        IMPORT_C TSelectItemV2();
    public:
        TSelectionPreference iPreference; //< Determines whether soft key is used with the menu, if available.
        };

    /** A packaged RSat::TSelectItemV2. */
    typedef TPckg<TSelectItemV2>  TSelectItemV2Pckg;

	class TSelectItemV5 : public TMenuItemBaseV5
    /**
    Contains a SELECT ITEM proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifySelectItemPCmd() for more information about
    the command.

    RSat::TSelectItemV5 derives from RSat::TMenuItemBaseV5 and
    RSat::TMenuItemBaseV1. RSat::TMenuItemBaseV1 and
    RSat::TMenuItemBaseV5 provide the basic mechanism to handle
    the menu items, text attributes, item next actions and item icon
    IDs.

    Note that although RSat::TSelectItemV5 does not derive from
    RSat::TSelectItemV2 and RSat::TSelectItemV1, it contains all of
    the parameters present in those two classes and is hence
    backwards compatible with both.

    Request notification for SELECT ITEM with RSat::NotifySelectItemPCmd(),
    passing it an object in which to store the command when it arrives.
    Pass a packaged version of the latest "TSelectItem" class.
    For Symbian OS 7.0s, 8.0 and 8.1 use RSat::TSelectItemV2Pckg,
    a packaged RSat::TSelectItemV2.  Use RSat::TSelectItemV1 with
    older versions of Symbian OS.  For Symbian OS version 9.2,
    use RSat::TSelectItemV5Pckg.  For later versions of Symbian OS,
    check for an updated API (RSat::TSelectItemV6 etc.).

    @see RSat::TMenuItemBaseV1
    @see RSat::TMenuItemBaseV5
    @see RSat::TSelectItemV5Pckg
    @see RSat::NotifySelectItemPCmd()
    */
		{
	public:
		IMPORT_C TSelectItemV5();
	public:
		TPresentationType iPresentationType;//< Describes the menu's content type, which may aid presentation.
		TAlphaId iAlphaId;                  //< Alpha identifier that may contain text.  If so, this text is the title of the menu.
		THelp iHelp;                        //< Determines whether help is available.
		TUint8 iDefaultItemId;              //< ID of the default menu item.
		TIconId iIconId;                    //< Identifies the icon to display in the title of the menu.
		TIconQualifier iIconListQualifier;  //< Icon list qualifier.

		TSelectionPreference iPreference; 	//< Determines whether soft key is used with the menu, if available.
		TTextAttribute iTextAttribute;      //< Text attribute data for the alpha identifier.
		};

    /** A packaged RSat::TSelectItemV5. */
    typedef TPckg<TSelectItemV5>  TSelectItemV5Pckg;

	class TSelectItemV6 : public TSelectItemV5
    /**
    Contains a SELECT ITEM proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifySelectItemPCmd() for more information about
    the command.

    RSat::TSelectItemV6 derives from RSat::TSelectItemV5.
    RSat::TSelectItemV5 derives from RSat::TMenuItemBaseV5 and
    RSat::TMenuItemBaseV1. RSat::TMenuItemBaseV1 and
    RSat::TMenuItemBaseV5 provide the basic mechanism to handle
    the menu items, text attributes, item next actions and item icon
    IDs.
    
    Request notification for SELECT ITEM with RSat::NotifySelectItemPCmd(),
    passing it an object in which to store the command when it arrives.
    Pass a packaged version of the latest "TSelectItem" class.
    For Symbian OS 7.0s, 8.0 and 8.1 use RSat::TSelectItemV2Pckg,
    a packaged RSat::TSelectItemV2.  Use RSat::TSelectItemV1 with
    older versions of Symbian OS.  For Symbian OS version 9.2,
    use RSat::TSelectItemV5Pckg.For Symbian OS version 9.4,
    use RSat::TSelectItemV6Pckg. For later versions of Symbian OS,
    check for an updated API (RSat::TSelectItemV7 etc.).

    @see RSat::TSelectItemV5
    @see RSat::TSelectItemV6Pckg
    @see RSat::NotifySelectItemPCmd()
    */
		{
	public:
		IMPORT_C TSelectItemV6();
	public:
		TUint8 iFrameId;      //<  Each frame has an ID. This field contains the ID of the frame. 
		};

    /** A packaged RSat::TSelectItemV6. */
    typedef TPckg<TSelectItemV6>  TSelectItemV6Pckg;
    
    IMPORT_C void NotifySelectItemPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TSelectItemRspV1 : public TSatPCmdBase
    /**
    Contains your response to a SELECT ITEM proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifySelectItemPCmd() for more information about the command.

    For Symbian OS versions up to and including  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TSelectItemRspV1Pckg (a packaged
    TSelectItemRspV1). For later versions of Symbian OS, check for an updated
    API (RSat::TSelectItemRspV2 etc).

    @see RSat::NotifySelectItemPCmd()
    */
        {
    public:
        IMPORT_C TSelectItemRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** Set to RSat::KItemIdentifier when returning the user's choice.
        Some error values of iGeneralResult require additional information, in
        which case set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains the user's chosen menu item.  Alternatively, some error
        values for iGeneralResult require additional information, which goes
        here. */
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TSelectItemRspV1. */
    typedef TPckg<TSelectItemRspV1>  TSelectItemRspV1Pckg;


    //
    // SEND SHORT MESSAGE
    //
    class TSendSmV1 : public TSatPCmdBase
    /**
    Contains a SEND SHORT MESSAGE proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifySendSmPCmd() for more information about
    the command.

    Request notification for SEND SHORT MESSAGE with RSat::NotifySendSmPCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TSendSm" class. For Symbian OS versions up
    to and including v8.1a and 8.1b use RSat::TSendSmV1Pckg, a packged
    RSat::TSendSmV1.  For later versions of Symbian OS, check for an updated
    API (RSat::TSendSmV2 etc).

    @see RSat::TSendSmV1Pckg
    @see RSat::NotifySendSmPCmd()
    */
        {
    public:
        IMPORT_C TSendSmV1();
    public:
        TAlphaId iAlphaId; //< An alpha identifier containing text to display.
        TAddress iAddress; //< The address of the message recipient.
        TTpdu iSmsTpdu;    //< The SMS message itself.
        TIconId iIconId;   //< An icon to display.
        };

    /** A packaged RSat::TSendSmV1. */
    typedef TPckg<TSendSmV1>  TSendSmV1Pckg;

	class TSendSmV5 : public TSendSmV1
	/**
	Contains a SEND SHORT MESSAGE proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifySendSmPCmd() for more information about
	the command.

	Request notification for SEND SHORT MESSAGE with RSat::NotifySendSmPCmd(),
	passing it an object in which to store the command when it arrives.  Pass a
	packaged version of the latest "TSendSm" class. For Symbian OS versions up
	to and including v8.1a and 8.1b use RSat::TSendSmV1Pckg, a packged
	RSat::TSendSmV1. For Symbian OS 9.2, use RSat::TSendSmV5Pckg.
	For later versions of Symbian OS, check for an updated
	API (RSat::TSendSmV6 etc).

	@see RSat::TSendSmV5Pckg
	@see RSat::NotifySendSmPCmd()
	*/
		{
	public:
		IMPORT_C TSendSmV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TSendSmV1::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TSendSmV5. */
	typedef TPckg<TSendSmV5>  TSendSmV5Pckg;

	class TSendSmV6 : public TSendSmV5
	/**
	Contains a SEND SHORT MESSAGE proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifySendSmPCmd() for more information about
	the command.

	Request notification for SEND SHORT MESSAGE with RSat::NotifySendSmPCmd(),
	passing it an object in which to store the command when it arrives.  Pass a
	packaged version of the latest "TSendSm" class. For Symbian OS versions up
	to and including v8.1a and 8.1b use RSat::TSendSmV1Pckg, a packged
	RSat::TSendSmV1. For Symbian OS 9.2, use RSat::TSendSmV5Pckg.
	For Symbian OS 9.4, use RSat::TSendSmV6Pckg.
	For later versions of Symbian OS, check for an updated
	API (RSat::TSendSmV7 etc).

	@see RSat::TSendSmV6Pckg
	@see RSat::NotifySendSmPCmd()

	*/
		{
	public:
		IMPORT_C TSendSmV6();

	public:
		TUint8 iFrameId; //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TSendSmV6. */
	typedef TPckg<TSendSmV6>  TSendSmV6Pckg;
    IMPORT_C void NotifySendSmPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TSendSmRspV1 : public TSatPCmdBase
    /**
    Contains your response to a SEND SHORT MESSAGE proactive command.  Pass
    this to the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifySendSmPCmd() for more information about the command.

    For Symbian OS versions up to and including  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TSendSmRspV1Pckg (a packaged TSendSmRspV1).
    For later versions of Symbian OS, check for an updated API
    (RSat::TSendSmRspV2 etc).

    @see RSat::NotifySendSmPCmd()
    */
        {
    public:
        IMPORT_C TSendSmRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  To return a network error,
        set this KSatNetworkErrorInfo.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If you get an error from the network, you can
        return it in here, setting iInfoType to KSatNetworkErrorInfo. If the
        value in iGeneralResult does not require any information then leave
        this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TSendSmRspV1. */
    typedef TPckg<TSendSmRspV1>  TSendSmRspV1Pckg;

    //
    // Send SMS no logging permitted
    //
    class TSatSmsV1: public TSatPCmdBase
    /**
    Parameters for a RSat::SendMessageNoLogging() message.
    */
        {
    public:
        IMPORT_C TSatSmsV1();
    public:
        TTpdu iBuf;             //< SMS TPDU
        TAddress iServiceCenter;//< If iServiceCenter is a null buffer then use the default sc address
        };

    /** A packaged RSat::TSendSmRspV1. */
    typedef TPckg<TSatSmsV1> TSatSmsV1Pckg;

    IMPORT_C void SendMessageNoLogging(TRequestStatus& aReqStatus, const TDesC8& aMsg, TUint16& aMsgRef) const;

    //
    // SEND SS (Supplementary Service pro-active command).
    //

    class TSendSsV1 : public TSatPCmdBase
    /**
    Contains a SEND SS (Supplementary Service) proactive command issued by the
    UICC.   See ETSI TS 102 223 and RSat::NotifySendSsPCmd() for more
    information about the command.

    Request notification for SEND SS with RSat::NotifySendSsPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TSendSs" class. For Symbian OS versions
    up to and including v8.1a and 8.1b use RSat::TSendSsV1Pckg, a packaged
    RSat:TSendSsV1.  For later versions of Symbian OS, check for an updated
    API (RSat::TSendSsV2 etc).

    @see RSat::TSendSsV1Pckg
    @see RSat::NotifySendSsPCmd()
    */
        {
    public:
        IMPORT_C TSendSsV1();
    public:
        TAlphaId iAlphaId;   //< An alpha identifier containing text to display.
        TSsString iSsString; //< The supplementary services string.
        TIconId iIconId;     //< An icon to display.
        };

    /** A packaged RSat::TSendSsV1. */
    typedef TPckg<TSendSsV1>  TSendSsV1Pckg;

   class TSendSsV6 : public TSendSsV1
    /**
    Contains a SEND SS (Supplementary Service) proactive command issued by the
    UICC.   See ETSI TS 102 223 and RSat::NotifySendSsPCmd() for more
    information about the command.

    Request notification for SEND SS with RSat::NotifySendSsPCmd(), For Symbian OS versions
    up to and including v8.1a and 8.1b use RSat::TSendSsV1Pckg, a packaged RSat:TSendSsV1. 
    For Symbian OS v9.4, use RSat::TSendSsV6. For later versions of Symbian OS, check for 
    an updated API (RSat::TSendSsV7 etc).

    @see RSat::TSendSsV6Pckg
    @see RSat::NotifySendSsPCmd()
  
    */
        {
    public:
        IMPORT_C TSendSsV6();
    public:
    	/** Contains formatting information to be applied when
		RSat::TSendSsV1::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
        };

    /** A packaged RSat::TSendSsV6. */
    typedef TPckg<TSendSsV6>  TSendSsV6Pckg;
    
    IMPORT_C void NotifySendSsPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TSendSsRspV1 : public TSatPCmdBase
    /**
    Contains your response to a SEND SS proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifySendSsPCmd() for more information about
    the command.

    For Symbian OS v6.1 or v7.0 pass RSat::TerminalRsp() a
    RSat::TSendSsRspV1Pckg (a packaged TSendSsRspV1). For versions up to and
    including v8.1a and v8.1b use RSat::TSendSsRspV2. For later versions of
    Symbian OS, check for an updated API (RSat::TSendSsRspV3 etc).

    @see RSat::NotifySendSsPCmd()
    */
        {
    public:
        IMPORT_C TSendSsRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;
        TUint iInfoType;                    //< Additional info type
        TAdditionalInfo iAdditionalInfo;    //< Additional info

         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult2;

        TUint iInfoType2;                   //< Second additional info type
        TAdditionalInfo iAdditionalInfo2;   //< Second additional info
        TCallControlV1 iCcRequestedAction;  //< Action requested by the call control procedure.
        };

    /** A packaged RSat::TSendSsRspV1. */
    typedef TPckg<TSendSsRspV1>  TSendSsRspV1Pckg;


    class TSendSsRspV2 : public TSendSsRspV1
    /**
    Contains your response to a SEND SS proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifySendSsPCmd() for more information about
    the command.

    For Symbian OS v6.1 or v7.0 pass RSat::TerminalRsp() a
    RSat::TSendSsRspV1Pckg (a packaged TSendSsRspV1). For versions up to and
    including v8.1a and v8.1b use RSat::TSendSsRspV2. For later versions of
    Symbian OS, check for an updated API (RSat::TSendSsRspV3 etc).

    @see RSat::NotifySendSsPCmd()
    */
        {
    public:
        IMPORT_C TSendSsRspV2();
    public:
        TUssdString iUssdString;    //< Text returned within the Return Result message from the network.
        };

    /** A packaged RSat::TSendSsRspV2. */
    typedef TPckg<TSendSsRspV2>  TSendSsRspV2Pckg;

    //
    // SEND USSD (Unstructured Supplementary Service Data)
    //

    class TSendUssdV1 : public TSatPCmdBase
    /**
    Contains a SEND USSD (Unstructured Supplementary Service Data) proactive
    command issued by the UICC.   See ETSI TS 102 223 and
    RSat::NotifySendUssdPCmd() for more information about the command.

    Request notification for SEND USSD with RSat::NotifySendUssdPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TSendUssd" class. For Symbian OS versions
    up to and including v8.1a and 8.1b use RSat::TSendUssdV1Pckg, a packaged
    RSat::TSendUssdV1.  For later versions of Symbian OS, check for an updated
    API (RSat::TSendUssdV2 etc).

    @see RSat::TSendUssdV1Pckg
    @see RSat::NotifySendUssdPCmd()
    */
       {
    public:
        IMPORT_C TSendUssdV1();
    public:
        TAlphaId iAlphaId;       //< An alpha identifier containing text to display.
        TUssdString iUssdString; //< The Unstructured Supplementary Service Data (USSD) string.
        TIconId iIconId;         //< An icon to display.
        };

    /** A packaged RSat::TSendUssdV1. */
    typedef TPckg<TSendUssdV1>  TSendUssdV1Pckg;

    class TSendUssdV6 : public TSendUssdV1
    /**
    Contains a SEND USSD (Unstructured Supplementary Service Data) proactive
    command issued by the UICC.   See ETSI TS 102 223 and
    RSat::NotifySendUssdPCmd() for more information about the command.

    Request notification for SEND USSD with RSat::NotifySendUssdPCmd(), For Symbian OS versions
    up to and including v8.1a and 8.1b use RSat::TSendUssdV1Pckg, a packaged RSat:TSendUssdV1. 
    For Symbian OS v9.4, use RSat::TSendUssdV6Pckg, a packaged RSat:TSendUssdV6. For later versions of Symbian OS, check for 
    an updated API (RSat::TSendUssdV7 etc). 
    @see RSat::TSendUssdV6Pckg
    @see RSat::NotifySendUssdPCmd()
    
    */
       {
    public:
        IMPORT_C TSendUssdV6();
    public:
    	/** Contains formatting information to be applied when
		RSat::TSendUssdV1::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
        };

    /** A packaged RSat::TSendUssdV1. */
    typedef TPckg<TSendUssdV6>  TSendUssdV6Pckg;
    
    IMPORT_C void NotifySendUssdPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TSendUssdRspV1 : public TSatPCmdBase
    /**
    Contains your response to a SEND USSD proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifySendUssdPCmd() for more information about
    the command.

    For Symbian OS versions up to and including  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TSendUssdRspV1Pckg (a packaged TSendUssdRspV1).
    For later versions of Symbian OS, check for an updated API
    (RSat::TSendUssdRspV2 etc).

    @see RSat::NotifySendUssdPCmd()
    */
        {
    public:
        IMPORT_C TSendUssdRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        TUint iInfoType;                    //< Additional info type
        TAdditionalInfo iAdditionalInfo;    //< Additional info

         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult2;

        TUint iInfoType2;                   //< Second additional info type
        TAdditionalInfo iAdditionalInfo2;   //< Second additional info
        TUssdString iUssdString;            //< USSD string
        TCallControlV1 iCcRequestedAction;  //< Action requested by the call control procedure.
        };

    /** A packaged RSat::TSendUssdRspV1. */
    typedef TPckg<TSendUssdRspV1>  TSendUssdRspV1Pckg;

    //
    // SET UP CALL
    //
    enum TSetUpCallType
    /** Part of a SET UP CALL proactive command.  Determines the conditions
    under which a call should be set up.
    @see RSat::TSetUpCallV1
    */
        {
        ESetUpCallTypeNotSet,            //< Value used in constructor.
        EOnlyIfNotBusy,                  //< Only make the call if line is not busy.
        EOnlyIfNotBusyWithRedial,        //< Only make the call if line is not busy. Redial if call cannot be made the first time.
        EHoldOtherCalls,                 //< Put other calls on hold then make the call.
        EHoldOtherCallsWithRedial,       //< Put other calls on hold then make the call. Redial if call cannot be made the first time.
        EDisconnectOtherCalls,           //< Disconnect other calls then make the call.
        EDisconnectOtherCallsWithRedial  //< Disconnect other calls then make the call. Redial if call cannot be made the first time.
        };


    class TSetUpCallV1 : public TSatPCmdBase
    /**
    Contains a SET UP CALL proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifySetUpCallPCmd() for more information about
    the command.

    Request notification for SET UP CALL with RSat::NotifySetUpCallPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TSetUpCall" class. For Symbian OS versions
    up to and including v8.1a and 8.1b use RSat::TSetUpCallV1Pckg, a packaged
    RSat::TSetUpCallV1.  For later versions of Symbian OS, check for an updated
    API (RSat::TSetUpCallV2 etc).

    @see RSat::TSetUpCallV1Pckg
    @see RSat::NotifySetUpCallPCmd()
    */
        {
    public:
        IMPORT_C TSetUpCallV1();
    public:
        /** Determines whether you re-dial if initial attempts to make the call fail.
        Plus, determines what to do if there is already a call in progress. */
        TSetUpCallType iType;
        TAlphaId iAlphaIdConfirmationPhase; //< Some text for the confirmation phase.
        TIconId iIconIdConfirmationPhase;   //< An icon for the confirmation phase.
        TAddress iAddress;                  //< The number to call.
        TCcp iCapabilityConfigParams;       //< Capability configuration parameters.
        TSubAddress iSubAddress;            //< The sub-address of the remote party.
        TDuration iDuration;                //< Determines how long you should you try to re-dial.
        TAlphaId iAlphaIdCallSetUpPhase;    //< Some text for the call set up phase.
        TIconId iIconIdCallSetUpPhase;      //< An icon for the call set up phase.
        };

    /** A packaged RSat::TSetUpCallV1. */
    typedef TPckg<TSetUpCallV1>  TSetUpCallV1Pckg;

	class TSetUpCallV5 : public TSetUpCallV1
	/**
	Contains a SET UP CALL proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifySetUpCallPCmd() for more information about
	the command.

	Request notification for SET UP CALL with RSat::NotifySetUpCallPCmd(), passing
	it an object in which to store the command when it arrives.  Pass a
	packaged version of the  latest "TSetUpCall" class. For Symbian OS versions
	up to and including v8.1a and 8.1b use RSat::TSetUpCallV1Pckg, a packaged
	RSat::TSetUpCallV1.  For Symbian OS 9.2, use RSat::TSetUpCallV5Pckg.
	For later versions of Symbian OS, check for an updated
	API (RSat::TSetUpCallV6 etc).

	@see RSat::TSetUpCallV1
	@see RSat::TSetUpCallV5Pckg
	@see RSat::NotifySetUpCallPCmd()
	*/
		{
	public:
		IMPORT_C TSetUpCallV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TSetUpCallV1::iAlphaIdConfirmationPhase is displayed. 
		@see RSat::TTextAttribute
		*/
		TTextAttribute iTextAttributeConfirmationPhase;

		/** Contains formatting information to be applied when
		RSat::TSetUpCallV1::iAlphaIdCallSetUpPhase is displayed. 
		@see RSat::TTextAttribute
		*/
		TTextAttribute iTextAttributeCallSetUpPhase;
		};

	/** A packaged RSat::TSetUpCallV5. */
	typedef TPckg<TSetUpCallV5>  TSetUpCallV5Pckg;

	class TSetUpCallV6 : public TSetUpCallV5
	/**
	Contains a SET UP CALL proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifySetUpCallPCmd() for more information about
	the command.

	Request notification for SET UP CALL with RSat::NotifySetUpCallPCmd(), passing
	it an object in which to store the command when it arrives.  Pass a
	packaged version of the  latest "TSetUpCall" class. For Symbian OS versions
	up to and including v8.1a and 8.1b use RSat::TSetUpCallV1Pckg, a packaged
	RSat::TSetUpCallV1.  For Symbian OS 9.2, use RSat::TSetUpCallV5Pckg.
	For Symbian OS 9.4, use RSat::TSetUpCallV6Pckg.
	For later versions of Symbian OS, check for an updated
	API (RSat::TSetUpCallV7 etc).

	@see RSat::TSetUpCallV5
	@see RSat::TSetUpCallV6Pckg
	@see RSat::NotifySetUpCallPCmd()

	*/
		{
	public:
		IMPORT_C TSetUpCallV6();

	public:
		TUint8 iFrameId; //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TSetUpCallV6. */
	typedef TPckg<TSetUpCallV6>  TSetUpCallV6Pckg;
	
	IMPORT_C void NotifySetUpCallPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;
    IMPORT_C void NotifySetUpCallPCmdCancel() const;

    class TSetUpCallRspV1 : public TSatPCmdBase
    /**
    Contains your response to a SET UP CALL proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifySetUpCallPCmd() for more information about
    the command.

    For Symbian OS v6.1 or v7.0 pass RSat::TerminalRsp() a
    RSat::TSetUpCallRspV1Pckg (a packaged TSetUpCallRspV1).  For versions up to
    and included v8.1a and v8.1b use RSat::TSetUpCallRspV2Pckg (a packaged
    TSetUpCallRspV2).   For later versions of Symbian OS, check for an updated
    API (RSat::TSetUpCallRspV3 etc).

    @see RSat::TSetUpCallRspV2
    @see RSat::NotifySetUpCallPCmd()
    */
        {
    public:
        IMPORT_C TSetUpCallRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        TUint iInfoType;                    //< Additional information type
        TAdditionalInfo iAdditionalInfo;    //< Additional info

         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult2;

        TUint iInfoType2;                   //< Second additional info type
        TAdditionalInfo iAdditionalInfo2;   //< Second additional info
        TCallControlV1 iCcRequestedAction;  //< Action requested by the call control procedure
        };

    /** A packaged RSat::TSetUpCallRspV1. */
    typedef TPckg<TSetUpCallRspV1>  TSetUpCallRspV1Pckg;


    class TSetUpCallRspV2 : public TSetUpCallRspV1
    /**
    Contains your response to a SET UP CALL proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifySetUpCallPCmd() for more information about
    the command.

    For Symbian OS v6.1 or v7.0 pass RSat::TerminalRsp() a
    RSat::TSetUpCallRspV1Pckg (a packaged TSetUpCallRspV1).  For versions up to
    and included v8.1a and v8.1b use RSat::TSetUpCallRspV2Pckg (a packaged
    TSetUpCallRspV2).   For later versions of Symbian OS, check for an updated
    API (RSat::TSetUpCallRspV3 etc).

    @see RSat::TSetUpCallRspV1
    @see RSat::NotifySetUpCallPCmd()
    */
        {
    public:
        IMPORT_C TSetUpCallRspV2();
    public:
        TUssdString iUssdString;    //< Text returned within the Return Result message from the network.
        };

    /** A packaged RSat::TSetUpCallRspV2. */
    typedef TPckg<TSetUpCallRspV2>  TSetUpCallRspV2Pckg;


    enum TRefreshType
    /** Part of a REFRESH proactive command.
    Indicates the type of REFRESH command sent by the UICC
    @see RSat::TRefreshV1
    */
        {
        ERefreshTypeNotSet,                 //< Initialization value used within constructor(s).
        ESimInitFullFileChangeNotification, //< SIM or USIM Initialization and Full File Change Notification.
        EFileChangeNotification,            //< SIM or USIM File Change Notification.
        ESimInitFileChangeNotification,     //< SIM or USIM Initialization and File Change Notification.
        ESimInit,                           //< SIM or USIM Initialization.
        ESimReset,                          //< GSM SIM or UICC Reset.
        EUsimApplicationReset,              //< USIM Application Reset.
        E3GSessionReset                     //< 3G Session Reset.
        };

	/**
	@see RSat::TRefreshFileList
	*/
    enum {KRefreshedFileListMaxSize=242};

    /** Part of a REFRESH proactive command.
    Contains the list of files that have been changed by the UICC.
    @see RSat::TRefreshV1
    */
    typedef TBuf16<KRefreshedFileListMaxSize> TRefreshFileList;


    class TRefreshV1 : public TSatPCmdBase
    /**
    Contains a REFERSH proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifyRefreshPCmd() for more information about
    the command.

    Request notification for REFRESH with RSat::NotifyRefreshPCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TRefresh" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TRefreshV2Pckg, a packged RSat::TRefreshV2.
    Use RSat::TRefreshV1 with older versions of Symbian OS.  For later
    versions of Symbian OS, check for an updated API (RSat::TRefreshV3
    etc).

    @see RSat::TRefreshV2
    @see RSat::TRefreshV1Pckg
    @see RSat::NotifyRefreshPCmd()
    */
        {
    public:
        IMPORT_C TRefreshV1();
    public:
        TRefreshType iType;         //< Type of refresh command
        TRefreshFileList iFileList; //< Optional list of files that need to be refreshed.
        };

    /** A packaged RSat::TRefreshV1. */
    typedef TPckg<TRefreshV1>  TRefreshV1Pckg;

	/**
	@see RSat::TAid
	*/
    enum {KAidMaxSize=16};

    /** Part of a REFRESH proactive command.
    Indicates the application on the UICC to be refreshed as defined in ETSI TS 101 220.
    @see RSat::TRefreshV1 */
    typedef TBuf8<KAidMaxSize> TAid;


    class TRefreshV2 : public TRefreshV1
    /**
    Contains a REFERSH proactive command issued by the UICC.   See ETSI TS
    102 223 and RSat::NotifyRefreshPCmd() for more information about
    the command.

    Request notification for REFRESH with RSat::NotifyRefreshPCmd(),
    passing it an object in which to store the command when it arrives.  Pass a
    packaged version of the latest "TRefresh" class. For Symbian OS 7.0s,
    8.0 and 8.1 use RSat::TRefreshV2Pckg, a packged RSat::TRefreshV2.
    Use RSat::TRefreshV1 with older versions of Symbian OS.  For later
    versions of Symbian OS, check for an updated API (RSat::TRefreshV3
    etc).

    @see RSat::TRefreshV2
    @see RSat::TRefreshV1Pckg
    @see RSat::NotifyRefreshPCmd()
    */
        {
    public:
        IMPORT_C TRefreshV2();
    public:
        TAid    iAid; //< ID of an application that needs to be refreshed.
        };

    /** A packaged RSat::TRefreshV2. */
    typedef TPckg<TRefreshV2>  TRefreshV2Pckg;

    IMPORT_C void NotifyRefreshPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TRefreshRspV1 : public TSatPCmdBase
    /**
    Contains your response to a REFRESH proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyRefreshPCmd() for more information about
    the command.

    For Symbian OS versions up to and including  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TRefreshRspV1Pckg (a packaged TRefreshRspV1).
    For later versions of Symbian OS, check for an updated API
    (RSat::TRefreshRspV2 etc).

    @see RSat::NotifyRefreshPCmd()
    */
        {
    public:
        IMPORT_C TRefreshRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        TUint iInfoType;                //< Additional info type
        TAdditionalInfo iAdditionalInfo;//< Additional info
        };

    /** A packaged RSat::TRefreshRspV1. */
    typedef TPckg<TRefreshRspV1>  TRefreshRspV1Pckg;

    //
    //SET UP EVENT LIST
    //
    enum TSetUpEventListType
    /** Part of the SET UP EVENT LIST proactive command. It determines the
    command type: does the command contain a new event list or does it cancel
    the previous event list. */
        {
        ESetUpEventListTypeNotSet, //< Value used in constructor.
        EUpdateEventList,          //< Command contains a new event list that replaces any previous list.
        ERemoveExistingEventList   //< Command contains no event list.  Instead, remove any existing events from the list.
        };


    class TSetUpEventListV1 : public TSatPCmdBase
    /**
    Contains a SET UP EVENT LIST proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifySetUpEventListPCmd() for more information about
    the command.

    Request notification for SET UP EVENT LIST with RSat::NotifySetUpEventListPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TSetUpEventList" class. For Symbian OS versions
    up to and including v8.1a and 8.1b use RSat::TSetUpEventListV1Pckg, a packged
    RSat::TSetUpEventListV1.  For later versions of Symbian OS, check for an updated
    API (RSat::TSetUpEventListV2 etc).

    @see RSat::TSetUpEventListV1Pckg
    @see RSat::NotifySetUpEventListPCmd()
    */
        {
    public:
        IMPORT_C TSetUpEventListV1();
    public:
        TSetUpEventListType iType; //< The command type.
        TUint iEvents; //< The list of events in the form of flags from RSat::TEventList.
        };

    /** A packaged RSat::TSetUpEventListV1. */
    typedef TPckg<TSetUpEventListV1>  TSetUpEventListV1Pckg;

    IMPORT_C void NotifySetUpEventListPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TSetUpEventListRspV1 : public TSatPCmdBase
    /**
    Contains your response to a SET UP EVENT LIST proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifySetUpEventListPCmd() for more information about
    the command.

    For Symbian OS versions up to and including  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TSetUpEventListRspV1Pckg (a packaged TSetUpEventListRspV1).
    For later versions of Symbian OS, check for an updated API
    (RSat::TSetUpEventListRspV2 etc).

    @see RSat::NotifySetUpEventListPCmd()
    */
        {
    public:
        IMPORT_C TSetUpEventListRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TSetUpEventListRspV1. */
    typedef TPckg<TSetUpEventListRspV1>  TSetUpEventListRspV1Pckg;

    //
    //SET UP IDLE MODE TEXT
    //
    enum TSetUpIdleModeTextType
    /** Part of the SET UP IDLE MODE TEXT proactive command.
    This determines the type of the command.
    @see RSat::TSetUpIdleModeTextV1 */
        {
        ESetUpIdleModeTextTypeNotSet, //< Initialization value

        /** The SET UP IDLE MODE TEXT command contains idle mode text and
        perhaps an icon to accompany it. */
        EUpdateIdleModeText,

        /** The existing idle mode text and icon should no longer be used.
        The SET UP IDLE MODE TEXT contains no new text. */
        ERemoveExistingIdleModeText
        };

	/**
	The buffer needs to be able to contain 276 16-bit characters.

	Expected Sequence 1.7 in section 27.22.4.22.1.4.2 of 3GPP TS 11.10-4
	v8.14.0 states that large text length to be tested should be of length
	274.  We are providing enough storage for 276 characters in any coding
	scheme as specified by TSatTextCoding.

	@see RSat::TSatTextCoding
	@see RSat::TSetUpIdleModeTextV1
	*/
    enum {KIdleModeTextMaxSize=276};

    /**
    Part of the SET UP IDLE MODE TEXT proactive command.
    This contains the idle mode text.

    This buffer assumes that 16-bit descriptors are being used by default.

    @see RSat::TSetUpIdleModeTextV1
	@see RSat::KIdleModeTextMaxSize
    */
    typedef TBuf16<KIdleModeTextMaxSize> TIdleModeText;


    class TSetUpIdleModeTextV1 : public TSatPCmdBase
    /**
    Contains a SET UP IDLE MODE TEXT proactive command issued by the UICC.   See
    ETSI TS 102 223 v5.9.0 section 6.6.22 and RSat::NotifySetUpIdleModeTextPCmd()
    for more information about the command.

    Request notification for SET UP IDLE MODE TEXT with
    RSat::NotifySetUpIdleModeTextPCmd(), passing it an object in which to store
    the command when it arrives.  Pass a packaged version of the  latest
    "TSetUpIdleModeText" class. For Symbian OS versions up to and including
    v8.1a and 8.1b use RSat::TSetUpIdleModeTextV1Pckg, a packaged
    RSat::TSetUpIdleModeTextV1.  For later versions of Symbian OS, check for an
    updated API (RSat::TSetUpIdleModeTextV2 etc).

    @see RSat::TSetUpIdleModeTextV1Pckg
    @see RSat::NotifySetUpIdleModeTextPCmd()
	@see RSat::TSatTextCoding
    */
        {
    public:
        IMPORT_C TSetUpIdleModeTextV1();
    public:

        /** Describes the type of command: does it contain new idle mode text
        or should the existing idle mode text be removed? */
        TSetUpIdleModeTextType iType;

        /** Idle mode text.
        Only valid if iType is RSat::EUpdateIdleModeText. */
        TIdleModeText iText;

        /** Identifies the icon to display.
        Only valid if iType is RSat::EUpdateIdleModeText. */
        TIconId iIconId;

		/**
		Specifies whether the text is 7-bit packed, 7-bit unpacked (i.e.
		8-bit) or UCS-2 16-bit.
		*/
		TSatTextCoding iCodingScheme;
        };

    /** A packaged RSat::TSetUpIdleModeTextV1. */
    typedef TPckg<TSetUpIdleModeTextV1>  TSetUpIdleModeTextV1Pckg;

	class TSetUpIdleModeTextV5 : public TSetUpIdleModeTextV1
	/**
   	Contains a SET UP IDLE MODE TEXT proactive command issued by the UICC.   See
	ETSI TS 102 223 v5.9.0 section 6.6.22 and RSat::NotifySetUpIdleModeTextPCmd()
	for more information about the command.

	Request notification for SET UP IDLE MODE TEXT with
	RSat::NotifySetUpIdleModeTextPCmd(), passing it an object in which to store
	the command when it arrives.  Pass a packaged version of the  latest
	"TSetUpIdleModeText" class. For Symbian OS versions up to and including
	v8.1a and 8.1b use RSat::TSetUpIdleModeTextV1Pckg, a packaged
	RSat::TSetUpIdleModeTextV1.
	For Symbian OS 9.2, use RSat::TSetUpIdleModeTextV5Pckg.
	For later versions of Symbian OS, check for an
	updated API (RSat::TSetUpIdleModeTextV6 etc).

	@see RSat::TSetUpIdleModeTextV1
	@see RSat::TSetUpIdleModeTextV5Pckg
	@see RSat::NotifySetUpIdleModeTextPCmd()
	*/
		{
	public:
		IMPORT_C TSetUpIdleModeTextV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TSetUpIdleModeTextV1::iText is displayed. 
		@see RSat::TTextAttribute
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TSetUpIdleModeTextV5. */
	typedef TPckg<TSetUpIdleModeTextV5>  TSetUpIdleModeTextV5Pckg;

   	class TSetUpIdleModeTextV6 : public TSetUpIdleModeTextV5
	/**
   	Contains a SET UP IDLE MODE TEXT proactive command issued by the UICC.   See
	ETSI TS 102 223 v5.9.0 section 6.6.22 and RSat::NotifySetUpIdleModeTextPCmd()
	for more information about the command.

	Request notification for SET UP IDLE MODE TEXT with
	RSat::NotifySetUpIdleModeTextPCmd(), passing it an object in which to store
	the command when it arrives.  Pass a packaged version of the  latest
	"TSetUpIdleModeText" class. For Symbian OS versions up to and including
	v8.1a and 8.1b use RSat::TSetUpIdleModeTextV1Pckg, a packaged
	RSat::TSetUpIdleModeTextV1.
	For Symbian OS 9.2, use RSat::TSetUpIdleModeTextV5Pckg.
	For Symbian OS 9.4, use RSat::TSetUpIdleModeTextV6Pckg.
	For later versions of Symbian OS, check for an
	updated API (RSat::TSetUpIdleModeTextV7 etc).

	@see RSat::TSetUpIdleModeTextV5
	@see RSat::TSetUpIdleModeTextV6Pckg
	@see RSat::NotifySetUpIdleModeTextPCmd()
	
	*/
		{
	public:
		IMPORT_C TSetUpIdleModeTextV6();

	public:
		TUint8 iFrameId; //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TSetUpIdleModeTextV6. */
	typedef TPckg<TSetUpIdleModeTextV6>  TSetUpIdleModeTextV6Pckg;
	
    IMPORT_C void NotifySetUpIdleModeTextPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TSetUpIdleModeTextRspV1 : public TSatPCmdBase
    /**
    Contains your response to a SET UP IDLE TEXT proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifySetUpIdleModeTextPCmd() for more information about
    the command.

    For Symbian OS versions up to and including  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TSetUpIdleModeTextRspV1Pckg (a packaged TSetUpIdleModeTextRspV1).
    For later versions of Symbian OS, check for an updated API
    (RSat::TSetUpIdleModeTextRspV2 etc).

    @see RSat::NotifySetUpIdleModeTextPCmd()
    */
        {
    public:
        IMPORT_C TSetUpIdleModeTextRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TSetUpIdleModeTextRspV1. */
    typedef TPckg<TSetUpIdleModeTextRspV1>  TSetUpIdleModeTextRspV1Pckg;

    //
    //SEND DTMF COMMAND
    //
    /**
    @see RSat::TDtmfString
    */
    enum{KDtmfStringMaxSize=243};

    /** Part of the SEND DTMF proactive command.
    This contains the DTMF string. Maximum size is 243.
    @see RSat::TSendDtmfV1 */
    typedef TBuf8<KDtmfStringMaxSize> TDtmfString;

    class TSendDtmfV1 : public TSatPCmdBase
    /**
    Contains a SEND DTMF proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifySendDtmfPCmd() for more information about
    the command.

    Request notification for SEND DTMF with RSat::NotifySendDtmfPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TSendDtmf" class. For Symbian OS versions
    up to and including v8.1a and 8.1b use RSat::TSendDtmfV1Pckg, a packaged
    RSat::TSendDtmfV1.  For later versions of Symbian OS, check for an updated
    API (RSat::TSendDtmfV2 etc).

    @see RSat::TSendDtmfV1Pckg
    @see RSat::NotifySendDtmfPCmd()
    */
        {
    public:
        IMPORT_C TSendDtmfV1();
    public:
        TAlphaId iAlphaId;       //< An alpha identifier containing text to display.
        TDtmfString iDtmfString; //< The DTMF string.
        TIconId iIconId;         //< An icon to display.
        };

    /** A packaged RSat::TSendDtmfV1. */
    typedef TPckg<TSendDtmfV1>  TSendDtmfV1Pckg;

	class TSendDtmfV5 : public TSendDtmfV1
	/**
	Contains a SEND DTMF proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifySendDtmfPCmd() for more information about
	the command.

	Request notification for SEND DTMF with RSat::NotifySendDtmfPCmd(), passing
	it an object in which to store the command when it arrives.  Pass a
	packaged version of the  latest "TSendDtmf" class.
	For Symbian OS versions up to and including v8.1a and 8.1b use
	RSat::TSendDtmfV1Pckg, a packaged RSat::TSendDtmfV1.
	For Symbian OS 9.2, use RSat::TSendDtmfV5Pckg.
	For later versions of Symbian OS, check for an updated
	API (RSat::TSendDtmfV6 etc).

	@see RSat::TSendDtmfV1
	@see RSat::TSendDtmfV5Pckg
	@see RSat::NotifySendDtmfPCmd()
	*/
		{
	public:
		IMPORT_C TSendDtmfV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TSendDtmfV5::iAlphaId is displayed. 
		@see RSat::TTextAttribute
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TSendDtmfV5. */
	typedef TPckg<TSendDtmfV5>  TSendDtmfV5Pckg;

	class TSendDtmfV6 : public TSendDtmfV5
	/**
	Contains a SEND DTMF proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifySendDtmfPCmd() for more information about
	the command.

	Request notification for SEND DTMF with RSat::NotifySendDtmfPCmd(), passing
	it an object in which to store the command when it arrives.  Pass a
	packaged version of the  latest "TSendDtmf" class.
	For Symbian OS versions up to and including v8.1a and 8.1b use
	RSat::TSendDtmfV1Pckg, a packaged RSat::TSendDtmfV1.
	For Symbian OS 9.2, use RSat::TSendDtmfV5Pckg.
	For Symbian OS 9.4, use RSat::TSendDtmfV6Pckg.
	For later versions of Symbian OS, check for an updated
	API (RSat::TSendDtmfV7 etc).

	@see RSat::TSendDtmfV1
	@see RSat::TSendDtmfV5
	@see RSat::TSendDtmfV6Pckg
	@see RSat::NotifySendDtmfPCmd()

	*/
		{
	public:
		IMPORT_C TSendDtmfV6();

	public:
		TUint8 iFrameId; //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TSendDtmfV6. */
	typedef TPckg<TSendDtmfV6>  TSendDtmfV6Pckg;
	
    IMPORT_C void NotifySendDtmfPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TSendDtmfRspV1 : public TSatPCmdBase
    /**
    Contains your response to a SEND DTMF proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifySendDtmfPCmd() for more information about
    the command.

    For Symbian OS versions up to and including  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TSendDtmfRspV1Pckg (a packaged TSendDtmfRspV1).
    For later versions of Symbian OS, check for an updated API
    (RSat::TSendDtmfRspV2 etc).

    @see RSat::NotifySendDtmfPCmd()
    */
        {
    public:
        IMPORT_C TSendDtmfRspV1();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TSendDtmfRspV1. */
    typedef TPckg<TSendDtmfRspV1>  TSendDtmfRspV1Pckg;

    //
    // PERFORM CARD APDU
    //
    /**
    @see RSat::TApduData
    */
    enum{KApduDataMaxSize=236};
    typedef TBuf8<KApduDataMaxSize> TApduData;

    struct TCApdu
    /** Part of the PERFORM CARD APDU proactive command.
    This contains the APDU command for the card. */
        {
        TUint8 iCla;    //< Command class
        TUint8 iIns;    //< Command instruction code
        TUint8 iP1;     //< P1 parameter
        TUint8 iP2;     //< P2 parameter
        TUint8 iLc;     //< Length of command data sent by the application layer in a case 3 or 4 Command.
        TApduData iData;//< Data
        TUint8 iLe;     //< Maximum length of data expected by the application layer in response to a case 2 or 4 Command.
        };


    class TPerformCardApduV2 : public TSatPCmdBase
    /**
    Contains a PERFORM CARD APDU proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyPerformCardApduPCmd() for more information about
    the command.

    Request notification for  PERFORM CARD APDU with
    RSat::NotifyPerformCardApduPCmd(), passing it an object in which to store
    the command when it arrives.  Pass a packaged version of the  latest
    "TPerformCardApdu" class. For Symbian OS version from 7.0s to v8.1a and
    8.1b use RSat::TPerformCardApduV2Pckg, a packaged RSat::TPerformCardApduV2.
    For later versions of Symbian OS, check for an updated API
    (RSat::TPerformCardApduV3 etc).

    @see RSat::TPerformCardApduV2Pckg
    @see RSat::NotifyPerformCardApduPCmd()
    */
        {
    public:
        IMPORT_C TPerformCardApduV2();
    public:
        TDeviceId   iDestination;  //< The destination for the command.
        TCApdu iCApdu; //< The APDU command.
        };

    /** A packaged RSat::TPerformCardApduV2. */
    typedef TPckg<TPerformCardApduV2>  TPerformCardApduV2Pckg;

    IMPORT_C void NotifyPerformCardApduPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TPerformCardApduRspV2 : public TSatPCmdBase
    /**
    Contains your response to a  PERFORM CARD APDU proactive command.  Pass
    this to the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifyPerformCardApduPCmd() for more information about the command.

    For Symbian OS versions from 7.0s to v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TPerformCardApduV2Pckg (a packaged
    TPerformCardApduRspV2). For later versions of Symbian OS, check for an
    updated API (RSat::TPerformCardApduRspV3 etc).

    @see RSat::NotifyPerformCardApduPCmd()
    */
        {
    public:
        IMPORT_C TPerformCardApduRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If there a response to the APDU then set this to
        RSat::KRApdu. If something goes wrong, then some error values of
        iGeneralResult require additional information, in which case set this
        to RSat::KMeProblem.  Otherwise set to RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** This should be the card's response to the APDU, is there is one.
        Otherwise, some error values of iGeneralResult require additional
        information, which goes here. */
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TPerformCardApduRspV2. */
    typedef TPckg<TPerformCardApduRspV2>  TPerformCardApduRspV2Pckg;

    //
    // POWER OFF CARD
    //

    class TPowerOffCardV2 : public TSatPCmdBase
    /**
    Contains a POWER OFF CARD proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyPowerOffCardPCmd() for more information about
    the command.

    Request notification for  POWER OFF CARD with RSat::NotifyPerformCardApduPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TPowerOffCard" class. For Symbian OS versions
    from 7.0s to v8.1a and 8.1b use RSat::TPowerOffCardV2Pckg, a packaged
    RSat::TPowerOffCardV2.  For later versions of Symbian OS, check for an updated
    API (RSat::TPowerOffCardV3 etc).

    @see RSat::TPowerOffCardV2Pckg
    @see RSat::NotifyPowerOffCardPCmd()
    */
        {
    public:
        IMPORT_C TPowerOffCardV2();
    public:
        TDeviceId   iDestination;  //< The card whose session should be closed.
        };

    /** A packaged RSat::TPowerOffCardV2. */
    typedef TPckg<TPowerOffCardV2>  TPowerOffCardV2Pckg;

    IMPORT_C void NotifyPowerOffCardPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TPowerOffCardRspV2 : public TSatPCmdBase
    /**
    Contains your response to a POWER OFF CARD proactive command.  Pass
    this to the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifyPowerOffCardPCmd() for more information about the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TPowerOffCardRspV2Pckg (a packaged
    TPowerOffCardRspV2). For later versions of Symbian OS, check for an
    updated API (RSat::TPowerOffCardRspV3 etc).

    @see RSat::NotifyPowerOffCardPCmd()
    */
        {
    public:
        IMPORT_C TPowerOffCardRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TPowerOffCardRspV2. */
    typedef TPckg<TPowerOffCardRspV2>  TPowerOffCardRspV2Pckg;

    //
    // POWER ON CARD
    //


    class TPowerOnCardV2 : public TSatPCmdBase
    /**
    Contains aPOWER ON CARD proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyPowerOnCardPCmd() for more information about
    the command.

    Request notification for POWER ON CARD with RSat::NotifyPerformCardApduPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TPowerOnCard" class. For Symbian OS versions
    from 7.0s to v8.1a and 8.1b use RSat::TPowerOnCardV2Pckg, a packaged
    RSat::TPowerOnCardV2.  For later versions of Symbian OS, check for an updated
    API (RSat::TPowerOnCardV3 etc).

    @see RSat::TPowerOnCardV2Pckg
    @see RSat::NotifyPowerOnCardPCmd()
    */
        {
    public:
        IMPORT_C TPowerOnCardV2();
    public:
        TDeviceId   iDestination;     //< The card to turn on.
        };

    /** A packaged RSat::TPowerOnCardV2. */
    typedef TPckg<TPowerOnCardV2>  TPowerOnCardV2Pckg;

    IMPORT_C void NotifyPowerOnCardPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TPowerOnCardRspV2 : public TSatPCmdBase
    /**
    Contains your response to a POWER ON CARD proactive command.  Pass
    this to the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifyPowerOnCardPCmd() for more information about the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TPowerOnCardRspV2Pckg (a packaged
    TPowerOnCardRspV2). For later versions of Symbian OS, check for an
    updated API (RSat::TPowerOnCardRspV3 etc).

    @see RSat::NotifyPowerOnCardPCmd()
    */
        {
    public:
        IMPORT_C TPowerOnCardRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If there a Answer To Reset (ATR) from the card, set this to
        RSat::KCardATR. If something goes wrong, then some error values of
        iGeneralResult require additional information, in which case set this
        to RSat::KMeProblem.  Otherwise set to RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** This should be the card's Answer To Reset (ATR), is there is one.
        Otherwise, some error values of iGeneralResult require additional
        information, which goes here. */
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TPowerOnCardRspV2. */
    typedef TPckg<TPowerOnCardRspV2>  TPowerOnCardRspV2Pckg;

    //
    // GET READER STATUS
    //

    enum TGetReaderStatusMode
    /** Part of a GET READER STATUS proactive command.
    Describes the information that the UICC is requesting.
    @see RSat::TGetReaderStatusV2 */
        {
        ECardReaderModeNotSet, //< Initialisation value.
        ECardReaderStatus,     //< Return the card reader's status, as described in the "Card Reader Status" object in ETSI TS 102 223.
        ECardReaderIdentifier  //< Return the card reader's identifier, as described in the "Card Reader Identifier" object in ETSI TS 102 223.
        };


    class TGetReaderStatusV2 : public TSatPCmdBase
    /**
    Contains a GET READER STATUS proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyGetReaderStatusPCmd() for more information about
    the command.

    Request notification for GET READER STATUS with RSat::NotifyGetReaderStatusPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TGetReaderStatus" class. For Symbian OS versions
    from 7.0s to v8.1a and 8.1b use RSat::TGetReaderStatusV2Pckg, a packaged
    RSat::TGetReaderStatusV2.  For later versions of Symbian OS, check for an updated
    API (RSat::TGetReaderStatusV2 etc).

    @see RSat::TGetReaderStatusV2Pckg
    @see RSat::NotifyGetReaderStatusPCmd()
    */
        {
    public:
        IMPORT_C TGetReaderStatusV2();
    public:
        /** The card reader whose information is wanted by the UICC. */
        TDeviceId   iDestination;

        /** Describes the information that the UICC is requesting: the card
        reader's identification or the card reader's status.*/
        TGetReaderStatusMode iMode;
        };

    /** A packaged RSat::TGetReaderStatusV2. */
    typedef TPckg<TGetReaderStatusV2>  TGetReaderStatusV2Pckg;

    IMPORT_C void NotifyGetReaderStatusPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TGetReaderStatusRspV2 : public TSatPCmdBase
    /**
    Contains your response to a DISPLAY TEXT proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyGetReaderStatusPCmd() for more information about
    the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TGetReaderStatusRspV2Pckg (a packaged
    TGetReaderStatusRspV2). For later versions of Symbian OS, check for an
    updated API (RSat::TGetReaderStatusRspV3 etc).

    @see RSat::NotifyGetReaderStatusPCmd()
    */
        {
    public:
        IMPORT_C TGetReaderStatusRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If returning card reader information to the UICC, set this to
        RSat::KCardReaderInfo. If something goes wrong, then some error values of
        iGeneralResult require additional information, in which case set this
        to RSat::KMeProblem.  Otherwise set to RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** This should the card reader information, if it could read.
        Otherwise, some error values of iGeneralResult require additional
        information, which goes here. */
        TAdditionalInfo iAdditionalInfo;

        /** Requested mode.  This should be the same as the
        RSat::TGetReaderStatusV2::iMode in the initial GET READER STATUS. */
        TGetReaderStatusMode iMode;
        };

    /** A packaged RSat::TGetReaderStatusRspV2. */
    typedef TPckg<TGetReaderStatusRspV2>  TGetReaderStatusRspV2Pckg;

    //
    // RUN AT COMMAND
    //

	/**
	@see RSat::TAtCommand
	*/
    enum {KAtCommandMaxSize=245};

    /** Part of the RUN AT COMMAND proactive command.
    This contains the AT command string.  Maximum size is 245 bytes.
    @see RSat::TRunAtCommandV2 */
    typedef TBuf8<KAtCommandMaxSize> TAtCommand;

    class TRunAtCommandV2 : public TSatPCmdBase
    /**
    Contains a RUN AT COMMAND proactive command issued by the UICC.   See ETSI
    TS 102 223 and RSat::NotifyRunAtCommandPCmd() for more information about
    the command.  Symbian OS support for this command is available from v7.0s
    onwards.

    Request notification for RUN AT COMMAND with
    RSat::NotifyRunAtCommandPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TRunAtCommand" class. For Symbian OS versions from v7.0s to v8.1a
    and 8.1b, use RSat::TRunAtCommandV2Pckg, a packaged RSat::TRunAtCommandV2.
    For later versions of Symbian OS, check for an updated API
    (RSat::TRunAtCommandV3 etc).

    @see RSat::TRunAtCommandV2Pckg
    @see RSat::NotifyRunAtCommandPCmd()
    */
        {
    public:
        IMPORT_C TRunAtCommandV2();
    public:
        TAlphaId iAlphaId;     //< An optional alpha identifier.
        TAtCommand iAtCommand; //< The AT command string.
        TIconId iIconId;       //< An optional icon.
        };

    /** A packaged RSat::TRunAtCommandV2. */
    typedef TPckg<TRunAtCommandV2>  TRunAtCommandV2Pckg;

	class TRunAtCommandV5 : public TRunAtCommandV2
	/**
	Contains a RUN AT COMMAND proactive command issued by the UICC.
	See ETSI TS 102 223 and RSat::NotifyRunAtCommandPCmd() for more
	information about the command.  Symbian OS support for this command
	is available from v7.0s onwards.

	Request notification for RUN AT COMMAND with
	RSat::NotifyRunAtCommandPCmd(), passing it an object in which to
	store the command when it arrives.  Pass a packaged version of the
	latest "TRunAtCommand" class. For Symbian OS versions from v7.0s
	to v8.1a and 8.1b, use RSat::TRunAtCommandV2Pckg, a packaged RSat::TRunAtCommandV2.
	For Symbian OS 9.2, use RSat::TRunAtCommandV5.
	For later versions of Symbian OS, check for an updated API
	(RSat::TRunAtCommandV6 etc).

	@see RSat::TSatPCmdBase
	@see RSat::TRunAtCommandV2
	@see RSat::TRunAtCommandV5Pckg
	@see RSat::NotifyRunAtCommandPCmd()
	*/
		{
	public:
		IMPORT_C TRunAtCommandV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TRunAtCommandV2::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TRunAtCommandV5. */
	typedef TPckg<TRunAtCommandV5>  TRunAtCommandV5Pckg;

 	class TRunAtCommandV6 : public TRunAtCommandV5
	/**
	Contains a RUN AT COMMAND proactive command issued by the UICC.
	See ETSI TS 102 223 and RSat::NotifyRunAtCommandPCmd() for more
	information about the command.  Symbian OS support for this command
	is available from v7.0s onwards.

	Request notification for RUN AT COMMAND with
	RSat::NotifyRunAtCommandPCmd(), passing it an object in which to
	store the command when it arrives.  Pass a packaged version of the
	latest "TRunAtCommand" class. For Symbian OS versions from v7.0s
	to v8.1a and 8.1b, use RSat::TRunAtCommandV2Pckg, a packaged RSat::TRunAtCommandV2.
	For Symbian OS 9.2, use RSat::TRunAtCommandV5.
	For Symbian OS 9.4, use RSat::TRunAtCommandV6.
	For later versions of Symbian OS, check for an updated API
	(RSat::TRunAtCommandV7 etc).

	@see RSat::TSatPCmdBase
	@see RSat::TRunAtCommandV2
	@see RSat::TRunAtCommandV5
	@see RSat::TRunAtCommandV6Pckg
	@see RSat::NotifyRunAtCommandPCmd()

	*/
		{
	public:
		IMPORT_C TRunAtCommandV6();

	public:
		TUint8 iFrameId; //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TRunAtCommandV6. */
	typedef TPckg<TRunAtCommandV6>  TRunAtCommandV6Pckg;
	
    IMPORT_C void NotifyRunAtCommandPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TRunAtCommandRspV2 : public TSatPCmdBase
    /**
    Contains your response to a RUN AT COMMAND proactive command. This should
    include the AT response string, if there is one.  Pass this to the UICC
    with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifyRunAtCommandPCmd() for more information about the command.

    For Symbian OS versions from v7.0s to v8.1a and v8.1b, pass
    RSat::TerminalRsp() a RSat::TRunAtCommandRspV2Pckg (a packaged
    TRunAtCommandRspV2). For later versions of Symbian OS, check for an
    updated API (RSat::TRunAtCommandRspV3 etc).

    @see RSat::NotifyRunAtCommandPCmd()
    */
        {
    public:
        IMPORT_C TRunAtCommandRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If there is a AT response string then set this to
        RSat::KATResponse. If something goes wrong, then some error values of
        iGeneralResult require additional information, in which case set this
        to RSat::KMeProblem.  Otherwise set to RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** This should be the AT response string, if there is one.
        Otherwise, some error values of iGeneralResult require additional information,
        which goes here. */
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TRunAtCommandRspV2. */
    typedef TPckg<TRunAtCommandRspV2>  TRunAtCommandRspV2Pckg;

    //
    // LANGUAGE NOTIFICATION
    //

    enum TLanguageNotificationType

    /** Part of the LANGUAGE NOTIFICATION proactive command. Determines whether
    the UICC is specifying its current language in iLanguage, or is saying that
    it is not using  a specific language.
    @see RSat::TLanguageNotificationV2 */
        {
        ENotificationTypeNotSet, //<  Initialisation value

        /** No specific language is in use by the UICC. This has the effect of
        cancelling previous specific LANGUAGE NOTIFICATION commands. */
        ENonSpecificLangNotification,

        /** The language currently in use by the UICC is given in
        RSat::TLanguageNotificationV2::iLanguage. */
        ESpecificLangNotification
        };


    class TLanguageNotificationV2 : public TSatPCmdBase
    /**
    Contains a LANGUAGE NOTIFICATION proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyLanguageNotificationPCmd() for more information about
    the command.

    Request notification for LANGUAGE NOTIFICATION with
    RSat::NotifyLanguageNotificationPCmd(), passing it an object in which to
    store the command when it arrives.  Pass a packaged version of the  latest
    "TLanguageNotification" class. For Symbian OS versions up to and including
    v8.1a and 8.1b use RSat::TLanguageNotificationV2Pckg, a packaged
    RSat:TLanguageNotificationV2.  For later versions of Symbian OS, check for
    an updated API (RSat::TLanguageNotificationV3 etc).

    @see RSat::TLanguageNotificationV2Pckg
    @see RSat::NotifyLanguageNotificationPCmd()
    */
        {
    public:
        IMPORT_C TLanguageNotificationV2();
    public:

        /** Describes the LANGUAGE NOTIFICATION command.  Determines whether
        the UICC is specifying its current language in iLanguage, or is saying
        that it is not using  a specific language. */
        TLanguageNotificationType iNotificationType;

        /** Currently used language by (U)ICC.  This is pair of alpha-numeric
        characters, defined in ISO 639. Each alpha-numeric character shall be
        coded on one byte using the SMS default 7-bit coded alphabet as defined
        in TS 123 038 with bit 8 set to 0.

        This is only valid if iNotificationType is set to RSat::ESpecificLangNotification. */
        TUint16 iLanguage;
        };

    /** A packaged RSat::TLanguageNotificationV2. */
    typedef TPckg<TLanguageNotificationV2>  TLanguageNotificationV2Pckg;

    IMPORT_C void NotifyLanguageNotificationPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TLanguageNotificationRspV2 : public TSatPCmdBase
    /**
    Contains your response to a LANGUAGE NITIFICATION proactive command.  Pass
    this to the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifyLanguageNotificationPCmd() for more information about the command.

    For Symbian OS versions from v7.0s to v8.1a and v8.1b, pass
    RSat::TerminalRsp() a RSat::TLanguageNotificationRspV2Pckg (a packaged
    TLanguageNotificationRspV2). For later versions of Symbian OS, check for an
    updated API (RSat::TLanguageNotificationRspV3 etc).

    @see RSat::NotifyLanguageNotificationPCmd()
    */
        {
    public:
        IMPORT_C TLanguageNotificationRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TLanguageNotificationRspV2. */
    typedef TPckg<TLanguageNotificationRspV2>  TLanguageNotificationRspV2Pckg;

    //
    // LAUNCH BROWSER
    //
    enum TBrowserSelection
    /** Part of the LAUNCH BROWSER proactive command.
    It describes the browser to use.
    @see RSat::TLaunchBrowserV2 */
        {
        EBrowserSelectionNotSet,                //< Command details do not specify the browser selection mode.
        ELaunchBrowserIfNotAlreadyLaunched,     //< Launch browser if not already launched.
        EUseExistingBrowser,                    //< Use the existing browser (the browser shall not use the active existing secured session).
        ECloseExistingLaunchNewBrowserSession   //< Close the existing browser session and launch new browser session.
        };

    enum TBrowserIdentity
    /** Part of the LAUNCH BROWSER proactive command.
    @see RSat::TLaunchBrowserV2 */
        {
        EBrowserIdNotSet,      //< Initialisation value.
        EBrowserIdNotPresent,  //< Object not present in the proactive command.
        EDefaultBrowser,       //< Default browser shall be used.
        EBrowserIdNotSpecified, //< Object present but does not identified a known browser ID.
        EWMLBrowser,            //< WML Browser shall be used.
        EHTMLBrowser,           //< HTML Browser shall be used.
        EXHTMLBrowser,          //< XHTML Browser shall be used.
        ECHTMLBrowser		    //< CHTML Browser shall be used.
        };

    enum TBrowserBearer
    /** @see RSat::TBearerList */
        {
        KSmsBearer  =0x00, //< Use SMS.
        KCsdBearer  =0x01, //< Use a circuit switched connection.
        KUssdBearer =0x02, //< Use USSD on GSM/WCDMA networks.
        KGprsBearer =0x03  //< Use a packet switched connection.
        };

	/**
	@see RSat::TUrl
	*/
    enum {KUrlMaxSize=243};

    /** Part of the LAUNCH BROWSER proactive command.
    Contains the URL to display.  Maximum size is 243 bytes.
    @see RSat::TLaunchBrowserV2 */
    typedef TBuf8<KUrlMaxSize> TUrl;

	/**
	@see RSat::TBearerList
	*/
    enum {KBearerListMaxSize=4};

    /** A list containing bearers in order of priority.  Each byte in this
    descriptor will be a value from RSat::TBrowserBearer that describes the
    bearer for the LAUNCH BROWSER command. Use the bearer in byte 1 if
    possible, followed by byte 2 etc.  The maximum number is 4.
    @see RSat::TLaunchBrowserV2 */
    typedef TBuf8<KBearerListMaxSize> TBearerList;

	/**
	@see RSat::TProvisioningFileRef
	*/
    enum {KFileRefMaxSize=251};

    /** A provisioning file, used with the LAUNCH BROWESER proactive command. */
    typedef TBuf8<KFileRefMaxSize> TProvisioningFileRef;

    /** A packaged RSat::TProvisioningFileRef. */
    typedef TPckg<TProvisioningFileRef> TProvisioningFileRefPckg;


    class TLaunchBrowserV2 : public TSatPCmdBase
    /**
    Contains a LAUNCH BROWSER proactive command issued by the UICC.   See ETSI
    TS 102 223 and RSat::NotifyLaunchBrowserPCmd() for more information about
    the command.  Symbian OS support for this command is available from v7.0s
    onwards.

    Request notification for LAUNCH BROWSER with
    RSat::NotifyLaunchBrowserPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the latest
    "TLaunchBrowser" class. For Symbian OS 7.0s, 8.0 and 8.1 use
    RSat::TLaunchBrowserV2Pckg, a packaged RSat::TLaunchBrowserV2.
    For Symbian OS 9.2, use RSat::TLaunchBrowserV5Pckg.  For later
    versions of Symbian OS, check for an updated API
    (RSat::TLaunchBrowserV6 etc).

    @see RSat::TLaunchBrowserV2Pckg
    @see RSat::NotifyLaunchBrowserPCmd()
    */
        {
    public:
        IMPORT_C TLaunchBrowserV2();
    public:
        IMPORT_C TUint NumberOfFileRef() const;
        IMPORT_C TInt GetFileRef (TUint aIndex, TProvisioningFileRef& aFileRef) const;
        IMPORT_C TInt AddFileRef (const TProvisioningFileRef& aFileRef);
        IMPORT_C void ResetFileRef();

        TBrowserSelection iBrowserSel;  //< Describes the browser to use.
        TBrowserIdentity iBrowserId;    //< Browser identity.
        TUrl iUrl;                      //< URL to display.  Maximum size is 243 bytes.
        TBearerList iBearerList;        //< List of bearers that can be used to access the URL. They are in order of priority.
        TTextString iText;              //< Name/identity of the Gateway/Proxy
        TAlphaId iAlphaId;              //< Alpha identifier for user confirmation phase; see RSat::NotifyLaunchBrowserPCmd()
        TIconId iIconId;                //< Icon identifier for user confirmation phase; see RSat::NotifyLaunchBrowserPCmd()
    protected:
        TProvisioningFileRef    iFileRef;                       //< Stored file refs as passed by TSY
        TUint8                  iOffset[KFileRefMaxSize];       //< Offsets of file ref entries in iFileRef
        TUint8                  iLength[KFileRefMaxSize];       //< Lengths of file ref entries in iFileRef
        TInt                    iNumOfFiles;                    //< Number of file refs in iFileRef
        };

    /** A packaged RSat::TLaunchBrowserV2. */
    typedef TPckg<TLaunchBrowserV2>  TLaunchBrowserV2Pckg;

	class TLaunchBrowserV5 : public TLaunchBrowserV2
	/**
	Contains a LAUNCH BROWSER proactive command issued by the UICC.   See ETSI
	TS 102 223 and RSat::NotifyLaunchBrowserPCmd() for more information about
	the command.  Symbian OS support for this command is available from v7.0s
	onwards.

	Request notification for LAUNCH BROWSER with
	RSat::NotifyLaunchBrowserPCmd(), passing it an object in which to store the
	command when it arrives.  Pass a packaged version of the latest
	"TLaunchBrowser" class. For Symbian OS 7.0s, 8.0 and 8.1 use
	RSat::TLaunchBrowserV2Pckg, a packaged RSat::TLaunchBrowserV2.
	For later versions of Symbian OS, check for an updated API
	(RSat::TLaunchBrowserV6 etc).

	@see RSat::TLaunchBrowserV2
	@see RSat::TLaunchBrowserV5Pckg
	@see RSat::NotifyLaunchBrowserPCmd()
	*/
		{
	public:
		IMPORT_C TLaunchBrowserV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TLaunchBrowserV5::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TLaunchBrowserV5. */
	typedef TPckg<TLaunchBrowserV5>  TLaunchBrowserV5Pckg;

	class TLaunchBrowserV6 : public TLaunchBrowserV5
	/**
	Contains a LAUNCH BROWSER proactive command issued by the UICC.   See ETSI
	TS 102 223 and RSat::NotifyLaunchBrowserPCmd() for more information about
	the command.  Symbian OS support for this command is available from v7.0s
	onwards.

	Request notification for LAUNCH BROWSER with
	RSat::NotifyLaunchBrowserPCmd(), passing it an object in which to store the
	command when it arrives.  Pass a packaged version of the latest "TLaunchBrowser" class. 
	For Symbian OS 7.0s, 8.0 and 8.1 use RSat::TLaunchBrowserV2Pckg, a packaged RSat::TLaunchBrowserV2.
	For Symbian OS 9.4 use RSat::TLaunchBrowserV6Pckg.For later versions of Symbian OS, check for an 
	updated API(RSat::TLaunchBrowserV7 etc).

	@see RSat::TLaunchBrowserV2
	@see RSat::TLaunchBrowserV6Pckg
	@see RSat::NotifyLaunchBrowserPCmd()

	*/
		{
	public:
		IMPORT_C TLaunchBrowserV6();

	public:
		TUint8 iFrameId;   //< Each frame has an ID. This field contains the ID of the frame.
		};

	/** A packaged RSat::TLaunchBrowserV6. */
	typedef TPckg<TLaunchBrowserV6>  TLaunchBrowserV6Pckg;
	
    IMPORT_C void GetProvisioningRefFile(TRequestStatus& aReqStatus,const TProvisioningFileRef& aFileRef, TDes8& aFile) const;
    IMPORT_C void NotifyLaunchBrowserPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TLaunchBrowserRspV2 : public TSatPCmdBase
    /**
    Contains your response to a LAUNCH BROWSER proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyLaunchBrowserPCmd() for more information about
    the command.

    For Symbian OS versions from v7.0s to v8.1a and v8.1b, pass
    RSat::TerminalRsp() a RSat::TLaunchBrowserRspV2Pckg (a packaged
    TLaunchBrowserRspV2). For later versions of Symbian OS, check for an
    updated API (RSat::TLaunchBrowserRspV3 etc).

    @see RSat::NotifyLaunchBrowserPCmd()
    */
        {
    public:
        IMPORT_C TLaunchBrowserRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        ETSI TS 102 223 and RSat::KLaunchBrowserError. */
        TPCmdResult iGeneralResult;

        /** Some error values for iGeneralResult (such as
        RSat::KLaunchBrowserError) require additional information, in which
        case set iInfoType to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Some error values of iGeneralResult require additional information,
        which goes here. If iGeneralResult is RSat::KLaunchBrowserError then
        place a value from RSat::TBrowserError in here. */
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TLaunchBrowserRspV2. */
    typedef TPckg<TLaunchBrowserRspV2>  TLaunchBrowserRspV2Pckg;

    //
    // OPEN CHANNEL
    //


    enum TOpenChannelType
    /** Part of an OPEN CHANNEL proactive command. There are three types of OPEN
    CHANNEL command described in ETSI TS 102 223, 3GPP TS 31.111 and 3GPP2
    C.S0035-0.  RSat::TOpenChannelType is set to the type of command.
    @see RSat::TOpenChannelBaseV2
    @see RSat::NotifyOpenChannelPCmd()
    */
        {
        EPCmdTypeNotSet,    	//< OPEN CHANNEL type not set
        ECsBearer,         		//< OPEN CHANNEL proactive command related to Circuit Switched (CS) bearer.
        EGprsBearer,        	//< OPEN CHANNEL proactive command related to Packet Switched bearer.  On GSM/WCDMA networks, this means GPRS.
        ELocalLinksBearer,  	//< OPEN CHANNEL proactive command for local links such as bluetooth and IrDA.
        EAnyBearer,          	//< OPEN CHANNEL proactive comment related to any bearer.
        EUiccServerMode			//< OPEN CHANNEL proactive command related to UICC in server mode.
        };


    enum TLinkEstablishment
    /** Part of an OPEN CHANNEL proactive command. Should the channel be
    opened now, or should you wait until you are given some data to send?
    @see RSat::TOpenChannelBaseV2
    */
        {
        ELinkEstablishmentNotSet,   //< Link establishment not set
        EOnDemand,                  //< On demand link establishment
        EImmediate                  //< Immediate link establishment
        };

    enum TReconnectionMode

    /** Part of an OPEN CHANNEL proactive command. If an attempt to make a
    connection fails, should you try again automatically?
    @see RSat::TOpenChannelBaseV2
    */
        {
        EReconnectionModeNotSet,  //< Reconnection mode not set

        /** Reconnection should not be automatic.  Do not attempt to reconnect. */
        ENoAutomaticReconnection,

        /** Reconnection should be automatic.  Attempt to reconnect if the
        first attempt fails. The OPEN CONNECTION command may include a time
        limit for reconnection attempts. */
        EAutomaticReconnection
        };

    enum TBearerType
    /** Part of an OPEN CHANNEL proactive command. Used by RSat::TBearer.
    Defines the bearer type that you should use to open the channel.
    @see RSat::TOpenChannelBaseV2
    */
        {
        EBearerTypeNotSet,  //< Bearer type not set
        ECSDBearer,         //< Circuit Switched Data bearer
        EGPRSBearer,        //< GPRS bearer
        EDefaultBearer,     //< Default bearer
        EBTBearer,          //< Bluetooth bearer
        EIrDaBearer,        //< Infra red  bearer
        ERS232Bearer,       //< RS232 bearer
        EUSBBearer,         //< USB bearer
        EUtranBearer 		//<Utran bearer
        };

	/**
	@see RSat::TBearerParams
	*/
    enum {KBearerParamsMaxSize=241};

    /** Part of an OPEN CHANNEL proactive command.  Used by RSat::TBearer.
    Contains any parameters necessary to open the channel.
    @see RSat::TOpenChannelBaseV2
    */
    typedef TBuf8<KBearerParamsMaxSize> TBearerParams;

    struct TBearer
    /** Part of an OPEN CHANNEL proactive command. Defines the bearer that you
    should use  to open a channel, plus any parameters necessary.
    @see RSat::TOpenChannelBaseV2
    */
        {
        TBearerType iType;      //< Indicates the type of bearer
        TBearerParams iParams;  //< Params specific to the bearer
        };

    enum TOtherAddressType
    /** Part of an OPEN CHANNEL proactive command.  Used by RSat::TOtherAddress.
    Defines the type of the IP addresses used when opening a channel.
    @see RSat::TOpenChannelBaseV2
    */
        {
        EAddressNotSet,     //< Initialisation value.
        EAddressNotPresent, //< This optional element is not present in this proactive command.
        EIPv4Address,       //< IP address is a IPv4 address
        EIPv6Address        //< IP address is a IPv6 address
        };

	/**
	@see RSat::TOtherAddress
	*/
    enum {KPcktAddressMaxSize=19};

    struct TOtherAddress
    /** Part of an OPEN CHANNEL proactive command.
    Used to define each the addresses needed when opening a channel.
    @see RSat::TOpenChannelBaseV2
    */
        {
        TOtherAddressType iType;                //< Other address type.
        TBuf8<KPcktAddressMaxSize> iAddress;    //< Other address. Maximum size is 19 bytes.
        };

    enum TTransportProtocol
    /** Part of an OPEN CHANNEL proactive command.  Used by RSat::TSimMeInterface.
    Describes the transport protocol to use for the channel.
    @see RSat::TOpenChannelBaseV2
    */
        {
        EProtocolNotSet,        //< Transport protocol not set
        EProtocolNotPresent,    //< Transport protocol details not present in proactive comment
        EUdp,                   //< UDP transport protocol
        ETcp,                   //< TCP transport protocol
        ETcpUiccInServerMode	//< TCP, UICC in server mode
        };

    struct TSimMeInterface
    /** Part of an OPEN CHANNEL proactive command.
    Describes the transport protocol and port number to use for the channel.
    @see RSat::TOpenChannelBaseV2
    */
        {
        TTransportProtocol iTransportProto; //< Transport protocol
        TUint16 iPrtNumber;                 //< Port Number
        };


    class TOpenChannelBaseV2 : public TSatPCmdBase
    /**
    Part of an OPEN CHANNEL proactive command which asks the phone to open a
    channel to a particular bearer.  More description of this command can be
    found in RSat::NotifyOpenChannelPCmd().

    OPEN CHANNEL commands can be of one of a number of types, depending upon 
    the type of bearer required.  RSat::TOpenChannelBaseV2 contains fields
    common to specific OPEN CHANNEL commands.  Other
    classes inherit from this class to provide an OPEN CHANNEL command for
    circuit switched (RSat::TOpenCsChannelV2), packet switched
    (RSat::TOpenGprsChannelV2, local (RSat::TOpenLocalLinksChannelV2) channels
    and a channel for UICC server mode (RSat::TOpenUiccServerModeChannelV7).

    @see RSat::TOpenChannelBaseV2Pckg
    @see RSat::NotifyOpenChannelPCmd()
    */
        {
    public:
        IMPORT_C TOpenChannelBaseV2();
    public:
        /** Determines the type of OPEN CHANNEL command.  Indicates that a local,
        circuit-switched or packet-switched channel should be opened. */
        TOpenChannelType iPCmdType;
        TLinkEstablishment iLinkEst;        //< Link establishment. Should the the channel be opened now, or should you wait until you are given some data to send?
        TReconnectionMode iReconnectionMode;//< Reconnection mode.  If an attempt to make a connection fails, should you try again automatically?
        TAlphaId iAlphaId;                  //< Alpha identifier to display to the user.
        TIconId iIconId;                    //< Identifies the icon to display.
        TBearer iBearer;                    //< Bearer for the channel.
        TUint16 iBufferSize;                //< Buffer size
        TSimMeInterface iSimMeInterface;    //< SIM / ME interface.
        TOtherAddress iDestinationAddress;  //< Destination address.
        };

    /** A packaged RSat::TOpenChannelBaseV2. */
    typedef TPckg<TOpenChannelBaseV2>  TOpenChannelBaseV2Pckg;


    class TOpenCsChannelV2 : public TOpenChannelBaseV2
    /**
    This contains a packet switched OPEN CHANNEL proactive command which asks
    the phone to open a channel to a circuit switched bearer.  All the
    parameters for the channel are supplied in RSat::TOpenCsChannelV2's
    fields and its base class, RSat::TOpenChannelBaseV2.

    Note: Alternative OPEN CHANNEL commands request a packet switched or local
    channel; see RSat::TOpenGprsChannelV2 and RSat::TOpenLocalLinksChannelV2
    respectively.

    More description of the OPEN CHANNEL command can be found in
    RSat::NotifyOpenChannelPCmd().

    @see RSat::NotifyOpenChannelPCmd()
    @see RSat::TOpenChannelBaseV2
    @see RSat::TOpenCsChannelV2Pckg
    @see RSat::TOpenGprsChannelV2
    @see RSat::TOpenLocalLinksChannelV2
    */
        {
    public:
        IMPORT_C TOpenCsChannelV2();
    public:
        TAddress iAddress;          //< Address to connect to
        TSubAddress iSubAddress;    //< Sub-address
        TDuration iDuration1;       //< Duration of reconnection tries
        TDuration iDuration2;       //< Timeout value before the phone releases the link
        TOtherAddress iLocalAddress;//< Local device IP address.  This identifers the phone opening the channel
        TTextString iUserLogin;     //< User login
        TTextString iUserPassword;  //< User password
        };

    /** A packaged RSat::TOpenCsChannelV2. */
    typedef TPckg<TOpenCsChannelV2>  TOpenCsChannelV2Pckg;

	class TOpenCsChannelV5 : public TOpenCsChannelV2
	/**
	This contains a packet switched OPEN CHANNEL proactive command
	which asks the phone to open a channel to a circuit switched
	bearer.  All the parameters for the channel are supplied in
	RSat::TOpenCsChannelV5's fields and its base classes,
	RSat::TOpenCsChannelV2 and RSat::TOpenChannelBaseV2.

	Note: Alternative OPEN CHANNEL commands request a packet
	switched or local channel; see RSat::TOpenGprsChannelV5
	and RSat::TOpenLocalLinksChannelV5 respectively.

	More description of the OPEN CHANNEL command can be found in
	RSat::NotifyOpenChannelPCmd() and ETSI TS 102 223.

	@see RSat::NotifyOpenChannelPCmd()
	@see RSat::TOpenChannelBaseV2
	@see RSat::TOpenCsChannelV2
	@see RSat::TOpenCsChannelV5Pckg
	@see RSat::TOpenGprsChannelV2
	@see RSat::TOpenLocalLinksChannelV2
	*/
		{
	public:
		IMPORT_C TOpenCsChannelV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TOpenChannelBaseV2::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TOpenCsChannelV5. */
	typedef TPckg<TOpenCsChannelV5>  TOpenCsChannelV5Pckg;

	class TOpenCsChannelV6 : public TOpenCsChannelV5
	/**
	This contains a packet switched OPEN CHANNEL proactive command
	which asks the phone to open a channel to a circuit switched
	bearer.  All the parameters for the channel are supplied in
	RSat::TOpenCsChannelV6's fields and its base classes,
	RSat::TOpenCsChannelV5,RSat::TOpenCsChannelV2 and RSat::TOpenChannelBaseV2.

	Note: Alternative OPEN CHANNEL commands request a packet
	switched or local channel; see RSat::TOpenGprsChannelV6
	and RSat::TOpenLocalLinksChannelV6 respectively.

	More description of the OPEN CHANNEL command can be found in
	RSat::NotifyOpenChannelPCmd() and ETSI TS 102 223.

	@see RSat::NotifyOpenChannelPCmd()
	@see RSat::TOpenChannelBaseV2
	@see RSat::TOpenCsChannelV5
	@see RSat::TOpenCsChannelV2
	@see RSat::TOpenCsChannelV6Pckg
	@see RSat::TOpenGprsChannelV6
	@see RSat::TOpenLocalLinksChannelV6

	*/
		{
	public:
		IMPORT_C TOpenCsChannelV6();

	public:
		TUint iFrameId; //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TOpenCsChannelV6. */
	typedef TPckg<TOpenCsChannelV6>  TOpenCsChannelV6Pckg;
	
	/** @see RSat::TAccessName */
    enum {KAccessNameMaxSize=236};
    typedef TBuf8<KAccessNameMaxSize> TAccessName;


    class TOpenGprsChannelV2 : public TOpenChannelBaseV2
    /**
    This contains a packet switched OPEN CHANNEL proactive command which asks
    the phone to open a channel to a packet switched bearer.  All the
    parameters for the channel are supplied in RSat::TOpenGprsChannelV2's
    fields and its base class, RSat::TOpenChannelBaseV2.

    Note: Alternative OPEN CHANNEL commands request a circuit switched or local
    channel; see RSat::TOpenCsChannelV2 and RSat::TOpenLocalLinksChannelV2
    respectively.

    More description of the OPEN CHANNEL command can be found in
    RSat::NotifyOpenChannelPCmd().

    @see RSat::NotifyOpenChannelPCmd()
    @see RSat::TOpenGprsChannelV2Pckg
    @see RSat::TOpenChannelBaseV2
    @see RSat::TOpenCsChannelV2
    @see RSat::TOpenLocalLinksChannelV2
    */
        {
    public:
        IMPORT_C TOpenGprsChannelV2();
    public:
        TAccessName iAccessName;		//< Access Point Name indentifying the Gateway GSN
        TOtherAddress iLocalAddress; 	//< Local device IP address
        };

    /** A packaged RSat::TOpenGprsChannelV2. */
    typedef TPckg<TOpenGprsChannelV2>  TOpenGprsChannelV2Pckg;

	class TOpenGprsChannelV4 : public TOpenGprsChannelV2
	/**
	This class is derived from TOpenGprsChannelV2 and extends the class's functionality
	by adding username and password support.

	@see RSat::TOpenGprsChannelV2
	@see RSat::TOpenGprsChannelV4Pckg
	*/
		{
	public:
		IMPORT_C TOpenGprsChannelV4();
	public:
		/** The user login to use to open the channel */
		TTextString iUserLogin;

		/** The password to use to open the channel */
		TTextString iUserPassword;
		};

    /** A packaged RSat::TOpenGprsChannelV4. */
    typedef TPckg<TOpenGprsChannelV4>  TOpenGprsChannelV4Pckg;

	class TOpenGprsChannelV5 : public TOpenGprsChannelV4
	/**
	This contains a packet switched OPEN CHANNEL proactive command
	which asks the phone to open a channel to a packet switched
	bearer.  All the parameters for the channel are supplied in
	RSat::TOpenGprsChannelV5's fields and its base classes,
	RSat::TOpenGprsChannelV4, RSat::TOpenGprsChannelV2 and
	RSat::TOpenChannelBaseV2.

	This class is derived from TOpenGprsChannelV4 and extends the
	class's functionality by adding text attribute support for
	the alpha identifier.

	Note: Alternative OPEN CHANNEL commands request a circuit
	switched or local channel; see RSat::TOpenCsChannelV5 and
	RSat::TOpenLocalLinksChannelV5 respectively.

	More description of the OPEN CHANNEL command can be found
	in RSat::NotifyOpenChannelPCmd().

	@see RSat::TOpenChannelBaseV2
	@see RSat::TOpenGprsChannelV2
	@see RSat::TOpenGprsChannelV4
	@see RSat::TOpenGprsChannelV5Pckg
	@see RSat::TOpenCsChannelV5
	@see RSat::TOpenLocalLinksChannelV5
	*/
		{
	public:
		IMPORT_C TOpenGprsChannelV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TOpenChannelBaseV2::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TOpenGprsChannelV5. */
	typedef TPckg<TOpenGprsChannelV5>  TOpenGprsChannelV5Pckg;

	class TOpenGprsChannelV6 : public TOpenGprsChannelV5
	/**
	This contains a packet switched OPEN CHANNEL proactive command
	which asks the phone to open a channel to a packet switched
	bearer.  All the parameters for the channel are supplied in
	RSat::TOpenGprsChannelV6's fields and its base classes,
	RSat::TOpenGprsChannelV5, RSat::TOpenGprsChannelV4, RSat::TOpenGprsChannelV2 and
	RSat::TOpenChannelBaseV2.

	This class is derived from TOpenGprsChannelV5 and extends the
	class's functionality by adding Frame identifier support.
	
	Note: Alternative OPEN CHANNEL commands request a circuit
	switched or local channel; see RSat::TOpenCsChannelV6 and
	RSat::TOpenLocalLinksChannelV6 respectively.

	More description of the OPEN CHANNEL command can be found
	in RSat::NotifyOpenChannelPCmd().

	@see RSat::TOpenChannelBaseV2
	@see RSat::TOpenGprsChannelV2
	@see RSat::TOpenGprsChannelV4
	@see RSat::TOpenGprsChannelV5
	@see RSat::TOpenGprsChannelV6Pckg
	@see RSat::TOpenCsChannelV6
	@see RSat::TOpenLocalLinksChannelV6
	*/
		{
	public:
		IMPORT_C TOpenGprsChannelV6();

	public:
		TUint iFrameId; //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TOpenGprsChannelV6. */
	typedef TPckg<TOpenGprsChannelV6>  TOpenGprsChannelV6Pckg;

    /**
    This contains a packet switched OPEN CHANNEL proactive command which asks
    the phone to open a channel to a local bearer such as Bluetooth or Irda.  All the
    parameters for the channel are supplied in RSat::TOpenLocalLinksChannelV2's
    fields and its base class, RSat::TOpenChannelBaseV2.
    Note: Alternative OPEN CHANNEL commands request a circuit switched or packet
    switched channel; see RSat::TOpenCsChannelV2 and RSat::TOpenGprsChannelV2
    respectively.

    More description of the OPEN CHANNEL command can be found in
    RSat::NotifyOpenChannelPCmd().

    @see RSat::NotifyOpenChannelPCmd()
    @see RSat::TOpenLocalLinksChannelV2Pckg
    @see RSat::TOpenChannelBaseV2
    @see RSat::TOpenGprsChannelV2
    @see RSat::TOpenCsChannelV2
    */
    class TOpenLocalLinksChannelV2 : public TOpenChannelBaseV2
        {
    public:
        IMPORT_C TOpenLocalLinksChannelV2();
    public:
        TDuration iDuration1;               //< Duration of reconnection tries
        TDuration iDuration2;               //< Timeout value before the ME releases the link
        TTextString iUserPassword;          //< User password
        TRemoteEntityAddress iRemoteAddress;//< Remote entity address
        };

    /** A packaged RSat::TOpenLocalLinksChannelV2. */
    typedef TPckg<TOpenLocalLinksChannelV2>  TOpenLocalLinksChannelV2Pckg;

	class TOpenLocalLinksChannelV5 : public TOpenLocalLinksChannelV2
	/**
	This contains a packet switched OPEN CHANNEL proactive command which asks
	the phone to open a channel to a local bearer such as Bluetooth or Irda.
	All the parameters for the channel are supplied in
	RSat::TOpenLocalLinksChannelV5's fields and its base classes,
	RSat::TOpenLocalLinksChannelV2 and RSat::TOpenChannelBaseV2.

	Note: Alternative OPEN CHANNEL commands request a circuit switched or packet
	switched channel; see RSat::TOpenCsChannelV5 and RSat::TOpenGprsChannelV5
	respectively.

	More description of the OPEN CHANNEL command can be found in
	RSat::NotifyOpenChannelPCmd().

	@see RSat::NotifyOpenChannelPCmd()
	@see RSat::TOpenLocalLinksChannelV5Pckg
	@see RSat::TOpenChannelBaseV2
	@see RSat::TOpenGprsChannelV5
	@see RSat::TOpenCsChannelV5
   	*/
		{
	public:
		IMPORT_C TOpenLocalLinksChannelV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TOpenChannelBaseV2::iAlphaId is displayed. 
		@see RSat::TTextAttribute
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TOpenLocalLinksChannelV5. */
	typedef TPckg<TOpenLocalLinksChannelV5>  TOpenLocalLinksChannelV5Pckg;

	class TOpenLocalLinksChannelV6 : public TOpenLocalLinksChannelV5
	/**
	This contains a packet switched OPEN CHANNEL proactive command which asks
	the phone to open a channel to a local bearer such as Bluetooth or Irda.
	All the parameters for the channel are supplied in
	RSat::TOpenLocalLinksChannelV6's fields and its base classes,
	RSat::TOpenLocalLinksChannelV5,RSat::TOpenLocalLinksChannelV2 and RSat::TOpenChannelBaseV2.

	Note: Alternative OPEN CHANNEL commands request a circuit switched or packet
	switched channel; see RSat::TOpenCsChannelV6 and RSat::TOpenGprsChannelV6
	respectively.

	More description of the OPEN CHANNEL command can be found in
	RSat::NotifyOpenChannelPCmd().

	@see RSat::NotifyOpenChannelPCmd()
	@see RSat::TOpenLocalLinksChannelV6Pckg
	@see RSat::TOpenChannelBaseV2
	@see RSat::TOpenGprsChannelV6
	@see RSat::TOpenCsChannelV6
   	*/
		{
	public:
		IMPORT_C TOpenLocalLinksChannelV6();

	public:
		TUint8 iFrameId; //< Each frame has an ID. This field contains the ID of the frame.
		};

	/** A packaged RSat::TOpenLocalLinksChannelV6. */
	typedef TPckg<TOpenLocalLinksChannelV6>  TOpenLocalLinksChannelV6Pckg;
	
	class TOpenUiccServerModeChannelV7 : public TOpenChannelBaseV2
	/**
	This contains an OPEN CHANNEL proactive command which asks
	the phone to open a channel when the UICC is in server mode.
	
	All the parameters for the channel are supplied in the fields of
	RSat::TOpenUiccServerModeChannelV7 and its base class
	RSat::TOpenChannelBaseV2.  A number of fields inherited by this class
	from RSat::TOpenChannelBaseV2 are not relevant to this proactive command
	and should be set to the following values:

	 - RSat::TOpenChannelBaseV2::iLinkEst should be set to
	   RSat::ELinkEstablishmentNotSet.
	 - RSat::TOpenChannelBaseV2::iReconnectionMode should be set to 
	   RSat::EReconnectionModeNotSet. 
	 - RSat::TBearer::iType in RSat::TOpenChannelBaseV2::iBearer should
	   be set to RSat::EBearerTypeNotSet.
	 - RSat::TOtherAddress::iType in RSat::TOpenChannelBaseV2::iDestinationAddress
	   should be set to RSat::EAddressNotSet.
	 - RSat::TSimMeInterface::iTransportProto in RSat::TOpenChannelBaseV2::iSimMeInterface
	   should be set to RSat::EProtocolNotSet.
	
	More details of the OPEN CHANNEL command can be found in
	RSat::NotifyOpenChannelPCmd().

	@see RSat::NotifyOpenChannelPCmd()
	@see RSat::TOpenUiccServerModeChannelV7Pckg
	@see RSat::TOpenChannelBaseV2
	@see RSat::TOpenLocalLinksChannelV6
	@see RSat::TOpenGprsChannelV6
	@see RSat::TOpenCsChannelV6
   	*/
		{
	public:
		IMPORT_C TOpenUiccServerModeChannelV7();

	public:
		/** Contains formatting information to be applied when
		RSat::TOpenChannelBaseV2::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		/** Each frame has an ID. This field contains the ID of the frame. */
		TUint8 iFrameId;
		};

	/** A packaged RSat::TOpenUiccServerModeChannelV7. */
	typedef TPckg<TOpenUiccServerModeChannelV7>  TOpenUiccServerModeChannelV7Pckg;
	
    IMPORT_C void NotifyOpenChannelPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;
    IMPORT_C void GetOpenChannelPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

	/**
	 *  TElementaryFile specifies the EFs that can be modified over the air by the
	 *  network or by the (U)SIM application toolkit on the (U)ICC. This is provided
	 *  in this API for ease of use.
	 *
	 *  Files that can not be modified are not mentioned in the following constants.
	 *  Other files that do not have a specified file number are not mentioned either
	 *  (mostly the file starting with 0x4F).
	 *
	 *  It is assumed that unless stated otherwise the following file ids are valid
	 *  for both GSM /WCDMA ICCs and CDMA RUIM. It has not been possible to define
	 *  which of the CDMA EFs can be refreshed. 
	 *
	 *  The enum values of TElementaryFiles are derived directly from ETSI 11.14,
	 *  3GPP TS 31.102 (Annex A) & 3GPP TS 31.103 (Annex A).
	 */
	enum TElementaryFiles
		{
		//
		// Commented out EFs cannot be modified over the air and are only included
		// to prevent people added them by mistake.
		//
		// There are already a number of EFs listed that cannot be modified over the
		// air: KCsp1Ef, KAdnEf, KCcpEf, KLndEf, KExt1Ef, KNiaEf, KLociGprsEf,
		// KRplmnActEf, KCsp2Ef, KPhaseEf, KGmsi, KHiddenKeyEf & KPnnEf.
		//
		KUnknownEf			=0x2000,	//< Unknown EF type
		KDirEf				=0x2F00,	//< Application directory EF
		KElpEf				=0x2F05,	//< Extended Language Preference EF
		KArrEf				=0x2F06,	//< Access Rule Reference EF at the MF level.
		//KIccId			=0x2FE2,	//< ICC identification	EF (operator dependant)
		KImgEf				=0x4F20,	//< Image EF
		KPscEf				=0x4F22,	//< Phonebook synchronisation counter EF
		KCcEf				=0x4F23,	//< Change counter EF
		KPuidEf				=0x4F24,	//< Previous unique identifier EF
		KPbr				=0x4F30,	//< Phonebook reference file EF
		//KSaiEf			=0x4F30,	//< SoLSA Access Indicator EF (clashes with KPbr)
		KSllEf				=0x4F31, 	//< SoLSA LSA List EF
		//KMexestEf			=0x4F40, 	//< MExE Service table EF
		//KOrpkEf			=0x4F41, 	//< Operator Root Public Key EF
		//KArpkEf			=0x4F42, 	//< Administrator Root Public Key EF
		//KTprpkEf			=0x4F43, 	//< Third Party Root Public Key EF
		KMmlEf			    =0x4F47, 	//< Multimedia Messages List EF
		KMmdfEf		        =0x4F48, 	//< Multimedia Messages Data File EF
		//KKcgprsEf			=0x4F52, 	//< GPRS Ciphering key KcGPRS EF
		//KCpbcchEf			=0x4F63, 	//< CPBCCH Information EF
		KInvScan			=0x4F64,	//< Investigation File EF
		KImpiEf				=0x6F02,	//< IMS private user identity EF
		KImpuEf				=0x6F03,	//< IMS public user identity EF (Note: TS 31.103 defines this as 6F04)
		KDomainEf			=0x6F04,	//< Home Network Domain Name EF (Note: TS 31.103 defines this as 6F03)
		KLpEf				=0x6F05,	//< Language Preference EF
		KArradEf			=0x6F06,	//< Access rule reference (under ADFUSIM and DFTELECOM) EF
		KImsiEf				=0x6F07,	//< IMSI EF
		//KKeysEf			=0x6F08, 	//< Ciphering and Integrity Keys EF
		//KKeysPsEf			=0x6F09, 	//< Ciphering and Integrity Keys for Packet Switched domain EF
		KCsp1Ef				=0x6F15,	//< CSP 1 EF (7F20/21 6F15)
		KDckEf				=0x6F2C,	//< De-personalization Control Keys EF
		KPlmnSelEf			=0x6F30,	//< PLMN Selection EF
		KHplmnEf			=0x6F31,	//< Higher Priority PLMN search period EF
		KCnlEf				=0x6F32,	//< Co-operative Network List EF
		KAcmMaxEf			=0x6F37,	//< ACM max value EF
		KSstEf				=0x6F38,	//< SIM Service Table EF
		KAcmEf				=0x6F39,	//< Accumulated Call Meter EF
		KAdnEf				=0x6F3A,	//< Abbreviated Dialling Numbers EF
		KFdnEf				=0x6F3B,	//< Fixed Diallind Numbers EF
		KSmsEf				=0x6F3C,	//< Short Messages EF
		KCcpEf				=0x6F3D,	//< Capability Configuration Parameters EF
		KGid1Ef				=0x6F3E,	//< Group Identifier Level 1 EF
		KGid2Ef				=0x6F3F,	//< Group Identifier Level 2 EF
		KMsisdnEf			=0x6F40,	//< MS ISDN EF
		KPuctEf				=0x6F41,	//< Price per Unit and Currency Table EF
		KSmspEf				=0x6F42,	//< SMS Parameters EF
		KSmssEf				=0x6F43,	//< SMS Status EF
		KLndEf				=0x6F44,	//< Last Number Dialled EF
		KCbmiEf				=0x6F45,	//< Cell Broadcast Message Identifier
		KSpnEf				=0x6F46,	//< Service Provider Name EF
		KSmsrEf				=0x6F47,	//< SMS reports EF
		KCbmidEf			=0x6F48,	//< CBMI for Data Download EF
		KSdnEf				=0x6F49,	//< Service Dialling Numbers EF
		KExt1Ef				=0x6F4A,	//< Extension 1 EF
		KExt2Ef				=0x6F4B,	//< Extension 2 EF
		KExt3Ef				=0x6F4C,	//< Extension 3 EF
		KBdnEf				=0x6F4D,	//< Barred Dialling Numbers EF
		KExt5Ef				=0x6F4E,	//< Extension 5 EF
		KCcp2Ef				=0x6F4F,	//< Capability configuration parameters 2 EF
		KCbmirEf			=0x6F50,	//< CBMI Range Selection EF
		KNiaEf				=0x6F51,	//< Network's Indication of Alerting EF (upto Rel 99)
		KLociGprsEf			=0x6F53,	//< GPRS Location Information EF
		KSumeEf				=0x6F54,	//< SetUpMenu Elements EF
		KExt4Ef				=0x6F55,	//< Extension 4 EF
		KEstEf				=0x6F56,	//< Enabled services table EF
		KAclEf				=0x6F57,	//< Access point name control list EF
		KCmiEf				=0x6F58,	//< Comparison method information EF
		KStartHfnEf			=0x6F5B,	//< Initialisation value for Hyperframe number EF
		KThresholdEf		=0x6F5C,	//< Maximum value of START EF
		//KUPlmnwAcTEf		=0x6F60, 	//< User controlled PLMN selector with Access Technology EF
		KOPlmnwAcTEf		=0x6F61,	//< Operator controlled PLMN selector with Access Technology EF
		KHPlmnwAcTEf		=0x6F62,	//< HPLMN selector with Access Technology EF
		KRplmnActEf			=0x6F65,	//< RPLMN Last used Access Technology EF
		KPsLociEf			=0x6F73,	//< Packet switched location information EF
		KAccEf				=0x6F78,	//< Access Control Class EF
		KFPlmnEf			=0x6F7B,	//< Forbidden PLMN EF
		//KLociEf			=0x6F7E, 	//< Location Information EF
		KIciEf				=0x6F80,	//< Incoming call information EF
		KOciEf				=0x6F81,	//< Outgoing call information EF
		KIctEf				=0x6F82,	//< Incoming call timer EF
		KOctEf				=0x6F83,	//< Outgoing call timer EF
		KCsp2Ef				=0x6F98,	//< CSP 2 EF (7F40 6F98)
		KAdEf				=0x6FAD,	//< Administrative Data EF
		KPhaseEf			=0x6FAE,	//< Phase Identification EF
		KVcgsEf				=0x6FB1,	//< Voice Group Call Service EF
		KVgcssEf			=0x6FB2,	//< Voice Group Call Service Status EF
		KVbsEf				=0x6FB3,	//< Voice Broadcast Service EF
		KVbssEf				=0x6FB4,	//< Voice Broadcast Service Status EF
		KeMlppEf			=0x6FB5,	//< enhanced Multi Level Pre-emption and Priority EF
		KAaemef				=0x6FB6,	//< Automatic Answer for eMLLP Service EF
		KEccEf				=0x6FB7,	//< Emergency Call Codes EF
		KGmsi				=0x6FC2,	//< Group Identity EF
		KHiddenKeyEf        =0x6FC3,	//< Key for hidden phone book entries EF
		//KNetpEf			=0x6FC4, 	//< Network Parameters EF
		KPnnEf				=0x6FC5,	//< PLMN Network Name EF
		KOplEf				=0x6FC6,	//< Operator Network List EF
		KMbdnEf				=0x6FC7,	//< Mailbox Dialling Numbers EF
		KExt6Ef				=0x6FC8,	//< Extension 6 EF
		KMbiEf				=0x6FC9,	//< Mailbox Identifier EF
		KMwisEf				=0x6FCA,	//< Message Waiting Indication Status EF
		KCfisEf				=0x6FCB,	//< Call Forwarding Indication Status EF
		KExt7Ef				=0x6FCC,	//< Extension 7 EF
		KSpdiEf				=0x6FCD,	//< Service provider display information EF
		KMmsnEF				=0x6FCE,	//< MMS notification EF
		KExt8Ef				=0x6FCF,	//< Extension 8 EF
		KMmsicpEf			=0x6FD0,	//< MMS issuer connectivity parameters EF
		KMmsupEf			=0x6FD1,	//< MMS user preferences EF
		KMmsucpEf			=0x6FD2, 	//< MMS user connectivity parameters EF
		KNiaRel5Ef			=0x6FD3,	//< Network's Indication of Alerting EF (from Rel 5 onwards)
		KVgcscaEf			=0x6FD4,    //< Voice Group Call Service Ciphering Algorithm EF
        KVbscaEf            =0x6FD5,	//< Voice Broadcast Service Ciphering Algorithm EF
        KGbabpEf            =0x6FD6,	//< GBA Bootstrapping parameters EF
        KMsklEf          =0x6FD7,	//< MBMS Service Keys List EF
        KMukEf           =0x6FD8,	//< MBMS User Key EF
		KGbanlEf          =0x6FDA,	//< GBA NAF List EF
		};


	//
	//REFRESH REQUIRED
	//
	IMPORT_C void NotifyRefreshRequired(TRequestStatus& aReqStatus) const;
	IMPORT_C void NotifyRefreshRequired(TRequestStatus& aReqStatus, TDes8& aPCmd) const;
	IMPORT_C void RefreshAllowed(TRequestStatus& aReqStatus, const TDesC8& aRefreshAllowedRsp) const;

    class TOpenChannelRspV2 : public TSatPCmdBase
    /**
    Contains your response to a OPEN CHANNEL proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyOpenChannelPCmd() for more information about
    the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TOpenChannelRspV2Pckg (a packaged
    TOpenChannelRspV2). For later versions of Symbian OS, check for an updated
    API (RSat::TOpenChannelRspV3 etc).

    @see RSat::NotifyOpenChannelPCmd()
    */
        {
    public:
        IMPORT_C TOpenChannelRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If you successfully opened a channel, then return the channel's number and
        status in iAdditionalInfo and set iInfoType to
        RSat::KChannelStatusInfo. Alternatively, some error values of
        iGeneralResult require additional information, in which case set this
        to RSat::KMeProblem.  Otherwise set to RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** If you successfully opened a channel, you need to give the channel
        a number from 1 to 7.  Return this number plus the channel's status in
        iAdditionalInfo.  The format is described in  ETSI TS 102 223 section
        8.56.  See RSat::NotifyOpenChannelPCmd() for more information about
        channel numbers.

        Alternatively, some error values of iGeneralResult require additional
        information, which goes here. */
        TAdditionalInfo iAdditionalInfo;

        /** The RSat::TBearer from the OPEN CHANNEL command to which you are responding. */
        TBearer iBearer;

        /** The buffer size from the OPEN CHANNEL command to which you are responding.*/
        TUint16 iBufferSize;
        };

    /** A packaged RSat::TOpenChannelRspV2. */
    typedef TPckg<TOpenChannelRspV2>  TOpenChannelRspV2Pckg;

	//
	// CLOSE CHANNEL
	//
    class TCloseChannelV2 : public TSatPCmdBase
    /**
    This contains a CLOSE CHANNEL proactive command which asks the phone to
    close a channel previously open with an OPEN CHANNEL command.     See ETSI
    TS 102 223 and RSat::NotifyCloseChannelPCmd() for more information about the
    command.
    Request notification for CLOSE CHANNEL with
    RSat::NotifyCloseChannelPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TCloseChannel" class. For Symbian OS versions from 7.0s to v8.1a and
    8.1b use RSat::TCloseChannelV2Pckg, a packaged RSat::TCloseChannelV2.  For later
    versions of Symbian OS, check for an updated API (RSat::TCloseChannelV3 etc).

    @see RSat::TCloseChannelV2Pckg
    @see RSat::NotifyCloseChannelPCmd()
    */
        {
    public:
        IMPORT_C TCloseChannelV2();
    public:
        TDeviceId iDestination; //< Specify which channel to close.
        TAlphaId iAlphaId;      //< Alpha identifier containg text to display to the user.
        TIconId iIconId;        //< Identifies the icon to display.
        };

    /** A packaged RSat::TCloseChannelV2. */
    typedef TPckg<TCloseChannelV2>  TCloseChannelV2Pckg;

	class TCloseChannelV5 : public TCloseChannelV2
	/**
	This contains a CLOSE CHANNEL proactive command which asks the phone to
	close a channel previously open with an OPEN CHANNEL command.     See ETSI
	TS 102 223 and RSat::NotifyCloseChannelPCmd() for more information about the
	command.
	Request notification for CLOSE CHANNEL with
	RSat::NotifyCloseChannelPCmd(), passing it an object in which to store the
	command when it arrives.  Pass a packaged version of the latest
	"TCloseChannel" class. For Symbian OS versions from 7.0s to v8.1a and
	8.1b use RSat::TCloseChannelV2Pckg, a packaged RSat::TCloseChannelV2.
	For Symbian OS 9.2, use RSat::TCloseChannelV5Pckg.
	For later versions of Symbian OS, check for an updated API
	(RSat::TCloseChannelV6 etc).

	@see RSat::TCloseChannelV2
	@see RSat::TCloseChannelV5Pckg
	@see RSat::NotifyCloseChannelPCmd()
	*/
		{
	public:
		IMPORT_C TCloseChannelV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TCloseChannelV2::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TCloseChannelV5. */
	typedef TPckg<TCloseChannelV5>  TCloseChannelV5Pckg;

	class TCloseChannelV6 : public TCloseChannelV5
	/**
	This contains a CLOSE CHANNEL proactive command which asks the phone to
	close a channel previously open with an OPEN CHANNEL command.     See ETSI
	TS 102 223 and RSat::NotifyCloseChannelPCmd() for more information about the
	command.
	Request notification for CLOSE CHANNEL with
	RSat::NotifyCloseChannelPCmd(), passing it an object in which to store the
	command when it arrives.  Pass a packaged version of the latest
	"TCloseChannel" class. For Symbian OS versions from 7.0s to v8.1a and
	8.1b use RSat::TCloseChannelV2Pckg, a packaged RSat::TCloseChannelV2.
	For Symbian OS 9.2, use RSat::TCloseChannelV5Pckg.
	For Symbian OS 9.4, use RSat::TCloseChannelV6Pckg.
	For later versions of Symbian OS, check for an updated API
	(RSat::TCloseChannelV7 etc).

	@see RSat::TCloseChannelV5
	@see RSat::TCloseChannelV6Pckg
	@see RSat::NotifyCloseChannelPCmd()
	*/
		{
	public:
		IMPORT_C TCloseChannelV6();

	public:
		TUint8 iFrameId; //< Each frame has an ID. This field contains the ID of the frame.
		};

	/** A packaged RSat::TCloseChannelV6. */
	typedef TPckg<TCloseChannelV6>  TCloseChannelV6Pckg;
	
    IMPORT_C void NotifyCloseChannelPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TCloseChannelRspV2 : public TSatPCmdBase
    /**
    Contains your response to a CLOSE CHANNEL proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyCloseChannelPCmd() for more information about
    the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TCloseChannelRspV2Pckg (a packaged
    TCloseChannelRspV2). For later versions of Symbian OS, check for an updated
    API (RSat::TCloseChannelRspV3 etc).

    @see RSat::NotifyCloseChannelPCmd()
    */
        {
    public:
        IMPORT_C TCloseChannelRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TCloseChannelRspV2. */
    typedef TPckg<TCloseChannelRspV2>  TCloseChannelRspV2Pckg;

    //
    // RECEIVE DATA
    //

    class TReceiveDataV2 : public TSatPCmdBase
    /**
    Contains a RECEIVE DATA proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyReceiveDataPCmd() for more information about
    the command.

    Request notification for RECEIVE DATA with RSat::NotifyReceiveDataPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TReceiveData" class. For Symbian OS versions
    up to and including v8.1a and 8.1b use RSat::TReceiveDataV1Pckg, a packaged
    RSat::TReceiveDataV1.  For later versions of Symbian OS, check for an updated
    API (RSat::TReceiveDataV2 etc).

    @see RSat::TReceiveDataV2Pckg
    @see RSat::NotifyReceiveDataPCmd()
    */
        {
    public:
        IMPORT_C TReceiveDataV2();
    public:
        TDeviceId iDestination;     //< Specify which channel on which to receive data.
        TAlphaId iAlphaId;          //< Alpha identifier containing text to display to the user.
        TIconId iIconId;            //< Identifies the icon to display.
        TUint8 iChannelDataLength;  //< Number of bytes that are requested.
        };

    /** A packaged RSat::TReceiveDataV2. */
    typedef TPckg<TReceiveDataV2>  TReceiveDataV2Pckg;

	class TReceiveDataV5 : public TReceiveDataV2
	/**
	Contains a RECEIVE DATA proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifyReceiveDataPCmd() for more information about
	the command.

	Request notification for RECEIVE DATA with RSat::NotifyReceiveDataPCmd(), passing
	it an object in which to store the command when it arrives.  Pass a
	packaged version of the  latest "TReceiveData" class. For Symbian OS versions
	up to and including v8.1a and 8.1b use RSat::TReceiveDataV1Pckg, a packaged
	RSat::TReceiveDataV1.
	For Symbian OS 9.2, use RSat::TReceiveDataV5Pckg. For later versions of
	Symbian OS,	check for an updated API (RSat::TReceiveDataV5 etc).

	@see RSat::TReceiveDataV2
	@see RSat::TReceiveDataV5Pckg
   	@see RSat::NotifyReceiveDataPCmd()
	*/
		{
	public:
		IMPORT_C TReceiveDataV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TReceiveDataV2::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TReceiveDataV5. */
	typedef TPckg<TReceiveDataV5>  TReceiveDataV5Pckg;

	class TReceiveDataV6 : public TReceiveDataV5
	/**
	Contains a RECEIVE DATA proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifyReceiveDataPCmd() for more information about
	the command.

	Request notification for RECEIVE DATA with RSat::NotifyReceiveDataPCmd(), passing
	it an object in which to store the command when it arrives.  Pass a
	packaged version of the  latest "TReceiveData" class. For Symbian OS versions
	up to and including v8.1a and 8.1b use RSat::TReceiveDataV1Pckg, a packaged
	RSat::TReceiveDataV1.
	For Symbian OS 9.2, use RSat::TReceiveDataV5Pckg.
	For Symbian OS 9.4, use RSat::TReceiveDataV6Pckg.For later versions of
	Symbian OS,	check for an updated API (RSat::TReceiveDataV7 etc).

	@see RSat::TReceiveDataV5
	@see RSat::TReceiveDataV6Pckg
   	@see RSat::NotifyReceiveDataPCmd()
   	*/
		{
	public:
		IMPORT_C TReceiveDataV6();

	public:
		TUint8 iFrameId; //<  Each frame has an ID. This field contains the ID of the frame.
		};

	/** A packaged RSat::TReceiveDataV6. */
	typedef TPckg<TReceiveDataV6>  TReceiveDataV6Pckg;
	
    IMPORT_C void NotifyReceiveDataPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TReceiveDataRspV2 : public TSatPCmdBase
    /**
    Contains your response to a RECEIVE DATA proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyReceiveDataPCmd() for more information about
    the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TReceiveDataRspV2Pckg (a packaged
    TReceiveDataRspV2). For later versions of Symbian OS, check for an updated
    API (RSat::TReceiveDataRspV3 etc).

    @see RSat::NotifyReceiveDataPCmd()
    */
        {
    public:
        IMPORT_C TReceiveDataRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If you are returning data to the UICC then set this to
        RSat::KChannelData.   Some error values of iGeneralResult require
        additional information, in which case set this to RSat::KMeProblem.
        Otherwise set to RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains the data that you are returning to the UICC.
        Alternatively, some error values of iGeneralResult require additional
        information, which goes here. */
        TAdditionalInfo iAdditionalInfo;

        TUint8 iChannelDataLength;
        };

    /** A packaged RSat::TReceiveDataRspV2. */
    typedef TPckg<TReceiveDataRspV2>  TReceiveDataRspV2Pckg;

    //
    // SEND DATA
    //

    enum TSendDataMode
    /** Part of a SEND DATA proactive command.  Describes the way in which data
    is send: should the SEND DATA command's data be sent immediately, or should
    the data from a series of SEND DATA command be buffered by the phone and
    sent at whatever rate the phone considers to be the best.

    @see RSat::TSendDataV2
    @see RSat::NotifySendDataPCmd() */
        {
        ESendDataModeNotSet,
        EStoreDataInTxBuffer,
        ESendDataImmediately
        };

	/** @see RSat::TChannelData */
    enum {KChannelDataMaxSize=245};

    /** Part of a SEND DATA proactive command.  Contains the data to be sent.
    @see RSat::TSendDataV2
    @see RSat::NotifySendDataPCmd() */
    typedef TBuf8<KChannelDataMaxSize> TChannelData;


    class TSendDataV2 : public TSatPCmdBase
    /**
    Contains a SEND DATA proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifySendDataPCmd() for more information about
    the command.

    Request notification for SEND DATA with RSat::NotifyReceiveDataPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TSendData" class. For Symbian OS versions
    up to and including v8.1a and 8.1b use RSat::TSendDataV1Pckg, a packaged
    RSat::TSendDataV1.  For later versions of Symbian OS, check for an updated
    API (RSat::TSendDataV2 etc).

    @see RSat::TSendDataV2Pckg
    @see RSat::NotifySendDataPCmd()
    */
        {
    public:
        IMPORT_C TSendDataV2();
    public:
        TDeviceId iDestination;

        /** Should the SEND DATA command's data be sent immediately, or should
        the data from a series of SEND DATA command be buffered by the phone
        and sent at whatever rate the phone considers to be the best. */
        TSendDataMode iMode;

        TAlphaId iAlphaId;          //< Alpha identifier containing text to display to the user.
        TIconId iIconId;            //< Identifies the icon to display.
        TChannelData iChannelData;  //< Channel data to send.
        };

    /** A packaged RSat::TSendDataV2. */
    typedef TPckg<TSendDataV2>  TSendDataV2Pckg;

	class TSendDataV5 : public TSendDataV2
	/**
	Contains a SEND DATA proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifySendDataPCmd() for more information about
	the command.

	Request notification for SEND DATA with RSat::NotifyReceiveDataPCmd(), passing
	it an object in which to store the command when it arrives.  Pass a
	packaged version of the  latest "TSendData" class. For Symbian OS versions
	up to and including v8.1a and 8.1b use RSat::TSendDataV1Pckg, a packaged
	RSat::TSendDataV1.  For Symbian OS 9.2, use TSendDataV5Pckg.
	For later versions of Symbian OS, check for an updated
	API (RSat::TSendDataV6 etc).

	@see RSat::TSendDataV5Pckg
	@see RSat::NotifySendDataPCmd()
	*/
		{
	public:
		IMPORT_C TSendDataV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TSendDataV2::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TSendDataV5. */
	typedef TPckg<TSendDataV5>  TSendDataV5Pckg;

	class TSendDataV6 : public TSendDataV5
	/**
	Contains a SEND DATA proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifySendDataPCmd() for more information about
	the command.

	Request notification for SEND DATA with RSat::NotifyReceiveDataPCmd(), passing
	it an object in which to store the command when it arrives.  Pass a
	packaged version of the  latest "TSendData" class. For Symbian OS versions
	up to and including v8.1a and 8.1b use RSat::TSendDataV1Pckg, a packaged
	RSat::TSendDataV1.  For Symbian OS 9.2, use TSendDataV5Pckg.
	For Symbian OS 9.4, use TSendDataV6Pckg.
	For later versions of Symbian OS, check for an updated
	API (RSat::TSendDataV7 etc).

	@see RSat::TSendDataV6Pckg
	@see RSat::NotifySendDataPCmd()
	*/
		{
	public:
		IMPORT_C TSendDataV6();

	public:
		TUint8 iFrameId; //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TSendDataV6. */
	typedef TPckg<TSendDataV6>  TSendDataV6Pckg;
	
    IMPORT_C void NotifySendDataPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TSendDataRspV2 : public TSatPCmdBase
    /**
    Contains your response to a SEND DATA proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifySendDataPCmd() for more information about
    the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TSendDataRspV2Pckg (a packaged
    TSendDataRspV2). For later versions of Symbian OS, check for an updated
    API (RSat::TSendDataRspV3 etc).

    @see RSat::NotifySendDataPCmd()
    */
        {
    public:
        IMPORT_C TSendDataRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        TUint iInfoType;                //< Additional Info type
        TAdditionalInfo iAdditionalInfo;//< Additional information
        TUint8 iChannelDataLength;      //< Channel data length.
        };

    /** A packaged RSat::TSendDataRspV2. */
    typedef TPckg<TSendDataRspV2>  TSendDataRspV2Pckg;

    //
    // GET CHANNEL STATUS
    //

    class TGetChannelStatusV2 : public TSatPCmdBase
    /**
    Contains a GET CHANNEL STATUS proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyGetChannelStatusPCmd() for more information about
    the command.

    Request notification for GET CHANNEL STATUS with RSat::NotifyGetChannelStatusPCmd(), passing
    it an object in which to store the command when it arrives.  Pass a
    packaged version of the  latest "TGetChannelStatus" class. For Symbian OS versions
    up to and including v8.1a and 8.1b use RSat::TGetChannelStatusV1Pckg, a packaged
    RSat::TGetChannelStatusV1.  For later versions of Symbian OS, check for an updated
    API (RSat::TGetChannelStatusV2 etc).

    @see RSat::TGetChannelStatusV2Pckg
    @see RSat::NotifyGetChannelStatusPCmd()
    */
        {
    public:
        IMPORT_C TGetChannelStatusV2();
        };

    /** A packaged RSat::TGetChannelStatusV2. */
    typedef TPckg<TGetChannelStatusV2> TGetChannelStatusV2Pckg;

    IMPORT_C void NotifyGetChannelStatusPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TGetChannelStatusRspV2 : public TSatPCmdBase
    /**
    Contains your response to a GET CHANNEL STATUS proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyGetChannelStatusPCmd() for more information about
    the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TGetChannelStatusRspV2Pckg (a packaged
    TGetChannelStatusRspV2). For later versions of Symbian OS, check for an updated
    API (RSat::TGetChannelStatusRspV3 etc).

    @see RSat::NotifyGetChannelStatusPCmd()
    */
        {
    public:
        IMPORT_C TGetChannelStatusRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        TUint iInfoType;                //< Additional Info type
        TAdditionalInfo iAdditionalInfo;//< Additional information
        };

    /** A packaged RSat::TGetChannelStatusRspV2. */
    typedef TPckg<TGetChannelStatusRspV2>  TGetChannelStatusRspV2Pckg;

    //
    // SERVICE SEARCH
    //

	/** RSat::TServiceSearchParam */
    enum {KServiceSearchMaxSize=242};
    /** Part of a SERVICE SEARCH proactive command.
    @see RSat::TServiceSearchV2 */
    typedef TBuf8<KServiceSearchMaxSize> TServiceSearchParam;

    struct TServiceSearchInfo
    /** Part of a SERVICE SEARCH proactive command.
    @see RSat::TServiceSearchV2 */
        {
        TLocalBearerId iBearerId;           //< Local bearer identifier
        TServiceSearchParam iSearchParam;   //< Service search parameters
        };

	/** @see RSat::TDeviceFilter */
    enum {KDeviceFilterMaxSize=239};
    /** Part of a SERVICE SEARCH proactive command.
    @see RSat::TServiceSearchV2 */
    typedef TBuf8<KDeviceFilterMaxSize> TDeviceFilter;

    struct TDeviceFilterInfo
    /** Part of a SERVICE SEARCH proactive command.
    @see RSat::TServiceSearchV2 */
        {
        TLocalBearerId iBearerId;       //< Bearer identifier
        TDeviceFilter iDeviceFilter;    //< Device filter
        };


    class TServiceSearchV2 : public TSatPCmdBase
    /**
    Contains a SERVICE SEARCH proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyServiceSearchPCmd() for more information about
    the command.

    Request notification for SERVICE SEARCH with
    RSat::NotifyServiceSearchPCmd(), passing it an object in which to store
    the command when it arrives.  Pass a packaged version of the  latest
    "TServiceSearch" class. For Symbian OS version from 7.0s to v8.1a and
    8.1b use RSat:TServiceSearchV2Pckg, a packaged RSat::TServiceSearchV2.
    For later versions of Symbian OS, check for an updated API
    (RSat::TServiceSearchV3 etc).

    @see RSat::TServiceSearchV2Pckg
    @see RSat::NotifyServiceSearchPCmd()
    */
        {
    public:
        IMPORT_C TServiceSearchV2();
    public:
        TAlphaId iAlphaId;          //< Alpha identifier that may contain text to display to the user.
        TIconId iIconId;            //< Identifies the icon to display.
        TServiceSearchInfo iSearch; //< Service search information
        TDeviceFilterInfo iFilter;  //< Device filter information
        };

    /** A packaged RSat::TServiceSearchV2. */
    typedef TPckg<TServiceSearchV2> TServiceSearchV2Pckg;

	class TServiceSearchV5 : public TServiceSearchV2
	/**
	Contains a SERVICE SEARCH proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifyServiceSearchPCmd() for more information about
	the command.

   	Request notification for SERVICE SEARCH with
	RSat::NotifyServiceSearchPCmd(), passing it an object in which to store
	the command when it arrives.  Pass a packaged version of the  latest
	"TServiceSearch" class. For Symbian OS version from 7.0s to v8.1a and
	8.1b use RSat:TServiceSearchV2Pckg, a packaged RSat::TServiceSearchV2.
	For Symbian OS 9.2, use TServiceSearchV5Pckg.
	For later versions of Symbian OS, check for an updated API
	(RSat::TServiceSearchV6 etc).

	@see RSat::TServiceSearchV2
	@see RSat::TServiceSearchV5Pckg
	@see RSat::NotifyServiceSearchPCmd()
	*/
		{
	public:
		IMPORT_C TServiceSearchV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TServiceSearchV2::iAlphaId is displayed.
		@see RSat::TTextAttribute 
		*/
		TTextAttribute iTextAttribute;
		};

    /** A packaged RSat::TServiceSearchV5. */
    typedef TPckg<TServiceSearchV5> TServiceSearchV5Pckg;
    
   	class TServiceSearchV6 : public TServiceSearchV5
	/**
	Contains a SERVICE SEARCH proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifyServiceSearchPCmd() for more information about
	the command.

   	Request notification for SERVICE SEARCH with
	RSat::NotifyServiceSearchPCmd(), passing it an object in which to store
	the command when it arrives.  Pass a packaged version of the  latest
	"TServiceSearch" class. For Symbian OS version from 7.0s to v8.1a and
	8.1b use RSat:TServiceSearchV2Pckg, a packaged RSat::TServiceSearchV2.
	For Symbian OS 9.2, use TServiceSearchV5Pckg.
	For Symbian OS 9.4, use TServiceSearchV6Pckg.
	For later versions of Symbian OS, check for an updated API
	(RSat::TServiceSearchV7 etc).

	@see RSat::TServiceSearchV5
	@see RSat::TServiceSearchV6Pckg
	@see RSat::NotifyServiceSearchPCmd()
	*/
		{
	public:
		IMPORT_C TServiceSearchV6();

	public:
		TUint8 iFrameId; //<  Each frame has an ID. This field contains the ID of the frame. 
		};

    /** A packaged RSat::TServiceSearchV6. */
    typedef TPckg<TServiceSearchV6> TServiceSearchV6Pckg;
    
    IMPORT_C void NotifyServiceSearchPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TServiceSearchRspV2 : public TSatPCmdBase
    /**
    Contains your response to a SERVICE SEARCH proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyServiceSearchPCmd() for more information about
    the command.

    For Symbian OS versions from 7.0s to v8.1a and 8.1b pass
    RSat::TerminalRsp() a RSat::TServiceSearchRspV2Pckg (a packaged
    TServiceSearchRspV2). For later versions of Symbian OS, check for an
    updated API (RSat::TServiceSearchRspV3 etc).

    @see RSat::NotifyServiceSearchPCmd()
    */
        {
    public:
        IMPORT_C TServiceSearchRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        TUint iInfoType;                //< Additional Info type
        TAdditionalInfo iAdditionalInfo;//< Additional information
       };

    /** A packaged RSat::TServiceSearchRspV2. */
    typedef TPckg<TServiceSearchRspV2>  TServiceSearchRspV2Pckg;

    //
    // GET SERVICE INFO
    //

	/**
	@see RSat::TAttributeInfo
	*/
    enum {KAttributeInfoMaxSize=242};
    /** Part of a GET SERVICE INFO proactive command.
    @see RSat::TGetServiceInfoV2 */
    typedef TBuf8<KAttributeInfoMaxSize> TAttributeInfo;

    struct TAttributeInfoDetails
    /** Part of a GET SERVICE INFO proactive command.
    @see RSat::TGetServiceInfoV2 */
        {
        TLocalBearerId iBearerId;       //< Local bearer identifier
        TAttributeInfo iAttributeInfo;  //< Attribute information
        };


    class TGetServiceInfoV2 : public TSatPCmdBase
    /**
    Contains a GET SERVICE INFO proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyGetServiceInfoPCmd() for more information about
    the command.

    Request notification for GET SERVICE INFO with
    RSat::NotifyGetServiceInfoPCmd(), passing it an object in which to store
    the command when it arrives.  Pass a packaged version of the  latest
    "TGetServiceInfo" class. For Symbian OS version from 7.0s to v8.1a and
    8.1b use RSat:TGetServiceInfoV2Pckg, a packaged RSat::TGetServiceInfoV2.
    For later versions of Symbian OS, check for an updated API
    (RSat::TGetServiceInfoV3 etc).

    @see RSat::TGetServiceInfoV2Pckg
    @see RSat::NotifyGetServiceInfoPCmd()
    */
        {
    public:
        IMPORT_C TGetServiceInfoV2();
    public:
        TAlphaId iAlphaId;                      //< Alpha identifier that may contain text to display to the user.
        TIconId iIconId;                        //< Identifies the icon to display.
        TAttributeInfoDetails iAttributeInfo;   //< Attribute information details
        };

    /** A packaged RSat::TGetServiceInfoV2. */
    typedef TPckg<TGetServiceInfoV2> TGetServiceInfoV2Pckg;

	class TGetServiceInfoV5: public TGetServiceInfoV2
	/**
	Contains a GET SERVICE INFO proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifyGetServiceInfoPCmd() for more information about
	the command.

	Request notification for GET SERVICE INFO with
	RSat::NotifyGetServiceInfoPCmd(), passing it an object in which to store
	the command when it arrives.  Pass a packaged version of the  latest
	"TGetServiceInfo" class. For Symbian OS version from 7.0s to v8.1a and
	8.1b use RSat:TGetServiceInfoV2Pckg, a packaged RSat::TGetServiceInfoV2.
	For Symbian OS version 9.2, use TGetServiceInfoV5Pckg.
	For later versions of Symbian OS, check for an updated API
	(RSat::TGetServiceInfoV6 etc).

	@see RSat::TGetServiceInfoV2
	@see RSat::TGetServiceInfoV5Pckg
	@see RSat::NotifyGetServiceInfoPCmd()
	*/
		{
	public:
		IMPORT_C TGetServiceInfoV5();

	public:
		/** Contains formatting information to be applied when
		RSat::TGetServiceInfoV2::iAlphaId is displayed. 
		@see RSat::TTextAttribute
		*/
		TTextAttribute iTextAttribute;
		};

	/** A packaged RSat::TGetServiceInfoV5. */
	typedef TPckg<TGetServiceInfoV5>  TGetServiceInfoV5Pckg;

	class TGetServiceInfoV6: public TGetServiceInfoV5
	/**
	Contains a GET SERVICE INFO proactive command issued by the UICC.   See
	ETSI TS 102 223 and RSat::NotifyGetServiceInfoPCmd() for more information about
	the command.

	Request notification for GET SERVICE INFO with
	RSat::NotifyGetServiceInfoPCmd(), passing it an object in which to store
	the command when it arrives.  Pass a packaged version of the  latest
	"TGetServiceInfo" class. For Symbian OS version from 7.0s to v8.1a and
	8.1b use RSat:TGetServiceInfoV2Pckg, a packaged RSat::TGetServiceInfoV2.
	For Symbian OS version 9.2, use TGetServiceInfoV5Pckg.
	For Symbian OS version 9.4, use TGetServiceInfoV6Pckg.
	For later versions of Symbian OS, check for an updated API
	(RSat::TGetServiceInfoV7 etc).

	@see RSat::TGetServiceInfoV5
	@see RSat::TGetServiceInfoV6Pckg
	@see RSat::NotifyGetServiceInfoPCmd()
	*/
		{
	public:
		IMPORT_C TGetServiceInfoV6();

	public:
		TUint8 iFrameId; //<  Each frame has an ID. This field contains the ID of the frame. 
		};

	/** A packaged RSat::TGetServiceInfoV6. */
	typedef TPckg<TGetServiceInfoV6>  TGetServiceInfoV6Pckg;
	
    IMPORT_C void NotifyGetServiceInfoPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TGetServiceInfoRspV2 : public TSatPCmdBase
    /**
    Contains your response to a GET SERVICE INFO proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyGetServiceInfoPCmd() for more information about
    the command.

    For Symbian OS versions from 7.0s to v8.1a and 8.1b pass
    RSat::TerminalRsp() a RSat::TGetServiceInfoRspV2Pckg (a packaged
    TGetServiceInfoRspV2). For later versions of Symbian OS, check for an
    updated API (RSat::TGetServiceInfoRspV3 etc).

    @see RSat::NotifyGetServiceInfoPCmd()
    */
        {
    public:
        IMPORT_C TGetServiceInfoRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        TUint iInfoType;                //< Additional Info type
        TAdditionalInfo iAdditionalInfo;//< Additional information
        TServiceRecordInfo iRecordInfo; //< Service record information
        };

    /** A packaged RSat::TGetServiceInfoRspV2. */
    typedef TPckg<TGetServiceInfoRspV2>  TGetServiceInfoRspV2Pckg;

    //
    // DECLARE SERVICE
    //

    enum TServiceDeclarationType
    /** Part of a DECLARE SERVICE proactive command.

    @see RSat::TDeclareServiceV2 */
        {
        EDeclarationTypeNotSet, //< Initialization value.
        EAddService,            //< Add a new service to the ME service database.
        EDeleteService          //< Delete a service from the ME service database.
        };

    class TDeclareServiceV2 : public TSatPCmdBase
    /**
    Contains a DECLARE SERVICE proactive command issued by the UICC.   See
    ETSI TS 102 223 and RSat::NotifyDeclareServicePCmd() for more information about
    the command.

    Request notification for DECLARE SERVICE with
    RSat::NotifyDeclareServicePCmd(), passing it an object in which to store
    the command when it arrives.  Pass a packaged version of the  latest
    "TDeclareService" class. For Symbian OS version from 7.0s to v8.1a and
    8.1b use RSat:TDeclareServiceV2Pckg, a packaged RSat::TDeclareServiceV2.
    For later versions of Symbian OS, check for an updated API
    (RSat::TDeclareServiceV3 etc).

    @see RSat::TDeclareServiceV2Pckg
    @see RSat::NotifyDeclareServicePCmd()
    */
        {
    public:
        IMPORT_C TDeclareServiceV2();
    public:
        TServiceDeclarationType iType;      //< Service declaration type
        TServiceRecordInfo iServiceRecord;  //< Service record information
        TSimMeInterface iInterface;         //< SIM / ME interface
        };

    /** A packaged RSat::TDeclareServiceV2. */
    typedef TPckg<TDeclareServiceV2> TDeclareServiceV2Pckg;

    IMPORT_C void NotifyDeclareServicePCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TDeclareServiceRspV2 : public TSatPCmdBase
    /**
    Contains your response to a DECLARE SERVICE proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyDeclareServicePCmd() for more information about
    the command.

    For Symbian OS versions from 7.0s to v8.1a and 8.1b pass
    RSat::TerminalRsp() a RSat::TDeclareServiceRspV2Pckg (a packaged
    TDeclareServiceRspV2). For later versions of Symbian OS, check for an
    updated API (RSat::TDeclareServiceRspV3 etc).

    @see RSat::NotifyDeclareServicePCmd()
    */
        {
    public:
        IMPORT_C TDeclareServiceRspV2();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        TUint iInfoType;                //< Additional Info type
        TAdditionalInfo iAdditionalInfo;//< Additional information
        };

    /** A packaged RSat::TDeclareServiceRspV2. */
    typedef TPckg<TDeclareServiceRspV2>  TDeclareServiceRspV2Pckg;

    IMPORT_C void NotifyTsyStateUpdated(TRequestStatus& aReqStatus, TPCmd& aPCmd) const;

    //
    // TIMER MANAGEMENT
    //

    enum TTimerMgmtType
    /** Part of the TIMER MANAGEMENT proactive command. It describes the type
    of command: start a timer, stop a timer, or read a timer's value.
    @see RSat::TTimerMgmtV3 */
        {
        EStartTimer,      //< Start a timer running physically on the phone.
        EDeactivateTimer, //< Deactivate a timer.
        EGetTimerValue    //< Get the current value of a timer.
        };


    class TTimerMgmtV3 : public TSatPCmdBase
    /**
    Contains a TIMER MANAGEMENT proactive command issued by the UICC.
    See ETSI TS 102 223 and RSat::NotifyTimerMgmtPCmd() for more information
    about the command.

    Request notification for TIMER MANAGEMENT with
    RSat::NotifyTimerMgmtPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TTimerMgmt" class. For Symbian OS versions from 7.0s to v8.1a and
    8.1b use RSat::TTimerMgmtV3Pckg, a packaged RSat::TTimerMgmtV3.  For later
    versions of Symbian OS, check for an updated API (RSat::TTimerMgmtV4 etc).

    @see RSat::TTimerMgmtV3Pckg
    @see RSat::NotifyTimerMgmtPCmd()
    */
        {
    public:
        IMPORT_C TTimerMgmtV3();
    public:
        /** This identifies the timer.  There can be up to 8 timer. */
        TTimerId    iTimerId;
        /** The initial value of the timer when setting a new timer.    */
        TDuration   iTimerValue;
        /** Should you start, stop, or read the current value of the timer identifier in iTimerId? */
        TTimerMgmtType  iOperation;
        };

    /** A packaged RSat::TTimerMgmtV3. */
    typedef TPckg<TTimerMgmtV3> TTimerMgmtV3Pckg;

    IMPORT_C void NotifyTimerMgmtPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TTimerMgmtRspV3 : public TSatPCmdBase
    /**
    Contains your response to a TIMER MANAGEMENT  proactive command.
    Pass this to the UICC with RSat::TerminalRsp(). See
    RSat::NotifyTimerMgmtPCmd() for more information about the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TTimerMgmtRspV3Pckg (a packaged TTimerMgmtRspV3).
    For later versions of Symbian OS, check for an updated API
    (RSat::TTimerMgmtRspV4 etc).

    @see RSat::NotifyTimerMgmtPCmd()
    */
        {
    public:
        IMPORT_C TTimerMgmtRspV3();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

       /** Set to RSat::KTimerMgmt if you are returning a timer's value to the
       UICC. This occurs when RSat::TTimerMgmtV3::iMode in the original command
       was is RSat::EGetTimerValue or RSat::EDeactivateTimer. Otherwise set to
       RSat::KMeProblem or RSat::KNoAdditionalInfo. */
       TUint iInfoType;

        /** Return the timer's value in here when RSat::TTimerMgmtV3::iMode in
        the original command was is RSat::EGetTimerValue or
        RSat::EDeactivateTimer. Alternatively, some error values of
        iGeneralResult require additional information, which goes here. */
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TTimerMgmtRspV3. */
    typedef TPckg<TTimerMgmtRspV3>  TTimerMgmtRspV3Pckg;

    //
    // PROVIDE LOCAL INFORMATION
    //

    /** Part of a PROVIDE LOCAL INFORMATION proactive command
    This describes the information that you should provide to the UICC.

    The format of the information depends upon the network on which the phone is
    running. For GSM/WCDMA networks, read 3GPP TS 31.111; for CDMA networks
    read 3GPP2 C.S0035-0.  ETSI TS 102 223 provides generic information
    about the PROVIDE LOCAL INFORMATION command.
    @see RSat::TLocalInfoV3. */
   enum TLocaInfomationType
        {
        /**
        Location information & phone identity
        UICC: MCC, MNC, LAC, Cell ID
        */
        KProvideLocalInfo               =0x00,
        KProvideLocalInfoImei           =0x01,   //< IMEI of the phone.
        KProvideLocalInformationNmr     =0x02,   //< Network Measurement Results.
        KProvideLocalInfoDateTimeTimeZone =0x03, //< Current date, time and time zone.
        KProvideLocalInfoLanguage       =0x04,   //< Language preference.
        KProvideLocalInfoTimingAdv      =0x05,   //< Timing advance (access network dependent).
        KProvideLocalInfoAccTech        =0x06,   //< Current access technology.
        KProvideLocalInfoESN            =0x07,    //< ESN of the phone.
        KProvideLocalInfoIMEISV         =0x08,    //< IMEISV of the terminal;
        KProvideLocalInfoSearchMode     =0x09,    //< Search Mode;
        KProvideLocalInfoChargeStateOfBattery   =0x0A, //< Charge State of the Battery;
        KProvideLocalInfoMEID     =0x0B //< MEID of the terminal;
        }; 
     
    class TLocalInfoV3 : public TSatPCmdBase
    /**
    Contains a PROVIDE LOCAL INFORMATION proactive command issued by the UICC.
    See ETSI TS 102 223 and RSat::NotifyLocalInfoPCmd() for more information
    about the command.

    Request notification for PROVIDE LOCAL INFORMATION with
    RSat::NotifyLocalInfoPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TLocalInfo" class. For Symbian OS versions from 7.0s to v8.1a and
    8.1b use RSat::TLocalInfoV3Pckg, a packaged RSat::TLocalInfoV3.  For later
    versions of Symbian OS, check for an updated API (RSat::TLocalInfoV4 etc).

    @see RSat::TLocalInfoV3Pckg
    @see RSat::NotifyLocalInfoPCmd()
    */
        {
    public:
        IMPORT_C TLocalInfoV3();
    public:
        TDeviceId iDevideId; //< The device for which the information is required.
        TLocaInfomationType iInfoType; //< The type of information required.
        };

    /** A packaged RSat::TLocalInfoV3. */
    typedef TPckg<TLocalInfoV3> TLocalInfoV3Pckg;

 	/**
    UTRAN Measurement Qualifier:It shall be included to indicate to the ME that 
    "Network Measurement Results for a UTRAN" is required,read 3GPP TS 31.111.
    @see  RSat::TLocalInfoV6. 
    */    
    enum TUtranMeasurementQualifier
    	{
    	EUtranMeasurementQualifierNotSet,         //<This value implies that a GERAN network measurement is required.
    	EUtranMeasurementQualifierIntraFrequency, //<Intra-Frequency Measurement.
    	EUtranMeasurementQualifierInterFrequency, //<Inter-Frequency Measurement.
    	EUtranMeasurementQualifierInterRATGsm     //<Inter-RAT (GSM) measurement.	
    	};
    	
    class TLocalInfoV6 : public TLocalInfoV3
    /**
    Contains a PROVIDE LOCAL INFORMATION proactive command issued by the UICC.
    See 3GPP TS 31.111 Release 6, v 6.8.0, 2006-03 and RSat::NotifyLocalInfoPCmd()
    for more information about the command.
    
    Request notification for PROVIDE LOCAL INFORMATION with
    RSat::NotifyLocalInfoPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TLocalInfo" class. 
    @see RSat::TLocalInfoV6Pckg
    @see RSat::NotifyLocalInfoPCmd()
    */
        {
    public:
        IMPORT_C TLocalInfoV6();
    public:
        TUtranMeasurementQualifier iUtranMeasurementQualifier; //<This information is only available when ME is connected to UTRAN
        };
    
    /** A packaged RSat::TLocalInfoV6. */
    typedef TPckg<TLocalInfoV6> TLocalInfoV6Pckg;

    IMPORT_C void NotifyLocalInfoPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TLocalInfoRspV3 : public TSatPCmdBase
    /**
    Contains your response to a PROVIDE LOCAL INFORMATION proactive command.
    Pass this to the UICC with RSat::TerminalRsp(). See
    RSat::NotifyLocalInfoPCmd() for more information about the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TLocalInfoRspV3Pckg (a packaged TLocalInfoRspV3).
    For later versions of Symbian OS, check for an updated API
    (RSat::TLocalInfoRspV4 etc).

    Set iGeneralResultSet as defined in the specifications. Set  iInfoType to
    RSat::KLocalInformation.  The local information goes into iAdditionalInfo.
    The format of the information depends upon the network on which the phone
    is running. For GSM/WCDMA networks, read 3GPP TS 31.111; for CDMA networks
    read 3GPP2 C.S0035-0.  ETSI TS 102 223 provides generic information about
    the PROVIDE LOCAL INFORMATION command.

    @see RSat::NotifyLocalInfoPCmd()
    */
        {
    public:
        IMPORT_C TLocalInfoRspV3();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** Set to RSat::KLocalInformation to return local information, or
        possibly RSat::KMeProblem is there is a problem. */
        TUint iInfoType;

        /** Contains the local information requested by the UICC.
        Alternatively, some error values of iGeneralResult require additional
        information, which goes here. */
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TLocalInfoRspV3. */
    typedef TPckg<TLocalInfoRspV3>  TLocalInfoRspV3Pckg;
  
    //
    //POLLING INTERVAL
    //
    class TPollingIntervalV3 : public TSatPCmdBase
    /**
    Contains a POLL INTERVAL proactive command issued by the UICC.
    See ETSI TS 102 223 and RSat::NotifyPollingIntervalPCmd() for more information
    about the command.

    Request notification for POLL INTERVAL with
    RSat::NotifyPollingIntervalPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TPollingInterval" class. For Symbian OS versions from 7.0s to v8.1a and
    8.1b use RSat::TPollingIntervalV3Pckg, a packaged RSat::TPollingIntervalV3.  For later
    versions of Symbian OS, check for an updated API (RSat::TPollingIntervalV4 etc).

    @see RSat::TPollingIntervalV3Pckg
    @see RSat::NotifyPollingIntervalPCmd()
    */
        {
    public:
        IMPORT_C TPollingIntervalV3();
    public:
        TDuration   iIntv; //< The requested polling interval.
        TDeviceId   iDestination; //< The device for which the polling is required.
        };

    /** A packaged RSat::TPollingIntervalV3. */
    typedef TPckg<TPollingIntervalV3> TPollingIntervalV3Pckg;

    IMPORT_C void NotifyPollingIntervalPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TPollingIntervalRspV3 : public TSatPCmdBase
    /**
    Contains your response to a POLL INTERVAL proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS
    102 223 and RSat::NotifyPollingIntervalPCmd() for more information about
    the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TPollingIntervalRspV3Pckg (a packaged TPollingIntervalRspV3).
    For later versions of Symbian OS, check for an updated API
    (RSat::TPollingIntervalRspV4 etc).

    @see RSat::NotifyPollingIntervalPCmd()
    */
        {
    public:
        IMPORT_C TPollingIntervalRspV3();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If you are returning a polling interval, set this to RSat::KPollingIntv.
        If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Return the polling interval in here.  Alternatively, some error
        values of iGeneralResult require additional information, which goes
        here. */
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TPollingIntervalRspV3. */
    typedef TPckg<TPollingIntervalRspV3> TPollingIntervalRspV3Pckg;

    //
    //Polling Off Card
    //

    class TPollingOffV3 : public TSatPCmdBase
    /**
    Contains a POLLING OFF proactive command issued by the UICC.
    See ETSI TS 102 223 and RSat::NotifyPollingOffPCmd() for more information
    about the command.

    Request notification for POLLING OFF with
    RSat::NotifyPollingIntervalPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TPollingOff" class. For Symbian OS versions from 7.0s to v8.1a and
    8.1b use RSat::TPollingOffV3Pckg, a packaged RSat::TPollingOffV3.  For later
    versions of Symbian OS, check for an updated API (RSat::TPollingOffV4 etc).

    @see RSat::TPollingOffV3Pckg
    @see RSat::NotifyPollingOffPCmd()
    */
        {
    public:
        IMPORT_C TPollingOffV3();
    public:
        TDuration   iIntv; //< The requested polling interval.
        TDeviceId   iDestination; //< The device for which the polling is required.
        };

    /** A packaged RSat::TPollingOffV3. */
    typedef TPckg<TPollingOffV3> TPollingOffV3Pckg;

    IMPORT_C void NotifyPollingOffPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TPollingOffRspV3 : public TSatPCmdBase
    /**
    Contains your response to a POLLING OFF proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifyPollingOffPCmd() for more information about the command.

    For Symbian OS versions from 7.0s to  v8.1a and v8.1b pass
    RSat::TerminalRsp() a RSat::TPollingOffRspV3Pckg (a packaged TPollingOffRspV3).
    For later versions of Symbian OS, check for an updated API
    (RSat::TPollingOffRspV4 etc).

    @see RSat::NotifyPollingOffPCmd()
    */
       {
    public:
        IMPORT_C TPollingOffRspV3();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TPollingOffRspV3. */
    typedef TPckg<TPollingOffRspV3>  TPollingOffRspV3Pckg;
	
	//
	//  V6 PROACTIVE COMMANDS
	//
	
	/** 
	@see RSat::TMultimediaMessageRef
	*/
	enum {KMaxMultimediaMessageRefSize = 255};
	
	/** 
	@see RSat::TFileList
	*/
	enum {KMaxFileListSize = 242};
	
	/** 
	@see RSat::TMultimediaContentId
	*/
	enum {KMaxMultimediaContentIdSize = 255};
	
	/** 
	@see RSat::TMultimediaMessageId 
	*/
	enum {KMaxMultimediaMessageIdSize = 255};
	
	/**
	This buffer contains the URI identifying the multimedia message in the network.
	@see RSat::KMaxMultimediaMessageRefSize
	*/
	typedef TBuf<KMaxMultimediaMessageRefSize> TMultimediaMessageRef;
	
	/**
	This buffer contains the information of the received file.
	@see RSat::KMMSFileNameSize
	*/
	typedef TBuf<KMaxFileListSize> TFileList;
	
	/**
	This buffer ontains the Data Object tag to be used when the MM Content is stored in the referenced BER-TLV file.
	@see RSat::KMaxMultimediaContentIdSize
	*/
    typedef TBuf<KMaxMultimediaContentIdSize> TMultimediaContentId;
    
    /**
    This may be present only if the iReceptionFile is present. It is mandatory in case the MMS Reception or Submission file can store several MMs.
    @see RSat::KMaxMultimediaMessageIdSize
    */
    typedef TBuf<KMaxMultimediaMessageIdSize> TMultimediaMessageId;
    
    //
    // RETRIEVE MULTIMEDIA MESSAGE
    //

    class TRetrieveMultimediaMessageV6 : public TSatPCmdBase
    /**
    Contains a RETRIEVE MULTIMEDIA MESSAGE proactive command issued by the UICC.
    See ETSI TS 102 223 and RSat::NotifyRetrieveMultimediaMsgPCmd() for more information
    about the command.

    Request notification for RETRIEVE MULTIMEDIA MESSAGE with
    RSat::NotifyRetrieveMultimediaMsgPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TRetrieveMultimediaMessage" class. For Symbian OS version 9.4
    use RSat::TRetrieveMultimediaMessageV6Pckg, a packaged RSat::TRetrieveMultimediaMessageV6.  
    For later versions of Symbian OS, check for an updated API (RSat::TRetrieveMultimediaMessageV7 etc).

    @see RSat::TRetrieveMultimediaMessageV6Pckg
    @see RSat::NotifyRetrieveMultimediaMsgPCmd()
    */
        {
    public:
        IMPORT_C TRetrieveMultimediaMessageV6();
    public:
		TDeviceId iDestination; //< The destination device for the proactive command is the network.
		TAlphaId iAlphaId; //< Alpha Identifier
		TIconId	iIconId; //< Icon identifer
		TMultimediaMessageRef	iMultimediaMessageRef;  //< Contains the URI identifying the multimedia message in the network.
		TFileList iReceptionFile;            //< MMS Reception File is a path of a file on the UICC. 
		TMultimediaContentId iMultimediaContentId; //< Contains the Data Object tag to be used when the MM Content is stored in the referenced BER-TLV file.
		TMultimediaMessageId iMultimediaMessageId; //< This may be present only if the iReceptionFile is present. It is mandatory in case the MMS Reception or Submission file can store several MMs.
		TTextAttribute	iTextAttribute; //< Contains formatting information to be applied when RSat::TRetrieveMultimediaMessageV6::iAlphaId  is displayed.
       	};

    /** A packaged RSat::TRetrieveMultimediaMessageV6. */
    typedef TPckg<TRetrieveMultimediaMessageV6> TRetrieveMultimediaMessageV6Pckg;

    IMPORT_C void NotifyRetrieveMultimediaMsgPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;

    class TRetrieveMultimediaMessageRspV6 : public TSatPCmdBase
    /**
    Contains your response to a RETRIEVE MULTIMEDIA MESSAGE proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifyRetrieveMultimediaMsgPCmd() for more information about the command.

    For Symbian OS version 9.4 pass
    RSat::TerminalRsp() a RSat::TRetrieveMultimediaMessageRspV6Pckg (a packaged TRetrieveMultimediaMessageRspV6).
    For later versions of Symbian OS, check for an updated API
    (RSat::TRetrieveMultimediaMessageRspV7 etc).

	@see RSat::TRetrieveMultimediaMessageRspV6Pckg
    @see RSat::NotifyRetrieveMultimediaMsgPCmd()
    */
       {
    public:
        IMPORT_C TRetrieveMultimediaMessageRspV6();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TRetrieveMultimediaMessageRspV6. */
    typedef TPckg<TRetrieveMultimediaMessageRspV6>  TRetrieveMultimediaMessageRspV6Pckg;
    
	//
	// SUBMIT MULTIMEDIA MESSAGE
	//    

    class TSubmitMultimediaMessageV6 : public TSatPCmdBase
    /**
    Contains a SUBMIT MULTIMEDIA MESSAGE proactive command issued by the UICC.
    See ETSI TS 102 223 and RSat::NotifySubmitMultimediaMsgPCmd() for more information
    about the command.

    Request notification for SUBMIT MULTIMEDIA MESSAGE with
    RSat::NotifySubmitMultimediaMsgPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TSubmitMultimediaMessage" class. For Symbian OS version 9.4
    use RSat::TSubmitMultimediaMessageV6Pckg, a packaged RSat::TSubmitMultimediaMessageV6.  
    For later versions of Symbian OS, check for an updated API (RSat::TSubmitMultimediaMessageV7 etc).

    @see RSat::TSubmitMultimediaMessageV6Pckg
    @see RSat::NotifySubmitMultimediaMsgPCmd()
    */
        {
    public:
        IMPORT_C TSubmitMultimediaMessageV6();
    public:
		TDeviceId iDestination; //< The destination device for the proactive command is the network.
		TAlphaId iAlphaId; //< Alpha Identifier
		TIconId	iIconId; //< Icon identifer
		TFileList iSubmissionFile; //< MMS Submission File is a path of a file on the UICC.
		TMultimediaMessageId iMultimediaMessageId; //< This may be present only if the iSubmissionFile is present. It is mandatory in case the MMS Reception or Submission file can store several MMs. Coded in hexadecimal.
		TTextAttribute	iTextAttribute; //< Contains formatting information to be applied when RSat::TSubmitMultimediaMessageV6::iAlphaId  is displayed.
       	};

    /** A packaged RSat::TSubmitMultimediaMessageV6. */
    typedef TPckg<TSubmitMultimediaMessageV6> TSubmitMultimediaMessageV6Pckg;

    IMPORT_C void NotifySubmitMultimediaMsgPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TSubmitMultimediaMessageRspV6 : public TSatPCmdBase
    /**
    Contains your response to a SUBMIT MULTIMEDIA MESSAGE proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifySubmitMultimediaMsgPCmd() for more information about the command.

    For Symbian OS version 9.4 pass
    RSat::TerminalRsp() a RSat::TSubmitMultimediaMessageRspV6Pckg (a packaged TSubmitMultimediaMessageRspV6).
    For later versions of Symbian OS, check for an updated API
    (RSat::TSubmitMultimediaMessageRspV7 etc).

    @see RSat::TSubmitMultimediaMessageRspV6Pckg
    @see RSat::NotifySubmitMultimediaMsgPCmd()
    */
       {
    public:
        IMPORT_C TSubmitMultimediaMessageRspV6();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TSubmitMultimediaMessageRspV6. */
    typedef TPckg<TSubmitMultimediaMessageRspV6>  TSubmitMultimediaMessageRspV6Pckg;

	//
	// DISPLAY MULTIMEDIA MESSAGE
	//
	
    class TDisplayMultimediaMessageV6 : public TSatPCmdBase
    /**
    Contains a DISPLAY MULTIMEDIA MESSAGE proactive command issued by the UICC.
    See ETSI TS 102 223 and RSat::NotifyDisplayMultimediaMsgPCmd() for more information
    about the command.

    Request notification for DISPLAY MULTIMEDIA MESSAGE with
    RSat::NotifyDisplayMultimediaMsgPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TDisplayMultimediaMessage" class. For Symbian OS version 9.4
    use RSat::TDisplayMultimediaMessageV6Pckg, a packaged RSat::TDisplayMultimediaMessageV6.  
    For later versions of Symbian OS, check for an updated API (RSat::TDisplayMultimediaMessageV7 etc).

    @see RSat::TDisplayMultimediaMessageV6Pckg
    @see RSat::NotifyDisplayMultimediaMsgPCmd()
    */
        {
    public:
        IMPORT_C TDisplayMultimediaMessageV6();
    public:
		TDeviceId iDestination; //< The destination device for the proactive command is the Mobile Equipment, typically a phone.
		TDisplayPriority iDisplayPriority;  //< This determines the text's priority.
		/** Determines whether the text should be removed from the screen
        after a delay or after user action. */
        TClearScreenTrigger iClearScreenTrigger;
		TFileList iSubmissionFile; //< Contains the information of the submitted file.
		TMultimediaMessageId iMultimediaMessageId; //< This may be present only if the iSubmissionFile is present. It is mandatory in case the MMS Reception or Submission file can store several MMs. Coded in hexadecimal.This parameter identifies the Multimedia message.
		TImmediateRsp iImmediateRsp; //< TImmediateRsp indicates if the (U)SAT UI client should sustain the display beyond sending the Terminal Response.
       	};

    /** A packaged RSat::TDisplayMultimediaMessageV6. */
    typedef TPckg<TDisplayMultimediaMessageV6> TDisplayMultimediaMessageV6Pckg;

    IMPORT_C void NotifyDisplayMultimediaMsgPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TDisplayMultimediaMessageRspV6 : public TSatPCmdBase
    /**
    Contains your response to a DISPLAY MULTIMEDIA MESSAGE proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifyDisplayMultimediaMsgPCmd() for more information about the command.

    For Symbian OS version 9.4 pass
    RSat::TerminalRsp() a RSat::TDisplayMultimediaMessageRspV6Pckg (a packaged TDisplayMultimediaMessageRspV6).
    For later versions of Symbian OS, check for an updated API
    (RSat::TDisplayMultimediaMessageRspV7 etc).

    @see RSat::TDisplayMultimediaMessageRspV6Pckg
    @see RSat::NotifyDisplayMultimediaMsgPCmd()
    */
       {
    public:
        IMPORT_C TDisplayMultimediaMessageRspV6();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        };

    /** A packaged RSat::TDisplayMultimediaMessageRspV6. */
    typedef TPckg<TDisplayMultimediaMessageRspV6>  TDisplayMultimediaMessageRspV6Pckg;
	
	//
	// SET FRAMES
	//		
	
	/**
	This enum structure gives us the format of the Frame layout.
	@see RSat::TFrameLayout
	@see RSat::TSetFramesV6
	*/
	enum TFramesLayout
		{
 		EFrameHorizontal 	= 0x01, //< This indicator is flagged when the frame layout is horizontal.
 		EFrameVertical 		= 0x02  //< This indicator is flagged when the frame layout is vertical.
		};
	/**
	This enum structure gives us the format of the Frame Layout information
	@see RSat::TFrameLayout
	*/
	enum {KMaxFrameLayoutSize = 243};
	
	/** 
	@see RSat::KMaxFrameLayoutSize 
	*/
	typedef TBuf<KMaxFrameLayoutSize> TFrameLayoutBuf;
	 
	/**
	This structure gives us the Frame Layout details.
	@see RSat::TSetFramesV6
	*/
	struct TFrameLayout 
		{
		TFramesLayout iFramesLayout;  //< It gives us the format of the Frame layout
		TFrameLayoutBuf iFrameLayoutBuf;  //< It gives us the information about the number of sub frames in it.
		};
	
	/**
	@see RSat::TSetFramesV6
	*/
	enum TFramesSeparator
		{
		EFrameSeparatorNotSet, //< Indicates that the frame separator has not been set. This is the default value.
		EFrameSeparatorDrawn,  //< This value tells the terminal to draw a separator between every adjoining frame;
		EFrameSeparatorNotDrawn, //< This value tells the terminal not to draw a separator between every adjoining frame;	
		};
		
    class TSetFramesV6 : public TSatPCmdBase
    /**
    Contains a SET FRAMES proactive command issued by the UICC.
    See ETSI TS 102 223 and RSat::NotifySetFramesPCmd() for more information
    about the command.

    Request notification for SET FRAMES with
    RSat::NotifySetFramesPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TSetFramesV6" class. For Symbian OS version 9.4
    use RSat::TSetFramesV6Pckg, a packaged RSat::TSetFramesV6.  
    For later versions of Symbian OS, check for an updated API (RSat::TSetFramesV7 etc).

    @see RSat::TSetFramesV6Pckg
    @see RSat::NotifySetFramesPCmd()
    */
        {
    public:
        IMPORT_C TSetFramesV6();
    public:
		TDeviceId iDestination; //< The destination device for the proactive command is the Mobile Equipment, typically a phone.
		TUint8 iFrameId; //< Identifies the frame relevant to this SET FRAMES command. Refer to TS 102 223 for further details of what values this attribute can contain.
		TFrameLayout iFrameLayout; //< It contains the information on layout and also the design details of the frame 	
		TUint8 iDefaultFrameId;    //< This is optional. It contains an indication of the frame to be used to display information in case a Frame Identifier is not included.
		TFramesSeparator iFramesSeparator;//< This value tells the terminal to draw a separator between every adjoining frame;
       	};

    /** A packaged RSat::TSetFramesV6. */
    typedef TPckg<TSetFramesV6> TSetFramesV6Pckg;

    IMPORT_C void NotifySetFramesPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TSetFramesRspV6 : public TSatPCmdBase
    /**
    Contains your response to a SET FRAMES proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifySetFramesPCmd() for more information about the command.

    For Symbian OS version 9.4 pass
    RSat::TerminalRsp() a RSat::TSetFramesRspV6Pckg (a packaged TSetFramesRspV6).
    For later versions of Symbian OS, check for an updated API
    (RSat::TSetFramesRspV7 etc).

    @see RSat::TSetFramesRspV6Pckg
    @see RSat::NotifySetFramesPCmd()
    */
       {
    public:
        IMPORT_C TSetFramesRspV6();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KFramesInfo or RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;

        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        
        /** Contains the Frames information in the ME, if iGeneralResult is KSuccess 
        and the frames information is available. If not, this is left blank
		*/
		TFramesInformation iFramesInformation;
        };

    /** A packaged RSat::TSetFramesRspV6. */
    typedef TPckg<TSetFramesRspV6>  TSetFramesRspV6Pckg;
	
	//
	// GET FRAMES STATUS
	//
	
    class TGetFramesStatusV6 : public TSatPCmdBase
    /**
    Contains a GET FRAMES STATUS proactive command issued by the UICC.
    See ETSI TS 102 223 and RSat::NotifyGetFramesStatusPCmd() for more information
    about the command.

    Request notification for GET FRAMES STATUS with
    RSat::NotifyGetFramesStatusPCmd(), passing it an object in which to store the
    command when it arrives.  Pass a packaged version of the  latest
    "TGetFramesStatusV6" class. For Symbian OS version 9.4
    use RSat::TGetFramesStatusV6Pckg, a packaged RSat::TGetFramesStatusV6.  
    For later versions of Symbian OS, check for an updated API (RSat::TGetFramesStatusV7 etc).

    @see RSat::TGetFramesStatusV6Pckg
    @see RSat::NotifyGetFramesStatusPCmd()
    */
        {
    public:
        IMPORT_C TGetFramesStatusV6();
    public:
		TDeviceId iDestination; //< The destination device for the proactive command is the Mobile Equipment, typically a phone.
       	};

    /** A packaged RSat::TGetFramesStatusV6. */
    typedef TPckg<TGetFramesStatusV6> TGetFramesStatusV6Pckg;

    IMPORT_C void NotifyGetFramesStatusPCmd(TRequestStatus& aReqStatus, TDes8& aPCmd) const;


    class TGetFramesStatusRspV6 : public TSatPCmdBase
    /**
    Contains the response to a GET FRAMES STATUS proactive command.  Pass this to
    the UICC with RSat::TerminalRsp(). See ETSI TS 102 223 and
    RSat::NotifyGetFramesStatusPCmd() for more information about the command.

    For Symbian OS version 9.4 pass
    RSat::TerminalRsp() a RSat::TGetFramesStatusRspV6Pckg (a packaged TGetFramesStatusRspV6).
    For later versions of Symbian OS, check for an updated API
    (RSat::TGetFramesStatusRspV7 etc).

	@see RSat::TGetFramesStatusRspV6Pckg
    @see RSat::NotifyGetFramesStatusPCmd()
    */
       {
    public:
        IMPORT_C TGetFramesStatusRspV6();
    public:
         /** General result of the proactive command. Set this as described in
        the proactive commands versus terminal response table in ETSI TS 102 223. */
        TPCmdResult iGeneralResult;

        /** If the value of iGeneralResult requires additional information,
        set this to RSat::KFramesInfo or RSat::KMeProblem.  Otherwise set to
        RSat::KNoAdditionalInfo. */
        TUint iInfoType;
	
        /** Contains additional information associated with certain error codes
        set in iGeneralResult. If the value in iGeneralResult does not require
        any information then leave this field empty.*/
        TAdditionalInfo iAdditionalInfo;
        
        /** Contains the Frames information in the ME, if iGeneralResult is KSuccess 
        and the frames information is available. If not, this is left blank
		*/
		TFramesInformation iFramesInformation;
        };

    /** A packaged RSat::TGetFramesStatusRspV6. */
    typedef TPckg<TGetFramesStatusRspV6>  TGetFramesStatusRspV6Pckg;
	
    //
    // V6 ENVELOPE COMMANDS
    //
    
    //
    // MMS NOTIFICATION DOWNLOAD
    //
    
    /** 
    @see RSat::TMMSNotification
    */
   	enum {KMaxMMSNotificationSize = 243};
   	
   	/** 
   	@see RSat::KMaxFrameLayoutSize
   	@see RSat::TMmsNotificationDownloadV6
   	*/
    typedef TBuf<KMaxFrameLayoutSize> TMMSNotification;
    
    class TMmsNotificationDownloadV6 : public TSatBase
    /**
    This data structure contains the details of the MMS NOTIFICATION DOWNLOAD envelope command 
    issued to the UICC. See ETSI TS 102 223 and RSat::MmsNotificationDownload() for 
    more information about the command.

	Pass a packaged version of the latest "TMMSNotificationDownloadV6" class. 
    For Symbian OS version 9.4 use RSat::TMMSNotificationDownloadV6Pckg, a 
    packaged version of RSat::TMMSNotificationDownloadV6.  
    For later versions of Symbian OS, check for an updated API (RSat::TMMSNotificationDownloadV7 etc).

    @see RSat::TMMSNotificationDownloadV6Pckg
    @see RSat::MmsNotificationDownload()
    */
        {
    public:
        IMPORT_C TMmsNotificationDownloadV6();
    public:
 		TDeviceId iDeviceId; //< The device identification is set to Network or ME
		TMMSNotification iMMSNotification; //<It contains the additional information requested by the network to retrieve a Multimedia Message(i.e MM1_notification.REQ).
		TBool	iLastEnvelope; //< Indicates the last envelope sent to transmit the MMS notification into the card. 
       	};

    /** A packaged RSat::TMMSNotificationDownloadV6. */
    typedef TPckg<TMmsNotificationDownloadV6> TMmsNotificationDownloadV6Pckg;

    IMPORT_C void MmsNotificationDownload(TRequestStatus& aReqStatus, const TDesC8& aMmsNotificationDownload) const;   
    
    //
    // Multimedia Service Transfer Status
    //
    
    /** 
    Multimedia Service Transfer Status is the status information of a Submission/Reception File
    It contains the details of MM1_Submit.RES/MM1_Retrieve.RES.
    
    The UICC is able to identify if the envelope corresponds to a previous submit or retrieve MMS by using the MMS Tranfer File 
    and the Multimedia Message Identifier that shall be the same between both commands.
	Refer to section 7.7 of 3GPP TS 31.111 Release 6 for more information.
	
	@see RSat::TMultimediaMessageStatus
	*/
    enum  {KMaxMultimediaMessageStatusSize=255};
    
    /** 
    Contains the Multimedia Transfer status information.	
    @see RSat::TMmsTransferStatusV6 
    */
    typedef TBuf<KMaxMultimediaMessageStatusSize> TMultimediaMessageStatus;
       
    class TMmsTransferStatusV6 : public TSatBase
    /**
    This asynchronous request is used to send the transfer status information of the Multimedia Message submitted/received 
    from ME to the UICC. The ME receives this information from the network.
    
    @see RSat::TMmsTransferStatusV6Pckg
    @see RSat::MmsTransferStatus()
    */
    	{
    public:
        IMPORT_C TMmsTransferStatusV6();
    public:
 		TDeviceId iDeviceId; //< The device identification is set to Network or ME
		TFileList iMMSTransferFile; //<MMS Transfer File is the path of the MMS Reception File or the MMS Submission File 
		TMultimediaMessageStatus  iMultimediaMessageStatus; //< MM Transfer status information	
    	TMultimediaMessageId iMultimediaMessageId;  //<This information is used in identifying a Multimedia Message when MMS Reception or Submission file are capable of storing several MMs.
    	};
    	
    /** A packaged RSat::TMmsTransferStatusV6. */
    typedef TPckg<TMmsTransferStatusV6> TMmsTransferStatusV6Pckg;
	
    IMPORT_C void MmsTransferStatus(TRequestStatus& aReqStatus, const TDesC8& aMmsTransferStatus) const;

	//USSD Data Download in Application mode
	
	class TUssdDataDownloadV6:TSatBase
	/**
	 This data structure contains the details of the USSD Data Download ENVELOPE command 
	 issued to the UICC. See ETSI TS 102 223 and RSat::UssdDataDownload() for
	 more information about the command.
	  
	 Pass a packaged version of the latest "TUssdDataDownloadV6" class. 
	 For Symbian OS version 9.4 use RSat::TUssdDataDownloadV6Pckg, a 
	 packaged version of RSat::TUssdDataDownloadV6.  
	 For later versions of Symbian OS, check for an updated API (RSat::TUssdDataDownloadV7 etc).
	  
	  
    @see RSat::TUssdDataDownloadV6Pckg
    @see RSat::UssdDataDownload()
    */
		{
	public:
		IMPORT_C TUssdDataDownloadV6();	
	public:
		TDeviceId iDeviceId; //< The device identification is set to Network 
		TUssdString iUssdString; //< An Unstructured Supplementary Services Data string.
		};
	
	 /** A packaged RSat::TUssdDataDownloadV6. */
    typedef TPckg<TUssdDataDownloadV6> TUssdDataDownloadV6Pckg;
    
    IMPORT_C  void UssdDataDownload(TRequestStatus& aReqStatus, const TDesC8& aDes) const;
    
protected:
    IMPORT_C virtual void ConstructL();
    IMPORT_C virtual void Destruct();
 
private:
    RSat(const RSat& aSat); //< Default copy constructor
    CSatPtrHolder* iEtelSatPtrHolder;
    };

#endif // __ETELSAT_H__
