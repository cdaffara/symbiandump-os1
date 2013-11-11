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
//

/**
 @file
 @publishedPartner
 @released
*/

#if !defined(__ETEL_H__)
/** @internalComponent */
#define __ETEL_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined (__C32COMM_H__)
#include <c32comm.h>
#endif

#if !defined(__D32COMM_H__)
#include <d32comm.h>
#endif

#if !defined(__FAXDEFN_H)
#include <faxdefn.h>
#endif
/** Major version number.*/
const TUint KEtelMajorVersionNumber=1;
/** Minor version number. */
const TUint KEtelMinorVersionNumber=0;
/** Build number. */
const TUint KEtelBuildVersionNumber=606;
/**
@internalComponent
*/
IMPORT_C void PanicClient(TInt aFault);

//
// Etel Core Error Definitions
//

const TInt KErrEtelCoreBase = -2000;

/** The client is not the call owner and has attempted to perform an operation 
that requires ownership.*/
const TInt KErrEtelNotCallOwner=KErrEtelCoreBase;
/** An attempt has been made to load an ETel (TSY) module which contains a phone 
with the same name as one already loaded. */
const TInt KErrEtelDuplicatePhoneName=KErrEtelCoreBase-1;
/** An attempt has been made to acquire call ownership when the requesting client 
is already the call owner. */
const TInt KErrEtelAlreadyCallOwner=KErrEtelCoreBase-2;
/** A connection has been terminated because the carrier has been lost. */
const TInt KErrEtelNoCarrier=KErrEtelCoreBase-3;
/** An attempt to establish a connection with the RCall::Dial() function has failed 
because the remote party's line was busy. */
const TInt KErrEtelBusyDetected=KErrEtelCoreBase-4;
/** An attempt has been made to transfer call ownership, but no other clients are 
interested in acquiring ownership. */
const TInt KErrEtelNoClientInterestedInThisCall=KErrEtelCoreBase-5;
/** The TSY has failed to initialise the modem. This may be occur if the modem 
is disconnected during initialisation, or if an attempt is made to initialise 
an unsupported modem. */
const TInt KErrEtelInitialisationFailure=KErrEtelCoreBase-6;
/** An attempt has been made to perform an operation which requires the call to 
be connected - when the call is not connected. E.g. RCall:LoanDataPort(). */
const TInt KErrEtelCallNotActive=KErrEtelCoreBase-7;
/** A connection request has failed because there is no answer. */
const TInt KErrEtelNoAnswer=KErrEtelCoreBase-8;
/** A connection request has failed because there is no dial tone. For example, 
if there is no connection between the modem and the network. */
const TInt KErrEtelNoDialTone=KErrEtelCoreBase-9;
/** An attempt to configure the port has failed because the hardware cannot match 
the desired configuration. For example, if an attempt is made to configure 
the comms port to a baud rate it cannot support. */
const TInt KErrEtelConfigPortFailure=KErrEtelCoreBase-10;
/** Out of memory error - global chunk cannot be created. */
const TInt KErrEtelFaxChunkNotCreated=KErrEtelCoreBase-11;
/** The client is not the fax owner and has attempted to perform an operation that 
requires fax ownership.

The fax client should be used to access fax functionality. Consequently, client 
code should not return this error. */
const TInt KErrEtelNotFaxOwner=KErrEtelCoreBase-12;
/** An attempt has been made to perform an operation which requires port ownership. 
However, the client does not own the port. */
const TInt KErrEtelPortNotLoanedToClient=KErrEtelCoreBase-13;
/** An operation has failed because the modem has been incorrectly configured. */
const TInt KErrEtelWrongModemType=KErrEtelCoreBase-14;
/** An operation has failed because the modem does not understand the request - 
possible bug in modem. */
const TInt KErrEtelUnknownModemCapability=KErrEtelCoreBase-15;
/** The call has already placed an 'answer incoming call' request. Only 1 request 
of a type (e.g. fax, voice, data) can be outstanding on the server at a time. */
const TInt KErrEtelAnswerAlreadyOutstanding=KErrEtelCoreBase-16;
/** An attempt to establish a connection with the modem has failed because the 
modem cannot be detected. */
const TInt KErrEtelModemNotDetected=KErrEtelCoreBase-17;
/** Operation failed because the modem settings are corrupt. */
const TInt KErrEtelModemSettingsCorrupt=KErrEtelCoreBase-18;
/** The Data port is in use by another call or application. */
const TInt KErrEtelPortAlreadyLoaned=KErrEtelCoreBase-19;
/** Another call active. Close it before opening a new one. */
const TInt KErrEtelCallAlreadyActive=KErrEtelCoreBase-20;

//
//
// RTelSubSessionBase
//
//
class CPtrHolder;
class RFile;
/**
A base class used in the derivation of RCall, RLine, and RPhone. It has no user
accessible functions. 

@publishedPartner
@released
*/
class RTelSubSessionBase : public RSubSessionBase
	{
public:
	inline RSessionBase& SessionHandle() const;
	inline void SetSessionHandle(RSessionBase& aSession);
	inline void ResetSessionHandle();
	inline TInt SubSessionHandle();

	IMPORT_C void CancelAsyncRequest(TInt aReqToCancel) const;

	enum TReqPriorityType
		{
		EIsNotaPriorityRequest,
		EIsaPriorityRequest,
		EIsaEmergencyRequest
		};
	
protected:

	IMPORT_C RTelSubSessionBase();
	IMPORT_C TInt Blank(const TInt aIpc,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void Blank(const TInt aIpc,TRequestStatus& aStatus,TReqPriorityType aType = EIsNotaPriorityRequest) const;

	IMPORT_C TInt Set(const TInt aIpc,const TDesC8& aDes,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void Set(const TInt aIpc,TRequestStatus& aStatus,const TDesC8& aDes,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C TInt Get(const TInt aIpc,TDes8& aDes,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void Get(const TInt aIpc,TRequestStatus& aStatus,TDes8& aDes,TReqPriorityType aType = EIsNotaPriorityRequest) const;

	IMPORT_C TInt Set(const TInt aIpc,const TDesC8& aDes1,const TDesC8& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void Set(const TInt aIpc,TRequestStatus& aStatus,const TDesC8& aDes1,const TDesC8& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C TInt Get(const TInt aIpc,TDes8& aDes1,TDes8& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void Get(const TInt aIpc,TRequestStatus& aStatus,TDes8& aDes1,TDes8& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	
	IMPORT_C TInt Set(const TInt aIpc,const TDesC16& aDes,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void Set(const TInt aIpc,TRequestStatus& aStatus,const TDesC16& aDes,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C TInt Get(const TInt aIpc,TDes16& aDes,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void Get(const TInt aIpc,TRequestStatus& aStatus,TDes16& aDes,TReqPriorityType aType = EIsNotaPriorityRequest) const;

	IMPORT_C TInt Set(const TInt aIpc,const TDesC16& aDes1,const TDesC16& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void Set(const TInt aIpc,TRequestStatus& aStatus,const TDesC16& aDes1,const TDesC16& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C TInt Set(const TInt aIpc,const TDesC8& aDes1,const TDesC16& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void Set(const TInt aIpc,TRequestStatus& aStatus,const TDesC8& aDes1,const TDesC16& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;

	IMPORT_C TInt Get(const TInt aIpc,TDes16& aDes1,TDes16& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void Get(const TInt aIpc,TRequestStatus& aStatus,TDes16& aDes1,TDes16& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C TInt Get(const TInt aIpc,TDes8& aDes1,TDes16& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void Get(const TInt aIpc,TRequestStatus& aStatus,TDes8& aDes1,TDes16& aDes2,TReqPriorityType aType = EIsNotaPriorityRequest) const;

	IMPORT_C void SetAndGet(const TInt aIpc, TRequestStatus& aStatus, const TDesC8& aDes1, TDes8& aDes2, TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void SetAndGet(const TInt aIpc, TRequestStatus& aStatus, TDes8& aDes1, const TDesC16& aDes2, TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void SetAndGet(const TInt aIpc, TRequestStatus& aStatus, const TDesC8&, TDes16& aDes2, TReqPriorityType aType = EIsNotaPriorityRequest) const;
	IMPORT_C void SetAndGet(const TInt aIpc, TRequestStatus& aStatus, const TDesC16& aDes1, TDes16& aDes2, TReqPriorityType aType = EIsNotaPriorityRequest) const;

	IMPORT_C TInt Set(const TInt aIpc, const RFile& aFile, TReqPriorityType aType = EIsNotaPriorityRequest) const;

	IMPORT_C TInt CancelReq(const TInt aIpc,const TInt aIpcToCancel) const;
	IMPORT_C TInt CancelSubSession() const;

private:
	RTelSubSessionBase(const RTelSubSessionBase& aTelSubSessionBase);
	RSessionBase* iTelSession;
protected:	
	CPtrHolder* iPtrHolder;
	};

//
//
// RFax
//
//
class RCall;
/**
ETel fax subsession.

Contains the fax client progress information structure.

ETel does include server-side RFax objects, however the 
associated APIs are intended solely for use by ETel's own 
fax client. Consequently, only the TProgress structure is 
considered part of the public API.

@publishedPartner
@released
*/
class RFax : public RTelSubSessionBase
	{
public:
	IMPORT_C RFax();
	IMPORT_C TInt Open(RCall& aCall);
	IMPORT_C void Close();

//
// Read,Write and Wait for Page End
//
	IMPORT_C void Read(TRequestStatus& aStatus,TDes8& aDes);
	IMPORT_C void Write(TRequestStatus& aStatus,const TDesC8& aDes);
	IMPORT_C void WaitForEndOfPage(TRequestStatus& aStatus) const;
	IMPORT_C TInt TerminateFaxSession() const;
//
// Progress Information
//
	struct TProgress
	/** Contains fax progress information.*/
		{
		/** Time of last progress update.*/
		TTime iLastUpdateTime;
		/** ID of remote fax - filled in after connection.*/
		TBuf<20> iAnswerback;

		/** Phase of session.*/
		TFaxPhase iPhase;
		/** Resolution of document.*/
		TFaxResolution iResolution;
		/** Compression method in use.*/
		TFaxCompression iCompression;
		/** Error correction mode. Reserved for future releases.*/
		TInt iECM;
		
		/** Last or current page being transferred.*/
		TInt iPage;
		/** Last or current line on page.*/
		TInt iLines;
		/** Speed of fax session.*/
		TInt iSpeed;
		};

	IMPORT_C TInt GetProgress(TProgress& aProgress);
private:
	TProgress* iProgressPtr;
	RChunk iChunk;
protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual void Destruct();
	};

//
//
// RCall
//
//
class RTelServer;
class RPhone;
class RLine;
/**
Call sub-session.

Provides access to the functionality associated with a specific call.

This class is not intended for user derivation. 

@publishedPartner
@released
*/
class RCall : public RTelSubSessionBase
	{
public:
	IMPORT_C RCall();
	IMPORT_C TInt OpenNewCall(RTelServer& aServer,const TDesC& aName,TDes& aNewName);
	IMPORT_C TInt OpenNewCall(RTelServer& aServer,const TDesC& aName);
	IMPORT_C TInt OpenNewCall(RPhone& aPhone,const TDesC& aName,TDes& aNewName);
	IMPORT_C TInt OpenNewCall(RPhone& aPhone,const TDesC& aName);
	IMPORT_C TInt OpenNewCall(RLine& aLine,TDes& aNewName);
	IMPORT_C TInt OpenNewCall(RLine& aLine);
	IMPORT_C TInt OpenExistingCall(RTelServer& aServer,const TDesC& aName);
	IMPORT_C TInt OpenExistingCall(RPhone& aPhone,const TDesC& aName);
	IMPORT_C TInt OpenExistingCall(RLine& aLine,const TDesC& aName);
	IMPORT_C void Close();

//
// Call Status
//
/** Call status flags. */
	enum TStatus
		{
	/** The call status is not known. */
		EStatusUnknown,
	/** The call is idle. */
		EStatusIdle,
	/** The call is dialling. */
		EStatusDialling,
	/** The call is ringing (an incoming, unanswered call). */
		EStatusRinging,
	/** The call is being answered. */
		EStatusAnswering,
	/** The call is connecting (immediate call establishment, without dialling). */
		EStatusConnecting,
	/** The call is connected and active. */
		EStatusConnected,
	/** The call is being terminated. */
		EStatusHangingUp
		};

	IMPORT_C TInt GetStatus(TStatus& aStatus) const;

//
// Call Caps
//
/** Call capability flags. */
	enum TCapsEnum
		{
	/** The call supports data calls. */
		KCapsData=0x00000001,
	/** The call supports fax calls. */
		KCapsFax=0x00000002,
	/** The call supports voice calls. */
		KCapsVoice=0x00000004,
	/** The Dial request is currently a valid action. */
		KCapsDial=0x00000008,
	/** The Connect request is currently a valid action. */
		KCapsConnect=0x00000010,
	/** The Hang Up request is currently a valid action. */
		KCapsHangUp=0x00000020,
	/** The Answer request is currently a valid action. */
		KCapsAnswer=0x00000040,
	/** The Loan Data Port request is currently a valid action. */
		KCapsLoanDataPort=0x00000080,
	/** The Recover Data Port request is currently a valid action. */
		KCapsRecoverDataPort=0x00000100
		};

	struct TCaps
/** Call capability information. */
		{
	/** Call capability flag. Contains a bitmask of values defined in the TCapsEnum 
	enumeration.*/
		TUint iFlags;
		};

	IMPORT_C TInt GetCaps(TCaps& aCaps) const;
	IMPORT_C void NotifyCapsChange(TRequestStatus& aStatus, TCaps& aCaps);
	IMPORT_C void NotifyCapsChangeCancel() const;

//
// Bearer Service Information
//
/** Bearer capability flags. */
	enum TBearerCaps
		{
	/** V42bis modem compression protocol. */
		KBearerCapsCompressionV42bis=0x00000001,
	/** MNP5 modem compression protocol. */
		KBearerCapsCompressionMNP5=0x00000002,
	/** No compression supported. */
		KBearerCapsCompressionNone=0x00000004,
	/** LAPM error correction protocol. */
		KBearerCapsProtocolLAPM=0x00000008,
	/** MNP10 mode. */
		KBearerCapsProtocolALT_CELLULAR=0x00000010,
	/** MNP error correction. */
		KBearerCapsProtocolALT=0x00000020,
	/** No protocol information provided. */
		KBearerCapsProtocolNone=0x00000040,
	/** Compression protocol unknown. */
		KBearerCapsCompressionUnknown=0x00000080,
	/** Unable to establish protocol. */
		KBearerCapsProtocolUnknown=0x00000100
		};

/** Bearer speed flags. */
	enum TCallBearerSpeed
		{
	/** Transfer rate of 57600 bps. */
		EBearerData57600,
	/** Transfer rate of 33600 bps. */
		EBearerData33600,
	/** Transfer rate of 31200 bps. */
		EBearerData31200,
	/** Transfer rate of 9200 bps. */
		EBearerData19200,
	/** Transfer rate of 4400 bps. */
		EBearerData14400,
	/** Transfer rate of 12000 bps. */
		EBearerData12000,
	/** Transfer rate of 9600 bps. */
		EBearerData9600,
	/** Transfer rate of 7200 bps. */
		EBearerData7200,
	/** Transfer rate of 4800 bps. */
		EBearerData4800,
	/** Transfer rate of 2400 bps. */
		EBearerData2400,
	/** Transfer rate of 200 bps. */
		EBearerData1200,
	/** V.23 75 bps forward, 1200 bps back */
		EBearerData75_1200,
	/** V.23 1200 bps forward, 75 bps back */
		EBearerData1200_75,
	/** Transfer rate of 300 bps. */
		EBearerData300,
	/** Transfer rate is unknown. */
		EBearerDataUnknown
		};

	struct TBearerService
/** Call bearer information.

@see TCallBearerSpeed */
		{
	/** The bearer capabilities. */
		TUint32 iBearerCaps;
	/** The bearer's speed.
	
	@see TCallBearerSpeed */
		TCallBearerSpeed iBearerSpeed;
		};

 	IMPORT_C TInt GetBearerServiceInfo(TBearerService& aBearerService) const;

//
// Notifications
//
/** Hook status flags. */
	enum THookStatus
		{
	/** The line is currently off hook. */
		EHookStatusOff,
	/** The line is currently on hook. */
		EHookStatusOn,
	/** The status of the line's hook is currently unknown */
		EHookStatusUnknown
		};

	class TCallInfo
/** Line call capability information.

@see TName 
@publishedPartner
@released
*/
		{
	public:
		IMPORT_C TCallInfo();
	/** The name of a call.
	
	@see TName */
		TName iCallName;
		TName iLineName;
		THookStatus iHookStatus;
	/** The call status.
	
	@see RCall::TStatus */
		TStatus iStatus;
		TTimeIntervalSeconds iDuration;
		};
	
	IMPORT_C TInt GetInfo(TCallInfo& aCallInfo) const;
	IMPORT_C void NotifyHookChange(TRequestStatus& aStatus,THookStatus& aHookStatus);
	IMPORT_C void NotifyHookChangeCancel() const;
	IMPORT_C void NotifyStatusChange(TRequestStatus& aStatus,TStatus& aCallStatus);
	IMPORT_C void NotifyStatusChangeCancel() const;
	IMPORT_C void NotifyCallDurationChange(TRequestStatus& aStatus,TTimeIntervalSeconds& aTime);
	IMPORT_C void NotifyCallDurationChangeCancel() const;

//
// Duration of call
//
	IMPORT_C TInt GetCallDuration(TTimeIntervalSeconds& aTime) const;
//
// Connecting and Disconnecting
//
/** Passes telephone numbers into Dial functions. */
	typedef TDesC TTelNumberC;
	IMPORT_C TInt Dial(const TTelNumberC& aTelNumber) const;
	IMPORT_C TInt Dial(const TDesC8& aCallParams,const TTelNumberC& aTelNumber) const;
	IMPORT_C void Dial(TRequestStatus& aStatus,const TTelNumberC& aTelNumber);
	IMPORT_C void Dial(TRequestStatus& aStatus,const TDesC8& aCallParams,const TTelNumberC& aTelNumber);
	IMPORT_C void DialCancel() const;
	IMPORT_C TInt Connect() const;
	IMPORT_C TInt Connect(const TDesC8& aCallParams) const;
	IMPORT_C void Connect(TRequestStatus& aStatus);
	IMPORT_C void Connect(TRequestStatus& aStatus,const TDesC8& aCallParams);
	IMPORT_C void ConnectCancel() const;
	IMPORT_C TInt AnswerIncomingCall() const;
	IMPORT_C TInt AnswerIncomingCall(const TDesC8& aCallParams) const;
	IMPORT_C void AnswerIncomingCall(TRequestStatus& aStatus);
	IMPORT_C void AnswerIncomingCall(TRequestStatus& aStatus,const TDesC8& aCallParams);
	IMPORT_C void AnswerIncomingCallCancel() const;
	IMPORT_C TInt HangUp() const;
	IMPORT_C void HangUp(TRequestStatus& aStatus) const;
	IMPORT_C void HangUpCancel() const;

//
// Call Data Access
//
	struct  TCommPort
/** Communications port information.

@see TFileName */
		{
	/** The CSY module used by the C32 Communications Server to talk to the serial 
	device.
	@deprecated This has been deprecated as all the information that is needed by the 
	lower layers is now being passed in iPort. 
	
	@see TFileName */
		TFileName iCsy;
	/** The port used by the C32 Communications Server to talk to the serial device.
	It holds both the Csy name and the Port name.
	
	@see TName */
		TName iPort;
		};

	IMPORT_C TInt LoanDataPort(TCommPort& aDataPort) const;
	IMPORT_C void LoanDataPort(TRequestStatus& aStatus,TCommPort& aDataPort);
	IMPORT_C void LoanDataPortCancel() const;
	IMPORT_C TInt RecoverDataPort() const;

//
// Call Ownership manipulation
//
/** Call ownership status flags. */
	enum TOwnershipStatus
		{
	/** The call is not owned. */
		EOwnershipUnowned,
	/** The call is owned, but not by this RCall instance. */
		EOwnershipOwnedByAnotherClient,		// not the priority client
	/** The call is owned by this RCall instance. */
		EOwnershipOwnedByThisClient,
	/** The call is owned, but not by the priority client */
		EOwnershipThisIsPriorityClient		// not owned by this (the priority) client
		};

	IMPORT_C TInt TransferOwnership() const;
	IMPORT_C void AcquireOwnership(TRequestStatus& aStatus) const;
	IMPORT_C void AcquireOwnershipCancel() const;
	IMPORT_C TInt GetOwnershipStatus(TOwnershipStatus& aOwnershipStatus) const;
	//
//
// Get and Set Call Parameters
//
/** Monitor speaker control setting flags. */
	enum TMonitorSpeakerControl
		{
	/** Speaker control is always off */
		EMonitorSpeakerControlAlwaysOff,
	/** Speaker control is on until the carrier is detected */
		EMonitorSpeakerControlOnUntilCarrier,
	/** Speaker control is always active. */
		EMonitorSpeakerControlAlwaysOn,
	/** Speaker control is on, except while dialling */
		EMonitorSpeakerControlOnExceptDuringDialling,
	/** Speaker control is unknown. */
		EMonitorSpeakerControlUnknown
		};

/** Monitor speaker volume control setting flags. */
	enum TMonitorSpeakerVolume
		{
	/** Speaker volume is off. */
		EMonitorSpeakerVolumeOff,
	/** Speaker volume is low. */
		EMonitorSpeakerVolumeLow,
	/** Speaker volume is medium. */
		EMonitorSpeakerVolumeMedium,
	/** Speaker volume is high. */
		EMonitorSpeakerVolumeHigh,
	/** Speaker volume is unknown. */
		EMonitorSpeakerVolumeUnknown
		};

/** Wait for dial tone flags. */
	enum TWaitForDialTone
		{
	/** Waiting for dial tone. */
		EDialToneWait,
	/** Not waiting for dial tone. */
		EDialToneNoWait
		};

	class TCallParams
	/**
	@publishedPartner
	@released
	*/
		{
	public:
		IMPORT_C TCallParams();
		IMPORT_C TCallParams(TMonitorSpeakerControl aSpeakerControl,
							 TMonitorSpeakerVolume aSpeakerVolume,
							 TUint aInterval,
							 TWaitForDialTone aWaitForDialTone);
		IMPORT_C TInt ExtensionId();
		IMPORT_C TBool IsDefault();
		TMonitorSpeakerControl iSpeakerControl;
		TMonitorSpeakerVolume iSpeakerVolume;
		TUint iInterval;
		TWaitForDialTone iWaitForDialTone;
	protected:
		TBool iIsDefault;
		TInt iExtensionId;
		};

/** Call parameters package

The call parameters to be associated with a call may be passed into functions 
using this type.

@see TCallParams */
	typedef TPckg<TCallParams> TCallParamsPckg;

	IMPORT_C TInt GetCallParams(TDes8& aParams) const;

//
// Fax Settings
//
/** Fax mode flags. */
	enum TFaxMode
		{
	/** Fax is transmitting */
		ETransmit,
	/** Fax is receiving. */
		EReceive
		};

/** Fax retrieval type flags. */
	enum TFaxRetrieveType
		{
	/** Retrieving a fax opened from voice call (not currently supported by GSM networks). */
		EFaxOnDemand,
	/** Retrieve a fax opened from fax call. */
		EFaxPoll
		};

/** Fax identifier size flags. */
	enum TConstants
		{
	/** 20 */
		KFaxIdUserNameMaxSize=20
		};

	struct TFaxSessionSettings
/** Fax session settings information.

@see TFaxMode */
		{
	/** The fax mode.
	
	@see TFaxMode */
		TFaxMode iMode;
	/** The mode of retrieving faxes.
	
	@see TFaxRetrieveType */
		TFaxRetrieveType iFaxRetrieveType;
	/** The class of fax 
	
	@see TFaxClass */
		TFaxClass iFaxClass;
	/** The fax identifier string displayed at the top of the fax.
	
	@see KFaxIdUserNameMaxSize */
		TBuf<KFaxIdUserNameMaxSize> iFaxId;
	/** The maximum transfer speed that can be negotiated. */
		TInt iMaxSpeed;
	/** The minimum transfer speed that should be negotiated. */
		TInt iMinSpeed;
	/** The preferred compression mode used for negotiation. This may take one of the 
	following values: EModifiedHuffman, EModifiedRead, EUncompressedModifiedRead, 
	EModifiedModifiedRead. */
		TInt iPreferredECM;
	/** The delay used for Fax on Demand services. Measured in seconds. */
		TInt iFaxOnDemandDelay;
		// Tx Settings
	/** The preferred transmit resolution. This may take one of the following values: 
	EFaxNormal EFaxFine
	
	@see TFaxResolution */
		TFaxResolution iTxResolution;
	/** Compression mode for transmitting faxes.
	
	@see TFaxCompression */
		TFaxCompression iTxCompression;
	/** The number of pages to be transmitted. */
		TInt iTxPages;
		// Rx Settings
	/** The preferred receive resolution. This may take one of the following values: 
	EFaxNormal EFaxFine.
	
	@see TFaxResolution */
		TFaxResolution iRxResolution;
	/** Compression mode for receiving faxes.
	
	@see TFaxCompression */
		TFaxCompression iRxCompression;
		};

	IMPORT_C TInt GetFaxSettings(TFaxSessionSettings& aSettings) const;
	IMPORT_C TInt SetFaxSettings(const TFaxSessionSettings& aSettings) const;
	IMPORT_C TInt AdoptFaxSharedHeaderFile(const RFile& aSharedFile) const;
//
// Retrieval of call objects' reference count
//
 	IMPORT_C TInt ReferenceCount() const;

protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual void Destruct();
	};

//
//
// RLine
//
//
class RPhone;
/**
Provides access to the functionality associated with a specific line.

This class is not intended for user derivation. 

@publishedPartner
@released
*/
class RLine : public RTelSubSessionBase
	{
public:
	IMPORT_C RLine();
	IMPORT_C TInt Open(RPhone& aPhone,const TDesC& aName);
	IMPORT_C TInt Open(RTelServer& aServer,const TDesC& aName);
	IMPORT_C void Close();

//
// Caps
//
/** Line capability flags. */
	enum TCapsFlags
		{
	/** The line supports data connections.*/
		KCapsData=0x00000001,
	/** The line supports fax connections. */
		KCapsFax=0x00000002,
	/** The line supports voice connections.*/
		KCapsVoice=0x00000004,
	/** The line supports incoming call notification requests. */
		KCapsEventIncomingCall=0x00010000,
		};

	struct TCaps
/** Line capability information. */
		{
	/** Line capability flag. Contains a bitmask of the values defined in the TCapsFlags 
	enumeration.*/
		TUint iFlags;
		};

	IMPORT_C TInt GetCaps(TCaps& aCaps) const;
	IMPORT_C void NotifyCapsChange(TRequestStatus& aStatus, TCaps& aCaps);
	IMPORT_C void NotifyCapsChangeCancel() const;

//
// Line and Hook Status
//
	IMPORT_C TInt GetStatus(RCall::TStatus& aStatus) const;
	IMPORT_C TInt GetHookStatus(RCall::THookStatus& aHookStatus) const;

//
// Call enumeration and Information
//
	struct TCallInfo
/** Line call capability information.

@see TName */
		{
	/** The name of a call.
	
	@see TName */
		TName iCallName;
	/** The call status.
	
	@see RCall::TStatus */
		RCall::TStatus iStatus;
	/** The call capabilities flag. 
	
	@see RCall::TCapsEnum*/
		TUint32 iCallCapsFlags;
		};

	IMPORT_C TInt EnumerateCall(TInt& aCount) const;
	IMPORT_C TInt GetCallInfo(TInt aIndex,TCallInfo& aCallInfo) const;
//
//	General line notification information
//
	class TLineInfo
/** Line information.

@see RCall::TStatus 
@publishedPartner
@released
*/
		{
	public:
		IMPORT_C TLineInfo();
		RCall::THookStatus iHookStatus;
	/** The current line status.
	
	@see RCall::TStatus */
		RCall::TStatus iStatus;
		TName iNameOfLastCallAdded;
		TName iNameOfCallForAnswering;	// if an incoming call occurs, this contains the name
										// of the call to answer it on.
		};
	
//
// Notifications
//
	IMPORT_C TInt GetInfo(TLineInfo& aLineInfo) const;
	IMPORT_C void NotifyIncomingCall(TRequestStatus& aStatus, TName& aName);
	IMPORT_C void NotifyIncomingCallCancel() const;
	IMPORT_C void NotifyHookChange(TRequestStatus& aStatus,RCall::THookStatus& aHookStatus);
	IMPORT_C void NotifyHookChangeCancel() const;
	IMPORT_C void NotifyStatusChange(TRequestStatus& aStatus,RCall::TStatus& aLineStatus);
	IMPORT_C void NotifyStatusChangeCancel() const;
	IMPORT_C void NotifyCallAdded(TRequestStatus& aStatus,TName& aName);
	IMPORT_C void NotifyCallAddedCancel() const;
protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual void Destruct();
	};

//
//
// RPhone
//
//
class RTelServer;
/**
Phone sub-session.

Provides access to the functionality associated with a specific phone.

This class is not intended for user derivation.

@publishedPartner
@released
*/
class RPhone : public RTelSubSessionBase
	{
public:
	IMPORT_C RPhone();
	IMPORT_C TInt Open(RTelServer& aSession,const TDesC& aName);
	IMPORT_C void Close();

//
// Initialisation
//
	IMPORT_C TInt Initialise();
	IMPORT_C void Initialise(TRequestStatus& aStatus);
	IMPORT_C void InitialiseCancel();

//
// Caps
//
/** Phone capability flags. */
	enum TCapsFlags
		{
	/** The capabilities of the phone are not known. */
		KCapsUnknown=0x00000001,
	/** The phone supports data calls. */
		KCapsData=0x00000002,
	/** The phone supports the fax class 1 interface. */
		KCapsFaxClassOne=0x0000004,
	/** The phone supports the fax class 1.0 interface. */
		KCapsFaxClassOnePointZero=0x0000008,
	/** The phone supports the fax class 2 interface. */
		KCapsFaxClassTwo=0x00000010,
	/** The phone supports the fax class 2.0 interface. */
		KCapsFaxClassTwoPointZero=0x00000020,
	/** The phone supports the fax class 2.1 interface. */
		KCapsFaxClassTwoPointOne=0x00000040,
	/** The phone supports voice calls. */
		KCapsVoice=0x00000080,
	/** The phone supports modem detection events, i.e. it can notify clients when 
	the modem can be or can no longer be detected. */
		KCapsEventModemDetection=0x00010000,
	/** The phone supports the ability to take the communications port from whatever 
	component is currently using it. */
		KCapsStealCommPort=0x00020000
		};

	struct TCaps
/** Phone capability information. */
		{
	/** Phone capability flag. Contains bitmask of values defined in the TCapsFlags 
	enumeration. */
		TUint iFlags;
		};

	IMPORT_C TInt GetCaps(TCaps& aCaps) const;
	IMPORT_C void NotifyCapsChange(TRequestStatus& aStatus, TCaps& aCaps);
	IMPORT_C void NotifyCapsChangeCancel() const;

//
// Status
//
/** Phone mode status flags. */
	enum TMode
		{
	/** Phone (modem) status unknown. */
		EModeUnknown,
	/** Phone is idle. */
		EModeIdle,
	/** Phone is establishing link. */
		EModeEstablishingLink,
	/** Modem is in data mode. */
		EModeOnlineData,
	/** Modem is in command mode and the line is active. */
		EModeOnlineCommand
		};

/** Modem connection status flags. */
	enum TModemDetection
		{
	/** Modem present. */
		EDetectedPresent,
	/** Modem is not present. */
		EDetectedNotPresent,
	/** Modem connection status is unknown. */
		EDetectedUnknown
		};

	struct TStatus
/** Current phone status.

@see TModemDetection */
		{
	/** The current modem connection status. 
	
	@see TModemDetection */
		TModemDetection iModemDetected;
	/** The current mode of the phone.
	
	@see TMode */
		TMode iMode;
		};

	IMPORT_C TInt GetStatus(TStatus& aStatus) const;

//
// Line Enumeration and Information
//
	struct TLineInfo
/** Line information.

@see RCall::TStatus */
		{
	/** The current line status.
	
	@see RCall::TStatus */
		RCall::TStatus iStatus;
	/** The line capabilities flag. 
	
	@see RLine::TCapsFlags */
		TUint32 iLineCapsFlags;
	/** The name of the line.
	
	@see TName */
		TName iName;
		};

	IMPORT_C TInt EnumerateLines(TInt& aCount) const;
	IMPORT_C TInt GetLineInfo(const TInt aIndex,TLineInfo& aLineInfo) const;

//
//	General phone notification information
//
	class TPhoneInfo
/** Phone notification information.

Collects all the information associated with an instance of the core API phone 
class is collected together into a single, directly accessible, structure. 
This increases the efficiency of applications that require access to the majority 
of the phone's data.

This class is not intended for user derivation.

@see TNetworkType 
@publishedPartner
@released
*/
		{
	public:
		IMPORT_C TPhoneInfo();
	/** The current modem detection state (is it present or not).
	
	@see TModemDetection */
		TModemDetection iDetection;
		};

	IMPORT_C TInt GetInfo(TPhoneInfo& aPhoneInfo) const;

//
// Notifications
//
	IMPORT_C void NotifyModemDetected(TRequestStatus& aStatus,TModemDetection& aDetection);
	IMPORT_C void NotifyModemDetectedCancel() const;

//	
// TEmergencyRequest is a bit-mask where each bit-flag represents 
// the emergency request type that the client wishes to handle
//
	enum TEmergencyRequest
		{
	/** Location Based Services Emergency Request */ 
		EEmergencyLCSRequest = 0x00000001,
	/** Circuit Switch Voice Call Emergency Request */
		EEmergencyCSVoiceCallRequest = 0x00000002,
		};
	
	IMPORT_C TInt SetEmergencyClient(const TUint32 aEmergencyRequest) const;

protected:
	IMPORT_C virtual void ConstructL();
	IMPORT_C virtual void Destruct();
	};

//
//
// RTelServer
//
//
/**
Root telephony server session.

Provides access to general telephony information and functionality. Clients 
wishing to use the ETel Server must open a connection with the root server 
using the functions defined in this class. They may then obtain information 
about loaded TSY modules, and the phones that they support. To access functionality 
associated with a specific phone, the client must open a sub-session for that 
phone.

This class is not intended for user derivation.

@publishedPartner
@released 
*/
class RTelServer : public RSessionBase
	{
public:
	enum { KDefaultMessageSlots=32 };

	IMPORT_C RTelServer();
	IMPORT_C TInt Connect(TInt aMessageSlots =KDefaultMessageSlots);
	inline TVersion Version() const;

//
// Enumeration and Information
//
/** Types of telephony connection flags. */
	enum TNetworkType
		{
	/** Wired analog network. */
		ENetworkTypeWiredAnalog,
	/** Wired digital network (not supported by ETel). */
		ENetworkTypeWiredDigital,
	/** Mobile analog network (not supported by ETel). */
		ENetworkTypeMobileAnalog,
	/** Mobile digital network. */
		ENetworkTypeMobileDigital,
	/** Unknown network type. */
		ENetworkTypeUnknown
		};

	struct  TPhoneInfo
/** Phone notification information.

Collects all the information associated with an instance of the core API phone 
class is collected together into a single, directly accessible, structure. 
This increases the efficiency of applications that require access to the majority 
of the phone's data.

This class is not intended for user derivation. 

@see TNetworkType */
		{
	/** The type of telephony connection supported by the phone.
	
	@see TNetworkType */
		TNetworkType iNetworkType;
	/** The name of the phone.
	
	@see TName */
		TName iName;
	/** The number of lines supported by the phone. */
		TUint iNumberOfLines;
	/** The extensions that the TSY module supports. */
		TUint iExtensions;
		};

	IMPORT_C TInt EnumeratePhones(TInt& aNoOfPhones) const;
	IMPORT_C TInt GetPhoneInfo(const TInt aIndex,TPhoneInfo& aInfo) const;
	IMPORT_C TInt GetTsyName(const TInt aIndexOfPhone, TDes& aTsyName) const;

//
// Load and Unload Phone modules
//

	IMPORT_C TInt LoadPhoneModule(const TDesC& aFileName) const;
	IMPORT_C TInt UnloadPhoneModule(const TDesC& aFileName) const;

//
// Check whether specified Phone module (TSY) supports certain functionality
//
		
	IMPORT_C TInt IsSupportedByModule(const TDesC& aTsyName, const TInt aMixin, TBool& aResult) const;
	IMPORT_C TInt GetTsyVersionNumber(const TDesC& aTsyName,TVersion& aVersion) const;
//
// Set this session as Priority client
//
	/** @deprecated This function has been deprecated. Use RTelServer::SetPriorityClientV2 followed by 
	RPhone::SetEmergencyClient(EEmergencyCSVoiceCallRequest) */
	IMPORT_C TInt SetPriorityClient() const;

//
// Set this session to receive detailed errors
//

/** Error granularity flags. */
	enum TErrorGranularity
		{
	/** Client is only interested in core errors */
		EErrorBasic,
	/** Client is interested in extended errors */
		EErrorExtended
		};

	IMPORT_C TInt SetExtendedErrorGranularity(const TErrorGranularity aGranularity) const;

//
// Debug calls - stubbed in release builds
//
	IMPORT_C TInt __DbgMarkHeap();
	IMPORT_C TInt __DbgCheckHeap(TInt aCount);
	IMPORT_C TInt __DbgMarkEnd(TInt aCount);
	IMPORT_C TInt __DbgFailNext(TInt aCount);
	IMPORT_C TInt __DbgFailNextAvailable(TBool& aResult);
	
//
// Set this session as Priority client
//
	IMPORT_C TInt SetPriorityClientV2() const;
	
	};


#include <etel.inl>
#endif
