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


#include "model.h"
#include "geometrystructs.h"
#include "egldefs.h"
#include "eglerror.h"

#include <e32math.h>


const TInt KMoonPositionCount = 100;
const TInt KSunPositionCount = 100;

const TInt KSunOrbitRadius = 2000; //heliocentric
const TInt KMoonOrbitRadius = 100;
const TInt KModelDistanceFromCamera = 200;

const TInt KMinResolution = 5;
const TInt KMaxResolution = 50;
const TInt KDefaultResolution = 50;

const TInt KSunRadius=100;
const TInt KPlanetRadius=50;
const TInt KMoonRadius = 10;

_LIT(KRed, "red");
_LIT(KGreen, "green");
_LIT(KBlue, "blue");
_LIT(KBlack, "black");


CModel* CModel::NewL(EGLDisplay aDisplay, EGLSurface aSurface)
    {
    TPtrC defaultColor(KBlack);
    return NewL(aDisplay, aSurface, defaultColor);
    }

CModel* CModel::NewL(EGLDisplay aDisplay, EGLSurface aSurface, TPtrC aBackgroundColor)
    {
    CModel* self = new(ELeave) CModel(aDisplay, aSurface);
    CleanupStack::PushL(self);
    self->ConstructL(aBackgroundColor);
    CleanupStack::Pop(self);
    return self;
    }

void CModel::PrecalculateOrbitL(TInt aOrbitingDistance, RArray<Vertex3F>& aVertices, TInt aPositionCount, TReal aAngle)
    {
    //fghCircleTable allocates for sin and cos tables, 
    //so we must not fail to append vertices to the array,
    //so we must reserve all the space we need.
    aVertices.ReserveL(aPositionCount);
    //precalculate the positions of the sun
    TReal32 *sint1,*cost1;
    User::LeaveIfError(fghCircleTable(&sint1,&cost1, aPositionCount));
    
    TReal radians = aAngle*(KPi/180);
    TReal32 cosA = cos(radians);
    TReal32 sinA = sin(radians);
    
    TReal32 x,y = 0;
    TReal32 cosCalc = aOrbitingDistance * cosA;
    TReal32 sinCalc = aOrbitingDistance * sinA;
    for(TInt i = 0; i < aPositionCount;  i++)
        {
        x = cost1[i] * cosCalc;
        y = cost1[i] * sinCalc;

        Vertex3F v(x,y, sint1[i]*aOrbitingDistance);
        User::LeaveIfError(aVertices.Append(v));
        }
    delete[] sint1;
    delete[] cost1;
    }

void CModel::ConstructL(TPtrC aBackgroundColor)
    {
    iResolution=KDefaultResolution;

    CSolidSphere* newSun = CSolidSphere::NewLC(KSunRadius, iResolution, iResolution);
    CSolidSphere* newPlanet = CSolidSphere::NewLC(KPlanetRadius, iResolution, iResolution);
    CSolidSphere* newMoon = CSolidSphere::NewLC(KMoonRadius, iResolution, iResolution);
    SetShapes(newSun, newPlanet, newMoon);
    CleanupStack::Pop(3, newSun);
    
    PrecalculateOrbitL(KSunOrbitRadius, iSunPositions, KSunPositionCount, 135);
    PrecalculateOrbitL(KMoonOrbitRadius, iMoonPositions, KMoonPositionCount, 23.5);
    if(aBackgroundColor == KRed)
        {
        iBackgroundColor[ERed] = 0.5;
        }
    else if (aBackgroundColor == KGreen)
        {
        iBackgroundColor[EGreen] = 0.5;
        }
    else if (aBackgroundColor == KBlue)
        {
        iBackgroundColor[EBlue] = 0.5;
        }
    }

CModel::CModel(EGLDisplay aDisplay, EGLSurface aSurface)
    :iDisplay(aDisplay), iSurface(aSurface)
    {
    }

CModel::~CModel()
    {
    delete iSun;
    delete iPlanet;
    delete iMoon;
    iSunPositions.Close();
    iMoonPositions.Close();
    }

void CModel::SetShapes(CSolidSphere* aSun, CSolidSphere* aPlanet, CSolidSphere* aMoon)
    {
    delete iSun;
    iSun = aSun;
    
    delete iPlanet;
    iPlanet = aPlanet;
    
    delete iMoon;
    iMoon = aMoon;
    }

void CModel::DrawToBuffer(TInt aTime) const
    {
    InitialiseFrame();
    DrawModelData(aTime);
    }

void CModel::InitialiseFrame() const
    {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_COLOR_ARRAY);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    
    glClearColor(iBackgroundColor[ERed], iBackgroundColor[EGreen], iBackgroundColor[EBlue], iBackgroundColor[EAlpha]); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    
    glCullFace(GL_FRONT);   
    glShadeModel (GL_SMOOTH);

    // Set the projection parameters
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    EGLint height;
    if(!eglQuerySurface(iDisplay, iSurface, EGL_HEIGHT, &height))
        {
        ProcessEglError();
        }
    EGLint width;
        if(!eglQuerySurface(iDisplay, iSurface, EGL_WIDTH, &width))
            {
            ProcessEglError();
            }
        
    GLfloat widthf = width;
    GLfloat heightf = height;
    GLfloat aspectRatio = widthf/heightf;
    glFrustumf( -(aspectRatio*KFrustumHeight), (aspectRatio*KFrustumHeight), 
                -KFrustumHeight, KFrustumHeight, 
                KFrustumNear, KFrustumFar);
    }

void CModel::DrawModelData(TInt aTime) const
    {
    //draw a light source where the sun is
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLfloat sun_color[] = { 1.0, 1.0, 0.5333, 1 };
    Vertex3F vSun = iSunPositions[aTime%KSunPositionCount];
    vSun.iZ-=KModelDistanceFromCamera;
    GLfloat light0position[] = {vSun.iX, vSun.iY, vSun.iZ, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light0position);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_color);  
    
    //draw the sun
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //this is obviously wrong: I don't understand why the z-coord for the 
    //sun needs to be the negation of the z-coord for the light source position,
    //in order for the light to appear to come from the sun!
    glTranslatef(vSun.iX, vSun.iY, -vSun.iZ);
        
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, sun_color);
        
    iSun->Draw();
    //draw the planet
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -KModelDistanceFromCamera);
    
    GLfloat mat_planet_color[] = { 0.459, 0.679, 0.8, 1 };
    GLfloat mat_planet_shininess[] = { 30.0 };
    GLfloat mat_no_emission[] = {0, 0, 0, 0};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_planet_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_planet_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_planet_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_planet_shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_no_emission);
    iPlanet->Draw();
    
    //draw the moon
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vertex3F vMoon = iMoonPositions[aTime%KMoonPositionCount];
    vMoon.iZ-=KModelDistanceFromCamera;
    glTranslatef(vMoon.iX, vMoon.iY, vMoon.iZ);
    
    GLfloat mat_moon_specular[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_moon_shininess[] = { 500.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_moon_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_moon_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_moon_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_moon_shininess);  
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_no_emission);
    
    iMoon->Draw();
    }

void CModel::SetResolutionL(TInt aResolution)
    {
    aResolution *= 5;
    if(aResolution > KMaxResolution)
        {
        aResolution = KMaxResolution;
        }
    if(aResolution < KMinResolution)
        {
        aResolution = KMinResolution;
        }
    iResolution = aResolution;
    CSolidSphere* newSun = CSolidSphere::NewLC(KSunRadius, iResolution, iResolution);
    CSolidSphere* newPlanet = CSolidSphere::NewLC(KPlanetRadius, iResolution, iResolution);
    CSolidSphere* newMoon = CSolidSphere::NewLC(KMoonRadius, iResolution, iResolution);
    SetShapes(newSun, newPlanet, newMoon);  
    CleanupStack::Pop(3, newSun);
    }

