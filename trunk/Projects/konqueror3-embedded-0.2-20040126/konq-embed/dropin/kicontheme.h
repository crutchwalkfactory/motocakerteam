#ifndef __kicontheme_h__
#define __kicontheme_h__

class KIcon
{
public:
    enum StdSizes { SizeMedium=32 };
    enum States { DefaultState, ActiveState, DisabledState, LastState };
    enum Group { NoGroup=-1, Desktop=0, FirstGroup=Desktop, Toolbar,
        MainToolbar, Small, Panel, LastGroup, User };
};

#endif
