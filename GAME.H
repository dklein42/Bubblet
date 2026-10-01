/*
===========================================================================
	game.h -- Headerfile for game.cpp
	(c) 1998/99 House of Bytes Software
===========================================================================
#include <windowsx.h>
#include <stdlib.h>
#include <stdarg.h>
*/

//#define REGISTERED_BUILD

#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdio.h>
#include "ddraw.h"
#include "gfx.h"

// Modes
#define IDLE   0
#define INTRO  1          //unbenutzt
#define GAME   2
#define ABOUT  3
#define SETUP  4
#define SCORES 5
#define SHAREWAREINFO 6
#define REGISTER 7
#define HELP   8          //unbenutzt
#define GAME_SCORE 9
#define ALWAYS 255 // Speziell nur für immer sichtbare Schalter!!
#define OLDMODE 256 // Zum zurück schalten in vorherigen mode

//Game-Types
#define GT_NORMAL      0
#define GT_CONTINUOUS  1
#define GT_SHIFTER     2
#define GT_MEGASHIFT   3

// Bitmaps
#define BMP_BACKGROUND       MAKEINTRESOURCE(100)
#define BMP_PAPER_X          MAKEINTRESOURCE(101)
#define BMP_PAPER            MAKEINTRESOURCE(102)
#define BMP_BUBBLET_LOGO     MAKEINTRESOURCE(103)
#define BMP_SCORE_TABLE      MAKEINTRESOURCE(104)
#define BMP_GAME_SCORE_TABLE MAKEINTRESOURCE(105)
#define BMP_SETUP_MAP        MAKEINTRESOURCE(106)
#define BMP_CHECKED_BOX      MAKEINTRESOURCE(107)
#define BTN_EXIT             MAKEINTRESOURCE(108)
#define BTN_EXIT_X           MAKEINTRESOURCE(109)
#define BTN_ABOUT            MAKEINTRESOURCE(110)
#define BTN_ABOUT_X          MAKEINTRESOURCE(111)
#define BTN_SETUP            MAKEINTRESOURCE(112)
#define BTN_SETUP_X          MAKEINTRESOURCE(113)
#define BTN_SCORES           MAKEINTRESOURCE(114)
#define BTN_SCORES_X         MAKEINTRESOURCE(115)
#define BTN_NEW              MAKEINTRESOURCE(116)
#define BTN_NEW_X            MAKEINTRESOURCE(117)
#define BTN_END              MAKEINTRESOURCE(118)
#define BTN_END_X            MAKEINTRESOURCE(119)
#define BTN_OK               MAKEINTRESOURCE(120)
#define BTN_OK_X             MAKEINTRESOURCE(121)
#define BTN_DONE             MAKEINTRESOURCE(122)
#define BTN_DONE_X           MAKEINTRESOURCE(123)
#define BTN_RESET            MAKEINTRESOURCE(124)
#define BTN_RESET_X          MAKEINTRESOURCE(125)
#define BTN_REGISTER         MAKEINTRESOURCE(126)
#define BTN_REGISTER_X       MAKEINTRESOURCE(127)
#define BTN_UNDO             MAKEINTRESOURCE(134)
#define BTN_UNDO_X           MAKEINTRESOURCE(135)
#define BTNMASK              MAKEINTRESOURCE(128)

#define BUBBLE_1             MAKEINTRESOURCE(129)
#define BUBBLE_2             MAKEINTRESOURCE(130)
#define BUBBLE_3             MAKEINTRESOURCE(131)
#define BUBBLE_4             MAKEINTRESOURCE(132)
#define BUBBLE_5             MAKEINTRESOURCE(133)

#define PLAYFIELD_START_X 8
#define PLAYFIELD_START_Y 8

//sfx
#define SFX_INTRO     "intro.wav"
#define SFX_CLICK     "click.wav"
#define SFX_POP       "pop.wav"
#define SFX_MEGAPOP   "megapop.wav"
#define SFX_SCORE     "score.wav"
#define SFX_HIGHSCORE "higscore.wav"
#define SFX_SHAREWARE "sharewar.wav"

//sfx aliase
#define A_INTRO     "intro"
#define A_CLICK     "click"
#define A_POP       "pop"
#define A_MEGAPOP   "megapop"
#define A_SCORE     "score"
#define A_HIGHSCORE "highscore"
#define A_SHAREWARE "shareware"

//Windows-Messages (für PostMessage)
#define WM_ENDGAME 999
#define WM_SURFACELOST 9999

struct sBUTTON
{
  POINT topleft;
  POINT bottomright;
  Surface bmp_normal;
  Surface bmp_pressed;
  BOOL pressed; // gedrückt?
  int mode;     // In welchem Mode aktiv?
};

struct sBUBBLE_FIELD  //einzelner Bubble
{
  int type;        //Bubble-Typ (1-5, 0 keiner)
  POINT move_pos;  //Für Animation wird hier die aktuelle Position vermerkt!
  POINT old_pos;  //Für Undo wird hier die alte Position (im Field) vermerkt!
  BOOL marked;     //gehört der Bubble zu den ausgewählten?
};

struct sGAME_SCORES
{
  int score;
  int bubble_bonus;
};

struct sSTATISTIC_BLOCK
{
  int games;        //Anzahl Spiele
  long scores;  //Alle erreichten Punkte
  int high;     //High-Score
};

struct sSCORE_STATISTIC
{
  sSTATISTIC_BLOCK normal;
  sSTATISTIC_BLOCK continuous;
  sSTATISTIC_BLOCK shifter;
  sSTATISTIC_BLOCK megashift;
};

struct sSETUP_DATA
{
  int game_type;
  BOOL play_sounds;
  BOOL animations;
};

FILE *f;
LPCSTR Status_Text;
Surface pic;
Surface paper_pic, paper_x_pic;
Surface score_table_pic, game_score_table_pic;
Surface setup_map_pic;
Surface checked_box_pic;
Surface bk_pic;
Surface btnmask;
Surface bubble_1,bubble_2,bubble_3,bubble_4,bubble_5;
HCURSOR cursor;
int mode= IDLE;
int last_mode= IDLE;
int last_mode_2= 0;
BOOL main_btn_mask= FALSE;
BOOL bubbles_marked= FALSE; //sind welche markiert?
BOOL game_busy= FALSE; //Beschäftigt? (mit Animation)
BOOL waitanim= FALSE;  //warten bis animation zuende?
BOOL highscore= FALSE;
BOOL undo= FALSE;      //ist Undo möglich?
int number_bubbles_marked= 0; //wieviele Bubbles sind angewählt?
int game_type= GT_CONTINUOUS; //gewählter Spieltyp
int score;
int bubble_points;
BOOL show_game_score= FALSE;
sSCORE_STATISTIC score_statistic;
sGAME_SCORES game_score;
sSETUP_DATA setup_data;
RECT PlayField_Rect;

sBUBBLE_FIELD PlayField[10][7];      //aktuelles Spielfeld
sBUBBLE_FIELD Dummy_PlayField[10][7];  
int undo_bubble_types[10][7];        //Bubble-Typen von vorherigem Zug für Undo-Funktion
int undo_score;							 //Score die bei Undo wieder abgezogen werden muß...

sBUTTON buttons[20];
int button_ctr= 0;  // Button-Index ab 1!
int pressed_button= 0; // Wenn ein Button gedrückt wird, wird er hier verzeichnet
							  // 0= keiner gedrückt

// Strings für Text-Ausgabe in diversen Modies...
LPCSTR info1,info2,info3,info4,info5,info6,info7,info8,
		 info9,info10,info11,info12,info13,info14,info15,
		 info16,info17,info18,info19,info20,info21,info22,info23,
		 info24,info25,info26,info27,info28,info29,info30;

extern LPDIRECTDRAW         dd_Object;    // DirectDraw object
extern Surface              dd_Primary;   // DirectDraw primary surface
extern LPDIRECTDRAWCLIPPER  dd_Clipper;   // DirectDraw Clipper
extern BOOL                 bActive;      // is application active?
extern Surface              v_screen;     // Virtual Screen
extern POINT                Client_Pos;   // Position des Fenster-Client
extern HWND                 main_hwnd;    // Window-Handle
extern UINT                 TIMER_ID;     // Timer ID
extern BOOL                 game_initialized; // Fertig initialisiert?

void Game_Init();
void Game_Message(UINT msg, WPARAM wParam, LPARAM lParam);
void Game_Shutdown();
void Update_Screen();

