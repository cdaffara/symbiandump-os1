// dobject_compat.h
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef DOBJECT_COMPAT_H
#define DOBJECT_COMPAT_H

#include <kernel.h>

__ASSERT_COMPILE(sizeof(DObject) == 6*4 || sizeof(DObject) == 8*4); // We can't cope if the size changes again!
#define DOBJECT_PADDING TUint64 _padding
const TBool KCompiledUsingOldDefinition = sizeof(DObject) == 6*4;
#define ADD_PADDING(type, variablePtr) (*reinterpret_cast<type*>((TUint8*)variablePtr + sizeof(TUint64)))
#define SUBTRACT_PADDING(type, variablePtr) (*reinterpret_cast<type*>((TUint8*)variablePtr - sizeof(TUint64)))


template <class T>
class TemplatedPadder
	{
public:
	inline static T& AddPadding(T* aPtr)
		{
		return ADD_PADDING(T, aPtr);
		}
	inline static T& SubtractPadding(T* aPtr)
		{
		return SUBTRACT_PADDING(T, aPtr);
		}
	};

template <class T>
inline T& AddPadding(T* aPtr)
	{
	return TemplatedPadder<T>::AddPadding(aPtr);
	}

template <class T>
inline T& SubtractPadding(T* aPtr)
	{
	return TemplatedPadder<T>::SubtractPadding(aPtr);
	}

TBool CalculateDObjectSize(); // returns false if unable to determine 
extern TBool gRunningWithOldDefinition;

/* The SAFE_MEMBER macro exists because DObject grew by 64 bits between 9.1 and 9.2, therefore to write a 
 * DObject-derived class (such as a DLogicalChannel in an LDD) that can be compiled on one but also works on the
 * other requires that you be very careful accessing member data of your DObject-derived objects, because the 
 * offsets are different on the two OS releases. This macro abstracts the differences away by calculating when
 * an 8 byte correction needs to be applied to where the compiler thinks the variable is versus where the runtime
 * knows it is. By necessity, it has a weird definition, that relies on the above helper macros and also on
 * CalculateDObjectSize being called at some point before the first time any code using SAFE_MACRO is run.
 * It is recommended that for an LDD, you call it from within your DECLARE_EXTENSION_LDD before doing anything
 * else.
 * 
 * SAFE_MEMBER can only be used to access member data of DObject-derived classes, and should be used in ALL 
 * places where the instance variables are used or set, this includes instance variables of the superclass and
 * instance variables in DObject-derived objects you define yourself. Accesses to instance variables of DObject
 * itself must NOT use this macro, since the DObject variable offsets do not change. In order to make sure your
 * class is large enough and that the macro can be used consistantly, you must include a DOBJECT_PADDING at 
 * the *end* of all of your DObject subclass definition. You won't get any warning if you forget to do this,
 * It will just crash. Likewise if you use SAFE_MEMBER to access the member data of the DObject itself.
 * 
 * In other words:
 * DO use SAFE_MEMBER to access the member data of DMyClassThatIsDerivedFromDObject
 * DO use SAFE_MEMBER to access the member data of DLogicalChannel/DLogicalDevice/DThread etc
 * DON'T use it to access the member data of DObject itself
 * ALWAYS include a DOBJECT_PADDING at the end of your DMyDObjectDerivedClass - ie after any other member data
 * 
 */

#define SAFE_MEMBER(variable) ( \
	(KCompiledUsingOldDefinition && gRunningWithOldDefinition) ? variable : ( \
	(KCompiledUsingOldDefinition && !gRunningWithOldDefinition) ? AddPadding(&variable) : ( \
	(!KCompiledUsingOldDefinition && gRunningWithOldDefinition) ? SubtractPadding(&variable) : ( \
	/* running and compiling with new-definition DObject */ variable ))))

/* Old definition that doesn't require templating to work, (so is callable from non-C++). A consequence is you
 * have to explicitly give the type of the variable.
 */
//#define SAFE_MEMBER(type, variable) ( \
//	(KCompiledUsingOldDefinition && gRunningWithOldDefinition) ? variable : ( \
//	(KCompiledUsingOldDefinition && !gRunningWithOldDefinition) ? ADD_PADDING(type, &variable) : ( \
//	(!KCompiledUsingOldDefinition && gRunningWithOldDefinition) ? SUBTRACT_PADDING(type, &variable) : ( \
//	/* running and compiling with new-definition DObject */ variable ))))


/* These macros are equivalent to _FOFF and _LOFF and should be used whenever
 * f refers to a member variable that you would need to use SAFE_MEMBER on
 */
#define _SAFE_FOFF(c,f)			(((TInt)&  SAFE_MEMBER((((c *)0x1000)->f) )  )-0x1000)
#define _SAFE_LSOFF(p,T,f) ((T*)(((TUint8*)(p))-_SAFE_FOFF(T,f)))

#endif
