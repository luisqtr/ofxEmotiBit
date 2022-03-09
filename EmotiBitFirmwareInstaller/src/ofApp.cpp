#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToConsole();
	ofSetLogLevel(OF_LOG_VERBOSE);
	_state = State::WAIT_FOR_FEATHER;
	setupErrorMessageList();
	setupInstructionList();
	// get initial list of available com ports
	comListOnStartup = getComPortList(true);
	
	ofBackground(54, 54, 54, 255);

	//old OF default is 96 - but this results in fonts looking larger than in other programs.
	ofTrueTypeFont::setGlobalDpi(72);

	instructionFont.load("verdana.ttf", 14, true, true);
	instructionFont.setLineHeight(18.0f);
	instructionFont.setLetterSpacing(1.037);
}

//--------------------------------------------------------------
void ofApp::update(){
	bool progressToNextState = false;
	if (!globalTimerReset)
	{
		resetStateTimer();
		ofLog(OF_LOG_NOTICE, "State: " + ofToString(_state));
	}
	if (ofGetElapsedTimef() < STATE_TIMEOUT)
	{
		if (_state == State::WAIT_FOR_FEATHER)
		{
			if (detectFeatherPlugin())
			{
				// progress to next state;
				progressToNextState = true;
			}
		}
		else if (_state == State::UPLOAD_WINC_FW_UPDATER_SKETCH)
		{
			if (uploadWincUpdaterSketch())
			{
				// progress to next state;
				progressToNextState = true;
			}
		}
		else if (_state == State::RUN_WINC_UPDATER)
		{
			if (runWincUpdater())
			{
				// progress to next state;
				progressToNextState = true;
			}
		}
		else if (_state == State::UPLOAD_EMOTIBIT_FW)
		{
			if (uploadEmotiBitFw())
			{
				// progress to next state;
				progressToNextState = true;
			}
		}
		else if (_state == State::TIMEOUT)
		{
			// print error on the console
			ofLog(OF_LOG_ERROR, errorMessage);
			// print the error message on GUI
			while (1);
		}
		else if (_state == State::COMPLETED)
		{
			// print some success message
			ofLog(OF_LOG_NOTICE, "EMOTBIT FIRMWARE SUCCESSFULLY UPDATED!");
			while (1);
		}
	}
	else
	{
		// timeout
		errorMessage = errorMessageList[_state];
		_state = State::TIMEOUT;
		globalTimerReset = false;
	}
	if (progressToNextState)
	{
		_state = State((int)_state + 1);
		globalTimerReset = false;
	}
	currentInstruction = onScreenInstructionList[_state];
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(225);
	instructionFont.drawString(currentInstruction + errorMessage, 30, 35);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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

void ofApp::setupInstructionList()
{
	onScreenInstructionList[State::WAIT_FOR_FEATHER] = "Plug in the feather using the provided USB cable";
	onScreenInstructionList[State::UPLOAD_WINC_FW_UPDATER_SKETCH] = "Step1: Uploading WINC Firmwaer updater Sketch";
	onScreenInstructionList[State::RUN_WINC_UPDATER] = "Step2: Updating WINC FW";
	onScreenInstructionList[State::UPLOAD_EMOTIBIT_FW] = "Step3: Updating EmotiBit firmware";
	onScreenInstructionList[State::COMPLETED] = "FIRMWARE UPDATE COMPLETED SUCCESSFULLY!";
	onScreenInstructionList[State::TIMEOUT] = "";
}

void ofApp::setupErrorMessageList()
{
	errorMessageList[State::WAIT_FOR_FEATHER] = "Feather not detected. Check USB cable";
	errorMessageList[State::UPLOAD_WINC_FW_UPDATER_SKETCH] = "Could not set feather into Bootloader mode. WINC FW UPDATER sketch upload failed";
	errorMessageList[State::RUN_WINC_UPDATER] = "WINC UPDATER executable failed to run";
	errorMessageList[State::UPLOAD_EMOTIBIT_FW] = "Could not set feather into Bootloader mode. EmotiBit FW update failed";
}

void ofApp::resetStateTimer()
{
	ofResetElapsedTimeCounter();
	globalTimerReset = true;
}

bool ofApp::detectFeatherPlugin()
{
	std::vector<std::string> currentComList = getComPortList(true);
	std::string newComPort = findNewComPort(comListOnStartup, currentComList);
	if (newComPort.compare(COM_PORT_NONE) != 0)
	{
		// found new COM port
		featherPort = newComPort;
		return true;
	}
	else
	{
		ofLog(OF_LOG_NOTICE, "No new COM port detected");
		return false;
	}
}

std::vector<std::string> ofApp::getComPortList(bool printOnConsole)
{
	ofSerial serial;
	std::vector<std::string> comPortList;
	// get list of com ports
	vector <ofSerialDeviceInfo> initDeviceList = serial.getDeviceList();
	// convert device list into COM ports
	for (int i = 0; i < initDeviceList.size(); i++)
	{
		comPortList.push_back(initDeviceList.at(i).getDevicePath());
	}
	// sort the list
	std::sort(comPortList.begin(), comPortList.end());
	
	// print available COM ports on console
	if (printOnConsole)
	{
		std::string comPorts;
		for (int i = 0; i < comPortList.size() - 1; i++)
		{
			comPorts += comPortList.at(i) + DELIMITER;
		}
		comPorts += comPortList.back();
		ofLog(OF_LOG_VERBOSE, "Available COM ports: " + comPorts);
	}
	return comPortList;
}

bool ofApp::initProgrammerMode(std::string &programmerPort)
{
	ofSerial serial;
	// get initial list of COM ports
	std::vector<std::string> initialComPortList = getComPortList(true);
	std::vector<std::string> updatedComPortList;
	// ping every COM port to try and set it to programmer mode
	for (int j = 0; j < MAX_NUM_TRIES_PING; j++)
	{
		ofLog(OF_LOG_NOTICE, "########## Try: " + ofToString(j + 1));
		serial.setup(featherPort, 1200);
		ofSleepMillis(200);
		serial.close();
		ofSleepMillis(1000);
		updatedComPortList = getComPortList(true);
		// check if a new OCM port has been detected
		std::string newPort = findNewComPort(initialComPortList, updatedComPortList);
		if (newPort.compare(COM_PORT_NONE) != 0)
		{
			// return the new COM port and the list of COM ports with the feather in programmer mode
			programmerPort = newPort;
			comListWithProgrammingPort = updatedComPortList;
			return true;
		}
	}
	return false;
}
std::string ofApp::findNewComPort(std::vector<std::string> oldList, std::vector<std::string> newList)
{
	// for every COM port in the new list
	for (int i = 0; i < newList.size(); i++)
	{
		// check if the COM port existed in the old list
		if (std::find(oldList.begin(), oldList.end(), newList.at(i)) == oldList.end())
		{
			// COM port not in oldList 
			// NEW PORT!
			ofLog(OF_LOG_NOTICE, "New COM port detected: " + newList.at(i));
			return newList.at(i);
		}
		else
		{
			// COM port exists in oldList
			continue;
		}
	}
	return COM_PORT_NONE;
}

bool ofApp::uploadWincUpdaterSketch()
{
	std::string programmerPort;
	std::vector<std::string> programmerPortComList;
	// try to set feather in programmer mode
	if (initProgrammerMode(programmerPort))
	{
		// run command to upload WiFi Updater sketch
		ofLog(OF_LOG_NOTICE, "uploading WiFi updater sketch");
		std::string command = "data\\bossac.exe -i -d -U true -e -w -v -R -b -p " + programmerPort + " data\\WINC\\FirmwareUpdater.ino.feather_m0.bin";
		system(command.c_str());
		ofLog(OF_LOG_NOTICE, "DONE!");
		ofLog(OF_LOG_NOTICE, "WiFi updater sketch uploaded");
		return true;
	}
	else
	{
		return false;
	}
}

bool ofApp::runWincUpdater()
{
	// get updated COM list. the feather returns back to feather port, after the bossa flash is completed.
	std::vector<std::string> newComPortList = getComPortList(true);
	// find the fether port 
	featherPort = findNewComPort(comListWithProgrammingPort, newComPortList); // old list, new list
	if (featherPort.compare(COM_PORT_NONE) != 0)
	{
		// found feather port
		ofLog(OF_LOG_NOTICE, "Feather found at: " + featherPort);
		ofLog(OF_LOG_NOTICE, "UPDATING WINC FW");
		std::string command = "data\\WINC\\FirmwareUploader.exe -port " + featherPort + " -firmware " + "data\\WINC\\m2m_aio_3a0.bin";
		system(command.c_str());
		ofLog(OF_LOG_NOTICE, "WINC FW updated!");
		return true;
	}
	else
	{
		return false;
	}
}

bool ofApp::uploadEmotiBitFw()
{
	std::string programmerPort;
	// try to set device in programmer mode
	if (initProgrammerMode(programmerPort))
	{
		// device successfully set to programmer mode!
		// run command to upload WiFi Updater sketch
		ofLog(OF_LOG_NOTICE, "Uploading EmotiBit FW");
		std::string command = "data\\bossac.exe -i -d -U true -e -w -v -R -b -p " + programmerPort + " data\\EmotiBit_stock_firmware.ino.feather_m0.bin";
		system(command.c_str());
		ofLog(OF_LOG_NOTICE, "DONE!");
		ofLog(OF_LOG_NOTICE, "EmotiBit FW uploaded!");
		ofSleepMillis(2000);
		return true;
	}
	else
	{
		return false;
	}
}