#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <gtk/gtk.h>
#include <clutter-gtk/clutter-gtk.h>
#include "todoblk.h"




#define BORDER_FILEPATH ("over.png")
#define WEBBLK_BG_FILEPATH ("mail_BG.png")
#define WEBBLK_BG_SHADOW_FILEPATH ("mail_shadow.png")




/* function for Web Block */
static gboolean todoblk_icon_rollover_enter_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean todoblk_icon_rollover_leave_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static ClutterActor* create_todoblk_icon(const char* icon_filepath);







ClutterActor* create_todoblk(void)
{
	ClutterActor* todoblk; //ClutterGroup
	ClutterActor* background; //ClutterTexture
	ClutterActor* background_shadow; //ClutterTexture
	ClutterActor* title; //ClutterText

	const int ICON_NUM = 6;
	ClutterActor** todoblk_icons = g_new0(ClutterActor*, ICON_NUM);
	const char** fps = g_new0(const char*, ICON_NUM);
	int idx;	

	fps[0] = g_strdup("wcap01.jpg");
	fps[1] = g_strdup("wcap02.jpg");
	fps[2] = g_strdup("wcap03.jpg");
	fps[3] = g_strdup("wcap04.jpg");
	fps[4] = g_strdup("wcap05.jpg");
	fps[5] = g_strdup("wcap06.jpg");

	todoblk = clutter_group_new();
	title = clutter_text_new_with_text("Mono 10", "Webblk");
	GError* gerr= NULL;
	background = clutter_texture_new_from_file(WEBBLK_BG_FILEPATH,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	background_shadow = clutter_texture_new_from_file(WEBBLK_BG_SHADOW_FILEPATH,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	for(idx = 0; idx < ICON_NUM; idx++)
	{
		todoblk_icons[idx] = create_todoblk_icon(fps[idx]);
	}


	clutter_actor_set_position(title, 0,0);
	clutter_actor_set_position(background, 0,0);
	clutter_actor_set_position(background_shadow, -15,333);
	for(idx=0; idx < ICON_NUM; idx++)
	{
		clutter_actor_set_position(todoblk_icons[idx], 25+(100+20)*(idx%2), 50+(80+20)*(idx/2));
	}

#if 0
	g_signal_connect(G_OBJECT(dock_icons[0]), "button-press-event", G_CALLBACK(dock_icon_browser_cb), NULL);
	g_signal_connect(G_OBJECT(dock_icons[1]), "button-press-event", G_CALLBACK(dock_icon_contacts_cb), NULL);
	g_signal_connect(G_OBJECT(dock_icons[2]), "button-press-event", G_CALLBACK(dock_icon_WC_cb), NULL);
	g_signal_connect(G_OBJECT(dock_icons[3]), "button-press-event", G_CALLBACK(dock_icon_SS_cb), NULL);
#else

#endif 

	clutter_container_add(CLUTTER_CONTAINER(todoblk), 
		/*title, */
		background_shadow,
		background,
		todoblk_icons[0], todoblk_icons[1], todoblk_icons[2], todoblk_icons[3], todoblk_icons[4], todoblk_icons[5],
		NULL);

	clutter_actor_set_size(todoblk, 300,400);
	return todoblk;
} //create_todoblk();







/* function for Web Block  */

static gboolean todoblk_icon_rollover_enter_cb(ClutterActor* act, ClutterEvent* evt,gpointer data)
{
	g_return_if_fail(CLUTTER_IS_ACTOR(act));
	ClutterTimeline* timeline =  (ClutterTimeline*)data;
	g_return_if_fail(CLUTTER_IS_TIMELINE(timeline));
	g_object_set_data_full(G_OBJECT(timeline), "is-back", g_strdup("T"),g_free);
	clutter_actor_raise_top(act);

	clutter_timeline_stop(timeline);

	//clutter_timeline_set_delay(timeline,  400);

	clutter_timeline_start(timeline);


	return TRUE;
} //todoblk_icon_rollover_enter_cb()


static gboolean todoblk_icon_rollover_leave_cb(ClutterActor* act, ClutterEvent* evt,gpointer data)
{
	g_return_if_fail(CLUTTER_IS_ACTOR(act));
	ClutterTimeline* timeline =  (ClutterTimeline*)data;
	g_return_if_fail(CLUTTER_IS_TIMELINE(timeline));
	g_object_set_data_full(G_OBJECT(timeline), "is-back", g_strdup("F"),g_free);
	clutter_timeline_stop(timeline);

	ClutterActor* up_pic = g_object_get_data(G_OBJECT(timeline), "up_pic");
	gfloat* up_pic_pos_x = g_object_get_data(G_OBJECT(timeline),"up_pic_pos_x" );
	gfloat* up_pic_pos_y = g_object_get_data(G_OBJECT(timeline),"up_pic_pos_y" );
	ClutterActor* border_pic = g_object_get_data(G_OBJECT(timeline), "border_pic");
	gfloat* border_pic_pos_x = g_object_get_data(G_OBJECT(timeline),"border_pic_pos_x" );
	gfloat* border_pic_pos_y = g_object_get_data(G_OBJECT(timeline),"border_pic_pos_y" );

	ClutterActor* shadow_pic = g_object_get_data(G_OBJECT(timeline), "shadow_pic");

	clutter_actor_set_scale(up_pic, 1.0, 1.0);
//	clutter_actor_set_position(up_pic, *up_pic_pos_x, *up_pic_pos_y);
	clutter_actor_set_scale(border_pic, 1.0, 1.0);
	clutter_actor_set_z_rotation_from_gravity(up_pic, 0, CLUTTER_GRAVITY_CENTER);
	clutter_actor_set_z_rotation_from_gravity(border_pic, 0, CLUTTER_GRAVITY_CENTER);
	clutter_actor_set_opacity(border_pic, 0x00);
	clutter_actor_set_opacity(up_pic, 0xff);

	return TRUE;
} //todoblk_icon_rollover_leave_cb()





static ClutterActor* create_todoblk_icon(const char* icon_filepath)
{
	g_return_val_if_fail(icon_filepath != NULL, NULL);
	g_return_val_if_fail(g_file_test(icon_filepath, G_FILE_TEST_EXISTS), NULL);
	
	ClutterActor* todoblk_icon; //ClutterGroup
	ClutterActor* title; //ClutterText
	ClutterActor* up_pic; //ClutterTexture
	ClutterActor* border_pic; //ClutterTexture
	gfloat* up_pic_pos_x;
	gfloat* up_pic_pos_y;
	gfloat* border_pic_pos_x;
	gfloat* border_pic_pos_y;

	ClutterTimeline* timeline;

	ClutterAlpha* alpha;
	ClutterPath* path;
	ClutterBehaviour* behaviour_up;
	ClutterBehaviour* behaviour_rotate;
	ClutterBehaviour* behaviour_scale;
	ClutterBehaviour* behaviour_opacity;
	ClutterBehaviour* border_opacity;
	ClutterBehaviour* border_rotate;
	ClutterBehaviour* border_scale;
	

	todoblk_icon = clutter_group_new();
	title = clutter_text_new_with_text("Mono 8", "Webblk\nIcon");
	GError* gerr= NULL;
	up_pic = clutter_texture_new_from_file(icon_filepath, &gerr);

	up_pic_pos_x = g_new0(gfloat,1);
	up_pic_pos_y = g_new0(gfloat,1);
	border_pic_pos_x = g_new0(gfloat, 1);
	border_pic_pos_y = g_new0(gfloat, 1);



	g_return_val_if_fail(gerr==NULL, NULL);
	border_pic = clutter_texture_new_from_file(BORDER_FILEPATH, &gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	
	clutter_actor_set_position(title, 5, 5);

	clutter_texture_set_filter_quality(CLUTTER_TEXTURE(up_pic), CLUTTER_TEXTURE_QUALITY_HIGH);

	clutter_actor_set_position(up_pic,5, 5);
	clutter_actor_set_size(up_pic, 96, 72);
	clutter_actor_get_position(up_pic, up_pic_pos_x, up_pic_pos_y);

	clutter_actor_set_position(border_pic,1, 1);
	clutter_actor_set_size(border_pic, 105, 79);
	clutter_actor_get_position(border_pic, border_pic_pos_x, border_pic_pos_y);

	clutter_actor_set_opacity(border_pic,0x00);

	clutter_actor_set_size(todoblk_icon, 105,79);
	clutter_container_add(CLUTTER_CONTAINER(todoblk_icon)/*, title*/, up_pic, border_pic, NULL);
	clutter_actor_set_reactive(todoblk_icon, TRUE);

	timeline = clutter_timeline_new(750);
//	clutter_timeline_set_delay(timeline, 30);

	//alpha = clutter_alpha_new_full(timeline, CLUTTER_EASE_OUT_CIRC);

	alpha = clutter_alpha_new_full(timeline, CLUTTER_EASE_OUT_BACK);
	path = clutter_path_new();
#if 0
	clutter_path_set_description(path, "L 0, -50");
#else
	clutter_path_set_description(path, "L 0, -25");
#endif

//	behaviour_up = clutter_behaviour_path_new(alpha, path);

#if 0
	behaviour_scale = clutter_behaviour_scale_new(alpha,1.0, 1.0, 1.08, 1.08);
	behaviour_rotate = clutter_behaviour_rotate_new(alpha, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CW, 0.1, 3.0);
	clutter_behaviour_rotate_set_center(behaviour_rotate, 48, 36, 0);

	border_scale = clutter_behaviour_scale_new(alpha, 1.0,1.0,1.08,1.08);
	border_opacity = clutter_behaviour_opacity_new(alpha, 0x00, 0xff);
	border_rotate = clutter_behaviour_rotate_new(alpha, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CW, 0.1, 3.0);
	clutter_behaviour_rotate_set_center(border_rotate, 52, 39, 0);
#else
	behaviour_scale = clutter_behaviour_scale_new(alpha,1.0, 1.0, 4.50, 4.50);
	behaviour_rotate = clutter_behaviour_rotate_new(alpha, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CW, 0.1, 3.0);
	behaviour_opacity = clutter_behaviour_opacity_new(alpha, 0x00, 0x7f);
	clutter_behaviour_rotate_set_center(CLUTTER_BEHAVIOUR_ROTATE(behaviour_rotate), 48, 36, 0);


	border_scale = clutter_behaviour_scale_new(alpha, 1.0,1.0,4.50,4.50);
	border_opacity = clutter_behaviour_opacity_new(alpha, 0x20, 0xdf);
	border_rotate = clutter_behaviour_rotate_new(alpha, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CW, 0.1, 3.0);
	clutter_behaviour_rotate_set_center(CLUTTER_BEHAVIOUR_ROTATE(border_rotate), 52, 39, 0);

#endif 


	clutter_behaviour_apply(border_scale, border_pic);
	clutter_actor_move_anchor_point_from_gravity (border_pic, CLUTTER_GRAVITY_CENTER);

	clutter_behaviour_apply(border_opacity, border_pic);

	clutter_behaviour_apply(border_rotate, border_pic);


//	clutter_behaviour_apply(behaviour_up, up_pic);
	
	clutter_behaviour_apply(behaviour_opacity, up_pic);
	clutter_behaviour_apply(behaviour_scale, up_pic);
	clutter_actor_move_anchor_point_from_gravity (up_pic, CLUTTER_GRAVITY_CENTER);

	clutter_behaviour_apply(behaviour_rotate, up_pic);


	clutter_timeline_add_marker_at_time(timeline, "StartPointer", 0);

	g_object_set_data_full(G_OBJECT(timeline), "is-back", g_strdup("T"),g_free);

	g_object_set_data(G_OBJECT(timeline), "up_pic", up_pic);
	g_object_set_data(G_OBJECT(timeline), "border_pic", border_pic);
	g_object_set_data(G_OBJECT(timeline), "up_pic_pos_x", up_pic_pos_x);
	g_object_set_data(G_OBJECT(timeline), "up_pic_pos_y", up_pic_pos_y);
	g_object_set_data(G_OBJECT(timeline), "border_pic_pos_x", border_pic_pos_x);
	g_object_set_data(G_OBJECT(timeline), "border_pic_pos_y", border_pic_pos_y);


	//g_signal_connect(G_OBJECT(timeline), "marker-reached", G_CALLBACK(marker_reached_cb), (gpointer)timeline);
	g_signal_connect(G_OBJECT(todoblk_icon), "enter-event", G_CALLBACK(todoblk_icon_rollover_enter_cb), (gpointer)timeline);
	g_signal_connect(G_OBJECT(todoblk_icon), "leave-event", G_CALLBACK(todoblk_icon_rollover_leave_cb), (gpointer)timeline);

	return todoblk_icon;
} //create_todoblk_icon()



