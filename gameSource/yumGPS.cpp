#include "yumGPS.h"

#include "yumStatues.h"
#include "yumStateStore.h"
#include "LivingLifePage.h"
#include "objectBank.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <climits>
#include <set>
#include <deque>
#include <algorithm>
#include <sstream>

// Configuration defaults
bool yumGPS::cfgEnabled = true;
int yumGPS::cfgScanRate = 2;

// Biome boundary constants (in global coordinates)
const int ARCTIC_MIN_Y = 220;
const int ARCTIC_MAX_Y = 420;
const int JUNGLE_MIN_Y = -180;
// this looks like it should be 20, but the language biome cuts one off
const int JUNGLE_MAX_Y = 19;
const int DESERT_MIN_Y = -382;
const int DESERT_MAX_Y = -182;

// Biome IDs
const int BIOME_ARCTIC_ID = 4;
const int BIOME_JUNGLE_ID = 6;
const int BIOME_DESERT_ID = 5;

// Scanning parameters
const int SCAN_STRIDE = 5;
const int MAX_SCAN_DISTANCE = 1000000;

const int wellIDs[] = {
    662, // Shallow Well
    664, // Dry Shallow Well

    663, // Deep Well
    665, // Dry Deep Well

    2233, // Newcomen Tower Foundation - 1
    2232, // Newcomen Tower Foundation - 2
    2231, // Newcomen Tower Foundation - 3
    2230, // Newcomen Tower
    2229, // Newcomen Pump Tower
    2226, // Newcomen Pump without Rope

    2220, // Dry Newcomen Pump
    2221, // Newcomen Pump with Full Boiler
    2222, // Newcomen Pump with Charcoal
    2224, // Firing Newcomen Pump
    2234, // Wet Newcomen Pump
    3262, // Dry Newcomen Pump with Torn Seal
    3032, // Exhausted Newcomen Pump
    3036, // Exhausted Newcomen Pump - no core
    3037, // Exhausted Newcomen Pump - no beam
    3038, // Exhausted Newcomen Pump - 3 stone
    3039, // Exhausted Newcomen Pump - 2 stone
    3040, // Exhausted Newcomen Pump - 1 stone
    3031, // Exhausted Deep Well

    3171, // Wet Kerosene Newcomen Pump -  burner added low
    2352, // Wet Kerosene Newcomen Pump
    3172, // Wet Kerosene Newcomen Pump -  burner removed
    3173, // Wet Kerosene Newcomen Pump -  burner added high

    3265, // Dry Kerosene Newcomen Pump with Torn Seal
    2345, // Dry Kerosene Newcomen Pump
    2349, // Kerosene Newcomen Pump with Full Boiler
    2350, // Newcomen Pump with Kerosene
    2344, // Firing Kerosene Newcomen Pump
    2352, // Wet Kerosene Newcomen Pump
    3033, // Exhausted Kerosene Newcomen Pump

    2387, // Unpowered Pump Head - from shallow
    3042, // Unpowered Pump Head - from deep
    3041, // Unpowered Pump Head - from exhausted

    2388, // Dry Diesel Water Pump
    2389, // Running Water Pump
    2390, // Wet Diesel Water Pump
};

std::string yumGPS::encodeBase26(int value) {
    if (value == 0) return "A";

    bool negative = value < 0;
    unsigned int uvalue = negative ? -value : value;
    uvalue <<= 1;
    if (negative) {
        uvalue |= 1;
    }

    std::string result;
    while (uvalue > 0) {
        int digit = uvalue % 26;
        result += char('A' + digit);
        uvalue /= 26;
    }

    std::reverse(result.begin(), result.end());

    return result;
}

bool yumGPS::decodeBase26(const char *str, int &value) {
    if (str == NULL || str[0] == '\0') return false;

    int i = 0;

    unsigned int uvalue = 0;
    while (str[i] != '\0') {
        char c = str[i];
        int digit;

        if (c >= 'A' && c <= 'Z') {
            digit = c - 'A';
        } else {
            return false;
        }

        uvalue = uvalue * 26 + digit;
        i++;
    }

    value = uvalue >> 1;
    if (uvalue & 1) {
        value = -value;
    }

    return true;
}

yumGPS::yumGPS() {
    mHasAbsoluteX = false;
    mAbsoluteX = 0;
    mHasAbsoluteY = false;
    mAbsoluteY = 0;

    // Initialize biome bounds
    for (int i = 0; i < BIOME_COUNT; i++) {
        mBiomeBounds[i].minY = 0;
        mBiomeBounds[i].maxY = 0;
        mBiomeBounds[i].hasMin = false;
        mBiomeBounds[i].hasMax = false;
    }

    mIterationCounter = 0;
    mLivingLifePage = NULL;
}

yumGPS::~yumGPS() {
    // Map and other resources cleaned up automatically
}

void yumGPS::onNewLife(LivingLifePage *livingLifePage) {
    // Reset birth-relative tracking
    for (int i = 0; i < BIOME_COUNT; i++) {
        mBiomeBounds[i].minY = 0;
        mBiomeBounds[i].maxY = 0;
        mBiomeBounds[i].hasMin = false;
        mBiomeBounds[i].hasMax = false;
    }

    // Reset guesses and scanning state
    mGuesses.clear();
    mScanOffsets.clear();
    mIterationCounter = 0;

    // Reset resolved coordinates (load() will restore if resuming)
    mHasAbsoluteX = false;
    mAbsoluteX = 0;
    mHasAbsoluteY = false;
    mAbsoluteY = 0;

    // Clear per-life well tracking
    mBirthRelativeWellsSeen.clear();
    mGlobalWellsSeen.clear();
    mHighPriorityScanQueue.clear();

    // NOTE: mKnownGlobalWells persists across lives (loaded from save file)

    mLivingLifePage = livingLifePage;
    mEnabled = cfgEnabled;
}

void yumGPS::step() {
    if (!mEnabled) {
        return;
    }

    if (mLivingLifePage == NULL) {
        return;
    }

    // Scan for wells and other known objects
    scanObjects();

    // If we have X resolved, we're done. The remaining code below pertains to
    // scanning for X.
    if (mHasAbsoluteX) {
        return;
    }

    // Process one high-priority queue entry if available
    if (!mHighPriorityScanQueue.empty()) {
        Coords coord = mHighPriorityScanQueue.front();
        mHighPriorityScanQueue.pop_front();
        sendStatueRequest(coord.first, coord.second);
    }

    updateBiomeTracking();
    generateYGuesses();
    if (mGuesses.empty()) {
        return;
    }

    // Scan faster once we have a known Y, since that practically guarantees
    // we'll find a hit. Keep in mind this scan rate is even faster than it
    // looks since the single Y guess isn't round-robining with a bunch of
    // other Y guesses.
    int scanRate = (mHasAbsoluteY ? 4 : 1) * cfgScanRate;


    // If we have a known global well, assume we're at around the same X
    // and start guessing at the X coord of the first statue relative to
    // that well.
    int xCenter = 0;
    if (!mKnownGlobalWells.empty()) {
        xCenter = STATUE_TARGETS[0].x - mKnownGlobalWells[0].first;
    }

    int exhaustedCount = 0;
    for (int scan = 0; scan < scanRate; scan++) {
        // Round-robin: pick which Y guess to scan this iteration
        int guessIndex = mIterationCounter % mGuesses.size();
        int yValue = mGuesses[guessIndex];

        // Look up current scanOffset for this Y (defaults to 0 if not found)
        int scanOffset = mScanOffsets[yValue];

        // Calculate birth-relative Y for statue scanning
        int birthRelativeY = STATUE_TARGET_Y - yValue;

        // Calculate scan distance
        int scanDistance = scanOffset * SCAN_STRIDE;

        // Skip if we've reached the maximum scan offset
        if (scanDistance > MAX_SCAN_DISTANCE) {
            mIterationCounter++;
            if (++exhaustedCount >= (int)mGuesses.size()) {
                break;  // all guesses exhausted, nothing left to scan this frame
            }
            continue;
        }

        exhaustedCount = 0;  // reset on successful scan

        // Send statue requests
        sendStatueRequest(xCenter + scanDistance, birthRelativeY);
        if (scanOffset > 0) {
            sendStatueRequest(xCenter - scanDistance, birthRelativeY);
        }

        // Increment scanOffset for this Y value
        mScanOffsets[yValue]++;

        // Move to next guess
        mIterationCounter++;
    }
}

void yumGPS::updateBiomeTracking() {
    if (mLivingLifePage == NULL) {
        return;
    }

    int *mapBiomes = mLivingLifePage->mMapBiomes;
    if (mapBiomes == NULL) {
        return;
    }

    int mapOffsetX = mLivingLifePage->mMapOffsetX;
    int mapOffsetY = mLivingLifePage->mMapOffsetY;

    // Map is always 64x64
    const int MAP_D = 64;

    // Scan entire map array
    for (int mapY = 0; mapY < MAP_D; mapY++) {
        for (int mapX = 0; mapX < MAP_D; mapX++) {
            // Calculate array index (row-major order)
            int mapIndex = mapY * MAP_D + mapX;

            // Get biome ID for this tile
            int biomeID = mapBiomes[mapIndex];

            // Skip unknown tiles (biomeID == -1)
            if (biomeID < 0) {
                continue;
            }

            // Convert map coordinates to world coordinates (birth-relative)
            int worldY = mapY + mapOffsetY - MAP_D / 2;

            // Check if this is one of our tracked biomes
            BiomeType biome;
            bool isTrackedBiome = true;

            if (biomeID == BIOME_ARCTIC_ID) {
                biome = BIOME_ARCTIC;
            } else if (biomeID == BIOME_JUNGLE_ID) {
                biome = BIOME_JUNGLE;
            } else if (biomeID == BIOME_DESERT_ID) {
                biome = BIOME_DESERT;
            } else {
                isTrackedBiome = false;
            }

            if (isTrackedBiome) {
                BiomeBounds &bounds = mBiomeBounds[biome];

                // Update min/max Y for this biome (in world coordinates)
                if (!bounds.hasMin || worldY < bounds.minY) {
                    bounds.minY = worldY;
                    bounds.hasMin = true;
                }

                if (!bounds.hasMax || worldY > bounds.maxY) {
                    bounds.maxY = worldY;
                    bounds.hasMax = true;
                }
            }
        }
    }
}

void yumGPS::generateYGuesses() {
    // We don't need to guess anymore
    if (mHasAbsoluteY) {
        mGuesses.clear();
        mGuesses.push_back(mAbsoluteY);
        return;
    }

    mGuesses.clear();

    // Build list of all observed potential boundaries
    struct BoundaryObservation {
        int birthRelativeY;
        int expectedGlobalY;
    };

    std::vector<BoundaryObservation> boundaries;
    BiomeBounds &desertBounds = mBiomeBounds[BIOME_DESERT];
    BiomeBounds &jungleBounds = mBiomeBounds[BIOME_JUNGLE];
    BiomeBounds &arcticBounds = mBiomeBounds[BIOME_ARCTIC];

    if (desertBounds.hasMin) boundaries.push_back({desertBounds.minY, DESERT_MIN_Y});
    if (desertBounds.hasMax) boundaries.push_back({desertBounds.maxY, DESERT_MAX_Y});
    if (jungleBounds.hasMin) boundaries.push_back({jungleBounds.minY, JUNGLE_MIN_Y});
    if (jungleBounds.hasMax) boundaries.push_back({jungleBounds.maxY, JUNGLE_MAX_Y});
    if (arcticBounds.hasMin) boundaries.push_back({arcticBounds.minY, ARCTIC_MIN_Y});
    if (arcticBounds.hasMax) boundaries.push_back({arcticBounds.maxY, ARCTIC_MAX_Y});

    // Phase 1: Check if any two boundaries are at their expected distance. In
    // this case, we have resolved the birth Y coordinate for certain.
    for (size_t i = 0; i < boundaries.size(); i++) {
        for (size_t j = i + 1; j < boundaries.size(); j++) {
            int observedDist = boundaries[j].birthRelativeY - boundaries[i].birthRelativeY;
            int expectedDist = boundaries[j].expectedGlobalY - boundaries[i].expectedGlobalY;

            if (observedDist == expectedDist) {
                // Found matching pair! Calculate birth Y from either boundary
                int birthGlobalY = boundaries[i].expectedGlobalY - boundaries[i].birthRelativeY;

                mHasAbsoluteY = true;
                mAbsoluteY = birthGlobalY;
                mGuesses.push_back(birthGlobalY);
                return;
            }
        }
    }

    // Phase 2: Uncertain - create guesses from all observed boundaries
    for (const auto &boundary : boundaries) {
        int birthGlobalY = boundary.expectedGlobalY - boundary.birthRelativeY;
        mGuesses.push_back(birthGlobalY);
    }
}

void yumGPS::addKnownGlobalWell(int globalX, int globalY) {
    Coords globalCoord(globalX, globalY);

    // Check if we've already processed this well globally
    if (mGlobalWellsSeen.find(globalCoord) != mGlobalWellsSeen.end()) {
        return;
    }

    // New global well
    mGlobalWellsSeen.insert(globalCoord);

    // Remove if already in persisted vector (to move to head)
    auto it = std::find(mKnownGlobalWells.begin(),
                       mKnownGlobalWells.end(),
                       globalCoord);
    if (it != mKnownGlobalWells.end()) {
        mKnownGlobalWells.erase(it);
    }

    // Add to head (most recently seen)
    mKnownGlobalWells.insert(mKnownGlobalWells.begin(), globalCoord);

    // Trim to 256 entries
    if (mKnownGlobalWells.size() > 256) {
        mKnownGlobalWells.resize(256);
    }
}

void yumGPS::scanObjects() {
    if (mLivingLifePage == NULL) {
        return;
    }

    // Scan a 65x65 area around origin (just over map size)
    const int RADIUS = 32;

    LiveObject *ourLiveObject = mLivingLifePage->getOurLiveObject();
    if (ourLiveObject == NULL) {
        return;
    }

    int startX = ourLiveObject->xd - RADIUS;
    int endX = ourLiveObject->xd + RADIUS;
    int startY = ourLiveObject->yd - RADIUS;
    int endY = ourLiveObject->yd + RADIUS;

    bool resolved = mHasAbsoluteX && mHasAbsoluteY;
    for (int birthRelX = startX; birthRelX <= endX; birthRelX++) {
        for (int birthRelY = startY; birthRelY <= endY; birthRelY++) {
            int objId = mLivingLifePage->hetuwGetObjId(birthRelX, birthRelY);

            // Skip empty/unknown tiles
            if (objId <= 0) continue;

            // Resolve useDummy to parent object
            ObjectRecord *obj = getObject(objId);
            if (obj != NULL && obj->isUseDummy) {
                objId = obj->useDummyParent;
            }

            const int *wellEnd = wellIDs + sizeof(wellIDs) / sizeof(int);
            if (std::find(wellIDs, wellEnd, objId) == wellEnd) {
                continue;  // Not a well
            }

            // Found a well!
            Coords birthRelCoord(birthRelX, birthRelY);

            // Check if we've already seen this well (birth-relative)
            if (mBirthRelativeWellsSeen.find(birthRelCoord) == mBirthRelativeWellsSeen.end()) {
                // New well found - add to seen set
                mBirthRelativeWellsSeen.insert(birthRelCoord);

                if (!resolved) {
                    // Bounce against known global wells to generate high-priority scans
                    for (const auto &globalWellCoord : mKnownGlobalWells) {
                        // Assume THIS well (at birthRelX, birthRelY) IS that known global well
                        int impliedBirthX = globalWellCoord.first - birthRelX;
                        int impliedBirthY = globalWellCoord.second - birthRelY;

                        // Calculate where the first statue would be
                        int statueX = STATUE_TARGETS[0].x - impliedBirthX;
                        int statueY = STATUE_TARGET_Y - impliedBirthY;

                        // Add to high-priority scan queue
                        mHighPriorityScanQueue.push_back(Coords(statueX, statueY));
                    }

                    // Check this new well against all mentioned GPS coords from chat
                    for (const auto &mentionedCoord : mMentionedWellCoords) {
                        checkCoordAgainstWell(mentionedCoord, birthRelCoord);
                    }
                }
            }

            // If GPS is resolved, update the persisted global wells list
            if (resolved) {
                int globalX = mAbsoluteX + birthRelX;
                int globalY = mAbsoluteY + birthRelY;
                addKnownGlobalWell(globalX, globalY);
            }
        }
    }
}

void yumGPS::sendStatueRequest(int birthRelativeX, int birthRelativeY) {
    if (mLivingLifePage == NULL) {
        return;
    }

    char message[128];
    snprintf(message, sizeof(message), "STATUE %d %d#",
             mLivingLifePage->sendX(birthRelativeX), mLivingLifePage->sendY(birthRelativeY));

    // Send to server
    mLivingLifePage->sendToServerSocket(message);
}

void yumGPS::onStatueResponse(int birthRelativeX, int birthRelativeY,
                               int displayID, const char *name,
                               const char *clothingSet, const char *finalWords) {
    if (!mEnabled) {
        return;
    }

    if (mHasAbsoluteX) {
        return;  // Already resolved
    }

    // Check against all known statue targets
    for (int i = 0; i < STATUE_COUNT; i++) {
        const StatueTarget &target = STATUE_TARGETS[i];

        if (target.displayID == displayID &&
            strcmp(target.name, name) == 0 &&
            strcmp(target.clothingSet, clothingSet) == 0 &&
            strcmp(target.finalWords, finalWords) == 0) {

            // Match found! Calculate birth global position
            mAbsoluteX = target.x - birthRelativeX;
            mAbsoluteY = STATUE_TARGET_Y - birthRelativeY;
            mHasAbsoluteX = true;
            mHasAbsoluteY = true;

            // Upgrade any already seen local wells to known global wells
            for (const auto &birthRelCoord : mBirthRelativeWellsSeen) {
                int globalX = mAbsoluteX + birthRelCoord.first;
                int globalY = mAbsoluteY + birthRelCoord.second;
                addKnownGlobalWell(globalX, globalY);
            }

            // Clear potential GPS coords - no longer needed
            mMentionedWellCoords.clear();

            return;
        }
    }
}

std::string yumGPS::getStatus() {
    if (!cfgEnabled) {
        return "GPS DISABLED";
    }

    if (mHasAbsoluteX && mHasAbsoluteY) {
        std::ostringstream oss;
        oss << "RESOLVED " << mAbsoluteX << " " << mAbsoluteY;
        return oss.str();
    }

    if (!mHighPriorityScanQueue.empty()) {
        return "WELL SCAN";
    }

    if (mGuesses.empty()) {
        return "FIND BIOME";
    }

    // Determine status base
    std::string statusBase;
    if (mHasAbsoluteY) {
        statusBase = "FAST SCAN";
    } else {
        statusBase = "SLOW SCAN";
    }

    // Find minimum and maximum scanOffset among current Y guesses
    int minOffset = -1;
    int maxOffset = -1;
    for (int yValue : mGuesses) {
        int offset = mScanOffsets[yValue];  // Defaults to 0 if not in map
        if (minOffset == -1 || offset < minOffset) {
            minOffset = offset;
        }
        if (maxOffset == -1 || offset > maxOffset) {
            maxOffset = offset;
        }
    }

    // Convert to distance and format
    if (minOffset >= 0 || maxOffset >= 0) {
        if (minOffset * SCAN_STRIDE > MAX_SCAN_DISTANCE) {
            return statusBase + " EXHAUSTED";
        }
        std::ostringstream oss;
        oss << statusBase << " " << minOffset * SCAN_STRIDE
            << " / " << maxOffset * SCAN_STRIDE;
        return oss.str();
    }

    return statusBase;
}

bool yumGPS::getAbsoluteX(int &x) {
    if (mHasAbsoluteX) {
        x = mAbsoluteX;
        return true;
    }
    return false;
}

bool yumGPS::getAbsoluteY(int &y) {
    if (mHasAbsoluteY) {
        y = mAbsoluteY;
        return true;
    }
    return false;
}

yumGPS::Coords yumGPS::findNearestKnownWell() {
    if (mKnownGlobalWells.empty()) {
        return Coords(0, 0); // Shouldn't happen, caller checks
    }

    if (!mHasAbsoluteX || !mHasAbsoluteY || !mLivingLifePage) {
        // None of these states should happen, but just in case we'll return
        // the most recent well.
        return mKnownGlobalWells[0];
    }

    LiveObject *ourLiveObject = mLivingLifePage->getOurLiveObject();
    if (!ourLiveObject) {
        return mKnownGlobalWells[0]; // Fallback if no live object
    }

    // Convert player's birth-relative position to global coordinates
    int playerGlobalX = mAbsoluteX + ourLiveObject->xd;
    int playerGlobalY = mAbsoluteY + ourLiveObject->yd;

    // Find geometrically nearest well to player's current position
    Coords nearest = mKnownGlobalWells[0];
    int minDistSq = INT_MAX;

    for (const auto &well : mKnownGlobalWells) {
        int dx = well.first - playerGlobalX;
        int dy = well.second - playerGlobalY;
        int distSq = dx*dx + dy*dy;

        if (distSq < minDistSq) {
            minDistSq = distSq;
            nearest = well;
        }
    }

    return nearest;
}

void yumGPS::onGPSCommand() {
    if (!mEnabled || !mHasAbsoluteX || !mHasAbsoluteY || mKnownGlobalWells.empty()) {
        return; // Silent fail
    }

    Coords nearest = findNearestKnownWell();
    std::string xEncoded = encodeBase26(nearest.first);
    std::string yEncoded = encodeBase26(nearest.second);
    std::string gpsMessage = "GPS " + xEncoded + " " + yEncoded;

    if (mLivingLifePage) {
        mLivingLifePage->hetuwSay(gpsMessage.c_str());
    }
}

bool yumGPS::parseGPSMessage(const char *message, int &x, int &y) {
    if (!message) return false;

    // Skip leading ':' (from notes)
    if (*message == ':') message++;

    // Check for "GPS " prefix
    if (strncmp(message, "GPS ", 4) != 0) return false;
    message += 4;

    // Parse X coordinate
    const char *start = message;
    while (*message && *message != ' ') message++;

    std::string xStr(start, message - start);
    if (!decodeBase26(xStr.c_str(), x)) return false;

    // Parse Y coordinate
    if (*message == ' ') message++;
    start = message;
    while (*message && *message != ' ') message++;

    std::string yStr(start, message - start);
    if (!decodeBase26(yStr.c_str(), y)) return false;

    return true;
}

void yumGPS::checkCoordAgainstWell(const Coords &mentionedCoord, const Coords &birthRelWell) {
    // Calculate implied birth position if this well is at mentionedCoord
    int impliedBirthX = mentionedCoord.first - birthRelWell.first;
    int impliedBirthY = mentionedCoord.second - birthRelWell.second;

    // Add statue scan at implied birth position
    int statueX = STATUE_TARGETS[0].x - impliedBirthX;
    int statueY = STATUE_TARGET_Y - impliedBirthY;

    mHighPriorityScanQueue.push_back(Coords(statueX, statueY));
}

void yumGPS::onLocalChat(int playerID, const char *message) {
    if (!mEnabled || (mHasAbsoluteX && mHasAbsoluteY)) {
        return; // GPS disabled or already resolved
    }

    int x, y;
    if (parseGPSMessage(message, x, y)) {
        // Check if already have this coordinate
        for (const auto &existing : mMentionedWellCoords) {
            if (existing.first == x && existing.second == y) {
                return; // Already stored
            }
        }

        // Add new potential coordinate
        Coords newCoord(x, y);
        mMentionedWellCoords.push_back(newCoord);

        // Check this new coord against all known birth-relative wells
        for (const auto &well : mBirthRelativeWellsSeen) {
            checkCoordAgainstWell(newCoord, well);
        }
    }
}

void yumGPS::addSavedCoordinate(char name, int x, int y) {
    bool isGlobal = mHasAbsoluteX && mHasAbsoluteY;
    if (isGlobal) {
        x += mAbsoluteX;
        y += mAbsoluteY;
    }

    // Update existing
    auto it = std::find_if(mSavedCoordinates.begin(),
                           mSavedCoordinates.end(),
                           [name](const SavedCoordinate &coord) {
                               return coord.name == name;
                           });
    if (it != mSavedCoordinates.end()) {
        it->coords = Coords(x, y);
        it->isGlobal = isGlobal;
        return;
    }

    // Add new
    mSavedCoordinates.push_back({name, Coords(x, y), isGlobal});
}

void yumGPS::removeSavedCoordinate(char name) {
    auto it = std::find_if(mSavedCoordinates.begin(),
                           mSavedCoordinates.end(),
                           [name](const SavedCoordinate &coord) {
                               return coord.name == name;
                           });
    if (it != mSavedCoordinates.end()) {
        mSavedCoordinates.erase(it);
    }
}

const std::vector<yumGPS::SavedCoordinate>& yumGPS::getSavedCoordinates() {
    return mSavedCoordinates;
}

void yumGPS::save(yumStateStore &store) {
    if (!mEnabled) {
        return;
    }

    store.clear("life.gps.absoluteX");
    if (mHasAbsoluteX) {
        store.push("life.gps.absoluteX", mAbsoluteX);
    }

    store.clear("life.gps.absoluteY");
    if (mHasAbsoluteY) {
        store.push("life.gps.absoluteY", mAbsoluteY);
    }

    store.clear("gps.wells");
    for (const auto &coord : mKnownGlobalWells) {
        store.push("gps.wells", coord.first);
        store.push("gps.wells", coord.second);
    }

    store.clear("gps.savedCoordinates");
    for (auto &coord : mSavedCoordinates) {
        // resolve now if we can
        if (!coord.isGlobal && mHasAbsoluteX && mHasAbsoluteY) {
            coord.coords.first += mAbsoluteX;
            coord.coords.second += mAbsoluteY;
            coord.isGlobal = true;
        }
        // only save global coords
        if (!coord.isGlobal) {
            continue;
        }
        char name[2] = {coord.name, '\0'};
        store.push("gps.savedCoordinates", name);
        store.push("gps.savedCoordinates", coord.coords.first);
        store.push("gps.savedCoordinates", coord.coords.second);
    }
}

void yumGPS::load(yumStateStore &store, const std::string &key) {
    if (key == "life.gps.absoluteX") {
        mHasAbsoluteX = true;
        store.get(mAbsoluteX);
    } else if (key == "life.gps.absoluteY") {
        mHasAbsoluteY = true;
        store.get(mAbsoluteY);
    } else if (key == "gps.wells") {
        // Load known global wells as X,Y pairs
        mKnownGlobalWells.clear();
        int x, y;
        while (store.get(x)) {
            if (!store.get(y)) break;  // X without Y - data corruption, stop
            mKnownGlobalWells.push_back(Coords(x, y));
        }
    } else if (key == "gps.savedCoordinates") {
        mSavedCoordinates.clear();
        std::string name;
        int x, y;
        while (store.get(name)) {
            store.get(x);
            if (!store.get(y)) break; // incomplete record, stop
            if (name.length() != 1) continue;  // invalid name, skip
            mSavedCoordinates.push_back({name[0], Coords(x, y), true});
        }
    }
}

bool yumGPS::isEnabled() {
    return cfgEnabled && mEnabled;
}

void yumGPS::disable() {
    mEnabled = false;
}