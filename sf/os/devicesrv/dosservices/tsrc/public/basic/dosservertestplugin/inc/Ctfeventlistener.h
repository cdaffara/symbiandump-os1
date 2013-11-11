/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Definition of CTFDosServerEventListener
*
*/


#ifndef __CTFEVENTLISTENER_H__
#define __CTFEVENTLISTENER_H__

#include <DosSvrServices.h>



class CTFDosControl;

class CTFEventListener : public CDosEventListenerBase
    {
public:
    CTFEventListener( RDosServer& aServer );
    virtual ~CTFEventListener( void );

private:
    CTFEventListener( const CTFEventListener& aListener );
    CTFEventListener& operator=( const CTFEventListener& aListener );

protected:

	void GeneralEventL(const TRegisterEvent& aEvent,TAny* aParameter);

	/**
	* It notifies about a change in the status of the handsfree mode.
	* @param aStatus The new status.
	*/

private:
    };


#endif

// End of file
