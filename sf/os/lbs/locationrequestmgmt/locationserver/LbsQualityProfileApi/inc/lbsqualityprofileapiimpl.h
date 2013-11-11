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
// Implementation of Location Based Services Quality Profile API
// 
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef LBS_QUALITY_PROFILE_API_IMPL_H
#define LBS_QUALITY_PROFILE_API_IMPL_H

#include <e32base.h>

#include <lbs/lbsqualityprofileapi.h>

//********************************************************************************************************

NONSHARABLE_CLASS(CLbsQualityProfileApiImpl) : public CBase
	{
public:		
	static CLbsQualityProfileApiImpl* NewL();
	virtual ~CLbsQualityProfileApiImpl();
	
	TInt GetValidated(	const TLbsQualityProfileId& aProfileId, 
						const TLbsQualityProfileApiSetting& aSetting, 
						TReal32& aRealValue);
	
	TInt GetValidated(	const TLbsQualityProfileId& aProfileId, 
						const TLbsQualityProfileApiSetting& aSetting, 
						TTimeIntervalMicroSeconds& aTimeValue);
	
private:
	CLbsQualityProfileApiImpl();
	void ConstructL();

private:
	
	};

#endif //LBS_QUALITY_PROFILE_API_IMPL_H
