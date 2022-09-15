#
# ================= Use  custom_option1=================
#
Import("env")
# env.Replace(PROGNAME=env.GetProjectOption("custom_option1"))
env.Replace(PROGNAME="%s_%s" % (env.GetProjectOption("custom_option1"), env.GetProjectOption("custom_prog_version")));

#
# ================= rename from firmware.bin to projectname.bin =================
#
# Import("env")
# import os

# project_name = os.path.basename(os.path.dirname(env["PROJECT_CONFIG"]))

#env.Replace(PROGNAME="%s" % project_name)

#
# If you want to rename to project name + env name then use it.
#
#env.Replace(PROGNAME="%s_%s" % (env.GetProjectOption("custom_option1"), env["PIOENV"]));