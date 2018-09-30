#include <GL/freeglut.h>
#include <GL/glui.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <ctime>
#define MaxBox 64

enum direction {up, down, left, right};

struct line
{
	GLfloat x1, y1, x2, y2;
	GLint painted;
	line() {painted = 0;}
};

struct box
{
	GLint color;
	line dir[4];
	box* neig[4];
	box() 
	{
		color = 0;
		for (GLint way = 0; way < 4; way++)
			neig[way] = NULL;
	}
};

GLint dots_v = 5;
GLint dots_h = 5;
GLint box_num = (dots_v-1)*(dots_h-1);
GLfloat height = 10.0;
GLfloat width = 10.0;
GLfloat step_H = height/(GLfloat)(dots_h+1);
GLfloat step_W = width/(GLfloat)(dots_v+1);
GLint window_width = 600;
GLint window_height = 350;
float k = (window_width-160)/(GLfloat)window_width;
box boxes[MaxBox];
bool f_p = false;
int pl = 0;
void CPU();

void filling_boxes()
{
	box_num = (dots_v-1)*(dots_h-1);
	step_H = height/(GLfloat)(dots_h+1);
	step_W = width/(GLfloat)(dots_v+1);
	GLint c = 0;
	for (GLfloat j = step_H; j < dots_h*step_H-0.1; j += step_H)
		for (GLfloat i = step_W; i < dots_v*step_W-0.1; i += step_W)
		
		{
			boxes[c].dir[up].x1 = boxes[c].dir[left].x1 = i;
			boxes[c].dir[up].y1 = boxes[c].dir[left].y1 = j;

			boxes[c].dir[up].x2 = boxes[c].dir[up].x1+step_W;
			boxes[c].dir[up].y2 = boxes[c].dir[up].y1;

			boxes[c].dir[left].x2 = boxes[c].dir[left].x1;
			boxes[c].dir[left].y2 = boxes[c].dir[left].y1+step_H;
				
			boxes[c].dir[down].x1 = boxes[c].dir[up].x1;
			boxes[c].dir[down].y1 = boxes[c].dir[up].y1+step_H;
			boxes[c].dir[down].x2 = boxes[c].dir[up].x2;
			boxes[c].dir[down].y2 = boxes[c].dir[up].y2+step_H;

			boxes[c].dir[right].x1 = boxes[c].dir[left].x1+step_W;
			boxes[c].dir[right].y1 = boxes[c].dir[left].y1;
			boxes[c].dir[right].x2 = boxes[c].dir[left].x2+step_W;
			boxes[c].dir[right].y2 = boxes[c].dir[left].y2;;

			if ((c+1)%(dots_v-1) != 0) boxes[c].neig[right] = &boxes[c+1];
			if (c%(dots_v-1) != 0) boxes[c].neig[left] = &boxes[c-1];
			if (c < (dots_h-2)*(dots_v-1)) boxes[c].neig[down] = &boxes[c+dots_v-1];
			if (c >= dots_v-1) boxes[c].neig[up] = &boxes[c-dots_v+1];
			c++;
		}
}

GLfloat dist (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) 
{
	return sqrtf(pow((x1-x2), 2) +pow((y1-y2), 2));
}

void display ()
{
	box_num = (dots_v-1)*(dots_h-1);
	step_H = height/(GLfloat)(dots_h+1);
	step_W = width/(GLfloat)(dots_v+1);
	CPU();
	glClear(GL_COLOR_BUFFER_BIT);								
	glColor3f(0.0f,0.0f,0.0f);
	glPointSize(9.1);
	glBegin(GL_POINTS);
	for (GLfloat i = step_W; i < (dots_v+1)*step_W-0.1; i += step_W)
		for (GLfloat j = step_H; j < (dots_h+1)*step_H-0.1; j += step_H)
			glVertex2f(i*k, j);	
	glEnd();
	glLineWidth(4);
	glBegin(GL_LINES);
	for (GLint i = 0; i < box_num; i++)
		{
			for (GLint way = 0; way < 4; way++)
			if (boxes[i].dir[way].painted) 
			{
				switch (boxes[i].dir[way].painted)
				{
					case 1: glColor3f(1.0f,0.0f,0.0f); break;
					case 2: glColor3f(0.0f,0.0f,1.0f); break;
					default: glColor3f(0.0f,0.0f,0.0f); break;
				}
				glVertex2f(boxes[i].dir[way].x1*k, boxes[i].dir[way].y1);
				glVertex2f(boxes[i].dir[way].x2*k, boxes[i].dir[way].y2);
			}
		}
	glEnd();
	int f_p_score = 0;
	int s_p_score = 0;
	glBegin(GL_QUADS);
		for (GLint i = 0; i < box_num; i++)
			if (boxes[i].color)
			{
				if (boxes[i].color == 1) {glColor3f(1.0f,0.0f,0.0f); f_p_score++;}
				else {glColor3f(0.0f,0.0f,1.0f); s_p_score++;}
				glVertex2f(boxes[i].dir[up].x1*k, boxes[i].dir[up].y1);
				glVertex2f(boxes[i].dir[up].x2*k, boxes[i].dir[up].y2);
				glVertex2f(boxes[i].dir[right].x2*k, boxes[i].dir[right].y2);
				glVertex2f(boxes[i].dir[left].x2*k, boxes[i].dir[left].y2);
			}
	glEnd();

	char score[100];
	char winner[100];
	float norm = 0.0;
	glColor3f(0.0f,0.0f,0.0f);
	sprintf(score,"first player: %i   second player: %i", f_p_score, s_p_score);
	glRasterPos2f((5-1290.0/(window_width-160))*k,step_H/2);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18,(unsigned char*)score);
	if ( f_p_score + s_p_score == box_num)
	{
		if (f_p_score > s_p_score) {sprintf(winner,"first player won");norm = 110;}
		else if (f_p_score < s_p_score) {sprintf(winner,"second player won"); norm = -40;}
		else {sprintf(winner,"draw"); norm = 560;}
		glRasterPos2f((5-(750.0-norm)/(window_width-160))*k,height - step_H/2);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18,(unsigned char*)winner);
	}
	glutSwapBuffers();
}

void new_game(int)
{
	for (GLint i = 0; i < MaxBox; i++)
	{
		boxes[i].color = 0;
		for (GLint way = 0; way < 4; way++)
		{
			boxes[i].dir[way].painted = 0;
			boxes[i].neig[way] = NULL;
		}
	}
	filling_boxes();
	f_p = (bool)pl;
	glutPostRedisplay();
}

void change_adj (box b, int way, int value)
{
	if (b.neig[way]) b.neig[way]->dir[(way%2==0)?way+1:way-1].painted = value;
}

void check_boxes(int player)
{
	GLboolean solid;
	for (GLint i = 0; i < box_num; i++)
	{
		solid = true;
		for (GLint j = 0; j < 4; j++)
			if (boxes[i].dir[j].painted == 0) solid = false;
		if ((solid)&&(!boxes[i].color)) 
		{
			boxes[i].color = player;
			for (GLint way = 0; way < 4; way++)
			{
				boxes[i].dir[way].painted = 3;
				change_adj (boxes[i], way, 3);
			}
			if (player == 1) f_p = false;
			else f_p = true;
		}
	}
}

int sides(box b)
{
	int sides = 0;
	for (GLint way = 0; way < 4; way++)
		if (b.dir[way].painted) sides++;
	return sides;
}

bool put_line(int i)
{
	if (f_p)
	for (GLint way = 0; way < 4; way++)
	{
		if (!boxes[i].dir[way].painted)
		{
			if (boxes[i].neig[way])
			{
				if (sides(*boxes[i].neig[way]) < 2)
				{
					boxes[i].dir[way].painted = 2;
					change_adj(boxes[i], way, 2);
					f_p = false; check_boxes(2);
					return true;
				}
			}
			else
			{
				boxes[i].dir[way].painted = 2;
				change_adj(boxes[i], way, 2);
				f_p = false; check_boxes(2);
				return true;
			}
		}
	}
	if (f_p)
	for (GLint way = 0; way < 4; way++)
		if (!boxes[i].dir[way].painted)
		{
			boxes[i].dir[way].painted = 2;
			change_adj(boxes[i], way, 2);
			f_p = false; check_boxes(2);
			return true;
		}
	return false;
}

void put_line_way(int i, int way)
{
	if (f_p)
		if (!boxes[i].dir[way].painted)
		{
			boxes[i].dir[way].painted = 2;
			change_adj(boxes[i], way, 2);
			f_p = false; check_boxes(2);
		}
}

void put_line_pointer(box *b)
{
	if (f_p)
		for (GLint way = 0; way < 4; way++)
		{
			if ((!(*b).dir[way].painted)&&(!(*b).neig[way]))
			{
				(*b).dir[way].painted = 2;
				change_adj((*b), way, 2);
				f_p = false; check_boxes(2);
			}
		}
}

bool first_part()
{
	for (GLint i = 0; i < box_num; i++)
	{
		if (sides(boxes[i]) == 3)
		{
			put_line(i);
			return true;
		}
	}
	bool t;
	srand(time(0));
	std::vector <int> box_m;
	std::vector <int> way_m;
	for (GLint i = rand()%box_num; box_m.size() < box_num ; i = rand()%box_num)
	{
		t = true;
		for (std::vector <int>::iterator it = box_m.begin(); it != box_m.end(); it++)
			if (i == *it) t = false;
		if (t) box_m.push_back(i);
	}
	for (GLint way = rand()%4; way_m.size() < 4 ; way = rand()%4)
	{
		t = true;
		for (std::vector <int>::iterator it = way_m.begin(); it != way_m.end(); it++)
			if (way == *it) t = false;
		if (t) way_m.push_back(way);
	}
	for (std::vector <int>::iterator i = box_m.begin(); i != box_m.end(); i++)
	{
		if (sides(boxes[*i]) < 2)
		{
			for (std::vector <int>::iterator way = way_m.begin(); way != way_m.end(); way++)
			{
				if (!boxes[*i].dir[*way].painted)
				{
					if (boxes[*i].neig[*way])
					{
						if (sides(*boxes[*i].neig[*way]) < 2)
						{
							boxes[*i].dir[*way].painted = 2;
							change_adj(boxes[*i], *way, 2);
							f_p = false; check_boxes(2);
							return true;
						}
					}
					else
					{
						boxes[*i].dir[*way].painted = 2;
						change_adj(boxes[*i], *way, 2);
						f_p = false; check_boxes(2);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool give_sacrifice_1()
{
	int two = 0;
	for (GLint i = 0; i < box_num; i++)
	{
		for (GLint way = 0; way < 4; way++)	
		{
			if (!boxes[i].dir[way].painted)
			{
				if ((!boxes[i].neig[way])||(sides(*boxes[i].neig[way]) == 1)) two++;
			}
		}
		if (two == 2) 
		{
			put_line(i);
			return true;
		}
		two = 0;
	}
	return false;
}

bool give_sacrifice_2()
{
	int two = 0;
	for (GLint i = 0; i < box_num; i++)
	{
		for (GLint way = 0; way < 4; way++)	
		{
			if (!boxes[i].dir[way].painted)
			{
				if (boxes[i].neig[way])
				{
					if ((sides(boxes[i]) == 2)&&(sides(*boxes[i].neig[way]) == 2))
					{
						for (GLint w = 0; w < 4; w++)
						{
							if (!boxes[i].dir[w].painted)
							{
								if ((!boxes[i].neig[w])||(sides(*boxes[i].neig[w]) == 1)) two++;
							}
							if (!(*boxes[i].neig[way]).dir[w].painted)
							{
								if ((!(*boxes[i].neig[way]).neig[w])||(sides(*(*boxes[i].neig[way]).neig[w]) == 1)) two++;
							}
						}
						if (two == 2) 
						{
							put_line_way(i, way);
							return true;
						}
						two = 0;
					}
				}
			}
		}
	}
	return false;
}

bool take_sacrifice_1()
{
	int one = 0;
	for (GLint i = 0; i < box_num; i++)
	{
		if (sides(boxes[i]) == 3)
		{
			for (GLint way = 0; way < 4; way++)	
			{
				if (!boxes[i].dir[way].painted)
				{
					if ((!boxes[i].neig[way])||(sides(*boxes[i].neig[way]) == 1)) one++;
				}
			}
			if (one == 1) 
			{
				put_line(i);
				return true;
			}
			one = 0;
		}
	}
	return false;
}

bool incorrect_chain()
{
	int count = 0;
	for (GLint i = 0; i < box_num; i++)
	{
		if (sides(boxes[i]) == 3)
			count++;
	}
	if (count > 1)
	{
		for (GLint i = 0; i < box_num; i++)
		{
			if (sides(boxes[i]) == 3)
			{
				put_line(i);
				return true;
			}
		}
	}
	return false;
}

bool take_chain()
{
	for (GLint i = 0; i < box_num; i++)
		if (sides(boxes[i]) < 2) return false;
	for (GLint i = 0; i < box_num; i++)
	{
		if (sides(boxes[i]) == 3)
		{
			for (GLint way = 0; way < 4; way++)
			{
				if (!boxes[i].dir[way].painted)
				{
					if ((boxes[i].neig[way])&&(sides(*boxes[i].neig[way]) == 2))
					{
						for (GLint w = 0; w < 4; w++)
						{
							if (!(*boxes[i].neig[way]).dir[w].painted)
							{
								if (((*boxes[i].neig[way]).neig[w])&&(sides(*(*boxes[i].neig[way]).neig[w]) == 2))
								{
									put_line(i);
									return true;
								}
							}
						}
					}
				}
			}
		}
	}
	if (incorrect_chain()) return true;
	int count = 0;
	for (GLint i = 0; i < box_num; i++)
	{
		for (GLint way = 0; way < 4; way++)
		{
			if (!boxes[i].dir[way].painted)
			{
				if (!boxes[i].neig[way]) count+=2;
				else count++;
			}
		}
	}
	for (GLint i = 0; i < box_num; i++)
	{
		if (sides(boxes[i]) == 3)
		{
			for (GLint way = 0; way < 4; way++)
			{
				if (!boxes[i].dir[way].painted)
				{
					if ((boxes[i].neig[way])&&(sides(*boxes[i].neig[way]) == 2))
					{
						for (GLint w = 0; w < 4; w++)
						{
							if (!(*boxes[i].neig[way]).dir[w].painted)
							{
								if (((*boxes[i].neig[way]).neig[w])&&(sides(*(*boxes[i].neig[way]).neig[w]) == 2))
								{
									put_line(i);
									return true;
								}
							}
						}
						if (count/2 > 2)
						{
							put_line_pointer(boxes[i].neig[way]);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool put_ran_line()
{
	for (GLint i = 0; i < box_num; i++)
	{
		if (sides(boxes[i]) == 2)
		{
			put_line(i);
			return true;
		}
	}
	return false;
}

void CPU()
{
	while (f_p)
	{			
		if (take_sacrifice_1()) continue;
		if (take_chain()) continue;
		if (first_part()) continue;
		if (give_sacrifice_1()) continue;
		if (give_sacrifice_2()) continue;
		if (put_ran_line()) continue;
		break;
	}
}

void mouse_action(int button, int state, int tx, int ty)
{
	if (button==GLUT_LEFT_BUTTON&&state==GLUT_DOWN) 
	{
		float x=tx*width/(float)window_width;
		float y=ty*height/(float)window_height;
		for (GLint i = 0; i < box_num; i++)
		{
			for (GLint way = 0; way < 4; way++)
				if (dist(boxes[i].dir[way].x1*k, boxes[i].dir[way].y1, boxes[i].dir[way].x2*k, boxes[i].dir[way].y2)+0.25 > 
					dist(x, y, boxes[i].dir[way].x1*k, boxes[i].dir[way].y1) + dist(x, y, boxes[i].dir[way].x2*k, boxes[i].dir[way].y2)) 
				{
					if ((!f_p)&&(!boxes[i].dir[way].painted)) 
					{
						boxes[i].dir[way].painted = 1;
						change_adj(boxes[i], way, 1);
						f_p = true;
						check_boxes(1);
					}
					//else if (!boxes[i].dir[way].painted) 
					//{
					//	boxes[i].dir[way].painted = 2;
					//	change_adj(boxes[i], way, 2);
					//	f_p = false;
					//	check_boxes(2);
					//}
					goto out;
				}
		}
		out:;
	}
	glutPostRedisplay();
}

void reshape(GLint w, GLint h)
{
	window_width = w;
	window_height = h;
	k = (window_width-160)/(GLfloat)window_width;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);
	glViewport(0, 0, w, h);
}

int main(int argc, char** argv)
{
	filling_boxes();
	FreeConsole();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(200, 100);
	int main_window = glutCreateWindow("Dots&Boxes");
	glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
	glEnable(GL_POINT_SMOOTH);
	glMatrixMode(GL_MODELVIEW);
	glutMouseFunc(mouse_action);
	glutDisplayFunc(display);
	//***********************************************//
	GLUI_Master.set_glutReshapeFunc(reshape);  
	GLUI *glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_RIGHT);
	glui->set_main_gfx_window(main_window);
	//***********************************************//
	new GLUI_StaticText(glui, "");
	new GLUI_Button(glui, "New game", 0, new_game);
	new GLUI_StaticText(glui, "");
	GLUI_Spinner *spinner_v = new GLUI_Spinner(glui, "ver. dots:", &dots_v, 0, new_game);
	spinner_v->set_int_limits(3, 8);
	spinner_v->set_alignment(GLUI_ALIGN_RIGHT);
	GLUI_Spinner *spinner_h = new GLUI_Spinner(glui, "hor. dots:", &dots_h, 0, new_game);
	spinner_h->set_int_limits(3, 8);
	spinner_h->set_alignment(GLUI_ALIGN_RIGHT);
	new GLUI_StaticText(glui, "");
	GLUI_Panel *player = new GLUI_Panel(glui, "Player");
	GLUI_RadioGroup *radio = new GLUI_RadioGroup (player, &pl, 0, new_game);
	new GLUI_RadioButton(radio, "first");
	new GLUI_RadioButton(radio, "second");
	new GLUI_StaticText(glui, "");
	new GLUI_Button(glui, "Quit", 0,(GLUI_Update_CB)exit);
	//***********************************************//
	glutMainLoop();
	return EXIT_SUCCESS;
}

