#ifndef YUMREBIRTH_H
#define YUMREBIRTH_H

#include "PageComponent.h"

#include "minorGems/ui/event/ActionListener.h"

#include <vector>
#include <unordered_map>

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

        yumRebirthComponent(class Font *font, double x, double y);
        virtual ~yumRebirthComponent();
    
        virtual void draw();

        virtual void actionPerformed(GUIComponent *inTarget);

        static Options getSelectedOptions(void) { return currentOptions; }

        static bool evaluateLife(char race, bool isFemale, bool isDonkeyTown);
    
    private:
        static Options currentOptions;

        class Font *mFont;

        struct OptionCheckbox {
            Options option;
            const char *label;
            class CheckboxButton *checkbox;
        };

        std::vector<OptionCheckbox> mOptionCheckboxes;
};

#endif // YUMREBIRTH_H