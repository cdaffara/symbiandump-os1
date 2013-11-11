// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// responsehandler.cpp
// 
//

/**
 @file
 @InternalComponent
*/

#include "cpositionerq.h"
#include "cresponsehandler.h"
#include "lbsdevloggermacros.h"

/*
* CResponseHandler::CResponseHandler
* C++ default constructor can NOT contain any code, that
* might leave.
*/
CResponseHandler::CResponseHandler()
    {
    }

CResponseHandler::~CResponseHandler()
    {
    }

/**
Nothing here, place holder for error when data error

@param aError Data error code
@param aStatus The status of Data bus
@return Symbian standard error
*/
TInt CResponseHandler::DataError(TInt /*aError*/, TInt /*aStatus*/)	
	{
	return KErrNone;
	}
