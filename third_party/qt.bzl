
def qt_cc_library(name, src, hdr, deps=None, **kwargs):
  """Compiles a QT library and generates the MOC for it.

  Args:
    name: A name for the rule.
    src: The cpp file to compile.
    hdr: The single header file that the MOC compiles to src.
    deps: cc_library dependencies for the library.
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
  hdrs = [hdr] 
  if 'srcs' in kwargs:
    srcs +=kwargs['srcs']
    kwargs.pop('srcs')
  if 'hdrs' in kwargs:
    hdrs +=kwargs['hdrs']
    kwargs.pop('hdrs')


  

  native.cc_library(
      name = name,
      srcs = srcs,
      hdrs = hdrs,
      deps = deps,
      **kwargs
  )


def qt_resource(name,file_list, **kwargs):

  qrc_filename = "%s.qrc"%(name)
  native.genrule(
      name = "%s_gen_resource" % name,
      srcs=file_list ,
      outs= ["rcc_%s.cpp" % name],
      cmd =  "$(location //third_party:generate_qrc) tmp.qrc %s $(SRCS) && rcc tmp.qrc -name %s -o $@ "%(name,name),
      tools=["//third_party:generate_qrc"]
  )
  srcs =[":rcc_%s.cpp" % name]

  native.cc_library(
      name = name,
      srcs = srcs,
      hdrs = [],
      deps = [],
      **kwargs
  )