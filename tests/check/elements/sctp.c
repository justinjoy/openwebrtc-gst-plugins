/* GStreamer
 *
 * unit test for sctp elements
 * Copyright (C) 2017 Collabora Ltd
 *   @author: Justin Kim <justin.kim@collabora.com>
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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_VALGRIND
# include <valgrind/valgrind.h>
#endif

#include <gst/gst.h>
#include <gst/check/gstcheck.h>

GST_START_TEST (test_sctp_create_unref)
{
  GstElement *e;

  e = gst_element_factory_make ("sctpenc", NULL);
  fail_unless (e != NULL);
  gst_element_set_state (e, GST_STATE_NULL);
  gst_object_unref (e);

  e = gst_element_factory_make ("sctpdec", NULL);
  fail_unless (e != NULL);
  gst_element_set_state (e, GST_STATE_NULL);
  gst_object_unref (e);
}

GST_END_TEST;

GST_START_TEST (test_sctp_play)
{
  GstElement *sctpenc, *sctpdec;
  GstBus *bus;
  GstMessage *msg;
  guint association_id = 0;
  GstElement *pipeline = 
      gst_parse_launch
      ("sctpenc sctp-association-id=1 name=sctpenc ! queue ! sctpdec sctp-association-id=1 name=sctpdec", NULL);

  fail_unless (gst_element_set_state (pipeline,
          GST_STATE_PLAYING) != GST_STATE_CHANGE_FAILURE);

  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  msg =
      gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_ERROR | GST_MESSAGE_STATE_CHANGED);
  fail_unless (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_STATE_CHANGED);
  gst_message_unref (msg);

  fail_unless (GST_STATE (pipeline) == GST_STATE_PLAYING);

  sctpenc = gst_bin_get_by_name (GST_BIN (pipeline), "sctpenc");
  sctpdec = gst_bin_get_by_name (GST_BIN (pipeline), "sctpdec");

  g_object_get (sctpenc, "sctp-association-id", &association_id, NULL);
  fail_unless (association_id == 1); 

  association_id = 0;
  g_object_get (sctpdec, "sctp-association-id", &association_id, NULL);
  fail_unless (association_id == 1); 

  gst_element_set_state (pipeline, GST_STATE_NULL);

  gst_object_unref (bus);

  gst_object_unref (sctpdec);
  gst_object_unref (sctpenc);

  gst_object_unref (pipeline);
}

GST_END_TEST;

GST_START_TEST (test_sctp_dual_play)
{
  GstBus *bus;
  GstMessage *msg;
  GstElement *q1, *q2;
  GstElement *sctpenc1, *sctpenc2;
  GstElement *sctpdec1, *sctpdec2;
  GstElement *pipeline = gst_pipeline_new ("dual-play");

  sctpenc1 = gst_element_factory_make ("sctpenc", NULL);
  sctpenc2 = gst_element_factory_make ("sctpenc", NULL);
  q1 = gst_element_factory_make ("queue", NULL);

  sctpdec1 = gst_element_factory_make ("sctpdec", NULL);
  sctpdec2 = gst_element_factory_make ("sctpdec", NULL);
  q2 = gst_element_factory_make ("queue", NULL);
 
  g_object_set (sctpenc1, "sctp-association-id", 1, NULL);
  g_object_set (sctpdec1, "sctp-association-id", 1, NULL);

  g_object_set (sctpenc2, "sctp-association-id", 2, NULL);
  g_object_set (sctpdec2, "sctp-association-id", 2, NULL);

  gst_bin_add_many (GST_BIN (pipeline),
      sctpenc1, q1, sctpdec1,
      sctpenc2, q2, sctpdec2,
      NULL);

  gst_element_link_many (sctpenc1, q1, sctpdec1, NULL);
  gst_element_link_many (sctpenc2, q2, sctpdec2, NULL);

  fail_unless (gst_element_set_state (pipeline,
          GST_STATE_PLAYING) != GST_STATE_CHANGE_FAILURE);

  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  msg =
      gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
      GST_MESSAGE_ERROR | GST_MESSAGE_STATE_CHANGED);
  fail_unless (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_STATE_CHANGED);
  gst_message_unref (msg);

  fail_unless (GST_STATE (pipeline) == GST_STATE_PLAYING);

  gst_element_set_state (pipeline, GST_STATE_NULL);

  gst_object_unref (bus);
  gst_object_unref (pipeline);
}

GST_END_TEST;

static Suite *
sctp_suite (void)
{
  Suite *s = suite_create ("sctp");
  TCase *tc = tcase_create ("sctp");

  suite_add_tcase (s, tc);
  tcase_add_test (tc, test_sctp_create_unref);
  tcase_add_test (tc, test_sctp_play);
  tcase_add_test (tc, test_sctp_dual_play);

  return s;
}

GST_CHECK_MAIN (sctp);
