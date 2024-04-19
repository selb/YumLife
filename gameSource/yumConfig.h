#ifndef YUMCONFIG_H
#define YUMCONFIG_H

#include <string>
#include <map>
#include <vector>

namespace yumConfig {
    typedef bool (*Predicate)(void);

    struct Options {
        const char *preComment = NULL;
        const char *postComment = NULL;
        // loadPredicate is called before the setting's value has been parsed!
        Predicate loadPredicate = NULL;
        Predicate savePredicate = NULL;
    };

    void registerSetting(const char *name, int &value, yumConfig::Options options = {});
    void registerSetting(const char *name, bool &value, yumConfig::Options options = {});
    void registerSetting(const char *name, std::string &value, yumConfig::Options options = {});
    void registerSetting(const char *name, std::vector<std::string> &value, yumConfig::Options options = {});
    void registerSetting(const char *name, unsigned char &value, yumConfig::Options options = {});
    void registerMappedSetting(const char *name, int &value, const std::map<std::string, int> &mapping, yumConfig::Options options = {});
    void registerScaledSetting(const char *name, float &value, int scale, yumConfig::Options options = {});

    void loadSettings(const char *filename);
    void saveSettings(const char *filename);
}

#endif // YUMCONFIG_H