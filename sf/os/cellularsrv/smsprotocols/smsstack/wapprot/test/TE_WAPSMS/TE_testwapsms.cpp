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
//

/**
 @file
*/

#include "TE_wapsmsbase.h"
#include "TE_testwapsms.h"
#include "wap_sock.h"

TVerdict CTestTxRx7DCS::doTestStepL()
/*
 * implementation of Send and Receive 7bit wap message test case
 */
{
	INFO_PRINTF1(_L("TE_WAPSMS Send and Receive 7bit wap message TEST - Starting ..."));
	INFO_PRINTF1(_L("\n"));

	INFO_PRINTF1(_L("Send and Receive 7 Bit Business Card"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket sock1;
    TInt ret=sock1.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();
	
	TWapAddr recvWapAddr;
	TBuf8<256> recvBuf;
	TRequestStatus recvStatus;
	sock1.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	//
	// Sending a datagram
	//
	TWapAddr wapAddr;
	TRequestStatus status;
	wapAddr.SetWapPort(TWapPortNumber(226));
    TBuf8<100> buf8;
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	ret = sock1.Bind(wapAddr);
	TESTL(ret == KErrNone);

	sock1.SendTo(KWapTestMsg,wapAddr,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),KWapTestMsg().Length());
	
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone);
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK\n"),recvBuf.Length());
	Print8BitDes(recvBuf);
	TESTL(recvBuf == KWapTestMsg());
	
#ifdef EKA2
	// Allow WaitForRequest to complete for EKA2
	User::After(200000); 
#endif
	
	
	
	sock1.Close();
    CleanupStack::PopAndDestroy(&socketServer);

	return TestStepResult();
}


TVerdict CTestTxRx8DCS::doTestStepL()
/*
 * implementation of Send and Receive  8bit wap message test case
 */
{
	INFO_PRINTF1(_L("TE_WAPSMS Send and Receive 8bit wap message TEST - Starting ..."));
	INFO_PRINTF1(_L("\n"));

	INFO_PRINTF1(_L("Send and Receive 8 Bit business card"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket sock1;
	TInt ret=sock1.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();

	TWapSmsDataCodingScheme codingScheme = EWapSms8BitDCS;
	ret = sock1.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,codingScheme);
	TESTL(ret == KErrNone);

	TWapAddr recvWapAddr;
	TBuf8<256> recvBuf;
	TRequestStatus recvStatus;
	sock1.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	//
	// Sending a datagram
	//	
	TWapAddr wapAddr;
	TRequestStatus status;
	wapAddr.SetWapPort(TWapPortNumber(226));
    TBuf8<100> buf8;
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	ret=sock1.Bind(wapAddr);
	TESTL(ret == KErrNone);

	sock1.SendTo(KWapTestMsg,wapAddr,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),KWapTestMsg().Length());

	//
	// Receiving a datagram
	//
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone);
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK\n"),recvBuf.Length());
	Print8BitDes(recvBuf);
	TESTL(recvBuf == KWapTestMsg());
	
#ifdef EKA2
	// Allow WaitForRequest to complete for EKA2
	User::After(200000); 
#endif
	
	
	sock1.Close();
    CleanupStack::PopAndDestroy(&socketServer);

	return TestStepResult();
}


TVerdict CTestTxRxRingTone::doTestStepL()
/*
 * implementation of Send and Receive ringtone test case
 */
{
	INFO_PRINTF1(_L("TE_WAPSMS Send and Receive ringtone TEST - Starting ..."));
	INFO_PRINTF1(_L("\n"));

	INFO_PRINTF1(_L("Test5 - Send a ringtone"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket sock1;
	TInt ret=sock1.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();

	TWapSmsDataCodingScheme codingScheme = EWapSms8BitDCS;
	ret = sock1.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,codingScheme);
	TESTL(ret == KErrNone);

	TWapAddr recvWapAddr;
	TBuf8<256> recvBuf;
	TRequestStatus recvStatus;
	sock1.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	//
	// Receiving a datagram
	//
    TWapAddr wapAddr;
	TRequestStatus status;
	TWapPortNumber ringTone = TWapPortNumber(5505);
	wapAddr.SetWapPort(ringTone);
    TBuf8<100> buf8;
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	ret=sock1.Bind(wapAddr);
	TESTL(ret == KErrNone);

	TBuf8<128> ringToneBuf(30);
	ringToneBuf[0] = 0x02;  ringToneBuf[10] = 0x94;  ringToneBuf[20] = 0x27;
	ringToneBuf[1] = 0x4A;  ringToneBuf[11] = 0x04;  ringToneBuf[21] = 0x02;
	ringToneBuf[2] = 0x3A;  ringToneBuf[12] = 0x00;  ringToneBuf[22] = 0x30;
	ringToneBuf[3] = 0x5D;  ringToneBuf[13] = 0x19;  ringToneBuf[23] = 0x49;
	ringToneBuf[4] = 0x29;  ringToneBuf[14] = 0x20;  ringToneBuf[24] = 0xC8;
	ringToneBuf[5] = 0x85;  ringToneBuf[15] = 0xA2;  ringToneBuf[25] = 0x1A;
	ringToneBuf[6] = 0xB1;  ringToneBuf[16] = 0xB0;  ringToneBuf[26] = 0xF6;
	ringToneBuf[7] = 0x51;  ringToneBuf[17] = 0x27;  ringToneBuf[27] = 0xF0;
	ringToneBuf[8] = 0xD5;  ringToneBuf[18] = 0x02;  ringToneBuf[28] = 0x00;
	ringToneBuf[9] = 0xB9;  ringToneBuf[19] = 0xAC;  ringToneBuf[29] = 0x5B;

	sock1.SendTo(ringToneBuf,wapAddr,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),ringToneBuf.Length());

	//
	// Receiving a datagram
	//
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone);
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK\n"),recvBuf.Length());
	Print8BitDes(recvBuf);
	TESTL(recvBuf == ringToneBuf);
	
#ifdef EKA2
	// Allow WaitForRequest to complete for EKA2
	User::After(400000); 
#endif
	
	
	sock1.Close();
    CleanupStack::PopAndDestroy(&socketServer);

	return TestStepResult();
}


TVerdict CTestTxRxWap::doTestStepL()
    {
	INFO_PRINTF1(_L("Send and Receive Wap SMS"));

    const TUint KStdDelimiter=',';  
    TInt             port;
    TInt             dcs;
    TInt             count = 0;

    CTestConfig* configFile;
    const CTestConfigSection* cfgFile;
    configFile=CTestConfig::NewLC(iFs,KConfigFileDir,KConfigFilename);

    // end of construction

    TESTL(((cfgFile = configFile->Section(KSendRecvWapSms))!=NULL));
    TESTL(count=cfgFile->ItemCount(KPort));

    const CTestConfigItem* item=NULL;
    TInt i, ret=KErrNone;
    for(i=0;i<count;i++)
        {
        item=cfgFile->Item(KPort,i);
        if(!item)
            break;
        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,port);
        if(ret!=KErrNone)
            continue;

        item=cfgFile->Item(KDCS,i);
        if(!item)
            break;
        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,dcs);
        if(ret!=KErrNone)
            continue;

        TPtrC8  filename; 
        item=cfgFile->Item(KFile,i);
        if(!item)
            break;
        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,filename);
        if(ret!=KErrNone)
            continue;

        // 
        // wapprot function call has to be here
        //
	    HBufC8* textBuf=HBufC8::New(4096);
	    CleanupStack::PushL(textBuf);
	    TPtr8 textPtr=textBuf->Des();

        TRAPD(readFileErr, ReadFileL(filename, textPtr));
        if (readFileErr != KErrNone)
            {
           	INFO_PRINTF1(_L("Can't find the test file"));
    	    CleanupStack::PopAndDestroy(textBuf);
            continue;
            }

        INFO_PRINTF2(_L("[test%02d]\n"), i+1 );

        TWapPortNumber wpn = TWapPortNumber(port);
        TWapSmsDataCodingScheme coding;
        if( dcs == 7 )
            coding = EWapSms7BitDCS;
        else
            coding = EWapSms8BitDCS;

        DoTestTxRxWapL(textPtr, wpn, coding);

	    CleanupStack::PopAndDestroy(textBuf);
        }
    // end of the parsing 

    // beginning of the destruction 
    CleanupStack::PopAndDestroy(configFile);//configFile
    return EPass;
    }


TVerdict CTestWapAndApplicationPortBinding::doTestStepL()
/*
 * implementation of Binding via wap and application port addressing test 
 * case
 */
{
	INFO_PRINTF1(_L("WAP binding"));

	RSocketServ socketServer;
    
	RSocket wapsock,wapsock2,wapsock3,socket8,socket16;
	TInt port(245),port16big(2245);
    TSmsAddr addr8,addr16;
	TInt ret=KErrNone;
	TWapAddr wapAddr;	
	TBuf8<100> buf8;	

	Connect2EsockLC(socketServer);
	
	//
	// 000 - bind 3 sockets to unspecified ports		
	
    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone );
		
	ret=wapsock2.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone );
		
	ret=wapsock3.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone );
		
	//WaitForInitializeL();

	// bind them to ports		
	
	wapAddr.SetWapPort(EWapPortUnspecified);   
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);

	ret = wapsock.Bind(wapAddr);
	TEST(ret==KErrNone);
	ret = wapsock2.Bind(wapAddr);
	TEST(ret==KErrNone);
	ret = wapsock3.Bind(wapAddr);
	TEST(ret==KErrNone);

	wapsock.Close();
	wapsock2.Close();
	wapsock3.Close();

	//
	// 007 - bind socket to 8-bit port, bind wap port to the same port		
	
    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone );
	
	addr8.SetSmsAddrFamily(ESmsAddrApplication8BitPort);	
	addr8.SetPort(port);
		
	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);	
	
	WaitForInitializeL();

	// bind them to ports		
	
	wapAddr.SetWapPort(TWapPortNumber(port));   
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	ret = wapsock.Bind(wapAddr);
	TEST(ret==KErrNone);	

	ret = socket8.Bind(addr8);
	TESTL(ret!=KErrInUse);
	
	wapsock.Close();
	socket8.Close();

	//
	// 008 - bind socket to 16-bit port, bind wap port to the same port	

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone );
	
	addr16.SetSmsAddrFamily(ESmsAddrApplication16BitPort);	
	addr16.SetPort(port);
		
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);		
	
	// bind them to ports	
	wapAddr.SetWapPort(TWapPortNumber(port));   
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	ret = wapsock.Bind(wapAddr);
	TEST(ret==KErrNone);

	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);
	
	wapsock.Close();
	socket16.Close();

	//
	// 009 - bind socket to 16-bit port, bind wap port to the same port	

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone );
	
	addr16.SetSmsAddrFamily(ESmsAddrApplication16BitPort);	
	addr16.SetPort(port16big);
		
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);		
	
	WaitForInitializeL();

	// bind to port
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	
	wapAddr.SetWapPort(TWapPortNumber(port16big));   
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	ret = wapsock.Bind(wapAddr);
	TEST(ret==KErrInUse);
	
	wapsock.Close();
	socket16.Close();

	//
	// 010 - bind wap port , then  bind socket to the same 8-bit port	

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone );
	
	addr8.SetSmsAddrFamily(ESmsAddrApplication8BitPort);	
	addr8.SetPort(port);
				
	
	wapAddr.SetWapPort(TWapPortNumber(port));   
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	ret = wapsock.Bind(wapAddr);
	TESTL(ret == KErrNone );

	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);		

	// bind it to port
	ret = socket8.Bind(addr8);
	TEST(ret==KErrAlreadyExists);
	
	
	wapsock.Close();
	socket8.Close();

	//
	// 011 - bind wap port , then  bind socket to the same 16-bit port	

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TEST(ret==KErrNone);
	
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);		
	

	addr16.SetSmsAddrFamily(ESmsAddrApplication16BitPort);	
	addr16.SetPort(port16big);
				
	wapAddr.SetWapPort(TWapPortNumber(port16big));   
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	ret = wapsock.Bind(wapAddr);

	TESTL(ret == KErrNone );

	// bind them to ports	
	ret = socket16.Bind(addr16);
	TEST(ret==KErrAlreadyExists);
	
	wapsock.Close();
	socket16.Close();

	// 012 - bind wap port , then  bind socket to the same 16-bit port	( less than 256)

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone );
	
	addr16.SetSmsAddrFamily(ESmsAddrApplication16BitPort);	
	addr16.SetPort(port);
				
	
	wapAddr.SetWapPort(TWapPortNumber(port));   
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	ret = wapsock.Bind(wapAddr);
	TESTL(ret == KErrNone );

	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);		

	// bind it
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);
	
	wapsock.Close();
	socket16.Close();
    CleanupStack::PopAndDestroy(&socketServer);

	return TestStepResult();
}

TVerdict CTestSendAndRecvViaAppllicationPortAndWap::doTestStepL()
/*
 * implementation of Sending and receiving via application port addressing 
 * and wap test case
 */
{
 	INFO_PRINTF1(_L("Sending and receiving via application port addressing and wap"));

	RSocketServ socketServer;    
	RSocket wapsock,wapsockother,socket8,socket16,socketany;
	TInt port(245),portother(255),port16big(2245),port16other(11111);
    TSmsAddr addr8,addr16,addrany,addr16big,addr8other,addr16other;
	TInt ret=KErrNone;
	TWapAddr wapAddr,wapAddrOther,recvWapAddr,wapAddrBig,wapAddrZero;
	TBuf8<100> buf8;
	TBuf8<256> recvBuf;	
	TRequestStatus recvStatus,status;
	CSmsMessage* sendMessage=NULL,*recvMessage=NULL;		
	TBuf8<128> buf;
	TBuf16<128> buf16;
	
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	addr8.SetSmsAddrFamily(ESmsAddrApplication8BitPort);	
	addr16.SetSmsAddrFamily(ESmsAddrApplication16BitPort);
	addrany.SetSmsAddrFamily(ESmsAddrRecvAny);
	addr8other.SetSmsAddrFamily(ESmsAddrApplication8BitPort);	
	addr16big.SetSmsAddrFamily(ESmsAddrApplication16BitPort);
	addr16other.SetSmsAddrFamily(ESmsAddrApplication16BitPort);
	addr8.SetPort(port);
	addr8other.SetPort(portother);
	addr16.SetPort(port);
	addr16big.SetPort(port16big);
	addr16other.SetPort(port16other);

	buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	wapAddr.SetWapPort(TWapPortNumber(port));

	wapAddrOther.SetWapAddress(buf8);	
	wapAddrOther.SetWapPort(TWapPortNumber(portother));

	wapAddrBig.SetWapAddress(buf8);	
	wapAddrBig.SetWapPort(TWapPortNumber(port16big));

	wapAddrZero.SetWapAddress(buf8);	
	wapAddrZero.SetWapPort(TWapPortNumber(0));



	Connect2EsockLC(socketServer);

	// 029 send from a wap socket to another wap socket ( ports <= 255)    

	buf.Format(_L8("029 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	ret=wapsockother.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();		
       
	ret = wapsock.Bind(wapAddr);
	TESTL(ret == KErrNone);

	
	ret = wapsockother.Bind(wapAddrOther);
	TESTL(ret == KErrNone);

	wapsockother.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	// Sending a datagram	
	
	wapsock.SendTo(buf,wapAddrOther,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),buf.Length());

	// Receiving a datagram
	
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone);
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK\n"),recvBuf.Length());
	Print8BitDes(recvBuf);
	TEST(recvBuf==buf);
	wapsock.Close();
	wapsockother.Close();

	// 030 send from a wap socket to another wap socket ( port > 255)    

	buf.Format(_L8("030 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	ret=wapsockother.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();		
   
	ret = wapsock.Bind(wapAddr);
	TESTL(ret == KErrNone);

	ret = wapsockother.Bind(wapAddrBig);
	TESTL(ret == KErrNone);

	wapsockother.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	// Sending a datagram	
	
	wapsock.SendTo(buf,wapAddrBig,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),buf.Length());

	// Receiving a datagram
	
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone);
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK\n"),recvBuf.Length());
	Print8BitDes(recvBuf);
	TEST(recvBuf==buf);
	wapsock.Close();
	wapsockother.Close();


	// 031 send from a wap socket to itself ( port <= 255)    

	buf.Format(_L8("031 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	

	WaitForInitializeL();		    
	ret = wapsock.Bind(wapAddr);
	TESTL(ret == KErrNone);


	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	// Sending a datagram	
	
	wapsock.SendTo(buf,wapAddr,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),buf.Length());

	// Receiving a datagram
	
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int()  == KErrNone);
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK\n"),recvBuf.Length());
	Print8BitDes(recvBuf);
	TEST(recvBuf==buf);
	wapsock.Close();

	// 032 send to itself ( port > 255)    

	buf.Format(_L8("032 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);	

	WaitForInitializeL();		
   	
	ret = wapsock.Bind(wapAddrBig);
	TESTL(ret == KErrNone);

	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	// Sending a datagram	
	
	wapsock.SendTo(buf,wapAddrBig,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),buf.Length());

	// Receiving a datagram
	
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone );
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK\n"),recvBuf.Length());
	Print8BitDes(recvBuf);
	TEST(recvBuf==buf);
	wapsock.Close();
	
	// 033 send from a wap socket to any addr recv  socket

	buf.Format(_L8("033 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	ret=socketany.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();		
   
	ret = socketany.Bind(addrany);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);
	CleanupClosePushL(socketany);
	
	// Sending a datagram	
	
	wapsock.SendTo(buf,wapAddrBig,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),buf.Length());

	// Receiving a datagram	
	WaitForRecvL(socketany);	

	recvMessage = RecvSmsL(socketany);
	CleanupStack::PushL(recvMessage);

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(&socketany);
	CleanupStack::PopAndDestroy(&wapsock);
	
	
	// 034 send from a wap socket to 8-bit application port bound  socket

	buf.Format(_L8("034 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	ret=socket8.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();		   
	ret = socket8.Bind(addr8);
	TESTL(ret == KErrNone);

	ret = wapsock.Bind(wapAddrOther);
	TESTL(ret == KErrNone);

	ret = wapsock.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,EWapSms8BitDCS);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);
	CleanupClosePushL(socket8);
	
	// Sending a datagram	
	wapsock.SendTo(buf,wapAddr,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),buf.Length());
	// Receiving a datagram	
	WaitForRecvL(socket8);	

	recvMessage = RecvSmsL(socket8);
	CleanupStack::PushL(recvMessage);
	CleanupStack::PopAndDestroy(recvMessage);	
	CleanupStack::PopAndDestroy(&socket8);
	CleanupStack::PopAndDestroy(&wapsock);
	

	// 035 send from a wap socket to 16-bit application port bound  socket

	buf.Format(_L8("035 - SendingAndReceivingViaAppllicationPortAndWap Msg"));
    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	
	ret=socket16.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();		
    	
	ret = socket16.Bind(addr16big);
	TESTL(ret == KErrNone);

	ret = wapsock.Bind(wapAddrOther);
	TESTL(ret == KErrNone);

	ret = wapsock.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,EWapSms8BitDCS);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);
	CleanupClosePushL(socket16);
	
	// Sending a datagram	
	
	wapsock.SendTo(buf,wapAddrBig,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),buf.Length());
	// Receiving a datagram	
	WaitForRecvL(socket16);	

	recvMessage = RecvSmsL(socket16);
	CleanupStack::PushL(recvMessage);
	CleanupStack::PopAndDestroy(recvMessage);	
	CleanupStack::PopAndDestroy(&socket16);
	CleanupStack::PopAndDestroy(&wapsock);
	
	// 036 send from a wap socket to any addr recv bound  socket,
	//     in presence of another socket bound to the same 16-bit port
	//     and  socket bounfd to other 8-bit port

	buf.Format(_L8("036 - SendingAndReceivingViaAppllicationPortAndWap Msg"));
    
    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	
	ret=socket8.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	ret=socket16.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	ret=socketany.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();		

	ret = wapsock.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,EWapSms8BitDCS);
	TESTL(ret == KErrNone);


	ret = socket8.Bind(addr8other);
	TESTL(ret == KErrNone);

	ret = socket16.Bind(addr16);
	TESTL(ret == KErrNone);

	ret = socketany.Bind(addrany);
	TESTL(ret == KErrNone);


	CleanupClosePushL(wapsock);
	CleanupClosePushL(socket8);
	CleanupClosePushL(socket16);
	CleanupClosePushL(socketany);
	
	// Sending a datagram	
	
	wapsock.SendTo(buf,wapAddr,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),buf.Length());
	// Receiving a datagram	
	WaitForRecvL(socketany);	

	recvMessage = RecvSmsL(socketany);
	CleanupStack::PushL(recvMessage);
	CleanupStack::PopAndDestroy(recvMessage);	
	CleanupStack::PopAndDestroy(&socketany);
	CleanupStack::PopAndDestroy(&socket16);
	CleanupStack::PopAndDestroy(&socket8);
	CleanupStack::PopAndDestroy(&wapsock);
	
	// 037 send from a wap socket to any addr recv socket in presence of
	// socket bound to other 16-bit application port and
	// another socket bound to other 8-bit port

	buf.Format(_L8("037 - SendingAndReceivingViaAppllicationPortAndWap Msg"));
    
    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	ret=socket8.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	ret=socket16.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	ret=socketany.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);


	WaitForInitializeL();		
       		

	ret = wapsock.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,EWapSms8BitDCS);
	TESTL(ret == KErrNone);

	ret = socket8.Bind(addr8);
	TESTL(ret == KErrNone);

	ret = socket16.Bind(addr16other);
	TESTL(ret == KErrNone);

	ret = socketany.Bind(addrany);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);
	CleanupClosePushL(socket8);
	CleanupClosePushL(socket16);
	CleanupClosePushL(socketany);

	// Sending a datagram	
	
	wapsock.SendTo(buf,wapAddrBig,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),buf.Length());
	// Receiving a datagram	
	WaitForRecvL(socketany);	

	recvMessage = RecvSmsL(socketany);
	CleanupStack::PushL(recvMessage);
	CleanupStack::PopAndDestroy(recvMessage);
	
	CleanupStack::PopAndDestroy(&socketany);
	CleanupStack::PopAndDestroy(&socket16);
	CleanupStack::PopAndDestroy(&socket8);
	CleanupStack::PopAndDestroy(&wapsock);


	// 038 send from recv any  addr bound socket to wap socket <=255	
	buf16.Format(_L("038 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	ret=socketany.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	WaitForInitializeL();		
       
	
	ret = socketany.Bind(addrany);
	TESTL(ret == KErrNone);

	ret = wapsock.Bind(wapAddr);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);	
	CleanupClosePushL(socketany);
	
	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);
	// Sending a message
	sendMessage=CreateSmsMessageL(buf16,alphabet);

	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, EFalse);

	// send SMS message
	SendSmsL(sendMessage,socketany);
	// Receiving a datagram
	//
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone );

		
	CleanupStack::PopAndDestroy(sendMessage);	
	CleanupStack::PopAndDestroy(&socketany);	
	CleanupStack::PopAndDestroy(&wapsock);

	// 039 send from recv any addr bound socket to wap socket > 255	
	buf16.Format(_L("039 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	ret=socketany.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	WaitForInitializeL();		
       	
	ret = socketany.Bind(addrany);
	TESTL(ret == KErrNone);

	ret = wapsock.Bind(wapAddrBig);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);	
	CleanupClosePushL(socketany);
	
	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);
	// Sending a message	
	buf.Format(buf);
	sendMessage=CreateSmsMessageL(buf16,alphabet);

	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port16big, port, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socketany);
	// Receiving a datagram
	//
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone );

		
	CleanupStack::PopAndDestroy(sendMessage);	
	CleanupStack::PopAndDestroy(&socketany);	
	CleanupStack::PopAndDestroy(&wapsock);

	// 040 send from recv any addr bound socket to 16-bit port <= 255 to itself
	// in presence of wap socket bound to the same port, now received on wap socket due to
	// modification of LookupSAP to allow <= 255 on 16-bit port 
	
	buf16.Format(_L("040 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	ret=socketany.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);
		
	WaitForInitializeL();		    
	
	ret = socketany.Bind(addrany);
	TESTL(ret == KErrNone);

	ret = wapsock.Bind(wapAddr);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);	
	CleanupClosePushL(socketany);

	// Sending a message		
	sendMessage=CreateSmsMessageL(buf16,alphabet);

	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socketany);
	// Receiving a datagram
	//	

	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone );
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK\n"),recvBuf.Length());
	Print8BitDes(recvBuf);
	wapsock.Close();

	CleanupStack::PopAndDestroy(sendMessage);	
	CleanupStack::PopAndDestroy(&socketany);	
	CleanupStack::PopAndDestroy(&wapsock);
	
	// 041 send socket bound to some 8-bit port to port d <= 255 to 
	// wap socket bound to port d

	buf16.Format(_L("041 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	ret=socket8.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	
	WaitForInitializeL();		
       	
	ret = socket8.Bind(addr8other);
	TESTL(ret == KErrNone);

	ret = wapsock.Bind(wapAddr);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);	
	CleanupClosePushL(socket8);

	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	// Sending a message	
	sendMessage=CreateSmsMessageL(buf16,alphabet);

	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 8 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, portother, EFalse);

	// send SMS message
	SendSmsL(sendMessage,socket8);
	// Receiving a datagram	
	
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone );
	

	CleanupStack::PopAndDestroy(sendMessage);	
	CleanupStack::PopAndDestroy(&socket8);	
	CleanupStack::PopAndDestroy(&wapsock);
	
	// 042 send socket bound to 8-bit port to port d > 255 to 
	// wap socket bound to port d
	
	buf16.Format(_L("042 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	ret=socket8.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	
	WaitForInitializeL();		
       	
	ret = socket8.Bind(addr8);
	TESTL(ret == KErrNone);

	ret = wapsock.Bind(wapAddrBig);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);	
	CleanupClosePushL(socket8);

	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	// Sending a message	
	sendMessage=CreateSmsMessageL(buf16,alphabet);

	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 8 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port16big, port, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socket8);
	// Receiving a datagram
	//
	// Receiving a datagram
	//
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone );
	

	CleanupStack::PopAndDestroy(sendMessage);	
	CleanupStack::PopAndDestroy(&socket8);	
	CleanupStack::PopAndDestroy(&wapsock);
	
	// 043 send from socket bound to 16-bit port to port d <= 255 to 
	// wap socket bound to port d

	buf16.Format(_L("043 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	ret=socket16.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	
	WaitForInitializeL();		
       	
	ret = socket16.Bind(addr16);
	TESTL(ret == KErrNone);

	ret = wapsock.Bind(wapAddr);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);	
	CleanupClosePushL(socket16);

	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	// Sending a message		
	sendMessage=CreateSmsMessageL(buf16,alphabet);

	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 8 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, EFalse);

	// send SMS message
	SendSmsL(sendMessage,socket16);
	// Receiving a datagram
	//	
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone );
	
	CleanupStack::PopAndDestroy(sendMessage);	
	CleanupStack::PopAndDestroy(&socket16);	
	CleanupStack::PopAndDestroy(&wapsock);
	
	// 044 send from socket bound to 16-bit port to port d >= 255 to 
	// wap socket bound to port d

	buf16.Format(_L("044 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	ret=socket16.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);
		
	WaitForInitializeL();		
       	
	ret = socket16.Bind(addr16other);
	TESTL(ret == KErrNone);

	ret = wapsock.Bind(wapAddrBig);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);	
	CleanupClosePushL(socket16);

	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	// Sending a message	
	sendMessage=CreateSmsMessageL(buf16,alphabet);

	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port16big, portother, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socket16);
	// Receiving a datagram
	//	
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone );
	

	CleanupStack::PopAndDestroy(sendMessage);	
	CleanupStack::PopAndDestroy(&socket16);	
	CleanupStack::PopAndDestroy(&wapsock);

	// 045 send from a wap socket to port 0, now possible with wapsock due to modification of LookupSAP to 
	// allow 16-bit ports <= 255

	buf.Format(_L8("045 - SendingAndReceivingViaAppllicationPortAndWap Msg"));

    ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	ret=socketany.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();		
   
	ret = socketany.Bind(addrany);
	TESTL(ret == KErrNone);
	
	CleanupClosePushL(wapsock);
	CleanupClosePushL(socketany);
	
	// Sending a datagram	
	
	wapsock.SendTo(buf,wapAddrZero,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),buf.Length());

	// Receiving a datagram	
	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone );
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK\n"),recvBuf.Length());
	Print8BitDes(recvBuf);
	TEST(recvBuf==buf);

	// now the same with app port addressing 
	
	ret = wapsock.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,EWapSms8BitDCS);
	TESTL(ret == KErrNone);

	wapsock.SendTo(buf,wapAddrZero,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),buf.Length());

	// Receiving a datagram	
	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone );
	INFO_PRINTF2(_L("Recv datagram length: %d on socket OK\n"),recvBuf.Length());
	Print8BitDes(recvBuf);
	TEST(recvBuf==buf);

	CleanupStack::PopAndDestroy(&socketany);
	CleanupStack::PopAndDestroy(&wapsock);
	
	
	// 038 send from recv any  addr bound socket to wap socket <=255	
	buf16.Format(_L("//SCKL55805580C20201\r\nAAAAAAAAAAAAAAAAAAAAAA"));

	ret=wapsock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	ret=socketany.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	
	WaitForInitializeL();		
       
	
	ret = socketany.Bind(addrany);
	TESTL(ret == KErrNone);
	
	wapAddr.SetWapPort(TWapPortNumber(245));
	ret = wapsock.Bind(wapAddr);
	TESTL(ret == KErrNone);

	CleanupClosePushL(wapsock);	
	CleanupClosePushL(socketany);
	
	wapsock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);
	// Sending a message
	sendMessage=CreateSmsMessageL(buf16,alphabet);

	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, EFalse);

	// send SMS message
	SendSmsL(sendMessage,socketany);
	// Receiving a datagram
	//
	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone );

		
	CleanupStack::PopAndDestroy(sendMessage);	
	CleanupStack::PopAndDestroy(&socketany);	
	CleanupStack::PopAndDestroy(&wapsock);


    CleanupStack::PopAndDestroy(&socketServer);

	return TestStepResult();
}


TVerdict CTestUnSupportedAck::doTestStepL()
/*
 * implementation of Return unsupported error to client test case
 */
{
	INFO_PRINTF1(_L("TE_WAPSMS Return unsupported error to client TEST - Starting ..."));
	INFO_PRINTF1(_L("\n"));

    RSocketServ socketServer;
    Connect2EsockLC(socketServer);
    RSocket sock1;
    TInt ret=sock1.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
    TESTL(ret == KErrNone);
 	
    WaitForInitializeL();
 	
    TRequestStatus status;
 	
    sock1.Ioctl(KIoctlReadMessageSucceeded, status, NULL);
 	
    User::WaitForRequest(status);
    TESTL(status.Int() == KErrNotSupported);
 	
    sock1.Close();
    CleanupStack::PopAndDestroy(&socketServer);

	return TestStepResult();
}


TVerdict CTestConsecutiveTx::doTestStepL()
/**
 * The test is a solution to defect DEF42065.
 * Synopsis - DEF42065 was raised because the sms stack
 * signalled to the server that it had completed sending
 * a wap message too early. This caused the client side
 * to close its socket and delete the corresponding stack
 * side observer before the message had been sent, resulting
 * in a panic when the stack tried to reference the observer
 * at completion time.
 * This defect was only detected when more than 1 SMS message
 * was sent consecutively, hence the reason for developing this
 * test case which simply checks that the stack can handle 
 * sending consecutive wap messages.
 */
	{
	INFO_PRINTF1(_L("Send and Receive 7 Bit Business Card"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket sock1;
    TInt ret=sock1.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);

	WaitForInitializeL();
	
	//
	// Sending a datagram
	//
	TWapAddr wapAddr;
	TRequestStatus status;
	wapAddr.SetWapPort(TWapPortNumber(226));
    TBuf8<100> buf8;
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);
	ret = sock1.Bind(wapAddr);
	TESTL(ret == KErrNone);

	sock1.SendTo(KWapTestMsg,wapAddr,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent datagram length: %d on socket OK\n"),KWapTestMsg().Length());

	sock1.SendTo(KWapTestMsg,wapAddr,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	
	INFO_PRINTF2(_L("Sent 2nd datagram length: %d on socket OK\n"),KWapTestMsg().Length());

	sock1.SendTo(KWapTestMsg,wapAddr,0,status);
	User::WaitForRequest(status);
	TESTL(status.Int() == KErrNone);
	INFO_PRINTF2(_L("Sent 3nd datagram length: %d on socket OK\n"),KWapTestMsg().Length());

#ifdef EKA2
    // Allow WaitForRequest to complete for EKA2
	User::After(200000); 
#endif

	sock1.Close();
    CleanupStack::PopAndDestroy(&socketServer);
	return TestStepResult();
	}

TVerdict CTestSinglePDUWapMessageWithConCatRef::doTestStepL()
	{
	INFO_PRINTF1(_L("TE_WAPSMS Send & Receive a business card with Long Header as normal text message - Starting ...\n"));
	
	//Connect to socket server
	RSocketServ socketServer;
    Connect2EsockLC(socketServer);

	//
	//	Define and open the socket to receive WAP Datagram
	//
	RSocket sock;
	TInt ret=sock.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	CleanupClosePushL(sock);

	//
	//	Define and open the socket to send sms text message.
	//
	RSocket socketany;
	ret=socketany.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TESTL(ret == KErrNone);
	CleanupClosePushL(socketany);

	//
	//	Waiting for the phone to be initialised
	//
	WaitForInitializeL();

	//
	//	Indicating to the protocol that it's a new client
	//
	INFO_PRINTF1(_L("Socket set option for indicating new client"));
	User::LeaveIfError(sock.SetOpt(KWapSmsOptionNewStyleClient,KWapSmsOptionLevel, 0));

	//
    //	Setting the port number and service center number of the wap address
	//	In this case port number is 9204, because in the below test SMS(WAP) message the
	//	destination port number is hard-coded as 9204.
	//
	TWapAddr wapAddr;
	wapAddr.SetWapPort(TWapPortNumber(9204));
    TBuf8<100> buf8;
    buf8.Copy(iTelephoneNumber);
	wapAddr.SetWapAddress(buf8);

	//
	//	Bind
	//
	ret = sock.Bind(wapAddr);
	TESTL(ret == KErrNone);

	TSmsAddr addrany;
	addrany.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret = socketany.Bind(addrany);
	TESTL(ret == KErrNone);

	//
	//	Send a WAP message as normal text message.
	//	The below WAP message contains long header with destination port, source port,
	//	reference, total segments, current segment(, other header)
	//
	TBuf16<128> msg1;
	msg1.Format(_L("//SCKL23F40000090101 \r\nBEGIN:VCARD\r\nVERSION:2.1\r\nN:Smith;Mike\r\nTEL;REF:+55512345\r\nEND:VCARD\r\n"));

	CSmsMessage* sendMessage=NULL;
	sendMessage=CreateSmsMessageL(msg1,TSmsDataCodingScheme::ESmsAlphabet7Bit);

	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(EFalse, 9204, 226, EFalse);

	// send SMS message
	SendSmsL(sendMessage,socketany);

	CleanupStack::PopAndDestroy(sendMessage);

	//
	//	Issue Ioctl for getting the length of the message
	//
	TRequestStatus ioctlStatus;
	TPckgBuf<TInt> length;
	INFO_PRINTF1(_L("Issue of IOCTL for GetLength"));
	sock.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

	//
	//	Get the size of the first incomming message
	//
	User::WaitForRequest(ioctlStatus);
	TESTL(ioctlStatus.Int() == KErrNone);
	INFO_PRINTF1(_L("IOCTL GetLength completed"));

	//
	//	Test the length of message.
	//	Length of message is 72 because the length of above message
	//	excluding header info is 72.
	//
	TESTL(length()==72);

	//
	//	Receiving a datagram
	//
	TBuf8<256> recvBuf(length());
	TWapAddr recvWapAddr;
	TRequestStatus recvStatus;
	sock.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	User::WaitForRequest(recvStatus);
	TESTL(recvStatus.Int() == KErrNone);
	INFO_PRINTF1(_L("Received the message..."));

	//
	//	Confirm the receipt of the message.
	//
	INFO_PRINTF1(_L("Socket set option for indicating receipt of message"));
	User::LeaveIfError(sock.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0));

	//
	//	Issue another Ioctl request to know whether the message has been deleted or not.
	//	In this particular case there is no other way to know whether message is deleted or not.
	//
	INFO_PRINTF1(_L("Issue of IOCTL for GetLength to confirm whether message has been deleted or not"));
	sock.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);

	RTimer timer;
	timer.CreateLocal();
	TRequestStatus timerStatus;
	TTimeIntervalMicroSeconds32 interVal = 15 * 1000000;	//15 Secs.
	timer.After(timerStatus, interVal);

	User::WaitForRequest(ioctlStatus, timerStatus);

	if (ioctlStatus.Int() == KErrNone)
		{
		//
		//	If the return value of length is 72 then assume that the previous
		//	delete request was not successful so the test has failed.
		//
		TESTL(length()!=72);
		}
	else if ((ioctlStatus.Int() == KRequestPending) ||
				(timerStatus.Int() == KErrNone))
		{
		sock.CancelIoctl();
		}
	timer.Close();

	//
	//	Clean-up
	//
	CleanupStack::PopAndDestroy(&socketany);
	CleanupStack::PopAndDestroy(&sock);
	CleanupStack::PopAndDestroy(&socketServer);

	return TestStepResult();
	}
