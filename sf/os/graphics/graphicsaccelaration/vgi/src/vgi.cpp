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
#include "vg_vgibridge.h"
#include <stdlib.h>
#include <string.h>
#include <VG/openvg.h>

#define VGI_API_CALL __declspec(dllexport)

#define VGI_GET_BRIDGE      VGIBridge *bridge = vgiGetBridge(); if(!bridge) return; 
#define VGI_GET_BRIDGE_RET  VGIBridge *bridge = vgiGetBridge(); if(!bridge) return 0; 


#if defined __SYMBIAN32__

static void vgiSetCurrentBridge( VGIBridge* bridge )
{
    Dll::SetTls( bridge );
}

static VGIBridge* vgiGetCurrentBridge()
{
    return (VGIBridge*)Dll::Tls();
}

#elif defined WIN32

#include <windows.h>

VGIBridge *currBridge = NULL;

static void vgiSetCurrentBridge( VGIBridge* bridge )
{
    currBridge = bridge;
}

static VGIBridge* vgiGetCurrentBridge()
{
    return currBridge;
}

#endif

static VGIBridge* vgiGetBridge()
{
    VGIBridge *bridge = vgiGetCurrentBridge();
    if( !bridge )
    {
        VGIBridgeFunc vgiVGIBridge = NULL;

        bridge = (VGIBridge *)malloc( sizeof(VGIBridge) );
        if( !bridge )
            return NULL;

        memset( bridge, 0, sizeof(VGIBridge) );

#if defined __SYMBIAN32__
        {
            RLibrary lib;
#ifdef OPENVG_VERSION_1_1
            if( lib.Load( _L("libOpenVG_SW.dll") ) == KErrNone )
                vgiVGIBridge = (VGIBridgeFunc)lib.Lookup( 89 ); // <- 89 is a hardcoded ordinal, refer to libopenvgu.def in bwins and eabi folders
#else // OPENVG_VERSION_1_0 and OPENVG_VERSION_1_0_1
			if( lib.Load( _L("libOpenVG.dll") ) == KErrNone )
                vgiVGIBridge = (VGIBridgeFunc)lib.Lookup( 1 ); // <- 1 is a hardcoded ordinal, refer to libopenvgu.def in bwins and eabi folders
#endif
            bridge->libHandle = lib;
        }
#elif defined WIN32
        {
            HMODULE hModule = LoadLibrary( "libOpenVG.dll" );
            if( hModule )
                vgiVGIBridge = (VGIBridgeFunc)GetProcAddress( hModule, "vgiVGIBridge" );
        }
#endif

        if( !vgiVGIBridge )
        {
            free( bridge );
            return NULL;
        }

        vgiVGIBridge( bridge );
        vgiSetCurrentBridge( bridge );
    }

    return bridge;
}

VGI_API_CALL int VGIInitialize( int width, int height, VGIColorSpace colorSpace )
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGIInitialize( width, height, colorSpace );
}

VGI_API_CALL int VGIInitializeEx( int width, int height, VGIColorSpace colorSpace, int premultiplied, int conformant )
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGIInitializeEx( width, height, colorSpace, premultiplied, conformant );
}

VGI_API_CALL int VGICopyToTarget( VGIColorBufferFormat format, int bufferStride, void *buffer, int maskStride, void *mask, VGICopyToTargetHint hint )
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGICopyToTarget( format, bufferStride, buffer, maskStride, mask, hint );
}

VGI_API_CALL void VGITerminate( void )
{
    VGI_GET_BRIDGE;

    bridge->VGITerminate();

#if defined __SYMBIAN32__
    bridge->libHandle.Close();
#endif
    
    free( bridge );
    vgiSetCurrentBridge( NULL );
}

VGI_API_CALL int VGIResize( int width, int height )
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGIResize( width, height );
}

VGI_API_CALL int VGIBindToImage( VGImage image )
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGIBindToImage( image );
}

VGI_API_CALL int VGIUnBindImage( void )
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGIUnBindImage();
}

#ifdef __SYMBIAN32__

EXPORT_C TInt VGISymbianInitialize( TSize aSize, VGIColorSpace aColorSpace )
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGISymbianInitialize( aSize, aColorSpace );
}

EXPORT_C TInt VGISymbianInitializeEx( TSize aSize, VGIColorSpace aColorSpace, TBool aPremultiplied, TBool aConformant )
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGISymbianInitializeEx( aSize, aColorSpace, aPremultiplied, aConformant );
}

EXPORT_C TInt VGISymbianCopyToBitmap( CFbsBitmap *aBitmap, CFbsBitmap *aMaskBitmap, VGICopyToTargetHint aHint )
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGISymbianCopyToBitmap( aBitmap, aMaskBitmap, aHint );
}

EXPORT_C void VGISymbianTerminate()
{
    VGI_GET_BRIDGE;

    bridge->VGISymbianTerminate();
    
    bridge->libHandle.Close();

    free( bridge );
    vgiSetCurrentBridge( NULL );
}

EXPORT_C TInt VGISymbianResize( TSize aSize )
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGISymbianResize( aSize );
}

EXPORT_C TInt VGISymbianBindToImage( VGImage aImage )
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGISymbianBindToImage( aImage );
}

EXPORT_C TInt VGISymbianUnBindImage()
{
    VGI_GET_BRIDGE_RET;

    return bridge->VGISymbianUnBindImage();
}

#endif
