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
//

/**
    @file
    @test
*/

#include "WapProtSuiteStepBase.h"

#include <Gsmumsg.h>
#include <gsmubuf.h>

#include "smspver.h"
#include "WapProtSuiteDefs.h"
#include "wap_sock.h"
#include "smsstacktestconsts.h"

TVerdict CWapProtSuiteStepBase::doTestStepPreambleL()
/**
 *  @return - TVerdict
 *  Implementation of CTestStep base class virtual
 *  Do all initialisation common to derived classes in here.
 */
	{
    //base class preamble - marks the heap
    CSmsBaseTestStep::doTestStepPreambleL();
    
	return TestStepResult();
	}

TVerdict CWapProtSuiteStepBase::doTestStepPostambleL()
	{
    //base class postamble - unmarks the heap
    CSmsBaseTestStep::doTestStepPostambleL();
	
	return TestStepResult();
	}

/**
	Set up and open socket
*/
void CWapProtSuiteStepBase::SetupWapSocketL()
	{
	SetupWapSocketL(iSocket, iWapAddr, KWapPort);
	}

void CWapProtSuiteStepBase::SetupWapSocketL(RSocket& aSocket, TWapAddr& aWapAddr, const TDesC& aPort, TBool aNewStyleClient)
    {
    OpenSocketL(iSocketServer, aSocket);

    //Read port and SC number from ini file
    ReadWapPortSettingsL(aWapAddr, aPort);
    
    if (aNewStyleClient)
        {
        //  Indicating to the protocol that it's a new client
        INFO_PRINTF1(_L("Socket set option for indicating new client"));
        TInt ret = aSocket.SetOpt(KWapSmsOptionNewStyleClient,KWapSmsOptionLevel, 0);
        TESTCHECKL(ret, KErrNone, "Indicating to the protocol that it's a new client")
        }
    
    //  Bind
    TInt ret = aSocket.Bind(aWapAddr);
    TESTCHECKL(ret, KErrNone, "Socket bind");
    
    TProtocolDesc desc;
    aSocket.Info(desc);
    INFO_PRINTF2(_L("Protocol name: %S"), &desc.iName);
	
    TTimeIntervalMicroSeconds32 InitPause=9000000;  //Required Pause to Allow SMSStack to Complete its Async Init
    User::After(InitPause);                         //call to the TSY and finish its StartUp.

    }

/**
Setup a socket for receiving status repots
*/
void CWapProtSuiteStepBase::SetupStatusReportSocketL()
{
    //  Open the socket for receiving status reports
    OpenSocketL(iSocketServer, iStatusReportSocket, KSMSAddrFamily, KSMSDatagramProtocol);
    
    //Bind to the socket
    iSmsAddr.SetSmsAddrFamily(ESmsAddrStatusReport);
    TInt ret=iStatusReportSocket.Bind(iSmsAddr);
    TESTCHECKL(ret, KErrNone, "Status Report Socket bind");

    TProtocolDesc desc;
    iStatusReportSocket.Info(desc);
    INFO_PRINTF2(_L("Protocol name: %S"), &desc.iName);
    
    TTimeIntervalMicroSeconds32 InitPause=9000000;  //Required Pause to Allow SMSStack to Complete its Async Init
    User::After(InitPause);                         //call to the TSY and finish its StartUp.

}

/**
	Get WAP Address from INI file
*/
void CWapProtSuiteStepBase::ReadWapPortSettingsL(TWapAddr& aWapAddr, const TDesC& aPort)
	{
	//Local vars	
	TPtrC16 telNumber;
	TInt port;
	
	//Read Port and SC number from INI file
	User::LeaveIfError(GetStringFromConfig(ConfigSection(),KSCNumber,telNumber));
	User::LeaveIfError(GetIntFromConfig(ConfigSection(), aPort, port));

	aWapAddr.SetWapPort(TWapPortNumber(port));
	TBuf8<100> scNumber;
	scNumber.Copy(telNumber);
	TPtrC8 scAddr(scNumber);
	aWapAddr.SetWapAddress(scAddr);	
	}

/**
Get the coding of the message
*/
void CWapProtSuiteStepBase::SetCodingSchemeL()
    {
    TInt dataCoding;
    TInt err = KErrNone;

    //Get the port number from the ini file
    GetIntFromConfig(ConfigSection(),_L("dataCoding"), dataCoding);

    if(dataCoding==8)
        {
        err = iSocket.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,EWapSms8BitDCS);
        }
    else
        {
        err = iSocket.SetOpt(KWapSmsOptionNameDCS,KWapSmsOptionLevel,EWapSms7BitDCS);
        }

    TESTCHECKL(err, KErrNone, "Setting Data Coding Scheme");
    }

void CWapProtSuiteStepBase::SetMessageTypeL()
    {
    TInt err = KErrNone;
    TPtrC messageType;

    //Get the type of message
    GetStringFromConfig(ConfigSection(),_L("type"), messageType);

    //Set the message type to WapDatagram
    if(messageType.Compare(_L("Datagram")) == 0)
        {
        err = iSocket.SetOpt(KWapSmsOptionWapDatagram,KWapSmsOptionLevel);
        }
    //The creation of the message will set the type to SMART by default
    //  else
    //      User::LeaveIfError(iSocket.SetOpt(KWapSmsOptionSmartMessage,KWapSmsOptionLevel));   
    
    TESTCHECKL(err, KErrNone, "Setting Message Type");
    }

/**
Used to check the CSmsMessage, will also print details of values in CSmsMessage
*/
void CWapProtSuiteStepBase::CheckSmsMessageL(CSmsMessage& aSmsmessagebuf, TPtrC8& aScnumber)
	{
	TPtrC toFromAddress = aSmsmessagebuf.ToFromAddress();
	TBuf<0x10> date;
	TBuf<0x10> timestring;
	TTime time = aSmsmessagebuf.Time();
	time.FormatL(date,(_L("%D%M%Y%/0%1%/1%2%/2%3%/3")));
	TDateTime dateTime(time.DateTime());
	INFO_PRINTF2(_L("Value for toFromAddress: %S"), &toFromAddress);
	INFO_PRINTF2(_L("Value for date: %S"), &date);
	INFO_PRINTF5(_L("TimeStamp=%02d:%02d:%02d.%06d"), dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond());
	HBufC16* scnumber16 = HBufC16::NewLC(aScnumber.Length());
	TPtr16 scnumber16mod = scnumber16->Des();
	scnumber16mod.Copy(aScnumber);
	INFO_PRINTF2(_L("Value for Service Centre number: %S"), &scnumber16mod);
	TESTL(scnumber16mod.Compare(aSmsmessagebuf.ServiceCenterAddress())==0);
	CleanupStack::PopAndDestroy(scnumber16);
	}

/**
Used to make call to get message parameters length via Ioctl
*/
TInt CWapProtSuiteStepBase::GetMessageParameterLengthL(RSocket& aSock)
	{
	TRequestStatus getParamStatus;
	TPckgBuf<TInt> paramLength;
	
	// Get the length of the message parameter value
	INFO_PRINTF1(_L("Issue of IOCTL for KSOGetMessageParametersLength"));
	aSock.Ioctl(KSOGetMessageParametersLength, getParamStatus, &paramLength, KSolWapProv);
	User::WaitForRequest(getParamStatus);	
	INFO_PRINTF1(_L("KSOGetMessageParametersLength on Ioctl completed"));
	TESTL(getParamStatus.Int()==KErrNone);

	// Return the message parameter length
	return paramLength();
	}
	
/**
Used to make call to get message parameters via Ioctl
*/
void CWapProtSuiteStepBase::GetMessageParameterL(RSocket& aSock, TPtr8& aParameterStorePtr)
	{
	TRequestStatus getParamStatus;

	INFO_PRINTF1(_L("Getting message parameter..."));
	aSock.Ioctl(KSOGetMessageParameters, getParamStatus, &aParameterStorePtr, KSolWapProv);
	User::WaitForRequest(getParamStatus);
	TESTCHECK(getParamStatus.Int(), KErrNone, "Getting message parameter");
	}

/**
Used to internalise the TDes8 and check the resultant CSmsMessage
*/
void CWapProtSuiteStepBase::InternaliseSmsDataAndCheckL(TDes8& aBuffer, TPtrC8& aScnumber)
	{
	CSmsMessage* smsMessageBuf = NULL;
	
	RDesReadStream reader(aBuffer);
	reader.PushL();
	
	RFs rFs;
	User::LeaveIfError(rFs.Connect());
	CleanupClosePushL(rFs);
	
	smsMessageBuf = CSmsMessage::NewL(rFs, CSmsPDU::ESmsStatusReport, CSmsBuffer::NewL());
	CleanupStack::PushL(smsMessageBuf);
	smsMessageBuf->InternalizeWithoutBufferL(reader);
	
	INFO_PRINTF1(_L("Check the contents of the internalized sms message"));
	CheckSmsMessageL(*smsMessageBuf, aScnumber);
	
	CleanupStack::PopAndDestroy(3, &reader); // reader, rFs, smsMessageBuf
	}

void CWapProtSuiteStepBase::OpenSocketL(RSocketServ& aSocketServer, RSocket& aSocket, TUint aAddrFamily,TUint aProtocol)
    {
    OpenSocketLC(aSocketServer, aSocket, aAddrFamily, aProtocol);   
    CleanupStack::Pop(&aSocket);
    }

void CWapProtSuiteStepBase::OpenSocketLC(RSocketServ& aSocketServer, RSocket& aSocket, TUint aAddrFamily, TUint aProtocol)
    {
    TInt error = aSocket.Open(aSocketServer,aAddrFamily,KSockDatagram,aProtocol);
    if(error != KErrNone)
         {
         //If error == KErrNone, do nothing.
         //If error == KErrServerBusy, change the leave code to "84" to imply the failure otherwise the failed step will be started again.
         //Any other errors will leave with the error code.
         if(error == KErrServerBusy)
             {
             error = TEST_ERROR_CODE;
             ERR_PRINTF2(_L("Open socket failed - error is KErrServerBusy, changing to TEST_ERROR_CODE (=%d)"), error);            
             }
         else
             {
             ERR_PRINTF2(_L("Open socket failed - error=%d"), error);
             }
          User::Leave(error);
          }   
    CleanupClosePushL(aSocket);
    }

void CWapProtSuiteStepBase::ReceiveWapMessageFromSocket(TInt aLength, TPtrC& aData)
    {
    //  Receiving a message, length of which should be requested prior to calling
    // this method from the socket and supplied thought aLength parameter
    
    TRequestStatus  status;
    TWapAddr recvWapAddr;           
    HBufC8* recvBuf = HBufC8::NewLC(aLength);
    TPtr8 recvPtr = recvBuf->Des(); 

    INFO_PRINTF1(_L("Receiving a message from the socket..."));
    iSocket.RecvFrom(recvPtr, recvWapAddr, 0, status);
    User::WaitForRequest(status);
    TESTCHECKL(status.Int(), KErrNone, "Receiving a message");
    
    TBuf8<300> data;
    data.Copy(aData);
    TESTCHECK(recvPtr.Compare(data), 0, "Checking the received message matched expected");
    
    //  Confirm the receipt of the message to the client
    INFO_PRINTF1(_L("Setting Socket option for indicating receipt of the message..."));
    TInt ret = iSocket.SetOpt(KWapSmsOptionOKToDeleteMessage,KWapSmsOptionLevel, 0);
    TESTCHECKL(ret, KErrNone, "Setting Socket option for indicating receipt of the message");
    CleanupStack::PopAndDestroy(recvBuf);
    }

void CWapProtSuiteStepBase::ReceiveWapMessage(TPtrC& aData)
    {
    TRequestStatus ioctlStatus;
    TPckgBuf<TInt> length;
    
    INFO_PRINTF1(_L("Issuing IOCTL for getting the length of the message..."));
    iSocket.Ioctl(KSOGetLength, ioctlStatus, &length, KSolWapProv);
    User::WaitForRequest(ioctlStatus);
    TESTCHECKL(ioctlStatus.Int(), KErrNone, "Getting the length of the incoming message");
    
    //  Check if reported length is correct
    TESTCHECK(length(), aData.Length(), "Check if reported length is correct");
    
    ReceiveWapMessageFromSocket(length(), aData);
    }

void CWapProtSuiteStepBase::SendWapMessage(TPtrC& aData)
    {
    TRequestStatus status;
    TBuf8<300> data;
    data.Copy(aData);

    INFO_PRINTF1(_L("Sending a message..."));
    iSocket.SendTo(data, iWapAddr, 0, status);
    User::WaitForRequest(status);
    TESTCHECKL(status.Int(), KErrNone, "Sending a message");
    }
