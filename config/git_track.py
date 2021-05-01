##############################################################################
#Script para gerar informacao de rastreamento de compilação com  (commit hash e data)
#Este script deve ser chamado antes da compilação, seja pelo buildsystem ou IDE
##############################################################################

import os
import subprocess

print("INICIANDO GIT")
os.chdir("..\\_config_modelo\\")  #sai da pasta sln para a pasta _config_modelo
current_directory = os.getcwd()
os.chdir("..\\..\\src\\")
proc = subprocess.Popen(["git", "rev-parse", "HEAD"],
                        stdout=subprocess.PIPE,
                        shell=True)
(a, err) = proc.communicate()
proc = subprocess.Popen(["git", "show", "-s", "--format=%ci"],
                        stdout=subprocess.PIPE,
                        shell=True)
(b, err) = proc.communicate()
hash_info = a.decode()
hash_info = hash_info.strip("\n")
date_info = b.decode()
date_info = date_info.strip("\n")

print("CRIANDO git_track.h")
print(date_info, hash_info)
p = str(input("\nContinuar execucao.... "))
os.chdir(current_directory)
f = open("git_track.h", "w")
f.write("#ifndef __GIT_TRACK_H__\n")
f.write("#define __GIT_TRACK_H__\n")
f.write("//\n//ARQUIVO GERADO AUTOMATICAMENTE por git_track.py\n//\n")
f.write("#define GIT_TRACK_DATE " + "\"" + date_info + "\"\n")
f.write("#define GIT_TRACK_HASH " + "\"" + hash_info.lower() + "\"\n")
f.write("#endif\n")
f.close()
