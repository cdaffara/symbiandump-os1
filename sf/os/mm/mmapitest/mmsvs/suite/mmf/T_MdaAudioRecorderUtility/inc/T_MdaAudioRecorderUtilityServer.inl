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


#include "T_MMDataMdaAudioRecorderUtility.h"

_LIT(KDataMdaAudioRecorderUtility,		"CMdaAudioRecorderUtility");

CT_MdaAudioRecorderUtilityServer::CT_MdaAudioRecorderUtilityBlock::CT_MdaAudioRecorderUtilityBlock()
:	CTestBlockController()
	{
	}

CT_MdaAudioRecorderUtilityServer::CT_MdaAudioRecorderUtilityBlock::~CT_MdaAudioRecorderUtilityBlock()
	{
	}

CDataWrapper* CT_MdaAudioRecorderUtilityServer::CT_MdaAudioRecorderUtilityBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper* wrapper = NULL;
	if( KDataMdaAudioRecorderUtility() == aData )
		{
		wrapper = CT_MMDataMdaAudioRecorderUtility::NewL();
		}
	return wrapper;
	}

CT_MdaAudioRecorderUtilityServer::CT_MdaAudioRecorderUtilityServer()
	{
	}

CT_MdaAudioRecorderUtilityServer::~CT_MdaAudioRecorderUtilityServer()
	{
	}

void CT_MdaAudioRecorderUtilityServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_MdaAudioRecorderUtilityServer::CreateTestBlock()
	{
	return new CT_MdaAudioRecorderUtilityBlock();
	}
