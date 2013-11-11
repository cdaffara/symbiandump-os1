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
 @file 
 @internalTechnology 
*/

#if !defined(__SS_CONNSETTINGS_H__)
#define __SS_CONNSETTINGS_H__

#include <e32def.h>
#include <e32base.h>

/**
 Base class that any client wishing to access underlying comms database settings
 @internalTechnology
 @released Since 9.1
 @see CConnectionProvider
 */
class CConnectionSettings : public CBase
	{
public:
	IMPORT_C CConnectionSettings();
	
	IMPORT_C void GetBoolSettingL(const TDesC& aSettingName, TBool& aValue, const RMessagePtr2* aMessage );
	IMPORT_C void GetDes16SettingL(const TDesC& aSettingName, TDes16& aValue, const RMessagePtr2* aMessage );
	IMPORT_C void GetDes8SettingL(const TDesC& aSettingName, TDes8& aValue, const RMessagePtr2* aMessage );
	IMPORT_C void GetIntSettingL(const TDesC& aSettingName, TUint32& aValue, const RMessagePtr2* aMessage );
	IMPORT_C HBufC* GetLongDesSettingLC(const TDesC& aSettingName, const RMessagePtr2* aMessage );

protected:
	/**
	 Override this to retrieve a boolean value from the database with the given field name
	 
	 @param aSettingName Field name
	 @param aValue Reference to a boolean that will contain the value on call success
	 @param aMessage RMessage pointer required for secure session communication
	 */
	virtual void DoGetBoolSettingL(const TDesC& aSettingName, TBool& aValue, const RMessagePtr2* aMessage ) = 0;
	
	/**
	 Override this to retrieve a unicode string value from the database with the given field name
	 
	 @param aSettingName Field name
	 @param aValue Reference to a string that will contain the value on call success
	 @param aMessage RMessage pointer required for secure session communication
	 */
	virtual void DoGetDes16SettingL(const TDesC& aSettingName, TDes16& aValue, const RMessagePtr2* aMessage ) = 0;
	
	/**
	 Override this to retrieve a 8-bit string value from the database with the given field name
	 
	 @param aSettingName Field name
	 @param aValue Reference to a string that will contain the value on call success
	 @param aMessage RMessage pointer required for secure session communication
	 */
	virtual void DoGetDes8SettingL(const TDesC& aSettingName, TDes8& aValue, const RMessagePtr2* aMessage ) = 0;
	
	/**
	 Override this to retrieve a unsigned 32-bit int value from the database with the given field name
	 
	 @param aSettingName Field name
	 @param aValue Reference to a TUint32 that will contain the value on call success
	 @param aMessage RMessage pointer required for secure session communication
	 */	
	virtual void DoGetIntSettingL(const TDesC& aSettingName, TUint32& aValue, const RMessagePtr2* aMessage ) = 0;
	
	/**
	 Override this to retrieve a string value as a heap allocated buffer from the database with the given field name
	 
	 @param aSettingName Field name	 
	 @param aMessage RMessage pointer required for secure session communication
	 @return Pointer to the dynamically allocated string
	 */
	virtual HBufC* DoGetLongDesSettingLC(const TDesC& aSettingName, const RMessagePtr2* aMessage ) = 0;
	};
	
#endif	// __SS_CONNSETTINGS_H__

