/* See LICENSE file for copyright and license details. */

#include "push.c"           /* http://dwm.suckless.org/patches/push */
#include <X11/XF86keysym.h> /* XFree volume keys */

/* appearance */
static const char *const fonts[] = {
	"terminus:size=8"
};
static const char dmenufont[] = "terminus:size=8";
static const char normbordercolor[] = "#888888";
static const char normbgcolor[]     = "#000000";
static const char normfgcolor[]     = "#ffffff";
static const char selbordercolor[]  = "#ffffff";
static const char selbgcolor[]      = "#ffffff";
static const char selfgcolor[]      = "#000000";
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
// 	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
// 	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
	{ "<unknown>", "<unknown>", "<unknown>", 0, True, -1 }, // fullscreen flash
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]  = { "uxterm", NULL };
static const char *browsercmd[]  = { "firefox", NULL };
static const char *sdwmcmd[]  = { "sdwm", NULL };
static const char *sopenboxcmd[]  = { "sopenbox", NULL };
static const char *scompizcmd[]  = { "scompiz", NULL };
static const char *vupcmd[]  = { "vup", NULL };
static const char *vdowncmd[]  = { "vdown", NULL };
static const char *vmaxcmd[]  = { "vmax", NULL };
static const char *vmutecmd[]  = { "vmute", NULL };
static const char *mopencmd[]  = { "mopen", NULL };
static const char *mnextcmd[]  = { "mnext", NULL };
static const char *mprevcmd[]  = { "mprev", NULL };
static const char *mpausecmd[]  = { "mpause", NULL };
static const char *togglemousecmd[]  = { "togglemouse", NULL };
static const char *dwmstatus_update[]  = { "pkill", "-HUP", "dwmstatus", NULL };
static const char *slockcmd[]  = { "slock", NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */
	// launch commands
	{ ControlMask|ShiftMask,        XK_s,      spawn,          {.v = termcmd } },
	{ ControlMask|ShiftMask,        XK_d,      spawn,          {.v = browsercmd } },
	{ ControlMask|ShiftMask,        XK_r,      spawn,          {.v = dmenucmd } },
	// switch window manager
	{ ControlMask|ShiftMask,        XK_F10,    spawn,          {.v = scompizcmd } },
	{ ControlMask|ShiftMask,        XK_F11,    spawn,          {.v = sopenboxcmd } },
	{ ControlMask|ShiftMask,        XK_F12,    spawn,          {.v = sdwmcmd } },
	// volume
	{ ControlMask|ShiftMask,        XK_Down,   spawn,          {.v = vdowncmd } },
	{ ControlMask|ShiftMask,        XK_Up,     spawn,          {.v = vupcmd } },
	{ ControlMask|ShiftMask,        XK_Left,   spawn,          {.v = vmutecmd } },
// 	{ NULL,                         XF86XK_AudioLowerVolume, spawn, {.v = vdowncmd } },
// 	{ NULL,                         XF86XK_AudioRaiseVolume, spawn, {.v = vupcmd } },
// 	{ NULL,                         XF86XK_AudioMute,        spawn, {.v = vmutecmd } },
	{ ControlMask|ShiftMask,        XK_Right,  spawn,          {.v = vmaxcmd } },
	// media shortcuts
	{ ControlMask|ShiftMask,        XK_y,      spawn,          {.v = mopencmd } },
	{ ControlMask|ShiftMask,        XK_u,      spawn,          {.v = mpausecmd } },
	{ ControlMask|ShiftMask,        XK_i,      spawn,          {.v = mnextcmd } },
	{ ControlMask|ShiftMask,        XK_o,      spawn,          {.v = mprevcmd } },
	// miscellaneous commands
	{ ControlMask|ShiftMask,        XK_F1,     spawn,          {.v = togglemousecmd } },
	{ ControlMask|ShiftMask,        XK_F2,     spawn,          {.v = dwmstatus_update } },
	{ ControlMask|ShiftMask,        XK_Delete,spawn,           {.v = slockcmd } },
	// miscellaneous functionality
	{ ControlMask|ShiftMask,        XK_q,      killclient,     {0} },
	{ ControlMask|ShiftMask,        XK_v,      togglebar,      {0} },
	{ ControlMask|ShiftMask,        XK_space,  togglefloating, {0} },
	// move/focus
	{ ControlMask|ShiftMask,        XK_comma,  focusmon,       {.i = +1 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = +1 } },
	{ ControlMask|ShiftMask,        XK_period, focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = -1 } },
	{ ControlMask|ShiftMask,        XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ ControlMask|ShiftMask,        XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	// move windows
	{ ControlMask|ShiftMask,        XK_n,      pushdown,       {0} },
	{ MODKEY,                       XK_n,      pushdown,       {0} },
	{ ControlMask|ShiftMask,        XK_p,      pushup,         {0} },
	{ MODKEY,                       XK_p,      pushup,         {0} },
	{ ShiftMask|MODKEY,             XK_comma,  tagmon,         {.i = +1 } },
	{ ShiftMask|MODKEY,             XK_period, tagmon,         {.i = -1 } },
	{ ControlMask|ShiftMask,        XK_a,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_a,      incnmaster,     {.i = +1 } },
	{ ControlMask|ShiftMask,        XK_x,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_x,      incnmaster,     {.i = -1 } },
	{ ControlMask|ShiftMask,        XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ ControlMask|ShiftMask,        XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ ControlMask|ShiftMask,        XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	// set layout
	{ ControlMask|ShiftMask,        XK_t,      setlayout,      {.v = &layouts[0]} },
	{ ControlMask|ShiftMask,        XK_f,      setlayout,      {.v = &layouts[1]} },
	{ ControlMask|ShiftMask,        XK_m,      setlayout,      {.v = &layouts[2]} },
	// tags
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	TAGKEYS(                        XK_KP_Insert,              9)
	TAGKEYS(                        XK_KP_End,                 10)
	TAGKEYS(                        XK_KP_Down,                11)
	TAGKEYS(                        XK_KP_Next,                12)
	TAGKEYS(                        XK_KP_Left,                13)
	TAGKEYS(                        XK_KP_Begin,               14)
	TAGKEYS(                        XK_KP_Right,               15)
	TAGKEYS(                        XK_KP_Home,                16)
	TAGKEYS(                        XK_KP_Up,                  17)
	TAGKEYS(                        XK_KP_Prior,               18)
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        zoom,           {0} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
