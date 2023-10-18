#include "hetuwmod.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "LivingLifePage.h"
#include "objectBank.h"
#include "emotion.h"
#include "minorGems/util/SimpleVector.h"
#include "minorGems/game/drawUtils.h"
#include "minorGems/graphics/openGL/ScreenGL.h"
#include "minorGems/io/file/File.h"
#include "minorGems/graphics/converters/TGAImageConverter.h"
#include "groundSprites.h"
#include "photos.h"
#include "phex.h"
#include "hetuwFont.h"

using namespace std;

constexpr int HetuwMod::OBJID_SharpStone;
constexpr int HetuwMod::OBJID_Fire;
constexpr int HetuwMod::OBJID_HotCoals;
constexpr int HetuwMod::OBJID_ClayBowl;
constexpr int HetuwMod::OBJID_ClayPlate;
constexpr int HetuwMod::OBJID_HotAdobeOven;

int HetuwMod::maxObjects;

int HetuwMod::viewWidth;
int HetuwMod::viewHeight;
double HetuwMod::viewWidthToHeightFactor;
double HetuwMod::viewHeightToWidthFactor;

extern doublePair lastScreenViewCenter;
doublePair HetuwMod::fromViewToMapCoordsVec;

float HetuwMod::zoomScale;
float HetuwMod::guiScaleRaw;
float HetuwMod::guiScale;
int HetuwMod::panelOffsetX;
int HetuwMod::panelOffsetY;
int HetuwMod::tutMessageOffsetX;
int HetuwMod::tutMessageOffsetX2;

HetuwMod::RainbowColor *HetuwMod::colorRainbow;

LivingLifePage *HetuwMod::livingLifePage;
LiveObject *HetuwMod::ourLiveObject;

bool HetuwMod::bDrawHelp;

float HetuwMod::lastPosX;
float HetuwMod::lastPosY;
int HetuwMod::magnetMoveDir = -1;
int HetuwMod::magnetWrongMoveDir = -1;
int HetuwMod::magnetMoveCount = 0;

int HetuwMod::cfgVersionNumber = 4;
int HetuwMod::cfgVersionRead = 4;

unsigned char HetuwMod::charKey_Up;
unsigned char HetuwMod::charKey_Down;
unsigned char HetuwMod::charKey_Left;
unsigned char HetuwMod::charKey_Right;
unsigned char HetuwMod::charKey_TileStandingOn;

unsigned char HetuwMod::charKey_Backpack;
unsigned char HetuwMod::charKey_TakeOffBackpack;
unsigned char HetuwMod::charKey_Pocket;
unsigned char HetuwMod::charKey_Eat;
unsigned char HetuwMod::charKey_Baby;
unsigned char HetuwMod::charKey_ShowHelp;
unsigned char HetuwMod::charKey_ShowNames;
unsigned char HetuwMod::charKey_ShowCords;
unsigned char HetuwMod::charKey_ShowPlayersInRange;
unsigned char HetuwMod::charKey_ShowDeathMessages;
unsigned char HetuwMod::charKey_ShowHomeCords;
unsigned char HetuwMod::charKey_ShowHostileTiles;
unsigned char HetuwMod::charKey_xRay;
unsigned char HetuwMod::charKey_Search;
unsigned char HetuwMod::charKey_TeachLanguage;
unsigned char HetuwMod::charKey_FindYum;
unsigned char HetuwMod::charKey_HidePlayers;
unsigned char HetuwMod::charKey_ShowGrid;
unsigned char HetuwMod::charKey_MakePhoto;
unsigned char HetuwMod::charKey_Phex;

unsigned char HetuwMod::charKey_CreateHome;
unsigned char HetuwMod::charKey_FixCamera;

unsigned char HetuwMod::charKey_ShowMap;
unsigned char HetuwMod::charKey_MapZoomIn;
unsigned char HetuwMod::charKey_MapZoomOut;

bool HetuwMod::upKeyDown;
bool HetuwMod::downKeyDown;
bool HetuwMod::leftKeyDown;
bool HetuwMod::rightKeyDown;

doublePair HetuwMod::debugRecPos;
doublePair HetuwMod::debugRecPos2;

int HetuwMod::currentEmote = -1;
time_t HetuwMod::lastEmoteTime;
time_t HetuwMod::lastSpecialEmote = 0;

bool *HetuwMod::isDangerousAnimal = NULL;

int* HetuwMod::closedDoorIDs;
int HetuwMod::closedDoorIDsLength;

bool HetuwMod::waitForDoorToOpen;
int HetuwMod::lastDoorToOpenX;
int HetuwMod::lastDoorToOpenY;

bool HetuwMod::stopAutoRoadRun;
time_t HetuwMod::stopAutoRoadRunTime;
bool HetuwMod::activateAutoRoadRun;

int HetuwMod::iDrawNames;
bool HetuwMod::bDrawSelectedPlayerInfo = false;
float HetuwMod::playerNameColor[3];
doublePair HetuwMod::playerNamePos;

bool HetuwMod::bDrawCords;
bool HetuwMod::bDrawHostileTiles = true;

bool HetuwMod::bWriteLogs = true;
int HetuwMod::lastLoggedId = -1;

double HetuwMod::curStepTime;
time_t HetuwMod::curStepSecondsSince1970;
int HetuwMod::stepCount;
double HetuwMod::ourAge;

SimpleVector<LiveObject> *HetuwMod::gameObjects;
SimpleVector<int> *HetuwMod::mMapContainedStacks;
SimpleVector<SimpleVector<int>> *HetuwMod::mMapSubContainedStacks;
int *HetuwMod::mMapD;

bool HetuwMod::invalidVersionDetected = false;
string HetuwMod::strInvalidVersion = "";

int *HetuwMod::mCurMouseOverID;
int HetuwMod::selectedPlayerID;
double HetuwMod::timeLastPlayerHover;

std::vector<HetuwMod::PlayerInMap*> HetuwMod::playersInMap;
bool HetuwMod::bDrawMap;
float HetuwMod::mapScale;
float HetuwMod::mapOffsetX;
float HetuwMod::mapOffsetY;

bool HetuwMod::mapZoomInKeyDown;
bool HetuwMod::mapZoomOutKeyDown;

int HetuwMod::playersInRangeNum = 0;
int HetuwMod::iDrawPlayersInRangePanel;
std::vector<HetuwMod::FamilyInRange*> HetuwMod::familiesInRange;

bool HetuwMod::bDrawDeathMessages;
std::vector<HetuwMod::DeathMsg*> HetuwMod::deathMessages;

bool HetuwMod::bDrawHomeCords;
float HetuwMod::longestCordsTextWidth = 0;
std::vector<HetuwMod::HomePos*> HetuwMod::homePosStack;
bool HetuwMod::bNextCharForHome;

GridPos HetuwMod::cordOffset;

bool HetuwMod::bDrawInputString;
string HetuwMod::tempInputString;

int HetuwMod::getCustomCords;
char HetuwMod::tempCordChar;
int HetuwMod::tempCordX;
int HetuwMod::tempCordY;

bool HetuwMod::searchIncludeHashText = false;
bool *HetuwMod::objIsBeingSearched;
int HetuwMod::getSearchInput;
std::vector<char*> HetuwMod::searchWordList;
bool HetuwMod::bDrawSearchList;
int HetuwMod::drawSearchListTopY;
std::vector<doublePair*> HetuwMod::searchWordStartPos;
std::vector<doublePair*> HetuwMod::searchWordEndPos;
std::vector<bool> HetuwMod::searchWordListDelete;

bool HetuwMod::takingPhoto;
bool HetuwMod::takingSpecialPhoto = false;
int HetuwMod::recTakePhoto[4];
bool HetuwMod::bDrawPhotoRec = false;

float HetuwMod::drawColorAlpha = 1.0f;
float HetuwMod::xRayOpacity = 0.3f;
bool HetuwMod::bxRay;
bool HetuwMod::bHidePlayers = false;
char HetuwMod::ourGender;

bool HetuwMod::bFoundFamilyName;
string HetuwMod::ourFamilyName;

bool HetuwMod::cameraIsFixed;

bool HetuwMod::bMoveClick = false;
bool HetuwMod::bMoveClickAlpha;
int HetuwMod::bMoveClickX;
int HetuwMod::bMoveClickY;

//constexpr char HetuwMod::languageArray[HetuwMod::languageArraySize1][HetuwMod::languageArraySize2];
bool HetuwMod::bTeachLanguage = false;
int HetuwMod::teachLanguageCount = 0;
double HetuwMod::timeLastLanguage = 0;

vector<char*> HetuwMod::sayBuffer;
double HetuwMod::timeLastSay = 0;
bool HetuwMod::clearSayBuffer;
float HetuwMod::sayDelay = 2.1;

int *HetuwMod::becomesFoodID;
SimpleVector<int> HetuwMod::yummyFoodChain;
bool HetuwMod::bDrawYum = false;

double *HetuwMod::objectDrawScale = NULL;
float HetuwMod::colorRainbowFast[3];

float *HetuwMod::objectDefaultColors;

bool HetuwMod::bHoldDownTo_FixCamera = true;
bool HetuwMod::bHoldDownTo_XRay = true;
bool HetuwMod::bHoldDownTo_FindYum = true;
bool HetuwMod::bHoldDownTo_ShowGrid = true;

bool HetuwMod::b_drawYumColor = false;
bool HetuwMod::b_drawYumPulsate = true;
bool HetuwMod::b_drawSearchText = true;
bool HetuwMod::b_drawSearchTileRec = false;
bool HetuwMod::b_drawSearchPulsate = true;

bool HetuwMod::bAutoDataUpdate = true;
bool HetuwMod::bDrawGrid = false;

char HetuwMod::usingCustomServer = false;
char *HetuwMod::serverIP = NULL;
int HetuwMod::serverPort = 0;

bool HetuwMod::addBabyCoordsToList = false;

bool HetuwMod::bRemapStart = true;
bool HetuwMod::bDrawHungerWarning = false;

std::vector<HetuwMod::HttpRequest*> HetuwMod::httpRequests;

bool HetuwMod::connectedToMainServer = false;
time_t HetuwMod::arcRunningSince = -1;

doublePair HetuwMod::mouseRelativeToView = {0, 0};

bool HetuwMod::isMovingInVog = false;
HetuwMod::IntervalTimed HetuwMod::intervalVogMove(0.1);

bool HetuwMod::phexIsEnabled = true;
std::string HetuwMod::phexIp = "phexonelife.duckdns.org";
int HetuwMod::phexPort = 6567;
bool HetuwMod::debugPhex = false;

bool HetuwMod::sendKeyEvents = false;

bool HetuwMod::bDrawBiomeInfo = false;

HetuwFont *HetuwMod::customFont = NULL;

std::string HetuwMod::helpTextSearch[6];
std::string HetuwMod::helpTextCustomCoord[5];

std::string HetuwMod::hexRaceColor_brown = "a85e3d";
std::string HetuwMod::hexRaceColor_ginger = "f2cac1";
std::string HetuwMod::hexRaceColor_white = "ddaf93";
std::string HetuwMod::hexRaceColor_black = "3f2a2a";
float HetuwMod::raceColor_brown[] = {0.658824, 0.372549, 0.243137};
float HetuwMod::raceColor_ginger[] = {0.952941, 0.796078, 0.756863};
float HetuwMod::raceColor_white[] = {0.866667, 0.690196, 0.576471};
float HetuwMod::raceColor_black[] = {0.250980, 0.168627, 0.164706};

extern doublePair lastScreenViewCenter;

void HetuwMod::init() {
	mouseRelativeToView = {0, 0};

	viewWidthToHeightFactor = defaultViewWidth/(double)defaultViewHeight;
	viewHeightToWidthFactor = defaultViewHeight/(double)defaultViewWidth;

	zoomScale = 1.5f;
	guiScaleRaw = 0.8f;
	guiScale = guiScaleRaw * zoomScale;
	zoomCalc();
	
	colorRainbow = new RainbowColor();

	bDrawHelp = false;
	iDrawPlayersInRangePanel = 0;
	bDrawDeathMessages = true;
	bDrawHomeCords = false;
	iDrawNames = 1;
	bDrawCords = true;

	charKey_Up = 'w';
	charKey_Down = 's';
	charKey_Left = 'a';
	charKey_Right = 'd';
	charKey_TileStandingOn = ' ';

	charKey_Backpack = 'q';
	charKey_TakeOffBackpack = 'b';
	charKey_Pocket = 't';
	charKey_Eat = 'e';
	charKey_Baby = 'c';
	charKey_ShowHelp = 'h';
	charKey_ShowNames = 'n';
	charKey_ShowCords = 'z';
	charKey_ShowPlayersInRange = 'p';
	charKey_ShowDeathMessages = 254;
	charKey_ShowHomeCords = 'g';
	charKey_ShowHostileTiles = 'u';

	charKey_xRay = 'x';
	charKey_Search = 'j';
	charKey_TeachLanguage = 'l';
	charKey_FindYum = 'y';
	charKey_HidePlayers = 254;
	charKey_ShowGrid = 'k';
	charKey_MakePhoto = 254;
	charKey_Phex = '#';

	charKey_ShowMap = 'm';
	charKey_MapZoomIn = 'u';
	charKey_MapZoomOut = 'o';

	charKey_CreateHome = 'r';
	charKey_FixCamera = 'f';

	debugRecPos = { 0.0, 0.0 };
	debugRecPos2 = { 0.0, 0.0 };

	bDrawMap = false;
	mapScale = 85;
	mapOffsetX = 0;
	mapOffsetY = 0;

	takingPhoto = false;
	bxRay = false;
	bHidePlayers = false;
	objIsBeingSearched = NULL;
	clearSayBuffer = false;
	selectedPlayerID = 0;
	timeLastPlayerHover = 0;

	familiesInRange.push_back(new FamilyInRange());
	strcpy(familiesInRange[0]->name, hetuwDefaultOurFamilyName);

	cordOffset = { 0, 0 };
	addHomeLocation( 0, 0, hpt_birth ); // add birth location

	initClosedDoorIDs();

	initSettings();

	lastLoggedId = getLastIdFromLogs();

	Phex::init();

	initHelpText();
}

void HetuwMod::initHelpText() {
	helpTextSearch[0] = "You pressed ";
	helpTextSearch[0] += toupper(charKey_Search);
	helpTextSearch[0] += " and activated SEARCH";

	helpTextSearch[1] = "Abort with ESC";
	helpTextSearch[2] = "Type in the name of the object you want to find";
	helpTextSearch[3] = "Put a . at the end for an exact search";

	helpTextSearch[4] = "Remove the last search term with SHIFT + ";
	helpTextSearch[4] += toupper(charKey_Search);

	helpTextSearch[5] = "Or click on it in the list";


	helpTextCustomCoord[0] = "You pressed ";
	helpTextCustomCoord[0] += toupper(charKey_CreateHome);
	helpTextCustomCoord[0] += " to create a new coord";

	helpTextCustomCoord[1] += "Abort with ESC";
	helpTextCustomCoord[2] += "Press any letter key to create a custom coord";
	helpTextCustomCoord[3] += "Left click a coord in the list to make it your 0,0 position";
	helpTextCustomCoord[4] += "Remove a coord by left clicking it while holding CTRL";
}

void HetuwMod::splitLogLine(string* lineElements, string line) { // lineElements should be a string array with size 16
	int k = 0;
	for (unsigned i=0; i<line.length(); i++) {
		if (line[i] == hetuwLogSeperator[0] && i+1 < line.length()) {
			if (line[i+1] == hetuwLogSeperator[1]) {
				k++;
				if (k >= 16) return;
				i += 2;
				continue;
			}
		}
		lineElements[k] += line[i];
	}
}

int HetuwMod::getLastIdFromLogs() {
	if (!bWriteLogs) return -1;

	ifstream ifs( hetuwLogFileName );
	if (!ifs.good()) return -1; // file does not exist

	string line;
	while (getline(ifs, line)) {
		string lineElements[16];
		splitLogLine(lineElements, line);
		if (lineElements[1].compare("my_id") == 0) {
			try {
				int r = stoi(lineElements[2]);
				return r;
			} catch (...) {
				return -1;
			}
		}
	}
	return -1;
}

int HetuwMod::getRecWidth(int rec[]) {
	return rec[2] - rec[0];
}
double HetuwMod::getRecWidth(double rec[]) {
	return rec[2] - rec[0];
}

int HetuwMod::getRecHeight(int rec[]) {
	return rec[3] - rec[1];
}
double HetuwMod::getRecHeight(double rec[]) {
	return rec[3] - rec[1];
}

void HetuwMod::setRecPosition(int rec[], int startX, int startY) {
	float width = rec[2] - rec[0];
	float height = rec[3] - rec[1];
	rec[0] = startX; rec[1] = startY;
	rec[2] = startX+width; rec[3] = startY+height;
}

void HetuwMod::setRecFromCenterWidthHeight(int rec[], int centerX, int centerY, int width, int height) {
	rec[0] = centerX - (width/2);
	rec[1] = centerY - (height/2);
	rec[2] = rec[0] + width;
	rec[3] = rec[1] + height;
}

void HetuwMod::addToRec(int rec[], int x, int y) {
	rec[0] += x; rec[1] += y; rec[2] += x; rec[3] += y;
}
void HetuwMod::addToRec(double rec[], double x, double y) {
	rec[0] += x; rec[1] += y; rec[2] += x; rec[3] += y;
}

bool HetuwMod::pointIsInsideRec(double rec[], double x, double y) {
	if (rec[0] <= x && rec[2] >= x) {
		if (rec[1] <= y && rec[3] >= y) return true;
	}
	return false;
}

void HetuwMod::set4BorderRecs(double rec[4], double outRecs[4][4], double borderWidth, double borderHeight) {
	double *leftBorder = outRecs[0];
	leftBorder[0] = rec[0];
	leftBorder[1] = rec[1];
	leftBorder[2] = rec[0] + borderWidth;
	leftBorder[3] = rec[3];
	double *topBorder = outRecs[1];
	topBorder[0] = rec[0];
	topBorder[1] = rec[3] - borderHeight;
	topBorder[2] = rec[2];
	topBorder[3] = rec[3];
	double *rightBorder = outRecs[2];
	rightBorder[0] = rec[2] - borderWidth;
	rightBorder[1] = rec[1];
	rightBorder[2] = rec[2];
	rightBorder[3] = rec[3];
	double *bottomBorder = outRecs[3];
	bottomBorder[0] = rec[0];
	bottomBorder[1] = rec[1];
	bottomBorder[2] = rec[2];
	bottomBorder[3] = rec[1] + borderHeight;
}

void HetuwMod::logRec(string desc, double rec[]) {
	printf("%s l: %f b: %f r: %f t: %f\n", desc.c_str(), rec[0], rec[1], rec[2], rec[3]);
}

doublePair HetuwMod::getFromMapToViewCoordsVec() {
	doublePair screenCenter = lastScreenViewCenter;
	screenCenter.x -= viewWidth/2;
	screenCenter.y -= viewHeight/2;
	screenCenter.x = -screenCenter.x;
	screenCenter.y = -screenCenter.y;
	return screenCenter;
}

doublePair HetuwMod::getFromViewToMapCoordsVec() {
	doublePair screenCenter = lastScreenViewCenter;
	screenCenter.x -= viewWidth/2;
	screenCenter.y -= viewHeight/2;
	return screenCenter;
}

void HetuwMod::pointFromPercentToMapCoords(float &x, float &y) {
	x *= viewWidth; y *= viewHeight;
	x += fromViewToMapCoordsVec.x; y += fromViewToMapCoordsVec.y;
}
void HetuwMod::pointFromPercentToMapCoords(double &x, double &y) {
	x *= viewWidth; y *= viewHeight;
	x += fromViewToMapCoordsVec.x; y += fromViewToMapCoordsVec.y;
}
void HetuwMod::pointFromMapToPercentCoords(float &x, float &y) {
	doublePair mapToView = getFromMapToViewCoordsVec();
	x += mapToView.x; y += mapToView.y;
	x /= viewWidth; y /= viewHeight;
}
void HetuwMod::pointFromMapToPercentCoords(double &x, double &y) {
	doublePair mapToView = getFromMapToViewCoordsVec();
	x += mapToView.x; y += mapToView.y;
	x /= viewWidth; y /= viewHeight;
}
void HetuwMod::xFromPercentToMapCoords(double &x) {
	x = (x*viewWidth)+fromViewToMapCoordsVec.x;
}
void HetuwMod::yFromPercentToMapCoords(double &y) {
	y = (y*viewHeight)+fromViewToMapCoordsVec.y;
}

void HetuwMod::recToPixelCoords(int *rec) {
	doublePair screenCoordsVec = getFromMapToViewCoordsVec();
	rec[0] += screenCoordsVec.x; rec[2] += screenCoordsVec.x;
	rec[1] += screenCoordsVec.y; rec[3] += screenCoordsVec.y;
	int screenWidth, screenHeight;
	getScreenDimensions( &screenWidth, &screenHeight );
	double scaleX = ((double)screenWidth/viewWidth);
	double scaleY = ((double)screenHeight/viewHeight);
	rec[0] *= scaleX; rec[2] *= scaleX;
	rec[1] *= scaleY; rec[3] *= scaleY;
}

void HetuwMod::recFromPercentToMapCoords(double rec[]) {
	rec[0] *= viewWidth; rec[1] *= viewHeight;
	rec[2] *= viewWidth; rec[3] *= viewHeight;
	//doublePair viewToMap = getFromViewToMapCoordsVec();
	doublePair viewToMap = fromViewToMapCoordsVec;
	rec[0] += viewToMap.x; rec[1] += viewToMap.y;
	rec[2] += viewToMap.x; rec[3] += viewToMap.y;
}
void HetuwMod::recFromMapToPercentCoords(double rec[]) {
	doublePair mapToView = getFromMapToViewCoordsVec();
	rec[0] += mapToView.x; rec[1] += mapToView.y;
	rec[2] += mapToView.x; rec[3] += mapToView.y;
	rec[0] /= viewWidth; rec[1] /= viewHeight;
	rec[2] /= viewWidth; rec[3] /= viewHeight;
}

void HetuwMod::setTakingPhoto(bool inTakingPhoto) {
	takingPhoto = inTakingPhoto;
}

void HetuwMod::updatePhotoRecPosition(int rec[]) {
	int mouseX, mouseY;
	livingLifePage->hetuwGetMouseXY( mouseX, mouseY );
	int size = hetuwPhotoSize;
	int screenWidth, screenHeight;
	getScreenDimensions( &screenWidth, &screenHeight );
	int width = size*(viewWidth/(double)screenWidth);
	int height = size*(viewHeight/(double)screenHeight);
	setRecFromCenterWidthHeight(rec, mouseX, mouseY, width, height);
}

void HetuwMod::drawPhotoRec(int rec[]) {
	updatePhotoRecPosition(rec);
	setDrawColor(1.0f, 1.0f, 1.0f, 1.0f);
	int width = getRecWidth(rec);
	int height = getRecHeight(rec);
	int centerX = rec[0] + (width/2);
	int centerY = rec[1] + (height/2);
	doublePair center = { (double)centerX, (double)centerY };
	int thickness = 4*zoomScale;
	int centerRecWidth = 28*zoomScale;
	drawRect( center, centerRecWidth, thickness );
	drawRect( center, thickness, centerRecWidth );
	int recWidth = getRecWidth(rec);
	int sideRecWidth = recWidth*0.3;
	thickness *= 2;

	drawRect(rec[0], rec[1], rec[0]+sideRecWidth, rec[1]+thickness);
	drawRect(rec[0], rec[1], rec[0]+thickness, rec[1]+sideRecWidth);

	drawRect(rec[0], rec[3]-sideRecWidth, rec[0]+thickness, rec[3]);
	drawRect(rec[0], rec[3], rec[0]+sideRecWidth, rec[3]-thickness);

	drawRect(rec[2]-sideRecWidth, rec[3], rec[2], rec[3]-thickness);
	drawRect(rec[2]-thickness, rec[3], rec[2], rec[3]-sideRecWidth);

	drawRect(rec[2]-thickness, rec[1], rec[2], rec[1]+sideRecWidth);
	drawRect(rec[2]-sideRecWidth, rec[1], rec[2], rec[1]+thickness);
}

int* HetuwMod::getPhotoRecForImage() {
	int *rec = new int[4];
	for (int i=0; i<4; i++) rec[i] = recTakePhoto[i];
	recToPixelCoords(rec);
	return rec;
}

void HetuwMod::saveImage(Image *image) {
	saveImage(image, to_string(time(NULL)));
}

void HetuwMod::saveImage(Image *image, string name) {
	File shotDir( NULL, "screenShots" );
	if( !shotDir.exists() ) {
		shotDir.makeDirectory();
	}
	File *file = shotDir.getChildFile( (name+".tga").c_str() );
	FileOutputStream fos( file );
	TGAImageConverter imageConverter;
	imageConverter.formatImage( image, &fos );
}

// splits a string count times whenever it finds a splitChar - removes splitChar chars
std::vector<std::string> HetuwMod::splitStrXTimes(const std::string &str, char splitChar, int count) {
	std::vector<std::string> result;
	if (str.length() <= 0) return result;
	size_t strPos = 0;
	for (int i=0; i<count; i++) {
		size_t firstCharPos = str.find(splitChar, strPos);
		if (firstCharPos == std::string::npos) break;
		string sub = str.substr(strPos, firstCharPos-strPos);
		if (sub.length() > 0) result.push_back(sub); // sub.length() might be 0 if str contains several splitChars in a row
		strPos = firstCharPos+1;
	}
	//string sub = str.substr(strPos, str.length());
	//printf("Phex len: %d sub: %s\n", sub.length(), sub.c_str());
	//if (sub.length() > 0) result.push_back(sub);
	if (strPos < str.length()) result.push_back(str.substr(strPos, str.length()));
	return result;
}

// does not check for all dangerous animals, use isDangerousAnimal(int objId) instead
bool HetuwMod::strContainsDangerousAnimal(const char* str) {
	if (strstr( str, "Dying Semi-tame Wolf") != NULL) return false;
	if (strstr( str, "Dead Semi-tame Wolf") != NULL) return false;
	if (strstr( str, "Old Semi-tame Wolf") != NULL) return false;
	if (strstr( str, "Wolf Puppy") != NULL) return false;
	if (strstr( str, "Semi-tame Wolf with Pup") != NULL) return false;
	if (strstr( str, "Wolf Skin") != NULL) return false;
	if (strstr( str, "Wolf Crown") != NULL) return false;
	if (strstr( str, "Wolf Hat") != NULL) return false;
	if (strstr( str, "Skinned Wolf") != NULL) return false;
	if (strstr( str, "Skinless Wolf") != NULL) return false;
	if (strstr( str, "Dead Wolf") != NULL) return false;
	if (strstr( str, "Buried Wolf") != NULL) return false;
	if (strstr( str, "Shot Domestic Boar with Piglet") != NULL) return false;
	if (strstr( str, "Shot Wild Boar with Piglet") != NULL) return false;
	if (strstr( str, "Dead Grizzly Bear") != NULL) return false;

	if (strstr( str, "Grizzly Bear") != NULL) return true;
	if (strstr( str, "Wild Boar") != NULL) return true;
	if (strstr( str, "Domestic Boar") != NULL) return true;
	if (strstr( str, "Wolf") != NULL) return true;

	return false;
}

void HetuwMod::initDangerousAnimals() {
	if (isDangerousAnimal) delete[] isDangerousAnimal;
	isDangerousAnimal = new bool[maxObjects];

	for (int i=0; i<maxObjects; i++) {
		ObjectRecord* obj = getObject(i);
		if (obj && obj->description && strContainsDangerousAnimal(obj->description)) {
			isDangerousAnimal[i] = true;
		} else if (   i == 2156 // Mosquito swarm
		           || i == 2157 // Mosquito swarm - just bit
		           || i == 764  // Rattle Snake
		           || i == 1385 // Attacking Rattle Snake
		           || i == 1789 // Abused Pit Bull
		           || i == 1747 // Mean Pit Bull
		           || i == 1712 // Attacking Pit Bull
		          ) {
			isDangerousAnimal[i] = true;
		} else {
			isDangerousAnimal[i] = false;
		}
	}
}

void HetuwMod::initClosedDoorIDs() {
	if (closedDoorIDs != NULL) {
		delete[] closedDoorIDs;
		closedDoorIDs = NULL;
	}
	closedDoorIDsLength = 10;
	closedDoorIDs = new int[closedDoorIDsLength];

	closedDoorIDs[0] = 116; // 116.txt:Pine Door# installed vert
	closedDoorIDs[1] = 2759; // 2759.txt:Springy Wooden Door# installed vert
	closedDoorIDs[2] = 876; // 876.txt:Wooden Door# Installed
	closedDoorIDs[3] = 1930; // 1930.txt:Twenty Minute Wooden Door# Installed
	closedDoorIDs[4] = 2757; // 2757.txt:Springy Wooden Door# Installed
	closedDoorIDs[5] = 877; // 877.txt:Wooden Door# installed vert
	closedDoorIDs[6] = 115; // 115.txt:Pine Door# Installed
	closedDoorIDs[7] = 1851; // 1851.txt:Fence Gate
	closedDoorIDs[8] = 2984; // 2984.txt:Shaky Property Gate# +owned
	closedDoorIDs[9] = 2962; // 2962.txt:Property Gate# +owned
}

// outputs name[] and value[] - input line
void HetuwMod::getSettingsFileLine( char* name, size_t nameSize, char* value, size_t valueSize, string line ) {
	size_t n = 0;
	size_t v = 0;
	bool readName = true;
	for (unsigned i=0; i<line.length(); i++) {
		if (line[i] == ' ') continue;

		if (readName) {
			if (line[i] == '=') {
				readName = false;
				continue;
			}
			if (n+1 >= nameSize) break;
			name[n] = line[i];
			n++;
		} else {
			if (v+1 >= valueSize) break;
			if (line[i] != '\r') {
				value[v] = line[i];
				v++;
			}
		}
	}
	name[n] = 0;
	value[v] = 0;
}

bool HetuwMod::setCharKey( char unsigned &key, const char *value ) {
	if (value[0] != '<') {
		key = value[0];
		return true;
	}
	if (strstr(value, "<space>")) {
		key = ' ';
		return true;
	}
	return false;
}

bool HetuwMod::setSetting( const char* name, const char* value ) {
	if (strlen(value) < 1) return false;

	if (strstr(name, "cfg_version")) {
		cfgVersionRead = stoi(value);
		return true;
	}

	if (strstr(name, "key_up")) return setCharKey( charKey_Up, value );
	if (strstr(name, "key_down")) return setCharKey( charKey_Down, value );
	if (strstr(name, "key_left")) return setCharKey( charKey_Left, value );
	if (strstr(name, "key_right")) return setCharKey( charKey_Right, value );
	if (strstr(name, "key_center")) return setCharKey( charKey_TileStandingOn, value );
	if (strstr(name, "key_backpack")) return setCharKey( charKey_Backpack, value );
	if (strstr(name, "key_takeOffBackpack")) return setCharKey( charKey_TakeOffBackpack, value );
	if (strstr(name, "key_pocket")) return setCharKey( charKey_Pocket, value );
	if (strstr(name, "key_eat")) return setCharKey( charKey_Eat, value );
	if (strstr(name, "key_baby")) return setCharKey( charKey_Baby, value );
	if (strstr(name, "key_show_help")) return setCharKey( charKey_ShowHelp, value );
	if (strstr(name, "key_show_names")) return setCharKey( charKey_ShowNames, value );
	if (strstr(name, "key_show_cords")) return setCharKey( charKey_ShowCords, value );
	if (strstr(name, "key_show_playersinrange")) return setCharKey( charKey_ShowPlayersInRange, value );
	if (strstr(name, "key_show_deathmessages")) return setCharKey( charKey_ShowDeathMessages, value );
	if (strstr(name, "key_show_homecords")) return setCharKey( charKey_ShowHomeCords, value );
	if (strstr(name, "key_show_hostiletiles")) return setCharKey( charKey_ShowHostileTiles, value );
	if (strstr(name, "key_xray")) return setCharKey( charKey_xRay, value );
	if (strstr(name, "key_remembercords")) return setCharKey( charKey_CreateHome, value );
	if (strstr(name, "key_fixcamera")) return setCharKey( charKey_FixCamera, value );
	if (strstr(name, "key_search")) return setCharKey( charKey_Search, value );
	if (strstr(name, "key_teachlanguage")) return setCharKey( charKey_TeachLanguage, value );
	if (strstr(name, "key_findyum")) return setCharKey( charKey_FindYum, value );
	if (strstr(name, "key_hideplayers")) return setCharKey( charKey_HidePlayers, value );
	if (strstr(name, "key_showgrid")) return setCharKey( charKey_ShowGrid, value );
	if (strstr(name, "key_takephoto")) return setCharKey( charKey_MakePhoto, value );
	if (strstr(name, "key_phex")) return setCharKey( charKey_Phex, value );

	if (strstr(name, "init_show_names")) {
		iDrawNames = (int)(value[0]-'0');
		return true;
	}
	if (strstr(name, "init_show_selectedplayerinfo")) {
		bDrawSelectedPlayerInfo = (int)(value[0]-'0');
		return true;
	}
	if (strstr(name, "init_show_cords")) {
		bDrawCords = bool(value[0]-'0');
		return true;
	}
	if (strstr(name, "init_show_playersinrange")) {
		iDrawPlayersInRangePanel = (int)(value[0]-'0');
		return true;
	}
	if (strstr(name, "init_show_deathmessages")) {
		bDrawDeathMessages = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "init_show_homecords")) {
		bDrawHomeCords = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "init_show_hostiletiles")) {
		bDrawHostileTiles = bool(value[0]-48);
		return true;
	}

	if (strstr(name, "phex_enabled")) {
		phexIsEnabled = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "phex_ip")) {
		phexIp = string(value);
		return true;
	}
	if (strstr(name, "phex_port")) {
		phexPort = atoi(value);
		return true;
	}
	if (strstr(name, "phex_coords")) {
		Phex::allowServerCoords = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "phex_debug")) {
		debugPhex = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "phex_channel")) {
		Phex::forceChannel = string(value);
		return true;
	}
	if (strstr(name, "phex_send_fake_life")) {
		Phex::bSendFakeLife = bool(value[0]-'0');
		return true;
	}
	if (strstr(name, "send_keyevents")) {
		sendKeyEvents = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "drawbiomeinfo")) {
		bDrawBiomeInfo = bool(value[0]-48);
		return true;
	}

	if (strstr(name, "keep_button_pressed_to_fixcamera")) {
		bHoldDownTo_FixCamera = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "keep_button_pressed_to_findyum")) {
		bHoldDownTo_FindYum = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "keep_button_pressed_to_showgrid")) {
		bHoldDownTo_ShowGrid = bool(value[0]-48);
		return true;
	}

	if (strstr(name, "keep_button_pressed_to_xray")) {
		bHoldDownTo_XRay = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "xray_opacity")) {
		xRayOpacity = ((int)(value[0]-48)/10.0f);
		return true;
	}

	if (strstr(name, "draw_yumcolor")) {
		b_drawYumColor = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "draw_yumpulsate")) {
		b_drawYumPulsate = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "search_include_hash_text")) {
		searchIncludeHashText = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "draw_searchtext")) {
		b_drawSearchText = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "draw_searchrec")) {
		b_drawSearchTileRec = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "draw_searchpulsate")) {
		b_drawSearchPulsate = bool(value[0]-48);
		return true;
	}

	if (strstr(name, "add_baby_coords_to_list")) {
		addBabyCoordsToList = bool(value[0]-48);
		return true;
	}

	if (strstr(name, "automatic_data_update")) {
		bAutoDataUpdate = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "hetuw_log")) {
		bWriteLogs = bool(value[0]-48);
		return true;
	}
	if (strstr(name, "chat_delay")) {
		sayDelay = stoi(value)/10.0f;
		return true;
	}
	if (strstr(name, "remap_start_enabled")) {
		bRemapStart = bool(value[0]-'0');
		return true;
	}
	if (strstr(name, "draw_hunger_warning")) {
		bDrawHungerWarning = bool(value[0]-'0');
		return true;
	}

	return false;
}

void HetuwMod::writeCharKeyToStream( ofstream &ofs, const char* keyName, char key ) {
	ofs << keyName << " = ";
	if (key == ' ')
		ofs << "<space>";
	else
		ofs << key;
	ofs << endl;
}

void HetuwMod::writeSettings(ofstream &ofs) {
	ofs << "// this file will be created whenever you start the mod" << endl;
	ofs << "// if you want to reset this file, just delete it" << endl;
	ofs << endl;

	ofs << "cfg_version = " << cfgVersionNumber << endl;
	ofs << endl;

	writeCharKeyToStream( ofs, "key_up", charKey_Up );
	writeCharKeyToStream( ofs, "key_down", charKey_Down );
	writeCharKeyToStream( ofs, "key_left", charKey_Left );
	writeCharKeyToStream( ofs, "key_right", charKey_Right );
	writeCharKeyToStream( ofs, "key_center", charKey_TileStandingOn );
	ofs << endl;
	writeCharKeyToStream( ofs, "key_backpack", charKey_Backpack );
	writeCharKeyToStream( ofs, "key_eat", charKey_Eat );
	writeCharKeyToStream( ofs, "key_baby", charKey_Baby );
	ofs << endl;
	writeCharKeyToStream( ofs, "key_show_help", charKey_ShowHelp );
	writeCharKeyToStream( ofs, "key_show_names", charKey_ShowNames );
	writeCharKeyToStream( ofs, "key_show_cords", charKey_ShowCords );
	writeCharKeyToStream( ofs, "key_show_playersinrange", charKey_ShowPlayersInRange );
	writeCharKeyToStream( ofs, "key_show_deathmessages", charKey_ShowDeathMessages );
	writeCharKeyToStream( ofs, "key_show_homecords", charKey_ShowHomeCords );
	writeCharKeyToStream( ofs, "key_show_hostiletiles", charKey_ShowHostileTiles );
	ofs << endl;
	writeCharKeyToStream( ofs, "key_remembercords", charKey_CreateHome );
	writeCharKeyToStream( ofs, "key_fixcamera", charKey_FixCamera );
	writeCharKeyToStream( ofs, "key_xray", charKey_xRay );
	writeCharKeyToStream( ofs, "key_search", charKey_Search );
	writeCharKeyToStream( ofs, "key_teachlanguage", charKey_TeachLanguage );
	writeCharKeyToStream( ofs, "key_findyum", charKey_FindYum );
	writeCharKeyToStream( ofs, "key_hideplayers", charKey_HidePlayers );
	writeCharKeyToStream( ofs, "key_takeOffBackpack", charKey_TakeOffBackpack );
	writeCharKeyToStream( ofs, "key_pocket", charKey_Pocket );
	writeCharKeyToStream( ofs, "key_showgrid", charKey_ShowGrid );
	writeCharKeyToStream( ofs, "key_phex", charKey_Phex );
	ofs << endl;
	ofs << "// WARNING: Jason doesnt want us to upload bogus photos and you might get banned if you do, read: OneLife/photoServer/protocol.txt" << endl;
	ofs << "// How to use:" << endl;
	ofs << "// 1. Set key_takephoto to a key you want" << endl;
	ofs << "// 2. Press the key in game, a photo frame will appear, move the mouse around and zoom in and out to choose your photo" << endl;
	ofs << "// 3. Press the key again to make the photo, it will be saved in the screenshots folder" << endl;
	ofs << "// 4. If you want to upload it make an image with the camera in game, the last photo you took will be uploaded" << endl;
	ofs << "//    Hold a camera and rightclick on a 'Protected Stack of Photo Paper'" << endl;
	ofs << "//    Place the camera on the ground and rightlick it while it is rewinding" << endl;
	writeCharKeyToStream( ofs, "key_takephoto", charKey_MakePhoto );
	ofs << endl;
	ofs << "init_show_names = " << (char)(iDrawNames+48) << " // 0 = dont show names, 1 = show first name, 2 = show first and last name" << endl;
	ofs << "init_show_selectedplayerinfo = " << (char)(bDrawSelectedPlayerInfo+48) << " // 1 = draw names bigger and show age when hovering over a player" << endl;
	ofs << "init_show_cords = " << (char)(bDrawCords+48) << endl;
	ofs << "init_show_playersinrange = " << (char)(iDrawPlayersInRangePanel+48) << endl;
	ofs << "init_show_deathmessages = " << (char)(bDrawDeathMessages+48) << endl;
	ofs << "init_show_homecords = " << (char)(bDrawHomeCords+48) << endl;
	ofs << "init_show_hostiletiles = " << (char)(bDrawHostileTiles+48) << endl;
	ofs << endl;
	ofs << "phex_enabled = " << (char)(phexIsEnabled+48) << endl;
	ofs << "phex_ip = " << phexIp << endl;
	ofs << "phex_port = " << phexPort << endl;
	ofs << "phex_coords = " << (char)(Phex::allowServerCoords+48) << endl;
	if (Phex::forceChannel.length() > 1) ofs << "phex_channel = " << Phex::forceChannel << endl;
	if (Phex::bSendFakeLife) ofs << "phex_send_fake_life = " << (char)(Phex::bSendFakeLife+48) << endl;
	if (debugPhex) ofs << "phex_debug = " << (char)(debugPhex+48) << endl;
	if (sendKeyEvents) {
		ofs << endl;
		ofs << "send_keyevents = " << (char)(sendKeyEvents+48) << endl;
	}
	ofs << endl;
	ofs << "keep_button_pressed_to_fixcamera = " << (char)(bHoldDownTo_FixCamera+48) << endl;
	ofs << "keep_button_pressed_to_findyum = " << (char)(bHoldDownTo_FindYum+48) << endl;
	ofs << "keep_button_pressed_to_showgrid = " << (char)(bHoldDownTo_ShowGrid+48) << endl;
	ofs << endl;
	ofs << "keep_button_pressed_to_xray = " << (char)(bHoldDownTo_XRay+48) << endl;
	ofs << "xray_opacity = " << (char)(xRayOpacity*10+48) << " // how visible objects should be, can be 0 - 10" << endl;
	ofs << endl;
	ofs << "draw_yumcolor = " << (char)(b_drawYumColor+48) << endl;
	ofs << "draw_yumpulsate = " << (char)(b_drawYumPulsate+48) << endl;
	ofs << endl;
	ofs << "search_include_hash_text = " << (char)(searchIncludeHashText+48) << endl;
	ofs << "draw_searchtext = " << (char)(b_drawSearchText+48) << endl;
	ofs << "draw_searchrec = " << (char)(b_drawSearchTileRec+48) << endl;
	ofs << "draw_searchpulsate = " << (char)(b_drawSearchPulsate+48) << endl;
	ofs << endl;
	ofs << "add_baby_coords_to_list = " << (char)(addBabyCoordsToList+48) << endl;
	ofs << endl;
	ofs << "automatic_data_update = " << (char)(bAutoDataUpdate+48) << endl;
	ofs << "hetuw_log = " << (char)(bWriteLogs+48) << " // will create a log file '" << hetuwLogFileName << "' that logs different events" << endl;
	ofs << endl;
	ofs << "chat_delay = " << to_string((int)(sayDelay*10)) << " // wait atleast X time before sending the next text (10 = 1 second) - set it to 0 to deactivate it" << endl;
	ofs << endl;
	ofs << "remap_start_enabled = " << (char)(bRemapStart+48) << " // enable mushroom effect" << endl;
	ofs << "draw_hunger_warning = " << (char)(bDrawHungerWarning+48) << endl;
}

void HetuwMod::initSettings() {
	bool migrating = false;

	ifstream ifs;
	ifs.open( hetuwSettingsFileName );
	if (!ifs.good()) {
		// try opening an existing hetuw.cfg instead
		migrating = true;
		ifs.open("hetuw.cfg");
	}
	if (ifs.good()) { // file exists
		string line;
		while (getline(ifs, line)) {
			//printf("hetuw read line: %s\n", line.c_str());
			if (line.length() < 3) continue;
			if (line[0] == '/' && line[1] == '/') continue;
			char name[256];
			char value[256];
			getSettingsFileLine( name, sizeof(name), value, sizeof(value), line );
			if (strlen(name) < 1) continue;
			//printf("hetuw name: %s, value: %s\n", name, value);
			try {
				if (!setSetting( name, value ))
					printf("hetuw WARNING invalid %s line: %s\n", hetuwSettingsFileName, line.c_str());
			} catch (...) {
				printf("hetuw WARNING %s, exception thrown at line: %s\n", hetuwSettingsFileName, line.c_str());
			}
		}
	} else {
		// just a new install, neither file existed
		migrating = false;
	}
	ifs.close();

	if (cfgVersionRead < 2) {
		Phex::allowServerCoords = true;
	}
	if (cfgVersionRead < 3) {
		charKey_ShowDeathMessages = 254;
	}
	if (cfgVersionRead < 4) {
		bWriteLogs = true;
	}

	ofstream ofs( hetuwSettingsFileName, ofstream::out );
	writeSettings(ofs);
	ofs.close();

	if (migrating) {
		ofs.open("hetuw.cfg", ofstream::out);
		ofs << "// +------------------------------------------------+" << endl;
		ofs << "// | !!  WARNING: YumLife now uses yumlife.cfg.  !! |" << endl;
		ofs << "// |                                                |" << endl;
		ofs << "// | Changes made here will not affect YumLife.     |" << endl;
		ofs << "// | Your settings have been preserved below in     |" << endl;
		ofs << "// | case you need to use hetuw in the future.      |" << endl;
		ofs << "// |                                                |" << endl;
		ofs << "// +------------------------------------------------+" << endl;
		for (int i = 0; i < 20; ++i) {
			ofs << endl;
		}
		ofs << "// This file does not affect YumLife! Use yumlife.cfg." << endl;
		ofs << endl;
		writeSettings(ofs);
		ofs << endl;
		ofs << "// This file does not affect YumLife! Use yumlife.cfg." << endl;
		ofs.close();
	}
}

void HetuwMod::onGotServerAddress(char inUsingCustomServer, char *inServerIP, int inServerPort) {
	usingCustomServer = inUsingCustomServer;
	serverIP = inServerIP;
	serverPort = inServerPort;
	connectedToMainServer = strstr(hetuwLinkMainServer, inServerIP) ? true : false;
	if (connectedToMainServer) {
		if (arcRunningSince < 0) makeHttpRequest(hetuwLinkArcReport, &processArcReport);
	}
}

void HetuwMod::initOnBirth() { // will be called from LivingLifePage.cpp
	ourLiveObject = livingLifePage->getOurLiveObject();
	if (ourLiveObject->id == lastLoggedId) return;

	currentEmote = -1;
	lastSpecialEmote = 0;

	playersInMap.clear();
	playersInMap.shrink_to_fit();

	playersInRangeNum = 0;

	familiesInRange.clear();
	familiesInRange.shrink_to_fit();
	familiesInRange.push_back(new FamilyInRange());
	strcpy(familiesInRange[0]->name, hetuwDefaultOurFamilyName);

	deathMessages.clear();
	deathMessages.shrink_to_fit();

	// searchWordList.clear();
	// searchWordList.shrink_to_fit();

	homePosStack.clear();
	homePosStack.shrink_to_fit();

	cordOffset = { 0, 0 };
	addHomeLocation( 0, 0, hpt_birth ); // add birth location

	bTeachLanguage = false;
	teachLanguageCount = 0;
	timeLastLanguage = 0;

	timeLastSay = 0;
	sayBuffer.clear();
	sayBuffer.shrink_to_fit();

    yummyFoodChain.deleteAll();

	createNewLogFile();
	writeLineToLogs("my_birth", getTimeStamp());
	writeLineToLogs("my_id", to_string(ourLiveObject->id));
	writeLineToLogs("my_age", to_string((int)livingLifePage->hetuwGetAge(ourLiveObject)));

	Phex::onBirth();
}

void HetuwMod::initOnServerJoin() { // will be called from LivingLifePage.cpp and hetuwmod.cpp
	lastPosX = 9999;
	lastPosY = 9999;

	upKeyDown = false;
	downKeyDown = false;
	leftKeyDown = false;
	rightKeyDown = false;

	mapZoomInKeyDown = false;
	mapZoomInKeyDown = false;

	stopAutoRoadRun = false;
	activateAutoRoadRun = false;
	stopAutoRoadRunTime = 0;
	magnetMoveDir = -1;
	magnetWrongMoveDir = -1;

	waitForDoorToOpen = false;
	lastDoorToOpenX = 9999;
	lastDoorToOpenY = 9999;
	
	bNextCharForHome = false;

	bDrawInputString = false;
	getCustomCords = 0;

	getSearchInput = 0;

	takingPhoto = false;
	bxRay = false;
	bHidePlayers = false;

	bFoundFamilyName = false;
	ourFamilyName = hetuwDefaultOurFamilyName;

 	ourLiveObject = livingLifePage->getOurLiveObject();
	if (ourLiveObject) {
		ourGender = getObject(ourLiveObject->displayID)->male ? 'M' : 'F';
	}

	isMovingInVog = false;

	Phex::onServerJoin();
}

void HetuwMod::setLivingLifePage(LivingLifePage *inLivingLifePage, SimpleVector<LiveObject> *inGameObjects,
							SimpleVector<int> *inmMapContainedStacks, SimpleVector<SimpleVector<int>> *inmMapSubContainedStacks,
							int &inmMapD, int &inmCurMouseOverID) {
	livingLifePage = inLivingLifePage;
	gameObjects = inGameObjects;
	mMapContainedStacks = inmMapContainedStacks;
	mMapSubContainedStacks = inmMapSubContainedStacks;
	mMapD = &inmMapD;
	mCurMouseOverID = &inmCurMouseOverID;

	mouseRelativeToView.x = viewWidth/2;
	mouseRelativeToView.y = viewHeight/2;

	maxObjects = getMaxObjectID() + 1;

	initDangerousAnimals();

	if (objIsBeingSearched != NULL) delete[] objIsBeingSearched;
	objIsBeingSearched = new bool[maxObjects];
	setSearchArray();

	initBecomesFood();

	objectDrawScale = new double[maxObjects];
	for (int i=0; i<maxObjects; i++) objectDrawScale[i] = 1.0;

	objectDefaultColors = new float[maxObjects*3];
	for (int i=0, k=0; i<maxObjects; i++) {
		ObjectRecord *o = getObject(i);
		if (!o) continue;
		k = i*3;
		objectDefaultColors[k] = o->spriteColor->r;
		objectDefaultColors[k+1] = o->spriteColor->g;
		objectDefaultColors[k+2] = o->spriteColor->b;
	}

	initCustomFont();
}

void HetuwMod::initCustomFont() {
	int fontCharSpacing = 3; // vanilla main font is 6
	int fontSpaceWidth = 8; // vanilla main font is 16
	char fontFixedWidth = false;
	double fontScaleFactor = 16.0;
	customFont = new HetuwFont("font_32_64.tga", fontCharSpacing, fontSpaceWidth, fontFixedWidth, fontScaleFactor);
	customFont->setMinimumPositionPrecision( 1 );
}

bool HetuwMod::charArrEqualsCharArr(const char *a, const char *b) {
	if (!a || !b) return false;
	for (int i=0; i<512; i++) {
		if (a[i] == 0 && b[i] == 0) return true;
		if (a[i] == 0 || b[i] == 0) return false;
		if (toupper(a[i]) != toupper(b[i])) return false;
	}
	return false;
}

void HetuwMod::setSearchArray() {
	int descrSize = 32;
	char descr[descrSize];
	char exactSearchArr[64];
	for (int i=0; i<maxObjects; i++) {
		objIsBeingSearched[i] = false;
		ObjectRecord *o = getObject( i );
		if (!o) continue;
		bool exactSearch = false;
		for (unsigned k=0; k<searchWordList.size(); k++) {
			exactSearch = false;
			for (int m=0; m < 64; m++) {
				if (searchWordList[k][m] == 0) {
					if (m > 0 && searchWordList[k][m-1] == '.') {
						exactSearch = true;
						strcpy(exactSearchArr, searchWordList[k]);
						exactSearchArr[m-1] = 0;
					}
					break;
				}
			}

			getObjSearchDescr(o->description, descr, descrSize);

			if (exactSearch) {
				if (charArrEqualsCharArr(descr, exactSearchArr)) {
					objIsBeingSearched[i] = true;
					break;
				}
			} else if (charArrContainsCharArr(descr, searchWordList[k])) {
				//printf("hetuw search for id: %i, desc: %s\n", i, o->description);
				objIsBeingSearched[i] = true;
				break;
			}
		}
	}
}

string HetuwMod::getTimeStamp() {
	time_t t = time(NULL);
	struct tm *timeinfo = localtime(&t);
	char *str = asctime(timeinfo);
	str[strlen(str)-1] = 0; // remove end of line char
	return string(str);
}

string HetuwMod::getTimeStamp(time_t t) {
	struct tm *timeinfo = localtime(&t);
	char *str = asctime(timeinfo);
	str[strlen(str)-1] = 0; // remove end of line char
	return string(str);
}

void HetuwMod::createNewLogFile() {
	if (!bWriteLogs) return;
	ofstream ofs( hetuwLogFileName, ofstream::app );
	ofs.close();
}

void HetuwMod::writeLineToLogs(string name, string data) {
	if (!bWriteLogs) return;
	ofstream ofs( hetuwLogFileName, ofstream::out | ofstream::app );
	ofs << time(NULL) << hetuwLogSeperator << name << hetuwLogSeperator << data << endl;
	ofs.close();
}

HetuwMod::RainbowColor::RainbowColor() {
	color[0] = 1.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	increase = true;
	cycle = 1;
}

void HetuwMod::RainbowColor::step() {
	bool nextCycle = false;
	if (increase) {
		color[cycle] += stepSize;
		if (color[cycle] >= 1.0f) {
			color[cycle] = 1.0f;
			nextCycle = true;
		}
	} else {
		color[cycle] -= stepSize;
		if (color[cycle] <= 0.0f) {
			color[cycle] = 0.0f;
			nextCycle = true;
		}
	}
	if (nextCycle) {
		increase = !increase;
		cycle--;
		if (cycle < 0) cycle = 2;
	}
}

void HetuwMod::zoomCalc() {
	int newViewWidth = defaultViewWidth*zoomScale;
	int newViewHeight = defaultViewHeight*zoomScale;
	if (viewWidth != 0 && viewHeight != 0) {
		float scaleX = newViewWidth/(float)viewWidth;
		float scaleY = newViewHeight/(float)viewHeight;
		mouseRelativeToView.x *= scaleX;
		mouseRelativeToView.y *= scaleY;
	}
	viewWidth = newViewWidth;
	viewHeight = newViewHeight;
	panelOffsetX = (int)(viewWidth - defaultViewWidth)/2;
	panelOffsetY = (int)(viewHeight - defaultViewHeight)/2;
	tutMessageOffsetX = viewHeight * 0.14f;
	tutMessageOffsetX2 = viewHeight * 0.31f;
	guiScale = guiScaleRaw * zoomScale;
	hetuwSetViewSize();
	Phex::onZoom();
}

void HetuwMod::zoomIncrease(float value) {
	zoomScale *= 1.0f + value;
	if (zoomScale > 10.0f) zoomScale = 10.0f;
	zoomCalc();
}

void HetuwMod::zoomDecrease(float value) {
	zoomScale *= 1.0f - value;
	if (!bDrawPhotoRec && zoomScale < 1.0f) zoomScale = 1.0f;
	zoomCalc();
}

void HetuwMod::setZoom(float newZoom) {
	zoomScale = newZoom;
	zoomCalc();
}

void HetuwMod::guiScaleIncrease() {
	guiScaleRaw *= 0.9f;
	if (guiScaleRaw < 0.1) guiScaleRaw = 0.1;
	guiScale = guiScaleRaw * zoomScale;
	Phex::onGuiScaleChange();
}

void HetuwMod::guiScaleDecrease() {
	guiScaleRaw *= 1.1f;
	if (guiScaleRaw > 1.5) guiScaleRaw = 1.5;
	guiScale = guiScaleRaw * zoomScale;
	Phex::onGuiScaleChange();
}

void HetuwMod::onMouseEvent(float mX, float mY) {
	doublePair toViewCoords = getFromMapToViewCoordsVec();
	mouseRelativeToView.x = mX + toViewCoords.x;
	mouseRelativeToView.y = mY + toViewCoords.y;
	Phex::onMouseEvent(mX, mY);
}

void HetuwMod::getMouseXY(int &x, int &y) {
	doublePair toMapCoords = getFromViewToMapCoordsVec();
	x = mouseRelativeToView.x + toMapCoords.x;
	y = mouseRelativeToView.y + toMapCoords.y;
}

void HetuwMod::hSetDrawColor(float rgba[]) {
	setDrawColor(rgba[0], rgba[1], rgba[2], rgba[3]);
}

void HetuwMod::drawWaitingText(doublePair pos) {
	pos.y -= 60;
	char hStr[256];
	sprintf( hStr, hetuwWaitingText, toupper(HetuwMod::charKey_ShowHelp) );
	livingLifePage->hetuwDrawMainFont(hStr, pos, alignCenter);
	if (!invalidVersionDetected) return;
	pos.y -= 60;
	setDrawColor(1.0, 0.5, 0.0, 1.0);
	livingLifePage->hetuwDrawMainFont(strInvalidVersion.c_str(), pos, alignCenter);
	pos.y -= 45;
	livingLifePage->hetuwDrawMainFont(hetuwGetNewestVersionFromGithub, pos, alignCenter);
	setDrawColor(1.0, 1.0, 1.0, 1.0);
}

void HetuwMod::onInvalidVersionDetected(int version, int requiredVersion) {
	invalidVersionDetected = true;
	strInvalidVersion = "Warning: Invalid Version detected "+to_string(version)+" < "+to_string(requiredVersion);
}

void HetuwMod::onPlayerHoverOver(int id) {
	selectedPlayerID = id;
	timeLastPlayerHover = game_getCurrentTime();
}

void HetuwMod::stepLoopTroughObjectsInRange() {
	int radius = 32;
	int startX = ourLiveObject->xd - radius;
	int endX = ourLiveObject->xd + radius;
	int startY = ourLiveObject->yd - radius;
	int endY = ourLiveObject->yd + radius;
	for (int x = startX; x < endX; x++) {
		for (int y = startY; y < endY; y++) {
			int objId = livingLifePage->hetuwGetObjId( x, y );
			if (objId == OBJID_TarrMonument) {
				addHomeLocation(x, y, hpt_tarr);
			}
		}
	}
}

// void callBackFunc(const char* website, string error)
// type is optional - default is GET
// intervalSeconds is optional - default is 60 - if request fails wait for X seconds and resend it, if its below 0 than report error and stop
void HetuwMod::makeHttpRequest(string link, void (*callBackFunc)(const char*, string), string type, int intervalSeconds) {
	httpRequests.push_back(new HttpRequest(link, callBackFunc, type, intervalSeconds));
}

void HetuwMod::stepHttpRequests() {
	if (httpRequests.size() <= 0) return;
	for(int i=0; (unsigned)i<httpRequests.size(); i++) {
		if (!httpRequests[i]->step()) httpRequests.erase(httpRequests.begin()+i);
	}
}

void HetuwMod::gameStep() {
	curStepTime = game_getCurrentTime();
	curStepSecondsSince1970 = time(NULL);
	HetuwMouseActionBuffer* mouseBuffer = hetuwGetMouseActionBuffer();
	for (int i = 0; i < mouseBuffer->bufferPos; i++) {
		switch (mouseBuffer->buffer[i]) {
			case MouseButton::WHEELUP:
				zoomDecrease(HetuwMod::zoomValueScroll);
				break;
			case MouseButton::WHEELDOWN:
				zoomIncrease(HetuwMod::zoomValueScroll);
				break;
		}
	}
	mouseBuffer->Reset();

	stepHttpRequests();
}

void HetuwMod::livingLifeStep() {

	stepCount++;
	if (stepCount > 10000) stepCount = 0;

 	ourLiveObject = livingLifePage->getOurLiveObject();
	if (!ourLiveObject) return;

	if (stepCount % 10 == 0) 
		ourAge = livingLifePage->hetuwGetAge( ourLiveObject );

	move();

	SayStep();

	colorRainbow->step();

	if (stepCount % 50 == 0) {
		updateMap();
	}
	if (bDrawMap && mapZoomInKeyDown) {
		mapScale *= 0.96;
		if (mapScale < 8.7333) mapScale = 8.73333;
	}
	else if (bDrawMap && mapZoomOutKeyDown) {
		mapScale *= 1.04;
		if (mapScale > 80177784) mapScale = 80177784;
	}

	if (stepCount % 46 == 0) {
		if (!bFoundFamilyName) getOurFamilyName();
		updatePlayersInRangePanel();
	}

	if (activateAutoRoadRun) {
		if (time(NULL) > stopAutoRoadRunTime+2) {
			stopAutoRoadRun = false;
			stopAutoRoadRunTime = 0;
			activateAutoRoadRun = false;
		}
	}

	if (currentEmote >= 0 && lastEmoteTime+8 < time(NULL)) {
		lastEmoteTime = time(NULL);
		char message[64];
		sprintf( message, "EMOT 0 0 %i#", currentEmote);
        livingLifePage->sendToServerSocket( message );
	}

	if (bTeachLanguage) teachLanguage();

	if (bDrawYum || searchWordList.size() > 0) objectDrawScaleStep();
	
	if (b_drawYumColor && bDrawYum) {
		stepColorRainbowFast();
		setYumObjectsColor();
	}

	if (stepCount % 78 == 0) stepLoopTroughObjectsInRange();

	if (bMoveClick && !ourLiveObject->inMotion) {
		bMoveClick = false;
		int tileRX = bMoveClickX - ourLiveObject->xd;
		int tileRY = bMoveClickY - ourLiveObject->yd;
		if (bMoveClickAlpha) actionAlphaRelativeToMe(tileRX, tileRY);
		else actionBetaRelativeToMe(tileRX, tileRY);
	}
	
	if (livingLifePage->hetuwIsVogMode()) {
		if (intervalVogMove.step()) moveInVogMode();
	}
}

void HetuwMod::moveInVogMode() {
	isMovingInVog = false;

	int mouseX, mouseY;
	mouseX = mouseRelativeToView.x;
	mouseY = mouseRelativeToView.y;

	int x=0; int y=0;
	int distX = viewWidth - mouseX;
	int distY = viewHeight - mouseY;
	x = mouseX < distX ? mouseX : distX;
	y = mouseY < distY ? mouseY : distY;

	int maxScreenEdgeDistance = viewWidth * 0.03;
	if (x > maxScreenEdgeDistance && y > maxScreenEdgeDistance) return;
	bool xToZero = x > maxScreenEdgeDistance ? true : false;
	bool yToZero = y > maxScreenEdgeDistance ? true : false;
	x = maxScreenEdgeDistance - x;
	y = maxScreenEdgeDistance - y;
	if (xToZero) x = 0;
	if (yToZero) y = 0;

	if (mouseX < distX) x = -x;
	if (mouseY < distY) y = -y;

	int maxTileJump = 1*zoomScale;
	if (maxTileJump > 6) maxTileJump = 6;
	x = round(x/(float)maxScreenEdgeDistance) * maxTileJump;
	y = round(y/(float)maxScreenEdgeDistance) * maxTileJump;
	if (x == 0 && y == 0) return;

	int max = 100; // just to be sure - very high numbers are bad for the server
	if (x > max || x < -max || y > max || y < -max) return;

	doublePair vogPos = livingLifePage->hetuwGetVogPos();
	GridPos newPos;
	newPos.x = vogPos.x;
	newPos.y = vogPos.y;
	newPos.x += x;
	newPos.y += y;

	isMovingInVog = true;
	char *message = autoSprintf( "VOGM %d %d#", newPos.x, newPos.y );
	livingLifePage->sendToServerSocket( message );
}

void HetuwMod::setYumObjectsColor() {
	for (int i=0, k=0; i<maxObjects; i++) {
		ObjectRecord *o = getObject(i);
		if (!o) continue;
		if (isYummy(i)) {
			o->spriteColor->r = colorRainbowFast[0];
			o->spriteColor->g = colorRainbowFast[1]+0.5;
			o->spriteColor->b = colorRainbowFast[2];
		} else {
			k = i*3;
			o->spriteColor->r = objectDefaultColors[k];
			o->spriteColor->g = objectDefaultColors[k+1];
			o->spriteColor->b = objectDefaultColors[k+2];
		}
	}
}

void HetuwMod::resetObjectsColor() {
	if (!b_drawYumColor) return;
	for (int i=0, k=0; i<maxObjects; i++) {
		ObjectRecord *o = getObject(i);
		if (!o) continue;
		k = i*3;
		o->spriteColor->r = objectDefaultColors[k];
		o->spriteColor->g = objectDefaultColors[k+1];
		o->spriteColor->b = objectDefaultColors[k+2];
	}
}

void HetuwMod::stepColorRainbowFast() {
	int speed = 30;
	int speedDiv3 = speed/3;

	float interv = stepCount % speed / (float)speed;
	if (interv > 0.5) interv = 1 - interv;
	colorRainbowFast[0] = interv;
	interv = (stepCount+speedDiv3) % speed / (float)speed;
	if (interv > 0.5) interv = 1 - interv;
	colorRainbowFast[1] = interv;
	interv = (stepCount+(speedDiv3*2)) % speed / (float)speed;
	if (interv > 0.5) interv = 1 - interv;
	colorRainbowFast[2] = interv;
}

void HetuwMod::resetObjectDrawScale() {
	for (int i=0; i<maxObjects; i++) objectDrawScale[i] = 1.0;
}

void HetuwMod::objectDrawScaleStep() {
	if (!b_drawSearchPulsate && !b_drawYumPulsate) return;

	double scaleSearch = 1.0;
	double scaleYum = 1.0;

	float interv = stepCount % 60 / (float)60;
	if (interv > 0.5) interv = 1 - interv;
	scaleSearch += interv*1.0*zoomScale;

	interv = (stepCount+20) % 60 / (float)60;
	if (interv > 0.5) interv = 1 - interv;
	scaleYum += interv*1.0*zoomScale;

	for (int i=0; i<maxObjects; i++) {
		if (b_drawSearchPulsate && objIsBeingSearched[i]) objectDrawScale[i] = scaleSearch;
		else if (b_drawYumPulsate && bDrawYum && isYummy(i)) objectDrawScale[i] = scaleYum;
		else objectDrawScale[i] = 1.0;
	}
}

void HetuwMod::SayStep() {
	if (clearSayBuffer) {
		sayBuffer.clear();
		sayBuffer.shrink_to_fit();
		clearSayBuffer = false;
		return;
	}

	if (sayBuffer.size() < 1) return;

	double curTime = game_getCurrentTime();
	if (curTime-timeLastSay < sayDelay) return;
	timeLastSay = curTime;

	livingLifePage->hetuwSay(sayBuffer.front());
	
	char *p = sayBuffer.front();
	sayBuffer.erase(sayBuffer.begin(), sayBuffer.begin()+1);
	delete[] p;
}

void HetuwMod::Say(const char *text) {
	if (bTeachLanguage) bTeachLanguage = false;

	char *msg = new char[strlen(text)+1];
	strcpy(msg, text);
	sayBuffer.push_back(msg);
}

void HetuwMod::teachLanguage() {
	double curTime = game_getCurrentTime();
	if (curTime-timeLastLanguage < 2.1) return;
	timeLastLanguage = curTime;
	
	int maxTextLength = livingLifePage->hetuwGetTextLengthLimit();
	char text[maxTextLength+1];
	int size1 = languageArraySize1;
	//int size2 = languageArraySize2;

	for (int i=0; i<maxTextLength; i++) {
		for (int k=0; ; k++, i++) {
			//if (k >= size2 || languageArray[teachLanguageCount][k] == 0) {
				teachLanguageCount++;
				if (teachLanguageCount >= size1) {
					teachLanguageCount = 0;
				}
				break;
			//}
			if (i >= maxTextLength) break;
			//text[i] = languageArray[teachLanguageCount][k];
		}
		if (i < maxTextLength) text[i] = ' ';
	}
	text[maxTextLength] = 0;

	char *msg = new char[strlen(text)+1];
	strcpy(msg, text);
	sayBuffer.push_back(msg);
}

void HetuwMod::logHomeLocation(HomePos* hp) {
	if (!bWriteLogs) return;

	string typeName = "";
	switch (hp->type) {
		case hpt_custom:
			typeName = "custom";
			break;
		case hpt_birth:
			typeName = "birth";
			break;
		case hpt_home:
			typeName = "homemarker";
			break;
		case hpt_bell:
			typeName = "bell";
			break;
		case hpt_apoc:
			typeName = "apoc";
			break;
		case hpt_tarr:
			typeName = "tarr";
			break;
		case hpt_map:
			typeName = "map";
			if (hp->text.length() > 0) typeName += " "+hp->text;
			break;
		case hpt_baby:
			typeName = "baby";
			break;
		case hpt_babyboy:
			typeName = "babyboy";
			break;
		case hpt_babygirl:
			typeName = "babygirl";
			break;
		case hpt_expert:
			typeName = "expert";
			break;
		case hpt_phex:
			typeName = "phex";
			if (hp->text.length() > 0) typeName += " "+hp->text;
			break;
		default:
			typeName = "unknowntype";
	}

	string data = "";
	data = data + typeName + hetuwLogSeperator;
	data = data + "X: " + to_string(hp->x) + hetuwLogSeperator;
	data = data + "Y: " + to_string(hp->y);
	if (hp->type == hpt_custom) data = data + hetuwLogSeperator + hp->c;
	if (hp->text.length() > 0) data = data + hetuwLogSeperator + hp->text;

	writeLineToLogs("coord", data);
}

void HetuwMod::addHomeLocation(HomePos *p) {
	if (p->text.length() > 0) {
		for (unsigned i=0; i<homePosStack.size(); i++) {
			if (homePosStack[i]->type != p->type) continue;
			if (!Phex::strEquals(homePosStack[i]->text, p->text)) continue;
			homePosStack[i]->x = p->x;
			homePosStack[i]->y = p->y;
			delete p;
			return;
		}
	}

	homePosStack.push_back(p);
	logHomeLocation(p);
}

void HetuwMod::addHomeLocation( int x, int y, homePosType type, char c, int personID ) {
	if (personID >= 0 && type != hpt_expert) {
		for (unsigned i=0; i<homePosStack.size(); i++) {
			if (homePosStack[i]->personID == personID && homePosStack[i]->type == type) {
				homePosStack[i]->x = x;
				homePosStack[i]->y = y;
				return;
			}
		}
	}

	int id = -1;
	if (type == hpt_custom) {
		bool cordsAlreadyExist = false;
		for (unsigned i=0; i<homePosStack.size(); i++) {
			if (c == homePosStack[i]->c) {
				id = i;
			}
			if (homePosStack[i]->x == x && homePosStack[i]->y == y) {
				cordsAlreadyExist = true;
			}
		}
		if (cordsAlreadyExist) return;
		if (id >= 0) { // overwrite existing
			homePosStack[id]->x = x;
			homePosStack[id]->y = y;
			logHomeLocation(homePosStack[id]);
			return;
		}
	}
 
	for (unsigned i=0; i<homePosStack.size(); i++) {
		if (homePosStack[i]->x == x && homePosStack[i]->y == y) {
			id = i;
			break;
		}
	}
	if (id >= 0) return; // home already exists

	HomePos *p = new HomePos();
	p->x = x;
	p->y = y;
	p->type = type;
	p->c = c;
	p->personID = personID;
	homePosStack.push_back(p);
	logHomeLocation(p);

	if (type == hpt_bell) Phex::onRingBell(x, y);
	if (type == hpt_apoc) Phex::onRingApoc(x, y);
}

void HetuwMod::setHomeLocationText(int x, int y, homePosType type, char *text) {
	for (unsigned i=0; i<homePosStack.size(); i++) {
		HomePos *home = homePosStack[i];
		if (home->type != type) continue;
		if (home->x != x) continue;
		if (home->y != y) continue;
		home->text = string(text);
		if (home->text[0] == ':' && home->text.length() > 1) {
			home->text = home->text.substr(1, home->text.length()-1);
		}
		logHomeLocation(home);
		return;
	}
	printf("hetuw Warning: Could not find coord in list with x: %d, y: %d, text: %s\n", x, y, text);
}

void HetuwMod::setMapText(char *message, int mapX, int mapY) {
	if (!message) return;
	setHomeLocationText(mapX, mapY, hpt_map, message);
}

void HetuwMod::addPersonHomeLocation(int x, int y, int personID ) {
	//printf("hetuw addPersonHomeLocation x:%i, y:%i, id:%i\n", x, y, personID);
	if (!addBabyCoordsToList) return;
	LiveObject* person = livingLifePage->getLiveObject(personID);
	if (!person) addHomeLocation(x, y, hpt_baby); // if person does not exist it is a new baby, otherwise it might come from an order

	// person is not jet defined, person will be null, would need to do this later in order to get the gender
	if (true) return;
	if (!person) return;
	//printf("hetuw person not null, age: %f\n", livingLifePage->hetuwGetAge(person));
	if (livingLifePage->hetuwGetAge(person) < 1) {
		homePosType type = getObject(person->displayID)->male ? hpt_babyboy : hpt_babygirl;
		addHomeLocation(x, y, type);
	}
}

int HetuwMod::getObjYumID(ObjectRecord *obj) {
	if (obj->yumParentID > -1) {
		//printf("hetuw obj->id: %d yumParentID: %d\n", obj->id, obj->yumParentID);
		return obj->yumParentID;
	} else {
		//printf("hetuw obj->id: %d\n", obj->id);
		return obj->id;
	}
}


// thanks to https://raw.githubusercontent.com/JustinLove/onelife-client-patches/master/yum-hover
void HetuwMod::initBecomesFood() {
    becomesFoodID = new int[maxObjects];
    for (int i=0; i<maxObjects; i++) {
		becomesFoodID[i] = becomesFood( i, 3 );
    }
}

// TransRecord: (all of the following can be 0 or below if they dont exist)
// transRecord->actor = objectID holding in your hand - is 0 or smaller when not holding anything
// transRecord->target = objectID of obj on the ground that is being targeted
// transRecord->newActor = objectID of new obj holding in your hand (after transition)
// transRecord->newTarget = objectID of new item on the ground
int HetuwMod::becomesFood( int objectID, int depth ) {
    if( objectID < 0) return -1;

    ObjectRecord* obj = getObject( objectID );
    if( obj == NULL ) return -1;

    if( obj->isUseDummy ) {
        objectID = obj->useDummyParent;
        obj = getObject( objectID );
        }

	if (objectID == OBJID_SharpStone) return -1;
	if (objectID == OBJID_ClayBowl) return -1;
	if (objectID == OBJID_ClayPlate) return -1;
	if (objectID == OBJID_HotAdobeOven) return -1;
	if (objectID == OBJID_Fire) return -1;
	if (objectID == OBJID_HotCoals) return -1;

    if( obj->foodValue > 0 ) {
		return getObjYumID(obj);
        }

    if( depth < 1) return -1;

    SimpleVector<TransRecord*> *trans = getAllUses( objectID );
    if( trans == NULL ) return -1;

    if( trans->size() < 1 ) return -1;

    if( trans->size() == 1) {
        TransRecord* t = trans->getElementDirect( 0 );
        if( ! livingLifePage->getTransHintable( t ) ) return -1;

        int targetEdible = becomesFood( t->newTarget, depth - 1 );
        if( targetEdible > 0 ) return targetEdible;

        int actorEdible = becomesFood( t->newActor, depth - 1 );
        if( actorEdible > 0 ) return actorEdible;
        }
    else { // trans > 1
        int lastTarget = -1;
        int targetCount = 0;
        int lastActor = -1;
        int actorCount = 0;
        for( int i = 0; i<trans->size(); i++) {
            TransRecord* t = trans->getElementDirect( i );
            if( ! livingLifePage->getTransHintable( t ) ) continue;

            if( t->newActor != lastActor ) {
                actorCount += 1;
                }
            lastActor = t->newActor;

            if( t->newTarget != lastTarget ) {
                targetCount += 1;
                }
            lastTarget = t->newTarget;

            //int actorEdible = becomesFood( t->newActor, 0 );
            //if( actorEdible > 0 ) return actorEdible;
			if ((t->actor <= 0 || t->actor == OBJID_ClayBowl || t->actor == OBJID_ClayPlate || t->actor == OBJID_SharpStone) && t->newActor > 0) { // becomes food when using empty hand, clay bowl, clay plate, or sharp stone on it
				int returnID = becomesFood(t->newActor, depth - 1);
				if (returnID > 0) return returnID;
				returnID = becomesFood(t->newTarget, depth - 1);
				if (returnID > 0) return returnID;
				}
			if (t->target == OBJID_HotAdobeOven || t->target == OBJID_Fire || t->target == OBJID_HotCoals) { // becomes food when used on hot adobe oven or fire or hot coals
				int returnID = becomesFood(t->newActor, depth - 1);
				if (returnID > 0) return returnID;
				}
            }

        if( actorCount == 1) {
            int actorEdible = becomesFood( lastActor, depth - 1 );
            if( actorEdible > 0 ) return actorEdible;
            }
        if( targetCount == 1) {
            int targetEdible = becomesFood( lastTarget, depth - 1 );
            if( targetEdible > 0 ) return targetEdible;
            }
        }

    return -1;
}

bool HetuwMod::isYummy(int objID) {
	if( objID < 0 ) return false;
	int objectID = becomesFoodID[objID];
	if( objectID < 0 ) return false;

	for( int i=0; i<yummyFoodChain.size(); i++ ) {
		if( objectID == yummyFoodChain.getElementDirect(i) ) return false;
	}
	return true;
}

void HetuwMod::foodIsMeh(ObjectRecord *obj) {
	if (!obj) return;
	int objID = getObjYumID(obj);
	if (!isYummy(objID)) return;
	yummyFoodChain.push_back(objID);
}

void HetuwMod::onJustAteFood(ObjectRecord *food) {
	if (!food) return;
	if(food->isUseDummy) {
		yummyFoodChain.push_back( getObjYumID(getObject(food->useDummyParent)) );
	} else {
		yummyFoodChain.push_back( HetuwMod::getObjYumID(food) );
	}
}

void HetuwMod::livingLifeDraw() {
	fromViewToMapCoordsVec = getFromViewToMapCoordsVec();

	if (takingPhoto) return; // dont draw special mod stuff while taking a photo

 	ourLiveObject = livingLifePage->getOurLiveObject();
	if (!ourLiveObject) return;

	if (bDrawGrid) drawGrid();
	drawAge();
	if (bDrawCords) drawCords();
	if (iDrawPlayersInRangePanel > 0) drawPlayersInRangePanel();
	if (searchWordList.size() > 0) drawSearchList();
	if (bDrawDeathMessages) drawDeathMessages();
	if (bDrawHomeCords) drawHomeCords();
	if (bDrawHostileTiles) drawHostileTiles();
	if (searchWordList.size() > 0) drawSearchTiles();
	if (bDrawSelectedPlayerInfo && iDrawNames > 0 && !bHidePlayers) drawHighlightedPlayer();
	if (bDrawPhotoRec) drawPhotoRec(recTakePhoto);
	if (bDrawMap) drawMap();
	Phex::draw();
	if (getCustomCords == 1) drawCoordsHelpA();
	if (bDrawInputString) {
		if (getSearchInput > 0) drawSearchHelpText();
		if (getCustomCords == 2) drawCoordsHelpB();
		if (getCustomCords == 3) drawCoordsHelpC();
		drawInputString();
	}
	if (bNextCharForHome) drawCustomCoordHelpText();
	if (bDrawHelp) drawHelp();

	//setDrawColor( 1.0, 0, 0, 1.0 );
	//drawRect( debugRecPos, 10, 10 );
	//setDrawColor( 0.0, 1.0, 0, 1.0 );
	//drawRect( debugRecPos2, 10, 10 );

	if (bDrawBiomeInfo) drawBiomeIDs();
	if (bDrawHungerWarning) drawHungerWarning();
}

void HetuwMod::drawCoordsHelpA() {
	double scale = customFont->hetuwGetScaleFactor();
	customFont->hetuwSetScaleFactor(scale * guiScale);

	doublePair drawPos = lastScreenViewCenter;
	drawPos.y += customFont->getFontHeight()*1.1;
	drawPos = drawCustomTextWithBckgr(drawPos, "Press any letter key to create new coord");
	drawPos = drawCustomTextWithBckgr(drawPos, "Press ESC to abort");

	customFont->hetuwSetScaleFactor(scale);
}

void HetuwMod::drawCoordsHelpB() {
	double scale = customFont->hetuwGetScaleFactor();
	customFont->hetuwSetScaleFactor(scale * guiScale);

	doublePair drawPos = lastScreenViewCenter;
	drawPos.y += viewHeight*0.15;
	drawPos = drawCustomTextWithBckgr(drawPos, "Type the X value of the coord");
	drawPos = drawCustomTextWithBckgr(drawPos, "Press ESC to abort");

	customFont->hetuwSetScaleFactor(scale);
}

void HetuwMod::drawCoordsHelpC() {
	double scale = customFont->hetuwGetScaleFactor();
	customFont->hetuwSetScaleFactor(scale * guiScale);

	doublePair drawPos = lastScreenViewCenter;
	drawPos.y += viewHeight*0.15;
	drawPos = drawCustomTextWithBckgr(drawPos, "Type the Y value of the coord");
	drawPos = drawCustomTextWithBckgr(drawPos, "Press ESC to abort");

	customFont->hetuwSetScaleFactor(scale);
}

void HetuwMod::drawSearchHelpText() {
	double scale = customFont->hetuwGetScaleFactor();
	customFont->hetuwSetScaleFactor(scale * guiScale);

	float lineHeight = customFont->getFontHeight();
	doublePair drawPos = lastScreenViewCenter;
	drawPos.y += viewHeight*0.37;
	drawPos = drawCustomTextWithBckgr(drawPos, helpTextSearch[0].c_str());
	drawPos = drawCustomTextWithBckgr(drawPos, helpTextSearch[1].c_str());
	drawPos.y -= lineHeight;
	drawPos = drawCustomTextWithBckgr(drawPos, helpTextSearch[2].c_str());
	drawPos = drawCustomTextWithBckgr(drawPos, helpTextSearch[3].c_str());
	drawPos.y -= lineHeight;
	drawPos = drawCustomTextWithBckgr(drawPos, helpTextSearch[4].c_str());
	drawPos = drawCustomTextWithBckgr(drawPos, helpTextSearch[5].c_str());

	customFont->hetuwSetScaleFactor(scale);
}

void HetuwMod::drawCustomCoordHelpText() {
	double scale = customFont->hetuwGetScaleFactor();
	customFont->hetuwSetScaleFactor(scale * guiScale);

	float lineHeight = customFont->getFontHeight();
	doublePair drawPos = lastScreenViewCenter;
	drawPos.y += viewHeight*0.2;
	drawPos = drawCustomTextWithBckgr(drawPos, helpTextCustomCoord[0].c_str());
	drawPos = drawCustomTextWithBckgr(drawPos, helpTextCustomCoord[1].c_str());
	drawPos.y -= lineHeight;
	drawPos = drawCustomTextWithBckgr(drawPos, helpTextCustomCoord[2].c_str());
	drawPos.y -= lineHeight;
	drawPos = drawCustomTextWithBckgr(drawPos, helpTextCustomCoord[3].c_str());
	drawPos = drawCustomTextWithBckgr(drawPos, helpTextCustomCoord[4].c_str());

	customFont->hetuwSetScaleFactor(scale);
}

void HetuwMod::drawBiomeIDs() {
	int radius = 32;
	int startX = ourLiveObject->xd - radius;
	int endX = ourLiveObject->xd + radius;
	int startY = ourLiveObject->yd - radius;
	int endY = ourLiveObject->yd + radius;
	for (int x = startX; x < endX; x++) {
		for (int y = startY; y < endY; y++) {
			int mapI = livingLifePage->hetuwGetMapI(x, y);
			if (mapI < 0) continue; // out of range
			doublePair startPos = { (double)x, (double)y };
			startPos.x *= CELL_D;
			startPos.y *= CELL_D;
			string str = to_string(livingLifePage->mMapBiomes[mapI]);
			setDrawColor(0.0f,0.0f,0.0f,1.0f);
			livingLifePage->hetuwDrawScaledHandwritingFont( str.c_str(), startPos, guiScale );
		}
	}
}

doublePair HetuwMod::drawCustomTextWithBckgr(doublePair pos, const char* text) {
	float textWidth = customFont->measureString(text);
	float lineHeight = customFont->getFontHeight() * 1.1;
	float spaceWidth = customFont->hetuwGetSpaceWidth();
	setDrawColor( 0, 0, 0, 0.8 );
	drawRect( pos, (textWidth/2) + spaceWidth*2, lineHeight/2 );
	setDrawColor( 1, 1, 1, 1 );
	customFont->drawString( text, pos, alignCenter );
	pos.y -= lineHeight;
	return pos;
}

void HetuwMod::drawTextWithBckgr( doublePair pos, const char* text ) {
	float textWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( text, guiScale );
	setDrawColor( 0, 0, 0, 0.8 );
	drawRect( pos, (textWidth/2) + 6*guiScale, 14*guiScale );
	setDrawColor( 1, 1, 1, 1 );
	livingLifePage->hetuwDrawScaledHandwritingFont( text, pos, guiScale, alignCenter );
}

void HetuwMod::drawTextWithBckgr( doublePair pos, const char* text, float rgba[] ) {
	float textWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( text, guiScale );
	setDrawColor( 0, 0, 0, 0.8 );
	drawRect( pos, (textWidth/2) + 6*guiScale, 14*guiScale );
	hSetDrawColor(rgba);
	livingLifePage->hetuwDrawScaledHandwritingFont( text, pos, guiScale, alignCenter );
}

void HetuwMod::drawPointFromPercent(float x, float y) {
	pointFromPercentToMapCoords(x, y);
	drawRect({x, y}, 5., 5.);
}

void HetuwMod::hDrawRecFromPercent(double rec[]) {
	double tRec[4];
	for (int i=0; i<4; i++) tRec[i] = rec[i];
	HetuwMod::recFromPercentToMapCoords(tRec);
	hDrawRect(tRec);
}

void HetuwMod::hDrawRecsFromPercent(double rec[][4], int recCount) {
	double tRec[4];
	for (int r=0; r<recCount; r++) {
		for (int i=0; i<4; i++) tRec[i] = rec[r][i];
		HetuwMod::recFromPercentToMapCoords(tRec);
		hDrawRect(tRec);
	}
}

void HetuwMod::hDrawRect( doublePair startPos, doublePair endPos ) {
	double width = endPos.x - startPos.x;
	double height = endPos.y - startPos.y;
	width /= 2;
	height /= 2;
	startPos.x += width;
	startPos.y += height;
	drawRect( startPos, width, height );
}

void HetuwMod::hDrawRect(double rec[]) {
	doublePair startPos = { rec[0], rec[1] };
	doublePair endPos = { rec[2], rec[3] };
	hDrawRect(startPos, endPos);
}

void HetuwMod::hDrawRectWidthHeight(int left, int bottom, int width, int height) {
	int widthHalf = (int)(width/2.0);
	int heightHalf = (int)(height/2.0);
	doublePair center = { (double)(left+widthHalf), (double)(bottom+heightHalf) };
	drawRect(center, widthHalf, heightHalf);
}

void HetuwMod::hDrawRect(int startX, int startY, int endX, int endY) {
	hDrawRect({(double)startX, (double)startY}, {(double)endX, (double)endY});
}

void HetuwMod::hDrawRect(int rec[]) {
	hDrawRect({(double)rec[0], (double)rec[1]}, {(double)rec[2], (double)rec[3]});
}

void HetuwMod::drawTileRect( int x, int y ) {
	doublePair startPos = { (double)x, (double)y };
	startPos.x *= CELL_D;
	startPos.y *= CELL_D;
	drawRect( startPos, CELL_D/2, CELL_D/2 );
}

void HetuwMod::drawHostileTiles() {
	float alpha = 0.2;
	float interv = stepCount % 40 / (float)40;
	if (interv > 0.5) interv = 1 - interv;
	alpha += interv;
	setDrawColor( 1, 0, 0, alpha );
	//drawTileRect( ourLiveObject->xd, ourLiveObject->yd );

	int radius = 32;
	int startX = ourLiveObject->xd - radius;
	int endX = ourLiveObject->xd + radius;
	int startY = ourLiveObject->yd - radius;
	int endY = ourLiveObject->yd + radius;
	for (int x = startX; x < endX; x++) {
		for (int y = startY; y < endY; y++) {
			int objId = livingLifePage->hetuwGetObjId( x, y );
			if (objId >= 0 && objId < maxObjects) {
				if (isDangerousAnimal[objId]) drawTileRect( x, y );
			}
		}
	}
}

bool HetuwMod::charArrContainsCharArr(const char* arr1, const char* arr2) {
	int i = 0, k = 0, r = 0;
	while (true) {
		while (true) {
			if (arr1[i] == 0) return false;
			if (toupper(arr1[i]) == toupper(arr2[0])) break;
			i++;
		}
		k = 0;
		r = i;
		while (true) {
			if (arr2[k] == 0) return true;
			if (arr1[i] == 0) return false;
			if (toupper(arr1[i]) != toupper(arr2[k])) break;
			i++; k++;
		}
		i = r+1;
	}
}

void HetuwMod::strToUpper(const char* src, char* dest, int maxSize) {
	int i = 0;
	for ( ; src[i] != 0; i++) {
		if (i >= maxSize) {
			dest[i-1] = 0;
			break;
		}
		dest[i] = src[i];
		dest[i] = toupper(dest[i]);
	}
	dest[i] = 0;
}

// also removes text after #
void HetuwMod::objDescrToUpper(const char* arr, char* output, int maxSize) {
	int i=0;
	for (; arr[i] != 0; i++) {
		if (i >= maxSize) {
			output[i] = 0;
			break;
		}
		if (arr[i] == '#') break;
		output[i] = arr[i];
		output[i] = toupper(output[i]);
	}
	output[i] = 0;
}

void HetuwMod::getObjSearchDescr(const char* arr, char* output, int maxSize) {
	if (searchIncludeHashText) {
		strncpy(output, arr, maxSize);
		output[maxSize-1] = '\0';
	} else objGetDescrWithoutHashtag(arr, output, maxSize);
}

void HetuwMod::objGetDescrWithoutHashtag(const char* arr, char* output, int maxSize) {
	int i=0;
	for (; arr[i] != 0; i++) {
		if (i >= maxSize) {
			output[i] = 0;
			break;
		}
		if (arr[i] == '#') break;
		output[i] = arr[i];
	}
	output[i] = 0;
}

void HetuwMod::drawSearchTilesLoop(bool drawText) {
	int radius = 32;
	int startX = ourLiveObject->xd - radius;
	int endX = ourLiveObject->xd + radius;
	int startY = ourLiveObject->yd - radius;
	int endY = ourLiveObject->yd + radius;

	doublePair textPos;
	bool drawRec = true;

	int descrSize = 32;
	char descr[descrSize];

	for (int x = startX; x < endX; x++) {
		for (int y = startY; y < endY; y++) {
			if (drawText) {
				textPos.x = x * CELL_D;
				textPos.y = y * CELL_D - (CELL_D/2);
			}
			drawRec = true;

			int objId = livingLifePage->hetuwGetObjId( x, y );
			if (!objId || objId <= 0 || objId >= maxObjects) continue;
			if (objIsBeingSearched[objId]) {
				if (!drawText) { drawTileRect( x, y ); continue; }
				else {
					ObjectRecord *obj = getObject(objId);
					if (obj && obj->description) {
						getObjSearchDescr(obj->description, descr, descrSize);
						livingLifePage->hetuwDrawScaledMainFont( descr, textPos, 1.2, alignCenter );
						//customFont->drawString( descr, textPos, alignCenter );
						textPos.y += 24;
					}
				}
			}

			int mapI = livingLifePage->hetuwGetMapI( x, y );
			if (mapI < 0) continue;
			if (mMapContainedStacks[mapI].size() > 0) {
				//int *stackArray = mMapContainedStacks[mapI].getElementArray();
				int size = mMapContainedStacks[mapI].size();
				for (int i=0; i < size; i++) {
					//int objId = stackArray[i];
					int objId = mMapContainedStacks[mapI].getElementDirect(i);
					if (objId <= 0 || objId >= maxObjects) continue;
					if (objIsBeingSearched[objId]) {
						if (!drawText) { 
							if (drawRec) drawTileRect( x, y );
							drawRec = false;
							break;
						} else {
							ObjectRecord *obj = getObject(objId);
							if (obj && obj->description) {
								getObjSearchDescr(obj->description, descr, descrSize);
								livingLifePage->hetuwDrawMainFont( descr, textPos, alignCenter );
								//customFont->drawString( descr, textPos, alignCenter );
								textPos.y += 24;
							}
						}
					}
				}
				//delete[] stackArray;
			}
			if (!drawText && !drawRec) continue;
			if (mMapSubContainedStacks[mapI].size() > 0) {
				//SimpleVector<int> *subStackArray = mMapSubContainedStacks[mapI].getElementArray();
				int size = mMapSubContainedStacks[mapI].size();
				for (int i=0; i < size; i++) {
					if (!drawText && !drawRec) break;
					//int *vec = subStackArray[i].getElementArray();
					//int size2 = subStackArray[i].size();
					SimpleVector<int> vec = mMapSubContainedStacks[mapI].getElementDirect(i);
					int size2 = vec.size();
					//if (!vec) continue;
					for (int k=0; k < size2; k++) {
						//int objId = vec[i];
						int objId = vec.getElementDirect(k);
						if (objId <= 0 || objId >= maxObjects) continue;
						if (objIsBeingSearched[objId]) {
							if (!drawText) {
								if (drawRec) drawTileRect( x, y );
								drawRec = false;
								break;
							} else {
								ObjectRecord *obj = getObject(objId);
								if (obj && obj->description) {
									getObjSearchDescr(obj->description, descr, descrSize);
									livingLifePage->hetuwDrawMainFont( descr, textPos, alignCenter );
									//customFont->drawString( descr, textPos, alignCenter );
									textPos.y += 24;
								}
							}
						}
					}
					//delete[] vec;
				}
				//delete[] subStackArray;
			}
		}
	}
/*
	if (!gameObjects) return;
	for(int i=0; i<gameObjects->size(); i++) {
		LiveObject *o = gameObjects->getElement( i );
		if (!o) continue;
		for(int c=0; c < o->numContained; c++ ) {
			if (objIsBeingSearched[o->containedIDs[c]]) {
				if (!drawText) drawTileRect( o->xd, o->yd );
				else {
					ObjectRecord *obj = getObject(o->containedIDs[c]);
					if (obj && obj->description) {
						livingLifePage->hetuwDrawMainFont( obj->description, textPos,  alignCenter );
						textPos.y += 24;
					}
				}
			}
			for(int s=0; s < o->subContainedIDs[c].size(); s++) {
				//o->subContainedIDs[c].getElementDirect(s);
			}
		}
	}
*/
}

void HetuwMod::drawSearchTiles() {
	float alpha = 0.0;
	float interv = (20+stepCount) % 60 / (float)60;
	if (interv > 0.5) interv = 1 - interv;
	alpha += interv;

	if (b_drawSearchTileRec) {
		setDrawColor( 0.2, colorRainbow->color[1], colorRainbow->color[2], alpha );
		drawSearchTilesLoop(false);
	}
	if (b_drawSearchText) {
		setDrawColor( colorRainbow->color[1]-0.5, colorRainbow->color[2]-0.5, 0.7, 1.3-alpha );
		drawSearchTilesLoop(true);
	}
}

void HetuwMod::drawInputString() {
	doublePair drawPosA = lastScreenViewCenter;
	const char *sBufA = tempInputString.c_str();
	float textWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( sBufA, guiScale );
	setDrawColor( 0, 0, 0, 0.8 );
	drawRect( drawPosA, (textWidth/2) + 6*guiScale, 14*guiScale );
	setDrawColor( 1, 1, 1, 1 );
	livingLifePage->hetuwDrawScaledHandwritingFont( sBufA, drawPosA, guiScale, alignCenter );
}

void HetuwMod::createCordsDrawStr() {
	float biggestTextWidth = 0;
	char sBufA[64];
	int homeCount = 0;
	int bellCount = 0;
	int apocCount = 0;
	int tarrCount = 0;
	int mapCount = 0;
	int babyCount = 0;
	int expertCount = 0;
	int phexCount = 0;
	for (unsigned i=0; i<homePosStack.size(); i++) {
		switch (homePosStack[i]->type) {
			case hpt_custom:
				sprintf( sBufA, "%c %d %d", homePosStack[i]->c, homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				break;
			case hpt_birth:
				sprintf( sBufA, "BIRTH %d %d", homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				break;
			case hpt_home:
				sprintf( sBufA, "HOME %c %d %d", (char)(homeCount+65), homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				homeCount++;
				break;
			case hpt_bell:
				sprintf( sBufA, "BELL %c %d %d", (char)(bellCount+65), homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				bellCount++;
				break;
			case hpt_apoc:
				sprintf( sBufA, "APOC %c %d %d", (char)(apocCount+65), homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				apocCount++;
				break;
			case hpt_tarr:
				// if (tarrCount > 0) break; // make sure it doesnt add more than 1 tarr monument to the list - saftey feature because of bug - idk what causes the bug
				sprintf( sBufA, "TARR %c %d %d", (char)(tarrCount+65), homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				tarrCount++;
				break;
			case hpt_map: {
				string mapName = "MAP "+(char)(mapCount+65);
				if (homePosStack[i]->text.length() > 0) {
					if (homePosStack[i]->text.length() > 12) {
						mapName = homePosStack[i]->text.substr(0, 12);
					} else mapName = homePosStack[i]->text;
				}
				sprintf( sBufA, "%s %d %d", mapName.c_str(), homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				mapCount++;
				break; }
			case hpt_baby:
				sprintf( sBufA, "BABY %c %d %d", (char)(babyCount+65), homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				babyCount++;
				break;
			case hpt_babyboy:
				sprintf( sBufA, "BABY BOY %c %d %d", (char)(babyCount+65), homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				babyCount++;
				break;
			case hpt_babygirl:
				sprintf( sBufA, "BABY GIRL %c %d %d", (char)(babyCount+65), homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				babyCount++;
				break;
			case hpt_expert:
				sprintf( sBufA, "EXPERT %c %d %d", (char)(expertCount+65), homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				expertCount++;
				break;
			case hpt_phex: {
				string str = "PHEX "+(char)(phexCount+65);
				if (homePosStack[i]->text.length() > 0) {
					if (homePosStack[i]->text.length() > 12) {
						str = homePosStack[i]->text.substr(0, 12);
					} else str = homePosStack[i]->text;
				}
				sprintf( sBufA, "%s %d %d", str.c_str(), homePosStack[i]->x+cordOffset.x, homePosStack[i]->y+cordOffset.y );
				phexCount++;
				break; }
		}
		homePosStack[i]->drawStr = string(sBufA);

		float textWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( sBufA, guiScale );
		if (textWidth > biggestTextWidth) biggestTextWidth = textWidth;
	}
	longestCordsTextWidth = biggestTextWidth;
}

void HetuwMod::setDrawColorToCoordType(homePosType type) {
	switch (type) {
		case hpt_custom:
			setDrawColor( 1.0, 1.0, 1.0, 1.0 );
			break;
		case hpt_birth:
			setDrawColor( 0.63, 1.0, 0.8, 1.0 );
			break;
		case hpt_home:
			setDrawColor( 0.2, 0.8, 1.0, 1.0 );
			break;
		case hpt_bell:
			setDrawColor( 1.0, 1.0, 0.2, 1.0 );
			break;
		case hpt_apoc:
			setDrawColor( 1.0, 0.5, 0.2, 1.0 );
			break;
		case hpt_tarr:
			setDrawColor( 0.4, 1.0, 0.4, 1.0 );
			break;
		case hpt_map:
			setDrawColor( 0.7, 0.3, 1.0, 1.0 );
			break;
		case hpt_baby:
		case hpt_babyboy:
		case hpt_babygirl:
			setDrawColor( 1.0, 0.45, 0.8, 1.0 );
			break;
		case hpt_expert:
			setDrawColor( 0.6, 0.6, 0.7, 1.0 );
			break;
		case hpt_phex:
			setDrawColor( 0.5, 0.5, 0.5, 1.0 );
			break;
	}
}

void HetuwMod::drawHomeCords() {
	if (homePosStack.size() <= 0) return;

	int mouseX, mouseY;
	livingLifePage->hetuwGetMouseXY( mouseX, mouseY );
	
	createCordsDrawStr();

	doublePair drawPosA = lastScreenViewCenter;
	drawPosA.x -= HetuwMod::viewWidth/2 - (20*guiScale);
	drawPosA.y += HetuwMod::viewHeight/2 - (40*guiScale);
	drawPosA.y -= (40*guiScale);

	float recWidth = longestCordsTextWidth/2;
	float recHeight = homePosStack.size()*24*guiScale/2-12*guiScale;
	doublePair drawPosB = drawPosA;
	drawPosB.x += recWidth;
	drawPosB.y -= recHeight;
	setDrawColor( 0, 0, 0, 0.8 );
	drawRect( drawPosB, recWidth + 6*guiScale, recHeight + 14*guiScale );

	for (unsigned i=0; i<homePosStack.size(); i++) {
		if (homePosStack[i]->hasCustomColor) hSetDrawColor(homePosStack[i]->rgba);
		else setDrawColorToCoordType(homePosStack[i]->type);

		livingLifePage->hetuwDrawScaledHandwritingFont( homePosStack[i]->drawStr.c_str(), drawPosA, guiScale );

		homePosStack[i]->drawStartPos.x = drawPosB.x-recWidth-6*guiScale;
		homePosStack[i]->drawEndPos.x = drawPosB.x+recWidth+6*guiScale;
		homePosStack[i]->drawEndPos.y = drawPosA.y+14*guiScale;
		homePosStack[i]->drawStartPos.y = drawPosA.y-14*guiScale;
		drawPosA.y -= 24*guiScale;
		if (mouseX >= homePosStack[i]->drawStartPos.x && mouseX <= homePosStack[i]->drawEndPos.x) {
			if (mouseY >= homePosStack[i]->drawStartPos.y && mouseY <= homePosStack[i]->drawEndPos.y) {
				setDrawColor( 1, 1, 1, 0.4 );
				hDrawRect( homePosStack[i]->drawStartPos, homePosStack[i]->drawEndPos );
			}
		}
	}
}

void HetuwMod::setOurFamilyName(const char* lastName) {
	bFoundFamilyName = true;
	ourFamilyName = string(lastName);
	familiesInRange[0]->setName(ourFamilyName.c_str());
	familiesInRange[0]->generation = ourLiveObject->lineage.size()+1;
	familiesInRange[0]->raceName = getRaceName(getObject(ourLiveObject->displayID)); 
}

void HetuwMod::getOurFamilyName() {
	if (!gameObjects) return;
	if (ourLiveObject) {
		char lastName[32];
		getLastName(lastName, ourLiveObject->name);
		if (lastName[0] != 0) {
			setOurFamilyName(lastName);
			return;
		}
	}
	for(int i=0; i<gameObjects->size(); i++) {
		LiveObject *o = gameObjects->getElement( i );
		if (!isRelated(o)) continue;
		char lastName[32];
		getLastName(lastName, o->name);
		if (lastName[0] == 0) continue;
		setOurFamilyName(lastName);
		return;
	}
	ourFamilyName = hetuwDefaultOurFamilyName;
	bFoundFamilyName = false;
}

bool HetuwMod::isRelated( LiveObject* player ) {
	if (!player->relationName) return false;
	return true;
}

void HetuwMod::getRelationNameColor( const char* name, float* color ) {
	if ( !name ) {
		color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; 
	} else if ( strstr( name, "MOTHER" )) {
		if ( strstr( name, "GRANDMOTHER" )) {
			color[0] = 0.0f; color[1] = 0.7f; color[2] = 0.0f; 
		} else { // MOTHER
			color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; 
		}
	} else if ( strstr( name, "BROTHER" ) || strstr( name, "SISTER" )) {
		color[0] = 0.3f; color[1] = 1.0f; color[2] = 1.0f; 
	} else if ( strstr( name, "SON" ) || strstr( name, "DAUGHTER" )) {
		if( strstr( name, "GRAND" )) {
			color[0] = 0.4f; color[1] = 1.0f; color[2] = 0.0f; 
		} else { // DIRECT SON / DAUGTHER
			color[0] = 0.6f; color[1] = 1.0f; color[2] = 0.2f; 
		}
	} else if ( strstr( name, "UNCLE" ) || strstr( name, "AUNT" )) {
		if ( strstr( name, "GREAT" )) {
			color[0] = 0.2f; color[1] = 0.7f; color[2] = 0.5f; 
		} else { // DIRECT UNCLE / AUNT
			color[0] = 0.2f; color[1] = 0.7f; color[2] = 0.5f; 
		}
	} else if ( strstr( name, "NIECE" ) || strstr( name, "NEPHEW" )) {
		if ( strstr( name, "GREAT" )) {
			color[0] = 0.1f; color[1] = 0.7f; color[2] = 0.8f; 
		} else { // DIRECT NIECE / NEPHEW
			color[0] = 0.1f; color[1] = 0.7f; color[2] = 1.0f; 
		}
	} else if ( strstr( name, "COUSIN" )) {
		if ( strstr( name, "FIRST" )) {
			color[0] = 0.3f; color[1] = 0.6f; color[2] = 1.0f; 
		} else if ( strstr( name, "SECOND" )) {
			color[0] = 0.4f; color[1] = 0.5f; color[2] = 1.0f; 
		} else {
			color[0] = 0.6f; color[1] = 0.4f; color[2] = 1.0f; 
		}
	} else {
		color[0] = 0.6f; color[1] = 0.4f; color[2] = 1.0f; 
	}
}

bool HetuwMod::itsTimeToDrawPhexName() {
	return stepCount%200 < 130;
}

void HetuwMod::drawPlayerNames( LiveObject* player ) {
	if ( bHidePlayers ) return;
	if ( !player->name ) return;
	if ( player->hide || player->outOfRange ) return;
	if ( !player->allSpritesLoaded ) return;

	bool playerIsSelected = selectedPlayerID == player->id;
	if (bDrawSelectedPlayerInfo && playerIsSelected) {
		playerIsSelected = (game_getCurrentTime() - timeLastPlayerHover < 4);
		if (playerIsSelected) return;
	}

	playerNamePos.x = player->currentPos.x * CELL_D;
	playerNamePos.y = player->currentPos.y * CELL_D;
	playerNamePos.y += 34;

	getRelationNameColor( player->relationName, playerNameColor );

	setDrawColor( 0.0, 0.0, 0.0, 0.8 );
	if (itsTimeToDrawPhexName() &&
		Phex::playerIdToHash.find(player->id) != Phex::playerIdToHash.end()) {
		std::string* name = Phex::getUserDisplayName(Phex::playerIdToHash[player->id]);
		float textWidth = customFont->measureString( name->c_str() );
		drawRect( playerNamePos, textWidth/2 + 6, 16 );
		setDrawColor( playerNameColor[0], playerNameColor[1], playerNameColor[2], 1 );
		customFont->drawString( name->c_str(), playerNamePos, alignCenter );
	} else if ( iDrawNames == 2 ) {
		float textWidth = livingLifePage->hetuwMeasureStringHandwritingFont( player->name );
		drawRect( playerNamePos, textWidth/2 + 6, 16 );
		setDrawColor( playerNameColor[0], playerNameColor[1], playerNameColor[2], 1 );
		livingLifePage->hetuwDrawWithHandwritingFont( player->name, playerNamePos, alignCenter );
	} else if ( iDrawNames == 1 ) {
		char playerName[48];
		removeLastName( playerName, player->name );
		float textWidth = livingLifePage->hetuwMeasureStringHandwritingFont( playerName );
		drawRect( playerNamePos, textWidth/2 + 6, 16 );
		setDrawColor( playerNameColor[0], playerNameColor[1], playerNameColor[2], 1 );
		livingLifePage->hetuwDrawWithHandwritingFont( playerName, playerNamePos, alignCenter );
	}
}

void HetuwMod::drawHighlightedPlayer() {
	if (game_getCurrentTime() - timeLastPlayerHover >= 4) return;

	LiveObject *player = livingLifePage->getLiveObject(selectedPlayerID);
	if (!player) return;
	if ( player == ourLiveObject ) return;

	playerNamePos.x = player->currentPos.x * CELL_D;
	playerNamePos.y = player->currentPos.y * CELL_D;
	playerNamePos.y += 34;

	getRelationNameColor( player->relationName, playerNameColor );

	float textWidth;
	if (player->curseName && strlen(player->curseName) > 1) {
		playerNamePos.y += 32*guiScale;
		textWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( player->curseName, guiScale );
		setDrawColor( 0.0, 0.0, 0.0, 0.8 );
		drawRect( playerNamePos, textWidth/2 + 6*guiScale, 16*guiScale );
		setDrawColor( playerNameColor[0], playerNameColor[1], playerNameColor[2], 1 );
		livingLifePage->hetuwDrawScaledHandwritingFont( player->curseName, playerNamePos, guiScale, alignCenter );
		playerNamePos.y -= 32*guiScale;
	}
	if (itsTimeToDrawPhexName() &&
		Phex::playerIdToHash.find(player->id) != Phex::playerIdToHash.end()) {

		double scale = customFont->hetuwGetScaleFactor();
		customFont->hetuwSetScaleFactor(scale * guiScale);
		std::string* name = Phex::getUserDisplayName(Phex::playerIdToHash[player->id]);
		textWidth = customFont->measureString( name->c_str() );
		setDrawColor( 0.0, 0.0, 0.0, 0.8 );
		drawRect( playerNamePos, textWidth/2 + 6*guiScale, 16*guiScale );
		setDrawColor( playerNameColor[0], playerNameColor[1], playerNameColor[2], 1 );
		customFont->drawString( name->c_str(), playerNamePos, alignCenter );
		customFont->hetuwSetScaleFactor(scale);
		playerNamePos.y -= 32*guiScale;
	} else if (player->name && strlen(player->name) > 1) {
		textWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( player->name, guiScale );
		setDrawColor( 0.0, 0.0, 0.0, 0.8 );
		drawRect( playerNamePos, textWidth/2 + 6*guiScale, 16*guiScale );
		setDrawColor( playerNameColor[0], playerNameColor[1], playerNameColor[2], 1 );
		livingLifePage->hetuwDrawScaledHandwritingFont( player->name, playerNamePos, guiScale, alignCenter );
		playerNamePos.y -= 32*guiScale;
	}

	char str[16]; char age[8];
	livingLifePage->hetuwGetStringAge( age, player );
	char gender = getObject(player->displayID)->male ? 'M' : 'F';
	sprintf(str, "%c %s", gender, age);
	setDrawColor( 0.0, 0.0, 0.0, 0.8 );
	textWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( str, guiScale );
	drawRect( playerNamePos, textWidth/2 + 6*guiScale, 16*guiScale );
	setDrawColor( playerNameColor[0], playerNameColor[1], playerNameColor[2], 1 );
	livingLifePage->hetuwDrawScaledHandwritingFont( str, playerNamePos, guiScale, alignCenter );
}

void HetuwMod::useTileRelativeToMe( int x, int y ) {
	x += ourLiveObject->xd;
	y += ourLiveObject->yd;
	x = livingLifePage->sendX(x);
	y = livingLifePage->sendY(y);
	char msg[32];
	sprintf( msg, "USE %d %d#", x, y);
	livingLifePage->hetuwSetNextActionMessage( msg, x, y );
}

void HetuwMod::dropTileRelativeToMe( int x, int y ) {
	x += ourLiveObject->xd;
	y += ourLiveObject->yd;
	x = livingLifePage->sendX(x);
	y = livingLifePage->sendY(y);
	char msg[32];
	sprintf( msg, "DROP %d %d -1#", x, y);
	livingLifePage->hetuwSetNextActionMessage( msg, x, y );
}

void HetuwMod::remvTileRelativeToMe( int x, int y ) {
	x += ourLiveObject->xd;
	y += ourLiveObject->yd;
	x = livingLifePage->sendX(x);
	y = livingLifePage->sendY(y);
	char msg[32];
	sprintf( msg, "REMV %d %d -1#", x, y);
	livingLifePage->hetuwSetNextActionMessage( msg, x, y );
}

bool HetuwMod::objIdReverseAction( int objId ) {
	if (objId <= 0) return false;

	bool r = false;
	if ( ourLiveObject->holdingID <= 0 ) {
		switch (objId) {
			case 253: // full berry clay bowl
			case 225: // wheat bundle
				return true;
				break;
		}
		if ( getObject(objId) ) {
			char* descr	= getObject(objId)->description;
			if ( strstr(descr, "Bowl of") != NULL ) {
				return true;
			}
		}
	}
	return r;
}

void HetuwMod::actionAlphaRelativeToMe( int x, int y ) {
	x += ourLiveObject->xd;
	y += ourLiveObject->yd;

	int objId = livingLifePage->hetuwGetObjId( x, y);
	bool use = false;

	if (objId > 0) use = true;
	else use = false;

	if( ourLiveObject->holdingID > 0 ) {
		ObjectRecord *held = getObject( ourLiveObject->holdingID );

		if( held->foodValue == 0 ) {
			TransRecord *r = getTrans( ourLiveObject->holdingID, -1 );
			if( r != NULL && r->newTarget != 0 ) { // a use-on-ground transition exists!
                use = true;	// override the drop action
			}
		}
	}

	bool remove = false;
	if (objIdReverseAction(objId)) remove = true;
	
	if ( ourLiveObject->holdingID < 0 ) { // holding babay
		remove = false;
		use = false;
	}
	//printf("hetuw alphaActionObjId: %d\n", objId);

	x = livingLifePage->sendX(x);
	y = livingLifePage->sendY(y);
	char msg[32];
	if (remove) sprintf( msg, "REMV %d %d -1#", x, y);
	else if (use) sprintf( msg, "USE %d %d#", x, y);
	else sprintf( msg, "DROP %d %d -1#", x, y);
	livingLifePage->hetuwSetNextActionMessage( msg, x, y );
}

void HetuwMod::actionBetaRelativeToMe( int x, int y ) {
	x += ourLiveObject->xd;
	y += ourLiveObject->yd;

	bool remove = false;
	if (ourLiveObject->holdingID <= 0) {
		remove = true;
	}
	bool use = false;
	int objId = livingLifePage->hetuwGetObjId( x, y );
	if (objId > 0) {
		ObjectRecord* obj = getObject(objId);
		if (obj->numSlots == 0 && obj->blocksWalking) {
			TransRecord *r = getTrans( ourLiveObject->holdingID, objId );
			if ( r != NULL && r->newTarget != 0 ) {
				use = true;
			}
		}
	}

	if ( objIdReverseAction( objId ) ) use = true;

	if ( ourLiveObject->holdingID < 0 ) { // holding babay
		remove = false;
		use = false;
	}

	x = livingLifePage->sendX(x);
	y = livingLifePage->sendY(y);
	char msg[32];
	if (use) sprintf( msg, "USE %d %d#", x, y);
	else if (remove) sprintf( msg, "REMV %d %d -1#", x, y);
	else sprintf( msg, "DROP %d %d -1#", x, y);
	livingLifePage->hetuwSetNextActionMessage( msg, x, y );
	if (!remove) livingLifePage->hetuwSetNextActionDropping( true );
}

void HetuwMod::actionGammaRelativeToMe( int x, int y ) {
	x += ourLiveObject->xd;
	y += ourLiveObject->yd;

	x = livingLifePage->sendX(x);
	y = livingLifePage->sendY(y);
	char msg[32];
	sprintf( msg, "SWAP %d %d#", x, y);
	livingLifePage->hetuwSetNextActionMessage( msg, x, y );
	livingLifePage->hetuwSetNextActionDropping( true );
}

void HetuwMod::setOurSendPosXY(int &x, int &y) {
	x = round( ourLiveObject->xd );
	y = round( ourLiveObject->yd );
	x = livingLifePage->sendX(x);
	y = livingLifePage->sendY(y);
}

void HetuwMod::useBackpack(bool replace) {
	int clothingSlot = 5; // backpack clothing slot

	int x, y;
	setOurSendPosXY(x, y);

	char msg[32];
	if( ourLiveObject->holdingID > 0 ) {
		if (replace) {
			sprintf( msg, "DROP %d %d %d#", x, y, clothingSlot );
		} else {
			sprintf( msg, "SELF %d %d %d#", x, y, clothingSlot );
		}
		livingLifePage->hetuwSetNextActionMessage( msg, x, y );
		livingLifePage->hetuwSetNextActionDropping(true);
	} else {
		sprintf( msg, "SREMV %d %d %d %d#", x, y, clothingSlot, -1 );
		livingLifePage->hetuwSetNextActionMessage( msg, x, y );
	}
}

void HetuwMod::useApronPocket() {
	usePocket(1);
}

void HetuwMod::usePantsPocket() {
	usePocket(4);
}

void HetuwMod::usePocket(int clothingID) {
	int x, y;
	setOurSendPosXY(x, y);

	char msg[32];
	if( ourLiveObject->holdingID > 0 ) {
		sprintf( msg, "DROP %d %d %d#", x, y, clothingID );
		livingLifePage->hetuwSetNextActionMessage( msg, x, y );
		livingLifePage->hetuwSetNextActionDropping(true);
	} else {
		sprintf( msg, "SREMV %d %d %d %d#", x, y, clothingID, -1 );
		livingLifePage->hetuwSetNextActionMessage( msg, x, y );
	}
}

void HetuwMod::useOnSelf() {
	int x, y;
	setOurSendPosXY(x, y);

	if( ourLiveObject->holdingID <= 0 ) return;

	char msg[32];
	sprintf( msg, "SELF %d %d %d#", x, y, -1 );
	livingLifePage->hetuwSetNextActionMessage( msg, x, y );

	if( getObject( ourLiveObject->holdingID )->foodValue > 0)
		livingLifePage->hetuwSetNextActionEating(true);
}

void HetuwMod::pickUpBaby( int x, int y ) {
	char msg[32];
	sprintf( msg, "BABY %d %d#", x, y );
	livingLifePage->hetuwSetNextActionMessage( msg, x, y );
}

bool HetuwMod::playerIsInCloseRange( LiveObject* o ) {
	if ( o->outOfRange ) return false;

	if ( o->xd != ourLiveObject->xd && o->yd != ourLiveObject->yd ) return false; 
	int posDiff = 0;
	if ( o->xd == ourLiveObject->xd) posDiff = o->yd - ourLiveObject->yd;
	else if ( o->yd == ourLiveObject->yd) posDiff = o->xd - ourLiveObject->xd;
	if (posDiff > 1 || posDiff < -1) return false;
	return true;
}

void HetuwMod::pickUpBabyInRange() {
	if ( livingLifePage->hetuwGetAge( ourLiveObject ) < 13 ) return;

	if ( ourLiveObject->holdingID != 0 ) {
		dropTileRelativeToMe( 0, 0 );
		return;
	}

	// find new baby to pick up - prefer babies further away
	int babyFound = false;
	int babyX = 0;
	int babyY = 0;
	for(int i=0; i<gameObjects->size(); i++) {
		LiveObject *o = gameObjects->getElement( i );
			
		if ( livingLifePage->hetuwGetAge( o ) > 5 ) continue;

		if ( o->xd != ourLiveObject->xd && o->yd != ourLiveObject->yd ) continue; 
		if ( !babyFound ) {
			if ( o->xd == ourLiveObject->xd && o->yd == ourLiveObject->yd ) {
				babyFound = true;
				babyX = o->xd;
				babyY = o->yd;
				continue;
			}
		}
		int posDiff = 0;
		if ( o->xd == ourLiveObject->xd) posDiff = o->yd - ourLiveObject->yd;
		else if ( o->yd == ourLiveObject->yd) posDiff = o->xd - ourLiveObject->xd;
		if (posDiff > 1 || posDiff < -1) continue;

		pickUpBaby( o->xd, o->yd );
		return;
	}
	if ( !babyFound ) return;
	pickUpBaby( babyX, babyY );
}

void HetuwMod::takeOffBackpack() {
	char message[32];
	sprintf(message, "SELF %i %i 5#", ourLiveObject->xd, ourLiveObject->yd);
	livingLifePage->sendToServerSocket( message );
}

char* HetuwMod::stringToChar(string str) { // dont forget to delete[] cstr
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	return cstr;
}

void HetuwMod::setEmote(int id) {
	lastEmoteTime = time(NULL);
	currentEmote = id;
}

void HetuwMod::sendEmote(string emoteName) {
	char* cstr = stringToChar(emoteName);
	int emoteIndex = getEmotionIndex( cstr );
	sendEmote(emoteIndex);
	delete[] cstr;
}

void HetuwMod::sendEmote(int emoteId) {
	string message = "EMOT 0 0 "+to_string(emoteId)+"#";
	char* cstr = stringToChar(message);
	livingLifePage->sendToServerSocket( cstr );
	delete[] cstr;
}

void HetuwMod::causeDisconnect() {
	char message[64];
	sprintf( message, "EMOT 0 0 \\][###");
	livingLifePage->sendToServerSocket( message );
}

bool HetuwMod::isCharKey(unsigned char c, unsigned char key) {
	char tKey = key;
	return (c == key || c == toupper(tKey));
}

bool HetuwMod::addToTempInputString( unsigned char c, bool onlyNumbers, int minStrLen ) {
	if (c == 8) { // EREASE
		if (tempInputString.length() <= (unsigned)minStrLen) return true;
		tempInputString = tempInputString.substr(0, tempInputString.length()-1);
		return true;
	}
	if (onlyNumbers) {
		if ((c < '0' || c > '9') && c != '-')
			return false;
	}
	tempInputString += c;
	return true;
}

// when return true -> end/return in keyDown function in LivingLife
bool HetuwMod::livingLifeKeyDown(unsigned char inASCII) {

	if (sendKeyEvents) {
		char message[32];
		sprintf(message, "KEY_EVENT %c#", inASCII);
		livingLifePage->sendToServerSocket( message );
	}

	if (Phex::onKeyDown(inASCII)) return true;

	if (livingLifePage->hetuwSayFieldIsFocused()) {
		return false;
	}
	// player is not trying to say something

	bool commandKey = isCommandKeyDown();
	bool controlKey = isControlKeyDown();
	bool altKey = isAltKeyDown();
	bool shiftKey = isShiftKeyDown();

	//printf("hetuw key pressed %c, value: %i, shiftKey %i, commandKey %i\n", inASCII, (int)inASCII, (int)shiftKey, (int)commandKey);

	if (!commandKey && !shiftKey && inASCII == 27) { // ESCAPE KEY
		upKeyDown = false;
		leftKeyDown = false;
		downKeyDown = false;
		rightKeyDown = false;
		bDrawHelp = false;
		bDrawMap = false;
		lastPosX = 9999;
		lastPosY = 9999;
		stopAutoRoadRunTime = time(NULL);
		activateAutoRoadRun = true;
		getCustomCords = 0;
		getSearchInput = 0;
		bDrawInputString = false;
		bxRay = false;
		bHidePlayers = false;
		bTeachLanguage = false;
		clearSayBuffer = true;
		bDrawPhotoRec = false;
	}

	if (bNextCharForHome) {
		bNextCharForHome = false;
		char c = toupper(inASCII);
		if (c >= 65 && c <= 90) {
			addHomeLocation( ourLiveObject->xd, ourLiveObject->yd, hpt_custom, c );
			bDrawHomeCords = true;
			return true;
		}
	}

	if (getCustomCords > 0) {
		if (getCustomCords == 1) {
			tempCordChar = toupper(inASCII);
			tempInputString = "X: ";
			getCustomCords++;
			bDrawInputString = true;
			return true;
		} else {
			if (inASCII == 13) { // ENTER
				if (getCustomCords == 3) {
					string cordStr = tempInputString.substr(3, tempInputString.length());
					try {
						tempCordY = stoi( cordStr );
					} catch(std::exception const & e) {
						getCustomCords = 0;
						bDrawInputString = false;
						return true;
					}
					addHomeLocation( tempCordX-cordOffset.x, tempCordY-cordOffset.y, hpt_custom, tempCordChar );
					getCustomCords = 0;
					bDrawInputString = false;
					bDrawHomeCords = true;
					return true;
				}
				string cordStr = tempInputString.substr(3, tempInputString.length());
				try {
					tempCordX = stoi( cordStr );
				} catch(std::exception const & e) {
					getCustomCords = 0;
					bDrawInputString = false;
					return true;
				}
				tempInputString = "Y: ";
				getCustomCords++;
				return true;
			}
			addToTempInputString( inASCII, true, 3);
			return true;
		}
	}

	if (getSearchInput > 0) {
		if (inASCII == 13) { // ENTER
			string strSearch = tempInputString.substr(8, tempInputString.length());
			bDrawInputString = false;
			getSearchInput = 0;
			if (strSearch.size() < 1) return true;
			char *cstrSearchWord = new char[strSearch.size()+1];
			strcpy(cstrSearchWord, strSearch.c_str());
			searchWordList.push_back(cstrSearchWord);
			searchWordStartPos.push_back(new doublePair());
			searchWordEndPos.push_back(new doublePair());
			searchWordListDelete.push_back(false);
			setSearchArray();
			//printf("hetuw strSearch: %s\n", strSearch.c_str());
		} else { // not enter
			addToTempInputString( toupper(inASCII), false, 8);
		}
		return true;
	}

	// for debugging
	if (false && inASCII == 'i') {
		//causeDisconnect(); if (true) return true;
		int mouseX, mouseY;
		livingLifePage->hetuwGetMouseXY( mouseX, mouseY );
		int x = round( mouseX / (float)CELL_D );
		int y = round( mouseY / (float)CELL_D );
		int objId = livingLifePage->hetuwGetObjId( x, y );
		printf("hetuw cell: %i, %i objID: %i\n", x, y, objId);
		if (objId > 0) {
			ObjectRecord *o = getObject( objId );
			if (o && o->description) {
				printf("hetuw description: %s\n", o->description);
			}
			printf("hetuw yum: %c, beingSearched: %c\n", isYummy(objId) ? '1' : '0', objIsBeingSearched[objId] ? '1' : '0');
		}
		int mapI = livingLifePage->hetuwGetMapI( x, y );
		if (mapI < 0) return true;
		if (mMapContainedStacks[mapI].size() > 0) {
			for (int i=0; i < mMapContainedStacks[mapI].size(); i++) {
				int objId = *mMapContainedStacks[mapI].getElement(i);
				if (objId <= 0) continue;
				ObjectRecord *o = getObject( objId );
				if (o && o->description) {
					printf("hetuw contains %i. %s\n", i, o->description);
				}
			}
		}
		if (mMapSubContainedStacks[mapI].size() > 0) {
			for (int i=0; i < mMapSubContainedStacks[mapI].size(); i++) {
				SimpleVector<int> *vec = mMapSubContainedStacks[mapI].getElement(i);
				for (int k=0; k < vec->size(); k++) {
					int objId = *vec->getElement(k);
					if (objId <= 0) continue;
					ObjectRecord *o = getObject( objId );
					if (o && o->description) {
						printf("hetuw sub contains %i.%i. %s\n", i, k, o->description);
					}
				}
			}
		}
		return true;
	}

	// emotes
	if (!commandKey && !shiftKey) {
		int jic = (int)inASCII - 48;
		if (jic >= 0 && jic <= 9) {
			if (jic > 6) jic += 2;
			currentEmote = -1;
			char message[64];
			sprintf( message, "EMOT 0 0 %i#", jic);
	        livingLifePage->sendToServerSocket( message );
			return true;
		}
	}

//	if (inASCII == 'u') {
//		useTileRelativeToMe(1, 0);
//		return true;
//	}
//	if (inASCII == 't') {
//		dropTileRelativeToMe(1, 0);
//		return true;
//	}
//	if (inASCII == 'r') {
//		remvTileRelativeToMe(1, 0);
//		return true;
//	}

	if (!commandKey && isCharKey(inASCII, charKey_ShowHelp)) {
		bDrawHelp = !bDrawHelp;
		return true;
	}
	if (!commandKey && isCharKey(inASCII, charKey_ShowNames)) {
		iDrawNames++;
		if (iDrawNames >= 3) iDrawNames = 0;
		return true;
	}
	if (!commandKey && !shiftKey && isCharKey(inASCII, charKey_ShowCords)) {
		bDrawCords = !bDrawCords;
		return true;
	}
	if (!commandKey && shiftKey && isCharKey(inASCII, charKey_ShowCords)) {
		cordOffset.x = -ourLiveObject->xd;
		cordOffset.y = -ourLiveObject->yd;
		return true;
	}
	if (!commandKey && isCharKey(inASCII, charKey_ShowPlayersInRange)) {
		iDrawPlayersInRangePanel++;
		if (iDrawPlayersInRangePanel >= 3) iDrawPlayersInRangePanel = 0;
		return true;
	}
	if (!commandKey && isCharKey(inASCII, charKey_ShowDeathMessages)) {
		bDrawDeathMessages = !bDrawDeathMessages;
		return true;
	}
	if (!commandKey && isCharKey(inASCII, charKey_ShowHomeCords)) {
		bDrawHomeCords = !bDrawHomeCords;
		return true;
	}
	if (!commandKey && !shiftKey && isCharKey(inASCII, charKey_CreateHome)) {
		bNextCharForHome = true;
		return true;
	}
	if (!commandKey && shiftKey && isCharKey(inASCII, charKey_CreateHome)) {
		getCustomCords = 1;
		return true;
	}
	if (!commandKey && !shiftKey && isCharKey(inASCII, charKey_Search)) {
		tempInputString = "SEARCH: ";
		getSearchInput = 1;
		bDrawInputString = true;
		return true;
	}
	if (!commandKey && shiftKey && isCharKey(inASCII, charKey_Search)) {
		if (searchWordList.size() > 0) searchWordListDelete[searchWordList.size()-1] = true;
		return true;
	}
	if (!commandKey && isCharKey(inASCII, charKey_FixCamera)) {
		if (!bHoldDownTo_FixCamera) livingLifePage->hetuwToggleFixCamera();
		else if (!cameraIsFixed) livingLifePage->hetuwToggleFixCamera();
		return true;
	}
	if (!bDrawMap && !commandKey && isCharKey(inASCII, charKey_ShowHostileTiles)) {
		bDrawHostileTiles = !bDrawHostileTiles;
		return true;
	}
	if (!commandKey && isCharKey(inASCII, charKey_xRay)) {
		if (bHoldDownTo_XRay) bxRay = true;
		else bxRay = !bxRay;
		return true; 
	}
	if (!commandKey && isCharKey(inASCII, charKey_TeachLanguage)) {
		//bTeachLanguage = !bTeachLanguage;
		//if (!bTeachLanguage) teachLanguageCount = 0;
		//return true;
	}
	if (!commandKey && isCharKey(inASCII, charKey_FindYum)) {
		if (bHoldDownTo_FindYum) bDrawYum = true;
		else bDrawYum = !bDrawYum;
		if (!bDrawYum) {
			resetObjectDrawScale();
			resetObjectsColor();
		}
		return true;
	}
	if (!commandKey && !shiftKey && isCharKey(inASCII, charKey_ShowGrid)) {
		if (bHoldDownTo_ShowGrid) bDrawGrid = true;
		else bDrawGrid = !bDrawGrid;
		return true;
	}
	if (!commandKey && isCharKey(inASCII, charKey_MakePhoto)) {
		if (bDrawPhotoRec) {
			bDrawPhotoRec = false;
			HetuwMod::takingSpecialPhoto = true;
			livingLifePage->hetuwSetTakingPhoto(true);
			HetuwMod::setTakingPhoto(true);
		} else {
			bDrawPhotoRec = true;
		}
		return true;
	}

	if (controlKey) {
		if (isCharKey(inASCII, charKey_TileStandingOn)) {
			actionBetaRelativeToMe( 0, 0 );
			return true;
		}
	} else if (altKey) {
		if (isCharKey(inASCII, charKey_TileStandingOn)) {
			actionGammaRelativeToMe( 0, 0);
			return true;
		}
	} else {
		if (isCharKey(inASCII, charKey_TileStandingOn)) {
			actionAlphaRelativeToMe( 0, 0 );
			return true;
		}
	}

	if (!shiftKey && !commandKey) {
		if (inASCII == charKey_Up || inASCII == toupper(charKey_Up)) {
			upKeyDown = true;
			stopAutoRoadRun = true;
			return true;
		}
		if (inASCII == charKey_Left || inASCII == toupper(charKey_Left)) {
			leftKeyDown = true;
			stopAutoRoadRun = true;
			return true;
		}
		if (inASCII == charKey_Down || inASCII == toupper(charKey_Down)) {
			downKeyDown = true;
			stopAutoRoadRun = true;
			return true;
		}
		if (inASCII == charKey_Right || inASCII == toupper(charKey_Right)) {
			rightKeyDown = true;
			stopAutoRoadRun = true;
			return true;
		}
	} else if (controlKey) {
		if (inASCII+64 == toupper(charKey_Up)) {
			actionBetaRelativeToMe( 0, 1 );
			return true;
		}
		if (inASCII+64 == toupper(charKey_Left)) {
			actionBetaRelativeToMe( -1, 0 );
			return true;
		}
		if (inASCII+64 == toupper(charKey_Down)) {
			actionBetaRelativeToMe( 0, -1 );
			return true;
		}
		if (inASCII+64 == toupper(charKey_Right)) {
			actionBetaRelativeToMe( 1, 0 );
			return true;
		}
    } else if (altKey) {
		if (inASCII == charKey_Up || inASCII == toupper(charKey_Up)) {
			actionGammaRelativeToMe( 0, 1 );
			return true;
		}
		if (inASCII == charKey_Left || inASCII == toupper(charKey_Left)) {
			actionGammaRelativeToMe( -1, 0 );
			return true;
		}
		if (inASCII == charKey_Down || inASCII == toupper(charKey_Down)) {
			actionGammaRelativeToMe( 0, -1 );
			return true;
		}
		if (inASCII == charKey_Right || inASCII == toupper(charKey_Right)) {
			actionGammaRelativeToMe( 1, 0 );
			return true;
		}
	} else if (shiftKey) {
		if (inASCII == charKey_Up || inASCII == toupper(charKey_Up)) {
			actionAlphaRelativeToMe( 0, 1 );
			return true;
		}
		if (inASCII == charKey_Left || inASCII == toupper(charKey_Left)) {
			actionAlphaRelativeToMe( -1, 0 );
			return true;
		}
		if (inASCII == charKey_Down || inASCII == toupper(charKey_Down)) {
			actionAlphaRelativeToMe( 0, -1 );
			return true;
		}
		if (inASCII == charKey_Right || inASCII == toupper(charKey_Right)) {
			actionAlphaRelativeToMe( 1, 0 );
			return true;
		}
	}

	if (!shiftKey && isCharKey(inASCII, charKey_Backpack)) {
		if (weAreWearingABackpack()) useBackpack();
		else if (weAreWearingPantsWithPocket()) usePantsPocket();
		else if (weAreWearingShirtWithPocket()) useApronPocket();
		else useBackpack();
		return true;
	}
	if ((shiftKey || commandKey) && isCharKey(inASCII, charKey_Backpack)) {
		if (weAreWearingABackpack()) useBackpack(true);
		else if (weAreWearingPantsWithPocket()) usePantsPocket();
		else if (weAreWearingShirtWithPocket()) useApronPocket();
		else useBackpack(true);
		return true;
	}
	if (isCharKey(inASCII, charKey_Eat)) {
		useOnSelf();
		return true;
	}
	if (isCharKey(inASCII, charKey_Baby)) {
		pickUpBabyInRange();
		return true;
	}
	if (!commandKey && !shiftKey && isCharKey(inASCII, charKey_TakeOffBackpack)) {
		takeOffBackpack();
		return true;
	}
	if (shiftKey && isCharKey(inASCII, charKey_Pocket)) {
		useApronPocket();
		return true;
	}
	if (!shiftKey && isCharKey(inASCII, charKey_Pocket)) {
		usePantsPocket();
		return true;
	}

	if (!commandKey && shiftKey && isCharKey(inASCII, charKey_ShowMap)) {
		bDrawMap = !bDrawMap;
		return true;
	}
	if (!commandKey && isCharKey(inASCII, charKey_MapZoomIn)) {
		mapZoomInKeyDown = true;
		return true;
	}
	if (!commandKey && isCharKey(inASCII, charKey_MapZoomOut)) {
		mapZoomOutKeyDown = true;
		return true;
	}

	if (!commandKey && isCharKey(inASCII, charKey_HidePlayers)) {
		bHidePlayers = !bHidePlayers;
		return true;
	}
	//printf("hetuw unknown key %c, value: %i\n", inASCII, (int)inASCII);

	return false;
}

bool HetuwMod::livingLifeKeyUp(unsigned char inASCII) {

	bool r = false;

	if (Phex::onKeyUp(inASCII)) r = true;

	bool commandKey = isCommandKeyDown();
	bool shiftKey = isShiftKeyDown();

	if (inASCII == charKey_Up || inASCII == toupper(charKey_Up)) {
		upKeyDown = false;
		r = true;
	}
	if (inASCII == charKey_Left || inASCII == toupper(charKey_Left)) {
		leftKeyDown = false;
		r = true;
	}
	if (inASCII == charKey_Down || inASCII == toupper(charKey_Down)) {
		downKeyDown = false;
		r = true;
	}
	if (inASCII == charKey_Right || inASCII == toupper(charKey_Right)) {
		rightKeyDown = false;
		r = true;
	}
	if (commandKey) {
		if (inASCII+64 == toupper(charKey_Up)) {
			upKeyDown = false;
			r = true;
		}
		if (inASCII+64 == toupper(charKey_Left)) {
			leftKeyDown = false;
			r = true;
		}
		if (inASCII+64 == toupper(charKey_Down)) {
			downKeyDown = false;
			r = true;
		}
		if (inASCII+64 == toupper(charKey_Right)) {
			rightKeyDown = false;
			r = true;
		}
	}

	if (!commandKey && isCharKey(inASCII, charKey_FixCamera)) {
		if (bHoldDownTo_FixCamera && cameraIsFixed) {
			livingLifePage->hetuwToggleFixCamera();
			r = true;
		}
	}
	if (!commandKey && isCharKey(inASCII, charKey_xRay)) {
		if (bHoldDownTo_XRay) {
			bxRay = false;
			r = true;
		}
	}
	if (!commandKey && isCharKey(inASCII, charKey_FindYum)) {
		if (bHoldDownTo_FindYum) {
			bDrawYum = false;
			resetObjectDrawScale();
			resetObjectsColor();
			r = true;
		}
	}
	if (!commandKey && !shiftKey && isCharKey(inASCII, charKey_ShowGrid)) {
		if (bHoldDownTo_ShowGrid) {
			bDrawGrid = false;
		}
	}

	if (inASCII == charKey_MapZoomIn || inASCII == toupper(charKey_MapZoomIn)) {
		mapZoomInKeyDown = false;
		r = true;
	}
	if (inASCII == charKey_MapZoomOut || inASCII == toupper(charKey_MapZoomOut)) {
		mapZoomOutKeyDown = false;
		r = true;
	}

	if (!upKeyDown && !leftKeyDown && !downKeyDown && !rightKeyDown) {
		lastPosX = 9999;
		lastPosY = 9999;
		stopAutoRoadRunTime = time(NULL);
		activateAutoRoadRun = true;
		magnetMoveDir = -1;
		magnetWrongMoveDir = -1;
		magnetMoveCount = 0;
	}

	return r;
}

bool HetuwMod::livingLifeSpecialKeyDown(unsigned char inKeyCode) {
	bool commandKey = isCommandKeyDown();
	bool shiftKey = isShiftKeyDown();
	bool r = false;

	if (!isCommandKeyDown() && !isShiftKeyDown()) {
		if( inKeyCode == MG_KEY_LEFT ) { 
			zoomDecrease(HetuwMod::zoomValueKey);
		} else if( inKeyCode == MG_KEY_RIGHT ) { 
			zoomIncrease(HetuwMod::zoomValueKey);
		}
	}
	if (isCommandKeyDown()) {
		if( inKeyCode == MG_KEY_LEFT ) {
			guiScaleDecrease();
		} else if( inKeyCode == MG_KEY_RIGHT ) {
			guiScaleIncrease();
		}
	}

	if (!commandKey && !shiftKey) {
		if ( inKeyCode == MG_KEY_F1 ) {
			currentEmote = -1;
			char message[] = "EMOT 0 0 12#"; // HMPH
	        livingLifePage->sendToServerSocket( message );
			r = true;
		}
		if ( inKeyCode == MG_KEY_F2 ) {
			currentEmote = -1;
			char message[] = "EMOT 0 0 13#"; // LOVE
	        livingLifePage->sendToServerSocket( message );
			r = true;
		}
		if ( inKeyCode == MG_KEY_F3 ) {
			currentEmote = -1;
			char message[] = "EMOT 0 0 14#"; // OREALLY
	        livingLifePage->sendToServerSocket( message );
			r = true;
		}
		if ( inKeyCode == MG_KEY_F4 ) {
			currentEmote = -1;
			char message[] = "EMOT 0 0 15#"; // SHOCK
	        livingLifePage->sendToServerSocket( message );
			r = true;
		}
	}

	return r;
}

bool HetuwMod::livingLifePageMouseDown( float mX, float mY ) {
	//printf("hetuw mouse down %f, %f\n", mX, mY);
	if (bDrawHomeCords) {
		for (unsigned i=0; i<homePosStack.size(); i++) {
			if (mX >= homePosStack[i]->drawStartPos.x && mX <= homePosStack[i]->drawEndPos.x) {
				if (mY >= homePosStack[i]->drawStartPos.y && mY <= homePosStack[i]->drawEndPos.y) {
					if (isCommandKeyDown()) {
						homePosStack.erase(homePosStack.begin()+i);
					} else {
						cordOffset.x = -homePosStack[i]->x;
						cordOffset.y = -homePosStack[i]->y;
					}
					return true;
				}
			}
		}
	}
	for (int k=0; (unsigned)k < searchWordList.size(); k++) {
		if (mX >= searchWordStartPos[k]->x && mX <= searchWordEndPos[k]->x) {
			if (mY >= searchWordStartPos[k]->y && mY <= searchWordEndPos[k]->y) {
				searchWordListDelete[k] = true;
				return true;
			}
		}
	}
	if (isCommandKeyDown()) {
		int tileX = round(mX/CELL_D);
		int tileY = round(mY/CELL_D);
		moveToAndClickTile(tileX, tileY, !isLastMouseButtonRight());
		return true;
	}
	bMoveClick = false;
	return false;
}

void HetuwMod::moveToAndClickTile(int tileX, int tileY, bool alpha) {
	if (!ourLiveObject) return;
	int tileRX = tileX - ourLiveObject->xd;
	int tileRY = tileY - ourLiveObject->yd;
	if (tileRX <= 1 && tileRX >= -1) {
		if (tileRY <= 1 && tileRY >= -1) {
			if (tileRY == 0 || tileRX == 0) {
				if (alpha) {
					actionAlphaRelativeToMe(tileRX, tileRY);
				} else {
					actionBetaRelativeToMe(tileRX, tileRY);
				}
			return;
			}
		}
	}
	bMoveClickX = tileX;
	bMoveClickY = tileY;
	bMoveClickAlpha = alpha;
	bMoveClick = true;
	float clickX = tileX*CELL_D;
	float clickY = tileY*CELL_D;
	livingLifePage->hetuwClickMove(clickX, clickY);
}

//	move direction
//	---------------
//	1	2	3
//  8	0	4
//	7	6	5
//	---------------

void HetuwMod::setMoveDirection(int &x, int &y, int direction) {
	switch (direction) {
		case 1: x--; y++; break;
		case 2: y++; break;
		case 3: x++; y++; break;
		case 4: x++; break;
		case 5: x++; y--; break;
		case 6: y--; break;
		case 7: x--; y--; break;
		case 8: x--; break;
	}
}

int HetuwMod::getMoveDirection() {
	if (!upKeyDown && !leftKeyDown && !downKeyDown && !rightKeyDown) return 0;
	if (upKeyDown && leftKeyDown && !downKeyDown && !rightKeyDown) return 1;
	if (upKeyDown && !leftKeyDown && !downKeyDown && !rightKeyDown) return 2;
	if (upKeyDown && !leftKeyDown && !downKeyDown && rightKeyDown) return 3;
	if (!upKeyDown && !leftKeyDown && !downKeyDown && rightKeyDown) return 4;
	if (!upKeyDown && !leftKeyDown && downKeyDown && rightKeyDown) return 5;
	if (!upKeyDown && !leftKeyDown && downKeyDown && !rightKeyDown) return 6;
	if (!upKeyDown && leftKeyDown && downKeyDown && !rightKeyDown) return 7;
	if (!upKeyDown && leftKeyDown && !downKeyDown && !rightKeyDown) return 8;
	return 0;
}

int HetuwMod::getNextMoveDir(int direction, int add) {
	direction += add;
	while (direction < 1) direction += 8;
	while (direction > 8) direction -= 8;
	return direction;
}

bool HetuwMod::tileHasNoDangerousAnimals(int x, int y) {
	int objId = livingLifePage->hetuwGetObjId( x, y);
	if (objId <= 0) return true;
	if (ourLiveObject->holdingID > 0 && getObject(ourLiveObject->holdingID)->rideable) {
		if (ourLiveObject->holdingID == 2396 || // Running Crude Car
			ourLiveObject->holdingID == 4655 || // Deliver Truck - +slotsInvis driving
			ourLiveObject->holdingID == 4660 || // Red Sports Car $30 - driving +varNumeral
			ourLiveObject->holdingID == 4681 || // Blue Sports Car $30 - driving +varNumeral
			ourLiveObject->holdingID == 4690 || // Green Sports Car $30 - driving +varNumeral
			ourLiveObject->holdingID == 4699 || // Yellow Sports Car $30 - driving +varNumeral
			ourLiveObject->holdingID == 4708 || // Black Sports Car $30 - driving +varNumeral
			ourLiveObject->holdingID == 4719) { // White Sports Car $30 - driving +varNumeral
				return true; // no dangerous animals for cars
		}
		// check dangerous animals for horses
		if (objId == 764) return false; // Rattle Snake	
		if (objId == 1385) return false; // Attacking Rattle Snake
		if (objId == 631) return false; // Hungry Grizzly Bear
		if (objId == 628) return false; // Grizzly Bear
		if (objId == 645) return false; // Fed Grizzly Bear
		if (objId == 4762) return false; // Sleepy Grizzly Bear
	} else { // moving by walking / not riding
		if (objId < maxObjects && isDangerousAnimal[objId]) return false;
	}
	return true;
}

bool HetuwMod::tileHasClosedDoor(int x, int y) {
	int objId = livingLifePage->hetuwGetObjId( x, y);
	if (objId > 0) {
		for (int i = 0; i < closedDoorIDsLength; i++) {
			if (objId == closedDoorIDs[i]) return true;
		}
	}
	return false;
}

bool HetuwMod::tileIsSafeToWalk(int x, int y) {
	int objId = livingLifePage->hetuwGetObjId( x, y);
	if (objId > 0) {
		if (!tileHasNoDangerousAnimals(x, y)) return false;

		ObjectRecord* obj = getObject(objId);
		if (obj && obj->blocksWalking) {
			if (ourLiveObject->xd == x || ourLiveObject->yd == y)
				if (tileHasClosedDoor( x, y )) return true;
			return false;
		}
	}
	return true;
}

bool HetuwMod::dirIsSafeToWalk(int x, int y, int dir) {
	int tX, tY;

	tX = x; tY = y; setMoveDirection(tX, tY, dir);
	if (!tileIsSafeToWalk(tX, tY)) return false;

	if (dir % 2 == 0) return true; // is not a corner dir

	int nextDir = getNextMoveDir(dir, 1);
	tX = x; tY = y; setMoveDirection(tX, tY, nextDir);
	if (!tileHasNoDangerousAnimals(tX, tY)) return false;

	nextDir = getNextMoveDir(dir, -1);
	tX = x; tY = y; setMoveDirection(tX, tY, nextDir);
	if (!tileHasNoDangerousAnimals(tX, tY)) return false;

	return true;
}

bool HetuwMod::setMoveDirIfSafe(int &x, int &y, int dir) {
	if (!dirIsSafeToWalk(x, y, dir)) return false;
	setMoveDirection(x, y, dir);
	return true;
}

bool HetuwMod::findNextMove(int &x, int &y, int dir) {
	if (dir <= 0) return false;
	
	if (magnetMoveDir > 0) {
		if (magnetWrongMoveDir != dir || magnetMoveCount > 2) {
			magnetWrongMoveDir = -1;
			magnetMoveDir = -1;
		} else {
			if (setMoveDirIfSafe(x, y, magnetMoveDir)) {
				magnetWrongMoveDir = -1;
				magnetMoveDir = -1;
				return true;
			}
		}
	}

	if (setMoveDirIfSafe(x, y, dir)) return true;

	int nextMoveDir = getNextMoveDir(dir, 1);
	if (dirIsSafeToWalk(x, y, nextMoveDir)) {
		setMoveDirection(x, y, nextMoveDir);
		if (dir % 2 == 0) {
			magnetWrongMoveDir = dir;
			magnetMoveDir = getNextMoveDir(dir, -1);
			magnetMoveCount = 0;
		}
		return true;
	}
	nextMoveDir = getNextMoveDir(dir, -1);
	if (dirIsSafeToWalk(x, y, nextMoveDir)) {
		setMoveDirection(x, y, nextMoveDir);
		if (dir % 2 == 0) {
			magnetWrongMoveDir = dir;
			magnetMoveDir = getNextMoveDir(dir, 1);
			magnetMoveCount = 0;
		}
		return true;
	}

	return false;
}

void HetuwMod::move() {
	if (!upKeyDown && !leftKeyDown && !downKeyDown && !rightKeyDown) return;

	int x = round(ourLiveObject->currentPos.x);
	int y = round(ourLiveObject->currentPos.y);

	if (x == lastPosX && y == lastPosY && ourLiveObject->inMotion) return;

	int objId = livingLifePage->hetuwGetObjId(x, y);
	if (objId > 0 && getObject(objId)->blocksWalking && ourLiveObject->inMotion) return;

	int sX = x;
	int sY = y;

	int dir = getMoveDirection();
	if (dir <= 0) return;

	//debugRecPos2.x = x*CELL_D;
	//debugRecPos2.y = y*CELL_D;

	if (!findNextMove(x, y, dir)) return; // sets x and y

	lastPosX = sX;
	lastPosY = sY;

	if (waitForDoorToOpen && (lastDoorToOpenX != x || lastDoorToOpenY != y)) {
		waitForDoorToOpen = false;
	} else if (waitForDoorToOpen) {
		if (tileHasClosedDoor( lastDoorToOpenX, lastDoorToOpenY ))
			return;
		waitForDoorToOpen = false;
	} else if (tileHasClosedDoor( x, y )) {
		char msg[32];
		sprintf( msg, "USE %d %d#", livingLifePage->sendX(x), livingLifePage->sendY(y));
		livingLifePage->hetuwSetNextActionMessage( msg, x, y );
		waitForDoorToOpen = true;
		lastDoorToOpenX = (int)x;
		lastDoorToOpenY = (int)y;
		return;
	}

	x *= CELL_D;
	y *= CELL_D;

	bMoveClick = false;
	livingLifePage->hetuwClickMove(x, y);
	magnetMoveCount++;

	//debugRecPos.x = x;
	//debugRecPos.y = y;
}

void HetuwMod::removeLastName(char *newName, const char* name) {
	int k = 0;
	bool skip = false;
	for (int i=0; name[i] != 0; i++) {
		if (name[i] == ' ') {
			skip = !skip;
		}
		if (!skip) {
			newName[k] = name[i];
			k++;
		}
	}
	if (k < 0) newName[0] = 0;
	else newName[k] = 0;
}

void HetuwMod::getLastName(char* lastName, const char* name) {
	if (!name) {
		lastName[0] = 0;
		return;
	}
	int k = -1;
	for (int i=0; name[i] != 0; i++) {
		if (name[i] == ' ') {
			if (k >= 0) break;
			else {
				k = 0;
				continue;
			}
		}
		if (k >= 0) {
			lastName[k] = name[i];
			k++;
		}
	}
	if (k < 0) lastName[0] = 0;
	else lastName[k] = 0;
}

void HetuwMod::getLastNameColor(const char* lastName, float rgba[]) {
	if (!lastName) {
		rgba[0] = 1.0f; rgba[1] = 1.0f;
		rgba[2] = 1.0f; rgba[3] = 1.0f;
		return;
	}
	int num = 0;
	for (int i=0; lastName[i] != 0; i++) {
		num += (int)lastName[i];
	}
	rgba[0] = 0.15 + (num%100)/70.0f;
	rgba[1] = 0.15 + (num%182)/140.0f;
	rgba[2] = 0.15 + (num%77)/50.0f;
	rgba[3] = 1.0;
}

void HetuwMod::setLastNameColor( const char* lastName, float alpha ) {
	float rgba[4];
	getLastNameColor(lastName, rgba);
	setDrawColor(rgba[0], rgba[1], rgba[2], alpha);
}

bool HetuwMod::isWearingABackpack(LiveObject *obj) {
	if (!obj) return false;
	if (obj->clothing.backpack)
		if (obj->clothing.backpack->numSlots > 0) return true;
	return false;
}

bool HetuwMod::isWearingPantsWithPocket(LiveObject *obj) {
	if (!obj) return false;
	if (obj->clothing.bottom)
		if (obj->clothing.bottom->numSlots > 0) return true;
	return false;
}

bool HetuwMod::isWearingShirtWithPocket(LiveObject *obj) {
	if (!obj) return false;
	if (obj->clothing.tunic)
		if (obj->clothing.tunic->numSlots > 0) return true;
	return false;
}

bool HetuwMod::weAreWearingABackpack() {
	return isWearingABackpack(ourLiveObject);
}

bool HetuwMod::weAreWearingPantsWithPocket() {
	return isWearingPantsWithPocket(ourLiveObject);
}

bool HetuwMod::weAreWearingShirtWithPocket() {
	return isWearingShirtWithPocket(ourLiveObject);
}

void HetuwMod::getSkinColor(float *rgba, ObjectRecord *obj) {
	if (!obj) return;
	rgba[3] = 1.0f;
	for (int i=0; i < obj->numSprites; i++) {
		if (obj->sprites[i] == spriteBodyWhiteID) {
			rgba[0] = obj->spriteColor[i].r;
			rgba[1] = obj->spriteColor[i].g;
			rgba[2] = obj->spriteColor[i].b;
			return;
		}
	}
	for (int i=0; i<3; i++) rgba[i] = 1.0f;
}

std::string HetuwMod::getRaceName(ObjectRecord *obj) {
	float rgba[4];
	getSkinColor(rgba, obj);
	//printf("hetuw skinColor %f %f %f", rgba[0], rgba[1], rgba[2]);
	//printf(" %s\n", Phex::colorsToHex(rgba, 3).c_str());
	std::string hexColor = Phex::colorsToHex(rgba, 3);
	if (Phex::strEquals(hexColor, hexRaceColor_brown)) return "BROWN";
	if (Phex::strEquals(hexColor, hexRaceColor_ginger)) return "GINGER";
	if (Phex::strEquals(hexColor, hexRaceColor_white)) return "WHITE";
	if (Phex::strEquals(hexColor, hexRaceColor_black)) return "BLACK";
	return "UNKOWN";
}

void HetuwMod::updatePlayerToMap(LiveObject *o, bool deathMsg) {
	if (!o) return;
	int p = -1;
	for(unsigned k=0; k<playersInMap.size(); k++) {
		if (playersInMap[k]->id == o->id) {
			p = (int)k;
			break;
		}
	}
	time_t timeNow = time(NULL);
	if (p < 0 && deathMsg) return;
	if (p < 0) {
		p = playersInMap.size();
		PlayerInMap *pInMap = new PlayerInMap();
		pInMap->id = o->id;
		pInMap->lastTime = timeNow;
		pInMap->gender = getObject(o->displayID)->male ? 'M' : 'F';
		playersInMap.push_back(pInMap);	
	}
	if (!playersInMap[p]->name && o->name) {
		playersInMap[p]->name = new char[64];
		strcpy(playersInMap[p]->name, o->name);
		playersInMap[p]->lastName = new char[32];
		getLastName( playersInMap[p]->lastName, playersInMap[p]->name );
		if (playersInMap[p]->lastName[0] == 0) {
			delete[] playersInMap[p]->lastName;
			playersInMap[p]->lastName = NULL;
		}
		playersInMap[p]->lastTime = timeNow;
	}
	if (o->xd != hetuwFakeCoord || o->yd != hetuwFakeCoord) {
		playersInMap[p]->x = o->xd;
		playersInMap[p]->y = o->yd;
		playersInMap[p]->lastTime = timeNow;
	}
	playersInMap[p]->age = (int)livingLifePage->hetuwGetAge(o);
	playersInMap[p]->finalAgeSet = deathMsg ? true : o->finalAgeSet;
}

void HetuwMod::updateMap() {
	for(int i=0; i<gameObjects->size(); i++) {
		LiveObject *o = gameObjects->getElement( i );
		updatePlayerToMap(o);
	}
}

#define hetuwPlayersInRangeDistance 50
void HetuwMod::updatePlayersInRangePanel() {
	playersInRangeNum = 0;
	char lastName[32];

	for(int i=0; (unsigned)i<familiesInRange.size(); i++)
		familiesInRange[i]->reset();

	for(int i=0; i<gameObjects->size(); i++) {
		LiveObject *o = gameObjects->getElement( i );

		if ( o == ourLiveObject ) continue;
		
		if (iDrawPlayersInRangePanel == 1) {
			if ( o->outOfRange ) continue;

			int distX = o->xd - ourLiveObject->xd;
			if ( distX > hetuwPlayersInRangeDistance || distX < -hetuwPlayersInRangeDistance)
				continue;
			int distY = o->yd - ourLiveObject->yd;
			if ( distY > hetuwPlayersInRangeDistance || distY < -hetuwPlayersInRangeDistance)
				continue;
		}

		playersInRangeNum++;

		ObjectRecord *obj = getObject(o->displayID);
		int youngWoman = 0;
		if ( !obj->male )
			if ( livingLifePage->hetuwGetAge( o ) < 40 )
				youngWoman = 1;

		if (isRelated(o)) {
			familiesInRange[0]->count++;
			familiesInRange[0]->youngWomenCount += youngWoman;
			continue;
		}


		getLastName(lastName, o->name);
		if (lastName[0] != 0) {
			FamilyInRange *fam = NULL;
			for (int k=1; (unsigned)k < familiesInRange.size(); k++) {
				if (familiesInRange[k]->nameEqualsName(lastName)) {
					fam = familiesInRange[k];
					break;
				}
			}
			if (!fam) {
				fam = new FamilyInRange();
				fam->setName(lastName);
				fam->generation = o->lineage.size()+1;
				fam->raceName = getRaceName(obj); 
				familiesInRange.push_back(fam);
			}
			fam->count++;
			fam->youngWomenCount += youngWoman;
			continue;
		}
		// person without family name
		FamilyInRange *fam = NULL;
		for (int k=1; (unsigned)k < familiesInRange.size(); k++) {
			if (familiesInRange[k]->nameEqualsName("UNKNOWN")) {
				fam = familiesInRange[k];
				break;
			}
		}
		if (!fam) {
			fam = new FamilyInRange();
			fam->setName("UNKNOWN");
			familiesInRange.push_back(fam);
		}
		fam->count++;
		fam->youngWomenCount += youngWoman;
	}
}

void HetuwMod::onOurDeath() {
	
	if (!bWriteLogs) return;

	for(unsigned k=0; k<playersInMap.size(); k++) {
		if (playersInMap[k]->x == 999999) continue;

		string name = playersInMap[k]->name ? string(playersInMap[k]->name) : "unknownName";
		string data = to_string(playersInMap[k]->id) + hetuwLogSeperator + name + hetuwLogSeperator;
		LiveObject *p = livingLifePage->getLiveObject(playersInMap[k]->id);
		string age = "age:";
		if (p) age = age + to_string((int)livingLifePage->hetuwGetAge(p));
		else age = age + to_string(playersInMap[k]->age);
		data = data + playersInMap[k]->gender + hetuwLogSeperator + age + hetuwLogSeperator;
		data = data + "X:"+to_string(playersInMap[k]->x) + hetuwLogSeperator + "Y:"+to_string(playersInMap[k]->y) + hetuwLogSeperator;
		if (p) data = data + (p->finalAgeSet ? "DEAD" : "ALIVE");
		else data = data + (playersInMap[k]->finalAgeSet ? "DEAD" : "ALIVE");
		writeLineToLogs("player_map", data);
	}

	writeLineToLogs("my_death", HetuwMod::getTimeStamp());
}

#define hetuwDeathMessageRange 200
// 1341060 2464 0 0 0 0 798 0 0 0 -1 0.24 0 0 X X 50.67 60.00 2.81 2885;202;0;0;200;198,560,3101 0 0 -1 0 reason_killed_152
void HetuwMod::onPlayerUpdate( LiveObject* inO, const char* line ) {
	if ( inO == NULL ) return;
	if ( ourLiveObject == NULL ) return;

	bool isDeathMsg = ( strstr( line, "X X" ) != NULL );
	if ( !isDeathMsg ) return;

	//printf("hetuw %s\n", line);

	LiveObject *o = NULL;
	for(int i=0; i<gameObjects->size(); i++) {
		LiveObject *kO = gameObjects->getElement(i);
		if (inO->id == kO->id) {
			o = kO;
		}
	}
	if ( o == NULL ) return;
	
	updatePlayerToMap(o, true);

	if (o == ourLiveObject) onOurDeath();

	DeathMsg* deathMsg = new DeathMsg();
	deathMsg->description = new char[128];

	string strLine(line);
	string reasonKilled = "reason_killed_"; // reason_killed_
	size_t reasonKilledIndex = strLine.find(reasonKilled);
	if (reasonKilledIndex != string::npos) {
		deathMsg->deathReason = 2; // killer
		string sstr = strLine.substr(reasonKilledIndex + reasonKilled.length());
		string strKillerId;
		for (unsigned i = 0; i < sstr.length(); i++) {
			if (sstr[i] < '0' || sstr[i] > '9') break;
			strKillerId += sstr[i]; 
		}
		int killerObjId = stoi(strKillerId); // object id - like knife or grizzly bear
		if (killerObjId >= 0 && killerObjId < maxObjects && isDangerousAnimal[killerObjId]) {
			deathMsg->deathReason = 1; // animal
		}
		ObjectRecord *ko = getObject( killerObjId );
		if ( ko && ko->description ) {
			char capitalDesc[128];
			int k = 0;
			for ( ; ko->description[k] != 0 && k < 128; k++) {
				if (ko->description[k] == '#') break;
				capitalDesc[k] = toupper(ko->description[k]);
			}
			capitalDesc[k] = 0;
			sprintf( deathMsg->description, "KILLED BY %s", capitalDesc );
		} else {
			sprintf( deathMsg->description, "KILLED BY ID %i", killerObjId );
		}
	} else if ( strLine.find("reason_hunger") != string::npos ) {
		sprintf( deathMsg->description, "KILLED BY STARVATION" );
	} else if ( strLine.find("reason_SID") != string::npos ) {
		sprintf( deathMsg->description, "SUDDEN INFANT DEATH" );
	} else if ( strLine.find("reason_age") != string::npos ) {
		sprintf( deathMsg->description, "KILLED BY OLD AGE" );
	} else {
		sprintf( deathMsg->description, "KILLED BY UNKNOWN" );
	}

	deathMsg->age = (int)livingLifePage->hetuwGetAge( o );
	if ( getObject( o->displayID ) )
		deathMsg->male = getObject( o->displayID )->male;

	string victimName = to_string(o->id);
	if (o->name) victimName = victimName + " " + o->name;
	string victimGender = deathMsg->male ? "M" : "F";
	writeLineToLogs("death", victimName + hetuwLogSeperator +  victimGender + hetuwLogSeperator + "age:"+to_string(deathMsg->age) + hetuwLogSeperator + deathMsg->description);

	if ( !o->name ) {
		delete deathMsg;
		return;
	}
	
	int diffX = o->xd - ourLiveObject->xd;
	int diffY = o->yd - ourLiveObject->yd;
	if ( diffX > hetuwDeathMessageRange || diffX < -hetuwDeathMessageRange) { delete deathMsg; return; }
	if ( diffY > hetuwDeathMessageRange || diffY < -hetuwDeathMessageRange) { delete deathMsg; return; }

	deathMsg->name = new char[64];
	sprintf( deathMsg->name, "%s", o->name); 

	deathMsg->timeReci = time(NULL);

	getRelationNameColor( o->relationName, deathMsg->nameColor );

	deathMessages.push_back(deathMsg);
}

void HetuwMod::onNameUpdate(LiveObject* o) {
	if (!o || !o->name) return;
	HetuwMod::writeLineToLogs("name", to_string(o->id) + hetuwLogSeperator + string(o->name));

	if (ourLiveObject && ourLiveObject->id == o->id) {
		if (strstr(o->name, "EVE SLINKER") != NULL) sendEmote("/BLUSH");
		else if (strstr(o->name, "EVE SLINKMAN") != NULL) sendEmote("/BLUSH");
		else if (strstr(o->name, "EVE SLINKEY") != NULL) sendEmote("/BLUSH");
		else if (strstr(o->name, "EVE SLINKY") != NULL) sendEmote("/BLUSH");
		else if (strstr(o->name, "EVE GAYLORD") != NULL) sendEmote("/HMPH");
		else if (strstr(o->name, "EVE YIKE") != NULL) sendEmote("/HMPH");
		else if (strstr(o->name, "EVE ZIV") != NULL) sendEmote("/DEVIOUS");
		else if (strstr(o->name, "EVE KILL") != NULL) sendEmote("/DEVIOUS");
		else if (strstr(o->name, "EVE TARR") != NULL) sendEmote("/JOY");
		else if (strstr(o->name, "EVE BOOB") != NULL) sendEmote("/SHOCK");
		else if (strstr(o->name, "EVE GAY") != NULL) sendEmote("/HAPPY");
		else if (strstr(o->name, "EVE GRIM") != NULL) sendEmote("/HUBBA");
		else if (strstr(o->name, "EVE ROHRER") != NULL) sendEmote("/HUBBA");
		else if (strstr(o->name, "EVE DEATH") != NULL) sendEmote("/LOVE");
		else if (strstr(o->name, "EVE METH") != NULL) sendEmote("/LOVE");
		else if (strstr(o->name, "EVE UNO") != NULL) sendEmote("/LOVE");
	}
}

void HetuwMod::onCurseUpdate(LiveObject* o) {
	string type = "forgive";
	if ( o->curseLevel ) {
		type = "curse";
	}
	string data = to_string(o->id);
	if ( o->name ) {
        data += " " + string(o->name);
	}
	if ( o->curseName ) {
    	data += hetuwLogSeperator + string(o->curseName);
	}
	HetuwMod::writeLineToLogs(type, data);
}

void HetuwMod::drawDeathMessages() {
	if ( deathMessages.size() <= 0 ) return;

	DeathMsg* dm = deathMessages[0];

	doublePair drawPos = lastScreenViewCenter;
	drawPos.y += viewHeight/2;
	drawPos.y -= 20*guiScale;
	
	char gender[8];
	sprintf( gender, "%c ", dm->male ? 'M' : 'F');
	char age[8];
	sprintf( age, "%d ", dm->age);

	double nameWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( dm->name, guiScale );
	double ripWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( "RIP ", guiScale );
	double genderWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( gender, guiScale );
	double ageWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( age, guiScale );
	double textWidth = nameWidth + ripWidth + genderWidth + ageWidth;

	doublePair recDrawPos = drawPos;

	setDrawColor( 0, 0, 0, 0.8 );
	drawRect( recDrawPos, (textWidth)/2 + 10*guiScale, 20*guiScale );

	drawPos.x -= textWidth/2;

	if ( dm->deathReason == 1 ) setDrawColor( 1, 0.8, 0, 1 ); // animal
	else if ( dm->deathReason == 2 ) setDrawColor( 1, 0.2, 0, 1 ); // killer
	else setDrawColor( 1, 1, 1, 1 );
	livingLifePage->hetuwDrawScaledHandwritingFont( "RIP " , drawPos, guiScale );
	drawPos.x += ripWidth;

	if ( dm->male ) setDrawColor( 0.2, 0.6, 1.0, 1 );
	else setDrawColor( 1, 0.4, 0.8, 1 );
	livingLifePage->hetuwDrawScaledHandwritingFont( gender , drawPos, guiScale );
	drawPos.x += genderWidth;

	setDrawColor( 1, 1, 1, 1 );
	livingLifePage->hetuwDrawScaledHandwritingFont( age , drawPos, guiScale );
	drawPos.x += ageWidth;

	setDrawColor( dm->nameColor[0], dm->nameColor[1], dm->nameColor[2], 1 );
	livingLifePage->hetuwDrawScaledHandwritingFont( dm->name , drawPos, guiScale );

	int mouseX, mouseY;
	livingLifePage->hetuwGetMouseXY( mouseX, mouseY );

	float recStartX = recDrawPos.x - textWidth/2 - 10*guiScale;
	float recEndX = recDrawPos.x + textWidth/2 + 10*guiScale;
	float recStartY = recDrawPos.y - 20*guiScale;
	float recEndY = recDrawPos.y + 20*guiScale;

	if (mouseX >= recStartX && mouseX <= recEndX) {
		if (mouseY >= recStartY && mouseY <= recEndY) {
			doublePair descDrawPos = { (double)mouseX, (double)mouseY };
			if ( dm->description )
				drawTextWithBckgr( descDrawPos, dm->description );
			else
				drawTextWithBckgr( descDrawPos, "UNKNOWN DEATH" );
		}
	}

	if ( dm->timeReci+15 < time(NULL) ) {
		delete deathMessages[0];
		deathMessages.erase( deathMessages.begin() );
		if ( deathMessages.size() > 0 )
			deathMessages[0]->timeReci = time(NULL);
	}
}

void HetuwMod::drawMap() {
	doublePair drawPos;
	doublePair screenCenter = lastScreenViewCenter;
	int mouseX, mouseY;
	livingLifePage->hetuwGetMouseXY( mouseX, mouseY );

	setDrawColor( 0, 0, 0, 0.2 );
	drawRect( screenCenter, viewWidth/2, viewHeight/2 );
	setDrawColor( 1, 1, 1, 1 );

	char names[32][32];
	int namesCount = 0;
	double minX = screenCenter.x - viewWidth/2;
	double minY = screenCenter.y - viewHeight/2;
	double maxX = screenCenter.x + viewWidth/2;
	double maxY = screenCenter.y + viewHeight/2;
	char drawMouseOver[128];
	bool bDrawMouseOver = false;
	int recWidthHalf = 10*zoomScale;
	int recHeightHalf = 10*zoomScale;
	for(unsigned k=0; k<playersInMap.size(); k++) {
		if (playersInMap[k]->x == 999999) continue;
		if (!playersInMap[k]->name) continue;
		drawPos.x = (playersInMap[k]->x - ourLiveObject->xd) / mapScale;
		drawPos.y = (playersInMap[k]->y - ourLiveObject->yd) / mapScale;
		drawPos.x += mapOffsetX;
		drawPos.y += mapOffsetY;
		drawPos.x *= viewHeight;
		drawPos.y *= viewHeight;
		drawPos.x += screenCenter.x;
		drawPos.y += screenCenter.y;
		if (drawPos.x < minX || drawPos.x > maxX || drawPos.y < minY || drawPos.y > maxY)
			continue;
		if (drawPos.x > mouseX-recWidthHalf && drawPos.x < mouseX+recWidthHalf &&
			drawPos.y > mouseY-recHeightHalf && drawPos.y < mouseY+recHeightHalf) {
			bDrawMouseOver = true;
			if (playersInMap[k]->lastName) {
				sprintf(drawMouseOver, "%s X:%d Y:%d", playersInMap[k]->name, playersInMap[k]->x, playersInMap[k]->y);
			} else {
				sprintf(drawMouseOver, "X:%d Y:%d", playersInMap[k]->x, playersInMap[k]->y);
			}
		}
		
		if (playersInMap[k]->lastName) {
			bool nameFound = false;
			for (int i=0; i<namesCount; i++) {
				if (strcmp(names[i], playersInMap[k]->lastName) == 0) {
					nameFound = true;
					break;
				}
			}
			if (!nameFound) {
				strcpy( names[namesCount], playersInMap[k]->lastName );
				namesCount++;
			}
		}
		
		float alpha = 1.0f;
		if (playersInMap[k]->finalAgeSet) alpha = 0.4f;

		if (ourLiveObject->id == playersInMap[k]->id) 
			setDrawColor( colorRainbow->color[0], 1.0f, colorRainbow->color[2], 1 );
		else setLastNameColor( playersInMap[k]->lastName, alpha );

		drawRect( drawPos, recWidthHalf, recHeightHalf );
	}

	setDrawColorToCoordType(hpt_bell);
	double bellCrossWidth = recWidthHalf*0.3;
	for (unsigned i=0; i<homePosStack.size(); i++) {
		if (homePosStack[i]->type != hpt_bell) continue;
		drawPos.x = (homePosStack[i]->x - ourLiveObject->xd) / mapScale;
		drawPos.y = (homePosStack[i]->y - ourLiveObject->yd) / mapScale;
		drawPos.x += mapOffsetX;
		drawPos.y += mapOffsetY;
		drawPos.x *= viewHeight;
		drawPos.y *= viewHeight;
		drawPos.x += screenCenter.x;
		drawPos.y += screenCenter.y;
		if (drawPos.x < minX || drawPos.x > maxX || drawPos.y < minY || drawPos.y > maxY)
			continue;
		drawRect( drawPos, bellCrossWidth, recHeightHalf );
		drawRect( drawPos, recHeightHalf, bellCrossWidth );
	}

	setDrawColor( 0, 0, 0, 0.8 );

	char strZoomKeys[64];
	sprintf( strZoomKeys, "USE %c/%c TO ZOOM IN/OUT - CLOSE IT WITH SHIFT+%c", toupper(charKey_MapZoomIn), toupper(charKey_MapZoomOut), toupper(charKey_ShowMap)); 
	float strZoomKeysWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( strZoomKeys, guiScale ); 
	doublePair drawKeysRecPos;
	drawKeysRecPos.x = screenCenter.x - viewWidth/2;
	drawKeysRecPos.y = screenCenter.y - viewHeight/2;
	drawKeysRecPos.x += strZoomKeysWidth/2 + 10*guiScale;
	drawKeysRecPos.y += 80;
	drawRect( drawKeysRecPos, strZoomKeysWidth/2+10*guiScale, 15*guiScale);  

	doublePair drawNameRecPos;
	drawNameRecPos.x = screenCenter.x - viewWidth/2 + 50*guiScale;
	drawNameRecPos.y = drawKeysRecPos.y + 15*guiScale;
	float drawNameRecWidth = 100*guiScale;
	float drawNameRecHeight = namesCount*15*guiScale + 10*guiScale;
	drawNameRecPos.y += drawNameRecHeight;
	drawRect( drawNameRecPos, drawNameRecWidth, drawNameRecHeight );

	doublePair drawNamesPos;
	drawNamesPos.x = screenCenter.x - viewWidth/2;
	drawNamesPos.y = drawKeysRecPos.y + 40*guiScale;
	drawNamesPos.x += 20*guiScale;
	for (int i=0; i<namesCount; i++) {
		setLastNameColor( names[i] , 1.0f );
		livingLifePage->hetuwDrawScaledHandwritingFont( names[i], drawNamesPos, guiScale );
		drawNamesPos.y += 30*guiScale;
	}

	setDrawColor( 1, 1, 1, 1 );
	livingLifePage->hetuwDrawScaledHandwritingFont( strZoomKeys, drawKeysRecPos, guiScale, alignCenter );

	if (bDrawMouseOver) {
		doublePair drawMouseOverPos;
		float textWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( drawMouseOver, guiScale ); 
		drawMouseOverPos.x = mouseX - textWidth/2;
		drawMouseOverPos.y = mouseY + 20*guiScale;
		setDrawColor( 0, 0, 0, 0.5 );
		doublePair bckgrRecPos;
		bckgrRecPos.x = mouseX;
		bckgrRecPos.y = mouseY + 20*guiScale;
		drawRect( bckgrRecPos, textWidth/2 + 10*guiScale, 15*guiScale );
		setDrawColor( 1, 1, 1, 1 );
		livingLifePage->hetuwDrawScaledHandwritingFont( drawMouseOver, drawMouseOverPos, guiScale );
	}
}

void HetuwMod::drawPlayersInRangePanel() {
	int listSize = 0;
	for (int k=0; (unsigned)k < familiesInRange.size(); k++) {
		if (k != 0 && familiesInRange[k]->count <= 0) continue;
		listSize++;
	}

	setDrawColor( 0, 0, 0, 0.8 );
	doublePair bckgrRecPos = lastScreenViewCenter;
	int bckgrRecWidthHalf = 140*guiScale;
	int bckgrRecHeightHalf = (int)(10 + listSize*12.5f + 12.5f)*guiScale;
	bckgrRecPos.x += viewWidth/2;
	bckgrRecPos.y += viewHeight/2;
	doublePair textPos = bckgrRecPos;
	bckgrRecPos.x -= bckgrRecWidthHalf;
	bckgrRecPos.y -= bckgrRecHeightHalf;
	drawRect( bckgrRecPos, bckgrRecWidthHalf, bckgrRecHeightHalf );

	drawSearchListTopY = bckgrRecPos.y - bckgrRecHeightHalf - (int)(10*guiScale);

	setDrawColor( 1, 1, 1, 1 );
	char text[32];
	textPos.y -= 20*guiScale;
	textPos.x -= 20*guiScale;
	
	if (iDrawPlayersInRangePanel == 1) {
		if (playersInRangeNum < 10) sprintf(text, "PLAYERS IN RANGE:   %d", playersInRangeNum);
		else if (playersInRangeNum < 100) sprintf(text, "PLAYERS IN RANGE:  %d", playersInRangeNum);
		else sprintf(text, "PLAYERS IN RANGE: %d", playersInRangeNum);
	} else {
		if (playersInRangeNum < 10) sprintf(text, "PLAYERS ON SERVER:   %d", playersInRangeNum);
		else if (playersInRangeNum < 100) sprintf(text, "PLAYERS ON SERVER:  %d", playersInRangeNum);
		else sprintf(text, "PLAYERS ON SERVER: %d", playersInRangeNum);
	}
	livingLifePage->hetuwDrawScaledHandwritingFont( text, textPos, guiScale, alignRight );

	int mouseX, mouseY;
	livingLifePage->hetuwGetMouseXY( mouseX, mouseY );
	float recStartX = bckgrRecPos.x - bckgrRecWidthHalf;
	float recEndX = bckgrRecPos.x + bckgrRecWidthHalf;
	float recStartY, recEndY;

	float lineHeight = 25*guiScale;

	doublePair drawPos = { textPos.x, textPos.y };
	for (int k=0; (unsigned)k < familiesInRange.size(); k++) {
		FamilyInRange* fam = familiesInRange[k];
		if (k != 0 && fam->count <= 0) continue;
		textPos.y -= lineHeight;
		setLastNameColor(fam->name, 1.0f);
		char text[32];
		sprintf( text, "%s  F:%i  %i", fam->name, fam->youngWomenCount, fam->count);
		livingLifePage->hetuwDrawScaledHandwritingFont( text, textPos, guiScale, alignRight );
	}
	for (int k=0; (unsigned)k < familiesInRange.size(); k++) {
		FamilyInRange* fam = familiesInRange[k];
		if (k != 0 && fam->count <= 0) continue;
		drawPos.y -= lineHeight;

		recStartY = drawPos.y - lineHeight/2;
		recEndY = drawPos.y + lineHeight/2;
		if (mouseX >= recStartX && mouseX <= recEndX) {
			if (mouseY >= recStartY && mouseY <= recEndY) {
				doublePair descDrawPos = { (double)mouseX, (double)mouseY };
				sprintf( text, "%s GEN:%i", fam->raceName.c_str(), fam->generation);
				float rgba[4];
				getLastNameColor(fam->name, rgba);
				drawTextWithBckgr(descDrawPos, text, rgba);
			}
		}
	}

}

void HetuwMod::drawSearchList() {
	int mouseX, mouseY;
	livingLifePage->hetuwGetMouseXY( mouseX, mouseY );

	for (unsigned i=0; i<searchWordList.size(); i++) {
		if (searchWordListDelete[i]) {
			//printf("hetuw searchWord delete %i. %s\n", i, searchWordList[i]);
			delete[] searchWordList[i];
			searchWordList.erase(searchWordList.begin()+i);
			delete searchWordStartPos[i];
			searchWordStartPos.erase(searchWordStartPos.begin()+i);
			delete searchWordEndPos[i];
			searchWordEndPos.erase(searchWordEndPos.begin()+i);
			searchWordListDelete.erase(searchWordListDelete.begin()+i);
			i--;
			setSearchArray();
		}
	}
	if (searchWordList.size() == 0) {
		resetObjectDrawScale();
		return;
	}

	float biggestTextWidth = 0;
	for (unsigned i=0; i<searchWordList.size(); i++) {
		float textWidth = livingLifePage->hetuwMeasureScaledHandwritingFont( searchWordList[i], guiScale );
		if (textWidth > biggestTextWidth) biggestTextWidth = textWidth;
	}

	int bckgrRecWidthHalf = (int)(10)*guiScale+(biggestTextWidth/2);
	int bckgrRecHeightHalf = (int)(10 + searchWordList.size()*12.5f)*guiScale;

	doublePair bckgrRecPos = lastScreenViewCenter;
	if (iDrawPlayersInRangePanel > 0) {
		bckgrRecPos.y = drawSearchListTopY;
	} else {
		bckgrRecPos.y += viewHeight/2;
	}
	bckgrRecPos.x += viewWidth/2;
	doublePair textPos = bckgrRecPos;
	bckgrRecPos.x -= bckgrRecWidthHalf;
	bckgrRecPos.y -= bckgrRecHeightHalf;

	setDrawColor( 0, 0, 0, 0.8 );
	drawRect( bckgrRecPos, bckgrRecWidthHalf, bckgrRecHeightHalf );

	textPos.x -= 10*guiScale;
	textPos.y += 2*guiScale;
	setDrawColor( 0.3, 1.0, 0, 1.0 );
	for (int k=0; (unsigned)k < searchWordList.size(); k++) {
		textPos.y -= 25*guiScale;
		livingLifePage->hetuwDrawScaledHandwritingFont( searchWordList[k], textPos, guiScale, alignRight );

		searchWordStartPos[k]->x = bckgrRecPos.x-bckgrRecWidthHalf-0*guiScale;
		searchWordEndPos[k]->x = bckgrRecPos.x+bckgrRecWidthHalf+0*guiScale;
		searchWordEndPos[k]->y = textPos.y+12.5*guiScale;
		searchWordStartPos[k]->y = textPos.y-12.5*guiScale;
	}
	for (int k=0; (unsigned)k < searchWordList.size(); k++) {
		if (mouseX >= searchWordStartPos[k]->x && mouseX <= searchWordEndPos[k]->x) {
			if (mouseY >= searchWordStartPos[k]->y && mouseY <= searchWordEndPos[k]->y) {
				setDrawColor( 1, 1, 1, 0.4 );
				hDrawRect( *searchWordStartPos[k], *searchWordEndPos[k] );
			}
		}
	}
}

void HetuwMod::drawAge() {
	setDrawColor( 0, 0, 0, 1 );
	doublePair drawPos;
	char sBuf[32];
	int age = (int)(ourAge*10);
	int ageDecimal = age - int(age*0.1)*10;
	age = (int)((age-ageDecimal)*0.1);
	sprintf(sBuf, "%c  %i.%i", ourGender, age, ageDecimal);
	drawPos = lastScreenViewCenter;
	drawPos.x += 290;
	drawPos.y -= viewHeight/2 - 25;
	livingLifePage->hetuwDrawWithHandwritingFont( sBuf, drawPos );
}

void HetuwMod::drawCords() {
	int x = round(ourLiveObject->currentPos.x+cordOffset.x);
	int y = round(ourLiveObject->currentPos.y+cordOffset.y);

	char sBufA[16];
	sprintf(sBufA, "%d", x );
	float textWidthA = livingLifePage->hetuwMeasureScaledHandwritingFont( sBufA, guiScale );
	char sBufB[16];
	sprintf(sBufB, "%d", y );
	float textWidthB = livingLifePage->hetuwMeasureScaledHandwritingFont( sBufB, guiScale );

	doublePair drawPosA = lastScreenViewCenter;
	doublePair drawPosB;
	drawPosA.x -= HetuwMod::viewWidth/2 - (20*guiScale);
	drawPosA.y += HetuwMod::viewHeight/2 - (40*guiScale);
	drawPosB.x = drawPosA.x + (20*guiScale) + textWidthA;
	drawPosB.y = drawPosA.y;

	doublePair drawPosC = drawPosA;
	drawPosC.x += textWidthA/2;
	doublePair drawPosD = drawPosB;
	drawPosD.x += textWidthB/2;
	setDrawColor( 0, 0, 0, 1 );
	drawRect( drawPosC, textWidthA/2 + 6*guiScale, 16*guiScale );
	drawRect( drawPosD, textWidthB/2 + 6*guiScale, 16*guiScale );

	if (x < 0) setDrawColor( 1, 0.8, 0, 1 );
	else setDrawColor( 0.2, 1, 0.2, 1 );
	livingLifePage->hetuwDrawScaledHandwritingFont( sBufA, drawPosA, guiScale );
	if (y < 0) setDrawColor( 1, 0.8, 0, 1 );
	else setDrawColor( 0.2, 1, 0.2, 1 );
	livingLifePage->hetuwDrawScaledHandwritingFont( sBufB, drawPosB, guiScale );
}

void HetuwMod::drawGrid() {
	setDrawColor( 0.0, 0.0, 0.0, 1.0 );
	doublePair drawPos = lastScreenViewCenter;
	float recWidth = 1*guiScale;
	float startX = drawPos.x - HetuwMod::viewWidth/2.0;
	float endX = drawPos.x + HetuwMod::viewWidth/2.0;
	float startY = drawPos.y - HetuwMod::viewHeight/2.0;
	float endY = drawPos.y + HetuwMod::viewHeight/2.0;
	int offsetX = fmod(startX, CELL_D)+(CELL_D/2.0);
	int offsetY = fmod(startY, CELL_D)+(CELL_D/2.0);
	drawPos.x = startX;
	drawPos.y = startY;
	for (float x = startX-offsetX; x < endX; x += CELL_D) {
		drawPos.x = x;
		drawRect( drawPos, recWidth, HetuwMod::viewHeight );
	}
	drawPos.x = startX;
	for (float y = startY-offsetY; y < endY; y += CELL_D) {
		drawPos.y = y;
		drawRect( drawPos, HetuwMod::viewWidth, recWidth );
	}
}

void HetuwMod::SetFixCamera(bool b) {
	cameraIsFixed = !b;
}

void HetuwMod::processArcReport(const char* data, string error) {
	if (error.length() > 0) {
		printf("hetuw processArcReport error: %s\n", error.c_str());
		return;
	}
	int arcTime;
	int scanCount = sscanf(data, "Current player arc has been going %d year", &arcTime);
	if (scanCount != 1) {
		printf("hetuw Warning: Could not get arc time from string\n");
		printf("hetuw string: %s\n", data);
		return;
	}
	arcRunningSince = time(NULL) - (arcTime*60);
}

string HetuwMod::getArcTimeStr() {
	if (arcRunningSince < 0) return "";
	int timeDiff = (int)(time(NULL) - arcRunningSince);
	int days = timeDiff/60/60/24;
	int hours = (timeDiff/60/60) - (days*24);
	int minutes = timeDiff % 60;
	if (days > 0) {
		return to_string(days)+" DAYS "+to_string(hours)+" HOURS ";
	} else {
		if (hours > 0) {
			return to_string(hours)+" HOURS "+to_string(minutes)+" MINUTES";
		} else return to_string(minutes)+" MINUTEs";
	}
}

void HetuwMod::setHelpColorNormal() {
	setDrawColor( 1.0f, 1.0f, 1.0f, 1 );
}

void HetuwMod::setHelpColorSpecial() {
	setDrawColor( colorRainbow->color[0], 0.5f, colorRainbow->color[2], 1 );
}

void HetuwMod::drawHelp() {
	float guiScale = (guiScaleRaw+0.1) * zoomScale;
	char str[128];
	setDrawColor( 0, 0, 0, 0.8 );
	drawRect( lastScreenViewCenter, viewWidth/2, viewHeight/2 );

	setHelpColorNormal();

	double lineHeight = 30*guiScale;

	doublePair drawPos = lastScreenViewCenter;
	drawPos.x -= viewWidth/2 - 20*guiScale;
	drawPos.y += viewHeight/2 - 30*guiScale;
	char serverIPupperCase[128];
	strToUpper(serverIP, serverIPupperCase, 128);
	sprintf(str, "%s:%d", serverIPupperCase, serverPort);
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );

	// emotion words
	drawPos = lastScreenViewCenter;
	drawPos.x -= viewWidth/2 - 20*guiScale;
	drawPos.y += viewHeight/2 - 80*guiScale;
	SimpleVector<Emotion> emotions = hetuwGetEmotions();
    for( int i=0; i<emotions.size()-1; i++ ) {
		if (i == 7 || i == 8) continue;
		int id = i;
		if (i > 6) id -= 2;

		if (strstr(emotions.getElement(i)->triggerWord, "/") == NULL) continue;

		if (id < 10) sprintf(str, " %i: %s", id, emotions.getElement(i)->triggerWord);
		else sprintf(str, "F%i: %s", id-9, emotions.getElement(i)->triggerWord);

		livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
		drawPos.y -= lineHeight;
	}
	drawPos.y -= lineHeight;
	livingLifePage->hetuwDrawScaledHandwritingFont( "PRESS NUMBER KEY FOR SHORT EMOTE", drawPos, guiScale );
	drawPos.y -= lineHeight;
	livingLifePage->hetuwDrawScaledHandwritingFont( "WRITE EMOTE FOR PERMANENT EMOTE", drawPos, guiScale );
	drawPos.y -= lineHeight;

	drawPos.y -= lineHeight;
	drawPos.y -= lineHeight;
	drawPos.y -= lineHeight;
	drawPos.y -= lineHeight;
	drawPos.y -= lineHeight;
	sprintf(str, "YOU CAN CHANGE KEYS AND SETTINGS BY MODIFYING THE HETUW.CFG FILE");
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	drawPos = lastScreenViewCenter;
	drawPos.x -= viewWidth/2 - 250*guiScale;
	drawPos.y += viewHeight/2 - 80*guiScale;

	livingLifePage->hetuwDrawScaledHandwritingFont( "= MAKE SCREENSHOT", drawPos, guiScale );
	drawPos.y -= lineHeight;

	setHelpColorSpecial();
	sprintf(str, "%c TOGGLE SHOW HELP", toupper(charKey_ShowHelp));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	if (cameraIsFixed) setHelpColorSpecial();
	else setHelpColorNormal();
	sprintf(str, "%c TOGGLE FIX CAMERA", toupper(charKey_FixCamera));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	if (iDrawNames > 0) setHelpColorSpecial();
	else setHelpColorNormal();
	sprintf(str, "%c TOGGLE SHOW NAMES", toupper(charKey_ShowNames));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	if (bDrawCords) setHelpColorSpecial();
	else setHelpColorNormal();
	sprintf(str, "%c TOGGLE SHOW CORDS", toupper(charKey_ShowCords));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	if (iDrawPlayersInRangePanel > 0) setHelpColorSpecial();
	else setHelpColorNormal();
	sprintf(str, "%c TOGGLE SHOW PLAYERS IN RANGE", toupper(charKey_ShowPlayersInRange));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	//if (bDrawDeathMessages) setHelpColorSpecial();
	//else setHelpColorNormal();
	//sprintf(str, "%c TOGGLE SHOW DEATH MESSAGES", toupper(charKey_ShowDeathMessages));
	//livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	//drawPos.y -= lineHeight;

	if (bDrawHomeCords) setHelpColorSpecial();
	else setHelpColorNormal();
	sprintf(str, "%c TOGGLE SHOW HOME CORDS", toupper(charKey_ShowHomeCords));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	if (bDrawHostileTiles) setHelpColorSpecial();
	else setHelpColorNormal();
	sprintf(str, "%c TOGGLE SHOW HOSTILE TILES", toupper(charKey_ShowHostileTiles));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	if (bxRay) setHelpColorSpecial();
	else setHelpColorNormal();
	sprintf(str, "%c X-RAY VISION", toupper(charKey_xRay));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	if (bDrawYum) setHelpColorSpecial();
	else setHelpColorNormal();
	sprintf(str, "%c FIND YUM", toupper(charKey_FindYum));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	if (bDrawGrid) setHelpColorSpecial();
	else setHelpColorNormal();
	sprintf(str, "%c SHOW GRID", toupper(charKey_ShowGrid));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	setHelpColorNormal();

	sprintf(str, "%c - USE SHORTS POCKET", toupper(charKey_Pocket));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	sprintf(str, "SHIFT+%c - USE APRON POCKET", toupper(charKey_Pocket));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	drawPos = lastScreenViewCenter;
	drawPos.x -= viewWidth/2 - 640*guiScale;
	drawPos.y += viewHeight/2 - 80*guiScale;

	sprintf(str, "%c - USE BACKPACK", toupper(charKey_Backpack));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	sprintf(str, "SHIFT+%c - USE BACKPACK", toupper(charKey_Backpack));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	sprintf(str, "%c - TAKE OFF BACKPACK", toupper(charKey_TakeOffBackpack));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	sprintf(str, "%c - EAT / PUT CLOTHES ON", toupper(charKey_Eat));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	sprintf(str, "%c - PICK UP / DROP BABY", toupper(charKey_Baby));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	sprintf(str, "%c%c%c%c - MOVE", toupper(charKey_Up), toupper(charKey_Left), toupper(charKey_Down), toupper(charKey_Right));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	sprintf(str, "SHIFT+%c%c%c%c - USE/PICK UP ITEM", toupper(charKey_Up), toupper(charKey_Left), toupper(charKey_Down), toupper(charKey_Right));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	sprintf(str, "CTRL+%c%c%c%c - DROP / PICK ITEM FROM CONTAINER", toupper(charKey_Up), toupper(charKey_Left), toupper(charKey_Down), toupper(charKey_Right));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	sprintf(str, "ALT+%c%c%c%c - SWAP ITEM (WITH CONTAINER)", toupper(charKey_Up), toupper(charKey_Left), toupper(charKey_Down), toupper(charKey_Right));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	if (charKey_TileStandingOn == ' ') sprintf(str, "SPACE - USE/PICK UP ITEM ON THE TILE YOU ARE STANDING ON");
	else sprintf(str, "%c - USE/PICK UP ITEM ON THE TILE YOU ARE STANDING ON", toupper(charKey_TileStandingOn));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	if (charKey_TileStandingOn == ' ') sprintf(str, "CTRL+SPACE - DROP / PICK ITEM FROM CONTAINER");
	else sprintf(str, "CTRL+%c - DROP / PICK ITEM FROM CONTAINER", toupper(charKey_TileStandingOn));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	if (charKey_TileStandingOn == ' ') sprintf(str, "ALT+SPACE - SWAP ITEM (WITH CONTAINER)");
	else sprintf(str, "ALT+%c - SWAP ITEM (WITH CONTAINER)", toupper(charKey_TileStandingOn));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	livingLifePage->hetuwDrawScaledHandwritingFont( "LEFTARROWKEY ZOOM IN", drawPos, guiScale );
	drawPos.y -= lineHeight;
	livingLifePage->hetuwDrawScaledHandwritingFont( "RIGHTARROWKEY ZOOM OUT", drawPos, guiScale );
	drawPos.y -= lineHeight;
	livingLifePage->hetuwDrawScaledHandwritingFont( "CTRL+ARROWKEYS SCALE GUI", drawPos, guiScale );
	drawPos.y -= lineHeight;
	sprintf(str, "%c THEN KEY - REMEMBER CORDS", toupper(charKey_CreateHome));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	sprintf(str, "SHIFT+%c THEN KEY - REMEMBER CUSTOM CORDS", toupper(charKey_CreateHome));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	sprintf(str, "SHIFT+%c - RESET CORDS TO WHERE YOU ARE STANDING", toupper(charKey_ShowCords));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	if (searchWordList.size() > 0) setHelpColorSpecial();
	else setHelpColorNormal();
	sprintf(str, "%c - SEARCH FOR AN OBJECT", toupper(charKey_Search));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;
	setHelpColorNormal();
	sprintf(str, "SHIFT+%c - DELETE LAST SEARCH WORD", toupper(charKey_Search));
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	//if (bTeachLanguage) setHelpColorSpecial();
	//else setHelpColorNormal();
	//sprintf(str, "%c - TEACH LANGUAGE", toupper(charKey_TeachLanguage));
	//livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	//drawPos.y -= lineHeight;

	sprintf(str, "CTRL+MOUSECLICK - TILE BASED CLICK");
	livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	drawPos.y -= lineHeight;

	if (connectedToMainServer && arcRunningSince > 0) {
		drawPos = lastScreenViewCenter;
		drawPos.x += viewWidth/2 - 440*guiScale;
		drawPos.y += viewHeight/2 - 30*guiScale;
		sprintf(str, "MAP RUNNING SINCE: %s", getArcTimeStr().c_str());
		livingLifePage->hetuwDrawScaledHandwritingFont( str, drawPos, guiScale );
	}
}

void HetuwMod::drawHungerWarning() {
	if ( ourLiveObject->foodStore + livingLifePage->hetuwGetYumBonus() <= 2 && ourLiveObject->maxFoodCapacity > 8) {
		float alpha = ( 1 - (ourLiveObject->foodStore / 8.0) ) * 0.3;
		doublePair startPos = livingLifePage->hetuwGetLastScreenViewCenter();
		setDrawColor( 1, 0, 0, alpha );
		drawRect( startPos, viewWidth * guiScale, viewHeight * guiScale );
	}
}
