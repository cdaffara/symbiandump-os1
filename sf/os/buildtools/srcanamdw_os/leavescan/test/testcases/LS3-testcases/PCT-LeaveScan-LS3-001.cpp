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
//desc:test new(ELeave) and User::Leave of LS3
//option:
//date:2008-8-12 15:58:1
//author:pingorliu
//type: CT

void func()
{
/*new(ELeave)CL();*/ //check:-func,-calls

/*
 *
 * new(ELeave)CL();  //check:-func,-calls
 *
*/	

new(ELeavet)CL(); //check:func,ELeave

//this is a function call User::Leave(); //check:-func,-calls
//this is a function call User::LeaveIfError(); //check:-func,-calls
//this is a function call User::LeaveNoMemory(); //check:-func,-calls
//this is a function call User::LeaveIfError(); //check:-func,-calls


string str = "calling new(ELeave)CL()"; //check:-func,-ELeave
string str2 = "\"calling User::Leave()\""; //check:-func,-calls
string str3 = "\"calling User::LeaveIfError()"; //check:-func,-calls
string str4 = "'calling ELeave'"; //check:-func,-calls
string str5 = "'calling Leave()"; //check:-func,-calls
string str6 = " this is a function\
                User::Leave()";  //check:-func,-calls

int ELeavet = 1; //check:-func,-calls
MARCOELEAVE(); //check:-func,-calls

}
