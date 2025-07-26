#include "ptr2reader.h"
#include "emuinterfacer.h"
#include "neodata.h"

uint32_t findhdbase(uint32_t RESOURCE_LIST_BASE) {
    uint32_t caddr = RESOURCE_LIST_BASE;
    uint32_t raddr;
    uint32_t sig;
    int timeout = 20000; //NOTE: 20,000 resources is plenty

    do {
        pcsx2reader::read(caddr, &raddr, 4);
        pcsx2reader::read(raddr, &sig, 4);
        if(sig == HD_SIGNATURE) return caddr;
        caddr += 4;
    } while(timeout--);
    return 0;
}

int getnumhd(uint32_t hdbase) {
    uint32_t caddr = hdbase;
    uint32_t raddr;
    uint32_t sig;
    int count = 0;
    for(;;) {
        pcsx2reader::read(caddr, &raddr, 4);
        caddr += 4;

        pcsx2reader::read(raddr, &sig, 4);
        if(sig != HD_SIGNATURE) return count;
        count++;
    }
}

uint32_t findbdbase(uint32_t RESOURCE_LIST_BASE) {
    uint32_t hdbase = findhdbase(RESOURCE_LIST_BASE);
    return hdbase + (getnumhd(hdbase) * 4);
}

uint32_t getvhbasefromhd(uint32_t hdbase) {
    return hdbase + 0x50; //FIXME: Assuming 0x50 offset
}

uint32_t getheadfromhd(uint32_t hdbase) {
    return hdbase + 0x10; //FIXME: Assuming 0x10 offset
}

uint32_t getsmplfromhd(uint32_t hdbase) {
    uint32_t vhbase = getvhbasefromhd(hdbase);
    uint32_t len;
    pcsx2reader::read(vhbase + 0x8, &len, 4);

    return vhbase + len;
}

uint32_t getssetfromhd(uint32_t hdbase) {
    uint32_t smplbase = getsmplfromhd(hdbase);
    uint32_t len;
    pcsx2reader::read(smplbase + 0x8, &len, 4);

    return smplbase + len;
}

uint32_t getprogfromhd(uint32_t hdbase) {
    uint32_t ssetbase = getssetfromhd(hdbase);
    uint32_t len;
    pcsx2reader::read(ssetbase + 0x8, &len, 4);

    return ssetbase + len;
}

int installprograms(uint32_t hdbase, e_soundboard_t &sb) {
    sb.keys.clear();
    uint32_t numprograms;
    uint32_t smplbase = getsmplfromhd(hdbase);
    uint32_t ssetbase = getssetfromhd(hdbase);
    uint32_t progbase = getprogfromhd(hdbase);

    uint32_t smplindexbase = smplbase + 0x10;
    uint32_t ssetindexbase = ssetbase + 0x10;

    pcsx2reader::read(progbase + 0xC,&numprograms, 4);
    numprograms++;

    uint32_t pindexbase = progbase + 0x10;

    for(int i = 0; i < numprograms; i++) {
        uint32_t index;
        pcsx2reader::read(pindexbase + (i * 4), &index, 4);

        uint32_t progentrybase = progbase + index;
        uint32_t progentrylen;
        int8_t numtokens;

        pcsx2reader::read(progentrybase, &progentrylen, 4);
        pcsx2reader::read(progentrybase + 4, &numtokens, 1);
        uint32_t progtokenbase = progentrybase + progentrylen;

        for(int k = 0; k < numtokens; k++) {
            uint16_t ssetindex;
            uint32_t ssetoffset;
            uint16_t smplindex;
            uint32_t smploffset;
            uint16_t bdindex;
            uint8_t keyid;

            pcsx2reader::read(progtokenbase + (k * 0x14), &ssetindex, 2);
            pcsx2reader::read(progtokenbase + (k * 0x14) + 2, &keyid, 1);
            pcsx2reader::read(ssetindexbase + (ssetindex * 4), &ssetoffset, 4);
            pcsx2reader::read((ssetbase + ssetoffset) + 4, &smplindex, 2);
            pcsx2reader::read(smplindexbase + (smplindex * 4), &smploffset, 4);
            pcsx2reader::read((smplbase + smploffset), &bdindex, 2);

            sb.prog[i][keyid] = bdindex;
        }
    }
	return sb.keys.size();
}

void pcsx2DwnlKeytable(uint32_t keybase, int numkeys, e_soundboard_t &sb) {
    sb.keys.clear();
    sb.keys.resize(numkeys);

    for(int i = 0; i < numkeys; i++) pcsx2reader::read(keybase + (6 * i), &sb.keys[i], 6);
}

void pcsx2DwnlSoundboard(uint32_t hdbase, uint32_t bdbase, uint32_t keybase, int numkeys, e_soundboard_t &sb) {
    sb.clear();
    uint32_t head = getheadfromhd(hdbase);
    uint32_t vhbase = getvhbasefromhd(hdbase);
    uint32_t vhcount;
    uint32_t bdlen;
    //e_sound_t vhentry;

    pcsx2reader::read(head + 0x10, &bdlen, 4);

    sb.bd.len = bdlen;
    sb.bd.bytes = (char*)(malloc(sb.bd.len));
    pcsx2reader::read(bdbase, sb.bd.bytes, sb.bd.len); // large chunk

    pcsx2reader::read(vhbase + 0xC, &vhcount, 4);
    vhcount++;

    uint32_t vhindices = vhbase + 0x10;
    //uint32_t vhentries = vhindices + (vhcount * 4);

    sb.sounds.resize(vhcount);
    for(int i = 0; i < vhcount; i++) {
        uint32_t vhoffset;
        pcsx2reader::read(vhindices + (i * 4), &vhoffset, 4);
        pcsx2reader::read(vhbase + vhoffset, &sb.sounds[i], 8);
    }

    installprograms(hdbase, sb);
    if(keybase) pcsx2DwnlKeytable(keybase, numkeys, sb);
}

void pcsx2GetKeytables(uint32_t keylistbase, int count, int sbbase) {
    uint32_t numkeys;
    uint32_t ptrkeytable;
    for(int i = 0; i < count; i++) {
        uint32_t keylistentry = keylistbase + (i * 0x10);
        pcsx2reader::read(keylistentry + 0x8, &numkeys, 4);
        pcsx2reader::read(keylistentry + 0xC, &ptrkeytable, 4);
        pcsx2DwnlKeytable(ptrkeytable, int(numkeys), Soundboards[sbbase + i]);
    }
}

void pcsx2GetSoundboards(uint32_t hdlistbase, uint32_t bdlistbase, uint32_t count) {
    Soundboards.clear();
    uint32_t hdbase;
    uint32_t bdbase;
    Soundboards.resize(count);
    for(int i = 0; i < count; i++) {
        pcsx2reader::read(hdlistbase + (i * 4), &hdbase, 4);
        pcsx2reader::read(bdlistbase + (i * 4), &bdbase, 4);
        pcsx2DwnlSoundboard(hdbase, bdbase, 0,0, Soundboards[i]);
    }
}

void ps2LineToEditor(const suggestline_t &ps2, const suggestline_t_pal &ps2p, e_suggestline_t &editor) {
    uint32_t buttoncount = (PALMode ? ps2p.buttoncount : ps2.buttoncount);
    editor.owner = (PALMode ? ps2p.owner : ps2.owner);
    editor.buttons.clear();
    editor.buttons.resize(buttoncount);

    uint32_t ptrbuttons = (PALMode ? ps2p.ptr_buttons : ps2.ptr_buttons);
    for(int i = 0; i < buttoncount; i++) {
        int loc = ptrbuttons + (sizeof(suggestbutton_t) * i);
        pcsx2reader::read(loc, &editor.buttons[i], sizeof(suggestbutton_t));
    }

    editor.ptr_oops = (PALMode ? ps2p.ptr_oops : ps2.ptr_oops);
    editor.oopscount = (PALMode ? ps2p.oopscount : ps2.oopscount);

    editor.coolmodethreshold = (PALMode ? ps2p.coolmodethreshold : ps2.coolmodethreshold);
    for(int s = 0; s < (PALMode ? 7 : 4); s++) {editor.localisations[s] = (PALMode ? ps2p.localisations[s] : ps2.localisations[s]);}
    editor.timestamp_start = (PALMode ? ps2p.timestamp_start : ps2.timestamp_start);
    editor.timestamp_end = (PALMode ? ps2p.timestamp_end : ps2.timestamp_end);
    editor.vs_count = (PALMode ? ps2p.vs_count : ps2.vs_count);
}

void ps2VariantToEditor(const suggestvariant_t &ps2, e_suggestvariant_t &editor) {
    editor.lines.clear();
    editor.lines.resize(ps2.numlines);

    suggestline_t ps2line;
    suggestline_t_pal ps2lineP;
    for(int i = 0; i < ps2.numlines; i++) {
        if(!PALMode) pcsx2reader::read(ps2.ptr_lines + (sizeof(suggestline_t) * i), &ps2line, sizeof(suggestline_t));
        else pcsx2reader::read(ps2.ptr_lines + (sizeof(suggestline_t_pal) * i), &ps2lineP, sizeof(suggestline_t_pal));
        
        ps2LineToEditor(ps2line, ps2lineP, editor.lines[i]);
    }
}

void ps2RecordToEditor(const suggestrecord_t &ps2, e_suggestrecord_t &editor) {
    editor.lengthinsubdots = ps2.lengthinsubdots;
    editor.soundboardid = ps2.soundboardid;
    memcpy(editor.vs_data, ps2.vs_data, sizeof(editor.vs_data));

    for(int i = 0; i < 17; i++) { // Download variant
        ps2VariantToEditor(ps2.variants[i], editor.variants[i]);
        editor.variants[i].islinked = false;
        editor.variants[i].linknum = -1;

        for(int k = 0; k < i; k++) { // Check if its linked
            if(ps2.variants[i].ptr_lines == ps2.variants[k].ptr_lines) {
                editor.variants[i].islinked = true;
                editor.variants[i].linknum = k;
                break;
            }
        }
    }
}

void pcsx2DwnlRecord(uint32_t record_addr, e_suggestrecord_t &editor_record) {
    suggestrecord_t ps2_record;
    pcsx2reader::read(record_addr, &ps2_record, sizeof(ps2_record));
    ps2RecordToEditor(ps2_record, editor_record);
}

void pcsx2ParseComRecords() {
    e_suggestrecord_t record;

    for(int type = 0; type < ModeCommands.size(); type++) {
        if(!VSMode && (type == 2 || type == 3)) continue; // Skip BAD & Awful
        for(const commandbuffer_t &buffer : ModeCommands[type]) {
            if(buffer.cmd_id != SCENECMD_SETRECORD && buffer.cmd_id != SCENECMD_ACTIVATE) continue;
            if(buffer.cmd_id == SCENECMD_ACTIVATE && buffer.arg1 != 0xE) continue; // Get only preload record
            
            uint32_t raddr = buffer.cmd_id == SCENECMD_SETRECORD ? buffer.arg4 : buffer.arg2;
            bool alreadyLoaded = false;
            for(const e_suggestrecord_t &r : Records) {if(r.address == raddr) {alreadyLoaded = true; break;}}
            if(raddr > OLM_LINK_ADDRESS && !alreadyLoaded) {
                pcsx2DwnlRecord(raddr, record);
                record.type = type;
                record.address = raddr;
                Records.push_back(record);
            }
        }
    }
}

void pcsx2GetComBuffers() {
    commandbuffer_t cb;

    for(int i = ModeCommands.size(); i < Modes.size(); i++) {
        std::vector<commandbuffer_t> modecbs;
        for(int k = 0; k < Modes[i].count_scenecommands; k++) {
            pcsx2reader::read(Modes[i].ptr_scenecommands + (k * sizeof(commandbuffer_t)), &cb, sizeof(commandbuffer_t));
            modecbs.push_back(cb);
        }
        ModeCommands.push_back(modecbs);
    }
}

void pcxs2GetModelist(uint32_t stagemode_start, int count) {
    scenemode_t scene;

    for(int i = Modes.size(); i < count; i++) {
        pcsx2reader::read(stagemode_start + (i * sizeof(scene)), &scene, sizeof(scene));
        Modes.push_back(scene);
    }
}

void getProjectRecordAddresses() {
    int recordCount = 0;
    int lastRecord = 0;
    for(int type = 0; type < ModeCommands.size(); type++) {
        if(!VSMode && (type == 2 || type == 3)) continue; // Skip BAD & Awful
        for(const commandbuffer_t &buffer : ModeCommands[type]) {
            if(buffer.cmd_id != SCENECMD_SETRECORD && buffer.cmd_id != SCENECMD_ACTIVATE) continue;
            if(buffer.cmd_id == SCENECMD_ACTIVATE && buffer.arg1 != 0xE) continue; // Get only preload record

            uint32_t raddr = buffer.cmd_id == SCENECMD_SETRECORD ? buffer.arg4 : buffer.arg2;
            bool alreadyLoaded = raddr == lastRecord;
            if(raddr > OLM_LINK_ADDRESS && !alreadyLoaded) { // ugly ptr2besms fixup :(
                if(recordCount > Records.size()) break; // FIXME
                Records[recordCount].address = raddr;
                recordCount++;
                lastRecord = raddr;
            }
        }
    }
}
