#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"

#include "target.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
		
private:
    void setupPanel();
    void setupKinect();
    
    ofxPanel panel;
    ofParameter<float>	panelFPS;
    ofParameter<int>	blobs;
    bool showPanel;
    
    ofxCvColorImage rgb, hsb;
    ofxCvGrayscaleImage hue, sat, bri, filtered;
    ofxCvContourFinder contours;
    
    float				now;
    float				deltaTime;
    float				lastKinectUpdate;
    float				fps;
    
    int w, h;
    int findHue;
    
    ofTrueTypeFont mFont;
    
    
    ofRectangle mTargetBounds;
    ofPoint mBall;
    
    vector < shared_ptr<ofxKinectV2> > kinects;
    
    vector <ofTexture> kinectTexDepth;
    vector <ofTexture> kinectTexRGB;
    vector <ofParameter<float>> kinectFPS;
    
    ofParameter<bool> showVideo;
    ofParameter<bool> showTarget;
    ofParameter<bool> showBall;
    ofParameter<bool> mirror;
    ofParameter<float> zoomPct;

    
//    bool inRange(ofColor c, ofColor lower, ofColor upper);

    Target* mTarget;
    

    
    
    void drawVideo();
    void drawTarget();
    void drawBall();
    void drawPanel();
};
