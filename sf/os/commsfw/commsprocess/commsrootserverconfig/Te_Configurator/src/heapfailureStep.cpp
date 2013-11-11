// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file loadcpmStep.h
*/


#include "heapfailureStep.h"

using namespace RootServer;


//NET-iConfigurator-I-0029-HP
//Heap Allocation failure test for load and unload API
CHeapAllocationFailure ::~CHeapAllocationFailure ()
/**
 * Destructor
 */
	{
	}

CHeapAllocationFailure ::CHeapAllocationFailure ()
/**
 * Constructor
 */
	{
	SetTestStepName(KHeapAllocationFailure );
	}


/**
 * @see HeapAllocationFailure test case NET-iConfigurator-I-00029-HP
 *
 * doTestStepL virtual function does the below action
 * it test the heap failure test for the fuction called
 * in the trap function
 * Expected:-HeapAllocationFailure return EPass
*/
TVerdict CHeapAllocationFailure ::doTestStepL()
	{
	SetTestStepResult(EFail);
	TInt error = KErrNone;
	TInt failRate = 0;

	do
	   {
	   // The OOM loop
		++failRate;
		//set the heap failure test to fail
		__UHEAP_SETFAIL(RHeap::EDeterministic, failRate);
		//Mark the biging of the heap to test for the heap failure test
		__UHEAP_MARK;
		// Do the tests
		  TRAP(error, CallOOM());
		//Mark the end of the heap to test for the heap failure test
		__UHEAP_MARKEND;
		//Reset the heap mark
		__UHEAP_RESET;
		if(error != KErrNone && error != KErrNoMemory)
			{
			INFO_PRINTF3(_L("Unexpected error occured in %S: %D"), &TestStepName(), error);
			SetTestStepResult(EFail);
			break;
			}
		else
			{
			SetTestStepResult(EPass);
			}
		}while(error != KErrNone);

	return TestStepResult();
   }


//CallOOM function use the LoadCpm and UnLoadCpm API to test for heap failure test
void CHeapAllocationFailure::CallOOM()
	{

	INFO_PRINTF1(_L("OOM load started \n"));
	_LIT8(KNameDummyCpm,"DummyCpm");

	//iConfigurator Load the DummyCpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);
	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Heap failure test DummyCpm Loaded Successfully \n"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("Heap failure test LoadCpm (DummyCpm) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
  	else
  		{
		INFO_PRINTF2(_L("Heap failure test LoadCpm (DummyCpm) returned Error (%d)  \n"), iStatus.Int());
		}
	
	INFO_PRINTF1(_L("OOM load end \n"));
   	//iConfigurator to unload the DummyCpm CPM
	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);
	User::WaitForRequest(iStatus);

	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("Heap failure test DummyCpm UnLoaded Successfully \n"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("Heap failure test UnLoadCpm (DummyCpm) returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
  	else
  		{
		INFO_PRINTF2(_L("Heap failure test UnLoadCpm (DummyCpm) returned Error (%d)  \n"), iStatus.Int());
		}
	}


//NET-iConfigurator-I-0030-HP
//Heap Allocation failure test for load unload and cancel load API
CHeapFailurecancelLoad ::~CHeapFailurecancelLoad ()
/**
 * Destructor
 */
	{
	}
CHeapFailurecancelLoad ::CHeapFailurecancelLoad ()
/**
 * Constructor
 */
	{
	SetTestStepName(KHeapFailurecancelLoad );
	}

/**
 * @see HeapFailurecancelLoad test case NET-iConfigurator-I-00029-HP
 *
 * doTestStepL virtual function does the below action
 * it test the heap failure test for the fuction called
 * in the trap function
 * Expected:-HeapFailurecancelLoad return EPass
*/
TVerdict CHeapFailurecancelLoad ::doTestStepL()
	{
	SetTestStepResult(EFail);
	TInt error = KErrNone;
	TInt failRate = 0;

	do
		{
	   	// The OOM loop
		++failRate;
		//set the heap failure test to fail
		__UHEAP_SETFAIL(RHeap::EDeterministic, failRate);
		//Mark the biging of the heap to test for the heap failure test
		__UHEAP_MARK;
		// Do the tests
	   	TRAP(error, CancelLoad());
	   	//Mark the end of the heap to test for the heap failure test
	   	__UHEAP_MARKEND;
	   	//Reset the heap mark
	   	__UHEAP_RESET;

	  	if(error != KErrNone && error != KErrNoMemory)
			{
			INFO_PRINTF3(_L("Unexpected error occured in %S: %D"), &TestStepName(), error);
			SetTestStepResult(EFail);
			break;
			}
	  	else
			{
			SetTestStepResult(EPass);
			}
		}while(error != KErrNone);
   	return TestStepResult();
   	}


//CancelLoad function use the LoadCpm,UnLoadCpm,cancelLoadCpm API to test for heap failure test
void CHeapFailurecancelLoad::CancelLoad()
	{
	_LIT8(KNameDummyCpm,"DummyCpm");

	//iConfigurator Load the DummyCpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm Loaded Successfully"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm returned Error (%d)  \n"), iStatus.Int());
		}
	
	//iConfigurator to call Cancel Load request preveously called
	iConfigurator.CancelLoadCpm(KNameDummyCpm());
	User::WaitForRequest(iStatus);
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);
	//iConfigurator Load the DummyCpm CPM
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("CancelLoadCpm Loaded Successfully"));
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist)
		{
		INFO_PRINTF1(_L("CancelLoading DummyCpm Failed\n"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("CancelLoading DummyCpm returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("CancelLoading DummyCpm Unknown status (%d)  \n"), iStatus.Int());
		}

	//iConfigurator unLoad the DummyCpm CPM
	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);
	User::WaitForRequest(iStatus);
}

//NET-iConfigurator-I-0031-HP
//Heap Allocation failure test for load unload  and  cancel unload API
CHeapFailurecancelUnLoad ::~CHeapFailurecancelUnLoad ()
/**
 * Destructor
 */
	{
	}

CHeapFailurecancelUnLoad ::CHeapFailurecancelUnLoad ()
/**
 * Constructor
 */
	{
	SetTestStepName(KHeapFailurecancelUnLoad );
	}

/**
 * HeapFailurecancelUnLoad test case NET-iConfigurator-I-0031-HP
 *
 * doTestStepL virtual function does the below action
 * it test the heap failure test for the fuction called
 * in the trap function
 * Expected:-HeapFailurecancelUnLoad return EPass
*/
TVerdict CHeapFailurecancelUnLoad ::doTestStepL()
	{
	SetTestStepResult(EFail);
	TInt error = KErrNone;
	TInt failRate = 0;

	do
	   {
	   // The OOM loop
		++failRate;
		//set the heap failure test to fail
		__UHEAP_SETFAIL(RHeap::EDeterministic, failRate);
		//Mark the biging of the heap to test for the heap failure test
		__UHEAP_MARK;
		// Do the tests
	    TRAP(error, CancelUnLoad());
		//Mark the end of the heap to test for the heap failure test
		__UHEAP_MARKEND;
		//Reset the heap mark
		__UHEAP_RESET;

		if(error != KErrNone && error != KErrNoMemory)
			{
			INFO_PRINTF3(_L("Unexpected error occured in %S: %D"), &TestStepName(), error);
			SetTestStepResult(EFail);
			break;
			}
		else
			{
			SetTestStepResult(EPass);
			}
		}while(error != KErrNone);

	return TestStepResult();
    }

//CancelUnLoad function use the LoadCpm,UnLoadCpm,CancelUnLoadCpm API to test for heap failure test
void CHeapFailurecancelUnLoad::CancelUnLoad()
	{
	_LIT8(KNameDummyCpm,"DummyCpm");

    //iConfigurator Load the DummyCpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm Loaded Successfully"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm returned Error (%d)  \n"), iStatus.Int());
		}

	//iConfigurator to unload the DummyCpm CPM
	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);
	//iConfigurator to cancel unload the DummyCpm CPM
	iConfigurator.CancelUnloadCpm(KNameDummyCpm());
	User::WaitForRequest(iStatus);
	//iConfigurator Load the DummyCpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("CancelLoadCpm Loaded Successfully"));
		SetTestStepResult(EFail);
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist)
		{
		INFO_PRINTF1(_L("CancelLoading DummyCpm Failed\n"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("CancelLoading DummyCpm returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("CancelLoading DummyCpm Unknown status (%d)  \n"), iStatus.Int());
		}
	
	//iConfigurator to unload the DummyCpm CPM
	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);
	//Waits for a specific asynchronous request to complete.
	User::WaitForRequest(iStatus);
	}

//NET-iConfigurator-I-0032-HP
//Heap Allocation failure test for load unload  and  GetModuleIniData
CHeapFailureconfigApi ::~CHeapFailureconfigApi()
/**
 * Destructor
 */
	{
	}

CHeapFailureconfigApi::CHeapFailureconfigApi ()
/**
 * Constructor
 */
	{
	SetTestStepName(KHeapFailureconfigApi );
	}

TVerdict CHeapFailureconfigApi ::doTestStepL()
	{
	SetTestStepResult(EFail);
	TInt error = KErrNone;
	TInt failRate = 0;
	do
	   {
	   // The OOM loop
		++failRate;
		//set the heap failure test to fail
		//__UHEAP_SETFAIL(RHeap::EDeterministic, failRate);
		//Mark the biging of the heap to test for the heap failure test
		__UHEAP_MARK;
		// Do the tests
	    TRAP(error, ConfiguratorApis());
		//Mark the end of the heap to test for the heap failure test
		__UHEAP_MARKEND;
		//Reset the heap mark
		__UHEAP_RESET;

		if(error != KErrNone && error != KErrNoMemory)
			{
			INFO_PRINTF3(_L("Unexpected error occured in %S: %D"), &TestStepName(), error);
			SetTestStepResult(EFail);
			break;
			}
		else
			{
			SetTestStepResult(EPass);
			}
		}while(error != KErrNone);

   return TestStepResult();

   }

//iConfiguratorApis function use the LoadCpm,UnLoadCpm,GetModuleIniData,EnumerateModules API to test for heap failure test
void CHeapFailureconfigApi::ConfiguratorApis()
	{
	_LIT8(KNameDummyCpm,"DummyCpm");

   	//iConfigurator Load the DummyCpm CPM
	iConfigurator.LoadCpm(KNameDummyCpm(), iStatus);
	User::WaitForRequest(iStatus);

	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("DummyCpm Loaded Successfully"));
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("LoadCpm returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("LoadCpm returned Error (%d)  \n"), iStatus.Int());
		}
	
	//iConfigurator unLoad the DummyCpm
	iConfigurator.UnloadCpm(KNameDummyCpm(), EImmediate, iStatus);
	User::WaitForRequest(iStatus);
	if(iStatus.Int() == KErrNone)
		{
		INFO_PRINTF1(_L("UnLoadCpm Loaded Successfully"));
		}
	else if(iStatus.Int() == KErrRSModuleAlreadyExist)
		{
		INFO_PRINTF2(_L("UnLoadCpm DummyCpm Unknown status (%d)  \n"), iStatus.Int());;
		}
	else if(iStatus.Int() == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("UnLoadCpm DummyCpm returned KErrRSModuleUnknown(%d)  \n"), iStatus.Int());
		}
	else
		{
		INFO_PRINTF2(_L("UnLoadCpm DummyCpm Unknown status (%d)  \n"), iStatus.Int());
		}

	RBuf8 data;
	data.Create(100);
	TInt actualdatasize;
	//iConfigurator call to get the module inidata section
	TInt error = iConfigurator.GetModuleIniData(KNameDummyCpm(), data, actualdatasize);
	if (error == KErrOverflow)
		{
		INFO_PRINTF2(_L("GetModuleIniData returned KErrOverflow (%d)  \n"), error);
		data.ReAlloc(actualdatasize);
	    error = iConfigurator.GetModuleIniData(KNameDummyCpm(), data, actualdatasize);
	    SetTestStepResult(EFail);
		}
	else if (error == KErrRSModuleUnknown )
		{
		INFO_PRINTF2(_L("GetModuleIniData returned KErrRSModuleUnknown (%d)  \n"), error);
		SetTestStepResult(EFail);
		}
	else if (error == KErrNone)
		{
	    INFO_PRINTF1(_L("GetModuleIniData Sucessful"));
		}
	else
		{
	    INFO_PRINTF2(_L("GetModuleIniData  (DummyCpm) returned Error (%d)  \n"), error);
	    }

    data.Close();
	TRSIter position;
	TCFModuleName moduleName;
	_LIT8(KGroupName, "Group1");
	TCFGroupName groupName(KGroupName);
	//Enables the client to assemble a list of modules for matching one group by retrieving the
	// name of one module at a time from all modules
    while(KErrNone == iConfigurator.EnumerateModules(groupName, position , moduleName))
		{
		INFO_PRINTF1(_L("EnumerateModules Sucessful\n"));
		}
	}

//NET-iConfigurator-I-0033-HP
//Heap Allocation failure test for GetIni API
CHeapFailurecancelGetIni ::CHeapFailurecancelGetIni ()
/**
 * Destructor
 */
	{
	}

CHeapFailurecancelGetIni ::~CHeapFailurecancelGetIni ()
/**
 * Constructor
 */
	{
	SetTestStepName(KHeapFailurecancelGetIni );
	}

TVerdict CHeapFailurecancelGetIni ::doTestStepL()
	{

	SetTestStepResult(EFail);
	TInt error = KErrNone;
	TInt failRate = 0;

	do
	   {
	   // The OOM loop
		++failRate;
		//set the heap failure test to fail
		//__UHEAP_SETFAIL(RHeap::EDeterministic, failRate);
		//Mark the biging of the heap to test for the heap failure test
		__UHEAP_MARK;
		// Do the tests
	    TRAP(error, CancelGetIni());
		//Mark the end of the heap to test for the heap failure test
		__UHEAP_MARKEND;
		//Reset the heap mark
		__UHEAP_RESET;
		if(error != KErrNone && error != KErrNoMemory)
			{
			INFO_PRINTF3(_L("Unexpected error occured in %S: %D"), &TestStepName(), error);
			SetTestStepResult(EFail);
			break;
			}
			else
			{
			SetTestStepResult(EPass);
			}
		}while(error != KErrNone);

   return TestStepResult();
   }

//CancelGetIni function use the LoadCpm,UnLoadCpm,GetModuleIniData,EnumerateModules API to test for heap failure test
void CHeapFailurecancelGetIni::CancelGetIni()
	{

	TCFModuleName DummyCpm;
	_LIT8(KNameDummyCpm,"DummyCpm");
	DummyCpm.Copy(KNameDummyCpm);
	RBuf8 data;
	data.Create(100);
	TInt actualdatasize;

	TInt error = iConfigurator.GetModuleIniData(DummyCpm, data, actualdatasize);
	if (error == KErrOverflow)
		{
		INFO_PRINTF2(_L("GetModuleIniData returned KErrOverflow (%d)  \n"), error);
		data.ReAlloc(actualdatasize);
	    error = iConfigurator.GetModuleIniData(DummyCpm, data, actualdatasize);
	    }
	else
	if (error == KErrRSModuleUnknown )
		{
		INFO_PRINTF2(_L("GetModuleIniData returned KErrRSModuleUnknown (%d)  \n"), error);
		}
	else
	if (error == KErrNone)
		{
	    INFO_PRINTF1(_L("GetModuleIniData Sucessful"));
		}
	else
		{
	    INFO_PRINTF2(_L("GetModuleIniData  (DummyCpm) returned Error (%d)  \n"), error);
	    }
	data.Close();
    }

//NET-iConfigurator-I-0034-HP
//Heap Allocation failure test for GetIni API
CHeapFailureEnumerateGrp ::CHeapFailureEnumerateGrp ()
/**
 * Destructor
 */
	{
	}

CHeapFailureEnumerateGrp ::~CHeapFailureEnumerateGrp ()
/**
 * Constructor
 */
	{
	SetTestStepName(KHeapFailureEnumerateGrp );
	}

TVerdict CHeapFailureEnumerateGrp ::doTestStepL()
	{

	SetTestStepResult(EFail);
	TInt error = KErrNone;
	TInt failRate = 0;

	do
	   {
	   // The OOM loop
		++failRate;
		//set the heap failure test to fail
		//__UHEAP_SETFAIL(RHeap::EDeterministic, failRate);
		//Mark the biging of the heap to test for the heap failure test
		__UHEAP_MARK;
		// Do the tests
	   TRAP(error, EnumerateGrp());
	   //Mark the end of the heap to test for the heap failure test
	   __UHEAP_MARKEND;
	   //Reset the heap mark
	   __UHEAP_RESET;

	   if(error != KErrNone && error != KErrNoMemory)
			{
			INFO_PRINTF3(_L("Unexpected error occured in %S: %D"), &TestStepName(), error);
			SetTestStepResult(EFail);
			break;
			}
		else
			{
			SetTestStepResult(EPass);
			}
		}while(error != KErrNone);

	return TestStepResult();
   }


void CHeapFailureEnumerateGrp::EnumerateGrp()
	{
	TRSIter position;
	TCFModuleName moduleName;
	_LIT8(KGroupName, "Group1");

	TCFGroupName groupName(KGroupName);
	//Enables the client to assemble a list of modules for matching group.
	while(KErrNone == iConfigurator.EnumerateModules(groupName, position , moduleName))
		{
		INFO_PRINTF1(_L("EnumerateModules Sucessful\n"));
		}

    }



