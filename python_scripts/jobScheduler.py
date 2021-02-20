import os, sys
import json
import psutil
import subprocess
import time


#%%
# ler arquivo JSON
def lerJson(nomeDoArquivo: str):
    try:
        nomeDoArquivo = nomeDoArquivo + ".json"
        arquivo = open(nomeDoArquivo, 'r')
        dados = json.load(arquivo)
        return dados
    except:
        print('###\nErro na leitura do arquivo (%s)\n' % (nomeDoArquivo))
        os.system("pause")
        sys.exit(2)


#%%
# salvar arquivo JSON
def salvarJson(nomeDoArquivo: str, data):

    #cria backup do arquivo anterior
    try:
        os.rename(nomeDoArquivo + ".json", nomeDoArquivo + "-backup.json")
        pass
    except:
        os.remove(nomeDoArquivo + "-backup.json")
        os.rename(nomeDoArquivo + ".json", nomeDoArquivo + "-backup.json")
        pass

    if len(data["nameJobs"]) != 0:
        output = "hasKeys"
        pass
    else:
        output = "hasNoKeys"
        data = ""
        pass
    try:
        nomeDoArquivo = nomeDoArquivo + ".json"
        arquivo = open(nomeDoArquivo, 'w')
        json.dump(data, arquivo, indent="    ")
        return output
    except:
        print('###\nErro ao salvar o arquivo (%s)\n' % (nomeDoArquivo))
        os.system("pause")
        sys.exit(2)


def findProcessIdByName(processName: str):
    '''
    Get a list of all the PIDs of a all the running process whose name contains
    the given string processName
    '''
    listOfProcessObjects = []
    #Iterate over the all the running process
    for proc in psutil.process_iter():
        try:
            pinfo = proc.as_dict(attrs=['pid', 'name', 'create_time'])
            # Check if process name contains the given name string.
            if processName.lower() in pinfo['name'].lower():
                listOfProcessObjects.append(pinfo)
        except (psutil.NoSuchProcess, psutil.AccessDenied,
                psutil.ZombieProcess):
            pass
    return listOfProcessObjects


def main_func(ler=None):
    user_input = str(
        input("\nNome do arquivo (ExemploArquivoJobSchedulerJSON):  "))
    #user_input = "ExemploArquivoJobSchedulerJSON"

    if user_input == '':
        user_input = "ExemploArquivoJobSchedulerJSON"
        pass

    user_max_proc = str(input("\nNumero maximo de processos a executar:  "))

    #ler arquivo json pela primeira vez
    arquivo_json = lerJson(user_input)
    #extrai primeiro trabalho
    nameJob = arquivo_json["nameJobs"].pop(0)
    worker = arquivo_json.pop(nameJob)
    #salva arquivo
    status = salvarJson(user_input, arquivo_json)

    #diretorio em que se encontra o json file
    parentFolder = os.getcwd()

    keyPass = True
    newWorker = False
    while keyPass == True:
        #muda de diretorio raiz
        print("\n\n")
        os.chdir(worker["cwd"])
        print(os.getcwd())

        if worker["mode"] == 0:
            #executa na mesma janela
            #executa .bat
            try:
                comandoBat = worker["bat"] + " " + worker["args"]
            except:
                comandoBat = worker["bat"]
                pass
            os.system(comandoBat)
            newWorker = True
            pass
        elif worker["mode"] == 1:
            #executa multiplos arquivos em janelas diferentes
            listOfProcessIds = findProcessIdByName(worker["process"])
            if len(listOfProcessIds) < user_max_proc:
                try:
                    comandoBat = "start " + worker["bat"] + " " + worker["args"]
                except:
                    comandoBat = "start " + worker["bat"]
                    pass
                subprocess.Popen(["cmd", "/c", comandoBat])
                newWorker = True
                pass
            else:
                time.sleep(60)
                pass
            pass

        if newWorker == True:
            if status == "hasNoKeys":
                keyPass = False
                pass
            else:
                os.chdir(parentFolder)
                arquivo_json = lerJson(user_input)
                nameJob = arquivo_json["nameJobs"].pop(0)
                worker = arquivo_json.pop(nameJob)
                status = salvarJson(user_input, arquivo_json)
                newWorker = False
                pass
        pass


# %%
if __name__ == "__main__":
    main_func()