#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <gtk/gtk.h>
#include <clutter-gtk/clutter-gtk.h>
#include "alphafuncs.h"
#include "webblk.h"




#define BORDER_FILEPATH ("over.png")
#define QUESTION_SIGN_FILEPATH ("Q.png")
#define FRAME_FILEPATH ("frame.png")
#define WEBBLK_BG_FILEPATH ("wc_BG.png")
#define WEBBLK_BG_SHADOW_FILEPATH ("wc_shadow.png")



#define ICON_WIDTH 108
#define ICON_HEIGHT 82
#define BORDER_WIDTH 116
#define BORDER_HEIGHT 90
#define ICON_X_DIST 20
#define ICON_Y_DIST 10
#define ICONS_OFFSET_X 17
#define ICONS_OFFSET_Y 65




/* function for Web Block */
static gboolean webblk_icon_rollover_enter_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean webblk_icon_rollover_leave_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);

static gboolean webblk_icon_pressed_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean webblk_icon_release_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);

static ClutterActor* create_webblk_icon(const char* icon_filepath);
static void icon_effect_apply(ClutterActor* group);

static gdouble shake_alpha_func(ClutterAlpha *alpha , gpointer data);





ClutterActor* create_webblk(void)
{
	ClutterActor* webblk; //ClutterGroup
	ClutterActor* background; //ClutterTexture
	ClutterActor* background_shadow; //ClutterTexture
	
	ClutterActor* title; //ClutterText
	

	const int ICON_NUM = 6;
	ClutterActor** webblk_icons = g_new0(ClutterActor*, ICON_NUM);
	const char** fps = g_new0(const char*, ICON_NUM);
	int idx;	

	fps[0] = g_strdup("wcap01.jpg");
	fps[1] = g_strdup("wcap02.jpg");
	fps[2] = g_strdup("wcap03.jpg");
#if 0
	fps[3] = g_strdup("wcap04.jpg");
	fps[4] = g_strdup("wcap05.jpg");
	fps[5] = g_strdup("wcap06.jpg");
#else
	fps[3] = NULL;
	fps[4] = NULL;
	fps[5] = NULL;

#endif 

	webblk = clutter_group_new();
	title = clutter_text_new_with_text("Mono 8 Bold", "Web Capture");
	GError* gerr= NULL;
	background = clutter_texture_new_from_file(WEBBLK_BG_FILEPATH,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	background_shadow = clutter_texture_new_from_file(WEBBLK_BG_SHADOW_FILEPATH,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	for(idx = 0; idx < ICON_NUM; idx++)
	{
		webblk_icons[idx] = create_webblk_icon(fps[idx]);
	}

	


	clutter_actor_set_position(title, 0,0);
	clutter_actor_set_position(background, 0,0);
	clutter_actor_set_position(background_shadow, -15,333);
	for(idx=0; idx < ICON_NUM; idx++)
	{
		clutter_actor_set_position(webblk_icons[idx], 
			ICONS_OFFSET_X + ( ICON_WIDTH + ICON_X_DIST)*(idx%2) , 
			ICONS_OFFSET_Y + ( ICON_HEIGHT  + ICON_Y_DIST)*(idx/2));
	}


	ClutterColor *text_color = clutter_color_new(0xff, 0xff, 0xff, 0xff);
	clutter_text_set_color(CLUTTER_TEXT(title), text_color);
	clutter_color_free(text_color);
	clutter_actor_set_position(title, 70,-1);

	clutter_container_add(CLUTTER_CONTAINER(webblk), 
		background_shadow,
		background,
		title,

		webblk_icons[0], webblk_icons[1], webblk_icons[2], webblk_icons[3], webblk_icons[4], webblk_icons[5],
		NULL);


	gfloat height;
	gfloat width;
	clutter_actor_get_size(background, &width, &height);
	clutter_actor_set_size(webblk, width,height);

	return webblk;
} //create_webblk();







/* function for Web Block  */

static gboolean webblk_icon_rollover_enter_cb(ClutterActor* act, ClutterEvent* evt,gpointer data)
{
	g_return_if_fail(CLUTTER_IS_ACTOR(act));
	ClutterTimeline* timeline =  (ClutterTimeline*)data;
	g_return_if_fail(CLUTTER_IS_TIMELINE(timeline));
	g_object_set_data_full(G_OBJECT(timeline), "is-back", g_strdup("T"),g_free);

	clutter_actor_raise_top(act);

	ClutterScore* score = (ClutterScore*) g_object_get_data(G_OBJECT(timeline), "score");
	ClutterActor* border_pic = g_object_get_data(G_OBJECT(timeline), "border_pic");
	ClutterActor* up_pic = g_object_get_data(G_OBJECT(timeline),"up_pic");
	char* picture_empty = (char*) g_object_get_data(G_OBJECT(up_pic), "picture-empty");
	
	if(picture_empty != NULL && picture_empty[0]=='F')
	{
		clutter_actor_set_opacity(border_pic, 0xff);
	}

	g_return_val_if_fail(CLUTTER_IS_SCORE(score),TRUE);
	clutter_score_stop(score);
	clutter_score_start(score);

	return TRUE;
} //webblk_icon_rollover_enter_cb()


static gboolean webblk_icon_rollover_leave_cb(ClutterActor* act, ClutterEvent* evt,gpointer data)
{
	gboolean ret = FALSE;
	g_return_if_fail(CLUTTER_IS_ACTOR(act));
	ClutterTimeline* timeline =  (ClutterTimeline*)data;
	g_return_if_fail(CLUTTER_IS_TIMELINE(timeline));
	g_object_set_data_full(G_OBJECT(timeline), "is-back", g_strdup("F"),g_free);


	ClutterScore* score = (ClutterScore*) g_object_get_data(G_OBJECT(timeline), "score");
	clutter_score_stop(score);
	
	ClutterActor* up_pic = g_object_get_data(G_OBJECT(timeline), "up_pic");
	g_return_val_if_fail(up_pic != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(up_pic), ret);
	gfloat* up_pic_pos_x = g_object_get_data(G_OBJECT(timeline),"up_pic_pos_x" );
	g_return_val_if_fail(up_pic != NULL, ret);
	gfloat* up_pic_pos_y = g_object_get_data(G_OBJECT(timeline),"up_pic_pos_y" );
	g_return_val_if_fail(up_pic != NULL, ret);

	ClutterActor* border_pic = g_object_get_data(G_OBJECT(timeline), "border_pic");
	g_return_val_if_fail(border_pic != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(border_pic), ret);
	gfloat* border_pic_pos_x = g_object_get_data(G_OBJECT(timeline),"border_pic_pos_x" );
	g_return_val_if_fail(border_pic_pos_x != NULL, ret);
	gfloat* border_pic_pos_y = g_object_get_data(G_OBJECT(timeline),"border_pic_pos_y" );
	g_return_val_if_fail(border_pic_pos_y != NULL, ret);


	ClutterActor* frame_pic = g_object_get_data(G_OBJECT(timeline), "frame_pic");
	g_return_val_if_fail(frame_pic != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(frame_pic), ret);
	gfloat* frame_pic_pos_x = g_object_get_data(G_OBJECT(timeline),"frame_pic_pos_x" );
	g_return_val_if_fail(frame_pic_pos_x != NULL, ret);
	gfloat* frame_pic_pos_y = g_object_get_data(G_OBJECT(timeline),"frame_pic_pos_y" );
	g_return_val_if_fail(frame_pic_pos_y != NULL, ret);

//	ClutterActor* shadow_pic = g_object_get_data(G_OBJECT(timeline), "shadow_pic");
//	g_return_val_if_fail(shadow_pic != NULL, ret);

	clutter_actor_set_scale(up_pic, 1.0, 1.0);
	clutter_actor_set_scale(frame_pic, 1.0, 1.0);
	clutter_actor_set_scale(border_pic, 1.0, 1.0);

	clutter_actor_set_z_rotation_from_gravity(up_pic, 0, CLUTTER_GRAVITY_CENTER);
	clutter_actor_set_z_rotation_from_gravity(border_pic, 0, CLUTTER_GRAVITY_CENTER);
	clutter_actor_set_z_rotation_from_gravity(frame_pic, 0, CLUTTER_GRAVITY_CENTER);
	clutter_actor_set_opacity(frame_pic, 0xff);
	clutter_actor_set_opacity(border_pic, 0x00);
	

	return TRUE;
} //webblk_icon_rollover_leave_cb()





static ClutterActor* create_webblk_icon(const char* icon_filepath)
{
	//g_return_val_if_fail(icon_filepath != NULL, NULL);
	//g_return_val_if_fail(g_file_test(icon_filepath, G_FILE_TEST_EXISTS), NULL);
	
	ClutterActor* webblk_icon; //ClutterGroup
	ClutterActor* title; //ClutterText
	ClutterActor* up_pic; //ClutterTexture
	ClutterActor* border_pic; //ClutterTexture
	ClutterActor* frame_pic; //ClutterTexture

	gfloat* up_pic_pos_x;
	gfloat* up_pic_pos_y;
	gfloat* border_pic_pos_x;
	gfloat* border_pic_pos_y;
	gfloat* frame_pic_pos_x;
	gfloat* frame_pic_pos_y;

	ClutterTimeline* timeline;

	ClutterAlpha* shake_alpha;
	ClutterAlpha* alpha;
	ClutterPath* path;
	ClutterBehaviour* behaviour_up;
	ClutterBehaviour* behaviour_rotate;
	ClutterBehaviour* behaviour_scale;
	ClutterBehaviour* behaviour_opacity;
	ClutterBehaviour* border_opacity;
	ClutterBehaviour* border_rotate;
	ClutterBehaviour* border_scale;

	up_pic_pos_x = g_new0(gfloat,1);
	up_pic_pos_y = g_new0(gfloat,1);
	border_pic_pos_x = g_new0(gfloat, 1);
	border_pic_pos_y = g_new0(gfloat, 1);
	frame_pic_pos_x = g_new0(gfloat, 1);
	frame_pic_pos_y = g_new0(gfloat, 1);

	webblk_icon = clutter_group_new();
	title = clutter_text_new_with_text("Mono 8", "Webblk\nIcon");
	GError* gerr= NULL;

	up_pic = clutter_texture_new_from_file(icon_filepath, &gerr);
	if(gerr!=NULL)
	{
		up_pic = clutter_text_new_with_text("Mono 8 Bold", " ");
		g_object_set_data_full(G_OBJECT(up_pic),"picture-empty",g_strdup("T"), g_free );
		g_clear_error(&gerr);
		gerr = NULL;
		frame_pic = clutter_texture_new_from_file(QUESTION_SIGN_FILEPATH, &gerr);
		g_return_val_if_fail(gerr==NULL, NULL);

	} //if(gerr!=NULL)
	else
	{
		g_object_set_data_full(G_OBJECT(up_pic),"picture-empty",g_strdup("F"), g_free );
		frame_pic = clutter_texture_new_from_file(FRAME_FILEPATH, &gerr);
		g_return_val_if_fail(gerr==NULL, NULL);
	}
	

	border_pic = clutter_texture_new_from_file(BORDER_FILEPATH, &gerr);
	g_return_val_if_fail(gerr==NULL, NULL);




	clutter_actor_set_position(title, 0, 0);

	clutter_texture_set_filter_quality(CLUTTER_TEXTURE(up_pic), CLUTTER_TEXTURE_QUALITY_HIGH);

		

	clutter_actor_set_position(up_pic,(BORDER_WIDTH-ICON_WIDTH)/2,(BORDER_HEIGHT-ICON_HEIGHT)/2);
	clutter_actor_set_size(up_pic, ICON_WIDTH, ICON_HEIGHT);
	clutter_actor_get_position(up_pic, up_pic_pos_x, up_pic_pos_y);

	clutter_actor_set_position(border_pic,0, 0);
	clutter_actor_set_size(border_pic, BORDER_WIDTH, BORDER_HEIGHT);
	clutter_actor_get_position(border_pic, border_pic_pos_x, border_pic_pos_y);
	clutter_actor_set_opacity(border_pic,0x00);


	clutter_actor_set_position(frame_pic,(BORDER_WIDTH-ICON_WIDTH)/2,(BORDER_HEIGHT-ICON_HEIGHT)/2);
	clutter_actor_get_position(frame_pic, frame_pic_pos_x, frame_pic_pos_y);
	clutter_actor_set_opacity(frame_pic,0xff);



	clutter_actor_set_size(webblk_icon, BORDER_WIDTH,BORDER_HEIGHT);

	clutter_container_add(CLUTTER_CONTAINER(webblk_icon)/*, title*/, up_pic, frame_pic, border_pic,  NULL);
	clutter_actor_set_reactive(webblk_icon, TRUE);

	timeline = clutter_timeline_new(400);


	alpha = clutter_alpha_new_full(timeline, CLUTTER_EASE_OUT_EXPO);
	shake_alpha = clutter_alpha_new_with_func( timeline, shake_alpha_func,NULL, NULL);

	path = clutter_path_new();
	clutter_path_set_description(path, "L 0, -25");

	ClutterScore* score = clutter_score_new();
	ClutterScore* pressed_score = clutter_score_new();


	char* picture_empty = (char*)g_object_get_data(G_OBJECT(up_pic),"picture-empty");
	g_return_val_if_fail(picture_empty != NULL, NULL);

	if(picture_empty[0] == 'F')
	{
	  if(1) 
	  {
		const int step_time = 100;
		ClutterTimeline* tl1 = clutter_timeline_new(step_time);
		ClutterTimeline* tl2 = clutter_timeline_new(step_time);
		ClutterTimeline* tl3 = clutter_timeline_new(step_time-10);
		ClutterTimeline* tl4 = clutter_timeline_new(step_time-10);
	
		ClutterAlpha* alpha1 = clutter_alpha_new_full(tl1, CLUTTER_EASE_OUT_CUBIC);
		ClutterAlpha* alpha2 = clutter_alpha_new_full(tl2, CLUTTER_LINEAR);
		ClutterAlpha* alpha3 = clutter_alpha_new_full(tl3, CLUTTER_LINEAR);
		ClutterAlpha* alpha4 = clutter_alpha_new_full(tl4, CLUTTER_LINEAR);
	
		ClutterBehaviour* b_scale_1 = clutter_behaviour_scale_new(alpha1, 1.00, 1.00, 1.15, 1.15); 
		ClutterBehaviour* b_rotate_1 = clutter_behaviour_rotate_new(alpha1, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CW, 4.99, 5.0);
		ClutterBehaviour* b_opacity_1 = clutter_behaviour_opacity_new(alpha, 0xfe, 0xff);
	
		ClutterBehaviour* b_scale_2 = clutter_behaviour_scale_new(alpha2, 1.15, 1.15, 1.13, 1.13); 
		ClutterBehaviour* b_rotate_2 = clutter_behaviour_rotate_new(alpha2, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CCW, 5.0, 360.0-6.0);
	
		ClutterBehaviour* b_scale_3 = clutter_behaviour_scale_new(alpha3, 1.13, 1.13, 1.10, 1.10); 
		ClutterBehaviour* b_rotate_3 = clutter_behaviour_rotate_new(alpha3, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CW, 360-6.0, 3.0);
	
		ClutterBehaviour* b_scale_4 = clutter_behaviour_scale_new(alpha4, 1.10, 1.10, 1.10, 1.10);
		ClutterBehaviour* b_rotate_4 = clutter_behaviour_rotate_new(alpha4, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CCW, 3.0, 0.0);
	
		clutter_behaviour_apply(b_scale_1, up_pic);
		clutter_behaviour_apply(b_scale_2, up_pic);
		clutter_behaviour_apply(b_scale_3, up_pic);
		clutter_behaviour_apply(b_scale_4, up_pic);
	
		clutter_behaviour_apply(b_rotate_1, up_pic);
		clutter_behaviour_apply(b_rotate_2, up_pic);
		clutter_behaviour_apply(b_rotate_3, up_pic);
		clutter_behaviour_apply(b_rotate_4, up_pic);
	
		clutter_behaviour_apply(b_scale_1, frame_pic);
		clutter_behaviour_apply(b_scale_2, frame_pic);
		clutter_behaviour_apply(b_scale_3, frame_pic);
		clutter_behaviour_apply(b_scale_4, frame_pic);
	
		clutter_behaviour_apply(b_rotate_1, frame_pic);
		clutter_behaviour_apply(b_rotate_2, frame_pic);
		clutter_behaviour_apply(b_rotate_3, frame_pic);
		clutter_behaviour_apply(b_rotate_4, frame_pic);
	
		clutter_behaviour_apply(b_scale_1, border_pic);
		clutter_behaviour_apply(b_scale_2, border_pic);
		clutter_behaviour_apply(b_scale_3, border_pic);
		clutter_behaviour_apply(b_scale_4, border_pic);
	
		clutter_behaviour_apply(b_rotate_1, border_pic);
		clutter_behaviour_apply(b_rotate_2, border_pic);
		clutter_behaviour_apply(b_rotate_3, border_pic);
		clutter_behaviour_apply(b_rotate_4, border_pic);
	
		clutter_score_append(score, NULL,tl1);
		clutter_score_append(score, tl1,tl2);
		clutter_score_append(score, tl2,tl3);
		clutter_score_append(score, tl3,tl4);


		behaviour_scale = clutter_behaviour_scale_new(alpha,1.5, 1.5, 1.08, 1.08);
		behaviour_rotate = clutter_behaviour_rotate_new(shake_alpha, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CCW, 20.0, 340.0);
		behaviour_opacity = clutter_behaviour_opacity_new(alpha, 0xdf, 0xaf);
	
		border_scale = clutter_behaviour_scale_new(alpha, 1.5,1.5,1.08,1.08);
		border_opacity = clutter_behaviour_opacity_new(alpha, 0x20, 0xdf);
		border_rotate = clutter_behaviour_rotate_new(shake_alpha, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CCW, 20.0, 340.0);
	
		clutter_behaviour_apply(border_scale, border_pic);
		clutter_actor_move_anchor_point_from_gravity (border_pic, CLUTTER_GRAVITY_CENTER);
		clutter_behaviour_apply(border_opacity, border_pic);
		clutter_behaviour_apply(border_rotate, border_pic);
		
		clutter_behaviour_apply(behaviour_scale, up_pic);
		clutter_actor_move_anchor_point_from_gravity (up_pic, CLUTTER_GRAVITY_CENTER);
		clutter_behaviour_apply(behaviour_rotate, up_pic);

		clutter_behaviour_apply(behaviour_scale, frame_pic);
		clutter_actor_move_anchor_point_from_gravity (frame_pic, CLUTTER_GRAVITY_CENTER);
		clutter_behaviour_apply(behaviour_rotate, frame_pic);
	  }

	  if(1)
	  {
		const int step_time = 500;
		ClutterTimeline* tl1 = clutter_timeline_new(step_time);
#if 1
		ClutterAlpha* al1 = clutter_alpha_new_full(tl1, CLUTTER_EASE_OUT_ELASTIC);
#else
		ClutterAlpha* al1 = clutter_alpha_new_full(tl1, CLUTTER_LINEAR);
#endif 
		ClutterBehaviour* bS1 = clutter_behaviour_scale_new(al1, 1.28, 1.28, 1.00, 1.00);

		clutter_behaviour_apply(bS1, border_pic);
		clutter_actor_move_anchor_point_from_gravity (border_pic, CLUTTER_GRAVITY_CENTER);
		
		clutter_behaviour_apply(bS1, up_pic);
		clutter_actor_move_anchor_point_from_gravity (up_pic, CLUTTER_GRAVITY_CENTER);

		clutter_behaviour_apply(bS1, frame_pic);
		clutter_actor_move_anchor_point_from_gravity (frame_pic, CLUTTER_GRAVITY_CENTER);

		clutter_score_append(pressed_score, NULL, tl1);

	  }

	} //END of [ if(picture_empty[0] == 'F') ]

	else //START when [ picture_empty[0] != 'F' ]
	{
		ClutterTimeline* tl1 = clutter_timeline_new(300);
		ClutterAlpha* al1 = clutter_alpha_new_full(tl1, CLUTTER_EASE_IN_CUBIC);
		ClutterBehaviour* bO1 = clutter_behaviour_opacity_new(al1, 0x00, 0xff);
		clutter_behaviour_apply(bO1, border_pic);
		clutter_score_append(score, NULL, tl1);	
	}  //END of [ if(picture_empty[0] == 'F') ]
	
	

//	clutter_timeline_add_marker_at_time(timeline, "StartPointer", 0);

	g_object_set_data_full(G_OBJECT(timeline), "is-back", g_strdup("T"),g_free);

	g_object_set_data(G_OBJECT(timeline), "up_pic", up_pic);
	g_object_set_data(G_OBJECT(timeline), "border_pic", border_pic);
	g_object_set_data(G_OBJECT(timeline), "frame_pic", frame_pic);

	g_object_set_data(G_OBJECT(timeline), "up_pic_pos_x", up_pic_pos_x);
	g_object_set_data(G_OBJECT(timeline), "up_pic_pos_y", up_pic_pos_y);
	g_object_set_data(G_OBJECT(timeline), "border_pic_pos_x", border_pic_pos_x);
	g_object_set_data(G_OBJECT(timeline), "border_pic_pos_y", border_pic_pos_y);
	g_object_set_data(G_OBJECT(timeline), "frame_pic_pos_x", frame_pic_pos_x);
	g_object_set_data(G_OBJECT(timeline), "frame_pic_pos_y", frame_pic_pos_y);

	g_object_set_data(G_OBJECT(timeline), "score", score);
	g_object_set_data(G_OBJECT(timeline), "pressed_score", pressed_score);

	g_signal_connect(G_OBJECT(webblk_icon), "enter-event", G_CALLBACK(webblk_icon_rollover_enter_cb), (gpointer)timeline);
	g_signal_connect(G_OBJECT(webblk_icon), "leave-event", G_CALLBACK(webblk_icon_rollover_leave_cb), (gpointer)timeline);
	g_signal_connect(G_OBJECT(webblk_icon), "button-press-event", G_CALLBACK(webblk_icon_pressed_cb), (gpointer)timeline);
	return webblk_icon;
} //create_webblk_icon()



static gdouble shake_alpha_func(ClutterAlpha *alpha , gpointer data)
{

	ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
	gdouble pos = clutter_timeline_get_progress(timeline);
	gdouble val;

	if(pos <= 0.25)
	{
		val = (1-4*pos)*0.5;
	}
	else if(pos <= 0.75)
	{

		val = 2*(pos-0.25); 
	}
	else
	{
		val = 4*(1-pos)*0.5 + 0.5;
	}
	return val;
} //triangle_alpha_func()








static void icon_effect_apply(ClutterActor* group)
{



} //icon_effect_apply()



static gboolean webblk_icon_pressed_cb(ClutterActor* act, ClutterEvent* evt, gpointer data)
{
	gboolean ret = FALSE;
	g_return_val_if_fail(act != NULL ,ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(act) ,ret);
	ClutterTimeline* timeline =  (ClutterTimeline*)data;
	g_return_if_fail(CLUTTER_IS_TIMELINE(timeline));

	ClutterScore* pressed_score = (ClutterScore*) g_object_get_data(G_OBJECT(timeline), "pressed_score");
	g_return_val_if_fail(pressed_score != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(pressed_score),ret);

	ClutterScore* enter_score = (ClutterScore*) g_object_get_data(G_OBJECT(timeline), "score");
	g_return_val_if_fail(enter_score != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(enter_score),ret);


	ClutterActor* up_pic = (ClutterActor*) g_object_get_data(G_OBJECT(timeline), "up_pic");
	g_return_val_if_fail(up_pic != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(up_pic), ret);

	ClutterActor* border_pic = (ClutterActor*) g_object_get_data(G_OBJECT(timeline), "border_pic");
	g_return_val_if_fail(border_pic != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(border_pic), ret);


	char* picture_empty = (char*) g_object_get_data(G_OBJECT(up_pic), "picture-empty");
	if(picture_empty != NULL && picture_empty[0]=='F')
	{
		webblk_icon_rollover_leave_cb(act, evt, data);
		clutter_actor_set_opacity(border_pic, 0xff);
		clutter_score_start(pressed_score);

	}
	return ret;

} //webblk_icon_pressed_cb()


static gboolean webblk_icon_release_cb(ClutterActor* act, ClutterEvent* evt, gpointer data)
{
	gboolean ret = FALSE;
	g_return_val_if_fail(act != NULL ,ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(act) ,ret);
	ClutterTimeline* timeline =  (ClutterTimeline*)data;
	g_return_if_fail(CLUTTER_IS_TIMELINE(timeline));



} //webblk_icon_release_cb()

