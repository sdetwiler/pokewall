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
    panel.add(showTarget.set("Show Target", false));
    
    panel.add(panelFPS.set("FPS", 0, 0, 60));

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
    
    setupKinect();
    setupPanel();
    
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
    
    
    sx = 1.0;//0.5;
    sy = 1.0;//0.5;
    
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
            //copy webcam pixels to rgb image
            rgb.setFromPixels(kinect->getRgbPixels());
            
            //mirror horizontal
//            rgb.mirror(false, true);
            
            
            //duplicate rgb
            hsb = rgb;
            
            //convert to hsb
            hsb.convertRgbToHsv();
            
            
            //store the three channels as grayscale images
            hsb.convertToGrayscalePlanarImages(hue, sat, bri);
            
            //filter image based on the hue value were looking for
            for (int i=0; i<w*h; i++) {
                filtered.getPixels()[i] = ofInRange(hue.getPixels()[i],findHue-5,findHue+5) ? 255 : 0;
            }
            filtered.flagImageChanged();
            
            //run the contour finder on the filtered image to find blobs with a certain hue
            contours.findContours(filtered, 50, w*h/2, 1, false);
            
            for (int i=0; i<contours.nBlobs; i++) {
                if(mTarget->test(contours.blobs[i].centroid.x*sx, contours.blobs[i].centroid.y*sy))
                {
                    mTarget->hit();
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
    
    float dw = w*sx;
    float dh = h*sy;

    //draw all cv images
    rgb.draw(0, 0, dw, dh);
    hsb.draw(dw,0, dw, dh);
    
    filtered.draw(dw*2, 0, dw, dh);
    contours.draw(dw*2, 0, dw, dh);
    
    hue.draw(dw*0, dh, dw, dh);
    sat.draw(dw*1, dh, dw, dh);
    bri.draw(dw*2, dh, dw, dh);
    
    
    ofSetColor(255, 0, 0);
    ofFill();
    
    //draw red circles for found blobs
    for (int i=0; i<contours.nBlobs; i++) {
        
        ofDrawCircle(contours.blobs[i].centroid.x*sx, contours.blobs[i].centroid.y*sy, 20);
    }
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
    else if(key == 't')
    {
        showTarget = !showTarget;
    }
    else if(key == 'h')
    {
        mTarget->hit();
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
    int mx = int(x*(1/sx)) % w;
    int my = int(y*(1/sy)) % h;
    
    //get hue value on mouse position
    findHue = hue.getPixels()[my*w+mx];
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
