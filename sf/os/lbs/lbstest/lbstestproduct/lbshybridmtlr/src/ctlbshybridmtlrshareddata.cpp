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


#include "ctlbshybridmtlrshareddata.h"


/**
  Function : NewL
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsHybridMTLRSharedData* CT_LbsHybridMTLRSharedData::NewL()
	{
	CT_LbsHybridMTLRSharedData* sharedData=new(ELeave) CT_LbsHybridMTLRSharedData();
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
void CT_LbsHybridMTLRSharedData::ConstructL()
	{
	CT_LbsSharedData::ConstructL();
	}


/**
  Function : CT_LbsHybridMTLRSharedData
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsHybridMTLRSharedData::CT_LbsHybridMTLRSharedData()
	{
	}
	
/**
  Function : ~CT_LbsHybridMTLRSharedData
  Description : Destructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/	
CT_LbsHybridMTLRSharedData::~CT_LbsHybridMTLRSharedData()
	{
/*	for (TInt i = 0; i < iUpdateArr.Count(); ++i)
		{
		delete iUpdateArr[i];		
		}
	iUpdateArr.Close();
*/	}


