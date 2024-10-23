#include "yumRebirthComponent.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/game.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"

#include "CheckboxButton.h"

yumRebirthComponent::Options yumRebirthComponent::currentOptions = 0;

yumRebirthComponent::yumRebirthComponent(Font *font, double x, double y)
    : PageComponent(x, y), mFont(font) {

    // now talking in relative coordinates for future rendering
    x = -200; y = 0;
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
    
    x = 160; y = 0;
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

void yumRebirthComponent::draw() {
    // Synchronize checkbox state just before drawing for consistency. There
    // could be multiple instances of this component, but also this allows us
    // to be lazy about updating the checkboxes when updating currentOptions
    // in general.
    for (auto optbox : mOptionCheckboxes) {
        bool on = currentOptions & optbox.option;
        optbox.checkbox->setToggled(on);
    }

    for (auto optbox : mOptionCheckboxes) {
        doublePair pos = optbox.checkbox->getCenter();
        pos.x += 24;
        mFont->drawString(optbox.label, pos, alignLeft);
    }
}

void yumRebirthComponent::actionPerformed(GUIComponent *inTarget) {
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