#ifndef YUMREBIRTH_H
#define YUMREBIRTH_H

#include "PageComponent.h"
#include "CheckboxButton.h"

#include "minorGems/ui/event/ActionListener.h"

#include <string>
#include <unordered_map>
#include <vector>

class yumRebirthComponent : public PageComponent, public ActionListener {
    public:
        using Options = unsigned int;

        static constexpr Options BIOME_ARCTIC   = 1 << 0;
        static constexpr Options BIOME_LANGUAGE = 1 << 1;
        static constexpr Options BIOME_JUNGLE   = 1 << 2;
        static constexpr Options BIOME_DESERT   = 1 << 3;
        static constexpr Options BIOME_ALL = BIOME_ARCTIC | BIOME_LANGUAGE | BIOME_JUNGLE | BIOME_DESERT;

        static constexpr Options GENDER_MALE   = 1 << 4;
        static constexpr Options GENDER_FEMALE = 1 << 5;
        static constexpr Options GENDER_ALL = GENDER_MALE | GENDER_FEMALE;

        static constexpr Options REGION_MAIN       = 1 << 6;
        static constexpr Options REGION_DONKEYTOWN = 1 << 7;
        static constexpr Options REGION_ALL = REGION_MAIN | REGION_DONKEYTOWN;

        // x, y: center of the enable checkbox
        // offX, offY: relative coords of the full selection UI when enabled
        yumRebirthComponent(class Font *font, double x, double y, double offX = 10.0, double offY = -40.0);
        virtual ~yumRebirthComponent();
    
        virtual void draw();

        virtual void actionPerformed(GUIComponent *inTarget);

        // true if the user has checked the "AUTO /DIE" checkbox and the full
        // UI is showing; useful for hiding other UI elements in the way
        bool isEnabled();

        // call when the page is made active to update the UI based on changes
        // from other pages that have the same component
        void onMakeActive();

        static bool evaluateLife(char race, bool isFemale, bool isDonkeyTown);

        // applies the configured defaults, removing any entries that aren't
        // recognized as options
        static void registerDefaults(std::vector<std::string> &options);
    
    private:
        class Font *mFont;

        double mOffX, mOffY;

        CheckboxButton mEnabledCheckbox;

        struct OptionCheckbox {
            Options option;
            class CheckboxButton *checkbox;
        };

        std::vector<OptionCheckbox> mOptionCheckboxes;

        static void setOption(Options option, bool on);
};

#endif // YUMREBIRTH_H