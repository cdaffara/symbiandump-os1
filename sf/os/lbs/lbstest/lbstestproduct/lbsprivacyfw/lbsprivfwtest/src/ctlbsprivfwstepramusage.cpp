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
// @file ctlbsprivfwstepramusage.cpp
// This is the class implementation for the LBS Client Test Step Base
// 
//

#include "ctlbsprivfwstepramusage.h"
#include <lbs/lbsloccommon.h>

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"

_LIT(KlogFilePath, "c:\\logs\\lbs\\");
_LIT(KNRHFile,	   "lbsnetworkrequesthandler_max_ram.txt");
_LIT(KNGFile,	   "lbsnetgateway_max_ram.txt");

const TInt KMaxFileLineSize = 100;


_LIT( KTxtRequestorName1,	"Requester1" );
_LIT( KTxtLcsClientName1,	"LcsClient1" );

_LIT( KTxtRequestorName2,	"Requester2" );
_LIT( KTxtLcsClientName2,	"LcsClient2" );

_LIT( KTxtRequestorName3,	"Requester3" );
_LIT( KTxtLcsClientName3,	"LcsClient3" );

/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepRamUsage::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepRamUsage::doTestStepPreambleL()"));
	CT_LbsPrivFwStepBase::doTestStepPreambleL();

	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepRamUsage::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepRamUsage::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


CT_LbsPrivFwStepRamUsage::~CT_LbsPrivFwStepRamUsage()
	{
	}


CT_LbsPrivFwStepRamUsage::CT_LbsPrivFwStepRamUsage(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_RamUsage);
	}


CT_LbsPrivFwStepRamUsage* CT_LbsPrivFwStepRamUsage::New(CT_LbsPrivFwServer& aParent)
{

CT_LbsPrivFwStepRamUsage* testStep = new CT_LbsPrivFwStepRamUsage(aParent);
// Note the lack of ELeave.
// This means that having insufficient memory will return NULL;

if (testStep)
	{
	TInt err = KErrNone;

	TRAP(err, testStep->ConstructL());
	if (err)
		{
		delete testStep;
		testStep = NULL;
		}
	}
	
return testStep;
}


void CT_LbsPrivFwStepRamUsage::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
	CreateLogDir();
	}


TVerdict CT_LbsPrivFwStepRamUsage::doTestStepL()
	{
	//Connect to Network Privacy API
	CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
	CleanupStack::PushL(netPrivacy);
	
	//Connect to Network Privacy API again
	CPrivFwApiWrapper* netPrivacy2 = CPrivFwApiWrapper::NewL(iApiUsed);
	CleanupStack::PushL(netPrivacy2);

	//Connect to Network Privacy API again
	CPrivFwApiWrapper* netPrivacy3 = CPrivFwApiWrapper::NewL(iApiUsed);
	CleanupStack::PushL(netPrivacy3);

	// Make network privacy type requestorinfo identified by name
	// iArray takes ownership of this item.
	CPosGSMPrivacyRequestInfo* reqInfo1 = CPosGSMPrivacyRequestInfo::NewL();
	CleanupStack::PushL(reqInfo1);

	TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;
    
	//Client 1
	TUint delay = 10 * 1000000; //10 secs
	reqInfo1->SetRequestorL(KTxtRequestorName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
	reqInfo1->SetLCSClientL(KTxtLcsClientName1, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
	reqInfo1->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
	//Issue a verification request - Takes ownership of CPosGSMPrivacyRequestInfo
	CleanupStack::Pop(reqInfo1);
	SubmitVerifyLocationRequest(reqInfo1, gsmInfoType, iApiObserver, netPrivacy, 
								CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
								EResponseTypeAccepted, delay); // I want this to happen...

	//Client 2
	delay = 2 * 1000000; //2 secs after the first
	CPosGSMPrivacyRequestInfo* reqInfo2 = CPosGSMPrivacyRequestInfo::NewL();
	CleanupStack::PushL(reqInfo2);
	reqInfo2->SetRequestorL(KTxtRequestorName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
	reqInfo2->SetLCSClientL(KTxtLcsClientName2, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
	reqInfo2->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
	CleanupStack::Pop(reqInfo2);
	SubmitVerifyLocationRequest(reqInfo2, gsmInfoType, iApiObserver, netPrivacy2, 
								CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
								EResponseTypeRejected, delay); // I want this to happen...		

	//Client 3
	delay = 2 * 1000000; //2 secs after the second
	CPosGSMPrivacyRequestInfo* reqInfo3 = CPosGSMPrivacyRequestInfo::NewL();
	CleanupStack::PushL(reqInfo3);
	reqInfo3->SetRequestorL(KTxtRequestorName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
	reqInfo3->SetLCSClientL(KTxtLcsClientName3, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
	reqInfo3->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
	CleanupStack::Pop(reqInfo3);
	SubmitVerifyLocationRequest(reqInfo3, gsmInfoType, iApiObserver, netPrivacy3, 
								CPosNetworkPrivacy::EDecisionNotAvailable, // If a timeout happens, do this...
								EResponseTypeAccepted, delay); // I want this to happen...

	
	
	iNotifierObserver->Start(); 
	iApiObserver->Start();

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(netPrivacy3);
	CleanupStack::PopAndDestroy(netPrivacy2);
	CleanupStack::PopAndDestroy(netPrivacy);
	
	User::After(60*1000000); // Allow LBS to close - ensures that the max_ram files are written
	
	TInt err = CalculateTotalMaxHeap();
	if (KErrNone == err)
		{
		CreateTotalMaxHeapLogFile();
		const TInt KMaxHeapLimit = 32572;
		if (iTotalMaxHeap > KMaxHeapLimit)
			{
			INFO_PRINTF3(_L("FAIL: Total Max Heap usage of %d greater than %d"), iTotalMaxHeap, KMaxHeapLimit);
			SetTestStepResult(EFail);
			}
		else
			{
			// Report the total to the log file
			INFO_PRINTF2(_L("PASS: Total Max Heap usage of %d "), iTotalMaxHeap);

			}
		}
	
	return TestStepResult();
	}


void CT_LbsPrivFwStepRamUsage::SubmitVerifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy, 
		CPosNetworkPrivacy::TRequestDecision aDecision,
		TResponseType aIntendedDecision, TUint aResponseDelay)
	{
	CT_LbsPrivFwVerifyRequest* req = CT_LbsPrivFwVerifyRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	
	// Set what the outcome should be
	req->SetDesiredResponse(aIntendedDecision);
	// Set the delay before the outcome is actioned
	req->SetResponseDelay(aResponseDelay);
	// Set what should happen if a timeout occurs
	req->SetDecision(aDecision);
	//Issue the verification request
	req->Start();
	
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}


void CT_LbsPrivFwStepRamUsage::CreateLogDir()
	{
    User::LeaveIfError(iFs.Connect());
    TEntry entry;
    TInt err =  iFs.Entry(KlogFilePath, entry);
    if (err != KErrNone)
        {
        err = iFs.MkDir(KlogFilePath);
        if (err!= KErrNone)
            {
            iFs.Close();
            User::Leave(err);
            }
        }
    iFs.Close();
	}


TInt CT_LbsPrivFwStepRamUsage::OpenFileForRead(const TDesC& aFileName)
    {
    TInt err = iFs.Connect();
    if (err != KErrNone)
        {
        return err;
        }

    err = iFile.Open(iFs,aFileName,EFileStreamText|EFileRead);

    if (err != KErrNone)
        {
        iFs.Close();
        return err;
        }

    return KErrNone;
    }


TInt CT_LbsPrivFwStepRamUsage::CalculateTotalMaxHeap()
	{
	//start with a clean sheet
	iTotalMaxHeap = 0;
	iTotalMaxChunk = 0;
	
	// increment max values by adding contents of NRH file
	TFileName readFileName = KlogFilePath();
	readFileName.Append(KNRHFile);
	TInt err = AddFileValuesToCurrentMax(readFileName);
	if (KErrNone != err)
		{
		return err;
		}

	// increment max values by adding contents of NG file
	readFileName = KlogFilePath();
	readFileName.Append(KNGFile);
	err = AddFileValuesToCurrentMax(readFileName);
	return err;
	}

TInt CT_LbsPrivFwStepRamUsage::AddFileValuesToCurrentMax(const TDesC& aFileName)
	{
	// open the given file
	TInt err = OpenFileForRead(aFileName);
	if (KErrNone == err)
		{
		// Read the line from the file
		TBuf8<KMaxFileLineSize> line;
		err = iFile.Read(line);
		if (KErrNone == err)
			{
			// locate the end of the first number string
			TInt pos = line.Locate(TChar(','));
			if (pos > 0)
				{
				// convert 1st string to a number and add to total
				TPtrC8 tmp = line.Left(pos);
				TLex8 lex(tmp);
				TInt tmpHeap;
				err = lex.Val(tmpHeap);
				if (KErrNone == err)
					{
					//add 1st string to total and repeat process for the second number string
					iTotalMaxHeap = iTotalMaxHeap + tmpHeap;
					TPtrC8 tmp2 = line.Mid(pos+2);
					lex.Assign(tmp2);
					TInt tmpChunk;
					err = lex.Val(tmpChunk);
					if (KErrNone == err)
						{
						iTotalMaxChunk = iTotalMaxChunk + tmpChunk;
						}
					else
						{
						INFO_PRINTF3(_L("FAIL: illegal file format in %S returned with error %d"), &aFileName, err);
						SetTestStepResult(EFail);					
						}
					}
				else
					{
					INFO_PRINTF3(_L("FAIL: attempt to read file %S returned with error %d"), &aFileName, err);
					SetTestStepResult(EFail);
					}				
				}
			else
				{
				INFO_PRINTF3(_L("FAIL: illegal file format in %S returned with error %d"), &aFileName, err);
				SetTestStepResult(EFail);
				}
			}
		else
			{
			INFO_PRINTF3(_L("FAIL: attempt to read file %S returned with error %d"), &aFileName, err);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		INFO_PRINTF3(_L("FAIL: attempt to open file %S returned with error %d"), &aFileName, err);
		SetTestStepResult(EFail);
		return err;
		}

	iFile.Close();
    iFs.Close();

	
	return err;
	}


void CT_LbsPrivFwStepRamUsage::CreateTotalMaxHeapLogFile()
	{
//	To be completed when time allows - useful for logging results on SMG dashboard
	}

