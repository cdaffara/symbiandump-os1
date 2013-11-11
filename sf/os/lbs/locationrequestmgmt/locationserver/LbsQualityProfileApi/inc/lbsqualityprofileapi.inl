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
 Gets one of the settings defined by the CLbsQualityProfileApi class.
 @param aProfileId The identity of the quality profile.
 @param aSetting The setting to be retrieved.
 @param aValue   The value of the setting.
 @return KErrNone if successful, one of the system wide error codes otherwise.
 @see TLbsQualityProfileApiSetting
 @publishedPartner
 @released
*/
template <typename T>
	inline TInt CLbsQualityProfileApi::Get(	const TLbsQualityProfileId& aProfileId, 
											const TLbsQualityProfileApiSetting& aSetting, 
											T& aValue) const
		{
		return DoGet(aProfileId, aSetting, aValue);		
		}
