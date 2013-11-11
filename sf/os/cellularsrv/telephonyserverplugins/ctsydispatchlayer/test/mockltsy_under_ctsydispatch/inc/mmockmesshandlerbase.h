// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalAll 
*/

#ifndef MMOCKMESSHANDLERBASE_H
#define MMOCKMESSHANDLERBASE_H


//  INCLUDES
#include "mmmmesshandlerbase.h"
class MMockSyEngineData;

#define VA_ARG_ENUM(ap,type) static_cast<type> (VA_ARG(ap,TInt))

class MMockMessHandlerBase : public MMmMessHandlerBase
	{
public:
    virtual void CompleteL(TInt /*aIpc*/, const TDesC8& /*aData*/, TInt /*aResult*/) =0;    
    virtual void LogErrorData(TInt /*aIpc*/, const MMockSyEngineData& /*aData*/, const TDesC8& /*aExpectedData*/)  {}; //=0;
    virtual TBool IsHandlerForApi(TInt aApiId) = 0;
	};
	
#endif
