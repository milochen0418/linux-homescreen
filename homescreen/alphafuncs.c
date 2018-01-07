

#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <gtk/gtk.h>
#include <clutter-gtk/clutter-gtk.h>
#include <math.h>
#include "alphafuncs.h"




static gdouble clutter_linear (ClutterAlpha *alpha,  gpointer data);
static gdouble clutter_ease_in_quad (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_out_quad (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_out_quad (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_cubic (ClutterAlpha *alpha,gpointer data);
static gdouble clutter_ease_out_cubic (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_out_cubic (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_quart (ClutterAlpha *alpha,  gpointer data);
static gdouble clutter_ease_out_quart (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_out_quart (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_quint (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_out_quint (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_out_quint (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_sine (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_out_sine (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_out_sine (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_expo (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_out_expo (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_out_expo (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_circ (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_out_circ (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_out_circ (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_elastic (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_out_elastic (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_out_elastic (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_back (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_out_back (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_out_back (ClutterAlpha *alpha, gpointer data);
static gdouble ease_out_bounce_internal (gdouble t, gdouble d);
static gdouble ease_in_bounce_internal (gdouble t, gdouble d);
static gdouble clutter_ease_in_bounce (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_out_bounce (ClutterAlpha *alpha, gpointer data);
static gdouble clutter_ease_in_out_bounce (ClutterAlpha *alpha, gpointer data);



typedef struct {
	gulong mode;
	ClutterAlphaFunc func;
} mode_pair_t;

ClutterAlphaFunc clutter_get_default_alpha_func(gulong mode)
{

static const mode_pair_t  modes[] = 
{
  { CLUTTER_CUSTOM_MODE,         NULL },

  { CLUTTER_LINEAR,              clutter_linear },
  { CLUTTER_EASE_IN_QUAD,        clutter_ease_in_quad },
  { CLUTTER_EASE_OUT_QUAD,       clutter_ease_out_quad },
  { CLUTTER_EASE_IN_OUT_QUAD,    clutter_ease_in_out_quad },
  { CLUTTER_EASE_IN_CUBIC,       clutter_ease_in_cubic },
  { CLUTTER_EASE_OUT_CUBIC,      clutter_ease_out_cubic },
  { CLUTTER_EASE_IN_OUT_CUBIC,   clutter_ease_in_out_cubic },
  { CLUTTER_EASE_IN_QUART,       clutter_ease_in_quart },
  { CLUTTER_EASE_OUT_QUART,      clutter_ease_out_quart },
  { CLUTTER_EASE_IN_OUT_QUART,   clutter_ease_in_out_quart },
  { CLUTTER_EASE_IN_QUINT,       clutter_ease_in_quint },
  { CLUTTER_EASE_OUT_QUINT,      clutter_ease_out_quint },
  { CLUTTER_EASE_IN_OUT_QUINT,   clutter_ease_in_out_quint },
  { CLUTTER_EASE_IN_SINE,        clutter_ease_in_sine },
  { CLUTTER_EASE_OUT_SINE,       clutter_ease_out_sine },
  { CLUTTER_EASE_IN_OUT_SINE,    clutter_ease_in_out_sine },
  { CLUTTER_EASE_IN_EXPO,        clutter_ease_in_expo },
  { CLUTTER_EASE_OUT_EXPO,       clutter_ease_out_expo },
  { CLUTTER_EASE_IN_OUT_EXPO,    clutter_ease_in_out_expo },
  { CLUTTER_EASE_IN_CIRC,        clutter_ease_in_circ },
  { CLUTTER_EASE_OUT_CIRC,       clutter_ease_out_circ },
  { CLUTTER_EASE_IN_OUT_CIRC,    clutter_ease_in_out_circ },
  { CLUTTER_EASE_IN_ELASTIC,     clutter_ease_in_elastic },
  { CLUTTER_EASE_OUT_ELASTIC,    clutter_ease_out_elastic },
  { CLUTTER_EASE_IN_OUT_ELASTIC, clutter_ease_in_out_elastic },
  { CLUTTER_EASE_IN_BACK,        clutter_ease_in_back },
  { CLUTTER_EASE_OUT_BACK,       clutter_ease_out_back },
  { CLUTTER_EASE_IN_OUT_BACK,    clutter_ease_in_out_back },
  { CLUTTER_EASE_IN_BOUNCE,      clutter_ease_in_bounce },
  { CLUTTER_EASE_OUT_BOUNCE,     clutter_ease_out_bounce },
  { CLUTTER_EASE_IN_OUT_BOUNCE,  clutter_ease_in_out_bounce },
  { CLUTTER_ANIMATION_LAST,      NULL },
};
	int number_of_modes = sizeof(modes) / sizeof(mode_pair_t);
	int idx;
	for(idx=0; idx<number_of_modes; idx++)
	{
		if(modes[idx].mode == mode)
		{
			return modes[idx].func;
		}
	} 
	return NULL;
} //clutter_get_default_alpha_func()




static gdouble clutter_linear (ClutterAlpha *alpha,  gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  return clutter_timeline_get_progress (timeline);
}

static gdouble clutter_ease_in_quad (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble progress = clutter_timeline_get_progress (timeline);
  return progress * progress;
}

static gdouble clutter_ease_out_quad (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble p = clutter_timeline_get_progress (timeline);
  return -1.0 * p * (p - 2);
}

static gdouble clutter_ease_in_out_quad (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = t / (d / 2);

  if (p < 1)
    return 0.5 * p * p;

  p -= 1;

  return -0.5 * (p * (p - 2) - 1);
}

static gdouble clutter_ease_in_cubic (ClutterAlpha *alpha,gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);

  gdouble p = clutter_timeline_get_progress (timeline);

  return p * p * p;
}

static gdouble clutter_ease_out_cubic (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = t / d - 1;

  return p * p * p + 1;
}

static gdouble
clutter_ease_in_out_cubic (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = t / (d / 2);

  if (p < 1)
    return 0.5 * p * p * p;

  p -= 2;

  return 0.5 * (p * p * p + 2);
}

static gdouble clutter_ease_in_quart (ClutterAlpha *alpha,  gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble p = clutter_timeline_get_progress (timeline);

  return p * p * p * p;
}

static gdouble  clutter_ease_out_quart (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = t / d - 1;

  return -1.0 * (p * p * p * p - 1);
}

static gdouble clutter_ease_in_out_quart (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = t / (d / 2);

  if (p < 1)
    return 0.5 * p * p * p * p;

  p -= 2;

  return -0.5 * (p * p * p * p - 2);
}

static gdouble clutter_ease_in_quint (ClutterAlpha *alpha, gpointer data)
 {
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble p = clutter_timeline_get_progress (timeline);

  return p * p * p * p * p;
} //clutter_ease_in_quint()

static gdouble clutter_ease_out_quint (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = t / d - 1;

  return p * p * p * p * p + 1;
}

static gdouble clutter_ease_in_out_quint (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = t / (d / 2);

  if (p < 1)
    return 0.5 * p * p * p * p * p;

  p -= 2;

  return 0.5 * (p * p * p * p * p + 2);
}

static gdouble clutter_ease_in_sine (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);

  return -1.0 * cos (t / d * G_PI_2) + 1.0;
}

static gdouble clutter_ease_out_sine (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);

  return sin (t / d * G_PI_2);
}

static gdouble clutter_ease_in_out_sine (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);

  return -0.5 * (cos (G_PI * t / d) - 1);
}

static gdouble clutter_ease_in_expo (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);

  return (t == 0) ? 0.0 : pow (2, 10 * (t / d - 1));
}

static gdouble clutter_ease_out_expo (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);

  return (t == d) ? 1.0 : -pow (2, -10 * t / d) + 1;
}

static gdouble clutter_ease_in_out_expo (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p;

  if (t == 0)
    return 0.0;

  if (t == d)
    return 1.0;

  p = t / (d / 2);

  if (p < 1)
    return 0.5 * pow (2, 10 * (p - 1));

  p -= 1;

  return 0.5 * (-pow (2, -10 * p) + 2);
}

static gdouble clutter_ease_in_circ (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble p = clutter_timeline_get_progress (timeline);

  return -1.0 * (sqrt (1 - p * p) - 1);
}

static gdouble clutter_ease_out_circ (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = t / d - 1;

  return sqrt (1 - p * p);
}

static gdouble clutter_ease_in_out_circ (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = t / (d / 2);

  if (p < 1)
    return -0.5 * (sqrt (1 - p * p) - 1);

  p -= 2;

  return 0.5 * (sqrt (1 - p * p) + 1);
}

static gdouble clutter_ease_in_elastic (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = d * .3;
  gdouble s = p / 4;
  gdouble q = t / d;

  if (q == 1)
    return 1.0;

  q -= 1;

  return -(pow (2, 10 * q) * sin ((q * d - s) * (2 * G_PI) / p));
}

static gdouble clutter_ease_out_elastic (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = d * .3;
  gdouble s = p / 4;
  gdouble q = t / d;

  if (q == 1)
    return 1.0;

  return pow (2, -10 * q) * sin ((q * d - s) * (2 * G_PI) / p) + 1.0;
}

static gdouble clutter_ease_in_out_elastic (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = d * (.3 * 1.5);
  gdouble s = p / 4;
  gdouble q = t / (d / 2);

  if (q == 2)
    return 1.0;

  if (q < 1)
    {
      q -= 1;

      return -.5 * (pow (2, 10 * q) * sin ((q * d - s) * (2 * G_PI) / p));
    }
  else
    {
      q -= 1;

      return pow (2, -10 * q)
           * sin ((q * d - s) * (2 * G_PI) / p)
           * .5 + 1.0;
    }
}

static gdouble clutter_ease_in_back (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble p = clutter_timeline_get_progress (timeline);

  return p * p * ((1.70158 + 1) * p - 1.70158);
}

static gdouble clutter_ease_out_back (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = t / d - 1;

  return p * p * ((1.70158 + 1) * p + 1.70158) + 1;
}

static gdouble clutter_ease_in_out_back (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);
  gdouble p = t / (d / 2);
  gdouble s = 1.70158 * 1.525;

  if (p < 1)
    return 0.5 * (p * p * ((s + 1) * p - s));

  p -= 2;

  return 0.5 * (p * p * ((s + 1) * p + s) + 2);
}

static gdouble ease_out_bounce_internal (gdouble t, gdouble d)
{
  gdouble p = t / d;

  if (p < (1 / 2.75))
    return 7.5625 * p * p;
  else if (p < (2 / 2.75))
    {
      p -= (1.5 / 2.75);

      return 7.5625 * p * p + .75;
    }
  else if (p < (2.5 / 2.75))
    {
      p -= (2.25 / 2.75);

      return 7.5625 * p * p + .9375;
    }
  else
    {
      p -= (2.625 / 2.75);

      return 7.5625 * p * p + .984375;
    }
}

static gdouble ease_in_bounce_internal (gdouble t, gdouble d)
{
  return 1.0 - ease_out_bounce_internal (d - t, d);
}

static gdouble clutter_ease_in_bounce (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);

  return ease_in_bounce_internal (t, d);
}

static gdouble clutter_ease_out_bounce (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);

  return ease_out_bounce_internal (t, d);
}

static gdouble clutter_ease_in_out_bounce (ClutterAlpha *alpha, gpointer data)
{
  ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
  gdouble t = clutter_timeline_get_elapsed_time (timeline);
  gdouble d = clutter_timeline_get_duration (timeline);

  if (t < d / 2)
    return ease_in_bounce_internal (t * 2, d) * 0.5;
  else
    return ease_out_bounce_internal (t * 2 - d, d) * 0.5 + 1.0 * 0.5;
}














