#!/usr/bin/python
import sys
import time
import string
import commands
import thread
import os

head_file_path = "version_git.h"

Template = """//this file is auto create by carlos do not edit
#ifndef VERSION_GIT_H
#define VERSION_GIT_H
#define VERSION_GIT \"V.1%05d\"
#define VERSION_GIT_HASH 0x%s
#endif
"""


def run_sys_command(command):
  if os.system(command) != 0:
    print command.strip()," error !!!"
    sys.exit()
  else:
    print command.strip()," success !!!"

commands_output = commands.getoutput("git rev-list HEAD")
lines = commands_output.split("\n");
lines_count = 0
for elements in lines:
  lines_count = lines_count+1
current_hash = commands.getoutput("git rev-list --max-count=1 HEAD")
temp_head = open(head_file_path,"w")
temp_head.write(Template%(lines_count,current_hash[0:8]))
temp_head.close()

