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
#ifndef VG_VGIBRIDGE_H
#define VG_VGIBRIDGE_H

#include <vg/vgcontext.h>

#ifdef __SYMBIAN32__
#include <vg/vgcontext_symbian.h>
#endif

typedef int (*VGIInitializeFunc)( int width, int height, VGIColorSpace colorSpace );
typedef int (*VGIInitializeExFunc)( int width, int height, VGIColorSpace colorSpace, int premultiplied, int conformant );
typedef int (*VGICopyToTargetFunc)( VGIColorBufferFormat format, int bufferStride, void *buffer, int maskStride, void *mask, VGICopyToTargetHint hint );
typedef void (*VGITerminateFunc)( void );
typedef int (*VGIResizeFunc)( int width, int height );
typedef int (*VGIBindToImageFunc)( VGImage image );
typedef int (*VGIUnBindImageFunc)( void );

#ifdef __SYMBIAN32__
typedef TInt (*VGISymbianInitializeFunc)( TSize aSize, VGIColorSpace aColorSpace );
typedef TInt (*VGISymbianInitializeExFunc)( TSize aSize, VGIColorSpace aColorSpace, TBool aPremultiplied, TBool aConformant );
typedef TInt (*VGISymbianCopyToBitmapFunc)( CFbsBitmap *aBitmap, CFbsBitmap *aMaskBitmap = NULL, VGICopyToTargetHint aHint = VGI_COPY_TRANSPARENT_PIXELS );
typedef void (*VGISymbianTerminateFunc)();
typedef TInt (*VGISymbianResizeFunc)( TSize aSize );
typedef TInt (*VGISymbianBindToImageFunc)( VGImage aImage );
typedef TInt (*VGISymbianUnBindImageFunc)();
#endif

typedef struct
{
    VGIInitializeFunc VGIInitialize;
    VGIInitializeExFunc VGIInitializeEx;
    VGICopyToTargetFunc VGICopyToTarget;
    VGITerminateFunc VGITerminate;
    VGIResizeFunc VGIResize;
    VGIBindToImageFunc VGIBindToImage;
    VGIUnBindImageFunc VGIUnBindImage;

#ifdef __SYMBIAN32__
    VGISymbianInitializeFunc VGISymbianInitialize;
    VGISymbianInitializeExFunc VGISymbianInitializeEx;
    VGISymbianCopyToBitmapFunc VGISymbianCopyToBitmap;
    VGISymbianTerminateFunc VGISymbianTerminate;
    VGISymbianResizeFunc VGISymbianResize;
    VGISymbianBindToImageFunc VGISymbianBindToImage;
    VGISymbianUnBindImageFunc VGISymbianUnBindImage;
    RLibrary libHandle;
#endif

} VGIBridge;

typedef void (*VGIBridgeFunc)( VGIBridge* functions );

#endif /* VG_VGIBRIDGE_H */
