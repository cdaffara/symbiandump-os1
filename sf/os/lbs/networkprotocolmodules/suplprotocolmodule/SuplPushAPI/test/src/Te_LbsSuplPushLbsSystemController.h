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
// A unit test wrapper over the RLbsSystemController class
// 
//

#if (!defined __TE_LBSSUPLPUSHLBSSYSTEMCONTROLLER_H__)
#define __TE_LBSSUPLPUSHLBSSYSTEMCONTROLLER_H__

#include <e32property.h>
#include "Te_LbsSuplCommon.h"
#include "lbssystemcontroller.h"

/**
A unit test wrapper over the RLbsSystemController class
*/
class RTe_LbsSuplPushLbsSystemController 
	{
public:	
	void OpenL(TUid aModuleId);
	void Close();
	TInt RequestSystemStartup();
	
private:
	/** The original session handler */
	RLbsSystemController iLbsSystemController;
	};
	
#endif //__TE_LBSSUPLPUSHLBSSYSTEMCONTROLLER_H__	
