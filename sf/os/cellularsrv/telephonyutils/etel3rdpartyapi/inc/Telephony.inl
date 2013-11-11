/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* inline-functions are not embedded in Telephony.DLL.
* So, they don't get EXPORTed, and they don't
* have any associated ordinal at link-time.
* Inline code, logically part of Etel3rdParty.h
*
*/


/**
 @file
*/

inline TVersion CTelephony::Version() const
/**
 Get current CTelephony version
 
 This function returns major, minor, and build version-numbers.
 
 @return	TVersion object, see E32STD.H
 */
	{
	return(TVersion(KTelephonyMajorVersionNumber,
					KTelephonyMinorVersionNumber,
					KTelephonyBuildVersionNumber));
	}
