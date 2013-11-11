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
// Dial API Test main test code.
// 
//



/**
 @file
*/
#include "DIALUTIL.H"

#include "TE_DialBase.h"
#include "TE_DialTest.h"


//
// Constants...
//
_LIT(KUkIntlPrefCode, "00");
_LIT(KUkNatPrefCode, "0");
_LIT(KUkNatCode, "44");
_LIT(KUkLondonAreaCode, "20");
_LIT(KUkBirminghamAreaCode, "121");


/**
 *  Each test step initialises it's own name
 */
CDialTestTest1::CDialTestTest1()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("Test1"));
	} // CDialTestTest1::CDialTestTest1


/**
 *  This test performs Test 1 of the Dial API.
 *
 *  @return Test verdict for this test.
 */	
enum TVerdict CDialTestTest1::doTestStepL()
	{
	INFO_PRINTF1(_L("At home calling Joan"));

	TChargeCard  chargeCard;
	TDialLocation  dialLocation;

	dialLocation.SetIntlPrefCode(KUkIntlPrefCode);
	dialLocation.SetNatPrefCode(KUkNatPrefCode);
	dialLocation.SetNatCode(KUkNatCode);
	dialLocation.SetAreaCode(KUkLondonAreaCode);

    TDialString  dialString;
	TPhoneNumber  numberToDial = _L("+44 (163) 296 0000"); // call Birmingham UK

	TInt ret = TelephoneNumber::Parse(dialString, dialLocation,
									  chargeCard, numberToDial, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString1, "T01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString1);

	// next test
	INFO_PRINTF1(_L("Office-Long distance"));

	dialLocation.SetDialOutLongDistanceCode(_L("9"));
	ret = TelephoneNumber::Parse(dialString, dialLocation,
								 chargeCard, numberToDial, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString2, "T9,01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString2);
	
	//next
	INFO_PRINTF1(_L("Office-local"));
	dialLocation.SetAreaCode(KUkBirminghamAreaCode);
	dialLocation.SetDialOutLocalCode(_L("8"));

	ret = TelephoneNumber::Parse(dialString, dialLocation,
								 chargeCard, numberToDial, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString3, "T9,01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString3);

	//next
	INFO_PRINTF1(_L("Office-Long"));
	dialLocation.SetAreaCode(KUkLondonAreaCode);
	dialLocation.SetDialLocationFlags(KPhoneNumberPbxUsePulseDial);

	ret = TelephoneNumber::Parse(dialString, dialLocation,
								 chargeCard, numberToDial, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString4, "P9,T01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString4);

	return TestStepResult();
	} // CDialTestTest1::CDialTestTest1


/**
 *  Each test step initialises it's own name
 */
CDialTestTest2::CDialTestTest2()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("Test2"));
	} // CDialTestTest2::CDialTestTest2


/**
 *  This test performs Test 2 of the Dial API.
 *
 *  @return Test verdict for this test.
 */	
enum TVerdict CDialTestTest2::doTestStepL()
	{
	INFO_PRINTF1(_L("Using mobile phone"));

	TChargeCard  chargeCard;
	TDialLocation  dialLocation;

	dialLocation.SetIntlPrefCode(KUkIntlPrefCode);
	dialLocation.SetNatPrefCode(KUkNatPrefCode);
	dialLocation.SetNatCode(KUkNatCode);
	dialLocation.SetAreaCode(KUkLondonAreaCode);

	TDialString  dialString;
	TPhoneNumber  numberToDialInUK  = _L("+44 (163) 296 0000");
	TPhoneNumber  numberToDialInUSA = _L("+1 (163) 296 0000");
	TPhoneNumber  numberToDialInLondon = _L("+44 (16) 3296 0000");

	INFO_PRINTF1(_L("Mobile - Long distance"));
	dialLocation.SetDialLocationFlags(KPhoneNumberMobile);
	
	TInt ret = TelephoneNumber::Parse(dialString, dialLocation, chargeCard,
									  numberToDialInUK, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString1, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString1);

	// for release 012
	dialLocation.SetDialLocationFlags(KReturnPhoneNumberWithPlusFormat);
	ret = TelephoneNumber::Parse(dialString, dialLocation, chargeCard,
								 numberToDialInUK, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString2, "+441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString2);
	dialLocation.ClearDialLocationFlags(KReturnPhoneNumberWithPlusFormat);

	INFO_PRINTF1(_L("Mobile - Intl call"));
	ret = TelephoneNumber::Parse(dialString, dialLocation,
								 chargeCard, numberToDialInUSA, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString3, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString3);

	// for release 012
	dialLocation.SetDialLocationFlags(KReturnPhoneNumberWithPlusFormat);
	ret = TelephoneNumber::Parse(dialString, dialLocation,
								 chargeCard, numberToDialInUSA, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString4, "+11632960000");
	TESTCHECKSTR(dialString, KExpectedDialString4);
	dialLocation.ClearDialLocationFlags(KReturnPhoneNumberWithPlusFormat);

	// this should be the same as long distance call
	// no concept of a local call using mobile phone !
	INFO_PRINTF1(_L("Mobile - Call in same area"));
	ret = TelephoneNumber::Parse(dialString,dialLocation, chargeCard,
								 numberToDialInLondon, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString5, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString5);

	// for release 012
	dialLocation.SetDialLocationFlags(KReturnPhoneNumberWithPlusFormat);
	ret = TelephoneNumber::Parse(dialString, dialLocation, chargeCard,
								 numberToDialInLondon, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString6, "+441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString6);
	dialLocation.ClearDialLocationFlags(KReturnPhoneNumberWithPlusFormat);

	return TestStepResult();
	} // CDialTestTest2::CDialTestTest2


/**
 *  Each test step initialises it's own name
 */
CDialTestTest3::CDialTestTest3()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("Test3"));
	} // CDialTestTest3::CDialTestTest3


/**
 *  This test performs Test 3 of the Dial API.
 *
 *  @return Test verdict for this test.
 */	
enum TVerdict CDialTestTest3::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Calling card"));

	// This is a BT charge card
	TChargeCard  chargeCard;
	TDialLocation  dialLocation;

	dialLocation.SetIntlPrefCode(KUkIntlPrefCode);
	dialLocation.SetNatPrefCode(KUkNatPrefCode);
	dialLocation.SetNatCode(KUkNatCode);
	dialLocation.SetAreaCode(KUkLondonAreaCode);

    TDialString  dialString;

	chargeCard.SetNumber(_L("123456"));			//iNumber
	chargeCard.SetPin(_L("3681"));				//iPin
	chargeCard.SetLocalRule(_L("144,,H,0FG"));	//iLocalRule
	chargeCard.SetNatRule(_L("144,,H,0FG"));	//iLongDistanceRule
	chargeCard.SetIntlRule(_L("144,,H,00EFG"));	//iIntlRule

	TPhoneNumber  numberInLondon = _L("+44 (16) 3296 0000");
	TInt ret = TelephoneNumber::Parse(dialString, dialLocation, chargeCard,
									  numberInLondon, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString1, "T144,,1234563681,01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString1);

	INFO_PRINTF1(_L("Call inter. with charge card"));

	TPhoneNumber  numberToDialInUSA = _L("+1 (163) 296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, chargeCard,
								 numberToDialInUSA, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString2, "T144,,1234563681,0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString2);

	return TestStepResult();
	} // CDialTestTest3::CDialTestTest3


/**
 *  Each test step initialises it's own name
 */
CDialTestTest4::CDialTestTest4()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("Test4"));
	} // CDialTestTest4::CDialTestTest4


/**
 *  This test performs Test 4 of the Dial API.
 *
 *  @return Test verdict for this test.
 */	
enum TVerdict CDialTestTest4::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Calling with Letter dialing"));

#if 0
	// This is a BT charge card
	TChargeCard  chargeCard;
    TDialLocation  dialLocation;

	dialLocation.SetIntlPrefCode(KUkIntlPrefCode);
	dialLocation.SetNatPrefCode(KUkNatPrefCode);
	dialLocation.SetNatCode(KUkNatCode);
	dialLocation.SetAreaCode(KUkLondonAreaCode);

    TDialString  dialString;

	chargeCard.SetNumber(_L("112233"));			//iNumber
	chargeCard.SetPin(_L("3681"));				//iPin
	chargeCard.SetLocalRule(_L("144,,H,0FG"));	//iLocalRule
	chargeCard.SetNatRule(_L("144,,H,0FG"));	//iLongDistanceRule
	chargeCard.SetIntlRule(_L("144,,H,00EFG"));	//iIntlRule

	TPhoneNumber telNumber = _L("+44 (20) 7PSION3A");
	TInt ret = TelephoneNumber::Parse(dialString, dialLocation,
									  chargeCard, telNumber, EForDialing);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString1, "T144,,1122333681,01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString1);


	INFO_PRINTF1(_L("Call inter. with charge card"));
	telNumber = _L("+1 (510) HAVE FUN");
	ret = TelephoneNumber::Parse(dialString, dialLocation,
								 chargeCard, telNumber, EForDialing);
	TESTCHECK(ret, KErrNone)
	_LIT(KExpectedDialString2, "T144,,1122333681,01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString2);

	_LIT(KExpectedTelNumber, "+1 (510) HAVE FUN");
	TESTCHECKSTR(telNumber, KExpectedTelNumber);
#else
	INFO_PRINTF1(_L("This test has been deactivated"));
#endif

	return TestStepResult();
	} // CDialTestTest4::CDialTestTest4


/**
 *  Each test step initialises it's own name
 */
CDialTestTest5::CDialTestTest5()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("Test5"));
	} // CDialTestTest5::CDialTestTest5


/**
 *  This test performs Test 5 of the Dial API.
 *
 *  @return Test verdict for this test.
 */	
enum TVerdict CDialTestTest5::doTestStepL()
	{
	TPhoneNumber  numberList[2] = {_L("+44(163)2960000"), _L("+44 (163) 2960000")};
	TInt  index;
	
	for (index = 0;  index < 2;  index++)
		{
		INFO_PRINTF1(_L("At home calling Joan"));

		TChargeCard  chargeCard;
		TDialLocation  dialLocation;

		dialLocation.SetIntlPrefCode(KUkIntlPrefCode);
		dialLocation.SetNatPrefCode(KUkNatPrefCode);
		dialLocation.SetNatCode(KUkNatCode);
		dialLocation.SetAreaCode(KUkLondonAreaCode);

		TDialString  dialString ;
		TPhoneNumber  numberToDial = numberList[index];

		TInt ret = TelephoneNumber::Parse(dialString, dialLocation,
										  chargeCard, numberToDial, EForDialing);
		TESTCHECK(ret, KErrNone);
		_LIT(KExpectedDialString1, "T01632960000");
		TESTCHECKSTR(dialString, KExpectedDialString1);

		// next test
		INFO_PRINTF1(_L("Office-Long distance"));
		dialLocation.SetDialOutLongDistanceCode(_L("9"));
		ret = TelephoneNumber::Parse(dialString, dialLocation,
									 chargeCard, numberToDial, EForDialing);
		TESTCHECK(ret, KErrNone);
		_LIT(KExpectedDialString2, "T9,01632960000");
		TESTCHECKSTR(dialString,KExpectedDialString2);

		//next
		INFO_PRINTF1(_L("Office-local"));
		dialLocation.SetAreaCode(KUkBirminghamAreaCode);
		dialLocation.SetDialOutLocalCode(_L("8"));
		ret = TelephoneNumber::Parse(dialString, dialLocation,
									 chargeCard, numberToDial, EForDialing);
		TESTCHECK(ret, KErrNone)
		_LIT(KExpectedDialString3, "T9,01632960000");
		TESTCHECKSTR(dialString, KExpectedDialString3);
		
		//next
		INFO_PRINTF1(_L("Office-Long"));
		dialLocation.SetAreaCode(KUkLondonAreaCode);
		dialLocation.SetDialLocationFlags(KPhoneNumberPbxUsePulseDial);
		ret = TelephoneNumber::Parse(dialString, dialLocation,
									 chargeCard, numberToDial, EForDialing);
		TESTCHECK(ret, KErrNone);
		_LIT(KExpectedDialString4, "P9,T01632960000");
		TESTCHECKSTR(dialString, KExpectedDialString4);
		}

	return TestStepResult();
	} // CDialTestTest5::CDialTestTest5


/**
 *  Each test step initialises it's own name
 */
CDialTestTest6::CDialTestTest6()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("Test6"));
	} // CDialTestTest6::CDialTestTest6


/**
 *  This test performs Test 6 of the Dial API.
 *
 *  @return Test verdict for this test.
 */	
enum TVerdict CDialTestTest6::doTestStepL()
	{
	INFO_PRINTF1(_L("Dialing for Time World Phone resolution"));

	TDialString  dialString;
	TChar  codeSeparator = KCharSpace;

	TCityDialCodes  londonUK; // London
	londonUK.iAreaCode = KUkLondonAreaCode;
	londonUK.iNatCode  = KUkNatCode;
	londonUK.iNatPref  = KUkNatPrefCode;
	londonUK.iIntlPref = KUkIntlPrefCode;

	TCityDialCodes  birmingham ; // Birmingham
	birmingham.iAreaCode = KUkBirminghamAreaCode;
	birmingham.iNatCode  = KUkNatCode;
	birmingham.iNatPref  = KUkNatPrefCode;
	birmingham.iIntlPref = KUkIntlPrefCode;

	TCityDialCodes  sanFr ; // San Francisco 
	sanFr.iAreaCode = _L("415");
	sanFr.iNatCode  = _L("1");
	sanFr.iNatPref  = _L("1");
	sanFr.iIntlPref = _L("011");

	TCityDialCodes  washingtonDC ;  
	washingtonDC.iAreaCode = _L("202");
	washingtonDC.iNatCode  = _L("1");
	washingtonDC.iNatPref  = _L("1");
	washingtonDC.iIntlPref = _L("011");

	TCityDialCodes  kingstonJam ;  
	kingstonJam.iAreaCode = _L("80992");
	kingstonJam.iNatCode  = _L("1");
	kingstonJam.iNatPref  = _L("1");
	kingstonJam.iIntlPref = _L("011");

	//
	// International dialing...
	//
	INFO_PRINTF1(_L("International dialing"));
	TInt ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, londonUK, sanFr,
						EDialCityStripDuplicateAreaCode, codeSeparator ,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString1, "00 1 415");
	TESTCHECKSTR(dialString, KExpectedDialString1);

	// no diff
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, londonUK, sanFr,
						EDialCityAllowDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString2, "00 1 415");
	TESTCHECKSTR(dialString, KExpectedDialString2);

	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, sanFr, londonUK ,
						EDialCityStripDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString3, "011 44 20");
	TESTCHECKSTR(dialString, KExpectedDialString3);

	// no diff
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, sanFr , londonUK,
						EDialCityAllowDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString4, "011 44 20");
	TESTCHECKSTR(dialString, KExpectedDialString4);

	//
	// National dialing...
	//
	INFO_PRINTF1(_L("National dialing"));
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, londonUK, birmingham,
						EDialCityStripDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString5, "0 121");
	TESTCHECKSTR(dialString, KExpectedDialString5);

	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, londonUK, birmingham,
						EDialCityAllowDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString6, "0 121");
	TESTCHECKSTR(dialString, KExpectedDialString6);

	//
	// Same area code dialing...
	//
	INFO_PRINTF1(_L("Same area dialling"));
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, birmingham, birmingham,
						EDialCityStripDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString7, "");
	TESTCHECKSTR(dialString, KExpectedDialString7);

	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, birmingham, birmingham,
						EDialCityAllowDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString8, "0 121");
	TESTCHECKSTR(dialString, KExpectedDialString8);

	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, sanFr, sanFr,
						EDialCityStripDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString9, "");
	TESTCHECKSTR(dialString, KExpectedDialString9);
	
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, sanFr, sanFr ,
						EDialCityAllowDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString10, "1 415");
	TESTCHECKSTR(dialString, KExpectedDialString10);

	//
	// Test Code separator...
	//
	INFO_PRINTF1(_L("Test Code separator"));
	codeSeparator = KCharStar;
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, londonUK, sanFr,
						EDialCityStripDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString11, "00*1*415");
	TESTCHECKSTR(dialString, KExpectedDialString11);

	codeSeparator = KCharUnderscore;
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, londonUK, birmingham,
						EDialCityAllowDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString12, "0_121");
	TESTCHECKSTR(dialString, KExpectedDialString12);

	codeSeparator = KCharMinus;
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, sanFr, sanFr ,
						EDialCityAllowDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString13, "1-415");
	TESTCHECKSTR(dialString, KExpectedDialString13);

	//
	// Kingston Jamaica - Washington DC...
	//
	INFO_PRINTF1(_L("From Kingston Jamaica to Washington DC"));
	codeSeparator = KCharSpace;
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, kingstonJam , washingtonDC ,
						EDialCityStripDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString14, "1 202");
	TESTCHECKSTR(dialString, KExpectedDialString14);

	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, kingstonJam , washingtonDC ,
						EDialCityAllowDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString15, "1 202");
	TESTCHECKSTR(dialString, KExpectedDialString15);

	INFO_PRINTF1(_L("From Washington DC to Kingston Jamaica"));
	codeSeparator = KCharSpace;
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, washingtonDC , kingstonJam ,
						EDialCityStripDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString16, "1 80992");
	TESTCHECKSTR(dialString, KExpectedDialString16);

	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString,  washingtonDC ,kingstonJam ,
						EDialCityAllowDuplicateAreaCode, codeSeparator,
						EPlusFormatDialNotAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString17, "1 80992");
	TESTCHECKSTR(dialString, KExpectedDialString17);

	//
	// Dialling String in Plus(+) format...
	//
	INFO_PRINTF1(_L("Dialling String in Plus(+) format"));
	codeSeparator = KCharSpace;
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, londonUK  , washingtonDC ,
						EDialCityStripDuplicateAreaCode, codeSeparator,
						EPlusFormatDialAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString18, "+1 202");
	TESTCHECKSTR(dialString, KExpectedDialString18);

	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, washingtonDC , londonUK,
						EDialCityAllowDuplicateAreaCode, codeSeparator,
						EPlusFormatDialAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString19, "+44 20");
	TESTCHECKSTR(dialString, KExpectedDialString19);

	codeSeparator = KCharSpace;
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, washingtonDC , kingstonJam ,
						EDialCityStripDuplicateAreaCode, codeSeparator,
						EPlusFormatDialAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString20, "1 80992");
	TESTCHECKSTR(dialString, KExpectedDialString20);

	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, kingstonJam , birmingham,
						EDialCityAllowDuplicateAreaCode, codeSeparator,
						EPlusFormatDialAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString21, "+44 121");
	TESTCHECKSTR(dialString, KExpectedDialString21);

	codeSeparator = KCharUnderscore;
	ret = TelephoneNumber::CalculateDialCityPrefix(	
						dialString, londonUK  , washingtonDC ,
						EDialCityStripDuplicateAreaCode, codeSeparator,
						EPlusFormatDialAllow);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString22, "+1_202");
	TESTCHECKSTR(dialString, KExpectedDialString22);

	return TestStepResult();
	} // CDialTestTest6::CDialTestTest6


/**
 *  Each test step initialises it's own name
 */
CDialTestTestDtmf2::CDialTestTestDtmf2()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("TestDtmf2"));
	} // CDialTestTestDtmf2::CDialTestTestDtmf2


/**
 *  This test performs DTMF Test 2 of the Dial API.
 *
 *  @return Test verdict for this test.
 */	
enum TVerdict CDialTestTestDtmf2::doTestStepL()
	{
	INFO_PRINTF1(_L("DTMF dialling test for space separator format!!!"));

	INFO_PRINTF1(_L("Call Birmingham UK"));
	TDialLocation  dialLocation;

	dialLocation.SetIntlPrefCode(KUkIntlPrefCode);
	dialLocation.SetNatPrefCode(KUkNatPrefCode);
	dialLocation.SetNatCode(KUkNatCode);
	dialLocation.SetAreaCode(KUkLondonAreaCode);

    TDialString  dialString ;
	TPhoneNumber  numberToDial = _L("+44 163 2960000");

	TInt ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);

	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString1, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString1);

	numberToDial = _L(" +44 163 2960000"); // does not effect the dial string
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString2, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString2);

	numberToDial = _L("   +44   163   296    0000"); // does not effect the dial string
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString3, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString3);

	//	
	INFO_PRINTF1(_L("Number to dial in London inner - UK"));
	numberToDial = _L("   +44  2016  3296 0000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString4, "0201632960000");
	TESTCHECKSTR(dialString, KExpectedDialString4);

	numberToDial = _L("   +44 16 32960000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString5, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString5);
	
	numberToDial = _L("   +44 16     3296    0000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString6, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString6);

	//	
	INFO_PRINTF1(_L("Number to dial in USA"));
	numberToDial = _L("+1  163   296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString7, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString7); 

	numberToDial = _L("+1    163    32960000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString8, "00116332960000");
	TESTCHECKSTR(dialString, KExpectedDialString8); 
	
	numberToDial = _L("+1   163          329 60000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString9, "00116332960000");
	TESTCHECKSTR(dialString, KExpectedDialString9); 
	
	//	
	numberToDial = _L("+1 163  32960000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString10, "00116332960000");
	TESTCHECKSTR(dialString, KExpectedDialString10); 

	//
	// set home location in San Francisco - USA and dial back to UK
	//
	dialLocation.SetNatCode(_L("1"));
	dialLocation.SetNatPrefCode(_L("1"));
	dialLocation.SetIntlPrefCode(_L("011"));
	dialLocation.SetAreaCode(_L("415"));

	numberToDial = _L("   +44  16  3296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString11, "011441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString11); 

	numberToDial = _L("   +44 16 32960000   "); // empty spaces at the end to see it can cope ?
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString12, "011441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString12); 
	
	// call Los Angeles
	numberToDial = _L("+1  163  3296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString13, "116332960000");
	TESTCHECKSTR(dialString, KExpectedDialString13); 

	numberToDial = _L("+1 163 2960000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString14, "11632960000");
	TESTCHECKSTR(dialString, KExpectedDialString14); 
	
	// French dialling stuff !
	TDialLocation ukLocation ;
	ukLocation.SetIntlPrefCode(KUkIntlPrefCode);
	ukLocation.SetNatPrefCode(KUkNatPrefCode);
	ukLocation.SetNatCode(KUkNatCode);
	ukLocation.SetAreaCode(KUkLondonAreaCode);

	INFO_PRINTF1(_L("Calling from London to somewhere in France(not Paris)"));
	numberToDial = _L("+16 32960000"); 
	ret = TelephoneNumber::Parse(dialString, ukLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString15, "001632960000");
	TESTCHECKSTR(dialString, KExpectedDialString15);

	INFO_PRINTF1(_L("Calling from London to somewhere in Paris"));
	numberToDial = _L("+01 6 32960000"); 
	ret = TelephoneNumber::Parse(dialString, ukLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString16, "0001632960000");
	TESTCHECKSTR(dialString, KExpectedDialString16);

	INFO_PRINTF1(_L("Calling from London to somewhere in Paris"));
	numberToDial = _L("+01 6 3 2 9 6 0 0 0 0"); 
	ret = TelephoneNumber::Parse(dialString, ukLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString17, "0001632960000");
	TESTCHECKSTR(dialString, KExpectedDialString17);

	return TestStepResult();
	} // CDialTestTestDtmf2::CDialTestTestDtmf2


/**
 *  Each test step initialises it's own name
 */
CDialTestTest7::CDialTestTest7()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("Test7"));
	} // CDialTestTest7::CDialTestTest7


/**
 *  This test performs Test 7 of the Dial API.
 *
 *  @return Test verdict for this test.
 */	
enum TVerdict CDialTestTest7::doTestStepL()
	{
	INFO_PRINTF1(_L("DTMF dialling"));

	INFO_PRINTF1(_L("Call Birmingham UK"));
	
	TDialLocation  dialLocation;

	dialLocation.SetIntlPrefCode(KUkIntlPrefCode);
	dialLocation.SetNatPrefCode(KUkNatPrefCode);
	dialLocation.SetNatCode(KUkNatCode);
	dialLocation.SetAreaCode(KUkLondonAreaCode);

    TDialString  dialString;

	TPhoneNumber numberToDial = _L("+44 (163) 296 0000"); 

	TInt ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString1, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString1);

	numberToDial = _L("+44(163)2960000"); // does not effect the dial string
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString2, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString2);

	numberToDial = _L("+44  (163)  296    0000"); // does not effect the dial string
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString3, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString3);

	//
	INFO_PRINTF1(_L("Number to dial in London inner - UK"));
	numberToDial = _L("+44 (16) 3296 0000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString4, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString4);
	
	numberToDial = _L("+44(16)32960000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString5, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString5);
	
	numberToDial = _L("+44(16)    3296    0000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString6, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString6);
	
	//	
	INFO_PRINTF1(_L("Number to dial in USA"));
	numberToDial = _L("+1 (163) 296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString7, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString7); 
	
	numberToDial = _L("+1   (163)  2960000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString8, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString8); 
	
	numberToDial = _L("+1  (163)        296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString9, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString9); 
	
	numberToDial = _L("+1(163)2960000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString10, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString10); 

	//
	// Set home location in San Francisco - USA and dial back to UK...
	//
	dialLocation.SetNatCode(_L("1"));
	dialLocation.SetNatPrefCode(_L("1"));
	dialLocation.SetIntlPrefCode(_L("011"));
	dialLocation.SetAreaCode(_L("415"));

	numberToDial = _L("+44 (16) 3296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString11, "011441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString11); 
	
	numberToDial = _L("+44(16)32960000   "); // empty spaces at the end to see it can cope ?
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString12, "011441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString12); 
	
	// call Los Angeles
	numberToDial = _L("+1 (163) 296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString13, "11632960000");
	TESTCHECKSTR(dialString, KExpectedDialString13); 

	numberToDial = _L("+1(163) 296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString14, "11632960000");
	TESTCHECKSTR(dialString, KExpectedDialString14); 
	
	//
	// Testing Parser with option to strip/allow areacode...
	//
	INFO_PRINTF1(_L("Call Birmingham UK"));
	dialLocation.SetIntlPrefCode(KUkIntlPrefCode);
	dialLocation.SetNatPrefCode(KUkNatPrefCode);
	dialLocation.SetNatCode(KUkNatCode);
	dialLocation.SetAreaCode(KUkLondonAreaCode);

	numberToDial = _L("+44 (163) 296 0000"); 

	//
	// 1) Strip duplicate area code should not make any difference to dial string
	//
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString15, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString15);

	//
	// 2) Allow duplicate area code should not make any difference to dial string
	//
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString16, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString16);

	//
	// 3) Strip duplicate area code should not make any difference to dial string
	//
	numberToDial = _L("+44(163)2960000"); // does not effect the dial string
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString17, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString17);

	//
	// 4) Allow duplicate area code should not make any difference to dial string
	//
	numberToDial = _L("+44  (163)  296    0000"); // does not effect the dial string
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString18, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString18);

	//
	// 5) Strip areacode 
	//
	INFO_PRINTF1(_L("Number to dial in London inner - UK"));
	numberToDial = _L("+44 (16) 3296 0000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString19, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString19);

	//
	// 6) Allow areacode 
	//
	INFO_PRINTF1(_L("Number to dial in London inner - UK"));
	numberToDial = _L("+44 (16) 3296 0000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString20, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString20);

	//
	// 7) Strip areacode 
	//
	numberToDial = _L("+44(16)32960000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString21, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString21);

	//
	// 8) Allow areacode 
	//
	numberToDial = _L("+44(16)    3296    0000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString22, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString22);
	
	//
	// 9a) Allow duplicate area code should not make any difference to dial string
	// in international call 	
	//
	INFO_PRINTF1(_L("Number to dial in USA"));
	numberToDial = _L("+1 (163) 296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString23, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString23); 

	//
	// 9b) Strip duplicate area code should not make any difference to dial string
	// in international call 	
	//
	numberToDial = _L("+1 (163) 296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString24, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString24); 

	return TestStepResult();
	} // CDialTestTest7::CDialTestTest7


/**
 *  Each test step initialises it's own name
 */
CDialTestTest8::CDialTestTest8()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("Test8"));
	} // CDialTestTest8::CDialTestTest8


/**
 *  This test performs Test 8 of the Dial API.
 *
 *  @return Test verdict for this test.
 */	
enum TVerdict CDialTestTest8::doTestStepL()
	{
	INFO_PRINTF1(_L("DTMF dialling "));

	INFO_PRINTF1(_L("Call Birmingham UK"));

	TDialLocation  dialLocation;

	dialLocation.SetIntlPrefCode(KUkIntlPrefCode);
	dialLocation.SetNatPrefCode(KUkNatPrefCode);
	dialLocation.SetNatCode(KUkNatCode);
	dialLocation.SetAreaCode(KUkLondonAreaCode);

    TDialString  dialString ;
	TPhoneNumber  numberToDial = _L("+1   (163)  2960000");

	//
	// 10) Strip duplicate area code should not make any difference to dial string
	// in international call 	
	//
	INFO_PRINTF1(_L("Number to dial in USA (Multiple tests)"));
	TInt  ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
									   EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString25, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString25); 

	//
	// 11) Allow duplicate area code should not make any difference to dial string
	// in international call 	
	//
	numberToDial = _L("+1   (163)  2960000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString26, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString26); 

	//
	// 12) Allow duplicate area code should not make any difference to dial string
	// in international call 	
	//
	numberToDial = _L("+1   (163)  2960000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString27, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString27); 	

	//
	// 12) Strip duplicate area code should not make any difference to dial string
	// in international call 	
	//
	numberToDial = _L("+1  (163)        296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString28, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString28); 
	
	//
	// 13) Strip duplicate area code should not make any difference to dial string
	// in international call 	
	//
	numberToDial = _L("+1(163)2960000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString29, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString29); 

	//
	// 14) Allow duplicate area code should not make any difference to dial string
	// in international call 	
	//
	numberToDial = _L("+1(163)2960000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString30, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString30); 

	//
	// set home location in San Francisco - USA and dial back to UK
	//
	dialLocation.SetNatCode(_L("1"));
	dialLocation.SetNatPrefCode(_L("1"));
	dialLocation.SetIntlPrefCode(_L("011"));
	dialLocation.SetAreaCode(_L("415"));

	//
	numberToDial = _L("+44 (16) 3296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString31, "011441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString31); 
	
	//
	numberToDial = _L("+44 (16) 3296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString32, "011441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString32); 
	
	//
	numberToDial = _L("+44(16)32960000   "); // empty spaces at the end to see it can cope ?
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString33, "011441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString33); 
	
	//
	numberToDial = _L("+44(16)32960000   "); // empty spaces at the end to see it can cope ?
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString34, "011441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString34); 

	//
	numberToDial = _L("+44(16)32960000   "); // empty spaces at the end to see it can cope ?
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString35, "011441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString35); 

	// call Los Angeles
	numberToDial = _L("+1 (163) 296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString36, "11632960000");
	TESTCHECKSTR(dialString, KExpectedDialString36); 
	
	// call Los Angeles
	numberToDial = _L("+1 (163) 296 0000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString37, "11632960000");
	TESTCHECKSTR(dialString, KExpectedDialString37); 

	//
	numberToDial = _L("+1(163)2960000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString38, "11632960000");
	TESTCHECKSTR(dialString, KExpectedDialString38); 

	//
	numberToDial = _L("+1(163)2960000");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString39, "11632960000");
	TESTCHECKSTR(dialString, KExpectedDialString39); 

	//
	numberToDial = _L("+11 6 32 96 00 00 (Symbian's Phone)");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString40, "01111632960000");
	TESTCHECKSTR(dialString, KExpectedDialString40); 	
	
	numberToDial = _L("+11 6 32 96 00 00 (Symbian's Phone a() )");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString41, "01111632960000");
	TESTCHECKSTR(dialString, KExpectedDialString41); 	

	numberToDial = _L("+11 6 32 96 00 00 (  () Symbian's Phone)");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString42, "01111632960000");
	TESTCHECKSTR(dialString, KExpectedDialString42); 	

	numberToDial = _L("+11 6 32 96 00 00 (  (Symbi(((an's Phone)");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString43, "01111632960000");
	TESTCHECKSTR(dialString, KExpectedDialString43); 	

	numberToDial = _L("+11 6 32 96 00 00 (   Symbian's Pho)))ne)");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString44, "01111632960000");
	TESTCHECKSTR(dialString, KExpectedDialString44); 	
	
	// 	
	dialLocation.SetIntlPrefCode(KUkIntlPrefCode);
	dialLocation.SetNatPrefCode(KUkNatPrefCode);
	dialLocation.SetNatCode(KUkNatCode);
	dialLocation.SetAreaCode(KUkLondonAreaCode);
	
	numberToDial = _L("+44 (16) 3296 0000 (Symbian's Office Phone Number)");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityStripDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString45, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString45); 	
	
	numberToDial = _L("+44 (16) 3296 0000 (Symbian's Office Phone Number)");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString46, "01632960000");
	TESTCHECKSTR(dialString, KExpectedDialString46); 	
	
	numberToDial = _L("+11 6 32 96 00 00 (");
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial,
								 EDialCityAllowDuplicateAreaCode);
	TESTCHECK(ret, KErrNone); 
	_LIT(KExpectedDialString47, "0011632960000");
	TESTCHECKSTR(dialString, KExpectedDialString47); 	

	return TestStepResult();
	} // CDialTestTest8::CDialTestTest8


/**
 *  Each test step initialises it's own name
 */
CDialTestIllegalTest::CDialTestIllegalTest()
	{
	//
	// Store the name of this test case.  This is the name that is used by
	// the script file.
	//
	SetTestStepName(_L("IllegalTest"));
	} // CDialTestIllegalTest::CDialTestIllegalTest


/**
 *  This test performs an illegal test of the Dial API.
 *
 *  @return Test verdict for this test.
 */	
enum TVerdict CDialTestIllegalTest::doTestStepL()
	{
	INFO_PRINTF1(_L("Illegalformat!!!"));

	//
	INFO_PRINTF1(_L("Phone with KCharOpenBracket but missing KCharcloseBracket char"));
	TDialLocation dialLocation; // default at London Inner
	TDialString dialString;
	TPhoneNumber numberToDial = _L("+44(163 2960000"); 
	TInt ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
#ifdef ORIGINAL_TEST_CODE
	TESTCHECK(ret, KErrNotSupported);
#else
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString1, "441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString1); 	
#endif

	//
	INFO_PRINTF1(_L("Phone Number with KCharcloseBracket but not KCharOpenBracket "));
	numberToDial = _L("   +44  16)  3296 0000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
#ifdef ORIGINAL_TEST_CODE
	TESTCHECK(ret, KErrNotSupported);
#else
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString2, "441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString2); 	
#endif

	//
    INFO_PRINTF1(_L("a funny Phone Number "));
	numberToDial = _L("+44)16(32960000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	_LIT(KExpectedDialString3, "441632960000");
	TESTCHECKSTR(dialString, KExpectedDialString3); 	

	//
	// Yes supported but take 56 as area code and 78 as 
	// the remain of the number
	//
	INFO_PRINTF1(_L("Phone Number wrong format"));
	numberToDial = _L("+33  1234(56)00"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);

	INFO_PRINTF1(_L("Phone Number wrong format"));
	numberToDial = _L(" 33  1234(56)78"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	
	// for spaces
	INFO_PRINTF1(_L("Phone Number wrong format"));
	numberToDial = _L(" 33  12345678"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	
	INFO_PRINTF1(_L("Phone Number wrong format"));
	numberToDial = _L("+441632960000"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	
	INFO_PRINTF1(_L("Phone Number wrong format"));
	numberToDial = _L("+441632960000 1 "); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	
	INFO_PRINTF1(_L("Phone Number wrong format"));
	numberToDial = _L("+123441156789  1234111115678 1234566789"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);

	// for bracket
	INFO_PRINTF1(_L("Phone Number wrong format"));
	numberToDial = _L(" 33  (12345678)"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	
	INFO_PRINTF1(_L("Phone Number wrong format"));
	numberToDial = _L("+441632960000()"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	
	INFO_PRINTF1(_L("Phone Number wrong format"));
	numberToDial = _L("+441632960000 (1) "); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);
	
	INFO_PRINTF1(_L("Phone Number wrong format"));
	numberToDial = _L("+123441156789  (1234111115678) 1234566789"); 
	ret = TelephoneNumber::Parse(dialString, dialLocation, numberToDial);
	TESTCHECK(ret, KErrNone);

	return TestStepResult();
	} // CDialTestIllegalTest::CDialTestIllegalTest
