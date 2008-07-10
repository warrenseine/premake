/**
 * \file   make_project.c
 * \brief  Makefile project generation functions.
 * \author Copyright (c) 2002-2008 Jason Perkins and the Premake project
 */

#include <assert.h>
#include <stdlib.h>
#include "premake.h"
#include "actions/make/make.h"
#include "actions/make/make_project.h"
#include "base/error.h"
#include "base/path.h"


/**
 * Write the rules to clean up output files on a `make clean`.
 */
int make_project_clean_rules(Session sess, Project prj, Stream strm)
{
	int z = OKAY;
	UNUSED(sess);
	z |= stream_writeline(strm, "clean:");
	z |= stream_writeline(strm, "\t@echo Cleaning %s", project_get_name(prj));
	z |= stream_writeline(strm, "ifeq (posix, $(SHELLTYPE))");
	z |= stream_writeline(strm, "\t@rm -f  $(SYS_OUTFILE)");
	z |= stream_writeline(strm, "\t@rm -rf $(SYS_OBJDIR)");
	z |= stream_writeline(strm, "else");
	z |= stream_writeline(strm, "\t@if exist $(SYS_OUTFILE) del $(SYS_OUTFILE)");
	z |= stream_writeline(strm, "\t@if exist $(SYS_OBJDIR) rmdir /s /q $(SYS_OBJDIR)");
	z |= stream_writeline(strm, "endif");
	z |= stream_writeline(strm, "");
	return OKAY;
}


/**
 * Write the opening conditional for a configuration block.
 */
int make_project_config_conditional(Session sess, Project prj, Stream strm)
{
	const char* cfg_name = project_get_configuration_filter(prj);
	UNUSED(sess);
	return stream_writeline(strm, "ifeq ($(CONFIG),%s)", cfg_name);
}


/**
 * Write the CFLAGS configuration variable.
 */
int make_project_config_cflags(Session sess, Project prj, Stream strm)
{
	UNUSED(sess);
	UNUSED(prj);
	return stream_writeline(strm, "   CFLAGS   += $(CPPFLAGS) $(ARCHFLAGS)");
}


/**
 * Write the CPPFLAGS configuration variable.
 */
int make_project_config_cppflags(Session sess, Project prj, Stream strm)
{
	Strings values;
	int z = OKAY;
	UNUSED(sess);
	
	z |= stream_write(strm, "   CPPFLAGS += -MMD");

	values = project_get_config_values(prj, BlockDefines);
	z |= stream_write_strings(strm, values, "", " -D \"", "\"", "", "", NULL);

	z |= stream_writeline(strm, "");
	return z;
}


/**
 * Write the CXXFLAGS configuration variable.
 */
int make_project_config_cxxflags(Session sess, Project prj, Stream strm)
{
	UNUSED(sess);
	UNUSED(prj);
	return stream_writeline(strm, "   CXXFLAGS += $(CFLAGS)");
}


/**
 * Write the opening conditional for a configuration block.
 */
int make_project_config_end(Session sess, Project prj, Stream strm)
{
	int z;
	UNUSED(sess);
	UNUSED(prj);
	z  = stream_writeline(strm, "endif");
	z |= stream_writeline(strm, "");
	return z;
}


/**
 * Write the LDDEPS configuration variable.
 */
int make_project_config_lddeps(Session sess, Project prj, Stream strm)
{
	UNUSED(sess);
	UNUSED(prj);
	return stream_writeline(strm, "   LDDEPS   :=");
}


/**
 * Write the LDFLAGS configuration variable.
 */
int make_project_config_ldflags(Session sess, Project prj, Stream strm)
{
	UNUSED(sess);
	UNUSED(prj);
	return stream_writeline(strm, "   LDFLAGS  +=");
}


/**
 * Write the OBJDIR configuration variable.
 */
int make_project_config_objdir(Session sess, Project prj, Stream strm)
{
	const char* cfg_name = project_get_configuration_filter(prj);
	UNUSED(sess);
	return stream_writeline(strm, "   OBJDIR   := obj/%s", cfg_name);
}


/**
 * Write the OUTDIR configuration variable.
 */
int make_project_config_outdir(Session sess, Project prj, Stream strm)
{
	UNUSED(sess);
	UNUSED(prj);
	return stream_writeline(strm, "   OUTDIR   := .");
}


/**
 * Write the OUTFILE configuration variable.
 */
int make_project_config_outfile(Session sess, Project prj, Stream strm)
{
	const char* outfile = project_get_outfile(prj);
	UNUSED(sess);
	return stream_writeline(strm, "   OUTFILE  := $(OUTDIR)/%s", outfile);
}


/**
 * Write the RESFLAGS configuration variable.
 */
int make_project_config_resflags(Session sess, Project prj, Stream strm)
{
	UNUSED(sess);
	UNUSED(prj);
	return stream_writeline(strm, "   RESFLAGS +=");
}


/**
 * Create a new output stream for a project , and make it active for subsequent writes.
 */
int make_project_create(Session sess, Project prj, Stream strm)
{
	/* create the makefile */
	const char* filename = make_get_project_makefile(sess, prj);
	strm = stream_create_file(filename);
	if (!strm)
	{
		return !OKAY;
	}

	/* make the stream active for the functions that come after */
	session_set_active_stream(sess, strm);
	return OKAY;
}


/**
 * Include the auto-generated dependencies into the project makefile.
 */
int make_project_include_dependencies(Session sess, Project prj, Stream strm)
{
	UNUSED(sess);
	UNUSED(prj);
	return stream_writeline(strm, "-include $(OBJECTS:%%.o=%%.d)");
}


/**
 * Write the rules to create the output and object directories.
 */
int make_project_mkdir_rules(Session sess, Project prj, Stream strm)
{
	int z = OKAY;
	UNUSED(sess);
	UNUSED(prj);
	z |= stream_writeline(strm, "$(OUTDIR):");
	z |= stream_writeline(strm, "\t@echo Creating $(OUTDIR)");
	z |= stream_writeline(strm, "\t@$(MKDIR) $(SYS_OUTDIR)");
	z |= stream_writeline(strm, "");
	z |= stream_writeline(strm, "$(OBJDIR):");
	z |= stream_writeline(strm, "\t@echo Creating $(OBJDIR)");
	z |= stream_writeline(strm, "\t@$(MKDIR) $(SYS_OBJDIR)");
	z |= stream_writeline(strm, "");
	return z;
}


/**
 * Write the OBJECTS project variable.
 */
int make_project_objects(Session sess, Project prj, Stream strm)
{
	Strings files;
	int i, n, z;
	UNUSED(sess);

	z  = stream_writeline(strm, "OBJECTS := \\");

	files = project_get_files(prj);
	n = strings_size(files);
	for (i = 0; i < n; ++i)
	{
		const char* filename = strings_item(files, i);
		if (path_is_cpp_source(filename))
		{
			const char* obj_name = make_get_obj_filename(filename);
			z |= stream_writeline(strm, "\t%s \\", obj_name);
		}
	}

	z |= stream_writeline(strm, "");
	return z;
}


/**
 * Write the .PHONY rule for a project.
 */
int make_project_phony_rule(Session sess, Project prj, Stream strm)
{
	int z;
	UNUSED(sess);
	UNUSED(prj);
	z  = stream_writeline(strm, ".PHONY: clean");
	z |= stream_writeline(strm, "");
	return z;
}


/**
 * Write the RESOURCES project variable.
 */
int make_project_resources(Session sess, Project prj, Stream strm)
{
	int z;
	UNUSED(sess);
	UNUSED(prj);
	z  = stream_writeline(strm, "RESOURCES := \\");
	z |= stream_writeline(strm, "");
	return z;
}


/**
 * Write the project makefile signature.
 */
int make_project_signature(Session sess, Project prj, Stream strm)
{
	int z;
	UNUSED(sess);
	UNUSED(prj);
	z  = stream_writeline(strm, "# GNU Makefile autogenerated by Premake");
	z |= stream_writeline(strm, "");
	return z;
}


/**
 * Write makefile rules for each source code file.
 */
int make_project_source_rules(Session sess, Project prj, Stream strm)
{
	Strings files;
	int i, n, z = OKAY;
	UNUSED(sess);

	files = project_get_files(prj);
	n = strings_size(files);
	for (i = 0; i < n; ++i)
	{
		const char* filename = strings_item(files, i);
		if (path_is_cpp_source(filename))
		{
			const char* obj_name = make_get_obj_filename(filename);
			z |= stream_writeline(strm, "%s: %s", obj_name, filename);
			z |= stream_writeline(strm, "\t@echo $(notdir $<)");
			z |= stream_writeline(strm, "\t@$(CXX) $(CXXFLAGS) -o $@ -c $<");
			z |= stream_writeline(strm, "");
		}
	}

	return z;
}


/**
 * Write the project output target rule.
 */
int make_project_target(Session sess, Project prj, Stream strm)
{
	int z = OKAY;
	UNUSED(sess);
	z |= stream_writeline(strm, "$(OUTFILE): $(OUTDIR) $(OBJDIR) $(OBJECTS) $(LDDEPS) $(RESOURCES)");
	z |= stream_writeline(strm, "\t@echo Linking %s", project_get_name(prj));
	z |= stream_writeline(strm, "\t@$(CXX) -o $@ $(LDFLAGS) $(ARCHFLAGS) $(OBJECTS) $(RESOURCES)");
	z |= stream_writeline(strm, "");
	return z;
}