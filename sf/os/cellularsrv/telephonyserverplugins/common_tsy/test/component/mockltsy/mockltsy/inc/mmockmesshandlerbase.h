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

#ifndef MMOCKMESSHANDLERBASE_H
#define MMOCKMESSHANDLERBASE_H


//  INCLUDES
#include "mmmmesshandlerbase.h"
#include "tmockltsydata.h"


class MMockMessHandlerBase : public MMmMessHandlerBase
	{
public:
    virtual void CompleteL(TInt /*aIpc*/, const TDesC8& /*aData*/, TInt /*aResult*/) =0;
    
    virtual void LogErrorData(TInt /*aIpc*/, const MMockSyEngineData& /*aData*/, const TDesC8& /*aExpectedData*/)  {}; //=0;
	};
	
#endif
