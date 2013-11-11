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
//desc:test TRAP and TRAPD of LS1
//option:
//date:2008-8-12 15:58:1
//author:pingorliu
//type: CT

void func()
{
	TRAP(fooL()); //check:-func,-leave
	TRAP(fooLC()); //check:-func,-leave
	TRAP(                                    fooL()   ); //check:-func,-leave

        TRAP(/*this is a function call */ fooL() /*hello*/    ); //check:-func,-leave
	

	TRAPD(result,fooL()  ); //check:-func,-leave

}
