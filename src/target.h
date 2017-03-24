//
//  target.h
//  pokewall
//
//  Created by Stephen Detwiler on 3/21/17.
//
//

#ifndef target_h
#define target_h

#include "ofMain.h"

enum State
{
    Initial,
    Entering,
    Idle,
    Hit
};

class Target
{
public:
    Target(char const* filename);
    ~Target();
    
    
    void draw();

    void update();
    
    void hit();
    bool test(int screenX, int screenY);
    
private:
    void updateInitial();
    void updateEntering();
    void updateIdle();
    void updateHit();
    
    bool updateTransform();
    
    void setState(State state);
    
    ofImage mImage;
    
    float mWidth;
    float mHeight;
        
    float mX;
    float mY;
    float mScaleX;
    float mScaleY;
    
    float mStartX;
    float mStartY;
    float mTargetX;
    float mTargetY;
    
    float mStartScaleX;
    float mStartScaleY;
    float mTargetScaleX;
    float mTargetScaleY;
    
    ofColor mColor;
    ofColor mStartColor;
    ofColor mTargetColor;
    
    float mStartTime;
    float mStateDuration;
    
    State mState;

    
};

#endif /* target_h */
