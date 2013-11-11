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


#include "T_MMDataMdaAudioConvertUtility.h"

_LIT(KT_MdaAudioConvertUtility,	"CMdaAudioConvertUtility");

CT_MdaAudioConvertUtilityServer::CT_MdaAudioConvertUtilityBlock::CT_MdaAudioConvertUtilityBlock()
:	CTestBlockController()
	{
	}

CT_MdaAudioConvertUtilityServer::CT_MdaAudioConvertUtilityBlock::~CT_MdaAudioConvertUtilityBlock()
	{
	}

CDataWrapper* CT_MdaAudioConvertUtilityServer::CT_MdaAudioConvertUtilityBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper*	wrapper = NULL;
	if( KT_MdaAudioConvertUtility() == aData )
		{
		wrapper = CT_MMDataMdaAudioConvertUtility::NewL();
		}
	return wrapper;
	}

CT_MdaAudioConvertUtilityServer::CT_MdaAudioConvertUtilityServer()
	{
	}

CT_MdaAudioConvertUtilityServer::~CT_MdaAudioConvertUtilityServer()
	{
	}

void CT_MdaAudioConvertUtilityServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_MdaAudioConvertUtilityServer::CreateTestBlock()
	{
	return new CT_MdaAudioConvertUtilityBlock();
	}
