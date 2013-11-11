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
// @file CT_LbsSharedData.cpp
// This file contains the shared data class for the Lbs  Test server
// 
//


#include "ctlbsshareddata.h"

/**
  Function : NewL
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsSharedData* CT_LbsSharedData::NewL()
	{
	CT_LbsSharedData* sharedData=new(ELeave) CT_LbsSharedData();
	CleanupStack::PushL(sharedData);
	sharedData->ConstructL();
	CleanupStack::Pop(sharedData);

	return sharedData;
	}


/**
  Function : ConstructL
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
void CT_LbsSharedData::ConstructL()
	{
	}


/**
  Function : CT_LbsSharedData
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsSharedData::CT_LbsSharedData()
	{
	iTestModuleInUse = ETrue; // Default is to use the test module.
	}
	
/**
  Function : ~CT_LbsSharedData
  Description : Destructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/	
CT_LbsSharedData::~CT_LbsSharedData()
	{
	// Clear arrays.
	for (TInt i = 0; i < iVerifyPosInfoArr.Count(); ++i)
		delete iVerifyPosInfoArr[i];
	iVerifyPosInfoArr.Close();

	for (TInt i = 0; i < iCurrentPosInfoArr.Count(); ++i)
		delete iCurrentPosInfoArr[i];
	iCurrentPosInfoArr.Close();
		
	//Clear logging arays
	for (TInt i = 0; i < iSelfLocateLogInfoArr.Count(); ++i)
		delete iSelfLocateLogInfoArr[i];	
	iSelfLocateLogInfoArr.Close();
	
	for (TInt i = 0; i < iNetworkLocateLogInfoArr.Count(); ++i)
		delete iNetworkLocateLogInfoArr[i];	
	iNetworkLocateLogInfoArr.Close();
	
	for (TInt i = 0; i < iExternalLocateLogInfoArr.Count(); ++i)
		delete iExternalLocateLogInfoArr[i];	
	iExternalLocateLogInfoArr.Close();
	
	for (TInt i = 0; i < iX3PLocateLogInfoArr.Count(); ++i)
		delete iX3PLocateLogInfoArr[i];	
	iX3PLocateLogInfoArr.Close();
	
	for (TInt i = 0; i < iAssistanceDataLogInfoArr.Count(); ++i)
		delete iAssistanceDataLogInfoArr[i];	
	iAssistanceDataLogInfoArr.Close();
	
	}


