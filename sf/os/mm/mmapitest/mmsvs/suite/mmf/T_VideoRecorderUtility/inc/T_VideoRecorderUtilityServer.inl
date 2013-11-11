/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "T_MMDataVideoRecorderUtility.h"
#include "T_MMDataCamera.h"

_LIT(KDataCamera,					"CCamera");
_LIT(KDataVideoRecorderUtility,		"CVideoRecorderUtility");

CT_VideoRecorderUtilityServer::CT_VideoRecorderUtilityBlock::CT_VideoRecorderUtilityBlock()
	:	CTestBlockController()
	{
	}

CT_VideoRecorderUtilityServer::CT_VideoRecorderUtilityBlock::~CT_VideoRecorderUtilityBlock()
	{
	}

CDataWrapper* CT_VideoRecorderUtilityServer::CT_VideoRecorderUtilityBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper*	wrapper = NULL;
	if( KDataVideoRecorderUtility() == aData )
		{
		wrapper = CT_MMDataVideoRecorderUtility::NewL();
		}
	else if(KDataCamera() == aData)
		{
		wrapper = CT_MMDataCamera::NewL();
		}
	return wrapper;
	}

CT_VideoRecorderUtilityServer::CT_VideoRecorderUtilityServer()
	{
	}

CT_VideoRecorderUtilityServer::~CT_VideoRecorderUtilityServer()
	{
	}

void CT_VideoRecorderUtilityServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_VideoRecorderUtilityServer::CreateTestBlock()
	{
	return new CT_VideoRecorderUtilityBlock();
	}
