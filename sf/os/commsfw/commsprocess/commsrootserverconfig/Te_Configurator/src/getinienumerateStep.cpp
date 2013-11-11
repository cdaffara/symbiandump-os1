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
// Example CTestStep derived implementation
// 
//

/**
 @file loadcpmStep.cpp
*/
#include "getinienumerateStep.h"

using namespace RootServer;

//NET-CONFIGURATOR-I-0018-HP
//Getting Info about CPM in a Configurator
CGetInfoCpm::~CGetInfoCpm()
/**
 * Destructor
 */
	{
	}

CGetInfoCpm::CGetInfoCpm()
/*
 * Constructor
 */
	{

	SetTestStepName(KGetInfoCpm);

	}
/**
 * @see GetInfoCpm test case NET-CONFIGURATOR-I-0018-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator get the module inidata section. Inidata section contains
   module initialisation information in the module's configuration file.
 * close the connection to configuator
 * Expected:-GetInfoCpm return kerrNone
*/
TVerdict CGetInfoCpm::doTestStepL()
	{

	SetTestStepResult(EFail);
	_LIT8(KNameDummyCpm,"DummyCpm");

	RBuf8 data;
	data.Create(100);
	TInt actualdatasize;

	//Configurator call to get the module inidata section
	TInt error = iConfigurator.GetModuleIniData(KNameDummyCpm(), data, actualdatasize);
	if (error == KErrOverflow)
		{
		INFO_PRINTF2(_L("GetModuleIniData returned KErrOverflow (%d)  \n"), error);
		data.ReAlloc(actualdatasize);
	    error = iConfigurator.GetModuleIniData(KNameDummyCpm(), data, actualdatasize);
	  	}
	else if (error == KErrRSModuleUnknown )
		{
		INFO_PRINTF2(_L("GetModuleIniData returned KErrRSModuleUnknown (%d)  \n"), error);
		}
	else if (error == KErrNone)
		{
	    INFO_PRINTF1(_L("GetModuleIniData Sucessful"));
	    SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("GetModuleIniData  (DummyCpm) returned Error (%d)  \n"), error);
		}

    data.Close();
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0019-HP
//Getting ini data section which should be long
CGetLongIniInfoCpm::~CGetLongIniInfoCpm()
/**
 * Destructor
 */
	{
	}

CGetLongIniInfoCpm::CGetLongIniInfoCpm()
/*
 * Constructor
 */
	{
	SetTestStepName(KGetLongIniInfoCpm);
	}
/**
 * @see GetLongIniInfoCpm test case NET-CONFIGURATOR-I-0019-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator get the long module inidata section. Inidata section contains
   module initialisation information in the module's configuration file.
 * close the connection to configuator
 * Expected:-GetLongIniInfoCpm return kerrNone
 **********************************************************************************
 * Result of this test case in not important because extreme testing is not required
 * Long line of ini data is not consider for testing
 **********************************************************************************
*/
TVerdict CGetLongIniInfoCpm::doTestStepL()
	{
	SetTestStepResult(EFail);
	_LIT8(KNameDummyCpm9,"DummyCpm9");

	RBuf8 data;
	data.Create(100);
	TInt actualdatasize;

	//Configurator call to get the module inidata section
	TInt error = iConfigurator.GetModuleIniData(KNameDummyCpm9(), data, actualdatasize);
	if (error == KErrOverflow)
		{
	    INFO_PRINTF2(_L("GetModuleIniData returned KErrOverflow (%d)  \n"), error);
		}
	else if (error == KErrRSModuleUnknown )
		{
	    INFO_PRINTF2(_L("GetModuleIniData returned KErrRSModuleUnknown (%d)  \n"), error);
		}
	else if (error == KErrNone)
		{
	    INFO_PRINTF1(_L("GetModuleIniData Sucessful"));
	    SetTestStepResult(EPass);
	   	}
	else
		{
	    INFO_PRINTF2(_L("GetModuleIniData  (DummyCpm) returned Error (%d)  \n"), error);
		}
	return TestStepResult();
	}


//NET-CONFIGURATOR-I-0020-HP
//Getting ini data section which should multiple lines of ini data
CGetMultiLineIniInfoCpm::~CGetMultiLineIniInfoCpm()
/**
 * Destructor
 */
	{
	}

CGetMultiLineIniInfoCpm::CGetMultiLineIniInfoCpm()
/*
 * Constructor
 */
	{
	SetTestStepName(KGetMultiLineIniInfoCpm);
	}
/**
 * @see GetMultiLineIniInfoCpm test case NET-CONFIGURATOR-I-0020-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator get the multiple line module inidata section. Inidata section contains
   module initialisation information in the module's configuration file.
 * close the connection to configuator
 * Expected:-GetMultiLineIniInfoCpm return kerrNone
 **********************************************************************************
 * Result of this test case in not important because extreme testing is not required
 * MultiLine line ini data is not consider for testing
 **********************************************************************************
*/
TVerdict CGetMultiLineIniInfoCpm::doTestStepL()
	{
	SetTestStepResult(EFail);
	_LIT8(KNameDummyCpm10,"DummyCpm10");

	RBuf8 data;
	data.Create(100);
	TInt actualdatasize;

	//Configurator call to get the module inidata section
	TInt error = iConfigurator.GetModuleIniData(KNameDummyCpm10(), data, actualdatasize);
	if (error == KErrOverflow)
	 	{
	    INFO_PRINTF2(_L("GetModuleIniData returned KErrOverflow (%d)  \n"), error);
	    data.ReAlloc(actualdatasize);
	    error = iConfigurator.GetModuleIniData(KNameDummyCpm10(), data, actualdatasize);
	    SetTestStepResult(EFail);
		}
	else if (error == KErrRSModuleUnknown )
		{
	    INFO_PRINTF2(_L("GetModuleIniData returned KErrRSModuleUnknown (%d)  \n"), error);
		}
	else if (error == KErrNone)
		{
	    INFO_PRINTF1(_L("GetModuleIniData Sucessful"));
	    SetTestStepResult(EPass);
		}
	else
		{
	    INFO_PRINTF2(_L("GetModuleIniData  (DummyCpm10) returned Error (%d)  \n"), error);
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0021-HP
//Getting Info about CPM in a Configurator without CMI files
CGetInfoCpmWithoutCMI::~CGetInfoCpmWithoutCMI()
/**
 * Destructor
 */
	{
	}


CGetInfoCpmWithoutCMI::CGetInfoCpmWithoutCMI()
/**
 * Constructor
 */
	{
	SetTestStepName(KGetInfoCpmWithoutCMI);
	}

/**
 * @see GetInfoCpmWithoutCMI test case NET-CONFIGURATOR-I-0021-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator get the module inidata section. which has no CMI files
 * close the connection to configuator
 * Expected:-GetInfoCpmWithoutCMI return kerrNone
*/
TVerdict CGetInfoCpmWithoutCMI::doTestStepL()
	{

	SetTestStepResult(EFail);
	_LIT8(KNameDummyCpm,"DummyNoCMI");

	RBuf8 data;
	data.Create(100);
	TInt actualdatasize;

	//Configurator call to get the module inidata section
	TInt error = iConfigurator.GetModuleIniData(KNameDummyCpm(), data, actualdatasize);
	if (error == KErrOverflow)
		{
	   	INFO_PRINTF2(_L("GetModuleIniData returned KErrOverflow (%d)  \n"), error);
	   	data.ReAlloc(actualdatasize);
	   	error = iConfigurator.GetModuleIniData(KNameDummyCpm(), data, actualdatasize);
	   	SetTestStepResult(EFail);
		}
	else if (error == KErrRSModuleUnknown)
		{
	   	INFO_PRINTF2(_L("GetModuleIniData returned KErrRSModuleUnknown (%d)  \n"), error);
	   	SetTestStepResult(EPass);
		}
	else if (error == KErrNone)
		{
	    INFO_PRINTF1(_L("GetModuleIniData Sucessful"));
		}
	else
		{
	    INFO_PRINTF2(_L("GetModuleIniData  (DummyCpm) returned Error (%d)  \n"), error);
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0022-HP
//Getting Info about CPM with cmi which does not have iniData section in CMI file
CGetInfoCpmWithoutINI::~CGetInfoCpmWithoutINI()
/**
 * Destructor
 */
	{
	}

CGetInfoCpmWithoutINI::CGetInfoCpmWithoutINI()
/**
 * Constructor
 */
	{

	SetTestStepName(KGetInfCpmWithoutINI);

	}
/**
 * @see GetInfoCpmWithoutINI test case NET-CONFIGURATOR-I-0022-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator get the module inidata section. which has no INI section
 * close the connection to configuator
 * Expected:-GetInfoCpmWithoutCMI return kerrNone
*/
TVerdict CGetInfoCpmWithoutINI::doTestStepL()
	{
	SetTestStepResult(EFail);

	//Connection to the configurator
    _LIT8(KNameDummyCpm11,"DummyCpm11");

	RBuf8 data;
    data.Create(100);
    TInt actualdatasize;

	//Configurator call to get the module inidata section
    TInt error = iConfigurator.GetModuleIniData(KNameDummyCpm11(), data, actualdatasize);
    if ( error == KErrOverflow)
		{
		INFO_PRINTF2(_L("GetModileIniData returned KErrOverflow (%d)  \n"), error);
		data.ReAlloc(actualdatasize);
	    error = iConfigurator.GetModuleIniData(KNameDummyCpm11(), data, actualdatasize);
	    }
	else if ( error == KErrRSModuleUnknown)
		{
		INFO_PRINTF2(_L("GetModuleIniData returned KErrRSModuleUnknown (%d)  \n"), error);
		}
	else if (error == KErrNone)
		{
		INFO_PRINTF1(_L("GetModuleIniData Sucessfully"));
		SetTestStepResult(EPass);
		}
	else
		{
		INFO_PRINTF2(_L("GetModuleIniData  (DummyCpm11) returned Error (%d)  \n"), error);
		}
    return TestStepResult();
	}

// NET-CONFIGURATOR-I-0023-HP
//Getting Info about CPM with cmi iniData overflow
CGetInfoCpmIniOverflow::~CGetInfoCpmIniOverflow()
/**
 * Destructor
 */
	{
	}

CGetInfoCpmIniOverflow::CGetInfoCpmIniOverflow()
/**
 * Constructor
 */
	{

	SetTestStepName(KGetInfoCpmIniOverflow);

	}
/**
 * @see GetInfoCpmIniOverflow test case NET-CONFIGURATOR-I-0023-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator get the module inidata section
 * close the connection to configuator
 * Expected:-GetInfoCpmIniOverflow return kerrOverflow
*/
TVerdict CGetInfoCpmIniOverflow::doTestStepL()
	{
	SetTestStepResult(EFail);
	_LIT8(KNameDummyCpm10,"DummyCpm10");

	TBuf8<10> data;
	TInt actualdatasize;

	//Configurator call to get the module inidata section
    TInt error = iConfigurator.GetModuleIniData(KNameDummyCpm10(), data, actualdatasize);
    if (error == KErrOverflow)
		{
	    INFO_PRINTF2(_L("GetModileIniData returned KErrOverflow (%d)  \n"), error);
        SetTestStepResult(EPass);
     	}
	else if (error == KErrRSModuleUnknown)
		{
	    INFO_PRINTF2(_L("GetModuleIniData returned KErrRSModuleUnknown (%d)  \n"), error);
		}
	else if (error == KErrNone)
	   	{
	   	INFO_PRINTF1(_L("GetModuleIniData Sucessful"));
	   	}
	else
		{
		INFO_PRINTF2(_L("GetModuleIniData  (DummyCpm) returned Error (%d)  \n"), error);
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0024-HP
//Request for The list of modules in The configurator
CListModules::~CListModules()
/**
 * Destructor
 */
	{
	}

CListModules::CListModules()
/**
 * Constructor
 */
	{
	SetTestStepName(KListModules);
	}
/**
 * @see ListModules test case NET-CONFIGURATOR-I-0024-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Enables the client to assemble a list of modules for matching group
 * Configurator close the connection to configuator
 * Expected:-ListModules return all the modules
*/
TVerdict CListModules::doTestStepL()
	{
	SetTestStepResult(EFail);
	TRSIter position;

	_LIT8(KGroupName, "Group1");
	_LIT8(KDummyCpm,"DummyCpm");
	_LIT8(KDummyCpm2,"DummyCpm2");
	_LIT8(KDummyCpm3,"DummyCpm3");
		
	TCFModuleName moduleName;
	TCFGroupName groupName(KGroupName);
	TCFGroupName cpmName[3];

	cpmName[0].Copy(KDummyCpm);
	cpmName[1].Copy(KDummyCpm2);
	cpmName[2].Copy(KDummyCpm3);

    TInt dummyCpm  = 0;
    TInt dummyCpm2 = 0;
    TInt dummyCpm3 = 0;
    TInt cpmCount  = 0;

	//Enables the client to assemble a list of modules for matching group by retrieving the
	// name of one module at a time from all modules
	while(KErrNone == iConfigurator.EnumerateModules(groupName, position , moduleName))
		{
		if(cpmName[0].CompareF(moduleName)==0)
			{
			if(dummyCpm == EModuleNotFoundEarlier)
				{
				cpmCount++;
				dummyCpm = EModuleFoundNow;
				}
			}
		else if(cpmName[1].CompareF(moduleName)==0)
			{
			if(dummyCpm2 == EModuleNotFoundEarlier)
				{
				cpmCount++;
				dummyCpm2 = EModuleFoundNow;
				}
			}
		else if(cpmName[2].CompareF(moduleName)==0)
			{
			if(dummyCpm3 == EModuleNotFoundEarlier)
				{
				cpmCount++;
				dummyCpm3 = EModuleFoundNow;
				}
			}
		}

	if(cpmCount == 3)
		{
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0025-HP
//Enumerate one group and then enumerate another group

CEnumerateGroup::~CEnumerateGroup()
/**
 * Destructor
 */
	{
	}

CEnumerateGroup::CEnumerateGroup()
/**
 * Constructor
 */
	{
	SetTestStepName(KEnumerateGroup);
	}
/**
 * @see EnumerateGroup test case NET-CONFIGURATOR-I-0025-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Enables the client to assemble a list of modules for matching groups
 * Configurator close the connection to configuator
 * Expected:-EnumerateGroup return list of mosules
*/
TVerdict CEnumerateGroup::doTestStepL()
	{
	SetTestStepResult(EFail);
	TRSIter position;
    TCFModuleName moduleName;
   	_LIT8(KGroupName1, "Group1");

    TCFGroupName groupName1(KGroupName1);
	TCFGroupName cpmName[3];


	_LIT8(KDummyCpm,"DummyCpm");
	_LIT8(KDummyCpm2,"DummyCpm2");
	_LIT8(KDummyCpm3,"DummyCpm3");

	cpmName[0].Copy(KDummyCpm);
	cpmName[1].Copy(KDummyCpm2);
    cpmName[2].Copy(KDummyCpm3);


	TInt dummyCpm  = 0;
    TInt dummyCpm2 = 0;
    TInt dummyCpm3 = 0;

    TInt cpmCount1 = 0;
  	TInt cpmCount2 = 0;
	//Enables the client to assemble a list of modules for matching one group by retrieving the
	//name of one module at a time from all modules
	while(KErrNone == iConfigurator.EnumerateModules(groupName1, position , moduleName))
		{
		if(cpmName[0].CompareF(moduleName)==0)
			{
			if(dummyCpm == EModuleNotFoundEarlier)
				{
				cpmCount1++;
				dummyCpm = EModuleFoundNow;
				}
			}
		else if(cpmName[1].CompareF(moduleName)==0)
			{
			if(dummyCpm2 == EModuleNotFoundEarlier)
				{
				cpmCount1++;
				dummyCpm2 = EModuleFoundNow;
				}
			}
		else if(cpmName[2].CompareF(moduleName)==0)
			{
			if(dummyCpm3 == EModuleNotFoundEarlier)
				{
				cpmCount1++;
				dummyCpm3 = EModuleFoundNow;
				}
			}
		}

	_LIT8(KGroupName2, "Group2");
	TCFGroupName groupName2(KGroupName2);

	_LIT8(KDummyCpm4,"DummyCpm4");
	_LIT8(KDummyCpm5,"DummyCpm5");

	cpmName[0].Copy(KDummyCpm4);
	cpmName[1].Copy(KDummyCpm5);

	dummyCpm  = 0;
	dummyCpm2 = 0;

   	// Enables the client to assemble a list of modules for matching another group by retrieving the
 	// name of one module at a time from all modules

   	while(KErrNone == iConfigurator.EnumerateModules(groupName2, position , moduleName))
   		{

   		if(cpmName[0].CompareF(moduleName)==0)
			{
			if(dummyCpm == EModuleNotFoundEarlier)
				{
				cpmCount2++;
				dummyCpm = EModuleFoundNow;
				}
			}
		else
		if(cpmName[1].CompareF(moduleName)==0)
			{
			if(dummyCpm2 == EModuleNotFoundEarlier)
				{
				cpmCount2++;
				dummyCpm2 = EModuleFoundNow;
				}
			}
   		}

	if(cpmCount1 == 3 && cpmCount2 == 2)
	   {
	   SetTestStepResult(EPass);
	   }
	return TestStepResult();
	}

//NET-CONFIGURATOR-I-0026-HP
//Enumerate cpm with two cmi file which has same CPM name
CEnumeratCpmwithTwoCmi::~CEnumeratCpmwithTwoCmi()
/**
 * Destructor
 */
	{
	}

CEnumeratCpmwithTwoCmi::CEnumeratCpmwithTwoCmi()
/**
 * Constructor
 */
	{

	SetTestStepName(KEnumeratCpmwithTwoCmi);
	}
/**
 * @see CnumeratCpmwithTwoCmi test case NET-CONFIGURATOR-I-0026-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Enables the client to assemble a list of modules for matching groups
 * Configurator close the connection to configuator
 * Expected:-EnumeratCpmwithTwoCmi return list of modules
*/
TVerdict CEnumeratCpmwithTwoCmi::doTestStepL()
	{

	SetTestStepResult(EFail);
	TRequestStatus status;
	RRsConfigurator configurator;
	//Connection to the configurator
	TInt result = configurator.Connect();
	if (result == KErrNone)
		{
		INFO_PRINTF1(_L("Configurator Connection Sucessful"));
		TRSIter position;
	    TCFModuleName moduleName;
	   	_LIT8(KGroupName, "Group5");

	    TCFGroupName groupName(KGroupName);
		TCFGroupName cpmName[2];

		_LIT8(KDummyCpm12,"DummyCpms");
		_LIT8(KDummyCpm13,"DummyCpms");
		cpmName[0].Copy(KDummyCpm12);
		cpmName[1].Copy(KDummyCpm13);

		TInt dummyCpm12 = 0;
		TInt dummyCpm13 = 0;
		TInt moduleCount = 0;

		//Enables the client to assemble a list of modules for matching group by retrieving the
		//name of one module at a time from all modules
		while(KErrNone == configurator.EnumerateModules(groupName, position , moduleName))
			{
			if(cpmName[0].CompareF(moduleName)==0)
				{
				if(dummyCpm12 == EModuleNotFoundEarlier)
					{
					moduleCount++;
					dummyCpm12 = EModuleFoundNow;
					}
				}
			else
			if(cpmName[1].CompareF(moduleName) == 0)
				{
				if(dummyCpm13 == EModuleNotFoundEarlier)
					{
					moduleCount++;
					dummyCpm13 = EModuleFoundNow;
					}
				}
			}

			if(moduleCount == 1)
				{
				SetTestStepResult(EPass);
				}
		//Close  to the configurator connection
		configurator.Close();
		}
	else
		{
		INFO_PRINTF1(_L("Configurator Connection Failure \n"));
		}
	return TestStepResult();
	}


//NET-CONFIGURATOR-I-0027-HP
//Enumerate cpm in a group which has only one CMI file
CEnumeratCpmwithOneCmi::~CEnumeratCpmwithOneCmi()
/**
 * Destructor
 */
	{
	}

CEnumeratCpmwithOneCmi::CEnumeratCpmwithOneCmi()
/**
 * Constructor
 */
	{

	SetTestStepName(KEnumeratCpmwithOneCmi);
	}
/**
 * @see EnumeratCpmwithOneCmi test case NET-CONFIGURATOR-I-0027-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Enables the client to assemble a list of modules for matching groups which is having one cmi file
 * Configurator close the connection to configuator
 * Expected:-EnumeratCpmwithOneCmi return list of modules
*/
TVerdict CEnumeratCpmwithOneCmi::doTestStepL()
	{

	SetTestStepResult(EFail);
	TRequestStatus status;
	RRsConfigurator configurator;

    //Connection to the configurator
	TInt result = configurator.Connect();
	if (result == KErrNone)
		{

		INFO_PRINTF1(_L("Configurator Connection Sucessful"));
		TRSIter position;
	    TCFModuleName moduleName;
	   	_LIT8(KGroupName, "Group6");

	    TCFGroupName groupName(KGroupName);

		_LIT8(KDummyCpm14,"DummyCpm14");

		TCFGroupName aGroupName;
		aGroupName.Copy(KDummyCpm14);
		TInt dummyCpm14 = 0;
		TInt moduleCount = 0;
		//Enables the client to assemble a list of modules for matching group by retrieving the
		//name of one module at a time from all modules
		while(KErrNone == configurator.EnumerateModules(groupName, position , moduleName))
			{

			if(aGroupName.CompareF(moduleName)==0)
				{
				if(dummyCpm14 == EModuleNotFoundEarlier)
					{
					moduleCount++;
					dummyCpm14 = EModuleFoundNow;
					}
				}

			 }
		if(moduleCount == 1)
			{
			SetTestStepResult(EPass);
			}
	//Close  to the configurator connection
	configurator.Close();
		}
	else
		{
		INFO_PRINTF1(_L("Configurator Connection Failure \n"));
		}
	return TestStepResult();
	}


//NET-CONFIGURATOR-I-0028-HP
//Request for The list of modules in The configurator unknown group name
CListModulesGroupUnknown::~CListModulesGroupUnknown()
/**
 * Destructor
 */
	{
	}

CListModulesGroupUnknown::CListModulesGroupUnknown()
/**
 * Constructor
 */
	{
	SetTestStepName(KListModulesGroupUnknown);
	}
/**
 * @see CListModulesGroupUnknown test case NET-CONFIGURATOR-I-0028-HP
 *
 * doTestStepL virtual function does the below action
 * Connect to a configurator
 * Configurator Enables the client to assemble a list of modules for matching groups which is having one cmi file
 * Configurator close the connection to configuator
 * Expected:-CListModulesGroupUnknown return KErrRSModuleUnknown
*/
TVerdict CListModulesGroupUnknown::doTestStepL()
	{
	SetTestStepResult(EFail);
	RRsConfigurator configurator;
	//Connection to the configurator
	TInt err = configurator.Connect();
	if (err == KErrNone)
		{
		INFO_PRINTF1(_L("Configurator Connection Sucessful"));

		TRSIter position;
		TCFModuleName moduleName;
		_LIT8(KGroupName, "UnknownGrp");
		TCFGroupName groupName(KGroupName);
		//Enables the client to assemble a list of modules for matching group by retrieving the
		//name of one module at a time from all modules
		TInt istatus = configurator.EnumerateModules(groupName, position , moduleName);
		if(istatus == KErrNone)
			{
			INFO_PRINTF1(_L("EnumerateModules Sucessful \n"));
			}
		else
		if (istatus == KErrRSModuleUnknown )
			{
			INFO_PRINTF2(_L("GetModileIniData returned KErrRSModuleUnknown (%d)  \n"), istatus);
			SetTestStepResult(EPass);
		   	}
		else
			{
			INFO_PRINTF2(_L("GetModileIniData returned  (%d)  \n"), istatus);
			}
		//Close  to the configurator connection
		configurator.Close();
		}
	else
		{
		INFO_PRINTF1(_L("Configurator Connection Failure \n"));
		}
   return TestStepResult();
   }



