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
//desc: test LCleanedup class used with Classic cleanup PushL in member function that defined out of class for LS15
//option:
//date:2008-8-21 15:19:15
//author:pingorliu
//type: CT

template<class T>
class temp
{
	public:
void funcL();
};
template<class T>
void temp::funcL<T>()
{
LCleanedupHandle<RBar> bar;
CFoo* foo2 = CFoo::NewL(); 
CleanupStack::PushL(foo2); //check:with,LCleaned


}
	
