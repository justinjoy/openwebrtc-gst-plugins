gstsctp_sources = ['sctpsendmeta.c', 'sctpreceivemeta.c']
gstsctp_headers = ['sctpsendmeta.h', 'sctpreceivemeta.h']

install_headers(gstsctp_headers, subdir : 'gstreamer-1.0/gst/sctp')

gstsctp = library('gstsctp-' + api_version,
  gstsctp_sources,
  c_args : openwebrtc_gst_plugins_args,
  include_directories : [configinc, libsinc],
  version : libversion,
  soversion : soversion,
  install : true,
  dependencies : [gstbase_dep, libusrsctp_dep],
)

gstsctp_dep = declare_dependency(link_with : gstsctp,
  include_directories : [libsinc],
  dependencies : [gstbase_dep, libusrsctp_dep])
