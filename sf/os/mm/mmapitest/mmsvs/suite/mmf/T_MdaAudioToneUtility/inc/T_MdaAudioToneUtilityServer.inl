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


#include "T_MMDataAudioToneUtility.h"

_LIT(KT_MdaAudioToneUtilityWrapper,           "CMdaAudioToneUtility");

CT_MdaAudioToneUtilityServer::CT_MdaAudioToneUtilityBlock::CT_MdaAudioToneUtilityBlock()
:	CTestBlockController()
	{
	}

CT_MdaAudioToneUtilityServer::CT_MdaAudioToneUtilityBlock::~CT_MdaAudioToneUtilityBlock()
	{
	}

CDataWrapper* CT_MdaAudioToneUtilityServer::CT_MdaAudioToneUtilityBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper* wrapper = NULL;

	if( KT_MdaAudioToneUtilityWrapper() == aData )
		{
		wrapper = CT_MMDataAudioToneUtility::NewL();
		}

	return wrapper;
	}

CT_MdaAudioToneUtilityServer::CT_MdaAudioToneUtilityServer()
	{
	}

CT_MdaAudioToneUtilityServer::~CT_MdaAudioToneUtilityServer()
	{
	}

void CT_MdaAudioToneUtilityServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_MdaAudioToneUtilityServer::CreateTestBlock()
	{
	return new CT_MdaAudioToneUtilityBlock();
	}
