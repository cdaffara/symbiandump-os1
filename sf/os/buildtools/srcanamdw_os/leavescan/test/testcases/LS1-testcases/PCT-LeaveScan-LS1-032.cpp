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
//desc:test leave function call in override member function for LS1
//option:
//date:2008-8-12 15:58:1
//author:pingorliu
//type: CT

class base
{
	void func(TInt x)
	{
	}
	void func()
	{
	}

};
class temp:public base
{
void func(TInt x)
{
	fxx();
}
void func()
{
	CL a;
	fooL(); //check:func,leave
}
};
