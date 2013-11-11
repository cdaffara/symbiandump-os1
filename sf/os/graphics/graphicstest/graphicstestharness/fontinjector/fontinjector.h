// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The FontInjector DLL is a utility that allows test code to intercept any
// virtual function calls to CFont or COpenFont objects. It is used to detect
// possible virtual function calls to objects in the shared heap of the Font
// and Bitmap Server from a client process. Currently, such detection only
// works on the emulator, where code in a DLL that has been loaded by one
// process can effectively be executed from any other process.
//

/**
@file
@internalTechnology
@test
*/

#ifndef FONTINJECTOR_H
#define FONTINJECTOR_H

#ifdef __WINS__

#include <e32std.h>

class CFont;
class COpenFont;

class MShellcode
	{
public:
	virtual void ExecuteShellcode(TInt aFromFunction) = 0;
	};

template<class C> class XVtableInjector
	{
public:
	IMPORT_C XVtableInjector();
	IMPORT_C ~XVtableInjector();
	IMPORT_C TInt InjectShellcode(C* aObject, MShellcode* aShellcode);
	IMPORT_C static void GetVirtualFunctionName(TInt aIndex, TDes& aName);
private:
	static void ObjectDestroyed(TAny* aObject);
	virtual void __declspec(naked) Function000();
	virtual void __declspec(naked) Function001();
	virtual void __declspec(naked) Function002();
	virtual void __declspec(naked) Function003();
	virtual void __declspec(naked) Function004();
	virtual void __declspec(naked) Function005();
	virtual void __declspec(naked) Function006();
	virtual void __declspec(naked) Function007();
	virtual void __declspec(naked) Function008();
	virtual void __declspec(naked) Function009();
	virtual void __declspec(naked) Function010();
	virtual void __declspec(naked) Function011();
	virtual void __declspec(naked) Function012();
	virtual void __declspec(naked) Function013();
	virtual void __declspec(naked) Function014();
	virtual void __declspec(naked) Function015();
	virtual void __declspec(naked) Function016();
	virtual void __declspec(naked) Function017();
	virtual void __declspec(naked) Function018();
	virtual void __declspec(naked) Function019();
	virtual void __declspec(naked) Function020();
	virtual void __declspec(naked) Function021();
	virtual void __declspec(naked) Function022();
	virtual void __declspec(naked) Function023();
	virtual void __declspec(naked) Function024();
	virtual void __declspec(naked) Function025();
	virtual void __declspec(naked) Function026();
	virtual void __declspec(naked) Function027();
	virtual void __declspec(naked) Function028();
	virtual void __declspec(naked) Function029();
	virtual void __declspec(naked) Function030();
	virtual void __declspec(naked) Function031();
private:
	static RPointerArray<TAny> gObjects;
	static MShellcode* gShellcode;
	static TAny* gOriginalVtable;
	};

typedef XVtableInjector<CFont> XFontInjector;
typedef XVtableInjector<COpenFont> XOpenFontInjector;

#endif // __WINS__

#endif // FONTINJECTOR_H
