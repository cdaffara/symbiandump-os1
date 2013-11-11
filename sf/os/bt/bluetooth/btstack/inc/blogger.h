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
//

#ifndef BLOGGER_H__
#define BLOGGER_H__

#include "debug.h"

#ifndef BLOGGING__
NONSHARABLE_CLASS(CBlogger): public CBase
	{
public:
	IMPORT_C TAny* GetFloggerInfo();
	};
#endif

#ifdef BLOGGING__

// Stack Blogging Layers
const TInt KBlogLayerLinkLayer =1;
const TInt KBlogLayerL2CAP     =2;
const TInt KBlogBadLayerChoice =3;
const TInt KBlogLayerRFComm    =4;

// Stack Layers Logging Masks
const TInt KBlogNoLayer   =0x00000000;
const TInt KBLogLinkLayer =0x00000001;
const TInt KBlogL2CAP     =0x00000002;
const TInt KBLogRFComm    =0x00000004;
const TInt KBlogAllLayers =0x7fffffff;

//Blogging Verbosity Levels
const TInt KBlogVerbosityOff          =0x00000000; 
const TInt KBlogVerbosityTimeCritical =0x00000001; 
const TInt KBlogVerbosityMinimal      =0x00000002; 
const TInt KBlogVerbosityNormal       =0x00000003; 
const TInt KBlogVerbosityBTQualLevel  =0x00000004;
const TInt KBlogVerbosityLoud         =0x00000005; 
const TInt KBlogVerbosityAllLevels    =0x00000006;


//BLogging Type Masks
const TInt KBlogTypeLogAllMask          = 0xffffffff;
const TInt KBlogTypeProtocol	        = 0x00000001;
const TInt KBlogTypeTimestamp           = 0x00000002;
const TInt KBlogTypeTimeout             = 0x00000004;
const TInt KBlogTypeTestAppContext      = 0x00000008;
const TInt KBlogTypeFCS                 = 0x00000010;
const TInt KBlogTypeBaseband            = 0x00000020;
const TInt KBlogTypeChannelNumber       = 0x00000040;
const TInt KBlogTypePSMNumber           = 0x00000080;
const TInt KBlogTypeIncomingPacket      = 0x00000100;
const TInt KBlogTypeOutgoingPacket      = 0x00000200;
const TInt KBlogTypeRawFrames           = 0x00000400;
const TInt KBlogTypeL2CAPSignallerState = 0x00000800;
const TInt KBlogTypeRFCOMMMuxState      = 0x00001000;
const TInt KBlogTypeSapState            = 0x00002000;
const TInt KBlogTypeCurrentState        = 0x00004000;
const TInt KBlogTypeStateChange         = 0x00008000;
const TInt KBlogTypeRequest             = 0x00010000;
const TInt KBlogTypeExpectedResponse    = 0x00020000;
const TInt KBlogTypeActualResponse      = 0x00040000;
const TInt KBlogTypeInquiry             = 0x00080000;
const TInt KBlogTypeInquiryResult       = 0x00100000;
const TInt KBlogTypeKeyRegistration     = 0x00200000;
const TInt KBlogTypeKeyFailure          = 0x00400000;
const TInt KBlogTypeLinkKeyInteraction  = 0x00800000;
const TInt KBlogTypeCtrlFrames          = 0x01000000;
const TInt KBlogTypeUIHFrames           = 0x02000000;
const TInt KBlogTypeMuxCtrlFrames       = 0x04000000;
const TInt KBlogTypeCtrlFrameBits       = 0x08000000; 
const TInt KBlogTypeOutgoingFrames		= 0x10000000; 
const TInt KBlogTypeCreditAccounting    = 0x20000000;
const TInt KBlogTypeSecurity            = 0x40000000;
const TInt KBLogTypeLocalHCPokes        = 0x80000000;

//Logging LITs


_LIT(KBlogCommand, "command");
_LIT(KBlogResponse, "response");


_LIT(KBlogBluetoothAddrLit,"Bluetooth Address:");

//Layer/Protocol dependent
_LIT(KBlogIncomingPacket, "Incoming packet, Type %d, Size %d, Payload %08x"); //received data pkt with size and type of pkt
_LIT(KBlogOutgoingPacket, "Outgoing packet, Type %d, Size %d, Payload %08x"); //sent data pkt with size and type of pkt
_LIT(KBlogRawFrames, "Frame Type %d, 0x%8x");// frame type and content - different specific info required from different types of Frame
_LIT(KBlogSapId, "%d SAP %08x"); //type of SAP (RFCOMM/L2CAP) and sap id
_LIT(KBlogSapState, "SAP State %d");
_LIT(KBlogCurrentState, "Current State %d");
_LIT(KBlogStateChange, "State %d");
_LIT(KBlogInquiryResult, "Inquiry Result");

//L2CAP
_LIT(KBlogChannelNumber, "Channel %d");
_LIT(KBlogL2CAPSignallerState, "Signaller State %d");
_LIT(KBlogPSMNumber, "PSM %d");
_LIT(KBlogL2, "L2: ");
_LIT(KBlogL2Test, "Test");
_LIT(KBlogTest, "L2: TEST NUMBER is %d");




//RFCOMM

_LIT(KBlogMuxId, "MUX %08x");
_LIT(KBlogFrameid, "Frame id %08x");
//States
_LIT(KBlogRFCOMMMuxState, "Mux State %d");
//Frame Parts
_LIT(KBlogDLCI, "         DLCI %d");
_LIT(KBlogPFSet, "         P/F bit = %d");
_LIT(KBlogEASet, "         EA  bit = %d");
_LIT(KBlogCRSet, "         C/R bit = %d");
_LIT(KBlogActualFCS, "         Actual PDU FCS %d");
_LIT(KBlogComputedFCS, "         Computed FCS %d - (should be 0xCF)");
_LIT(KBlogComparedFCS, "         Compared FCS %d");
_LIT(KBlogSignals, "         Signals 0x%02x");

//Timeouts
_LIT(KBlogTimeout, "T1 timer expired awaiting response for ");
//Ctrl Frames
_LIT(KBlogSABM, "SABM");
_LIT(KBlogDM, "DM");
_LIT(KBlogUA, "UA");
_LIT(KBlogDISC, "DISC");
//Frame Types
_LIT(KBlogFrameType, "Frame of type %d");
_LIT(KBlogCtrlFrame, "Ctrl Frame: ");
_LIT(KBlogUIHCtrlFrame, "UIH Ctrl Frame");
_LIT(KBlogSimpleUIH, "UIH simple data frame");
_LIT(KBlogShortUIH, "UIH simple data frame");
_LIT(KBlogLongUIH, "UIH simple data frame");
_LIT(KBlogCreditUIH, "UIH credit data frame");
_LIT(KBlogLongCreditUIH, "Tx :Long UIH credit data frame with %d credits");
_LIT(KBlogShortCreditUIH, "Tx: Short UIH credit data frame with %d credits");
_LIT(KBlogMuxCtrlFrame, "Mux Ctrlframe");
//MuxCtrlCommands
_LIT(KBlogTestCommand, "TestCommand");
_LIT(KBlogFcOn, "FcOn");
_LIT(KBlogFcOff, "FcOff");
_LIT(KBlogMSC, "MSC");
_LIT(KBlogRPN, "RPN");
_LIT(KBlogRLS, "RLS");
_LIT(KBlogPN, "PN");
_LIT(KBlogNSC, "NSC");
//Data
_LIT(KBlogNewData, "New packet of data arrived from L2CAP, count %d");
_LIT(KBlogNewDataFrame, "Creating new datapacket for outbound queue, dlci %d, data length %d, sap %08x, ");
_LIT(KBlogWriteData, "Writing data, 0x%x");
_LIT(KBlogNonZeroLength, "with non-zero length");

//Link Layer
_LIT(KBlogNewLinkConnectionEvent,"LL: Connection complete event. Code %d, link type=%d ");
_LIT(KBlogNewLinkConnectionReqEvent, "LL: Incoming connection request event"); 
_LIT(KBlogNewLinkDisconnectionEvent,"LL: Disconnection complete event. Handle %x, Code %d, Reason %d");
_LIT(KBlogHCSendPacketsOK,"LL: Notifying DataQ Controller of %d Completed packets from HC to remote");
_LIT(KBlogLLDataIn ,"LL: Data received for handle %d, flags %d, length %d");
_LIT(KBlogACLFrame,"LL: ACL Frame:");
_LIT(KBlogLLDataIn2Null,"LL: Warning!! Data received with no datalink notifier in place");
_LIT(KBlogLLScanEnableResult,"LL: ReadScanEnableResult received, with error code %d, value %d");
_LIT(KBlogHCBufferSizeResult,"LL: ReadBufferSizeResult - aErr %d, aAclMaxLen %d, aNoACL %d");
_LIT(KBlogHCReadTimeoutResult,"LL: Warning!! Unhandled ReadTimeoutResult (EConnectionAcceptTimeout)");
_LIT(KBlogVendorEvent,"LL: Warning!! Just received a Vendor Specific Debug Event !");
_LIT(KBlogHardwareError,"LL: Error! HardwareErrorEvent Received: %d");
_LIT(KBlogHCIBufferNewCredits,"LL: HCI Buffer Strategy, Credit available, trying to issue.");
_LIT(KBlogLLTransportFreed,"LL: LinkMgr, Transport is now free");
_LIT(KBlogHCIssuedCmdCredits,"LL: LinkMgr, HC issued %d command credits");
_LIT(KBlogLLSetLocalNameComplete,"LL: SetLocalName Command Complete Event");
_LIT(KBlogLLWritePageTOComplete,"LL: WritePageTimeout Command Complete Event");
_LIT(KBlogLLWriteCoDComplete,"LL: WriteClassOfDevice Command Complete Event");
_LIT(KBlogLLWriteVoiceSettingComplete,"LL: WriteVoiceSetting Command Complete Event");
_LIT(KBlogLLWriteScanEnable,"LL: WriteScanEnable Command Complete Event");
_LIT(KBlogLLWriteCurrentIACLAPComplete,"LL: WriteCurrentIACLAP Command Complete Event");
_LIT(KBlogLLSetHC2HostFCComplete,"LL: SetHostControllerToHostFlow Command Complete Event");
_LIT(KBlogLLCommandWarning,"LL: Warning, Unhandled Command complete event! Opcode %d error code %d");
_LIT(KBlogLLWriteAFHHostChannelClassification,"LL: WriteAFHHostChannelClassificationComplete Command Complete Event");
_LIT(KBlogLLReadAFHChannelAssessmentModeComplete,"LL: ReadAFHChannelAssessmentModeComplete Command Complete Event");
_LIT(KBlogLLWriteAFHChannelAssessmentModeComplete,"LL: WriteAFHChannelAssessmentModeComplete Command Complete Event");

//MISC
_LIT(KKeyRegistration, "Key Registration");
_LIT(KKeyFailure, "Key Failure");
_LIT(KPINKeyInteraction, "PIN Key");

// Const Lit Lengths for the predifined Lits
//255 is the max RDebug::Print size
const TInt KRDBMaxLit=255;
const TInt KBlogDateTimeLitLength=36; //depends on date, time formating lits
const TInt KBlogTCPreambleLitLength=20;
const TInt KBlogTestCaseMaxLitLength=KRDBMaxLit+KBlogDateTimeLitLength+KBlogTCPreambleLitLength;
const TInt KBlogTestCaseResultPreambleLitLength=36;
const TInt KBlogTestCaseResultMaxLitLength=KBlogTestCaseResultPreambleLitLength+KBlogDateTimeLitLength+KRDBMaxLit;
const TInt KBlogTesterMessageInfoLitLength=100;
const TInt KBlogTesterMessageInfoMaxLitLength=KBlogDateTimeLitLength+KBlogTesterMessageInfoLitLength;
const TInt KBlogMessageMaxLitLength=512;

//Blogging Output Resource Types
// if we ever need to put more than 2 then implement it with dynamic binding
const TInt KBlogOutputTypeIsRDebug  =0;
const TInt KBlogOutputTypeIsFlogger =1;

// The number of chars to be printed on single line over COM1 or on a file
const TInt KBlogOutputWindowDefault=80;


// Common Lits

// need to be in sync with KBlogDateTimeLitLength
_LIT(KBlogTimeFormat,"%J%:1%T%:2%S      "); 
_LIT(KBlogDateFormat,"%1%/1%2%/2%3      ");
_LIT(KBlogBlankDate,"--:--:----      ");
_LIT(KBlogBlankTime,"--:--:--      ");
_LIT(KBlogTestCaseLit,"Test Case Running:  ");
// need to be in sync with KBlogTesterMessageInfoLitLength
//_LIT(KBlogTesterMessageInfoLit,"[Message from the tester follows:]"); 
_LIT(KBlogTesterMessageInfoLit,"Tester: "); 

// Test Case Result Consts
// these should be used with the KBloggerSetTestCaseResult option
const TInt KBlogTestCaseResultPassed       = 0;
const TInt KBlogTestCaseResultFailed       = 1;
const TInt KBlogTestCaseResultInconclusive = 2;
const TInt KBlogTestCaseResultSkipped      = 3;
const TInt KBlogTestCaseResultUpperNo      = 3; // for bounds checking

// Test Case Result Lits
// these should be used with the KBloggerSetTestCaseResult option
// remember to cjheck with KBlogTestCaseResultPreambleLitLength
// if any of the below Lits gets changed
_LIT(KBlogTestCaseResultLitPass,"[PASSED] test for test case : ");
_LIT(KBlogTestCaseResultLitFailed, "[FAILED] test for test case : ");
_LIT(KBlogTestCaseResultLitInconclusive, "[INCONCLUSIVE] test for test case : ");
_LIT(KBlogTestCaseResultLitSkipped,"[SKIPPED] test for test case : ");

enum TBTBloggingOptions
	{
	KBloggerSetLayers,
	KBloggerSetVerbosity,
	KBloggerSetLoggingMask,
	KBloggerSetMedium,
	KBloggerSetTestCaseName,
	KBloggerSetOutputWindow,
	KBloggerSetTestCaseResult, // to be used with the appropriate lits to indicate result
	KBloggerSetTesterMessage,  // to be formated according to the corresponding lit
	KBloggerSetAllOptions //should use TBlogSettings
	};

struct TBlogSettings
	{
	TInt iLayersToLog;
	TInt iVerbosity;
	TInt iLoggingMask;
	TInt iMediumToLog;
	TInt iOutputWindowSize;
	TBuf<255> iTestCaseLit;
	};
typedef TPckgBuf<TBlogSettings> TBlogSettingsPckg;

typedef TBuf<255>  TBlogTestCaseLit;

enum TBloggerPanic
	{
	EBloggerLayerRegistrationOutOfRange    =0,
	EBloggerLayerBloggetteAlreadyRegistered=1,
	EBloggerLayerRequestedOutOfRange       =2,
	EBloggerResourceTypeIsBogus            =3,
	EBloggerTestCaseResultIsBogus          =4, // check your KBlogTestCaseResultUpperNo
	EBloggerDescriptorLengthWasShorter     =5,
	EBloggerMessageLengthTooLong           =6
	};

class CRfcommSAP;
class CRfcommMuxCtrlFrame;
class CRfcommMuxer;
class CRfcommFrame;
class CRfcommCtrlFrame;
class CBloggette;

NONSHARABLE_CLASS(CBlogger) : public CBase
	{
public:
	~CBlogger();
	static CBlogger* NewL();
	void ConstructL();
	TInt LoggingMask(TInt aLayer, TInt aVerbosity) const;
	CBloggette* GetLayerBloggette(TInt aLayer); //only one per layer please
	void Register(CBloggette* aLayerBloggette);
	void Blog(const TDesC& aMessage, TInt aFrameWindow);
	void Blog(const TDesC& aMessage);
	void HexDump(const TDesC& aDescription,const TDesC8& aHexFrame,TInt aFrameWindow=KBlogOutputWindowDefault);
	// to be used by the protocol layer which receives the SetOp
	TInt UpdateLoggingSettings(TUint aOptionType,const TDesC8& aOption);
	void StoreFloggerInfo(TAny* aLogInfo);
	IMPORT_C TAny* GetFloggerInfo();
private:
	CBlogger();
	void Panic(TBloggerPanic aPanic);
	// Blogging calls
	void BlogTestCase(TDesC& aTestCaseName);
	void BlogTestCaseResult(TInt aTestCaseResult);
	void BlogTesterMessage(TDesC& aTesterMessage);	
	void DoBlog(const TDesC& aMessage, TInt aFrameWindow=KBlogOutputWindowDefault);
	// Output and formating
	void DoOutputToResourceL(const TDesC& aMessage);
	void CreateBlogFileL();
	void DoFileL();
	void MakefilenameL(const TDesC &aFileName);
	TInt DoDateTimeFormat(TDes& aDesToPrepend);
	//for testing and debugging only
	void DoBloggerTests(); 
private:
	friend class CHCILinkMgr; // for testing
	CBloggette* iLLBloggette;
	CBloggette* iL2Bloggette;
	CBloggette* iRFBloggette;

	TInt  iOutputWindowSize;
	TInt  iVerbosityLevel;
	TInt  iLayersToLogMask;
	TInt  iLoggingMask;
	TInt  iMediumType;  // 0 for port 0, 1 for Flogger - defaults to zero because member of C Class.
	TBuf<KRDBMaxLit> iTestCaseLit;

	TAny* iFloggerInfo; // when flogging is compiled in
	RFs	  ifileserv;
	RFile ifile;
	TBool iFileExists;
	HBufC* ifilename;
	};

NONSHARABLE_CLASS(CBloggette) : public CBase
	{
public:
	~CBloggette();
	inline TInt Layer() const;
	void Log(TRefByValue<const TDesC> aFormatingLit, ...);
	void HexDump(TRefByValue<const TDesC> aDescription,const TDesC8& aHexFrame, TInt aFrameWindow=80);
	virtual TDesC NameDes()=0;
protected:
	CBloggette(CBlogger* aProvider, TInt aLayer);
	TInt	  iLayer;
	CBlogger* iBlogger;
	};

NONSHARABLE_CLASS(CLLBloggette) : public CBloggette
	{
public:
	static CLLBloggette* NewL(CBlogger* aProvider, TInt aLayer); //only one per layer please
	~CLLBloggette();
	virtual TDesC NameDes();
private:
	CLLBloggette(CBlogger* aProvider, TInt aLayer);
	};

NONSHARABLE_CLASS(CL2Bloggette) : public CBloggette
	{
public:
	static CL2Bloggette* NewL(CBlogger* aProvider, TInt aLayer);
	~CL2Bloggette();
	virtual TDesC NameDes();
private:
	CL2Bloggette(CBlogger* aProvider, TInt aLayer);
	};

NONSHARABLE_CLASS(CRFBloggette) : public CBloggette
	{
	friend class CRfcommCtrlFrame;
public:
	static CRFBloggette* NewL(CBlogger* aProvider, TInt aLayer);
	~CRFBloggette();
	virtual TDesC NameDes();
private:
	CRFBloggette(CBlogger* aProvider, TInt aLayer);
	friend class CRfcommFrame; //Shehla
	friend class CRfcommMuxer; //Shehla	
	void Timeout(/*CRfcommSAP* aSAP, CRfcommMuxer* aMux,  */CRfcommFrame* aFrm );
	void ExplainOutgoingFrame(CRfcommFrame* aFrm, CRfcommMuxer* aMux);
	void ExplainIncomingFrame(TUint8 addr, TUint8 ctrl, TBool poll);
	void LogMuxCommand(CRfcommSAP* aSAP, CRfcommMuxer* aMux, TUint8 aCommand);
	void IncomingCtrlMessage(TUint8 aCtrlmessage, TInt aLength);
	};

#endif
#endif

