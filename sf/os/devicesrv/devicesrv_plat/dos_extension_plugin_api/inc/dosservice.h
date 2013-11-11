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
*    This is the definition for CDosService class.
*
*/



#ifndef __DOSSERVICE_H__
#define __DOSSERVICE_H__

#include <e32base.h>

class CDosEventManager;

/**
* Base class for all the Server side services.
*/
class CDosService : public CObject
{
private:
	/**
	* This function has to be implemented by the derived class.
	*/
	virtual TInt ExecuteMessageL(const RMessage2& aMessage)=0;

protected:
	/**
	* Puts data into the client's thread.
	* @param aMessage Client-server message.
	* @param aIndex Index of RMessage2 slot where to write.
	* @param aDes Data to be copied.
	*/
	void Write(const RMessage2& aMessage, const TInt& aIndex, const TDesC8& aDes) const;

	/**
	* Returns the DosServer event manager.
	* @return A pointer to the event manager.
	*/
	inline CDosEventManager* EventManager() const;

	/**
	* Completes a request that is not auto-completed.
	* @param aMessage The message to be completed.
	* @param aResult Symbian error code for the completion.
	*/
	inline void RequestComplete(const RMessagePtr2 aMessage, TInt aResult) const;

private: //friend classes
	friend class CDosSession;

private:
	//Pointer to the DosServer's event manager.
	CDosEventManager* iEventManager;

};

#include "dosservice.inl"


#endif //__DOSSERVICE_H__
