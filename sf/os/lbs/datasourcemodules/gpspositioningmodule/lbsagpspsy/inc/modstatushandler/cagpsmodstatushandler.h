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
//

/**
 @file
 @InternalComponent
*/

#include <e32base.h>
#include "cmodulestatushandler.h"

class CAgpsModStatusHandler : public CModuleStatusHandler 
	{
    public:
        static CAgpsModStatusHandler* NewL(CPositionerQ* aPositionerQ);
        ~CAgpsModStatusHandler();
        
    	// from CActive
    	void RunL();
    	void DoCancel();

	private:
        CAgpsModStatusHandler();
        void ConstructL(CPositionerQ* aPositionerQ);	
	};
