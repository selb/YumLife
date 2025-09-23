#include "yumStateStore.h"

#include "minorGems/util/crc32.h"

#include <sstream>
#include <iomanip>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>

// The format of the state store is currently:
// key=value|value|value|
// where values are (aggressively) %-encoded.

std::string encode(const std::string &s) {
    std::stringstream ss;
    for (size_t i = 0; i < s.size(); i++) {
        if ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'A' && s[i] <= 'Z') ||
            (s[i] >= 'a' && s[i] <= 'z') || s[i] == '-') {
            ss << s[i];
        } else {
            ss << "%" << std::setfill('0') << std::setw(2) << std::hex << (unsigned int)(unsigned char)s[i];
        }
    }
    return ss.str();
}

std::string decode(const std::string &s) {
    std::stringstream ss;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '%') {
            if (i + 2 >= s.size()) {
                break;
            }
            ss << (char)std::stoi(s.substr(i + 1, 2), nullptr, 16);
            i += 2;
        } else {
            ss << s[i];
        }
    }
    return ss.str();
}

yumStateStore::yumStateStore(const char *filename) : mFilename(filename), mChanged(false) {
}

yumStateStore::~yumStateStore() {
}

static void renameAtomically(const char *oldName, const char *newName) {
#ifdef _WIN32
    MoveFileExA(oldName, newName, MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
#else
    rename(oldName, newName);
#endif
}

void yumStateStore::write() {
    std::string tempFilename = mFilename + ".tmp";
    std::ofstream file(tempFilename);
    if (!file.good()) {
        return;
    }

    std::stringstream checksumStream;
    for (auto it = mState.begin(); it != mState.end(); it++) {
        std::stringstream lineStream;
        std::string key = it->first;
        std::vector<std::string> values = it->second;
        lineStream << key << '=';
        for (auto it2 = values.begin(); it2 != values.end(); it2++) {
            lineStream << encode(*it2) << '|';
        }
        std::string line = lineStream.str();
        checksumStream << line << '~';
        file << line << '\n';
    }
    file << '@' << crc32((const unsigned char *)checksumStream.str().c_str(),
                         checksumStream.str().size()) << '\n';
    file.flush();
    file.close();
    renameAtomically(tempFilename.c_str(), mFilename.c_str());

    mChanged = false;
}

void yumStateStore::read() {
    std::ifstream file(mFilename);
    if (!file.good()) {
        return;
    }

    clearAll();

    unsigned int checksum = 0;
    // Lines of data are collected for checksumming without looking at specific
    // newline format. This should allow switching between Linux and Windows
    // builds during a life.
    std::stringstream checksumStream;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        if (line[0] == '@') {
            std::stringstream ss(line.substr(1));
            ss >> checksum;
            continue;
        }

        // strip carriage return in case this was saved on Windows but we're
        // loading on Linux or macOS
        if (line[line.size() - 1] == '\r') {
            line = line.substr(0, line.size() - 1);
        }

        checksumStream << line << '~';

        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }
        std::string key = line.substr(0, pos);
        std::string values = line.substr(pos + 1);
        std::vector<std::string> valuesList;
        while (!values.empty()) {
            size_t pos = values.find('|');
            if (pos == std::string::npos) {
                // current writer always ends in |, but let's leave this open
                // for future writers to decide
                valuesList.push_back(decode(values));
                values = "";
            } else {
                std::string value = values.substr(0, pos);
                valuesList.push_back(decode(value));
                values = values.substr(pos + 1);
            }
        }
        mState[key] = valuesList;
    }

    if (checksum != crc32((const unsigned char *)checksumStream.str().c_str(),
                            checksumStream.str().size())) {
        // XXX: mState.clear();
    }

    mKeys.clear();
    for (auto it = mState.begin(); it != mState.end(); it++) {
        mKeys.push_back(it->first);
    }

    mChanged = false;
}

bool yumStateStore::nextField(std::string &key) {
    if (mKeys.empty()) {
        key.clear();
        return false;
    }
    key = mKeys.front();
    mKeys.pop_front();
    mValuesIterator = mState[key].begin();
    mValuesEnd = mState[key].end();
    return true;
}

bool yumStateStore::get(std::string &value) {
    if (mValuesIterator == mValuesEnd) {
        value.clear();
        return false;
    }
    value = *mValuesIterator;
    mValuesIterator++;
    return true;
}

bool yumStateStore::get(int &value) {
    std::string valueStr;
    if (!get(valueStr)) {
        return false;
    }
    value = std::stoi(valueStr);
    return true;
}

bool yumStateStore::get(double &value) {
    std::string valueStr;
    if (!get(valueStr)) {
        return false;
    }
    value = std::stod(valueStr);
    return true;
}

bool yumStateStore::get(const char *key, std::string &value) {
    if (mState.count(key) == 0) {
        value.clear();
        return false;
    }
    if (mState[key].size() == 0) {
        value.clear();
        return false;
    }
    value = mState[key][0];
    return true;
}

bool yumStateStore::get(const char *key, int &value) {
    std::string valueStr;
    if (!get(key, valueStr)) {
        return false;
    }
    value = std::stoi(valueStr);
    return true;
}

bool yumStateStore::get(const char *key, double &value) {
    std::string valueStr;
    if (!get(key, valueStr)) {
        return false;
    }
    value = std::stod(valueStr);
    return true;
}

void yumStateStore::clear(const char *key) {
    mChanged = mChanged || mState.count(key) > 0;
    mState.erase(key);
}

void yumStateStore::clearAll() {
    mChanged = mChanged || !mState.empty();
    mState.clear();
}

void yumStateStore::push(const char *key, const char *value) {
    mChanged = true;
    mState[key].push_back(value);
}

void yumStateStore::push(const char *key, int value) {
    std::stringstream ss;
    ss << value;
    push(key, ss.str().c_str());
}

void yumStateStore::push(const char *key, double value) {
    std::stringstream ss;
    ss << value;
    push(key, ss.str().c_str());
}