/* main.cc
*/

#include <stdio.h>
#include "view.h"    // for glut functions
#include "palletViewer.h"

/********************************************************************/

/* main

Returned Value: int

This is the main function for the palletViewer. It reads the arguments,
calls PalletViewer::init, and prints information about how to use
palletViewer. Then it enters the glutMainLoop, which handles all the
user interaction and draws the display.

*/

#define charSize 100
int main(
 int argc,
 char * argv[])
{
  char orderFile[charSize] = "";
  char packlistFile[charSize] = "";
  char asBuiltFile[charSize] = "";
  unsigned int i;
  
  if(argc < 5)
    {
      fprintf(stderr, "usage: %s -o <order> -p <packlist> [-b <asBuilt>]\n",
	      argv[0]);
      fprintf(stderr, 
	      "Example: %s -o Example.order.xml -p Example.packlist.xml\n",
	      argv[0]);
      exit(1);
    }
  for (i = 1; (int)i < argc; i+=2)
    {
      if(strcmp(argv[i], "-o") == 0)
	strncpy(orderFile, argv[i+1], charSize);
      if(strcmp(argv[i], "-p") == 0)
	strncpy(packlistFile, argv[i+1], charSize);
      if(strcmp(argv[i], "-b") == 0)
	strncpy(asBuiltFile, argv[i+1], charSize);
    }
  PalletViewer::init(orderFile, packlistFile, asBuiltFile);
  printf("Hold down left mouse button and move mouse to translate pallet\n");
  printf("Hold down middle mouse button and move mouse to rotate pallet\n");
  printf("Hold down right mouse button and move mouse up/dn to zoom pallet\n");
  printf("Press h to return to the default view\n");
  printf("Press g to add the next box to the stack\n");
  printf("Press f to remove the last box from the stack\n");
  printf("Press t to save the current image in a ppm file\n");
  printf("Press z or q to exit\n");
  glInit(argc, argv, "Pallet Viewer");
  glutMainLoop();
  
  return 0;
}

/********************************************************************/
