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
// This contains CTestCaseRootServer 


// Test system includes
#include "TestStepRootServer.h"

// const (times in ms)
const TInt CTestStepRootServer::isTestServerLifeTime = 3000;
const TInt CTestStepRootServer::isTestServerWaitTime = 
					2 * CTestStepRootServer::isTestServerLifeTime;
const TInt CTestStepRootServer::isRendezvousDelayTime = 1000;



// constructor
CTestStepRootServer::CTestStepRootServer(CTE_RootServerServer *aSuite) 
{
	iRootServerSuite = aSuite;
}

// destructor
CTestStepRootServer::~CTestStepRootServer()
{
}


//------------------------
TVerdict CTestStepRootServer::doTestStepPreambleL( void )
	{
	TInt ret;

	// kick off the RootServer thread
	INFO_PRINTF1(_L("Attempting to start RootServer"));
	ret = iRootServerSuite->StartRootServer();
	if (KErrNone != ret)
		{
		INFO_PRINTF2(_L("Failed to start RootServer: err %d"), ret);
		return EInconclusive;
		}

        SetTestStepResult(EPass);

	return TestStepResult();
	}


//------------------------
TVerdict CTestStepRootServer::doTestStepPostambleL( void )
	{
	INFO_PRINTF1(_L("Attempting to shutdown RootServer"));
	if (!iRootServerSuite->ShutdownRootServer())
		{
		INFO_PRINTF1(_L("Failed to shutdown RootServer"));
		return EInconclusive;
		}
        SetTestStepResult(EPass);

	return TestStepResult();
	}


//------------------------
void CTestStepRootServer::GetTestStepServerName(const TDesC &aNamePostFix, 
												TDes &aServerName)
	{
	aServerName.Copy(_L("TS_"));
	aServerName.Append(TestStepName());
	aServerName.Append(_L("_"));
	aServerName.AppendNum(iRootServerSuite->NextServerNumber());
	if (aNamePostFix.Length() > 0)
		{
		aServerName.Append(_L("_"));
		aServerName.Append(aNamePostFix);															  
		}
	}


void CTestStepRootServer::DefaultStartParams(TRSStartModuleParams& aStartParams, TTestModuleIniData& aIniData)
	{
	aStartParams.iParams.iFilename = _L("testcpm.cpm");
	aStartParams.iParams.iPriority = EPriorityNormal;
	aStartParams.iParams.iStackSize = 8192;
	aStartParams.iParams.iHeapType = ENewHeap;
	aStartParams.iParams.iMinHeapSize = 65536;
	aStartParams.iParams.iMaxHeapSize = 262144;
	aStartParams.iParams.iShareHeapWith = KNullDesC8;
	aStartParams.iParams.iThreadFunctionOrdinal = 1;
	aStartParams.iParams.iIsSticky = EFalse;
	aStartParams.iParams.iIsServer = EFalse;

	aIniData.iParams.iInitDelay = 0;
	aIniData.iParams.iDeathDelay = NO_PREMATURE_DEATH;
	aIniData.iParams.iDiscoveryDelay = 0;
	aIniData.iParams.iBindDelay = 0;
	aIniData.iParams.iUnbindDelay = 0;
	aIniData.iParams.iShutdownDelay = 0;
	}

void CTestStepRootServer::LoadNormalModuleL(CTestAsynchHandler* aHandler, const TDesC8& aModule, TThreadPriority aPriority)
	{
	TRSStartModuleParams startParams;
	TTestModuleIniData iniData;
	DefaultStartParams(startParams, iniData);
	startParams.iParams.iName = aModule;
	startParams.iParams.iPriority = aPriority;

    aHandler->TryLoad( startParams, iniData);
	
	CActiveScheduler::Start();
	if (aHandler->iStatus != KErrNone) 
		{
		INFO_PRINTF2(_L("Error: load returned %d"), aHandler->iStatus.Int());
		User::Leave(aHandler->iStatus.Int());
		}
	}

TInt CTestStepRootServer::TryHierachicalBind(CTestAsynchHandler* aHandler, 
											 const TDesC8& aTopModule, const TDesC8& aTopSubModule, 
											 const TDesC8& aBottomModule, const TDesC8& aBottomSubModule)
	{
	TRSBindingInfo bindInfo;
	bindInfo.iParams.iType = EHierarchical;
	bindInfo.iParams.iAddress1.SetModule(aTopModule);
	bindInfo.iParams.iAddress1.SetSubModule(aTopSubModule);
	bindInfo.iParams.iAddress2.SetModule(aBottomModule);
	bindInfo.iParams.iAddress2.SetSubModule(aBottomSubModule);
	bindInfo.iParams.iState1 = KErrNone;
	bindInfo.iParams.iState2 = KErrNone;
	bindInfo.iParams.iForwardQLength = TRSBindingInfo::EMinQueueLength;	
	bindInfo.iParams.iReverseQLength = TRSBindingInfo::EMinQueueLength;	
	
	aHandler->TryBind( bindInfo );
	CActiveScheduler::Start();
	return aHandler->iStatus.Int();
	}

_LIT(KSingleDesFmt1, "%S");
_LIT(KSingleDesFmt2, "%d %S");
_LIT(KSingleDesFmt4, "%S %d %d %S");

void CTestStepRootServer::Log( TRefByValue<const TDesC8> aFormat, ... )
	{
	// Although a memory-spendthrift approach it's ok for the case of TS_ROOTSERVER
	TBuf8<EMaxLogLength> buf8;
	VA_LIST list;
	VA_START(list, aFormat);
	buf8.FormatList(aFormat, list);
	TBuf16<EMaxLogLength> buf16;
	buf16.Copy(buf8);
	INFO_PRINTF2(KSingleDesFmt1, &buf16);
	}

void CTestStepRootServer::Log( TInt aSeverity, TRefByValue<const TDesC8> aFormat, ... )
	{
	// Although a memory-spendthrift approach it's ok for the case of TS_ROOTSERVER
	TBuf8<EMaxLogLength> buf8;
	VA_LIST list;
	VA_START(list, aFormat);
	buf8.FormatList(aFormat, list);
	TBuf16<EMaxLogLength> buf16;
	buf16.Copy(buf8);
	INFO_PRINTF3(KSingleDesFmt2, aSeverity, &buf16);
	}

void CTestStepRootServer::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity, TRefByValue<const TDesC8> aFormat,...)
	{
	// Although a memory-spendthrift approach it's ok for the case of TS_ROOTSERVER
	TBuf8<EMaxLogLength> buf8;
	VA_LIST list;
	VA_START(list, aFormat);
	buf8.FormatList(aFormat, list);
	TBuf16<EMaxLogLength> buf16;
	buf16.Copy(buf8);
	INFO_PRINTF5(KSingleDesFmt4, aFile, aLine, aSeverity, &buf16);
	}

void CTestStepRootServer::Log( TRefByValue<const TDesC16> aFormat, ... )
	{
	// Although a memory-spendthrift approach it's ok for the case of TS_ROOTSERVER
	TBuf16<EMaxLogLength> buf16;
	VA_LIST list;
	VA_START(list, aFormat);
	buf16.FormatList(aFormat, list);
	INFO_PRINTF2(KSingleDesFmt1, &buf16);
	}

void CTestStepRootServer::Log( TInt aSeverity, TRefByValue<const TDesC16> aFormat, ... )
	{
	// Although a memory-spendthrift approach it's ok for the case of TS_ROOTSERVER
	TBuf16<EMaxLogLength> buf16;
	VA_LIST list;
	VA_START(list, aFormat);
	buf16.FormatList(aFormat, list);
	INFO_PRINTF3(KSingleDesFmt2, aSeverity, &buf16);
	}

void CTestStepRootServer::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
	TRefByValue<const TDesC> aFormat,...)
	{
	// Although a memory-spendthrift approach it's ok for the case of TS_ROOTSERVER
	TBuf16<EMaxLogLength> buf16;
	VA_LIST list;
	VA_START(list, aFormat);
	buf16.FormatList(aFormat, list);
	INFO_PRINTF5(KSingleDesFmt4, aFile, aLine, aSeverity, &buf16);
	}
