#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <gtk/gtk.h>
#include <clutter-gtk/clutter-gtk.h>
#include "dock.h"
#include "clutter-behaviour-scale-gravity.h"
#include "clutter-behaviour-function.h"

#define WINDOW_FULL
//#define GTK_ACTION_SIMULATE
#ifdef GTK_ACTION_SIMULATE
#	undef WINDOW_FULL
#endif


#define SHADOW_FILEPATH ("shadow.png")
#define REEL_RIGHT_FILEPATH ("R3.png")
#define REEL_CENTER_FILEPATH ("C3.png")
#define REEL_LEFT_FILEPATH ("L3.png")
#define REEL_FONTNAME ("Mono 6")

/* function for Dock*/
static void marker_reached_cb(ClutterTimeline* timeline, gchar* marker_name, gint msecs, gpointer data);
static gboolean dock_icon_rollover_enter_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean dock_icon_rollover_leave_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static ClutterActor* create_dock_icon(const char* icon_filepath, const char* tooltip_name);

/* button-press callback function for Dock */
static gboolean dock_icon_browser_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean dock_icon_contacts_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean dock_icon_WC_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean dock_icon_SS_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);

static ClutterActor* create_dock_icon_reel(const char* text);



static void timeline_completed_cb(ClutterScore* score, ClutterTimeline* timeline, gpointer data);




typedef struct
{
	ClutterActor* dock_icon_reel;
	ClutterActor* reel_right;
	ClutterActor* reel_left;
	ClutterActor* reel_center;
	ClutterActor* reel_bar;
	ClutterActor* reel_label;
	ClutterScore* score;
	guint8* default_opacity;
	guint8* final_opacity;

}reel_private_data_t ;

static reel_private_data_t* reel_private_data_new0(void);
//static void reel_private_data_free (reel_private_data_t* reel_priv);
static void reel_private_data_free (gpointer reel_priv);



ClutterActor* create_dock(void)
{
	ClutterActor* dock; //ClutterGroup
	ClutterActor* title; //ClutterText
	const int ICON_NUM = 4;
	ClutterActor** dock_icons = g_new0(ClutterActor*, ICON_NUM);
	const char** fps = g_new0(const char*, ICON_NUM);
	const char** tooltip_names = g_new0(const char*, ICON_NUM);
	int idx;	

#if 0
	fps[0] = g_strdup("Browser.png");
	fps[1] = g_strdup("Contacts.png");
	fps[2] = g_strdup("WC.png");
	fps[3] = g_strdup("SS2.png");

#else
	fps[0] = g_strdup("Browser2.png");
	fps[1] = g_strdup("Contacts2.png");
	fps[2] = g_strdup("WC2.png");
	fps[3] = g_strdup("SS2.png");

	tooltip_names[0] = g_strdup("Browser");
	tooltip_names[1] = g_strdup("Contacts");
	tooltip_names[2] = g_strdup("Web Capture");
	tooltip_names[3] = g_strdup("Snapshot");


#endif 



	dock = clutter_group_new();
	title = clutter_text_new_with_text("Mono 10", "Dock");
	for(idx = 0; idx < ICON_NUM; idx++)
	{
		dock_icons[idx] = create_dock_icon(fps[idx], tooltip_names[idx]);
	}

	clutter_actor_set_position(title, 0,0);
	for(idx=0; idx < ICON_NUM; idx++)
	{
		clutter_actor_set_position(dock_icons[idx], (80+20)*idx,0);
	}

	g_signal_connect(G_OBJECT(dock_icons[0]), "button-press-event", G_CALLBACK(dock_icon_browser_cb), NULL);
	g_signal_connect(G_OBJECT(dock_icons[1]), "button-press-event", G_CALLBACK(dock_icon_contacts_cb), NULL);
	g_signal_connect(G_OBJECT(dock_icons[2]), "button-press-event", G_CALLBACK(dock_icon_WC_cb), NULL);
	g_signal_connect(G_OBJECT(dock_icons[3]), "button-press-event", G_CALLBACK(dock_icon_SS_cb), NULL);


	clutter_container_add(CLUTTER_CONTAINER(dock), 
		/*title, */
		dock_icons[0], dock_icons[1], dock_icons[2], dock_icons[3],
		NULL);

	clutter_actor_set_size(dock, 400,120);
	return dock;
} //create_dock();


static ClutterActor* create_dock_icon(const char* icon_filepath, const char* tooltip_name)
{
	g_return_val_if_fail(icon_filepath != NULL, NULL);
	g_return_val_if_fail(g_file_test(icon_filepath, G_FILE_TEST_EXISTS), NULL);
	ClutterActor* dock_icon_reel;
	ClutterActor* dock_icon; //ClutterGroup
	ClutterActor* title; //ClutterText
	ClutterActor* up_pic; //ClutterTexture
	gfloat* up_pic_pos_x;
	gfloat* up_pic_pos_y;
	ClutterActor* shadow_pic; //ClutterTexture
	
	ClutterTimeline* timeline;
	
	ClutterAlpha* alpha;
	ClutterPath* path;
	ClutterBehaviour* behaviour_up;
	ClutterBehaviour* behaviour_rotate;
	ClutterBehaviour* behaviour_scale;
	ClutterBehaviour* shadow_scale;
	ClutterBehaviour* shadow_opacity;

	

	dock_icon = clutter_group_new();
	title = clutter_text_new_with_text("Mono 8", "Dock\nIcon");
	GError* gerr= NULL;
	up_pic = clutter_texture_new_from_file(icon_filepath, &gerr);

	up_pic_pos_x = g_new0(gfloat,1);
	up_pic_pos_y = g_new0(gfloat,1);

	g_return_val_if_fail(gerr==NULL, NULL);
	shadow_pic = clutter_texture_new_from_file(SHADOW_FILEPATH, &gerr);
	g_return_val_if_fail(gerr==NULL, NULL);

#if 0
	dock_icon_reel = create_dock_icon_reel("Splashtop");
#else
	dock_icon_reel = create_dock_icon_reel(tooltip_name);
#endif 
	g_return_val_if_fail(dock_icon_reel!=NULL, NULL);

	clutter_actor_set_position(title, 5, 5);

	clutter_texture_set_filter_quality(CLUTTER_TEXTURE(up_pic), CLUTTER_TEXTURE_QUALITY_HIGH);

#if 0
	clutter_actor_set_position(up_pic,10, 25);
#else
	if(1)
	{
		gfloat width;
		gfloat height;
		clutter_actor_get_size(up_pic, &width, &height);
		clutter_actor_set_position(up_pic, 13, (70-height));
	}
#endif 





	clutter_actor_set_opacity(up_pic, 0xff);
clutter_actor_get_position(up_pic, up_pic_pos_x, up_pic_pos_y);


	clutter_actor_set_position(shadow_pic,-8, 58);
	clutter_actor_set_size(shadow_pic, 100, 15);

	clutter_actor_set_position(dock_icon_reel, 5, 80);

	clutter_container_add(CLUTTER_CONTAINER(dock_icon)/*, title*/, shadow_pic, up_pic, dock_icon_reel,NULL);
	clutter_actor_set_size(dock_icon, 100,100);
	clutter_actor_set_reactive(dock_icon, TRUE);


#if 0
	timeline = clutter_timeline_new(500);
//	clutter_timeline_set_delay(timeline, 30);

	alpha = clutter_alpha_new_full(timeline, CLUTTER_EASE_OUT_CIRC);
	path = clutter_path_new();
#if 0
	clutter_path_set_description(path, "L 0, -50");
#else

	clutter_path_set_description(path, "M 10, 25  L 10, 10");
#endif
	behaviour_up = clutter_behaviour_path_new(alpha, path);
	behaviour_scale = clutter_behaviour_scale_new(alpha,1.0, 1.0, 1.08, 1.08);
	behaviour_rotate = clutter_behaviour_rotate_new(alpha, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CW, 0.1, 3.0);
	clutter_behaviour_rotate_set_center(CLUTTER_BEHAVIOUR_ROTATE(behaviour_rotate), 32, 20, 0);

	shadow_scale = clutter_behaviour_scale_new(alpha, 1.0,1.0,1.18,1.18);
	shadow_opacity = clutter_behaviour_opacity_new(alpha, 0xff, 0xcf);
	

	clutter_behaviour_apply(shadow_scale, shadow_pic);
	clutter_actor_move_anchor_point_from_gravity (shadow_pic, CLUTTER_GRAVITY_CENTER);

	clutter_behaviour_apply(shadow_opacity, shadow_pic);

	clutter_behaviour_apply(behaviour_up, up_pic);
	clutter_behaviour_apply(behaviour_scale, up_pic);
//	clutter_actor_move_anchor_point_from_gravity (up_pic, CLUTTER_GRAVITY_CENTER);

	clutter_behaviour_apply(behaviour_rotate, up_pic);

	clutter_timeline_add_marker_at_time(timeline, "StartPointer", 0);

	g_object_set_data_full(G_OBJECT(timeline), "is-back", g_strdup("T"),g_free);

	g_object_set_data(G_OBJECT(timeline), "up_pic", up_pic);
	g_object_set_data(G_OBJECT(timeline), "shadow_pic", shadow_pic);
	g_object_set_data(G_OBJECT(timeline), "up_pic_pos_x", up_pic_pos_x);
	g_object_set_data(G_OBJECT(timeline), "up_pic_pos_y", up_pic_pos_y);


	g_signal_connect(G_OBJECT(timeline), "marker-reached", G_CALLBACK(marker_reached_cb), (gpointer)timeline);

#else



	ClutterScore* score = clutter_score_new();
	int* fm_state = g_new0(int,1);
	*fm_state=0;
	g_object_set_data_full(G_OBJECT(score), "is-back", g_strdup("T"),g_free);
	g_object_set_data(G_OBJECT(score), "up_pic", up_pic);
	g_object_set_data(G_OBJECT(score), "shadow_pic", shadow_pic);
	g_object_set_data(G_OBJECT(score), "up_pic_pos_x", up_pic_pos_x);
	g_object_set_data(G_OBJECT(score), "up_pic_pos_y", up_pic_pos_y);
	g_object_set_data(G_OBJECT(score), "fm_state", fm_state);
	g_object_set_data(G_OBJECT(score),"dock_icon_reel", dock_icon_reel);


  if(1)
  {
	const int step_time = 100;

	ClutterTimeline* tl1 = clutter_timeline_new(step_time);
	ClutterTimeline* tl2 = clutter_timeline_new(step_time);
	ClutterTimeline* tl3 = clutter_timeline_new(step_time);
	ClutterTimeline* tl4 = clutter_timeline_new(step_time);
	ClutterTimeline* tl5 = clutter_timeline_new(step_time);
	ClutterTimeline* tl6 = clutter_timeline_new(step_time/2);
	ClutterTimeline* tl7 = clutter_timeline_new(step_time/2);
	ClutterTimeline* tl8 = clutter_timeline_new(2*step_time);


	ClutterAlpha* al1 = clutter_alpha_new_full(tl1, CLUTTER_LINEAR);
	//ClutterAlpha* al2 = clutter_alpha_new_full(tl2, CLUTTER_EASE_OUT_CIRC);
	ClutterAlpha* al2 = clutter_alpha_new_full(tl2, CLUTTER_LINEAR);
	ClutterAlpha* al3 = clutter_alpha_new_full(tl3, CLUTTER_LINEAR);
	//ClutterAlpha* al4 = clutter_alpha_new_full(tl4, CLUTTER_EASE_IN_CIRC);
	ClutterAlpha* al4 = clutter_alpha_new_full(tl4, CLUTTER_LINEAR);
	ClutterAlpha* al5 = clutter_alpha_new_full(tl5, CLUTTER_LINEAR);
	ClutterAlpha* al6 = clutter_alpha_new_full(tl6, CLUTTER_LINEAR);
	ClutterAlpha* al7 = clutter_alpha_new_full(tl7, CLUTTER_LINEAR);

#if 0
	ClutterBehaviour* uS1 = clutter_behaviour_scale_new(al1, 1.00, 1.00, 1.10, 0.80);
	ClutterBehaviour* uS2 = clutter_behaviour_scale_new(al2, 1.10, 0.80, 0.80, 1.10);
	ClutterBehaviour* uS3 = clutter_behaviour_scale_new(al3, 0.80, 1.10, 1.10, 0.90);
	ClutterBehaviour* uS4 = clutter_behaviour_scale_new(al4, 1.10, 0.90, 1.00, 1.00);
#else
	ClutterBehaviour* uS1 = clutter_behaviour_scale_gravity_new(al1, CLUTTER_GRAVITY_SOUTH, 1.00, 1.00, 1.10, 0.80);
	ClutterBehaviour* uS2 = clutter_behaviour_scale_gravity_new(al2, CLUTTER_GRAVITY_CENTER, 1.10, 0.80, 0.80, 1.10);
	ClutterBehaviour* uS3 = clutter_behaviour_scale_gravity_new(al3, CLUTTER_GRAVITY_NORTH, 0.80, 1.10, 1.10, 0.90);
	ClutterBehaviour* uS4 = clutter_behaviour_scale_gravity_new(al4, CLUTTER_GRAVITY_NORTH, 1.10, 0.90, 1.00, 1.00);
	ClutterBehaviour* uS5 = clutter_behaviour_scale_gravity_new(al5, CLUTTER_GRAVITY_SOUTH, 1.00, 1.00, 1.05, 0.90);
	ClutterBehaviour* uS6 = clutter_behaviour_scale_gravity_new(al6, CLUTTER_GRAVITY_SOUTH, 1.05, 0.90, 1.10, 0.80);
	ClutterBehaviour* uS7 = clutter_behaviour_scale_gravity_new(al7, CLUTTER_GRAVITY_SOUTH, 1.10, 0.80, 1.00, 1.00);
#endif 

#if 0
	ClutterPath* pPath2 = clutter_path_new();
	ClutterPath* pPath4 = clutter_path_new();
	ClutterBehaviour* pS2 = clutter_behaviour_path_new(al2, pPath2);
	ClutterBehaviour* pS4 = clutter_behaviour_path_new(al4, pPath4);
#else


  #if 1
	ClutterPath* pPath2 = clutter_path_new();
	ClutterPath* pPath5 = clutter_path_new();
	
	clutter_path_clear(pPath2);
	gfloat width;
	gfloat height;
	gfloat pos_x;
	gfloat pos_y;
	clutter_actor_get_position(up_pic, &pos_x, &pos_y);
	clutter_actor_get_size(up_pic, &width, &height);




	int H=30;
	int X = pos_x;
	int Y = pos_y;

	clutter_path_add_move_to(pPath2, X, Y);
	clutter_path_add_rel_line_to(pPath2, 0, -1*H);
	clutter_path_add_move_to(pPath5, X, Y-H);
	clutter_path_add_rel_line_to(pPath5, 0, H);


	ClutterBehaviour* pS2 = clutter_behaviour_path_new(al2, pPath2);
	ClutterBehaviour* pS5 = clutter_behaviour_path_new(al5, pPath5);
  #else
	ClutterBehaviour* pS2 = clutter_behaviour_path_new_with_description(al2, "M 0 0 l 0 -100 l -100 0");
	ClutterBehaviour* pS4 = clutter_behaviour_path_new_with_description(al4, "M -100 -100 l 0 100 l 100 0");
  #endif 
#endif 	

	

//	clutter_actor_move_anchor_point_from_gravity(up_pic, CLUTTER_GRAVITY_SOUTH);
	clutter_behaviour_apply(uS1, up_pic);
	clutter_behaviour_apply(uS2, up_pic);
	clutter_behaviour_apply(uS3, up_pic);
	clutter_behaviour_apply(uS4, up_pic);
	clutter_behaviour_apply(uS5, up_pic);
	clutter_behaviour_apply(uS6, up_pic);
	clutter_behaviour_apply(uS7, up_pic);


#if 0
	clutter_behaviour_apply(pS2, up_pic);
	clutter_behaviour_apply(pS4, up_pic);
#else
//	clutter_behaviour_apply(pS1, up_pic);
	clutter_behaviour_apply(pS2, up_pic);
//	clutter_behaviour_apply(pS3, up_pic);
	clutter_behaviour_apply(pS5, up_pic);

#endif 


	clutter_score_append(score, NULL, tl1);
	clutter_score_append(score, tl1, tl2);
	clutter_score_append(score, tl2, tl3);
	clutter_score_append(score, tl3, tl4);
	clutter_score_append(score, tl4, tl5);
	clutter_score_append(score, tl5, tl6);
	clutter_score_append(score, tl6, tl7);
	clutter_score_append(score, tl7, tl8);
	clutter_score_set_loop(score, TRUE);
	
	g_signal_connect(G_OBJECT(score), "timeline-completed", G_CALLBACK(timeline_completed_cb), NULL);

  } //if(1)
#endif 


	

	g_signal_connect(G_OBJECT(dock_icon), "enter-event", G_CALLBACK(dock_icon_rollover_enter_cb), (gpointer)score);
	g_signal_connect(G_OBJECT(dock_icon), "leave-event", G_CALLBACK(dock_icon_rollover_leave_cb), (gpointer)score);

	return dock_icon;
} //create_dock_icon()






static gboolean dock_icon_rollover_enter_cb(ClutterActor* act, ClutterEvent* evt,gpointer data)
{
	g_return_val_if_fail(CLUTTER_IS_ACTOR(act), TRUE);
	ClutterScore* score =  (ClutterScore*)data;	
	g_return_val_if_fail(CLUTTER_IS_SCORE(score), TRUE);
	g_object_set_data_full(G_OBJECT(score), "is-back", g_strdup("T"),g_free);

	ClutterActor* dock_icon_reel = (ClutterActor*)g_object_get_data(G_OBJECT(score), "dock_icon_reel");
	g_return_val_if_fail(dock_icon_reel != NULL, TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(dock_icon_reel), TRUE);

	reel_private_data_t* reel_priv = (reel_private_data_t*)g_object_get_data(G_OBJECT(dock_icon_reel),"reel_priv");
	g_return_val_if_fail(reel_priv != NULL, TRUE);

	ClutterScore* score_reel = reel_priv->score;
	g_return_val_if_fail(score_reel != NULL, TRUE);
	g_return_val_if_fail(CLUTTER_IS_SCORE(score_reel), TRUE);

	clutter_score_stop(score);

	int* fm_state  = (int*)g_object_get_data(G_OBJECT(score),"fm_state");
	g_return_if_fail(fm_state != NULL);
	*fm_state = 0;

	ClutterActor* up_pic = (ClutterActor*)g_object_get_data(G_OBJECT(score), "up_pic");
	g_return_if_fail(up_pic != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(up_pic));
	
//	clutter_actor_move_anchor_point_from_gravity(up_pic, CLUTTER_GRAVITY_SOUTH);

	clutter_score_start(score);
	clutter_score_start(score_reel);

	return TRUE;
} //dock_icon_rollover_enter_cb()


static gboolean dock_icon_rollover_leave_cb(ClutterActor* act, ClutterEvent* evt,gpointer data)
{
	g_return_if_fail(CLUTTER_IS_ACTOR(act));
	ClutterScore* score =  (ClutterScore*)data;	
	g_return_if_fail(CLUTTER_IS_SCORE(score));
	g_object_set_data_full(G_OBJECT(score), "is-back", g_strdup("F"),g_free);

	ClutterActor* dock_icon_reel = (ClutterActor*)g_object_get_data(G_OBJECT(score), "dock_icon_reel");
	g_return_val_if_fail(dock_icon_reel != NULL, TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(dock_icon_reel), TRUE);

	reel_private_data_t* reel_priv = (reel_private_data_t*)g_object_get_data(G_OBJECT(dock_icon_reel),"reel_priv");
	g_return_val_if_fail(reel_priv != NULL, TRUE);

	ClutterScore* score_reel = reel_priv->score;
	g_return_val_if_fail(score_reel != NULL, TRUE);
	g_return_val_if_fail(CLUTTER_IS_SCORE(score_reel), TRUE);
	
	clutter_score_stop(score_reel);
	clutter_score_stop(score);

	ClutterActor* shadow_pic;
	ClutterActor* up_pic;
	gfloat* up_pic_pos_x;
	gfloat* up_pic_pos_y;

	shadow_pic = (ClutterActor*) g_object_get_data(G_OBJECT(score), "shadow_pic");
	up_pic = (ClutterActor*) g_object_get_data(G_OBJECT(score), "up_pic");
	up_pic_pos_x = (gfloat*) g_object_get_data(G_OBJECT(score), "up_pic_pos_x");
	up_pic_pos_y = (gfloat*) g_object_get_data(G_OBJECT(score), "up_pic_pos_y");
	
	g_return_val_if_fail(shadow_pic != NULL, TRUE);
	g_return_val_if_fail(up_pic != NULL, TRUE);
	g_return_val_if_fail(up_pic_pos_x != NULL, TRUE);
	g_return_val_if_fail(up_pic_pos_y != NULL, TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(up_pic), TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(shadow_pic), TRUE);

	clutter_actor_set_position(up_pic, *up_pic_pos_x, *up_pic_pos_y);
	clutter_actor_set_scale(up_pic, 1.0, 1.0);
	clutter_actor_set_scale(shadow_pic, 1.0, 1.0);	
	

	ClutterActor* reel_bar = clutter_container_find_child_by_name(CLUTTER_CONTAINER(dock_icon_reel),"bar" );
	ClutterActor* reel_label = clutter_container_find_child_by_name(CLUTTER_CONTAINER(dock_icon_reel),"label");
	g_return_val_if_fail(reel_bar != NULL, TRUE);
	g_return_val_if_fail(reel_label != NULL, TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(reel_bar), TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(reel_label), TRUE);
	ClutterActor* reel_left = clutter_container_find_child_by_name(CLUTTER_CONTAINER(reel_bar),"left" );
	ClutterActor* reel_center = clutter_container_find_child_by_name(CLUTTER_CONTAINER(reel_bar),"center" );
	ClutterActor* reel_right = clutter_container_find_child_by_name(CLUTTER_CONTAINER(reel_bar),"right" );
	g_return_val_if_fail(reel_left != NULL, TRUE);
	g_return_val_if_fail(reel_center != NULL, TRUE);
	g_return_val_if_fail(reel_right != NULL, TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(reel_left), TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(reel_center), TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(reel_right), TRUE);

	

  if(1)
  {
	clutter_actor_set_opacity(reel_bar , 0x00);
	clutter_actor_set_opacity(reel_label , 0x00);
	gfloat* pos_x;
	gfloat* pos_y;

 	pos_x = (gfloat*)g_object_get_data(G_OBJECT(reel_left), "orig_pos_x");
 	pos_y = (gfloat*)g_object_get_data(G_OBJECT(reel_left), "orig_pos_y");
	g_return_val_if_fail(pos_x!=NULL, TRUE );
	g_return_val_if_fail(pos_y!=NULL, TRUE );
	clutter_actor_set_position(reel_left, *pos_x, *pos_y);


 	pos_x = (gfloat*)g_object_get_data(G_OBJECT(reel_right), "orig_pos_x");
 	pos_y = (gfloat*)g_object_get_data(G_OBJECT(reel_right), "orig_pos_y");
	g_return_val_if_fail(pos_x!=NULL, TRUE );
	g_return_val_if_fail(pos_y!=NULL, TRUE );
	clutter_actor_set_position(reel_right, *pos_x, *pos_y);

	clutter_actor_set_scale(reel_center, 1.0, 1.0);	
  } //if(1)


	

	return TRUE;
} //dock_icon_rollover_leave_cb()

static void marker_reached_cb(ClutterTimeline* timeline, gchar* marker_name, gint msecs, gpointer data)
{
	return;
	gchar* is_back = (gchar*) g_object_get_data(G_OBJECT(timeline), "is-back");
	if(is_back[0] != 'F') return;
	//clutter_timeline_stop(timeline);
} //marker_reached_cb()

void test(ClutterTimeline* timeline)
{
	gchar* is_back = (gchar*) g_object_get_data(G_OBJECT(timeline), "is-back");
} //test(0



/* button-press callbck function for Dock */
static gboolean dock_icon_browser_cb(ClutterActor* act, ClutterEvent* evt, gpointer data)
{
	g_message("%s:%s()", G_STRLOC, G_STRFUNC);
	return TRUE;
} //dock_icon_browser_cb()

static gboolean dock_icon_contacts_cb(ClutterActor* act, ClutterEvent* evt, gpointer data)
{
	g_message("%s:%s()", G_STRLOC, G_STRFUNC);
	return TRUE;

} //dock_icon_contacts_cb()

static gboolean dock_icon_WC_cb(ClutterActor* act, ClutterEvent* evt, gpointer data)
{
	g_message("%s:%s()", G_STRLOC, G_STRFUNC);
	return TRUE;

} //dock_icon_WC_cb()

static gboolean dock_icon_SS_cb(ClutterActor* act, ClutterEvent* evt, gpointer data)
{
	g_message("%s:%s()", G_STRLOC, G_STRFUNC);
	return TRUE;

} //dock_icon_SS_cb()





static void timeline_completed_cb(ClutterScore* score, ClutterTimeline* timeline, gpointer data)
{	
	g_return_if_fail(score!=NULL);
	g_return_if_fail(timeline!=NULL);
	g_return_if_fail(CLUTTER_IS_SCORE(score));
	g_return_if_fail(CLUTTER_IS_TIMELINE(timeline));

	ClutterActor* up_pic = (ClutterActor*)g_object_get_data(G_OBJECT(score), "up_pic");
	g_return_if_fail(up_pic != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(up_pic));

	int* fm_state  = (int*)g_object_get_data(G_OBJECT(score),"fm_state");
	g_return_if_fail(fm_state != NULL);

	
	switch(*fm_state)
	{
#if 0
	case 0:
		clutter_actor_move_anchor_point_from_gravity(up_pic, CLUTTER_GRAVITY_SOUTH);
	case 1:
		clutter_actor_move_anchor_point_from_gravity(up_pic, CLUTTER_GRAVITY_NORTH);
	break;
	case 3:
		clutter_actor_move_anchor_point_from_gravity(up_pic, CLUTTER_GRAVITY_SOUTH);
	break;
#endif 
	default:
	break;
	};
	*fm_state = *fm_state + 1;



	
} //timeline_completed_cb()






static ClutterActor* create_dock_icon_reel(const char* text)
{
	g_return_val_if_fail(text!=NULL, NULL);
	
	reel_private_data_t* reel_priv;
	ClutterActor* dock_icon_reel;
	ClutterActor* reel_right;
	ClutterActor* reel_left;
	ClutterActor* reel_center;
	ClutterActor* reel_bar;
	ClutterActor* reel_label;
	
	guint8* default_opacity;
	guint8* final_opacity;

	default_opacity = g_new0(guint8, 1);
	final_opacity = g_new0(guint8, 1);
	*default_opacity = 0x00;
	*final_opacity = 0x4f;



	gfloat width;	
	gfloat height;
	gfloat bar_width;
	gfloat bar_height;
	gfloat pos_x;
	gfloat pos_y;
	gfloat bar_pos_x;
	gfloat bar_pos_y;
	
	GError* gerr = NULL;

	reel_priv = reel_private_data_new0();

	dock_icon_reel = clutter_group_new();
	reel_bar = clutter_group_new();
	

	reel_right = clutter_texture_new_from_file(REEL_RIGHT_FILEPATH, &gerr);
	g_return_val_if_fail(gerr==NULL, NULL);

	reel_left = clutter_texture_new_from_file(REEL_LEFT_FILEPATH, &gerr);
	g_return_val_if_fail(gerr==NULL, NULL);

	reel_center = clutter_texture_new_from_file(REEL_CENTER_FILEPATH, &gerr);
	g_return_val_if_fail(gerr==NULL, NULL);

	reel_label = clutter_text_new_with_text( REEL_FONTNAME, text);
	g_return_val_if_fail(reel_label != NULL, NULL);
	


	clutter_actor_set_position(reel_left , 0, 0);
	clutter_actor_set_opacity(reel_left, 0xff);

	clutter_actor_get_size(reel_left, &width, &height);
	clutter_actor_get_position(reel_left, &pos_x, &pos_y);

	clutter_actor_set_size(reel_center, 10, height);
	clutter_actor_set_position(reel_center , width, 0);
	clutter_actor_set_opacity(reel_center, 0xff);

	clutter_actor_get_size(reel_center, &width, &height);
	clutter_actor_get_position(reel_center, &pos_x, &pos_y);

	clutter_actor_set_position(reel_right , pos_x + width, 0);
	clutter_actor_set_opacity(reel_right, 0xff);

	clutter_actor_set_opacity(reel_bar, *default_opacity);
	

	ClutterColor* label_fg_color = clutter_color_new(0xff,0xff,0xff,0xff);
	clutter_text_set_color(CLUTTER_TEXT(reel_label), label_fg_color);
	clutter_actor_set_opacity(reel_label, 0x00);
	clutter_actor_get_position(reel_label, &pos_x, &pos_y);
	clutter_actor_get_size(reel_label, &width, &height);
	clutter_actor_get_size(reel_bar, &bar_width, &bar_height);
	clutter_actor_get_position(reel_bar, &bar_pos_x, &bar_pos_y);
	

	clutter_actor_set_position(
		reel_label, 
		37 - 0.5*width  ,
		-3
		);


	
	



	clutter_container_add_actor(CLUTTER_CONTAINER(reel_bar), reel_left);
	clutter_container_add_actor(CLUTTER_CONTAINER(reel_bar), reel_center);
	clutter_container_add_actor(CLUTTER_CONTAINER(reel_bar), reel_right);
	clutter_container_add_actor(CLUTTER_CONTAINER(dock_icon_reel), reel_bar);
	clutter_container_add_actor(CLUTTER_CONTAINER(dock_icon_reel), reel_label);

	clutter_actor_set_name(reel_center, "center");
	clutter_actor_set_name(reel_left, "left");
	clutter_actor_set_name(reel_right, "right");
	clutter_actor_set_name(reel_bar, "bar");
	clutter_actor_set_name(reel_label, "label");


	if(1)
	{
		gfloat width;
 		clutter_actor_get_size(dock_icon_reel,&width, NULL);
	 	clutter_actor_set_position(reel_bar, 25, -5);
	}

	reel_priv->dock_icon_reel = dock_icon_reel;
	reel_priv->reel_right = reel_right;
	reel_priv->reel_center = reel_center;
	reel_priv->reel_left = reel_left;
	reel_priv->reel_bar =reel_bar;
	reel_priv->reel_label = reel_label;
	reel_priv->default_opacity = default_opacity;
	reel_priv->final_opacity = final_opacity;
	
	ClutterScore* score = clutter_score_new();
	reel_priv->score = score;

  if(1)
  {

	int step_time = 180;
	ClutterTimeline* tl1 = clutter_timeline_new(step_time);
	ClutterTimeline* tl2 = clutter_timeline_new(step_time);
	ClutterTimeline* tl3 = clutter_timeline_new(step_time);
	ClutterAlpha* al1 = clutter_alpha_new_full(tl1, CLUTTER_EASE_IN_CIRC);
	ClutterAlpha* al2 = clutter_alpha_new_full(tl2, CLUTTER_LINEAR);
	ClutterAlpha* al3 = clutter_alpha_new_full(tl3, CLUTTER_LINEAR);

	gfloat width;
	gfloat height;
	gfloat pos_x;
	gfloat pos_y;
	gfloat reel_center_width;
	gfloat reel_center_height;
	gfloat scale_add_ratio = 10.0;
	gfloat scale_add_half_ratio = scale_add_ratio * 0.5;

	clutter_actor_get_size(reel_center, &reel_center_width, &reel_center_height);

	clutter_actor_get_position(reel_left, &pos_x, &pos_y);
	clutter_actor_get_size(reel_left, &width, &height);
	ClutterPath* lPath2 = clutter_path_new();
	clutter_path_add_move_to(lPath2, pos_x, pos_y);
	clutter_path_add_rel_line_to(lPath2, -1*scale_add_half_ratio*reel_center_width,0);

	clutter_actor_get_position(reel_right, &pos_x, &pos_y);
	clutter_actor_get_size(reel_right, &width, &height);
	ClutterPath* rPath2 = clutter_path_new();
	clutter_path_add_move_to(rPath2,  pos_x, pos_y);
	clutter_path_add_rel_line_to(rPath2, scale_add_half_ratio*reel_center_width,0);
	ClutterBehaviour* AO1 = clutter_behaviour_opacity_new(al1, *default_opacity, *final_opacity);

	ClutterBehaviour* rP2 = clutter_behaviour_path_new(al2, rPath2);
	ClutterBehaviour* lP2 = clutter_behaviour_path_new(al2, lPath2);
	ClutterBehaviour* cSG2 = clutter_behaviour_scale_gravity_new(al2, CLUTTER_GRAVITY_CENTER,1.00,1.00, 1.0+scale_add_ratio, 1.00);
	ClutterBehaviour* barF2 = clutter_behaviour_function_new(al2, CLUTTER_GRAVITY_CENTER, 1.00,1.00, 1.0+scale_add_ratio, 1.00);

	
	ClutterBehaviour* labelO3 = clutter_behaviour_opacity_new(al3, 0x00, 0xff);
	
	clutter_behaviour_apply(AO1, reel_bar);

	if(1)
	{
		gfloat* pos_x;
		gfloat* pos_y;

		pos_x = g_new0(gfloat,1);
		pos_y = g_new0(gfloat,1);
		clutter_actor_get_position(reel_left, pos_x, pos_y);
		g_object_set_data(G_OBJECT(reel_left), "orig_pos_x", pos_x);
		g_object_set_data(G_OBJECT(reel_left), "orig_pos_y", pos_y);

		pos_x = g_new0(gfloat,1);
		pos_y = g_new0(gfloat,1);
		clutter_actor_get_position(reel_right, pos_x, pos_y);
		g_object_set_data(G_OBJECT(reel_right), "orig_pos_x", pos_x);
		g_object_set_data(G_OBJECT(reel_right), "orig_pos_y", pos_y);
	} 


#if 1
	clutter_behaviour_apply(barF2, reel_bar);
#else
	clutter_behaviour_apply(cSG2, reel_center);
	clutter_behaviour_apply(lP2, reel_left);
	clutter_behaviour_apply(rP2, reel_right);
#endif

	clutter_behaviour_apply(labelO3, reel_label);
	clutter_score_append(score, NULL, tl1);
	clutter_score_append(score, tl1, tl2);
	clutter_score_append(score, tl2, tl3);

  } //if(1)

	g_object_set_data_full(G_OBJECT(dock_icon_reel), "reel_priv", reel_priv, reel_private_data_free);
	return dock_icon_reel;
	
} //create_dock_icon_reel()



static reel_private_data_t* reel_private_data_new0(void)
{
	reel_private_data_t *reel_priv = g_new0(reel_private_data_t, 1);
	return reel_priv;
} //reel_private_data_new0()

static void  reel_private_data_free(gpointer reel_priv)
{
	g_free(reel_priv);
} //reel_private_data_free()


