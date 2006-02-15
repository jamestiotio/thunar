/* $Id$ */
/*-
 * Copyright (c) 2005 Benedikt Meurer <benny@xfce.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <glib/gi18n-lib.h>

#include <exo/exo.h>

#include <thunar-uca/thunar-uca-editor.h>



static void           thunar_uca_editor_class_init      (ThunarUcaEditorClass   *klass);
static void           thunar_uca_editor_init            (ThunarUcaEditor        *uca_editor);
static void           thunar_uca_editor_finalize        (GObject                *object);
static const gchar   *thunar_uca_editor_get_icon_name   (const ThunarUcaEditor  *uca_editor);
static void           thunar_uca_editor_set_icon_name   (ThunarUcaEditor        *uca_editor,
                                                         const gchar            *icon_name);
static ThunarUcaTypes thunar_uca_editor_get_types       (const ThunarUcaEditor  *uca_editor);
static void           thunar_uca_editor_set_types       (ThunarUcaEditor        *uca_editor,
                                                         ThunarUcaTypes          types);
static void           thunar_uca_editor_command_clicked (ThunarUcaEditor        *uca_editor);
static void           thunar_uca_editor_icon_clicked    (ThunarUcaEditor        *uca_editor);



struct _ThunarUcaEditorClass
{
  GtkDialogClass __parent__;
};

struct _ThunarUcaEditor
{
  GtkDialog __parent__;

  GtkTooltips *tooltips;

  GtkWidget   *name_entry;
  GtkWidget   *description_entry;
  GtkWidget   *icon_button;
  GtkWidget   *command_entry;
  GtkWidget   *parameter_entry;
  GtkWidget   *patterns_entry;
  GtkWidget   *directories_button;
  GtkWidget   *audio_files_button;
  GtkWidget   *image_files_button;
  GtkWidget   *text_files_button;
  GtkWidget   *video_files_button;
  GtkWidget   *other_files_button;
};



THUNARX_DEFINE_TYPE (ThunarUcaEditor, thunar_uca_editor, GTK_TYPE_DIALOG);



static void
thunar_uca_editor_class_init (ThunarUcaEditorClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = thunar_uca_editor_finalize;
}



static void
thunar_uca_editor_init (ThunarUcaEditor *uca_editor)
{
  AtkRelationSet *relations;
  PangoAttribute *attribute;
  PangoAttrList  *attrs_small_bold;
  PangoAttrList  *attrs_small;
  AtkRelation    *relation;
  AtkObject      *object;
  GtkWidget      *notebook;
  GtkWidget      *button;
  GtkWidget      *itable;
  GtkWidget      *align;
  GtkWidget      *image;
  GtkWidget      *label;
  GtkWidget      *table;
  GtkWidget      *hbox;
  GtkWidget      *vbox;

  /* configure the dialog properties */
  gtk_dialog_add_button (GTK_DIALOG (uca_editor), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
  gtk_dialog_add_button (GTK_DIALOG (uca_editor), GTK_STOCK_OK, GTK_RESPONSE_OK);
  gtk_dialog_set_default_response (GTK_DIALOG (uca_editor), GTK_RESPONSE_OK);
  gtk_dialog_set_has_separator (GTK_DIALOG (uca_editor), FALSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (uca_editor), TRUE);
  gtk_window_set_resizable (GTK_WINDOW (uca_editor), FALSE);

  /* allocate shared tooltips object */
  uca_editor->tooltips = gtk_tooltips_new ();
  g_object_ref (G_OBJECT (uca_editor->tooltips));
  gtk_object_sink (GTK_OBJECT (uca_editor->tooltips));

  notebook = gtk_notebook_new ();
  gtk_container_set_border_width (GTK_CONTAINER (notebook), 6);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (uca_editor)->vbox), notebook, TRUE, TRUE, 0);
  gtk_widget_show (notebook);

  /*
     Basic
   */
  label = gtk_label_new (_("Basic"));
  table = gtk_table_new (6, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), 12);
  gtk_table_set_row_spacings (GTK_TABLE (table), 6);
  gtk_container_set_border_width (GTK_CONTAINER (table), 12);
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), table, label);
  gtk_widget_show (label);
  gtk_widget_show (table);

  label = g_object_new (GTK_TYPE_LABEL, "label", _("_Name:"), "use-underline", TRUE, "xalign", 0.0f, NULL);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  uca_editor->name_entry = g_object_new (GTK_TYPE_ENTRY, "activates-default", TRUE, NULL);
  gtk_tooltips_set_tip (uca_editor->tooltips, uca_editor->name_entry, _("The name of the action that will be displayed in the context menu."), NULL);
  gtk_table_attach (GTK_TABLE (table), uca_editor->name_entry, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), uca_editor->name_entry);
  gtk_widget_grab_focus (uca_editor->name_entry);
  gtk_widget_show (uca_editor->name_entry);

  /* set Atk label relation for the entry */
  object = gtk_widget_get_accessible (uca_editor->name_entry);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  label = g_object_new (GTK_TYPE_LABEL, "label", _("_Description:"), "use-underline", TRUE, "xalign", 0.0f, NULL);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  uca_editor->description_entry = g_object_new (GTK_TYPE_ENTRY, "activates-default", TRUE, NULL);
  gtk_tooltips_set_tip (uca_editor->tooltips, uca_editor->description_entry, _("The description of the action that will be displayed as tooltip "
                                                                               "in the statusbar when selecting the item from the context menu."), NULL);
  gtk_table_attach (GTK_TABLE (table), uca_editor->description_entry, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), uca_editor->description_entry);
  gtk_widget_show (uca_editor->description_entry);

  /* set Atk label relation for the entry */
  object = gtk_widget_get_accessible (uca_editor->description_entry);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  label = g_object_new (GTK_TYPE_LABEL, "label", _("_Command:"), "use-underline", TRUE, "xalign", 0.0f, NULL);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (hbox);

  uca_editor->command_entry = g_object_new (GTK_TYPE_ENTRY, "activates-default", TRUE, NULL);
  gtk_tooltips_set_tip (uca_editor->tooltips, uca_editor->command_entry, _("The command (including the necessary parameters) to perform the action. "
                                                                           "See the command parameter legend below for a list of supported parameter "
                                                                           "variables, which will be substituted when launching the command. When "
                                                                           "upper-case letters (e.g. %F, %D, %N) are used, the action will be applicable "
                                                                           "even if more than one item is selected. Else the action will only be "
                                                                           "applicable if exactly one item is selected."), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), uca_editor->command_entry, TRUE, TRUE, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), uca_editor->command_entry);
  gtk_widget_show (uca_editor->command_entry);

  button = gtk_button_new ();
  gtk_tooltips_set_tip (uca_editor->tooltips, button, _("Browse the file system to select an application to use for this action."), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  g_signal_connect_swapped (G_OBJECT (button), "clicked", G_CALLBACK (thunar_uca_editor_command_clicked), uca_editor);
  gtk_widget_show (button);

  image = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
  gtk_container_add (GTK_CONTAINER (button), image);
  gtk_widget_show (image);

  /* set Atk label relation for the entry */
  object = gtk_widget_get_accessible (uca_editor->command_entry);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  label = g_object_new (GTK_TYPE_LABEL, "label", _("_Icon:"), "use-underline", TRUE, "xalign", 0.0f, NULL);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  align = gtk_alignment_new (0.0f, 0.5f, 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), align, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (align);

  uca_editor->icon_button = gtk_button_new_with_label (_("No icon"));
  gtk_tooltips_set_tip (uca_editor->tooltips, uca_editor->icon_button, _("Click this button to select an icon file that will be displayed "
                                                                         "in the context menu in addition to the action name chosen above."), NULL);
  gtk_container_add (GTK_CONTAINER (align), uca_editor->icon_button);
  g_signal_connect_swapped (G_OBJECT (uca_editor->icon_button), "clicked", G_CALLBACK (thunar_uca_editor_icon_clicked), uca_editor);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), uca_editor->icon_button);
  gtk_widget_show (uca_editor->icon_button);

  /* set Atk label relation for the button */
  object = gtk_widget_get_accessible (uca_editor->icon_button);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  align = g_object_new (GTK_TYPE_ALIGNMENT, "height-request", 12, NULL);
  gtk_table_attach (GTK_TABLE (table), align, 0, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (align);

  hbox = gtk_hbox_new (FALSE, 6);
  gtk_table_attach (GTK_TABLE (table), hbox, 0, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (hbox);

  image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DND);
  gtk_misc_set_alignment (GTK_MISC (image), 0.5f, 0.0f);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
  gtk_widget_show (image);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
  gtk_widget_show (vbox);

  label = gtk_label_new (_("The following command parameters will be\n"
                           "substituted when launching the action:"));
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  table = gtk_table_new (2, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), 6);
  gtk_table_set_row_spacings (GTK_TABLE (table), 3);
  gtk_container_set_border_width (GTK_CONTAINER (table), 6);
  gtk_box_pack_start (GTK_BOX (vbox), table, TRUE, TRUE, 0);
  gtk_widget_show (table);

  /* setup the small+bold attribute list */
  attrs_small_bold = pango_attr_list_new ();
  attribute = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
  attribute->start_index = 0;
  attribute->end_index = -1;
  pango_attr_list_insert (attrs_small_bold, attribute);
  attribute = pango_attr_scale_new (PANGO_SCALE_SMALL);
  attribute->start_index = 0;
  attribute->end_index = -1;
  pango_attr_list_insert (attrs_small_bold, attribute);

  /* setup the small attribute list */
  attrs_small = pango_attr_list_new ();
  attribute = pango_attr_scale_new (PANGO_SCALE_SMALL);
  attribute->start_index = 0;
  attribute->end_index = -1;
  pango_attr_list_insert (attrs_small, attribute);

  label = gtk_label_new ("%f");
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small_bold);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  label = gtk_label_new (_("the path to the first selected file"));
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  label = gtk_label_new ("%F");
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small_bold);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  label = gtk_label_new (_("the paths to all selected files"));
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  label = gtk_label_new ("%d");
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small_bold);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  label = gtk_label_new (_("directory containing the file that is passed in %f"));
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  label = gtk_label_new ("%D");
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small_bold);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  label = gtk_label_new (_("directories containing the files that are passed in %F"));
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  label = gtk_label_new ("%n");
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small_bold);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  label = gtk_label_new (_("the first selected filename (without path)"));
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  label = gtk_label_new ("%N");
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small_bold);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 5, 6, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  label = gtk_label_new (_("the selected filenames (without paths)"));
  gtk_label_set_attributes (GTK_LABEL (label), attrs_small);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_table_attach (GTK_TABLE (table), label, 1, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  /* release the pango attribute lists */
  pango_attr_list_unref (attrs_small_bold);
  pango_attr_list_unref (attrs_small);

  /*
     Appearance Conditions
   */
  table = gtk_table_new (3, 2, FALSE);
  label = gtk_label_new (_("Appearance Conditions"));
  gtk_table_set_col_spacings (GTK_TABLE (table), 12);
  gtk_table_set_row_spacings (GTK_TABLE (table), 6);
  gtk_container_set_border_width (GTK_CONTAINER (table), 12);
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), table, label);
  gtk_widget_show (label);
  gtk_widget_show (table);

  label = g_object_new (GTK_TYPE_LABEL, "label", _("_File Pattern:"), "use-underline", TRUE, "xalign", 0.0f, NULL);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  uca_editor->patterns_entry = g_object_new (GTK_TYPE_ENTRY, "activates-default", TRUE, "text", "*", NULL);
  gtk_tooltips_set_tip (uca_editor->tooltips, uca_editor->patterns_entry, _("Enter a list of patterns that will be used to determine "
                                                                            "whether this action should be displayed for a selected "
                                                                            "file. If you specify more than one pattern here, the list "
                                                                            "items must be separated with semicolons (e.g. *.txt;*.doc)."), NULL);
  gtk_table_attach (GTK_TABLE (table), uca_editor->patterns_entry, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), uca_editor->patterns_entry);
  gtk_widget_show (uca_editor->patterns_entry);

  /* set Atk label relation for the entry */
  object = gtk_widget_get_accessible (uca_editor->patterns_entry);
  relations = atk_object_ref_relation_set (gtk_widget_get_accessible (label));
  relation = atk_relation_new (&object, 1, ATK_RELATION_LABEL_FOR);
  atk_relation_set_add (relations, relation);
  g_object_unref (G_OBJECT (relation));

  align = g_object_new (GTK_TYPE_ALIGNMENT, "height-request", 0, NULL);
  gtk_table_attach (GTK_TABLE (table), align, 0, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (align);

  label = g_object_new (GTK_TYPE_LABEL, "label", _("Appears if selection contains:"), "xalign", 0.0f, NULL);
  gtk_table_attach (GTK_TABLE (table), label, 0, 2, 2, 3, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (label);

  align = g_object_new (GTK_TYPE_ALIGNMENT, "left-padding", 18, NULL);
  gtk_table_attach (GTK_TABLE (table), align, 0, 2, 3, 4, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (align);

  itable = gtk_table_new (3, 2, TRUE);
  gtk_table_set_col_spacings (GTK_TABLE (itable), 12);
  gtk_container_add (GTK_CONTAINER (align), itable);
  gtk_widget_show (itable);

  uca_editor->directories_button = gtk_check_button_new_with_mnemonic (_("_Directories"));
  gtk_table_attach (GTK_TABLE (itable), uca_editor->directories_button, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (uca_editor->directories_button);

  uca_editor->audio_files_button = gtk_check_button_new_with_mnemonic (_("_Audio Files"));
  gtk_table_attach (GTK_TABLE (itable), uca_editor->audio_files_button, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (uca_editor->audio_files_button);

  uca_editor->image_files_button = gtk_check_button_new_with_mnemonic (_("_Image Files"));
  gtk_table_attach (GTK_TABLE (itable), uca_editor->image_files_button, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (uca_editor->image_files_button);

  uca_editor->text_files_button = gtk_check_button_new_with_mnemonic (_("_Text Files"));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (uca_editor->text_files_button), TRUE);
  gtk_table_attach (GTK_TABLE (itable), uca_editor->text_files_button, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (uca_editor->text_files_button);

  uca_editor->video_files_button = gtk_check_button_new_with_mnemonic (_("_Video Files"));
  gtk_table_attach (GTK_TABLE (itable), uca_editor->video_files_button, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (uca_editor->video_files_button);

  uca_editor->other_files_button = gtk_check_button_new_with_mnemonic (_("_Other Files"));
  gtk_table_attach (GTK_TABLE (itable), uca_editor->other_files_button, 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (uca_editor->other_files_button);

  align = g_object_new (GTK_TYPE_ALIGNMENT, "height-request", 12, NULL);
  gtk_table_attach (GTK_TABLE (table), align, 0, 2, 4, 5, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (align);

  hbox = gtk_hbox_new (FALSE, 6);
  gtk_table_attach (GTK_TABLE (table), hbox, 0, 2, 5, 6, GTK_EXPAND | GTK_FILL, GTK_FILL, 0, 0);
  gtk_widget_show (hbox);

  image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DND);
  gtk_misc_set_alignment (GTK_MISC (image), 0.5f, 0.0f);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
  gtk_widget_show (image);

  label = gtk_label_new (_("This page lists the conditions under which the\n"
                           "action will appear in the file managers context\n"
                           "menus. The file patterns are specified as a list\n"
                           "of simple file patterns separated by semicolons\n"
                           "(e.g. *.txt;*.doc). For an action to appear in the\n"
                           "context menu of a file or folder, atleast one of\n"
                           "these patterns must match the name of the file\n"
                           "or folder. Additionally, you can specify that the\n"
                           "action should only appear for certain kinds of\nfiles."));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0f, 0.0f);
  gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
  gtk_widget_show (label);
}



static void
thunar_uca_editor_finalize (GObject *object)
{
  ThunarUcaEditor *uca_editor = THUNAR_UCA_EDITOR (object);

  /* release the tooltips */
  g_object_unref (G_OBJECT (uca_editor->tooltips));

  (*G_OBJECT_CLASS (thunar_uca_editor_parent_class)->finalize) (object);
}



static void
thunar_uca_editor_command_clicked (ThunarUcaEditor *uca_editor)
{
  GtkFileFilter *filter;
  GtkWidget     *chooser;
  gchar         *filename;
  gchar         *s;

  g_return_if_fail (THUNAR_UCA_IS_EDITOR (uca_editor));

  chooser = gtk_file_chooser_dialog_new (_("Select an Application"),
                                         GTK_WINDOW (uca_editor),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                         NULL);
  gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER (chooser), TRUE);

  /* add file chooser filters */
  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("All Files"));
  gtk_file_filter_add_pattern (filter, "*");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), filter);

  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("Executable Files"));
  gtk_file_filter_add_mime_type (filter, "application/x-csh");
  gtk_file_filter_add_mime_type (filter, "application/x-executable");
  gtk_file_filter_add_mime_type (filter, "application/x-perl");
  gtk_file_filter_add_mime_type (filter, "application/x-python");
  gtk_file_filter_add_mime_type (filter, "application/x-ruby");
  gtk_file_filter_add_mime_type (filter, "application/x-shellscript");
  gtk_file_filter_add_pattern (filter, "*.pl");
  gtk_file_filter_add_pattern (filter, "*.py");
  gtk_file_filter_add_pattern (filter, "*.rb");
  gtk_file_filter_add_pattern (filter, "*.sh");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), filter);
  gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (chooser), filter);

  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("Perl Scripts"));
  gtk_file_filter_add_mime_type (filter, "application/x-perl");
  gtk_file_filter_add_pattern (filter, "*.pl");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), filter);

  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("Python Scripts"));
  gtk_file_filter_add_mime_type (filter, "application/x-python");
  gtk_file_filter_add_pattern (filter, "*.py");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), filter);

  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("Ruby Scripts"));
  gtk_file_filter_add_mime_type (filter, "application/x-ruby");
  gtk_file_filter_add_pattern (filter, "*.rb");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), filter);

  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("Shell Scripts"));
  gtk_file_filter_add_mime_type (filter, "application/x-csh");
  gtk_file_filter_add_mime_type (filter, "application/x-shellscript");
  gtk_file_filter_add_pattern (filter, "*.sh");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), filter);

  /* use the bindir as default folder */
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (chooser), BINDIR);

  /* setup the currently selected file */
  filename = gtk_editable_get_chars (GTK_EDITABLE (uca_editor->command_entry), 0, -1);
  if (G_LIKELY (filename != NULL))
    {
      /* use only the first argument */
      s = strchr (filename, ' ');
      if (G_UNLIKELY (s != NULL))
        *s = '\0';

      if (G_LIKELY (*filename != '\0'))
        gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (chooser), filename);
      g_free (filename);
    }

  /* run the chooser dialog */
  if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_ACCEPT)
    {
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
      s = g_strconcat (filename, " %f", NULL);
      gtk_entry_set_text (GTK_ENTRY (uca_editor->command_entry), s);
      g_free (filename);
      g_free (s);
    }

  gtk_widget_destroy (chooser);
}



static void
update_preview (GtkFileChooser *chooser,
                GtkWidget      *image)
{
  GdkPixbuf *pixbuf;
  GdkPixbuf *scaled;
  gchar     *filename;
  gint       height;
  gint       width;

  filename = gtk_file_chooser_get_filename (chooser);
  pixbuf = (filename != NULL) ? gdk_pixbuf_new_from_file (filename, NULL) : NULL;
  if (G_LIKELY (pixbuf != NULL))
    {
      width = gdk_pixbuf_get_width (pixbuf);
      height = gdk_pixbuf_get_height (pixbuf);
      if (G_UNLIKELY (width > 128 || height > 128))
        {
          scaled = exo_gdk_pixbuf_scale_ratio (pixbuf, 128);
          g_object_unref (G_OBJECT (pixbuf));
          pixbuf = scaled;
        }

      gtk_file_chooser_set_preview_widget_active (chooser, TRUE);
      gtk_image_set_from_pixbuf (GTK_IMAGE (image), pixbuf);
      g_object_unref (G_OBJECT (pixbuf));
    }
  else
    {
      gtk_file_chooser_set_preview_widget_active (chooser, FALSE);
    }

  g_free (filename);
}



static void
thunar_uca_editor_icon_clicked (ThunarUcaEditor *uca_editor)
{
  GtkFileFilter *filter;
  GtkWidget     *chooser;
  GtkWidget     *image;
  gchar        **extensions;
  gchar        **mime_types;
  gchar         *filename;
  gchar         *pattern;
  GSList        *formats;
  GSList        *lp;
  gint           n;

  g_return_if_fail (THUNAR_UCA_IS_EDITOR (uca_editor));

  chooser = gtk_file_chooser_dialog_new (_("Select an Icon"),
                                         GTK_WINDOW (uca_editor),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                         NULL);
  gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER (chooser), TRUE);

  /* add file chooser filters */
  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("All Files"));
  gtk_file_filter_add_pattern (filter, "*");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), filter);

  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, _("Image Files"));
  formats = gdk_pixbuf_get_formats ();
  for (lp = formats; lp != NULL; lp = lp->next)
    {
      extensions = gdk_pixbuf_format_get_extensions (lp->data);
      for (n = 0; extensions[n] != NULL; ++n)
        {
          pattern = g_strconcat ("*.", extensions[n], NULL);
          gtk_file_filter_add_pattern (filter, pattern);
          g_free (pattern);
        }
      g_strfreev (extensions);

      mime_types = gdk_pixbuf_format_get_mime_types (lp->data);
      for (n = 0; mime_types[n] != NULL; ++n)
        gtk_file_filter_add_mime_type (filter, mime_types[n]);
      g_strfreev (mime_types);
    }
  g_slist_free (formats);
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), filter);
  gtk_file_chooser_set_filter (GTK_FILE_CHOOSER (chooser), filter);

  image = g_object_new (GTK_TYPE_IMAGE, "height-request", 128 + 2 * 12, "width-request", 128 + 2 * 12, NULL);
  g_signal_connect (G_OBJECT (chooser), "update-preview", G_CALLBACK (update_preview), image);
  gtk_file_chooser_set_preview_widget_active (GTK_FILE_CHOOSER (chooser), FALSE);
  gtk_file_chooser_set_preview_widget (GTK_FILE_CHOOSER (chooser), image);
  gtk_widget_show (image);

  /* use the datadir/pixmaps as default folder */
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (chooser), DATADIR "/pixmaps");

  /* setup the currently selected icon file */
  filename = g_object_get_data (G_OBJECT (uca_editor->icon_button), "thunar-uca-icon-filename");
  if (G_LIKELY (filename != NULL && *filename != '\0'))
    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (chooser), filename);

  /* run the chooser dialog */
  if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_ACCEPT)
    {
      /* remember the selected file for the icon */
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
      thunar_uca_editor_set_icon_name (uca_editor, filename);
      g_free (filename);
    }

  gtk_widget_destroy (chooser);
}



static const gchar*
thunar_uca_editor_get_icon_name (const ThunarUcaEditor *uca_editor)
{
  g_return_val_if_fail (THUNAR_UCA_IS_EDITOR (uca_editor), NULL);
  return g_object_get_data (G_OBJECT (uca_editor->icon_button), "thunar-uca-icon-name");
}



static void
thunar_uca_editor_set_icon_name (ThunarUcaEditor *uca_editor,
                                 const gchar     *icon_name)
{
  GtkIconTheme *icon_theme;
  GdkPixbuf    *icon_scaled;
  GdkPixbuf    *icon = NULL;
  GtkWidget    *image;
  GtkWidget    *label;
  gint          icon_height;
  gint          icon_width;

  g_return_if_fail (THUNAR_UCA_IS_EDITOR (uca_editor));

  /* drop the previous button child */
  if (GTK_BIN (uca_editor->icon_button)->child != NULL)
    gtk_widget_destroy (GTK_BIN (uca_editor->icon_button)->child);

  /* try to load the icon */
  if (G_LIKELY (icon_name != NULL && g_path_is_absolute (icon_name)))
    {
      /* load the icon from the file */
      icon = gdk_pixbuf_new_from_file (icon_name, NULL);
    }
  else if (icon_name != NULL)
    {
      /* determine the appropriate icon theme */
      if (G_LIKELY (gtk_widget_has_screen (GTK_WIDGET (uca_editor))))
        icon_theme = gtk_icon_theme_get_for_screen (gtk_widget_get_screen (GTK_WIDGET (uca_editor)));
      else
        icon_theme = gtk_icon_theme_get_default ();

      /* try to load the named icon */
      icon = gtk_icon_theme_load_icon (icon_theme, icon_name, 48, 0, NULL);
    }

  /* setup the icon button */
  if (G_LIKELY (icon != NULL))
    {
      /* scale down the icon if required */
      icon_width = gdk_pixbuf_get_width (icon);
      icon_height = gdk_pixbuf_get_height (icon);
      if (G_UNLIKELY (icon_width > 48 || icon_height > 48))
        {
          icon_scaled = exo_gdk_pixbuf_scale_ratio (icon, 48);
          g_object_unref (G_OBJECT (icon));
          icon = icon_scaled;
        }

      /* setup an image for the icon */
      image = gtk_image_new_from_pixbuf (icon);
      gtk_container_add (GTK_CONTAINER (uca_editor->icon_button), image);
      gtk_widget_show (image);

      /* remember the name for the icon */
      g_object_set_data_full (G_OBJECT (uca_editor->icon_button), "thunar-uca-icon-name", g_strdup (icon_name), g_free);

      /* release the icon */
      g_object_unref (G_OBJECT (icon));
    }
  else
    {
      /* reset the remembered icon name */
      g_object_set_data (G_OBJECT (uca_editor->icon_button), "thunar-uca-icon-name", NULL);

      /* setup a label to tell that no icon was selected */
      label = gtk_label_new (_("No icon"));
      gtk_container_add (GTK_CONTAINER (uca_editor->icon_button), label);
      gtk_widget_show (label);
    }
}



static ThunarUcaTypes
thunar_uca_editor_get_types (const ThunarUcaEditor *uca_editor)
{
  ThunarUcaTypes types = 0;

  g_return_val_if_fail (THUNAR_UCA_IS_EDITOR (uca_editor), 0);

  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (uca_editor->directories_button)))
    types |= THUNAR_UCA_TYPE_DIRECTORIES;
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (uca_editor->audio_files_button)))
    types |= THUNAR_UCA_TYPE_AUDIO_FILES;
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (uca_editor->image_files_button)))
    types |= THUNAR_UCA_TYPE_IMAGE_FILES;
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (uca_editor->text_files_button)))
    types |= THUNAR_UCA_TYPE_TEXT_FILES;
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (uca_editor->video_files_button)))
    types |= THUNAR_UCA_TYPE_VIDEO_FILES;
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (uca_editor->other_files_button)))
    types |= THUNAR_UCA_TYPE_OTHER_FILES;

  return types;
}



static void
thunar_uca_editor_set_types (ThunarUcaEditor *uca_editor,
                             ThunarUcaTypes   types)
{
  g_return_if_fail (THUNAR_UCA_IS_EDITOR (uca_editor));

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (uca_editor->directories_button), (types & THUNAR_UCA_TYPE_DIRECTORIES));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (uca_editor->audio_files_button), (types & THUNAR_UCA_TYPE_AUDIO_FILES));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (uca_editor->image_files_button), (types & THUNAR_UCA_TYPE_IMAGE_FILES));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (uca_editor->text_files_button),  (types & THUNAR_UCA_TYPE_TEXT_FILES));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (uca_editor->video_files_button), (types & THUNAR_UCA_TYPE_VIDEO_FILES));
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (uca_editor->other_files_button), (types & THUNAR_UCA_TYPE_OTHER_FILES));
}



/**
 * thunar_uca_editor_load:
 * @uca_editor  : a #ThunarUcaEditor.
 * @uca_model   : a #ThunarUcaModel.
 * @iter        : a #GtkTreeIter.
 *
 * Loads @uca_editor with the data from @uca_model
 * at the specified @iter.
 **/
void
thunar_uca_editor_load (ThunarUcaEditor *uca_editor,
                        ThunarUcaModel  *uca_model,
                        GtkTreeIter     *iter)
{
  ThunarUcaTypes types;
  gchar         *description;
  gchar         *patterns;
  gchar         *command;
  gchar         *icon;
  gchar         *name;

  g_return_if_fail (THUNAR_UCA_IS_EDITOR (uca_editor));
  g_return_if_fail (THUNAR_UCA_IS_MODEL (uca_model));
  g_return_if_fail (iter != NULL);

  /* determine the current values from the model */
  gtk_tree_model_get (GTK_TREE_MODEL (uca_model), iter,
                      THUNAR_UCA_MODEL_COLUMN_DESCRIPTION, &description,
                      THUNAR_UCA_MODEL_COLUMN_PATTERNS, &patterns,
                      THUNAR_UCA_MODEL_COLUMN_COMMAND, &command,
                      THUNAR_UCA_MODEL_COLUMN_TYPES, &types,
                      THUNAR_UCA_MODEL_COLUMN_ICON, &icon,
                      THUNAR_UCA_MODEL_COLUMN_NAME, &name,
                      -1);

  /* setup the new selection */
  thunar_uca_editor_set_types (uca_editor, types);

  /* setup the new icon */
  thunar_uca_editor_set_icon_name (uca_editor, icon);

  /* apply the new values */
  gtk_entry_set_text (GTK_ENTRY (uca_editor->description_entry), (description != NULL) ? description : "");
  gtk_entry_set_text (GTK_ENTRY (uca_editor->patterns_entry), (patterns != NULL) ? patterns : "");
  gtk_entry_set_text (GTK_ENTRY (uca_editor->command_entry), (command != NULL) ? command : "");
  gtk_entry_set_text (GTK_ENTRY (uca_editor->name_entry), (name != NULL) ? name : "");

  /* cleanup */
  g_free (description);
  g_free (patterns);
  g_free (command);
  g_free (icon);
  g_free (name);
}



/**
 * thunar_uca_editor_save:
 * @uca_editor : a #ThunarUcaEditor.
 * @uca_model  : a #ThunarUcaModel.
 * @iter       : a #GtkTreeIter.
 *
 * Stores the current values from the @uca_editor in
 * the @uca_model at the item specified by @iter.
 **/
void
thunar_uca_editor_save (ThunarUcaEditor *uca_editor,
                        ThunarUcaModel  *uca_model,
                        GtkTreeIter     *iter)
{
  g_return_if_fail (THUNAR_UCA_IS_EDITOR (uca_editor));
  g_return_if_fail (THUNAR_UCA_IS_MODEL (uca_model));
  g_return_if_fail (iter != NULL);

  thunar_uca_model_update (uca_model, iter,
                           gtk_entry_get_text (GTK_ENTRY (uca_editor->name_entry)),
                           gtk_entry_get_text (GTK_ENTRY (uca_editor->description_entry)),
                           thunar_uca_editor_get_icon_name (uca_editor),
                           gtk_entry_get_text (GTK_ENTRY (uca_editor->command_entry)),
                           gtk_entry_get_text (GTK_ENTRY (uca_editor->patterns_entry)),
                           thunar_uca_editor_get_types (uca_editor));
}


