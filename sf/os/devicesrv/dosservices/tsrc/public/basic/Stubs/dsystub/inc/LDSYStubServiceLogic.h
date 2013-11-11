/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of CLDSYStubServiceLogic class
*
*/


#ifndef LDSYSTUBSERVICELOGIC_H
#define LDSYSTUBSERVICELOGIC_H

// INCLUDES
#include <e32base.h>
#include "LDSYServiceLogic.h"
#include <DosExtensionBase.h>


// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CCDSYController;
class TCDSYMessage;
class CLDSYStubServiceLogic;

NONSHARABLE_CLASS( CDSYTimer ) : public CActive
  	{
public:
		// Construction
	static CDSYTimer* NewLC( CLDSYStubServiceLogic* aCLDSYStubServiceLogic );
	
		// From CActive
	void RunL();
	
	void DoCancel();
		
	void StartEventTimer();
	
	~CDSYTimer();
private:
		// C++ constructor - not exported;
		// implicitly called from NewLC()
	CDSYTimer( CLDSYStubServiceLogic* aCLDSYStubServiceLogic );
		// 2nd phase construction, called by NewLC()
	void ConstructL(); // second-phase constructor
private:
	RTimer iTimer;
	CLDSYStubServiceLogic* iCLDSYStubServiceLogic;
	};
	
/**
*  CLDSYStubServiceLogic
*  This class declares the CLDSYStubServiceLogic.
*/
class CLDSYStubServiceLogic : public CBase, public TLDSYServiceLogic, public MDosExtensionBaseDSY
    {
    public:
        /**
        * Destructor.
        */
        virtual ~CLDSYStubServiceLogic();

        /**
        * Creates a new StubServiceLogic
        * @param CCDSYController& aCDSYController       The common DSY message controller.
        * @return CLDSYServiceLogicFactory*             The new service logic.
        */
        static CLDSYStubServiceLogic* NewL( CCDSYController& aCDSYController );
        
        void SendEventL();

    private:
        /**
        * Constructor.
        * @param CCDSYController& aCDSYController       The common DSY message controller.
        * @return 
        */
        CLDSYStubServiceLogic( CCDSYController& aCDSYController );

        /**
        * 2nd phase constructor.
        * @return void 
        */
        void ConstructL();

        /**
        * Processes a Common DSY message
        * @param TCDSYMessage& aCDSYMessage             Reference to TCDSYMessage.
        * @return void 
        */
        virtual void ProcessCDSYMessageL( TCDSYMessage& aCDSYMessage );
                
        /**
        * Implementation of the DSY Extension interface function.
        * @param aFunc                  Function ID
        * @param aParameter             Parameter(s) to the called function
        * @param aParLength             Length of the parameter data
        * @param aParameterModified     Flag returning boolean status of the parameter modification
        * @return KErrNone or KErrGeneral
        */
        TInt CallFunctionL( TInt aFunc, TAny* aParameter, TInt aParLength, TBool& aParameterModified );

        /**
        * Implementation of the automatically completed DSY extension function
        * @param aFunc                  Function ID
        * @param aParameter             Parameter(s) to the called function
        * @param aParLength             Length of the parameter data
        * @param aMessage               Rmessage reference used to complete the client message.
        * @return void 
        */
        void CallFunctionAndCompleteL( TInt aFunc, TAny* aParameter, TInt aParLength, const RMessage2& aMessage );
                        
                
        TInt iValue;
        
        CDSYTimer* iEventTimer;
    };



#endif //#ifndef LDSYSTUBSERVICELOGIC_H



//  End of File