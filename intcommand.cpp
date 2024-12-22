#include "intcommand.h"

QString modesSingleP[] = {"Cool", "Good", "Bad", "Awful", "Cooler", "Less cool", "Better", "Worse"};

QString job_setup_owners[] = {"Teacher", "PaRappa", "BoxxyBoi", "Special"};
QString job_setup_icon[] = {"Nothing", "PaRappa", "Boxxy", "Beard B.", "Master Onion", "Ant", "Sister Moosesha", "Takoyama", "Takoyama", "Colonel Noodle", "Mushi", "White fog", "Orange line", "Blue line"};

QString subjob_displaylr[] = {"Lesson 1", "Lesson 2", "Lesson 3", "Lesson 4", "Lesson 5", "Round 1", "Round 2", "Round 3", "Round 4", "Round 5"};

/*QString jobCommands[] = {
    "SETUP",                   // 0x0
    "LOAD_PLAY_RECORD",        // 0x1
    "SCORE",                   // 0x2
    "SCENE_BETTER",            // 0x3 (Check for accuracy)
    "SCENE_COOLER",            // 0x4 (Check for accuracy)
    "SCENE_OOPS",              // 0x5 (Check for accuracy)
    "SCENE_WORSE",             // 0x6 (Check for accuracy)
    "RETURN",                  // 0x7
    "END_STAGE",               // 0x8
    "RUN_SUBJOB",              // 0x9
    "",                        // 0xA (No entry for this value)
    "SCENE_UNKB",              // 0xB (Probably VS)
    "SCENE_UNKC",              // 0xC (Probably VS)
    "SCENE_UNKD",              // 0xD (Probably VS)
    "SCENE_UNKE",              // 0xE (Probably VS)
};*/

QString jobCommands[] = {
    "SCRRJ_PLY",               // 0x0
    "SCRRJ_SCR",               // 0x1
    "SCRRJ_EXAM",              // 0x2
    "SCRRJ_UP_LINE",           // 0x3
    "SCRRJ_UP_JOB",            // 0x4
    "SCRRJ_DOWN_LINE",         // 0x5
    "SCRRJ_DOWN_JOB",          // 0x6
    "SCRRJ_ENDJOB",            // 0x7
    "SCRRJ_ENDGAME",           // 0x8
    "SCRRJ_SUBJOB",            // 0x9
    "SCRRJ_MSG_DISP",          // 0xA
    "SCRRJ_ADD_JOB",           // 0xB
    "SCRRJ_SUB_JOB",           // 0xC
    "SCRRJ_LINE",              // 0xD
    "SCRRJ_JOB",               // 0xE
};

QString subjobCommands[] = {
    "SCRSUBJ_CDSND_ON",        // 0x0
    "SCRSUBJ_CDSND_OFF",       // 0x1
    "SCRSUBJ_DRAW_CHANGE",     // 0x2
    "",                        // 0x3 (No entry for this value)
    "SCRSUBJ_EFFECT",          // 0x4
    "SCRSUBJ_REVERS",          // 0x5
    "SCRSUBJ_SPU_ON",          // 0x6
    "SCRSUBJ_SPU_ON2",         // 0x7
    "SCRSUBJ_SPU_ON3",         // 0x8
    "SCRSUBJ_SPU_ON4",         // 0x9
    "SCRSUBJ_TITLE",           // 0xA
    "SCRSUBJ_LOOP",            // 0xB
    "SCRSUBJ_FADEOUT",         // 0xC
    "SCRSUBJ_ENDLOOP",         // 0xD
    "SCRSUBJ_SPUTRANS",        // 0xE
    "SCRSUBJ_STOP_MENDERER",   // 0xF
    "SCRSUBJ_BONUS_GAME",      // 0x10
    "SCRSUBJ_BONUS_GAME_END",  // 0x11
    "SCRSUBJ_LESSON",          // 0x12
    "SCRSUBJ_VS_RESET",        // 0x13
    "SCRSUBJ_CDSND_READY",     // 0x14
    "SCRSUBJ_CDSND_REQ",       // 0x15
    "SCRSUBJ_SPU_OFF",         // 0x16
    "SCRSUBJ_JIMAKU_OFF"       // 0x17
};

void intcommand::handleSceneCommand(GUICommand &gui, int rank, int time) {
    gui.arg1 = "RANK: " + modesSingleP[rank];
    gui.arg4 = "TIME: " + QString::number(time);
}

void intcommand::reverseHandleSceneCommand(GUICommand &gui, uint16_t &rank, uint32_t &time) {
    QString rankPrefix = "RANK: ";
    QString timePrefix = "TIME: ";

    if (gui.arg1.startsWith(rankPrefix)) {
        QString rankTmp = gui.arg1.mid(rankPrefix.length());
        for (int i = 0; i < sizeof(modesSingleP) / sizeof(modesSingleP[0]); ++i) {
            if (rankTmp == modesSingleP[i]) {
                rank = i;
                break;
            }
        }
    }

    time = gui.arg4.mid(timePrefix.length()).toInt();
}

void intcommand::handlePointer(GUICommand &gui, int pointer) {
    // TODO
}

GUICommand intcommand::ConvertToGUI(commandbuffer_t command) {
    GUICommand gui;
    if(command.arg1 >= 0 && command.arg1 <= 17) {
        gui.commandType = jobCommands[command.cmd_id];
    } else {
        gui.commandType = "UNKNOWN JOB";
    }

    switch(command.cmd_id) {
    case 00: // "SETUP";
        gui.arg1 = "OWNER: ";
        switch(command.arg1) {
        case 0x02:
            gui.arg1 += job_setup_owners[0]; break;
        case 0x04:
            gui.arg1 += job_setup_owners[1]; break;
        case 0x08:
            gui.arg1 += job_setup_owners[2]; break;
        case 0x10:
            gui.arg1 += job_setup_owners[3]; break;
        }
        gui.arg2 = "ICON: " + job_setup_icon[command.arg2];
        gui.arg4 = "TIME: " + QString::number(command.arg4);

        break;
    case 1:
        gui.commandType = "LOAD_PLAY_RECORD";
        gui.arg4 = "TODO: Pointer";
        break;
    case 2: // "SCORE"; "SCENE_BETTER"; "SCENE_COOLER"; "SCENE_OOPS"; "SCENE_WORSE";
        handleSceneCommand(gui, command.arg1, command.arg4);
        break;
    case 7:
    case 8: // "RETURN"; "END_STAGE";
        gui.arg1 = "Unknown";
        gui.arg4 = "Unknown";
        break;
    case 9: // "RUN_SUBJOB";
        if(command.arg1 >= 0 && command.arg1 <= 17) {
            gui.arg1 = subjobCommands[command.arg1];
        } else {
            gui.arg1 = "UNKNOWN SUBJOB";
        }

        if(command.arg1 == 0x2) { // "SCRSUBJ_DRAW_CHANGE"
            gui.arg4 = "TODO: Unknown";
        }

        if(command.arg1 == 0xE) { // "SCRSUBJ_SPUTRANS"
            gui.arg4 = "TODO: Point to Record";
        }

        if(command.arg1 == 0x12) { // "SCRSUBJ_LESSON"
            gui.arg2 = subjob_displaylr[command.arg2];
        }

        break;
    case 0xA:
    case 0xB:
    case 0xC:
    case 0xD:
    case 0xE:
        break;
    }
    return gui;
}

commandbuffer_t intcommand::ConvertToNormal(GUICommand gui) {
    commandbuffer_t normal;

    if (gui.commandType == "SETUP") {
        normal.cmd_id = 0x00;

        if (gui.arg1.contains("Teacher")) {
            normal.arg1 = 0x02;
        } else if (gui.arg1.contains("PaRappa")) {
            normal.arg1 = 0x04;
        } else if (gui.arg1.contains("BoxxyBoi")) {
            normal.arg1 = 0x08;
        } else if (gui.arg1.contains("Special")) {
            normal.arg1 = 0x10;
        }

        QString iconPrefix = "ICON: ";
        if (gui.arg2.startsWith(iconPrefix)) {
            QString icon = gui.arg2.mid(iconPrefix.length());
            for (int i = 0; i < sizeof(job_setup_icon) / sizeof(job_setup_icon[0]); ++i) {
                if (icon == job_setup_icon[i]) {
                    normal.arg2 = i;
                    break;
                }
            }
        }

        QString timePrefix = "TIME: ";
        if (gui.arg4.startsWith(timePrefix)) {
            normal.arg4 = gui.arg4.mid(timePrefix.length()).toInt();
        }

    } else if (gui.commandType == "LOAD_PLAY_RECORD") {
        normal.cmd_id = 0x01;

    } else if (gui.commandType.startsWith("SCENE_")) {
        if (gui.commandType == "SCORE") {
            normal.cmd_id = 0x02;
        } else if (gui.commandType == "SCENE_BETTER") {
            normal.cmd_id = 0x03;
        } else if (gui.commandType == "SCENE_COOLER") {
            normal.cmd_id = 0x04;
        } else if (gui.commandType == "SCENE_OOPS") {
            normal.cmd_id = 0x05;
        } else if (gui.commandType == "SCENE_WORSE") {
            normal.cmd_id = 0x06;
        }

        reverseHandleSceneCommand(gui, normal.arg1, normal.arg4);

    } else if (gui.commandType == "RETURN") {
        normal.cmd_id = 0x07;

    } else if (gui.commandType == "END_STAGE") {
        normal.cmd_id = 0x08;

    } else if (gui.commandType == "RUN_SUBJOB") {
        normal.cmd_id = 0x09;

        normal.arg1 = 0xFFFF;
        for (int i = 0; i < sizeof(subjobCommands) / sizeof(subjobCommands[0]); ++i) {
            if (gui.arg2 == subjobCommands[i]) {
                normal.arg1 = i;
            }
        }

        if(normal.arg1 == 0xFFFF) {
            gui.arg2 = "NOT_FOUND";
        }
    }

    return normal;
}
