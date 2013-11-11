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


#include "T_MMDataImageDecoder.h"

_LIT(KT_ImageDecoderWrapper,	"CImageDecoder");

// --------------------- CT_ImageDecoderBlock -----------------------------------

inline CT_ImageDecoderServer::CT_ImageDecoderBlock::CT_ImageDecoderBlock()
    {
    }

inline CT_ImageDecoderServer::CT_ImageDecoderBlock::~CT_ImageDecoderBlock()
    {
    }

inline CDataWrapper* CT_ImageDecoderServer::CT_ImageDecoderBlock::CreateDataL( const TDesC& aData )
	{
	CDataWrapper* wrapper = NULL;
	if( KT_ImageDecoderWrapper() == aData )
		{
		wrapper = CT_MMDataImageDecoder::NewL();
		}
	return wrapper;
	}

// --------------------- CT_ImageDecoderServer -----------------------------------

inline CT_ImageDecoderServer::CT_ImageDecoderServer()
    {
    }

inline CT_ImageDecoderServer::~CT_ImageDecoderServer()
    {
    }

void CT_ImageDecoderServer::DeleteActiveSchedulerL()
	{
	}

inline CTestBlockController* CT_ImageDecoderServer::CreateTestBlock()
	{
	return new CT_ImageDecoderBlock();
	}
