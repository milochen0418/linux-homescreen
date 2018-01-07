#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <gtk/gtk.h>
#include <clutter-gtk/clutter-gtk.h>
#include <math.h>
#include "mailblk.h"
#include "clutter-behaviour-scale-gravity.h"

#define BORDER_FILEPATH ("over.png")
#define MAILBLK_BG_FILEPATH ("mail_BG.png")
#define MAILBLK_BG_SHADOW_FILEPATH ("mail_shadow.png")
#define HOLLOW_LINE_FILEPATH ("divider.png")
#define HOLLOW_LIGHT_FILEPATH ("highlight.png")
#define TRACK_FILEPATH ("track.png")
#define SCROLL_BUTTON_D_FILEPATH ("Mail_D.gif")
#define SCROLL_BUTTON_O_FILEPATH ("Mail_O.gif")
#define SCROLL_BUTTON_P_FILEPATH ("Mail_P.gif")



/* function for Web Block */

static gboolean mailblk_mail_item_enter_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean mailblk_mail_item_leave_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);


static ClutterActor* create_hollow(void);
static void hollow_effect_apply(ClutterActor* hollow_group);
static void mailmenu_scrolling_effects_apply (ClutterActor* mailmenu);

static ClutterActor* create_scrollbar(void);
static ClutterActor* create_mail_item(int idx);
static void  mail_item_set_index(ClutterActor* mail_item, int index);


typedef struct
{
	void (*get_distances) (ClutterActor* mailmenu, guint* total_distance, guint* page_distance);
	guint (*request_move_to) (ClutterActor* mailmenu, guint position);
}scrollbar_invokee_t;




typedef struct
{
	ClutterActor* scrollbar;
	ClutterActor* track;
	ClutterActor* button; 
	ClutterActor* button_o; 
	ClutterActor* button_d;
	ClutterActor* button_p;

	guint current_position;
	scrollbar_invokee_t* invokee;
	ClutterActor* scrolled_item;
	gboolean ondrag;
	gfloat drag_x; //drag_x offset_x for the button
	gfloat drag_y; //drag_y offset_y for the button
} scrollbar_private_data_t;


static scrollbar_private_data_t* scrollbar_private_data_new0(void);
static void  scrollbar_private_data_free(gpointer data);

typedef struct
{
	ClutterActor* mailmenu;
	ClutterActor* hollows[6+6];
	ClutterActor* mail_items[5+6];
} mailmenu_private_data_t;

static mailmenu_private_data_t* mailmenu_private_data_new0(void);
static void  mailmenu_private_data_free(gpointer data);
static ClutterActor* create_mailmenu(void);


typedef struct
{
	int status;
	char* senddate;
	char* from;
	char* title;
} mail_info_t;



static mail_info_t* mail_info_new0(void);
static void mail_info_free(gpointer info);
static mail_info_t* mail_info_new(const char* senddate, const char* from, const char* title);

static void mail_info_set_data(mail_info_t* info, const char* senddate, const char* from, const char* title);
static const mail_info_t* get_mail_info(int idx);
static gint get_mail_number(void);


typedef struct
{
	guint total_distance;
	guint page_distance;
	guint position;
}scrolling_info_t;
/* 
 * Like relation of employer and employee. Invokee is the one invoked  by invoker. 
 * Here scrollbar is the invoker for the invokee 'scrollbar_ivnokee_t'
 *
 * */

static void mailmenu_get_distances (ClutterActor* mailmenu, guint* total_distance, guint* page_distance);
static guint mailmenu_request_move_to (ClutterActor* mailmenu, guint position);

static void mailmenu_scrolling_info_init(ClutterActor* mailmenu);
static void scrollbar_connect_mailmenu(ClutterActor* scrollbar, ClutterActor* mailmenu);

static void scrollbar_request_page_down(ClutterActor* scrollbar);
static void scrollbar_request_page_up(ClutterActor* scrollbar);
static void scrollbar_request_unit_down(ClutterActor* scrollbar);
static void scrollbar_request_unit_up(ClutterActor* scrollbar);
static void scrollbar_request_move_to(ClutterActor* scrollbar, guint position);
static guint scrollbar_get_current_position(ClutterActor* scrollbar);
static void scrollbar_refresh_ui(ClutterActor* scrollbar);

static gboolean scrollbar_press_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean scrollbar_motion_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean scrollbar_release_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);
static gboolean scrollbar_leave_cb(ClutterActor* act, ClutterEvent* evt, gpointer data);


static gdouble circle_alpha_func(ClutterAlpha *alpha , gpointer data);

ClutterActor* create_mailblk(void)
{
	ClutterActor* mailblk; //ClutterGroup
	ClutterActor* background; //ClutterTexture
	ClutterActor* background_shadow; //ClutterTexture
	ClutterActor* title; //ClutterText
	ClutterActor* mailmenu_viewport;
	ClutterActor* mailmenu;
	

	ClutterActor* hollow1;
	ClutterActor* hollow2;
	ClutterActor* hollow3;
	ClutterActor* scrollbar;

	ClutterActor* hollows[6+12];
	ClutterActor* mail_items[5+12];
	int idx;	



	mailblk = clutter_group_new();
	mailmenu_viewport = clutter_group_new();
	mailmenu = create_mailmenu();

	clutter_container_add_actor(CLUTTER_CONTAINER(mailmenu_viewport), mailmenu);
#if 1
	clutter_actor_set_clip(mailmenu_viewport, 0,0,  230, 270);
#endif 



	clutter_actor_set_position(mailmenu_viewport, 18, 70);

	title = clutter_text_new_with_text("Mono 8 Bold", "Mail");

	GError* gerr= NULL;
	background = clutter_texture_new_from_file(MAILBLK_BG_FILEPATH,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	background_shadow = clutter_texture_new_from_file(MAILBLK_BG_SHADOW_FILEPATH,&gerr);
	g_return_val_if_fail(gerr==NULL, NULL);

	clutter_actor_set_position(title, 0,0);
	clutter_actor_set_position(background, 0,0);
	clutter_actor_set_position(background_shadow, -15,333);

	clutter_container_add(CLUTTER_CONTAINER(mailblk), 
		/*title, */
		background_shadow,
		background,	
		mailmenu_viewport,
		NULL);


	scrollbar = create_scrollbar();
	clutter_actor_set_position(scrollbar, 262-13, 70);
	clutter_container_add_actor(CLUTTER_CONTAINER(mailblk), scrollbar);
	

	ClutterColor *text_color = clutter_color_new(0xff, 0xff, 0xff, 0xff);
	clutter_text_set_color(CLUTTER_TEXT(title), text_color);
	clutter_color_free(text_color);
	clutter_actor_set_position(title, 115,-1);
	
	clutter_container_add_actor(CLUTTER_CONTAINER(mailblk),title);

	mailmenu_scrolling_effects_apply (mailmenu);
	
	scrollbar_connect_mailmenu(scrollbar, mailmenu);

	return mailblk;
} //create_mailblk();



static ClutterActor* create_hollow(void)
{
	ClutterActor* hollow_group;
	ClutterActor* text;
	ClutterActor* hollow_line;
	ClutterActor* hollow_light;	
	ClutterScore* enter_score;
	GError* gerr = NULL;


	hollow_group = clutter_group_new();

	enter_score = clutter_score_new();


	text = clutter_text_new_with_text("Mono 12", " ");	
	clutter_actor_set_position(text, 0,0);

	
	hollow_line = clutter_texture_new_from_file(HOLLOW_LINE_FILEPATH, &gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	clutter_actor_set_position(hollow_line, 0,3);
	clutter_actor_set_opacity(hollow_line, 0xff);
	clutter_actor_set_name(hollow_line, "hollow_line");


	hollow_light = clutter_texture_new_from_file(HOLLOW_LIGHT_FILEPATH, &gerr);
	g_return_val_if_fail(gerr==NULL, NULL);
	clutter_actor_set_position(hollow_light, -3, -1);
	clutter_actor_set_opacity(hollow_light, 0xff);
	clutter_actor_set_name(hollow_light, "hollow_light");


	g_object_set_data(G_OBJECT(hollow_group), "enter_score", enter_score);

	clutter_container_add_actor(CLUTTER_CONTAINER(hollow_group), text);
	clutter_container_add_actor(CLUTTER_CONTAINER(hollow_group), hollow_line);
	clutter_container_add_actor(CLUTTER_CONTAINER(hollow_group), hollow_light);
	clutter_actor_set_scale_with_gravity(hollow_light, 0.00, 1.00, CLUTTER_GRAVITY_WEST);


	gfloat* line_orig_pos_x = g_new0(gfloat, 1);
	gfloat* line_orig_pos_y = g_new0(gfloat, 1);
	guint8* line_orig_opacity=g_new0(guint8, 1);
	gfloat* light_orig_pos_x=  g_new0(gfloat, 1);
	gfloat* light_orig_pos_y=  g_new0(gfloat, 1);
	guint8* light_orig_opacity=g_new0(guint8, 1);

	clutter_actor_get_position(hollow_line, line_orig_pos_x, line_orig_pos_y);
	*line_orig_opacity = clutter_actor_get_opacity(hollow_line);
	clutter_actor_get_position(hollow_light,light_orig_pos_x,light_orig_pos_y);
	*light_orig_opacity = clutter_actor_get_opacity(hollow_light);

	g_object_set_data(G_OBJECT(hollow_group), "line_orig_pos_x", line_orig_pos_x);
	g_object_set_data(G_OBJECT(hollow_group), "line_orig_pos_y", line_orig_pos_y);
	g_object_set_data(G_OBJECT(hollow_group), "line_orig_opacity", line_orig_opacity);
	g_object_set_data(G_OBJECT(hollow_group), "light_orig_pos_x", light_orig_pos_x);
	g_object_set_data(G_OBJECT(hollow_group), "light_orig_pos_y", light_orig_pos_y);
	g_object_set_data(G_OBJECT(hollow_group), "light_orig_opacity", light_orig_opacity);


	hollow_effect_apply(hollow_group);

	clutter_score_set_loop(enter_score, FALSE);
//	clutter_score_start(enter_score);

	return hollow_group;
} //create_hollow()

static void hollow_effect_apply(ClutterActor* hollow_group)
{

	g_return_if_fail(hollow_group != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(hollow_group));
	//g_return_if_fail(CLUTTER_IS_GROUP(hollow_group));
	
	ClutterScore* enter_score;
	ClutterActor* hollow_line;
	ClutterActor* hollow_light;
	gfloat* line_orig_pos_x;
	gfloat* line_orig_pos_y;
	guint8* line_orig_opacity;
	gfloat* light_orig_pos_x;
	gfloat* light_orig_pos_y;
	guint8* light_orig_opacity;

	enter_score =(ClutterScore*) g_object_get_data(G_OBJECT(hollow_group), "enter_score");
	g_return_if_fail(enter_score != NULL);
	g_return_if_fail(CLUTTER_IS_SCORE(enter_score));

	line_orig_pos_x = (gfloat*) g_object_get_data(G_OBJECT(hollow_group), "line_orig_pos_x");
	line_orig_pos_y = (gfloat*) g_object_get_data(G_OBJECT(hollow_group), "line_orig_pos_y");
	line_orig_opacity = (guint8*) g_object_get_data(G_OBJECT(hollow_group), "line_orig_opacity");
	light_orig_pos_x = (gfloat*) g_object_get_data(G_OBJECT(hollow_group), "light_orig_pos_x");
	light_orig_pos_y = (gfloat*) g_object_get_data(G_OBJECT(hollow_group), "light_orig_pos_y");
	light_orig_opacity = (guint8*) g_object_get_data(G_OBJECT(hollow_group), "light_orig_opacity");

	g_return_if_fail(line_orig_pos_x != NULL);
	g_return_if_fail(line_orig_pos_y != NULL);
	g_return_if_fail(line_orig_opacity != NULL);
	g_return_if_fail(light_orig_pos_x != NULL);
	g_return_if_fail(light_orig_pos_y != NULL);
	g_return_if_fail(light_orig_opacity != NULL);

	hollow_line = clutter_container_find_child_by_name(CLUTTER_CONTAINER(hollow_group),"hollow_line");
	g_return_if_fail(hollow_line!=NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(hollow_line));
	hollow_light= clutter_container_find_child_by_name(CLUTTER_CONTAINER(hollow_group),"hollow_light");
	g_return_if_fail(hollow_light != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(hollow_light));


	const int step_time = 200;
	ClutterTimeline* tl1 = clutter_timeline_new(step_time);
	ClutterTimeline* tl2 = clutter_timeline_new(2*step_time);

	ClutterAlpha* al1 = clutter_alpha_new_full(tl1, CLUTTER_LINEAR);
	ClutterAlpha* al2 = clutter_alpha_new_full(tl2, CLUTTER_LINEAR);
	
	ClutterBehaviour* lightSG2 = clutter_behaviour_scale_gravity_new(al2, CLUTTER_GRAVITY_WEST, 0.00,0.00, 1.00, 1.00);

	clutter_behaviour_apply(lightSG2, hollow_light);

	clutter_score_append(enter_score, NULL, tl1);
	clutter_score_append(enter_score, tl1, tl2);

	
} //hollow_effect_apply()




static ClutterActor* create_scrollbar(void)
{
	scrollbar_private_data_t*  priv;
	priv = scrollbar_private_data_new0();
	g_return_val_if_fail(priv!=NULL, NULL);
	scrollbar_private_data_t* p = priv;

	GError* gerr = NULL;	

	p->scrollbar = clutter_group_new();
	p->button = clutter_group_new();
	p->track = clutter_texture_new_from_file(TRACK_FILEPATH, &gerr);

	p->button_d = clutter_texture_new_from_file(SCROLL_BUTTON_D_FILEPATH, &gerr);
	p->button_o = clutter_texture_new_from_file(SCROLL_BUTTON_O_FILEPATH, &gerr);
	p->button_p = clutter_texture_new_from_file(SCROLL_BUTTON_P_FILEPATH, &gerr);

	g_return_val_if_fail(p->scrollbar!=NULL, NULL);
	g_return_val_if_fail(p->button!=NULL, NULL);
	g_return_val_if_fail(p->button_d!=NULL, NULL);
	g_return_val_if_fail(p->button_o!=NULL, NULL);
	g_return_val_if_fail(p->button_p!=NULL, NULL);
	g_return_val_if_fail(p->track!=NULL,NULL);

	gfloat max_width = 0;
	gfloat max_height = 0;
	
	if( clutter_actor_get_width(p->button_d) > max_width) max_width = clutter_actor_get_width(p->button_d);
	if( clutter_actor_get_width(p->button_o) > max_width) max_width = clutter_actor_get_width(p->button_o);
	if( clutter_actor_get_width(p->button_p) > max_width) max_width = clutter_actor_get_width(p->button_p);
	
	if( clutter_actor_get_height(p->button_d) > max_height) max_height = clutter_actor_get_height(p->button_d);
	if( clutter_actor_get_height(p->button_o) > max_height) max_height = clutter_actor_get_height(p->button_o);
	if( clutter_actor_get_height(p->button_p) > max_height) max_height = clutter_actor_get_height(p->button_p);

	clutter_actor_set_size(p->button, max_width, max_height);
	clutter_actor_set_position(p->button_d, (max_width - clutter_actor_get_width(p->button_d))/2, 0);
	clutter_actor_set_position(p->button_o, (max_width - clutter_actor_get_width(p->button_o))/2, 0);
	clutter_actor_set_position(p->button_p, (max_width - clutter_actor_get_width(p->button_p))/2, 0);

#if 0
	clutter_actor_set_position(p->track, (max_width - clutter_actor_get_width(p->track))/2, 0);
#else
	if(1)
	{
		gfloat scroll_width = 30.0;
		gfloat scroll_height = clutter_actor_get_height(p->track);
		clutter_actor_set_size(p->scrollbar, scroll_width, scroll_height);
		clutter_actor_set_anchor_point_from_gravity(p->track, CLUTTER_GRAVITY_NORTH_WEST);
		clutter_actor_set_anchor_point_from_gravity(p->button, CLUTTER_GRAVITY_NORTH_WEST);

		clutter_actor_set_position(p->track, (scroll_width - clutter_actor_get_width(p->track))/2, 0);
		clutter_actor_set_position(p->button,(scroll_width - clutter_actor_get_width(p->button))/2, 0);
	}
#endif 	
	clutter_container_add(CLUTTER_CONTAINER(p->button),p->button_o, p->button_d, p->button_p ,NULL);

	clutter_container_add(CLUTTER_CONTAINER(p->scrollbar), p->track, p->button, NULL);
	clutter_actor_show_all(p->scrollbar);
	clutter_actor_hide(p->button_o);
	clutter_actor_hide(p->button_d);
	g_object_set_data(G_OBJECT(priv->scrollbar), "priv", priv);

	return priv->scrollbar;
} //create_scroll_bar()




static scrollbar_private_data_t* scrollbar_private_data_new0(void)
{
	return g_new0(scrollbar_private_data_t,1);
} //scrollbar_private_data_new0()

static void  scrollbar_private_data_free(gpointer data)
{
	g_free(data);
} //scroll_private_data_free()





static mail_info_t* mail_info_new0(void)
{
	return g_new0(mail_info_t,1);
} //mail_info_new0()

static void mail_info_free(gpointer info)
{
	if(info == NULL ) return;
	mail_info_t* mail_info = (mail_info_t*)info;
	g_free(mail_info->senddate);
	g_free(mail_info->from);
	g_free(mail_info->title);
	g_free(mail_info);
} //mail_info_free()



static mail_info_t* mail_info_new(const char* senddate, const char* from, const char* title)
{
	mail_info_t* mail_info;
	g_return_val_if_fail(senddate != NULL, NULL);
	g_return_val_if_fail(from!= NULL, NULL);
	g_return_val_if_fail(title!=NULL, NULL);
	mail_info = mail_info_new0();
	g_return_val_if_fail(mail_info != NULL, NULL);
	mail_info_set_data(mail_info, senddate, from, title);
	return mail_info;
} //mail_info_new()


static void mail_info_set_data(mail_info_t* info, const char* senddate, const char* from, const char* title)
{
	g_return_if_fail(info!=NULL);
	
	if(senddate != NULL)
	{
		g_free(info->senddate);
		info->senddate = g_strdup(senddate);
	}

	if(from != NULL)
	{
		g_free(info->from);
		info->from = g_strdup(from);
	}

	if(title != NULL)
	{
		g_free(info->title);
		info->title = g_strdup(title);
	}
} //mail_info_set_data()

static int get_mail_number(void)
{
	return 103;
}

static const mail_info_t* get_mail_info(int idx)
{	
	if(idx >= get_mail_number()) return NULL;
	static mail_info_t** mails = NULL;
	if(mails == NULL)
	{
		mails  = g_new0(mail_info_t*, get_mail_number());
		int idx;
		for(idx = 0 ; idx < get_mail_number(); idx++)
		{
			char* senddate;
			char* from;
			char* title;
			mail_info_t* mail_info;

			senddate = g_strdup_printf("2009-02-%d", idx);
			from = g_strdup_printf("Man.%d", idx);
			title = g_strdup_printf("- title = %d -", idx);
			mail_info = mail_info_new(senddate, from, title);
			g_free(senddate);
			g_free(from);
			g_free(title);
			mails[idx] = mail_info;
		}
	}

	g_return_val_if_fail(idx >= 0 && idx < get_mail_number(),NULL);
	g_return_val_if_fail(mails[idx] != NULL,NULL);
	return mails[idx];

} //get_mail_info()








static void  mail_item_set_index(ClutterActor* mail_item, int idx)
{
	g_debug("%s enter idx = %d",G_STRFUNC, idx);
	g_return_if_fail(mail_item != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(mail_item));

	ClutterActor* mail_item_title = clutter_container_find_child_by_name(CLUTTER_CONTAINER(mail_item), "title");
	ClutterActor* mail_item_from = clutter_container_find_child_by_name(CLUTTER_CONTAINER(mail_item), "from");
	ClutterActor* mail_item_senddate = clutter_container_find_child_by_name(CLUTTER_CONTAINER(mail_item),"senddate");
	const mail_info_t* mail_info = get_mail_info(idx);

	if(mail_info == NULL)
	{
		static const char fromstr[] = "";
		static const char titlestr[] = "";
		static const char senddatestr[] = "";
		clutter_text_set_text(CLUTTER_TEXT(mail_item_from),  fromstr);
		clutter_text_set_text(CLUTTER_TEXT(mail_item_title),  titlestr);
		clutter_text_set_text(CLUTTER_TEXT(mail_item_senddate),  senddatestr);
		g_debug("%s leave idx = %d",G_STRFUNC, idx);
		return;

	}

	g_return_if_fail(mail_info != NULL);
	g_return_if_fail(mail_item_title != NULL);
	g_return_if_fail(mail_item_from != NULL);
	g_return_if_fail(mail_item_senddate != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(mail_item_title));
	g_return_if_fail(CLUTTER_IS_ACTOR(mail_item_from));
	g_return_if_fail(CLUTTER_IS_ACTOR(mail_item_senddate));
	g_return_if_fail(CLUTTER_IS_TEXT(mail_item_title));
	g_return_if_fail(CLUTTER_IS_TEXT(mail_item_from));
	g_return_if_fail(CLUTTER_IS_TEXT(mail_item_senddate));


	g_return_if_fail(mail_info->from != NULL);
	g_return_if_fail(mail_info->title != NULL);
	g_return_if_fail(mail_info->senddate != NULL);

	clutter_text_set_text(CLUTTER_TEXT(mail_item_from),  mail_info->from);
	clutter_text_set_text(CLUTTER_TEXT(mail_item_title),  mail_info->title);
	clutter_text_set_text(CLUTTER_TEXT(mail_item_senddate),  mail_info->senddate);
	g_debug("%s leave idx = %d",G_STRFUNC, idx);

} //mail_item_set_index()



static ClutterActor* create_mail_item(int idx)
{
	ClutterActor* mail_item;
	ClutterActor* mail_item_title;
	ClutterActor* mail_item_from;
	ClutterActor* mail_item_senddate;
	const mail_info_t* mail_info;

	mail_item = clutter_group_new();

	mail_info = get_mail_info(idx);
	mail_item_from = clutter_text_new_with_text("Mono 10", mail_info->from);
	mail_item_senddate = clutter_text_new_with_text("Mono 8", mail_info->senddate);
	mail_item_title = clutter_text_new_with_text("Mono 10", mail_info->title);


	clutter_actor_set_name(mail_item_from, "from");
	clutter_actor_set_name(mail_item_senddate, "senddate");
	clutter_actor_set_name(mail_item_title, "title");


	clutter_actor_set_position(mail_item_from, 0,0);
	clutter_actor_set_position(mail_item_senddate, 110, 0);
	clutter_actor_set_position(mail_item_title, 0, 20);

	clutter_container_add(CLUTTER_CONTAINER(mail_item), mail_item_title, mail_item_from, mail_item_senddate, NULL);
	return mail_item;
} //create_mail_item()





static gboolean mailblk_mail_item_enter_cb(ClutterActor* mail_item, ClutterEvent* evt, gpointer data)
{
	gboolean ret = FALSE;
	ClutterActor* prev_hollow;
	ClutterActor* next_hollow;
	ClutterScore* prev_hollow_enter_score;
	ClutterScore* next_hollow_enter_score;
	ClutterActor* prev_hollow_light;
	ClutterActor* next_hollow_light;

	prev_hollow =(ClutterActor*) g_object_get_data(G_OBJECT(mail_item), "prev_hollow");
	next_hollow =(ClutterActor*) g_object_get_data(G_OBJECT(mail_item), "next_hollow");
	g_return_val_if_fail(prev_hollow!=NULL, ret);
	g_return_val_if_fail(next_hollow!=NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(prev_hollow),ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(next_hollow),ret);
	prev_hollow_enter_score = (ClutterScore*) g_object_get_data(G_OBJECT(prev_hollow), "enter_score");
	next_hollow_enter_score = (ClutterScore*) g_object_get_data(G_OBJECT(next_hollow), "enter_score");
	g_return_val_if_fail(prev_hollow_enter_score!=NULL, ret);
	g_return_val_if_fail(next_hollow_enter_score!=NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(prev_hollow_enter_score),ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(next_hollow_enter_score),ret);

	prev_hollow_light = clutter_container_find_child_by_name(CLUTTER_CONTAINER(prev_hollow), "hollow_light");
	next_hollow_light = clutter_container_find_child_by_name(CLUTTER_CONTAINER(next_hollow), "hollow_light");
	g_return_val_if_fail(prev_hollow_light != NULL, ret);
	g_return_val_if_fail(next_hollow_light != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(prev_hollow_light),ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(next_hollow_light),ret);
	
	clutter_score_start(prev_hollow_enter_score);
	clutter_score_start(next_hollow_enter_score);

	return ret;

} //mailblk_mail_item_enter_cb()



static gboolean mailblk_mail_item_leave_cb(ClutterActor* mail_item, ClutterEvent* evt, gpointer data)
{
	gboolean ret = FALSE;
	ClutterActor* prev_hollow;
	ClutterActor* next_hollow;
	ClutterScore* prev_hollow_enter_score;
	ClutterScore* next_hollow_enter_score;
	ClutterActor* prev_hollow_light;
	ClutterActor* next_hollow_light;

	prev_hollow =(ClutterActor*) g_object_get_data(G_OBJECT(mail_item), "prev_hollow");
	next_hollow =(ClutterActor*) g_object_get_data(G_OBJECT(mail_item), "next_hollow");
	g_return_val_if_fail(prev_hollow!=NULL, ret);
	g_return_val_if_fail(next_hollow!=NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(prev_hollow),ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(next_hollow),ret);
	prev_hollow_enter_score = (ClutterScore*) g_object_get_data(G_OBJECT(prev_hollow), "enter_score");
	next_hollow_enter_score = (ClutterScore*) g_object_get_data(G_OBJECT(next_hollow), "enter_score");
	g_return_val_if_fail(prev_hollow_enter_score!=NULL, ret);
	g_return_val_if_fail(next_hollow_enter_score!=NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(prev_hollow_enter_score),ret);
	g_return_val_if_fail(CLUTTER_IS_SCORE(next_hollow_enter_score),ret);

	prev_hollow_light = clutter_container_find_child_by_name(CLUTTER_CONTAINER(prev_hollow), "hollow_light");
	next_hollow_light = clutter_container_find_child_by_name(CLUTTER_CONTAINER(next_hollow), "hollow_light");
	g_return_val_if_fail(prev_hollow_light != NULL, ret);
	g_return_val_if_fail(next_hollow_light != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(prev_hollow_light),ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(next_hollow_light),ret);

	clutter_score_stop(prev_hollow_enter_score);
	clutter_score_stop(next_hollow_enter_score);
	clutter_actor_set_scale_with_gravity(prev_hollow_light, 0.00,1.00, CLUTTER_GRAVITY_WEST);
	clutter_actor_set_scale_with_gravity(next_hollow_light, 0.00,1.00, CLUTTER_GRAVITY_WEST);



	return ret;

} //mailblk_mail_item_leave_cb()
 

static void mailmenu_scrolling_info_init(ClutterActor* mailmenu)
{
	g_return_if_fail(mailmenu != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(mailmenu));
	scrolling_info_t* scrolling_info;
	scrolling_info =(scrolling_info_t*)g_object_get_data(G_OBJECT(mailmenu),"scrolling_info");
	if(scrolling_info != NULL) return;

	scrolling_info = g_new0(scrolling_info_t,1);
	
	scrolling_info->total_distance = get_mail_number();
	scrolling_info->page_distance = 5;
	scrolling_info->position = 0;
	g_object_set_data(G_OBJECT(mailmenu), "scrolling_info", scrolling_info);
} //mailmenu_scrollbar_init()



static void mailmenu_get_distances (ClutterActor* mailmenu, guint* total_distance, guint* page_distance)
{
	g_return_if_fail(mailmenu != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(mailmenu));
	scrolling_info_t* scrolling_info;
	mailmenu_scrolling_info_init(mailmenu);
	scrolling_info =(scrolling_info_t*)g_object_get_data(G_OBJECT(mailmenu),"scrolling_info");

	g_return_if_fail(scrolling_info != NULL);
	if(total_distance != NULL)	*total_distance = scrolling_info->total_distance;
	if(page_distance != NULL) *page_distance = scrolling_info->page_distance;

	return;
} //mailmenu_viewport_get_distances

static guint mailmenu_request_move_to (ClutterActor* mailmenu, guint position)
{
	g_debug("%s(position=%d)", G_STRFUNC, position);
	g_return_if_fail(mailmenu != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(mailmenu));
	scrolling_info_t* scrolling_info;
	mailmenu_scrolling_info_init(mailmenu);
	scrolling_info =(scrolling_info_t*)g_object_get_data(G_OBJECT(mailmenu),"scrolling_info");
	g_return_if_fail(scrolling_info != NULL);
	guint predict_position = 0;
	guint move_distance = 0;
	if(scrolling_info->total_distance  < scrolling_info->page_distance)
	{
		predict_position = 0;
	}
	else if(position > (gint)scrolling_info->total_distance - (gint)scrolling_info->page_distance )
	{
		predict_position = (gint)scrolling_info->total_distance - (gint)scrolling_info->page_distance;
	}
	else
	{
		predict_position = position;
	}



	if(predict_position == scrolling_info->position )
	{
		return predict_position;
	}

	ClutterScore** scroll_up_scores = (ClutterScore**)g_object_get_data(G_OBJECT(mailmenu),"scroll_up_scores");
	ClutterScore** scroll_down_scores = (ClutterScore**)g_object_get_data(G_OBJECT(mailmenu),"scroll_down_scores");

	int idx;
	for (idx = 0; idx < 5; idx ++)
	{
		clutter_score_stop(scroll_up_scores[idx]);
		clutter_score_stop(scroll_down_scores[idx]);
	}

	mailmenu_private_data_t* priv = (mailmenu_private_data_t*) g_object_get_data(G_OBJECT(mailmenu),"priv");
	g_return_if_fail(priv!=NULL);


	if( predict_position > scrolling_info->position  )
	{

		/* Case Scrolling Down */

		g_debug("\n\n\n Case Scrolling Down \n");
		move_distance = predict_position - scrolling_info->position;

		if(move_distance > scrolling_info->page_distance) 
		{
			move_distance = scrolling_info->page_distance;
			predict_position = move_distance + scrolling_info->position;
		}

		for(idx = 0; idx < 11; idx++)
		{

			ClutterActor* mail_item = priv->mail_items[idx];
			int set_index = scrolling_info->position + idx;
			g_debug("scrolling_info->position = %d ; set mail_items[%d] = %d", scrolling_info->position, idx, set_index);

			mail_item_set_index(mail_item, set_index);
		}
		clutter_score_start(scroll_down_scores[move_distance-1]);

		g_debug("scroll down %d", move_distance);

	} 
	else 
	{
		/* Case Scrolling Up */

		g_debug("\n\n\n Case Scrolling Up \n");
		move_distance = scrolling_info->position - predict_position;
		if(move_distance > scrolling_info->page_distance) 
		{
			move_distance = scrolling_info->page_distance;
			predict_position = scrolling_info->position - move_distance;
		}


		for(idx = 0; idx < 11; idx++)
		{
			ClutterActor* mail_item = priv->mail_items[idx];
			int set_index =  scrolling_info->position - 5 + idx;
			g_debug("scrolling_info->position = %d ; set mail_items[%d] = %d", scrolling_info->position, idx, set_index);
			mail_item_set_index(mail_item,  set_index);
		}
		clutter_score_start(scroll_up_scores[move_distance-1]);
		g_debug("scroll up %d", move_distance);
	}
	scrolling_info->position = predict_position;
	return predict_position;


} //mailmenu_request_move_to(guint position)





static void mailmenu_scrolling_effects_apply(ClutterActor* mailmenu)
{
	g_return_if_fail(mailmenu != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(mailmenu));

	ClutterScore** scroll_up_scores = g_new0(ClutterScore*,5);
	ClutterScore** scroll_down_scores = g_new0(ClutterScore*,5);

	g_object_set_data(G_OBJECT(mailmenu), "scroll_up_scores", scroll_up_scores);
	g_object_set_data(G_OBJECT(mailmenu), "scroll_down_scores", scroll_down_scores);

	int idx;
	for(idx = 0; idx< 5; idx++)
	{
		const int step_time = 125;
		const int halt_time = 0;
		gulong alpha_mode = CLUTTER_LINEAR;
		scroll_down_scores[idx] = clutter_score_new();
		if(1)
		{
			ClutterTimeline *tl0 = clutter_timeline_new(step_time*3);
			ClutterTimeline *tl1 = clutter_timeline_new((1+idx) * step_time);
			ClutterTimeline *tl2 = clutter_timeline_new(halt_time);
			
			ClutterAlpha *al1 = clutter_alpha_new_full(tl1, alpha_mode);
			ClutterAlpha *alT1 = clutter_alpha_new_with_func(tl1, circle_alpha_func, NULL, NULL);

			ClutterPath* pPath1 = clutter_path_new();
			clutter_path_add_move_to(pPath1, 0,0);
			clutter_path_add_rel_line_to(pPath1, 0, (1+idx) *-52);
			ClutterBehaviour* pP1 = clutter_behaviour_path_new(al1, pPath1);


			ClutterBehaviour *pO1 = NULL;
			ClutterBehaviour *pSG1 = NULL;
			if( idx >= 3 )
			{
				pO1 = clutter_behaviour_opacity_new(alT1, 0xff, 0x4f);
				pSG1 = clutter_behaviour_scale_gravity_new(alT1, CLUTTER_GRAVITY_WEST, 1.00,1.00,1.20,1.20);
			}
			else
			{
				pO1 = clutter_behaviour_opacity_new(alT1, 0xff, 0xff);
				pSG1 = clutter_behaviour_scale_gravity_new(alT1, CLUTTER_GRAVITY_WEST, 1.00,1.00,1.00,1.00);
			}

			clutter_behaviour_apply(pP1, mailmenu);
			clutter_behaviour_apply(pO1, mailmenu);
			clutter_behaviour_apply(pSG1, mailmenu);
			clutter_score_append(scroll_down_scores[idx], NULL, tl1);
			clutter_score_append(scroll_down_scores[idx], tl1, tl2);
		}

		scroll_up_scores[idx] = clutter_score_new();
		if(1)
		{
			ClutterTimeline *tl0 = clutter_timeline_new(step_time*3);
			ClutterTimeline *tl1 = clutter_timeline_new((1+idx) * step_time);
			ClutterTimeline *tl2 = clutter_timeline_new(halt_time);
		
			ClutterAlpha *al1 = clutter_alpha_new_full(tl1, alpha_mode);
			ClutterAlpha *alT1 = clutter_alpha_new_with_func(tl1, circle_alpha_func, NULL, NULL);
			ClutterPath* pPath1 = clutter_path_new();
			clutter_path_add_move_to(pPath1, 0,-52*5);
			clutter_path_add_rel_line_to(pPath1, 0, (1+idx) * 52);
			ClutterBehaviour* pP1 = clutter_behaviour_path_new(al1, pPath1);

			ClutterBehaviour* pO1;
			ClutterBehaviour* pSG1;
			if( idx >= 3)
			{		
				pO1 = clutter_behaviour_opacity_new(alT1, 0xff, 0x7f);
				pSG1 = clutter_behaviour_scale_gravity_new(alT1, CLUTTER_GRAVITY_WEST, 1.00,1.00,0.80,0.80);
			}
			else
			{
				pO1 = clutter_behaviour_opacity_new(alT1, 0xff, 0xff);
				pSG1 = clutter_behaviour_scale_gravity_new(alT1, CLUTTER_GRAVITY_WEST, 1.00,1.00,1.00,1.00);
			}

			clutter_behaviour_apply(pP1, mailmenu);
			clutter_behaviour_apply(pO1, mailmenu);
			clutter_behaviour_apply(pSG1, mailmenu);
			clutter_score_append(scroll_up_scores[idx], NULL, tl1);
			clutter_score_append(scroll_up_scores[idx], tl1, tl2);
		}
	}

	//ClutterScore* score = scroll_down_scores[4];
	ClutterScore* score = scroll_up_scores[4];
//	clutter_score_set_loop(score, TRUE);
//	clutter_score_start(score);

} //mailmenu_scrolling_effect_apply()




static void scrollbar_connect_mailmenu(ClutterActor* scrollbar, ClutterActor* mailmenu)
{
	g_return_if_fail(scrollbar != NULL);
	g_return_if_fail(mailmenu != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(scrollbar));
	g_return_if_fail(CLUTTER_IS_ACTOR(mailmenu));

	scrollbar_private_data_t* priv;

	priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_if_fail(priv!=NULL);
	
	if(priv->invokee != NULL) return;
	
	priv->scrolled_item = mailmenu;

	scrollbar_invokee_t* scrollbar_invokee;
	scrollbar_invokee = g_new0(scrollbar_invokee_t,1);
	scrollbar_invokee->get_distances = mailmenu_get_distances;
	scrollbar_invokee->request_move_to = mailmenu_request_move_to;
	priv->invokee = scrollbar_invokee;
	priv->current_position = 0;


	priv->current_position = priv->invokee->request_move_to(priv->scrolled_item, 0);
	clutter_actor_set_reactive(scrollbar, TRUE);
	g_signal_connect(G_OBJECT(scrollbar), "button-press-event", G_CALLBACK(scrollbar_press_cb), NULL);
	g_signal_connect(G_OBJECT(scrollbar), "motion-event", G_CALLBACK(scrollbar_motion_cb), NULL);
	g_signal_connect(G_OBJECT(scrollbar), "button-release-event", G_CALLBACK(scrollbar_release_cb), NULL);
	g_signal_connect(G_OBJECT(scrollbar), "leave-event", G_CALLBACK(scrollbar_leave_cb), NULL);

} //scrollbar_connect_mailmenu()

static void scrollbar_request_move_to(ClutterActor* scrollbar, guint position)
{

	g_debug("%s(position=%d)", G_STRFUNC, position);
	guint total_distance;
	guint page_distance;
	guint current_position;
	guint predict_position = position; 

	g_return_if_fail(scrollbar != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(scrollbar));
	scrollbar_private_data_t* priv;
	priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_if_fail(priv!=NULL);

	ClutterActor* scrolled_item = priv->scrolled_item;
	g_return_if_fail(scrolled_item != NULL);
	scrollbar_invokee_t* invokee = priv->invokee;
	g_return_if_fail(invokee != NULL);
	g_return_if_fail(invokee->request_move_to != NULL);
	g_return_if_fail(invokee->get_distances != NULL);	
	invokee->get_distances(scrolled_item, &total_distance, &page_distance);
	g_return_if_fail(total_distance >= page_distance);

	current_position = priv->current_position;

	if(predict_position >= total_distance) predict_position = total_distance - 1;
	if(current_position < predict_position)
	{
		if((gint)predict_position - (gint)current_position > page_distance)
		{
			predict_position = current_position + page_distance;
		}	
	}
	else if(current_position > predict_position )
	{
		if((gint)current_position - (gint)predict_position > page_distance)
		{
			gint configure_predict_position = (gint)current_position - (gint)page_distance;
			if(configure_predict_position < 0 ) configure_predict_position = 0;
			predict_position = configure_predict_position;
		}
	}

	priv->current_position = invokee->request_move_to(scrolled_item,position);
	scrollbar_refresh_ui(scrollbar);

} //scrollbar_request_move_to()


static void scrollbar_request_page_down(ClutterActor* scrollbar)
{
	guint total_distance;
	guint page_distance;
	guint current_position;
	guint predict_position;

	g_return_if_fail(scrollbar != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(scrollbar));
	scrollbar_private_data_t* priv;
	priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_if_fail(priv!=NULL);

	ClutterActor* scrolled_item = priv->scrolled_item;
	g_return_if_fail(scrolled_item != NULL);
	scrollbar_invokee_t* invokee = priv->invokee;
	g_return_if_fail(invokee != NULL);
	g_return_if_fail(invokee->request_move_to != NULL);
	g_return_if_fail(invokee->get_distances != NULL);
	invokee->get_distances(scrolled_item, &total_distance, &page_distance);
	g_return_if_fail(total_distance >= page_distance);

	current_position = priv->current_position;
	predict_position = current_position  + page_distance;

	g_debug("%s:%s(predict_position=%d)", G_STRLOC, G_STRFUNC, predict_position);
#if 0
	if((current_position > predict_position) || (predict_position >= total_distance)) 
	{
		if(total_distance > total_distance - page_distance )
		{
			predict_position = total_distance - page_distance ;
		}
		else
		{
			predict_position = 0;
			g_debug("%s:page_distance = %d", G_STRLOC, page_distance);
		}
	}
#else
	if(predict_position + page_distance >= total_distance)
	{
		predict_position = total_distance - page_distance;
	}
#endif 

	
	g_debug("%s:page_distance = %d", G_STRLOC, page_distance);


	scrollbar_request_move_to(scrollbar, predict_position); 
} //scrollbar_request_page_down()

static void scrollbar_request_page_up(ClutterActor* scrollbar)
{
	guint total_distance;
	guint page_distance;
	guint current_position;
	guint predict_position;

	g_return_if_fail(scrollbar != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(scrollbar));
	scrollbar_private_data_t* priv;
	priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_if_fail(priv!=NULL);

	ClutterActor* scrolled_item = priv->scrolled_item;
	g_return_if_fail(scrolled_item != NULL);
	scrollbar_invokee_t* invokee = priv->invokee;
	g_return_if_fail(invokee != NULL);
	g_return_if_fail(invokee->request_move_to != NULL);
	g_return_if_fail(invokee->get_distances != NULL);
	invokee->get_distances(scrolled_item, &total_distance, &page_distance);
	g_return_if_fail(total_distance >= page_distance);

	current_position = priv->current_position;
	if(current_position < page_distance)
	{
		predict_position = 0;
	}
	else
	{
		predict_position = (guint)((gint)current_position - (gint)page_distance);
	}
	scrollbar_request_move_to(scrollbar, predict_position); 
} //scrollbar_request_page_up()


static void scrollbar_request_unit_down(ClutterActor* scrollbar)
{
	guint total_distance;
	guint page_distance;
	guint current_position;
	guint predict_position;

	g_return_if_fail(scrollbar != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(scrollbar));
	scrollbar_private_data_t* priv;
	priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_if_fail(priv!=NULL);

	ClutterActor* scrolled_item = priv->scrolled_item;
	g_return_if_fail(scrolled_item != NULL);
	scrollbar_invokee_t* invokee = priv->invokee;
	g_return_if_fail(invokee != NULL);
	g_return_if_fail(invokee->request_move_to != NULL);
	g_return_if_fail(invokee->get_distances != NULL);
	invokee->get_distances(scrolled_item, &total_distance, &page_distance);
	g_return_if_fail(total_distance >= page_distance);

	current_position = priv->current_position;
	if(current_position >= total_distance || ( current_position > (current_position+1)) )
	{
		if(total_distance > total_distance - 1)
		{
			predict_position = total_distance - 1;
		}
		else
		{
			predict_position = 0;
		}
	}
	else
	{
		if(predict_position <= predict_position + page_distance)
		{
			predict_position = predict_position + page_distance;
		}
		else
		{
			predict_position = 0;
		}
	}
	scrollbar_request_move_to(scrollbar, predict_position); 
} //scrollbar_request_unit_down()




static void scrollbar_request_unit_up(ClutterActor* scrollbar)
{
	guint total_distance;
	guint page_distance;
	guint current_position;
	guint predict_position;

	g_return_if_fail(scrollbar != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(scrollbar));
	scrollbar_private_data_t* priv;
	priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_if_fail(priv!=NULL);

	ClutterActor* scrolled_item = priv->scrolled_item;
	g_return_if_fail(scrolled_item != NULL);
	scrollbar_invokee_t* invokee = priv->invokee;
	g_return_if_fail(invokee != NULL);
	g_return_if_fail(invokee->request_move_to != NULL);
	g_return_if_fail(invokee->get_distances != NULL);
	invokee->get_distances(scrolled_item, &total_distance, &page_distance);
	g_return_if_fail(total_distance >= page_distance);

	current_position = priv->current_position;
	if(current_position == 0)
	{
		predict_position = 0;
	}
	else
	{
		if(current_position > current_position - 1)
		{
			predict_position = current_position -1;
		}
		else
		{
			predict_position = 0;
		}
	}
	scrollbar_request_move_to(scrollbar, predict_position); 
} //scrollbar_request_unit_up()



static void scrollbar_refresh_ui(ClutterActor* scrollbar)
{
	guint total_distance;
	guint page_distance;
	guint current_position;
	guint predict_position;
	ClutterActor* button;

	gfloat orig_button_pos_x;
	gfloat orig_button_height;
	gfloat orig_track_height;
	gfloat orig_track_pos_y;

	gfloat predict_button_pos_y;

	g_return_if_fail(scrollbar != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(scrollbar));
	scrollbar_private_data_t* priv;
	priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_if_fail(priv!=NULL);

	ClutterActor* scrolled_item = priv->scrolled_item;
	g_return_if_fail(scrolled_item != NULL);
	scrollbar_invokee_t* invokee = priv->invokee;
	g_return_if_fail(invokee != NULL);
	g_return_if_fail(invokee->request_move_to != NULL);
	g_return_if_fail(invokee->get_distances != NULL);
	invokee->get_distances(scrolled_item, &total_distance, &page_distance);
	g_return_if_fail(total_distance >= page_distance);

	current_position = priv->current_position;
	g_return_if_fail(priv->button != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(priv->button));
	g_return_if_fail(priv->track != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(priv->track));

	orig_button_pos_x = clutter_actor_get_x(priv->button);
	orig_button_height = clutter_actor_get_height(priv->button);
	orig_track_height = clutter_actor_get_height(priv->track);
	orig_track_pos_y = clutter_actor_get_y(priv->track);
	
	predict_button_pos_y = orig_track_pos_y +  (orig_track_height - orig_button_height)*((gfloat)current_position/(gfloat)(total_distance-page_distance));

	g_debug("orig_button_pos_x=%f, orig_button_height=%f, orig_track_height=%f, orig_track_pos_y=%f", orig_button_pos_x, orig_button_height, orig_track_height, orig_track_pos_y);
	g_debug("predict_button_pos_y=%f, [PS: current_position=%d, total_distance=%d",predict_button_pos_y, current_position, total_distance);


	clutter_actor_set_position(priv->button, orig_button_pos_x, predict_button_pos_y);
	return;
} //scrollbar_refresh_ui()

static guint scrollbar_get_current_position(ClutterActor* scrollbar)
{
	g_return_if_fail(scrollbar != NULL);
	g_return_if_fail(CLUTTER_IS_ACTOR(scrollbar));
	scrollbar_private_data_t* priv;
	priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_if_fail(priv!=NULL);
	return priv->current_position;
} //scrollbar_get_current_position()


static gboolean scrollbar_press_cb(ClutterActor* scrollbar, ClutterEvent* evt, gpointer data)
{
	gboolean ret = FALSE;
	g_return_val_if_fail(scrollbar != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(scrollbar), ret);
	scrollbar_private_data_t* priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_val_if_fail(priv != NULL ,ret);

	
	ClutterButtonEvent* btnevt = (ClutterButtonEvent*)evt;


	gfloat x_out = 0;
	gfloat y_out = 0;
	clutter_actor_transform_stage_point(scrollbar, btnevt->x, btnevt->y, &x_out, &y_out);
	

	if( priv->ondrag == TRUE) return ret;
	if( y_out < clutter_actor_get_y(priv->button))
	{
		scrollbar_request_page_up(scrollbar);
	}
	else if(y_out > clutter_actor_get_y(priv->button) + clutter_actor_get_height(priv->button))
	{
		scrollbar_request_page_down(scrollbar);
	}
	else
	{
		return ret;
		priv->ondrag = TRUE;
		clutter_actor_transform_stage_point(scrollbar, btnevt->x, btnevt->y, &(priv->drag_x), &(priv->drag_y));
	}
	return ret;
} //scrollbar_press_cb()

static gboolean scrollbar_motion_cb(ClutterActor* scrollbar, ClutterEvent* evt, gpointer data)
{
	gboolean ret = TRUE;
	g_return_val_if_fail(scrollbar != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(scrollbar), ret);
	scrollbar_private_data_t* priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_val_if_fail(priv != NULL ,ret);

	
	ClutterMotionEvent* motionevt = (ClutterMotionEvent*)evt;

	if(priv->ondrag == FALSE) return ret;

	gfloat x_out = 0;
	gfloat y_out = 0;
	clutter_actor_transform_stage_point(scrollbar, motionevt->x, motionevt->y, &x_out, &y_out);
	

	guint page_distance;
	guint total_distance;
	scrollbar_invokee_t *invokee;
	ClutterActor* scrolled_item;

	scrolled_item  = priv->scrolled_item;
	g_return_val_if_fail(scrolled_item != NULL,ret);
	invokee = priv->invokee;
	g_return_val_if_fail(invokee != NULL,ret);
	g_return_val_if_fail(invokee->request_move_to != NULL,ret);
	g_return_val_if_fail(invokee->get_distances != NULL,ret);
	invokee->get_distances(scrolled_item, &total_distance, &page_distance);
	g_return_val_if_fail(total_distance >= page_distance,ret);

	guint current_position = priv->current_position;
	g_return_val_if_fail(priv->button != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(priv->button),ret);
	g_return_val_if_fail(priv->track != NULL,ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(priv->track), ret);

	gfloat orig_track_pos_y = clutter_actor_get_y(priv->track);
	gfloat orig_button_pos_x = clutter_actor_get_x(priv->button);
	gfloat orig_button_height = clutter_actor_get_height(priv->button);
	gfloat orig_track_height = clutter_actor_get_height(priv->track);
	
	gfloat predict_button_pos_y = y_out - priv->drag_y;
	guint moveto_position = (guint)( ( (predict_button_pos_y - orig_track_pos_y) / (orig_track_height - orig_button_height) ) * (gfloat)(total_distance-page_distance) );
	g_debug("moveto_position = %d", moveto_position);
#if 0		
	scrollbar_request_move_to(scrolled_item, moveto_position);
#else
	scrollbar_request_page_down(scrolled_item);
#endif 

	g_debug("ONDRAG");	

	return ret;
} //scrollbar_motion_cb()


static gboolean scrollbar_release_cb(ClutterActor* scrollbar, ClutterEvent* evt, gpointer data)
{
	gboolean ret = FALSE;
	g_return_val_if_fail(scrollbar != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(scrollbar), ret);
	scrollbar_private_data_t* priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_val_if_fail(priv != NULL ,ret);

	return ret;
	ClutterButtonEvent* btnevt = (ClutterButtonEvent*)evt;


	gfloat x_out = 0;
	gfloat y_out = 0;
	clutter_actor_transform_stage_point(scrollbar, btnevt->x, btnevt->y, &x_out, &y_out);
	

	priv->ondrag = FALSE;
	g_debug("RELEASE");	
	return ret;
} //scrollbar_release_cb()

static gboolean scrollbar_leave_cb(ClutterActor* scrollbar, ClutterEvent* evt, gpointer data)
{
	gboolean ret = FALSE;
	g_return_val_if_fail(scrollbar != NULL, ret);
	g_return_val_if_fail(CLUTTER_IS_ACTOR(scrollbar), ret);
	scrollbar_private_data_t* priv = (scrollbar_private_data_t*)g_object_get_data(G_OBJECT(scrollbar), "priv");
	g_return_val_if_fail(priv != NULL ,ret);

	return ret;	
	ClutterCrossingEvent* crossingevt = (ClutterCrossingEvent*)evt;


	gfloat x_out = 0;
	gfloat y_out = 0;
	clutter_actor_transform_stage_point(scrollbar, crossingevt->x, crossingevt->y, &x_out, &y_out);

	priv->ondrag = FALSE;
	g_debug("LEAVE");
	return ret;
} //scrollbar_leave_cb()



static ClutterActor* create_mailmenu(void)
{


	ClutterActor* mailmenu = clutter_group_new();
	mailmenu_private_data_t* priv = mailmenu_private_data_new0();
	ClutterActor** hollows  = priv->hollows;
	ClutterActor** mail_items = priv->mail_items;

	int idx;
	for(idx = 0; idx < 6+6; idx++)
	{
		hollows[idx] = create_hollow();
		clutter_actor_set_position( hollows[idx], 0,  (idx)*52);
		clutter_container_add_actor(CLUTTER_CONTAINER(mailmenu), hollows[idx]);

		if(idx < 5+6)
		{
			mail_items[idx] = create_mail_item(idx);
			clutter_actor_set_position(mail_items[idx], 4, (idx)*52+10);
			clutter_actor_set_size(mail_items[idx], 230, 52);
			clutter_container_add_actor(CLUTTER_CONTAINER(mailmenu), mail_items[idx]);
		}
	}

	for(idx = 0; idx <5+6; idx++)
	{
		g_object_set_data(G_OBJECT(mail_items[idx]), "prev_hollow", hollows[idx]);
		g_object_set_data(G_OBJECT(mail_items[idx]), "next_hollow", hollows[idx+1]);
		clutter_actor_set_reactive(mail_items[idx], TRUE);
		g_signal_connect(G_OBJECT(mail_items[idx]),"enter-event", G_CALLBACK(mailblk_mail_item_enter_cb), NULL);
		g_signal_connect(G_OBJECT(mail_items[idx]),"leave-event", G_CALLBACK(mailblk_mail_item_leave_cb), NULL);
	}

	priv->mailmenu = mailmenu;
	g_object_set_data(G_OBJECT(mailmenu), "priv", priv);
	
	return mailmenu;

} //create_mailmenu()


static mailmenu_private_data_t* mailmenu_private_data_new0(void)
{
	return g_new0(mailmenu_private_data_t, 1);
} //mailmenu_private_data_t()

static void  mailmenu_private_data_free(gpointer data)
{
	g_free(data);
} //mailmenu_private_data_free()

static gdouble circle_alpha_func(ClutterAlpha *alpha , gpointer data)
{

	ClutterTimeline *timeline = clutter_alpha_get_timeline(alpha);

	gdouble pos = clutter_timeline_get_progress(timeline);
	gdouble p = 2*(pos - 0.5);
	gdouble val = sqrt(1-p*p);
	return val;
} //circle_alpha_func

