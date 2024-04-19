#include "yumConfig.h"

#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <cmath>

class yumConfigVar {
    public:
        virtual void parse(const std::string &value) = 0;
        virtual void format(std::string &value) = 0;
        yumConfig::Options options;
};

static std::unordered_map<std::string, yumConfigVar*> configVars;
static std::vector<std::string> configVarNames;

static void registerSetting(const char *name, yumConfigVar *var, yumConfig::Options options) {
    configVars[name] = var;
    configVars[name]->options = options;
    configVarNames.push_back(name);
}

class yumConfigIntVar : public yumConfigVar {
    public:
        yumConfigIntVar(int &value) : mValue(value) {
            // mValue is not set to a default
        }

        virtual void parse(const std::string &value) {
            try {
                mValue = atoi(value.c_str());
            } catch (...) {
                // leave mValue as is
            }
        }

        virtual void format(std::string &value) {
            std::stringstream ss;
            ss << mValue;
            value = ss.str();
        }

    private:
        int &mValue;
};

void yumConfig::registerSetting(const char *name, int &value, yumConfig::Options options) {
    registerSetting(name, new yumConfigIntVar(value), options);
}

class yumConfigBoolVar : public yumConfigVar {
    public:
        yumConfigBoolVar(bool &value) : mValue(value) {
            // mValue is not set to a default
        }

        virtual void parse(const std::string &value) {
            if (value == "1" || value == "true" || value == "yes" || value == "on") {
                mValue = true;
            } else if (value == "0" || value == "false" || value == "no" || value == "off") {
                mValue = false;
            }
        }

        virtual void format(std::string &value) {
            value = mValue ? "yes" : "no";
        }

    private:
        bool &mValue;
};

void yumConfig::registerSetting(const char *name, bool &value, yumConfig::Options options) {
    registerSetting(name, new yumConfigBoolVar(value), options);
}

class yumConfigStringVar : public yumConfigVar {
    public:
        yumConfigStringVar(std::string &value) : mValue(value) {
            // mValue is not set to a default
        }

        virtual void parse(const std::string &value) {
            mValue = value;
        }

        virtual void format(std::string &value) {
            value = mValue;
        }

    private:
        std::string &mValue;
};

void yumConfig::registerSetting(const char *name, std::string &value, yumConfig::Options options) {
    registerSetting(name, new yumConfigStringVar(value), options);
}

class yumConfigVectorVar : public yumConfigVar {
    public:
        yumConfigVectorVar(std::vector<std::string> &value) : mValue(value) {
            // mValue is not set to a default
        }

        virtual void parse(const std::string &value) {
            mValue.clear();
            std::string item;
            std::stringstream ss(value);
            // this works because we currently strip all interior whitespace
            while (std::getline(ss, item, ',')) {
                mValue.push_back(item);
            }
        }

        virtual void format(std::string &value) {
            value.clear();
            for (size_t i = 0; i < mValue.size(); i++) {
                if (i > 0) {
                    value += ", ";
                }
                value += mValue[i];
            }
        }

    private:
        std::vector<std::string> &mValue;
};

void yumConfig::registerSetting(const char *name, std::vector<std::string> &value, yumConfig::Options options) {
    registerSetting(name, new yumConfigVectorVar(value), options);
}

class yumConfigKeyVar : public yumConfigVar {
    public:
        yumConfigKeyVar(unsigned char &value) : mValue(value) {
            // mValue is not set to a default
        }

        virtual void parse(const std::string &value) {
            if (value == "<space>") {
                mValue = ' ';
            } else if (!value.empty()) {
                mValue = value[0];
            } else {
                // hetuw defaulted to 254 for unbound keys; keep that behavior for now
                // in case there's a bug somewhere that causes a key event of 0
                mValue = 254;
            }
        }

        virtual void format(std::string &value) {
            if (mValue == 0 || mValue == 254) {
                value = "";
            } else if (mValue == ' ') {
                value = "<space>";
            } else {
                value = mValue;
            }
        }

    private:
        unsigned char &mValue;
};

void yumConfig::registerSetting(const char *name, unsigned char &value, yumConfig::Options options) {
    registerSetting(name, new yumConfigKeyVar(value), options);
}

class yumConfigMappedVar : public yumConfigVar {
public:
    yumConfigMappedVar(int& value, const std::map<std::string, int>& map)
        : mValue(value), mMap(map) {
        // mValue is not set to a default
    }

    virtual void parse(const std::string& value) {
        auto it = mMap.find(value);
        if (it != mMap.end()) {
            mValue = it->second;
            parsedValue = value;
            return;
        }

        // try parsing as an integer so we can upgrade
        // the old integer values to some new string values
        try {
            int candidate = std::stoi(value);
            for (const auto& pair : mMap) {
                if (pair.second == candidate) {
                    mValue = candidate;
                    parsedValue = pair.first;
                    return;
                }
            }
        } catch (...) { }

        // if we didn't find a match, leave mValue as is
    }

    virtual void format(std::string& value) {
        // prefer the value the user originally specified,
        // in case there's a duplicate value in the map
        if (!parsedValue.empty()) {
            value = parsedValue;
            return;
        }

        // otherwise, find the first matching value
        for (const auto& pair : mMap) {
            if (pair.second == mValue) {
                value = pair.first;
                return;
            }
        }

        // this is awkward, but should only happen if the referenced
        // mValue was never initialized to something in the map
        value = "";
    }

private:
    int& mValue;
    std::string parsedValue;
    std::map<std::string, int> mMap;
};

void yumConfig::registerMappedSetting(const char* name, int& value, const std::map<std::string, int>& mapping, yumConfig::Options options) {
    registerSetting(name, new yumConfigMappedVar(value, mapping), options);
}

class yumConfigScaledVar : public yumConfigVar {
public:
    yumConfigScaledVar(float& value, int scale)
        : mValue(value), mScale(scale) {
        // mValue is not set to a default
    }

    virtual void parse(const std::string& value) {
        try {
            int ival = std::stoi(value);
            mValue = float(ival) / mScale;
            mValue = std::max(0.0f, std::min(1.0f, mValue));
        } catch (...) {
            // leave mValue as is
        }
    }

    virtual void format(std::string& value) {
        std::stringstream ss;
        ss << int(round(mValue * mScale));
        value = ss.str();
    }

private:
    float& mValue;
    int mScale;
};

void yumConfig::registerScaledSetting(const char* name, float& value, int scale, yumConfig::Options options) {
    registerSetting(name, new yumConfigScaledVar(value, scale), options);
}

static void stripWhitespaceAndComments(std::string& str, bool stripComments) {
	// Strip leading whitespace
	size_t firstNonWhitespace = str.find_first_not_of(" \t\r");
	if (firstNonWhitespace != std::string::npos) {
		str.erase(0, firstNonWhitespace);
	}

	// Strip trailing whitespace
	size_t lastNonWhitespace = str.find_last_not_of(" \t\r");
	if (lastNonWhitespace != std::string::npos && lastNonWhitespace + 1 < str.length()) {
		str.erase(lastNonWhitespace + 1);
	}

	// Strip trailing // comments
	if (stripComments) {
		size_t commentPos = str.find("//");
		if (commentPos != std::string::npos) {
			str.erase(commentPos);
		}
	}
}

static bool getSettingsFileLine(std::string& name, std::string& value, const std::string& line) {
	bool readName = true;

    if (line.length() >= 2 && line[0] == '/' && line[1] == '/') {
        return false;
    }

	for (size_t i = 0; i < line.length(); i++) {
        // we should remove this later so that we can have nice strings with spaces in them,
        // but for now we'll stay compatible with current parsing
		if (line[i] == ' ') continue;

		if (readName) {
			if (line[i] == '=') {
				readName = false;
				continue;
			}
			name.push_back(line[i]);
		} else {
            value.push_back(line[i]);
		}
	}

    // we didn't find an '='
    if (readName) {
        return false;
    }

	stripWhitespaceAndComments(name, false);
	stripWhitespaceAndComments(value, true);

    return true;
}

void yumConfig::loadSettings(const char *filename) {
    std::ifstream file(filename);
    if (!file.good()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string name, value;
        if (!getSettingsFileLine(name, value, line)) {
            continue;
        }

        auto it = configVars.find(name);
        if (it != configVars.end()) {
            if (it->second->options.loadPredicate && !it->second->options.loadPredicate()) {
                continue;
            }
            it->second->parse(value);
        }
    }
}

void yumConfig::saveSettings(const char *filename) {
    std::ofstream file(filename);
    if (!file.good()) {
        return;
    }

    for (const auto& name : configVarNames) {
        auto it = configVars.find(name);
        if (it != configVars.end()) {
            if (it->second->options.savePredicate != NULL && !it->second->options.savePredicate()) {
                continue;
            }

            std::string value;
            it->second->format(value);

            if (it->second->options.preComment) {
                file << it->second->options.preComment;
            }

            file << name << " = " << value;

            if (it->second->options.postComment) {
                file << it->second->options.postComment;
            }

            file << std::endl;
        }
    }
}