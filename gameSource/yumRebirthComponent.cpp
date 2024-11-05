#include "yumRebirthComponent.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"

#include "CheckboxButton.h"

#include <string>
#include <unordered_map>

using Options = yumRebirthComponent::Options;

static bool enabled = false;
static Options currentOptions = 0;

static struct {
    Options option;
    const char *label;
} optionInfo[] = {
    { yumRebirthComponent::BIOME_ARCTIC,      "ARCTIC"      },
    { yumRebirthComponent::BIOME_LANGUAGE,    "LANGUAGE"    },
    { yumRebirthComponent::BIOME_JUNGLE,      "JUNGLE"      },
    { yumRebirthComponent::BIOME_DESERT,      "DESERT"      },
    { yumRebirthComponent::GENDER_FEMALE,     "FEMALE"      },
    { yumRebirthComponent::GENDER_MALE,       "MALE"        },
    { yumRebirthComponent::REGION_MAIN,       "MAIN AREA"   },
    { yumRebirthComponent::REGION_DONKEYTOWN, "DONKEY TOWN" }
};

std::unordered_map<Options, const char*> optionLabelMapping = [] {
    std::unordered_map<Options, const char*> labels;
    for (auto &info : optionInfo) {
        labels[info.option] = info.label;
    }
    return labels;
}();

std::unordered_map<std::string, Options> configNameMapping = [] {
    std::unordered_map<std::string, Options> map;
    for (auto &info : optionInfo) {
        // remove whitespace since we can't parse that in the config file
        std::string label;
        for (const char *c = info.label; *c; c++) {
            if (*c != ' ') {
                label.push_back(*c);
            }
        }
        map[label] = info.option;
    }
    return map;
}();

yumRebirthComponent::yumRebirthComponent(Font *font, double x, double y, double offX, double offY)
    : PageComponent(x, y), mFont(font), mOffX(offX), mOffY(offY), mEnabledCheckbox(0, 0, 4.0) {

    addComponent(&mEnabledCheckbox);
    mEnabledCheckbox.addActionListener(this);
    mEnabledCheckbox.setToggled(currentOptions != 0);

    // now talking in relative coordinates for future rendering
    x = mOffX; y = mOffY;
    auto addOption = [&](Options option) {
        CheckboxButton *checkbox = new CheckboxButton(x, y, 4.0);
        addComponent(checkbox);
        checkbox->addActionListener(this);
        mOptionCheckboxes.push_back(OptionCheckbox{option, checkbox});
        y -= 35;
    };

    addOption(BIOME_ARCTIC);
    addOption(BIOME_LANGUAGE);
    addOption(BIOME_JUNGLE);
    addOption(BIOME_DESERT);
    
    x = mOffX + 240; y = mOffY;
    addOption(GENDER_FEMALE);
    addOption(GENDER_MALE);

    y -= 15;
    addOption(REGION_MAIN);
    addOption(REGION_DONKEYTOWN);
}

yumRebirthComponent::~yumRebirthComponent() {
    for (auto optbox : mOptionCheckboxes) {
        delete optbox.checkbox;
    }
}

bool yumRebirthComponent::isEnabled() {
    return enabled;
}

void yumRebirthComponent::onMakeActive() {
    if (!enabled) {
        mEnabledCheckbox.setToggled(false);
    }
}

void yumRebirthComponent::draw() {
    // Synchronize checkbox state just before drawing for consistency. There
    // are multiple instances of this component, but also this allows us to be
    // lazy about updating the checkboxes when updating currentOptions.
    mEnabledCheckbox.setToggled(enabled);
    for (auto optbox : mOptionCheckboxes) {
        bool on = currentOptions & optbox.option;
        optbox.checkbox->setToggled(on);
        optbox.checkbox->setVisible(enabled);
    }

    setDrawColor(1, 1, 1, 1.0);

    if (enabled) {
        for (auto optbox : mOptionCheckboxes) {
            doublePair pos = optbox.checkbox->getCenter();
            pos.x += 24;
            mFont->drawString(optionLabelMapping[optbox.option], pos, alignLeft);
        }
    }

    doublePair pos = mEnabledCheckbox.getCenter();
    pos.x += 24;
    mFont->drawString(enabled ? "AUTO /DIE UNLESS:" : "AUTO /DIE", pos, alignLeft);
}

void yumRebirthComponent::setOption(Options option, bool on) {
    // single choice options
    if (option & GENDER_ALL) {
        currentOptions &= ~GENDER_ALL;
    } else if (option & REGION_ALL) {
        currentOptions &= ~REGION_ALL;
    }

    if (on) {
        currentOptions |= option;
    } else {
        currentOptions &= ~option;
    }

    // all biomes = no preference
    if ((currentOptions & BIOME_ALL) == BIOME_ALL) {
        currentOptions &= ~BIOME_ALL;
    }
}

void yumRebirthComponent::actionPerformed(GUIComponent *inTarget) {
    if (inTarget == &mEnabledCheckbox) {
        enabled = mEnabledCheckbox.getToggled();
        return;
    }

    for (auto optbox : mOptionCheckboxes) {
        if (inTarget == optbox.checkbox) {
            bool on = optbox.checkbox->getToggled();
            setOption(optbox.option, on);
        }
    }
}

bool yumRebirthComponent::evaluateLife(char race, bool isFemale, bool isDonkeyTown) {
    if (!enabled) {
        return true;
    }

    Options biome;
    switch (race) {
        case 'A': biome = BIOME_DESERT; break;
        case 'C': biome = BIOME_JUNGLE; break;
        case 'D': biome = BIOME_LANGUAGE; break;
        case 'F': biome = BIOME_ARCTIC; break;
        default:  biome = BIOME_ALL; break;
    }

    if ((currentOptions & BIOME_ALL) != 0 && (biome & currentOptions) == 0) {
        return false;
    }

    Options gender = isFemale ? GENDER_FEMALE : GENDER_MALE;
    if ((currentOptions & GENDER_ALL) != 0 && (gender & currentOptions) == 0) {
        return false;
    }

    Options region = isDonkeyTown ? REGION_DONKEYTOWN : REGION_MAIN;
    if ((currentOptions & REGION_ALL) != 0 && (region & currentOptions) == 0) {
        return false;
    }

    return true;
}

void yumRebirthComponent::registerDefaults(std::vector<std::string> &options) {
    std::vector<std::string> filteredOptions;
    for (auto &option : options) {
        auto it = configNameMapping.find(option);
        if (it != configNameMapping.end()) {
            setOption(it->second, true);
            filteredOptions.push_back(option);
        }
    }
    options = filteredOptions;
    enabled = (currentOptions != 0);
}