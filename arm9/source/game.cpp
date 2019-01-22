#include "game.h"

char* Element::getName() {
    return this->name;
}

unsigned int Category::getChildren() {
    return this->children;
}

Element* Category::getParent() {
    return this->parent;
}

unsigned int Game::getVersion() {
    return this->version;
}

Element* Game::getParent() {
    return this->parent;
}

u16* Game::getIcon() {
    return this->icon;
}
