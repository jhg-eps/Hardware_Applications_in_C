#include <gtk/gtk.h>	

// This is a program adapted from a "Hello, World" tutorial written by 
// people at the GNOME project
// The link to the tutorial:
// https://developer.gnome.org/gtk-tutorial/stable/c39.html#SEC-HELLOWORLD
// Please see http://www.gtk.org/tutorial1.2/gtk_tut-2.html, it does a 
// great job of explaining GTK

// COMPILING THE PROGRAM
// $ gcc LAB1.c -o LAB1 `pkg-config --cflags --libs gtk+-3.0`

// RUNNING THE PROGRAM
// $ ./LAB1

/* Callback function declarations and implementations */
/* Callback functions are functions called by other functions). The 
"data arguments are ignored" in this case.*/

 static void hello( GtkWidget *widget, gpointer   data )
{
    g_print ("Hello, world, from the command line.\n"); // prints on the 
							// console
}

static gboolean delete_event( GtkWidget *widget, GdkEvent  *event, gpointer data )
{
   // Returning FALSE tells GTK to emit the "destroy" signal, 
   // while returning TRUE does not destroy the window/widget
   g_print ("delete event occurred\n");
   return TRUE;
}

static void destroy( GtkWidget *widget, gpointer   data )
{
    gtk_main_quit ();   // this is what actually closes everything out, 
			// erasing the window and its associated objects
			// from existence
}

int main( int   argc, char *argv[] )
{
    // GtkWidget is a generic class for GTK objects 
    GtkWidget *window;
    GtkWidget *button;
    
// gtk_init must always be called. Command line arguments are parsed 
// and then passed (returned) to the executable (application) 
// and the GIMP toolkit is initialized 
   gtk_init (&argc, &argv);
    
    /* create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL); // A 200x200 pixel 
                                                   // window is created, 
                                                   // with the basic window 
                                                   // dressings (e.g., an 
						   // "x" button)				
    
    /* when the window manager creates a "delete event" (usuallu by the "x" 
buttton being clicked
     * by the window manager, usually by the "close" option, or on the
     * titlebar), we ask it to call the delete_event () function
     * as defined above. The data passed to the callback
     * function is NULL and is ignored in the callback function. */
    g_signal_connect (window, 
"delete-event", G_CALLBACK 
(gtk_widget_destroy), NULL);
    
    /* Here we connect the "destroy" event to a signal handler.  
     * This event occurs when we call gtk_widget_destroy() on the window,
     * or if we return FALSE in the "delete-event" callback. */
    g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);
    
    /* Sets the border width of the window. */
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    
    /* Creates a new button with the label "Hello World". */
    button = gtk_button_new_with_label ("Click here for a surprise.");
    
    /* When the button receives the "clicked" signal, it will call the
     * function hello() passing it NULL as its argument.  The hello()
     * function is defined above. */
    g_signal_connect (button, "clicked",G_CALLBACK (hello), NULL);
    
    /* This will cause the window to be destroyed by calling
     * gtk_widget_destroy(window) when "clicked".  Again, the destroy
     * signal could come from here, or the window manager. */
   g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
    
    /* This packs the button into the window (a gtk container). */
    gtk_container_add (GTK_CONTAINER (window), button);
    
    /* The final step is to display this newly created widget. */
    gtk_widget_show (button);
    
    /* and the window */
    gtk_widget_show (window);
    
    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or
     * mouse event). */
    gtk_main ();
    
    return 0;
}
