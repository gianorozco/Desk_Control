#include <gtk/gtk.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

static void volume(GtkWidget *widget, gpointer data){
  gdouble value = gtk_range_get_value(GTK_RANGE(widget));
  char value_str[100];
  sprintf(value_str, "%f", value);
  char per[] = "%";
  strcat(value_str, per);

  pid_t pid;
  pid=fork();
  assert(pid >= 0);
  if (pid == 0){
    //child
    execl("/usr/bin/amixer", "amixer", "set", "Master", value_str, NULL);
    exit(1);
  } else if (pid > 0) {
    //parent
    g_print("Value changed to:%f\n", value);
  }
}

static void brightness(GtkWidget *widget, gpointer data){
  gdouble value = gtk_range_get_value(GTK_RANGE(widget));
  int value_trunc = (int)value;
  char value_str[100];
  sprintf(value_str, "%d", value_trunc);
  char file_path[] = "/sys/class/backlight/intel_backlight/brightness";

  pid_t pid;
  int pipefd[2];
  assert(pipe(pipefd) != -1);

  pid=fork();
  assert(pid >= 0);
  if (pid == 0){
    //child
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    execl("/usr/bin/echo", "echo", value_str, NULL);
    exit(1);
  }

  pid_t tee_pid = fork();
  assert(tee_pid >=0);
  if (tee_pid == 0){
    //child
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);

    execl("/usr/bin/tee", "tee", file_path, NULL);
    exit(1);
  } else if (tee_pid > 0) {
    //parent
    g_print("Value changed to:%f\n", value);
  }
}

static void activate(GtkApplication* app, gpointer user_data){
  GtkWidget *window;
  GtkWidget *vol_scale;
  GtkWidget *br_scale;
  GtkWidget *button;
  GtkWidget *box;
  GtkWidget *vol_label;
  GtkWidget *br_label;

  window = gtk_application_window_new(app);
  gtk_window_set_title (GTK_WINDOW(window), "Desktop Control");
  gtk_window_set_default_size(GTK_WINDOW (window), 200, 200);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);


  gtk_window_set_child (GTK_WINDOW (window), box);

  vol_label=gtk_label_new("Volume");
  gtk_box_append(GTK_BOX(box), vol_label);
  vol_scale=gtk_scale_new_with_range(0, 0, 100, 2);
  gtk_scale_set_draw_value(GTK_SCALE(vol_scale),TRUE);
  g_signal_connect(vol_scale, "value-changed", G_CALLBACK(volume), window);
  gtk_box_append(GTK_BOX(box), vol_scale);

  br_label=gtk_label_new("Brightness");
  gtk_box_append(GTK_BOX(box), br_label);
  br_scale=gtk_scale_new_with_range(0, 9600, 96000, 2);
  gtk_scale_set_draw_value(GTK_SCALE(br_scale),TRUE);
  g_signal_connect(br_scale, "value-changed", G_CALLBACK(brightness), window);
  gtk_box_append(GTK_BOX(box), br_scale);

  button = gtk_button_new_with_label("Exit");
  g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window);
  gtk_box_append(GTK_BOX (box), button);

  gtk_window_present(GTK_WINDOW (window));
}

int main(int argc, char **argv){
  GtkApplication *app;
  int status;

  app = gtk_application_new("org.gtk.desktopcontrol", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run(G_APPLICATION (app), argc, argv);
  g_object_unref(app);

  return status;
}
