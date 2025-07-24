#include "intcommand.h"

QString modesSingleP[] = {"Cool", "Good", "Bad", "Awful", "Cooler", "Less cool", "Better", "Worse"};

QString examMode[] = {"EXAM_NONE", "EXAM_COOL", "EXAM_GOOD", "EXAM_BAD", "EXAM_AWFUL", "EXAM_HOOK", "EXAM_VS", "EXAM_CANCEL", "EXAM_BONUS", "EXAM_MAX"};
QString examModeDo[] = { "EXAM_DO_NON", "EXAM_DO_END", "EXAM_DO_END_GO", "EXAM_DO_END_GO_RET"};

QString job_setup_owners[] = {"Unknown", "None", "Teacher", "PaRappa", "BoxxyBoi", "Special"};
QString job_setup_icon[] = {"Nothing", "PaRappa", "Boxxy", "Beard B.", "Master Onion", "Ant", "Sister Moosesha", "Takoyama", "Takoyama", "Colonel Noodle", "Mushi", "White fog", "Orange line", "Blue line"};

QString subjob_displaylr[] = {"Lesson 1", "Lesson 2", "Lesson 3", "Lesson 4", "Lesson 5", "Round 1", "Round 2", "Round 3", "Round 4", "Round 5"};

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
    "SCRSUBJ_TAP_RESET",       // 0x3
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
    "SCRSUBJ_JIMAKU_OFF",      // 0x17
    "SCRSUBJ_MAX"              // 0x18
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

void intcommand::handleRecord(GUICommand &gui, int pointer) {
    int count = 0;
    for(e_suggestrecord_t &record : Records) {
        if(pointer == record.address) {
            gui.arg4.append(QString::number(count));
            return;
        }
        count++;
    }
    gui.arg4.append("Not Loaded");
}

uint32_t intcommand::reverseHandleRecord(QString argument) {
    int recordNum = argument.toInt();

    if (recordNum >= 0 && recordNum < Records.size()) return Records[recordNum].address;
    return 0xFFFFFFF;
}

GUICommand intcommand::ConvertToGUI(commandbuffer_t command) {
    GUICommand gui;
    if(command.arg1 >= 0 || command.arg1 <= 17) {
        gui.commandType = jobCommands[command.cmd_id];
    } else {
        gui.commandType = "UNKNOWN JOB";
    }

    switch(command.cmd_id) {
    case 00: // "SETUP";
        gui.arg1 = "Owner: ";

        if(command.arg1 == 0) gui.arg1 += job_setup_owners[0];
        else if(command.arg1 & 0x01) gui.arg1 += job_setup_owners[1] + " ";
        else if(command.arg1 & 0x02) gui.arg1 += job_setup_owners[2] + " ";
        else if(command.arg1 & 0x04) gui.arg1 += job_setup_owners[3] + " ";
        else if(command.arg1 & 0x08) gui.arg1 += job_setup_owners[4] + " ";
        else if(command.arg1 & 0x10) gui.arg1 += job_setup_owners[5] + " ";

        gui.arg2 = "Icon: " + job_setup_icon[command.arg2];
        gui.arg4 = "Time: " + QString::number(command.arg4);
        break;
    case 1: // "LOAD_PLAY_RECORD";
        gui.arg4 = "Record: ";
        handleRecord(gui, command.arg4);
        break;
    case 2: // "SCORE"
        gui.arg1 = "Rank: " + modesSingleP[command.arg1];

        if(VSMode) {
            gui.arg4 = "Player: ";

            if(command.arg4 == 0) gui.arg4 += job_setup_owners[0];
            else if(command.arg4 & 0x01) gui.arg4 += job_setup_owners[1] + " ";
            else if(command.arg4 & 0x02) gui.arg4 += job_setup_owners[2] + " ";
            else if(command.arg4 & 0x04) gui.arg4 += job_setup_owners[3] + " ";
            else if(command.arg4 & 0x08) gui.arg4 += job_setup_owners[4] + " ";
            else if(command.arg4 & 0x10) gui.arg4 += job_setup_owners[5] + " ";
        }
        break;
    case 3: // "SCENE_BETTER";
    case 4: // "SCENE_COOLER";
    case 5: // "SCENE_OOPS";
    case 6: // "SCENE_WORSE";
        gui.arg1 = "Rank: " + modesSingleP[command.arg1];
        gui.arg4 = "Time: " + QString::number(command.arg4);
        break;
    case 7:
    case 8: // "RETURN"; "END_STAGE";
        // Doesn't seem to read anything directly afaik
        //gui.arg1 = "Unknown";
        //gui.arg4 = "Unknown";
        break;
    case 9: // "RUN_SUBJOB";
        if(command.arg1 >= 0 || command.arg1 <= 17) {
            gui.arg1 = subjobCommands[command.arg1];
        } else {
            gui.arg1 = "UNKNOWN SUBJOB";
        }
        //gui.arg4 = QString::number(command.arg4);

        if(command.arg1 >= 5 && command.arg1 <= 8) { // "SCRSUBJ_SPU_ON"
            gui.arg2 = "SoundBoard: " + QString::number(command.arg2);
            gui.arg3 = "Number: " + QString::number(command.arg3);
        }

        if(command.arg1 == 16) { // "SCRSUBJ_TITLE"
            gui.arg2 = "Dera: " + QString::number(command.arg2);
            gui.arg3 = "Menderer: " + QString::number(command.arg1);
        }

        if(command.arg1 == 0xE) { // "SCRSUBJ_SPUTRANS"
            gui.arg4 = "Record: ";
            handleRecord(gui, command.arg4);
        }

        if(command.arg1 == 0x12) { // "SCRSUBJ_LESSON"
            gui.arg2 = subjob_displaylr[command.arg2];
            gui.arg3 = "Time: " + QString::number(command.arg3);
        }

        if(command.arg1 == 0x16) { // "SCRSUBJ_SPU_OFF"
            gui.arg2 = "SoundBoard: " + QString::number(command.arg2);
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

int intcommand::ConvertToNormal(QString guiText, commandbuffer_t job, int row, int col) {
    int numberInput = 0;
    bool isNumber = false;
    if(guiText.startsWith("0x")) {
        numberInput = guiText.toInt(&isNumber, 16);
    } else {
        numberInput = guiText.toInt(&isNumber, 10);
    }

    // Modifying job type, see if job name is typed then get the number
    if(col == 0) {
        if(!isNumber) {
            int count = 0;
            for(QString job : jobCommands) {
                if(job == guiText) return count;
                count++;
            }
        }
    }

    if(col == 4 && job.cmd_id == 1) { // SCRRJ_SCR
        return reverseHandleRecord(guiText);
    }

    if(job.arg1 == 9) { // SCRRJ_SUB_JOB
        if(col == 4 && job.arg2 == 0xE) { // SCRSUBJ_SPUTRANS
            return reverseHandleRecord(guiText);
        }
    }

    return numberInput;
}
