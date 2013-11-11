// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// declaration of CAsynchEvent class
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file 
 @internalTechnology
*/

#include <e32base.h>

#ifndef _ASYNCHEVENT_H_
#define _ASYNCHEVENT_H_

class CStateMachine;
/**
 This is an abstract base for one asynchronous statemachine event

 The event can be part of the list built beforehand or on the fly.
 @see CStateMachine
 @internalTechnology

**/
class CAsynchEvent : public CBase
{
public:
   CAsynchEvent( CStateMachine* aStateMachine ) :
      iStateMachine( aStateMachine )
   {
   }

   virtual CAsynchEvent* ProcessL( TRequestStatus& aStatus ) = 0;

   void SetNext( CAsynchEvent* aNext );
   CAsynchEvent* Next() const;

protected:
   CAsynchEvent* iNext; //reference
   CStateMachine* iStateMachine; //reference
};

inline void CAsynchEvent::SetNext( CAsynchEvent* aNext )
/** 
 * Sets the next event
 *
 * @param aNext the next event to be set
 */
{
   iNext = aNext;
}

inline CAsynchEvent* CAsynchEvent::Next() const
/** 
 * Gets the next event in the chain
 *
 * @return the next event in the chain
 */
{
   return iNext;
}

#endif

