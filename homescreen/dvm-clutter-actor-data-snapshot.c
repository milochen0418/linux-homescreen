

#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <gtk/gtk.h>
#include <clutter-gtk/clutter-gtk.h>
#include "dvm-clutter-actor-data-snapshot.h"





DvmClutterActorDataSnapshot* dvm_clutter_actor_data_snapshot_new(
	ClutterActor* actor, 	
	DvmClutterActorDataSnapshotRestoredFlags flags
)
{
	g_return_val_if_fail( !(flags & DVM_CAS_ROTATION), NULL);
	/* DVM_CAS_ROTATION is TODO */

	g_return_val_if_fail(actor != NULL, NULL);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(actor), NULL);


	DvmClutterActorDataSnapshot* data = g_new0(DvmClutterActorDataSnapshot,1);

	data->flags = flags;
	clutter_actor_get_size(actor, &(data->width), &(data->height));
	clutter_actor_get_position(actor, &(data->position_x), &(data->position_y));
	data->depth = clutter_actor_get_depth(actor);
	clutter_actor_get_scale(actor, &(data->scale_x), &(data->scale_y));
	clutter_actor_get_anchor_point(actor, &(data->anchor_x), &(data->anchor_y));
	
	/* TODO for rotation */
	data->opacity = clutter_actor_get_opacity(actor);
	
	return data;
}  //dvm_clutter_actor_data_snapshot_new()



void dvm_clutter_actor_data_snapshot_free( gpointer data_snapshot)
{
	g_free(data_snapshot);
} //dvm_clutter_actor_data_snapshot_free()



void dvm_clutter_actor_data_snapshot_restore(DvmClutterActorDataSnapshot* data, ClutterActor* actor)
{
	g_return_if_fail(data != NULL);
	g_return_if_fail(actor != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(actor));

	DvmClutterActorDataSnapshotRestoredFlags flags;
	flags = data->flags;

	if( flags & DVM_CAS_SIZE == DVM_CAS_SIZE)
	{
		clutter_actor_set_size(actor, data->width, data->height);
	}
	else if(flags & DVM_CAS_POSITION == DVM_CAS_POSITION)
	{
		clutter_actor_set_position(actor, data->position_x, data->position_y);
		clutter_actor_set_depth(actor, data->depth);
	}
	else if(flags & DVM_CAS_SCALE == DVM_CAS_SCALE)
	{
		clutter_actor_set_scale(actor, data->scale_x, data->scale_y);
	}
	else if(flags & DVM_CAS_ANCHOR_POINT == DVM_CAS_ANCHOR_POINT)
	{
		clutter_actor_set_anchor_point(actor, data->anchor_x, data->anchor_y);
	}
	else if(flags & DVM_CAS_ROTATION == DVM_CAS_ROTATION)
	{
		g_return_if_fail(!(flags & DVM_CAS_ROTATION));
	}
	else if(flags & DVM_CAS_OPACITY == DVM_CAS_OPACITY)
	{
		clutter_actor_set_opacity(actor, data->opacity);
	}
	else
	{
	
	}

} //dvm_clutter_actor_data_snapshot_restore()




