/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file
*/

#ifndef _EIRMANAGER_INL
#define _EIRMANAGER_INL

inline TBool CEirManager::TagValid(TEirTag aTag) const
	{
	return aTag >= EEirTagName && aTag < EEirTagRESERVED;
	}

#endif // _EIRMANAGER_INL

