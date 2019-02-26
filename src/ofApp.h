#pragma once

#include "ofMain.h"
#include "ofxSvg.h"
#include "ofxHapInAVFoundationPlayer.h"
#include "ofxOsc.h"

// listen on port 12345
#define PORT 12345

#define VIDEOS_LENGTH 15
#define SEQUENCE_LENGTH 7
#define MAX_VISITORS 40
#define HD_ASPECT_RATIO 1.777

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void keyReleased(int key);
    void windowResized(int w, int h);
    void gotMessage(ofMessage msg);
    void exit();
    
    
    //
    // VIDEO STUFF
    //
    
    // use these functions to start each new video
    // we don't access the array of videos directly
    void playVideo(int n);
    
    ofRectangle videoSize;
    
    // array of all the video files
    ofxHapInAVFoundationPlayer videos[VIDEOS_LENGTH];
    
    // use these functions to regenerate a list of videos to play
    void generateVideoSequence();
    
    // Array to hold the generated video sequence
    // each number represents a video from the video array
    int sequence[SEQUENCE_LENGTH];
    
    // int to remember which video we're playing
    int videosIndex;
    int sequenceIndex;
    bool isSequencePlaying = false;
    bool isSequenceAutomatic = false;
    
    
    
    //
    // GFX STUFF
    //
    
    ofColor orange;
    
    void drawDebugInfo(int x, int y, int w, int h);
    void drawGFX(float x, float y, float w, float h);
    
    // padding to space things away from the edge of the screen (percent)
    const float gfxPaddingX = 0.055;
    const float gfxPaddingY = 0.02;
    
    // logo
    ofxSVG logo;
    ofxSVG person;
    
    // audio
    ofSoundPlayer  ding;
    
    // flag to show/hide the debug controls and keyboard shortcuts
    bool showControls = false;
    
    //
    // INTERACTIVE STUFF
    //
    
    // vector to hold the unique ids for every visitor
    vector<string> visitors;
    
    void addVisitor();
    void removeVisitor();
    char getRandomChar();
    
    // timer to show the visitors id
    float visitorStartTime;
    bool isVisitorAnimating = false;
    bool numVisitorsChanged = false;
    
    //
    // Communications
    //
    ofxOscReceiver receiver;
    void receiveOSC();
    int people;
};
