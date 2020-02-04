
def qt_cc_library(name, src, hdr, normal_hdrs=[], deps=None, ui=None,
                  ui_deps=None, **kwargs):
  """Compiles a QT library and generates the MOC for it.

  If a UI file is provided, then it is also compiled with UIC.

  Args:
    name: A name for the rule.
    src: The cpp file to compile.
    hdr: The single header file that the MOC compiles to src.
    normal_hdrs: Headers which are not sources for generated code.
    deps: cc_library dependencies for the library.
    ui: If provided, a UI file to compile with UIC.
    ui_deps: Dependencies for the UI file.
    kwargs: Any additional arguments are passed to the cc_library rule.
  """
  native.genrule(
      name = "%s_moc" % name,
      srcs = [hdr],
      outs = ["moc_%s.cpp" % name],
      cmd =  "moc $(location %s) -o $@ -f'%s'" \
        % (hdr, '%s/%s' % (native.package_name(), hdr)),
  )
  srcs = [src, ":%s_moc" % name]

  hdrs = [hdr] + normal_hdrs

  native.cc_library(
      name = name,
      srcs = srcs,
      hdrs = hdrs,
      deps = deps,
      **kwargs
  )


def qt_resource(name,file_list, **kwargs):

  qrc_filename = "%s.qrc"%(name)
  # qrc_filename = "%s/%s.qrc"%(native.package_name(),name)

  native.genrule(
      name = "%s_gen_qrc" % name,
      srcs=file_list,
      outs =[qrc_filename],
      cmd="$(location //third_party:generate_qrc) $@ $(SRCS)",
      tools=["//third_party:generate_qrc"]
  )
  native.genrule(
      name = "%s_gen_resource" % name,
      srcs=[qrc_filename]+file_list,
      outs= ["rcc_%s.cpp" % name],
      cmd =  "rcc $(GENDIR)/%s/%s -o $@"%(native.package_name(),qrc_filename),
  )
   
  
  srcs =[":rcc_%s.cpp" % name]


  native.cc_library(
      name = name,
      srcs = srcs,
      hdrs = [],
      deps = [],
      **kwargs
  )