/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* C32 Baseband Channel Adaptor(BCA) Factory APIs.
* This file contains all the APIs required to implement a CC32BcaFactory for Symbian OS.
* 
*
*/



/**
 @file 
 @internalComponent
*/
 
#ifndef C32BCAFACTORY_H
#define C32BCAFACTORY_H

#include <networking/bcafactory.h>
#include <e32base.h>

/** This namespace includes the BCA component names.*/
namespace BasebandChannelAdaptation
{
	namespace C32Bca
	{
	
	/**
	 * The interface to the factory responsible for C32BCA creation. */
	NONSHARABLE_CLASS(CC32BcaFactory): public CBase, public MBcaFactory
		{
	public:
		virtual void Release();
		virtual MBca* NewBcaL();
		};
	}
}
#endif //C32BCAFACTORY_H
