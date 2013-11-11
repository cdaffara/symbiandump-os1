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
// @file CTLbsAGPSHAIServerRoot.cpp
// This is the class implementation for the LBS AGPS HAI Validation Test Server
// 
//

// User includes
// Header files of LBS Client API Test Steps
#include "ctlbsagpshaivalidatestep.h"



CT_LbsAGPSHAIServerRoot::~CT_LbsAGPSHAIServerRoot()
	{
	delete iScheduler;
	}
	
	
/**
  Function : CT_LbsClientServer
  Description : Constructor
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
CT_LbsAGPSHAIServerRoot::CT_LbsAGPSHAIServerRoot()
	{
	}

/**
  Function : ConstructL
  Description : 
  @internalTechnology
  @param :
  @return : N/A
  @precondition : none
  @postcondition : none
*/
void CT_LbsAGPSHAIServerRoot::ConstructL()
	{
	CTestServer::ConstructL(ServerName());

	// Create the active scheduler for the test steps
	iScheduler = new (ELeave) CActiveScheduler();
	}
