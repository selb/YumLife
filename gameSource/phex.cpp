#include "phex.h"

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#include "minorGems/crypto/hashes/sha1.h"

#include "hetuwmod.h"
#include "LivingLifePage.h"
#include "accountHmac.h"
#include "hetuwFont.h"

TCPConnection Phex::tcp;
bool Phex::bSendFirstMsg = true;
bool Phex::lifeStarted = false;

std::unordered_map<std::string, Phex::ServerCommand> Phex::serverCommands;
std::unordered_map<std::string, Phex::ChatCommand> Phex::chatCommands;

char Phex::chatServerCmdChar = '/';

char Phex::chatCmdChar = '.';
std::string Phex::strCmdChar;

std::string Phex::publicHash = "";
std::unordered_map<std::string, Phex::User> Phex::users;
std::unordered_map<int, std::string> Phex::playerIdToHash;
std::unordered_set<std::string> Phex::blockedUsers;

bool Phex::hasFocus = false;
bool Phex::isMinimized = false;
bool Phex::bDrawRecInput = false;

double Phex::textScale = 0.6;

double Phex::textInRecPaddingX;
double Phex::textInRecPaddingY;

double Phex::recBckgr[4];
double Phex::recBckgrBig[4];
float Phex::colorRecBckgr[4];
float Phex::colorRecBckgrHighlight[4];

double Phex::recInput[4];
float Phex::colorRecInput[4];
Phex::Text Phex::inputText;

float Phex::colorWhite[4];
float Phex::colorNamesInChat[4];
float Phex::colorCmdMessage[4];
float Phex::colorCmdInGameNames[4];
float Phex::colorCmdMessageError[4];
std::string Phex::colorCodeWhite;
std::string Phex::colorCodeNamesInChat;
std::string Phex::colorCodeCmdMessage;
std::string Phex::colorCodeCmdInGameNames;
std::string Phex::colorCodeCmdMessageError;

float Phex::colorButPhexOffline[4];
float Phex::colorButPhexConnecting[4];
float Phex::colorButPhexOnline[4];

constexpr int Phex::CMD_MSG_ERROR;
bool Phex::userNameWasChanged = false;

std::string Phex::channelName = "";

double Phex::butHeight = 0.04;
double Phex::butBorderSize = 0.004;
std::vector<Phex::Button*> Phex::buttons;
Phex::Button Phex::butPhex;
Phex::Button Phex::butTurnOff;
Phex::Button Phex::butMinimize;
Phex::Button Phex::butMaximize;

Phex::PhexTitleText Phex::titleText;
double Phex::recLineBelowTitle[4];
Phex::Text Phex::textHotKeyInfo;
double Phex::recLineBelowHotKey[4];

HetuwFont *Phex::mainFont = NULL;
double Phex::mainFontScaleFactor = 16;

Phex::ChatWindow Phex::mainChatWindow;

HetuwMod::KeyHandler Phex::keyHandler(&onKey);

bool Phex::allowServerCoords = true;

std::string Phex::forceChannel = "";

bool Phex::bSendFakeLife = false;

bool Phex::sendBiomeDataActive = false;
char Phex::biomeChunksSent[biomeChunksSentSize][biomeChunksSentSize];
HetuwMod::IntervalTimed Phex::intervalSendBiomeData = HetuwMod::IntervalTimed(1.0);
std::vector<float*> Phex::biomeChunksDrawRecs;

bool Phex::sendPositionActive = false;
HetuwMod::IntervalTimed Phex::intervalSendPosition = HetuwMod::IntervalTimed(3.0);
int Phex::lastPositionSentX = -9999;
int Phex::lastPositionSentY = -9999;

static bool mouseOverBckgr = false;

constexpr char Phex::hexDigits[];

extern doublePair lastScreenViewCenter;
extern char *userEmail;
extern int versionNumber;

static bool temporaryJasonAuthOptIn = false;

void Phex::init() {
	if (!HetuwMod::phexIsEnabled) return;

	strCmdChar = ""; strCmdChar += chatCmdChar;

	initServerCommands();
	initChatCommands();

	tcp.init(HetuwMod::phexIp, HetuwMod::phexPort, &onReceivedMessage, &onConnectionStatusChanged);
	tcp.logTag = "Phex";
	tcp.charEnd = PHEX_CHAR_END;
	tcp.verbose = HetuwMod::debugPhex;
	if (!HetuwMod::phexStartOffline)
		tcp.connect();

	textInRecPaddingX = 0.01;
	textInRecPaddingY = textInRecPaddingX * HetuwMod::viewWidthToHeightFactor;

	if (HetuwMod::phexOnLeft()) {
		setArray(recBckgrBig, (const double[]){ 0.0f, 0.0f, 0.3f, 1.0f }, 4);
	} else {
		setArray(recBckgrBig, (const double[]){ 0.7f, 0.0f, 1.0f, 1.0f }, 4);
    }
	setArray(recBckgr, recBckgrBig, 4);

	setArray(colorRecBckgr, (const float[]){ 0.0f, 0.0f, 0.0f, 0.7f }, 4);

	setArray(colorWhite, (const float[]){ 1.0f, 1.0f, 1.0f, 1.0f }, 4);
	setArray(colorRecInput, (const float[]){ 0.0f, 0.0f, 0.0f, 0.6 }, 4);
	setArray(colorNamesInChat, (const float[]){ 0.2f, 0.7f, 1.0f, 1.0f }, 4);
	setArray(colorCmdMessage, (const float[]){ 0.2f, 1.0f, 0.5f, 1.0f }, 4);
	setArray(colorCmdInGameNames, (const float[]){ 0.6f, 1.0f, 0.2f, 1.0f }, 4);
	setArray(colorCmdMessageError, (const float[]){ 1.0f, 0.7f, 0.4f, 1.0f }, 4);

	mainChatWindow.init(recBckgr);
	setArray(colorButPhexOffline, (const float[]){0.6, 0.0, 0.0, 0.6}, 4);
	setArray(colorButPhexConnecting, (const float[]){0.3, 0.3, 0.0, 0.6}, 4);
	setArray(colorButPhexOnline, (const float[]){0.0, 0.6, 0.0, 0.6}, 4);

	mainChatWindow.rec[0] += textInRecPaddingX;
	mainChatWindow.rec[1] += textInRecPaddingY;
	mainChatWindow.rec[2] -= textInRecPaddingX;
	mainChatWindow.rec[3] -= textInRecPaddingY;
	mainChatWindow.onZoom();

	inputText.bDrawCursor = false;

	initFont();

	colorCodeWhite = mainFont->hetuwGetColorCode(colorWhite);
	colorCodeNamesInChat = mainFont->hetuwGetColorCode(colorNamesInChat);
	colorCodeCmdMessage = mainFont->hetuwGetColorCode(colorCmdMessage);
	colorCodeCmdInGameNames = mainFont->hetuwGetColorCode(colorCmdInGameNames);
	colorCodeCmdMessageError = mainFont->hetuwGetColorCode(colorCmdMessageError);

	// Disable hetuwMaxXActive during UI initialization. The screen state when
	// these texts are being measured is not sane and can lead to weird wrapping
	// during the initial measurement that does not repeat itself in the actual
	// fully initialized game screen.
	mainFont->hetuwMaxXActive = false;

	initButtons();

	double titleTextRec[4];
	titleTextRec[0] = recBckgr[0]; titleTextRec[1] = recBckgr[3]-butHeight-0.13;
	titleTextRec[2] = recBckgr[2]; titleTextRec[3] = recBckgr[3]-butHeight;
	titleText.init(titleTextRec);
	setArray(titleText.recBckgr, titleTextRec, 4);
	titleText.recBckgr[1] += 0.04;

	recLineBelowTitle[0] = recBckgr[0];
	recLineBelowTitle[1] = titleText.recBckgr[1]-butBorderSize;
	recLineBelowTitle[2] = recBckgr[2];
	recLineBelowTitle[3] = recLineBelowTitle[1]+butBorderSize;

	double recHotKeyInfo[4];
	recHotKeyInfo[0] = recBckgr[0];
	recHotKeyInfo[1] = recLineBelowTitle[1]-0.03;
	recHotKeyInfo[2] = recBckgr[2];
	recHotKeyInfo[3] = recLineBelowTitle[1];
	textHotKeyInfo.str = "press ";
	textHotKeyInfo.str += (char)HetuwMod::charKey_Phex;
	textHotKeyInfo.str += " or CTRL+";
	textHotKeyInfo.str += (char)HetuwMod::charKey_Phex;
	textHotKeyInfo.str += " to toggle";
	textHotKeyInfo.getWidthHeight();
	textHotKeyInfo.setTextPosToCenter(recHotKeyInfo);
	
	recLineBelowHotKey[0] = recHotKeyInfo[0];
	recLineBelowHotKey[1] = recHotKeyInfo[1]-butBorderSize;
	recLineBelowHotKey[2] = recHotKeyInfo[2];
	recLineBelowHotKey[3] = recHotKeyInfo[1];

	mainChatWindow.rec[3] = recLineBelowHotKey[3] - textInRecPaddingY;

	mainFont->hetuwMaxXActive = true;

	minimize();
}

void Phex::initFont() {
	int fontCharSpacing = 3; // vanilla main font is 6
	int fontSpaceWidth = 8; // vanilla main font is 16
	char fontFixedWidth = false;
	mainFont = new HetuwFont(getFontTGAFileName(), fontCharSpacing, fontSpaceWidth, fontFixedWidth, mainFontScaleFactor);
	mainFont->setMinimumPositionPrecision( 1 );
	mainFont->hetuwMaxXActive = true;
	setMainFontScale();
	fontSetMaxX();
}

void Phex::fontSetMaxX() {
	if (HetuwMod::phexOnLeft()) {
		mainFont->hetuwMaxX = - HetuwMod::viewWidth/2.0 * 0.4;
	} else {
		mainFont->hetuwMaxX = HetuwMod::viewWidth/2.0;
	}
	mainFont->hetuwMaxX += lastScreenViewCenter.x;
	mainFont->hetuwMaxX -= textInRecPaddingX*HetuwMod::viewWidth;
	mainFont->hetuwMaxX = round(mainFont->hetuwMaxX); 
}

void Phex::setMainFontScale() {
	if (!mainFont) return;
	//mainFont->hetuwSetScaleFactor(mainFontScaleFactor * HetuwMod::guiScale * textScale);
	mainFont->hetuwSetScaleFactor(mainFontScaleFactor * textScale * HetuwMod::zoomScale);
}

void Phex::onGuiScaleChange() {
	//setMainFontScale();
}

void Phex::setButtonStyle(Button *but) {
	but->bDrawBorder = true;
	setArray(but->colorBckgr, (const float[]){ 0.0f, 0.0f, 0.0f, 0.0f }, 4);
	but->setBorderSize(butBorderSize);
}

void Phex::initButtons() {
	double recBckgrWidth = HetuwMod::getRecWidth(recBckgr);
	double butWidth = recBckgrWidth/2.0 + butBorderSize/2.0;

	butPhex.init("PH", &onClickPhex);
	setButtonStyle(&butPhex);
	double butPhexWidth = 0.03;
	double butPhexHeight = butPhexWidth * HetuwMod::viewWidthToHeightFactor;
	double butPhexPaddingX = 0.01;
	double butPhexPaddingY = butPhexPaddingX * HetuwMod::viewWidthToHeightFactor;
	if (HetuwMod::phexOnLeft()) {
		butPhex.setPosition(butPhexPaddingX, butPhexPaddingY);
	} else {
		butPhex.setPosition(1.0-butPhexWidth-butPhexPaddingX, butPhexPaddingY);
	}
	butPhex.setWidth(butPhexWidth);
	butPhex.setHeight(butPhexHeight);
	setArray(butPhex.colorBckgr, colorButPhexOffline, 4);
	butPhex.text.scale = 1.2;
	butPhex.setDrawData();
	butPhex.visible = false;
	buttons.push_back(&butPhex);

	butTurnOff.init(HetuwMod::phexStartOffline ? "Turn On" : "Turn Off", &onClickTurnOff);
	setButtonStyle(&butTurnOff);
	butTurnOff.setPosition(recBckgr[0], recBckgr[3]-butHeight);
	butTurnOff.setWidth(butWidth);
	butTurnOff.setHeight(butHeight);
	butTurnOff.setDrawData();
	buttons.push_back(&butTurnOff);

	butMinimize.init("Minimize", &minimize);
	setButtonStyle(&butMinimize);
	butMinimize.setPosition(recBckgr[0]+butWidth-butBorderSize/2.0, recBckgr[3]-butHeight);
	butMinimize.setWidth(butWidth-butBorderSize/2.0);
	butMinimize.setHeight(butHeight);
	butMinimize.setDrawData();
	buttons.push_back(&butMinimize);

	butMaximize.init("Maximize", &maximize);
	setButtonStyle(&butMaximize);
	if (HetuwMod::phexOnLeft()) {
		butMaximize.setPosition(0, 0);
	} else {
		butMaximize.setPosition(1.0-recBckgrWidth, 0);
	}
	butMaximize.setWidth(recBckgrWidth);
	butMaximize.setHeight(butHeight);
	setArray(butMaximize.colorBckgr, colorRecBckgr, 4);
	butMaximize.setDrawData();
	butMaximize.visible = false;
	buttons.push_back(&butMaximize);
}

void Phex::initServerCommands() {
	serverCommands["VERSION"].func = &serverCmdVERSION;
	serverCommands["VERSION"].minWords = 2;
	serverCommands["HASH"].func = serverCmdHASH;
	serverCommands["HASH"].minWords = 2;
	serverCommands["USERNAME"].func = serverCmdUSERNAME;
	serverCommands["USERNAME"].minWords = 2;
	serverCommands["USERNAME_ERR"].func = serverCmdUSERNAME_ERR;
	serverCommands["USERNAME_ERR"].minWords = 2;
	serverCommands["SAY"].func = &serverCmdSAY;
	serverCommands["SAY"].minWords = 5;
	serverCommands["SAY_RAW"].func = &serverCmdSAY_RAW;
	serverCommands["SAY_RAW"].minWords = 2;
	serverCommands["HASH_USERNAME"].func = serverCmdHASH_USERNAME;
	serverCommands["HASH_USERNAME"].minWords = 2;
	serverCommands["ONLINE"].func = serverCmdONLINE;
	serverCommands["ONLINE"].minWords = 2;
	serverCommands["OFFLINE"].func = serverCmdOFFLINE;
	serverCommands["OFFLINE"].minWords = 2;
	serverCommands["JOINED_CHANNEL"].func = serverCmdJOINED_CHANNEL;
	serverCommands["JOINED_CHANNEL"].minWords = 3;
	serverCommands["LEFT_CHANNEL"].func = serverCmdLEFT_CHANNEL;
	serverCommands["LEFT_CHANNEL"].minWords = 3;
	serverCommands["DISCONNECT"].func = serverCmdDISCONNECT;
	serverCommands["DISCONNECT"].minWords = 1;
	serverCommands["CLOSE"].func = serverCmdDISCONNECT;
	serverCommands["CLOSE"].minWords = 1;
	serverCommands["COORD"].func = serverCmdCOORD;
	serverCommands["COORD"].minWords = 5;
	serverCommands["SEND_BIOMES"].func = serverCmdSEND_BIOMES;
	serverCommands["SEND_BIOMES"].minWords = 2;
	serverCommands["SEND_POSITION"].func = serverCmdSEND_POSITION;
	serverCommands["SEND_POSITION"].minWords = 2;
	serverCommands["HASH_SERVER_LIFE"].func = serverCmdHASH_SERVER_LIFE;
	serverCommands["HASH_SERVER_LIFE"].minWords = 4;
	serverCommands["GET_ALL_PLAYERS"].func = serverCmdGET_ALL_PLAYERS;
	serverCommands["GET_ALL_PLAYERS"].minWords = 1;
	serverCommands["JASON_AUTH"].func = serverCmdJASON_AUTH;
	serverCommands["JASON_AUTH"].minWords = 2;
	serverCommands["IDK"].func = serverCmdIDK;
	serverCommands["IDK"].minWords = 1;
}

void Phex::serverCmdVERSION(std::vector<std::string> input) {

}

void Phex::serverCmdHASH(std::vector<std::string> input) {
	createUser(input[1], false);
	publicHash = input[1];
	users[input[1]].online = true;
}

void Phex::serverCmdUSERNAME(std::vector<std::string> input) {
	if (publicHash.length() < 1) {
		printf("Phex Error received our username but we dont have our hash");
		printf("Phex Error message: %s\n", joinStr(input).c_str());
		return;
	}
	createUser(input[1], false);
	users[publicHash].name = input[1];
	users[publicHash].displayName = users[publicHash].name;
	if (userNameWasChanged) {
		addCmdMessageToChatWindow("name set to: "+input[1]);
		userNameWasChanged = false;
	}
}

void Phex::serverCmdUSERNAME_ERR(std::vector<std::string> input) {
	std::string msg = joinStr(input, " ", 1);
	addCmdMessageToChatWindow(msg, CMD_MSG_ERROR);
}

void Phex::serverCmdSAY(std::vector<std::string> input) {
	std::string hash = input[2];
	createUser(hash, true);

	if (blockedUsers.count(hash)) {
		return;
	}

	ChatElement chatElement;
	chatElement.hash = hash;

	chatElement.unixTimeStamp = strToTimeT(input[3]);
	chatElement.text = joinStr(input, " ", 4);

	chatElement.name = string(*getUserDisplayName(chatElement.hash));

	chatElement.textToDraw = colorCodeNamesInChat+chatElement.name+": "+colorCodeWhite+chatElement.text;
	mainChatWindow.addElement(chatElement);
}

void Phex::serverCmdSAY_RAW(std::vector<std::string> input) {
	addCmdMessageToChatWindow(joinStr(input, " ", 1));
}

void Phex::serverCmdHASH_USERNAME(std::vector<std::string> input) {
	createUser(input[1], false);
	users[input[1]].name = input[2];
	users[input[1]].displayName = input[2];
}

void Phex::serverCmdONLINE(std::vector<std::string> input) {
	// ONLINE is not considered activity since every idle player gets an ONLINE
	// message on first connect
	createUser(input[1], false);
	users[input[1]].online = true;
}

void Phex::serverCmdOFFLINE(std::vector<std::string> input) {
	createUser(input[1], false);
	users[input[1]].online = false;
}

void Phex::serverCmdJOINED_CHANNEL(std::vector<std::string> input) {
	createUser(input[1], true);
	users[input[1]].channel = input[2];
}

void Phex::serverCmdLEFT_CHANNEL(std::vector<std::string> input) {
	createUser(input[1], false);
	users[input[1]].channel = "";
}

void Phex::serverCmdDISCONNECT(std::vector<std::string> input) {
	tcp.reconnect();
}

void Phex::serverCmdCOORD(std::vector<std::string> input) {
	if (!allowServerCoords) return;
	//COORD x y color name
	HetuwMod::HomePos *coord = new HetuwMod::HomePos();
	try {
		coord->x = stoi(input[1]);
		coord->y = stoi(input[2]);
		hexToColors(input[3], coord->rgba, 3);
		coord->rgba[3] = 1.0f;
		coord->hasCustomColor = true;
		coord->text = joinStr(input, " ", 4);
		strToUpper(coord->text);
		coord->type = HetuwMod::hpt_phex;
		HetuwMod::addHomeLocation(coord);
	} catch(std::exception const & ex) {
		printf("Phex EXCEPTION when receiving COORD command\n");
		printf("Phex command: %s\n", joinStr(input, " ", 0).c_str());
		printf("Phex EXCEPTION: %s\n", ex.what());
		return;
	}
}

void Phex::serverCmdSEND_BIOMES(std::vector<std::string> input) {
	if (strEquals(input[1], "0")) {
		sendBiomeDataActive = false;
		if (HetuwMod::bDrawBiomeInfo) printf("Phex turned sending biome info OFF\n");
	} else if (strEquals(input[1], "1")) {
		sendBiomeDataActive = true;
		if (HetuwMod::bDrawBiomeInfo) printf("Phex turned sending biome info ON\n");
	} else {
		printf("Phex unknown argument '%s' for command %s\n", input[1].c_str(), input[0].c_str());
		printf("Phex argument can be 0 or 1");
	}
}

void Phex::serverCmdSEND_POSITION(std::vector<std::string> input) {
	if (strEquals(input[1], "0")) {
		sendPositionActive = false;
	} else if (strEquals(input[1], "1")) {
		sendPositionActive = true;
	} else {
		printf("Phex unknown argument '%s' for command %s\n", input[1].c_str(), input[0].c_str());
		printf("Phex argument can be 0 or 1");
	}
}

// HASH_SERVER_LIFE c32b6353fb5b4d705593 bigserver2.onehouronelife.com 3031046
void Phex::serverCmdHASH_SERVER_LIFE(std::vector<std::string> input) {
	try {
		if (!strEquals(input[2], string(HetuwMod::serverIP))) return;
		int playerID = stoi(input[3]);
		createUser(input[1], false);
		playerIdToHash[playerID] = input[1];
		users[input[1]].inGameServerPlayerID = playerID;
	} catch(std::invalid_argument const &) {
		printf("Phex HASH_SERVER_LIFE playerID invalid\n");
	} catch(std::out_of_range const &) {
		printf("Phex HASH_SERVER_LIFE playerID out of range\n");
	}
}

void Phex::serverCmdGET_ALL_PLAYERS(std::vector<std::string> input) {
	if (!HetuwMod::gameObjects) return;

	std::string str = "";
	str += "ALL_PLAYERS ";
	str += string(HetuwMod::serverIP)+" ";
	for(int i=0; i<HetuwMod::gameObjects->size(); i++) {
		LiveObject *o = HetuwMod::gameObjects->getElement( i );
		if (!o) continue;
		ObjectRecord* obj = getObject(o->displayID);
		if (!obj) continue;

		str += to_string(o->id);
		string inRange = o->outOfRange ? "0" : "1";
		str += " "+inRange;
		string alive = o->finalAgeSet ? "D" : "A";
		str += " "+alive;
		str += " "+to_string(obj->race - 1 + 'A');
		string gender = obj->male ? "M" : "F";
		str += " "+gender;
		str += " "+to_string((int)HetuwMod::livingLifePage->hetuwGetAge(o));
		str += " "+to_string(o->xd)+" "+to_string(o->yd);

		if (o->name) str += " "+string(o->name);

		str += ",";
	}
	//printf("Phex %s\n", str.c_str());
	tcp.send(str);
}

void Phex::serverCmdJASON_AUTH(std::vector<std::string> input) {
	std::string const &challenge = input[1];

	// Require the challenge to start with "phex_" to prevent a trivial
	// credential forwarding attack. This would still allow a Phex server to
	// forward credentials to other Phex servers, but that's more of a feature
	// than a bug since Phex proxies have been desired in the past.
	if (challenge.find("phex_") != 0) {
		addCmdMessageToChatWindow("The Phex server sent an invalid JASON_AUTH challenge. Disconnecting.", CMD_MSG_ERROR);
		tcp.disconnect();
		return;
	}

	// If the authentication email isn't a bogus Steam one, require the user to
	// opt in to sending this personal detail to the Phex server.
	if (strstr(userEmail, "@steamgames.com") == NULL) {
		if (temporaryJasonAuthOptIn) {
			addCmdMessageToChatWindow("To permanently opt in to sending your email to the Phex server, set phex_send_email in " hetuwSettingsFileName ".", CMD_MSG_ERROR);
		} else if (!HetuwMod::phexSendEmail) {
			addCmdMessageToChatWindow("This Phex server requires your email address for account verification.", CMD_MSG_ERROR);
			addCmdMessageToChatWindow("To opt in, say: .OPTIN", CMD_MSG_ERROR);
			tcp.disconnect();
			return;
		}
	}

	char *pureKey = getPureAccountKey();
	char *keyHash = hmac_sha1(pureKey, challenge.c_str());

	std::stringstream ss;
	ss << "JASON_AUTH " << userEmail << " " << keyHash;
	tcp.send(ss.str());

	delete [] pureKey;
	delete [] keyHash;
}

void Phex::serverCmdIDK(std::vector<std::string> input) {
	printf("Phex IDK: %s\n", joinStr(input).c_str());
}

void Phex::initChatCommands() {
	chatCommands["HELP"].func = chatCmdHELP;
	chatCommands["HELP"].minWords = 1;
	chatCommands["HELP"].helpStr = "Lists all commands";
	chatCommands["HELP"].allowOffline = true;

	chatCommands["NAME"].func = chatCmdNAME;
	chatCommands["NAME"].minWords = 2;
	chatCommands["NAME"].helpStr = "You can change your name by typing:\n"+strCmdChar+"name [newName]";

	chatCommands["LIST"].func = chatCmdLIST;
	chatCommands["LIST"].minWords = 1;
	chatCommands["LIST"].helpStr = "Lists all online players";

	chatCommands["BLOCK"].func = chatCmdBLOCK;
	chatCommands["BLOCK"].minWords = 2;
	chatCommands["BLOCK"].helpStr = "Block a user's messages (until exit)";

	chatCommands["LIFE"].func = chatCmdLIFE;
	chatCommands["LIFE"].minWords = 1;
	chatCommands["LIFE"].helpStr = "Sends your real life ID to server";

	chatCommands["OPTIN"].func = chatCmdOPTIN;
	chatCommands["OPTIN"].minWords = 1;
	chatCommands["OPTIN"].helpStr = "Opt in to sending your email to the Phex server";
	chatCommands["OPTIN"].allowOffline = true;

	chatCommands["TEST"].func = chatCmdTEST;
	chatCommands["TEST"].minWords = 1;
	chatCommands["TEST"].helpStr = "For testing - dont use";
}

void Phex::chatCmdHELP(std::vector<std::string> input) {
	for (std::pair<std::string, ChatCommand> element : chatCommands) {
		strToLower(element.first);
		if (strEquals(element.first, "test") || (strEquals(element.first, "life") && !bSendFakeLife)) continue;
		addCmdMessageToChatWindow(strCmdChar+element.first);
		addCmdMessageToChatWindow(element.second.helpStr);
	}
}

void Phex::chatCmdNAME(std::vector<std::string> input) {
	tcp.send("USERNAME "+input[1]);
	userNameWasChanged = true;
}

void Phex::chatCmdLIST(std::vector<std::string> input) {
	for (std::pair<std::string, User> element : users) {
		User *user = &element.second;
		if (!user->online) continue;
		
		LiveObject *player = NULL;
		if (user->inGameServerPlayerID >= 0) {
			player = HetuwMod::livingLifePage->getLiveObject(user->inGameServerPlayerID);
		}

		if (player || (time(NULL) - user->lastSeen) < 15*60) {
			std::string str = "";
			if (element.first.length() > ChatElement::maxHashDisplayLength) {
				str += element.first.substr(0, ChatElement::maxHashDisplayLength);
			} else {
				str += element.first;
			}
			if (user->name.length() > 0) str += " "+colorCodeNamesInChat+user->name;
			if (player) {
				const char *name = player->name;
				if (name == NULL || name[0] == 0) {
					name = "(unnamed)";
				}
				str += " "+colorCodeCmdInGameNames+string(name);
			}

			addCmdMessageToChatWindow(str);
		}

		/* log the hashes in addition to displaying them */
		std::stringstream ss;
		ss << element.first << hetuwLogSeperator << user->name << hetuwLogSeperator;
		if (player) {
			ss << user->inGameServerPlayerID;
		}
		ss << hetuwLogSeperator;
		if (player && player->name) {
			ss << player->name;
		}
		HetuwMod::writeLineToLogs("phex_list", ss.str());
	}
}

void Phex::chatCmdBLOCK(std::vector<std::string> input) {
	std::string matchedHash;
	for (std::pair<std::string, User> element : users) {
		User &user = element.second;
		std::string *displayName = getUserDisplayName(user.hash);
		if (displayName != NULL && input[1] == *displayName) {
			matchedHash = user.hash;
			break;
		}
	}

	std::stringstream ss;
	if (matchedHash == "") {
		ss << colorCodeCmdMessageError << "No user with that name.";
	} else {
		ss << colorCodeCmdMessage << "Blocking messages from " << input[1] << " (" << matchedHash.substr(0, ChatElement::maxHashDisplayLength) << ")";
		blockedUsers.insert(matchedHash);
	}
	addCmdMessageToChatWindow(ss.str());
}

void Phex::chatCmdLIFE(std::vector<std::string> input) {
	if (bSendFakeLife) {
		sendServerLife(HetuwMod::ourLiveObject->id);
	}
}

void Phex::chatCmdOPTIN(std::vector<std::string> input) {
	if (HetuwMod::phexSendEmail || temporaryJasonAuthOptIn) {
		addCmdMessageToChatWindow("You have already opted in to sending your email to the Phex server.");
		return;
	}

	temporaryJasonAuthOptIn = true;
	tcp.reconnect();

	addCmdMessageToChatWindow("Opted in. Set phex_send_email in " hetuwSettingsFileName " to opt in permanently.");
	addCmdMessageToChatWindow("Reconnecting...");
}

void Phex::chatCmdTEST(std::vector<std::string> input) {
	if (true) return;
	serverCmdGET_ALL_PLAYERS(input);
	LiveObject *o = HetuwMod::ourLiveObject;
	ObjectRecord* obj = getObject(o->displayID);
	printf("Phex numSprites: %d", obj->numSprites);
	std::string str = "";
	for (int i=0; i< obj->numSprites; i++) {
		str = "";
		str += " "+to_string((int)(obj->spriteColor[i].r*255));
		str += " "+to_string((int)(obj->spriteColor[i].g*255));
		str += " "+to_string((int)(obj->spriteColor[i].b*255));
		printf("Phex %d: %s\n", i, str.c_str());
	}
}

void Phex::setArray(float arrDst[], const float arrSrc[], int size) {
	for (int i=0; i<size; i++) arrDst[i] = arrSrc[i];
}

void Phex::setArray(double arrDst[], const double arrSrc[], int size) {
	for (int i=0; i<size; i++) arrDst[i] = arrSrc[i];
}

void Phex::multipleArray(double arr[], double factor, int size) {
	for (int i=0; i<size; i++) arr[i] *= factor;
}

void Phex::hexToColors(std::string& hex, float rgba[], int size) {
	if (size < 0) size = hex.length()/2;
	for (int i=0; i<size; i++) {
		std::string s = "0x"+hex.substr(i*2, 2);
		unsigned int x = std::stoul(s, nullptr, 16);
		rgba[i] = x / 255.0f;
	}
}

std::string Phex::colorToHex(float f) {
	if (f > 1.0f) f = 1.0f;
	if (f < 0.0f) f = 0.0f;
	float a = f*255;
	float b = a/16;
	float c = b-(int)b;
	int digitA = (int)b;
	int digitB = (int)(c*16);
	//printf("Phex f:%f, a:%f, b:%f, c:%f, dA %d %c dB %d %c\n", f, a, b, c, digitA, hexDigits[digitA], digitB, hexDigits[digitB]);
	string str = ""; str += hexDigits[digitA]; str += hexDigits[digitB];
	return str;
}

std::string Phex::colorsToHex(float rgba[], int size) {
	std::string str="";
	for (int i=0; i<size; i++) {
		str += colorToHex(rgba[i]);
	}
	return str;
}

bool Phex::strEquals(std::string strA, std::string strB) {
	return strA.compare(strB) == 0;
}

void Phex::strToUpper(std::string &str) {
	for (size_t i=0; i < str.length(); i++) str[i] = toupper(str[i]);
}

void Phex::strToLower(std::string &str) {
	for (size_t i=0; i < str.length(); i++) str[i] = tolower(str[i]);
}

std::string Phex::joinStr(std::vector<std::string> strVector, string seperator, int offset) {
	std::string str = "";
	for (unsigned i=offset; i<strVector.size(); i++) {
		str += strVector[i];
		if (i+1 != strVector.size()) str += seperator;
	}
	return str;
}

doublePair Phex::getStringWidthHeight(doublePair startPos, string str) {
	HetuwMod::pointFromPercentToMapCoords(startPos.x, startPos.y);
	SimpleVector<doublePair> outPos;
	mainFont->getCharPos(&outPos, str.c_str(), startPos, alignLeft);
	doublePair widthHeight = { mainFont->hetuwWidth, mainFont->hetuwHeight };
	widthHeight.x /= HetuwMod::viewWidth; widthHeight.y /= HetuwMod::viewHeight;
	return widthHeight;
}

double Phex::getLineHeight(HetuwFont *font) {
	double lineHeight = font->getFontHeight();
	lineHeight /= HetuwMod::viewHeight;
	return lineHeight;
}

void Phex::drawString(string str, doublePair startPos) {
	HetuwMod::pointFromPercentToMapCoords(startPos.x, startPos.y);
	startPos.y -= mainFont->hetuwGetHalfSpriteHeight();
	mainFont->drawString(str.c_str(), startPos, alignLeft);
}

void Phex::Text::getWidthHeight() {
	double scaleTemp = mainFont->hetuwGetScaleFactor();
	mainFont->hetuwSetScaleFactor(scaleTemp * scale);

	doublePair pos = drawStartPos;
	HetuwMod::pointFromPercentToMapCoords(pos.x, pos.y);
	SimpleVector<doublePair> outPos;
	mainFont->getCharPos(&outPos, str.c_str(), pos, alignLeft);
	width = mainFont->hetuwWidth / HetuwMod::viewWidth;
	height = mainFont->hetuwHeight / HetuwMod::viewHeight;

	doublePair cursorPos = mainFont->hetuwNextCharPos;
	cursorPos.y -= mainFont->getFontHeight() * 0.95;
	double halfSpaceWidth = mainFont->hetuwGetSpaceWidth() / 2;
	cursorRec[0] = cursorPos.x - halfSpaceWidth;
	cursorRec[2] = cursorPos.x + halfSpaceWidth;
	double halfSpriteHeight = mainFont->hetuwGetHalfSpriteHeight() * 0.8;
	cursorRec[1] = cursorPos.y - halfSpriteHeight;
	cursorRec[3] = cursorPos.y + halfSpriteHeight;
	double cursorPaddingY = halfSpriteHeight*0.15;
	cursorRec[1] += cursorPaddingY;
	cursorRec[3] += cursorPaddingY;
	HetuwMod::recFromMapToPercentCoords(cursorRec);
	HetuwMod::addToRec(cursorRec, -drawStartPos.x, -drawStartPos.y);

	mainFont->hetuwSetScaleFactor(scaleTemp);
}

void Phex::Text::setTextPosToCenter(double rec[4]) {
	double recWidthHalf = (rec[2]-rec[0])/2.0;
	double recHeightHalf = (rec[3]-rec[1])/2.0;
	drawStartPos.x = rec[0] + recWidthHalf - width/2.0;
	drawStartPos.y = rec[1] + recHeightHalf + height/2.0;

	double scaleTemp = mainFont->hetuwGetScaleFactor();
	mainFont->hetuwSetScaleFactor(scaleTemp * scale);

	// substract the special height of chracters like 'g' that go below the normal text position
	drawStartPos.y -= mainFont->hetuwGetHalfSpriteHeight()/8/HetuwMod::viewHeight;

	mainFont->hetuwSetScaleFactor(scaleTemp);
}

void Phex::Text::draw() {
	double scaleTemp = mainFont->hetuwGetScaleFactor();
	mainFont->hetuwSetScaleFactor(scaleTemp * scale);
	drawString(str, drawStartPos);
	mainFont->hetuwSetScaleFactor(scaleTemp);

	if (!bDrawCursor) return;
	if (lastChangedTime+1 < HetuwMod::curStepTime) {
		if ((unsigned long)(HetuwMod::curStepTime*2.0) % 2 == 0) return;
	}
	double drawRec[4];
	setArray(drawRec, cursorRec, 4);
	HetuwMod::addToRec(drawRec, drawStartPos.x, drawStartPos.y);
	HetuwMod::hDrawRecFromPercent(drawRec);
}

void Phex::createUser(std::string &hash, bool active) {
	users[hash].hash = hash;
	if (active) {
		users[hash].lastSeen = time(NULL);
	}
}

std::string* Phex::getUserDisplayName(std::string &hash) {
	createUser(hash, false);
	if (users[hash].displayName.length() > 0) return &users[hash].displayName;
	users[hash].displayName = string(users[hash].name);
	if (users[hash].displayName.length() < 1) {
		users[hash].displayName = users[hash].hash;
		if (users[hash].displayName.length() > ChatElement::maxHashDisplayLength)
			users[hash].displayName = users[hash].displayName.substr(0, ChatElement::maxHashDisplayLength);
	}
	return &users[hash].displayName;
}

time_t Phex::strToTimeT(std::string str) {
	return (time_t) strtol(str.c_str(), NULL, 10); // on some architectures it may be 32 bit integer and the cast will cause an overflow
}

void Phex::onZoom() {
	setMainFontScale();
	mainChatWindow.onZoom();
}

void Phex::ChatWindow::addElement(ChatElement element) {
	doublePair pos = {rec[0], rec[1]};
	doublePair widthHeight = getStringWidthHeight(pos, element.textToDraw);
	element.textHeight = widthHeight.y;
	elements.push_back(element);
	if (!hasFocus || scrollPos == (int)elements.size() - 2) {
		scrollToBottom();
	}
}

void Phex::ChatWindow::onZoom() {
	if (true) return;
	textDrawPos.x = rec[0]*HetuwMod::viewWidth - (HetuwMod::viewWidth/2);
	textDrawPos.y = rec[1]*HetuwMod::viewHeight - (HetuwMod::viewHeight/2);
	for(int i=(int)elements.size()-1; i>=0; i--) {
		elements[i].textHeightScaled = elements[i].textHeight * HetuwMod::zoomScale;
		textDrawPos.y += elements[i].textHeightScaled;
	}
}

bool Phex::ChatWindow::onScroll(int dir) {
	scrollPos -= dir;
	if (scrollPos < 0) {
		scrollPos = 0;
	} else if (scrollPos >= (int)elements.size()) {
		scrollPos = (int)elements.size() - 1;
	}
	return true;
}

void Phex::ChatWindow::draw(bool bDraw) {
	float x = rec[0];
	float y = rec[1];
	topMinimum = 0;

	if (bDraw) setDrawColor(1.0f, 1.0f, 1.0f, 1.0f);

	// This should not happen, but making sure before we index with it
	if (scrollPos >= (int)elements.size()) {
		scrollPos = (int)elements.size() - 1;
	}

	for(int i=(int)scrollPos; i>=0; i--) {
		if (msgDisplayDur > 0)
			if ((int)(scrollPos + 1 - i) > drawMaxElements)
				if (elements[i].unixTimeStamp+msgDisplayDur < HetuwMod::curStepSecondsSince1970) break;
		y += elements[i].textHeight;
		if (y > rec[3]) break;
		topMinimum = y;
		if (bDraw) drawString(elements[i].textToDraw.c_str(), {x, y});
	}

	if (bDraw && isScrolledUp()) {
		drawString("...", { rec[0], rec[1] + HetuwMod::zoomScale*0.005 });
	}
}

double Phex::ChatWindow::getTopMinimum() {
	draw(false);
	return topMinimum;
}

void Phex::addCmdMessageToChatWindow(std::string msg, int type) {
	std::string colorCode = "";
	if (type == CMD_MSG_ERROR) colorCode = colorCodeCmdMessageError;
	else colorCode = colorCodeCmdMessage;

	ChatElement element;
	element.unixTimeStamp = time(NULL);
	element.textToDraw = colorCode + msg + colorCodeWhite;
	mainChatWindow.addElement(element);
}

void Phex::Button::draw() {
	if (!visible) return;
	HetuwMod::hSetDrawColor(colorBckgr);
	HetuwMod::hDrawRecFromPercent(rec);
	if (highlightIsActive) {
		HetuwMod::hSetDrawColor(colorBckgrHighlight);
		HetuwMod::hDrawRecFromPercent(rec);
	}
	setDrawColor(1.0, 1.0, 1.0, 1.0);
	if (bDrawBorder) {
		HetuwMod::hDrawRecsFromPercent(borderRecs, 4);
	}
	text.draw();
}

void Phex::Button::calcTextDimensions() {
	text.getWidthHeight();
	textPaddingY = text.height * textPadding;
	textPaddingX = textPaddingY * HetuwMod::viewHeightToWidthFactor * 1.05;
}

void Phex::Button::calcRecDimensions() {
	if (!widthIsSet)  rec[2] = rec[0] + text.width +  (textPaddingX*2);
	if (!heightIsSet) rec[3] = rec[1] + text.height + (textPaddingY*2);
}

void Phex::Button::calcDimensions() {
	calcTextDimensions();
	calcRecDimensions();
}

void Phex::Button::setBorderRecs() {
	double borderHeight = borderSize;
	double borderWidth = borderSize * HetuwMod::viewHeightToWidthFactor;
	if (!borderSizeIsSet) {
		borderHeight = HetuwMod::getRecHeight(rec) * 0.08;
		borderWidth = borderHeight * HetuwMod::viewHeightToWidthFactor;
	}
	HetuwMod::set4BorderRecs(rec, borderRecs, borderWidth, borderHeight);
}

void Phex::Button::setDrawData() {
	calcDimensions();
	text.setTextPosToCenter(rec);
	if (bDrawBorder) setBorderRecs();
}

void Phex::setInputRecDrawData() {
	recInput[0] = recBckgr[0]+(textInRecPaddingX/2);
	recInput[2] = recBckgr[2]-(textInRecPaddingX/2);
	recInput[1] = recBckgr[1]+textInRecPaddingY;
	inputText.drawStartPos.x = recInput[0]+(textInRecPaddingX/2);
	inputText.drawStartPos.y = recInput[1];

	inputText.getWidthHeight();
	double height = inputText.height;

	recInput[3] = recInput[1]+height;
	inputText.drawStartPos.y = recInput[3]+(textInRecPaddingY*0.07);
	recInput[3] += textInRecPaddingY/2.0;
}

void Phex::drawInputRec() {
	HetuwMod::hSetDrawColor(colorRecInput);
	HetuwMod::hDrawRecFromPercent(recInput);
	setDrawColor(1.0, 1.0, 1.0, 1.0);
	inputText.draw();
}

void Phex::handleChatCommand(std::string input) {
	std::vector<std::string> splittedMsg = HetuwMod::splitStrXTimes(input, ' ', 99);
	std::string command = splittedMsg[0];
	command = command.substr(1, command.length());
	strToUpper(command);
	if (command.length() <= 0) {
		addCmdMessageToChatWindow("invalid command", CMD_MSG_ERROR);
		return;
	}

	if (chatCommands.find(command) == chatCommands.end()) {
		addCmdMessageToChatWindow("unknown command "+command, CMD_MSG_ERROR);
		return;
	}
	if (chatCommands[command].minWords > (int)splittedMsg.size()) {
		addCmdMessageToChatWindow("command needs atleast "+to_string(chatCommands[command].minWords-1)+" arguments", CMD_MSG_ERROR);
		return;
	}
	if (tcp.status != TCPConnection::ONLINE && !chatCommands[command].allowOffline) {
		addCmdMessageToChatWindow("You are not connected to the Phex server.", CMD_MSG_ERROR);
		return;
	}
	chatCommands[command].func(splittedMsg);
}

void Phex::sendInputStr() {
	if (inputText.str.length() < 1) return;
	if (inputText.str[0] == chatServerCmdChar) {
		std::string sendToServer = "USER_CMD "+inputText.str.substr(1, inputText.str.length()-1);
		tcp.send(sendToServer);
		return;
	}
	if (inputText.str[0] == chatCmdChar) {
		handleChatCommand(inputText.str);
		return;
	}
	tcp.send("SAY "+channelName+" "+inputText.str);

	if (users[publicHash].name.length() < 1) {
		addCmdMessageToChatWindow("to set your name type: /name yourName");
	}
}

bool Phex::addToInputStr(unsigned char c) {
	if (c == 13) { // enter
		if (tcp.status != TCPConnection::ONLINE && inputText.str[0] != chatCmdChar) return true;
		sendInputStr();
		inputText.str = "";
		mainChatWindow.scrollToBottom();
		return true;
	}
	inputText.setLastChangedTime();
	if (c == 8) { // backspace
		inputText.deleteLastChar();
		return true;
	}
	if (c < 32) return false;
	if (inputText.str.length() > PHEX_MAX_INPUT_STR_LENGTH) return false;
	inputText.str += c;
	return true;
}

void Phex::sendFirstMessage() {
	string clientName = "yumlife";
	string phexVersionNumber = to_string(PHEX_VERSION);
	string secretHash = getSecretHash();
	string jasonsOneLifeVersion = to_string(versionNumber);
	string msg = "FIRST "+clientName+" "+phexVersionNumber+" "+secretHash+" "+jasonsOneLifeVersion;
	tcp.send(msg);
}

void Phex::joinChannel(std::string inChannelName) {
	if (channelName.length() > 0) tcp.send("LEAVE "+channelName);
	channelName = inChannelName;
	tcp.send("JOIN "+channelName);
	mainChatWindow.clear();
	tcp.send("GETLAST "+channelName+" 30");
	sendServerLife(bSendFakeLife ? 1 : HetuwMod::ourLiveObject->id);
	if (!HetuwMod::phexSkipTOS) {
		tcp.send("USER_CMD tos");
	}
}

void Phex::sendServerLife(int life) {
	std::string msg = "SERVER_LIFE ";
	msg += std::string(HetuwMod::serverIP)+" ";
	msg += std::to_string(life);
	tcp.send(msg);
}

void Phex::draw() {
	if (!HetuwMod::phexIsEnabled) return;

	lifeStarted = true;
	if (HetuwMod::bDrawBiomeInfo) testDrawBiomeChunks();
	fontSetMaxX();
	keyHandler.step();
	if (sendBiomeDataActive && intervalSendBiomeData.step()) loopBiomeChunks();
	if (sendPositionActive && intervalSendPosition.step()) sendPosition();

	if (isMinimized) drawMinimized();
	else drawNormal();
}

void Phex::drawNormal() {
	setInputRecDrawData();

	HetuwMod::hSetDrawColor(colorRecBckgr);
	HetuwMod::hDrawRecFromPercent(recBckgr);

	setDrawColor(1.0, 1.0, 1.0, 1.0);
	mainChatWindow.rec[1] = recInput[3] + textInRecPaddingY;
	mainChatWindow.draw();

	butTurnOff.draw();
	butMinimize.draw();

	//setDrawColor(0.6, 0.0, 1.0, 0.5);
	//HetuwMod::hDrawRecFromPercent(titleText.rec);
	titleText.draw();

	setDrawColor(1.0, 1.0, 1.0, 1.0);
	HetuwMod::hDrawRecFromPercent(recLineBelowTitle);
	textHotKeyInfo.draw();
	HetuwMod::hDrawRecFromPercent(recLineBelowHotKey);

	drawInputRec();
}

void Phex::drawMinimized() {
	if (bDrawRecInput) {
		setInputRecDrawData();
		mainChatWindow.rec[1] = recInput[3] + textInRecPaddingY;
		mainChatWindow.msgDisplayDur = ChatWindow::messageDisplayDurationInSec;
		mainChatWindow.drawMaxElements = 4;
	} else {
		mainChatWindow.rec[1] = textInRecPaddingY;
		mainChatWindow.msgDisplayDur = ChatWindow::messageDisplayDurationInSec;
		mainChatWindow.drawMaxElements = 0;
	}

	butMaximize.visible = false;

	double chatTop = mainChatWindow.getTopMinimum();
	if (chatTop <= 0 && !bDrawRecInput) {
		butPhex.visible = true;
		butPhex.draw();
		return;
	}
	butPhex.visible = false;

	if (chatTop <= 0) {
		recBckgr[3] = mainChatWindow.rec[1];
	} else {
		recBckgr[3] = chatTop + textInRecPaddingY;
	}

	if (hasFocus) {
		butMaximize.visible = true;
		butMaximize.setY(recBckgr[3]);
		butMaximize.setDrawData();
		recBckgr[3] = butMaximize.rec[3];
	}

	HetuwMod::hSetDrawColor(colorRecBckgr);
	HetuwMod::hDrawRecFromPercent(recBckgr);

	if (hasFocus) butMaximize.draw();

	mainChatWindow.draw();

	if (bDrawRecInput) drawInputRec();
}

void Phex::minimize() {
	if (isMinimized) return;
	isMinimized = true;

	butTurnOff.visible = false;
	butMinimize.visible = false;
	onUpdateFocus(false);
}

void Phex::maximize() {
	if (!isMinimized) return;
	isMinimized = false;

	mainChatWindow.msgDisplayDur = -1;
	mainChatWindow.drawMaxElements = 99999;
	setArray(recBckgr, recBckgrBig, 4);
	butTurnOff.visible = true;
	butMinimize.visible = true;
	inputText.bDrawCursor = true;
	onUpdateFocus(true);
}

string Phex::getSecretHash() {
	char *accKey = getPureAccountKey();
	int keyLength = strlen(accKey)+5;
	char key[keyLength];
	sprintf(key, "%sphex", accKey);
	char *hash = hmac_sha1("phex", key);
	string strHash(hash);
	delete[] hash;
	delete[] accKey;
	return strHash;
}

void Phex::onReceivedMessage(std::string msg) {
	if (msg.length() <= 0) {
		printf("Phex Error: received empty message from server\n");
		return;
	}
	std::vector<std::string> splittedMsg = HetuwMod::splitStrXTimes(msg, ' ', 99);
	std::string command = splittedMsg[0];
	if (command.length() <= 0) {
		printf("Phex Error: received empty command from server\n");
		return;
	}
	if (serverCommands.find(command) == serverCommands.end()) {
		printf("Phex Error: unknown command '%s'\n", command.c_str());
		printf("Phex Error: message: '%s'\n", msg.c_str());
		tcp.send(std::string("IDK ") + command);
		return;
	}
	if (serverCommands[command].minWords > (int)splittedMsg.size()) {
		printf("Phex Error: server message to short, expected atleast %d words, but got %d\n", serverCommands[command].minWords, (int)splittedMsg.size());
		printf("Phex Error: message: '%s'\n", msg.c_str());
		std::stringstream ss;
		ss << "IDK " << command << " " << (splittedMsg.size() - 1);
		tcp.send(ss.str());
		return;
	}
	serverCommands[command].func(splittedMsg);
}

void Phex::onConnectionStatusChanged(TCPConnection::statusType status) {
	/* This causes a re-measure of the status text, so wrapping needs to be
	 * disabled for the duration.
	 */
	mainFont->hetuwMaxXActive = false;

	switch (status) {
		case TCPConnection::UNINITIALIZED:
		case TCPConnection::OFFLINE:
			HetuwMod::writeLineToLogs("phex_status", "offline");
			titleText.setToOffline();
			setArray(butPhex.colorBckgr, colorButPhexOffline, 4);
			break;
		case TCPConnection::CONNECTING:
			HetuwMod::writeLineToLogs("phex_status", "connecting");
			titleText.setToConnecting();
			setArray(butPhex.colorBckgr, colorButPhexConnecting, 4);
			bSendFirstMsg = true;
			break;
		case TCPConnection::ONLINE:
			HetuwMod::writeLineToLogs("phex_status", "online");
			titleText.setToOnline();
			setArray(butPhex.colorBckgr, colorButPhexOnline, 4);
			channelName = "";
			if (bSendFirstMsg) {
				sendFirstMessage();
				bSendFirstMsg = false;
			}
			if (forceChannel.length() > 1) joinChannel(forceChannel);
			else joinChannel(string(HetuwMod::serverIP));
			break;
	}

	mainFont->hetuwMaxXActive = true;
}

void Phex::onClickPhex() {
	onUpdateFocus(true);
}

void Phex::onClickTurnOff() {
	if (strEquals(butTurnOff.text.str, "Turn Off")) {
		tcp.disconnect();
		butTurnOff.text.str = "Turn On";
	} else {
		tcp.connect();
		butTurnOff.text.str = "Turn Off";
	}
	butTurnOff.setDrawData();
}

void Phex::onServerJoin() {
	if (!HetuwMod::phexIsEnabled) return;
}

void Phex::onPhexKeyPressed() {
	if (!hasFocus) {
		onUpdateFocus(true);
		return;
	}
	if (!isMinimized) {
		minimize();
		return;
	}
	onUpdateFocus(false);
}

void Phex::onPhexKeyPressed2() {
	if (isMinimized) {
		maximize();
		onUpdateFocus(true);
		return;
	}
	minimize();
	onUpdateFocus(false);
}

bool Phex::onUpdateFocus(bool focus) {
	if (hasFocus == focus) return false;
	hasFocus = focus;
	bDrawRecInput = hasFocus;
	inputText.bDrawCursor = hasFocus;
	if (!hasFocus) {
		mainChatWindow.scrollToBottom();
	}
	return true;
}

void Phex::onMouseEvent(float x, float y) {
	if (!HetuwMod::phexIsEnabled) return;
	HetuwMod::pointFromMapToPercentCoords(x, y);
	for(unsigned k=0; k<buttons.size(); k++) {
		buttons[k]->onMouseEvent(x, y);
	}
	mouseOverBckgr = HetuwMod::pointIsInsideRec(recBckgr, x, y);
}

bool Phex::onMouseDown(float x, float y) {
	if (!HetuwMod::phexIsEnabled) return false;
	if (HetuwMod::livingLifePage->hetuwIsAutoClick()) return false;
	HetuwMod::pointFromMapToPercentCoords(x, y);
	if (!HetuwMod::pointIsInsideRec(recBckgr, x, y)) {
		onUpdateFocus(false);
		return false;
	}
	for(unsigned k=0; k<buttons.size(); k++) {
		if (buttons[k]->onMouseDown(x, y)) return true;
	}
	if (!butPhex.visible) {
		onUpdateFocus(true);
		return true;
	} else return false;
}

bool Phex::onMouseUp(float x, float y) {
	if (!HetuwMod::phexIsEnabled) return false;
	HetuwMod::pointFromMapToPercentCoords(x, y);
	for(unsigned k=0; k<buttons.size(); k++) {
		if (buttons[k]->onMouseUp(x, y)) return true;
	}
	return false;
}

bool Phex::onScroll(int dir) {
	if (!mouseOverBckgr || !lifeStarted || !hasFocus || !HetuwMod::phexIsEnabled) {
		return false;
	}

	if (mainChatWindow.onScroll(dir)) return true;

	return false;
}

void Phex::onKey(unsigned char inASCII) {
	if (!hasFocus) return;
	addToInputStr(inASCII);
}

bool Phex::onKeyDown(unsigned char inASCII) {
	if (!HetuwMod::phexIsEnabled) return false;
	if (HetuwMod::isCharKey(inASCII, HetuwMod::charKey_Phex)) {
		if (isCommandKeyDown()) onPhexKeyPressed2();
		else onPhexKeyPressed();
		return true;
	}

	// 8 = backspace -> disable keyHandler except for backspace because of unknown bug
	if (inASCII != 8) onKey(inASCII);
	else keyHandler.onKeyDown(inASCII);

	if (!hasFocus) return false;
	return true;
}

bool Phex::onKeyUp(unsigned char inASCII) {
	if (!HetuwMod::phexIsEnabled) return false;
	keyHandler.onKeyUp(inASCII);
	if (!hasFocus) return false;
	return true;
}

void Phex::onRingBell(int x, int y) {
	if (!HetuwMod::phexIsEnabled) return;
	tcp.send("BELL "+to_string(x)+" "+to_string(y)+" "+string(HetuwMod::serverIP));
}

void Phex::onRingApoc(int x, int y) {
	if (!HetuwMod::phexIsEnabled) return;
	tcp.send("APOC "+to_string(x)+" "+to_string(y)+" "+string(HetuwMod::serverIP));
}

void Phex::onBirth() {
	if (!HetuwMod::phexIsEnabled) return;

	if (tcp.status == TCPConnection::ONLINE) {
		std::string expectedChannel;
		if (forceChannel.length() > 1) {
			expectedChannel = forceChannel;
		} else {
			expectedChannel = HetuwMod::serverIP;
		}

		if (channelName != expectedChannel) {
			HetuwMod::writeLineToLogs("phex_status", channelName + " != " + expectedChannel + ", reconnecting");
			tcp.reconnect();
		} else {
			sendServerLife(bSendFakeLife ? 1 : HetuwMod::ourLiveObject->id);
		}
	}
	/*
	for (int x=0; x<biomeChunksSentSize; x++) {
		for (int y=0; y<biomeChunksSentSize; y++) {
			biomeChunksSent[x][y] = 0;
		}
	}
	*/
}

void Phex::onGameStep() {
	if (!HetuwMod::phexIsEnabled) return;

	// Phex originally tcp.step()'d on the livingLifePage draw loop, so this
	// condition is a bit of a kludge to avoid having to rework this module's
	// lifecycle assumptions.
	if (lifeStarted)
		tcp.step();
}

void Phex::sendBiomeChunk(int chunkX, int chunkY) {
	int tileX = chunkX * biomeChunkSize;
	int tileY = chunkY * biomeChunkSize;
	chunkX += biomeChunksSentSize;
	chunkY += biomeChunksSentSize;
	if (chunkX < 0 || chunkY < 0) return;

	unsigned int bitPos = (unsigned int)( (chunkX%2)+(chunkY%2)*2 );
	
	int arrX = chunkX / 2; // each char can hold the information for 4 chunks
	int arrY = chunkY / 2;
	if (arrX >= biomeChunksSentSize || arrY >= biomeChunksSentSize) return;
	char *c = &biomeChunksSent[arrX][arrY];

	if (*c & (1U << bitPos)) return; // bit is already set

	std::string sendStr = "BIOME ";
	sendStr += to_string(tileX) + " ";
	sendStr += to_string(tileY) + " ";
	for (int y=tileY; y < tileY+biomeChunkSize; y++) {
		for (int x=tileX; x < tileX+biomeChunkSize; x++) {
			int mapI = HetuwMod::livingLifePage->hetuwGetMapI(x, y);
			if (mapI < 0) return; // out of range
			int biomeType = HetuwMod::livingLifePage->mMapBiomes[mapI];
			if (biomeType < 0) return; // chunk contains unloaded areas
			sendStr += to_string(biomeType);
		}
	}
	
	if (HetuwMod::bDrawBiomeInfo) { // for debugging
		float *r = new float[4];
		r[0] = tileX * 128 - 64; // 128 == CELL_D
		r[1] = tileY * 128 - 64;
		r[2] = r[0] + biomeChunkSize*128;
		r[3] = r[1] + biomeChunkSize*128;
		biomeChunksDrawRecs.push_back(r);
		printf("Phex %s\n", sendStr.c_str());
	}

	tcp.send(sendStr);
	*c |= (1U << bitPos); // set bit
}

void Phex::loopBiomeChunks() {
	if (!HetuwMod::ourLiveObject) return;
	if (HetuwMod::livingLifePage->hetuwUsesGlobalOffset()) return;

	int chunkX = HetuwMod::ourLiveObject->xd / biomeChunkSize;
	int chunkY = HetuwMod::ourLiveObject->yd / biomeChunkSize;
	chunkX -= biomeChunksPerInterval/2;
	chunkY -= biomeChunksPerInterval/2;

	for (int x=chunkX; x < (chunkX+biomeChunksPerInterval); x++) {
		for (int y=chunkY; y < (chunkY+biomeChunksPerInterval); y++) {
			sendBiomeChunk(x, y);
		}
	}
}

void Phex::testDrawBiomeChunks() {
	for(unsigned k=0; k<biomeChunksDrawRecs.size(); k++) {
		if (k%4 == 0) setDrawColor(1.0, 0.0, 1.0, 0.5);
		else if (k%4 == 1) setDrawColor(0.0, 1.0, 1.0, 0.5);
		else if (k%4 == 2) setDrawColor(1.0, 1.0, 0.0, 0.5);
		else if (k%4 == 3) setDrawColor(0.0, 1.0, 0.0, 0.5);
		float *r = biomeChunksDrawRecs[k];
		HetuwMod::hDrawRect(r[0], r[1], r[2], r[3]);
	}
}

void Phex::sendPosition() {
	if (!HetuwMod::ourLiveObject) return;

	int posX = HetuwMod::ourLiveObject->xd;
	int posY = HetuwMod::ourLiveObject->yd;
	if (posX == lastPositionSentX && posY == lastPositionSentY) return;
	lastPositionSentX = posX; lastPositionSentY = posY;
	std::string str = "POSITION "+to_string(posX)+" "+to_string(posY);
	tcp.send(str);
}
