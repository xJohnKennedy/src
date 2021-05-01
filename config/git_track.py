import os
import subprocess
##############################################################################
#Script para gerar informacao de rastreamento de compilação com  (commit hash e data)
##############################################################################
print "INICIANDO GIT"
proc = subprocess.Popen(["git", "rev-parse", "HEAD"],
                        stdout=subprocess.PIPE,
                        shell=True)
#proc = subprocess.Popen(["git", "show", "-s", "--format=%H"], stdout=subprocess.PIPE, shell=True)
(a, err) = proc.communicate()
proc = subprocess.Popen(["git", "show", "-s", "--format=%ci"],
                        stdout=subprocess.PIPE,
                        shell=True)
(b, err) = proc.communicate()
hash_info = a.strip("\n\r")
date_info = b.strip("\n\r")

print "CRIANDO git_track.h"
print b, a
f = open("git_track.h", "w")
f.write("#ifndef __GIT_TRACK_H__\n")
f.write("#define __GIT_TRACK_H__\n")
f.write("//\n//ARQUIVO GERADO AUTOMATICAMENTE\n//\n")
f.write("#define GIT_TRACK_DATE " + "\"" + date_info + "\"\n")
f.write("#define GIT_TRACK_HASH " + "\"" + hash_info.upper() + "\"\n")
f.write("#endif\n")
