/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *
 * Copyright (C) 2006 OpenedHand
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:clutter-behaviour-scale
 * @short_description: A behaviour controlling scale
 *
 * A #ClutterBehaviourScaleGravity interpolates actors size between two values.
 *
 */

#if 0

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif





#include <clutter/clutter-actor.h>
#include <clutter/clutter-behaviour.h>
#include <clutter/clutter-enum-types.h>
#include <clutter/clutter-main.h>
#include <clutter/clutter-behaviour-scale.h>
//#include <clutter/clutter-private.h>
//#include <clutter/clutter-debug.h>


#else


#include "clutter-behaviour-scale-gravity.h"

//#include <clutter/clutter.h>

//#include <clutter/clutter-private.h>
#define CLUTTER_PARAM_READABLE  \
        G_PARAM_READABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB
#define CLUTTER_PARAM_WRITABLE  \
        G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB
#define CLUTTER_PARAM_READWRITE \
        G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |G_PARAM_STATIC_BLURB




#endif 
#include <math.h>

G_DEFINE_TYPE (ClutterBehaviourScaleGravity,
               clutter_behaviour_scale_gravity,
	       CLUTTER_TYPE_BEHAVIOUR);

struct _ClutterBehaviourScaleGravityPrivate
{
  ClutterGravity scale_gravity;

  gdouble x_scale_start;
  gdouble y_scale_start;

  gdouble x_scale_end;
  gdouble y_scale_end;
};

#define CLUTTER_BEHAVIOUR_SCALE_GRAVITY_GET_PRIVATE(obj)        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_TYPE_BEHAVIOUR_SCALE_GRAVITY, ClutterBehaviourScaleGravityPrivate))

enum
{
  PROP_0,
  PROP_SCALE_GRAVITY,
  PROP_X_SCALE_START,
  PROP_Y_SCALE_START,
  PROP_X_SCALE_END,
  PROP_Y_SCALE_END,
  LAST_ARG
};

typedef struct {
  ClutterGravity scale_gravity;
  gdouble scale_x;
  gdouble scale_y;
} ScaleFrameClosure;

static void
scale_frame_foreach (ClutterBehaviour *behaviour,
                     ClutterActor     *actor,
		     gpointer          data)
{
  ScaleFrameClosure *closure = data;


#if 0
  clutter_actor_set_scale (actor, closure->scale_x, closure->scale_y);
#else
  clutter_actor_set_scale_with_gravity (actor, closure->scale_x, closure->scale_y, closure->scale_gravity);
#endif 
} //scale_frame_foreach

static void
clutter_behaviour_scale_gravity_alpha_notify (ClutterBehaviour *behave,
                                      gdouble           alpha_value)
{
  ClutterBehaviourScaleGravityPrivate *priv;
#if 0
  ScaleFrameClosure closure = { 0, };
#else
  ScaleFrameClosure closure = { CLUTTER_GRAVITY_CENTER, 0, 0};
#endif 

  priv = CLUTTER_BEHAVIOUR_SCALE_GRAVITY (behave)->priv;

  /* Fix the start/end values, avoids potential rounding errors on large
   * values. 
  */

  closure.scale_gravity = priv->scale_gravity;
  if (alpha_value == 1.0)
    {
      closure.scale_x = priv->x_scale_end;
      closure.scale_y = priv->y_scale_end;
    }
  else if (alpha_value == 0)
    {
      closure.scale_x = priv->x_scale_start;
      closure.scale_y = priv->y_scale_start;
    }
  else
    {
      closure.scale_x = (priv->x_scale_end - priv->x_scale_start)
                      * alpha_value
                      + priv->x_scale_start;
      
      closure.scale_y = (priv->y_scale_end - priv->y_scale_start)
                      * alpha_value
                      + priv->y_scale_start;
    }

  clutter_behaviour_actors_foreach (behave,
                                    scale_frame_foreach,
                                    &closure);
}

static void
clutter_behaviour_scale_gravity_set_property (GObject      *gobject,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  ClutterBehaviourScaleGravityPrivate *priv;

  priv = CLUTTER_BEHAVIOUR_SCALE_GRAVITY (gobject)->priv;

  switch (prop_id)
    {
    case PROP_SCALE_GRAVITY:
      priv->scale_gravity = g_value_get_enum(value);
      break;

    case PROP_X_SCALE_START:
      priv->x_scale_start = g_value_get_double (value);
      break;

    case PROP_X_SCALE_END:
      priv->x_scale_end = g_value_get_double (value);
      break;

    case PROP_Y_SCALE_START:
      priv->y_scale_start = g_value_get_double (value);
      break;

    case PROP_Y_SCALE_END:
      priv->y_scale_end = g_value_get_double (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
clutter_behaviour_scale_gravity_get_property (GObject    *gobject,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  ClutterBehaviourScaleGravityPrivate *priv;

  priv = CLUTTER_BEHAVIOUR_SCALE_GRAVITY (gobject)->priv;

  switch (prop_id)
    {

    case PROP_SCALE_GRAVITY:
      g_value_set_enum (value, priv->scale_gravity);
      break;


    case PROP_X_SCALE_START:
      g_value_set_double (value, priv->x_scale_start);
      break;

    case PROP_X_SCALE_END:
      g_value_set_double (value, priv->x_scale_end);
      break;

    case PROP_Y_SCALE_START:
      g_value_set_double (value, priv->y_scale_start);
      break;

    case PROP_Y_SCALE_END:
      g_value_set_double (value, priv->y_scale_end);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
clutter_behaviour_scale_gravity_class_init (ClutterBehaviourScaleGravityClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterBehaviourClass *behave_class = CLUTTER_BEHAVIOUR_CLASS (klass);
  GParamSpec *pspec = NULL;

  g_type_class_add_private (klass, sizeof (ClutterBehaviourScaleGravityPrivate));

  gobject_class->set_property = clutter_behaviour_scale_gravity_set_property;
  gobject_class->get_property = clutter_behaviour_scale_gravity_get_property;


  /**
   * ClutterBehaviourScaleGravity:scale-gravity:
   *
   * The initial scaling gravity for the actors.
   *
   * Since: dvm
   */
   pspec = g_param_spec_enum ("scale-gravity",
                               "Scale Gravity",
                               "The scale gravity for applied actor",
								CLUTTER_TYPE_GRAVITY,
								CLUTTER_GRAVITY_CENTER,
                            	CLUTTER_PARAM_READWRITE);

  g_object_class_install_property (gobject_class,
                                   PROP_SCALE_GRAVITY,
                                   pspec);
 


  /**
   * ClutterBehaviourScaleGravity:x-scale-start:
   *
   * The initial scaling factor on the X axis for the actors.
   *
   * Since: 0.6
   */
  pspec = g_param_spec_double ("x-scale-start",
                               "X Start Scale",
                               "Initial scale on the X axis",
                               0.0, G_MAXDOUBLE,
                               1.0,
                               CLUTTER_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_X_SCALE_START,
                                   pspec);
  /**
   * ClutterBehaviourScaleGravity:x-scale-end:
   *
   * The final scaling factor on the X axis for the actors.
   *
   * Since: 0.6
   */
  pspec = g_param_spec_double ("x-scale-end",
                               "X End Scale",
                               "Final scale on the X axis",
                               0.0, G_MAXDOUBLE,
                               1.0,
                               CLUTTER_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_X_SCALE_END,
                                   pspec);
  /**
   * ClutterBehaviourScaleGravity:y-scale-start:
   *
   * The initial scaling factor on the Y axis for the actors.
   *
   * Since: 0.6
   */
  pspec = g_param_spec_double ("y-scale-start",
                               "Y Start Scale",
                               "Initial scale on the Y axis",
                               0.0, G_MAXDOUBLE,
                               1.0,
                               CLUTTER_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_Y_SCALE_START,
                                   pspec);
  /**
   * ClutterBehaviourScaleGravity:y-scale-end:
   *
   * The final scaling factor on the Y axis for the actors.
   *
   * Since: 0.6
   */
  pspec = g_param_spec_double ("y-scale-end",
                               "Y End Scale",
                               "Final scale on the Y axis",
                               0.0, G_MAXDOUBLE,
                               1.0,
                               CLUTTER_PARAM_READWRITE);
  g_object_class_install_property (gobject_class,
                                   PROP_Y_SCALE_END,
                                   pspec);

  behave_class->alpha_notify = clutter_behaviour_scale_gravity_alpha_notify;
}

static void
clutter_behaviour_scale_gravity_init (ClutterBehaviourScaleGravity *self)
{
  ClutterBehaviourScaleGravityPrivate *priv;

  self->priv = priv = CLUTTER_BEHAVIOUR_SCALE_GRAVITY_GET_PRIVATE (self);
  priv->scale_gravity = CLUTTER_GRAVITY_CENTER;
  priv->x_scale_start = priv->x_scale_end = 1.0;
  priv->y_scale_start = priv->y_scale_end = 1.0;
}

/**
 * clutter_behaviour_scale_gravity_new:
 * @alpha: a #ClutterAlpha
 * @x_scale_start: initial scale factor on the X axis
 * @y_scale_start: initial scale factor on the Y axis
 * @x_scale_end: final scale factor on the X axis
 * @y_scale_end: final scale factor on the Y axis
 *
 * Creates a new  #ClutterBehaviourScaleGravity instance.
 *
 * Return value: the newly created #ClutterBehaviourScaleGravity
 *
 * Since: 0.2
 */
ClutterBehaviour *
clutter_behaviour_scale_gravity_new (ClutterAlpha   *alpha,
				 ClutterGravity  scale_gravity,
			     gdouble         x_scale_start,
			     gdouble         y_scale_start,
			     gdouble         x_scale_end,
			     gdouble         y_scale_end)
{
  g_return_val_if_fail (alpha == NULL || CLUTTER_IS_ALPHA (alpha), NULL);
 
#if 1
  return g_object_new (CLUTTER_TYPE_BEHAVIOUR_SCALE_GRAVITY,
                       "alpha", alpha,
					 	"scale-gravity", scale_gravity,
                       "x-scale-start", x_scale_start,
                       "y-scale-start", y_scale_start,
                       "x-scale-end", x_scale_end,
                       "y-scale-end", y_scale_end,
                       NULL);
#else
	GObject* obj = g_object_new (CLUTTER_TYPE_BEHAVIOUR_SCALE_GRAVITY,
                       "alpha", alpha,
                       "x-scale-start", x_scale_start,
                       "y-scale-start", y_scale_start,
                       "x-scale-end", x_scale_end,
                       "y-scale-end", y_scale_end,
                       NULL);
	return CLUTTER_BEHAVIOUR(obj);

#endif 
} //clutter_behaviour_scale_gravity()

/**
 * clutter_behaviour_scale_gravity_set_bounds:
 * @scale: a #ClutterBehaviourScaleGravity
 * @x_scale_start: initial scale factor on the X axis
 * @y_scale_start: initial scale factor on the Y axis
 * @x_scale_end: final scale factor on the X axis
 * @y_scale_end: final scale factor on the Y axis
 *
 * Sets the bounds used by scale behaviour.
 *
 * Since: 0.6
 */
void
clutter_behaviour_scale_gravity_set_bounds (ClutterBehaviourScaleGravity *scale,
                                    gdouble                x_scale_start,
                                    gdouble                y_scale_start,
                                    gdouble                x_scale_end,
                                    gdouble                y_scale_end)
{
  ClutterBehaviourScaleGravityPrivate *priv;

  g_return_if_fail (CLUTTER_IS_BEHAVIOUR_SCALE_GRAVITY (scale));

  priv = scale->priv;

  g_object_freeze_notify (G_OBJECT (scale));

  if (priv->x_scale_start != x_scale_start)
    {
      priv->x_scale_start = x_scale_start;
      g_object_notify (G_OBJECT (scale), "x-scale-start");
    }

  if (priv->y_scale_start != y_scale_start)
    {
      priv->y_scale_start = y_scale_start;
      g_object_notify (G_OBJECT (scale), "y-scale-start");
    }

  if (priv->x_scale_end != x_scale_end)
    {
      priv->x_scale_end = x_scale_end;
      g_object_notify (G_OBJECT (scale), "x-scale-end");
    }

  if (priv->y_scale_end != y_scale_end)
    {
      priv->y_scale_end = y_scale_end;
      g_object_notify (G_OBJECT (scale), "y-scale-end");
    }

  g_object_thaw_notify (G_OBJECT (scale));
}

/**
 * clutter_behaviour_scale_gravity_get_bounds:
 * @scale: a #ClutterBehaviourScaleGravity
 * @x_scale_start: return location for the initial scale factor on the X
 *   axis, or %NULL
 * @y_scale_start: return location for the initial scale factor on the Y
 *   axis, or %NULL
 * @x_scale_end: return location for the final scale factor on the X axis,
 *   or %NULL
 * @y_scale_end: return location for the final scale factor on the Y axis,
 *   or %NULL
 *
 * Retrieves the bounds used by scale behaviour.
 *
 * Since: 0.4
 */
void
clutter_behaviour_scale_gravity_get_bounds (ClutterBehaviourScaleGravity *scale,
                                    gdouble               *x_scale_start,
                                    gdouble               *y_scale_start,
                                    gdouble               *x_scale_end,
                                    gdouble               *y_scale_end)
{
  ClutterBehaviourScaleGravityPrivate *priv;

  g_return_if_fail (CLUTTER_IS_BEHAVIOUR_SCALE_GRAVITY (scale));

  priv = scale->priv;

  if (x_scale_start)
    *x_scale_start = priv->x_scale_start;

  if (x_scale_end)
    *x_scale_end = priv->x_scale_end;

  if (y_scale_start)
    *y_scale_start = priv->y_scale_start;

  if (y_scale_end)
    *y_scale_end = priv->y_scale_end;
}
