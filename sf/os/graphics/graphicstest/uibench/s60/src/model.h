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
//


#ifndef MODEL_H_
#define MODEL_H_


#include "geometrystructs.h"
#include <e32base.h>

#include <GLES/egl.h>


class CSolidSphere;

/*
 The model class contains one or more shapes and some knowledge of how to
 arrange and light them to create a scene. It knows how to do two things:
 - change the resolution of the shapes in the scene, which involves 
 recalculating their vertices.
 - draw the scene to a buffer by drawing, arranging and lighting the shapes
 using Open GL ES commands.
 
 It understands that these two operations may be happening in different 
 threads, so it serialises access to the model data (the vertices) with a mutex.  
 */

class CModel : public CBase
    {
public:
    static CModel* NewL(EGLDisplay aDisplay, EGLSurface aSurface, TPtrC aBackgroundColor);
    static CModel* NewL(EGLDisplay aDisplay, EGLSurface aSurface);
    ~CModel();
    void PrecalculateOrbitL(TInt aOrbitingDistance, RArray<Vertex3F>& aVertices, TInt aPositionCount, TReal aAngle);
    void SetResolutionL(TInt aResolution);
    void DrawToBuffer(TInt aTime) const;
    
    enum {ERed = 0, EGreen, EBlue, EAlpha};
    
private:
    CModel(EGLDisplay aDisplay, EGLSurface aSurface);
    void ConstructL(TPtrC aBackgroundColor);
    void SetShapes(CSolidSphere* aSun, CSolidSphere* aPlanet, CSolidSphere* aSatellite);
    void InitialiseFrame() const;
    void DrawModelData(TInt aTime) const;
    
private:
    EGLDisplay iDisplay;
    EGLSurface iSurface;

    CSolidSphere* iSun;
    CSolidSphere* iPlanet;
    CSolidSphere* iMoon;
    
    RArray<Vertex3F> iSunPositions;
    RArray<Vertex3F> iMoonPositions;
    GLclampf iBackgroundColor[4];
    TInt iResolution;
    };

#endif /* MODEL_H_ */
