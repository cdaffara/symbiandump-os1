// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <emsformatie.h>
#include <emssoundie.h>
#include <emspictureie.h>
#include <emsanimationie.h>
#include <emsuserpromptie.h>
#include <emsobjectdistributionie.h>
#include <emsformatie.h>
#include "emstestutils.h"
#include <es_wsms.h>
#include <sacls.h>
#include <e32math.h>
#include <testconfigfileparser.h>

#include "TE_smsinterbase.h"
#include "TE_smsinter.h"

#include <gsmubuf.h>
#include <gsmumsg.h>
#include <smsuact.h>
#include <gsmuset.h>
#include <smsuaddr.h>
#include <smsustrm.h>
#include "smspdudb.h"


// general section
_LIT(KConfigFileDir, "sms");
_LIT(KConfigFilenameTest, "te_intersetupgsmsms.txt");
_LIT8(KSetupTelNumbers, "Defaults");
_LIT8(KServiceCenter, "ServiceCenter");
_LIT8(KTelefoneNumber, "TelephoneNumber");

// section names
_LIT8(KEnumerateReadWriteAndDeleteEMSOnSIM,	"EnumerateReadWriteAndDeleteEMSOnSIM");
_LIT8(KEnumerateReadWriteAndDeleteEMSOnPhone,	"EnumerateReadWriteAndDeleteEMSOnPhone");

// section names
_LIT8(KTestPublicMethods,				"PublicMethods");
_LIT8(KTestSendAndReceive7bitMsgs,	"SendAndReceive7bitMsgs");
_LIT8(KTestSendAndReceive8bitMsgs,	"SendAndReceive8bitMsgs");
_LIT8(KTestSendAndReceive16bitMsgs,	"SendAndReceive16bitMsgs");
_LIT8(KTestSendAndReceiveConcatenatedMsgs,	"SendAndReceiveConcatenatedMsgs");
_LIT8(KSmsStoreMsgOfDifferentClasses,	"SmsStoreMsgOfDifferentClasses");
_LIT8(KTestSendAndReceiveDifferentIEIs,	"SendAndReceiveDifferentIEIs");
_LIT8(KTestSendAndReceiveBinaryData,	"SendAndReceiveBinaryData");
_LIT8(KEnumerateAndWriteAndDeleteMessages,	"EnumerateAndWriteAndDeleteMessages");
_LIT8(KPDUSlotStoreWithEnumeration, "PDUSlotStoreWithEnumeration");
_LIT8(KDeleteMessagesWithoutEnumeration, "DeleteMessagesWithoutEnumeration");
_LIT8(KTestSmartMessaging,	"SmartMessaging");
_LIT8(KRetrieveAndStoreSmsParams,	"RetrieveAndStoreSmsParams");
_LIT8(KDuplicatePdusOnStore,	"DuplicatePdusOnStore");
_LIT8(KConcatenatedMessages,	"ConcatenatedMessages");
_LIT8(KDeletingMessageInEmptyStore,	"DeletingMessageInEmptyStore");
_LIT8(KOutOfMemoryWrite,	"OutOfMemoryWrite");
_LIT8(KOutOfMemoryDelete,	"OutOfMemoryDelete");
_LIT8(KTestSendSMS2Port0000,	"SendSMS2Port0000");

// item names
_LIT8(KPDU,     "PDU");
_LIT8(KAlphabet,	"Alphabet");
_LIT8(KMessageLength,	"MessageLength");
_LIT8(KExtendedCharacter,	"ExtendedCharacter");
_LIT8(KInformationElement,	"InformationElement");
_LIT8(KIEIData,	"IEIData");
_LIT8(KServiceCentreAddress,   "ServiceCentreAddress");
_LIT8(KSmspEntryValidParams,  "SmspEntryValidParams");
_LIT8(KSmspEntryScTypeOfNumber,  "SmspEntryScTypeOfNumber");
_LIT8(KSmspEntryScNumberPlan,  "SmspEntryScNumberPlan");
_LIT8(KSmsClass,  "SmsClass");
_LIT8(KSmsStorage,  "SmsStorage");


// messages
_LIT8(KWapTestMsg,"BEGIN:VCARD\r\nVERSION:2.1\r\nFN:Jal\r\nN:Jal\r\nORG:PanSoftware\r\nTITLE:Director\r\nLABEL:Islington\r\nEND:VCARD\r\n");
_LIT(KFormatedMsg2,"This is a text formated message with all formats");
_LIT(KSoundMsg1, "First sound here, 2nd sound here, end");
_LIT(KPictureMsg1, "We got a picture here, small, large or variable");
_LIT(KPictureMsg2, "First picture here, 2nd picture here, end");
_LIT(KAnimationMsg1, "We got animation here, small or large");
_LIT(KAnimationMsg2, "First animation here, 2nd animation here, end");
_LIT(KLongEmsMsg, "This is an EMS Message with large picture ,user melody ,small animation ,followed by a large animation ,predef melody ,a small picture ,and nothing else in bold");
_LIT(KLongFormatMsg, "0123456789012345678 012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");  // 200
_LIT(KTestEmsMsg8Bit1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");	// 268
_LIT(KTestEmsMsg8Bit2,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");	        // 258
_LIT(KTestEmsMsg8Bit3,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");	    // 264
_LIT(KTestEmsMsg16Bit1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");	//128
_LIT(KTest2PduMsg8Bit,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
_LIT(KTest2PduMsg7Bit,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABC");
_LIT(KTest2PduMsg16Bit,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
_LIT(KTestExtdMsg7Bit1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB$");   //154(>134 Octet + 6 = 2 Msg
_LIT(KTestExtdMsg7Bit2,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB$$$$$$$$");
_LIT(KTestEmptyMsg8Bit,"");


_LIT8(KBarbieMelody,"MELODY:*3#g3e3#g3*5#c3*4a2r2#f3#d3#f3b3#g2#f3e3r2#c3#f#c2r2#f3e3#g2\r\n");
_LIT8(KOasisMelody,"MELODY:c2.c3c2c2c2f2f2d2#d2.#d3#d2#d2#d2#g2#g2c2.c3c2c2c2f2f2d2#d2.#d3#d2#d2#d2#g2g2c2.c3c2c2c2f2f\r\n");


TVerdict CSmsInterTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	//
	// Create the Active Scheduler...
	//
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	//
	// Connect to File Server...
	//
	User::LeaveIfError(iFs.Connect());

	//
	// Ensure KUidPhonePwr is defined...
	//
	TInt  defErr = RProperty::Define(KUidSystemCategory, KUidPhonePwr.iUid, RProperty::EInt);
	if (defErr != KErrNone && defErr != KErrAlreadyExists)
		{
		User::Leave(defErr);
		}

    User::LeaveIfError(RProperty::Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOn));

    //
    // Parse the settings from the INI file...
    //
	CTestConfig* configFile = CTestConfig::NewLC(iFs,KConfigFileDir,KConfigFilenameTest);
	const CTestConfigSection* cfgFile = configFile->Section(KSetupTelNumbers);
	if (cfgFile == NULL)
		{
		User::Leave(KErrNotFound);
		}

	const CTestConfigItem* item = cfgFile->Item(KServiceCenter,0);
	if (item == NULL)
		{
		User::Leave(KErrNotFound);
		}

	iServiceCenterNumber.Copy(item->Value());

	item = cfgFile->Item(KTelefoneNumber,0);
	if (item == NULL)
		{
		User::Leave(KErrNotFound);
		}

	iTelephoneNumber.Copy(item->Value());

	// beginning of the destruction
	CleanupStack::PopAndDestroy(configFile);//configFile

    INFO_PRINTF2(_L("TelNo: [%S]"), &iTelephoneNumber);
    INFO_PRINTF2(_L("ScNo:  [%S]"), &iServiceCenterNumber);

	//
	// Load the utility DLL...
	//
	iSmsStackTestUtils = CSmsStackTestUtils::NewL(this, iFs);
	
    return TestStepResult();
	} // CSmsInterTestStep::doTestStepPreambleL


TVerdict CSmsInterTestStep::doTestStepPostambleL()
	{
	delete iSmsStackTestUtils;
	iSmsStackTestUtils = NULL;
	
	iFs.Close();

	delete iScheduler;
	iScheduler = NULL;
	
	__UHEAP_MARKEND;

 	return TestStepResult();
	} // CSmsInterTestStep::doTestStepPostambleL


TBool CSmsInterTestStep::DoSingleTestCaseL( const TDesC8& aSection, TInt aCount )
    {

    CTestConfig* configFile;        //< Pointer to the Configuration file reader
    configFile=CTestConfig::NewLC(iFs,KConfigFileDir,KConfigFilenameTest);

	TBuf8<64> sectionName(aSection);
	sectionName.AppendNum(aCount);
    const CTestConfigSection* section = NULL;
	TBool found( ETrue );

	if ( ( section = configFile->Section( sectionName ) ) != NULL )
		{
		if ( aSection == KEnumerateReadWriteAndDeleteEMSOnSIM )
		{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			EnumerateReadWriteAndDeleteEMSOnSIML( array );
			CleanupStack::PopAndDestroy(&array);
		}
		else if ( aSection == KEnumerateReadWriteAndDeleteEMSOnPhone )
		{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			EnumerateReadWriteAndDeleteEMSOnPhoneL( array );
			CleanupStack::PopAndDestroy(&array);
		}
		else if ( aSection == KTestPublicMethods )
			{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestPublicMethodsL( array );
			CleanupStack::PopAndDestroy(&array);
			}
		else if ( aSection == KTestSendAndReceive7bitMsgs )
			{
		    const TUint KStdDelimiter=',';
		    TPtrC8 pdu;
			const CTestConfigItem* item = NULL;
            item = section->Item( KPDU, 0 );
            if ( item )
				{
				TInt ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,pdu);
				if( ret != KErrNone )
					{
					return EFalse;
					}
				}
			TInt num( 1 );
			CArrayFixFlat<TInt>* messageLengthArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(messageLengthArray);
			CArrayFixFlat<TInt>* charArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(charArray);

			TBuf8<32> itemName( KMessageLength );
			itemName.AppendNum( num );

			TInt param  = section->ItemValue( (TPtrC8)itemName, 0xFF );
			while ( param != 0xFF )
				{
				messageLengthArray->AppendL(param);
				itemName = KMessageLength;
				itemName.AppendNum( ++num );
				param = section->ItemValue( (TPtrC8)itemName, 0xFF );
				}
			num = 1;
			itemName = KExtendedCharacter;
			TBuf8<32> itemName2( KExtendedCharacter );
			itemName.AppendNum( num );
			itemName.AppendNum( 1 );
			itemName2.AppendNum( num );
			itemName2.AppendNum( 2 );

			param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
			TInt param2  = section->ItemValue( (TPtrC8)itemName2, KErrNotFound );

			while ( param != KErrNotFound && param2 != KErrNotFound )
				{
				charArray->AppendL( param2 | ( param << 8)  );
				num++;
				itemName = KExtendedCharacter;
				itemName.AppendNum( num );
				itemName.AppendNum( 1 );
				itemName2 = KExtendedCharacter;
				itemName2.AppendNum( num );
				itemName2.AppendNum( 2 );

				param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
				param2 = section->ItemValue( (TPtrC8)itemName2, KErrNotFound );
				}

			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestSendAndReceive7bitMsgsL( pdu, array, charArray, messageLengthArray );
			CleanupStack::PopAndDestroy(&array);
			CleanupStack::PopAndDestroy(charArray);
			CleanupStack::PopAndDestroy(messageLengthArray);

			}
		else if ( aSection == KTestSendAndReceive8bitMsgs )
			{
			TInt num( 1 );
			CArrayFixFlat<TInt>* messageLengthArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(messageLengthArray);

			TBuf8<32> itemName( KMessageLength );
			itemName.AppendNum( num );

			TInt param  = section->ItemValue( (TPtrC8)itemName, 0xFF );
			while ( param != 0xFF )
				{
				messageLengthArray->AppendL(param);
				itemName = KMessageLength;
				itemName.AppendNum( ++num );
				param = section->ItemValue( (TPtrC8)itemName, 0xFF );
				}

			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestSendAndReceive8bitMsgsL( array, messageLengthArray );
			CleanupStack::PopAndDestroy(&array);
			CleanupStack::PopAndDestroy(messageLengthArray);
			}
		else if ( aSection == KTestSendAndReceive16bitMsgs )
			{
			TInt num( 1 );
			CArrayFixFlat<TInt>* messageLengthArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(messageLengthArray);

			TBuf8<32> itemName( KMessageLength );
			itemName.AppendNum( num );

			TInt param  = section->ItemValue( (TPtrC8)itemName, 0xFF );
			while ( param != 0xFF )
				{
				messageLengthArray->AppendL(param);
				itemName = KMessageLength;
				itemName.AppendNum( ++num );
				param = section->ItemValue( (TPtrC8)itemName, 0xFF );
				}

			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestSendAndReceive16bitMsgsL( array, messageLengthArray );
			CleanupStack::PopAndDestroy(&array);
			CleanupStack::PopAndDestroy(messageLengthArray);
			}
		else if ( aSection == KTestSendAndReceiveConcatenatedMsgs )
			{
			TInt num( 1 );
			CArrayFixFlat<TInt>* alphabetArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(alphabetArray);

			TBuf8<32> itemName( KAlphabet );
			itemName.AppendNum( num );

			TInt param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
			while ( param != KErrNotFound )
				{
				alphabetArray->AppendL(param);
				itemName = KAlphabet;
				itemName.AppendNum( ++num );
				param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
				}

			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestSendAndReceiveConcatenatedMsgsL( array, alphabetArray );
			CleanupStack::PopAndDestroy(&array);
			CleanupStack::PopAndDestroy(alphabetArray);
			}
		else if ( aSection == KTestSendAndReceiveDifferentIEIs )
			{
			TInt num( 1 );
			CArrayFixFlat<TInt>* alphabetArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(alphabetArray);
			CArrayFixFlat<TInt>* informationElementArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(informationElementArray);

			CArrayFixFlat<TInt>* ieiDataArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(ieiDataArray);

			TBuf8<32> itemName( KAlphabet );
			itemName.AppendNum( num );

			TInt param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
			while ( param != KErrNotFound )
				{
				alphabetArray->AppendL(param);
				itemName = KAlphabet;
				itemName.AppendNum( ++num );
				param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
				}
			num = 1;
			itemName = KInformationElement;
			itemName.AppendNum( num );

			param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
			while ( param != KErrNotFound )
				{
				informationElementArray->AppendL(param);
				itemName = KInformationElement;
				itemName.AppendNum( ++num );
				param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
				}

			num = 1;
			itemName = KIEIData;
			itemName.AppendNum( num );

			for ( TInt i = 1; i <= alphabetArray->Count(); i++  )
				{

				TInt j = 1;
				itemName = KIEIData;
				itemName.AppendNum( i );
				itemName.AppendNum( j );

				param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
				while ( param != KErrNotFound )
					{
					ieiDataArray->AppendL(param);
					itemName = KIEIData;
					itemName.AppendNum( i );
					itemName.AppendNum( ++j );
					param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
					}
				ieiDataArray->AppendL(KErrNotFound);

				}

			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestSendAndReceiveDifferentIEIsL( array, alphabetArray, informationElementArray, ieiDataArray );
			CleanupStack::PopAndDestroy(&array);
			CleanupStack::PopAndDestroy(ieiDataArray);
			CleanupStack::PopAndDestroy(informationElementArray);
			CleanupStack::PopAndDestroy(alphabetArray);
			}
		else if ( aSection == KTestSendAndReceiveBinaryData )
			{
			TInt num( 1 );
			CArrayFixFlat<TInt>* messageLengthArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(messageLengthArray);

			TBuf8<32> itemName( KMessageLength );
			itemName.AppendNum( num );

			TInt param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
			while ( param != KErrNotFound )
				{
				messageLengthArray->AppendL(param);
				itemName = KMessageLength;
				itemName.AppendNum( ++num );
				param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
				}
			TestSendAndReceiveBinaryDataL( messageLengthArray );
			CleanupStack::PopAndDestroy(messageLengthArray);
			}
		else if ( aSection == KEnumerateAndWriteAndDeleteMessages )
			{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestEnumerateAndWriteAndDeleteMessagesL( array );
			CleanupStack::PopAndDestroy(&array);
			}
		else if ( aSection == KPDUSlotStoreWithEnumeration )
			{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestPDUSlotStoreWithEnumerationL( array );
			CleanupStack::PopAndDestroy(&array);
			}
		else if ( aSection == KDeleteMessagesWithoutEnumeration )
			{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestDeleteMessagesWithoutEnumerationL( array );
			CleanupStack::PopAndDestroy(&array);
			}
		else if ( aSection == KTestSmartMessaging )
			{
			TInt num( 1 );
			CArrayFixFlat<TInt>* ieiDataArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(ieiDataArray);
			CArrayFixFlat<TInt>* informationElementArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(informationElementArray);
			CArrayFixFlat<TInt>* alphabetArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(alphabetArray);

			TBuf8<32> itemName( KAlphabet );
			itemName.AppendNum( num );

			TInt param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
			while ( param != KErrNotFound )
				{
				alphabetArray->AppendL(param);
				itemName = KAlphabet;
				itemName.AppendNum( ++num );
				param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
				}
			num = 1;
			itemName = KInformationElement;
			itemName.AppendNum( num );

			param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
			while ( param != KErrNotFound )
				{
				informationElementArray->AppendL(param);
				itemName = KInformationElement;
				itemName.AppendNum( ++num );
				param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
				}

			num = 1;
			itemName = KIEIData;
			itemName.AppendNum( num );

			for ( TInt i = 1; i <= alphabetArray->Count(); i++  )
				{

				TInt j = 1;
				itemName = KIEIData;
				itemName.AppendNum( i );
				itemName.AppendNum( j );

				param  = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
				while ( param != KErrNotFound )
					{
					ieiDataArray->AppendL(param);
					itemName = KIEIData;
					itemName.AppendNum( i );
					itemName.AppendNum( ++j );
					param = section->ItemValue( (TPtrC8)itemName, KErrNotFound );
					}
				ieiDataArray->AppendL(KErrNotFound);

				}

			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestSmartMessagingL( array, ieiDataArray, informationElementArray, alphabetArray );
			CleanupStack::PopAndDestroy(&array);
			CleanupStack::PopAndDestroy(alphabetArray);
			CleanupStack::PopAndDestroy(informationElementArray);
			CleanupStack::PopAndDestroy(ieiDataArray);

			}
		else if ( aSection == KRetrieveAndStoreSmsParams )
			{
			TInt param = section->ItemValue(KSmspEntryValidParams, 2);
			RMobileSmsMessaging::TMobileSmspStoreValidParams smspEntryValidParams = ( RMobileSmsMessaging::TMobileSmspStoreValidParams )param;
			param = section->ItemValue(KSmspEntryScTypeOfNumber, 1);
			RMobilePhone::TMobileTON smspEntryScTypeOfNumber = ( RMobilePhone::TMobileTON )param;
			param = section->ItemValue(KSmspEntryScNumberPlan, 1);
			RMobilePhone::TMobileNPI smspEntryScNumberPlan = ( RMobilePhone::TMobileNPI )param;
			const TDesC8& smspEntryScTelNumber = section->ItemValue(KServiceCentreAddress, (_L8("+358508771010")));

			TestRetrieveAndStoreSmsParamsL(smspEntryValidParams, smspEntryScTypeOfNumber, smspEntryScNumberPlan, smspEntryScTelNumber );
			}
		else if ( aSection == KSmsStoreMsgOfDifferentClasses )
			{
			TInt num( 0 );
			CArrayFixFlat<TInt>* smsClassArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(smsClassArray);
			CArrayFixFlat<TInt>* smsStorageArray = new ( ELeave ) CArrayFixFlat<TInt>( 1 );
			CleanupStack::PushL(smsStorageArray);

			TBuf8<16> itemName( KSmsClass );
			itemName.AppendNum( num );

			TInt param  = section->ItemValue( (TPtrC8)itemName, 0xFF );
			while ( param != 0xFF )
				{
				smsClassArray->AppendL(param);
				itemName = KSmsClass;
				itemName.AppendNum( ++num );
				param = section->ItemValue( (TPtrC8)itemName, 0xFF );
				}
			num = 0;
			itemName = KSmsStorage;
			itemName.AppendNum( num );

			param  = section->ItemValue( (TPtrC8)itemName, 0xFF );
			while ( param != 0xFF )
				{
				smsStorageArray->AppendL(param);
				itemName = KSmsStorage;
				itemName.AppendNum( ++num );
				param = section->ItemValue( (TPtrC8)itemName, 0xFF );
				}

			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestSmsStoreMsgOfDifferentClassesL( array, smsStorageArray, smsClassArray );
			CleanupStack::PopAndDestroy(&array);
			CleanupStack::PopAndDestroy(smsStorageArray);
			CleanupStack::PopAndDestroy(smsClassArray);
			}
		else if ( aSection == KDuplicatePdusOnStore )
			{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestDuplicatePdusOnStoreL( array );
			CleanupStack::PopAndDestroy(&array);
			}
		else if ( aSection == KDeletingMessageInEmptyStore )
			{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestDeletingMessageInEmptyStoreL( array );
			CleanupStack::PopAndDestroy(&array);
			}
		else if ( aSection == KConcatenatedMessages )
			{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestConcatenatedMessagesL( array );
			CleanupStack::PopAndDestroy(&array);
			}
		else if ( aSection == KOutOfMemoryWrite )
			{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestOutOfMemoryWriteL( array );
			CleanupStack::PopAndDestroy(&array);
			}
		else if ( aSection == KOutOfMemoryDelete )
			{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestOutOfMemoryDeleteL( array );
			CleanupStack::PopAndDestroy(&array);
			}
		else if ( aSection == KTestSendSMS2Port0000 )
			{
			RPointerArray<CSmsPduDbMessage> array;
			CleanupResetAndDestroyPushL(array);
			ReadPduL( sectionName, array );
			TestSendSMS2Port0000L( array );
			CleanupStack::PopAndDestroy(&array);
			}
		else
			found = EFalse;
		}
	else
		found = EFalse;

    CleanupStack::PopAndDestroy(configFile);//configFile

	return found;
   }


void CSmsInterTestStep::ReadPduL( TBuf8<64> aSectionName, RPointerArray<CSmsPduDbMessage>& aArray )
	{

	CSmsPduDatabase* db = CSmsPduDatabase::NewL(iFs, aSectionName, KConfigFilenameTest, KConfigFileDir);
	CleanupStack::PushL(db);

	db->GetMessageL(aArray, CSmsPDU::ESmsSubmit);

	CleanupStack::PopAndDestroy(db);
	}


void CSmsInterTestStep::TestCreateSocketL()
/**
 *  @test Create a socket
 */
    {
    INFO_PRINTF1(_L("Create a socket") );

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);
    RSocket socket;
    TInt ret=socket.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	CleanupClosePushL(socket);
    TSmsAddr smsaddr;
    smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
    ret=socket.Bind(smsaddr);
	TEST(ret==KErrNone);
	CleanupStack::PopAndDestroy(&socket);

	User::After(2000000); // wait for socket to close before reopening

    RSocket socket2;
    ret=socket2.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	CleanupClosePushL(socket2);
    smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
    ret=socket2.Bind(smsaddr);
	TEST(ret==KErrNone);
	CleanupStack::PopAndDestroy(&socket2);
	CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
    }


void CSmsInterTestStep::TestPublicMethodsL( const RPointerArray<CSmsPduDbMessage>& aArray )
/**
 *  @test Briefly test all public methods
 */
    {
    INFO_PRINTF1(_L("Briefly test all public methods"));

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

    CSmsBuffer* smsBuffer=CSmsBuffer::NewL();
    CSmsPDU::TSmsPDUType pdu = CSmsPDU::ESmsSubmit;
    CSmsMessage* smsMsg = CSmsMessage::NewL(iFs, pdu,smsBuffer);
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
    smsMsg->SetUserDataSettingsL(smsSettings);

    TInt numUnconvChars;
    TInt indexOfFirstUnconvChar;

	TInt bufLen=aArray[0]->iSmsMessage->Buffer().Length();
	HBufC* textBuf=HBufC::NewL(bufLen);
	CleanupStack::PushL(textBuf);
	TPtr textPtr(textBuf->Des());
	aArray[0]->iSmsMessage->Buffer().Extract(textPtr,0,bufLen);

    smsMsg->IsSupportedL(textPtr,numUnconvChars,indexOfFirstUnconvChar);

    CArrayFix<TGsmSms>* smsArray= new(ELeave) CArrayFixFlat<TGsmSms>(10);
	CleanupStack::PushL(smsArray);
    smsMsg->EncodeMessagePDUsL(*smsArray);

    CleanupStack::PopAndDestroy(smsArray);
    CleanupStack::PopAndDestroy(textBuf);
    CleanupStack::PopAndDestroy(smsMsg);
	CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
    }


CSmsMessage* CSmsInterTestStep::CreateAndSetMessageWithOneIEL( const RPointerArray<CSmsPduDbMessage>& aArray, TInt aArrayIndex )
{
/*
 *	Create messages and set them up for SIM storage
 */
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;

	CSmsBufferBase& smsBuffer=aArray[aArrayIndex]->iSmsMessage->Buffer();
	TInt bufLen=smsBuffer.Length();
	HBufC* textBuf=HBufC::NewL(bufLen);
	CleanupStack::PushL(textBuf);
	TPtr buffer(textBuf->Des());
	smsBuffer.Extract(buffer,0,bufLen);

	CSmsMessage* smsMessage=CreateSmsMessageL(buffer,alphabet);
	CleanupStack::PopAndDestroy(textBuf);

	// add information element
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(4);
	object->SetFormatLength(5);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	smsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);
	return smsMessage;
}


CSmsMessage* CSmsInterTestStep::CreateAndSetMessageWithOneIEOnPhoneL( const RPointerArray<CSmsPduDbMessage>& aArray, TInt aArrayIndex )
{
/*
 *	Create messages and set them up for phone storage
 */
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;

	CSmsBufferBase& smsBuffer=aArray[aArrayIndex]->iSmsMessage->Buffer();
	TInt bufLen=smsBuffer.Length();
	HBufC* textBuf=HBufC::NewL(bufLen);
	CleanupStack::PushL(textBuf);
	TPtr buffer(textBuf->Des());
	smsBuffer.Extract(buffer,0,bufLen);

	CSmsMessage* smsMessage=CreateSmsMessageL(buffer,alphabet);
	CleanupStack::PopAndDestroy(textBuf);

	// add information element
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(4);
	object->SetFormatLength(5);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::Pop(object);

	smsMessage->SetStorage(CSmsMessage::ESmsPhoneStorage);
	return smsMessage;
}


HBufC8* CSmsInterTestStep::CreatePictureBufferL(const CFbsBitmap& aBitmap)
/**
 *  Creates buffer and fullfills it bny contents of the bitmap
 */
	{
	const TSize size = aBitmap.SizeInPixels();
	const TInt numLineBytes = size.iWidth/8;
	HBufC8* pictureBuf = HBufC8::NewLC(numLineBytes * size.iHeight);

	HBufC8* scanLine = HBufC8::NewLC(numLineBytes);
	TPtr8 line(scanLine->Des());

	HBufC8* working = HBufC8::NewLC(numLineBytes);
	TPtr8 convertedScanLine(working->Des());
	convertedScanLine.SetLength(numLineBytes);

	for (TInt a=0; a<size.iHeight; a++)
		{
		aBitmap.GetScanLine(line, TPoint(0,a), size.iWidth, EGray2);

		for (TInt word=0; word<numLineBytes; word++)
			{
			convertedScanLine[word] = line[word];
			}

		pictureBuf->Des().Append(convertedScanLine);
		}

	CleanupStack::PopAndDestroy(2, scanLine);
	return pictureBuf;
	}


TBool CSmsInterTestStep::CompareEmsElementL(const CEmsInformationElement& aOriginalElement,RPointerArray<const CEmsInformationElement>& aOtherElementsArray)
/**
 *  Finds a EMS information element in the array and compares contents against found object
 */
	{
	TBool ret=EFalse;
	TUint startPosition=aOriginalElement.StartPosition();
	TSmsId id=aOriginalElement.Identifier();

	TInt count=aOtherElementsArray.Count();
	TInt i=0;
	const CEmsInformationElement* ie=NULL;

		if(id == CSmsInformationElement::ESmsEnhancedTextFormatting)
		{
			for ( i=--count; ret == EFalse && i >= 0 ; --i)
			{
				ie = aOtherElementsArray[i];
				if(ie->Identifier() == CSmsInformationElement::ESmsEnhancedTextFormatting)
				{
					CEmsFormatIE& formatIe = (CEmsFormatIE&) *ie;
					if(startPosition == formatIe.StartPosition()
						&& ((CEmsFormatIE&)aOriginalElement).Bold()==formatIe.Bold()
						&& ((CEmsFormatIE&)aOriginalElement).Italic()==formatIe.Italic()
						&& ((CEmsFormatIE&)aOriginalElement).Underline()==formatIe.Underline()
						&& ((CEmsFormatIE&)aOriginalElement).Strikethrough()==formatIe.Strikethrough()
						&& ((CEmsFormatIE&)aOriginalElement).Alignment()==formatIe.Alignment()
						&& ((CEmsFormatIE&)aOriginalElement).FontSize()==formatIe.FontSize())
						{
								ret= ETrue;
						}
					if(ret)
					{
						if(((CEmsFormatIE&)aOriginalElement).FormatLength()==formatIe.FormatLength())break;
						else if(((CEmsFormatIE&)aOriginalElement).FormatLength() > formatIe.FormatLength())
						{	// just in case original format length goes beyond text
							CEmsFormatIE* object2 = (CEmsFormatIE*)aOriginalElement.DuplicateL();
							CleanupStack::PushL(object2);
							aOtherElementsArray.Remove(i);
							object2->SetFormatLength(formatIe.FormatLength()-formatIe.FormatLength());
							object2->SetStartPosition(formatIe.StartPosition()+formatIe.FormatLength());
							ret=CompareEmsElementL(*object2,aOtherElementsArray);
							CleanupStack::PopAndDestroy(object2);
						}
					}
				}

			}
		}
		else
		{
			for ( i=--count; ret == EFalse && i >= 0 ; --i)
			{
				ie = aOtherElementsArray[i];
				if(startPosition == ie->StartPosition() && id == ie->Identifier())
				{
				// compare contents
					switch (id)
					{
					case CSmsInformationElement::ESmsEnhancedUserDefinedSound:
						{
						CEmsSoundIE& soundIe = (CEmsSoundIE&)*ie;
						const HBufC8* melody=soundIe.Melody();

						const HBufC8* originalMelody=((CEmsSoundIE&)aOriginalElement).Melody();

						if(Mem::Compare(melody->Ptr(),melody->Size(),
						originalMelody->Ptr(),originalMelody->Size())==0)
							{
							aOtherElementsArray.Remove(i);
							ret=ETrue;
							}
						}
						break;
					case CSmsInformationElement::ESmsEnhancedPredefinedSound:
						{
						CEmsPreDefSoundIE& soundIe = (CEmsPreDefSoundIE&) *ie;
						if(((CEmsPreDefSoundIE&)aOriginalElement).PredefinedSound()==soundIe.PredefinedSound())
							{
							aOtherElementsArray.Remove(i);
							ret=ETrue;
							}
						}
						break;
					case CSmsInformationElement::ESmsEnhancedSmallPicture:
					case CSmsInformationElement::ESmsEnhancedLargePicture:
					case CSmsInformationElement::ESmsEnhancedVariablePicture:
						{
						CEmsPictureIE& picIe = (CEmsPictureIE&) *ie;
						CFbsBitmap* pic = picIe.GetBitmapL();
						CleanupStack::PushL(pic);

						CFbsBitmap* originalPic = ((CEmsPictureIE&)aOriginalElement).GetBitmapL();
						CleanupStack::PushL(originalPic);

						if(pic->SizeInPixels()==originalPic->SizeInPixels())
							{
							const HBufC8* picBuf=CreatePictureBufferL(*pic);
							const HBufC8* originalPicBuf=CreatePictureBufferL(*originalPic);

							if(Mem::Compare(picBuf->Ptr(),picBuf->Size(),
								originalPicBuf->Ptr(),originalPicBuf->Size())==0)
								{
								aOtherElementsArray.Remove(i);
								ret=ETrue;
								}
							CleanupStack::PopAndDestroy(2);
							}

						CleanupStack::PopAndDestroy(originalPic);
						CleanupStack::PopAndDestroy(pic);
						}
						break;
					case CSmsInformationElement::ESmsEnhancedSmallAnimation:
					case CSmsInformationElement::ESmsEnhancedLargeAnimation:
						{
						CEmsAnimationIE& picIe = (CEmsAnimationIE&) *ie;
						CFbsBitmap* pic = picIe.GetBitmapL();
						CleanupStack::PushL(pic);

						CFbsBitmap* originalPic = ((CEmsAnimationIE&)aOriginalElement).GetBitmapL();
						CleanupStack::PushL(originalPic);

						if(pic->SizeInPixels()==originalPic->SizeInPixels())
							{
							const HBufC8* picBuf=CreatePictureBufferL(*pic);
							const HBufC8* originalPicBuf=CreatePictureBufferL(*originalPic);

							if(Mem::Compare(picBuf->Ptr(),picBuf->Size(),
								originalPicBuf->Ptr(),originalPicBuf->Size())==0)
								{
								aOtherElementsArray.Remove(i);
								ret=ETrue;
								}
							CleanupStack::PopAndDestroy(2);
							}

						CleanupStack::PopAndDestroy(originalPic);
						CleanupStack::PopAndDestroy(pic);
						}
						break;
					case CSmsInformationElement::ESmsEnhancedPredefinedAnimation:
						{
						CEmsPreDefAnimationIE& animIe = (CEmsPreDefAnimationIE&) *ie;
						if(((CEmsPreDefAnimationIE&)aOriginalElement).Animation()==animIe.Animation())
							{
							aOtherElementsArray.Remove(i);
							ret=ETrue;
							}
						}
						break;
					case CSmsInformationElement::ESmsEnhancedUserPromptIndicator:
						{
						CEmsUserPrompt& upIe = (CEmsUserPrompt&) *ie;
						if(((CEmsUserPrompt&)aOriginalElement).ObjectCount()==upIe.ObjectCount())
							{
							aOtherElementsArray.Remove(i);
							ret=ETrue;
							}
						}
						break;
					case CSmsInformationElement::ESmsEnhancedODI:
						{
						CEmsObjectDistribution& upIe = (CEmsObjectDistribution&) *ie;
						if(((CEmsObjectDistribution&)aOriginalElement).ObjectCount()==upIe.ObjectCount() && ((CEmsObjectDistribution&)aOriginalElement).Forwarding()==upIe.Forwarding())
							{
							aOtherElementsArray.Remove(i);
							ret=ETrue;
							}
						}
						break;
						default:
						break;
					} //end switch id
				} // end if start position and identifier
			} //end for
		} //end else
	return ret;
	}


void CSmsInterTestStep::CompareEmsElementsL(CSmsMessage& aOriginalMessage,CSmsMessage& aOtherMessage)
/**
 *  Compares contents of EMS messages
 */
	{
	// extract and compare text
	CSmsBufferBase& sendBuffer=aOriginalMessage.Buffer();
	CSmsBufferBase& recvBuffer=aOtherMessage.Buffer();
	TInt sendBufLen=sendBuffer.Length();
	TInt recvBufLen=recvBuffer.Length();
	HBufC* sendTextBuf=HBufC::NewL(sendBufLen);
	CleanupStack::PushL(sendTextBuf);
	HBufC* recvTextBuf=HBufC::NewL(recvBufLen);
	CleanupStack::PushL(recvTextBuf);

	TPtr sendTextPtr(sendTextBuf->Des());
	TPtr recvTextPtr(recvTextBuf->Des());
	sendBuffer.Extract(sendTextPtr,0,sendBufLen);
	recvBuffer.Extract(recvTextPtr,0,recvBufLen);

	if (sendTextPtr==recvTextPtr)
		{
		INFO_PRINTF1(_L("Incoming text matches outgoing text!") );
		}
	else
		{
		INFO_PRINTF1(_L("Incoming text does not match outgoing text!"));

		if (aOriginalMessage.SmsPDU().Alphabet() == TSmsDataCodingScheme::ESmsAlphabetUCS2)
			{
			INFO_PRINTF1(_L("<font color=Orange>This is a UCS2 message and some Service Centres (such as Vodafone UK) can drop the last character at the end of a PDU.</font>"));
			INFO_PRINTF1(_L("<font color=Orange>If the incoming text message is similiar to the outgoing one, then ignore the failure.</font>"));
			}
		
		INFO_PRINTF2(_L("Outgoing text was: \"%S\""), &sendTextPtr);
		INFO_PRINTF2(_L("Incoming text was: \"%S\""), &recvTextPtr);

		TEST(sendTextPtr==recvTextPtr);
		}

	//compare number of segments
	TInt numSendMsg = aOriginalMessage.NumMessagePDUsL();
	TInt numRecvMsg = aOtherMessage.NumMessagePDUsL();

	TEST(numSendMsg==numRecvMsg);
	INFO_PRINTF2(_L("Incoming number of segments = %d matches outgoing!"), numRecvMsg);

	// now compare IEs
	const RPointerArray<const CEmsInformationElement>&  emsOriginalElements= aOriginalMessage.GetEMSInformationElementsL();
	RPointerArray<const CEmsInformationElement> copyOfOriginalElements;
	CleanupClosePushL(copyOfOriginalElements);

	const RPointerArray<const CEmsInformationElement>&  emsOtherElements= aOtherMessage.GetEMSInformationElementsL();
	RPointerArray<const CEmsInformationElement> copyOfOtherElements;
	CleanupClosePushL(copyOfOtherElements);

	TInt originalCount=emsOriginalElements.Count();
	TInt i=0;
	const CEmsInformationElement* ie=NULL;

	for ( i=--originalCount; i >= 0 ; --i)
		{
		ie = emsOriginalElements[i];
		copyOfOriginalElements.Append(ie);
		}

	TInt otherCount=emsOtherElements.Count();

	for ( i=--otherCount; i >= 0 ; --i)
		{
		ie = emsOtherElements[i];
		copyOfOtherElements.Append(ie);
		}

	TBool ret=EFalse;
	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());

	i=0;
	while(i < originalCount)
		{
			const CEmsInformationElement& ie=*copyOfOriginalElements[i];
			if(ie.Identifier()==CSmsInformationElement::ESmsEnhancedTextFormatting)
			{
				const CEmsFormatIE& formatIE=(CEmsFormatIE&)ie;
				if(aOriginalMessage.MessageLengthL() < (TInt)(formatIE.FormatLength() + formatIE.StartPosition()))
				{
					CEmsFormatIE* newIE=(CEmsFormatIE*)ie.DuplicateL();
					CleanupStack::PushL(newIE);
					newIE->SetFormatLength(aOriginalMessage.MessageLengthL()-formatIE.StartPosition());
					ret=CompareEmsElementL(*newIE,copyOfOtherElements);
					CleanupStack::PopAndDestroy(newIE);
				}
				else
					ret=CompareEmsElementL(ie,copyOfOtherElements);
			}
			else
				ret=CompareEmsElementL(ie,copyOfOtherElements);
			if(ret)
			{
			}
			else
			{
				switch (ie.Identifier())
				{
				case CSmsInformationElement::ESmsEnhancedTextFormatting:
					{
					CEmsFormatIE& formatIe = (CEmsFormatIE&) ie;
					INFO_PRINTF6(_L("Cannot find FormatIE : start position: %d format length: %d bold: %d italic: %d alignment: %d "), formatIe.StartPosition(),formatIe.FormatLength(),formatIe.Bold(),formatIe.Italic(),formatIe.Alignment());
					//gTest.Getch();
					}
					break;
				case CSmsInformationElement::ESmsEnhancedUserDefinedSound:
					{
					CEmsSoundIE& soundIe = (CEmsSoundIE&) ie;
					INFO_PRINTF3(_L("Cannot find UserDefinedSoundIE start position: %d size: %d"),soundIe.StartPosition(),soundIe.Melody()->Size());
					//gTest.Getch();
					}
					break;
				case CSmsInformationElement::ESmsEnhancedPredefinedSound:
					{
					CEmsPreDefSoundIE& soundIe = (CEmsPreDefSoundIE&) ie;
					INFO_PRINTF3(_L("Cannot find PrefinedSoundIE start position: %d type: %d "),soundIe.StartPosition(),soundIe.PredefinedSound());
					//gTest.Getch();
					}
					break;
				case CSmsInformationElement::ESmsEnhancedSmallPicture:
					{
					CEmsPictureIE& picIe = (CEmsPictureIE&) ie;
					INFO_PRINTF4(_L("Cannot find SmallPictureIE start position: %d width: %d height: %d"),picIe.StartPosition(),picIe.SizeInPixels().iWidth,picIe.SizeInPixels().iHeight);
					//gTest.Getch();
					}
					break;
				case CSmsInformationElement::ESmsEnhancedLargePicture:
					{
					CEmsPictureIE& picIe = (CEmsPictureIE&) ie;
					INFO_PRINTF4(_L("Cannot find LargePictureIE start position: %d width: %d height: %d"),picIe.StartPosition(),picIe.SizeInPixels().iWidth,picIe.SizeInPixels().iHeight);
					//gTest.Getch();
					}
					break;
				case CSmsInformationElement::ESmsEnhancedVariablePicture:
					{
					CEmsPictureIE& picIe = (CEmsPictureIE&) ie;
					INFO_PRINTF4(_L("Cannot find VariablePictureIE start position: %d width: %d height: %d"),picIe.StartPosition(),picIe.SizeInPixels().iWidth,picIe.SizeInPixels().iHeight);
					//gTest.Getch();
					}
					break;
				case CSmsInformationElement::ESmsEnhancedSmallAnimation:
					{
					CEmsAnimationIE& picIe = (CEmsAnimationIE&) ie;
					INFO_PRINTF2(_L("Cannot find SmallAnimationIE start position: %d "),picIe.StartPosition());
					//gTest.Getch();
					}
					break;
				case CSmsInformationElement::ESmsEnhancedLargeAnimation:
					{
					CEmsAnimationIE& picIe = (CEmsAnimationIE&) ie;
					INFO_PRINTF2(_L("Cannot find LargeAnimationIE start position %d "),picIe.StartPosition());
					//gTest.Getch();
					}
					break;
				case CSmsInformationElement::ESmsEnhancedPredefinedAnimation:
					{
					CEmsPreDefAnimationIE& animIe = (CEmsPreDefAnimationIE&) ie;
					INFO_PRINTF3(_L("Cannot find PreDefinedAnimationIE start position: %d type: %d "),animIe.StartPosition(),animIe.Animation());
					//gTest.Getch();
					}
					break;
				case CSmsInformationElement::ESmsEnhancedUserPromptIndicator:
					{
					CEmsUserPrompt& upIe = (CEmsUserPrompt&) ie;
					INFO_PRINTF3(_L("Cannot find UserPromptIE start position:%d number of objects: %d"),upIe.StartPosition(),upIe.ObjectCount() );
					//gTest.Getch();
					}
					break;
				case CSmsInformationElement::ESmsEnhancedODI:
					{
					CEmsObjectDistribution& odiIe = (CEmsObjectDistribution&) ie;
					INFO_PRINTF4(_L("Cannot find ObjectDistributionIE start position:%d number of objects: %d forwarding: %d"),odiIe.StartPosition(),odiIe.ObjectCount(), odiIe.Forwarding() );
					//gTest.Getch();
					}
					break;
				default:
					break;
				}
			}
			i++;
		}
	RFbsSession::Disconnect();
	CleanupStack::PopAndDestroy(&copyOfOtherElements);
	CleanupStack::PopAndDestroy(&copyOfOriginalElements);
	CleanupStack::PopAndDestroy(recvTextBuf);
	CleanupStack::PopAndDestroy(sendTextBuf);
	}


void CSmsInterTestStep::SendRecvTextFormated7BitEMSL()
/**
 *  @test Send and Receive 7Bit EMS message with all supported formating
 */
{
    INFO_PRINTF1(_L("Send and receive 7Bit Ems message with text formating "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// create EMS message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendMessage=CreateSmsMessageL(KFormatedMsg2,alphabet);
	CleanupStack::PushL(sendMessage);

	// add information element
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(5);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	object->SetItalic(ETrue);
	object->SetUnderline(ETrue);
	object->SetStrikethrough(ETrue);
	object->SetAlignment(CEmsFormatIE::ECenter);
	object->SetFontSize(CEmsFormatIE::ELarge);
	CleanupStack::PushL(object);
	sendMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);


	// send EMS message
	SendSmsL(sendMessage,socket);
	
	WaitForRecvL(socket);
	
	CSmsMessage* recvMessage = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage);
	//compare send and received messages
	CompareEmsElementsL(*sendMessage, *recvMessage);
	 
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);
	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
   CleanupStack::PopAndDestroy(&socketServer);
  
   

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::SendRecvTextFormated8BitEMSL()
/**
 *  @test Send and Receive 8Bit EMS message with all supported formating
 */
{
    INFO_PRINTF1(_L("Send and receive 8Bit Ems message with text formating "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// create EMS message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* sendMessage=CreateSmsMessageL(KFormatedMsg2,alphabet);
	CleanupStack::PushL(sendMessage);

	// add information element
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(5);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	object->SetItalic(ETrue);
	object->SetUnderline(ETrue);
	object->SetStrikethrough(ETrue);
	object->SetAlignment(CEmsFormatIE::ECenter);
	object->SetFontSize(CEmsFormatIE::ELarge);
	CleanupStack::PushL(object);
	sendMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);


	// send EMS message
	SendSmsL(sendMessage,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage);


	//compare send and received messages
	CompareEmsElementsL(*sendMessage, *recvMessage);

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);
	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::SendRecvTextFormated16BitEMSL()
/**
 *  @test Send and Receive 8Bit EMS message with all supported formating
 */
{
    INFO_PRINTF1(_L("Send and receive 16Bit Ems message with text formating "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// create EMS message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabetUCS2;
	CSmsMessage* sendMessage=CreateSmsMessageL(KFormatedMsg2,alphabet);
	CleanupStack::PushL(sendMessage);

	// add information element
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(5);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	object->SetItalic(ETrue);
	object->SetUnderline(ETrue);
	object->SetStrikethrough(ETrue);
	object->SetAlignment(CEmsFormatIE::ECenter);
	object->SetFontSize(CEmsFormatIE::ELarge);
	CleanupStack::PushL(object);
	sendMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);


	// send EMS message
	SendSmsL(sendMessage,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage);


	//compare send and received messages
	CompareEmsElementsL(*sendMessage, *recvMessage);

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);
	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::EnumerateReadWriteAndDeleteEMSOnSIML( const RPointerArray<CSmsPduDbMessage>& aArray )
/**
 *  @test Switch on the phone with a SIM.
 *  Enumerate, delete (first message if exist and last message after writing),
 *  write (two messages to the sim store)
 */
{
	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	// Open the socket for SIM operations
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrLocalOperation);

	TInt ret = 0;
	CSmsMessage* smsmessage;

	// Enumerate messages
    INFO_PRINTF1(_L("Enumeration"));
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);
	TInt count = messages.Count();
	TInt index = 0;

	// Delete messages
    INFO_PRINTF1(_L("Delete messages"));
	while ( index < count )
	{
		smsmessage = messages[index++];

		INFO_PRINTF2(_L("Delete %d. message from the store "), index );
		ret = DeleteSmsL(*smsmessage, socket);
		TEST( !( ret == KErrNone ) );

	}
	messages.ResetAndDestroy();

	// Create and store messages
    INFO_PRINTF1(_L("store messages"));
	count = aArray.Count();
	index = 0;
	while ( index < count )
	{
		INFO_PRINTF2(_L("Write %d. message"),index+1);
		CSmsMessage* emsMessage = CreateAndSetMessageWithOneIEL( aArray, index++ );
		CleanupStack::PushL(emsMessage);
		WriteSmsToSimL(*emsMessage, socket);
		CleanupStack::PopAndDestroy(emsMessage);
	}

	// Enumerate messages
    INFO_PRINTF1(_L("Enumeration"));
	ReadSmsStoreL(socket, messages);
	count = messages.Count();
	index = 0;

	// Delete messages
    INFO_PRINTF1(_L("Delete messages"));
	while ( index < count )
	{
		smsmessage = messages[index++];
		INFO_PRINTF2(_L("Delete %d. message from the store"), index );
		ret = DeleteSmsL(*smsmessage, socket);
		TEST( !( ret == KErrNone ) );
	}

	CleanupStack::PopAndDestroy(&messages);
    CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::SendRecvPreDefSoundEMSL()
/**
 *  @test Send and Receive EMS message with multiple predefined sound IEs
 */
{
    INFO_PRINTF1(_L("Send and receive EMS with multiple predefined sound "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// create EMS message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendMessage=CreateSmsMessageL(KSoundMsg1,alphabet);
	CleanupStack::PushL(sendMessage);

	// add two sound objects
	CEmsPreDefSoundIE* object1 = CEmsPreDefSoundIE::NewL( CEmsPreDefSoundIE::EChordLow);
	CleanupStack::PushL(object1);
	object1->SetStartPosition(17);
	sendMessage->AddEMSInformationElementL(*object1);
	CleanupStack::PopAndDestroy(object1);


	CEmsPreDefSoundIE* object2 = CEmsPreDefSoundIE::NewL(CEmsPreDefSoundIE::ETaDa);
	CleanupStack::PushL(object2);
	object2->SetStartPosition(33);
	sendMessage->AddEMSInformationElementL(*object2);
	CleanupStack::PopAndDestroy(object2);

	// send EMS message
	SendSmsL(sendMessage,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage);


	//compare send and received messages
	CompareEmsElementsL(*sendMessage, *recvMessage);


	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);
	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::SendRecvUserDefSoundEMSL()
/**
 *  @test Send and Receive EMS message with multiple user defined sound IEs
 */
{
    INFO_PRINTF1(_L("Send and receive EMS with multiple user defined sound "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// create EMS message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* sendMessage=CreateSmsMessageL(KSoundMsg1,alphabet);
	CleanupStack::PushL(sendMessage);

	// add two user melodies
	CEmsSoundIE* object1 = CEmsSoundIE::NewL(KBarbieMelody());
	CleanupStack::PushL(object1);
	object1->SetStartPosition(17);
	sendMessage->AddEMSInformationElementL(*object1);
	CleanupStack::PopAndDestroy(object1);

	CEmsSoundIE* object2 = CEmsSoundIE::NewL(KOasisMelody());
	CleanupStack::PushL(object2);
	object2->SetStartPosition(33);
	sendMessage->AddEMSInformationElementL(*object2);
	CleanupStack::PopAndDestroy(object2);

	// send EMS message
	SendSmsL(sendMessage,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage);


	//compare send and received messages
	CompareEmsElementsL(*sendMessage, *recvMessage);

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);
	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::EnumerateReadWriteAndDeleteEMSOnPhoneL( const RPointerArray<CSmsPduDbMessage>& aArray )
/**
 *  @test Switch on the phone with a SIM.
 *  Enumerate, delete (first message if exist and last message after writing),
 *  write (two messages to the phone store)
 */
{
	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	// Open the socket for SIM operations
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrLocalOperation);

	TInt ret = 0;
	CSmsMessage* smsmessage;

	// Enumerate messages
    INFO_PRINTF1(_L("Enumeration"));    
   	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);
	TInt count = messages.Count();
	TInt index = 0;

	// Delete messages
    INFO_PRINTF1(_L("Delete messages"));
	while ( index < count )
	{
		smsmessage = messages[index++];

		INFO_PRINTF2(_L("Delete %d. message from the store "), index );
		ret = DeleteSmsL(*smsmessage, socket);
		TEST( !( ret == KErrNone ) );

	}
	messages.ResetAndDestroy();

	// Create and store messages
    INFO_PRINTF1(_L("store messages"));
	count = aArray.Count();
	index = 0;
	while ( index < count )
	{
		INFO_PRINTF2(_L("Write %d. message"),index+1);
		CSmsMessage* emsMessage = CreateAndSetMessageWithOneIEOnPhoneL( aArray, index++ );
		CleanupStack::PushL(emsMessage);
		/*
		Some phones doesn't support phone memory. In this case SMS stack completes
		write request with KErrNotFound, or KErrNoMemory.
		*/
		TRAPD(ret, WriteSmsLeaveIfErrorL(*emsMessage, socket););
		if(ret == KErrNoMemory || ret == KErrNotFound)
		{
			INFO_PRINTF1(_L("Phone does not support phone memory!"));
			User::After(5000000);
			CleanupStack::PopAndDestroy(emsMessage);
			break;
		}
		CleanupStack::PopAndDestroy(emsMessage);
	}

	// Enumerate messages
	INFO_PRINTF1(_L("Enumeration"));
	ReadSmsStoreL(socket, messages);
	count = messages.Count();
	index = 0;

	// Delete messages
	INFO_PRINTF1(_L("Delete messages"));
	while ( index < count )
	{
		smsmessage = messages[index++];
		INFO_PRINTF2(_L("Delete %d. message from the store"), index );
		ret = DeleteSmsL(*smsmessage, socket);
		TEST( !( ret == KErrNone ) );
	}

	CleanupStack::PopAndDestroy(&messages);
    CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
}


void CSmsInterTestStep::SendRecvPicturesEMSL()
/**
 *  @test Send and Receive EMS message with multiple pictures
 */
{
    INFO_PRINTF1(_L("Send and receive EMS messages with pictures "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// send, receive and compare EMS with one small picture, 7Bit coding
	INFO_PRINTF1(_L("Send EMS with 1 small picture "));
	TSmsDataCodingScheme::TSmsAlphabet alphabet1=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendMessage1=CreateSmsMessageL(KPictureMsg1,alphabet1);
	CleanupStack::PushL(sendMessage1);

	CEmsPictureIE* pic1 = EmsTestUtils::CreatePictureL(0);   // 0 for small pic
	CleanupStack::PushL(pic1);
	pic1->SetStartPosition(22);

	sendMessage1->AddEMSInformationElementL(*pic1);
	CleanupStack::PopAndDestroy(pic1);

	SendSmsL(sendMessage1,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage1 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage1);

	CompareEmsElementsL(*sendMessage1, *recvMessage1);

	CleanupStack::PopAndDestroy(recvMessage1);
	CleanupStack::PopAndDestroy(sendMessage1);

	// send, receive and compare EMS with one large picture, 16Bit coding

	INFO_PRINTF1(_L("Send EMS with 1 large picture "));
	TSmsDataCodingScheme::TSmsAlphabet alphabet2=TSmsDataCodingScheme::ESmsAlphabetUCS2;
	CSmsMessage* sendMessage2=CreateSmsMessageL(KPictureMsg1,alphabet2);
	CleanupStack::PushL(sendMessage2);

	CEmsPictureIE* pic2 = EmsTestUtils::CreatePictureL(1);   // 1 for large pic
	CleanupStack::PushL(pic2);
	pic2->SetStartPosition(22);

	sendMessage2->AddEMSInformationElementL(*pic2);
	CleanupStack::PopAndDestroy(pic2);

	SendSmsL(sendMessage2,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage2 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage2);

	CompareEmsElementsL(*sendMessage2, *recvMessage2);

	CleanupStack::PopAndDestroy(recvMessage2);
	CleanupStack::PopAndDestroy(sendMessage2);

	// send, receive and compare EMS with one variable picture, 8Bit coding

	INFO_PRINTF1(_L("Send EMS with 1 variable picture "));
	TSmsDataCodingScheme::TSmsAlphabet alphabet3=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* sendMessage3=CreateSmsMessageL(KPictureMsg1,alphabet3);
	CleanupStack::PushL(sendMessage3);

	CEmsPictureIE* pic3 = EmsTestUtils::CreatePictureL(2);	 // 2 for variable pic
	CleanupStack::PushL(pic3);
	pic3->SetStartPosition(22);

	sendMessage3->AddEMSInformationElementL(*pic3);
	CleanupStack::PopAndDestroy(pic3);

	SendSmsL(sendMessage3,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage3 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage3);

	CompareEmsElementsL(*sendMessage3, *recvMessage3);

	CleanupStack::PopAndDestroy(recvMessage3);
	CleanupStack::PopAndDestroy(sendMessage3);

	// send, receive and compare EMS with two small pictures, 7Bit coding

	INFO_PRINTF1(_L("Send EMS with 2 small pictures "));
	TSmsDataCodingScheme::TSmsAlphabet alphabet4=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendMessage4=CreateSmsMessageL(KPictureMsg2,alphabet4);
	CleanupStack::PushL(sendMessage4);

	CEmsPictureIE* pic4 = EmsTestUtils::CreatePictureL(0);
	CleanupStack::PushL(pic4);
	pic4->SetStartPosition(19);

	sendMessage4->AddEMSInformationElementL(*pic4);
	CleanupStack::PopAndDestroy(pic4);

	CEmsPictureIE* pic5 = EmsTestUtils::CreatePictureL(0);
	CleanupStack::PushL(pic5);
	pic5->SetStartPosition(37);

	sendMessage4->AddEMSInformationElementL(*pic5);
	CleanupStack::PopAndDestroy(pic5);

	SendSmsL(sendMessage4,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage4 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage4);

	CompareEmsElementsL(*sendMessage4, *recvMessage4);

	CleanupStack::PopAndDestroy(recvMessage4);
	CleanupStack::PopAndDestroy(sendMessage4);

	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::SendRecvPreDefAnimationL()
/**
 *  @test Send and Receive EMS message with multiple predefined animation
 */
{
    INFO_PRINTF1(_L("Send and receive EMS messages with predefined animation "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// send, receive and compare EMS with one predefined animation, 16Bit coding
	INFO_PRINTF1(_L("Send EMS with 1 predefined animation "));
	TSmsDataCodingScheme::TSmsAlphabet alphabet1=TSmsDataCodingScheme::ESmsAlphabetUCS2;
	CSmsMessage* sendMessage1=CreateSmsMessageL(KAnimationMsg1,alphabet1);
	CleanupStack::PushL(sendMessage1);
	CEmsPreDefAnimationIE* anim1=NULL;

	anim1 = CEmsPreDefAnimationIE::NewL(CEmsPreDefAnimationIE::EWinking);
	CleanupStack::PushL(anim1);
	anim1->SetStartPosition(22);

	sendMessage1->AddEMSInformationElementL(*anim1);
	CleanupStack::PopAndDestroy(anim1);

	SendSmsL(sendMessage1,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage1 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage1);

	CompareEmsElementsL(*sendMessage1, *recvMessage1);

	CleanupStack::PopAndDestroy(recvMessage1);
	CleanupStack::PopAndDestroy(sendMessage1);

	// send, receive and compare EMS with two predefined animation, 7Bit coding

	INFO_PRINTF1(_L("Send EMS with 2 predefined animation "));
	TSmsDataCodingScheme::TSmsAlphabet alphabet2=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendMessage2=CreateSmsMessageL(KAnimationMsg2,alphabet2);
	CleanupStack::PushL(sendMessage2);
	CEmsPreDefAnimationIE* anim2=NULL;

	anim2 = CEmsPreDefAnimationIE::NewL(CEmsPreDefAnimationIE::ECrying);
	CleanupStack::PushL(anim2);
	anim2->SetStartPosition(21);

	sendMessage2->AddEMSInformationElementL(*anim2);
	CleanupStack::PopAndDestroy(anim2);

	CEmsPreDefAnimationIE* anim3=NULL;

	anim3 = CEmsPreDefAnimationIE::NewL(CEmsPreDefAnimationIE::ELaughing);
	CleanupStack::PushL(anim3);
	anim3->SetStartPosition(41);

	sendMessage2->AddEMSInformationElementL(*anim3);
	CleanupStack::PopAndDestroy(anim3);

	SendSmsL(sendMessage2,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage2 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage2);

	CompareEmsElementsL(*sendMessage2, *recvMessage2);

	CleanupStack::PopAndDestroy(recvMessage2);
	CleanupStack::PopAndDestroy(sendMessage2);

	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::SendRecvUserDefAnimationL()
/**
 *  @test Send and Receive EMS message with multiple user defined animation
 */
{
    INFO_PRINTF1(_L("Send and receive EMS messages with user defined animation "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// send, receive and compare EMS with one small animation, 16Bit coding
	INFO_PRINTF1(_L("Send EMS with 1 small user animation "));
	TSmsDataCodingScheme::TSmsAlphabet alphabet1=TSmsDataCodingScheme::ESmsAlphabetUCS2;
	CSmsMessage* sendMessage1=CreateSmsMessageL(KAnimationMsg1,alphabet1);
	CleanupStack::PushL(sendMessage1);

	CEmsAnimationIE* anim1 = EmsTestUtils::CreateAnimationL(0); // 0 for small animation
	CleanupStack::PushL(anim1);
	anim1->SetStartPosition(22);

	sendMessage1->AddEMSInformationElementL(*anim1);
	CleanupStack::PopAndDestroy(anim1);

	SendSmsL(sendMessage1,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage1 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage1);

	CompareEmsElementsL(*sendMessage1, *recvMessage1);

	CleanupStack::PopAndDestroy(recvMessage1);
	CleanupStack::PopAndDestroy(sendMessage1);

	// send, receive and compare EMS with one large animation, 8Bit coding

	INFO_PRINTF1(_L("Send EMS with 1 large user animation "));
	TSmsDataCodingScheme::TSmsAlphabet alphabet2=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* sendMessage2=CreateSmsMessageL(KAnimationMsg1,alphabet2);
	CleanupStack::PushL(sendMessage2);

	CEmsAnimationIE* anim2 = EmsTestUtils::CreateAnimationL(1); // 1 for large animation
	CleanupStack::PushL(anim2);
	anim2->SetStartPosition(22);

	sendMessage2->AddEMSInformationElementL(*anim2);
	CleanupStack::PopAndDestroy(anim2);

	SendSmsL(sendMessage2,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage2 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage2);

	CompareEmsElementsL(*sendMessage2, *recvMessage2);

	CleanupStack::PopAndDestroy(recvMessage2);
	CleanupStack::PopAndDestroy(sendMessage2);

	// send, receive and compare EMS with two small animation, 7Bit coding

	INFO_PRINTF1(_L("Send EMS with 2 small user animation "));
	TSmsDataCodingScheme::TSmsAlphabet alphabet3=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendMessage3=CreateSmsMessageL(KAnimationMsg2,alphabet3);
	CleanupStack::PushL(sendMessage3);

	CEmsAnimationIE* anim3 = EmsTestUtils::CreateAnimationL(0);
	CleanupStack::PushL(anim3);
	anim3->SetStartPosition(21);

	sendMessage3->AddEMSInformationElementL(*anim3);
	CleanupStack::PopAndDestroy(anim3);

	CEmsAnimationIE* anim4 = EmsTestUtils::CreateAnimationL(0);
	CleanupStack::PushL(anim4);
	anim4->SetStartPosition(41);

	sendMessage3->AddEMSInformationElementL(*anim4);
	CleanupStack::PopAndDestroy(anim4);

	SendSmsL(sendMessage3,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage3 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage3);

	CompareEmsElementsL(*sendMessage3, *recvMessage3);

	CleanupStack::PopAndDestroy(recvMessage3);
	CleanupStack::PopAndDestroy(sendMessage3);

	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::SendRecvUserPromptEMSL()
/**
 *  @test Send and Receive EMS message with user prompt indicator
 */
{
    INFO_PRINTF1(_L("Send and receive EMS messages with user prompt indicator"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// send, receive and compare EMS with one small picture and a user prompt, 7Bit coding
	INFO_PRINTF1(_L("Send EMS with 1 small picture and a user prompt "));
	TSmsDataCodingScheme::TSmsAlphabet alphabet1=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendMessage1=CreateSmsMessageL(KPictureMsg1,alphabet1);
	CleanupStack::PushL(sendMessage1);

	CEmsPictureIE* pic1 = EmsTestUtils::CreatePictureL(0);
	CleanupStack::PushL(pic1);
	pic1->SetStartPosition(22);

	sendMessage1->AddEMSInformationElementL(*pic1);
	CleanupStack::PopAndDestroy(pic1);

	// add a user prompt
	CEmsUserPrompt* prompt1 = CEmsUserPrompt::NewL(1);   //one picture
	CleanupStack::PushL(prompt1);
	prompt1->SetStartPosition(22);
	sendMessage1->AddEMSInformationElementL(*prompt1);
	CleanupStack::PopAndDestroy(prompt1);

	SendSmsL(sendMessage1,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage1 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage1);

	CompareEmsElementsL(*sendMessage1, *recvMessage1);

	CleanupStack::PopAndDestroy(recvMessage1);
	CleanupStack::PopAndDestroy(sendMessage1);

	// Two small pictures in one position with a prompt on the same position, 8Bit

	INFO_PRINTF1(_L("Send EMS with 2 small pictures and a user prompt"));
	TSmsDataCodingScheme::TSmsAlphabet alphabet2=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* sendMessage2=CreateSmsMessageL(KPictureMsg2,alphabet2);
	CleanupStack::PushL(sendMessage2);

	CEmsPictureIE* pic2 = EmsTestUtils::CreatePictureL(0);
	CleanupStack::PushL(pic2);
	pic2->SetStartPosition(19);

	sendMessage2->AddEMSInformationElementL(*pic2);
	CleanupStack::PopAndDestroy(pic2);

	CEmsPictureIE* pic3 = EmsTestUtils::CreatePictureL(0);
	CleanupStack::PushL(pic3);
	pic3->SetStartPosition(19);

	sendMessage2->AddEMSInformationElementL(*pic3);
	CleanupStack::PopAndDestroy(pic3);

	// add a user prompt
	CEmsUserPrompt* prompt2 = CEmsUserPrompt::NewL(2);   //two pictures
	CleanupStack::PushL(prompt2);
	prompt2->SetStartPosition(19);
	sendMessage2->AddEMSInformationElementL(*prompt2);
	CleanupStack::PopAndDestroy(prompt2);

	SendSmsL(sendMessage2,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage2 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage2);

	CompareEmsElementsL(*sendMessage2, *recvMessage2);

	CleanupStack::PopAndDestroy(recvMessage2);
	CleanupStack::PopAndDestroy(sendMessage2);

	// two small pictures and a user prompt positioned on the 2nd, 7Bit coding
	INFO_PRINTF1(_L("Send EMS with 2 small pictures and prompt on the 2nd"));
	TSmsDataCodingScheme::TSmsAlphabet alphabet3=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendMessage3=CreateSmsMessageL(KPictureMsg2,alphabet3);
	CleanupStack::PushL(sendMessage3);

	CEmsPictureIE* pic4 = EmsTestUtils::CreatePictureL(0);
	CleanupStack::PushL(pic4);
	pic4->SetStartPosition(19);

	sendMessage3->AddEMSInformationElementL(*pic4);
	CleanupStack::PopAndDestroy(pic4);

	CEmsPictureIE* pic5 = EmsTestUtils::CreatePictureL(0);
	CleanupStack::PushL(pic5);
	pic5->SetStartPosition(37);

	sendMessage3->AddEMSInformationElementL(*pic5);
	CleanupStack::PopAndDestroy(pic5);

	// add a user prompt
	CEmsUserPrompt* prompt3 = CEmsUserPrompt::NewL(1);   //one pictures
	CleanupStack::PushL(prompt3);
	prompt3->SetStartPosition(37);
	sendMessage3->AddEMSInformationElementL(*prompt3);
	CleanupStack::PopAndDestroy(prompt3);

	SendSmsL(sendMessage3,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage3 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage3);

	CompareEmsElementsL(*sendMessage3, *recvMessage3);

	CleanupStack::PopAndDestroy(recvMessage3);
	CleanupStack::PopAndDestroy(sendMessage3);

	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::SendRecvObjectDistributionEMSL()
/**
 *  @test Send and Receive EMS message with user prompt indicator
 */
    {
    INFO_PRINTF1(_L("Send and receive EMS messages with object distribution indicator"));

    RSocketServ socketServer;
    Connect2EsockLC(socketServer);
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    // send, receive and compare EMS with one small picture and a odi, 7Bit coding
    INFO_PRINTF1(_L("Send EMS with 1 small picture and object distribution indicator "));
    TSmsDataCodingScheme::TSmsAlphabet alphabet1=TSmsDataCodingScheme::ESmsAlphabet7Bit;
    CSmsMessage* sendMessage1=CreateSmsMessageL(KPictureMsg1,alphabet1);
    CleanupStack::PushL(sendMessage1);

    CEmsPictureIE* pic1 = EmsTestUtils::CreatePictureL(0);
    CleanupStack::PushL(pic1);
    pic1->SetStartPosition(22);

    sendMessage1->AddEMSInformationElementL(*pic1);
    CleanupStack::PopAndDestroy(pic1);

    // add an object distribution indicator
    CEmsObjectDistribution* odi1 = CEmsObjectDistribution::NewL(1, CEmsObjectDistribution::EForward);   //one picture
    CleanupStack::PushL(odi1);
    odi1->SetStartPosition(22);
    sendMessage1->AddEMSInformationElementL(*odi1);
    CleanupStack::PopAndDestroy(odi1);

    SendSmsL(sendMessage1,socket);

    WaitForRecvL(socket);
    CSmsMessage* recvMessage1 = RecvSmsL(socket);
    CleanupStack::PushL(recvMessage1);

    CompareEmsElementsL(*sendMessage1, *recvMessage1);

    CleanupStack::PopAndDestroy(recvMessage1);
    CleanupStack::PopAndDestroy(sendMessage1);

    // Two small pictures in one position with an odi on the same position, 8Bit

    INFO_PRINTF1(_L("Send EMS with 2 small pictures and an object distribution indicator"));
    TSmsDataCodingScheme::TSmsAlphabet alphabet2=TSmsDataCodingScheme::ESmsAlphabet8Bit;
    CSmsMessage* sendMessage2=CreateSmsMessageL(KPictureMsg2,alphabet2);
    CleanupStack::PushL(sendMessage2);

    CEmsPictureIE* pic2 = EmsTestUtils::CreatePictureL(0);
    CleanupStack::PushL(pic2);
    pic2->SetStartPosition(19);

    sendMessage2->AddEMSInformationElementL(*pic2);
    CleanupStack::PopAndDestroy(pic2);

    CEmsPictureIE* pic3 = EmsTestUtils::CreatePictureL(0);
    CleanupStack::PushL(pic3);
    pic3->SetStartPosition(19);

    sendMessage2->AddEMSInformationElementL(*pic3);
    CleanupStack::PopAndDestroy(pic3);

    // add an object distribution indicator
    CEmsObjectDistribution* odi2 = CEmsObjectDistribution::NewL(2, CEmsObjectDistribution::EForward);   //two pictures
    CleanupStack::PushL(odi2);
    odi2->SetStartPosition(19);
    sendMessage2->AddEMSInformationElementL(*odi2);
    CleanupStack::PopAndDestroy(odi2);

    SendSmsL(sendMessage2,socket);

    WaitForRecvL(socket);
    CSmsMessage* recvMessage2 = RecvSmsL(socket);
    CleanupStack::PushL(recvMessage2);

    CompareEmsElementsL(*sendMessage2, *recvMessage2);

    CleanupStack::PopAndDestroy(recvMessage2);
    CleanupStack::PopAndDestroy(sendMessage2);

    // two small pictures and an object distribution indicator positioned on the 2nd, 7Bit coding
    INFO_PRINTF1(_L("Send EMS with 2 small pictures and an object distribution indicator on the 2nd"));
    TSmsDataCodingScheme::TSmsAlphabet alphabet3=TSmsDataCodingScheme::ESmsAlphabet7Bit;
    CSmsMessage* sendMessage3=CreateSmsMessageL(KPictureMsg2,alphabet3);
    CleanupStack::PushL(sendMessage3);

    CEmsPictureIE* pic4 = EmsTestUtils::CreatePictureL(0);
    CleanupStack::PushL(pic4);
    pic4->SetStartPosition(19);

    sendMessage3->AddEMSInformationElementL(*pic4);
    CleanupStack::PopAndDestroy(pic4);

    CEmsPictureIE* pic5 = EmsTestUtils::CreatePictureL(0);
    CleanupStack::PushL(pic5);
    pic5->SetStartPosition(37);

    sendMessage3->AddEMSInformationElementL(*pic5);
    CleanupStack::PopAndDestroy(pic5);

    // add a object distribution indicator
    CEmsObjectDistribution* odi3 = CEmsObjectDistribution::NewL(1, CEmsObjectDistribution::EForward);   //one picture
    CleanupStack::PushL(odi3);
    odi3->SetStartPosition(37);
    sendMessage3->AddEMSInformationElementL(*odi3);
    CleanupStack::PopAndDestroy(odi3);

    SendSmsL(sendMessage3,socket);

    WaitForRecvL(socket);
    CSmsMessage* recvMessage3 = RecvSmsL(socket);
    CleanupStack::PushL(recvMessage3);

    CompareEmsElementsL(*sendMessage3, *recvMessage3);

    CleanupStack::PopAndDestroy(recvMessage3);
    CleanupStack::PopAndDestroy(sendMessage3);

    CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

    User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::SendRecvManyEMSObjectsL()
/**
 *  @test Send and Receive EMS message with many objects of different types
 */
{
    INFO_PRINTF1(_L("Send and receive EMS messages with 7 objects"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// create EMS message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendMessage=CreateSmsMessageL(KLongEmsMsg,alphabet);
	CleanupStack::PushL(sendMessage);

	// add a large picture in position 42
	CEmsPictureIE* pic1 = EmsTestUtils::CreatePictureL(1);
	CleanupStack::PushL(pic1);
	pic1->SetStartPosition(42);
	sendMessage->AddEMSInformationElementL(*pic1);
	CleanupStack::PopAndDestroy(pic1);

	//add a user melody in position 55
	CEmsSoundIE* melody1 = CEmsSoundIE::NewL(KBarbieMelody());
	CleanupStack::PushL(melody1);
	melody1->SetStartPosition(55);
	sendMessage->AddEMSInformationElementL(*melody1);
	CleanupStack::PopAndDestroy(melody1);

	//add a small animation in position 72
	CEmsAnimationIE* anim1 = EmsTestUtils::CreateAnimationL(0);
	CleanupStack::PushL(anim1);
	anim1->SetStartPosition(72);

	sendMessage->AddEMSInformationElementL(*anim1);
	CleanupStack::PopAndDestroy(anim1);

	//add a large animation in position 103
	CEmsAnimationIE* anim2 = EmsTestUtils::CreateAnimationL(1);
	CleanupStack::PushL(anim2);
	anim2->SetStartPosition(103);

	sendMessage->AddEMSInformationElementL(*anim2);
	CleanupStack::PopAndDestroy(anim2);

	//add a predef melody in position 117
	CEmsPreDefSoundIE* sound1 = CEmsPreDefSoundIE::NewL( CEmsPreDefSoundIE::EChordLow);
	CleanupStack::PushL(sound1);
	sound1->SetStartPosition(117);
	sendMessage->AddEMSInformationElementL(*sound1);
	CleanupStack::PopAndDestroy(sound1);

	// add a small picture in position 134
	CEmsPictureIE* pic2 = EmsTestUtils::CreatePictureL(0);
	CleanupStack::PushL(pic2);
	pic2->SetStartPosition(134);
	sendMessage->AddEMSInformationElementL(*pic2);
	CleanupStack::PopAndDestroy(pic2);

	// add text formating in position 140, 12 char long
	CEmsFormatIE* format1 = CEmsFormatIE::NewL();
	format1->SetStartPosition(140);
	format1->SetFormatLength(12);
	format1->SetBold(ETrue);
	CleanupStack::PushL(format1);
	sendMessage->AddEMSInformationElementL(*format1);
	CleanupStack::PopAndDestroy(format1);

	// now send and receive the EMS message
	SendSmsL(sendMessage,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage);

	CompareEmsElementsL(*sendMessage, *recvMessage);

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::SendRecvEMSOn8BitPortL()
/**
 *  @test Send and Receive EMS message using 8-Bit port addressing
 *  Use t_editdb to setup the service centre number in the right modem record,
 *  otherwise this case will fail.
 */
{
    INFO_PRINTF1(_L("Send and receive EMS using 8-Bit port addressing "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	RSocket wapSocket;
	TSmsAddr addr8;
	addr8.SetSmsAddrFamily(ESmsAddrApplication8BitPort);
	TWapAddr wapAddr8;

	TInt port8 = 245;
	TInt wapPort8 = 246;
	addr8.SetPort(port8);
	wapAddr8.SetWapPort(TWapPortNumber(wapPort8));
	TBuf8<100> buf8;
    buf8.Copy(iTelephoneNumber);
	wapAddr8.SetWapAddress(buf8);
	TInt ret = 0;
	// open sockets and bind them to ports
	ret = socket.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket.Bind(addr8);
	TEST(ret==KErrNone);
	ret = wapSocket.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = wapSocket.Bind(wapAddr8);
	TEST(ret==KErrNone);

	// create EMS message
	//TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* sendMessage=CreateSmsMessageL(KSoundMsg1,alphabet);
	CleanupStack::PushL(sendMessage);

	// add two sound objects
	CEmsPreDefSoundIE* object1 = CEmsPreDefSoundIE::NewL( CEmsPreDefSoundIE::EChordLow);
	CleanupStack::PushL(object1);
	object1->SetStartPosition(17);
	sendMessage->AddEMSInformationElementL(*object1);
	CleanupStack::PopAndDestroy(object1);

	CEmsPreDefSoundIE* object2 = CEmsPreDefSoundIE::NewL(CEmsPreDefSoundIE::ETaDa);
	CleanupStack::PushL(object2);
	object2->SetStartPosition(33);
	sendMessage->AddEMSInformationElementL(*object2);
	CleanupStack::PopAndDestroy(object2);

	// set SMS port addresing for 8 Bit ports
	CSmsPDU& pdu = sendMessage->SmsPDU();
	pdu.SetApplicationPortAddressingL(ETrue, port8, port8, EFalse);

	// send EMS message
	SendSmsL(sendMessage,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage);

	//compare send and received messages
	CompareEmsElementsL(*sendMessage, *recvMessage);

	// now send a message to the wap socket
	TWapAddr recvWapAddr;
	TBuf8<256> recvBuf;
	TRequestStatus recvStatus;
	wapSocket.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	TRequestStatus status;
	wapSocket.SendTo(KWapTestMsg,wapAddr8,0,status);
	User::WaitForRequest(status);
	TEST(status.Int()==KErrNone);

	User::WaitForRequest(recvStatus);

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);
	socket.Close();
	wapSocket.Close();
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::SendRecvEMSOn16BitPortL()
/*
 * @test Send and Receive EMS message using 16-Bit port addressing
 * Use t_editdb to setup the service centre number in the right modem record,
 * otherwise this case will fail.
 */
{
    INFO_PRINTF1(_L("Send and receive EMS using 16-Bit port addressing "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	RSocket wapSocket;
	TSmsAddr addr16;
	addr16.SetSmsAddrFamily(ESmsAddrApplication16BitPort);
	TWapAddr wapAddr16;

	TInt port16 = 16001;
	TInt wapPort16 = 16002;
	addr16.SetPort(port16);
	wapAddr16.SetWapPort(TWapPortNumber(wapPort16));
	TBuf8<100> buf8;
    buf8.Copy(iTelephoneNumber);
	wapAddr16.SetWapAddress(buf8);
	TInt ret = 0;
	// open sockets and bind them to ports
	ret = socket.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket.Bind(addr16);
	TEST(ret==KErrNone);
	ret = wapSocket.Open(socketServer,KWAPSMSAddrFamily,KSockDatagram,KWAPSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = wapSocket.Bind(wapAddr16);
	TEST(ret==KErrNone);

	// create EMS message
	//TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
	CSmsMessage* sendMessage=CreateSmsMessageL(KSoundMsg1,alphabet);
	CleanupStack::PushL(sendMessage);

	// add two sound objects
	CEmsPreDefSoundIE* object1 = CEmsPreDefSoundIE::NewL( CEmsPreDefSoundIE::EChordLow);
	CleanupStack::PushL(object1);
	object1->SetStartPosition(17);
	sendMessage->AddEMSInformationElementL(*object1);
	CleanupStack::PopAndDestroy(object1);

	CEmsPreDefSoundIE* object2 = CEmsPreDefSoundIE::NewL(CEmsPreDefSoundIE::ETaDa);
	CleanupStack::PushL(object2);
	object2->SetStartPosition(33);
	sendMessage->AddEMSInformationElementL(*object2);
	CleanupStack::PopAndDestroy(object2);

	// set SMS port addresing for 8 Bit ports
	CSmsPDU& pdu = sendMessage->SmsPDU();
	pdu.SetApplicationPortAddressingL(ETrue, port16, port16, ETrue);

	// send EMS message
	SendSmsL(sendMessage,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage);

	//compare send and received messages
	CompareEmsElementsL(*sendMessage, *recvMessage);

	// now send a message to the wap socket
	TWapAddr recvWapAddr;
	TBuf8<256> recvBuf;
	TRequestStatus recvStatus;
	wapSocket.RecvFrom(recvBuf,recvWapAddr,0,recvStatus);

	TRequestStatus status;
	wapSocket.SendTo(KWapTestMsg,wapAddr16,0,status);
	User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);

	User::WaitForRequest(recvStatus);

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);
	socket.Close();
	wapSocket.Close();
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::TestFormattingSegmentationL(void)
/*
 *	Test correct text format segmentation upon send and receive.
 *  Format length goes beyond text, therefore, received FormatLength
 *  will not be the same as the sent one.
 */
{
	INFO_PRINTF1(_L("Send/Receive EMS with correct formatted segments"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// send/receive EMS message with format stretching over 3 PDUs, 8 Bit
	INFO_PRINTF1(_L("Sending 8-Bit EMS with format stretching over 3 PDUs "));
	CSmsMessage* sendMessage1 = CreateSmsMessageL(KTestEmsMsg8Bit1,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(sendMessage1);

	CEmsFormatIE* object1 = CEmsFormatIE::NewL();
	object1->SetStartPosition(10);
	object1->SetFormatLength(261);	//271, gose beyond message
	object1->SetBold(ETrue);
	CleanupStack::PushL(object1);
	sendMessage1->AddEMSInformationElementL(*object1);
	CleanupStack::PopAndDestroy(object1);

	SendSmsL(sendMessage1,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage1 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage1);

	// compare sent and received
	CompareEmsElementsL(*sendMessage1, *recvMessage1);

	CleanupStack::PopAndDestroy(recvMessage1);
	CleanupStack::PopAndDestroy(sendMessage1);

	// send/receive EMS message with format stretching over 2 PDUs, 8 Bit
	INFO_PRINTF1(_L("Sending 8-Bit EMS with format stretching over 2 PDUs "));
	CSmsMessage* sendMessage2 = CreateSmsMessageL(KTestEmsMsg8Bit2,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(sendMessage2);

	CEmsFormatIE* object2 = CEmsFormatIE::NewL();
	object2->SetStartPosition(10);
	object2->SetFormatLength(261);	//271 goes beyond message
	object2->SetBold(ETrue);
	CleanupStack::PushL(object2);
	sendMessage2->AddEMSInformationElementL(*object2);
	CleanupStack::PopAndDestroy(object2);

	SendSmsL(sendMessage2,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage2 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage2);

	// compare sent and received
	CompareEmsElementsL(*sendMessage2, *recvMessage2);

	CleanupStack::PopAndDestroy(recvMessage2);
	CleanupStack::PopAndDestroy(sendMessage2);

	// send/receive EMS message with format stretching over middle PDU, 8 Bit
	INFO_PRINTF1(_L("Sending 8-Bit EMS with format stretching over middle PDU "));
	CSmsMessage* sendMessage3 = CreateSmsMessageL(KTestEmsMsg8Bit3,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(sendMessage3);

	CEmsFormatIE* object3 = CEmsFormatIE::NewL();
	object3->SetStartPosition(134);
	object3->SetFormatLength(129);				//263 withen message
	object3->SetBold(ETrue);
	CleanupStack::PushL(object3);
	sendMessage3->AddEMSInformationElementL(*object3);
	CleanupStack::PopAndDestroy(object3);

	SendSmsL(sendMessage3,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage3 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage3);

	// compare sent and received
	CompareEmsElementsL(*sendMessage3, *recvMessage3);

	CleanupStack::PopAndDestroy(recvMessage3);
	CleanupStack::PopAndDestroy(sendMessage3);

	// send/receive EMS message with format stretching over middle PDU, 16 Bit
	INFO_PRINTF1(_L("Sending 16-Bit EMS with format stretching over 3 PDUs "));
	CSmsMessage* sendMessage4 = CreateSmsMessageL(KTestEmsMsg16Bit1,TSmsDataCodingScheme::ESmsAlphabetUCS2);
	CleanupStack::PushL(sendMessage4);

	CEmsFormatIE* object4 = CEmsFormatIE::NewL();
	object4->SetStartPosition(10);
	object4->SetFormatLength(261);	//271 goes beyond message
	object4->SetBold(ETrue);
	CleanupStack::PushL(object4);
	sendMessage4->AddEMSInformationElementL(*object4);
	CleanupStack::PopAndDestroy(object4);

	SendSmsL(sendMessage4,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage4 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage4);

	// compare sent and received
	CompareEmsElementsL(*sendMessage4, *recvMessage4);

	CleanupStack::PopAndDestroy(recvMessage4);
	CleanupStack::PopAndDestroy(sendMessage4);

	// Test correct coding of formatted text, regression for LEE-5AEF7Z
	INFO_PRINTF1(_L("Sending long format that contain a picture object "));
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* sendMessage5 = CreateSmsMessageL(KLongFormatMsg,alphabet);
	CleanupStack::PushL(sendMessage5);

	CEmsFormatIE* object5 = CEmsFormatIE::NewL();
	CleanupStack::PushL(object5);

	object5->SetStartPosition(10);
	object5->SetFormatLength(100);	//110 withen message
	object5->SetBold(ETrue);
	object5->SetItalic(ETrue);
	object5->SetUnderline(ETrue);
	object5->SetStrikethrough(ETrue);

	sendMessage5->AddEMSInformationElementL(*object5);
	CleanupStack::PopAndDestroy(object5);

	CEmsPictureIE* object6 = EmsTestUtils::CreatePictureL(1);
	CleanupStack::PushL(object6);
	object6->SetStartPosition(20);

	sendMessage5->AddEMSInformationElementL(*object6);
	CleanupStack::PopAndDestroy(object6);

	SendSmsL(sendMessage5,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage5 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage5);

	// compare sent and received
	CompareEmsElementsL(*sendMessage5, *recvMessage5);

	CleanupStack::PopAndDestroy(recvMessage5);
	CleanupStack::PopAndDestroy(sendMessage5);

	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::TestNumberOfMessagesL(void)
/*
 *	Test correct segmentation of EMS messages upon send and receive.
 */
	{
	INFO_PRINTF1(_L("Send/Receive EMS with correct number of segments"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	// send/receive 8-Bit EMS message with 2 PDU.
	INFO_PRINTF1(_L("Sending 8-Bit EMS with 2 PDUs "));

	CSmsMessage* sendMessage1 = CreateSmsMessageL(KTest2PduMsg8Bit,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(sendMessage1);

	CEmsFormatIE* object1 = CEmsFormatIE::NewL();
	object1->SetStartPosition(100);
	object1->SetFormatLength(10);
	object1->SetBold(ETrue);
	CleanupStack::PushL(object1);
	sendMessage1->AddEMSInformationElementL(*object1);
	CleanupStack::PopAndDestroy(object1);

	SendSmsL(sendMessage1,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage1 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage1);

	// compare sent and received
	CompareEmsElementsL(*sendMessage1, *recvMessage1);

	CleanupStack::PopAndDestroy(recvMessage1);
	CleanupStack::PopAndDestroy(sendMessage1);

	// send/receive 7-Bit EMS message with 2 PDU.
	INFO_PRINTF1(_L("Sending 7-Bit EMS with 2 PDUs "));
	CSmsMessage* sendMessage2 = CreateSmsMessageL(KTest2PduMsg7Bit,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(sendMessage2);

	CEmsFormatIE* object2 = CEmsFormatIE::NewL();
	object2->SetStartPosition(100);
	object2->SetFormatLength(10);
	object2->SetBold(ETrue);
	CleanupStack::PushL(object2);
	sendMessage2->AddEMSInformationElementL(*object2);
	CleanupStack::PopAndDestroy(object2);

	SendSmsL(sendMessage2,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage2 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage2);

	// compare sent and received
	CompareEmsElementsL(*sendMessage2, *recvMessage2);

	CleanupStack::PopAndDestroy(recvMessage2);
	CleanupStack::PopAndDestroy(sendMessage2);

	// send/receive 16-Bit EMS message with 2 PDU.
	INFO_PRINTF1(_L("Sending 16-Bit EMS with 2 PDUs "));
	CSmsMessage* sendMessage3 = CreateSmsMessageL(KTest2PduMsg16Bit,TSmsDataCodingScheme::ESmsAlphabetUCS2);
	CleanupStack::PushL(sendMessage3);

	CEmsFormatIE* object3 = CEmsFormatIE::NewL();
	object3->SetStartPosition(40);
	object3->SetFormatLength(10);
	object3->SetBold(ETrue);
	CleanupStack::PushL(object3);
	sendMessage3->AddEMSInformationElementL(*object3);
	CleanupStack::PopAndDestroy(object3);

	SendSmsL(sendMessage3,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage3 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage3);

	// compare sent and received
	CompareEmsElementsL(*sendMessage3, *recvMessage3);

	CleanupStack::PopAndDestroy(recvMessage3);
	CleanupStack::PopAndDestroy(sendMessage3);

	// send/receive 8-Bit empty text EMS message with no IEs
	INFO_PRINTF1(_L("Sending 8-Bit empty text EMS with no IEs "));

	CSmsMessage* sendMessage4 = CreateSmsMessageL(KTestEmptyMsg8Bit,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(sendMessage4);

	SendSmsL(sendMessage4,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage4 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage4);

	// compare sent and received
	CompareEmsElementsL(*sendMessage4, *recvMessage4);

	CleanupStack::PopAndDestroy(recvMessage4);
	CleanupStack::PopAndDestroy(sendMessage4);

	// send/receive 8-Bit empty text EMS message with IE pointing to the empty text
	INFO_PRINTF1(_L("Sending 8-Bit EMS with IE points to empty text "));

	CSmsMessage* sendMessage5 = CreateSmsMessageL(KTestEmptyMsg8Bit,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(sendMessage5);

	CEmsFormatIE* object5 = CEmsFormatIE::NewL();
	object5->SetStartPosition(0);
	object5->SetFormatLength(0);
	object5->SetBold(ETrue);
	CleanupStack::PushL(object5);
	sendMessage5->AddEMSInformationElementL(*object5);
	CleanupStack::PopAndDestroy(object5);

	SendSmsL(sendMessage5,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage5 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage5);

	// compare sent and received
	CompareEmsElementsL(*sendMessage5, *recvMessage5);

	CleanupStack::PopAndDestroy(recvMessage5);
	CleanupStack::PopAndDestroy(sendMessage5);

	// send/receive 7-Bit EMS with 2 PDU and extended char at boundary
	INFO_PRINTF1(_L("Sending EMS with extended char at boundary "));

	TInt bufLen = KTestExtdMsg7Bit1().Length();
	HBufC* textBuf1=KTestExtdMsg7Bit1().AllocLC();
	TPtr textPtr1(textBuf1->Des());
	textPtr1[bufLen - 1] = 0x20ac;		//Unicode Euro sign

	CSmsMessage* sendMessage6 = CreateSmsMessageL(*textBuf1,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(sendMessage6);

	CEmsFormatIE* object6 = CEmsFormatIE::NewL();
	object6->SetStartPosition(100);
	object6->SetFormatLength(10);
	object6->SetBold(ETrue);
	CleanupStack::PushL(object6);
	sendMessage6->AddEMSInformationElementL(*object6);
	CleanupStack::PopAndDestroy(object6);

	SendSmsL(sendMessage6,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage6 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage6);

	// compare sent and received
	CompareEmsElementsL(*sendMessage6, *recvMessage6);

	CleanupStack::PopAndDestroy(recvMessage6);
	CleanupStack::PopAndDestroy(sendMessage6);
	CleanupStack::PopAndDestroy(textBuf1);

	// send/receive 7-Bit EMS with 2 PDU and IE pointing to extended char at boundary
	INFO_PRINTF1(_L("Sending EMS with IE pointing to extended char at boundary "));

	bufLen = KTestExtdMsg7Bit2().Length();
	HBufC* textBuf2=KTestExtdMsg7Bit2().AllocLC();
	TPtr textPtr2(textBuf2->Des());
	for(TInt i=1;i<=8;i++)
		textPtr2[bufLen - i] = 0x20ac;

	CSmsMessage* sendMessage7 = CreateSmsMessageL(*textBuf2,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(sendMessage7);

	CEmsFormatIE* object7 = CEmsFormatIE::NewL();
	object7->SetStartPosition(153);
	object7->SetFormatLength(2);
	object7->SetBold(ETrue);
	CleanupStack::PushL(object7);
	sendMessage7->AddEMSInformationElementL(*object7);
	CleanupStack::PopAndDestroy(object7);

	SendSmsL(sendMessage7,socket);

	WaitForRecvL(socket);
	CSmsMessage* recvMessage7 = RecvSmsL(socket);
	CleanupStack::PushL(recvMessage7);

	// compare sent and received
	CompareEmsElementsL(*sendMessage7, *recvMessage7);

	CleanupStack::PopAndDestroy(recvMessage7);
	CleanupStack::PopAndDestroy(sendMessage7);
	CleanupStack::PopAndDestroy(textBuf2);

	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
}

void CSmsInterTestStep::EnumerateReadWriteAndDeleteEMSOnSIML(void)
{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
	{
		//run the test
		found = DoSingleTestCaseL( KEnumerateReadWriteAndDeleteEMSOnSIM, count++ );
	}
}

void CSmsInterTestStep::EnumerateReadWriteAndDeleteEMSOnPhoneL(void)
{
/*
    if( gAutoTest->KeepGoing() )
        return;
*/
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
	{
		//run the test
		found = DoSingleTestCaseL( KEnumerateReadWriteAndDeleteEMSOnPhone, count++ );
	}
}

CSmsMessage* CSmsInterTestStep::CreateAndSetMessageL( const RPointerArray<CSmsPduDbMessage>& aArray, TInt aArrayIndex )
	{
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;

	CSmsBufferBase& smsBuffer=aArray[aArrayIndex]->iSmsMessage->Buffer();
	TInt bufLen=smsBuffer.Length();
	HBufC* textBuf=HBufC::NewL(bufLen);
	CleanupStack::PushL(textBuf);
	TPtr buffer(textBuf->Des());
	smsBuffer.Extract(buffer,0,bufLen);

	CSmsMessage* smsMessage=CreateSmsMessageL(buffer,alphabet,CSmsPDU::ESmsDeliver);
	CleanupStack::PopAndDestroy(textBuf);

	smsMessage->SetStorage(CSmsMessage::ESmsSIMStorage);
	return smsMessage;
	} // CSmsInterTestStep::CreateAndSetMessageL


void CSmsInterTestStep::TestSendAndReceive7bitMsgsL( TPtrC8 aPdu,
										  const RPointerArray<CSmsPduDbMessage>& aArray,
										  const CArrayFixFlat<TInt>* aCharArray,
										  const CArrayFixFlat<TInt>* aMessageLengthArray )
/**
 *  @test Send and Receive 7-bit SMS messages with different message lengths
 */
	{
    INFO_PRINTF1(_L("Send and Receive SMS messages with different message lengths"));
	INFO_PRINTF1(_L("GT83-T_SmsSendReceive-006"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	TInt bufLen=aArray[0]->iSmsMessage->Buffer().Length();
	HBufC* textBuf=HBufC::NewL(bufLen);
	CleanupStack::PushL(textBuf);
	TPtr textPtr(textBuf->Des());
	aArray[0]->iSmsMessage->Buffer().Extract(textPtr,0,bufLen);

	SendAndRecvSms7BitL( textPtr, socket );
	CleanupStack::PopAndDestroy(textBuf);

	for ( TInt i = 0; i < aMessageLengthArray->Count(); i++ )
		{
		TInt length = aMessageLengthArray->At(i);

		SendAndRecvSms7BitL(length, socket);

		}

    // Test extended characters
	TInt charCount = aCharArray->Count();

	bufLen=aArray[1]->iSmsMessage->Buffer().Length();
	bufLen+= charCount;
	HBufC* textBuf2=HBufC::NewL(bufLen);
	CleanupStack::PushL(textBuf2);
	TPtr textPtr2(textBuf2->Des());
	aArray[1]->iSmsMessage->Buffer().Extract(textPtr2,0,bufLen-( charCount * 2 ) );

	for ( TInt j = 0; j < charCount; j++ )
		textPtr2.Append(aCharArray->At(j) );

	SendAndRecvSms7BitL( textPtr2, socket );
	CleanupStack::PopAndDestroy(textBuf2);

    // Next tests extended character split across PDU boundary
    INFO_PRINTF1(_L("Sending message with extended char split over PDU boundary"));

	TInt pduLength = aPdu.Length();
	TBuf16<165> buffer;

    for (TInt k = 0; k < pduLength; k += 1 )
		buffer.Append( TUint16( aPdu[k] ) );

    SendAndRecvSms7BitL( buffer , socket );

	CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
	}


void CSmsInterTestStep::TestSendAndReceive8bitMsgsL( const RPointerArray<CSmsPduDbMessage>& aArray,
										  const CArrayFixFlat<TInt>* aMessageLengthArray )
/**
 *  @test Send and Recieve 8bit messages (no UDH information)
 */
    {
    INFO_PRINTF1(_L("Send and Receive 8bit messages"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	TInt bufLen=aArray[0]->iSmsMessage->Buffer().Length();
	HBufC* textBuf=HBufC::NewL(bufLen);
	CleanupStack::PushL(textBuf);
	TPtr textPtr(textBuf->Des());
	aArray[0]->iSmsMessage->Buffer().Extract(textPtr,0,bufLen);

    INFO_PRINTF1( _L( "Sending message: ") );
    INFO_PRINTF1( textPtr );
	SendAndRecvSms8BitL( textPtr, socket );
	CleanupStack::PopAndDestroy(textBuf);

	for ( TInt i = 0; i < aMessageLengthArray->Count(); i++ )
		{
		TInt length = aMessageLengthArray->At(i);

	    INFO_PRINTF2(_L("Sending %d byte message"), length);
		SendAndRecvSms8BitL(length, socket);

		}

	CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestSendAndReceive16bitMsgsL( const RPointerArray<CSmsPduDbMessage>& aArray,
										   const CArrayFixFlat<TInt>* aMessageLengthArray )
/**
 *  @test Send and Recieve 16bit messages (no UDH information)
 */
    {
    INFO_PRINTF1(_L("Send and Receive 16bit messages"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	TInt bufLen=aArray[0]->iSmsMessage->Buffer().Length();
	HBufC* textBuf=HBufC::NewL(bufLen);
	CleanupStack::PushL(textBuf);
	TPtr textPtr(textBuf->Des());
	aArray[0]->iSmsMessage->Buffer().Extract(textPtr,0,bufLen);

    INFO_PRINTF1( _L( "Sending message: ") );
    INFO_PRINTF1( textPtr );
	SendAndRecvSms16BitL( textPtr, socket );
	CleanupStack::PopAndDestroy(textBuf);

	for ( TInt i = 0; i < aMessageLengthArray->Count(); i++ )
		{
		TInt length = aMessageLengthArray->At(i);

	    INFO_PRINTF2(_L("Sending %d byte message"), length);
		SendAndRecvSms16BitL(length, socket);

		}

	CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestSendAndReceiveConcatenatedMsgsL( const RPointerArray<CSmsPduDbMessage>& aArray,
												  const CArrayFixFlat<TInt>* aAlphabetArray )
/**
 *  Test transmition and reception of a concatenated SMS message spanning 3 TPDUs
 */
	{
	INFO_PRINTF1(_L("Test Tx and Rx SMS with different alphabet types "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	for ( TInt i = 0; i < aAlphabetArray->Count(); i++ )
		{

		TSmsDataCodingScheme::TSmsAlphabet alphabet = ( TSmsDataCodingScheme::TSmsAlphabet )aAlphabetArray->At(i);

		TInt bufLen=aArray[i]->iSmsMessage->Buffer().Length();
		HBufC* textBuf=HBufC::NewL(bufLen);
		CleanupStack::PushL(textBuf);
		TPtr testText(textBuf->Des());
		aArray[i]->iSmsMessage->Buffer().Extract(testText,0,bufLen);

		CSmsMessage* smsMessage;
		if ( i == 0 )
			smsMessage=CreateSmsWithStatusReportReqL(testText,alphabet);
		else
			smsMessage=CreateSmsMessageL(testText,alphabet);

		CleanupStack::PushL(smsMessage);
		SendSmsL(smsMessage,socket);
		CleanupStack::PopAndDestroy(smsMessage);

		WaitForRecvL(socket);
		smsMessage = RecvSmsL(socket);
		CleanupStack::PushL(smsMessage);
		
		TestSmsContentsL(smsMessage,testText);
		CleanupStack::PopAndDestroy(smsMessage);

		CleanupStack::PopAndDestroy(textBuf);
		}

	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
	}

void CSmsInterTestStep::TestStringMatchingIncomingMsgL()
/**
 *  @test Test string matching incoming Sms
 */
    {
    INFO_PRINTF1(_L("Test string matching of incoming Sms Messages"));

//Once all the tests are fixed try uncommenting this and run the test

    /*TTestCase tests[] =
    {
        TTestCase( _L("Message String"),         _L("Message String")),
        TTestCase( _L("AABB: TestMessage1"),     _L("AABB:") ),
        TTestCase( _L("AAB: Another test"),      _L("AAB:") ),
        TTestCase( _L("xyqwerty") ),             // Recv any
        TTestCase( _L("xyzuiop"),                _L("xyz") ),
        TTestCase( _L("AAA  abcdefg"),           _L("aa") ),
        TTestCase( _L("Voicemail Indication"),   TSmsDataCodingScheme::ESmsVoicemailMessageWaiting,
                                               TSmsDataCodingScheme::ESmsDCSMessageWaitingIndication7Bit,
											   TSmsDataCodingScheme::ESmsIndicationActive)
	};*/

    TTestCase tests[] =
       {
           TTestCase( _L("Message String"),         _L("Message String")),
           TTestCase( _L("AABB: TestMessage1"),     _L("AABB:") ),
           TTestCase( _L("AAB: Another test"),      _L("AAB:") ),
           TTestCase( _L("xyzuiop"),                _L("xyz") ),
           TTestCase( _L("AAA  abcdefg"),           _L("aa") ),
           
       };
	RSocketServ socketServer;
    Connect2EsockLC(socketServer);

	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrSendOnly);

    const TInt numTests = sizeof(tests)/sizeof(TTestCase);
    CActiveReceiver* msgReceiver = CActiveReceiver::NewL(socketServer, iFs);
    CleanupStack::PushL(msgReceiver);

	msgReceiver->StopIfKeyPressedL();
    for (TInt i=0; i<numTests; i++)
        msgReceiver->AddTestL(tests[i]);
    for (TInt j=0; j<numTests; j++)
        {
        SendTestMessageL(tests[j],socket);
        User::After(30000000);
        }
    msgReceiver->StartTestsL();
    CleanupStack::PopAndDestroy();

	CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestSendAndReceiveDifferentIEIsL( const RPointerArray<CSmsPduDbMessage>& aArray,
											   const CArrayFixFlat<TInt>* aAlphabetArray,
											   const CArrayFixFlat<TInt>* aInformationElementArray,
											   const CArrayFixFlat<TInt>* aIeiDataArray )
/**
 *  @test Send and receive messages of different IEIs
 *  TODO - 6210 doesn't route incoming ESmsIEIApplicationPortAddressing8Bit and
 *  ESmsIEIApplicationPortAddressing16Bit messages to the MMTSY.
 */
    {
    INFO_PRINTF1(_L("Send and receive messages with different IEIs"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	for ( TInt i = 0; i < aArray.Count(); i++ )
		{

		TSmsDataCodingScheme::TSmsAlphabet alphabet = ( TSmsDataCodingScheme::TSmsAlphabet )aAlphabetArray->At(i);
		CSmsInformationElement::TSmsInformationElementIdentifier iei = ( CSmsInformationElement::TSmsInformationElementIdentifier )aInformationElementArray->At(i);

		TBuf8<20> ieiData;
		TInt testCaseNum(0), count(0);
		while ( testCaseNum < i )
			if ( aIeiDataArray->At(count++) == KErrNotFound )
				testCaseNum++;

		while ( aIeiDataArray->At(count) != KErrNotFound )
			ieiData.Append(aIeiDataArray->At(count++));

		TInt bufLen=aArray[i]->iSmsMessage->Buffer().Length();
		HBufC* textBuf=HBufC::NewL(bufLen);
		CleanupStack::PushL(textBuf);
		TPtr testText(textBuf->Des());
		aArray[i]->iSmsMessage->Buffer().Extract(testText,0,bufLen);

		SendAndRecvAndCompareMessageIEIL( testText, ieiData, socket, alphabet, iei );

		CleanupStack::PopAndDestroy(textBuf);
		}

    CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestSendAndReceiveBinaryDataL( const CArrayFixFlat<TInt>* aMessageLengthArray )
/**
 *  @test Send and Recieve binary data (+ UDH information)
 *  TODO These tests doesn't work with 6210. It is possible to send message, but
 *  phone won't route incoming message to the MMTSY.
 */
    {
    INFO_PRINTF1(_L("Send and Receive binary data"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	for ( TInt i = 0; i < aMessageLengthArray->Count(); i++ )
		{

		TInt messageLength = aMessageLengthArray->At(i);
	    INFO_PRINTF2(_L("Sending %d byte message"),messageLength);
	    SendAndRecvSmsBinaryL(messageLength,socket);

		}

    CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
	}

void CSmsInterTestStep::TestOOML()
/**
 *  Low memory testing
 */
    {
#if defined(_DEBUG)
    INFO_PRINTF1(_L("Low memory tests"));

	RSocketServ debugSocketServer;
	TInt ret=debugSocketServer.Connect(KSocketMessageSlots);
	TEST(ret==KErrNone);
	CleanupClosePushL(debugSocketServer);

	debugSocketServer.__DbgMarkHeap();


	RSocketServ socketServer;
    Connect2EsockLC(socketServer);

	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    TInt count=0;
    ret=KErrNoMemory;
    while (ret==KErrNoMemory  || count < 7)
        {
        INFO_PRINTF2(_L("Creating message ... No. of allocations = %d"),count);
		debugSocketServer.__DbgFailNext(count);
        CSmsMessage* sms=NULL;
        TRAP(ret,sms=CreateSmsMessageL(_L("Test message"),TSmsDataCodingScheme::ESmsAlphabet7Bit));
        delete sms;
        count++;
        }

    INFO_PRINTF1(_L(""));
    debugSocketServer.__DbgFailNext(-1); // Reset heap


    count=0;
    ret=KErrNoMemory;
    while (ret==KErrNoMemory  || count < 7)
        {
        INFO_PRINTF2(_L("Creating message ... No. of allocations = %d"),count);
		debugSocketServer.__DbgFailNext(count);
        TRAP(ret,SendAndRecvSms7BitL(_L("x"),socket););
        count++;
        }

    INFO_PRINTF1(_L(""));
    debugSocketServer.__DbgFailNext(-1); // Reset heap

	CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close

	debugSocketServer.__DbgMarkEnd(0);

	CleanupStack::PopAndDestroy(1); //debugSocketServer


#else
    INFO_PRINTF1(_L("No low memory tests in release builds"));
#endif
    }

void CSmsInterTestStep::TestSmsWapEmailNotificationL(void)
/**
 *  @test Test Email notification according to Smart Messaging
 *  spec 2.0.0
 */
    {
    _LIT(KEmailNot, "//SCKL1588\n42");

    INFO_PRINTF1(_L("test Email Notification"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    //
    // Tx
    //
    INFO_PRINTF1(_L("sending Email Notification "));
    CSmsMessage* smsMessage=CreateSmsMessageL(KEmailNot, TSmsDataCodingScheme::ESmsAlphabet7Bit);
    CleanupStack::PushL(smsMessage);
    SendSmsL(smsMessage,socket);
    CleanupStack::PopAndDestroy(smsMessage);

    //
    // Rx
    //
    INFO_PRINTF1(_L("waiting for incoming WAP SMS"));
    WaitForRecvL(socket);
    smsMessage = RecvSmsL(socket);
    CleanupStack::PushL(smsMessage);
    CSmsBuffer& smsbuffer = (CSmsBuffer&)smsMessage->Buffer();
    TInt len = smsbuffer.Length();
    HBufC* hbuf = HBufC::NewL(len);
    TPtr ptr = hbuf->Des();
    smsbuffer.Extract(ptr,0,len);
    INFO_PRINTF2(_L("SMS contains %S"),&ptr);
    delete hbuf;
    CleanupStack::PopAndDestroy(smsMessage);

    //
    // cleanup
    //
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    INFO_PRINTF1(_L("Done"));
    }

void CSmsInterTestStep::TestSendAndReceive159bytesMsgL()
/**
 *  @test Send and Receive 7-bit SMS message with 159 characters
 */
    {
    INFO_PRINTF1(_L("Sending 159 byte message"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    SendAndRecvSms7BitL(159,socket);
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestSendAndReceive1byteMsgL()
/**
 *  @test Send and Receive 7-bit SMS message with 1 characters
 *  TODO this is only a placeholder at the moment to
 *  keep the numbering of the test cases because
 *  SendSMS2Port0000L has been moved to the manual test code
 *  This method can be removed later
 */
    {
    INFO_PRINTF1(_L("Sending 1 byte message"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    SendAndRecvSms7BitL(1,socket);
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestSocketConnectTwiceL()
/**
 *  @test connecting to the sockets server twice
 *  
 *  added after problems reported with NBS watchers and Esock's
 *  RSocketServ::Connect hanging the second time.
 */
    {
    INFO_PRINTF1(_L("connecting to the sockets server twice") );

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

    TInt ret;
    INFO_PRINTF1(_L("socket 1...") );
    RSocketServ serv1;
    ret = serv1.Connect();
    TEST(ret==KErrNone);
    RSocket socket1;
    ret = socket1.Open(serv1, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
    TEST(ret==KErrNone);
    TSmsAddr smsaddr;
    smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
    ret = socket1.Bind(smsaddr);
    TEST(ret==KErrNone);
    socket1.Close();
    serv1.Close();
    INFO_PRINTF1(_L("Done") );

    INFO_PRINTF1(_L("socket 2...") );
    RSocketServ serv2;
    ret = serv2.Connect();
    TEST(ret==KErrNone);
    RSocket socket2;
    ret = socket2.Open(serv2, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
    TEST(ret==KErrNone);
    smsaddr.SetSmsAddrFamily(ESmsAddrRecvAny);
    ret = socket2.Bind(smsaddr);
    TEST(ret==KErrNone);
    socket2.Close();
    serv2.Close();
    INFO_PRINTF1(_L("Done") );
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestSocketBindingL()
/**
 *  Test opening and binding the socket to different SMS Address types
 */
	{
	INFO_PRINTF1(_L("Test Open & Bind the Socket"));

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	RSocket socket1;
	RSocket socket2;
	TInt ret1=socket1.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TEST(ret1 == KErrNone);
	CleanupClosePushL(socket1);
	TSmsAddr smsaddr1;

	TInt ret2=socket2.Open(socketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TEST(ret2 == KErrNone);
	CleanupClosePushL(socket2);
	TSmsAddr smsaddr2;

	//ESmsAddrUnbound
	smsaddr1.SetSmsAddrFamily(ESmsAddrUnbound);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

	smsaddr2.SetSmsAddrFamily(ESmsAddrUnbound);
	ret2=socket2.Bind(smsaddr2);
	TEST(ret2 == KErrNone);

	//ESmsAddrSendOnly
	smsaddr1.SetSmsAddrFamily(ESmsAddrSendOnly);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

	smsaddr2.SetSmsAddrFamily(ESmsAddrSendOnly);
	ret2=socket1.Bind(smsaddr2);
	TEST(ret2 == KErrNone);

	//ESmsAddrMessageIndication
	smsaddr1.SetSmsAddrFamily(ESmsAddrMessageIndication);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

//  Only one client could bind to ESmsAddrMessageIndication at a time
//  This address type is returned with KErrAlreadyExists
	smsaddr2.SetSmsAddrFamily(ESmsAddrMessageIndication);
	ret2=socket2.Bind(smsaddr2);
	TEST(ret2==KErrAlreadyExists);

	//ESmsAddrMatchIEI
	smsaddr1.SetSmsAddrFamily(ESmsAddrMatchIEI);
	smsaddr1.SetIdentifierMatch(CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

	smsaddr2.SetSmsAddrFamily(ESmsAddrMatchIEI);
	smsaddr2.SetIdentifierMatch(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication);
	ret2=socket1.Bind(smsaddr2);
	TEST(ret2 == KErrNone);

	//ESmsAddrMatchText
	smsaddr1.SetSmsAddrFamily(ESmsAddrMatchText);
	smsaddr1.SetTextMatch(_L8("test message, len"));
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

	smsaddr2.SetSmsAddrFamily(ESmsAddrMatchText);
	smsaddr2.SetTextMatch(_L8("test message,"));
	ret2=socket1.Bind(smsaddr2);
	TEST(ret2 == KErrNone);

	//ESmsAddrRecvAny
	smsaddr1.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

//  Only one client could bind to ESmsAddrRecvAny at a time
//  This address type is returned with KErrAlreadyExists
	smsaddr2.SetSmsAddrFamily(ESmsAddrRecvAny);
	ret2=socket2.Bind(smsaddr2);
	TEST(ret2==KErrAlreadyExists);

	//ESmsAddrStatusReport
	smsaddr1.SetSmsAddrFamily(ESmsAddrStatusReport);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

//  Only one client could bind to ESmsAddrStatusReport at a time
//  This address type is returned with KErrAlreadyExists
	smsaddr2.SetSmsAddrFamily(ESmsAddrStatusReport);
	ret2=socket2.Bind(smsaddr2);
	TEST(ret2 == KErrAlreadyExists);

	//ESmsAddrLocalOperation
	smsaddr1.SetSmsAddrFamily(ESmsAddrLocalOperation);
	ret1=socket1.Bind(smsaddr1);
	TEST(ret1 == KErrNone);

	smsaddr2.SetSmsAddrFamily(ESmsAddrLocalOperation);
	ret2=socket2.Bind(smsaddr2);
	TEST(ret2 == KErrNone);

	CleanupStack::PopAndDestroy(&socket2);
	CleanupStack::PopAndDestroy(&socket1);

	INFO_PRINTF1(_L("All bindings ok!"));

    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
	}

void CSmsInterTestStep::TestEnumerateAndReceiveMessagesL()
/**
 *  @test Enumerate and receive the messages
 */
    {
    INFO_PRINTF1(_L("Enumeration"));
    INFO_PRINTF1(_L("Place SIM in phone that has one SMS message present"));

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    INFO_PRINTF1(_L("Starting enumeration ..."));
    TPckgBuf<TUint> sbuf;
    TRequestStatus status;
    socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
    User::WaitForRequest(status);
    TEST(status.Int() == KErrNone);
    TInt count=sbuf();
    INFO_PRINTF2(_L("Enumerated %d messages"),count);

    CSmsMessage* smsMessage = CreateSmsMessageL(_L(""),TSmsDataCodingScheme::ESmsAlphabet7Bit);
    CleanupStack::PushL(smsMessage);

    RSmsSocketReadStream readstream(socket);
    TPtr ptr(NULL,0);
    for (TInt i=0; i<count; i++)
        {
        TRAPD(ret,readstream >> *smsMessage);
        TEST(ret==KErrNone);
        socket.Ioctl(KIoctlReadMessageSucceeded,status,NULL, KSolSmsProv);
        User::WaitForRequest(status);
        TEST(status.Int() == KErrNone);


        INFO_PRINTF1(smsMessage->ToFromAddress());
		INFO_PRINTF1(_L("     "));
		INFO_PRINTF1(smsMessage->ServiceCenterAddress());
        INFO_PRINTF1(_L(""));

        CSmsBufferBase& buffer = smsMessage->Buffer();
        TInt length=buffer.Length();
        HBufC* buf=HBufC::New(length);
        TEST(buf != KErrNone);
        ptr.Set((TText*) buf->Des().Ptr(),length,length);
        buffer.Extract(ptr,0,length);
        TPtrC displayBuf(ptr.Mid(0,(length>80? 80: length)));
        INFO_PRINTF2(_L("%S"),&displayBuf);
        delete buf;
        }

    CleanupStack::PopAndDestroy(smsMessage);
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestEnumerateAndDeleteMessagesL()
/**
 *  @test Enumerate and delete all messages
 */
    {
    INFO_PRINTF1(_L("Enumerate and delete messages"));
    INFO_PRINTF1(_L("Enumeration"));

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    INFO_PRINTF1(_L("Starting enumeration ..."));
    TPckgBuf<TUint> sbuf;
    TRequestStatus status;
    socket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
    User::WaitForRequest(status);
    TEST(status.Int() == KErrNone);
    TInt count=sbuf();
    INFO_PRINTF2(_L("Enumerated %d messages"),count);

    CSmsMessage* smsMessage = CreateSmsMessageL(_L(""),TSmsDataCodingScheme::ESmsAlphabet7Bit);
    CleanupStack::PushL(smsMessage);

    RSmsSocketReadStream readstream(socket);
    TPtr ptr(NULL,0);
    for (TInt i=0; i<count; i++)
        {
        TRAPD(ret,readstream >> *smsMessage);
        TEST(ret==KErrNone);
        socket.Ioctl(KIoctlReadMessageSucceeded,status,NULL, KSolSmsProv);
        User::WaitForRequest(status);
        TEST(status.Int() == KErrNone);


        INFO_PRINTF1(smsMessage->ToFromAddress());
		INFO_PRINTF1(_L("     "));
		INFO_PRINTF1(smsMessage->ServiceCenterAddress());
        INFO_PRINTF1(_L(""));

        CSmsBufferBase& buffer = smsMessage->Buffer();
        TInt length=buffer.Length();
        HBufC* buf=HBufC::New(length);
        TEST(buf != KErrNone);
        ptr.Set((TText*) buf->Des().Ptr(),length,length);
        buffer.Extract(ptr,0,length);
        TPtrC displayBuf(ptr.Mid(0,(length>70? 70: length)));
        INFO_PRINTF2(_L("%S"),&displayBuf);

        INFO_PRINTF1(_L("Deleting message ..."));
        RSmsSocketWriteStream writestream(socket);
        writestream << *smsMessage;
        writestream.CommitL();
        socket.Ioctl(KIoctlDeleteSmsMessage,status,NULL, KSolSmsProv);
        User::WaitForRequest(status);
        TEST(status.Int() == KErrNone);

        delete buf;
        }

    CleanupStack::PopAndDestroy(smsMessage);
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestEnumerateAndWriteAndDeleteMessagesL( const RPointerArray<CSmsPduDbMessage>& aArray )
/**
 *  @test Switch on the phone with a SIM.
 *  Enumerate, delete (first message if exist and last message after writing),
 *  write (two messages to the sim)
 */
    {
	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	// Open the socket for SIM operations
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrLocalOperation);

	TInt ret = 0;
	CSmsMessage* smsmessage;
	
	// Enumerate messages
    INFO_PRINTF1(_L("Enumeration"));
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);
	TInt count = messages.Count();
	TInt index = 0;

	// Delete messages
    INFO_PRINTF1(_L("Delete messages"));
	while ( index < count )
		{
		smsmessage = messages[index++];

		INFO_PRINTF2(_L("Delete %d. message from the store "), index );
		ret = DeleteSmsL(*smsmessage, socket);
		TEST( !( ret == KErrNone ) );

		}
	messages.ResetAndDestroy();

	// Create and store messages
    INFO_PRINTF1(_L("store messages"));
	count = aArray.Count();
	index = 0;
	while ( index < count )
		{
		INFO_PRINTF2(_L("Write %d. message"),index+1);
		CSmsMessage* smsMessage = CreateAndSetMessageL( aArray, index++ );
		CleanupStack::PushL(smsMessage);
		WriteSmsToSimL(*smsMessage, socket);
		CleanupStack::PopAndDestroy(smsMessage);
		}

	// Enumerate messages
    INFO_PRINTF1(_L("Enumeration"));
	ReadSmsStoreL(socket, messages);
	count = messages.Count();
	index = 0;

	// Delete messages
    INFO_PRINTF1(_L("Delete messages"));
	while ( index < count )
		{
		smsmessage = messages[index++];
		INFO_PRINTF2(_L("Delete %d. message from the store"), index );
		ret = DeleteSmsL(*smsmessage, socket);
		TEST( ( ret == KErrNone ) );
		}

	CleanupStack::PopAndDestroy(&messages);
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }

TInt CSmsInterTestStep::TestParametersL(CMobilePhoneSmspList& aLeft,CMobilePhoneSmspList& aRight)
/**
 *  Compare sms parameters taken from chunk to parameters retrieved from SIM
 *  @param aLeft sms parameters taken from the Dummychunk
 *  @param aRight sms parameters retrieved from the SIM
 *  @return TInt : The error code
 */
	{
	TInt ret(KErrNone);

	if(aLeft.Enumerate() == aRight.Enumerate())
		{
        for(TInt storeIndex=0; storeIndex<aLeft.Enumerate(); storeIndex++)
            {
            const RMobileSmsMessaging::TMobileSmspEntryV1& leftEntry=aLeft.GetEntryL(storeIndex);
            TBool found=EFalse;

            for(TInt writeIndex=0; writeIndex<aRight.Enumerate(); writeIndex++)
                {
                const RMobileSmsMessaging::TMobileSmspEntryV1& rigthEntry=aRight.GetEntryL(writeIndex);
                if(leftEntry.iIndex == rigthEntry.iIndex)
                    {
                    found=ETrue;
					if((leftEntry.iValidParams != rigthEntry.iValidParams) ||
						(leftEntry.iText != rigthEntry.iText))
						{
						ret = KErrCorrupt;
						break;
						}
					if(leftEntry.iValidParams & RMobileSmsMessaging::KDestinationIncluded)
						{
						ret = TestPhoneNumbers(leftEntry.iDestination,rigthEntry.iDestination);
						if(ret)
							break;
						}
					if(leftEntry.iValidParams & RMobileSmsMessaging::KSCAIncluded)
						{
						ret = TestPhoneNumbers(leftEntry.iServiceCentre,rigthEntry.iServiceCentre);
						if(ret)
							break;
						}
					if((leftEntry.iValidParams & RMobileSmsMessaging::KProtocolIdIncluded) &&
						(leftEntry.iProtocolId != rigthEntry.iProtocolId))
						ret = KErrCorrupt;
					if((leftEntry.iValidParams & RMobileSmsMessaging::KValidityPeriodIncluded) &&
						(leftEntry.iValidityPeriod != rigthEntry.iValidityPeriod))
						ret = KErrCorrupt;
					if((leftEntry.iValidParams & RMobileSmsMessaging::KDcsIncluded) &&
						(leftEntry.iDcs != rigthEntry.iDcs))
						ret = KErrCorrupt;
                    break;
                    }
                }

            if(!found || ret != KErrNone)
                {
                ret = KErrCorrupt;
                break;
                }
            }
		}
	else
		{
		ret = KErrCorrupt;
		}

	if(ret == KErrNone)
		{
		INFO_PRINTF1(_L("SMS Parameters Ok!") );
		}
	else
		{
		INFO_PRINTF2(_L("ERROR: SMSP lists doesn't match: %d"), ret );
		}

	return ret;
	}

TInt CSmsInterTestStep::TestPhoneNumbers(const RMobilePhone::TMobileAddress& aLeftNumber,const RMobilePhone::TMobileAddress& aRightNumber)
/**
 *  Compare phone numbers
 *  @param aLeftNumber reference to TMobileAddress object
 *  @param aRightNumber reference to TMobileAddress object
 */
	{
	TInt ret(KErrNone);
	TBuf<TGsmSmsTelNumberMaxLen> leftNumber;
	TBuf<TGsmSmsTelNumberMaxLen> rightNumber;

	//Take '+' sign away first
	leftNumber.Copy(aLeftNumber.iTelNumber);
	if(leftNumber.Length() > 0 && leftNumber[0] == '+')
		{
		// Remove "+" sign from telephony number
		leftNumber.Delete(0,1);
		leftNumber.Trim();
		}
	//Take '+' sign away first
	rightNumber.Copy(aRightNumber.iTelNumber);
	if(rightNumber.Length() > 0 && rightNumber[0] == '+')
		{
		// Remove "+" sign from telephony number
		rightNumber.Delete(0,1);
		rightNumber.Trim();
		}
	if((leftNumber.Compare(rightNumber) != 0) ||
       (aLeftNumber.iTypeOfNumber != aRightNumber.iTypeOfNumber) ||
       (aLeftNumber.iNumberPlan != aRightNumber.iNumberPlan))
		{
        ret = KErrCorrupt;
        }
	return(ret);
	}


void CSmsInterTestStep::TestRetrieveAndStoreSmsParamsL(
                            RMobileSmsMessaging::TMobileSmspStoreValidParams aSmspEntryValidParams,
                            RMobilePhone::TMobileTON aSmspEntryScTypeOfNumber,
                            RMobilePhone::TMobileNPI aSmspEntryScNumberPlan,
                            const TDesC8& aSmspEntryScTelNumber )
    {
    INFO_PRINTF1(_L("SMS parameters test"));

    RSocketServ socketServer;
    Connect2EsockLC(socketServer);
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrLocalOperation);

    RSmsSocketReadStream readstream(socket);
    TRequestStatus status;

//
//Retrieve SMSP list
//
    // Create the smspList
    CMobilePhoneSmspList* orginalSmspList = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(orginalSmspList);

    // Make read SMS params request to the SMS Stack.
    socket.Ioctl(KIoctlReadSmsParams,status,NULL, KSolSmsProv);
    INFO_PRINTF1(_L("waiting for SMS parameters...") );
    User::WaitForRequest(status);
    TEST(status.Int() == KErrNone);

    // Read list from stream and make positive acknowledgement to the SMS Stack
    TRAPD(ret,readstream >> *orginalSmspList);
    TEST(ret==KErrNone);
    socket.Ioctl(KIoctlCompleteReadSmsParams,status,NULL, KSolSmsProv);
    User::WaitForRequest(status);
    TEST(status.Int() == KErrNone);
    TInt maxNumberEntries = orginalSmspList->MaxNumberEntries();
    // There should be one SMSP entry
    TEST(maxNumberEntries != 0);

//
//Store SMSP list
//
    // Create the smspList
    CMobilePhoneSmspList* smspList1 = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspList1);

    // Because of AT-command limitations we can store only SC address to the phone.
    RMobileSmsMessaging::TMobileSmspEntryV1 smspEntry;
    smspEntry.iValidParams = aSmspEntryValidParams;
    smspEntry.iServiceCentre.iTypeOfNumber = aSmspEntryScTypeOfNumber;
    smspEntry.iServiceCentre.iNumberPlan = aSmspEntryScNumberPlan;
    smspEntry.iServiceCentre.iTelNumber.Copy(aSmspEntryScTelNumber);

    //Add first too many SMSP entries to the list
    TInt i=0;
    for(i=0;i<(maxNumberEntries+1);i++)
        {
        smspEntry.iIndex = i;
        smspList1->AddEntryL(smspEntry);
        }

    INFO_PRINTF1(_L("writing SMS parameters...") );
    ret = iSmsStackTestUtils->StoreParamsL(*smspList1,socket,EFalse);
    TEST(ret==KErrArgument);
    // Writing failed because SMSP list contained too many entries.

    // Delete one entry from SMSP list and try to write the list again.
    smspList1->DeleteEntryL(i-1);

    INFO_PRINTF1(_L("writing SMS parameters...") );
    ret = iSmsStackTestUtils->StoreParamsL(*smspList1,socket,EFalse);
    TEST(ret==KErrNone);

    INFO_PRINTF1(_L("SMS stack has now buffered stored parameters to it's internal memory.") );
    INFO_PRINTF1(_L("Shut down SMS stack before retrieving parameters again.") );
    socket.Close();
    socketServer.Close();
    User::After(5000000); // Defect BEN-56FFEP - increase delay from 2 to 5s to prevent
                          // hang when using slower phone

    ret=socketServer.Connect(KSocketMessageSlots);
    TEST(ret==KErrNone);
    iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrLocalOperation);
    CleanupStack::Pop(&socket); //already on the cleanupstack

//
//Retrieve SMSP list again and compare stored and retrieved SMSP lists
//
    // Create the smspList
    CMobilePhoneSmspList* smspList2 = CMobilePhoneSmspList::NewL();
    CleanupStack::PushL(smspList2);

    // Make new read SMS params request to the SMS Stack.
    socket.Ioctl(KIoctlReadSmsParams,status,NULL, KSolSmsProv);
    INFO_PRINTF1(_L("waiting for SMS parameters...") );
    User::WaitForRequest(status);
    TEST(status.Int() == KErrNone);

    // Read list from stream and make positive acknowledgement to the SMS Stack
    TRAP(ret,readstream >> *smspList2);
    TEST(ret==KErrNone);
    socket.Ioctl(KIoctlCompleteReadSmsParams,status,NULL, KSolSmsProv);
    User::WaitForRequest(status);
    TEST(status.Int() == KErrNone);

    // Compare SMS parameter lists
    ret = TestParametersL(*smspList1,*smspList2);
//TODO Fails here with 6210. Maybe there is defect in MMTSY.
    TEST(ret==KErrNone);

//
//Return the orginal SMSP store
//
    ret = iSmsStackTestUtils->StoreParamsL(*orginalSmspList,socket,EFalse);
    TEST(ret==KErrNone);

    CleanupStack::PopAndDestroy(5); //orginalSmspList,smspList1,smspList2,socket,socketServer
    User::After(2000000); // wait for socket to close before reopening
    }


void CSmsInterTestStep::TestPDUSlotStoreWithEnumerationL( const RPointerArray<CSmsPduDbMessage>& aArray )
/**
 *  @test Switch on the phone with a SIM.
 *  Enumerate, delete (first message if exist and last message after writing),
 *  write (one message to the sim), enumerate and compare with returned PDU slots,
 *  write a further message, enumerate and compare with update PDU
 */
	{
	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	// Open the socket for SIM operations
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrLocalOperation);

	TInt ret = 0;
	CSmsMessage* tempSmsMessage;

	// Enumerate messages
	INFO_PRINTF1(_L("Enumeration"));
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);
	TInt count = messages.Count();
	TInt index = 0;

	// Delete messages
	INFO_PRINTF1(_L("Delete messages"));
	while ( index < count )
		{
		tempSmsMessage = messages[index++];

		INFO_PRINTF2(_L("Delete %d. message from the store "), index );
		ret = DeleteSmsL(*tempSmsMessage, socket);
		TEST( !( ret == KErrNone ) );

		}
	messages.ResetAndDestroy();

	// Create and store messages
	INFO_PRINTF1(_L("store messages"));
	count = aArray.Count();
	index = 0;
	ret=KErrNone;
	TBuf8<100> buf;

	// Write a message, enumerate and check updated slot info
	INFO_PRINTF2(_L("Write %d. message"),index+1);
	CSmsMessage* smsMessage1 = CreateAndSetMessageL( aArray, index );
	CleanupStack::PushL(smsMessage1);
	TRAP(ret,WriteSmsToSimL(*smsMessage1, socket, &buf));
	TEST(ret==KErrNone);
	TRAP(ret,smsMessage1->UpdateSlotsL(buf));
	TEST(ret==KErrNone);

	// Create and store a further message
	INFO_PRINTF2(_L("Write %d. message"),1);
	CSmsMessage* smsMessage2 = CreateAndSetMessageL( aArray, 1 ); // Point to final message
	CleanupStack::PushL(smsMessage2);
	TRAP(ret,WriteSmsToSimL(*smsMessage2, socket, &buf));

	TEST(ret==KErrNone);
	TRAP(ret,smsMessage2->UpdateSlotsL(buf));
	TEST(ret==KErrNone);

	// Enumerate messages
	INFO_PRINTF1(_L("Enumeration"));
	ReadSmsStoreL(socket, messages);
	count = messages.Count();
	TEST(!(smsMessage1->MatchSlots(messages[0]->iSlotArray) == EFalse));
	TEST(!(smsMessage2->MatchSlots(messages[1]->iSlotArray) == EFalse));
	CleanupStack::PopAndDestroy(smsMessage2);
	CleanupStack::PopAndDestroy(smsMessage1);

	index = 0;

	// Delete messages
	INFO_PRINTF1(_L("Delete messages"));
	while ( index < count )
		{
		tempSmsMessage = messages[index++];
		INFO_PRINTF2(_L("Delete %d. message from the store"), index );
		ret = DeleteSmsL(*tempSmsMessage, socket);
		TEST( ( ret == KErrNone ) );
		}

	CleanupStack::PopAndDestroy(&messages);
	CleanupStack::PopAndDestroy(&socket);
	CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
	}

void CSmsInterTestStep::TestDeleteMessagesWithoutEnumerationL( const RPointerArray<CSmsPduDbMessage>& aArray )
/**
 *  @test Switch on the phone with a SIM.
 *  Enumerate, delete (first message if exist and last message after writing),
 *  write (one messages to the sim), delete message without enumeration based on slot information
 */
	{
	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	// Open the socket for SIM operations
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrLocalOperation);

	TInt ret = 0;
	CSmsMessage* tempSmsMessage;

	// Enumerate messages
	INFO_PRINTF1(_L("Enumeration"));
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);
	TInt count = messages.Count();
	TInt index = 0;

	// Delete messages
	INFO_PRINTF1(_L("Delete messages"));
	while ( index < count )
		{
		tempSmsMessage = messages[index++];

		INFO_PRINTF2(_L("Delete %d. message from the store "), index );
		ret = DeleteSmsL(*tempSmsMessage, socket);
		TEST( !( ret == KErrNone ) );

		}
	messages.ResetAndDestroy();

	// Create and store messages
	INFO_PRINTF1(_L("store messages"));
	count = aArray.Count();
	index = 0;
	ret=KErrNone;
	TBuf8<100> buf;

	CSmsMessage* smsMessage = 0;
	// Write message
	while ( ret == KErrNone && index < count-1 )
		{
    	INFO_PRINTF2(_L("Write %d. message"),index+1);
    	smsMessage = CreateAndSetMessageL( aArray, index++ );
    	CleanupStack::PushL(smsMessage);
    	TRAP(ret,WriteSmsToSimL(*smsMessage, socket, &buf));
    	TEST(ret==KErrNone);
        TRAP(ret,smsMessage->UpdateSlotsL(buf));
    	TEST(ret==KErrNone);
		CleanupStack::PopAndDestroy(smsMessage);
		}

	INFO_PRINTF2(_L("Write %d. message"),count);
	smsMessage = CreateAndSetMessageL( aArray, count-1 );
	CleanupStack::PushL(smsMessage);
	TRAP(ret,WriteSmsToSimL(*smsMessage, socket, &buf));
	TEST(ret==KErrNone);
	TRAP(ret,smsMessage->UpdateSlotsL(buf));
	TEST(ret==KErrNone);

	// Enumerate messages
	INFO_PRINTF1(_L("Enumeration"));
	ReadSmsStoreL(socket, messages);
	count = messages.Count();

	messages.ResetAndDestroy();

	// Ensure three messages remain on SIM
	INFO_PRINTF2(_L("Count =  %d. "),count);
	
	TEST((count == 3));
	     

	// Delete message
	ret = DeleteSmsL(*smsMessage, socket);
	
	TEST( ( ret == KErrNone ) );

	// Enumerate messages
	INFO_PRINTF1(_L("Enumeration"));
	ReadSmsStoreL(socket, messages);
	count = messages.Count();

	//Ensure only two messages remain on SIM
	TEST((count == 2));

	//Ensure that deleted message no longer present on sim
	for (index = 0; index < count; index++)
        {
        TEST((smsMessage->MatchSlots(messages[count-1]->iSlotArray) == EFalse));
        }

	CleanupStack::PopAndDestroy(smsMessage);

	index = 0;

	// Delete messages
	INFO_PRINTF1(_L("Delete messages"));
	while ( index < count )
		{
		tempSmsMessage = messages[index++];
		INFO_PRINTF2(_L("Delete %d. message from the store"), index );
		ret = DeleteSmsL(*tempSmsMessage, socket);
		TEST( ( ret == KErrNone ) );
		}

	CleanupStack::PopAndDestroy(&messages);
	CleanupStack::PopAndDestroy(&socket);
	CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
	}


void CSmsInterTestStep::TestRetrieveAndStoreSmsParamsOOML()
/**
 *  @test Retrieve and store SMS parameters - OOM condition applied
 */
    {
	INFO_PRINTF1(_L("Test out of memory handling"));
#ifdef _DEBUG
	RSocketServ debugSocketServer;
	TInt ret=debugSocketServer.Connect(KSocketMessageSlots);
	TEST(ret==KErrNone);
	CleanupClosePushL(debugSocketServer);

	debugSocketServer.__DbgMarkHeap();

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrLocalOperation);

	RSmsSocketReadStream readstream(socket);


//
//Retrieve SMS parameters
//
	CMobilePhoneSmspList* smspList = CMobilePhoneSmspList::NewL();
	CleanupStack::PushL(smspList);
	INFO_PRINTF1(_L("Retrieve SMS parameters."));
    TInt count=0;
    ret=KErrNoMemory;
    while (ret==KErrNoMemory || count < 7)
        {
		debugSocketServer.__DbgFailNext(count);
        TRAP(ret,iSmsStackTestUtils->MakeParametersReadRequestL(socket););
		count++;
		}

	INFO_PRINTF1(_L(""));
    debugSocketServer.__DbgFailNext(-1); // Reset heap

    // Read list from stream and make acknowledgement to the SMS Stack
	readstream >> *smspList;
	TRequestStatus status;
	socket.Ioctl(KIoctlCompleteReadSmsParams,status,NULL,KSolSmsProv);
    User::WaitForRequest(status);
	TEST(status.Int() == KErrNone);
	TInt maxNumberEntries = smspList->MaxNumberEntries();
	// There should be one SMSP entry
	TEST(maxNumberEntries != 0);

//
//Store parameters
//
    INFO_PRINTF1(_L("Store SMS parameters."));
	count=0;
    ret=KErrNoMemory;
    while (ret==KErrNoMemory || count < 7)
        {
		debugSocketServer.__DbgFailNext(count);
        TRAP(ret,iSmsStackTestUtils->StoreParamsLeaveIfErrorL(*smspList, socket););
		count++;
		}

	 INFO_PRINTF1(_L(""));
    debugSocketServer.__DbgFailNext(-1); // Reset heap

	CleanupStack::PopAndDestroy(smspList);
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);

	User::After(5000000); // wait for socket to close before reopening
	debugSocketServer.__DbgMarkEnd(0);
	CleanupStack::PopAndDestroy(1); //debugSocketServer

#endif
	}

void CSmsInterTestStep::TestSmsStoreMsgOfDifferentClassesL( const RPointerArray<CSmsPduDbMessage>& aArray,
												 const CArrayFixFlat<TInt>* aSmsStorageArray,
												 const CArrayFixFlat<TInt>* aSmsClassArray )
/**
 *  Test read, write and delete messages of different classes to/from
 *  SIM & phone memory.
 */
    {
    INFO_PRINTF1(_L("Read, write and delete messages of different classes"));

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    TInt ret(KErrNone);

//
// Enumerate messages
//
    INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> orginalMessages;
	CleanupResetAndDestroyPushL(orginalMessages);
	ReadSmsStoreL(socket, orginalMessages);

//
// Delete all messages
//
    INFO_PRINTF1(_L("Deleting all messages from phone side SMS storage ..."));
    TInt msgCount = orginalMessages.Count();
    while(msgCount--)
        {
	    ret = DeleteSmsL(*orginalMessages[msgCount], socket);
        TEST(ret==KErrNone);
        }


//
// Write messages
//
    INFO_PRINTF1(_L("Write, read and delete messages with different classes to/from SIM and phone memory "));


	CSmsMessage* smsMessage = CreateAndSetMessageL( aArray, 0 );
	CleanupStack::PushL(smsMessage);

	smsMessage->SmsPDU().SetBits7To4(TSmsDataCodingScheme::ESmsDCSTextUncompressedWithClassInfo);

	for ( TInt i = 0; i < aSmsStorageArray->Count(); i++ )
		{
		for ( TInt j = 0; j < aSmsClassArray->Count(); j++ )
			{

			smsMessage->SmsPDU().SetClass( ETrue, ( TSmsDataCodingScheme::TSmsClass )aSmsClassArray->At(j) );
			smsMessage->SetStorage( ( CSmsMessage::TMobileSmsStorage )aSmsStorageArray->At(i) );

            INFO_PRINTF2(_L("Writing Class %d Message ..."),aSmsClassArray->At(j));
			TRAPD(ret,WriteSmsLeaveIfErrorL(*smsMessage,socket););
			// Some phones doesn't support phone memory. In this case SMS Stack
			// completes write request with KErrNotFound.
			// Some phones has phone memory that can be used only by phone
			// when SIM comes full (6210). Store info from this kind of
			// phone memory tells that iTotalEntries==iUsedEntries. In this case
			// SMS Stack completes write request with KErrNoMemory.
			if(ret == KErrNoMemory || ret == KErrNotFound)
				{
				INFO_PRINTF1(_L("Phone does not support phone memory!"));
				break;
				}
		//
		// Read message and compare it to written message
		//
			if(( CSmsMessage::TMobileSmsStorage )aSmsStorageArray->At(i) == CSmsMessage::ESmsSIMStorage)
				{
				RPointerArray<CSmsMessage> messages2;
				CleanupResetAndDestroyPushL(messages2);
				ReadSmsStoreL(socket, messages2);
				TestSmsContentsL(smsMessage,aArray[0]->iSmsMessage->Buffer());
				TEST(smsMessage->ToFromAddress()==messages2[0]->ToFromAddress());
				if(messages2[0]->ServiceCenterAddress().Length())
				// if the test is run on live netwrok then uncomment this
					// TEST(smsMessage->ServiceCenterAddress()==messages2[0]->ServiceCenterAddress());

		//
		// Delete message
		//
				ret = DeleteSmsL(*messages2[0], socket);
				TEST(ret==KErrNone);

				CleanupStack::PopAndDestroy(&messages2); // smsMessage, messages
				}
			}
		}

	CleanupStack::PopAndDestroy(smsMessage);

//
// Enumerate messages and check that message count is zero
//
    INFO_PRINTF1(_L("Enumerating messages and checking that phone store is empty ..."));
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);
	TEST(messages.Count() == 0);

//
// Return the original phone side SMS store
//
    INFO_PRINTF1(_L("Returning the original phone side SMS store ..."));
    msgCount = orginalMessages.Count();
    while(msgCount--)
        {
	    WriteSmsToSimL(*orginalMessages[msgCount], socket);
        }

	CleanupStack::PopAndDestroy(2); // messages, orginalMessages
    CleanupStack::PopAndDestroy(&socket);
	//closing is too fast when using EKA2
	User::After(2000000);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestDuplicatePdusOnStoreL( const RPointerArray<CSmsPduDbMessage>& aArray )
/**
 *  Test read, write and delete duplicate PDUs to/from SIM & phone store.
 */
	{
	INFO_PRINTF1(_L("Read, write and delete duplicate PDUs to/from store"));

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    TInt ret(KErrNone);

//
// Enumerate messages
//
    INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> orginalMessages;
	CleanupResetAndDestroyPushL(orginalMessages);
	ReadSmsStoreL(socket, orginalMessages);

//
// Delete all messages
//
    INFO_PRINTF1(_L("Deleting all messages from phone side SMS storage ..."));
    TInt msgCount = orginalMessages.Count();
    while(msgCount--)
        {
	    ret = DeleteSmsL(*orginalMessages[msgCount], socket);
        TEST(ret==KErrNone);
        }

//
// Write duplicate messages to the SIM and phone memory
//
	CSmsMessage* smsMessage = CreateAndSetMessageL( aArray, 0 );
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF1(_L("Writing 4 duplicate messages to SIM storage ..."));
	msgCount=4;
	while(msgCount--)
		WriteSmsToSimL(*smsMessage, socket);

	INFO_PRINTF1(_L("Writing 4 duplicate messages to phone memory ..."));
	smsMessage->SetStorage(CSmsMessage::ESmsPhoneStorage);
	msgCount=4;
	while(msgCount--)
		{
		TRAP(ret,WriteSmsLeaveIfErrorL(*smsMessage,socket););
		// Some phones doesn't support phone memory. In this case SMS Stack
		// completes write request with KErrNotFound.
		// Some phones has phone memory that can be used only by phone
		// when SIM comes full (6210). Store info from this kind of
		// phone memory tells that iTotalEntries==iUsedEntries. In this case
		// SMS Stack completes write request with KErrNoMemory.
		if(ret == KErrNoMemory || ret == KErrNotFound)
			{
			INFO_PRINTF1(_L("Phone does not support phone memory!"));
			break;
			}
		}
	CleanupStack::PopAndDestroy(smsMessage);

//
// Read duplicate messages
//
	INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);
	msgCount = messages.Count();

	TEST(msgCount == 4);

//
// Check messages contents and after that delete messages
//
	while(msgCount--)
		{
		TestSmsContentsL( messages[msgCount], aArray[0]->iSmsMessage->Buffer() );
		ret = DeleteSmsL(*messages[msgCount], socket);
		TEST(ret==KErrNone);
		}
	messages.ResetAndDestroy();

//
// Read messages again and check that message count is zero
//
	INFO_PRINTF1(_L("Enumerating and checking that whole message store is empty ..."));
	ReadSmsStoreL(socket, messages);
	TEST(!(messages.Count()));

//
// Return the original phone side SMS store
//
    INFO_PRINTF1(_L("Returning the original phone side SMS store ..."));
    msgCount = orginalMessages.Count();
    while(msgCount--)
	    WriteSmsToSimL(*orginalMessages[msgCount], socket);

	CleanupStack::PopAndDestroy(2); // messages, orginalMessages
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
	}

void CSmsInterTestStep::TestMaxNumberOfPdusOnStoreL()
/**
 *  Test Write to store until it's full and then read and delete all
 */
	{
	INFO_PRINTF1(_L("Read, write and delete duplicate PDUs to/from store"));

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    TInt ret(KErrNone);

//
// Enumerate messages
//
    INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> orginalMessages;
	CleanupResetAndDestroyPushL(orginalMessages);
	ReadSmsStoreL(socket, orginalMessages);

//
// Delete all messages
//
    INFO_PRINTF1(_L("Deleting all messages from phone side SMS storage ..."));
    TInt msgCount = orginalMessages.Count();
    while(msgCount--)
        {
	    ret = DeleteSmsL(*orginalMessages[msgCount], socket);
        TEST(ret==KErrNone);
        }

//
// Write messages until storage comes full
//
	RPointerArray<CSmsMessage> writtenMessages;
	CleanupResetAndDestroyPushL(writtenMessages);

	INFO_PRINTF1(_L("Writing messages until SIM comes full ..."));
	WriteMessagesUntilStoreComesFullL(writtenMessages,CSmsMessage::ESmsSIMStorage,socket);

//
// Enumerate messages
//
    INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> readMessages;
	CleanupResetAndDestroyPushL(readMessages);
	ReadSmsStoreL(socket, readMessages);
	INFO_PRINTF1(_L("Comparing written and read messages ..."));
	TRAPD(testSmsListsErr, TestSmsListsL(readMessages,writtenMessages));
	TEST(testSmsListsErr == KErrNone);

//
// Delete all messages
//
    INFO_PRINTF1(_L("Deleting all messages from phone side SMS storage ..."));
	msgCount = readMessages.Count();
    while(msgCount--)
        {
	    ret = DeleteSmsL(*readMessages[msgCount], socket);
        TEST(ret==KErrNone);
        }
	readMessages.ResetAndDestroy();

//
// Read messages again and check that message count is zero
//
	INFO_PRINTF1(_L("Enumerating and checking that whole message store is empty ..."));
	ReadSmsStoreL(socket, readMessages);
	TEST(readMessages.Count()==0);

//
// Return the original phone side SMS store
//
    INFO_PRINTF1(_L("Returning the original phone side SMS store ..."));
    msgCount = orginalMessages.Count();
    while(msgCount--)
	    WriteSmsToSimL(*orginalMessages[msgCount], socket);

	CleanupStack::PopAndDestroy(3);	//readMessages,writtenMessages,orginalMessages
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
	}

void CSmsInterTestStep::TestDeletingMessageInEmptyStoreL( RPointerArray<CSmsPduDbMessage> aArray )
/**
 *  @test Attempt to delete a message in the empty store
 */
    {
    INFO_PRINTF1(_L("Attempt to delete a message in the empty store"));

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    TInt ret(KErrNone);

//
// Enumerate messages
//
    INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> orginalMessages;
	CleanupResetAndDestroyPushL(orginalMessages);
	ReadSmsStoreL(socket, orginalMessages);

//
// Delete all messages
//
    INFO_PRINTF1(_L("Deleting all messages from phone side SMS storage ..."));
    TInt msgCount = orginalMessages.Count();
    while(msgCount--)
        {
	    ret = DeleteSmsL(*orginalMessages[msgCount], socket);
        TEST(ret==KErrNone);
        }

//
// Try to delete messages in the empty store
//
	CSmsMessage* smsMessage = CreateAndSetMessageL( aArray, 0 );
	CleanupStack::PushL(smsMessage);

    INFO_PRINTF1(_L("Trying to delete message from empty SIM storage ..."));
	ret = DeleteSmsL(*smsMessage, socket);
    TEST(ret == KErrArgument);

    INFO_PRINTF1(_L("Trying to delete message from empty phone storage ..."));
	smsMessage->SetStorage(CSmsMessage::ESmsPhoneStorage);
	ret = DeleteSmsL(*smsMessage, socket);
    TEST(ret == KErrArgument);

//
// Return the original phone side SMS store
//
    INFO_PRINTF1(_L("Returning the original phone side SMS store ..."));
    msgCount = orginalMessages.Count();
    while(msgCount--)
        {
	    WriteSmsToSimL(*orginalMessages[msgCount], socket);
        }

	CleanupStack::PopAndDestroy(2); // smsMessage, orginalMessages
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestConcatenatedMessagesL( const RPointerArray<CSmsPduDbMessage>& aArray )
/**
 *  Test enumerate and write and delete concatenated messages
 */
	{
	INFO_PRINTF1(_L("Enumerate and write and delete concatenated messages"));

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);

	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    TInt ret(KErrNone);

//
// Enumerate messages
//
    INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> orginalMessages;
	CleanupResetAndDestroyPushL(orginalMessages);
	ReadSmsStoreL(socket, orginalMessages);

//
// Delete all messages
//
    INFO_PRINTF1(_L("Deleting all messages from phone side SMS storage ..."));
    TInt msgCount = orginalMessages.Count();
    while(msgCount--)
        {
	    ret = DeleteSmsL(*orginalMessages[msgCount], socket);
        TEST(ret==KErrNone);
        }

//
// Write concatenated message to SIM and phone memory
//
	CSmsMessage* smsMessage = CreateAndSetMessageL( aArray, 0 );
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF1(_L("Writing concatenated message to the SIM ..."));
	WriteSmsToSimL(*smsMessage, socket);

	smsMessage->SetStorage(CSmsMessage::ESmsPhoneStorage);
	INFO_PRINTF1(_L("Writing concatenated message to the phone memory ..."));
	TRAP(ret,WriteSmsLeaveIfErrorL(*smsMessage,socket););
	// Some phones doesn't support phone memory. In this case SMS Stack
	// completes write request with KErrNotFound.
	// Some phones has phone memory that can be used only by phone
	// when SIM comes full (6210). Store info from this kind of
	// phone memory tells that iTotalEntries==iUsedEntries. In this case
	// SMS Stack completes write request with KErrNoMemory.
	if(ret == KErrNoMemory || ret == KErrNotFound)
		{
		INFO_PRINTF1(_L("Phone does not support phone memory!"));
		}
	CleanupStack::PopAndDestroy(smsMessage);

//TODO SKU?? Add more concatenated messages to this test case when PDU library system is ready

//
// Enumerate messages
//
    INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);
	INFO_PRINTF1(_L("Comparing written and read messages ..."));
	msgCount = messages.Count();
    while(msgCount--)
        {
		TestSmsContentsL( messages[msgCount], aArray[0]->iSmsMessage->Buffer() );
		}

//
// Delete all messages
//
    INFO_PRINTF1(_L("Deleting messages from phone side SMS storage ..."));
	msgCount = messages.Count();
    while(msgCount--)
        {
	    ret = DeleteSmsL(*messages[msgCount], socket);
        TEST(ret==KErrNone);
        }

//
// Return the original phone side SMS store
//
    INFO_PRINTF1(_L("Returning the original phone side SMS store ..."));
    msgCount = orginalMessages.Count();
    while(msgCount--)
	    WriteSmsToSimL(*orginalMessages[msgCount], socket);

	CleanupStack::PopAndDestroy(2);	//messages,orginalMessages
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
	}

void CSmsInterTestStep::TestOutOfMemoryWriteL( const RPointerArray<CSmsPduDbMessage>& aArray )
/**
 *  Test out of memory handling while write process
 */
	{
	INFO_PRINTF1(_L("Test out of memory handling while write process"));
#ifdef _DEBUG
	/* Open dedicated debug socket server in order to detect heap imbalance after cache has been removed */
	RSocketServ debugSocketServer;
	TInt ret=debugSocketServer.Connect(KSocketMessageSlots);
	TEST(ret==KErrNone);
	CleanupClosePushL(debugSocketServer);

	debugSocketServer.__DbgMarkHeap();


	RSocketServ socketServer;
	Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

//
// Enumerate messages
//
    INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> orginalMessages;
	CleanupResetAndDestroyPushL(orginalMessages);
	ReadSmsStoreL(socket, orginalMessages);

//
// Delete all messages
//
    INFO_PRINTF1(_L("Deleting all messages from phone side SMS storage ..."));
    TInt count = orginalMessages.Count();
    while(count--)
        {
	    ret = DeleteSmsL(*orginalMessages[count], socket);
        TEST(ret==KErrNone);
        }

//
// Write SMS
//
	CSmsMessage* smsMessage = CreateAndSetMessageL( aArray, 0 );
	CleanupStack::PushL(smsMessage);

	INFO_PRINTF1(_L("Write SMS message to SIM"));
    count=0;
    ret=KErrNoMemory;
    while (ret==KErrNoMemory || count < 7)
        {
		debugSocketServer.__DbgFailNext(count);
        TRAP(ret,WriteSmsLeaveIfErrorL(*smsMessage,socket););
        count++;
        }

	debugSocketServer.__DbgFailNext(-1); // Reset heap

	CleanupStack::PopAndDestroy(smsMessage);

	INFO_PRINTF1(_L("Shut down SMS stack and retrieve messages.") );

//
// Enumerate messages
//
    INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(socket, messages);

//
// Delete messages
//
    INFO_PRINTF1(_L("Deleting written messages from phone side SMS storage ..."));
	count = messages.Count();
    while(count--)
        {
	    ret = DeleteSmsL(*messages[count], socket);
        TEST(ret==KErrNone);
        }

//
// Return the original phone side SMS store
//
    INFO_PRINTF1(_L("Returning the original phone side SMS store ..."));
    count = orginalMessages.Count();
    while(count--)
	    WriteSmsToSimL(*orginalMessages[count], socket);

	CleanupStack::PopAndDestroy(4);  //orginalMessages,socketServer,socket,messages
	User::After(2000000); // wait for socket to close before reopening
	debugSocketServer.__DbgMarkEnd(0);
	CleanupStack::PopAndDestroy(1); //debugSocketServer

#else
    (void) aArray;
#endif
	}

void CSmsInterTestStep::TestOutOfMemoryReadL()
/**
 *  Test out of memory handling while read process
 */
	{
	INFO_PRINTF1(_L("Test out of memory handling while read process"));
#ifdef _DEBUG
	/* Open dedicated debug socket server in order to detect heap imbalance after cache has been removed */
	RSocketServ debugSocketServer;
	TInt ret=debugSocketServer.Connect(KSocketMessageSlots);
	TEST(ret==KErrNone);
	CleanupClosePushL(debugSocketServer);

	debugSocketServer.__DbgMarkHeap();

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	RSmsSocketReadStream readstream(socket);
	TRequestStatus status;

//
// Enumerate messages
//
	TInt numberOfMessages(0);
	TInt count(0);
	ret=KErrNoMemory;
	while ((ret==KErrNoMemory || count < 7) && (ret != KErrNone))
		{
		debugSocketServer.__DbgFailNext(count);
		TRAP(ret,numberOfMessages=MakeReadSmsStoreRequestL(socket););
		count++;
		}
	debugSocketServer.__DbgFailNext(-1); // Reset heap


	INFO_PRINTF2(_L("%d enumerated messages"), numberOfMessages);

	//Check that test client can read all messages from the stream
	for(TInt i=0; i< numberOfMessages; i++)
		{
		CSmsBuffer* buffer=CSmsBuffer::NewL();
		CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
		CleanupStack::PushL(smsMessage);

		readstream >> *smsMessage;
		socket.Ioctl(KIoctlReadMessageSucceeded, status, NULL, KSolSmsProv);
		User::WaitForRequest(status);
		TEST(status.Int() == KErrNone);

		CleanupStack::PopAndDestroy(smsMessage);
		}
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
	debugSocketServer.__DbgMarkEnd(0);

	CleanupStack::PopAndDestroy(1); //debugSocketServer
#endif
	}

void CSmsInterTestStep::TestOutOfMemoryDeleteL( const RPointerArray<CSmsPduDbMessage>& aArray )
/**
 *  Test out of memory handling while delete process
 */
	{
	INFO_PRINTF1(_L("Test out of memory handling while delete process"));
#ifdef _DEBUG
	/* Open dedicated debug socket server in order to detect heap imbalance after cache has been removed */
	RSocketServ debugSocketServer;
	TInt ret=debugSocketServer.Connect(KSocketMessageSlots);
	TEST(ret==KErrNone);
	CleanupClosePushL(debugSocketServer);

	debugSocketServer.__DbgMarkHeap();

	RSocketServ socketServer;
	Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	TBool returnOriginalStore(EFalse);
	TInt count(0);

//
// Enumerate messages
//
    INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> originalMessages;
	RPointerArray<CSmsMessage> addedMessages;
	CleanupResetAndDestroyPushL(originalMessages);
	CleanupResetAndDestroyPushL(addedMessages);

	ReadSmsStoreL(socket, originalMessages);

//
// Write couple messages if store was empty
//
	if(0==originalMessages.Count())
		{
		returnOriginalStore=EFalse;
		TInt arrayCount = aArray.Count();
		INFO_PRINTF2(_L("Store is empty. Writing %d messages ..."), arrayCount );

		count=0;
		while( count < arrayCount )
			{

			CSmsMessage* smsMessage = CreateAndSetMessageL( aArray, count++ );

			CleanupStack::PushL(smsMessage);
			WriteSmsToSimL(*smsMessage,socket);
			CleanupStack::PopAndDestroy(smsMessage);
			}
		}
	else
		returnOriginalStore=ETrue;

	ReadSmsStoreL(socket, addedMessages);
//
// Delete all messages
//
    INFO_PRINTF1(_L("Deleting all messages from phone side SMS storage ..."));
	TInt i(0);
	count=1;
    ret=KErrNoMemory;
    while (i<addedMessages.Count())
        {
		if(count<5)
			debugSocketServer.__DbgFailNext(count);
        TRAP(ret,DeleteSmsLeaveIfErrorL(*addedMessages[i],socket));

		if(ret == KErrNone)
			{
		    INFO_PRINTF2(_L("Deleting %d. message was successfull"),i+1);
			i++;
			}
		count++;
        }

	INFO_PRINTF1(_L("before reseting the heap ..."));

	debugSocketServer.__DbgFailNext(-1); // Reset heap
	INFO_PRINTF1(_L("after reseting the heap ..."));

	if(i<addedMessages.Count())  //we havn't managed to delete all messages from SIM
	{
		while (i< addedMessages.Count() )
		{
			TRAP(ret,DeleteSmsLeaveIfErrorL(*addedMessages[i],socket));
			if(ret == KErrNone)
			{
		    INFO_PRINTF2(_L("Deleting %d. message was successfull"),i+1);
			i++;
			}
		}
	}

	INFO_PRINTF1(_L("Shut down SMS stack .") );

//
// Return the original phone side SMS store if it contained messages when test started
//
	if(returnOriginalStore)
		{
		INFO_PRINTF1(_L("Returning the original phone side SMS store ..."));
		count = originalMessages.Count();
		while(count--)
			WriteSmsToSimL(*originalMessages[count], socket);
		}


	User::After(2000000); // wait for socket to close before reopening
    CleanupStack::PopAndDestroy(4);  //originalMessages,addedMessages, socket, socketServer
	User::After(2000000); // wait for socket to close before reopening
	//INFO_PRINTF1(_L("before reset heap..."));

	INFO_PRINTF1(_L("before ending the heap ..."));

	debugSocketServer.__DbgMarkEnd(0);

	User::After(2000000);
	INFO_PRINTF1(_L("After debug end ..."));
	CleanupStack::PopAndDestroy(1); //debugSocketServer

#else
    (void)aArray;
#endif
	}

void CSmsInterTestStep::TestSmartMessagingL( const RPointerArray<CSmsPduDbMessage>& aArray,
								  const CArrayFixFlat<TInt>* aIeiDataArray,
								  const CArrayFixFlat<TInt>* aInformationElementArray,
								  const CArrayFixFlat<TInt>* aAlphabetArray )
/**
 *  @test Send and receive smart messages
 *  Note! This test doesn't work properly with 6210. Phone doesn't route incoming application port address
 *  messages to the MM TSY.
 *  TODO 6210 won't show received vCards to user. Maybe used version is not supported or destination port
 *  is wrong?
 */
    {
    INFO_PRINTF1(_L("Send and receive smart messages"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	TBuf<512> confFileBuf;
	TBuf<255> dataBuf;
	TBuf8<5> ieiData;

	TSmsDataCodingScheme::TSmsAlphabet alphabet;
	CSmsInformationElement::TSmsInformationElementIdentifier iei;

	TInt index = 2;
	for ( ; index < aArray.Count(); index++ )
		{
		alphabet = ( TSmsDataCodingScheme::TSmsAlphabet )aAlphabetArray->At(index);
		iei = ( CSmsInformationElement::TSmsInformationElementIdentifier )aInformationElementArray->At(index);

		TInt bufLen=aArray[index]->iSmsMessage->Buffer().Length();
		aArray[index]->iSmsMessage->Buffer().Extract(confFileBuf,0,bufLen);

		TInt ind(0);
		dataBuf.SetLength(0);
		TInt length = confFileBuf.Length();

		//Changes \n to be 0x0A for SendAndRecvAndCompareMessageIEIL
		if ( index != 2 )
			while ( ind < length )
				{
				if ( confFileBuf[ind] == 0x5C && confFileBuf[ind+1] == 0x6E ) //0x5C=='\', 0x6E=='n'
					{
					dataBuf.Append( 0x0A ); //"\n"
					ind += 2;
					}
				else
					dataBuf.Append( confFileBuf[ind++] );
				}
		else
			{

			for(TInt pos = 0;pos < length; pos += 2)
				{
				const TPtrC asciiVal(confFileBuf.Mid(pos,2));
				TUint8 val;
				TLex lexer(asciiVal);
				lexer.Val(val,EHex);

				dataBuf.Append(val);
				}
			}

		TInt testCaseNum(0), count(0);
		ieiData.SetLength(0);
		while ( testCaseNum < index )
			if ( aIeiDataArray->At(count++) == KErrNotFound )
				testCaseNum++;

		while ( aIeiDataArray->At(count) != KErrNotFound )
			ieiData.Append(aIeiDataArray->At(count++));


		SendAndRecvAndCompareMessageIEIL( dataBuf, ieiData, socket, alphabet, iei );

		}
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
	}

void CSmsInterTestStep::TestSendSMS2Port0000L( const RPointerArray<CSmsPduDbMessage>& aArray )
/**
 *  @test Send and Receive message to port 0000 and 23F4
 *  TODO This test doesn't work with 6210 because phone doesn't route incoming
 *  application port addressing messages to the MM TSY.
 */
    {
    INFO_PRINTF1(_L("Sending a SMS 2 port 0x0000 "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	for ( TInt i = 0; i < aArray.Count(); i++ )
		{
		TInt bufLen=aArray[i]->iSmsMessage->Buffer().Length();
		HBufC* textBuf=HBufC::NewL(bufLen);
		CleanupStack::PushL(textBuf);
		TPtr textPtr(textBuf->Des());
		aArray[i]->iSmsMessage->Buffer().Extract(textPtr,0,bufLen);

		SendAndRecvSms7BitL( textPtr, socket );
		CleanupStack::PopAndDestroy(textBuf);
		}

    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
    }

void CSmsInterTestStep::TestReceiveLongClass2L()
/**
 *  @test Receive a concatenated class 2 SMS message which is too long
 *  to fit on the SIM so the other parts have yet to be sent from
 *  the SMSC
 */
    {
	INFO_PRINTF1(_L("<font color=Orange>This test does not run in automated mode yet.</font>"));
    


    INFO_PRINTF1(_L("Send message too long for SIM"));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

	RPointerArray<CSmsMessage> writtenMessages;
	CleanupResetAndDestroyPushL(writtenMessages);

	INFO_PRINTF1(_L("Writing messages until SIM comes full ..."));
	WriteMessagesUntilStoreComesFullL(writtenMessages,CSmsMessage::ESmsSIMStorage,socket);


    INFO_PRINTF1(_L("Please manually delete three messages from the SIM."));
    //gTest.Getch();
    
    INFO_PRINTF1(_L("A message concatenated into 3 parts will now be sent."));

    TInt messageNum = 3;
    TInt msgSize = messageNum * KMaxNumberOfCharsFor7BitSms - (KMaxNumberOfCharsFor7BitSms / 2);

    HBufC* msgBuf = HBufC::NewLC(msgSize);
    TPtr msgPtr = msgBuf->Des();
    FillDes(msgPtr,msgSize);

    TTestCase testMsg(*msgBuf);
    testMsg.SetSmsClass(TSmsDataCodingScheme::ESmsClass2);
    INFO_PRINTF1(_L("testMsg.SetSmsClass()"));
    SendAndRecvTestMessageL(testMsg,socket);
    CleanupStack::PopAndDestroy(msgBuf);
    
	CleanupStack::PopAndDestroy(1);
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);

    }

void CSmsInterTestStep::TestSendAndReceiveConcatenated1L()
/**
 *  @test Receive a concatenated SMS with all message parts on the phone
 */
    {
	INFO_PRINTF1(_L("Receive a concatenated SMS with all message parts on the phone"));
    INFO_PRINTF1(_L("Please ensure 3 SMS messages can be stored on the phone."));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrSendOnly);

    TInt messageNum = 3;
    TInt msgSize = messageNum * KMaxNumberOfCharsFor7BitSms - (KMaxNumberOfCharsFor7BitSms / 2);

    HBufC* msgBuf = HBufC::NewLC(msgSize);
    TPtr msgPtr = msgBuf->Des();
    FillDes(msgPtr,msgSize);

    TTestCase testMsg(*msgBuf);

	RSocket socketRecv;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socketRecv,ESmsAddrRecvAny);

    SendTestMessageL(testMsg,socket);

    INFO_PRINTF1(_L("Press any key once all 3 parts of the message have arrived."));
    //gTest.Getch();
    INFO_PRINTF1(_L("Attempting to receive message."));

    WaitForRecvL(socketRecv);
    CSmsMessage* smsMessage = RecvSmsL(socketRecv);
    CleanupStack::PushL(smsMessage);
    TestMessageContentsL(smsMessage,testMsg);
    CleanupStack::PopAndDestroy(3); // msgBuf, socketRecv, smsMessage
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
    }

void CSmsInterTestStep::TestSendAndReceiveConcatenated2L()
/**
 *  @test Receive a concatenated SMS with some parts missing from the phone
 */
    {
	INFO_PRINTF1(_L("Receive a concatenated SMS with some parts missing from the phone"));
    INFO_PRINTF1(_L("Please ensure 3 SMS messages can be stored on the phone."));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	RSocket socket;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrSendOnly);

    TInt messageNum = 3;
    TInt msgSize = messageNum * KMaxNumberOfCharsFor7BitSms - (KMaxNumberOfCharsFor7BitSms / 2);

    HBufC* msgBuf = HBufC::NewLC(msgSize);
    TPtr msgPtr = msgBuf->Des();
    FillDes(msgPtr,msgSize);

    TTestCase testMsg(*msgBuf);
	RSocket socketRecv;
	iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socketRecv,ESmsAddrRecvAny);

    SendTestMessageL(testMsg,socket);

    INFO_PRINTF1(_L("Delete one message part then press any key."));
    //gTest.Getch();
    INFO_PRINTF1(_L("Attempting to receive message."));

    WaitForRecvL(socketRecv);
    CSmsMessage* smsMessage = RecvSmsL(socketRecv);
    CleanupStack::PushL(smsMessage);
    TestMessageContentsL(smsMessage,testMsg);
    CleanupStack::PopAndDestroy(3); // msgBuf, socketRecv, smsMessage
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
    }

void CSmsInterTestStep::TestBindingToApplicationPortL()
/**
 *  @test Binding to application port address family
 */
    {
	INFO_PRINTF1(_L("Test binding using application port addressing "));

	RSocketServ socketServer;
    Connect2EsockLC(socketServer);
	TSmsAddr addr8,addr16;
	addr8.SetSmsAddrFamily(ESmsAddrApplication8BitPort);
	addr16.SetSmsAddrFamily(ESmsAddrApplication16BitPort);
	TInt port(245),portother(255);
	addr8.SetPort(port);
	addr16.SetPort(port);

	RSocket socket8,socket16;

	TInt ret = 0;
	// 001 open sockets and bind them to the same port of both families, 8-bit addressing first
	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	socket8.Close();
	socket16.Close();

	// 002 open sockets and bind them to the same port of both families, 16-bit family first

	// open sockets
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	// bind sockets
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	socket8.Close();
	socket16.Close();

	// 003 rebind socket 8-bit family
	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	// bind sockets
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	addr8.SetPort(portother);
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	socket8.Close();

	// 004 rebind socket 16-bit family
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	// bind sockets
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	addr8.SetPort(portother);
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	socket16.Close();

	// 005-6 open sockets and bind them to the same port of the same family
	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	RSocket socket8other,socket16other;
	ret = socket8other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	ret = socket8other.Bind(addr8);
	TEST(ret==KErrAlreadyExists);

	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	ret = socket16other.Bind(addr16);
	TEST(ret==KErrAlreadyExists);

	socket8.Close();
	socket16.Close();
	socket8other.Close();
	socket16other.Close();

    CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestSendAndReceiveAndDeleteClass2MsgsL()
/**
 *  @test Send and Recieve Class 2 message, attempt to delete. Test needs phone that reports store information so GT code is responsible for deletion
 */
    {
	INFO_PRINTF1(_L("Send receive and attempt to delete a class 2 message"));

    RSocketServ socketServer;
    Connect2EsockLC(socketServer);
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

//
// Enumerate messages
//
    INFO_PRINTF1(_L("Starting enumeration ..."));
	RPointerArray<CSmsMessage> orginalMessages;
	CleanupResetAndDestroyPushL(orginalMessages);
	ReadSmsStoreL(socket, orginalMessages);

//
// Delete all messages
//
    INFO_PRINTF1(_L("Deleting all messages from phone side SMS storage ..."));
    TInt msgCount = orginalMessages.Count();
    while(msgCount--)
        {
	    TInt ret = DeleteSmsL(*orginalMessages[msgCount], socket);
        TEST(ret==KErrNone);
        }

    TInt messageNum = 1;
    TInt msgSize = messageNum * KMaxNumberOfCharsFor7BitSms - (KMaxNumberOfCharsFor7BitSms / 2);

    HBufC* msgBuf = HBufC::NewLC(msgSize);
    TPtr msgPtr = msgBuf->Des();
    FillDes(msgPtr,msgSize);

    TTestCase testMsg(*msgBuf);
    testMsg.SetSmsClass(TSmsDataCodingScheme::ESmsClass2);

    SendTestMessageL(testMsg, socket);

    WaitForRecvL(socket);
    CSmsMessage* smsMessage = RecvSmsL(socket);
    CleanupStack::PushL(smsMessage);
    TestMessageContentsL(smsMessage,testMsg);

//    Attempt to delete received message
    TInt ret = DeleteSmsL(*smsMessage, socket);
    if (ret != 0)
        INFO_PRINTF1(_L("Test is not valid with this phone as it doesn't report required storage information, try T65"));

    TEST(ret == 0);

    CleanupStack::PopAndDestroy(smsMessage);

    CleanupStack::PopAndDestroy(msgBuf);
    CleanupStack::PopAndDestroy(&orginalMessages);
    CleanupStack::PopAndDestroy(&socket);
    CleanupStack::PopAndDestroy(&socketServer);
    }

void CSmsInterTestStep::PublishandSubscribeTestL()
/**
 *  @test This is the regression test for defect PEN-4TKC82 (Defect Database v4)
 *  'SMSProt reschedules the phone initilization by timer'.
 *  test updated to use Publish and Subscribe
 */
    {
    //
    // Start-up
    //
    INFO_PRINTF1(_L("Create a socket") );

    RSocketServ socketServer;
    Connect2EsockLC(socketServer);

    INFO_PRINTF1(_L("Attaching to phone power property"));
    RProperty phonePowerProperty;
    User::LeaveIfError(phonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));
    CleanupClosePushL(phonePowerProperty);

    TRequestStatus status;
    INFO_PRINTF1(_L("Set phone power property to off"));
	TInt phonePowerCheck;
	do
    	{
       	phonePowerProperty.Subscribe(status);
    	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
    	User::After(5 * 1000000);	// sleep 5 secs;
    	User::WaitForRequest(status);
       	TEST(status.Int() == KErrNone);
    	User::LeaveIfError(phonePowerProperty.Get(phonePowerCheck));
    	}
	while (phonePowerCheck==ESAPhoneOn);

    INFO_PRINTF1(_L("Starting up Sms Stack"));
    RSocket socket;
    iSmsStackTestUtils->OpenSmsSocketLC(socketServer,socket,ESmsAddrRecvAny);

    _LIT(KSmsTestMessage,"Test message");
    CSmsMessage* smsMessage=CreateSmsMessageL(KSmsTestMessage,TSmsDataCodingScheme::ESmsAlphabet7Bit);
    CleanupStack::PushL(smsMessage);
    CSmsSubmit& submitPdu=(CSmsSubmit&)smsMessage->SmsPDU();
    submitPdu.SetReplyPath(ETrue);

    //
    // Check that Sms Stack can handle starting up with phone off
    //
    INFO_PRINTF1(_L("Checking that Sms Stack will NOT allow us to send an SMS"));
    TRAPD(ret,SendSmsDontCheckReturnValueL(smsMessage,socket));
    if(ret!=KErrDisconnected)
        User::Leave(KErrGeneral);

    //
    // Check that Sms Stack can handle the phone being switched on
    //
     INFO_PRINTF1(_L("Set phone power property to on"));
	do
    	{
       	phonePowerProperty.Subscribe(status);
    	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOn));
    	User::After(5 * 1000000);	// sleep 5 secs;
    	User::WaitForRequest(status);
       	TEST(status.Int() == KErrNone);
    	User::LeaveIfError(phonePowerProperty.Get(phonePowerCheck));
    	}
	while (phonePowerCheck==ESAPhoneOff);

    INFO_PRINTF1(_L("Checking that Sms Stack will allow us to send an SMS"));
    TRAP(ret,SendSmsDontCheckReturnValueL(smsMessage,socket));
    TEST(ret==KErrNone);

    WaitForRecvL(socket);
    CSmsMessage* recvMessage = RecvSmsL(socket);
    delete recvMessage;

    //
    // Check that Sms Stack can handle the phone being switched off
    //
    INFO_PRINTF1(_L("Set phone power property to off"));
	do
    	{
       	phonePowerProperty.Subscribe(status);
    	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOff));
    	User::After(5 * 1000000);	// sleep 5 secs;
    	User::WaitForRequest(status);
       	TEST(status.Int() == KErrNone);
    	User::LeaveIfError(phonePowerProperty.Get(phonePowerCheck));
    	}
	while (phonePowerCheck==ESAPhoneOn);


    INFO_PRINTF1(_L("Checking that Sms Stack will NOT allow us to send an SMS"));
    TRAP(ret,SendSmsDontCheckReturnValueL(smsMessage,socket));
    if(ret!=KErrDisconnected)
        User::Leave(KErrGeneral);

    //
    // Check that Sms Stack can handle the phone being switched on (just for good measures ;-))
    //
    INFO_PRINTF1(_L("Set phone power property to on"));
	do
    	{
       	phonePowerProperty.Subscribe(status);
    	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOn));
    	User::After(5 * 1000000);	// sleep 5 secs;
    	User::WaitForRequest(status);
       	TEST(status.Int() == KErrNone);
    	User::LeaveIfError(phonePowerProperty.Get(phonePowerCheck));
    	}
	while (phonePowerCheck==ESAPhoneOff);

    INFO_PRINTF1(_L("Checking that Sms Stack will allow us to send an SMS"));
    TRAP(ret,SendSmsDontCheckReturnValueL(smsMessage,socket));
    TEST(ret==KErrNone);

    WaitForRecvL(socket);
    recvMessage = RecvSmsL(socket);
    delete recvMessage;

    //
    // Shut-down
    //
    CleanupStack::PopAndDestroy(smsMessage);

    INFO_PRINTF1(_L("Closing down Sms Stack"));
    CleanupStack::PopAndDestroy(&socket);

    INFO_PRINTF1(_L("Releasing handle to phone power property"));
    CleanupStack::PopAndDestroy(&phonePowerProperty);
    //closing is too fast when using EKA2
    User::After(2000000);

    CleanupStack::PopAndDestroy(&socketServer);
    User::After(2000000); // wait for socket to close before reopening
    }

void CSmsInterTestStep::TestSendingReceivingViaAppPortPublishAndSubscribeL()
/**
 *  @test Send & receive via application port bound sockets with publish and subscribe
 */
    {
	INFO_PRINTF1(_L("Test sending and receiving utilising application port addressing "));

	RSocketServ socketServer;
	TSmsAddr addr8,addr16,addrany,addr8other,addr16other;
	addr8.SetSmsAddrFamily(ESmsAddrApplication8BitPort);
	addr16.SetSmsAddrFamily(ESmsAddrApplication16BitPort);
	addrany.SetSmsAddrFamily(ESmsAddrRecvAny);
	addr8other.SetSmsAddrFamily(ESmsAddrApplication8BitPort);
	addr16other.SetSmsAddrFamily(ESmsAddrApplication16BitPort);

	TInt port(245),portother(255);

	TBuf<256> buf;

	addr8.SetPort(port);
	addr16.SetPort(port);
	addr8other.SetPort(portother);
	addr16other.SetPort(portother);

	RSocket socket8,socket16,socketany,socket8other,socket16other;

	TInt ret = 0;

	Connect2EsockLC(socketServer);

    RProperty phonePowerProperty;
    User::LeaveIfError(phonePowerProperty.Attach(KUidSystemCategory, KUidPhonePwr.iUid));
    CleanupClosePushL(phonePowerProperty);

    TRequestStatus status;
	TInt phonePowerCheck;
	do
    	{
       	phonePowerProperty.Subscribe(status);
    	User::LeaveIfError(phonePowerProperty.Set(KUidSystemCategory,KUidPhonePwr.iUid,ESAPhoneOn));
    	User::After(5 * 1000000);	// sleep 5 secs;
    	User::WaitForRequest(status);
       	TEST(status.Int() == KErrNone);
    	User::LeaveIfError(phonePowerProperty.Get(phonePowerCheck));
    	}
	while (phonePowerCheck==ESAPhoneOff);

	// 013 send from  socket bound to recv any address to another socket bound to  8-bit addressing port
	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socketany.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	ret = socketany.Bind(addrany);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket8);
	CleanupClosePushL(socketany);

	//
	// create SMS message
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	buf.Format(_L("013 - SendingRecevingViaApplicationPortL Msg"));
	CSmsMessage* sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 8 Bit ports
	CSmsPDU& pdu = sendMessage->SmsPDU();
	pdu.SetApplicationPortAddressingL(ETrue, port, port, EFalse);

	// send SMS message
	SendSmsL(sendMessage,socketany);

	WaitForRecvL(socket8);
	CSmsMessage* recvMessage = RecvSmsL(socket8);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);
	CleanupStack::PopAndDestroy(&socketany);
	CleanupStack::PopAndDestroy(&socket8);


	// 014 send from  socket bound to recv any address to another socket bound to  16-bit addressing port

	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socketany.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);
	ret = socketany.Bind(addrany);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket16);
	CleanupClosePushL(socketany);

	//
	// create SMS message
	buf.Format(_L("014 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	CSmsPDU& pdu16 = sendMessage->SmsPDU();
	pdu16.SetApplicationPortAddressingL(ETrue, port, port, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socketany);

	WaitForRecvL(socket16);
	recvMessage = RecvSmsL(socket16);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socketany);
	CleanupStack::PopAndDestroy(&socket16);

	// 015 send from  socket bound to 8-bit addressing port to itself

	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);


	// bind them to ports
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket8);

	//
	// create SMS message
	buf.Format(_L("015 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 8 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, EFalse);

	// send EMS message
	SendSmsL(sendMessage,socket8);

	WaitForRecvL(socket8);
	recvMessage = RecvSmsL(socket8);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket8);

	// 016 send from socket bound to 16-bit addressing port to itself

	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);


	// bind it
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket16);

	//
	// create SMS message
	buf.Format(_L("016 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit port
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socket16);

	WaitForRecvL(socket16);
	recvMessage = RecvSmsL(socket16);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket16);
	//

	// 017 send from  socket bound to recv any address to another socket bound to  8-bit addressing port, in presence
	// of another socket bound to the same 16-bit port

	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socketany.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	ret = socketany.Bind(addrany);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket8);
	CleanupClosePushL(socket16);
	CleanupClosePushL(socketany);

	//
	// create SMS message
	buf.Format(_L("017 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 8 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, EFalse);

	// send SMS message
	SendSmsL(sendMessage,socketany);

	WaitForRecvL(socket8);
	recvMessage = RecvSmsL(socket8);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socketany);
	CleanupStack::PopAndDestroy(&socket16);
	CleanupStack::PopAndDestroy(&socket8);


	// 018 send from the socket bound to recv any address to another socket bound to  16-bit addressing port,
	// in presence of another socket bound to the same 8-bit application port

	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socketany.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);
	ret = socketany.Bind(addrany);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket16);
	CleanupClosePushL(socket8);
	CleanupClosePushL(socketany);

	//
	// create SMS message
	buf.Format(_L("018 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socketany);

	WaitForRecvL(socket16);
	recvMessage = RecvSmsL(socket16);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);


	CleanupStack::PopAndDestroy(&socketany);
	CleanupStack::PopAndDestroy(&socket8);
	CleanupStack::PopAndDestroy(&socket16);


	// 019 send from  socket bound to 8-bit addressing port to itself, in presence of another socket bound to
	// the same 16-bit port

	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket8);
	CleanupClosePushL(socket16);

	//
	// create SMS message
	buf.Format(_L("019 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 8 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, EFalse);

	// send EMS message
	SendSmsL(sendMessage,socket8);

	WaitForRecvL(socket8);
	recvMessage = RecvSmsL(socket8);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket16);
	CleanupStack::PopAndDestroy(&socket8);

	// 020 send from socket bound to 16-bit addressing port to itself, in presence
	// of other socket bound to the same 8-bit port

	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind it
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket16);
	CleanupClosePushL(socket8);

	//
	// create SMS message
	buf.Format(_L("020 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit port
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socket16);

	WaitForRecvL(socket16);
	recvMessage = RecvSmsL(socket16);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket8);
	CleanupStack::PopAndDestroy(&socket16);


	// 021 send from the socket bound to  8-bit addressing port, to another
	// socket bound to other 8-bit port

	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket8other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	ret = socket8other.Bind(addr8other);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket8);
	CleanupClosePushL(socket8other);

	//
	// create SMS message
	buf.Format(_L("021 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 8 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, portother, port, EFalse);

	// send SMS message
	SendSmsL(sendMessage,socket8);

	WaitForRecvL(socket8other);
	recvMessage = RecvSmsL(socket8other);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket8other);
	CleanupStack::PopAndDestroy(&socket8);


	// 022 send from the socket bound to 16-bit addressing port, to another
	// socket bound to other 16-bit port

	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	ret = socket16other.Bind(addr16other);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket16);
	CleanupClosePushL(socket16other);

	//
	// create SMS message
	buf.Format(_L("022 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, portother, port, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socket16);

	WaitForRecvL(socket16other);
	recvMessage = RecvSmsL(socket16other);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket16other);
	CleanupStack::PopAndDestroy(&socket16);

	// 023 send from the socket bound to 8-bit addressing port, to another
	// socket bound to 16-bit port

	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket8);
	CleanupClosePushL(socket16);

	//
	// create SMS message
	buf.Format(_L("023 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socket8);

	WaitForRecvL(socket16);
	recvMessage = RecvSmsL(socket16);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket16);
	CleanupStack::PopAndDestroy(&socket8);

	// 024 send from the socket bound to 16-bit addressing port, to another
	// socket bound to 8-bit port

	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket8);
	CleanupClosePushL(socket16);

	//
	// create SMS message
	buf.Format(_L("024 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, port, port, EFalse);

	// send SMS message
	SendSmsL(sendMessage,socket16);

	WaitForRecvL(socket8);
	recvMessage = RecvSmsL(socket8);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);


	CleanupStack::PopAndDestroy(&socket16);
	CleanupStack::PopAndDestroy(&socket8);


	// 025 send from the socket bound to  8-bit addressing port, to another
	// socket bound to other 8-bit port, in presence of another socket bound
	// to the same 16-bit port

	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket8other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	ret = socket16other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	ret = socket8other.Bind(addr8other);
	TEST(ret==KErrNone);

	ret = socket16other.Bind(addr16other);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket8);
	CleanupClosePushL(socket8other);
	CleanupClosePushL(socket16other);

	//
	// create SMS message
	buf.Format(_L("025 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 8 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, portother, port, EFalse);

	// send SMS message
	SendSmsL(sendMessage,socket8);

	WaitForRecvL(socket8other);
	recvMessage = RecvSmsL(socket8other);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket16other);
	CleanupStack::PopAndDestroy(&socket8other);
	CleanupStack::PopAndDestroy(&socket8);


	// 026 send from the socket bound to 16-bit addressing port, to another
	// socket bound to other 16-bit port, in presence of other socket bound
	// to the same 8-bit port

	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket8other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	ret = socket16other.Bind(addr16other);
	TEST(ret==KErrNone);

	ret = socket8other.Bind(addr8other);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket16);
	CleanupClosePushL(socket16other);
	CleanupClosePushL(socket8other);

	//
	// create SMS message
	buf.Format(_L("026 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, portother, port, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socket16);

	WaitForRecvL(socket16other);
	recvMessage = RecvSmsL(socket16other);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket8other);
	CleanupStack::PopAndDestroy(&socket16other);
	CleanupStack::PopAndDestroy(&socket16);

	// 027 send from the socket bound to 8-bit addressing port, to another
	// socket bound to 16-bit port, in presence of other socket bound
	// to the same 8-bit port

	ret = socket8.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket8other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket8.Bind(addr8);
	TEST(ret==KErrNone);

	ret = socket8other.Bind(addr8other);
	TEST(ret==KErrNone);

	ret = socket16other.Bind(addr16other);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket8);
	CleanupClosePushL(socket8other);
	CleanupClosePushL(socket16other);

	//
	// create SMS message
	buf.Format(_L("027 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 16 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, portother, port, ETrue);

	// send SMS message
	SendSmsL(sendMessage,socket8);

	WaitForRecvL(socket16other);
	recvMessage = RecvSmsL(socket16other);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//
	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket16other);
	CleanupStack::PopAndDestroy(&socket8other);
	CleanupStack::PopAndDestroy(&socket8);

	// 028 send from the socket bound to 16-bit addressing port, to another
	// socket bound to 8-bit port, in presence of another socket bound to the
	// same 16-bit port

	ret = socket8other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	ret = socket16other.Open(socketServer, KSMSAddrFamily, KSockDatagram, KSMSDatagramProtocol);
	TEST(ret==KErrNone);

	// bind them to ports
	ret = socket8other.Bind(addr8other);
	TEST(ret==KErrNone);

	ret = socket16.Bind(addr16);
	TEST(ret==KErrNone);

	ret = socket16other.Bind(addr16other);
	TEST(ret==KErrNone);

	CleanupClosePushL(socket8other);
	CleanupClosePushL(socket16);
	CleanupClosePushL(socket16other);

	//
	// create SMS message
	buf.Format(_L("028 - SendingRecevingViaApplicationPortL Msg"));
	sendMessage=CreateSmsMessageL(buf,alphabet);
	CleanupStack::PushL(sendMessage);

	// set SMS port addresing for 8 Bit ports
	sendMessage->SmsPDU().SetApplicationPortAddressingL(ETrue, portother, port, EFalse);

	// send SMS message
	SendSmsL(sendMessage,socket16);

	WaitForRecvL(socket8other);
	recvMessage = RecvSmsL(socket8other);
	CleanupStack::PushL(recvMessage);

	//compare sent and received messages
	TestSmsContentsL(recvMessage,buf);

	//

	CleanupStack::PopAndDestroy(recvMessage);
	CleanupStack::PopAndDestroy(sendMessage);

	CleanupStack::PopAndDestroy(&socket16other);
	CleanupStack::PopAndDestroy(&socket16);
	CleanupStack::PopAndDestroy(&socket8other);

    INFO_PRINTF1(_L("Releasing handle to phone power property"));
    CleanupStack::PopAndDestroy(&phonePowerProperty);

	//closing is too fast when using EKA2
	User::After(2000000);
	CleanupStack::PopAndDestroy(&socketServer);
	User::After(2000000); // wait for socket to close before reopening
    }


void CSmsInterTestStep::EnumerateAndWriteAndDeleteMessagesL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KEnumerateAndWriteAndDeleteMessages, count++ );
		}
	}

void CSmsInterTestStep::PDUSlotStoreWithEnumerationL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KPDUSlotStoreWithEnumeration, count++ );
		}
	}

void CSmsInterTestStep::DeleteMessagesWithoutEnumerationL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KDeleteMessagesWithoutEnumeration, count++ );
		}
	}

void CSmsInterTestStep::SmartMessagingL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KTestSmartMessaging, count++ );
		}
	}

void CSmsInterTestStep::RetrieveAndStoreSmsParamsL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KRetrieveAndStoreSmsParams, count++ );
		}
	}
void CSmsInterTestStep::DeletingMessageInEmptyStoreL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KDeletingMessageInEmptyStore, count++ );
		}
	}
void CSmsInterTestStep::SmsStoreMsgOfDifferentClassesL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KSmsStoreMsgOfDifferentClasses, count++ );
		}
	}
void CSmsInterTestStep::DuplicatePdusOnStoreL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KDuplicatePdusOnStore, count++ );
		}
	}
void CSmsInterTestStep::ConcatenatedMessagesL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KConcatenatedMessages, count++ );
		}
	}
void CSmsInterTestStep::OutOfMemoryWriteL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KOutOfMemoryWrite, count++ );
		}
	}
void CSmsInterTestStep::OutOfMemoryDeleteL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KOutOfMemoryDelete, count++ );
		}
	}
void CSmsInterTestStep::PublicMethodsL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KTestPublicMethods, count++ );
		}
	}

void CSmsInterTestStep::SendAndReceive7bitMsgsL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KTestSendAndReceive7bitMsgs, count++ );
		}
	}

void CSmsInterTestStep::SendAndReceive8bitMsgsL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KTestSendAndReceive8bitMsgs, count++ );
		}
	}

void CSmsInterTestStep::SendAndReceive16bitMsgsL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KTestSendAndReceive16bitMsgs, count++ );
		}
	}

void CSmsInterTestStep::SendAndReceiveConcatenatedMsgsL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KTestSendAndReceiveConcatenatedMsgs, count++ );
		}
	}
void CSmsInterTestStep::SendAndReceiveDifferentIEIsL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KTestSendAndReceiveDifferentIEIs, count++ );
		}
	}
void CSmsInterTestStep::SendAndReceiveBinaryDataL(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KTestSendAndReceiveBinaryData, count++ );
		}
	}

void CSmsInterTestStep::SendSMS2Port0000L(void)
	{
	TBool found( ETrue );
	TInt count( 0 );
	while( found )
		{
		//run the test
		found = DoSingleTestCaseL( KTestSendSMS2Port0000, count++ );
		}
	}

void CSmsInterTestStep::Connect2EsockLC(RSocketServ& aSocketServer)
/**
 *  seperating the connection to esock due to calling in every test case
 */
    {
	// delete segmentation and reassembly store files before the test
	_LIT(KReassemblyStoreName,"C:\\Private\\101F7989\\sms\\smsreast.dat");
	_LIT(KSegmentationStoreName,"C:\\Private\\101F7989\\sms\\smssegst.dat");

	TInt ret=0;
	ret=iFs.Delete(KReassemblyStoreName);
	ret=iFs.Delete(KSegmentationStoreName);

    INFO_PRINTF1(_L(" ... Connecting to Socket Server ... ") );

    ret=aSocketServer.Connect(KSocketMessageSlots);
    TEST(ret==KErrNone);
	CleanupClosePushL(aSocketServer);

    }

CSmsMessage* CSmsInterTestStep::CreateSmsMessageL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, CSmsPDU::TSmsPDUType aType)
/**
 *  Create a uninitialised SMS message
 *  @param aDes contains text that will be inserted to the pdu
 *  @param aAlphabet describes the alphabet of the pdu
 *  @return CSmsMessage* :Pointer to the created CSmsMessage object.
 */
	{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, aType, buffer);
	CleanupStack::PushL(smsMessage);

	TSmsUserDataSettings smsSettings;
	smsSettings.SetAlphabet(aAlphabet);
	smsSettings.SetTextCompressed(EFalse);
	smsMessage->SetUserDataSettingsL(smsSettings);

	smsMessage->SetToFromAddressL(iTelephoneNumber);
	smsMessage->SmsPDU().SetServiceCenterAddressL(iServiceCenterNumber);
	buffer->InsertL(0,aDes);
	CleanupStack::Pop(smsMessage);
	return smsMessage;
	}

CSmsMessage* CSmsInterTestStep::CreateSmsWithStatusReportReqL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet)
/**
 *  Create a uninitialised SMS message with Status Report request
 *  @param aDes contains text that will be inserted to the pdu
 *  @param aAlphabet describes the alphabet of the pdu
 *  @return CSmsMessage* :Pointer to the created CSmsMessage object.
 */
	{
	CSmsMessage* smsMessage=CreateSmsMessageL(aDes, aAlphabet);

	CleanupStack::PushL(smsMessage);

	//Set Status report request
	CSmsSubmit& submitPdu=(CSmsSubmit&)smsMessage->SmsPDU();
	submitPdu.SetStatusReportRequest(ETrue);

	CleanupStack::Pop(smsMessage);
	return smsMessage;
	}

void CSmsInterTestStep::OpenSmsSocketMatchTextLC(RSocketServ& aSocketServer, RSocket& aSocket, TSmsAddrFamily aFamily, TDesC8* aTextMatch)
/**
 *  Initialise an RSocket for SMS, aSocket is pushed to CleanupStack.
 *  @param aSocket The socket that will be opened
 *  @param aFamily The sms address family
 *  @param aTextMatch The text to which the socket is bound to match
 */
	{
	TInt ret=aSocket.Open(aSocketServer,KSMSAddrFamily,KSockDatagram,KSMSDatagramProtocol);
	TEST(ret==KErrNone);
	CleanupClosePushL(aSocket);

	//aFamily must be ESmsAddrMatchText!
    TEST(aFamily == ESmsAddrMatchText);

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(aFamily);
//	_LIT(KTextMatch,"\\SCK");
	smsaddr.SetTextMatch(*aTextMatch);
	User::LeaveIfError(aSocket.Bind(smsaddr));
	}

void CSmsInterTestStep::SendSimpleSmsL(RSocket& aSocket)
/**
 *  Open the socket for sending a sms message
 *  then stream the message out to the socket server
 *  and wait for request to see that the message have been sent without any error
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with error code
 */
	{
	// Create message with SRR
	_LIT(KText,"This is a simple SMS!");
	CSmsMessage* msg=CreateSmsMessageL(KText,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(msg);

	// Send message
	RSmsSocketWriteStream writestream(aSocket);
	TRAPD(ret,writestream << *msg);
	TEST(ret==KErrNone);
	TRAP(ret,writestream.CommitL());
	TEST(ret==KErrNone);

	TPckgBuf<TUint> sbuf;
	TRequestStatus status;
	aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int()==KErrNone);

	CleanupStack::PopAndDestroy(msg);
	}

void CSmsInterTestStep::SendSmsL(const CSmsMessage* aSms, RSocket& aSocket)
/**
 *  Stream aSms out to the socket server
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket is used to stream the aSms to the sms stack
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with error code
 */
	{
	TBool tryAgain = ETrue;
	TInt sendTry (0);
	TRequestStatus status = KErrNone;

	while (tryAgain && sendTry < 3)
		{
		RSmsSocketWriteStream writestream(aSocket);
		TRAPD(ret,writestream << *aSms);

		TRAP(ret,writestream.CommitL());

		TPckgBuf<TUint> sbuf;
		aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
		User::WaitForRequest(status);
		INFO_PRINTF2(_L("SendSmsL - sendSmsMessage returned %d"), status.Int());
		if (status.Int() )
			{
			tryAgain = ETrue;
			INFO_PRINTF1(_L("Try again... "));
			sendTry++;
			}
		else tryAgain = EFalse;
		}

	TEST(status.Int()==KErrNone);

    PrintMessageL(aSms);
	}

void CSmsInterTestStep::SendSmsErrorL(CSmsMessage* aSms, RSocket& aSocket)
/**
 *  Stream aSms out to the socket server. Sending is completed with error code.
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket is used to stream the aSms to the sms stack
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with KErrNone
 */
	{
	RSmsSocketWriteStream writestream(aSocket);
	TRAPD(ret,writestream << *aSms);
	TEST(ret==KErrNone);
	TRAP(ret,writestream.CommitL());
	TEST(ret==KErrNone);

	TPckgBuf<TUint> sbuf;
	TRequestStatus status;

  	User::After(50000);
	// test cancel first
	aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
	aSocket.CancelIoctl();
	User::WaitForRequest(status);
	TEST(status.Int()==KErrCancel);

	//Now send again, completed with error
	TRAP(ret,writestream << *aSms);
	TEST(ret==KErrNone);
	TRAP(ret,writestream.CommitL());
	TEST(ret==KErrNone);

	aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("SendSmsL - sendSmsMessage returned %d"), status.Int());
    PrintMessageL(aSms);
	INFO_PRINTF2(_L("Sending failed! %d"), status.Int());
    TEST(status.Int() != KErrNone);
	}

void CSmsInterTestStep::SendSmsDontCheckReturnValueL(CSmsMessage* aSms, RSocket& aSocket)
/**
 *  Stream aSms out to the socket server and don't check return value.
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket is used to stream the aSms to the sms stack
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with KErrNone
 */
	{
	RSmsSocketWriteStream writestream(aSocket);
	writestream << *aSms;
	writestream.CommitL();

	TPckgBuf<TUint> sbuf;
	TRequestStatus status;
	aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Send SMS message returned %d"), status.Int());
	if(status.Int() != KErrNone)
		User::Leave(status.Int());
	}

void CSmsInterTestStep::SendCommandSmsL(CSmsMessage* aSms, RSocket& aSocket)
/**
 *  Stream command message out to the socket server and wait for the return status
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket is used to stream the aSms to the sms stack
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with error code
 */
	{
	RSmsSocketWriteStream writestream(aSocket);
	TRAPD(ret,writestream << *aSms);
	TEST(ret==KErrNone);
	TRAP(ret,writestream.CommitL());
	TEST(ret==KErrNone);

	TRequestStatus status;
	TPckgBuf<TUint> sbuf;
	aSocket.Ioctl(KIoctlSendSmsMessage,status,&sbuf,KSolSmsProv);

	User::WaitForRequest(status);

	INFO_PRINTF2(_L("SendCommandSmsL, sendSms returned %d"), status.Int());
	User::After(1000000);
	TEST(status.Int()==KErrNone);

	}

void CSmsInterTestStep::SendSmsCancelL(CSmsMessage* aSms, RSocket& aSocket1, RSocket& aSocket2)
/**
 *  Stream Sms out to the socket server by two RSmsSocketWriteStream object.
 *  The first request is canceled and then the second request is completed with error code.
 *  @param aSms contains the sms tpdu that will be streamed to the sms stack
 *  @param aSocket1 The socket used with message that will be canceled
 *  @param aSocket2 The socket used with message that will be completed with error code
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if sending is completed with KErrNone
 */
	{
	RSmsSocketWriteStream writestream(aSocket1);
	TRAPD(ret,writestream << *aSms);
	TEST(ret==KErrNone);
	TRAP(ret,writestream.CommitL());
	TEST(ret==KErrNone);

	TPckgBuf<TUint> sbuf;
	TRequestStatus status1,status2;

	//stream to socket2
	RSmsSocketWriteStream writestream2(aSocket2);
	TRAP(ret,writestream2 << *aSms);
	TEST(ret==KErrNone);
	TRAP(ret,writestream2.CommitL());
	TEST(ret==KErrNone);

	aSocket1.Ioctl(KIoctlSendSmsMessage,status1,&sbuf, KSolSmsProv);
	aSocket2.Ioctl(KIoctlSendSmsMessage,status2,&sbuf, KSolSmsProv);

	User::After(2000000);

	// Test cancel first
	aSocket1.CancelIoctl();
	User::WaitForRequest(status1);
	TEST(status1.Int()==KErrCancel);

	User::After(50000);


	User::WaitForRequest(status2);

	INFO_PRINTF2(_L("SendSmsL - sendSmsMessage returned %d"), status2.Int());
    PrintMessageL(aSms);

	//Ensure the request is completed with error code ;)
    TEST(status2.Int() != KErrNone);
	INFO_PRINTF2(_L("Sending failed! %d"), status2.Int());
	}

void CSmsInterTestStep::SendAndRecvAndCompareMessageIEIL(const TDesC& aUserData,
											  const TDesC8& aIEIOctets,
											  RSocket& aSocket,
											  TSmsDataCodingScheme::TSmsAlphabet aSmsAlphabet,
											  CSmsInformationElement::TSmsInformationElementIdentifier aIEI)
/**
 *  Send and receive message. Compare sent and received message.
 *  @param aUserData reference to message user data
 *  @param aSocket reference to socket that is used for sending and receiving
 *  @param aSmsAlphabet alphabet info is used while creating message
 *  @param aIEI messages information element
 */
	{
	CSmsMessage* sendMessage = CreateSmsMessageL(aUserData, aSmsAlphabet);
	CleanupStack::PushL(sendMessage);
	CSmsPDU& sendPdu = sendMessage->SmsPDU();
	CSmsUserData& sendUserData = sendPdu.UserData();
	sendUserData.AddInformationElementL(aIEI,aIEIOctets);
	SendSmsL(sendMessage,aSocket);

	WaitForRecvL(aSocket);
	CSmsMessage* recvMessage = RecvSmsL(aSocket);
	CleanupStack::PushL(recvMessage);
	CSmsPDU& recvPdu = recvMessage->SmsPDU();
	CSmsUserData& recvUserData = recvPdu.UserData();

	//compare send and received messages
	TestSmsContentsL(recvMessage,aUserData);
	CSmsInformationElement& sendInformationElement = sendUserData.InformationElement(0);
	if((sendInformationElement.Identifier() != CSmsInformationElement::ESmsIEIConcatenatedShortMessages8BitReference) &&
		(sendInformationElement.Identifier() != CSmsInformationElement::ESmsIEIConcatenatedShortMessages16BitReference))
		{
		CSmsInformationElement& recvInformationElement2 = recvUserData.InformationElement(0);
		TEST(sendInformationElement.Identifier() == recvInformationElement2.Identifier());
		}
	CleanupStack::PopAndDestroy(2);	//sendMessage,recvMessage
	}

void CSmsInterTestStep::WaitForRecvL(RSocket& aSocket)
/**
 *  Wait for an Sms to be received
 *  @param aSocket The status is return to this socket
 *  @leave Leaves if receiving is completed with error code
 */
	{
	TPckgBuf<TUint> sbuf;
	sbuf()=KSockSelectRead;
	TRequestStatus status;
	aSocket.Ioctl(KIOctlSelect,status,&sbuf,KSOLSocket);
	User::WaitForRequest(status);
	TEST(status.Int()==KErrNone);
	}

TBool CSmsInterTestStep::TimedWaitForRecvL(RSocket& aSocket, TUint aDelay)
/**
 *  Wait for up to the delay for an Sms to be received
 *  @param aSocket The status is return to this socket
 *  @param aDelay Maximum time to wait for receipt
 *  @return ETrue if data received
 *  @leave Leaves if receiving is completed with error code
 */
	{
	TRequestStatus timerStatus;
	RTimer timer;
	timer.CreateLocal();
	timer.After(timerStatus, TTimeIntervalMicroSeconds32(aDelay));

	TPckgBuf<TUint> sbuf;
	sbuf()=KSockSelectRead;
	TRequestStatus ioctlStatus;
	aSocket.Ioctl(KIOctlSelect, ioctlStatus, &sbuf, KSOLSocket);
	User::WaitForRequest(timerStatus, ioctlStatus);
    TBool retval;
	if(ioctlStatus.Int() != KErrNone)
		{
		aSocket.CancelIoctl();
    	User::WaitForRequest(ioctlStatus);
        retval = EFalse;
		}
    else
        {
        timer.Cancel();
    	User::WaitForRequest(timerStatus);
        retval = ETrue;
        }
    timer.Close();
    return retval;
	}

CSmsMessage* CSmsInterTestStep::RecvSmsL(RSocket& aSocket, TInt aIoctl)
/**
 *  Receive an Sms
 *  @param aSocket is used to stream the sms message from the socket server
 *  @return CSmsMessage* :Sms message from Sms stack
 *  @leave Leaves if streaming the message from the socket server doesn't succeed
 */
	{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);

	RSmsSocketReadStream readstream(aSocket);
	TRAPD(ret,readstream >> *smsMessage);
	TEST(ret==KErrNone);

	TPckgBuf<TUint> sbuf;
	TRequestStatus status;
	aSocket.Ioctl(aIoctl, status, &sbuf, KSolSmsProv);
	User::WaitForRequest(status);

	CleanupStack::Pop(smsMessage);
	return smsMessage;
	}

CSmsMessage* CSmsInterTestStep::RecvSmsFailedL(RSocket& aSocket)
/**
 *  Receive an Sms, first nack the receive several times before succeeding
 *  @param aSocket is used to stream the sms message from the socket server
 *  @return CSmsMessage* :Sms message from Sms stack
 *  @leave Leaves if streaming the message from the socket server doesn't succeed
 *  @leave Leaves if nack of receiving is completed with error code
 *  @leave Leaves if ack of receiving is completed with error code
 */
	{
	CSmsBuffer* buffer=CSmsBuffer::NewL();
	CSmsMessage* smsMessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsSubmit,buffer);
	CleanupStack::PushL(smsMessage);
	RSmsSocketReadStream readstream(aSocket);

	TPckgBuf<TUint> sbuf;
	TRequestStatus status;

	for(TInt i=0; i<10; i++)
		{
		TRAPD(ret,readstream >> *smsMessage);
		TEST(ret==KErrNone);
		aSocket.Ioctl(KIoctlReadMessageFailed, status, &sbuf, KSolSmsProv);
		User::WaitForRequest(status);
		TEST(status.Int()==KErrNone);
		}

	TRAPD(ret,readstream >> *smsMessage);
	TEST(ret==KErrNone);
	aSocket.Ioctl(KIoctlReadMessageSucceeded, status, NULL, KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int()==KErrNone);

	CleanupStack::Pop(smsMessage);
	return smsMessage;
	}

void CSmsInterTestStep::RecvStatusReportL(TSmsServiceCenterAddress& aRecipientNumber, RSocket& aSocket)
/**
 *  Receive a Status report
 *  @param aRecipientNumber The supposed recipient number
 *  @param aSocket is used to stream the sms message from the socket server
 */
	{
	//Receive SMS
	WaitForRecvL(aSocket);
	CSmsMessage* smsMessage = RecvSmsL(aSocket);
    CleanupStack::PushL(smsMessage);

	//Check the status report
	TBool isSR = (smsMessage->Type()==CSmsPDU::ESmsStatusReport);
	if (isSR)
		{
		INFO_PRINTF1(_L("Received status report"));
		TSmsServiceCenterAddress telephoneNumber=smsMessage->ToFromAddress();
		TEST(telephoneNumber==aRecipientNumber);
		INFO_PRINTF2(_L("Message delivered to %S "),&telephoneNumber);
		}
	else
		INFO_PRINTF1(_L("Received SMS is NOT a Status report!"));

	TEST(isSR);

	CleanupStack::PopAndDestroy(smsMessage);
	}

void CSmsInterTestStep::ReadSmsStoreL(RSocket& aSocket, RPointerArray<CSmsMessage>& aMessages)
/**
 *  This method retrieves SMS messages from SMS storage and print them out.
 *  @param aSocket Used to stream SMS messages from the socket server
 *  @param aMessages Sms messages will be streamed to this array
 *  @leave Leaves if first request is NOT completed with KErrCancel
 *  @leave Leaves if second request is completed with error code
 *  @leave Leaves if streaming the message from the socket server doesn't succeed
 *  @leave Leaves if nack of reading is completed with error code
 *  @leave Leaves if ack of reading is completed with error code
 */
	{
	INFO_PRINTF1(_L("Enumerating messages"));

	TRequestStatus status;

	TPckgBuf<TUint> sbuf;
	sbuf()=0;
	
	// test cancel first - due to Lubbock problems canceled out
    aSocket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
    aSocket.CancelIoctl(); 
    User::WaitForRequest(status);
    INFO_PRINTF2(_L(" status = %d "),status.Int());
    TEST((status.Int()==KErrCancel) ||(status.Int()==KErrNone));
    
    User::After(2000000);
    
	//Now enumerate messages from store
	aSocket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	TEST(status.Int()==KErrNone);
	
	   User::After(2000000);
	
	
	
	//sbuf() includes the count of messages on Store
	TInt count = sbuf();
	INFO_PRINTF2(_L("%d enumerated messages"), count);

	RSmsSocketReadStream readstream(aSocket);

	//Read each message from the stream
	for(TInt i=0; i< count; i++)
		{
		CSmsBuffer* buffer=CSmsBuffer::NewL();
		CSmsMessage* smsmessage=CSmsMessage::NewL(iFs, CSmsPDU::ESmsDeliver,buffer);
		CleanupStack::PushL(smsmessage);

		TRAPD(ret,readstream >> *smsmessage);
		TEST(ret==KErrNone);
		aSocket.Ioctl(KIoctlReadMessageFailed, status, NULL, KSolSmsProv);
		User::WaitForRequest(status);
		TEST(status.Int()==KErrNone);

		TRAP(ret,readstream >> *smsmessage);
		TEST(ret==KErrNone);
		aSocket.Ioctl(KIoctlReadMessageSucceeded, status, NULL, KSolSmsProv);
		User::WaitForRequest(status);
		TEST(status.Int()==KErrNone);

		INFO_PRINTF1(smsmessage->ToFromAddress());
		INFO_PRINTF1(_L("     "));
		INFO_PRINTF1(smsmessage->ServiceCenterAddress());
		INFO_PRINTF1(_L(""));

		if(smsmessage->Storage() == CSmsMessage::ESmsSIMStorage)
			INFO_PRINTF1(_L("Store: SIM"));
		else if (smsmessage->Storage() == CSmsMessage::ESmsPhoneStorage)
			INFO_PRINTF1(_L("Store: Phone"));
		else if (smsmessage->Storage() == CSmsMessage::ESmsCombinedStorage)
			INFO_PRINTF1(_L("Store: Combined"));
		else INFO_PRINTF1(_L("Store: Unknown"));


		switch (smsmessage->Status())
			{
			case RMobileSmsStore::EStoredMessageUnread:
				INFO_PRINTF1(_L("Status: Unread"));
				break;

			case RMobileSmsStore::EStoredMessageRead:
				INFO_PRINTF1(_L("Status: Read"));
				break;

			case RMobileSmsStore::EStoredMessageUnsent:
				INFO_PRINTF1(_L("Status: Unsent"));
				break;

			case RMobileSmsStore::EStoredMessageSent:
				INFO_PRINTF1(_L("Status: Sent"));
				break;

			case RMobileSmsStore::EStoredMessageDelivered:
				INFO_PRINTF1(_L("Status: Delivered"));
				break;

			case RMobileSmsStore::EStoredMessageUnknownStatus:
			default:
				INFO_PRINTF1(_L("Status: Unknown"));
				break;
			}

        PrintMessageL(smsmessage);
		User::LeaveIfError(aMessages.Append(smsmessage));
		CleanupStack::Pop(smsmessage);
		}
	}

TInt CSmsInterTestStep::MakeReadSmsStoreRequestL(RSocket& aSocket)
/**
 *  This method retrieves SMS messages from SMS storage.
 *  Main purpose is to test out of memory conditions.
 *  @param aSocket Used to stream SMS messages from the sms stack
 *  @param aMessages reference to CSmsMessage pointer array.
 */
	{
	TRequestStatus status;
	TPckgBuf<TUint> sbuf;
	sbuf()=0;

	//Now enumerate messages from SIM
	aSocket.Ioctl(KIoctlEnumerateSmsMessages,status,&sbuf, KSolSmsProv);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Read SMS returned %d"), status.Int());
	if(status.Int() != KErrNone)
		User::Leave(status.Int());

	//sbuf() includes the count of messages on SIM
	return sbuf();
	}

void CSmsInterTestStep::WriteSmsToSimL(CSmsMessage& aSms, RSocket& aSocket, TDes8* aDesc)
/**
 *  This method stores SMS messages to the SMS storage.
 *  @param aSms SMS message that will be stored
 *  @param aSocket Used to stream SMS message to the sms stack
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if store request is completed with error code
 */
	{
	INFO_PRINTF1(_L("Write message"));

	TRequestStatus status;

	RSmsSocketWriteStream writestream(aSocket);
	TRAPD(ret,writestream << aSms);
	TEST(ret==KErrNone);
	TRAP(ret,writestream.CommitL());
	TEST(ret==KErrNone);
	aSocket.Ioctl(KIoctlWriteSmsMessage,status,aDesc, KSolSmsProv);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("WriteSmsToSimL - returned %d"), status.Int());
	}

TInt CSmsInterTestStep::CancelWriteSmsToSimL(CSmsMessage& aSms, RSocket& aSocket, TInt aDelay)
/**
 *  This method stores SMS messages to the SMS storage.
 *  @param aSms SMS message that will be stored
 *  @param aSocket Used to stream SMS message to the sms stack
 *  @leave Leaves if streaming the message to the socket server doesn't succeed
 *  @leave Leaves if store request is completed with error code
 */
	{
	INFO_PRINTF1(_L("Write message"));

	TRequestStatus status;

	RSmsSocketWriteStream writestream(aSocket);
	TRAPD(ret,writestream << aSms);
	TEST(ret==KErrNone);
	TRAP(ret,writestream.CommitL());
	TEST(ret==KErrNone);

	aSocket.Ioctl(KIoctlWriteSmsMessage,status,NULL, KSolSmsProv);
	INFO_PRINTF1(_L("Cancel"));
	User::After(aDelay);
	aSocket.CancelIoctl();
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("WriteSmsToSimL - returned %d"), status.Int());

	return status.Int();
	}

void CSmsInterTestStep::WriteMessagesUntilStoreComesFullL(RPointerArray<CSmsMessage>& aMessages, const CSmsMessage::TMobileSmsStorage& aStorage, RSocket& aSocket)
/**
 *  This method writes SMS messages to the SMS storage until SMS Stack completes
 *  write request with KErrNoMemory.
 *  @param aMessages an Array to where all successfully written PDUs that are stored
 *  @param aStorage a store where message will be stored
 *  @param aSocket Used to stream SMS message to the sms stack
 *  @return TInt amounth of successfully writed messages
 */
	{
	TBuf<20> msgBuffer;
	_LIT(KTestMessage,"Test message ");
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	TInt writeCount(0);
	TInt ret(KErrNone);
    while (ret==KErrNone)
        {
		msgBuffer.Copy(KTestMessage);
		msgBuffer.AppendNum(writeCount);
		CSmsMessage* smsMessage=CreateSmsMessageL(msgBuffer,alphabet,CSmsPDU::ESmsDeliver);
		CleanupStack::PushL(smsMessage);
		smsMessage->SetStorage(aStorage);
        TRAP(ret,WriteSmsLeaveIfErrorL(*smsMessage,aSocket););
		if(ret==KErrNone)
			{
			User::LeaveIfError(aMessages.Append(smsMessage));
			CleanupStack::Pop(smsMessage);
			writeCount++;
			}
		else
			CleanupStack::PopAndDestroy(smsMessage);
        }
	TEST(ret == KErrNoMemory || ret == KErrNotFound);	// KErrNotFound means requested store n/a
	}

void CSmsInterTestStep::WriteSmsLeaveIfErrorL(const CSmsMessage& aSms, RSocket& aSocket)
/**
 *  This method stores SMS messages to the SMS storage.
 *  @param aSms SMS message that will be stored
 *  @param aSocket Used to stream SMS message to the sms stack
 */
	{
	TRequestStatus status;
	RSmsSocketWriteStream writestream(aSocket);
	writestream << aSms;
	writestream.CommitL();
	aSocket.Ioctl(KIoctlWriteSmsMessage,status,NULL, KSolSmsProv);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Write SMS message returned %d"), status.Int());
	if(status.Int() != KErrNone)
		User::Leave(status.Int());
	}

TInt CSmsInterTestStep::DeleteSmsL(const CSmsMessage& aSms, RSocket& aSocket)
/**
 *  This method deletes SMS message from the SMS storage.
 *  @param aSms SMS message that will be deleted
 *  @param aSocket Used to stream SMS message to the sms stack
 *  @leave Leaves if delete request is completed with error code (except KErrNotFound)
 *  @return TInt :error code
 */
	{
	INFO_PRINTF1(_L("Delete message"));

	TRequestStatus status;

	RSmsSocketWriteStream writestream(aSocket);
	TRAPD(ret,writestream << aSms);
	TEST(ret==KErrNone);
	TRAP(ret,writestream.CommitL());
	TEST(ret==KErrNone);

	aSocket.Ioctl(KIoctlDeleteSmsMessage, status, NULL, KSolSmsProv);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("DeleteSmsL - returned %d"), status.Int());

	if (status.Int()!=KErrArgument)
		TEST(status.Int() == KErrNone);

	return status.Int();
	}

void CSmsInterTestStep::DeleteSmsLeaveIfErrorL(const CSmsMessage& aSms, RSocket& aSocket)
/**
 *  This method deletes SMS message from the SMS storage.
 *  @param aSms SMS message that will be deleted
 *  @param aSocket Used to stream SMS message to the sms stack
 */
	{
	TRequestStatus status;
	RSmsSocketWriteStream writestream(aSocket);
	writestream << aSms;
	writestream.CommitL();
	aSocket.Ioctl(KIoctlDeleteSmsMessage, status, NULL, KSolSmsProv);
	User::WaitForRequest(status);
	INFO_PRINTF2(_L("Delete SMS returned %d"), status.Int());
	if(status.Int() != KErrNone)
		User::Leave(status.Int());
	}

void CSmsInterTestStep::TestSmsContentsL(CSmsMessage* aSms, const TDesC& aDes)
/**
 *  Check that aSms contains text that matches to aDes
 *  @param aSms SMS message that has been come from SMS stack
 *  @param aDes SMS message's text that is defined at client side
 *  @leave Leaves if aSms doesn't match to aDes
 */
	{
	CSmsBufferBase& smsBuffer=aSms->Buffer();
	TInt bufLen=smsBuffer.Length();
	HBufC* textBuf=HBufC::NewL(bufLen);
	CleanupStack::PushL(textBuf);
	TPtr textPtr(textBuf->Des());
	smsBuffer.Extract(textPtr,0,bufLen);
	TEST(textPtr==aDes);
	INFO_PRINTF1(_L("Incoming matches outgoing !") );
	CleanupStack::PopAndDestroy(textBuf);
	}

void CSmsInterTestStep::TestSmsContentsL(CSmsMessage* aSms, const CSmsBufferBase& aSmsBuffer )
/**
 *  Check that aSms contains text that matches to aDes
 *  @param aSms SMS message that has been come from SMS stack
 *  @param aDes SMS message's text that is defined at client side
 *  @leave Leaves if aSms doesn't match to aDes
 */
	{
	CSmsBufferBase& smsBuffer=aSms->Buffer();
	TInt bufLen=smsBuffer.Length();
	HBufC* textBuf=HBufC::NewL(bufLen);
	CleanupStack::PushL(textBuf);
	TPtr textPtr(textBuf->Des());
	smsBuffer.Extract(textPtr,0,bufLen);

	TInt bufLen2=aSmsBuffer.Length();
	HBufC* textBuf2=HBufC::NewL(bufLen2);
	CleanupStack::PushL(textBuf2);
	TPtr textPtr2(textBuf2->Des());
	aSmsBuffer.Extract(textPtr2,0,bufLen2);

	TEST(textPtr==textPtr2);
	INFO_PRINTF1(_L("Incoming matches outgoing !") );
	CleanupStack::PopAndDestroy(textBuf2);
	CleanupStack::PopAndDestroy(textBuf);
	}

void CSmsInterTestStep::TestSmsListsL(RPointerArray<CSmsMessage>& aLeft,RPointerArray<CSmsMessage>& aRight)
/**
 *  Compare sms lists
 *  @param aLeft an Array containing CSmsMessage objects
 *  @param aRight an Array containing CSmsMessage objects
 *  @return TInt : The error code
 */
	{
    if(aLeft.Count() == aRight.Count())
        {
        for(TInt leftIndex=0; leftIndex<aLeft.Count(); leftIndex++)
            {
			TBool found=EFalse;
            CSmsMessage* leftEntry=aLeft[leftIndex];

			CSmsBufferBase& leftBuffer=leftEntry->Buffer();
			TInt bufLen=leftBuffer.Length();
			HBufC* leftTextBuf=HBufC::NewL(bufLen);
			CleanupStack::PushL(leftTextBuf);
			TPtr leftTextPtr(leftTextBuf->Des());
			leftBuffer.Extract(leftTextPtr,0,bufLen);

            for(TInt rightIndex=0; rightIndex<aRight.Count(); rightIndex++)
                {
                CSmsMessage* rigthEntry=aRight[rightIndex];

				CSmsBufferBase& rightBuffer=rigthEntry->Buffer();
				bufLen=rightBuffer.Length();
				HBufC* rigthTextBuf=HBufC::NewL(bufLen);
				CleanupStack::PushL(rigthTextBuf);
				TPtr rightTextPtr(rigthTextBuf->Des());
				rightBuffer.Extract(rightTextPtr,0,bufLen);

                if((leftTextPtr == rightTextPtr) &&
					/* 9210 does not report SCA (leftEntry->ServiceCenterAddress() == rigthEntry->ServiceCenterAddress()) &&*/
					(leftEntry->ToFromAddress() == rigthEntry->ToFromAddress()) &&
					(leftEntry->Storage() == rigthEntry->Storage()) &&
					(leftEntry->Status() == rigthEntry->Status()))
					{
					found=ETrue;
                    }
				CleanupStack::PopAndDestroy(rigthTextBuf);
				if(found)
					break;
                }
			CleanupStack::PopAndDestroy(leftTextBuf);

            if(!found)
                {
			    INFO_PRINTF1(_L("ERROR: SMS lists doesn't match: KErrCorrupt"));
                User::Leave(KErrNotFound);
                }
            }
        }
    else
    	{
	    INFO_PRINTF1(_L("ERROR: SMS lists doesn't match: KErrCorrupt"));
        User::Leave(KErrCorrupt);
        }

    INFO_PRINTF1(_L("SMS list comparing Ok!") );
	}

void CSmsInterTestStep::DoSendSmsL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RSocket& aSocket)
/**
 *  Send an SMS
 *  @param aDes contains text that will be inserted to the pdu at CreateSmsMessageL
 *  @param aAlphabet describes the alphabet of the pdu that will be created at CreateSmsMessageL
 *  @leave Leaves if any of the leaving functions used at this function leaves
 */
	{
	CSmsMessage* smsMessage=CreateSmsMessageL(aDes, aAlphabet);
	CleanupStack::PushL(smsMessage);
	SendSmsL(smsMessage, aSocket);
	CleanupStack::PopAndDestroy(smsMessage);
	}

void CSmsInterTestStep::SendSms7BitL(const TDesC& aDes, RSocket& aSocket)
/**
 *  Send a 7bit sms
 *  @param aDes contains the text to be send
 *  @leave Leaves if DoSendSmsL leaves
 */
	{
	DoSendSmsL(aDes, TSmsDataCodingScheme::ESmsAlphabet7Bit, aSocket);
	}

void CSmsInterTestStep::SendSms8BitL(const TDesC& aDes, RSocket& aSocket)
/**
 *  Send a 8bit sms
 *  @param aDes contains the text to be send
 *  @leave Leaves if DoSendSmsL leaves
 */
	{
	DoSendSmsL(aDes, TSmsDataCodingScheme::ESmsAlphabet8Bit, aSocket);
	}

void CSmsInterTestStep::SendSms16BitL(const TDesC& aDes, RSocket& aSocket)
/**
 *  Send a 16bit sms
 *  @param aDes contains the text to be send
 *  @leave Leaves if DoSendSmsL leaves
 */
	{
	DoSendSmsL(aDes, TSmsDataCodingScheme::ESmsAlphabetUCS2, aSocket);
	}


void CSmsInterTestStep::DoSendAndRecvSmsL(const TDesC& aDes, TSmsDataCodingScheme::TSmsAlphabet aAlphabet, RSocket& aSocket)
/**
 *  Send and recv one sms,
 *  then check that the sent message corresponds with the received message
 *  @param aDes contains the text that will be inserted to the pdu at CreateSmsMessageL
 *  @param aAlphabet describes the alphabet of the pdu that will be created at CreateSmsMessageL
 *  @leave Leaves if any of the leaving functions used at this function leaves
 */
	{
	CSmsMessage* smsMessage=CreateSmsMessageL(aDes, aAlphabet);
	CleanupStack::PushL(smsMessage);
	SendSmsL(smsMessage, aSocket);
	CleanupStack::PopAndDestroy(smsMessage); //destroyed because created again in RecvSmsL

	WaitForRecvL( aSocket);
	smsMessage = RecvSmsL( aSocket);
	CleanupStack::PushL(smsMessage);

	TestSmsContentsL(smsMessage,aDes);

	User::After(1000000);

	CleanupStack::PopAndDestroy(smsMessage);
	}


void CSmsInterTestStep::SendAndRecvSms7BitL(const TDesC& aDes, RSocket& aSocket)
/**
 *  Send and receive one 7bit sms
 *  @param aDes contains the text to be send
 *  @leave Leaves if DoSendAndRecvSmsL leaves
 */
	{
	DoSendAndRecvSmsL(aDes,TSmsDataCodingScheme::ESmsAlphabet7Bit, aSocket);
	}

void CSmsInterTestStep::SendAndRecvSms8BitL(const TDesC& aDes, RSocket& aSocket)
/**
 *  Send and receive one 8bit sms
 *  @param aDes contains the text to be send
 *  @leave Leaves if DoSendAndRecvSmsL leaves
 */
	{
	DoSendAndRecvSmsL(aDes,TSmsDataCodingScheme::ESmsAlphabet8Bit, aSocket);
	}

void CSmsInterTestStep::SendAndRecvSmsBinaryL(const TDesC& aDes, RSocket& aSocket)
/**
 *  Send and recv one sms containing binary data
 *  @param aDes contains the text to be send
 *  @leave Leaves if any of the leaving functions used at this function leaves
 */
	{
	CSmsMessage* smsMessage=CreateSmsMessageL(aDes,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(smsMessage);

	CSmsPDU& pdu = smsMessage->SmsPDU();
	CSmsUserData& userData = pdu.UserData();
	userData.AddInformationElementL(CSmsInformationElement::ESmsIEIApplicationPortAddressing8Bit,_L8("98"));

	SendSmsL(smsMessage, aSocket);
	CleanupStack::PopAndDestroy(smsMessage); //destroyed because created again in RecvSmsL
	WaitForRecvL( aSocket);
	smsMessage = RecvSmsL( aSocket);
	CleanupStack::PushL(smsMessage);
	TestSmsContentsL(smsMessage,aDes);
	CleanupStack::PopAndDestroy(smsMessage);
	}

void CSmsInterTestStep::SendAndRecvSms16BitL(const TDesC16& aDes, RSocket& aSocket)
/**
 *  Send and receive one 16bit sms
 *  @param aDes contains the text to be send
 *  @leave Leaves if DoSendAndRecvSmsL leaves
 */
	{
	DoSendAndRecvSmsL(aDes,TSmsDataCodingScheme::ESmsAlphabetUCS2, aSocket);
	}

void CSmsInterTestStep::FillDes(TDes& aDes,TInt aLength)
/**
 *  Fill aDes with randomly generated 7bit data
 *  @param aDes will be filled with random data
 *  @param aLength has the length to be set to aDes
 */
	{
	TText baseChar='A';

	aDes.SetLength(aLength);
	for (TInt i=0;i<aLength;i++)
		aDes[i]=TText(baseChar + i%26 );
	}

void CSmsInterTestStep::SendAndRecvSms7BitL(TInt aLength, RSocket& aSocket)
/**
 *  Send and recv one sms of aLength with arbitrarily generated text
 *  @param aLength has the length of the message to be created here
 *  @leave Leaves if SendAndRecvSms7BitL leaves
 */
	{
	HBufC* textBuf=HBufC::New(aLength);
	CleanupStack::PushL(textBuf);
	TPtr textPtr=textBuf->Des();
	FillDes(textPtr,aLength);
	SendAndRecvSms7BitL(textPtr, aSocket);
	CleanupStack::PopAndDestroy(textBuf);
	}

void CSmsInterTestStep::BinaryFillDes(TDes& aDes, TInt aLength)
/**
 *  Fill aDes with randomly initialised 8bit data
 *  @param aDes will be filled with random data
 *  @param aLength has the length to be set to aDes
 */
	{
	TTime time;
	time.UniversalTime();
	TInt64 timeAs64BitInt = time.Int64();

	TText8 baseChar = TText8( Math::Rand(timeAs64BitInt) );
	aDes.SetLength(aLength);
	for (TInt i=0;i<aLength;i++)
		{
		aDes[i] = TText8(baseChar++);
		}
	}

void CSmsInterTestStep::SendAndRecvSms8BitL(TInt aLength, RSocket& aSocket)
/**
 *  Send and recv one 8bit sms of aLength with arbitrarily generated text
 *  @param aLength has the length of the message
 *  @leave Leaves if SendAndRecvSms8BitL leaves
 */
	{
	HBufC* textBuf=HBufC::New(aLength);
	CleanupStack::PushL(textBuf);
	TPtr textPtr=textBuf->Des();
	FillDes(textPtr,aLength);
	SendAndRecvSms8BitL(textPtr, aSocket);
	CleanupStack::PopAndDestroy(textBuf);
	}

void CSmsInterTestStep::SendAndRecvSmsBinaryL(TInt aLength, RSocket& aSocket)
/**
 *  Send and recv one binary sms message of aLength with arbitrarily generated text
 *  @param aLength has the length of the message
 *  @leave Leaves if SendAndRecvSmsBinaryL leaves
 */
	{
	HBufC* textBuf=HBufC::New(aLength);
	CleanupStack::PushL(textBuf);
	TPtr textPtr=textBuf->Des();
	BinaryFillDes(textPtr,aLength);
	SendAndRecvSmsBinaryL(textPtr, aSocket);
	CleanupStack::PopAndDestroy(textBuf);
	}

void CSmsInterTestStep::SendAndRecvSms16BitL(TInt aLength, RSocket& aSocket)
/**
 *  Send and recv one 16bit sms of aLength with arbitrarily generated text
 *  @param aLength has the length of the message
 *  @leave Leaves if SendAndRecvSms16BitL leaves
 */
	{
	HBufC16* textBuf=HBufC16::New(aLength);
	CleanupStack::PushL(textBuf);
	TPtr16 textPtr=textBuf->Des();
	FillDes(textPtr,aLength);
	SendAndRecvSms16BitL(textPtr, aSocket);
	CleanupStack::PopAndDestroy(textBuf);
	}

void CSmsInterTestStep::TestMessageContentsL(CSmsMessage* aSms, const TTestCase& aTestCase)
/**
 *  Check the sms matches the expected test result
 *  @param aSms has the message to be tested with aTestCase.iMsg
 *  @param aTestCase has information about the used test message, e.g. message data and DCS
 *  @leave Leaves if TSmsClass isn't defined at the pdu
 *  @leave Leaves if class of pdu doesn't match to supposed class (aTestCase.iSmsClass)
 */
	{
	TestSmsContentsL(aSms,aTestCase.iMsg);
	CSmsPDU& pdu = aSms->SmsPDU();
	INFO_PRINTF1(_L("Send message too long for SIM"));
	if (aTestCase.iTestSmsClass)
		{
		INFO_PRINTF1(_L("Send message too long for SIM"));
		TSmsDataCodingScheme::TSmsClass smsClass;
		TBool isDefined = pdu.Class(smsClass);
		TEST(isDefined);
		TEST(smsClass == aTestCase.iSmsClass);
		}
	INFO_PRINTF1(_L("Send message too long for SIM"));
	if (aTestCase.iTestIndicators)
		{
		INFO_PRINTF1(_L("Send message too long for SIM"));
		TEST(pdu.Bits7To4() == aTestCase.iBits7To4);
		TEST(pdu.IndicationType() == aTestCase.iIndicationType);
		TEST(pdu.IndicationState() == aTestCase.iIndicationState);
		}
	INFO_PRINTF1(_L("Send message too long for SIM"));
	}

void CSmsInterTestStep::SetIndicatorL(const TTestCase& aTestCase, CSmsMessage* aSms)
/**
 *  
 */
	{
	TSmsDataCodingScheme::TSmsAlphabet alphabet;
	if (aTestCase.iBits7To4 == TSmsDataCodingScheme::ESmsDCSMessageWaitingIndicationUCS2)
		alphabet = TSmsDataCodingScheme::ESmsAlphabetUCS2;
	else
		alphabet = TSmsDataCodingScheme::ESmsAlphabet7Bit;

	TSmsUserDataSettings smsSettings;
	smsSettings.SetAlphabet(alphabet);
	smsSettings.SetTextCompressed(EFalse);
	aSms->SetUserDataSettingsL(smsSettings);

	CSmsPDU& pdu = aSms->SmsPDU();
	pdu.SetBits7To4(aTestCase.iBits7To4);
	pdu.SetIndicationType(aTestCase.iIndicationType);
	pdu.SetIndicationState(aTestCase.iIndicationState);

	}

void CSmsInterTestStep::SendAndRecvTestMessageL(const TTestCase& aTestCase, RSocket& aSocket)
/**
 *  Send a test message. This method is used to send and receive different DCS type messages
 *  @param aTestCase has information about the used test message, e.g. message data and DCS
 *  @leave Leaves if any of the leaving functions used at this function leaves
 */
	{
	SendTestMessageL(aTestCase, aSocket);
	INFO_PRINTF1(_L("(SendTestMessageL"));
	WaitForRecvL(aSocket);
	INFO_PRINTF1(_L("(WaitForRecvL(aSocket)"));
	CSmsMessage* smsMessage = RecvSmsL(aSocket);
	 INFO_PRINTF1(_L("(RecvSmsL(aSocket)"));
	CleanupStack::PushL(smsMessage);
	INFO_PRINTF1(_L("(RecvSmsL(aSocket)"));
	TestMessageContentsL(smsMessage,aTestCase);
	CleanupStack::PopAndDestroy(smsMessage);
	}

void CSmsInterTestStep::SendTestMessageL(const TTestCase& aTestCase, RSocket& aSocket)
/**
 *  Send a test message
 *  Assumes recv is already done.
 *  @param aTestCase has information about the used test message, e.g. message data and DCS
 *  @leave Leaves if any of the leaving functions used at this function leaves
 */
	{
	CSmsMessage* smsMessage = CreateSmsMessageL(aTestCase.iMsg, TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(smsMessage);

	CSmsPDU& pdu = smsMessage->SmsPDU();
	CSmsUserData& userData = pdu.UserData();

	if (aTestCase.iMatchType == ESmsAddrMatchIEI)
		{
		userData.AddInformationElementL(aTestCase.iIdentifierMatch,_L8("98"));
		}

	if (aTestCase.iTestSmsClass)
		{
		pdu.SetBits7To4(TSmsDataCodingScheme::ESmsDCSTextUncompressedWithClassInfo);
		pdu.SetClass(ETrue,aTestCase.iSmsClass);
		}

	if (aTestCase.iTestValidityPeriod && pdu.Type()==CSmsPDU::ESmsSubmit)
		{
		CSmsSubmit* submitPdu = REINTERPRET_CAST(CSmsSubmit*,&pdu);
		submitPdu->SetValidityPeriod(aTestCase.iValidityPeriod);
		}

	if (aTestCase.iTestIndicators && pdu.Type()==CSmsPDU::ESmsSubmit)
		{
		SetIndicatorL(aTestCase, smsMessage);
		}

	SendSmsL(smsMessage, aSocket);
	CleanupStack::PopAndDestroy(smsMessage);
	}

TInt CSmsInterTestStep::WriteReadAndDeleteMessageL(const TTestCase& aTestCase, const CSmsMessage::TMobileSmsStorage& aSmsStorage, RSocket& aSocket)
/**
 *  Write, read and delete a message to/from phone side SMS storage
 *  Assumes phone side storage is empty
 *  @param aTestCase has information about the used test message, e.g. message data and DCS
 *  @leave Leaves if any of the leaving functions used at this function leaves
 */
	{
//
// Write message
//
	CSmsMessage* smsMessage = CreateSmsMessageL(aTestCase.iMsg, TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(smsMessage);
	smsMessage->SetStorage(aSmsStorage);
	CSmsPDU& pdu = smsMessage->SmsPDU();
	if (aTestCase.iTestSmsClass)
		{
		pdu.SetBits7To4(TSmsDataCodingScheme::ESmsDCSTextUncompressedWithClassInfo);
		pdu.SetClass(ETrue,aTestCase.iSmsClass);
		if(aTestCase.iSmsClass==TSmsDataCodingScheme::ESmsClass0)
			INFO_PRINTF1(_L("Writing Class 0 Message ..."));
		else if(aTestCase.iSmsClass==TSmsDataCodingScheme::ESmsClass1)
			INFO_PRINTF1(_L("Writing Class 1 Message ..."));
		else if(aTestCase.iSmsClass==TSmsDataCodingScheme::ESmsClass2)
			INFO_PRINTF1(_L("Writing Class 2 Message ..."));
		else if(aTestCase.iSmsClass==TSmsDataCodingScheme::ESmsClass3)
			INFO_PRINTF1(_L("Writing Class 3 Message ..."));
		}

	TRAPD(ret,WriteSmsLeaveIfErrorL(*smsMessage,aSocket););
	// Some phones doesn't support phone memory. In this case SMS Stack
	// completes write request with KErrNotFound.
	// Some phones has phone memory that can be used only by phone
	// when SIM comes full (6210). Store info from this kind of
	// phone memory tells that iTotalEntries==iUsedEntries. In this case
	// SMS Stack completes write request with KErrNoMemory.
	if(ret == KErrNoMemory || ret == KErrNotFound)
		{
		TEST(ret==KErrNone);
		CleanupStack::PopAndDestroy(smsMessage);
		return ret;
		}
	else if(ret != KErrNone)
		{
		User::Leave(ret);
		}

//
// Read message and compare it to written message
//
	RPointerArray<CSmsMessage> messages;
	CleanupResetAndDestroyPushL(messages);
	ReadSmsStoreL(aSocket, messages);
	TestMessageContentsL(smsMessage,aTestCase);
	TEST(smsMessage->ToFromAddress()==messages[0]->ToFromAddress());
	TEST(smsMessage->ServiceCenterAddress()==messages[0]->ServiceCenterAddress());

//
// Delete message
//
	ret = DeleteSmsL(*messages[0], aSocket);
	TEST(ret==KErrNone);

	CleanupStack::PopAndDestroy(2); // smsMessage, messages
	return KErrNone;
	}

void CSmsInterTestStep::SendMessagesUntilKeyPressL(RSocketServ& aSocketServer)
/**
 *  Keep sending messages until a key is pressed
 *  @leave Leaves if any of the leaving functions used at this function leaves
 */
	{
	CActiveSender* activeSender = CActiveSender::NewL(aSocketServer, iFs, iTelephoneNumber,
                                                      iServiceCenterNumber);
	CleanupStack::PushL(activeSender);

	activeSender->StopIfKeyPressedL();
	activeSender->StartSendingL();
	CleanupStack::PopAndDestroy(activeSender);
	}


void CSmsInterTestStep::PrintMessageL(const CSmsMessage* aSms)
/**
 *  Print the content of SMS to the console
 */
	{
	CSmsBuffer& smsbuffer = (CSmsBuffer&)aSms->Buffer();
	const TInt len = smsbuffer.Length();
	HBufC* hbuf = HBufC::NewL(len);
	TPtr ptr = hbuf->Des();
	smsbuffer.Extract(ptr,0,len);
	INFO_PRINTF1(_L("SMS contains..."));

	for (TInt j = 0; j < len; j++)
		{
		if (ptr[j] < 0x20  ||  ptr[j] > 0xff)
			{
			ptr[j] = 0x007f;
			}
		}

	INFO_PRINTF1(ptr);
	INFO_PRINTF1(_L(""));

	delete hbuf;
	}
