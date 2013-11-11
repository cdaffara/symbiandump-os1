// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if (!defined __SOCKETTESTSECTIONS_H__)
#define __SOCKETTESTSECTIONS_H__

#include "TE_TestStepSocketServer.h"

class CSocketServerTest1 : public CSocketServerTestStep_OOMCapable
	{
	public:
		static const TDesC& GetTestName();
		CSocketServerTest1();
		~CSocketServerTest1();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketServerTest2 : public CSocketServerTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest2();
		~CSocketServerTest2();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketServerTest3 : public CSocketServerTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest3();
		~CSocketServerTest3();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};


class CSocketServerTest5 : public CSocketServerTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest5();
		~CSocketServerTest5();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketServerTest7 : public CSocketServerTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest7();
		~CSocketServerTest7();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketServerTest9 : public CSocketServerTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest9();
		~CSocketServerTest9();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketServerTest16 : public CSocketServerTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest16();
		~CSocketServerTest16();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketServerTest17 : public CTestStepSocketServer
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest17();
		~CSocketServerTest17();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};
	
	
class CSocketServerTest18 : public CTestStepSocketServer
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest18();
		~CSocketServerTest18();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};



class CSocketServerTest20 : public CTestStepSocketServer
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest20();
		~CSocketServerTest20();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};
	
class CSocketServerTest21 : public CTestStepSocketServer
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest21();
		~CSocketServerTest21();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};

		
class CSocketServerTest22 : public CTestStepSocketServer
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest22();
		~CSocketServerTest22();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};


class CSocketServerTest23 : public CTestStepSocketServer
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest23();
		~CSocketServerTest23();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketServerTest24 : public CTestStepSocketServer
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest24();
		~CSocketServerTest24();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};

		
class CSocketServerTest25 : public CTestStepSocketServer
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest25();
		~CSocketServerTest25();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};


class CSocketServerTest26 : public CTestStepSocketServer
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest26();
		~CSocketServerTest26();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};


class CSocketServerTest27 : public CTestStepSocketServer
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest27();
		~CSocketServerTest27();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};
	
class CSocketServerTest28 : public CTestStepSocketServer
	{
	public:
		static const TDesC &GetTestName();
		CSocketServerTest28();
		~CSocketServerTest28();
		
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketServerTest29 : public CTestStepSocketServer
/**
* Security attack test
* Attack server by sending invalid message, e.g. containing illegal arguments.
* @see PDEF099357
*/
    {
    public:
        static const TDesC &GetTestName();
        virtual enum TVerdict InternalDoTestStepL( void );
    private:
        static TInt PanicTestThread(TAny*);
    };

#endif //(__SOCKETTESTSECTIONS_H__)

