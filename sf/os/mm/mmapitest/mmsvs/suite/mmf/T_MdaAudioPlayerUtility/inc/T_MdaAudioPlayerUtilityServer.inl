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


#include "T_MMDataMdaAudioPlayerUtility.h"

// Constants
_LIT(KDataMdaAudioPlayerUtility,		"CMdaAudioPlayerUtility");

CT_MdaAudioPlayerUtilityServer::CT_MdaAudioPlayerUtilityBlock::CT_MdaAudioPlayerUtilityBlock()
:	CTestBlockController()
	{
	}

CT_MdaAudioPlayerUtilityServer::CT_MdaAudioPlayerUtilityBlock::~CT_MdaAudioPlayerUtilityBlock()
	{
	}

CDataWrapper* CT_MdaAudioPlayerUtilityServer::CT_MdaAudioPlayerUtilityBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper* wrapper = NULL;
	if( KDataMdaAudioPlayerUtility() == aData )
		{
		wrapper = CT_MMDataMdaAudioPlayerUtility::NewL();
		}
	return wrapper;
	}

CT_MdaAudioPlayerUtilityServer::CT_MdaAudioPlayerUtilityServer()
	{
	}
CT_MdaAudioPlayerUtilityServer::~CT_MdaAudioPlayerUtilityServer()
	{
	}

void CT_MdaAudioPlayerUtilityServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_MdaAudioPlayerUtilityServer::CreateTestBlock()
	{
	return new CT_MdaAudioPlayerUtilityBlock();
	}
