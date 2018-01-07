#ifndef __CLUTTER_ACTOR_DATA_SNAPSHOT_H_
#define __CLUTTER_ACTOR_DATA_SNAPSHOT_H_


/* 
 * Author: milo.chen@devicevm.com.tw  23 July 2009
 * Purpose: To plus maintainablity.
 * 	To solve the issue for the follow 2 item will plus maintainability for the wirtten effect.
 *
 * Item 1 : Animation recovery
 * 	Since timeline or animation could not support the way to recover orginal data.
 * 	So we need to maintain the orginal data in two different function (one is callback and another is default setting)
 *
 * Item 2 :Animation initialization
 *  Whenever you have set initial data for actor, you will also set somedata again for behaviour
 *  when your behaviour is designed only for the spefic actor.
 *  
 * */

#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <gtk/gtk.h>
#include <clutter-gtk/clutter-gtk.h>




typedef enum
{
	//SIZE
	DVM_CAS_WIDTH = 1<<0,
	DVM_CAS_HEIGHT = 1<<1,

	//POSITION
	DVM_CAS_POS_X = 1<<2,
	DVM_CAS_POS_Y = 1<<3,
	DVM_CAS_DEPTH = 1<<4,


	//SCALE
	DVM_CAS_SCALE_X = 1<<5,
	DVM_CAS_SCALE_Y = 1<<6,

	//ANCHOR_POINT
	DVM_CAS_ANCHOR_X = 1<<7,
	DVM_CAS_ANCHOR_Y = 1<<8,


	//ROTATION
	DVM_CAS_ROTATION_X = 1<<9,
	DVM_CAS_ROTATION_Y = 1<<10,
	DVM_CAS_ROTATION_Z = 1<<11,

	//OPACITY
	DVM_CAS_OPACITY = 1<<12,


	DVM_CAS_SIZE = DVM_CAS_WIDTH | DVM_CAS_HEIGHT,
	DVM_CAS_POSITION = DVM_CAS_POS_X | DVM_CAS_POS_Y | DVM_CAS_DEPTH,
	DVM_CAS_SCALE = DVM_CAS_SCALE_X | DVM_CAS_SCALE_Y,
	DVM_CAS_ANCHOR_POINT = DVM_CAS_SCALE_X | DVM_CAS_SCALE_Y,
	DVM_CAS_ROTATION = DVM_CAS_ROTATION_X | DVM_CAS_ROTATION_Y | DVM_CAS_ROTATION_Z

}DvmClutterActorDataSnapshotRestoredFlags;




typedef struct
{
	DvmClutterActorDataSnapshotRestoredFlags flags;
	gfloat width;
	gfloat height;
	gfloat position_x;
	gfloat position_y;
	gfloat depth;
	gdouble scale_x;
	gdouble scale_y;

	gfloat anchor_x;
	gfloat anchor_y;
/* TODO rotation */
	gfloat rotation_x;
	gfloat rotation_y;
	gfloat rotation_z;

	guint8 opacity;

}DvmClutterActorDataSnapshot;




DvmClutterActorDataSnapshot* dvm_clutter_actor_snapshot_data_new(
	ClutterActor* actor, 
	DvmClutterActorDataSnapshotRestoredFlags flags
);

void dvm_clutter_actor_data_snapshot_free( gpointer data_snapshot);
void dvm_clutter_actor_data_snapshot_restore(DvmClutterActorDataSnapshot* data, ClutterActor* actor);

#endif //__CLUTTER_ACTOR_DATA_SNAPSHOT_H_
