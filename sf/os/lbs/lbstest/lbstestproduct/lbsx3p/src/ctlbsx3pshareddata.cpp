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
// @file ctlbsx3pshareddata.cpp
// This file contains the shared data class for the Lbs Assistance GPS Data Test server
// 
//


#include "ctlbsx3pshareddata.h"


/**
  Function : NewL
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsX3PSharedData* CT_LbsX3PSharedData::NewL()
	{
	CT_LbsX3PSharedData* sharedData=new(ELeave) CT_LbsX3PSharedData();
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
void CT_LbsX3PSharedData::ConstructL()
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
CT_LbsX3PSharedData::CT_LbsX3PSharedData()
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
CT_LbsX3PSharedData::~CT_LbsX3PSharedData()
	{
	}


