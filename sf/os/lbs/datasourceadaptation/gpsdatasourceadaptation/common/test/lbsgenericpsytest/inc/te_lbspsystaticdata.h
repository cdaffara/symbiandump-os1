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
// The class for retrieving the test static data.
// 
//

#if (!defined __TE_LBSPSYSTATICDATA__)
#define __TE_LBSPSYSTATICDATA__

#include <e32base.h>

#include "LbsInternalInterface.h"

class CTestExecuteLogger;

class MTe_LbsPsyStaticData
	{	
public:	
	static MTe_LbsPsyStaticData& Static();
	IMPORT_C static void SetStaticL(MTe_LbsPsyStaticData* aData);
	
	virtual TUid ModuleUid() const = 0;
	virtual const RLbsPositionUpdateRequests::TChannelIdentifer& UpdateReqChanId() const = 0;
	
	virtual TUint CountPositioner() = 0;
	
	virtual CTestExecuteLogger& Logger() const = 0;
	
	
	IMPORT_C void PrintPosInfo(const TPositionInfo& aPosInfo) const;
	
	
	};
		
#endif //__TE_LBSPSYSTATICDATA__
