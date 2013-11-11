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
// This is the header file for ESOCK basic socket test section 1
// 
//


#if (!defined __SOCKETTEST_1_H__)
#define __SOCKETTEST_1_H__

#include "TestStepSocket.h"

class CSocketTest1_1 : public CSocketTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest1_2 : public CSocketTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest1_3 : public CSocketTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest1_4 : public CSocketTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest1_5 : public CSocketTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest1_6 : public CSocketTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest1_7 : public CSocketTestStep_OOMCapable
	{
	public:
		CSocketTest1_7()
		: iNormalTest(ETrue)
			{
			}
		static const TDesC &GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	private:
		TBool iNormalTest;
	};

class CSocketTest1_8 : public CSocketTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest1_9 : public CSocketTestStep_OOMCapable
	{
	public:
		static const TDesC &GetTestName();
		virtual enum TVerdict InternalDoTestStepL( void );
	};

class CSocketTest1_10 : public CSocketTestStep_OOMCapable
    {
    public:
        static const TDesC &GetTestName();
        virtual enum TVerdict InternalDoTestStepL( void );
    };

#endif //__SOCKETTEST_1_H__


