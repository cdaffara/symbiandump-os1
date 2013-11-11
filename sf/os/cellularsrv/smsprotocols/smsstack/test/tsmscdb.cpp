// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// sms stack commdb util functions
// 
//

/**
 @file
*/

#include <f32file.h>
#include <e32test.h>
#include <d32comm.h>
#include <faxdefn.h>
#include <cdblen.h>
#include <cdbtemp.h>
#include "tsmscdb.h"
#include "autotest.h"


_LIT(KVodafoneSC,"+447785016005"); //Vodafone


GLREF_D RTest gTest;
GLDEF_D CCommsDatabase* TheDb;

GLDEF_C TUint32 TcpProtocolId;
GLDEF_C TUint32 UdpProtocolId;
GLDEF_C TUint32 IcmpProtocolId;
GLDEF_C TUint32 IpProtocolId;
GLDEF_C TUint32 IrmuxProtocolId;
GLDEF_C TUint32 TinyTPProtocolId;
GLDEF_C TUint32 SmsProtocolId;
GLDEF_C TUint32 WapProtocolId;
GLDEF_C TUint32 PlpProtocolId;
GLDEF_C TUint32 Link1ProtocolId;
GLDEF_C TUint32 PlpLoopProtocolId;
GLDEF_C TUint32 BtLinkManagerProtocolId;
GLDEF_C TUint32 L2capProtocolId;
GLDEF_C TUint32 RfCommProtocolId;
GLDEF_C TUint32 SdpProtocolId;

LOCAL_C void CreateLocationTableL(	TMyDialLocation aLocDataFax, TMyDialLocation aLocServSMS);
LOCAL_C void CreateModemTablesL(	TMyModemType aModemDataFax,  TMyModemType aModemServSMS,
                                    TUint aCommPortDataFax,      TUint aCommPortServSMS );
LOCAL_C void CreateChargecardTableL();
LOCAL_C TUint32 CreateISPTableL();
LOCAL_C void PopulateTemplatesL();

#ifndef MODEM_PHONE_SERVICES_SMS
// TODO - #ifndef has to be removed later
LOCAL_C void CreateInterfaceSettingsTableL();
LOCAL_C void CreateProtocolBindingsTableL();
LOCAL_C void CreateProtocolFamiliesTableL();
#endif


GLDEF_C void CreateCommDBL( TMyModemType aModemDataFax,  TMyModemType aModemServSMS,
                            TMyDialLocation aLocDataFax, TMyDialLocation aLocServSMS,
							TUint aCommPortDataFax,      TUint aCommPortServSMS )
/**
 *  @test Create a special CommDb Database for this test suite.
 */
    {
    gTest.Printf(_L("Creating the Chargecard. Modem and Location Tables\n"));

	// at first delete the old comms db file
	RFs fs;
	_LIT(KSvDbFileName,"C:\\System\\Data\\Cdbv3.dat");
	CHECKPOINT(fs.Connect()==KErrNone);
	TUint dummy;
	if(fs.Att(KSvDbFileName,dummy)==KErrNone)
	    CHECKPOINT(fs.Delete(KSvDbFileName)==KErrNone);
    fs.Close();

    TheDb=CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(TheDb);
    CHECKPOINT(TheDb->BeginTransaction()==KErrNone);

#ifndef MODEM_PHONE_SERVICES_SMS
// TODO - #ifndef has to be removed later
    CreateInterfaceSettingsTableL();
    CreateProtocolBindingsTableL();
    CreateProtocolFamiliesTableL();
#endif

//
// Write the database tables
//
    TUint32 ispId=CreateISPTableL();
    CreateChargecardTableL();
    CreateLocationTableL( aLocDataFax, aLocServSMS );
    CreateModemTablesL( aModemDataFax, aModemServSMS, aCommPortDataFax, aCommPortServSMS );
    PopulateTemplatesL();

    CCommsDbTableView* table=TheDb->OpenTableLC(TPtrC(DIAL_OUT_IAP));

    TUint32 id;
    CHECKPOINT(table->InsertRecord(id)==KErrNone);
    table->WriteTextL(TPtrC(COMMDB_NAME),_L("Test Configuration: GsmBsc Loopback"));
    table->WriteUintL(TPtrC(IAP_ISP),ispId);
//  table->WriteUintL(TPtrC(IAP_MODEM),modemId);
    CHECKPOINT(table->PutRecordChanges()==KErrNone);
//
// Set the Default
//
    TheDb->SetGlobalSettingL(TPtrC(DIAL_OUT_IAP),id);
    TheDb->SetGlobalSettingL(TPtrC(REDIAL_ATTEMPTS),3);
	TheDb->SetGlobalSettingL(TPtrC(SMS_BEARER),1); //vep
	TheDb->SetGlobalSettingL(TPtrC(SMS_RECEIVE_MODE),2); //vep


    CleanupStack::PopAndDestroy();
    CHECKPOINT(TheDb->CommitTransaction()==KErrNone);
    CleanupStack::PopAndDestroy();
    }

#ifndef MODEM_PHONE_SERVICES_SMS
// TODO - #ifndef has to be removed later
GLDEF_C void CreateProtocolBindingsTableL()
	{
	// This section contains information previously stored in esk files
	// Esock reads the database to determine which protocols are defined
	// and which protocol modules to load

	CCommsDbTableView* table=TheDb->OpenTableLC(TPtrC(PROTOCOL_BINDINGS));

	table->InsertRecord(TcpProtocolId);
	table->WriteTextL(TPtrC(PROTNAME), _L("tcp"));
	table->WriteUintL(TPtrC(INDEX),4);
	table->WriteUintL(TPtrC(NUM_BINDS),2);
	table->WriteTextL(TPtrC(BIND_1),_L("ip"));
	table->WriteTextL(TPtrC(BIND_2),_L("icmp"));
	table->PutRecordChanges();

	table->InsertRecord(UdpProtocolId);
	table->WriteTextL(TPtrC(PROTNAME), _L("udp"));
	table->WriteUintL(TPtrC(INDEX),3);
	table->WriteUintL(TPtrC(NUM_BINDS),2);
	table->WriteTextL(TPtrC(BIND_1),_L("ip"));
	table->WriteTextL(TPtrC(BIND_2),_L("icmp"));
	table->PutRecordChanges();

	table->InsertRecord(IcmpProtocolId);
	table->WriteTextL(TPtrC(PROTNAME), _L("icmp"));
	table->WriteUintL(TPtrC(INDEX),2);
	table->WriteUintL(TPtrC(NUM_BINDS),1);
	table->WriteTextL(TPtrC(BIND_1),_L("ip"));
	table->PutRecordChanges();

	table->InsertRecord(IpProtocolId);
	table->WriteTextL(TPtrC(PROTNAME), _L("ip"));
	table->WriteUintL(TPtrC(INDEX),1);
	table->WriteUintL(TPtrC(NUM_BINDS),0);
	table->PutRecordChanges();

	table->InsertRecord(IrmuxProtocolId);
	table->WriteTextL(TPtrC(PROTNAME), _L("irmux"));
	table->WriteUintL(TPtrC(INDEX),1);
	table->WriteUintL(TPtrC(NUM_BINDS),0);
	table->PutRecordChanges();

	table->InsertRecord(TinyTPProtocolId);
	table->WriteTextL(TPtrC(PROTNAME), _L("tinytp"));
	table->WriteUintL(TPtrC(INDEX),2);
	table->WriteUintL(TPtrC(NUM_BINDS),0);
	table->PutRecordChanges();

	//
	// extra records for SmsStack added by AlfredH
	//

	// from smswap.sms.esk
	table->InsertRecord(SmsProtocolId);
	table->WriteTextL(TPtrC(PROTNAME), _L("sms"));
	table->WriteUintL(TPtrC(INDEX),1);
	table->WriteUintL(TPtrC(NUM_BINDS),0);
	table->PutRecordChanges();

	// from smswap.wap.esk
	table->InsertRecord(WapProtocolId);
	table->WriteTextL(TPtrC(PROTNAME), _L("wap"));
	table->WriteUintL(TPtrC(INDEX),1);
	table->WriteUintL(TPtrC(NUM_BINDS),1);
	table->WriteTextL(TPtrC(BIND_1),_L("sms"));
	table->PutRecordChanges();

	CleanupStack::PopAndDestroy();
	}


GLDEF_C void CreateProtocolFamiliesTableL()
	{
	TUint32 Id;
	CCommsDbTableView* table=TheDb->OpenTableLC(TPtrC(PROTOCOL_FAMILIES));

	table->InsertRecord(Id);
	table->WriteTextL(TPtrC(FAMILY_NAME),_L("TCPIP"));
	table->WriteTextL(TPtrC(PFR_FILENAME),_L("tcpip.prt"));
	table->WriteTextL(TPtrC(PROT_SETTINGS),TPtrC(GLOBAL_TCPIP_SETTINGS));
	table->WriteUintL(TPtrC(PROT_SETTINGS_REFERENCE),1);
	table->WriteUintL(TPtrC(P1),TcpProtocolId);
	table->WriteUintL(TPtrC(P2),UdpProtocolId);
	table->WriteUintL(TPtrC(P3),IcmpProtocolId);
	table->WriteUintL(TPtrC(P4),IpProtocolId);
	table->PutRecordChanges();

	table->InsertRecord(Id);
	table->WriteTextL(TPtrC(FAMILY_NAME),_L("IRDA"));
	table->WriteTextL(TPtrC(PFR_FILENAME),_L("irda.prt"));
	table->WriteUintL(TPtrC(PROT_SETTINGS_REFERENCE),1);
	table->WriteUintL(TPtrC(P1),IrmuxProtocolId);
	table->WriteUintL(TPtrC(P2),TinyTPProtocolId);
	table->PutRecordChanges();

 	//
	// extra records for SmsStack added by AlfredH
 	//

 	// from smswap.sms.esk
 	table->InsertRecord(Id);
 	table->WriteTextL(TPtrC(FAMILY_NAME),_L("SMS"));
 	table->WriteTextL(TPtrC(PFR_FILENAME),_L("smsprot.prt"));
 	table->WriteUintL(TPtrC(PROT_SETTINGS_REFERENCE),1);
 	table->WriteUintL(TPtrC(P1),SmsProtocolId);
 	table->PutRecordChanges();

 	// from smswap.wap.esk
 	table->InsertRecord(Id);
 	table->WriteTextL(TPtrC(FAMILY_NAME),_L("WAP"));
 	table->WriteTextL(TPtrC(PFR_FILENAME),_L("wapprot.prt"));
 	table->WriteUintL(TPtrC(PROT_SETTINGS_REFERENCE),1);
 	table->WriteUintL(TPtrC(P1),WapProtocolId);
	table->PutRecordChanges();

	CleanupStack::PopAndDestroy();
	}

LOCAL_C void CreateInterfaceSettingsTableL()
	{
	TUint32 id;
	CCommsDbTableView* table=TheDb->OpenTableLC(TPtrC(INTERFACE_SETTINGS));

	table->InsertRecord(id);
	table->WriteTextL(TPtrC(NIF),TPtrC(ETHINT));
	table->WriteUintL(TPtrC(NIF_REFERENCE),1);
	table->WriteTextL(TPtrC(PACKET_DRIVER),_L("EtherPkt"));
	table->WriteUintL(TPtrC(PACKET_DRIVER_REFERENCE),1);
	table->WriteTextL(TPtrC(LOGICAL_DEVICE),_L("ethercard"));
	table->WriteTextL(TPtrC(LOGICAL_DEVICE_NAME),_L("EtherCard"));
	table->WriteUintL(TPtrC(LOGICAL_DEVICE_REFERENCE),1);
#if !defined(__WINS__)
	table->WriteTextL(TPtrC(PHYSICAL_DEVICE),_L("Ether3com"));
	table->WriteTextL(TPtrC(PHYSICAL_DEVICE_NAME),_L("EtherCard.3com"));
	table->WriteUintL(TPtrC(PHYSICAL_DEVICE_REFERENCE),1);
#else
	table->WriteTextL(TPtrC(PHYSICAL_DEVICE),_L("EtherWins"));
	table->WriteTextL(TPtrC(PHYSICAL_DEVICE_NAME),_L("EtherCard.wins"));
	table->WriteUintL(TPtrC(PHYSICAL_DEVICE_REFERENCE),1);
#endif
	table->PutRecordChanges();

	table->InsertRecord(id);
	table->WriteTextL(TPtrC(NIF),_L("ppp"));
	table->WriteUintL(TPtrC(NIF_REFERENCE),1);
	table->WriteTextL(TPtrC(LOGICAL_DEVICE),_L("ECOMM"));
	table->WriteTextL(TPtrC(LOGICAL_DEVICE_NAME),_L("Comm"));
	table->WriteUintL(TPtrC(LOGICAL_DEVICE_REFERENCE),1);
	table->WriteTextL(TPtrC(PHYSICAL_DEVICE),TPtrC(_L("ECDRV")));
	table->WriteTextL(TPtrC(PHYSICAL_DEVICE_NAME),_L("Comm.Wins"));
	table->WriteUintL(TPtrC(PHYSICAL_DEVICE_REFERENCE),1);
	table->PutRecordChanges();

	table->InsertRecord(id);
	table->WriteTextL(TPtrC(NIF),_L("tnif"));
	table->PutRecordChanges();

	CleanupStack::PopAndDestroy();
	}
#endif

LOCAL_C void CreateChargecardTableL()
/**
 *  @test ChargeCard Database Entry
 */
    {
    TUint32 id;
    CCommsDbTableView* table1=TheDb->OpenTableLC(TPtrC(CHARGECARD));

    CHECKPOINT(table1->InsertRecord(id)==KErrNone);
    table1->WriteTextL(TPtrC(COMMDB_NAME),_L("Dummy BT Chargecard"));
    table1->WriteTextL(TPtrC(CHARGECARD_ACCOUNT_NUMBER),_L("144,12345678"));
    table1->WriteTextL(TPtrC(CHARGECARD_PIN),_L("0000"));
    table1->WriteTextL(TPtrC(CHARGECARD_LOCAL_RULE),_L("HG"));
    table1->WriteTextL(TPtrC(CHARGECARD_NAT_RULE),_L("HFG"));
    table1->WriteTextL(TPtrC(CHARGECARD_INTL_RULE),_L("HEFG"));
    CHECKPOINT(table1->PutRecordChanges()==KErrNone);

    CHECKPOINT(table1->InsertRecord(id)==KErrNone);
    table1->WriteTextL(TPtrC(COMMDB_NAME),_L("Dummy Mercury Chargecard"));
    table1->WriteTextL(TPtrC(CHARGECARD_ACCOUNT_NUMBER),_L("0500800800,,8944223457573566"));
    table1->WriteTextL(TPtrC(CHARGECARD_PIN),_L("****"));
    table1->WriteTextL(TPtrC(CHARGECARD_LOCAL_RULE),_L("HG"));
    table1->WriteTextL(TPtrC(CHARGECARD_NAT_RULE),_L("J,K,0FG"));
    table1->WriteTextL(TPtrC(CHARGECARD_INTL_RULE),_L("HEFG"));
    CHECKPOINT(table1->PutRecordChanges()==KErrNone);

    CleanupStack::PopAndDestroy();
    }

LOCAL_C void CreateLocationTableL( TMyDialLocation aLocDataFax, TMyDialLocation aLocServSMS )
/**
 *  @test Location Database Entries
 */
    {
#ifdef MODEM_PHONE_SERVICES_SMS

    CCommsDbTableView* table4;
    TUint32 id1, id2,id3,id4;

// Office Location
    table4 = TheDb->OpenTableLC(TPtrC(LOCATION));
    CHECKPOINT(table4->InsertRecord(id1)==KErrNone);
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
    CHECKPOINT(table4->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table4 = NULL;
// Office (without dial out code) Location
    table4 = TheDb->OpenTableLC(TPtrC(LOCATION));
    CHECKPOINT(table4->InsertRecord(id2)==KErrNone);
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
    CHECKPOINT(table4->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table4 = NULL;
// Mobile Network
    table4 = TheDb->OpenTableLC(TPtrC(LOCATION));
    CHECKPOINT(table4->InsertRecord(id3)==KErrNone);
    table4->WriteTextL(TPtrC(COMMDB_NAME),_L("Mobile"));
    table4->WriteTextL(TPtrC(LOCATION_INTL_PREFIX_CODE),_L("+"));
    table4->WriteTextL(TPtrC(LOCATION_NAT_PREFIX_CODE),_L("0"));
    table4->WriteTextL(TPtrC(LOCATION_NAT_CODE),_L("44"));
    table4->WriteTextL(TPtrC(LOCATION_AREA_CODE),KNullDesC);
    table4->WriteTextL(TPtrC(LOCATION_DIAL_OUT_CODE),KNullDesC);
    table4->WriteTextL(TPtrC(LOCATION_DISABLE_CALL_WAITING_CODE),KNullDesC);
    table4->WriteBoolL(TPtrC(LOCATION_MOBILE),ETrue);
    table4->WriteBoolL(TPtrC(LOCATION_USE_PULSE_DIAL),EFalse);
    table4->WriteBoolL(TPtrC(LOCATION_WAIT_FOR_DIAL_TONE),EFalse);
    table4->WriteUintL(TPtrC(LOCATION_PAUSE_AFTER_DIAL_OUT),(TUint32)0);
    CHECKPOINT(table4->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table4 = NULL;
// Home
    table4 = TheDb->OpenTableLC(TPtrC(LOCATION));
    CHECKPOINT(table4->InsertRecord(id4)==KErrNone);
    table4->WriteTextL(TPtrC(COMMDB_NAME),_L("Home"));
    table4->WriteTextL(TPtrC(LOCATION_INTL_PREFIX_CODE),_L("00"));
    table4->WriteTextL(TPtrC(LOCATION_NAT_PREFIX_CODE),_L("0"));
    table4->WriteTextL(TPtrC(LOCATION_NAT_CODE),_L("44"));
    table4->WriteTextL(TPtrC(LOCATION_AREA_CODE),_L("181"));
    table4->WriteTextL(TPtrC(LOCATION_DIAL_OUT_CODE),KNullDesC);
    table4->WriteTextL(TPtrC(LOCATION_DISABLE_CALL_WAITING_CODE),KNullDesC);
    table4->WriteBoolL(TPtrC(LOCATION_MOBILE),EFalse);
    table4->WriteBoolL(TPtrC(LOCATION_USE_PULSE_DIAL),ETrue);
    table4->WriteBoolL(TPtrC(LOCATION_WAIT_FOR_DIAL_TONE),ETrue);
    table4->WriteUintL(TPtrC(LOCATION_PAUSE_AFTER_DIAL_OUT),(TUint32)0);
    CHECKPOINT(table4->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table4 = NULL;

	if ( aLocDataFax != EDialLocationChooseManual )
		{
		if ( aLocDataFax == EOffice )
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_DATA_FAX),id1);
		else if ( aLocDataFax == EOfficeDirect )
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_DATA_FAX),id2);
		else if ( aLocDataFax == EMobile )
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_DATA_FAX),id3);
		else if ( aLocDataFax == EHome )
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_DATA_FAX),id4);
		else
			User::Invariant();
		}
	else
		{
		gTest.Printf(_L("\nPlease select the location from which you are dialling for data modem:\n"));
		gTest.Printf(_L("a) Office\n"));
		gTest.Printf(_L("b) Office Direct Dial\n"));
		gTest.Printf(_L("c) Mobile\n"));
		gTest.Printf(_L("d) Home\n"));
		TUint32 ch=gTest.Getch();

		if (ch=='a' || ch=='A')
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_DATA_FAX),id1);
		else if (ch=='b' || ch=='B')
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_DATA_FAX),id2);
		else if (ch=='c' || ch=='C')
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_DATA_FAX),id3);
		else if (ch=='d' || ch=='D')
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_DATA_FAX),id4);
		else
			User::Invariant();
		}
	if ( aLocServSMS != EDialLocationChooseManual )
		{
		if ( aLocServSMS == EOffice )
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_PHONE_SERVICES_SMS),id1);
		else if ( aLocServSMS == EOfficeDirect )
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_PHONE_SERVICES_SMS),id2);
		else if ( aLocServSMS == EMobile )
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_PHONE_SERVICES_SMS),id3);
		else if ( aLocServSMS == EHome )
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_PHONE_SERVICES_SMS),id4);
		else
			User::Invariant();
		}
	else
		{
		gTest.Printf(_L("\nPlease select the location from which you are dialling for modem for sms service:\n"));
		gTest.Printf(_L("a) Office\n"));
		gTest.Printf(_L("b) Office Direct Dial\n"));
		gTest.Printf(_L("c) Mobile\n"));
		gTest.Printf(_L("d) Home\n"));
		TUint32 ch=gTest.Getch();

		if (ch=='a' || ch=='A')
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_PHONE_SERVICES_SMS),id1);
		else if (ch=='b' || ch=='B')
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_PHONE_SERVICES_SMS),id2);
		else if (ch=='c' || ch=='C')
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_PHONE_SERVICES_SMS),id3);
		else if (ch=='d' || ch=='D')
			TheDb->SetGlobalSettingL(TPtrC(LOCATION_PHONE_SERVICES_SMS),id4);
		else
			User::Invariant();
		}
#else // no MODEM_PHONE_SERVICES_SMS defined (old commdb)
	aLocDataFax = aLocServSMS;

    TUint32 ch = 'c';
	if ( aLocServSMS != EDialLocationChooseManual )
		{
		if ( aLocServSMS == EOffice )
			ch = 'a';
		else if ( aLocServSMS == EOfficeDirect )
			ch = 'b';
		else if ( aLocServSMS == EMobile )
			ch = 'c';
		else if ( aLocServSMS == EHome )
			ch = 'd';
		else
			User::Invariant();
		}
	else
        {
	    gTest.Printf(_L("\nPlease select the location from which you are dialling:\n"));
	    gTest.Printf(_L("a) Office\n"));
	    gTest.Printf(_L("b) Office Direct Dial\n"));
	    gTest.Printf(_L("c) Mobile\n"));
	    gTest.Printf(_L("d) Home\n"));
	    ch=gTest.Getch();
        }

	CCommsDbTableView* table4=TheDb->OpenTableLC(TPtrC(LOCATION));
	TUint32 id;
	CHECKPOINT(table4->InsertRecord(id)==KErrNone);

	if (ch=='a' || ch=='A')
		{
// Office Location
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
		}
	else if (ch=='b' || ch=='B')
		{
// Office (without dial out code) Location
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
		}
	else if (ch=='c' || ch=='C')
		{
// Mobile Network
		table4->WriteTextL(TPtrC(COMMDB_NAME),_L("Mobile"));
		table4->WriteTextL(TPtrC(LOCATION_INTL_PREFIX_CODE),_L("+"));
		table4->WriteTextL(TPtrC(LOCATION_NAT_PREFIX_CODE),_L("0"));
		table4->WriteTextL(TPtrC(LOCATION_NAT_CODE),_L("44"));
		table4->WriteTextL(TPtrC(LOCATION_AREA_CODE),KNullDesC);
		table4->WriteTextL(TPtrC(LOCATION_DIAL_OUT_CODE),KNullDesC);
		table4->WriteTextL(TPtrC(LOCATION_DISABLE_CALL_WAITING_CODE),KNullDesC);
		table4->WriteBoolL(TPtrC(LOCATION_MOBILE),ETrue);
		table4->WriteBoolL(TPtrC(LOCATION_USE_PULSE_DIAL),EFalse);
		table4->WriteBoolL(TPtrC(LOCATION_WAIT_FOR_DIAL_TONE),EFalse);
		table4->WriteUintL(TPtrC(LOCATION_PAUSE_AFTER_DIAL_OUT),(TUint32)0);
		}
	else if (ch=='d' || ch=='D')
		{
// Home
		table4->WriteTextL(TPtrC(COMMDB_NAME),_L("Home"));
		table4->WriteTextL(TPtrC(LOCATION_INTL_PREFIX_CODE),_L("00"));
		table4->WriteTextL(TPtrC(LOCATION_NAT_PREFIX_CODE),_L("0"));
		table4->WriteTextL(TPtrC(LOCATION_NAT_CODE),_L("44"));
		table4->WriteTextL(TPtrC(LOCATION_AREA_CODE),_L("181"));
		table4->WriteTextL(TPtrC(LOCATION_DIAL_OUT_CODE),KNullDesC);
		table4->WriteTextL(TPtrC(LOCATION_DISABLE_CALL_WAITING_CODE),KNullDesC);
		table4->WriteBoolL(TPtrC(LOCATION_MOBILE),EFalse);
		table4->WriteBoolL(TPtrC(LOCATION_USE_PULSE_DIAL),ETrue);
		table4->WriteBoolL(TPtrC(LOCATION_WAIT_FOR_DIAL_TONE),ETrue);
		table4->WriteUintL(TPtrC(LOCATION_PAUSE_AFTER_DIAL_OUT),(TUint32)0);
		}
	else
		User::Invariant();

	CHECKPOINT(table4->PutRecordChanges()==KErrNone);
	CleanupStack::PopAndDestroy();
#endif

    }


LOCAL_C void CreateModemTablesL( TMyModemType aModemDataFax, TMyModemType aModemServSMS,
                                 TUint aCommPortDataFax,     TUint aCommPortServSMS )
/**
 *  @test Modem Database Entries
 */
    {
#ifdef MODEM_PHONE_SERVICES_SMS

    CCommsDbTableView* table2;
    TUint32 id1, id2,id3,id4,id5,id6,id7,id8,id9;
    TBuf<9>  commPort;
    TBuf<11> ircommPort;

    commPort.Format(_L("COMM::%c"),'0');
    ircommPort.Format(_L("IRCOMM::%c"),'0');

//  NULL Modem with 115200 bps
    table2=TheDb->OpenTableLC(TPtrC(MODEM));
    CHECKPOINT(table2->InsertRecord(id1)==KErrNone);
    table2->WriteTextL(TPtrC(COMMDB_NAME),_L("Null Modem 115200bps"));
    table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
    table2->WriteTextL(TPtrC(MODEM_TSY_NAME), _L("NTRASTSY"));
    table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("ECUART"));
    table2->WriteUintL(TPtrC(MODEM_DATA_BITS),(TUint32)EData8);
    table2->WriteUintL(TPtrC(MODEM_STOP_BITS),(TUint32)EStop1);
    table2->WriteUintL(TPtrC(MODEM_PARITY),(TUint32)EParityNone);
    table2->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps115200);
    table2->WriteUintL(TPtrC(MODEM_HANDSHAKING),(TUint32)(KConfigObeyCTS|KConfigObeyDSR|KConfigFailDSR));
    table2->WriteUintL(TPtrC(MODEM_SPECIAL_RATE),(TUint32)0);
    table2->WriteUintL(TPtrC(MODEM_XON_CHAR),(TUint32)0);
    table2->WriteUintL(TPtrC(MODEM_XOFF_CHAR),(TUint32)0);
    table2->WriteUintL(TPtrC(MODEM_FAX_CLASS_PREF),(TUint32)EClassAuto);
    table2->WriteUintL(TPtrC(MODEM_SPEAKER_PREF),(TUint32)EModemSpeakerSettingNever);
    table2->WriteUintL(TPtrC(MODEM_SPEAKER_VOL_PREF),(TUint32)EModemSpeakerVolumeQuiet);
    table2->WriteTextL(TPtrC(MODEM_MODEM_INIT_STRING),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_DATA_INIT_STRING),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_ISP_INIT_STRING),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_DIAL_PAUSE_LENGTH),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_CARRIER_TIMEOUT),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_AUTO_ANSWER_RING_COUNT),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_SPEAKER_VOL_CONTROL_LOW),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_SPEAKER_VOL_CONTROL_MEDIUM),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_SPEAKER_VOL_CONTROL_HIGH),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_SPEAKER_ALWAYS_OFF),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_SPEAKER_ON_UNTIL_CARRIER),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_SPEAKER_ALWAYS_ON),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_SPEAKER_ON_AFTER_DIAL_UNTIL_CARRIER),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_DIAL_TONE_WAIT_MODIFIER),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_CALL_PROGRESS_1),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_CALL_PROGRESS_2),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_CALL_PROGRESS_3),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_CALL_PROGRESS_4),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_ECHO_OFF),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_VERBOSE_TEXT),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_QUIET_OFF),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_QUIET_ON),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_DIAL_COMMAND_STATE_MODIFIER),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_ON_LINE),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_RESET_CONFIGURATION),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_RETURN_TO_FACTORY_DEFS),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_DCD_ON_DURING_LINK),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_DTR_HANG_UP),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_DSR_ALWAYS_ON),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_RTS_CTS_HANDSHAKE),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_XON_XOFF_HANDSHAKE),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_ESCAPE_CHARACTER),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_ESCAPE_GUARD_PERIOD),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_FAX_CLASS_INTERROGATE),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_FAX_CLASS),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_NO_DIAL_TONE),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_BUSY),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_NO_ANSWER),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_CARRIER),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_CONNECT),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_COMPRESSION_CLASS_5),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_COMPRESSION_V42BIS),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_COMPRESSION_NONE),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_PROTOCOL_LAPD),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_PROTOCOL_ALT),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_PROTOCOL_ALTCELLULAR),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_PROTOCOL_NONE),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),KVodafoneSC);
    table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),0);
    table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),0);
    CHECKPOINT(table2->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table2 = NULL;
    //  USR Sportster Modem
    table2=TheDb->OpenTableLC(TPtrC(MODEM));
    CHECKPOINT(table2->InsertRecord(id2)==KErrNone);
    table2->WriteTextL(TPtrC(COMMDB_NAME),_L("US Robotics Sportster"));
    table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
    table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("ECUART"));
    table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("HAYES"));
    table2->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps57600);
    table2->WriteUintL(TPtrC(MODEM_DATA_BITS),(TUint32)EData8);
    table2->WriteUintL(TPtrC(MODEM_STOP_BITS),(TUint32)EStop1);
    table2->WriteUintL(TPtrC(MODEM_PARITY),(TUint32)EParityNone);
    table2->WriteUintL(TPtrC(MODEM_HANDSHAKING),(TUint32)(KConfigObeyCTS|KConfigObeyDSR|KConfigFailDSR|KConfigObeyDCD|KConfigFailDCD)); //KConfigObeyCTS;
    table2->WriteUintL(TPtrC(MODEM_SPECIAL_RATE),(TUint32)0);
    table2->WriteUintL(TPtrC(MODEM_XON_CHAR),(TUint32)0);
    table2->WriteUintL(TPtrC(MODEM_XOFF_CHAR),(TUint32)0);
    table2->WriteTextL(TPtrC(MODEM_MODEM_INIT_STRING),_L8("AT&F1"));
    table2->WriteTextL(TPtrC(MODEM_DATA_INIT_STRING),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING),_L8("AT&d2"));
    table2->WriteUintL(TPtrC(MODEM_FAX_CLASS_PREF),(TUint32)EClassAuto);
    table2->WriteUintL(TPtrC(MODEM_SPEAKER_PREF),(TUint32)EModemSpeakerSettingAfterDialUntilAnswer);
    table2->WriteUintL(TPtrC(MODEM_SPEAKER_VOL_PREF),(TUint32)EModemSpeakerVolumeQuiet);
    CHECKPOINT(table2->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table2 = NULL;
// Dacom Surfer
    table2=TheDb->OpenTableLC(TPtrC(MODEM));
    CHECKPOINT(table2->InsertRecord(id3)==KErrNone);
    table2->WriteTextL(TPtrC(COMMDB_NAME),_L("Dacom Surfer"));
    table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
    table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("ECUART"));
    table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("HAYES"));
    table2->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps57600);
    table2->WriteUintL(TPtrC(MODEM_DATA_BITS),(TUint32)EData8);
    table2->WriteUintL(TPtrC(MODEM_STOP_BITS),(TUint32)EStop1);
    table2->WriteUintL(TPtrC(MODEM_PARITY),(TUint32)EParityNone);
    table2->WriteUintL(TPtrC(MODEM_HANDSHAKING),(TUint32)(KConfigObeyCTS|KConfigObeyDSR|KConfigFailDSR|KConfigObeyDCD|KConfigFailDCD)); //KConfigObeyCTS;
    table2->WriteUintL(TPtrC(MODEM_SPECIAL_RATE),(TUint32)0);
    table2->WriteUintL(TPtrC(MODEM_XON_CHAR),(TUint32)0);
    table2->WriteUintL(TPtrC(MODEM_XOFF_CHAR),(TUint32)0);
    table2->WriteTextL(TPtrC(MODEM_MODEM_INIT_STRING),_L8("AT&F"));
    table2->WriteTextL(TPtrC(MODEM_DATA_INIT_STRING),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING),_L8("AT&d2"));
    table2->WriteUintL(TPtrC(MODEM_FAX_CLASS_PREF),(TUint32)EClassAuto);
    table2->WriteUintL(TPtrC(MODEM_SPEAKER_PREF),(TUint32)EModemSpeakerSettingAfterDialUntilAnswer);
    table2->WriteUintL(TPtrC(MODEM_SPEAKER_VOL_PREF),(TUint32)EModemSpeakerVolumeQuiet);
    CHECKPOINT(table2->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table2 = NULL;
// IR Mobile Phone
    table2=TheDb->OpenTableLC(TPtrC(MODEM));
    CHECKPOINT(table2->InsertRecord(id4)==KErrNone);
    table2->WriteTextL(TPtrC(COMMDB_NAME),_L("GSM Mobile Phone via Infrared"));
    table2->WriteTextL(TPtrC(MODEM_PORT_NAME),ircommPort);
    table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("IRCOMM"));
    table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("GSMBSC"));
    table2->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps115200);
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
    table2->WriteUintL(TPtrC(MODEM_FAX_CLASS_PREF),(TUint32)EClassAuto);
    table2->WriteUintL(TPtrC(MODEM_SPEAKER_PREF),(TUint32)EModemSpeakerSettingNever);
    table2->WriteUintL(TPtrC(MODEM_SPEAKER_VOL_PREF),(TUint32)EModemSpeakerVolumeQuiet);
    table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),KVodafoneSC);
    table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),1440ul);
    table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),EFalse);
    CHECKPOINT(table2->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table2 = NULL;
// Cabled 9110, etc
    table2=TheDb->OpenTableLC(TPtrC(MODEM));
    CHECKPOINT(table2->InsertRecord(id5)==KErrNone);
    table2->WriteTextL(TPtrC(COMMDB_NAME),_L("GSM Mobile Phone via Serial"));
    table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
    table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("ECUART"));
    table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("GSMBSC"));
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
    table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),KVodafoneSC);
    table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),1440ul);
    table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),EFalse);
    CHECKPOINT(table2->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table2 = NULL;
// R520m via IR
    table2=TheDb->OpenTableLC(TPtrC(MODEM));
    CHECKPOINT(table2->InsertRecord(id6)==KErrNone);
    table2->WriteTextL(TPtrC(COMMDB_NAME),_L("GPRS Ericsson R520m via IR"));
    table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
    table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("IRCOMM"));
    table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("ATGPRS"));
    table2->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps115200);
    table2->WriteUintL(TPtrC(MODEM_DATA_BITS),(TUint32)EData8);
    table2->WriteUintL(TPtrC(MODEM_STOP_BITS),(TUint32)EStop1);
    table2->WriteUintL(TPtrC(MODEM_PARITY),(TUint32)EParityNone);
    table2->WriteUintL(TPtrC(MODEM_HANDSHAKING),(TUint32)KConfigObeyCTS);
    table2->WriteUintL(TPtrC(MODEM_SPECIAL_RATE),(TUint32)0);
    table2->WriteUintL(TPtrC(MODEM_XON_CHAR),(TUint32)17);
    table2->WriteUintL(TPtrC(MODEM_XOFF_CHAR),(TUint32)19);
    table2->WriteTextL(TPtrC(MODEM_MODEM_INIT_STRING),_L8("ATZ"));
    table2->WriteTextL(TPtrC(MODEM_DATA_INIT_STRING),KNullDesC8);
    table2->WriteTextL(TPtrC(MODEM_FAX_INIT_STRING),KNullDesC8);
    table2->WriteUintL(TPtrC(MODEM_FAX_CLASS_PREF),(TUint32)EClassAuto);
    table2->WriteUintL(TPtrC(MODEM_SPEAKER_PREF),(TUint32)EModemSpeakerSettingNever);
    table2->WriteUintL(TPtrC(MODEM_SPEAKER_VOL_PREF),(TUint32)EModemSpeakerVolumeQuiet);
    table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),KVodafoneSC);
    table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),1440ul);
    table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),EFalse);
    CHECKPOINT(table2->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table2 = NULL;
// R520m via serial
    table2=TheDb->OpenTableLC(TPtrC(MODEM));
    CHECKPOINT(table2->InsertRecord(id7)==KErrNone);
    table2->WriteTextL(TPtrC(COMMDB_NAME),_L("GPRS Ericsson R520m via Serial"));
    table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
    table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("ECUART"));
    table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("ATGPRS"));
    table2->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps115200);
    table2->WriteUintL(TPtrC(MODEM_DATA_BITS),(TUint32)EData8);
    table2->WriteUintL(TPtrC(MODEM_STOP_BITS),(TUint32)EStop1);
    table2->WriteUintL(TPtrC(MODEM_PARITY),(TUint32)EParityNone);
    table2->WriteUintL(TPtrC(MODEM_HANDSHAKING),(TUint32)KConfigObeyCTS);
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
    table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),KVodafoneSC);
    table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),1440ul);
    table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),EFalse);
    CHECKPOINT(table2->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table2 = NULL;
// GPRS Timeport
    table2=TheDb->OpenTableLC(TPtrC(MODEM));
    CHECKPOINT(table2->InsertRecord(id8)==KErrNone);
    table2->WriteTextL(TPtrC(COMMDB_NAME),_L("GPRS Motorola Mobile Phone via Serial"));
    table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
    table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("ECUART"));
    table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("ATGPRS"));
    table2->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps57600);   // Old one is 38400
    table2->WriteUintL(TPtrC(MODEM_DATA_BITS),(TUint32)EData8);
    table2->WriteUintL(TPtrC(MODEM_STOP_BITS),(TUint32)EStop1); // Old one is 2
    table2->WriteUintL(TPtrC(MODEM_PARITY),(TUint32)EParityNone);
    table2->WriteUintL(TPtrC(MODEM_HANDSHAKING),(TUint32)KConfigObeyCTS);
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
    table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),KVodafoneSC);
    table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),1440ul);
    table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),EFalse);
    CHECKPOINT(table2->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table2 = NULL;

//Loads the T_REG.TSY and uses the DUMMY csy
    table2=TheDb->OpenTableLC(TPtrC(MODEM));
    CHECKPOINT(table2->InsertRecord(id9)==KErrNone);
    table2->WriteTextL(TPtrC(COMMDB_NAME),_L("Test Mobile Phone"));
    table2->WriteTextL(TPtrC(MODEM_PORT_NAME),_L("DUMMY::0"));
    table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("DUMMY"));
    table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("T_REG"));
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
    table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),KVodafoneSC);
    table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),1440ul);
    table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),EFalse);
    CHECKPOINT(table2->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    table2 = NULL;


	TUint32 defaultModemDataId = 0ul;
	TUint32 defaultModemSMSId = 0ul;
	TUint acommPort;

	if( aModemDataFax != EModemTypeChooseMamual )
		{

		// Choose the Data Fax modem
		if( aModemDataFax == ENullModem)
			defaultModemDataId = id1;
		else if( aModemDataFax == EUSRSportster)
			defaultModemDataId = id2;
		else if( aModemDataFax == EDacomSurfer)
			defaultModemDataId = id3;
		else if( aModemDataFax == EGSMviaIR)
			defaultModemDataId = id4;
		else if( aModemDataFax == EGSMviaCOM)
			defaultModemDataId = id5;
		else if( aModemDataFax == EEricssonGPRSMobilePhoneviaIR)
			defaultModemDataId = id6;
		else if( aModemDataFax == EricssonGPRSMobilePhoneviaSerialcable)
			defaultModemDataId = id7;
		else if( aModemDataFax == EMotorolaGPRSMobilePhoneviaSerialcable)
			defaultModemDataId = id8;
		else if( aModemDataFax == EDummy)
			defaultModemDataId = id9;
		else
			User::Invariant();
		TheDb->SetGlobalSettingL(TPtrC(MODEM_DATA_FAX),defaultModemDataId);

		if( aModemDataFax != EGSMviaIR)
			commPort.Format(_L("COMM::%c"),aCommPortDataFax);
        else
			commPort.Format(_L("IRCOMM::%c"),aCommPortDataFax);
		table2 = TheDb->OpenViewMatchingUintLC(TPtrC(MODEM),TPtrC(COMMDB_ID),defaultModemDataId);
		CHECKPOINT(table2->GotoFirstRecord()==KErrNone);
		CHECKPOINT(table2->UpdateRecord()==KErrNone);
		table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
		CHECKPOINT(table2->PutRecordChanges()==KErrNone);
		CleanupStack::PopAndDestroy();
		table2 = NULL;
		}
	else
		{
		gTest.Printf(_L("\nPlease select the modem to be used for data:\n"));
		gTest.Printf(_L("a) Null Modem\n"));
		gTest.Printf(_L("b) USR Sportster\n"));
		gTest.Printf(_L("c) Dacom Surfer\n"));
		gTest.Printf(_L("d) GSM Mobile Phone via IR\n"));
		gTest.Printf(_L("e) GSM Mobile Phone via Serial cable\n"));
		gTest.Printf(_L("f) Ericsson GPRS Mobile Phone via IR\n"));
		gTest.Printf(_L("g) Ericsson GPRS Mobile Phone via Serial cable\n"));
		gTest.Printf(_L("h) Motorola GPRS Mobile Phone via Serial cable\n"));
		gTest.Printf(_L("i) Dummy Tsy and Modem\n"));

		TUint32 ch=gTest.Getch();

		if(ch == 'a' || ch ==  'A')
			defaultModemDataId = id1;
		else if(ch == 'b' || ch ==  'B')
			defaultModemDataId = id2;
		else if(ch == 'c' || ch ==  'C')
			defaultModemDataId = id3;
		else if(ch == 'd' || ch ==  'D')
			defaultModemDataId = id4;	//EGSMviaIR
		else if(ch == 'e' || ch ==  'E')
			defaultModemDataId = id5;
		else if(ch == 'f' || ch ==  'F')
			defaultModemDataId = id6;
		else if(ch == 'g' || ch ==  'G')
			defaultModemDataId = id7;
		else if(ch == 'h' || ch ==  'H')
			defaultModemDataId = id8;
		else if(ch == 'i' || ch ==  'I')
			defaultModemDataId = id9;
		else
			User::Invariant();

		TheDb->SetGlobalSettingL(TPtrC(MODEM_DATA_FAX),defaultModemDataId);
		gTest.Printf(_L("\nPlease Choose Test Board Comm Port for data modem: "));
		acommPort = gTest.Getch();
		if( defaultModemDataId != id4 )	//EGSMviaIR
			commPort.Format(_L("COMM::%c"),acommPort);
        else
			commPort.Format(_L("IRCOMM::%c"),acommPort);
		table2 = TheDb->OpenViewMatchingUintLC(TPtrC(MODEM),TPtrC(COMMDB_ID),defaultModemDataId);
		CHECKPOINT(table2->GotoFirstRecord()==KErrNone);
		CHECKPOINT(table2->UpdateRecord()==KErrNone);
		table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
		CHECKPOINT(table2->PutRecordChanges()==KErrNone);
		CleanupStack::PopAndDestroy();
		table2 = NULL;
		}

	if( aModemServSMS != EModemTypeChooseMamual )
		{

		// Choose the ServiceSMS modem
		if( aModemServSMS == ENullModem)
			defaultModemSMSId = id1;
		else if( aModemServSMS == EUSRSportster)
			defaultModemSMSId = id2;
		else if( aModemServSMS == EDacomSurfer)
			defaultModemSMSId = id3;
		else if( aModemServSMS == EGSMviaIR)
			defaultModemSMSId = id4;
		else if( aModemServSMS == EGSMviaCOM)
			defaultModemSMSId = id5;
		else if( aModemServSMS == EEricssonGPRSMobilePhoneviaIR)
			defaultModemSMSId = id6;
		else if( aModemServSMS == EricssonGPRSMobilePhoneviaSerialcable)
			defaultModemSMSId = id7;
		else if( aModemServSMS == EMotorolaGPRSMobilePhoneviaSerialcable)
			defaultModemSMSId = id8;
		else if( aModemServSMS == EDummy)
			defaultModemSMSId = id9;
		else
			User::Invariant();

		TheDb->SetGlobalSettingL(TPtrC(MODEM_PHONE_SERVICES_SMS),defaultModemSMSId);
		if( aModemServSMS != EGSMviaIR)
			commPort.Format(_L("COMM::%c"),aCommPortServSMS);
        else
			commPort.Format(_L("IRCOMM::%c"),aCommPortServSMS);
		table2 = TheDb->OpenViewMatchingUintLC(TPtrC(MODEM),TPtrC(COMMDB_ID),defaultModemSMSId);
		CHECKPOINT(table2->GotoFirstRecord()==KErrNone);
		CHECKPOINT(table2->UpdateRecord()==KErrNone);
		table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
		CHECKPOINT(table2->PutRecordChanges()==KErrNone);
		CleanupStack::PopAndDestroy();
		table2 = NULL;
		gTest.Printf(_L("\n"));
		}
	else
		{
		gTest.Printf(_L("\n\n"));
		gTest.Printf(_L("\nPlease select the modem you will be using for sms service:\n"));
		gTest.Printf(_L("a) Null Modem\n"));
		gTest.Printf(_L("b) USR Sportster\n"));
		gTest.Printf(_L("c) Dacom Surfer\n"));
		gTest.Printf(_L("d) GSM Mobile Phone via IR\n"));
		gTest.Printf(_L("e) GSM Mobile Phone via Serial cable\n"));
		gTest.Printf(_L("f) Ericsson GPRS Mobile Phone via IR\n"));
		gTest.Printf(_L("g) Ericsson GPRS Mobile Phone via Serial cable\n"));
		gTest.Printf(_L("h) Motorola GPRS Mobile Phone via Serial cable\n"));
		gTest.Printf(_L("i) Dummy Tsy and Modem\n"));

		TUint32 ch=gTest.Getch();

		if(ch == 'a' || ch ==  'A')
			defaultModemSMSId = id1;
		else if(ch == 'b' || ch ==  'B')
			defaultModemSMSId = id2;
		else if(ch == 'c' || ch ==  'C')
			defaultModemSMSId = id3;
		else if(ch == 'd' || ch ==  'D')
			defaultModemSMSId = id4;      //EGSMviaIR
		else if(ch == 'e' || ch ==  'E')
			defaultModemSMSId = id5;
		else if(ch == 'f' || ch ==  'F')
			defaultModemSMSId = id6;
		else if(ch == 'g' || ch ==  'G')
			defaultModemSMSId = id7;
		else if(ch == 'h' || ch ==  'H')
			defaultModemSMSId = id8;
		else if(ch == 'i' || ch ==  'I')
			defaultModemSMSId = id9;
		else
			User::Invariant();

		TheDb->SetGlobalSettingL(TPtrC(MODEM_PHONE_SERVICES_SMS),defaultModemSMSId);
		gTest.Printf(_L("\nPlease Choose your Test Board Comm Port for modem for sms services : "));
		acommPort = gTest.Getch();
		if(defaultModemSMSId != id4)      //EGSMviaIR
			commPort.Format(_L("COMM::%c"),acommPort);
        else
			commPort.Format(_L("IRCOMM::%c"),acommPort);
		table2 = TheDb->OpenViewMatchingUintLC(TPtrC(MODEM),TPtrC(COMMDB_ID),defaultModemSMSId);
		CHECKPOINT(table2->GotoFirstRecord()==KErrNone);
		CHECKPOINT(table2->UpdateRecord()==KErrNone);
		table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
		CHECKPOINT(table2->PutRecordChanges()==KErrNone);
		CleanupStack::PopAndDestroy();
		table2 = NULL;

		gTest.Printf(_L("\n"));
		}
#else //MODEM_PHONE_SERVICES_SMS not defined (uses old commdb)
    aModemDataFax    = aModemServSMS;
    aCommPortDataFax = aCommPortServSMS;

    TUint32 ch = 'c';
    TUint acommPort = '0';
	if ( aModemServSMS != EModemTypeChooseMamual )
		{
		if ( aModemServSMS == EGSMviaIR )
			ch = 'a';
		else if ( aModemServSMS == EGSMviaCOM )
			ch = 'b';
		else if ( aModemServSMS == EDummy )
			ch = 'c';
		else
			User::Invariant();
        acommPort = aCommPortServSMS;
		}
	else
        {
	    gTest.Printf(_L("\nPlease select the modem you are using:\n"));
	    gTest.Printf(_L("a) GSM Mobile Phone via IR\n"));
	    gTest.Printf(_L("b) GSM Mobile Phone via Serial cable\n"));
	    gTest.Printf(_L("c) Test Mobile Phone - Dummy TSY\n"));
	    ch=gTest.Getch();
    	gTest.Printf(_L("\nPlease Choose your Test Board Comm Port : "));
	    acommPort = gTest.Getch();
        }

	CCommsDbTableView* table2=TheDb->OpenTableLC(TPtrC(MODEM));
	TUint32 id;
	CHECKPOINT(table2->InsertRecord(id)==KErrNone);

	if (ch=='a' || ch=='A')
		{
// IR Mobile Phone
		table2->WriteTextL(TPtrC(COMMDB_NAME),_L("GSM Mobile Phone via Infrared"));
		TBuf<9> commPort;
		commPort.Format(_L("IRCOMM::%c"),acommPort);
		table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
		table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("IRCOMM"));
		table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("GSMBSC"));
		table2->WriteUintL(TPtrC(MODEM_RATE),(TUint32)EBps115200);
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
		table2->WriteUintL(TPtrC(MODEM_FAX_CLASS_PREF),(TUint32)EClassAuto);
		table2->WriteUintL(TPtrC(MODEM_SPEAKER_PREF),(TUint32)EModemSpeakerSettingNever);
		table2->WriteUintL(TPtrC(MODEM_SPEAKER_VOL_PREF),(TUint32)EModemSpeakerVolumeQuiet);
		table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),KVodafoneSC);
		table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),1440ul);
		table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),EFalse);
		}
	else if (ch=='b' || ch=='B')
		{
// Cabled 9110, etc
		table2->WriteTextL(TPtrC(COMMDB_NAME),_L("GSM Mobile Phone via Serial"));
		TBuf<9> commPort;
		commPort.Format(_L("COMM::%c"),acommPort);
		table2->WriteTextL(TPtrC(MODEM_PORT_NAME),commPort);
		table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("ECUART"));
		table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("GSMBSC"));
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
		table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),KVodafoneSC);
		table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),1440ul);
		table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),EFalse);
		}
	else if (ch=='c' || ch=='C')
		{
// Cabled 9110, etc
		table2->WriteTextL(TPtrC(COMMDB_NAME),_L("Test Mobile Phone"));
		TBuf<9> commPort;
		commPort.Format(_L("COMM::%c"),acommPort);
		table2->WriteTextL(TPtrC(MODEM_PORT_NAME),_L("DUMMY::0"));
		table2->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("DUMMY"));
		table2->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("T_REG"));
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
		table2->WriteTextL(TPtrC(MODEM_MESSAGE_CENTRE_NUMBER),KVodafoneSC);
		table2->WriteUintL(TPtrC(MODEM_MESSAGE_VALIDITY_PERIOD),1440ul);
		table2->WriteBoolL(TPtrC(MODEM_MESSAGE_DELIVERY_REPORT),EFalse);
		}
	else
		User::Invariant();
	CHECKPOINT(table2->PutRecordChanges()==KErrNone);
	CleanupStack::PopAndDestroy();
#endif

    }


LOCAL_C void PopulateTemplatesL()
    {
    CCommsDbTemplateRecord* temp1=CCommsDbTemplateRecord::NewL(TheDb,TPtrC(MODEM));
    CleanupStack::PushL(temp1);
    temp1->Modify();

    temp1->WriteTextL(TPtrC(COMMDB_NAME),_L("Default Modem"));
    temp1->WriteTextL(TPtrC(MODEM_PORT_NAME),_L("COMM::0"));
    temp1->WriteTextL(TPtrC(MODEM_CSY_NAME),_L("ECUART"));
    temp1->WriteTextL(TPtrC(MODEM_TSY_NAME),_L("GSMBSC"));
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

    CHECKPOINT(temp1->StoreModifications()==KErrNone);
    CleanupStack::PopAndDestroy();

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

    CHECKPOINT(temp2->StoreModifications()==KErrNone);
    CleanupStack::PopAndDestroy();

    CCommsDbTemplateRecord* temp3=CCommsDbTemplateRecord::NewL(TheDb,TPtrC(DIAL_OUT_ISP));
    CleanupStack::PushL(temp3);
    temp3->Modify();

    temp3->WriteTextL(TPtrC(COMMDB_NAME),_L("Defaul Dial Out ISP"));
    temp3->WriteBoolL(TPtrC(ISP_DIAL_RESOLUTION),ETrue);
    temp3->WriteBoolL(TPtrC(ISP_PROMPT_FOR_LOGIN),ETrue);
    temp3->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
    temp3->WriteBoolL(TPtrC(ISP_DISPLAY_PCT),EFalse);
    temp3->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
    temp3->WriteBoolL(TPtrC(ISP_IF_PROMPT_FOR_AUTH),ETrue);
    temp3->WriteBoolL(TPtrC(ISP_IF_CALLBACK_ENABLED),EFalse);
    temp3->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
    temp3->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
    temp3->WriteBoolL(TPtrC(ISP_ENABLE_IP_HEADER_COMP),EFalse);
    temp3->WriteBoolL(TPtrC(ISP_ENABLE_LCP_EXTENSIONS),EFalse);
    temp3->WriteBoolL(TPtrC(ISP_DISABLE_PLAIN_TEXT_AUTH),EFalse);
    temp3->WriteBoolL(TPtrC(ISP_ENABLE_SW_COMP),EFalse);

    CHECKPOINT(temp3->StoreModifications()==KErrNone);
    CleanupStack::PopAndDestroy();

    CCommsDbTemplateRecord* temp4=CCommsDbTemplateRecord::NewL(TheDb,TPtrC(DIAL_IN_ISP));
    CleanupStack::PushL(temp4);
    temp4->Modify();

    temp4->WriteTextL(TPtrC(COMMDB_NAME),_L("Defaul Dial In ISP"));
    temp4->WriteBoolL(TPtrC(ISP_USE_LOGIN_SCRIPT),EFalse);
    temp4->WriteTextL(TPtrC(ISP_IF_NAME),_L("ppp"));
    temp4->WriteBoolL(TPtrC(ISP_IP_ADDR_FROM_SERVER),ETrue);
    temp4->WriteBoolL(TPtrC(ISP_IP_DNS_ADDR_FROM_SERVER),ETrue);
    temp4->WriteBoolL(TPtrC(ISP_ENABLE_IP_HEADER_COMP),EFalse);
    temp4->WriteBoolL(TPtrC(ISP_ENABLE_LCP_EXTENSIONS),EFalse);
    temp4->WriteBoolL(TPtrC(ISP_DISABLE_PLAIN_TEXT_AUTH),EFalse);
    temp4->WriteBoolL(TPtrC(ISP_ENABLE_SW_COMP),EFalse);

    CHECKPOINT(temp4->StoreModifications()==KErrNone);
    CleanupStack::PopAndDestroy();

    CCommsDbTemplateRecord* temp5=CCommsDbTemplateRecord::NewL(TheDb,TPtrC(WAP_ACCESS_POINT));
    CleanupStack::PushL(temp5);
    temp5->Modify();

    temp5->WriteTextL(TPtrC(COMMDB_NAME),_L("Defaul Dial In ISP"));
    temp5->WriteTextL(TPtrC(WAP_CURRENT_BEARER),TPtrC(WAP_IP_BEARER));
    temp5->WriteTextL(TPtrC(WAP_START_PAGE),_L(""));

    CHECKPOINT(temp5->StoreModifications()==KErrNone);
    CleanupStack::PopAndDestroy();

    CCommsDbTemplateRecord* temp6=CCommsDbTemplateRecord::NewL(TheDb,TPtrC(WAP_IP_BEARER));
    CleanupStack::PushL(temp6);
    temp6->Modify();

    temp6->WriteUintL(TPtrC(WAP_ACCESS_POINT_ID),0ul);
    temp6->WriteTextL(TPtrC(WAP_GATEWAY_ADDRESS),_L(""));
    temp6->WriteUintL(TPtrC(WAP_IAP),0);
//  temp6->WriteUintL(TPtrC(WAP_ISP),0);
//  temp6->WriteUintL(TPtrC(WAP_LOCATION),0);
//    temp6->WriteUintL(TPtrC(WAP_CHARGECARD),0);
//  temp6->WriteUintL(TPtrC(WAP_ISP_TYPE),0);
    temp6->WriteUintL(TPtrC(WAP_WSP_OPTION),0);
    temp6->WriteBoolL(TPtrC(WAP_SECURITY),EFalse);

    CHECKPOINT(temp6->StoreModifications()==KErrNone);
    CleanupStack::PopAndDestroy();

    CCommsDbTemplateRecord* temp7=CCommsDbTemplateRecord::NewL(TheDb,TPtrC(WAP_SMS_BEARER));
    CleanupStack::PushL(temp7);
    temp7->Modify();

    temp7->WriteUintL(TPtrC(WAP_ACCESS_POINT_ID),0ul);
    temp7->WriteTextL(TPtrC(WAP_GATEWAY_ADDRESS),_L(""));
    temp7->WriteTextL(TPtrC(WAP_SERVICE_CENTRE_ADDRESS),_L(""));
    temp7->WriteUintL(TPtrC(WAP_WSP_OPTION),0);
    temp7->WriteBoolL(TPtrC(WAP_SECURITY),EFalse);

    CHECKPOINT(temp7->StoreModifications()==KErrNone);
    CleanupStack::PopAndDestroy();
    }


LOCAL_C TUint32 CreateISPTableL()
/**
 *  Populate the ISP table.
 *  The presence of this table in the database is important, but the values are not.
 */
    {
    TUint32 dialOutId;
    CCommsDbTableView* table=TheDb->OpenTableLC(TPtrC(DIAL_OUT_ISP));
    CHECKPOINT(table->InsertRecord(dialOutId)==KErrNone);
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

    CHECKPOINT(table->PutRecordChanges()==KErrNone);
    CleanupStack::PopAndDestroy();
    return dialOutId;
    }
