#include "ptr2struct.h"

/* Search functions */

bool e_suggestvariant_t::getLineRefFromSubdot(int owner, uint32_t subdot, e_suggestline_t **line) {
    for(e_suggestline_t &l : this->lines) {
        if(!(l.owner & uint32_t(owner))) continue;
        if(l.containssubdot(subdot)) { *line = &l; return true; }
    }
    return false;
}

bool e_suggestline_t::containssubdot(uint32_t subdot) {
    return subdot >= this->timestamp_start && subdot < this->timestamp_end;
}

// TODO: Use only one, this is waste of lines!
bool e_suggestvariant_t::getButFromSubdot(uint32_t owner, uint32_t subdot, suggestbutton_t &button) {
    for(e_suggestline_t &line : this->lines) {
        if(!(line.owner & uint32_t(owner))) continue;
        if(!line.containssubdot(subdot)) continue;

        for(suggestbutton_t &b : line.buttons) {
            if(b.timestamp + line.timestamp_start == subdot) { button = b; return true; }
        }
    }
    return false;
}

bool e_suggestvariant_t::getButRefFromSubdot(uint32_t owner, uint32_t subdot, suggestbutton_t **button) {
    for(e_suggestline_t &line : this->lines) {
        if(!(line.owner & uint32_t(owner))) continue;
        if(!line.containssubdot(subdot)) continue;

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

    this->islinked = linkId >= 0;
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
        e.unk = 0; // -1 = broken line
        e.relativetime = ~0;
    }

    for(e_suggestline_t &line : this->lines) {
        if(!(line.owner & uint32_t(owner))) continue;
        if(!line.containssubdot(subdot)) continue;

        newbutton.timestamp = subdot - line.timestamp_start;
        line.buttons.push_back(newbutton);
        std::sort(line.buttons.begin(), line.buttons.end(), buttonSorter);
        return;
    }
}

void e_suggestvariant_t::deleteButton(uint32_t subdot, uint32_t owner) {
    for(e_suggestline_t &line : this->lines) {
        if(!(line.owner & uint32_t(owner))) continue;
        if(!line.containssubdot(subdot)) continue;
        uint32_t lineSubDot = subdot - line.timestamp_start;

        for(auto it = line.buttons.begin(); it != line.buttons.end(); ++it) {
            if(it->timestamp == lineSubDot) {
                line.buttons.erase(it);
                return;
            }
        }
    }
}

/* Managing Lines */

bool lineSorter(e_suggestline_t &line1, e_suggestline_t &line2) { return (line1.timestamp_start < line2.timestamp_start); }

void e_suggestvariant_t::createLine(uint32_t subdot_start, uint32_t subdot_end, int owner) {
    e_suggestline_t *line;
    if(this->getLineRefFromSubdot(owner, subdot_start, &line)) return;

    e_suggestline_t newline;
    newline.owner = owner;
    newline.timestamp_start = subdot_start;
    newline.timestamp_end = subdot_end;
    newline.coolmodethreshold = (newline.owner == 0x8 ? -1 : 150); // SFX = hidden
    for(uint32_t &i : newline.localisations) i = 0;
    newline.vs_count = 0;

    this->lines.push_back(newline);
    std::sort(this->lines.begin(), this->lines.end(), lineSorter);
}

void e_suggestvariant_t::deleteLine(int subdot, int owner) {
    for(int i = 0; i < this->lines.size(); i++) {
        e_suggestline_t &line = this->lines[i];
        if(!(line.owner & uint32_t(owner))) continue;
        if(!line.containssubdot(subdot)) continue;

        this->lines.erase(this->lines.begin() + i);
        return;
    }
}
