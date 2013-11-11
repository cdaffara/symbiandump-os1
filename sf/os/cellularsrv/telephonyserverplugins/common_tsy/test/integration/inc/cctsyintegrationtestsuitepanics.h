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
* TRpsFunctor declaration
* 
*
*/


 
//Base class for the functor's.  This defines a couple of pure virtual functions that are overriden by the templated functor class.  
//We need this base class otherwise passing the templated class around becomes more difficult.
#ifndef CTSYPANICS_H
#define CTSYPANICS_H

_LIT(KPanic,"te_ctsyint");
//Trp test panic codes
const TInt KCTSYPanic_nosetupstep = 100;
const TInt KCTSYPanic_invalidrpsparameters = 101;
const TInt KCTSYPanic_badhandle = 102;

#endif //CTSYPANICS_H
