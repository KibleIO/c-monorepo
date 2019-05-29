#ifndef BMP_BAKER_H_
#define BMP_BAKER_H_

#include "../Utilities/TIMER.h"
#include "../Utilities/utilities.h"
#include "Layout/nuklear_plus.h"

#define DEFAULT_INITIAL_CONTAINER_CAPACITY 8

// The atom of the baking engine
// A single screen buffer with multiple bmps drawn onto it, ready to be
// quickly and efficiency copied into the screen buffer without the hassle
// manually setting the pixels every frame
struct BAKED_SCREEN {
  char* screenBuff; // Buffer drawn to the screen
  int width;
  int height;
  int byteResolution; // NOTE: $numPixels * 4 since each pixel is four bytes
};

// A type of baked screen which has zones defined on it
// The screen can then be identified based on the variants of each zone represented on the screen
struct ZONED_SCREEN {
  struct BAKED_SCREEN screen;
  // Array representing the variants of the zones drawn on the screen
  // NOTE: this array of integers is PARALLEL to the list of ZONES
  // on the baking engine
  int* zoneVariants;
  int totalZones;
};

// Pairs a single bmp with the rect where it will be drawn
struct BMP_SOCKET {
    BMP* bmp;
    int x, y, w, h;
    bool empty; // If true, this socket represents an area where nothing at all is drawn
};

// Encapsulates a set of bmp sockets where no two bmps will be drawn at the same time
struct EXCLUSIVE_DRAWING_ZONE {
  int layer;  // Used to draw the zones in correct order. Lower levels are drawn beneath higher layers
  // List of bmp sockets
  struct BMP_SOCKET* sockets;
  int totalSockets;
};

// The powerhouse of the baking system
// Given a set of "zones", the engine can generate a screen for every
// possible combination of bmps. It also allows easy(-ish) access to the screen desired
// by allowing the client to simply give the name of the zone and the variant index they
// want to draw, and the engine will automatically find and draw the screen with the correct variants
struct BAKING_ENGINE {
  struct ZONED_SCREEN* screens; // Array of baked screens paired with a list of the variants that represent that screen
  int totalScreens; // Total screens managed by the engine
  int screenCapacity; // Number of screens that the engine can currently store without resizing
  // List of each "zone" defined on screen that can have multiple variants
  struct EXCLUSIVE_DRAWING_ZONE* zones;
  int totalZones;
  int zoneCapacity;
  // Widht/Height/resolution of all baked screens
  int width;
  int height;
  // NOTE: resolution is $(numPixels) * 4, since each pixel is four bytes
  int byteResolution;
};

/*
BAKED SCREEN MEMBER FUNCTIONS
-----------------------------
*/
// Initialize the baked screen with the given resolution
void Initialize_Baked_Screen(BAKED_SCREEN*, int w, int h);
// The BMP is expected to be drawn inside of the window layout
// given at the given row and column
void Bake_BMP(BAKED_SCREEN*, string dir, const struct window_layout*,
	int row, int column);
// The BMP is drawn with the dimensions explicitly given
void Bake_BMP(BAKED_SCREEN*, string dir, int x, int y, int w, int h);
// Bake the BMP given onto the given bakes screen
void Bake_BMP(BAKED_SCREEN*, struct BMP*, int x, int y);
// Bake the bmp at the location specified in the socket
void Bake_BMP(BAKED_SCREEN*, const BMP_SOCKET*);
// Copy raw buffer data into the baked screen's buffer
void Bake_Raw_Buffer(BAKED_SCREEN*, char*);
// Draw the baked screen onto the char buffer given with a copy operation
void Draw_Baked_Screen(BAKED_SCREEN*, char*);
void Delete_Baked_Screen(BAKED_SCREEN*);

/*
ZONED SCREEN MEMBER FUNCTIONS
-----------------------------
*/
void Initialize_Zoned_Screen_Copy(
ZONED_SCREEN*, BAKED_SCREEN*, int* variants, int total_variants);
void Initialize_Zoned_Screen_Move(
ZONED_SCREEN*, BAKED_SCREEN*, int* variants, int total_variants);
void Delete_Zoned_Screen(ZONED_SCREEN*);

/*
BMP SOCKET MEMBER FUNCTIONS
---------------------------
*/
// Initialize the bmp socket with the bmp at the given dir
void Initialize_BMP_Socket(BMP_SOCKET*, string dir, int x, int y, int w, int h);
void Initialize_BMP_Socket(BMP_SOCKET*);
void Initialize_BMP_Socket_Copy(BMP_SOCKET*, BMP*, int x, int y, int w, int h);
void Initialize_BMP_Socket_Move(BMP_SOCKET*, BMP*, int x, int y, int w, int h);
void Delete_BMP_Socket(BMP_SOCKET*);

/*
EXCLUSIVE_DRAWING_ZONE MEMBER FUNCTIONS
---------------------------------------
*/
// Initialize exclusive drawing zone
void Initialize_Exclusive_Drawing_Zone(EXCLUSIVE_DRAWING_ZONE*, int layer,
	string bmp_dirs, bool has_empty, int x, int y, int w, int h);
void Initialize_Exclusive_Drawing_Zone(EXCLUSIVE_DRAWING_ZONE*, int layer,
	const string* bmp_dirs, int total_bmps, bool has_empty,
	int x, int y, int w, int h);
// Give the socket a deep copy of the bmps given
void Initialize_Exclusive_Drawing_Zone_Copy(EXCLUSIVE_DRAWING_ZONE*, int layer,
	BMP**, int total_bmps, bool has_empty, int x, int y, int w, int h);
// Pass ownership of the bmps to the socket without copying them
void Initialize_Exclusive_Drawing_Zone_Move(EXCLUSIVE_DRAWING_ZONE*, int layer,
	BMP**, int total_bmps, bool has_empty, int x, int y, int w, int h);
// Initialize exclusive drawing zone from the bmp socket explicitly given
void Initialize_Exclusive_Drawing_Zone_Copy(EXCLUSIVE_DRAWING_ZONE*, int layer,
	BMP_SOCKET*, int total_sockets);
void Initialize_Exclusive_Drawing_Zone_Move(EXCLUSIVE_DRAWING_ZONE*, int layer,
	BMP_SOCKET*, int total_sockets);
void Delete_Exclusive_Drawing_Zone(EXCLUSIVE_DRAWING_ZONE*);

/*
BAKING_ENGINE MEMBER FUNCTIONS
------------------------------
*/
// Intialize the baking engine
void Initialize_Baking_Engine(BAKING_ENGINE*, int w, int h,
	int initialCapacity = DEFAULT_INITIAL_CONTAINER_CAPACITY);
void Delete_Baking_Engine(BAKING_ENGINE*);

/*
Baking algorithm
*/
// Create all the screens from the sockets currently initialized on the baking engine
void Bake_All_Screens(BAKING_ENGINE*);
void Bake_Screens_Helper(BAKING_ENGINE*);

/*
Drawing the baked screen
------------------------
- Draw the screen from the baking engine that matches the variant specified
- Client code gives in a list of ids of the sockets paired with the variant the
client wants to draw. Given that information, the engine finds the baked screen
that matches the client's specification
- If any baked socket is not explicitly identified by the client, the engine assumes
that the client wants variant 0 on that zone
------------------------
*/
// Draw the screen with the variant list 0, 0, ... 0
void Draw_Baked_Screen(BAKING_ENGINE*, char* scr_buffer);
// Draw the baked screen with the variant list that matches the list given
// WARNING: This function assumes that the variant list given is parellel to the list
// of exclusive drawing zones in the baking engine. It's up to the client code
// to maintain maintain a valid variant list to draw the correct screen on the engine
void Draw_Baked_Screen(BAKING_ENGINE*, const int* variant_list, char* scr_buffer);
int* Generate_Zone_IDs(EXCLUSIVE_DRAWING_ZONE*, int total_zones,
	const int* user_given_ids, int total_ids);
int Find_Screen_By_ID(ZONED_SCREEN*, int total_screens, const int* variant_list);
bool Zone_IDs_Match(const int*, int total_zones, const int*);

/*
Screen management
*/
// Add the baked screen specified to the baking engine
void Add_Zoned_Screen(BAKING_ENGINE*, BAKED_SCREEN*, int* zone_variants,
	int total_zones);
void Add_Zoned_Screen(BAKING_ENGINE*, ZONED_SCREEN);
// Allocate more space for more baked screens on the baking engine
void Allocate_Screens(BAKING_ENGINE*, int increase);

/*
Zone management
*/
// Add a baking socket with the given id, initalized from
// a punctuated list of bmp directories
void Add_Exclusive_Drawing_Zone(BAKING_ENGINE*, int layer, string bmp_dirs,
	bool hasEmpty, int x, int y, int w, int h);
// Add a baking socket with the given id, intialized
// from an array of bmp directories
void Add_Exclusive_Drawing_Zone(BAKING_ENGINE*, int layer,
	const string* bmp_dirs, int total_bmps, bool hasEmpty,
	int x, int y, int w, int h);
// Add a drawing zone with the given sockets
void Add_Exclusive_Drawing_Zone_Move(BAKING_ENGINE*, int layer, BMP_SOCKET*,
	int total_sockets);
// Add the baking socket specified
void Add_Exclusive_Drawing_Zone(BAKING_ENGINE*, EXCLUSIVE_DRAWING_ZONE);
// Allocate more space for more baking sockets on the baking engine
void Allocate_Exclusive_Drawing_Zones(BAKING_ENGINE*, int new_capacity);

/*
Helpers
*/
// Sort exclusive drawing zones by ids
void Sort_Exclusive_Drawing_Zones_By_Layer(EXCLUSIVE_DRAWING_ZONE*,
	int total_zones);
// Caluclate the number of zoned screens the engine will have
// based on the number of drawing zones that have been initialized on it
int Calculate_Total_Zoned_Screens(BAKING_ENGINE*);

#endif // BMP_BAKER_H_
