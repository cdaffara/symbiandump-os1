// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file define the classes for the different PDU elements
// 
//



/**
 @file
 @publishedAll
*/

#ifndef __GSMUELEM_H__
#define __GSMUELEM_H__

// Includes

#include <e32std.h>
#include <e32base.h>
#include <charconv.h>
#include <gsmunmspacemobph.h>

class RFs;
class RReadStream;
class RWriteStream;
class CSmsEMSBufferSegmenter;
struct TEncodeParams;

// class declarations

/**
 *  Decoding error.
 *  @publishedAll
 *  @released 
 */
const TInt KErrGsmuDecoding = KErrCorrupt;

/**
 *  Thin wrapper over TLex8 to provide leaving functions when getting the next
 *  character(s).
 *  
 *  These functions leave with KErrGsmuDecoding if there are insufficient characters
 *  remaining.
 *  @publishedAll
 *  @released
 */
class TGsmuLex8 : public TLex8
	{
	public:
		TGsmuLex8(const TDesC8& aSource);
		TUint8 GetL();
		void IncL(TInt aNumber = 1);
		TPtrC8 NextWithNoIncL(TInt aLength) const;
		TPtrC8 NextWithNoIncL(TInt aLength, TBool aAcceptTruncation) const;		
		TPtrC8 NextAndIncL(TInt aLength);

	private:
		inline void LeaveL() const;
	};


/**
 *  Base class for performing all operations on octets.
 *  @publishedAll
 *  @released 
 */
class TSmsOctet
	{
public:
	inline TSmsOctet(TInt aValue);
	inline TSmsOctet(TUint8 aValue = 0);
	inline operator TInt() const;
	const TSmsOctet& operator = (TInt aValue);
	inline void FillSemiOctets(TInt aNum);
	inline TInt SemiOctetsToNum() const;
	TUint8* EncodeL(TUint8* aPtr) const;
	inline void DecodeL(TGsmuLex8& aPdu);
	inline void InternalizeL(RReadStream& aStream);
	inline void ExternalizeL(RWriteStream& aStream) const;
protected:
	/** Octet value. */
	TUint8 iValue;
	};


/**
 *  Bit masks for key first octet of an SMS PDU
 *  @publishedAll
 *  @released
 */
class TSmsFirstOctet : public TSmsOctet
	{
public:
/** TP-MTI (Message Type Indicator) First octet of all SMS PDUs. */
	enum TSmsMTI
		{
	/** SMS-DELIVER or SMS-DELIVER-REPORT message. */
		ESmsMTIDeliverOrDeliverReport=0x00,
	/** SMS-SUBMIT or SMS-SUBMIT-REPORT message. */
		ESmsMTISubmitOrSubmitReport=0x01,
	/** SMS-STATUS or SMS-COMMAND message. */
		ESmsMTIStatusReportOrCommand=0x02,

	/** Mask for these bit flags. */
		ESmsMTIMask=0x03
		};

/**
 *  TP-UDHI (User Data Header Indicator) Found in first octet of
 *  Submit and Deliver and possibly, Deliver Report and Status Report. 
 */
	enum TSmsUDHI
		{
	/** User data header indicator not present. */
		ESmsUDHIHeaderNotPresent=0x00,
	/** User data header indicator present. */
		ESmsUDHIHeaderPresent=0x40,

	/** Mask for these bit flags. */
		ESmsUDHIMask=0x40
		};

/** TP-RP (Reply Path) Found in first octet of Submit and Deliver. */
	enum TSmsReplyPath
		{
	/** Reply path does not exist. */
		ESmsReplyPathNone=0x00,
	/** Reply path exists. */
		ESmsReplyPathExists=0x080,

	/** Mask for these bit flags. */
		ESmsReplyPathMask=0x80
		};

/** TP-MMS (More Messages To Send) Found in Deliver and Status Report. */
	enum TSmsMoreMessagesToSend
		{
	/** More messages to send. */
		ESmsMoreMessagesToSend=0x00,
	/** No more messages to send. */
		ESmsNoMoreMessagesToSend=0x04,

	/** Mask for these bit flags. */
		ESmsMoreMessagesToSendMask=0x04,
		};

/** T-SRI (Status Report Indicator) Found in Status Report. */
	enum TSmsStatusReportIndicator
		{
	/** Status report is not going to be returned. */
		ESmsStatusReportNotReturned=0x00,
	/** Status report is going to be returned. */
		ESmsStatusReportReturned=0x20,

	/** Mask for these bit flags. */
		ESmsStatusReportIndicatorMask=0x20
		};

/** TP-RD bit flags. */
	enum TSmsRejectDuplicates
		/**
		 *  T-RD (Reject Duplicates) Found in Submit
		 */
		{
	/** Accept duplicates. */
		ESmsAcceptDuplicates=0x00,
	/** Reject duplicates. */
		ESmsRejectDuplicates=0x04,

	/** Mask for these bit flags. */
		ESmsRejectDuplicatesMask=0x04
		};

/** T-SRQ (Status Report Qualifier). Found in Status Report. */
	enum TSmsStatusReportQualifier
		{
	/** Status report result of submit. */
		ESmsStatusReportResultOfSubmit=0x00,
	/** Status report result of command. */
		ESmsStatusReportResultOfCommand=0x20,

	/** Mask for these bit flags. */
		ESmsStatusReportQualifierMask=0x20
		};

/** TP-VPF (Validity Period Format). Found in Submit. */
	enum TSmsValidityPeriodFormat
		{
	/** TP-VP field not present. */
		ESmsVPFNone=0x00,
	/** TP-VP field present. Enhanced format (7 octets). */
		ESmsVPFEnhanced=0x08,	// was ESmsVPFReserved, the new GSM spec!
	/** TP-VP field present, relative validity format. */
		ESmsVPFInteger=0x10,    // relative validity period
	/** TP-VP field present, absolute validity format. */
		ESmsVPFSemiOctet=0x18,  // absolute validity period

	/** Mask for these bit flags. */
		ESmsVPFMask=0x18
		};

/** TP-SRR (Status Report Request) Found in Submit and Command. */
	enum TSmsStatusReportRequest
		{
	/** Status report is not requested. */
		ESmsStatusReportNotRequested=0x00,
	/** Status report is requested. */
		ESmsStatusReportRequested=0x20,

	/** Mask for these bit flags. */
		ESmsStatusReportRequestMask=0x20
		};

public:
	TSmsFirstOctet(TInt aValue=0);
	const TSmsFirstOctet& operator = (TInt aValue);  //  Review
	};


/**
 *  TP-FCS (Failure Cause) octet. For Deliver and Submit errors
 *  @publishedAll
 *  @released
 */
class TSmsFailureCause : public TSmsOctet
	{
public:
/** TP-FCS flags. */
	enum TSmsFailureCauseError
		{
//  Reserved values
//		....
//		....
//  PID errors
	/** Telematic interworking not supported. */
		ESmsPIDErrorTelematicInterworkingNotSupported=0x80,
	/** Short message Type 0 not supported. */
		ESmsPIDErrorShortMessageType0NotSupported=0x81,
	/** Cannot replace short message. */
		ESmsPIDErrorCannotReplaceShortMessage=0x82,
	/** Reserved. */
		ESmsPIDErrorReserved1=0x83,
	/** Reserved. */
		ESmsPIDErrorReserved2=0x84,
	/** Reserved. */
		ESmsPIDErrorReserved3=0x85,
	/** Reserved. */
		ESmsPIDErrorReserved4=0x86,
	/** Reserved. */
		ESmsPIDErrorReserved5=0x87,
	/** Reserved. */
		ESmsPIDErrorReserved6=0x88,
	/** Reserved. */
		ESmsPIDErrorReserved7=0x89,
	/** Reserved. */
		ESmsPIDErrorReserved8=0x8A,
	/** Reserved. */
		ESmsPIDErrorReserved9=0x8B,
	/** Reserved. */
		ESmsPIDErrorReserved10=0x8C,
	/** Reserved. */
		ESmsPIDErrorReserved11=0x8D,
	/** Reserved. */
		ESmsPIDErrorReserved12=0x8E,
	/** Unspecified TP-PID error. */
		ESmsPIDErrorUnspecified=0x8F,
//  DCS errors
	/** Data coding scheme (alphabet) not supported. */
		ESmsDCSErrorAlphabetNotSupported=0x90,
	/** Message class not supported. */
		ESmsDCSErrorMessageClassNotSupported=0x91,
	/** Reserved. */
		ESmsDCSErrorReserved1=0x92,
	/** Reserved. */
		ESmsDCSErrorReserved2=0x93,
	/** Reserved. */
		ESmsDCSErrorReserved3=0x94,
	/** Reserved. */
		ESmsDCSErrorReserved4=0x95,
	/** Reserved. */
		ESmsDCSErrorReserved5=0x96,
	/** Reserved. */
		ESmsDCSErrorReserved6=0x97,
	/** Reserved. */
		ESmsDCSErrorReserved7=0x98,
	/** Reserved. */
		ESmsDCSErrorReserved8=0x99,
	/** Reserved. */
		ESmsDCSErrorReserved9=0x9A,
	/** Reserved. */
		ESmsDCSErrorReserved10=0x9B,
	/** Reserved. */
		ESmsDCSErrorReserved11=0x9C,
	/** Reserved. */
		ESmsDCSErrorReserved12=0x9D,
	/** Reserved. */
		ESmsDCSErrorReserved13=0x9E,
	/** Reserved. */
		ESmsDCSErrorUnspecified=0x9F,
//  CommandErrors
	/** Command cannot be actioned. */
		ESmsCommandErrorCannotBeActioned=0xA0,
	/** Command unsupported. */
		ESmsCommandErrorUnsupported=0xA1,
	/** Reserved. */
		ESmsCommandErrorReserved1=0xA2,
	/** Reserved. */
		ESmsCommandErrorReserved2=0xA3,
	/** Reserved. */
		ESmsCommandErrorReserved3=0xA4,
	/** Reserved. */
		ESmsCommandErrorReserved4=0xA5,
	/** Reserved. */
		ESmsCommandErrorReserved5=0xA6,
	/** Reserved. */
		ESmsCommandErrorReserved6=0xA7,
	/** Reserved. */
		ESmsCommandErrorReserved7=0xA8,
	/** Reserved. */
		ESmsCommandErrorReserved8=0xA9,
	/** Reserved. */
		ESmsCommandErrorReserved9=0xAA,
	/** Reserved. */
		ESmsCommandErrorReserved10=0xAB,
	/** Reserved. */
		ESmsCommandErrorReserved11=0xAC,
	/** Reserved. */
		ESmsCommandErrorReserved12=0xAD,
	/** Reserved. */
		ESmsCommandErrorReserved13=0xAE,
	/** Unspecified TP-Command error. */
		ESmsCommandErrorUnspecified=0xAF,
//
	/** PDU not supported. */
		ESmsErrorPDUNotSupported=0xB0,
//  Reserved values
//		....
//		....
	/** SC busy. */
		ESmsErrorSCBusy=0xC0,
	/** No SC subscription. */
		ESmsErrorNoSCSubscription=0xC1,
	/** SC system failure. */
		ESmsErrorNoSCSystemFailure=0xC2,
	/** Invalid SME address. */
		ESmsErrorInvalidSMEAddress=0xC3,
	/** Destination SME barred. */
		ESmsErrorDestinationSMEBarred=0xC4,
	/** SM Rejected-Duplicate SM. */
		ESmsErrorSMRejectedDuplicateSM=0xC5,
	/** TP-VPF not supported. */
		ESmsErrorVPFNotSupported=0xC6,
	/** TP-VP not supported. */
		ESmsErrorVPNotSupported=0xC7,
//  Reserved values
//		....
//		....
	/** SIM SMS storage full. */
		ESmsErrorSIMSMSStorageFull=0xD0,
	/** No SMS storage capability in (U)SIM. */
		ESmsErrorNoSMSStorageCapabilityOnSIM=0xD1,
	/** Error in MS. */
		ESmsErrorErrorInMS=0xD2,
	/** Memory Capacity Exceeded. */
		ESmsErrorMemoryCapacityExceded=0xD3,
	/** (U)SIM Application Toolkit Busy. */
		ESmsErrorSIMApplicationToolkitBusy=0xD4,
	/** (U)SIM data download error. */
		ESmsErrorSIMDataDownloadError=0xD5,
//  Reserved values
//		....
//		....
	/** Value specific to an application. */
		ESmsApplicationError1=0xE0,
	/** Value specific to an application. */
		ESmsApplicationError2=0xE1,
	/** Value specific to an application. */
		ESmsApplicationError3=0xE2,
	/** Value specific to an application. */
		ESmsApplicationError4=0xE3,
	/** Value specific to an application. */
		ESmsApplicationError5=0xE4,
	/** Value specific to an application. */
		ESmsApplicationError6=0xE5,
	/** Value specific to an application. */
		ESmsApplicationError7=0xE6,
	/** Value specific to an application. */
		ESmsApplicationError8=0xE7,
	/** Value specific to an application. */
		ESmsApplicationError9=0xE8,
	/** Value specific to an application. */
		ESmsApplicationError10=0xE9,
	/** Value specific to an application. */
		ESmsApplicationError11=0xEA,
	/** Value specific to an application. */
		ESmsApplicationError12=0xEB,
	/** Value specific to an application. */
		ESmsApplicationError13=0xEC,
	/** Value specific to an application. */
		ESmsApplicationError14=0xED,
	/** Value specific to an application. */
		ESmsApplicationError15=0xEE,
	/** Value specific to an application. */
		ESmsApplicationError16=0xEF,
	/** Value specific to an application. */
		ESmsApplicationError17=0xF0,
	/** Value specific to an application. */
		ESmsApplicationError18=0xF1,
	/** Value specific to an application. */
		ESmsApplicationError19=0xF2,
	/** Value specific to an application. */
		ESmsApplicationError20=0xF3,
	/** Value specific to an application. */
		ESmsApplicationError21=0xF4,
	/** Value specific to an application. */
		ESmsApplicationError22=0xF5,
	/** Value specific to an application. */
		ESmsApplicationError23=0xF6,
	/** Value specific to an application. */
		ESmsApplicationError24=0xF7,
	/** Value specific to an application. */
		ESmsApplicationError25=0xF8,
	/** Value specific to an application. */
		ESmsApplicationError26=0xF9,
	/** Value specific to an application. */
		ESmsApplicationError27=0xFA,
	/** Value specific to an application. */
		ESmsApplicationError28=0xFB,
	/** Value specific to an application. */
		ESmsApplicationError29=0xFC,
	/** Value specific to an application. */
		ESmsApplicationError30=0xFD,
	/** Value specific to an application. */
		ESmsApplicationError31=0xFE,
//
	/** Unspecified error cause. */
		ESmsErrorUnspecified=0xFF,

	/**	@publishedAll	*/
		ESmsErrorFree=0x100
		};
public:
	TSmsFailureCause();
	inline TInt Error() const;
	inline void SetError(TSmsFailureCauseError aError);
	};


/**
 *  TP-ST (Status Report) octet. Found in Status Report
 *  @publishedAll
 *  @released
 */
class TSmsStatus : public TSmsOctet
	{
public:
/** TP-ST flag values. */
	enum TSmsStatusValue
		{
//  Short message transaction completed
	/** Short message received by the SME. */
		ESmsShortMessageReceivedBySME=0x00,
	/**
	 *  Short message forwarded by the SC to the SME but the SC is unable to confirm
	 *  	delivery. 
	 */
		ESmsShortMessageForwardedBySCToSMEButUnconfirmedBySC=0x01,
	/** Short message replaced by the SC. */
		ESmsShortMessageReplacedBySC=0x02,
//  Reserved values
//		....
//		....
//  Temporary error, SC still trying to transfer SM
	/** Temporary error, SC still trying to transfer SM: congestion. */
		ESmsTempError1StatusCongestion=0x20,
	/** Temporary error, SC still trying to transfer SM: SME busy. */
		ESmsTempError1StatusSMEBusy=0x21,
	/** Temporary error, SC still trying to transfer SM: No response from SME. */
		ESmsTempError1StatusNoResponseFromSME=0x22,
	/** Temporary error, SC still trying to transfer SM: Service rejected. */
		ESmsTempError1StatusServiceRejected=0x23,
	/** Temporary error, SC still trying to transfer SM: Quality of service not available. */
		ESmsTempError1StatusQualityOfServiceNotAvailable=0x24,
	/** Temporary error, SC still trying to transfer SM: Error in SME. */
		ESmsTempError1StatusErrorInSME=0x25,
//  Reserved values
//		....
//		....
//  Permanent error, SC is not making any more transfer attempts
	/**
	 *  Permanent error, SC is not making any more transfer attempts: Remote procedure
	 *  	error. 
	 */
		ESmsPermErrorRemoteProcedureError=0x40,
	/**
	 *  Permanent error, SC is not making any more transfer attempts: Incompatible
	 *  	destination. 
	 */
		ESmsPermErrorIncompatibleDestination=0x41,
	/**
	 *  Permanent error, SC is not making any more transfer attempts: Connection rejected
	 *  	by SME. 
	 */
		ESmsPermErrorConnectionRejectedBySME=0x42,
	/** Permanent error, SC is not making any more transfer attempts: Not obtainable. */
		ESmsPermErrorNotObtainable=0x43,
	/**
	 *  Permanent error, SC is not making any more transfer attempts: Quality of service
	 *  	not available. 
	 */
		ESmsPermErrorQualityOfServiceNotAvailable2=0x44,
	/**
	 *  Permanent error, SC is not making any more transfer attempts: No interworking
	 *  	available. 
	 */
		ESmsPermErrorNoInterworkingAvailable=0x45,
	/**
	 *  Permanent error, SC is not making any more transfer attempts: SM Validity Period
	 *  	Expired. 
	 */
		ESmsPermErrorSMValidityPeriodExpired=0x46,
	/**
	 *  Permanent error, SC is not making any more transfer attempts: SM Deleted by
	 *  	originating SME. 
	 */
		ESmsPermErrorSMDeletedByOriginatingSME=0x47,
	/**
	 *  Permanent error, SC is not making any more transfer attempts: SM Deleted by
	 *  	SC Administration. 
	 */
		ESmsPermErrorSMDeletedBySCAdministration=0x48,
	/** Permanent error, SC is not making any more transfer attempts: SM does not exist. */
		ESmsPermErrorDoesNotExist=0x49,
//  Reserved values
//		....
//		....
//  Temporary error, SC is not making any more transfer attempts
	/** Temporary error, SC is not making any more transfer attempts: Congestion. */
		ESmsTempError2Congestion=0x60,
	/** Temporary error, SC is not making any more transfer attempts: SME Busy. */
		ESmsTempError2SMEBusy=0x61,
	/**
	 *  Temporary error, SC is not making any more transfer attempts: No response from
	 *  	SME. 
	 */
		ESmsTempError2NoResponseFromSME=0x62,
	/** Temporary error, SC is not making any more transfer attempts: Service rejected. */
		ESmsTempError2ServiceRejected=0x63,
	/**
	 *  Temporary error, SC is not making any more transfer attempts: Quality of service
	 *  	not available. 
	 */
		ESmsTempError2QualityOfServiceNotAvailable=0x64,
	/** Temporary error, SC is not making any more transfer attempts: Error in SME. */
		ESmsTempError2ErrorInSME=0x65,
//  Reserved values
//		....
//		....
		};
public:
	TSmsStatus();
	inline TSmsStatus::TSmsStatusValue Status() const;
	inline void SetStatus(TSmsStatusValue aValue);
	};


/**
 *  TP-PI octet. Found in Reports and Commands
 *  @publishedAll
 *  @released
 */
class TSmsParameterIndicator : public TSmsOctet
	{
public:
/** TP-PI bit flags. */
	enum TSmsPIBits
		{
	/** Protocol identifier present. */
		ESmsPIDProtocolIdentifierPresent=0x01,
	/** Data coding scheme present. */
		ESmsPIDDataCodingSchemePresent=0x02,
	/** User data present. */
		ESmsPIDUserDataPresent=0x04,
	/** Reserved. */
		ESmsPIDReserved1=0x08,
	/** Reserved. */
		ESmsPIDReserved2=0x10,
	/** Reserved. */
		ESmsPIDReserved3=0x20,
	/** Reserved. */
		ESmsPIDReserved4=0x40,
	/** Extension. */
		ESmsPIDExtension=0x80
		};
public:
	TSmsParameterIndicator();
	inline TBool Extension() const;
	inline void SetExtension(TBool aExtension);
	inline TBool UserDataPresent() const;
	inline void SetUserDataPresent(TBool aPresent);
	inline TBool DataCodingSchemePresent() const;
	inline void SetDataCodingSchemePresent(TBool aPresent);
	inline TBool ProtocolIdentifierPresent() const;
	inline void SetProtocolIdentifierPresent(TBool aPresent);
	};

// TSmsPID - conversion (None, Fax, Mail, etc.)

/**
 *  TP-PID PDU octet. Found in ALL 6 message types.
 *  @publishedAll
 *  @released
 */
class TSmsProtocolIdentifier : public TSmsOctet
	{
public:
/** PID bits 7-6, which determine the meaning of the lower order bits. */
	enum TSmsPIDType
		{
	/** Telematic interworking. */
		ESmsPIDTelematicInterworking=0x00,
	/** Short message. */
		ESmsPIDShortMessageType=0x40,
	/** Reserved. */
		ESmsPIDReserved=0x80,
	/** SC specific use. */
		ESmsPIDSCSpecificUse=0xC0,

	/** Mask of bits 7-6. */
		ESmsPIDTypeMask=0xC0
		};

/** Telematic device indicator flags. Bit 5 - When Bit 7 = 0, Bit 6 = 0 */
	enum TSmsTelematicDeviceIndicator
		{
	/** No telematic device. */
		ESmsNoTelematicDevice=0x00,
	/** Telematic device. */
		ESmsTelematicDevice=0x20,

	/** Telematic device indicator mask. */
		EPIDTelematicDeviceIndicatorMask=0x20
		};

/** Telematic Interworking device type flags. Bits 4-0 - When Bit 5 = 1*/
	enum TSmsTelematicDeviceType
		{
	/** Device type is specific to this SC. */
		ESmsSCSpecificDevice=0x00,
	/** Telex. */
		ESmsTelex=0x01,
	/** Group 3 telefax. */
		ESmsGroup3TeleFax=0x02,
	/** Group 4 telefax. */
		ESmsGroup4TeleFax=0x03,
	/** Voice telephone. */
		ESmsVoiceTelephone=0x04,
	/** ERMES. */
		ESmsERMES=0x05,
	/** National Paging System. */
		ESmsNationalPagingSystem=0x06,
	/** Videotex. */
		ESmsVideotex=0x07,
	/** Teletex, carrier unspecified. */
		ESmsTeletexCarrierUnspecified=0x08,
	/** Teletex, PSPDN. */
		ESmsTeletexPSPDN=0x09,
	/** Teletex, CSPDN. */
		ESmsTeletexCSPDN=0x0A,
	/** Teletex, in analog PSTN. */
		ESmsTeletexAnaloguePSTN=0x0B,
	/** Teletex, in digital ISDN */
		ESmsTeletexDigitalISDN=0x0C,
	/** UCI (Universal Computer Interface, ETSI DE/PS 3 01-3). */
		ESmsUCI=0x0D,
	/** Reserved. */
		ESmsReserved1=0x0E,
	/** Reserved. */
		ESmsReserved2=0x0F,
	/** A message handling facility. */
		ESmsMessageHandlingFacility=0x10,
	/** X.400 message handling system. */
		ESmsX400MessageHandlingSystem=0x11,
	/** Internet Electronic Mail. */
		ESmsInternetElectronicMail=0x12,
	/** Reserved. */
		ESmsReserved3=0x13,
	/** Reserved. */
		ESmsReserved4=0x14,
	/** Reserved. */
		ESmsReserved5=0x15,
	/** Reserved. */
		ESmsReserved6=0x16,
	/** Reserved. */
		ESmsReserved7=0x17,
	/** Value specific to each SC. */
		ESmsSCSpecific1=0x18,
	/** Value specific to each SC. */
		ESmsSCSpecific2=0x19,
	/** Value specific to each SC. */
		ESmsSCSpecific3=0x1A,
	/** Value specific to each SC. */
		ESmsSCSpecific4=0x1B,
	/** Value specific to each SC. */
		ESmsSCSpecific5=0x1C,
	/** Value specific to each SC. */
		ESmsSCSpecific6=0x1D,
	/** Value specific to each SC. */
		ESmsSCSpecific7=0x1E,
	/** Value specific to each SC. */
		ESmsGsmMobileStation=0x1F,

	/** Mask for telematic device type bits. */
		ESmsTelematicDeviceTypeMask=0x1F
		};

/** SM-AL protocol flag. No Telematic Interworking. Bits 4-0 - When Bit 5 = 0*/
	enum TSmsShortMessageALProtocol
		{
	/** Mask for SM-AL protocol flag bit. */
		ESmsShortMessageALProtocolMask=0x0F
		};

/** Short Message Type flags. Bits 5-0 - When Bit 7 = 1, Bit 6 = 0*/
	enum TSmsShortMessageType
		{
	/** Short Message Type 0. */
		ESmsShortMessageType0=0x00,
	/** Replace Short Message Type 1. */
		ESmsReplaceShortMessageType1=0x01,
	/** Replace Short Message Type 2. */
		ESmsReplaceShortMessageType2=0x02,
	/** Replace Short Message Type 3. */
		ESmsReplaceShortMessageType3=0x03,
	/** Replace Short Message Type 4. */
		ESmsReplaceShortMessageType4=0x04,
	/** Replace Short Message Type 5. */
		ESmsReplaceShortMessageType5=0x05,
	/** Replace Short Message Type 6. */
		ESmsReplaceShortMessageType6=0x06,
	/** Replace Short Message Type 7. */
		ESmsReplaceShortMessageType7=0x07,
//  Reserved values
//		....
//		....
	/** Return Call Message. */
		ESmsReturnCallMesage=0x1F,
//  Reserved values
//		....
//		....
		ESmsAnsi136RData=0x3C,
 		ESmsMEDataDownload=0x3D,
	/** ME De-personalization Short Message. */
		ESmsMEDepersonalizationShortMessage=0x3E,
	/** ME Data download. */
		ESmsSIMDataDownLoad=0x3F,

//		ESmsShortMessageTypeMask=0x1F
	/** Mask for Short Message type bits. */
		ESmsShortMessageTypeMask=0x3F
		};
public:
	TSmsProtocolIdentifier();
	inline TSmsPIDType PIDType() const;
	inline void SetPIDType(TSmsPIDType aSmsPIDType);
	TSmsTelematicDeviceIndicator TelematicDeviceIndicator() const;
	void SetTelematicDeviceIndicator(TSmsTelematicDeviceIndicator aIndicator);
	TSmsTelematicDeviceType TelematicDeviceType() const;
	void SetTelematicDeviceType(TSmsTelematicDeviceType aDeviceType);
	TInt ShortMessageALProtocol() const;
	void SetShortMessageALProtocol(TSmsShortMessageALProtocol aProtocol);
	TInt ShortMessageType() const;
	void SetShortMessageType(TSmsShortMessageType aShortMessageType);
	};


/**
 *  TP-DCS Data Coding Scheme defined in 3GPP TS 23.038.
 *  
 *  The data coding scheme is not always present in an GSM SMS PDU (CSmsPDU).
 *  It is always present for a SUBMIT (CSmsSubmit) and a DELIVER (CSmsDeliver),
 *  and is sometimes present for a DELIVER REPORT (CSmsDeliverReport), a SUBMIT
 *  REPORT (CSmsSubmitReport) or a STATUS REPORT (CSmsStatusReport), depending
 *  on the parameter indicator (TSmsParameterIndicator).
 *  @publishedAll
 *  @released 
 */
class TSmsDataCodingScheme : public TSmsOctet
	{
public:
/** Flags for bits 7-4, which determine the meaning of the lower order bits. */
	enum TSmsDCSBits7To4
		{
	/** Text is uncompressed, no class information. */
		ESmsDCSTextUncompressedWithNoClassInfo=0x00,
	/** Text is uncompressed, class information present. */
		ESmsDCSTextUncompressedWithClassInfo=0x10,
	/** Text is compressed, no class information. */
		ESmsDCSTextCompressedWithNoClassInfo=0x20,
	/** Text is compressed, class information present. */
		ESmsDCSTextCompressedWithClassInfo=0x30,

	/** Auto Deletion, Text is uncompressed, no class information. */
		ESmsDCSAutoDelNoClassInfoUncompressedText=0x40,
	/** Auto Deletion, Text is uncompressed, class information present. */
		ESmsDCSAutoDelClassInfoUncompressedText=0x50,
	/** Auto Deletion, Text is compressed, no class information. */
		ESmsDCSAutoDelNoClassInfoCompressedText=0x60,
	/** Auto Deletion, Text is compressed, class information present. */
		ESmsDCSAutoDelClassInfoTextCompressedText=0x70,

	/** Reserved. */
		ESmsDCSReserved1=0x40,
	/** Reserved. */
		ESmsDCSReserved2=0x50,
	/** Reserved. */
		ESmsDCSReserved3=0x60,
	/** Reserved. */
		ESmsDCSReserved4=0x70,
	/** Reserved. */
		ESmsDCSReserved5=0x80,
	/** Reserved. */
		ESmsDCSReserved6=0x90,
	/** Reserved. */
		ESmsDCSReserved7=0xA0,
	/** Reserved. */
		ESmsDCSReserved8=0xB0,



	/** Message Waiting Indication Group: Discard Message. */
		ESmsDCSMessageWaitingIndicationDiscardMessage=0xC0,

	/** Message Waiting Indication Group (7 bit). */
		ESmsDCSMessageWaitingIndication7Bit=0xD0,  //  7 bit User data
	/** Message Waiting Indication Group (UCS2). */
		ESmsDCSMessageWaitingIndicationUCS2=0xE0,  //  unicode User data

	/** Text uncompressed, 7 bit or 8 bit. */
		ESmsDCSTextUncompressed7BitOr8Bit=0xF0,

	/** Masks bits 7 to 4. */
		ESmsDCSBits7To4Mask=0xF0
		};

/** Message Marked for Automatic Deletion */
    enum TAutomaticDeletionGroup
        {
        ESmsDCSAutomaticDeletion     = 0x40,
        ESmsDCSAutomaticDeletionMask = 0xC0
        };

/** Alphabet bit flags. */
	enum TSmsAlphabet
		{
	/** GSM 7 bit default alphabet. */
		ESmsAlphabet7Bit=0x00,
	/** 8-bit data. */
		ESmsAlphabet8Bit=0x04,
	/** UCS2. */
		ESmsAlphabetUCS2=0x08,
	/** Reserved. */
		ESmsAlphabetReserved=0x0C,

	/** Mask for these bit flags. */
		ESmsAlphabetMask=0x0C,
		};
/** Message Class bit flags. */
	enum TSmsClass
		{
	/** Class 0. */
		ESmsClass0=0x00,
	/** Class 1. */
		ESmsClass1=0x01,
	/** Class 2. */
		ESmsClass2=0x02,
	/** Class 3. */
		ESmsClass3=0x03,

	/** Mask for these bit flags. */
		ESmsClassMask=0x03,
		};

/** Indication Sense bit flags. */
	enum TSmsIndicationState
		{
	/** Indication inactive. */
		ESmsIndicationInactive=0x00,
	/** Indication active. */
		ESmsIndicationActive=0x08,

	/** Mask for these bit flags. */
		ESmsIndicationStateMask=0x08
		};
/** Indication Type bit flags. */
	enum TSmsIndicationType
		{
	/** Voicemail Message Waiting. */
		ESmsVoicemailMessageWaiting=0x00,
	/** Fax Message Waiting. */
		ESmsFaxMessageWaiting=0x01,
	/** Electronic Mail Message Waiting. */
		ESmsElectronicMailMessageWaiting=0x02,
	/** Other Message Waiting. */
		ESmsFaxOtherMessageWaiting=0x03,

	/** Mask for these bit flags. */
		ESmsIndicationTypeMask=0x03
		};

public:
	TSmsDataCodingScheme();
	inline TSmsDCSBits7To4 Bits7To4() const;
	inline void SetBits7To4(TSmsDCSBits7To4 aBits7To4);
//  Normal SMS settings
	TBool TextCompressed() const;
	void SetTextCompressed(TBool aCompressed);
	TSmsAlphabet Alphabet() const;
	void SetAlphabet(TSmsAlphabet aAlphabet);
	TBool Class(TSmsClass& aClass) const;
	void SetClass(TBool aClassDefined,TSmsClass aClass);
//  Message waiting settings
	TSmsIndicationState IndicationState() const;
	void SetIndicationState(TSmsIndicationState aState);
	TSmsIndicationType IndicationType() const;
	void SetIndicationType(TSmsIndicationType aType);
	};


/**
 *  Specifies alternative 7bit encoding combinations to use if the default
 *  GSM encoding cannot encode the message as 7bit without data loss.
 * 
 *  @publishedAll
 *  @released 
 */
enum TSmsEncoding
	{
	/** Default value meaning that no alternative encoding would be used. */
	ESmsEncodingNone = 0,
	
	/**
	 *  Allows the use of the Turkish Single Shift table in place of the
	 *  default GSM shift table.
	 * 
	 *  @note If used during encoding there will be a 3 octet cost in the User Data
	 *        Header.
	 */
	ESmsEncodingTurkishSingleShift = 0x11,

	/**
	 *  Allows the use of the Turkish Locking Shift table in place of the
	 *  default GSM alphabet table.
	 * 
	 *  @note If used during encoding there will be a 3 octet cost in the User Data
	 *        Header.
	 *  @note This encoding should only be used if the relevant national
	 *        regulatory body has requested its use.
	 */
	ESmsEncodingTurkishLockingShift	= 0x12,

    /**
     *  Allows the use of the Turkish Locking Shift and/or the Turkish Single
     *  Shift tables in place of the default GSM alphabet and shift tables.
	 * 
	 *  @note If used during encoding there will be a 3 or 6 octet cost in the User Data
	 *        Header.
	 *  @note This encoding should only be used if the relevant national
	 *        regulatory body has requested its use.
	 */
	ESmsEncodingTurkishLockingAndSingleShift = 0x13,

	/**
	 *  Allows the use of the Spanish Single Shift table in place of the
	 *  default GSM shift table.
	 * 
	 *  @note If used during encoding there will be a 3 octet cost in the User Data
	 *        Header.
	 */
	ESmsEncodingSpanishSingleShift = 0x21,

	/**
	 *  Allows the use of the Portuguese Single Shift table in place of the
	 *  default GSM shift table.
	 * 
	 *  @note If used during encoding there will be a 3 octet cost in the User Data
	 *        Header.
	 */
	ESmsEncodingPortugueseSingleShift = 0x31,

	/**
	 *  Allows the use of the Portuguese Locking Shift table in place of the
	 *  default GSM alphabet table.
	 * 
	 *  @note If used during encoding there will be a 3 octet cost in the User Data
	 *        Header.
	 *  @note This encoding should only be used if the relevant national
	 *        regulatory body has requested its use.
	 */
	ESmsEncodingPortugueseLockingShift = 0x32,

    /**
     *  Allows the use of the Portuguese Locking Shift and/or the Portuguese Single
     *  Shift tables in place of the default GSM alphabet and shift tables.
	 * 
	 *  @note If used during encoding there will be a 3 or 6 octet cost in the User Data
	 *        Header.
	 *  @note This encoding should only be used if the relevant national
	 *        regulatory body has requested its use.
	 */
	ESmsEncodingPortugueseLockingAndSingleShift = 0x33
	};


/**
 *  GSM National Language Identifiers.
 * 
 *  @publishedAll
 *  @released 
 */
enum TSmsNationalLanguageIdentifier
	{
	/** Turkish. */
	ESmsNationalLanguageIdentifierTurkish = 1,

	/** Spanish. */
	ESmsNationalLanguageIdentifierSpanish = 2,

	/** Portuguese. */
	ESmsNationalLanguageIdentifierPortuguese = 3
	};


/**
 *  Utility to provide piecewise character set conversion to / from unpacked user
 *  data elements.
 *  @publishedAll
 *  @released 
 */
class CSmsAlphabetConverter : public CBase
	{
public:
// Construction and destruction methods
	IMPORT_C static CSmsAlphabetConverter* NewLC(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TSmsDataCodingScheme::TSmsAlphabet aSmsAlphabet,TBool aIsBinary);
	IMPORT_C ~CSmsAlphabetConverter();

// Enumerated types and structs
/**
 *  Indicates whether there is a fixed relationship between the number of characters
 *  and user data elements.
 *  
 *  For example, Unicode characters always map to a single SMS UCS2 character,
 *  while a Unicode character might map to one, two or more SMS 7-bit User Data
 *  Elements (extended 7-bit characters). 
 */
	enum TSmsAlphabetWidthConversion
		{
	/** Fixed relationship. */
		ESmsAlphabetWidthConversionFixed,
	/** Variable relationship. */
		ESmsAlphabetWidthConversionVariable
		};

	struct TSmsAlphabetConversionProperties
/**
 *  Holds the number of user data elements required for conversion from a single
 *  native character.
 *  
 *  This value is applicable only if the iWidthConversion parameter is ESmsAlphabetWidthConversionFixed. 
 */
		{
	/** Alphabet width conversion. */
		TSmsAlphabetWidthConversion iWidthConversion;
	/** Number of user data elements required for conversion from a single native character */
		TInt iUDElementsPerNativeCharacter;
		};

public:
// Conversion methods
	void ConversionPropertiesL(TSmsAlphabetConversionProperties& aConversionProperties) const;
	IMPORT_C TPtrC8 ConvertFromNativeL(const TDesC& aNativeCharacters);
	IMPORT_C TPtrC8 ConvertFromNativeL(const TDesC& aNativeCharacters,
	                                   TSmsEncoding aEncoding,
	                                   TInt& aNumberOfUnconvertibleCharacters,
	                                   TInt& aNumberOfDowngradedCharacters);
	IMPORT_C TPtrC ConvertToNativeL(const TDesC8& aUDElements);
	IMPORT_C TPtrC ConvertToNativeL(const TDesC8& aUDElements, TSmsEncoding aEncoding);

	TBool IsSupportedL(const TDesC& aDes, TInt& aNumberOfUnconvertibleCharacters,
			           TInt& aIndexOfFirstUnconvertibleCharacter);

	TBool IsSupportedL(const TDesC& aDes, TSmsEncoding aEncoding,
			           TInt& aNumberOfUnconvertibleCharacters,
			           TInt& aNumberOfDowngradedCharacters,
			           TInt& aNumberRequiringAlternativeEncoding,
			           TInt& aIndexOfFirstUnconvertibleCharacter);

    void ConvertWith7BitEncodingL(const TDesC& aDes, TDes& aRestoredDes,
                       TInt& aNumberOfUnconvertibleCharacters,
                       TInt& aNumberOfDowngradedCharacters,
                       TInt& aIndexOfFirstUnconvertibleCharacter, TBool aIsCountDowngrade);

    void ConvertWithAlternativeEncodingL(const TDesC& aDes, const TDesC& aRestoredStdDes, TSmsEncoding aEncoding,
                       TInt& aNumberOfUnconvertibleCharacters,
                       TInt& aNumberOfDowngradedCharacters,
                       TInt& aNumberRequiringAlternativeEncoding,
                       TInt& aIndexOfFirstUnconvertibleCharacter);
    
// Alternative Encoding methods
	TSmsEncoding FindBestAlternativeEncodingL(const TDesC& aNativeCharacters,
											  TSmsEncoding aSuggestedEncoding);
	void ConfirmAlternativeEncoderL(TSmsEncoding aEncoding) const;
	
// Unconverted buffer methods
	inline void ResetUnconvertedNativeCharacters();
	inline void ResetUnconvertedUDElements();
	inline TPtrC UnconvertedNativeCharacters();
	inline TPtrC8 UnconvertedUDElements();

// Getter / setters
	inline TSmsDataCodingScheme::TSmsAlphabet Alphabet() const;

private:
// Private construction methods
	CSmsAlphabetConverter(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TSmsDataCodingScheme::TSmsAlphabet aSmsAlphabet,TBool aIsBinary);
	void ConstructL();

// Private conversion preparation methods
	void PrepareForConversionFromNativeL(TSmsEncoding aEncoding);
	void PrepareForConversionToNativeL(TSmsEncoding aEncoding);

// Private Alternative Encoding methods
	void GetAlternativeEncoderIDL(TSmsEncoding aEncoding, TUint& aEncoderID) const;

// Private buffer helper methods
	TPtr16 CheckAllocBufferL(HBufC16** aBuffer,TInt aMaxLength,TInt aUsedLength);
	TPtr8 CheckAllocBufferL(HBufC8** aBuffer,TInt aMaxLength,TInt aUsedLength);

// Private constants
	enum
		{
		KMaxSmsAlphabetConversionRetries=4
		};

	enum
		{
		KMinSmsAlphabetConversionAllocIncrement=4
		};

private:

	CCnvCharacterSetConverter& iCharacterSetConverter;
	RFs& iFs;
	TSmsDataCodingScheme::TSmsAlphabet iSmsAlphabet;
	TBool iIsBinary;
	HBufC* iConvertedNativeCharacters;
	HBufC8* iConvertedUDElements;
	HBufC* iUnconvertedNativeCharacters;
	TPtr iUnconvertedNativeCharactersPtr;
	HBufC8* iUnconvertedUDElements;
	TPtr8 iUnconvertedUDElementsPtr;
	};


/** Type-of-number, as defined in ETSI 3GPP TS 23.040. */
enum TGsmSmsTypeOfNumber
	{
	/**
	 *  Unknown, used when the user or network has no a priori information about the
	 *  numbering plan.
	 *  In this case, the Address-Value field is organized according to the network
	 *  	dialling plan, e.g. prefix or escape digits might be present. 
	 */
	EGsmSmsTONUnknown=0x00,
	/**
	 *  International number.
	 *  The international format shall be accepted also when the message is destined
	 *  	to a recipient in the same country as the MSC or as the SGSN. 
	 */
		EGsmSmsTONInternationalNumber=0x10,
	/**
	 *  National number.
	 *  	Prefix or escape digits shall not be included. 
	 */
		EGsmSmsTONNationalNumber=0x20,
	/**
	 *  Network specific number.
	 *  Administration/service number specific to the serving network, e.g. used to
	 *  	access an operator. 
	 */
		EGsmSmsTONNetworkSpecificNumber=0x30,
	/**
	 *  Subscriber number.
	 *  Used when a specific short number representation is stored in one or more
	 *  SCs as part of a higher layer application. (Note that "Subscriber number"
	 *  	shall only be used in connection with the proper PID referring to this application). 
	 */
		EGsmSmsTONSubscriberNumber=0x40,
	/**
	 *  Alpha-numeric.
	 *  	Coded according to 3GPP TS 23.038 - GSM 7-bit default alphabet. 
	 */
		EGsmSmsTONAlphaNumeric=0x50,
		EGsmSmsTONAbbreviatedNumber=0x60,		//< Abbreviated number
		EGsmSmsTONReserverved=0x70,				//< Reserved for extension
	};

/** Numbering-plan-identification defined in ETSI 3GPP TS 23.040. */
enum TGsmSmsNumberingPlanIdentification
	{
	/** Unknown. */
	EGsmSmsNPIUnknown=0x00,
	/** ISDN telephone numbering plan. */
	EGsmSmsNPIISDNTelephoneNumberingPlan=0x01,
	/** Data numbering plan. */
	EGsmSmsNPIDataNumberingPlan=0x03,
	/** Telex numbering plan. */
	EGsmSmsNPITelexNumberingPlan=0x04,
	/** National numbering plan. */
	EGsmSmsNPINationalNumberingPlan=0x08,
	/** Private numbering plan. */
	EGsmSmsNPIPrivateNumberingPlan=0x09,
	/** ERMES numbering plan. */
	EGsmSmsNPIERMESNumberingPlan=0x0A,
	/** Reserved for extension. */
	EGsmSmsNPIReservedForExtension=0x0F,
	};


/**
 *  Type-of-address field defined in ETSI 3GPP TS 23.040
 *  @publishedAll
 *  @released
 */
class TGsmSmsTypeOfAddress : public TSmsOctet
	{
	public:

    	enum
    		{
    		EGsmSmsFirstBitMask=0x80,
    		EGsmSmsTONMask=0x70,
    		EGsmSmsNPIMask=0x0F
    		};

	public:

		inline TGsmSmsTypeOfAddress(TInt aValue = EGsmSmsFirstBitMask);
		inline TGsmSmsTypeOfAddress(TGsmSmsTypeOfNumber aTon, TGsmSmsNumberingPlanIdentification aNPI);

		inline TGsmSmsTypeOfNumber TON() const;
		inline void SetTON(TGsmSmsTypeOfNumber aTON);

		inline TGsmSmsNumberingPlanIdentification NPI() const;
	/**
	 *  Sets the numbering plan identification.
	 *  	@param aNPI Numbering plan identification 
	 */
		inline void SetNPI(TGsmSmsNumberingPlanIdentification aNPI);

	/**
	 *  Converts type of number and numbering plan identification information from
	 *  the type of address parameter to the NMobilePhone::TMobileTON and NMobilePhone::TMobileNPI
	 *  format.
	 *  
	 *  @param aTON On return, type of number
	 *  	@param aNPI On return, numbering plan identification 
	 */
		IMPORT_C void ConvertToETelMM(NMobilePhone::TMobileTON& aTON, NMobilePhone::TMobileNPI& aNPI) const;
	/**
	 *  Sets type of number and numbering plan identification information from values
	 *  specified in NMobilePhone::TMobileTON and NMobilePhone::TMobileNPI formats.
	 *  
	 *  @param aTON Type of number
	 *  	@param aNPI Numbering plan identification 
	 */
		IMPORT_C void SetFromETelMM(NMobilePhone::TMobileTON aTON, NMobilePhone::TMobileNPI aNPI);
	};

/** Maximum length of a Service Centre address = 21, as defined in ETSI GSM 04.11. */
const TInt TGsmSmsTelNumberMaxLen = 21;


/**
 *  Encapsulation of basic address information
 *  @publishedAll
 *  @released
 */
class TGsmSmsTelNumber
	{
	public:
        /**
         *  @publishedAll
         *  If the address is an alphanumeric address,
         *  it can represent a voice message waiting indicator
         *  as defined in the Common PCN Handset Specification
         *  v4.2. The specification allows other indicators
         *          to be defined in the future.
         */
        enum TTypeOfIndicator
        {
        EVoiceMessageWaitingIndicator = 0
        };

        private:
    	enum TCPHSIndicatorTypeByte
        {
        ECPSHIndicatorTypeBitMask             = 0x7E,
        ECPSHVoiceMailId                      = 0x10,
        ECPHSIndicatorSettingBit              = 0x01,
        ECPHSVoiceMailSettingSpareBit         = 0x80
        };

        enum TCPHSIndicatorIdByte
        {
        ECPSHIndicatorIdBitMask               = 0x7E,
        ECPSHIndicatorId                      = 0x00,
        ECPSHIndicatorIdSettingBit            = 0x01,
        ECTSHVoiceMailIndicatorSpareBit       = 0x80
        };

        enum TCPHSByteIndex
        {
        ECPHSLength                   = 0,
        ECPHSAddressType              = 1,
        ECPHSAddressIndicatorType     = 2,
        ECPHSAddressIndicatorId       = 3,
        ECPHSSizeOfAddressField       = 4
        };

	public:
		inline   TGsmSmsTelNumber();

		IMPORT_C TBool IsInstanceOf(TTypeOfIndicator aType);

	public:

		TGsmSmsTypeOfAddress iTypeOfAddress;		//< The type-of-address of iTelNumber
		TBuf<TGsmSmsTelNumberMaxLen> iTelNumber;	//< Telephone number, in format of iTypeOfAddress
	};


/**
 *  CSmsAddress - address of the recipient or SC
 *  @publishedAll
 *  @released
 */
class CSmsAddress : public CBase
	{
public:
	enum
		{
		KSmsAddressMaxAddressValueLength=10,// octets
		KSmsAddressMaxAddressLength = 12 // address length, type and address value
		};
public:
	static CSmsAddress* NewL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs);
	~CSmsAddress();
	TPtrC Address() const;
	void SetAddressL(const TDesC& aAddress);
	void ParsedAddress(TGsmSmsTelNumber& aParsedAddress) const;
	void SetParsedAddressL(const TGsmSmsTelNumber& aParsedAddress);

	void SetRawAddressL(TGsmSmsTypeOfAddress aTypeOfAddress, TPtrC aBuffer);
	TGsmSmsTypeOfAddress& TypeOfAddress();

	TUint8 SizeL();

	TUint8* EncodeL(TUint8* aPtr) const;
	void DecodeL(TGsmuLex8& aPdu);
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;

	CSmsAddress*  DuplicateL() const;

private:
	CSmsAddress(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs);
	void NewBufferL(TInt aLength);
	void DoSetParsedAddressL(const TDesC& aAddress);
private:

	CCnvCharacterSetConverter& iCharacterSetConverter;
	RFs& iFs;
	TGsmSmsTypeOfAddress iTypeOfAddress;
	HBufC* iBuffer;
	};


/**
 *  TP-SCTS Service Center Time Stamp Found in Deliver, Submit Report, Status Report,
 *  @publishedAll
 *  @released
 */
class TSmsServiceCenterTimeStamp
	{
public:
	enum {KSmsMaxTimeZoneNumQuarterHours=79};
/** Time zone offset sign bit. */
	enum TSmsTimeZoneSignBit
		{
	/** Positive offset. */
		ESmsTimeZonePositive=0x00,
	/** Negative offset. */
		ESmsTimeZoneNegative=0x08,

	/** Mask for these bit flags. */
		ESmsTimeZoneSignBitMask=0x08
		};
public:
	TSmsServiceCenterTimeStamp();
	inline TInt TimeOffset() const;
	void SetTimeOffset(TInt aNumQuarterHours);
	inline const TTime& Time() const;
	inline void SetTime(const TTime& aTime);
	TUint8* EncodeL(TUint8* aPtr) const;
	void DecodeL(TGsmuLex8& aPdu, TInt& aTimeError);
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
private:
	TTime iTime;
	TInt iTimeZoneNumQuarterHours;
	};


/**
 *  TP-VP Validity Period Found in SUBMIT PDUs.
 *  
 *  The validy period format is encoded in the first octet of the PDU, so the
 *  class takes a reference to a TSmsFirstOctet. The validty period specifies
 *  the length of time the PDU lives in the service center if the PDU cannot be
 *  immediately delivered.
 *  @publishedAll
 *  @released 
 */
class TSmsValidityPeriod
	{
public:
/** Validity period units (in minutes). */
	enum TValidityPeriodUnitInMinutes
		{
	/** Five minutes. */
		EFiveMinuteUnitInMinutes=5,
	/** 30 minutes. */
		EHalfHourUnitInMinutes=30,
	/** 1 day. */
		EOneDayUnitInMinutes=1440,
	/** 1 week. */
		EOneWeekUnitInMinutes=7*EOneDayUnitInMinutes
		};
/** Limits for validity period units (in minutes). */
	enum TValidityPeriodUnitLimitInMinutes
		{
	/** Limit for 5 minute validity period unit. */
		EFiveMinuteUnitLimitInMinutes=24*EHalfHourUnitInMinutes,
	/** Limit for 30 minute validity period unit. */
		EHalfHourUnitLimitInMinutes=EOneDayUnitInMinutes,
	/** Limit for 1 day validity period unit. */
		EOneDayUnitLimitInMinutes=30*EOneDayUnitInMinutes,
	/** Limit for 1 week validity period unit. */
		EOneWeekUnitLimitInMinutes=63*EOneWeekUnitInMinutes
		};
/** Limits for validity period units. */
	enum TValidityPeriodLimit
		{
	/** Limit for 5 minute validity period unit. */
		EFiveMinuteUnitLimit=143,
	/** Limit for 30 minute validity period unit. */
		EHalfHourUnitLimit=167,
	/** Limit for 1 day validity period unit. */
		EOneDayUnitLimit=196,
	/** Limit for 1 week validity period unit. */
		EOneWeekUnitLimit=255
		};
public:
	TSmsValidityPeriod(TSmsFirstOctet& aFirstOctet);
	inline TSmsFirstOctet::TSmsValidityPeriodFormat ValidityPeriodFormat() const;
	inline void SetValidityPeriodFormat(TSmsFirstOctet::TSmsValidityPeriodFormat aValidityPeriodFormat);
	inline const TTimeIntervalMinutes& TimeIntervalMinutes() const;
	inline void SetTimeIntervalMinutes(const TTimeIntervalMinutes& aTimeIntervalMinutes);
	TTime Time() const;
	TUint8* EncodeL(TUint8* aPtr) const;
	TUint8* EncodeL(TUint8* aPtr, const TEncodeParams* aEncodeParams) const;	
	void DecodeL(TGsmuLex8& aPdu);
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
private:
	TSmsFirstOctet& iFirstOctet;
	TTimeIntervalMinutes iTimeIntervalMinutes;
	};

class CEmsInformationElement;

/**
 *  SMS element defined in TP-UD octet.
 *  
 *  This element is found in Deliver, Deliver Report, Submit, Submit Report, Status
 *  Report and Command type messages.
 *  @publishedAll
 *  @released 
 */
class CSmsInformationElement : public CBase
	{
public:
/** TP-UD Information Element Identifier. */
	enum TSmsInformationElementIdentifier
		{
	/** Concatenated short messages, 8-bit reference number. */
		ESmsIEIConcatenatedShortMessages8BitReference=0x00,
	/** Special SMS Message Indication. */
		ESmsIEISpecialSMSMessageIndication=0x01,
	/** Reserved. */
		ESmsIEIReserved=0x02,
	/** Value not used to avoid misinterpretation as line feed character. */
		ESmsIEIValueNotUsed=0x03,
	/** Application port addressing scheme, 8 bit address. */
		ESmsIEIApplicationPortAddressing8Bit=0x04,
	/** Application port addressing scheme, 16 bit address */
		ESmsIEIApplicationPortAddressing16Bit=0x05,
	/** SMSC Control Parameters. */
		ESmsIEISMSCControlParameters=0x06,
	/** UDH Source Indicator. */
		ESmsIEIUDHSourceIndicator=0x07,
	/** Concatenated short message, 16-bit reference number. */
		ESmsIEIConcatenatedShortMessages16BitReference=0x08,
	/** Wireless Control Message Protocol. */
		ESmsIEIWirelessControlMessageProtocol=0x09,
		ESmsIEIRFC822EmailHeader=0x20,


//	Enhanced SMS IE
		ESmsEnhancedTextFormatting=0x0A,
		ESmsEnhancedPredefinedSound=0x0B,
		ESmsEnhancedUserDefinedSound=0x0C,
		ESmsEnhancedPredefinedAnimation=0x0D,
		ESmsEnhancedLargeAnimation=0x0E,
		ESmsEnhancedSmallAnimation=0x0F,
		ESmsEnhancedLargePicture=0x10,
		ESmsEnhancedSmallPicture=0x11,
		ESmsEnhancedVariablePicture=0x12,
		ESmsEnhancedUserPromptIndicator=0x13,
		ESmsEnhancedExtendedObject=0x14,
		ESmsEnhancedReusedExtendedObject=0x15,
		ESmsEnhancedCompressionControl=0x16,
		ESmsEnhancedODI=0x17,
		ESmsEnhancedStandardWVG=0x18,
		ESmsEnhancedCharacterSizeWVG=0x19,
		ESmsEnhancedextendedObjectDataRequest=0x1A,

//  Control Information Elements

		ESmsHyperLinkFormat = 0x21,
		ESmsReplyAddressFormat = 0x22,
		ESmsEnhanceVoiceMailInformation = 0x23,
		ESmsNationalLanguageSingleShift = 0x24,
		ESmsNationalLanguageLockingShift = 0x25,

//  Reserved values
//		....
//		....
	/** SIM Toolkit Security Header 1. */
		ESmsIEISIMToolkitSecurityHeaders1=0x70,
	/** SIM Toolkit Security Header 2. */
		ESmsIEISIMToolkitSecurityHeaders2=0x71,
	/** SIM Toolkit Security Header 3. */
		ESmsIEISIMToolkitSecurityHeaders3=0x72,
	/** SIM Toolkit Security Header 4. */
		ESmsIEISIMToolkitSecurityHeaders4=0x73,
	/** SIM Toolkit Security Header 5. */
		ESmsIEISIMToolkitSecurityHeaders5=0x74,
	/** SIM Toolkit Security Header 6. */
		ESmsIEISIMToolkitSecurityHeaders6=0x75,
	/** SIM Toolkit Security Header 7. */
		ESmsIEISIMToolkitSecurityHeaders7=0x76,
	/** SIM Toolkit Security Header 8. */
		ESmsIEISIMToolkitSecurityHeaders8=0x77,
	/** SIM Toolkit Security Header 9. */
		ESmsIEISIMToolkitSecurityHeaders9=0x78,
	/** SIM Toolkit Security Header 10. */
		ESmsIEISIMToolkitSecurityHeaders10=0x79,
	/** SIM Toolkit Security Header 11. */
		ESmsIEISIMToolkitSecurityHeaders11=0x7A,
	/** SIM Toolkit Security Header 12. */
		ESmsIEISIMToolkitSecurityHeaders12=0x7B,
	/** SIM Toolkit Security Header 13. */
		ESmsIEISIMToolkitSecurityHeaders13=0x7C,
	/** SIM Toolkit Security Header 14. */
		ESmsIEISIMToolkitSecurityHeaders14=0x7D,
	/** SIM Toolkit Security Header 15. */
		ESmsIEISIMToolkitSecurityHeaders15=0x7E,
	/** SIM Toolkit Security Header 16. */
		ESmsIEISIMToolkitSecurityHeaders16=0x7F,
	/** SME to SME specific use 1. */
		ESmsIEISMEToSMESpecificUse1=0x80,
	/** SME to SME specific use 2. */
		ESmsIEISMEToSMESpecificUse2=0x81,
	/** SME to SME specific use 3. */
		ESmsIEISMEToSMESpecificUse3=0x82,
	/** SME to SME specific use 4. */
		ESmsIEISMEToSMESpecificUse4=0x83,
	/** SME to SME specific use 5. */
		ESmsIEISMEToSMESpecificUse5=0x84,
	/** SME to SME specific use 6. */
		ESmsIEISMEToSMESpecificUse6=0x85,
	/** SME to SME specific use 7. */
		ESmsIEISMEToSMESpecificUse7=0x86,
	/** SME to SME specific use 8. */
		ESmsIEISMEToSMESpecificUse8=0x87,
	/** SME to SME specific use 9. */
		ESmsIEISMEToSMESpecificUse9=0x88,
	/** SME to SME specific use 10. */
		ESmsIEISMEToSMESpecificUse10=0x89,
	/** SME to SME specific use 11. */
		ESmsIEISMEToSMESpecificUse11=0x8A,
	/** SME to SME specific use 12. */
		ESmsIEISMEToSMESpecificUse12=0x8B,
	/** SME to SME specific use 13. */
		ESmsIEISMEToSMESpecificUse13=0x8C,
	/** SME to SME specific use 14. */
		ESmsIEISMEToSMESpecificUse14=0x8D,
	/** SME to SME specific use 15. */
		ESmsIEISMEToSMESpecificUse15=0x8E,
	/** SME to SME specific use 16. */
		ESmsIEISMEToSMESpecificUse16=0x8F,
	/** SME to SME specific use 17. */
		ESmsIEISMEToSMESpecificUse17=0x90,
	/** SME to SME specific use 18. */
		ESmsIEISMEToSMESpecificUse18=0x91,
	/** SME to SME specific use 19. */
		ESmsIEISMEToSMESpecificUse19=0x92,
	/** SME to SME specific use 20. */
		ESmsIEISMEToSMESpecificUse20=0x93,
	/** SME to SME specific use 21. */
		ESmsIEISMEToSMESpecificUse21=0x94,
	/** SME to SME specific use 22. */
		ESmsIEISMEToSMESpecificUse22=0x95,
	/** SME to SME specific use 23. */
		ESmsIEISMEToSMESpecificUse23=0x96,
	/** SME to SME specific use 24. */
		ESmsIEISMEToSMESpecificUse24=0x97,
	/** SME to SME specific use 25. */
		ESmsIEISMEToSMESpecificUse25=0x98,
	/** SME to SME specific use 26. */
		ESmsIEISMEToSMESpecificUse26=0x99,
	/** SME to SME specific use 27. */
		ESmsIEISMEToSMESpecificUse27=0x9A,
	/** SME to SME specific use 28. */
		ESmsIEISMEToSMESpecificUse28=0x9B,
	/** SME to SME specific use 29. */
		ESmsIEISMEToSMESpecificUse29=0x9C,
	/** SME to SME specific use 30. */
		ESmsIEISMEToSMESpecificUse30=0x9D,
	/** SME to SME specific use 31. */
		ESmsIEISMEToSMESpecificUse31=0x9E,
	/** SME to SME specific use 32. */
		ESmsIEISMEToSMESpecificUse32=0x9F,
//  Reserved values
//		....
//		....
	/** SC specific use 1. */
		ESmsIEISCSpecificUse1=0xC0,
	/** SC specific use 2. */
		ESmsIEISCSpecificUse2=0xC1,
	/** SC specific use 3. */
		ESmsIEISCSpecificUse3=0xC2,
	/** SC specific use 4. */
		ESmsIEISCSpecificUse4=0xC3,
	/** SC specific use 5. */
		ESmsIEISCSpecificUse5=0xC4,
	/** SC specific use 6. */
		ESmsIEISCSpecificUse6=0xC5,
	/** SC specific use 7. */
		ESmsIEISCSpecificUse7=0xC6,
	/** SC specific use 8. */
		ESmsIEISCSpecificUse8=0xC7,
	/** SC specific use 9. */
		ESmsIEISCSpecificUse9=0xC8,
	/** SC specific use 10. */
		ESmsIEISCSpecificUse10=0xC9,
	/** SC specific use 11. */
		ESmsIEISCSpecificUse11=0xCA,
	/** SC specific use 12. */
		ESmsIEISCSpecificUse12=0xCB,
	/** SC specific use 13. */
		ESmsIEISCSpecificUse13=0xCC,
	/** SC specific use 14. */
		ESmsIEISCSpecificUse14=0xCD,
	/** SC specific use 15. */
		ESmsIEISCSpecificUse15=0xCE,
	/** SC specific use 16. */
		ESmsIEISCSpecificUse16=0xCF,
	/** SC specific use 17. */
		ESmsIEISCSpecificUse17=0xD0,
	/** SC specific use 18. */
		ESmsIEISCSpecificUse18=0xD1,
	/** SC specific use 19. */
		ESmsIEISCSpecificUse19=0xD2,
	/** SC specific use 20. */
		ESmsIEISCSpecificUse20=0xD3,
	/** SC specific use 21. */
		ESmsIEISCSpecificUse21=0xD4,
	/** SC specific use 22. */
		ESmsIEISCSpecificUse22=0xD5,
	/** SC specific use 23. */
		ESmsIEISCSpecificUse23=0xD6,
	/** SC specific use 24. */
		ESmsIEISCSpecificUse24=0xD7,
	/** SC specific use 25. */
		ESmsIEISCSpecificUse25=0xD8,
	/** SC specific use 26. */
		ESmsIEISCSpecificUse26=0xD9,
	/** SC specific use 27. */
		ESmsIEISCSpecificUse27=0xDA,
	/** SC specific use 28. */
		ESmsIEISCSpecificUse28=0xDB,
	/** SC specific use 29. */
		ESmsIEISCSpecificUse29=0xDC,
	/** SC specific use 30. */
		ESmsIEISCSpecificUse30=0xDD,
	/** SC specific use 31. */
		ESmsIEISCSpecificUse31=0xDE,
	/** SC specific use 32. */
		ESmsIEISCSpecificUse32=0xDF,
	/**
	 *  @publishedAll
	 */
		ESmsIEMaximum = 0xFF
		};

public:
	// Exported functions
	IMPORT_C TSmsInformationElementIdentifier Identifier() const;
	IMPORT_C TPtr8 Data();
	IMPORT_C const TDesC8& Data() const;
public:
	static CSmsInformationElement* NewL(TSmsInformationElementIdentifier aIdentifier,const TDesC8& aData);
	static CSmsInformationElement* NewL();
	~CSmsInformationElement();

	TUint8* EncodeL(TUint8* aPtr) const;
	void DecodeL(TGsmuLex8& aPdu);
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	void ConstructL(const TDesC8& aData);
	TUint Length() const;
protected:
	void NewDataL(TInt aLength);
	inline CSmsInformationElement(TSmsInformationElementIdentifier aInformationElementIdentifier);
protected:
	TSmsOctet iIdentifier;
	HBufC8* iData;
	};


/**
 *  @publishedAll
 *  
 *  TSmsInformationElementCategories
 *  
 *  This class specifies where information elements are located SMS Messages.
 *  
 *  It specifies which PDUs each information element can be located in.
 *  
 *  The SMS stack uses this information when encoding and decoding SMS messages.
 */
class TSmsInformationElementCategories
    {
public:

    // Comment, table approach probably is not necessary here.
    // Simply use a switch statement as cannot index directly into table.

    enum TInformationElementCategory
    {
        EEmsInformationElement,
        ECtrlMandatoryInEveryPDUAndWithIdenticalValues,    // e.g. Port Addresses
        ECtrlMandatoryInEveryPDUMultipleInstancesPerPDU,   // e.g. Special SMS Message Indication
        ECtrlMandatoryInEveryPDUButWithValueSpecificToPDU, // e.g. Email Header / Concatenation Elements, SMSC Parameters
        ECtrlMandatoryIn1stPDUOnly,                        // e.g. Reply Address
        ECtrlSingleInstanceOnly,                           // e.g. Enhanced Voice Mail
        ECtrlMultipleInstancesAllowed,                     // e.g. Hyperlink format
        ENumberOfCategories
    };

typedef CSmsInformationElement::TSmsInformationElementIdentifier TInformationElementId;

private:
    enum TIndexToCategory
    {
        EIndexEmsInformationElement,
        EIndexCtrlMandatoryInEveryPDUAndWithIdenticalValues,
        EIndexCtrlMandatoryInEveryPDUMultipleInstancesPerPDU,
        EIndexCtrlMandatoryInEveryPDUButWithValueSpecificToPDU,
        EIndexCtrlMandatoryIn1stPDUOnly,
        EIndexCtrlSingleInstanceOnly,
        EIndexCtrlMultipleInstancesAllowed,
        EIndexCtrlOptionalInEveryPDUWithValueSpecificToPDU,
        ENumberOfIndices
    };

public:
    static TBool GetCategoryDefinition(TInformationElementId aId, TInformationElementCategory& aCategory);
private:
    static TBool TranslateCategoryToIndex(TInformationElementId aId, TInt& index);
    TSmsInformationElementCategories()  {};
    ~TSmsInformationElementCategories() {};
    void operator=(TSmsInformationElementCategories& aCategory) {(void) aCategory;  };
    TBool operator==(TSmsInformationElementCategories& aCategory){(void) aCategory; return EFalse; };
    static const TInformationElementCategory categories[ENumberOfIndices];
    };


/**
 *  @publishedAll
 */
typedef CSmsInformationElement::TSmsInformationElementIdentifier TSmsId;


/**
 *  Mobile originated SMS sent to the network requesting an action or information
 *  on the status of a previously sent SUBMIT.
 *  
 *  This is internal and not intended for use.
 *  @publishedAll
 *  @released 
 */
class CSmsCommandData : public CBase
	{
public:
	enum {KSmsMaxDataSize=157};
public:
	static CSmsCommandData* NewL(TSmsFirstOctet& aFirstOctet);
	~CSmsCommandData();

	inline TInt NumInformationElements() const;
	CSmsInformationElement& InformationElement(TInt aIndex) const;
	CSmsInformationElement*& InformationElementPtr(TInt aIndex);
	TBool InformationElementIndex(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier,
		TInt& aIndex) const;
	void AddInformationElementL(const TSmsId aIdentifier,const TDesC8& aData);
	void RemoveInformationElement(TInt aIndex);

	inline TInt MaxDataLength() const;
	TPtrC8 Data() const;
	void SetDataL(const TDesC8& aData);

	TUint8* EncodeL(TUint8* aPtr) const;
	void DecodeL(TGsmuLex8& aPdu);
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	
	CSmsCommandData* DuplicateL() const;

private:
	CSmsCommandData(TSmsFirstOctet& aFirstOctet);

	TInt HeaderLength() const;
	TInt TotalHeaderLengthInUDLUnits() const;

	TBool HeaderPresent() const;
	void SetHeaderPresent(TBool aHeaderPresent);

private:
	TSmsFirstOctet& iFirstOctet;
	CArrayPtrFlat <CSmsInformationElement> iInformationElementArray;
	HBufC8* iBuffer;
	};


/**
 *  Enumerations and operations for SMS commands.
 *  @publishedAll
 *  @released 
 */
class TSmsCommandType : public TSmsOctet
	{
public:
/** Command types. */
	enum TSmsCommandTypeValue
		{
	/** Enquiry. */
		ESmsCommandTypeEnquiry=0x00,
	/** Cancel. */
		ESmsCommandTypeCancel=0x01,
	/** Delete. */
		ESmsCommandTypeDelete=0x02,
	/** Enable Status Report Request. */
		ESmsCommandTypeEnableStatusReportRequest=0x03
//  Reserved values
//		....
//		....
		};
public:
	TSmsCommandType();
	inline TInt CommandType() const;
	inline void SetCommandType(TSmsCommandTypeValue aType);
	};


/**
 *  Operations on the User Data described in TP-UD.
 *  @publishedAll
 *  @released 
 */
class CSmsUserData : public CBase
	{
public:
	enum {KSmsMaxUserDataSize=140};
	enum {KSmsMaxUserDataLengthInChars=(KSmsMaxUserDataSize*8)/7};

public:
	static CSmsUserData* NewL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TSmsFirstOctet& aFirstOctet,const TSmsDataCodingScheme& aDataCodingScheme);
	~CSmsUserData();

	inline TInt NumInformationElements() const;
	IMPORT_C  CSmsInformationElement& InformationElement(TInt aIndex) const;
	IMPORT_C TBool InformationElementIndex(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier,TInt& aIndex) const;
	IMPORT_C TBool InformationElementLastIndex(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier,TInt& aIndex) const;
	IMPORT_C void AddInformationElementL(TSmsId aIdentifier,const TDesC8& aData);
	IMPORT_C void RemoveInformationElement(TInt aIndex);
	void InformationElementIndicesL(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier, CArrayFixFlat<TInt>& aIndices) const;

	// EMS related methods
	void AddEmsInformationElementL(CEmsInformationElement* aIe); // takes ownership od aIe;
	TBool EmsInformationElementWillFitL(CEmsInformationElement* aIe,CSmsEMSBufferSegmenter& aSeg,TUint& aCharsAddedToCurrentPDU);
	// Control Information Element related methods
	TBool ControlInformationElementWillFitL(CSmsInformationElement* aIe);
	void UpdateInformationElementArrayL(TSmsId aIdentifier,const TDesC8& aData);
	CSmsInformationElement*& InformationElementPtr(TInt aIndex);

	TInt MaxPackedUDUnitsInBodyRemaining() const;
	TInt MaxPackedUDUnitsInBodyRemaining(TUint totalHeaderLengthInUDLUnits) const;

	IMPORT_C TInt MaxBodyLengthInChars() const;
	IMPORT_C TPtrC8 Body() const;
	IMPORT_C void SetBodyL(const TDesC8& aBody);
	void AppendBodyL(const TDesC8& aExtraBody);

	IMPORT_C TBool IsSupportedL(TChar aChar);
	IMPORT_C TBool IsSupportedL(const TDesC& aDes, TInt& aNumberOfUnconvertibleCharacters,
			                    TInt& aIndexOfFirstUnconvertibleCharacter) const;

	IMPORT_C TBool IsSupportedL(const TDesC& aDes, TSmsEncoding aEncoding,
								TInt& aNumberOfUnconvertibleCharacters,
			                    TInt& aNumberOfDowngradedCharacters,
			                    TInt& aNumberRequiringAlternativeEncoding,
			                    TInt& aIndexOfFirstUnconvertibleCharacter) const;

	TBool IsBinaryData() const;

	TUint8* EncodeL(TUint8* aPtr) const;
	void DecodeL(TGsmuLex8& aPdu);
	void DecodeL(TGsmuLex8& aPdu, TBool aAcceptTruncation);	
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	
	CSmsUserData* DuplicateL(TSmsFirstOctet& aFirstOctet, const TSmsDataCodingScheme& aDataCodingScheme) const;

private:
	CSmsUserData(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TSmsFirstOctet& aFirstOctet,const TSmsDataCodingScheme& aDataCodingScheme);
	void ConstructL();
	TInt HeaderLength() const;
	TInt TotalHeaderLengthInUDLUnits() const;
	TInt BodyLengthInUDLUnits() const;
	void NewBodyL(TInt aLengthInChars);
	TBool HeaderPresent() const;
	void SetHeaderPresent(TBool aHeaderPresent);
	TInt TotalHeaderLengthInUDLUnits(TInt aIElen) const;

private:

	CCnvCharacterSetConverter& iCharacterSetConverter;
	RFs& iFs;

	TSmsFirstOctet& iFirstOctet;
	const TSmsDataCodingScheme& iDataCodingScheme;
	CArrayPtrFlat<CSmsInformationElement> iInformationElementArray;
	HBufC8* iBody;
	};

 enum TSmsMessageIndicationType
	{
	EGsmSmsVoiceMessageWaiting            =0x00,
	EGsmSmsFaxMessageWaiting              =0x01,
	EGsmSmsElectronicMailMessageWaiting   =0x02,
	EGsmSmsExtendedMessageTypeWaiting     =0x03,
	};

 enum TExtendedSmsIndicationType
	{
	EGsmSmsNoExtendedMessageTypeIndication=0x00,
	EGsmSmsVideoMessageWaiting            =0x01,
	EGsmSmsExtendedIndicationType2        =0x02,
	EGsmSmsExtendedIndicationType3        =0x03,
	EGsmSmsExtendedIndicationType4        =0x04,
	EGsmSmsExtendedIndicationType5        =0x05,
	EGsmSmsExtendedIndicationType6        =0x06,
	EGsmSmsExtendedIndicationType7        =0x07
	};

enum TSmsSpecialMessageIndicationTypeMask
    {
    /** Define a mask for the bits representing */
    /** the TSmsMessageIndicationType and the   */
    /** TExtendedSmsIndicationType              */
    EGsmSmsSpecialMessageIndicationTypeMask = 0x1F
    };

 enum TSmsMessageProfileType
    {
    EGsmSmsProfileId1 =0x00,
    EGsmSmsProfileId2 =0x01,
    EGsmSmsProfileId3 =0x02,
    EGsmSmsProfileId4 =0x03
    };

 enum TVoiceMailInfoType
    {
    EGsmSmsVoiceMailNotification       = 0x00,
    EGsmSmsVoiceMailDeleteConfirmation = 0x01
    };
    
 /** SMSC Control Parameters Selective Status Report For Each Segment. Found in Submit. */		
 enum TSmsSMSCControlParameters
	{
 /** Status Report for short message transaction completed. */
	ESmsStatusReportTransactionCompleted=0x01,
 /** Status Report for permanent error when Service Centre is not making any more transfer attempts. */	
	ESmsStatusReportPermanentError=0x02,
 /** Status Report for temporary error when Service Centre is not making any more transfer attempts. */
	ESmsStatusReportTemporaryError=0x04,
 /** Status Report for temporary error when Service Centre is still trying to transfer message segment. */
	ESmsStatusReportTemporaryErrorSCTrying=0x08,
 /** This is not Supported.
 	Reserved for future use. */
	ESmsStatusReportForFutureUse1=0x10,
 /** This is not Supported. 
 	Reserved for future use. */
	ESmsStatusReportForFutureUse2=0x20,
 /** This is not Supported. 
 	A Status Report generated by this Short Message, due to a permanent error or last temporary error, 
	 cancels the SRR of the rest of the Short Messages in a concatenated message. */
	ESmsStatusReportCancelRestSRR=0x40,
 /** This is not Supported. 
 	Include original UDH into the Status Report. */
	ESmsStatusReportIncludeOriginalUDH=0x80,
 /** Mask. The 4 least significant bits, which are supported, are set. */
 	ESmsSMSCControlParametersMask=0x0F	
 	};


/** Non Information Element Identifiers. */
 enum TSmsNonIEIdentifier
 	{
 	ESmsTPSRRParameter = 0x00,
	ESmsIncompleteClass0MessageParameter = 0x01
	};


/** Status Report Scheme*/
enum TSmsStatusReportScheme
	{
	EDefaultScheme = 0x00,
	ETPSRRScheme = 0x01,
	EControlParametersScheme = 0x10
	};


/**
 *  This is the base class for Enhanced Voice Mail Notifications and
 *  Enhanced Voice Mail Delete Confirmations. It encapsulates
 *  the attributes and encoding / decoding algorithms common to
 *  both types of Enhanced Voice Mail Information Element.
 *  
 *  @publishedAll
 *  @released
 */
class CEnhancedVoiceMailBoxInformation : public CBase
{
public:
   enum {KSmsMaxEnhancedVoiceMailSize = CSmsUserData::KSmsMaxUserDataSize - 3};
  // allow space in user data for the following bytes: user data header length, IE ID, IE Length
public:
    IMPORT_C TVoiceMailInfoType Type() const;
    IMPORT_C void   SetProfile(TSmsMessageProfileType aProfile);
    IMPORT_C TSmsMessageProfileType Profile() const;
    IMPORT_C void   SetStorage(TBool aIsStored);
    IMPORT_C TBool  Store() const;
    IMPORT_C void   SetAlmostMaximumCapacity(TBool aIsAlmostFull);
    IMPORT_C TBool  AlmostMaximumCapacity() const;
    IMPORT_C void   SetMaximumCapacity(TBool aIsFull);
    IMPORT_C TBool  MaximumCapacity() const;
    IMPORT_C TBool  ExtensionIndicator() const;
    IMPORT_C void   SetAccessAddressL(const TDesC& aParsedAddress);
    IMPORT_C void   SetParsedAccessAddressL(const TGsmSmsTelNumber& aParsedAddress);
    IMPORT_C TPtrC  AccessAddress() const;
    IMPORT_C void   ParsedAccessAddress(TGsmSmsTelNumber& aParsedAddress) const;
    IMPORT_C void   SetNumberOfVoiceMessages(TUint8 aNumber);
    IMPORT_C TUint8 NumberOfVoiceMessages() const;

    static   CEnhancedVoiceMailBoxInformation* NewL();
    virtual  ~CEnhancedVoiceMailBoxInformation();

    virtual TUint8* EncodeL(TUint8* aPtr, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs) const;
    virtual void DecodeL(TGsmuLex8& aVoiceMailInfo, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs);
protected:
    CEnhancedVoiceMailBoxInformation();
    CEnhancedVoiceMailBoxInformation(TVoiceMailInfoType aTVoiceMailInfoType);

    CEnhancedVoiceMailBoxInformation(const CEnhancedVoiceMailBoxInformation&);
    TBool operator==(const CEnhancedVoiceMailBoxInformation&);
    void operator=(const CEnhancedVoiceMailBoxInformation&);

    void ConstructL();
    void NewBufferL(TInt aLength);
    void DoSetParsedAddressL(const TDesC& aAddress);
protected:
    TVoiceMailInfoType iType;
    TBool iOctet1Bit1;
    TSmsMessageProfileType iProfile;
    TBool iStorage;
    TBool iAlmostFull;
    TBool iFull;
    TBool iExtensionIndicator;

    HBufC* iAccessAddress;
    TGsmSmsTypeOfAddress iTypeOfAddress;

    TUint8 iNumberOfVoiceMessages;
private:
enum TBitMasks
	{
	EMask1Bit  = 0x01,
	EMask2Bits = 0x03,
	EMask4Bits = 0x0F
	};
};


/**
 *  This class encapsulates the attributes of a VM Notification as described in 23.040 V6.5.0.
 *  It is used as one of the attributes a Enhanced Voice Mail Notification.
 *  
 *  @publishedAll
 *  @released
 */
class CVoiceMailNotification : public CBase
{
public:
    IMPORT_C void    SetMessageId(TUint16 aMessageId);
    IMPORT_C TUint16 MessageId() const;
    IMPORT_C void    SetMessageLength(TUint8 aLength);
    IMPORT_C TUint8  MessageLength() const;
    IMPORT_C void    SetRetentionDays(TUint8 aDays);
    IMPORT_C TUint8  RetentionDays() const;
    IMPORT_C void    SetPriorityIndication(TBool aPriority);
    IMPORT_C TBool   PriorityIndication() const;
    IMPORT_C TBool   MessageExtensionIndication() const;
    IMPORT_C void    SetCallingLineIdentityL(TDesC& aLineIdentity);
    IMPORT_C TPtrC   CallingLineIdentity() const;
    IMPORT_C void    SetParsedCallingLineIdentityL(TGsmSmsTelNumber& aParsedAddress);
    IMPORT_C void    ParsedCallingLineIdentity(TGsmSmsTelNumber& aParsedAddress) const;

    IMPORT_C static  CVoiceMailNotification* NewL();
    IMPORT_C virtual ~CVoiceMailNotification();

    TUint8  SizeL(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs);

    virtual TUint8* EncodeL(TUint8* aPtr, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs) const;
    virtual void    DecodeL(TGsmuLex8& aVoiceMailInfo, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs);
protected:
    CVoiceMailNotification();
    CVoiceMailNotification(const CVoiceMailNotification&);
    TBool    operator==(const CVoiceMailNotification&);
    void    operator=(const CVoiceMailNotification&);
    void    ConstructL();
    void    NewBufferL(TInt aLength);
    void    NewExtensionL(TInt aLength);

    void DoSetParsedAddressL(const TDesC& aAddress);
protected:
    TUint16 iMessageId;
    TUint8  iMessageLength;
    TUint8  iRetentionDays;
    TBool   iOctetN8Bit1;
    TBool   iPriorityIndication;
    TBool   iMessageExtensionIndicator;

    HBufC*  iCallingLineIdentity;
    TGsmSmsTypeOfAddress iTypeOfAddress;

    HBufC*  iExtension;
private:
 enum TBitMasks
	{
	EMask1Bit  = 0x01,
	EMask2Bits = 0x03,
	EMask4Bits = 0x0F,
	EMask5Bits = 0x1F
	};
};


/**
 *  This class encapsulates the attributes of an Enhanced Voice Mail Notification as described in
 *  9.2.3.24.13.1. An enhanced voice mail notification is added to a CSmsMessage using the
 *  interface provided by the class CSmsEnhancedVoiceMailOperations.
 *  
 *  Clients should be aware that 23.040 v6.5.0 specifies that this information element must fit into the
 *  user data field of a single PDU. The amount of space available in the user data field depends on both
 *  the number and size of the mandatory information elements that must also be present.
 *  Intuitively the largest Enhanced Voice Mail information element can be added when no mandatory information
 *  elements need to be encoded in the PDU. To achieve this, the CSmsMessage must be configured with
 *  only the Enhanced Voice Mail Information Element and no other information elements or text.
 *  
 *  @publishedAll
 *  @released
 */
class CEnhancedVoiceMailNotification : public CEnhancedVoiceMailBoxInformation
{
public:
    enum {KMaxNumberOfNotifications = 15};
protected:
    enum {KSmsNotificationBitMask = 0x0F};
    // bits mask for bit representing the number of voice mail notifications.
public:
    IMPORT_C TUint8   NumberOfVoiceMails();
    IMPORT_C RPointerArray<CVoiceMailNotification>& GetVoiceMailNotifications();

    IMPORT_C static  CEnhancedVoiceMailNotification* NewL();
    IMPORT_C ~CEnhancedVoiceMailNotification();

    TUint8* EncodeL(TUint8* aPtr, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs) const;
    void    DecodeL(TGsmuLex8& aVoiceMailInfo, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs);
protected:
    CEnhancedVoiceMailNotification();
    CEnhancedVoiceMailNotification(const CEnhancedVoiceMailNotification&);
    TBool operator==(const CEnhancedVoiceMailNotification&);
    void operator=(const CEnhancedVoiceMailNotification&);
    void NewExtensionL(TInt aLength);
    void ConstructL();
protected:
    TUint  iNumberOfVMNotifications;
    HBufC* iExtension;
    RPointerArray<CVoiceMailNotification>* iNotifications;
};


/**
 *  This class encapsulates the attributes of a VM Deletion  as described in 23.040 V6.5.0.
 *  9.2.3.24.13.1.
 *  
 *  It is used as one of the attributes of a Enhanced Voice Mail Delete Confirmation.
 *  
 *  @publishedAll
 *  @released
 */
class CVoiceMailDeletion : public CBase
{
public:
    IMPORT_C void    SetMessageId(TUint16 aMessageId);
    IMPORT_C TUint16 MessageId() const;
    IMPORT_C TBool   MessageExtensionIndication() const;

    IMPORT_C static  CVoiceMailDeletion* NewL();
    IMPORT_C virtual ~CVoiceMailDeletion();
    TUint8 SizeL();

	virtual TUint8* EncodeL(TUint8* aPtr) const;
    virtual void DecodeL(TGsmuLex8& aVoiceMailInfo);
protected:
    CVoiceMailDeletion();
    CVoiceMailDeletion(const CVoiceMailDeletion&);
    TBool operator==(const CVoiceMailDeletion&);
    void operator=(const CVoiceMailDeletion&);

    void ConstructL();
    void NewBufferL(TInt aLength);
protected:
    TUint   iMessageId;
    TBool   iExtensionIndicator;
    HBufC*  iExtension;
};


/**
 *  This class encapsulates the attributes of an Enhanced Voice Delete Confirmation as described in
 *  9.2.3.24.13.2. An enhanced voice delete confirmation is added to a CSmsMessage using the
 *  interface provided by the class CSmsEnhancedVoiceMailOperations.
 *  
 *  @publishedAll
 *  @released
 */
class CEnhancedVoiceMailDeleteConfirmations : public
      CEnhancedVoiceMailBoxInformation
{
public:
    enum {KMaxNumberOfNotifications = 31};
protected:
    enum {KSmsNotificationBitMask = 0x1F};
    // bits mask for bit representing the number of voice mail deletions.
public:
    IMPORT_C TUint8  NumberOfDeletes();
    IMPORT_C RPointerArray<CVoiceMailDeletion>& GetVoiceMailDeletions();

    IMPORT_C static CEnhancedVoiceMailDeleteConfirmations* NewL();
    IMPORT_C ~CEnhancedVoiceMailDeleteConfirmations();

	TUint8* EncodeL(TUint8* aPtr, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs) const;
    void    DecodeL(TGsmuLex8& aVoiceMailInfo, CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFs);
protected:
    CEnhancedVoiceMailDeleteConfirmations();
    CEnhancedVoiceMailDeleteConfirmations(const CEnhancedVoiceMailDeleteConfirmations&);
    TBool operator==(const CEnhancedVoiceMailDeleteConfirmations&);
    void operator=(const CEnhancedVoiceMailDeleteConfirmations&);

    void ConstructL();
    void NewExtensionL(TInt aLength);
protected:
    HBufC*  iExtension;
    RPointerArray<CVoiceMailDeletion>* iVoiceMailDeletions;
};


#include <gsmuelem.inl>

#endif // !defined __GSMUELEM_H__
