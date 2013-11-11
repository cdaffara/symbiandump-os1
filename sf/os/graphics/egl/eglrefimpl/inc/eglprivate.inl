// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Reference EGL implementation to support EGL sync objects and OpenWF extensions

// XEglPls inline functions
//
inline void XEglPls::Lock()
    {
    iLock.Wait();
    }

inline void XEglPls::Unlock()
    {
    iLock.Signal();
    }

// CEglDriver inline functions
//
inline void CEglDriver::Lock()
    {
    iLock.Wait();
    }

inline void CEglDriver::Unlock()
    {
    iLock.Signal();
    }

inline TBool CEglDriver::IsDisplayValid(EGLDisplay aDisplay) const
    {
    return (iDisplay && iDisplay->Handle() == aDisplay);
    }

inline CEglDisplay* CEglDriver::FindDisplay(EGLDisplay aDisplay) const
    {
    return IsDisplayValid(aDisplay) ? iDisplay : NULL;
    }

#ifdef _DEBUG
inline RHeap& CEglDriver::Heap() const
    {
    return iHeap;
    }
#endif

// CEglDisplay inline functions
//
inline TBool CEglDisplay::IsInitialized() const
    {
    return iIsInitialized;
    }

inline RHeap& CEglDisplay::Heap() const
    {
    return iHeap;
    }

inline EGLDisplay CEglDisplay::Handle() const
    {
    return iHandle;
    }

// CEglSync inline functions
//
inline void CEglSync::Open()
    {
    ++iRefCount;
    }

inline TBool CEglSync::IsDestroyed() const
    {
    return iIsDestroyed;
    }

inline EGLenum CEglSync::Type() const
    {
    return iType;
    }

inline EGLenum CEglSync::Status() const
    {
    return iStatus;
    }
