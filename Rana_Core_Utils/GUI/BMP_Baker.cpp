#include "BMP_Baker.h"

/*
BAKED SCREEN MEMBER FUNCTIONS
-----------------------------
*/
void Initialize_Baked_Screen(BAKED_SCREEN* screen, int w, int h) {
	screen->screenBuff = new char[w * h * 4];
	screen->width = w;
	screen->height = h;
	screen->byteResolution = w * h * 4;
}

void Bake_BMP(BAKED_SCREEN* screen, string dir, const struct window_layout* window, int row, int column) {
  struct nk_rect image_rect = Item_Rect(window, row, column);
  Bake_BMP(screen, dir, image_rect.x, image_rect.y, image_rect.w, image_rect.h);
}

// Intialize the bmp with the dimensions and bake it
void Bake_BMP(BAKED_SCREEN* screen, string dir, int x, int y, int w, int h) {
  BMP* image = new BMP();
  Initialize_BMP(image, dir, w, h);
  image->Transparent = true;
  Bake_BMP(screen, image, x, y);
  delete image;
}

// Draw the BMP onto the char buffer of the baked screen
void Bake_BMP(BAKED_SCREEN* screen, BMP* image, int x, int y) {
  Draw_BMP(image, screen->screenBuff, screen->width, x, y);
}

void Bake_BMP(BAKED_SCREEN* screen, const BMP_SOCKET* socket) {
	Bake_BMP(screen, socket->bmp, socket->x, socket->y);
}

void Bake_Raw_Buffer(BAKED_SCREEN* screen, char* buffer) {
	copy(buffer, buffer + screen->byteResolution, screen->screenBuff);
}

void Draw_Baked_Screen(BAKED_SCREEN* screen, char* scrBuffer) {
  // cout << "Drawing baked screen on buffer at address: " << (int*)scrBuffer << endl;
  copy(screen->screenBuff, screen->screenBuff + screen->byteResolution, scrBuffer);
}

void Delete_Baked_Screen(BAKED_SCREEN* screen) {
	delete [] screen->screenBuff;
}

/*
ZONED SCREEN MEMBER FUNCTIONS
-----------------------------
*/
void Initialize_Zoned_Screen_Copy(ZONED_SCREEN* zoned_screen, BAKED_SCREEN* screen, int* variants, int total_variants) {
  zoned_screen->totalZones = total_variants;
  zoned_screen->zoneVariants = new int[total_variants];
  for(int i = 0; i < total_variants; i++) {
    zoned_screen->zoneVariants[i] = variants[i];
  }
  zoned_screen->screen = *screen;
}

void Initialize_Zoned_Screen_Move(ZONED_SCREEN* zoned_screen, BAKED_SCREEN* screen, int* variants, int total_variants) {
  zoned_screen->totalZones = total_variants;
  zoned_screen->zoneVariants = variants;
  zoned_screen->screen = *screen;
}

void Delete_Zoned_Screen(ZONED_SCREEN* zoned_screen) {
	Delete_Baked_Screen(&zoned_screen->screen);
	delete [] zoned_screen->zoneVariants;
}

/*
BMP SOCKET MEMBER FUNCTIONS
---------------------------
*/
void Initialize_BMP_Socket(BMP_SOCKET* socket, string dir, int x, int y, int w, int h) {
  BMP* bmp = new BMP();
  Initialize_BMP(bmp, dir, w, h);
  Initialize_BMP_Socket_Move(socket, bmp, x, y, w, h);
  socket->bmp->Transparent = true;
}

void Initialize_BMP_Socket(BMP_SOCKET* socket) {
  socket->bmp = nullptr;
  socket->x = 0;
  socket->y = 0;
  socket->w = 0;
  socket->h = 0;
  socket->empty = true;
}

void Initialize_BMP_Socket_Copy(BMP_SOCKET* socket, BMP* bmp, int x, int y, int w, int h) {
  *(socket->bmp) = *bmp;
  socket->x = x;
  socket->y = y;
  socket->w = w;
  socket->h = h;
  socket->empty = false;
}

void Initialize_BMP_Socket_Move(BMP_SOCKET* socket, BMP* bmp, int x, int y, int w, int h) {
  socket->bmp = bmp;
  socket->x = x;
  socket->y = y;
  socket->w = w;
  socket->h = h;
  socket->empty = false;
}

void Delete_BMP_Socket(BMP_SOCKET* socket) {
	if(socket->bmp != nullptr) {
		Delete_BMP(socket->bmp);
		delete socket->bmp;
	}
}

/*
EXCLUSIVE_DRAWING_ZONE MEMBER FUNCTIONS
---------------------------------------
*/
void Initialize_Exclusive_Drawing_Zone(EXCLUSIVE_DRAWING_ZONE* zone, int layer, string bmp_dirs, bool has_empty, int x, int y, int w, int h) {
  int total_bmps;
  string* bmp_dirs_arr = Parse_Punctuated_List(bmp_dirs, total_bmps);
  Initialize_Exclusive_Drawing_Zone(zone, layer, bmp_dirs_arr, total_bmps, has_empty, x, y, w, h);
}

void Initialize_Exclusive_Drawing_Zone(EXCLUSIVE_DRAWING_ZONE* zone, int layer, const string* bmp_dirs, int total_bmps, bool has_empty, int x, int y, int w, int h) {
  int adder = (int)has_empty;
  BMP_SOCKET* sockets = new BMP_SOCKET[total_bmps + adder];
  // Initialize local array from array of directories
  for(int i = 0; i < total_bmps; i++) {
    Initialize_BMP_Socket(&sockets[i], bmp_dirs[i], x, y, w, h);
  }
  // If the drawing zone has an empty socket, initialize the last socket to empty
  if(has_empty) {
    Initialize_BMP_Socket(&sockets[total_bmps]);
  }
  // Move ownership of local bmp array to the zone
  Initialize_Exclusive_Drawing_Zone_Move(zone, layer, sockets, total_bmps + adder);
}

void Initialize_Exclusive_Drawing_Zone_Copy(EXCLUSIVE_DRAWING_ZONE* zone, int layer, BMP** bmps, int total_bmps, bool has_empty, int x, int y, int w, int h) {
  int adder = (int)has_empty;
  BMP_SOCKET* sockets = new BMP_SOCKET[total_bmps + adder];
  // Initialize local array from array of directories
  for(int i = 0; i < total_bmps; i++) {
    Initialize_BMP_Socket_Copy(&sockets[i], bmps[i], x, y, w, h);
  }
  // If the drawing zone has an empty socket, initialize the last socket to empty
  if(has_empty) {
    Initialize_BMP_Socket(&sockets[total_bmps]);
  }
  // Pass data into the struct
  zone->layer = layer;
  zone->sockets = sockets;
  zone->totalSockets = total_bmps + adder;
}

void Initialize_Exclusive_Drawing_Zone_Move(EXCLUSIVE_DRAWING_ZONE* zone, int layer, BMP** bmps, int total_bmps, bool has_empty, int x, int y, int w, int h) {
  int adder = (int)has_empty;
  BMP_SOCKET* sockets = new BMP_SOCKET[total_bmps + adder];
  // Initialize local array from array of directories
  for(int i = 0; i < total_bmps; i++) {
    Initialize_BMP_Socket_Move(&sockets[i], bmps[i], x, y, w, h);
  }
  // If the drawing zone has an empty socket, initialize the last socket to empty
  if(has_empty) {
    Initialize_BMP_Socket(&sockets[total_bmps]);
  }
  // Pass data into the struct
  zone->layer = layer;
  zone->sockets = sockets;
  zone->totalSockets = total_bmps + adder;
}

void Initialize_Exclusive_Drawing_Zone_Copy(EXCLUSIVE_DRAWING_ZONE* zone, int layer, BMP_SOCKET* sockets, int total_sockets) {
  zone->layer = layer;
  zone->sockets = new BMP_SOCKET[total_sockets];
  for(int i = 0; i < total_sockets; i++) {
    zone->sockets[i] = sockets[i];
  }
  zone->totalSockets = total_sockets;
}

void Initialize_Exclusive_Drawing_Zone_Move(EXCLUSIVE_DRAWING_ZONE* zone, int layer, BMP_SOCKET* sockets, int total_sockets) {
  zone->layer = layer;
  zone->sockets = sockets;
  zone->totalSockets = total_sockets;
}

void Delete_Exclusive_Drawing_Zone(EXCLUSIVE_DRAWING_ZONE* zone) {
	for(int i = 0; i < zone->totalSockets; i++) {
		Delete_BMP_Socket(&zone->sockets[i]);
	}
	delete [] zone->sockets;
}

/*
BAKING_ENGINE Initializers
*/

void Initialize_Baking_Engine(BAKING_ENGINE* engine, int w, int h, int initialCapacity) {
  engine->screens = new ZONED_SCREEN[initialCapacity];
  engine->totalScreens = 0;
  engine->screenCapacity = initialCapacity;

  engine->zones = new EXCLUSIVE_DRAWING_ZONE[initialCapacity];
  engine->totalZones = 0;
  engine->zoneCapacity = initialCapacity;

  engine->width = w;
  engine->height = h;
  engine->byteResolution = w * h * 4;
}

void Delete_Baking_Engine(BAKING_ENGINE* engine) {
	for(int i = 0; i < engine->totalScreens; i++) {
		Delete_Zoned_Screen(&engine->screens[i]);
	}
	delete [] engine->screens;

	for(int i = 0; i < engine->totalZones; i++) {
		Delete_Exclusive_Drawing_Zone(&engine->zones[i]);
	}
	delete [] engine->zones;
}

/*
Initializing the baked screens
*/
void Bake_All_Screens(BAKING_ENGINE* engine) {
  TIMER t;
  Start_TIMER(&t);
  Sort_Exclusive_Drawing_Zones_By_Layer(engine->zones, engine->totalZones);
  Bake_Screens_Helper(engine);
  cout << "Finished baking all screens on the baking engine in " << Stop_TIMER(&t) << " milliseconds" << endl;
}

void Bake_Screens_Helper(BAKING_ENGINE* engine) {
	// Stores the socket currently being baked onto the screen
	struct BMP_SOCKET temp_socket;
	// Index of the current socket on the current zone
	// being baked onto the current baked screen
	int current_socket_index;
	// Screen on which the current socket is baked
	// Passed to the zoned screen
	struct BAKED_SCREEN temp_screen;
	// List of variants on the zoned screen currently being baked
	int* socket_list = new int[engine->totalZones] {0};
	int total_screens = Calculate_Total_Zoned_Screens(engine);
	int variants_preceding;
	// Loop through and set up each screen
	for(int screen = 0; screen < total_screens; screen++) {
		// Re-initialize the baked screen for each zoned screen
		Initialize_Baked_Screen(&temp_screen, engine->width, engine->height);
		// Loop through and bake the correct sockets on each zone
		// onto the baked screen
		for(int zone = 0; zone < engine->totalZones; zone++) {
			// Add the total sockets on the zone preceding this zone
			// to the number of varaints preceding the current screen bake
			if(zone <= 0) {
				variants_preceding = 1;
			}
			else {
				variants_preceding *= engine->zones[zone - 1].totalSockets;
			}
			// Calculate the current socket to bake
			current_socket_index = (screen / variants_preceding);
			current_socket_index %= engine->zones[zone].totalSockets;
			// Store the current socket to bake
			temp_socket = engine->zones[zone].sockets[current_socket_index];
			// If the socket isn't empty, bake it
			if(!temp_socket.empty) {
				Bake_BMP(&temp_screen, &temp_socket);
			}
			// Store the index of the socket baked in the local array
			socket_list[zone] = current_socket_index;
		}
		// Add a zoned screen, pairing the baked screen with an ID code
		Add_Zoned_Screen(engine, &temp_screen, socket_list, engine->totalZones);
	}

	// TEMP
	// Commented out below is the older recursive solution
	// to the bmp baking problem

	// for(int current_variant = 0; current_variant < engine->zones[current_zone].totalSockets; current_variant++) {
	// 	//cout << "Current variant: " << current_variant << endl;
	// 	// Put the current variant being baked into the array of zone variants at this zone's index
	// 	zone_ids[current_zone] = current_variant;
	// 	// If this is not the last zone, bake the variants on the zone after this one
	// 	if(current_zone < (engine->totalZones - 1)) {
	// 		Bake_Screens_Helper(engine, zone_ids, current_zone + 1);
	// 	}
	// 	// If this is the last zone, bake all the bmps
	// 	// on all current variants of the zones onto a baked screen
	// 	else {
	// 		struct BMP_SOCKET socket; // Stores the bmp socket being baked on the current zone for convenience
	// 		struct BAKED_SCREEN temp_screen;  // All bmps of the current zones are drawn on this
	// 		Initialize_Baked_Screen(&temp_screen, engine->width, engine->height);
	// 		// Loop through each zone and bake the current bmp variant of every zone
	// 		for (int zone = 0; zone < engine->totalZones; zone++) {
	// 			// Bake the bmp on the socket, unless the socket is empty
	// 			socket = engine->zones[zone].sockets[zone_ids[zone]];
	// 			if(!socket.empty) {
	// 			  	Bake_BMP(&temp_screen, socket.bmp, socket.x, socket.y);
	// 			}
	// 		}
	// 		// Add zoned screen to the engine with the variants baked on the current baked screen
	// 		Add_Zoned_Screen(engine, &temp_screen, zone_ids, engine->totalZones);
	// 	}
	// }
}

/*
Drawing the baked screen
*/

// Draw the default screen
void Draw_Baked_Screen(BAKING_ENGINE* engine, char* scr_buffer) {
  Draw_Baked_Screen(&engine->screens[0].screen, scr_buffer);
}

void Draw_Baked_Screen(BAKING_ENGINE* engine, const int* zone_ids, char* scr_buffer) {
	int screen_index = Find_Screen_By_ID(engine->screens, engine->totalScreens, zone_ids);
	if(screen_index > -1) {
		// cout << "Found screen to draw at index #" << screen_index << endl;
		// cout << "Screen zone ids:" << endl << left;
		// for(int i = 0; i < engine->screens[screen_index].totalZones; i++) {
		//   cout << '\t';
		//   cout << setw(20) << engine->screens[screen_index].zoneVariants[i];
		//   cout << endl;
		// }
		Draw_Baked_Screen(&engine->screens[screen_index].screen, scr_buffer);
	}
	else {
		cout << "ERROR: no screen with the id given was found" << endl;
		cout << "\tids: ";
		auto print = [](const int& i) {
		  	cout << i << ' ';
		};
		for_each(zone_ids, zone_ids + engine->totalZones, print);
		cout << endl;
	}
}

int Find_Screen_By_ID(ZONED_SCREEN* screens, int total_screens, const int* zone_ids) {
  int index = 0;
  bool match = false;

  // Loop until a match is found or the whole array has been searched
  while(!match && index < total_screens) {
    // cout << "\tChecking zoned screen indexed #" << index << endl;
    // Check to see if this variant list and the variant list of the current screen match
    match = equal(screens[index].zoneVariants, screens[index].zoneVariants + screens[index].totalZones, zone_ids);
    // If these don't match, go to the next index
    if(!match) {
      index++;
    }
  }
  // If a match was found, return the index
  if(match) {
    return index;
  }
  // Otherwise, return invalid index
  else {
    return -1;
  }
}

/*
Screen management
*/
void Add_Zoned_Screen(BAKING_ENGINE* engine, BAKED_SCREEN* screen, int* zone_ids, int total_zones) {
  ZONED_SCREEN* new_screen = new ZONED_SCREEN;
  Initialize_Zoned_Screen_Copy(new_screen, screen, zone_ids, total_zones);
  Add_Zoned_Screen(engine, *new_screen);
  delete new_screen;
}

void Add_Zoned_Screen(BAKING_ENGINE* engine, ZONED_SCREEN screen) {
  // If array of zoned screens is full, expand it
  if(engine->totalScreens >= engine->screenCapacity) {
    Allocate_Screens(engine, engine->screenCapacity * 2);
  }
  // Put the screen at the last place in the array, then post-increment total screens
  engine->screens[engine->totalScreens++] = screen;
}

void Allocate_Screens(BAKING_ENGINE* engine, int new_capacity) {
  if(new_capacity > 0) {
    ZONED_SCREEN* new_screens = new ZONED_SCREEN[new_capacity];
    engine->screenCapacity = new_capacity;
    // Copy all existing screens into new screens
    for(int i = 0; i < engine->totalScreens; i++) {
      new_screens[i] = engine->screens[i];
    }
    // Make current sockets point to the new sockets array
    delete [] engine->screens;
    engine->screens = new_screens;
  }
}

/*
Zone management
*/
void Add_Exclusive_Drawing_Zone(BAKING_ENGINE* engine, int layer, string bmp_dirs, bool hasEmpty, int x, int y, int w, int h) {
  EXCLUSIVE_DRAWING_ZONE* new_zone = new EXCLUSIVE_DRAWING_ZONE;
  Initialize_Exclusive_Drawing_Zone(new_zone, layer, bmp_dirs, hasEmpty, x, y, w, h);
  Add_Exclusive_Drawing_Zone(engine, *new_zone);
  delete new_zone;
}

void Add_Exclusive_Drawing_Zone(BAKING_ENGINE* engine, int layer, const string* bmp_dirs, int total_bmps, bool hasEmpty, int x, int y, int w, int h) {
  EXCLUSIVE_DRAWING_ZONE* new_zone = new EXCLUSIVE_DRAWING_ZONE;
  Initialize_Exclusive_Drawing_Zone(new_zone, layer, bmp_dirs, total_bmps, hasEmpty, x, y, w, h);
  Add_Exclusive_Drawing_Zone(engine, *new_zone);
  delete new_zone;
}

void Add_Exclusive_Drawing_Zone_Move(BAKING_ENGINE* engine, int layer, BMP_SOCKET* sockets, int total_sockets) {
  EXCLUSIVE_DRAWING_ZONE* new_zone = new EXCLUSIVE_DRAWING_ZONE;
  Initialize_Exclusive_Drawing_Zone_Move(new_zone, layer, sockets, total_sockets);
  Add_Exclusive_Drawing_Zone(engine, *new_zone);
  delete new_zone;
}

void Add_Exclusive_Drawing_Zone(BAKING_ENGINE* engine, EXCLUSIVE_DRAWING_ZONE zone) {
  // If array of sockets is full, expand it
  if(engine->totalZones >= engine->zoneCapacity) {
    Allocate_Exclusive_Drawing_Zones(engine, engine->zoneCapacity * 2);
  }
  // Add the zone at the last index and increment total sockets after assignment
  engine->zones[engine->totalZones++] = zone;
}

void Allocate_Exclusive_Drawing_Zones(BAKING_ENGINE* engine, int new_capacity) {
  if(new_capacity > 0) {
    EXCLUSIVE_DRAWING_ZONE* new_zones = new EXCLUSIVE_DRAWING_ZONE[new_capacity];
    engine->zoneCapacity = new_capacity;
    // Copy all existing sockets into new sockets
    for(int i = 0; i < engine->totalZones; i++) {
      new_zones[i] = engine->zones[i];
    }
    // Make current sockets point to the new sockets array
    delete [] engine->zones;
    engine->zones = new_zones;
  }
}

void Sort_Exclusive_Drawing_Zones_By_Layer(EXCLUSIVE_DRAWING_ZONE* zones, int total_zones) {
  int inner_index;  // Inner looping index for the insertion sort algorithm
  EXCLUSIVE_DRAWING_ZONE temp_zone; // Temporary storage for the zone being sorted
  // Outer loop for the whole array
  for(int outer_index = 0; outer_index < total_zones; outer_index++) {
    // Store the current zone being sorted
    temp_zone = zones[outer_index];
    // Loop through all elements behind the one being sorted
    inner_index = outer_index - 1;
    // Loop until we reach the back of the array, or an id is found that is
    // less than the one being sored
    while(inner_index >= 0 && zones[inner_index].layer > temp_zone.layer) {
      zones[inner_index + 1] = zones[inner_index];
      inner_index--;
    }
    // Assign the sorted element to the part of the array we stopped at
    zones[inner_index + 1] = temp_zone;
  }
}

int Calculate_Total_Zoned_Screens(BAKING_ENGINE* engine) {
	int total_screens = 1;
	// Create a temporary function object that muliplies the local variable
	// by the total sockets on the drawing zone
	auto mutliply_total_sockets = [&total_screens]
	(const EXCLUSIVE_DRAWING_ZONE& zone) {
		total_screens *= zone.totalSockets;
	};
	// Multiply total screens by the total sockets of each zone
	for_each(engine->zones, engine->zones + engine->totalZones,
		mutliply_total_sockets);
	return total_screens;
}
