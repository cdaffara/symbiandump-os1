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

#ifndef __TE_SMSEMSPRT_H_
#define __TE_SMSEMSPRT_H_

#include "TE_SMSEMSPRTBASE.h"

//
// Test EMS FormatA test object - basic Format IE test
//
class CTestEmsFormatA : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS FormatB test object - 8-bit 200 char (approx) text with format
// object in the middle
//
class CTestEmsFormatB : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS FormatC test object - 7-bit 200 char (approx) text with format
// object in the middle
//
class CTestEmsFormatC : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS FormatD test object - 16-bit 100 char (approx) text with format
// object in the middle
//
class CTestEmsFormatD : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS FormatE test object - out of range FormatIE object will not
// get sent
//
class CTestEmsFormatE : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS FormatF test object - check that format object gets repositioned
// correctly
//
class CTestEmsFormatF : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS SoundA - predefined sounds
//
class CTestEmsSoundA : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS SoundB - user-defined sounds
//
class CTestEmsSoundB : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS SoundC - userdefined sounds over 5 PDUs
//
class CTestEmsSoundC : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS SoundD - sending sound over 128 bytes
//
class CTestEmsSoundD : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS Picture - sending small, large, and variable pictures
//
class CTestEmsPicture : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS Animations - sending predefined, large, and small animations
//
class CTestEmsAnimation : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS UserPromptA - basic scenario
//
class CTestEmsUserPromptA : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS UserPromptB - two user prompts at position 20 and 21, referencing
// two melodies at 20 and 21.
//
class CTestEmsUserPromptB : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS LongMessageA, B, and C. These tests create all the same
// text and EMS object types except using different character sets.
// Test A is 8-bit text, test B is 7-bit, test C is UCS2.
//
// Text is 104 chars long, EMS objects are: text formatting beginning at
// character 30, length 70, User defined melody 128 bytes long
// beginning at character 40, 32x32 bitmap beginning character 50.
//
// TestEmsLongMsgBase provides the guts for all three of these tests
//
class CTestEmsLongMsgBase : public CSmsEmsPrtTestStep
	{
	protected:
		void StepWithCharSetL(TSmsDataCodingScheme::TSmsAlphabet aAlphabet);
	};

class CTestEmsLongMessageA : public CTestEmsLongMsgBase
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestEmsLongMessageB : public CTestEmsLongMsgBase
	{
public:
	virtual TVerdict doTestStepL();
	};


class CTestEmsLongMessageC : public CTestEmsLongMsgBase
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS Unsupported IE - Send a message with an unsupported IE.
// Verify it doesn't crash.
//
class CTestEmsUnsupportedIE : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS Unsorted  - Sends a message and receives the same message except
// with the information elements encoded in an unsorted order. The messages
// should still compare OK.
//
class CTestEmsUnsorted : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS TestEmsObjectDistributionA - basic scenario
//
class CTestEmsODIA : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS TestEmsODIB - two odis at position 20 and 21, referencing
// two melodies at 20 and 21.
//
class CTestEmsODIB : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS TestEmsODIC - two odis at position 20 and 21, referencing
// two melodies at 20 and 21.
//
class CTestEmsODIC : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS TestEmsObjectDistributionD - test zero object count applies to all IEs
//
class CTestEmsODID : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS TestEmsObjectDistributionE - apply two odi IEs over more than 1 PDUs
//
class CTestEmsODIE : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


//
// Test EMS TestEmsObjectDistributionF - apply odi and user prompt in wrong order
//
class CTestEmsODIF : public CSmsEmsPrtTestStep
	{
public:
	virtual TVerdict doTestStepL();
	};


#endif
