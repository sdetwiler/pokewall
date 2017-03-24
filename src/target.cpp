//
//  target.cpp
//  pokewall
//
//  Created by Stephen Detwiler on 3/21/17.
//
//

#include "target.h"

Target::Target(char const* filename)
{
    // TODO filename
    mImage.load("007Squirtle.png");
    mWidth = 475;
    mHeight = 475;
    mScaleX = 1.0;
    mScaleY = 1.0;
    mStartX = 0;
    mStartY = 0;

    mStartScaleX = 1.0;
    mStartScaleY = 1.0;
    mTargetScaleX = mStartScaleX;
    mTargetScaleY = mStartScaleY;
    
    mColor = ofColor(255,255,255);
    mStartColor = mColor;
    mTargetColor = mColor;
    
    mState = Initial;
}

Target::~Target()
{
    
}


bool Target::updateTransform()
{
    float now = ofGetElapsedTimef();
    float pct = (now-mStartTime)/mStateDuration;
    mX = ofLerp(mStartX, mTargetX, pct);
    mY = ofLerp(mStartY, mTargetY, pct);
    mScaleX = ofLerp(mStartScaleX, mTargetScaleX, pct);
    mScaleY = ofLerp(mStartScaleY, mTargetScaleY, pct);
    
    mColor = mStartColor.lerp(mTargetColor, pct);
    
    if(pct >=1.0)
    {
        return true;
    }
    
    return false;
    
}

void Target::setState(State state)
{
    mState = state;
    mStartTime = ofGetElapsedTimef();
}

void Target::updateInitial()
{
    mStartX = ofGetScreenWidth()/2 - mWidth/2;
    mStartY = -mHeight;
    mTargetX = ofGetScreenWidth()/2 - mWidth/2;
    mTargetY = ofGetScreenHeight() - mHeight;
    mStateDuration = 2.0;
    
    mStartScaleX = 1.0;
    mStartScaleY = 1.0;
    mTargetScaleX = 1.0;
    mTargetScaleY = 1.0;
    
    mScaleX = 1.0;
    mScaleY = 1.0;
    mColor = mStartColor = mTargetColor = ofColor(255,255,255);

    setState(Entering);
}

void Target::updateEntering()
{
    if(updateTransform())
    {
        mStartX = mTargetX;
        mStartY = mTargetY;
        mTargetScaleY = 1.2;
        mStateDuration = 1.0;
        
        setState(Idle);
    }
}

void Target::updateIdle()
{
    if(updateTransform())
    {
        if(mTargetScaleY > 1.0)
        {
            mStartScaleY = mTargetScaleY;
            mTargetScaleY = 0.95;
        }
        else
        {
            mStartScaleY = mTargetScaleY;
            mTargetScaleY = 1.05;
        }
        setState(Idle);
    }
}

void Target::updateHit()
{
    if(updateTransform())
    {
        setState(Initial);
    }
}

void Target::update()
{
    switch(mState)
    {
        case Initial:
            updateInitial();
            break;
        case Entering:
            updateEntering();
            break;
        case Idle:
            updateIdle();
            break;
        case Hit:
            updateHit();
            break;
    }
    
}

void Target::draw()
{

    ofSetColor(mColor);

//    int ux = (mX*mScaleX) - (mWidth/2);
//    int uy = mY - (mHeight*mScaleY);
//    int lx = mX+mWidth/2;
//    int ly = mY;
//    ofDrawRectangle(ux, uy, lx-ux, ly-uy);
    
    ofPushMatrix();
    
    mImage.setAnchorPoint(mWidth/2, mHeight);
    
    ofTranslate(mX, mY);
    ofScale(mScaleX, mScaleY);
    mImage.draw(0,0);
    ofPopMatrix();
}

void Target::hit()
{
    if(mState == Idle)
    {
        mStateDuration = .150;
        mTargetScaleX = 3;
        mTargetScaleY = 3;
        mTargetColor = ofColor(255,0,0);
        setState(Hit);
    }
}

bool Target::test(int screenX, int screenY)
{
    int ux = (mX*mScaleX) - (mWidth/2);
    int uy = mY - (mHeight*mScaleY);
    int lx = mX+mWidth/2;
    int ly = mY;
    
//    cout << screenX << "," << screenY << endl;
//    cout << ux << ","<< lx << endl;
//    cout << uy << "," << ly << endl;
//    cout << endl;
    if(screenX > ux && screenX < lx && screenY > uy && screenY < ly)
    {
        return true;
    }
    
    return false;
}
