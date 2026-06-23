#ifndef YUMGPS_H
#define YUMGPS_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <deque>

class LivingLifePage;


class yumGPS {
public:
    // Coordinate type alias
    using Coords = std::pair<int, int>;
    struct SavedCoordinate {
        char name;
        Coords coords;
        bool isGlobal;
    };

    yumGPS();
    ~yumGPS();

    // Events from hetuwmod
    void onNewLife(LivingLifePage *livingLifePage);
    void onStatueResponse(int birthRelativeX, int birthRelativeY,
                          int displayID, const char *name,
                          const char *clothingSet, const char *finalWords);
    void onFlightReset();
    void step();
    void disable();

    // Query interface for hetuwmod
    std::string getStatus();
    bool getAbsoluteX(int &x);
    bool getAbsoluteY(int &y);
    bool isEnabled();

    // Chat command handling
    void onGPSCommand();
    void onLocalChat(int playerID, const char *message);

    // Cross-life saved coordinate tracking
    void addSavedCoordinate(char name, int x, int y); // birth-relative coords
    void removeSavedCoordinate(char name);
    const std::vector<SavedCoordinate>& getSavedCoordinates();

    // Persistence
    void save(class yumStateStore &store);
    void load(class yumStateStore &store, const std::string &key);

    // Configuration
    static bool cfgEnabled;
    static int cfgScanRate;

private:

    enum BiomeType {
        BIOME_ARCTIC,
        BIOME_JUNGLE,
        BIOME_DESERT,
        BIOME_COUNT
    };

    struct BiomeBounds {
        int minY;
        int maxY;
        bool hasMin;
        bool hasMax;
    };

    // Resolved coordinates
    bool mHasAbsoluteX;
    int mAbsoluteX;
    bool mHasAbsoluteY;
    int mAbsoluteY;

    // Biome tracking (birth-relative coordinates)
    BiomeBounds mBiomeBounds[BIOME_COUNT];

    // Active Y coordinate guesses
    std::vector<int> mGuesses;

    // Y coordinate scan offsets (including guesses that are no longer active)
    std::map<int, int> mScanOffsets;
    int mIterationCounter;

    // Well tracking (birth-relative coordinates)
    std::set<Coords> mBirthRelativeWellsSeen;

    // Well tracking (global coordinates, when resolved)
    std::set<Coords> mGlobalWellsSeen;

    // High-priority statue scan queue (FIFO)
    std::deque<Coords> mHighPriorityScanQueue;

    // Persisted known global well locations (most recent first, max 256)
    std::vector<Coords> mKnownGlobalWells;

    // Saved coordinates
    std::vector<SavedCoordinate> mSavedCoordinates;

    LivingLifePage *mLivingLifePage;
    bool mEnabled;

    // Base-36 encoding/decoding
    static std::string encodeBase26(int value);
    static bool decodeBase26(const char *str, int &value);

    // GPS coordinate parsing from chat
    std::vector<Coords> mMentionedWellCoords;

    bool parseGPSMessage(const char *message, int &x, int &y);
    void checkCoordAgainstWell(const Coords &mentionedCoord, const Coords &birthRelWell);
    Coords findNearestKnownWell();

    void updateBiomeTracking();
    void generateYGuesses();
    void sendStatueRequest(int birthRelativeX, int birthRelativeY);
    void scanObjects();
    void addKnownGlobalWell(int globalX, int globalY);
};

#endif // YUMGPS_H
