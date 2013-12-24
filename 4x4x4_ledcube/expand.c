// ############################################
// 
// Derivative 4x4x4 LED Cube project
// By Christian Moen 2008
// chr@syntaks.org
// Lisence: GPL
//
// Custom effects by Sublime 2013
//
// ############################################

// See included raindrop diagram pdf for color coordinates

// Expands a voxel in four directions on the z plane (original voxel, purple, red, green, yellow, grey, black)

void expand_ring (char* color, int x, int y, int z, int state);

void expand_ring (char* color, int x, int y, int z, int state)
{
	if (color == "purple")
	{
		altervoxel(x+1,y,z,state);
		altervoxel(x-1,y,z,state);
		altervoxel(x,y+1,z,state);
		altervoxel(x,y-1,z,state);
	}
	
	if (color == "red")
	{
		altervoxel(x+2,y,z,state);
		altervoxel(x-2,y,z,state);
		altervoxel(x,y+2,z,state);
		altervoxel(x,y-2,z,state);
		altervoxel(x+1,y+1,z,state);
		altervoxel(x-1,y+1,z,state);
		altervoxel(x+1,y-1,z,state);
		altervoxel(x-1,y-1,z,state);
	}
	
	if (color == "green")
	{
		altervoxel(x+2,y+1,z,state);
		altervoxel(x+1,y+2,z,state);
		altervoxel(x,y+3,z,state);
		altervoxel(x-1,y+2,z,state);
		altervoxel(x-2,y+1,z,state);
		altervoxel(x-3,y,z,state);
		altervoxel(x-2,y-1,z,state);
		altervoxel(x-1,y-2,z,state);
		altervoxel(x,y-3,z,state);
		altervoxel(x+1,y-2,z,state);
		altervoxel(x+2,y-1,z,state);
		altervoxel(x+3,y,z,state);
	}
	
	if (color == "yellow")
	{
		altervoxel(x+3,y+1,z,state);
		altervoxel(x+2,y+2,z,state);
		altervoxel(x+1,y+3,z,state);
		altervoxel(x-1,y+3,z,state);
		altervoxel(x-2,y+2,z,state);
		altervoxel(x-3,y+1,z,state);
		altervoxel(x-3,y-1,z,state);
		altervoxel(x-2,y-2,z,state);
		altervoxel(x-1,y-3,z,state);
		altervoxel(x+1,y-3,z,state);
		altervoxel(x+2,y-2,z,state);
		altervoxel(x+3,y-1,z,state);
	}
	
	if (color == "grey")
	{
		altervoxel(x+3,y+2,z,state);
		altervoxel(x+2,y+3,z,state);
		altervoxel(x-2,y+3,z,state);
		altervoxel(x-3,y+2,z,state);
		altervoxel(x-3,y-2,z,state);
		altervoxel(x-2,y-3,z,state);
		altervoxel(x+2,y-3,z,state);
		altervoxel(x+3,y-2,z,state);
	}
	
	if (color == "black")
	{
		altervoxel(x+3,y+3,z,state);
		altervoxel(x-3,y+3,z,state);
		altervoxel(x-3,y-3,z,state);
		altervoxel(x+3,y-3,z,state);
	}
	
}
