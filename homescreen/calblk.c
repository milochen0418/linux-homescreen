#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <gtk/gtk.h>
#include <clutter-gtk/clutter-gtk.h>
#include "calblk.h"
#include "clutter-behaviour-scale-gravity.h"


#ifndef CLUTTER_IS_COLOR
#define CLUTTER_IS_COLOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), CLUTTER_TYPE_COLOR) )
#endif //CLUTTER_IS_COLOR

#define BORDER_FILEPATH ("Untitled-2.png")
#define WEBBLK_BG_FILEPATH ("cal_BG.png")
#define WEBBLK_BG_SHADOW_FILEPATH ("cal_shadow.png")

#define ROW_NUMBER (6)
#define COL_NUMBER (7)
#define TIMELINE_NUMBER ((ROW_NUMBER)+(COL_NUMBER)-1)
#define ICON_WIDTH 30
#define ICON_HEIGHT 38
#define BORDER_WIDTH 38
#define BORDER_HEIGHT 46
#define ICON_X_DIST 4
#define ICON_Y_DIST 4
#define ICONS_OFFSET_X 22
#define ICONS_OFFSET_Y 87

#define FLIP_MS (500)
#define FLIP_DELAY_MS (30)

#define ARROW_MS (200)

#define ARROW_LEFT_FILEPATH ("ArrowL.png")
#define ARROW_RIGHT_FILEPATH ("ArrowR.png")


#define WAVE_FLIP


/* Calendar Special Function */
static void calendar_number(
int year, int month, int pos_x, int pos_y, int* num, gboolean* is_in_this_month);
static int get_weekday(int year, int month, int day);



/* function for Web Block  days*/
static gboolean calblk_icon_rollover_enter_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean calblk_icon_rollover_leave_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static ClutterActor* create_calblk_icon(const char* icon_filepath);

static gboolean calblk_next_month_cb(ClutterActor* act, ClutterEvent *evt, gpointer data);
static gboolean calblk_prev_month_cb(ClutterActor* act, ClutterEvent *evt, gpointer data);
static void calblk_icon_set_number(ClutterActor* icon, int number);
static void calblk_icon_set_color(ClutterActor* icon, ClutterColor* fg_color, ClutterColor* bg_color);
static gboolean calblk_refresh_data(ClutterActor* act);
static gboolean press_day_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);

static void calblk_flip_effect_and_refresh_data(ClutterActor* act, gboolean is_prev_month);
static gdouble triangle_alpha_func(ClutterAlpha *alpha , gpointer data);
static void marker_reached_cb (ClutterTimeline* timeline, gchar* marker_name, gint msecs, gpointer data);
static void timeline_started_cb (ClutterTimeline* timeline, gpointer data);
static void timeline_completed_cb (ClutterTimeline* timeline, gpointer data);
static void last_timeline_completed_cb (ClutterTimeline* timeline, gpointer data);

static ClutterActor* create_right_arrow(void);
static ClutterActor* create_left_arrow(void);
static void left_arrow_effect_apply(ClutterActor* act);
static void right_arrow_effect_apply(ClutterActor* act);

static gboolean right_arrow_enter_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean right_arrow_leave_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);

static gboolean left_arrow_enter_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean left_arrow_leave_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean left_arrow_press_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean left_arrow_release_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean left_arrow_pressed_effect_completed_cb(ClutterScore* pressed_score, gpointer data);


static gboolean right_arrow_enter_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean right_arrow_leave_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean right_arrow_press_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean right_arrow_release_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean right_arrow_pressed_effect_completed_cb(ClutterScore* pressed_score, gpointer data);



typedef struct
{

#if 1
	ClutterActor* cells[ROW_NUMBER][COL_NUMBER];

#else
	ClutterActor*** cells; /*cells[ROW_NUMBER][COL_NUMBER]*/

#endif 
	ClutterTimeline* timelines[TIMELINE_NUMBER]; /*timeline*/
	ClutterAlpha* alphas[TIMELINE_NUMBER];
#ifdef WAVE_FLIP
	ClutterAlpha* triangle_alphas[TIMELINE_NUMBER];
#endif
	ClutterTimeline* prev_timelines[TIMELINE_NUMBER]; /*timeline*/
	ClutterAlpha* prev_alphas[TIMELINE_NUMBER]; /* alphas */
#ifdef WAVE_FLIP
	ClutterAlpha* prev_triangle_alphas[TIMELINE_NUMBER];
#endif
	ClutterActor* calblk;
	ClutterActor* labelYearMonth;
	ClutterActor* labelWeekdays[7];
	ClutterActor* btnNextMonth;
	ClutterActor* btnPrevMonth;
	int year;
	int month;
	int pointer_row; //Use pointer_row < 0 to denote pointer is not point to calendar
	int pointer_col; //Use pointer_col < 0 to denote pointer is not point to calendar

}private_data_t;

static private_data_t* private_data_new0(void);
static private_data_free(private_data_t* priv);


typedef struct
{
	int status;
}date_info_t;



static date_info_t* date_info_new0(void);
static void  date_info_free(date_info_t* info);
static date_info_t* get_date_info(int year, int month, int day);



static gdouble triangle_alpha_func(ClutterAlpha *alpha , gpointer data)
{
	ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);
	gdouble pos = clutter_timeline_get_progress(timeline);
	gdouble val = (pos > 0.5)?(2*(1-pos)):(2*pos);
	return val;
} //triangle_alpha_func()
static gulong TRIANGLE_LINEAR=0;



ClutterActor* create_calblk(void)
{

#if 1
	
	TRIANGLE_LINEAR = clutter_alpha_register_func(triangle_alpha_func , NULL);

#endif 

	private_data_t* priv = private_data_new0();

	priv->pointer_row = -1;
	priv->pointer_col = -1;
#if 1
	priv->year = 2009;
	priv->month = 6;
	
#endif 


	ClutterActor* (*cells)[COL_NUMBER];
	cells = priv->cells;


	ClutterActor* calblk; //ClutterGroup
	ClutterActor* background; //ClutterTexture
	ClutterActor* background_shadow; //ClutterTexture
	ClutterActor* title; //ClutterText




	const int ICON_NUM = 6;
	ClutterActor** calblk_icons = g_new0(ClutterActor*, ICON_NUM);
	const char** fps = g_new0(const char*, ICON_NUM);
	int idx;
	int col_idx;
	int row_idx;
	int tl_idx;
	

	fps[0] = g_strdup("wcap01.jpg");
	fps[1] = g_strdup("wcap02.jpg");
	fps[2] = g_strdup("wcap03.jpg");
	fps[3] = g_strdup("wcap04.jpg");
	fps[4] = g_strdup("wcap05.jpg");
	fps[5] = g_strdup("wcap06.jpg");

	calblk = clutter_group_new();
	title = clutter_text_new_with_text("Mono 8 Bold", "Calendar");
	GError* gerr= NULL;
	background = clutter_texture_new_from_file(WEBBLK_BG_FILEPATH,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	background_shadow = clutter_texture_new_from_file(WEBBLK_BG_SHADOW_FILEPATH,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);

	idx = 0;
	for (row_idx = 0; row_idx < ROW_NUMBER; row_idx++)
	{
	  for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
	  {

		cells[row_idx][col_idx] = create_calblk_icon(fps[idx%5]);
		idx++;
	  } // for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
	} //for (row_idx = 0; row_idx < ROW_NUMBER; row_idx++)


	clutter_actor_set_position(title, 0,0);
	clutter_actor_set_position(background, 0,0);
	clutter_actor_set_position(background_shadow, -15,333);


	for (row_idx = 0; row_idx < ROW_NUMBER; row_idx++)
	{
	  for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
	  {
		clutter_actor_set_position(cells[row_idx][col_idx], 
			ICONS_OFFSET_X + ( ICON_WIDTH + ICON_X_DIST)*col_idx - ICON_X_DIST,
			ICONS_OFFSET_Y + (ICON_HEIGHT + ICON_Y_DIST)*row_idx - ICON_Y_DIST
		);
	  } // for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
	} //for (row_idx = 0; row_idx < ROW_NUMBER; row_idx++)






	clutter_container_add_actor(CLUTTER_CONTAINER(calblk), background_shadow);
	clutter_container_add_actor(CLUTTER_CONTAINER(calblk), background);

	ClutterColor *text_color = clutter_color_new(0xff, 0xff, 0xff, 0xff);
	clutter_text_set_color(CLUTTER_TEXT(title), text_color);
	clutter_color_free(text_color);
	clutter_actor_set_position(title, 90,-1);

	clutter_container_add_actor(CLUTTER_CONTAINER(calblk), title);


	for (row_idx = 0; row_idx < ROW_NUMBER; row_idx++)
	{
	  for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
	  {	
		int* p_row_idx = g_new0(int,1);
		int* p_col_idx = g_new0(int,1);
		*p_row_idx = row_idx;
		*p_col_idx = col_idx;
		int* p_day_num = g_new0(int,1);
		ClutterActor* act = cells[row_idx][col_idx];

		clutter_container_add_actor(CLUTTER_CONTAINER(calblk), act);
		g_object_set_data(G_OBJECT(act),"priv",priv);
		g_object_set_data(G_OBJECT(act),"row-idx", p_row_idx);
		g_object_set_data(G_OBJECT(act),"col-idx", p_col_idx);
		g_signal_connect(G_OBJECT(act), "button-press-event", G_CALLBACK(press_day_cb), NULL);
	  } // for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
	} //for (row_idx = 0; row_idx < ROW_NUMBER; row_idx++)

	if(1)
	{
		gfloat height;
		gfloat width;
		clutter_actor_get_size(background, &width, &height);
		clutter_actor_set_size(calblk, width,height);
	}
	else
	{
		clutter_actor_set_size(calblk, 300,400);
	}



	ClutterColor* arrow_color = clutter_color_new(0x7f, 0x3f, 0x7f, 0xff);


 if(1)
 { 
	priv->btnNextMonth = create_right_arrow();
	priv->btnPrevMonth = create_left_arrow();
	if(priv->btnNextMonth == NULL)
	{
		priv->btnNextMonth = clutter_rectangle_new_with_color(arrow_color);
	}

	if(priv->btnPrevMonth == NULL)
	{
		priv->btnPrevMonth = clutter_rectangle_new_with_color(arrow_color);
	}
 } //if(1)


#if 1
	priv->labelYearMonth = clutter_text_new_with_text("Sans 7","July 2009");
	ClutterColor* label_color = clutter_color_new(0xaf, 0x00, 0x00, 0xff);
	clutter_text_set_color(CLUTTER_TEXT(priv->labelYearMonth),label_color);
	clutter_color_free(label_color);

	if(1) //set labelYearMonth position
	{
		gfloat label_width;
		gfloat label_height;
		gfloat parent_width;
		gfloat parent_height;
		clutter_actor_get_size(calblk, &parent_width, &parent_height);
		clutter_actor_get_size(priv->labelYearMonth, &label_width, &label_height);
		clutter_actor_set_position(priv->labelYearMonth,
			(parent_width-label_width)/2,
			35
		);

	}
#endif

	
	
	for(tl_idx = 0; tl_idx < TIMELINE_NUMBER; tl_idx++)
	{
		ClutterTimeline* timeline;
		ClutterAlpha* alpha;
		ClutterBehaviour* behaviour_rotate;
		GList* actor_list = NULL;
#ifdef WAVE_FLIP
		ClutterAlpha* triangle_alpha;
		ClutterBehaviour* behaviour_scale_gravity;
		ClutterBehaviour* behaviour_opacity;
#endif 


		priv->timelines[tl_idx] = clutter_timeline_new(FLIP_MS);
		timeline = priv->timelines[tl_idx];
		priv->alphas[tl_idx] = clutter_alpha_new_full(timeline, CLUTTER_LINEAR);
		alpha = priv->alphas[tl_idx];
#ifdef WAVE_FLIP
		priv->triangle_alphas[tl_idx] = clutter_alpha_new_with_func(timeline, triangle_alpha_func, NULL, NULL);
		triangle_alpha = priv->triangle_alphas[tl_idx];
#endif 


		behaviour_rotate = clutter_behaviour_rotate_new(alpha, CLUTTER_Y_AXIS, CLUTTER_ROTATE_CW, 0.1, 179.9);
#ifdef WAVE_FLIP
		behaviour_scale_gravity = clutter_behaviour_scale_gravity_new(triangle_alpha, CLUTTER_GRAVITY_CENTER,1.00,1.00,2.00,1.30);
		behaviour_opacity = clutter_behaviour_opacity_new(triangle_alpha, 0xff, 0x4f);
#endif 


		guint* delay_ms = g_new0(guint,1);
		*delay_ms  = tl_idx*FLIP_DELAY_MS ;
		g_object_set_data(G_OBJECT(timeline), "delay-ms", delay_ms);
		for (row_idx = 0; row_idx < ROW_NUMBER; row_idx++)
		{
		  for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
		  {
			if(row_idx + col_idx == tl_idx)
			{
				actor_list = g_list_prepend( actor_list, cells[row_idx][col_idx]);
				clutter_behaviour_apply(behaviour_rotate, cells[row_idx][col_idx]);
#ifdef WAVE_FLIP
				clutter_behaviour_apply(behaviour_scale_gravity, cells[row_idx][col_idx]);
				clutter_behaviour_apply(behaviour_opacity, cells[row_idx][col_idx]);

#endif


				clutter_behaviour_rotate_set_center(
					CLUTTER_BEHAVIOUR_ROTATE(behaviour_rotate),
					BORDER_WIDTH/2, BORDER_HEIGHT/2, 0
				);

			}
		  } // for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
		} //for (row_idx = 0; row_idx < ROW_NUMBER; row_idx++)
		g_object_set_data(G_OBJECT(timeline), "actor-list", actor_list);
		clutter_timeline_add_marker_at_time(timeline, "change-icon", FLIP_MS/2);
		g_signal_connect(G_OBJECT(timeline), "marker-reached", G_CALLBACK(marker_reached_cb), actor_list);
		g_signal_connect(G_OBJECT(timeline), "started", G_CALLBACK(timeline_started_cb), actor_list);
		g_signal_connect(G_OBJECT(timeline), "completed", G_CALLBACK(timeline_completed_cb), actor_list);
		if(tl_idx == TIMELINE_NUMBER-1)
		{
			g_signal_connect(G_OBJECT(timeline), "completed", G_CALLBACK(last_timeline_completed_cb), &(priv->calblk));
		}
	} //for(tl_idx = 0; tl_idx < TIMELINE_NUMBER; tl_idx++)
	


	for(tl_idx = 0; tl_idx < TIMELINE_NUMBER; tl_idx++)
	{
		ClutterTimeline* timeline;
		ClutterAlpha* alpha;
		ClutterBehaviour* behaviour_rotate;
		GList* actor_list = NULL;
#ifdef WAVE_FLIP
		ClutterAlpha* triangle_alpha;
		ClutterBehaviour* behaviour_scale_gravity;
		ClutterBehaviour* behaviour_opacity;
#endif 



		priv->prev_timelines[tl_idx] = clutter_timeline_new(FLIP_MS);
		timeline = priv->prev_timelines[tl_idx];
		priv->prev_alphas[tl_idx] = clutter_alpha_new_full(timeline, CLUTTER_LINEAR);
		alpha = priv->prev_alphas[tl_idx];
#ifdef WAVE_FLIP
		priv->prev_triangle_alphas[tl_idx] = clutter_alpha_new_with_func(timeline, triangle_alpha_func, NULL, NULL);
		triangle_alpha = priv->prev_triangle_alphas[tl_idx];
#endif 


		//behaviour_rotate = clutter_behaviour_rotate_new(alpha, CLUTTER_Y_AXIS, CLUTTER_ROTATE_CW, 0.1, 179.9);
		behaviour_rotate = clutter_behaviour_rotate_new(alpha, CLUTTER_Y_AXIS, CLUTTER_ROTATE_CCW, 0.1, 179.9);
#ifdef WAVE_FLIP
		behaviour_scale_gravity = clutter_behaviour_scale_gravity_new(triangle_alpha, CLUTTER_GRAVITY_CENTER,1.00,1.00,2.00,1.30);
		behaviour_opacity = clutter_behaviour_opacity_new(triangle_alpha, 0xff, 0x4f);
#endif 


		guint* delay_ms = g_new0(guint,1);
		*delay_ms  = tl_idx*FLIP_DELAY_MS ;
		g_object_set_data(G_OBJECT(timeline), "delay-ms", delay_ms);
		for (row_idx = 0; row_idx < ROW_NUMBER; row_idx++)
		{
		  for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
		  {
			if(row_idx + ((COL_NUMBER-1) - col_idx) == tl_idx)
			{
				actor_list = g_list_prepend( actor_list, cells[row_idx][col_idx]);
				clutter_behaviour_apply(behaviour_rotate, cells[row_idx][col_idx]);
#ifdef WAVE_FLIP
				clutter_behaviour_apply(behaviour_scale_gravity, cells[row_idx][col_idx]);
				clutter_behaviour_apply(behaviour_opacity, cells[row_idx][col_idx]);

#endif




				clutter_behaviour_rotate_set_center(
					CLUTTER_BEHAVIOUR_ROTATE(behaviour_rotate),
					BORDER_WIDTH/2, BORDER_HEIGHT/2, 0
				);
			}
		  } // for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
		} //for (row_idx = 0; row_idx < ROW_NUMBER; row_idx++)
		g_object_set_data(G_OBJECT(timeline), "actor-list", actor_list);
		clutter_timeline_add_marker_at_time(timeline, "change-icon", FLIP_MS/2);
		g_signal_connect(G_OBJECT(timeline), "marker-reached", G_CALLBACK(marker_reached_cb), actor_list);
		g_signal_connect(G_OBJECT(timeline), "started", G_CALLBACK(timeline_started_cb), actor_list);
		g_signal_connect(G_OBJECT(timeline), "completed", G_CALLBACK(timeline_completed_cb), actor_list);
		if(tl_idx == TIMELINE_NUMBER-1)
		{
			g_signal_connect(G_OBJECT(timeline), "completed", G_CALLBACK(last_timeline_completed_cb), &(priv->calblk));
		}
	} //for(tl_idx = 0; tl_idx < TIMELINE_NUMBER; tl_idx++)
	








	clutter_color_free(arrow_color);
	clutter_actor_set_reactive(priv->btnNextMonth, TRUE);
	clutter_actor_set_reactive(priv->btnPrevMonth, TRUE);
	g_signal_connect(G_OBJECT(priv->btnNextMonth), "button-press-event", 
					G_CALLBACK(calblk_next_month_cb), NULL);
	g_signal_connect(G_OBJECT(priv->btnPrevMonth), "button-press-event", 
					G_CALLBACK(calblk_prev_month_cb), NULL);

	if(CLUTTER_IS_RECTANGLE(priv->btnPrevMonth))
	{
		clutter_actor_set_size(priv->btnPrevMonth, 30,30);
	}

	if(CLUTTER_IS_RECTANGLE(priv->btnNextMonth))
	{
		clutter_actor_set_size(priv->btnNextMonth, 30,30);
	}

	clutter_actor_set_position(priv->btnPrevMonth, 20,30);
	clutter_actor_set_position(priv->btnNextMonth, 230,30);

	clutter_container_add_actor(CLUTTER_CONTAINER(calblk), priv->btnNextMonth);
	clutter_container_add_actor(CLUTTER_CONTAINER(calblk), priv->btnPrevMonth);
	clutter_container_add_actor(CLUTTER_CONTAINER(calblk), priv->labelYearMonth);





	priv->calblk = calblk;
	g_object_set_data(G_OBJECT(calblk),"priv",priv);
	g_object_set_data(G_OBJECT(priv->btnNextMonth),"priv",priv);
	g_object_set_data(G_OBJECT(priv->btnPrevMonth),"priv",priv);









	calblk_refresh_data(calblk);
	return calblk;

} //create_calblk();







/* function for Web Block  */

static gboolean calblk_icon_rollover_enter_cb(ClutterActor* act, ClutterEvent* evt,gpointer data)
{
	g_return_if_fail(CLUTTER_IS_ACTOR(act));
	ClutterTimeline* timeline =  (ClutterTimeline*)data;
	g_return_if_fail(CLUTTER_IS_TIMELINE(timeline));
	g_object_set_data_full(G_OBJECT(timeline), "is-back", g_strdup("T"),g_free);

	clutter_actor_raise_top(act);

	clutter_timeline_stop(timeline);


	clutter_timeline_start(timeline);


	return TRUE;
} //calblk_icon_rollover_enter_cb()


static gboolean calblk_icon_rollover_leave_cb(ClutterActor* act, ClutterEvent* evt,gpointer data)
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
	

	return TRUE;
} //calblk_icon_rollover_leave_cb()





static ClutterActor* create_calblk_icon(const char* icon_filepath)
{
	g_return_val_if_fail(icon_filepath != NULL, NULL);
	g_return_val_if_fail(g_file_test(icon_filepath, G_FILE_TEST_EXISTS), NULL);
	
	
	ClutterActor* calblk_icon; //ClutterGroup
	ClutterActor* title; //ClutterText
	ClutterActor* up_pic; //ClutterTexture
	ClutterActor* border_pic; //ClutterTexture
	ClutterActor* text;


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
	

	calblk_icon = clutter_group_new();
	title = clutter_text_new_with_text("Mono 8", "Webblk\nIcon");
	GError* gerr= NULL;

	ClutterColor* rect_color = clutter_color_new(0x00, 0xff, 0x7f, 0xff);
	up_pic = clutter_rectangle_new_with_color(rect_color);
	clutter_color_free(rect_color);

	up_pic_pos_x = g_new0(gfloat,1);
	up_pic_pos_y = g_new0(gfloat,1);
	border_pic_pos_x = g_new0(gfloat, 1);
	border_pic_pos_y = g_new0(gfloat, 1);


	border_pic = clutter_texture_new_from_file(BORDER_FILEPATH, &gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	
	clutter_actor_set_position(title, 5, 5);



	clutter_actor_set_position(up_pic, (BORDER_WIDTH - ICON_WIDTH) / 2, (BORDER_HEIGHT - ICON_HEIGHT ) / 2);
	clutter_actor_set_size(up_pic, ICON_WIDTH, ICON_HEIGHT);

	clutter_actor_get_position(up_pic, up_pic_pos_x, up_pic_pos_y);

	clutter_actor_set_position(border_pic,0, 0);
	clutter_actor_set_size(border_pic, BORDER_WIDTH, BORDER_HEIGHT);
	clutter_actor_get_position(border_pic, border_pic_pos_x, border_pic_pos_y);

	clutter_actor_set_opacity(border_pic,0x00);
	clutter_actor_set_size(calblk_icon, BORDER_WIDTH , BORDER_HEIGHT );

	text = clutter_text_new_with_text("Sans 12 Bold", "00");
	gfloat width;
	gfloat height;
	clutter_actor_get_size(text, &width, &height);
	clutter_actor_set_position(text, BORDER_WIDTH/2 - width/2 , BORDER_HEIGHT/2 - height/2);

	clutter_container_add(CLUTTER_CONTAINER(calblk_icon)/*, title*/, up_pic, border_pic, text,NULL);
	clutter_actor_set_reactive(calblk_icon, TRUE);

	timeline = clutter_timeline_new(750);


	alpha = clutter_alpha_new_full(timeline, CLUTTER_EASE_OUT_BOUNCE);

	behaviour_scale = clutter_behaviour_scale_new(alpha,1.0, 1.0, 1.28, 1.28);
	behaviour_rotate = clutter_behaviour_rotate_new(alpha, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CW, 0.1, 3.0);
	behaviour_opacity = clutter_behaviour_opacity_new(alpha, 0xff, 0xff);

	
	clutter_behaviour_rotate_set_center(CLUTTER_BEHAVIOUR_ROTATE(behaviour_rotate), ICON_WIDTH / 2,ICON_HEIGHT / 2, 0);


	border_scale = clutter_behaviour_scale_new(alpha, 1.0,1.0,1.28,1.28);
	border_opacity = clutter_behaviour_opacity_new(alpha, 0x00, 0xff);
	border_rotate = clutter_behaviour_rotate_new(alpha, CLUTTER_Z_AXIS, CLUTTER_ROTATE_CW, 0.1, 3.0);
	clutter_behaviour_rotate_set_center(CLUTTER_BEHAVIOUR_ROTATE(border_rotate), BORDER_WIDTH / 2, BORDER_HEIGHT / 2, 0);



	clutter_behaviour_apply(border_scale, border_pic);

	clutter_actor_move_anchor_point_from_gravity (border_pic, CLUTTER_GRAVITY_CENTER);

	clutter_behaviour_apply(border_opacity, border_pic);
	
	clutter_behaviour_apply(behaviour_scale, up_pic);
	clutter_actor_move_anchor_point_from_gravity (up_pic, CLUTTER_GRAVITY_CENTER);

	clutter_timeline_add_marker_at_time(timeline, "StartPointer", 0);

	g_object_set_data_full(G_OBJECT(timeline), "is-back", g_strdup("T"),g_free);

	g_object_set_data(G_OBJECT(timeline), "up_pic", up_pic);
	g_object_set_data(G_OBJECT(timeline), "border_pic", border_pic);
	g_object_set_data(G_OBJECT(timeline), "up_pic_pos_x", up_pic_pos_x);
	g_object_set_data(G_OBJECT(timeline), "up_pic_pos_y", up_pic_pos_y);
	g_object_set_data(G_OBJECT(timeline), "border_pic_pos_x", border_pic_pos_x);
	g_object_set_data(G_OBJECT(timeline), "border_pic_pos_y", border_pic_pos_y);
	
	g_object_set_data(G_OBJECT(calblk_icon), "rect", up_pic);
	g_object_set_data(G_OBJECT(calblk_icon), "text", text);



	g_signal_connect(G_OBJECT(calblk_icon), "enter-event", G_CALLBACK(calblk_icon_rollover_enter_cb), (gpointer)timeline);
	g_signal_connect(G_OBJECT(calblk_icon), "leave-event", G_CALLBACK(calblk_icon_rollover_leave_cb), (gpointer)timeline);


	return calblk_icon;
} //create_calblk_icon()


static gboolean calblk_refresh_data(ClutterActor *act)
{
	//TODO
	g_return_val_if_fail(act!=NULL, FALSE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(act), FALSE);
	private_data_t* priv = (private_data_t*)g_object_get_data(G_OBJECT(act),"priv");
	g_return_val_if_fail(priv != NULL, FALSE);
	ClutterActor* (*cells)[COL_NUMBER] = priv->cells;

	int year = priv->year;
	int month =  priv->month;
	int row_idx = 0;
	int col_idx = 0;
	for(row_idx = 0; row_idx < ROW_NUMBER; row_idx++)
	{
	  for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
	  {
		ClutterActor* icon = cells[row_idx][col_idx];
		ClutterColor* fg_color;
		ClutterColor* bg_color;
		ClutterColor* gray_fg_color;
		ClutterColor* gray_bg_color;
		ClutterColor* gray_bd_color;

		fg_color = clutter_color_new(0x22, 0x22, 0x22, 0xff);
		bg_color = clutter_color_new(0xaf, 0xaf, 0xaf, 0xff);
		gray_fg_color = clutter_color_new(0xd8,0xd8,0xd8,0xff);
		gray_bd_color = clutter_color_new(0xd8,0xd8,0xd8,0xff);
		gray_bg_color = clutter_color_new(0xe8,0xe8,0xe8,0xff);

		
		calblk_icon_set_color(icon, fg_color, bg_color);
		gboolean in_month;
		int day;
		calendar_number( year,month, col_idx, row_idx, &day, &in_month);
		if(in_month == FALSE)
		{
			calblk_icon_set_color(icon, gray_fg_color, gray_bg_color);
			ClutterActor* rect = (ClutterActor*) g_object_get_data(G_OBJECT(icon), "rect");
			if(CLUTTER_IS_RECTANGLE(rect))
			{
				clutter_rectangle_set_border_width(CLUTTER_RECTANGLE(rect),1);
				clutter_rectangle_set_border_color(CLUTTER_RECTANGLE(rect), gray_bd_color);
			}
		}

		calblk_icon_set_number(icon, day);
	
		clutter_color_free(gray_fg_color);
		clutter_color_free(gray_bg_color);
		clutter_color_free(fg_color);
		clutter_color_free(bg_color);
	  }//  for(col_idx = 0; col_idx < COL_NUMBER; col_idx++)
	}//	for(row_idx = 0; row_idx < ROW_NUMBER; row_idx++)
	
#if 1
	static const char* mons[13]={"","Junuary","February","March","April","May","June","July","August","September","October","November","December"};
	gchar* str=	g_strdup_printf("%s %d", mons[month],year);
	clutter_text_set_text(CLUTTER_TEXT( priv->labelYearMonth),str);
	
	if(1) //set labelYearMonth position
	{
		gfloat label_width;
		gfloat label_height;
		gfloat parent_width;
		gfloat parent_height;
		clutter_actor_get_size(priv->calblk, &parent_width, &parent_height);
		clutter_actor_get_size(priv->labelYearMonth, &label_width, &label_height);
		clutter_actor_set_position(priv->labelYearMonth,
			(parent_width-label_width)/2,
			35
		);

	}




	g_free(str);
#endif 
	
	

	return TRUE;

} //calblk_refresh_data()




static gboolean calblk_next_month_cb(ClutterActor* act, ClutterEvent *evt, gpointer data)
{
	ClutterButtonEvent* btnevt = (ClutterButtonEvent*)evt;
	//if(btnevt->click_count ==1) return TRUE;

//	if((btnevt->modifier_state & CLUTTER_BUTTON1_MASK) ) return TRUE;
	if(btnevt->click_count % 5 == 2 || btnevt->click_count % 5 == 4) return TRUE;
		
	g_debug("%s:%s:click_count=%d:button=%d", 
		G_STRLOC, G_STRFUNC, btnevt->click_count, btnevt->button);


	g_return_if_fail(act != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(act));	
	private_data_t* priv;
	priv = (private_data_t*) g_object_get_data(G_OBJECT(act), "priv");
	g_return_if_fail(priv != NULL);


	if(priv->month >= 12)
	{
		priv->month = 1;
		priv->year++;
	}
	else
	{
		priv->month  += 1;
	}
	calblk_flip_effect_and_refresh_data(act, FALSE);

//	calblk_refresh_data(priv->calblk);
	return TRUE;
} //calblk_next_month_cb()
static gboolean calblk_prev_month_cb(ClutterActor* act, ClutterEvent *evt, gpointer data)
{
	g_return_if_fail(act != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(act));	
	private_data_t* priv;
	priv = (private_data_t*) g_object_get_data(G_OBJECT(act), "priv");
	g_return_if_fail(priv != NULL);

	if(priv->month <= 1)
	{
		priv->month = 12;
		priv->year--;
	}
	else
	{
		priv->month  -= 1;
	}

	calblk_flip_effect_and_refresh_data(act, TRUE);
	//calblk_refresh_data(priv->calblk);
} //calblk_prev_month_cb()



static void calblk_flip_effect_and_refresh_data(ClutterActor* act, gboolean is_prev_month)
{
	g_return_if_fail(act!=NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(act));

	private_data_t* priv = (private_data_t*)g_object_get_data(G_OBJECT(act),"priv");
	int tl_idx;
	for(tl_idx = 0; tl_idx < TIMELINE_NUMBER; tl_idx++ )
	{
		ClutterTimeline* timeline;
		if(is_prev_month)
		{	
			timeline = priv->prev_timelines[tl_idx];
		}
		else
		{
			timeline = priv->timelines[tl_idx];
		}
		guint delay_ms = *((guint*)g_object_get_data(G_OBJECT(timeline),"delay-ms") );
		clutter_timeline_set_delay(timeline, delay_ms);
		clutter_timeline_set_direction(timeline, CLUTTER_TIMELINE_FORWARD);
		clutter_timeline_start(timeline);
	}//for(tl_idx = 0; tl_idx < TIMELINE_NUMBER; tl_idx++ )
	
} //calblk_flip_effect()

static void calblk_icon_set_number(ClutterActor* icon, int number)
{
	g_return_if_fail(icon!=NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(icon));
	g_return_if_fail(number>0);
	ClutterActor* text;
	text = g_object_get_data(G_OBJECT(icon),"text");
	g_return_if_fail(text != NULL);
	g_return_if_fail(CLUTTER_IS_TEXT(text));
	
	gchar* content = g_strdup_printf("%d",number);

	clutter_text_set_text(CLUTTER_TEXT(text), content);
	gfloat text_width;
	gfloat text_height;
	gfloat icon_width;
	gfloat icon_height;
	clutter_actor_get_size(text, &text_width, &text_height);
	clutter_actor_get_size(icon, &icon_width, &icon_height);
	clutter_actor_set_position(text, (icon_width-text_width)/2 , (icon_height-text_height)/2 );

	g_free(content);
	/* TODO */
	return;
} //calblk_icon_set_numer()


static void calblk_icon_set_color(ClutterActor* icon, ClutterColor* fg_color, ClutterColor* bg_color)
{
	g_return_if_fail(icon!=NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(icon));
	ClutterActor* rect;
	ClutterActor* text;

	rect = (ClutterActor*)g_object_get_data(G_OBJECT(icon), "rect");
	text  = (ClutterActor*)g_object_get_data(G_OBJECT(icon), "text");

	g_return_if_fail(rect != NULL);
	g_return_if_fail(text != NULL);
	g_return_if_fail(CLUTTER_IS_TEXT(text));
	g_return_if_fail(CLUTTER_IS_RECTANGLE(rect));
	
	if(fg_color != NULL)
	{
		//g_assert(TRUE!=TRUE);
		//g_return_if_fail(CLUTTER_IS_COLOR(fg_color));
		clutter_text_set_color(CLUTTER_TEXT(text), fg_color);
	}

	if(bg_color != NULL)
	{
		//g_return_if_fail(CLUTTER_IS_COLOR(bg_color));
		clutter_rectangle_set_color(CLUTTER_RECTANGLE(rect), bg_color);
	}
	return;
} //calblk_icon_set_color()

static gboolean press_day_cb(ClutterActor* act, ClutterEvent* evt, gpointer data)
{
	g_message("%s:%s", G_STRLOC, G_STRFUNC);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(act), TRUE);
	private_data_t*  priv = (private_data_t*)g_object_get_data(G_OBJECT(act), "priv");
	int row_idx = *((int*)g_object_get_data(G_OBJECT(act), "row-idx"));
	int col_idx = *((int*)g_object_get_data(G_OBJECT(act), "col-idx"));
	int year  = priv->year;
	int month = priv->month;
	gboolean in_month;
	int day;
	calendar_number(year, month, col_idx, row_idx, &day, &in_month);

	/* TODO: changing pointer_row and pointer_col might be in specific condition */
	priv->pointer_row = row_idx;
	priv->pointer_col = col_idx;
	
	//g_message("day=%d, in_month=%d, pointer_row=%d, pointer_col=%d\n", day, in_month, priv->pointer_row, priv->pointer_col);



	return TRUE;
} //press_day_cb()



static private_data_t* private_data_new0(void)
{
	private_data_t* priv = g_new0(private_data_t,1);
	
	return priv;
} //private_data_new0()


static private_data_free(private_data_t* priv)
{
	g_free(priv);
} //private_data_free()



static date_info_t* get_date_info(int year, int month, int day)
{
	static int idx=0;
	idx++;
	date_info_t* info = date_info_new0();
	info->status = idx % 3;
	return info;
} //get_date_info()



static date_info_t* date_info_new0(void)
{
	date_info_t* info=NULL;
	info = g_new0(date_info_t,1);
} //date_info_new0()



static void date_info_free(date_info_t* info)
{
	g_free(info);
} //date_info_free()







static void calendar_number
(int year, int month, int pos_x, int pos_y, int* num, gboolean* is_in_this_month)
{
	g_return_if_fail(pos_x>=0 && pos_x < 7);
	//g_return_if_fail(pos_y>=0 && pos_y < 5);
	g_return_if_fail(pos_y>=0 && pos_y < 6);
#if 0
	g_debug("year = %d, month = %d\n", year, month);
#endif
	int prev_month_ddd;
	int this_month_ddd;
	int next_month_ddd;
	int first_row_gray_days; /*The number of day is not belong to this month on first row*/
	int last_row_gray_days; /* The number of day is not belong to this month on last row */
	int prev_biggest_day; /* The biggest day for prev month */
	int biggest_day; /* The biggest day for this month */

	/* To figure out ddd of first day in this month  (ddd is Sunday, Monday, ...,  Saturday*/
	this_month_ddd = get_weekday(year, month, 1);
	if(month < 12)
	{
		next_month_ddd = get_weekday(year, month+1, 1);
	}
	else
	{
		next_month_ddd = get_weekday(year+1, 1, 1);
	}
	if(month == 1)
	{
		prev_month_ddd = get_weekday(year-1, 12, 1);
	}
	else
	{
		prev_month_ddd = get_weekday(year, month-1, 1);
	}

	first_row_gray_days = this_month_ddd;
	last_row_gray_days = (7-next_month_ddd)%7;
	if(month == 2 && first_row_gray_days == 0)
	{
		last_row_gray_days = 7;
	}
	biggest_day = 35 - (first_row_gray_days + last_row_gray_days);
	prev_biggest_day = 35 - (prev_month_ddd + (7-(first_row_gray_days)) );
	if(prev_biggest_day < 28)
	{
		prev_biggest_day+=7;
	}


	(*num) = (((pos_y+1)*7) - first_row_gray_days ) - (6-pos_x);

	if((*num)<=0)
	{
		(*num) = prev_biggest_day + (*num);
		*is_in_this_month = FALSE;
	} 
	else
	{
		if( (*num) > biggest_day )
		{
			*is_in_this_month = FALSE;
			(*num) = (*num)- biggest_day;
		}
		else
		{
			*is_in_this_month = TRUE;
		}
	}
} //calendar_number()





/*ddd is Sunday, Monday, ...,  Saturday*/
static int get_weekday(int year, int month, int day) 
{ 
  GDate* gdate = g_date_new();
  g_date_set_day(gdate,day);
  g_date_set_month(gdate,month);
  g_date_set_year(gdate,year);
  int weekday = g_date_get_weekday(gdate);
  weekday %= 7;
  g_date_free(gdate);
  return weekday;
} //int get_weekday




static void marker_reached_cb(ClutterTimeline* timeline, gchar* marker_name, gint msecs, gpointer data)
{
	int num=0;
	GList* list = (GList*)(data);
	while(list!=NULL)
	{
		ClutterActor* icon;
		ClutterActor* rect;
		ClutterActor* text; 
		ClutterColor* fg_color;
		ClutterColor* bg_color;
		ClutterColor* gray_fg_color;
		ClutterColor* gray_bg_color;
		ClutterColor* gray_bd_color;
		int row_idx=0;
		int col_idx=0;
		int* p_row_idx=NULL;
		int* p_col_idx=NULL;
		int year;
		int month;
		private_data_t* priv;
		

		
		num++;
		icon = list->data;
		g_return_if_fail(icon!=NULL);
		g_return_if_fail(CLUTTER_IS_ACTOR(icon));
		p_row_idx = (int*)g_object_get_data(G_OBJECT(icon), "row-idx" );
		p_col_idx = (int*)g_object_get_data(G_OBJECT(icon), "col-idx" );
		g_return_if_fail(p_row_idx != NULL);
		g_return_if_fail(p_col_idx != NULL);
		row_idx = *p_row_idx;
		col_idx = *p_col_idx;

		rect = (ClutterActor*)g_object_get_data(G_OBJECT(icon), "rect");
		text = (ClutterActor*)g_object_get_data(G_OBJECT(icon), "text");
		g_return_if_fail(rect!=NULL);
		g_return_if_fail(text!=NULL);
		g_return_if_fail(CLUTTER_IS_RECTANGLE(rect));
		g_return_if_fail(CLUTTER_IS_TEXT(text));

		priv = (private_data_t*)g_object_get_data(G_OBJECT(icon),"priv");
		g_return_if_fail(priv!=NULL);
		year = priv->year;
		month = priv->month;

		fg_color = clutter_color_new(0x22, 0x22, 0x22, 0xff);
		bg_color = clutter_color_new(0xaf, 0xaf, 0xaf, 0xff);
		gray_fg_color = clutter_color_new(0xd8,0xd8,0xd8,0xff);
		gray_bd_color = clutter_color_new(0xd8,0xd8,0xd8,0xff);
		gray_bg_color = clutter_color_new(0xe8,0xe8,0xe8,0xff);

		
		calblk_icon_set_color(icon, fg_color, bg_color);
		gboolean in_month;
		int day;
		calendar_number( year,month, col_idx, row_idx, &day, &in_month);
		if(in_month == FALSE)
		{
			calblk_icon_set_color(icon, gray_fg_color, gray_bg_color);
			ClutterActor* rect = (ClutterActor*) g_object_get_data(G_OBJECT(icon), "rect");
			if(CLUTTER_IS_RECTANGLE(rect))
			{
				clutter_rectangle_set_border_width(CLUTTER_RECTANGLE(rect),1);
				clutter_rectangle_set_border_color(CLUTTER_RECTANGLE(rect), gray_bd_color);
			}
		}

		
		calblk_icon_set_number(icon, day);


		gfloat width;
		gfloat height;
		clutter_actor_get_size(text, &width, &height);
		clutter_actor_set_rotation(text, CLUTTER_Y_AXIS, 180.0, width/2, height/2, 0 );


		clutter_color_free(gray_fg_color);
		clutter_color_free(gray_bg_color);
		clutter_color_free(fg_color);
		clutter_color_free(bg_color);



	

		
		list=list->next;	
	}

	//g_debug("%s:%s:num=%d", G_STRLOC, G_STRFUNC, num);
	

} //marker_reached_cb()




static void timeline_started_cb (ClutterTimeline* timeline, gpointer data)
{
	GList* actor_list;

	actor_list = (GList*) g_object_get_data(G_OBJECT(timeline), "actor-list");
	g_return_if_fail(actor_list != NULL);
	
	while(actor_list != NULL)
	{
		ClutterActor* icon;
		ClutterActor* text;
		icon = actor_list->data;
		g_return_if_fail(icon != NULL);
		g_return_if_fail(CLUTTER_IS_ACTOR(icon));
		text =(ClutterActor*) g_object_get_data(G_OBJECT(icon), "text");
		g_return_if_fail(text != NULL);
		g_return_if_fail(CLUTTER_IS_ACTOR(text));

		gfloat width;
		gfloat height;
		clutter_actor_get_size(text, &width, &height);
		clutter_actor_set_rotation(text, CLUTTER_Y_AXIS, 0.0, width/2, height/2, 0 );

		actor_list = actor_list->next;
	} //while(actor_list != NULL)
} //timeline_start_cb()



static void timeline_completed_cb (ClutterTimeline* timeline, gpointer data)
{
	//g_debug("%s:%s", G_STRLOC, G_STRFUNC);
	
	
	GList* actor_list;

	actor_list = (GList*) g_object_get_data(G_OBJECT(timeline), "actor-list");
	g_return_if_fail(actor_list != NULL);
	
	while(actor_list != NULL)
	{
		ClutterActor* icon;
		ClutterActor* text;
		icon = actor_list->data;
		g_return_if_fail(icon != NULL);
		g_return_if_fail(CLUTTER_IS_ACTOR(icon));
		text =(ClutterActor*) g_object_get_data(G_OBJECT(icon), "text");
		g_return_if_fail(text != NULL);
		g_return_if_fail(CLUTTER_IS_ACTOR(text));

		gfloat width;
		gfloat height;
		clutter_actor_get_size(icon, &width, &height);
		clutter_actor_set_rotation(icon, CLUTTER_Y_AXIS, 0.0, width/2, height/2, 0 );
		clutter_actor_get_size(text, &width, &height);
		clutter_actor_set_rotation(text, CLUTTER_Y_AXIS, 0.0, width/2, height/2, 0 );

	

		actor_list = actor_list->next;
	} //while(actor_list != NULL)
} //timeline_start_cb()







static void last_timeline_completed_cb (ClutterTimeline* timeline, gpointer data)
{

	ClutterActor*  calblk;
	
	calblk = *((ClutterActor**)data);
	g_return_if_fail(calblk!=NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(calblk));
	
	calblk_refresh_data(calblk);
	
} //last_timeline_start_cb()








static ClutterActor* create_right_arrow(void)
{
	
	ClutterActor* arrow_group;
	ClutterActor* arrow_fg_pic;
	ClutterActor* arrow_bg_pic;
	ClutterScore* score;
	ClutterScore* pressed_score;

	gfloat parent_width;
	gfloat parent_height;
	gfloat width;
	gfloat height;
	gfloat pos_x;
	gfloat pos_y;

	const char* filepath = ARROW_RIGHT_FILEPATH;
	GError* gerr = NULL;
	arrow_fg_pic = clutter_texture_new_from_file(filepath ,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	arrow_bg_pic = clutter_texture_new_from_file(filepath ,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);

	arrow_group =  clutter_group_new();
	clutter_actor_set_size(arrow_group, 30,30);

	clutter_actor_set_opacity(arrow_fg_pic, 0xff);
	clutter_actor_set_opacity(arrow_bg_pic, 0x00);

	clutter_actor_get_size(arrow_group, &parent_width, &parent_height);
	clutter_actor_get_size(arrow_fg_pic, &width, &height);

	clutter_actor_set_position(arrow_fg_pic, 
			(parent_width - width)/2,
			(parent_height - height)/2
	);

	clutter_actor_get_position(arrow_fg_pic, &pos_x, &pos_y);
	clutter_actor_set_position(arrow_bg_pic, pos_x-width/2, pos_y);

	
	score = clutter_score_new();
	pressed_score = clutter_score_new();

	clutter_actor_set_name(arrow_fg_pic, "arrow_fg_pic");
	clutter_actor_set_name(arrow_bg_pic, "arrow_bg_pic");

	gfloat* orig_pos_x;
	gfloat* orig_pos_y; 
	orig_pos_x = g_new0(gfloat,1);
	orig_pos_y = g_new0(gfloat,1);
	clutter_actor_get_position(arrow_fg_pic, orig_pos_x, orig_pos_y);
	g_object_set_data(G_OBJECT(arrow_fg_pic), "orig_pos_x", orig_pos_x);
	g_object_set_data(G_OBJECT(arrow_fg_pic), "orig_pos_y", orig_pos_y);

	orig_pos_x = g_new0(gfloat,1);
	orig_pos_y = g_new0(gfloat,1);
	clutter_actor_get_position(arrow_bg_pic, orig_pos_x, orig_pos_y);
	g_object_set_data(G_OBJECT(arrow_bg_pic), "orig_pos_x", orig_pos_x);
	g_object_set_data(G_OBJECT(arrow_bg_pic), "orig_pos_y", orig_pos_y);
	
	g_object_set_data(G_OBJECT(arrow_group), "score", score);
	g_object_set_data(G_OBJECT(arrow_group), "pressed_score", pressed_score);

	clutter_actor_set_reactive(arrow_group,TRUE);


	g_signal_connect(G_OBJECT(arrow_group), "enter-event",G_CALLBACK(right_arrow_enter_cb),NULL);
	g_signal_connect(G_OBJECT(arrow_group), "leave-event",G_CALLBACK(right_arrow_leave_cb),NULL);
	g_signal_connect(G_OBJECT(arrow_group), "button-press-event",G_CALLBACK(right_arrow_press_cb),NULL);
	g_signal_connect(G_OBJECT(arrow_group), "button-release-event",G_CALLBACK(right_arrow_release_cb),NULL);
	g_signal_connect(G_OBJECT(pressed_score), "completed", G_CALLBACK(right_arrow_pressed_effect_completed_cb),arrow_group);



	clutter_container_add(CLUTTER_CONTAINER(arrow_group), arrow_bg_pic, arrow_fg_pic, NULL);
	right_arrow_effect_apply(arrow_group);
	clutter_actor_show_all(arrow_group);
	return arrow_group;

} //create_right_arrow()




static ClutterActor* create_left_arrow(void)
{
	
	ClutterActor* arrow_group;
	ClutterActor* arrow_fg_pic;
	ClutterActor* arrow_bg_pic;
	ClutterScore* score;
	ClutterScore* pressed_score;

	gfloat parent_width;
	gfloat parent_height;
	gfloat width;
	gfloat height;
	gfloat pos_x;
	gfloat pos_y;

	const char* filepath = ARROW_LEFT_FILEPATH;
	GError* gerr = NULL;
	arrow_fg_pic = clutter_texture_new_from_file(filepath ,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	arrow_bg_pic = clutter_texture_new_from_file(filepath ,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);

	arrow_group =  clutter_group_new();
	clutter_actor_set_size(arrow_group, 30,30);

	clutter_actor_set_opacity(arrow_fg_pic, 0xff);
	clutter_actor_set_opacity(arrow_bg_pic, 0x00);

	clutter_actor_get_size(arrow_group, &parent_width, &parent_height);
	clutter_actor_get_size(arrow_fg_pic, &width, &height);

	clutter_actor_set_position(arrow_fg_pic, 
			(parent_width - width)/2,
			(parent_height - height)/2
	);

	clutter_actor_get_position(arrow_fg_pic, &pos_x, &pos_y);
	clutter_actor_set_position(arrow_bg_pic, pos_x+width/2, pos_y);
	

	score = clutter_score_new();
	pressed_score = clutter_score_new();

	clutter_actor_set_name(arrow_fg_pic, "arrow_fg_pic");
	clutter_actor_set_name(arrow_bg_pic, "arrow_bg_pic");

	gfloat* orig_pos_x;
	gfloat* orig_pos_y; 
	orig_pos_x = g_new0(gfloat,1);
	orig_pos_y = g_new0(gfloat,1);
	clutter_actor_get_position(arrow_fg_pic, orig_pos_x, orig_pos_y);
	g_object_set_data(G_OBJECT(arrow_fg_pic), "orig_pos_x", orig_pos_x);
	g_object_set_data(G_OBJECT(arrow_fg_pic), "orig_pos_y", orig_pos_y);

	orig_pos_x = g_new0(gfloat,1);
	orig_pos_y = g_new0(gfloat,1);
	clutter_actor_get_position(arrow_bg_pic, orig_pos_x, orig_pos_y);
	g_object_set_data(G_OBJECT(arrow_bg_pic), "orig_pos_x", orig_pos_x);
	g_object_set_data(G_OBJECT(arrow_bg_pic), "orig_pos_y", orig_pos_y);
	
	g_object_set_data(G_OBJECT(arrow_group), "score", score);
	g_object_set_data(G_OBJECT(arrow_group), "pressed_score", pressed_score);

	clutter_actor_set_reactive(arrow_group,TRUE);


	g_signal_connect(G_OBJECT(arrow_group), "enter-event",G_CALLBACK(left_arrow_enter_cb),NULL);
	g_signal_connect(G_OBJECT(arrow_group), "leave-event",G_CALLBACK(left_arrow_leave_cb),NULL);
	g_signal_connect(G_OBJECT(arrow_group), "button-press-event",G_CALLBACK(left_arrow_press_cb),NULL);
	g_signal_connect(G_OBJECT(arrow_group), "button-release-event",G_CALLBACK(left_arrow_release_cb),NULL);
	g_signal_connect(G_OBJECT(pressed_score), "completed", G_CALLBACK(left_arrow_pressed_effect_completed_cb),arrow_group);


	clutter_container_add(CLUTTER_CONTAINER(arrow_group), arrow_bg_pic, arrow_fg_pic, NULL);
	left_arrow_effect_apply(arrow_group);
	clutter_actor_show_all(arrow_group);
	return arrow_group;

} //create_left_arrow()





static void left_arrow_effect_apply(ClutterActor* arrow_group)
{
	g_return_if_fail(arrow_group != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_group));

	gfloat* fg_orig_pos_x;
	gfloat* fg_orig_pos_y;
	gfloat* bg_orig_pos_x;
	gfloat* bg_orig_pos_y;
	ClutterScore* score;
	ClutterScore* pressed_score;
	ClutterActor* arrow_fg_pic;
	ClutterActor* arrow_bg_pic;

	score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "score");
	pressed_score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "pressed_score");
	arrow_fg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_fg_pic");
	arrow_bg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_bg_pic");
	g_return_if_fail(score!=NULL);
	g_return_if_fail(pressed_score!=NULL);
	g_return_if_fail(arrow_fg_pic!=NULL);
	g_return_if_fail(arrow_bg_pic!=NULL);
	g_return_if_fail(CLUTTER_IS_SCORE(score));
	g_return_if_fail(CLUTTER_IS_SCORE(pressed_score));
	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_fg_pic));
	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_bg_pic));

	fg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_x");
	fg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_y");
	bg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_x");
	bg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_y");

	g_return_if_fail(fg_orig_pos_x !=NULL);
	g_return_if_fail(fg_orig_pos_y !=NULL);
	g_return_if_fail(bg_orig_pos_x !=NULL);
	g_return_if_fail(bg_orig_pos_y !=NULL);


	gfloat width;
	gfloat height;

 	const int step_time = 500;

	ClutterTimeline* tl1 = clutter_timeline_new(step_time);


	ClutterAlpha* al1 = clutter_alpha_new_full(tl1, CLUTTER_LINEAR);

	
	ClutterPath* fgPath1 = clutter_path_new();
	clutter_path_add_move_to(fgPath1, *fg_orig_pos_x, *fg_orig_pos_y);
	clutter_path_add_rel_line_to(fgPath1, -1*(*bg_orig_pos_x - *fg_orig_pos_x), 0);

	ClutterPath* bgPath1 = clutter_path_new();
	clutter_path_add_move_to(bgPath1, *bg_orig_pos_x, *bg_orig_pos_y);
	clutter_path_add_rel_line_to(bgPath1, -1*(*bg_orig_pos_x - *fg_orig_pos_x), 0);

	ClutterBehaviour* fgP1 = clutter_behaviour_path_new(al1, fgPath1);
	ClutterBehaviour* bgP1 = clutter_behaviour_path_new(al1, bgPath1);
	ClutterBehaviour* fgO1 = clutter_behaviour_opacity_new(al1, 0xff, 0x00);
	ClutterBehaviour* bgO1 = clutter_behaviour_opacity_new(al1, 0x00, 0xff);
	ClutterBehaviour* fgSG1 = clutter_behaviour_scale_gravity_new(al1, CLUTTER_GRAVITY_CENTER, 1.00,1.00, 0.50, 0.50);
	ClutterBehaviour* bgSG1 = clutter_behaviour_scale_gravity_new(al1, CLUTTER_GRAVITY_CENTER, 0.50,0.50, 1.00,1.00 );

	clutter_behaviour_apply(fgP1, arrow_fg_pic);
	clutter_behaviour_apply(bgP1, arrow_bg_pic);
	clutter_behaviour_apply(fgO1, arrow_fg_pic);
	clutter_behaviour_apply(bgO1, arrow_bg_pic);
	clutter_behaviour_apply(fgSG1, arrow_fg_pic);
	clutter_behaviour_apply(bgSG1, arrow_bg_pic);

	clutter_score_append(score, NULL, tl1);
	clutter_score_set_loop(score, TRUE);

	
	/* Setting Pressed Score */
	if(1)
	{
		
		ClutterTimeline* tl1 = clutter_timeline_new(30);
		ClutterTimeline* tl2 = clutter_timeline_new(4*step_time);
		
		ClutterAlpha* al1 = clutter_alpha_new_full(tl1, CLUTTER_EASE_OUT_ELASTIC);
		ClutterAlpha* al2 = clutter_alpha_new_full(tl2, CLUTTER_EASE_OUT_ELASTIC);
		ClutterAlpha *alO2 = clutter_alpha_new_full(tl2, CLUTTER_EASE_OUT_EXPO);
		ClutterBehaviour* fgSG1 = clutter_behaviour_scale_gravity_new(al1, CLUTTER_GRAVITY_CENTER, 2.0, 2.00, 2.00, 2.00);

		ClutterBehaviour* fgSG2 = clutter_behaviour_scale_gravity_new(al2, CLUTTER_GRAVITY_CENTER, 2.00, 2.00, 0.80, 0.80);

		ClutterBehaviour* fgO2 = clutter_behaviour_opacity_new(alO2, 0x8f, 0xff);

		clutter_behaviour_apply(fgSG1, arrow_fg_pic);
		clutter_behaviour_apply(fgSG2, arrow_fg_pic);
		clutter_behaviour_apply(fgO2, arrow_fg_pic);
#if 0
		clutter_score_append( pressed_score, NULL, tl1);
		clutter_score_append( pressed_score, tl1, tl2);
#else
		clutter_score_append( pressed_score, NULL, tl2);
#endif

		clutter_score_set_loop(pressed_score, FALSE);
	} //if(1) /* Setting Pressed Score */

//	clutter_score_start(score);

} //left_arrow_effect_apply()







static void right_arrow_effect_apply(ClutterActor* arrow_group)
{
	g_return_if_fail(arrow_group != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_group));

	gfloat* fg_orig_pos_x;
	gfloat* fg_orig_pos_y;
	gfloat* bg_orig_pos_x;
	gfloat* bg_orig_pos_y;
	ClutterScore* score;
	ClutterScore* pressed_score;
	ClutterActor* arrow_fg_pic;
	ClutterActor* arrow_bg_pic;

	score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "score");
	pressed_score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "pressed_score");
	arrow_fg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_fg_pic");
	arrow_bg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_bg_pic");

	g_return_if_fail(score!=NULL);
	g_return_if_fail(pressed_score!=NULL);
	g_return_if_fail(arrow_fg_pic!=NULL);
	g_return_if_fail(arrow_bg_pic!=NULL);
	g_return_if_fail(CLUTTER_IS_SCORE(score));
	g_return_if_fail(CLUTTER_IS_SCORE(pressed_score));
	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_fg_pic));
	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_bg_pic));

	fg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_x");
	fg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_y");
	bg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_x");
	bg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_y");

	g_return_if_fail(fg_orig_pos_x !=NULL);
	g_return_if_fail(fg_orig_pos_y !=NULL);
	g_return_if_fail(bg_orig_pos_x !=NULL);
	g_return_if_fail(bg_orig_pos_y !=NULL);


	gfloat width;
	gfloat height;


 	const int step_time = 500;

	ClutterTimeline* tl1 = clutter_timeline_new(step_time);


	ClutterAlpha* al1 = clutter_alpha_new_full(tl1, CLUTTER_LINEAR);

	
	ClutterPath* fgPath1 = clutter_path_new();
	clutter_path_add_move_to(fgPath1, *fg_orig_pos_x, *fg_orig_pos_y);
	clutter_path_add_rel_line_to(fgPath1, -1*(*bg_orig_pos_x - *fg_orig_pos_x), 0);

	ClutterPath* bgPath1 = clutter_path_new();
	clutter_path_add_move_to(bgPath1, *bg_orig_pos_x, *bg_orig_pos_y);
	clutter_path_add_rel_line_to(bgPath1, -1*(*bg_orig_pos_x - *fg_orig_pos_x), 0);

	ClutterBehaviour* fgP1 = clutter_behaviour_path_new(al1, fgPath1);
	ClutterBehaviour* bgP1 = clutter_behaviour_path_new(al1, bgPath1);
	ClutterBehaviour* fgO1 = clutter_behaviour_opacity_new(al1, 0xff, 0x00);
	ClutterBehaviour* bgO1 = clutter_behaviour_opacity_new(al1, 0x00, 0xff);
	ClutterBehaviour* fgSG1 = clutter_behaviour_scale_gravity_new(al1, CLUTTER_GRAVITY_CENTER, 1.00,1.00, 0.50, 0.50);
	ClutterBehaviour* bgSG1 = clutter_behaviour_scale_gravity_new(al1, CLUTTER_GRAVITY_CENTER, 0.50,0.50, 1.00,1.00 );

	clutter_behaviour_apply(fgP1, arrow_fg_pic);
	clutter_behaviour_apply(bgP1, arrow_bg_pic);
	clutter_behaviour_apply(fgO1, arrow_fg_pic);
	clutter_behaviour_apply(bgO1, arrow_bg_pic);
	clutter_behaviour_apply(fgSG1, arrow_fg_pic);
	clutter_behaviour_apply(bgSG1, arrow_bg_pic);

	clutter_score_append(score, NULL, tl1);
	clutter_score_set_loop(score, TRUE);


	/* Setting Pressed Score */
	if(1)
	{
		
		ClutterTimeline* tl1 = clutter_timeline_new(30);
		ClutterTimeline* tl2 = clutter_timeline_new(4*step_time);
		
		ClutterAlpha* al1 = clutter_alpha_new_full(tl1, CLUTTER_EASE_OUT_ELASTIC);
		ClutterAlpha* al2 = clutter_alpha_new_full(tl2, CLUTTER_EASE_OUT_ELASTIC);
		ClutterAlpha *alO2 = clutter_alpha_new_full(tl2, CLUTTER_EASE_OUT_EXPO);
		ClutterBehaviour* fgSG1 = clutter_behaviour_scale_gravity_new(al1, CLUTTER_GRAVITY_CENTER, 2.0, 2.00, 2.00, 2.00);

		ClutterBehaviour* fgSG2 = clutter_behaviour_scale_gravity_new(al2, CLUTTER_GRAVITY_CENTER, 2.00, 2.00, 0.80, 0.80);

		ClutterBehaviour* fgO2 = clutter_behaviour_opacity_new(alO2, 0x8f, 0xff);

		clutter_behaviour_apply(fgSG1, arrow_fg_pic);
		clutter_behaviour_apply(fgSG2, arrow_fg_pic);
		clutter_behaviour_apply(fgO2, arrow_fg_pic);
#if 0
		clutter_score_append( pressed_score, NULL, tl1);
		clutter_score_append( pressed_score, tl1, tl2);
#else
		clutter_score_append( pressed_score, NULL, tl2);
#endif

		clutter_score_set_loop(pressed_score, FALSE);
	} //if(1) /* Setting Pressed Score */



} //right_arrow_effect_apply()}



static gboolean right_arrow_enter_cb(ClutterActor* arrow_group, ClutterEvent* evt, gpointer data)
{
	g_return_val_if_fail(arrow_group != NULL, TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(arrow_group), TRUE);

	ClutterScore* score = (ClutterScore*) g_object_get_data(G_OBJECT(arrow_group),"score");
	g_return_val_if_fail(CLUTTER_IS_SCORE(score), TRUE);
	clutter_score_stop(score);
	clutter_score_start(score);
} //right_arrow_enter_cb()


static gboolean right_arrow_leave_cb(ClutterActor* arrow_group, ClutterEvent* evt, gpointer data)
{
	g_return_val_if_fail(arrow_group != NULL,TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(arrow_group),TRUE);


	gfloat* fg_orig_pos_x;
	gfloat* fg_orig_pos_y;
	gfloat* bg_orig_pos_x;
	gfloat* bg_orig_pos_y;
	ClutterScore* score;
	ClutterActor* arrow_fg_pic;
	ClutterActor* arrow_bg_pic;

	score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "score");
	g_return_if_fail(score!=NULL);
	g_return_if_fail(CLUTTER_IS_SCORE(score));
	clutter_score_stop(score);


	arrow_fg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_fg_pic");
	arrow_bg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_bg_pic");


	g_return_if_fail(arrow_fg_pic!=NULL);
	g_return_if_fail(arrow_bg_pic!=NULL);



	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_fg_pic));

	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_bg_pic));

	fg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_x");
	fg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_y");
	bg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_x");
	bg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_y");

	g_return_if_fail(fg_orig_pos_x !=NULL);
	g_return_if_fail(fg_orig_pos_y !=NULL);
	g_return_if_fail(bg_orig_pos_x !=NULL);
	g_return_if_fail(bg_orig_pos_y !=NULL);


	clutter_actor_set_position(arrow_fg_pic, *fg_orig_pos_x, *fg_orig_pos_y );
	clutter_actor_set_position(arrow_bg_pic, *bg_orig_pos_x, *bg_orig_pos_y );
	clutter_actor_set_opacity(arrow_fg_pic,0xff);
	clutter_actor_set_opacity(arrow_bg_pic,0x00);
	clutter_actor_set_scale_with_gravity( arrow_fg_pic, 1.00, 1.00, CLUTTER_GRAVITY_CENTER);
	clutter_actor_set_scale_with_gravity( arrow_bg_pic, 0.50, 0.50, CLUTTER_GRAVITY_CENTER);

} //right_arrow_leave_cb()


static gboolean left_arrow_enter_cb(ClutterActor* arrow_group, ClutterEvent* evt, gpointer data)
{
	g_return_val_if_fail(arrow_group != NULL, TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(arrow_group), TRUE);

	ClutterScore* score = (ClutterScore*) g_object_get_data(G_OBJECT(arrow_group),"score");
	g_return_val_if_fail(CLUTTER_IS_SCORE(score), TRUE);
	clutter_score_stop(score);
	clutter_score_start(score);

} //left_arrow_enter_cb()

static gboolean left_arrow_leave_cb(ClutterActor* arrow_group, ClutterEvent* evt, gpointer data)
{

	g_return_val_if_fail(arrow_group != NULL,TRUE);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(arrow_group),TRUE);

	gfloat* fg_orig_pos_x;
	gfloat* fg_orig_pos_y;
	gfloat* bg_orig_pos_x;
	gfloat* bg_orig_pos_y;
	ClutterScore* score;
	ClutterScore* pressed_score;
	ClutterActor* arrow_fg_pic;
	ClutterActor* arrow_bg_pic;

	score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "score");
	g_return_if_fail(score!=NULL);
	g_return_if_fail(CLUTTER_IS_SCORE(score));
	clutter_score_stop(score);


	pressed_score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "pressed_score");
	g_return_if_fail(pressed_score!=NULL);
	g_return_if_fail(CLUTTER_IS_SCORE(pressed_score));
	clutter_score_stop(pressed_score);


	arrow_fg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_fg_pic");
	arrow_bg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_bg_pic");

	g_return_if_fail(arrow_fg_pic!=NULL);
	g_return_if_fail(arrow_bg_pic!=NULL);

	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_fg_pic));
	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_bg_pic));

	fg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_x");
	fg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_y");
	bg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_x");
	bg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_y");

	g_return_if_fail(fg_orig_pos_x !=NULL);
	g_return_if_fail(fg_orig_pos_y !=NULL);
	g_return_if_fail(bg_orig_pos_x !=NULL);
	g_return_if_fail(bg_orig_pos_y !=NULL);

	clutter_actor_set_position(arrow_fg_pic, *fg_orig_pos_x, *fg_orig_pos_y );
	clutter_actor_set_position(arrow_bg_pic, *bg_orig_pos_x, *bg_orig_pos_y );
	clutter_actor_set_opacity(arrow_fg_pic,0xff);
	clutter_actor_set_opacity(arrow_bg_pic,0x00);
	clutter_actor_set_scale_with_gravity( arrow_fg_pic, 1.00, 1.00, CLUTTER_GRAVITY_CENTER);
	clutter_actor_set_scale_with_gravity( arrow_bg_pic, 0.50, 0.50, CLUTTER_GRAVITY_CENTER);

} //left_arrow_leave_cb()



static gboolean left_arrow_press_cb(ClutterActor* arrow_group, ClutterEvent* evt, gpointer data)
{
	gboolean ret = FALSE;
	g_return_val_if_fail(arrow_group != NULL,ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(arrow_group),ret);

	gfloat* fg_orig_pos_x;
	gfloat* fg_orig_pos_y;
	gfloat* bg_orig_pos_x;
	gfloat* bg_orig_pos_y;
	ClutterScore* score;
	ClutterActor* arrow_fg_pic;
	ClutterActor* arrow_bg_pic;

	score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "score");
	g_return_if_fail(score!=NULL);
	g_return_if_fail(CLUTTER_IS_SCORE(score));
	clutter_score_stop(score);

	arrow_fg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_fg_pic");
	arrow_bg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_bg_pic");

	g_return_if_fail(arrow_fg_pic!=NULL);
	g_return_if_fail(arrow_bg_pic!=NULL);

	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_fg_pic));
	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_bg_pic));

	fg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_x");
	fg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_y");
	bg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_x");
	bg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_y");

	g_return_if_fail(fg_orig_pos_x !=NULL);
	g_return_if_fail(fg_orig_pos_y !=NULL);
	g_return_if_fail(bg_orig_pos_x !=NULL);
	g_return_if_fail(bg_orig_pos_y !=NULL);

	clutter_actor_set_position(arrow_fg_pic, *fg_orig_pos_x, *fg_orig_pos_y );
	clutter_actor_set_position(arrow_bg_pic, *bg_orig_pos_x, *bg_orig_pos_y );
	clutter_actor_set_opacity(arrow_fg_pic,0xff);
	clutter_actor_set_opacity(arrow_bg_pic,0x00);
	clutter_actor_set_scale_with_gravity( arrow_fg_pic, 1.00, 1.00, CLUTTER_GRAVITY_CENTER);
	clutter_actor_set_scale_with_gravity( arrow_bg_pic, 0.50, 0.50, CLUTTER_GRAVITY_CENTER);


	ClutterScore* pressed_score = (ClutterScore*) g_object_get_data(G_OBJECT(arrow_group),"pressed_score");
	g_return_val_if_fail(CLUTTER_IS_SCORE(pressed_score), ret);
	clutter_score_stop(pressed_score);
	clutter_score_start(pressed_score);

	return ret; //return FALSE and the press-button-event signal on the same button will processed.

} //left_arrow_press_cb()

static gboolean left_arrow_release_cb(ClutterActor* arrow_group, ClutterEvent* evt, gpointer data)
{
	gboolean ret = FALSE;
	ClutterScore* score;
	ClutterScore* pressed_score;

	pressed_score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group),"pressed_score");
	g_return_val_if_fail(pressed_score != NULL,ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(pressed_score), ret);
	
	score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "score");
	g_return_val_if_fail(score != NULL,ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(score), ret);

	clutter_score_stop(pressed_score);
	clutter_score_start(score);
	return ret;
} //left_arrow_release()

static gboolean left_arrow_pressed_effect_completed_cb(ClutterScore* pressed_score, gpointer data)
{
	gboolean ret = FALSE;
	ClutterActor* arrow_group;
	ClutterScore* score;

	arrow_group = (ClutterActor*)data;
	g_return_val_if_fail(pressed_score != NULL,ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(pressed_score), ret);
	
	score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "score");
	g_return_val_if_fail(score != NULL,ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(score), ret);

	clutter_score_stop(pressed_score);
	g_debug("ABCDE");
	clutter_score_start(score);
	return ret;
} //left_arrow_pressed_effect_completed_cb




static gboolean right_arrow_release_cb(ClutterActor* arrow_group, ClutterEvent* evt, gpointer data)
{
	gboolean ret = FALSE;
	ClutterScore* score;
	ClutterScore* pressed_score;

	pressed_score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group),"pressed_score");
	g_return_val_if_fail(pressed_score != NULL,ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(pressed_score), ret);
	
	score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "score");
	g_return_val_if_fail(score != NULL,ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(score), ret);

	clutter_score_stop(pressed_score);
	clutter_score_start(score);
	return ret;
} //right_arrow_release()

static gboolean right_arrow_pressed_effect_completed_cb(ClutterScore* pressed_score, gpointer data)
{
	gboolean ret = FALSE;
	ClutterActor* arrow_group;
	ClutterScore* score;

	arrow_group = (ClutterActor*)data;
	g_return_val_if_fail(pressed_score != NULL,ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(pressed_score), ret);
	
	score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "score");
	g_return_val_if_fail(score != NULL,ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(score), ret);

	clutter_score_stop(pressed_score);
	g_debug("ABCDE");
	clutter_score_start(score);
	return ret;
} //right_arrow_pressed_effect_completed_cb





static gboolean right_arrow_press_cb(ClutterActor* arrow_group, ClutterEvent* evt, gpointer data)
{
	gboolean ret = FALSE;
	g_return_val_if_fail(arrow_group != NULL,ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(arrow_group),ret);

	gfloat* fg_orig_pos_x;
	gfloat* fg_orig_pos_y;
	gfloat* bg_orig_pos_x;
	gfloat* bg_orig_pos_y;
	ClutterScore* score;
	ClutterActor* arrow_fg_pic;
	ClutterActor* arrow_bg_pic;

	score = (ClutterScore*)g_object_get_data(G_OBJECT(arrow_group), "score");
	g_return_if_fail(score!=NULL);
	g_return_if_fail(CLUTTER_IS_SCORE(score));
	clutter_score_stop(score);

	arrow_fg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_fg_pic");
	arrow_bg_pic = clutter_container_find_child_by_name(CLUTTER_CONTAINER (arrow_group), "arrow_bg_pic");

	g_return_if_fail(arrow_fg_pic!=NULL);
	g_return_if_fail(arrow_bg_pic!=NULL);

	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_fg_pic));
	g_return_if_fail(CLUTTER_IS_ACTOR(arrow_bg_pic));

	fg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_x");
	fg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_fg_pic), "orig_pos_y");
	bg_orig_pos_x = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_x");
	bg_orig_pos_y = (gfloat*)g_object_get_data(G_OBJECT(arrow_bg_pic), "orig_pos_y");

	g_return_if_fail(fg_orig_pos_x !=NULL);
	g_return_if_fail(fg_orig_pos_y !=NULL);
	g_return_if_fail(bg_orig_pos_x !=NULL);
	g_return_if_fail(bg_orig_pos_y !=NULL);

	clutter_actor_set_position(arrow_fg_pic, *fg_orig_pos_x, *fg_orig_pos_y );
	clutter_actor_set_position(arrow_bg_pic, *bg_orig_pos_x, *bg_orig_pos_y );
	clutter_actor_set_opacity(arrow_fg_pic,0xff);
	clutter_actor_set_opacity(arrow_bg_pic,0x00);
	clutter_actor_set_scale_with_gravity( arrow_fg_pic, 1.00, 1.00, CLUTTER_GRAVITY_CENTER);
	clutter_actor_set_scale_with_gravity( arrow_bg_pic, 0.50, 0.50, CLUTTER_GRAVITY_CENTER);


	ClutterScore* pressed_score = (ClutterScore*) g_object_get_data(G_OBJECT(arrow_group),"pressed_score");
	g_return_val_if_fail(CLUTTER_IS_SCORE(pressed_score), ret);
	clutter_score_stop(pressed_score);
	clutter_score_start(pressed_score);

	return ret; //return FALSE and the press-button-event signal on the same button will processed.

} //right_arrow_press_cb()


