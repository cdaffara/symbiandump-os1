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


#include "T_MMDataImageEncoder.h"

_LIT(KDataImageEncoder,	"CImageEncoder");

// --------------------- CT_ImageEncoder -----------------------------------

inline CT_ImageEncoderServer::CT_ImageEncoder::CT_ImageEncoder()
    {
    }

inline CT_ImageEncoderServer::CT_ImageEncoder::~CT_ImageEncoder()
    {
    }

inline CDataWrapper* CT_ImageEncoderServer::CT_ImageEncoder::CreateDataL( const TDesC& aData )
	{
	CDataWrapper* wrapper = NULL;
	if( KDataImageEncoder() == aData )
		{
		wrapper = CT_MMDataImageEncoder::NewL();
		}
	return wrapper;
	}

// --------------------- CT_ImageEncoderServer -----------------------------------

inline CT_ImageEncoderServer::CT_ImageEncoderServer()
    {
    }

inline CT_ImageEncoderServer::~CT_ImageEncoderServer()
    {
    }

void CT_ImageEncoderServer::DeleteActiveSchedulerL()
	{
	}

inline CTestBlockController*	CT_ImageEncoderServer::CreateTestBlock()
	{
	return new CT_ImageEncoder();
	}
