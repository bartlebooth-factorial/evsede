/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static int showbar                  = 1;        /* 0 means no bar */
static int topbar                   = 1;        /* 0 means bottom bar */
static const double activeopacity   = 1.0f;     /* Window opacity when it's focused (0 <= opacity <= 1) */
static const double inactiveopacity = 0.60f;    /* Window opacity when it's inactive (0 <= opacity <= 1) */
static Bool bUseOpacity             = False;    /* Starts with opacity on any unfocused windows */
static const char *fonts[]          = { "monospace:size=16" };
static const char dmenufont[]       = "monospace:size=20";
static const char col_gray0[]       = "#111111";
static const char col_gray1[]       = "#10100e";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_white[]       = "#ffffff";
static const char col_purple[]      = "#4d0099";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray0 },
	[SchemeSel]  = { col_gray4, col_gray1, col_purple },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Alacritty",  NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ "kitty",      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,		NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact        = 0.50; /* factor of master area size [0.05..0.95] */
static const int nmaster        = 1;    /* number of clients in master area */
static const int resizehints    = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1;    /* 1 will force focus on the fullscreen window */
static       int attachbelow    = 0;    /* 1 means attach after the currently active window */

#include "vanitygaps.c"
#include "tatami.c"

static const Layout layouts[] = {
	/* symbol     arrange function          keybind (Mod-z Mod-{key})   [first entry is default] */
	{ "[]=",      tile },                   // h
	{ "[M]",      monocle },                // j
	{ "TTT",      bstack },                 // k
	{ "[\\]",     dwindle },                // l
	{ ":::",      gaplessgrid },            // g
	{ "|M|",      centeredmaster },         // n
	{ ">M>",      centeredfloatingmaster }, // m
	{ "[@]",      spiral },                 // y
	{ "H[]",      deck },                   // u
	{ "|+|",      tatami },                 // i
	{ "><>",      NULL },                   // o
	{ NULL,       NULL },
};

#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	&((Keychord){1, {{MODKEY, KEY}}, view, {.ui = 1 << TAG} }), \
		&((Keychord){1, {{MODKEY|ControlMask, KEY}}, toggleview, {.ui = 1 << TAG} }), \
		&((Keychord){1, {{MODKEY|ShiftMask, KEY}}, tag, {.ui = 1 << TAG} }), \
		&((Keychord){1, {{MODKEY|ControlMask|ShiftMask, KEY}}, toggletag, {.ui = 1 << TAG} }),

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *browsercmd[] = { "qutebrowser", NULL };
static const char *dmenucmd[]   = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_purple, "-sf", col_gray4, NULL };
static const char *emacscmd[]   = { "emacsclient", "-c", NULL };
static const char *lockcmd[]    = { "slock", NULL };
static const char *termcmd[]    = { "kitty", NULL };
static const char *winselcmd[]  = { "rofi", "-show", "window", "-matching", "fuzzy", NULL };

#include "selfrestart.c"

static Keychord *keychords[] = {

	/* Moving between windows */

	&((Keychord){1, {{MODKEY, XK_j}},     focusstack,  {.i = +1 }        }),
	&((Keychord){1, {{MODKEY, XK_k}},     focusstack,  {.i = -1 }        }),
	&((Keychord){1, {{MODKEY, XK_space}}, focusmaster, {0}               }),
	&((Keychord){1, {{MODKEY, XK_w}},     spawn,       {.v = winselcmd } }),

	/* Changing window sizes */

	&((Keychord){1, {{MODKEY, XK_h}},      setmfact,      {.f = -0.05} }),
	&((Keychord){1, {{MODKEY, XK_l}},      setmfact,      {.f = +0.05} }),
	&((Keychord){1, {{MODKEY, XK_comma}},  incnmaster,    {.i = +1 }   }),
	&((Keychord){1, {{MODKEY, XK_period}}, incnmaster,    {.i = -1 }   }),
	&((Keychord){1, {{MODKEY, XK_f}},      togglefullscr, {0}          }),

	/* Altering the stack */

	&((Keychord){1, {{ MODKEY, XK_Return}},      zoom,        {0}        }),
	&((Keychord){1, {{ MODKEY|ShiftMask, XK_j}}, rotatestack, {.i = +1 } }),
	&((Keychord){1, {{ MODKEY|ShiftMask, XK_k}}, rotatestack, {.i = -1 } }),

	/* Spawning programs */

	&((Keychord){1, {{ MODKEY, XK_b}},                spawn, {.v = browsercmd } }),
	&((Keychord){1, {{ MODKEY, XK_e}},                spawn, {.v = emacscmd }   }),
	&((Keychord){1, {{ MODKEY, XK_p}},                spawn, {.v = dmenucmd }   }),
	&((Keychord){1, {{ MODKEY|ShiftMask, XK_l}},      spawn, {.v = lockcmd }    }),
	&((Keychord){1, {{ MODKEY|ShiftMask, XK_Return}}, spawn, {.v = termcmd }    }),

	/* Scripts */

	&((Keychord){2, {{MODKEY, XK_s}, {0, XK_b}},     spawn, SHCMD("chbg")                          }),
	&((Keychord){2, {{MODKEY, XK_s}, {0, XK_c}},     spawn, SHCMD("configmenu")                    }),
	&((Keychord){2, {{MODKEY, XK_s}, {0, XK_e}},     spawn, SHCMD("qutebrowser https://gmail.com") }),
	&((Keychord){2, {{MODKEY, XK_s}, {0, XK_l}},     spawn, SHCMD("alacritty -e lf")               }),
	&((Keychord){2, {{MODKEY, XK_s}, {0, XK_m}},     spawn, SHCMD("sysmenu")                       }),
	&((Keychord){2, {{MODKEY, XK_s}, {0, XK_p}},     spawn, SHCMD("passmenu")                      }),
	&((Keychord){2, {{MODKEY, XK_s}, {0, XK_s}},     spawn, SHCMD("flameshot gui")                 }),
	&((Keychord){2, {{MODKEY, XK_s}, {0, XK_t}},     spawn, SHCMD("emacsclient -c ~/.todo.org")    }),
	&((Keychord){2, {{MODKEY, XK_s}, {0, XK_u}},     spawn, SHCMD("alacritty -e sysup")            }),
	&((Keychord){2, {{MODKEY, XK_s}, {0, XK_w}},     spawn, SHCMD("webmenu")                       }),
	&((Keychord){2, {{MODKEY, XK_s}, {0, XK_equal}}, spawn, SHCMD("rofiqalc")                      }),

	/* Music player */

	&((Keychord){2, {{MODKEY, XK_m}, {0,         XK_m}}, spawn, SHCMD("alacritty -e cmus")                     }),
	&((Keychord){2, {{MODKEY, XK_m}, {0,         XK_p}}, spawn, SHCMD("cmus-remote -u")                        }),
	&((Keychord){2, {{MODKEY, XK_m}, {0,         XK_j}}, spawn, SHCMD("cmus-remote -n")                        }),
	&((Keychord){2, {{MODKEY, XK_m}, {0,         XK_k}}, spawn, SHCMD("cmus-remote -r")                        }),
	&((Keychord){2, {{MODKEY, XK_m}, {0,         XK_h}}, spawn, SHCMD("cmus-remote -k -10")                    }),
	&((Keychord){2, {{MODKEY, XK_m}, {0,         XK_l}}, spawn, SHCMD("cmus-remote -k +10")                    }),
	&((Keychord){2, {{MODKEY, XK_m}, {ShiftMask, XK_h}}, spawn, SHCMD("cmus-remote -k -60")                    }),
	&((Keychord){2, {{MODKEY, XK_m}, {ShiftMask, XK_l}}, spawn, SHCMD("cmus-remote -k +60")                    }),
	&((Keychord){2, {{MODKEY, XK_m}, {0,         XK_o}}, spawn, SHCMD("cmus-remote -C 'toggle repeat_current") }),
	&((Keychord){2, {{MODKEY, XK_m}, {0,         XK_i}}, spawn, SHCMD("alacritty -e cmus_info")                }),
	/* &((Keychord){2, {{MODKEY, XK_m}, {0,         XK_m}}, spawn, SHCMD("alacritty -e mocp")      }), */
	/* &((Keychord){2, {{MODKEY, XK_m}, {0,         XK_p}}, spawn, SHCMD("mocp -G")                }), */
	/* &((Keychord){2, {{MODKEY, XK_m}, {0,         XK_j}}, spawn, SHCMD("mocp -f")                }), */
	/* &((Keychord){2, {{MODKEY, XK_m}, {0,         XK_k}}, spawn, SHCMD("mocp -r")                }), */
	/* &((Keychord){2, {{MODKEY, XK_m}, {0,         XK_h}}, spawn, SHCMD("mocp -k -10")            }), */
	/* &((Keychord){2, {{MODKEY, XK_m}, {0,         XK_l}}, spawn, SHCMD("mocp -k +10")            }), */
	/* &((Keychord){2, {{MODKEY, XK_m}, {ShiftMask, XK_h}}, spawn, SHCMD("mocp -k -60")            }), */
	/* &((Keychord){2, {{MODKEY, XK_m}, {ShiftMask, XK_l}}, spawn, SHCMD("mocp -k +60")            }), */
	/* &((Keychord){2, {{MODKEY, XK_m}, {0,         XK_o}}, spawn, SHCMD("mocp -t repeat")         }), */
	/* &((Keychord){2, {{MODKEY, XK_m}, {0,         XK_s}}, spawn, SHCMD("mocp -t shuffle")        }), */
	/* &((Keychord){2, {{MODKEY, XK_m}, {0,         XK_i}}, spawn, SHCMD("alacritty -e mocp_info") }), */

	/* Mouse movement */

	&((Keychord){1, {{ MODKEY, XK_Up}},                spawn, SHCMD("xdotool mousemove_relative --sync -- 0 -10")  }),
	&((Keychord){1, {{ MODKEY, XK_Down}},              spawn, SHCMD("xdotool mousemove_relative --sync -- 0 10")   }),
	&((Keychord){1, {{ MODKEY, XK_Left}},              spawn, SHCMD("xdotool mousemove_relative --sync -- -10 0")  }),
	&((Keychord){1, {{ MODKEY, XK_Right}},             spawn, SHCMD("xdotool mousemove_relative --sync -- 10 0")   }),
	&((Keychord){1, {{ MODKEY|ControlMask, XK_Up}},    spawn, SHCMD("xdotool mousemove_relative --sync -- 0 -100") }),
	&((Keychord){1, {{ MODKEY|ControlMask, XK_Down}},  spawn, SHCMD("xdotool mousemove_relative --sync -- 0 100")  }),
	&((Keychord){1, {{ MODKEY|ControlMask, XK_Left}},  spawn, SHCMD("xdotool mousemove_relative --sync -- -100 0") }),
	&((Keychord){1, {{ MODKEY|ControlMask, XK_Right}}, spawn, SHCMD("xdotool mousemove_relative --sync -- 100 0")  }),
	&((Keychord){1, {{ MODKEY|ShiftMask, XK_Left}},    spawn, SHCMD("xdotool click 1") }),
	&((Keychord){1, {{ MODKEY|ShiftMask, XK_Right}},   spawn, SHCMD("xdotool click 3") }),

	/* Closing programs */

	&((Keychord){1, {{MODKEY, XK_q}}, killclient, {0} }),
	&((Keychord){1, {{MODKEY, XK_x}}, killunsel,  {0} }),

	/* Layouts */

	&((Keychord){2, {{MODKEY, XK_z}, {0, XK_h}}, setlayout,   {.v = &layouts[0]}  }), // tile
	&((Keychord){2, {{MODKEY, XK_z}, {0, XK_j}}, setlayout,   {.v = &layouts[1]}  }), // monocle
	&((Keychord){2, {{MODKEY, XK_z}, {0, XK_k}}, setlayout,   {.v = &layouts[2]}  }), // vtile
	&((Keychord){2, {{MODKEY, XK_z}, {0, XK_l}}, setlayout,   {.v = &layouts[3]}  }), // dwindle
	&((Keychord){2, {{MODKEY, XK_z}, {0, XK_g}}, setlayout,   {.v = &layouts[4]}  }), // grid
	&((Keychord){2, {{MODKEY, XK_z}, {0, XK_n}}, setlayout,   {.v = &layouts[5]}  }), // centeredmaster
	&((Keychord){2, {{MODKEY, XK_z}, {0, XK_m}}, setlayout,   {.v = &layouts[6]}  }), // centeredfloatingmaster
	&((Keychord){2, {{MODKEY, XK_z}, {0, XK_y}}, setlayout,   {.v = &layouts[7]}  }), // spiral
	&((Keychord){2, {{MODKEY, XK_z}, {0, XK_u}}, setlayout,   {.v = &layouts[8]}  }), // deck
	&((Keychord){2, {{MODKEY, XK_z}, {0, XK_i}}, setlayout,   {.v = &layouts[9]}  }), // tatami
	&((Keychord){2, {{MODKEY, XK_z}, {0, XK_o}}, setlayout,   {.v = &layouts[10]} }), // floating
	&((Keychord){1, {{MODKEY, XK_bracketleft}},  cyclelayout, {.i = -1 }          }), // [prev layout]
	&((Keychord){1, {{MODKEY, XK_bracketright}}, cyclelayout, {.i = +1 }          }), // [next layout]

	/* Options */

	&((Keychord){2, {{MODKEY, XK_t}, {0, XK_a}}, toggleAttachBelow, {0} }),
	&((Keychord){2, {{MODKEY, XK_t}, {0, XK_b}}, togglebar,         {0} }),
	&((Keychord){2, {{MODKEY, XK_t}, {0, XK_f}}, togglefloating,    {0} }),
	&((Keychord){2, {{MODKEY, XK_t}, {0, XK_g}}, togglegaps,        {0} }),
	&((Keychord){2, {{MODKEY, XK_t}, {0, XK_o}}, toggleopacity,     {0} }),
	&((Keychord){2, {{MODKEY, XK_t}, {0, XK_t}}, toggletopbar,      {0} }),

	/* Tags */

	TAGKEYS(                                          XK_1,                  0)
	TAGKEYS(                                          XK_2,                  1)
	TAGKEYS(                                          XK_3,                  2)
	TAGKEYS(                                          XK_4,                  3)
	TAGKEYS(                                          XK_5,                  4)
	TAGKEYS(                                          XK_6,                  5)
	TAGKEYS(                                          XK_7,                  6)
	TAGKEYS(                                          XK_8,                  7)
	TAGKEYS(                                          XK_9,                  8)
	&((Keychord){1, {{MODKEY,           XK_Tab}},     view,              {0} }),
	&((Keychord){1, {{MODKEY|ShiftMask, XK_0}},       view,      {.ui = ~0 } }),
	&((Keychord){1, {{MODKEY,           XK_minus}},   viewprev,          {0} }),
	&((Keychord){1, {{MODKEY,           XK_equal}},   viewnext,          {0} }),
	&((Keychord){1, {{MODKEY|ShiftMask, XK_minus}},   tagtoprev,         {0} }),
	&((Keychord){1, {{MODKEY|ShiftMask, XK_equal}},   tagtonext,         {0} }),

	/* Moniters */

	&((Keychord){1, {{MODKEY, XK_comma}},            focusmon, {.i = -1 } }),
	&((Keychord){1, {{MODKEY, XK_period}},           focusmon, {.i = +1 } }),
	&((Keychord){1, {{MODKEY|ShiftMask, XK_comma}},  tagmon,   {.i = -1 } }),
	&((Keychord){1, {{MODKEY|ShiftMask, XK_period}}, tagmon,   {.i = +1 } }),

	/* Custom functions */

	&((Keychord){1, {{MODKEY, XK_0}}, cornercursor, {0} }),

	/* Quitting and restarting dwm */

	&((Keychord){1, {{MODKEY|ShiftMask, XK_q}}, quit, {0} }),
	&((Keychord){1, {{MODKEY, XK_r}},           quit, {1} }), 
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

