// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code
*/

// A "utility class" that contains a number of static functions that don't 
// have anywhere else obvious to live.

#include "egltest_endpoint_util.h"
#include <test/testexecuteserverbase.h>


/**
Cut and paste from CTestStep::SetLogger() - which is not exported
As the name suggests, this is for use by the egl test process wrapper
*/
void TEndpointUtil::SetLoggerForProcessWrapperL(CTestExecuteLogger& aLogger)
    {
    // Create a cinidata object for parsing the testexecute.ini
    CTestExecuteIniData* parseTestExecuteIni = NULL;
    TBuf<KMaxTestExecuteNameLength> resultFilePath;
    TBuf<KMaxTestExecuteNameLength> xmlFilePath;
    TInt logMode;
    TInt logLevel;

    TRAPD(err,parseTestExecuteIni = CTestExecuteIniData::NewL());
    if (err == KErrNone)
        {
        CleanupStack::PushL(parseTestExecuteIni);
        parseTestExecuteIni->ExtractValuesFromIni();
        parseTestExecuteIni->GetKeyValueFromIni(KTEFHtmlKey, resultFilePath);
        parseTestExecuteIni->GetKeyValueFromIni(KTEFXmlKey, xmlFilePath);
        parseTestExecuteIni->GetKeyValueFromIni(KTEFLogMode, logMode);
        parseTestExecuteIni->GetKeyValueFromIni(KTEFLogSeverityKey, logLevel);
        }
    else
        {
        resultFilePath.Copy(KTestExecuteLogPath);
        xmlFilePath.Copy(KTestExecuteLogPath);
        logMode = TLoggerOptions(ELogHTMLOnly);
        logLevel = RFileFlogger::TLogSeverity(ESevrAll);
        }
    aLogger.SetLoggerOptions(logMode);

    // Initialise a handle to the file logger
    User::LeaveIfError(aLogger.Connect());
    RFs fS;
    User::LeaveIfError(fS.Connect());
    CleanupClosePushL(fS);
    RFile file;
    TBuf<KMaxTestExecuteNameLength> xmlLogFile(xmlFilePath);
    TBuf<KMaxTestExecuteNameLength> logFile;
    TBuf<KMaxTestExecuteNameLength> logFileNameFile(resultFilePath);
    logFileNameFile.Append(KTestExecuteScheduleTestLogCompatibilityNameFile);
    if(file.Open(fS,logFileNameFile,EFileRead | EFileShareAny) != KErrNone)
        {
        User::Panic(_L("Unsupported"), -1);
        }
    else
        {
        CleanupClosePushL(file);
        TBuf8<KMaxTestExecuteNameLength> logFile8;
        TInt fileSize;
        User::LeaveIfError(file.Size(fileSize));
        User::LeaveIfError(file.Read(logFile8,fileSize));
        logFile.Copy(logFile8);
        xmlLogFile.Append(logFile);
        _LIT(KXmlExtension,".xml");
        xmlLogFile.Append(KXmlExtension);
        _LIT(KHtmExtension,".htm");
        logFile.Append(KHtmExtension);
        CleanupStack::Pop(&file);
        file.Close();
        }
    TBuf<KMaxTestExecuteLogFilePath> logFilePath(resultFilePath);
    logFilePath.Append(logFile);
    CleanupStack::Pop(&fS);
    fS.Close();

    if (logMode == 0 || logMode == 2)
        {
        User::LeaveIfError(aLogger.HtmlLogger().CreateLog(logFilePath,RTestExecuteLogServ::ELogModeAppend));
        aLogger.HtmlLogger().SetLogLevel(TLogSeverity(logLevel));
        }
    if (logMode == 1 || logMode == 2)
        {
        User::LeaveIfError(aLogger.XmlLogger().CreateLog(xmlLogFile,RFileFlogger::ELogModeAppend));
        aLogger.XmlLogger().SetLogLevel(RFileFlogger::TLogSeverity(logLevel));
        }
    if (parseTestExecuteIni != NULL)
        {
        CleanupStack::PopAndDestroy(parseTestExecuteIni);
        }
    }
