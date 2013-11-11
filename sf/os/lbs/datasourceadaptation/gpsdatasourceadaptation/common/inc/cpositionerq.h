// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @InternalComponent
*/

#ifndef CPOSITIONERQ_H
#define CPOSITIONERQ_H

#include <e32base.h>

class MPositioner;
class CRequestHandler;
class CResponseHandler;
class CModuleStatusHandler;
class CPositionerEnvironment;
class TPositionerCall;
class TRetBoolPositionerCall;
class TRetTUintPositionerCall;

/**
* CPositionerQ controls access to the queue of MPositioner objects
*/
class CPositionerQ : public CBase
    {
    public:
        static CPositionerQ* NewL();
        ~CPositionerQ();
        
        void RegisterPSYL(MPositioner* aPSY);
        void UnregisterPSY(MPositioner* aPSY);
        void PositionerIterator(TPositionerCall& aCall);
		TUint IterGreatest(TRetTUintPositionerCall& aCall);
		TBool IterETrue(TRetBoolPositionerCall& aCall);
        TUint Count();
        
		TInt Index(MPositioner& aPSY);
        MPositioner& Positioner(TUint aIndex);

		/**
		* Request panic code
		*/
		enum TRequestQPanic
			{
			ERequestQIndexToLarge = 0,
			ERequestQItemNotFound = 1,
			};
		
	private:
        CPositionerQ();
        
		TInt FindPsy(MPositioner* aPSY);

    private:
        RArray<MPositioner*> iPsyArray;
    };

#endif // CPOSITIONERQ_H

// End of File
