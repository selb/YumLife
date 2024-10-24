#include "GamePage.h"



#include "minorGems/ui/event/ActionListener.h"


#include "TextButton.h"

#include "yumRebirthComponent.h"

class RebirthChoicePage : public GamePage, public ActionListener {
        
    public:
        RebirthChoicePage();
        

        void showReviewButton( char inShow );


        virtual void actionPerformed( GUIComponent *inTarget );

        virtual void draw( doublePair inViewCenter, 
                           double inViewSize );

        virtual void makeActive( char inFresh );
        
    protected:
        
        TextButton mQuitButton;
        TextButton mReviewButton;
        TextButton mRebornButton;
        TextButton mGenesButton;

        yumRebirthComponent mYumRebirth;
        
        TextButton mTutorialButton;
        TextButton mMenuButton;
        TextButton mFriendsButton;
    };
