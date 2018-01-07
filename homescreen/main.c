#include <stdio.h>
#include <stdlib.h>
#include <gmodule.h>
#include <clutter/clutter.h>
#include <gtk/gtk.h>
#include <clutter-gtk/clutter-gtk.h>
#include "dock.h"
#include "webblk.h"
#include "calblk.h"
#include "todoblk.h"
#include "mailblk.h"
#include "clutter-behaviour-scale-gravity.h"

#define WINDOW_FULL
//#define GTK_ACTION_SIMULATE
#ifdef GTK_ACTION_SIMULATE
#	undef WINDOW_FULL
#endif


#define BACKGROUND_FILEPATH ("BG.gif")
#define BORDER_FILEPATH ("over.png")
#define WEBBLK_BG_FILEPATH ("wc_BG.png")
#define WEBBLK_BG_SHADOW_FILEPATH ("wc_shadow.png")



static ClutterActor* create_main_actor(int* argc, char** argv[]);




/* function for */
int main(int argc, char* argv[] )
{

	GtkWidget* window;
	GtkWidget* clutter;
	GtkWidget* vbox;
	GtkWidget* hbox;
	GtkWidget* gtkhello;
	ClutterActor* stage;
	ClutterActor* clutterhello;
	ClutterActor* main_actor;


	if (gtk_clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
		g_error ("Unable to initialize GtkClutter");
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE); 
	gtk_window_move(GTK_WINDOW(window), 0, 0);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	clutter = gtk_clutter_embed_new ();
	gtk_widget_set_size_request(clutter, 1024, 600);
	
	//gtk_container_add(GTK_CONTAINER(window), clutter);
	stage = gtk_clutter_embed_get_stage (GTK_CLUTTER_EMBED (clutter));

	gtk_window_set_title(GTK_WINDOW(window), "Hello World! Clutter-Gtk" );
	hbox = gtk_hbox_new(TRUE, 5);
#ifndef WINDOW_FULL
	gtk_container_add(GTK_CONTAINER(window), hbox);
#else
	gtk_container_add(GTK_CONTAINER(window), clutter);
#endif //WINDOW_FULL

#if 1
	GtkWidget* boxtmp = gtk_vbox_new(TRUE, 5);
	gtk_box_pack_start(GTK_BOX(boxtmp), clutter, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), boxtmp  ,TRUE, TRUE, 5);
#else
	gtk_box_pack_start(GTK_BOX(hbox), clutter, TRUE, TRUE, 5);
#endif 


#ifdef GTK_ACTION_SIMULATE
	vbox = gtk_vbox_new(TRUE, 5);
	
  do
  {
	GtkWidget* btn1 = gtk_button_new_with_label("effect 1");
	GtkWidget* btn2 = gtk_button_new_with_label("effect 2");
	GtkWidget* btn3 = gtk_button_new_with_label("effect 3");
	GtkWidget* btn4 = gtk_button_new_with_label("effect 4");
	gtk_box_pack_start(GTK_BOX(vbox), btn1, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), btn2, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), btn3, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox), btn4, TRUE, TRUE, 5);

  }while(0);

	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 5);
#endif  //#ifdef GTK_ACTION_SIMULATE


	main_actor = create_main_actor(&argc, &argv);
	clutter_container_add_actor(CLUTTER_CONTAINER(stage), main_actor);

	gtk_widget_show_all(window);
	clutter_actor_show(stage);
	gtk_main();
} //main()





static ClutterActor* create_main_actor(int* argc, char** argv[])
{

	ClutterActor* title; //ClutterText
	ClutterActor* background; //ClutterTexture
	ClutterActor* dock; //ClutterGroup  by create_dock()
	ClutterActor* webblk; //ClutterGroup by create_webblk()
	ClutterActor* calblk; //ClutterGroup by create_calblk()
	ClutterActor* todoblk; //ClutterGroup by create_todoblk()
	ClutterActor* mailblk; //ClutterGroup by create_mailblk()


	ClutterActor* main_actor;

	main_actor = clutter_group_new();
	title  = clutter_text_new_with_text("Mono 12", " ");

	GError* gerr = NULL;
	background = clutter_texture_new_from_file(BACKGROUND_FILEPATH, &gerr);
	g_return_val_if_fail(gerr==NULL, NULL);

	dock = create_dock();
	calblk = create_calblk();
	todoblk = create_todoblk();
	webblk = create_webblk();
	mailblk = create_mailblk();

	clutter_actor_show_all(dock);
	clutter_actor_show_all(calblk);
	clutter_actor_show_all(todoblk);
	clutter_actor_show_all(webblk);
	clutter_actor_show_all(mailblk);


	clutter_actor_set_position(title, 0,0);
	clutter_actor_set_position(background, 0,0);
	clutter_actor_set_position(dock, 300,500);

	clutter_actor_set_position(calblk, 65,104);
	clutter_actor_set_position(todoblk, 372,104);
	clutter_actor_set_position(webblk, 679,104);
	clutter_actor_set_position(mailblk, 372,104);



#if 0	
	clutter_container_add(CLUTTER_CONTAINER(main_actor),background,title,dock,NULL);
#else
	/* clutter_container_add(CLUTTER_CONTAINER(main_actor),background,title,calblk, webblk, todoblk, dock,  NULL); */
	clutter_container_add(CLUTTER_CONTAINER(main_actor),background,title,calblk, webblk, mailblk, dock,  NULL);
#endif 

	return main_actor;
} //create_main_actor()



