
// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#include "TE_testR6SMS.h"

#include "TE_R6SMSbase.h"
#include "Gsmumsg.h"
#include "gsmumsgadditionalattributes.h"
#include "Gsmuelem.h"
#include "gsmuset.h"
#include "EMSInformationElement.h"
#include "EMSFormatIE.h"

TVerdict CTestCSmsMessageAdditionalAttributes1::doTestStepL()
    {
    /**
     *  Test 1: The class CSmsMessageAdditionalAttributes is a container class used to store
     *  the CSmsMessage's new attributes so that CSmsMessage maintains Data Compatibility.
     *  
     *  CSmsMessageAdditionalAttributes contains a set of queues which are used to store
     *  the control information elements needed by release 6 SMS.
     *  
     *  This test case performs the following initial tests.
     *  
     *  (1) Queries for the 1st entry when the queue is empty.
     *  (2) Checks that an attempt to add a NULL information element can be handled (exception scenario).
     *  (3) Checks that the 1st information element can be added successfully.
     *  (4) Checks that a series of attempts to add information elements using invalid input parameters can be handled.
     *  (5) Checks that a search for a not existent information element can be handled.
     *  (6) Checks that a search can find the information element using the correct input parameters.
     *  (7) Checks that a search for a second instance of the same information element returns EFalse.
     *  (8) Checks that a search for a second instance of a non existent information element returns EFalse
     *  (9) Gets a reference to the information element contained on the queue.
     *  (10) Attempts to reference the information element at an index which is out of range.
     *  (11) Attempts to index an non existent type of information element.
     *  (12) Attempt to locate a non existent type of information element.
     *  (13) Attempts to get reference to an invalid category of information element.
     *  (14) Attempts to a get a reference to the correct category of information element but one that is out range.
     *  (15) Attempts to remove the information element from the queue, but uses various combinations of invalid input parameters.
     *  (16) Removes the information element using the required input parameters.
     *  (17) Adds 2 new information elements to the queue
     *  (18) Locates the second information element.
     *  (19) Resets the class attributes
     *  (20) Attempt to access an invalid type of operation - done here to get the branch coverage.
     */
    __UHEAP_MARK;

    CSmsMessageAdditionalAttributes* aCollectionOfAttributes = CSmsMessageAdditionalAttributes::NewL();

    CleanupStack::PushL(aCollectionOfAttributes);

    _LIT8(KIeData, "hi");
    TBuf8<3> data(KIeData);
    CSmsInformationElement* aIE = CSmsInformationElement::NewL(CSmsInformationElement::ESmsIEISCSpecificUse1, KIeData);
    CleanupStack::PushL(aIE);

    // (1) Queries for the 1st entry when the queue is empty.
    TBool rc = ETrue;
    TUint index, startIndex = 0xFFFF;
	rc = aCollectionOfAttributes->Find1stInstanceOfControlInformationElement(CSmsInformationElement::ESmsIEISCSpecificUse1, index);
    TEST(rc == EFalse);

    // (2) Checks that an attempt to add a NULL information element can be handled (exception scenario).
    TRAPD(err, aCollectionOfAttributes->AddControlInformationElementL(NULL));
    TEST(err == KErrArgument);

    // (3) Checks that the 1st information element can be added successfully.
    TRAP(err,aCollectionOfAttributes->AddControlInformationElementL(aIE));
    TEST(err == KErrNone);
    CleanupStack::Pop(aIE);

    // (4) Checks that a series of attempts to add information elements using invalid input parameters can be handled.
    CSmsInformationElement* aIE1 = CSmsInformationElement::NewL(CSmsInformationElement::ESmsIEISCSpecificUse2, KIeData);
    TRAP(err,aCollectionOfAttributes->AddControlInformationElementL((TSmsInformationElementCategories::TInformationElementCategory) 10, aIE1));
    TEST(err == KErrArgument);
    TRAP(err,aCollectionOfAttributes->AddControlInformationElementL(TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed, NULL));
    TEST(err == KErrArgument);
    TRAP(err,aCollectionOfAttributes->AddControlInformationElementL((TSmsInformationElementCategories::TInformationElementCategory) 10, NULL));
    TEST(err == KErrArgument);

	// (5) Checks that a search for a not existent information element can be handled.
	rc = aCollectionOfAttributes->Find1stInstanceOfControlInformationElement(CSmsInformationElement::ESmsIEISCSpecificUse2, index);
	TEST(rc == EFalse);

    // (6) Checks that a search can find the information element using the correct input parameters.
	rc = aCollectionOfAttributes->Find1stInstanceOfControlInformationElement(CSmsInformationElement::ESmsIEISCSpecificUse1, index);
	TEST(rc != EFalse);

	// (7) Checks that a search for a second instance of the same information element returns false.
	startIndex = index;
    rc = aCollectionOfAttributes->FindNextInstanceOfControlInformationElement(CSmsInformationElement::ESmsIEISCSpecificUse1, 1, index);
    TEST(rc == EFalse);

    // (8) Checks that a search for a second instance of a non existent information element returns false
	startIndex = 0;
    rc = aCollectionOfAttributes->FindNextInstanceOfControlInformationElement(CSmsInformationElement::ESmsIEISCSpecificUse2, 0, index);
    TEST(rc == EFalse);

    // (9) Gets a reference to the information element contained on the queue.
    CSmsInformationElement& aIEReference = aCollectionOfAttributes->GetControlInformationElementL(CSmsInformationElement::ESmsIEISCSpecificUse1, 0);

    // (10) Attempts to reference the information element at an index which is out of range.
    TRAP(err,aCollectionOfAttributes->GetControlInformationElementL(CSmsInformationElement::ESmsIEISCSpecificUse1, 1));
    TEST(err == KErrArgument);

    // (11) Attempts to index an non existent type of information element.
    TRAP(err,aCollectionOfAttributes->GetControlInformationElementL(CSmsInformationElement::ESmsIEISCSpecificUse2, 0));
    TEST(err == KErrArgument);

    // (12) Attempt to locate a non existent type of information element.
    TRAP(err,aCollectionOfAttributes->GetControlInformationElementL((TSmsInformationElementCategories::TInformationElementCategory) 10, 0));
    TEST(err == KErrArgument);

    // (13) Attempts to get reference to an invalid category of information element.
    TRAP(err,aCollectionOfAttributes->GetControlInformationElementL((TSmsInformationElementCategories::TInformationElementCategory) 0, 10));
    TEST(err == KErrArgument);

    CSmsInformationElement*  aInformationElement = NULL;
    CSmsInformationElement*& extractedInformationElement = aInformationElement;

    // (15) Attempts to remove the information element from the queue, but uses various combinations of invalid input parameters.
    rc = aCollectionOfAttributes->RemoveControlInformationElement(CSmsInformationElement::ESmsIEISCSpecificUse2, startIndex, extractedInformationElement);
    TEST(rc == EFalse);
    rc = aCollectionOfAttributes->RemoveControlInformationElement(CSmsInformationElement::ESmsIEISCSpecificUse1, 1, extractedInformationElement);
    TEST(rc == EFalse);
    rc = aCollectionOfAttributes->RemoveControlInformationElement(CSmsInformationElement::ESmsIEISCSpecificUse2, 1, extractedInformationElement);
    TEST(rc == EFalse);

    // (16) Removes the information element using the required input parameters.
    aCollectionOfAttributes->RemoveControlInformationElement(CSmsInformationElement::ESmsIEISCSpecificUse1, startIndex, extractedInformationElement);
    TEST(aIE == extractedInformationElement);

    // (17) Adds 2 new information elements to the queue
    TRAP(err,aCollectionOfAttributes->AddControlInformationElementL(aIE));
    TEST(err==KErrNone);
    TRAP(err,aCollectionOfAttributes->AddControlInformationElementL(aIE1));
    TEST(err==KErrNone);
    // (18) Locates the second information element.
    rc = aCollectionOfAttributes->FindNextInstanceOfControlInformationElement(CSmsInformationElement::ESmsIEISCSpecificUse1, 0, index);
    TEST(rc == EFalse);

    // (19) Resets the class attributes
    aCollectionOfAttributes->ResetAttributesL();

    // (20) Attempt to access an invalid type of operation
    //      Do this here to get the branch coverage
    TRAP(err,aCollectionOfAttributes->SetIEOperationL(NULL));
    TEST(err==KErrArgument);

    CleanupStack::Pop(aCollectionOfAttributes);
    delete aCollectionOfAttributes;

    __UHEAP_MARKEND;

	return TestStepResult();
    }


TVerdict CTestCSmsMessageAdditionalAttributes2::doTestStepL()
	{
    /**
     *  Test CSmsMessageAdditionalAttributes
     *  
     *  Test Scenario:
     *  
     *  Verify that 2 elements of the same type can be added to the collection, indexed and deleted.
     *  
     *  (1) Create 2 information elements and add to collection
     *  (2) Find 1st instance
     *  (3) Find 2nd instance
     *  (4) Get references
     *  (5) Remove and delete 1st element
     *  (6) Remove and delete 2nd instance
     */
    __UHEAP_MARK;

    CSmsMessageAdditionalAttributes* aCollectionOfAttributes = CSmsMessageAdditionalAttributes::NewL();
    CleanupStack::PushL(aCollectionOfAttributes);

    // (1) Create 2 information elements and add to collection
    _LIT8(KIeData, "hi");
    TBuf8<3> data(KIeData);
    CSmsInformationElement* aIE = CSmsInformationElement::NewL(CSmsInformationElement::ESmsHyperLinkFormat, KIeData);
    CleanupStack::PushL(aIE);
    TRAPD(err,aCollectionOfAttributes->AddControlInformationElementL(aIE));
    TEST(err == KErrNone);
    CleanupStack::Pop(aIE);

    CSmsInformationElement* aIE1 = CSmsInformationElement::NewL(CSmsInformationElement::ESmsHyperLinkFormat, KIeData);
    CleanupStack::PushL(aIE1);
    TRAP(err,aCollectionOfAttributes->AddControlInformationElementL(aIE1));
    TEST(err == KErrNone);
    CleanupStack::Pop(aIE1);

    // (2) Find 1st instance
    TUint index, startIndex;
    TUint a2ndIndex = 0xFFFF;
	TBool found = aCollectionOfAttributes->Find1stInstanceOfControlInformationElement(CSmsInformationElement::ESmsHyperLinkFormat, index);
    TEST (found != EFalse);

    // (3) Find 2nd instance
	startIndex = index;
    found = aCollectionOfAttributes->FindNextInstanceOfControlInformationElement(CSmsInformationElement::ESmsHyperLinkFormat, startIndex, a2ndIndex);
    TEST (found != EFalse);
    TEST (a2ndIndex == 1);

    // (4) Get references
    TRAP(err, CSmsInformationElement&  aIEReference  = aCollectionOfAttributes->GetControlInformationElementL(CSmsInformationElement::ESmsHyperLinkFormat, startIndex));
    TEST(err == KErrNone);
    TRAP(err, CSmsInformationElement&  aIEReference1 = aCollectionOfAttributes->GetControlInformationElementL(CSmsInformationElement::ESmsHyperLinkFormat, a2ndIndex));
    TEST(err == KErrNone);

    CSmsInformationElement*  aInformationElement = NULL;
    CSmsInformationElement*& extractedInformationElement = aInformationElement;

    // (5) Remove and delete 1st element
    TBool deleted = aCollectionOfAttributes->RemoveControlInformationElement(CSmsInformationElement::ESmsHyperLinkFormat, startIndex, extractedInformationElement);
    TEST (deleted != EFalse);
    TEST (aIE == extractedInformationElement);

    delete aIE;

    // (6) Remove and delete 2nd instance
    deleted = aCollectionOfAttributes->RemoveControlInformationElement(CSmsInformationElement::ESmsHyperLinkFormat,
                                                                       --a2ndIndex,
                                                                       extractedInformationElement);
    TEST (deleted != EFalse);
    TEST (aIE1 == extractedInformationElement);

    delete aIE1;
    CleanupStack::Pop(aCollectionOfAttributes);
    delete aCollectionOfAttributes;

    __UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTestCSmsMessageAdditionalAttributes3::doTestStepL()
	{
    /**
     *  Test CSmsMessageAdditionalAttributes, client facing methods.
     *  
     *  Verify that CSmsMessageAdditionalAttributes can handle the following requests:
     *  
     *  (1) the addition of 2 elements to one of its collections.
     *  (2) an attempt to access information elements specifying an invalid (out of range) queue ID.
     *  (3) a request to remove the control information element at back of the queue, transfering ownership to the client.
     *  (4) a request to remove the control information element remaining on the queue, transfering ownership to the client.
     *  (5) a request to remove a control information element from the now empty queue.
     *  
     */
    __UHEAP_MARK;

    CSmsMessageAdditionalAttributes* aCollectionOfAttributes = CSmsMessageAdditionalAttributes::NewL();
    CleanupStack::PushL(aCollectionOfAttributes);

    // (1) Create 2 information elements and add to collection
    _LIT8(KIeData, "hi");
    TBuf8<3> data(KIeData);

    CSmsInformationElement* aIE =  CSmsInformationElement::NewL(CSmsInformationElement::ESmsHyperLinkFormat, KIeData);
    CleanupStack::PushL(aIE);
    TRAPD(err,aCollectionOfAttributes->AddControlInformationElementL(aIE));
    TEST(err == KErrNone);
    CleanupStack::Pop(aIE);

    CSmsInformationElement* aIE1 = CSmsInformationElement::NewL(CSmsInformationElement::ESmsHyperLinkFormat, KIeData);
    CleanupStack::PushL(aIE1);
    TRAP(err,aCollectionOfAttributes->AddControlInformationElementL(aIE1));
    TEST(err == KErrNone);
    CleanupStack::Pop(aIE1);

    CSmsInformationElement* IEptr = NULL;
    TBool rc;
    rc = aCollectionOfAttributes->RemoveNextControlInformationElement((TSmsInformationElementCategories::TInformationElementCategory) 10, IEptr);
    TEST(rc == EFalse);
    rc = aCollectionOfAttributes->RemoveNextControlInformationElement(TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed, IEptr);
    TEST(rc != EFalse);
    delete IEptr;
    IEptr = NULL;
    rc = aCollectionOfAttributes->RemoveNextControlInformationElement(TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed, IEptr);
    TEST(rc != EFalse);
    delete IEptr;
    IEptr = NULL;
    rc = aCollectionOfAttributes->RemoveNextControlInformationElement(TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed, IEptr);
    TEST(rc == EFalse);

    CleanupStack::Pop(aCollectionOfAttributes);
    delete aCollectionOfAttributes;

    __UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTestR6CSmsMessage::doTestStepL()
    {
    /**
     *  Synopsis: Test New R6 CSmsMessage interfaces.
     *  
     *  Provide branch coverage.
     *  
     *  (1) Create all valid operations classes and one invalid operations class.
     *  
     *  (2) Test "Get" and "Set" methods that indicate that indicate whether
     *  a particular message originated on the SIM and whether it should be forwarded
     *  by the stack to its client.
     *  
     */
    RFs fs;
    User::LeaveIfError(fs.Connect());

    __UHEAP_MARK;

    _LIT16(KName, "David Narey");
    CSmsBuffer* buffer=CSmsBuffer::NewL();
    buffer->InsertL(0,KName);

    CSmsMessage* smsMessage = CSmsMessage::NewL(fs, CSmsPDU::ESmsSubmit, buffer);

    TRAPD(err, smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));
    TEST(err == KErrNone);
    TRAP(err, smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsHyperLinkFormat));
    TEST(err == KErrNone);
    TRAP(err, smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsReplyAddressFormat));
    TEST(err == KErrNone);
    TRAP(err, smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));
    TEST(err == KErrNone);
    TRAP(err, smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhancedextendedObjectDataRequest));
    TEST(err == KErrArgument);

    smsMessage->SetDecodedOnSIM(EFalse);
    TEST(smsMessage->DecodedOnSim() == EFalse);
    smsMessage->SetDecodedOnSIM(ETrue);
    TEST(smsMessage->DecodedOnSim() != EFalse);
    smsMessage->SetForwardToClient(EFalse);
    TEST(smsMessage->ForwardToClient() == EFalse);
    smsMessage->SetForwardToClient(ETrue);
    TEST(smsMessage->ForwardToClient() != EFalse);

    delete smsMessage;

    __UHEAP_MARKEND;
	return TestStepResult();
    }


TVerdict CTestAddingIEsViaNewAndExistingInterfaces::doTestStepL()
    {
    /**
     *  
     *  Encode and Decode a Special SMS Message Indication into a single PDU
     *  verifying the use of the new Special Message Waiting API and the existing
     *  interface used by Special Message Waiting and other control information elements.
     */
    RFs fs;
    User::LeaveIfError(fs.Connect());

    __UHEAP_MARK;

    CSmsBuffer* buffer=CSmsBuffer::NewL();

    CSmsMessage* message = CSmsMessage::NewL(fs, CSmsPDU::ESmsSubmit, buffer );
    CleanupStack::PushL(message);

    TSmsUserDataSettings userDataSettings;
    userDataSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
    message->SetUserDataSettingsL(userDataSettings);

    // Get a handle to the user data
    CSmsPDU& submitPDU = message->SmsPDU();
    CSmsUserData& userData = submitPDU.UserData();

    // create a data field for undefined information elements.
    _LIT8(KUndefinedIE, "\xAA\xBB\xCC");

    // Check that the user data interface does not support the addition of information elements whose values have not yet been defined.
    TRAPD(err,userData.AddInformationElementL((CSmsInformationElement::TSmsInformationElementIdentifier)0x21,KUndefinedIE));
    TEST(err==KErrNotSupported);
    TRAP(err, userData.AddInformationElementL((CSmsInformationElement::TSmsInformationElementIdentifier)0x6F,KUndefinedIE));
    TEST(err==KErrNotSupported);
    TRAP(err, userData.AddInformationElementL((CSmsInformationElement::TSmsInformationElementIdentifier)0xA0,KUndefinedIE));
    TEST(err==KErrNotSupported);
    TRAP(err, userData.AddInformationElementL((CSmsInformationElement::TSmsInformationElementIdentifier)0xBF,KUndefinedIE));
    TEST(err==KErrNotSupported);
    TRAP(err, userData.AddInformationElementL((CSmsInformationElement::TSmsInformationElementIdentifier)0xE0,KUndefinedIE));
    TEST(err==KErrNotSupported);
    TRAP(err, userData.AddInformationElementL((CSmsInformationElement::TSmsInformationElementIdentifier)0xFF,KUndefinedIE));
    TEST(err==KErrNotSupported);

    // add 1st special information element
    _LIT8(K1stSpecialMessageIE,"\x00\x01");
    TRAP(err,userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,K1stSpecialMessageIE));
    TEST(err==KErrNone);
    // check the same special message cannot be added again
    // add 1st special information element (modified value)
    _LIT8(K1stSpecialMessageIEWithNewCount,"\x00\x02");
    TRAP(err,userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,K1stSpecialMessageIEWithNewCount));
    TEST(err==KErrAlreadyExists);

    // add 2nd special information element
    _LIT8(K2ndSpecialMessageIE,"\x01\x03");
    // add 3rd special information element
    _LIT8(K3rdSpecialMessageIE,"\x02\x04");
    // add 4th special information element
    _LIT8(K4thSpecialMessageIE,"\x07\x05");

    TRAP(err,userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,K2ndSpecialMessageIE));
    TEST(err==KErrNone);
    TRAP(err,userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,K3rdSpecialMessageIE));
    TEST(err==KErrNone);
    TRAP(err,userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,K4thSpecialMessageIE));
    TEST(err==KErrNone);

    // add mandatory information element
    _LIT8(KReplyAddress,"\x0C\x91\x53\x48\x80\x14\x55\x82");
    CSmsInformationElement* iE = CSmsInformationElement::NewL(CSmsInformationElement::ESmsReplyAddressFormat, KReplyAddress);
    CleanupStack::PushL(iE);
    CSmsMessageAdditionalAttributes* additionalAttributes = (CSmsMessageAdditionalAttributes*) message->AdditionalInfo();
    TRAP(err,additionalAttributes->AddControlInformationElementL(iE));
    TEST(err==KErrNone);
    CleanupStack::Pop(iE);

    // Increase the branch coverage by exercising the method UpdateInformationElementArrayL() which the client cannot call directly.
    TRAP(err,userData.UpdateInformationElementArrayL(CSmsInformationElement::ESmsReplyAddressFormat, KReplyAddress));
    TEST(err == KErrNone);
    TRAP(err,userData.UpdateInformationElementArrayL(CSmsInformationElement::ESmsReplyAddressFormat, KReplyAddress));
    TEST(err == KErrAlreadyExists);
    TInt index = 0;
	TBool rc = userData.InformationElementIndex(CSmsInformationElement::ESmsReplyAddressFormat,index);
	TEST(rc != EFalse);
	userData.RemoveInformationElement(index);

    // Add a single PDU information element
    _LIT8(KEnhancedVoiceMail,"\x01\x02\x03\x04\x05\x06\x07\x08");
    CSmsInformationElement* iE1 = CSmsInformationElement::NewL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation, KEnhancedVoiceMail);
    CleanupStack::PushL(iE1);
    TRAP(err,additionalAttributes->AddControlInformationElementL(iE1));
    TEST(err == KErrNone);
    CleanupStack::Pop(iE1);

    // Increase the branch coverage by exercising the method UpdateInformationElement in ways the the client cannot do directly.
    // add single pdu information element.
    TRAP(err,userData.UpdateInformationElementArrayL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation, KEnhancedVoiceMail));
    TEST(err == KErrNone);
    TRAP(err,userData.UpdateInformationElementArrayL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation, KEnhancedVoiceMail));
    TEST(err == KErrAlreadyExists);
    index = 0;
	rc = userData.InformationElementIndex(CSmsInformationElement::ESmsEnhanceVoiceMailInformation,index);
	TEST(rc != EFalse);
	userData.RemoveInformationElement(index);

    // Increase the branch coverage by exercising the method UpdateInformationElement in ways the the client cannot do directly.
    // add email header
    _LIT8(KEmailHeader,"\x01\x02\x03\x04\x05\x06\x07\x08");
    TRAP(err,userData.UpdateInformationElementArrayL(CSmsInformationElement::ESmsIEIRFC822EmailHeader, KEmailHeader));
    TEST(err == KErrNone);
    TRAP(err,userData.UpdateInformationElementArrayL(CSmsInformationElement::ESmsIEIRFC822EmailHeader, KEmailHeader));
    TEST(err == KErrAlreadyExists);
    index = 0;
	rc = userData.InformationElementIndex(CSmsInformationElement::ESmsIEIRFC822EmailHeader,index);
	TEST(rc != EFalse);
	userData.RemoveInformationElement(index);

    // Increase the branch coverage by exercising the method UpdateInformationElement in ways the the client cannot do directly.
    // add an information element which will be present in every PDU.
    _LIT8(KESmsIEISIMToolkitSecurityHeaders1,"\x01\x02\x03\x04\x05\x06\x07\x08");
    TRAP(err,userData.UpdateInformationElementArrayL(CSmsInformationElement::ESmsIEISIMToolkitSecurityHeaders1, KESmsIEISIMToolkitSecurityHeaders1));
    TEST(err == KErrNone);
    TRAP(err,userData.UpdateInformationElementArrayL(CSmsInformationElement::ESmsIEISIMToolkitSecurityHeaders1, KESmsIEISIMToolkitSecurityHeaders1));
    TEST(err == KErrAlreadyExists);
    index = 0;
	rc = userData.InformationElementIndex(CSmsInformationElement::ESmsIEISIMToolkitSecurityHeaders1,index);
	TEST(rc != EFalse);
	userData.RemoveInformationElement(index);

    // Test adding 17 Multi PDU Hyper Links

    for (TUint i = 0; i < 17; i++)
    {
    _LIT8(KBinaryTestData,"\x00\x00\x0F\x0E");
    CSmsInformationElement* iE = CSmsInformationElement::NewL(CSmsInformationElement::ESmsHyperLinkFormat, KBinaryTestData);
    CleanupStack::PushL(iE);
    CSmsMessageAdditionalAttributes* additionalAttributes1 = (CSmsMessageAdditionalAttributes*) message->AdditionalInfo();
    TRAP(err,additionalAttributes1->AddControlInformationElementL(iE));
    TEST(err == KErrNone);
    CleanupStack::Pop(iE);
    }

    CArrayFixFlat<TGsmSms>* smsArray=new(ELeave) CArrayFixFlat<TGsmSms>(8);

    TRAP(err,rc = message->EncodeIntoSinglePDUL(*smsArray));
    TEST(err == KErrNone);
    TEST(rc  != EFalse);

    CleanupStack::Pop(message);
    delete message;

    // now decode the message and check the information elements returned.
	CSmsBuffer* receiveBuffer = CSmsBuffer::NewL();
	TGsmSms& sms = smsArray->operator[](0);

	CSmsMessage* decodedMessage = CSmsMessage::NewL(fs, sms, receiveBuffer, EFalse, EFalse);
    CleanupStack::PushL(decodedMessage);

    CSmsMessageAdditionalAttributes* additionalDecodedAttributes = (CSmsMessageAdditionalAttributes*) decodedMessage->AdditionalInfo();

	TUint numberOfElementsMandatoryIn1stPDU = additionalDecodedAttributes->NumberOfControlInformationElements(TSmsInformationElementCategories::ECtrlMandatoryIn1stPDUOnly);
    TEST(numberOfElementsMandatoryIn1stPDU==1);

	TUint numberOfSingleInstanceElement     = additionalDecodedAttributes->NumberOfControlInformationElements(TSmsInformationElementCategories::ECtrlSingleInstanceOnly);
    TEST(numberOfSingleInstanceElement==1);

	TUint numberOfMultipleInstanceElements  = additionalDecodedAttributes->NumberOfControlInformationElements(TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed);
    TEST(numberOfMultipleInstanceElements==17);

	CSmsPDU& decodedSubmitPDU = decodedMessage->SmsPDU();
    CSmsUserData& decodedUserData = decodedSubmitPDU.UserData();

	CArrayFixFlat<TInt>* indices = new(ELeave) CArrayFixFlat<TInt>(8);

	TRAP(err, decodedUserData.InformationElementIndicesL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, *indices));
	TEST(err == KErrNone);
	TESTL(indices->Count() == 4);

    for (TUint8 i = 0; i < 4; i++)
        {
	    CSmsInformationElement*& specialSMSI1 = decodedUserData.InformationElementPtr(indices->operator[](i));
        }

    delete indices;

	CleanupStack::Pop(decodedMessage);
    delete decodedMessage;

    delete smsArray;

	__UHEAP_MARKEND;
	return TestStepResult();
    }


TVerdict CTestIECategoryDefinitions::doTestStepL()
    {
    /**
     *  Synopsis:
     *  
     *  Test that each type of information element can be mapped to its correct category
     *  using the utility mehtod TSmsInformationElementCategories::GetCategoryDefinition();
     *  
     */
    TSmsInformationElementCategories::TInformationElementCategory category;

    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference, category));
    TEST(category==TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUButWithValueSpecificToPDU );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsIEIConcatenatedShortMessages16BitReference, category));
    TEST(category==TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUButWithValueSpecificToPDU );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsIEISMSCControlParameters, category));
    TEST(category==TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUButWithValueSpecificToPDU );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsIEIRFC822EmailHeader, category));
    TEST(category==TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUButWithValueSpecificToPDU );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, category));
    TEST(category== TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUMultipleInstancesPerPDU );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsHyperLinkFormat, category));
    TEST(category==TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsReplyAddressFormat, category));
    TEST(category==TSmsInformationElementCategories::ECtrlMandatoryIn1stPDUOnly );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedPredefinedSound, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedUserDefinedSound, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedPredefinedAnimation, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedLargeAnimation, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedSmallAnimation, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedVariablePicture, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedUserPromptIndicator, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedExtendedObject, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedReusedExtendedObject, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedCompressionControl, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedODI, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedStandardWVG, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedCharacterSizeWVG, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsEnhancedextendedObjectDataRequest, category));
    TEST(category==TSmsInformationElementCategories::EEmsInformationElement );

    //
    // Some invalid categories...
    //
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::ESmsIEIReserved, category) == EFalse);
    TEST(TSmsInformationElementCategories::GetCategoryDefinition(CSmsInformationElement::TSmsInformationElementIdentifier(9999), category) == EFalse);

    return TestStepResult();
    }


TVerdict CTestR6DCS::doTestStepL()
    /**
     *  
     *  Test the Data coding scheme bytes can be correctly configured
     *  when R6 automatic deletion feature is active.
     */
    {
	TSmsDataCodingScheme dataCodingScheme;
	dataCodingScheme.SetBits7To4((TSmsDataCodingScheme::TSmsDCSBits7To4)0x40);

    // Turn on compression
    TEST(dataCodingScheme.TextCompressed() == EFalse);
    dataCodingScheme.SetTextCompressed(ETrue);
	TEST(dataCodingScheme.TextCompressed() != EFalse);

    //  Test switching from 7 bit to 8 bit encoding.
    TEST(dataCodingScheme.Alphabet() == TSmsDataCodingScheme::ESmsAlphabet7Bit);
    dataCodingScheme.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
    TEST(dataCodingScheme.Alphabet() == TSmsDataCodingScheme::ESmsAlphabet8Bit);

    // Test switching on and off class
	dataCodingScheme.SetClass(ETrue,(TSmsDataCodingScheme::TSmsClass) 1);
    TSmsDataCodingScheme::TSmsClass testClass;
	dataCodingScheme.Class(testClass);
	TEST(testClass == (TSmsDataCodingScheme::TSmsClass) 1);
	dataCodingScheme.SetClass(ETrue,(TSmsDataCodingScheme::TSmsClass) 2);
	dataCodingScheme.Class(testClass);
	TEST(testClass == (TSmsDataCodingScheme::TSmsClass) 2);

    // Turn off compression
    dataCodingScheme.SetTextCompressed(EFalse);
	TEST(dataCodingScheme.TextCompressed() == EFalse);

    // Switch from 8 bit to 7 bit encoding
    //  Test switching from 7 bit to 8 bit encoding.
    TEST(dataCodingScheme.Alphabet() == TSmsDataCodingScheme::ESmsAlphabet8Bit);
    dataCodingScheme.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
    TEST(dataCodingScheme.Alphabet() == TSmsDataCodingScheme::ESmsAlphabet7Bit);

    // Test data coding scheme class
    dataCodingScheme.Class(testClass);
	TEST(testClass == (TSmsDataCodingScheme::TSmsClass) 2);
	dataCodingScheme.Class(testClass);
	TEST(testClass == (TSmsDataCodingScheme::TSmsClass) 2);

	return TestStepResult();
	}
