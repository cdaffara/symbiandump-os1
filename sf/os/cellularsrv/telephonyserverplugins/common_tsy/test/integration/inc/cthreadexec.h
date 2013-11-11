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
// Contains the CThreadExec class declaration.
// 
//

/**
 @file 
 @internalTechnology
*/

#ifndef CTHREADEXEC_H
#define CTHREADEXEC_H

#include <e32base.h>
#include <etelmm.h>


class CThreadExec : public CBase
	{
public:
	class MFunctor
		{
	public:
		virtual void ExecL() =0;
		};

	template<class C>
	class TFunctor : public MFunctor
		{
	public:
		typedef void (C::*TFnPtr)(void);
		TFunctor(C* aObj, TFnPtr aFnPtr) :iObj(aObj), iFnPtr(aFnPtr) {};
		virtual void ExecL() {(iObj->*iFnPtr)();}
	protected:
		C*     iObj;
		TFnPtr iFnPtr;
		};

	template<class C, typename P1>
	class TFunctorP1 : public MFunctor
		{
	public:
		typedef void (C::*TFnPtr)(P1);
		TFunctorP1(C* aObj, TFnPtr aFnPtr, P1 aP1)  :iObj(aObj),iFnPtr(aFnPtr),iP1(aP1) {};
		virtual void ExecL() {(iObj->*iFnPtr)(iP1);}
	protected:
		C*     iObj;
		TFnPtr iFnPtr;
		P1 	   iP1;
		};

	template<class C, typename P1 , typename P2>
	class TFunctorP2 : public MFunctor
		{
	public:
		typedef void (C::*TFnPtr)(P1, P2);
		TFunctorP2(C* aObj, TFnPtr aFnPtr, P1 aP1, P2 aP2)  :iObj(aObj),iFnPtr(aFnPtr),iP1(aP1),iP2(aP2) {};
		virtual void ExecL() {(iObj->*iFnPtr)(iP1,iP2);}
	protected:
		C*     iObj;
		TFnPtr iFnPtr;
		P1 	   iP1;
		P2     iP2;
		};

	template<class C, typename P1 , typename P2, typename P3>
	class TFunctorP3 : public MFunctor
		{
	public:
		typedef void (C::*TFnPtr)(P1, P2, P3);
		TFunctorP3(C* aObj, TFnPtr aFnPtr, P1 aP1, P2 aP2, P3 aP3)  :iObj(aObj),iFnPtr(aFnPtr),iP1(aP1),iP2(aP2),iP3(aP3) {};
		virtual void ExecL() {(iObj->*iFnPtr)(iP1,iP2,iP3);}
	protected:
		C*     iObj;
		TFnPtr iFnPtr;
		P1 	   iP1;
		P2     iP2;
		P3     iP3;
		};

	template<class C, typename P1 , typename P2, typename P3, typename P4>
	class TFunctorP4 : public MFunctor
		{
	public:
		typedef void (C::*TFnPtr)(P1, P2, P3, P4);
		TFunctorP4(C* aObj, TFnPtr aFnPtr, P1 aP1, P2 aP2, P3 aP3, P4 aP4)  :iObj(aObj),iFnPtr(aFnPtr),iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4) {};
		virtual void ExecL() {(iObj->*iFnPtr)(iP1,iP2,iP3,iP4);}
	protected:
		C*     iObj;
		TFnPtr iFnPtr;
		P1 	   iP1;
		P2     iP2;
		P3     iP3;
		P4     iP4;
		};

	template<class C, typename P1 , typename P2, typename P3, typename P4, typename P5>
	class TFunctorP5 : public MFunctor
		{
	public:
		typedef void (C::*TFnPtr)(P1, P2, P3, P4, P5);
		TFunctorP5(C* aObj, TFnPtr aFnPtr, P1 aP1, P2 aP2, P3 aP3, P4 aP4, P5 aP5) : iObj(aObj),iFnPtr(aFnPtr),iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4),iP5(aP5) {};
		virtual void ExecL() {(iObj->*iFnPtr)(iP1,iP2,iP3,iP4,iP5);}
	protected:
		C*     iObj;
		TFnPtr iFnPtr;
		P1 	   iP1;
		P2     iP2;
		P3     iP3;
		P4     iP4;
		P5     iP5;
		};


		
public:
	static CThreadExec* NewL();
	virtual ~CThreadExec();
	
public: 
	TInt Exec(MFunctor* aFunction);	
	void AsyncExec(MFunctor* aFunction);
		
private:
	/**
	Active object to execute a function in the thread the PhoneManager is created
	*/
	class CActiveFnExec: public CActive
		{
	public:
		static CActiveFnExec* NewL(CThreadExec& aOwner);
	public:
		void Start();
		void Exec();
	public: // from CActive
		void RunL();
		void DoCancel();
	private:
		CActiveFnExec(CThreadExec& aOwner);
	private:
		CThreadExec& iOwner;
		RThread iThread;
		};
private:
	CThreadExec();
	void ConstructL();

private:
	void DoExec();

private:
	CActiveFnExec *iActiveFnExec;		
	MFunctor* iFunction;	
	RSemaphore iExecSemaphore;	
	TInt iLatestError;	
	TBool iNeedSignal;
	};

#endif
