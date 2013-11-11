// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @prototype
*/

#ifndef SGRESOURCEINTERNAL_H
#define SGRESOURCEINTERNAL_H


/**
@internalTechnology
@prototype

Flags controlling how a drawable resource is opened.
*/
enum TSgDrawableOpenModes
	{
	/**
	By default a drawable resource is allowed to be used only for the purposes
	explicitly requested when it was created, but any handle to it which is opened
	with this option can be used in all the rendering pipelines that are compatible
	with the drawable resource according to the underlying implementation of the
	Graphics subsystem, regardless of the usage explicitly requested during creation.
	*/
	ESgDoNotRestrictUsage = 0x0001
	};


#endif // SGRESOURCEINTERNAL_H
