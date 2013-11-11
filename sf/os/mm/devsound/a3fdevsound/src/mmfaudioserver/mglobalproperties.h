/*
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
* Save as expressly licensed to you by Symbian Software Ltd, all rights reserved.
* 
*
*/



/**
 @file
 @internalTechnology 
*/

#ifndef MGLOBALPROPERTIES_H
#define MGLOBALPROPERTIES_H


/**
	The Audio Server has certain global properties that are common to all DevSound Server threads
	this is an interface to access those properties
*/

class MGlobalProperties
	{
public:
	/**
	 * Returns reference to FourCC to format converter

	 * @return CFourCCConvertor a reference to the fourcc to format converter
	*/
	virtual const CFourCCConvertor& GetFourCCConvertor() = 0;
	};


#endif //MGLOBALPROPERTIES_H
