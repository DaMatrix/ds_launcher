#ifndef ARM9_GAME_H
#define ARM9_GAME_H

#include "constants.h"

#include <string>
#include <vector>

class Element {
private:
    char* name;
public:
    Element(char* name)   {
        this->name = name;
    }
    ~Element() {
        delete this->name;
    }
    char* getName();
    virtual Element* getParent() = 0;
};

class Category: public Element {
private:
    unsigned int children;
    Category* parent;
public:
    Category(unsigned int children, Category* parent, char* name) : Element(name)  {
        this->children = children;
        this->parent = parent;
    }
    unsigned int getChildren();
    Element* getParent() = 0;
};

class Game: public Element {
private:
    unsigned int version;
    Category* parent;
    u16* icon;
public:
    Game(unsigned int version, Category* parent, u16* icon, char* name) : Element(name)  {
        this->version = version;
        this->parent = parent;
        this->icon = icon;
    }
    ~Game() {
        delete this->icon;
    }
    unsigned int getVersion();
    Element* getParent() = 0;
    u16* getIcon();
};

#endif //ARM9_GAME_H
