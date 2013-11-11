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
//desc:test warning message:Uses->uses
//option:
//date:2008-12-22 14:58:10
//author:bolowy
//type: CT

void func()
{
	LCleanedup a; 
	CClass::NewLC();//check:uses
}

void func2()
{
	TRAP(CClass::NewLC());//check:uses
	LCleanedup a; 
}

void func3LC()
{
	LCleanedup a; 
	CClass::NewLC();//check:uses
}

void func4LC()
{
	LCleanedup a;
	CClass::NewLC();//check:uses
}



