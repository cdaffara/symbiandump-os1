// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TE_smsstor.h"

#include <bautils.h>
#include <smspproc.h>

#include "gsmuieoperations.h"
#include "gsmunonieoperations.h"

TVerdict CSmsSegmentionStorePurge::doTestStepL()
	{
	_LIT(KSegmentationStoreFileName,"C:\\Private\\101FE442\\smssegst.dat");

	TInt  fileErr = iFs.Delete(KSegmentationStoreFileName);
	if (fileErr != KErrNotFound  &&  fileErr != KErrPathNotFound)
		{
		User::LeaveIfError(fileErr);
		}

    iSegmentationStore=CSmsSegmentationStore::NewL(iFs);
	INFO_PRINTF2(_L("Seg Store 0x%X"), iSegmentationStore);

	INFO_PRINTF1(_L("Calling OpenL"));
    iSegmentationStore->OpenL(KSegmentationStoreName,KSegmentationStoreUid);
    INFO_PRINTF1(_L("OpenL Successful"));
    // TODO iSmsSettings.KSegmentationLifetimeMultiplier() = 1000 ??
    iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);
	INFO_PRINTF1(_L("PurgeL"));

    TSmsAddr smsAddr;
    smsAddr.SetSmsAddrFamily(ESmsAddrRecvAny);
    CSmsMessage* originalSmsMessage = NULL;
    CSmsMessage* smsMessage;

	CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray refStatusArray;
	CleanupClosePushL(refStatusArray);
	
	CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray refStatusArray2;
	CleanupClosePushL(refStatusArray2);
	
    smsMessage = CreateSmsMessageL(_L("Test message"),TSmsDataCodingScheme::ESmsAlphabet7Bit);
    CleanupStack::PushL(smsMessage);
    INFO_PRINTF1(_L("CreateSmsMessageL"));
    CSmsSubmit& submitPdu=(CSmsSubmit&)smsMessage->SmsPDU();
    submitPdu.SetStatusReportRequest(ETrue);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    iSegmentationStore->AddSubmitL( smsAddr, *smsMessage );
    INFO_PRINTF1(_L("AddSubmitL"));
    iSegmentationStore->AddReferenceL( *smsMessage, 5 );
    INFO_PRINTF1(_L("AddReferenceL"));
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    CleanupStack::PopAndDestroy(); //smsMessage

    TInt index =0;
    TBool isComplete = EFalse;
    smsMessage = CreateSmsMessageL(_L("Test mesage"),TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsStatusReport);
    CleanupStack::PushL(smsMessage);
    INFO_PRINTF1(_L("CreateSmsMessageL"));
    CSmsStatusReport& statusPdu = (CSmsStatusReport&)smsMessage->SmsPDU();
    statusPdu.SetMessageReference(5);
    const TInt reference=statusPdu.MessageReference();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    TBool found=iSegmentationStore->AddStatusReportL(index,isComplete,*smsMessage);
    INFO_PRINTF1(_L("AddStatusReportL"));
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (found)
        {
        const TSmsSegmentationEntry& entry =
            ( const TSmsSegmentationEntry& )iSegmentationStore->Entries()[index];
        smsMessage->SetLogServerId( entry.LogServerId() );
        CSmsBuffer* buffer = CSmsBuffer::NewL();
        originalSmsMessage = CSmsMessage::NewL( iFs, CSmsPDU::ESmsSubmit, buffer );
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        iSegmentationStore->GetMessageL( index, smsAddr, *originalSmsMessage, refStatusArray );
        INFO_PRINTF1(_L("GetMessageL"));
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
         }

    if (isComplete)
        {
        smsMessage->SetTime(originalSmsMessage->Time());
        smsMessage->SetToFromAddressL(originalSmsMessage->ToFromAddress());
        TInt count=refStatusArray.Count();
        CSmsStatusReport& statusreport=(CSmsStatusReport&) smsMessage->SmsPDU();
		TInt refStatusPos = refStatusArray.Find(reference); 
		TEST(refStatusPos != KErrNotFound);
        for (TInt i=0; i<count; i++)
            {
            const TInt status=refStatusArray[i].Status();
            statusreport.SetStatus((TSmsStatus::TSmsStatusValue) status);
            if (status!=TSmsStatus::ESmsShortMessageReceivedBySME)
                break;
            }
        }
        
	refStatusArray2.CopyL(refStatusArray);
	refStatusArray2.ResetAllStatus();
		
	const TInt count2 = refStatusArray.Count();
	__ASSERT_DEBUG(count2 == refStatusArray2.Count(),SmspPanic(KSmspPanicBadReferenceArray));

    //    iSegmentationStore->AddReferenceL( *ismsMessage, 6 );
    //    iSegmentationStore->AddReferenceL( *ismsMessage, 7 );

    iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);

    if(originalSmsMessage)
        delete originalSmsMessage;
    CleanupStack::PopAndDestroy(3); // iStatusArray, smsMessage
    delete iSegmentationStore;
    iSegmentationStore = NULL;

	return TestStepResult();
	}


TVerdict CSmsSegmentionStorePurge1::doTestStepL()
	// Background information
	// ------------------------
	//
	// Each TSAREntry contains the following attributes
	//	iReference - concatenation reference number
	//	iTotal - total number of PDUs
	//	iCount - the number of PDUs sent
	//	iData1 - Bytes 0 & 1 Number of PDUs that have been negatively
	//               acknowledged with a permanent failure notification.
	//		 Bytes 2 & 3 - Number of PDUs that have been acknowledged
	//		 as successfully received by the far end.
	//
	// The segmentation store maintains a collection of reference/status pairs
	// for each message message in the store. TSAREntry::iCount is set to the
	// size of this collection.
	//
	// The test follows the following routine:
	//  1) it creates an entry in the CSmsSegmentationStore for each submit message.
	//  2) it adds a reference for each part of the message.
	//  3) it attempts to update the store with a temporary status value for each message value
	//     (although the store internally discards the temporary values).
	//  4) it updates the store with a permanent status value for each message reference.
	//
	// The test finishes by purging the store. The test expects that every message should
	// be purged. This is because every message in the store should satisfy the condition (1)
	// in the following method:
	//
	// void CSARStore::PurgeL
	//		(entry.IsComplete() && (deliv_n_failed >= entry.Total())) // condition 1
	//		||
	//		((time>(entry.Time()+lifetime)) && !aPurgeIncompleteOnly ) // condition 2
	//		{
	//		  DoDeleteEntryL(i);
	//		}
	// Condition 2 is used to purge incomplete messages whose time stamp exceeds their
	// validity period - this condition should never be exercised in this test.
	//
	// The following test cases are used:
	//	TestSmsSegmentationStoreMessageTestL(20);            // Test 1
	//
	//	TestSmsSegmentationStoreMessageTestL(200, EFalse);   // Test 2
	//	TestSmsSegmentationStoreMessageTestL(200, ETrue);    // Test 3
	//	TestSmsSegmentationStoreMessageTestL(500, EFalse);   // Test 4
	//	TestSmsSegmentationStoreMessageTestL(500, ETrue);    // Test 5
	//
	// Test 1 simulates sending 20 messages, each encoded into a single PDU containing 20 characters.
	// Test 2 simulates sending 20 messages, each encoding a 200 character mesage into multiple PDUs using
	//  a unique message reference for each PDU.
	// Test 3 - as per test 2 but although the message reference is incremented after every message, the same
	//  (i.e. a duplicate) message reference is used for each of the message's PDUs. This simulates the
	//  message reference number (range 0..255) wrapping around when sending long messages.
	// Tests 4 & 5 are as per 2/3 but with 500 characters.

	{
	_LIT(KSegmentationStoreFileName,"C:\\Private\\101FE442\\smssegst.dat");

	TInt  fileErr = iFs.Delete(KSegmentationStoreFileName);
	if (fileErr != KErrNotFound  &&  fileErr != KErrPathNotFound)
		{
		User::LeaveIfError(fileErr);
		}

    iSegmentationStore=CSmsSegmentationStore::NewL(iFs);
    iSegmentationStore->OpenL(KSegmentationStoreName,KSegmentationStoreUid);
    // TODO iSmsSettings.KSegmentationLifetimeMultiplier() = 1000 ??

	TestSmsSegmentationStoreMessageTestL(20);

	TestSmsSegmentationStoreMessageTestL(200, EFalse);
	TestSmsSegmentationStoreMessageTestL(200, ETrue);
	TestSmsSegmentationStoreMessageTestL(500, EFalse);
	TestSmsSegmentationStoreMessageTestL(500, ETrue);

    iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);

    delete iSegmentationStore;
    iSegmentationStore = NULL;
    
    return TestStepResult();
    }


TVerdict CSmsSegmentionStorePurge2::doTestStepL()
    {
	TestSmsSegmentionStorePurge2L(20, 20, 20);
	TestSmsSegmentionStorePurge2L(200, 20, 20);
	TestSmsSegmentionStorePurge2L(200, 20, 5);
	TestSmsSegmentionStorePurge2L(200, 20, 0);
	return TestStepResult();
    }

TVerdict CSmsSegmentionStorePurge3::doTestStepL()
    {
    INFO_PRINTF1(_L("Testing segmentation store max number of messages"));

    //Read maximum number of messages from c:\private\101fe442\smswap.sms.esk (the value of the section [SegmentationStoreOptions])  
    TInt maxNumMessages = ReadMaxNumMessageValueL();
 
//Test 1 Make sure there are maximum messages in the segmentation store after adding 25 incomplete messages which exceeds the limit allowed in segmentation store. 
    CreateNewSegmentationStoreL();
    // Add 5 more incomplete messages more than the limit.
    TInt numMessage = maxNumMessages + 5;
    AddMessageAndCheckPurgeL(numMessage, maxNumMessages); 

//Test 2 Make sure there are still maximun messages in the segmentation store after adding more incompleted messages which exceeds the limit allowed in segmentation store.
    TInt startLogId = iLogId;
    //Add another maximum number of incomplete messages
    AddMessageAndCheckPurgeL(maxNumMessages, maxNumMessages);
     
    //Make sure that the newly added messages are not deleted from the store (first in first out)
    //Get log ids for those newly added messages
    RArray<TInt> logIds;
    CleanupClosePushL(logIds);
    for(TInt ii = 0; ii < maxNumMessages; ++ii)
        {
        logIds.AppendL(startLogId++);
        }
    //Test that those two messages are not deleted from the store
    CheckSegmentationStoreForMessagesL(logIds);
    CleanupStack::PopAndDestroy(&logIds);
    DeleteSegmetationStore();
    INFO_PRINTF1(_L("Complete"));  
    return TestStepResult();
    }

TVerdict CSmsSegmentionStorePurge4::doTestStepL()
    {
#if defined (__WINS__)
    INFO_PRINTF1(_L("Test that the maximum number of messages in segmentaion store is set to 0"));

    //Read maximum number of messages from c:\private\101fe442\smswap.sms.esk. The value should be set to 0
    TInt maxNumMessages = ReadMaxNumMessageValueL();
    TEST(maxNumMessages == 0);
    CreateNewSegmentationStoreL();
  
    //Make sure there are as many as messages can be stored in the segmentation store. 
    const TInt numMessage = 1000;// It could be any number
    AddMessageAndCheckPurgeL(numMessage, numMessage);
 
    //Make sure it is possible to add more messages.
    const TInt numMoreMessage = 5;
    AddMessageAndCheckPurgeL(numMoreMessage, numMessage + numMoreMessage);
  
    DeleteSegmetationStore();
    INFO_PRINTF1(_L("Complete"));

    
#else
	INFO_PRINTF1(_L("CSmsSegmentionStorePurge4::doTestStepL() is not running in hardware because it takes too long to finish"));
#endif
	return TestStepResult();
    }

TVerdict CReassemblystore3::doTestStepL()
	{
	_LIT(KReassemblyStoreFileName, "C:\\Private\\101FE442\\smsreast.dat");

	TInt  fileErr = iFs.Delete(KReassemblyStoreFileName);
	if (fileErr != KErrNotFound  &&  fileErr != KErrPathNotFound)
		{
		User::LeaveIfError(fileErr);
		}
	
	// initialisation of the reassembly store
    iReassemblyStore=CSmsReassemblyStore::NewL(iFs);

    // TODO: check if the correct file is opened (KReassemblyStoreName or KReassemblyStoreFileName)
    iReassemblyStore->OpenL(KReassemblyStoreName,KReassemblyStoreUid);
    iReassemblyStore->PurgeL(iSmsSettings.ReassemblyLifetime(), ETrue);

	iReassemblyStore->BeginTransactionLC();
    iReassemblyStore->CompactL();
	iReassemblyStore->CommitTransactionL();

    SetGlobalsL( _L("+447747065825"),_L("+44385016005"), 400);

    // adding a message to the reassemblystore
    TInt index=0;

    CArrayFix<TGsmSms>* smsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
    CleanupStack::PushL(smsArray);
    CSmsMessage* smsMsg = CreateSmsMessageL(iMsgBuf->Des(),TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliver);
    CleanupStack::PushL(smsMsg);

    CSmsDeliver& deliverPdu = (CSmsDeliver&)smsMsg->SmsPDU();
    deliverPdu.SetMoreMessagesToSend(EFalse);

    smsMsg->EncodeMessagePDUsL(*smsArray);

    TRAPD(ret,iReassemblyStore->NewMessagePDUL(index,*smsMsg,smsArray->At(0)));
    TSAREntry entry=iReassemblyStore->Entries()[index];
    smsMsg->SetLogServerId(entry.LogServerId());
    smsMsg->SetTime(entry.Time());


    index = 1;

    ret = KErrNone;
	iReassemblyStore->BeginTransactionLC();
	TEST(iReassemblyStore->FindMessageL(*smsMsg, EFalse, index));
	if(iReassemblyStore->FindMessageL(*smsMsg, EFalse, index))
	        TRAP(ret,iReassemblyStore->DeleteEntryL(index));
	TEST(ret == KErrNone);
	iReassemblyStore->CommitTransactionL();


    iReassemblyStore->PurgeL(iSmsSettings.ReassemblyLifetime(), ETrue);
	iReassemblyStore->BeginTransactionLC();
    iReassemblyStore->CompactL();
	iReassemblyStore->CommitTransactionL();

    CleanupStack::PopAndDestroy(2); // smsMsg, smsArray

    delete iReassemblyStore;
    iReassemblyStore = NULL;

    return TestStepResult();
    }


TVerdict CReassemblystore2::doTestStepL()
	{
	//
    //  TODO - ahe - to test purging of the reastore
    //  or to give this test case a sense
    //

    // initialisation of the reassembly store
    iReassemblyStore=CSmsReassemblyStore::NewL(iFs);

    iReassemblyStore->OpenL(KReassemblyStoreName,KReassemblyStoreUid);
    iReassemblyStore->PurgeL(iSmsSettings.ReassemblyLifetime(), ETrue);

	//TODO Maybe this is not needed anymore (iReassemblyStore->CommitTransactionL())
	//    iReassemblyStore->CompactL();

	SetGlobalsL( _L("+447747065825"),_L("+44385016005"), 20);

    CSmsBuffer* buffer=CSmsBuffer::NewL();
    CSmsMessage* smsmessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
    CleanupStack::PushL(smsmessage);

    TInt i;
	TInt msgCount=20;

    for( i = 0; i < msgCount ; i++ )
        {
        AddDeliverL(i);
	    iReassemblyStore->BeginTransactionLC();
		iReassemblyStore->SetPassedToClientL(i, EFalse );
	    iReassemblyStore->CommitTransactionL();
        }

	// Check that all messages still exist in the iReassemblyStore
    iReassemblyStore->BeginTransactionLC();
    for( i = 0; i < iReassemblyStore->Entries().Count(); i++ )
        {
        TRAPD(ret,iReassemblyStore->GetMessageL(i,*smsmessage));
        TEST(ret==KErrNone);
		msgCount--;
        }
    iReassemblyStore->CommitTransactionL();
	//msgCount must be 0!!
	//GLOBAL_CHECKPOINT_CODE(msgCount); //vep, will leave because of possible defect in gsmu

	// Messages will be passed now to client
    for( i = 0; i < 20 ; i++ )
        {
	    iReassemblyStore->BeginTransactionLC();
		iReassemblyStore->SetPassedToClientL(i, ETrue );
	    iReassemblyStore->CommitTransactionL();
        }

    CleanupStack::PopAndDestroy(smsmessage);

	// Same again with concatenated pdus

    SetGlobalsL( _L("+447747065825"),_L("+44385016005"), 400);

    buffer=CSmsBuffer::NewL();
    smsmessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
    CleanupStack::PushL(smsmessage);

	msgCount=20;

    for( i = 0; i < msgCount ; i++ )
        {
        AddDeliverL(i);
	    iReassemblyStore->BeginTransactionLC();
		iReassemblyStore->SetPassedToClientL(i, EFalse );
	    iReassemblyStore->CommitTransactionL();
        }

	// Check that all messages still exist in the iReassemblyStore
    iReassemblyStore->BeginTransactionLC();
    for( i = 0; i < iReassemblyStore->Entries().Count(); i++ )
        {
        TRAPD(ret,iReassemblyStore->GetMessageL(i,*smsmessage));
        TEST(ret == KErrNone);
        }
    iReassemblyStore->CommitTransactionL();

	// Messages will be passed now to client
    for( i = 0; i < 20 ; i++ )
        {
	    iReassemblyStore->BeginTransactionLC();
		iReassemblyStore->SetPassedToClientL(i, ETrue);
	    iReassemblyStore->CommitTransactionL();
        }

    CleanupStack::PopAndDestroy(smsmessage);

	// Same again with mixture of entries
	// 5 messages are passed to client
	// 15 messages aren't passed

    SetGlobalsL( _L("+447747065825"),_L("+44385016005"), 80);

    buffer=CSmsBuffer::NewL();
    smsmessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
    CleanupStack::PushL(smsmessage);

	msgCount=20;
	TInt notPassed = 15;

    for( i = 0; i < msgCount ; i++ )
        {
        AddDeliverL(i);
	    iReassemblyStore->BeginTransactionLC();
		if (i<notPassed)
			iReassemblyStore->SetPassedToClientL(i, EFalse );
		else
			iReassemblyStore->SetPassedToClientL(i, ETrue );

	    iReassemblyStore->CommitTransactionL();
        }

	// Check that 15 (not passed) messages still exist in the iReassemblyStore
	msgCount = iReassemblyStore->Entries().Count();

	// Rest of messages will be passed now to client
    for( i = 0; i < notPassed ; i++ )
        {
	    iReassemblyStore->BeginTransactionLC();
		iReassemblyStore->SetPassedToClientL(i, ETrue );
	    iReassemblyStore->CommitTransactionL();
        }

    CleanupStack::PopAndDestroy(smsmessage);

    iReassemblyStore->PurgeL(iSmsSettings.ReassemblyLifetime(), ETrue);
	// Check that all messages were removed from the iSegmentationStore
	//TInt afterCount = iReassemblyStore->Entries().Count();
	//GLOBAL_CHECKPOINT_CODE(afterCount);

    delete iReassemblyStore; //allocated in SetGlobals
    iReassemblyStore = NULL;

    return TestStepResult();
	}


TVerdict CEncodePDUs::doTestStepL()
	{

/*  enum TSmsPDUType
        {
        ESmsDeliver       = 0, ///< Deliver, sent from SC to MS
        ESmsDeliverReport = 1, ///< Deliver report, sent from MS to SC
        ESmsSubmit        = 2, ///< Submit, sent from MS to SC
        ESmsSubmitReport  = 3, ///< Submit report, sent from SC to MS
        ESmsStatusReport  = 4, ///< Status report, sent from SC to MS
        ESmsCommand       = 5  ///< Command, sent from MS to SC
        };*/

/*
    const TUint KSocketMessageSlots = 16;
    RSocketServ ss;
    TInt ret=ss.Connect(KSocketMessageSlots);
    GLOBAL_CHECKPOINT_CODE(ret);

    RSocket socket;
    //OpenSmsSocketL(socket,ESmsAddrLocalOperation);
    ret=socket.Open(ss,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
    GLOBAL_CHECKPOINT_CODE(ret);
    TSmsAddr smsaddr;
    smsaddr.SetSmsAddrFamily(ESmsAddrLocalOperation);
    //smsaddr.SetSmsAddrFamily(aFamily);
    ret=socket.Bind(smsaddr);
    GLOBAL_CHECKPOINT_CODE(ret);
    CleanupClosePushL(socket);
*/

    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliver );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsDeliver );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsDeliver );

    DoEncodePDUsL( 200, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliver );
    DoEncodePDUsL( 200, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsDeliver );
    DoEncodePDUsL( 200, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsDeliver );


    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsSubmit );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsSubmit );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsSubmit );

    DoEncodePDUsL( 200, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsSubmit );
    DoEncodePDUsL( 200, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsSubmit );
    DoEncodePDUsL( 200, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsSubmit );


    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsStatusReport );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsStatusReport );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsStatusReport );

    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsSubmitReport );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsSubmitReport );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsSubmitReport );

    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliverReport );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsDeliverReport );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsDeliverReport );


    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsCommand );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsCommand );
    DoEncodePDUsL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsCommand );

/*
    CleanupStack::PopAndDestroy(); //socket
    ss.Close();
*/
	return TestStepResult() ;
    }


TVerdict CInternalizeExternalizeSMS::doTestStepL()
	{
    _LIT(KStoreName,"C:\\System\\Data\\smstest.dat");
    const TInt KStoreUidValue=0x1000089e;
    const TUid KStoreUid={KStoreUidValue};  //  Used for second uid of SAR stores
    iReassemblyStore=CSmsReassemblyStore::NewL(iFs);
    iReassemblyStore->OpenL(KStoreName,KStoreUid);

    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsStatusReport);
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsStatusReport);
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsStatusReport);


    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliver);
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsDeliver);
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsDeliver);

    DoInternalizeExternalizeSMSL( 200, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliver);
    DoInternalizeExternalizeSMSL( 200, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsDeliver);
    DoInternalizeExternalizeSMSL( 200, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsDeliver);

    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsSubmit );
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsSubmit );
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsSubmit );

    DoInternalizeExternalizeSMSL( 200, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsSubmit );
    DoInternalizeExternalizeSMSL( 200, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsSubmit );
    DoInternalizeExternalizeSMSL( 200, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsSubmit );


    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsCommand );
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsCommand );
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsCommand );

    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsSubmitReport );
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsSubmitReport );
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsSubmitReport );

    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliverReport );
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsDeliverReport );
    DoInternalizeExternalizeSMSL( 20, TSmsDataCodingScheme::ESmsAlphabetUCS2,CSmsPDU::ESmsDeliverReport );


    TInt count =iReassemblyStore->Entries().Count();
    TInt index;
    for(index = 0; index < count; index ++ )
        {
        CSmsBuffer* buffer = CSmsBuffer::NewL();
        TSmsReassemblyEntry entry= (TSmsReassemblyEntry&) iReassemblyStore->Entries()[index];
        CSmsPDU::TSmsPDUType pdu = entry.PduType();
        CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, pdu, buffer );
        CleanupStack::PushL(smsMessage);
        TRAPD(ret,iReassemblyStore->GetMessageL(index,*smsMessage));
		TEST(ret == KErrNone);
        CleanupStack::PopAndDestroy(smsMessage);
        }
	delete iReassemblyStore;
	iReassemblyStore = NULL;
	
    return TestStepResult();
    }


TVerdict CPublicMethodTest::doTestStepL()
//
// Briefly test all public methods
//
    {

    INFO_PRINTF1(_L("Briefly test all public methods"));

    CSmsBuffer* smsBuffer=CSmsBuffer::NewL();
	CleanupStack::PushL(smsBuffer);
    CSmsPDU::TSmsPDUType pdu = CSmsPDU::ESmsSubmit;
    CSmsMessage* smsMsg = CSmsMessage::NewL(iFs, pdu,smsBuffer);
	CleanupStack::Pop(smsBuffer);
	CleanupStack::PushL(smsMsg);


    smsMsg->IsComplete();
    smsMsg->IsDecoded();
    smsMsg->Status();

    smsMsg->SetStatus(NMobileSmsStore::EStoredMessageUnread);
    smsMsg->LogServerId();
    smsMsg->SetLogServerId(1);
    smsMsg->Time();

    TTime time;
    time.UniversalTime();
    smsMsg->SetTime(time);
    smsMsg->SetUTCOffset(User::UTCOffset());
    smsMsg->UTCOffset();

    smsMsg->ServiceCenterAddress();
    smsMsg->SetServiceCenterAddressL(_L("+44385016005"));

    TGsmSmsTelNumber address;
    smsMsg->ParsedServiceCenterAddress(address);
    smsMsg->SetParsedServiceCenterAddressL(address);
    TPtrC toFromAddress=smsMsg->ToFromAddress();
    smsMsg->SetToFromAddressL(toFromAddress);
    smsMsg->ParsedToFromAddress(address);
    smsMsg->SetParsedToFromAddressL(address);

    smsMsg->TextPresent();
    smsMsg->NumMessagePDUsL();
    smsMsg->MaxMessageLength();
    smsMsg->MessageLengthL();

    TSmsUserDataSettings smsSettings;
    smsMsg->UserDataSettings(smsSettings);

    smsSettings.TextConcatenated(NULL);
    smsSettings.SetTextConcatenated(ETrue);
    smsSettings.SetTextConcatenated(EFalse);
    smsMsg->SetUserDataSettingsL(smsSettings);


    smsMsg->OptimizeSettingsL(CSmsMessage::ESmsFlagOptimizeAlphabet);
    smsMsg->OptimizeSettingsL(CSmsMessage::ESmsFlagOptimizeConcatenation);
    smsMsg->OptimizeSettingsL(CSmsMessage::ESmsFlagOptimizeConcatenation16BitReference);

	TBuf16<256> buf16;
	buf16.Format(_L16("Unicode %c %c %c %c"), 0x4eca, 0x65e5, 0x306f, 0x0021);
	smsBuffer->InsertL(0,buf16);
	smsMsg->OptimizeSettingsL(CSmsMessage::ESmsFlagOptimizeAlphabet);
	TEST(smsMsg->SmsPDU().Alphabet()==TSmsDataCodingScheme::ESmsAlphabetUCS2);

	buf16.Format(_L16("7 bit data"));
	smsBuffer->Reset();
	smsBuffer->InsertL(0,buf16);
	smsMsg->SmsPDU().SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit); //set alphabet to default value
	smsMsg->OptimizeSettingsL(CSmsMessage::ESmsFlagOptimizeAlphabet);
	TEST(smsMsg->SmsPDU().Alphabet()==TSmsDataCodingScheme::ESmsAlphabet7Bit);

	buf16.Format(_L16("Unicode %c"), 0x4eca);
	smsBuffer->Reset();
	smsBuffer->InsertL(0,buf16);
	smsMsg->OptimizeSettingsL(CSmsMessage::ESmsFlagOptimizeAlphabet);
	TEST(smsMsg->SmsPDU().Alphabet()==TSmsDataCodingScheme::ESmsAlphabetUCS2);

    TInt numUnconvChars;
    TInt indexOfFirstUnconvChar;
	smsMsg->SmsPDU().SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
    smsMsg->IsSupportedL(_L("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"),numUnconvChars,indexOfFirstUnconvChar);


    CArrayFix<TGsmSms>* smsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
	CleanupStack::PushL(smsArray);
    smsMsg->EncodeMessagePDUsL(*smsArray);


	CleanupStack::PopAndDestroy(smsArray);
	CleanupStack::PopAndDestroy(smsMsg);


    //
    // sets the local iSmsSettings for the test
    //
	TInt lifeTime(1000);
    iSmsSettings.SetKSegmentationLifetimeMultiplier( lifeTime );
	TInt segmentationLifeTime = iSmsSettings.KSegmentationLifetimeMultiplier();
	if(segmentationLifeTime != lifeTime)
		TEST(KErrArgument == KErrNone);

    iSmsSettings.SetReassemblyLifetime( lifeTime );
	TTimeIntervalMinutes reassemblyLifetime = iSmsSettings.ReassemblyLifetime();
	if(reassemblyLifetime.Int() != lifeTime)
		TEST(KErrArgument == KErrNone);

    return TestStepResult();
    }


TVerdict CLowMemoryTest::doTestStepL()
//
// Low memory testing
//
    {

    INFO_PRINTF1(_L("Low memory tests"));

	INFO_PRINTF1(_L("Creating message..."));
    TInt count=0;
    TInt ret=KErrNoMemory;
    while (ret==KErrNoMemory)
        {
		INFO_PRINTF2(_L("FAIL %d"), count);
        __UHEAP_FAILNEXT(count);
        
        __UHEAP_MARK;
        CSmsMessage* sms=NULL;
        TRAP(ret,sms=CreateSmsMessageL(_L("Test message"),TSmsDataCodingScheme::ESmsAlphabet7Bit));
    	
        if(ret==KErrNone)
			delete sms;
        __UHEAP_MARKEND;        
        
	    __UHEAP_RESET;
        count++;
        }
    
   	INFO_PRINTF2(_L(". Ret=%d"), ret);    

	TEST(ret == KErrNone);

	INFO_PRINTF1(_L("Purging the segmentation store..."));
	count=0;
    ret=KErrNoMemory;
    while (ret==KErrNoMemory)
        {
		INFO_PRINTF2(_L("FAIL %d "), count);
        __UHEAP_FAILNEXT(count);

		__UHEAP_MARK;
        TRAP(ret,TestSmsSegmentionStorePurge2L(200, 20, 5));
 		__UHEAP_MARKEND;
	
	    __UHEAP_RESET;
            count++;
        }

    INFO_PRINTF2(_L(". Ret=%d"), ret);
	TEST(ret == KErrNone);
	
    return TestStepResult();
	}


TVerdict CCorruptionTest::doTestStepL()
//
// Corruption of file stores testing
//
    {

    INFO_PRINTF1(_L("Corruption tests"));

	_LIT(KZNoPduStoreName,"Z:\\Sms\\smsreastnopdu.dat");
	_LIT(KNoPduStoreName,"C:\\System\\Data\\smsreastnopdu.dat");

	_LIT(KZNoStreamStoreName,"Z:\\Sms\\smsreastnostream.dat");
	_LIT(KNoStreamStoreName,"C:\\System\\Data\\smsreastnostream.dat");

	_LIT(KZArrayNoRootStoreName,"Z:\\Sms\\smsreastarraynoroot.dat");
	_LIT(KArrayNoRootStoreName,"C:\\System\\Data\\smsreastarraynoroot.dat");

	_LIT(KZArrayNoEntriesStoreName,"Z:\\Sms\\smsreastarraynoentries.dat");
	_LIT(KArrayNoEntriesStoreName,"C:\\System\\Data\\smsreastarraynoentries.dat");

	_LIT(KZCountWrongStoreName,"Z:\\Sms\\smsreastcountwrong.dat");
	_LIT(KCountWrongStoreName,"C:\\System\\Data\\smsreastcountwrong.dat");

	TInt ret=KErrNone;

	User::LeaveIfError(BaflUtils::CopyFile(iFs,KZNoPduStoreName ,KNoPduStoreName));
	User::LeaveIfError(iFs.SetAtt(KNoPduStoreName,KEntryAttNormal,KEntryAttReadOnly));

	User::LeaveIfError(BaflUtils::CopyFile(iFs,KZNoStreamStoreName ,KNoStreamStoreName));
	User::LeaveIfError(iFs.SetAtt(KNoStreamStoreName,KEntryAttNormal,KEntryAttReadOnly));

	User::LeaveIfError(BaflUtils::CopyFile(iFs,KZArrayNoRootStoreName ,KArrayNoRootStoreName));
	User::LeaveIfError(iFs.SetAtt(KArrayNoRootStoreName,KEntryAttNormal,KEntryAttReadOnly));

	User::LeaveIfError(BaflUtils::CopyFile(iFs,KZArrayNoEntriesStoreName ,KArrayNoEntriesStoreName));
	User::LeaveIfError(iFs.SetAtt(KArrayNoEntriesStoreName,KEntryAttNormal,KEntryAttReadOnly));

	User::LeaveIfError(BaflUtils::CopyFile(iFs,KZCountWrongStoreName ,KCountWrongStoreName));
	User::LeaveIfError(iFs.SetAtt(KCountWrongStoreName,KEntryAttNormal,KEntryAttReadOnly));


    const TInt KStoreUidValue=0x1000089e;
    const TUid KStoreUid={KStoreUidValue};  //  Used for second uid of SAR stores
    iReassemblyStore=CSmsReassemblyStore::NewL(iFs);

   	TInt count=0;
   	TInt index=0;

    TRAP(ret,iReassemblyStore->OpenL(KNoPduStoreName,KStoreUid));
    TEST(ret == KErrNone);
    count =iReassemblyStore->Entries().Count();
    for(index = 0; index < count; index ++ )
        {
        CSmsBuffer* buffer = CSmsBuffer::NewL();
        TSmsReassemblyEntry entry= (TSmsReassemblyEntry&) iReassemblyStore->Entries()[index];
        CSmsPDU::TSmsPDUType pdu = entry.PduType();
        CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, pdu, buffer );
        CleanupStack::PushL(smsMessage);
        TRAP(ret,iReassemblyStore->GetMessageL(index,*smsMessage));
		TEST(ret == KErrEof);
        CleanupStack::PopAndDestroy(smsMessage);
        }
	iReassemblyStore->Close();

	TRAP(ret,iReassemblyStore->OpenL(KNoStreamStoreName,KStoreUid));
	TEST(ret == KErrNone);
	count =iReassemblyStore->Entries().Count();
    for(index = 0; index < count; index ++ )
        {
        CSmsBuffer* buffer = CSmsBuffer::NewL();
        TSmsReassemblyEntry entry= (TSmsReassemblyEntry&) iReassemblyStore->Entries()[index];
        CSmsPDU::TSmsPDUType pdu = entry.PduType();
        CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, pdu, buffer );
        CleanupStack::PushL(smsMessage);
        TRAP(ret,iReassemblyStore->GetMessageL(index,*smsMessage));
		TEST(ret == KErrNotFound);
        CleanupStack::PopAndDestroy(smsMessage);
        }
	iReassemblyStore->Close();

	TRAP(ret,iReassemblyStore->OpenL(KArrayNoRootStoreName,KStoreUid));
	TEST(ret == KErrNone);
	count =iReassemblyStore->Entries().Count();
    for(index = 0; index < count; index ++ )
        {
        CSmsBuffer* buffer = CSmsBuffer::NewL();
        TSmsReassemblyEntry entry= (TSmsReassemblyEntry&) iReassemblyStore->Entries()[index];
        CSmsPDU::TSmsPDUType pdu = entry.PduType();
        CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, pdu, buffer );
        CleanupStack::PushL(smsMessage);
        TRAP(ret,iReassemblyStore->GetMessageL(index,*smsMessage));
		TEST(ret == KErrEof);
        CleanupStack::PopAndDestroy(smsMessage);
        }
	iReassemblyStore->Close();

	TRAP(ret,iReassemblyStore->OpenL(KArrayNoEntriesStoreName,KStoreUid));
	TEST(ret == KErrNone);
	iReassemblyStore->Close();

	TRAP(ret,iReassemblyStore->OpenL(KCountWrongStoreName,KStoreUid));
	TEST(ret == KErrNone);
	count =iReassemblyStore->Entries().Count();
    for(index = 0; index < count; index ++ )
        {
        CSmsBuffer* buffer = CSmsBuffer::NewL();
        TSmsReassemblyEntry entry= (TSmsReassemblyEntry&) iReassemblyStore->Entries()[index];
        CSmsPDU::TSmsPDUType pdu = entry.PduType();
        CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, pdu, buffer );
        CleanupStack::PushL(smsMessage);
        TRAP(ret,iReassemblyStore->GetMessageL(index,*smsMessage));
		TEST(ret == KErrCorrupt);
        CleanupStack::PopAndDestroy(smsMessage);
        }
	iReassemblyStore->Close();


	User::LeaveIfError(BaflUtils::DeleteFile(iFs,KNoPduStoreName));
	User::LeaveIfError(BaflUtils::DeleteFile(iFs,KNoStreamStoreName));
	User::LeaveIfError(BaflUtils::DeleteFile(iFs,KArrayNoRootStoreName));
	User::LeaveIfError(BaflUtils::DeleteFile(iFs,KArrayNoEntriesStoreName));
	User::LeaveIfError(BaflUtils::DeleteFile(iFs,KCountWrongStoreName));

	delete iReassemblyStore;
	iReassemblyStore = NULL;

    return TestStepResult();
	}


TVerdict CStatusReportTest::doTestStepL()
//
// Status Report testing
//
    {
	_LIT(KSegmentationStoreFileName,"C:\\Private\\101FE442\\smssegst.dat");

	TInt  fileErr = iFs.Delete(KSegmentationStoreFileName);
	if (fileErr != KErrNotFound  &&  fileErr != KErrPathNotFound)
		{
		User::LeaveIfError(fileErr);
		}

    iSegmentationStore=CSmsSegmentationStore::NewL(iFs);

    iSegmentationStore->OpenL(KSegmentationStoreName,KSegmentationStoreUid);
    // TODO iSmsSettings.KSegmentationLifetimeMultiplier() = 1000 ??
    iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);

    TSmsAddr smsAddr;
    smsAddr.SetSmsAddrFamily(ESmsAddrRecvAny);
    CSmsMessage* originalSmsMessage = NULL;
    CSmsMessage* smsMessage;
	
	_LIT(KText,"YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
	
	smsMessage = CreateSmsMessageL(KText,TSmsDataCodingScheme::ESmsAlphabet8Bit);
    CleanupStack::PushL(smsMessage);
    
    TInt count = smsMessage->NumMessagePDUsL();
	
 	CSmsSMSCCtrlParameterOperations& operation =  (CSmsSMSCCtrlParameterOperations&)smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters);
 	operation.SetSchemeL();
    
    operation.SetStatusReportL(0,ESmsStatusReportPermanentError);
   
	CArrayFix<TGsmSms>* smsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
	CleanupStack::PushL(smsArray);
	smsMessage->EncodeMessagePDUsL(*smsArray);
	CleanupStack::PopAndDestroy(smsArray);
    
    CSmsSubmit& submitPdu=(CSmsSubmit&)smsMessage->SmsPDU();
    submitPdu.SetStatusReportRequest(ETrue);

    iSegmentationStore->AddSubmitL( smsAddr, *smsMessage );
    
    for (TInt ii=0;ii<count;ii++)
    	{
    	iSegmentationStore->AddReferenceStatusPairL( *smsMessage, ii, ii);
    	}
        
    CleanupStack::PopAndDestroy(smsMessage); //smsMessage
    
    CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray refStatusArray;
	CleanupClosePushL(refStatusArray);

    smsMessage = CreateSmsMessageL(_L(""),TSmsDataCodingScheme::ESmsAlphabet8Bit,CSmsPDU::ESmsStatusReport);
    CleanupStack::PushL(smsMessage);
    CSmsStatusReport& statusPdu = (CSmsStatusReport&)smsMessage->SmsPDU();
    statusPdu.SetMessageReference(0);
    
    
	TInt index =0;
	TBool isComplete = EFalse;
	
	TBool found=iSegmentationStore->AddStatusReportL(index,isComplete,*smsMessage);
    
    if (found)
        {
        const TSmsSegmentationEntry& entry =
            ( const TSmsSegmentationEntry& )iSegmentationStore->Entries()[index];
        smsMessage->SetLogServerId( entry.LogServerId() );
        CSmsBuffer* buffer = CSmsBuffer::NewL();
        originalSmsMessage = CSmsMessage::NewL( iFs, CSmsPDU::ESmsSubmit, buffer );
    
        iSegmentationStore->GetMessageL( index, smsAddr, *originalSmsMessage, refStatusArray );
        }

    if (isComplete)
        {
        smsMessage->SetTime(originalSmsMessage->Time());
        smsMessage->SetToFromAddressL(originalSmsMessage->ToFromAddress());
        TInt count=refStatusArray.Count();
        CSmsStatusReport& statusreport=(CSmsStatusReport&) smsMessage->SmsPDU();
        for (TInt i=0; i<count; i++)
            {
            const TInt status=refStatusArray[i].Status();
            statusreport.SetStatus((TSmsStatus::TSmsStatusValue) status);
            if (status!=TSmsStatus::ESmsShortMessageReceivedBySME)
                break;
            }
        }

    
    iSegmentationStore->PurgeL(iSmsSettings.KSegmentationLifetimeMultiplier(),EFalse);

    
    if(originalSmsMessage)
        delete originalSmsMessage;
    CleanupStack::PopAndDestroy(2); // refStatusArray, smsMessage
    delete iSegmentationStore;
    iSegmentationStore = NULL;

	return TestStepResult();
	}

TVerdict CSmsPermanentFileStoreTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CSmsPermanentFileStoreTest tests"));

	_LIT(KPermanentStoreName,"C:\\System\\Data\\smsreaclass0st.dat");

	const TInt KStoreUidValue=0x1000089e;
	const TUid KStoreUid={KStoreUidValue};

	//Create Permanent store file
	CSmsPermanentFileStore* permanentFileStore;
	permanentFileStore = CSmsPermanentFileStore::NewL(iFs, KPermanentStoreName, KStoreUid);
	CleanupStack::PushL(permanentFileStore);

	TInt ret=KErrNone;

	TRAP(ret, permanentFileStore->OpenL());
	TEST(ret==KErrNotFound);

	TRAP(ret, permanentFileStore->CreateL());
	TEST(ret==KErrNone);
	permanentFileStore->Close();

	TInt index;
	TInt count;
	TReassemblyEntry entry1;
	TReassemblyEntry entry2;

/*
Test 1
Add 2 new entries. And check whether 2 new entries are added or not.
These functions of CSmsPermanentFileStore are tested in this scenario:
	PopulateEntryArrayL()
	AddNewMessageL()
*/
	{
	ret=KErrNotFound;
	TRAP(ret, permanentFileStore->OpenL());
	TEST(ret==KErrNone);

	count = permanentFileStore->Entries().Count();
	TEST(count==0);

	//Create 2 PDU based message
	SetGlobalsL( _L("+447747065825"),_L("+44385016005"), 200);
	CSmsMessage* smsMsg = CreateSmsMessageL(iMsgBuf->Des(),TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliver);
	CleanupStack::PushL(smsMsg);

	CSmsDeliver& deliverPdu = (CSmsDeliver&)smsMsg->SmsPDU();
	deliverPdu.SetMoreMessagesToSend(EFalse);

	CArrayFix<TGsmSms>* smsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
	CleanupStack::PushL(smsArray);
	smsMsg->EncodeMessagePDUsL(*smsArray);

	//Create a new message to add that to re-assembly store.
	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, smsArray->At(0), buffer, EFalse, ETrue);
	CleanupStack::PushL( smsMessage );

	smsMessage->SmsPDU().SetConcatenatedMessageReference(1);
	smsMessage->SmsPDU().SetConcatenatedMessagePDUIndex(1);
	CReassemblyStoreUtility::PopulateEntry(entry1, *smsMessage, 1);

	index = KErrNotFound;
	permanentFileStore->MatchEntryToExistingMessage(entry1, index);
	TEST(index==KErrNotFound);
	//Add a new message
	permanentFileStore->BeginTransactionL();
	TRAP(ret,permanentFileStore->AddNewMessageL(index,*smsMessage,smsArray->At(0)));
	permanentFileStore->CommitTransactionL();
	TEST(ret==KErrNone);
	TEST(index==0);

	smsMessage->SmsPDU().SetConcatenatedMessageReference(2);
	smsMessage->SmsPDU().SetConcatenatedMessagePDUIndex(1);
	CReassemblyStoreUtility::PopulateEntry(entry2, *smsMessage, 1);

	index = KErrNotFound;
	permanentFileStore->MatchEntryToExistingMessage(entry2, index);
	TEST(index==KErrNotFound);
	//Add another new message
	permanentFileStore->BeginTransactionL();
	TRAP(ret,permanentFileStore->AddNewMessageL(index,*smsMessage,smsArray->At(0)));
	permanentFileStore->CommitTransactionL();
	TEST(ret==KErrNone);
	TEST(index==1);

	count = permanentFileStore->Entries().Count();
	TEST(count==2);

	permanentFileStore->Close();

	CleanupStack::PopAndDestroy(3);	//smsMsg, smsArray, smsMessage
	}

/*
Test 2
Update 2 new existing message.
These functions of CSmsPermanentFileStore are tested in this scenario:
	PopulateEntryArrayL()
	MatchEntryToExistingMessage()
	InternalizeEntryL()
	UpdateExistingMessageL()
*/
	{
	ret=KErrNotFound;
	TRAP(ret, permanentFileStore->OpenL());
	TEST(ret==KErrNone);

	count = permanentFileStore->Entries().Count();
	TEST(count==2);

	//Create 2 PDU based message
	SetGlobalsL( _L("+447747065825"),_L("+44385016005"), 200);
	CSmsMessage* smsMsg = CreateSmsMessageL(iMsgBuf->Des(),TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliver);
	CleanupStack::PushL(smsMsg);

	CSmsDeliver& deliverPdu = (CSmsDeliver&)smsMsg->SmsPDU();
	deliverPdu.SetMoreMessagesToSend(EFalse);

	CArrayFix<TGsmSms>* smsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
	CleanupStack::PushL(smsArray);
	smsMsg->EncodeMessagePDUsL(*smsArray);

	//Create an existing message to add that to re-assembly store.
	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, smsArray->At(1), buffer, EFalse, ETrue);
	CleanupStack::PushL( smsMessage );

	smsMessage->SmsPDU().SetConcatenatedMessageReference(1);
	smsMessage->SmsPDU().SetConcatenatedMessagePDUIndex(2);
	CReassemblyStoreUtility::PopulateEntry(entry1, *smsMessage, 1);

	index = KErrNotFound;
	permanentFileStore->MatchEntryToExistingMessage(entry1, index);
	TEST(index==0);

	CSmsBuffer* tmpBuffer = CSmsBuffer::NewL();
	CSmsMessage* tmpSmsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, tmpBuffer );
	CleanupStack::PushL( tmpSmsMessage );

	CArrayFix<TInt>* tmpIndexArray = new(ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(tmpIndexArray);

	CArrayFix<TGsmSms>* tmpSmsArray = new(ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(tmpSmsArray);

	permanentFileStore->InternalizeEntryL(index, *tmpSmsMessage, *tmpIndexArray, *tmpSmsArray);
	count = tmpIndexArray->Count();
	TEST(count==1);
	count = tmpSmsArray->Count();
	TEST(count==1);

	TInt  concatPDUIndex = smsMessage->SmsPDU().ConcatenatedMessagePDUIndex();
	tmpIndexArray->AppendL(concatPDUIndex);
	tmpSmsArray->AppendL(smsArray->At(1));

	//Update an existing message
	ret = KErrNotFound;
	permanentFileStore->BeginTransactionL();
	TRAP(ret, permanentFileStore->UpdateExistingMessageL(index, *tmpSmsMessage, *tmpIndexArray, *tmpSmsArray));
	permanentFileStore->CommitTransactionL();
	TEST(ret==KErrNone);

	smsMessage->SmsPDU().SetConcatenatedMessageReference(2);
	smsMessage->SmsPDU().SetConcatenatedMessagePDUIndex(2);
	CReassemblyStoreUtility::PopulateEntry(entry2, *smsMessage, 1);

	index = KErrNotFound;
	permanentFileStore->MatchEntryToExistingMessage(entry2, index);
	TEST(index==1);

	tmpIndexArray->Reset();
	tmpSmsArray->Reset();

	permanentFileStore->InternalizeEntryL(index, *tmpSmsMessage, *tmpIndexArray, *tmpSmsArray);
	count = tmpIndexArray->Count();
	TEST(count==1);
	count = tmpSmsArray->Count();
	TEST(count==1);

	concatPDUIndex = smsMessage->SmsPDU().ConcatenatedMessagePDUIndex();
	tmpIndexArray->AppendL(concatPDUIndex);
	tmpSmsArray->AppendL(smsArray->At(1));

	//Update an existing message
	ret = KErrNotFound;
	permanentFileStore->BeginTransactionL();
	TRAP(ret, permanentFileStore->UpdateExistingMessageL(index, *tmpSmsMessage, *tmpIndexArray, *tmpSmsArray));
	permanentFileStore->CommitTransactionL();
	TEST(ret==KErrNone);

	count = permanentFileStore->Entries().Count();
	TEST(count==2);

	permanentFileStore->Close();

	CleanupStack::PopAndDestroy(6);	//smsMsg, smsArray, smsMessage, tmpSmsMessage, tmpIndexArray, tmpSmsArray
	}

/*
Test 3
Call UpdateLogServerIdL(), SetPassedToClient(). And check whether properties are set correctly or not.
These functions of CSmsPermanentFileStore are tested in this scenario:
	MatchEntryToExistingMessage()
	UpdateLogServerIdL()
	SetPassedToClient()
	BeginTransactionLC()
	CommitTransactionL()
	InternalizeEntryL()
*/
	{
	ret = KErrNotFound;
	TRAP(ret, permanentFileStore->OpenL());
	TEST(ret==KErrNone);

	count = permanentFileStore->Entries().Count();
	TEST(count==2);

	index = KErrNotFound;
	permanentFileStore->MatchEntryToExistingMessage(entry1, index);
	TEST(index==0);

	permanentFileStore->BeginTransactionL();
	permanentFileStore->UpdateLogServerIdL(index, 11);
	permanentFileStore->CommitTransactionL();
	permanentFileStore->BeginTransactionL();
	permanentFileStore->SetPassedToClientL(index, ETrue);
	permanentFileStore->CommitTransactionL();

	//call InternalizeEntryL & test updatelogserverid, passedtoclient value.
	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer );
	CleanupStack::PushL( smsMessage );

	CArrayFix<TInt>* indexArray = new(ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(indexArray);

	CArrayFix<TGsmSms>* smsArray = new(ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(smsArray);

	permanentFileStore->InternalizeEntryL(index, *smsMessage, *indexArray, *smsArray);
	count = indexArray->Count();
	TEST(count==2);
	count = smsArray->Count();
	TEST(count==2);

	TInt logId = smsMessage->LogServerId();
	TBool passesToClient = permanentFileStore->Entries()[index].PassedToClient();
	TEST(logId == 11);
	TEST(passesToClient == TRUE);

	permanentFileStore->BeginTransactionL();
	permanentFileStore->UpdateLogServerIdL(index, 12);
	permanentFileStore->CommitTransactionL();
	permanentFileStore->BeginTransactionL();
	permanentFileStore->SetPassedToClientL(index, EFalse);
	permanentFileStore->CommitTransactionL();
	permanentFileStore->Close();

	//After closing it, open it & test again whether logserverid, passedtoclient contains actual value.
	ret = KErrNotFound;
	TRAP(ret, permanentFileStore->OpenL());
	TEST(ret==KErrNone);

	indexArray->Reset();
	smsArray->Reset();
	//call InternalizeEntryL & test updatelogserverid, passedtoclient value.
	permanentFileStore->InternalizeEntryL(index, *smsMessage, *indexArray, *smsArray);
	count = indexArray->Count();
	TEST(count==2);
	count = smsArray->Count();
	TEST(count==2);

	logId = smsMessage->LogServerId();
	passesToClient = permanentFileStore->Entries()[index].PassedToClient();
	TEST(logId == 12);
	TEST(passesToClient == FALSE);

	permanentFileStore->Close();
	CleanupStack::PopAndDestroy(3);	//smsMessage, indexArray, smsArray
	}

/*
Test 4
Test Delete Entries
These functions of CSmsPermanentFileStore are tested in this scenario:
	DeleteEntryL()
*/
	{
	ret = KErrNotFound;
	TRAP(ret, permanentFileStore->OpenL());
	TEST(ret==KErrNone);

	count = permanentFileStore->Entries().Count();
	TEST(count==2);

	permanentFileStore->BeginTransactionL();
	permanentFileStore->DeleteEntryL(0);
	permanentFileStore->CommitTransactionL();

	count = permanentFileStore->Entries().Count();
	TEST(count==1);

	permanentFileStore->Close();

	TRAP(ret, permanentFileStore->OpenL());
	TEST(ret==KErrNone);

	count = permanentFileStore->Entries().Count();
	TEST(count==1);
	permanentFileStore->Close();
	}

	CleanupStack::PopAndDestroy(1);	//permanentFileStore
	BaflUtils::DeleteFile(iFs, KPermanentStoreName);

	return TestStepResult();
	}

TVerdict CPreallocatedFileTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CPreallocatedFileTest tests"));

	_LIT(KPreAllocatedFileStoreName,"C:\\System\\Data\\smsclass0preallocated.dat");

	//Create pre-allocated store file
	TInt maxClass0Msg = 10;
	TInt maxPDUSeg = 20;
	CPreallocatedFile* preAllocatedFile;
	preAllocatedFile = CPreallocatedFile::NewL(iFs, KPreAllocatedFileStoreName, maxClass0Msg, maxPDUSeg);
	CleanupStack::PushL(preAllocatedFile);

	TInt ret=KErrNone;

	TRAP(ret, preAllocatedFile->OpenL());
	TEST(ret==KErrNotFound);

	TRAP(ret, preAllocatedFile->CreateL());
	TEST(ret==KErrNone);
	preAllocatedFile->Close();

	TInt index;
	TInt count;
	TReassemblyEntry entry1;
	TReassemblyEntry entry2;

/*
Test 1
Add 2 new entries. And check whether 2 new entries are added or not.
These functions of CPreallocatedFile are tested in this scenario:
	PopulateEntryArrayL()
	AddNewMessageL()
*/
	{
	ret=KErrNotFound;
	TRAP(ret, preAllocatedFile->OpenL());
	TEST(ret==KErrNone);

	count = preAllocatedFile->Entries().Count();
	TEST(count==0);

	//Create 2 PDU based message
	SetGlobalsL( _L("+447747065825"),_L("+44385016005"), 200);
	CSmsMessage* smsMsg = CreateSmsMessageL(iMsgBuf->Des(),TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliver);
	CleanupStack::PushL(smsMsg);

	CSmsDeliver& deliverPdu = (CSmsDeliver&)smsMsg->SmsPDU();
	deliverPdu.SetMoreMessagesToSend(EFalse);

	CArrayFix<TGsmSms>* smsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
	CleanupStack::PushL(smsArray);
	smsMsg->EncodeMessagePDUsL(*smsArray);

	//Create a new message to add that to re-assembly store.
	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, smsArray->At(0), buffer, EFalse, ETrue);
	CleanupStack::PushL( smsMessage );

	smsMessage->SmsPDU().SetConcatenatedMessageReference(1);
	smsMessage->SmsPDU().SetConcatenatedMessagePDUIndex(1);
	CReassemblyStoreUtility::PopulateEntry(entry1, *smsMessage, 1);

	index = KErrNotFound;
	preAllocatedFile->MatchEntryToExistingMessage(entry1, index);
	TEST(index==KErrNotFound);
	//Add a new message
	preAllocatedFile->BeginTransactionL();
	TRAP(ret,preAllocatedFile->AddNewMessageL(index,*smsMessage,smsArray->At(0)));
	preAllocatedFile->CommitTransactionL();
	TEST(ret==KErrNone);
	TEST(index==0);

	smsMessage->SmsPDU().SetConcatenatedMessageReference(2);
	smsMessage->SmsPDU().SetConcatenatedMessagePDUIndex(1);
	CReassemblyStoreUtility::PopulateEntry(entry2, *smsMessage, 1);

	index = KErrNotFound;
	preAllocatedFile->MatchEntryToExistingMessage(entry2, index);
	TEST(index==KErrNotFound);
	//Add another new message
	preAllocatedFile->BeginTransactionL();
	TRAP(ret,preAllocatedFile->AddNewMessageL(index,*smsMessage,smsArray->At(0)));
	preAllocatedFile->CommitTransactionL();
	TEST(ret==KErrNone);
	TEST(index==1);

	count = preAllocatedFile->Entries().Count();
	TEST(count==2);

	preAllocatedFile->Close();

	CleanupStack::PopAndDestroy(3);	//smsMsg, smsArray, smsMessage
	}

/*
Test 2
Update 2 new existing message.
These functions of CPreallocatedFile are tested in this scenario:
	PopulateEntryArrayL()
	MatchEntryToExistingMessage()
	InternalizeEntryL()
	UpdateExistingMessageL()
*/
	{
	ret=KErrNotFound;
	TRAP(ret, preAllocatedFile->OpenL());
	TEST(ret==KErrNone);

	count = preAllocatedFile->Entries().Count();
	TEST(count==2);

	//Create 2 PDU based message
	SetGlobalsL( _L("+447747065825"),_L("+44385016005"), 200);
	CSmsMessage* smsMsg = CreateSmsMessageL(iMsgBuf->Des(),TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliver);
	CleanupStack::PushL(smsMsg);

	CSmsDeliver& deliverPdu = (CSmsDeliver&)smsMsg->SmsPDU();
	deliverPdu.SetMoreMessagesToSend(EFalse);

	CArrayFix<TGsmSms>* smsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
	CleanupStack::PushL(smsArray);
	smsMsg->EncodeMessagePDUsL(*smsArray);

	//Create an existing message to add that to re-assembly store.
	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, smsArray->At(1), buffer, EFalse, ETrue);
	CleanupStack::PushL( smsMessage );

	smsMessage->SmsPDU().SetConcatenatedMessageReference(1);
	smsMessage->SmsPDU().SetConcatenatedMessagePDUIndex(2);
	CReassemblyStoreUtility::PopulateEntry(entry1, *smsMessage, 1);

	index = KErrNotFound;
	preAllocatedFile->MatchEntryToExistingMessage(entry1, index);
	TEST(index==0);

	CSmsBuffer* tmpBuffer = CSmsBuffer::NewL();
	CSmsMessage* tmpSmsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, tmpBuffer );
	CleanupStack::PushL( tmpSmsMessage );

	CArrayFix<TInt>* tmpIndexArray = new(ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(tmpIndexArray);

	CArrayFix<TGsmSms>* tmpSmsArray = new(ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(tmpSmsArray);

	preAllocatedFile->InternalizeEntryL(index, *tmpSmsMessage, *tmpIndexArray, *tmpSmsArray);
	count = tmpIndexArray->Count();
	TEST(count==1);
	count = tmpSmsArray->Count();
	TEST(count==1);

	TInt  concatPDUIndex = smsMessage->SmsPDU().ConcatenatedMessagePDUIndex();

	//Update an existing message
	ret = KErrNotFound;
	preAllocatedFile->BeginTransactionL();
	TRAP(ret, preAllocatedFile->UpdateExistingMessageL(index, *tmpSmsMessage, concatPDUIndex, smsArray->At(1)));
	preAllocatedFile->CommitTransactionL();
	TEST(ret==KErrNone);

	smsMessage->SmsPDU().SetConcatenatedMessageReference(2);
	smsMessage->SmsPDU().SetConcatenatedMessagePDUIndex(2);
	CReassemblyStoreUtility::PopulateEntry(entry2, *smsMessage, 1);

	index = KErrNotFound;
	preAllocatedFile->MatchEntryToExistingMessage(entry2, index);
	TEST(index==1);

	tmpIndexArray->Reset();
	tmpSmsArray->Reset();

	preAllocatedFile->InternalizeEntryL(index, *tmpSmsMessage, *tmpIndexArray, *tmpSmsArray);
	count = tmpIndexArray->Count();
	TEST(count==1);
	count = tmpSmsArray->Count();
	TEST(count==1);

	concatPDUIndex = smsMessage->SmsPDU().ConcatenatedMessagePDUIndex();

	//Update an existing message
	ret = KErrNotFound;
	preAllocatedFile->BeginTransactionL();
	TRAP(ret, preAllocatedFile->UpdateExistingMessageL(index, *tmpSmsMessage, concatPDUIndex, smsArray->At(1)));
	preAllocatedFile->CommitTransactionL();
	TEST(ret==KErrNone);

	count = preAllocatedFile->Entries().Count();
	TEST(count==2);

	preAllocatedFile->Close();

	CleanupStack::PopAndDestroy(6);	//smsMsg, smsArray, smsMessage, tmpSmsMessage, tmpIndexArray, tmpSmsArray
	}

/*
Test 3
Call UpdateLogServerIdL(), SetPassedToClient(). And check whether properties are set correctly or not.
These functions of CPreallocatedFile are tested in this scenario:
	MatchEntryToExistingMessage()
	UpdateLogServerIdL()
	SetPassedToClient()
	BeginTransactionL()
	CommitTransactionL()
	InternalizeEntryL()
*/
	{
	ret = KErrNotFound;
	TRAP(ret, preAllocatedFile->OpenL());
	TEST(ret==KErrNone);

	count = preAllocatedFile->Entries().Count();
	TEST(count==2);

	index = KErrNotFound;
	preAllocatedFile->MatchEntryToExistingMessage(entry1, index);
	TEST(index==0);

	preAllocatedFile->BeginTransactionL();
	preAllocatedFile->UpdateLogServerIdL(index, 11);
	preAllocatedFile->SetPassedToClientL(index, ETrue);
	preAllocatedFile->CommitTransactionL();

	//call InternalizeEntryL & test updatelogserverid, passedtoclient value.
	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer );
	CleanupStack::PushL( smsMessage );

	CArrayFix<TInt>* indexArray = new(ELeave) CArrayFixFlat<TInt>(8);
	CleanupStack::PushL(indexArray);

	CArrayFix<TGsmSms>* smsArray = new(ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(smsArray);

	preAllocatedFile->InternalizeEntryL(index, *smsMessage, *indexArray, *smsArray);
	count = indexArray->Count();
	TEST(count==2);
	count = smsArray->Count();
	TEST(count==2);

	TInt logId = smsMessage->LogServerId();
	TBool passesToClient = preAllocatedFile->Entries()[index].PassedToClient();
	TEST(logId == 11);
	TEST(passesToClient == TRUE);

	preAllocatedFile->BeginTransactionL();
	preAllocatedFile->UpdateLogServerIdL(index, 12);
	preAllocatedFile->SetPassedToClientL(index, EFalse);
	preAllocatedFile->CommitTransactionL();
	preAllocatedFile->Close();

	//After closing it, open it & test again whether logserverid, passedtoclient contains actual value.
	ret = KErrNotFound;
	TRAP(ret, preAllocatedFile->OpenL());
	TEST(ret==KErrNone);

	indexArray->Reset();
	smsArray->Reset();
	//call InternalizeEntryL & test updatelogserverid, passedtoclient value.
	preAllocatedFile->InternalizeEntryL(index, *smsMessage, *indexArray, *smsArray);
	count = indexArray->Count();
	TEST(count==2);
	count = smsArray->Count();
	TEST(count==2);

	logId = smsMessage->LogServerId();
	passesToClient = preAllocatedFile->Entries()[index].PassedToClient();
	TEST(logId == 12);
	TEST(passesToClient == FALSE);

	preAllocatedFile->Close();
	CleanupStack::PopAndDestroy(3);	//smsMessage, indexArray, smsArray
	}

/*
Test 4
Test Delete Entries
These functions of CPreallocatedFile are tested in this scenario:
	DeleteEntryL()
*/

	{
	ret = KErrNotFound;
	TRAP(ret, preAllocatedFile->OpenL());
	TEST(ret==KErrNone);

	count = preAllocatedFile->Entries().Count();
	TEST(count==2);

	preAllocatedFile->BeginTransactionL();
	preAllocatedFile->DeleteEntryL(0);
	preAllocatedFile->CommitTransactionL();
	count = preAllocatedFile->Entries().Count();
	TEST(count==1);
	preAllocatedFile->Close();

	TRAP(ret, preAllocatedFile->OpenL());
	TEST(ret==KErrNone);

	count = preAllocatedFile->Entries().Count();
	TEST(count==1);
	preAllocatedFile->Close();
	}

	CleanupStack::PopAndDestroy(1);	//preAllocatedFile
	BaflUtils::DeleteFile(iFs, KPreAllocatedFileStoreName);

	return TestStepResult();
	}

TVerdict CClass0ReassemblyStoreTest2::doTestStepPreambleL()
	{
	CSmsStorTestStep::doTestStepPreambleL();

	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);

	iSmsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
	return TestStepResult();
	}

TVerdict CClass0ReassemblyStoreTest2::doTestStepPostambleL()
	{
	iSmsArray->Reset();
	delete iSmsArray;
	iSmsArray = NULL;

	delete iScheduler;
	iScheduler = NULL;

	CSmsStorTestStep::doTestStepPostambleL();
	return TestStepResult();
	}

TVerdict CClass0ReassemblyStoreTest2::doTestStepL()
	{
	TestSingleSegmentMessageL();
	TestMultiSegmentMessageL();
	TestUpdationOfFieldsL();
	TestNumberOfMessageExceedingL();
	TestMaxPDUSegExceedingL();
	TestCodeCoverageForCopyDuplicateToMasterL();
	TestCodeCoverageForRevertOperationsL();
	TestGuardTimerL();
	return TestStepResult();
	}

void CClass0ReassemblyStoreTest2::TestCodeCoverageForRevertOperationsL()
	{
	INFO_PRINTF1(_L("TestCodeCoverageForRevertOperationsL") );
	for (TInt i=0; i<2; i++)
		{
		OpenStoreL();
		if (i==1)
			{
			iClass0ReassemblyStore->SetDiskSpaceState(ESmsDiskSpaceFull);
			}
		TestAddMessageTillHeapAllocDoesNotFailL();
		CloseStore();

		OpenStoreL();
		TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
		TEST(noOfCompleteMessages==2);
		CloseStore();

		if (i==0)
			{
			//Data is stored in permanent store file so check in permanent store file
			TFileName permanentStoreFileName;
			CReassemblyStoreUtility::PrivatePath(iFs, permanentStoreFileName);
			permanentStoreFileName.Append(KClass0ReassemblyStoreName);

			//Open Permanent store file
			CSmsPermanentFileStore* permanentFileStore;
			permanentFileStore = CSmsPermanentFileStore::NewL(iFs, permanentStoreFileName, KClass0ReassemblyStoreUid);
			CleanupStack::PushL(permanentFileStore);

			TInt ret=KErrNotFound;
			TRAP(ret, permanentFileStore->OpenL());
			TEST(ret==KErrNone);
			TInt count = permanentFileStore->Entries().Count();
			TEST(count==2);
			permanentFileStore->Close();
			CleanupStack::PopAndDestroy(1);	//permanentFileStore
			}
		else
			{
			//Data is stored in pre-allocated file so check in pre-allocated file
			TFileName preAllocatedFileName;
			CReassemblyStoreUtility::PrivatePath(iFs, preAllocatedFileName);
			preAllocatedFileName.Append(KPreAllocatedFileName);
			
			CPreallocatedFile* preAllocatedFile;
			preAllocatedFile = CPreallocatedFile::NewL(iFs, preAllocatedFileName, 10, 20);
			CleanupStack::PushL(preAllocatedFile);

			TInt ret=KErrNotFound;
			TRAP(ret, preAllocatedFile->OpenL());
			TEST(ret==KErrNone);
			TInt count = preAllocatedFile->NumberOfPDUStored();
			TEST(count==2);
			preAllocatedFile->Close();
			CleanupStack::PopAndDestroy(1);	//preAllocatedFile
			}
		OpenStoreL();
		TestDeleteMessageTillHeapAllocDoesNotFailL();
		CloseStore();

		OpenStoreL();
		noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
		TEST(noOfCompleteMessages==1);
		CloseStore();

		if (i==0)
			{
			//Data is stored in permanent store file so check in permanent store file
			TFileName permanentStoreFileName;
			CReassemblyStoreUtility::PrivatePath(iFs, permanentStoreFileName);
			permanentStoreFileName.Append(KClass0ReassemblyStoreName);

			//Open Permanent store file
			CSmsPermanentFileStore* permanentFileStore;
			permanentFileStore = CSmsPermanentFileStore::NewL(iFs, permanentStoreFileName, KClass0ReassemblyStoreUid);
			CleanupStack::PushL(permanentFileStore);

			TInt ret=KErrNotFound;
			TRAP(ret, permanentFileStore->OpenL());
			TEST(ret==KErrNone);
			TInt count = permanentFileStore->Entries().Count();
			TEST(count==1);
			permanentFileStore->Close();
			CleanupStack::PopAndDestroy(1);	//permanentFileStore
			}
		else
			{
			//Data is stored in pre-allocated file so check in pre-allocated file
			TFileName preAllocatedFileName;
			CReassemblyStoreUtility::PrivatePath(iFs, preAllocatedFileName);
			preAllocatedFileName.Append(KPreAllocatedFileName);
			
			CPreallocatedFile* preAllocatedFile;
			preAllocatedFile = CPreallocatedFile::NewL(iFs, preAllocatedFileName, 10, 20);
			CleanupStack::PushL(preAllocatedFile);

			TInt ret=KErrNotFound;
			TRAP(ret, preAllocatedFile->OpenL());
			TEST(ret==KErrNone);
			TInt count = preAllocatedFile->NumberOfPDUStored();
			TEST(count==1);
			preAllocatedFile->Close();
			CleanupStack::PopAndDestroy(1);	//preAllocatedFile			
			}
		DeleteClass0ReassemblyStoreFiles();
		}
	}

void CClass0ReassemblyStoreTest2::TestDeleteMessageTillHeapAllocDoesNotFailL()
	{
	TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TESTL(noOfCompleteMessages==2);

	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer );
	CleanupStack::PushL( smsMessage );

	iClass0ReassemblyStore->GetMessageL(1, *smsMessage);

	TInt ret = KErrNone;
	TInt allocFailNumber=0;

	do
		{
		++allocFailNumber;
		__UHEAP_SETFAIL(RHeap::EDeterministic,allocFailNumber);
		TRAP(ret, iClass0ReassemblyStore->DeleteMessageL(*smsMessage, EFalse));
		__UHEAP_RESET;

		// We need to ensure the loaded store reflects 100% what is on the disk!
		CloseStore();
		OpenStoreL();
		}
	while (ret != KErrNone);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TESTL(noOfCompleteMessages==1);

	CleanupStack::PopAndDestroy(1);	//smsMessage
	}

void CClass0ReassemblyStoreTest2::TestAddMessageTillHeapAllocDoesNotFailL()
	{
	TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CreatePDUsForMessageL(1, 0);

	//Single segment will be stored in permanent store file & will be tested.
	CSmsMessage* smsMessage = CreateMessageL(1);
	CleanupStack::PushL(smsMessage);

	TInt index = KErrNotFound;
	TBool isComplete = smsMessage->IsComplete();
	TInt count, total;
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
	TEST(index==0);
	TEST(isComplete==1);
	TEST(count==1);
	TEST(total==1);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==1);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	TInt ret = KErrNone;
	TInt allocFailNumber=0;

	do
		{
		CreatePDUsForMessageL(1, allocFailNumber);
		smsMessage = CreateMessageL(1);
		CleanupStack::PushL(smsMessage);

		__UHEAP_SETFAIL(RHeap::EDeterministic,++allocFailNumber);

		TRAP(ret, iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total));
		__UHEAP_RESET;
		CleanupStack::PopAndDestroy(1);	//smsMessage
		}
	while (ret != KErrNone);

	TEST(index==1);
	TEST(isComplete==1);
	TEST(count==1);
	TEST(total==1);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==2);
	}

void CClass0ReassemblyStoreTest2::TestCodeCoverageForCopyDuplicateToMasterL()
	{
	INFO_PRINTF1(_L("TestCodeCoverageForCopyDuplicateToMasterL") );
	OpenStoreL();

	TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CreatePDUsForMessageL(1, 0);

	//Single segment will be stored in permanent store file & will be tested.
	CSmsMessage* smsMessage = CreateMessageL(1);
	CleanupStack::PushL(smsMessage);

	TInt index = KErrNotFound;
	TBool isComplete = smsMessage->IsComplete();
	TInt count, total;
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
	TEST(index==0);
	TEST(isComplete==1);
	TEST(count==1);
	TEST(total==1);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==1);

	CleanupStack::PopAndDestroy(1);	//smsMessage
	CloseStore();
	//This function will corrupt master header information of pre-allocated file.
	CorruptMasterHeaderInfoL();

	OpenStoreL();
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==1);
	CloseStore();

	DeleteClass0ReassemblyStoreFiles();
	}

void CClass0ReassemblyStoreTest2::CorruptMasterHeaderInfoL()
	{
	TFileName preAllocatedFileName;
	CReassemblyStoreUtility::PrivatePath(iFs, preAllocatedFileName);
	preAllocatedFileName.Append(KPreAllocatedFileName);

	RFile iFile;
	iFile.Open(iFs, preAllocatedFileName, EFileShareExclusive|EFileRead|EFileWrite);

	//Change the number of mesage stored to 0 (to make it corrupt).
	TInt numberOfMessage = 0;
	TPtr8 memPtr((TUint8*) &numberOfMessage, sizeof(numberOfMessage), sizeof(numberOfMessage));
	iFile.Write(4, memPtr);

	iFile.Close();
	}

void CClass0ReassemblyStoreTest2::TestGuardTimerL()
	{
	INFO_PRINTF1(_L("TestGuardTimerL") );
	OpenStoreL();

	iTestMSmsComm2->EnableGuardTimerTest(ETrue);
	iGuardTimerTestStage=0;

	TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CreatePDUsForMessageL(2, 1);

	TInt index, count, total;
	TBool isComplete;

	CSmsMessage* smsMessage = CreateMessageL(1);
	CleanupStack::PushL(smsMessage);
	smsMessage->SetLogServerId(1);

	index = KErrNotFound;
	isComplete = smsMessage->IsComplete();
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
	TEST(index==0);
	TEST(isComplete==0);
	TEST(count==1);
	TEST(total==2);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	iClass0ReassemblyStore->SetDiskSpaceState(ESmsDiskSpaceFull);

	RTimer timer;
	timer.CreateLocal();
	TRequestStatus status;
	//Waiting for 30 secs..
	TTimeIntervalMicroSeconds32 interval = 30*1000000;
	timer.After(status, interval);
	User::WaitForRequest(status);
	if (status.Int()==KErrNone)
		{
		CreatePDUsForMessageL(1, 0);
		smsMessage = CreateMessageL(1);
		CleanupStack::PushL(smsMessage);
		smsMessage->SetLogServerId(2);

		index = KErrNotFound;
		isComplete = smsMessage->IsComplete(); 
		iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
		TEST(index==1);
		TEST(isComplete==1);
		TEST(count==1);
		TEST(total==1);

		CleanupStack::PopAndDestroy(1);	//smsMessage

		noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
		TEST(noOfCompleteMessages==1);
		}

	timer.Close();

	TTime curTime;
	curTime.UniversalTime();
	//Set time by adding current time with 11hrs 59mins
	TTimeIntervalSeconds addTime = (12*3600) - 60;
	TTime setTime = curTime + addTime;
	User::SetUTCTime(setTime);

	//wait for 3 mins (180 secs)
	iTestMSmsComm2->After(180*1000000);
	iScheduler->Start();

	CSmsMessage* tmpSmsMessage=iTestMSmsComm2->GetTestMessage();
	iClass0ReassemblyStore->DeleteMessageL(*tmpSmsMessage, ETrue);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CloseStore();
	DeleteClass0ReassemblyStoreFiles();
	}

void CClass0ReassemblyStoreTest2::TestGuardTimerMessageL()
	{
	INFO_PRINTF1(_L("TestGuardTimerMessageL Message Received") );
	iGuardTimerTestStage++;
	CSmsMessage* tmpSmsMessage=iTestMSmsComm2->GetTestMessage();
	TESTL(tmpSmsMessage!=NULL);

	TInt logServerId = tmpSmsMessage->LogServerId();
	if (iGuardTimerTestStage==1)
		{
		TEST(logServerId==1);
		TEST(tmpSmsMessage->IsComplete()==0);

		CIncompleteClass0MessageInfo& incompClass0MsgInfo = (CIncompleteClass0MessageInfo&) tmpSmsMessage->GetOperationsForNonIEL(ESmsIncompleteClass0MessageParameter);
		TInt stPos, endPos;
		TBool lastSeg;
		incompClass0MsgInfo.GetIncompleteMessageInfoL(stPos, endPos, lastSeg);
		TEST(stPos==1);
		TEST(endPos==1);
		TEST(lastSeg==1);

		TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
		TEST(noOfCompleteMessages==1);
		}
	else
		{
		TEST(logServerId==2);
		TEST(tmpSmsMessage->IsComplete()==1);

		TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
		TEST(noOfCompleteMessages==1);
		}

	if (iGuardTimerTestStage == 2)
		{
		iScheduler->Stop();
		}
	}

void CClass0ReassemblyStoreTest2::StopScheduler()
	{
	//Forcefully make this test fail
	TEST(1==0);
	iScheduler->Stop();
	}

void CClass0ReassemblyStoreTest2::TestMaxPDUSegExceedingL()
	{
	INFO_PRINTF1(_L("TestMaxPDUSegExceedingL") );
	/*
	Create 1 multi segment (15) message & send only the first 14 segment.
	Then add another 7 single segment message.
	Check whether it is receiving the incomplete messsage with first 14
	segments or not. Then add a duplicate message (say 3rd PDU). Check
	whether it is recognizing duplication or not.
	Then add the 15th segment. Check whether it is receiving 
	the last incomplete message with only one segment.
	*/
	OpenStoreL();

	TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	//Below it is little-bit confusing, this function actually creates
	//a message with 15 segments.
	CreatePDUsForMessageL(14, 1);
	CSmsMessage* smsMessage = NULL;

	//TInt MaxMsgAllowed = 10;
	TInt index, count, total;
	TBool isComplete;

	iClass0ReassemblyStore->SetDiskSpaceState(ESmsDiskSpaceFull);

	//Add 14 PDU segments
	for (TInt i=1; i<15; i++)
		{
		smsMessage = CreateMessageL(i);
		CleanupStack::PushL(smsMessage);
		if (i==1)
			{
			smsMessage->SetLogServerId(1);
			}
		index = KErrNotFound;
		isComplete = smsMessage->IsComplete();
		iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(i-1), index, isComplete, EFalse, count, total);
		TEST(index==0);
		TEST(isComplete==0);
		TEST(count==i);
		TEST(total==15);

		noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
		TEST(noOfCompleteMessages==0);

		iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(*iTestMSmsComm2);
		TEST(iTestMSmsComm2->GetTestMessage()==NULL);

		CleanupStack::PopAndDestroy(1);	//smsMessage
		}

	CreatePDUsForMessageL(1, 0);

	RTimer timer;
	timer.CreateLocal();
	TRequestStatus status;
	//Waiting for 2 secs..
	TTimeIntervalMicroSeconds32 interval = 2*1000000;

	//Add 5 New single segment messages.
	for (TInt j=1; j<6; j++)
		{
		smsMessage = CreateMessageL(1);
		CleanupStack::PushL(smsMessage);

		timer.After(status, interval);
		User::WaitForRequest(status);
		if (status.Int()==KErrNone)
			{
			index = KErrNotFound;
			isComplete = smsMessage->IsComplete();
			iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
			TEST(index==j);
			TEST(isComplete==1);
			TEST(count==1);
			TEST(total==1);

			noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
			TEST(noOfCompleteMessages==j);

			iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(*iTestMSmsComm2);
			TEST(iTestMSmsComm2->GetTestMessage()==NULL);
			}

		CleanupStack::PopAndDestroy(1);	//smsMessage
		}

	timer.After(status, interval);
	User::WaitForRequest(status);
	timer.Close();

	//After adding this segment it will touch max allowed segment.
	smsMessage = CreateMessageL(1);
	CleanupStack::PushL(smsMessage);

	index = KErrNotFound;
	isComplete = smsMessage->IsComplete();
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
	TEST(index==6);
	TEST(isComplete==1);
	TEST(count==1);
	TEST(total==1);

	CleanupStack::PopAndDestroy(1);	//smsMessage
	
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==6);

	iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(*iTestMSmsComm2);
	CSmsMessage* tmpSmsMessage=iTestMSmsComm2->GetTestMessage();
	TESTL(tmpSmsMessage!=NULL);
	TEST(tmpSmsMessage->LogServerId()==1);
	isComplete = tmpSmsMessage->IsComplete();
	if (tmpSmsMessage->IsComplete() == EFalse)
		{
		CIncompleteClass0MessageInfo& incompClass0MsgInfo = (CIncompleteClass0MessageInfo&) tmpSmsMessage->GetOperationsForNonIEL(ESmsIncompleteClass0MessageParameter);
		TInt stPos, endPos;
		TBool lastSeg;
		incompClass0MsgInfo.GetIncompleteMessageInfoL(stPos, endPos, lastSeg);
		TEST(stPos==1);
		TEST(endPos==14);
		TEST(lastSeg==0);
		}
	TEST(tmpSmsMessage->IsComplete()==0);
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==6);

	iClass0ReassemblyStore->SetIncompleteMessageForwardedToClientL(*tmpSmsMessage);

	//Below it is little-bit confusing, this function actually creates
	//a message with 15 segments.
	CreatePDUsForMessageL(14, 1);

	smsMessage = CreateMessageL(1);
	CleanupStack::PushL(smsMessage);
	index = KErrNotFound;
	isComplete = smsMessage->IsComplete();
	//Check Duplication
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
	TEST(index==0);
	TEST(isComplete==0);
	TEST(count==14);
	TEST(total==15);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==6);

	iTestMSmsComm2->DeleteTestMessage();
	iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(*iTestMSmsComm2);
	TEST(iTestMSmsComm2->GetTestMessage()==NULL);

	smsMessage = CreateMessageL(15);
	CleanupStack::PushL(smsMessage);
	index = KErrNotFound;
	isComplete = smsMessage->IsComplete();
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(14), index, isComplete, EFalse, count, total);
	TEST(index==0);
	TEST(isComplete==1);
	TEST(count==15);
	TEST(total==15);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==7);

	iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(*iTestMSmsComm2);
	TEST(iTestMSmsComm2->GetTestMessage()==NULL);

	TBuf16<CSmsPDUProcessor::ESmsMaxDeliverReportBufferSize> deliverReportBuffer;
	iClass0ReassemblyStore->ForwardCompleteClass0SmsMessagesL(*iTestMSmsComm2, *smsMessage, NULL, NULL, deliverReportBuffer);
	tmpSmsMessage=iTestMSmsComm2->GetTestMessage();
	TESTL(tmpSmsMessage!=NULL);
	TEST(tmpSmsMessage->LogServerId()==1);
	TESTL(tmpSmsMessage->IsComplete()==0);
	if (tmpSmsMessage->IsComplete() == EFalse)
		{
		CIncompleteClass0MessageInfo& incompClass0MsgInfo = (CIncompleteClass0MessageInfo&) tmpSmsMessage->GetOperationsForNonIEL(ESmsIncompleteClass0MessageParameter);
		TInt stPos, endPos;
		TBool lastSeg;
		incompClass0MsgInfo.GetIncompleteMessageInfoL(stPos, endPos, lastSeg);
		TEST(stPos==15);
		TEST(endPos==15);
		TEST(lastSeg==1);
		}
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==7);

	iClass0ReassemblyStore->DeleteMessageL(*tmpSmsMessage, EFalse);
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==7);

	iClass0ReassemblyStore->DeleteMessageL(*tmpSmsMessage, ETrue);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==6);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	CloseStore();

	OpenStoreL();
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==6);
	CloseStore();

	DeleteClass0ReassemblyStoreFiles();
	}

void CClass0ReassemblyStoreTest2::TestUpdationOfFieldsL()
	{
	INFO_PRINTF1(_L("TestUpdationOfFieldsL") );
	OpenStoreL();
	TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==2);

	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer );
	CleanupStack::PushL( smsMessage );

	iClass0ReassemblyStore->GetMessageL(1, *smsMessage);
	smsMessage->SetLogServerId(10);
	iClass0ReassemblyStore->UpdateLogServerIdOfMessageL(*smsMessage, 1);
	iClass0ReassemblyStore->SetMessagePassedToClientL(*smsMessage);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	buffer = CSmsBuffer::NewL();
	smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer );
	CleanupStack::PushL( smsMessage );

	iClass0ReassemblyStore->GetMessageL(0, *smsMessage);
	//This should not delete the message, because message is not 
	//passed to client. To delete the message 2nd parameter
	//should be EFalse
	iClass0ReassemblyStore->DeleteMessageL(*smsMessage, ETrue);
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==2);

	//This should delete the message
	iClass0ReassemblyStore->DeleteMessageL(*smsMessage, EFalse);
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==1);

	CleanupStack::PopAndDestroy(1);	//smsMessage
	CloseStore();

	OpenStoreL();
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==1);

	buffer = CSmsBuffer::NewL();
	smsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver, buffer );
	CleanupStack::PushL( smsMessage );

	iClass0ReassemblyStore->GetMessageL(0, *smsMessage);
	TEST(smsMessage->LogServerId()==10);

	//This should not delete the message, because message is already 
	//passed to client. To delete the message 2nd parameter
	//should be ETrue
	iClass0ReassemblyStore->DeleteMessageL(*smsMessage, EFalse);
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==1);

	//This should delete the message
	iClass0ReassemblyStore->DeleteMessageL(*smsMessage, ETrue);
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	CloseStore();
	DeleteClass0ReassemblyStoreFiles();
	}

void CClass0ReassemblyStoreTest2::TestMultiSegmentMessageL()
	{
	INFO_PRINTF1(_L("TestMultiSegmentMessageL") );
	OpenStoreL();

	TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CreatePDUsForMessageL(2, 1);

	iClass0ReassemblyStore->SetDiskSpaceState(ESmsDiskSpaceAvailable);

	CSmsMessage* smsMessage = CreateMessageL(1);
	CleanupStack::PushL(smsMessage);

	TInt index = KErrNotFound;
	TBool isComplete = smsMessage->IsComplete();
	TInt count, total;
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
	TEST(index==0);
	TEST(isComplete==EFalse);
	TEST(count==1);
	TEST(total==2);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	iClass0ReassemblyStore->SetDiskSpaceState(ESmsDiskSpaceFull);

	CreatePDUsForMessageL(2, 2);

	smsMessage = CreateMessageL(1);
	CleanupStack::PushL(smsMessage);

	index = KErrNotFound;
	isComplete = smsMessage->IsComplete();
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
	TEST(index==1);
	TEST(isComplete==EFalse);
	TEST(count==1);
	TEST(total==2);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	CreatePDUsForMessageL(2, 1);

	//Create a message to add that to re-assembly store.
	smsMessage = CreateMessageL(2);
	CleanupStack::PushL(smsMessage);

	index = KErrNotFound;
	isComplete = smsMessage->IsComplete();
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(1), index, isComplete, EFalse, count, total);
	TEST(index==0);
	TEST(isComplete==1);
	TEST(count==2);
	TEST(total==2);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==1);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	CreatePDUsForMessageL(2, 2);

	//Create a message to add that to re-assembly store.
	smsMessage = CreateMessageL(2);
	CleanupStack::PushL(smsMessage);

	index = KErrNotFound;
	isComplete = smsMessage->IsComplete();
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(1), index, isComplete, EFalse, count, total);
	TEST(index==1);
	TEST(isComplete==1);
	TEST(count==2);
	TEST(total==2);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==2);

	CleanupStack::PopAndDestroy(1);	//smsMessage
	CloseStore();

	OpenStoreL();
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==2);
	CloseStore();
	}

void CClass0ReassemblyStoreTest2::OpenStoreL()
	{
	iTestMSmsComm2 = CTestMSmsComm2::NewL(iFs, *this);

	iClass0ReassemblyStore = CClass0SmsReassemblyStore::NewL(iFs, *iTestMSmsComm2);

	TInt ret = KErrNone;
	TRAP(ret, iClass0ReassemblyStore->OpenStoreL());
	TESTL(ret == KErrNone);
	}

void CClass0ReassemblyStoreTest2::CloseStore()
	{
	iClass0ReassemblyStore->Close();
	delete iClass0ReassemblyStore;
	iClass0ReassemblyStore = NULL;
	delete iTestMSmsComm2;
	iTestMSmsComm2 = NULL;
	}

void CClass0ReassemblyStoreTest2::DeleteClass0ReassemblyStoreFiles()
	{
	//Delete the cretaed files. (Permanent & Pre-allocated)
	TFileName permanentStoreFileName;
	CReassemblyStoreUtility::PrivatePath(iFs, permanentStoreFileName);
	permanentStoreFileName.Append(KClass0ReassemblyStoreName);
	BaflUtils::DeleteFile(iFs, permanentStoreFileName);

	TFileName preAllocatedFileName;
	CReassemblyStoreUtility::PrivatePath(iFs, preAllocatedFileName);
	preAllocatedFileName.Append(KPreAllocatedFileName);
	BaflUtils::DeleteFile(iFs, preAllocatedFileName);
	}

void CClass0ReassemblyStoreTest2::TestNumberOfMessageExceedingL()
	{
	INFO_PRINTF1(_L("TestNumberOfMessageExceedingL") );
	/*
	Create 11 single segment message & add those to re-assembly store.
	After addition of 11th messsage, check whether oldest message
	is there or not. Also check how many messsages left in re-assembly
	store.
	*/
	OpenStoreL();

	TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CSmsMessage* smsMessage = NULL;

	TInt MaxMsgAllowed = 10;
	TInt index;
	TInt count;
	TInt total;
	TBool isComplete;

	for (TInt i=1; i<=MaxMsgAllowed; i++)
		{
		CreatePDUsForMessageL(1, i);
		smsMessage = CreateMessageL(1);
		CleanupStack::PushL(smsMessage);
		smsMessage->SetLogServerId(i);
		if (i==(MaxMsgAllowed/2))
			{
			iClass0ReassemblyStore->SetDiskSpaceState(ESmsDiskSpaceFull);
			}
		index = KErrNotFound;
		isComplete = smsMessage->IsComplete();
		iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
		TEST(index==i-1);
		TEST(isComplete==1);
		TEST(count==1);
		TEST(total==1);

		noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
		TEST(noOfCompleteMessages==i);

		iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(*iTestMSmsComm2);
		TEST(iTestMSmsComm2->GetTestMessage()==NULL);

		CleanupStack::PopAndDestroy(1);	//smsMessage
		}

	CreatePDUsForMessageL(1, 11);
	smsMessage = CreateMessageL(1);
	CleanupStack::PushL(smsMessage);

	index = KErrNotFound;
	isComplete = smsMessage->IsComplete();
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
	TEST(index==10);
	TEST(isComplete==1);
	TEST(count==1);
	TEST(total==1);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==11);

	iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(*iTestMSmsComm2);
	TESTL(iTestMSmsComm2->GetTestMessage()!=NULL);
	TEST(iTestMSmsComm2->GetTestMessage()->LogServerId()==1);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==10);

	CloseStore();

	OpenStoreL();
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==10);

	CloseStore();
	DeleteClass0ReassemblyStoreFiles();


	/*
	Create 1 multi segment (4) message & send only the first 3 segment.
	Then add another 10 single segment message.
	Check whether it is receiving the incomplete messsage with first 3 
	segments or not. The add the 4th segment. Then it must be added 
	as new messsage with only one PDU received & not complete.
	*/
	OpenStoreL();

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CreatePDUsForMessageL(4, 1);

	for (TInt j=1; j<4; j++)
		{
		smsMessage = CreateMessageL(j);
		CleanupStack::PushL(smsMessage);
		if (j==1)
			{
			iClass0ReassemblyStore->SetDiskSpaceState(ESmsDiskSpaceFull);
			}
		index = KErrNotFound;
		isComplete = smsMessage->IsComplete();
		iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(j-1), index, isComplete, EFalse, count, total);
		TEST(index==0);
		TEST(isComplete==0);
		TEST(count==j);
		TEST(total==4);

		if (j==1)
			{
			smsMessage->SetLogServerId(1);
			iClass0ReassemblyStore->UpdateLogServerIdOfMessageL(*smsMessage,0);
			}

		noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
		TEST(noOfCompleteMessages==0);

		iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(*iTestMSmsComm2);
		TEST(iTestMSmsComm2->GetTestMessage()==NULL);

		CleanupStack::PopAndDestroy(1);	//smsMessage
		}

	iClass0ReassemblyStore->SetDiskSpaceState(ESmsDiskSpaceAvailable);

	for (TInt k=2; k<=MaxMsgAllowed; k++)
		{
		CreatePDUsForMessageL(1, k);
		smsMessage = CreateMessageL(1);
		CleanupStack::PushL(smsMessage);
		smsMessage->SetLogServerId(k);
		if (k==(MaxMsgAllowed/2))
			{
			iClass0ReassemblyStore->SetDiskSpaceState(ESmsDiskSpaceFull);
			}
		index = KErrNotFound;
		isComplete = smsMessage->IsComplete();
		iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
		TEST(index==k-1);
		TEST(isComplete==1);
		TEST(count==1);
		TEST(total==1);

		noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
		TEST(noOfCompleteMessages==k-1);

		iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(*iTestMSmsComm2);
		TEST(iTestMSmsComm2->GetTestMessage()==NULL);

		CleanupStack::PopAndDestroy(1);	//smsMessage
		}

	CreatePDUsForMessageL(1, 11);

	smsMessage = CreateMessageL(1);
	CleanupStack::PushL(smsMessage);

	index = KErrNotFound;
	isComplete = smsMessage->IsComplete();
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
	TEST(index==10);
	TEST(isComplete==1);
	TEST(count==1);
	TEST(total==1);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==10);

	iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(*iTestMSmsComm2);
	CSmsMessage* tmpSmsMessage=iTestMSmsComm2->GetTestMessage();
	TESTL(tmpSmsMessage!=NULL);
	TEST(tmpSmsMessage->LogServerId()==1);
	isComplete = tmpSmsMessage->IsComplete();
	if (tmpSmsMessage->IsComplete() == EFalse)
		{
		CIncompleteClass0MessageInfo& incompClass0MsgInfo = (CIncompleteClass0MessageInfo&) tmpSmsMessage->GetOperationsForNonIEL(ESmsIncompleteClass0MessageParameter);
		TInt stPos, endPos;
		TBool lastSeg;
		incompClass0MsgInfo.GetIncompleteMessageInfoL(stPos, endPos, lastSeg);
		TEST(stPos==1);
		TEST(endPos==3);
		TEST(lastSeg==0);
		}
	TEST(tmpSmsMessage->IsComplete()==0);
	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==10);

	CloseStore();

	OpenStoreL();

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==10);

	CreatePDUsForMessageL(4, 1);

	smsMessage = CreateMessageL(4);
	CleanupStack::PushL(smsMessage);

	index = KErrNotFound;
	isComplete = smsMessage->IsComplete();
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(3), index, isComplete, EFalse, count, total);
	TEST(index==10);
	TEST(isComplete==0);
	TEST(count==1);
	TEST(total==4);

	CleanupStack::PopAndDestroy(1);	//smsMessage

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==10);

	iClass0ReassemblyStore->ProcessMessageIfExceedLimitationL(*iTestMSmsComm2);
	tmpSmsMessage=iTestMSmsComm2->GetTestMessage();
	TESTL(tmpSmsMessage!=NULL);

	CloseStore();

	OpenStoreL();

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==9);

	CloseStore();
	DeleteClass0ReassemblyStoreFiles();
	}

void CClass0ReassemblyStoreTest2::TestSingleSegmentMessageL()
	{
	INFO_PRINTF1(_L("TestSingleSegmentMessageL") );
	OpenStoreL();

	TInt noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);


	CreatePDUsForMessageL(1, 0);

	//Single segment will be stored in permanent store file & will be tested.
	CSmsMessage* smsMessage = CreateMessageL(1);
	CleanupStack::PushL(smsMessage);

	TInt index = KErrNotFound;
	TBool isComplete = smsMessage->IsComplete();
	TInt count, total;
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
	TEST(index==0);
	TEST(isComplete==1);
	TEST(count==1);
	TEST(total==1);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==1);

	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* tmpSmsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
	CleanupStack::PushL( tmpSmsMessage );

	iClass0ReassemblyStore->GetMessageL(index, *tmpSmsMessage);
	TestSmsContentsL(*smsMessage, *tmpSmsMessage);

	iClass0ReassemblyStore->DeleteMessageL(*smsMessage, EFalse);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CleanupStack::PopAndDestroy(2);	//smsMessage, tmpSmsMessage

	//Single segment will be stored in pre-allocated file & will be tested.
	iClass0ReassemblyStore->SetDiskSpaceState(ESmsDiskSpaceFull);

	smsMessage = CreateMessageL(1);
	CleanupStack::PushL(smsMessage);

	index = KErrNotFound;
	isComplete = smsMessage->IsComplete();
	iClass0ReassemblyStore->AddSegmentToReassemblyStoreL(*smsMessage, iSmsArray->At(0), index, isComplete, EFalse, count, total);
	TEST(index==0);
	TEST(isComplete==1);
	TEST(count==1);
	TEST(total==1);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==1);

	buffer = CSmsBuffer::NewL();
	tmpSmsMessage = CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
	CleanupStack::PushL( tmpSmsMessage );

	iClass0ReassemblyStore->GetMessageL(index, *tmpSmsMessage);
	TestSmsContentsL(*smsMessage, *tmpSmsMessage);

	TBuf16<CSmsPDUProcessor::ESmsMaxDeliverReportBufferSize> deliverReportBuffer;
	iClass0ReassemblyStore->ForwardCompleteClass0SmsMessagesL(*iTestMSmsComm2, *tmpSmsMessage, NULL, NULL, deliverReportBuffer);
	TEST(iTestMSmsComm2->GetTestMessage()!=NULL);

	iClass0ReassemblyStore->DeleteMessageL(*smsMessage, EFalse);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==1);

	iClass0ReassemblyStore->DeleteMessageL(*smsMessage, ETrue);

	noOfCompleteMessages = iClass0ReassemblyStore->NumberOfCompleteMessages();
	TEST(noOfCompleteMessages==0);

	CleanupStack::PopAndDestroy(2);	//smsMessage, tmpSmsMessage

	CloseStore();

	DeleteClass0ReassemblyStoreFiles();
	}

void CClass0ReassemblyStoreTest2::CreatePDUsForMessageL(TInt aNoOfPDUs, TInt aMsgRef)
	{
	TInt sizeOfMsg = ((aNoOfPDUs-1) * RMobileSmsMessaging::KGsmTpduSize) + 100;
	SetGlobalsL( _L("+447747065825"),_L("+44385016005"), sizeOfMsg);
	CSmsMessage* smsMsg = CreateSmsMessageL(iMsgBuf->Des(),TSmsDataCodingScheme::ESmsAlphabet7Bit,CSmsPDU::ESmsDeliver);
	CleanupStack::PushL(smsMsg);

	TSmsDataCodingScheme::TSmsClass smsclass;
	smsclass = TSmsDataCodingScheme::ESmsClass0;
	smsMsg->SmsPDU().SetClass(ETrue, smsclass);

	CSmsDeliver& deliverPdu = (CSmsDeliver&)smsMsg->SmsPDU();
	deliverPdu.SetMoreMessagesToSend(EFalse);
	iSmsArray->Reset();
	smsMsg->EncodeMessagePDUsL(*iSmsArray, aMsgRef);
	CleanupStack::PopAndDestroy(1);	//smsMsg
	}

CSmsMessage* CClass0ReassemblyStoreTest2::CreateMessageL(TInt aMsgPDUIndex)
	{
	CSmsBuffer* buffer = CSmsBuffer::NewL();
	CSmsMessage* smsMessage = CSmsMessage::NewL(iFs, iSmsArray->At(aMsgPDUIndex-1), buffer, EFalse, ETrue);
	return smsMessage;
	}

void CClass0ReassemblyStoreTest2::TestSmsContentsL(CSmsMessage& aSms1, CSmsMessage& aSms2)
	{
	CSmsBufferBase& smsBuffer1=aSms1.Buffer();
	TInt bufLen1=smsBuffer1.Length();
	HBufC* textBuf1=HBufC::NewL(bufLen1);
	CleanupStack::PushL(textBuf1);
	TPtr textPtr1(textBuf1->Des());
	smsBuffer1.Extract(textPtr1,0,bufLen1);

	CSmsBufferBase& smsBuffer2=aSms2.Buffer();
	TInt bufLen2=smsBuffer2.Length();
	HBufC* textBuf2=HBufC::NewL(bufLen2);
	CleanupStack::PushL(textBuf2);
	TPtr textPtr2(textBuf2->Des());
	smsBuffer2.Extract(textPtr2,0,bufLen2);

	TEST(textPtr1==textPtr2);
	CleanupStack::PopAndDestroy(2);
	}

CTestMSmsComm2* CTestMSmsComm2::NewL(RFs& aFs, CClass0ReassemblyStoreTest2& aStoreTest)
	{
	CTestMSmsComm2* self = new (ELeave)CTestMSmsComm2(aFs, aStoreTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}

CTestMSmsComm2::CTestMSmsComm2(RFs& aFs, CClass0ReassemblyStoreTest2& aStoreTest)
			: CTimer(KSmsSessionPriority), iFs(aFs), iStoreTest(aStoreTest), 
			iSmsMessage(NULL), iTestGuardTimer(EFalse)
	{
	CActiveScheduler::Add(this);
	}

TInt CTestMSmsComm2::ProcessMessageL(const CSmsMessage& aSmsMessage,const TSmsAddr* /*aOriginalSmsAddr*/,const CSmsMessage* /*aOriginalSmsMessage*/,TDes& /*aDes*/)
	{
	if (iSmsMessage)
		{
		delete iSmsMessage;
		iSmsMessage = NULL;
		}

	CBufSeg* recvbufseg = CBufSeg::NewL(KSmsMaxSegmentLength);
	CleanupStack::PushL(recvbufseg);
	// Attempt to serialize this message into the buffer
	RBufWriteStream writestream(*recvbufseg);
	writestream.Open(*recvbufseg);
	CleanupClosePushL(writestream);
	writestream << aSmsMessage;

	// Initialize the read stream with the buffer
	RBufReadStream readstream(*recvbufseg);
	readstream.Open(*recvbufseg,0);
	CleanupClosePushL(readstream);
	// Create a buffer and message to store the result
	CSmsBufferBase* buffer = CSmsBuffer::NewL();
	iSmsMessage= CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);

	// De-serialize the message from using the read stream
	readstream >> *iSmsMessage;
	CleanupStack::PopAndDestroy(3);		//readstream, writestream, recvbufseg

	if (iTestGuardTimer)
		{
		iStoreTest.TestGuardTimerMessageL();
		}
	return KErrNone;
	};

void CTestMSmsComm2::DeletePDUs(const CArrayFix<TGsmSmsSlotEntry>& /*aSlotArray*/, MSmsMessageObserver* /*aObserver*/)
	{
	
	}

void CTestMSmsComm2::MessageSendCompleted(TInt /*aStatus*/)
	{
	
	}

void CTestMSmsComm2::ModemNotificationCompleted(TInt /*aStatus*/,RPhone::TModemDetection /*aState*/)
	{
	
	}

void CTestMSmsComm2::PhoneEnumerationCompleted(TInt /*aStatus*/)
	{
	
	}

TBool CTestMSmsComm2::ClosingDown() const
	{
	return EFalse;
	}

void CTestMSmsComm2::CloseNowWrap()
	{
	
	}

inline TBool CTestMSmsComm2::ObserverIsPresent(const MSmsMessageObserver& /*aObserver*/) const
	{
	return EFalse;
	}

const RMobilePhone::TMobilePhoneNetworkInfoV1& CTestMSmsComm2::NetworkInfo()const
	{
	return iMobilePhoneNetworkInfo;
	}

TBool CTestMSmsComm2::NetworkInfoAvailable()const
	{
	return EFalse;
	}

TBool CTestMSmsComm2::IsPowerUp()const
	{
	return EFalse;
	}

void CTestMSmsComm2::DiskSpaceMonitorStateChange(TSmsDiskSpaceMonitorStatus /*aStatus*/)
	{
	
	}

void CTestMSmsComm2::DeleteTestMessage() 
	{
	if (iSmsMessage)
		{
		delete iSmsMessage;
		iSmsMessage = NULL;
		}
	}

void CTestMSmsComm2::RunL()
	{
	iStoreTest.StopScheduler();
	} // CGuardTimer::RunL


/**
 *  Runs a set of performance tests of the store.
 * 
 *  @param  aReassemblyStore  A pointer to an opened CSmsReassemblyStore object.
 *  @param  aTestName         Name of the test.
 *  @param  aMsgSize          Size of message to use.
 *  @param  aPduCount         Expected number of PDUs.
 *  @param  aStoreLevel       Number of PDUs in the store before testing.
 */
void CSmsReassemblyStorePerformanceTest::RunPerformanceTestL(CSmsReassemblyStore* aReassemblyStore,
                                                             const TDesC& aTestName, TInt aMsgSize,
                                                             TInt aPduCount, TInt aStoreLevel)
	{
	const TInt  KSmsPerfTestRepeatCount(100);

	//
	// Print the test name...
	//
	INFO_PRINTF2(_L("%S"), &aTestName);

    //
    // Create array to store encoded test PDUs into...
    //
    CArrayFix<TGsmSms>* smsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
    CleanupStack::PushL(smsArray);

    //
    // Fill the store if needed...
    //
    for (TInt pdu = 0;  pdu < aStoreLevel;  pdu++)
    	{
        SetGlobalsL(_L("+447747065825"), _L("+44385016005"), 80);

    	CSmsMessage*  smsMsg = CreateSmsMessageL(iMsgBuf->Des(), TSmsDataCodingScheme::ESmsAlphabet7Bit,
    											 CSmsPDU::ESmsDeliver);
    	CleanupStack::PushL(smsMsg);

    	((CSmsDeliver&)smsMsg->SmsPDU()).SetMoreMessagesToSend(EFalse);
    	
    	smsArray->Reset();
    	TRAPD(err, smsMsg->EncodeMessagePDUsL(*smsArray));
    	TEST(err == KErrNone);
    	TESTL(smsArray->Count() == 1);

       	TInt  index = KErrNotFound;
	       	
       	aReassemblyStore->NewMessagePDUL(index, *smsMsg, smsArray->At(0));
    	TEST(index >= 0); // Actual index increments everytime a PDU is added.

        CleanupStack::PopAndDestroy(smsMsg);
    	}

	//
	// Variables for recording the number of calls made and how long they took.
	//
	TTime   start, end;
	TInt64  timeToMatchPDU  = 0;
	TInt64  timeToAddPDU    = 0;
	TInt64  timeToUpdatePDU = 0;
	TInt64  timeToGetPDU    = 0;
	TInt64  timeToFindPDU   = 0;
	TInt64  timeToDeletePDU = 0;
	TInt    matchPDUCalls   = 0;
	TInt    addPDUCalls     = 0;
	TInt    updatePDUCalls  = 0;
	TInt    getPDUCalls     = 0;
	TInt    findPDUCalls    = 0;
	TInt    deletePDUCalls  = 0;

    for (TInt test = 0;  test < KSmsPerfTestRepeatCount;  test++)
    	{
        TInt  index;

        SetGlobalsL(_L("+447747065825"), _L("+44385016005"), aMsgSize);

    	CSmsMessage*  smsMsg = CreateSmsMessageL(iMsgBuf->Des(), TSmsDataCodingScheme::ESmsAlphabet7Bit,
    											 CSmsPDU::ESmsDeliver);
    	CleanupStack::PushL(smsMsg);

    	((CSmsDeliver&)smsMsg->SmsPDU()).SetMoreMessagesToSend(EFalse);
    	
    	smsArray->Reset();
    	TRAPD(err, smsMsg->EncodeMessagePDUsL(*smsArray));
    	TEST(err == KErrNone);
    	TESTL(smsArray->Count() == aPduCount);

    	for (TInt pdu = 0;  pdu < aPduCount;  pdu++)
    		{
    		//INFO_PRINTF5(_L("test=%d/%d, pdu=%d/%d"), test, KSmsPerfTestRepeatCount, pdu, aPduCount);
    		
	       	index = KErrNotFound;
	       	
	    	CSmsBuffer*  smsBuffer = CSmsBuffer::NewL();
	    	CSmsMessage*  smsPDU = CSmsMessage::NewL(iFs, smsArray->At(pdu), smsBuffer);
	    	CleanupStack::PushL(smsPDU);

	    	if (aPduCount > 1)
	    		{
		       	start.UniversalTime();
		       	aReassemblyStore->MatchPDUToExistingMessage(*smsPDU, index);
		    	end.UniversalTime();
		    	timeToMatchPDU = timeToMatchPDU + end.MicroSecondsFrom(start).Int64();
		    	matchPDUCalls++;
	    		}
			
	    	if (pdu == 0)
	    		{
		       	TEST(index == KErrNotFound);
		       	start.UniversalTime();
		       	aReassemblyStore->NewMessagePDUL(index, *smsPDU, smsArray->At(pdu));
		    	end.UniversalTime();
		    	timeToAddPDU = timeToAddPDU + end.MicroSecondsFrom(start).Int64();
		    	addPDUCalls++;

		    	TEST(index >= 0); // Actual index increments everytime a PDU is added.
		    	
		    	//
		    	// Update the Log Server ID and time so the message can be
		    	// found in the store later.
		    	//
		    	TSAREntry  entry = aReassemblyStore->Entries()[index];

		    	smsMsg->SetLogServerId(entry.LogServerId());
		        smsMsg->SetTime(entry.Time());
	    		}
	    	else
	    		{
	    		TBool  isComplete        = EFalse;
	    		TBool  isDuplicateMsgRef = EFalse;
	    		TBool  isDuplicateSlot   = EFalse;
	    		
		       	start.UniversalTime();
		       	aReassemblyStore->UpdateExistingMessageL(*smsPDU, smsArray->At(pdu), index,
														 isComplete, isDuplicateMsgRef,
														 isDuplicateSlot);
		    	end.UniversalTime();
		    	timeToUpdatePDU = timeToUpdatePDU + end.MicroSecondsFrom(start).Int64();
		    	updatePDUCalls++;

		    	TEST(isComplete == (pdu == aPduCount - 1));
		    	TEST(isDuplicateMsgRef == EFalse);
		    	TEST(isDuplicateSlot == EFalse);
		    	
		       	start.UniversalTime();
		       	aReassemblyStore->GetMessageL(index, *smsMsg);
		    	end.UniversalTime();
		    	timeToGetPDU = timeToGetPDU + end.MicroSecondsFrom(start).Int64();
		    	getPDUCalls++;
				}

	    	CleanupStack::PopAndDestroy(smsPDU);
   		}
	
        //
        // Test finding the message...
        //
        TInt   foundIndex(KErrNotFound);
        TBool  found(EFalse);

        start.UniversalTime();
        found = aReassemblyStore->FindMessageL(*smsMsg, EFalse, foundIndex);
        end.UniversalTime();
    	timeToFindPDU = timeToFindPDU + end.MicroSecondsFrom(start).Int64();
    	findPDUCalls++;

        TEST(found != EFalse);
        TEST(foundIndex == index);
    	
        CleanupStack::PopAndDestroy(smsMsg);

        //
        // Delete this entry (otherwise the store may behave differently next time).
        //
        aReassemblyStore->BeginTransactionLC();
       	start.UniversalTime();
       	aReassemblyStore->DeleteEntryL(index);
    	end.UniversalTime();
        aReassemblyStore->CommitTransactionL();
    	timeToDeletePDU = timeToDeletePDU + end.MicroSecondsFrom(start).Int64();
    	deletePDUCalls++;
        
        //
        // Ensure we reset the store back to empty if not testing with a filled store...
        //
        if (aStoreLevel == 0)
        	{
        	aReassemblyStore->PurgeL(iSmsSettings.ReassemblyLifetime(), ETrue);
	        aReassemblyStore->BeginTransactionLC();
	    	aReassemblyStore->CompactL();
	        aReassemblyStore->CommitTransactionL();
    		}
    	}

	CleanupStack::PopAndDestroy(smsArray);

	//
	// Output any average call times that we have obtained during testing...
	//
	if (matchPDUCalls > 0)
		{
		INFO_PRINTF2(_L("  Average call to CSmsReassemblyStore::MatchPDUToExistingMessage() took %.3fs"),
			 	     I64REAL(timeToMatchPDU) / (TReal(1000000.0) * TReal(matchPDUCalls)));
		}
	
	if (addPDUCalls > 0)
		{
		INFO_PRINTF2(_L("  Average call to CSmsReassemblyStore::NewMessagePDUL() took %.3fs"),
				 	 I64REAL(timeToAddPDU) / (TReal(1000000.0) * TReal(addPDUCalls)));
		}
	
	if (updatePDUCalls > 0)
		{
		INFO_PRINTF2(_L("  Average call to CSmsReassemblyStore::UpdateExistingMessageL() took %.3fs"),
			 	     I64REAL(timeToUpdatePDU) / (TReal(1000000.0) * TReal(updatePDUCalls)));
		}
	
	if (getPDUCalls > 0)
		{
		INFO_PRINTF2(_L("  Average call to CSmsReassemblyStore::GetMessageL() took %.3fs"),
			 	     I64REAL(timeToGetPDU) / (TReal(1000000.0) * TReal(getPDUCalls)));
		}
	
	if (findPDUCalls > 0)
		{
		INFO_PRINTF2(_L("  Average call to CSmsReassemblyStore::FindMessageL() took %.3fs"),
			 	     I64REAL(timeToFindPDU) / (TReal(1000000.0) * TReal(findPDUCalls)));
		}
	
	if (deletePDUCalls > 0)
		{
		INFO_PRINTF2(_L("  Average call to CSmsReassemblyStore::DeleteEntryL() took %.3fs"),
			 	     I64REAL(timeToDeletePDU) / (TReal(1000000.0) * TReal(deletePDUCalls)));
		}
    } // CSmsReassemblyStorePerformanceTest::RunPerformanceTestL

	
/**
 *  Perform a number of store operations of various sized PDUs in different scenarios.
 */
TVerdict CSmsReassemblyStorePerformanceTest::doTestStepL()
	{
	//
	// Delete segmentation and reassembly store files before the test...
	//
	_LIT(KReassemblyStoreFileName, "C:\\Private\\101FE442\\smsreast.dat");

	TInt  fileErr = iFs.Delete(KReassemblyStoreFileName);
	if (fileErr != KErrNotFound  &&  fileErr != KErrPathNotFound)
		{
		User::LeaveIfError(fileErr);
		}

	//
	// Initialisation of the reassembly store...
	//
	CSmsReassemblyStore*  reassemblyStore = CSmsReassemblyStore::NewL(iFs);
    CleanupStack::PushL(reassemblyStore);

	reassemblyStore->OpenL(KReassemblyStoreFileName, KReassemblyStoreUid);
	reassemblyStore->PurgeL(iSmsSettings.ReassemblyLifetime(), ETrue);
	reassemblyStore->BeginTransactionLC();
	reassemblyStore->CompactL();
	reassemblyStore->CommitTransactionL();
	
    //
    // Run various tests.
    //
	RunPerformanceTestL(reassemblyStore,
	                    _L("Test average responce to add a new single PDU message segment in an empty store..."),
			            80, 1, 0);
	RunPerformanceTestL(reassemblyStore,
	                    _L("Test average responce to add and find a new four-PDU message segment in an empty store..."),
					    600, 4, 0);
	RunPerformanceTestL(reassemblyStore,
            			_L("Test average responce to finding a new single PDU message segment in a average store..."),
            			80, 1, 10);
	RunPerformanceTestL(reassemblyStore,
	            		_L("Test average responce to finding a new four-PDU message segment in a average store..."),
	            		600, 4, 10);
	RunPerformanceTestL(reassemblyStore,
	        			_L("Test average responce to finding a new single PDU message segment in a excessive store..."),
	        			80, 1, 400);
	RunPerformanceTestL(reassemblyStore,
	        			_L("Test average responce to finding a new four-PDU message segment in a excessive store..."),
	        			600, 4, 400);

	//
	// Clean up...
	//
	reassemblyStore->Close();
	CleanupStack::PopAndDestroy(reassemblyStore);

    return TestStepResult();
    } // CSmsReassemblyStorePerformanceTest::doTestStepL


