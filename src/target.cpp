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
//    mImage.load("007Squirtle.png");
    
    mColor = ofColor(255,255,255);
    mStartColor = mColor;
    mTargetColor = mColor;
    
    mSound.load("audio/catch.wav");
    reload();
}

Target::~Target()
{
    
}


void Target::loadRandom()
{
    string path = "targets/";
    ofDirectory dir(path);
    //only show png files
    dir.allowExt("png");
    //populate the directory object
    dir.listDir();
    
    int r = ofRandom(0, dir.size());
    auto filename = dir.getPath(r);
    cout << "filename: " << filename << endl;
    mImage.load(filename);
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
    cout << "state: " << mState << endl;
}

void Target::reload()
{
    mWidth = 475;
    mHeight = 475;
    mScaleX = 1.0;
    mScaleY = 1.0;
    mStartX = 0;
    mStartY = -mHeight;
    mTargetX = mStartX;
    mTargetY = mStartY;
    
    mStartScaleX = 1.0;
    mStartScaleY = 1.0;
    mTargetScaleX = mStartScaleX;
    mTargetScaleY = mStartScaleY;

    
    
    mTargetColor = mStartColor = mColor = mStartColor = mTargetColor = ofColor(0,0,0);
    loadRandom();

    mStateDuration = 1.0;
    mState = Reload;
}
void Target::updateReload()
{
    if(updateTransform())
    {
        setState(Initial);
    }
}

void Target::updateInitial()
{
    float xPct = ofRandom(0.2, 0.75);
    float yPct = ofRandom(0.5, 0.7);
    mStartX = (ofGetScreenWidth()*xPct) - mWidth/2;
    mTargetX = mStartX;

    mStartY = -mHeight;
    mTargetY = ofGetScreenHeight()*yPct;
    mStateDuration = 1.0;
    
    mStartScaleX = 1.0;
    mStartScaleY = 1.0;
    mTargetScaleX = 1.0;
    mTargetScaleY = 1.0;
    
    mScaleX = 1.0;
    mScaleY = 1.0;
    mColor = mStartColor = mTargetColor = ofColor(255,255,255);

    cout << mStartX << "," << mStartY << endl;
    cout << mTargetX << "," << mTargetY << endl;
    cout << endl;
    
    
    setState(Entering);
}

void Target::updateEntering()
{
    if(updateTransform())
    {
        mStartX = mTargetX;
        mStartY = mTargetY;
        mTargetScaleY = 0.95;
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
        reload();
    }
}

void Target::update()
{
    switch(mState)
    {
        case Reload:
            updateReload();
            break;
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
        mSound.play();
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
