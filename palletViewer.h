/*

The PalletViewer class is used to encapsulate (as static class variables)
variables that would otherwise be global. Making the functions static
helps avoid name conflicts and makes it unnecessary to have an instance
of a PalletViewer.

See palletViewer.cc for documentation of what the functions do and what
the variables are for. Since the variables are static, they are
initialized in that file.

*/

#ifndef PALLETVIEWER_HH
#define PALLETVIEWER

#include <stdlib.h>
#include <vector>
#include <map>
#include "asBuilt.h"
#include "response.h"

struct col {float r, g, b; };

class PalletViewer
{
 public:
  static void drawString(float x, float y, void * font, char * string);
  static void insertBox(col boxColor, unsigned int length,
			unsigned int width, unsigned int height, int orient,
			float posX, float posY, float posZ, bool solid);
  static void init(char * orderFile, char * packlistFile, char * asBuiltFile);
  static void makeColors(int howMany);
  static void printText(int extent);
  static void recalculate(int change);
  static void redraw();
 private:
  static AsBuilt             asBuilt;
  static std::map <int, col> color;
  static col *               colors;
  static unsigned int        count;
  static PackPallet          cpallet;
  static PackList            list;
};

#endif //PALLETVIEWER_HH
