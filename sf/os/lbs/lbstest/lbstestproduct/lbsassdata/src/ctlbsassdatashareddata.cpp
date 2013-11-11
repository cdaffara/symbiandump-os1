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
// @file ctlbsassdatashareddata.cpp
// This file contains the shared data class for the Lbs Assistance GPS Data Test server
// 
//


#include "ctlbsassdatashareddata.h"


/**
  Function : NewL
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsAssDataSharedData* CT_LbsAssDataSharedData::NewL()
	{
	CT_LbsAssDataSharedData* sharedData=new(ELeave) CT_LbsAssDataSharedData();
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
void CT_LbsAssDataSharedData::ConstructL()
	{
	CT_LbsSharedData::ConstructL();
	}


/**
  Function : CT_LbsDataSrcSharedData
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsAssDataSharedData::CT_LbsAssDataSharedData()
	{
	}
	
/**
  Function : ~CT_LbsDataSrcSharedData
  Description : Destructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/	
CT_LbsAssDataSharedData::~CT_LbsAssDataSharedData()
	{
	}


