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
// This file contains the definitions of the Simulator TSY Sat functionality.  
// 
//

/**
 @file
 @internalAll
*/

#ifndef __CSIMSAT_H__
#define __CSIMSAT_H__

#include <et_phone.h>
#include <testconfigfileparser.h>
#include "csimtimer.h"
#include "utils.h"
#include <etelsat.h>

#define SIMTSY_MMMESSAGE_ADD_INFO_1		_L("KNoSpecificMeProblem")
#define SIMTSY_MMMESSAGE_ADD_INFO_2		_L("KScreenBusy")
#define SIMTSY_SET_FRMS_ADD_INFO_1		_L("KRequestSizeNotSupported")
#define SIMTSY_PRV_LCLINFO_ADD_INFO_1		_L("KNoService")
#define SIMTSY_SEND_USSD_ADD_INFO		_L("KMeBusyOnUssd")
#define SIMTSY_SEND_USSD_ADD_INFO_1		_L("KMeBusyOnSs")
#define SIMTSY_SETUPCALL_ADD_INFO_1		_L("KMeBusyOnCall")
#define SIMTSY_LNCH_BRWSR_ADD_INFO1		_L("KNoSpecificBrowserError")
#define SIMTSY_LNCH_BRWSR_ADD_INFO2		_L("KBrowserUnavailable")		
#define SIMTSY_LNCH_BRWSR_ADD_INFO3		_L("KBearerUnvailable")
#define SIMTSY_LNCH_BRWSR_ADD_INFO4		_L("KMeUnableToReadProvisioningData")
#define SIMTSY_SET_FRMS_ADD_INFO_2		_L("KNumberOfFramesExceeded")
#define SIMTSY_MMS_TRF_FILE_PATH		_L("TransferFilePath")
#define SIMTSY_MMS_MSG_BUF			_L("MMMessageBuf")
#define SIMTSY_MMS_MSG_ID			_L("MMMessageIdentifier")
#define SIMTSY_MMS_INFORMATION			_L("ABCDE")
#define SIMTSY_MMS_LAST_ENV			0x2A
#define SIMTSY_UICC_SUCC_RSP			0x9000
#define SIMTSY_UICC_RETRY_RSP			0x9300
#define SIMTSY_UICC_FLR_RSP			0x6F00
#define SIMTSY_UICC_FLR_RSP_1			0x6200
#define SIMTSY_UICC_FLR_RSP_2			0x6300

#define SIMTSY_PLAYTONE_TESTCASES_NUMBER		0x04
#define SIMTSY_SETUPCALL_TESTCASES_NUMBER		0x05

class CSimPhone;
class CSimSat : public CSubSessionExtBase, public MTimerCallBack
	{
public:
	static CSimSat* NewL(CSimPhone* aPhone);
	CSimSat(CSimPhone* aPhone);
	void ConstructL();
	TInt ConfigL(unsigned int);
	~CSimSat();

	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual TInt NumberOfSlotsL(const TInt aIpc);

	const CTestConfigSection* CfgFile();

private:
	//< The Sat API methods implemented by the TSY
	//< These will be called from the ExtFunc method above
	TInt NotifyRefreshPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyRefreshPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyRefreshRequired(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyRefreshRequired(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyRefreshRequiredCancel(const TTsyReqHandle aTsyReqHandle);
	TInt RefreshAllowed(const TTsyReqHandle aTsyReqHandle, TDes8* aRefreshAllowedRsp);
	TInt RefreshAllowedCancel(const TTsyReqHandle aTsyReqHandle);
	
	
	TInt NotifyRetrieveMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd);
	TInt NotifyRetrieveMultimediaMsgPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySubmitMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd);
	TInt NotifySubmitMultimediaMsgPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyDisplayMultimediaMsgPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd);
	TInt NotifyDisplayMultimediaMsgPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySetFramesPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd);
	TInt NotifySetFramesPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyGetFramesStatusPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd);
	TInt NotifyGetFramesStatusPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyLocalInfoPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd);
	TInt NotifyLocalInfoPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyOpenChannelPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd);
	TInt NotifyOpenChannelPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyPlayTonePCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd);
	TInt NotifyPlayTonePCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySetupCallPCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd);
	TInt NotifySetupCallPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyRefreshV6PCmd(const TTsyReqHandle aTsyReqHandle,TDes8* aPCmd);
	TInt NotifyRefreshV6PCmdCancel(const TTsyReqHandle aTsyReqHandle);

	TInt MmsNotificationDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt MmsNotificationDownloadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt MmsTransferStatus(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt MmsTransferStatusCancel(const TTsyReqHandle aTsyReqHandle);
	TInt UssdDataDownload(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt UssdDataDownloadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt EventDownload(const TTsyReqHandle aTsyReqHandle, RSat::TEventList* aPCmd, TDes8* aRsp);
	TInt EventDownloadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySendUssdPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySendUssdPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyGetInkeyPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyGetInkeyPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifySendSsPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifySendSsPCmdCancel(const TTsyReqHandle aTsyReqHandle);
	TInt NotifyLaunchBrowserPCmd(const TTsyReqHandle aTsyReqHandle, TDes8* aPCmd);
	TInt NotifyLaunchBrowserPCmdCancel(const TTsyReqHandle aTsyReqHandle);
		
	TInt GetMeSideSatProfile(const TTsyReqHandle aTsyReqHandle, TDes8* aMeSimSatProfile);
	TInt GetMeSideSatProfileCancel(const TTsyReqHandle aTsyReqHandle);
	TInt ClientSatProfileIndication(const TTsyReqHandle aTsyReqHandle, TDes8* aClientSatProfile);

	TInt TerminalRsp(const TTsyReqHandle aTsyReqHandle, RSat::TPCmd* aPCmd, TDes8* aRsp);
	TInt TerminalRspCancel(const TTsyReqHandle aTsyReqHandle);
	TInt RetrieveMultimediaRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SubmitMultimediaRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt DisplayMultimediaRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SetFramesRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt GetFramesStatusRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt ProvideLocalInfoRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt OpenChannelRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt PlayToneRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SetupCallRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt RefreshRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt SendUssdRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt GetInkeyRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);
	TInt LaunchBrowserRsp(const TTsyReqHandle aTsyReqHandle, TDes8* aRsp);

	/* Functions for conversion of Text to Hex */
	TInt ConvertTextToFileList(const TPtrC8& aText, RSat::TRefreshFileList& aFileList) const;
	TInt ConvertTextToTBuf242(const TPtrC8& aText, TBuf<242>& aFileList) const;
	TInt ConvertTextToTBuf243(const TPtrC8& aText, TBuf<243>& aFileList) const;
	TInt ConvertTextToTBuf254(const TPtrC8& aText, TBuf<254>& aFileList) const;
	TInt ConvertTextToTBuf255(const TPtrC8& aText, TBuf<255>& aFileList) const;
	TInt ConvertTextToTBuf4(const TPtrC8& aText, TBuf8<4>& aFileList) const;
	TInt ConvertTextToTBuf16(const TPtrC8& aText, TBuf8<16>& aFileList) const;
	
	TInt randTime(); // Function to generate random Time to simulate asynchronous request handling.
	virtual void TimerCallBack(TInt aId);	//< Timer Callback for the timeout events

private: // Data
	CSimPhone* iPhone;		//< Pointer to the phone object (The parent of this class)
	CSimTimer* iTimer;	//< Pointer to the Timer object for callback
	
	struct TSatInfo
		{
		TInt iDuration;		// < Duration spent in this refresh state
		TInt iType;		//< Type of refresh command
		TPtrC8 iFileList;	//< Optional list of files that need to be refreshed.
		};

	struct TMMRetrieve
		{
		TInt iDestn;		// Destination for the command
		TInt iAlphaStatus;	// AlphaID status
		TPtrC8 iAlphaIDBuf;	// AlphaID Buffer
		TInt iIconID;		// IconID identifier
		TInt iIconIDBuf;	// IconID qualifier
		TPtrC8	iMMMsgRef;	// MMMessage Reference
		TPtrC8  iRcptnFile;	// Reception File
		TPtrC8  iContentId;	// Content Identifier
		TPtrC8  iMsgId;		// Message Identifier
		TInt iTextAttStatus;	// Status for the TextAttribute
		TPtrC8  iTextAttBuf;	// Buffer for the TextAttribute
		};
		
	struct TMMSubmit
		{
		TInt iDestn;		// Destination for the command
		TInt iAlphaStatus;	// AlphaID status
		TPtrC8 iAlphaIDBuf;	// AlphaID Buffer
		TInt iIconID;		// IconID identifier
		TInt iIconIDBuf;	// IconID qualifier
		TPtrC8 iSbmsnFile;	// Submission File
		TPtrC8  iMsgId;		// Message Identifier
		TInt iTextAttStatus;	// Status for the TextAttribute
		TPtrC8  iTextAttBuf;	// Buffer for the TextAttribute
		};

	struct TMMDisplay
		{
		TInt iDestn;		// Destination for the command
		TPtrC8 iSbmsnFile;	// Submission File
		TPtrC8  iMsgId;		// Message Identifier
		TInt iImmRsp;		// Immediate response
		TInt iDispPri;		// Display Priority
		TInt iClrScr;		// ClearScreen Trigger
		};
	
	struct TSetFrms
		{
		TInt iDestn;		// Destination for the command
		TUint8	iFrmId;		// Frame identifier
		TInt iFrmLayout;	// Frame layout
		TPtrC8 iFrmLayoutBuf;// Frame layout buffer
		TUint8 iDefFrmId;	// Default Frame identifier
		TInt iFrmSeparator;	// Frame Separator.
		};
	
	struct TGetFrmsSts
		{
		TInt iDestn;		// Destination for the command
		TInt8 iFrmId;		// Frame identifier
		TPtrC8 iFrmList;	// Frame List
		};
	
  	struct TMmsNotificationDownload
		{
		TUint iSrc;			// Source of the command
		TUint iDestn;		// Destination of the command
		RSat::TMMSNotification iMMSNotfn;	// MMS Notification information
		TUint8 iLastEnv;	// last envelope sent to transmit notification into the card.
		TUint iUICCRsp;		// Response from UICC for the action of the command.
		};
	
	struct TSendUSSD
		{
		TInt iTextAttStatus;	// Status for the TextAttribute
		TPtrC8  iTextAttBuf;	// Buffer for the TextAttribute
		};
	
	struct TUssdData
		{
		TUint iSrc;			// Source of the command
		TUint iDestn;		// Destination of the command
		TUint8 iDcs;		// Data Coding Scheme
		TBuf<RSat::KStringMaxSize> iUssdStr;	// USSD String
		TUint iUICCRsp;		// Response from UICC for the action of the command.
		};
	
	struct TLocalInfo
		{
		TUint iDeviceId;	// Destination of the command
		TUint iLocalInfoType;// LocalInformation type
		TUint iUTRANQlfr;	// UTRAN Qualifier.
		};
	
	struct TOpenChannel
		{
		TUint iBearerType; 	// Bearer Type of the channel.
		TUint iFrameId;		// Frame identifier of the channel.
		};
	
	struct TMiscPCmd
		{
		TUint iFrameId;		// Frame identifier for the Proactive command
		};
	
	struct TEventDnld
		{
		TInt iVar;			// iNetworkSearchMode/browsingStatus/iFrameId
		TPtrC8 iFrameList;	// Frame list
		};
	
	struct TLnchBrwsr
		{
		TUint iFrameId;		// Frame identifier for the Proactive command
		TUint iTerRsp;		// Terminal response
		};
	
	CArrayFixFlat<TSatInfo>* iSatInfo;	// < Array containing all the sat changes for this test.
	CArrayFixFlat<TSatInfo>* iRefresh;	// < Array containing the information about REFRESH V2 command 
	CArrayFixFlat<TSatInfo>* iElemFiles;// < Array containing the information about REFRESH command with EF change information

	CArrayFixFlat<TMMRetrieve>* iMMRetrieve;	// < Array containing the MMRetrieve Command info.
	CArrayFixFlat<TMMSubmit>* iMMSubmit; // < Array containing the MMSubmit Command info.
	CArrayFixFlat<TMMDisplay>* iMMDisplay;	// < Array containing the MMDisplay Command info.
	CArrayFixFlat<TSetFrms>* iSetFrms;	// < Array containing the SetFrms Command info.
	CArrayFixFlat<TGetFrmsSts>* iGetFrmsSts;	// < Array containing the GetFrmsSts Command info.
	CArrayFixFlat<TMmsNotificationDownload>* iMmsDownloadData;	// < Array containing the MMNotification Command notification info.
	CArrayFixFlat<TUssdData>* iUssdData; // < Array containing the USSD Data Download Command info.
	CArrayFixFlat<TLocalInfo>* iLocalInfo;	// < Array containing the PROVIDE LOCAL INFORMATION Command
	CArrayFixFlat<TOpenChannel>* iOpenChn;	// < Array containing the OPEN CHANNEL Command info.
	CArrayFixFlat<TMiscPCmd> *iMiscCmd;	// < Array containing the common information of some proactive commands. 
										// < like PLAYTONE,etc.
	CArrayFixFlat<TLnchBrwsr>* iLnchBrwsr; // <Array containing the information of LAUNCH BROWSER command.
	CArrayFixFlat<TEventDnld> *iEventList; //< Array containing the information about the EVENT DOWNLOAD command.
	CArrayFixFlat<TSendUSSD> *iSendUssd; //< Array containing the information about the SEND USSD command
	CArrayFixFlat<TSendUSSD> *iSendSS;	//< Array containing the information about the SEND SS command
	
	
	TInt iRetIndex;			// Current index from the iMMRetrieve array
	TInt iRetTIndex;		// Previous index from the iMMRetrieve array for TerminalRsp
	TInt iSubIndex;			// Current index from the iMMSubmit array
	TInt iSubTIndex;		// Previous index from the iMMSubmit array for TerminalRsp
	TInt iDispIndex;		// Current index from the iMMDisplay array
	TInt iDispTIndex;		// Previous index from the iMMDispaly array for TerminalRsp
	TInt iSetFrmsIndex;		// Current index from the iSetFrms array
	TInt iGetFrmsStsIndex;	// Current index from the iGetFrmsSts array
	TInt iMmsDownloadDataIndex;		// Current index from the iMMDnld array 
	TInt iLocalInfoIndex;	// Current index from the iLocalInfo array
	TInt iOpenChnIndex;		// Current index from the iOpenChn array
	TInt iMiscCmdIndex;		// Current index from the iMiscCmd array
	TInt iEventDnldIndex;	// Current index from the iEventList array
	TInt iSendUssdIndex;	// Current index from the iSendUssd array
	TInt iSendSsIndex;		// Current index from the iSendSs array
	TInt iRefreshIndex;		// Current index from the iRefresh array
	TInt iElemFilesIndex;	// Current index from the iElemFiles array
	TInt iLnchBrwsrIndex;	// Current index from the iLaunchBrowser array
	
	TBool iPendingTerRsp;	// Boolean that contains the information whether a TerminalRsp is pending/not.
	TBool iSendRspMmsNotificationDownload;// Boolean that contains the information whether to send MMSNtfndnld response/not.
	TBool iSendRspEventDnld; // Boolean that contains the information whether to send EventDnld response/not.
	TBool iConfigDone;	// Boolean that carries the information whether the configuration is done / not.
	TInt iSatIndex;			// < Current index from the iSatInfo array
	TNotifyData	iNotifyRefresh;	//< Holds the notification data (a flag, TsyReqHandle and any raw data) 
	TNotifyData	iRefreshRequired; //< Holds the notification data (a flag, TsyReqHandle, data is unused) 
	TNotifyData	iRefreshAllowed; //< Holds the notification data (a flag, TsyReqHandle, data is unused) 
	TNotifyData iNotifyRetrieveMM; //Holds the notification data for RETRIEVE MULTIMEDIA MESSAGE COMMAND
	TNotifyData iNotifySubmitMM; //Holds the notification data for SUBMIT MULTIMEDIA MESSAGE COMMAND
	TNotifyData	iNotifyDisplayMM; //Holds the notification data for DISPLAY MULTIMEDIA MESSAGE COMMAND
	TNotifyData iNotifySetFrms; //Holds the notification data for SET FRAMES PROACTIVE COMMAND
	TNotifyData iNotifyGetFrmsSts; // Holds the notification data for GET FRAMES STATUS PROACTIVE COMMAND
	TNotifyData iNotifyLocalInfo;	// Holds the notification data for PROVIDE LOCAL INFORMATION command
	TNotifyData iNotifyOpenChn;		// Holds the notification data for OPEN CHANNEL command
	TNotifyData iNotifyMiscCmd;		// Holds the notification data for some proactive commands like PLAYTONE,etc.
	TNotifyData iNotifySendUssd;	// Holds the notification data for SEND USSD data command
	TNotifyData iNotifySendSs;		// Holds the notification data for SEND SS data command
	TNotifyData iNotifyLnchBrwsr;	// Holds the notification data for LAUNCH BROWSER command.
			
	RSat::TRefreshType iType; //< Holds type of refresh command from config file
	RSat::TRefreshFileList iFileList; //< Holds file list from config file
	TInt iConvertTextToFileReturnCode; //< Holds most recent return code for text to file conversion
	TBool iRefreshRequiredPending; //< Holds a flag to indicate that first phase of refresh required cycle has completed
	TBool iPlayTone;	//< Holds a flag to indicate whether PLAYTONE testcases are handled or not.
	TBool iSetupCall;	//< Holds a flag to indicate whether SETUPCALL testcases are handled or not.
	TBool iGetInkey;	//< Holds a flag to indicate whether GET INKEY testcases are handled or not.
	TBool iRefreshEf;	//< Holds a flag to indicate whether EFs are updated to be notified by REFRESH command
	TBool iRefreshV2;	//< Holds a flag to indicate the version of the REFRESH command

	TTsyReqHandle iGetMeSideProfileHandle; // handle to hold the GetMeSideProfile API's handle.
	TTsyReqHandle iClientSideProfileHandle; // handle to hold the ClientSatSideProfile API's handle.
	TTsyReqHandle iMMSTransferStatusHandle;	// handle to hold the MMSTransferStatus API's handle.
	TTsyReqHandle iMmsDownloadDataHandle;	// handle to hold the MMS Notification Download's handle.
	TTsyReqHandle iEventDnldHandle;	// handle to hold the EVENT DOWNLOAD command's handle. 
	TTsyReqHandle iUSSDDataDownloadHandle;	// handle to hold the USSD DATA DONWLOAD API's handle. 
		
	};

#endif  //__CSimSat_H__

