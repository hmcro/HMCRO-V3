#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // VERSION 3
    // Updated to use HAP-AV-Foundation library, by Henry Betts, and compile in 64-bit on Xcode 10
    //
    // there is a run script that copies the contents of the /bin/data/ folder
    // into the /Resources/data/ folder every time we run the project
    // so we must tell oF to load from the correct folder
    
    ofSetDataPathRoot("../Resources/data/");
    
    
    ofSetWindowTitle("Citizen Rotation Office");
    
    // set the colour
    orange.setHex(0xAF2C00);
    
    // load the GFX
    logo.load("hmcro-logo.svg");
    person.load("person-icon.svg");
    
    // load the audio
    ding.load("91926__corsica-s__ding.wav");
    
    // V2 Volume of ding sound to be halved to better match video volume
    // set ding volume
    ding.setVolume(0.5);
    
    
    // calculate the screen dimensions by calling the windowResized function
    windowResized( ofGetWidth(), ofGetHeight() );
    
    // load all video urls into array
    videos[0].load("Attractor.mov");
    videos[1].load("Welcome1.mov");
    videos[2].load("Welcome2.mov");
    videos[3].load("Tour1.mov");
    videos[4].load("Tour2.mov");
    videos[5].load("Tour3.mov");
    videos[6].load("Questions1.mov");
    videos[7].load("Questions2.mov");
    videos[8].load("Questions3.mov");
    videos[9].load("Questions4.mov");
    videos[10].load("Meeting1.mov");
    videos[11].load("Reflection1.mov");
    videos[12].load("Reflection2.mov");
    videos[13].load("End.mov");
    videos[14].load("Detected.mov");
    
    // tell all the videos to only play once
    for(int i = 0; i < VIDEOS_LENGTH; i++) {
        videos[i].setLoopState(OF_LOOP_NONE);
    }
    
    // BUT tell the attractor to play on looping
    //    videos[0].setLoopState(OF_LOOP_NORMAL);
    
    // start playing the attractor video immediately
    playVideo(0);
    
    // Setup comms
    cout << "listening for osc messages on port " << PORT << "\n";
    receiver.setup(PORT);
    
    // Hide the cursor
    ofHideCursor();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // update the sound playing system:
    ofSoundUpdate();
    
    // check if the video has finished
    if ( videos[videosIndex].getIsMovieDone() ) {
        cout << "video finished = videosIndex:" << videosIndex << ", sequenceIndex:" << sequenceIndex << endl;
        if (isSequencePlaying) {
            // a video in the sequence has finished
            // are there any more videos to play?
            if ( sequenceIndex >= SEQUENCE_LENGTH-1 ) { // if sequence is finished
                if (numVisitorsChanged) {
                    // start playing again
                    // set the flag to false to record the current number of visitors
                    numVisitorsChanged = false;
                    // regenerate a new video sequence
                    generateVideoSequence();
                    // reset the index so the next video will be the first in the sequence
                    sequenceIndex = 0;
                    // play the first video in the sequence
                    playVideo(sequence[sequenceIndex]);
                    isSequenceAutomatic = false;
                    isSequencePlaying = true;
                } else {
                    // play the attractor
                    playVideo(0);
                    isSequencePlaying = false;
                }
            } else {
                // increase the nPlaying number to the next video in the array
                sequenceIndex++;
                playVideo( sequence[sequenceIndex] );
            }
        } else {
            // the attractor has finished
            // regenerate a new video sequence
            generateVideoSequence();
            // reset the index so the next video will be the first in the sequence
            sequenceIndex = 0;
            // play the first video in the sequence
            playVideo(sequence[sequenceIndex]);
            isSequenceAutomatic = true;
            isSequencePlaying = true;
        }
    }
    
    videos[videosIndex].update();
    
    if (isVisitorAnimating) {
        float timer = ofGetElapsedTimef() - visitorStartTime;
        
        // check and stop the timer
        if (timer >= 6) {
            cout << "stop the timer" << endl;
            isVisitorAnimating = false;
        }
    }
    
    receiveOSC();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0);
    
    videos[videosIndex].draw( videoSize.x, videoSize.y, videoSize.width, videoSize.height );
    drawGFX( videoSize.x, videoSize.y, videoSize.width, videoSize.height );
    
    if (showControls) {
        drawDebugInfo( videoSize.x, videoSize.y, videoSize.width, videoSize.height );
    }
    
}


//--------------------------------------------------------------
void ofApp::drawGFX(float x, float y, float w, float h){
    
    float scale = w / 1920.0f;
    
    // draw the line
    ofDrawLine( x+(w*gfxPaddingX), y+h-(logo.getHeight()*scale)-((h*gfxPaddingY)*2),
               w-(w*gfxPaddingX), y+h-(logo.getHeight()*scale)-((h*gfxPaddingY)*2) );
    
    // move to bottom of screen to draw logo
    ofPushMatrix();
    ofTranslate(x+(w*gfxPaddingX), y+h-(logo.getHeight()*scale)-(h*gfxPaddingY));
    
    // draw the logo at the correct scale
    ofScale(scale, scale);
    
    // draw the logo
    logo.draw();
    
    ofPopMatrix();
    
    
    
    // move to bottom of screen to draw logo
    ofPushMatrix();
    ofTranslate(w-(w*gfxPaddingX), y+h-(logo.getHeight()*scale)-((person.getHeight()*scale)/2));
    
    if (isSequenceAutomatic) {
        // just draw a circle
        ofPushStyle();
        ofSetColor(255,255,255);
        ofDrawCircle(0, 0, 10);
        ofPopStyle();
        
    } else {
        // draw the people
        // create an iterator that points to the first element
        vector<string>::iterator it = visitors.begin();
        
        // loop through, increasing to next element until the end is reached
        for(; it != visitors.end(); ++it){
            // draw the people icons
            ofTranslate(-(person.getWidth()*scale), 0);
            
            // scale the icon to the right size
            ofPushMatrix();
            ofScale(scale, scale);
            person.draw();
            ofPopMatrix();
            
            ofTranslate(-5, 0);
        }
        
        if (isVisitorAnimating) {
            // nudge up a little and draw the string
            ofTranslate((person.getWidth()*scale)/2+5, -50);
            ofDrawBitmapStringHighlight("Citizen #" + visitors.back() + " detected", -110, 0, orange);
            
            ofPushStyle();
            ofSetColor(orange);
            ofDrawLine(0, 0, 0, 50);
            ofPopStyle();
        }
    }
    
    ofPopMatrix();
    
}

//--------------------------------------------------------------
void ofApp::drawDebugInfo(int x, int y, int w, int h){
    
    string str = "VIDEO SEQUENCER";
    str += "\n\nA = attractor video";
    str += "\n\nS = skip to near end of attractor video";
    str += "\nattractor duration = " + ofToString(videos[0].getTotalNumFrames());
    str += "\nattractor position = " + ofToString(videos[0].getCurrentFrame());
    str += "\nnumVisitorsChanged = " + ofToString(numVisitorsChanged);
    str += "\nisSequenceAutomatic: " + ofToString(isSequenceAutomatic);
    str += "\nF = fullscreen";
    str += "\n+ = Add 1 visitor";
    str += "\n- = Remove 1 visitor";
    str += "\n\nisPlayingSequence: " + ofToString(isSequencePlaying);
    str += "\nSequence: ";
    
    // output the sequence order and highlight the new video position
    for(int i = 0; i < SEQUENCE_LENGTH; i++) {
        if (i == sequenceIndex) {
            str += "[";
        }
        str += ofToString(sequence[i]);
        if (i == sequenceIndex) {
            str += "]";
        }
        if (i < SEQUENCE_LENGTH-1) {
            str += ",";
        }
    }
    
    str += "\nVisitors: " + ofToString(visitors.size());
    
    ofDrawBitmapString(str, x+(w*gfxPaddingX), y+(w*gfxPaddingX));
}

//--------------------------------------------------------------
void ofApp::generateVideoSequence(){
    cout << "generateVideoSequence" << endl;
    
    // create a new sequence randomly and replace current sequence
    sequence[0] = 14; // detected
    sequence[1] = round( ofRandom(1, 2) ); // welcome
    sequence[2] = round( ofRandom(3, 5) ); // tour
    sequence[3] = round( ofRandom(6, 9) ); // question
    sequence[4] = 10; // meeting
    sequence[5] = round( ofRandom(11, 12) ); // reflection
    sequence[6] = 13; // end
}

//--------------------------------------------------------------
void ofApp::playVideo(int n){
    
    // stop any existing video
    videos[videosIndex].stop();
    
    // point to the new video
    videosIndex = n;
    
    // set to starting frame
    videos[videosIndex].setPosition(0);
    
    // start playing from the beginning
    videos[videosIndex].play();
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if (key == 'a') {
        
        isSequencePlaying = false;
        playVideo(0);
        
    }
    
    else if (key == 's') {
        
        isSequencePlaying = false;
        playVideo(0);
        videos[0].setFrame(6200);
        
    }
    
    else if (key == 'f') {
        
        ofToggleFullscreen();
        
    }
    
    else if (key == '+') {
        
        addVisitor();
        
    }
    
    else if (key == '-') {
        
        removeVisitor();
        
    }
    
    else if (key == ' ') {
        
        // toggle the controls top left
        showControls = !showControls;
        
    }
    
}

//--------------------------------------------------------------
void ofApp::addVisitor(){
    
    if (visitors.size() < MAX_VISITORS) {
        
        // add random ID to the vector
        string id = "";
        id += getRandomChar();
        id += getRandomChar();
        id += ofToString( rand() % 9 + 1 );
        id += ofToString( rand() % 9 + 1 );
        id += ofToString( rand() % 9 + 1 );
        id += ofToString( rand() % 9 + 1 );
        id += ofToString( visitors.size() % 10 );
        id += getRandomChar();
        
        visitors.push_back( id );
        
        // play the audio sound
        ding.play();
        
        // start the timer to show/hide the id
        visitorStartTime = ofGetElapsedTimef();
        isVisitorAnimating = true;
        
        if (!isSequencePlaying) {
            
            // set the flag to false to record the current number of visitors
            numVisitorsChanged = false;
            
            // regenerate a new video sequence
            generateVideoSequence();
            
            // reset the index so the next video will be the first in the sequence
            sequenceIndex = 0;
            
            // play the first video in the sequence
            playVideo(sequence[sequenceIndex]);
            
            isSequenceAutomatic = false;
            isSequencePlaying = true;
        }
        else {
            // we're already playing so the num of visitors has changed!
            numVisitorsChanged = true;
        }
        
    }
    
}

//--------------------------------------------------------------
void ofApp::removeVisitor(){
    
    if (visitors.size() > 0) {
        
        // erase the last element in the vector
        visitors.pop_back();
        
        // V2 ding.play() is disabled on visitor leaving the space
        // ding.play();
        
        // check and hide the id if everyone's left
        if ( visitors.size() == 0 ) {
            isVisitorAnimating = false;
            numVisitorsChanged = false;
        }
        else {
            numVisitorsChanged = true;
        }
        
    }
    
}


//--------------------------------------------------------------
char ofApp::getRandomChar(){
    
    return rand() % 26 + 'A';
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    // convert width and height to floats so we can use decimals
    float screenWidth = w;
    float screenHeight = h;
    float screenRatio = screenWidth / screenHeight;
    
    // check the screen ratio and stretch the width or the height
    if (screenRatio >= HD_ASPECT_RATIO) {
        
        // wider
        float newWidth = screenHeight * HD_ASPECT_RATIO;
        videoSize.set((screenWidth-newWidth)/2, 0, screenHeight * HD_ASPECT_RATIO, screenHeight);
        
    }
    else {
        
        // taller
        float newHeight = screenWidth / HD_ASPECT_RATIO;
        videoSize.set(0, (screenHeight-newHeight)/2, screenWidth, newHeight);
        
    }
}

//--------------------------------------------------------------
void ofApp::receiveOSC(){
    
    if(receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(m);
        int num = m.getArgAsInt(0);
        if (num < people) removeVisitor();
        if (num > people) addVisitor();
        people = num;
        cout << "people = " << people << "\n";
    }
}


//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::exit() {
    
    // stop the current video that's playing
    videos[videosIndex].stop();
    
    // tell all the videos to only play once
    for(int i = 0; i < VIDEOS_LENGTH; i++) {
        videos[i].close();
    }
    
    // stop and unload the sound
    ding.unload();
    
    cout << "\nBYE!";
}


