// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// AT Command TSY Standard Header
// 
//

/**
 @file
 @internalAll 
*/

#ifndef __ATSTD_H__
#define __ATSTD_H__

#include <etel.h>
#include <etelmm.h>
#include <etelqos.h>
#include "panic.h"		// for panic codes and function
/**
@internalComponent
*/
const TInt KLengthOfGuardTime=50;				// in 1/50ths of a second
const TInt KDefaultSecondsToWaitForCarrier=40;
const TInt KExtraWaitTime=4000;					// in milliseconds
const TInt KMaxNoChecks=3;
const TInt KTimeForExtraRxData=500;
const TInt KDefaultPauseInterval=5;
const TInt KTimeBetweenRings=7000000;			// in microseconds
const TInt KDelayForModemSettleAfterRing=500;	// in milliseconds
const TInt KMinTimeForSuccessfulInit=2000000;	// in microseconds
const TInt KMinTimeForSuccessfulPreConnectInit=1000000;	// in microseconds
const TInt KMaxLengthOfUserDefinedInitString=50;		// Chosen because this is the max length of a "short" CommDb text entry
const TInt KDefaultAutoAnswer=0;
const TInt KFaxCaps=(RPhone::KCapsFaxClassOne | RPhone::KCapsFaxClassTwo | RPhone::KCapsFaxClassTwoPointZero);
const TInt KOneSecondPause=1000;				// in milliseconds
const TInt KCallTimerInterval=1000000;			// in microseconds

/**
CMS Error Codes
@internalComponent
*/
const TInt KErrCmsMe=300;
const TInt KErrCmsUndefined=500;
/**
@internalComponent
*/
_LIT(KDataLineName,"Data");
_LIT(KFaxLineName,"Fax");
_LIT(KDataCallName,"DataCall");
_LIT(KFaxCallName,"FaxCall");
_LIT(KFaxModuleName,"FaxSvr.DLL");
_LIT(KModuleName,"HAYES");
_LIT(KsmsName,"Sms");
_LIT(KVoiceLineName,"Voice");
_LIT(KVoiceCallName,"VoiceCall");
/**
@internalComponent
*/
const TInt KFaxLineIndex=0;
const TInt KDataLineIndex=1;
const TInt KVoiceLineIndex=2;
const TInt KNumberOfLines=3;


/**
Command strings
@internalComponent
*/
_LIT8(KATCommand,"AT\r");
_LIT8(KAT2Command,"AT");
_LIT8(KATAndStringFormatString,"AT%S\r");
_LIT8(KReturnATCommand,"\rAT");
_LIT8(KHangUpCommand,"ATH");
_LIT8(KDialCommand,"ATD");
_LIT8(KDialDataCommandFormat,"ATD%S\r");
_LIT8(KDialVoiceCommandFormat,"ATD%S;\r");
_LIT8(KAnswerCommand,"ATA");
_LIT8(KDataClassCommand,"AT+FCLASS=0");
_LIT8(KGetPhoneCapsCommand,"AT+FCLASS=?");
_LIT8(KAutoAnswerCommand,"ATS0=");
_LIT8(KGetCarrierWaitTimeCommand,"ATS7?");


/**
Expected modem responses
@internalComponent
*/
_LIT8(KCarrierString, "CONNECT");
_LIT8(KNoCarrierString,"NO CARRIER");
_LIT8(KCallMonitoringEventString,"?ECAV: ?,0*"); // For Ericsson Phones that don't return NO CARRIER on Voice calls
_LIT8(KRingString,"RING");
_LIT8(KOkString,"OK");
_LIT8(KErrorString,"ERROR");
_LIT8(KDelayedString,"DELAYED");
_LIT8(KNotifyMeIfErrorString,"QWERTY"); // so that CATBase-derived thing will always be notified if an IO error occurs
_LIT8(KOkAndCarriageReturnString,"OK");


/**
Fax class strings
@internalComponent
*/
_LIT8(KZeroString,"0");
_LIT8(KOneString,"1");
_LIT8(KOnePointZeroString,"1.0");
_LIT8(KTwoString,"2");
_LIT8(KTwoPointZeroString,"2.0");
_LIT8(KTwoPointOneString,"2.1");


/**
Gprs strings
@internalComponent
*/
_LIT8(KMSClassA, "\"A\"");
_LIT8(KMSClassB, "\"B\"");
_LIT8(KMSClassCAlternateMode, "\"CA\"");
_LIT8(KMSClassCGPRSOnly, "\"CG\"");			
_LIT8(KMSClassCCircuitSwitchedOnly, "\"CC\"");


/**
General strings for TSY
@internalComponent
*/
_LIT8(KCarriageReturn,"\r");
_LIT8(KEscapeChar,"\x1b");
_LIT8(KEqualsSign,"=");
_LIT8(KStringAndIntegerFormatString,"%S%d\r");
_LIT8(KStringFormatString,"%S\r");
_LIT8(KStringAndStringFormatString,"%S%S\r");
_LIT8(KThreeStringFormatString,"%S%S%S\r");
_LIT8(KDblQuoteChar,"\"");
_LIT8(KPlusChar,"+");
_LIT8(KCommaChar,",");
_LIT8(KCtrlZChar,"\032");		



/**
@internalComponent
*/
enum TPortAccess
	{
	EPortAccessAllowed,
	EPortAccessDenied
	};
/**
@internalComponent
*/
enum TPhoneInitStatus
	{
	EPhoneNotInitialised,
	EPhoneInitialising,
	EPhoneInitialiseCancelling,
	EPhoneInitialised
	};
/**
@internalComponent
*/
typedef TBuf8<2> TStorageType;  
/**
@internalComponent
*/
struct TPhoneStatusTSY
	{
	RPhone::TModemDetection iModemDetected;
	RPhone::TMode iMode;
	TUint iDataAndFaxFlags;
	TPhoneInitStatus iInitStatus;
	RCall::TStatus iLineStatus;	// Used for all lines as physically there is only one line
	TPortAccess iPortAccess;
	TInt iWaitForCarrierTime;
	TBool iDataPortLoaned;
	TUint iSupportedPhoneBookStorageFlag;
	TInt iSupportedPhoneBookCount;
	TStorageType iLastAccessedPhoneBook;
	TBool iNetworkChanged;
	RMobilePhone::TMobilePhoneRegistrationStatus iRegistrationStatus;
	RMobilePhone::TMobilePhoneLocationAreaV1 iLocationArea;
	RMobilePhone::TMobilePhoneNetworkInfoV1 iCurrentNetwork;

	};

// Forward class declarations (as CPhoneGlobals holds pointers to these classes)
class CTsyConfig;
class CNotifications;
class CATWaitForMessageArrived;
class CATHandleCmtArrived;
class CATSmsMessagingRead;
class CATNetworkInfo;
class CReceiveSmsQueue;

class CPhoneGlobals : public CBase
/**
This class is used by nearly all CATCommands dervied classes
as a general storage place for TSY global values.

If this TSY was better designed, there should be no need for this class.
@internalComponent
*/
	{
public:
	static CPhoneGlobals* NewL(TBool aExplicit =EFalse);			// Factory function
	~CPhoneGlobals();						// destructor

public:
	// Utility functions
	static TBool IsWriteAccess(TStorageType aStorageType);
	inline void SetUnreadMsgPtr(CATSmsMessagingRead* aATSmsMessagingRead);
	void CheckForUnreadMessages();
	inline void SetChangeOfNetworkPtr(CATNetworkInfo* aATNetworkInfo);
	void CheckForChangeOfNetwork();
	TInt ConvertFromUnicode(TDes8& aMEString, const TDesC16& aUnicode) const;
	TInt ConvertToUnicode(TDes16& aUnicode, const TDesC8& aMEString) const;

public:
	// TSY Global data
	TPhoneStatusTSY iPhoneStatus;
	CNotifications* iNotificationStore;
	CTsyConfig* iConfiguration;
			    
	TStorageType iPhonePrefMem;  	
	RMobilePhone::TMobilePhoneIdentityV1 iPhoneId;

	enum TPhoneSmsPduStd
		{
		EPhoneTestUndefined,
		EPhoneTestNewStandard,
		EPhoneTestOldStandard
		};

	struct TMsgLocation
		{
		TInt  iIndex;
		TName iStoreName;
		};

	TPhoneSmsPduStd iPhoneTestState;
	CATHandleCmtArrived* iCmtArrived;

	TBool iSmsPduModeSupported;

	CATSmsMessagingRead* iATSmsMessagingRead;
	CATNetworkInfo* iATNetworkInfo;

	TUint32 iBatteryCaps;
	TUint32 iSignalCaps;
	TUint32 iPhoneIdCaps;
	TUint32 iSubscriberIdCaps;
	TUint32 iNetworkCaps;
	RMobilePhone::TMobilePhoneNetworkMode iNetworkMode;

	// CATBase flow control flag.
	// This flag is true when an AT command is writing to the serial port and thus
	// no other AT command should write to the serial port.
	// Generally setting and clearing of this flag is done in CATBAse::ExecuteCommand.
	// Some special case AT commands implement their own flow control.
	TBool iEventSignalActive;

	// We have a copy of a pointer to the CReceiveSmsQueue object (owned by CMobileSmsMessaging)
	// as when CATCommands::Complete is called we need to prod the CReceiveSmsQueue
	// object to let it know that it is safe for it to read any PDUs that it needs 
	// from the phones memory.
	CReceiveSmsQueue* iReceiveSmsQueuePtr;		// This class does not own this object
	// Gprs capabilities
	TInt iGprsMaxNumContexts;
	RPacketService::TMSClass iMSClass;
	RPacketService::TMSClass iMaxMSClass;
	RPacketService::TStatus	iGprsStatus;
	RPacketService::TRegistrationStatus	iGprsRegistrationStatus;
	RPacketQoS::TQoSCapsGPRS iStaticQoSCaps;

	//
	// Modem's SMS receiving capabiltiies.
	// These values set during the initialisation of the TSY
	TBool iModemSupportsCMTIMode;
	TBool iModemSupportsCMTMode;
	TBool iModemSupportsCSCAMode;

	//
	// Modem's GPRS capabiltiies.
	// These values set during the initialisation of the TSY
	TBool	iModemSupportsCGREGNotification;
	
	//
	// Data call capabilities (found out using +CBST=? in ATINIT)
	RMobileCall::TMobileCallDataCapsV1 iCallDataCaps;

private:
	void ConstructL(TBool aExplicit);	// 2nd stage constructor
	};
/**
@internalComponent
*/
enum TPanicOccurred
	{
	ENoPanicOccurred,
	EPanicOccurredWithoutDataPortLoan,
	EPanicOccurredDuringDataPortLoan
	};
/**
@internalComponent
*/
struct TCallInfoTSY
	{
	TName iCallName;
	TName iLineOwnerName;
	RMobileCall::TMobileCallStatus iMobileStatus;
	TTime iTimeCallBegan;
	RCall::TBearerService iBearerService;
	RCall::THookStatus iHookStatus;
	TPanicOccurred iClientPanicOccurred;
	RCall::TMonitorSpeakerControl iSpeakerControl;
	RCall::TMonitorSpeakerVolume iSpeakerVolume;
	TInt iInterval;
	RCall::TWaitForDialTone iWaitForDialTone;
	TBool iLoanedToClient;
	RMobileCall::TMobileHscsdCallParamsV1 iCallParams;   // just moved DJN
	};
 
inline void CPhoneGlobals::SetUnreadMsgPtr(CATSmsMessagingRead* aATSmsMessagingRead)
	{
	iATSmsMessagingRead = aATSmsMessagingRead;
	}

inline void CPhoneGlobals::SetChangeOfNetworkPtr(CATNetworkInfo* aATNetworkInfo)
	{
	iATNetworkInfo = aATNetworkInfo;
	}

#endif
