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
//desc: test LCleanedup is used to declare a data member of a specialised class
//option:
//date:2008-8-20 13:51:31
//author:pingorliu
//type: CT

template<class T>
class base
{
};
template<class TT>
struct temp:private base<TInt>
{
	private:
                
} mystruct;

template<>
struct temp<TInt>:private base<TInt>
{
	private:
		static LCleanedupPtr<TT> member; //check:LCleanedup,temp<TInt>

};
