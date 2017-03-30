#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setupKinect()
{
    lastKinectUpdate = ofGetElapsedTimef();
    
    
    //see how many devices we have.
    ofxKinectV2 tmp;
    vector <ofxKinectV2::KinectDeviceInfo> deviceList = tmp.getDeviceList();
    
    //allocate for this many devices
    kinects.resize(deviceList.size());
    kinectTexDepth.resize(kinects.size());
    kinectTexRGB.resize(kinects.size());
    kinectFPS.resize(kinects.size());
    
    
    //Note you don't have to use ofxKinectV2 as a shared pointer, but if you want to have it in a vector ( ie: for multuple ) it needs to be.
    for(int d = 0; d < kinects.size(); d++){
        kinects[d] = shared_ptr <ofxKinectV2> (new ofxKinectV2());
        kinects[d]->open(deviceList[d].serial);
        kinectFPS[d] = 0;
    }
}

//--------------------------------------------------------------
void ofApp::setupPanel()
{
    showPanel = false;
    int guiColorSwitch = 0;
    ofColor guiHeaderColor[2];
    guiHeaderColor[0].set(160, 160, 80, 200);
    guiHeaderColor[1].set(80, 160, 160, 200);
    ofColor guiFillColor[2];
    guiFillColor[0].set(160, 160, 80, 200);
    guiFillColor[1].set(80, 160, 160, 200);
    
    panel.setup("settings");
    
    panel.add(showVideo.set("Show Video", false));
    panel.add(showTarget.set("Show Target", true));
    panel.add(showBall.set("Show Ball", false));
    panel.add(mirror.set("Mirror Video", true));
    
    panel.add(panelFPS.set("FPS", 0, 0, 60));
    panel.add(blobs.set("Blobs", 0, 0, 60));
    panel.add(zoomPct.set("Zoom pct", .5, .1, 1));

    for(int d = 0; d < kinects.size(); d++){
        panel.add(kinects[d]->params);
        panel.add(kinectFPS[d].set("Kinect FPS", 0, 0, 30));
    }

    panel.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
    panel.setDefaultFillColor(guiFillColor[guiColorSwitch]);
    guiColorSwitch = 1 - guiColorSwitch;
    
    
    if (!ofFile("settings.xml"))
        panel.saveToFile("settings.xml");
    
    panel.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    
    mFont.load("arial.ttf", 32);
    
    mTargetBounds = ofRectangle(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
    
    setupKinect();
    setupPanel();
    
    findHue = 178;  // Red ball.
    
    showVideo = false;
    showTarget = false;
    w = 1920;
    h = 1080;
    
//    movie.setup(w, h);
    //reserve memory for cv images
    rgb.allocate(w, h);
    hsb.allocate(w, h);
    hue.allocate(w, h);
    sat.allocate(w, h);
    bri.allocate(w, h);
    filtered.allocate(w, h);
    
    
    mTarget = new Target("foo");

}



//--------------------------------------------------------------
void ofApp::update(){

    mTarget->update();
    if(kinects.size())
    {
        auto kinect = kinects[0];
        kinect->update();
        if(kinect->isFrameNew())
        {
            float sw = w * zoomPct;
            float sh = float(h)/float(w) * sw;
            
            float x = (w - sw)/2;
            float y = (h - sh)/2;
            rgb.setROI(x,0,sw,sh);

            //copy webcam pixels to rgb image
            rgb.setFromPixels(kinect->getRgbPixels());
            
            //mirror horizontal
            if(mirror)
            {
                rgb.mirror(false, true);
            }
            
            
            //duplicate rgb
            hsb.setROI(rgb.getROI());
            hsb = rgb;
            
            //convert to hsb
            hsb.convertRgbToHsv();
            
            hue.setROI(hsb.getROI());
            sat.setROI(hsb.getROI());
            bri.setROI(hsb.getROI());
            //store the three channels as grayscale images
            hsb.convertToGrayscalePlanarImages(hue, sat, bri);
            
            //filter image based on the hue value were looking for
            for (int i=0; i<w*h; i++) {
                filtered.getPixels()[i] = ofInRange(hue.getPixels()[i],findHue-5,findHue+5) ? 255 : 0;
            }
            filtered.setROI(hue.getROI());
            filtered.flagImageChanged();
            
            
            //run the contour finder on the filtered image to find blobs with a certain hue
            contours.findContours(filtered, 50, w*h/2, 1, false);
            blobs = contours.nBlobs;
            
            if(true || showBall) // HACK disable for character testing.
            {
                for (int i=0; i<contours.nBlobs; i++) {
                    mBall.set(contours.blobs[i].centroid.x, contours.blobs[i].centroid.y, 0);

                    float sx = (mBall.x/mTargetBounds.width) * ofGetScreenWidth();
                    float sy = (mBall.y/mTargetBounds.height) * ofGetScreenHeight();

                    
                    if(mTarget->test(sx, sy))
                    {
                        mTarget->hit();
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::drawPanel()
{
    ofPushStyle();
    
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    panel.draw();
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawVideo()
{
    ofSetColor(255,255,255);
    

    //draw all cv images
//    rgb.draw(0, 0, dw, dh);
    
    rgb.drawROI(0,0,w,h);
    
    
    
//    hsb.draw(dw,0, dw, dh);
//    
//    filtered.draw(dw*2, 0, dw, dh);
//    contours.draw(dw*2, 0, dw, dh);
//    
//    hue.draw(dw*0, dh, dw, dh);
//    sat.draw(dw*1, dh, dw, dh);
//    bri.draw(dw*2, dh, dw, dh);
    
    
    
}

void ofApp::drawBall()
{
    
//    float sx = mBall.x;
//    float sy = mBall.y;
    float sx = (mBall.x/mTargetBounds.width) * ofGetScreenWidth();
    float sy = (mBall.y/mTargetBounds.height) * ofGetScreenHeight();
    
    //draw red circles for found blobs
    ofFill();
    ofSetColor(255, 0, 0);
    ofDrawCircle(sx, sy, 20);
    ofSetColor(255,255,255);
    
    std::string label = std::to_string(int(sx)) + "," + std::to_string(int(sy));
    mFont.drawString(label, sx, sy);
    
    ofNoFill();
    ofSetColor(255,255,255);
    ofDrawRectangle(mTargetBounds);
}

void ofApp::drawTarget()
{
    mTarget->draw();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    
    if(showVideo)
    {
        drawVideo();
    }
    if(showBall)
    {
        drawBall();
    }
    if(showTarget)
    {
        drawTarget();
    }
    if(showPanel)
    {
        drawPanel();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'd')
    {
        showPanel = !showPanel;
    }
    else if(key == 'v')
    {
        showVideo = !showVideo;
    }
    else if(key == 'm')
    {
        mirror = !mirror;
    }
    else if(key == 't')
    {
        showTarget = !showTarget;
    }
    else if(key == 'b')
    {
        showBall = !showBall;
    }
    else if(key == 'h')
    {
        mTarget->hit();
    }
    
    
    else if(key == '1')
    {
        mTargetBounds.setX(mBall.x);
        mTargetBounds.setY(mBall.y);
    }
    else if(key == '2')
    {
        mTargetBounds.setWidth(mBall.x);
        mTargetBounds.setHeight(mBall.y);
        
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    if(mTarget->test(x,y))
    {
        cout << "hit" << endl;
        mTarget->hit();
    }
    
    //calculate local mouse x,y in image
    int mx = int(x) % w;
    int my = int(y) % h;
    
    //get hue value on mouse position
    findHue = hue.getPixels()[my*w+mx];
    cout << findHue << endl;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
