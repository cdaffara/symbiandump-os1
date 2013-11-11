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


#include "T_MMDataVideoPlayerUtility.h"

_LIT(KDataVideoPlayerUtility,	"CVideoPlayerUtility");

CT_VideoPlayerUtilityServer::CT_VideoPlayerUtility::CT_VideoPlayerUtility()
:	CTestBlockController()
	{
	}

CT_VideoPlayerUtilityServer::CT_VideoPlayerUtility::~CT_VideoPlayerUtility()
	{
	}

CDataWrapper* CT_VideoPlayerUtilityServer::CT_VideoPlayerUtility::CreateDataL(const TDesC& aData)
	{
	CDataWrapper* wrapper = NULL;
	if( KDataVideoPlayerUtility() == aData )
		{
		wrapper = CT_MMDataVideoPlayerUtility::NewL();
		}
	return wrapper;
	}

CT_VideoPlayerUtilityServer::CT_VideoPlayerUtilityServer()
	{
	}

CT_VideoPlayerUtilityServer::~CT_VideoPlayerUtilityServer()
	{
	}

void CT_VideoPlayerUtilityServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_VideoPlayerUtilityServer::CreateTestBlock()
	{
	return new CT_VideoPlayerUtility();
	}
