#include "suggest.h"

/* Getting data inside lines */

bool e_suggestline_t::containssubdot(uint32_t subdot) {
    return subdot >= this->timestamp_start && subdot < this->timestamp_end;
}

bool e_suggestvariant_t::getButFromSubdot(uint32_t owner, uint32_t subdot, suggestbutton_t &button) {
    for(e_suggestline_t &line : this->lines) {
        if(line.owner != owner || !line.containssubdot(subdot)) continue;

        for(suggestbutton_t &b : line.buttons) {
            if(b.timestamp + line.timestamp_start == subdot) { button = b; return true; }
        }
    }
    return false;
}

bool e_suggestvariant_t::getButRefFromSubdot(uint32_t owner, uint32_t subdot, suggestbutton_t **button) {
    for(e_suggestline_t &line : this->lines) {
        if(line.owner != owner || !line.containssubdot(subdot)) continue;

        for(suggestbutton_t &b : line.buttons) {
            if(b.timestamp + line.timestamp_start == subdot) { *button = &b; return true; }
        }
    }
    return false;
}

bool e_suggestvariant_t::isDotOwned(int dot, uint32_t owner) {
    int subdot = dot * 24;

    for(e_suggestline_t &line : this->lines) {
        if(line.owner == owner && line.containssubdot(subdot)) return true;
    }
    return false;
}

/* Linking */

int e_suggestvariant_t::setLink(int linkId) {
    if(linkId >= 17) return 1; // Too high

    this->islinked = linkId <= 0;
    this->linknum = linkId;
    return 0; // Good
}

/* Managing Buttons */

bool buttonSorter(suggestbutton_t &button1, suggestbutton_t &button2) { return (button1.timestamp < button2.timestamp); }

void e_suggestvariant_t::createButton(uint32_t subdot, uint32_t owner, int buttonid) {
    /* First check if the button exists, if yes, just change button id and leave */
    suggestbutton_t *bref;
    if(this->getButRefFromSubdot(owner, subdot, &bref)) { bref->buttonid = buttonid; return; }

    suggestbutton_t newbutton;
    newbutton.buttonid = buttonid;
    for(soundentry_t &e : newbutton.sounds) {
        e.always_zero = 0;
        e.soundid = ~0;
        e.animationid = ~0;
        e.relativetime = ~0;
    }

    for(e_suggestline_t &line : this->lines) {
        if(line.owner != owner || !line.containssubdot(subdot)) continue;

        newbutton.timestamp = subdot - line.timestamp_start;
        line.buttons.push_back(newbutton);
        std::sort(line.buttons.begin(), line.buttons.end(), buttonSorter);
        return;
    }
}

void e_suggestvariant_t::deleteButton(uint32_t subdot, uint32_t owner) {
    for(e_suggestline_t &line : this->lines) {
        if(line.owner != owner || !line.containssubdot(subdot)) continue;
        uint32_t lineSubDot = subdot - line.timestamp_start;

        for(auto it = line.buttons.begin(); it != line.buttons.end(); ++it) {
            if(it->timestamp == lineSubDot) {
                line.buttons.erase(it);
                return;
            }
        }
    }
}
