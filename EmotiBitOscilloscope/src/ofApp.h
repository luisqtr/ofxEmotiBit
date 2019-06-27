#pragma once

#include "ofMain.h"
#include "ofxOscilloscope.h"
#include "ofxNetwork.h"
#include "ofxNetworkUtils.h"
#include "ofxThreadedLogger.h"
#include "ofxGui.h"
#include "ofxInputField.h"
#include "ofxLSL.h"
#include "DoubleBuffer.h"
#include "EmotiBitPacket.h"


class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void exit();

	ofxLSL lsl;
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void parsePacket(string dataString);
	void parseUdp();
	void parseSerial();
	vector<string> getLocalIPs();
	void sendEmotiBitPacket(string typeTag, string data = "", uint16_t dataLength = 1, uint16_t protocolVersion = 1, uint16_t dataReliability = 100);
	void sendEmotiBitPacket(ofxUDPManager &udpManager, string typeTag, string data = "", uint16_t dataLength = 1, uint16_t protocolVersion = 1, uint16_t dataReliability = 100);
	void parseIncomingAck(vector<string> splitPacket);
	void parseIncomingRequestData(EmotiBitPacket::Header header, vector<string> splitPacket);

	void recordButtonPressed(bool & recording);
	void hibernateButtonPressed(bool & hibernate);
	void sendExperimenterNoteButton();
	template <class T>
	vector<vector<vector<T>>> initBuffer(vector<vector<vector<T>>> buffer);
	float smoother(float smoothData, float newData, float newDataWeight);
	void deviceSelection(bool & selected);
	bool checkDeviceList(string ip);
	void changeConnection(bool selected);
	void sendBroadcast(string ipAddress);
	void processSlowResponseMessage(string message);
	void processSlowResponseMessage(vector<string> splitMessage);
	string ofGetTimestampString(const string& timestampFormat); // Adds %f for microseconds


	//ofxMultiScope scopeWin;
	//ofxMultiScope scopeWin2;
	//int newPoints;
	//int nScopes;
	//std::vector<std::vector<float> > gyroData;
	//std::vector<std::vector<float> > accData;
	//std::vector<std::vector<float> > magData;
	//std::vector<std::vector<float> > gsrData;
	//std::vector<std::vector<float> > ppgIRData;
	//std::vector<std::vector<float> > ppgRData;
	//std::vector<std::vector<float> > ppgGData;
	//std::vector<std::vector<float> > tempData;
	//std::vector<std::vector<float> > thermData;
	//std::vector<std::vector<float> > humData;
	int counter;
	int counter2;
	ofPoint min;
	ofPoint max;

	int selectedScope;

	bool isPaused;
	ofSerial mySerial;

	string stringData;

	ofxUDPManager udpConnection;
	LoggerThread dataLogger;
	LoggerThread consoleLogger;
	ofTrueTypeFont legendFont;
	ofTrueTypeFont subLegendFont;

	//struct EmotibitPacketHeader_V1 {
	//	uint32_t timestamp;  // milliseconds since EmotiBit bootup
	//	uint16_t packetCount;
	//	uint16_t dataReliability;  // Reliability of data 0-100, Reliability=0 no data is sent
	//	uint16_t dataLength;  // length of data value array
	//	uint8_t typeTag;
	//	uint8_t protocolVersion
	//}

	vector<ofxMultiScope> scopeWins;
	vector<vector<vector<string>>> typeTags;
	unordered_map<string, vector<int>> typeTagIndexes;
	vector<vector<float>> samplingFreqs;
	vector<vector<vector<string>>> plotNames;
	vector<vector<vector<float>>> yLims;
	vector<ofColor> plotColors;

	vector<vector<vector<int>>> bufferSizes;
	vector<vector<vector<int>>> dataCounts;
	vector<vector<vector<float>>> dataFreqs;

	//ofxButton recordingStatus;
	ofParameter<bool> recordingButton;
	ofxLabel recordingStatus;
	ofParameter<bool> hibernateButton;
	ofxLabel hibernateStatus;
	ofxLabel batteryStatus; 
	ofxLabel sdCardStatus;
	//ofParameter<float> batteryStatus;
	//ofParameter<float> sdCardStatus;
	ofxTextField userNote;
	ofxButton sendUserNote;
	ofParameter<int> dataClippingCount;
	ofParameter<int> dataOverflowCount;
	ofParameterGroup deviceMenuGroup;
	ofParameter<string> deviceSelected;
	ofxLabel currentDevice;
	vector<ofParameter<bool>> deviceList;
	ofParameterGroup deviceGroup;

	const string GUI_STRING_NOT_RECORDING = "NOT RECORDING";
	const string GUI_STRING_RECORDING = "RECORDING";
	const string GUI_STRING_MODE_HIBERNATE = "HIBERNATING";
	const string GUI_STRING_MODE_ACTIVE = "ACTIVE";
	const string GUI_STRING_NO_EMOTIBIT_SELECTED = "NONE SELECTED";
	const string GUI_DEVICE_GROUP_MENU_NAME = "Emotibit Device Menu";
	const string GUI_DEVICE_GROUP_NAME = "EmotiBit Device List";
	const string GUI_STRING_CLIPPING_EVENTS = "Clipping Events:";
	const string GUI_STRING_OVERFLOW_EVENTS = "Overflow Events:";
	int guiPanelDevice;
	int guiPanelRecord;
	int guiPanelMode;
	int guiPanelLevels;
	int guiPanelErrors;
	int guiPanelUserNote;

	//ofxFloatSlider batteryStatus;
	//ofxFloatSlider sdCardStatus;
	//ofxColorSlider color;
	//ofxVec2Slider center;
	//ofxIntSlider circleResolution;
	//ofxToggle filled;
	//ofxButton twoCircles;
	//ofxButton ringButton;
	//ofxLabel screenSize;

  vector<ofxPanel> guiPanels;

	bool plotUdpData = true;

	int drawYTranslate = 0;
	float drawYScale = 1.f;
	uint32_t malformedMessages = 0;
	uint32_t bufferUnderruns = 0;
	uint16_t MAX_BUFFER_LENGTH = 64;
	size_t messageLen = 0;

	DoubleBuffer<string> messageBuffer; 
	ofMutex connectionLock;
	std::thread* connectionThread;
	bool runConnectionThread = true;

	int connectionPort;
	bool drawDataInfo = false;

	int nDataClippingEvents = 0;
	int nDataOverflowEvents = 0;
};
