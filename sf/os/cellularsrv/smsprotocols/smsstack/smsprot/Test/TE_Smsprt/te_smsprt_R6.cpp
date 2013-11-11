// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "te_smsprt_R6.h"

#include <gsmunonieoperations.h>
#include <gsmubuf.h>
#include <gsmuset.h>
#include <gsmuieoperations.h>

#include "smsstacktestconsts.h"
#include "smsstacktestutilities.h"

TVerdict CTestSinglePDUHyperLinks::doTestStepL()
/**
 *  Test a simple Transmit and Receive of a TPDU
 */
	{
	INFO_PRINTF1(_L("Test HyperLinks in SMS message"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	//This is a 95 character msg.
	_LIT(KTestMsg,"HyperLink message, Symbian. http://www.symbian.com Some content Intranet http://web.intra/homes");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//8 bit coding scheme
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S "),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S "),&iServiceCenterNumber);

	//Get the operations
	CSmsHyperLinkOperations& operations = static_cast<CSmsHyperLinkOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsHyperLinkFormat));

	TUint count=operations.NumberOfHyperLinksL();
	TEST(count==0);

	const TUint KFirstHyperLinkPosition=20;
	const TUint KSecondHyperLinkPosition=65;
	TUint8 firstHyperLinkLabelLength=8;
	TUint8 firstHyperLinkURLLength=22;

	//Try to remove index 0 before configuration
	TUint index=0;
	TRAPD(err,operations.RemoveHyperLinkL(index));
	TEST(err==KErrArgument);
	count=operations.NumberOfHyperLinksL();
	TEST(count==0);

	//Try copy with no hyper links installed
	TUint hyperLinkPosition=0;
	TUint8 hyperLinkLableLength=0;
	TUint8 hyperLinkURLLength=0;
	TRAP(err,operations.CopyHyperLinkAtIndexL(index,hyperLinkPosition,hyperLinkLableLength,hyperLinkURLLength));
	TEST(err==KErrArgument);

	//Add hyperlinks
	operations.AddHyperLinkL(KFirstHyperLinkPosition,firstHyperLinkLabelLength, firstHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==1);
	//Add second hyperlink
	TUint8 secondHyperLinkLabelLength=10;
	TUint8 secondHyperLinkURLLength=25;
	operations.AddHyperLinkL(KSecondHyperLinkPosition,secondHyperLinkLabelLength,secondHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==2);
	//Add hyperlink with position greater than message size
	const TUint KPositionLargerThanMessage=2000;
	TUint8 thirdHyperLinkLabelLength=15;
	TUint8 thirdHyperLinkURLLength=33;
	operations.AddHyperLinkL(KPositionLargerThanMessage,thirdHyperLinkLabelLength,thirdHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==3);

	//Get hyperlinks
	hyperLinkPosition=0;
	hyperLinkLableLength=0;
	hyperLinkURLLength=0;
	//Copy HyperLinks
	index=0;
	operations.CopyHyperLinkAtIndexL(index,hyperLinkPosition,hyperLinkLableLength,hyperLinkURLLength);
	TEST(hyperLinkPosition==KFirstHyperLinkPosition);
	TEST(hyperLinkLableLength==firstHyperLinkLabelLength);
	TEST(hyperLinkURLLength==firstHyperLinkURLLength);

	//Copy at the boundares
	//Index is on the boundary 2
	hyperLinkPosition=0;
	hyperLinkLableLength=0;
	hyperLinkURLLength=0;
	index=2;
	operations.CopyHyperLinkAtIndexL(index,hyperLinkPosition,hyperLinkLableLength,hyperLinkURLLength);
	TEST(hyperLinkPosition==KPositionLargerThanMessage);
	TEST(hyperLinkLableLength==thirdHyperLinkLabelLength);
	TEST(hyperLinkURLLength==thirdHyperLinkURLLength);
	//index is just out of bounds
	hyperLinkPosition=0;
	hyperLinkLableLength=0;
	hyperLinkURLLength=0;
	index=3;
	TRAP(err,operations.CopyHyperLinkAtIndexL(index,hyperLinkPosition,hyperLinkLableLength,hyperLinkURLLength));
	TEST(err==KErrArgument);
	//Try invalid boundary
	index=10;
	TRAP(err,operations.CopyHyperLinkAtIndexL(index,hyperLinkPosition,hyperLinkLableLength,hyperLinkURLLength));
	TEST(err==KErrArgument);

	//Remove HyperLinks
	index=0;
	operations.RemoveHyperLinkL(index);
	count=operations.NumberOfHyperLinksL();
	TEST(count==2);

	index=4;
	TRAP(err,operations.RemoveHyperLinkL(index));
	TEST(err==KErrArgument);

	operations.RemoveAllHyperLinksL();
	count=operations.NumberOfHyperLinksL();
	TEST(count==0);

	//Try remove all again - on no hyperlinks
	operations.RemoveAllHyperLinksL();
	count=operations.NumberOfHyperLinksL();
	TEST(count==0);

	//Send message with no hyperlinks
	SendSmsL(smsMessage,socket);

	//Add hyperlink
	operations.AddHyperLinkL(KFirstHyperLinkPosition,firstHyperLinkLabelLength,firstHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==1);

	//Send SMS with one hyperlink
	SendSmsL(smsMessage,socket);
	//Send SMS with two hyperlinks
	//Add hyperlink
	operations.AddHyperLinkL(KSecondHyperLinkPosition,secondHyperLinkLabelLength,secondHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==2);
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);

	//---------------------------------
	//Receive SMS's
	// 1 - Receive PDU with a single hyperlink
	// 2 - Receive PDU with two hyperlinks
	// 3 - Receive PDU with no hyperlinks
	//---------------------------------
	//Single Hyperlink SMS
	INFO_PRINTF1(_L("waiting for incoming SMS with single hyperlink...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsHyperLinkOperations& deliverPDUOperations = static_cast<CSmsHyperLinkOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsHyperLinkFormat));
	//NumberOfHyperLinks
	count=deliverPDUOperations.NumberOfHyperLinksL();
	TEST(count==1);
	//Copy HyperLinks
	index=0;
	deliverPDUOperations.CopyHyperLinkAtIndexL(index,hyperLinkPosition,hyperLinkLableLength,hyperLinkURLLength);
	TEST(hyperLinkPosition==KFirstHyperLinkPosition);
	TEST(hyperLinkLableLength==firstHyperLinkLabelLength);
	TEST(hyperLinkURLLength==firstHyperLinkURLLength);
	CleanupStack::PopAndDestroy(smsMessage);

	//Multiple Hyperlink SMS
	INFO_PRINTF1(_L("waiting for incoming SMS with two hyperlinks...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	TestSmsContentsL(smsMessage,KTestMsg);
	//Get the operations
	CSmsHyperLinkOperations& multipleHyperLinkOperations = static_cast<CSmsHyperLinkOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsHyperLinkFormat));
	//NumberOfHyperLinks
	count=multipleHyperLinkOperations.NumberOfHyperLinksL();
	TEST(count==2);
	//Copy HyperLinks
	index=0;
	multipleHyperLinkOperations.CopyHyperLinkAtIndexL(index,hyperLinkPosition,hyperLinkLableLength,hyperLinkURLLength);
	TEST(hyperLinkPosition==KFirstHyperLinkPosition);
	TEST(hyperLinkLableLength==firstHyperLinkLabelLength);
	TEST(hyperLinkURLLength==firstHyperLinkURLLength);
	index=1;
	multipleHyperLinkOperations.CopyHyperLinkAtIndexL(index,hyperLinkPosition,hyperLinkLableLength,hyperLinkURLLength);
	TEST(hyperLinkPosition==KSecondHyperLinkPosition);
	TEST(hyperLinkLableLength==secondHyperLinkLabelLength);
	TEST(hyperLinkURLLength==secondHyperLinkURLLength);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive third sms with no hyperlinks
	INFO_PRINTF1(_L("waiting for incoming SMS with no hyperlinks...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);
	//Get the operations
	CSmsHyperLinkOperations& noneHyperLinkOperations = static_cast<CSmsHyperLinkOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsHyperLinkFormat));
	//NumberOfHyperLinks
	count=noneHyperLinkOperations.NumberOfHyperLinksL();
	TEST(count==0);
	CleanupStack::PopAndDestroy(smsMessage);

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
}

TVerdict CTestMultiplePDUHyperLinks::doTestStepL()
/**
 *  Test a simple Transmit and Receive of a TPDU
 */
	{
	INFO_PRINTF1(_L("Test multiple PDUs with hyperLinks in SMS message"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KLongText,"YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBCCCCCCC");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KLongText,alphabet);

	//Check number of hyperlinks
	//Get the operations
	CSmsHyperLinkOperations& operations =
		static_cast<CSmsHyperLinkOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsHyperLinkFormat));

	//NumberOfHyperLinks
	TUint count=operations.NumberOfHyperLinksL();
	TEST(count==0);

	//Try remove all
	operations.RemoveAllHyperLinksL();
	count=operations.NumberOfHyperLinksL();
	TEST(count==0);

	//Send multiple PDU with no hyperlinks
	SendSmsL(smsMessage,socket);

	//Send multiple PDU with multiple hyperlinks.  Hyperlinks are all in the first PDU.
	//hyperlinks with positions referencing two in the 1st PDU, three in 2nd, and four in 3rd
	TUint firstHyperLinkPosition=25;
	TUint secondHyperLinkPosition=75;
	TUint thirdHyperLinkPosition=200;
	TUint fourthHyperLinkPosition=235;
	TUint fifthHyperLinkPosition=265;
	TUint sixthHyperLinkPosition=450;
	TUint seventhHyperLinkPosition=485;
	TUint eighthHyperLinkPosition=510;
	TUint ninethHyperLinkPosition=535;

	TUint8 KHyperLinkLabelLength=8;
	TUint8 KHyperLinkURLLength=25;

	//1st PDU
	operations.AddHyperLinkL(firstHyperLinkPosition,KHyperLinkLabelLength,KHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==1);
	operations.AddHyperLinkL(secondHyperLinkPosition,KHyperLinkLabelLength,KHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==2);

	//2nd PDU
	operations.AddHyperLinkL(thirdHyperLinkPosition,KHyperLinkLabelLength,KHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==3);
	operations.AddHyperLinkL(fourthHyperLinkPosition,KHyperLinkLabelLength,KHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==4);
	operations.AddHyperLinkL(fifthHyperLinkPosition,KHyperLinkLabelLength,KHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==5);

	//3rd PDU
	operations.AddHyperLinkL(sixthHyperLinkPosition,KHyperLinkLabelLength,KHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==6);
	operations.AddHyperLinkL(seventhHyperLinkPosition,KHyperLinkLabelLength,KHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==7);
	operations.AddHyperLinkL(eighthHyperLinkPosition,KHyperLinkLabelLength,KHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==8);
	operations.AddHyperLinkL(ninethHyperLinkPosition,KHyperLinkLabelLength,KHyperLinkURLLength);
	count=operations.NumberOfHyperLinksL();
	TEST(count==9);

	(void) smsMessage->TextPresent();
	(void) smsMessage->NumMessagePDUsL();
	(void) smsMessage->MaxMessageLength();
	(void) smsMessage->MessageLengthL();

	//Send multiple PDU with nine hyperlinks
	SendSmsL(smsMessage,socket);

	CleanupStack::PopAndDestroy(smsMessage);

	//Reveive multiple PDU with no hyperlinks
	INFO_PRINTF1(_L("waiting for incoming SMS with no hyperlinks...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	//Get the operations
	CSmsHyperLinkOperations& deliveredPDUOperations =
		static_cast<CSmsHyperLinkOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsHyperLinkFormat));

	//NumberOfHyperLinks
	count=deliveredPDUOperations.NumberOfHyperLinksL();
	TEST(count==0);

	CleanupStack::PopAndDestroy(smsMessage);

	//Reveive multiple PDU with multiple (9) hyperlinks all in the first PDU
	INFO_PRINTF1(_L("waiting for incoming SMS with 9 hyperlinks...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	//Get the operations
	CSmsHyperLinkOperations& multipleHyperLinkPDUOperations =
		static_cast<CSmsHyperLinkOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsHyperLinkFormat));

	//NumberOfHyperLinks
	count=multipleHyperLinkPDUOperations.NumberOfHyperLinksL();
	TEST(count==9);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive multiple PDU with hyperlinks in all PDU's
	INFO_PRINTF1(_L("waiting for incoming SMS with hyperlinks in all PDU...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	//Get the operations
	CSmsHyperLinkOperations& hyperlinkInEveryPDUOperations =
		static_cast<CSmsHyperLinkOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsHyperLinkFormat));

	//NumberOfHyperLinks
	count=hyperlinkInEveryPDUOperations.NumberOfHyperLinksL();
	TEST(count==6);

	//Copy the hyperlinks
	//Get hyperlinks
	TUint hyperLinkPosition=0;
	TUint8 hyperLinkLableLength=0;
	TUint8 hyperLinkURLLength=0;
	TUint expectedHyperLinkPosition=0;
	TUint8 expectedHyperLinkLableLength=0;
	TUint8 expectedHyperLinkURLLength=0;

	//Copy HyperLinks
	//Fist hyperlink
	TUint index=0;
	expectedHyperLinkPosition=25;
	expectedHyperLinkLableLength=8;
	expectedHyperLinkURLLength=25;
	hyperlinkInEveryPDUOperations.CopyHyperLinkAtIndexL(index,hyperLinkPosition,hyperLinkLableLength,hyperLinkURLLength);
	TEST(hyperLinkPosition==expectedHyperLinkPosition);
	TEST(hyperLinkLableLength==expectedHyperLinkLableLength);
	TEST(hyperLinkURLLength==expectedHyperLinkURLLength);

	//first hyper link in second PDU
	index=1;
	expectedHyperLinkPosition=265;
	expectedHyperLinkLableLength=10;
	expectedHyperLinkURLLength=30;
	hyperlinkInEveryPDUOperations.CopyHyperLinkAtIndexL(index,hyperLinkPosition,hyperLinkLableLength,hyperLinkURLLength);
	TEST(hyperLinkPosition==expectedHyperLinkPosition);
	TEST(hyperLinkLableLength==expectedHyperLinkLableLength);
	TEST(hyperLinkURLLength==expectedHyperLinkURLLength);

	//Last hyperlink in thrid PDU
	index=5;
	expectedHyperLinkPosition=535;
	expectedHyperLinkLableLength=15;
	expectedHyperLinkURLLength=45;
	hyperlinkInEveryPDUOperations.CopyHyperLinkAtIndexL(index,hyperLinkPosition,hyperLinkLableLength,hyperLinkURLLength);
	TEST(hyperLinkPosition==expectedHyperLinkPosition);
	TEST(hyperLinkLableLength==expectedHyperLinkLableLength);
	TEST(hyperLinkURLLength==expectedHyperLinkURLLength);
	CleanupStack::PopAndDestroy(smsMessage);


	//receive mulitple PDU message with no hyperlinks in the first PDU and hyperlinks in the third.
	INFO_PRINTF1(_L("waiting for incoming SMS with no hyperlinks in first PDU...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	//Get the operations
	CSmsHyperLinkOperations& noHyperlinksInFirstPDUOperations =
		static_cast<CSmsHyperLinkOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsHyperLinkFormat));

	//NumberOfHyperLinks
	count=noHyperlinksInFirstPDUOperations.NumberOfHyperLinksL();
	TEST(count==9);

	CleanupStack::PopAndDestroy(smsMessage);
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult() ;
}


TVerdict CTestSinglePDUReplyAddress::doTestStepL()
/**
 *  Test reply address can be different to the senders address
 */
	{
	INFO_PRINTF1(_L("Test reply address in SMS message"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	//This is character msg
	_LIT(KTestMsg,"Reply address message.  Message will be sent to other address than sender");
	_LIT(KEmptyMsg,"");
	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//8 bit coding scheme
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* specialMessageAndReplyAddresMessage=CreateSmsMessageLC(KEmptyMsg,alphabet);

	//Get the operations
	CSmsReplyAddressOperations& replyAddressOperations =
		static_cast<CSmsReplyAddressOperations&>(specialMessageAndReplyAddresMessage->GetOperationsForIEL(CSmsInformationElement::ESmsReplyAddressFormat));

	//Check for reply address
	TBool result=replyAddressOperations.ContainsReplyAddressIEL();
	TEST(!result);

	//Remove - should not leave
	TRAPD(err,replyAddressOperations.RemoveReplyAddressL());
	TEST(err==KErrNone);

	//Try to get non existent address
	// as HBufC
	HBufC* replyAddressHBuf = NULL;
	TRAP(err,replyAddressHBuf=replyAddressOperations.GetReplyAddressL());
	TEST(err==KErrNotFound);

	// as TGsmSmsTelNumber
	TGsmSmsTelNumber replyAddressTGsm;
	TRAP(err,result=replyAddressOperations.GetParsedReplyAddressL(replyAddressTGsm));
	TEST(err==KErrNone);
	TEST(!result);


	//send a message with lots of information elements (e.g. special message waiting ) with a reply address.
	//  The reply address will not fit into the 1st PDU.  Sending should fail.

	//Get hyperlink operations
	CSmsSpecialSMSMessageOperations& specialMessageOperations =
		static_cast<CSmsSpecialSMSMessageOperations&>(specialMessageAndReplyAddresMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));

	//PDU maximym of 140 octects = C8
	//Each special message is 4 octects.
	//Maximum variations on the type of messages for 5 bits is 32
	for(TInt indicationType=EGsmSmsVoiceMessageWaiting; indicationType<=EGsmSmsExtendedMessageTypeWaiting;
				++indicationType)
	{
		TBool toStore=EFalse;
		TSmsMessageProfileType messageProfileType=EGsmSmsProfileId1;
		TUint8 messageCount=5;

		for(TUint extendedType=EGsmSmsNoExtendedMessageTypeIndication;
					extendedType<=EGsmSmsExtendedIndicationType7; ++extendedType)
		{
			TRAP(err,specialMessageOperations.AddSpecialMessageIndicationL(toStore,TSmsMessageIndicationType(indicationType),
									TExtendedSmsIndicationType(extendedType),messageProfileType,messageCount));
			TEST(err==KErrNone);
		}
	}

	//Add six zero length information elements
	//Get the PDU
	CSmsPDU &pdu=specialMessageAndReplyAddresMessage->SmsPDU();
	//Get the UserData
	CSmsUserData &userData=pdu.UserData();
	
	//Add the information element
	//TSmsId
	for(TUint informationElement=CSmsInformationElement::ESmsIEISIMToolkitSecurityHeaders1;
		informationElement<=CSmsInformationElement::ESmsIEISIMToolkitSecurityHeaders5;
		++informationElement)
	    {
		TRAP(err,userData.AddInformationElementL(
			CSmsInformationElement::TSmsInformationElementIdentifier(informationElement),KNullDesC8()));
		TEST(err==KErrNone);
	    }

	//Add address
	TPtrC replyAddress;
	GetStringFromConfig(ConfigSection(), _L("replyAddressInternational"), replyAddress);
	TRAP(err,replyAddressOperations.AddReplyAddressL(replyAddress));
	TEST(err==KErrNone);
	//Check for reply address - should not contain
	result=replyAddressOperations.ContainsReplyAddressIEL();
	TEST(result);

	(void) specialMessageAndReplyAddresMessage->TextPresent();
	TRAP(err,(void) specialMessageAndReplyAddresMessage->NumMessagePDUsL());
	TEST(err==KErrArgument);
	(void) specialMessageAndReplyAddresMessage->MaxMessageLength();
	(void) specialMessageAndReplyAddresMessage->MessageLengthL();

	//Send the message
	SendSmsErrorL(specialMessageAndReplyAddresMessage,socket);

	//Remove addresses
	TRAP(err,replyAddressOperations.RemoveReplyAddressL());
	TEST(err==KErrNone);

	//Check boundary value lengths for reply addreses.
	//	The IE for reply address has it's own length specified as IEL.
	//Maximum length for this is: KSmsAddressMaxAddressLength = 12.  That is 10 plus 2 for the type
	//Gives us a maximum of 10 octects.  That allows us a maximum of 20 digits.
	//Boundary values would therefore be 19, 20, and 21 digit numbers
	TPtrC minReplyAddress;
	GetStringFromConfig(ConfigSection(), _L("nineteenDigitReplyAddress"), minReplyAddress);
	TRAP(err,replyAddressOperations.AddReplyAddressL(minReplyAddress));
	TEST(err==KErrNone);
	result=replyAddressOperations.ContainsReplyAddressIEL();
	TEST(result);
	//Get the reply address
	HBufC* returnedHBufReplyAddress=NULL;
	TRAP(err,returnedHBufReplyAddress=replyAddressOperations.GetReplyAddressL());
	TEST(err==KErrNone);
	CleanupStack::PushL(returnedHBufReplyAddress);
	TPtr returnedBuffPtr(returnedHBufReplyAddress->Des());
	TEST(returnedBuffPtr==minReplyAddress);
	CleanupStack::PopAndDestroy(returnedHBufReplyAddress);

	//Remove addresses
	TRAP(err,replyAddressOperations.RemoveReplyAddressL());
	TEST(err==KErrNone);

	//On the boundary
	TPtrC boundaryReplyAddress;
	GetStringFromConfig(ConfigSection(), _L("twentyDigitReplyAddress"), boundaryReplyAddress);
	TRAP(err,replyAddressOperations.AddReplyAddressL(boundaryReplyAddress));
	TEST(err==KErrNone);
	result=replyAddressOperations.ContainsReplyAddressIEL();
	TEST(result);
	TRAP(err,returnedHBufReplyAddress=replyAddressOperations.GetReplyAddressL());
	TEST(err==KErrNone);
	CleanupStack::PushL(returnedHBufReplyAddress);
	TPtr boundaryBuffPtr=returnedHBufReplyAddress->Des();
	TEST(boundaryBuffPtr==boundaryReplyAddress);
	CleanupStack::PopAndDestroy(returnedHBufReplyAddress);

	//Remove addresses
	TRAP(err,replyAddressOperations.RemoveReplyAddressL());
	TEST(err==KErrNone);

	//Just outside the boundary (21)
	TPtrC overBoundaryReplyAddress;
	GetStringFromConfig(ConfigSection(), _L("twentyOneDigitReplyAddress"), overBoundaryReplyAddress);
	TRAP(err,replyAddressOperations.AddReplyAddressL(overBoundaryReplyAddress));
	TEST(err==KErrArgument);
	result=replyAddressOperations.ContainsReplyAddressIEL();
	TEST(!result);
	TRAP(err,returnedHBufReplyAddress=replyAddressOperations.GetReplyAddressL());
	TEST(err==KErrNotFound);

	//Remove addresses
	TRAP(err,replyAddressOperations.RemoveReplyAddressL());
	TEST(err==KErrNone);

	//Check boundary value lengths for parsed reply addresses.
	//	The IE for reply address has it's own length specified as IEL.
	//Maximum length for this is: KSmsAddressMaxAddressLength = 12.  That is 10 plus 2 for the type
	//Gives us a maximum of 10 octects.  That allows us a maximum of 10 characters @ 7 bit coding as per spec.
	//Boundary values would therefore be 10, 11, and 12 character numbers
	TPtrC smallBoundaryParsedReplyAddress;
	GetStringFromConfig(ConfigSection(), _L("tenCharacterParsedReplyAddress"), smallBoundaryParsedReplyAddress);
	replyAddressTGsm.iTelNumber=smallBoundaryParsedReplyAddress;
	replyAddressTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
	TRAP(err,replyAddressOperations.AddParsedReplyAddressL(replyAddressTGsm));
	TEST(err==KErrNone);
	result=replyAddressOperations.ContainsReplyAddressIEL();
	TEST(result);
	TGsmSmsTelNumber retrievedParsedReplyAddress;
	TRAP(err,result=replyAddressOperations.GetParsedReplyAddressL(retrievedParsedReplyAddress));
	TEST(err==KErrNone);
	TEST(result);
	TEST(replyAddressTGsm.iTelNumber==retrievedParsedReplyAddress.iTelNumber);

	//Remove address
	TRAP(err,replyAddressOperations.RemoveReplyAddressL());
	TEST(err==KErrNone);

	//Check length 11
	TPtrC onBoundaryParsedReplyAddress;
	GetStringFromConfig(ConfigSection(), _L("elevenCharacterParsedReplyAddress"), onBoundaryParsedReplyAddress);
	replyAddressTGsm.iTelNumber=onBoundaryParsedReplyAddress;
	replyAddressTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
	TRAP(err,replyAddressOperations.AddParsedReplyAddressL(replyAddressTGsm));
	TEST(err==KErrNone);
	result=replyAddressOperations.ContainsReplyAddressIEL();
	TEST(result);

	//Remove address
	TRAP(err,replyAddressOperations.RemoveReplyAddressL());
	TEST(err==KErrNone);

	//Check length 12
	TPtrC overBoundaryParsedReplyAddress;
	GetStringFromConfig(ConfigSection(), _L("twelveCharacterParsedReplyAddress"), overBoundaryParsedReplyAddress);
	replyAddressTGsm.iTelNumber=overBoundaryParsedReplyAddress;
	replyAddressTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
	TRAP(err,replyAddressOperations.AddParsedReplyAddressL(replyAddressTGsm));
	TEST(err==KErrOverflow);
	result=replyAddressOperations.ContainsReplyAddressIEL();
	TEST(!result);

	CleanupStack::PopAndDestroy(specialMessageAndReplyAddresMessage);

	//Create a new message
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	//Get the operations
	CSmsReplyAddressOperations& operations =
		static_cast<CSmsReplyAddressOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsReplyAddressFormat));

	//Add international number
	TRAP(err,operations.AddReplyAddressL(replyAddress));
	TEST(err==KErrNone);

	//Read value as a parsed address
	TRAP(err,result=operations.GetParsedReplyAddressL(retrievedParsedReplyAddress));
	TEST(err==KErrNone);
	TEST(result);
	_LIT(KInternationalPrefix, "+");
	const TUint KMaxNumberSize=15;
	TBuf<KMaxNumberSize> internationalNumber(KInternationalPrefix);
	internationalNumber.Append( retrievedParsedReplyAddress.iTelNumber);
	TEST(replyAddress==internationalNumber);

	//Remove address
	TRAP(err,operations.RemoveReplyAddressL());
	TEST(err==KErrNone);

	//Add address as a parsed address
	GetStringFromConfig(ConfigSection(), _L("elevenCharacterParsedReplyAddress"), onBoundaryParsedReplyAddress);
	replyAddressTGsm.iTelNumber=onBoundaryParsedReplyAddress;
	replyAddressTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
	TRAP(err,operations.AddParsedReplyAddressL(replyAddressTGsm));
	TEST(err==KErrNone);

	//Read address as an international number
	TRAP(err,returnedHBufReplyAddress=operations.GetReplyAddressL());
	TEST(err==KErrNone);
	CleanupStack::PushL(returnedHBufReplyAddress);
	TPtr parsedAddressPtr=returnedHBufReplyAddress->Des();
	TEST(parsedAddressPtr==onBoundaryParsedReplyAddress);
	CleanupStack::PopAndDestroy(returnedHBufReplyAddress);

	//Remove address
	TRAP(err,operations.RemoveReplyAddressL());
	TEST(err==KErrNone);

	//Add address
	TRAP(err,operations.AddReplyAddressL(replyAddress));
	TEST(err==KErrNone);
	//Check for reply address - should contain
	result=operations.ContainsReplyAddressIEL();
	TEST(result);

	//Add parsed address
	replyAddressTGsm.iTelNumber=_L("+447583927594");
	replyAddressTGsm.iTypeOfAddress=EGsmSmsTONInternationalNumber;
	TRAP(err,operations.AddParsedReplyAddressL(replyAddressTGsm));
	TEST(err==KErrAlreadyExists);
	//Check for reply address - should contain
	result=operations.ContainsReplyAddressIEL();
	TEST(result);

	//Remove address
	TRAP(err,operations.RemoveReplyAddressL());
	TEST(err==KErrNone);
	//Check for reply address - should not contain
	result=operations.ContainsReplyAddressIEL();
	TEST(!result);

	//Add Parsed reply address
	TRAP(err,operations.AddParsedReplyAddressL(replyAddressTGsm));
	TEST(err==KErrNone);
	//Check for reply address - should not contain
	result=operations.ContainsReplyAddressIEL();
	TEST(result);

	//Remove parsed address
	TRAP(err,operations.RemoveReplyAddressL());
	TEST(err==KErrNone);
	//Check for reply address - should not contain
	result=operations.ContainsReplyAddressIEL();
	TEST(!result);

	//Send message with no reply address
	SendSmsL(smsMessage,socket);
	// -------- END Sending no reply

	//Add reply address
	TRAP(err,operations.AddReplyAddressL(replyAddress));
	TEST(err==KErrNone);

	//Check for reply address
	result=operations.ContainsReplyAddressIEL();
	TEST(result);

	//Get the reply address
	// as HBufC
	TRAP(err,replyAddressHBuf=operations.GetReplyAddressL());
	CleanupStack::PushL(replyAddressHBuf);
	TEST(err==KErrNone);
	TPtr buffPtr(replyAddressHBuf->Des());
	TEST(buffPtr==replyAddress);

	//Send message with one reply address
	CleanupStack::PopAndDestroy(replyAddressHBuf);
	SendSmsL(smsMessage,socket);
	// ------- END Send with Reply address

	//Send message with one parsed reply address
	//Remove address
	TRAP(err,operations.RemoveReplyAddressL());
	TEST(err==KErrNone);
	//Check for reply address - should not contain
	result=operations.ContainsReplyAddressIEL();
	TEST(!result);

	//Add parsed address
	TGsmSmsTelNumber parsedReplyAddressTGsm;
	parsedReplyAddressTGsm.iTelNumber=_L("44758ABCD3");
	parsedReplyAddressTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
	TRAP(err,operations.AddParsedReplyAddressL(parsedReplyAddressTGsm));
	TEST(err==KErrNone);
	//Check for reply address - should contain
	result=operations.ContainsReplyAddressIEL();
	TEST(result);

	//Get Parsed address
	TGsmSmsTelNumber retrievedReplyAddressTGsm;
	TRAP(err,result=operations.GetParsedReplyAddressL(retrievedReplyAddressTGsm));
	TEST(err==KErrNone);
	TEST(result);
	TEST(parsedReplyAddressTGsm.iTelNumber==retrievedReplyAddressTGsm.iTelNumber);

	//Send message with one parsed reply address
	SendSmsL(smsMessage,socket);
	// ------- END Send parsed reply

	CleanupStack::PopAndDestroy(smsMessage);
	//-------- END Sending

	//Receive a message with no reply address
	INFO_PRINTF1(_L("waiting for incoming SMS with no reply address...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsReplyAddressOperations& deliverOperations =
		static_cast<CSmsReplyAddressOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsReplyAddressFormat));

	//Check has no reply address
	result=deliverOperations.ContainsReplyAddressIEL();
	TEST(result==0);

	//Try to get non existent address
	// as HBufC
	TRAP(err,replyAddressHBuf=deliverOperations.GetReplyAddressL());
	TEST(err==KErrNotFound);

	// as TGsmSmsTelNumber
	TRAP(err,result=deliverOperations.GetParsedReplyAddressL(replyAddressTGsm));
	TEST(err==KErrNone);
	TEST(result==0);

	CleanupStack::PopAndDestroy(smsMessage);
	//-------- END Receive with no reply address

	INFO_PRINTF1(_L("waiting for incoming SMS with reply address...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsReplyAddressOperations& deliverReplyOperations =
		static_cast<CSmsReplyAddressOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsReplyAddressFormat));

	//Check has reply address
	result=deliverReplyOperations.ContainsReplyAddressIEL();
	TEST(result==1);

	//Get address
	// as HBufC
	TRAP(err,replyAddressHBuf=deliverReplyOperations.GetReplyAddressL());
	CleanupStack::PushL(replyAddressHBuf);
	TEST(err==KErrNone);
	TPtr buffReplyPtr(replyAddressHBuf->Des());
	TEST(buffReplyPtr==replyAddress);

	CleanupStack::PopAndDestroy(replyAddressHBuf);
	CleanupStack::PopAndDestroy(smsMessage);
	//-------- END Receive with reply address

	INFO_PRINTF1(_L("waiting for incoming SMS with parsed reply address...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsReplyAddressOperations& deliverParsedReplyOperations =
		static_cast<CSmsReplyAddressOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsReplyAddressFormat));

	//Check has reply address
	result=deliverParsedReplyOperations.ContainsReplyAddressIEL();
	TEST(result==1);

	//Get Parsed address
	//Test is panicing here ---
	TRAP(err,result=deliverParsedReplyOperations.GetParsedReplyAddressL(retrievedReplyAddressTGsm));
	TEST(err==KErrNone);
	TEST(result);
	TEST(parsedReplyAddressTGsm.iTelNumber==retrievedReplyAddressTGsm.iTelNumber);

	CleanupStack::PopAndDestroy(smsMessage);
	//-------- END Receive with parsed reply address

	//This PDU has three reply address in it.  We are looking for the stack to store only the last occurence
	INFO_PRINTF1(_L("waiting for incoming SMS with three reply addresses...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	//TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsReplyAddressOperations& deliverMultipleReplyOperations =
		static_cast<CSmsReplyAddressOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsReplyAddressFormat));

	//Check has reply address
	result=deliverMultipleReplyOperations.ContainsReplyAddressIEL();
	TEST(result==1);

	//Get Parsed address
	TPtrC multiReplyAddress(_L("+447583927596"));

	TRAP(err,replyAddressHBuf=deliverMultipleReplyOperations.GetReplyAddressL());
	TEST(err==KErrNone);
	CleanupStack::PushL(replyAddressHBuf);
	TPtr multiBuffPtr(replyAddressHBuf->Des());
	TEST(multiBuffPtr==multiReplyAddress);
	CleanupStack::PopAndDestroy(replyAddressHBuf);
	CleanupStack::PopAndDestroy(smsMessage);
	//-------- END receive with multiple reply address

	//This PDU has three reply address in it.  We are looking for the stack to store only the last occurence
	INFO_PRINTF1(_L("waiting for incoming SMS with corrupt reply address...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	//Get the operations
	CSmsReplyAddressOperations& corruptReplyOperations = static_cast<CSmsReplyAddressOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsReplyAddressFormat));
	//Check has reply address
	result=corruptReplyOperations.ContainsReplyAddressIEL();
	TEST(result==1);
	//Get address
	TRAP(err,replyAddressHBuf=corruptReplyOperations.GetReplyAddressL());
	TEST(err==KErrCorrupt);
	CleanupStack::PopAndDestroy(smsMessage);

	//-------- END receive with corrupt reply address
	//-------- END receive
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
}


TVerdict CTestMultiplePDUReplyAddress::doTestStepL()
/**
 *  Test a simple Transmit and Receive of a TPDU
 */
	{
    // TODO: redundant test case???
	INFO_PRINTF1(_L("Test multiple PDUs with reply address in SMS message"));

	return TestStepResult();
	}


TVerdict CTestSpecialMessageOperations::doTestStepL()
/**
 *  Test the operations available for special message waiting
 */
	{
	INFO_PRINTF1(_L("Test special message waiting operations"));
	//This is character msg - 96 characters
	_LIT(KTestMsg,"This message is a special message waiting message. You have a special message waiting for you :)");

	//8 bit coding scheme
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	//Get the operations for a version 0 message i.e. before operations for IE were added
	smsMessage->SetVersion(CSmsMessage::ESmsMessageV0);
	TRAPD(err,smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));
	TEST(err==KErrNotSupported);

	smsMessage->SetVersion(CSmsMessage::ESmsMessageV1);
	//Get the operations for a version 1 message
	CSmsSpecialSMSMessageOperations& operations = static_cast<CSmsSpecialSMSMessageOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));

	//get a IE Identifier operations for future use
	TRAP(err,smsMessage->GetOperationsForIEL(CSmsInformationElement::TSmsInformationElementIdentifier(0xE0) ));
	TEST(err==KErrArgument);

	//get operations for a IE identifier not currently supported: ESmsIEIRFC822EmailHeader
	TRAP(err,smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEIRFC822EmailHeader ));
	TEST(err==KErrArgument);

	//Lets bash these API's, make sure they work correctly !
	INFO_PRINTF1(_L("Testing API's with message with no special messages"));
	TUint count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==0);

	//Set the type
	//Voice message, No extended message type
	TSmsMessageIndicationType messageIndicationType=EGsmSmsVoiceMessageWaiting;
	TExtendedSmsIndicationType extendedType=EGsmSmsNoExtendedMessageTypeIndication;

	//delete a special message
	TRAP(err,operations.RemoveSpecialMessageIndicationL(messageIndicationType,extendedType));
	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==0);
	//delete all special messagRemoveAllSpecialMessageIndicationsLe
	TRAP(err,operations.RemoveAllSpecialMessageIndicationsL());
	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==0);

	//Get special message
	TUint index=0;
	TBool toStore=EFalse;
	TSmsMessageProfileType messageProfileType;
	TUint8 messageCount=0;
	TRAP(err,operations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));

	TEST(err==KErrArgument);

	//Add special messages
	INFO_PRINTF1(_L("Testing API AddSpecialMessageIndicationL"));
	toStore=ETrue;
	messageCount=1;
	messageProfileType=EGsmSmsProfileId1;
	TRAP(err,operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));

	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==1);

	//Get this message
	//Change the current values = ensure the API is changing them
	INFO_PRINTF1(_L("Testing API GetMessageIndicationIEL"));
	toStore=EFalse;
	messageCount=10;
	messageIndicationType=EGsmSmsFaxMessageWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageProfileType=EGsmSmsProfileId2;

	TRAP(err,operations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsVoiceMessageWaiting);
	TEST(extendedType==EGsmSmsNoExtendedMessageTypeIndication);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount==1);

	//Add the same type, special message will be updated.
	toStore=ETrue;
	messageCount=10;
	messageProfileType=EGsmSmsProfileId1;
	TRAP(err,operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
							extendedType,messageProfileType,messageCount));

	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==1);

	//Add a different type
	//Add special messages
	toStore=ETrue;
	messageIndicationType=EGsmSmsFaxMessageWaiting;
	extendedType=EGsmSmsNoExtendedMessageTypeIndication;
	messageProfileType=EGsmSmsProfileId1;
	messageCount=3;
	TRAP(err,operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));

	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==2);

	//Get the type
	toStore=EFalse;
	messageCount=10;
	messageIndicationType=EGsmSmsVoiceMessageWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageProfileType=EGsmSmsProfileId2;
	index=1;

	TRAP(err,operations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsFaxMessageWaiting);
	TEST(extendedType==EGsmSmsNoExtendedMessageTypeIndication);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount==3);


	//Add another different type
	//Email message, No extended message type
	messageIndicationType=EGsmSmsElectronicMailMessageWaiting;
	extendedType=EGsmSmsNoExtendedMessageTypeIndication;
	messageProfileType=EGsmSmsProfileId1;
	messageCount=5;
	TRAP(err,operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));

	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==3);

	//Get this type
	toStore=EFalse;
	messageCount=10;
	messageIndicationType=EGsmSmsVoiceMessageWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageProfileType=EGsmSmsProfileId2;
	index=2;

	TRAP(err,operations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsElectronicMailMessageWaiting);
	TEST(extendedType==EGsmSmsNoExtendedMessageTypeIndication);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount==5);

	//Add another diffent type
	//Special message, extended type=video message
	messageIndicationType=EGsmSmsExtendedMessageTypeWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageCount=7;
	TRAP(err,operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));

	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==4);

	//Get this type
	toStore=EFalse;
	messageCount=10;
	messageIndicationType=EGsmSmsVoiceMessageWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageProfileType=EGsmSmsProfileId2;
	index=3;

	TRAP(err,operations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsExtendedMessageTypeWaiting);
	TEST(extendedType==EGsmSmsVideoMessageWaiting);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount==7);

	//Remove first Special message
	INFO_PRINTF1(_L("Testing API RemoveSpecialMessageIndicationL"));
	messageIndicationType=EGsmSmsVoiceMessageWaiting;
	extendedType=EGsmSmsNoExtendedMessageTypeIndication;
	TRAP(err,operations.RemoveSpecialMessageIndicationL(messageIndicationType,extendedType));
	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==3);

	//Get index 0
	toStore=EFalse;
	messageCount=10;
	messageIndicationType=EGsmSmsVoiceMessageWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageProfileType=EGsmSmsProfileId2;
	index=0;

	TRAP(err,operations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsFaxMessageWaiting);
	TEST(extendedType==EGsmSmsNoExtendedMessageTypeIndication);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount=3);

	//Get Index out of range, boundary condition
	index=3;
	TRAP(err,operations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrArgument);

	//Get index out of range, massive
	index=25;
	TRAP(err,operations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrArgument);

	//Remove all
	INFO_PRINTF1(_L("Testing API RemoveAllSpecialMessageIndicationsL"));
	TRAP(err,operations.RemoveAllSpecialMessageIndicationsL());
	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==0);


	//Regression test of previous existing API's
	CSmsPDU &pdu=smsMessage->SmsPDU();
	//Get the UserData
	CSmsUserData &userData=pdu.UserData();

	_LIT8(KSpecialMessageString,"0500");	//5 voice mails
	TRAP(err,userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,KSpecialMessageString));
	TEST(err==KErrNone);
	count=userData.NumInformationElements();
	TEST(count==1);

	//Get using Type
	TInt returnedIndex=-5;
	TBool callResult = EFalse;
	TRAP(err,callResult=userData.InformationElementIndex(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,returnedIndex));
	TEST(err==KErrNone);
	TEST(callResult);
	TEST(returnedIndex==0);

	//Get using index
	index=0;
	CSmsInformationElement& informationElement=userData.InformationElement(index);
	CSmsInformationElement::TSmsInformationElementIdentifier identifier=informationElement.Identifier();
	TEST(identifier==CSmsInformationElement::ESmsIEISpecialSMSMessageIndication);

	//Remove the indication
	index=0;
	userData.RemoveInformationElement(index);
	count=userData.NumInformationElements();
	TEST(count==0);

	//What about a combination
	//Add with existing CUserData::AddInformationElement
	TRAP(err,userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,KSpecialMessageString));
	TEST(err==KErrNone);
	count=userData.NumInformationElements();
	TEST(count==1);

	//Add special message waiting with operations
	toStore=ETrue;
	messageIndicationType=EGsmSmsVoiceMessageWaiting;
	extendedType=EGsmSmsNoExtendedMessageTypeIndication;
	messageProfileType=EGsmSmsProfileId1;
	messageCount=5;
	TRAP(err,operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==2);

	//Add another with existing CUserData::AddInformationElement
	TRAP(err,userData.AddInformationElementL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication,KSpecialMessageString));
	TEST(err==KErrAlreadyExists);
	count=userData.NumInformationElements();
	TEST(count==2);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==2);

	//Get IE
	index=0;
	CSmsInformationElement &informationElementIndexZero=userData.InformationElement(index);
	identifier=informationElementIndexZero.Identifier();
	TEST(identifier==CSmsInformationElement::ESmsIEISpecialSMSMessageIndication);

	index=1;
	CSmsInformationElement &informationElementIndexOne=userData.InformationElement(index);
	identifier=informationElementIndexOne.Identifier();
	TEST(identifier==CSmsInformationElement::ESmsIEISpecialSMSMessageIndication);

	//Remove
	index=1;
	userData.RemoveInformationElement(index);
	count=userData.NumInformationElements();
	TEST(count==1);

	index=0;
	userData.RemoveInformationElement(index);
	count=userData.NumInformationElements();
	TEST(count==0);

	INFO_PRINTF1(_L("Test step end !"));

	CleanupStack::PopAndDestroy(smsMessage);
	//-------- END API Test for special message operations
	return TestStepResult();
	}

TVerdict CTestSinglePDUSpecialMessageWaiting::doTestStepL()
/**
 *  Test a simple Transmit and Receive of a TPDU
 */
	{
	INFO_PRINTF1(_L("Test special message waiting in single PDU SMS message"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	//This is a 95 character msg.
	_LIT(KTestMsg,"This message is a special message waiting message. You have a special message waiting for you :)");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//8 bit coding scheme
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S "),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S "),&iServiceCenterNumber);

	//Get the operations
	CSmsSpecialSMSMessageOperations& operations = static_cast<CSmsSpecialSMSMessageOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));
	TUint count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==0);

	//Send message with no special message indication
	SendSmsL(smsMessage,socket);

	//Send message with special message indication - voice mail
	INFO_PRINTF1(_L("Creating message with voice mail waiting"));
	TSmsMessageIndicationType messageIndicationType=EGsmSmsVoiceMessageWaiting;
	TExtendedSmsIndicationType extendedType=EGsmSmsNoExtendedMessageTypeIndication;
	TSmsMessageProfileType messageProfileType=EGsmSmsProfileId1;
	TUint index=0;
	TBool toStore=ETrue;
	TUint8 messageCount=5;

	//Add special messages
	TRAPD(err,operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==1);

	//Send message with no voice mail message indication
	SendSmsL(smsMessage,socket);

	//Send message with special message indication - voice mail, fax, email, and enhanced message
	//Add the special message indications - FAX
	INFO_PRINTF1(_L("Creating message with fax waiting"));
	messageIndicationType=EGsmSmsFaxMessageWaiting;
	extendedType=EGsmSmsNoExtendedMessageTypeIndication;
	messageCount=2;
	//Add IE
	TRAP(err,operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==2);

	//Add the special message indications - Email
	INFO_PRINTF1(_L("Creating message with email waiting"));
	messageIndicationType=EGsmSmsElectronicMailMessageWaiting;
	messageCount=3;

	//Add IE
	TRAP(err,operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==3);

	//Add the special message indications - Enhanced message waiting
	INFO_PRINTF1(_L("Creating message with enhanced message waiting"));
	messageIndicationType=EGsmSmsExtendedMessageTypeWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageCount=1;
	//Add IE
	TRAP(err,operations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	count=operations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==4);

	(void) smsMessage->TextPresent();
	(void) smsMessage->NumMessagePDUsL();
	(void) smsMessage->MaxMessageLength();
	(void) smsMessage->MessageLengthL();

	//Send message with one of each type of special message IE
	SendSmsL(smsMessage,socket);

	CleanupStack::PopAndDestroy(smsMessage);
	//-------- END Sending Single PDU

	//-------- START receiving
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	//Get the operations
	CSmsSpecialSMSMessageOperations& noSpecialMessageOperations = static_cast<CSmsSpecialSMSMessageOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));
	count=noSpecialMessageOperations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==0);

	TestSmsContentsL(smsMessage,KTestMsg);

	CleanupStack::PopAndDestroy(smsMessage);
	//-------- End receive no SMW

	INFO_PRINTF1(_L("waiting for incoming SMS with voice mail waiting...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsSpecialSMSMessageOperations& voiceMailMessageOperations = static_cast<CSmsSpecialSMSMessageOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));

	count=voiceMailMessageOperations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==1);

	//Get this type
	toStore=EFalse;
	messageCount=0;
	messageIndicationType=EGsmSmsExtendedMessageTypeWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageProfileType=EGsmSmsProfileId2;
	index=0;

	TRAP(err,voiceMailMessageOperations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsVoiceMessageWaiting);
	TEST(extendedType==EGsmSmsNoExtendedMessageTypeIndication);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount==10);

	CleanupStack::PopAndDestroy(smsMessage);
	//-------- End receive with voice mail, count==10

	INFO_PRINTF1(_L("waiting for incoming SMS with one of each type of special message waiting IE...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsSpecialSMSMessageOperations& multipleSpecialMessageOperations = static_cast<CSmsSpecialSMSMessageOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));

	count=multipleSpecialMessageOperations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==4);

	//Get them
	//Reset values
	toStore=EFalse;
	messageCount=0;
	messageIndicationType=EGsmSmsExtendedMessageTypeWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageProfileType=EGsmSmsProfileId2;
	index=0;

	//Get at index 0 - voice mail
	TRAP(err,multipleSpecialMessageOperations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsVoiceMessageWaiting);
	TEST(extendedType==EGsmSmsNoExtendedMessageTypeIndication);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount==8);

	//Get at index 1 - Fax
	toStore=EFalse;
	messageCount=0;
	messageIndicationType=EGsmSmsExtendedMessageTypeWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageProfileType=EGsmSmsProfileId2;
	index=1;
	TRAP(err,multipleSpecialMessageOperations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsFaxMessageWaiting);
	TEST(extendedType==EGsmSmsNoExtendedMessageTypeIndication);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount==3);

	//Get at index 2 - Email
	toStore=EFalse;
	messageCount=0;
	messageIndicationType=EGsmSmsExtendedMessageTypeWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageProfileType=EGsmSmsProfileId2;
	index=2;
	TRAP(err,multipleSpecialMessageOperations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsElectronicMailMessageWaiting);
	TEST(extendedType==EGsmSmsNoExtendedMessageTypeIndication);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount==1);

	//Get at index 3 - Enhanced message
	toStore=EFalse;
	messageCount=0;
	messageProfileType=EGsmSmsProfileId2;
	index=3;
	TRAP(err,multipleSpecialMessageOperations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsExtendedMessageTypeWaiting);
	TEST(extendedType==EGsmSmsVideoMessageWaiting);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount==2);

	CleanupStack::PopAndDestroy(smsMessage);
	//-------- End receive with all instanced of special message,voice mail(8),Fax(3),Email(1),Enhanced(2)

	//Receive message with three fax IE,and one enhanced
	INFO_PRINTF1(_L("waiting for incoming SMS with multiple instances of special message waiting IE...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsSpecialSMSMessageOperations& multipleInstancesSpecialMessageOperations = static_cast<CSmsSpecialSMSMessageOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));

	//Should only detect two valid
	count=multipleInstancesSpecialMessageOperations.GetCountOfSpecialMessageIndicationsL();
	TEST(count==4);

	//Get these two, FAX,and Enhanced
	//Reset values
	toStore=EFalse;
	messageCount=0;
	messageIndicationType=EGsmSmsExtendedMessageTypeWaiting;
	extendedType=EGsmSmsVideoMessageWaiting;
	messageProfileType=EGsmSmsProfileId2;
	index=0;

	//Get at index 0 - Fax mail
	TRAP(err,multipleInstancesSpecialMessageOperations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsFaxMessageWaiting);
	TEST(extendedType==EGsmSmsNoExtendedMessageTypeIndication);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount==1);

	//Reset values
	toStore=EFalse;
	messageCount=0;
	messageProfileType=EGsmSmsProfileId2;
	index=3;

	//Get at index 3 - Enhanced
	TRAP(err,multipleInstancesSpecialMessageOperations.GetMessageIndicationIEL(index,toStore,messageIndicationType,
								extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);
	TEST(toStore);
	TEST(messageIndicationType==EGsmSmsExtendedMessageTypeWaiting);
	TEST(extendedType==EGsmSmsVideoMessageWaiting);
	TEST(messageProfileType==EGsmSmsProfileId1);
	TEST(messageCount==4);

	CleanupStack::PopAndDestroy(smsMessage);
	//-------- END receive multiple instances of enhanced IE

	//-------- END receive
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

TVerdict CTestMultiplePDUSpecialMessageWaiting::doTestStepL()
/**
 *  Test a simple Transmit and Receive of a TPDU
 */
	{
	INFO_PRINTF1(_L("Test special message waiting in multiple PDU SMS message"));

	return TestStepResult();
	}

TVerdict CTestEnhancedVoiceMailOperations::doTestStepL()
/**
 *  Test the operations available for special message waiting
 */
	{
	INFO_PRINTF1(_L("Test enhanced voice mail message operations"));
	//This is character msg - 96 characters
	_LIT(KTestMsg,"This is a enhanced voice mail information message!");

	//8 bit coding scheme
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	//Get the operations
	CSmsEnhancedVoiceMailOperations& operations =
			static_cast<CSmsEnhancedVoiceMailOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));

	//Lets bash these API's, make sure they work correctly !
	INFO_PRINTF1(_L("Testing API's for enhanced voice mail information operations"));
	//ContainsEnhancedVoiceMailL
	TBool result=operations.ContainsEnhancedVoiceMailIEL();
	TEST(!result);
	//RemoveEnhancedVoiceMailL
	TRAPD(err,operations.RemoveEnhancedVoiceMailIEL());
	TEST(err==KErrNotFound);
	//CopyEnhancedVoidMail
	TRAP(err,operations.CopyEnhancedVoiceMailIEL());
	TEST(err==KErrNotFound);

	//Create CEnhancedVoiceMailNotification
	CEnhancedVoiceMailNotification* enhancedVoiceMailNotification=CEnhancedVoiceMailNotification::NewL();
	CleanupStack::PushL(enhancedVoiceMailNotification);
	//Set the properties
	TSmsMessageProfileType messageProfileType=EGsmSmsProfileId1;
	enhancedVoiceMailNotification->SetProfile(messageProfileType);
	enhancedVoiceMailNotification->SetStorage(ETrue);
	enhancedVoiceMailNotification->SetAlmostMaximumCapacity(ETrue);
	enhancedVoiceMailNotification->SetMaximumCapacity(EFalse);
	enhancedVoiceMailNotification->SetNumberOfVoiceMessages(10);

	//Add access address - mail box address
	TPtrC parsedAccessAddress;
	GetStringFromConfig(ConfigSection(), _L("parsedAccessAddress"), parsedAccessAddress);
	TGsmSmsTelNumber accessAddressTGsm;
	accessAddressTGsm.iTelNumber=parsedAccessAddress;
	accessAddressTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
	enhancedVoiceMailNotification->SetParsedAccessAddressL(accessAddressTGsm);

	//Create mail messages
	CVoiceMailNotification* voiceMailNotification=CVoiceMailNotification::NewL();

	//Setup the message
	TUint16 messageId=1;
	TUint8 messageLength=20;
	voiceMailNotification->SetMessageId(messageId);
	voiceMailNotification->SetMessageLength(messageLength);
	TUint8 daysRetention=5;
	voiceMailNotification->SetRetentionDays(daysRetention);
	voiceMailNotification->SetPriorityIndication(ETrue);

	//Test the boundary values for calling line identity
	TUint count=0;
	//Do this test for both parsed address and international numbers
	for (TUint addressType=EInternational; addressType<=EParsed; ++addressType)
	{
		for(TUint boundaryCheck=EInside; boundaryCheck<=EOutside; ++boundaryCheck)
		{
			//Just before the boundary
			TPtrC boundaryCallingLineIdentity;
			if (boundaryCheck==EInside)
			{
				if(addressType==EParsed)
				{
					GetStringFromConfig(ConfigSection(), _L("tenCharacterParsedReplyAddress"), boundaryCallingLineIdentity);
				}
				else
				{
					GetStringFromConfig(ConfigSection(), _L("nineteenDigitReplyAddress"), boundaryCallingLineIdentity);
				}
			}
			//on the boundary
			else if(boundaryCheck==EOn)
			{
				if(addressType==EParsed)
				{
					GetStringFromConfig(ConfigSection(), _L("elevenCharacterParsedReplyAddress"), boundaryCallingLineIdentity);
				}
				else
				{
					GetStringFromConfig(ConfigSection(), _L("twentyDigitReplyAddress"), boundaryCallingLineIdentity);
				}

			}
			//Just past the boundary
			else if(boundaryCheck==EOutside)
			{
				if(addressType==EParsed)
				{
					GetStringFromConfig(ConfigSection(), _L("twelveCharacterParsedReplyAddress"), boundaryCallingLineIdentity);
				}
				else
				{
					GetStringFromConfig(ConfigSection(), _L("twentyOneDigitReplyAddress"), boundaryCallingLineIdentity);
				}
			}

			//Add
			AddAddressToNotifcationL(enhancedVoiceMailNotification,
					voiceMailNotification, boundaryCallingLineIdentity,addressType);

			//Add the enhanced voice mail to the message
			TRAP(result,operations.AddEnhancedVoiceMailIEL(*enhancedVoiceMailNotification));

			//Outside the boundary produces different results
			if(boundaryCheck==EOutside)
			{
				TEST(result==KErrArgument);
				result=operations.ContainsEnhancedVoiceMailIEL();
				TEST(!result);

			}
			else
			{
				TEST(result==KErrNone);
				result=operations.ContainsEnhancedVoiceMailIEL();
				TEST(result);

				//Remove
				CEnhancedVoiceMailNotification* notNeededEnhancedVoiceMailNotification =
					static_cast<CEnhancedVoiceMailNotification*>(operations.RemoveEnhancedVoiceMailIEL() );
				result=operations.ContainsEnhancedVoiceMailIEL();
				TEST(!result);
				count=notNeededEnhancedVoiceMailNotification->NumberOfVoiceMails();
				TEST(count==1);

				delete notNeededEnhancedVoiceMailNotification;
			}
		}
	}

	voiceMailNotification->SetParsedCallingLineIdentityL(accessAddressTGsm);

	//Get the notifications in the enhanced message
	RPointerArray<CVoiceMailNotification>& newNotificationsList=
					enhancedVoiceMailNotification->GetVoiceMailNotifications();

	count=newNotificationsList.Count();
	//Remove all pointers to notifications
	if(count)
	    {
        newNotificationsList.Reset();
	    }

	result=newNotificationsList.Append(voiceMailNotification);
	TEST(result==KErrNone);
	count=enhancedVoiceMailNotification->NumberOfVoiceMails();
	TEST(count==1);
	//Add
	TRAP(err,operations.AddEnhancedVoiceMailIEL(*enhancedVoiceMailNotification));
	TEST(err==KErrNone);

	//Contains
	//ContainsEnhancedVoiceMailL
	result=operations.ContainsEnhancedVoiceMailIEL();
	TEST(result);
	//Add
	TRAP(err,operations.AddEnhancedVoiceMailIEL(*enhancedVoiceMailNotification));
	TEST(err==KErrAlreadyExists);
	//Copy
	CEnhancedVoiceMailBoxInformation* retrievedNotification = NULL;
	TRAP(err,retrievedNotification=operations.CopyEnhancedVoiceMailIEL() );
	TEST(err==KErrNone);
	CleanupStack::PushL(retrievedNotification);

	//Check the contents
	//Type
	TVoiceMailInfoType typeInfo=retrievedNotification->Type();
	TEST(typeInfo==EGsmSmsVoiceMailNotification);
	//Profile
	TSmsMessageProfileType profile=retrievedNotification->Profile();
	TEST(profile==EGsmSmsProfileId1);
	//Store
	TBool isStore=retrievedNotification->Store();
	TEST(isStore);
	//Almost Capacity
	TBool isAlmostMaxCapacity=retrievedNotification->AlmostMaximumCapacity();
	TEST(isAlmostMaxCapacity);
	//MaximumCapacity
	TBool isMaximumCapacity=retrievedNotification->MaximumCapacity();
	TEST(!isMaximumCapacity);
	//Access address - mail box address
	TGsmSmsTelNumber retrievedAccessAddressTGsm;
	retrievedNotification->ParsedAccessAddress(retrievedAccessAddressTGsm);
	TEST(retrievedAccessAddressTGsm.iTelNumber==accessAddressTGsm.iTelNumber);
	//Number Of Voice Messages
	count=retrievedNotification->NumberOfVoiceMessages();
	TEST(count==10);

	//Cast the type, get the numberOfVoiceMails
	//Number of voice mails
	CEnhancedVoiceMailNotification* retrievedEnhancedNotification=
						static_cast<CEnhancedVoiceMailNotification*>(retrievedNotification);
	count=retrievedEnhancedNotification->NumberOfVoiceMails();
	TEST(count==1);
	//contains
	result=operations.ContainsEnhancedVoiceMailIEL();
	TEST(result);
	//remove
	CEnhancedVoiceMailNotification* throwAwayEnhancedVoiceMailNotification = NULL;
	TRAP(err, throwAwayEnhancedVoiceMailNotification =
		static_cast<CEnhancedVoiceMailNotification*>(operations.RemoveEnhancedVoiceMailIEL() ));

	TEST(err==KErrNone);

	delete throwAwayEnhancedVoiceMailNotification;

	//contains
	result=operations.ContainsEnhancedVoiceMailIEL();
	TEST(!result);

	CleanupStack::PopAndDestroy(retrievedNotification);
	CleanupStack::PopAndDestroy(enhancedVoiceMailNotification);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

//Helper function:  Adds the address to a notification
void CTestEnhancedVoiceMailOperations::AddAddressToNotifcationL(
			CEnhancedVoiceMailNotification* aEnhancedVoiceMailNotification,
			CVoiceMailNotification* aVoiceMailNotification,
			TPtrC aCallingLineIdentity,TUint aAddressType)
    {
	//Get the notifications in the enhanced message
	RPointerArray<CVoiceMailNotification>& notificationsList=
					aEnhancedVoiceMailNotification->GetVoiceMailNotifications();

	TUint count=notificationsList.Count();

	if(count==1)
	   {
		notificationsList.Remove(0);
	   }

	//Add the address
	if(aAddressType==EParsed)
	   {
		//Create Calling Line Identity
		TGsmSmsTelNumber callingLineIdentityTGsm;
		callingLineIdentityTGsm.iTelNumber=aCallingLineIdentity;
		callingLineIdentityTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
		aVoiceMailNotification->SetParsedCallingLineIdentityL(callingLineIdentityTGsm);
    	}
	else
	    {
        aVoiceMailNotification->SetCallingLineIdentityL(aCallingLineIdentity);
	    }

	//Add the notification to the voice mail
	TInt result=notificationsList.Append(aVoiceMailNotification);
	TEST(result==KErrNone);
}

TVerdict CTestSinglePDUEnhancedVoiceMailInformation::doTestStepL()
/**
 *  Test the sending and receiving of enhanced voice mail
 */
	{
	INFO_PRINTF1(_L("Test enhanced voice mail information with a single PDU message"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	//This is an enhanced voice mail character msg.
	_LIT(KTestMsg,"");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//8 bit coding scheme
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S "),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S "),&iServiceCenterNumber);

	//Get the operations
	CSmsEnhancedVoiceMailOperations& operations =
		static_cast<CSmsEnhancedVoiceMailOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));

	//ContainsEnhancedVoiceMailL
	TBool result=operations.ContainsEnhancedVoiceMailIEL();
	TEST(!result);

	//Send message with no special message indication
	SendSmsL(smsMessage,socket);

	//Create message with Enhanced void mail information
	//Add access address - mail box address
	TPtrC usualAccessAddress;
	GetStringFromConfig(ConfigSection(), _L("accessAddress"), usualAccessAddress);
	TGsmSmsTelNumber accessAddressTGsm;
	accessAddressTGsm.iTelNumber=usualAccessAddress;
	accessAddressTGsm.iTypeOfAddress=EGsmSmsTONInternationalNumber;

	TUint length=accessAddressTGsm.iTelNumber.Length();

	CEnhancedVoiceMailNotification* enhancedVoiceMailNotification=CreateEnhancedVoiceMailNotificationL(accessAddressTGsm);
	CleanupStack::PushL(enhancedVoiceMailNotification);

	//Create notifications
	TPtrC usualCallingLineIdentity;
	GetStringFromConfig(ConfigSection(), _L("callingLineIdentity"), usualCallingLineIdentity);
	TGsmSmsTelNumber callingLineIdentityTGsm;
	callingLineIdentityTGsm.iTelNumber=usualCallingLineIdentity;
	callingLineIdentityTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;

	length=callingLineIdentityTGsm.iTelNumber.Length();


	CVoiceMailNotification* voiceMailNotification=CreateVoiceMailNotificationL(callingLineIdentityTGsm);

	//Add this to the list of notifications in the enhanced message
	RPointerArray<CVoiceMailNotification>& notificationsList=
					enhancedVoiceMailNotification->GetVoiceMailNotifications();

	result=notificationsList.Append(voiceMailNotification);
	TEST(result==KErrNone);
	TUint count=enhancedVoiceMailNotification->NumberOfVoiceMails();
	TEST(count==1);
	//Add Enhanced Voice Mail Information to message
	TRAPD(err,operations.AddEnhancedVoiceMailIEL(*enhancedVoiceMailNotification));
	TEST(err==KErrNone);

	(void) smsMessage->TextPresent();
	(void) smsMessage->NumMessagePDUsL();
	(void) smsMessage->MaxMessageLength();
	(void) smsMessage->MessageLengthL();

	//Send the message
	SendSmsL(smsMessage,socket);

	CleanupStack::Pop(enhancedVoiceMailNotification);

	CleanupStack::PopAndDestroy(smsMessage);

	//-----------------------------------
	//Send message with enhanced voice mail length at boundary values
	//Single pdu, max size = 137.  That is 140-2(UDHL) -1(IEId) = 137
	//User data = 8C
	//Make a new message
	for (TUint i = 0; i < enhancedVoiceMailNotification->NumberOfVoiceMails(); ++i)
	    {
	    delete notificationsList[i];
	    }
	notificationsList.Reset();

	CSmsMessage* boundaryValueVoiceMessage=CreateSmsMessageL(KTestMsg,alphabet);
	CleanupStack::PushL(boundaryValueVoiceMessage);
	CleanupStack::PushL(enhancedVoiceMailNotification);

	//Get the operations
	CSmsEnhancedVoiceMailOperations& boundaryValueVoiceMailOperations =
		static_cast<CSmsEnhancedVoiceMailOperations&>(boundaryValueVoiceMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));

	//Add five more notifications
	const TUint KNumNotifications=4;
	for(count=0; count<KNumNotifications; ++count)
	   {
		CVoiceMailNotification* voiceMailNotification=CreateVoiceMailNotificationL(callingLineIdentityTGsm);
		result=notificationsList.Append(voiceMailNotification);
		TEST(result==KErrNone);
	   }

	//Add the number to make the size on the boundary
	TPtrC boundaryCallingLineIdentity;
	GetStringFromConfig(ConfigSection(), _L("boundaryAccessAddress"), boundaryCallingLineIdentity);
	callingLineIdentityTGsm.iTelNumber=boundaryCallingLineIdentity;
	callingLineIdentityTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
	CVoiceMailNotification* boundaryMailNotification=CreateVoiceMailNotificationL(callingLineIdentityTGsm);
	result=notificationsList.Append(boundaryMailNotification);
	TEST(result==KErrNone);
	count=enhancedVoiceMailNotification->NumberOfVoiceMails();
	TEST(count==5);

	//Add Enhanced Voice Mail Information to message
	TRAP(err,boundaryValueVoiceMailOperations.AddEnhancedVoiceMailIEL(*enhancedVoiceMailNotification));
	TEST(err==KErrNone);
	//Send the message
	SendSmsL(boundaryValueVoiceMessage,socket);

	CleanupStack::Pop(enhancedVoiceMailNotification);
	CleanupStack::PopAndDestroy(boundaryValueVoiceMessage);
	delete enhancedVoiceMailNotification;

	//---------- END sending


	//Receiving message with no enhanced voice mail
	INFO_PRINTF1(_L("waiting for incoming SMS with no enhanced voice mail...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsEnhancedVoiceMailOperations& noEnhancedVoiceMailOperations =
		static_cast<CSmsEnhancedVoiceMailOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));

	//ContainsEnhancedVoiceMailL
	result=noEnhancedVoiceMailOperations.ContainsEnhancedVoiceMailIEL();
	TEST(!result);

	CleanupStack::PopAndDestroy(smsMessage);

	//Receive message with enhanced voice mail information
	INFO_PRINTF1(_L("waiting for incoming SMS with enhanced voice information...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsEnhancedVoiceMailOperations& enhancedVoiceMailOperations =
		static_cast<CSmsEnhancedVoiceMailOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));

	//ContainsEnhancedVoiceMailL
	result=enhancedVoiceMailOperations.ContainsEnhancedVoiceMailIEL();
	TEST(result);

	//Copy
	CEnhancedVoiceMailBoxInformation* retrievedNotification = NULL;
	TRAP(err,retrievedNotification=enhancedVoiceMailOperations.CopyEnhancedVoiceMailIEL() );
	TEST(err==KErrNone);
	CleanupStack::PushL(retrievedNotification);
	//Check the contents
	//Type
	TVoiceMailInfoType typeInfo=retrievedNotification->Type();
	TEST(typeInfo==EGsmSmsVoiceMailNotification);
	//Profile
	TSmsMessageProfileType profile=retrievedNotification->Profile();
	TEST(profile==EGsmSmsProfileId1);
	//Store
	TBool isStore=retrievedNotification->Store();
	TEST(isStore);
	//Almost Capacity
	TBool isAlmostMaxCapacity=retrievedNotification->AlmostMaximumCapacity();
	TEST(isAlmostMaxCapacity);
	//MaximumCapacity
	TBool isMaximumCapacity=retrievedNotification->MaximumCapacity();
	TEST(!isMaximumCapacity);
	//Access address - mail box address
	TGsmSmsTelNumber retrievedAccessAddressTGsm;
	retrievedNotification->ParsedAccessAddress(retrievedAccessAddressTGsm);
	TEST(retrievedAccessAddressTGsm.iTelNumber==accessAddressTGsm.iTelNumber);
	//Number Of Voice Messages
	count=retrievedNotification->NumberOfVoiceMessages();
	TEST(count==5);
	//Cast the type, get the numberOfVoiceMails
	//Number of voice mails
	CEnhancedVoiceMailNotification* retrievedEnhancedNotification=
						static_cast<CEnhancedVoiceMailNotification*>(retrievedNotification);
	count=retrievedEnhancedNotification->NumberOfVoiceMails();
	TEST(count==1);

	CleanupStack::PopAndDestroy(retrievedNotification);
	CleanupStack::PopAndDestroy(smsMessage);

	//---------- END receiving
	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult();
	}

CEnhancedVoiceMailNotification* CSmsEnhancedVoiceMailTestStep::CreateEnhancedVoiceMailNotificationL(TGsmSmsTelNumber aAccessAddress)
{
	CEnhancedVoiceMailNotification* enhancedVoiceMailNotification=CEnhancedVoiceMailNotification::NewL();

	//Set the properties
	TSmsMessageProfileType messageProfileType=EGsmSmsProfileId1;
	enhancedVoiceMailNotification->SetProfile(messageProfileType);
	enhancedVoiceMailNotification->SetStorage(ETrue);
	enhancedVoiceMailNotification->SetAlmostMaximumCapacity(ETrue);
	enhancedVoiceMailNotification->SetMaximumCapacity(EFalse);
	enhancedVoiceMailNotification->SetNumberOfVoiceMessages(5);
	enhancedVoiceMailNotification->SetParsedAccessAddressL(aAccessAddress);

	return enhancedVoiceMailNotification;
}

CVoiceMailNotification*  CSmsEnhancedVoiceMailTestStep::CreateVoiceMailNotificationL(TGsmSmsTelNumber aCallingLineIdentity)
{
	CVoiceMailNotification* voiceMailNotification=CVoiceMailNotification::NewL();

	//Setup the notification
	TUint16 messageId=2;
	TUint8 messageLength=25;
	voiceMailNotification->SetMessageId(messageId);
	voiceMailNotification->SetMessageLength(messageLength);
	TUint8 daysRetention=3;
	voiceMailNotification->SetRetentionDays(daysRetention);
	voiceMailNotification->SetPriorityIndication(EFalse);
	voiceMailNotification->SetParsedCallingLineIdentityL(aCallingLineIdentity);

	return voiceMailNotification;
}


TVerdict CTestMultiplePDUEnhancedVoiceMailInformation::doTestStepL()
/**
 *  Test the sending and receiving of enhanced voice mail
 */
	{
    // TODO: redundant test case???
	INFO_PRINTF1(_L("Test enhanced voice mail information with multiple PDU message"));

	return TestStepResult();
	}

TVerdict CTestEnhancedVoiceMailDeleteOperations::doTestStepL()
/**
 *  Test the enhanced voice mail delete confirmation
 */
	{
	INFO_PRINTF1(_L("Test enhanced voice mail delete confirmation operations"));
	//This is character msg - 96 characters
	_LIT(KTestMsg,"This is a enhanced voice mail information message!");

	//8 bit coding scheme
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	//Get the operations
	CSmsEnhancedVoiceMailOperations& operations =
			static_cast<CSmsEnhancedVoiceMailOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));

	//Lets bash these API's, make sure they work correctly !
	INFO_PRINTF1(_L("Testing API's for enhanced voice mail delete confirmation operations"));
	//ContainsEnhancedVoiceMailL
	TBool result=operations.ContainsEnhancedVoiceMailIEL();
	TEST(!result);
	//RemoveEnhancedVoiceMailL
	TRAPD(err,operations.RemoveEnhancedVoiceMailIEL());
	TEST(err==KErrNotFound);
	//CopyEnhancedVoidMail
	TRAP(err,operations.CopyEnhancedVoiceMailIEL());
	TEST(err==KErrNotFound);

	//Create CEnhancedVoiceMailDeleteConfirmations
	CEnhancedVoiceMailDeleteConfirmations* enhancedVoiceMailDeleteConfirmation=
			CEnhancedVoiceMailDeleteConfirmations::NewL();
	CleanupStack::PushL(enhancedVoiceMailDeleteConfirmation);

	//Set the properties
	TSmsMessageProfileType messageProfileType=EGsmSmsProfileId1;
	enhancedVoiceMailDeleteConfirmation->SetProfile(messageProfileType);
	enhancedVoiceMailDeleteConfirmation->SetStorage(ETrue);
	enhancedVoiceMailDeleteConfirmation->SetAlmostMaximumCapacity(EFalse);
	enhancedVoiceMailDeleteConfirmation->SetMaximumCapacity(EFalse);
	enhancedVoiceMailDeleteConfirmation->SetNumberOfVoiceMessages(3);
	//Add access address - mail box address
	TGsmSmsTelNumber accessAddressTGsm;
	accessAddressTGsm.iTelNumber=_L("44758ABCD3");
	accessAddressTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
	enhancedVoiceMailDeleteConfirmation->SetParsedAccessAddressL(accessAddressTGsm);

	//Create mail messages
	CVoiceMailDeletion* voiceMailDeletion=CVoiceMailDeletion::NewL();
	//CleanupStack::PushL(voiceMailDeletion);

	//Setup the message
	TUint16 messageId=3;
	voiceMailDeletion->SetMessageId(messageId);
	TEST(voiceMailDeletion->MessageId() == messageId);
	TEST(voiceMailDeletion->MessageExtensionIndication() == EFalse);

	//Add the deletion to the enhanced message
	RPointerArray<CVoiceMailDeletion>& deletionsList=
					enhancedVoiceMailDeleteConfirmation->GetVoiceMailDeletions();
	result=deletionsList.Append(voiceMailDeletion);
	TEST(result==KErrNone);
	TUint count=enhancedVoiceMailDeleteConfirmation->NumberOfDeletes();
	TEST(count==1);
	//Add
	TRAP(err,operations.AddEnhancedVoiceMailIEL(*enhancedVoiceMailDeleteConfirmation));
	TEST(err==KErrNone);
	//Contains
	//ContainsEnhancedVoiceMailL
	result=operations.ContainsEnhancedVoiceMailIEL();
	TEST(result);
	//Add
	TRAP(err,operations.AddEnhancedVoiceMailIEL(*enhancedVoiceMailDeleteConfirmation));
	TEST(err==KErrAlreadyExists);

	//Copy
	CEnhancedVoiceMailDeleteConfirmations* retrievedDeletion = NULL;
	TRAP(err,retrievedDeletion=static_cast<CEnhancedVoiceMailDeleteConfirmations*>(operations.CopyEnhancedVoiceMailIEL() ));
	TEST(err==KErrNone);
	CleanupStack::PushL(retrievedDeletion);

	//Check the contents
	//Type
	TVoiceMailInfoType typeInfo=retrievedDeletion->Type();
	TEST(typeInfo==EGsmSmsVoiceMailDeleteConfirmation);
	//Profile
	TSmsMessageProfileType profile=retrievedDeletion->Profile();
	TEST(profile==EGsmSmsProfileId1);
	//Store
	TBool isStore=retrievedDeletion->Store();
	TEST(isStore);
	//Almost Capacity
	TBool isAlmostMaxCapacity=retrievedDeletion->AlmostMaximumCapacity();
	TEST(!isAlmostMaxCapacity);
	//MaximumCapacity
	TBool isMaximumCapacity=retrievedDeletion->MaximumCapacity();
	TEST(!isMaximumCapacity);
	//Access address - mail box address
	TGsmSmsTelNumber retrievedAccessAddressTGsm;
	retrievedDeletion->ParsedAccessAddress(retrievedAccessAddressTGsm);
	TEST(retrievedAccessAddressTGsm.iTelNumber==accessAddressTGsm.iTelNumber);
	//Number Of Voice Messages
	count=retrievedDeletion->NumberOfVoiceMessages();
	TEST(count==3);
	//Number of deletes
	count=retrievedDeletion->NumberOfDeletes();
	TEST(count==1);
	//contains
	result=operations.ContainsEnhancedVoiceMailIEL();
	TEST(result);
	//remove
	CEnhancedVoiceMailBoxInformation* enhancedVoiceMailBoxInformation = 0;
	TRAP(err,enhancedVoiceMailBoxInformation = operations.RemoveEnhancedVoiceMailIEL());
	TEST(err==KErrNone);
	delete enhancedVoiceMailBoxInformation;
	//contains
	result=operations.ContainsEnhancedVoiceMailIEL();
	TEST(!result);

	CleanupStack::PopAndDestroy(retrievedDeletion);
	CleanupStack::PopAndDestroy(enhancedVoiceMailDeleteConfirmation);
	CleanupStack::PopAndDestroy(smsMessage);
	return TestStepResult();
	}

TVerdict CTestSinglePDUEnhancedVoiceMailDeleteConfirmation::doTestStepL()
/**
 *  Test the enhanced voice mail delete confirmation
 */
	{
	INFO_PRINTF1(_L("Test enhanced voice mail single PDU message delete confirmation"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	//This is an enhanced voice mail character msg.
	_LIT(KTestMsg,"");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//8 bit coding scheme
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S "),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S "),&iServiceCenterNumber);

	//Get the operations
	CSmsEnhancedVoiceMailOperations& operations =
		static_cast<CSmsEnhancedVoiceMailOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));

	//ContainsEnhancedVoiceMailL
	TBool result=operations.ContainsEnhancedVoiceMailIEL();
	TEST(!result);

	//Send message with no special message indication
	SendSmsL(smsMessage,socket);

	//---- Add enhanced voice mail delete confirmation to the message
	//Create the delete confirmation
	CEnhancedVoiceMailDeleteConfirmations* enhancedVoiceMailDeleteConfirmation=
			CEnhancedVoiceMailDeleteConfirmations::NewL();
	CleanupStack::PushL(enhancedVoiceMailDeleteConfirmation);

	//Set the properties
	TSmsMessageProfileType messageProfileType=EGsmSmsProfileId2;
	enhancedVoiceMailDeleteConfirmation->SetProfile(messageProfileType);
	enhancedVoiceMailDeleteConfirmation->SetStorage(EFalse);
	enhancedVoiceMailDeleteConfirmation->SetAlmostMaximumCapacity(EFalse);
	enhancedVoiceMailDeleteConfirmation->SetMaximumCapacity(EFalse);
	enhancedVoiceMailDeleteConfirmation->SetNumberOfVoiceMessages(2);
	//Add access address - mail box address
	TGsmSmsTelNumber accessAddressTGsm;
	accessAddressTGsm.iTelNumber=_L("44758ABCD3");
	accessAddressTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
	enhancedVoiceMailDeleteConfirmation->SetParsedAccessAddressL(accessAddressTGsm);

	//Create mail messages
	CVoiceMailDeletion* voiceMailDeletion=CVoiceMailDeletion::NewL();
	//Setup the message
	TUint16 messageId=5;
	voiceMailDeletion->SetMessageId(messageId);

	//Add the deletion to the enhanced message
	RPointerArray<CVoiceMailDeletion>& deletionsList=
					enhancedVoiceMailDeleteConfirmation->GetVoiceMailDeletions();
	result=deletionsList.Append(voiceMailDeletion);
	TEST(result==KErrNone);
	TUint count=enhancedVoiceMailDeleteConfirmation->NumberOfDeletes();
	TEST(count==1);

	//Add to message
	TRAPD(err,operations.AddEnhancedVoiceMailIEL(*enhancedVoiceMailDeleteConfirmation));
	TEST(err==KErrNone);
	//Send the message
	SendSmsL(smsMessage,socket);

	CleanupStack::PopAndDestroy(enhancedVoiceMailDeleteConfirmation);
	CleanupStack::PopAndDestroy(smsMessage);
	//---------- END sending

	//---------- BEGIN receiving
	//Receiving message with no enhanced voice mail
	INFO_PRINTF1(_L("waiting for incoming SMS with no enhanced voice mail delete confirmation...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsEnhancedVoiceMailOperations& noEnhancedVoiceMailOperations =
		static_cast<CSmsEnhancedVoiceMailOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));

	//ContainsEnhancedVoiceMailL
	result=noEnhancedVoiceMailOperations.ContainsEnhancedVoiceMailIEL();
	TEST(!result);

	CleanupStack::PopAndDestroy(smsMessage);

	//---
	//Receive message with enhanced voice mail, one delete confirmation
	INFO_PRINTF1(_L("waiting for incoming SMS with enhanced voice mail delete confirmation, one deletion...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Get the operations
	CSmsEnhancedVoiceMailOperations& enhancedVoiceMailOperations =
		static_cast<CSmsEnhancedVoiceMailOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));

	//ContainsEnhancedVoiceMailL
	result=enhancedVoiceMailOperations.ContainsEnhancedVoiceMailIEL();
	TEST(result);

	//Copy
	CEnhancedVoiceMailBoxInformation* retrievedNotification = NULL;
	TRAP(err,retrievedNotification=enhancedVoiceMailOperations.CopyEnhancedVoiceMailIEL() );
	TEST(err==KErrNone);
	CleanupStack::PushL(retrievedNotification);

	//Check the contents
	//Type
	TVoiceMailInfoType typeInfo=retrievedNotification->Type();
	TEST(typeInfo==EGsmSmsVoiceMailDeleteConfirmation);
	//Extension Indicator
	TEST(retrievedNotification->ExtensionIndicator() == EFalse);
	//Profile
	TSmsMessageProfileType profile=retrievedNotification->Profile();
	TEST(profile==EGsmSmsProfileId2);
	//Store
	TBool isStore=retrievedNotification->Store();
	TEST(!isStore);
	//Almost Capacity
	TBool isAlmostMaxCapacity=retrievedNotification->AlmostMaximumCapacity();
	TEST(!isAlmostMaxCapacity);
	//MaximumCapacity
	TBool isMaximumCapacity=retrievedNotification->MaximumCapacity();
	TEST(!isMaximumCapacity);
	//Access address - mail box address
	TGsmSmsTelNumber retrievedAccessAddressTGsm;
	retrievedNotification->ParsedAccessAddress(retrievedAccessAddressTGsm);
	TEST(retrievedAccessAddressTGsm.iTelNumber==accessAddressTGsm.iTelNumber);
	//Number Of Voice Messages
	count=retrievedNotification->NumberOfVoiceMessages();
	TEST(count==2);
	//Cast the type, get the numberOfVoiceMails
	//Number of voice mails
	CEnhancedVoiceMailDeleteConfirmations* retrievedEnhancedVoiceMailDeleteConfirmation=
						static_cast<CEnhancedVoiceMailDeleteConfirmations*>(retrievedNotification);
	count=retrievedEnhancedVoiceMailDeleteConfirmation->NumberOfDeletes();
	TEST(count==1);

	CleanupStack::PopAndDestroy(retrievedNotification);
	CleanupStack::PopAndDestroy(smsMessage);
	//---------- END receiving
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}


TVerdict CTestMultiplePDUEnhancedVoiceMailDeleteConfirmation::doTestStepL()
/**
 *  Test the enhanced voice mail delete confirmation
 */
	{
    // TODO: redundant test case???
	INFO_PRINTF1(_L("Test enhanced voice mail multiple PDU message delete confirmation"));

	return TestStepResult();
	}

TVerdict CTest7BitDefaultAlphabet::doTestStepL()
/**
 *  Test the enhanced voice mail delete confirmation
 */
	{
	INFO_PRINTF1(_L("Test 7 bit default alphabet"));

	//Send a message with no alphabet set
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	//This is a msg.
	_LIT(KTestMsg,"This message has no coding scheme set.  Assume 7 bit default.");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//Create message
	CSmsPDU::TSmsPDUType messageType = CSmsPDU::ESmsSubmit;
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, messageType, buffer);
	CleanupStack::PushL(smsMessage);

	smsMessage->SetToFromAddressL(iTelephoneNumber);
	smsMessage->SmsPDU().SetServiceCenterAddressL(iServiceCenterNumber);
	buffer->InsertL(0,KTestMsg);

	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);
	//Data coding scheme received by SIM TSY is correct 00000000 = 7 bit default alphabet

	//Receive message with no coding scheme specified
	//Basically receive the message that was sent
	//Receive message with enhanced voice mail, one delete confirmation
	INFO_PRINTF1(_L("waiting for incoming SMS with no coding scheme (DCS)...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Check the coding scheme
	TSmsUserDataSettings smsSettings;
	smsMessage->UserDataSettings(smsSettings);

	TSmsDataCodingScheme::TSmsAlphabet alphabet=smsSettings.Alphabet();
	TEST(alphabet==TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive a second message, this time a (U)SIM specific message
	INFO_PRINTF1(_L("waiting for incoming SMS with no coding scheme and type USIM message...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Check the coding scheme
	smsMessage->UserDataSettings(smsSettings);
	alphabet=smsSettings.Alphabet();
	TEST(alphabet==TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PopAndDestroy(smsMessage);

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

TVerdict CTestZeroLengthIE::doTestStepL()
/**
 *  Test the enhanced voice mail delete confirmation
 */
	{
	INFO_PRINTF1(_L("Test zero length IE, USIM toolkit IE"));

	//Send a message with no alphabet set
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg,"This message has a zero length information element.  USIM toolkit security header.");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//Create message
	//8 bit coding scheme
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	INFO_PRINTF2(_L("Destination number:..... %S "),&iTelephoneNumber);
	INFO_PRINTF2(_L("ServiceCenter number:... %S "),&iServiceCenterNumber);

	//Add zero length IE
	//Get the PDU
	CSmsPDU &pdu=smsMessage->SmsPDU();

	//Get the UserData
	CSmsUserData &userData=pdu.UserData();

	//Add the information element
	_LIT8(KEmptyString,"");
	//TSmsId
	CSmsInformationElement::TSmsInformationElementIdentifier informationElement=CSmsInformationElement::ESmsIEISIMToolkitSecurityHeaders1;
	TRAPD(err,userData.AddInformationElementL(informationElement,KEmptyString()));
	TEST(err==KErrNone);

	TRAP(err,userData.AddInformationElementL(informationElement,KEmptyString()));
	TEST(err==KErrAlreadyExists);

	//Send the message
	SendSmsL(smsMessage,socket);

	CleanupStack::PopAndDestroy(smsMessage);
	//---------- END sending

	//----------
	//Receive message with USIM security header IE, 7F, with zero length
	INFO_PRINTF1(_L("waiting for incoming SMS with USIM security header IE, 7F, with zero length...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Interrogate PDU for contents
	CSmsPDU &receivedPdu=smsMessage->SmsPDU();
	//Get UserData
	CSmsUserData &receivedUserData=receivedPdu.UserData();
	TInt count=receivedUserData.NumInformationElements();
	TEST(count==1);

	TInt index=0;
	CSmsInformationElement &receivedPDUInformationElement=receivedUserData.InformationElement(index);
	informationElement=receivedPDUInformationElement.Identifier();
	TEST(informationElement==CSmsInformationElement::ESmsIEISIMToolkitSecurityHeaders16);
	CleanupStack::PopAndDestroy(smsMessage);

	//-----------
	//Receive message with multiple USIM security headers - 7F and 70, with zero length
	INFO_PRINTF1(_L("Receive SMS with multiple USIM security headers, 7F and 70, with zero length...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Interrogate
	CSmsPDU &receivedPduMultipleIE=smsMessage->SmsPDU();
	//Get UserData
	CSmsUserData &receivedUserDataMultipleIE=receivedPduMultipleIE.UserData();
	count=receivedUserDataMultipleIE.NumInformationElements();
	TEST(count==2);

	CleanupStack::PopAndDestroy(smsMessage);

	//-----------
	//Receive message with multiple USIM security headers - 7F and 7F, with zero length
	//This is a duplicate information element
	INFO_PRINTF1(_L("Receive SMS with multiple USIM security headers, 7F and 7F, with zero length...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);

	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Interrogate
	CSmsPDU &receivedPduMultipleDuplicateIE=smsMessage->SmsPDU();
	//Get UserData
	CSmsUserData &receivedUserDataDuplicateIE=receivedPduMultipleDuplicateIE.UserData();
	count=receivedUserDataDuplicateIE.NumInformationElements();
	TEST(count==2);

	CleanupStack::PopAndDestroy(smsMessage);
	//End receiving

	//-----Send multiple PDU message with one zero length IE =  0x70
	 _LIT(KLongMessage,"We might as well use our SMS stack in these phones.  It is by far the most robust, and it performs well under stress.  We have built a stack that is worth being inside a phone, processing messages for the wireless communnity :)");
	CSmsMessage *longSmsMessage=CreateSmsMessageLC(KLongMessage,alphabet);

	//Get the PDU
	CSmsPDU &multiplePdu=longSmsMessage->SmsPDU();

	//Get the UserData
	CSmsUserData &userDataMultiplePdu=multiplePdu.UserData();

	//Add IE
	informationElement=CSmsInformationElement::ESmsIEISIMToolkitSecurityHeaders1;
	TRAP(err,userDataMultiplePdu.AddInformationElementL(informationElement,KEmptyString()));
	TEST(err==KErrNone);

	//Send the message
	SendSmsL(longSmsMessage,socket);
	CleanupStack::PopAndDestroy(longSmsMessage);

	//--------------Receiving
	//Receive multiple PDU with no zero length USIM security headers
	INFO_PRINTF1(_L("waiting for incoming multiple PDU SMS...") );
	WaitForRecvL(socket);
	longSmsMessage = RecvSmsL(socket);

	CleanupStack::PushL(longSmsMessage);
	TestSmsContentsL(longSmsMessage,KLongMessage);

	//Interrogate
	CSmsPDU &multiplePduMessage=longSmsMessage->SmsPDU();
	//Get UserData
	CSmsUserData &multiplePDUUserDataMessage=multiplePduMessage.UserData();
	count=multiplePDUUserDataMessage.NumInformationElements();
	TEST(count==1);

	index=0;
	CSmsInformationElement &multiplePDUInformationElement=multiplePDUUserDataMessage.InformationElement(index);
	informationElement=multiplePDUInformationElement.Identifier();
	TEST(informationElement==CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference);

	CleanupStack::PopAndDestroy(longSmsMessage);

	//--------------
	//Receive multiple PDU with IE 70 and zero length in each PDU
	INFO_PRINTF1(_L("waiting for incoming multiple PDU SMS with one USIM security header, 70 with zero length, in each PDU ...") );
	WaitForRecvL(socket);
	longSmsMessage = RecvSmsL(socket);

	CleanupStack::PushL(longSmsMessage);
	TestSmsContentsL(longSmsMessage,KLongMessage);

	//Interrogate
	CSmsPDU &receivedMultiplePduIE=longSmsMessage->SmsPDU();
	//Get UserData
	CSmsUserData &receivedMultiplePDUUserData=receivedMultiplePduIE.UserData();
	count=receivedMultiplePDUUserData.NumInformationElements();
	TEST(count==2);

	index=0;
	CSmsInformationElement &multiplePDUIE=receivedMultiplePDUUserData.InformationElement(index);
	informationElement=multiplePDUIE.Identifier();
	TEST(informationElement==CSmsInformationElement::ESmsIEISIMToolkitSecurityHeaders1);

	index=1;
	CSmsInformationElement &secondElementIE=receivedMultiplePDUUserData.InformationElement(index);
	informationElement=secondElementIE.Identifier();
	TEST(informationElement==CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference);

	CleanupStack::PopAndDestroy(longSmsMessage);

	//----------------------
	//Receive multiple PDU with IE USIM security header only in first pdu
	INFO_PRINTF1(_L("waiting for incoming multiple PDU SMS with one USIM security header, 70 with zero length, only in first PDU ...") );
	WaitForRecvL(socket);
	longSmsMessage = RecvSmsL(socket);

	CleanupStack::PushL(longSmsMessage);
	TestSmsContentsL(longSmsMessage,KLongMessage);

	//Interrogate
	CSmsPDU &multiplePduIEInFirst=longSmsMessage->SmsPDU();
	//Get UserData
	CSmsUserData &multiplePDUUserData=multiplePduIEInFirst.UserData();

	count=multiplePDUUserData.NumInformationElements();
	TEST(count==2);

	CSmsInformationElement &multiplePDUInformationElementInFirst=multiplePDUUserData.InformationElement(index);
	informationElement=multiplePDUInformationElementInFirst.Identifier();
	TEST(informationElement==CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference);

	//Check the second IE
	index=1;
	CSmsInformationElement &secondElement=multiplePDUUserData.InformationElement(index);
	informationElement=secondElement.Identifier();
	TEST(informationElement==CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference);

	CleanupStack::PopAndDestroy(longSmsMessage);

	//-----------------------
	//Receive multiple PDU with IE USIM security header in second PDU
	INFO_PRINTF1(_L("waiting for incoming multiple PDU SMS with one USIM security header, 70 with zero length, in second PDU ...") );
	WaitForRecvL(socket);
	longSmsMessage = RecvSmsL(socket);

	CleanupStack::PushL(longSmsMessage);
	TestSmsContentsL(longSmsMessage,KLongMessage);

	//Interrogate
	CSmsPDU &multiplePduIEInSecond=longSmsMessage->SmsPDU();
	//Get UserData
	CSmsUserData &multiplePDUUserDataWithIEInSecondPDU=multiplePduIEInSecond.UserData();

	count=multiplePDUUserDataWithIEInSecondPDU.NumInformationElements();
	TEST(count==2);

	CSmsInformationElement &multiplePDUInformationElementInSecondPDU=multiplePDUUserDataWithIEInSecondPDU.InformationElement(index);
	informationElement=multiplePDUInformationElementInSecondPDU.Identifier();
	TEST(informationElement==CSmsInformationElement::ESmsIEISIMToolkitSecurityHeaders6);

	//Check the second IE
	index=1;
	CSmsInformationElement &secondInformationElement=multiplePDUUserDataWithIEInSecondPDU.InformationElement(index);
	informationElement=secondInformationElement.Identifier();
	TEST(informationElement==CSmsInformationElement::ESmsIEISIMToolkitSecurityHeaders6);

	CleanupStack::PopAndDestroy(longSmsMessage);
	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

/*
	Class 2 Messages are stored on the SIM.  Automatic deletion in the DCS does not delete these messages
	from the SIM.  This is the default behaviour.  This behaviour can be changed and it is done in
	the esk file.  This is tested in test step CTestAutomaticDeletionInDCSAndInESK.
*/
TVerdict CTestAutomaticDeletionInDCS::doTestStepL()
/**
 *  Test the enhanced voice mail delete confirmation
 */
	{
	INFO_PRINTF1(_L("Test automatic deletion of messages marked as such in the code data segment(cds)"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg,"");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//8 bit coding scheme
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	//Set message to class 2
	TSmsDataCodingScheme::TSmsClass pduClass=TSmsDataCodingScheme::ESmsClass2;
	CSmsPDU& pdu = smsMessage->SmsPDU();
	pdu.SetClass(ETrue,pduClass);

	//Enumerate the Sim Store
	INFO_PRINTF1(_L("Enumerating messages"));

	TPckgBuf<TUint> sbuf;
	sbuf()=0;

	//Now enumerate messages from store
	TRequestStatus status;
	socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	//sbuf() includes the count of messages on Store
	TInt count = sbuf();
	INFO_PRINTF2(_L("%d enumerated messages"), count);
	TEST(count==0);

	//Send the message
	SendSmsL(smsMessage,socket);

	//Set bits 7-4 of DCS
	//Set message to auto delete
	TInt autoDeleteHexValue=0x40;
	pdu.SetBits7To4(TSmsDataCodingScheme::TSmsDCSBits7To4(autoDeleteHexValue));
	//Send this message
	SendSmsL(smsMessage,socket);
	CleanupStack::PopAndDestroy(smsMessage);
	//----------End Sending

	INFO_PRINTF1(_L("Receive a message of class 2 - message will be stored on the SIM..."));
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	//Get PDU
	CSmsPDU& classTwoPdu = smsMessage->SmsPDU();
	//Class
	//TSmsDataCodingScheme::TSmsClass pduClass;
	classTwoPdu.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass2);
	//Test the storage
	CSmsMessage::TMobileSmsStorage storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsSIMStorage);
	//Test the slots
	count=smsMessage->iSlotArray.Count();
	TEST(count==1);
	TGsmSmsSlotEntry smsSlot=smsMessage->iSlotArray[0];
	TEST(smsSlot.iStore.Compare(_L("S14"))==0);
	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("Receive a message of class 2 with automatic delete - message will be stored on the SIM..."));
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	//Get PDU
	CSmsPDU& pduClass2 = smsMessage->SmsPDU();
	//Class
	pduClass2.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass2);
	//Test the storage
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsSIMStorage);
	//Test the slots
	count=smsMessage->iSlotArray.Count();
	TEST(count==1);
	smsSlot=smsMessage->iSlotArray[0];
	TEST(smsSlot.iStore.Compare(_L("S14"))==0);
	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("Receive a message of class 1 with automatic delete...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	//Get PDU
	CSmsPDU& pduClass1 = smsMessage->SmsPDU();
	//Class
	pduClass1.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass1);
	
	//Test the storage
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsNoStorage);
	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("Receive a message of class 3 with automatic delete...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	//Get PDU
	CSmsPDU& pduClass3 = smsMessage->SmsPDU();
	//Class
	pduClass3.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass3);
	
	//Test the storage
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsNoStorage);
	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("Receive a message of class 0 with automatic delete...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	
	//Get PDU
	CSmsPDU& pduClass0 = smsMessage->SmsPDU();
	//Class
	pduClass0.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass0);
	
	//Test the storage
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsNoStorage);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive a message with no class with automatic delete
	INFO_PRINTF1(_L("waiting for incoming SMS with no class and with automatic delete...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsNoStorage);
	CleanupStack::PopAndDestroy(smsMessage);

	User::After(3000000);	//KIoctlEnumerateSmsMessages returns -14 (KErrInUse) if no wait.
	//Enuermate
	sbuf()=0;
	socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	//sbuf() includes the count of messages on Store
	count = sbuf();
	INFO_PRINTF2(_L("%d enumerated messages after receiving messages"), count);

	//These are the two class messages received.
	TEST(count==2);

	CleanupStack::PopAndDestroy(&socket);
	return TestStepResult();
	}

/*
	Class 2 Messages are stored on the SIM.  Automatic deletion in the DCS does not delete these messages
	from the SIM.  This is the default behaviour.  This behaviour can be changed and it is done in
	the esk file.  This tested in test step CTestAutomaticDeletionInDCSAndInESK.
*/
TVerdict CTestAutomaticDeletionInDCSAndInESK::doTestStepL()
/**
 *  Test the enhanced voice mail delete confirmation
 */
	{
	INFO_PRINTF1(_L("Test automatic deletion of messages marked in dcs, and set for delete in the ESK file"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg,"Esk file has been set to delete class 2 messages marked for automatic deletion");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//8 bit coding scheme - create message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	//Set message to class 2
	TSmsDataCodingScheme::TSmsClass pduClass=TSmsDataCodingScheme::ESmsClass2;
	CSmsPDU& pdu = smsMessage->SmsPDU();
	pdu.SetClass(ETrue,pduClass);

	//Enumerate the Sim Store
	INFO_PRINTF1(_L("Enumerating messages"));
	TPckgBuf<TUint> sbuf;
	sbuf()=0;
	TRequestStatus status;
	socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	//sbuf() includes the count of messages on Store
	TInt count = sbuf();
	INFO_PRINTF2(_L("%d enumerated messages"), count);
	TEST(count==0);

	//Set bits 7-4 of DCS
	//Set message to auto delete
	TInt autoDeleteHexValue=0x40;
	pdu.SetBits7To4(TSmsDataCodingScheme::TSmsDCSBits7To4(autoDeleteHexValue));
	//Send this message
	SendSmsL(smsMessage,socket);

	CleanupStack::PopAndDestroy(smsMessage);
	//----------End Sending

	//Receive a message with no class
	INFO_PRINTF1(_L("waiting for incoming SMS with no class...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	//Test the storage
	CSmsMessage::TMobileSmsStorage storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsNoStorage);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive a message of class 0
	INFO_PRINTF1(_L("waiting for incoming SMS class 0 with automatic delete...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	//Get PDU
	CSmsPDU& classZeroPdu = smsMessage->SmsPDU();
	classZeroPdu.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass0);
	//Test the storage
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsNoStorage);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive a message of class 1
	INFO_PRINTF1(_L("waiting for incoming SMS class 1 with automatic delete...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	//Get PDU
	CSmsPDU& classOnePdu = smsMessage->SmsPDU();
	classOnePdu.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass1);
	//Test the storage
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsNoStorage);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive a message of class 3
	INFO_PRINTF1(_L("waiting for incoming SMS class 3 with automatic delete...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	//Get PDU
	CSmsPDU& classThreePdu = smsMessage->SmsPDU();
	classThreePdu.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass3);
	//Test the storage
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsNoStorage);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive a message of class 2 - message will be deleted from SIM
	INFO_PRINTF1(_L("waiting for incoming SMS class 2 with automatic delete...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	//Get PDU
	CSmsPDU& classTwoPdu = smsMessage->SmsPDU();
	classTwoPdu.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass2);
	//Test the storage
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsNoStorage);
	CleanupStack::PopAndDestroy(smsMessage);

	User::After(3000000);
	//Enuermate
	sbuf()=0;
	socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	//sbuf() includes the count of messages on Store
	count = sbuf();
	INFO_PRINTF2(_L("%d enumerated messages after receiving messages"), count);
	TEST(count==0);

	CleanupStack::PopAndDestroy(&socket);
 	return TestStepResult();
	}


TVerdict CTestAutomaticDeletionTypeZeroSMS::doTestStepL()
/**
 *  Test the enhanced voice mail delete confirmation
 */
	{
	INFO_PRINTF1(_L("Test automatic deletion of messages of type zero"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg,"Type 0 Sms message");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//8 bit coding scheme - create message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	//Set message to type 0
	CSmsPDU& pdu = smsMessage->SmsPDU();
	//Set the PID type
	TSmsProtocolIdentifier::TSmsPIDType pIdType=TSmsProtocolIdentifier::ESmsPIDShortMessageType;
	pdu.SetPIDType(pIdType);
	TSmsProtocolIdentifier::TSmsShortMessageType shortMessageType=TSmsProtocolIdentifier::ESmsShortMessageType0;
	pdu.SetShortMessageType(shortMessageType);

	//Send this message
	SendSmsL(smsMessage,socket);

	CleanupStack::PopAndDestroy(smsMessage);
	//----------End Sending

	//----------Start Receiving
	//Receive a class 0 type 0 message
	INFO_PRINTF1(_L("waiting for incoming SMS, type 0 class 0...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Test PID and class
	CSmsPDU& classZeroPdu = smsMessage->SmsPDU();
	//Get the PID type
	pIdType=classZeroPdu.PIDType();
	TEST(pIdType==TSmsProtocolIdentifier::ESmsPIDShortMessageType);
	shortMessageType=classZeroPdu.ShortMessageType();
	TEST(shortMessageType==TSmsProtocolIdentifier::ESmsShortMessageType0);
	//Class
	TSmsDataCodingScheme::TSmsClass pduClass;
	classZeroPdu.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass0);
	//Test the storage
	CSmsMessage::TMobileSmsStorage storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsNoStorage);
	//Test the slots
	TUint count=smsMessage->iSlotArray.Count();
	TEST(count==0);
	CleanupStack::PopAndDestroy(smsMessage);


	//All type 0 messages will be deleted an not given to the client.  The only exception is for class 0, and class 2
	// messages.  These will be given to the client.
	//Sim TSY is setup to receive multiple type 0 messages, the last two are class 0 class 2.

	//Receive a class 2, type 0 message
	INFO_PRINTF1(_L("waiting for incoming SMS, type 0 class 2...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	//Interrorgate this message to verify it is the one we are expecting
	TestSmsContentsL(smsMessage,KTestMsg);
	//Test PID and class
	CSmsPDU& classTwoPdu = smsMessage->SmsPDU();
	pIdType=classTwoPdu.PIDType();
	TEST(pIdType==TSmsProtocolIdentifier::ESmsPIDShortMessageType);
	shortMessageType=classTwoPdu.ShortMessageType();
	TEST(shortMessageType==TSmsProtocolIdentifier::ESmsShortMessageType0);
	//Class
	classTwoPdu.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass2);
	//Test the storage
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsSIMStorage);
	//Test the slots
	count=smsMessage->iSlotArray.Count();
	TEST(count==1);
	TGsmSmsSlotEntry smsSlot=smsMessage->iSlotArray[0];
	TEST(smsSlot.iIndex==1);
	TEST(smsSlot.iStore.Compare(_L("S14"))==0);
	CleanupStack::PopAndDestroy(smsMessage);


	//Receive a class 0 message which is NOT Type 0
	INFO_PRINTF1(_L("waiting for incoming SMS, class 0 but NOT type 0...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);

	//Interrorgate this message to verify it is the one we are expecting
	TestSmsContentsL(smsMessage,KTestMsg);
	//Test PID and class
	CSmsPDU& classZeroNotTypeZeroPdu = smsMessage->SmsPDU();
	pIdType=classZeroNotTypeZeroPdu.PIDType();
	TEST(pIdType==0x00); // PID defining a Short Message Type

	//Class
	classZeroNotTypeZeroPdu.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass0);
	//Test the storage
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsSIMStorage);
	count=smsMessage->iSlotArray.Count();
	TEST(count==1);
	smsSlot=smsMessage->iSlotArray[0];
	TEST(smsSlot.iIndex==2);
	TEST(smsSlot.iStore.Compare(_L("S14"))==0);
	CleanupStack::PopAndDestroy(smsMessage);

	//Enumerate the Sim Store
	INFO_PRINTF1(_L("Enumerating messages"));
	TPckgBuf<TUint> sbuf;
	sbuf()=0;
	TRequestStatus status;
	socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	count = sbuf();
	INFO_PRINTF2(_L("%d enumerated messages"), count);
	TEST(count==2);

	CleanupStack::PopAndDestroy(&socket);
  	return TestStepResult();
	}

TVerdict CTestAutomaticDeletionTypeZeroAndSetInESK::doTestStepL()
/**
 *  Test the enhanced voice mail delete confirmation
 */
	{
	INFO_PRINTF1(_L("Test automatic deletion of messages of type zero, and set for deletion for Class 0 in ESK file."));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	_LIT(KTestMsg,"Type 0 Sms message");

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	//8 bit coding scheme - create message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg,alphabet);

	//Set message to type 0
	CSmsPDU& pdu = smsMessage->SmsPDU();
	//Set the PID type
	TSmsProtocolIdentifier::TSmsPIDType pIdType=TSmsProtocolIdentifier::ESmsPIDShortMessageType;
	pdu.SetPIDType(pIdType);
	TSmsProtocolIdentifier::TSmsShortMessageType shortMessageType=TSmsProtocolIdentifier::ESmsShortMessageType0;
	pdu.SetShortMessageType(shortMessageType);

	//Send this message
	SendSmsL(smsMessage,socket);

	CleanupStack::PopAndDestroy(smsMessage);
	//----------End Sending

	//----------Start Receiving
	//All type 0 messages will be deleted an not given to the client.
	//Sim TSY is setup to receive multiple type 0 messages
	//
	//No type 0 messages will be sent to the client
	//Two type 0 messages will be sent, one before the type 0s, and one after.  This is just to confirm
	//that no messages are being received during these two.

	//Receive class two message
	INFO_PRINTF1(_L("waiting for incoming SMS, class 2...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Test class and storage
	CSmsPDU& classTwoPdu = smsMessage->SmsPDU();
	//Class
	TSmsDataCodingScheme::TSmsClass pduClass;
	classTwoPdu.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass2);
	//Test the storage
	CSmsMessage::TMobileSmsStorage storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsSIMStorage);
	//Test the slots
	TUint count=smsMessage->iSlotArray.Count();
	TEST(count==1);
	TGsmSmsSlotEntry smsSlot=smsMessage->iSlotArray[0];
	TEST(smsSlot.iIndex==1);
	TEST(smsSlot.iStore.Compare(_L("S14"))==0);
	CleanupStack::PopAndDestroy(smsMessage);

	//Receive class one message
	INFO_PRINTF1(_L("waiting for incoming SMS, class 1...") );
	WaitForRecvL(socket);
	smsMessage = RecvSmsL(socket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,KTestMsg);

	//Test class and storage
	CSmsPDU& classOnePdu = smsMessage->SmsPDU();
	//Class
	classOnePdu.Class(pduClass);
	TEST(pduClass==TSmsDataCodingScheme::ESmsClass1);
	//Test the storage
	storage=smsMessage->Storage();
	TEST(storage==CSmsMessage::ESmsSIMStorage);
	//Test the slots
	count=smsMessage->iSlotArray.Count();
	TEST(count==1);
	smsSlot=smsMessage->iSlotArray[0];
	TEST(smsSlot.iIndex==2);
	TEST(smsSlot.iStore.Compare(_L("S14"))==0);
	CleanupStack::PopAndDestroy(smsMessage);

	//Enumerate the Sim Store
	INFO_PRINTF1(_L("Enumerating messages"));
	TPckgBuf<TUint> sbuf;
	sbuf()=0;
	TRequestStatus status;
	socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	count = sbuf();
	INFO_PRINTF2(_L("%d enumerated messages"), count);
	//This is the class 2 message and the class 1 message with PID=00, i.e. Not type 0
	TEST(count==2);

	CleanupStack::PopAndDestroy(&socket);
  	return TestStepResult();
	}

TVerdict CTestOOMSendR6Sms::doTestStepL()
/**
 *  Intent:
 *  
 *  The intent of the test is to check that the stack can recover from every potential
 *  memory failure that can occur whilst the stack is trying to send a message.
 *  
 *  Test description:
 *  
 *  1) Create a SMS Message containing a hyperlink, special message waiting indication,
 *  reply address and enhanced voice mail notification.
 *  2) Configure the ESockServer so that request for memory from the heap fails after
 *  'i' number of times.
 *  3) Send the message and check that the stack recovers successfully from the out of memory
 *  failure.
 *  4) Repeat the test iteratively, incrementing the counter "i" each time until the message is sent
 *  successfully.
 */
	{
	INFO_PRINTF1(_L("Test out of memory handling"));
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	//Set destination and SC numbers
	iTelephoneNumber=KPekka;
	iServiceCenterNumber=KRadiolinjaSC;

	_LIT(KTestMsg1,"test message, 8bits, length 30");

	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	//Get the operations
	CSmsHyperLinkOperations& hyperLinkOperations = static_cast<CSmsHyperLinkOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsHyperLinkFormat));

	TUint count=hyperLinkOperations.NumberOfHyperLinksL();
	TEST(count==0);

	const TUint KFirstHyperLinkPosition=20;
	TUint8 firstHyperLinkLabelLength=8;
	TUint8 firstHyperLinkURLLength=22;

	//Add hyperlinks
	hyperLinkOperations.AddHyperLinkL(KFirstHyperLinkPosition,firstHyperLinkLabelLength, firstHyperLinkURLLength);
	count=hyperLinkOperations.NumberOfHyperLinksL();
	TEST(count==1);

	//Get the operations
	CSmsSpecialSMSMessageOperations& specialMsgOperations = static_cast<CSmsSpecialSMSMessageOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication));

	//Send message with special message indication - voice mail
	INFO_PRINTF1(_L("Creating message with voice mail waiting"));
	TSmsMessageIndicationType messageIndicationType=EGsmSmsVoiceMessageWaiting;
	TExtendedSmsIndicationType extendedType=EGsmSmsNoExtendedMessageTypeIndication;
	TSmsMessageProfileType messageProfileType=EGsmSmsProfileId1;
	//TUint index=0;
	TBool toStore=ETrue;
	TUint8 messageCount=5;

	//Add special messages
	TRAPD(err,specialMsgOperations.AddSpecialMessageIndicationL(toStore,messageIndicationType,
								   extendedType,messageProfileType,messageCount));
	TEST(err==KErrNone);

	//Get the operations
	CSmsReplyAddressOperations& replyAddressOperations = static_cast<CSmsReplyAddressOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsReplyAddressFormat));
	TPtrC replyAddress;
	GetStringFromConfig(ConfigSection(), _L("replyAddressInternational"), replyAddress);
	TRAP(err,replyAddressOperations.AddReplyAddressL(replyAddress));
	TEST(err==KErrNone);


	//Get the operations
	CSmsEnhancedVoiceMailOperations& enhanceVoiceMailOperations =
			static_cast<CSmsEnhancedVoiceMailOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation));

	//Create CEnhancedVoiceMailNotification
	CEnhancedVoiceMailNotification* enhancedVoiceMailNotification=CEnhancedVoiceMailNotification::NewL();
	CleanupStack::PushL(enhancedVoiceMailNotification);

	//Set the properties
	messageProfileType=EGsmSmsProfileId1;
	enhancedVoiceMailNotification->SetProfile(messageProfileType);
	enhancedVoiceMailNotification->SetStorage(ETrue);
	enhancedVoiceMailNotification->SetAlmostMaximumCapacity(ETrue);
	enhancedVoiceMailNotification->SetMaximumCapacity(EFalse);
	enhancedVoiceMailNotification->SetNumberOfVoiceMessages(10);

	//Add access address - mail box address
	TGsmSmsTelNumber accessAddressTGsm;
	accessAddressTGsm.iTelNumber=_L("44758ABCD3");
	accessAddressTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
	enhancedVoiceMailNotification->SetParsedAccessAddressL(accessAddressTGsm);

	//Create mail messages
	CVoiceMailNotification* voiceMailNotification=CVoiceMailNotification::NewL();

	//Setup the message
	TUint16 messageId=1;
	TUint8 messageLength=20;
	voiceMailNotification->SetMessageId(messageId);
	TEST(messageId == voiceMailNotification->MessageId());
	voiceMailNotification->SetMessageLength(messageLength);
	TEST(messageLength == voiceMailNotification->MessageLength());
	TUint8 daysRetention=5;
	voiceMailNotification->SetRetentionDays(daysRetention);
	TEST(daysRetention == voiceMailNotification->RetentionDays());
	voiceMailNotification->SetPriorityIndication(ETrue);
    TEST(voiceMailNotification->PriorityIndication() != EFalse);
    TEST(voiceMailNotification->MessageExtensionIndication() == EFalse);

	//Create Calling Line Identity
    _LIT16(KCaller, "+447747065405");
    TBuf<14> buff(KCaller);
    voiceMailNotification->SetCallingLineIdentityL(buff);
    TEST(voiceMailNotification->CallingLineIdentity().Compare(_L("+447747065405")) == 0);

	TGsmSmsTelNumber callingLineIdentityTGsm;
	callingLineIdentityTGsm.iTelNumber=_L("44999DCBA8");
	callingLineIdentityTGsm.iTypeOfAddress=EGsmSmsTONAlphaNumeric;
	voiceMailNotification->SetParsedCallingLineIdentityL(callingLineIdentityTGsm);
	TGsmSmsTelNumber verifiedCallingLineIdentityTGsm;
	voiceMailNotification->ParsedCallingLineIdentity(verifiedCallingLineIdentityTGsm);
	TEST(verifiedCallingLineIdentityTGsm.iTypeOfAddress == EGsmSmsTONAlphaNumeric);
	TEST(verifiedCallingLineIdentityTGsm.iTelNumber.Compare(_L("44999DCBA8")) == 0);

	//Add this to the list of notifications in the enhanced message
	RPointerArray<CVoiceMailNotification>& notificationsList=
					enhancedVoiceMailNotification->GetVoiceMailNotifications();

	TBool result=notificationsList.Append(voiceMailNotification);

	TRAP(err,enhanceVoiceMailOperations.AddEnhancedVoiceMailIEL(*enhancedVoiceMailNotification));
	TEST(err==KErrNone);
	CleanupStack::PopAndDestroy(enhancedVoiceMailNotification);

	INFO_PRINTF1(_L("Testing status report schemes..."));
	INFO_PRINTF1(_L("SMSC Scheme"));

	//SMSC Scheme
	CSmsSMSCCtrlParameterOperations& smscOperations = static_cast<CSmsSMSCCtrlParameterOperations&>(smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters));
	smscOperations.SetSchemeL();

	count=0;
	TInt ret=KErrNoMemory;
	TInt successfullSends=0;  //maximum expected number of succeeded sends is 2 in TSY config file
	while (successfullSends<8)
		{
		iSocketServer.__DbgFailNext(count);		
		iSocketServer.__DbgMarkHeap();
		TRAP(ret,SendSmsDontCheckReturnValueL(smsMessage,socket));
		iSocketServer.__DbgMarkEnd(0);
		if(ret==KErrNone)
		    {
			++successfullSends;
		    }
		++count;
		}

	TEST(ret == KErrNone);
	iSocketServer.__DbgFailNext(-1); // Reset heap

	INFO_PRINTF1(_L("TPSRR Scheme"));
	
	// TPSRR Scheme
	CSmsTPSRROperations& TPSRROperations = static_cast<CSmsTPSRROperations&>(smsMessage->GetOperationsForNonIEL(ESmsTPSRRParameter));
	TPSRROperations.SetSchemeL();
	TPSRROperations.SetLastSegmentStatusReportL(ETrue);

	count=0;
	ret=KErrNoMemory;
	successfullSends=0;  //maximum expected number of succeeded sends is 2 in TSY config file
	while (successfullSends<12)
		{
		iSocketServer.__DbgFailNext(count);
		
		iSocketServer.__DbgMarkHeap();
		TRAP(ret,SendSmsDontCheckReturnValueL(smsMessage,socket));
		iSocketServer.__DbgMarkEnd(0);
		if(ret==KErrNone)
		    {
            ++successfullSends;		
		    }
		++count;
		}

	TEST(ret == KErrNone);
	iSocketServer.__DbgFailNext(-1); // Reset heap

	CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
	return TestStepResult() ;
	}

TVerdict CTestEnhancedVoiceMessageBoundary::doTestStepL()
{
	/**
	 *  Create a Enhanced Voice Mail Information Element that occupies 139 bytes.
	 *  139 bytes is the maximum that fit into a single segment message.
	 */
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	CEnhancedVoiceMailNotification* enhancedVoiceMailNotification = CEnhancedVoiceMailNotification::NewL();

	// Set up base class
	enhancedVoiceMailNotification->SetProfile(EGsmSmsProfileId3);
	enhancedVoiceMailNotification->SetStorage(ETrue);
 	enhancedVoiceMailNotification->SetAlmostMaximumCapacity(ETrue);
	enhancedVoiceMailNotification->SetMaximumCapacity(ETrue);

	_LIT(KPhoneNumber, "+44774706"); // Sets up test for max length information element, 139 bytes.

	enhancedVoiceMailNotification->SetAccessAddressL(KPhoneNumber);
	enhancedVoiceMailNotification->SetNumberOfVoiceMessages(0xFF);

	RPointerArray<CVoiceMailNotification>& voiceMailNotificationArray = enhancedVoiceMailNotification->GetVoiceMailNotifications();
	for (TUint i = 0; i < 8; ++i)
		{
		// Set up notification 1
		CVoiceMailNotification* voiceMailNotification = CVoiceMailNotification::NewL();

		voiceMailNotification->SetMessageId(0xFFFF);
		voiceMailNotification->SetMessageLength(0xAA);
    	voiceMailNotification->SetRetentionDays(31);
    	voiceMailNotification->SetPriorityIndication(ETrue);

    	TGsmSmsTelNumber gsmSmsTelNumber;
    	gsmSmsTelNumber.iTypeOfAddress.SetTON(EGsmSmsTONAlphaNumeric);
    	_LIT(KReplyAddress,"SONNERA1234");
    	gsmSmsTelNumber.iTelNumber = KReplyAddress;
    	voiceMailNotification->SetParsedCallingLineIdentityL(gsmSmsTelNumber);

    	voiceMailNotificationArray.Append(voiceMailNotification);
    	}

	_LIT(KTestMsg1,"");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	TSmsUserDataSettings userDataSettings;
	userDataSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
	smsMessage->SetUserDataSettingsL(userDataSettings);

	CSmsEnhancedVoiceMailOperations& enhancedVoiceMailInformation = (CSmsEnhancedVoiceMailOperations&) smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation);
	enhancedVoiceMailInformation.AddEnhancedVoiceMailIEL(*enhancedVoiceMailNotification);
	delete enhancedVoiceMailNotification;

	//Send the message
	SendSmsL(smsMessage,socket);

    CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
  	return TestStepResult();
}

TVerdict CTestEnhancedVoiceMessageBoundary1::doTestStepL()
{
	/*Creates the maximum size enhanced voice message ie that will fit
	* into a concatenated message, size 134 bytes, allowing 5 bytes for
	* a concatenation ie in the PDU.
	*/
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(iSocketServer,socket,ESmsAddrRecvAny);

	CEnhancedVoiceMailNotification* enhancedVoiceMailNotification = CEnhancedVoiceMailNotification::NewL();

	// Set up base class
	enhancedVoiceMailNotification->SetProfile(EGsmSmsProfileId3);
	enhancedVoiceMailNotification->SetStorage(ETrue);
	enhancedVoiceMailNotification->SetAlmostMaximumCapacity(ETrue);
	enhancedVoiceMailNotification->SetMaximumCapacity(ETrue);

	_LIT(KPhoneNumber, "+447747");

	enhancedVoiceMailNotification->SetAccessAddressL(KPhoneNumber);
	enhancedVoiceMailNotification->SetNumberOfVoiceMessages(0xFF);

	RPointerArray<CVoiceMailNotification>& voiceMailNotificationArray = enhancedVoiceMailNotification->GetVoiceMailNotifications();
	for (TUint i = 0; i < 8; ++i)
	{
		// Set up notification 1
		CVoiceMailNotification* voiceMailNotification = CVoiceMailNotification::NewL();

		voiceMailNotification->SetMessageId(0xFFFF);
		voiceMailNotification->SetMessageLength(0xAA);
		voiceMailNotification->SetRetentionDays(31);
		voiceMailNotification->SetPriorityIndication(ETrue);

		TGsmSmsTelNumber gsmSmsTelNumber;
		gsmSmsTelNumber.iTypeOfAddress.SetTON(EGsmSmsTONAlphaNumeric);
		_LIT(KReplyAddress,"SONNERA1234");  // Test limit 11 characters 10 bytes can hold 11 7 bit characters
		_LIT(KReplyAddress_8th,"SONNER");   // reduce the size the final address so that the IE has a length of 134 bytes.
		if (i == 7)
			{
			gsmSmsTelNumber.iTelNumber = KReplyAddress_8th;
			}
		else
			{
			gsmSmsTelNumber.iTelNumber = KReplyAddress;
			}

			voiceMailNotification->SetParsedCallingLineIdentityL(gsmSmsTelNumber);

			voiceMailNotificationArray.Append(voiceMailNotification);
	}

	_LIT(KTestMsg1,"Dundee United");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* smsMessage=CreateSmsMessageLC(KTestMsg1,alphabet);

	TSmsUserDataSettings userDataSettings;
	userDataSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
	smsMessage->SetUserDataSettingsL(userDataSettings);

	CSmsEnhancedVoiceMailOperations& enhancedVoiceMailInformation = (CSmsEnhancedVoiceMailOperations&) smsMessage->GetOperationsForIEL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation);
	enhancedVoiceMailInformation.AddEnhancedVoiceMailIEL(*enhancedVoiceMailNotification);
	delete enhancedVoiceMailNotification;

	//Send the message
	SendSmsL(smsMessage,socket);

    CleanupStack::PopAndDestroy(2, &socket); // socket, smsMessage
 	return TestStepResult();
}

TVerdict CTestSubmitReportDefaultsTo7BitDecoding::doTestStepL()
{
	TBuf8<30> buff;
	buff.SetLength(30);
	buff[0]  =   1;
	buff[1]  =   5; // Parameter Id
	buff[2]  =   0; // Time Stamp 1
	buff[3]  =  16; // Time Stamp 2
	buff[4]  =  16; // Time Stamp 3
	buff[5]  =   0; // Time Stamp 4
	buff[6]  =   0; // Time Stamp 5
	buff[7]  =   0; // Time Stamp 6
	buff[8]  =   0; // Time Stamp 7
	buff[9]  =   0; // PID
	buff[10] =  11; // User data length
	buff[11] = 196; // David Narey
	buff[12] = 176; //
	buff[13] =  61; //
	buff[14] =  77; //
	buff[15] =   6; //
	buff[16] =  57; //
	buff[17] = 195; //
	buff[18] = 242; //
	buff[19] = 114; //
	buff[20] =  30; //

	TGsmSms sms;
	sms.SetPdu(buff);

    RFs fs;
    User::LeaveIfError(fs.Connect());
	CSmsBuffer* receiveBuffer = CSmsBuffer::NewL();
	CSmsMessage* decodedMessage = CSmsMessage::NewL(fs, sms, receiveBuffer, EFalse, ETrue);
    CleanupStack::PushL(decodedMessage);
	CSmsBuffer& aBuffer = (CSmsBuffer&) decodedMessage->Buffer();

    TBuf16<12> name;
    name.SetLength(12);
	aBuffer.Extract(name, 0, 11);

    // Verify that the message has been decoded with the DCS defaulted to 7 Bit
	_LIT(KTestMsg,"David Narey");
	TestSmsContentsL(decodedMessage,KTestMsg);

	CleanupStack::PopAndDestroy(decodedMessage);
    return TestStepResult();
}

TVerdict CTestDeliverReportDefaultsTo7BitDecoding::doTestStepL()
{
	TBuf8<30> buff;
	buff.SetLength(30);
	buff[0]  =   0; // change from a 1
	buff[1]  =   5; // Parameter Id
	buff[2]  =   0; // PID
	buff[3]  =  11; // User data length
	buff[4]  = 196; // David Narey
	buff[5]  = 176; //
	buff[6]  =  61; //
	buff[7]  =  77; //
	buff[8]  =   6; //
	buff[9]  =  57; //
	buff[10] = 195; //
	buff[11] = 242; //
	buff[12] = 114; //
	buff[13] =  30; //

	TGsmSms sms;
	sms.SetPdu(buff);

    RFs fs;
    User::LeaveIfError(fs.Connect());
	CSmsBuffer* receiveBuffer = CSmsBuffer::NewL();
	CSmsMessage* decodedMessage = CSmsMessage::NewL(fs, sms, receiveBuffer, EFalse, EFalse);
    CleanupStack::PushL(decodedMessage);
	CSmsBuffer& aBuffer = (CSmsBuffer&) decodedMessage->Buffer();

    TBuf16<12> name;
    name.SetLength(12);
	aBuffer.Extract(name, 0, 11);

    // Verify that the message has been decoded with the DCS defaulted to 7 Bit
	_LIT(KTestMsg,"David Narey");
	TestSmsContentsL(decodedMessage,KTestMsg);

	CleanupStack::PopAndDestroy(decodedMessage);
    return TestStepResult();
}

TVerdict CTestStatusReportDefaultsTo7BitDecoding::doTestStepL()
{
	TBuf8<40> buff;
	buff.SetLength(40);

	buff[0]  = 6;
	buff[1]  = 0;
	buff[2]  = 0;
	buff[3]  = 129;
	buff[4]  = 0;
	buff[5]  = 16;
	buff[6]  = 16;
	buff[7]  = 0;
	buff[8]  = 0;
	buff[9]  = 0;
	buff[10]  = 0;
	buff[11]  = 0;
	buff[12]  = 16;
	buff[13]  = 16;
	buff[14]  = 0;
	buff[15]  = 0;
	buff[16]  = 0;
	buff[17]  = 0;
	buff[18]  = 0;
	buff[19]  = 5; // Parameter Indicator
	buff[20]  = 0;
	buff[21]  = 11; // User data length
	buff[22]  = 196;// David Narey
	buff[23]  = 176;
	buff[24]  = 61;
	buff[25]  = 77;
	buff[26]  = 6;
	buff[27]  = 57;
	buff[28]  = 195;
	buff[29]  = 242;
	buff[30]  = 114;
	buff[31]  = 30;

	TGsmSms sms;
	sms.SetPdu(buff);

    RFs fs;
    User::LeaveIfError(fs.Connect());
	CSmsBuffer* receiveBuffer = CSmsBuffer::NewL();
	CSmsMessage* decodedMessage = CSmsMessage::NewL(fs, sms, receiveBuffer, EFalse, ETrue);
    CleanupStack::PushL(decodedMessage);
	CSmsBuffer& aBuffer = (CSmsBuffer&) decodedMessage->Buffer();

    TBuf16<12> name;
    name.SetLength(12);
	aBuffer.Extract(name, 0, 11);

    // Verify that the message has been decoded with the DCS defaulted to 7 Bit
	_LIT(KTestMsg,"David Narey");
	TestSmsContentsL(decodedMessage,KTestMsg);

	CleanupStack::PopAndDestroy(decodedMessage);
    return TestStepResult();
}
