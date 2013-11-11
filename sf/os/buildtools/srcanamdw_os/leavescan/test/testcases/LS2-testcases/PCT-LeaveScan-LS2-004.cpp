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
//desc:test LC LD LX for LS2
//option:
//date:2008-8-12 15:58:1
//author:pingorliu
//type: CT

void funcL()
{//check:-funcL,-leavers

/*fooLC();*/

/*
 *
 * fooLC();
 *
*/	

//this is a function call fooLD();

//"this is another function call fooLX();"

string str = "calling fooLC()";
string str2 = "\"calling fooLD()\"";
string str3 = "\"calling fooLX()";
string str4 = "'calling fooLC()'";
string str5 = "'calling fooLD()";
string str6 = " this is a function\
                fooLX()";

MARCOFOOLLLD();

fooLDD();
}

