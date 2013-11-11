// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Provides the implementation of the PanicNow function for global
// use throughout the Ssm Reference Plugin component.
// 
//

/**
 @internalComponent
*/

#include "ssmpanic.h"

//
// Global panic function for use within System State Management component
void PanicNow(const TDesC &aCategory, TInt aReason)
  	{
  	User::Panic(aCategory, aReason);
  	}
