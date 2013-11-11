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
* Baseband Channel Adaptor(BCA) Factory APIs.
* This file contains all the APIs required to implement a MBcaFactory for Symbian OS.
* 
*
*/



/**
 @file 
 @publishedPartner
 @prototype
 @released
*/
 
#ifndef BCA2FACTORY_H
#define BCA2FACTORY_H

#include <networking/bca2.h>

/** This namespace includes the BCA component names.*/
namespace BasebandChannelAdaptation2
{

/**
 * The abstract interface responsible for BCA creation.
 
 * @publishedPartner
 * @prototype
 * @released
 */
class MBca2Factory
	{
public:
	/** deletes the allocated data in BcaFactory*/
	virtual void Release()=0;
	
	/**
	* This method creates a BCA instance and returns a pointer to it.
	
	* @return a pointer to BCA instance.
	*/
	virtual MBca2* NewBcaL()=0;		
	};
} 
#endif // BCA2FACTORY_H
