/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the definition of the server side for the
*			   : custom interface custom command handler.
*
*
*/



#ifndef CUSTOMINTERFACECUSTOMCOMMANDIMPLEMENTOR_H
#define CUSTOMINTERFACECUSTOMCOMMANDIMPLEMENTOR_H

// INCLUDES


// CLASS DECLARATION

/**
*  This is the main class of the Custom Interface Custom Commands Implementor
*
*  @lib CustomInterfaceBuilder.lib
*  @since 3.0
*/

class MCustomInterfaceCustomCommandImplementor
{

public:

	/**
	*
	* Adds a new effect.
	* @since	3.0
	* @return Handle to the effects mananger
	*/
	virtual const TMMFMessageDestination& GetCustomInterfaceBuilderL() = 0;

};


#endif  // CUSTOMINTERFACECUSTOMCOMMANDIMPLEMENTOR_H
