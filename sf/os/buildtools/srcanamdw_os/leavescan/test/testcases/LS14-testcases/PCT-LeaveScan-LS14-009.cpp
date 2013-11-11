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
//desc:test LData is used as a parameter of a non-leaving friend function of a class for LS14
//option:
//date:2008-8-21 14:10:2
//author:pingorliu
//type: CT

class temp
{
	public:
friend  void func(LData x); //check:func,parameter
};

void func(LData x) //check:func,parameter
{
}
