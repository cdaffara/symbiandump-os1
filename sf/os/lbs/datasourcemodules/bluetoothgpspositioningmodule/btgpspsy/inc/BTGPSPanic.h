/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef BTGPSPANIC_H
#define BTGPSPANIC_H

//  INCLUDES

#include <e32base.h>


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

//BTGPSPSY panic category
_LIT(KBtGpsPanicCategory, "BTGPSPSY");

//Panic reason
enum TPanicReason
    {
    EPanicInvalidCase,				///Invalid case in switch clause.
    EPanicProcessDefination,		///Invalid process defination during connection.
    EPanicInvalidNmeaSentence,		///Invalid Nmea sentence.
    EPanicUnknownPositioningClass,  ///Unkown positioning class.
    EPanicSingletonNotConstructed,  ///Singleton not constructed.
    EPanicIndexOutOfRange			///Index out of range.
    };

/**
* Panic function for BT GPS PSY.
* @param aReason The panice reason.
*
*/
inline void Panic(TInt aReason)
    {
    User::Panic(KBtGpsPanicCategory, aReason);
    };

#endif
// End of File

