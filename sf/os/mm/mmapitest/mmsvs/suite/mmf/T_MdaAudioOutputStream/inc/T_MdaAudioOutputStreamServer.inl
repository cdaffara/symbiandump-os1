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


#include "T_MMDataMdaAudioOutputStream.h"

_LIT(KT_MdaAudioOutputStream,	"CMdaAudioOutputStream");

CT_MdaAudioOutputStreamServer::CT_MdaAudioOutputStreamBlock::CT_MdaAudioOutputStreamBlock()
:	CTestBlockController()
	{
	}

CT_MdaAudioOutputStreamServer::CT_MdaAudioOutputStreamBlock::~CT_MdaAudioOutputStreamBlock()
	{
	}

CDataWrapper* CT_MdaAudioOutputStreamServer::CT_MdaAudioOutputStreamBlock::CreateDataL(const TDesC& aData)
	{
	CDataWrapper* wrapper = NULL;

	if( KT_MdaAudioOutputStream() == aData )
		{
		wrapper = CT_MMDataMdaAudioOutputStream::NewL();
		}

	return wrapper;
	}

CT_MdaAudioOutputStreamServer::CT_MdaAudioOutputStreamServer()
	{
	}

CT_MdaAudioOutputStreamServer::~CT_MdaAudioOutputStreamServer()
	{
	}

void CT_MdaAudioOutputStreamServer::DeleteActiveSchedulerL()
	{
	}

CTestBlockController* CT_MdaAudioOutputStreamServer::CreateTestBlock()
	{
	return new CT_MdaAudioOutputStreamBlock();
	}
