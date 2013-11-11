/**
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
* TRpsFunctor declaration
* 
*
*/



/**
 @file
*/
 
//Base class for the functor's.  This defines a couple of pure virtual functions that are overriden by the templated functor class.  
//We need this base class otherwise passing the templated class around becomes more difficult.
#ifndef TRPSFUNCTOR_H
#define TRPSFUNCTOR_H

#include <e32base.h>
#include "rpsasciirqstdefs.h"
#include "cctsyintegrationtestsuitepanics.h"



class MRpsFunctorBase
	{
public:
	virtual void ExecuteRpsRequestL() = 0;
	virtual const TDesC& DisplayToClientTxt() = 0;
	
	typedef TUint8 TNotUsed;
	};

class CRPSMaster;
//Templates Functor class.  This holds a pointer to a CRPSMaster member function and the parameters for that function. 
//It is templated so that any function of CRPSMaster can be invoked through this class and also so that it can support function prototypes that take up to 3 parameters.
template <class TRpsParam1, class TRpsParam2, class TRpsParam3> 
class TRpsFunctor : public MRpsFunctorBase
   {
public:  

   	typedef void (CRPSMaster::*TRpsWith1ParamFptr)(TRpsParam1 aParam1);   // typedef pointer to a CRPSMaster member function which takes 1 param
	typedef void (CRPSMaster::*TRpsWith2ParamFptr)(TRpsParam1 aParam1, TRpsParam2 aParam2);   // typedef pointer to a CRPSMaster member function which takes 2 param
	typedef void (CRPSMaster::*TRpsWith3ParamFptr)(TRpsParam1 aParam1, TRpsParam2 aParam2, TRpsParam3 aParam3);   // typedef pointer to a CRPSMaster member function which takes 3 param
	
public: //overloads of constructors taking 1, 2 or 3 paramters
	TRpsFunctor(CRPSMaster* aRpsMaster, const TDesC& aDisplayToClientTxt, TEtelLine aLine, TRpsWith1ParamFptr aRpsFptr, TRpsParam1 aRpsParam1);
	TRpsFunctor(CRPSMaster* aRpsMaster, const TDesC& aDisplayToClientTxt, TEtelLine aLine, TRpsWith2ParamFptr aRpsFptr, TRpsParam1 aRpsParam1, TRpsParam2 aRpsParam2);
	TRpsFunctor(CRPSMaster* aRpsMaster, const TDesC& aDisplayToClientTxt, TEtelLine aLine, TRpsWith3ParamFptr aRpsFptr, TRpsParam1 aRpsParam1, TRpsParam2 aRpsParam2, TRpsParam3 aRpsParam3);
	
public: //from MRpsFunctorBase
	void ExecuteRpsRequestL();
	const TDesC& DisplayToClientTxt();

protected:
	void SetDisplayToClientTxt(const TDesC& aDisplayToClientTxt, TEtelLine aLine);
	
private:
    enum TRpsCallType
    	{
    	EZeroParam,
    	EOneParam,
    	ETwoParam,
    	EThreeParam
    	};
    TRpsWith1ParamFptr iRpsFptrWith1Param;
    TRpsWith2ParamFptr iRpsFptrWith2Param;
    TRpsWith3ParamFptr iRpsFptrWith3Param;
    CRPSMaster* iRpsMaster;               
    TRpsParam1 iRpsParam1;	//1st rps param
    TRpsParam2 iRpsParam2;  //2nd rps param
    TRpsParam3 iRpsParam3;  //3rd rps param
    TRpsCallType iRpsCallType; //how many params does the RPS call take - 1,2 or 3
    TBuf<255> iDisplayToClientTxt;
 	};
 #endif //TRPSFUNCTOR_H
