// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
//
// Description:
// Display Adaptation Interface
// 
//

#ifndef OPENWFC_RI_DISPLAY_H
#define OPENWFC_RI_DISPLAY_H

#include <WF/openwfc_ri_display_update.h>
#include <pixelformats.h>

class COpenWFC_RI_Display: public CBase, public MOpenWFC_RI_Display_Update
    {
public:    
    typedef enum
        {
        EScreenInvalid                = 0x0000,    // Initialisation value, don't use
        EScreenRotate0                = 0x0001,    // No rotation
        EScreenRotate90               = 0x0002,    // 90 degree
        EScreenRotate180              = 0x0004,    // 180 degree
        EScreenRotate270              = 0x0008,    // 270 degree
        EScreenRotateMirrorVertical   = 0x0010,    // Upside down/flip
        EScreenRotateMirrorHorizontal = 0x0020,    // Mirrored
        EScreenRotateAll              = 0x000F
        } TScreenRotation;

    typedef enum
        {
        EScreenAttributeInvalid       = 0,
        EScreenAttributeImplementationVersion,      // Version implementation   (TUint32)   RO
        // Currently the only  pixel format support is EUidPixelFormatXRGB_8888
        EScreenAttributePixelFormat,                // Pixel format             (TUint32)   RO
        // Curently, the only value supported is "4"
        EScreenAttributeBytesPerPixel,              // Bytes per pixel          (TUint32)   RO
        EScreenAttributeSupportedRotation,          // Supported rotation bitmap(TUint32)   RO
        EScreenAttributeDefaultRotation,            // Default rotation         (TUint32)   RO
        EScreenAttributeCurrentRotation,            // Current rotation         (TUint32)   RW
        EScreenAttributeNormalWidth,                // Width - rotaton 0        (TUint32)   RO
        EScreenAttributeNormalHeight,               // Height - rotaton 0       (TUint32)   RO
        EScreenAttributeNormalStride,               // Stride - rotaton 0       (TUint32)   RO
        EScreenAttributeFlippedWidth,               // Width - rotaton 90       (TUint32)   RO
        EScreenAttributeFlippedHeight,              // Height - rotaton 90      (TUint32)   RO
        EScreenAttributeFlippedStride,              // Stride - rotaton 90      (TUint32)   RO
        EScreenAttributeScreenGeometry              // geometry struct                      RO
        } TScreenAttribute;
        
    typedef struct {
        TInt32 iSupportedRotations;
        TInt32 iBytesPerPixel;
        TScreenRotation iDefaultRotation;
        TScreenRotation iCurrentRotation;
        TUidPixelFormat iPixelFormat;
        TInt32 iNormalWidth;
        TInt32 iNormalHeight;
        TInt32 iNormalStride;
        TInt32 iFlippedWidth;
        TInt32 iFlippedHeight;
        TInt32 iFlippedStride;
        } TScreenGeometryAttribute;
        
public:
        /**
         * API version
         * 
         * @return  API version
         */
    inline TUint32 APIVersion();
    
    /**
     * Symbian function factory
     * 
     * @param   aStream   Screen Id
     * 
     * @return  An instance of implemented interface
     */
    IMPORT_C static COpenWFC_RI_Display* NewL(TUint aScreen);
    
    /**
     * retrieves the default screen number for an implementation
     * 
     * 
     * @return  Default Screen ID
     */
	IMPORT_C static TUint32 DefaultScreenNumber();
	
public:
    static const TInt32 KTopMostLayer = 0;
    static const TUint32 KAPIVersion  = 0x00000001;
    
protected:
     TInt GetAttributeSize(TUint aAttributeId);
    };

inline TUint32 COpenWFC_RI_Display::APIVersion()
    {
    return KAPIVersion;
    }


#endif // OPENWFC_RI_DISPLAY_H
