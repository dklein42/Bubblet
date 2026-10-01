/*
===========================================================================
	game.cpp
	Main-Game-Code
	coded by: Daniel Klein
	(c) 1998/99 House of Bytes Software
===========================================================================
*/
#include "game.h"
#include "mci.h"

BOOL Check_In_Box(POINT topleft, POINT bottomright, UINT xPos, UINT yPos)
{
  if(xPos > topleft.x && xPos < bottomright.x)
  {
	 if(yPos > topleft.y && yPos < bottomright.y)
	 {
		return TRUE;
	 }
  }
  return FALSE;
}

// Nur durch pressed_button selektierten Button
void Update_Button(int used_button)
{
  if( (!main_btn_mask)|(used_button>=5) )
  if( (buttons[used_button].mode==mode)|
		(buttons[used_button].mode==ALWAYS) )
  {
	 if(buttons[used_button].pressed)
	 {
		PutSurfacePrimary(buttons[used_button].topleft.x,
								buttons[used_button].topleft.y,
								buttons[used_button].bmp_pressed);
	 } else
	 {
		PutSurfacePrimary(buttons[used_button].topleft.x,
								buttons[used_button].topleft.y,
								buttons[used_button].bmp_normal);
	 }
  }
}

// Alle Buttons auf vscreen
void Draw_Buttons()
{
  for(int i= 1; i <= button_ctr; i++)
  {
	 if( (buttons[i].mode==mode)|(buttons[i].mode== ALWAYS) )
	 {
		if(buttons[i].pressed)
		{
		  PutSurface(buttons[i].topleft.x, buttons[i].topleft.y,
						 buttons[i].bmp_pressed, v_screen);
		} else
		{
		  PutSurface(buttons[i].topleft.x, buttons[i].topleft.y,
						 buttons[i].bmp_normal, v_screen);
		}
	 }
  }

  //wenn vorhergehender modus = GAME dann new-button manuell auf 'end' überschreiben
  if(last_mode!= 0)                          //buttons nicht checken da eh maskiert!
  {
	 if( (last_mode== GAME)&(mode!= IDLE) )
	 {
		PutSurface(buttons[6].topleft.x, buttons[6].topleft.y,
					  buttons[6].bmp_normal, v_screen);
	 }
  }

  if(main_btn_mask)
  {
	 PutSurfaceCC(buttons[1].topleft.x, buttons[1].topleft.y, btnmask, v_screen);
	 PutSurfaceCC(buttons[2].topleft.x, buttons[2].topleft.y, btnmask, v_screen);
	 PutSurfaceCC(buttons[3].topleft.x, buttons[3].topleft.y, btnmask, v_screen);
	 PutSurfaceCC(buttons[4].topleft.x, buttons[4].topleft.y, btnmask, v_screen);
	 PutSurfaceCC(buttons[5].topleft.x, buttons[5].topleft.y, btnmask, v_screen);
	 PutSurfaceCC(buttons[6].topleft.x, buttons[6].topleft.y, btnmask, v_screen);
  }
}

void Update_Screen()
{
  PutSurfacePrimary(0, 0, v_screen);
}

//Erstellt Bildschirminhalt im Spiel-Modus (incl. Spielfeld, Score etc.)
void Create_Game_Screen()
{
  int i,j;
  int x,y;
  int x1,y1,x2,y2;
  int dx, dy;
  Surface* bubble_pic_ptr;
  char buffer[80];

  PutSurface(0, 0, bk_pic, v_screen); // Hintergrundbild
  //Status-Zeile
  sprintf(buffer, "Score: %u  Bubble-Points: %u   ---   %u Bubbles selected.",
			 game_score.score, bubble_points, number_bubbles_marked);
  Status_Text= buffer;
  ShowText(v_screen, 10, 352, Status_Text, RGB(255,255,255)); // Status-Zeile
  Draw_Buttons(); // Buttons

  //PlayField
  for(j=0;j<=6;j++)
  {
	 for(i=0;i<=9;i++)
	 {
		if(PlayField[i][j].type!= 0)
		{
		  //Bubbles setzen!
		  x= PLAYFIELD_START_X+48*i;
		  y= PLAYFIELD_START_Y+48*j;

		  if(game_busy)
		  {
			 if(PlayField[i][j].move_pos.x!= 0xFFFF)
			 {
				x= PlayField[i][j].move_pos.x;
			 }

			 if(PlayField[i][j].move_pos.y!= 0xFFFF)
			 {
				y= PlayField[i][j].move_pos.y;
			 }
		  }

		  if(PlayField[i][j].type== 1) bubble_pic_ptr= &bubble_1;
		  if(PlayField[i][j].type== 2) bubble_pic_ptr= &bubble_2;
		  if(PlayField[i][j].type== 3) bubble_pic_ptr= &bubble_3;
		  if(PlayField[i][j].type== 4) bubble_pic_ptr= &bubble_4;
		  if(PlayField[i][j].type== 5) bubble_pic_ptr= &bubble_5;
		  PutSurfaceClipCC(x, y, *bubble_pic_ptr, v_screen, PlayField_Rect);

		  //Markierungen setzen!
		  if(bubbles_marked)
		  {
			 if(PlayField[i][j].marked)
			 {
				if( (j==0) | ((j>0)&(!PlayField[i][j-1].marked)) ) //Top
				{
				  x1= PLAYFIELD_START_X+(48*i)-1;
				  y1= PLAYFIELD_START_Y+(48*j)-1;
				  x2= PLAYFIELD_START_X+(48*(i+1))-1;
				  y2= PLAYFIELD_START_Y+(48*j)-1;
				  DrawLine(v_screen, x1, y1, x2, y2, RGB(0xFF,0x0,0x0));
				}
				if( (j==6) | ((j<6)&(!PlayField[i][j+1].marked)) ) //Bottom
				{
				  x1= PLAYFIELD_START_X+(48*i)-1;
				  y1= PLAYFIELD_START_Y+(48*(j+1))-2;
				  x2= PLAYFIELD_START_X+(48*(i+1))-1;
				  y2= PLAYFIELD_START_Y+(48*(j+1))-2;
				  DrawLine(v_screen, x1, y1, x2, y2, RGB(0xFF,0x0,0x0));
				}
				if( (i==0) | ((i>0)&(!PlayField[i-1][j].marked)) ) //Left
				{
				  x1= PLAYFIELD_START_X+(48*i)-1;
				  y1= PLAYFIELD_START_Y+(48*j)-1;
				  x2= PLAYFIELD_START_X+(48*i)-1;
				  y2= PLAYFIELD_START_Y+(48*(j+1))-1;
				  DrawLine(v_screen, x1, y1, x2, y2, RGB(0xFF,0x0,0x0));
				}
				if( (i==9) | ((i<9)&(!PlayField[i+1][j].marked)) ) //Right
				{
				  x1= PLAYFIELD_START_X+(48*(i+1))-2;
				  y1= PLAYFIELD_START_Y+(48*j)-1;
				  x2= PLAYFIELD_START_X+(48*(i+1))-2;
				  y2= PLAYFIELD_START_Y+(48*(j+1))-1;
				  DrawLine(v_screen, x1, y1, x2, y2, RGB(0xFF,0x0,0x0));
				}
			 }
		  }
		}
	 }
  } //PlayField

  Update_Screen(); // Und raus damit
}

void Create_Scores()
{
  char buffer1[80],buffer2[80],buffer3[80],buffer4[80],buffer5[80],buffer6[80],
		 buffer7[80],buffer8[80],buffer9[80],buffer10[80],buffer11[80],buffer12[80];
  int average;

  PutSurface(0, 0, bk_pic, v_screen); // Hintergrundbild
  PutSurfaceCC(8, 6, paper_pic, v_screen); // Papier
  PutSurfaceCC(75, 40, score_table_pic, v_screen); // Tabelle

  if(score_statistic.normal.games > 0)
  {
	 average= score_statistic.normal.scores/score_statistic.normal.games;
  } else
  {
	 average= score_statistic.normal.scores;
  }

  sprintf(buffer1, "%u", score_statistic.normal.games);
  info1= buffer1;
  sprintf(buffer2, "%u", average);
  info2= buffer2;
  sprintf(buffer3, "%u", score_statistic.normal.high);
  info3= buffer3;

  if(score_statistic.continuous.games > 0)
  {
	 average= score_statistic.continuous.scores/score_statistic.continuous.games;
  } else
  {
	 average= score_statistic.continuous.scores;
  }

  sprintf(buffer4, "%u", score_statistic.continuous.games);
  info4= buffer4;
  sprintf(buffer5, "%u", average);
  info5= buffer5;
  sprintf(buffer6, "%u", score_statistic.continuous.high);
  info6= buffer6;

  if(score_statistic.shifter.games > 0)
  {
	 average= score_statistic.shifter.scores/score_statistic.shifter.games;
  } else
  {
	 average= score_statistic.shifter.scores;
  }

  sprintf(buffer7, "%u", score_statistic.shifter.games);
  info7= buffer7;
  sprintf(buffer8, "%u", average);
  info8= buffer8;
  sprintf(buffer9, "%u", score_statistic.shifter.high);
  info9= buffer9;

  if(score_statistic.megashift.games > 0)
  {
	 average= score_statistic.megashift.scores/score_statistic.megashift.games;
  } else
  {
	 average= score_statistic.megashift.scores;
  }

  sprintf(buffer10, "%u", score_statistic.megashift.games);
  info10= buffer10;
  sprintf(buffer11, "%u", average);
  info11= buffer11;
  sprintf(buffer12, "%u", score_statistic.megashift.high);
  info12= buffer12;

  ShowTextCenter(v_screen, 240, 135, info1, RGB(000,000,000));
  ShowTextCenter(v_screen, 315, 135, info2, RGB(000,000,000));
  ShowTextCenter(v_screen, 385, 135, info3, RGB(000,000,000));

  ShowTextCenter(v_screen, 240, 167, info4, RGB(000,000,000));
  ShowTextCenter(v_screen, 315, 167, info5, RGB(000,000,000));
  ShowTextCenter(v_screen, 385, 167, info6, RGB(000,000,000));

  ShowTextCenter(v_screen, 240, 197, info7, RGB(000,000,000));
  ShowTextCenter(v_screen, 315, 197, info8, RGB(000,000,000));
  ShowTextCenter(v_screen, 385, 197, info9, RGB(000,000,000));

  ShowTextCenter(v_screen, 240, 229, info10, RGB(000,000,000));
  ShowTextCenter(v_screen, 315, 229, info11, RGB(000,000,000));
  ShowTextCenter(v_screen, 385, 229, info12, RGB(000,000,000));

  Status_Text= "Click 'Reset' to clear the damn thing...";
  ShowText(v_screen, 10, 352, Status_Text, RGB(255,255,255)); // Status-Zeile
  Draw_Buttons(); // Buttons
  Update_Screen(); // Und raus damit
}

void Create_Game_Score()
{
  char buffer1[80],buffer2[80],buffer3[80],buffer4[80],buffer5[80],buffer6[80];
  int average;

  PutSurface(0, 0, bk_pic, v_screen); // Hintergrundbild
  PutSurfaceCC(8, 6, paper_pic, v_screen); // Papier
  PutSurfaceCC(135, 40, game_score_table_pic, v_screen); // Tabelle

  sprintf(buffer1, "%u", game_score.score);
  info1= buffer1;
  sprintf(buffer2, "%u", game_score.bubble_bonus);
  info2= buffer2;
  sprintf(buffer3, "%u", game_score.score+game_score.bubble_bonus);
  info3= buffer3;

  if(game_type==GT_NORMAL)
  {
	 sprintf(buffer4, "%u",
				score_statistic.normal.scores/score_statistic.normal.games);
	 info4= buffer4;
	 sprintf(buffer5, "%u", score_statistic.normal.games);
	 info5= buffer5;
  }
  if(game_type==GT_CONTINUOUS)
  {
	 sprintf(buffer4, "%u",
				score_statistic.continuous.scores/score_statistic.continuous.games);
	 info4= buffer4;
	 sprintf(buffer5, "%u", score_statistic.continuous.games);
	 info5= buffer5;
  }
  if(game_type==GT_SHIFTER)
  {
	 sprintf(buffer4, "%u",
				score_statistic.shifter.scores/score_statistic.shifter.games);
	 info4= buffer4;
	 sprintf(buffer5, "%u", score_statistic.shifter.games);
	 info5= buffer5;
  }
  if(game_type==GT_MEGASHIFT)
  {
	 sprintf(buffer4, "%u",
				score_statistic.megashift.scores/score_statistic.megashift.games);
	 info4= buffer4;
	 sprintf(buffer5, "%u", score_statistic.megashift.games);
	 info5= buffer5;
  }

  ShowTextCenter(v_screen, 335, 115, info1, RGB(000,000,000));
  ShowTextCenter(v_screen, 335, 146, info2, RGB(000,000,000));
  if(highscore)
  {
	 ShowTextCenter(v_screen, 335, 176, info3, RGB(255,000,000));
  }
  else
  {
	 ShowTextCenter(v_screen, 335, 176, info3, RGB(000,000,000));
  }
  ShowTextCenter(v_screen, 335, 208, info4, RGB(000,000,000));
  ShowTextCenter(v_screen, 335, 238, info5, RGB(000,000,000));

  if(highscore)
  {
	 Status_Text= "You have a new highscore!!";
  } else
  {
	 Status_Text= "Game Over!";
  }
  ShowText(v_screen, 10, 352, Status_Text, RGB(255,255,255)); // Status-Zeile
  Draw_Buttons(); // Buttons
  Update_Screen(); // Und raus damit
}

void CreateSetup()
{
  PutSurface(0, 0, bk_pic, v_screen); // Hintergrundbild
  PutSurfaceCC(8, 6, paper_pic, v_screen); // Papier-Hintergrund
  PutSurfaceCC(73, 65, setup_map_pic, v_screen); // Setup-Bild

  if(setup_data.game_type==GT_NORMAL)
						PutSurfaceCC(99, 97, checked_box_pic, v_screen); //Häkchen
  if(setup_data.game_type==GT_CONTINUOUS)
						PutSurfaceCC(99, 126, checked_box_pic, v_screen); //Häkchen
  if(setup_data.game_type==GT_SHIFTER)
						PutSurfaceCC(99, 159, checked_box_pic, v_screen); //Häkchen
  if(setup_data.game_type==GT_MEGASHIFT)
						PutSurfaceCC(99, 190, checked_box_pic, v_screen); //Häkchen

  if(setup_data.play_sounds==TRUE)
						PutSurfaceCC(279, 77, checked_box_pic, v_screen); //Häkchen
  if(setup_data.animations==TRUE)
						PutSurfaceCC(280, 119, checked_box_pic, v_screen); //Häkchen

  Status_Text= "Let's change something...";
  ShowText(v_screen, 10, 352, Status_Text, RGB(255,255,255)); // Status-Zeile
  Draw_Buttons(); // Buttons
  Update_Screen(); // Und raus damit
}

void Create_Screen()
{
  switch(mode)
  {
	 case IDLE:
		PutSurface(0, 0, bk_pic, v_screen); // Hintergrundbild
		PutSurfaceCC(20, 50, pic, v_screen); // HOB Logo

#ifdef REGISTERED_BUILD
		info1= "REGISTERED-VERSION";

		info2= "For the actual version of Bubblet visit our Homepage:";
		info3= "www.hobsoft.de";

		info4= "Please send comments and questions to:";
		info5= "d.klein@hobsoft.de";

		info6= "Thank you for registering Bubblet!";
		info7= "Please do NOT distribute it! Thanks.";
#else
		info1= "SHAREWARE-VERSION";

		info2= "This is the Shareware-Version of Bubblet.";
		info3= "Feel free to distribute it how ever you want!";

		info4= "For the actual version of Bubblet visit our Homepage:";
		info5= "www.hobsoft.de";

		info6= "Please support the concept of Shareware and register Bubblet!";
		info7= "";
#endif

		ShowTextCenter(v_screen, 400, 158, info1, RGB(0xFF,0xFF,0xFF));

		ShowTextCenter(v_screen, 250, 200, info2, RGB(0xFF,0xFF,0xFF));
		ShowTextCenter(v_screen, 250, 215, info3, RGB(0xFF,0xFF,0xFF));

		ShowTextCenter(v_screen, 250, 245, info4, RGB(0xFF,0xFF,0xFF));
		ShowTextCenter(v_screen, 250, 260, info5, RGB(0xFF,0xFF,0xFF));

		ShowTextCenter(v_screen, 250, 295, info6, RGB(0xFF,0xFF,0xFF));
		ShowTextCenter(v_screen, 250, 310, info7, RGB(0xFF,0xFF,0xFF));

		Status_Text= "Click 'New' to start a new game!";
		ShowText(v_screen, 10, 352, Status_Text, RGB(255,255,255)); // Status-Zeile
		Draw_Buttons(); // Buttons

		Update_Screen(); // Und raus damit
		break;

	 case INTRO:
		break;

	 case GAME:
		Create_Game_Screen();
		break;

	 case ABOUT:
		info1= "Bubblet v1.1";
		info2= "----------------------------------------------------------------------------------------------------";
		info3= "Programming: Daniel Klein";
		info4= "Copyright (c) 1999-2000 by House of Bytes Software";
		info5= "All rights reserved.";
		info6= "----------------------------------------------------------------------------------------------------";
		info7= "Testers: Martin Boese, Markus Jakubasch, Christian Heinen";
		info8= "Original game created by Frank Fejes (www.oopdreams.com)";

		info20= "Greetings to:";
		info21= "Heidi Amling, Daniel Gerber, Peter Gladigau,";
		info22= "Alexander Hegener, Jörg Heil, Sebastian Jude,";
		info23= "Tobias Pohl, Hendrik Pütz, Tyrell, Niko, Björn, Nina,";
		info24= "Eva and Peter Weber -- hey-ho Holland! :-)";

		PutSurface(0, 0, bk_pic, v_screen); // Hintergrundbild
		PutSurfaceCC(8, 6, paper_x_pic, v_screen); // Papier

		ShowText(v_screen, 50, 40, info1, RGB(0,0,0));
		ShowText(v_screen, 50, 55, info2, RGB(0,0,0));
		ShowText(v_screen, 50, 70, info3, RGB(0,0,0));
		ShowText(v_screen, 50, 85, info4, RGB(0,0,0));
		ShowText(v_screen, 50, 100, info5, RGB(0,0,0));
		ShowText(v_screen, 50, 115, info6, RGB(0,0,0));
		ShowText(v_screen, 50, 130, info7, RGB(0,0,0));
		ShowText(v_screen, 50, 145, info8, RGB(0,0,0));

		ShowText(v_screen, 50, 190, info20, RGB(0,0,0));
		ShowText(v_screen, 50, 205, info21, RGB(0,0,0));
		ShowText(v_screen, 50, 220, info22, RGB(0,0,0));
		ShowText(v_screen, 50, 235, info23, RGB(0,0,0));
		ShowText(v_screen, 50, 250, info24, RGB(0,0,0));

		Status_Text= "The creators of Bubblet...";
		ShowText(v_screen, 10, 352, Status_Text, RGB(255,255,255)); // Status-Zeile
		Draw_Buttons(); // Buttons
		Update_Screen(); // Und raus damit
		break;

	 case SETUP:
		CreateSetup();
		break;

	 case SCORES:
		Create_Scores();
		break;

	 case SHAREWAREINFO:
		PutSurface(0, 0, bk_pic, v_screen); // Hintergrundbild

		info1= "";
		info2= "";
		info3= "";
		info4= "";
		info5= "";
		info6= "Undo is the only thing that's not working";
		info7= "in the Shareware-Version of Bubblet! ;-)";
		info8= "";
		info9= "Please Register!";
		info10="";
		info11="";
		info12="";

		info20= "";
		info21= "";
		info22= "";
		info23= "";

		PutSurface(0, 0, bk_pic, v_screen); // Hintergrundbild
		PutSurfaceCC(8, 6, paper_x_pic, v_screen); // Papier

		ShowTextCenter(v_screen, 250, 40, info1, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 55, info2, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 60, info3, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 75, info4, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 90, info5, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 105, info6, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 120, info7, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 135, info8, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 150, info9, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 165, info10, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 180, info11, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 195, info12, RGB(0,0,0));

		ShowTextCenter(v_screen, 250, 215, info20, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 230, info21, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 245, info22, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 260, info23, RGB(0,0,0));

		Status_Text= "hehe...";
		ShowText(v_screen, 10, 352, Status_Text, RGB(255,255,255)); // Status-Zeile
		Draw_Buttons(); // Buttons
		Update_Screen(); // Und raus damit
		break;

	 case REGISTER:
#ifdef REGISTERED_BUILD
		info1= "";
		info2= "";
		info3= "";
		info4= "";
		info5= "";
		info6= "";
		info7= "Thank you for registering Bubblet!";
		info8= "";
		info9= "";
		info10="";
		info11="";
		info12="";

		info20= "";
		info21= "";
		info22= "";
		info23= "";
#else
		info1= "Please register Bubblet!";
		info2= "";
		info3= "Bubblet is Shareware and costs only $8/EUR8/DM15!";
		info4= "";
		info5= "Register Bubblet online @ our homepage:";
		info6= "www.hobsoft.de";
		info7= "";
		info8= "Pay via Credit Card, Check or Bank Transfer";
		info9= "(For Germany: Auch per Bankabbuchung!)";
		info10="";
		info11="For further details please refer to the enclosed";
		info12="'readme.txt'!";

		info20= "";
		info21= "Thank you for supporting the concept of Shareware!";
		info22= "";
		info23= "";
/* old one... ;-)
		info1= "Bubblet is Shareware and costs only $10 (15DM).";
		info2= "";
		info3= "In order to register Bubblet send money (or Money-Order) to:";
		info4= "-----------------------------------------";
		info5= "House of Bytes Software";
		info6= "c/o Daniel Klein";
		info7= "Walzenrather Str. 5";
		info8= "53819 Neunkirchen";
		info9= "Germany";
		info10="-----------------------------------------";
		info11="Please include your E-Mail Address!";
		info12="The registered Version will be sent to you via E-Mail!";

		info20= "Once registered, you're registered forever!";
		info21= "All registered Users receive future updates directly via E-Mail!";
		info22= "";
		info23= "Thank you for supporting the concept of Shareware!";
*/
#endif

		PutSurface(0, 0, bk_pic, v_screen); // Hintergrundbild
		PutSurfaceCC(8, 6, paper_x_pic, v_screen); // Papier

		ShowTextCenter(v_screen, 250, 40, info1, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 55, info2, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 60, info3, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 75, info4, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 90, info5, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 105, info6, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 120, info7, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 135, info8, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 150, info9, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 165, info10, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 180, info11, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 195, info12, RGB(0,0,0));

		ShowTextCenter(v_screen, 250, 215, info20, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 230, info21, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 245, info22, RGB(0,0,0));
		ShowTextCenter(v_screen, 250, 260, info23, RGB(0,0,0));

		Status_Text= "How to register Bubblet...";
		ShowText(v_screen, 10, 352, Status_Text, RGB(255,255,255)); // Status-Zeile
		Draw_Buttons(); // Buttons
		Update_Screen(); // Und raus damit
		break;

	 case HELP:
		PutSurface(0, 0, bk_pic, v_screen); // Hintergrundbild
		Status_Text= "Mode: HELP";
		ShowText(v_screen, 10, 352, Status_Text, RGB(255,255,255)); // Status-Zeile
		Draw_Buttons(); // Buttons
		Update_Screen(); // Und raus damit
		break;

	 case GAME_SCORE:
		Create_Game_Score();
		break;
  }
}

void New_Game()
{
  int i,j;

  //Game-Type setzen
  game_type= setup_data.game_type;

  //Game_Score zurücksetzen
  game_score.score= 0;
  game_score.bubble_bonus= 0;

  //Spielfeld initialisieren!
  for(j=0;j<=6;j++)
  {
	 for(i=0;i<=9;i++)
	 {
		PlayField[i][j].type= random(5)+1;
		PlayField[i][j].move_pos.x= 0xFFFF;
		PlayField[i][j].move_pos.y= 0xFFFF;
		PlayField[i][j].marked= FALSE;
	 }
  }
}

void change_mode(int new_mode)
{
  int last_mode_buffer= last_mode;
  last_mode= mode;
  switch(new_mode)
  {
	 case IDLE:
		mode= IDLE;      // Mode umschalten
		main_btn_mask= FALSE;
		Create_Screen(); // ...und aktualisieren!
		break;

	 case INTRO:
		break;

	 case GAME:
		mode= GAME;      // Mode umschalten
		main_btn_mask= FALSE;
		New_Game();
		Create_Screen(); // ...und aktualisieren!
		break;

	 case ABOUT:
		mode= ABOUT;     // Mode umschalten
		main_btn_mask= TRUE;
		Create_Screen(); // ...und aktualisieren!
		break;

	 case SETUP:
		mode= SETUP;     // Mode umschalten
		main_btn_mask= TRUE;
		Create_Screen(); // ...und aktualisieren!
		break;

	 case SCORES:
		mode= SCORES;    // Mode umschalten
		main_btn_mask= TRUE;
		Create_Screen(); // ...und aktualisieren!
		break;

	 case SHAREWAREINFO:
		mode= SHAREWAREINFO;   //Mode umschalten
		main_btn_mask= TRUE;
		Create_Screen();
		break;

	 case REGISTER:
		mode= REGISTER;   //Mode umschalten
		main_btn_mask= TRUE;
		Create_Screen();
		break;

	 case HELP:
		//main_btn_mask= TRUE;
		break;

	 case GAME_SCORE:
		mode= GAME_SCORE;
		main_btn_mask= TRUE;
		if(highscore)
		{
			if(setup_data.play_sounds) mci_PlayWaveWait(A_HIGHSCORE);
		}
		else
		{
		  if(setup_data.play_sounds) mci_PlayWaveWait(A_SCORE);
		}
		Create_Screen(); // ...und aktualisieren!
		break;

	 case OLDMODE: // in vorherigen modus zurück schalten!
		mode= last_mode_buffer;
		main_btn_mask= FALSE;
		Create_Screen(); // ...und aktualisieren!
		last_mode= 0;
		break;
  }
}

//Rekursive Funktion zum Markierungsdurchlauf...
int Check_Bubble(int x, int y, int type)
{
  int n= 0;
  int r;

  if(/*PlayField[x][y].*/type==0) return 0; //Ungültiger Aufruf

  if( (PlayField[x][y].type==type)&(PlayField[x][y].marked==FALSE) )
  {
	 PlayField[x][y].marked= TRUE;
	 n++;

	 if(y>0)   //Top
	 {
		r= Check_Bubble(x, y-1, type);
		n= n+r;
	 }

	 if(y<6)   //Bottom
	 {
		r= Check_Bubble(x, y+1, type);
		n= n+r;
	 }

	 if(x>0)   //Left
	 {
		r= Check_Bubble(x-1, y, type);
		n= n+r;
	 }

	 if(x<9)   //Right
	 {
		r= Check_Bubble(x+1, y, type);
		n+= r;
	 }
  }
  return n;
}

BOOL Check_Game_Over()
{
  int i,j,n;

  for(j=0;j<=6;j++)
  {
	 for(i=0;i<=9;i++)
	 {
		n= Check_Bubble(i, j, PlayField[i][j].type);
		if(n>=2) return FALSE;
	 }
  }
  return TRUE; //Game Over!
}

//ist Reihe r leer?
BOOL Check_Free_Row(int r)
{
  if((PlayField[r][0].type==0)&(PlayField[r][1].type==0)&
	  (PlayField[r][2].type==0)&(PlayField[r][3].type==0)&
	  (PlayField[r][4].type==0)&(PlayField[r][5].type==0)&
	  (PlayField[r][6].type==0)) return TRUE; else return FALSE;
}

//Füllt leere Reihe (wird vom GT_CONTINUOUS benutzt)
void Fill_Rows(int r)
{
  int i, j;

  for(i=r;i>=0;i--)
  {
	 for(j=0;j<=6;j++)
	 {
		PlayField[i][j].type= random(5)+1;
		PlayField[i][j].move_pos.x= PLAYFIELD_START_X-(48*(r-i+1));
		PlayField[i][j].move_pos.y= 0xFFFF /*336+(j*48)*/;
		PlayField[i][j].marked= FALSE;
	 }
  }
}

//                  (Destination/Source)
void Move_Bubble(int x1,int y1,int x2,int y2)
{
  PlayField[x1][y1].type=       PlayField[x2][y2].type;
  PlayField[x1][y1].move_pos.x= PlayField[x2][y2].move_pos.x;
  PlayField[x1][y1].move_pos.y= PlayField[x2][y2].move_pos.y;
  PlayField[x1][y1].old_pos.x=  PlayField[x2][y2].old_pos.x;
  PlayField[x1][y1].old_pos.y=  PlayField[x2][y2].old_pos.y;
  PlayField[x1][y1].marked=     PlayField[x2][y2].marked;

  PlayField[x2][y2].type= 0;
  PlayField[x2][y2].move_pos.x= 0xFFFF;
  PlayField[x2][y2].move_pos.y= 0xFFFF;
  PlayField[x2][y2].old_pos.x= -1;
  PlayField[x2][y2].old_pos.y= -1;
  PlayField[x2][y2].marked= FALSE;
}

//Zählt die vorhandenen Bubbles
int Count_Bubbles()
{
  int i, j, ctr;

  ctr= 0;
  for(j=0;j<=6;j++)
  {
	 for(i=0;i<=9;i++)
	 {
		if(PlayField[i][j].type!= 0) ctr++;
	 }
  }
  return ctr;
}

//gewählte Bubbles werden hier entfernt, und das restliche Spielfeld aktualisiert
void Update_Bubbles()
{
  int i,j,ctr,pos;

  for(j=0;j<=6;j++)
  {
	 for(i=0;i<=9;i++)
	 {
		undo_bubble_types[i][j]= PlayField[i][j].type; //Zustand merken
		PlayField[i][j].old_pos.x= i;
		PlayField[i][j].old_pos.y= j;

		PlayField[i][j].move_pos.x= PLAYFIELD_START_X+48*i;
		PlayField[i][j].move_pos.y= PLAYFIELD_START_Y+48*j;
		if(PlayField[i][j].marked)
		{
		  PlayField[i][j].marked= FALSE;
		  PlayField[i][j].type= 0; //Bubble geplatzt ;-)
		  PlayField[i][j].move_pos.x= 0xFFFF;
		  PlayField[i][j].move_pos.y= 0xFFFF;
		}
	 }
  }

  if(game_type==GT_NORMAL) //------------------- GT_NORMAL --------------------
  {
	 for(i=9;i>=0;i--)
	 {
		for(ctr=0;ctr<=6;ctr++)
		{
		  if(PlayField[i][6].type==0)
		  {
			 Move_Bubble(i,6,i,5);
			 Move_Bubble(i,5,i,4);
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][5].type==0)
		  {
			 Move_Bubble(i,5,i,4);
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][4].type==0)
		  {
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][3].type==0)
		  {
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][2].type==0)
		  {
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][1].type==0)
		  {
			 Move_Bubble(i,1,i,0);
		  }
		}
	 }
	 //evtl. freihe Reihen schliessen
	 for(ctr=9;ctr>=1;ctr--) //das ganze *2 damit auch Mehrfachreihen fertig sind
	 {
		for(i=9;i>=1;i--)
		{
		  if(Check_Free_Row(i))
		  {
			 Move_Bubble(i,0,i-1,0);
			 Move_Bubble(i,1,i-1,1);
			 Move_Bubble(i,2,i-1,2);
			 Move_Bubble(i,3,i-1,3);
			 Move_Bubble(i,4,i-1,4);
			 Move_Bubble(i,5,i-1,5);
			 Move_Bubble(i,6,i-1,6);
		  }
		}
	 }
	 if(Check_Free_Row(0)) //unnötig????
	 {
		PlayField[0][0].type= 0;
		PlayField[0][1].type= 0;
		PlayField[0][2].type= 0;
		PlayField[0][3].type= 0;
		PlayField[0][4].type= 0;
		PlayField[0][5].type= 0;
		PlayField[0][6].type= 0;
	 }
  }

  if(game_type==GT_CONTINUOUS) //------------- GT_CONTINUOUS -----------------
  {
	 for(i=9;i>=0;i--)
	 {
		for(ctr=0;ctr<=6;ctr++)
		{
		  if(PlayField[i][6].type==0)
		  {
			 Move_Bubble(i,6,i,5);
			 Move_Bubble(i,5,i,4);
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][5].type==0)
		  {
			 Move_Bubble(i,5,i,4);
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][4].type==0)
		  {
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][3].type==0)
		  {
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][2].type==0)
		  {
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][1].type==0)
		  {
			 Move_Bubble(i,1,i,0);
		  }
		}
	 }
	 //evtl. freihe Reihen schliessen
	 for(ctr=9;ctr>=1;ctr--) //das ganze *2 damit auch Mehrfachreihen fertig sind
	 {
		for(i=9;i>=1;i--)
		{
		  if(Check_Free_Row(i))
		  {
			 Move_Bubble(i,0,i-1,0);
			 Move_Bubble(i,1,i-1,1);
			 Move_Bubble(i,2,i-1,2);
			 Move_Bubble(i,3,i-1,3);
			 Move_Bubble(i,4,i-1,4);
			 Move_Bubble(i,5,i-1,5);
			 Move_Bubble(i,6,i-1,6);
		  }
		}
	 }
	 for(i=9;i>=0;i--)
	 {
		if(Check_Free_Row(i))
		{
		 Fill_Rows(i);
		 break;
		}
	 }
  }

  if(game_type==GT_SHIFTER) //----------------------- SHIFTER -------------------------
  {
	 for(i=9;i>=0;i--)
	 {
		for(ctr=0;ctr<=6;ctr++)
		{
		  if(PlayField[i][6].type==0)
		  {
			 Move_Bubble(i,6,i,5);
			 Move_Bubble(i,5,i,4);
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][5].type==0)
		  {
			 Move_Bubble(i,5,i,4);
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][4].type==0)
		  {
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][3].type==0)
		  {
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][2].type==0)
		  {
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][1].type==0)
		  {
			 Move_Bubble(i,1,i,0);
		  }
		}
	 }
	 //evtl. freihe Reihen schliessen
	 for(ctr=9;ctr>=1;ctr--) //das ganze *2 damit auch Mehrfachreihen fertig sind
	 {
		for(i=9;i>=1;i--)
		{
		  if(Check_Free_Row(i))
		  {
			 Move_Bubble(i,0,i-1,0);
			 Move_Bubble(i,1,i-1,1);
			 Move_Bubble(i,2,i-1,2);
			 Move_Bubble(i,3,i-1,3);
			 Move_Bubble(i,4,i-1,4);
			 Move_Bubble(i,5,i-1,5);
			 Move_Bubble(i,6,i-1,6);
		  }
		}
	 }
	 if(Check_Free_Row(0)) //unnötig????
	 {
		PlayField[0][0].type= 0;
		PlayField[0][1].type= 0;
		PlayField[0][2].type= 0;
		PlayField[0][3].type= 0;
		PlayField[0][4].type= 0;
		PlayField[0][5].type= 0;
		PlayField[0][6].type= 0;
	 }
	 for(ctr=0;ctr<=9;ctr++)
	 {
		for(i=0;i<=8;i++)
		{
		  for(j=0;j<=6;j++)
		  {
			 if(PlayField[i+1][j].type==0) Move_Bubble(i+1, j, i, j);
		  }
		}
	 }
  }

  if(game_type==GT_MEGASHIFT) //---------------------- MEGASHIFT ----------------------
  {
	 for(i=9;i>=0;i--)
	 {
		for(ctr=0;ctr<=6;ctr++)
		{
		  if(PlayField[i][6].type==0)
		  {
			 Move_Bubble(i,6,i,5);
			 Move_Bubble(i,5,i,4);
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][5].type==0)
		  {
			 Move_Bubble(i,5,i,4);
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][4].type==0)
		  {
			 Move_Bubble(i,4,i,3);
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][3].type==0)
		  {
			 Move_Bubble(i,3,i,2);
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][2].type==0)
		  {
			 Move_Bubble(i,2,i,1);
			 Move_Bubble(i,1,i,0);
		  }
		  if(PlayField[i][1].type==0)
		  {
			 Move_Bubble(i,1,i,0);
		  }
		}
	 }
	 //evtl. freihe Reihen schliessen
	 for(ctr=9;ctr>=1;ctr--) //das ganze *2 damit auch Mehrfachreihen fertig sind
	 {
		for(i=9;i>=1;i--)
		{
		  if(Check_Free_Row(i))
		  {
			 Move_Bubble(i,0,i-1,0);
			 Move_Bubble(i,1,i-1,1);
			 Move_Bubble(i,2,i-1,2);
			 Move_Bubble(i,3,i-1,3);
			 Move_Bubble(i,4,i-1,4);
			 Move_Bubble(i,5,i-1,5);
			 Move_Bubble(i,6,i-1,6);
		  }
		}
	 }
	 for(i=9;i>=0;i--)
	 {
		if(Check_Free_Row(i))
		{
		 Fill_Rows(i);
		 break;
		}
	 }
	 for(ctr=0;ctr<=9;ctr++)
	 {
		for(i=0;i<=8;i++)
		{
		  for(j=0;j<=6;j++)
		  {
			 if(PlayField[i+1][j].type==0) Move_Bubble(i+1, j, i, j);
		  }
		}
	 }
  }

  bubbles_marked= FALSE;
  number_bubbles_marked= 0;
  game_score.score+= bubble_points; //Punkte zählen!
  undo_score= bubble_points;        //bubble-points für Undo sichern
  bubble_points= 0;
  if(setup_data.animations) game_busy= TRUE; //Animations-Flag! Wenn man es nicht setzt gibt es keine Animation!
  else game_busy= FALSE;
  Create_Screen();

  if(setup_data.play_sounds) mci_PlayWave(A_POP); //sfx pop bubbles

  if(Check_Game_Over())
  {
	 highscore= FALSE;
	 if(game_type==GT_NORMAL)
	 {
		if(Count_Bubbles()==4) game_score.bubble_bonus= 25;
		if(Count_Bubbles()==3) game_score.bubble_bonus= 50;
		if(Count_Bubbles()==2) game_score.bubble_bonus= 100;
		if(Count_Bubbles()==1) game_score.bubble_bonus= 200;
		if(Count_Bubbles()==0) game_score.bubble_bonus= 500;

		score_statistic.normal.games++;
		score_statistic.normal.scores+= game_score.score+game_score.bubble_bonus;
		if((game_score.score+game_score.bubble_bonus > score_statistic.normal.high))
		{
		  score_statistic.normal.high= game_score.score+game_score.bubble_bonus;
		  highscore= TRUE;
		}
	 }

	 if(game_type==GT_CONTINUOUS)
	 {
		score_statistic.continuous.games++;
		score_statistic.continuous.scores+= game_score.score+game_score.bubble_bonus;
		if((game_score.score+game_score.bubble_bonus > score_statistic.continuous.high))
		{
		  score_statistic.continuous.high= game_score.score+game_score.bubble_bonus;
		  highscore= TRUE;
		}
	 }

	 if(game_type==GT_SHIFTER)
	 {
		if(Count_Bubbles()==4) game_score.bubble_bonus= 25;
		if(Count_Bubbles()==3) game_score.bubble_bonus= 50;
		if(Count_Bubbles()==2) game_score.bubble_bonus= 100;
		if(Count_Bubbles()==1) game_score.bubble_bonus= 200;
		if(Count_Bubbles()==0) game_score.bubble_bonus= 500;

		score_statistic.shifter.games++;
		score_statistic.shifter.scores+= game_score.score+game_score.bubble_bonus;
		if((game_score.score+game_score.bubble_bonus > score_statistic.shifter.high))
		{
		  score_statistic.shifter.high= game_score.score+game_score.bubble_bonus;
		  highscore= TRUE;
		}
	 }

	 if(game_type==GT_MEGASHIFT)
	 {
		score_statistic.megashift.games++;
		score_statistic.megashift.scores+= game_score.score+game_score.bubble_bonus;
		if((game_score.score+game_score.bubble_bonus > score_statistic.megashift.high))
		{
		  score_statistic.megashift.high= game_score.score+game_score.bubble_bonus;
		  highscore= TRUE;
		}
	 }

	 show_game_score= TRUE;
	 if(game_busy== FALSE)
	 {
		change_mode(GAME_SCORE); //Spiel vorbei! Game_Scores anzeigen!
	 } else
	 {
		waitanim= TRUE;
	 }
  }

}

//Undo-Funktion
void Undo_Move()
{
  if( undo )   //geht Undo überhaupt?
  {
	 int i,j,ctr,pos;

	 for(j=0;j<=6;j++)
	 {
		for(i=0;i<=9;i++)
		{
		  Dummy_PlayField[i][j].type= 0;

		  Dummy_PlayField[PlayField[i][j].old_pos.x][PlayField[i][j].old_pos.y].type= PlayField[i][j].type;

		  Dummy_PlayField[PlayField[i][j].old_pos.x][PlayField[i][j].old_pos.y].move_pos.x= PLAYFIELD_START_X+48*i;
		  Dummy_PlayField[PlayField[i][j].old_pos.x][PlayField[i][j].old_pos.y].move_pos.y= PLAYFIELD_START_Y+48*j;

		  Dummy_PlayField[PlayField[i][j].old_pos.x][PlayField[i][j].old_pos.y].old_pos.x= -1;
		  Dummy_PlayField[PlayField[i][j].old_pos.x][PlayField[i][j].old_pos.y].old_pos.y= -1;

		  Dummy_PlayField[PlayField[i][j].old_pos.x][PlayField[i][j].old_pos.y].marked= FALSE;
		}
	 }
	 for(j=0;j<=6;j++)         //geplatzten Bubbles rekonstuieren....
	 {
		for(i=0;i<=9;i++)
		{
		  if( Dummy_PlayField[i][j].type== 0 )
		  {
			  Dummy_PlayField[i][j].type= undo_bubble_types[i][j];
			  Dummy_PlayField[i][j].move_pos.x= PLAYFIELD_START_X+48*i;
			  Dummy_PlayField[i][j].move_pos.y= PLAYFIELD_START_Y+48*j;
			  Dummy_PlayField[i][j].old_pos.x= -1;
			  Dummy_PlayField[i][j].old_pos.y= -1;
			  Dummy_PlayField[i][j].marked= FALSE;
		  }
		}
	 }
	 for(j=0;j<=6;j++)         //fertiges Dummy_PlayField in PlayField kopieren...
	 {
		for(i=0;i<=9;i++)
		{
		  PlayField[i][j].type= Dummy_PlayField[i][j].type;
		  PlayField[i][j].move_pos.x= Dummy_PlayField[i][j].move_pos.x;
		  PlayField[i][j].move_pos.y= Dummy_PlayField[i][j].move_pos.y;
		  PlayField[i][j].old_pos.x= Dummy_PlayField[i][j].old_pos.x;
		  PlayField[i][j].old_pos.y= Dummy_PlayField[i][j].old_pos.y;
		  PlayField[i][j].marked= Dummy_PlayField[i][j].marked;
		}
	 }
	 game_score.score-= undo_score;
	 undo_score= 0;
	 undo= FALSE;
	 game_busy= TRUE;  //animationsflag setzen!
	 //Update_Bubbles(); //...und raus damit!
	 Create_Screen();
  }
}

//Welcher Bubble wurde angeklickt (bzw wurde abgewählt?)
void Check_Bubbles_Clicked(int xPos, int yPos)
{
  int x, y;
  int n;
  int i, j;

  x= floor( (xPos-8)/48 );
  y= floor( (yPos-8)/48 );

  if(bubbles_marked)
  {
	 if(PlayField[x][y].marked)
	 {
		Update_Bubbles();
		undo= TRUE;
		return;
	 }
  }

  for(j=0;j<=6;j++) //evtl. alte Marker alle auf 0 setzen
  {
	 for(i=0;i<=9;i++)
	 {
		PlayField[i][j].marked= FALSE;
	 }
  }

  if(!bubbles_marked)
  {
	 n= Check_Bubble(x, y, PlayField[x][y].type); //Rekursiver Test welche Bubbles markiert werden müssen
	 if(n>=2)
	 {
		bubbles_marked= TRUE;
		number_bubbles_marked= n;
		bubble_points= n*(n-1); //Punkte für angewählte Gruppe!
	 }
  } else
  {       //zurück setzen
	 bubbles_marked= FALSE;
	 number_bubbles_marked= 0;
	 bubble_points= 0;
  }
  Create_Screen();
}

BOOL check_all_bubbles_moved()
{
  int i,j;
  int x,y;

  for(j=0;j<=6;j++)
  {
	 for(i=0;i<=9;i++)
	 {
		if( ((PlayField[i][j].move_pos.x!= 0xFFFF)|(PlayField[i][j].move_pos.y!= 0xFFFF)) )
		{
		  return FALSE;
		}
	 }
  }
  return TRUE;
}

//wird durch Timer aufgerufen. Animiert (wenn nötig) die Bubbles...
void CALLBACK animate(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
  int i,j;
  int x,y;

  for(j=0;j<=6;j++)
  {
	 for(i=0;i<=9;i++)
	 {
		x= PLAYFIELD_START_X+48*i;
		y= PLAYFIELD_START_Y+48*j;

		if(PlayField[i][j].move_pos.x== x) PlayField[i][j].move_pos.x= 0xFFFF;
		if(PlayField[i][j].move_pos.y== y) PlayField[i][j].move_pos.y= 0xFFFF;

		if(PlayField[i][j].move_pos.x!= 0xFFFF)
		{
		  if(PlayField[i][j].move_pos.x < x)
		  {
			 PlayField[i][j].move_pos.x+= 8;
		  } else
		  {
			 PlayField[i][j].move_pos.x-= 8;
		  }
		}

		if(PlayField[i][j].move_pos.y!= 0xFFFF)
		{
		  if(PlayField[i][j].move_pos.y < y)
		  {
			 PlayField[i][j].move_pos.y+= 8;
		  } else
		  {
			 PlayField[i][j].move_pos.y-= 8;
		  }
		}
	 }
  }
  if(check_all_bubbles_moved()) //fertig?
  {
	 game_busy= FALSE;
	 if(waitanim)
	 {
		PostMessage(main_hwnd, WM_ENDGAME, 0, 0);
		waitanim= FALSE;
	 }
  }
  if(game_busy) Create_Screen();
}

void Reset_Scores()
{
  if(MessageBox( 0, "Are you sure?", "Reset",
		MB_OKCANCEL|MB_SETFOREGROUND|MB_SYSTEMMODAL)==IDOK)
  {
	 score_statistic.normal.games= 0;
	 score_statistic.normal.scores= 0;
	 score_statistic.normal.high= 0;

	 score_statistic.continuous.games= 0;
	 score_statistic.continuous.scores= 0;
	 score_statistic.continuous.high= 0;

	 score_statistic.shifter.games= 0;
	 score_statistic.shifter.scores= 0;
	 score_statistic.shifter.high= 0;

	 score_statistic.megashift.games= 0;
	 score_statistic.megashift.scores= 0;
	 score_statistic.megashift.high= 0;
  }

  Create_Screen();
}

void Execute_Button(int used_button)
{
  //Beep(1000, 100); click-notification
  switch(used_button)
  {
	 case 1: //Exit
		#ifdef REGISTERED_BUILD
		PostMessage(main_hwnd, WM_CLOSE, 0, 0);
		#else
		change_mode(REGISTER);
		#endif
		break;

	 case 2: //About
		change_mode(ABOUT);
		last_mode_2= last_mode;
		break;

	 case 3: //Setup
		change_mode(SETUP);
		break;

	 case 4: //Scores
		change_mode(SCORES);
		break;

	 case 5: //New
		change_mode(GAME);
		break;

	 case 6: //End
		if(game_type==GT_NORMAL)
		{
		  score_statistic.normal.games++;
		}
		if(game_type==GT_CONTINUOUS)
		{
		  score_statistic.continuous.games++;
		}
		if(game_type==GT_SHIFTER)
		{
		  score_statistic.shifter.games++;
		}
		if(game_type==GT_MEGASHIFT)
		{
		  score_statistic.megashift.games++;
		}
		highscore= FALSE; //abgebrochen! -> keine Highscore
		change_mode(GAME_SCORE);
		break;

	 case 7: //OK (Setup)
		change_mode(OLDMODE);
		break;

	 case 8: //OK (About)
		change_mode(OLDMODE);
		break;

	 case 9: //OK (Scores)
		change_mode(OLDMODE);
		show_game_score= FALSE;
		break;

	 case 10: //Reset (Scores)
		Reset_Scores();
		break;

	 case 11: //Done (Game_Scores)
		change_mode(IDLE);
		break;

	 case 12: //New (Game_Scores)
		change_mode(GAME);
		break;

	 case 13: //Register (About)
		change_mode(REGISTER);
		break;

	 case 14: //Undo (Game)
		#ifdef REGISTERED_BUILD
		Undo_Move();
		#else
      change_mode(SHAREWAREINFO);
		#endif
		break;

	 case 15: //OK (Register)
		if( last_mode == ABOUT )
		{
			change_mode(ABOUT);
			last_mode= last_mode_2;
		} else
		{
			PostMessage(main_hwnd, WM_CLOSE, 0, 0);
		}
		break;

	 case 16: //OK (SharewareInfo)
		change_mode(OLDMODE);
		break;
  }
}

void Check_Button_Down(UINT xPos, UINT yPos)
{
  POINT p1, p2;

  //Buttons
  for(int i= 1; i <= button_ctr; i++)
  {
	 if(Check_In_Box(buttons[i].topleft, buttons[i].bottomright, xPos, yPos))
	 {
		if(buttons[i].mode==mode|buttons[i].mode== ALWAYS)
		{
		  if((!main_btn_mask)|(i>=5))
		  {
			 pressed_button= i;
			 buttons[i].pressed= TRUE;
			 Update_Button(i);
		  }
		}
	 }
  } //Buttons
  //Bubble-Feld in Game-Mode
  p1.x= 7;
  p1.y= 7;
  p2.x= 486;
  p2.y= 342;
  if( (mode==GAME)&(Check_In_Box(p1, p2, xPos, yPos)) )
  {
	 if(!game_busy)
	 {
		Check_Bubbles_Clicked(xPos, yPos);
	 }
  }

}

void Check_Button_Up(UINT xPos, UINT yPos)
{
  if(Check_In_Box(buttons[pressed_button].topleft,
	  buttons[pressed_button].bottomright, xPos, yPos))
  {
	 if(buttons[pressed_button].mode==mode|buttons[pressed_button].mode== ALWAYS)
	 {
		if((!main_btn_mask)|(pressed_button>=5))
		{
		  buttons[pressed_button].pressed= FALSE;
		  Update_Button(pressed_button);
		  Execute_Button(pressed_button);
		}
	 }
  }
  pressed_button= 0; // nicht mehr gedrückt!

  //Checkboxen...
  if(mode==SETUP)
  {
	 POINT a, b;

	 //NORMAL
	 a.x= 97;
	 a.y= 101;
	 b.x= 210;
	 b.y= 120;
	 if(Check_In_Box(a, b, xPos, yPos))
	 {
		setup_data.game_type= GT_NORMAL;
		Create_Screen();
	 }

	 //CONTINUOUS
	 a.x= 97;
	 a.y= 132;
	 b.x= 229;
	 b.y= 151;
	 if(Check_In_Box(a, b, xPos, yPos))
	 {
		setup_data.game_type= GT_CONTINUOUS;
		Create_Screen();
	 }


	 //SHIFTER
	 a.x= 97;
	 a.y= 163;
	 b.x= 189;
	 b.y= 182;
	 if(Check_In_Box(a, b, xPos, yPos))
	 {
		setup_data.game_type= GT_SHIFTER;
		Create_Screen();
	 }


	 //MEGASHIFT
	 a.x= 97;
	 a.y= 194;
	 b.x= 219;
	 b.y= 217;
	 if(Check_In_Box(a, b, xPos, yPos))
	 {
		setup_data.game_type= GT_MEGASHIFT;
		Create_Screen();
	 }


	 //play_sounds
	 a.x= 277;
	 a.y=  81;
	 b.x= 416;
	 b.y= 101;
	 if(Check_In_Box(a, b, xPos, yPos))
	 {
		if(setup_data.play_sounds== FALSE) setup_data.play_sounds= TRUE;
		else setup_data.play_sounds= FALSE;
		Create_Screen();
	 }

	 //animations
	 a.x= 278;
	 a.y= 124;
	 b.x= 406;
	 b.y= 143;
	 if(Check_In_Box(a, b, xPos, yPos))
	 {
		if(setup_data.animations== FALSE) setup_data.animations= TRUE;
		else setup_data.animations= FALSE;
		Create_Screen();
	 }
  }
}

void Check_Button_Move(UINT xPos, UINT yPos, UINT wParam)
{
  if( (wParam!= MK_LBUTTON)&(pressed_button!= 0) ) // wenn die Maustaste losgelassen wurde!
  {
	 buttons[pressed_button].pressed= FALSE;
	 Update_Button(pressed_button);
	 pressed_button= 0;
  }
  if(pressed_button!= 0)
  {
	 if(Check_In_Box(buttons[pressed_button].topleft,
		 buttons[pressed_button].bottomright, xPos, yPos))
	 {
		  buttons[pressed_button].pressed= TRUE;
		  Update_Button(pressed_button);
	 }
	 if(!Check_In_Box(buttons[pressed_button].topleft,
		 buttons[pressed_button].bottomright, xPos, yPos))
	 {
		  buttons[pressed_button].pressed= FALSE;
		  Update_Button(pressed_button);
	 }
  }
}

void Game_Init()
{
  char str[255];

  randomize();

  FillSurface(v_screen, 0, 0, 493, 419, RGB(0,0,0));
  ShowTextCenter(v_screen, 245, 195, "Loading Bubblet...", RGB(0xFF,0xFF,0xFF));
  PutSurfacePrimary(0, 0, v_screen);

  //Spielfeld-Dimensionen eingeben (nicht ändern!)
  PlayField_Rect.left=     7;
  PlayField_Rect.top=      7;
  PlayField_Rect.right=  486;
  PlayField_Rect.bottom= 342;

  //Setup-Einstellungen laden...
  f= fopen("bubblet.cfg","rb");
  fread(&setup_data, sizeof(setup_data), 1, f);
  fread(&score_statistic, sizeof(score_statistic), 1, f);
  fclose(f);
/*
  setup_data.game_type= GT_CONTINUOUS;
  setup_data.play_sounds= TRUE;
  setup_data.animations= TRUE;
*/

  //Bubbles
  bubble_1= LoadSurface(BUBBLE_1);
  bubble_2= LoadSurface(BUBBLE_2);
  bubble_3= LoadSurface(BUBBLE_3);
  bubble_4= LoadSurface(BUBBLE_4);
  bubble_5= LoadSurface(BUBBLE_5);
  SetColorKey(bubble_1, RGB(0x0,0x0,0x0));
  SetColorKey(bubble_2, RGB(0x0,0x0,0x0));
  SetColorKey(bubble_3, RGB(0x0,0x0,0x0));
  SetColorKey(bubble_4, RGB(0x0,0x0,0x0));
  SetColorKey(bubble_5, RGB(0x0,0x0,0x0));

  // Hintergrundbild
  bk_pic= LoadSurface(BMP_BACKGROUND);

  // Zerknittertes Papier (für Menü-Hintergründe)
  paper_x_pic= LoadSurface(BMP_PAPER_X);
  SetColorKey(paper_x_pic, RGB(0x00,0x00,0x00));

  // Papier (für Menü-Hintergründe)
  paper_pic= LoadSurface(BMP_PAPER);
  SetColorKey(paper_pic, RGB(0x00,0x00,0x00));

  // Tabellen-Grafik für Statistic
  score_table_pic= LoadSurface(BMP_SCORE_TABLE);
  SetColorKey(score_table_pic, RGB(0xFF,0xFF,0xFF));

  // Tabellen-Grafik für Game_Statistic
  game_score_table_pic= LoadSurface(BMP_GAME_SCORE_TABLE);
  SetColorKey(game_score_table_pic, RGB(0xFF,0xFF,0xFF));

  // Setup-Grafik
  setup_map_pic= LoadSurface(BMP_SETUP_MAP);
  SetColorKey(setup_map_pic, RGB(0xFF,0xFF,0xFF));

  // Checked_Box (Häkchen)
  checked_box_pic= LoadSurface(BMP_CHECKED_BOX);
  SetColorKey(checked_box_pic, RGB(0xFF,0xFF,0xFF));

  // Bubblet-Logo
  pic= LoadSurface(BMP_BUBBLET_LOGO);
  SetColorKey(pic, RGB(0x00,0x00,0x00));

  // Button-Maske
  btnmask= LoadSurface(BTNMASK);
  SetColorKey(btnmask, RGB(0xFF,0xFF,0xFF));

  // Exit-Button (1)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_EXIT);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_EXIT_X);
  buttons[button_ctr].topleft.x=  6;
  buttons[button_ctr].topleft.y= 376;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= ALWAYS;

  // About-Button (2)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_ABOUT);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_ABOUT_X);
  buttons[button_ctr].topleft.x= 74;
  buttons[button_ctr].topleft.y= 376;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= ALWAYS;

  // Setup-Button (3)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_SETUP);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_SETUP_X);
  buttons[button_ctr].topleft.x= 142;
  buttons[button_ctr].topleft.y= 376;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= ALWAYS;

  // Scores-Button (4)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_SCORES);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_SCORES_X);
  buttons[button_ctr].topleft.x= 210;
  buttons[button_ctr].topleft.y= 376;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= ALWAYS;

  // New-Button (5)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_NEW);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_NEW_X);
  buttons[button_ctr].topleft.x= 278;
  buttons[button_ctr].topleft.y= 376;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= IDLE;

  // End-Button (6)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_END);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_END_X);
  buttons[button_ctr].topleft.x= 278;
  buttons[button_ctr].topleft.y= 376;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= GAME;

  // OK (Setup) (7)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_OK);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_OK_X);
  buttons[button_ctr].topleft.x= 215;
  buttons[button_ctr].topleft.y= 277;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= SETUP;

  // OK (About) (8)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_OK);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_OK_X);
  buttons[button_ctr].topleft.x= 170;
  buttons[button_ctr].topleft.y= 277;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= ABOUT;

  // OK (Scores) (9)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_OK);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_OK_X);
  buttons[button_ctr].topleft.x= 180;
  buttons[button_ctr].topleft.y= 277;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= SCORES;

  // Reset (Scores) (10)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_RESET);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_RESET_X);
  buttons[button_ctr].topleft.x= 250;
  buttons[button_ctr].topleft.y= 277;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= SCORES;

  // Done (Game_Scores) (11)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_DONE);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_DONE_X);
  buttons[button_ctr].topleft.x= 180;
  buttons[button_ctr].topleft.y= 277;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= GAME_SCORE;

  // New (Game_Scores) (12)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_NEW);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_NEW_X);
  buttons[button_ctr].topleft.x= 250;
  buttons[button_ctr].topleft.y= 277;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= GAME_SCORE;

  // Register (About) (13)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_REGISTER);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_REGISTER_X);
  buttons[button_ctr].topleft.x= 240;
  buttons[button_ctr].topleft.y= 277;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+92;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= ABOUT;

  // Undo-Button (14)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_UNDO);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_UNDO_X);
  buttons[button_ctr].topleft.x= 426;
  buttons[button_ctr].topleft.y= 376;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= GAME;

  // OK (Register) (15)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_OK);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_OK_X);
  buttons[button_ctr].topleft.x= 215;
  buttons[button_ctr].topleft.y= 283;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= REGISTER;

  // OK (Shareware) (16)
  button_ctr++;
  buttons[button_ctr].bmp_normal= LoadSurface(BTN_OK);
  buttons[button_ctr].bmp_pressed= LoadSurface(BTN_OK_X);
  buttons[button_ctr].topleft.x= 215;
  buttons[button_ctr].topleft.y= 277;
  buttons[button_ctr].bottomright.x= buttons[button_ctr].topleft.x+62;
  buttons[button_ctr].bottomright.y= buttons[button_ctr].topleft.y+38;
  buttons[button_ctr].pressed= FALSE;
  buttons[button_ctr].mode= SHAREWAREINFO;

  //sfx-initialisieren
  mci_OpenWave(SFX_INTRO, A_INTRO);
  mci_OpenWave(SFX_CLICK, A_CLICK);
  mci_OpenWave(SFX_POP, A_POP);
  mci_OpenWave(SFX_MEGAPOP, A_MEGAPOP);
  mci_OpenWave(SFX_SCORE, A_SCORE);
  mci_OpenWave(SFX_HIGHSCORE, A_HIGHSCORE);
  mci_OpenWave(SFX_SHAREWARE, A_SHAREWARE);

  //'pre-load' damit das wave-file im festplattencache ist!
  wsprintf(str, "set %s time format tms", A_POP);
  mci_ExecStr(str);
  wsprintf(str, "seek %s to end", A_POP);
  mci_ExecStr(str);
  mci_PlayWave(A_POP);
  wsprintf(str, "seek %s to start", A_POP);
  mci_ExecStr(str);

  /*  den ganzen rest braucht man nicht im cache zu haben...
  wsprintf(str, "set %s time format tms", A_INTRO);
  mci_ExecStr(str);
  wsprintf(str, "seek %s to end", A_INTRO);
  mci_ExecStr(str);
  mci_PlayWave(A_INTRO);
  wsprintf(str, "seek %s to start", A_INTRO);
  mci_ExecStr(str);

  wsprintf(str, "set %s time format tms", A_CLICK);
  mci_ExecStr(str);
  wsprintf(str, "seek %s to end", A_CLICK);
  mci_ExecStr(str);
  mci_PlayWave(A_CLICK);
  wsprintf(str, "seek %s to start", A_CLICK);
  mci_ExecStr(str);

  wsprintf(str, "set %s time format tms", A_MEGAPOP);
  mci_ExecStr(str);
  wsprintf(str, "seek %s to end", A_MEGAPOP);
  mci_ExecStr(str);
  mci_PlayWave(A_MEGAPOP);
  wsprintf(str, "seek %s to start", A_MEGAPOP);
  mci_ExecStr(str);

  wsprintf(str, "set %s time format tms", A_HIGHSCORE);
  mci_ExecStr(str);
  wsprintf(str, "seek %s to end", A_HIGHSCORE);
  mci_ExecStr(str);
  mci_PlayWave(A_HIGHSCORE);
  wsprintf(str, "seek %s to start", A_HIGHSCORE);
  mci_ExecStr(str);

  wsprintf(str, "set %s time format tms", A_SHAREWARE);
  mci_ExecStr(str);
  wsprintf(str, "seek %s to end", A_SHAREWARE);
  mci_ExecStr(str);
  mci_PlayWave(A_SHAREWARE);
  wsprintf(str, "seek %s to start", A_SHAREWARE);
  mci_ExecStr(str);

  wsprintf(str, "set %s time format tms", A_SCORE);
  mci_ExecStr(str);
  wsprintf(str, "seek %s to end", A_SCORE);
  mci_ExecStr(str);
  mci_PlayWave(A_SCORE);
  wsprintf(str, "seek %s to start", A_SCORE);
  mci_ExecStr(str);
  */
  if(setup_data.play_sounds) mci_PlayWave(A_CLICK); //sfx

  Create_Screen();
  game_initialized= TRUE;
}

void Game_Shutdown()
{
  //Speichert Configuretions-Daten und Statistik
  f= fopen("bubblet.cfg","wb");
  fwrite(&setup_data, sizeof(setup_data), 1, f);
  fwrite(&score_statistic, sizeof(score_statistic), 1, f);
  fclose(f);

  mci_CloseWave(A_INTRO);
  mci_CloseWave(A_CLICK);
  mci_CloseWave(A_POP);
  mci_CloseWave(A_MEGAPOP);
  mci_CloseWave(A_SCORE);
  mci_CloseWave(A_HIGHSCORE);
  mci_CloseWave(A_SHAREWARE);

  timeKillEvent(TIMER_ID);
}

//Reload imgaes after DDERR_SURFACELOST
void Game_Reload()
{
  //if(->IsLost dann ->Restore und ReLoadSurface?
}

void Game_Message(UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
	 case WM_MOUSEMOVE:
		Check_Button_Move(LOWORD(lParam), HIWORD(lParam), wParam);
		break;

	 case WM_LBUTTONDOWN:
		Check_Button_Down(LOWORD(lParam), HIWORD(lParam));
		break;

	 case WM_LBUTTONUP:
		Check_Button_Up(LOWORD(lParam), HIWORD(lParam));
		break;

	 /* Momentan nur linker Button unterstützt!
	 case WM_RBUTTONDOWN:
		break;

	 case WM_RBUTTONUP:
		break;
	 */

	 case WM_CREATE:
		break;

	 /*
	 case WM_KEYDOWN:
		switch(wParam)
		{
		  case VK_ESCAPE:
		  case VK_F12:
			 PostMessage(main_hwnd, WM_CLOSE, 0, 0);
			 break;
		}
		break;
	 */

	 case WM_ENDGAME: //wird von animate(..) aufgerufen wenn das Spiel zu ende ist...
		change_mode(GAME_SCORE); //Spiel vorbei! Game_Scores anzeigen!
		break;

	 case WM_SURFACELOST:
		Game_Reload();
		break;

	 case WM_PAINT:
		if(game_initialized) Update_Screen();
		break;

	 case WM_QUIT:
		#ifdef REGISTERED_BUILD
		PostMessage(main_hwnd, WM_CLOSE, 0, 0);
		#else
		change_mode(REGISTER);
		#endif
		break;

	 case WM_CLOSE:
		Game_Shutdown();
  } //switch
}


