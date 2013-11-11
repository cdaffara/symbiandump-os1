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

#include "TE_smsstorbase.h"

#include "TE_smsstor.h"

TVerdict CSmsStorTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	User::LeaveIfError(iFs.Connect());

	iTelephoneNumber =_L("+447747065825");     // Andre
	iServiceCenterNumber =_L("+447785016005"); // Vodafone

	iLogId = 1 ;
	iMsgRef = 1 ;
	iMsgBuf = NULL;

    return TestStepResult() ;
	}

TVerdict CSmsStorTestStep::doTestStepPostambleL()
	{
	iFs.Close();

	delete iMsgBuf;
	iMsgBuf = NULL;

	__UHEAP_MARKEND;

 	return TestStepResult() ;
	}

void CSmsStorTestStep::AddReferenceL( TInt aLogId, TInt aRef )
    {
    TSmsAddr iSmsAddr;
    iSmsAddr.SetSmsAddrFamily(ESmsAddrRecvAny);
    CSmsMessage* iSmsMessage;
    //@@@ later test for memory leak
    //CArrayFix<TInt>* iStatusArray=new(ELeave) CArrayFixFlat<TInt>(8);
    //CleanupStack::PushL(iStatusArray);

    iSmsMessage = CreateSmsMessageL(iMsgBuf->Des(),TSmsDataCodingScheme::ESmsAlphabet7Bit);
    CleanupStack::PushL(iSmsMessage);
    iSmsMessage->SetLogServerId( aLogId );
    CSmsSubmit& submitPdu=(CSmsSubmit&)iSmsMessage->SmsPDU();
    submitPdu.SetStatusReportRequest(ETrue);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    iSegmentationStore->AddReferenceL( *iSmsMessage, aRef );
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    CleanupStack::PopAndDestroy(); //iSmsMessage
    }



CSmsMessage* CSmsStorTestStep::CreateSmsMessageL(const TDesC& aDes,
							   TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
							   CSmsPDU::TSmsPDUType aPDUType)
//
// Create a uninitialised SMS message
//
    {

    CSmsBuffer* buffer=NULL;
    buffer=CSmsBuffer::NewL();
    CSmsMessage* smsMessage=NULL;
    smsMessage=CSmsMessage::NewL(iFs, aPDUType,buffer);
    CleanupStack::PushL(smsMessage);

    TSmsUserDataSettings smsSettings;
    smsSettings.SetAlphabet(aAlphabet);
    smsSettings.SetTextCompressed(EFalse);
    smsSettings.SetTextConcatenated(EFalse);
    switch( aPDUType )
        {
        case CSmsPDU::ESmsSubmit:
        case CSmsPDU::ESmsDeliver:
            {
            smsMessage->SetToFromAddressL(iTelephoneNumber);
            smsMessage->SmsPDU().SetServiceCenterAddressL(iServiceCenterNumber);
            if( aDes.Length() > 160 )
                smsSettings.SetTextConcatenated(ETrue);
            break;
            }
        case CSmsPDU::ESmsCommand:
            {
            smsMessage->SetToFromAddressL(iTelephoneNumber);
            smsMessage->SmsPDU().SetServiceCenterAddressL(iServiceCenterNumber);
            break;
            }
        case CSmsPDU::ESmsStatusReport:
            {
            smsMessage->SetToFromAddressL(iTelephoneNumber);
            smsMessage->SmsPDU().SetServiceCenterAddressL(iServiceCenterNumber);

            CSmsStatusReport& srPdu=(CSmsStatusReport&)smsMessage->SmsPDU();
            srPdu.SetParameterIndicatorPresent(ETrue);
            TEST(srPdu.ParameterIndicatorPresent());

            srPdu.SetProtocolIdentifierPresent(EFalse);
            TEST(!srPdu.ProtocolIdentifierPresent());
            srPdu.SetProtocolIdentifierPresent(ETrue);
            TEST(srPdu.ProtocolIdentifierPresent());
            break;
            }
        case CSmsPDU::ESmsSubmitReport:
        case CSmsPDU::ESmsDeliverReport:
            {
            break;
            }
        }
    if( aPDUType != CSmsPDU::ESmsCommand )
        {
        buffer->InsertL(0,aDes);
        smsMessage->SetUserDataSettingsL(smsSettings);
        }

	CArrayFixFlat<TGsmSms>* smsPdus = new (ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(smsPdus);
	smsMessage->EncodeMessagePDUsL(*smsPdus);
	CleanupStack::PopAndDestroy(smsPdus);

    CleanupStack::Pop(smsMessage);
    return smsMessage;
    }

void CSmsStorTestStep::AddStatusReportL( TInt aLogId, TInt aRef, TSmsStatus::TSmsStatusValue aValue )
    {
    TSmsAddr iSmsAddr;
    iSmsAddr.SetSmsAddrFamily(ESmsAddrRecvAny);
    CSmsMessage* iOriginalSmsMessage = NULL;
    CSmsMessage* iSmsMessage;

	CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray refStatusArray;
	CleanupClosePushL(refStatusArray);

    TInt iIndex =0;
    TBool iIsComplete = EFalse;

    iSmsMessage = CreateSmsMessageL(_L("Status Report"),TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsStatusReport);
    CleanupStack::PushL(iSmsMessage);
    iSmsMessage->SetLogServerId( aLogId );
    CSmsStatusReport& statusPdu = (CSmsStatusReport&)iSmsMessage->SmsPDU();
    statusPdu.SetMessageReference(aRef);
    statusPdu.SetStatus(aValue);

    TBool found=iSegmentationStore->AddStatusReportL(iIndex,iIsComplete,*iSmsMessage);

    if (found)
        {
        CSmsBuffer*  buffer = CSmsBuffer::NewL();
        iOriginalSmsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);

        iSegmentationStore->GetMessageL(iIndex,iSmsAddr,*iOriginalSmsMessage,refStatusArray);
        }

    if (iIsComplete)
        {
//      iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);
        }

    if(iOriginalSmsMessage)
    	{
        delete iOriginalSmsMessage;
        }

    CleanupStack::PopAndDestroy(2); // iStatusArray, iSmsMessage
    }


void CSmsStorTestStep::SetGlobalsL(TSmsServiceCenterAddress aTel,
								   TSmsServiceCenterAddress aSC, TInt aMsgSize)
    {
    iTelephoneNumber =      aTel;
    iServiceCenterNumber  = aSC;
   
    delete iMsgBuf;
    iMsgBuf = HBufC::NewL(aMsgSize);

    TPtr msgPtr(iMsgBuf->Des());
    FillDes(msgPtr,aMsgSize);
    }

void CSmsStorTestStep::AddSubmitL( TInt aLogId )
    {
    TSmsAddr iSmsAddr;
    iSmsAddr.SetSmsAddrFamily(ESmsAddrRecvAny);
    CSmsMessage* iSmsMessage;
    //@@@ later test for memory leak
    //CArrayFix<TInt>* iStatusArray=new(ELeave) CArrayFixFlat<TInt>(8);
    //CleanupStack::PushL(iStatusArray);

    iSmsMessage = CreateSmsMessageL(iMsgBuf->Des(),TSmsDataCodingScheme::ESmsAlphabet7Bit);
    CleanupStack::PushL(iSmsMessage);
    iSmsMessage->SetLogServerId( aLogId );
    CSmsSubmit& submitPdu=(CSmsSubmit&)iSmsMessage->SmsPDU();
    submitPdu.SetStatusReportRequest(ETrue);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    iSegmentationStore->AddSubmitL( iSmsAddr, *iSmsMessage );
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    CleanupStack::PopAndDestroy(); //iSmsMessage
    }

void CSmsStorTestStep::TestSmsSegmentationStoreMessageTestL(TInt aMsgLen,
											TBool aDupMessageRef, TInt aAddCount)
/**
 *  Purges the segmentation store then...
 *  
 *  1. Adds a message to the segmentation store
 *  2. Adds a reference for each segment of the message
 *  3. Adds a temporary status report for each segment
 *  4. Adds a permanent status report for each segment
 *  
 */
	{
	INFO_PRINTF4(_L("SmsSegmentationStoreMessageTestL [aMsgLen=%d aDupMessageRef=%d aAddCount=%d]"), aMsgLen, aDupMessageRef, aAddCount);
	iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);
	const TInt startCount = iSegmentationStore->Entries().Count();

    const TInt TmpLogId  = iLogId;
    const TInt TmpMsgRef = iMsgRef;
	TestAddStatusReportL(aMsgLen, iLogId, iMsgRef, TSmsStatus::ESmsTempError1StatusCongestion, aAddCount, ETrue, aDupMessageRef);

	iLogId = TmpLogId;
	iMsgRef = TmpMsgRef;
	TestAddStatusReportL(aMsgLen, iLogId, iMsgRef, TSmsStatus::ESmsShortMessageReceivedBySME, aAddCount, EFalse, aDupMessageRef);

	iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);

	const TInt endCount = iSegmentationStore->Entries().Count();
	INFO_PRINTF3(_L("Complete [startCount=%d endCount=%d]"), startCount, endCount);
	TEST(endCount == startCount);
	}

void CSmsStorTestStep::TestAddStatusReportL(TInt aMsgLen, TInt& aStartLogId, TInt& aStartMsgRef, TSmsStatus::TSmsStatusValue aStatus, TInt aAddCount, TBool aAddSubmit, TBool aDupRef)
	{
	SetGlobalsL( _L("+447747065825"),_L("+44385016005"), aMsgLen);
	CSmsMessage* smsMessage = CreateSmsMessageL(iMsgBuf->Des(),TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(smsMessage);
	const TInt numParts = smsMessage->NumMessagePDUsL();
	CleanupStack::PopAndDestroy(smsMessage);

    for(TInt i = 0; i < aAddCount ; i++ )
        {
		if (aAddSubmit)
			AddSubmitL(aStartLogId);

		for (TInt j = 0; j < numParts; j++)
			{
			if (aAddSubmit)
				AddReferenceL(aStartLogId, aStartMsgRef);

			AddStatusReportL(aStartLogId, aStartMsgRef, aStatus);

			if (!aDupRef)
		        aStartMsgRef =  (aStartMsgRef + 1)  % 0x100;
			}

        aStartLogId++;
        aStartMsgRef =  (aStartMsgRef + 1)  % 0x100;
		//INFO_PRINTF1(_L("."));
        }
        
    delete iMsgBuf;
    iMsgBuf = NULL;
	}

void CSmsStorTestStep::TestSmsSegmentionStorePurge2L(TInt aMsgLen,
													 TInt aAddCount, TInt aDelivered)
	{
	INFO_PRINTF4(_L("TestSmsSegmentionStorePurge2L [aMsgLen=%d aAddCount=%d aDelivered=%d]"), aMsgLen, aAddCount, aDelivered);
	TEST(aDelivered <= aAddCount);
	TEST(aDelivered >= 0);
	TEST(aAddCount > 0);

	_LIT(KSegmentationStoreName,"C:\\System\\Data\\smssegst.dat");
	const TInt fileErr = iFs.Delete(KSegmentationStoreName);
	if (fileErr != KErrNotFound && fileErr != KErrPathNotFound)
		User::LeaveIfError(fileErr);

    iSegmentationStore=CSmsSegmentationStore::NewL(iFs);

    iSegmentationStore->OpenL(KSegmentationStoreName,KSegmentationStoreUid);

    TSmsAddr smsAddr;
    smsAddr.SetSmsAddrFamily(ESmsAddrRecvAny);
    CSmsBuffer* buffer = CSmsBuffer::NewL();
    CSmsMessage* smsMessage = CSmsMessage::NewL( iFs, CSmsPDU::ESmsSubmit, buffer );
    CleanupStack::PushL(smsMessage);

	CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray refStatusArray;
	CleanupClosePushL(refStatusArray);


    const TInt TmpLogId  = iLogId;
    const TInt TmpMsgRef = iMsgRef;
	TInt msgCount = aAddCount;
	TestAddStatusReportL(aMsgLen, iLogId, iMsgRef, TSmsStatus::ESmsTempError1StatusCongestion, aAddCount, ETrue, EFalse);

	//Purge the segmentation store
    iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);

	// Check that all messages still exist in the iSegmentationStore
    for(TInt i = 0; i < iSegmentationStore->Entries().Count(); i++ )
        {
		iSegmentationStore->GetMessageL( i, smsAddr, *smsMessage, refStatusArray );
		msgCount--;
        }

	TEST(msgCount == 0);

	iLogId = TmpLogId;
	iMsgRef = TmpMsgRef;
	TestAddStatusReportL(aMsgLen, iLogId, iMsgRef, TSmsStatus::ESmsShortMessageReceivedBySME, aDelivered, EFalse, EFalse);
    iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);

	// Check that all messages were removed from the iSegmentationStore
	TInt afterCount = iSegmentationStore->Entries().Count();
	TEST(afterCount== (aAddCount-aDelivered));

	if (aAddCount != aDelivered)
		{
		TestAddStatusReportL(aMsgLen, iLogId, iMsgRef, TSmsStatus::ESmsShortMessageReceivedBySME, aAddCount-aDelivered, EFalse, EFalse);
	    iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);
		afterCount = iSegmentationStore->Entries().Count();
		TEST(afterCount==0);
		}

	CleanupStack::PopAndDestroy(&refStatusArray);
	CleanupStack::PopAndDestroy(smsMessage);
	delete iSegmentationStore ;
	iSegmentationStore = NULL;

	INFO_PRINTF1(_L("Complete"));
	}

void CSmsStorTestStep::CreateNewSegmentationStoreL()
    {
    //Create the segmentation store for the first time adding messages.
    _LIT(KCreateNewSegmentationStore, "Create segmentation store");
    INFO_PRINTF1(KCreateNewSegmentationStore);
    _LIT(KSegmentationStoreName,"C:\\System\\Data\\smssegst.dat");
    const TInt fileErr = iFs.Delete(KSegmentationStoreName);
    if (fileErr != KErrNotFound && fileErr != KErrPathNotFound)
         {
         User::LeaveIfError(fileErr);
         }
    iSegmentationStore=CSmsSegmentationStore::NewL(iFs);  
    iSegmentationStore->OpenL(KSegmentationStoreName,KSegmentationStoreUid);
    }

void CSmsStorTestStep::DeleteSegmetationStore()
    {
    _LIT(KDeleteSegmetationStore, "Delete segmentation store");
    INFO_PRINTF1(KDeleteSegmetationStore);
  
    delete iSegmentationStore ;
    iSegmentationStore = NULL;
    }

TInt CSmsStorTestStep::ReadMaxNumMessageValueL()
    {
    _LIT(KReadMaxNumMessageValue, "Read maximum number of messages");
    INFO_PRINTF1(KReadMaxNumMessageValue);
  
    _LIT(KSmseskfile, "smswap.sms.esk");
    CESockIniData* ini = CESockIniData::NewL(KSmseskfile);
    CleanupStack::PushL(ini);
    TInt ret = 0;
    TPtrC value;
    if(ini->FindVar(_L("SegmentationStoreOptions"),_L("MaxNumOfMessInSegStore"),value))
        {
        TLex16 valueconv(value);
        valueconv.Val(ret); 
        CleanupStack::PopAndDestroy(ini);
        INFO_PRINTF2(_L("MaxNumOfMessInSegStor = %d"), ret);
        }
    else
        {
        _LIT(KErrorMessage, "[SegmentationStoreOptions] is not defined in .esk file");
        ERR_PRINTF1(KErrorMessage);
        User::Leave(KErrNotFound);
        }
    
    return ret;
    }

void CSmsStorTestStep::AddMessageAndCheckPurgeL(TInt aAddNumMessages, TInt aExpectedNumMessages)
    {
    TestAddStatusReportL(20, iLogId, iMsgRef, TSmsStatus::ESmsTempError1StatusCongestion, aAddNumMessages, ETrue, EFalse);
    //Purge the segmentation store
    iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);
    if(aExpectedNumMessages != iSegmentationStore->Entries().Count())
        {
        ERR_PRINTF3(_L("Number of messages in segmentation store = %d does not match expected %d"), iSegmentationStore->Entries().Count(), aExpectedNumMessages);
        SetTestStepResult(EFail);
        }
    }

void CSmsStorTestStep::CheckSegmentationStoreForMessagesL(RArray<TInt>& aLogIdsToFind)
    {
//Get Messages in segmentation Store and check their log ids to make sure aLogIdsToFind are included
    _LIT(KCheckSegmentationStoreForMessages, "Check segmentation store for given messages");
    INFO_PRINTF1(KCheckSegmentationStoreForMessages);
  
    CSmsBuffer* buffer = CSmsBuffer::NewL();
    CSmsMessage* smsMessage = CSmsMessage::NewL( iFs, CSmsPDU::ESmsSubmit, buffer );
    CleanupStack::PushL(smsMessage);
    CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray refStatusArray;
    CleanupClosePushL(refStatusArray);
    TInt msgCountFound = 0;
    const TInt countLogIdsToFind = aLogIdsToFind.Count();
    const TInt countLogIds = iSegmentationStore->Entries().Count();
    for(TInt i = 0; i < countLogIds; ++i )
        {
        TSmsAddr smsAddr;
        smsAddr.SetSmsAddrFamily(ESmsAddrRecvAny);
        iSegmentationStore->GetMessageL( i, smsAddr, *smsMessage, refStatusArray );
        TInt id = smsMessage->LogServerId();
        for(TInt jj = 0; jj < countLogIdsToFind; ++jj)
            {
            if(id == aLogIdsToFind[jj])
                {
                ++msgCountFound;
                break;
                }
            }
        }
    
    INFO_PRINTF3(_L("Expecting %d message in store - found %d"), msgCountFound, countLogIdsToFind);

    TEST(msgCountFound == countLogIdsToFind);
    CleanupStack::PopAndDestroy(&refStatusArray);
    CleanupStack::PopAndDestroy(smsMessage);
    }

void CSmsStorTestStep::AddDeliverL( TInt aIndex )
    {
    // adding a message to the reassemblystore
  //  TInt iIndex=0;

    CArrayFix<TGsmSms>* smsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
    CleanupStack::PushL(smsArray);
    CSmsMessage* smsMsg = CreateSmsMessageL(iMsgBuf->Des(),TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliver);
    CleanupStack::PushL(smsMsg);
    //iSmsMessage->SetLogServerId( aLogId );
    CSmsDeliver& deliverPdu = (CSmsDeliver&)smsMsg->SmsPDU();
    deliverPdu.SetMoreMessagesToSend(EFalse);

    smsMsg->EncodeMessagePDUsL(*smsArray);

    TRAP_IGNORE(iReassemblyStore->NewMessagePDUL(aIndex,*smsMsg,smsArray->At(0)));
    TSAREntry entry=iReassemblyStore->Entries()[aIndex];
    smsMsg->SetLogServerId(entry.LogServerId());
    smsMsg->SetTime(entry.Time());

    CleanupStack::PopAndDestroy(2); // smsMsg, smsArray
	}

void CSmsStorTestStep::DoEncodePDUsL(TInt aSize, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aPDUType)
    {
    TSmsAddr iSmsAddr;
    iSmsAddr.SetSmsAddrFamily(ESmsAddrRecvAny);
    CSmsMessage* iSmsMessage;
    CArrayFix<TGsmSms>* smsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
    TGsmSms gsmSms = TGsmSms();
    TBool mobileTerminated = EFalse;
    if(    (aPDUType == CSmsPDU::ESmsDeliver)
       ||  (aPDUType == CSmsPDU::ESmsSubmitReport)
       ||  (aPDUType == CSmsPDU::ESmsStatusReport))
        mobileTerminated = ETrue;

    HBufC* MsgBuf = HBufC::NewLC(aSize);
    TPtr msgPtr   = MsgBuf->Des();
    FillDes(msgPtr,aSize);

    iSmsMessage = CreateSmsMessageL(msgPtr,aAlphabet,aPDUType);
    CleanupStack::PushL(iSmsMessage);

    // if concatenated message
    if( aSize > 160 )
        {
        TSmsUserDataSettings smsSettings;
        iSmsMessage->UserDataSettings(smsSettings);
        smsSettings.SetTextConcatenated(ETrue);
        iSmsMessage->SetUserDataSettingsL(smsSettings);
        }

    CSmsPDU& pdu = (CSmsPDU&)iSmsMessage->SmsPDU();

    if( aPDUType != CSmsPDU::ESmsCommand )
        {
        TSmsDataCodingScheme::TSmsClass smsclass, gsmsclass;
        smsclass = TSmsDataCodingScheme::ESmsClass0;
        pdu.SetClass(ETrue, smsclass );
        TEST(pdu.Class(gsmsclass));
        TEST( gsmsclass == smsclass );
        }


//
//  Check before Encoding and Decoding of the CSmsMessage
//
    //
    //  Check the Deliver PDU
    //
    if( aPDUType == CSmsPDU::ESmsDeliver )
        {
        CSmsDeliver& delPdu=(CSmsDeliver&)iSmsMessage->SmsPDU();

        delPdu.SetMoreMessagesToSend(EFalse);
        TEST( delPdu.MoreMessagesToSend() == EFalse);
        delPdu.SetMoreMessagesToSend(ETrue);
        TEST( delPdu.MoreMessagesToSend());

        delPdu.SetReplyPath( EFalse);
        TEST( delPdu.ReplyPath() == EFalse);
        delPdu.SetReplyPath( ETrue);
        TEST( delPdu.ReplyPath());

        TTime stime, gtime; TInt squart, gquart;
        stime = 1234555; squart = 20;
        delPdu.SetServiceCenterTimeStamp( stime, squart );
        delPdu.ServiceCenterTimeStamp( gtime, gquart );
        TEST( stime == gtime );
        TEST( squart == gquart);

        delPdu.SetStatusReportIndication( EFalse );
        //TEST(delPdu.StatusReportIndication() == EFalse);
        delPdu.SetStatusReportIndication( ETrue );
        //TEST( delPdu.StatusReportIndication() != EFalse);

        }

    //
    //  Check the Command PDU
    //
    if( aPDUType == CSmsPDU::ESmsCommand )
        {
        CSmsCommand& cmdPdu=(CSmsCommand&)iSmsMessage->SmsPDU();

        // Check the Command Type
        cmdPdu.SetCommandType(TSmsCommandType::ESmsCommandTypeEnquiry);
        TEST(TSmsCommandType::ESmsCommandTypeEnquiry == cmdPdu.CommandType());
        cmdPdu.SetCommandType(TSmsCommandType::ESmsCommandTypeCancel);
        TEST(TSmsCommandType::ESmsCommandTypeCancel == cmdPdu.CommandType());
        cmdPdu.SetCommandType(TSmsCommandType::ESmsCommandTypeDelete);
        TEST(TSmsCommandType::ESmsCommandTypeDelete == cmdPdu.CommandType());
        cmdPdu.SetCommandType(TSmsCommandType::ESmsCommandTypeEnableStatusReportRequest);
        TEST(TSmsCommandType::ESmsCommandTypeEnableStatusReportRequest == cmdPdu.CommandType());

        // Check the Command Data
        _LIT8( KCmdData1, "CMD1" );
        cmdPdu.SetCommandDataL( KCmdData1 );
        TEST(KCmdData1() == cmdPdu.CommandData());

        // Check the Message Reference
        cmdPdu.SetMessageReference( 100 );
        TEST( 100 == cmdPdu.MessageReference());

        // Check the Message Number
        cmdPdu.SetMessageNumber( 150 );
        TEST( 150 == cmdPdu.MessageNumber());

        // Check the Status Report Request
        if(TSmsCommandType::ESmsCommandTypeEnableStatusReportRequest == cmdPdu.CommandType())
            {
            TBool srr = ETrue;
            cmdPdu.SetStatusReportRequest( srr );
            TEST( srr == cmdPdu.StatusReportRequest() );
            }

        // Check the IE index
        TInt index = 0;
        _LIT8( KCmdIE1, "\x01\x02\x03" );
        TBuf8<10> buf8;
        buf8.Copy(KCmdIE1);
        cmdPdu.AddInformationElementL(CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference, buf8);
        TInt length = cmdPdu.MaxCommandDataLength();
        length++;
        TInt num = cmdPdu.NumInformationElements();
        num++;
        cmdPdu.InformationElementIndex(CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference, index);
        cmdPdu.RemoveInformationElement(index);
        }
    //
    //  Check the Status Report PDU
    //
    if( aPDUType == CSmsPDU::ESmsStatusReport )
        {
        CSmsStatusReport& srPdu=(CSmsStatusReport&)iSmsMessage->SmsPDU();
        srPdu.SetParameterIndicatorPresent(ETrue);
        TEST(srPdu.ParameterIndicatorPresent());

        pdu.SetProtocolIdentifierPresent(EFalse);
        TEST(!pdu.ProtocolIdentifierPresent());
        pdu.SetProtocolIdentifierPresent(ETrue);
        TEST(pdu.ProtocolIdentifierPresent());
        }

    // Have placed this here because StatusR, SubmitR and DeliverR have to set
    // the parameter indicator first before setting the protocol identifier

    //pdu.SetProtocolIdentifierPresent(EFalse);
    // TODO - ahe - this test can't be done because of a assert_debug ...
    //TEST(!pdu.ProtocolIdentifierPresent());
    //pdu.SetProtocolIdentifierPresent(ETrue);
    //TEST(pdu.ProtocolIdentifierPresent());
    TSmsProtocolIdentifier::TSmsPIDType aPIDType = TSmsProtocolIdentifier::ESmsPIDTelematicInterworking;
    pdu.SetPIDType(aPIDType);
    TEST( aPIDType == pdu.PIDType());

//
// #################################################################################
//
    iSmsMessage->EncodeMessagePDUsL(*smsArray);
    if (iSmsMessage->NumMessagePDUsL()>1)
        iSmsMessage->DecodeMessagePDUsL(*smsArray);
    CleanupStack::PopAndDestroy(2); //iSmsMessage, MsgBuf

    gsmSms.SetPdu( smsArray->At(0).Pdu() );
    CSmsBuffer* buffer = CSmsBuffer::NewL();
    iSmsMessage = CSmsMessage::NewL( iFs, gsmSms, buffer, EFalse, mobileTerminated );
    CleanupStack::PushL(iSmsMessage);
    // to fill out

//
// #################################################################################
//
/*
//
//  Final Check after Encoding and Decoding of the CSmsMessage
//
    //
    //  Check the Command Deliver PDU
    //
    if( aPDUType == CSmsPDU::ESmsDeliver )
        {
        CSmsDeliver& delPdu=(CSmsDeliver&)iSmsMessage->SmsPDU();
        }

    //
    //  Check the Command Deliver PDU
    //
    if( aPDUType == CSmsPDU::ESmsSubmit )
        {
        CSmsSubmit& subPdu=(CSmsSubmit&)iSmsMessage->SmsPDU();
        }

    //
    //  Check the Command Deliver PDU
    //
    if( aPDUType == CSmsPDU::ESmsDeliverReport )
        {
        CSmsDeliverReport& delRPdu=(CSmsDeliverReport&)iSmsMessage->SmsPDU();
        }

    //
    //  Check the Command Deliver PDU
    //
    if( aPDUType == CSmsPDU::ESmsSubmitReport )
        {
        CSmsSubmitReport& subRPdu=(CSmsSubmitReport&)iSmsMessage->SmsPDU();
        }
*/
    //
    //  Check the Command PDU
    //
    if( aPDUType == CSmsPDU::ESmsCommand )
        {
        CSmsCommand& cmdPdu=(CSmsCommand&)iSmsMessage->SmsPDU();
        // Check the Command Type
        TEST(TSmsCommandType::ESmsCommandTypeEnableStatusReportRequest == cmdPdu.CommandType());
        // Check the Command Data
        _LIT8( KCmdData1, "CMD1" );
        TEST(KCmdData1() == cmdPdu.CommandData());
        // Check the Message Reference
        TEST( 100 == cmdPdu.MessageReference());
        // Check the Message Number
        TEST( 150 == cmdPdu.MessageNumber());
        // Check the Status Report Request
        if(TSmsCommandType::ESmsCommandTypeEnableStatusReportRequest == cmdPdu.CommandType())
            {
            TBool srr = ETrue;
            TEST( srr == cmdPdu.StatusReportRequest() );
            }

        // Check the IE index
        TInt index = 0;
        _LIT8( KCmdIE1, "\x01\x02\x03" );
        TBuf8<10> buf8;
        buf8.Copy(KCmdIE1);
        cmdPdu.AddInformationElementL(CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference, buf8);
        TInt length = cmdPdu.MaxCommandDataLength();
        length++;
        TInt num = cmdPdu.NumInformationElements();
        num++;
        cmdPdu.InformationElementIndex(CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference, index);
        cmdPdu.RemoveInformationElement(index);
        }


    CleanupStack::PopAndDestroy(); //iSmsMessage
    smsArray->Reset();
    delete smsArray;

    }

void CSmsStorTestStep::DoInternalizeExternalizeSMSL(TInt aSize, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aPDUType)
    {
    CArrayFix<TInt>* indexArray = new( ELeave ) CArrayFixFlat<TInt>( 8 );
    CleanupStack::PushL( indexArray );
    CArrayFixFlat<TGsmSms>* smsArray = new( ELeave ) CArrayFixFlat<TGsmSms>( 8 );
    CleanupStack::PushL( smsArray );

    HBufC* MsgBuf = HBufC::NewLC(aSize);
    TPtr msgPtr   = MsgBuf->Des();
    FillDes(msgPtr,aSize);
    CSmsMessage* iSmsMessage = CreateSmsMessageL(msgPtr,aAlphabet,aPDUType);
    CleanupStack::PushL( iSmsMessage );

    if(iSmsMessage->TextPresent())
	{
		TSmsUserDataSettings smsSettings;
		iSmsMessage->UserDataSettings(smsSettings);
		smsSettings.SetTextConcatenated(ETrue);
		iSmsMessage->SetUserDataSettingsL(smsSettings);
	}

	if(!iSmsMessage->EncodeIntoSinglePDUL(*smsArray))
		iSmsMessage->EncodeMessagePDUsL(*smsArray);

    if (iSmsMessage->NumMessagePDUsL()>1)
        iSmsMessage->DecodeMessagePDUsL(*smsArray);

    TInt index, smsindex, count;
    count = smsArray->Count();
    for( index = 0; index < count; index ++)
        iReassemblyStore->NewMessagePDUL(smsindex,*iSmsMessage,smsArray->At(index));
    count =iReassemblyStore->Entries().Count();
    index = 0; // reset index so reading can begin from beginning of file

    if(count && (aPDUType != CSmsPDU::ESmsSubmitReport) && (aPDUType != CSmsPDU::ESmsDeliverReport))
        {

		TRAPD(ret,iReassemblyStore->GetMessageL(index,*iSmsMessage));
        ret=iReassemblyStore->FindMessageL(*iSmsMessage, EFalse, index);
		TEST(ret==1);
        }

    CleanupStack::PopAndDestroy(4); // indexArray, smsArray, iSmsMessage, MsgBuf
    }

void CSmsStorTestStep::FillDes(TDes& aDes,TInt aLength)
//
// Fill aDes with randomly generated 7bit data
//
    {
    TText baseChar='A';

    aDes.SetLength(aLength);
    for (TInt i=0;i<aLength;i++)
//      aDes[i]=TText(baseChar+(Math::Rand(*intPtr)%26));
        aDes[i]=TText(baseChar + i%26 );
    }

void CSmsStorTestStep::Ascii2Hex(TPtr8 aPDU )
    {

    if((!aPDU.Length()) && (aPDU.Length()%2))
        return;
    HBufC8* asciibuf=HBufC8::New(aPDU.Length()/2);
    if(!asciibuf)
        return;
    TPtr8 AsciiBuf=asciibuf->Des();
    AsciiBuf.SetMax();
    TInt k = 0;
    for(TInt j=0;j<aPDU.Length();j++)
        {
        // '0' = 0x30
        // 'A' = 0x41
        // 'a' = 0x61
        TText8 myCL =    (TText8)aPDU[j++];
        TText8 myCR =    (TText8)aPDU[j];
        myCL = (TText8)((myCL < 'A') ? (myCL - 0x30) : ((myCL < 'a') ? (myCL - 0x37) : (myCL - 0x57)));
        myCR = (TText8)((myCR < 'A') ? (myCR - 0x30) : ((myCR < 'a') ? (myCR - 0x37) : (myCR - 0x57)));
        AsciiBuf[k++] =  (TText8)((myCL << 4) | myCR);
        }

    AsciiBuf.SetLength(k);
    aPDU = AsciiBuf;
//    aPDU.SetLength(k/2);

    if( asciibuf )
        delete(asciibuf);
    }
