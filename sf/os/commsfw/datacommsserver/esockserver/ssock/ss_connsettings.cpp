// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file SS_CONNSETTINGS.CPP
*/

#include "ss_connsettings.h"

/**
 Constructor
 */
EXPORT_C CConnectionSettings::CConnectionSettings()
	{
	}

/**
 Retrieve a boolean value from the database with the given field name
 
 @param aSettingName Field name
 @param aValue Reference to a boolean that will contain the value on call success
 @param aMessage RMessage pointer required for secure session communication
 */
EXPORT_C void CConnectionSettings::GetBoolSettingL(const TDesC& aSettingName, TBool& aValue, const RMessagePtr2* aMessage )
	{
	DoGetBoolSettingL( aSettingName, aValue, aMessage );
	}
	
/**
 Retrieve a unicode string value from the database with the given field name
 
 @param aSettingName Field name
 @param aValue Reference to a string that will contain the value on call success
 @param aMessage RMessage pointer required for secure session communication
 */	
EXPORT_C void CConnectionSettings::GetDes16SettingL(const TDesC& aSettingName, TDes16& aValue, const RMessagePtr2* aMessage )
	{
	DoGetDes16SettingL( aSettingName, aValue, aMessage );
	}
	
/**
 Retrieve a 8-bit string value from the database with the given field name
 
 @param aSettingName Field name
 @param aValue Reference to a string that will contain the value on call success
 @param aMessage RMessage pointer required for secure session communication
 */	
EXPORT_C void CConnectionSettings::GetDes8SettingL(const TDesC& aSettingName, TDes8& aValue, const RMessagePtr2* aMessage )
	{
	DoGetDes8SettingL( aSettingName, aValue, aMessage );
	}
	
/**
 Retrieve a unsigned 32-bit int value from the database with the given field name
 
 @param aSettingName Field name
 @param aValue Reference to a TUint32 that will contain the value on call success
 @param aMessage RMessage pointer required for secure session communication
 */
EXPORT_C void CConnectionSettings::GetIntSettingL(const TDesC& aSettingName, TUint32& aValue, const RMessagePtr2* aMessage )
	{
	DoGetIntSettingL( aSettingName, aValue, aMessage );
	}

/**
 Retrieve a string value as a heap allocated buffer from the database with the given field name
 
 @param aSettingName Field name	 
 @param aMessage RMessage pointer required for secure session communication
 @return Pointer to the dynamically allocated string
 */	
EXPORT_C HBufC* CConnectionSettings::GetLongDesSettingLC(const TDesC& aSettingName, const RMessagePtr2* aMessage )
	{
	return DoGetLongDesSettingLC( aSettingName, aMessage );
	}
	

