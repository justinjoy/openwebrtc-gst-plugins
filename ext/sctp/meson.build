sctp_sources = [
  'gstsctpdec.c',
  'gstsctpenc.c',
  'gstsctpplugin.c',
  'sctpassociation.c',
]

sctp = library('gstsctp',
  sctp_sources,
  c_args : openwebrtc_gst_plugins_args,
  link_args : noseh_link_args,
  include_directories : [configinc, libsinc],
  dependencies : [gstsctp_dep],
  install : true,
  install_dir : plugins_install_dir,
)
