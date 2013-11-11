/*
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



#ifndef MINTERFACEPROVIDER_H
#define MINTERFACEPROVIDER_H

#include <a3f/a3fbase.h>

class CMMFBuffer;
class MOutputPort;

/**
 */
class MInterfaceProvider
	{
public:
	/**
	* @param aInterfaceUid the uid of the custom interface 
	*/
	virtual TAny* GetCustomInterface(TUid aInterfaceId) = 0;
	};

#endif // MINTERFACEPROVIDER_H
