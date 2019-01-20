#ifndef ARM9_GAME_H
#define ARM9_GAME_H

#include <nds.h>

#include <stdio.h>
#include <string>
#include <vector>

class Element {
private:
    std::string name;
public:
    Element(std::string* name)   {
        this->name.assign(*name);
    }
    std::string getName();
    virtual Element* getParent() = 0;
};

class Category: public Element {
private:
    std::vector<Element>* children;
    Category* parent;
public:
    Category(std::vector<Element>* children, Category* parent, std::string* name) : Element(name)  {
        this->children = children;
        this->parent = parent;
    }
    std::vector<Element>* getChildren();
    Element* getParent() = 0;
};

class Game: public Element {
private:
    unsigned int version;
    Category* parent;
public:
    Game(unsigned int version, Category* parent, std::string* name) : Element(name)  {
        this->version = version;
        this->parent = parent;
    }
    unsigned int getVersion();
    Element* getParent() = 0;
};

#endif //ARM9_GAME_H
