// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 Sets one of the settings defined by the CLbsAdmin class.
 @param aSetting The setting to be changed.
 @param aValue   The new value of the setting.
 @return KErrNone if successful, KErrArgument if the value is out of range, KErrNotSupported if the value is not supported and one of the system wide error codes otherwise. 
 @capability WriteDeviceData
 @see TLbsAdminSetting
 @released 
*/
template <typename T>
	inline TInt CLbsAdmin::Set(const TLbsAdminSetting& aSetting, const T&  aValue)	
		{
		return DoSet(aSetting, aValue);
		}


/** Gets one of the settings defined by the CLbsAdmin class.
@param aSetting The setting to be retrieved.
@param aValue   The value of the setting.
@return KErrNone if successful, one of the system wide error codes otherwise.
@see TLbsAdminSetting
@released */
template <typename T>
	inline TInt CLbsAdmin::Get(const TLbsAdminSetting& aSetting, T&  aValue) const
		{
		return DoGet(aSetting, aValue);		
		}
