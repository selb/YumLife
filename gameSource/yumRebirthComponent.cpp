#include "yumRebirthComponent.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"

#include "CheckboxButton.h"

yumRebirthComponent::Options yumRebirthComponent::currentOptions = 0;

yumRebirthComponent::yumRebirthComponent(Font *font, double x, double y, double offX, double offY)
    : PageComponent(x, y), mFont(font), mOffX(offX), mOffY(offY), mEnabledCheckbox(0, 0, 4.0) {

    addComponent(&mEnabledCheckbox);
    mEnabledCheckbox.addActionListener(this);
    mEnabledCheckbox.setToggled(currentOptions != 0);

    // now talking in relative coordinates for future rendering
    x = mOffX; y = mOffY;
    auto addOption = [&](Options option, const char *label) {
        CheckboxButton *checkbox = new CheckboxButton(x, y, 4.0);
        addComponent(checkbox);
        checkbox->addActionListener(this);
        mOptionCheckboxes.push_back(OptionCheckbox{option, label, checkbox});
        y -= 35;
    };

    addOption(BIOME_ARCTIC,   "ARCTIC");
    addOption(BIOME_LANGUAGE, "LANGUAGE");
    addOption(BIOME_JUNGLE,   "JUNGLE");
    addOption(BIOME_DESERT,   "DESERT");
    
    x = mOffX + 240; y = mOffY;
    addOption(GENDER_FEMALE, "FEMALE");
    addOption(GENDER_MALE,   "MALE");

    y -= 15;
    addOption(REGION_MAIN,       "MAIN AREA");
    addOption(REGION_DONKEYTOWN, "DONKEY TOWN");
}

yumRebirthComponent::~yumRebirthComponent() {
    for (auto optbox : mOptionCheckboxes) {
        delete optbox.checkbox;
    }
}

bool yumRebirthComponent::isEnabled() {
    // turn on if another yumRebirthComponent on another page was used
    if (currentOptions != 0) {
        mEnabledCheckbox.setToggled(true);
    }

    return mEnabledCheckbox.getToggled();
}

void yumRebirthComponent::onMakeActive() {
    if (currentOptions == 0) {
        mEnabledCheckbox.setToggled(false);
    }
}

void yumRebirthComponent::draw() {
    bool featureEnabled = isEnabled();

    // Synchronize checkbox state just before drawing for consistency. There
    // are multiple instances of this component, but also this allows us to be
    // lazy about updating the checkboxes when updating currentOptions.
    for (auto optbox : mOptionCheckboxes) {
        bool on = currentOptions & optbox.option;
        optbox.checkbox->setToggled(on);
        optbox.checkbox->setVisible(featureEnabled);
    }

    setDrawColor(1, 1, 1, 1.0);

    if (featureEnabled) {
        for (auto optbox : mOptionCheckboxes) {
            doublePair pos = optbox.checkbox->getCenter();
            pos.x += 24;
            mFont->drawString(optbox.label, pos, alignLeft);
        }
    }

    doublePair pos = mEnabledCheckbox.getCenter();
    pos.x += 24;
    mFont->drawString(featureEnabled ? "AUTO /DIE UNLESS:" : "AUTO /DIE", pos, alignLeft);
}

void yumRebirthComponent::actionPerformed(GUIComponent *inTarget) {
    if (inTarget == &mEnabledCheckbox) {
        // clear all options if the user disables the feature
        if (!mEnabledCheckbox.getToggled()) {
            currentOptions = 0;
        }
        return;
    }

    for (auto optbox : mOptionCheckboxes) {
        if (inTarget == optbox.checkbox) {
            bool on = optbox.checkbox->getToggled();

            // single choice options
            if (optbox.option & GENDER_ALL) {
                currentOptions &= ~GENDER_ALL;
            } else if (optbox.option & REGION_ALL) {
                currentOptions &= ~REGION_ALL;
            }

            if (on) {
                currentOptions |= optbox.option;
            } else {
                currentOptions &= ~optbox.option;
            }

            // all biomes = no preference
            if ((currentOptions & BIOME_ALL) == BIOME_ALL) {
                currentOptions &= ~BIOME_ALL;
            }
        }
    }
}

bool yumRebirthComponent::evaluateLife(char race, bool isFemale, bool isDonkeyTown) {
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