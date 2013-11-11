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
// @file CT_LbsClientSharedData.cpp
// This file contains the shared data class for the Lbs Client Test server
// 
//


#include "ctlbsclientshareddata.h"


/**
  Function : NewL
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsClientSharedData* CT_LbsClientSharedData::NewL()
	{
	CT_LbsClientSharedData* sharedData=new(ELeave) CT_LbsClientSharedData();
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
void CT_LbsClientSharedData::ConstructL()
	{
	}


/**
  Function : CT_LbsClientSharedData
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsClientSharedData::CT_LbsClientSharedData()
	{
	}
	
/**
  Function : ~CT_LbsClientSharedData
  Description : Destructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/	
CT_LbsClientSharedData::~CT_LbsClientSharedData()
	{
	// Clear arrays.
	for (TInt i = 0; i < iVerifyPosInfoArr.Count(); ++i)
		delete iVerifyPosInfoArr[i];
	iVerifyPosInfoArr.Close();

	for (TInt i = 0; i < iCurrentPosInfoArr.Count(); ++i)
		delete iCurrentPosInfoArr[i];
	iCurrentPosInfoArr.Close();
	}


