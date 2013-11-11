/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    This is the definition for CDosFactoryBase class.
*
*/



#ifndef __DOSFACTORYBASE_H__
#define __DOSFACTORYBASE_H__

#include "dosdsy.h"

class CDosEventManager;

/**
* Factory class that is responsible to create all the other services. 
* The plug-in's factory derives from this object.
*/
class CDosFactoryBase : public CBase, public MDosFactoryBaseDSY
{
public:
	/**
	* Returns a pointer to the DosServer's event manager.
	* @return A pointer to event manager.
	*/
	inline CDosEventManager* EventManager() const;

private: //friend classes
	friend class CDosServer;

private:
	//Pointer to the DosServer's event manager.
	CDosEventManager* iEventManager;
};


#include "dosfactorybase.inl"


#endif //__DOSFACTORYBASE_H__
