#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(30);

    fft = new ofxFFTFile();
//    fft = new ofxFFTLive();
    fft->setMirrorData(false);
    fft->setup();
    
    soundPlayer.loadSound("sample.mp3");
    soundPlayer.setLoop(true);
    soundPlayer.play();
    
    GUI = new ofxUICanvas(0, 0, ofGetWidth(), ofGetHeight());
    float slider_widths = 100.0;
    float slider_heights = 5.0;
    GUI->addWidgetDown(new ofxUILabel("FFT PARAMS", OFX_UI_FONT_LARGE));
    GUI->addWidgetDown(new ofxUISlider("Ball H Res", 4, 300, 21, slider_widths, slider_heights));
    GUI->addWidgetDown(new ofxUISlider("Ball S Res", 1, 300, 15, slider_widths, slider_heights));
    GUI->addWidgetDown(new ofxUISlider("Ball Radius", 1, 100, 23, slider_widths, slider_heights));
    GUI->addWidgetDown(new ofxUISlider("Ball Max Radius", 0.01, 10, 5, slider_widths, slider_heights));
    GUI->addWidgetDown(new ofxUISlider("Color R", 0.0, 1.0, 0.0, slider_widths, slider_heights));
    GUI->addWidgetDown(new ofxUISlider("Color G", 0.0, 1.0, 0.0, slider_widths, slider_heights));
    GUI->addWidgetDown(new ofxUISlider("Color B", 0.0, 1.0, 1.0, slider_widths, slider_heights));
    GUI->addWidgetDown(new ofxUISlider("Ambient Color R", 0.0, 1.0, 0.0, slider_widths, slider_heights));
    GUI->addWidgetDown(new ofxUISlider("Ambient Color G", 0.0, 1.0, 0.0, slider_widths, slider_heights));
    GUI->addWidgetDown(new ofxUISlider("Ambient Color B", 0.0, 1.0, 1.0, slider_widths, slider_heights));
    ofAddListener(GUI->newGUIEvent,this,&testApp::GUIEvent);
    GUI->loadSettings( "GUI/FFT_0.xml") ;
    ballSegmentsR = round( ((ofxUISlider*)GUI->getWidget("Ball H Res"))->getScaledValue());
    ballSegmentsH = round( ((ofxUISlider*)GUI->getWidget("Ball S Res"))->getScaledValue());
    ballRadius = ((ofxUISlider*)GUI->getWidget("Ball Radius"))->getScaledValue();
    ballMaxRadius = ((ofxUISlider*)GUI->getWidget("Ball Max Radius"))->getScaledValue();
    outerColor.r = ((ofxUISlider*)GUI->getWidget("Color R"))->getScaledValue();
    outerColor.g = ((ofxUISlider*)GUI->getWidget("Color G"))->getScaledValue();
    outerColor.b = ((ofxUISlider*)GUI->getWidget("Color B"))->getScaledValue();
    
    diffuseColor.r = ((ofxUISlider*)GUI->getWidget("Ambient Color R"))->getScaledValue();
    diffuseColor.g = ((ofxUISlider*)GUI->getWidget("Ambient Color G"))->getScaledValue();
    diffuseColor.b = ((ofxUISlider*)GUI->getWidget("Ambient Color B"))->getScaledValue();
    
    generateBallVbo();
    
    currentSampleIndex = 0;
    fftDataImage.allocate(fft->getNoOfBands(), ballSegmentsH, OF_IMAGE_COLOR);
//    ofFloatPixels pix = fftDataImage.getPixelsRef();
//    int total_pixels = pix.getHeight()*pix.getWidth()*3;
//    for(int i = 0; i < total_pixels; i++){
//        pix.getPixels()[i] = 0.5;
//    }
    for(int x = 0; x < fftDataImage.getWidth(); x++){
        for(int y = 0; y < fftDataImage.getHeight(); y++){
            float f = 1.0;
            fftDataImage.setColor(x, y, ofFloatColor(f,f,f));
        }
    }
//    delete[] fftBuffer;
    fftBuffer = new float[fft->getNoOfBands()];
    

    cam.setPosition(0,0,-100);
    cam.lookAt(ofVec3f::zero());
    cam.setFarClip(10000);
    
    ballShader.load("shaders/ball");
    
    difuseLight.setAmbientColor(diffuseColor);
    difuseLight.setDiffuseColor(diffuseColor);
    difuseLight.setPosition(-100.0, 100, -100.0);
    
    diffuseMat.setDiffuseColor(ofFloatColor(0.0,0.0,1.0));
    
    runningAverageCount = 0;
    runningAverageMax = 8;
    runningAverage = new float[runningAverageMax];
    for(int i = 0; i < runningAverageMax; i++){
        runningAverage[i] = 0.0;
    }
}

//--------------------------------------------------------------
void testApp::update(){
    fft->update();
    const int bands_to_grab = fft->getNoOfBands();
    fft->getFftData(fftBuffer, bands_to_grab);
    runningAverage[runningAverageCount] = *fftBuffer;
    runningAverageCount++;
    runningAverageCount %= runningAverageMax;
    float total = 0;
    for(int i = 0; i < runningAverageMax; i++){
        total += runningAverage[i];
    }
    ballMaxRadiusAudioMod = total/runningAverageMax;
    for(int i = 0; i < bands_to_grab; i++){
        float f = fftBuffer[i];
        fftDataImage.setColor(i, currentSampleIndex, ofFloatColor(f,f,f));
    }
    fftDataImage.update();
    difuseLight.setDiffuseColor(diffuseColor);
    float lightscale = 1000;
//    difuseLight.setPosition(-cos(currentSampleIndex/(float)ballSegmentsH*2*PI)*lightscale, lightscale, -sin(currentSampleIndex/(float)ballSegmentsH*2*PI)*lightscale);

//    difuseLight.setPointLight();
//    diffuseMat.setDiffuseColor(diffuseColor);
    currentSampleIndex = (currentSampleIndex+1)%ballSegmentsH;
    
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(80);
//    fft->draw();
    GUI->draw();
#ifdef DRAW_FFTIMAGE
    fftDataImage.draw ( 0 , 0 ) ;
#endif
    cam.begin();

    difuseLight.enable();
    diffuseMat.begin();
    ballShader.begin();
    ballShader.setUniform1f("r", ballRadius);
    
    
    ballShader.setUniformTexture("fftData",fftDataImage.getTextureReference(), 1);

    ballShader.setUniform1i("fftDataW",fftDataImage.getWidth());
    ballShader.setUniform1i("fftDataH",fftDataImage.getHeight());
    ballShader.setUniform1i("ballSegmentsH",ballSegmentsH);
    ballShader.setUniform1i("ballSegmentsR",ballSegmentsR);
    ballShader.setUniform1i("currentSampleIndex",currentSampleIndex);
    ballShader.setUniform1f("s",ballMaxRadius*ballMaxRadiusAudioMod);
    ballShader.setUniform3f("color",outerColor.r,outerColor.g,outerColor.b);
    ofPushMatrix();
    
    baseBallVbo.setMode(OF_PRIMITIVE_TRIANGLES);
    baseBallVbo.draw();
    ofPopMatrix();
    ballShader.end();
    diffuseMat.end();
    difuseLight.disable();
    cam.end();
}

void testApp::GUIEvent(ofxUIEventArgs &e){
    bool regenVbo = false;
     string name = e.widget->getName();
    if (name == "Ball H Res") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        ballSegmentsH = round(slider->getScaledValue());
        fftDataImage.resize(fft->getNoOfBands(), ballSegmentsH);
        currentSampleIndex %= ballSegmentsH;
        regenVbo = true;
    }else if (name == "Ball S Res") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        ballSegmentsR = round(slider->getScaledValue());
        
    }else if (name == "Ball Radius") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        ballRadius = slider->getScaledValue();
        regenVbo = true;
    }else if (name == "Ball Max Radius") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        ballMaxRadius = slider->getScaledValue();
    }else if (name == "Color R") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        outerColor.r = slider->getScaledValue();
    }else if (name == "Color G") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        outerColor.g = slider->getScaledValue();
    }else if (name == "Color B") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        outerColor.b = slider->getScaledValue();
    }else if (name == "Ambient Color R") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        diffuseColor.r = slider->getScaledValue();
    }else if (name == "Ambient Color G") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        diffuseColor.g = slider->getScaledValue();
    }else if (name == "Ambient Color B") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        diffuseColor.b = slider->getScaledValue();
    }
    if(regenVbo){
        generateBallVbo();
    }
    GUI->saveSettings("GUI/FFT_0.xml" ) ;
}

void testApp::generateBallVbo(){
    
    baseBallVbo.clearVertices();
    baseBallVbo.clearColors();
    baseBallVbo.clearIndices();
    baseBallVbo.clearNormals();
    baseBallVbo.setMode(OF_PRIMITIVE_POINTS);
    baseBallVbo.setMode(OF_PRIMITIVE_TRIANGLES);
    baseBallVbo.disableTextures();
//    baseBallVbo.disableColors();
    baseBallVbo.addVertex(ofVec3f(0,-ballRadius,0));
    baseBallVbo.addNormal(ofVec3f(0,-1,0));
    baseBallVbo.addColor(ofColor(255,0,0));
    
    for(int h = 1; h < ballSegmentsH; h++){
        float theta1 = h * PI / ballSegmentsH - (PI*0.5);

        int indexOffset = (ballSegmentsR*h)+1;
        float t = ((float)h)/ballSegmentsH;
        ofColor the_color = ofColor(255,0,0).lerp(ofColor(0,0,255),t);
        for(int r = 0; r <= ballSegmentsR; r++){
            ofVec3f e, p;
            float theta3 = r * 2 * PI / (float)ballSegmentsR;
            
            e.x = cosf( theta1 ) * cosf( theta3 );
            e.y = sinf( theta1 );
            e.z = cosf( theta1 ) * sinf( theta3 );

            p = e * ballRadius;// * ofRandom(1.0, 1.5);
            baseBallVbo.addVertex(p);
            baseBallVbo.addColor(the_color);
            baseBallVbo.addNormal(e);
            
            if(h > 0 && r>0){
                //first tri
                baseBallVbo.addIndex(indexOffset+r);
                baseBallVbo.addIndex(indexOffset+r-1);
                baseBallVbo.addIndex(indexOffset+r-(ballSegmentsR+1));
                //second tri
                baseBallVbo.addIndex(indexOffset+r);
                baseBallVbo.addIndex(indexOffset+r-(ballSegmentsR+1));
                baseBallVbo.addIndex(indexOffset+r-ballSegmentsR);
            }

        }
        if(h == 1){
            //add top cap indexs
            for (int r = 2; r <= ballSegmentsR+1; r++) {
                baseBallVbo.addIndex(0);
                baseBallVbo.addIndex(r-1);
                baseBallVbo.addIndex(r);
            }
        }
    }
    baseBallVbo.addVertex(ofVec3f(0,ballRadius,0));
    baseBallVbo.addNormal(ofVec3f(0,1,0));
    baseBallVbo.addColor(ofColor(0,0,255));
    //add bottom cap indexs
    const int last_vert = baseBallVbo.getNumVertices()-1;
    for (int r = 1; r <= ballSegmentsR; r++) {
        baseBallVbo.addIndex(last_vert);
        baseBallVbo.addIndex(last_vert-r-1);
        baseBallVbo.addIndex(last_vert-r);
//        printf("%i,%i,%i\n",last_vert,last_vert-r-1,last_vert-r);
//        ofVec3f v = baseBallVbo.getVertex(last_vert-r);
//        printf("%f,%f,%f\n",v.x,v.y,v.z);
    }
//    printf("%f\n",baseBallVbo.getColor(last_vert).b);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}