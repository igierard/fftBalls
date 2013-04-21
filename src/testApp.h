#pragma once

#include "ofMain.h"
#include "ofxFFTLive.h"
#include "ofxFFTFile.h"
#include "ofxUI.h"
#include "ofxGrabCam.h"

#define DRAW_FFTIMAGE2  1

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void GUIEvent(ofxUIEventArgs &e);
        ofxFFTBase* fft;
        ofShader ballShader;
        ofVboMesh baseBallVbo;
        ofxUICanvas *GUI;
        ofxGrabCam cam;
    ofSoundPlayer soundPlayer;
    ofFloatImage fftDataImage ;
    ofFloatColor outerColor, diffuseColor;
    ofLight difuseLight;
    ofMaterial diffuseMat;
    int ballSegmentsR, ballSegmentsH, currentSampleIndex, runningAverageCount,runningAverageMax;
    float ballRadius, ballMaxRadius, ballMaxRadiusAudioMod;
    float *fftBuffer;
    float *runningAverage;
    
    void generateBallVbo();

};
