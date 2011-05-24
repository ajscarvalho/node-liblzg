srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
	opt.tool_options("compiler_cxx")
	opt.tool_options("compiler_cc")

def configure(conf):
	conf.check_tool("compiler_cxx")
	conf.check_tool("compiler_cc")
	conf.check_tool("node_addon")
	conf.env.append_value('CPPFLAGS', ['-Du_int32_t=uint32_t', '-Du_int16_t=uint16_t'])
	conf.env.append_value('CCFLAGS', ['-O3'])

def build(bld):
	liblzg = bld.new_task_gen("cc", "shlib")
	liblzg.source = """
		src/checksum.c
		src/decode.c
		src/encode.c
		src/version.c
	"""
	liblzg.includes = "./"
	liblzg.name = "liblzg"
	liblzg.target = "liblzg"

	obj = bld.new_task_gen("cxx", "shlib", "node_addon")
	obj.target = "liblzg"
	obj.source = "liblzg_js.cc"
	obj.includes = "liblzg"
	obj.add_objects = "liblzg"
