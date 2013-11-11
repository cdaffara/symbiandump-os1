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
//desc:test LC LD LX of LS1
//option:
//date:2008-8-12 15:58:1
//author:pingorliu
//type: CT

void func()
{
/*fooLC();*/ //check:-func

/*
 *
 * fooLC();  //check:-func
 *
*/	

//this is a function call fooLD(); //check:-func

//"this is another function call fooLX();" //check:-func

string str = "calling fooLC()"; //check:-func
string str2 = "\"calling fooLD()\""; //check:-func
string str3 = "\"calling fooLX()"; //check:-func
string str4 = "'calling fooLC()'"; //check:-func
string str5 = "'calling fooLD()"; //check:-func
string str6 = " this is a function\
                fooLX()";  //check:-func

MARCOFOOLLLD(); //check:func

fooLDD(); //check:-func
}

