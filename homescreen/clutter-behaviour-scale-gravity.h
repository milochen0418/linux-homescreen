/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *             Jorn Baayen  <jorn@openedhand.com>
 *             Emmanuele Bassi  <ebassi@openedhand.com>
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
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined(__CLUTTER_H_INSIDE__) && !defined(CLUTTER_COMPILATION)
//#error "Only <clutter/clutter.h> can be included directly."
#endif

#ifndef __CLUTTER_BEHAVIOUR_SCALE_GRAVITY_H__
#define __CLUTTER_BEHAVIOUR_SCALE_GRAVITY_H__
#if 0
#include <clutter/clutter-behaviour.h>
#else
#include <clutter/clutter.h>
#endif 

G_BEGIN_DECLS

#define CLUTTER_TYPE_BEHAVIOUR_SCALE_GRAVITY            (clutter_behaviour_scale_gravity_get_type ())
#define CLUTTER_BEHAVIOUR_SCALE_GRAVITY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_BEHAVIOUR_SCALE_GRAVITY, ClutterBehaviourScaleGravity))
#define CLUTTER_BEHAVIOUR_SCALE_GRAVITY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_TYPE_BEHAVIOUR_SCALE_GRAVITY, ClutterBehaviourScaleGravityClass))
#define CLUTTER_IS_BEHAVIOUR_SCALE_GRAVITY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_BEHAVIOUR_SCALE_GRAVITY))
#define CLUTTER_IS_BEHAVIOUR_SCALE_GRAVITY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_TYPE_BEHAVIOUR_SCALE_GRAVITY))
#define CLUTTER_BEHAVIOUR_SCALE_GRAVITY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_TYPE_BEHAVIOUR_SCALE_GRAVITY, ClutterBehaviourScaleGravityClass))

typedef struct _ClutterBehaviourScaleGravity           ClutterBehaviourScaleGravity;
typedef struct _ClutterBehaviourScaleGravityPrivate    ClutterBehaviourScaleGravityPrivate;
typedef struct _ClutterBehaviourScaleGravityClass      ClutterBehaviourScaleGravityClass;

/**
 * ClutterBehaviourScaleGravity:
 *
 * The #ClutterBehaviourScaleGravity struct contains only private data and
 * should be accessed using the provided API
 *
 * Since: 0.2
 */
struct _ClutterBehaviourScaleGravity
{
  /*< private >*/
  ClutterBehaviour parent_instance;

  ClutterBehaviourScaleGravityPrivate *priv;
};

/**
 * ClutterBehaviourScaleGravityClass:
 *
 * The #ClutterBehaviourScaleGravityClass struct contains only private data
 *
 * Since: 0.2
 */
struct _ClutterBehaviourScaleGravityClass
{
  /*< private >*/
  ClutterBehaviourClass parent_class;
};

GType clutter_behaviour_scale_gravity_get_type (void) G_GNUC_CONST;

ClutterBehaviour *clutter_behaviour_scale_gravity_new        (ClutterAlpha          *alpha,
													  ClutterGravity  scale_gravity,
                                                      gdouble                x_scale_start,
                                                      gdouble                y_scale_start,
                                                      gdouble                x_scale_end,
                                                      gdouble                y_scale_end);
void              clutter_behaviour_scale_gravity_set_bounds (ClutterBehaviourScaleGravity *scale,
                                                      gdouble                x_scale_start,
                                                      gdouble                y_scale_start,
                                                      gdouble                x_scale_end,
                                                      gdouble                y_scale_end);
void              clutter_behaviour_scale_gravity_get_bounds (ClutterBehaviourScaleGravity *scale,
                                                      gdouble               *x_scale_start,
                                                      gdouble               *y_scale_start,
                                                      gdouble               *x_scale_end,
                                                      gdouble               *y_scale_end);

G_END_DECLS

#endif /* __CLUTTER_BEHAVIOUR_SCALE_GRAVITY_H__ */
