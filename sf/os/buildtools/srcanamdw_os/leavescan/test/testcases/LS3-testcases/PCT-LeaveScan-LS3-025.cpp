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
//desc:test OR_LEAVE call in member specialised template function of a class with TRAP for LS3
//option:
//date:2008-8-12 15:58:1
//author:pingorliu
//type: CT


class temp
{
template<class T>
void func(TInt x)
{
	fxx();
}
template<>
void func<TInt>(TInt x)
{
	T a;
	TRAP(serv.Connect() OR_LEAVE);  //check:-func,-calls,-Leave
}
};
