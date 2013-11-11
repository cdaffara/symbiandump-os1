// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the header file which contains the test version of a intergration module
//

//! @file dummyagpsmodule.h

#ifndef __DUMMY_AGPS_MODULE_H__
#define __DUMMY_AGPS_MODULE_H__


// Epoc includes

// LBS includes
#include <lbs.h>
#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <lbs/lbslocdatasourcegpsbase.h>

class CDummyAGpsModule : public CLbsLocationSourceGpsBase
	{	
public:
	static CLbsLocationSourceGpsBase* NewL(MLbsLocationSourceGpsObserver& aObserver);
	~CDummyAGpsModule();

	// CLbsLocationSourceGpsBase functions.
	void RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality);
	void CancelLocationRequest();
	void AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode);
	void SetGpsOptions(const TLbsGpsOptions& aGpsOptions);
	void AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask);

		
private:
	CDummyAGpsModule(MLbsLocationSourceGpsObserver& aObserver);
	void ConstructL();

private:
	};

#endif //__DUMMY_AGPS_MODULE_H__

