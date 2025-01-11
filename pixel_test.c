#include <GL/glut.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

// compilation : gcc pixel_test.c -o pixel_test -lglut `pkg-config --cflags --libs glu`


#define CLICK_GAUCHE 0
#define CLICK_DROIT 2
#define ROULETTE_CLICK 1 
#define ROULETTE_BAS 4
#define ROULETTE_HAUT 3

#define TAILLE_SELECTEUR_MAX 256
#define TAILLE_SELECTEUR_MIN 1

#define MODE_STIMULATION_NOIR_BLANC           0     // passe les pixels du noir au blanc d'un coup
#define MODE_STIMULATION_255_NIVEAUX_ROUGE    1     // passe les pixels juste avec la composante rouge
#define MODE_STIMULATION_255_NIVEAUX_VERT     2     
#define MODE_STIMULATION_255_NIVEAUX_BLEU     3
#define MODE_STIMULATION_RANDOM               4   

// #define MODE_STIMULATION_255_ROUGE_VERT_BLEU  4     // passe les pixels du rouge au vert puis au bleu
// #define MODE_STIMULATION_TOUS                 5     // passe tous les modes

#define MAX_DEAD_PIXEL 50


void stimulation(int);

struct Pixel
{
 GLbyte rouge;
 GLbyte vert;
 GLbyte bleu;
};


int screen_x, screen_y;
// int index_test = 0;
int training_mode = 0;
unsigned int nb_pixel_hs = 0;
unsigned int taille_selecteur = 4;
int selecteur_visible = 0;
int selecteur_posx;
int selecteur_posy;
int mode_stimulation = MODE_STIMULATION_NOIR_BLANC;
int stimulation_start = 0;
int flag_stimulation_commutation = 0;
int stimulation_delais = 50;  // ms
int fullscreen = 0;

GLbyte cs_pixel_rouge = 0;
GLbyte cs_pixel_vert = 0;
GLbyte cs_pixel_bleu = 0;

GLbyte stimulation_rouge = 0;
GLbyte stimulation_verte = 0;
GLbyte stimulation_bleue = 0;

int screen_x_max_size = 0;
int screen_y_max_size = 0;


struct dead_pixel
{
 float r, g, b;
 unsigned int pos_x, pos_y;
};


struct dead_pixel dead_pixel_table[MAX_DEAD_PIXEL]={};



/*struct Pixel PixelTest[8] = { { 0, 0, 0 }, 
                              { 0, 0, 255 },
			      { 0, 255, 0 },
			      { 0, 255, 255 },
			      { 255, 0, 0 },
			      { 255, 0, 255 },
			      { 255, 255, 0 },
			      { 255, 255, 255 },
};*/





void affichage();

void syntaxe(char * cmd)
{
 fprintf(stdout, "syntaxe %s --fullscreen --window=XxY --fake_dead_pixel=n --help\n", cmd);
 fprintf(stdout, "%s --fullscreen\n", cmd);
 fprintf(stdout, "%s --window=800x600\n", cmd);
 fprintf(stdout, "%s --window=1024x768 --fake_dead_pixel=2\n", cmd);


}


void show_command()
{
 fprintf(stdout, "\n[Keyboard commands]\n");
 fprintf(stdout, "R/r - add / remove red color\n");
 fprintf(stdout, "G/g - add / remove green color\n");
 fprintf(stdout, "B/b - add / remove blue color\n");
 fprintf(stdout, "S/s - run / stop pixel stimulator\n");
 fprintf(stdout, "M/m - change pixel stimulator mode\n");
 fprintf(stdout, "+/- - increase / decrease the stimulation time\n");
 fprintf(stdout, "Q/q - Exit the application\n");
 fprintf(stdout, "\n[Mouse commands]\n");
 fprintf(stdout, "Left click - show selector\n");
 fprintf(stdout, "Right click - hide selector\n");
 fprintf(stdout, "Wheel up/down - increase/reduce size of selector\n");
 
}



/*void resize(int width, int height) 
{
 // no resize window    
 glutReshapeWindow(screen_x, screen_y);
}*/



void key_up(unsigned char key, int x, int y)
{
 // fprintf(stdout, "debug : key_up = %c / %d\n", key, key);
  
 switch(key)
 {
  // touche échap
  case 27: 
  case 'Q':
  case 'q':

   exit(0);
   break;
 

  // fullscreen
  /*case 'F':
  case 'f':
  
   if (fullscreen == 0)
   { 
    glutFullScreen();
    fullscreen = 1;
   }
   else
   {
    glutReshapeWindow(screen_x, screen_y);
    //glutPositionWindow(0,0);
    fullscreen = 0;
   }

  break; */


  // commutation du rouge
  case 'R': 
  case 'r':
   
   if (cs_pixel_rouge == 1)
    cs_pixel_rouge = 0;
   else
    cs_pixel_rouge = 1;
       
   break;
  // **********************
    
  // commutation du vert 
  case 'G': 
  case 'g':
   
   if (cs_pixel_vert == 1)
    cs_pixel_vert = 0;
   else
    cs_pixel_vert = 1;
        
   break;
  // ********************
    
  // commutation du bleu
  case 'B': 
  case 'b':
   
   if (cs_pixel_bleu == 1)
    cs_pixel_bleu = 0;
   else
    cs_pixel_bleu = 1;
        
   break; 
  // *********************
    
   
  // changer le mode de stimulation
  case 'M':
  case 'm':
    
  if (selecteur_visible == 1)
  {
   if (mode_stimulation == 4) mode_stimulation = 0;
    else mode_stimulation++; 
  } 
   
   break;
  // **********************
  
  // activer ou désactiver la stimulation des pixels
  case 'S':
  case 's':
   
   if (selecteur_visible == 1)
   {
    if (stimulation_start == 0) 
    {
     stimulation_start = 1;
     
     glutTimerFunc(stimulation_delais, stimulation, 0);
    }
    else
    {
     stimulation_start = 0; 
    } 
   }
   
   break;
   
  case '+':

   // ajout de 10 ms
   stimulation_delais += 10;

   // 1s max
   if (stimulation_delais > 1000) 
   {
    stimulation_delais = 1000;
   } 
   

   fprintf(stdout, "Speeds stimulating pixels set to %d ms\n", stimulation_delais);
   break;

  case '-':

   // - 10 ms
   stimulation_delais -= 10;

   if (stimulation_delais < 0) 
   {
    stimulation_delais = 0;
   }
   

  fprintf(stdout, "Speeds stimulating pixels set to %d ms\n", stimulation_delais);
  break;

  default:
  
   return; 
 }
 
 glutPostRedisplay(); 
}


void mouse(int button, int state, int x, int y)
{
 // fprintf(stdout, "debug : mouse pos(%d,%d) - bouton %d\n", x, y, button);
 
 switch(button)
 {
  // pour sélectionner la zone à stimuler
  case CLICK_GAUCHE:
   
   if (selecteur_visible == 0) selecteur_visible = 1;
   
   selecteur_posx = x;
   
   // y inverser par rapport au repère cartésien
   y -= (screen_y - 1);
   selecteur_posy = abs(y);
   break;
  
   
  // supprimer la sélection
  case CLICK_DROIT:
   
   if (selecteur_visible == 1) selecteur_visible = 0;
   
   break;
  // ************************** 
   
  // roulette bas réduire la zone sélectionner
  case ROULETTE_BAS:
   
   if (taille_selecteur == TAILLE_SELECTEUR_MIN) return;
   
   taille_selecteur -= 1;
   break;
   
   
  // roulette haut agrandir la zone sélectionner   
  case ROULETTE_HAUT:
  
   if (taille_selecteur == TAILLE_SELECTEUR_MAX) return;
   
   taille_selecteur += 1;
   break;
   
   
  // click avec la roulette, changer le mode de stimulation
  /*case ROULETTE_CLICK:
    
   if (mode_stimulation == 3)
   {
    mode_stimulation = 0;
   }
   else
   {
    mode_stimulation++; 
   }
   
   break;*/
   
  default:
   
   return;
 }
 
 glutPostRedisplay();
}


void stimulation(int value)
{
 // value ignorée
 struct timeval tv1;
 int tmp_mode_stimulation = mode_stimulation;
 
 // cas particulier du mode de stimulation random
 if (mode_stimulation == MODE_STIMULATION_RANDOM)
 {
  gettimeofday(&tv1, NULL);
  srand(tv1.tv_usec);
  tmp_mode_stimulation = rand() % 4;
 }
  

 if (stimulation_start == 1)
 {
  switch (tmp_mode_stimulation)
   {
    case MODE_STIMULATION_NOIR_BLANC:
          
     if (flag_stimulation_commutation == 0)
     {
      stimulation_rouge = 255;
      stimulation_verte = 255;
      stimulation_bleue = 255;
      flag_stimulation_commutation = 1;
     }
     else
     { 
      stimulation_rouge = 0;
      stimulation_verte = 0;
      stimulation_bleue = 0;
      flag_stimulation_commutation = 0;   
     }
      
     break;
    
    case MODE_STIMULATION_255_NIVEAUX_ROUGE:
          
     if (flag_stimulation_commutation == 0)
     {
      stimulation_rouge = 255;
      stimulation_verte = 0;
      stimulation_bleue = 0;
      flag_stimulation_commutation = 1;
     }
     else
     { 
      stimulation_rouge = 0;
      stimulation_verte = 0;
      stimulation_bleue = 0;
      flag_stimulation_commutation = 0;   
     }

     break;
    
    case MODE_STIMULATION_255_NIVEAUX_VERT:
    
     if (flag_stimulation_commutation == 0)
     {
      stimulation_rouge = 0;
      stimulation_verte = 255;
      stimulation_bleue = 0;
      flag_stimulation_commutation = 1;
     }
     else
     { 
      stimulation_rouge = 0;
      stimulation_verte = 0;
      stimulation_bleue = 0;
      flag_stimulation_commutation = 0;   
     }

     break;
    
    case MODE_STIMULATION_255_NIVEAUX_BLEU:
        
     if (flag_stimulation_commutation == 0)
     {
      stimulation_rouge = 0;
      stimulation_verte = 0;
      stimulation_bleue = 255;
      flag_stimulation_commutation = 1;
     }
     else
     { 
      stimulation_rouge = 0;
      stimulation_verte = 0;
      stimulation_bleue = 0;
      flag_stimulation_commutation = 0;   
     }

     // glColor3f(0.0, 0.0, 1.0);
     break;
    
    /*case MODE_STIMULATION_RANDOM:
   
     glColor3f(1.0, 1.0, 1.0);
     break;*/
    
    /*case MODE_STIMULATION_TOUS:
   
     // glColor3f(1.0, 1.0, 0.0);
     break; */ 
   } 
   
   
   

  glutPostRedisplay();

  // fprintf(stdout, "retour glutPostRedisplay()\n");

  glutTimerFunc(stimulation_delais, stimulation, 0);
  
  
  
  
  
  
 }
  
}



int main(int argc, char * argv[])
{
 int i;
 unsigned int j; 

 struct timeval tv1;

 glutInit(&argc, argv);
 

 fprintf(stdout, "\n\tpixel_test v0.1 - [ Copyright (C) 1997-today - Chédotal Julien ]\n\n");

 // récupération de la définition de l'écran maximum
 screen_y_max_size = glutGet(GLUT_SCREEN_HEIGHT);
 screen_x_max_size = glutGet(GLUT_SCREEN_WIDTH);
 
 fprintf(stdout, "Max screen definition -> %dx%d\n", screen_x_max_size, screen_y_max_size);
 
 
 // au moins 1 argument
 if (argc < 2)
 {
  syntaxe(argv[0]);
  return 1;
 }
 
 // gestion des arguments
 for (i=1; i < argc; i++)
 {
  if (strcmp(argv[i], "--fullscreen") == 0)
  {
   screen_x = screen_x_max_size;
   screen_y = screen_y_max_size;
   fprintf(stdout, "[full screen mode] \n"); 
   fprintf(stdout, "window x_size = %d\n", screen_x);
   fprintf(stdout, "window y_size = %d\n", screen_y);
   fullscreen = 1;
  }
  else if (sscanf(argv[i], "--window=%dx%d", &screen_x, &screen_y) == 2)
  {
  if (fullscreen == 1)
   {
    fprintf(stdout, "the full screen mode option has priority !\n");
    screen_x = screen_x_max_size; 
    screen_y = screen_y_max_size;
   }
   fprintf(stdout, "window x_size = %d\n", screen_x);
   fprintf(stdout, "window y_size = %d\n", screen_y);
  } 
  else if (strcmp(argv[i], "--help") == 0)
  {
   syntaxe(argv[0]);
   return 1;
  }
  else if (sscanf(argv[i], "--fake_dead_pixel=%d", &nb_pixel_hs) == 1)
  {
   // mode pixel hs d'entrainement
   training_mode = 1;
   
   if (nb_pixel_hs < 2 || nb_pixel_hs > MAX_DEAD_PIXEL)
   {
    nb_pixel_hs = 2;
   }    

   fprintf(stdout, "There are %d fake dead pixels, try to find them by changing color...\n", nb_pixel_hs);
  
   // calculer la position et la couleur des pixels hs de façon aléatoire
   gettimeofday(&tv1, NULL);
   srand(tv1.tv_usec);

   for (j=0; j < nb_pixel_hs; j++)
   {
    // position de la pixel
    dead_pixel_table[j].pos_x = rand() % screen_x;
    dead_pixel_table[j].pos_y = rand() % screen_y;
   
    // sa couleur
    dead_pixel_table[j].r = rand() % 2;  
    dead_pixel_table[j].g = rand() % 2;
    dead_pixel_table[j].b = rand() % 2; 
   }

   // ignorer les autres options restantes qui pourrait changer la définition final
   break;
  }
   
  else
  {
   syntaxe(argv[0]);
   return 1;
  }

 }
 

 // vérifier définition de l'écran x et y != 0
 if (screen_x == 0 || screen_y == 0)
 {
  syntaxe(argv[0]);
  return 1;
 }

 
 // afficher les touches de commandes du clavier et de la souris
 show_command();
 

 glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
 glutInitWindowSize(screen_x, screen_y);
 // glutInitWindowPosition(100, 100);
 glutCreateWindow("Pixel test v0.1");
 glutKeyboardFunc(key_up);
 glutMouseFunc(mouse);
 // glutReshapeFunc(resize);

 
 // définir le repère cartésien
 gluOrtho2D(0.0f, screen_x, 0.0f, screen_y);
 // glViewport(0.0f, 0.0f, 1.0f, 1.0f);
 
 
 
 //fprintf(stdout, "\n[ Test des pixels en %dx%d ...]\n", screen_x, screen_y); 
 // getchar();
 
 if (fullscreen == 1) 
 {
  glutFullScreen();
 }
  
 // définir la fonction callback d'affichage
 glutDisplayFunc(affichage);
 
 // fonction d'attente des événements
 glutMainLoop();
 
 return 0; 
}


void affichage()
{
 //unsigned int i;
 unsigned int j;
 // éffacer l'écran
 glClearColor(cs_pixel_rouge, cs_pixel_vert, cs_pixel_bleu, 1.0);
 glClear(GL_COLOR_BUFFER_BIT);
 
 // glColor3f(PixelTest[index_test].rouge, PixelTest[index_test].vert, PixelTest[index_test].bleu);
 
 // glRectf(0.0f, 0.0f, x, y);
 
 // afficher le sélecteur qui encadre la zone des pixels à stimuler
 if (selecteur_visible == 1)
 {
  if (stimulation_start == 0)
  { 
   switch (mode_stimulation)
   {
    case MODE_STIMULATION_NOIR_BLANC:
    
     glColor3f(1.0, 1.0, 1.0);
     break;
    
    case MODE_STIMULATION_255_NIVEAUX_ROUGE:
     
     glColor3f(1.0, 0.0, 0.0);
     break;
    
    case MODE_STIMULATION_255_NIVEAUX_VERT:
    
     glColor3f(0.0, 1.0, 0.0);
     break;
    
    case MODE_STIMULATION_255_NIVEAUX_BLEU:
   
     glColor3f(0.0, 0.0, 1.0);
     break;
    
    case MODE_STIMULATION_RANDOM:
   
     glColor3f(1.0, 1.0, 1.0);
     break;

    /*case MODE_STIMULATION_255_ROUGE_VERT_BLEU:
   
     glColor3f(0.0, 1.0, 1.0);
     break;
    
    case MODE_STIMULATION_TOUS:
   
     glColor3f(1.0, 1.0, 0.0);
     break;*/ 
   }
  }
  else
  {
   // mode stimulation 
   glColor3ub(stimulation_rouge, stimulation_verte, stimulation_bleue); 
  }
  
   
  
    
   // glBegin(GL_POINTS);
   // glVertex2i(selecteur_posx, selecteur_posy);
   // glEnd();
   
  glRecti(selecteur_posx - taille_selecteur, selecteur_posy - taille_selecteur, selecteur_posx + taille_selecteur, selecteur_posy + taille_selecteur); 
 }
 
 
 // générer une fausse pixel hs
 if (training_mode == 1)
 {     
  

  glPointSize(1.0); 
  
  glBegin(GL_POINTS);
   
  for (j=0; j < nb_pixel_hs; j++)
  {
   
   glColor3f(dead_pixel_table[j].r, dead_pixel_table[j].g, dead_pixel_table[j].b);
   glVertex3f(dead_pixel_table[j].pos_x, dead_pixel_table[j].pos_y, 1.0f); 
  }
  
  glEnd();
 }
 
 
 glFlush(); 
}
