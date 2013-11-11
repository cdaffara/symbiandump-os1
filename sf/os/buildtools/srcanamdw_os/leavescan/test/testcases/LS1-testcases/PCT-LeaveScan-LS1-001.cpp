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
//desc:test comment and string and marco of LS1
//option:
//date:2008-8-12 15:58:1
//author:pingorliu
//type: CT

void func()
{
/*fooL();*/ //check:-func

/*
 *
 * fooL();  //check:-func
 *
*/	

//this is a function call fooL(); //check:-func

//"this is another function call fooL();" //check:-func

string str = "calling fooL()"; //check:-func
string str2 = "\"calling fooL()\""; //check:-func
string str3 = "\"calling fooL()"; //check:-func
string str4 = "'calling fooL()'"; //check:-func
string str5 = "'calling fooL()"; //check:-func
string str6 = " this is a function\
                fooL()";  //check:-func

MARCOFOOL(); //check:func,macro

int b = 1;
fx(/*fooL()*/ b); //check:-func
fx(/*                                        fooL()//        */ b); //check:-func
}

