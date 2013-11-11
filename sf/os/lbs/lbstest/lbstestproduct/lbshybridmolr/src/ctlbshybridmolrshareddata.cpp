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
// @file ctlbshybridmtlrshareddata.cpp
// This file contains the shared data class for the Lbs MTLR Test server
// 
//


#include "ctlbshybridmolrshareddata.h"


/**
  Function : NewL
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsHybridMOLRSharedData* CT_LbsHybridMOLRSharedData::NewL()
	{
	CT_LbsHybridMOLRSharedData* sharedData=new(ELeave) CT_LbsHybridMOLRSharedData();
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
void CT_LbsHybridMOLRSharedData::ConstructL()
	{
	CT_LbsSharedData::ConstructL();
	}


/**
  Function : CT_LbsHybridMOLRSharedData
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsHybridMOLRSharedData::CT_LbsHybridMOLRSharedData()
	{
	}
	
/**
  Function : ~CT_LbsHybridMOLRSharedData
  Description : Destructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/	
CT_LbsHybridMOLRSharedData::~CT_LbsHybridMOLRSharedData()
	{
/*	for (TInt i = 0; i < iUpdateArr.Count(); ++i)
		{
		delete iUpdateArr[i];		
		}
	iUpdateArr.Close();
*/	}


