/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ErrorLogStrings.h
*/

#if (!defined ERRORLOGSTRINGS_H)
#define ERRORLOGSTRINGS_H

// Error codes
//------------

// Config (ini) file data errors
_LIT(KErrString_MissingConfigFileField, "Couldn't find expected config field. section:%S, field:%S");
_LIT(KErrString_UnsignedConfigValueLessThanZero, "Unsigned config value less than zero. section:%S, field:%S");

_LIT(KErrString_NamedObjectNotFound, "Couldn't find object instance. name:%S");
_LIT(KErrString_OutOfBounds, "Numerical value out of bounds. section:%S, field:%S");

#endif // ERRORLOGSTRINGS_H

