/* user and group to drop privileges to */
static const char *user  = "eric";
static const char *group = "wheel";

static const char *colorname[NUMCOLS] = {
	[INIT]   = "#000000",   /* after initialization */
	//[INPUT] =  "#0047bd",   /* during input */
	//[INPUT] =  "#4d0099",   /* during input */
	[INPUT]  = "#136dff",   /* during input */
	[FAILED] = "#ff0000",   /* wrong password */
};

/* treat a cleared input like a wrong password (color) */
static const int failonclear = 1;
