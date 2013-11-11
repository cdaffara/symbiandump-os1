// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ESockDebug.TestSteps.cpp
*/

#include <comms-infras/esockdebugmessages.h>

#include "SocketServer.TestSteps.h"
#include "ESockDebug.TestSteps.h"

#include <xml/parser.h>
#include <xml/xmlparsererrors.h>
#include "XmlParsing.h"
#include <elements/sd_mintercept.h>

#include <utf.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockTestESckDbg, "ESockTestESckDbg");
#endif

using namespace Xml;

// ESOCK Message Intercept Register
//=================================

// Build message intercept pattern
//--------------------------------
CAppendMessageInterceptPattern::CAppendMessageInterceptPattern(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest)
	{
	SetTestStepName(KAppendMessageInterceptPattern);
	}

TInt CAppendMessageInterceptPattern::ConfigureFromIni()
	{
	// Read in name of socket server used for controlling esock debug
	if((GetStringFromConfig(iSection, KTe_SocketServName, iSocketSvrName) != 1)
		|| (iSocketSvrName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

	// We need to have at least one expression
	TInt success;
	TBool done(EFalse);

	// The expressions are indexed from 1 in our config files
	TInt expressionCount = 1;

	// Fetch the action first
	TAction action;
	TRAPD(error, ReadActionL(action));
	if(error != KErrNone)
		{
		INFO_PRINTF1(_L("Couldn't find action field in config file"));
		return KErrNotFound;
		}
	else
		{
		RExpressionList expressionList;
		CleanupClosePushL(expressionList);

		// We have an action so start looking for expressions to go with it
		// One at a time read in whole expressions till no more to be found
		while(!done)
			{
			TEventExpression eventExpression;

			// Read in the expression
			TRAPD(error, ReadEventExpressionL(expressionCount, eventExpression));

			if(error == KErrNone)
				{
				// Found an expression so lets add it to an expression list for this pattern
				expressionList.Append(eventExpression);
				}
			else if(error == KErrNotFound)
				{
				// Must be the end of list of expressions
				done = ETrue;
				}
			else
				{
				// Some kind of xml parsing issue lets get out of here
				CleanupStack::PopAndDestroy(&expressionList);
				return error;
				}

			expressionCount++;
			}

		if(expressionList.Count() > 0)
			{
			iPattern = CPattern::NewL(expressionList, action);
			CleanupStack::Pop(&expressionList);
			success = KErrNone;
			}
		else
			{
			CleanupStack::PopAndDestroy(&expressionList);
			success = KErrArgument;
			}

		}

	return success;
	}

const TInt KConfigSubSectionNameMaxLength = 64;
const TInt KIniDataMaxLength = 256;

void CAppendMessageInterceptPattern::ReadEventExpressionL(TInt aIndex, TEventExpression& aExpression)
	{
	// Fetch the whole ini data subsection that we will parse
	_LIT(KEventSubSectionFormat, "Event%d");
	TBuf<KConfigSubSectionNameMaxLength> subSectionName;
	subSectionName.Format(KEventSubSectionFormat, aIndex);

	TPtrC iniData;
	if(!GetStringFromConfig(iSection, subSectionName, iniData))
		{
		// We must have already found the last event expression
		User::Leave(KErrNotFound);
		}

	// Damn we need to convert the whole ini section back to UTF8
	TBuf8<KIniDataMaxLength> iniData8;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(iniData8, iniData);

	// Add some tags around the outside. XML parser needs them and leaving them out of the ini keeps things simple
	_LIT8(KSurroundingEventTagsFormat, "<Event>%S</Event>");
	TBuf8<KIniDataMaxLength> iniData8tagged;
	iniData8tagged.Format(KSurroundingEventTagsFormat, &iniData8);

	// Its some xml so lets just parse the lot
	CEventExpressionXmlHandler handler(aExpression);
	CParser* parser = CParser::NewLC(KXmlMimeType, handler);
	TRAPD(error, Xml::ParseL(*parser, iniData8tagged));
	if(error != KErrNone)
		{
		INFO_PRINTF2(_L("Error during XML parsing of (%S)."), &subSectionName);
		User::Leave(KErrArgument);
		}

	// Cleanup
	CleanupStack::PopAndDestroy(parser);
	}

void CAppendMessageInterceptPattern::ReadActionL(TAction& aAction)
	{
	// Fetch the whole ini data subsection that we will parse
	TPtrC iniData;
	_LIT(KAction, "Action");
	if(!GetStringFromConfig(iSection, KAction, iniData))
		{
		User::Leave(KErrNotFound);
		}

	// Damn we need to convert the whole ini section back to UTF8
	TBuf8<KIniDataMaxLength> iniData8;
	CnvUtfConverter::ConvertFromUnicodeToUtf8(iniData8, iniData);

	// Its some xml so lets just parse the lot
	CActionXmlHandler handler(aAction);
	CParser* parser = CParser::NewLC(KXmlMimeType, handler);
	TRAPD(error, Xml::ParseL(*parser, iniData8));
	if(error != KErrNone)
		{
		INFO_PRINTF2(_L("Error during XML parsing of (%S)."), KAction);
		User::Leave(KErrNotFound);
		}

	// Cleanup
	CleanupStack::PopAndDestroy(parser);
	}

TVerdict CAppendMessageInterceptPattern::doSingleTestStep()
	{
	SetTestStepResult(EFail);

	// Look up the socket server being used for esock debug control
	RSocketServ* debugSocketServer = iEsockTest->iSockServs.Find(iSocketSvrName);

	if(debugSocketServer)
		{
		// Make the call to reset the ESock debug register with the socket server given
		TRAPD(error, AppendPatternToRegisterL(*debugSocketServer));
		if(error == KErrNone)
			{
			SetTestStepResult(EPass);
			}
		else
			{
			// If it errored then this step failed
			INFO_PRINTF2(_L("Could not enable the ESock message intercept register. Error (%S)."), error);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Could not find socket server for debug control of esock. Error (%S)."));
		}

	return TestStepResult();
	}

CAppendMessageInterceptPattern::~CAppendMessageInterceptPattern()
	{
	delete iPattern;
	}

void CAppendMessageInterceptPattern::AppendPatternToRegisterL(RSocketServ& /*aSocketSvr*/) const
	{
	__ASSERT_DEBUG(iPattern, User::Panic(KSpecAssert_ESockTestESckDbg, 1));
	TUint numExpressions = iPattern->Expressions().Count();
	__ASSERT_DEBUG(numExpressions > 0, User::Panic(KSpecAssert_ESockTestESckDbg, 2));

//TODO MessageIntercept not operational
#if 0
 	// Pull apart the pattern built and send it across to the message intercept register
	TUint index;
	TInt error;
	for(index = 0; index < numExpressions; index++)
		{
		TEventExpression expression = iPattern->Expressions()[index];

		// Build the debug control message for current expression and send it to esock
		TAppendExpressionMsg msg1(expression);
		error = aSocketSvr.__DbgControl(msg1);
		if(error != KErrNone)
			{
			User::Leave(KErrGeneral);
			}
		}

	// And finally terminate the pattern with the action
	TAppendActionMsg msg2(iPattern->Action());
	error = aSocketSvr.__DbgControl(msg2);
	if(error != KErrNone)
		{
		User::Leave(KErrGeneral);
		}
#endif
	User::Leave(KErrGeneral);
	}



// Enable message intercept register
//----------------------------------
CEnableMessageInterceptRegister::CEnableMessageInterceptRegister(CCEsockTestBase*& aEsockTest)
:   CTe_EsockStepBase(aEsockTest), iSocketSvrName(KNullDesC)
	{
	SetTestStepName(KEnableMessageInterceptRegister);
	}

TInt CEnableMessageInterceptRegister::ConfigureFromIni()
	{
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketServName, iSocketSvrName) != 1)
		|| (iSocketSvrName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

	// All ok if we got this far
	return KErrNone;
	}

TVerdict CEnableMessageInterceptRegister::doSingleTestStep()
	{
	SetTestStepResult(EFail);

	// Look up the socket server being used for esock debug control
	RSocketServ* debugSocketServer = iEsockTest->iSockServs.Find(iSocketSvrName);

//TODO MessageIntercept not operational
#if 0
	if(debugSocketServer)
		{
		// Build a debug control message and send it to esock
		MessageIntercept::TEnableMsg msg;
		TInt error = debugSocketServer->__DbgControl(msg);

		if (error != KErrNone)
			{
			// If it errored then this step failed
			INFO_PRINTF2(_L("Could not enable the ESock message intercept register. Error (%S)."), error);
			}
		else
			{
			SetTestStepResult(EPass);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Could not find socket server for debug control of esock. Error (%S)."));
		}
#endif
	return TestStepResult();
	}


// Disable message intercept
//--------------------------
CDisableMessageInterceptRegister::CDisableMessageInterceptRegister(CCEsockTestBase*& aEsockTest)
	: CTe_EsockStepBase(aEsockTest), iSocketSvrName(KNullDesC)
	{
	SetTestStepName(KDisableMessageInterceptRegister);
	}

TInt CDisableMessageInterceptRegister::ConfigureFromIni()
	{
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketServName, iSocketSvrName) != 1)
		|| (iSocketSvrName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CDisableMessageInterceptRegister::doSingleTestStep()
	{
	SetTestStepResult(EFail);

	// Look up the socket server being used for esock debug control
	RSocketServ* debugSocketServer = iEsockTest->iSockServs.Find(iSocketSvrName);

	if(debugSocketServer)
		{
		// Build a debug control message and send it to esock
//TODO MessageIntercept not operational
#if 0
		MessageIntercept::TDisableMsg msg;
		TInt error = debugSocketServer->__DbgControl(msg);

		if (error != KErrNone)
			{
			// If it errored then this step failed
			INFO_PRINTF2(_L("Could not disable the ESock message intercept register. Error (%S)."), error);
			}
		else
			{
			SetTestStepResult(EPass);
			}
#endif
		}
	else
		{
		INFO_PRINTF1(_L("Could not find socket server for debug control of esock. Error (%S)."));
		}

	// All ok if we got this far
	return TestStepResult();
	}


// Reset message intercept register
//---------------------------------

CResetMessageInterceptRegister::CResetMessageInterceptRegister(CCEsockTestBase*& aEsockTest)
	: CTe_EsockStepBase(aEsockTest), iSocketSvrName(KNullDesC)
	{
	SetTestStepName(KResetMessageInterceptRegister);
	}

TInt CResetMessageInterceptRegister::ConfigureFromIni()
	{
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketServName, iSocketSvrName) != 1)
		|| (iSocketSvrName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CResetMessageInterceptRegister::doSingleTestStep()
	{
    // Default to failing
	SetTestStepResult(EFail);
//TODO MessageIntercept not operational
#if 0
    // Look up the socket server being used for esock debug control
    RSocketServ* debugSocketServer = iEsockTest->iSockServs.Find(iSocketSvrName);

    if(debugSocketServer)
    	{
		// Build a debug control message and send it to esock
		MessageIntercept::TSoftResetMsg msg;
		TInt error = debugSocketServer->__DbgControl(msg);

		if (error != KErrNone)
			{
			// If it errored then this step failed
			INFO_PRINTF2(_L("Could not reset the ESock message intercept register. Error (%S)."), error);
			SetTestStepError(error);
			}
		else
			// Everything ok so we passed
			{
			SetTestStepResult(EPass);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Could not find socket server for debug control of esock. Error (%S)."));
		}
#endif
	return TestStepResult();
	}


// Query completion of message interception
//-----------------------------------------

CQueryMsgInterceptComplete::CQueryMsgInterceptComplete(CCEsockTestBase*& aEsockTest)
	: CTe_EsockStepBase(aEsockTest), iSocketSvrName(KNullDesC)
	{
	SetTestStepName(KQueryMsgInterceptComplete);
	}

TInt CQueryMsgInterceptComplete::ConfigureFromIni()
	{
	// Read in appropriate fields
	if((GetStringFromConfig(iSection, KTe_SocketServName, iSocketSvrName) != 1)
		|| (iSocketSvrName.Length() == 0))
		{
		INFO_PRINTF1(_L("Couldn't find appropriate field in config file"));
		return KErrNotFound;
		}

    // All ok if we got this far
    return KErrNone;
	}

TVerdict CQueryMsgInterceptComplete::doSingleTestStep()
	{
    // Default to failing
	SetTestStepResult(EFail);

    // Look up the socket server being used for esock debug control
    RSocketServ* debugSocketServer = iEsockTest->iSockServs.Find(iSocketSvrName);

//TODO MessageIntercept not operational
#if 0
    if(debugSocketServer)
    	{
		// Build a debug control message and send it to esock
		MessageIntercept::TQueryCompleteMsg msg;
		TInt result = debugSocketServer->__DbgControl(msg);

		if (result < 0)
			{
			// If it errored then this step failed
			INFO_PRINTF2(_L("Could not reset the ESock message intercept register. Error (%S)."), result);
			SetTestStepError(result);
			}
		else if (result == MessageIntercept::EPassedState)
			// Everything ok so we passed
			{
			SetTestStepResult(EPass);
			}
		}
	else
		{
		INFO_PRINTF1(_L("Could not find socket server for debug control of esock. Error (%S)."));
		}
#endif
	return TestStepResult();
	}

