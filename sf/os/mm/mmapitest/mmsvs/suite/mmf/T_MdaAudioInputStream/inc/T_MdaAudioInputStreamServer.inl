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


#include "T_MMDataMdaAudioInputStream.h"

_LIT(KT_MdaAudioInputStream,	"CMdaAudioInputStream");

CT_MdaAudioInputStreamServer::CT_MdaAudioInputStreamBlock::CT_MdaAudioInputStreamBlock()
:	CTestBlockController()
	{
	}

CT_MdaAudioInputStreamServer::CT_MdaAudioInputStreamBlock::~CT_MdaAudioInputStreamBlock()
	{
	}

CDataWrapper* CT_MdaAudioInputStreamServer::CT_MdaAudioInputStreamBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper*	wrapper = NULL;

	if( KT_MdaAudioInputStream() == aData )
		{
		wrapper = CT_MMDataMdaAudioInputStream::NewL();
		}

	return wrapper;
	}

CT_MdaAudioInputStreamServer::CT_MdaAudioInputStreamServer()
	{
	}

CT_MdaAudioInputStreamServer::~CT_MdaAudioInputStreamServer()
	{
	}

void CT_MdaAudioInputStreamServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_MdaAudioInputStreamServer::CreateTestBlock()
	{
	return new CT_MdaAudioInputStreamBlock();
	}
