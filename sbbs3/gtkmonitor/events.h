#ifndef _EVENTS_H_
#define _EVENTS_H_

#include <gtk/gtk.h>

void on_guru_brain1_activate(GtkWidget *wiggy, gpointer data);
void on_text_strings1_activate(GtkWidget *wiggy, gpointer data);
void on_default_colours1_activate(GtkWidget *wiggy, gpointer data);
void on_nodes_full_message1_activate(GtkWidget *wiggy, gpointer data);
void on_answer_screen1_activate(GtkWidget *wiggy, gpointer data);
void on_logon_message1_activate(GtkWidget *wiggy, gpointer data);
void on_auto_message1_activate(GtkWidget *wiggy, gpointer data);
void on_zip_file_comment1_activate(GtkWidget *wiggy, gpointer data);
void on_system_information1_activate(GtkWidget *wiggy, gpointer data);
void on_new_user_message1_activate(GtkWidget *wiggy, gpointer data);
void on_new_user_welcome_email1_activate(GtkWidget *wiggy, gpointer data);
void on_new_user_password_failure1_activate(GtkWidget *wiggy, gpointer data);
void on_new_user_feedbakc_instructions1_activate(GtkWidget *wiggy, gpointer data);
void on_allowed_rlogin_list1_activate(GtkWidget *wiggy, gpointer data);
void on_alias_list1_activate(GtkWidget *wiggy, gpointer data);
void on_domain_list1_activate(GtkWidget *wiggy, gpointer data);
void on_spam_bait_list1_activate(GtkWidget *wiggy, gpointer data);
void on_spam_block_list1_activate(GtkWidget *wiggy, gpointer data);
void on_allowed_relay_list1_activate(GtkWidget *wiggy, gpointer data);
void on_dnsbased_blacklists1_activate(GtkWidget *wiggy, gpointer data);
void on_dnsblacklist_exempt_ips1_activate(GtkWidget *wiggy, gpointer data);
void on_external_mail_processing1_activate(GtkWidget *wiggy, gpointer data);
void on_login_message1_activate(GtkWidget *wiggy, gpointer data);
void on_hello_message1_activate(GtkWidget *wiggy, gpointer data);
void on_goodbye_message1_activate(GtkWidget *wiggy, gpointer data);
void on_filename_aliases1_activate(GtkWidget *wiggy, gpointer data);
void on_mime_types1_activate(GtkWidget *wiggy, gpointer data);
void on_cgi_environment_variables1_activate(GtkWidget *wiggy, gpointer data);
void on_external_content_handlers1_activate(GtkWidget *wiggy, gpointer data);
void on_servicesini1_activate(GtkWidget *wiggy, gpointer data);

void on_error_log1_activate(GtkWidget *wiggy, gpointer data);
void on_statistics_log1_activate(GtkWidget *wiggy, gpointer data);
void on_todays_log1_activate(GtkWidget *wiggy, gpointer data);
void on_yesterdays_log1_activate(GtkWidget *wiggy, gpointer data);
void on_another_days_log1_activate(GtkWidget *wiggy, gpointer data);
void on_spam_log1_activate(GtkWidget *wiggy, gpointer data);
void on_todays_log2_activate(GtkWidget *wiggy, gpointer data);
void on_yesterdays_log2_activate(GtkWidget *wiggy, gpointer data);
void on_another_days_log2_activate(GtkWidget *wiggy, gpointer data);
void on_todays_log3_activate(GtkWidget *wiggy, gpointer data);
void on_yesterdays_log3_activate(GtkWidget *wiggy, gpointer data);
void on_another_days_log3_activate(GtkWidget *wiggy, gpointer data);


#endif
