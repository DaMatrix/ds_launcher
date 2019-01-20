#include "game.h"

std::string Element::getName() {
    return this->name;
}

std::vector<Element>* Category::getChildren() {
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
