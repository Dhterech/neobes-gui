#include "ptr2struct.h"

/* Search functions */

e_suggestline_t* e_suggestvariant_t::getLineRefFromSubdot(PLAYER_CODE owner, uint32_t subdot) {
    for(e_suggestline_t &l : this->lines) {
        if(has_player(l.owner, owner) && l.containssubdot(subdot)) {
            return &l;
        }
    }
    return nullptr;
}

bool e_suggestline_t::containssubdot(uint32_t subdot) {
    return subdot >= this->timestamp_start && subdot < this->timestamp_end;
}

suggestbutton_t* e_suggestvariant_t::getButtonRefFromSubdot(PLAYER_CODE owner, uint32_t subdot) {
    e_suggestline_t* line = getLineRefFromSubdot(owner, subdot);
    if (!line) return nullptr;

    for (suggestbutton_t &b : line->buttons) {
        if (b.timestamp + line->timestamp_start == subdot) {
            return &b;
        }
    }
    return nullptr;
}

bool e_suggestvariant_t::isDotOwned(int dot, PLAYER_CODE owner) {
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

void e_suggestvariant_t::createButton(uint32_t subdot, PLAYER_CODE owner, int buttonid) {
    /* First check if the button exists, if yes, just change button id and leave */
    if(suggestbutton_t *bref = this->getButtonRefFromSubdot(owner, subdot)) { bref->buttonid = buttonid; return; }

    suggestbutton_t newbutton;
    newbutton.buttonid = buttonid;
    for(soundentry_t &e : newbutton.sounds) {
        e.reserved = 0;
        e.soundid = ~0;
        e.animationid = ~0;
        e.relativetime = ~0;
    }

    for(e_suggestline_t &line : this->lines) {
        if(!has_player(line.owner, owner)) continue;
        if(!line.containssubdot(subdot)) continue;

        newbutton.timestamp = subdot - line.timestamp_start;
        line.buttons.push_back(newbutton);
        std::sort(line.buttons.begin(), line.buttons.end(), buttonSorter);
        return;
    }
}

void e_suggestvariant_t::deleteButton(uint32_t subdot, PLAYER_CODE owner) {
    for(e_suggestline_t &line : this->lines) {
        if(!has_player(line.owner, owner)) continue;
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

void e_suggestvariant_t::createLine(uint32_t subdot_start, uint32_t subdot_end, PLAYER_CODE owner) {
    e_suggestline_t *line = this->getLineRefFromSubdot(owner, subdot_start);
    if(line == nullptr) return;

    e_suggestline_t newline;
    newline.owner = owner;
    newline.timestamp_start = subdot_start;
    newline.timestamp_end = subdot_end;
    newline.coolmodethreshold = (has_player(owner, PLAYER_CODE::PCODE_BOXY) ? -1 : 150); // SFX = hidden
    for(uint32_t &i : newline.ptr_subtitles) i = 0;
    newline.type = TAPSCODE_ENUM::TAPSCODE_NORMAL;

    this->lines.push_back(newline);
    std::sort(this->lines.begin(), this->lines.end(), lineSorter);
}

void e_suggestvariant_t::deleteLine(int subdot, PLAYER_CODE owner) {
    for(int i = 0; i < this->lines.size(); i++) {
        e_suggestline_t &line = this->lines[i];
        if(!has_player(line.owner, owner)) continue;
        if(!line.containssubdot(subdot)) continue;

        this->lines.erase(this->lines.begin() + i);
        return;
    }
}

// resizing

void e_suggestvariant_t::resizeLine(uint32_t subdot, PLAYER_CODE owner, bool searchLeft, EditMode mode) {
    e_suggestline_t *pline = nullptr;

    if (searchLeft) {
        // Find closest line starting BEFORE subdot
        for (auto &line : this->lines) {
            if (!has_player(line.owner, owner)) continue;
            if (line.timestamp_start < subdot) {
                if (!pline || line.timestamp_start > pline->timestamp_start) {
                    pline = &line;
                }
            }
        }
    } else {
        // Try to find line containing subdot; if not, find closest starting AFTER subdot
        pline = this->getLineRefFromSubdot(owner, subdot);
        if (pline == nullptr) {
            for (auto &line : this->lines) {
                if (!has_player(line.owner, owner)) continue;
                if (line.timestamp_start > subdot) {
                    if (!pline || line.timestamp_start < pline->timestamp_start) {
                        pline = &line;
                    }
                }
            }
        }
    }

    if (!pline) return;
    e_suggestline_t &line = *pline;

    if (mode == EditMode::Move) { // Drag line
        int delta = (int)subdot - (int)line.timestamp_start;
        line.timestamp_start = subdot;
        line.timestamp_end += delta;
    }
    else { // Resize line
        if (searchLeft) {
            // Left logic: move the end point
            line.timestamp_end = subdot;
        } else {
            // Right logic: move start and adjust button offsets
            int delta = (int)subdot - (int)line.timestamp_start;
            line.timestamp_start = subdot;
            for (auto &btn : line.buttons) {
                btn.timestamp -= delta;
            }
        }

        line.buttons.erase(
            std::remove_if(line.buttons.begin(), line.buttons.end(), [&](const auto &btn) {
                return !line.containssubdot(line.timestamp_start + btn.timestamp);
            }),
            line.buttons.end()
            );
    }
}

