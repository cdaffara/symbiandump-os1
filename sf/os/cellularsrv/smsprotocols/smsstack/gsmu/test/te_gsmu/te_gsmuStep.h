/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file
*/
#ifndef __TE_GSMUSTEP_H__
#define __TE_GSMUSTEP_H__

#include "gsmumsg.h"
#include "gsmubuf.h"
#include "gsmuset.h"
// For CEditableTextObject
#include "txtrich.h"
#include "txtfmlyr.h"

#include <test/testexecutestepbase.h>
#include "te_gsmu.h"


//
// Test macros
//
#define TESTCHECKL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(_L("Failed: Got %d while expecting %d."),a,b); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

#define TESTCHECKCONDITIONL(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(_L("Failed: Condition was false.")); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECKSTRL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    TBuf<255> temp = a ; \
	    INFO_PRINTF3(_L("Failed: Got \"%S\" while expecting \"%S\"."), &temp,&(b)); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECK(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(_L("Failed: Got %d while expecting %d."),a,b); \
        SetTestStepResult(EFail); \
        } \
    }

#define TESTCHECKCONDITION(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(_L("Failed: Condition was false.")); \
        SetTestStepResult(EFail); \
        } \
    }
    
#define TESTCHECKSTR(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    TBuf<255> temp = a ; \
	    INFO_PRINTF3(_L("Failed: Got \"%S\" while expecting \"%S\"."), &temp,&(b)); \
        SetTestStepResult(EFail); \
        } \
    }


// __EDIT_ME__ - Create your own test step definitions

class CTgsmuStepBase : public CTestStep
{
public:
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();

protected:
	void DoTestAlphabetConversionL(enum TSmsDataCodingScheme::TSmsAlphabet aAlphabet,const TDesC16** aBufferTable);
	void DoBinaryTestAlphabetConversionL(const TDesC8** aBufferTable);
	void DoConversionSplitTestL();
	void TestSmsHeaderL();
	void TestDecodePDUL();
	void TestCommandDataHeaderL(void);
	void TestSmsProtocolIdentifierL();
	void TestSmsTONandNPIL();
	void TestSmsOptimiseSettingsL();
	void TestSetAndGetSchemeL();
	void TestDefaultSetReportAndGetReportL();
	void TestTPSRRSetAndGetSchemeL();
	void TestTPSRRDefaultSetReportAndGetReportL();
	void TestOnlyOneSchemeExistsL();
	void TestExternaliseAndInternaliseL();
	void TestEncodeAndDecodeL();
	void TestTPSRREncodeAndDecodeL();
	TVerdict TestSmsDefaultVersionL();
	TVerdict TestSmsCurrentVersionL();
	TVerdict TestSmsVersionBackwardsL();
	void TestCSmsEditorBufferL();

	void DoTestConversionL(CSmsAlphabetConverter& aConverter,const TDesC16& aDescriptor);
	void DoBinaryTestConversionL(CSmsAlphabetConverter& aConverter,const TDesC8& aDescriptor);
	void NewSmsMessageL(const TSmsUserDataSettings& aUserDataSettings,TInt aNumBufs);

	RFs iFs;
};


class CTgsmuTestAlphabetConversion7Bit : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestAlphabetConversion8Bit : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestAlphabetConversionUCS2 : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestAlphabetConversionBinary : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestAlphabetConversionSplit : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestSmsHeader : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestDecodePDU : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestCommandDataHeader : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestSmsProtocolIdentifier : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestSmsTONandNPI : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestSmsOptimiseSettings : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestSmsDefaultVersion : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestSmsCurrentVersion : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmuTestSmsVersionBackwards : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmTgsmuTestSMSCCtrlParameterOperations : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


class CTgsmTgsmuTestTPSRROperations : public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};
	
	
// This class tests the functions within the class CSmsEditorBuffer	
class CTgsmuTestCSmsEditorBuffer	: public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL()	;
	};

class CTgsmuTestBCDAddress	: public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL()	;
	};

class CTestAlternative7bitEncodingAPIs	: public CTgsmuStepBase
	{
public:
	TVerdict doTestStepL();
	};


_LIT(KTgsmuTestBCDAddress,"TgsmuTestBCDAddress");
_LIT(KTgsmuTestAlphabetConversion7Bit,"TgsmuTestAlphabetConversion7Bit");
_LIT(KTgsmuTestAlphabetConversion8Bit,"TgsmuTestAlphabetConversion8Bit");
_LIT(KTgsmuTestAlphabetConversionUCS2,"TgsmuTestAlphabetConversionUCS2");
_LIT(KTgsmuTestAlphabetConversionBinary,"TgsmuTestAlphabetConversionUBinary");
_LIT(KTgsmuTestAlphabetConversionSplit,"TgsmuTestAlphabetConversionUSplit");
_LIT(KTgsmuTestSmsHeader,"TgsmuTestSmsHeader");
_LIT(KTgsmuTestDecodePDU,"TgsmuTestDecodePDU");
_LIT(KTgsmuTestCommandDataHeader,"TgsmuTestCommandDataHeader");
_LIT(KTgsmuTestSmsProtocolIdentifier,"TgsmuTestSmsProtocolIdentifier");
_LIT(KTgsmuTestSmsTONandNPI,"TgsmuTestSmsTONandNPI");
_LIT(KTgsmuTestSmsOptimiseSettings,"TgsmuTestSmsOptimiseSettings");
_LIT(KTgsmuTestSmsDefaultVersion,"TgsmuTestSmsDefaultVersion");
_LIT(KTgsmuTestSmsCurrentVersion,"TgsmuTestSmsCurrentVersion");
_LIT(KTgsmuTestSmsVersionBackwards,"TgsmuTestSmsVersionBackwards");
_LIT(KTgsmuTestSMSCCtrlParameterOperations,"TgsmuTestSMSCCtrlParameterOperations");
_LIT(KTgsmuTestTPSRROperations,"TgsmuTestTPSRROperations");
_LIT(KTgsmuTestCSmsEditorBuffer,"TgsmuTestCSmsEditorBuffer");

#endif // __TE_GSMUSTEP_H__
