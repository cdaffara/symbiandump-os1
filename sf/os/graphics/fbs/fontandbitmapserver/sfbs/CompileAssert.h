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
//

#ifndef __COMPILEASSERT_H__
#define __COMPILEASSERT_H__

template<TBool> class __CompileTimeAssert 
	{
	public: 
		inline static void Dummy() 
			{
			}
	};

TEMPLATE_SPECIALIZATION class __CompileTimeAssert<0> 
	{
	private: 
		inline static void Dummy() 
			{
			}
	};

#define COMPILE_TIME_ASSERT(aCondition)		\
	{										\
	const TBool condition=aCondition;		\
	__CompileTimeAssert<condition>::Dummy();\
	}

#endif// __COMPILEASSERT_H__
