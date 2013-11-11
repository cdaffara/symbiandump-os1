// DynamicBuffer.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#if !defined(AFX_DYNAMICBUFFER_H__FC5FE4BC_8299_4843_BC15_E5B01611B9E2__INCLUDED_)
#define AFX_DYNAMICBUFFER_H__FC5FE4BC_8299_4843_BC15_E5B01611B9E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CDynamicReadBuffer  
	{
public:
	static CDynamicReadBuffer* New(int aInitialLength);
	virtual ~CDynamicReadBuffer();
	const char* Buffer() const;
	char* Buffer(int aLength);
private:
	CDynamicReadBuffer();
	void Construct(int aInitialLength);
	void GrowBuf(int aNewLength);
private:
	char* iBuf;
	int iMaxBufLength;
	};


class CDynamicWriteBuffer  
	{
public:
	static CDynamicWriteBuffer* New(int aInitialLength);
	virtual ~CDynamicWriteBuffer();
	void Add(char* aBuf, int aLength);
	void Remove(int aLength);
	const char* Buffer() const;
	int Length() const;
private:
	CDynamicWriteBuffer();
	void Construct(int aInitialLength);
	void GrowBuf(int aNewLength);
private:
	char* iBuf;
	int iMaxBufLength;
	int iCurrentBufLength;
	};

#endif // !defined(AFX_DYNAMICBUFFER_H__FC5FE4BC_8299_4843_BC15_E5B01611B9E2__INCLUDED_)
