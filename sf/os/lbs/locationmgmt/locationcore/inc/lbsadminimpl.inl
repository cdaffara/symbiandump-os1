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
//

/**
 Gets the settings whose associated property is
 being observed by the CAdminSettingSubscriber.
 @return iSetting in the CAdminSettingSubscriber.
 @capability 
 @see TLbsAdminSetting
 @released 
*/
inline TLbsAdminSetting CAdminSettingSubscriber::GetSetting()
{
	return iSetting;
}

