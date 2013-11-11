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
// This file contains the entry point for the Multimode TSY Regression Test Harness.  It includes
// the main entry point for the test harness (E32Main) and the user interface processing.  The
// environment is initialized by methods in this file as well.  The Communications Database that
// is created here is specific to the ETel Regression Test Harness.
// 
//

/**
 @file
*/

#include <commsdattypesv1_1.h>

#include <e32base.h>
#include <e32test.h>
#include <c32comm.h>
#include <f32file.h>
#include <etelmm.h>

#include "Te_LoopbackCRxMess.h"
#include "Te_LoopbackCNoPduRxMess.H"
#include "Te_LoopbackCSca.H"
#include "Te_LoopbackCTxMess.H"
#include "Te_LoopbackCDataCall.H"
#include "Te_LoopbackCOddInits.H"
#include "Te_LoopbackCMessStor.H"
#include "Te_LoopbackCFailInits.H"
#include "Te_LoopbackCInCall.H"
#include "Te_LoopbackCSmsCancel.H"
#include "Te_LoopbackCShutdown.H"
#include "Te_LoopbackCSmsDelete.h"
#include "Te_LoopbackCSsFax.H"
#include "Te_LoopbackCDataCallCancel.H"
#include "Te_LoopbackCFaxPremClose.H"
#include "Te_LoopbackCTwoFaxRx.H"
#include "Te_LoopbackCVoiceCall.H"
#include "Te_LoopbackCDataCallCallBack.H"
#include "Te_LoopbackCDataCallRemoteHangupDial.H"
#include "Te_LoopbackCDataCallRemoteTerm.H"
#include "Te_LoopbackCPhoneBook.H"
#include "Te_LoopbackCNetwork.H"
#include "Te_LoopbackCPhoneLine.H"
#include "Te_LoopbackCCalls.H"
#include "Te_LoopbackCNoCnmi.H"
#include "Te_LoopbackCNoCmgf.H"
#include "Te_LoopbackCOOMVoiceCall.H"
#include "Te_LoopbackCDataCallDiffParam.H"
#include "Te_LoopbackCCbstParse.H"
#include "Te_LoopbackCCGQREQ_Responses.h"
#include "Te_LoopbackCUnsolicited.h"

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define PDD2_NAME _L("EUART2")
#define LDD_NAME _L("ECOMM")
#endif

#define SIMPLETESTMACRO(TEXT,CLASSNAME,SCRIPTNUMBER) \
		_LIT8(KTestLit_##CLASSNAME_##SCRIPTNUMBER,TEXT); \
		StartTest(KTestLit_##CLASSNAME_##SCRIPTNUMBER); \
		CLASSNAME* test_##CLASSNAME_##SCRIPTNUMBER=CLASSNAME::NewL(SCRIPTNUMBER); \
		CleanupStack::PushL(test_##CLASSNAME_##SCRIPTNUMBER); \
		User::LeaveIfError(test_##CLASSNAME_##SCRIPTNUMBER->RunTestL()); \
		CleanupStack::PopAndDestroy(); \
		EndTest(KTestLit_##CLASSNAME_##SCRIPTNUMBER); \
		User::After(1000000L);


// KMaxVariableDelay is Maximum Variable Delay value that can be input by the user as the timeout by script
// processing when the iParam value is set -1 in a TScript script whose iCommand is an EWait.
const TInt KInputWaitOnDelaySecs=5;

GLDEF_D RTest TESTL(_L("T_TsyLb"));
GLDEF_D RTelServer server;
GLDEF_D RTelServer server2;
GLDEF_D RMobilePhone iPhone;
GLDEF_D RMobilePhone phone2;
GLDEF_C TInt ReturnValue;
GLDEF_D CCommsDatabase* TheDb;
GLDEF_D RFs TheFs;
GLDEF_D RFile TestRpt;

_LIT(KTestReportFile,"C:\\T_TsyLb.TestReport.TXT");
//
// Create dummy ISP Table
//
TUint32 CreateISPTableL()
/**
 *
 * This method creates an ISP table entry in the communications database for an NT RAS
 * connection (no password allowed on the connection).
 *
 * @leave	This method leaves via operations failing while accessing the database.
 * @return	A dial out id
 *
 */
//
// NT RAS Connection without Database Stored Password
//
	{
	TUint32 dialOutId;
	CCommsDbTableView* table=TheDb->OpenTableLC(TPtrC(DIAL_OUT_ISP));

	TESTL(table->InsertRecord(dialOutId)==KErrNone);
	table->WriteTextL(TPtrC(COMMDB_NAME),_L("NT RAS"));
	table->WriteUintL(TPtrC(ISP_TYPE),(TUint32)EIspTypeInternetOnly);
	table->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
	table->WriteTextL(TPtrC(ISP_DEFAULT_TEL_NUM),KNullDesC);
	table->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),EFalse);
	table->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),ETrue);
	table->WriteLongTextL(TPtrC(ISP_LOGIN_SCRIPT),_L("CHARMAP [CODE PAGE 1252]\nLOOP 10\n{\nSEND \"CLIENT\"+<0x0d>\nWAIT 3\n{\n\"SERVER\" OK\n}\n}\nEXIT KErrNoAnswer$\n\nOK:\nEXIT\n"));
	table->WriteBoolL(TPtrC(ISP_DISPLAY_PCT),EFalse);
	table->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
	table->WriteTextL(TPtrC(ISP_IF_NETWORKS),_L("ip"));
	table->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
	table->WriteTextL(TPtrC(ISP_IF_AUTH_NAME),_L("RasUser"));
	table->WriteTextL(TPtrC(ISP_IF_AUTH_PASS),_L("O`rr"));
	table->WriteBoolL(TPtrC(ISP_IF_CALLBACK_ENABLED),EFalse);
	table->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
	table->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
	table->WriteBoolL(TPtrC(ISP_ENABLE_IP_HEADER_COMP),EFalse);
	table->WriteBoolL(TPtrC(ISP_ENABLE_LCP_EXTENSIONS),EFalse);
	table->WriteBoolL(TPtrC(ISP_DISABLE_PLAIN_TEXT_AUTH),EFalse);
	table->WriteBoolL(TPtrC(ISP_ENABLE_SW_COMP),EFalse);
	TESTL(table->PutRecordChanges()==KErrNone);

	CleanupStack::PopAndDestroy();
	return dialOutId;
	}

//
// Create Modem Table (This is the one that will actually be used)
//
void CreateModemTablesL()
/**
 * This method creates a modem table entry in the communications database.  Note that the
 * Port and CSY Name are "Loopback".  This is specific to the Regression Test Harness.  The
 * use of the "Infrared Mobile Phone" communications database name is for convience.
 *
 * This is the Modem table that will actually be used by the test harness.
 *
 * @leave	This method leaves if operations on the database fail.
 * @return	A modem id
 */
	{
	CCommsDbTableView* table2=TheDb->OpenTableLC(TPtrC(MODEM));
	TUint32 dataModemId, smsModemId;

// IR mobile iPhone
	TESTL(table2->InsertRecord(dataModemId)==KErrNone);
	table2->WriteTextL(TPtrC(COMMDB_NAME),_L("Infrared Mobile Phone"));
	table2->WriteTextL(TPtrC(MODEM_PORT_NAME),_L("LOOPBACK::0"));
	table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("LOOPBACK"));
	table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("MM")); // MM TSY
	table2->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps115200);
	table2->WriteUintL(TPtrC(MODEM_DATA_BITS),(TUint32)EData8);
	table2->WriteUintL(TPtrC(MODEM_STOP_BITS),(TUint32)EStop1);
	table2->WriteUintL(TPtrC(MODEM_PARITY),(TUint32)EParityNone);
	table2->WriteUintL(TPtrC(MODEM_HANDSHAKING),(TUint32)0);
	table2->WriteUintL(TPtrC(MODEM_SPECIAL_RATE),(TUint32)0);
	table2->WriteUintL(TPtrC(MODEM_XON_CHAR),(TUint32)17);
	table2->WriteUintL(TPtrC(MODEM_XOFF_CHAR),(TUint32)19);
	table2->WriteTextL(TPtrC(MODEM_MODEM_INIT_STRING),_L8("ATZ"));
	table2->WriteTextL(TPtrC(MODEM_DATA_INIT_STRING),_L8("AT345678901234567890123456789012345678901234567890"));
	table2->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING),KNullDesC8);
	table2->WriteUintL(TPtrC(MODEM_FAX_CLASS_PREF),(TUint32)EClassAuto);
	table2->WriteUintL(TPtrC(MODEM_SPEAKER_PREF),(TUint32)EModemSpeakerSettingNever);
	table2->WriteUintL(TPtrC(MODEM_SPEAKER_VOL_PREF),(TUint32)EModemSpeakerVolumeQuiet);
	table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),_L("+44 123456789"));
	table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),1440ul);
	table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),EFalse);
	TESTL(table2->PutRecordChanges()==KErrNone);
	CleanupStack::PopAndDestroy();

// Cabled 9110, etc
	table2=TheDb->OpenTableLC(TPtrC(MODEM));
	TESTL(table2->InsertRecord(smsModemId)==KErrNone);
	table2->WriteTextL(TPtrC(COMMDB_NAME),_L("GSM Mobile Phone via Serial"));
	table2->WriteTextL(TPtrC(MODEM_PORT_NAME),_L("LOOPBACK::0"));
	table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("LOOPBACK"));
	table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("PHONETSY")); // Phone TSY - has to be different to GSMBSC
	table2->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps19200);
	table2->WriteUintL(TPtrC(MODEM_DATA_BITS),(TUint32)EData8);
	table2->WriteUintL(TPtrC(MODEM_STOP_BITS),(TUint32)EStop1);
	table2->WriteUintL(TPtrC(MODEM_PARITY),(TUint32)EParityNone);
	table2->WriteUintL(TPtrC(MODEM_HANDSHAKING),(TUint32)KConfigObeyCTS|KConfigObeyDCD|KConfigFailDCD);
	table2->WriteUintL(TPtrC(MODEM_SPECIAL_RATE),(TUint32)0);
	table2->WriteUintL(TPtrC(MODEM_XON_CHAR),(TUint32)17);
	table2->WriteUintL(TPtrC(MODEM_XOFF_CHAR),(TUint32)19);
	table2->WriteTextL(TPtrC(MODEM_MODEM_INIT_STRING),_L8("ATZ"));
	table2->WriteTextL(TPtrC(MODEM_DATA_INIT_STRING),KNullDesC8);
	table2->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING),KNullDesC8);
	table2->WriteTextL(TPtrC(MODEM_ISP_INIT_STRING),KNullDesC8);
	table2->WriteUintL(TPtrC(MODEM_FAX_CLASS_PREF),(TUint32)EClassAuto);
	table2->WriteUintL(TPtrC(MODEM_SPEAKER_PREF),(TUint32)EModemSpeakerSettingNever);
	table2->WriteUintL(TPtrC(MODEM_SPEAKER_VOL_PREF),(TUint32)EModemSpeakerVolumeQuiet);
	table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),_L("+441632960000"));
	table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),1440ul);
	table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),EFalse);
	TESTL(table2->PutRecordChanges()==KErrNone);
	CleanupStack::PopAndDestroy();

// Setting default modem values for both fax-data and sms modems
	TUint32 defaultModemDataId = 0ul;
	TUint32 defaultModemSMSId = 0ul;

// Set default modem to be used for fax-data
	defaultModemDataId = dataModemId;

	TheDb->SetGlobalSettingL(TPtrC(MODEM_DATA_FAX),defaultModemDataId);
	table2 = TheDb->OpenViewMatchingUintLC(TPtrC(MODEM),TPtrC(COMMDB_ID),defaultModemDataId);
	TESTL(table2->GotoFirstRecord()==KErrNone);
	TESTL(table2->UpdateRecord()==KErrNone);
	TESTL(table2->PutRecordChanges()==KErrNone);
	CleanupStack::PopAndDestroy();

// Set default modem for sms service
	defaultModemSMSId = smsModemId;

	TheDb->SetGlobalSettingL(TPtrC(MODEM_PHONE_SERVICES_SMS),defaultModemSMSId);
	table2 = TheDb->OpenViewMatchingUintLC(TPtrC(MODEM),TPtrC(COMMDB_ID),defaultModemSMSId);
	TESTL(table2->GotoFirstRecord()==KErrNone);
	TESTL(table2->UpdateRecord()==KErrNone);
	TESTL(table2->PutRecordChanges()==KErrNone);
	CleanupStack::PopAndDestroy();
	}

void PopulateModemTemplateL()
//
// Populate the Modem Template table
//
/**
 * This method populates the Modem template table in the Communications Database.
 *
 * @leave	This method leaves if operations fail on the database.
 * @return	None
 *
 */
	{
	CCommsDbTemplateRecord* temp1=CCommsDbTemplateRecord::NewL(TheDb,TPtrC(MODEM));
	CleanupStack::PushL(temp1);
	temp1->Modify();

	temp1->WriteTextL(TPtrC(COMMDB_NAME),_L("Default Modem"));
	temp1->WriteTextL(TPtrC(MODEM_PORT_NAME),_L("COMM::0"));
	temp1->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("ECUART"));
	temp1->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("HAYES"));
	temp1->WriteUintL(TPtrC(MODEM_DATA_BITS),(TUint32)EData8);
	temp1->WriteUintL(TPtrC(MODEM_STOP_BITS),(TUint32)EStop1);
	temp1->WriteUintL(TPtrC(MODEM_PARITY),(TUint32)EParityNone);
	temp1->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps115200);
	temp1->WriteUintL(TPtrC(MODEM_HANDSHAKING),(TUint32)0);
	temp1->WriteUintL(TPtrC(MODEM_SPECIAL_RATE),(TUint32)0);
	temp1->WriteUintL(TPtrC(MODEM_XON_CHAR),(TUint32)0);
	temp1->WriteUintL(TPtrC(MODEM_XOFF_CHAR),(TUint32)0);
	//
	temp1->WriteTextL(TPtrC(MODEM_MODEM_INIT_STRING),_L8("AT"));
	temp1->WriteTextL(TPtrC(MODEM_DATA_INIT_STRING),_L8("AT"));
	temp1->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING),_L8("AT"));
	temp1->WriteTextL(TPtrC(MODEM_SPEAKER_VOL_CONTROL_LOW),_L8("L0"));
	temp1->WriteTextL(TPtrC(MODEM_SPEAKER_VOL_CONTROL_MEDIUM),_L8("L1"));
	temp1->WriteTextL(TPtrC(MODEM_SPEAKER_VOL_CONTROL_HIGH),_L8("L2"));
	temp1->WriteTextL(TPtrC(MODEM_SPEAKER_ALWAYS_OFF),_L8("M0"));
	temp1->WriteTextL(TPtrC(MODEM_SPEAKER_ON_UNTIL_CARRIER),_L8("M1"));
	temp1->WriteTextL(TPtrC(MODEM_SPEAKER_ALWAYS_ON),_L8("M2"));
	temp1->WriteTextL(TPtrC(MODEM_SPEAKER_ON_AFTER_DIAL_UNTIL_CARRIER),_L8("M3"));
	temp1->WriteTextL(TPtrC(MODEM_DIAL_TONE_WAIT_MODIFIER),_L8("W"));
	temp1->WriteTextL(TPtrC(MODEM_DIAL_PAUSE_LENGTH),_L8("S8="));
	temp1->WriteTextL(TPtrC(MODEM_CALL_PROGRESS_1),_L8("X1"));
	temp1->WriteTextL(TPtrC(MODEM_CALL_PROGRESS_2),_L8("X2"));
	temp1->WriteTextL(TPtrC(MODEM_CALL_PROGRESS_3),_L8("X3"));
	temp1->WriteTextL(TPtrC(MODEM_CALL_PROGRESS_4),_L8("X4"));
	temp1->WriteTextL(TPtrC(MODEM_ECHO_OFF),_L8("E0"));
	temp1->WriteTextL(TPtrC(MODEM_VERBOSE_TEXT),_L8("V1"));
	temp1->WriteTextL(TPtrC(MODEM_QUIET_ON),_L8("Q1"));
	temp1->WriteTextL(TPtrC(MODEM_QUIET_OFF),_L8("Q0"));
	temp1->WriteTextL(TPtrC(MODEM_DIAL_COMMAND_STATE_MODIFIER),_L8(";"));
	temp1->WriteTextL(TPtrC(MODEM_ON_LINE),_L8("O"));
	temp1->WriteTextL(TPtrC(MODEM_RESET_CONFIGURATION),_L8("Z"));
	temp1->WriteTextL(TPtrC(MODEM_RETURN_TO_FACTORY_DEFS),_L8("&F"));
	temp1->WriteTextL(TPtrC(MODEM_DCD_ON_DURING_LINK),_L8("&C1"));
	temp1->WriteTextL(TPtrC(MODEM_DTR_HANG_UP),_L8("&D2"));
	temp1->WriteTextL(TPtrC(MODEM_DSR_ALWAYS_ON),_L8("&S0"));
	temp1->WriteTextL(TPtrC(MODEM_RTS_CTS_HANDSHAKE),_L8("&K3"));
	temp1->WriteTextL(TPtrC(MODEM_XON_XOFF_HANDSHAKE),_L8("&K4"));
	temp1->WriteTextL(TPtrC(MODEM_ESCAPE_CHARACTER),_L8("+"));
	temp1->WriteTextL(TPtrC(MODEM_ESCAPE_GUARD_PERIOD),_L8("S12"));
	//
	temp1->WriteTextL(TPtrC(MODEM_NO_DIAL_TONE),_L8("NO DIAL TONE"));
	temp1->WriteTextL(TPtrC(MODEM_BUSY),_L8("BUSY"));
	temp1->WriteTextL(TPtrC(MODEM_NO_ANSWER),_L8("NO ANSWER"));
	temp1->WriteTextL(TPtrC(MODEM_CARRIER),_L8("CARRIER"));
	temp1->WriteTextL(TPtrC(MODEM_CONNECT),_L8("CONNECT"));
	temp1->WriteTextL(TPtrC(MODEM_COMPRESSION_CLASS_5),_L8("COMPRESSION:CLASS 5"));
	temp1->WriteTextL(TPtrC(MODEM_COMPRESSION_V42BIS),_L8("COMPRESSION:V.42 bis"));
	temp1->WriteTextL(TPtrC(MODEM_COMPRESSION_NONE),_L8("COMPRESSION:NONE"));
	temp1->WriteTextL(TPtrC(MODEM_PROTOCOL_LAPD),_L8("PROTOCOL:LAPD"));
	temp1->WriteTextL(TPtrC(MODEM_PROTOCOL_ALT),_L8("PROTOCOL:ALT"));
	temp1->WriteTextL(TPtrC(MODEM_PROTOCOL_ALTCELLULAR),_L8("PROTOCOL:ALT-CELLULAR"));
	temp1->WriteTextL(TPtrC(MODEM_PROTOCOL_NONE),_L8("PROTOCOL:NONE"));
	//
	temp1->WriteUintL(TPtrC(MODEM_FAX_CLASS_PREF),(TUint32)EClassAuto);
	temp1->WriteUintL(TPtrC(MODEM_SPEAKER_PREF),(TUint32)EModemSpeakerSettingAfterDialUntilAnswer);
	temp1->WriteUintL(TPtrC(MODEM_SPEAKER_VOL_PREF),(TUint32)EModemSpeakerVolumeQuiet);

	User::LeaveIfError(temp1->StoreModifications());
	CleanupStack::PopAndDestroy();

// Added location defaults for the modem table
	CCommsDbTemplateRecord* temp2=CCommsDbTemplateRecord::NewL(TheDb,TPtrC(LOCATION));
	CleanupStack::PushL(temp2);
	temp2->Modify();

	temp2->WriteTextL(TPtrC(COMMDB_NAME),_L("Default Location"));
	temp2->WriteTextL(TPtrC(LOCATION_INTL_PREFIX_CODE),_L("+"));
	temp2->WriteTextL(TPtrC(LOCATION_NAT_PREFIX_CODE),_L("0"));
	temp2->WriteTextL(TPtrC(LOCATION_NAT_CODE),_L("44"));
	temp2->WriteTextL(TPtrC(LOCATION_AREA_CODE),KNullDesC);
	temp2->WriteBoolL(TPtrC(LOCATION_MOBILE),ETrue);
	temp2->WriteBoolL(TPtrC(LOCATION_USE_PULSE_DIAL),EFalse);
	temp2->WriteBoolL(TPtrC(LOCATION_WAIT_FOR_DIAL_TONE),EFalse);
	temp2->WriteUintL(TPtrC(LOCATION_PAUSE_AFTER_DIAL_OUT),(TUint32)0);

	TESTL(temp2->StoreModifications()==KErrNone);
	CleanupStack::PopAndDestroy();
	}

void CreateLocationTablesL()
/**
 * This method creates a modem table entry in the communications database.
 * This table contains dummy information.  It is essential to have a location table for
 * the correct running of the GSM TSY, but the contents are unimportant.
 *
 * @leave	This method leaves if operations on the database fail.
 * @return	A location table id.
 */
{
	CCommsDbTableView* table4;
	TUint32 id1, id2;

// Office Location
	table4 = TheDb->OpenTableLC(TPtrC(LOCATION));
	TESTL(table4->InsertRecord(id1)==KErrNone);
	table4->WriteTextL(TPtrC(COMMDB_NAME),_L("Office"));
	table4->WriteTextL(TPtrC(LOCATION_INTL_PREFIX_CODE),_L("00"));
	table4->WriteTextL(TPtrC(LOCATION_NAT_PREFIX_CODE),_L("0"));
	table4->WriteTextL(TPtrC(LOCATION_NAT_CODE),_L("44"));
	table4->WriteTextL(TPtrC(LOCATION_AREA_CODE),_L("171"));
	table4->WriteTextL(TPtrC(LOCATION_DIAL_OUT_CODE),_L("9,"));
	table4->WriteTextL(TPtrC(LOCATION_DISABLE_CALL_WAITING_CODE),KNullDesC);
	table4->WriteBoolL(TPtrC(LOCATION_MOBILE),EFalse);
	table4->WriteBoolL(TPtrC(LOCATION_USE_PULSE_DIAL),EFalse);
	table4->WriteBoolL(TPtrC(LOCATION_WAIT_FOR_DIAL_TONE),EFalse);
	table4->WriteUintL(TPtrC(LOCATION_PAUSE_AFTER_DIAL_OUT),(TUint32)0);
	TESTL(table4->PutRecordChanges()==KErrNone);
	CleanupStack::PopAndDestroy();
	table4 = NULL;

// Office (without dial out code) Location
	table4 = TheDb->OpenTableLC(TPtrC(LOCATION));
	TESTL(table4->InsertRecord(id2)==KErrNone);
	table4->WriteTextL(TPtrC(COMMDB_NAME),_L("Office Direct Dial"));
	table4->WriteTextL(TPtrC(LOCATION_INTL_PREFIX_CODE),_L("00"));
	table4->WriteTextL(TPtrC(LOCATION_NAT_PREFIX_CODE),_L("0"));
	table4->WriteTextL(TPtrC(LOCATION_NAT_CODE),_L("44"));
	table4->WriteTextL(TPtrC(LOCATION_AREA_CODE),_L("171"));
	table4->WriteTextL(TPtrC(LOCATION_DIAL_OUT_CODE),KNullDesC);
	table4->WriteTextL(TPtrC(LOCATION_DISABLE_CALL_WAITING_CODE),KNullDesC);
	table4->WriteBoolL(TPtrC(LOCATION_MOBILE),EFalse);
	table4->WriteBoolL(TPtrC(LOCATION_USE_PULSE_DIAL),EFalse);
	table4->WriteBoolL(TPtrC(LOCATION_WAIT_FOR_DIAL_TONE),EFalse);
	table4->WriteUintL(TPtrC(LOCATION_PAUSE_AFTER_DIAL_OUT),(TUint32)0);
	TESTL(table4->PutRecordChanges()==KErrNone);
	CleanupStack::PopAndDestroy();
	table4 = NULL;

// Set default location from which you are dialling for data modem
	TheDb->SetGlobalSettingL(TPtrC(LOCATION_DATA_FAX),id1);

// Set default location from which you are dialling for modem for sms service
	TheDb->SetGlobalSettingL(TPtrC(LOCATION_PHONE_SERVICES_SMS),id2);
//	return locationId;
	}

LOCAL_C TKeyCode ShortWaitForKey(TInt aDelay, TBool& aKeyPressed)
	{
	TRequestStatus stat1,stat2;
	RTimer timer;
	aKeyPressed=EFalse;
	timer.CreateLocal();
	for(TInt i=0;i<aDelay;i++)
		{
		test.Console()->Read(stat1);
		timer.After(stat2,1000000L);
		User::WaitForRequest(stat1,stat2);
		if(stat1==KErrNone)
			{
			timer.Cancel();
			User::WaitForRequest(stat2);
			aKeyPressed=ETrue;
			break;
			}
		else
			{
			test.Console()->ReadCancel();
			User::WaitForRequest(stat1);
			INFO_PRINTF1(_L("."));
			}
		}
	INFO_PRINTF1(_L("\n"));
	return test.Console()->KeyCode();
	}

//
// Set the Default CommDb Combination
//
void CreateCommDbL()
/**
 * This method creates a special Communications Database to be used with the regression test
 * harness.  The user is given a chance to cancel this operation.  This method uses the file
 * server and timer services.  It also calls other methods to actually populate the
 * special communications database.  Note that the communications database created is tailored
 * to the needs of the ETel Regression Test harness.
 *
 * @leave	This method leaves if the user cancels the overwrite of the communications
 *			database or if the NewL method called in this method fails.  Also, the database
 *			functions can also leave.
 */
	{
	RFs fs;
	_LIT(KSvDbFileName,"C:\\System\\Data\\Cdbv3.dat");

	TESTL(fs.Connect()==KErrNone);

	RTimer timer;
	timer.CreateLocal();
	TUint dummy;
	if(fs.Att(KSvDbFileName,dummy)==KErrNone)
		{
		INFO_PRINTF1(_L("\nThis test requires a special CommDb Database.\nExisting Database File detected\nPress any key to STOP Overwrite\n\n"));

		TBool keyPressed=ETrue;
		ShortWaitForKey(5,keyPressed);
		if(keyPressed)
		User::Leave(KErrAlreadyExists);

		INFO_PRINTF1(_L("\n"));
		TESTL(fs.Delete(KSvDbFileName)==KErrNone);
		}
	fs.Close();

	TheDb=CCommsDatabase::NewL(EDatabaseTypeIAP);
	CleanupStack::PushL(TheDb);
	TESTL(TheDb->BeginTransaction()==KErrNone);

//
// Write the database tables
//
	TUint32 ispId=CreateISPTableL();
	CreateModemTablesL();
	CreateLocationTablesL();
	PopulateModemTemplateL();

	CCommsDbTableView* table=TheDb->OpenTableLC(TPtrC(IAP));

	TUint32 id;
	TESTL(table->InsertRecord(id)==KErrNone);
	table->WriteTextL(TPtrC(COMMDB_NAME),_L("Test Configuration: MM Loopback"));
	table->WriteUintL(TPtrC(IAP_SERVICE),ispId);
	table->WriteTextL(TPtrC(IAP_SERVICE_TYPE),TPtrC(DIAL_OUT_ISP));
	TESTL(table->PutRecordChanges()==KErrNone);
//
// Set the Default
//
	CCommsDbConnectionPrefTableView* cpTable=TheDb->OpenConnectionPrefTableViewOnRankLC(ECommDbConnectionDirectionOutgoing,1);
	cpTable->GotoFirstRecord();
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;
	cpTable->ReadConnectionPreferenceL(pref);
	pref.iBearer.iIapId=id;
	pref.iBearer.iBearerSet = KCommDbBearerCSD;
	cpTable->UpdateBearerL(pref.iBearer);

	CleanupStack::PopAndDestroy();
	TESTL(TheDb->CommitTransaction()==KErrNone);
	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy();
	}

TInt GetNumberEntry
(
	const TInt aMaxDigits,	// max numbers of digits
	const TInt aMin,		// min value allowed
	const TInt aMax,		// max value allowed
	const TInt aInputWait,	// (s) how long to wait for each user key entry
	const TInt aDefaultVal,	// default value if timed out
	const TDesC &aPrompt	// string prompt
)
/**
 * This method gets numeric user entry from the console. It checks that the key entered
 * is between 0 to 9 inclusive. This method exits by user hitting the
 * Enter key or timing out.
 *
 * @param aMaxNumbers integer for max numbers of digits
 * @param aMin integer for min value allowed
 * @param aMax integer for max value allowed
 * @param aInputWait integer for number of seconds to wait for each user key entry
 * @param aDefaultVal integer for default value if time-out occurs without
 * @param aPrompt string prompt
 * @return KErrNone.
 */
{
TInt keyFolded;				 // actual digit entered, not TKeyCode
TRequestStatus stat1,stat2;
RTimer timer;
TBool bCorrectEntry = EFalse;
TInt userNum = -1;
TInt limit;
TKeyCode key;

	TTimeIntervalMicroSeconds32 anInterval = aInputWait * 1000000;
	CConsoleBase *pConsole = test.Console();
	timer.CreateLocal();

	while (!bCorrectEntry)
		{
		userNum = -1;
		limit = aMaxDigits;
		key = EKeyNull;

		INFO_PRINTF1(aPrompt); // print prompt
		INFO_PRINTF1(_L(" (range %d-%d) or <CR> for default of %d: "), aMin, aMax, aDefaultVal);
		// exits loop when Enter keyed or limit reached (by decrement to 0)
		while ((key != EKeyEnter) && limit)
			{
			pConsole->Read(stat1);				// set read
			timer.After(stat2, anInterval);		// set wait for this period
			User::WaitForRequest(stat1,stat2);  // whatever comes first

			if(stat1 == KErrNone)					// user entered key
				{
				timer.Cancel();
				User::WaitForRequest(stat2);

				key = pConsole->KeyCode();
				if((key >= '0') && (key <= '9'))
					{
					// valid digit
					keyFolded = (TKeyCode)(key - '0');  // convert to digit
					INFO_PRINTF1(_L("%d"), keyFolded);	// echo
					limit--;			// tracks number of digits

					// "append" to number
					if (-1 == userNum)	   // ie first char entered
						{
						userNum = keyFolded;
						}
					else					//  next char entered
						{
						userNum = userNum * 10 + keyFolded;  // shift
						}
					}
				}
			else	// timer went off, use default unless valid key entered before timer expired
				{
				pConsole->ReadCancel();
				User::WaitForRequest(stat1);
				if (-1 == userNum)
					{
					// no value entered before timer went off
					userNum = aDefaultVal;
					}
				break;
				}
			} // endwhile

		test.Printf (_L("\n"));
		if ((userNum >= aMin) && (userNum <= aMax))
			{
			bCorrectEntry = ETrue;  // exit loop
			}
		else						// return to loop
			{
			if (userNum == -1)
				{
				// <CR> was entered before any numbers, so use default
				userNum = aDefaultVal;
				break;
				}
			INFO_PRINTF1(_L("Try again, Entry out of limit. Must be between %d and %d inclusive.\n\n"), aMin, aMax);
			}
		} // endwhile

	return (userNum);
}

LOCAL_C void CreateTestReportFileL()
	{
	TInt ret=TheFs.Connect();
	User::LeaveIfError(ret);
	TheFs.Delete(KTestReportFile);

	ret=TestRpt.Create(TheFs,KTestReportFile,EFileWrite|EFileShareAny);
	User::LeaveIfError(ret);
	}

LOCAL_C void CloseTestReportFile()
	{
	TestRpt.Close();
	TheFs.Close();
	}

LOCAL_C void TestReportLog(const TDesC8& aText)
	{
	TBuf8<200> buf;

	TTime now;
	now.UniversalTime();
	TDateTime dateTime;
	dateTime = now.DateTime();
	buf.Format(_L8 ("%02d.%02d:%02d:%06d "),dateTime.Hour(),dateTime.Minute(),dateTime.Second(),dateTime.MicroSecond());
	buf.AppendFormat(_L8("%S\015\012"),&aText);
	TestRpt.Write(buf);
	TestRpt.Flush();
	}

LOCAL_C void StartTest(const TDesC8& aTestName)
	{
	TBuf8<200> buf8;
	buf8.AppendFormat(_L8("Starting %S Test"),&aTestName);
	TestReportLog(buf8);
	TBuf<200> buf;
	buf.Copy(aTestName);
	INFO_PRINTF1(_L("\nStarting %S Test\n"),&buf);
	}

LOCAL_C void EndTest(const TDesC8& aTestName)
	{
	TBuf8<200> buf8;
	buf8.AppendFormat(_L8("%S Test Completed Successfully"),&aTestName);
	TestReportLog(buf8);
	TBuf<200> buf;
	buf.Copy(aTestName);
	INFO_PRINTF1(_L("Completed %S Test\n"),&buf);
	}

//
// Perform the Test
//
LOCAL_C void PerformTestL()
/**
 *
 * The PerformTestL method is the main user interface routine for the GSM TSY Regression
 * Test Harness.  It loads the appropriate serial drivers and unloads them at completion.
 * This method presents the user with the list of tests to run and then
 * responds to the user's input.  If the user does not select any specific test, it runs all
 * tests as the default.
 *
 * @leave	This method leaves if it can not connect to the communications server or load
 *			the CSY.  Additionally, this method leaves if the test script can not be created.
 *			If the test fails with an error, User::Leave will be invoked.
 */
	{
	RCommServ commServer;
	TESTL(commServer.Connect()==KErrNone);
	TInt res=commServer.LoadCommModule(_L("ECUART.CSY"));
	INFO_PRINTF1(TRefByValue<const TDesC>(_L("LoadCommModuleError = %d\n")), res);
	TESTL(res==KErrNone || res==KErrAlreadyExists);

//  This block of prints is for all tests to be listed in one column
//  Note: by displaying menu selectons in a single column, all selections will not be visible
	INFO_PRINTF1(_L("Which test?  Leave to run all tests\n"));
	INFO_PRINTF1(_L("a) Receive SMS Test\n"));
	INFO_PRINTF1(_L("b) SMS Notification Test\n"));
	INFO_PRINTF1(_L("c) SMS PDU Capability Test\n"));
	INFO_PRINTF1(_L("d) Default SCA Test\n"));
	INFO_PRINTF1(_L("e) SMS Transmission Test\n"));
	INFO_PRINTF1(_L("f) Data Call Test\n"));
	INFO_PRINTF1(_L("g) Errored Data Call Test\n"));
	INFO_PRINTF1(_L("h) Odd Initialisation Tests\n"));
	INFO_PRINTF1(_L("i) SMS Storage Tests\n"));
	INFO_PRINTF1(_L("j) Failure Initialisation Tests\n"));
	INFO_PRINTF1(_L("k) Incoming Call Tests\n"));
	INFO_PRINTF1(_L("l) SMS Cancel Scenarios\n"));
	INFO_PRINTF1(_L("m) Shutdown Scenarios\n"));
	INFO_PRINTF1(_L("n) SMS Storage Delete Test\n"));
	INFO_PRINTF1(_L("o) Simultaneous Fax Reception and Signal Strength Retrieval Scenario\n"));
	INFO_PRINTF1(_L("p) Data Call Cancel Scenarios\n"));
	INFO_PRINTF1(_L("q) Fax Call Premature Close Scenario\n"));
	INFO_PRINTF1(_L("r) Two Fax Reception Scenario\n"));
	INFO_PRINTF1(_L("s) Voice Call Scenarios\n"));
	INFO_PRINTF1(_L("t) Data Call Set-up, Data Transfer and Remote Termination Test\n"));
	INFO_PRINTF1(_L("u) Data Call Dial-up Networking Call-back Test\n"));
	INFO_PRINTF1(_L("v) Data Call Answer and Remote Hang-up Closely Followed by a Dial Test\n"));
	INFO_PRINTF1(_L("w) Phonebook tests\n"));
	INFO_PRINTF1(_L("x) Network and Registration tests\n"));
	INFO_PRINTF1(_L("y) Phone and Line tests\n"));
	INFO_PRINTF1(_L("z) Voice and Data calls tests\n"));
	INFO_PRINTF1(_L("1) No CNMI info and Modem Detection tests\n"));
	INFO_PRINTF1(_L("2) No CMGF info tests\n"));
	INFO_PRINTF1(_L("3) AT+CBST  tests\n"));
	INFO_PRINTF1(_L("4) CBST & CNMI string parsing test\n")) ;
	INFO_PRINTF1(_L("5) CGQREQ responses during initialise\n"));
	INFO_PRINTF1(_L("6) Unsolicited messages injected during initialisation\n"));
	INFO_PRINTF1(_L("9) Voice Call OOM tests (NOT RUN AS PART OF 'ALL TESTS')\n"));

	TBool keyPressed=ETrue;
	TKeyCode key=ShortWaitForKey(10, keyPressed);

	TBool allTests=!keyPressed;
	if(keyPressed)
		{
		if((key>='A')&&(key<='Z'))	// A simple fold
			key=(TKeyCode)(key-'A'+'a');
		}

//
// Run the tests...
//


// Test Rx SMS messages
	if((key=='a')||(allTests))
		{
		SIMPLETESTMACRO("Simple SMS Receive",CTestDriveRxMess,ERxMessScript);
		SIMPLETESTMACRO("CMT SMS Receive",CTestDriveRxMessCmt,ERxMessCmtAndCmtiScript);
		SIMPLETESTMACRO("CMTI SMS Receive emulating an Ericsson T28",CTestDriveRxMess,ERxMessEricssonT28Script);
		}

// Test Notification of SMS messages
	if((key=='b')||(allTests))
		{
		SIMPLETESTMACRO("SMS CMTI Notification and Receive",CTestDriveNotMess,ERxMessScript);
		SIMPLETESTMACRO("SMS CMT Notification and Receive",CTestDriveNotMessCmt,ERxMessCmtScript);
		}

// Test behaviour when a modem claims not to support SMS PDU mode
	if((key=='c')||(allTests))
		{
		SIMPLETESTMACRO("No SMS PDU capability",CTestDriveNoPduRxMess,ENoPduRxMessScript);
		}


// Test retrieval and setting of default service centre

	if ((key=='d')||(allTests))
		{
		SIMPLETESTMACRO("Simple SCA retrieval & setting",CTestDriveSca,EScaSimpleScript);
		SIMPLETESTMACRO("8210-style SCA retrieval & setting",CTestDriveSca,ESca8210StyleScript);
		}


// Test a simple SMS Tx
	if((key=='e')||(allTests))
		{
		SIMPLETESTMACRO("A simple SMS Tx",CTestDriveTxMess,ETxMessScript);
		SIMPLETESTMACRO("New Standard SMS Tx",CTestDriveTxNewStdMess,ETxMessNewStdScript);
		SIMPLETESTMACRO("New Standard SMS Tx Test emulating an Ericsson T28",CTestDriveTxNewStdMess,ETxMessT28NewStdScript);

	// Test no prefix on pdu, don't set default sca, new ETSI format
		_LIT8(KTxA,"Tx: no PDU prefix, no default SCA set, new standard");
		StartTest(KTxA);
		CTestDriveTxWithScaCombo* test4c=CTestDriveTxWithScaCombo::NewL(ETxMessScriptNoprefixNodefscaNew,EFalse,EFalse,ETrue);
		CleanupStack::PushL(test4c);
		User::LeaveIfError(test4c->RunTestL());
		CleanupStack::PopAndDestroy();
		EndTest(KTxA);
		User::After(1000000L);

	// Test prefix on pdu, don't set default sca, new ETSI format
		_LIT8(KTxB,"Tx: PDU prefix, no default SCA set, new standard");
		StartTest(KTxB);
		test4c=CTestDriveTxWithScaCombo::NewL(ETxMessScriptPrefixNodefscaNew,ETrue,EFalse,ETrue);
		CleanupStack::PushL(test4c);
		User::LeaveIfError(test4c->RunTestL());
		CleanupStack::PopAndDestroy();
		EndTest(KTxB);
		User::After(1000000L);

	// Test no prefix on pdu, set default sca, new ETSI format
		_LIT8(KTxC,"Tx: no PDU prefix, default SCA set, new standard");
		StartTest(KTxC);
		test4c=CTestDriveTxWithScaCombo::NewL(ETxMessScriptNoprefixDefscaNew,EFalse,ETrue,ETrue);
		CleanupStack::PushL(test4c);
		User::LeaveIfError(test4c->RunTestL());
		CleanupStack::PopAndDestroy();
		EndTest(KTxC);
		User::After(1000000L);

	// Test prefix on pdu, set default sca, new ETSI format
		_LIT8(KTxD,"Tx: PDU prefix, default SCA set, new standard");
		StartTest(KTxD);
		test4c=CTestDriveTxWithScaCombo::NewL(ETxMessScriptPrefixDefscaNew,ETrue,ETrue,ETrue);
		CleanupStack::PushL(test4c);
		User::LeaveIfError(test4c->RunTestL());
		CleanupStack::PopAndDestroy();
		EndTest(KTxD);
		User::After(1000000L);

	// Test no prefix on pdu, don't set default sca, old ETSI format
		_LIT8(KTxE,"Tx: No PDU prefix, no default SCA set, old standard");
		StartTest(KTxE);
		test4c=CTestDriveTxWithScaCombo::NewL(ETxMessScriptNoprefixNodefscaOld,EFalse,EFalse,EFalse);
		CleanupStack::PushL(test4c);
		User::LeaveIfError(test4c->RunTestL());
		CleanupStack::PopAndDestroy();
		EndTest(KTxE);
		User::After(1000000L);

	// Test prefix on pdu, don't set default sca, old ETSI format
		_LIT8(KTxF,"Tx: PDU prefix, no default SCA set, old standard");
		StartTest(KTxF);
		test4c=CTestDriveTxWithScaCombo::NewL(ETxMessScriptPrefixNodefscaOld,ETrue,EFalse,EFalse);
		CleanupStack::PushL(test4c);
		User::LeaveIfError(test4c->RunTestL());
		CleanupStack::PopAndDestroy();
		EndTest(KTxF);
		User::After(1000000L);

	// Test no prefix on pdu, set default sca, old ETSI format
		_LIT8(KTxG,"Tx: no PDU prefix, default SCA set, old standard");
		StartTest(KTxG);
		test4c=CTestDriveTxWithScaCombo::NewL(ETxMessScriptNoprefixDefscaOld,EFalse,ETrue,EFalse);
		CleanupStack::PushL(test4c);
		User::LeaveIfError(test4c->RunTestL());
		CleanupStack::PopAndDestroy();
		EndTest(KTxG);
		User::After(1000000L);

	// Test prefix on pdu, set default sca, old ETSI format
		_LIT8(KTxH,"Tx: PDU prefix, default SCA set, old standard");
		StartTest(KTxH);
		test4c=CTestDriveTxWithScaCombo::NewL(ETxMessScriptPrefixDefscaOld,ETrue,ETrue,EFalse);
		CleanupStack::PushL(test4c);
		User::LeaveIfError(test4c->RunTestL());
		CleanupStack::PopAndDestroy();
		EndTest(KTxH);
		User::After(1000000L);

	// Test a "+CMS ERROR 321" from a Read (observed on an Ericsson 888)
		SIMPLETESTMACRO("Invalid Read Test",CTestDriveTxRx,ETxMessRxTxScriptA);
		}

// Test Dialing a Data Call
	if((key=='f')||(allTests))
		{
		SIMPLETESTMACRO("Dialing a simple Data Call",CTestDriveDataCall,EDataCallScript);
		}

// Test an Error Dialing a Data Call
	if((key=='g')||(allTests))
		{
		SIMPLETESTMACRO("Dialing a simple Data Call, with the modem ERRORing some commands",CTestDriveDataCall,EDataCallErrorAScript);
		}

// Test some odd initialisation sequences
	if((key=='h')||(allTests))
		{
		SIMPLETESTMACRO("Odd Initialisation Sequence A",CTestDriveOddInit,EOddInitAScript);
		SIMPLETESTMACRO("Odd Initialisation Sequence B",CTestDriveOddInit,EOddInitBScript);
		SIMPLETESTMACRO("Odd Initialisation Sequence C",CTestDriveOddInit,EOddInitCScript);
		SIMPLETESTMACRO("Odd Initialisation Sequence D",CTestDriveOddInit,EOddInitDScript);
		SIMPLETESTMACRO("Odd Initialisation Sequence E",CTestDriveOddInit,EOddInitEScript);
		SIMPLETESTMACRO("Odd Initialisation Sequence F",CTestDriveOddInit,EOddInitFScript);
		SIMPLETESTMACRO("Odd Initialisation Sequence G",CTestDriveOddInit,EOddInitGScript);
		SIMPLETESTMACRO("Odd Initialisation Sequence H",CTestDriveOddInit,EOddInitHScript);
 		SIMPLETESTMACRO("Odd Initialisation Sequence I",CTestDriveOddInit,EOddInitIScript);
		}

// Test SMS Storage functions
	if((key=='i')||(allTests))
		{
		SIMPLETESTMACRO("Message Storage Functions with old standard PDUs",CTestDriveMessStor,EMessStorOldSmsStdScript);
		SIMPLETESTMACRO("Message Storage Functions with new standard PDUs",CTestDriveMessStor,EMessStorNewSmsStdScript);
		}

// Test Initialisation Failure Scenarios
	if((key=='j')||(allTests))
		{
		SIMPLETESTMACRO("Initialisation Failure Scenarios",CTestDriveFailInit,EFailInitAScript);
		}

// Test Incoming Call Scenarios
	if((key=='k')||(allTests))
		{
		SIMPLETESTMACRO("Incoming Call Scenarios",CTestDriveInCall,EInCallScriptA);
		SIMPLETESTMACRO("Incoming Call Scenarios with a Nokia",CTestDriveInCall,EInCallScriptB);
		}

// Test SMS Cancel
	if((key=='l')||(allTests))
		{
		SIMPLETESTMACRO("SMS Cancel CMT Scenarios",CTestDriveSmsCancel,ESmsCancelScript);
		SIMPLETESTMACRO("SMS Cancel CMTI Scenarios",CTestDriveSmsCancel,ESmsCancelCmtiScript);
		}

// Test Shutdown Scenarios
	if((key=='m')||(allTests))
		{
 		SIMPLETESTMACRO("Shutdown Scenarios",CTestDriveShutdown,EShutdownScript);
		SIMPLETESTMACRO("Shutdown Scenarios A",CTestDriveShutdownA,EShutdownScriptA);
		}

// Test SMS Delete
	if((key=='n')||(allTests))
		{
		SIMPLETESTMACRO("A Simple SMS Storage Delete",CTestDriveSmsDelete,ESmsStorageDeleteScript);
		}

// Test simultaneous Fax Reception and Signal Strength Retrieval
	if((key=='o')||(allTests))
		{
		SIMPLETESTMACRO("Simultaneous Fax Reception and Signal Strength Retrieval",CTestDriveSsFax,ESsFaxScriptA);
		SIMPLETESTMACRO("Simultaneous Data Reception and Signal Strength Retrieval",CTestDriveSSData,ESsDataScriptA);
		}

// Test Data Call Cancel Scenarios
	if((key=='p')||(allTests))
		{
		SIMPLETESTMACRO("Data Call Cancel Scenarios",CTestDriveDataCallCancel,EDataCallCancelScript);
		}

// Fax Call Premature Close Scenarios
	if((key=='q')||(allTests))
		{
		SIMPLETESTMACRO("Fax Call Premature Close Scenarios",CTestDrivePremClose,EFaxPremCloseScriptA);
//		SIMPLETESTMACRO("Fax Call Premature Close Scenarios Part B",CTestDrivePremCloseB,EFaxPremCloseScriptB);
		}

// Two Fax Reception Scenarios
	if((key=='r')||(allTests))
		{
		SIMPLETESTMACRO("Two Fax Reception Scenarios",CTestDriveTwoFaxRx,ETwoFaxRxScriptA);
		}

// Voice Call Scenarios
	if((key=='s')||(allTests))
		{
		SIMPLETESTMACRO("Voice Call Scenarios",CTestDriveVoiceCall,EVoiceCallScriptA);
		}

// Test Dialing a Set-up, Data Transfer and Remote Termination Data Call
	if((key=='t')||(allTests))
		{
		SIMPLETESTMACRO("Dialing a Set-up, Data Transfer and Remote Termination Data Call",CTestDriveDataCallRemoteTerm,EDataCallRemoteTermScript);
		}

// Test Dialing a Dial-up Networking Call-back Data Call
	if((key=='u')||(allTests))
		{
		TInt varDelay;  // Variable Delay for EWait script, for scoping purposes
		varDelay = KCallBackDefVarDelay;
		if(!allTests)
			{
			// This tests is valid for supporting variable delay for an EWait script
			varDelay = GetNumberEntry(	KCallBackMaxDigits,	// max numbers of digits
										KCallBackMinVarDelay,	// min value allowed
										KCallBackMaxVarDelay,	// max value allowed
										KInputWaitOnDelaySecs,	// secs to wait for a user key entry
										KCallBackDefVarDelay,	// default value if timed out
										_L("Enter delay value")
									 );
			}
		_LIT8(KCallbackA,"Dialing a Dial-up Networking Call-back Data Call");
		StartTest(KCallbackA);
		CTestDriveDataCallCallBack* test23=CTestDriveDataCallCallBack::NewL(EDataCallCallBackScript, varDelay);
		CleanupStack::PushL(test23);
		User::LeaveIfError(test23->RunTestL());
		CleanupStack::PopAndDestroy();
		EndTest(KCallbackA);
		User::After(1000000L);
		}

// Test an Answer and Remote Hang-up Closely Followed by a Dial Data Call
	if((key=='v')||(allTests))
		{
		TInt varDelay;  // Variable Delay for EWait script, for scoping purposes
		varDelay = KHangupDialDefVarDelay;
		if(!allTests)
			{
			// This tests is valid for supporting variable delay for an EWait script
			varDelay = GetNumberEntry(	KHangupDialMaxDigits,	// max numbers of digits
										KHangupDialMinVarDelay,	// min value allowed
										KHangupDialMaxVarDelay,	// max value allowed
										KInputWaitOnDelaySecs,	// secs to wait for a user key entry
										KHangupDialDefVarDelay,	// default value if timed out
										_L("Enter delay value")
									 );
			}
		_LIT8(KCallbackB,"Answer and Remote Hang-up Closely Followed by a Dial Data Call");
		StartTest(KCallbackB);
		CTestDriveRemoteHangupDial* test24=CTestDriveRemoteHangupDial::NewL(EDataCallRemoteHangDialScript, varDelay);
		CleanupStack::PushL(test24);
		User::LeaveIfError(test24->RunTestL());
		CleanupStack::PopAndDestroy();
		EndTest(KCallbackB);
		User::After(1000000L);
		}

// Phone book tests
	if((key=='w')||(allTests))
		{
		SIMPLETESTMACRO("Phone book Scenarios",CTestDrivePhoneBook,EPhoneBookScript);
		}

// Network and Registration tests
	if((key=='x')||(allTests))
		{
		SIMPLETESTMACRO("Testing Network and Registration",CTestDriveNetworkRegistration,ENetworkRegistrationScript);
		}

// Phone and Line tests
	if((key=='y')||(allTests))
		{
		SIMPLETESTMACRO("Phone and Line Scenarios",CTestDrivePhoneLine,EPhoneLineScript);
		}

// Voice and Data Call tests
	if((key=='z')||(allTests))
		{
		SIMPLETESTMACRO("Voice and Data Calls",CTestDriveAllCalls,EAllCallsScript);
		}

// No CNMI information and Modem notification tests
	if((key=='1')||(allTests))
		{
		SIMPLETESTMACRO("No CNMI info and Modem notification request",CTestDriveNoCnmi,ENoCnmiScript);
		}

// No CMGF information tests
	if((key=='2')||(allTests))
		{
		SIMPLETESTMACRO("No CMGF info request",CTestDriveNoCmgf,ENoCmgfScript);
		}

// AT+CBST information tests
	if((key=='3')||(allTests))
		{
		SIMPLETESTMACRO("AT+CBST tests",CTestDriveDataCallDiffParam, ECallDiffParamScript);
		}
//CBST string parsing test
	if((key=='4')||(allTests))
		{
		SIMPLETESTMACRO("CBST & CNMI string parsing test",CTestDriveCbstParse, ECbstParseScript);
		}

// CGQREQ responses during initialise (added to increase conditional code coverage)
	if((key=='5')||(allTests))
		{
		SIMPLETESTMACRO("CGQREQ responses during initialise",CTestDriveCGQREQResponses,ECGQREQResponsesScript);
		}

// Unsolicited messages injected during initialisation
	if((key=='6')||(allTests))
		{
		SIMPLETESTMACRO("Unsolicited messages injected during initialisation",CTestDriveUnsolicited,EUnsolicitedScript);
		}

// OOM Voice Call tests, not run as part of 'all tests'
	if(key=='9')
		{
		SIMPLETESTMACRO("OOM Voice Call",CTestDriveOOMVoiceCall,EOOMVoiceCall);
		}

//
// Tidy up after the tests
//
	res = commServer.UnloadCommModule(_L("ECUART.CSY"));
	commServer.Close();
	}

GLDEF_C TInt E32Main()
//
// Main entry point - make sure everything is going.
//
/**
 * This method is the main entry point for the GSM TSY Regression Test Harness code.  This
 * code first makes sure the file server is included.  It also loads the comm server if
 * necessary.  After calling a method to create the necessary communications database,
 * this method invokes the user interface for the test harness.
 *
 * @return	KErrNone on Success; Errors returned from either communications database
 *			creation failure or individual tests on failures.
 *
 * @note	This method creates a new instance of the clean up stack.
 *
 */
	{
	// Make sure we're linked to the file server
	RFs s;
	s.Connect();
	s.Close();
	test.Title();

	test.Start(_L("Loading Serial drivers"));

	TInt r;

 	// When bootstrapping C32 we have to avoid the PhBkSyncServer being started, since
 	// it needs a different CommDB
 	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	r = StartC32WithCMISuppressions(KPhbkSyncCMI);
	if((r!=KErrNone)&&(r!=KErrAlreadyExists))
		INFO_PRINTF1(_L("Failed to start C32 %d\n\r"),r);

	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAP(r,CreateCommDbL());
	if(r==KErrNone)
		{
		TRAP(r,CreateTestReportFileL());
		if(r==KErrNone)
			{
			TRAP(r,PerformTestL());
			if(ReturnValue)
				r=ReturnValue;
			}
		}

	CloseTestReportFile();
	if (r)
		INFO_PRINTF1(TRefByValue<const TDesC>(_L("\nError %d reported")),r);
	else
		INFO_PRINTF1(_L("\nTest Completed Sucessfully"));

	INFO_PRINTF1(_L(", press any key to end"));
	TBool keyPressed=ETrue;
	ShortWaitForKey(10, keyPressed);
	test.End();
	delete cleanup;
//	delete testActiveScheduler;
	return KErrNone;
	}

//
// Test Scenarios to be created:
//
//	Initialise synchronously and read
//	Initialise async, read before init completion, wait for both complete
//	Init async, read SMS revealing incorrect prefered SMS memory
//
//	Currently if the data initialisation string is ERRORed then the request is errored with
//	KErrGeneral.  Should this succeed?
//
//	SMS Store
//	Test some failure scenarios - try old, fail, try new, fail, fail...
//	Test write requiring prepended SCA with no SCA set in memory - check that an error ensures
//
//	Nokia's require "AT+CRES=1" before "AT+CSCA=?" actually returns the real SCA
//
//	SMS Tx
//	New Std does not require "AT+CSCA=xxx" before transmitting SMS
//
//	SMS Rx
//	What happens if an SMS comes in after the AT+CNMI command, but before the end of the initialisation?
//	What happens if an SMS comes in after the AT+CNMI command, but before the end of a failed initialisation?
//
//	SMS Rx
//	+CDS: incoming Delivery Status Reports
//	Test +CNMI caps and results



