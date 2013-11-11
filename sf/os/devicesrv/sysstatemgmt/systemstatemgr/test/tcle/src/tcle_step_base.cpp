// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ssm/ssmcommandlist.h>
#include <s32file.h> 
#include <bautils.h>
#include <ssm/ssmcommand.h>
#include <ssm/ssmcommandfactory.h>
#include <ssm/ssmstartupproperties.h>
#include <ssm/ssmcmd.hrh>

#include "ssmcommandbase.h"
#include "ssmcommandlistinterface.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
#include "clesrv.h"
#include "clecli.h"
#include "tcle_step_base.h"


#include "../../testapps/inc/ssmtestapps.h"

//
//
/**
 * Creates a command list with commands that will succeed. 
 * The commands are of the specified type (process, app, custom), the execution behaviour requested
 * and the number of commands asked for. Every third command should fail, retry and launch successfully.
 * With ESsmDeferredWaitForSignal, there are 2 additional commands:
 * a MultipleWait and a StartProcess command.
 */  
CSsmCommandList* CCleTestStepBase::CreateGoodCommandListL(TCleTestCommandType aCommandType, TSsmExecutionBehaviour aExecutionBehaviour, TInt aCount)
	{
	INFO_PRINTF2(_L("Constructing a good Command list consisting of '%d' commands..."), aCount);
		
	CSsmCommandList* cmdList = CSsmCommandList::NewLC();
	
	if (ESsmWaitForSignal == aExecutionBehaviour)
		{
		for(TInt appNum = 0; appNum < aCount; appNum++)
			{
			TBuf<4> arg;
			arg.Format(_L("%d"), appNum);
			TInt delay = 100;
			TInt timeout = 1000;
			TInt failures = 0;
			TInt retries = 0;
			
			if (appNum%3 == 2) //Every third command should fail, retry and launch successfully
				{
				delay = 0;
				failures = 1;
				retries = 2;
				}
			
			MSsmCommand* command = NULL;
			command = CreateGoodCommandLC(aCommandType, aExecutionBehaviour, arg, delay, timeout, failures, retries );
			cmdList->AppendL(command);
			CleanupStack::Pop(command);
			}
		}
	else
		{
		for(TInt appNum = 0; appNum < aCount; appNum++)
			{
			TBuf<4> arg;
			arg.Format(_L("%d"), appNum);
			MSsmCommand* command = NULL;
			command = CreateGoodCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 1000, 0, 0);
			cmdList->AppendL(command);
			CleanupStack::Pop(command);
			}

		if(ESsmDeferredWaitForSignal == aExecutionBehaviour)
			{
			INFO_PRINTF1(_L("Creating MultipleWait command"));
			MSsmCommand* command = NULL;
			command = SsmCommandFactory::ConstructMultipleWaitCommandLC(10000);
			cmdList->AppendL(command);
			CleanupStack::Pop(command);
			
			command = NULL;
			command = CreateGoodCommandLC(EProcess, ESsmFireAndForget, _L("final"), 0, 500, 0, 0); //To show that the MultipleWait didn't fail
			cmdList->AppendL(command);
			CleanupStack::Pop(command);
			
			}
		}
	
	CleanupStack::Pop(cmdList);
	return cmdList;
	}

/**
 * For each type of command execution, a cycle (up to the number of commands requested) of commands that fail
 * (except for those marked EDontFail).
 * 	ESsmWaitForSignal:
 * 		EPanic, EBadRendezvous, ENoRendezvous, EBadPath(ECmdCriticalSeverity), EDontFail
 * 	ESsmFireAndForget:
 * 		EPanic, EBadRendezvous, EBadPath, EDontFail
 * 	ESsmDeferredWaitForSignal: (With MultipleWait and EDontFail appended)
 * 		ENormal, EIgnoreAll:
 * 			EPanic, EBadRendezvous(ECmdCriticalSeverity for ENormal), EBadPath
 * 		EDontRendezvous:
 * 			ENoRendezvous, ENoRendezvous(ECmdCriticalSeverity), ENoRendezvous
 * 
 * The switch statements allow this cycling of different command types.
 */
CSsmCommandList* CCleTestStepBase::CreateBadCommandListL(TCleTestCommandType aCommandType, TSsmExecutionBehaviour aExecutionBehaviour, TInt aCount, TDwfsListType aDwfsListType)
	{
	INFO_PRINTF2(_L("Constructing a bad Command list consisting of '%d' commands..."), aCount);

	CSsmCommandList* cmdList = CSsmCommandList::NewLC();
	
	switch (aExecutionBehaviour)
		{
		case ESsmWaitForSignal:
			{
			for(TInt appNum = 0; appNum < aCount; appNum++)
				{
				TBuf<10> arg;
				arg.Format(_L("%d"), appNum);
				MSsmCommand* command = NULL;
				switch (appNum%5)
					{
					case 0:
						{
						// This cmd will fail but severity is set to ECmdIgnoreFailure 
						command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 500, 0, 0, EPanic);
						break;
						}
					case 1:
						{
						// This cmd will fail but severity is set to ECmdIgnoreFailure 
						command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 500, 0, 0, EBadRendezvous);
						break;
						}
					case 2:
						{
						// This cmd will fail but severity is set to ECmdIgnoreFailure 
						command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 500, 0, 0, ENoRendezvous);
						break;
						}
					case 3:
						{
						// This cmd will fail and severity is set to ECmdCriticalSeverity ==> Execution will halt
						command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 500, 0, 0, EBadPath, ECmdCriticalSeverity);
						break;
						}
					case 4:
						{
						// This is a good command but will never run because the previous cmd failed
						command = CreateGoodCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 500, 0, 0);
						break;
						}
					}
				cmdList->AppendL(command);
				CleanupStack::Pop(command);
				}
			break;
			}
		case ESsmFireAndForget:
			{
			for(TInt appNum = 0; appNum < aCount; appNum++)
				{
				TBuf<10> arg;
				arg.Format(_L("%d"), appNum);
				MSsmCommand* command = NULL;
				switch (appNum%4)
					{
					case 0:
						{
						command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 1000, 0, 0, EPanic);
						break;
						}
					case 1:
						{
						command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 1000, 0, 0, EBadRendezvous);
						break;
						}
					case 2:
						{
						command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 1000, 0, 0, EBadPath);
						break;
						}
					case 3:
						{
						command = CreateGoodCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 1000, 0, 0);
						break;
						}
					}
				cmdList->AppendL(command);
				CleanupStack::Pop(command);//Command
				}
			break;
			}
		case ESsmDeferredWaitForSignal:
			{
			for(TInt appNum = 0; appNum < aCount; appNum++)
				{
				TBuf<10> arg;
				arg.Format(_L("%d"), appNum);
				MSsmCommand* command = NULL;
				switch (aDwfsListType)
					{
					case ENormal:
						{
						switch (appNum%3)
							{
							case 0:
								{
								command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 0, 0, 0, EPanic);
								break;
								}
							case 1:
								{
								command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 0, 0, 0, EBadRendezvous, ECmdCriticalSeverity);
								break;
								}
							case 2:
								{
								command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 0, 0, 0, EBadPath);
								break;
								}
							}
						break;
						}
					case EIgnoreAll:
						{
						switch (appNum%3)
							{
							case 0:
								{
								command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 0, 0, 0, EPanic);
								break;
								}
							case 1:
								{
								command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 0, 0, 1, EBadRendezvous);
								break;
								}
							case 2:
								{
								command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 0, 0, 0, EBadPath);
								break;
								}
							}
						break;
						}
					case EDontRendezvous:
						{
						switch (appNum%3)
							{
							case 0:
								{
								command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 0, 0, 0, ENoRendezvous);
								break;
								}
							case 1:
								{
								command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 0, 0, 0, ENoRendezvous, ECmdCriticalSeverity);
								break;
								}
							case 2:
								{
								command = CreateBadCommandLC(aCommandType, aExecutionBehaviour, arg, 0, 0, 0, 0, ENoRendezvous);
								break;
								}
							}
						break;
						}
					}
				cmdList->AppendL(command);
				CleanupStack::Pop(command);
				}
				
		
			INFO_PRINTF1(_L("Creating MultipleWait command"));
			MSsmCommand* command = NULL;
			command = SsmCommandFactory::ConstructMultipleWaitCommandLC(10000);
			cmdList->AppendL(command);
			CleanupStack::Pop(command);
			
			command = NULL;
			command = CreateGoodCommandLC(EProcess, ESsmFireAndForget, _L("final"), 0, 500, 0, 0); //To show that the MultipleWait did or didn't fail
			cmdList->AppendL(command);
			CleanupStack::Pop(command);

			break;
			}
		}
	
	CleanupStack::Pop(cmdList);
	return cmdList;
	}
	
/**
 * Creates a command that will ultimately launch successfully, with the properties specified.
 */
MSsmCommand* CCleTestStepBase::CreateGoodCommandLC(TCleTestCommandType aCommandType, TSsmExecutionBehaviour aExecutionBehaviour, const TDesC& aLogPrefix, TInt aDelay, TInt aTimeout, TInt aFailures, TInt aRetries)
	{
	CSsmStartupProperties* props = CSsmStartupProperties::NewLC();
	props->SetRetries(aRetries);
	props->SetTimeout(aTimeout);
	props->SetExecutionBehaviour(aExecutionBehaviour);
	
	MSsmCommand* cmd = NULL;
	
	switch (aCommandType)
		{
		case EProcess:
			{
			TBuf<KTestCommandLineMaxLength> args;
			args.Format(_L("%S %d %d"), &aLogPrefix, aDelay, aFailures);
			props->SetFileParamsL(KSsmCleTestProc, args);
			INFO_PRINTF3(_L("Creating start process command with process name: %S, argument string: %S"), &KSsmCleTestProc, &args);
			cmd = SsmCommandFactory::ConstructStartProcessCommandLC(ECmdMediumSeverity, props);
			break;
			}
		case EApp:
			{
			TBuf<KTestCommandLineMaxLength> args;
			args.Format(_L("%S %d %d"), &aLogPrefix, aDelay, aFailures);
			props->SetFileParamsL(KSsmCleTestApplication, args);
			INFO_PRINTF3(_L("Creating start app command with app name: %S, argument string: %S"), &KSsmCleTestApplication, &args);
			cmd = SsmCommandFactory::ConstructStartApplicationCommandLC(ECmdMediumSeverity, props);
			break;
			}
		case ECustom:
			{
			INFO_PRINTF3(_L("Creating start custom command with process name: %S, argument string: %S"), &KTestProcWriteTime, &aLogPrefix);
			break;
			}
		default:
			{
			User::Leave(KErrArgument);
			}
		}
		
		CleanupStack::Pop(cmd);
		CleanupStack::PopAndDestroy(props);
		CleanupStack::PushL(cmd);
		return cmd;
	}
	
/**
 * Creates a command that will ultimately fail to launch successfully, with the properties specified.
 */
MSsmCommand* CCleTestStepBase::CreateBadCommandLC(TCleTestCommandType aCommandType, TSsmExecutionBehaviour aExecutionBehaviour, const TDesC& aLogPrefix, TInt aDelay, TInt aTimeout, TInt aFailures, TInt aRetries, TCleTestFailHow aFailHow, TCmdErrorSeverity aSeverity)
	{
	CSsmStartupProperties* props = CSsmStartupProperties::NewLC();
	props->SetRetries(aRetries);
	props->SetTimeout(aTimeout);
	props->SetExecutionBehaviour(aExecutionBehaviour);
	
	MSsmCommand* cmd = NULL;
	
	switch (aCommandType)
		{
		case EProcess:
			{
			TBuf<KTestCommandLineMaxLength> args;
			args.Format(_L("%S %d %d %d"), &aLogPrefix, aDelay, aFailures, aFailHow);
			if (EBadPath == aFailHow)
				{
				props->SetFileParamsL(KSsmCleTestNotExist, args);
				}
			else
				{
				props->SetFileParamsL(KSsmCleTestProc, args);
				}
			INFO_PRINTF3(_L("Creating start process command with process name: %S, argument string: %S"), &KSsmCleTestProc, &args);
			cmd = SsmCommandFactory::ConstructStartProcessCommandLC(aSeverity, props);
			break;
			}
		case EApp:
			{
			TBuf<KTestCommandLineMaxLength> args;
			args.Format(_L("%S %d %d %d"), &aLogPrefix, aDelay, aFailures, aFailHow);
			if (EBadPath == aFailHow)
				{
				props->SetFileParamsL(KSsmCleTestNotExist, args);
				}
			else
				{
				props->SetFileParamsL(KSsmCleTestApplication, args);
				}
			INFO_PRINTF3(_L("Creating start app command with app name: %S, argument string: %S"), &KSsmCleTestApplication, &args);
			cmd = SsmCommandFactory::ConstructStartApplicationCommandLC(aSeverity, props);
			break;
			}
		case ECustom:
			{
			INFO_PRINTF3(_L("Creating start custom command with process name: %S, argument string: %S"), &KTestProcWriteTime, &aLogPrefix);
			break;
			}
		default:
			{
			User::Leave(KErrArgument);
			}
		}
		
		CleanupStack::Pop(cmd);
		CleanupStack::PopAndDestroy(props);
		CleanupStack::PushL(cmd);
		return cmd;
	}
	
/**
 * Reads a time from a file
 */
TTime CCleTestStepBase::GetTimeFromFileL(RFs aFs, const TDesC& aFileName)
	{
	RFileReadStream rs;
	TInt err = KErrNone;
	TInt count = 0;
	err = rs.Open(aFs, aFileName, EFileRead);
	while ((err == KErrInUse) && (count < 20))
		{
		User::After(500000); //Half a second
		err = rs.Open(aFs, aFileName, EFileRead);
		count++;
		}
	User::LeaveIfError(err);
	rs.PushL();
	TUint32 high = 0;
	TUint32 low = 0;
	high = rs.ReadUint32L();
	low = rs.ReadUint32L();
	CleanupStack::PopAndDestroy(); // rs
	TTime startTime(MAKE_TINT64(high,low));
	return startTime;
	}
	
/**
 * Reads an integer from a file
 */
TInt CCleTestStepBase::GetRunCountFromFileL(RFs aFs, const TDesC& aFileName)
	{
	RFileReadStream file;
	TInt err = KErrNone;
	TInt count = 0;
	err = file.Open(aFs, aFileName, EFileRead);
	while ((err == KErrInUse) && (count < 20))
		{
		User::After(500000); //Half a second
		err = file.Open(aFs, aFileName, EFileRead);
		count++;
		}
	User::LeaveIfError(err);
	TInt result = 0;
	file.PushL();
	result = file.ReadInt32L();
	CleanupStack::PopAndDestroy(); //file
	return result;
	}

/**
 * Reads the results for each app or process. For ESsmDeferredWaitForSignal also reads for the final process.
 */
void CCleTestStepBase::GetTestResultsL(RFs aFs, TInt aCount, TSsmExecutionBehaviour aExecutionBehaviour)
	{
	for(TInt i = 0; i < aCount; ++i)
		{
		TBuf<KTestCommandLineMaxLength> logfilePrefix;
		logfilePrefix.Format(_L("%S%d"), &KTestAppLogFileLoc, i);
		TBuf<KTestCommandLineMaxLength> startTimeFile;
		TBuf<KTestCommandLineMaxLength> runCountFile;
		TBuf<KTestCommandLineMaxLength> resultFile;
		startTimeFile.Format(_L("%Stime"), &logfilePrefix);
		runCountFile.Format(_L("%Scount"), &logfilePrefix);
		resultFile.Format(_L("%Sresult"), &logfilePrefix);
		TCleTestResult result;
		// Get Start Time from file
		if (BaflUtils::FileExists(aFs, startTimeFile))
			{
			result.iStartTime = GetTimeFromFileL(aFs, startTimeFile);
			result.iStartHour = result.iStartTime.DateTime().Hour();
			result.iStartMinute = result.iStartTime.DateTime().Minute();
			result.iStartSecond = result.iStartTime.DateTime().Second();
			}
		
		// Get Run Result from file
		if (BaflUtils::FileExists(aFs, resultFile))
			{
			result.iResult = GetRunCountFromFileL(aFs,resultFile);
			}
		else
			{
			result.iResult = KErrNotFound; // indicates result not found
			}
		
		//Get Run Count from file
		if (BaflUtils::FileExists(aFs, runCountFile))
			{
			result.iStartCount = GetRunCountFromFileL(aFs, runCountFile);
			}
		else
			{
			result.iStartCount = 0;
			}
				
		iTestResults.AppendL(result);
		}
		
	if(ESsmDeferredWaitForSignal == aExecutionBehaviour)
		{
		TBuf<KTestCommandLineMaxLength> logfilePrefix;
		logfilePrefix.Format(_L("%S%S"), &KTestAppLogFileLoc, &KFinal);
		TBuf<KTestCommandLineMaxLength> startTimeFile;
		TBuf<KTestCommandLineMaxLength> runCountFile;
		TBuf<KTestCommandLineMaxLength> resultFile;
		startTimeFile.Format(_L("%Stime"), &logfilePrefix);
		runCountFile.Format(_L("%Scount"), &logfilePrefix);
		resultFile.Format(_L("%Sresult"), &logfilePrefix);
		TCleTestResult result;
		
		// Get Start Time from file
		if (BaflUtils::FileExists(aFs, startTimeFile))
			{
			result.iStartTime = GetTimeFromFileL(aFs, startTimeFile);
			result.iStartHour = result.iStartTime.DateTime().Hour();
			result.iStartMinute = result.iStartTime.DateTime().Minute();
			result.iStartSecond = result.iStartTime.DateTime().Second();
			}
		
		// Get Run Result from file
		if (BaflUtils::FileExists(aFs, resultFile))
			{
			result.iResult = GetRunCountFromFileL(aFs,resultFile);
			}
		else
			{
			result.iResult = -1; // indicates result not found
			}
		
		//Get Run Count from file
		if (BaflUtils::FileExists(aFs, runCountFile))
			{
			result.iStartCount = GetRunCountFromFileL(aFs, runCountFile);
			}
		else
			{
			result.iStartCount = 0;
			}
		
		
		iTestResults.AppendL(result);
		}
	}
	
/**
 * Delete the log files created by the processes and apps launched by the command list.
 */
void CCleTestStepBase::DeleteLogFiles(RFs aFs, TInt aCount)
	{
	for(TInt i = 0; i < aCount; ++i)
		{
		TBuf<KTestCommandLineMaxLength> logfilePrefix;
		logfilePrefix.Format(_L("%S%d"), &KTestAppLogFileLoc, i);
		TBuf<KTestCommandLineMaxLength> startTimeFile;
		TBuf<KTestCommandLineMaxLength> runCountFile;
		TBuf<KTestCommandLineMaxLength> resultFile;
		startTimeFile.Format(_L("%Stime"), &logfilePrefix);
		runCountFile.Format(_L("%Scount"), &logfilePrefix);
		resultFile.Format(_L("%Sresult"), &logfilePrefix);

		aFs.Delete(startTimeFile);
		aFs.Delete(runCountFile);
		aFs.Delete(resultFile);
		}
	
	
	TBuf<KTestCommandLineMaxLength> logfilePrefix;
	logfilePrefix.Format(_L("%S%S"), &KTestAppLogFileLoc, &KFinal);
	TBuf<KTestCommandLineMaxLength> startTimeFile;
	TBuf<KTestCommandLineMaxLength> runCountFile;
	TBuf<KTestCommandLineMaxLength> resultFile;
	
	startTimeFile.Format(_L("%Stime"), &logfilePrefix);
	runCountFile.Format(_L("%Scount"), &logfilePrefix);
	resultFile.Format(_L("%Sresult"), &logfilePrefix);

	aFs.Delete(startTimeFile);
	aFs.Delete(runCountFile);
	aFs.Delete(resultFile);	
	}

/**
Execute a command list.
*/
void CCleTestStepBase::ExecuteCommandListL(CSsmCommandList* aCmdList, TInt aExpectedResult)
	{
	INFO_PRINTF1(_L("ExecuteCommandListL() started..."));
	TRequestStatus trs;
	TCmdErrorSeverity severity;
	iCleCli.ExecuteCommandList(*aCmdList, trs, severity);	
	User::WaitForRequest(trs);
	INFO_PRINTF3(_L("WaitForRequest completed with '%d', expected was '%d'."), trs.Int(), aExpectedResult);
	TEST(trs == aExpectedResult);
	INFO_PRINTF1(_L("ExecuteCommandListL() finished!"));
	}

/*  Find and kill all intances of the specified process name
 */
TInt CCleTestStepBase::FindAndKill(const TDesC& aProcessName)
 	{
 	TFullName searchTerm(aProcessName);
	StripExtension( searchTerm );	
 	searchTerm += _L("*");
 	
 	TFindProcess find(searchTerm);
 	TFullName name;
 	TInt instancesFound = 0;
 	TParse appParseName;
 	 	
 	
 	while(find.Next(name) == KErrNone)
 		{
 		RProcess process;
 		const TInt err = process.Open(name);
 		
 		if ( KErrNone == err)
 			{
 	 		if (process.ExitType() == EExitPending)
 				{
 				name = process.FileName();
 				appParseName.SetNoWild(name,NULL,NULL);
 	 			TPtrC appName = appParseName.NameAndExt();
 				if(appName == aProcessName)
 					{
 					instancesFound++; 					
 					process.Kill(KErrCancel);
					INFO_PRINTF2(_L("Process %S found and killed"), &aProcessName);
 					}
 				}
 			process.Close(); 
 			}
 		}
 	return instancesFound;
 	}
void CCleTestStepBase::StripExtension( TDes& aFilename )
	{
	TInt dot = aFilename.Find( _L(".") );
	
	if( KErrNotFound != dot)
		{
		aFilename.SetLength( dot );
		}
	
	}

//
//

CCleTestStepBase::CCleTestStepBase()
	{
	}

CCleTestStepBase::~CCleTestStepBase()
	{
	iCleCli.Close();
	iTestResults.Close();
	}
	
TVerdict CCleTestStepBase::doTestStepPreambleL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	TInt err = fs.MkDirAll(KTestAppLogFileLoc);
	if (KErrAlreadyExists == err)
		{
		//If the folder already exists, delete any old leftover files
		CFileMan* fm = CFileMan::NewL(fs);
		CleanupStack::PushL(fm);
		TBuf<KTestCommandLineMaxLength> wildPath;
		wildPath.Append(KTestAppLogFileLoc);
		wildPath.Append(_L("*.*"));
		err = fm->Delete(wildPath);
		if (KErrNotFound != err)
			{
			ERR_PRINTF2(_L("Attempt to clear out results driectory resulted in an error (%d)"),err);
			User::LeaveIfError(err);
			}
		CleanupStack::PopAndDestroy(); //fm
		}
	else
		{
		ERR_PRINTF2(_L("Attempt to create results driectory resulted in an error (%d)"),err);
		User::LeaveIfError(err);
		}
	
	CleanupStack::PopAndDestroy(); //fs
	
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CCleTestStepBase::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}
