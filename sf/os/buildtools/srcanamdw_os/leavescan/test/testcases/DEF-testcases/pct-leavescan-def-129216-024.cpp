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
* Description: 
*
*/
//desc:test warning message:Returns->returns
//option:
//date:2008-12-22 14:58:10
//author:bolowy
//type: CT

class a
{
int i;
LData func(int a)//check:return
{
	foo(); 
}
void func2(LData a)//check:uses
{
	foo();
}
LString func3(int a)//check:return
{
	foo(); 
}
void func4(LString a)//check:uses
{
	foo();
}

};

LData func5(int a)//check:return
{
	foo(); 
}
void func6(LData a)//check:uses
{
	foo();
}
LString func7(int a)//check:return
{
	foo(); 
}
void func8(LString a)//check:uses
{
	foo();
}

