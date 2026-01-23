# Job System Command Reference

This document details the internal scripting commands ("Jobs") and their associated sub-routines used in the PaRappa The Rapper 2 engine's `ScrCtrl` (Score Control) system.

---

## 1. Main Job Commands (SCRRJ)

These are the primary commands handled by the engine's main loop (`ScrCtrlIndvNextRead`).

### 0x00: SCRRJ_PLY
* **Arguments:**
    * `Sub`: Player Code Mask (e.g., PCODE_PARA, PCODE_TEACHER)
    * `JobD1`: Icon Index (See [`GUI_CURSOR_ENUM`](#setup-icons-gui_cursor_enum))
    * `Data`: Start Time (Frame/Tick)
* **Description:**
    Defines the active player segment. It sets the current timeline position (`current_time`) and the HUD icon (`cursor_num`) for the line.

### 0x01: SCRRJ_SCR
* **Arguments:**
    * `Sub`: Tap Follow Mode (`TAP_FOLLOW_NONE`, `SAVE`, `LOAD`)
    * `Data`: SCRDAT Pointer (SoundBank)
* **Description:**
    Loads the SoundBank (Tap Set) for the current segment. It initializes the sound buffer (`ScrTapDbuffSet`) based on the `sndrec_num` found in the SoundBank.

### 0x02: SCRRJ_EXAM
* **Arguments:**
    * `Sub`: Exam Type (See [`EXAM_ENUM`](#exams-exam_enum))
    * `Data`: VS Player Code (if VS mode)
* **Description:**
    Initializes the scoring and evaluation logic. It determines which rank logic to apply (e.g., checking if the player is in "Cool" mode) and resets the `exam_start` timer.

### 0x03: SCRRJ_UP_LINE
* **Arguments:**
    * `Sub`: Target Line Number (See [`SCRLINE_ENUM`](#ranks-scrline_enum))
    * `Data`: Jump Time
* **Description:**
    Configures the branching path for the "Getting Better" event. If the player performs well enough to increase their rank, the engine will jump to the specific `Line` and `Time` defined here.

### 0x04: SCRRJ_UP_JOB
* **Arguments:**
    * `Sub`: Target Job ID (`SCRR_JOB`)
    * `Data`: Jump Time
* **Description:**
    Configures an alternative branching path for a "Getting Better" event. Instead of changing the script line, this changes the execution pointer to a specific Job ID at a specific time.

### 0x05: SCRRJ_DOWN_LINE
* **Arguments:**
    * `Sub`: Target Line Number (See [`SCRLINE_ENUM`](#ranks-scrline_enum))
    * `Data`: Jump Time
* **Description:**
    Configures the branching path for a "Getting Worse" event. If the player performs poorly, the engine will jump to the specified `Line` and `Time`

### 0x06: SCRRJ_DOWN_JOB
* **Arguments:**
    * `Sub`: Target Job ID (`SCRR_JOB`)
    * `Data`: Jump Time
* **Description:**
    Configures the alternative branching path for a "Getting Worse" event, targeting a specific Job ID rather than a script line.

### 0x07: SCRRJ_ENDJOB
* **Arguments:**
    * `Data`: End Time
* **Description:**
    Signals the completion of the current individual job script thread. It sets the `SCS_KILL_REQ` flag, effectively terminating the processing for this specific job script entity once the time is reached.

### 0x08: SCRRJ_ENDGAME
* **Arguments:**
    * `Data`: End Time
* **Description:**
    Triggers the end of the entire stage. It sets the `SCS_END_REQ` flag.

### 0x09: SCRRJ_SUBJOB
* **Arguments:**
    * `Sub`: Sub-Command ID (`SCRSUBJ_ENUM`)
    * `JobD1`: Parameter 1 (Context dependent)
    * `JobD2`: Parameter 2 (Context dependent)
    * `Data`: Parameter 3 (Context dependent, often Time or ID)
* **Description:**
    Executes a specific engine sub-routine defined in the **[Sub-Job Commands List (Section 2)](#2-sub-job-commands-scrsubj)**. This is used for one-shot events like playing sounds, changing draw tables, etc etc...

### 0x0A: SCRRJ_MSG_DISP
* **Arguments:** None.
* **Description:**
    Nothing. Probably was meant to display a message?

### 0x0B: SCRRJ_ADD_JOB
* **Arguments:**
    * `Sub`: Target Job ID
    * `JobD1`: Target Line Number
    * `Data`: Jump Time
* **Description:**
    Configures the jump destination for the `SCREX_ADD` slot. This is likely used for specific positive feedback loops or "Getting Better" transitions that don't fit the standard Up/Down logic.

### 0x0C: SCRRJ_SUB_JOB
* **Arguments:**
    * `Sub`: Target Job ID
    * `JobD1`: Target Line Number
    * `Data`: Jump Time
* **Description:**
    Configures the jump destination for the `SCREX_SUB` slot. Used for specific negative feedback loops or transitions.

### 0x0D: SCRRJ_LINE
* **Arguments:**
    * `Sub`: Target Line Number
    * `JobD2`: Exam Job Index (e.g., `SCREX_AR_WIN0`, `SCREX_AR_LOSE0`)
    * `Data`: Jump Time
* **Description:**
    A generic line jump configuration command. It sets the `goto_time` and `goto_line` for a specific slot in the `scr_exam_job` array, indexed by `JobD2`. Prrobably used in VS mode for scoring.

### 0x0E: SCRRJ_JOB
* **Arguments:**
    * `Sub`: Target Job ID
    * `JobD2`: Exam Job Index
    * `Data`: Jump Time
* **Description:**
    A generic job jump configuration command. Similar to `SCRRJ_LINE`, but sets `goto_job` and `goto_job_time` for the specified exam job slot.

---

## 2. Sub-Job Commands (SCRSUBJ)

These commands are executed when the main command is [`0x09` (`SCRRJ_SUBJOB`)](#0x09-scrrj_subjob).

### 0x00: SCRSUBJ_CDSND_ON
* **Arguments:** None.
* **Description:**
    Sets the CD Audio (WP2) volume to 120 (On).

### 0x01: SCRSUBJ_CDSND_OFF
* **Arguments:** None.
* **Description:**
    Sets the CD Audio (WP2) volume to 0 (Mute).

### 0x02: SCRSUBJ_DRAW_CHANGE
* **Arguments:** None.
* **Description:**
    Triggers `ScrLincChangTbl`. This updates the drawing table pointers based on the current line, effectively switching the visual scene or animation set.

### 0x03: SCRSUBJ_TAP_RESET
* **Arguments:** None.
* **Description:**
    Nothing.

### 0x04: SCRSUBJ_EFFECT
* **Arguments:**
    * `JobD1`: Effect Mode (Reverb/Echo type)
    * `JobD2`: Effect Depth
* **Description:**
    Calls `TapCt(TAPCT_SETEFFECTMODE...)`. Configures the audio DSP effects (reverb) for the stage.
    **Presets (`JobD1`):**
    * `0`: OFF
    * `1`: ROOM
    * `2`: STUDIO A
    * `3`: STUDIO B
    * `4`: STUDIO C
    * `5`: HALL
    * `6`: SPACE
    * `7`: ECHO
    * `8`: DELAY
    * `9`: PIPE

### 0x05: SCRSUBJ_REVERS
* **Arguments:**
    * `JobD1`: Speed Factor
* **Description:**
    Handles playback manipulation (Reversing). It calculates a new reference time based on the ratio between `refTargetTime`, `refStartTime`, and the current time, then updates the draw table reference via `ScrLincChangTblRef`.

### 0x06: SCRSUBJ_SPU_ON
* **Arguments:**
    * `JobD1`: SoundBoard ID (Box)
    * `JobD2`: Sound Number
* **Description:**
    Triggers a sound effect via the SPU (Sound Processing Unit). Calls `ScrTapReq`.
    *(Note: SPU_ON2, ON3, and ON4 share this logic).*

### 0x0A: SCRSUBJ_TITLE
* **Arguments:**
    * `JobD1`: Mode (0 = Title, 2 = Dera Title)
    * `JobD2`: Initialization Flag
* **Description:**
    Manages the Title Screen loop. Checks for the Start button (`SCE_PADstart`). If pressed, it calculates the next time segment and advances the tempo/timeline to start the game.

### 0x0B: SCRSUBJ_LOOP
* **Arguments:**
    * `JobD1`: Target Time
* **Description:**
    Loops playback. Calls `TimeCallbackTimeSetChanTempo` to jump the audio and timeline back to the specified Target Time.

### 0x0C: SCRSUBJ_FADEOUT
* **Arguments:** None.
* **Description:**
    Sets the global flag `fadeoutStartKey = TRUE`, instructing the main game loop to begin a fade-out.

### 0x0D: SCRSUBJ_ENDLOOP
* **Arguments:** None.
* **Description:**
    Sets `gameEndWaitLoop = TRUE`. Used to hold the game in a specific state until a condition is met.

### 0x0E: SCRSUBJ_SPUTRANS
* **Arguments:**
    * `JobD1`: Pointer to Data Index
* **Description:**
    Transfers SPU data dynamically. Calls `ScrTapDbuffSetSp` to swap sound banks or load new audio data into memory during gameplay.

### 0x0F: SCRSUBJ_STOP_MENDERER
* **Arguments:** None.
* **Description:**
    Calculates the remaining time in the measure and calls `PrDecelerateMenderer`. MENDERER is a wordplay with 麺 (noodle) + renderer, so it stops the "Noodle Renderer" (the engine's rendering sequencer).

### 0x10: SCRSUBJ_BONUS_GAME
* **Arguments:** None.
* **Description:**
    Executes `bonusGameCtrl`. This handles the logic for the bonus stages, processing player inputs against the "Kotama" (spirits) objects.

### 0x11: SCRSUBJ_BONUS_GAME_END
* **Arguments:** None.
* **Description:**
    Finalizes the bonus stage. Saves the accumulated bonus score (`bonusPointSave`) and triggers the score display (`bonusScoreDraw`).

### 0x12: SCRSUBJ_LESSON
* **Arguments:**
    * `JobD1`: Lesson/Round Type (`SCRRJ_LESSON_ROUND_ENUM`)
    * `JobD2`: Timer
* **Description:**
    Displays the "Lesson X" or "Round X" graphic (`LessonRoundDisp`).

### 0x13: SCRSUBJ_VS_RESET
* **Arguments:** None.
* **Description:**
    Resets the VS Mode state. Sets scores for Para and Teacher to 500 and resets the VS animation states.

### 0x14: SCRSUBJ_CDSND_READY
* **Arguments:**
    * `JobD1`: File Index
* **Description:**
    Calls `CdctrlWP2Set`. Prepares the CD sub-system with a specific audio file structure, getting it ready for playback.

### 0x15: SCRSUBJ_CDSND_REQ
* **Arguments:**
    * `JobD1`: Volume
* **Description:**
    Triggers actual CD playback via `CdctrlWP2Play` and sets the volume to `JobD1`.

### 0x16: SCRSUBJ_SPU_OFF
* **Arguments:**
    * `JobD1`: SoundBoard ID (Box)
* **Description:**
    Stops the playback of a specific sound effect via `ScrTapReqStop`.

### 0x17: SCRSUBJ_JIMAKU_OFF
* **Arguments:** None.
* **Description:**
    Sets `jimakuWakuOff = TRUE`. Disables the display of the subtitles.

---

## 3. Reference Constants

### Ranks (`SCRLINE_ENUM`)
*   `0`: Cool
*   `1`: Good
*   `2`: Bad
*   `3`: Awful
*   `4`: To Cool (Transition)
*   `5`: From Cool (Transition)
*   `6`: To Up
*   `7`: To Down
*   `8`: Game Over

### Setup Icons (`GUI_CURSOR_ENUM`)
*   `0`: None
*   `1`: PaRappa
*   `2`: Boxy Boy
*   `3`: Stage 1 (Onion)
*   `4`: Stage 2 (Mooselini)
*   `5`: Stage 3 (Frog)
*   `6`: Stage 4 (Chicken)
*   `7`: Stage 5 (Toilet)
*   `8`: Stage 6 (Hairdresser)
*   `9`: Stage 7 (Noodle)
*   `10`: Stage 8 (Mushi)

### Exams (`EXAM_ENUM`)
*   `0`: None
*   `1`: Cool
*   `2`: Good
*   `3`: Bad
*   `4`: Awful
*   `5`: Hook (Branching)
*   `6`: VS
*   `7`: Cancel
*   `8`: Bonus
