// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef OMX_COMPONENTIMPL_H
#define OMX_COMPONENTIMPL_H

#include <e32msgqueue.h>

class CMMFBuffer;

class COmxBufferManager : public CBase
	{
public:

	class COmxBuffer : public CBase
		{
	public:
		static COmxBuffer* NewL(OMX_BUFFERHEADERTYPE* aBufferHeader, CMMFBuffer* aBuffer);
		~COmxBuffer();
		
		CMMFBuffer* MmfBuffer() const;
		OMX_BUFFERHEADERTYPE* BufferHeader() const;
		
		void SetInputPortCallbacks(MOmxInputPortCallbacks* aCallbacks);
		void SetOutputPortCallbacks(MOmxOutputPortCallbacks* aCallbacks);
		
		MOmxInputPortCallbacks* InputPortCallbacks() const;
		MOmxOutputPortCallbacks* OutputPortCallbacks() const;
		
	private:
		COmxBuffer();
		void ConstructL(OMX_BUFFERHEADERTYPE* aBufferHeader, CMMFBuffer* aBuffer);
		
		OMX_BUFFERHEADERTYPE* iBufferHeader;
		CMMFBuffer* iMmfBuffer;
		
		TBool iOwnsMmfBuffer;
		MOmxInputPortCallbacks* iInputPortCallbacks;
		MOmxOutputPortCallbacks* iOutputPortCallbacks;
		};
		
public:
	COmxBufferManager(OMX_COMPONENTTYPE* aHandle);
	~COmxBufferManager();
	
	// Buffer lookup
	COmxBuffer* FindBuffer(const CMMFBuffer* aBuffer) const;
	COmxBuffer* FindBuffer(OMX_BUFFERHEADERTYPE* aBuffer) const;
	
	// OMX Calls
	TInt UseBuffer(CMMFBuffer& aBuffer, TUint nPortIndex);
	CMMFBuffer* AllocateBufferL(TUint nPortIndex, TUint nSizeBytes);
	TInt FreeBuffer(CMMFBuffer* aBuffer);
	
	TInt EmptyThisBuffer(const CMMFBuffer* aBuffer, MOmxInputPortCallbacks* aObserver);
	TInt FillThisBuffer(CMMFBuffer* aBuffer, MOmxOutputPortCallbacks* aObserver);
	
	// Store OMX buffer pointer
	void StoreBufferL(OMX_BUFFERHEADERTYPE* aBufferHeader, CMMFBuffer* aBuffer);
	
	
private:	
	RPointerArray<COmxBuffer> iBuffers;
	OMX_COMPONENTTYPE* iHandle;
	};

class COmxCallbacks : public CActive
	{
public:
	enum TMessageType 
		{
		EFillBufferCallback,
		EEmptyBufferCallback,
		EEventCallback
		};	

	class TEventParams
		{
	public:		
	 	OMX_EVENTTYPE iEvent; 
		TUint iData1;
		TUint iData2;
		TAny* iExtra;
		};

	class TOmxMessage
		{
	public:
		TMessageType iType;
		OMX_HANDLETYPE iComponent;
		union 
			{
			COmxBufferManager::COmxBuffer* iBuffer;
			TEventParams iEventParams;
			};	
		};
public:
	static COmxCallbacks* NewL(MOmxPuCallbacks& aPuCallbacks);
	operator OMX_CALLBACKTYPE*();
	void RunL();
	void DoCancel();
	TInt FillBufferDone(OMX_HANDLETYPE aComponent, COmxBufferManager::COmxBuffer* aBuffer);
	TInt EmptyBufferDone(OMX_HANDLETYPE aComponent, COmxBufferManager::COmxBuffer* aBuffer);
	TInt EventHandler(OMX_HANDLETYPE aComponent, TEventParams aParams);
	virtual ~COmxCallbacks();
private:
	void ConstructL();
	COmxCallbacks(MOmxPuCallbacks& aPuCallbacks);
	int iState;
	MOmxPuCallbacks* iPuCallbacks;
	RMsgQueue<TOmxMessage> iMsgQueue;
	OMX_CALLBACKTYPE iHandle;
	};

#endif // OMX_COMPONENTIMPL_H
