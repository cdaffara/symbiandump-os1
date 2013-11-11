/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file tefexportconst.h
* 
*
*/



#ifndef TEF_EXPORT_CONST_H
#define TEF_EXPORT_CONST_H
#include <e32def.h>

_LIT(KInfoPrint, "Info ");
_LIT(KWarnPrint, "Warning ");
_LIT(KErrorPrint,"Error ");

// Logger Macros
#define INFO_PRINTF1(p1)							Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Info ") , (p1))
#define INFO_PRINTF2(p1, p2)						Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Info ") , (p1), (p2))
#define INFO_PRINTF3(p1, p2, p3)					Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Info ") , (p1), (p2), (p3))
#define INFO_PRINTF4(p1, p2, p3, p4)				Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Info ") , (p1), (p2), (p3), (p4))
#define INFO_PRINTF5(p1, p2, p3, p4, p5)			Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Info ") , (p1), (p2), (p3), (p4), (p5))
#define INFO_PRINTF6(p1, p2, p3, p4, p5, p6)		Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Info ") , (p1), (p2), (p3), (p4), (p5), (p6))
#define INFO_PRINTF7(p1, p2, p3, p4, p5, p6, p7)	Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Info ") , (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define WARN_PRINTF1(p1)							Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Warning ") , (p1)) 
#define WARN_PRINTF2(p1, p2)						Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Warning ") , (p1), (p2)) 
#define WARN_PRINTF3(p1, p2, p3)					Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Warning ") , (p1), (p2), (p3)) 
#define WARN_PRINTF4(p1, p2, p3, p4)				Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Warning ") , (p1), (p2), (p3), (p4)) 
#define WARN_PRINTF5(p1, p2, p3, p4, p5)			Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Warning ") , (p1), (p2), (p3), (p4), (p5)) 
#define WARN_PRINTF6(p1, p2, p3, p4, p5, p6)		Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Warning ") , (p1), (p2), (p3), (p4), (p5), (p6)) 
#define WARN_PRINTF7(p1, p2, p3, p4, p5, p6, p7)	Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Warning ") , (p1), (p2), (p3), (p4), (p5), (p6), (p7)) 

#define ERR_PRINTF1(p1)								Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Error ") , (p1)) 
#define ERR_PRINTF2(p1, p2)							Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Error ") , (p1), (p2)) 
#define ERR_PRINTF3(p1, p2, p3)						Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Error ") , (p1), (p2), (p3)) ;
#define ERR_PRINTF4(p1, p2, p3, p4)					Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Error ") , (p1), (p2), (p3), (p4)) 
#define ERR_PRINTF5(p1, p2, p3, p4, p5)				Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Error ") , (p1), (p2), (p3), (p4), (p5)) 
#define ERR_PRINTF6(p1, p2, p3, p4, p5, p6)			Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Error ") , (p1), (p2), (p3), (p4), (p5), (p6)) 
#define ERR_PRINTF7(p1, p2, p3, p4, p5, p6, p7)		Logger()->DiagPrint(((TText8*)__FILE__), __LINE__, _L("Error ") , (p1), (p2), (p3), (p4), (p5), (p6), (p7))

const TInt KMaxTestExecuteNameLength = 80;

enum TVerdict
	{
	EPass =0,			
	EFail = 106,
	EInconclusive,
	ETestSuiteError,
	EAbort,
	EIgnore
	};

enum TLoggerOptions
	{
	ELogHTMLOnly,
	ELogXMLOnly,
	ELogBoth
	};

_LIT(KTEFResultPass,"PASS");
_LIT(KTEFResultFail,"FAIL");
_LIT(KTEFResultTag,"***Result");
_LIT(KTEFResult,"!Result=");

_LIT(KTEFResultPanic,"PANIC");
_LIT(KTEFResultUnexecuted, "UNEXECUTED");
_LIT(KTEFTestCaseResultTag,"***TestCaseResult");
_LIT(KTEFCommentedCommands,"COMMENTED OUT COMMAND'S");
_LIT(KTEFTestCaseSummary, "TEST_CASE_SUMMARY");
_LIT(KTEFTestStepSummary, "TEST_STEP_BLOCK_SUMMARY");
_LIT(KTEFRunProgramSummary, "RUN_PROGRAM_SUMMARY");
_LIT(KTEFRunScriptSummary, "RUN_SCRIPT_SUMMARY:");
_LIT(KTEFScriptExecuted, "EXECUTED_SCRIPTS");
_LIT(KTEFFailedOpen, "FAILED_TO_OPEN");
_LIT(KTEFOpenQuotes,"\"");
_LIT(KTEFSpaceEquals," = ");
_LIT(KTEFSection, "TestExecute");
_LIT(KTEFXmlKey, "XML");
_LIT(KTEFEqualsInteger," = %d");

_LIT(KTEFResultInconclusive,"INCONCLUSIVE");
_LIT(KTEFResultAbort,"ABORT");
_LIT(KTEFResultUnknown,"UNKNOWN");
_LIT(KTEFFontLightBlue, "<font color=00AFFF>");
_LIT(KTEFFontGreen,"<font color=00AF00>");
_LIT(KTEFFontRed,"<font color=FF0000>");
_LIT(KTEFFontBlue,"<font color=0000FF>");
_LIT(KTEFFontEnd,"</font>");
_LIT(KTEFTestCaseDefault, "NOTESTCASE");

#endif
