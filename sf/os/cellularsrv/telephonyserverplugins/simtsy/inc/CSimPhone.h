// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the definitions of the Simulator TSY phone class.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __SIMPHONE_H__
#define __SIMPHONE_H__

#include <f32file.h>
#include <et_phone.h>
#include "CSimLine.h"
#include "CSimDataLine.h"
#include "CSimVoiceLine.h"
#include "csimtimer.h"
#include "SimConstants.h"
#include "CSimPhoneSecurity.h"
#include "CSimPhoneUSimApp.h"
#include "CSimReduceTimers.h"
#include "CSimPhoneSmartCardApp.h"
#include "CSimPhoneInitialise.h"
#include "CSimPhoneIMSAuth.h"
#include "CSimUsimR6.h"

#include <etelmm.h>
#include <etelmmerr.h>
#include <etelpckt.h>
#include <etelsat.h>
#include <pcktcs.h>
#include "SimTsy.h"

// Macro to create an extended error code.
#define EXTENDEDERROR(aNormal,aExtended)	((aExtended << 16) | (aNormal & 0x0000ffff))

// Timer Values
const TInt KSetCallProcessingSuspendStateTimeout=1;

// Panics
enum TSimPanic
	{
	ELineNotPreInstantiated,
	ESubsessionNotPreInstantiated,
	ECallStatusUnknownIllegal,
	ETimeOutEventActionFailed,
	EIllegalConfigFileIdentifier,
	ENotificationAlreadyPending,
	EIllegalStateInconsistancy,
	EIllegalAnswerNextIncomingCallInconsistancy,
	EIllegalNoCallObjectAssignedForNextIncomingCall,
	EPhoneModeUnknownIllegal,
	ENetworkModeUnknownIllegal,
	EIllegalHexCharacterInConfigFilePdu,
	EIllegalOddNumberOfCharactersInConfigFilePdu,
	EIllegalSmsRxEvent,
	EIllegalSmsTxEvent,
	EPhonebookNameOrNumberTooLarge,
	EOwnNumberNameOrNumberTooLarge,
	EIllegalCallNotRegisteredWithLine,
	EIlegalValueForSmsExpectedErrorInConfigFile,
	ENotificationReqAlreadyOutstanding,
	EIllegalCallBackId,
	ECommWriterNotConstructed,
	ECommReaderNotConstructed,
	EVoiceCallIllegalStatus,
	ECallEventIllegal,
	EIllegalCancelRequest,
	EIllegalDtmfEvent,
	EIllegalDtmfReq,
	EIllegalVoiceCallEvent,
	EPacketMSClassUnKnown,
	EOjectNotConstructed,
	EConstraintsOverlapping,
	EMobileSmsMessagingPhoneNotSetToAReceiveMode,
	EFailedToDefineProperty,
	EFailedToSetProperty,
	ETlsAlreadySet,
	ETlsNotSet,
	ECdmaModeRequired, 
	EGeneral,
	EUnableToDefineProperties,
	EInvalidParameterFormatInConfigFile,
	EIllegalAnswerRemoteHangupCallInconsistency,
	EIllegalNullPtrParameter
	};

GLREF_C void SimPanic(TSimPanic aPanicNumber, TInt aLineNumber = -1);

const TUint KDefaultNumberOfSlots=5;	// < Default number of slots that the ETel Server should allocate for buffered requests.

/**
* This class 'CSimPhone' is the Phone class for Simulator TSY.  
* The CSimPhone class main responsibility is to handle all the functionality associated 
* with the RPhone class in the Core API and the RMobilePhone class in the Multimode API.
* It will create and own most of the resources the phone will require during its session. 
* These include creating both voice and data lines, CConfigReader objects, CSimSysAgent objects,
* CSimSmsMessaging object, CSimPhoneStore object, CSimPhoneBook object, and CSimPacket object.
*
*/
class CSimCall;
class CSimVoiceLine;
class CSimSmsMessaging;
class CSimPhBkStore;
class CSimPhBkUSimStore;
class CSimONStore;
class CSimSignalStrength;
class CSimNetworkStatus;
class CSimIndicator;
class CSimBatteryCharger;
class CSimDtmf;
class CTestConfig;
class CTestConfigSection;
class CSimPacketService;
class CSimSat;
class CSimTimer;
class CSimCallBarring;
class CSimCallForwarding;
class CSimCallWaiting;
class CSimSmartCardEapManager;
class CSimSmartCardAuth;
class CSimPhone : public CPhoneBase
	{
public:
	static CSimPhone* NewL(CPhoneBase*& aPhoneBaseRef);
	CSimPhone(CPhoneBase*& aPhoneBaseRef);
	~CSimPhone();
//
	static void CloseObj(TAny* aObj);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIpc);

// MTelObjectTSY pure virtual
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual void Init();
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);

// MPhoneBaseTSY pure virtuals
	virtual TInt GetInfo(const TTsyReqHandle aTsyReqHandle, RPhone::TPhoneInfo* aPhoneNotifyInfo);
	virtual TInt NotifyCapsChange(const TTsyReqHandle aTsyReqHandle, RPhone::TCaps* aCaps);
	virtual TInt NotifyCapsChangeCancel(const TTsyReqHandle aTsyReqHandle);
	virtual TInt ControlledInitialisation(const TTsyReqHandle aTsyReqHandle);
	virtual TInt ControlledInitialisationCancel(const TTsyReqHandle aTsyReqHandle);
    virtual TInt NotifyModemDetected(const TTsyReqHandle aTsyReqHandle, RPhone::TModemDetection* aDetection);
    virtual TInt NotifyModemDetectedCancel(const TTsyReqHandle aTsyReqHandle);
    virtual TInt GetCaps(const TTsyReqHandle aTsyReqHandle,RPhone::TCaps* aCaps);
	virtual TInt GetStatus(const TTsyReqHandle aTsyReqHandle,RPhone::TStatus* aStatus);
	virtual TInt EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt*);
	virtual TInt GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex* aLineInfo);
	virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);
	virtual TInt NumberOfSlotsL(const TInt aIpc);


// MSubSessionExtBaseTSY pure virtual
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);

	const CTestConfigSection* CfgFile();		// < Accessor function for the configuration file.
	const CTestConfigSection* DefaultCfgFile();		// < Accessor function for the default configuration file.

	CSimReduceTimers* GetReduceTimersSubject();

	RMobilePhone::TAID GetActiveUSim();
	TBool IsICCLocked();
	TBool IsPIN2Locked();
	TBool IsHiddenEnabled();
	RMobilePhone::TMobilePhoneSubscriberId GetImsi();

	void SecurityEvent(RMobilePhone::TMobilePhoneSecurityEvent aEvent);

	TInt FindActiveVoiceCall(CSimVoiceCall*& aCall);

	TInt ValidateChangeState(CSimLine* aOriginatingLine, RMobileCall::TMobileCallStatus aState);
	TInt ValidateChangeState(RPacketService::TStatus aState);

	TInt GetMultimodeCaps(const TTsyReqHandle aTsyReqHandle,TUint32* aCaps);
	TInt GetIccAccessCaps(const TTsyReqHandle aReqHandle, TUint32* aCaps);
	TInt GetCurrentMode(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	void UpdateNetworkMode(RMobilePhone::TMobilePhoneNetworkMode aNetworkMode);
	
	TInt GetTestNumber(TInt& aTestNumber);	
	TInt SetTestNumberInUse(TInt aTestNumber);	
	inline RPhone::TMode Mode() const {return iMode; }

	RMobilePhone::TMobilePhoneNetworkMode NetworkMode();
	void SetTestNumberAndReadConfigurationFileL();

	// Methods added to allow indicator object to retrieve information
	// from other objects owned by the phone
	RMobilePhone::TMobilePhoneBatteryStatus BatteryStatus();
	RMobilePhone::TMobilePhoneRegistrationStatus RegistrationStatus();
	void CheckIndicatorNotification();

	// Simulated TSY version checking
	TInt iSimTsyVersion;
	// data  structures deriving from TMultimodeType (the majority of cases)
	TInt CheckSimTsyVersion(RMobilePhone::TMultimodeType& aDataStruct);
	// data  structures deriving from TPacketBase 
	TInt CheckSimTsyVersion(TPacketBase& aDataStruct);
	// data  structures deriving from TCallParams 
	TInt CheckSimTsyVersion(RCall::TCallParams& aDataStruct);
	// data  structures deriving from CBase (usually one-off cases)
	TInt CheckSimTsyVersion(RMobilePhone::CImsAuthorizationInfoV5& aDataStruct);
	TInt CheckSimTsyVersion(RPacketContext::CTFTMediaAuthorizationV3& aDataStruct);
	
	// Callback function invoked by the observer object
	void HandleTestNumberChangedL();
	TInt CheckConfigFile();
	
protected:
	void ConstructL();

private:

	void FindAndCreatePhBkStoresL();
	void CreateONStoreL();
	void InitPhoneStatus();
	TInt GetPhoneStoreInfo(TTsyReqHandle aReqHandle,TDes8* aPckg1,TDes16* aPckg2);
	TInt GetPhoneStoreInfo(TTsyReqHandle aReqHandle,TDes8* aPckg1,
								  RMobilePhone::TPhoneStoreNameAndIccType* aTable);
	TInt GetSubscriberInfo(TTsyReqHandle aReqHandle,TDes8* aPckg1);
	void GetPhoneIdAndCaps();
	TInt GetIdentityCaps(TTsyReqHandle aReqHandle, TUint32& aCaps);
	TInt GetPhoneId(TTsyReqHandle aReqHandle,RMobilePhone::TMobilePhoneIdentityV1* aPhoneId);

	TInt SetCallProcessingSuspendState(const TTsyReqHandle aReqHandle, const TBool* aSuspend);
	TInt SetCallProcessingSuspendStateCancel(const TTsyReqHandle aTsyReqHandle);
	
	// Service Table IPC support functionality
	TInt EnumerateAPNEntries(TTsyReqHandle aReqHandle, TUint32* aIndex);
	TInt GetAPNname(TTsyReqHandle aReqHandle, TUint32& aIndex, RMobilePhone::TAPNEntryV3Pckg& aRetrieved);
	TInt AppendAPNName(TTsyReqHandle aReqHandle, RMobilePhone::TAPNEntryV3Pckg& aEntry);
	TInt DeleteAPNName(TTsyReqHandle aReqHandle, TUint32& aIndex);
	TInt NotifyAPNListChanged(TTsyReqHandle aReqHandle);
	TInt SetAPNControlListServiceStatus(TTsyReqHandle aReqHandle, RMobilePhone::TAPNControlListServiceStatus& aAPNControlListServiceStatus);
	TInt GetAPNControlListServiceStatus(TTsyReqHandle aReqHandle, RMobilePhone::TAPNControlListServiceStatus& aAPNControlListServiceStatus);
	TInt NotifyAPNControlListServiceStatusChange(TTsyReqHandle aReqHandle, RMobilePhone::TAPNControlListServiceStatus& aAPNControlListServiceStatus);
	TInt GetServiceTable(TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneServiceTable aTable, TDes8* aSst);	
	// Service Table utility functions
	void PopulateServiceTableL(TInt aMaxSupported, const TDesC8& aLookup, RMobilePhone::TMobilePhoneServiceTableV1** aTable);						
	void PopulateServiceTableV8L(const TDesC8& aLookup,RMobilePhone::TMobilePhoneServiceTableV8** aTableV8 );						
	TBool ServiceEnabled(RMobilePhone::TMobilePhoneServiceTableV1* aTable, TInt aOffset, TUint8 aModifier);
	TInt CopyServiceTable(RMobilePhone::TMobilePhoneServiceTableV1* aFrom, RMobilePhone::TMobilePhoneServiceTableV1* aTo);
	TInt CopyServiceTableV8(RMobilePhone::TMobilePhoneServiceTableV8* aFrom, RMobilePhone::TMobilePhoneServiceTableV8* aTo);

	void ReadConfigurationFileL();
	TInt NotifyModeChange(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TMobilePhoneNetworkMode* aCaps);
	TInt NotifyModeChangeCancel(const TTsyReqHandle aTsyReqHandle);

	void TimerCallBackNetworkMode();
	TNotifyData iNotifyNetworkModeChange;
	TInt iNetworkModeIndex;

	struct TNetworkModeBundle
		{
		TInt iDuration;										//< Duration of dynamic caps value
		RMobilePhone::TMobilePhoneNetworkMode iNetworkMode;	//< Dynamic Caps value that will be maintained through period iDuration
		};
	CArrayFixFlat<TNetworkModeBundle>* iNetworkModeArray; //< List of Network Mode changes

	CPhoneBase*&								iPhoneBaseRef;
	TNotifyData									iAPNListNotify;
	TNotifyData									iAPNStatusNotify;

	RArray<RMobilePhone::TAPNEntryV3>			iAPNList;			//< Pointer to the object that holds the Access Point Name List
	const TInt 									iAPNServiceOffset;

// 	from the TMobilePhoneServiceTable enum
	RMobilePhone::TMobilePhoneServiceTableV1*	iUSIMServiceTable;  		//< USIM service table from active USIM application on UICC.
	RMobilePhone::TMobilePhoneServiceTableV8*	iUSIMServiceTableV8;  		//< USIM service table from active USIM application on UICC.
	RMobilePhone::TMobilePhoneServiceTableV1*	iUSIMEnabledServiceTable;  	//< USIM Enabled Services Table to be used in conjunction with the USIM Service 
	RMobilePhone::TMobilePhoneServiceTableV1*	iSIMServiceTable;  			//< SIM service table on SIM (or from GSM application on UICC).
	
	RFs											iFs;				//< Connection to file server for config file access
	CSimSmsMessaging* 							iSmsMessaging;		//< pointer to the SmsMessaging object
	CSimLine*									iActiveLine;		//< Pointer to the currently active line

	CSimVoiceLine*								iVoiceLine;			//< Pointer to the voice line
	CSimDataLine*								iDataLine;			//< Pointer to the Data line

	CSimSignalStrength*							iSignalStrength;	//< Pointer to the object that handles Signal Strength
	CSimNetworkStatus*							iNetworkStatus;		//< Pointer to the object that handles Network Information and Status
	CSimDtmf*									iDtmf;				//< Pointer to the object that handles DTMF transmission
	CArrayFixFlat<CSimPhBkStore*>*				iPhBkStores;
	CArrayFixFlat<CSimPhBkUSimStore*>*			iPhBkUSimStores;

	CSimONStore*								iONStore;
	
	CTestConfig*             					iConfigFile;        //< Pointer to the Configuration file reader
	CTestConfigSection*                         iConfigSection;     //< Pointer to the Configuration section for current test. 
	TBuf8<KMaxName>          					iSectionName;
	CSimSat*                 					iSat;               //< Pointer to the Sat object
	CSimPacketService*       					iPacketService;     //< Pointer to the Packet object
	CSimIndicator*           					iIndicator;         //< Pointer to the Indicator object
	CSimBatteryCharger*	     					iBatteryCharger;    //< Pointer to the Battery Charger object
	CSimPhoneSecurity*	     					iPhoneSecurity;     //< Pointer to the Phone Security object
	CSimPhoneUSimApp*	     					iPhoneUSimApp;      //< Pointer to the Phone USimApp object
	CSimPhoneSmartCardApp*   					iPhoneSmartCardApp; //< Pointer to the Phone SmartCardApp object
	CSimPhoneIMSAuth*        					iPhoneIMSAuth;      //< Pointer to the IMS Authorization/Authentication object
	CSimSmartCardAuth*       					iPhoneScAuth;       //< Pointer to the object used for SmartCardAuthenticate requests
	CSimSmartCardEapManager* 					iPhoneScEapMan;     //< Pointer to the Smart Card EAP sub-session object

	CSimReduceTimers*							iReduceTimers;      //< Pointer to the object used for reducing all simtsy timers; see KPSSimTsyTimersReduceTime
	CSimUsimR6* 								iUsimRelease6;   	//< Pointer to the object that handles USIM Release6 features
	
	RPhone::TModemDetection						iModemDetection;	//< Modem detection
	RPhone::TMode								iMode;				//< Phone mode
	RMobilePhone::TMobilePhoneNetworkMode		iNtwkMode;			//< current network mode

	struct TSimTsySubsriberId
	{
	RMobilePhone::TMobilePhoneSubscriberId		iIMSI;					//< Pointer to the subscriber ID
	TInt										iError;
	};
	TSimTsySubsriberId iSubscriberId;
	
	CSimCallBarring*							iCallBarring;
	CSimCallForwarding*							iCallForwarding;
	CSimCallWaiting*							iCallWaiting;
	CSimPhoneInitialise*						iSimPhoneInitialise;

	struct TSimTsyPhoneId
		{
		TBuf<RMobilePhone::KPhoneManufacturerIdSize> iManufacturerId;	//Manufacturer identity
		TBuf<RMobilePhone::KPhoneModelIdSize> iModelId;	//Model identity
		TBuf<RMobilePhone::KPhoneRevisionIdSize> iRevisionId;	//Revision identity
		TBuf<RMobilePhone::KPhoneSerialNumberSize> iSerialId;	//IMEI (or ESN) identity
		TInt iError;			//Error code for failure
		};
	
	TSimTsyPhoneId iPhoneId;
	TUint32 iPhoneIdCaps;

	// timer and callback classes for NetworkMode
	CSimTimer* iNetworkModeTimer;   								//< Timer for the Network Mode change simulation
	class CNetworkModeTimerCallBack : public MTimerCallBack
		{
	public:
		CNetworkModeTimerCallBack();
		void TimerCallBack(TInt /*aId*/);
		void SetHandle(CSimPhone* aPhone);
	private:
		CSimPhone *iPhone;
		};
	CNetworkModeTimerCallBack iTimerCallBackNetworkMode;


	// timer and callback classes for SetCallProcessingSuspendState
	CSimTimer* iSetCallProcessingSuspendStateTimer;
	class CSetCallProcessingSuspendStateTimerCallBack : public MTimerCallBack
		{
	public:
		CSetCallProcessingSuspendStateTimerCallBack();
		void TimerCallBack(TInt aId);
		void SetHandle(CSimPhone* aPhone, const TTsyReqHandle aHandle);
	private:
		CSimPhone *iPhone;
		TTsyReqHandle iHandle;
		};
	
	// Observer class monitoring test number property
	NONSHARABLE_CLASS(CSimTestNumberObserver) : public CActive
	    {
	public:
	    static CSimTestNumberObserver* NewL(CSimPhone& aSimPhone);
	    virtual ~CSimTestNumberObserver();
	        
	private:    // methods from CActive
	    virtual void RunL();
	    virtual void DoCancel();
	        
	private:
	    CSimTestNumberObserver(CSimPhone& aSimPhone);
	    void ConstructL();
	    void Start();
	    
	private:
	    CSimPhone& iSimPhone;   // Reference to sim phone object (owner object)
	    RProperty iProperty;   // Property describing the event we require notification of
	    };
	
private:
    CSetCallProcessingSuspendStateTimerCallBack iTimerCallBackSetCallProcessingSuspendState;
    CSimTestNumberObserver* iTestNumberObserver; 
	};

#endif
